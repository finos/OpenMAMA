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
	public class MamdaSecStatusTicker
	{
		public static void Main(string[] args)
		{
			MamaTransport        transport          = null;
            MamaTransport        dictTransport      = null;
            string               dictTransportName  = null;
			MamaQueue            defaultQueue       = null;
			MamaDictionary       dictionary         = null;
			CommandLineProcessor options            = new CommandLineProcessor(args);
            
			if (options.hasLogLevel())
			{
				Mama.enableLogging(options.getLogLevel());
			}

			try
			{
				// Initialize MAMDA
				myBridge = new MamaBridge(options.getMiddleware());
				Mama.open();
				transport = new MamaTransport();
				myMamaSource = new MamaSource ();
                transport.create(options.getTransport(), myBridge);
				defaultQueue = Mama.getDefaultEventQueue(myBridge);
                
				MamaSource dictionarySource = new MamaSource();
				dictionarySource.symbolNamespace = "WOMBAT";
                dictTransportName = options.getDictTransport();
                if (null != dictTransportName)
                {
                    dictTransport = new MamaTransport();
                    dictTransport.create(dictTransportName, myBridge);
                }
                else
                {
                    dictTransport = transport;
                }
                dictionarySource.transport = dictTransport;
                
				/*Get the data dictionary*/
				dictionary = buildDataDictionary(dictTransport, defaultQueue, dictionarySource);
	            
				MamdaSecurityStatusFields.setDictionary(dictionary, null);

				foreach (string symbol in options.getSymbolList())
				{
					MamdaSubscription  aSubscription =  new MamdaSubscription();
					MamdaSecurityStatusListener aSecListener = new MamdaSecurityStatusListener();
					SecTicker        aTicker        = new SecTicker();

					aSecListener.addHandler(aTicker);
					aSubscription.addMsgListener(aSecListener);

					aSubscription.create(transport,
										defaultQueue,
										options.getSource (),
										symbol,
										null);
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

		private class SecTicker : MamdaSecurityStatusHandler
		{
            
			public void onSecurityStatusRecap(
                MamdaSubscription            subscription,
                MamdaSecurityStatusListener  listener,
                MamaMsg                      msg,
                MamdaSecurityStatusRecap     recap)
            {
                Console.WriteLine ("SecStatus recap (" + subscription.getSymbol ()
                + "    Status:" + recap.getSecurityStatusStr ());
            }
            
            public void onSecurityStatusUpdate(
                MamdaSubscription            subscription,
                MamdaSecurityStatusListener  listener,
                MamaMsg                      msg,
                MamdaSecurityStatusUpdate    update,
                MamdaSecurityStatusRecap     recap)
            {
                Console.WriteLine ("SecStatus update (" + subscription.getSymbol ()
                 + "    Status:" + listener.getSecurityStatusStr ());
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

		private static MamaSource myMamaSource = null;
		private static MamaBridge myBridge = null;
	}
}

