/* $Id: imagerequest.c,v 1.67.4.1.2.4.4.4 2011/08/10 14:53:25 nicholasmarriott Exp $
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

#include "wombat/port.h"
#include <mama/mama.h>
#include <mamainternal.h>
#include <imagerequest.h>
#include <throttle.h>
#include <bridge.h>
#include <list.h>
#include <mama/timer.h>
#include <mama/subscription.h>
#include <subscriptionimpl.h>
#include <assert.h>
#include "msgutils.h"
#include "destroyHandle.h"
#include "wlock.h"
#include "msgimpl.h"
#include "inboximpl.h"
#include "publisherimpl.h"
#include <mama/statfields.h>
#include <queueimpl.h>
#include <mama/statscollector.h>

#define userSymbolFormatted userSymbol ? userSymbol : "", \
                            userSymbol ? ":" : ""

/**
 * This class sends requests requests to feed handlers the require a point to
 * point response.
 */

extern int gGenerateTransportStats;
extern int gGenerateQueueStats;

typedef struct imageReqImpl_
{
    mamaTimer              mReplyTimer;
    uint8_t                mWaitingForResponse;
    mamaSubscription       mSubscription;
    SubjectContext*        mContext;
    mamaInbox              mP2PListener;
    double                 mTimeout;
    int                    mRetries;
    mamaMsg                mMsg;
    int                    mReqCount;
    mamaPublisher          mPublisher;
    wList                  mPendingActions;
    int                    mIsRecapRequest;
    wombatThrottle         mThrottle;

    /* This destroy handle is used to track the number of responses coming back
     * from the image requests. This is to ensure that the onMsg callback always
     * has memory to access if this structure has been destroyed.
     */
    pDestroyHandle         mResponse;

    /* This time object tracks when the reply timer is started. When the
     * image request is destroyed the reply timer will only be destroyed if the
     * timer event has not already been placed on the queue.
     */
    mamaDateTime           mReplyTimerStart;

    wLock                  mLock;
} imageReqImpl;

/* *************************************************** */
/* Private Function Prototypes. */
/* *************************************************** */

/**
 * This function will destroy the reply timer and is used whenever the image
 * request object is being destroyed. Note that it will only destroy the timer
 * if the timer event has not yet been placed on the queue. This check will be
 * made by working out the elapsed time since the timer was started and
 * comparing it to the interval.
 *
 * @param[in] impl The image request impl.
 * @return Mama status return code, can be one of:
 *      MAMA_STATUS_OK
 */
mama_status imageRequestImpl_checkDestroyReplyTimer (imageReqImpl *impl);

/**
 * This function is called whenever an intial or recap message is received. This message
 * will then be forwarded onto the subscription.
 *
 * @param[in] msg The initial message.
 * @param[in] closure The closure, this will be the image request structure wrapped in a
 *                    destroy handle. This is because it is possible that the image request
 *                    could be freed before the initial message is processed.
 */
void MAMACALLTYPE imageRequestImpl_onInitialMessage (mamaMsg msg, void *closure);

/**
 * This function is called whenever the reply timer ticks. This signals a timeout when
 * waiting for an initial or recap.
 *
 * @param[in] timer The mama timer.
 * @param[in] closure The closure, this will be the image request structure wrapped in a
 *                    destroy handle. This is because it is possible that the image request
 *                    could be freed before the initial message is processed.
 */
void MAMACALLTYPE imageRequestImpl_onReplyTimeout (mamaTimer timer, void *closure);

/**
 * This function will start the reply timer, this timer will tick if the initial or recap has not
 * been received and will then attempt to retry the request.
 *
 * @param[in] impl The image request impl.
 * @return Mama status value can be:
 *      MAMA_STATUS_INVALID_ARG if the reply timer is currently valid, (i.e. can't create a new one).
 *      MAMA_STATUS_OK
 */
mama_status imageRequestImpl_startReplyTimer (imageReqImpl *impl);
void         requestAction (void* request, void*      msg);
static void MAMACALLTYPE imageRequestImpl_processTimeout (imageReqImpl* impl);

/* *************************************************** */
/* Private Functions. */
/* *************************************************** */

