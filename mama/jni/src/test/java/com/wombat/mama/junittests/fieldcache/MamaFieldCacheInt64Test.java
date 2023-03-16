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
 * This class will test MamaFieldCacheInt64's functions
 */
public class MamaFieldCacheInt64Test extends MamaTestBaseTestCase
{
    /* ****************************************************** */
    /* Protected Member Variables. */
    /* ****************************************************** */
    
    protected MamaMsg mMsg;
    protected MamaFieldCacheInt64 mFieldCacheInt64;

    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Before
    public void setUp()
    {
        super.setUp();

        mFieldCacheInt64 = new MamaFieldCacheInt64(101, "example", true);
        mMsg = new MamaMsg();   
    }

    @After
    public void tearDown()
    {
        mMsg.destroy();
        super.tearDown();
    }

    /* ****************************************************** */
    /* Test Functions. */
    /* ****************************************************** */

    @Test
    public void testAddToMessage()
    {
        //set the value of the field to true (the default, otherwise, is false
        mFieldCacheInt64.set(1L);
        //add it to the message
        mFieldCacheInt64.addToMessage(false, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getU64("example", 101), 1L);
    }

    @Test
    public void testAddToMessagefieldName()
    {
        //set the value of the field to true (the default, otherwise, is false
        mFieldCacheInt64.set(1L);
        //add it to the message
        mFieldCacheInt64.addToMessage(true, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getU64("example", 101), 1L);
    }

    @Test
    public void testCopy()
    {
        MamaFieldCacheField copyCache =  mFieldCacheInt64.copy();
        assertEquals(mFieldCacheInt64.getAsString(), copyCache.getAsString());
    }

    @Test
    public void testApplyMsgField()
    {
        MamaFieldCacheInt64 testCacheInt64 = new MamaFieldCacheInt64(102, "example2L", true);
        mFieldCacheInt64.set(2L);
        testCacheInt64.set(1L);
        assertEquals(testCacheInt64.get(), 1L);
        assertEquals(mFieldCacheInt64.get(), 2L);
        mFieldCacheInt64.apply(testCacheInt64);
        assertEquals(mFieldCacheInt64.get(), 1L);
    }

    @Test
    public void testApplyFieldCache()
    {
       MamaFieldCacheInt64 testField  = new MamaFieldCacheInt64(102, "example2L", true);
       testField.set(1L);
       mFieldCacheInt64.apply(testField);
       assertEquals(mFieldCacheInt64.get(), 1L);
    }


    @Test
    public void testSet()
    {
        MamaFieldCacheInt64 testCacheInt64 = new MamaFieldCacheInt64(102, "example2L", false);

        assertEquals(testCacheInt64.get(), 0);
        testCacheInt64.set(1L);
        //set without track state
        assertEquals(testCacheInt64.get(), 1L);

        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, testCacheInt64.getModState());

    }

    @Test
    public void testSetTrackState()
    {
        assertEquals(mFieldCacheInt64.get(), 0);
        mFieldCacheInt64.set(1L);
        //set with track state
        assertEquals(mFieldCacheInt64.get(), 1L);
        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, mFieldCacheInt64.getModState());
    }

    @Test
    public void testSetTrackStateTouched()
    {
    
        assertEquals(mFieldCacheInt64.get(), 0);
        mFieldCacheInt64.set(1L);
        mFieldCacheInt64.set(1L);
        //set with track state
        assertEquals(mFieldCacheInt64.get(), 1L);
        assertEquals(MamaFieldCacheField.MOD_STATE_TOUCHED, mFieldCacheInt64.getModState());

    }

    @Test
    public void testIsEqual()
    {   
        MamaFieldCacheInt64 testCacheInt64 = new MamaFieldCacheInt64(102, "example2L", true);
        testCacheInt64.set(1L);
        mFieldCacheInt64.set(1L);
        assertEquals(testCacheInt64.get(), 1L);
        assertEquals(mFieldCacheInt64.get(), 1L);
        assertTrue(testCacheInt64.isEqual(mFieldCacheInt64.get()));
    }

    @Test
    public void testGet()
    {
        assertEquals(mFieldCacheInt64.get(), 0);
        mFieldCacheInt64.set(1L);
        assertEquals(mFieldCacheInt64.get(), 1L);
    }

    @Test
    public void testGetAsString()
    {
        assertEquals(mFieldCacheInt64.get(), 0);
        assertEquals("0", mFieldCacheInt64.getAsString());
        mFieldCacheInt64.set(1L);
        assertEquals(mFieldCacheInt64.get(), 1L);
        assertEquals("1", mFieldCacheInt64.getAsString());
    }
}
