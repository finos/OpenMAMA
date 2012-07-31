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

/**
 *
 * This is the main class for the test suite.
 */
public class Main
{
    private static MamaBridge m_bridge;

    public static MamaBridge GetBridge()
    {
        return m_bridge;
    }

    public static Test suite()
    {
        // Create a new test suite
        TestSuite suite = new TestSuite();

        // Add all tests
        suite.addTestSuite(MamaDateTimeSetTimeZone.class); 
        suite.addTestSuite(MamaInboxCallbacks.class);     
        suite.addTestSuite(MamaMsgAddArrayMsgWithLength.class); 
        suite.addTestSuite(MamaMsgGetByteBuffer.class);     
        suite.addTestSuite(MamaMsgGetStringAsCharBuffer.class); 
        suite.addTestSuite(MamaMsgTryMethods.class); 
        suite.addTestSuite(MamaMsgVectorFields.class); 
        suite.addTestSuite(MamaOpenClose.class); 
        suite.addTestSuite(MamaPriceGetRoundedPrice.class);
        suite.addTestSuite(MamaSetLogCallback.class);
        suite.addTestSuite(MamaTimerCallbacks.class);

        return suite;
    }

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args)
    {
        // Open mama to satisfy all link references
        m_bridge = Mama.loadBridge("wmw");

        /*Always the first API method called. Initialized internal API
         * state*/
        Mama.open ();

        // Run the test suite
        junit.textui.TestRunner.run(suite());

        // Close mama
        Mama.close();
    }

}
