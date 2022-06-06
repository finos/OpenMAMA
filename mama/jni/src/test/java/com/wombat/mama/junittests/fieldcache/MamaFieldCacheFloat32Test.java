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
 * This class will test MamaFieldCacheFloat32's functions
 */
public class MamaFieldCacheFloat32Test extends MamaTestBaseTestCase
{
    /* ****************************************************** */
    /* Protected Member Variables. */
    /* ****************************************************** */
    
    protected MamaMsg mMsg;
    protected MamaFieldCacheFloat32 mFieldCacheFloat32;

    protected Double mDelta = 0.0000001;

    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Before
    public void setUp()
    {
        super.setUp();

        mFieldCacheFloat32 = new MamaFieldCacheFloat32(101, "example", true);
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
        mFieldCacheFloat32.set(1.0f);
        //add it to the message
        mFieldCacheFloat32.addToMessage(false, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getF64("example", 101), 1.0, mDelta);
    }

    @Test
    public void testAddToMessagefieldName()
    {
        //set the value of the field to true (the default, otherwise, is false
        mFieldCacheFloat32.set(1.0f);
        //add it to the message
        mFieldCacheFloat32.addToMessage(true, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getF64("example", 101), 1.0, mDelta);
    }

    @Test
    public void testCopy()
    {
        MamaFieldCacheField copyCache =  mFieldCacheFloat32.copy();
        assertEquals(mFieldCacheFloat32.getAsString(), copyCache.getAsString());
    }

    @Test
    public void testApplyMsgField()
    {
        MamaFieldCacheFloat32 testCacheFloat32 = new MamaFieldCacheFloat32(102, "example2", true);
        mFieldCacheFloat32.set(2);
        testCacheFloat32.set(1.0f);
        assertEquals(testCacheFloat32.get(), 1.0f, mDelta);
        assertEquals(mFieldCacheFloat32.get(), 2.0f, mDelta);
        mFieldCacheFloat32.apply(testCacheFloat32);
        assertEquals(mFieldCacheFloat32.get(), 1.0f, mDelta);
    }

    @Test
    public void testApplyFieldCache()
    {
       MamaFieldCacheFloat32 testField  = new MamaFieldCacheFloat32(102, "example2", true);
       testField.set(1.0f);
       mFieldCacheFloat32.apply(testField);
       assertEquals(mFieldCacheFloat32.get(), 1.0f, mDelta);
    }


    @Test
    public void testSet()
    {
        MamaFieldCacheFloat32 testCacheFloat32 = new MamaFieldCacheFloat32(102, "example2", false);

        assertEquals(testCacheFloat32.get(), 0.0f, mDelta);
        testCacheFloat32.set(1.0f);
        //set without track state
        assertEquals(testCacheFloat32.get(), 1.0f, mDelta);

        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, testCacheFloat32.getModState());

    }

    @Test
    public void testSetTrackState()
    {
        assertEquals(mFieldCacheFloat32.get(), 0.0f, mDelta);
        mFieldCacheFloat32.set(1.0f);
        //set with track state
        assertEquals(mFieldCacheFloat32.get(), 1.0f, mDelta);
        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, mFieldCacheFloat32.getModState());
    }

    @Test
    public void testSetTrackStateTouched()
    {
    
        assertEquals(mFieldCacheFloat32.get(), 0.0f, mDelta);
        mFieldCacheFloat32.set(1.0f);
        mFieldCacheFloat32.set(1.0f);
        //set with track state
        assertEquals(mFieldCacheFloat32.get(), 1.0f, mDelta);
        assertEquals(MamaFieldCacheField.MOD_STATE_TOUCHED, mFieldCacheFloat32.getModState());

    }

    @Test
    public void testIsEqual()
    {   
        MamaFieldCacheFloat32 testCacheFloat32 = new MamaFieldCacheFloat32(102, "example2", true);
        testCacheFloat32.set(1.0f);
        mFieldCacheFloat32.set(1.0f);
        assertEquals(testCacheFloat32.get(), 1.0f, mDelta);
        assertEquals(mFieldCacheFloat32.get(), 1.0f, mDelta);
        assertTrue(testCacheFloat32.isEqual(mFieldCacheFloat32.get()));
    }

    @Test
    public void testGet()
    {
        assertEquals(mFieldCacheFloat32.get(), 0.0f, mDelta);
        mFieldCacheFloat32.set(1.0f);
        assertEquals(mFieldCacheFloat32.get(), 1.0f, mDelta);
    }

    @Test
    public void testGetAsString()
    {
        assertEquals(mFieldCacheFloat32.get(), 0.0f, mDelta);
        assertEquals("0.0", mFieldCacheFloat32.getAsString());
        mFieldCacheFloat32.set(1.0f);
        assertEquals(mFieldCacheFloat32.get(), 1.0f, mDelta);
        assertEquals("1.0", mFieldCacheFloat32.getAsString());
    }
}
