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

/**
 *
 * This class will test MamaFieldCache's functions
 */
public class MamaFieldCacheTest extends TestCase
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

    @Override
    protected void setUp()
    {
        mFieldCache = new MamaFieldCache();
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
  
    public void testAdd()
    {
        mFieldCache.add(mFieldCacheInt32);
        assertEquals(mFieldCache.find(101), mFieldCacheInt32);
    }

    public void testFindOrAddDescriptor()
    {
        String example3 = "example3";
        mFieldDescriptor = new MamaFieldDescriptor( 101, (short) 18, "example3", example3);
        assertEquals(mFieldCache.find(mFieldDescriptor), null);
        mFieldCache.findOrAdd(mFieldDescriptor);
        assertEquals(mFieldCache.find(101).getDescriptor().toString(), example3);
    }

    public void testFindOrAdd()
    {
        String example3 = "example3";
        assertEquals(mFieldCache.find(101), null);
        mFieldCache.findOrAdd(101, (short) 18, "example3");
        assertEquals(mFieldCache.find(101).getDescriptor().toString(), "I32");
    }

    public void testSetTrackModState()
    {
        assertEquals(mFieldCache.getTrackModState(), false);
        mFieldCache.setTrackModState(true);
        assertEquals(mFieldCache.getTrackModState(), true);
    }

    public void testSetOverrideDescriptorTrackModState()
    {
        assertEquals(mFieldCache.getOverrideDescriptorTrackModState(), true);
        mFieldCache.setOverrideDescriptorTrackModState(false);
        assertEquals(mFieldCache.getOverrideDescriptorTrackModState(), false);
    }

}
