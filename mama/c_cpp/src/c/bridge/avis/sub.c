/* $Id$
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
#include <wombat/wSemaphore.h>

#include "avisbridgefunctions.h"
#include "transportbridge.h"
#include "avisdefs.h"
#include "subinitial.h"
#include "sub.h"

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
    char*             mSubject;
    void*             mClosure;

    int               mIsNotMuted;
    int               mIsValid;

    wsem_t            mCreateDestroySem;

} avisSubscription;

typedef struct avisCallbackContext
{
    void* attributes;
    void* subscriber;
} avisCallbackContext;

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


static
void subscribeAvis (Elvin* avis,
                    void*  closure)
{
    avisSubscription* impl = (avisSubscription*) closure;

    impl->mAvisSubscription = elvin_subscribe(impl->mAvis, makeAvisSubject(impl->mSubject));

    if (impl->mAvisSubscription == NULL)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "subscribeAvis(): "
                                        "Failed to subscribe to: %s", impl->mSubject);

        wsem_post (&impl->mCreateDestroySem);
    
        return;
    }

    elvin_subscription_add_listener (impl->mAvisSubscription, avis_callback, impl);

    mama_log (MAMA_LOG_LEVEL_FINER,
              "Made Avis subscription to: %s", impl->mSubject);

    wsem_post (&impl->mCreateDestroySem);
}

static
void unsubscribeAvis (Elvin* avis,
                      void*  closure)
{
    Subscription* sub = (Subscription*) closure;

    elvin_subscription_remove_listener (sub, avis_callback);

    if (!elvin_unsubscribe(avis, sub))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "unsubscribeAvis(): "
                                        "Failed to unsubscribe to: %s",
                                        sub->subscription_expr);
    }

    free (sub);
}

/**
 * Called when message removed from queue by dispatch thread
 *
 * @param data The Avis Attributes* clone (must be freed)
 * @param closure The subscriber
 */
