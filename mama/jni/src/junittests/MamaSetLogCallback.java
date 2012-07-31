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

import junit.framework.*;
import com.wombat.mama.*;

/**
 *
 * This class will test the setLogCallback function.
 */
public class MamaSetLogCallback extends TestCase
{
    public class TestLogCallback implements MamaLogFileCallback2
    {
        // Used to receive log messages
        public String mBuffer = "";

        public void onLog(MamaLogLevel level, String message)
        {
            /* Format the log as a string, do this separately so we can
             * see the string in the debugger.
             */
            mBuffer = new String(message);

            /* Write it out. */
            System.out.println(mBuffer);
        }

        public void onLogSizeExceeded()
        {
        }
    }

    // This class is used to intercept log messages
    private TestLogCallback mCallback;
    
    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Override
    protected void setUp()
    {
        // Create the callback class
        mCallback = new TestLogCallback();
    }

    @Override
    protected void tearDown()
    {
        // Reset member variables
        mCallback = null;
    }

    /* ****************************************************** */
    /* Test Functions. */
    /* ****************************************************** */

    public void testNullArguments()
    {
        // Set the log function
        try
        {
            Mama.setLogCallback(null);

            // If reach here then fail
            assertTrue(true);
        }

        catch(IllegalArgumentException iae)
        {
        }
    }

    public void testLogMessage()
    {   
        // Set the log function
        Mama.setLogCallback(mCallback);

        // Write a log
        Mama.log(MamaLogLevel.WARN, "This is a test");

        // Check to make sure that the message has been received
        assertEquals(mCallback.mBuffer, "This is a test");

        // Repeat
        Mama.log(MamaLogLevel.WARN, "Hooray");

        // Check to make sure that the message has been received
        assertEquals(mCallback.mBuffer, "Hooray");
    }  
}
