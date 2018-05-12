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
 * This class will test MamaFieldCachePrice's functions
 */
public class MamaFieldCachePriceTest extends TestCase
{
    /* ****************************************************** */
    /* Protected Member Variables. */
    /* ****************************************************** */
    
    protected MamaMsg mMsg;
    protected MamaFieldCachePrice mFieldCachePrice;
    protected MamaPrice           mprice;
    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Override
    protected void setUp()
    {
        mFieldCachePrice = new MamaFieldCachePrice(101, "example", true);
        mprice = new MamaPrice(10.01);
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
        mFieldCachePrice.set(mprice);
        //add it to the message
        mFieldCachePrice.addToMessage(false, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getPrice("example", 101).getValue(), 10.01);
    }

    public void testAddToMessagefieldName()
    {
        //set the value of the field to true (the default, otherwise, is false
        mFieldCachePrice.set(mprice);
        //add it to the message
        mFieldCachePrice.addToMessage(true, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getPrice("example", 101).getValue(), 10.01);
    }

    public void testCopy()
    {
        mFieldCachePrice.set(mprice);
        MamaFieldCacheField copyCache =  mFieldCachePrice.copy();
        assertEquals(mFieldCachePrice.getAsString(), copyCache.getAsString());
    }

    public void testApplyMsgField()
    {
        MamaFieldCachePrice testCachePrice = new MamaFieldCachePrice(102, "example2", true);
        testCachePrice.set(new MamaPrice(20.02));
        mFieldCachePrice.set(mprice);
        assertEquals(testCachePrice.get().getValue(), 20.02);
        assertEquals(mFieldCachePrice.get().getValue(), 10.01);
        mFieldCachePrice.apply(testCachePrice);
        assertEquals(mFieldCachePrice.get().getValue(), 20.02);
    }

    public void testApplyFieldCache()
    {
       MamaFieldCachePrice testField  = new MamaFieldCachePrice(102, "example2", true);
       testField.set(new MamaPrice(20.02));
       mFieldCachePrice.apply(testField);
       assertEquals(mFieldCachePrice.get().getValue(), 20.02);
    }


    public void testSet()
    {
        MamaFieldCachePrice testCachePrice = new MamaFieldCachePrice(102, "example2", false);

        assertEquals(testCachePrice.get(), null);
        testCachePrice.set(mprice);
        //set without track state
        assertEquals(testCachePrice.get().getValue(), 10.01);

        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, testCachePrice.getModState());

    }

    public void testSetTrackState()
    {
        assertEquals(mFieldCachePrice.get(), null);
        mFieldCachePrice.set(mprice);
        //set with track state
        assertEquals(mFieldCachePrice.get().getValue(), 10.01);
        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, mFieldCachePrice.getModState());
    }

    public void testIsEqual()
    {   
        MamaFieldCachePrice testCachePrice = new MamaFieldCachePrice(102, "example2", true);
        testCachePrice.set(mprice);
        mFieldCachePrice.set(mprice);
        assertEquals(testCachePrice.get().getValue(), 10.01);
        assertEquals(mFieldCachePrice.get().getValue(), 10.01);
        assertTrue(testCachePrice.isEqual(mFieldCachePrice.get()));
    }

    public void testGet()
    {
        assertEquals(mFieldCachePrice.get(), null);
        mFieldCachePrice.set(mprice);
        assertEquals(mFieldCachePrice.get(), 10.01);
    }

    public void testGetAsString()
    {
        assertEquals(mFieldCachePrice.get(), null);
        assertEquals("null", mFieldCachePrice.getAsString());
        mFieldCachePrice.set(mprice);
        assertEquals(mFieldCachePrice.get().getValue(), 10.01);
        assertEquals("10.01", mFieldCachePrice.getAsString());
    }

}
