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
 * This class will test MamaFieldCacheInt8's functions
 */
public class MamaFieldCacheInt8Test extends TestCase
{
    /* ****************************************************** */
    /* Protected Member Variables. */
    /* ****************************************************** */
    
    protected MamaMsg mMsg;
    protected MamaFieldCacheInt8 mFieldCacheInt8;

    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Override
    protected void setUp()
    {
        mFieldCacheInt8 = new MamaFieldCacheInt8(101, "example", true);
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
        mFieldCacheInt8.set((byte) 1);
        //add it to the message
        mFieldCacheInt8.addToMessage(false, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getU8("example", 101), (byte)1);
    }

    public void testAddToMessagefieldName()
    {
        //set the value of the field to true (the default, otherwise, is false
        mFieldCacheInt8.set((byte) 1);
        //add it to the message
        mFieldCacheInt8.addToMessage(true, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getI8("example", 101), (byte) 1);
    }

    public void testCopy()
    {
        MamaFieldCacheField copyCache =  mFieldCacheInt8.copy();
        assertEquals(mFieldCacheInt8.getAsString(), copyCache.getAsString());
    }

    public void testApplyMsgField()
    {
        MamaFieldCacheInt8 testCacheInt8 = new MamaFieldCacheInt8(102, "example2", true);
        mFieldCacheInt8.set((byte) 2);
        testCacheInt8.set((byte) 1);
        assertEquals(testCacheInt8.get(), (byte) 1);
        assertEquals(mFieldCacheInt8.get(), (byte) 2);
        mFieldCacheInt8.apply(testCacheInt8);
        assertEquals(mFieldCacheInt8.get(), 1);
    }

    public void testApplyFieldCache()
    {
       MamaFieldCacheInt8 testField  = new MamaFieldCacheInt8(102, "example2", true);
       testField.set((byte) 1);
       mFieldCacheInt8.apply(testField);
       assertEquals(mFieldCacheInt8.get(), (byte) 1);
    }


    public void testSet()
    {
        MamaFieldCacheInt8 testCacheInt8 = new MamaFieldCacheInt8(102, "example2", false);

        assertEquals(testCacheInt8.get(), (byte) 0);
        testCacheInt8.set((byte) 1);
        //set without track state
        assertEquals(testCacheInt8.get(), (byte) 1);

        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, testCacheInt8.getModState());

    }

    public void testSetTrackState()
    {
        assertEquals(mFieldCacheInt8.get(), (byte) 0);
        mFieldCacheInt8.set((byte) 1);
        //set with track state
        assertEquals(mFieldCacheInt8.get(), (byte) 1);
        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, mFieldCacheInt8.getModState());
    }

    public void testSetTrackStateTouched()
    {
    
        assertEquals(mFieldCacheInt8.get(), (byte) 0);
        mFieldCacheInt8.set((byte) 1);
        mFieldCacheInt8.set((byte) 1);
        //set with track state
        assertEquals(mFieldCacheInt8.get(), (byte) 1);
        assertEquals(MamaFieldCacheField.MOD_STATE_TOUCHED, mFieldCacheInt8.getModState());

    }

    public void testIsEqual()
    {   
        MamaFieldCacheInt8 testCacheInt8 = new MamaFieldCacheInt8(102, "example2", true);
        testCacheInt8.set((byte) 1);
        mFieldCacheInt8.set((byte) 1);
        assertEquals(testCacheInt8.get(), (byte) 1);
        assertEquals(mFieldCacheInt8.get(), (byte) 1);
        assertTrue(testCacheInt8.isEqual(mFieldCacheInt8.get()));
    }

    public void testGet()
    {
        assertEquals(mFieldCacheInt8.get(), 0);
        mFieldCacheInt8.set((byte) 1);
        assertEquals(mFieldCacheInt8.get(), (byte) 1);
    }

    public void testGetAsString()
    {
        assertEquals(mFieldCacheInt8.get(), 0);
        assertEquals("0", mFieldCacheInt8.getAsString());
        mFieldCacheInt8.set((byte) 1);
        assertEquals(mFieldCacheInt8.get(), (byte) 1);
        assertEquals("1", mFieldCacheInt8.getAsString());
    }
}
