/*=========================================================================
  =                             Includes                                  =
  =========================================================================*/

#include <mama/mama.h>
#include <wombat/queue.h>
#include <mama/integration/bridge.h>
#include <mama/integration/queue.h>
#include <mama/integration/bridge/base.h>


/*=========================================================================
  =                Typedefs, structs, enums and globals                   =
  =========================================================================*/

typedef struct baseQueueBridge {
    mamaQueue               mParent;
    wombatQueue             mQueue;
    uint8_t                 mHighWaterFired;
    size_t                  mHighWatermark;
    size_t                  mLowWatermark;
    uint8_t                 mIsDispatching;
    mamaQueueEnqueueCB      mEnqueueCallback;
    void*                   mEnqueueClosure;
    wthread_mutex_t         mDispatchLock;
    void*                   mClosure;
    baseQueueClosureCleanup mClosureCleanupCb;
} baseQueueBridge;


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
 * @param impl The queue bridge implementation to check.
 */
static void
baseBridgeMamaQueueImpl_checkWatermarks (baseQueueBridge* impl);


/*=========================================================================
  =               Public interface implementation functions               =
  =========================================================================*/

mama_status
baseBridgeMamaQueue_create (queueBridge* queue,
                            mamaQueue    parent)
{
    /* Null initialize the queue to be created */
    baseQueueBridge*    impl                = NULL;
    wombatQueueStatus   underlyingStatus    = WOMBAT_QUEUE_OK;

    if (queue == NULL || parent == NULL)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Null initialize the queueBridge */
    *queue = NULL;

    /* Allocate memory for the queue implementation */
    impl = (baseQueueBridge*) calloc (1, sizeof (baseQueueBridge));
    if (NULL == impl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "baseBridgeMamaQueue_create (): "
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
                  "baseBridgeMamaQueue_create (): "
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
                  "baseBridgeMamaQueue_create (): "
                  "Failed to create underlying queue.");
        wombatQueue_deallocate (impl->mQueue);
        free (impl);
        return MAMA_STATUS_PLATFORM;
    }

    /* Populate the queueBridge pointer with the implementation for return */
    *queue = (queueBridge) impl;

    return MAMA_STATUS_OK;
}

mama_status
baseBridgeMamaQueue_create_usingNative (queueBridge* queue,
                                        mamaQueue    parent,
                                        void*        nativeQueue)
{
    baseQueueBridge* impl = NULL;
    if (NULL == queue || NULL == parent || NULL == nativeQueue)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Null initialize the queueBridge to be returned */
    *queue = NULL;

    /* Allocate memory for the bridge implementation */
    impl = (baseQueueBridge*) calloc (1, sizeof (baseQueueBridge));
    if (NULL == impl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "baseBridgeMamaQueue_create_usingNative (): "
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
baseBridgeMamaQueue_destroy (queueBridge queue)
{
    wombatQueueStatus   status  = WOMBAT_QUEUE_OK;
    baseQueueBridge*    impl    = (baseQueueBridge*) queue;

    /* Perform null checks and return if null arguments provided */
    CHECK_QUEUE(impl);

    /* Destroy the underlying wombatQueue - can be called from any thread*/
    wthread_mutex_lock              (&impl->mDispatchLock);
    status = wombatQueue_destroy    (impl->mQueue);
    wthread_mutex_unlock            (&impl->mDispatchLock);

    /* Give any queue specific resources a chance to clean up */
    if (NULL != impl->mClosureCleanupCb && NULL != impl->mClosure)
    {
        impl->mClosureCleanupCb (impl->mClosure);
    }

    /* Free the impl container struct */
    free (impl);

    if (WOMBAT_QUEUE_OK != status)
    {
        mama_log (MAMA_LOG_LEVEL_WARN,
                  "baseBridgeMamaQueue_destroy (): "
                  "Failed to destroy wombat queue (%d).",
                  status);
        return MAMA_STATUS_PLATFORM;
    }

    return MAMA_STATUS_OK;
}

mama_status
baseBridgeMamaQueue_getEventCount (queueBridge queue, size_t* count)
{
    baseQueueBridge* impl       = (baseQueueBridge*) queue;
    int              countInt   = 0;

    if (NULL == count)
        return MAMA_STATUS_NULL_ARG;

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
baseBridgeMamaQueue_dispatch (queueBridge queue)
{
    wombatQueueStatus   status;
    baseQueueBridge*    impl = (baseQueueBridge*) queue;

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
        baseBridgeMamaQueueImpl_checkWatermarks (impl);

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
                  "baseBridgeMamaQueue_dispatch (): "
                  "Failed to dispatch Qpid Middleware queue (%d). ",
                  status);
        return MAMA_STATUS_PLATFORM;
    }

    return MAMA_STATUS_OK;
}

mama_status
baseBridgeMamaQueue_timedDispatch (queueBridge queue, uint64_t timeout)
{
    wombatQueueStatus   status;
    baseQueueBridge*    impl        = (baseQueueBridge*) queue;

    /* Perform null checks and return if null arguments provided */
    CHECK_QUEUE(impl);

    /* Check the watermarks to see if thresholds have been breached */
    baseBridgeMamaQueueImpl_checkWatermarks (impl);

    /* Attempt to dispatch the queue with a timeout once */
    status = wombatQueue_timedDispatch (impl->mQueue,
                                        NULL,
                                        NULL,
                                        timeout);

    /* If dispatch failed, report here */
    if (WOMBAT_QUEUE_OK != status && WOMBAT_QUEUE_TIMEOUT != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "baseBridgeMamaQueue_timedDispatch (): "
                  "Failed to dispatch Qpid Middleware queue (%d).",
                  status);
        return MAMA_STATUS_PLATFORM;
    }

    return MAMA_STATUS_OK;

}

