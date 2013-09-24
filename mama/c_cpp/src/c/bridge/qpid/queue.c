/* $Id$
 *
 * OpenMAMA: The open middleware agnostic messaging API
 * Copyright (C) 2011 NYSE Inc.
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


/*=========================================================================
  =                             Includes                                  =
  =========================================================================*/

#include <mama/mama.h>
#include <wombat/queue.h>
#include <bridge.h>
#include "queueimpl.h"
#include "qpidbridgefunctions.h"


/*=========================================================================
  =                Typedefs, structs, enums and globals                   =
  =========================================================================*/

typedef struct qpidQueueBridge {
    mamaQueue          mParent;
    wombatQueue        mQueue;
    uint8_t            mHighWaterFired;
    size_t             mHighWatermark;
    size_t             mLowWatermark;
    uint8_t            mIsDispatching;
    mamaQueueEnqueueCB mEnqueueCallback;
    void*              mEnqueueClosure;
    wthread_mutex_t    mDispatchLock;
} qpidQueueBridge;


/*=========================================================================
  =                              Macros                                   =
  =========================================================================*/

#define     CHECK_QUEUE(IMPL)                                                  \
    do {                                                                       \
        if (IMPL == NULL)              return MAMA_STATUS_NULL_ARG;            \
        if (IMPL->mQueue == NULL)      return MAMA_STATUS_NULL_ARG;            \
    } while(0)

/* Timeout is in milliseconds */
#define     QPID_QUEUE_DISPATCH_TIMEOUT     500
#define     QPID_QUEUE_MAX_SIZE             WOMBAT_QUEUE_MAX_SIZE
#define     QPID_QUEUE_CHUNK_SIZE           WOMBAT_QUEUE_CHUNK_SIZE
#define     QPID_QUEUE_INITIAL_SIZE         WOMBAT_QUEUE_CHUNK_SIZE


/*=========================================================================
  =                  Private implementation prototypes                    =
  =========================================================================*/

/**
 * This funcion is called to check the current queue size against configured
 * watermarks to determine whether or not it should call the watermark callback
 * functions. If it determines that it should, it invokes the relevant callback
 * itself.
 *
 * @param impl The qpid queue bridge implementation to check.
 */
static void
qpidBridgeMamaQueueImpl_checkWatermarks (qpidQueueBridge* impl);


/*=========================================================================
  =               Public interface implementation functions               =
  =========================================================================*/

mama_status
qpidBridgeMamaQueue_create (queueBridge* queue,
                            mamaQueue    parent)
{
    /* Null initialize the queue to be created */
    qpidQueueBridge*    impl                = NULL;
    wombatQueueStatus   underlyingStatus    = WOMBAT_QUEUE_OK;

    if (queue == NULL)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Null initialize the queueBridge */
    *queue = NULL;

    /* Allocate memory for the qpid queue implementation */
    impl = (qpidQueueBridge*) calloc (1, sizeof (qpidQueueBridge));
    if (NULL == impl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "qpidBridgeMamaQueue_create (): ",
                  "Failed to allocate memory for queue.");
        return MAMA_STATUS_NOMEM;
    }

    /* Initialize the dispatch lock */
    wthread_mutex_init (&impl->mDispatchLock, NULL);

    /* Back-reference the parent for future use in the implementation struct */
    impl->mParent = parent;

    /* Allocate and create the wombat queue */
    underlyingStatus = wombatQueue_allocate (&impl->mQueue);
    if (WOMBAT_QUEUE_OK != underlyingStatus)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "qpidBridgeMamaQueue_create (): ",
                  "Failed to allocate memory for underlying queue.");
        free (impl);
        return MAMA_STATUS_NOMEM;
    }

    underlyingStatus = wombatQueue_create (impl->mQueue,
                                           QPID_QUEUE_MAX_SIZE,
                                           QPID_QUEUE_INITIAL_SIZE,
                                           QPID_QUEUE_CHUNK_SIZE);
    if (WOMBAT_QUEUE_OK != underlyingStatus)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "qpidBridgeMamaQueue_create (): ",
                  "Failed to create underlying queue.");
        wombatQueue_destroy (impl->mQueue);
        free (impl);
        return MAMA_STATUS_PLATFORM;
    }

    /* Populate the queueBridge pointer with the implementation for return */
    *queue = (queueBridge) impl;

    return MAMA_STATUS_OK;
}

mama_status
qpidBridgeMamaQueue_create_usingNative (queueBridge* queue,
                                        mamaQueue    parent,
                                        void*        nativeQueue)
{
    qpidQueueBridge* impl = NULL;
    if (NULL == queue)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Null initialize the queueBridge to be returned */
    *queue = NULL;

    /* Allocate memory for the qpid bridge implementation */
    impl = (qpidQueueBridge*) calloc (1, sizeof (qpidQueueBridge));
    if (NULL == impl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "qpidBridgeMamaQueue_create_usingNative (): ",
                  "Failed to allocate memory for queue.");
        return MAMA_STATUS_NOMEM;
    }

    /* Back-reference the parent for future use in the implementation struct */
    impl->mParent = parent;

    /* Wombat queue has already been created, so simply reference it here */
    impl->mQueue = (wombatQueue) nativeQueue;

    /* Populate the queueBridge pointer with the implementation for return */
    *queue = (queueBridge) impl;

    return MAMA_STATUS_OK;
}

