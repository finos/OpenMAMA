/* $Id: MamaSymbolListSubscriberCS.cs,v 1.1.2.3 2012/09/07 07:44:52 ianbell Exp $
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
using System.IO;
using System.Collections;
using System.Threading;

using Wombat;

namespace MamaSymbolListSubscriber
{
	class MamaSymbolListSubscriber
	{
		private static MamaTransport    transport;
		private static MamaTransport    myDictTransport;
		private static string           myMiddleware        = "wmw";
		private static MamaBridge       myBridge            = null;
		private static MamaQueue        myDefaultQueue      = null;

		private static MamaDictionary   dictionary;
		private static string           dictSource          = "WOMBAT";
		private static bool				dictionaryComplete  = false;

		private static string           transportName       = "internal";
		private static string           myDictTportName     = null;
		private static int              quietness           = 0;

		private static int              numThreads          = 1;
		private static MamaQueueGroup   queueGroup          = null;
		
		private static ArrayList		subscriptions	     = new ArrayList();
		private static ArrayList		subjectList	         = new ArrayList();

		private static string           mySymbolNamespace   = null;

		private static SubscriptionCallback subscriptionCallback
			= new SubscriptionCallback();
		private static SymbolListSubscriptionCallback symbolListSubscriptionCallback
			= new SymbolListSubscriptionCallback();

		private static MamaLogLevel		mamaLogLevel		= MamaLogLevel.MAMA_LOG_LEVEL_WARN;
		private static MamaSource       mySource            = null;
		private static MamaSource       myDictSource        = null;

		public static void Main (string[] args)
		{
			parseCommandLine (args);

			try
			{
				initializeMama ();
				buildDataDictionary ();

				/* Subscribe to the symbol list */
				MamaSubscription symbolListSubscription = new MamaSubscription();
				symbolListSubscription.setSubscriptionType (mamaSubscriptionType.MAMA_SUBSC_TYPE_SYMBOL_LIST);
				symbolListSubscription.create (myDefaultQueue,
					symbolListSubscriptionCallback,
					mySource,
					"Symbol List",
					null);

				Console.WriteLine( "Type CTRL-C to exit." );
				Mama.start (myBridge);
			}
			catch (Exception e)
			{
				Console.Error.WriteLine(e.ToString());
				Environment.Exit(1);
			}
		}

		private static void subscribeToSubjects()
		{
			int howMany = 0;

			queueGroup = new MamaQueueGroup (myBridge, numThreads);

			/* Subscribe to all symbols parsed from the symbol list */
			for (IEnumerator enumerator = subjectList.GetEnumerator(); enumerator.MoveNext();)
			{
				string symbol = (string) enumerator.Current;

				MamaSubscription subscription = new MamaSubscription ();

				subscription.create (
					queueGroup.getNextQueue (),
					subscriptionCallback,
					mySource,
					symbol,
					null);

                subscriptions.Add (subscription);
				if (++howMany % 1000 == 0)
				{
					Console.WriteLine ("Subscribed to " + howMany + " symbols.");
				}
			}
		}

		private static MamaDictionary buildDataDictionary()
		{
			bool[] gotDict = new bool[] { false };
			MamaDictionaryCallback dictionaryCallback = new DictionaryCallback(gotDict);
			lock (dictionaryCallback)
			{
				MamaSubscription subscription = new MamaSubscription ();

				MamaDictionary dictionary = new MamaDictionary();
				dictionary.create(
					myDefaultQueue,
					dictionaryCallback,
					myDictSource,
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

		private static void initializeMama()
		{
			Mama.enableLogging(mamaLogLevel);

			try
			{
				myBridge = Mama.loadBridge (myMiddleware);
				/* Always the first API method called. Initialized internal API
				 * state*/
				Mama.open ();
				myDefaultQueue = Mama.getDefaultEventQueue (myBridge);
			}
			catch (Exception e)
			{
				Console.WriteLine(e.ToString());
				Console.WriteLine("Failed to initialize MAMA");
				Environment.Exit (1);
			}

			transport = new MamaTransport ();
			/* The name specified here is the name identifying properties in the
			 * mama.properties file*/
			transport.create (transportName, myBridge);

			if (myDictTportName != null)
			{
				myDictTransport = new MamaTransport ();
				myDictTransport.create (myDictTportName, myBridge);
			}
			else 
			{
				myDictTransport = transport;
			}

			/*MamaSource for all subscriptions*/
			mySource     = new MamaSource ();
			mySource.transport = transport;
			mySource.symbolNamespace = mySymbolNamespace;

			/*MamaSource for dictionary subscription*/
			myDictSource = new MamaSource ();
			myDictSource.transport = myDictTransport;
			myDictSource.symbolNamespace = dictSource;
		}
 
		private static void parseCommandLine (string[] args)
		{
			for(int i = 0; i < args.Length;)
			{
				if (args[i] == "-source" || args[i] == "-S")
				{
					mySymbolNamespace = args[i +1];
					i += 2;
				}
				else if (args[i] == "-d" || args[i] == "-dict_source")
				{
					dictSource = args[i + 1];
					i += 2;
				}
				else if (args[i] == "-dict_tport")
				{
					myDictTportName = args[i + 1];
					i += 2;
				}
				else if (args[i] == "-tport")
				{
					transportName = args[i + 1];
					i += 2;
				}
				else if (args[i] == "-threads")
				{
					numThreads = Convert.ToInt32 (args[i+1]);
					i += 2;
				}
				else if (args[i] == "-q")
				{
					quietness++;
					i++;
				}
				else if (args[i] == ("-v"))
				{
					switch (mamaLogLevel)
					{
						case MamaLogLevel.MAMA_LOG_LEVEL_WARN:
							mamaLogLevel = MamaLogLevel.MAMA_LOG_LEVEL_NORMAL;
							break;
						case MamaLogLevel.MAMA_LOG_LEVEL_NORMAL:
							mamaLogLevel = MamaLogLevel.MAMA_LOG_LEVEL_FINE;
							break;
						case MamaLogLevel.MAMA_LOG_LEVEL_FINE:
							mamaLogLevel = MamaLogLevel.MAMA_LOG_LEVEL_FINER;
							break;    
						case MamaLogLevel.MAMA_LOG_LEVEL_FINER:
							mamaLogLevel = MamaLogLevel.MAMA_LOG_LEVEL_FINEST;
							break;
					}
					i++;
				}
				else if (args[i] == ("-m"))
				{
					myMiddleware = args[i + 1];
					i += 2;
				}
			}
		}

		/*Class for processing all event callbacks for all subscriptions*/
		private class SubscriptionCallback : MamaSubscriptionCallback
		{
			public void onMsg (MamaSubscription subscription, MamaMsg msg)
			{
				if (quietness < 1)
				{
					Console.WriteLine("Recieved message of type " + msg.stringForType (msg.getType()) + " for " + subscription.subscSymbol);
				}
			}

			/*Invoked once the subscrption request has been dispatched from the
			 * throttle queue.*/
			public void onCreate (MamaSubscription subscription)
			{
				
			}

			/*Invoked if any errors are encountered during subscription processing*/
			public void onError(MamaSubscription	  subscription,
								MamaStatus.mamaStatus status,
								string			   	  subject)
			{
				Console.WriteLine ("Symbol=[" + subscription.subscSymbol + "] : " +
					"An error occurred creating subscription: " +
					MamaStatus.stringForStatus (status));

			}

			/*Invoked if the quality status for the subscription changes*/
			public void onQuality (MamaSubscription subscription, mamaQuality quality, string symbol)
			{
				Console.WriteLine	( subscription.subscSymbol +
					": quality is now " +
					(int)quality);
			}

			public void onGap (MamaSubscription subscription)
			{
				Console.WriteLine (subscription.subscSymbol + ": gap detected ");
			}

            public void onDestroy(MamaSubscription subscription)
            {
            }

			public void onRecapRequest (MamaSubscription subscription)
			{
				Console.WriteLine (subscription.subscSymbol + ": recap requested ");
			}
		}
    
		private class SymbolListSubscriptionCallback : MamaSubscriptionCallback
		{
			public void onMsg (MamaSubscription subscription, MamaMsg msg)
			{
				switch (msg.getType())
				{
					case mamaMsgType.MAMA_MSG_TYPE_INITIAL:
						char[] delimiter = {','};
						string newSymbols = msg.getString (MamaReservedFields.SymbolList.getName(), (ushort)MamaReservedFields.SymbolList.getFid());
						string[] parsedSymbols = newSymbols.Split(delimiter);
						for (int i=0; i<parsedSymbols.Length; i++)
						{
							subjectList.Add ((Object)parsedSymbols[i]);
						}   
						Console.WriteLine("Received initial for symbol list subscription.  Updating list of symbols to subscribe to.\n");
						break;
					case mamaMsgType.MAMA_MSG_TYPE_END_OF_INITIALS:
						Console.WriteLine("End of initials.  Subscribing to symbols.\n");
						subscribeToSubjects();
						break;
					default:
						break;
				}
			}

			public void onCreate (MamaSubscription subscription)
			{
				if (quietness < 1)
				{
					Console.WriteLine("Created symbol list subscription.\n");
				}    
			}

			public void onError (MamaSubscription		subscription,
								 MamaStatus.mamaStatus	status,
								 string					subject)
			{
				Console.WriteLine("Subscription error:" + subject);  
			}

			/*Invoked if the quality status for the subscription changes*/
			public void onQuality (MamaSubscription subscription, mamaQuality quality, string symbol)
			{
				Console.WriteLine( "Symbol list subscription: quality is now " +
					(int)quality);
			}

			public void onGap (MamaSubscription subscription)
			{
				Console.WriteLine (subscription.subscSymbol + ": gap detected ");
			}

			public void onRecapRequest (MamaSubscription subscription)
			{
				Console.WriteLine (subscription.subscSymbol + ": recap requested ");
			}

            public void onDestroy(MamaSubscription subscription)
            {
            }
		}
	}
}
		