static void MAMACALLTYPE
avis_queue_callback (mamaQueue queue,
                     void*     closure)
{
    mama_status status     = MAMA_STATUS_OK;
    mamaMsg     tmpMsg     = NULL;
    msgBridge   bridgeMsg  = NULL;
    const void* buf        = NULL;
    mama_size_t bufSize    = 0;

    avisCallbackContext* ctx = (avisCallbackContext*) closure;

    void* data       = ctx->attributes;
    void* subscriber = ctx->subscriber;

    /* cant do anything without a subscriber */
    if (!subscriber)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "avis_callback(): called with NULL subscriber!");

        attributes_destroy (data);
        free (ctx);
        return;
    }

    /*Make sure that the subscription is processing messages*/
    if ((!avisSub(subscriber)->mIsNotMuted) || 
        (!avisSub(subscriber)->mIsValid))
    {
        attributes_destroy (data);
        free (ctx);
        return;
    }

    /*This is the reuseable message stored on the associated MamaQueue*/
    tmpMsg = mamaQueueImpl_getMsg(avisSub(subscriber)->mQueue);
    if (!tmpMsg)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "avis_callback(): "
                  "Could not get cached mamaMsg from event queue.");

        attributes_destroy (data);
        free (ctx);
        return;
    }

    /*Get the bridge message from the mamaMsg*/
    if (MAMA_STATUS_OK!=(status=mamaMsgImpl_getBridgeMsg (tmpMsg,
                    &bridgeMsg)))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "avis_callback(): "
                  "Could not get bridge message from cached"
                  " queue mamaMsg [%d]", status);

        attributes_destroy (data);
        free (ctx);
        return;
    }

    /*Set the buffer and the reply handle on the bridge message structure*/
    if (MAMA_STATUS_OK!=(status=mamaMsgImpl_setMsgBuffer (tmpMsg,
                                data,
                                sizeof(data), MAMA_PAYLOAD_AVIS)))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "avis_callback(): mamaMsgImpl_setMsgBuffer() failed. [%d]",
                  status);

        attributes_destroy (data);
        free (ctx);
        return;
    }

    if (MAMA_STATUS_OK == mamaMsg_getOpaque (
            tmpMsg, ENCLOSED_MSG_FIELD_NAME, 0, &buf, &bufSize) && 
            bufSize > 0)
    {
        mamaMsg         encMsg = NULL;
        mamaBridgeImpl* bridge = 
            mamaSubscription_getBridgeImpl (avisSub(subscriber)->mMamaSubscription);

        status = mamaMsg_createFromByteBuffer (&encMsg, buf, bufSize);

        if (MAMA_STATUS_OK != status)
        {
            mama_log (MAMA_LOG_LEVEL_ERROR,
                      "avis_callback(): "
                      "Could not create message from enclosed byte buffer. [%d]",
                      status);

            attributes_destroy (data);
            free (ctx);
            return;
        }

        mamaMsgImpl_useBridgePayload (encMsg, bridge);
        mamaMsgImpl_getBridgeMsg (encMsg, &bridgeMsg);

        /*Set the buffer and the reply handle on the bridge message structure*/
        avisBridgeMamaMsgImpl_setAttributesAndSecure (bridgeMsg, data, 0);

        if (gMamaLogLevel >= MAMA_LOG_LEVEL_FINEST)
        {
            mama_log (MAMA_LOG_LEVEL_FINEST, "avis_callback(): "
                      "Received enclosed message: %s", mamaMsg_toString (encMsg));
        }

        if (MAMA_STATUS_OK != (status=mamaSubscription_processMsg
                    (avisSub(subscriber)->mMamaSubscription, encMsg)))
        {
            mama_log (MAMA_LOG_LEVEL_ERROR,
                      "avis_callback(): "
                      "mamaSubscription_processMsg() failed. [%d]",
                      status);
        }

        mamaMsg_destroy (encMsg);
    }
    else
    {
        avisBridgeMamaMsgImpl_setAttributesAndSecure (bridgeMsg, data, 0);

        if (gMamaLogLevel >= MAMA_LOG_LEVEL_FINEST)
        {
            mama_log (MAMA_LOG_LEVEL_FINEST, "avis_callback(): "
                           "Received message: %s", mamaMsg_toString (tmpMsg));
        }

        /*Process the message as normal*/
        if (MAMA_STATUS_OK != (status=mamaSubscription_processMsg
                    (avisSub(subscriber)->mMamaSubscription, tmpMsg)))
        {
            mama_log (MAMA_LOG_LEVEL_ERROR,
                      "avis_callback(): "
                      "mamaSubscription_processMsg() failed. [%d]",
                      status);
        }
    }

    free (ctx);
}

