/* $Id: publisher.c,v 1.19.12.1.4.4 2011/08/10 14:53:25 nicholasmarriott Exp $
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
#include "throttle.h"
#include "transportimpl.h"

#include "msgimpl.h"
#include "publisherimpl.h"

#include "list.h"

/*Main mamaPublisher structure for the API*/
typedef struct mamaPublisherImpl_
{
    /*The bridge function pointer structure*/
    mamaBridgeImpl*     mBridgeImpl;
    /*The bridge implementation for the publisher*/
    publisherBridge     mMamaPublisherBridgeImpl;
    /*Outstanding actions for the throttled publishing*/
    wList           	mPendingActions;
    /*Used for throttling of sends.*/
    mamaTransport   	mTport;

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

static mama_status
_createByIndex (mamaPublisher*              result,
                mamaTransport               tport,
                int                         tportIndex,
                const char*                 symbol,
                const char*                 source,
                const char*                 root)
{
    mama_status         status      = MAMA_STATUS_OK;
    mamaPublisherImpl*  impl        = NULL;
    mamaBridgeImpl*     bridgeImpl  = NULL;

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

    impl->mPendingActions = list_create (sizeof(struct publisherClosure));
    if (impl->mPendingActions == NULL) return MAMA_STATUS_NOMEM;

    impl->mBridgeImpl = bridgeImpl;
    impl->mTport      = tport;

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
        free (impl);
        return status;
    }

    mamaTransport_addPublisher(tport, impl, &impl->mTransportHandle);

    *result = (mamaPublisher)impl;

    return MAMA_STATUS_OK;
}

mama_status
mamaPublisher_createByIndex (mamaPublisher* result,
                             mamaTransport  tport,
                             int            tportIndex,
                             const char*    symbol,
                             const char*    source,
                             const char*    root)
{
    return _createByIndex(result,
                          tport,
                          tportIndex,
                          symbol,
                          source,
                          root);
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
                                        symbol,
                                        source,
                                        root);
}

mama_status mamaPublisherImpl_destroy(mamaPublisherImpl *impl)
{
    /* Returns. */
    mama_status        status   = MAMA_STATUS_OK;

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

    impl->mBridgeImpl               =   NULL;
    impl->mMamaPublisherBridgeImpl  =   NULL;
    impl->mTport                    =   NULL;
    return status;
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

    free (impl);
    return status;
}

mama_status
mamaPublisher_send (mamaPublisher publisher,
                    mamaMsg       msg)
{
    mamaPublisherImpl* impl     = (mamaPublisherImpl*)publisher;

    if (!impl) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMamaPublisherBridgeImpl) return MAMA_STATUS_INVALID_ARG;
    if (!impl->mBridgeImpl) return MAMA_STATUS_NO_BRIDGE_IMPL;

    return impl->mBridgeImpl->bridgeMamaPublisherSend
        (impl->mMamaPublisherBridgeImpl,
         msg);
}

mama_status
mamaPublisher_sendFromInbox (mamaPublisher  publisher,
                             mamaInbox      inbox,
                             mamaMsg        msg)
{
    mamaPublisherImpl* impl     = (mamaPublisherImpl*)publisher;

    if (!impl) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMamaPublisherBridgeImpl) return MAMA_STATUS_INVALID_ARG;
    if (!impl->mBridgeImpl) return MAMA_STATUS_NO_BRIDGE_IMPL;

    return impl->mBridgeImpl->bridgeMamaPublisherSendFromInbox
        (impl->mMamaPublisherBridgeImpl,
         inbox,
         msg);
}

mama_status
mamaPublisher_sendFromInboxByIndex (mamaPublisher  publisher,
                                    int            tportIndex,
                                    mamaInbox      inbox,
                                    mamaMsg        msg)
{
    mamaPublisherImpl* impl     = (mamaPublisherImpl*)publisher;

    if (!impl) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMamaPublisherBridgeImpl) return MAMA_STATUS_INVALID_ARG;
    if (!impl->mBridgeImpl) return MAMA_STATUS_NO_BRIDGE_IMPL;

    return impl->mBridgeImpl->bridgeMamaPublisherSendFromInboxByIndex
        (impl->mMamaPublisherBridgeImpl,
         tportIndex,
         inbox,
         msg);
}

mama_status
mamaPublisher_sendReplyToInboxHandle (mamaPublisher publisher,
                                      mamaMsgReply  replyAddress,
                                      mamaMsg       reply)
{
    mamaPublisherImpl*  impl        = (mamaPublisherImpl*)publisher;
    mamaMsgReplyImpl*   replyStruct = ( mamaMsgReplyImpl *)replyAddress;


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


    return impl->mBridgeImpl->bridgeMamaPublisherSendReplyToInboxHandle
        (impl->mMamaPublisherBridgeImpl,
         replyStruct->replyHandle,
         reply);
}

mama_status
mamaPublisher_sendReplyToInbox (mamaPublisher publisher,
                                mamaMsg       request,
                                mamaMsg       reply)
{
    mamaPublisherImpl* impl     = (mamaPublisherImpl*)publisher;

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
    return impl->mBridgeImpl->bridgeMamaPublisherSendReplyToInbox
        (impl->mMamaPublisherBridgeImpl,
         request,
         reply);
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


