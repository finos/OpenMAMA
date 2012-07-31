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

import junit.framework.TestCase;
import com.wombat.mama.*;
import com.wombat.common.*;

/**
 *
 * This class will test the MamaMsg.getByteBuffer() method.
 */
public class MamaMsgGetByteBuffer extends TestCase
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

    @Override
    protected void setUp()
    {
         // Load the wmw bridge
        mBridge = Mama.loadBridge("wmw");
        Mama.open();

        // Create the message
        mMessage = new MamaMsg();

        // Create the second message
        mSecondMessage = new MamaMsg();

        // Add some data to it
        mSecondMessage.addString(null, 1, mStringData);       
    }

    @Override
    protected void tearDown()
    {
        // Destroy the message
        mMessage.destroy();
        mMessage = null;

        // Destroy the second message
        mSecondMessage.destroy();
    }

    /* ****************************************************** */
    /* Test Functions. */
    /* ****************************************************** */

    public void testNullArguments()
    {
        try
        {
            mMessage.createFromByteBuffer(null);
        }

        catch(WombatException e)
        {
        }
    }

    public void createByteBufferTwice()
    {
        // Get the byte buffer from the second message
        byte[] buffer = mSecondMessage.getByteBuffer();

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

    public void testPair()
    {
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
