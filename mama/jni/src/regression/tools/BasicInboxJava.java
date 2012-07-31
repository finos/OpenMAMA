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

//package com.wombat.mama.examples;

import com.wombat.mama.*;
import com.wombat.common.WombatException;
import java.util.logging.Logger;
import java.util.logging.Level;

/**
 * This sample application demonstrates how to send mamaMsgs from an inbox,
 * and receive the reply.
 *
 * It accepts the following command line arguments:
 *      [-s topic]         The topic on which to send the request. Default value
 *                         is "MAMA_INBOUND_TOPIC".
 *      [-tport name]      The transport parameters to be used from
 *                         mama.properties.
 *      [-q]               Quiet mode. Suppress output.
 *
 */
public class BasicInboxJava
{



    private static MamaTransport         myTransport     = null;
    private static MamaInbox             myInbox         = null;
    private static MamaPublisher         myPublisher     = null;
    private static String                myMiddleware    = "avis";
    private static MamaBridge            myBridge        = null;

    private static String                myTopic         = "MAMA_INBOUND_TOPIC";
    private static String                myTransportName = null;
    private static int                   myQuietLevel    = 0;

    private static int[]                 myStopEvent     = new int[1];

    private static final Logger     logger              =
                                    Logger.getLogger( "com.wombat.mama" );
    private static Level            myLogLevel;

    public static void main( String [] args )
    {
        parseCommandLine( args);

        initializeMama();
        createPublisher();
        createInbox();
        sendRequest();
        Mama.start (myBridge);
    }

    private static void initializeMama( )
    {
        try
        {
            myBridge = Mama.loadBridge (myMiddleware);
            Mama.open();
            myTransport = new MamaTransport ();
            myTransport.create (myTransportName, myBridge);
        }
        catch( WombatException e )
        {
            e.printStackTrace( );
            System.err.println(
                "Error initializing mama or creating transport: " + e );
            System.exit(1);
        }
    }

    private static void sendRequest( )
    {
        try
        {
            MamaMsg msg = new MamaMsg ();

            /* WombatMsg does not like empty messages */
            msg.addU32( "field", 1, 32 );
            myPublisher.sendFromInbox( myInbox, msg );
            /* Throttle destroys msg */
        }
        catch( WombatException e )
        {
            e.printStackTrace( );
            System.err.println( "Error sending request: " + e );
            System.exit(1);
        }
    }

    private static class CB implements MamaInboxCallback
    {
        public void onMsg( MamaInbox inbox, MamaMsg msg )
        {
            System.out.println ("Received reply:" + msg);
            synchronized( myStopEvent )
            {
                myStopEvent.notify();
            }
            Mama.stop(myBridge);
        }

        public void onDestroy (MamaInbox inbox)
        {
            System.out.println ("Inbox destroyed");
        }
    };

    private static void createInbox( )
    {
        try
        {
            myInbox = new MamaInbox();
            myInbox.create( myTransport,Mama.getDefaultQueue (myBridge) ,new CB());
        }
        catch (WombatException e )
        {
            System.err.println ("Error creating inbox: " + e);
            System.exit (1);
        }
    }


    private static void createPublisher( )
    {
        try
        {
            myPublisher = new MamaPublisher();
            myPublisher.create( myTransport, myTopic);
        }
        catch (WombatException e )
        {
            e.printStackTrace( );
            System.err.println( "Error creating publisher: " + e );
            System.exit(1);
        }
    }

    static void parseCommandLine( String [] args )
    {
        for ( int i = 0; i < args.length;)
        {
            String arg = args[ i ];
            if( "-s".equals( arg ) )
            {
                myTopic = args[i+1];
                i += 2;
            }
            else if( "-tport".equals( arg ) )
            {
                myTransportName = args[i+1];
                i += 2;
            }
            else if( "-q".equals( arg ) )
            {
                myQuietLevel++;
                i++;
            }
            else if ( "-m".equals( arg ))
            {
                myMiddleware = args[i+1];
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
        }

        if( myQuietLevel < 2 )
        {
            System.out.println(
                "Starting Publisher with:\n" +
                "   topic:              " + myTopic         + "\n" +
                "   transport:          " + myTransportName + "\n" );
        }
    }
}
