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

namespace Wombat
{
	class MamaFtMemberCS : MamaFtMemberCallback,
		MamaTimerCallback
	{
		private MamaLogLevel mamaLogLevel = MamaLogLevel.MAMA_LOG_LEVEL_WARN;
		private static string usageString = "MamaFtMember - Simple fault tolerance example. Demonstrates use of the MAMA FT API.";

		public MamaFtMemberCS()
		{
			myState = mamaFtState.MAMA_FT_STATE_UNKNOWN;
			myTransport = null;
			myWeight = 50;
			myGroup = null;
		}

		public void onFtStateChange(
			MamaFtMember member,
			string groupName,
			mamaFtState state,
			object closure)
		{
			myState = state;
			printState ("FT Notification");
		}

		public void onTimer (MamaTimer timer, object closure)
		{
			printState ("Timer");
		}

        public void onDestroy(MamaTimer mamaTimer, object closure)
        {
        }

		public void init()
		{
			myDefaultQueue = Mama.getDefaultEventQueue (myBridge);
			myTimer.create (myDefaultQueue, this, 2.0, null);  // two seconds
			myFtMember.create();
			myFtMember.setup (myDefaultQueue, this, myTransport, myGroup, myWeight, 1.0, 6.0, null);
			myFtMember.activate();
		}

		public void printState (string context)
		{
			Console.WriteLine ("(" + context + ") Current state is: " + MamaFtMember.mamaFtStateToString (myState));
		}

		private void parseCommandLine (string[] args)
		{
			string tport = null;
			uint ft_type = (uint)mamaFtType.MAMA_FT_TYPE_MULTICAST;
			bool displayVersion = false;
			for (int i = 0; i < args.Length; )
			{
				if ((args[i].CompareTo ("-h")) == 0 ||
					(args[i].CompareTo ("-?")) == 0)
				{
					usage (0);
				}

				if ((args[i].CompareTo ("-g") == 0) ||
					(args[i].CompareTo ("-group") == 0))
				{
					myGroup = args[i+1];
					i += 2;
					continue;
				}

				if ((args[i].CompareTo ("-w") == 0) ||
					(args[i].CompareTo ("-weight") == 0))
				{
					myWeight = Convert.ToUInt32 (args[i+1]);
					i += 2;
					continue;
				}

				if (args[i].CompareTo ("-m") == 0)
				{
					myMiddleware = args[i+1];
					i += 2;
					continue;
				}

				if ((args[i].CompareTo ("-t") == 0) ||
					(args[i].CompareTo ("-tport") == 0))
				{
					tport = args[i+1];
					i += 2;
					continue;
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
					i++;
					continue;
				}

				if (args[i].CompareTo ("-version") == 0)
				{
					displayVersion = true;
					break;
				}

				if (args[i].CompareTo ("-b") == 0)
				{
					ft_type = (uint)mamaFtType.MAMA_FT_TYPE_BRIDGE;
					continue;
				}

                i++;
			}

			myBridge = Mama.loadBridge(myMiddleware);

			Mama.open();

			if (displayVersion)
			{
				Console.WriteLine (Mama.getVersion (myBridge));
				Exit (0);
			}

			myTimer = new MamaTimer();

			myTransport = new MamaTransport();
			myTransport.create(tport, myBridge);

			if (myGroup == null)
			{
				Console.WriteLine ("No FT group name specified");
				usage(1);
			}

			switch (ft_type)
			{
				case (uint)mamaFtType.MAMA_FT_TYPE_MULTICAST:
					myFtMember = new MamaMulticastFtMember ();
					break;

				case (uint)mamaFtType.MAMA_FT_TYPE_BRIDGE:
					myFtMember = new MamaBridgeFtMember ();
					break;

				default:
					Console.WriteLine ("No FT type specified");
					usage(1);
					break;
			}
		}

		private void usage (int exitStatus)
		{
			Console.WriteLine (usageString);
			Exit(exitStatus);
		}

		private MamaFtMember  myFtMember;
		private MamaTimer     myTimer;
		private mamaFtState   myState;
		private string        myMiddleware = "wmw";
		private MamaBridge    myBridge;
		private MamaQueue     myDefaultQueue;
		private MamaTransport myTransport;
		private uint          myWeight;
		private string        myGroup;

		private static void Exit (int errorCode)
		{
			Environment.ExitCode = errorCode;
			Process.GetCurrentProcess().Kill();
		}

		[STAThread]
		static void Main (string[] args)
		{  
		    try
			{
				MamaFtMemberCS ftDemo = new MamaFtMemberCS();
				ftDemo.parseCommandLine (args);

				ftDemo.init ();

				Mama.start (ftDemo.myBridge);
			}
			catch (Exception e)
			{
				Console.WriteLine (e.Message);
			}
		}
	}
}
