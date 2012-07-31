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

//package regression;

import com.wombat.mama.*;
import com.wombat.common.WombatException;
import java.util.logging.Logger;
import java.util.logging.Level;


/**
 * This sample application demonstrates how to publish mama messages, and
 * respond to requests from a clients inbox.
 *
 * It accepts the following command line arguments:
 *      [-s topic]         The topic on which to send messages. Default value
 *                         is "MAMA_TOPIC".
 *      [-l topic]         The topic on which to listen for inbound requests.
 *                         Default is "MAMA_INBOUND_TOPIC".
 *      [-i interval]      The interval between messages .Default, 0.5.
 *      [-tport name]      The transport parameters to be used from
 *                         mama.properties.
 *      [-q]               Quiet mode. Suppress output.
 *      [-c number]        How many messages to publish (default infinite).
 *
 *
 *
 */
public class BasicPubJava
{
    private static String                myOutboundTopic = "MAMA_TOPIC";
    private static String                myInBoundTopic  = "MAMA_INBOUND_TOPIC";
    private static String                myMiddleware    = "avis";
    private static MamaBridge            myBridge        = null;
    private static MamaQueue             myQueue         = null;
    private static String                myTransportName = null;
    private static MamaMsg               myMsg           = null;

    private static double                myInterval      = 0.5;
    private static int                   myQuietLevel    = 0;

    private static MamaTransport         myTransport     = null;
    private static MamaTimer             myTimer         = null;
    private static MamaBasicSubscription mySubscription  = null;
    private static MamaPublisher         myPublisher     = null;
    private static long                  myCount         = 0;
    private static long                  myMsgNumber     = 0;

    private static int []                myStopEvent     = new int[1];

    private static MamaTransportTopicListener      myTopicListener           = null;
    private static final Logger     logger              =
                                    Logger.getLogger( "com.wombat.mama" );
    private static Level            myLogLevel;
    private static int			mamaMsgNumber	= 0;
    /**
     * Initialize Mama and create a basic transport.
     */
    private static void initializeMama( )
    {
        try
        {
            myBridge = Mama.loadBridge (myMiddleware);
            Mama.open();
            myQueue = Mama.getDefaultQueue (myBridge);
            myTransport = new MamaTransport ();

            /* Add transport and transport topic listeners */
            myTopicListener = new MamaTransportTopicListener ();
            myTransport.addTransportTopicListener( myTopicListener );

            myTransport.create (myTransportName, myBridge);
            myMsg = new MamaMsg();
        }
        catch( WombatException e )
        {
            e.printStackTrace( );
            System.err.println( "Exception occurred initializing mama: " + e );
            System.exit(1);
        }
    }


    private static void createPublisher(  )
    {
        try
        {
            myPublisher = new MamaPublisher();
            myPublisher.create( myTransport, myOutboundTopic );
        }
        catch( WombatException e  )
        {
            e.printStackTrace( );
            System.err.println( "Exception creating publisher: " + e );
            System.exit( 1 );
        }
    }

    private static void publishMessage( MamaMsg request )
    {
        try
        {
            myMsg.clear();

            /* Add some fields. This is not required, but illustrates how to
            * send data.
            */

 	switch(mamaMsgNumber)
        {
         case 0 :
             myMsg.addString ("PublisherTopic", mamaMsgNumber+1, myOutboundTopic);
             break;
         case 1 :
             myMsg.addF32("f32", mamaMsgNumber+1, (float)1.234567);
             break;
         case 2 :
             myMsg.addF64("f64",mamaMsgNumber+1, 123.456789d);
             break;
         case 3 :
             myMsg.addI8("i8",mamaMsgNumber+1, (byte)-126);
             break;
         case 4 :
            myMsg.addI16("i16",mamaMsgNumber+1, (short)32766);
            break;
         case 5 :
            myMsg.addI32("i32", mamaMsgNumber+1, 2147483647);
            break;
         case 6 :
            myMsg.addI64("i64", mamaMsgNumber+1, 4294967297L);
            break;
         case 7 :
            myMsg.addU8("u8", mamaMsgNumber+1, (short) 255);
            break;
         case 8 :
            myMsg.addU16("u16", mamaMsgNumber+1, (int)55000);
            break;
         case 9 :
            myMsg.addU32("u32", mamaMsgNumber+1, 4294967295L);
            break;
         case 10 :
            myMsg.addU64("u64", mamaMsgNumber+1, 5294967297L);
            break;
	case 11 :
            myMsg.addChar("char", mamaMsgNumber+1, 'c');
            break;
         case 12 :
            myMsg.addString("EXIT", mamaMsgNumber+1,"EXIT");
            break;

        }

            if( request != null )
            {
                if( myQuietLevel < 1 )
                {
                    System.out.println( "\nPublishing message to inbox.\n" );
                }
                myPublisher.sendReplyToInbox( request, myMsg );
            }
            else
            {
                if( myQuietLevel < 1 )
                {
                    System.out.print( "\nPublishing message "+ mamaMsgNumber + " to " + myOutboundTopic );
                }
                myPublisher.send( myMsg );
                System.out.print ("\tmsg="+ myMsg.toString());   
            }


        }
        catch( WombatException e )
        {
            e.printStackTrace( );
            System.err.println( "Exception publishing message: " + e );
            System.exit(1);
        }
        mamaMsgNumber++;
    }

