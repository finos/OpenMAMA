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
 * This class will test MamaFieldCacheInt8's functions
 */
public class MamaFieldCacheInt8Test extends MamaTestBaseTestCase
{
    /* ****************************************************** */
    /* Protected Member Variables. */
    /* ****************************************************** */
    
    protected MamaMsg mMsg;
    protected MamaFieldCacheInt8 mFieldCacheInt8;

    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Before
    public void setUp()
    {
        super.setUp();

        mFieldCacheInt8 = new MamaFieldCacheInt8(101, "example", true);
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
        mFieldCacheInt8.set((byte) 1);
        //add it to the message
        mFieldCacheInt8.addToMessage(false, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getU8("example", 101), (byte)1);
    }

    @Test
    public void testAddToMessagefieldName()
    {
        //set the value of the field to true (the default, otherwise, is false
        mFieldCacheInt8.set((byte) 1);
        //add it to the message
        mFieldCacheInt8.addToMessage(true, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getI8("example", 101), (byte) 1);
    }

    @Test
    public void testCopy()
    {
        MamaFieldCacheField copyCache =  mFieldCacheInt8.copy();
        assertEquals(mFieldCacheInt8.getAsString(), copyCache.getAsString());
    }

    @Test
    public void testApplyMsgField()
    {
        MamaFieldCacheInt8 testCacheInt8 = new MamaFieldCacheInt8(102, "example2", true);
        mFieldCacheInt8.set((byte) 2);
        testCacheInt8.set((byte) 1);
        assertEquals(testCacheInt8.get(), (byte) 1);
        assertEquals(mFieldCacheInt8.get(), (byte) 2);
        mFieldCacheInt8.apply(testCacheInt8);
        assertEquals(mFieldCacheInt8.get(), 1);
    }

    @Test
    public void testApplyFieldCache()
    {
       MamaFieldCacheInt8 testField  = new MamaFieldCacheInt8(102, "example2", true);
       testField.set((byte) 1);
       mFieldCacheInt8.apply(testField);
       assertEquals(mFieldCacheInt8.get(), (byte) 1);
    }


    @Test
    public void testSet()
    {
        MamaFieldCacheInt8 testCacheInt8 = new MamaFieldCacheInt8(102, "example2", false);

        assertEquals(testCacheInt8.get(), (byte) 0);
        testCacheInt8.set((byte) 1);
        //set without track state
        assertEquals(testCacheInt8.get(), (byte) 1);

        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, testCacheInt8.getModState());

    }

    @Test
    public void testSetTrackState()
    {
        assertEquals(mFieldCacheInt8.get(), (byte) 0);
        mFieldCacheInt8.set((byte) 1);
        //set with track state
        assertEquals(mFieldCacheInt8.get(), (byte) 1);
        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, mFieldCacheInt8.getModState());
    }

    @Test
    public void testSetTrackStateTouched()
    {
    
        assertEquals(mFieldCacheInt8.get(), (byte) 0);
        mFieldCacheInt8.set((byte) 1);
        mFieldCacheInt8.set((byte) 1);
        //set with track state
        assertEquals(mFieldCacheInt8.get(), (byte) 1);
        assertEquals(MamaFieldCacheField.MOD_STATE_TOUCHED, mFieldCacheInt8.getModState());

    }

    @Test
    public void testIsEqual()
    {   
        MamaFieldCacheInt8 testCacheInt8 = new MamaFieldCacheInt8(102, "example2", true);
        testCacheInt8.set((byte) 1);
        mFieldCacheInt8.set((byte) 1);
        assertEquals(testCacheInt8.get(), (byte) 1);
        assertEquals(mFieldCacheInt8.get(), (byte) 1);
        assertTrue(testCacheInt8.isEqual(mFieldCacheInt8.get()));
    }

    @Test
    public void testGet()
    {
        assertEquals(mFieldCacheInt8.get(), 0);
        mFieldCacheInt8.set((byte) 1);
        assertEquals(mFieldCacheInt8.get(), (byte) 1);
    }

    @Test
    public void testGetAsString()
    {
        assertEquals(mFieldCacheInt8.get(), 0);
        assertEquals("0", mFieldCacheInt8.getAsString());
        mFieldCacheInt8.set((byte) 1);
        assertEquals(mFieldCacheInt8.get(), (byte) 1);
        assertEquals("1", mFieldCacheInt8.getAsString());
    }
}
