package com.wombat.mama.junittests.fieldcache;

import com.wombat.mama.junittests.MamaTestBaseTestCase;
import com.wombat.mama.*;
import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import static org.junit.Assert.*;

/**
 *
 * This class will test MamaFieldCache's functions
 */
public class MamaFieldCacheTest extends MamaTestBaseTestCase
{
    /* ****************************************************** */
    /* Protected Member Variables. */
    /* ****************************************************** */
    
    protected MamaFieldCache mFieldCache;
    protected MamaFieldCacheInt32 mFieldCacheInt32;
    protected MamaFieldCacheInt32 mFieldCacheInt32_2;
    protected MamaFieldDescriptor mFieldDescriptor;

    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Before
    public void setUp()
    {
        super.setUp();
        mFieldCache = new MamaFieldCache();
        mFieldCacheInt32 = new MamaFieldCacheInt32(101, "example", true);
        mFieldCacheInt32_2 = new MamaFieldCacheInt32(102, "example2", true);
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
    public void testAdd()
    {
        mFieldCache.add(mFieldCacheInt32);
        assertEquals(mFieldCache.find(101), mFieldCacheInt32);
    }

    @Test
    public void testFindOrAddDescriptor()
    {
        String example3 = "example3";
        mFieldDescriptor = new MamaFieldDescriptor( 101, (short) 18, "example3", example3);
        assertNull(mFieldCache.find(mFieldDescriptor));
        mFieldCache.findOrAdd(mFieldDescriptor);
        assertEquals(mFieldCache.find(101).getDescriptor().toString(), example3);
    }

    @Test
    public void testFindOrAdd()
    {
        String example3 = "example3";
        assertNull(mFieldCache.find(101));
        mFieldCache.findOrAdd(101, (short) 18, "example3");
        assertEquals(mFieldCache.find(101).getDescriptor().toString(), "I32");
    }

    @Test
    public void testSetTrackModState()
    {
        assertFalse(mFieldCache.getTrackModState());
        mFieldCache.setTrackModState(true);
        assertTrue(mFieldCache.getTrackModState());
    }

    @Test
    public void testSetOverrideDescriptorTrackModState()
    {
        assertTrue(mFieldCache.getOverrideDescriptorTrackModState());
        mFieldCache.setOverrideDescriptorTrackModState(false);
        assertFalse(mFieldCache.getOverrideDescriptorTrackModState());
        mFieldCache.setOverrideDescriptorTrackModState(true);
        assertTrue(mFieldCache.getOverrideDescriptorTrackModState());
    }

}
