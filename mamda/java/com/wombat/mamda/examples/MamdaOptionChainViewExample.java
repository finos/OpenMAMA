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
import java.util.Collection;
import java.util.Date;
import java.util.Iterator;
import com.wombat.mamda.*;
import com.wombat.mamda.options.*;
import com.wombat.mama.*;

/**
 * An example of MAMDA option chain "view" processing.  A "view" on an
 * option chain is the set of option contracts that fits into a
 * sliding set of strike prices and expiration cycles.  The strike
 * prices included in the view may change as the price of underlying
 * security changes.
 */

public class MamdaOptionChainViewExample
{
    private static Logger theLogger = Logger.getLogger( "com.wombat.mamda.examples" );

    private static MamaSource   mMamaSource;
    private static MamaBridge   mBridge         = null;
    private static Date         nullDate        = new Date (0,0,0);

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
            
            mMamaSource.setTransport       (transport);
            mMamaSource.setSymbolNamespace (dictSource);
            MamaDictionary   dictionary   = subscription.
                createDictionarySubscription (
                    dictionaryCallback,
                    Mama.getDefaultQueue (mBridge),
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
        MamaTransport        baseTransport    = null;
        MamaTransport        optionTransport  = null;
        MamaTransport        dictTransport    = null;
        MamaDictionary       dictionary       = null;
        CommandLineProcessor options          = new CommandLineProcessor (args);

        Level level = options.getLogLevel();
        if (level != null)
        {
            theLogger.setLevel (level);
            Mama.enableLogging (level);
        }
        theLogger.info( "Source: " + options.getSource() );

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
            baseTransport = new MamaTransport();
            baseTransport.create(options.getTransport(), mBridge);
            optionTransport = baseTransport;
            mMamaSource = new MamaSource ();
            
            // The dictionary transport might be different from the
            // other transports (although in this example we're just
            // using the same transport name as the underlying).
            dictTransport = baseTransport;

            // Fetch and initialize the data dictionary
            dictionary = buildDataDictionary     (dictTransport,"WOMBAT");
            MamdaQuoteFields.setDictionary       (dictionary, null);
            MamdaTradeFields.setDictionary       (dictionary, null);
            MamdaFundamentalFields.setDictionary (dictionary, null);
            MamdaOptionFields.setDictionary      (dictionary, null);
            MamdaCommonFields.setDictionary      (dictionary, null);

            // Create listeners for each chain.  Two subscriptions are
            // necessary.
            for (Iterator iterator = options.getSymbolList().iterator();
                 iterator.hasNext();
                )
            {
                final String symbol = (String) iterator.next();

                // Create chain and listener objects.
                MamdaTradeListener aBaseTradeListener = new MamdaTradeListener();
                MamdaQuoteListener aBaseQuoteListener = new MamdaQuoteListener();
                MamdaOptionChain   anOptionChain      = new MamdaOptionChain  (symbol);

                anOptionChain.setUnderlyingQuoteListener(aBaseQuoteListener);
                anOptionChain.setUnderlyingTradeListener(aBaseTradeListener);

                MamdaOptionChainListener  anOptionListener =
                    new MamdaOptionChainListener(anOptionChain);

                // Set up the view (could be configurable, of course):
                MamdaOptionChainView anOptionView =
                    new MamdaOptionChainView (anOptionChain);
                anOptionView.setNumberOfExpirations (2);
                anOptionView.setStrikeRangeNumber   (4);
                anOptionView.setJitterMargin        (1.0);

                // Create our handlers (the UnderlyingTicker and
                // OptionChainDisplay could be a single class).
                UnderlyingTicker   aBaseTicker  = new UnderlyingTicker   (anOptionChain);
                OptionChainDisplay aDisplay     = new OptionChainDisplay (anOptionView);

                // Create subscriptions for underlying and option chain:
                MamdaSubscription  aBaseSubscription    = new MamdaSubscription ();
                MamdaSubscription  anOptionSubscription = new MamdaSubscription ();

                // Register for underlying quote and trade events.
                aBaseTradeListener.addHandler    (aBaseTicker);
                aBaseQuoteListener.addHandler    (aBaseTicker);
                aBaseSubscription.addMsgListener (aBaseTradeListener);
                aBaseSubscription.addMsgListener (aBaseQuoteListener);

                aBaseSubscription.create (baseTransport,
                                          Mama.getDefaultQueue (mBridge), 
                                          options.getSource(),
                                          symbol,
                                          null);

                // Register for underlying option events.
                anOptionListener.addHandler (aDisplay);
                anOptionListener.addHandler (anOptionView);

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
                                             options.getSource (),
                                             symbol,
                                             null);

                theLogger.fine( "Subscribed to: " + symbol );
            }

