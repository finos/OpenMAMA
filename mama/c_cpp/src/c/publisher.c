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

#include "mama/mama.h"
#include "mama/publisher.h"

#include "bridge.h"
#include "plugin.h"
#include "throttle.h"
#include "transportimpl.h"

#include "msgimpl.h"
#include "publisherimpl.h"
#include "mama/statscollector.h"
#include "mama/statfields.h"

#include "wombat/wInterlocked.h"

#include "list.h"

/* For Stats */
extern int gGenerateTransportStats;
extern int gGenerateGlobalStats;

/*Main mamaPublisher structure for the API*/
typedef struct mamaPublisherImpl_
{
    /*The bridge function pointer structure*/
    mamaBridgeImpl*     mBridgeImpl;
    /*The bridge implementation for the publisher*/
    publisherBridge     mMamaPublisherBridgeImpl;
    /*Outstanding actions for the throttled publishing*/
    wList               mPendingActions;
    /*Used for throttling of sends.*/
    mamaTransport       mTport;

    /* Publisher State */
    mamaPublisherState  mState;

    /* Publisher events callbacks */
    mamaPublisherCallbacks mUserCallbacks;

    /* Queue for publisher events */
    mamaQueue           mQueue;

    /* Closure for publisher events */
    void*               mClosure;

    wLock               mCreateDestroyLock;

    /* Store the topic */
    const char*         mRoot;
    const char*         mSource;
    const char*         mSymbol;

    void *mTransportHandle;
} mamaPublisherImpl;

/*Used when sending with the throttle*/
struct publisherClosure
{
    /*mAction must be first member of structure so throttle can remove it*/
    wombatThrottleAction        mAction;
    mamaPublisher               mPublisher;
    mamaInbox                   mInbox;
    mamaThrottledSendCompleteCb mSendCompleteCallback;
    void*                       mSendCompleteClosure;
};

/* Forward declarations */
static void
mamaPublisherImpl_setState(mamaPublisherImpl* impl, mamaPublisherState state);

static void
MAMACALLTYPE mamaPublisher_onPublisherDestroyed(mamaPublisher publisher, void *closure);

static mama_status
mamaPublisherImpl_destroy(mamaPublisherImpl *impl);

static void
mamaPublisherImpl_cleanup(mamaPublisherImpl* impl);

/**
 * Core create method used by all create methods
 */
