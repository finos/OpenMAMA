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

import com.wombat.mama.*;
import com.wombat.common.*;
import org.junit.After;
import org.junit.Before;
import org.junit.Test;

/**
 *
 * This class will test the MamaMsg.getByteBuffer() method.
 */
public class MamaMsgGetByteBuffer extends MamaTestBaseTestCase
{
    /* ****************************************************** */
    /* Protected Member Variables. */
    /* ****************************************************** */

    // The bridge
    MamaBridge mBridge;

    // The message under test.
    MamaMsg mMessage;

    // This message is used to create other messages from
    MamaMsg mSecondMessage;

    // String data used for the test
    String mStringData = "This is a test";

    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Before
    public void setUp()
    {
        // Load the bridge
        super.setUp();

        // Create the second message
        mSecondMessage = new MamaMsg();

        // Add some data to it
        mSecondMessage.addString(null, 1, mStringData);
    }

    @After
    public void tearDown()
    {
        // Destroy the message
        mMessage = null;
        super.tearDown();
    }

    /* ****************************************************** */
    /* Test Functions. */
    /* ****************************************************** */

    @Test
    public void testNullArguments()
    {
        try
        {
            // Create the message
            mMessage = new MamaMsg();
            mMessage.createFromByteBuffer(null);
        }

        catch(WombatException e)
        {
        }
    }

    @Test
    public void createByteBufferTwice()
    {
        // Get the byte buffer from the second message
        byte[] buffer = mSecondMessage.getByteBuffer();

        // Create the message
        mMessage = new MamaMsg();

        // Create the message twice
        mMessage.createFromByteBuffer(buffer);

        try
        {
            mMessage.createFromByteBuffer(buffer);
        }

        catch(Exception e)
        {
        }
    }

    @Test
    public void testPair()
    {
        // Create the message
        mMessage = new MamaMsg();

        // Get the byte buffer from the second message
        byte[] buffer = mSecondMessage.getByteBuffer();

        // Create the first message using this buffer
        mMessage.createFromByteBuffer(buffer);

        // Get the string
        String data = mMessage.getString(null, 1);
        if(data.compareTo(mStringData) != 0)
        {
            throw new IllegalStateException();
        }
    }
}
