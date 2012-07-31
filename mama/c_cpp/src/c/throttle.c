/* $Id: throttle.c,v 1.35.6.1.16.3 2011/08/10 14:53:26 nicholasmarriott Exp $
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

/**
 * Processes messages at a specified rate (msgs/sec). If the rate is 0 the 
 * messages are sent as quickly as possible.
 * Messages dispatched for throttling must provide an action callback.
 *
 */

#include <string.h>

#include "mama/mama.h"
#include "mama/timer.h"
#include "throttle.h"
#include "list.h"
#include "wlock.h"
#include "wombat/wincompat.h"

#define self ((wombatThrottleImpl*)(throttle))

#define WOMBAT_THROTTLE_DEFAULT_RATE    1000.0 /* msgs/sec */ 


typedef struct MsgProperties_
{
    void*      mOwner;
    throttleCb mActionCb;
    void*      mClosure1;
    void*      mClosure2;

} MsgProperties;

typedef struct wombatThrottle_
{
    wList mMsgQueue; 
    double mRate; /* msgs/second */
    int mMsgsSentThisInterval; 
    int mMessagesPerInterval; 
    double mInterval; /* in seconds */
    mamaQueue mQueue;
    mamaTimer mTimer;

    /* This lock is used to syncrhonize calls to timer_destroy to prevent
     * a race condition.
     */
    wLock mTimerLock;

} wombatThrottleImpl;



/* forward declarations */
static void MAMACALLTYPE wombatThrottle_timerCB (mamaTimer timer, void *throttle);
static void wombatThrottle_cleanUp (wombatThrottle throttle);
static int wombatThrottle_sendQueuedMessage (wombatThrottle throttle);

mama_status
wombatThrottle_allocate (wombatThrottle *throttle)
{
    wombatThrottleImpl *impl =
        (wombatThrottleImpl*) malloc (sizeof (wombatThrottleImpl));

    if (impl == NULL)
        return (MAMA_STATUS_NOMEM); 
    
    memset (impl, 0, sizeof (wombatThrottleImpl));

    impl->mMsgQueue = list_create (sizeof (MsgProperties));
   
    impl->mInterval = 0.1;

    /* Create the timer lock. */
    impl->mTimerLock = wlock_create ();

    /* dispatchThread wait ()s immediately after creation because 
     * mRate == 0.0  Call setRate () last to signal () it.  
     * setRate () effectively toggles throttle on and off */
    wombatThrottle_setRate (impl, WOMBAT_THROTTLE_DEFAULT_RATE);
    
    *throttle = (wombatThrottle)impl;

    return MAMA_STATUS_OK;
}


mama_status
wombatThrottle_create (wombatThrottle throttle, mamaQueue queue)
{
    self->mQueue = queue;
    return MAMA_STATUS_OK;
}

mama_status
wombatThrottle_destroy (wombatThrottle throttle)
{
    if (self == NULL)
        return MAMA_STATUS_OK;

    /* Acquire the lock before destroying the timer. */
    wlock_lock (self->mTimerLock);

    /* Destroy the timer. */
    if (self->mTimer != NULL)
    {
        mamaTimer_destroy (self->mTimer);
        self->mTimer = NULL;     
    }     

    /* Release the lock. */
    wlock_unlock (self->mTimerLock);

        /* Destroy the timer lock. */
    if (self->mTimerLock != NULL)
    {
        wlock_destroy (self->mTimerLock);
        self->mTimerLock = NULL;
    }

    
    wombatThrottle_cleanUp (throttle);

    return MAMA_STATUS_OK;
}

mama_status
wombatThrottle_setInterval (wombatThrottle throttle, double interval)
{
    if (self == NULL) return MAMA_STATUS_NULL_ARG;
    self->mInterval = interval;
    return MAMA_STATUS_OK;
}

static void
wombatThrottle_cleanUp (wombatThrottle throttle)
{
    list_destroy (self->mMsgQueue, NULL, NULL);
    bzero (self, sizeof (wombatThrottleImpl));
    free (self);
}



