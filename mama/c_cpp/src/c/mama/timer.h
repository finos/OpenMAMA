/* $Id: timer.h,v 1.15.6.1.2.1.4.3 2011/08/10 14:53:28 nicholasmarriott Exp $
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

#ifndef WTimerH__
#define WTimerH__

#include <mama/status.h>
#include <mama/types.h>
#include <mama/queue.h>

#if defined( __cplusplus )
extern "C"
{
#endif /* defined( __cplusplus ) */

/**
 * Prototype for callback invoked by timer.
 *
 * @param timer The timer handle.
 * @param closure Caller supplied closure.
 */
typedef void (MAMACALLTYPE *mamaTimerCb)(mamaTimer timer, void *closure); 

/**
 * Create a repeating timer. Since the mamaTimer relies on the timer mechanism of the
 * underlying middleware, the resolution of the timer is also dependent on the
 * middleware. Consult your middleware documentation for details.
 *
 * The callback is invoked repeatedly at the specified interval until the timer
 * is destroyed.
 *
 * @param result A pointer to the timer handle.
 * @param queue  The queue from which the timer event will be dispatched.
 * @param action The callback to be invoked after the interval.
 * @param closure The closure that is passed to the callback.
 * @param interval: The interval in seconds.
 */
MAMAExpDLL
extern mama_status 
mamaTimer_create(mamaTimer*   result,
                 mamaQueue    queue,
                 mamaTimerCb  action, 
                 mama_f64_t   interval,
                 void*        closure);

/**
 * Create a repeating timer. Since the mamaTimer relies on the timer mechanism of the
 * underlying middleware, the resolution of the timer is also dependent on the
 * middleware. Consult your middleware documentation for details.
 *
 * The callback is invoked repeatedly at the specified interval until the timer
 * is destroyed.
 *
 * @param result A pointer to the timer handle.
 * @param queue  The queue from which the timer event will be dispatched.
 * @param action The callback to be invoked after the interval.
 * @param onTimerDestroyed This callback will be invoked whenever the timer is destroyed,
 *                         can be NULL.
 * @param closure The closure that is passed to the callback.
 * @param interval: The interval in seconds.
 */
MAMAExpDLL
extern mama_status
mamaTimer_create2(mamaTimer*   result,
                  mamaQueue    queue,
                  mamaTimerCb  action, 
                  mamaTimerCb  onTimerDestroyed, 
                  mama_f64_t   interval,
                  void*        closure);

/**
 * Allocate a repeating timer. 
 *
 * @param result A pointer to the timer handle.
 * @param queue  The queue from which the timer event will be dispatched.
 */
MAMAExpDLL
extern mama_status 
mamaTimer_allocate(mamaTimer*   result,
                   mamaQueue    queue);

/**
 * Allocate a repeating timer. 
 *
 * @param result A pointer to the timer handle.
 * @param queue  The queue from which the timer event will be dispatched.
 * @param onTimerDestroyed Callback will be invoked whenever the timer has been
 *                         completely destroyed.
 */
MAMAExpDLL
extern mama_status
mamaTimer_allocate2(mamaTimer*   result,
                    mamaQueue    queue,
                    mamaTimerCb  onTimerDestroyed);

/**
 * Start a repeating timer created using allocate 
 *
 * The callback is invoked repeatedly at the specified interval until the timer
 * is destroyed.
 *
 * @param result The timer handle returned from allocate.
 * @param queue  The queue from which the timer event will be dispatched.
 * @param action The callback to be invoked after the interval.
 * @param closure The closure that is passed to the callback.
 * @param interval: The interval in seconds.
 */    
MAMAExpDLL
extern mama_status
mamaTimer_start(mamaTimer   result,
                mamaTimerCb  action, 
                mama_f64_t   interval,
                void*        closure);
    
/**
 * Destroy the timer.
 * This function must be called from the same thread dispatching on the
 * associated event queue unless both the default queue and dispatch queue are
 * not actively dispatching.
 * Note that this function is asynchronous and is only guaranteed to have finished
 * whenever the onTimerDestroyed function passed to the mamaTimer_create2 has been
 * called.
 * @param timer The mamaTimer to be destroyed.
 */
MAMAExpDLL
extern mama_status
mamaTimer_destroy(mamaTimer timer);

/**
 * Reset the timer to the beginning of the interval.
 *
 * @param timer The mamaTimer to be reset.
 */
MAMAExpDLL
extern mama_status
mamaTimer_reset(mamaTimer timer);

/**
 * Set the timer to use a different interval (and reset to the
 * beginning of that interval).
 *
 * @param timer The mamaTimer to change the interval.
 * @param interval The new interval for the timer.
 */
MAMAExpDLL
extern mama_status
mamaTimer_setInterval(mamaTimer   timer,
                      mama_f64_t  interval);

/**
 * Get the current timer interval.
 *
 * @param timer The mamaTimer.
 * @param interval Address of the location where the interval will be written.
 */
MAMAExpDLL
extern mama_status
mamaTimer_getInterval(const mamaTimer   timer,
                      mama_f64_t*       interval);

/**
 * Return the <code>mamaQueue</code> for this timer.
 *
 * @param timer The timer.
 * @param queue  A pointer to hold the queue.
 */
MAMAExpDLL
extern mama_status 
mamaTimer_getQueue (const mamaTimer timer,
                    mamaQueue*      queue);
        
#if defined( __cplusplus )
}
#endif /* defined( __cplusplus ) */

#endif /* WTimerH__ */
