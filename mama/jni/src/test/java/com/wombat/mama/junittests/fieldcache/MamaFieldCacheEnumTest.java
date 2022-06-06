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
 * This class will test MamaFieldCacheEnum's functions
 */
public class MamaFieldCacheEnumTest extends MamaTestBaseTestCase
{
    /* ****************************************************** */
    /* Protected Member Variables. */
    /* ****************************************************** */
    
    protected MamaMsg mMsg;
    protected MamaFieldCacheEnum mFieldCacheEnum;

    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Before
    public void setUp()
    {
        super.setUp();

        mFieldCacheEnum = new MamaFieldCacheEnum(101, "example", true);
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
        mFieldCacheEnum.set(1);
        //add it to the message
        mFieldCacheEnum.addToMessage(false, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getI32("example", 101), 1);
    }

    @Test
    public void testAddToMessagefieldName()
    {
        //set the value of the field to true (the default, otherwise, is false
        mFieldCacheEnum.set(1);
        //add it to the message
        mFieldCacheEnum.addToMessage(true, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getI32("example", 101), 1);
    }

    @Test
    public void testCopy()
    {
        MamaFieldCacheField copyCache =  mFieldCacheEnum.copy();
        assertEquals(mFieldCacheEnum.getAsString(), copyCache.getAsString());
    }

    @Test
    public void testApplyMsgField()
    {
        MamaFieldCacheEnum testCacheEnum = new MamaFieldCacheEnum(102, "example2", true);
        mFieldCacheEnum.set(2);
        testCacheEnum.set(1);
        assertEquals(testCacheEnum.get(), 1);
        assertEquals(mFieldCacheEnum.get(), 2);
        mFieldCacheEnum.apply(testCacheEnum);
        assertEquals(mFieldCacheEnum.get(), 1);
    }

    @Test
    public void testApplyFieldCache()
    {
       MamaFieldCacheEnum testField  = new MamaFieldCacheEnum(102, "example2", true);
       testField.set(1);
       mFieldCacheEnum.apply(testField);
       assertEquals(mFieldCacheEnum.get(), 1);
    }


    @Test
    public void testSet()
    {
        MamaFieldCacheEnum testCacheEnum = new MamaFieldCacheEnum(102, "example2", false);

        assertEquals(testCacheEnum.get(), 0);
        testCacheEnum.set(1);
        //set without track state
        assertEquals(testCacheEnum.get(), 1);

        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, testCacheEnum.getModState());

    }

    @Test
    public void testSetTrackState()
    {
        assertEquals(mFieldCacheEnum.get(), 0);
        mFieldCacheEnum.set(1);
        //set with track state
        assertEquals(mFieldCacheEnum.get(), 1);
        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, mFieldCacheEnum.getModState());
    }

    @Test
    public void testSetTrackStateTouched()
    {
    
        assertEquals(mFieldCacheEnum.get(), 0);
        mFieldCacheEnum.set(1);
        mFieldCacheEnum.set(1);
        //set with track state
        assertEquals(mFieldCacheEnum.get(), 1);
        assertEquals(MamaFieldCacheField.MOD_STATE_TOUCHED, mFieldCacheEnum.getModState());

    }

    @Test
    public void testIsEqual()
    {   
        MamaFieldCacheEnum testCacheEnum = new MamaFieldCacheEnum(102, "example2", true);
        testCacheEnum.set(1);
        mFieldCacheEnum.set(1);
        assertEquals(testCacheEnum.get(), 1);
        assertEquals(mFieldCacheEnum.get(), 1);
        assertTrue(testCacheEnum.isEqual(mFieldCacheEnum.get()));
    }

    @Test
    public void testGet()
    {
        assertEquals(mFieldCacheEnum.get(), 0);
        mFieldCacheEnum.set(1);
        assertEquals(mFieldCacheEnum.get(), 1);
    }

    @Test
    public void testGetAsString()
    {
        assertEquals(mFieldCacheEnum.get(), 0);
        assertEquals("0", mFieldCacheEnum.getAsString());
        mFieldCacheEnum.set(1);
        assertEquals(mFieldCacheEnum.get(), 1);
        assertEquals("1", mFieldCacheEnum.getAsString());
    }
}
