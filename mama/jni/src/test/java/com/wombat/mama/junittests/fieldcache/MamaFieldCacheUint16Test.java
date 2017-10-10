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
 * This class will test MamaFieldCacheUint16's functions
 */
public class MamaFieldCacheUint16Test extends TestCase
{
    /* ****************************************************** */
    /* Protected Member Variables. */
    /* ****************************************************** */
    
    protected MamaMsg mMsg;
    protected MamaFieldCacheUint16 mFieldCacheUint16;

    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Override
    protected void setUp()
    {
        mFieldCacheUint16 = new MamaFieldCacheUint16(101, "example", true);
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
        mFieldCacheUint16.set(1);
        //add it to the message
        mFieldCacheUint16.addToMessage(false, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getU16("example", 101), 1);
    }

    public void testAddToMessagefieldName()
    {
        //set the value of the field to true (the default, otherwise, is false
        mFieldCacheUint16.set(1);
        //add it to the message
        mFieldCacheUint16.addToMessage(true, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getU16("example", 101), 1);
    }

    public void testCopy()
    {
        MamaFieldCacheField copyCache =  mFieldCacheUint16.copy();
        assertEquals(mFieldCacheUint16.getAsString(), copyCache.getAsString());
    }

    public void testApplyMsgField()
    {
        MamaFieldCacheUint16 testCacheUint16 = new MamaFieldCacheUint16(102, "example2", true);
        mFieldCacheUint16.set(2);
        testCacheUint16.set(1);
        assertEquals(testCacheUint16.get(), 1);
        assertEquals(mFieldCacheUint16.get(), 2);
        mFieldCacheUint16.apply(testCacheUint16);
        assertEquals(mFieldCacheUint16.get(), 1);
    }

    public void testApplyFieldCache()
    {
       MamaFieldCacheUint16 testField  = new MamaFieldCacheUint16(102, "example2", true);
       testField.set(1);
       mFieldCacheUint16.apply(testField);
       assertEquals(mFieldCacheUint16.get(), 1);
    }


    public void testSet()
    {
        MamaFieldCacheUint16 testCacheUint16 = new MamaFieldCacheUint16(102, "example2", false);

        assertEquals(testCacheUint16.get(), 0);
        testCacheUint16.set(1);
        //set without track state
        assertEquals(testCacheUint16.get(), 1);

        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, testCacheUint16.getModState());

    }

    public void testSetTrackState()
    {
        assertEquals(mFieldCacheUint16.get(), 0);
        mFieldCacheUint16.set(1);
        //set with track state
        assertEquals(mFieldCacheUint16.get(), 1);
        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, mFieldCacheUint16.getModState());
    }

    public void testSetTrackStateTouched()
    {
    
        assertEquals(mFieldCacheUint16.get(), 0);
        mFieldCacheUint16.set(1);
        mFieldCacheUint16.set(1);
        //set with track state
        assertEquals(mFieldCacheUint16.get(), 1);
        assertEquals(MamaFieldCacheField.MOD_STATE_TOUCHED, mFieldCacheUint16.getModState());

    }

    public void testIsEqual()
    {   
        MamaFieldCacheUint16 testCacheUint16 = new MamaFieldCacheUint16(102, "example2", true);
        testCacheUint16.set(1);
        mFieldCacheUint16.set(1);
        assertEquals(testCacheUint16.get(), 1);
        assertEquals(mFieldCacheUint16.get(), 1);
        assertTrue(testCacheUint16.isEqual(mFieldCacheUint16.get()));
    }

    public void testGet()
    {
        assertEquals(mFieldCacheUint16.get(), 0);
        mFieldCacheUint16.set(1);
        assertEquals(mFieldCacheUint16.get(), 1);
    }

    public void testGetAsString()
    {
        assertEquals(mFieldCacheUint16.get(), 0);
        assertEquals("0", mFieldCacheUint16.getAsString());
        mFieldCacheUint16.set(1);
        assertEquals(mFieldCacheUint16.get(), 1);
        assertEquals("1", mFieldCacheUint16.getAsString());
    }
}
