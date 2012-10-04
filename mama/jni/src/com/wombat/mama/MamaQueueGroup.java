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

/**
 * A simple class for distributing events across multiple queues in a round
 * robin.
 */
public class MamaQueueGroup
{
    private final MamaQueue [] myQueues;
    private final MamaDispatcher[] myDispatchers;
    private int myCurQueue = 0;

    private MamaBridge myBridge = null;

    /**
     * Create a <code>MamaQueueGroup</code> with the specified number of
     * <code>MamaQueue</code>s. If the number of queues is 0, 
     * <code>getNextQueue()</code> returns the default queue for the bridge.
     *
     * @param numberOfQueues The number of queues to create.
     *
     */
    public MamaQueueGroup (int numberOfQueues, MamaBridge bridge)
    {
        if (numberOfQueues > 0)
        {
            myQueues = new MamaQueue[numberOfQueues];
            myDispatchers = new MamaDispatcher[numberOfQueues];
            for ( int i = 0; i < myQueues.length; i++ )
            {
                myQueues[i] = new MamaQueue ();
                myQueues[i].create (bridge);
                myDispatchers[i] = new MamaDispatcher( );
                myDispatchers[i].create (myQueues[i]);
            }
        }
        else
        {
            myQueues = null;
            myDispatchers = null;
        }
        myBridge = bridge;
    }

    /**
     * Destroy the queue, this function should only be called if there are no
     * open objects against the queue. Use one of the other destroy functions
     * to block until all objects have been cleaned up.
     *
     * @exception MamaException will be thrown if there are open objects.
     */
    public void destroy()
    {
        if(myQueues != null)
        {
            for ( int nextQueue = 0; nextQueue < myQueues.length; nextQueue++ )
            {
                myQueues[nextQueue].destroy();
            }
        }
    }

    /**
     * Destroy a queue. Note that the queue can only be destroyed if all of the objects created
     * on it, (timers, subscriptions etc), have been destroyed. This function will block until
     * all of the objects have been destroyed and will then destroy the queue.
     */
    public void destroyWait()
    {
        if(myQueues != null)
        {
            for ( int nextQueue = 0; nextQueue < myQueues.length; nextQueue++ )
            {
                myQueues[nextQueue].destroyWait();
            }
        }
    }

    /**
     * Destroy a queue. Note that the queue can only be destroyed if all of the objects created
     * on it, (timers, subscriptions etc), have been destroyed. This function will block until
     * all of the objects have been destroyed and will then destroy the queue.
     *
     * @param timeout The time to block for in milliseconds.
     * @exception MamaException will be thrown if the timeout elapses.
     */
    public void destroyTimedWait(long timeout)
    {
        if(myQueues != null)
        {
            for ( int nextQueue = 0; nextQueue < myQueues.length; nextQueue++ )
            {
                myQueues[nextQueue].destroyTimedWait(timeout);
            }
        }
    }

    /**
     * Return the next queue. The queues are returned in a round robin.
     *
     * @return The next queue.
     */
    public MamaQueue getNextQueue()
    {
        return myQueues == null ? Mama.getDefaultQueue (myBridge)
                                : myQueues[myCurQueue++ % myQueues.length];
    }
    
    
    public void stopDispatch ()
    {
        if(myQueues != null)
        {
            for ( int i = 0; i < myQueues.length; i++ )
            {
                if (myDispatchers[i]!= null)
                {
                    myDispatchers[i].destroy();
                    myDispatchers[i]=null;
                }
            }
        }
    }
    
    
    public void startDispatch ()
    {
        if(myQueues != null)
        {
            for ( int i = 0; i < myQueues.length; i++ )
            {
                if (myDispatchers[i]== null)
                {
                    myDispatchers[i] = new MamaDispatcher( );
                    myDispatchers[i].create (myQueues[i]);
                }
            }
        }
    }
}
