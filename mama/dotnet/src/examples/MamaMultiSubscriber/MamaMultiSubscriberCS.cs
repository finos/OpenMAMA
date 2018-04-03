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

namespace Wombat
{
	public class Dispatcher
	{
		private MamaBridge myBridge;

		public Dispatcher (MamaBridge bridge)
		{
			myBridge = bridge;
		}

		public void dispatch ()
		{
			Mama.start (myBridge);
		}
	}

	/// <summary>
	///	Example of how to subscribe and process messages using multiple bridges
	///	<summary>
	public class MamaMultiSubscriberCS:
		MamaTransportCallback,
		MamaSubscriptionCallback
	{
		private ArrayList	middlewareNames = new ArrayList();
		private string		transportName	= "sub";
		private string		topicName		= "MAMA_TOPIC";

		private MamaLogLevel		mamaLogLevel		= MamaLogLevel.MAMA_LOG_LEVEL_WARN;
		private	ArrayList			mamaBridges			= new ArrayList();
		private ArrayList			mamaTransports		= new ArrayList();
		private ArrayList			mamaSubscriptions	= new ArrayList();

		const string usage =
			"This sample application demonstrates how to subscribe and process\n" +
			"mamaMsg's from a basic subscription using one or more middlewares at once.\n" +
			"If using more than one middleware then the same transport must be defined\n" +
			"for each.\n\n" +
			" It accepts the following command line arguments:\n" +
			"      [-s topic]         The topic to which to subscribe. Default value\n" +
			"                         is \"MAMA_INBOUND_TOPIC\".\n" +
			"      [-tport name]      The transport parameters to be used from\n" +
			"                         mama.properties. Default is sub\n" +
			"      [-q]               Quiet mode. Suppress output.\n" +
			"      [-m  middleware]    The one or more middlewares to use [wmw/lbm/tibrv].\n" +
			"      [-v]               Increase verbosity. Can be passed multiple times\n";

		private bool parseCommandLine(string[] args)
		{
			for (int i = 0; i < args.Length;i++)
			{
				if (args[i].CompareTo("-s") == 0)
				{
					if ((i +1) < args.Length)
					{
						topicName = args[++i];
						continue;
					}
				}

				if (args[i].CompareTo("-m") == 0)
				{
					if ((i +1) < args.Length)
					{
						middlewareNames.Add (args[++i]);
						continue;
					}
				}

				if (args[i].CompareTo("-tport") == 0)
				{
					if ((i +1) < args.Length)
					{
						transportName = args[++i];
						continue;
					}
				}

				if (args[i].CompareTo("--help") == 0)
				{
					Console.WriteLine(usage);
					return false;
				}

				if (args[i].CompareTo("-?") == 0)
				{
					Console.WriteLine(usage);
					return false;
				}

				if (args[i].CompareTo ("-v") == 0)
				{
					if (mamaLogLevel == MamaLogLevel.MAMA_LOG_LEVEL_WARN)
					{
						mamaLogLevel = MamaLogLevel.MAMA_LOG_LEVEL_NORMAL;
						Mama.enableLogging (mamaLogLevel);
					}
					else if (mamaLogLevel == MamaLogLevel.MAMA_LOG_LEVEL_NORMAL)
					{
						mamaLogLevel = MamaLogLevel.MAMA_LOG_LEVEL_FINE;
						Mama.enableLogging (mamaLogLevel);
					}
					else if (mamaLogLevel == MamaLogLevel.MAMA_LOG_LEVEL_FINE)
					{
						mamaLogLevel = MamaLogLevel.MAMA_LOG_LEVEL_FINER;
						Mama.enableLogging (mamaLogLevel);
					}
					else
					{
						mamaLogLevel = MamaLogLevel.MAMA_LOG_LEVEL_FINEST;
						Mama.enableLogging (mamaLogLevel);
					}
					continue;
				}

				Console.WriteLine("Error: unrecognized parameter: " + args[i]);
				return false;
			}

			return true;
		}

		private MamaMultiSubscriberCS(string[] args)
		{
			try
			{
				if (parseCommandLine(args))
				{
					IEnumerator iterator = middlewareNames.GetEnumerator();

					while (iterator.MoveNext())
					{
						MamaBridge bridge = Mama.loadBridge ((string)iterator.Current);
						mamaBridges.Add(bridge);
					}

					Mama.open();

					iterator = mamaBridges.GetEnumerator();

					while (iterator.MoveNext())
					{
						createSubscriber ((MamaBridge)iterator.Current);
					}
				}
			}
			catch (Exception e)
			{
				Console.WriteLine (e.Message);
			}
		}

		private void createSubscriber (MamaBridge bridge)
		{
			MamaSubscription mamaSubscription = new MamaSubscription();
			MamaTransport	 mamaTransport	  = new MamaTransport();
			MamaQueue		 mamaDefaultQueue = Mama.getDefaultEventQueue(bridge);
			Dispatcher		 dispatcher		  = new Dispatcher(bridge);

			mamaTransport = new MamaTransport();
			mamaTransport.setTransportCallback(this);
			mamaTransport.create(transportName, bridge);

			mamaSubscription = new MamaSubscription();
			mamaSubscription.createBasic(
				mamaTransport,
				mamaDefaultQueue,
				this,   // callback
				topicName);

			Console.WriteLine("Starting mama...");

			mamaSubscriptions.Add(mamaSubscription);
			mamaTransports.Add(mamaTransport);

			Thread startThread = new Thread (new ThreadStart(dispatcher.dispatch));
			startThread.Start();
		}

		[STAThread]
		static void Main(string[] args)
		{
			MamaMultiSubscriberCS mamaMultiSubscriber = new MamaMultiSubscriberCS(args);
		}

		private void displayField(MamaMsgField field)
			   {
				   Console.Write(String.Format("{0,20}{1,20}{2,20}",
					   field.getName(),
					   field.getFid(),
					   field.getTypeName()));

				   /*
					   The most efficient way of extracting data while iterating
					   fields is to obtain the field type and then call the
					   associated strongly-typed accessor.

					   MamaMsgField.getAsString() will return a string version
					   of the data but is considerably less efficient and is not
					   recommended for production use.
				   */

				   switch(field.getType())
				   {
					   case mamaFieldType.MAMA_FIELD_TYPE_MSG:
						   MamaMsg myMsg = field.getMsg();
						   Console.WriteLine(" {");
						   displayAllFields(myMsg);
						   Console.WriteLine("}");
						   break;
					   default:
						   Console.WriteLine(String.Format("{0,20}",field.getAsString()));
						   break;
				   }
                   Console.Out.Flush();
			   }

		private void displayAllFields(MamaMsg msg)
		{
			MamaMsgIterator iterator = new MamaMsgIterator(null);
			MamaMsgField field;
			msg.begin(ref iterator);
			while ((field = iterator.getField()) != null)
			{
				displayField(field);
				iterator++;
			}
		}

		// Transport callbacks
        public void onDisconnect(MamaTransport transport)
        {
            Console.WriteLine("Transport disconnected.");
        }

        public void onReconnect(MamaTransport transport)
        {
            Console.WriteLine("Transport Reconnected.");
        }

        public void onQuality(MamaTransport transport)
        {
            Console.WriteLine("Transport Quality.");
        }

        public void onConnect(MamaTransport transport)
        {
            Console.WriteLine("Transport Connect");
        }

        public void onAcceptConnect(MamaTransport transport)
        {
            Console.WriteLine("Transport Accept Connect");
        }

        public void onAcceptReconnect(MamaTransport transport)
        {
            Console.WriteLine("Transport Accept Reconnect.");
        }

        public void onPublisherDisconnect(MamaTransport transport)
        {
            Console.WriteLine("Transport Publisher disconnected.");
        }

        public void onNamingServiceConnect(MamaTransport transport)
        {
            Console.WriteLine("Transport Naming Service Connect.");
        }

        public void onNamingServiceDisconnect(MamaTransport transport)
        {
            Console.WriteLine("Transport Naming Service Disconnected.");
        }

		// Subscription callbacks
		public void onCreate(MamaSubscription subscription)
		{
			Console.WriteLine("onCreate, " + subscription.subscSymbol);
		}

		public void onError(MamaSubscription subscription,
			MamaStatus.mamaStatus status, string subject)
		{
			Console.WriteLine("Subscription error:" + subject);
		}

		public void onQuality(MamaSubscription subscription,
			mamaQuality quality, string symbol)
		{
			Console.WriteLine("Subscription quality:" + (int)quality);
		}

		public void onMsg(MamaSubscription subscription, MamaMsg msg)
		{
			Console.WriteLine("Received msg:");
			displayAllFields(msg);
		}

		public void onGap (MamaSubscription  subscription)
		{
			Console.WriteLine("Subscription gap");
		}

		public void onRecapRequest (MamaSubscription subscription)
		{
			Console.WriteLine("Subscription recap request");
		}

        public void onDestroy(MamaSubscription subscription)
        {
        }
	}

}
