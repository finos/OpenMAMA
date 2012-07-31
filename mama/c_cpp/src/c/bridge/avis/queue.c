/* $Id: queue.c,v 1.1.2.6 2011/10/02 19:02:18 ianbell Exp $
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

#include <mama/mama.h>
#include <bridge.h>
#include "queueimpl.h"
#include "avisbridgefunctions.h"
#include <wombat/queue.h>

#include <avis/elvin.h>

typedef struct avisQueueBridge {
    mamaQueue          mParent;
    wombatQueue        mQueue;
    uint8_t            mIsNative;
} avisQueueBridge;

typedef struct avisQueueClosure {
    avisQueueBridge* mImpl;
    mamaQueueEventCB mCb;
    void*            mUserClosure;
} avisQueueClosure;

#define avisQueue(queue) ((avisQueueBridge*) queue)
#define CHECK_QUEUE(queue) \
        do {  \
           if (avisQueue(queue) == 0) return MAMA_STATUS_NULL_ARG; \
           if (avisQueue(queue)->mQueue == NULL) return MAMA_STATUS_NULL_ARG; \
         } while(0)


mama_status
avisBridgeMamaQueue_create (queueBridge* queue,
                            mamaQueue    parent)
{
    avisQueueBridge* avisQueue = NULL;
    if (queue == NULL)
        return MAMA_STATUS_NULL_ARG;
    *queue = NULL;

    avisQueue = (avisQueueBridge*)calloc (1, sizeof (avisQueueBridge));
    if (avisQueue == NULL)
        return MAMA_STATUS_NOMEM;

    avisQueue->mParent  = parent;

    wombatQueue_allocate (&avisQueue->mQueue);
    wombatQueue_create (avisQueue->mQueue, 0, 0, 0);

    *queue = (queueBridge) avisQueue;

    return MAMA_STATUS_OK;
}

mama_status
avisBridgeMamaQueue_create_usingNative (queueBridge* queue,
                                        mamaQueue    parent,
                                        void*        nativeQueue)
{
    avisQueueBridge* avisQueue = NULL;
    if (queue == NULL)
        return MAMA_STATUS_NULL_ARG;
    *queue = NULL;

    avisQueue = (avisQueueBridge*)calloc (1, sizeof (avisQueueBridge));
    if (avisQueue == NULL)
        return MAMA_STATUS_NOMEM;

    avisQueue->mParent  = parent;
    avisQueue->mQueue   = (wombatQueue)nativeQueue;
    avisQueue->mIsNative = 1;

    *queue = (queueBridge) avisQueue;

    return MAMA_STATUS_OK;
}

mama_status
avisBridgeMamaQueue_destroy (queueBridge queue)
{
    CHECK_QUEUE(queue);
    if (avisQueue(queue)->mIsNative)
        wombatQueue_destroy (avisQueue(queue)->mQueue);
    free(avisQueue(queue));
    return MAMA_STATUS_OK;
}

mama_status
avisBridgeMamaQueue_dispatch (queueBridge queue)
{
    wombatQueueStatus status;

    CHECK_QUEUE(queue);
    do
    {
        /* 500 is .5 seconds */
        status = wombatQueue_timedDispatch (avisQueue(queue)->mQueue,
                     NULL, NULL, 500);
    }
    while ((status == WOMBAT_QUEUE_OK ||
            status == WOMBAT_QUEUE_TIMEOUT) &&
            mamaQueueImpl_isDispatching (avisQueue(queue)->mParent));

    if (status != WOMBAT_QUEUE_OK && status != WOMBAT_QUEUE_TIMEOUT)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "Failed to dispatch Avis Middleware queue. %d",
                  "mamaQueue_dispatch ():",
                  status);
        return MAMA_STATUS_PLATFORM;
    }

    return MAMA_STATUS_OK;
}

