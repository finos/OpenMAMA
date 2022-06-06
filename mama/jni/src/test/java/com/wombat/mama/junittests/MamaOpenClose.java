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

package com.wombat.mama.junittests;

import com.wombat.mama.*;
import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import static org.junit.Assert.assertTrue;

/**
 *
 * This class will test Mama.open() and Mama.close().
 */
public class MamaOpenClose extends MamaTestBaseTestCase
{
    /* ****************************************************** */
    /* Protected Member Variables. */
    /* ****************************************************** */

    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Before
    public void setUp()
    {
        super.setUp();
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
    public void testOpenClose()
    {
        // Load the bridge
        Mama.loadBridge(getBridgeName());

        // Open mama
        Mama.open();

        // Close mama
        Mama.close();
    }

    @Test
    public void testOpenCloseWithProperties()
    {
        // Load the bridge
        Mama.loadBridge(getBridgeName());

        // Open mama
        Mama.open("", "mama.properties");

        // Close mama
        Mama.close();
    }

    @Test
    public void testNestedOpenClose()
    {
        // Load the bridge
        Mama.loadBridge(getBridgeName());

        // Open mama
        Mama.open();

        // Open mama
        Mama.open();

        // Close mama
        Mama.close();

        // Close mama
        Mama.close();
    }

    @Test
    public void testOpenCloseReopenSameBridge()
    {
        // Load the bridge
        Mama.loadBridge(getBridgeName());

        // Open mama
        Mama.open();

        // Close mama
        Mama.close();

        // Open again, this will cause an error as the bridge has not been re-loaded
        try
        {
            Mama.open();
        }

        catch(MamaException except)
        {
            return;
        }

        // If we get here there is a problem
        assertTrue(true);
    }
}
