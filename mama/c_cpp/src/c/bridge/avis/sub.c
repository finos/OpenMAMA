/* $Id: sub.c,v 1.1.2.10 2011/10/02 19:02:18 ianbell Exp $
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

#include <string.h>
#include <mama/mama.h>
#include <subscriptionimpl.h>
#include <transportimpl.h>
#include <msgimpl.h>
#include <queueimpl.h>
#include <wombat/queue.h>

#include "avisbridgefunctions.h"
#include "transportbridge.h"
#include "avisdefs.h"
#include "subinitial.h"

typedef struct avisSubscription
{
    Subscription*     mAvisSubscription;
    Elvin*            mAvis;

    mamaMsgCallbacks  mMamaCallback;
    mamaSubscription  mMamaSubscription;
    mamaQueue         mQueue;
    mamaTransport     mTransport;

    const char*       mSource;
    const char*       mSymbol;
    char              mSubject[256];
    void*             mClosure;

    int               mIsNotMuted;
    int               mIsValid;

} avisSubscription;

#define avisSub(subscriber) ((avisSubscription*)(subscriber))
#define CHECK_SUBSCRIBER(subscriber) \
        do {  \
           if (avisSub(subscriber) == 0) return MAMA_STATUS_NULL_ARG; \
           if (avisSub(subscriber)->mAvis == 0) return MAMA_STATUS_INVALID_ARG; \
         } while(0)


static void avis_callback(Subscription *subscription, Attributes *attributes, bool secure, void *user_data);

const char* makeAvisSubject(const char* subject);

const char* makeAvisSubject(const char* subject)
{
   static char subjectBuf[4096];
   snprintf(subjectBuf, 4096, "(%s == \"%s\")", SUBJECT_FIELD_NAME, subject);
   return subjectBuf;
}


static void MAMACALLTYPE
destroy_callback(void* subscriber, void* closure)
{
    // cant do anything without a subscriber
    if (!avisSub(subscriber)) {
        mama_log (MAMA_LOG_LEVEL_ERROR, "avis_callback(): called with NULL subscriber!");
        return;
    }

    (*avisSub(subscriber)->mMamaCallback.onDestroy)(avisSub(subscriber)->mMamaSubscription, avisSub(subscriber)->mClosure);

    free(avisSub(subscriber));
}

/**
 * Called when message removed from queue by dispatch thread
 *
 * @param data The Avis Attributes* clone (must be freed)
 * @param closure The subscriber
 */
static void MAMACALLTYPE
avis_queue_callback (void* data, void* closure)
{
    mama_status status;
    mamaMsg tmpMsg;
    msgBridge bridgeMsg;

    /* cant do anything without a subscriber */
    if (!avisSub(closure)) {
        mama_log (MAMA_LOG_LEVEL_ERROR, "avis_callback(): called with NULL subscriber!");
        return;
    }

    /*Make sure that the subscription is processing messages*/
    if ((!avisSub(closure)->mIsNotMuted) || (!avisSub(closure)->mIsValid)) return;


    /*This is the reuseable message stored on the associated MamaQueue*/
    tmpMsg = mamaQueueImpl_getMsg(avisSub(closure)->mQueue);
    if (!tmpMsg)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "avis_callback(): "
                  "Could not get cached mamaMsg from event queue.");
        return;
    }

    /*Get the bridge message from the mamaMsg*/
    if (MAMA_STATUS_OK!=(status=mamaMsgImpl_getBridgeMsg (tmpMsg,
                    &bridgeMsg)))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "avis_callback(): "
                  "Could not get bridge message from cached"
                  " queue mamaMsg [%d]", status);
        return;
    }

    /*Set the buffer and the reply handle on the bridge message structure*/
    avisBridgeMamaMsgImpl_setAttributesAndSecure (bridgeMsg, data, 0);

    if (MAMA_STATUS_OK!=(status=mamaMsgImpl_setMsgBuffer (tmpMsg,
                                data,
                                0, MAMA_PAYLOAD_AVIS)))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "avis_callback(): mamaMsgImpl_setMsgBuffer() failed. [%d]",
                  status);
        return;
    }

    /*Process the message as normal*/
    if (MAMA_STATUS_OK != (status=mamaSubscription_processMsg
                (avisSub(closure)->mMamaSubscription, tmpMsg)))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "avis_callback(): "
                  "mamaSubscription_processMsg() failed. [%d]",
                  status);
    }

    attributes_free ((Attributes*)data);
    free ((Attributes*)data);
}

static void
avis_callback(
    Subscription*   subcription,
    Attributes*     attributes,
    bool            secure,
    void*           subscriber)
{
    wombatQueue queue = NULL;

    /* cant do anything without a subscriber */
    if (!avisSub(subscriber)) {
        mama_log (MAMA_LOG_LEVEL_ERROR, "avis_callback(): called with NULL subscriber!");
        return;
    }

    /*Make sure that the subscription is processing messages*/
    if ((!avisSub(subscriber)->mIsNotMuted) || (!avisSub(subscriber)->mIsValid)) return;

    mamaQueue_getNativeHandle(avisSub(subscriber)->mQueue, &queue);
    if (!queue)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "avis_callback(): "
                  "Could not get event queue.");
        return;
    }

    wombatQueue_enqueue (queue, avis_queue_callback,
        attributes_clone(attributes), subscriber);

    return;
}