mama_status
avisBridgeMamaQueue_timedDispatch (queueBridge queue, uint64_t timeout)
{
    wombatQueueStatus status;
    CHECK_QUEUE(queue);

    status = wombatQueue_timedDispatch (avisQueue(queue)->mQueue,
                     NULL, NULL, timeout);
    if (status == WOMBAT_QUEUE_TIMEOUT) return MAMA_STATUS_TIMEOUT;

    if (status != WOMBAT_QUEUE_OK)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "Failed to dispatch Avis Middleware queue. %d",
                  "mamaQueue_dispatch ():",
                  status);
        return MAMA_STATUS_PLATFORM;
    }

    return MAMA_STATUS_OK;
}

mama_status
avisBridgeMamaQueue_dispatchEvent (queueBridge queue)
{
    wombatQueueStatus status;
    CHECK_QUEUE(queue);

    status = wombatQueue_dispatch (avisQueue(queue)->mQueue,
                     NULL, NULL);

    if (status != WOMBAT_QUEUE_OK)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "Failed to dispatch Avis Middleware queue. %d",
                  "mamaQueue_dispatch ():",
                  status);
        return MAMA_STATUS_PLATFORM;
    }

    return MAMA_STATUS_OK;
}

static void MAMACALLTYPE queueCb (void *ignored, void* closure)
{
    avisQueueClosure* cl = (avisQueueClosure*)closure;
    if (NULL ==cl) return;
    cl->mCb (cl->mImpl->mParent, cl->mUserClosure);
    free (cl);
}

mama_status
avisBridgeMamaQueue_enqueueEvent (queueBridge        queue,
                                  mamaQueueEventCB   callback,
                                  void*              closure)
{
    wombatQueueStatus status;
    avisQueueClosure* cl = NULL;
    CHECK_QUEUE(queue);

    cl = (avisQueueClosure*)calloc(1, sizeof(avisQueueClosure));
    if (NULL == cl) return MAMA_STATUS_NOMEM;

    cl->mImpl = avisQueue(queue);
    cl->mCb    = callback;
    cl->mUserClosure = closure;

    status = wombatQueue_enqueue (avisQueue(queue)->mQueue,
                queueCb, NULL, cl);

    if (status != WOMBAT_QUEUE_OK)
        return MAMA_STATUS_PLATFORM;

    return MAMA_STATUS_OK;
}

mama_status
avisBridgeMamaQueue_stopDispatch (queueBridge queue)
{
    wombatQueueStatus status;
    CHECK_QUEUE(queue);

    if (queue == NULL)
        return MAMA_STATUS_NULL_ARG;

    status = wombatQueue_unblock (avisQueue(queue)->mQueue);
    if (status != WOMBAT_QUEUE_OK)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  " Failed to stop dispatching Avis Middleware queue.",
                  "wmwMamaQueue_stopDispatch ():");
        return MAMA_STATUS_PLATFORM;
    }

    return MAMA_STATUS_OK;
}

mama_status
avisBridgeMamaQueue_setEnqueueCallback (queueBridge        queue,
                                        mamaQueueEnqueueCB callback,
                                        void*              closure)
{
    CHECK_QUEUE(queue);
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisBridgeMamaQueue_removeEnqueueCallback (queueBridge queue)
{
    CHECK_QUEUE(queue);
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisBridgeMamaQueue_getNativeHandle (queueBridge queue,
                                     void**      result)
{
    CHECK_QUEUE(queue);
    *result = avisQueue(queue)->mQueue;
    return MAMA_STATUS_OK;
}

mama_status
avisBridgeMamaQueue_setHighWatermark (queueBridge queue,
                                      size_t      highWatermark)
{
    CHECK_QUEUE(queue);
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisBridgeMamaQueue_setLowWatermark (queueBridge queue,
                                     size_t lowWatermark)
{
    CHECK_QUEUE(queue);
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisBridgeMamaQueue_getEventCount (queueBridge queue, size_t* count)
{
    CHECK_QUEUE(queue);
    *count = 0;
    wombatQueue_getSize (avisQueue(queue)->mQueue, (int*)count);
    return MAMA_STATUS_OK;
}
