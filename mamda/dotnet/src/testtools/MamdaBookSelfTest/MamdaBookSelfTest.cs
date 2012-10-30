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
using System.Threading;
using System.Collections;

namespace Wombat.Mamda.Examples
{
	/// <summary>
	/// The purpose of this example is to illustrate use of the
	/// MamdaOrderBookChecker utility class, which periodically takes a
	/// snapshot of an order book that is also being managed by a
	/// MamdaOrderBookListener in real time.
	/// </summary>
	public class MamdaBookSelfTest
	{
		private class BookSelfTest : MamdaOrderBookCheckerHandler
		{
			public void onSuccess(MamdaOrderBookCheckType checkType)
			{
				//Successfull Check
				Console.WriteLine("Successful check ({0})", checkType);
			}
	    
			public void onInconclusive(
				MamdaOrderBookCheckType checkType,
				string reason)
			{
				//Inconclusive Check
				Console.WriteLine("OnInconclusive check ({0} - {1})", checkType, reason);
			}
	    
			public void onFailure(
				MamdaOrderBookCheckType  checkType,
				string reason,
				MamaMsg msg,
				MamdaOrderBook realTimeBook,
				MamdaOrderBook checkBook)
			{
				//Failed Check
				Console.WriteLine("Failure check ({0} - {1})", checkType, reason);
				Console.WriteLine("msg: {0}", msg);
				Console.WriteLine("Failed Current Book: ");  
				realTimeBook.dump();
				Console.WriteLine("Failed Checking Book: ");  
				checkBook.dump();
			}
		}

		public static void Main(string[] args)
		{
			CommandLineProcessor options      = new CommandLineProcessor(args);
			MamaTransport        transport    = null;
			MamaQueue            defaultQueue = null;
			MamaDictionary       dictionary   = null;
			double               intervalSecs = options.getTimerInterval();
			if (options.hasLogLevel())
			{
				myLogLevel = options.getLogLevel();
				Mama.enableLogging(myLogLevel);
			}
	        
			try
			{
				myBridge = new MamaBridge(options.getMiddleware());
				Mama.open();
	            
				transport = new MamaTransport();
				transport.create(options.getTransport(), myBridge);
				mySource = new MamaSource();
	            
				defaultQueue = Mama.getDefaultEventQueue(myBridge);

				// We might as well also enforce strict checking of order book updates
				MamdaOrderBook.setStrictChecking(true);
	            
				/*Get the Data Dictionary*/
				MamaSource dictionarySource = new MamaSource();
				dictionarySource.symbolNamespace = "WOMBAT";
				dictionarySource.transport = transport;
				dictionary = buildDataDictionary(transport, defaultQueue, dictionarySource);
				MamdaOrderBookFields.setDictionary(dictionary, null);

				if (intervalSecs == 0)
				{
					intervalSecs = 5;
				}
	            
				foreach (string symbol in options.getSymbolList())
				{
					BookSelfTest aSelfTest             = new BookSelfTest ();
					MamdaOrderBookChecker aBookChecker = new MamdaOrderBookChecker(
						transport,
						defaultQueue,
						aSelfTest,
						options.getSource(),
						symbol,
						intervalSecs);
                    obookCheckers.Add(aBookChecker);
                    
				}

				Mama.start(myBridge);
				GC.KeepAlive(dictionary);
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.ToString());
				Environment.Exit(1);
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

        private static ArrayList obookCheckers = new ArrayList();
		private static MamaSource mySource = null;
		private static MamaLogLevel myLogLevel = MamaLogLevel.MAMA_LOG_LEVEL_NORMAL;
		private static MamaBridge myBridge = null;
	}
}
