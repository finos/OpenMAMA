/* $Id$
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
	/// <summary>
	/// The MamdaBookTicker example shows how to make a subscription for an
	/// order book (similar to trades & quotes). In the callback, one gets
	/// access to the complete book after the delta has been applied.  The
	/// example prints the entire book upon receipt of a recap/initial and
	/// just the top-10 for a delta (although the full book is readily
	/// available).
	/// </summary>
	public class MamdaBookTicker
	{
        private static ArrayList mamdaSubscriptions = new ArrayList();
		private static int		 myQuietModeLevel	= 0;
		static void Main(string[] args)
		{
			MamaTransport        transport    = null;
			MamaQueue            defaultQueue = null;
			MamaDictionary       dictionary   = null;
			CommandLineProcessor options      = new CommandLineProcessor (args);
			myQuietModeLevel = options.getQuietModeLevel();

            
			if (options.hasLogLevel())
			{
				myLogLevel = options.getLogLevel();
				Mama.enableLogging(myLogLevel);
			}
            
			myCacheFullBooks = options.cacheFullBooks();
			myPrintEntries = options.getPrintEntries();
			mySnapshot = options.getSnapshot();
            myPrecision = options.getPrecision();
			try
			{
				//Initialize MAMA API
				myBridge = new MamaBridge(options.getMiddleware());
				Mama.open();
				transport = new MamaTransport();
				transport.create(options.getTransport(), myBridge);
				defaultQueue = Mama.getDefaultEventQueue(myBridge);
				myMamaSource = new MamaSource ();
				//Get the Data dictionary.....
				MamaSource dictionarySource = new MamaSource();
                dictionarySource.symbolNamespace = options.getDictSource();
				dictionarySource.transport = transport;
				dictionary = buildDataDictionary(transport, defaultQueue, dictionarySource);
	            
				MamdaOrderBookFields.setDictionary (dictionary, null);

				foreach (string symbol in options.getSymbolList())
				{
					MamdaSubscription aSubscription = new MamdaSubscription();
					MamdaOrderBookListener aBookListener;
					if (myCacheFullBooks)
					{
						aBookListener = new MamdaOrderBookListener();
					}
					else
					{
						aBookListener = new MamdaOrderBookListener(null, new MamdaOrderBook());                    
					}
	                
					BookTicker aTicker = new BookTicker();

					aBookListener.addHandler(aTicker);
					aSubscription.addMsgListener(aBookListener);
					aSubscription.addStaleListener(aTicker);
					aSubscription.addErrorListener(aTicker);

					aSubscription.setType(mamaSubscriptionType.MAMA_SUBSC_TYPE_BOOK);
					if (mySnapshot)
					{
						aSubscription.setServiceLevel(mamaServiceLevel.MAMA_SERVICE_LEVEL_SNAPSHOT, 0);
					}
					aSubscription.create(
						transport,
						defaultQueue,
						options.getSource(),
						symbol,
						null);
                    
                    mamdaSubscriptions.Add(aSubscription);
				}

				//Start dispatching on the default MAMA event queue
				Console.WriteLine("Hit Enter or Ctrl-C to exit.");
				Mama.start(myBridge);
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
		private class BookTicker :
			MamdaOrderBookHandler,
			MamdaStaleListener,
			MamdaErrorListener
		{
			public void onBookRecap(
				MamdaSubscription        sub,
				MamdaOrderBookListener   listener,
				MamaMsg                  msg,
				MamdaOrderBookDelta      bookDelta,
				MamdaOrderBookRecap      bookRecap)
			{
				if (myQuietModeLevel < 2)
				{
					Console.WriteLine ("RECAP!!!  (seq# {0}) ", msg.getSeqNum());
				}
				
				if (myCacheFullBooks)
				{
					prettyPrint(bookRecap.getFullOrderBook());
				}
				else
				{
					prettyPrint(bookDelta.getDeltaOrderBook());
				}
			}   

			public void onBookDelta(
				MamdaSubscription        sub,
				MamdaOrderBookListener   listener,
				MamaMsg                  msg,
				MamdaOrderBookDelta      bookDelta,
				MamdaOrderBookRecap      bookRecap)
			{
				if (myQuietModeLevel < 2)
				{
					Console.WriteLine ("DELTA!!!  (seq# {0}) ", msg.getSeqNum());
				}
				if (myCacheFullBooks)
				{
					prettyPrint(bookRecap.getFullOrderBook());
				}
				else
				{
					prettyPrint(bookDelta.getDeltaOrderBook());
				}
			}

			public void onBookClear (
				MamdaSubscription        sub,
				MamdaOrderBookListener   listener,
				MamaMsg                  msg,
				MamdaOrderBookDelta      bookDelta,
				MamdaOrderBookRecap      fullBook)
			{
				if (myQuietModeLevel < 2)
				{
					Console.WriteLine ("CLEAR!!!  (seq# {0}) ", msg.getSeqNum());
				}
			}

			public void onBookGap (
				MamdaSubscription        sub,
				MamdaOrderBookListener   listener,
				MamaMsg                  msg,
				MamdaOrderBookGap        gapEvent,
				MamdaOrderBookRecap      fullBook)
			{
				Console.WriteLine (
					"Received book gap " + sub.getSymbol ()
						+ " Start: " + gapEvent.getBeginGapSeqNum() 
						+ " End: "   + gapEvent.getEndGapSeqNum());
			}

			public void onStale (
				MamdaSubscription   subscription,
				mamaQuality			quality)
			{
				Console.WriteLine ("Stale (" + subscription.getSymbol() + ")");
			}

			public void onError (
				MamdaSubscription   subscription,
				MamdaErrorSeverity	severity,
				MamdaErrorCode		errorCode,
				String              errorMessage)
			{
				if (myQuiteModeLevel < 2)
				{
					Console.WriteLine ("Error (" + subscription.getSymbol() + "): "
						+ errorMessage);
				}
			}

			public void prettyPrint(MamdaOrderBook  book)
			{
				if (myPrintEntries)
				{
					prettyPrintEntries(book);
				}
				else
				{
					prettyPrintLevels(book);
				}
			}

			public void prettyPrintLevels(MamdaOrderBook  book)
			{
				IEnumerator bidEnum = book.getBidLevels().GetEnumerator();
				IEnumerator askEnum = book.getAskLevels().GetEnumerator();
				bool bidHasNext = bidEnum.MoveNext();
				bool askHasNext = askEnum.MoveNext();

				if (myQuietModeLevel<2)
				{
					Console.WriteLine ("Book for: " + book.getSymbol());
				}
				if (myQuietModeLevel<1)
				{
					Console.WriteLine ("        Time     Num  Size      Price      Act | Act     Price   Size    Num       Time  ");
				}

				while (bidHasNext || askHasNext)
				{
					if (bidHasNext)
					{
						MamdaOrderBookPriceLevel bidLevel = (MamdaOrderBookPriceLevel)bidEnum.Current;
						if (myQuietModeLevel < 1)
						{       
                            Console.Write("   ");
							paddedPrint(bidLevel.getTime().ToString("hh:mm:ss.FFF"), 12, false);
							paddedPrint(bidLevel.getNumEntries(), 4, false);
							paddedPrint(bidLevel.getSize(), 7, false);
							paddedPrintPrice(bidLevel.getPrice(), 12, false);
							Console.Write (" ");
							paddedPrint(bidLevel.getAction(), 7, false);
							Console.Write (" ");
						}
						bidHasNext = bidEnum.MoveNext();
					}
					else
					{
						if (myQuietModeLevel < 1)
							Console.Write ("                                               ");
					}
					if (myQuietModeLevel < 1)
					{
						Console.Write("|");
					}

					if(askHasNext)
					{
						MamdaOrderBookPriceLevel askLevel = (MamdaOrderBookPriceLevel)askEnum.Current;
						if (myQuietModeLevel < 1)
						{
							Console.Write ("  ");
							Console.Write (askLevel.getAction());
							Console.Write ("  ");
                            paddedPrintPrice(askLevel.getPrice(), 12, true);
							Console.Write (" ");
							paddedPrint(askLevel.getSize(), 7, true);
							Console.Write (" ");
							paddedPrint(askLevel.getNumEntries(), 6, true);
							Console.Write (" ");
							paddedPrint(askLevel.getTime().ToString("hh:mm:ss.FFF"), 12, true);
						}
						askHasNext = askEnum.MoveNext();
					}
					if (myQuietModeLevel < 1)
					{
						Console.WriteLine("");
					}
				}
				if (myQuietModeLevel < 1)
				{
					Console.WriteLine("");
				}
			}

			public void prettyPrintEntries(MamdaOrderBook book)
			{
				if (myQuietModeLevel < 2)
				{
					Console.WriteLine ("Book for: " + book.getSymbol());
				}
				if (myQuietModeLevel < 1)
				{
					Console.WriteLine ("    ID/NUM         TIME      SIZE   PRICE");
				}
				prettyPrintEntriesSide (book.getBidLevels(), "BID");
				prettyPrintEntriesSide (book.getAskLevels(), "ASK");
			}

			public void prettyPrintEntriesSide(IEnumerable sideLevels, string side)
			{
				if (myQuietModeLevel < 1)
				{
					foreach (MamdaOrderBookPriceLevel level in sideLevels)
					{
						Console.Write(" " + side + " ");
						paddedPrint(level.getNumEntries(), 4, false);
						Console.Write("       ");
						paddedPrint(level.getTime().ToString("hh:mm:ss.FFF"), 12, false);
						Console.Write(" ");
						paddedPrint(level.getSize(), 7, false);
						Console.Write(" ");
                        paddedPrintPrice(level.getPrice(), 12, false);
						Console.Write("\n");

						foreach (MamdaOrderBookEntry entry in level)
						{
							Console.Write (" ");
							paddedPrint(entry.getId(), 14, false);
							Console.Write (" ");
							paddedPrint(entry.getTime().ToString("hh:mm:ss.FFF"), 12, false);
							Console.Write (" ");
							paddedPrint(entry.getSize(), 7, false);
							Console.Write (" ");
                            paddedPrintPrice(level.getPrice(), 12, false);
							Console.Write("\n");
						}
					}
				}
			}

			private void paddedPrint(object val, int padLen, bool padAfter)
			{
				string strVal = val != null ? val.ToString() : "null";
				paddedPrint(strVal, padLen, padAfter);
			}
            
            private void paddedPrintPrice(MamaPrice price, int padLen, bool padAfter)
            {
                String precisionStr = "F2";

                if (myPrecision >= 1 && myPrecision <=6)
                {
                    precisionStr = "F" + myPrecision;
                }

                double val = price.getValue();
                paddedPrint(val.ToString(precisionStr), padLen, padAfter);
            }

			private void paddedPrint(string val, int padLen, bool padAfter)
			{
				int len = val.Length;
				if (len > padLen)
				{
					Console.WriteLine (val.Substring (0, padLen));
				}
				else
				{
					if (padAfter)
					{
						Console.Write (val);
						string padding = new string(' ', padLen - len);
						Console.Write(padding);
					}
					else
					{
						string padding = new string(' ', padLen - len);
						Console.Write (padding);
						Console.Write (val);
					}
				}
			}
		}

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

		private static MamaLogLevel	myLogLevel          = MamaLogLevel.MAMA_LOG_LEVEL_NORMAL;
		private static int			myQuiteModeLevel	= 0;
		private static bool			myCacheFullBooks    = true;
		private static MamaSource	myMamaSource;
		private static bool			myPrintEntries      = false;
		private static bool			mySnapshot			= false;
		private static MamaBridge	myBridge			= null;
        private static int          myPrecision         = 0;
	}

}
