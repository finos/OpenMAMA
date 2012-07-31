/* $Id: throttle.h,v 1.8.24.4 2011/09/01 16:34:38 emmapollock Exp $
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

#ifndef ThrottleH__
#define ThrottleH__

#include "mama/status.h"
#include "list.h"

/**
 * processes messages at a specified rate (msgs/sec). if the rate is 0 the 
 * messages are sent as quickly as possible.
 * messages dispatched for throttling must provide an action callback.
 *
 */


#if defined(__cplusplus)
extern "C" {
#endif

typedef struct wombatThrottle_* wombatThrottle;

typedef void (*throttleCb)(void *closure1, void *closure2 );

typedef struct actionHandle* wombatThrottleAction;



/**
 *  allocate a new wombatThrottle. throttle is enabled by default.
 */
mama_status
wombatThrottle_allocate (wombatThrottle *throttle);

/**
 * crete a new wombatThrottle.
 */
mama_status
wombatThrottle_create (wombatThrottle throttle, mamaQueue queue);


/**
 * destroy this wombatThrottle and release any * resources associated with it.
 * destroy () waits for the dispatchThread to complete its current cycle before
 * killing it. Therefore, destroy () might take a long time to complete in some
 * cases. However, any messages remaining in the queue after the current
 * cycle are NOT processed.
 */
mama_status
wombatThrottle_destroy (wombatThrottle throttle );


/**
 * Set the current throttle rate in messages/second. A rate less than or
 * equal to 0 disables throttling, and messages are sent as quickly as
 * possible. In this way, setRate () acts as a toggle for throttling.
 * 
 * If rate is set to 0 while there are still messages waiting for dispatch,
 * those messages will be processed without delay by the dispatchThread.
 *
 * @param rate  approximate desired throttle rate in msgs (jobs)/second
 */
void
wombatThrottle_setRate (wombatThrottle throttle, double rate );


/**
 * Return the current throttle rate in messages per second. If throttling
 * is disabled, this method returns 0.
 *
 * @return the rate. 0 if throttling is disabled.
 */
double
wombatThrottle_getRate (wombatThrottle throttle );


/**
 * Dispatch a throttled task. If throtle is disabled (mRate == 0), the task
 * is run immediately. Otherwise, the task is placed on the queue and run in
 * the order it was added according to the throttle rate.
 * 
 * Specifying an owner allows tasks to be removed before they are dispatched.
 * This is useful if an object is being destroyed or closed.
 *
 * @param owner The owner of the task.
 * @param actionCB Function to invoke when the task is dispatched.
 * @param closure1 User-defined 1st parameter to actionCB function
 * @param closure2 User-defined 2nd parameter to actionCB function
 * @param immediate. If true put the action on the front of the throttle
 * queue.
 */
mama_status
wombatThrottle_dispatch (wombatThrottle         throttle,
                         void*                  owner, 
                         throttleCb             action, 
                         void*                  closure1, 
                         void*                  closure2, 
                         int                    immediate,
                         wombatThrottleAction*  handle);

/**
 * Removed the messages in the queue for the specified owner.
 *
 * @param owner  The message's "owner", or identifying void*
 */
mama_status
wombatThrottle_removeMessagesForOwner (wombatThrottle throttle, void *owner );

/**
 * Removed the messages in the queue contained in the specified list of
 * pointers to wombatThrottleActions.
 *
 */
mama_status
wombatThrottle_removeMessagesFromList (wombatThrottle throttle, wList list );

/**
 * Lock the throttle. Must be paired with call to wombatThrottle_unlock.
 *
 * The lock is recursive. 
 */
void 
wombatThrottle_lock (wombatThrottle throttle );

/**
 * Unlock the throttle. Must be paired with call to wombatThrottle_lock.
 *
 * The lock is recursive. 
 */
void 
wombatThrottle_unlock (wombatThrottle throttle );

/**
 * Set how frequently the throttle runs.
 *
 * @param interval Time in seconds between runs.
 */
mama_status
wombatThrottle_setInterval (wombatThrottle throttle, double interval);

/** 
 * Remove an action.
 * 
 * @parma action The action to remove.
 */
mama_status
wombatThrottle_removeAction (wombatThrottle throttle, 
                             wombatThrottleAction action);

#if defined(__cplusplus)
}
#endif

#endif /* ThrottleH__ */
