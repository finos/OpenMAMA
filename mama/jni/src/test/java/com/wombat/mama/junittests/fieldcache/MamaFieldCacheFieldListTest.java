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
/**
 *
 * This class will test MamaFieldCacheFieldList's functions
 */
public class MamaFieldCacheFieldListTest extends TestCase
{
    /* ****************************************************** */
    /* Protected Member Variables. */
    /* ****************************************************** */
    
    protected MamaFieldCacheFieldList mFieldCacheFieldList;

    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Override
    protected void setUp()
    {
        mFieldCacheFieldList = new MamaFieldCacheFieldList();
    }

    @Override
    protected void tearDown()
    {
    }

    /* ****************************************************** */
    /* Test Functions. */
    /* ****************************************************** */
    
    public void testClear()
    {
        MamaFieldCacheInt8 testField = new MamaFieldCacheInt8(101, "example", true);
        mFieldCacheFieldList.add(testField);
        assertEquals(mFieldCacheFieldList.find("example"), testField);
        mFieldCacheFieldList.clear();
        assertEquals(mFieldCacheFieldList.find("example"), null);
    }

    public void testClearAndDelete()
    {
        MamaFieldCacheInt8 testField = new MamaFieldCacheInt8(101, "example", true);
        mFieldCacheFieldList.add(testField);
        assertEquals(mFieldCacheFieldList.find("example"), testField);
        mFieldCacheFieldList.clearAndDelete();
        assertEquals(mFieldCacheFieldList.find("example"), null);
    }

    public void testAddIfModifiedNotModified()
    {
        MamaFieldCacheInt8 testField = new MamaFieldCacheInt8(101, "example", true);
        testField.setModState(MamaFieldCacheField.MOD_STATE_NOT_MODIFIED);
        mFieldCacheFieldList.addIfModified(testField);
        assertEquals(mFieldCacheFieldList.find("example"), null); 
    }

    public void testAddIfModifiedModified()
    {
        MamaFieldCacheInt8 testField = new MamaFieldCacheInt8(101, "example", true);
        mFieldCacheFieldList.addIfModified(testField);
        assertEquals(mFieldCacheFieldList.find("example"), testField); 
    }

    public void testFind()
    {
        MamaFieldCacheInt8 testField = new MamaFieldCacheInt8(101, "example", true);
        MamaFieldCacheInt8 testField2 = new MamaFieldCacheInt8(102, "example2", true);
        mFieldCacheFieldList.add(testField);
        mFieldCacheFieldList.add(testField2);
        assertEquals(mFieldCacheFieldList.find("example"), testField); 
        assertEquals(mFieldCacheFieldList.find("example2"), testField2); 
    }

    public void findByFid()
    {
        MamaFieldCacheInt8 testField = new MamaFieldCacheInt8(101, "example", true);
        MamaFieldCacheInt8 testField2 = new MamaFieldCacheInt8(102, "example2", true);
        mFieldCacheFieldList.add(testField);
        mFieldCacheFieldList.add(testField2);
        assertEquals(mFieldCacheFieldList.find(101), testField); 
        assertEquals(mFieldCacheFieldList.find(102), testField2); 
    }

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

    public void testGetIterator()
    {
        Iterator i;
        MamaFieldCacheInt8 testField = new MamaFieldCacheInt8(101, "example", true);
        mFieldCacheFieldList.add(testField);
        i = mFieldCacheFieldList.getIterator();
        assertTrue(i.hasNext());
    }
}
