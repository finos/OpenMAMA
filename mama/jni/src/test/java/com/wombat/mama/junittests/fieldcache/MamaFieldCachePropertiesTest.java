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
 * This class will test MamaFieldCacheProperties's functions
 */
public class MamaFieldCachePropertiesTest extends MamaTestBaseTestCase
{
    /* ****************************************************** */
    /* Protected Member Variables. */
    /* ****************************************************** */
    
    protected MamaFieldCacheProperties mFieldCacheProperties;
    protected MamaFieldCacheInt32 mFieldCacheInt32;
    protected MamaFieldCacheInt32 mFieldCacheInt32_2;
    protected Iterator i;

    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Before
    public void setUp()
    {
        super.setUp();
        mFieldCacheProperties = new MamaFieldCacheProperties();
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
    public void testClearAndDelete()
    {
        mFieldCacheProperties.add(mFieldCacheInt32);
        assertEquals(mFieldCacheProperties.find(101), mFieldCacheInt32);
        mFieldCacheProperties.clear();
        assertNull(mFieldCacheProperties.find(101));
    }

    @Test
    public void testAddIfModifiedNotModified()
    {
         mFieldCacheInt32.setModState(MamaFieldCacheField.MOD_STATE_NOT_MODIFIED);
         mFieldCacheProperties.addIfModified(mFieldCacheInt32);
         assertNull(mFieldCacheProperties.find("example"));
    }

    @Test
    public void testAddIfModifiedModified()
    {
         mFieldCacheProperties.addIfModified(mFieldCacheInt32);
         assertEquals(mFieldCacheProperties.find("example"), mFieldCacheInt32);
    }

    @Test
    public void testClearAndUnregisterAll()
    {
        mFieldCacheProperties.add(mFieldCacheInt32);
        assertEquals(mFieldCacheProperties.find(101), mFieldCacheInt32);
        mFieldCacheProperties.clear();
        assertNull(mFieldCacheProperties.find(101));
    }

    @Test
    public void testRegisterProperty()
    {
        MamaFieldCacheFieldList testFieldList = new MamaFieldCacheFieldList();

        testFieldList.add(mFieldCacheInt32);
        testFieldList.add(mFieldCacheInt32);

        mFieldCacheProperties.registerProperty(101, mFieldCacheInt32);
        mFieldCacheProperties.apply(testFieldList);
        
        assertEquals(mFieldCacheProperties.find(101), mFieldCacheInt32);
    }

    @Test
    public void testRegisterPropertyFieldNull()
    {
        MamaFieldCacheFieldList testFieldList = new MamaFieldCacheFieldList();
        mFieldCacheInt32 = null;
        testFieldList.add(mFieldCacheInt32);
        testFieldList.add(mFieldCacheInt32);

        mFieldCacheProperties.registerProperty(101, mFieldCacheInt32);
        mFieldCacheProperties.apply(testFieldList);

        assertNull(mFieldCacheProperties.find(101));
    }

    @Test
    public void testApply()
    {
        MamaFieldCacheFieldList testFieldList = new MamaFieldCacheFieldList();
        testFieldList.add(mFieldCacheInt32);
        
        mFieldCacheProperties.registerProperty(101, mFieldCacheInt32);
        assertNull(mFieldCacheProperties.find(101));

        mFieldCacheProperties.apply(testFieldList);
        assertEquals(mFieldCacheProperties.find(101), mFieldCacheInt32);
    }

    @Test
    public void testFindNullName()
    {
        MamaFieldCacheFieldList testFieldList = new MamaFieldCacheFieldList();
        testFieldList.add(mFieldCacheInt32);
        
        mFieldCacheProperties.registerProperty(101, mFieldCacheInt32);
        assertNull(mFieldCacheProperties.find(101));

        mFieldCacheProperties.apply(testFieldList);
        assertEquals(mFieldCacheProperties.find(101), mFieldCacheInt32);
        assertNull(mFieldCacheProperties.find(""));
    }

    @Test
    public void testFindByDesc()
    {
        MamaFieldCacheFieldList testFieldList = new MamaFieldCacheFieldList();
        testFieldList.add(mFieldCacheInt32);
        
        mFieldCacheProperties.registerProperty(101, mFieldCacheInt32);
        assertNull(mFieldCacheProperties.find(101));

        mFieldCacheProperties.apply(testFieldList);
        assertEquals(mFieldCacheProperties.find("example"), mFieldCacheInt32);
    }

    @Test
    public void testSize()
    {
        MamaFieldCacheFieldList testFieldList = new MamaFieldCacheFieldList();
        assertEquals(mFieldCacheProperties.size(), 0);
        testFieldList.add(mFieldCacheInt32);
        
        mFieldCacheProperties.registerProperty(101, mFieldCacheInt32);

        mFieldCacheProperties.apply(testFieldList);
        assertEquals(mFieldCacheProperties.size(), 1);
    }

    @Test
    public void testEmpty()
    {
        MamaFieldCacheFieldList testFieldList = new MamaFieldCacheFieldList();
        assertTrue(mFieldCacheProperties.empty());
        testFieldList.add(mFieldCacheInt32);
        
        mFieldCacheProperties.registerProperty(101, mFieldCacheInt32);

        mFieldCacheProperties.apply(testFieldList);
        assertFalse(mFieldCacheProperties.empty());
    }
    @Test
    public void testGetIterator()
    {
        MamaFieldCacheFieldList testFieldList = new MamaFieldCacheFieldList();

        assertTrue(mFieldCacheProperties.empty());
        i = mFieldCacheProperties.getIterator();
        assertFalse(i.hasNext());
        testFieldList.add(mFieldCacheInt32);
        mFieldCacheProperties.registerProperty(101, mFieldCacheInt32);
        mFieldCacheProperties.apply(testFieldList);

        assertFalse(mFieldCacheProperties.empty());
        i = mFieldCacheProperties.getIterator();
        assertTrue(i.hasNext());
    }
}