static void
avis_callback(
    Subscription*   subcription,
    Attributes*     attributes,
    bool            secure,
    void*           subscriber)
{
    avisCallbackContext* ctx = NULL;
    /* cant do anything without a subscriber */
    if (!avisSub(subscriber)) {
        mama_log (MAMA_LOG_LEVEL_ERROR, "avis_callback(): called with NULL subscriber!");
        return;
    }

    /*Make sure that the subscription is processing messages*/
    if ((!avisSub(subscriber)->mIsNotMuted) || (!avisSub(subscriber)->mIsValid)) return;

    ctx = (avisCallbackContext*) malloc (sizeof (avisCallbackContext));
    ctx->attributes = attributes_clone (attributes);
    ctx->subscriber = subscriber;

    mamaQueue_enqueueEvent (avisSub (subscriber)->mQueue,
                            avis_queue_callback, ctx);
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
    avisSubscription*    impl          = NULL;
    avisTransportBridge* avisTransport = NULL;
    
    if (!subscriber || !subscription || !transport )
        return MAMA_STATUS_NULL_ARG;

    impl = (avisSubscription*)calloc (1, sizeof(avisSubscription));
    if (impl == NULL)
       return MAMA_STATUS_NOMEM;

    impl->mAvis = getAvis(transport);

    if (!impl->mAvis)
       return MAMA_STATUS_INVALID_ARG;

    mamaTransport_getBridgeTransport (
        transport, (void*) &avisTransport);

    if (!avisTransport)
        return MAMA_STATUS_INVALID_ARG;

    /* Use a standard centralized method to determine a topic key */
    avisBridgeMamaSubscriptionImpl_generateSubjectKey (NULL,
                                                       source,
                                                       symbol,
                                                       &impl->mSubject);

    impl->mMamaCallback       = callback;
    impl->mMamaSubscription   = subscription;
    impl->mQueue              = queue;
    impl->mTransport          = transport;
    impl->mClosure            = closure;
    impl->mIsNotMuted         = 1;
    impl->mIsValid            = 1;
    impl->mAvisSubscription   = NULL;

    wsem_init(&impl->mCreateDestroySem, 0, 0);

    *subscriber =  (subscriptionBridge) impl;

    if (avisTransportBridge_isDispatching (avisTransport))
    {
        elvin_invoke (impl->mAvis, &subscribeAvis, impl);
        wsem_wait (&impl->mCreateDestroySem);
    }
    else
    {
        subscribeAvis (impl->mAvis, impl);
    }

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
    mama_status          status        = MAMA_STATUS_OK;
    wombatQueue          queue         = NULL;
    avisTransportBridge* avisTransport = NULL;

    CHECK_SUBSCRIBER(subscriber);

    mamaTransport_getBridgeTransport (
        avisSub(subscriber)->mTransport, (void*) &avisTransport);

    if (!avisTransport)
        return MAMA_STATUS_INVALID_ARG;

    if (avisTransportBridge_isDispatching (avisTransport))
    {
        elvin_invoke (avisSub(subscriber)->mAvis, &unsubscribeAvis, 
                      avisSub(subscriber)->mAvisSubscription);
    }
    else
    {
        unsubscribeAvis (avisSub(subscriber)->mAvis,
                         avisSub(subscriber)->mAvisSubscription);
    }

    avisSub(subscriber)->mAvisSubscription = NULL;
    
    mamaQueue_getNativeHandle(avisSub(subscriber)->mQueue, &queue);

    if (!queue)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "avisBridgeMamaSubscription_destroy(): "
                  "Could not get event queue.");
        return MAMA_STATUS_PLATFORM;
    }
                            
    avisSub(subscriber)->mMamaCallback.onDestroy (
        avisSub(subscriber)->mMamaSubscription, 
        avisSub(subscriber)->mClosure);

    wsem_destroy(&avisSub(subscriber)->mCreateDestroySem);

    free(avisSub(subscriber)->mSubject);
    free(avisSub(subscriber));

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
    if (NULL == error) return MAMA_STATUS_NULL_ARG;
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
    return avisBridgeMamaSubscription_mute (subscriber);
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

#define COPY_TOKEN_AND_SLIDE(dst, src, remaining, first)\
do {\
    if (src) {\
        size_t written = snprintf (dst, remaining, (first?"%s":".%s"), src);\
        dst       += written;\
        remaining -= written;\
    }\
} while(0);

/*
 * Internal function to ensure that the topic names are always calculated
 * in a particular way
 */
mama_status
avisBridgeMamaSubscriptionImpl_generateSubjectKey (const char* root,
                                                   const char* source,
                                                   const char* topic,
                                                   char**      keyTarget)
{
    char   subject_[MAX_SUBJECT_LENGTH];
    char*  subject   = subject_;
    size_t remaining = MAX_SUBJECT_LENGTH;

    COPY_TOKEN_AND_SLIDE(subject, root,   remaining, (subject_==subject));
    COPY_TOKEN_AND_SLIDE(subject, source, remaining, (subject_==subject));
    COPY_TOKEN_AND_SLIDE(subject, topic,  remaining, (subject_==subject));

    mama_log (MAMA_LOG_LEVEL_FINEST,
              "avisBridgeMamaSubscriptionImpl_generateSubjectKey(): %s", subject_);

    /*
     * Allocate the memory for copying the string. Caller is responsible for
     * destroying.
     */
    *keyTarget = strdup (subject_);
    if (NULL == *keyTarget)
    {
        return MAMA_STATUS_NOMEM;
    }
    else
    {
        return MAMA_STATUS_OK;
    }
}
