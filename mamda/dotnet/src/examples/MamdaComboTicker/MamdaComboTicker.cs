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
using System.IO;

namespace Wombat.Mamda.Examples
{
	/// <summary>
	/// </summary>
	public class MamdaComboTicker
	{
        private static MamdaSubscription[] mamdaSubscriptions;
		private static int				   myQuietModeLevel;

		public static void Main(string[] args)
		{
			MamaTransport        transport    = null;
			MamaQueue            defaultQueue = null;
			MamaDictionary       dictionary   = null;
			CommandLineProcessor options      = new CommandLineProcessor(args);
			myQuietModeLevel = options.getQuietModeLevel();

			if (options.hasLogLevel())
			{
				myLogLevel = options.getLogLevel();
				Mama.enableLogging(myLogLevel);
			}

			try
			{
				// Initialize MAMDA
				myBridge = new MamaBridge(options.getMiddleware());
				Mama.open();
				transport = new MamaTransport();
				transport.create(options.getTransport(), myBridge);
				defaultQueue = Mama.getDefaultEventQueue(myBridge);

				/*Get the data dictionary*/
				MamaSource dictionarySource = new MamaSource();
                dictionarySource.symbolNamespace = options.getDictSource();
				dictionarySource.transport = transport;
				dictionary = buildDataDictionary(transport, defaultQueue, dictionarySource);

				MamdaQuoteFields.setDictionary(dictionary, null);
				MamdaTradeFields.setDictionary(dictionary, null);
                MamdaSecurityStatusFields.setDictionary(dictionary, null);

                mamdaSubscriptions = new MamdaSubscription [options.getSymbolList().Count];
                int i=0;
				foreach (string symbol in options.getSymbolList())
				{
					mamdaSubscriptions[i] =  new MamdaSubscription();

					MamdaTradeListener aTradeListener = new MamdaTradeListener();
					MamdaQuoteListener aQuoteListener = new MamdaQuoteListener();
                    MamdaSecurityStatusListener aSecurityStatusListener = new MamdaSecurityStatusListener();
					ComboTicker        aTicker        = new ComboTicker();

					aTradeListener.addHandler(aTicker);
					aQuoteListener.addHandler(aTicker);
                    aSecurityStatusListener.addHandler(aTicker);
					mamdaSubscriptions[i].addMsgListener(aTradeListener);
					mamdaSubscriptions[i].addMsgListener(aQuoteListener);
					mamdaSubscriptions[i].addStaleListener(aTicker);
					mamdaSubscriptions[i].addErrorListener(aTicker);

					mamdaSubscriptions[i].create(transport,
						defaultQueue,
						options.getSource(),
						symbol,
						null/*Closure*/);

                    i++;
				}

				Console.WriteLine("Hit Enter or Ctrl-C to exit.");
				Mama.start(myBridge);
				GC.KeepAlive(dictionary);
				Console.ReadLine();
			}
			catch (Exception e)
			{
				Console.Error.WriteLine(e.ToString());
				Environment.Exit(1);
			}
		}

