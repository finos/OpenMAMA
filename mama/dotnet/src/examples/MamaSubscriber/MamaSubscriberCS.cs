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

namespace Wombat
{
	/// <summary>
    /// Example of how to subscribe and process messages from a basic subscription
    /// </summary>
    public class MamaSubscriberCS: 
        MamaTransportCallback, 
        MamaSubscriptionCallback
	{
        private string middlewareName = "wmw";
		private string transportName = "sub";
        private string topicName = "MAMA_TOPIC";
        
        private MamaLogLevel mamaLogLevel = MamaLogLevel.MAMA_LOG_LEVEL_WARN;
		private MamaBridge mamaBridge;
        private MamaTransport mamaTransport;
		private MamaQueue mamaDefaultQueue;
        private MamaSubscription mamaSubscription;
        private ArrayList mamaSubscriptions = new ArrayList();
        private ArrayList myMethods = new ArrayList();
        
        const string usage =
            "Example of how to subscribe and process messages from a basic subscription \n" +
            "Command line arguments:\n" +
            "      [-s topic]         The topic to which to subscribe. Default value\n" +
            "                         is 'MAMA_TOPIC'\n" +
            "      [-m name]          The middleware name to the used. Default value is 'wmw'\n" +
            "      [-tport name]      The transport parameters to be used from\n" +
            "                         mama.properties. Default is 'sub'\n" +
            "      [-v]               Increase MAMA verbosity. Can be passed multiple times\n";
        
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
						middlewareName = args[++i];
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
        
        private MamaSubscriberCS(string[] args)
        {
            try 
            {
                if (parseCommandLine(args))
                {                    
					mamaBridge = Mama.loadBridge(middlewareName);
					
					Mama.open();
         
                    mamaTransport = new MamaTransport();
                    mamaTransport.setTransportCallback(this);
                    mamaTransport.create(transportName, mamaBridge);

					mamaDefaultQueue = Mama.getDefaultEventQueue(mamaBridge);

                    mamaSubscription = new MamaSubscription();
                    mamaSubscription.createBasic(
                        mamaTransport,
                        mamaDefaultQueue,
                        this,   // callback
                        topicName);
                
                    Console.WriteLine("Starting mama...");
                    Mama.start(mamaBridge);
                    
                    // Prevent over-enthusiastic GC from reclaiming transport object
                    GC.KeepAlive(mamaTransport);
                }
            } 
            catch (Exception e) 
            {
                Console.WriteLine(e.Message);
            }
        }

	    
        [STAThread]
        static void Main(string[] args)
        {  
            MamaSubscriberCS mamaSubscriber = new MamaSubscriberCS(args);
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

                MamaMsgField.getAsString() will return a stringified version 
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
