package com.wombat.mama.junittests.fieldcache;

import com.wombat.mama.junittests.MamaTestBaseTestCase;
import com.wombat.mama.*;
import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;

/**
 *
 * This class will test MamaFieldCacheFloat64's functions
 */
public class MamaFieldCacheFloat64Test extends MamaTestBaseTestCase
{
    /* ****************************************************** */
    /* Protected Member Variables. */
    /* ****************************************************** */
    
    protected MamaMsg mMsg;
    protected MamaFieldCacheFloat64 mFieldCacheFloat64;

    protected Double mDelta = 0.00000001;

    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Before
    public void setUp()
    {
        super.setUp();

        mFieldCacheFloat64 = new MamaFieldCacheFloat64(101, "example", true);
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
        //set the value of the field to true (the default, otherwise, is false
        mFieldCacheFloat64.set(1.0);
        //add it to the message
        mFieldCacheFloat64.addToMessage(false, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getF64("example", 101), 1.0, mDelta);
    }

    @Test
    public void testAddToMessagefieldName()
    {
        //set the value of the field to true (the default, otherwise, is false
        mFieldCacheFloat64.set(1.0);
        //add it to the message
        mFieldCacheFloat64.addToMessage(true, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getF64("example", 101), 1.0, mDelta);
    }

    @Test
    public void testCopy()
    {
        MamaFieldCacheField copyCache =  mFieldCacheFloat64.copy();
        assertEquals(mFieldCacheFloat64.getAsString(), copyCache.getAsString());
    }

    @Test
    public void testApplyMsgField()
    {
        MamaFieldCacheFloat64 testCacheFloat64 = new MamaFieldCacheFloat64(102, "example2", true);
        mFieldCacheFloat64.set(2);
        testCacheFloat64.set(1.0);
        assertEquals(testCacheFloat64.get(), 1.0, mDelta);
        assertEquals(mFieldCacheFloat64.get(), 2.0, mDelta);
        mFieldCacheFloat64.apply(testCacheFloat64);
        assertEquals(mFieldCacheFloat64.get(), 1.0, mDelta);
    }

    @Test
    public void testApplyFieldCache()
    {
       MamaFieldCacheFloat64 testField  = new MamaFieldCacheFloat64(102, "example2", true);
       testField.set(1.0);
       mFieldCacheFloat64.apply(testField);
       assertEquals(mFieldCacheFloat64.get(), 1.0, mDelta);
    }


    @Test
    public void testSet()
    {
        MamaFieldCacheFloat64 testCacheFloat64 = new MamaFieldCacheFloat64(102, "example2", false);

        assertEquals(testCacheFloat64.get(), 0.0, mDelta);
        testCacheFloat64.set(1.0);
        //set without track state
        assertEquals(testCacheFloat64.get(), 1.0, mDelta);

        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, testCacheFloat64.getModState());

    }

    @Test
    public void testSetTrackState()
    {
        assertEquals(mFieldCacheFloat64.get(), 0.0, mDelta);
        mFieldCacheFloat64.set(1.0);
        //set with track state
        assertEquals(mFieldCacheFloat64.get(), 1.0, mDelta);
        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, mFieldCacheFloat64.getModState());
    }

    @Test
    public void testSetTrackStateTouched()
    {
    
        assertEquals(mFieldCacheFloat64.get(), 0.0, mDelta);
        mFieldCacheFloat64.set(1.0);
        mFieldCacheFloat64.set(1.0);
        //set with track state
        assertEquals(mFieldCacheFloat64.get(), 1.0, mDelta);
        assertEquals(MamaFieldCacheField.MOD_STATE_TOUCHED, mFieldCacheFloat64.getModState());

    }

    @Test
    public void testIsEqual()
    {   
        MamaFieldCacheFloat64 testCacheFloat64 = new MamaFieldCacheFloat64(102, "example2", true);
        testCacheFloat64.set(1.0);
        mFieldCacheFloat64.set(1.0);
        assertEquals(testCacheFloat64.get(), 1.0, mDelta);
        assertEquals(mFieldCacheFloat64.get(), 1.0, mDelta);
        assertTrue(testCacheFloat64.isEqual(mFieldCacheFloat64.get()));
    }

    @Test
    public void testGet()
    {
        assertEquals(mFieldCacheFloat64.get(), 0.0, mDelta);
        mFieldCacheFloat64.set(1.0);
        assertEquals(mFieldCacheFloat64.get(), 1.0, mDelta);
    }

    @Test
    public void testGetAsString()
    {
        assertEquals(mFieldCacheFloat64.get(), 0.0, mDelta);
        assertEquals("0.0", mFieldCacheFloat64.getAsString());
        mFieldCacheFloat64.set(1.0);
        assertEquals(mFieldCacheFloat64.get(), 1.0, mDelta);
        assertEquals("1.0", mFieldCacheFloat64.getAsString());
    }
}
