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

/**
 * The MamdaBookTicker example shows how to make a subscription for an
 * order book (similar to trades & quotes). In the callback, one gets
 * access to the complete book after the delta has been applied.  The
 * example prints the entire book upon receipt of a recap/initial and
 * just the top-10 for a delta (although the full book is readily
 * available).
 */

public class MamdaBookTicker
{
    private static Logger   theLogger   = 
       Logger.getLogger( "com.wombat.mamda.examples" );
    /*Controls the logging within the API*/
    private static Level                    mLogLevel               = Level.INFO;
    /*Controls what gets printed in the example program*/
    private static int                      mQuietModeLevel         = 0;
    private static int                      mPrecision              = 0;
    private static boolean                  mShowDeltas             = false;
    private static boolean                  mShowMarketOrders       = false;
    private static MamaSource               mMamaSource;
    private static boolean                  mPrintEntries           = false;
    private static MamaBridge               mBridge                 = null;
    private static MamaQueueGroup           mQueueGroup             = null;
    private static String                   mDictTransportName      = null;
    
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
        CommandLineProcessor options      = new CommandLineProcessor (args);
        mQuietModeLevel = options.getQuietModeLevel ();
        mLogLevel       = options.getLogLevel ();
        mPrecision      = options.getPrecision();
        
        if (mLogLevel != null)
        {
            theLogger.setLevel (mLogLevel);
            Mama.enableLogging (mLogLevel);
        }

