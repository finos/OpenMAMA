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

import java.util.ArrayList;
import java.util.Iterator;
import java.util.logging.Logger;
import java.util.logging.Level;

import com.wombat.mama.*;

/*----------------------------------------------------------------------------
 * This sample application subscribes to a symbol list and then makes individual 
 * subscriptions to the symbols returned.
 * Please note the program will be more efficient by using the -q flag.
 *
 * It accepts the following command line arguments: 
 *
 *      [-S, -source]      The symbol name space for the data.
 *      [-m middleware]    The middleware to use [wmw/lbm/tibrv].
 *      [-tport name]      The transport parameters to be used from
 *                         mama.properties. 
 *      [-d name]          The symbol name space for the data dictionary. Default:WOMBAT
 *      [-dict_tport] name The name of the transport to use for obtaining the data"
 *                         dictionary.
 *      [-q]               Quiet mode. Suppress output.
 *      [-v]               Increase the level of logging verbosity. Pass up to 4
 *                         times.
 *      [-threads] n       The number of threads/queues from which to dispatch data.
 *                         The subscriptions will be created accross all queues in a
 *                         round robin fashion.
 *
 *
 *---------------------------------------------------------------------------*/

public class MamaSymbolListSubscriberJava
{
    private static MamaTransport    transport;
    private static MamaTransport    myDictTransport;
    private static String           myMiddleware        = "wmw";
    private static MamaBridge       myBridge            = null;
    private static MamaQueue        myDefaultQueue      = null;


    private static MamaDictionary   dictionary;
    private static String           dictSource          = "WOMBAT";
    private static boolean          dictionaryComplete  = false;

    private static String           transportName       = "internal";
    private static String           myDictTportName     = null;
    private static int              quietness           = 0;

    private static int              numThreads          = 0;
    private static MamaQueueGroup   queueGroup          = null;

    private static final ArrayList  subscriptions       = new ArrayList();
    private static final ArrayList  subjectList         = new ArrayList();

    private static String           mySymbolNamespace   = null;

    private static final SubscriptionCallback subscriptionCallback
                            = new SubscriptionCallback();
    private static final SymbolListSubscriptionCallback symbolListSubscriptionCallback
                            = new SymbolListSubscriptionCallback();

    private static final Logger     logger              =
                                    Logger.getLogger( "com.wombat.mama" );
    private static Level            myLogLevel;
    private static MamaSource       mySource            = null;
    private static MamaSource       myDictSource        = null;

    private static boolean          myBuildDataDict 	=true;
    
    public static void main (final String[] args)
    {
        parseCommandLine (args);

        try
        {
            initializeMama ();
            if (myBuildDataDict)
            	buildDataDictionary ();

            /* Subscribe to the symbol list */
            MamaSubscription symbolListSubscription = new MamaSubscription();
            symbolListSubscription.setSubscriptionType (MamaSubscriptionType.SYMBOL_LIST);
            symbolListSubscription.createSubscription (symbolListSubscriptionCallback,
                                                 myDefaultQueue, 
                                                 mySource,
                                                 "Symbol List",
                                                 null);

            System.out.println( "Type CTRL-C to exit." );
            /*This will only block on JNI*/
            Mama.start (myBridge);
            /*Keep the main thread running if using pure Java MAMA*/
            Thread.currentThread().join();
        }
        catch (Exception e)
        {
            if (e.getCause() != null)
            {
                e.getCause ().printStackTrace ();
            }

            e.printStackTrace ();
            System.exit (1);
        }
    }

    private static void subscribeToSubjects()
    {
        int howMany = 0;

        queueGroup = new MamaQueueGroup (numThreads, myBridge);

        /* Subscribe to all symbols parsed from the symbol list */
        for (Iterator iterator = subjectList.iterator(); iterator.hasNext();)
        {
            final String symbol = (String) iterator.next();

            MamaSubscription subscription = new MamaSubscription ();

            subscription.createSubscription (
                subscriptionCallback,
                queueGroup.getNextQueue (),
                mySource,
                symbol,
                null);

            logger.fine ("Subscribed to: " + symbol);

            if (++howMany % 1000 == 0)
            {
                System.out.println ("Subscribed to " + howMany + " symbols.");
            }
        }
    }

