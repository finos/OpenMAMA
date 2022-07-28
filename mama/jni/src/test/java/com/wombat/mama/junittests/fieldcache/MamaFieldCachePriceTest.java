package com.wombat.mama.junittests.fieldcache;

import com.wombat.mama.junittests.MamaTestBaseTestCase;
import com.wombat.mama.*;
import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import static org.junit.Assert.*;

/**
 *
 * This class will test MamaFieldCachePrice's functions
 */
public class MamaFieldCachePriceTest extends MamaTestBaseTestCase
{
    /* ****************************************************** */
    /* Protected Member Variables. */
    /* ****************************************************** */
    
    protected MamaMsg mMsg;
    protected MamaFieldCachePrice mFieldCachePrice;
    protected MamaPrice           mprice;
    protected Double mDelta = 0.000000001;
    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Before
    public void setUp()
    {
        super.setUp();

        mFieldCachePrice = new MamaFieldCachePrice(101, "example", true);
        mprice = new MamaPrice(10.01);
        mMsg = new MamaMsg();   
    }

    @After
    public void tearDown()
    {
        mMsg.destroy();
        Mama.close();
    }

    /* ****************************************************** */
    /* Test Functions. */
    /* ****************************************************** */

    @Test
    public void testAddToMessage()
    {
        //set the value of the field to true (the default, otherwise, is false
        mFieldCachePrice.set(mprice);
        //add it to the message
        mFieldCachePrice.addToMessage(false, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getPrice("example", 101).getValue(), 10.01, mDelta);
    }

    @Test
    public void testAddToMessagefieldName()
    {
        //set the value of the field to true (the default, otherwise, is false
        mFieldCachePrice.set(mprice);
        //add it to the message
        mFieldCachePrice.addToMessage(true, mMsg);
        //check we can get the value of true returned to us when searching for the field
        assertEquals(mMsg.getPrice("example", 101).getValue(), 10.01, mDelta);
    }

    @Test
    public void testCopy()
    {
        mFieldCachePrice.set(mprice);
        MamaFieldCacheField copyCache =  mFieldCachePrice.copy();
        assertEquals(mFieldCachePrice.getAsString(), copyCache.getAsString());
    }

    @Test
    public void testApplyMsgField()
    {
        MamaFieldCachePrice testCachePrice = new MamaFieldCachePrice(102, "example2", true);
        testCachePrice.set(new MamaPrice(20.02));
        mFieldCachePrice.set(mprice);
        assertEquals(testCachePrice.get().getValue(), 20.02, mDelta);
        assertEquals(mFieldCachePrice.get().getValue(), 10.01, mDelta);
        mFieldCachePrice.apply(testCachePrice);
        assertEquals(mFieldCachePrice.get().getValue(), 20.02, mDelta);
    }

    @Test
    public void testApplyFieldCache()
    {
       MamaFieldCachePrice testField  = new MamaFieldCachePrice(102, "example2", true);
       testField.set(new MamaPrice(20.02));
       mFieldCachePrice.apply(testField);
       assertEquals(mFieldCachePrice.get().getValue(), 20.02, mDelta);
    }


    @Test
    public void testSet()
    {
        MamaFieldCachePrice testCachePrice = new MamaFieldCachePrice(102, "example2", false);

        assertNull(testCachePrice.get());
        testCachePrice.set(mprice);
        //set without track state
        assertEquals(testCachePrice.get().getValue(), 10.01, mDelta);

        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, testCachePrice.getModState());

    }

    @Test
    public void testSetTrackState()
    {
        assertNull(mFieldCachePrice.get());
        mFieldCachePrice.set(mprice);
        //set with track state
        assertEquals(mFieldCachePrice.get().getValue(), 10.01, mDelta);
        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, mFieldCachePrice.getModState());
    }

    @Test
    public void testIsEqual()
    {   
        MamaFieldCachePrice testCachePrice = new MamaFieldCachePrice(102, "example2", true);
        testCachePrice.set(mprice);
        mFieldCachePrice.set(mprice);
        assertEquals(testCachePrice.get().getValue(), 10.01, mDelta);
        assertEquals(mFieldCachePrice.get().getValue(), 10.01, mDelta);
        assertTrue(testCachePrice.isEqual(mFieldCachePrice.get()));
    }

    @Test
    public void testGet()
    {
        assertNull(mFieldCachePrice.get());
        mFieldCachePrice.set(mprice);
        assertEquals(mFieldCachePrice.get().getValue(), 10.01, mDelta);
    }

    @Test
    public void testGetAsString()
    {
        assertNull(mFieldCachePrice.get());
        assertEquals("null", mFieldCachePrice.getAsString());
        mFieldCachePrice.set(mprice);
        assertEquals(mFieldCachePrice.get().getValue(), 10.01, mDelta);
        assertEquals("10.01", mFieldCachePrice.getAsString());
    }

}
