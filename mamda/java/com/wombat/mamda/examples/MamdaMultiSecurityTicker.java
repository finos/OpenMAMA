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
import com.wombat.mamda.*;
import com.wombat.mama.Mama;
import com.wombat.mama.MamaBridge;
import com.wombat.mama.MamaMsg;
import com.wombat.mama.MamaMsgType;
import com.wombat.mama.MamaTransport;
import com.wombat.mama.MamaBoolean;
import com.wombat.mama.MamaSubscriptionType;
import com.wombat.mama.MamaDictionary;
import com.wombat.mama.MamaDictionaryCallback;
import com.wombat.mama.MamaSubscription;
import com.wombat.mama.MamaSource;

public class MamdaMultiSecurityTicker
{
    private static Logger theLogger = Logger.getLogger( "com.wombat.mamda.examples" );

    private static MamaSource  mMamaSource;
    private static MamaBridge  mBridge       = null;

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
        MamaTransport        transport    = null;
        MamaDictionary       dictionary   = null;
        CommandLineProcessor options      = new CommandLineProcessor (args);
        double               throttleRate = options.getThrottleRate ();

        Level level = options.getLogLevel ();
        if (level != null)
        {
            theLogger.setLevel (level);
            Mama.enableLogging (level);
        }
        theLogger.info ("Source: " + options.getSource ());

        if ((throttleRate > 100.0) || (throttleRate <= 0.0))
        {
            throttleRate = 100.0;
        }