    private static void buildDataDictionary()
        throws InterruptedException
    {
        MamaDictionaryCallback dictionaryCallback = createDictionaryCallback();

        synchronized (dictionaryCallback)
        {
            /*The dictionary is obtained through a specialized form of
             * subscription.*/
            MamaSubscription subscription = new MamaSubscription ();

            dictionary = subscription.createDictionarySubscription (
                    dictionaryCallback,
                    myDefaultQueue,
                    myDictSource,
                    10.0,
                    2);
            Mama.start (myBridge);
            if (!dictionaryComplete) dictionaryCallback.wait( 30000 );
            if (!dictionaryComplete)
            {
                System.err.println( "Timed out waiting for dictionary." );
                System.exit( 0 );
            }
        }
    }

    private static MamaDictionaryCallback createDictionaryCallback()
    {
        return new MamaDictionaryCallback()
        {
            public void onTimeout()
            {
                System.err.println( "Timed out waiting for dictionary" );
                System.exit(1);
            }

            public void onError (final String s)
            {
                System.err.println ("Error getting dictionary: " + s);
                System.exit (1);
            }

            public synchronized void onComplete()
            {
                dictionaryComplete = true;
                Mama.stop(myBridge);
                notifyAll();
            }
        };
    }

    private static void initializeMama()
    {
        try
        {
            myBridge = Mama.loadBridge (myMiddleware);
            /* Always the first API method called. Initialized internal API
             * state*/
            Mama.open ();
            myDefaultQueue = Mama.getDefaultQueue (myBridge);
        }
        catch (Exception e)
        {
            e.printStackTrace ();
            System.out.println ("Failed to initialize MAMA");
            System.exit (1);
        }

        transport = new MamaTransport ();
        /* The name specified here is the name identifying properties in the
         * mama.properties file*/
        transport.create (transportName, myBridge);

        if (myDictTportName != null)
        {
            myDictTransport = new MamaTransport ();
            myDictTransport.create (myDictTportName, myBridge);
        }
        else 
        {
            myDictTransport = transport;
        }

        /*MamaSource for all subscriptions*/
        mySource     = new MamaSource ();
        mySource.setTransport (transport);
        mySource.setSymbolNamespace (mySymbolNamespace);

        /*MamaSource for dictionary subscription*/
        myDictSource = new MamaSource ();
        myDictSource.setTransport (myDictTransport);
        myDictSource.setSymbolNamespace (dictSource);
    }
 
    private static void parseCommandLine (final String[] args)
    {
        for(int i = 0; i < args.length;)
        {
            if (args[i].equals ("-source") || args[i].equals("-S"))
            {
                mySymbolNamespace = args[i +1];
                i += 2;
            }
            else if (args[i].equals ("-d") || args[i].equals("-dict_source"))
            {
                dictSource = args[i + 1];
                i += 2;
            }
            else if (args[i].equals ("-dict_tport"))
            {
                myDictTportName = args[i + 1];
               i += 2;
            }
            else if (args[i].equals ("-tport"))
            {
                transportName = args[i + 1];
                i += 2;
            }
            else if (args[i].equals ("-threads"))
            {
                numThreads = Integer.parseInt (args[i+1]);
                i += 2;
            }
            else if (args[i].equals ("-q"))
            {
                myLogLevel = myLogLevel == null
                            ? Level.WARNING : myLogLevel == Level.WARNING
                            ? Level.SEVERE  : Level.OFF;

                Mama.enableLogging (myLogLevel);
                quietness++;
                i++;
            }
            else if (args[i].equals ("-v"))
            {
                myLogLevel = myLogLevel == null
                            ? Level.FINE    : myLogLevel == Level.FINE
                            ? Level.FINER   : Level.FINEST;

                Mama.enableLogging (myLogLevel);
                i++;
            }
            else if (args[i].equals ("-B"))
            {
            	myBuildDataDict=false;
                i++;
            }
            else if (args[i].equals ("-m"))
            {
                myMiddleware = args[i + 1];
                i += 2;
            }
        }
    }