mama_status
qpidBridgeMamaQueue_destroy (queueBridge queue)
{
    wombatQueueStatus   status  = WOMBAT_QUEUE_OK;
    qpidQueueBridge*    impl    = (qpidQueueBridge*) queue;

    /* Perform null checks and return if null arguments provided */
    CHECK_QUEUE(impl);

    /* Destroy the underlying wombatQueue - can be called from any thread*/
    wthread_mutex_lock              (&impl->mDispatchLock);
    status = wombatQueue_destroy    (impl->mQueue);
    wthread_mutex_unlock            (&impl->mDispatchLock);

    /* Free the qpidQueueImpl container struct */
    free (impl);

    if (WOMBAT_QUEUE_OK != status)
    {
        mama_log (MAMA_LOG_LEVEL_WARN,
                  "qpidBridgeMamaQueue_destroy (): ",
                  "%s Failed to destroy wombat queue (%d).",
                  mamaStatus_stringForStatus(status),
                  status);
        return MAMA_STATUS_PLATFORM;
    }

    return MAMA_STATUS_OK;
}

mama_status
qpidBridgeMamaQueue_getEventCount (queueBridge queue, size_t* count)
{
    qpidQueueBridge* impl       = (qpidQueueBridge*) queue;
    int              countInt   = 0;

    /* Perform null checks and return if null arguments provided */
    CHECK_QUEUE(impl);

    /* Initialize count to zero */
    *count = 0;

    /* Get the wombatQueue size */
    wombatQueue_getSize (impl->mQueue, &countInt);
    *count = (size_t)countInt;

    return MAMA_STATUS_OK;
}

mama_status
qpidBridgeMamaQueue_dispatch (queueBridge queue)
{
    wombatQueueStatus   status;
    qpidQueueBridge*    impl = (qpidQueueBridge*) queue;

    /* Perform null checks and return if null arguments provided */
    CHECK_QUEUE(impl);

    /* Lock for dispatching */
    wthread_mutex_lock (&impl->mDispatchLock);

    impl->mIsDispatching = 1;

    /*
     * Continually dispatch as long as the calling application wants dispatching
     * to be done and no errors are encountered
     */
    do
    {
        /* Check the watermarks to see if thresholds have been breached */
        qpidBridgeMamaQueueImpl_checkWatermarks (impl);

        /*
         * Perform a dispatch with a timeout to allow the dispatching process
         * to be interrupted by the calling application between iterations
         */
        status = wombatQueue_timedDispatch (impl->mQueue,
                                            NULL,
                                            NULL,
                                            QPID_QUEUE_DISPATCH_TIMEOUT);
    }
    while ( (WOMBAT_QUEUE_OK == status || WOMBAT_QUEUE_TIMEOUT == status)
            && impl->mIsDispatching);

    /* Unlock the dispatch lock */
    wthread_mutex_unlock (&impl->mDispatchLock);

    /* Timeout is encountered after each dispatch and so is expected here */
    if (WOMBAT_QUEUE_OK != status && WOMBAT_QUEUE_TIMEOUT != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "qpidBridgeMamaQueue_dispatch (): "
                  "Failed to dispatch Qpid Middleware queue (%d). ",
                  status);
        return MAMA_STATUS_PLATFORM;
    }

    return MAMA_STATUS_OK;
}

mama_status
qpidBridgeMamaQueue_timedDispatch (queueBridge queue, uint64_t timeout)
{
    wombatQueueStatus   status;
    qpidQueueBridge*    impl        = (qpidQueueBridge*) queue;

    /* Perform null checks and return if null arguments provided */
    CHECK_QUEUE(impl);

    /* Check the watermarks to see if thresholds have been breached */
    qpidBridgeMamaQueueImpl_checkWatermarks (impl);

    /* Attempt to dispatch the queue with a timeout once */
    status = wombatQueue_timedDispatch (impl->mQueue,
                                        NULL,
                                        NULL,
                                        timeout);

    /* If dispatch failed, report here */
    if (WOMBAT_QUEUE_OK != status && WOMBAT_QUEUE_TIMEOUT != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "qpidBridgeMamaQueue_timedDispatch (): ",
                  "Failed to dispatch Qpid Middleware queue (%d).",
                  status);
        return MAMA_STATUS_PLATFORM;
    }

    return MAMA_STATUS_OK;

}

mama_status
qpidBridgeMamaQueue_dispatchEvent (queueBridge queue)
{
    wombatQueueStatus   status;
    qpidQueueBridge*    impl = (qpidQueueBridge*) queue;

    /* Perform null checks and return if null arguments provided */
    CHECK_QUEUE(impl);

    /* Check the watermarks to see if thresholds have been breached */
    qpidBridgeMamaQueueImpl_checkWatermarks (impl);

    /* Attempt to dispatch the queue with a timeout once */
    status = wombatQueue_dispatch (impl->mQueue, NULL, NULL);

    /* If dispatch failed, report here */
    if (WOMBAT_QUEUE_OK != status && WOMBAT_QUEUE_TIMEOUT != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "qpidBridgeMamaQueue_dispatchEvent (): ",
                  "Failed to dispatch Qpid Middleware queue (%d).",
                  status);
        return MAMA_STATUS_PLATFORM;
    }

    return MAMA_STATUS_OK;
}

