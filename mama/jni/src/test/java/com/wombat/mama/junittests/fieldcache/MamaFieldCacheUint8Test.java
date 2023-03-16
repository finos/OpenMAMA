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
 * This class will test MamaFieldCacheUint8's functions
 */
public class MamaFieldCacheUint8Test extends MamaTestBaseTestCase
{
    /* ****************************************************** */
    /* Protected Member Variables. */
    /* ****************************************************** */
    
    protected MamaMsg mMsg;
    protected MamaFieldCacheUint8 mFieldCacheUint8;

    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Before
    public void setUp()
    {
        super.setUp();

        mFieldCacheUint8 = new MamaFieldCacheUint8(101, "example", true);
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
        mFieldCacheUint8.set((short)1);
        //add it to the message
        mFieldCacheUint8.addToMessage(false, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getU8("example", 101), 1);
    }

    @Test
    public void testAddToMessagefieldName()
    {
        //set the value of the field to true (the default, otherwise, is false
        mFieldCacheUint8.set((short)1);
        //add it to the message
        mFieldCacheUint8.addToMessage(true, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getU8("example", 101), 1);
    }

    @Test
    public void testCopy()
    {
        MamaFieldCacheField copyCache =  mFieldCacheUint8.copy();
        assertEquals(mFieldCacheUint8.getAsString(), copyCache.getAsString());
    }

    @Test
    public void testApplyMsgField()
    {
        MamaFieldCacheUint8 testCacheUint8 = new MamaFieldCacheUint8(102, "example2", true);
        mFieldCacheUint8.set((short)2);
        testCacheUint8.set((short)1);
        assertEquals(testCacheUint8.get(), 1);
        assertEquals(mFieldCacheUint8.get(), 2);
        mFieldCacheUint8.apply(testCacheUint8);
        assertEquals(mFieldCacheUint8.get(), 1);
    }

    @Test
    public void testApplyFieldCache()
    {
       MamaFieldCacheUint8 testField  = new MamaFieldCacheUint8(102, "example2", true);
       testField.set((short)1);
       mFieldCacheUint8.apply(testField);
       assertEquals(mFieldCacheUint8.get(), 1);
    }


    @Test
    public void testSet()
    {
        MamaFieldCacheUint8 testCacheUint8 = new MamaFieldCacheUint8(102, "example2", false);

        assertEquals(testCacheUint8.get(), 0);
        testCacheUint8.set((short)1);
        //set without track state
        assertEquals(testCacheUint8.get(), 1);

        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, testCacheUint8.getModState());

    }

    @Test
    public void testSetTrackState()
    {
        assertEquals(mFieldCacheUint8.get(), 0);
        mFieldCacheUint8.set((short)1);
        //set with track state
        assertEquals(mFieldCacheUint8.get(), 1);
        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, mFieldCacheUint8.getModState());
    }

    @Test
    public void testSetTrackStateTouched()
    {
    
        assertEquals(mFieldCacheUint8.get(), 0);
        mFieldCacheUint8.set((short)1);
        mFieldCacheUint8.set((short)1);
        //set with track state
        assertEquals(mFieldCacheUint8.get(), 1);
        assertEquals(MamaFieldCacheField.MOD_STATE_TOUCHED, mFieldCacheUint8.getModState());

    }

    @Test
    public void testIsEqual()
    {   
        MamaFieldCacheUint8 testCacheUint8 = new MamaFieldCacheUint8(102, "example2", true);
        testCacheUint8.set((short)1);
        mFieldCacheUint8.set((short)1);
        assertEquals(testCacheUint8.get(), 1);
        assertEquals(mFieldCacheUint8.get(), 1);
        assertTrue(testCacheUint8.isEqual(mFieldCacheUint8.get()));
    }

    @Test
    public void testGet()
    {
        assertEquals(mFieldCacheUint8.get(), 0);
        mFieldCacheUint8.set((short)1);
        assertEquals(mFieldCacheUint8.get(), 1);
    }

    @Test
    public void testGetAsString()
    {
        assertEquals(mFieldCacheUint8.get(), 0);
        assertEquals("0", mFieldCacheUint8.getAsString());
        mFieldCacheUint8.set((short)1);
        assertEquals(mFieldCacheUint8.get(), 1);
        assertEquals("1", mFieldCacheUint8.getAsString());
    }
}
