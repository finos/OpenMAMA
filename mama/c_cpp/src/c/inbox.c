/* $Id: inbox.c,v 1.4.14.3.2.1.4.3 2011/08/10 14:53:25 nicholasmarriott Exp $
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
#include "mama/inbox.h"

#include "inboximpl.h"
#include "bridge.h"
#include "queueimpl.h"

typedef struct mamaInboxImpl_
{
    /*Structure of bridge specific function pointers*/
   mamaBridgeImpl*  mBridgeImpl;
   /*The bridge specific inbox implementation*/
   inboxBridge      mMamaInboxBridgeImpl;

   /* This callback is invoked whenever the inbox has been destroyed. */
   mamaInboxDestroyCallback mOnInboxDestroyed;

   /* The mama queue associated with the inbox. */
   mamaQueue        mMamaQueue;

   /* The queue lock handle. */
   mamaQueueLockHandle mLockHandle;

} mamaInboxImpl;

/* Function prototypes. */
void MAMACALLTYPE mamaInbox_onInboxDestroyed (mamaInbox inbox, void *closure);

mama_status
mamaInbox_create (mamaInbox*             inbox,
                  mamaTransport          transport,
                  mamaQueue              queue,
                  mamaInboxMsgCallback   msgCB,
                  mamaInboxErrorCallback errorCB,
                  void*                  closure)
{
    /* Call the overload. */
    return mamaInbox_create2(inbox, transport, queue, msgCB, errorCB, NULL,
            closure);
}


mama_status
mamaInbox_create2 (mamaInbox*             inbox,
                   mamaTransport          transport,
                   mamaQueue              queue,
                   mamaInboxMsgCallback   msgCB,
                   mamaInboxErrorCallback errorCB,
                   mamaInboxDestroyCallback onInboxDestroyed,
                   void*                  closure)
{
    mama_status     status              =   MAMA_STATUS_OK;
    mamaInboxImpl*  impl                =   NULL;
    mamaBridgeImpl* bridgeImpl          =   NULL;

    if (!inbox)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaInbox_create (): NULL inbox"
                  " address.");
        return MAMA_STATUS_NULL_ARG;
    }

    *inbox = NULL;

    if (!transport)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaInbox_create (): NULL transport.");
        return MAMA_STATUS_INVALID_ARG;
    }

    if (!queue) return MAMA_STATUS_INVALID_QUEUE;

    /*Get the bridge impl from the queue - mandatory*/
    bridgeImpl = mamaQueueImpl_getBridgeImpl (queue);
    if (!bridgeImpl) return MAMA_STATUS_NO_BRIDGE_IMPL;

    impl = (mamaInboxImpl*)calloc (1, sizeof (mamaInboxImpl));
    if (!impl) return MAMA_STATUS_NOMEM;

    impl->mBridgeImpl = bridgeImpl;
    impl->mOnInboxDestroyed = onInboxDestroyed;
    impl->mMamaQueue = queue;

    /*Create the inbox bridge.*/
    if (MAMA_STATUS_OK!=(status=bridgeImpl->bridgeMamaInboxCreate (
                    &impl->mMamaInboxBridgeImpl,
                    transport,
                    queue,
                    msgCB,
                    errorCB,
                    mamaInbox_onInboxDestroyed,
                    closure,
                    (mamaInbox)impl)))
    {
        free (impl);
        return status;
    }

    /* Increment the object lock count on the queue. */
    impl->mLockHandle = mamaQueue_incrementObjectCount (impl->mMamaQueue, impl);

    *inbox = (mamaInbox)impl;

    return MAMA_STATUS_OK;
}

mama_status
mamaInbox_createByIndex (mamaInbox*             inbox,
                         mamaTransport          transport,
                         int                    tportIndex,
                         mamaQueue              queue,
                         mamaInboxMsgCallback   msgCB,
                         mamaInboxErrorCallback errorCB,
                         void*                  closure)
{
    return mamaInbox_createByIndex2(inbox, transport, tportIndex, queue, msgCB,
            errorCB, NULL, closure);
}

