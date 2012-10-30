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
using Wombat;
using System.Collections;

namespace Wombat.Mamda.Examples
{
	public class MamdaMultiSecurityTicker
	{
        private static ArrayList mamdaSubscriptions = new ArrayList();
		public static void Main(string[] args)
		{
			MamaTransport        transport    = null;
			MamaQueue            defaultQueue = null;
			MamaDictionary       dictionary   = null;
			CommandLineProcessor options      = new CommandLineProcessor(args);
			double               throttleRate = options.getThrottleRate();
            
            
			if (options.hasLogLevel())
			{
				logLevel_ = options.getLogLevel();
				Mama.enableLogging(logLevel_);
			}

			if ((throttleRate > 100.0) || (throttleRate <= 0.0))
			{
				throttleRate = 100.0;
			}

			try
			{
				// Initialize MAMDA
				myBridge = new MamaBridge(options.getMiddleware());
				Mama.open();

				transport = new MamaTransport();
				transport.create(options.getTransport(), myBridge);
				defaultQueue = Mama.getDefaultEventQueue(myBridge);
				//Get the Data dictionary.....
				MamaSource dictionarySource = new MamaSource();
                dictionarySource.symbolNamespace = options.getDictSource();
				dictionarySource.transport = transport;
				dictionary = buildDataDictionary(transport, defaultQueue, dictionarySource);

				//Initialise the dictionary and fields
				MamdaCommonFields.setDictionary(dictionary, null);
				MamdaQuoteFields.setDictionary(dictionary, null);
				MamdaTradeFields.setDictionary(dictionary, null);

				foreach (string symbol in options.getSymbolList())
				{
					MamdaSubscription aSubscription = new MamdaSubscription();
					aSubscription.setType(mamaSubscriptionType.MAMA_SUBSC_TYPE_GROUP);
					aSubscription.setTimeout(60.0);
					aSubscription.create(
						transport,
                        defaultQueue,
						options.getSource(),
						symbol,
						null);

					/* For each subscription a MamdaMultiSecurityicipantManager is
					* added as the message listener. The callbacks on the
					* MamdaMultiSecurityHandler will be invokes as new group members
					* become available.*/
					MamdaMultiSecurityManager multiSecurityManager =
						new MamdaMultiSecurityManager(symbol);
					multiSecurityManager.addHandler(new MultiSecurityHandler());

					aSubscription.addMsgListener(multiSecurityManager);

					aSubscription.activate();

                    mamdaSubscriptions.Add(aSubscription);
				}

				Mama.start(myBridge);
				GC.KeepAlive(dictionary);
				Console.WriteLine("Press ENTER or Ctrl-C to quit...");
				Console.ReadLine();
			}
			catch (Exception e)
			{
				Console.WriteLine(e.ToString());
				Environment.Exit(1);
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
		private class MultiSecurityHandler : MamdaMultiSecurityHandler
		{
			public void onSecurityCreate (
				MamdaSubscription           subscription,
				MamdaMultiSecurityManager   manager,
				string                      symbol)
			{
				MamdaTradeListener aTradeListener = new MamdaTradeListener();
				MamdaQuoteListener aQuoteListener = new MamdaQuoteListener();
				ComboTicker        aTicker        = new ComboTicker();

				aTradeListener.addHandler(aTicker);
				aQuoteListener.addHandler(aTicker);

				manager.addListener(aTradeListener, symbol);
				manager.addListener(aQuoteListener, symbol);
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

		private static MamaLogLevel	logLevel_ = MamaLogLevel.MAMA_LOG_LEVEL_NORMAL;
		private static object       guard_ = new object();
		private static MamaBridge	myBridge = null;
	}
}
