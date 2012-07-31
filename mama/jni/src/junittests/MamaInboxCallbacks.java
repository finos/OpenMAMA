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
 * This class will test MamaInbox callbacks.
 */
public class MamaInboxCallbacks extends TestCase
{
    /* ****************************************************** */
    /* Protected Member Variables. */
    /* ****************************************************** */

    // The mama bridge
    private MamaBridge mBridge;

    // The mama transport
    private MamaTransport mTransportBasic;

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

        // Create the transport        
        mTransportBasic = new MamaTransport();        
        mTransportBasic.create("sub_wmw_5", mBridge);        
    }

    @Override
    protected void tearDown()
    {
        // Destroy the transport        
        mTransportBasic.destroy();
        mTransportBasic = null;

        // Close mama
        Mama.close();
    }

    /* ************************************************************************* */
    /* Private Classes - Inbox */
    /* ************************************************************************* */

    public class TestInboxCallback implements MamaInboxCallback
    {
        // Private member variables
        private MamaBridge mBridge;

        public TestInboxCallback(MamaBridge bridge)
        {
            mBridge = bridge;
        }

        public void onMsg (
            MamaInbox         inbox,
            MamaMsg     msg)
        {
            Mama.stop(mBridge);
        }

        public void onDestroy(MamaInbox inbox)
        {
            Mama.stop(mBridge);
        }
    };

    public class TestInboxCallback_RecreateOnMsg implements MamaInboxCallback
    {
        // Private member variables
        private MamaBridge mBridge;
        private MamaTransport mTransport;
        private TestInboxCallback mTestCallback;

         public TestInboxCallback_RecreateOnMsg(MamaBridge bridge, MamaTransport transport)
         {
            mBridge = bridge;
            mTransport = transport;
            mTestCallback = new TestInboxCallback(bridge);
         }

        public void onMsg(MamaInbox inbox, MamaMsg msg)
        {
            // Destroy the inbox
            inbox.destroy();

            // Get the default queue
            MamaQueue queue = Mama.getDefaultQueue(mBridge);

            // Recreate the inbox
            inbox.create(mTransport, queue, mTestCallback);

            // Create a message
            MamaMsg message = new MamaMsg();
            message.addI32("field", 1, 222);

            // Create a publisher
            MamaPublisher publisher = new MamaPublisher();
            publisher.create(mTransport, "MAMA_INBOUND_TOPIC");

            // Send the inbox request
            publisher.sendFromInbox(inbox, message);
        }

        public void onDestroy(MamaInbox inbox)
        {
        }
    };

    public class TestInboxCallback_RecreateOnDestroy implements MamaInboxCallback
    {
        // Private member variables
        private MamaBridge mBridge;
        private MamaTransport mTransport;
        private TestInboxCallback mTestCallback;

         public TestInboxCallback_RecreateOnDestroy(MamaBridge bridge, MamaTransport transport)
         {
            mBridge = bridge;
            mTransport = transport;
            mTestCallback = new TestInboxCallback(bridge);
         }
         
         public void onMsg(MamaInbox inbox, MamaMsg msg)
         {
             // Destroy the inbox
             inbox.destroy();
         }

         public void onDestroy(MamaInbox inbox)
         {
             // Get the default queue
             MamaQueue queue = Mama.getDefaultQueue(mBridge);

             // Recreate the inbox
             inbox.create(mTransport, queue, mTestCallback);

             // Create a message
             MamaMsg message = new MamaMsg();
             message.addI32("field", 1, 222);

             // Create a publisher
             MamaPublisher publisher = new MamaPublisher();
             publisher.create(mTransport, "MAMA_INBOUND_TOPIC");

             // Send the inbox request
             publisher.sendFromInbox(inbox, message);
         }
    };

    /* ************************************************************************* */
    /* Test Functions - Inbox */
    /* ************************************************************************* */

    public void testInbox()
    {
        // Create a callback object
        TestInboxCallback testCallback = new TestInboxCallback(mBridge);

        // Allocate an inbox
        MamaInbox inbox = new MamaInbox();

            // Get the default queue
        MamaQueue queue = Mama.getDefaultQueue(mBridge);

        // Create the inbox
        inbox.create(mTransportBasic, queue, testCallback);

        // Create a message
        MamaMsg message = new MamaMsg();        
        message.addI32("field", 1, 222);

        // Create a publisher
        MamaPublisher publisher = new MamaPublisher();
        publisher.create(mTransportBasic, "MAMA_INBOUND_TOPIC");

        // Send the inbox request
        publisher.sendFromInbox(inbox, message);

        // Process messages until the first message is received
        Mama.start(mBridge);

        // Destroy the inbox
        inbox.destroy();

        // Process messages until the destroy is received
        Mama.start(mBridge);
    }

    public void testInboxRecreateOnDestroy()
    {
        // Create a callback object
        TestInboxCallback_RecreateOnDestroy testCallback = new TestInboxCallback_RecreateOnDestroy(mBridge, mTransportBasic);

        // Allocate an inbox
        MamaInbox inbox = new MamaInbox();

        // Get the default queue
        MamaQueue queue = Mama.getDefaultQueue(mBridge);

        // Create the inbox
        inbox.create(mTransportBasic, queue, testCallback);

        // Create a message
        MamaMsg message = new MamaMsg();
        message.addI32("field", 1, 222);

        // Create a publisher
        MamaPublisher publisher = new MamaPublisher();
        publisher.create(mTransportBasic, "MAMA_INBOUND_TOPIC");

        // Send the inbox request
        publisher.sendFromInbox(inbox, message);

        // Process messages until the first message is received
        Mama.start(mBridge);

        // Destroy the inbox
        inbox.destroy();
    }

    public void testInboxRecreateOnMsg()
    {
        // Create a callback object
        TestInboxCallback_RecreateOnMsg testCallback = new TestInboxCallback_RecreateOnMsg(mBridge, mTransportBasic);
        
        // Allocate a timer
        MamaInbox inbox = new MamaInbox();

        // Get the default queue
        MamaQueue queue = Mama.getDefaultQueue(mBridge);

        // Create the inbox
        inbox.create(mTransportBasic, queue, testCallback);

        // Create a message
        MamaMsg message = new MamaMsg();
        message.addI32("field", 1, 222);

        // Create a publisher
        MamaPublisher publisher = new MamaPublisher();
        publisher.create(mTransportBasic, "MAMA_INBOUND_TOPIC");

        // Send the inbox request
        publisher.sendFromInbox(inbox, message);

        // Process messages until the first message is received
        Mama.start(mBridge);

        // Destroy the inbox
        inbox.destroy();
    }
}
