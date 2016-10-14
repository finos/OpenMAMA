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
using System.Diagnostics;
using System.Globalization;
using System.Threading;
using System.Collections;

namespace Wombat.Mamda.Examples
{
	public class MamdaListen
	{
        private static ArrayList mamdaSubscriptions = new ArrayList();
		public static void Main(string[] args)
		{
			MamaTransport transport = null;
			MamaQueue defaultQueue = null;
			MamaDictionary dictionary = null;
			ListenerCallback callback = new ListenerCallback();
			CommandLineProcessor options = new CommandLineProcessor(args);

			try
			{
				// Initialize MAMA
				myBridge = new MamaBridge(options.getMiddleware());
				Mama.open ();
				if (options.hasLogLevel())
				{
					Mama.enableLogging (options.getLogLevel());
				}

				transport = new MamaTransport();
				transport.create(options.getTransport(), myBridge);
				defaultQueue = Mama.getDefaultEventQueue(myBridge);
				//Get the Data dictionary.....
				MamaSource dictionarySource = new MamaSource();
                dictionarySource.symbolNamespace = options.getDictSource();
				dictionarySource.transport = transport;
				dictionary = buildDataDictionary(transport, defaultQueue, dictionarySource);

				foreach (string symbol in options.getSymbolList())
				{
					MamdaSubscription aSubscription = new MamdaSubscription ();

					aSubscription.addMsgListener(callback);
					aSubscription.addStaleListener(callback);
					aSubscription.addErrorListener(callback);

					if (options.getSnapshot())
						aSubscription.setServiceLevel(mamaServiceLevel.MAMA_SERVICE_LEVEL_SNAPSHOT, 0);

					aSubscription.create(transport,
										defaultQueue,
										options.getSource(),
										symbol,
										null);
	                mamdaSubscriptions.Add(aSubscription);
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

		private class ListenerCallback :
			MamdaMsgListener,
			MamdaStaleListener,
			MamdaErrorListener
		{
			public void onMsg(
				MamdaSubscription subscription,
				MamaMsg msg,
				mamaMsgType msgType)
			{
				Console.WriteLine("Update ({0})", subscription.getSymbol());
			}

			public void onStale(
				MamdaSubscription subscription,
				mamaQuality quality)
			{
				Console.WriteLine("Stale ({0})", subscription.getSymbol());
			}

			public void onError (
				MamdaSubscription subscription,
				MamdaErrorSeverity severity,
				MamdaErrorCode errorCode,
				string errorMessage)
			{
				Console.WriteLine("Error ({0})", subscription.getSymbol());
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
