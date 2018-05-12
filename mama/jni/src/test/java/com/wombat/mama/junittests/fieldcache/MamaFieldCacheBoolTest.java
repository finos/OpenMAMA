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
 * This class will test MamaFieldCacheBool's functions
 */
public class MamaFieldCacheBoolTest extends TestCase
{
    /* ****************************************************** */
    /* Protected Member Variables. */
    /* ****************************************************** */
    
    protected MamaMsg mMsg;
    protected MamaFieldCacheBool mFieldCacheBool;

    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Override
    protected void setUp()
    {
        mFieldCacheBool = new MamaFieldCacheBool(101, "example", true);
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
        mFieldCacheBool.set(true);
        //add it to the message
        mFieldCacheBool.addToMessage(false, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertTrue(mMsg.getBoolean("example", 101));
    }

    public void testAddToMessagefieldName()
    {
        //set the value of the field to true (the default, otherwise, is false
        mFieldCacheBool.set(true);
        //add it to the message
        mFieldCacheBool.addToMessage(true, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertTrue(mMsg.getBoolean("example", 101));
    }

    public void testCopy()
    {
        MamaFieldCacheField copyCache =  mFieldCacheBool.copy();
        assertEquals(mFieldCacheBool.getAsString(), copyCache.getAsString());
    }

    public void testApplyMsgField()
    {
        MamaFieldCacheBool testCacheBool = new MamaFieldCacheBool(102, "example2", true);
        testCacheBool.set(true);
        assertTrue(testCacheBool.get());
        assertFalse(mFieldCacheBool.get());
        mFieldCacheBool.apply(testCacheBool);
        assertTrue(mFieldCacheBool.get());
    }

    public void testApplyFieldCache()
    {
       MamaFieldCacheBool testField  = new MamaFieldCacheBool(102, "example2", true);
       testField.set(true);
       mFieldCacheBool.apply(testField);
       assertTrue(mFieldCacheBool.get());
    }


    public void testSet()
    {
        MamaFieldCacheBool testCacheBool = new MamaFieldCacheBool(102, "example2", false);

        assertFalse(testCacheBool.get());
        testCacheBool.set(false);
        //unset without track state
        assertFalse(testCacheBool.get());

        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, testCacheBool.getModState());

        assertFalse(testCacheBool.get());
        testCacheBool.set(true);
        //set without track state
        assertTrue(testCacheBool.get());

        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, testCacheBool.getModState());

    }

    public void testSetTrackState()
    {
        assertFalse(mFieldCacheBool.get());
        mFieldCacheBool.set(false);
        //unset with track state
        assertFalse(mFieldCacheBool.get());
        assertEquals(MamaFieldCacheField.MOD_STATE_TOUCHED, mFieldCacheBool.getModState());

        assertFalse(mFieldCacheBool.get());
        mFieldCacheBool.set(true);
        //set with track state
        assertTrue(mFieldCacheBool.get());
        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, mFieldCacheBool.getModState());
    }

    public void testIsEqual()
    {   
        MamaFieldCacheBool testCacheBool = new MamaFieldCacheBool(102, "example2", true);
        testCacheBool.set(true);
        mFieldCacheBool.set(true);
        assertTrue(testCacheBool.get());
        assertTrue(mFieldCacheBool.get());
        assertTrue(testCacheBool.isEqual(mFieldCacheBool.get()));
    }

    public void testGet()
    {
        assertFalse(mFieldCacheBool.get());
        mFieldCacheBool.set(true);
        assertTrue(mFieldCacheBool.get());
    }

    public void testGetAsString()
    {
        assertFalse(mFieldCacheBool.get());
        assertEquals("false", mFieldCacheBool.getAsString());
        mFieldCacheBool.set(true);
        assertTrue(mFieldCacheBool.get());
        assertEquals("true", mFieldCacheBool.getAsString());
    }
}
