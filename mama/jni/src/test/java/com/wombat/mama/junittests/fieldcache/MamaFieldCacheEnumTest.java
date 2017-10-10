
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
 * This class will test MamaFieldCacheEnum's functions
 */
public class MamaFieldCacheEnumTest extends TestCase
{
    /* ****************************************************** */
    /* Protected Member Variables. */
    /* ****************************************************** */
    
    protected MamaMsg mMsg;
    protected MamaFieldCacheEnum mFieldCacheEnum;

    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Override
    protected void setUp()
    {
        mFieldCacheEnum = new MamaFieldCacheEnum(101, "example", true);
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
        mFieldCacheEnum.set(1);
        //add it to the message
        mFieldCacheEnum.addToMessage(false, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getI32("example", 101), 1);
    }

    public void testAddToMessagefieldName()
    {
        //set the value of the field to true (the default, otherwise, is false
        mFieldCacheEnum.set(1);
        //add it to the message
        mFieldCacheEnum.addToMessage(true, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getI32("example", 101), 1);
    }

    public void testCopy()
    {
        MamaFieldCacheField copyCache =  mFieldCacheEnum.copy();
        assertEquals(mFieldCacheEnum.getAsString(), copyCache.getAsString());
    }

    public void testApplyMsgField()
    {
        MamaFieldCacheEnum testCacheEnum = new MamaFieldCacheEnum(102, "example2", true);
        mFieldCacheEnum.set(2);
        testCacheEnum.set(1);
        assertEquals(testCacheEnum.get(), 1);
        assertEquals(mFieldCacheEnum.get(), 2);
        mFieldCacheEnum.apply(testCacheEnum);
        assertEquals(mFieldCacheEnum.get(), 1);
    }

    public void testApplyFieldCache()
    {
       MamaFieldCacheEnum testField  = new MamaFieldCacheEnum(102, "example2", true);
       testField.set(1);
       mFieldCacheEnum.apply(testField);
       assertEquals(mFieldCacheEnum.get(), 1);
    }


    public void testSet()
    {
        MamaFieldCacheEnum testCacheEnum = new MamaFieldCacheEnum(102, "example2", false);

        assertEquals(testCacheEnum.get(), 0);
        testCacheEnum.set(1);
        //set without track state
        assertEquals(testCacheEnum.get(), 1);

        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, testCacheEnum.getModState());

    }

    public void testSetTrackState()
    {
        assertEquals(mFieldCacheEnum.get(), 0);
        mFieldCacheEnum.set(1);
        //set with track state
        assertEquals(mFieldCacheEnum.get(), 1);
        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, mFieldCacheEnum.getModState());
    }

    public void testSetTrackStateTouched()
    {
    
        assertEquals(mFieldCacheEnum.get(), 0);
        mFieldCacheEnum.set(1);
        mFieldCacheEnum.set(1);
        //set with track state
        assertEquals(mFieldCacheEnum.get(), 1);
        assertEquals(MamaFieldCacheField.MOD_STATE_TOUCHED, mFieldCacheEnum.getModState());

    }

    public void testIsEqual()
    {   
        MamaFieldCacheEnum testCacheEnum = new MamaFieldCacheEnum(102, "example2", true);
        testCacheEnum.set(1);
        mFieldCacheEnum.set(1);
        assertEquals(testCacheEnum.get(), 1);
        assertEquals(mFieldCacheEnum.get(), 1);
        assertTrue(testCacheEnum.isEqual(mFieldCacheEnum.get()));
    }

    public void testGet()
    {
        assertEquals(mFieldCacheEnum.get(), 0);
        mFieldCacheEnum.set(1);
        assertEquals(mFieldCacheEnum.get(), 1);
    }

    public void testGetAsString()
    {
        assertEquals(mFieldCacheEnum.get(), 0);
        assertEquals("0", mFieldCacheEnum.getAsString());
        mFieldCacheEnum.set(1);
        assertEquals(mFieldCacheEnum.get(), 1);
        assertEquals("1", mFieldCacheEnum.getAsString());
    }
}