mama_status imageRequestImpl_checkDestroyReplyTimer (imageReqImpl *impl)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_OK;

    /* Only continue if the reply timer is in fact valid. */
    if (NULL != impl->mReplyTimer)
    {
        /* Create a new date time object. */
        mamaDateTime currentTime = NULL;
        ret = mamaDateTime_create (&currentTime);
        if (MAMA_STATUS_OK == ret)
        {
            /* Record the current time. */
            ret = mamaDateTime_setToNow (currentTime);
            if (MAMA_STATUS_OK == ret)
            {
                /* Compare this to the time that the reply timer was started. */
                mama_f64_t timeDifference = 0;
                ret = mamaDateTime_diffSeconds (currentTime,
                        impl->mReplyTimerStart, &timeDifference);
                if (MAMA_STATUS_OK == ret)
                {
                    /* The timer event will only have been placed on the queue
                     * if the timer has elapsed. If the event is already on the
                     * queue then we must wait for it to be processed. Otherwise
                     * if the image request and the destroy handle are cleaned
                     * up here then there will be an access violation whenever
                     * the timer ticks. Alternatively if we always wait for the
                     * timer to tick then application shutdown will block for
                     * the maximum timeout if a subscription has just been
                     * destroyed.
                     * Note that a slight margin of error, (100ms) will be allowed.
                     */
                    if ((timeDifference + 0.1) < impl->mTimeout)
                    {
                        /* The timer event has not been placed on the queue and
                         * the timer can safely be destroyed. */
                        ret = mamaTimer_destroy (impl->mReplyTimer);
                        if (MAMA_STATUS_OK == ret)
                        {
                            /* Clear the variable. */
                            impl->mReplyTimer = NULL;

                            /* Also decrement the destroy handle count. */
                            destroyHandle_removeReference (impl->mResponse);
                        }
                    }
                }
            }
            /* Destroy the date time as it is no longer needed. */
            mamaDateTime_destroy (currentTime);
        }
    }

    return ret;
}

void MAMACALLTYPE imageRequestImpl_onInitialMessage (mamaMsg msg, void *closure)
{
    /* Get the destroy handle from the closure. */
    pDestroyHandle handle = (pDestroyHandle)closure;
    if (NULL != handle)
    {
        /* Obtain the impl from the destroy handle while decrementing the
         * response count.  Note that the destroy handle will be freed by this
         * function if there are no more references and the image request has
         * been destroyed.
         */
        imageReqImpl *impl = (imageReqImpl *)destroyHandle_removeReference
            (handle);

        /* The impl and subscription will be NULL if the image request has been
         * destroyed. */
        if ((NULL != impl) && (NULL != impl->mSubscription)) {
            /* The message will only be forwarded to the subscription if it is
             * awaiting activation, note that doing this here avoids a race
             * condition on shutdown where the subscription has already been
             * marked to be de-activating before this point.
             */
            mamaSubscriptionState state = MAMA_SUBSCRIPTION_UNKNOWN;
            mamaSubscription_getState (impl->mSubscription, &state);
            if (MAMA_SUBSCRIPTION_ACTIVATED == state)
            {
                mamaSubscription_processMsg (impl->mSubscription, msg);
            }
        }
    }
}

void MAMACALLTYPE imageRequestImpl_onReplyTimeout (
        mamaTimer timer, 
        void *closure)
{
    /* Only 1 reply is expected, destroy the timer before doing anything else.
     * Note that if the image request is retried then the timer will be
     * recreated.
     */
    mamaTimer_destroy (timer);
    {
        /* Get the destroy handle from the closure. */
        pDestroyHandle handle = (pDestroyHandle)closure;
        if (NULL != handle)
        {
            /* Obtain the impl from the destroy handle while decrementing the
             * response count.  Note that the destroy handle will be freed by
             * this function if there are no more references and the image
             * request has been destroyed.
             */
            imageReqImpl *impl = (imageReqImpl *)
                destroyHandle_removeReference (handle);

            /* The impl will be NULL if the image request has been destroyed. */
            if (NULL != impl)
            {
                /* Get the symbol. */
                const char *userSymbol = impl->mContext->mSymbol;

                /* Write a log message. */
                mama_log (MAMA_LOG_LEVEL_FINEST, 
                        "imageRequest::timeoutCallback (): %s%s timeout "
                        "callback (request=%p; timer=%p).",
                        userSymbolFormatted, impl, timer);

                /* Before processing the timeout clear the timer variable that
                 * was destroyed earlier, this is to ensure that it is correctly
                 * re-created if retries are required.
                 */
                impl->mReplyTimer = NULL;

                /* Process the timeout now that the impl is definitely valid. */
                imageRequestImpl_processTimeout (impl);
            }
        }
    }
}

