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
 * This class will test MamaFieldCacheFloat64's functions
 */
public class MamaFieldCacheFloat64Test extends TestCase
{
    /* ****************************************************** */
    /* Protected Member Variables. */
    /* ****************************************************** */
    
    protected MamaMsg mMsg;
    protected MamaFieldCacheFloat64 mFieldCacheFloat64;

    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Override
    protected void setUp()
    {
        mFieldCacheFloat64 = new MamaFieldCacheFloat64(101, "example", true);
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
        mFieldCacheFloat64.set(1.0);
        //add it to the message
        mFieldCacheFloat64.addToMessage(false, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getF64("example", 101), 1.0);
    }

    public void testAddToMessagefieldName()
    {
        //set the value of the field to true (the default, otherwise, is false
        mFieldCacheFloat64.set(1.0);
        //add it to the message
        mFieldCacheFloat64.addToMessage(true, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getF64("example", 101), 1.0);
    }

    public void testCopy()
    {
        MamaFieldCacheField copyCache =  mFieldCacheFloat64.copy();
        assertEquals(mFieldCacheFloat64.getAsString(), copyCache.getAsString());
    }

    public void testApplyMsgField()
    {
        MamaFieldCacheFloat64 testCacheFloat64 = new MamaFieldCacheFloat64(102, "example2", true);
        mFieldCacheFloat64.set(2);
        testCacheFloat64.set(1.0);
        assertEquals(testCacheFloat64.get(), 1.0);
        assertEquals(mFieldCacheFloat64.get(), 2.0);
        mFieldCacheFloat64.apply(testCacheFloat64);
        assertEquals(mFieldCacheFloat64.get(), 1.0);
    }

    public void testApplyFieldCache()
    {
       MamaFieldCacheFloat64 testField  = new MamaFieldCacheFloat64(102, "example2", true);
       testField.set(1.0);
       mFieldCacheFloat64.apply(testField);
       assertEquals(mFieldCacheFloat64.get(), 1.0);
    }


    public void testSet()
    {
        MamaFieldCacheFloat64 testCacheFloat64 = new MamaFieldCacheFloat64(102, "example2", false);

        assertEquals(testCacheFloat64.get(), 0.0);
        testCacheFloat64.set(1.0);
        //set without track state
        assertEquals(testCacheFloat64.get(), 1.0);

        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, testCacheFloat64.getModState());

    }

    public void testSetTrackState()
    {
        assertEquals(mFieldCacheFloat64.get(), 0.0);
        mFieldCacheFloat64.set(1.0);
        //set with track state
        assertEquals(mFieldCacheFloat64.get(), 1.0);
        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, mFieldCacheFloat64.getModState());
    }

    public void testSetTrackStateTouched()
    {
    
        assertEquals(mFieldCacheFloat64.get(), 0.0);
        mFieldCacheFloat64.set(1.0);
        mFieldCacheFloat64.set(1.0);
        //set with track state
        assertEquals(mFieldCacheFloat64.get(), 1.0);
        assertEquals(MamaFieldCacheField.MOD_STATE_TOUCHED, mFieldCacheFloat64.getModState());

    }

    public void testIsEqual()
    {   
        MamaFieldCacheFloat64 testCacheFloat64 = new MamaFieldCacheFloat64(102, "example2", true);
        testCacheFloat64.set(1.0);
        mFieldCacheFloat64.set(1.0);
        assertEquals(testCacheFloat64.get(), 1.0);
        assertEquals(mFieldCacheFloat64.get(), 1.0);
        assertTrue(testCacheFloat64.isEqual(mFieldCacheFloat64.get()));
    }

    public void testGet()
    {
        assertEquals(mFieldCacheFloat64.get(), 0.0);
        mFieldCacheFloat64.set(1.0);
        assertEquals(mFieldCacheFloat64.get(), 1.0);
    }

    public void testGetAsString()
    {
        assertEquals(mFieldCacheFloat64.get(), 0.0);
        assertEquals("0.0", mFieldCacheFloat64.getAsString());
        mFieldCacheFloat64.set(1.0);
        assertEquals(mFieldCacheFloat64.get(), 1.0);
        assertEquals("1.0", mFieldCacheFloat64.getAsString());
    }
}