    /*Class for processing all event callbacks for all subscriptions*/
    private static class SubscriptionCallback implements MamaSubscriptionCallback
    {
        public void onMsg (final MamaSubscription subscription, final MamaMsg msg)
        {
            if (quietness < 1)
            {
                System.out.println("Recieved message of type " + MamaMsgType.stringForType (MamaMsgType.typeForMsg(msg)) + " for " + subscription.getSymbol());
            }
        }

        /*Invoked once the subscrption request has been dispatched from the
         * throttle queue.*/
        public void onCreate (MamaSubscription subscription)
        {
            subscriptions.add (subscription);
        }

        /*Invoked if any errors are encountered during subscription processing*/
        public void onError(MamaSubscription subscription,
                            short            mamaStatus,
                            int              tibrvStatus,
                            String           subject,
                            Exception        e)
        {
            System.err.println ("Symbol=[" + subscription.getSymbol() + "] : " +
                                "An error occurred creating subscription: " +
                                MamaStatus.stringForStatus (mamaStatus));

        }

        /*Invoked if the quality status for the subscription changes*/
        public void onQuality (MamaSubscription subscription, short quality,
                               short cause, final Object platformInfo)
        {
            System.err.println( subscription.getSymbol () +
                                ": quality is now " +
                                MamaQuality.toString (quality) +
                                ", cause " + cause +
                                ", platformInfo: " + platformInfo);
        }

        public void onGap (MamaSubscription subscription)
        {
            System.err.println (subscription.getSymbol () + ": gap detected ");
        }

        public void onRecapRequest (MamaSubscription subscription)
        {
            System.err.println (subscription.getSymbol () + ": recap requested ");
        }

        public void onDestroy (MamaSubscription subscription)
        {
            System.out.println ("Subscription destroyed.");
        }
    }
    
    private static class SymbolListSubscriptionCallback implements MamaSubscriptionCallback
    {
        public void onMsg (MamaSubscription subscription, MamaMsg msg)
        {
            switch (MamaMsgType.typeForMsg(msg))
            {
                case MamaMsgType.TYPE_INITIAL:
                    String newSymbols = msg.getString (MamaReservedFields.SymbolList.getName(), MamaReservedFields.SymbolList.getId());
                    String[] parsedSymbols = newSymbols.split(",");
                    for (int i=0; i<parsedSymbols.length; i++)
                    {
                       subjectList.add ((Object)parsedSymbols[i]);
                    }   
                    System.out.println("Received initial for symbol list subscription.  Updating list of symbols to subscribe to.\n");
                    break;
                case MamaMsgType.TYPE_END_OF_INITIALS:
                    System.out.println("End of initials.  Subscribing to symbols.\n");
                    subscribeToSubjects();
                    break;
                default:
                    break;
            }
        }

        public void onCreate (MamaSubscription subscription)
        {
            if (quietness < 1)
            {
                System.out.println("Created symbol list subscription.\n");
            }    
        }

        public void onError (MamaSubscription subscription,
                             short            mamaStatus,
                             int              tibrvStatus,
                             String           subject,
                             Exception        e)
        {
             System.err.println ("Symbol list subscription: An error occurred creating subscription: " +
                                 MamaStatus.stringForStatus (mamaStatus));    
        }

        /*Invoked if the quality status for the subscription changes*/
        public void onQuality (MamaSubscription subscription, short quality,
                               short cause, final Object platformInfo)
        {
            System.err.println( "Symbol list subscription: quality is now " +
                                MamaQuality.toString (quality) +
                                ", cause " + cause +
                                ", platformInfo: " + platformInfo);
        }

        public void onGap (MamaSubscription subscription)
        {
            System.err.println (subscription.getSymbol () + ": gap detected ");
        }

        public void onRecapRequest (MamaSubscription subscription)
        {
            System.err.println (subscription.getSymbol () + ": recap requested ");
        }

        public void onDestroy (MamaSubscription subscription)
        {   
            System.out.println ("Subcription destroyed");
        }
    }
}



