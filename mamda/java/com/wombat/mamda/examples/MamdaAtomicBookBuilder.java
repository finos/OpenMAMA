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
import com.wombat.mama.MamaQuality;
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

/*  *********************************************************************** */
/* This example program demonstrates the use of the MAMDA atomic book       */
/* listener to maintain an orderbook. Due to platform differences between   */
/* V5 and legacy platforms, a number of methods should not be accessed      */
/* when processing V5 entry updates using the atomic book listener          */
/* interface. This example program demonstrates how an orderbook can be     */
/* maintained by using the atomic book listener, independent of platform    */
/* (V5 or legacy) or updates being received (entry or level).               */

/* In this application, a MamdaOrderBook object is created and populated    */ 
/* as updates are received. For each update, the orderbook is printed.      */

/* The application handles the following scenarios:                         */
/*      1. V5 entry updates which have invalid level information            */
/*      2. Legacy entry updates which have fully structured book            */
/*      2. Level updates which have no entry information                    */

/* To handle all these cases, the application store processes levels and    */
/* entries. When a level callback is received, a MamdaOrderBookPriceLevel   */
/* is built but not applied to the book. If an entry callback is received   */
/* following this, the entry is applied to the book and the price level     */
/* is not applied to the book. If no entry callback is received, the        */
/* price level is applied to the book and in this manner the correct        */
/* book state is maintained.                                                */

/*  *********************************************************************** */

