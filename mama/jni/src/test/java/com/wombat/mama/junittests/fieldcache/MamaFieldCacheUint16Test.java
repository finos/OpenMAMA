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
 * This class will test MamaFieldCacheUint16's functions
 */
public class MamaFieldCacheUint16Test extends MamaTestBaseTestCase
{
    /* ****************************************************** */
    /* Protected Member Variables. */
    /* ****************************************************** */
    
    protected MamaMsg mMsg;
    protected MamaFieldCacheUint16 mFieldCacheUint16;

    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Before
    public void setUp()
    {
        super.setUp();

        mFieldCacheUint16 = new MamaFieldCacheUint16(101, "example", true);
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
        mFieldCacheUint16.set(1);
        //add it to the message
        mFieldCacheUint16.addToMessage(false, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getU16("example", 101), 1);
    }

    @Test
    public void testAddToMessagefieldName()
    {
        //set the value of the field to true (the default, otherwise, is false
        mFieldCacheUint16.set(1);
        //add it to the message
        mFieldCacheUint16.addToMessage(true, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getU16("example", 101), 1);
    }

    @Test
    public void testCopy()
    {
        MamaFieldCacheField copyCache =  mFieldCacheUint16.copy();
        assertEquals(mFieldCacheUint16.getAsString(), copyCache.getAsString());
    }

    @Test
    public void testApplyMsgField()
    {
        MamaFieldCacheUint16 testCacheUint16 = new MamaFieldCacheUint16(102, "example2", true);
        mFieldCacheUint16.set(2);
        testCacheUint16.set(1);
        assertEquals(testCacheUint16.get(), 1);
        assertEquals(mFieldCacheUint16.get(), 2);
        mFieldCacheUint16.apply(testCacheUint16);
        assertEquals(mFieldCacheUint16.get(), 1);
    }

    @Test
    public void testApplyFieldCache()
    {
       MamaFieldCacheUint16 testField  = new MamaFieldCacheUint16(102, "example2", true);
       testField.set(1);
       mFieldCacheUint16.apply(testField);
       assertEquals(mFieldCacheUint16.get(), 1);
    }


    @Test
    public void testSet()
    {
        MamaFieldCacheUint16 testCacheUint16 = new MamaFieldCacheUint16(102, "example2", false);

        assertEquals(testCacheUint16.get(), 0);
        testCacheUint16.set(1);
        //set without track state
        assertEquals(testCacheUint16.get(), 1);

        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, testCacheUint16.getModState());

    }

    @Test
    public void testSetTrackState()
    {
        assertEquals(mFieldCacheUint16.get(), 0);
        mFieldCacheUint16.set(1);
        //set with track state
        assertEquals(mFieldCacheUint16.get(), 1);
        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, mFieldCacheUint16.getModState());
    }

    @Test
    public void testSetTrackStateTouched()
    {
    
        assertEquals(mFieldCacheUint16.get(), 0);
        mFieldCacheUint16.set(1);
        mFieldCacheUint16.set(1);
        //set with track state
        assertEquals(mFieldCacheUint16.get(), 1);
        assertEquals(MamaFieldCacheField.MOD_STATE_TOUCHED, mFieldCacheUint16.getModState());

    }

    @Test
    public void testIsEqual()
    {   
        MamaFieldCacheUint16 testCacheUint16 = new MamaFieldCacheUint16(102, "example2", true);
        testCacheUint16.set(1);
        mFieldCacheUint16.set(1);
        assertEquals(testCacheUint16.get(), 1);
        assertEquals(mFieldCacheUint16.get(), 1);
        assertTrue(testCacheUint16.isEqual(mFieldCacheUint16.get()));
    }

    @Test
    public void testGet()
    {
        assertEquals(mFieldCacheUint16.get(), 0);
        mFieldCacheUint16.set(1);
        assertEquals(mFieldCacheUint16.get(), 1);
    }

    @Test
    public void testGetAsString()
    {
        assertEquals(mFieldCacheUint16.get(), 0);
        assertEquals("0", mFieldCacheUint16.getAsString());
        mFieldCacheUint16.set(1);
        assertEquals(mFieldCacheUint16.get(), 1);
        assertEquals("1", mFieldCacheUint16.getAsString());
    }
}
