package com.wombat.mama.junittests.fieldcache;

import com.wombat.mama.junittests.MamaTestBaseTestCase;
import com.wombat.mama.*;
import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import static org.junit.Assert.*;

/**
 *
 * This class will test MamaFieldCacheString's functions
 */
public class MamaFieldCacheStringTest extends MamaTestBaseTestCase
{
    /* ****************************************************** */
    /* Protected Member Variables. */
    /* ****************************************************** */
    
    protected MamaMsg mMsg;
    protected MamaFieldCacheString mFieldCacheString;

    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Before
    public void setUp()
    {
        super.setUp();

        mFieldCacheString = new MamaFieldCacheString(101, "example", true);
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
        mFieldCacheString.set("a");
        //add it to the message
        mFieldCacheString.addToMessage(false, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getString("example", 101), "a");
    }

    @Test
    public void testAddToMessagefieldName()
    {
        //set the value of the field to true (the default, otherwise, is false
        mFieldCacheString.set("a");
        //add it to the message
        mFieldCacheString.addToMessage(true, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getString("example", 101), "a");
    }

    @Test
    public void testCopy()
    {
        MamaFieldCacheField copyCache =  mFieldCacheString.copy();
        assertEquals(mFieldCacheString.getAsString(), copyCache.getAsString());
    }

    @Test
    public void testApplyMsgField()
    {
        MamaFieldCacheString testCacheString = new MamaFieldCacheString(102, "example2", true);
        mFieldCacheString.set("b");
        testCacheString.set("a");
        assertEquals(testCacheString.get(), "a");
        assertEquals(mFieldCacheString.get(), "b");
        mFieldCacheString.apply(testCacheString);
        assertEquals(mFieldCacheString.get(), "a");
    }

    @Test
    public void testApplyFieldCache()
    {
       MamaFieldCacheString testField  = new MamaFieldCacheString(102, "example2", true);
       testField.set("a");
       mFieldCacheString.apply(testField);
       assertEquals(mFieldCacheString.get(), "a");
    }


    @Test
    public void testSet()
    {
        MamaFieldCacheString testCacheString = new MamaFieldCacheString(102, "example2", false);

        assertNull(testCacheString.get());
        testCacheString.set("a");
        //set without track state
        assertEquals(testCacheString.get(), "a");

        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, testCacheString.getModState());

    }

    @Test
    public void testSetTrackState()
    {
        assertNull(mFieldCacheString.get());
        mFieldCacheString.set("a");
        //set with track state
        assertEquals(mFieldCacheString.get(), "a");
        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, mFieldCacheString.getModState());
    }

    @Test
    public void testSetTrackStateTouched()
    {

        assertNull(mFieldCacheString.get());
        mFieldCacheString.set("a");
        mFieldCacheString.set("a");
        //set with track state
        assertEquals(mFieldCacheString.get(), "a");
        assertEquals(MamaFieldCacheField.MOD_STATE_TOUCHED, mFieldCacheString.getModState());

    }

    @Test
    public void testIsEqual()
    {   
        MamaFieldCacheString testCacheString = new MamaFieldCacheString(102, "example2", true);
        testCacheString.set("a");
        mFieldCacheString.set("a");
        assertEquals(testCacheString.get(), "a");
        assertEquals(mFieldCacheString.get(), "a");
        assertTrue(testCacheString.isEqual(mFieldCacheString.get()));
    }

    @Test
    public void testGet()
    {
        assertNull(mFieldCacheString.get());
        mFieldCacheString.set("a");
        assertEquals(mFieldCacheString.get(), "a");
    }

    @Test
    public void testGetAsString()
    {
        assertNull(mFieldCacheString.get());
        assertEquals("null", mFieldCacheString.getAsString());
        mFieldCacheString.set("a");
        assertEquals(mFieldCacheString.get(), "a");
        assertEquals("a", mFieldCacheString.getAsString());
    }
}
