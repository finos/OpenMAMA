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
 * This class will test MamaFieldCacheChar's functions
 */
public class MamaFieldCacheCharTest extends TestCase
{
    /* ****************************************************** */
    /* Protected Member Variables. */
    /* ****************************************************** */
    
    protected MamaMsg mMsg;
    protected MamaFieldCacheChar mFieldCacheChar;

    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Override
    protected void setUp()
    {
        mFieldCacheChar = new MamaFieldCacheChar(101, "example", true);
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
        mFieldCacheChar.set('a');
        //add it to the message
        mFieldCacheChar.addToMessage(false, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getChar("example", 101), 'a');
    }

    public void testAddToMessagefieldName()
    {
        //set the value of the field to true (the default, otherwise, is false
        mFieldCacheChar.set('a');
        //add it to the message
        mFieldCacheChar.addToMessage(true, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getChar("example", 101), 'a');
    }

    public void testCopy()
    {
        MamaFieldCacheField copyCache =  mFieldCacheChar.copy();
        assertEquals(mFieldCacheChar.getAsString(), copyCache.getAsString());
    }

    public void testApplyMsgField()
    {
        MamaFieldCacheChar testCacheChar = new MamaFieldCacheChar(102, "example2", true);
        mFieldCacheChar.set('b');
        testCacheChar.set('a');
        assertEquals(testCacheChar.get(), 'a');
        assertEquals(mFieldCacheChar.get(), 'b');
        mFieldCacheChar.apply(testCacheChar);
        assertEquals(mFieldCacheChar.get(), 'a');
    }

    public void testApplyFieldCache()
    {
       MamaFieldCacheChar testField  = new MamaFieldCacheChar(102, "example2", true);
       testField.set('a');
       mFieldCacheChar.apply(testField);
       assertEquals(mFieldCacheChar.get(), 'a');
    }


    public void testSet()
    {
        MamaFieldCacheChar testCacheChar = new MamaFieldCacheChar(102, "example2", false);

        assertEquals(testCacheChar.get(), '\0');
        testCacheChar.set('a');
        //set without track state
        assertEquals(testCacheChar.get(), 'a');

        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, testCacheChar.getModState());

    }

    public void testSetTrackState()
    {
        assertEquals(mFieldCacheChar.get(), '\0');
        mFieldCacheChar.set('a');
        //set with track state
        assertEquals(mFieldCacheChar.get(), 'a');
        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, mFieldCacheChar.getModState());
    }

    public void testSetTrackStateTouched()
    {
    
        assertEquals(mFieldCacheChar.get(), '\0');
        mFieldCacheChar.set('a');
        mFieldCacheChar.set('a');
        //set with track state
        assertEquals(mFieldCacheChar.get(), 'a');
        assertEquals(MamaFieldCacheField.MOD_STATE_TOUCHED, mFieldCacheChar.getModState());

    }

    public void testIsEqual()
    {   
        MamaFieldCacheChar testCacheChar = new MamaFieldCacheChar(102, "example2", true);
        testCacheChar.set('a');
        mFieldCacheChar.set('a');
        assertEquals(testCacheChar.get(), 'a');
        assertEquals(mFieldCacheChar.get(), 'a');
        assertTrue(testCacheChar.isEqual(mFieldCacheChar.get()));
    }

    public void testGet()
    {
        assertEquals(mFieldCacheChar.get(), '\0');
        mFieldCacheChar.set('a');
        assertEquals(mFieldCacheChar.get(), 'a');
    }

    public void testGetAsString()
    {
        assertEquals(mFieldCacheChar.get(), '\0');
        assertEquals("\0", mFieldCacheChar.getAsString());
        mFieldCacheChar.set('a');
        assertEquals(mFieldCacheChar.get(), 'a');
        assertEquals("a", mFieldCacheChar.getAsString());
    }
}
