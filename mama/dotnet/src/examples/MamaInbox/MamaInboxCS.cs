/* $Id: MamaInboxCS.cs,v 1.1.2.5 2012/08/24 16:12:02 clintonmcdowell Exp $
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

namespace Wombat
{
	/// <summary>
	/// This sample application demonstrates how to send mamaMsg's from an inbox,
	/// and receive the reply.
	/// 
	/// It accepts the following command line arguments: 
	/// [-s topic]         The topic on which to send the request. Default value
	/// is "MAMA_INBOUND_TOPIC".
	/// [-tport name]      The transport parameters to be used from
	/// mama.properties. 
	/// [-q]               Quiet mode. Suppress output.
	/// </summary>
	class EntryPoint
	{
		[STAThread]
		static int Main(string[] args)
		{
			try
			{
				return new MamaInboxCS(args).Run();
			}
			catch (Exception e)
			{
				Console.WriteLine("Error occurred: {0}\r\nDetails:\r\n{1}",
					e.Message,
					e.ToString());
				return 1;
			}
		}
	}

	class MamaInboxCS
	{
		public MamaInboxCS(string[] args)
		{
			this.args = args;
		}

		public int Run()
		{
			ParseArgs();
			if (helpNeeded)
			{
				DisplayUsage();
				return 0;
			}

			InitializeMama();
			CreatePublisher();
			CreateInbox();

			for (int i = 0; i < 1000; ++i)
			{
				SendRequest();
			}

			Mama.start(bridge);

			return 0;
		}

		private void InitializeMama()
		{
			try
			{
				Mama.logToFile(@"mama.log", MamaLogLevel.MAMA_LOG_LEVEL_FINEST);
				bridge = new MamaBridge(middlewareName);
				Mama.open();
				transport = new MamaTransport();
				transport.create(transportName, bridge);
				defaultQueue = Mama.getDefaultEventQueue(bridge);
			}
			catch (MamaException e)
			{
				Console.WriteLine("Error initializing mama or creating transport: {0}", e.ToString());
				Exit(1);
			}
		}

		private void CreatePublisher()
		{
			try
			{
				publisher = new MamaPublisher();
				publisher.create(transport, inboundTopic);
				sendCompleteCallback = new SendCompleteCallback();
			}
			catch (MamaException e)
			{
				Console.WriteLine("Error creating publisher: {0}", e.ToString());
				Exit(1);
			}
		}

		private void CreateInbox()
		{
			try
			{
				inbox = new MamaInbox();
				inboxCallback = new InboxCallback();
				inbox.create(transport, defaultQueue, inboxCallback);
			}
			catch (MamaException e)
			{
				Console.WriteLine("Error creating inbox: {0}", e.ToString());
				Exit(1);
			}
		}

		private void SendRequest()
		{
			try
			{
				MamaMsg msg = new MamaMsg();
				msg.addI32 ("field", 1, 32);

				publisher.sendFromInboxWithThrottle(inbox, msg, sendCompleteCallback, null);

				GC.KeepAlive(msg);
			}
			catch (MamaException e)
			{
				Console.WriteLine("Error sending request: {0}", e.ToString());
				Exit(1);
			}
		}

		private static void Exit(int errorCode)
		{
			Environment.ExitCode = errorCode;
			Process.GetCurrentProcess().Kill();
		}

		private void ParseArgs()
		{
			for (int i = 0; i < args.Length; )
			{
				string arg = args[i];
				if (arg[0] != '-')
				{
					Console.WriteLine("Ignoring invalid argument {0}", arg);
					++i;
					continue;
				}
				string opt = arg.Substring(1);
				switch (opt)
				{
					case "s":
						if ((i + 1) == args.Length)
						{
							Console.WriteLine("Expecting inbound topic name after {0}", arg);
							++i;
							continue;
						}
						inboundTopic = args[++i];
						break;
					case "h":
					case "?":
						helpNeeded = true;
						return;
					case "m":
						if ((i + 1) == args.Length)
						{
							Console.WriteLine("Expecting middleware name after {0}", arg);
							++i;
							continue;
						}
						middlewareName = args[++i];
						break;
					case "tport":
						if ((i + 1) == args.Length)
						{
							Console.WriteLine("Expecting transport name after {0}", arg);
							++i;
							continue;
						}
						transportName = args[++i];
						break;
					case "q":
						quiet = true;
						break;
					case "v":
						if (logLevel == MamaLogLevel.MAMA_LOG_LEVEL_WARN)
						{
							logLevel = MamaLogLevel.MAMA_LOG_LEVEL_NORMAL;
							Mama.enableLogging (logLevel);
						}
						else if (logLevel == MamaLogLevel.MAMA_LOG_LEVEL_NORMAL)
						{
							logLevel = MamaLogLevel.MAMA_LOG_LEVEL_FINE;
							Mama.enableLogging (logLevel);
						}
						else if (logLevel == MamaLogLevel.MAMA_LOG_LEVEL_FINE)
						{
							logLevel = MamaLogLevel.MAMA_LOG_LEVEL_FINER;
							Mama.enableLogging (logLevel);
						}
						else
						{
							logLevel = MamaLogLevel.MAMA_LOG_LEVEL_FINEST;
							Mama.enableLogging (logLevel);
						}
						break;
					default:
						Console.WriteLine("Ignoring invalid option {0}", arg);
						break;
				}
				++i;
			}

			if (!quiet)
			{
				Console.WriteLine("Starting Publisher with:\n" +
					"   topic:              {0}\n" +
					"   transport:          {1}\n",
					inboundTopic,
					transportName);
			}
		}

		private void DisplayUsage()
		{
			Console.WriteLine(usage_);
		}

		private sealed class InboxCallback : MamaInboxCallback
		{
			public void onMsg(MamaInbox inbox, MamaMsg msg)
			{
				Console.WriteLine("Received reply:");
			}
			public void onError(MamaInbox inbox, MamaStatus.mamaStatus status)
			{
				Console.WriteLine("Error creating inbox: {0}", MamaStatus.stringForStatus(status));
				Exit(1);
			}
            public void onDestroy(MamaInbox inbox, object closure)
            {
            }
		};

		private sealed class SendCompleteCallback : MamaSendCompleteCallback
		{
			public void onSendComplete(
				MamaPublisher publisher,
				MamaMsg message,
				MamaStatus.mamaStatus status,
				object closure)
			{
				message.destroy();
			}
		}

		private string[] args;
		private string inboundTopic = "MAMA_INBOUND_TOPIC";
		private string middlewareName = "wmw";
		private string transportName = "sub"; // tib_rvd
		private MamaLogLevel logLevel = MamaLogLevel.MAMA_LOG_LEVEL_WARN;
		private bool helpNeeded = false;
		private bool quiet = false;
		public  static MamaBridge bridge;
		private MamaTransport transport;
		private MamaQueue defaultQueue;
		private MamaPublisher publisher;
		private MamaInbox inbox;
		private MamaInboxCallback inboxCallback;
		private SendCompleteCallback sendCompleteCallback;

		private const string usage_ = @"
This sample application demonstrates how to send mamaMsg's from an inbox,
and receive the reply.

It accepts the following command line arguments: 
     [-s topic]         The topic on which to send the request. Default value
                        is ""MAMA_INBOUND_TOPIC"".
     [-m name]          The middleware to be used, default value is ""wmw"".
     [-tport name]      The transport parameters to be used from
                        mama.properties. 
     [-q]               Quiet mode. Suppress output.
";
	}
}
