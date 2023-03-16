package com.wombat.mama.junittests.fieldcache;

import com.wombat.mama.junittests.MamaTestBaseTestCase;
import com.wombat.mama.*;
import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import java.util.Iterator;

import static org.junit.Assert.*;

/**
 *
 * This class will test MamaFieldCacheFieldList's functions
 */
public class MamaFieldCacheFieldListTest extends MamaTestBaseTestCase
{
    /* ****************************************************** */
    /* Protected Member Variables. */
    /* ****************************************************** */
    
    protected MamaFieldCacheFieldList mFieldCacheFieldList;

    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Before
    public void setUp()
    {
        super.setUp();
        mFieldCacheFieldList = new MamaFieldCacheFieldList();
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
    public void testClear()
    {
        MamaFieldCacheInt8 testField = new MamaFieldCacheInt8(101, "example", true);
        mFieldCacheFieldList.add(testField);
        assertEquals(mFieldCacheFieldList.find("example"), testField);
        mFieldCacheFieldList.clear();
        assertNull(mFieldCacheFieldList.find("example"));
    }

    @Test
    public void testClearAndDelete()
    {
        MamaFieldCacheInt8 testField = new MamaFieldCacheInt8(101, "example", true);
        mFieldCacheFieldList.add(testField);
        assertEquals(mFieldCacheFieldList.find("example"), testField);
        mFieldCacheFieldList.clearAndDelete();
        assertNull(mFieldCacheFieldList.find("example"));
    }

    @Test
    public void testAddIfModifiedNotModified()
    {
        MamaFieldCacheInt8 testField = new MamaFieldCacheInt8(101, "example", true);
        testField.setModState(MamaFieldCacheField.MOD_STATE_NOT_MODIFIED);
        mFieldCacheFieldList.addIfModified(testField);
        assertNull(mFieldCacheFieldList.find("example"));
    }

    @Test
    public void testAddIfModifiedModified()
    {
        MamaFieldCacheInt8 testField = new MamaFieldCacheInt8(101, "example", true);
        mFieldCacheFieldList.addIfModified(testField);
        assertEquals(mFieldCacheFieldList.find("example"), testField); 
    }

    @Test
    public void testFind()
    {
        MamaFieldCacheInt8 testField = new MamaFieldCacheInt8(101, "example", true);
        MamaFieldCacheInt8 testField2 = new MamaFieldCacheInt8(102, "example2", true);
        mFieldCacheFieldList.add(testField);
        mFieldCacheFieldList.add(testField2);
        assertEquals(mFieldCacheFieldList.find("example"), testField); 
        assertEquals(mFieldCacheFieldList.find("example2"), testField2); 
    }

    @Test
    public void findByFid()
    {
        MamaFieldCacheInt8 testField = new MamaFieldCacheInt8(101, "example", true);
        MamaFieldCacheInt8 testField2 = new MamaFieldCacheInt8(102, "example2", true);
        mFieldCacheFieldList.add(testField);
        mFieldCacheFieldList.add(testField2);
        assertEquals(mFieldCacheFieldList.find(101), testField); 
        assertEquals(mFieldCacheFieldList.find(102), testField2); 
    }

    @Test
    public void findByDesc()
    {
        MamaFieldDescriptor desc = new MamaFieldDescriptor(101, (short) 38, "example", null);
        MamaFieldDescriptor desc2 = new MamaFieldDescriptor(102, (short) 38, "example2", null);
        MamaFieldCacheInt8 testField = new MamaFieldCacheInt8(101, "example", true);
        MamaFieldCacheInt8 testField2 = new MamaFieldCacheInt8(102, "example2", true);
        mFieldCacheFieldList.add(testField);
        mFieldCacheFieldList.add(testField2);
        assertEquals(mFieldCacheFieldList.find(desc), testField); 
        assertEquals(mFieldCacheFieldList.find(desc2), testField2); 
    }

    @Test
    public void testSize()
    {
        MamaFieldCacheInt8 testField = new MamaFieldCacheInt8(101, "example", true);
        MamaFieldCacheInt8 testField2 = new MamaFieldCacheInt8(102, "example2", true);
        assertEquals(mFieldCacheFieldList.size(), 0);
        mFieldCacheFieldList.add(testField);
        assertEquals(mFieldCacheFieldList.size(), 1);
        mFieldCacheFieldList.add(testField2);
        assertEquals(mFieldCacheFieldList.size(), 2);
    }

    @Test
    public void testEmpty()
    {
        MamaFieldCacheInt8 testField = new MamaFieldCacheInt8(101, "example", true);
        MamaFieldCacheInt8 testField2 = new MamaFieldCacheInt8(102, "example2", true);
        assertTrue(mFieldCacheFieldList.empty());
        mFieldCacheFieldList.add(testField);
        assertFalse(mFieldCacheFieldList.empty());
        mFieldCacheFieldList.add(testField2);
        assertFalse(mFieldCacheFieldList.empty());
    }

    @Test
    public void testGetIterator()
    {
        MamaFieldCacheInt8 testField = new MamaFieldCacheInt8(101, "example", true);
        mFieldCacheFieldList.add(testField);
        Iterator i = mFieldCacheFieldList.getIterator();
        assertTrue(i.hasNext());
    }
}
