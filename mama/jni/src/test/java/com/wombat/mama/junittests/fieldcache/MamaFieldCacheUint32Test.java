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
 * This class will test MamaFieldCacheUint32's functions
 */
public class MamaFieldCacheUint32Test extends MamaTestBaseTestCase
{
    /* ****************************************************** */
    /* Protected Member Variables. */
    /* ****************************************************** */
    
    protected MamaMsg mMsg;
    protected MamaFieldCacheUint32 mFieldCacheUint32;

    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Before
    public void setUp()
    {
        super.setUp();
        mFieldCacheUint32 = new MamaFieldCacheUint32(101, "example", true);
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
        mFieldCacheUint32.set(1);
        //add it to the message
        mFieldCacheUint32.addToMessage(false, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getU32("example", 101), 1);
    }

    @Test
    public void testAddToMessagefieldName()
    {
        //set the value of the field to true (the default, otherwise, is false
        mFieldCacheUint32.set(1);
        //add it to the message
        mFieldCacheUint32.addToMessage(true, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getU32("example", 101), 1);
    }

    @Test
    public void testCopy()
    {
        MamaFieldCacheField copyCache =  mFieldCacheUint32.copy();
        assertEquals(mFieldCacheUint32.getAsString(), copyCache.getAsString());
    }

    @Test
    public void testApplyMsgField()
    {
        MamaFieldCacheUint32 testCacheUint32 = new MamaFieldCacheUint32(102, "example2", true);
        mFieldCacheUint32.set(2);
        testCacheUint32.set(1);
        assertEquals(testCacheUint32.get(), 1);
        assertEquals(mFieldCacheUint32.get(), 2);
        mFieldCacheUint32.apply(testCacheUint32);
        assertEquals(mFieldCacheUint32.get(), 1);
    }

    @Test
    public void testApplyFieldCache()
    {
       MamaFieldCacheUint32 testField  = new MamaFieldCacheUint32(102, "example2", true);
       testField.set(1);
       mFieldCacheUint32.apply(testField);
       assertEquals(mFieldCacheUint32.get(), 1);
    }


    @Test
    public void testSet()
    {
        MamaFieldCacheUint32 testCacheUint32 = new MamaFieldCacheUint32(102, "example2", false);

        assertEquals(testCacheUint32.get(), 0);
        testCacheUint32.set(1);
        //set without track state
        assertEquals(testCacheUint32.get(), 1);

        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, testCacheUint32.getModState());

    }

    @Test
    public void testSetTrackState()
    {
        assertEquals(mFieldCacheUint32.get(), 0);
        mFieldCacheUint32.set(1);
        //set with track state
        assertEquals(mFieldCacheUint32.get(), 1);
        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, mFieldCacheUint32.getModState());
    }

    @Test
    public void testSetTrackStateTouched()
    {
    
        assertEquals(mFieldCacheUint32.get(), 0);
        mFieldCacheUint32.set(1);
        mFieldCacheUint32.set(1);
        //set with track state
        assertEquals(mFieldCacheUint32.get(), 1);
        assertEquals(MamaFieldCacheField.MOD_STATE_TOUCHED, mFieldCacheUint32.getModState());

    }

    @Test
    public void testIsEqual()
    {   
        MamaFieldCacheUint32 testCacheUint32 = new MamaFieldCacheUint32(102, "example2", true);
        testCacheUint32.set(1);
        mFieldCacheUint32.set(1);
        assertEquals(testCacheUint32.get(), 1);
        assertEquals(mFieldCacheUint32.get(), 1);
        assertTrue(testCacheUint32.isEqual(mFieldCacheUint32.get()));
    }

    @Test
    public void testGet()
    {
        assertEquals(mFieldCacheUint32.get(), 0);
        mFieldCacheUint32.set(1);
        assertEquals(mFieldCacheUint32.get(), 1);
    }

    @Test
    public void testGetAsString()
    {
        assertEquals(mFieldCacheUint32.get(), 0);
        assertEquals("0", mFieldCacheUint32.getAsString());
        mFieldCacheUint32.set(1);
        assertEquals(mFieldCacheUint32.get(), 1);
        assertEquals("1", mFieldCacheUint32.getAsString());
    }
}
