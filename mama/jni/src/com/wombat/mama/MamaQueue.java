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
* MamaQueue
*/
public class MamaQueue
{

    static
    {
        initIDs();
    }

    /*A long holding the C pointer value to the underlying 
      MamaQueue C structure */
    private long queuePointer_i = 0;

    /*Used to store pointer to the C closure used for queue monitoring*/
    private long monitorClosure_i  = 0;

    public long getPointerVal()
    {
        return queuePointer_i;
    }
    
    public MamaQueue () {};

    public native void create (MamaBridge bridge);

    public native void createUsingNative (MamaBridge bridge, Object nativeQueue);

    /**
     * Destroy the queue, this function should only be called if there are no
     * open objects against the queue. Use one of the other destroy functions
     * to block until all objects have been cleaned up.
     *
     * @exception MamaException will be thrown if there are open objects.
     */
    public native void destroy();

    /**
     * Destroy a queue. Note that the queue can only be destroyed if all of the objects created
     * on it, (timers, subscriptions etc), have been destroyed. This function will block until
     * all of the objects have been destroyed and will then destroy the queue.
     */
    public native void destroyWait();

    /**
     * Destroy a queue. Note that the queue can only be destroyed if all of the objects created
     * on it, (timers, subscriptions etc), have been destroyed. This function will block until
     * all of the objects have been destroyed and will then destroy the queue.
     *
     * @param timeout The time to block for in milliseconds.
     * @exception MamaException will be thrown if the timeout elapses.
     */
    public native void destroyTimedWait(long timeout);

    public native void dispatch();

    public native void stopDispatch();
    
    public native void enqueueEvent (MamaQueueEventCallback callback,
                                     Object closure);
    
    public native void setQueueMonitorCallback (
                                        MamaQueueMonitorCallback callback);

    public native void setHighWatermark (long highWatermark);

    public native long getHighwatermark ();

    public native void setLowWatermark (long lowWatermark);

    public native long getLowWatermark ();

    public native void setQueueName (String queueName);

    public native String getQueueName ();

    public native String getQueueBridgeName ();

    public native long getEventCount ();
    
    /*Used to cache ids for callback methods/fields*/
    private static native void initIDs();
}/*end class*/
