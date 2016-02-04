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
public class MamaPublisherTest extends TestCase implements MamaStartBackgroundCallback, MamaThrottleCallback,
                                                           MamaPublisherCallback,
                                                           MamaTransportListener
{
    /* ****************************************************** */
    /* Protected Member Variables. */
    /* ****************************************************** */
    MamaBridge bridge;
    MamaTransport transport;

    int onCreates = 0;
    int onDestroys = 0;
    int onErrors = 0;

    boolean connected = false;
    
    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Override
    protected void setUp()
    {        
        try
        {
            onCreates = 0;
            onDestroys = 0;
            onErrors = 0;
            connected = false;

            // Load the bridge
            bridge = Mama.loadBridge(Main.GetBridgeName());
        
            // Open mama
            Mama.open();

            transport = new MamaTransport();
            transport.addTransportListener(this);
            transport.addTransportTopicListener(new MamaPublisherTransportTopicListener());
            transport.create(Main.GetTransportName(), bridge);        

            int waits = 0;
            while (!connected)
            {
                Thread.sleep(1000);
                if (waits++ > 10)
                {
                    Assert.fail("Transport did not connect");
                }
            }

            Thread.sleep(4000);            // TODO use transport onCreate() here

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

    // Transport Listener
    @Override
    public void onDisconnect(short cause, final Object platformInfo)
    {
    }

    @Override
    public void onReconnect(short cause, final Object platformInfo)
    {
    }

    @Override
    public void onQuality (short cause, final Object platformInfo)
    {
    }

    @Override
    public void onConnect (short cause, final Object platformInfo)
    {
        connected = true;
    }

    @Override
    public void onAccept (short cause, final Object platformInfo)
    {
    }

    @Override
    public void onAcceptReconnect (short cause, final Object platformInfo)
    {
    }

    @Override
    public void onPublisherDisconnect (short cause, final Object platformInfo)
    {
    }

    @Override
    public void onNamingServiceConnect (short cause, final Object platformInfo)
    {
    }

    @Override
    public void onNamingServiceDisconnect (short cause, final Object platformInfo)
    {
    }

    // Transport Topic Listener
    class MamaPublisherTransportTopicListener extends MamaTransportTopicListener
    {
        @Override
        public void onTopicPublishError(String topic, final Object platformInfo)
        {
            onErrors++;
        }

        @Override
        public void onTopicPublishErrorNotEntitled(String topic, final Object platformInfo)
        {
            onErrors++;
        }

        @Override
        public void onTopicPublishErrorBadSymbol(String topic, final Object platformInfo)
        {
            onErrors++;
        }
    }

    // Publisher Callbacks
    @Override
    public void onCreate(MamaPublisher pub)
    {
        onCreates++;
    }

    @Override
    public void onDestroy(MamaPublisher pub)
    {
        onDestroys++;
    }

    @Override
    public void onError(MamaPublisher pub, short status, String info)
    {
        onErrors++;
    }

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
                p.destroy();
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
                    p.destroy();
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
            Thread.sleep(250);
        }
        catch (Exception e)
        {
            System.err.println("testPublisherDestroy: " + e.toString());
            Assert.fail("Unexpected exception");
        }
    }

    public void testPublisherCallbacks()
    {
        try
        {
            MamaMsg msg = new MamaMsg();
            msg.addU8(MamaReservedFields.MsgType.getName(), MamaReservedFields.MsgType.getId(), MamaMsgType.TYPE_INITIAL);
            msg.addU8(MamaReservedFields.MsgStatus.getName(), MamaReservedFields.MsgStatus.getId(), MamaMsgStatus.STATUS_OK);
            msg.addString("MdFeedGroup", 13, "MdFeedGroup Field");

            MamaQueueGroup queueGroup = new MamaQueueGroup(1, bridge);

            int numPublishers = 100;

            for (int i = 0; i < numPublishers; i++)
            {
                MamaPublisher p = new MamaPublisher();
                p.create(transport, queueGroup.getNextQueue(), Main.GetSymbol(), Main.GetSource(), this, null);
                p.send(msg);
                p.destroy();
                
                Thread.sleep(100);
            }

            Thread.sleep(1000);

            Assert.assertEquals(onCreates, numPublishers);
            Assert.assertEquals(onErrors, 0);
            Assert.assertEquals(onDestroys, numPublishers);
        }
        catch (Exception e)
        {
            System.err.println("testPublisherDestroy: " + e.toString());
            Assert.fail("Unexpected exception");
        }
    }

    public void testPublisherCallbacksBadSource()
    {
        try
        {
            MamaMsg msg = new MamaMsg();
            msg.addU8(MamaReservedFields.MsgType.getName(), MamaReservedFields.MsgType.getId(), MamaMsgType.TYPE_INITIAL);
            msg.addU8(MamaReservedFields.MsgStatus.getName(), MamaReservedFields.MsgStatus.getId(), MamaMsgStatus.STATUS_OK);
            msg.addString("MdFeedGroup", 13, "MdFeedGroup Field");

            MamaQueueGroup queueGroup = new MamaQueueGroup(1, bridge);

            int numPublishers = 100;

            for (int i = 0; i < numPublishers; i++)
            {
                MamaPublisher p = new MamaPublisher();
                p.create(transport, queueGroup.getNextQueue(), Main.GetSymbol(), Main.GetBadSource(), this, null);
                p.send(msg);
                p.destroy();
                
                Thread.sleep(100);
            }

            Thread.sleep(1000);

            Assert.assertEquals(onCreates, numPublishers);
            Assert.assertEquals(onErrors, numPublishers);
            Assert.assertEquals(onDestroys, numPublishers);
        }
        catch (Exception e)
        {
            System.err.println("testPublisherDestroy: " + e.toString());
            Assert.fail("Unexpected exception");
        }
    }
}