        theLogger.info ( "Source: " + options.getSource() );
        mShowDeltas         = options.showDeltas();
        mPrintEntries       = options.getPrintEntries();
        mShowMarketOrders   = options.showMarketOrders();

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
                 iterator.hasNext();
                )
            {
                final String symbol = (String)iterator.next ();
                MamdaSubscription  aSubscription =
                    new MamdaSubscription ();
                MamdaOrderBookListener aBookListener;
                
                aBookListener =  new MamdaOrderBookListener ();

                /*Whether to process and subsequently print entry details*/
                aBookListener.setProcessEntries (mPrintEntries);
                aBookListener.setProcessMarketOrders(mShowMarketOrders);

                BookTicker aTicker = new BookTicker ();

                aBookListener.addHandler       (aTicker);
                aSubscription.addMsgListener   (aBookListener);
                aSubscription.addStaleListener (aTicker);
                aSubscription.addErrorListener (aTicker);

                aSubscription.setType (MamaSubscriptionType.BOOK);
                
                aSubscription.create (transport,
                                      mQueueGroup.getNextQueue(),
                                      options.getSource (),
                                      symbol,
                                      null);

                theLogger.info ("Subscribed to: " + symbol);
            }

            //Start dispatching on the default MAMA event queue
            Mama.start (mBridge);
            synchronized (MamdaBookTicker.class)
            {
                MamdaBookTicker.class.wait ();
            }
        }
        catch (Exception e)
        {
            e.printStackTrace ();
            System.exit (1);
        }
    }

    /*Handler class which receives update callbacks*/
    private static class BookTicker implements MamdaOrderBookHandler,
                                               MamdaStaleListener,
                                               MamdaErrorListener
    {
        public void onBookDelta (MamdaSubscription         sub,
                                 MamdaOrderBookListener    listener,
                                 MamaMsg                   msg,
                                 MamdaOrderBookSimpleDelta delta,
                                 MamdaOrderBook            book)
        {
            long seqNum = (msg != null) ? msg.getSeqNum() : 0;

            if (mQuietModeLevel<2)
            {
                System.out.print ("DELTA!!!  (seq# " + seqNum + ")\n");
            }
            MamdaOrderBookEntry entr = delta.getEntry ();
            if (entr != null)
            {
                if (mQuietModeLevel<1)
                    System.out.print (
                          ' ' + (entr.getTime()).getAsString() +
                          ' ' + entr.getId() +
                          ' ' + (entr.getUniqueId () == null ? " "
                                 : entr.getUniqueId ())  +
                          ' ' + delta.getEntryDeltaAction() +
                          ' ' + entr.getPrice() +
                          ' ' + entr.getSide() +
                          ' ' + entr.getSize () + '\n');
            }

            if (mShowDeltas)
            {
                prettyPrint (delta);
            }
            prettyPrint (book);
        }

        public void onBookComplexDelta (MamdaSubscription          subscription,
                                        MamdaOrderBookListener     listener,
                                        MamaMsg                    msg,
                                        MamdaOrderBookComplexDelta delta,
                                        MamdaOrderBook             book)
        {
            long seqNum = (msg != null) ? msg.getSeqNum() : 0;

            if (mQuietModeLevel<2)
            {
                System.out.println ("COMPLEX DELTA!!!  (seq# " + seqNum + ")");
            }

            if (mShowDeltas)
            {
                Iterator i = delta.iterator ();
                while (i.hasNext ())
                {
                    MamdaOrderBookBasicDelta  basicDelta =
                        (MamdaOrderBookBasicDelta) i.next ();
                    prettyPrint (basicDelta);
                }
            }
            prettyPrint (book);

        }

        /**
         * Method invoked when an order book is cleared.
         *
         * @param subscription The subscription which received the update.
         * @param listener     The listener which invoked this callback.
         * @param msg          The MamaMsg that triggered this invocation.
         * @param event        The clear event.
         */
        public void onBookClear (MamdaSubscription      subscription,
                                 MamdaOrderBookListener listener,
                                 MamaMsg                msg,
                                 MamdaOrderBookClear    event,
                                 MamdaOrderBook         book)
        {
            long seqNum = (msg!=null) ? msg.getSeqNum() : 0;
            
            if (mQuietModeLevel<2)
            {
                System.out.println ("CLEAR!!!  (seq# " + seqNum + ")");
            }

            prettyPrint (book);
        }

        public void onBookRecap (MamdaSubscription          sub,
                                 MamdaOrderBookListener     listener, 
                                 MamaMsg                    msg,
                                 MamdaOrderBookComplexDelta delta,
                                 MamdaOrderBookRecap        event,
                                 MamdaOrderBook             book)
        {
            long seqNum = (msg != null) ? msg.getSeqNum() : 0;
            
            if (mQuietModeLevel < 2)
            {
                System.out.print ("RECAP!!!  (seq# " + seqNum + ")\n");
            }
            prettyPrint (book);
        }


        public void onBookGap (MamdaSubscription      sub,
                               MamdaOrderBookListener listener,
                               MamaMsg                msg,
                               MamdaOrderBookGap      event,
                               MamdaOrderBook         fullBook)
        {            
            theLogger.info("\nReceived book gap " + sub.getSymbol ()
                    + " Start: " + event.getBeginGapSeqNum ()
                    + " End: "   + event.getEndGapSeqNum ());
        }

        public void onStale (MamdaSubscription   subscription,
                             short               quality)
        {
            theLogger.info("Stale (" + subscription.getSymbol() + ")");
        }

        public void onError (MamdaSubscription   subscription,
                             short               severity,
                             short               errorCode,
                             String              errorStr)
        {
            theLogger.info("Error (" + subscription.getSymbol() + "): "
                           + errorStr);
        }

        public void setShowMarketOrders (boolean showMarketOrders)
        {
            mShowMarketOrders = showMarketOrders;
        }
            
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

        private void prettyPrint (MamdaOrderBookBasicDelta event)
        {
            MamdaOrderBookEntry entry = event.getEntry();
        
            MamdaOrderBookPriceLevel  level  = event.getPriceLevel ();
            double                    size   = event.getPlDeltaSize ();
            char                      action = event.getPlDeltaAction();

            if (MamdaOrderBookPriceLevel.LEVEL_LIMIT == level.getOrderType())
            {
                System.out.print ("   ");
                paddedPrint (String.valueOf((long)size),
                             7, false);
                System.out.print ("   ");
                paddedPrint (level.getPrice().getValue (), 9, mPrecision, false);
                System.out.print ("  ");
                System.out.print (action);
                System.out.print ("  \n");
            }
            else
            {
                System.out.print ("   ");
                paddedPrint (String.valueOf((long)size),
                             7, false);
                System.out.print ("   ");
                System.out.print(" MARKET ");
                System.out.print ("  ");
                System.out.print (action);
                System.out.print ("  \n");
            }
        }

        public void prettyPrintLevels (final MamdaOrderBook  book)
        {
            if (mQuietModeLevel < 2)
            {
                System.out.println ("Book for: " + book.getSymbol ());
            }
            if (mQuietModeLevel < 1)
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
                    System.out.print (" ");
                    paddedPrint (marketOrders.getTime ().getTimeAsString (), 12,
                                 true);
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
                String      id    = entry.getId ();
                double      size  = entry.getSize ();
                double      price = level.getPrice ().getValue ();
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

        private void paddedPrint (String val, int padLen, boolean padAfter)
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
