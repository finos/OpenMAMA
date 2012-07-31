/* $Id: syncresponder.c,v 1.9.12.1.4.5 2011/10/02 19:02:17 ianbell Exp $
 *
 * OpenMAMA: The open middleware agnostic messaging API
 * Copyright (C) 2011 NYSE Technologies, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#include "mama/mama.h"
#include "mama/clientmanage.h"
#include "clientmanageresponder.h"
#include "list.h"
#include "syncresponder.h"
#include "transportimpl.h"

/**
 *
 * The synCommand responds to a request for a subject list from the FH.
 * It sends a list of all matching topics.
 */

/**
 * We pass this function to the CmResponder so it knows how to clean up
 */
static void syncCommand_dtor (void *handle);

static void MAMACALLTYPE
timerCB (mamaTimer timer, void *closure);

typedef struct
{
    mamaMsg          mMsg;
    mamaTransport    mTransport;
    /* The index of the sub transport bridge that the sync command will use. */
	int				 mTransportIndex;
    mamaPublisher    mPublisher;
    mamaTimer        mTimer;
    int              mCurIndex;
    mama_u16_t       mTopicsPerMsg;
    const char**     mTopics;
    int              mTopicsLen;
    mama_i32_t*      mTypes;
    double           mResponseInterval;
    mamaCommandEndCB mEndCB;
    void*            mClosure;
    mamaCommand*     mCommand;
    mamaQueue        mDefaultQueue;
} syncCommand;

static mama_status setup (syncCommand* impl);
static int sendNextMessage (syncCommand *impl);

mama_status
mamaSyncCommand_create (mamaCommand*        command,
                        mamaMsg             msg,
                        mamaTransport       tport,
						int					transportIndex,
                        mamaPublisher       publisher,
                        mamaCommandEndCB    endCB,
                        void*               closure)
{
    syncCommand* impl = (syncCommand*) calloc (sizeof(syncCommand), 1);

    /* This is the command structure passed in by the CmResponder */
    impl->mCommand = command;

    /* CmResponder can invoke dtor(handle) to clean up. */
    command->mHandle = impl;
    command->mDtor   = syncCommand_dtor;

    /* We use the message later on to determine how to format the response. */
    impl->mMsg = msg;

    mamaMsg_detach (msg);

    impl->mTransport = tport;
	impl->mTransportIndex = transportIndex;
	impl->mPublisher = publisher; /* To respond to inbox */
    impl->mEndCB = endCB; /* To tell CmResponder when we finish. */
    impl->mClosure = closure;
    impl->mDefaultQueue =   NULL;

    if (MAMA_STATUS_OK!=(mama_getDefaultEventQueue (
            (mamaBridge)mamaTransportImpl_getBridgeImpl (tport),
            &impl->mDefaultQueue)))
    {
        mama_log (MAMA_LOG_LEVEL_WARN, "mamaSyncCommand_create(): Could not"
                                         " get default event queue");
    }

    /* Get the information from the message to prepare for the response */
    return setup (impl);
}

mama_status
setup (syncCommand* impl)
{
    mama_status rval = MAMA_STATUS_OK;
    const char* source;
    int intervals = 0;
    double duration = 0.0;

    /* This field is set if the FH only wants topics with a specific source */
    rval = mamaMsg_getString (impl->mMsg, NULL, MAMA_SYNC_SOURCE_ID, &source);

    if (rval != MAMA_STATUS_OK) /* All sources */
    {
        source = NULL;
    }

    /* We need all the subscribed topics and subscription types (GROUP, etc).
     */
    rval = mamaTransportImpl_getTopicsAndTypesForSource (impl->mTransport,
														 impl->mTransportIndex,
                                                         source,
                                                         &impl->mTopics,
                                                         &impl->mTypes,
                                                         &impl->mTopicsLen);

    /* How many topics to send per response message. */
    mamaMsg_getU16 (impl->mMsg, NULL, MAMA_SYNC_TOPICS_PER_MSG_ID,
                    &impl->mTopicsPerMsg);

    /* Interval over which to send the response messages. */
    mamaMsg_getF64 (impl->mMsg, NULL, MAMA_SYNC_RESPONSE_DURATION_ID,
                    &duration);

    mama_log (MAMA_LOG_LEVEL_FINE,
              "Responding to sync request with %d total topics. "
              "topics per message: %d. duration: %f", impl->mTopicsLen,
              impl->mTopicsPerMsg, duration);

    /* Number of messages that we need to send. */
    intervals = impl->mTopicsLen / impl->mTopicsPerMsg;

    if (intervals > 0)
    {
        /* * 2.0 so average is correct */
        impl->mResponseInterval = 2.0 * duration / intervals;
    }
    else
    {
        impl->mResponseInterval = 0.0;
    }

    return MAMA_STATUS_OK;
}

