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
 * This class will test MamaFieldCacheInt32's functions
 */
public class MamaFieldCacheInt32Test extends TestCase
{
    /* ****************************************************** */
    /* Protected Member Variables. */
    /* ****************************************************** */
    
    protected MamaMsg mMsg;
    protected MamaFieldCacheInt32 mFieldCacheInt32;

    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Override
    protected void setUp()
    {
        mFieldCacheInt32 = new MamaFieldCacheInt32(101, "example", true);
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
        mFieldCacheInt32.set(1);
        //add it to the message
        mFieldCacheInt32.addToMessage(false, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getU32("example", 101), 1);
    }

    public void testAddToMessagefieldName()
    {
        //set the value of the field to true (the default, otherwise, is false
        mFieldCacheInt32.set(1);
        //add it to the message
        mFieldCacheInt32.addToMessage(true, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getU32("example", 101), 1);
    }

    public void testCopy()
    {
        MamaFieldCacheField copyCache =  mFieldCacheInt32.copy();
        assertEquals(mFieldCacheInt32.getAsString(), copyCache.getAsString());
    }

    public void testApplyMsgField()
    {
        MamaFieldCacheInt32 testCacheInt32 = new MamaFieldCacheInt32(102, "example2", true);
        mFieldCacheInt32.set(2);
        testCacheInt32.set(1);
        assertEquals(testCacheInt32.get(), 1);
        assertEquals(mFieldCacheInt32.get(), 2);
        mFieldCacheInt32.apply(testCacheInt32);
        assertEquals(mFieldCacheInt32.get(), 1);
    }

    public void testApplyFieldCache()
    {
       MamaFieldCacheInt32 testField  = new MamaFieldCacheInt32(102, "example2", true);
       testField.set(1);
       mFieldCacheInt32.apply(testField);
       assertEquals(mFieldCacheInt32.get(), 1);
    }


    public void testSet()
    {
        MamaFieldCacheInt32 testCacheInt32 = new MamaFieldCacheInt32(102, "example2", false);

        assertEquals(testCacheInt32.get(), 0);
        testCacheInt32.set(1);
        //set without track state
        assertEquals(testCacheInt32.get(), 1);

        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, testCacheInt32.getModState());

    }

    public void testSetTrackState()
    {
        assertEquals(mFieldCacheInt32.get(), 0);
        mFieldCacheInt32.set(1);
        //set with track state
        assertEquals(mFieldCacheInt32.get(), 1);
        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, mFieldCacheInt32.getModState());
    }

    public void testSetTrackStateTouched()
    {
    
        assertEquals(mFieldCacheInt32.get(), 0);
        mFieldCacheInt32.set(1);
        mFieldCacheInt32.set(1);
        //set with track state
        assertEquals(mFieldCacheInt32.get(), 1);
        assertEquals(MamaFieldCacheField.MOD_STATE_TOUCHED, mFieldCacheInt32.getModState());

    }

    public void testIsEqual()
    {   
        MamaFieldCacheInt32 testCacheInt32 = new MamaFieldCacheInt32(102, "example2", true);
        testCacheInt32.set(1);
        mFieldCacheInt32.set(1);
        assertEquals(testCacheInt32.get(), 1);
        assertEquals(mFieldCacheInt32.get(), 1);
        assertTrue(testCacheInt32.isEqual(mFieldCacheInt32.get()));
    }

    public void testGet()
    {
        assertEquals(mFieldCacheInt32.get(), 0);
        mFieldCacheInt32.set(1);
        assertEquals(mFieldCacheInt32.get(), 1);
    }

    public void testGetAsString()
    {
        assertEquals(mFieldCacheInt32.get(), 0);
        assertEquals("0", mFieldCacheInt32.getAsString());
        mFieldCacheInt32.set(1);
        assertEquals(mFieldCacheInt32.get(), 1);
        assertEquals("1", mFieldCacheInt32.getAsString());
    }
}
