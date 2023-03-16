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
 * This class will test MamaFieldCacheChar's functions
 */
public class MamaFieldCacheCharTest extends MamaTestBaseTestCase
{
    /* ****************************************************** */
    /* Protected Member Variables. */
    /* ****************************************************** */
    
    protected MamaMsg mMsg;
    protected MamaFieldCacheChar mFieldCacheChar;

    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Before
    public void setUp()
    {
        super.setUp();

        mFieldCacheChar = new MamaFieldCacheChar(101, "example", true);
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
        mFieldCacheChar.set('a');
        //add it to the message
        mFieldCacheChar.addToMessage(false, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getChar("example", 101), 'a');
    }

    @Test
    public void testAddToMessagefieldName()
    {
        //set the value of the field to true (the default, otherwise, is false
        mFieldCacheChar.set('a');
        //add it to the message
        mFieldCacheChar.addToMessage(true, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getChar("example", 101), 'a');
    }

    @Test
    public void testCopy()
    {
        MamaFieldCacheField copyCache =  mFieldCacheChar.copy();
        assertEquals(mFieldCacheChar.getAsString(), copyCache.getAsString());
    }

    @Test
    public void testApplyMsgField()
    {
        MamaFieldCacheChar testCacheChar = new MamaFieldCacheChar(102, "example2", true);
        mFieldCacheChar.set('b');
        testCacheChar.set('a');
        assertEquals(testCacheChar.get(), 'a');
        assertEquals(mFieldCacheChar.get(), 'b');
        mFieldCacheChar.apply(testCacheChar);
        assertEquals(mFieldCacheChar.get(), 'a');
    }

    @Test
    public void testApplyFieldCache()
    {
       MamaFieldCacheChar testField  = new MamaFieldCacheChar(102, "example2", true);
       testField.set('a');
       mFieldCacheChar.apply(testField);
       assertEquals(mFieldCacheChar.get(), 'a');
    }


    @Test
    public void testSet()
    {
        MamaFieldCacheChar testCacheChar = new MamaFieldCacheChar(102, "example2", false);

        assertEquals(testCacheChar.get(), '\0');
        testCacheChar.set('a');
        //set without track state
        assertEquals(testCacheChar.get(), 'a');

        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, testCacheChar.getModState());

    }

    @Test
    public void testSetTrackState()
    {
        assertEquals(mFieldCacheChar.get(), '\0');
        mFieldCacheChar.set('a');
        //set with track state
        assertEquals(mFieldCacheChar.get(), 'a');
        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, mFieldCacheChar.getModState());
    }

    @Test
    public void testSetTrackStateTouched()
    {
    
        assertEquals(mFieldCacheChar.get(), '\0');
        mFieldCacheChar.set('a');
        mFieldCacheChar.set('a');
        //set with track state
        assertEquals(mFieldCacheChar.get(), 'a');
        assertEquals(MamaFieldCacheField.MOD_STATE_TOUCHED, mFieldCacheChar.getModState());

    }

    @Test
    public void testIsEqual()
    {   
        MamaFieldCacheChar testCacheChar = new MamaFieldCacheChar(102, "example2", true);
        testCacheChar.set('a');
        mFieldCacheChar.set('a');
        assertEquals(testCacheChar.get(), 'a');
        assertEquals(mFieldCacheChar.get(), 'a');
        assertTrue(testCacheChar.isEqual(mFieldCacheChar.get()));
    }

    @Test
    public void testGet()
    {
        assertEquals(mFieldCacheChar.get(), '\0');
        mFieldCacheChar.set('a');
        assertEquals(mFieldCacheChar.get(), 'a');
    }

    @Test
    public void testGetAsString()
    {
        assertEquals(mFieldCacheChar.get(), '\0');
        assertEquals("\0", mFieldCacheChar.getAsString());
        mFieldCacheChar.set('a');
        assertEquals(mFieldCacheChar.get(), 'a');
        assertEquals("a", mFieldCacheChar.getAsString());
    }
}