static mama_status
_createByIndex (mamaPublisher*              result,
                mamaTransport               tport,
                int                         tportIndex,
                mamaQueue                   queue,
                mamaPublisherCallbacks*     publisherCallbacks,
                const char*                 symbol,
                const char*                 source,
                const char*                 root,
                void*                       closure)
{
    mama_status         status      = MAMA_STATUS_OK;
    mamaPublisherImpl*  impl        = NULL;
    mamaBridgeImpl*     bridgeImpl  = NULL;
    mamaPublisherCallbacks cb;

    if (!result)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaPublisher_create(): NULL"
                  " publisher address");
        return MAMA_STATUS_NULL_ARG;
    }

    *result = NULL;

    if (!tport)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaPublisher_create(): NULL"
                  " transport");
        return MAMA_STATUS_INVALID_ARG;
    }

    mama_log (MAMA_LOG_LEVEL_FINE, "mamaPublisher_create(): "
              "Creating publisher for %s.%s.%s",
              root ? root : "",
              source ? source : "",
              symbol ? symbol : "");

    /*Get the bridge impl from the transport - mandatory*/
    bridgeImpl = mamaTransportImpl_getBridgeImpl (tport);
    if (!bridgeImpl) return MAMA_STATUS_NO_BRIDGE_IMPL;

    impl = (mamaPublisherImpl*)calloc (1, sizeof (mamaPublisherImpl));
    if (!impl) return MAMA_STATUS_NOMEM;

    /* Save symbol */
    if (root) impl->mRoot = strdup(root);
    if (source) impl->mSource = strdup(source);
    if (symbol) impl->mSymbol = strdup(symbol);

    impl->mBridgeImpl = bridgeImpl;
    impl->mTport      = tport;

    if (publisherCallbacks)
    {
        impl->mUserCallbacks.onCreate = publisherCallbacks->onCreate;
        impl->mUserCallbacks.onError = publisherCallbacks->onError;
        impl->mUserCallbacks.onDestroy = publisherCallbacks->onDestroy;
    }
    impl->mQueue      = queue;
    impl->mClosure    = closure;

    /* Initialise the state. */
    wInterlocked_initialize(&impl->mState);

    /* Set the initial state of the publisher now that the memory has been allocated. */
    mamaPublisherImpl_setState(impl, MAMA_PUBLISHER_CREATING);

    if (MAMA_STATUS_OK!=(status=(bridgeImpl->bridgeMamaPublisherCreateByIndex (
                                    &impl->mMamaPublisherBridgeImpl,
                                    tport,
                                    tportIndex,
                                    symbol,                 /* topic */
                                    source,
                                    root,
                                    NULL,                   /* nativeQueueHandle */
                                    (mamaPublisher)impl)))) /* topic */
    {
        if (NULL != impl->mRoot) free((void*) impl->mRoot);
        if (NULL != impl->mSource) free((void*) impl->mSource);
        if (NULL != impl->mSymbol) free((void*) impl->mSymbol);
        free (impl);
        return status;
    }

    impl->mPendingActions = list_create (sizeof(struct publisherClosure));
    if (impl->mPendingActions == NULL) return MAMA_STATUS_NOMEM;

    cb.onCreate = impl->mUserCallbacks.onCreate;
    cb.onError = impl->mUserCallbacks.onError;
    cb.onDestroy = mamaPublisher_onPublisherDestroyed;        /* intercept onDestroy */
    if (MAMA_STATUS_OK!=(status=(bridgeImpl->bridgeMamaPublisherSetUserCallbacks (
                                   impl->mMamaPublisherBridgeImpl,
                                   impl->mQueue,
                                   cb,
                                   impl->mClosure
    ))))
    {
        free (impl);
        return status;
    }

    /* Create the mutex. */
    impl->mCreateDestroyLock = wlock_create();

    /* Set the state of the publisher now that the bridge's work is done. */
    mamaPublisherImpl_setState(impl, MAMA_PUBLISHER_LIVE);

    mamaTransport_addPublisher(tport, impl, &impl->mTransportHandle);

    *result = (mamaPublisher)impl;

    if (impl->mUserCallbacks.onCreate)
    {
        (*impl->mUserCallbacks.onCreate)(*result, impl->mClosure);
    }

    return MAMA_STATUS_OK;
}

mama_status
mamaPublisher_createByIndex (mamaPublisher* result,
                             mamaTransport  tport,
                             int            tportIndex,
                             mamaQueue      queue,
                             mamaPublisherCallbacks* cb,
                             const char*    symbol,
                             const char*    source,
                             const char*    root,
                             void*          closure)
{
    return _createByIndex(result,
                          tport,
                          tportIndex,
                          queue,
                          cb,
                          symbol,
                          source,
                          root,
                          closure);
}

mama_status
mamaPublisher_create (mamaPublisher*    result,
                      mamaTransport     tport,
                      const char*       symbol,
                      const char*       source,
                      const char*       root)
{

    return mamaPublisher_createByIndex (result,
                                        tport,
                                        0,
                                        NULL,
                                        NULL,
                                        symbol,
                                        source,
                                        root,
                                        NULL);
}

mama_status
mamaPublisher_createWithCallbacks (
                      mamaPublisher*    result,
                      mamaTransport     tport,
                      mamaQueue         queue,
                      const char*       symbol,
                      const char*       source,
                      const char*       root,
                      mamaPublisherCallbacks* cb,
                      void*             closure)
{
    return mamaPublisher_createByIndex (result,
                                        tport,
                                        0,
                                        queue,
                                        cb,
                                        symbol,
                                        source,
                                        root,
                                        closure);
}

mama_status
mamaPublisher_destroy (mamaPublisher publisher)
{
    mamaPublisherImpl* impl     = (mamaPublisherImpl*)publisher;
    mama_status        status   = MAMA_STATUS_OK;

    if (!publisher) return MAMA_STATUS_NULL_ARG;

    if(NULL != impl->mTport)
    {
        status = mamaPublisherImpl_destroy(impl);
    }

    return status;
}

