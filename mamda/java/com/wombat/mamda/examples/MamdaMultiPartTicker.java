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
import com.wombat.mama.MamaTransport;
import com.wombat.mama.MamaBoolean;
import com.wombat.mama.MamaSubscriptionType;
import com.wombat.mama.MamaDictionary;
import com.wombat.mama.MamaDictionaryCallback;
import com.wombat.mama.MamaSubscription;
import com.wombat.mama.MamaSource;

/**
 * Example program illustrating the use of MAMDA with multi participant group
 * subscriptions.
 *
 * In this example, we create one ComboTicker for *each* of the
 * individual participants and consolidated member of the
 * MamdaMultiParticipantManager (i.e., one each for the BBO and each
 * participant). (The same instance could be used for all. However, a separate
 * MamdaMsglistener instance must be used for each.)
 *
 * If the developer only wanted a subset of these members, then those
 * could be selected in his/her implementation of
 * MamdaMultiParticipantManager.  Similarly, the developer may not
 * want to create listeners for both trades and quotes for each
 * participant.
 *
 * Note: if trades are reported, they will be reported twice: once
 * for the participant and once for the consolidated.  Depending upon
 * exchange and oversight rules, there may be differences between
 * the trade reports for a participant versus the same report for
 * the consolidated.  For example, by certain rules, a trade may
 * qualify to update the official last price for an exchange but not
 * the consolidated last price.  The aggregate volume traded will, of
 * course, be different for each participant and the consolidated.
 */

public class MamdaMultiPartTicker
{
    private static Logger theLogger = Logger.getLogger( "com.wombat.mamda.examples" );

    private static String      mDictTransportName = null;
    private static MamaBridge  mBridge            = null;

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
            
            MamaSource mDictSource = new MamaSource ();
            mDictSource.setTransport (transport);
            mDictSource.setSymbolNamespace (dictSource);
            
            MamaDictionary   dictionary   = subscription.
                createDictionarySubscription (
                    dictionaryCallback,
                    Mama.getDefaultQueue (mBridge),
                    mDictSource);
                    

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
        MamaTransport        transport      = null;
        MamaDictionary       dictionary     = null;
        MamaTransport	     mDictTransport = null;
        CommandLineProcessor options        = new CommandLineProcessor (args);
        double               throttleRate   = options.getThrottleRate ();
        Level                level          = options.getLogLevel ();

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
            dictionary = buildDataDictionary (mDictTransport,options.getDictSource());

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

                aSubscription.create (transport,
                    Mama.getDefaultQueue (mBridge),
                    options.getSource(),
                    symbol,
                    null);

                /* For each subscription a MamdaMultiParticipantManager is
                 * added as the message listener. The callbacks on the
                 * MamdaMultiPartHandler will be invokes as new group members
                 * become available.*/
                MamdaMultiParticipantManager multiPartManager = new
                    MamdaMultiParticipantManager (symbol);
                multiPartManager.addHandler (new MultiPartHandler ());

                aSubscription.addMsgListener (multiPartManager);

                aSubscription.activate ();

                theLogger.fine( "Subscribed to: " + symbol );
            }

            Mama.start (mBridge);
            synchronized (MamdaMultiPartTicker.class)
            {
                MamdaMultiPartTicker.class.wait ();
            }
        }
        catch (Exception e)
        {
            e.printStackTrace();
            System.exit( 1 );
        }
    }

    /**
     * Implementation of the <code>MamdaMultiParticipantHandler</code>
     * interface.
     *
     * Here we are adding a trade and quote listener for every participant and
     * consolidated update as part of the underlying group subscription. The
     * assumption here is that we are interested in all trade and quote
     * information for all participants.
     */
    private static class MultiPartHandler implements
        MamdaMultiParticipantHandler
    {
        public void onConsolidatedCreate (
            MamdaSubscription              subscription,
            MamdaMultiParticipantManager   manager)
        {
            MamdaTradeListener aTradeListener = new MamdaTradeListener();
            MamdaQuoteListener aQuoteListener = new MamdaQuoteListener();
            ComboTicker        aTicker        = new ComboTicker();

            aTradeListener.addHandler (aTicker);
            aQuoteListener.addHandler (aTicker);

            manager.addConsolidatedListener (aTradeListener);
            manager.addConsolidatedListener (aQuoteListener);
        }

        public void onParticipantCreate (
            MamdaSubscription             subscription,
            MamdaMultiParticipantManager  manager,
            String                        particpantId,
            MamaBoolean                   isPrimary)
        {
            MamdaTradeListener aTradeListener = new MamdaTradeListener();
            MamdaQuoteListener aQuoteListener = new MamdaQuoteListener();
            ComboTicker        aTicker        = new ComboTicker();

            aTradeListener.addHandler (aTicker);
            aQuoteListener.addHandler (aTicker);

            manager.addParticipantListener (aTradeListener,particpantId);
            manager.addParticipantListener (aQuoteListener,particpantId);
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
                              (MamdaCommonFields.ISSUE_SYMBOL)+ "): ");
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
                               (MamdaCommonFields.ISSUE_SYMBOL)+ "): ");
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
            System.out.println ("Quote gap (" + event.getBeginGapSeqNum() +
                               "-" + event.getEndGapSeqNum() + ")");
        }

        public void onQuoteClosing (
            final MamdaSubscription   sub,
            final MamdaQuoteListener  listener,
            final MamaMsg             msg,
            final MamdaQuoteClosing   event,
            final MamdaQuoteRecap     recap)
        {
            System.out.println ("Quote Closing (" + sub.getSymbol() + "): ");
        }

        public void onStale (
            MamdaSubscription   subscription,
            short               quality)
        {
            System.out.println ("Stale (" + subscription.getSymbol() + "): ");
        }

        public void onError (
            MamdaSubscription   subscription,
            short               severity,
            short               errorCode,
            String              errorStr)
        {
            System.out.println ("Error (" + subscription.getSymbol() + "): ");
        }
    }
}
