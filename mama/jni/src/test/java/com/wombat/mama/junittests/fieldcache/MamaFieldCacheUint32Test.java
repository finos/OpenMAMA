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
 * This class will test MamaFieldCacheUint32's functions
 */
public class MamaFieldCacheUint32Test extends TestCase
{
    /* ****************************************************** */
    /* Protected Member Variables. */
    /* ****************************************************** */
    
    protected MamaMsg mMsg;
    protected MamaFieldCacheUint32 mFieldCacheUint32;

    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Override
    protected void setUp()
    {
        mFieldCacheUint32 = new MamaFieldCacheUint32(101, "example", true);
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
        mFieldCacheUint32.set(1);
        //add it to the message
        mFieldCacheUint32.addToMessage(false, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getU32("example", 101), 1);
    }

    public void testAddToMessagefieldName()
    {
        //set the value of the field to true (the default, otherwise, is false
        mFieldCacheUint32.set(1);
        //add it to the message
        mFieldCacheUint32.addToMessage(true, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getU32("example", 101), 1);
    }

    public void testCopy()
    {
        MamaFieldCacheField copyCache =  mFieldCacheUint32.copy();
        assertEquals(mFieldCacheUint32.getAsString(), copyCache.getAsString());
    }

    public void testApplyMsgField()
    {
        MamaFieldCacheUint32 testCacheUint32 = new MamaFieldCacheUint32(102, "example2", true);
        mFieldCacheUint32.set(2);
        testCacheUint32.set(1);
        assertEquals(testCacheUint32.get(), 1);
        assertEquals(mFieldCacheUint32.get(), 2);
        mFieldCacheUint32.apply(testCacheUint32);
        assertEquals(mFieldCacheUint32.get(), 1);
    }

    public void testApplyFieldCache()
    {
       MamaFieldCacheUint32 testField  = new MamaFieldCacheUint32(102, "example2", true);
       testField.set(1);
       mFieldCacheUint32.apply(testField);
       assertEquals(mFieldCacheUint32.get(), 1);
    }


    public void testSet()
    {
        MamaFieldCacheUint32 testCacheUint32 = new MamaFieldCacheUint32(102, "example2", false);

        assertEquals(testCacheUint32.get(), 0);
        testCacheUint32.set(1);
        //set without track state
        assertEquals(testCacheUint32.get(), 1);

        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, testCacheUint32.getModState());

    }

    public void testSetTrackState()
    {
        assertEquals(mFieldCacheUint32.get(), 0);
        mFieldCacheUint32.set(1);
        //set with track state
        assertEquals(mFieldCacheUint32.get(), 1);
        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, mFieldCacheUint32.getModState());
    }

    public void testSetTrackStateTouched()
    {
    
        assertEquals(mFieldCacheUint32.get(), 0);
        mFieldCacheUint32.set(1);
        mFieldCacheUint32.set(1);
        //set with track state
        assertEquals(mFieldCacheUint32.get(), 1);
        assertEquals(MamaFieldCacheField.MOD_STATE_TOUCHED, mFieldCacheUint32.getModState());

    }

    public void testIsEqual()
    {   
        MamaFieldCacheUint32 testCacheUint32 = new MamaFieldCacheUint32(102, "example2", true);
        testCacheUint32.set(1);
        mFieldCacheUint32.set(1);
        assertEquals(testCacheUint32.get(), 1);
        assertEquals(mFieldCacheUint32.get(), 1);
        assertTrue(testCacheUint32.isEqual(mFieldCacheUint32.get()));
    }

    public void testGet()
    {
        assertEquals(mFieldCacheUint32.get(), 0);
        mFieldCacheUint32.set(1);
        assertEquals(mFieldCacheUint32.get(), 1);
    }

    public void testGetAsString()
    {
        assertEquals(mFieldCacheUint32.get(), 0);
        assertEquals("0", mFieldCacheUint32.getAsString());
        mFieldCacheUint32.set(1);
        assertEquals(mFieldCacheUint32.get(), 1);
        assertEquals("1", mFieldCacheUint32.getAsString());
    }
}