mama_status
qpidBridgeMamaQueue_enqueueEvent (queueBridge        queue,
                                  mamaQueueEventCB   callback,
                                  void*              closure)
{
    wombatQueueStatus   status;
    qpidQueueBridge*    impl = (qpidQueueBridge*) queue;

    /* Perform null checks and return if null arguments provided */
    CHECK_QUEUE(impl);

    /* Call the underlying wombatQueue_enqueue method */
    status = wombatQueue_enqueue (impl->mQueue,
                                  (wombatQueueCb) callback,
                                  impl->mParent,
                                  closure);

    /* Call the enqueue callback if provided */
    if (NULL != impl->mEnqueueCallback)
    {
        impl->mEnqueueCallback (impl->mParent, impl->mEnqueueClosure);
    }

    /* If dispatch failed, report here */
    if (WOMBAT_QUEUE_OK != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "qpidBridgeMamaQueue_enqueueEvent (): ",
                  "Failed to enqueueEvent (%d). Callback: %p; Closure: %p",
                  status, callback, closure);
        return MAMA_STATUS_PLATFORM;
    }

    return MAMA_STATUS_OK;
}

mama_status
qpidBridgeMamaQueue_stopDispatch (queueBridge queue)
{
    qpidQueueBridge* impl = (qpidQueueBridge*) queue;

    /* Perform null checks and return if null arguments provided */
    CHECK_QUEUE(impl);

    /* Tell this implementation to stop dispatching */
    impl->mIsDispatching = 0;

    return MAMA_STATUS_OK;
}

mama_status
qpidBridgeMamaQueue_setEnqueueCallback (queueBridge        queue,
                                        mamaQueueEnqueueCB callback,
                                        void*              closure)
{
    qpidQueueBridge* impl   = (qpidQueueBridge*) queue;

    /* Perform null checks and return if null arguments provided */
    CHECK_QUEUE(impl);

    /* Set the enqueue callback and closure */
    impl->mEnqueueCallback  = callback;
    impl->mEnqueueClosure   = closure;

    return MAMA_STATUS_OK;
}

mama_status
qpidBridgeMamaQueue_removeEnqueueCallback (queueBridge queue)
{
    qpidQueueBridge* impl = (qpidQueueBridge*) queue;

    /* Perform null checks and return if null arguments provided */
    CHECK_QUEUE(impl);

    /* Set the enqueue callback to NULL */
    impl->mEnqueueCallback  = NULL;
    impl->mEnqueueClosure   = NULL;

    return MAMA_STATUS_OK;
}

mama_status
qpidBridgeMamaQueue_getNativeHandle (queueBridge queue,
                                     void**      nativeHandle)
{
    qpidQueueBridge* impl = (qpidQueueBridge*) queue;

    /* Perform null checks and return if null arguments provided */
    CHECK_QUEUE(impl);

    /* Return the handle to the native queue */
    *nativeHandle = queue;

    return MAMA_STATUS_OK;
}

mama_status
qpidBridgeMamaQueue_setHighWatermark (queueBridge queue,
                                      size_t      highWatermark)
{
    qpidQueueBridge* impl = (qpidQueueBridge*) queue;

    /* Perform null checks and return if null arguments provided */
    CHECK_QUEUE(impl);

    /* Set the high water mark */
    impl->mHighWatermark = highWatermark;

    return MAMA_STATUS_OK;
}

mama_status
qpidBridgeMamaQueue_setLowWatermark (queueBridge    queue,
                                     size_t         lowWatermark)
{
    qpidQueueBridge* impl = (qpidQueueBridge*) queue;

    /* Perform null checks and return if null arguments provided */
    CHECK_QUEUE(impl);

    /* Set the low water mark */
    impl->mLowWatermark = lowWatermark;

    return MAMA_STATUS_OK;
}


/*=========================================================================
  =                  Private implementation functions                     =
  =========================================================================*/

void
qpidBridgeMamaQueueImpl_checkWatermarks (qpidQueueBridge* impl)
{
    size_t              eventCount      =  0;

    /* Get the current size of the wombat impl */
    qpidBridgeMamaQueue_getEventCount      ((queueBridge) impl, &eventCount);

    /* If the high watermark had been fired but the event count is now down */
    if (0 != impl->mHighWaterFired && eventCount == impl->mLowWatermark)
    {
        impl->mHighWaterFired = 0;
        mamaQueueImpl_lowWatermarkExceeded (impl->mParent, eventCount);
    }
    /* If the high watermark is not currently fired and now above threshold */
    else if (0 == impl->mHighWaterFired && eventCount >= impl->mHighWatermark)
    {
        impl->mHighWaterFired = 1;
        mamaQueueImpl_highWatermarkExceeded (impl->mParent, eventCount);
    }
}

