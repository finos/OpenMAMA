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
import java.util.Date;
import java.util.Iterator;
import com.wombat.mamda.*;
import com.wombat.mamda.options.*;
import com.wombat.mama.*;

/**
 * An example of MAMDA option chain processing.
 *
 * <p>This example suffers from a minor lack of configurability: while
 * it does support subscriptions to multiple underlying symbols (with
 * multiple -s options), it does not allow a different source to be
 * specified for each option.</p>
 */

public class MamdaOptionChainExample
{
    private static Logger theLogger = Logger.getLogger( "com.wombat.mamda.examples" );
    private static MamaSource  mMamaSource;
    private static MamaBridge  mBridge              = null;
    private static String      mDictTransportName   = null;
    private static Date        nullDate             = new Date(0,0,0);
    
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
            mMamaSource.setTransport(transport);
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
        MamaTransport        baseTransport    = null;
        MamaTransport        optionTransport  = null;
        MamaTransport        dictTransport    = null;
        MamaDictionary       dictionary       = null;
        CommandLineProcessor options          = new CommandLineProcessor (args);

        Level level = options.getLogLevel ();
        if (level != null)
        {
            theLogger.setLevel (level);
            Mama.enableLogging (level);
        }
        theLogger.info ("Source: " + options.getSource ());

