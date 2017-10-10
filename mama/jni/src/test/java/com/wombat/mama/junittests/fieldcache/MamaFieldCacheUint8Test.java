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
 * This class will test MamaFieldCacheUint8's functions
 */
public class MamaFieldCacheUint8Test extends TestCase
{
    /* ****************************************************** */
    /* Protected Member Variables. */
    /* ****************************************************** */
    
    protected MamaMsg mMsg;
    protected MamaFieldCacheUint8 mFieldCacheUint8;

    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Override
    protected void setUp()
    {
        mFieldCacheUint8 = new MamaFieldCacheUint8(101, "example", true);
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
        mFieldCacheUint8.set((short)1);
        //add it to the message
        mFieldCacheUint8.addToMessage(false, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getU8("example", 101), 1);
    }

    public void testAddToMessagefieldName()
    {
        //set the value of the field to true (the default, otherwise, is false
        mFieldCacheUint8.set((short)1);
        //add it to the message
        mFieldCacheUint8.addToMessage(true, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getU8("example", 101), 1);
    }

    public void testCopy()
    {
        MamaFieldCacheField copyCache =  mFieldCacheUint8.copy();
        assertEquals(mFieldCacheUint8.getAsString(), copyCache.getAsString());
    }

    public void testApplyMsgField()
    {
        MamaFieldCacheUint8 testCacheUint8 = new MamaFieldCacheUint8(102, "example2", true);
        mFieldCacheUint8.set((short)2);
        testCacheUint8.set((short)1);
        assertEquals(testCacheUint8.get(), 1);
        assertEquals(mFieldCacheUint8.get(), 2);
        mFieldCacheUint8.apply(testCacheUint8);
        assertEquals(mFieldCacheUint8.get(), 1);
    }

    public void testApplyFieldCache()
    {
       MamaFieldCacheUint8 testField  = new MamaFieldCacheUint8(102, "example2", true);
       testField.set((short)1);
       mFieldCacheUint8.apply(testField);
       assertEquals(mFieldCacheUint8.get(), 1);
    }


    public void testSet()
    {
        MamaFieldCacheUint8 testCacheUint8 = new MamaFieldCacheUint8(102, "example2", false);

        assertEquals(testCacheUint8.get(), 0);
        testCacheUint8.set((short)1);
        //set without track state
        assertEquals(testCacheUint8.get(), 1);

        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, testCacheUint8.getModState());

    }

    public void testSetTrackState()
    {
        assertEquals(mFieldCacheUint8.get(), 0);
        mFieldCacheUint8.set((short)1);
        //set with track state
        assertEquals(mFieldCacheUint8.get(), 1);
        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, mFieldCacheUint8.getModState());
    }

    public void testSetTrackStateTouched()
    {
    
        assertEquals(mFieldCacheUint8.get(), 0);
        mFieldCacheUint8.set((short)1);
        mFieldCacheUint8.set((short)1);
        //set with track state
        assertEquals(mFieldCacheUint8.get(), 1);
        assertEquals(MamaFieldCacheField.MOD_STATE_TOUCHED, mFieldCacheUint8.getModState());

    }

    public void testIsEqual()
    {   
        MamaFieldCacheUint8 testCacheUint8 = new MamaFieldCacheUint8(102, "example2", true);
        testCacheUint8.set((short)1);
        mFieldCacheUint8.set((short)1);
        assertEquals(testCacheUint8.get(), 1);
        assertEquals(mFieldCacheUint8.get(), 1);
        assertTrue(testCacheUint8.isEqual(mFieldCacheUint8.get()));
    }

    public void testGet()
    {
        assertEquals(mFieldCacheUint8.get(), 0);
        mFieldCacheUint8.set((short)1);
        assertEquals(mFieldCacheUint8.get(), 1);
    }

    public void testGetAsString()
    {
        assertEquals(mFieldCacheUint8.get(), 0);
        assertEquals("0", mFieldCacheUint8.getAsString());
        mFieldCacheUint8.set((short)1);
        assertEquals(mFieldCacheUint8.get(), 1);
        assertEquals("1", mFieldCacheUint8.getAsString());
    }
}