mama_status
avisBridgeMamaSubscription_create (subscriptionBridge* subscriber,
                                    const char*         source,
                                    const char*         symbol,
                                    mamaTransport       transport,
                                    mamaQueue           queue,
                                    mamaMsgCallbacks    callback,
                                    mamaSubscription    subscription,
                                    void*               closure)
{
    avisSubscription* impl = NULL;
    if (!subscriber || !subscription || !transport )
        return MAMA_STATUS_NULL_ARG;

    impl = (avisSubscription*)calloc (1, sizeof(avisSubscription));
    if (impl == NULL)
       return MAMA_STATUS_NOMEM;

    impl->mAvis = getAvis(transport);
    if (!impl->mAvis)
       return MAMA_STATUS_INVALID_ARG;

    if (source != NULL && source[0])
    {
        impl->mSource = source;
    }
    else
    {
        impl->mSource = "";
    }

    if (symbol != NULL && symbol[0])
    {
        impl->mSymbol = symbol;
        snprintf (impl->mSubject, sizeof(impl->mSubject), "%s.%s",
                  impl->mSource, impl->mSymbol);
    }
    else
    {
        impl->mSymbol = "";
        snprintf (impl->mSubject, sizeof(impl->mSubject), "%s",
                  impl->mSource );
    }

    impl->mMamaCallback       = callback;
    impl->mMamaSubscription   = subscription;
    impl->mQueue              = queue;
    impl->mTransport          = transport;
    impl->mClosure            = closure;
    impl->mIsNotMuted         = 1;
    impl->mIsValid            = 1;

    impl->mAvisSubscription = elvin_subscribe(impl->mAvis, makeAvisSubject(impl->mSubject));
    if (impl->mAvisSubscription == NULL)
       return MAMA_STATUS_PLATFORM;

    elvin_subscription_add_listener(impl->mAvisSubscription, avis_callback, impl);
    mama_log (MAMA_LOG_LEVEL_FINER,
              "Made Avis subscription to: %s", impl->mSubject);

    *subscriber =  (subscriptionBridge) impl;
    return MAMA_STATUS_OK;
}

mama_status
avisBridgeMamaSubscription_createWildCard (
                                subscriptionBridge* subscriber,
                                const char*         source,
                                const char*         symbol,
                                mamaTransport       transport,
                                mamaQueue           queue,
                                mamaMsgCallbacks    callback,
                                mamaSubscription    subscription,
                                void*               closure )
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisBridgeMamaSubscription_destroy (subscriptionBridge subscriber)
{
    mama_status status = MAMA_STATUS_OK;
    wombatQueue   queue = NULL;
    CHECK_SUBSCRIBER(subscriber);

    elvin_subscription_remove_listener(avisSub(subscriber)->mAvisSubscription, avis_callback);

    if (!elvin_unsubscribe(avisSub(subscriber)->mAvis, avisSub(subscriber)->mAvisSubscription)) {
        // NOTE: elvin_unsubscribe sometimes returns failure for no apparent reason, so dont log errors here:
        // 2011-09-02 11:59:10: avis error code=2, error msg=Illegal frame size: 61
        //log_avis_error(MAMA_LOG_LEVEL_ERROR, avisSub(subscriber)->mAvis);
        //status = MAMA_STATUS_PLATFORM;
    }

    free(avisSub(subscriber)->mAvisSubscription);

    mamaQueue_getNativeHandle(avisSub(subscriber)->mQueue, &queue);
    if (!queue)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "avis_callback(): "
                  "Could not get event queue.");
        return MAMA_STATUS_PLATFORM;
    }

    wombatQueue_enqueue (queue, destroy_callback,
    		(void*)subscriber, NULL);

    return status;
}


mama_status
avisBridgeMamaSubscription_mute (subscriptionBridge subscriber)
{
    CHECK_SUBSCRIBER(subscriber);
    avisSub(subscriber)->mIsNotMuted = 0;
    return MAMA_STATUS_OK;
}


int
avisBridgeMamaSubscription_isValid (subscriptionBridge subscriber)
{
    if (!subscriber) return 0;
    return avisSub(subscriber)->mIsValid;
}

mama_status
avisBridgeMamaSubscription_getPlatformError (subscriptionBridge subscriber,
                                              void**             error)
{
    CHECK_SUBSCRIBER(subscriber);
    *error = &(avisSub(subscriber)->mAvis->error);
    return MAMA_STATUS_OK;
}

mama_status
avisBridgeMamaSubscription_setTopicClosure (subscriptionBridge subscriber,
                                             void* closure)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisBridgeMamaSubscription_muteCurrentTopic (subscriptionBridge subscriber)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

int
avisBridgeMamaSubscription_hasWildcards (subscriptionBridge subscriber)
{
    return 0;
}

int
avisBridgeMamaSubscription_isTportDisconnected (subscriptionBridge subscriber)
{
    CHECK_SUBSCRIBER(subscriber);
    return elvin_is_open(avisSub(subscriber)->mAvis) ? 0 : 1;
}
