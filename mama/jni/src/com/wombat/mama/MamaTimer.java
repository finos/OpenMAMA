/* $Id:
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

package com.wombat.mama;

/*
* Repeating timer Interface.
*/
public class MamaTimer
{
    static
    {
        initIDs();
    }

    /*A long holding the C pointer value to the underlying Timer C structure*/
    private long timerPointer_i = 0;

    /*
        A pointer to the C structure used as the internal closure. We need this
        so it can be deleted on Timer destruction.
    */
    private long closurePointer_i = 0;

    /*A closure that can be set when creating the timer*/
    private Object myClosure = null;

    public long getPointerVal()
    {
        return timerPointer_i;
    }

    /**
     * Create a timer.
     *
     * @param queue         The MamaQueue.
     * @param callback      The object implementing the callback functions.
     * @param interval      The timer interval in seconds.
     */
    public native void create (MamaQueue         queue,
                               MamaTimerCallback callback,
                               double            interval);

    /**
     * Create a timer.
     *
     * @param queue         The MamaQueue.
     * @param callback      The object implementing the callback functions.
     * @param interval      The timer interval in seconds.
     * @param closure       The closure will be passed to subsequent callback invocations
     *                      for this subscription.
     */
    public void create (MamaQueue         queue,
                        MamaTimerCallback callback,
                        double            interval,
                        Object            closure)
    {
        // Save a reference to the closure
        myClosure = closure;

        // Create the timer without a closure
        create (queue, callback, interval);
    }
    
    /* Destroy the timer. 
     * This function must be called from the same thread dispatching on the
     * associated event queue unless both the default queue and dispatch queue are
     * not actively dispatching.
     */
    public native void destroy ();

    public native void resetInterval (double interval);

    public native double getInterval ();

    public native void reset ();

    public Object getClosure ()
    {
        return myClosure;
    }
    
    /*Used to cache ids for callback methods/fields*/
    private static native void initIDs();
}