mama_status
mamaPublisher_send (mamaPublisher publisher,
                    mamaMsg       msg)
{
    mamaPublisherImpl* impl     = (mamaPublisherImpl*)publisher;
    mama_status        status; 
    mamaStatsCollector tportStatsCollector = NULL;

    if (!impl) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMamaPublisherBridgeImpl) return MAMA_STATUS_INVALID_ARG;
    if (!impl->mBridgeImpl) return MAMA_STATUS_NO_BRIDGE_IMPL;

    /* Calling plugin hook */
    status = mamaPlugin_firePublisherPreSendHook (publisher, msg);
    if (MAMA_STATUS_OK != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mamaPublisher_send(): PublisherPreSendHook failed. Not sending message.");
        return status;
    }

    status = impl->mBridgeImpl->bridgeMamaPublisherSend
        (impl->mMamaPublisherBridgeImpl,
         msg);
    if (status == MAMA_STATUS_OK)
    {
        if (gGenerateTransportStats)
        {
            tportStatsCollector = mamaTransport_getStatsCollector (impl->mTport);
        }
        if (tportStatsCollector)
            mamaStatsCollector_incrementStat (tportStatsCollector, MamaStatPublisherSend.mFid);

        if (mamaInternal_getGlobalStatsCollector() != NULL)
            mamaStatsCollector_incrementStat (mamaInternal_getGlobalStatsCollector(),
                    MamaStatPublisherSend.mFid);

    }

    return status;
}

mama_status
mamaPublisher_sendFromInbox (mamaPublisher  publisher,
                             mamaInbox      inbox,
                             mamaMsg        msg)
{
    mamaPublisherImpl* impl     = (mamaPublisherImpl*)publisher;
    mama_status        status; 
    mamaStatsCollector tportStatsCollector = NULL;

    if (!impl) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMamaPublisherBridgeImpl) return MAMA_STATUS_INVALID_ARG;
    if (!impl->mBridgeImpl) return MAMA_STATUS_NO_BRIDGE_IMPL;

    status = impl->mBridgeImpl->bridgeMamaPublisherSendFromInbox
        (impl->mMamaPublisherBridgeImpl,
         inbox,
         msg);
    if (status == MAMA_STATUS_OK)
    {
        if (gGenerateTransportStats)
        {
            tportStatsCollector = mamaTransport_getStatsCollector (impl->mTport);
        }
        if (tportStatsCollector)
            mamaStatsCollector_incrementStat (tportStatsCollector, MamaStatPublisherInboxSend.mFid);

        if (mamaInternal_getGlobalStatsCollector() != NULL)
            mamaStatsCollector_incrementStat (mamaInternal_getGlobalStatsCollector(),
                    MamaStatPublisherInboxSend.mFid);

    }

    return status;
}

mama_status
mamaPublisher_sendFromInboxByIndex (mamaPublisher  publisher,
                                    int            tportIndex,
                                    mamaInbox      inbox,
                                    mamaMsg        msg)
{
    mamaPublisherImpl* impl     = (mamaPublisherImpl*)publisher;
    mama_status        status; 
    mamaStatsCollector tportStatsCollector = NULL;

    if (!impl) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMamaPublisherBridgeImpl) return MAMA_STATUS_INVALID_ARG;
    if (!impl->mBridgeImpl) return MAMA_STATUS_NO_BRIDGE_IMPL;

    status = impl->mBridgeImpl->bridgeMamaPublisherSendFromInboxByIndex
        (impl->mMamaPublisherBridgeImpl,
         tportIndex,
         inbox,
         msg);
    if (status == MAMA_STATUS_OK)
    {
        if (gGenerateTransportStats)
        {
            tportStatsCollector = mamaTransport_getStatsCollector (impl->mTport);
        }
        if (tportStatsCollector)
            mamaStatsCollector_incrementStat (tportStatsCollector, MamaStatPublisherInboxSend.mFid);

        if (mamaInternal_getGlobalStatsCollector() != NULL)
            mamaStatsCollector_incrementStat (mamaInternal_getGlobalStatsCollector(),
                    MamaStatPublisherInboxSend.mFid);

    }

    return status;
}

