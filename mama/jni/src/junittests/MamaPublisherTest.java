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

package com.wombat.mama.junittests;

import junit.framework.*;
import com.wombat.mama.*;

/**
 *
 * This class will test MamaPublisher 
 */
public class MamaPublisherTest extends TestCase implements MamaStartBackgroundCallback, MamaThrottleCallback
{
    /* ****************************************************** */
    /* Protected Member Variables. */
    /* ****************************************************** */
    MamaBridge bridge;
    MamaTransport transport;
    
    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Override
    protected void setUp()
    {        
        try
        {
            // Load the bridge
            bridge = Mama.loadBridge(Main.GetBridgeName());
        
            // Open mama
            Mama.open();

            transport = new MamaTransport();
            transport.create(Main.GetTransportName(), bridge);        

            Thread.sleep(4000);            // TODO use onCreate() here

            Mama.startBackground(bridge, this);
        }
        catch (Exception e)
        {
            System.err.println("testPublisherThrottle: " + e.toString());
            Assert.fail("Unexpected exception");
        }
    }

    @Override
    protected void tearDown()
    {        
        try
        {
            // Close mama
            Mama.stop(bridge);

            transport.destroy();

            Mama.close();
        }
        catch (Exception e)
        {
            System.err.println("testPublisherThrottle: " + e.toString());
            Assert.fail("Unexpected exception");
        }
    }

    /* ****************************************************** */
    /* Test Functions. */
    /* ****************************************************** */

    @Override
    public void onStartComplete(int status)
    {
    }

    @Override
    public void onThrottledSendComplete()
    {
    }

    public void testPublisherThrottle()
    {
        try
        {
            MamaMsg msg = new MamaMsg();
            msg.addU8(MamaReservedFields.MsgType.getName(), MamaReservedFields.MsgType.getId(), MamaMsgType.TYPE_INITIAL);
            msg.addU8(MamaReservedFields.MsgStatus.getName(), MamaReservedFields.MsgStatus.getId(), MamaMsgStatus.STATUS_OK);
            msg.addString("MdFeedGroup", 13, "MdFeedGroup Field");

            for (int i = 0; i < 100; i++)
            {
                MamaPublisher p = new MamaPublisher();
                p.create(transport, Main.GetSymbol(), Main.GetSource());
                p.sendWithThrottle(msg, this);
                
                Thread.sleep(250);
            }
        }
        catch (Exception e)
        {
            System.err.println("testPublisherThrottle: " + e.toString());
            Assert.fail("Unexpected exception");
        }
    }

    public void testPublisherDestroy()
    {
        try
        {
            MamaMsg msg = new MamaMsg();
            msg.addU8(MamaReservedFields.MsgType.getName(), MamaReservedFields.MsgType.getId(), MamaMsgType.TYPE_INITIAL);
            msg.addU8(MamaReservedFields.MsgStatus.getName(), MamaReservedFields.MsgStatus.getId(), MamaMsgStatus.STATUS_OK);
            msg.addString("MdFeedGroup", 13, "MdFeedGroup Field");

            // Test destroy w/o create
            try
            {
                MamaPublisher p = new MamaPublisher();
                // p.destroy();
                Assert.fail("Expected an exception for destroy with no create");
            }
            catch (Exception e)
            {
            }

            for (int i = 0; i < 100; i++)
            {
                MamaPublisher p = new MamaPublisher();
                p.create(transport, Main.GetSymbol(), Main.GetSource());
                p.send(msg);

                if (Math.random() < 0.50)
                {
                    // 50% destroy
                    // p.destroy();
                }
                else
                {
                    // 50% let finalize destroy
                    p = null;
                }
                
                Thread.sleep(250);
            }

            // Try to make Java run GC to call finalize
            System.gc();

        }
        catch (Exception e)
        {
            System.err.println("testPublisherDestroy: " + e.toString());
            Assert.fail("Unexpected exception");
        }
    }
}

