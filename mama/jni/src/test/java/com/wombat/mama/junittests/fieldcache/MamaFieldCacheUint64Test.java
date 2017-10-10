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
 * This class will test MamaFieldCacheUint64's functions
 */
public class MamaFieldCacheUint64Test extends TestCase
{
    /* ****************************************************** */
    /* Protected Member Variables. */
    /* ****************************************************** */
    
    protected MamaMsg mMsg;
    protected MamaFieldCacheUint64 mFieldCacheUint64;

    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Override
    protected void setUp()
    {
        mFieldCacheUint64 = new MamaFieldCacheUint64(101, "example", true);
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
        mFieldCacheUint64.set(1);
        //add it to the message
        mFieldCacheUint64.addToMessage(false, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getU64("example", 101), 1);
    }

    public void testAddToMessagefieldName()
    {
        //set the value of the field to true (the default, otherwise, is false
        mFieldCacheUint64.set(1);
        //add it to the message
        mFieldCacheUint64.addToMessage(true, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getU64("example", 101), 1);
    }

    public void testCopy()
    {
        MamaFieldCacheField copyCache =  mFieldCacheUint64.copy();
        assertEquals(mFieldCacheUint64.getAsString(), copyCache.getAsString());
    }

    public void testApplyMsgField()
    {
        MamaFieldCacheUint64 testCacheUint64 = new MamaFieldCacheUint64(102, "example2", true);
        mFieldCacheUint64.set(2);
        testCacheUint64.set(1);
        assertEquals(testCacheUint64.get(), 1);
        assertEquals(mFieldCacheUint64.get(), 2);
        mFieldCacheUint64.apply(testCacheUint64);
        assertEquals(mFieldCacheUint64.get(), 1);
    }

    public void testApplyFieldCache()
    {
       MamaFieldCacheUint64 testField  = new MamaFieldCacheUint64(102, "example2", true);
       testField.set(1);
       mFieldCacheUint64.apply(testField);
       assertEquals(mFieldCacheUint64.get(), 1);
    }


    public void testSet()
    {
        MamaFieldCacheUint64 testCacheUint64 = new MamaFieldCacheUint64(102, "example2", false);

        assertEquals(testCacheUint64.get(), 0);
        testCacheUint64.set(1);
        //set without track state
        assertEquals(testCacheUint64.get(), 1);

        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, testCacheUint64.getModState());

    }

    public void testSetTrackState()
    {
        assertEquals(mFieldCacheUint64.get(), 0);
        mFieldCacheUint64.set(1);
        //set with track state
        assertEquals(mFieldCacheUint64.get(), 1);
        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, mFieldCacheUint64.getModState());
    }

    public void testSetTrackStateTouched()
    {
    
        assertEquals(mFieldCacheUint64.get(), 0);
        mFieldCacheUint64.set(1);
        mFieldCacheUint64.set(1);
        //set with track state
        assertEquals(mFieldCacheUint64.get(), 1);
        assertEquals(MamaFieldCacheField.MOD_STATE_TOUCHED, mFieldCacheUint64.getModState());

    }

    public void testIsEqual()
    {   
        MamaFieldCacheUint64 testCacheUint64 = new MamaFieldCacheUint64(102, "example2", true);
        testCacheUint64.set(1);
        mFieldCacheUint64.set(1);
        assertEquals(testCacheUint64.get(), 1);
        assertEquals(mFieldCacheUint64.get(), 1);
        assertTrue(testCacheUint64.isEqual(mFieldCacheUint64.get()));
    }

    public void testGet()
    {
        assertEquals(mFieldCacheUint64.get(), 0);
        mFieldCacheUint64.set(1);
        assertEquals(mFieldCacheUint64.get(), 1);
    }

    public void testGetAsString()
    {
        assertEquals(mFieldCacheUint64.get(), 0);
        assertEquals("0", mFieldCacheUint64.getAsString());
        mFieldCacheUint64.set(1);
        assertEquals(mFieldCacheUint64.get(), 1);
        assertEquals("1", mFieldCacheUint64.getAsString());
    }
}
