/* $Id: MamdaAtomicBookTicker.cs,v 1.4.30.5 2012/09/07 07:44:57 ianbell Exp $
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

using System;
using System.Collections;
using System.Threading;

namespace Wombat.Mamda.Examples
{

    public class MamdaAtomicBookTicker
    {
        private static ArrayList mamdaSubscriptions = new ArrayList();

        /*Controls the logging within the API*/
        private static MamaLogLevel myLogLevel          = MamaLogLevel.MAMA_LOG_LEVEL_NORMAL;
        /*Controls what gets printed in the example program*/
        private static int         myQuietModeLevel    = 0;
        private static MamaSource  myMamaSource;
        private static MamaBridge  myBridge            = null;
        private static MamaQueueGroup myQueueGroup     = null;

        private static MamaDictionary buildDataDictionary(
            MamaTransport transport,
            MamaQueue defaultQueue,
            MamaSource dictionarySource)
        {
            bool[] gotDict = new bool[] { false };
            MamaDictionaryCallback dictionaryCallback = new DictionaryCallback(gotDict);
            lock (dictionaryCallback)
            {
                MamaSubscription subscription = new MamaSubscription ();
                MamaDictionary dictionary = new MamaDictionary();
                dictionary.create(
                    defaultQueue,
                    dictionaryCallback,
                    dictionarySource,
                    3,
                    10);
                Mama.start(myBridge);
                if (!gotDict[0])
                {
                    if (!Monitor.TryEnter(dictionaryCallback, 30000))
                    {
                        Console.Error.WriteLine("Timed out waiting for dictionary.");
                        Environment.Exit(0);
                    }
                    Monitor.Exit(dictionaryCallback);
                }
                return dictionary;
            }
        }

        private class DictionaryCallback : MamaDictionaryCallback
        {
            public DictionaryCallback(bool[] gotDict)
            {
                gotDict_ = gotDict;
            }

            public void onTimeout(MamaDictionary dictionary)
            {
                Console.Error.WriteLine("Timed out waiting for dictionary");
                Environment.Exit(1);
            }

            public void onError(MamaDictionary dictionary, string message)
            {
                Console.Error.WriteLine("Error getting dictionary: {0}", message);
                Environment.Exit(1);
            }

            public void onComplete(MamaDictionary dictionary)
            {
                lock (this)
                {
                    gotDict_[0] = true;
                    Mama.stop(myBridge);
                    Monitor.PulseAll(this);
                }
            }

            private bool[] gotDict_;
        }


        static void Main(string[] args)
        {

            MamaTransport        transport = null;
            MamaQueue            defaultQueue = null;
            MamaDictionary       dictionary = null;
            CommandLineProcessor options = new CommandLineProcessor (args);
            myQuietModeLevel = options.getQuietModeLevel ();
            

            if (options.hasLogLevel ())
            {	
				myLogLevel = options.getLogLevel ();
                Mama.enableLogging (myLogLevel);
            }

            try
            {
                //Initialize MAMA API
                myBridge = new MamaBridge (options.getMiddleware());
                Mama.open();
                transport = new MamaTransport();
                transport.create(options.getTransport(), myBridge);
                defaultQueue = Mama.getDefaultEventQueue(myBridge);
                myMamaSource = new MamaSource ();
                //Get the Data dictionary.....
				MamaSource dictionarySource = new MamaSource ();
                dictionarySource.symbolNamespace = "WOMBAT";
                dictionarySource.transport = transport;
                dictionary = buildDataDictionary(transport, defaultQueue, dictionarySource);

                MamdaQuoteFields.setDictionary (dictionary, null);
                MamdaTradeFields.setDictionary (dictionary, null);
                MamdaCommonFields.setDictionary (dictionary, null);
                MamdaOrderBookFields.setDictionary (dictionary, null);

                myQueueGroup = new MamaQueueGroup (myBridge, options.getNumThreads());

                foreach (string symbol in options.getSymbolList())
                {
                    MamdaSubscription  aSubscription = new MamdaSubscription ();

                    MamdaBookAtomicListener aBookListener = new MamdaBookAtomicListener();
                    AtomicBookTicker        aTicker       = new AtomicBookTicker ();

                    aBookListener.addBookHandler (aTicker);
                    aBookListener.addLevelHandler (aTicker);
     
                    if (options.getPrintEntries())
                    {   // Entries
                        aBookListener.addLevelEntryHandler (aTicker);
                    }

                    aSubscription.addMsgListener (aBookListener);
                    aSubscription.addStaleListener (aTicker);
                    aSubscription.addErrorListener (aTicker);
                    aSubscription.setType(mamaSubscriptionType.MAMA_SUBSC_TYPE_BOOK);

                    aSubscription.create (transport,
                                          myQueueGroup.getNextQueue(),
                                          options.getSource (),
                                          symbol,
                                          null);
					
					mamdaSubscriptions.Add(aSubscription);
                }

                //Start dispatching on the default MAMA event queue
                Console.WriteLine("Hit Enter or Ctrl-C to exit.");
                Mama.start (myBridge);
                GC.KeepAlive(dictionary);
                Console.ReadLine();
                }
                catch (Exception e)
                {
                    Console.WriteLine(e.ToString());
                    Environment.Exit(1);
                }

        }

        /*Handler class which receives update callbacks*/
        private class AtomicBookTicker : MamdaBookAtomicBookHandler,
                                                         MamdaBookAtomicLevelHandler,
                                                         MamdaBookAtomicLevelEntryHandler,
                                                         MamdaStaleListener,
                                                         MamdaErrorListener
        {
            public void onBookAtomicLevelRecap (
                MamdaSubscription           subscription,
                MamdaBookAtomicListener     listener,
                MamaMsg                     msg,
                MamdaBookAtomicLevel        level)
            {
                if (myQuietModeLevel<2)
                {
                    prettyPrint (subscription, level);
                }
            }
        
            public void onBookAtomicBeginBook (
                MamdaSubscription           subscription,
                MamdaBookAtomicListener     listener,
                bool                     isRecap) 
        
            {
                if (myQuietModeLevel<2)
                {
                     Console.WriteLine ("BEGIN BOOK - " + subscription.getSymbol ());
                }
            }
        
            public void onBookAtomicClear (
                MamdaSubscription           subscription,
                MamdaBookAtomicListener     listener,
                MamaMsg                     msg)
            {
                if (myQuietModeLevel<2)
                {
                     Console.WriteLine ("BOOK CLEAR - " + subscription.getSymbol () );
                     Console.WriteLine ();
                }
            }
        
            public void onBookAtomicGap (
                MamdaSubscription              subscription,
                MamdaBookAtomicListener        listener,
                MamaMsg                        msg,
                MamdaBookAtomicGap             gapEvent)
            {
                if (myQuietModeLevel<2)
                {
                    Console.WriteLine ( "BOOK GAP - " + subscription.getSymbol () );
                    Console.WriteLine ( "Begin Gap: " + listener.getBeginGapSeqNum () );
                    Console.WriteLine ( "End Gap  : " + listener.getEndGapSeqNum () );
                    Console.WriteLine ();
                }
            }
        
            public void onBookAtomicEndBook (
                MamdaSubscription           subscription,
                MamdaBookAtomicListener     listener) 
            {
                if (myQuietModeLevel<2)
                {
                    if (listener.getPriceLevelNumLevels () == 0)
                    {
                        Console.WriteLine ();
                        Console.WriteLine ( "END BOOK - " + subscription.getSymbol () );
                        Console.WriteLine ( " - empty book message !!!" );
                        Console.WriteLine ();
                        Console.WriteLine ();
                    }
                    else
                    {
                      Console.WriteLine ();
                      Console.WriteLine ( "END BOOK - " + subscription.getSymbol () );
                      Console.WriteLine ();
                      Console.WriteLine ();
                    }
                }
            }
        
            public void onBookAtomicLevelEntryRecap (
                MamdaSubscription             subscription,
                MamdaBookAtomicListener       listener,
                MamaMsg                       msg,
                MamdaBookAtomicLevelEntry     levelEntry)
            {
                if (myQuietModeLevel<2)
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
                if (myQuietModeLevel<2)
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
                if (myQuietModeLevel<2)
                {
                    prettyPrint (subscription, levelEntry);
                }
            }
        
            public void onError (
                    MamdaSubscription   subscription,
                    MamdaErrorSeverity  severity,
                    MamdaErrorCode      errorCode,
                    String              errorStr)
            {
            }
        
            public void onStale (
                    MamdaSubscription   subscription,
                    mamaQuality               quality)
            {
            }
        
            void prettyPrint (MamdaSubscription subscription,
                            MamdaBookAtomicLevelEntry levelEntry)
            {
                // Print Entry Level Info
                string	symbol        = subscription.getSymbol ();
                long	actNumEntries = levelEntry.getPriceLevelActNumEntries ();
                char	entryAction   = levelEntry.getPriceLevelEntryAction ();
                string  entryId       = levelEntry.getPriceLevelEntryId ();
                long    entrySize     = levelEntry.getPriceLevelEntrySize ();
        
                Console.WriteLine ("\tENTRY | " + symbol + " | " + actNumEntries + " | "
                    + entryAction + " | " + entryId + " | " + entrySize );
            }
        
            void prettyPrint (MamdaSubscription subscription, 
                            MamdaBookAtomicLevel level)
            {
                // Price Level Info
                double  price      = level.getPriceLevelPrice ();
                decimal dPrice     = System.Convert.ToDecimal(price);
                string  roundPrice = (Decimal.Round(dPrice,2)).ToString("0.00");
                
                string   symbol     = subscription.getSymbol ();
                double   size       = level.getPriceLevelSize ();
                char     action     = level.getPriceLevelAction ();
                char     side       = level.getPriceLevelSide ();
                double   numEntries = level.getPriceLevelNumEntries ();
                DateTime levelTime  = level.getPriceLevelTime();

                Console.WriteLine ();
                Console.WriteLine ( "LEVEL | " + symbol + " | " + roundPrice + " | " + size + " | "
                    + action + " | " + side + " | " + numEntries + " | " + "{0:HH:mm:ss.fff}", levelTime);
            }
        }
    }
}
