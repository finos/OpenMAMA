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
using System.IO;
using System.Collections.Specialized;

namespace Wombat.Mamda.Examples
{
	public class CommandLineProcessor
	{
		public CommandLineProcessor(string[] args)
		{
			for (int i = 0; i < args.Length;  )
			{
				string arg = args[i];
				if ( arg == "-S")
				{
					mSource = args[i + 1];
					i += 2;
				}
                else if (args[i].CompareTo("-dict_tport") == 0)
                {
                    mDictTransport = args[i + 1];
                    i += 2;
                }
                else if ((args[i].CompareTo("-dict_source") == 0) ||
                    (args[i].CompareTo("-d") == 0))
                {
                    mDictSource = args[i + 1];
                    i += 2;

                }
				else if (arg == "-T" || arg == "-tport")
				{
					mTransport = args[i + 1];
					i += 2;
				}
				else if (arg == "-m" || arg == "-middleware")
				{
					mMiddleware = args[i + 1];
					i += 2;
				}
                else if (arg == "-precision")
                {
                  mPrecision = Int32.Parse(args[i + 1]);
                  i += 2;
                }
				else if (arg == "-s")
				{
					mSymbolList.Add(args[i + 1]);
					i += 2;
				}
				else if (arg == "-f")
				{
					mFileName = args[i + 1];
					i += 2;
				}
				else if (arg == "-r" || arg == "-rate")
				{
					mThrottleRate = Double.Parse(args[i + 1]);
					i += 2;
				}
				else if (arg == "-v")
				{
					mLogLevel = 
						mLogLevel == MamaLogLevel.MAMA_LOG_LEVEL_WARN ? MamaLogLevel.MAMA_LOG_LEVEL_NORMAL :
						mLogLevel == MamaLogLevel.MAMA_LOG_LEVEL_NORMAL ? MamaLogLevel.MAMA_LOG_LEVEL_FINE :
						mLogLevel == MamaLogLevel.MAMA_LOG_LEVEL_FINE ? MamaLogLevel.MAMA_LOG_LEVEL_FINER : MamaLogLevel.MAMA_LOG_LEVEL_FINEST;
					i++;
					mHasLogLevel = true;
				}
				else if (arg == "-q")
				{
					mQuietModeLevel++;
					i++;
				}   
				else if (arg == "-b")
				{
					mCacheFullBooks = false;
					i++;
				}   
				else if (arg == "-e")
				{
					mPrintEntries = true;
					i++;
				}
				else if (arg == "-W")
				{
					mUseWorldView = true;
					++i;
				}
				else if (arg == "-L")
				{
					mLogReqResp= true;
					++i;
				}
				else if (arg == "-Y")
				{
					mSymbology = args[i+1];
					i += 2;
				}
				else if (arg == "-churn")
				{
					mChurnRate = Int32.Parse(args[i+1]);
					i += 2;
				}
				else if (arg == "-logfile")
				{
					mLogFileName = args[i+1];
					i += 2;
				}
				else if (arg == "-timerInterval")
				{
					mTimerInterval = Double.Parse(args[i + 1]);
					i += 2;
				}
				else if (arg == "-1")
				{
					mSnapshot = true;
					++i;
				}
				else if (arg == "-threads")
				{
					mNumThreads = Int32.Parse(args[i+1]);
					i += 2;
				}
				else
				{
					i++;
				}
			}
			if (mSymbolList.Count == 0)
			{
				readSymbolList();
			}
		}

		public String getSource()
		{
			return mSource;
		}

        public String getDictSource()
		{
			return mDictSource;
		}
        
		public double getThrottleRate ()
		{
			return mThrottleRate;
		}

		public string getTransport()
		{
			return mTransport;
		}

        public string getDictTransport()
		{
			return mDictTransport;
		}
        
		public string getMiddleware()
		{
			return mMiddleware;
		}

		public StringCollection getSymbolList()
		{
			return mSymbolList;
		}

		public int getSymbolListSize()
		{
			return mSymbolList.Count;
		}

		public bool hasLogLevel()
		{
			return mHasLogLevel;
		}
    
		public MamaLogLevel getLogLevel()
		{
			return mLogLevel;
		}

		public int getQuietModeLevel()
		{
			return mQuietModeLevel;
		}
      
		public bool cacheFullBooks()
		{
			return mCacheFullBooks;
		}
    
		public bool getPrintEntries()
		{
			return mPrintEntries;
		}

		public string getSymbology()
		{
			return mSymbology;
		}

		public bool useWorldview()
		{
			return mUseWorldView;
		}
      
		public bool logReqResp()
		{
			return mLogReqResp;
		}

		public int getChurnRate()
		{
			return mChurnRate;
		}
        
        public int getPrecision()
        {
          return mPrecision;
        }
   
		public string getLogFileName()
		{
			return mLogFileName;
		}
   
		public double getTimerInterval()
		{
			return mTimerInterval;
		}

		public bool getSnapshot()
		{
			return mSnapshot;
		}
		
		public int getNumThreads()
		{
			return mNumThreads;
		}

		private void readSymbolList()
		{
			try
			{
				TextReader reader = null;
				bool readingFromConsole = false;
				if (mFileName != null)
				{
					reader = new StreamReader(new FileStream(mFileName, FileMode.Open, FileAccess.Read));
				}
				else
				{
					readingFromConsole = true;
					reader = Console.In;
					Console.WriteLine("Enter one symbol per line and terminate with a .");
					Console.Write("SUBJECT> ");
				}
        
				string symbol;
				while (null != (symbol = reader.ReadLine()))
				{
					if (symbol.Length > 0)
					{
						if (symbol == ".")
						{
							break;
						}
						mSymbolList.Add(symbol);
					}
					if (readingFromConsole)
					{
						Console.Write("SUBJECT> ");
					}
				}
			}
			catch (Exception e)
			{
				Console.Error.WriteLine("Error reading symbol list");
				Console.Error.WriteLine(e.ToString());
				Environment.Exit(0);
			}
		}

		private string				mSource         = "WOMBAT";
		private string				mTransport      = null;
        private string				mDictSource         = "WOMBAT";
		private string				mDictTransport      = null;
		private string				mMiddleware     = "wmw";
		private StringCollection	mSymbolList     = new StringCollection();
		private string				mFileName       = null;
		private bool				mHasLogLevel	 = false;
		private MamaLogLevel		mLogLevel       = MamaLogLevel.MAMA_LOG_LEVEL_WARN;
		private int					mQuietModeLevel = 0;
		private double				mThrottleRate   = 1000.0;/*Same as MAMA default*/
		private bool				mCacheFullBooks = true;
		private bool				mPrintEntries   = false;
		private bool				mUseWorldView   = false;
		private bool				mLogReqResp     = false; 
		private string				mSymbology      = "DIRECT";
		private int					mChurnRate      = 0; /*Default Value*/
		private string				mLogFileName    = null;
		private double				mTimerInterval  = 1.0; /*Default Value*/
		private bool				mSnapshot       = false; /*Default Value*/
		private int					mNumThreads	 = 1;
        private int                 mPrecision      = 2;
	}
}
