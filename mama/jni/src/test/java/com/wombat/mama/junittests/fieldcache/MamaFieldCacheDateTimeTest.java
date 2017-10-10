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
import java.util.TimeZone;

/**
 *
 * This class will test MamaFieldCacheDateTime's functions
 */
public class MamaFieldCacheDateTimeTest extends TestCase
{
    /* ****************************************************** */
    /* Protected Member Variables. */
    /* ****************************************************** */
    
    protected MamaMsg mMsg;
    protected MamaFieldCacheDateTime mFieldCacheDateTime;
    protected MamaDateTime mdateTime;
    protected MamaTimeZone mtimeZone;
    protected String nineAM = "09:00:00";
    protected String tenAM  = "10:00:00";
    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Override
    protected void setUp()
    {
        mFieldCacheDateTime = new MamaFieldCacheDateTime(101, "example", true);
        mdateTime = new MamaDateTime();
        mtimeZone = new MamaTimeZone ();
        mtimeZone = mtimeZone.utc();
        mdateTime.setTime(9,0,0,0L, MamaDateTimePrecision.PREC_SECONDS, mtimeZone);
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
        mFieldCacheDateTime.set(mdateTime);
        //add it to the message
        mFieldCacheDateTime.addToMessage(false, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getDateTime("example", 101).getAsString(), "09:00:00");
    }

    public void testAddToMessagefieldName()
    {
        mFieldCacheDateTime.set(mdateTime);
        //add it to the message
        mFieldCacheDateTime.addToMessage(true, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getDateTime("example", 101).getAsString(), "09:00:00");
    }

    public void testCopy()
    {
        mFieldCacheDateTime.set(mdateTime);
        MamaFieldCacheField copyCache =  mFieldCacheDateTime.copy();
        assertEquals(mFieldCacheDateTime.getAsString(), copyCache.getAsString());
    }

    public void testApplyMsgField()
    {
        MamaFieldCacheDateTime testCacheDateTime = new MamaFieldCacheDateTime(102, "example2", true);
        mFieldCacheDateTime.set(mdateTime);
        
        mdateTime.setTime(10,0,0,0L, MamaDateTimePrecision.PREC_SECONDS, mtimeZone);
        testCacheDateTime.set(mdateTime);
        assertEquals(testCacheDateTime.get().getAsString(), tenAM);
        assertEquals(mFieldCacheDateTime.get().getAsString(), nineAM);
        mFieldCacheDateTime.apply(testCacheDateTime);
        assertEquals(mFieldCacheDateTime.get().getAsString(), tenAM);
    }

    public void testApplyFieldCache()
    {
       MamaFieldCacheDateTime testField  = new MamaFieldCacheDateTime(102, "example2", true);
       testField.set(mdateTime);
       mFieldCacheDateTime.apply(testField);
       assertEquals(mFieldCacheDateTime.get().getAsString(), nineAM);
    }


    public void testSet()
    {
        MamaFieldCacheDateTime testCacheDateTime = new MamaFieldCacheDateTime(102, "example2", false);
        assertEquals(testCacheDateTime.get(), null);
        testCacheDateTime.set(mdateTime);
        //set without track state
        assertEquals(testCacheDateTime.get().getAsString(), nineAM);

        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, testCacheDateTime.getModState());
    }

    public void testSetTrackState()
    {
        assertEquals(mFieldCacheDateTime.get(), null);
        mFieldCacheDateTime.set(mdateTime);
        //set with track state
        assertEquals(mFieldCacheDateTime.get().getAsString(), nineAM);
        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, mFieldCacheDateTime.getModState());
    }

    public void testSetTrackStateTouched()
    {
        assertEquals(mFieldCacheDateTime.get(), null);
        mFieldCacheDateTime.set(mdateTime);
        mFieldCacheDateTime.set(mdateTime);
        //set with track state
        assertEquals(mFieldCacheDateTime.get().getAsString(), nineAM);
        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, mFieldCacheDateTime.getModState());

    }

    public void testIsEqual()
    {   
        MamaFieldCacheDateTime testCacheDateTime = new MamaFieldCacheDateTime(102, "example2", true);
        testCacheDateTime.set(mdateTime);
        mFieldCacheDateTime.set(mdateTime);
        assertEquals(testCacheDateTime.get().getAsString(), nineAM);
        assertEquals(mFieldCacheDateTime.get().getAsString(), nineAM);
        assertTrue(testCacheDateTime.isEqual(mFieldCacheDateTime.get()));
    }

    public void testGet()
    {
        assertEquals(mFieldCacheDateTime.get(), null);
        mFieldCacheDateTime.set(mdateTime);
        assertEquals(mFieldCacheDateTime.get().getAsString(), nineAM);
    }

    public void testGetAsString()
    {
        assertEquals(mFieldCacheDateTime.get(), null);
        assertEquals("null", mFieldCacheDateTime.getAsString());
        mFieldCacheDateTime.set(mdateTime);
        assertEquals(mFieldCacheDateTime.get().getAsString(), nineAM);
    }
}
