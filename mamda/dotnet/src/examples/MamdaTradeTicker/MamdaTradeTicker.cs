/* $Id: MamdaTradeTicker.cs,v 1.11.30.5 2012/09/07 07:44:57 ianbell Exp $
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
using System.Collections;

namespace Wombat.Mamda.Examples
{
	/// <summary>
	/// </summary>
	public class MamdaTradeTicker
	{
       private static MamdaSubscription[] mamdaSubscriptions;

		static void Main(string[] args)
		{
			MamaTransport        transport    = null;
			MamaQueue            defaultQueue = null;
			MamaDictionary       dictionary   = null;
			CommandLineProcessor options      = new CommandLineProcessor (args);
            
			if (options.hasLogLevel())
			{
				Mama.enableLogging(options.getLogLevel());
			}

			try
			{
				//Initialize MAMA API
				myBridge = new MamaBridge(options.getMiddleware());
				Mama.open();
				transport = new MamaTransport();
				transport.create(options.getTransport(), myBridge);
				defaultQueue = Mama.getDefaultEventQueue(myBridge);

				//Get the Data dictionary.....
				MamaSource dictionarySource = new MamaSource();
				dictionarySource.symbolNamespace = "WOMBAT";
				dictionarySource.transport = transport;
				dictionary = buildDataDictionary(transport, defaultQueue, dictionarySource);
	            
				MamdaTradeFields.setDictionary(dictionary, null);

                mamdaSubscriptions = new MamdaSubscription [options.getSymbolList().Count];
                int i=0;
				foreach (string symbol in options.getSymbolList())
				{
					mamdaSubscriptions[i] =  new MamdaSubscription();
					MamdaTradeListener aTradeListener = new MamdaTradeListener();
					TradeTicker aTicker = new TradeTicker();

					aTradeListener.addHandler(aTicker);
					mamdaSubscriptions[i].addMsgListener(aTradeListener);
					mamdaSubscriptions[i].addStaleListener(aTicker);
					mamdaSubscriptions[i].addErrorListener(aTicker);

					mamdaSubscriptions[i].create(
						transport,
						defaultQueue,
						options.getSource(),
						symbol,
						null);
                    i++;
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

		private class TradeTicker : 
			MamdaTradeHandler,
			MamdaStaleListener,
			MamdaErrorListener

		{
			public void onTradeRecap(
				MamdaSubscription   sub,
				MamdaTradeListener  listener,
				MamaMsg             msg,
				MamdaTradeRecap     recap)
			{
				Console.WriteLine("Trade Recap ({0}): ", sub.getSymbol());
			}

			public void onTradeReport(
				MamdaSubscription   sub,
				MamdaTradeListener  listener,
				MamaMsg             msg,
				MamdaTradeReport    trade,
				MamdaTradeRecap     recap)
			{
				Console.WriteLine("wSettlePrice: " + recap.getSettlePrice());
				Console.WriteLine("wSettleDate: " + recap.getSettleDate());
			
				Console.WriteLine(
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
				Console.WriteLine("Trade gap ({0}-{1})",
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
				Console.WriteLine("Trade error/cancel ({0}):", sub.getSymbol());
			}

			public void onTradeCorrection (
				MamdaSubscription        sub,
				MamdaTradeListener       listener,
				MamaMsg                  msg,
				MamdaTradeCorrection     correction,
				MamdaTradeRecap          recap)
			{
				Console.WriteLine("Trade correction ({0}):", sub.getSymbol());
			}

			public void onTradeClosing (
				MamdaSubscription        sub,
				MamdaTradeListener       listener,
				MamaMsg                  msg,
				MamdaTradeClosing        closing,
				MamdaTradeRecap          recap)
			{
			}

			public void onStale (
				MamdaSubscription   subscription,
				mamaQuality			quality)
			{
				Console.WriteLine("Stale ({0}): ", subscription.getSymbol());
			}

			public void onError (
				MamdaSubscription   subscription,
				MamdaErrorSeverity	severity,
				MamdaErrorCode		errorCode,
				String              errorStr)
			{
				Console.WriteLine("Error ({0}): {1} ({2})",
					subscription.getSymbol(),
					MamdaErrorCodes.stringForMamdaError(errorCode),
					errorStr);
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

		private static MamaBridge myBridge = null;
	}

}