mama_status imageRequestImpl_startReplyTimer (imageReqImpl *impl)
{
    /* Returns. */
    mama_status ret= MAMA_STATUS_INVALID_ARG;

    /* Only continue if the timer is NULL. */
    if (NULL == impl->mReplyTimer)
    {
        /* The timer must run on the same thread as the subscription, therefore
         * get its queue. */
        mamaQueue queue = NULL;
        ret = mamaSubscription_getQueue (impl->mSubscription, &queue);
        if (MAMA_STATUS_OK == ret)
        {
            /* Record the current time when the reply timer is started. */
            ret = mamaDateTime_setToNow (impl->mReplyTimerStart);
            if (MAMA_STATUS_OK == ret)
            {
                /* Increment the count as the destroy handle will be passed to
                 * the timer. */
                destroyHandle_incrementRefCount (impl->mResponse);
                /* Note that this timer will be destroyed in the callback. */
                ret = mamaTimer_create (&impl->mReplyTimer, queue,
                        imageRequestImpl_onReplyTimeout, impl->mTimeout,
                        impl->mResponse);
            }
        }
    }

    return ret;
}


mama_status
imageRequest_create (
    imageRequest*      result,
    mamaSubscription   subscription,
    SubjectContext*    subCtx,
    mamaPublisher      publisher,
    wombatThrottle     throttle)
{
    imageReqImpl* impl = NULL;
    mama_status   rval = MAMA_STATUS_OK;
    mamaTransport tport;
    mamaQueue     queue;
    const char*   userSymbol = NULL;
    int           tportIndex;

    mamaSubscription_getTransport ( subscription, &tport );
    mamaSubscription_getQueue ( subscription, &queue );

    impl = (imageReqImpl*)calloc ( 1, sizeof ( imageReqImpl ) ) ;

    mamaSubscription_getSymbol (subscription, &userSymbol);
    mama_log (MAMA_LOG_LEVEL_FINE, "%s%s Creating feed handler request: %p",
              userSymbolFormatted, (void*)impl );

    if ( impl == NULL )
    {
        return MAMA_STATUS_NOMEM;
    }

    impl->mPendingActions = list_create (sizeof (wombatThrottleAction));
    if ( impl->mPendingActions == NULL )
    {
        return MAMA_STATUS_NOMEM;
    }

    *result = (imageRequest)impl;

    impl->mTimeout          = MAMA_DEFAULT_TIMEOUT;
    impl->mRetries          = MAMA_DEFAULT_RETRIES;
    impl->mPublisher        = publisher;
    impl->mIsRecapRequest   = 0;
    impl->mLock             = wlock_create ();
    impl->mThrottle         = throttle;
    impl->mSubscription     = subscription;
    impl->mContext          = subCtx;

    /* Create a destroy handle to use when cleaning up the responses. */
    impl->mResponse = destroyHandle_create ((void *)impl);
    if ( impl->mResponse == NULL )
    {
        return MAMA_STATUS_NOMEM;
    }

    /* Create the date time object used to track when the reply timer starts. */
    mamaDateTime_create (&impl->mReplyTimerStart);

    mamaSubscription_getTransportIndex (subscription, &tportIndex);

    /* Create the inbox. */
    rval = mamaInbox_createByIndex (&impl->mP2PListener,
                                    tport,
                                    tportIndex,
                                    queue,
                                    imageRequestImpl_onInitialMessage,
                                    NULL,
                                    impl->mResponse);

    if ( rval != MAMA_STATUS_OK )
    {
        free (impl);
        return rval;
    }

    return rval; /* MAMA_STATUS_OK */
}

void imageRequest_destroy (imageRequest request)
{
    /* Get the impl. */
    imageReqImpl *impl = (imageReqImpl *)request;

    /* Get the transport from the subscription object. */
    mamaTransport transport = NULL;
    mamaSubscription_getTransport (impl->mSubscription, &transport);

    /* If the transport is invalid then there will be no image requests pending,
     * this check must be made here to avoid a crash on shut-down whenever the
     * transport is destroyed before the subscription is fully cleaned up.
     */
    if (NULL != transport)
    {
        /* Lock the throttle before the image request. */
        wombatThrottle_lock (impl->mThrottle);
        wlock_lock (impl->mLock);

        /* Remove any pending request. */
        if (impl->mWaitingForResponse)
        {
            imageRequest_stopWaitForResponse (request);
        }

        /* Unlock the image request then the throttle. */
        wlock_unlock (impl->mLock);
        wombatThrottle_unlock (impl->mThrottle);
    }

    /* Inform the destroy handle that the image request is being freed.
     * This means that the destroy handle will be deleted when the final
     * response comes in.
     */
    destroyHandle_destroyOwner (impl->mResponse);

    if ( impl->mP2PListener != NULL )
    {
        mamaInbox_destroy ( impl->mP2PListener);
        impl->mP2PListener = NULL;
    }

    list_destroy (impl->mPendingActions, NULL, NULL);

    if ( impl->mMsg )
    {
        mamaMsg_destroy ( impl->mMsg );
        impl->mMsg = NULL;
    }

    /* Destroy the date time object. */
    if (NULL != impl->mReplyTimerStart)
    {
        mamaDateTime_destroy (impl->mReplyTimerStart);
    }

    wlock_destroy ( impl->mLock );

    free (impl);
}

