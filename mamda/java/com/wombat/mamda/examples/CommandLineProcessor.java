/* $Id$
 *
 * OpenMAMA: The open middleware agnostic messaging API
 * Copyright (C) 2012 NYSE Technologies, Inc.
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

package com.wombat.mamda.examples;

import java.util.logging.Level;
import java.util.ArrayList;
import java.io.*;
import com.wombat.mama.Mama;
import com.wombat.mama.MamaBridge;


class CommandLineProcessor
{
    private String     mSource             = "WOMBAT";
    private String     mTransport          = null;
    private String     mDictTransport      = null;
    private String     mDictSource         = "WOMBAT";
    private String     mPubSource          = null;
    private String     mPartId             = null;
    private ArrayList  mSymbolList         = new ArrayList();
    private String     mFileName           = null;
    private Level      mLogLevel           = Level.WARNING;
    private double     mThrottleRate       = 1000.0;    /*Same as MAMA default*/
    private boolean    mShowDeltas         = false;
    private boolean    mShowMarketOrders   = false;
    private boolean    mPrintEntries       = false;
    private boolean    mUseWorldView       = false;
    private boolean    mLogReqResp         = false; 
    private boolean    mSnapshot           = false; 
    private int        mQuietModeLevel     = 0;
    private String     mSymbology          = "DIRECT";
    private int        mChurnRate          = 0;         /*Default Value*/
    private String     mLogFileName        = null;
    private double     mTimerInterval      = 1.0;       /*Default Value*/
    private String     mMiddleware         = "wmw";
    private int        mPrecision          = 2;
    private MamaBridge mBridge             = null;
    private int        mThreads            = 0;
    
    public CommandLineProcessor (final String[] args)
    {
        for( int i = 0; i < args.length;  )
        {
            if ( args[i].equals( "-S" ) )
            {
                mSource = args[i + 1];
                i += 2;
            }
            else if ( args[i].equals("-SP"))
            {
                mPubSource = args[i+1];
                i+=2;
            }
            else if (args[i].equals("-p"))
            {
                mPartId = args[i+1];
                i+=2;
            }
            else if (args[i].equals( "-tport" ))
            {
                mTransport = args[i + 1];
                i += 2;
            }
            else if (args[i].equals("-dict_tport") ||
                args[i].equals("-DT" ) )
            {
                mDictTransport = args[i+1];
                i += 2;
            }
            else if ( args[i].equals( "-d" ) ||
                args[i].equals( "-dict_source" ) )
            {
                mDictSource = args[i+1];
                i += 2;
            }
            else if ( args[i].equals( "-s" ) )
            {
                mSymbolList.add( args[i + 1] );
                i += 2;
            }
            else if ( args[i].equals( "-f" ) )
            {
                mFileName = args[i + 1];
                i += 2;
            }
            else if (args[i].equals( "-r" ) ||
                     args[i].equals ("-rate"))
            {
                mThrottleRate = Double.parseDouble(args[i + 1]);
                i += 2;
            }
            else if ( args[i].equals( "-v" ) )
            {
                mLogLevel = 
                    mLogLevel == null       ? Level.FINE  :
                    mLogLevel == Level.FINE ? Level.FINER : Level.FINEST;
                i++;
            }
            else if ( args[i].equals( "-q" ) )
            {
                mQuietModeLevel++;
                i++;
            }   
            else if (args[i].equals ("-deltas"))
            {
                mShowDeltas = true;
                i++;
            }
            else if (args[i].equals ("-k"))
            {
              mShowMarketOrders = true;
              i++;
            }
            else if (args[i].equals ("-e"))
            {
                mPrintEntries = true;
                i++;
            }
            else if (args[i].equals ("-1"))
            {
                mSnapshot = true;
                i++;
            }
            else if (args[i].equals ("-W"))
            {
                mUseWorldView = true;
                ++i;
            }
            else if (args[i].equals ("-L"))
            {
                mLogReqResp= true;
                ++i;
            }
            else if (args[i].equals ("-Y"))
            {
                mSymbology = args[i+1];
                i += 2;
            }
            else if (args[i].equals ("-m"))
            {
                mMiddleware = args[i+1];
                i += 2;
            }
            else if (args[i].equals ("-precision"))
            {
              mPrecision = Integer.parseInt (args[i+1]);
              i += 2;
              if (mPrecision==0)
              {
                mPrecision = 2;
              }
              if (mPrecision >6)
              {
                mPrecision = 6;
              }
            } 
            else if (args[i].equals ("-churn"))
            {
                mChurnRate = Integer.parseInt (args[i+1]);
                i += 2;
            }
            else if (args[i].equals ("-threads"))
            {
                mThreads = Integer.parseInt (args[i+1]);
                i += 2;
            }
            else if (args[i].equals ("-logfile"))
            {
                mLogFileName = args[i+1];
                i += 2;
            }
            else if (args[i].equals ("-timerInterval"))
            {
                mTimerInterval = Double.parseDouble(args[i + 1]);
                i += 2;
            }
            else
            {
                i++;
            }
        }
        mBridge = Mama.loadBridge (mMiddleware);
        
        if (mSymbolList.isEmpty())
        {
            readSymbolList();
        }
    }

    public String getSource()
    {
        return mSource;
    }

    public String getPubSource()
    {
        return mPubSource;
    }

    public String getPartId()
    {
        return mPartId;
    }
    
    public boolean getSnapshot()
    {
        return mSnapshot;
    }

    public int getQuietModeLevel ()
    {
       return mQuietModeLevel;
    }
    
    public double getThrottleRate ()
    {
        return mThrottleRate;
    }

    public String getTransport()
    {
        return mTransport;
    }

    public String getDictTransport()
    {
        return mDictTransport;
    }

    public String getDictSource()
    {
	    return mDictSource;
    }

    public ArrayList getSymbolList()
    {
        return mSymbolList;
    }

    public int getSymbolListSize ()
    {
        return mSymbolList.size();    
    }
    
    public Level getLogLevel()
    {
        return mLogLevel;
    }
      
    public boolean showDeltas()
    {
        return mShowDeltas;
    }
    public boolean showMarketOrders()
    {
        return mShowMarketOrders;
    }
    public boolean getPrintEntries ()
    {
        return mPrintEntries;
    }

    public String getSymbology()
    {
        return mSymbology;
    }

    public boolean useWorldview()
    {
        return mUseWorldView;
    }
      
    public boolean logReqResp ()
    {
        return mLogReqResp;
    }

    public int getChurnRate ()
    {
        return mChurnRate;
    }
   
    public String getLogFileName ()
    {
        return mLogFileName;
    }
   
    public double getTimerInterval ()
    {
        return mTimerInterval;
    }
    
    public MamaBridge getBridge ()
    {
        return mBridge;
    }

    public int getNumThreads ()
    {
        return mThreads;
    }
    
    public int getPrecision()
    {
      return mPrecision;
    }

    private void readSymbolList()
    {
        try
        {
            InputStream input;
            if (mFileName != null)
            {
                input = new FileInputStream (mFileName);
            }
            else
            {
                input = System.in;
                System.out.println ("Enter one symbol per line and terminate with a .");
                System.out.print   ("SUBJECT> ");
            }
        
            final BufferedReader reader =
                new BufferedReader (new InputStreamReader (input));
        
            String symbol;
            while (null != (symbol = reader.readLine()))
            {
                if (!symbol.equals (""))
                {
                    if ( symbol.equals ("."))
                    {
                        break;
                    }
                    mSymbolList.add (symbol);
                }
        
                if (input == System.in)
                {
                    System.out.print ("SUBJECT>");
                }
            }
        }
        catch (Exception e)
        {
            System.err.println ("Error reading symbol list");
            if( e.getCause() != null )
                e.getCause().printStackTrace();

            e.printStackTrace();
                System.exit (0);
        }
    }
}
