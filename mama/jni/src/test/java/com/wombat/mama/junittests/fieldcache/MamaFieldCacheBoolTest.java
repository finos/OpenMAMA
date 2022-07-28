package com.wombat.mama.junittests.fieldcache;

import com.wombat.mama.junittests.MamaTestBaseTestCase;
import com.wombat.mama.*;
import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import static org.junit.Assert.*;

/**
 *
 * This class will test MamaFieldCacheBool's functions
 */
public class MamaFieldCacheBoolTest extends MamaTestBaseTestCase
{
    /* ****************************************************** */
    /* Protected Member Variables. */
    /* ****************************************************** */
    
    protected MamaMsg mMsg;
    protected MamaFieldCacheBool mFieldCacheBool;

    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Before
    public void setUp()
    {
        super.setUp();
        mFieldCacheBool = new MamaFieldCacheBool(101, "example", true);
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
        mFieldCacheBool.set(true);
        //add it to the message
        mFieldCacheBool.addToMessage(false, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertTrue(mMsg.getBoolean("example", 101));
    }

    @Test
    public void testAddToMessagefieldName()
    {
        //set the value of the field to true (the default, otherwise, is false
        mFieldCacheBool.set(true);
        //add it to the message
        mFieldCacheBool.addToMessage(true, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertTrue(mMsg.getBoolean("example", 101));
    }

    @Test
    public void testCopy()
    {
        MamaFieldCacheField copyCache =  mFieldCacheBool.copy();
        assertEquals(mFieldCacheBool.getAsString(), copyCache.getAsString());
    }

    @Test
    public void testApplyMsgField()
    {
        MamaFieldCacheBool testCacheBool = new MamaFieldCacheBool(102, "example2", true);
        testCacheBool.set(true);
        assertTrue(testCacheBool.get());
        assertFalse(mFieldCacheBool.get());
        mFieldCacheBool.apply(testCacheBool);
        assertTrue(mFieldCacheBool.get());
    }

    @Test
    public void testApplyFieldCache()
    {
       MamaFieldCacheBool testField  = new MamaFieldCacheBool(102, "example2", true);
       testField.set(true);
       mFieldCacheBool.apply(testField);
       assertTrue(mFieldCacheBool.get());
    }


    @Test
    public void testSet()
    {
        MamaFieldCacheBool testCacheBool = new MamaFieldCacheBool(102, "example2", false);

        assertFalse(testCacheBool.get());
        testCacheBool.set(false);
        //unset without track state
        assertFalse(testCacheBool.get());

        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, testCacheBool.getModState());

        assertFalse(testCacheBool.get());
        testCacheBool.set(true);
        //set without track state
        assertTrue(testCacheBool.get());

        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, testCacheBool.getModState());

    }

    @Test
    public void testSetTrackState()
    {
        assertFalse(mFieldCacheBool.get());
        mFieldCacheBool.set(false);
        //unset with track state
        assertFalse(mFieldCacheBool.get());
        assertEquals(MamaFieldCacheField.MOD_STATE_TOUCHED, mFieldCacheBool.getModState());

        assertFalse(mFieldCacheBool.get());
        mFieldCacheBool.set(true);
        //set with track state
        assertTrue(mFieldCacheBool.get());
        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, mFieldCacheBool.getModState());
    }

    @Test
    public void testIsEqual()
    {   
        MamaFieldCacheBool testCacheBool = new MamaFieldCacheBool(102, "example2", true);
        testCacheBool.set(true);
        mFieldCacheBool.set(true);
        assertTrue(testCacheBool.get());
        assertTrue(mFieldCacheBool.get());
        assertTrue(testCacheBool.isEqual(mFieldCacheBool.get()));
    }

    @Test
    public void testGet()
    {
        assertFalse(mFieldCacheBool.get());
        mFieldCacheBool.set(true);
        assertTrue(mFieldCacheBool.get());
    }

    @Test
    public void testGetAsString()
    {
        assertFalse(mFieldCacheBool.get());
        assertEquals("false", mFieldCacheBool.getAsString());
        mFieldCacheBool.set(true);
        assertTrue(mFieldCacheBool.get());
        assertEquals("true", mFieldCacheBool.getAsString());
    }
}
