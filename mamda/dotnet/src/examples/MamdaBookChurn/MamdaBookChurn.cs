/* $Id: MamdaBookChurn.cs,v 1.6.32.7 2012/08/24 16:12:13 clintonmcdowell Exp $
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
	public class MamdaBookChurn
	{

		public static void Main(string[] args)
		{
			try
			{
				options = new CommandLineProcessor(args);

				//Getting the log File name 
				myLogFileName = options.getLogFileName();
	            
				if (myLogFileName != null)
				{
					myOutFile = new FileStream(myLogFileName, FileMode.OpenOrCreate); 
					myOut = new StreamWriter(myOutFile);
					myOut.WriteLine("Date/Time"+ "," + "ChurnStats" +","
						+ "UpdateStats" + "," + "PeakMsgCount" + ","
						+ "RecapStats" + "," + "BookGapStats" + ","
						+"freeMemory" +"," +"Memory Used");
				}
				else
				{
					myOut = Console.Out;
				}
	            
				// Initialize MAMA
				bridge = new MamaBridge(options.getMiddleware());
				Mama.open();
				if (options.hasLogLevel())
				{
					myLogLevel = options.getLogLevel();
					Mama.enableLogging(myLogLevel);
				}

				transport = new MamaTransport();
				transport.create(options.getTransport(), bridge);

				defaultQueue = Mama.getDefaultEventQueue(bridge);

				//Get the Data dictionary.....
				MamaSource dictionarySource = new MamaSource();
                dictionarySource.symbolNamespace = options.getDictSource();
				dictionarySource.transport = transport;
				dictionary = buildDataDictionary(transport, defaultQueue, dictionarySource);
	    
				MamdaOrderBookFields.setDictionary(dictionary, null);
	            
				foreach (string symbol in options.getSymbolList())
				{
					subscribeToBooks(symbol);
				}
	            
				myChurnRate = options.getChurnRate(); 
	            
				Thread.Sleep(10000);
	            
				//Getting the TimerInterval from the cmd prompt
				myChurnInterval = options.getTimerInterval();
	            
				//Churn Timer
				if (myChurnRate > 0)
				{
					myChurnTimer = safeCreateTimer(new ChurnCallback(), myChurnInterval);
				}
	            
				//Stats Timer
				myStatsTimer = safeCreateTimer(new StatsCallback(), 1.0);
	            
				Console.WriteLine("Hit Enter or Ctrl-C to exit.");
				Mama.start(bridge);
				GC.KeepAlive(dictionary);
				Console.ReadLine();

				if (myOutFile != null)
				{
					myOut.Close();
				}
			}
			catch (Exception e)
			{
				Console.Error.WriteLine(e.ToString());
				Environment.Exit(1);
			}
		}

		private static void subscribeToBooks(string symbol)
		{
			MamdaSubscription aSubscription      = new MamdaSubscription();
			MamdaOrderBookListener aBookListener = new MamdaOrderBookListener();
			ListenerCallback callback            = new ListenerCallback();

			aBookListener.addHandler(callback);
			aSubscription.addMsgListener(aBookListener);
			aSubscription.addStaleListener(callback);
			aSubscription.setType(mamaSubscriptionType.MAMA_SUBSC_TYPE_BOOK);
        
			aSubscription.create(
				transport,
				defaultQueue,
				options.getSource(),
				symbol,
				null);
        
			//Add the Mamda Subscription to the ArrayList
			subscriptionList.Add(aSubscription);
		}

		private class ListenerCallback : MamdaOrderBookHandler, 
										 MamdaStaleListener
		{
			public void onBookRecap (
				MamdaSubscription        sub,
				MamdaOrderBookListener   listener,
				MamaMsg                  msg,
				MamdaOrderBookDelta      bookDelta,
				MamdaOrderBookRecap      bookRecap)
			{
				++myRecapStats;
			}
	        
			public void onBookDelta (
				MamdaSubscription        sub,
				MamdaOrderBookListener   listener,
				MamaMsg                  msg,
				MamdaOrderBookDelta      bookDelta,
				MamdaOrderBookRecap      bookRecap)
			{
				++myUpdateStats;
			}
	            
			public void onBookClear (
				MamdaSubscription        sub,
				MamdaOrderBookListener   listener,
				MamaMsg                  msg,
				MamdaOrderBookDelta      bookDelta,
				MamdaOrderBookRecap      fullBook)
			{
				Console.WriteLine("BookClear");
			}
	            
			public void onBookGap(
				MamdaSubscription        sub,
				MamdaOrderBookListener   listener,
				MamaMsg                  msg,
				MamdaOrderBookGap        gapEvent,
				MamdaOrderBookRecap      fullBook)
			{
				++myBookGapStats;
			}
	            
			public void onStale(
				MamdaSubscription   subscription,
				mamaQuality         quality)
			{
				Console.WriteLine("Stale ({0} - {1})", subscription.getSymbol(), quality);
			}
		}

		//ChurnCallback
		private class ChurnCallback : MamaTimerCallback
		{
			public void onTimer(MamaTimer mamaTimer, object closure)
			{
				lock (myGuard)
				{
					onTimerImpl();
				}
			}
			
			public void onDestroy(MamaTimer mamaTimer, object closure)
			{
			}

			private void onTimerImpl()
			{
				myNumToChurn = myChurnRate;
				while (myNumToChurn-- > 0)
				{
					myRandomNo = generator.Next(65536) % options.getSymbolListSize();
					//Accessing the mamda subscription object from the arrayList
					try
					{
						sub = (MamdaSubscription)subscriptionList[myRandomNo];
						subscriptionList.RemoveAt(myRandomNo);
						tempSymbol = sub.getSymbol();
						if (tempSymbol != null) 
						{
							//Deactivating the Mamda Subscription
							sub.deactivate();
							//Creating the MamdaSubscription 
							subscribeToBooks(tempSymbol);
							++myChurnStats;
						}
					}
					catch (Exception e)
					{
						Console.WriteLine("Error: {0}", e.ToString());
					}
				}
			}

			private MamdaSubscription sub   = null;
			private String      tempSymbol  = null;
			private int         myNumToChurn= 0;
			private int         myRandomNo  = 0; 
			Random              generator   = new Random();
		}

		//StatsCallback
		private class StatsCallback : MamaTimerCallback
		{
			public void onTimer(MamaTimer mamaTimer, object closure)
			{
				lock (myGuard)
				{
					onTimerImpl();
				}
			}
			
			public void onDestroy(MamaTimer mamaTimer, object closure)
			{
			}
	        
			private void onTimerImpl()
			{
				myToday = DateTime.Now;
	            
				if (myUpdateStats > myPeakMsgCount)
				{
					myPeakMsgCount = myUpdateStats;
				}

				if (myOutFile != null)
				{
					myOut.WriteLine(
						myToday.ToString() + "," + myChurnStats + "," + myUpdateStats + "," +
						myPeakMsgCount + "," + myRecapStats + "," +myBookGapStats + "," 
						+ 0/* Runtime.getRuntime().freeMemory()*/ +"," + 
						0 /*((Runtime.getRuntime().totalMemory())-(Runtime.getRuntime().freeMemory())*/);
					myOut.Flush();
				}
	                               
				Console.WriteLine(
						myToday.ToString() +" ChurnStats:" + myChurnStats +
						" UpdateStats:" + myUpdateStats + " PeakMsgStats:" + myPeakMsgCount + 
						" RecapStats:"  + myRecapStats  + " BookGap:" + myBookGapStats +
						" Free Memory:" + 0 /*(Runtime.getRuntime().freeMemory())/1024*/ +" Memory Used:" +
						0/*((Runtime.getRuntime().totalMemory())-(Runtime.getRuntime().freeMemory()))/1024*/);
	            
				myChurnStats    = 0;
				myUpdateStats   = 0;
				myRecapStats    = 0;
				myBookGapStats  = 0;
			}

			private int myPeakMsgCount = 0;
		}

		private static MamaTimer safeCreateTimer(
			MamaTimerCallback callback,
			double interval)
		{
			MamaTimer timer = null;
			try
			{
				timer = new MamaTimer();
				timer.create(defaultQueue, callback, interval, null);
			}
			catch (Exception e)
			{
				Console.Error.WriteLine(e.ToString());
				Console.Error.WriteLine("Error creating timer: {0}", e.Message);
				Environment.Exit(1);
			}
			return timer;
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

				Mama.start(bridge);
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
					Mama.stop(bridge);
					Monitor.PulseAll(this);
				}
			}

			private bool[] gotDict_;
		}

		private static MamaTimer            myChurnTimer     = null;
		private static MamaTimer            myStatsTimer     = null;
		private static MamaBridge           bridge           = null;
		private static MamaTransport        transport        = null;
		private static MamaQueue            defaultQueue     = null;
		private static MamaDictionary       dictionary       = null;
		private static CommandLineProcessor options          = null;
		private static ArrayList			subscriptionList = new ArrayList(); 
		private static int                  myChurnRate      = 0;
		internal static int                 myUpdateStats    = 0;
		internal static int                 myRecapStats     = 0;
		internal static int                 myBookGapStats   = 0;
		private static int                  myChurnStats     = 0;
		private static DateTime             myToday          = DateTime.Now;
		private static Stream               myOutFile        = null;
		private static TextWriter           myOut            = null;
		private static String               myLogFileName    = null;
		private static double               myChurnInterval  = 1.0;
		private static MamaLogLevel			myLogLevel		 = MamaLogLevel.MAMA_LOG_LEVEL_NORMAL;
		internal static object				myGuard			 = new object();

	}
}
