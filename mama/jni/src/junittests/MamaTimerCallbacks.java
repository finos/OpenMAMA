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

import java.nio.*;
import junit.framework.*;
import com.wombat.mama.*;

/**
 *
 * This class will test the MamaTimer callbacks.
 */
public class MamaTimerCallbacks extends TestCase
{
    /* ****************************************************** */
    /* Protected Member Variables. */
    /* ****************************************************** */

    // The mama bridge
    private MamaBridge mBridge;
    
    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Override
    protected void setUp()
    {
        // Load the bridge
        mBridge = Mama.loadBridge("wmw");

        // Open mama
        Mama.open();
    }

    @Override
    protected void tearDown()
    {
        // Close mama
        Mama.close();
    }

    /* ************************************************************************* */
    /* Private Classes - Timer */
    /* ************************************************************************* */

    public class TestTimerCallback implements MamaTimerCallback
    {
        // Private member variables
        private MamaBridge mBridge;

         public TestTimerCallback(MamaBridge bridge)
         {
             mBridge = bridge;
         }

         public void onTimer (MamaTimer timer)
         {
             // Stop on the first message
             Mama.stop(mBridge);
         }

        public void onDestroy (MamaTimer timer)
        {
        }
    };

    public class TestTimerCallback_RecreateOnTick implements MamaTimerCallback
    {
        // Private member variables
        private MamaBridge mBridge;
        private TestTimerCallback mTestCallback;

    
         public TestTimerCallback_RecreateOnTick(MamaBridge bridge)
         {
            mBridge = bridge;
            mTestCallback = new TestTimerCallback(bridge);
         }

         public void onTimer(MamaTimer timer)
         {
             // Destroy the timer
             timer.destroy();

             // Get the default queue
             MamaQueue queue = Mama.getDefaultQueue(mBridge);

             /* Recreate the timer using the normal tick callback meaning that mama_stop
              * will be called during the next tick.
              */
             timer.create(queue, mTestCallback, 1);
         }

        public void onDestroy (MamaTimer timer)
        {
        }
    };

    public class TestTimerCallback_RecreateOnDestroy implements MamaTimerCallback
    {
        // Private member variables
        private MamaBridge mBridge;
        private TestTimerCallback mTestCallback;

         public TestTimerCallback_RecreateOnDestroy(MamaBridge bridge)
         {
             mBridge = bridge;
             mTestCallback = new TestTimerCallback(bridge);
         }

         public void onTimer (MamaTimer timer)
         {
             // Destroy the timer
             timer.destroy();
         }

         public void onDestroy(MamaTimer timer)
         {
             // Get the default queue
             MamaQueue queue = Mama.getDefaultQueue(mBridge);

             /* Recreate the timer using the normal tick callback meaning that mama_stop
              * will be called during the next tick.
              */
             timer.create(queue, mTestCallback, 1);
         }
    };

    /* ************************************************************************* */
    /* Test Functions - Timer */
    /* ************************************************************************* */

    public void testTimer()
    {
        // Create a callback object
        TestTimerCallback testCallback = new TestTimerCallback(mBridge);

        // Allocate a timer
        MamaTimer timer = new MamaTimer();

        // Get the default queue
        MamaQueue queue = Mama.getDefaultQueue(mBridge);

        // Create the timer
        timer.create(queue, testCallback, 1);

        // Process messages until the first message is received
        Mama.start(mBridge);

        // Destroy the timer
        timer.destroy();

        // Process messages until the destroy is received
        Mama.start(mBridge);
    }

    public void testTimerRecreateOnDestroy()
    {
        // Create a callback object
        TestTimerCallback_RecreateOnDestroy testCallback = new TestTimerCallback_RecreateOnDestroy(mBridge);

        // Allocate a timer
        MamaTimer timer = new MamaTimer();
            
        // Get the default queue
        MamaQueue queue = Mama.getDefaultQueue(mBridge);

        // Create the timer
        timer.create(queue, testCallback, 1);

        // Process messages until the first message is received
        Mama.start(mBridge);

        // Destroy the timer
        timer.destroy();
    }

    public void testTimerRecreateOnTick()
    {
        // Create a callback object
        TestTimerCallback_RecreateOnTick testCallback = new TestTimerCallback_RecreateOnTick(mBridge);

        // Allocate a timer
        MamaTimer timer = new MamaTimer();

        // Get the default queue
        MamaQueue queue = Mama.getDefaultQueue(mBridge);

        // Create the timer
        timer.create(queue, testCallback, 1);

        // Process messages until the first message is received
        Mama.start(mBridge);

        // Destroy the timer
        timer.destroy();
    }
}