		private class ComboTicker : MamdaTradeHandler,
									MamdaQuoteHandler,
									MamdaSecurityStatusHandler,
									MamdaStaleListener,
									MamdaErrorListener
		{
			public void onTradeRecap(
				MamdaSubscription   sub,
				MamdaTradeListener  listener,
				MamaMsg             msg,
				MamdaTradeRecap     recap)
			{
				prettyPrint("Trade Recap ({0}): ", sub.getSymbol());
			}

			public void onTradeReport(
				MamdaSubscription   sub,
				MamdaTradeListener  listener,
				MamaMsg             msg,
				MamdaTradeReport    trade,
				MamdaTradeRecap     recap)
			{
				prettyPrint(
					"Trade ({0}:{1}): {2} @ {3} (seq#: {4}; time: {5}; acttime: {6}; qual: {7})",
					sub.getSymbol(),
					recap.getTradeCount(),
					trade.getTradeVolume(),
					trade.getTradePrice(),
					trade.getEventSeqNum(),
					trade.getEventTime(),
					trade.getActivityTime(),
					trade.getTradeQual());
			}

			public void onTradeGap (
				MamdaSubscription   sub,
				MamdaTradeListener  listener,
				MamaMsg             msg,
				MamdaTradeGap       gap,
				MamdaTradeRecap     recap)
			{
				prettyPrint("Trade gap ({0}-{1})",
					gap.getBeginGapSeqNum(),
					gap.getEndGapSeqNum());
			}

			public void onTradeCancelOrError (
				MamdaSubscription        sub,
				MamdaTradeListener       listener,
				MamaMsg                  msg,
				MamdaTradeCancelOrError  cancelOrError,
				MamdaTradeRecap          recap)
			{
				prettyPrint("Trade error/cancel ({0}):", sub.getSymbol());
			}

			public void onTradeCorrection (
				MamdaSubscription        sub,
				MamdaTradeListener       listener,
				MamaMsg                  msg,
				MamdaTradeCorrection     correction,
				MamdaTradeRecap          recap)
			{
				prettyPrint("Trade correction ({0}):", sub.getSymbol());
			}

			public void onTradeClosing (
				MamdaSubscription        sub,
				MamdaTradeListener       listener,
				MamaMsg                  msg,
				MamdaTradeClosing        closing,
				MamdaTradeRecap          recap)
			{
				prettyPrint("Trade closing ({0}):", sub.getSymbol());
			}

			public void onQuoteRecap(
				MamdaSubscription   subscription,
				MamdaQuoteListener  listener,
				MamaMsg             msg,
				MamdaQuoteRecap     recap)
			{
				prettyPrint("Quote Recap ({0})", subscription.getSymbol());
			}

			public void onQuoteUpdate(
				MamdaSubscription   subscription,
				MamdaQuoteListener  listener,
				MamaMsg             msg,
				MamdaQuoteUpdate    update,
				MamdaQuoteRecap     recap)
			{
				string format = "Quote ({0}:{1}): {2} {3} X {4} {5} (seq#: {6}; time: {7}; qual: {8})";
				prettyPrint(format,
					subscription.getSymbol(),
					recap.getQuoteCount(),
					update.getBidPrice(),
					update.getBidSize(),
					update.getAskSize(),
					update.getAskPrice(),
					update.getEventSeqNum(),
					update.getEventTime(),
					update.getQuoteQual());
			}

			public void onQuoteGap(
				MamdaSubscription   subscription,
				MamdaQuoteListener  listener,
				MamaMsg             msg,
				MamdaQuoteGap       gap,
				MamdaQuoteRecap     recap)
			{
				prettyPrint("Quote gap ({0}-{1})",
					gap.getBeginGapSeqNum(),
					gap.getEndGapSeqNum());
			}

			public void onQuoteClosing(
				MamdaSubscription   subscription,
				MamdaQuoteListener  listener,
				MamaMsg             msg,
				MamdaQuoteClosing   quoteClosing,
				MamdaQuoteRecap     recap)
			{
				prettyPrint("Quote closing ({0})", subscription.getSymbol());
			}

			public void onSecurityStatusRecap(
				MamdaSubscription            subscription,
				MamdaSecurityStatusListener  listener,
				MamaMsg                      msg,
				MamdaSecurityStatusRecap     recap)
			{
				prettyPrint("SecStatus recap ({0} - {1})", subscription.getSymbol(), recap.getSecurityStatusStr ());
 			}

			public void onSecurityStatusUpdate(
				MamdaSubscription            subscription,
				MamdaSecurityStatusListener  listener,
				MamaMsg                      msg,
				MamdaSecurityStatusUpdate    update,
				MamdaSecurityStatusRecap     recap)
			{
				prettyPrint("SecStatus update ({0} - {1}", subscription.getSymbol (), listener.getSecurityStatusStr ());
			}

			public void onStale(
				MamdaSubscription   subscription,
				mamaQuality         quality)
			{
				prettyPrint("Stale ({0} - {1})", subscription.getSymbol(), quality);
			}

			public void onError(
				MamdaSubscription   subscription,
				MamdaErrorSeverity  severity,
				MamdaErrorCode      errorCode,
				string              errorStr)
			{
				prettyPrint("Error ({0}): {1} ({2})",
					subscription.getSymbol(),
					MamdaErrorCodes.stringForMamdaError(errorCode),
					errorStr);
			}

			private static void prettyPrint(string format, params object[] args)
			{
				if (myQuietModeLevel<1)
				{
					Console.WriteLine(format, args);
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

		private static MamaLogLevel myLogLevel = MamaLogLevel.MAMA_LOG_LEVEL_NORMAL;
		private static MamaBridge	myBridge   = null;
	}
}
