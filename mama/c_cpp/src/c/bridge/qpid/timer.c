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
#include <mama/timer.h>
#include <timers.h>
#include "qpidbridgefunctions.h"
#include "qpiddefs.h"
#include <wombat/queue.h>


/*=========================================================================
  =                Typedefs, structs, enums and globals                   =
  =========================================================================*/

typedef struct qpidTimerImpl_
{
    timerElement    mTimerElement;
    double          mInterval;
    void*           mClosure;
    mamaTimer       mParent;
    void*           mQueue;
    uint8_t         mDestroying;
    /* This callback will be invoked whenever the timer has been destroyed. */
    mamaTimerCb     mOnTimerDestroyed;
    /* This callback will be invoked on each timer firing */
    mamaTimerCb     mAction;
} qpidTimerImpl;


/*=========================================================================
  =                  Private implementation prototypes                    =
  =========================================================================*/

/**
 * Due to the fact that timed events may still be on the event queue, the
 * timer's destroy function does not destroy the implementation immediately.
 * Instead, it sets an implementation specific flag to stop further callbacks
 * from being enqueued from this timer, and then enqueues this function as a
 * callback on the queue to perform the actual destruction. This function also
 * calls the application developer's destroy callback function.
 *
 * @param queue   MAMA queue from which this callback was fired.
 * @param closure In this instance, the closure will contain the qpid timer
 *                implementation.
 */
static void MAMACALLTYPE
qpidBridgeMamaTimerImpl_destroyCallback (mamaQueue queue, void* closure);

/**
 * When a timer fires, it enqueues this callback for execution. This is where
 * the action callback provided in the timer's create function gets fired.
 *
 * @param queue   MAMA queue from which this callback was fired.
 * @param closure In this instance, the closure will contain the qpid timer
 *                implementation.
 */
static void MAMACALLTYPE
qpidBridgeMamaTimerImpl_queueCallback (mamaQueue queue, void* closure);

/**
 * Every time the timer fires, it calls this timer callback which adds
 * qpidBridgeMamaTimerImpl_queueCallback to the queue as long as the timer's
 * mDestroying flag is not currently set.
 *
 * @param timer   The underlying timer element which has just fired (not used).
 * @param closure In this instance, the closure will contain the qpid timer
 *                implementation.
 */
static void
qpidBridgeMamaTimerImpl_timerCallback (timerElement timer, void* closure);

/**
 * Utility function to get the qpid bridge closure from the timer implementation
 * object
 * @param impl  The timer implementation to query for the bridge closure
 * @return Qpid bridge closure object (bridge specific context)
 */
qpidBridgeClosure*
qpidBridgeMamaTimerImpl_getQpidBridgeClosure(qpidTimerImpl* impl);


/*=========================================================================
  =               Public interface implementation functions               =
  =========================================================================*/

mama_status
qpidBridgeMamaTimer_create (timerBridge*  result,
                           void*         nativeQueueHandle,
                           mamaTimerCb   action,
                           mamaTimerCb   onTimerDestroyed,
                           double        interval,
                           mamaTimer     parent,
                           void*         closure)
{
    qpidBridgeClosure*          bridgeClosure   = NULL;
    qpidTimerImpl*              impl            = NULL;
    int                         timerResult     = 0;
    struct timeval              timeout;

    if (NULL == result || NULL == nativeQueueHandle
            || NULL == action
            || NULL == parent )
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Null initialize the timer bridge supplied */
    *result = NULL;

    /* Allocate the timer implementation and set up */
    impl = (qpidTimerImpl*) calloc (1, sizeof (qpidTimerImpl));
    if (NULL == impl)
    {
        return MAMA_STATUS_NOMEM;
    }

    *result                     = (timerBridge) impl;
    impl->mQueue                = nativeQueueHandle;
    impl->mParent               = parent;
    impl->mAction               = action;
    impl->mClosure              = closure;
    impl->mInterval             = interval;
    impl->mOnTimerDestroyed     = onTimerDestroyed;
    impl->mDestroying           = 0;

    /* Determine when the next timer should fire */
    timeout.tv_sec  = (time_t) interval;
    timeout.tv_usec = ((interval-timeout.tv_sec) * 1000000.0);

    /* Get the timer heap from the bridge */
    bridgeClosure = qpidBridgeMamaTimerImpl_getQpidBridgeClosure (impl);

    /* Create the first single fire timer */
    timerResult = createTimer (&impl->mTimerElement,
                               bridgeClosure->mTimerHeap,
                               qpidBridgeMamaTimerImpl_timerCallback,
                               &timeout,
                               impl);
    if (0 != timerResult)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "Failed to create Qpid underlying timer [%d].", timerResult);
        return MAMA_STATUS_PLATFORM;
    }

    return MAMA_STATUS_OK;
}