    private static void createInboundSubscription( )
    {

        try
        {
           MamaBasicSubscription subsc = new MamaBasicSubscription ();
           subsc.createBasicSubscription(
                new SubscriptionCallback(),
                myTransport,
                myQueue,
                myInBoundTopic,
                null);
        }
        catch( WombatException e )
        {
            e.printStackTrace( );
            System.err.println( "Exception creating subscription: " + e );
            System.exit(1);
        }

    }

    private static void createIntervalTimer( )
    {
        try
        {
            myTimer = new MamaTimer();
            myTimer.create (myQueue, new TimerCallback(), myInterval);
        }
        catch( WombatException e )
        {
            e.printStackTrace( );
            System.err.println( "Error creating timer: " + e );
            System.exit(1);
        }
    }

    private static void parseCommandLine( String [] args )
    {
        for ( int i = 0; i < args.length; )
        {
            String arg = args[ i ];

            if( "-s".equals( arg ) )
            {
                myOutboundTopic = args[i+1];
                i += 2;
            }
            else if( "-l".equals( arg ) )
            {
                myInBoundTopic = args[i+1];
                i += 2;
            }
            else if( "-c".equals( arg ) )
            {
                myCount = Integer.parseInt( args[i+1]);
                i += 2;
            }
            else if( "-i".equals( arg ) )
            {
                myInterval = Double.parseDouble( args[i+1] );
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
                    "   topic:              " + myOutboundTopic + "\n" +
                    "   inbound topic:      " + myInBoundTopic  + "\n" +
                    "   interval            " + myInterval      + "\n" +
                    "   transport:          " + myTransportName + "\n" );
        }
    }

    public static void main( String [] args ) throws InterruptedException
    {
        parseCommandLine( args );

        initializeMama();

        createIntervalTimer();
        createInboundSubscription();
        createPublisher();

        Mama.start (myBridge);/*Blocks for JNI version of MAMA*/

        myTimer.destroy();
        mySubscription.destroy();
        myTransport.destroy();

        Mama.close();
    }

    private static class SubscriptionCallback implements MamaBasicSubscriptionCallback
    {
        public void onCreate(
            MamaBasicSubscription  subscription )
        {
            mySubscription = subscription;
            if( myQuietLevel < 2 )
            {
                System.out.println( "Created inbound subscription.\n" );
            }
        }

        public void onError( MamaBasicSubscription subscription,
                             short status,
                             int platformError,
                             String subject )
        {
            System.out.println( "Error creating subscriptionBridge: " + status );
            System.exit(1);
        }

        public void onMsg(
            MamaBasicSubscription subscription,
            MamaMsg    msg )
        {
            if( !msg.isFromInbox() )
            {
                System.out.println ("Symbol=[" + subscription.getSymbol() + "] : " +
                                    "Inbound msg not from inbox. No reply sent.\n" );
                return;
            }

            publishMessage (msg);
        }

        public void onDestroy (MamaBasicSubscription subscription)
        {
            System.out.println ("Subscription destroyed");
        }
    };



    private static class TimerCallback implements MamaTimerCallback
    {
        public void onTimer (MamaTimer timer)
        {
            if( myCount > 0 && myCount++ == 1000 )
            {
                synchronized( myStopEvent )
                {
                    myStopEvent.notify();
                }

                return;
            }
            if( mamaMsgNumber > 12 )
            {   
                try
                {
                    Thread.sleep(1000);
                }   
                catch (Exception e)
		{
	            e.printStackTrace( ); 
                }
                timer.destroy();
                Mama.stop(myBridge);
                return;
            }

            publishMessage( null );
        }

        public void onDestroy (MamaTimer timer)
        {
            System.out.println ("Timer destroyed");
        }
    };

}








