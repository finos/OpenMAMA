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
 * This class will test MamaFieldCacheString's functions
 */
public class MamaFieldCacheStringTest extends TestCase
{
    /* ****************************************************** */
    /* Protected Member Variables. */
    /* ****************************************************** */
    
    protected MamaMsg mMsg;
    protected MamaFieldCacheString mFieldCacheString;

    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Override
    protected void setUp()
    {
        mFieldCacheString = new MamaFieldCacheString(101, "example", true);
        mMsg = new MamaMsg();   
    }

    @Override
    protected void tearDown()
    {
    }

    /* ****************************************************** */
    /* Test Functions. */
    /* ****************************************************** */

    public void testAddToMessage()
    {
        //set the value of the field to true (the default, otherwise, is false
        mFieldCacheString.set("a");
        //add it to the message
        mFieldCacheString.addToMessage(false, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getString("example", 101), "a");
    }

    public void testAddToMessagefieldName()
    {
        //set the value of the field to true (the default, otherwise, is false
        mFieldCacheString.set("a");
        //add it to the message
        mFieldCacheString.addToMessage(true, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getString("example", 101), "a");
    }

    public void testCopy()
    {
        MamaFieldCacheField copyCache =  mFieldCacheString.copy();
        assertEquals(mFieldCacheString.getAsString(), copyCache.getAsString());
    }

    public void testApplyMsgField()
    {
        MamaFieldCacheString testCacheString = new MamaFieldCacheString(102, "example2", true);
        mFieldCacheString.set("b");
        testCacheString.set("a");
        assertEquals(testCacheString.get(), "a");
        assertEquals(mFieldCacheString.get(), "b");
        mFieldCacheString.apply(testCacheString);
        assertEquals(mFieldCacheString.get(), "a");
    }

    public void testApplyFieldCache()
    {
       MamaFieldCacheString testField  = new MamaFieldCacheString(102, "example2", true);
       testField.set("a");
       mFieldCacheString.apply(testField);
       assertEquals(mFieldCacheString.get(), "a");
    }


    public void testSet()
    {
        MamaFieldCacheString testCacheString = new MamaFieldCacheString(102, "example2", false);

        assertEquals(testCacheString.get(), null);
        testCacheString.set("a");
        //set without track state
        assertEquals(testCacheString.get(), "a");

        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, testCacheString.getModState());

    }

    public void testSetTrackState()
    {
        assertEquals(mFieldCacheString.get(), null);
        mFieldCacheString.set("a");
        //set with track state
        assertEquals(mFieldCacheString.get(), "a");
        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, mFieldCacheString.getModState());
    }

    public void testSetTrackStateTouched()
    {
    
        assertEquals(mFieldCacheString.get(), null);
        mFieldCacheString.set("a");
        mFieldCacheString.set("a");
        //set with track state
        assertEquals(mFieldCacheString.get(), "a");
        assertEquals(MamaFieldCacheField.MOD_STATE_TOUCHED, mFieldCacheString.getModState());

    }

    public void testIsEqual()
    {   
        MamaFieldCacheString testCacheString = new MamaFieldCacheString(102, "example2", true);
        testCacheString.set("a");
        mFieldCacheString.set("a");
        assertEquals(testCacheString.get(), "a");
        assertEquals(mFieldCacheString.get(), "a");
        assertTrue(testCacheString.isEqual(mFieldCacheString.get()));
    }

    public void testGet()
    {
        assertEquals(mFieldCacheString.get(), null);
        mFieldCacheString.set("a");
        assertEquals(mFieldCacheString.get(), "a");
    }

    public void testGetAsString()
    {
        assertEquals(mFieldCacheString.get(), null);
        assertEquals("null", mFieldCacheString.getAsString());
        mFieldCacheString.set("a");
        assertEquals(mFieldCacheString.get(), "a");
        assertEquals("a", mFieldCacheString.getAsString());
    }
}
