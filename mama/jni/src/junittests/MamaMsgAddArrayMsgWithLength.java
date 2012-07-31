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

/**
 *
 * This class will test the addArrayMsgWithLength function.
 */
public class MamaMsgAddArrayMsgWithLength extends TestCase
{
    // This array message will be added
    private MamaMsg[] mArrayMsg;

    // The length of the array message
    private int mArrayLength;

    // The message under test
    private MamaMsg mMsg;
   
    /* ****************************************************** */
    /* Private Functions. */
    /* ****************************************************** */

    private void TestAddingArray(int numberElements)
    {
        // Add the array message up to the supplied number of elements
        mMsg.addArrayMsgWithLength(null, 1, mArrayMsg, numberElements);

        // Obtain the array back from the message
        MamaMsg[] array = mMsg.getArrayMsg(null, 1);

        // Enumerate all the elements and make sure they are OK
        for(int nextMsg=0; nextMsg<array.length; nextMsg++)
        {
            // Verify that the data is correct
            byte data = array[nextMsg].getI8(null, (nextMsg + 1));
            assertEquals((nextMsg + 18), data);
        }
    }

    private void CheckException(MamaException me, String message)
    {
        // Check for null arg
        if(me.getMessage().compareTo(message) == 0)
        {
            // Success
        }

        else
        {
            throw me;
        }
    }

    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Override
    protected void setUp()
    {
        // Create a new mama msg
        mMsg = new MamaMsg();

        // Create the array message
        mArrayLength = 10;
        mArrayMsg = new MamaMsg[mArrayLength];
        for(int nextMsg=0; nextMsg<mArrayLength; nextMsg++)
        {
            // Create a sub message
            mArrayMsg[nextMsg] = new MamaMsg();

            // Add a single I8 field to it
            mArrayMsg[nextMsg].addI8(null, (nextMsg + 1), (byte)(nextMsg + 18));
        }
    }

    @Override
    protected void tearDown()
    {
        // Clear all variables
        mMsg = null;
        mArrayMsg = null;
    }

    /* ****************************************************** */
    /* Test Functions. */
    /* ****************************************************** */

    public void testNullArguments()
    {
        // Call the function with NULL array
        try
        {
            mMsg.addArrayMsgWithLength(null, 0, null, mArrayLength);
        }

        catch(MamaException me)
        {
            CheckException(me, "STATUS_NULL_ARG");
        }        
    }

    public void testNormal()
    {
        // Call the function with a normal number
        TestAddingArray(5);
    }

    public void testUpperBoundary()
    {
        // Call the function with the maximum allowed length
        TestAddingArray(10);
    }

    public void testAboveUpperBoundary()
    {
        // Call the function with one greater than the maximum allowed length
        try
        {
            TestAddingArray(11);
        }

        catch(MamaException me)
        {
            CheckException(me, "STATUS_INVALID_ARG");
        }
    }

    public void testLowerBoundary()
    {
        // Call the function with the minimum allowed length
        TestAddingArray(1);
    }

    public void testBelowLowerBoundary()
    {
        // Call the function with one less than the minimum allowed length
        try
        {
            TestAddingArray(0);
        }

        catch(MamaException me)
        {
            CheckException(me, "STATUS_INVALID_ARG");
        } 
    }   
}
