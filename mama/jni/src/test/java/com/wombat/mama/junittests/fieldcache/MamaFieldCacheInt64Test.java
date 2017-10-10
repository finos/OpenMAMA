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
 * This class will test MamaFieldCacheInt64's functions
 */
public class MamaFieldCacheInt64Test extends TestCase
{
    /* ****************************************************** */
    /* Protected Member Variables. */
    /* ****************************************************** */
    
    protected MamaMsg mMsg;
    protected MamaFieldCacheInt64 mFieldCacheInt64;

    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Override
    protected void setUp()
    {
        mFieldCacheInt64 = new MamaFieldCacheInt64(101, "example", true);
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
        mFieldCacheInt64.set(1L);
        //add it to the message
        mFieldCacheInt64.addToMessage(false, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getU64("example", 101), 1L);
    }

    public void testAddToMessagefieldName()
    {
        //set the value of the field to true (the default, otherwise, is false
        mFieldCacheInt64.set(1L);
        //add it to the message
        mFieldCacheInt64.addToMessage(true, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getU64("example", 101), 1L);
    }

    public void testCopy()
    {
        MamaFieldCacheField copyCache =  mFieldCacheInt64.copy();
        assertEquals(mFieldCacheInt64.getAsString(), copyCache.getAsString());
    }

    public void testApplyMsgField()
    {
        MamaFieldCacheInt64 testCacheInt64 = new MamaFieldCacheInt64(102, "example2L", true);
        mFieldCacheInt64.set(2L);
        testCacheInt64.set(1L);
        assertEquals(testCacheInt64.get(), 1L);
        assertEquals(mFieldCacheInt64.get(), 2L);
        mFieldCacheInt64.apply(testCacheInt64);
        assertEquals(mFieldCacheInt64.get(), 1L);
    }

    public void testApplyFieldCache()
    {
       MamaFieldCacheInt64 testField  = new MamaFieldCacheInt64(102, "example2L", true);
       testField.set(1L);
       mFieldCacheInt64.apply(testField);
       assertEquals(mFieldCacheInt64.get(), 1L);
    }


    public void testSet()
    {
        MamaFieldCacheInt64 testCacheInt64 = new MamaFieldCacheInt64(102, "example2L", false);

        assertEquals(testCacheInt64.get(), 0);
        testCacheInt64.set(1L);
        //set without track state
        assertEquals(testCacheInt64.get(), 1L);

        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, testCacheInt64.getModState());

    }

    public void testSetTrackState()
    {
        assertEquals(mFieldCacheInt64.get(), 0);
        mFieldCacheInt64.set(1L);
        //set with track state
        assertEquals(mFieldCacheInt64.get(), 1L);
        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, mFieldCacheInt64.getModState());
    }

    public void testSetTrackStateTouched()
    {
    
        assertEquals(mFieldCacheInt64.get(), 0);
        mFieldCacheInt64.set(1L);
        mFieldCacheInt64.set(1L);
        //set with track state
        assertEquals(mFieldCacheInt64.get(), 1L);
        assertEquals(MamaFieldCacheField.MOD_STATE_TOUCHED, mFieldCacheInt64.getModState());

    }

    public void testIsEqual()
    {   
        MamaFieldCacheInt64 testCacheInt64 = new MamaFieldCacheInt64(102, "example2L", true);
        testCacheInt64.set(1L);
        mFieldCacheInt64.set(1L);
        assertEquals(testCacheInt64.get(), 1L);
        assertEquals(mFieldCacheInt64.get(), 1L);
        assertTrue(testCacheInt64.isEqual(mFieldCacheInt64.get()));
    }

    public void testGet()
    {
        assertEquals(mFieldCacheInt64.get(), 0);
        mFieldCacheInt64.set(1L);
        assertEquals(mFieldCacheInt64.get(), 1L);
    }

    public void testGetAsString()
    {
        assertEquals(mFieldCacheInt64.get(), 0);
        assertEquals("0", mFieldCacheInt64.getAsString());
        mFieldCacheInt64.set(1L);
        assertEquals(mFieldCacheInt64.get(), 1L);
        assertEquals("1", mFieldCacheInt64.getAsString());
    }
}