mama_status
mamaPublisher_sendReplyToInboxHandle (mamaPublisher publisher,
                                      mamaMsgReply  replyAddress,
                                      mamaMsg       reply)
{
    mamaPublisherImpl*  impl        = (mamaPublisherImpl*)publisher;
    mamaMsgReplyImpl*   replyStruct = ( mamaMsgReplyImpl *)replyAddress;
    mama_status         status; 
    mamaStatsCollector tportStatsCollector = NULL;


    if (!impl) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMamaPublisherBridgeImpl) return MAMA_STATUS_INVALID_ARG;
    if (!impl->mBridgeImpl) return MAMA_STATUS_NO_BRIDGE_IMPL;

    if (!replyAddress)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaPublisher_sendReplyToInbox() :"
                  "NULL reply handle message.");
        return MAMA_STATUS_INVALID_ARG;
    }

    if (!reply)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaPublisher_sendReplyToInbox(): "
                  "NULL reply message.");
        return MAMA_STATUS_INVALID_ARG;
    }


    status = impl->mBridgeImpl->bridgeMamaPublisherSendReplyToInboxHandle
        (impl->mMamaPublisherBridgeImpl,
         replyStruct->replyHandle,
         reply);
    if (status == MAMA_STATUS_OK)
    {
        if (gGenerateTransportStats)
        {
            tportStatsCollector = mamaTransport_getStatsCollector (impl->mTport);
        }
        if (tportStatsCollector)
            mamaStatsCollector_incrementStat (tportStatsCollector, MamaStatPublisherReplySend.mFid);

        if (mamaInternal_getGlobalStatsCollector() != NULL)
            mamaStatsCollector_incrementStat (mamaInternal_getGlobalStatsCollector(),
                    MamaStatPublisherReplySend.mFid);

    }

    return status;
}

mama_status
mamaPublisher_sendReplyToInbox (mamaPublisher publisher,
                                mamaMsg       request,
                                mamaMsg       reply)
{
    mamaPublisherImpl* impl     = (mamaPublisherImpl*)publisher;
    mama_status         status; 
    mamaStatsCollector tportStatsCollector = NULL;

    if (!impl) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMamaPublisherBridgeImpl) return MAMA_STATUS_INVALID_ARG;
    if (!impl->mBridgeImpl) return MAMA_STATUS_NO_BRIDGE_IMPL;
    if (!request)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaPublisher_sendReplyToInbox() :"
                  "NULL request message.");
        return MAMA_STATUS_INVALID_ARG;
    }

    if (!reply)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaPublisher_sendReplyToInbox(): "
                  "NULL reply message.");
        return MAMA_STATUS_INVALID_ARG;
    }

    if (!mamaMsg_isFromInbox (request))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaPublisher_sendReplyToInbox(): "
                  "Request message not from an inbox.");
        return MAMA_STATUS_INVALID_ARG;
    }
    status = impl->mBridgeImpl->bridgeMamaPublisherSendReplyToInbox
        (impl->mMamaPublisherBridgeImpl,
         request,
         reply);
    if (status == MAMA_STATUS_OK)
    {
        if (gGenerateTransportStats)
        {
            tportStatsCollector = mamaTransport_getStatsCollector (impl->mTport);
        }
        if (tportStatsCollector)
            mamaStatsCollector_incrementStat (tportStatsCollector, MamaStatPublisherReplySend.mFid);

        if (mamaInternal_getGlobalStatsCollector() != NULL)
            mamaStatsCollector_incrementStat (mamaInternal_getGlobalStatsCollector(),
                    MamaStatPublisherReplySend.mFid);

    }

    return status;
}

/*Action function used with sendWithThrottle*/
static void
sendMsgAction (void *closure1, void *closure2)
{
    mama_status status = MAMA_STATUS_OK;
    struct publisherClosure* pc = (struct publisherClosure*)closure2;

    mamaPublisherImpl* impl = (mamaPublisherImpl*)pc->mPublisher;

    status = mamaPublisher_send ((mamaPublisher)impl, (mamaMsg)closure1);

    if (pc->mSendCompleteCallback)
    {
        pc->mSendCompleteCallback (pc->mPublisher,
                                   (mamaMsg)closure1,
                                   status,
                                   pc->mSendCompleteClosure);
    }

    list_remove_element (impl->mPendingActions, pc);
    list_free_element (impl->mPendingActions, pc);
}