/**
 * Invoked by CmManager once it finsishes its clean up */
void syncCommand_dtor(void *handle)
{
    syncCommand *impl = (syncCommand*)handle;

    mama_log (MAMA_LOG_LEVEL_FINE, "Destroying sync command.");

    mamaMsg_destroy (impl->mMsg);

    if (impl->mTimer)
    {
        mamaTimer_destroy (impl->mTimer);
        impl->mTimer = NULL;
    }

    free ((void*)impl->mTopics);
    free (impl->mTypes);

    free (impl);
}

mama_status
mamaSyncCommand_run (mamaCommand *command)
{
    syncCommand *impl = (syncCommand*)command->mHandle;
    double delay = 0.0;

    if (impl->mTopicsLen == 0)
    {
        mama_log (MAMA_LOG_LEVEL_FINE, "Not running sync command."
                  " No matching subscriptions");
        impl->mEndCB (impl->mCommand, impl->mClosure);
        return MAMA_STATUS_OK;
    }


    /* We wait a random interval up to this long before responding */
    mamaMsg_getF64 (impl->mMsg, "", MAMA_SYNC_RESPONSE_DELAY_ID, &delay);

    mama_log (MAMA_LOG_LEVEL_FINE, "Running sync command. Delay: %f", delay);

    if (delay != 0.0)
    {
        double randDelay = delay + (rand()/(RAND_MAX + 1.0));
        mamaTimer_create (&impl->mTimer,
                          impl->mDefaultQueue,
                          timerCB,
                          randDelay,
                          impl);
    }
    else
    {
        timerCB (NULL, impl);
    }

    return MAMA_STATUS_OK;
}

static void MAMACALLTYPE
timerCB (mamaTimer timer, void *closure)
{
    syncCommand *impl = (syncCommand*)closure;

    if (impl->mResponseInterval == 0.0)
    {
        /* Send them all at once */
        while (sendNextMessage (impl));
        mamaTimer_destroy (impl->mTimer);
        impl->mTimer = NULL;
        impl->mEndCB (impl->mCommand, impl->mClosure);
    }
    else if (sendNextMessage (impl))
    {
        /* There are more */
        if (impl->mTimer == NULL) /* delay was 0 */
        {
            double interval = impl->mResponseInterval*(rand()/(RAND_MAX + 1.0));
            mamaTimer_create (&impl->mTimer,
                              impl->mDefaultQueue,
                              timerCB,
                              interval,
                              impl);
        }
        else
        {
            double interval = impl->mResponseInterval*(rand()/(RAND_MAX + 1.0));
            /* no resetInterval in C */
            mamaTimer_destroy (impl->mTimer);
            mamaTimer_create (&impl->mTimer,
                              impl->mDefaultQueue,
                              timerCB,
                              interval,
                              impl);
        }
    }
    else
    {
        mamaTimer_destroy (impl->mTimer);
        impl->mTimer = NULL;
        /* We have sent all the messages. */
        impl->mEndCB (impl->mCommand, impl->mClosure);
    }
}

int
sendNextMessage (syncCommand *impl)
{
    int topicsLeft   = impl->mTopicsLen - impl->mCurIndex;
    int topicsToSend = topicsLeft > impl->mTopicsPerMsg
        ? impl->mTopicsPerMsg
        : topicsLeft;

    mamaMsg reply;

    mama_log (MAMA_LOG_LEVEL_FINE, "Sending sync message with %d topics.", topicsToSend);

    mamaMsg_create (&reply);
    mamaMsg_addVectorString (reply, NULL, MAMA_SYNC_TOPICS_ID,
                             impl->mTopics + impl->mCurIndex, topicsToSend);

    mamaMsg_addVectorI32 (reply, NULL, MAMA_SYNC_TYPES_ID,
                             impl->mTypes + impl->mCurIndex, topicsToSend);

    mamaPublisher_sendReplyToInbox (impl->mPublisher, impl->mMsg, reply);
    mamaMsg_destroy (reply);

    impl->mCurIndex += topicsToSend;
    return topicsLeft - topicsToSend;
}