mama_status
baseBridgeMamaQueue_dispatchEvent (queueBridge queue)
{
    wombatQueueStatus   status;
    baseQueueBridge*    impl = (baseQueueBridge*) queue;

    /* Perform null checks and return if null arguments provided */
    CHECK_QUEUE(impl);

    /* Check the watermarks to see if thresholds have been breached */
    baseBridgeMamaQueueImpl_checkWatermarks (impl);

    /* Attempt to dispatch the queue with a timeout once */
    status = wombatQueue_dispatch (impl->mQueue, NULL, NULL);

    /* If dispatch failed, report here */
    if (WOMBAT_QUEUE_OK != status && WOMBAT_QUEUE_TIMEOUT != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "baseBridgeMamaQueue_dispatchEvent (): "
                  "Failed to dispatch Qpid Middleware queue (%d).",
                  status);
        return MAMA_STATUS_PLATFORM;
    }

    return MAMA_STATUS_OK;
}

mama_status
baseBridgeMamaQueue_enqueueEvent (queueBridge        queue,
                                  mamaQueueEventCB   callback,
                                  void*              closure)
{
    wombatQueueStatus   status;
    baseQueueBridge*    impl = (baseQueueBridge*) queue;

    if (NULL == callback)
        return MAMA_STATUS_NULL_ARG;

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
                  "baseBridgeMamaQueue_enqueueEvent (): "
                  "Failed to enqueueEvent (%d). Callback: %p; Closure: %p",
                  status, callback, closure);
        return MAMA_STATUS_PLATFORM;
    }

    return MAMA_STATUS_OK;
}

mama_status
baseBridgeMamaQueue_stopDispatch (queueBridge queue)
{
    baseQueueBridge* impl = (baseQueueBridge*) queue;

    /* Perform null checks and return if null arguments provided */
    CHECK_QUEUE(impl);

    /* Tell this implementation to stop dispatching */
    impl->mIsDispatching = 0;

    return MAMA_STATUS_OK;
}

mama_status
baseBridgeMamaQueue_setEnqueueCallback (queueBridge        queue,
                                        mamaQueueEnqueueCB callback,
                                        void*              closure)
{
    baseQueueBridge* impl   = (baseQueueBridge*) queue;

    if (NULL == callback)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    /* Perform null checks and return if null arguments provided */
    CHECK_QUEUE(impl);

    /* Set the enqueue callback and closure */
    impl->mEnqueueCallback  = callback;
    impl->mEnqueueClosure   = closure;

    return MAMA_STATUS_OK;
}

mama_status
baseBridgeMamaQueue_removeEnqueueCallback (queueBridge queue)
{
    baseQueueBridge* impl = (baseQueueBridge*) queue;

    /* Perform null checks and return if null arguments provided */
    CHECK_QUEUE(impl);

    /* Set the enqueue callback to NULL */
    impl->mEnqueueCallback  = NULL;
    impl->mEnqueueClosure   = NULL;

    return MAMA_STATUS_OK;
}

mama_status
baseBridgeMamaQueue_getNativeHandle (queueBridge queue,
                                     void**      nativeHandle)
{
    baseQueueBridge* impl = (baseQueueBridge*) queue;

    if (NULL == nativeHandle)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Perform null checks and return if null arguments provided */
    CHECK_QUEUE(impl);

    /* Return the handle to the native queue */
    *nativeHandle = queue;

    return MAMA_STATUS_OK;
}

mama_status
baseBridgeMamaQueue_setHighWatermark (queueBridge queue,
                                      size_t      highWatermark)
{
    baseQueueBridge* impl = (baseQueueBridge*) queue;

    if (0 == highWatermark)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    /* Perform null checks and return if null arguments provided */
    CHECK_QUEUE(impl);

    /* Set the high water mark */
    impl->mHighWatermark = highWatermark;

    return MAMA_STATUS_OK;
}

mama_status
baseBridgeMamaQueue_setLowWatermark (queueBridge    queue,
                                     size_t         lowWatermark)
{
    baseQueueBridge* impl = (baseQueueBridge*) queue;

    if (0 == lowWatermark)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    /* Perform null checks and return if null arguments provided */
    CHECK_QUEUE(impl);

    /* Set the low water mark */
    impl->mLowWatermark = lowWatermark;

    return MAMA_STATUS_OK;
}


/*=========================================================================
  =                  Public implementation functions                      =
  =========================================================================*/

void
baseBridgeMamaQueueImpl_setClosure (queueBridge              queue,
                                    void*                    closure,
                                    baseQueueClosureCleanup  callback)
{
    baseQueueBridge* impl = (baseQueueBridge*) queue;
    impl->mClosure          = closure;
    impl->mClosureCleanupCb = callback;
}

void*
baseBridgeMamaQueueImpl_getClosure (queueBridge              queue)
{
    baseQueueBridge* impl = (baseQueueBridge*) queue;
    return impl->mClosure;
}


/*=========================================================================
  =                  Private implementation functions                     =
  =========================================================================*/

void
baseBridgeMamaQueueImpl_checkWatermarks (baseQueueBridge* impl)
{
    size_t              eventCount      =  0;

    /* Get the current size of the wombat impl */
    baseBridgeMamaQueue_getEventCount      ((queueBridge) impl, &eventCount);

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

