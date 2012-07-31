/* $Id$
 *
 * OpenMAMA: The open middleware agnostic messaging API
 * Copyright (C) 2012 NYSE Technologies, Inc.
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

package com.wombat.mamda.examples;

import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.Iterator;
import java.util.ArrayList;
import java.util.Date;
import java.io.*;
import java.util.Random;
import com.wombat.mamda.*;
import com.wombat.mama.Mama;
import com.wombat.mama.MamaBridge;
import com.wombat.mama.MamaMsg;
import com.wombat.mama.MamaTransport;
import com.wombat.mama.MamaDictionary;
import com.wombat.mama.MamaDictionaryCallback;
import com.wombat.mama.MamaSubscription;
import com.wombat.mama.MamaSubscriptionType;
import com.wombat.mama.MamaSource;
import com.wombat.mama.MamaTimer;
import com.wombat.mama.MamaQueue;
import com.wombat.mamda.orderbook.*;
import com.wombat.mama.MamaTimerCallback;

public class MamdaBookChurn
{
    private static Logger               theLogger = 
            Logger.getLogger ("com.wombat.mamda.examples");

    private static MamaSource           mMamaSource      = null;
    private static MamaTimer            mChurnTimer      = null;
    private static MamaTimer            mStatsTimer      = null;
    private static MamaTransport        transport        = null;
    private static MamaDictionary       dictionary       = null;
    private static CommandLineProcessor options          = null;
    private static Level                level            = null;
    private static final ArrayList      subscriptionList = new ArrayList(); 
    private static int                  mChurnRate       = 0;
    private static int                  mUpdateStats     = 0;
    private static int                  mRecapStats      = 0;
    private static int                  mBookGapStats    = 0;
    private static int                  mChurnStats      = 0;
    private static Date                 mToday           = null;
    private static FileWriter           mOutFile         = null;
    private static PrintWriter          mOut             = null;
    private static String               mLogFileName     = null;
    private static double               mChurnInterval   = 1.0;
    private static MamaBridge           mBridge          = null;
    private static MamaQueue            mDefaultQueue    = null;

    private static MamaDictionary       buildDataDictionary (MamaTransport transport,
                                                             String        dictSource)
        throws InterruptedException
    {
        final boolean gotDict[] = {false};
        MamaDictionaryCallback dictionaryCallback =
            new MamaDictionaryCallback ()
            {
                public void onTimeout ()
                {
                    System.err.println
                        ("Timed out waiting for dictionary");
                    System.exit (1);
                }

                public void onError (final String s)
                {
                    System.err.println ("Error getting dictionary: " + s);
                    System.exit (1);
                }

                public synchronized void onComplete ()
                {
                    gotDict[0] = true;
                    Mama.stop (mBridge);
                    notifyAll ();
                }
            };

        synchronized (dictionaryCallback)
        {
            MamaSubscription subscription = new MamaSubscription ();
            
            mMamaSource.setTransport (transport);
            mMamaSource.setSymbolNamespace (dictSource);
            MamaDictionary dictionary = 
                subscription.createDictionarySubscription (
                                                dictionaryCallback,
                                                mDefaultQueue,
                                                mMamaSource);
                    

            Mama.start(mBridge);
            if (!gotDict[0]) dictionaryCallback.wait (30000);
            if (!gotDict[0])
            {
                System.err.println( "Timed out waiting for dictionary." );
                System.exit( 0 );
            }
            return dictionary;
        }
    }

    public static void main (final String[] args)
    {
        options      = new CommandLineProcessor (args);
        level        = options.getLogLevel();
        
        try
        {
            //Getting the log File name 
            mLogFileName = options.getLogFileName();
            
            if (mLogFileName != null)
            {
               mOutFile = new FileWriter(mLogFileName); 
               mOut = new PrintWriter(mOutFile);
               mOut.println("Date/Time"+ "," + "ChurnStats" +","
                             + "UpdateStats" + "," + "PeakMsgCount" + ","
                             + "RecapStats" + "," + "BookGapStats" + ","
                             +"freeMemory" +"," +"Memory Used");
               
            }
            mBridge = options.getBridge();

            // Initialize MAMA
            Mama.open ();
            mDefaultQueue = Mama.getDefaultQueue (mBridge);

            if (level != null)
            {
                theLogger.setLevel (level);
                Mama.enableLogging (level);
            }
            theLogger.info( "Source: " + options.getSource() );

            transport = new MamaTransport ();
            transport.create (options.getTransport (), mBridge);
            mMamaSource = new MamaSource ();
            
            //Get the Data dictionary.....
            dictionary = buildDataDictionary (transport,"WOMBAT");
    
            MamdaOrderBookFields.setDictionary (dictionary, null);
            
            for (Iterator iterator = options.getSymbolList ().iterator ();
                 iterator.hasNext ();
                )
            {
                String symbol = (String)iterator.next ();
                subscribeToBooks (symbol);
            }

            mChurnRate = options.getChurnRate(); 

            try
            {
                Thread.sleep(10000);
            }
            catch(Exception e)
            {
                System.out.println("Couldn't Sleep ");
            }

            //Getting the TimerInterval from the cmd prompt
            mChurnInterval = options.getTimerInterval();
            
            //Churn Timer
            if (mChurnRate > 0)
            {
                try
                {
                    mChurnTimer = new MamaTimer();
                    mChurnTimer.create (mDefaultQueue,new ChurnCallback(),mChurnInterval);
                }
                catch ( Exception e )
                {
                    e.printStackTrace( );
                    System.err.println( "Error creating timer: " + e );
                    System.exit(1);
                }
            }
            
            //Stats Timer
            try
            {
                mStatsTimer = new MamaTimer();
                mStatsTimer.create (mDefaultQueue,new StatsCallback(),1.0);
            }
            catch ( Exception e )
            {
                e.printStackTrace( );
                System.err.println( "Error creating timer: " + e );
                System.exit(1);
            }
            
            System.out.println ("Ctrl-C to exit.");
            Mama.start(mBridge);
            synchronized (MamdaListen.class)
            {
                MamdaListen.class.wait ();
            }
            mOut.close();
        }
        catch (Exception e)
        {
            e.printStackTrace();
            System.exit( 1 );
        }
    }

    public static void subscribeToBooks (String symbol)
    {
        MamdaSubscription       aSubscription  = new MamdaSubscription ();
        MamdaOrderBookListener  aBookListener  = new MamdaOrderBookListener ();
        ListenerCallback        callback       = new ListenerCallback ();

        aBookListener.addHandler       (callback);
        aSubscription.addMsgListener   (aBookListener);
        aSubscription.addStaleListener (callback);
        aSubscription.setType          (MamaSubscriptionType.BOOK);
        
        aSubscription.create (transport,
                              mDefaultQueue,
                              options.getSource (),
                              symbol,
                              null);
        
        // Add the Mamda Subscription to the ArrayList
        subscriptionList.add (aSubscription);
        theLogger.fine ("Subscribed to: " + symbol);
    }
    
    private static class ListenerCallback implements MamdaOrderBookHandler, 
                                                     MamdaStaleListener
    {
        public void onBookClear (
            final MamdaSubscription        sub,
            final MamdaOrderBookListener   listener,
            final MamaMsg                  msg,
            final MamdaOrderBookClear      clear,
            final MamdaOrderBook           fullBook)
        {
            System.out.println ("BookClear");
        }

        public void onStale (
            MamdaSubscription   subscription,
            short               quality)
        {
            System.out.println ("Stale (" + subscription.getSymbol() + ")");
        }

        /**
         * Method invoked when an updated full order book image is
         * available.  The reason for the invocation may be any of the
         * following:
         * - Initial image.
         * - Recap update (e.g., after server fault tolerant event or data
         * quality event.)
         * - After stale status removed.
         *
         * @param subscription The subscription which received the update.
         * @param listener     The listener which invoked this callback.
         * @param msg          The MamaMsg that triggered this invocation.
         * @param delta        The order book delta generated from the update.
         * @param event        The MamdaOrderBookRecap.
         * @param book         The book.
         */
        public void onBookRecap (MamdaSubscription subscription,
                                 MamdaOrderBookListener listener, 
                                 MamaMsg msg,
                                 MamdaOrderBookComplexDelta delta,
                                 MamdaOrderBookRecap event, 
                                 MamdaOrderBook book)
        {
            ++mRecapStats;
        }

        /**
         * Method invoked when a regular update for an order book is
         * available.
         *
         * @param subscription The subscription which received the update.
         * @param listener     The listener which invoked this callback.
         * @param msg          The MamaMsg that triggered this invocation.
         * @param event        The order book delta generated from the update.
         * @param book         The full order book after the update is applied.
         */
        public void onBookDelta (MamdaSubscription subscription,
                                 MamdaOrderBookListener listener,
                                 MamaMsg msg,
                                 MamdaOrderBookSimpleDelta event,
                                 MamdaOrderBook book)
        {
            ++mUpdateStats;
        }

        /**
         * Method invoked when an order book delta is reported.  A delta
         * consists of one or more price levels (add/update/delete), each
         * of which contains zero or more entries (add/update/delete).
         * Some feeds do not provide order book entry information.
         *
         * @param subscription The MamdaSubscription handle.
         * @param listener     The order book listener that received the update.
         * @param msg          The MamaMsg that triggered this invocation.
         * @param event        Details of the the delta.
         * @param book         The current full book (after applying the delta).
         */
        public void onBookComplexDelta (MamdaSubscription subscription,
                                        MamdaOrderBookListener listener,
                                        MamaMsg msg,
                                        MamdaOrderBookComplexDelta event,
                                        MamdaOrderBook book)
        {
            System.out.println ("ComplexDelta");
        }

        /**
         * Method invoked when a gap in order book updates is discovered.
         *
         * @param subscription The subscription which received the update.
         * @param listener     The listener which invoked this callback.
         * @param msg          The MamaMsg that triggered this invocation.
         * @param event        Access to details on the order book gap event.
         * @param fullBook     The full order book.
         */
        public void onBookGap (MamdaSubscription subscription,
                               MamdaOrderBookListener listener, 
                               MamaMsg msg,
                               MamdaOrderBookGap event, 
                               MamdaOrderBook fullBook)
        {
            ++mBookGapStats;
        }
    }

    // ChurnCallback
    private static class ChurnCallback implements MamaTimerCallback 
    {
        private MamdaSubscription  sub         = null;
        private String             tempSymbol  = null;
        private int                mNumToChurn = 0;
        private int                mRandomNo   = 0; 
        Random                     generator   = new Random();
        
        public void onTimer (MamaTimer timer)
        {
            mNumToChurn = mChurnRate;
            while (mNumToChurn-- > 0)
            {
                mRandomNo = generator.nextInt(65536) % options.getSymbolListSize();
                // Accessing the mamda subscription object from the arrayList
                try
                {
                    sub = (MamdaSubscription)subscriptionList.remove (mRandomNo);
                    tempSymbol = sub.getSymbol();
                    if (tempSymbol != null) 
                    {
                        // Deactivating the Mamda Subscription
                        sub.deactivate();
                        // Creating the MamdaSubscription 
                        subscribeToBooks (tempSymbol);
                        ++mChurnStats;
                    }
                }
                catch(Exception e)
                {
                }
            }
        }

        public void onDestroy (MamaTimer timer)
        {
        }
    }

    // StatsCallback
    private static class StatsCallback implements MamaTimerCallback
    {
        private int mPeakMsgCount = 0;
        
        public void onTimer (MamaTimer timer)
        {
            mToday = new Date();
            
            if (mUpdateStats > mPeakMsgCount)
            {
                mPeakMsgCount = mUpdateStats;
            }

            if(mOut != null)
            {
                 mOut.println(
                         mToday.toString() + "," + mChurnStats + "," + mUpdateStats + "," +
                         mPeakMsgCount + "," + mRecapStats + "," +mBookGapStats + "," 
                         + Runtime.getRuntime().freeMemory() +"," + ((Runtime.getRuntime().totalMemory())-(Runtime.getRuntime().freeMemory())));
                 mOut.flush();
            }
                               
            System.out.println(
                    mToday.toString() +" ChurnStats:" + mChurnStats +
                    " UpdateStats:" + mUpdateStats + " PeakMsgStats:" + mPeakMsgCount + 
                    " RecapStats:"  + mRecapStats  + " BookGap:" + mBookGapStats +
                    " Free Memory:" + (Runtime.getRuntime().freeMemory())/1024 +" Memory Used:" +
                    ((Runtime.getRuntime().totalMemory())-(Runtime.getRuntime().freeMemory()))/1024);
            
            mChurnStats    = 0;
            mUpdateStats   = 0;
            mRecapStats    = 0;
            mBookGapStats  = 0;
        }

        public void onDestroy (MamaTimer timer)
        {
        }
    }
}
