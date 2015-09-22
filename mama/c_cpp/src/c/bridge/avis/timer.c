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
#include "avisbridgefunctions.h"
#include <wombat/queue.h>


/*=========================================================================
  =                Typedefs, structs, enums and globals                   =
  =========================================================================*/

extern timerHeap gAvisTimerHeap;

typedef struct avisTimerImpl_
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
} avisTimerImpl;


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
 * @param closure In this instance, the closure will contain the avis timer
 *                implementation.
 */
static void MAMACALLTYPE
avisBridgeMamaTimerImpl_destroyCallback (mamaQueue queue, void* closure);

/**
 * When a timer fires, it enqueues this callback for execution. This is where
 * the action callback provided in the timer's create function gets fired.
 *
 * @param queue   MAMA queue from which this callback was fired.
 * @param closure In this instance, the closure will contain the avis timer
 *                implementation.
 */
static void MAMACALLTYPE
avisBridgeMamaTimerImpl_queueCallback (mamaQueue queue, void* closure);

/**
 * Every time the timer fires, it calls this timer callback which adds
 * avisBridgeMamaTimerImpl_queueCallback to the queue as long as the timer's
 * mDestroying flag is not currently set.
 *
 * @param timer   The underlying timer element which has just fired (not used).
 * @param closure In this instance, the closure will contain the avis timer
 *                implementation.
 */
static void
avisBridgeMamaTimerImpl_timerCallback (timerElement timer, void* closure);


/*=========================================================================
  =               Public interface implementation functions               =
  =========================================================================*/

mama_status
avisBridgeMamaTimer_create (timerBridge*  result,
                           void*         nativeQueueHandle,
                           mamaTimerCb   action,
                           mamaTimerCb   onTimerDestroyed,
                           double        interval,
                           mamaTimer     parent,
                           void*         closure)
{

    avisTimerImpl*              impl            = NULL;
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
    impl = (avisTimerImpl*) calloc (1, sizeof (avisTimerImpl));
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

    /* Create the first single fire timer */
    timerResult = createTimer (&impl->mTimerElement,
                               gAvisTimerHeap,
                               avisBridgeMamaTimerImpl_timerCallback,
                               &timeout,
                               impl);
    if (0 != timerResult)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "Failed to create Avis underlying timer [%d].", timerResult);
        return MAMA_STATUS_PLATFORM;
    }

    return MAMA_STATUS_OK;
}

/* This call should always come from MAMA queue thread */
mama_status
avisBridgeMamaTimer_destroy (timerBridge timer)
{
    avisTimerImpl*  impl            = NULL;
    mama_status     returnStatus    = MAMA_STATUS_OK;
    int             timerResult     = 0;

    if (NULL == timer)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Nullify the callback and set destroy flag */
    impl                            = (avisTimerImpl*) timer;
    impl->mDestroying               = 1;
    impl->mAction                   = NULL;

    /* Destroy the timer element */
    timerResult = destroyTimer (gAvisTimerHeap, impl->mTimerElement);
    if (0 != timerResult)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "Failed to destroy Avis underlying timer [%d].",
                  timerResult);
        returnStatus = MAMA_STATUS_PLATFORM;
    }

    /*
     * Put the impl free at the back of the queue to be executed when all
     * pending timer events have been completed
     */
    avisBridgeMamaQueue_enqueueEvent ((queueBridge) impl->mQueue,
                                      avisBridgeMamaTimerImpl_destroyCallback,
                                      (void*) impl);

    return returnStatus;
}

mama_status
avisBridgeMamaTimer_reset (timerBridge timer)
{
    avisTimerImpl*      impl            = (avisTimerImpl*) timer;
    int                 timerResult     = 0;
    struct timeval      timeout;

    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Calculate next time interval */
    timeout.tv_sec  = (time_t) impl->mInterval;
    timeout.tv_usec = ((impl->mInterval- timeout.tv_sec) * 1000000.0);

    /* Create the timer for the next firing */
    timerResult = resetTimer (gAvisTimerHeap,
                              impl->mTimerElement,
                              &timeout);
    if (0 != timerResult)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "Failed to reset Avis underlying timer [%d].", timerResult);
        return MAMA_STATUS_PLATFORM;
    }

    return MAMA_STATUS_OK;

}

mama_status
avisBridgeMamaTimer_setInterval (timerBridge  timer,
                                 mama_f64_t   interval)
{
    avisTimerImpl* impl  = (avisTimerImpl*) timer;
    if (NULL == timer)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    impl->mInterval = interval;

    return  avisBridgeMamaTimer_reset (timer);
}

mama_status
avisBridgeMamaTimer_getInterval (timerBridge    timer,
                                mama_f64_t*    interval)
{
    avisTimerImpl* impl  = NULL;
    if (NULL == timer || NULL == interval)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    impl = (avisTimerImpl*) timer;
    *interval = impl->mInterval;

    return MAMA_STATUS_OK;
}


/*=========================================================================
  =                  Private implementation functions                     =
  =========================================================================*/

/* This callback is invoked by the avis bridge's destroy event */
static void MAMACALLTYPE
avisBridgeMamaTimerImpl_destroyCallback (mamaQueue queue, void* closure)
{
    avisTimerImpl* impl = (avisTimerImpl*) closure;
    (*impl->mOnTimerDestroyed)(impl->mParent, impl->mClosure);

    /* Free the implementation memory here */
    free (impl);
}

/* This callback is invoked by the avis bridge's timer event */
static void MAMACALLTYPE
avisBridgeMamaTimerImpl_queueCallback (mamaQueue queue, void* closure)
{
    avisTimerImpl* impl = (avisTimerImpl*) closure;
    if (impl->mAction)
    {
        impl->mAction (impl->mParent, impl->mClosure);
    }
}

/* This callback is invoked by the common timer's dispatch thread */
static void
avisBridgeMamaTimerImpl_timerCallback (timerElement  timer,
                                       void*         closure)
{

    avisTimerImpl* impl = (avisTimerImpl*) closure;

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
        avisBridgeMamaTimer_reset ((timerBridge) closure);

        /* Enqueue the callback for handling */
        avisBridgeMamaQueue_enqueueEvent ((queueBridge) impl->mQueue,
                                          avisBridgeMamaTimerImpl_queueCallback,
                                          closure);
    }
}


