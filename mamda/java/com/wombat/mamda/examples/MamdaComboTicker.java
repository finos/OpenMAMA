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
import com.wombat.mama.MamaDictionary;
import com.wombat.mama.MamaDictionaryCallback;
import com.wombat.mama.MamaSubscription;
import com.wombat.mama.MamaSource;

public class MamdaComboTicker
{
    private static Logger theLogger = 
                              Logger.getLogger( "com.wombat.mamda.examples" );

    private static MamaSource   mMamaSource ;
    private static MamaBridge   mBridge = null;
    private static Level        mLogLevel = Level.INFO;
    private static int          mQuietModeLevel = 0;
    private static String       mDictTransportName = null;

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
                System.err.println ("Timed out waiting for dictionary.");
                System.exit (0);
            }
            return dictionary;
        }
    }

    public static void main (final String[] args)
    {
        MamaTransport        transport      = null;
	    MamaTransport	     mDictTransport = null;
        MamaDictionary       dictionary     = null;
        CommandLineProcessor options        = new CommandLineProcessor (args);

        mQuietModeLevel = options.getQuietModeLevel ();
        mLogLevel       = options.getLogLevel ();
        if (mLogLevel != null)
        {
            theLogger.setLevel (mLogLevel);
            Mama.enableLogging (mLogLevel);
        }
        
        theLogger.info ("Source: " + options.getSource());

        try
        {
            // Initialize MAMA
            mBridge = options.getBridge();
            Mama.open ();
            transport = new MamaTransport ();
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
            dictionary = buildDataDictionary (mDictTransport,options.getDictSource());
            
            MamdaQuoteFields.setDictionary          (dictionary, null);
            MamdaTradeFields.setDictionary          (dictionary, null);
            MamdaCommonFields.setDictionary         (dictionary, null);
            MamdaSecurityStatusFields.setDictionary (dictionary, null);

            for (Iterator iterator = options.getSymbolList().iterator();
                 iterator.hasNext();)
            {
                final String symbol = (String) iterator.next();
                MamdaSubscription  aSubscription =  new MamdaSubscription ();
                
                MamdaTradeListener aTradeListener = new MamdaTradeListener ();
                MamdaQuoteListener aQuoteListener = new MamdaQuoteListener ();
                ComboTicker        aTicker        = new ComboTicker ();
                MamdaSecurityStatusListener aSecurityStatusListener = 
                    new MamdaSecurityStatusListener();

                aTradeListener.addHandler          (aTicker);
                aQuoteListener.addHandler          (aTicker);
                aSecurityStatusListener.addHandler (aTicker);
                aSubscription.addMsgListener       (aTradeListener);
                aSubscription.addMsgListener       (aQuoteListener);
                aSubscription.addStaleListener     (aTicker);
                aSubscription.addErrorListener     (aTicker);

                aSubscription.create (transport,
                                      Mama.getDefaultQueue (mBridge),
                                      options.getSource (),
                                      symbol,
                                      null /* Closure */);

                theLogger.fine ("Subscribed to: " + symbol);
            }

            Mama.start (mBridge);
            Thread.currentThread ().join ();
        }
        catch (Exception e)
        {
            e.printStackTrace ();
            System.exit (1);
        }
    }

    private static class ComboTicker implements MamdaTradeHandler, 
                                                MamdaQuoteHandler,
                                                MamdaSecurityStatusHandler,
                                                MamdaStaleListener,
                                                MamdaErrorListener 
   {
        public void onTradeRecap (
            final MamdaSubscription   sub,
            final MamdaTradeListener  listener,
            final MamaMsg             msg,
            final MamdaTradeRecap     recap)
        {
            prettyPrint ("Trade Recap (" + sub.getSymbol () + ")");
        }

        public void onTradeReport (
            final MamdaSubscription   sub,
            final MamdaTradeListener  listener,
            final MamaMsg             msg,
            final MamdaTradeReport    trade,
            final MamdaTradeRecap     recap)
        {
            prettyPrint ("Trade ("  + sub.getSymbol () +
                         ":"        + recap.getTradeCount ()    +
                         "):  "     + trade.getTradeVolume ()   +
                         " @ "      + trade.getTradePrice ()    +
                         " (seq#: " + trade.getEventSeqNum ()   +
                         "; time: " + trade.getEventTime ()     +
                         "; qual: " + trade.getTradeQual ()     + 
                         "; acttime: " + trade.getActivityTime () +
                         "; tradeCount:"+ recap.getTradeCount () + ")");
        }

        public void onTradeGap (
            final MamdaSubscription   sub,
            final MamdaTradeListener  listener,
            final MamaMsg             msg,
            final MamdaTradeGap       event,
            final MamdaTradeRecap     recap)
        {
            prettyPrint ("Trade gap (" + event.getBeginGapSeqNum () +
                         "-" + event.getEndGapSeqNum () + ")");
        }

        public void onTradeCancelOrError (
            final MamdaSubscription        sub,
            final MamdaTradeListener       listener,
            final MamaMsg                  msg,
            final MamdaTradeCancelOrError  event,
            final MamdaTradeRecap          recap)
        {
            prettyPrint ("Trade error/cancel (" + sub.getSymbol() + ")");
        }

        public void onTradeCorrection (
            final MamdaSubscription        sub,
            final MamdaTradeListener       listener,
            final MamaMsg                  msg,
            final MamdaTradeCorrection     event,
            final MamdaTradeRecap          recap)
        {
            prettyPrint ("Trade correction (" + sub.getSymbol() + ")");
        }

        public void onTradeClosing (
            final MamdaSubscription        sub,
            final MamdaTradeListener       listener,
            final MamaMsg                  msg,
            final MamdaTradeClosing        event,
            final MamdaTradeRecap          recap)
        {
            prettyPrint ("Trade closing (" + sub.getSymbol() + ")");
        }

        public void onQuoteRecap (
            final MamdaSubscription   sub,
            final MamdaQuoteListener  listener,
            final MamaMsg             msg,
            final MamdaQuoteRecap     recap)
        {
            prettyPrint ("Quote Recap (" + sub.getSymbol() + "): ");
        }

        public void onQuoteUpdate (
            final MamdaSubscription   sub,
            final MamdaQuoteListener  listener,
            final MamaMsg             msg,
            final MamdaQuoteUpdate    event,
            final MamdaQuoteRecap     recap)
        {
            prettyPrint ("Quote ("  + sub.getSymbol()        +
                         ":"        + recap.getQuoteCount()  +
                         "):  "     + event.getBidPrice()    +
                         " "        + event.getBidSize()     +
                         " X "      + event.getAskSize()     +
                         " "        + event.getAskPrice()    +
                         " (seq#: " + event.getEventSeqNum() +
                         "; time: " + event.getEventTime()   +
                         "; qual: " + event.getQuoteQual()   +
                         "; quote count: " + recap.getQuoteCount ()+ ")");
        }

        public void onQuoteGap (
            final MamdaSubscription   sub,
            final MamdaQuoteListener  listener,
            final MamaMsg             msg,
            final MamdaQuoteGap       event,
            final MamdaQuoteRecap     recap)
        {
            prettyPrint ("Quote gap (" + event.getBeginGapSeqNum() +
                         "-" + event.getEndGapSeqNum() + ")");
        }

        public void onQuoteClosing (
            final MamdaSubscription   sub,
            final MamdaQuoteListener  listener,
            final MamaMsg             msg,
            final MamdaQuoteClosing   event,
            final MamdaQuoteRecap     recap)
        {
            prettyPrint ("Quote gap (" + sub.getSymbol () + ")");
        }

        public void onSecurityStatusRecap (
            MamdaSubscription            subscription,
            MamdaSecurityStatusListener  listener,
            MamaMsg                      msg,
            MamdaSecurityStatusRecap     recap)
        {
            prettyPrint ("SecStatus recap (" + subscription.getSymbol () + 
                         " Status:" + recap.getSecurityStatusStr () + recap.getSecurityStatusQualifierStr ()+ ")" );

        }

        public void onSecurityStatusUpdate (
            MamdaSubscription            subscription,
            MamdaSecurityStatusListener  listener,
            MamaMsg                      msg,
            MamdaSecurityStatusUpdate    event,
            MamdaSecurityStatusRecap     recap)
        {
            System.out.println ("SecStatus update (" + subscription.getSymbol () +
                                " Status:" + recap.getSecurityStatusStr () + recap.getSecurityStatusQualifierStr ()+ ")");
        }


        public void onStale (
            MamdaSubscription   subscription,
            short               quality)
        {
            prettyPrint ("Stale (" + subscription.getSymbol () + ")");
        }

        public void onError (
            MamdaSubscription   subscription,
            short               severity,
            short               errorCode,
            String              errorStr)
        {
            prettyPrint ("Error (" + subscription.getSymbol () + ")");
        }

        public void prettyPrint (String what)
        {
            if (mQuietModeLevel<1)
            {
                System.out.println(what);
            }
        }
    }
}