static void MAMACALLTYPE imageRequestImpl_processTimeout (imageReqImpl *impl)
{
    const char*   userSymbol = impl->mContext->mSymbol;

    if ( impl->mP2PListener == NULL )
    {
        return;
    }

    if ((!impl->mIsRecapRequest
            ? (mamaSubscription_getInitialCount (impl->mSubscription) > 0)
            : (impl->mContext->mImageCount >0)))
    {
        /* We received at least one full initial value.  This is not
         * an error/timeout. */
        const char*        source = NULL;
        const char*        symbol = NULL;
        void*              closure = NULL;
        mamaMsgCallbacks*  callbacks = NULL;
        mamaMsg            msg;
        mamaSubscription   subscription = impl->mSubscription;
        /* This is reset when imageRequest_stopWaitForResponse () is called
           so get the value now */
        int                isRecap      = impl->mIsRecapRequest;

        imageRequest_stopWaitForResponse (impl);
        if (!isRecap)
        {
            mamaSubscription_getSource  (subscription, &source);
            mamaSubscription_getSymbol  (subscription, &symbol);
            mamaSubscription_getClosure (subscription, &closure);

            msgUtils_createEndOfInitialsMsg (&msg);
            mamaMsgImpl_setSubscInfo (msg, NULL, source, symbol, 0);

            /* Mark the subscription as inactive if we are not expecting
             * any more updates. */
            if (!mamaSubscription_isExpectingUpdates (impl->mSubscription))
            {
                mamaSubscription_deactivate (impl->mSubscription);
            }

            callbacks = mamaSubscription_getUserCallbacks (subscription);
            callbacks->onMsg (subscription, msg, closure, NULL);

            mamaMsg_destroy ( msg );

            /*Do not access subscription here as it may have been deleted/destroyed*/
        }
        return;
    }

    if (impl->mReqCount++ < impl->mRetries)
    {
        mamaTransport tport;
        wombatThrottleAction *action = NULL;

        if ( !impl->mWaitingForResponse )
        {
            /* we are already done */
            return;
        }

        mama_log (MAMA_LOG_LEVEL_FINE,
                  "imageRequest::timeoutCallback (): %s%s Retrying subscription.",
                  userSymbolFormatted);
        mamaSubscription_getTransport ( impl->mSubscription, &tport );

        wombatThrottle_lock (impl->mThrottle);
        wlock_lock (impl->mLock);
        action = list_allocate_element (impl->mPendingActions);
        list_push_back (impl->mPendingActions, action);
        wombatThrottle_dispatch (impl->mThrottle,
                                 impl,
                                 requestAction,
                                 impl,
                                 action,
                                 1,
                                 action);

        /* Restart the reply timer now that another request is being made. */
        imageRequestImpl_startReplyTimer (impl);


        wlock_unlock (impl->mLock);
        wombatThrottle_unlock (impl->mThrottle);
    }
    else
    {
        const char*         source = NULL;
        const char*         symbol = NULL;
        void*               closure = NULL;
        mamaMsgCallbacks*   callbacks = NULL;
        mamaSubscription    subscription = impl->mSubscription;
         /* This is reset when imageRequest_stopWaitForResponse () is called
           so get the value now */
        int                 isRecap      = impl->mIsRecapRequest;
        mamaQueue           queue;
        mamaTransport       transport;
        mamaStatsCollector* queueStatsCollector = NULL;
        mamaStatsCollector* transportStatsCollector = NULL;

        /* We do not want to process any more messages if initial does not
         * arrive. For recaps we continue to process messages.
         */
        if (!isRecap)
        {
            mamaSubscription_cancel ( impl->mSubscription );
        }
        imageRequest_stopWaitForResponse ( impl );

        mamaSubscription_getSource   (subscription, &source);
        mamaSubscription_getSymbol   (subscription, &symbol);
        mamaSubscription_getClosure  (subscription, &closure);

        if (gGenerateQueueStats)
        {
            mamaSubscription_getQueue (subscription, &queue);
            queueStatsCollector = mamaQueueImpl_getStatsCollector (queue);

            mamaStatsCollector_incrementStat (*queueStatsCollector,
                    MamaStatTimeouts.mFid);
        }
        if (gGenerateTransportStats)
        {
            mamaSubscription_getTransport (subscription, &transport);
            transportStatsCollector = mamaTransport_getStatsCollector (transport);

            mamaStatsCollector_incrementStat (*transportStatsCollector,
                    MamaStatTimeouts.mFid);
        }
        if (mamaInternal_getGlobalStatsCollector () != NULL)
        {
            mamaStatsCollector_incrementStat
                (*(mamaInternal_getGlobalStatsCollector ()),
                 MamaStatTimeouts.mFid);
        }

        if (!isRecap)
        {
            mamaSubscription_deactivate  (subscription);
        }

        /* As the response will not come in the destroy handle reference count
         * must be decremented to counter the increment for the original
         * request.
         */
        destroyHandle_removeReference (impl->mResponse);

        mama_log (
            MAMA_LOG_LEVEL_FINE,
            "imageRequest::timeoutCallback (): "
            "%s%s Timeout waiting for recap or initial value (subscription=%p)",
            userSymbolFormatted, subscription);

        /* Send the message to the client.  We want it to show up
         * on the user's queue, not our internal queue. */

        if ( gMamaLogLevel >= MAMA_LOG_LEVEL_FINER )
        {
            mama_log (MAMA_LOG_LEVEL_FINER,
                      "imageRequest::timeoutCallback (): source=%s; symbol=%s",
                      source == NULL ? "" : source,
                      symbol == NULL ? "" : symbol);
        }

        if (isRecap)
        {
            mama_setLastError (MAMA_ERROR_RECAP_TIMEOUT);
        }
        else
        {
            mama_setLastError (MAMA_ERROR_INITIAL_TIMEOUT);
        }

        callbacks = mamaSubscription_getUserCallbacks (subscription);

        callbacks->onError (subscription,
                           MAMA_STATUS_TIMEOUT,
                           (void *)MAMA_MSG_STATUS_OK,
                           symbol,
                           closure);
    }
}

