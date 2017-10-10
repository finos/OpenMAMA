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
 * This class will test MamaFieldCacheInt16's functions
 */
public class MamaFieldCacheInt16Test extends TestCase
{
    /* ****************************************************** */
    /* Protected Member Variables. */
    /* ****************************************************** */
    
    protected MamaMsg mMsg;
    protected MamaFieldCacheInt16 mFieldCacheInt16;

    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Override
    protected void setUp()
    {
        mFieldCacheInt16 = new MamaFieldCacheInt16(101, "example", true);
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
        mFieldCacheInt16.set((short) 1);
        //add it to the message
        mFieldCacheInt16.addToMessage(false, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getU16("example", 101), (short)1);
    }

    public void testAddToMessagefieldName()
    {
        //set the value of the field to true (the default, otherwise, is false
        mFieldCacheInt16.set((short) 1);
        //add it to the message
        mFieldCacheInt16.addToMessage(true, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getI16("example", 101), (short) 1);
    }

    public void testCopy()
    {
        MamaFieldCacheField copyCache =  mFieldCacheInt16.copy();
        assertEquals(mFieldCacheInt16.getAsString(), copyCache.getAsString());
    }

    public void testApplyMsgField()
    {
        MamaFieldCacheInt16 testCacheInt16 = new MamaFieldCacheInt16(102, "example2", true);
        mFieldCacheInt16.set((short) 2);
        testCacheInt16.set((short) 1);
        assertEquals(testCacheInt16.get(), (short) 1);
        assertEquals(mFieldCacheInt16.get(), (short) 2);
        mFieldCacheInt16.apply(testCacheInt16);
        assertEquals(mFieldCacheInt16.get(), 1);
    }

    public void testApplyFieldCache()
    {
       MamaFieldCacheInt16 testField  = new MamaFieldCacheInt16(102, "example2", true);
       testField.set((short) 1);
       mFieldCacheInt16.apply(testField);
       assertEquals(mFieldCacheInt16.get(), (short) 1);
    }


    public void testSet()
    {
        MamaFieldCacheInt16 testCacheInt16 = new MamaFieldCacheInt16(102, "example2", false);

        assertEquals(testCacheInt16.get(), (short) 0);
        testCacheInt16.set((short) 1);
        //set without track state
        assertEquals(testCacheInt16.get(), (short) 1);

        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, testCacheInt16.getModState());

    }

    public void testSetTrackState()
    {
        assertEquals(mFieldCacheInt16.get(), (short) 0);
        mFieldCacheInt16.set((short) 1);
        //set with track state
        assertEquals(mFieldCacheInt16.get(), (short) 1);
        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, mFieldCacheInt16.getModState());
    }

    public void testSetTrackStateTouched()
    {
    
        assertEquals(mFieldCacheInt16.get(), (short) 0);
        mFieldCacheInt16.set((short) 1);
        mFieldCacheInt16.set((short) 1);
        //set with track state
        assertEquals(mFieldCacheInt16.get(), (short) 1);
        assertEquals(MamaFieldCacheField.MOD_STATE_TOUCHED, mFieldCacheInt16.getModState());

    }

    public void testIsEqual()
    {   
        MamaFieldCacheInt16 testCacheInt16 = new MamaFieldCacheInt16(102, "example2", true);
        testCacheInt16.set((short) 1);
        mFieldCacheInt16.set((short) 1);
        assertEquals(testCacheInt16.get(), (short) 1);
        assertEquals(mFieldCacheInt16.get(), (short) 1);
        assertTrue(testCacheInt16.isEqual(mFieldCacheInt16.get()));
    }

    public void testGet()
    {
        assertEquals(mFieldCacheInt16.get(), 0);
        mFieldCacheInt16.set((short) 1);
        assertEquals(mFieldCacheInt16.get(), (short) 1);
    }

    public void testGetAsString()
    {
        assertEquals(mFieldCacheInt16.get(), 0);
        assertEquals("0", mFieldCacheInt16.getAsString());
        mFieldCacheInt16.set((short) 1);
        assertEquals(mFieldCacheInt16.get(), (short) 1);
        assertEquals("1", mFieldCacheInt16.getAsString());
    }
}