mama_status
mamaPublisher_sendWithThrottle (mamaPublisher               publisher,
                                mamaMsg                     msg,
                                mamaThrottledSendCompleteCb sendCompleteCb,
                                void*                       closure)
{
    mamaPublisherImpl*       impl     = (mamaPublisherImpl*)publisher;
    struct publisherClosure* pClosure = NULL;

    if (!publisher) return MAMA_STATUS_NULL_ARG;

    if (!msg)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mamaPublisher_sendWithThrottle(): NULL msg.");
        return MAMA_STATUS_INVALID_ARG;
    }

    pClosure = list_allocate_element (impl->mPendingActions);
    if (!pClosure) return MAMA_STATUS_NOMEM;

    list_push_back (impl->mPendingActions, pClosure);

    /*Initializse the closuse for the throttle*/
    pClosure->mPublisher            = publisher;
    pClosure->mSendCompleteCallback = sendCompleteCb;
    pClosure->mSendCompleteClosure  = closure;

    return mamaTransport_throttleAction (impl->mTport,
                                         MAMA_THROTTLE_DEFAULT,
                                         sendMsgAction,
                                         impl,
                                         msg, /*1st closure*/
                                         pClosure, /*2nd Closure*/
                                         0,
                                         &pClosure->mAction);
}

/*Used by sendFromInboxWithThrottle*/
static void
sendFromInboxMsgAction (void* closure1, void *closure2)
{
    mama_status status = MAMA_STATUS_OK;

    struct publisherClosure *c2 = (struct publisherClosure*)closure2;

    mamaPublisherImpl *impl = (mamaPublisherImpl*)c2->mPublisher;

    status = mamaPublisher_sendFromInbox ((mamaPublisher)impl,
                                          c2->mInbox,
                                          (mamaMsg)closure1);

    if (c2->mSendCompleteCallback)
    {
        c2->mSendCompleteCallback (
                (mamaPublisher)impl,
                (mamaMsg)closure1,
                status,
                c2->mSendCompleteClosure);
    }

    list_remove_element (impl->mPendingActions, c2);
    list_free_element (impl->mPendingActions, c2);
}

mama_status
mamaPublisher_sendFromInboxWithThrottle (mamaPublisher               publisher,
                                         mamaInbox                   inbox,
                                         mamaMsg                     msg,
                                         mamaThrottledSendCompleteCb sendCompleteCb,
                                         void*                       closure)
{
    mamaPublisherImpl*          impl    = (mamaPublisherImpl*)publisher;
    struct publisherClosure*    c2      = NULL;

    if (!impl) return MAMA_STATUS_NULL_ARG;

    c2 = (struct publisherClosure*)list_allocate_element(impl->mPendingActions);
    if (c2 == NULL) return MAMA_STATUS_NOMEM;

    if(!inbox)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                "mamaPublisher_sendFromInboxWithThrottle(): NULL inbox.");
        return MAMA_STATUS_INVALID_ARG;
    }

    if (!msg)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                "mamaPublisher_sendFromInboxWithThrottle(): NULL msg.");
        return MAMA_STATUS_INVALID_ARG;
    }

    if (gMamaLogLevel >= MAMA_LOG_LEVEL_FINER)
    {
        const char* msgString = mamaMsg_toString (msg);
        mama_log (MAMA_LOG_LEVEL_FINER, "INBOX REQUEST (throttled): %s", msgString);
        mamaMsg_freeString (msg, msgString);
    }

    c2->mPublisher            = publisher;
    c2->mInbox                = inbox;
    c2->mSendCompleteCallback = sendCompleteCb;
    c2->mSendCompleteClosure  = closure;

    list_push_back (impl->mPendingActions, c2);
    return mamaTransport_throttleAction (impl->mTport,
                                         MAMA_THROTTLE_DEFAULT,
                                         sendFromInboxMsgAction,
                                         impl,
                                         msg,
                                         c2,
                                         0,
                                         &c2->mAction);
}

mama_status mamaPublisherImpl_clearTransport (mamaPublisher publisher)
{
    return mamaPublisherImpl_destroy((mamaPublisherImpl *)publisher);
}

mama_status
mamaPublisher_getTransport (mamaPublisher   publisher,
                            mamaTransport*  transport)
{
    mamaPublisherImpl* impl   = (mamaPublisherImpl*)publisher;

    if (!impl) return MAMA_STATUS_NULL_ARG;
    *transport = impl->mTport;

    return MAMA_STATUS_OK;
}