        try
        {
            // Initialize MAMDA
            mBridge = options.getBridge();
            Mama.open ();

            transport = new MamaTransport ();
            transport.create (options.getTransport(), mBridge);
            mMamaSource = new MamaSource ();

            //Get the Data dictionary.....
            dictionary = buildDataDictionary (transport,"WOMBAT");

            //Initialise the dictionary and fields
            MamdaCommonFields.setDictionary (dictionary,null);
            MamdaQuoteFields.setDictionary  (dictionary,null);
            MamdaTradeFields.setDictionary  (dictionary,null);

            for (Iterator iterator = options.getSymbolList().iterator();
                 iterator.hasNext();)
            {
                final String symbol = (String) iterator.next();
                MamdaSubscription  aSubscription =  new MamdaSubscription ();
                aSubscription.setType (MamaSubscriptionType.GROUP);
                aSubscription.setTimeout (60.0);
                aSubscription.create (transport,
                    Mama.getDefaultQueue (mBridge),
                    options.getSource(),
                    symbol,
                    null);

                /* For each subscription a MamdaMultiSecurityicipantManager is
                 * added as the message listener. The callbacks on the
                 * MamdaMultiSecurityHandler will be invokes as new group members
                 * become available.*/
                MamdaMultiSecurityManager multiSecurityManager = new
                    MamdaMultiSecurityManager (symbol);
                multiSecurityManager.addHandler (new MultiSecurityHandler ());

                aSubscription.addMsgListener (multiSecurityManager);

                aSubscription.activate ();

                theLogger.fine( "Subscribed to: " + symbol );
            }

            Mama.start (mBridge);
            synchronized (MamdaMultiSecurityTicker.class)
            {
                MamdaMultiSecurityTicker.class.wait ();
            }
        }
        catch (Exception e)
        {
            e.printStackTrace();
            System.exit( 1 );
        }
    }

    /**
     * Implementation of the <code>MamdaMultiSecurityicipantHandler</code>
     * interface.
     *
     * Here we are adding a trade and quote listener for every participant and
     * consolidated update as part of the underlying group subscription. The
     * assumption here is that we are interested in all trade and quote
     * information for all participants.
     */
    private static class MultiSecurityHandler implements
        MamdaMultiSecurityHandler
    {
        public void onSecurityCreate (
                MamdaSubscription           subscription,
                MamdaMultiSecurityManager   manager,
                String                      symbol)
        {
            MamdaTradeListener aTradeListener = new MamdaTradeListener();
            MamdaQuoteListener aQuoteListener = new MamdaQuoteListener();
            ComboTicker        aTicker        = new ComboTicker();

            aTradeListener.addHandler (aTicker);
            aQuoteListener.addHandler (aTicker);

            manager.addListener (aTradeListener, symbol);
            manager.addListener (aQuoteListener, symbol);
        }
    }

    /**
     * Is a MamdaTradeHandler and a MamdaQuoteHandler.
     *
     * Can be used to handler callbacks from both a TradeListener and a
     * QuoteListener.
     */
    private static class ComboTicker implements MamdaTradeHandler,
                                                MamdaQuoteHandler,
                                                MamdaStaleListener,
                                                MamdaErrorListener
   {
        public void onTradeRecap (
            final MamdaSubscription   sub,
            final MamdaTradeListener  listener,
            final MamaMsg             msg,
            final MamdaTradeRecap     recap)
        {
            System.out.println("Trade Recap (" + msg.getString
                               (MamdaCommonFields.ISSUE_SYMBOL)+ "):" +
                               " Type: " + MamaMsgType.stringForType (msg) + 
                               ")");
        }

        public void onTradeReport (
            final MamdaSubscription   sub,
            final MamdaTradeListener  listener,
            final MamaMsg             msg,
            final MamdaTradeReport    trade,
            final MamdaTradeRecap     recap)
        {
            System.out.println ("Trade ("  + msg.getString
                                             (MamdaCommonFields.ISSUE_SYMBOL) +
                                ":"        + recap.getTradeCount()    +
                                "):  "     + trade.getTradeVolume()   +
                                " @ "      + trade.getTradePrice()    +
                                " (seq#: " + trade.getEventSeqNum()   +
                                "; time: " + trade.getEventTime()     +
                                "; qual: " + trade.getTradeQual()     +
                                "; acttime: " + trade.getActivityTime() + ")");
        }

        public void onTradeGap (
            final MamdaSubscription   sub,
            final MamdaTradeListener  listener,
            final MamaMsg             msg,
            final MamdaTradeGap       event,
            final MamdaTradeRecap     recap)
        {
            System.out.println("Trade gap  (" +  msg.getString
                               (MamdaCommonFields.ISSUE_SYMBOL) +
                               ":"+   event.getBeginGapSeqNum() +
                               "-" + event.getEndGapSeqNum() + ")");
        }

        public void onTradeCancelOrError (
            final MamdaSubscription        sub,
            final MamdaTradeListener       listener,
            final MamaMsg                  msg,
            final MamdaTradeCancelOrError  event,
            final MamdaTradeRecap          recap)
        {
            System.out.println("Trade error/cancel (" + sub.getSymbol() + "): ");
        }

        public void onTradeCorrection (
            final MamdaSubscription        sub,
            final MamdaTradeListener       listener,
            final MamaMsg                  msg,
            final MamdaTradeCorrection     event,
            final MamdaTradeRecap          recap)
        {
            System.out.println("Trade correction (" + sub.getSymbol() + "): ");
        }

        public void onTradeClosing (
            final MamdaSubscription        sub,
            final MamdaTradeListener       listener,
            final MamaMsg                  msg,
            final MamdaTradeClosing        event,
            final MamdaTradeRecap          recap)
        {
            System.out.println("Trade Closing (" + sub.getSymbol() + "): ");
        }

        public void onQuoteRecap (
            final MamdaSubscription   sub,
            final MamdaQuoteListener  listener,
            final MamaMsg             msg,
            final MamdaQuoteRecap     recap)
        {
            System.out.println ("Quote Recap (" + msg.getString
                               (MamdaCommonFields.ISSUE_SYMBOL)+ "):" +
                               " Type: " + MamaMsgType.stringForType (msg));
        }

        public void onQuoteUpdate (
            final MamdaSubscription   sub,
            final MamdaQuoteListener  listener,
            final MamaMsg             msg,
            final MamdaQuoteUpdate    event,
            final MamdaQuoteRecap     recap)
        {
            System.out.println ("Quote ("  + msg.getString
                                 (MamdaCommonFields.ISSUE_SYMBOL)   +
                                ":"        + recap.getQuoteCount()  +
                                "):  "     + event.getBidPrice()    +
                                " "        + event.getBidSize()     +
                                " X "      + event.getAskSize()     +
                                " "        + event.getAskPrice()    +
                                " (seq#: " + event.getEventSeqNum() +
                                "; time: " + event.getEventTime()   +
                                "; qual: " + event.getQuoteQual()   + ")");
        }

        public void onQuoteGap (
            final MamdaSubscription   sub,
            final MamdaQuoteListener  listener,
            final MamaMsg             msg,
            final MamdaQuoteGap       event,
            final MamdaQuoteRecap     recap)
        {
            System.out.println("Quote gap (" + event.getBeginGapSeqNum() +
                               "-" + event.getEndGapSeqNum() + ")");
        }

        public void onQuoteClosing (
            final MamdaSubscription   sub,
            final MamdaQuoteListener  listener,
            final MamaMsg             msg,
            final MamdaQuoteClosing   event,
            final MamdaQuoteRecap     recap)
        {
            System.out.println("Quote Closing (" + sub.getSymbol() + "): ");
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
    }
}
