/* $Id$
 *
 * OpenMAMA: The open middleware agnostic messaging API
 * Copyright (C) 2011 NYSE Technologies, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

import junit.framework.*;
import com.wombat.mama.*;
import java.util.Iterator;
import java.io.OutputStream;
import java.io.BufferedOutputStream;
/**
 *
 * This class will test MamaFieldCacheProperties's functions
 */
public class MamaFieldCachePropertiesTest extends TestCase
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

    @Override
    protected void setUp()
    {
        mFieldCacheProperties = new MamaFieldCacheProperties();
        mFieldCacheInt32 = new MamaFieldCacheInt32(101, "example", true);
        mFieldCacheInt32_2 = new MamaFieldCacheInt32(102, "example2", true);
    }

    @Override
    protected void tearDown()
    {
    }

    /* ****************************************************** */
    /* Test Functions. */
    /* ****************************************************** */
   
    public void testClearAndDelete()
    {
        mFieldCacheProperties.add(mFieldCacheInt32);
        assertEquals(mFieldCacheProperties.find(101), mFieldCacheInt32);
        mFieldCacheProperties.clear();
        assertEquals(mFieldCacheProperties.find(101), null);
    }

     public void testAddIfModifiedNotModified()
     {
         mFieldCacheInt32.setModState(MamaFieldCacheField.MOD_STATE_NOT_MODIFIED);
         mFieldCacheProperties.addIfModified(mFieldCacheInt32);
         assertEquals(mFieldCacheProperties.find("example"), null);
     }

     public void testAddIfModifiedModified()
     {
         mFieldCacheProperties.addIfModified(mFieldCacheInt32);
         assertEquals(mFieldCacheProperties.find("example"), mFieldCacheInt32);
     }

    public void testClearAndUnregisterAll()
    {
        mFieldCacheProperties.add(mFieldCacheInt32);
        assertEquals(mFieldCacheProperties.find(101), mFieldCacheInt32);
        mFieldCacheProperties.clear();
        assertEquals(mFieldCacheProperties.find(101), null);
    }

    public void testRegisterProperty()
    {
        MamaFieldCacheFieldList testFieldList = new MamaFieldCacheFieldList();

        testFieldList.add(mFieldCacheInt32);
        testFieldList.add(mFieldCacheInt32);

        mFieldCacheProperties.registerProperty(101, mFieldCacheInt32);
        mFieldCacheProperties.apply(testFieldList);
        
        assertEquals(mFieldCacheProperties.find(101), mFieldCacheInt32);
    }

    public void testRegisterPropertyFieldNull()
    {
        MamaFieldCacheFieldList testFieldList = new MamaFieldCacheFieldList();
        mFieldCacheInt32 = null;
        testFieldList.add(mFieldCacheInt32);
        testFieldList.add(mFieldCacheInt32);

        mFieldCacheProperties.registerProperty(101, mFieldCacheInt32);
        mFieldCacheProperties.apply(testFieldList);
        
        assertEquals(mFieldCacheProperties.find(101), null);
    }

    public void testApply()
    {
        MamaFieldCacheFieldList testFieldList = new MamaFieldCacheFieldList();
        testFieldList.add(mFieldCacheInt32);
        
        mFieldCacheProperties.registerProperty(101, mFieldCacheInt32);
        assertEquals(mFieldCacheProperties.find(101), null);

        mFieldCacheProperties.apply(testFieldList);
        assertEquals(mFieldCacheProperties.find(101), mFieldCacheInt32);
    }

    public void testFindNullName()
    {
        MamaFieldCacheFieldList testFieldList = new MamaFieldCacheFieldList();
        testFieldList.add(mFieldCacheInt32);
        
        mFieldCacheProperties.registerProperty(101, mFieldCacheInt32);
        assertEquals(mFieldCacheProperties.find(101), null);

        mFieldCacheProperties.apply(testFieldList);
        assertEquals(mFieldCacheProperties.find(101), mFieldCacheInt32);
        assertEquals(mFieldCacheProperties.find(""), null);
    }

    public void testFindByDesc()
    {
        MamaFieldCacheFieldList testFieldList = new MamaFieldCacheFieldList();
        testFieldList.add(mFieldCacheInt32);
        
        mFieldCacheProperties.registerProperty(101, mFieldCacheInt32);
        assertEquals(mFieldCacheProperties.find(101), null);

        mFieldCacheProperties.apply(testFieldList);
        assertEquals(mFieldCacheProperties.find("example"), mFieldCacheInt32);
    }

    public void testSize()
    {
        MamaFieldCacheFieldList testFieldList = new MamaFieldCacheFieldList();
        assertEquals(mFieldCacheProperties.size(), 0);
        testFieldList.add(mFieldCacheInt32);
        
        mFieldCacheProperties.registerProperty(101, mFieldCacheInt32);

        mFieldCacheProperties.apply(testFieldList);
        assertEquals(mFieldCacheProperties.size(), 1);
    }

    public void testEmpty()
    {
        MamaFieldCacheFieldList testFieldList = new MamaFieldCacheFieldList();
        assertEquals(mFieldCacheProperties.empty(), true);
        testFieldList.add(mFieldCacheInt32);
        
        mFieldCacheProperties.registerProperty(101, mFieldCacheInt32);

        mFieldCacheProperties.apply(testFieldList);
        assertEquals(mFieldCacheProperties.empty(), false);
    }
    public void testGetIterator()
    {
        MamaFieldCacheFieldList testFieldList = new MamaFieldCacheFieldList();

        assertEquals(mFieldCacheProperties.empty(), true);
        i = mFieldCacheProperties.getIterator();
        assertFalse(i.hasNext());
        testFieldList.add(mFieldCacheInt32);
        mFieldCacheProperties.registerProperty(101, mFieldCacheInt32);
        mFieldCacheProperties.apply(testFieldList);

        assertEquals(mFieldCacheProperties.empty(), false);
        i = mFieldCacheProperties.getIterator();
        assertTrue(i.hasNext());
    }
}