void
requestAction (void* request,
               void* action)
{
    imageReqImpl* impl =    (imageReqImpl*)request;
    mamaTransport tport;
    const char*   userSymbol = NULL;
    int           tportIndex;

    mamaSubscription_getTransport ( impl->mSubscription, &tport );
    wombatThrottle_lock (impl->mThrottle);
    wlock_lock (impl->mLock);
    if (action != NULL)
    {
        list_remove_element (impl->mPendingActions, action);
        list_free_element (impl->mPendingActions, action);
    }

    if ( impl->mMsg  == NULL ) /* We have been stoped */
    {
        wlock_unlock (impl->mLock);
        wombatThrottle_unlock (impl->mThrottle);
        return;
    }

    /* Start the reply timer, this will tick if the image request doesn't return
     * inside the interval. */
    imageRequestImpl_startReplyTimer (impl);

    mamaSubscription_getSymbol (impl->mSubscription, &userSymbol);
    mama_log (MAMA_LOG_LEVEL_FINE,
              "imageRequest::requestAction (): "
              "%s%s requesting initial value, dictionary,"
              "or recap (request=%p; timeoutTimer=%p).",
              userSymbolFormatted, request, impl->mReplyTimer);

    mamaSubscription_getTransportIndex (impl->mSubscription, &tportIndex);
    mamaPublisher_sendFromInboxByIndex (impl->mPublisher,
                                        tportIndex,
                                        impl->mP2PListener,
                                        impl->mMsg);
    wlock_unlock (impl->mLock);
    wombatThrottle_unlock (impl->mThrottle);
}


