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
import java.math.BigDecimal;
import java.text.DecimalFormat;

import com.wombat.mamda.orderbook.*;
import com.wombat.mamda.*;
import com.wombat.mama.Mama;
import com.wombat.mama.MamaBridge;
import com.wombat.mama.MamaMsg;
import com.wombat.mama.MamaTransport;
import com.wombat.mama.MamaSubscriptionType;
import com.wombat.mama.MamaSubscription;
import com.wombat.mama.MamaDictionary;
import com.wombat.mama.MamaDictionaryCallback;
import com.wombat.mama.MamaSource;
import com.wombat.mama.MamaQueueGroup;
import com.wombat.mama.MamaDateTime;

public class MamdaAtomicBookTicker
{
    private static Logger   theLogger   =
       Logger.getLogger( "com.wombat.mamda.examples" );

    /* Controls the logging within the API */
    private static Level            mLogLevel           = Level.INFO;
    /* Controls what gets printed in the example program */
    private static int              mQuietModeLevel     = 0;
    private static MamaSource       mMamaSource;
    private static MamaBridge       mBridge             = null;
    private static MamaQueueGroup   mQueueGroup         = null;
    private static String	        mDictTransportName  = null;
	private static boolean          mShowMarketOrders   = false;

    private static MamaDictionary buildDataDictionary (MamaTransport transport, 
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
                    System.exit(1);
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
            MamaDictionary   dictionary   = subscription.
                createDictionarySubscription (
                    dictionaryCallback,
                    Mama.getDefaultQueue (mBridge),
                    mMamaSource);

            Mama.start (mBridge);
            if (!gotDict[0]) dictionaryCallback.wait (30000);
            if (!gotDict[0])
            {
                System.err.println ("Timed out waiting for dictionary.");
                System.exit (0);
            }
            return dictionary;
        }
    }

    public static void main (final String[] args)
    {
        MamaTransport        transport;
        MamaTransport	     mDictTransport;
        MamaDictionary       dictionary;

        CommandLineProcessor options = new CommandLineProcessor (args);
        mQuietModeLevel              = options.getQuietModeLevel ();
        mLogLevel                    = options.getLogLevel ();
        mShowMarketOrders            = options.showMarketOrders();

        if (mLogLevel != null)
        {
            theLogger.setLevel (mLogLevel);
            Mama.enableLogging (mLogLevel);
        }

        theLogger.info( "Source: " + options.getSource() );
        try
        {
            //Initialize MAMA API
            mBridge = options.getBridge();
            Mama.open();
            transport = new MamaTransport();
            transport.create (options.getTransport(), mBridge);
            mMamaSource = new MamaSource ();

            mDictTransportName = options.getDictTransport();

            if (mDictTransportName != null)
            {
                mDictTransport = new MamaTransport ();
                mDictTransport.create (mDictTransportName, mBridge);
            }
            else 
            {
                mDictTransport = transport;
            }

            //Get the Data dictionary.....
            dictionary = buildDataDictionary(mDictTransport,options.getDictSource());

            MamdaQuoteFields.setDictionary     (dictionary, null);
            MamdaTradeFields.setDictionary     (dictionary, null);
            MamdaCommonFields.setDictionary    (dictionary, null);
            MamdaOrderBookFields.setDictionary (dictionary, null);

            mQueueGroup = new MamaQueueGroup (options.getNumThreads(), mBridge);

            for (Iterator iterator = options.getSymbolList().iterator();
                 iterator.hasNext();)
            {
                final String symbol = (String)iterator.next ();
                MamdaSubscription  aSubscription =
                    new MamdaSubscription ();

                MamdaBookAtomicListener aBookListener = new MamdaBookAtomicListener();
                AtomicBookTicker        aTicker       = new AtomicBookTicker ();

                aBookListener.addBookHandler  (aTicker);
                aBookListener.addLevelHandler (aTicker);
 
                if (options.getPrintEntries())
                {   // Entries
                    aBookListener.addLevelEntryHandler (aTicker);
                }
                aBookListener.setProcessMarketOrders (mShowMarketOrders);

                aSubscription.addMsgListener   (aBookListener);
                aSubscription.addStaleListener (aTicker);
                aSubscription.addErrorListener (aTicker);
                aSubscription.setType          (MamaSubscriptionType.BOOK);

                aSubscription.create (transport,
                                      mQueueGroup.getNextQueue(),
                                      options.getSource (),
                                      symbol,
                                      null);

                theLogger.info ("Subscribed to: " + symbol);
            }

            //Start dispatching on the default MAMA event queue
            Mama.start (mBridge);
            synchronized (MamdaAtomicBookTicker.class)
            {
                MamdaAtomicBookTicker.class.wait ();
            }
        }
        catch (Exception e)
        {
            e.printStackTrace ();
            System.exit (1);
        }
    }

    /*Handler class which receives update callbacks*/
    private static class AtomicBookTicker implements MamdaBookAtomicBookHandler,
                                                     MamdaBookAtomicLevelHandler,
                                                     MamdaBookAtomicLevelEntryHandler,
                                                     MamdaStaleListener,
                                                     MamdaErrorListener
    {
        DecimalFormat df = new DecimalFormat("0.00");

        public void onBookAtomicLevelRecap (
            MamdaSubscription           subscription,
            MamdaBookAtomicListener     listener,
            MamaMsg                     msg,
            MamdaBookAtomicLevel        level)
        {
            if (mQuietModeLevel<2)
            {
                prettyPrint (subscription, level);
            }
        }
    
        public void onBookAtomicBeginBook (
            MamdaSubscription           subscription,
            MamdaBookAtomicListener     listener,
            boolean                     isRecap) 
    
        {
            if (mQuietModeLevel<2)
            {
                System.out.println ("BEGIN BOOK - " + subscription.getSymbol ());
            }
        }
    
        public void onBookAtomicClear (
            MamdaSubscription           subscription,
            MamdaBookAtomicListener     listener,
            MamaMsg                     msg)
        {
            if (mQuietModeLevel<2)
            {
                System.out.println ("BOOK CLEAR - " + subscription.getSymbol ()  + "\n" );
            }
        }
    
        public void onBookAtomicGap (
            MamdaSubscription           subscription,
            MamdaBookAtomicListener     listener,
            MamaMsg                     msg,
            MamdaBookAtomicGap          event)
        {
            if (mQuietModeLevel<2)
            {
                System.out.println ( "BOOK GAP - " + subscription.getSymbol () );
                System.out.println ( "Begin Gap: " + listener.getBeginGapSeqNum () );
                System.out.println ( "End Gap  : " + listener.getEndGapSeqNum () + "\n" );
            }
        }
    
        public void onBookAtomicEndBook (
            MamdaSubscription           subscription,
            MamdaBookAtomicListener     listener) 
        {
            if (mQuietModeLevel<2)
            {
                System.out.print ("\n");
                if ((listener.getPriceLevelNumLevels() == 0) && (listener.getHasMarketOrders() == false))
                {
                    System.out.println ( "END BOOK - " + subscription.getSymbol () );
                    System.out.println ( " - empty book message !!!" );
                }
                else
                {
                    System.out.println ( "END BOOK - " + subscription.getSymbol () + "\n" );
                }
                System.out.print ("\n");
            }
        }
    
        public void onBookAtomicLevelEntryRecap (
            MamdaSubscription             subscription,
            MamdaBookAtomicListener       listener,
            MamaMsg                       msg,
            MamdaBookAtomicLevelEntry     levelEntry)
        {
            if (mQuietModeLevel<2)
            {
                prettyPrint (subscription, levelEntry);
            }
        }
    
        public void onBookAtomicLevelDelta (
            MamdaSubscription           subscription,
            MamdaBookAtomicListener     listener,
            MamaMsg                     msg,
            MamdaBookAtomicLevel        level)
        {
            if (mQuietModeLevel<2)
            {
                prettyPrint (subscription, level);
            }
        }
    
        public void onBookAtomicLevelEntryDelta (
            MamdaSubscription               subscription,
            MamdaBookAtomicListener         listener,
            MamaMsg                         msg,
            MamdaBookAtomicLevelEntry       levelEntry)
        {
            if (mQuietModeLevel<2)
            {
                prettyPrint (subscription, levelEntry);
            }
        }
    
        public void onError (
            MamdaSubscription   subscription,
            short               severity,
            short               errorCode,
            String              errorStr)
        {
            if (mQuietModeLevel<2)
            {
                theLogger.info("Error (" + subscription.getSymbol() + "): "
                                + errorStr);
            }
        }
    
        public void onStale (
            MamdaSubscription   subscription,
            short               quality)
        {
            theLogger.info("Stale (" + subscription.getSymbol() + ")");
        }
    
        public void setShowMarketOrders (boolean showMarketOrders)
        {
            mShowMarketOrders = showMarketOrders;
        }

        void prettyPrint (MamdaSubscription         subscription,
                          MamdaBookAtomicLevelEntry levelEntry)
        {
            // Print Entry Level Info
            String  symbol        = subscription.getSymbol ();
            long    actNumEntries = levelEntry.getPriceLevelActNumEntries ();
            char    entryAction   = levelEntry.getPriceLevelEntryAction ();
            String  entryId       = levelEntry.getPriceLevelEntryId ();
            long    entrySize     = levelEntry.getPriceLevelEntrySize ();
    
            System.out.println ("\tENTRY | " + symbol + " | " + actNumEntries + " | "
                    + entryAction + " | " + entryId + " | " + entrySize );
        }
    
        void prettyPrint (MamdaSubscription    subscription, 
                          MamdaBookAtomicLevel level)
        {
            // Price Level Info
            String  symbol     = subscription.getSymbol ();
            double  price      = level.getPriceLevelPrice ();
            int     size       = (int) level.getPriceLevelSize ();
            char    action     = level.getPriceLevelAction ();
            char    side       = level.getPriceLevelSide ();
            int     numEntries = (int) level.getPriceLevelNumEntries ();
            String  time       = level.getPriceLevelTime().getTimeAsString();
            
            System.out.println ( "\nLEVEL | " + symbol + " | " + df.format(price) + " | " + size + " | "
                    + action + " | " + side + " | " + numEntries + " | " + time);
        }
    }
}