/* This call should always come from MAMA queue thread */
mama_status
qpidBridgeMamaTimer_destroy (timerBridge timer)
{
    qpidTimerImpl*      impl            = NULL;
    qpidBridgeClosure*  bridgeClosure   = NULL;
    mama_status         returnStatus    = MAMA_STATUS_OK;
    int                 timerResult     = 0;

    if (NULL == timer)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Nullify the callback and set destroy flag */
    impl                            = (qpidTimerImpl*) timer;

    /* It is important to set mDestroying prior to calling destroyTimer.
     * This flag is checked by the common timer callback, during which the
     * common timer heap's lock is being held.  The call to destroyTimer uses
     * the common timer heap's lock, so we know that once the heap lock is
     * released for the destroy action that all future common timer callbacks
     * will see the mDestroying flag as set.
     *
     * If, for example, we were to remove the flag and instead use the
     * mTimerElement == NULL as a 'destroyed' flag, we wouldn't be able to
     * NULL the pointer until after it is destroyed.  Then there would be a
     * short period of time where mTimerElement is not NULL after it is
     * destroyed. */
    impl->mDestroying               = 1;
    impl->mAction                   = NULL;

    /* Get the timer heap from the bridge */
    bridgeClosure = qpidBridgeMamaTimerImpl_getQpidBridgeClosure (impl);

    /* Destroy the timer element */
    timerResult = destroyTimer (bridgeClosure->mTimerHeap, impl->mTimerElement);
    if (0 != timerResult)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "Failed to destroy Qpid underlying timer [%d].",
                  timerResult);
        returnStatus = MAMA_STATUS_PLATFORM;
    }

    /*
     * Put the impl free at the back of the queue to be executed when all
     * pending timer events have been completed
     */
    qpidBridgeMamaQueue_enqueueEvent ((queueBridge) impl->mQueue,
                                      qpidBridgeMamaTimerImpl_destroyCallback,
                                      (void*) impl);

    return returnStatus;
}

mama_status
qpidBridgeMamaTimer_reset (timerBridge timer)
{
    qpidTimerImpl*      impl            = (qpidTimerImpl*) timer;
    qpidBridgeClosure*  bridgeClosure   = NULL;
    int                 timerResult     = 0;
    struct timeval      timeout;

    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Calculate next time interval */
    timeout.tv_sec  = (time_t) impl->mInterval;
    timeout.tv_usec = ((impl->mInterval- timeout.tv_sec) * 1000000.0);

    /* Get the timer heap from the bridge */
    bridgeClosure = qpidBridgeMamaTimerImpl_getQpidBridgeClosure (impl);

    /* Create the timer for the next firing */
    timerResult = resetTimer (bridgeClosure->mTimerHeap,
                               impl->mTimerElement,
                               &timeout);
    if (0 != timerResult)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "Failed to reset Qpid underlying timer [%d].", timerResult);
        return MAMA_STATUS_PLATFORM;
    }

    return MAMA_STATUS_OK;

}

mama_status
qpidBridgeMamaTimer_setInterval (timerBridge  timer,
                                 mama_f64_t   interval)
{
    qpidTimerImpl* impl  = (qpidTimerImpl*) timer;
    if (NULL == timer)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    impl->mInterval = interval;

    return  qpidBridgeMamaTimer_reset (timer);
}

mama_status
qpidBridgeMamaTimer_getInterval (timerBridge    timer,
                                mama_f64_t*    interval)
{
    qpidTimerImpl* impl  = NULL;
    if (NULL == timer || NULL == interval)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    impl = (qpidTimerImpl*) timer;
    *interval = impl->mInterval;

    return MAMA_STATUS_OK;
}


/*=========================================================================
  =                  Private implementation functions                     =
  =========================================================================*/

/* This callback is invoked by the qpid bridge's destroy event */
static void MAMACALLTYPE
qpidBridgeMamaTimerImpl_destroyCallback (mamaQueue queue, void* closure)
{
    qpidTimerImpl* impl = (qpidTimerImpl*) closure;
    (*impl->mOnTimerDestroyed)(impl->mParent, impl->mClosure);

    /* Free the implementation memory here */
    free (impl);
}

/* This callback is invoked by the qpid bridge's timer event */
static void MAMACALLTYPE
qpidBridgeMamaTimerImpl_queueCallback (mamaQueue queue, void* closure)
{
    qpidTimerImpl* impl = (qpidTimerImpl*) closure;
    if (impl->mAction)
    {
        impl->mAction (impl->mParent, impl->mClosure);
    }
}

/* This callback is invoked by the common timer's dispatch thread */
static void
qpidBridgeMamaTimerImpl_timerCallback (timerElement  timer,
                                       void*         closure)
{

    qpidTimerImpl* impl = (qpidTimerImpl*) closure;

    if (NULL == impl)
    {
        return;
    }

    /*
     * Only enqueue further timer callbacks the timer is not currently getting
     * destroyed
     */
    if (0 == impl->mDestroying)
    {
        /* Set the timer for the next firing */
        qpidBridgeMamaTimer_reset ((timerBridge) closure);

        /* Enqueue the callback for handling */
        qpidBridgeMamaQueue_enqueueEvent ((queueBridge) impl->mQueue,
                                          qpidBridgeMamaTimerImpl_queueCallback,
                                          closure);
    }
}

qpidBridgeClosure*
qpidBridgeMamaTimerImpl_getQpidBridgeClosure (qpidTimerImpl* impl)
{
    mamaQueue           queue           = NULL;
    qpidBridgeClosure*  bridgeClosure   = NULL;
    mamaBridge          bridgeImpl      = NULL;

    /* Get the queue from the timer */
    mamaTimer_getQueue(impl->mParent, &queue);

    /* Get the bridge impl from the queue */
    bridgeImpl = mamaQueueImpl_getBridgeImpl (queue);

    /* Get the closure from the bridge */
    mamaBridgeImpl_getClosure(bridgeImpl, (void**)&bridgeClosure);

    return bridgeClosure;

}