mama_status
imageRequest_sendRequest (
    imageRequest request,
    mamaMsg      msg,
    double       timeout,
    int          retries,
    int          throttle,
    int          isRecapRequest )
{
    imageReqImpl* impl = (imageReqImpl*)request;
    mamaTransport tport;
    const char*   userSymbol = NULL;
    int           tportIndex;
    mamaQueue     queue;
    impl->mTimeout              = timeout;
    impl->mRetries              = retries;
    impl->mIsRecapRequest       = isRecapRequest;
    impl->mWaitingForResponse   = 1;
    impl->mReqCount             = 0;
    impl->mMsg                  = msg;
    mamaSubscription_resetInitialCount (impl->mSubscription);
    impl->mContext->mImageCount = 0;
    mamaSubscription_getQueue (impl->mSubscription, &queue);

    if ( impl->mP2PListener == NULL )
    {
        mamaTransport tport;


        mamaSubscription_getTransport ( impl->mSubscription, &tport );
        mamaSubscription_getTransportIndex (impl->mSubscription, &tportIndex);


        mamaInbox_createByIndex (&impl->mP2PListener,
                                 tport,
                                 tportIndex,
                                 queue,
                                 imageRequestImpl_onInitialMessage,
                                 NULL,
                                 impl->mResponse);
    }

    mamaSubscription_getTransport ( impl->mSubscription, &tport );

    mamaSubscription_getSymbol (impl->mSubscription, &userSymbol);
    mama_log (MAMA_LOG_LEVEL_FINE,
              "imageRequest_sendRequest (): "
              "%s%s requesting initial value, dictionary, or recap.",
              userSymbolFormatted);

    /* An extra request means there will be an extra response, increment the
     * count. */
    destroyHandle_incrementRefCount (impl->mResponse);

    /*Initial requests are not throttled but dq recaps will be*/
    if ( throttle )
    {
        wombatThrottleAction *action = NULL;
        /* we destroy the message in the timeout callback or when
         * the initial arrives. In other words, when we stop waiting
         * for the response.
         */
        wombatThrottle_lock (impl->mThrottle);
        wlock_lock (impl->mLock);
        action = list_allocate_element (impl->mPendingActions);
        list_push_back (impl->mPendingActions, action);

        wombatThrottle_dispatch (impl->mThrottle,
                                impl,
                                requestAction,
                                impl,
                                action,
                                isRecapRequest,
                                action);

        wlock_unlock (impl->mLock);
        wombatThrottle_unlock (impl->mThrottle);
    }
    else
    {
        requestAction (request, NULL );
    }/*end if*/
    return MAMA_STATUS_OK;
}

void
imageRequest_stopWaitForResponse (
    imageRequest request)
{
    imageReqImpl* impl = (imageReqImpl*)request;
    mamaTransport tport;
    const char*   userSymbol = NULL;

    mamaSubscription_getTransport ( impl->mSubscription, &tport );

    wombatThrottle_lock (impl->mThrottle);
    wlock_lock (impl->mLock);

    /* Destroy the reply timer if a timer event has not been placed on the queue. */
    imageRequestImpl_checkDestroyReplyTimer (impl);

    if ( list_size (impl->mPendingActions) )
    {
        /* This is slow so only do it we think there is a request
         * on the throttle queue.
         */
        wombatThrottle_removeMessagesFromList (impl->mThrottle,
                                               impl->mPendingActions);
    }

    mamaSubscription_setExpectingInitial (impl->mSubscription, 0);
    mama_log (MAMA_LOG_LEVEL_FINER,
              "imageRequest_stopWaitForResponse (): "
              "%s%s stopping wait for response (request=%p; timer=%p).",
              userSymbolFormatted, request, impl->mReplyTimer);

    impl->mWaitingForResponse   = 0;
    impl->mIsRecapRequest       = 0;


    if ( impl->mP2PListener != NULL )
    {
        mamaInbox_destroy ( impl->mP2PListener);
        impl->mP2PListener = NULL;
    }

    if ( impl->mMsg )
    {
        mamaMsg_destroy ( impl->mMsg );
        impl->mMsg = NULL;
    }
    wlock_unlock (impl->mLock);
    wombatThrottle_unlock (impl->mThrottle);
}

/* protected */
uint8_t
imageRequest_isWaitingForResponse (
    imageRequest request)
{
    imageReqImpl* impl = (imageReqImpl*)request;
    return impl->mWaitingForResponse;
}

int
imageRequest_isRecapRequest (imageRequest request)
{
    imageReqImpl* impl = (imageReqImpl*)request;
    if (impl)
    {
        return impl->mIsRecapRequest;
    }
    return 0;
}

void imageRequest_cancelRequest (imageRequest request)
{
    /* If any initial request is pending it must be canceled. */
    if ((request != NULL) && (imageRequest_isWaitingForResponse (request)))
    {
        imageRequest_stopWaitForResponse (request);
    }
}

