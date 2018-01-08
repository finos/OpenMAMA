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

package com.wombat.mama.testtools.load;

import java.util.Random;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.logging.Logger;
import java.util.logging.Level;
import java.util.Date;
import java.io.*;
import java.text.*;

import com.wombat.mama.*;

interface ConstantStuff
{
    public static final int DEFAULT_CHURN_RATE = 10;
}

public class MamaChurnTest
{
    private static MamaTransport    transport;
    private static long             throttle            = -1;
    private static MamaDictionary   dictionary;
    private static MamaBridge       myBridge            = null;
    private static String           myMiddleware        = "wmw";
    private static String           dictSource          = "WOMBAT";
    private static MamaQueue        myDefaultQueue      = null;
    private static boolean          dictionaryComplete  = false;
    private static String           transportName       = "internal";    
    private static boolean          requireInitial      = true;
    private static double           timeout             = 10.0;
    private static int              quietness           = 0;
    private static double           churnRate           = ConstantStuff.DEFAULT_CHURN_RATE;
    private static int              numThreads          = 0;
    private static MamaQueueGroup   queueGroup          = null;   
    private static boolean          qualityForAll       = true;
    private static final ArrayList  subscriptions       = new ArrayList();
    private static final ArrayList  subjectList         = new ArrayList();
    private static       String     filename            = null;
    private static String           mySymbolNamespace   = null;
    private static final SubscriptionCallback   
        callback            = new SubscriptionCallback();
    private static final Logger     logger              = 
                                    Logger.getLogger( "com.wombat.mama" );
    private static Level            myLogLevel;
    private static MamaSource       mySource;   
    private static MamaSource       myDictSource;
    private static MamaTimer        myChurnTimer        = null; 
    private static MamaTimer        myShutdownTimer     = null;
    private static MamaTimer        myStatsTimer        = null;
    private static final ArrayList  subscriptionInfoList= new ArrayList();
    private static int              myShutdownTimeout   = 0;
    private static int              myChurnStats        = 0;
    private static int              myRecapStats        = 0; 
    private static int              myMsgStats          = 0;
    private static FileWriter       myOutFile           = null;
    private static PrintWriter      myOut               = null;
    private static String           logFileName         = null; 
    private static int              myNumToChurn        = 0;

