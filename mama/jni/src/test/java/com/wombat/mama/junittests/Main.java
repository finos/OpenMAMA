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
import org.junit.runner.JUnitCore;
import org.junit.runner.Result;
import org.junit.runner.notification.Failure;

import java.util.logging.Level;

/**
 *
 * This is the main class for the test suite.
 */
public class Main
{
    private static String bridgeName = "qpid";

    public static String GetBridgeName() { return bridgeName; }

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
            else
            {
                ok = false;            // error
                break;
            }
        }
        if (!ok || bridgeName.isEmpty())
        {
            System.err.println("Usage: Main -m bridgeName");
            System.exit(1);
        }

        // Each test will load/unload Mama as needed
        Level level = Mama.getLogLevel();            // force load of the shared libs

        // Run the test suite
        Result result = JUnitCore.runClasses(MamaDateTimeSetTimeZone.class, MamaMsgAddArrayMsgWithLength.class,
                MamaMsgGetByteBuffer.class,
                MamaMsgTryMethods.class,
                MamaMsgVectorFields.class,
                MamaOpenClose.class,
                MamaPriceGetRoundedPrice.class,
                MamaSetLogCallback.class,
                MamaTimerCallbacks.class);
        for (Failure failure : result.getFailures()) {
            System.out.println(failure.toString());
        }
        System.out.println("Result=="+result.wasSuccessful());

        System.exit(0);                                                // TODO temporary - not exiting JVM - daemon thread?
    }
}
