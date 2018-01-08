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
 *      [-pubCb]           Listen for publisher callbacks.
 */
public class MamaPublisherJava implements MamaPublisherCallback
{
    private String                myOutboundTopic = "MAMA_TOPIC";
    private String                myInBoundTopic  = "MAMA_INBOUND_TOPIC";
    private String                myMiddleware    = "wmw";
    private MamaBridge            myBridge        = null;
    private MamaQueue             myQueue         = null;
    private String                myTransportName = null;
    private MamaMsg               myMsg           = null;

    private double                myInterval      = 1.0;
    private int                   myQuietLevel    = 0;
    private boolean               myPubCb         = false;

    private MamaTransport         myTransport     = null;
    private MamaTimer             myTimer         = null;
    private MamaBasicSubscription mySubscription  = null;
    private MamaPublisher         myPublisher     = null;
    private MamaQueueGroup        myQueueGroup    = null;
    private long                  myCount         = 0;
    private long                  myMsgNumber     = 1;

    private MamaTransportTopicListener myTopicListener = null;
    private Level            myLogLevel;

    /**
     * Initialize Mama and create a basic transport.
     */
    private void initializeMama( )
    {
        try
        {
            myBridge = Mama.loadBridge (myMiddleware);
            Mama.open();
            myQueue = Mama.getDefaultQueue (myBridge);
            myQueueGroup = new MamaQueueGroup (1, myBridge);
            myMsg = new MamaMsg();

            /* Add transport and transport topic listeners */
            myTransport = new MamaTransport ();
            myTopicListener = new MamaTransportTopicListener ();
            myTransport.addTransportTopicListener( myTopicListener );
            myTransport.create (myTransportName, myBridge);
        }
        catch( WombatException e )
        {
            e.printStackTrace( );
            System.err.println( "Exception occurred initializing mama: " + e );
            System.exit(1);
        }
    }


    private void createPublisher(  )
    {
        try
        {
            myPublisher = new MamaPublisher();
            if (myPubCb)
            {
                myPublisher.create (myTransport,
                                    myQueueGroup.getNextQueue(),
                                    myOutboundTopic,
                                    null,
                                    this,
                                    null);
            }
            else
            {
                myPublisher.create( myTransport, myOutboundTopic );
            }
        }
        catch( WombatException e  )
        {
            e.printStackTrace( );
            System.err.println( "Exception creating publisher: " + e );
            System.exit( 1 );
        }
    }

    private void publishMessage( MamaMsg request )
    {
        try
        {
            myMsg.clear();

            /* Add some fields. This is not required, but illustrates how to
            * send data.
            */
            short msgType;
            if (myMsgNumber == 1) msgType = MamaMsgType.TYPE_INITIAL;
            else msgType = MamaMsgType.TYPE_UPDATE;

            myMsg.addI32 (MamaReservedFields.MsgType.getName(), MamaReservedFields.MsgType.getId(), msgType);
            myMsg.addI32 (MamaReservedFields.MsgStatus.getName(), MamaReservedFields.MsgStatus.getId(), MamaMsgStatus.STATUS_OK);
            myMsg.addI32 (MamaReservedFields.SeqNum.getName(), MamaReservedFields.SeqNum.getId(), (int) myMsgNumber);
            myMsg.addString ("MdFeedHost", 12, myOutboundTopic);

            if( request != null )
            {
                if( myQuietLevel < 1 )
                {
                    System.out.println( "Publishing message to inbox" );
                }
                myPublisher.sendReplyToInbox( request, myMsg );
            }
            else
            {
                if( myQuietLevel < 1 )
                {
                    System.out.println( "Publishing message " + myMsgNumber + " to " + 
                                        myOutboundTopic );
                }
                myPublisher.send( myMsg );
            }
            myMsgNumber++;
        }
        catch( WombatException e )
        {
            e.printStackTrace( );
            System.err.println( "Exception publishing message: " + e );
            System.exit(1);
        }

    }

    private void createInboundSubscription( )
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

    private void createIntervalTimer( )
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

    public void onCreate(MamaPublisher pub)
    {
        if( myQuietLevel < 1 )
        {
            System.out.println("onPublishCreate: " + pub.getSymbol());
        }
    }

    public void onDestroy(MamaPublisher pub)
    {
        if( myQuietLevel < 1 )
        {
            System.out.println("onPublishDestroy: " + pub.getSymbol());
        }
    }

    public void onError(MamaPublisher pub, short status, String info)
    {
        System.err.println("onPublishError: " + pub.getSymbol() + " " + status + " " + info);
    }

    public void onSuccess(MamaPublisher pub, short status, String info)
    {
        System.err.println("onSuccess: " + pub.getSymbol() + " " + status + " " + info);
    }

    private void parseCommandLine( String [] args )
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
            else if( "-pubCb".equals( arg ) )
            {
                myPubCb = true;
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
        MamaPublisherJava m = new MamaPublisherJava();
        m.run(args);
        System.exit(0);
    }

    public void run(String[] args)
    {
        parseCommandLine( args );

        initializeMama();

        createIntervalTimer();
        createInboundSubscription();
        createPublisher();

        Mama.start (myBridge);

        myTimer.destroy();
        mySubscription.destroy();
        myTransport.destroy();

        Mama.close();
    }

    private class SubscriptionCallback implements MamaBasicSubscriptionCallback
    {
        public void onCreate(
            MamaBasicSubscription  subscription )
        {
            mySubscription = subscription;
            if( myQuietLevel < 2 )
            {
                System.out.println( "Created inbound subscription" );
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
                                    "Inbound msg not from inbox. No reply sent" );
                return;
            }

            publishMessage (msg);
        }

        public void onDestroy (MamaBasicSubscription subscription)
        {
            System.out.println ("Subscription destroyed");
        }
    };



    private class TimerCallback implements MamaTimerCallback
    {
        public void onTimer (MamaTimer timer)
        {
            try
            {
                publishMessage( null );
                if (myCount > 0 && --myCount <= 0)
                {
                    myPublisher.destroy();
                    Thread.sleep(1000);            // let queued events finish
                    Mama.stop(myBridge);
                }
            }
            catch( Exception e )
            {
                   e.printStackTrace( );
                   System.err.println( "Exception occurred in timer cb: " + e );
                   System.exit(1);
            }
        }

        public void onDestroy (MamaTimer timer)
        {
            System.out.println ("Timer destroyed");
        }
    };

}