double
wombatThrottle_getRate (wombatThrottle throttle)
{
    return self->mRate;
}


void
wombatThrottle_setRate (wombatThrottle throttle, double rate)
{
    if (rate < 0.0) rate = 0.0;
    self->mRate = rate;

    /* calc interval size (in microsecs) and #msgs/interval based on mRate.
     * Usually the interval size (mInterval) will be .1 sec.
     */ 
    if (self->mRate == 0.0)
    {
        /* mRate == 0.0 means throttle is disabled.
         * We want to process any/all remaining jobs without throttling. 
         * The dispatchThread will do so when it sees mRate == 0.0 
         * However, if the thread is in the middle of a normal  
         * cycle, it will complete the cycle and possibly sleep before
         * it notices mRate == 0. Thus, disabling throttle won't always 
         * have an instant effect. This could be easily changed. */ 
        return;
    }

    if (self->mRate < 10)
    {
        self->mInterval = 1.0; /* 1 second */
        self->mMessagesPerInterval = self->mRate;
    }
    else
    {
        self->mMessagesPerInterval = (int) (self->mRate * self->mInterval);
        if (self->mMessagesPerInterval <= 0) self->mMessagesPerInterval = 1;
    }
}

mama_status
wombatThrottle_dispatch (wombatThrottle throttle, 
                         void *owner, 
                         throttleCb actionCb, 
                         void *closure1, 
                         void *closure2,
                         int immediate,
                         wombatThrottleAction *handle)
{
    MsgProperties* info = NULL;
    
    if (self->mRate <= 0.0)
    {
        /* throttle is not in use, execute immediately */
        mama_log (MAMA_LOG_LEVEL_FINEST, "wombatThrottle_dispatch (): "
                  "no throttle; triggering action.");
        actionCb (closure1, closure2);
    }
    else
    {
        list_lock (self->mMsgQueue);
         info = (MsgProperties*)list_allocate_element (self->mMsgQueue);

        /* Acquire the lock before creating the timer. */
        wlock_lock (self->mTimerLock);

        /* Create the timer. */
        if (self->mTimer == NULL)
        {
            mamaTimer_create (&self->mTimer, 
                              self->mQueue,
                              wombatThrottle_timerCB,
                              self->mInterval,
                              self);

            mama_log (MAMA_LOG_LEVEL_FINEST,
                      "wombatThrottle_dispatch (): "
                      "creating throttle timer (%p).", self->mTimer);
        }

        /* Release the lock. */
        wlock_unlock (self->mTimerLock);

        if (info == NULL)
        {
            list_unlock (self->mMsgQueue);
            return (MAMA_STATUS_NOMEM);
        }

        info->mOwner = owner;
        info->mActionCb = actionCb;
        info->mClosure1 = closure1;
        info->mClosure2 = closure2;
        if (handle != NULL)
        {
            *handle = (wombatThrottleAction)info;
        }

        if (immediate)
        {
            list_push_front (self->mMsgQueue, info);
        }
        else
        {
            list_push_back (self->mMsgQueue, info);
        }
        list_unlock (self->mMsgQueue);
    }

    return MAMA_STATUS_OK;
}

static void 
wombatThrottle_dispatchMessagesUntilQuota (wombatThrottle throttle)
{
    /* Send queued messages until we reach the limit for this 
     * interval or empty the queue. */
    while (self->mMsgsSentThisInterval++ < self->mMessagesPerInterval)
    {
        if (!wombatThrottle_sendQueuedMessage (throttle))
        {
            /* msg queue is empty, nothing left to do */
            return;
        }
    } 
}


/**
 * Removes the oldest job (message) from the queue and "runs" it. 
 *
 * @return 1 if a job was processed, 0 if the queue was empty. 
 */
static int
wombatThrottle_sendQueuedMessage (wombatThrottle throttle)
{
    MsgProperties *info;

    list_lock (self->mMsgQueue);
    if (NULL != (info = list_pop_front (self->mMsgQueue)))
    {
        /* Process a throttled message.*/
        info->mActionCb (info->mClosure1, info->mClosure2);
        list_free_element (self->mMsgQueue, info);
        list_unlock (self->mMsgQueue);
        return 1;
    }

    list_unlock (self->mMsgQueue);
    /* We did not send a message. */
    return 0;
}