    public static void main (final String[] args)
    {
       
        parseCommandLine (args);
        try
        {
            if(logFileName != null)
            {
                myOutFile = new FileWriter(logFileName);
                myOut = new PrintWriter(myOutFile);
                myOut.println("Date/Time"+ "," + "ChurnStats" +","
                        +"RecapStats" + "," + "MsgStats" + "," +
                        "PeakMsgCount" + "," +"freeMemory" +"," +"Memory Used");
            }
                
            if (subjectList.isEmpty())
            {
                readSubjectsFromFile();
            }

            // Set the churn to 0 if some invalid value is passed
            if(myNumToChurn > subjectList.size() || (myNumToChurn < 1))
            {
                myNumToChurn = 0;
            }

            initializeMama ();
            buildDataDictionary ();
            
            /*Subscribe to all symbols specified on the command line or from the symbol file*/
            for (Iterator iterator = subjectList.iterator();iterator.hasNext();)
            {
                final String symbol = (String) iterator.next();
                subscribeToSubject(symbol);
            }
            
            //Stats Timer
            myStatsTimer = new MamaTimer();
            myStatsTimer.create (myDefaultQueue, new StatsCallback(),1.0);
            
           //Churn Timer
            if ( churnRate > 0)
            {
                try
                {
                    myChurnTimer = new MamaTimer();
                    myChurnTimer.create (myDefaultQueue, new ChurnCallback(myNumToChurn),churnRate);
                }
                catch( Exception e)
                {
                    e.printStackTrace( );
                    System.err.println( "Error creating timer: " + e );
                    System.exit(1);
                }
            }
                
            // Shutdown timer
            if ( myShutdownTimeout > 0)
            {
                myShutdownTimer = new MamaTimer();
                myShutdownTimer.create (myDefaultQueue, new ShutdownCallback(myBridge, queueGroup),myShutdownTimeout);
            }
                
            System.out.println( "Type CTRL-C to exit." );
            Mama.start (myBridge);
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
        finally
        {
            shutdown ();
        }
    }
    
    //Subscribe to a particular symbol
    private static void subscribeToSubject(final String symbol)
    {
        MamaSubscription subscription = new MamaSubscription ();
        /*Properties common to all subscription types*/
        subscription.setTimeout (timeout);
        subscription.setRetries (1);
        subscription.setRequiresInitial (requireInitial);
        subscription.createSubscription (
                        callback,
                        queueGroup.getNextQueue (),
                        mySource,
                        symbol,
                        null);
        
        //Adding the subscription to the array list
        subscriptionInfoList.add(subscription); 
    }

    private static void buildDataDictionary() throws InterruptedException
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
    //Shutdown function
    private static void shutdown()
    {
        for (Iterator iterator = subscriptions.iterator(); iterator.hasNext();)
        {
            final MamaSubscription subscription = (MamaSubscription) iterator.next();
            subscription.destroyEx();
        }

        if(queueGroup != null)
        {
            queueGroup.destroyWait();
        }
        if (transport != null)
        {
            transport.destroy();
        }
        
        Mama.close();
        myOut.close();
    }

    private static void initializeMama()
    {
        try
        {
            myBridge = Mama.loadBridge (myMiddleware);
            /*Always the first API method called. Initialized internal API
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
        /*The name specified here is the name identifying properties in the
         * mama.properties file*/
        transport.create (transportName, myBridge);
       
        /*Receive notification of transport level events*/
        transport.addTransportListener( new MamaTransportListener()
        {
            public void onDisconnect(short cause, final Object opaque)
            {
                System.out.println ("TRANSPORT DISCONNECTED!");
            }

            public void onReconnect(short cause, final Object opaque)
            {
                System.out.println ("TRANSPORT RECONNECTED!");
            }

            public void onQuality(short cause, final Object opaque)
            {
                System.out.println ("TRANSPORT QUALITY!");
                short quality = transport.getQuality();
                System.out.println ("Transport quality is now " +
                                    MamaQuality.toString(quality) +
                                    ", cause " + cause +
                                    ", opaque: " + opaque);
            }

            public void onConnect(short cause, final Object platformInfo)
            {
                System.out.println ("TRANSPORT CONNECTED!");
            }

            public void onPublisherDisconnect(short cause, final Object platformInfo)
            {
                System.out.println ("PUBLISHER DISCONNECTED!");
            }

            public void onAccept(short cause, final Object platformInfo)
            {
                System.out.println ("TRANSPORT ACCEPTED!");
            }

            public void onAcceptReconnect(short cause, final Object platformInfo)
            {
                System.out.println ("TRANSPORT RECONNECT ACCEPTED!");
            }

            public void onNamingServiceConnect(short cause, final Object platformInfo)
            {
                System.out.println ("NSD CONNECTED!");
            }

            public void onNamingServiceDisconnect(short cause, final Object platformInfo)
            {
                System.out.println ("NSD DISCONNECTED!");
            }
        } );

        if (!qualityForAll)
        {
            transport.setInvokeQualityForAllSubscs (false);
        }

        /*MamaSource for all subscriptions*/
        mySource     = new MamaSource ();
        mySource.setTransport (transport);
        mySource.setSymbolNamespace (mySymbolNamespace);

        /*MamaSource for dictionary subscription*/
        myDictSource = new MamaSource ();
        myDictSource.setTransport (transport);
        myDictSource.setSymbolNamespace (dictSource);

        if (throttle != -1)
        {
            transport.setOutboundThrottle(MamaThrottleInstance.DEFAULT_THROTTLE, throttle);
        }

        // Create a big load of queues and threads
        queueGroup = new MamaQueueGroup (numThreads, myBridge);
    }

    private static void readSubjectsFromFile() throws IOException
    {
        InputStream input;
        if (filename != null)
        {
            input = new FileInputStream (filename);
        }
        else
        {
            input = System.in;
            System.out.println ("Enter one symbol per line and terminate with a .");
            System.out.print ("SUBJECT>");
        }

        final BufferedReader reader =
            new BufferedReader (new InputStreamReader (input));

        String symbol;
        while (null != (symbol = reader.readLine()))
        {
            if (!symbol.equals(""))
            {
                if (symbol.equals( "." ))
                {
                    break;
                }
                subjectList.add (symbol);
            }

            if (input == System.in)
            {
                System.out.print ("SUBJECT>");
            }
        }

        if (subjectList.isEmpty())
        {
            System.err.println ("No subjects specified");
            System.exit (1);
        }
    }
    
    //Parsing CommandLine Parameter
    private static void parseCommandLine (final String[] args)
    {
        for(int i = 0; i < args.length;)
        {
            if (args[i].equals ("-source") || args[i].equals("-S"))
            {
                mySymbolNamespace = args[i +1];
                i += 2;
            }
            else if (args[i].equals ("-s"))
            {
                subjectList.add (args[i + 1]);
                i += 2;
            }
            else if (args[i].equals ("-f"))
            {
                filename = args[i + 1];
                i += 2;
            }
            else if (args[i].equals("-r"))
            {
                throttle = Long.parseLong(args[i+1]);
                i += 2;
            }
            else if (args[i].equals ("-d"))
            {
                dictSource = args[i + 1];
                i += 2;
            }
            else if (args[i].equals ("-tport"))
            {
                transportName = args[i + 1];
                i += 2;
            }
            else if (args[i].equals ("-q"))
            {
                quietness++;
                i++;
            }
            else if (args[i].equals ("-m"))
            {
              myMiddleware = args[i + 1];
              i += 2;
            }
            else if (args[i].equals ("-v"))
            {
                myLogLevel = myLogLevel == null
                            ? Level.FINE    :
                            myLogLevel == Level.FINE    
                            ? Level.FINER   : 
                            Level.FINEST;

                Mama.enableLogging (myLogLevel);
                i++;
            }
                        
            else if (args[i].equals ("-t") || args[i].equals("-timeout"))
            {
                churnRate = Double.parseDouble (args[i+1]);
                i +=2;
            }

            else if (args[i].equals ("-c") || args[i].equals("-churn"))
            {
                myNumToChurn = Integer.parseInt(args[i+1]);
                i +=2;
            }

            else if (args[i].equals ("-logfile"))
            {
                logFileName = args[i + 1];
                i +=2;
            }

            else if (args[i].equals ("-threads"))
            {
                numThreads = Integer.parseInt(args[i+1]);
                i += 2;
            }

            else if (args[i].equals ("-k"))
            {
                myShutdownTimeout = Integer.parseInt(args[i+1]);
                i +=2;
            }

            else
            {
                i++;
            }

        }
    }

    /*Class for processing all event callbacks for all subscriptions*/
    private static class SubscriptionCallback implements MamaSubscriptionCallback
    {
        public void onMsg (final MamaSubscription subscription, final MamaMsg msg)
        {
            myMsgStats++;
            try
            {
                switch (MamaMsgType.typeForMsg (msg))
                {
                    case MamaMsgType.TYPE_INITIAL:
                    case MamaMsgType.TYPE_RECAP:
                        myRecapStats++;
                        return;
                    case MamaMsgType.TYPE_DELETE:
                    case MamaMsgType.TYPE_EXPIRE:
                        subscription.destroy ();
                        subscriptions.remove (subscription);
                        return;
                }

                switch (MamaMsgStatus.statusForMsg (msg))
                {
                    case MamaMsgStatus.STATUS_BAD_SYMBOL:
                    case MamaMsgStatus.STATUS_EXPIRED:
                    case MamaMsgStatus.STATUS_TIMEOUT:
                        subscription.destroy();
                        subscriptions.remove (subscription);
                        return;
                }
            }
            catch (Exception ex) 
            {
                ex.printStackTrace ();
                System.exit (0);
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
            /*System.err.println ("Symbol=[" + subscription.getSymbol() + "] : " +
                                "An error occurred creating subscription: " +
                                MamaMsgStatus.stringForStatus (mamaStatus));*/

        }

        /*Invoked if the quality status for the subscription changes*/
        public void onQuality (MamaSubscription subscription, short quality,
                               short cause, final Object opaque)
        {
            System.err.println( subscription.getSymbol () + 
                                ": quality is now " +
                                MamaQuality.toString (quality) +
                                ", cause " + cause +
                                ", opaque: " + opaque);
        }
        /*Invoked for gap*/
        public void onGap (MamaSubscription subscription)
        {
            
        }

        /*Invoked on recap request*/
        public void onRecapRequest (MamaSubscription subscription)
        {
        }

        public void onDestroy (MamaSubscription subscription)
        {
        }
    }
   
    //ChurnCallback
    private static class ChurnCallback implements MamaTimerCallback
    {
        private long myLastCallTime = 0;
        private long myCurrentTime  = 0;
        private int  myDifference   = 0;
        private int  myNumToChurn   = 0;
        private int  myRandomNo     = 0;
        private String myTempSymbol = null;
        MamaSubscription sub        = null;
        
        Random generator = new Random();
       
        public ChurnCallback(int numToChurn)
        {
            // Save arguments in member variables
            myNumToChurn = numToChurn;
        }
       
        public void onTimer (MamaTimer timer)
        {
            // Work out the number to churn
            int numToChurn = myNumToChurn;
            if(numToChurn == 0)
            {
            myCurrentTime = System.currentTimeMillis ();
            if (myLastCallTime == 0)
            {
                myDifference   = 1;
            }
            else
            {
                myDifference = (int)(myCurrentTime - myLastCallTime);
            }
            myLastCallTime = myCurrentTime;
            
                numToChurn = (int)(churnRate * myDifference) / 1000;
            }
            
            while (numToChurn-- > 0)
            {
                myRandomNo = generator.nextInt(65536) % subjectList.size();
                //Getting Subscription Object for the ArrayList
                sub = (MamaSubscription)subscriptionInfoList.get(myRandomNo);
                System.out.println("Subscritpion state is " + MamaSubscriptionState.toString(sub.getState()));
                try
                {    
                    myTempSymbol = sub.getSymbol();
                }
                catch(Exception e)
                {
                    System.out.println("Error getting Symbol");
                }
                if (myTempSymbol != null)
                {
                    //UnSubscribe for the particular symbol
                    {
                        sub.destroyEx();
                        subscriptionInfoList.remove(myRandomNo);
                    }
                    //Subscribe for the particular symbol
                    subscribeToSubject(myTempSymbol);
                    ++myChurnStats;
                }
            }
        }

        public void onDestroy (MamaTimer timer)
        {
            System.out.println ("Timer destroyed");
        }
    }
    
    //StatsCallback
    private static class StatsCallback implements MamaTimerCallback
    {
        private int myPeakMsgCount = 0;
         
        public void onTimer (MamaTimer timer)
        {
            if (myMsgStats > myPeakMsgCount)
            {
                myPeakMsgCount = myMsgStats;
            }
            
            if(myOut != null)
            {
                myOut.println(
                        DateFormat.getDateTimeInstance().format(new Date()) +
                         "," + myChurnStats +"," + myRecapStats + "," + myMsgStats + "," +
                        myPeakMsgCount + "," + Runtime.getRuntime().freeMemory() +"," +
                        ((Runtime.getRuntime().totalMemory())-(Runtime.getRuntime().freeMemory())));
                myOut.flush();
            }
           
            System.out.println(
                    DateFormat.getDateTimeInstance().format(new Date()) +
                    " Churns:" + myChurnStats +" Recaps:" +
                    myRecapStats + " Msgs:" + myMsgStats + " PeakMsgs:" +
                    myPeakMsgCount + " FreeMem:" + Runtime.getRuntime().freeMemory() +" UsedMem:" +
                    ((Runtime.getRuntime().totalMemory())-(Runtime.getRuntime().freeMemory())));
                    
            myChurnStats = 0;
            myMsgStats   = 0;
            myRecapStats = 0;
        }

        public void onDestroy (MamaTimer timer)
        {
            System.out.println ("Timer destroyed");
        }
    }

    //ChurnCallback
    private static class ShutdownCallback implements MamaTimerCallback
    {
        private MamaBridge myBridge;
        private MamaQueueGroup myQueues;

        public ShutdownCallback(MamaBridge bridge, MamaQueueGroup queues)
        {
            myBridge = bridge;
            myQueues = queues;
        }

        public void onTimer (MamaTimer timer)
        {
            // Stop all the queues
            if(myQueues != null)
            {
                myQueues.stopDispatch();
            }

            // Stop mama
            Mama.stop(myBridge);
        }
    
        public void onDestroy (MamaTimer timer)
        {
            System.out.println ("Timer destroyed");
        }
    }
}
