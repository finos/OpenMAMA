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
 * This class will test MamaFieldCacheInt32's functions
 */
public class MamaFieldCacheInt32Test extends MamaTestBaseTestCase
{
    /* ****************************************************** */
    /* Protected Member Variables. */
    /* ****************************************************** */
    
    protected MamaMsg mMsg;
    protected MamaFieldCacheInt32 mFieldCacheInt32;

    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Before
    public void setUp()
    {
        super.setUp();

        mFieldCacheInt32 = new MamaFieldCacheInt32(101, "example", true);
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
        mFieldCacheInt32.set(1);
        //add it to the message
        mFieldCacheInt32.addToMessage(false, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getU32("example", 101), 1);
    }

    @Test
    public void testAddToMessagefieldName()
    {
        //set the value of the field to true (the default, otherwise, is false
        mFieldCacheInt32.set(1);
        //add it to the message
        mFieldCacheInt32.addToMessage(true, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getU32("example", 101), 1);
    }

    @Test
    public void testCopy()
    {
        MamaFieldCacheField copyCache =  mFieldCacheInt32.copy();
        assertEquals(mFieldCacheInt32.getAsString(), copyCache.getAsString());
    }

    @Test
    public void testApplyMsgField()
    {
        MamaFieldCacheInt32 testCacheInt32 = new MamaFieldCacheInt32(102, "example2", true);
        mFieldCacheInt32.set(2);
        testCacheInt32.set(1);
        assertEquals(testCacheInt32.get(), 1);
        assertEquals(mFieldCacheInt32.get(), 2);
        mFieldCacheInt32.apply(testCacheInt32);
        assertEquals(mFieldCacheInt32.get(), 1);
    }

    @Test
    public void testApplyFieldCache()
    {
       MamaFieldCacheInt32 testField  = new MamaFieldCacheInt32(102, "example2", true);
       testField.set(1);
       mFieldCacheInt32.apply(testField);
       assertEquals(mFieldCacheInt32.get(), 1);
    }


    @Test
    public void testSet()
    {
        MamaFieldCacheInt32 testCacheInt32 = new MamaFieldCacheInt32(102, "example2", false);

        assertEquals(testCacheInt32.get(), 0);
        testCacheInt32.set(1);
        //set without track state
        assertEquals(testCacheInt32.get(), 1);

        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, testCacheInt32.getModState());

    }

    @Test
    public void testSetTrackState()
    {
        assertEquals(mFieldCacheInt32.get(), 0);
        mFieldCacheInt32.set(1);
        //set with track state
        assertEquals(mFieldCacheInt32.get(), 1);
        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, mFieldCacheInt32.getModState());
    }

    @Test
    public void testSetTrackStateTouched()
    {
    
        assertEquals(mFieldCacheInt32.get(), 0);
        mFieldCacheInt32.set(1);
        mFieldCacheInt32.set(1);
        //set with track state
        assertEquals(mFieldCacheInt32.get(), 1);
        assertEquals(MamaFieldCacheField.MOD_STATE_TOUCHED, mFieldCacheInt32.getModState());

    }

    @Test
    public void testIsEqual()
    {   
        MamaFieldCacheInt32 testCacheInt32 = new MamaFieldCacheInt32(102, "example2", true);
        testCacheInt32.set(1);
        mFieldCacheInt32.set(1);
        assertEquals(testCacheInt32.get(), 1);
        assertEquals(mFieldCacheInt32.get(), 1);
        assertTrue(testCacheInt32.isEqual(mFieldCacheInt32.get()));
    }

    @Test
    public void testGet()
    {
        assertEquals(mFieldCacheInt32.get(), 0);
        mFieldCacheInt32.set(1);
        assertEquals(mFieldCacheInt32.get(), 1);
    }

    @Test
    public void testGetAsString()
    {
        assertEquals(mFieldCacheInt32.get(), 0);
        assertEquals("0", mFieldCacheInt32.getAsString());
        mFieldCacheInt32.set(1);
        assertEquals(mFieldCacheInt32.get(), 1);
        assertEquals("1", mFieldCacheInt32.getAsString());
    }
}
