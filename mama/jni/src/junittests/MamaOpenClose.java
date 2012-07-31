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
 * This class will test Mama.open() and Mama.close().
 */
public class MamaOpenClose extends TestCase
{
    /* ****************************************************** */
    /* Protected Member Variables. */
    /* ****************************************************** */
    
    /* ****************************************************** */
    /* Protected Functions. */
    /* ****************************************************** */

    @Override
    protected void setUp()
    {        
    }

    @Override
    protected void tearDown()
    {        
    }

    /* ****************************************************** */
    /* Test Functions. */
    /* ****************************************************** */

    public void testOpenClose()
    {
        // Load the bridge
        Mama.loadBridge("wmw");
        
        // Open mama
        Mama.open();

        // Close mama
        Mama.close();
    }

    public void testOpenCloseWithProperties()
    {
        // Load the bridge
        Mama.loadBridge("wmw");

        // Open mama
        Mama.open("c:\\devtools\\windows\\alias", "mama.properties");

        // Close mama
        Mama.close();
    }

    public void testNestedOpenClose()
    {
        // Load the bridge
        Mama.loadBridge("wmw");

        // Open mama
        Mama.open();

        // Open mama
        Mama.open();

        // Close mama
        Mama.close();

        // Close mama
        Mama.close();
    }

    public void testOpenCloseReopenSameBridge()
    {
        // Load the bridge
        Mama.loadBridge("wmw");

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