mama_status
mamaInbox_createByIndex2 (mamaInbox*             inbox,
                          mamaTransport          transport,
                          int                    tportIndex,
                          mamaQueue              queue,
                          mamaInboxMsgCallback   msgCB,
                          mamaInboxErrorCallback errorCB,
                          mamaInboxDestroyCallback onInboxDestroyed,
                          void*                  closure)
{
    mama_status     status              =   MAMA_STATUS_OK;
    mamaInboxImpl*  impl                =   NULL;
    mamaBridgeImpl* bridgeImpl          =   NULL;

    if (!inbox)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaInbox_create (): NULL inbox"
                  " address.");
        return MAMA_STATUS_NULL_ARG;
    }

    *inbox = NULL;

    if (!transport)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaInbox_create (): NULL transport.");
        return MAMA_STATUS_INVALID_ARG;
    }

    if (!queue) return MAMA_STATUS_INVALID_QUEUE;

    /*Get the bridge impl from the queue - mandatory*/
    bridgeImpl = mamaQueueImpl_getBridgeImpl (queue);
    if (!bridgeImpl) return MAMA_STATUS_NO_BRIDGE_IMPL;

    impl = (mamaInboxImpl*)calloc (1, sizeof (mamaInboxImpl));
    if (!impl) return MAMA_STATUS_NOMEM;

    impl->mBridgeImpl = bridgeImpl;
    impl->mOnInboxDestroyed = onInboxDestroyed;
    impl->mMamaQueue = queue;

    /*Create the inbox bridge.*/
    if (MAMA_STATUS_OK!=(status=bridgeImpl->bridgeMamaInboxCreateByIndex (
                    &impl->mMamaInboxBridgeImpl,
                    transport,
                    tportIndex,
                    queue,
                    msgCB,
                    errorCB,
                    mamaInbox_onInboxDestroyed,
                    closure,
                    (mamaInbox)impl)))
    {
        free (impl);
        return status;
    }

    /* Increment the object lock count on the queue. */
    impl->mLockHandle = mamaQueue_incrementObjectCount (impl->mMamaQueue, impl);

    *inbox = (mamaInbox)impl;

    return MAMA_STATUS_OK;
}

void MAMACALLTYPE mamaInbox_onInboxDestroyed (mamaInbox inbox, void *closure)
{
    /* Obtain the impl from the inbox object. */
    mamaInboxImpl *impl = (mamaInboxImpl *)inbox;

    /* Invoke the user's callback if it has been supplied. */
    if (impl->mOnInboxDestroyed != NULL)
    {
        (*impl->mOnInboxDestroyed)((mamaInbox)impl, closure);
    }

    /* Decrement the open object count on the queue. */
    mamaQueue_decrementObjectCount (&impl->mLockHandle, impl->mMamaQueue);

    /* Free the timer structure itself. */
    free (impl);
}

mama_status
mamaInbox_destroy (mamaInbox inbox)
{
    mamaInboxImpl* impl     =   (mamaInboxImpl*)inbox;
    mama_status    status   =   MAMA_STATUS_OK;

    if (!impl) return MAMA_STATUS_NULL_ARG;

    if (!impl->mBridgeImpl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaInbox_destroy (): "
                  "NULL bridge. Cannot destroy.");
        status = MAMA_STATUS_NO_BRIDGE_IMPL;
    }

    if (!impl->mMamaInboxBridgeImpl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaInbox_destroy (): "
                  "NULL io bridge. Cannot destroy.");
        status  = MAMA_STATUS_INVALID_ARG;
    }

    if (impl->mBridgeImpl && impl-> mMamaInboxBridgeImpl)
    {
        if (MAMA_STATUS_OK!=(status=impl->mBridgeImpl->bridgeMamaInboxDestroy
                    (impl->mMamaInboxBridgeImpl)))
        {
            mama_log (MAMA_LOG_LEVEL_ERROR, "mamaInbox_destroy (): "
                      "Could not destroy inbox bridge.");
        }
    }

    return MAMA_STATUS_OK;
}

inboxBridge
mamaInboxImpl_getInboxBridge (mamaInbox inbox)
{
    mamaInboxImpl* impl = (mamaInboxImpl*)inbox;
    if (!impl) return NULL;
    return impl->mMamaInboxBridgeImpl;
}
