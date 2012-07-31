/* $Id: timer.c,v 1.1.2.4 2011/09/23 12:35:42 ianbell Exp $
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
#include <mama/timer.h>
#include <timers.h>
#include "avisbridgefunctions.h"
#include <wombat/queue.h>

extern timerHeap gTimerHeap;

typedef struct avisTimerImpl_
{
    timerElement  mTimerElement;
    double        mInterval;
    mamaTimerCb   mAction;
    void*         mClosure;
    mamaTimer     mParent;
    wombatQueue   mQueue;

    /* This callback will be invoked whenever the timer has been completely destroyed. */
    mamaTimerCb     mOnTimerDestroyed;

    /* TODO: add queue */
} avisTimerImpl;

static void MAMACALLTYPE
destroy_callback(void* timer, void* closure)
{
	avisTimerImpl* impl = (avisTimerImpl*)timer;

    (*impl->mOnTimerDestroyed)(impl->mParent, impl->mClosure);
    free (impl);
}

static void MAMACALLTYPE
timerQueueCb (void* data, void* closure)
{
    avisTimerImpl* impl = (avisTimerImpl*)data;

    if (impl->mAction)
        impl->mAction (impl->mParent, impl->mClosure);

}

static void
timerCb (timerElement  timer,
         void*         closure)
{
    avisTimerImpl* impl = (avisTimerImpl*)closure;
    struct timeval timeout;

    if (impl == NULL) return;

    /* Mama timers are repeating */
    timeout.tv_sec = (time_t)impl->mInterval;
    timeout.tv_usec = ((impl->mInterval- timeout.tv_sec) * 1000000.0);
    if (0 != createTimer (&impl->mTimerElement,
                          gTimerHeap,
                          timerCb,
                          &timeout,
                          impl))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
              "%s Failed to create Avis timer.",
              "mamaTimer_create ():");
    }

    wombatQueue_enqueue (impl->mQueue, timerQueueCb,
            (void*)impl, NULL);
}

mama_status
avisBridgeMamaTimer_create (timerBridge*  result,
                           void*         nativeQueueHandle,
                           mamaTimerCb   action,
                           mamaTimerCb   onTimerDestroyed,
                           double        interval,
                           mamaTimer     parent,
                           void*         closure)
{
    avisTimerImpl* impl      =   NULL;
    struct timeval timeout;

    if (result == NULL) return MAMA_STATUS_NULL_ARG;

    mama_log (MAMA_LOG_LEVEL_FINEST,
              "%s Entering with interval [%f].",
              "avisMamaTimer_create ():",
              interval);

    *result = NULL;

    impl = (avisTimerImpl*)calloc (1, sizeof (avisTimerImpl));
    if (impl == NULL) return MAMA_STATUS_NOMEM;

    impl->mQueue    = (wombatQueue)nativeQueueHandle;
    impl->mParent   = parent;
    impl->mAction   = action;
    impl->mClosure  = closure;
    impl->mInterval = interval;
    impl->mOnTimerDestroyed = onTimerDestroyed;

    *result = (timerBridge)impl;

    timeout.tv_sec = (time_t)interval;
    timeout.tv_usec = ((interval-timeout.tv_sec) * 1000000.0);
    if (0 != createTimer (&impl->mTimerElement,
                          gTimerHeap,
                          timerCb,
                          &timeout,
                          impl))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "%s Failed to create Avis timer.",
                  "mamaTimer_create ():");
        return MAMA_STATUS_TIMER_FAILURE;
    }

    return MAMA_STATUS_OK;
}

mama_status
avisBridgeMamaTimer_destroy (timerBridge timer)
{
    mama_status    returnStatus = MAMA_STATUS_OK;
    avisTimerImpl* impl = NULL;

    if (timer == NULL)
        return MAMA_STATUS_NULL_ARG;
    impl = (avisTimerImpl*)timer;

    impl->mAction = NULL;
    mama_log (MAMA_LOG_LEVEL_FINEST,
              "%s Entering.",
              "avisMamaTimer_destroy ():");

    if (0 != destroyTimer (gTimerHeap, impl->mTimerElement))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "%s Failed to destroy Avis timer.",
                  "avisMamaTimer_destroy ():");
        returnStatus = MAMA_STATUS_PLATFORM;
    }


    wombatQueue_enqueue (impl->mQueue, destroy_callback,
            (void*)impl, NULL);

    return returnStatus;
}

mama_status
avisBridgeMamaTimer_reset (timerBridge timer)
{
    mama_status status      = MAMA_STATUS_OK;
    avisTimerImpl* impl  = NULL;
    struct timeval timeout;

    if (timer == NULL)
        return MAMA_STATUS_NULL_ARG;
    impl = (avisTimerImpl*)timer;

    timeout.tv_sec = (time_t)impl->mInterval;
    timeout.tv_usec = ((impl->mInterval-timeout.tv_sec) * 1000000.0);

    if (timer == NULL)
        return MAMA_STATUS_NULL_ARG;
    impl = (avisTimerImpl*)timer;

    if (0 != destroyTimer (gTimerHeap, impl->mTimerElement))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "%s Failed to destroy Avis timer.",
                  "avisMamaTimer_destroy ():");
        avisBridgeMamaTimer_destroy (timer);
        status = MAMA_STATUS_PLATFORM;
    }
    else
    {
        if (0 != createTimer (&impl->mTimerElement,
                              gTimerHeap,
                              timerCb,
                              &timeout,
                              impl))
        {
            mama_log (MAMA_LOG_LEVEL_ERROR,
                  "%s Failed to create Avis timer.",
                  "mamaTimer_create ():");
            status = MAMA_STATUS_PLATFORM;
        }
    }
    return status;
}

mama_status
avisBridgeMamaTimer_setInterval (timerBridge  timer,
                                mama_f64_t   interval)
{
    avisTimerImpl* impl  = NULL;

    if (timer == NULL)
        return MAMA_STATUS_NULL_ARG;
    impl = (avisTimerImpl*)timer;

    impl->mInterval = interval;

   return  avisBridgeMamaTimer_reset (timer);
}

mama_status
avisBridgeMamaTimer_getInterval (timerBridge    timer,
                                mama_f64_t*    interval)
{
    avisTimerImpl* impl  = NULL;

    if (timer == NULL)
        return MAMA_STATUS_NULL_ARG;
    impl = (avisTimerImpl*)timer;

    *interval = impl->mInterval;
    return MAMA_STATUS_OK;
}