mama_status
mamaPublisherCallbacks_allocate (mamaPublisherCallbacks** cb)
{
    mamaPublisherCallbacks* c = NULL;

    *cb = NULL;
    c = (mamaPublisherCallbacks*)calloc (1, sizeof (mamaPublisherCallbacks));
    if (NULL == c) return MAMA_STATUS_NOMEM;
    *cb = c;
    return MAMA_STATUS_OK;
}

mama_status
mamaPublisherCallbacks_deallocate (mamaPublisherCallbacks* cb)
{
    if (NULL == cb) return MAMA_STATUS_NULL_ARG;
    free((void*) cb);
    return MAMA_STATUS_OK;
}

mama_status
mamaPublisher_getUserCallbacks (mamaPublisher publisher,
                                mamaPublisherCallbacks* cb)
{
    mamaPublisherImpl *impl = (mamaPublisherImpl *)publisher;
    if (NULL == impl ||  NULL == cb)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    cb->onCreate = impl->mUserCallbacks.onCreate;
    cb->onError = impl->mUserCallbacks.onError;
    cb->onDestroy = impl->mUserCallbacks.onDestroy;

    return MAMA_STATUS_OK;
}

mama_status
mamaPublisher_getState (mamaPublisher publisher,
                        mamaPublisherState* state)
{
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if((NULL != publisher) && (NULL != state))
    {
        /* Get the impl. */
        mamaPublisherImpl *impl = (mamaPublisherImpl *)publisher;

        /* Return the state. */
        *state = wInterlocked_read(&impl->mState);
        ret = MAMA_STATUS_OK;
    }

    return ret;
}

const char*
mamaPublisher_stringForState (mamaPublisherState state)
{
    /* Switch the state and simply return a string representation of the error code. */
    switch(state)
    {
        case MAMA_PUBLISHER_UNKNOWN:                return "MAMA_PUBLISHER_UNKNOWN";
        case MAMA_PUBLISHER_CREATING:               return "MAMA_PUBLISHER_CREATING";
        case MAMA_PUBLISHER_LIVE:                   return "MAMA_PUBLISHER_LIVE";
        case MAMA_PUBLISHER_DESTROYING:             return "MAMA_PUBLISHER_DESTROYING";
        case MAMA_PUBLISHER_DESTROYED_BRIDGE:       return "MAMA_PUBLISHER_DESTROYED_BRIDGE";
        case MAMA_PUBLISHER_DESTROYING_WAIT_BRIDGE: return "MAMA_PUBLISHER_DESTROYING_WAIT_BRIDGE";
        case MAMA_PUBLISHER_DESTROYED:              return "MAMA_PUBLISHER_DESTROYED";
    }

    return "State not recognised";
}

mama_status
mamaPublisher_getRoot (mamaPublisher publisher, const char** root)
{
    mamaPublisherImpl* impl = (mamaPublisherImpl*) publisher;
    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    *root = impl->mRoot;
    return MAMA_STATUS_OK;
}

mama_status
mamaPublisher_getSource (mamaPublisher publisher, const char** source)
{
    mamaPublisherImpl* impl = (mamaPublisherImpl*) publisher;
    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    *source = impl->mSource;
    return MAMA_STATUS_OK;
}

mama_status
mamaPublisher_getSymbol (mamaPublisher publisher, const char** symbol)
{
    mamaPublisherImpl* impl = (mamaPublisherImpl*) publisher;
    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    *symbol = impl->mSymbol;
    return MAMA_STATUS_OK;
}

/* IMPL */

static void mamaPublisherImpl_cleanup(mamaPublisherImpl* impl)
{
    impl->mBridgeImpl               =   NULL;
    impl->mMamaPublisherBridgeImpl  =   NULL;
    impl->mTport                    =   NULL;
    impl->mQueue                    =   NULL;
    impl->mClosure                  =   NULL;

    if (NULL != impl->mRoot) free((void*) impl->mRoot);
    if (NULL != impl->mSource) free((void*) impl->mSource);
    if (NULL != impl->mSymbol) free((void*) impl->mSymbol);

    /* Destroy the state. */
    wInterlocked_destroy(&impl->mState);

    /* Destroy the mutex. */
    wlock_destroy(impl->mCreateDestroyLock);

    free (impl);
}

