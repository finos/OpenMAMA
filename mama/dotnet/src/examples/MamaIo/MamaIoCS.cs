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
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Diagnostics;
using System.Globalization;

namespace Wombat
{
	/// <summary>
	/// This sample application demonstrates how to use mamaIoHandlers. It creates a
	/// socket an waits for a connection. Upon accepting a connection it creates a
	/// mamaIoHandler to read from the socket and connects back to the client on port
	/// 107 (telnet). It echos the users input. Type "quit" to exit.
	///
	/// It accepts the following command line arguments:
	///      [-m[iddleware] name]   The middleware to use. Defaults to wmw
	///      [-p[ort] number]       The TCP/IP port on which to listen. Defaults to 9998
	///      [-q]                   Quiet mode. Suppress output.
	/// </summary>
	class EntryPoint
	{
		[STAThread]
		static int Main(string[] args)
		{
			try
			{
				return new MamaIoCS(args).Run();
			}
			catch (Exception e)
			{
				Debug.WriteLine(String.Format("Error occurred: {0}\r\nDetails:\r\n{1}",
						e.Message,
						e.ToString()));
				Console.WriteLine("Error occurred: {0}\r\nDetails:\r\n{1}",
					e.Message,
					e.ToString());
				return 1;
			}
		}
	}

	class MamaIoCS
	{
		public MamaIoCS(string[] args)
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
			WaitForConnection();
			CreateIoHandlers();

			Mama.start(bridge);

			readHandler.destroy();
			writeHandler.destroy();
			exceptHandler.destroy();

			client.Shutdown(SocketShutdown.Both);
			client.Close();

			return 0;
		}

		private void InitializeMama()
		{
			try
			{
				Mama.logToFile(@"mama.log", MamaLogLevel.MAMA_LOG_LEVEL_FINEST);
				bridge = Mama.loadBridge(middleware);
				Mama.open();
				defaultQueue = Mama.getDefaultEventQueue(bridge);
			}
			catch (MamaException e)
			{
				Console.WriteLine("Error initializing mama: {0}", e.ToString());
				Exit(1);
			}
		}

		private void WaitForConnection()
		{
			if (!quiet)
			{
				Console.WriteLine("Creating socket on port {0}", port);
			}
			acceptor = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.IP);
			acceptor.Bind(new IPEndPoint(IPAddress.Any, port));

			Console.WriteLine("Waiting for a connection on port {0}", port);

			acceptor.Listen(1);
			client = acceptor.Accept();

			if (!quiet)
			{
				Console.WriteLine("Accepting connection from {0}", client.RemoteEndPoint);
			}

			string text = "Type \"quit\" to stop server.\r\n";
			byte [] data = Encoding.ASCII.GetBytes(text);
			client.Send(data);
		}

		private void CreateIoHandlers()
		{
			try
			{
				uint descriptor = (uint)client.Handle;
				readHandler = new MamaIo();
				readHandler.create(defaultQueue, new ReadIoCallback(this), descriptor, mamaIoType.MAMA_IO_READ);
				writeHandler = new MamaIo();
				writeHandler.create(defaultQueue, new WriteIoCallback(this), descriptor, mamaIoType.MAMA_IO_WRITE);
				exceptHandler = new MamaIo();
				exceptHandler.create(defaultQueue, new ExceptIoCallback(this), descriptor, mamaIoType.MAMA_IO_EXCEPT);
			}
			catch (MamaException e)
			{
				Console.WriteLine("Error creation IO Handler: {0}", e.ToString());
				Exit(1);
			}
		}

		private static void Exit(int errorCode)
		{
			Environment.ExitCode = errorCode;
			Process.GetCurrentProcess().Kill();
		}

		private abstract class IoCallbackBase : MamaIoCallback
		{
			protected IoCallbackBase(MamaIoCS example)
			{
				example_ = example;
			}
			public abstract void onIo(MamaIo io, mamaIoType ioType);
			protected MamaIoCS example_;
		}

		private sealed class ReadIoCallback : IoCallbackBase
		{
			public ReadIoCallback(MamaIoCS example) : base(example)
			{
			}
			public override void onIo(MamaIo io, mamaIoType ioType)
			{
				Socket sock = example_.client;

				byte [] buffer = new byte[1024];
				int len = sock.Receive(buffer, 1023, SocketFlags.None);
				buffer[len] = (byte)'\0';

				string text = Encoding.ASCII.GetString(buffer, 0, len);
				if (!example_.quiet)
				{
					Console.WriteLine("READ: {0}", text);
				}

				// should test for "quit", but Windows' telnet client won't send it
				// no matter if "set mode stream" was used and would send only partial
				// substrings, such as "q", then "uit", "qu", then "it", etc.
				if (text.StartsWith("q"))
				{
					Console.WriteLine("QUITING");
					Mama.stop(MamaIoCS.bridge);
					return;
				}

				sock.Send(buffer, len, SocketFlags.None);
			}
		}

		private sealed class WriteIoCallback : IoCallbackBase
		{
			public WriteIoCallback(MamaIoCS example) : base(example)
			{
			}

			// called too many times, debug output disabled
			public override void onIo(MamaIo io, mamaIoType ioType)
			{
			}
		}

		private sealed class ExceptIoCallback : IoCallbackBase
		{
			public ExceptIoCallback(MamaIoCS example) : base(example)
			{
			}
			public override void onIo(MamaIo io, mamaIoType ioType)
			{
				if (!example_.quiet)
				{
					Console.WriteLine("EXCEPT");
				}
			}
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
					case "m":
					case "middleware":
						if ((i + 1) == args.Length)
						{
							Console.WriteLine("Expecting name after {0}", arg);
							++i;
							continue;
						}
						middleware = args[++i];
						break;

					case "p":
					case "port":
						if ((i + 1) == args.Length)
						{
							Console.WriteLine("Expecting port after {0}", arg);
							++i;
							continue;
						}
						try
						{
							port = int.Parse(args[++i], NumberStyles.Integer, CultureInfo.InvariantCulture);
						}
						catch // ignore parse error
						{
						}
						break;
					case "h":
					case "?":
						helpNeeded = true;
						return;
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
				Console.WriteLine("Starting IO with:\n" +
						"   port:               {0}", port);
			}
		}

		private void DisplayUsage()
		{
			Console.WriteLine(usage_);
		}

		private string[] args;
		private string middleware = "wmw";
		private int port = 9998;
		private MamaLogLevel logLevel = MamaLogLevel.MAMA_LOG_LEVEL_WARN;
		private bool helpNeeded = false;
		private MamaIo readHandler, writeHandler, exceptHandler;
		private static MamaBridge bridge;
		private MamaQueue defaultQueue;
		internal bool quiet = false;
		internal Socket acceptor, client;

		private const string usage_ = @"
This sample application demonstrates how to use mamaIoHandlers. It creates a
socket an waits for a connection. Upon accepting a connection it creates a
mamaIoHandler to read from the socket and connects back to the client on port
107 (telnet). It echos the users input. Type ""quit"" to exit.

It accepts the following command line arguments:
     [-p[ort] number]   The TCP/IP port on which to listen. Defaults to 9998
     [-q]               Quiet mode. Suppress output.
";
	}
}
