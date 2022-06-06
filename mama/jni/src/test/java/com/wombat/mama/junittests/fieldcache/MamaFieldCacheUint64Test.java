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
 * This class will test MamaFieldCacheUint64's functions
 */
public class MamaFieldCacheUint64Test extends MamaTestBaseTestCase
{
    /* ****************************************************** */
    /* Protected Member Variables. */
    /* ****************************************************** */
    
    protected MamaMsg mMsg;
    protected MamaFieldCacheUint64 mFieldCacheUint64;

    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Before
    public void setUp()
    {
        super.setUp();

        mFieldCacheUint64 = new MamaFieldCacheUint64(101, "example", true);
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
        mFieldCacheUint64.set(1);
        //add it to the message
        mFieldCacheUint64.addToMessage(false, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getU64("example", 101), 1);
    }

    @Test
    public void testAddToMessagefieldName()
    {
        //set the value of the field to true (the default, otherwise, is false
        mFieldCacheUint64.set(1);
        //add it to the message
        mFieldCacheUint64.addToMessage(true, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getU64("example", 101), 1);
    }

    @Test
    public void testCopy()
    {
        MamaFieldCacheField copyCache =  mFieldCacheUint64.copy();
        assertEquals(mFieldCacheUint64.getAsString(), copyCache.getAsString());
    }

    @Test
    public void testApplyMsgField()
    {
        MamaFieldCacheUint64 testCacheUint64 = new MamaFieldCacheUint64(102, "example2", true);
        mFieldCacheUint64.set(2);
        testCacheUint64.set(1);
        assertEquals(testCacheUint64.get(), 1);
        assertEquals(mFieldCacheUint64.get(), 2);
        mFieldCacheUint64.apply(testCacheUint64);
        assertEquals(mFieldCacheUint64.get(), 1);
    }

    @Test
    public void testApplyFieldCache()
    {
       MamaFieldCacheUint64 testField  = new MamaFieldCacheUint64(102, "example2", true);
       testField.set(1);
       mFieldCacheUint64.apply(testField);
       assertEquals(mFieldCacheUint64.get(), 1);
    }


    @Test
    public void testSet()
    {
        MamaFieldCacheUint64 testCacheUint64 = new MamaFieldCacheUint64(102, "example2", false);

        assertEquals(testCacheUint64.get(), 0);
        testCacheUint64.set(1);
        //set without track state
        assertEquals(testCacheUint64.get(), 1);

        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, testCacheUint64.getModState());

    }

    @Test
    public void testSetTrackState()
    {
        assertEquals(mFieldCacheUint64.get(), 0);
        mFieldCacheUint64.set(1);
        //set with track state
        assertEquals(mFieldCacheUint64.get(), 1);
        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, mFieldCacheUint64.getModState());
    }

    @Test
    public void testSetTrackStateTouched()
    {
    
        assertEquals(mFieldCacheUint64.get(), 0);
        mFieldCacheUint64.set(1);
        mFieldCacheUint64.set(1);
        //set with track state
        assertEquals(mFieldCacheUint64.get(), 1);
        assertEquals(MamaFieldCacheField.MOD_STATE_TOUCHED, mFieldCacheUint64.getModState());

    }

    @Test
    public void testIsEqual()
    {   
        MamaFieldCacheUint64 testCacheUint64 = new MamaFieldCacheUint64(102, "example2", true);
        testCacheUint64.set(1);
        mFieldCacheUint64.set(1);
        assertEquals(testCacheUint64.get(), 1);
        assertEquals(mFieldCacheUint64.get(), 1);
        assertTrue(testCacheUint64.isEqual(mFieldCacheUint64.get()));
    }

    @Test
    public void testGet()
    {
        assertEquals(mFieldCacheUint64.get(), 0);
        mFieldCacheUint64.set(1);
        assertEquals(mFieldCacheUint64.get(), 1);
    }

    @Test
    public void testGetAsString()
    {
        assertEquals(mFieldCacheUint64.get(), 0);
        assertEquals("0", mFieldCacheUint64.getAsString());
        mFieldCacheUint64.set(1);
        assertEquals(mFieldCacheUint64.get(), 1);
        assertEquals("1", mFieldCacheUint64.getAsString());
    }
}
