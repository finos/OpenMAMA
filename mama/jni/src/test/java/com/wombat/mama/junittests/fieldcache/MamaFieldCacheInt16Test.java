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
 * This class will test MamaFieldCacheInt16's functions
 */
public class MamaFieldCacheInt16Test extends MamaTestBaseTestCase
{
    /* ****************************************************** */
    /* Protected Member Variables. */
    /* ****************************************************** */
    
    protected MamaMsg mMsg;
    protected MamaFieldCacheInt16 mFieldCacheInt16;

    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Before
    public void setUp()
    {
        super.setUp();

        mFieldCacheInt16 = new MamaFieldCacheInt16(101, "example", true);
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
        mFieldCacheInt16.set((short) 1);
        //add it to the message
        mFieldCacheInt16.addToMessage(false, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getU16("example", 101), (short)1);
    }

    @Test
    public void testAddToMessagefieldName()
    {
        //set the value of the field to true (the default, otherwise, is false
        mFieldCacheInt16.set((short) 1);
        //add it to the message
        mFieldCacheInt16.addToMessage(true, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getI16("example", 101), (short) 1);
    }

    @Test
    public void testCopy()
    {
        MamaFieldCacheField copyCache =  mFieldCacheInt16.copy();
        assertEquals(mFieldCacheInt16.getAsString(), copyCache.getAsString());
    }

    @Test
    public void testApplyMsgField()
    {
        MamaFieldCacheInt16 testCacheInt16 = new MamaFieldCacheInt16(102, "example2", true);
        mFieldCacheInt16.set((short) 2);
        testCacheInt16.set((short) 1);
        assertEquals(testCacheInt16.get(), (short) 1);
        assertEquals(mFieldCacheInt16.get(), (short) 2);
        mFieldCacheInt16.apply(testCacheInt16);
        assertEquals(mFieldCacheInt16.get(), 1);
    }

    @Test
    public void testApplyFieldCache()
    {
       MamaFieldCacheInt16 testField  = new MamaFieldCacheInt16(102, "example2", true);
       testField.set((short) 1);
       mFieldCacheInt16.apply(testField);
       assertEquals(mFieldCacheInt16.get(), (short) 1);
    }


    @Test
    public void testSet()
    {
        MamaFieldCacheInt16 testCacheInt16 = new MamaFieldCacheInt16(102, "example2", false);

        assertEquals(testCacheInt16.get(), (short) 0);
        testCacheInt16.set((short) 1);
        //set without track state
        assertEquals(testCacheInt16.get(), (short) 1);

        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, testCacheInt16.getModState());

    }

    @Test
    public void testSetTrackState()
    {
        assertEquals(mFieldCacheInt16.get(), (short) 0);
        mFieldCacheInt16.set((short) 1);
        //set with track state
        assertEquals(mFieldCacheInt16.get(), (short) 1);
        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, mFieldCacheInt16.getModState());
    }

    @Test
    public void testSetTrackStateTouched()
    {
    
        assertEquals(mFieldCacheInt16.get(), (short) 0);
        mFieldCacheInt16.set((short) 1);
        mFieldCacheInt16.set((short) 1);
        //set with track state
        assertEquals(mFieldCacheInt16.get(), (short) 1);
        assertEquals(MamaFieldCacheField.MOD_STATE_TOUCHED, mFieldCacheInt16.getModState());

    }

    @Test
    public void testIsEqual()
    {   
        MamaFieldCacheInt16 testCacheInt16 = new MamaFieldCacheInt16(102, "example2", true);
        testCacheInt16.set((short) 1);
        mFieldCacheInt16.set((short) 1);
        assertEquals(testCacheInt16.get(), (short) 1);
        assertEquals(mFieldCacheInt16.get(), (short) 1);
        assertTrue(testCacheInt16.isEqual(mFieldCacheInt16.get()));
    }

    @Test
    public void testGet()
    {
        assertEquals(mFieldCacheInt16.get(), 0);
        mFieldCacheInt16.set((short) 1);
        assertEquals(mFieldCacheInt16.get(), (short) 1);
    }

    @Test
    public void testGetAsString()
    {
        assertEquals(mFieldCacheInt16.get(), 0);
        assertEquals("0", mFieldCacheInt16.getAsString());
        mFieldCacheInt16.set((short) 1);
        assertEquals(mFieldCacheInt16.get(), (short) 1);
        assertEquals("1", mFieldCacheInt16.getAsString());
    }
}