public class MamdaAtomicBookBuilder
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
    private static int              mPrecision          = 0;
    private static boolean          mPrintEntries       = false;

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
                System.err.println ("Timed out waiting for dictionary");
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

            mMamaSource.setTransport       (transport);
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

        mQuietModeLevel     = options.getQuietModeLevel ();
        mLogLevel           = options.getLogLevel ();
        mShowMarketOrders   = options.showMarketOrders();
        mPrecision          = options.getPrecision();
        mPrintEntries       = options.getPrintEntries();

        if (mLogLevel != null)
        {
            theLogger.setLevel (mLogLevel);
            Mama.enableLogging (mLogLevel);
        }

        theLogger.info( "Source: " + options.getSource() );
        try
        {
            // Initialize MAMA API
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
                MamdaSubscription aSubscription = new MamdaSubscription ();

                MamdaBookAtomicListener aBookListener = new MamdaBookAtomicListener();
                AtomicBookBuilder       aTicker       = new AtomicBookBuilder (symbol);

                aBookListener.addBookHandler         (aTicker);
                aBookListener.addLevelHandler        (aTicker);
                aBookListener.addLevelEntryHandler   (aTicker);
                aBookListener.setProcessMarketOrders (mShowMarketOrders);

                aSubscription.addMsgListener         (aBookListener);
                aSubscription.addStaleListener       (aTicker);
                aSubscription.addErrorListener       (aTicker);
                aSubscription.setType                (MamaSubscriptionType.BOOK);

                aSubscription.create (transport,
                                      mQueueGroup.getNextQueue(),
                                      options.getSource (),
                                      symbol,
                                      null);

                theLogger.info ("Subscribed to: " + symbol);
            }

            //Start dispatching on the default MAMA event queue
            Mama.start (mBridge);
            synchronized (MamdaAtomicBookBuilder.class)
            {
                MamdaAtomicBookBuilder.class.wait ();
            }

        }
        catch (Exception e)
        {
            e.printStackTrace ();
            System.exit (1);
        }
    }

    /* ***************************************************************************
     * AtomicBookBuilder class implements the required base classes to receive
     * callbacks from MamdaBookAtomicListener. These are:
     *      - MamdaBookAtomicBookHandler to receive book callbacks
     *      - MamdaBookAtomicLevelHandler to recieve level callbacks
     *      - MamdaBookAtomicLevelEntryHandler to receive entry callbacks
     *
     * As per MAMDA Developers guide, when consuming V5 entry updates the 
     *  following methods should not be accessed on the MamdaBookAtomicLevel
     *  interface:
     *      - getPriceLevelSize()
     *      - getPriceLevelSizeChange()
     *      - getPriceLevelAction()
     *      - getPriceLevelNumEntries()
     *
     * As per MAMDA Developers guide, when consuming V5 entry updates the 
     *  following methods should not be accessed on the MamdaBookAtomicLevelEntry
     *  interface:
     *      - getPriceLevelSize()
     *      - getPriceLevelAction()
     *      - getPriceLevelNumEntries()
     ****************************************************************************/
    private static class AtomicBookBuilder implements MamdaBookAtomicBookHandler,
                                                      MamdaBookAtomicLevelEntryHandler,
                                                      MamdaBookAtomicLevelHandler,
                                                      MamdaStaleListener,
                                                      MamdaErrorListener
    {
        DecimalFormat            df                  = new DecimalFormat("0.00");
        MamdaOrderBook           mOrderBook          = new MamdaOrderBook();
        MamdaOrderBookPriceLevel mPriceLevel         = null;
        MamdaOrderBookEntry      mEntry              = null;
        MamdaOrderBookPriceLevel mReusablePriceLevel = 
            new MamdaOrderBookPriceLevel (0.0, MamdaOrderBookPriceLevel.SIDE_UNKNOWN);
           
        /*
         * Helper function to apply a MamdaOrderBookPriceLevel to 
         * a MamdaOrderBook
         */
        private void applyLevel (MamdaOrderBookPriceLevel level) 
        {
            if(mOrderBook.getQuality() == MamaQuality.QUALITY_OK)
            {
                switch(level.getAction()) 
                {
                    case MamdaOrderBookPriceLevel.ACTION_UPDATE :
                        try
                        {
                            /*
                             * When in the order book the only Action which makes sense is
                             * ADD
                             */
                            level.setAction(MamdaOrderBookPriceLevel.ACTION_ADD);

                            mOrderBook.updateLevel(level); 
                        }   
                        catch ( MamdaOrderBookException e)
                        {       
                            // Exception is thrown when updating level which does not exist
                            // level will be added so handled internally
                        }       
                        break;

                    case MamdaOrderBookPriceLevel.ACTION_ADD :
                        try                 
                        {                   
                            mPriceLevel = new MamdaOrderBookPriceLevel(level);
                            /*
                             * When in the order book the only Action which makes sense is
                             * ADD
                             */
                            mPriceLevel.setAction(MamdaOrderBookPriceLevel.ACTION_ADD);

                            mOrderBook.addLevel(mPriceLevel);
                        }
                        catch ( MamdaOrderBookException e)
                        {
                            // Exception is thrown if adding a level already in book
                            // handled internally by updating level
                        }
                        break;

                    case MamdaOrderBookPriceLevel.ACTION_DELETE :
                        try
                        {
                            mOrderBook.deleteLevel(mReusablePriceLevel);
                        }
                        catch (MamdaOrderBookException e)
                        {
                            // Thrown if the level cannot be found in the book
                            // No need for handling as level is deleted
                        }
                        break;

                    default:
                        System.out.println("atomicbookbuilder: Unknown price level [" +
                                level.getAction() + "]");
                        break;
                }

                mPriceLevel = null;
            }
        }

        /*
         * Helper function a apply a MamdaBookAtomicLevelEntry to
         * a MamdaOrderBook
         */
        private void applyEntry (MamdaBookAtomicLevelEntry levelEntry)
        {
            char entryAction;

            if(mOrderBook.getQuality() == MamaQuality.QUALITY_OK)
            {
                entryAction = levelEntry.getPriceLevelEntryAction();

                switch(entryAction)
                {
                    case MamdaOrderBookEntry.ACTION_UPDATE :
                        try
                        {
                            // get the price level by price
                            mPriceLevel = mOrderBook.getLevelAtPrice(
                                    levelEntry.getPriceLevelPrice(),
                                    levelEntry.getPriceLevelSide());
                            if(mPriceLevel != null)
                            {
                                // get the entry by id
                                mEntry = mPriceLevel.findOrCreateEntry(levelEntry.getPriceLevelEntryId());

                                mOrderBook.updateEntry(
                                        mEntry,
                                        levelEntry.getPriceLevelEntrySize(),
                                        new MamaDateTime(levelEntry.getPriceLevelEntryTime()),
                                        null);
                                break;
                            }
                        }
                        catch(MamdaOrderBookException e)
                        {
                            System.out.println("atomicbookbuilder: could not update entry.");
                            System.out.println("Caught MamdaOrderBookException [" + e.getMessage() + "]");
                        }

                        //intentional fall through if level does not exist to add entry
                    case MamdaOrderBookEntry.ACTION_ADD :
                        mEntry = new MamdaOrderBookEntry(
                                levelEntry.getPriceLevelEntryId(),
                                levelEntry.getPriceLevelEntrySize(),
                                MamdaOrderBookEntry.ACTION_ADD,
                                new MamaDateTime (levelEntry.getPriceLevelEntryTime()),
                                null);
                        mEntry.setReason(levelEntry.getPriceLevelEntryReason());

                        mOrderBook.addEntry(
                                mEntry,
                                levelEntry.getPriceLevelPrice(),
                                levelEntry.getPriceLevelSide(),
                                new MamaDateTime (levelEntry.getPriceLevelEntryTime()),
                                null);
                        break;
                    case MamdaOrderBookEntry.ACTION_DELETE :
                        try
                        {
                            //get the price level by price
                            mPriceLevel = mOrderBook.getLevelAtPrice(
                                    levelEntry.getPriceLevelPrice(),
                                    levelEntry.getPriceLevelSide());
                            if(mPriceLevel != null)
                            {
                                mEntry = mPriceLevel.findOrCreateEntry(
                                        levelEntry.getPriceLevelEntryId());

                                mOrderBook.deleteEntry(
                                        mEntry,
                                        new MamaDateTime(levelEntry.getPriceLevelEntryTime()),
                                        null);
                            }
                        }
                        catch (MamdaOrderBookException e)
                        {
                            System.out.print ( "atomicbookbuilder: could not delete entry.\n");
                            System.out.print( "Caught MamdaOrderBookException [" + e.getMessage() + "]\n");

                        }
                        break;
                    default:
                        System.out.println( "atomicbookbuilder: Unknown entry action" + entryAction );
                        break;
                }

                mEntry      = null;
                mPriceLevel = null;
            }
        }

        public AtomicBookBuilder(
                String symbol)
        {
            mOrderBook.setSymbol  (symbol);
            mOrderBook.setQuality (MamaQuality.QUALITY_OK);
            mReusablePriceLevel.setTime (new MamaDateTime());
        }

        /*
         * Method invoked before we start processing the first level in a message.
         * The book should be cleared when isRecap == true.
         */
        public void onBookAtomicBeginBook (
            MamdaSubscription           subscription,
            MamdaBookAtomicListener     listener,
            boolean                     isRecap) 
    
        {
            System.out.println("BOOK BEGIN");

            if(isRecap)
                mOrderBook.clear();

            if (mQuietModeLevel<2)
            {
                if(isRecap)
                {
                    System.out.println ("RECAP!!!");
                }
                else
                {
                    System.out.println ("DELTA!!!");
                }
            }
        }

        /*
         * Method invoked when an order book is cleared.
         */
        public void onBookAtomicClear (
            MamdaSubscription           subscription,
            MamdaBookAtomicListener     listener,
            MamaMsg                     msg)
        {
            if (mQuietModeLevel<2)
            {
                System.out.println ("CLEAR!!!");
            }
            mOrderBook.clear();
        }
   
        /*
         * Method invoked when a gap in orderBook reports is discovered.
         */ 
        public void onBookAtomicGap (
            MamdaSubscription              subscription,
            MamdaBookAtomicListener        listener,
            MamaMsg                        msg,
            MamdaBookAtomicGap             event)
        {
            if (mQuietModeLevel<2)
            {
                System.out.println ( 
                        "Book gap for " + subscription.getSymbol () + 
                        " (" + listener.getBeginGapSeqNum () + "-" + 
                        listener.getEndGapSeqNum () + ")" );
            }
        }

        /*
         * Method invoked when we stop processing the last level in a message. We 
         * invoke this method after the last entry for the level gets processed. 
         * The subscription may be destroyed from this callback.
         */
        public void onBookAtomicEndBook (
            MamdaSubscription           subscription,
            MamdaBookAtomicListener     listener) 
        {
            /*
             * When level recap/delta is received, it is stored in a MamdaOrderBookPriceLevel.
             * If no entry deltas/recaps are received, then the price level should be applied
             *  to the book. 
             * The entry delta/recap callback will mark the price level with an UNKNOWN side to
             *  show that it does not need to be applied to the book
             */
            if (mReusablePriceLevel.getSide() != MamdaOrderBookPriceLevel.SIDE_UNKNOWN)
            {
                if (mOrderBook.getQuality() == MamaQuality.QUALITY_OK && mQuietModeLevel<2)
                {
                    prettyPrint (subscription, mReusablePriceLevel);
                }
                applyLevel(mReusablePriceLevel);
                mReusablePriceLevel.setSide (MamdaOrderBookPriceLevel.SIDE_UNKNOWN);
            }

            if (mOrderBook.getQuality() == MamaQuality.QUALITY_OK && mQuietModeLevel<2)
            {
                prettyPrint(mOrderBook);
            }

            System.out.println("BOOK END");
        }

        /*
         * Helper function to store a MamdaBookAtomicLevel in 
         * the resuabale MamdaOrderBookPriceLevel
         */
        private void storeLevel(MamdaBookAtomicLevel  level)
        {
            mReusablePriceLevel.clear    ();
            mReusablePriceLevel.setPrice (level.getPriceLevelPrice());
            mReusablePriceLevel.setSide  (level.getPriceLevelSide());
            mReusablePriceLevel.setTime  (new MamaDateTime(level.getPriceLevelTime()));

            /*
             * As per the MAMDA Developers Guide, the following three accessors on a 
             *  MamdaBookAtomicLevel object should not be used for V5 entry updates. 
             *  Here, these calls are used and the resulting MamdaOrderBookPriceLevel 
             *  is only used when the callbacks received indicate that the update was 
             *  not a V5 entry update.
             */
            mReusablePriceLevel.setSize       (level.getPriceLevelSize());
            mReusablePriceLevel.setAction     (level.getPriceLevelAction());
            mReusablePriceLevel.setNumEntries (level.getPriceLevelNumEntries());
        }

        /*
         * Method invoked when a full refresh of the order book for the
         * security is available.  The reason for the invocation may be
         * any of the following:
         * - Initial image.
         * - Recap update (e.g., after server fault tolerant event or data
         *   quality event.)
         * - After stale status removed.
         */
        public void onBookAtomicLevelRecap (
            MamdaSubscription             subscription,
            MamdaBookAtomicListener       listener,
            MamaMsg                       msg,
            MamdaBookAtomicLevel          level)
        {       
            /*
             * The level should only be processed when entires are not received
             *  i.e. for level only based feeds
             * If an entry was received on the previous level, then the level will
             *  have been marked with an UNKNOWN side and should not be applied to 
             *  the book
             */
            if(mReusablePriceLevel.getSide() != MamdaOrderBookPriceLevel.SIDE_UNKNOWN)
            {
                applyLevel(mReusablePriceLevel);
            }

            /*
             * Store the current level
             */
            storeLevel(level);
        }

        /*
         * Method invoked when an order book delta is reported.
         */
        public void onBookAtomicLevelDelta (
            MamdaSubscription               subscription,
            MamdaBookAtomicListener         listener,
            MamaMsg                         msg,
            MamdaBookAtomicLevel            level)
        {   
            /*
             * The level should only be processed when entires are not received
             *  i.e. for level only based feeds
             * If an entry was received on the previous level, then the level will
             *  have been marked with an UNKNOWN side and should not be applied to 
             *  the book
             */
            if (mReusablePriceLevel.getSide() != MamdaOrderBookPriceLevel.SIDE_UNKNOWN)
            {
                if (mQuietModeLevel<2 && mOrderBook.getQuality() == MamaQuality.QUALITY_OK)
                {
                    prettyPrint(subscription, mReusablePriceLevel);
                }
                applyLevel(mReusablePriceLevel);
            }

            /*
             * Store the current level
             */
            storeLevel(level);
        }

        /*
         * Method invoked when a full refresh of the order book for the
         * security is available.  The reason for the invocation may be
         * any of the following:
         * - Initial image.
         * - Recap update (e.g., after server fault tolerant event or data
         *   quality event.)
         * - After stale status removed.
         */
        public void onBookAtomicLevelEntryRecap (
            MamdaSubscription             subscription,
            MamdaBookAtomicListener       listener,
            MamaMsg                       msg,
            MamdaBookAtomicLevelEntry     levelEntry)
        {
            applyEntry(levelEntry);

            /*
             * An entry has been processed on the level so mark the level with
             *  an unknown side so that it will not be applied to book
             */
            mReusablePriceLevel.setSide(MamdaOrderBookPriceLevel.SIDE_UNKNOWN);
        }

        /*
         * Method invoked when an order book delta is reported.
         */
        public void onBookAtomicLevelEntryDelta (
            MamdaSubscription               subscription,
            MamdaBookAtomicListener         listener,
            MamaMsg                         msg,
            MamdaBookAtomicLevelEntry       levelEntry)
        {
            if (mQuietModeLevel<2 && mOrderBook.getQuality() == MamaQuality.QUALITY_OK)
            {
                prettyPrint (subscription, levelEntry);
            }
            applyEntry(levelEntry);

            /*
             * An entry has been processed on the level so mark the level with
             *  an unknown side so that it will not be applied to book
             */
            mReusablePriceLevel.setSide(MamdaOrderBookPriceLevel.SIDE_UNKNOWN);
        }
    
        public void onError (
            MamdaSubscription   subscription,
            short               severity,
            short               errorCode,
            String              errorStr)
        {
            if (mQuietModeLevel<2)
            {
                theLogger.info ("Error (" + subscription.getSymbol() + "): "
                                + errorStr);
            }
        }
    
        public void onStale (
            MamdaSubscription   subscription,
            short               quality)
        {
            theLogger.info ("Stale (" + subscription.getSymbol() + ")");
            mOrderBook.setQuality(quality);
        }
    
        public void setShowMarketOrders (boolean showMarketOrders)
        {
            mShowMarketOrders = showMarketOrders;
        }

        /*
         * Helper function to print MamdaBookAtomicLevelEntry
         */
        private void prettyPrint (MamdaSubscription subscription, 
                        MamdaBookAtomicLevelEntry levelEntry)
        {
            String  symbol        = subscription.getSymbol ();
            String  time          = levelEntry.getPriceLevelEntryTime().getTimeAsString();
            String  entryId       = levelEntry.getPriceLevelEntryId ();
            char    entryAction   = levelEntry.getPriceLevelEntryAction ();
            double  price         = levelEntry.getPriceLevelPrice ();
            char    side          = levelEntry.getPriceLevelSide ();
            int     size          = (int) levelEntry.getPriceLevelEntrySize ();
            
            System.out.println ( 
                    "ENTRY " +
                    symbol + " " + 
                    time + " " + 
                    entryId + " " + 
                    entryAction + " " + 
                    df.format(price) + " " + 
                    side + " " + 
                    size );
        }

        /*
         * Helper function to print MamdaOrderBookPriceLevel
         */
        private void prettyPrint (MamdaSubscription subscription, 
                        MamdaOrderBookPriceLevel level)
        {
            String  symbol     = subscription.getSymbol ();
            String  time       = level.getTime().getTimeAsString();
            char    action     = level.getAction ();
            double  price      = level.getPrice ().getValue();
            char    side       = level.getSide ();
            int     size       = (int)level.getSize ();
            
            System.out.println ( 
                    "LEVEL " +
                    symbol + " " + 
                    time + " " + 
                    action + " " + 
                    df.format(price) + " " + 
                    side + " " + 
                    size );
        }       

        /* 
         * Helper function to print a MamdaOrderBook based on
         *  configuration
         */
        public void prettyPrint (final MamdaOrderBook  book)
        {
            if (mPrintEntries)
            {
                prettyPrintEntries (book);
            }
            else
            {
                prettyPrintLevels (book);
            }
        }

        /*
         * Helper function to print the levels in a MamdaOrderBook
         */
        public void prettyPrintLevels (final MamdaOrderBook  book)
        {
            if (mQuietModeLevel<2)
            {
                System.out.println ("Book for: " + book.getSymbol ());
            }
            if (mQuietModeLevel<1)
            {
                System.out.println ( "        Time     Num    Size   Price   Act | Act Price     Size     Num         Time");
            }

            if (mShowMarketOrders)
            {
                System.out.print("   MARKET ORDERS ---------------------------------------------------------------------\n");
                MamdaOrderBookPriceLevel marketOrders = null;

                if ((marketOrders = book.getBidMarketOrders()) != null)
                {
                    System.out.print ("   ");
                    paddedPrint (marketOrders.getTime ().getTimeAsString (), 12,
                                 false);
                    System.out.print (" ");
                    paddedPrint (String.valueOf((long)marketOrders.getNumEntries ()),
                                 4, false);
                    System.out.print (" ");
                    paddedPrint (String.valueOf((long)marketOrders.getSize ()),
                                 7, false);
                    System.out.print (" ");
                    System.out.print ("   MARKET");
                    System.out.print ("  ");
                    System.out.print (marketOrders.getAction ());
                    System.out.print ("  ");
                    System.out.print ("|");
                }
                else
                {
                    if (mQuietModeLevel<1)
                        System.out.print (
                                        "                                           ");
                    if (mQuietModeLevel<1)
                        System.out.print ("|");
                }


                if ((marketOrders = book.getAskMarketOrders()) != null)
                {
                    System.out.print ("  ");
                    System.out.print (marketOrders.getAction ());
                    System.out.print ("  ");
                    System.out.print ("MARKET   ");
                    System.out.print (" ");
                    paddedPrint (String.valueOf((long)marketOrders.getSize ()),
                                 7, true);
                    System.out.print (" ");
                    paddedPrint (String.valueOf((long)marketOrders.getNumEntries ()),
                                 6, true);
                }
            }
            if (mQuietModeLevel<1)
            {
                System.out.println ("");
                System.out.print ("   LIMIT ORDERS  ---------------------------------------------------------------------\n");
            }

            Iterator bidIter = book.bidIterator ();
            Iterator askIter = book.askIterator ();

            while (bidIter.hasNext () || askIter.hasNext ())
            {
                if (bidIter.hasNext ())
                {
                    MamdaOrderBookPriceLevel bidLevel =
                        (MamdaOrderBookPriceLevel) bidIter.next ();
                    if (mQuietModeLevel<1)
                    {
                        System.out.print ("   ");
                        paddedPrint (bidLevel.getTime ().getTimeAsString (), 12,
                                     false);
                        System.out.print (" ");
                        paddedPrint (String.valueOf((long)bidLevel.getNumEntries ()),
                                     4, false);
                        System.out.print (" ");
                        paddedPrint (String.valueOf((long)bidLevel.getSize ()),
                                     7, false);
                        System.out.print (" ");
                        paddedPrint (bidLevel.getPrice ().getValue (), 9, mPrecision,
                                     false);
                        System.out.print ("  ");
                        System.out.print (bidLevel.getAction ());
                        System.out.print ("  ");
                    }
                }
                else
                {
                    if (mQuietModeLevel<1)
                        System.out.print (
                            "                                           ");
                }
                if (mQuietModeLevel<1)
                    System.out.print ("|");
                if (askIter.hasNext ())
                {
                    MamdaOrderBookPriceLevel askLevel =
                        (MamdaOrderBookPriceLevel) askIter.next ();
                    if (mQuietModeLevel<1)
                    {
                        System.out.print ("  ");
                        System.out.print (askLevel.getAction ());
                        System.out.print ("  ");
                        paddedPrint (askLevel.getPrice ().getValue (), 9, mPrecision,
                                     true);
                        System.out.print (" ");
                        paddedPrint (String.valueOf ((long)askLevel.getSize ()),
                                     7, true);
                        System.out.print (" ");
                        paddedPrint (String.valueOf ((long)askLevel.getNumEntries ()),
                                     6, true);
                        System.out.print (" ");
                        paddedPrint (askLevel.getTime ().getTimeAsString (), 12,
                                     true);
                    }
                }
                if (mQuietModeLevel<1)
                  System.out.println ("");
            }

            if (mQuietModeLevel<1)
                System.out.println ("");
        }

        /*
         * Helper function to print the entries in a MamdaOrderBook
         */
        public void prettyPrintEntries (final MamdaOrderBook book)
        {
            if (mQuietModeLevel<2)
            {
                System.out.println ("Book for: " + book.getSymbol ());
            }
            if (mQuietModeLevel<1)
            {
                System.out.println ("     ID/Num           Time       Size   Price");
            }

            if (mQuietModeLevel<1)
            {
                if (mShowMarketOrders)
                {
                    MamdaOrderBookPriceLevel marketBidLevel = book.getBidMarketOrders();

                    if (marketBidLevel != null)
                    {
                        prettyPrintEntriesForLevel (marketBidLevel, "BID");
                    }

                    MamdaOrderBookPriceLevel marketAskLevel = book.getAskMarketOrders();

                    if (marketAskLevel != null)
                    {
                        prettyPrintEntriesForLevel (marketAskLevel, "ASK");
                    }
                }
            }

            Iterator bidIter = book.bidIterator ();
            Iterator askIter = book.askIterator ();

            while (bidIter.hasNext ())
            {
                MamdaOrderBookPriceLevel bidLevel =
                    (MamdaOrderBookPriceLevel) bidIter.next ();
                prettyPrintEntriesForLevel (bidLevel, "BID");
            }

            while (askIter.hasNext ())
            {
                MamdaOrderBookPriceLevel askLevel =
                    (MamdaOrderBookPriceLevel) askIter.next ();
                prettyPrintEntriesForLevel (askLevel, "ASK");
            }
        }

        /*
         * Helper function to print the entries for a price level
         */
        private void prettyPrintEntriesForLevel (MamdaOrderBookPriceLevel level,
                                                 String                   side)
        {
            if (mQuietModeLevel<1)
            {
                System.out.print ("  " + side + "  ");
                paddedPrint (String.valueOf((long)level.getNumEntries ()), 4,
                             false);
                System.out.print ("       ");
                paddedPrint (level.getTime ().getTimeAsString (), 12, false);
                System.out.print (" ");
                paddedPrint (String.valueOf ((long)level.getSize ()), 7,
                             false);
                System.out.print (" ");
                if (level.getOrderType() == MamdaOrderBookPriceLevel.LEVEL_LIMIT)
                    paddedPrint (level.getPrice ().getValue (), 9, mPrecision, false);
                else System.out.print("  MARKET");
                System.out.println ("");
            }

            Iterator i = level.entryIterator ();
            while (i.hasNext ())
            {
                MamdaOrderBookEntry entry = (MamdaOrderBookEntry) i.next ();
                String              id    = entry.getId ();
                double              size  = entry.getSize ();
                double              price = level.getPrice ().getValue ();

                if (mQuietModeLevel<1)
                {
                    System.out.print ("  ");
                    paddedPrint (id, 18, false);
                    System.out.print ("  ");
                    paddedPrint (entry.getTime().getTimeAsString (), 12, false);
                    System.out.print (" ");
                    paddedPrint (String.valueOf ((long)size), 7, false);
                    System.out.print (" ");
                    if (level.getOrderType() == MamdaOrderBookPriceLevel.LEVEL_LIMIT)
                        paddedPrint (price, 9, mPrecision, false);
                    else System.out.print("  MARKET");
                    System.out.println ("");
                }
            }
        }


        private static final String [] sPadding = new String[128];
        static
        {
            String s = "";
            for (int i = 0; i < sPadding.length; i++)
            {
                sPadding[i] = s;
                s += " ";
            }
        }

        private void paddedPrint (double  value, 
                                  int     width, 
                                  int     prec,
                                  boolean padAfter)
        {
            paddedPrint (new BigDecimal (value).setScale (prec,BigDecimal.ROUND_HALF_UP).toString (),
                         width, padAfter);

        }

        private void paddedPrint (String  val, 
                                  int     padLen, 
                                  boolean padAfter)
        {
            int len = val.length ();
            if (len > padLen)
            {
                System.out.print (val.substring (0, padLen));
            }
            else
            {
                if (padAfter)
                {
                    System.out.print (val);
                    System.out.print (sPadding[padLen - len]);
                }
                else
                {
                    System.out.print (sPadding[padLen - len]);
                    System.out.print (val);
                }
            }
        }
    }
}
