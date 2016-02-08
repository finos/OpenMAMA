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

import junit.framework.Test;
import junit.framework.TestSuite;
import com.wombat.mama.*;

/**
 *
 * This is the main class for the test suite.
 */
public class Main
{
    private static String bridgeName = "";
    private static String transportName = "";
    private static String symbol = "";
    private static String source = "";
    private static String badsource = "BADSRC";

    public static String GetBridgeName() { return bridgeName; }
    public static String GetTransportName() { return transportName; }
    public static String GetSymbol() { return symbol; }
    public static String GetSource() { return source; }
    public static String GetBadSource() { return badsource; }

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
        suite.addTestSuite(MamaPublisherTest.class);

        return suite;
    }

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args)
    {
        // Get cmd line args
        boolean ok = true;
        for (int i = 0; i < args.length; )
        {
            if (args[i].equalsIgnoreCase("-m"))
            {
                bridgeName = ++i < args.length ? args[i++] : "";
            }
            else if (args[i].equalsIgnoreCase("-tport"))
            {
                transportName = ++i < args.length ? args[i++] : "";
            }
            else if (args[i].equals("-s"))
            {
                symbol = ++i < args.length ? args[i++] : "";
            }
            else if (args[i].equals("-S"))
            {
                source = ++i < args.length ? args[i++] : "";
            }
            else if (args[i].equals("-badsource"))
            {
                badsource = ++i < args.length ? args[i++] : "";
            }
            else
            {
                ok = false;            // error 
                break;
            }
        }
        if (!ok || bridgeName.isEmpty() || transportName.isEmpty())
        {
            System.err.println("Usage: Main -m bridgeName -tport transportName");
            System.exit(1);
        }

        // Each test will load/unload Mama as needed
        java.util.logging.Level level = Mama.getLogLevel();            // force load of the shared libs

        // Run the test suite
        junit.textui.TestRunner.run(suite());

        System.exit(0);                                                // TODO temporary - not exiting JVM - daemon thread?
    }
}