        try
        {
            // Initialize MAMA
            mBridge = options.getBridge();
            Mama.open ();

            // Initialize transports.  We're assuming that options and
            // underlyings might be on different transports.  Note:
            // some companies might use multiple transports for
            // underlyings (e.g., CTA and NASDAQ), which case it woud
            // be necessary to create three transports here and be
            // sure to pass the correct transport later.
            baseTransport = new MamaTransport ();
            baseTransport.create (options.getTransport (), mBridge);
            optionTransport = baseTransport;
            mMamaSource = new MamaSource ();

            mDictTransportName = options.getDictTransport();

            if (mDictTransportName != null)
            {
                dictTransport = new MamaTransport ();
                dictTransport.create (mDictTransportName, mBridge);
            }
            else 
            {
                dictTransport = baseTransport;
            }

            // Fetch and initialize the data dictionary
            dictionary = buildDataDictionary     (dictTransport, options.getDictSource());
            MamdaQuoteFields.setDictionary       (dictionary, null);
            MamdaTradeFields.setDictionary       (dictionary, null);
            MamdaFundamentalFields.setDictionary (dictionary, null);
            MamdaOptionFields.setDictionary      (dictionary, null);
            MamdaCommonFields.setDictionary      (dictionary, null);

            // Create listeners for each chain.  Two subscriptions are
            // necessary.
            for (Iterator iterator = options.getSymbolList ().iterator ();
                 iterator.hasNext ();
                )
            {
                final String symbol = (String)iterator.next ();

                // Create chain and listener objects.
                MamdaTradeListener aBaseTradeListener = new MamdaTradeListener ();
                MamdaQuoteListener aBaseQuoteListener = new MamdaQuoteListener ();
                MamdaOptionChain   anOptionChain      = new MamdaOptionChain (symbol);

                anOptionChain.setUnderlyingQuoteListener (aBaseQuoteListener);
                anOptionChain.setUnderlyingTradeListener (aBaseTradeListener);

                MamdaOptionChainListener  anOptionListener =
                    new MamdaOptionChainListener (anOptionChain);

                // Create our handlers (the UnderlyingTicker and
                // OptionChainDisplay could be a single class).
                UnderlyingTicker    aBaseTicker = new UnderlyingTicker (anOptionChain);
                OptionChainDisplay  aDisplay    = new OptionChainDisplay (anOptionChain);

                // Create subscriptions for underlying and option chain:
                MamdaSubscription  aBaseSubscription    = new MamdaSubscription ();
                MamdaSubscription  anOptionSubscription = new MamdaSubscription ();

                // Register for underlying quote and trade events.
                aBaseTradeListener.addHandler (aBaseTicker);
                aBaseQuoteListener.addHandler (aBaseTicker);
                aBaseSubscription.addMsgListener (aBaseTradeListener);
                aBaseSubscription.addMsgListener (aBaseQuoteListener);
                
                aBaseSubscription.create (baseTransport,
                                          Mama.getDefaultQueue (mBridge), 
                                          options.getSource (),
                                          symbol,
                                          null);

                // Register for underlying option events.
                anOptionListener.addHandler (aDisplay);

                // We set the timeout to 1 for this example because we
                // currently use the timeout feature to determine when
                // to say that we have received all of the initials.
                anOptionSubscription.setTimeout       (1);
                anOptionSubscription.addMsgListener   (anOptionListener);
                anOptionSubscription.addStaleListener (aDisplay);
                anOptionSubscription.addErrorListener (aDisplay);
                anOptionSubscription.setType          (MamaSubscriptionType.GROUP);

                anOptionSubscription.create (optionTransport,
                                             Mama.getDefaultQueue (mBridge),                       
                                             options.getSource(),
                                             symbol,
                                             null);

                theLogger.fine ("Subscribed to: " + symbol);
            }
           
            //Start dispatching on the default event queue
            Mama.start (mBridge);
            synchronized (MamdaOptionChainListener.class)
            {
                MamdaOptionChainListener.class.wait ();
            }
        }
        catch (Exception e)
        {
            e.printStackTrace ();
            System.exit (1);
        }
    }

    private static class OptionChainDisplay implements MamdaOptionChainHandler,
                                                       MamdaStaleListener,
                                                       MamdaErrorListener 
    {
        MamdaOptionChain  mChain    = null;
        boolean           mGotRecap = false;  // Updated when we get the first recap.

        public OptionChainDisplay (MamdaOptionChain  chain)
        {
            mChain = chain;
        }

        public void onOptionChainRecap (
            final MamdaSubscription         subscription,
            final MamdaOptionChainListener  listener,
            final MamaMsg                   msg,
            final MamdaOptionChain          chain)
        {
            mGotRecap = true;
            printChain (chain);
        }

        public void onOptionSeriesUpdate (
            final MamdaSubscription         subscription,
            final MamdaOptionChainListener  listener,
            final MamaMsg                   msg,
            final MamdaOptionSeriesUpdate   event,
            final MamdaOptionChain          chain)
        {
            printChain (chain);
        }

        public void onOptionChainGap (
            final MamdaSubscription         subscription,
            final MamdaOptionChainListener  listener,
            final MamaMsg                   msg,
            final MamdaOptionChain          chain)
        {
            printChain (chain);
        }

        public void onOptionContractCreate (
            final MamdaSubscription         subscription,
            final MamdaOptionChainListener  listener,
            final MamaMsg                   msg,
            final MamdaOptionContract       contract,
            final MamdaOptionChain          chain)
        {

            // (Optional: create a trade/quote handler for the
            // individual option contract.)
            OptionContractTicker aTicker = 
                new OptionContractTicker (contract, chain, this);
            contract.addQuoteHandler (aTicker);
            contract.addTradeHandler (aTicker);
        }

        public void onStale (
            MamdaSubscription   subscription,
            short               quality)
        {
            System.out.println ("Stale (" + subscription.getSymbol () + ")");
        }

        public void onError (
            MamdaSubscription   subscription,
            short               severity,
            short               errorCode,
            String              errorStr)
        {
            System.out.println ("Error (" + subscription.getSymbol() + ")");
        }

        public void printChain (MamdaOptionChain  chain)
        {
            if (!mGotRecap)
                return;

            int      i        = 0;
            Iterator callIter = chain.callIterator ();
            Iterator putIter  = chain.putIterator ();
            System.out.println ("Chain: " + chain.getSymbol ());

            while (callIter.hasNext () || putIter.hasNext ())
            {
                System.out.print ("" + i + " | ");
                if (callIter.hasNext ())
                {
                    MamdaOptionContract callContract =
                        (MamdaOptionContract)callIter.next ();
                    printContract (callContract);
                }
                else
                {
                    System.out.print ("          ");
                }

                System.out.print (" | ");

                if (putIter.hasNext ())
                {
                    MamdaOptionContract putContract =
                        (MamdaOptionContract)putIter.next ();
                    printContract (putContract);
                }
                else
                {
                }
                System.out.println ();
                ++i;
            }
        }

        public void printContract (MamdaOptionContract  contract)
        {
            if (!mGotRecap)
                return;

            MamdaTradeRecap tradeRecap = contract.getTradeInfo   ();
            MamdaQuoteRecap quoteRecap = contract.getQuoteInfo   ();
            String symbol              = contract.getSymbol      ();
            String exchange            = contract.getExchange    ();
            Date   expireDate          = contract.getExpireDate  ();
            double strikePrice         = contract.getStrikePrice ();
            double lastPrice           = tradeRecap.getLastPrice ().getValue();
            double accVolume           = tradeRecap.getAccVolume ();
            double bidPrice            = quoteRecap.getBidPrice  ().getValue();
            double askPrice            = quoteRecap.getAskPrice  ().getValue();

            // The new java.util.Formatter class (JS2E 5.0) would be handy here!
            if (!expireDate.equals(nullDate))
            {
                System.out.print (expireDate + " "   + strikePrice + " ("    +
                                  symbol     + " "   + exchange    + ")  | " +
                                  lastPrice  + " | " + bidPrice    + " | "   +
                                  askPrice   + " | " + accVolume);
            }
            else
            {
                System.out.print ("expireDate(NULL) " + strikePrice + " ("    +
                                  symbol    + " "     + exchange    + ")  | " +
                                  lastPrice + " | "   + bidPrice    + " | "   +
                                  askPrice  + " | "   + accVolume);
            }
        }

        public void printlnContract (MamdaOptionContract  contract)
        {
            if (!mGotRecap)
                return;
            printContract (contract);
            System.out.println ();
        }
    }

    /**
     * OptionContractTicker is a class that we can use to handle
     * updates to individual option contracts.  This example just
     * prints individual trades and quotes.  Instances of this class
     * are associated with actual option contracts in the
     * OptionChainDisplay.onOptionSeriesUpdate method, above.  The
     * trade and quote information of the underlying is accessible via
     * the chain argument.
     */
    private static class OptionContractTicker implements MamdaTradeHandler, 
                                                         MamdaQuoteHandler
    {
        MamdaOptionContract mContract = null;
        MamdaOptionChain    mChain    = null;
        OptionChainDisplay  mDisplay  = null;

        public OptionContractTicker (MamdaOptionContract contract,
                                     MamdaOptionChain    chain,
                                     OptionChainDisplay  display)
        {
            mContract            = contract;
            mChain               = chain;
            mDisplay             = display;
        }

        public void onTradeRecap (
            final MamdaSubscription   sub,
            final MamdaTradeListener  listener,
            final MamaMsg             msg,
            final MamdaTradeRecap     recap)
        {
            // Refresh the "display" of this contract.
            mDisplay.printlnContract (mContract);
        }

        public void onTradeReport (
            final MamdaSubscription   sub,
            final MamdaTradeListener  listener,
            final MamaMsg             msg,
            final MamdaTradeReport    trade,
            final MamdaTradeRecap     recap)
        {
            // Do something with a specific trade report.
            mDisplay.printlnContract (mContract);
        }

        public void onTradeGap (
            final MamdaSubscription   sub,
            final MamdaTradeListener  listener,
            final MamaMsg             msg,
            final MamdaTradeGap       event,
            final MamdaTradeRecap     recap)
        {
        }

        public void onTradeCancelOrError (
            final MamdaSubscription        sub,
            final MamdaTradeListener       listener,
            final MamaMsg                  msg,
            final MamdaTradeCancelOrError  event,
            final MamdaTradeRecap          recap)
        {
        }

        public void onTradeCorrection (
            final MamdaSubscription        sub,
            final MamdaTradeListener       listener,
            final MamaMsg                  msg,
            final MamdaTradeCorrection     event,
            final MamdaTradeRecap          recap)
        {
        }

        public void onTradeClosing (
            final MamdaSubscription        sub,
            final MamdaTradeListener       listener,
            final MamaMsg                  msg,
            final MamdaTradeClosing        event,
            final MamdaTradeRecap          recap)
        {
        }

        public void onQuoteRecap (
            final MamdaSubscription   sub,
            final MamdaQuoteListener  listener,
            final MamaMsg             msg,
            final MamdaQuoteRecap     recap)
        {
            // Refresh the "display" of this contract.
            mDisplay.printlnContract (mContract);
        }

        public void onQuoteUpdate (
            final MamdaSubscription   sub,
            final MamdaQuoteListener  listener,
            final MamaMsg             msg,
            final MamdaQuoteUpdate    event,
            final MamdaQuoteRecap     recap)
        {
            // Do something with a specific quote.
            mDisplay.printlnContract (mContract);
        }

        public void onQuoteGap (
            final MamdaSubscription   sub,
            final MamdaQuoteListener  listener,
            final MamaMsg             msg,
            final MamdaQuoteGap       event,
            final MamdaQuoteRecap     recap)
        {
        }

        public void onQuoteClosing (
            final MamdaSubscription   sub,
            final MamdaQuoteListener  listener,
            final MamaMsg             msg,
            final MamdaQuoteClosing   event,
            final MamdaQuoteRecap     recap)
        {
        }
    }

    /**
     * UnderlyingTicker is a class that we can use to handle updates
     * to the underlying security.  This example just prints
     * individual trades and quotes.
     */
    private static class UnderlyingTicker implements MamdaTradeHandler, 
                                                     MamdaQuoteHandler
    {
        private MamdaOptionChain mChain = null;

        UnderlyingTicker (MamdaOptionChain  chain)
        {
            mChain = chain;
        }

        public void onTradeRecap (
            final MamdaSubscription   sub,
            final MamdaTradeListener  listener,
            final MamaMsg             msg,
            final MamdaTradeRecap     recap)
        {
        }

        public void onTradeReport (
            final MamdaSubscription   sub,
            final MamdaTradeListener  listener,
            final MamaMsg             msg,
            final MamdaTradeReport    trade,
            final MamdaTradeRecap     recap)
        {
            System.out.println ("Underlying trade: " + trade.getTradeVolume () +
                                " @ " + trade.getTradePrice ());
        }

        public void onTradeGap (
            final MamdaSubscription   sub,
            final MamdaTradeListener  listener,
            final MamaMsg             msg,
            final MamdaTradeGap       event,
            final MamdaTradeRecap     recap)
        {
        }

        public void onTradeCancelOrError (
            final MamdaSubscription        sub,
            final MamdaTradeListener       listener,
            final MamaMsg                  msg,
            final MamdaTradeCancelOrError  event,
            final MamdaTradeRecap          recap)
        {
        }

        public void onTradeCorrection (
            final MamdaSubscription        sub,
            final MamdaTradeListener       listener,
            final MamaMsg                  msg,
            final MamdaTradeCorrection     event,
            final MamdaTradeRecap          recap)
        {
        }

        public void onTradeClosing (
            final MamdaSubscription        sub,
            final MamdaTradeListener       listener,
            final MamaMsg                  msg,
            final MamdaTradeClosing        event,
            final MamdaTradeRecap          recap)
        {
        }

        public void onQuoteRecap (
            final MamdaSubscription   sub,
            final MamdaQuoteListener  listener,
            final MamaMsg             msg,
            final MamdaQuoteRecap     recap)
        {
        }

        public void onQuoteUpdate (
            final MamdaSubscription   sub,
            final MamdaQuoteListener  listener,
            final MamaMsg             msg,
            final MamdaQuoteUpdate    quote,
            final MamdaQuoteRecap     recap)
        {
            System.out.println (
                "Underlying quote: " + 
                quote.getBidSize () + "x" + quote.getBidPrice () + "   " +
                quote.getAskPrice () + "x" + quote.getAskSize () + "   " +
                "mid=" + recap.getQuoteMidPrice ());
            double lowStrike  = -1.0;
            double highStrike = -1.0;

            System.out.println (
                "  strikes within 15%: " + lowStrike + " " + highStrike);
        }

        public void onQuoteGap (
            final MamdaSubscription   sub,
            final MamdaQuoteListener  listener,
            final MamaMsg             msg,
            final MamdaQuoteGap       event,
            final MamdaQuoteRecap     recap)
        {
        }

        public void onQuoteClosing (
            final MamdaSubscription   sub,
            final MamdaQuoteListener  listener,
            final MamaMsg             msg,
            final MamdaQuoteClosing   event,
            final MamdaQuoteRecap     recap)
        {
        }
    }
}