static void MAMACALLTYPE mamaPublisher_onPublisherDestroyed(mamaPublisher publisher, void *closure)
{
    mamaPublisherImpl* impl = (mamaPublisherImpl*) publisher;

    wlock_lock(impl->mCreateDestroyLock);

    if (NULL != impl->mUserCallbacks.onDestroy)
    {
        (*impl->mUserCallbacks.onDestroy)(publisher, impl->mClosure);
    }

    switch(wInterlocked_read(&impl->mState))
    {
        case MAMA_PUBLISHER_DESTROYING:
            /* Still in the destroy() call */
            mamaPublisherImpl_setState(impl, MAMA_PUBLISHER_DESTROYED_BRIDGE);
            break;
        case MAMA_PUBLISHER_DESTROYING_WAIT_BRIDGE:
            /* Not in destroy() finish */
            mamaPublisherImpl_setState(impl, MAMA_PUBLISHER_DESTROYED);
            wlock_unlock(impl->mCreateDestroyLock);
            mamaPublisherImpl_cleanup(impl);
            return;
            break;
        default:
            /* TODO */
            break;
    }
    wlock_unlock(impl->mCreateDestroyLock);
}

static void mamaPublisherImpl_setState(mamaPublisherImpl* impl, mamaPublisherState state)
{
    /* Set the state using an atomic operation so it will be thread safe. */
    wInterlocked_set(state, &impl->mState);
    mama_log(MAMA_LOG_LEVEL_FINEST, "Pubilsher %p is now at state %s.", impl, mamaPublisher_stringForState(state));
}

static mama_status mamaPublisherImpl_destroy(mamaPublisherImpl *impl)
{
    /* Returns. */
    mama_status        status   = MAMA_STATUS_OK;

    wlock_lock(impl->mCreateDestroyLock);

    /* Set the state to be deleted to at least show in the log that it has been completely removed. */
    mamaPublisherImpl_setState(impl, MAMA_PUBLISHER_DESTROYING);
                
    mamaTransport_removePublisher(impl->mTport, impl->mTransportHandle);

    if (!impl->mMamaPublisherBridgeImpl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mamaPublisher_destroy() No publisher impl. Cannot destroy.");
        status = MAMA_STATUS_INVALID_ARG;
    }

    if (!impl->mBridgeImpl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mamaPublisher_destroy() No bridge impl. Cannot destroy.");
        status = MAMA_STATUS_INVALID_ARG;
    }

    if (impl->mPendingActions && list_size (impl->mPendingActions))
    {
        if (MAMA_STATUS_OK!=(status=mamaTransport_throttleRemoveFromList (
                                                impl->mTport,
                                                MAMA_THROTTLE_DEFAULT,
                                                impl->mPendingActions)))
        {
            mama_log (MAMA_LOG_LEVEL_ERROR, "mamaPublisher_destroy():"
                    " Failed to remove actions for publisher");
        }
    }

    list_destroy (impl->mPendingActions, NULL, NULL);

    wlock_unlock(impl->mCreateDestroyLock);

    if (impl->mBridgeImpl &&
            impl->mMamaPublisherBridgeImpl)
    {
        if (MAMA_STATUS_OK!=(status=
                    impl->mBridgeImpl->bridgeMamaPublisherDestroy
                    (impl->mMamaPublisherBridgeImpl)))
        {
           mama_log (MAMA_LOG_LEVEL_ERROR, "mamaPublisher_destroy(): "
                     "Could not destroy publisher bridge.");
        }
    }

    wlock_lock(impl->mCreateDestroyLock);

    switch(wInterlocked_read(&impl->mState))
    {
        case MAMA_PUBLISHER_DESTROYING:
            /* The bridge has not yet called onDestroy */
            mamaPublisherImpl_setState(impl, MAMA_PUBLISHER_DESTROYING_WAIT_BRIDGE);
            break;
        case MAMA_PUBLISHER_DESTROYED_BRIDGE:
            /* The bridge has called onDestroy, so finish up */
            mamaPublisherImpl_setState(impl, MAMA_PUBLISHER_DESTROYED);
            wlock_unlock(impl->mCreateDestroyLock);
            mamaPublisherImpl_cleanup(impl);
            return status;
            break;
        default:
            /* TODO */
            break;
    }

    wlock_unlock(impl->mCreateDestroyLock);

    return status;
}

mamaTransport
mamaPublisherImpl_getTransportImpl (mamaPublisher publisher)
{
    mamaPublisherImpl* impl   = (mamaPublisherImpl*)publisher;

    if (!impl) return NULL;
    return impl->mTport;
}

