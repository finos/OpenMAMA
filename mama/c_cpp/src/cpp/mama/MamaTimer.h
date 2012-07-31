/* $Id$
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

#ifndef MAMA_TIMER_CPP_H__
#define MAMA_TIMER_CPP_H__

#include <mama/mamacpp.h>

namespace Wombat 
{

    /**
     * A repeating timer. The callback will be repeatedly called at the
     * specified interval until the timer is destroyed.  To restart the
     * timer after destroying it if destroy(), use create().  To reset the
     * timer to the beginning of the given interval, use reset().  To set
     * the timer to a different interval, use setInterval().
     *
     * The MAMA timer relies on underlying middleware so its resolution is
     * also dependent on the middleware.
     *
     */

    class MAMACPPExpDLL MamaTimer
    {
    public:

        MamaTimer (void);

        virtual ~MamaTimer (void);
        
       /**
         * Create a repeating timer. The interval is in seconds.
         *
         * The queue is the queue from which the timer event will be dispatched.
         *
         * @param queue The queue.
         * @param callback The callback.
         * @param interval The interval in seconds.
         * @param closure The caller supplied closure.
         */
        virtual void create (MamaQueue*          queue,
                             MamaTimerCallback*  callback,
                             mama_f64_t          interval,
                             void*               closure = NULL);

        /**
         * Destroy (stop) the timer.  
         * This function must be called from the same thread dispatching on the
         * associated event queue unless both the default queue and dispatch queue are
         * not actively dispatching.
         */
        virtual void destroy ();

        /**
         * Return whether the timer is active.
         */
        virtual bool isActive () const;

        /**
         * Reset the timer to the beginning of the interval.
         */
        virtual void reset ();

        /**
         * Set the timer to use a different interval (and reset to the
         * beginning of that interval).
         */
        virtual void setInterval (mama_f64_t intervalSeconds);

        /**
         * Get the current timer interval.
         */
        virtual mama_f64_t getInterval () const;

        /**
         * Return the callback for the timer.
         *
         * @return the callback.
         */
        virtual MamaTimerCallback* getCallback () const;

        /**
         * Return the closure for the timer.
         *
         * @return the closure.
         */
        virtual void* getClosure () const;

        // Access to C types for implementation of related classes.
        mamaTimer        getCValue    ();
        const mamaTimer  getCValue    () const;

    private:
        mamaTimer           myTimer;
        MamaTimerCallback*  myCallback;
        void *              myClosure;

        /* Private functions. */    
        static void MAMACALLTYPE onTimerDestroy (mamaTimer timer, void *closure);
        static void MAMACALLTYPE onTimerTick    (mamaTimer timer, void* closure);
    };

} // namespace Wombat
#endif // MAMA_TIMER_CPP_H__