            Mama.start (mBridge);
            synchronized (MamdaOptionChainViewExample.class)
            {
                MamdaOptionChainViewExample.class.wait ();
            }
        }
        catch (Exception e)
        {
            e.printStackTrace();
            System.exit( 1 );
        }
    }

    private static class OptionChainDisplay implements MamdaOptionChainHandler,
                                                       MamdaStaleListener,
                                                       MamdaErrorListener 
    {
        MamdaOptionChainView  mView  = null;
        boolean mGotRecap = false;  // Updated when we get the first recap.

        public OptionChainDisplay (MamdaOptionChainView  view)
        {
            mView  = view;
        }

        public void onOptionChainRecap (
            final MamdaSubscription         subscription,
            final MamdaOptionChainListener  listener,
            final MamaMsg                   msg,
            final MamdaOptionChain          chain)
        {
            mGotRecap = true;
            printView ();
        }

        public void onOptionSeriesUpdate (
            final MamdaSubscription         subscription,
            final MamdaOptionChainListener  listener,
            final MamaMsg                   msg,
            final MamdaOptionSeriesUpdate   event,
            final MamdaOptionChain          chain)
        {
            printView ();
        }

        public void onOptionChainGap (
            final MamdaSubscription         subscription,
            final MamdaOptionChainListener  listener,
            final MamaMsg                   msg,
            final MamdaOptionChain          chain)
        {
            printView ();
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
                new OptionContractTicker(contract, mView, this);
            contract.addQuoteHandler (aTicker);
            contract.addTradeHandler (aTicker);
        }

        public void onStale (
            MamdaSubscription   subscription,
            short               quality)
        {
            System.out.println("Stale (" + subscription.getSymbol() + "): ");
        }

        public void onError (
            MamdaSubscription   subscription,
            short               severity,
            short               errorCode,
            String              errorStr)
        {
            System.out.println("Error (" + subscription.getSymbol() + "): ");
        }

        public void printView ()
        {
            if (!mGotRecap)
                return;

            System.out.println("Chain: " + mView.getSymbol());

            // Loop through the expiration date sets (within the view):
            Iterator expireIter = mView.expirationIterator();
            while (expireIter.hasNext())
            {
                MamdaOptionExpirationStrikes expireStrikes =
                    (MamdaOptionExpirationStrikes) expireIter.next();
                printExpireStrikes (expireStrikes);
            }

            System.out.println("\nDone with chain\n");
        }

        public void printExpireStrikes (
            final MamdaOptionExpirationStrikes  expireStrikes)
        {
            // Loop through the strike prices (within the view):
            Iterator strikeIter = expireStrikes.values().iterator();
            while (strikeIter.hasNext())
            {
                MamdaOptionStrikeSet strikeSet =
                    (MamdaOptionStrikeSet)strikeIter.next();

                MamdaOptionContractSet callContracts = strikeSet.getCallSet();
                MamdaOptionContractSet putContracts  = strikeSet.getPutSet();
                Date  expireDate = strikeSet.getExpireDate();
                double  strikePrice   = strikeSet.getStrikePrice();

                if (!expireDate.equals(nullDate))
                {
                    System.out.println ();
                    System.out.println ("Strike: " + expireDate + " " +
                                        strikePrice);
                }
                else
                {
                  System.out.println ();
                  System.out.println ("Strike:  ExpireDate(null) " + " " +
                      strikePrice);
                }
                printContractSet (callContracts, "Call");
                printContractSet (putContracts,  "Put");
            }
        }

        public void printContractSet (
            final MamdaOptionContractSet  contractSet,
            final String                  putCallStr)
        {
            if (contractSet == null)
                return;
            MamdaOptionContract  bbo      = contractSet.getBboContract();
            MamdaOptionContract  wbbo     = contractSet.getWombatBboContract();
            Collection           exchs    = contractSet.getExchangeContracts();
            Iterator             exchIter = exchs.iterator();

            System.out.println ("  " + putCallStr + " options:");
            printContract (bbo);
            printContract (wbbo);
            while (exchIter.hasNext())
            {
                MamdaOptionContract exch =(MamdaOptionContract)exchIter.next();
                printContract (exch);
            }
        }

        public void printContract (final MamdaOptionContract  contract)
        {
            if (!mGotRecap)
                return;
            if (contract == null)
                return;

            MamdaTradeRecap tradeRecap   = contract.getTradeInfo    ();
            MamdaQuoteRecap quoteRecap   = contract.getQuoteInfo    ();
            String          symbol       = contract.getSymbol       ();
            String          exchange     = contract.getExchange     ();
            Date            expireDate   = contract.getExpireDate   ();
            double          strikePrice  = contract.getStrikePrice  ();
            double          lastPrice    = tradeRecap.getLastPrice  ().getValue();
            double          accVolume    = tradeRecap.getAccVolume  ();
            double          bidPrice     = quoteRecap.getBidPrice   ().getValue();
            double          askPrice     = quoteRecap.getAskPrice   ().getValue();

            if (!expireDate.equals(nullDate))
            {
            System.out.println ("    " +
                                expireDate + " "   + strikePrice + " ("    +
                                symbol     + " "   + exchange    + ")  | " +
                                lastPrice  + " | " + bidPrice    + " | "   +
                                askPrice   + " | " + accVolume);
            }
            else
              System.out.println ("    ExpireDate(null) " 
                  + strikePrice + " ("    +
                  symbol     + " "   + exchange    + ")  | " +
                  lastPrice  + " | " + bidPrice    + " | "   +
                  askPrice   + " | " + accVolume);
              
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
        MamdaOptionContract   mContract = null;
        MamdaOptionChainView  mView     = null;
        OptionChainDisplay    mDisplay  = null;

        public OptionContractTicker (MamdaOptionContract   contract,
                                     MamdaOptionChainView  view,
                                     OptionChainDisplay    display)
        {
            mContract            = contract;
            mView                = view;
            mDisplay             = display;
        }

        public void onTradeRecap (
            final MamdaSubscription   sub,
            final MamdaTradeListener  listener,
            final MamaMsg             msg,
            final MamdaTradeRecap     recap)
        {
            // Refresh the "display" of this contract.
            if (mView.isVisible(mContract))
                mDisplay.printContract (mContract);
        }

        public void onTradeReport (
            final MamdaSubscription   sub,
            final MamdaTradeListener  listener,
            final MamaMsg             msg,
            final MamdaTradeReport    trade,
            final MamdaTradeRecap     recap)
        {
            // Do something with a specific trade report.
            if (mView.isVisible(mContract))
                mDisplay.printContract (mContract);
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
            if (mView.isVisible(mContract))
                mDisplay.printContract (mContract);
        }

        public void onQuoteUpdate (
            final MamdaSubscription   sub,
            final MamdaQuoteListener  listener,
            final MamaMsg             msg,
            final MamdaQuoteUpdate    event,
            final MamdaQuoteRecap     recap)
        {
            // Do something with a specific quote.
            if (mView.isVisible(mContract))
                mDisplay.printContract (mContract);
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
            System.out.println ("Underlying trade: " + trade.getTradeVolume() +
                                " @ " + trade.getTradePrice());
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
            System.out.println(
                "Underlying quote: " + 
                quote.getBidSize()  + "x" + quote.getBidPrice() + "   " +
                quote.getAskPrice() + "x" + quote.getAskSize()  + "   " +
                "mid=" + recap.getQuoteMidPrice());
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
