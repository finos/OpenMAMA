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

import junit.framework.Test;
import junit.framework.TestSuite;
import com.wombat.mama.*;

public class Main
{
    private static MamaBridge mbridge;

    public static MamaBridge GetBridge()
    {
        return mbridge;
    }

    public static Test suite()
    {
        // Create a new test suite
        TestSuite suite = new TestSuite();

        // Add all tests
        suite.addTestSuite(MamaFieldCacheBoolTest.class);
        suite.addTestSuite(MamaFieldCacheCharTest.class);
        suite.addTestSuite(MamaFieldCacheDateTimeTest.class);
        suite.addTestSuite(MamaFieldCacheEnumTest.class);
        suite.addTestSuite(MamaFieldCacheFloat32Test.class);
        suite.addTestSuite(MamaFieldCacheFloat64Test.class);
        suite.addTestSuite(MamaFieldCacheStringTest.class);
        suite.addTestSuite(MamaFieldCacheUint16Test.class);
        suite.addTestSuite(MamaFieldCacheUint32Test.class);
        suite.addTestSuite(MamaFieldCacheUint64Test.class);
        suite.addTestSuite(MamaFieldCacheUint8Test.class);
        suite.addTestSuite(MamaFieldCacheInt16Test.class);
        suite.addTestSuite(MamaFieldCacheInt32Test.class);
        suite.addTestSuite(MamaFieldCacheInt64Test.class);
        suite.addTestSuite(MamaFieldCacheInt8Test.class);
        suite.addTestSuite(MamaFieldCachePriceTest.class);
        suite.addTestSuite(MamaFieldCacheFieldTest.class);
        suite.addTestSuite(MamaFieldCacheFieldListTest.class);
        suite.addTestSuite(MamaFieldCachePropertiesTest.class);
        suite.addTestSuite(MamaFieldCacheTest.class);

        return suite;
    }

    public static void main(String[] args)
    {
        // Open mama to satisfy all link references
        mbridge = Mama.loadBridge("wmw");

        /*Always the first API method called. Initialized internal API
         * state*/
        Mama.open ();

        // Run the test suite
        junit.textui.TestRunner.run(suite());

        // Close mama
        Mama.close();
    }

}