typedef struct biclosure_
{
    wombatThrottleImpl *impl;
    void*     owner;
} biclosure;

static int gRemoveCount = 0;


static void
removeMessagesForOwnerCb (wList list, void *element, void *closure)
{
    MsgProperties *info = (MsgProperties*)element;
    biclosure* pair = (biclosure*)closure;

    if (pair->owner == info->mOwner)
    {
        list_remove_element (pair->impl->mMsgQueue, element);

        list_free_element (pair->impl->mMsgQueue, element);
        gRemoveCount++;
    }
}


mama_status
wombatThrottle_removeMessagesForOwner (wombatThrottle throttle, void *owner)
{
    biclosure  closure;
    
    if (NULL==throttle)
      return MAMA_STATUS_NULL_ARG;

    closure.impl     = self;
    closure.owner    = owner;

    gRemoveCount = 0;

    list_for_each (self->mMsgQueue, removeMessagesForOwnerCb,
                                                     (void*)(&closure));
    mama_log (MAMA_LOG_LEVEL_FINE, "wombatThrottle_removeMessagesForOwner (): "
              "%d Messages removed from queue.",
              gRemoveCount);

    return MAMA_STATUS_OK;
}


static void
removeMessagesFromListCb (wList list, void *element, void *throttle) 
{
    wombatThrottleAction *action = (wombatThrottleAction*)element;
    list_remove_element (self->mMsgQueue, *action);
    list_free_element (self->mMsgQueue, *action);
    gRemoveCount++;
}

mama_status
wombatThrottle_removeMessagesFromList (wombatThrottle throttle, wList list)
{
    gRemoveCount = 0;

    list_lock (self->mMsgQueue);
    list_for_each (list, removeMessagesFromListCb, self);
    mama_log (MAMA_LOG_LEVEL_FINE, "wombatThrottle_removeMessagesFromList (): "
              "%d Messages removed from queue list.",
              gRemoveCount);

    list_unlock (self->mMsgQueue);
    return MAMA_STATUS_OK;
}

mama_status
wombatThrottle_removeAction (wombatThrottle throttle, 
                             wombatThrottleAction action)
{
    list_remove_element (self->mMsgQueue, action);
    list_free_element (self->mMsgQueue, action);
    return MAMA_STATUS_OK;
}


/**
 * Runs all queued tasks up to a fixed #/cycle. The quota and cycle
 * duration are calculated in #wombatThrottle_setRate. After quota
 * is reached for the current cycle, thread sleeps for the remainder.
 * When the queue is empty, dispatchThread wait ()s. 
 */

static void MAMACALLTYPE
wombatThrottle_timerCB (mamaTimer timer, void *throttle)
{

    /* If mRate == 0, throttle has been deactivated. Finish any remaining 
     * jobs in queue, and wait. If mRate is set again before we finish 
     * purging the queue, we exit the while and don't call wait () */
    while (self->mRate == 0.0
           && wombatThrottle_sendQueuedMessage (throttle)) { }

    self->mMsgsSentThisInterval = 0;
    
    if (list_size (self->mMsgQueue) == 0)     
    {     
        /* Acquire the lock before destroying the timer. */
        wlock_lock (self->mTimerLock);

        /* Destroy the timer. */
        if (self->mTimer != NULL)     
        {     
            mamaTimer_destroy (self->mTimer);     
            self->mTimer = NULL;     
        }     

        /* Release the lock. */
        wlock_unlock (self->mTimerLock);
        
    }     

    wombatThrottle_dispatchMessagesUntilQuota (throttle);

}

void 
wombatThrottle_lock (wombatThrottle throttle)
{
    list_lock (self->mMsgQueue);
}

void 
wombatThrottle_unlock (wombatThrottle throttle)
{
    list_unlock (self->mMsgQueue);
}

