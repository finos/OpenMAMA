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
/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
import java.nio.*;
import junit.framework.*;
import com.wombat.mama.*;

/**
 *
 * This class will test the MamaMsg.getStringAsCharBuffer() method.
 */
public class MamaMsgGetStringAsCharBuffer extends TestCase
{
    /* ****************************************************** */
    /* Protected Member Variables. */
    /* ****************************************************** */

    // The message under test
    private MamaMsg mMessage;
    
    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Override
    protected void setUp()
    {
        // Allocate the message
        mMessage = new MamaMsg();

        // Add a string to the message
        mMessage.addString(null, 1, "This is a test");
    }

    @Override
    protected void tearDown()
    {
        // Clear the message reference
        mMessage = null;
    }

    /* ****************************************************** */
    /* Test Functions. */
    /* ****************************************************** */

    public void testGetCharBuffer()
    {
        // Get the character buffer from the message
        CharBuffer charBuffer = mMessage.getStringAsCharBuffer(null, 1);

        // Verify that the string representation is correct
        Assert.assertEquals("This is a test", charBuffer.toString());
    }
}
