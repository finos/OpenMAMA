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
 * This class will test MamaFieldCacheField's functions - Implemented through Int32
 */
public class MamaFieldCacheFieldTest extends TestCase
{
    /* ****************************************************** */
    /* Protected Member Variables. */
    /* ****************************************************** */
    
    protected MamaMsg mMsg;
    protected MamaFieldCacheInt32 mFieldCacheInt32;

    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Override
    protected void setUp()
    {
        mFieldCacheInt32 = new MamaFieldCacheInt32(101, "example", true);

        mMsg = new MamaMsg();   
    }

    @Override
    protected void tearDown()
    {
    }

    /* ****************************************************** */
    /* Test Functions. */
    /* ****************************************************** */
    
    public void testSetDescriptor()
    {
        MamaFieldDescriptor desc = new MamaFieldDescriptor(101, (short) 38, "example", null);
        mFieldCacheInt32.setDescriptor(desc);
        assertEquals(MamaFieldCacheField.MOD_STATE_MODIFIED, mFieldCacheInt32.getModState());
        assertEquals(mFieldCacheInt32.getDescriptor(), desc);
    }
    
    public void testGetType()
    {
        assertEquals(mFieldCacheInt32.getType(), MamaFieldDescriptor.I32);
    }

    public void testGetTrackModState()
    {
        assertTrue(mFieldCacheInt32.getTrackModState());
    }

    public void testSetTrackModState()
    {
        mFieldCacheInt32.setTrackModState(false);
        assertFalse(mFieldCacheInt32.getTrackModState());
    }

    public void testClearModState()
    {
        mFieldCacheInt32.clearModState();
        assertEquals(MamaFieldCacheField.MOD_STATE_NOT_MODIFIED, mFieldCacheInt32.getModState());
    }

    public void testIsUnmodified()
    {
        assertFalse(mFieldCacheInt32.isUnmodified());
        mFieldCacheInt32.clearModState();
        assertTrue(mFieldCacheInt32.isUnmodified());
    }
    
    public void testIsModified()
    {
        assertTrue(mFieldCacheInt32.isModified());
        mFieldCacheInt32.setModState(MamaFieldCacheField.MOD_STATE_NOT_MODIFIED);
        assertFalse(mFieldCacheInt32.isModified());
    }

    public void testTouch()
    {
        assertFalse(mFieldCacheInt32.isTouched());
        mFieldCacheInt32.clearModState();
        mFieldCacheInt32.touch();
        assertTrue(mFieldCacheInt32.isTouched());
    }
}
