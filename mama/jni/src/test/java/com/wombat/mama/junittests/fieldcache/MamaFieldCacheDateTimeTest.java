package com.wombat.mama.junittests.fieldcache;

import com.wombat.mama.junittests.MamaTestBaseTestCase;
import com.wombat.mama.*;
import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import static org.junit.Assert.*;

/**
 *
 * This class will test MamaFieldCacheDateTime's functions
 */
public class MamaFieldCacheDateTimeTest extends MamaTestBaseTestCase
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

    @Before
    public void setUp()
    {
        super.setUp();

        mFieldCacheDateTime = new MamaFieldCacheDateTime(101, "example", true);
        mdateTime = new MamaDateTime();
        mtimeZone = MamaTimeZone.utc();
        mdateTime.setTime(9,0,0,0L, MamaDateTimePrecision.PREC_SECONDS, mtimeZone);
        mMsg = new MamaMsg(); 
    }

    @After
    public void tearDown()
    {
        super.tearDown();
    }

    /* ****************************************************** */
    /* Test Functions. */
    /* ****************************************************** */

    @Test
    public void testAddToMessage()
    {
        mFieldCacheDateTime.set(mdateTime);
        //add it to the message
        mFieldCacheDateTime.addToMessage(false, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getDateTime("example", 101).getAsString(), "09:00:00");
    }

    @Test
    public void testAddToMessagefieldName()
    {
        mFieldCacheDateTime.set(mdateTime);
        //add it to the message
        mFieldCacheDateTime.addToMessage(true, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getDateTime("example", 101).getAsString(), "09:00:00");
    }

    @Test
    public void testCopy()
    {
        mFieldCacheDateTime.set(mdateTime);
        MamaFieldCacheField copyCache =  mFieldCacheDateTime.copy();
        assertEquals(mFieldCacheDateTime.getAsString(), copyCache.getAsString());
    }

    @Test
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

    @Test
    public void testApplyFieldCache()
    {
       MamaFieldCacheDateTime testField  = new MamaFieldCacheDateTime(102, "example2", true);
       testField.set(mdateTime);
       mFieldCacheDateTime.apply(testField);
       assertEquals(mFieldCacheDateTime.get().getAsString(), nineAM);
    }


    @Test
    public void testSet()
    {
        MamaFieldCacheDateTime testCacheDateTime = new MamaFieldCacheDateTime(102, "example2", false);
        assertNull(testCacheDateTime.get());
        testCacheDateTime.set(mdateTime);
        //set without track state
        assertEquals(testCacheDateTime.get().getAsString(), nineAM);

        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, testCacheDateTime.getModState());
    }

    @Test
    public void testSetTrackState()
    {
        assertNull(mFieldCacheDateTime.get());
        mFieldCacheDateTime.set(mdateTime);
        //set with track state
        assertEquals(mFieldCacheDateTime.get().getAsString(), nineAM);
        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, mFieldCacheDateTime.getModState());
    }

    @Test
    public void testSetTrackStateTouched()
    {
        assertNull(mFieldCacheDateTime.get());
        mFieldCacheDateTime.set(mdateTime);
        mFieldCacheDateTime.set(mdateTime);
        //set with track state
        assertEquals(mFieldCacheDateTime.get().getAsString(), nineAM);
        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, mFieldCacheDateTime.getModState());

    }

    @Test
    public void testIsEqual()
    {   
        MamaFieldCacheDateTime testCacheDateTime = new MamaFieldCacheDateTime(102, "example2", true);
        testCacheDateTime.set(mdateTime);
        mFieldCacheDateTime.set(mdateTime);
        assertEquals(testCacheDateTime.get().getAsString(), nineAM);
        assertEquals(mFieldCacheDateTime.get().getAsString(), nineAM);
        assertTrue(testCacheDateTime.isEqual(mFieldCacheDateTime.get()));
    }

    @Test
    public void testGet()
    {
        assertNull(mFieldCacheDateTime.get());
        mFieldCacheDateTime.set(mdateTime);
        assertEquals(mFieldCacheDateTime.get().getAsString(), nineAM);
    }

    @Test
    public void testGetAsString()
    {
        assertNull(mFieldCacheDateTime.get());
        assertEquals("null", mFieldCacheDateTime.getAsString());
        mFieldCacheDateTime.set(mdateTime);
        assertEquals(mFieldCacheDateTime.get().getAsString(), nineAM);
    }
}
