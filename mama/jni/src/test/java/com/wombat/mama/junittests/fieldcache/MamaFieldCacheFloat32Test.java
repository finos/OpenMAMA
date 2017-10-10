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
 * This class will test MamaFieldCacheFloat32's functions
 */
public class MamaFieldCacheFloat32Test extends TestCase
{
    /* ****************************************************** */
    /* Protected Member Variables. */
    /* ****************************************************** */
    
    protected MamaMsg mMsg;
    protected MamaFieldCacheFloat32 mFieldCacheFloat32;

    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Override
    protected void setUp()
    {
        mFieldCacheFloat32 = new MamaFieldCacheFloat32(101, "example", true);
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
        mFieldCacheFloat32.set(1.0f);
        //add it to the message
        mFieldCacheFloat32.addToMessage(false, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getF64("example", 101), 1.0);
    }

    public void testAddToMessagefieldName()
    {
        //set the value of the field to true (the default, otherwise, is false
        mFieldCacheFloat32.set(1.0f);
        //add it to the message
        mFieldCacheFloat32.addToMessage(true, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getF64("example", 101), 1.0);
    }

    public void testCopy()
    {
        MamaFieldCacheField copyCache =  mFieldCacheFloat32.copy();
        assertEquals(mFieldCacheFloat32.getAsString(), copyCache.getAsString());
    }

    public void testApplyMsgField()
    {
        MamaFieldCacheFloat32 testCacheFloat32 = new MamaFieldCacheFloat32(102, "example2", true);
        mFieldCacheFloat32.set(2);
        testCacheFloat32.set(1.0f);
        assertEquals(testCacheFloat32.get(), 1.0f);
        assertEquals(mFieldCacheFloat32.get(), 2.0f);
        mFieldCacheFloat32.apply(testCacheFloat32);
        assertEquals(mFieldCacheFloat32.get(), 1.0f);
    }

    public void testApplyFieldCache()
    {
       MamaFieldCacheFloat32 testField  = new MamaFieldCacheFloat32(102, "example2", true);
       testField.set(1.0f);
       mFieldCacheFloat32.apply(testField);
       assertEquals(mFieldCacheFloat32.get(), 1.0f);
    }


    public void testSet()
    {
        MamaFieldCacheFloat32 testCacheFloat32 = new MamaFieldCacheFloat32(102, "example2", false);

        assertEquals(testCacheFloat32.get(), 0.0f);
        testCacheFloat32.set(1.0f);
        //set without track state
        assertEquals(testCacheFloat32.get(), 1.0f);

        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, testCacheFloat32.getModState());

    }

    public void testSetTrackState()
    {
        assertEquals(mFieldCacheFloat32.get(), 0.0f);
        mFieldCacheFloat32.set(1.0f);
        //set with track state
        assertEquals(mFieldCacheFloat32.get(), 1.0f);
        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, mFieldCacheFloat32.getModState());
    }

    public void testSetTrackStateTouched()
    {
    
        assertEquals(mFieldCacheFloat32.get(), 0.0f);
        mFieldCacheFloat32.set(1.0f);
        mFieldCacheFloat32.set(1.0f);
        //set with track state
        assertEquals(mFieldCacheFloat32.get(), 1.0f);
        assertEquals(MamaFieldCacheField.MOD_STATE_TOUCHED, mFieldCacheFloat32.getModState());

    }

    public void testIsEqual()
    {   
        MamaFieldCacheFloat32 testCacheFloat32 = new MamaFieldCacheFloat32(102, "example2", true);
        testCacheFloat32.set(1.0f);
        mFieldCacheFloat32.set(1.0f);
        assertEquals(testCacheFloat32.get(), 1.0f);
        assertEquals(mFieldCacheFloat32.get(), 1.0f);
        assertTrue(testCacheFloat32.isEqual(mFieldCacheFloat32.get()));
    }

    public void testGet()
    {
        assertEquals(mFieldCacheFloat32.get(), 0.0f);
        mFieldCacheFloat32.set(1.0f);
        assertEquals(mFieldCacheFloat32.get(), 1.0f);
    }

    public void testGetAsString()
    {
        assertEquals(mFieldCacheFloat32.get(), 0.0f);
        assertEquals("0.0", mFieldCacheFloat32.getAsString());
        mFieldCacheFloat32.set(1.0f);
        assertEquals(mFieldCacheFloat32.get(), 1.0f);
        assertEquals("1.0", mFieldCacheFloat32.getAsString());
    }
}
