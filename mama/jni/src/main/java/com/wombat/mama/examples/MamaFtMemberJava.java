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
package com.wombat.mama.examples;

import com.wombat.mama.*;
import java.util.logging.Logger;
import java.util.logging.Level;


/**
 * MamaFtMember - Simple fault tolerance example.
 *
 * <pre>
 *
 * Demonstrates use of the MAMA FT API.
 * </pre>
 */
public class MamaFtMemberJava
{
    private static FtDemo myFtDemo               = null;
    private static MamaFtMember  myFtMember      = null;
    private static MamaTimer     myTimer         = null;
    private static short         myFtState       = MamaFtState.MAMA_FT_STATE_UNKNOWN; 
    private static MamaTransport myTransport     = null;
    private static int           myWeight; 
    private static String        myGroup         = null;
    private static MamaBridge    myBridge        = null;
    private static MamaQueue     myQueue         = null;
    private static String        myTportName; 
    private static short         myFtType        = MamaFtType.MAMA_FT_TYPE_MULTICAST;
    private static String        myMiddleware    = "wmw";
    private static final Logger  logger          =
                                 Logger.getLogger( "com.wombat.mama" );
    private static Level         myLogLevel;

    public static void main (String[] args)
    {
        parseCommandLine (args);
   
        try
        {
            initializeMama();

            System.out.println( "Type CTRL-C to exit.");

            Mama.start(myBridge);
        }
        catch (Exception e)
        {
            if (e.getCause() != null)
            {
                e.getCause().printStackTrace();
            }

            e.printStackTrace();
            System.exit (1);
        }
    }

    public static void initializeMama ()
    {
        myBridge = Mama.loadBridge (myMiddleware);

        Mama.open();

        myTransport = new MamaTransport ();
        myTransport.create (myTportName, myBridge);

        if (myGroup == null)
        {
            logger.fine ("No FT group name specified.");
            return;
        }

        myFtDemo = new FtDemo();

        myTimer = new MamaTimer();

        myTimer.create (Mama.getDefaultQueue(myBridge), myFtDemo, 2.0);

        myFtMember = new MamaFtMember();

        myFtMember.setupFtMember (myFtType,
                                  Mama.getDefaultQueue(myBridge),
                                  myFtDemo,
                                  myTransport,
                                  myGroup,
                                  myWeight,
                                  1.0,
                                  6.0);

        myFtMember.activate(); 

        Mama.start(myBridge);
    }   

    private static void parseCommandLine (final String[] args)
    {
        for(int i = 0; i < args.length;)
        {
            if (args[i].equals ("-group") || args[i].equals("-g"))
            {
                myGroup = args[i +1];
                i += 2;
            }
            else if (args[i].equals ("-w") || args[i].equals("-weight"))
            {
                myWeight = Integer.parseInt(args[i + 1]);
                i += 2;
            }
            else if (args[i].equals ("-tport") || args[i].equals("-transport"))
            {
                myTportName = args[i + 1];
                i += 2;
            }
            else if (args[i].equals ("-v"))
            {
                myLogLevel = myLogLevel == null
                             ? Level.FINE    : myLogLevel == Level.FINE
                             ? Level.FINER   : Level.FINEST;

                Mama.enableLogging (myLogLevel);
                i++;
            }
            else if (args[i].equals ("-m"))
            {
                myMiddleware = args[i + 1];
                i += 2;
            }
            else if (args[i].equals ("-b"))
            {
                myFtType = MamaFtType.MAMA_FT_TYPE_BRIDGE;
                i += 1;
            }
            else
            {
                i++;
            }
        }
    }

    private static class FtDemo extends MamaFtCallback implements MamaTimerCallback
    {
        public void onFtStateChange (MamaFtMember member, String name, short state)
        {
            myFtState = state;
            printState ("FT Notification");
        }

        public void onTimer (MamaTimer timer)
        {
            printState ("Timer");
        }

        public void onDestroy (MamaTimer timer)
        {
            printState ("Timer destroyed");
        }

        private void printState (String context)
        {
            System.out.println ("( " + context + " ) Current state is: " + MamaFtState.toString (myFtState));
        }    
    }
}
