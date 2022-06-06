package com.wombat.mama.junittests.fieldcache;

import com.wombat.mama.junittests.MamaTestBaseTestCase;
import com.wombat.mama.*;
import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import static org.junit.Assert.*;

/**
 *
 * This class will test MamaFieldCacheField's functions - Implemented through Int32
 */
public class MamaFieldCacheFieldTest extends MamaTestBaseTestCase
{
    /* ****************************************************** */
    /* Protected Member Variables. */
    /* ****************************************************** */
    
    protected MamaMsg mMsg;
    protected MamaFieldCacheInt32 mFieldCacheInt32;

    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Before
    public void setUp()
    {
        super.setUp();
        mFieldCacheInt32 = new MamaFieldCacheInt32(101, "example", true);
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
    public void testSetDescriptor()
    {
        MamaFieldDescriptor desc = new MamaFieldDescriptor(101, (short) 38, "example", null);
        mFieldCacheInt32.setDescriptor(desc);

        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, mFieldCacheInt32.getModState());
        assertEquals(mFieldCacheInt32.getDescriptor(), desc);
    }

    @Test
    public void testGetType()
    {
        assertEquals(mFieldCacheInt32.getType(), MamaFieldDescriptor.I32);
    }

    @Test
    public void testGetTrackModState()
    {
        assertTrue(mFieldCacheInt32.getTrackModState());
    }

    @Test
    public void testSetTrackModState()
    {
        mFieldCacheInt32.setTrackModState(false);
        assertFalse(mFieldCacheInt32.getTrackModState());
    }

    @Test
    public void testClearModState()
    {
        mFieldCacheInt32.clearModState();
        assertEquals(MamaFieldCacheField.MOD_STATE_NOT_MODIFIED, mFieldCacheInt32.getModState());
    }

    @Test
    public void testIsUnmodified()
    {
        assertFalse(mFieldCacheInt32.isUnmodified());
        mFieldCacheInt32.clearModState();
        assertTrue(mFieldCacheInt32.isUnmodified());
    }

    @Test
    public void testIsModified()
    {
        assertTrue(mFieldCacheInt32.isModified());
        mFieldCacheInt32.setModState(MamaFieldCacheField.MOD_STATE_NOT_MODIFIED);
        assertFalse(mFieldCacheInt32.isModified());
    }

    @Test
    public void testTouch()
    {
        assertFalse(mFieldCacheInt32.isTouched());
        mFieldCacheInt32.clearModState();
        mFieldCacheInt32.touch();
        assertTrue(mFieldCacheInt32.isTouched());
    }
}
