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

#include <mama/mama.h>
#include <mama/mamacpp.h>
#include <mama/MamaTransport.h>
#include "parsecmd.h"
#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <string.h>
#include <mamda/MamdaVersion.h>

using std::ifstream;
using std::cerr;
using std::string;
using std::map;

using namespace Wombat;

typedef map<string,const char*>  OptionMap;

extern void usage (int);

ExampleLogLevel gExampleLogLevel = EXAMPLE_LOG_LEVEL_NORMAL;

struct CommonCommandLineParser::CommonCommandLineParserImpl
{
    CommonCommandLineParserImpl (int          argc, 
                                 const char*  argv[]);

    const char*          mSourceName;
    const char*          mTportName;
    MamaSource*          mSource;
    const char*          mDictSourceName;
    const char*          mDictTportName;
    MamaSource*          mDictSource;
    const char*          mOptionSourceName;
    const char*          mOptionTportName;
    MamaSource*          mOptionSource;
    const char*          mSymbolMapFile;
    vector<const char*>  mSymbolList;
    double               mThrottleRate;
    int                  mThreads;
    double               mTimeout;
    MamaLogLevel         mSubscLogLevel;
    OptionMap            mOptions;
    bool                 mLogReqResp;
    bool                 mUseWorldView;
    const char*          mSymbology;
    int                  mChurnRate;
    double               mTimerInterval;
    const char*          mLogFileName;
    const char*          mMiddleware;
    int                  mPrecision;
    int                  mShutdownTime;
    bool                 mShowDeltas;
    mamaBridge           mBridge;
    bool                 mSnapShot;
    const char*          mQueryArg1;
    const char*          mQueryArg2;
    const char*          mQueryArg3;
	const char*          mQueryArg4;
    int                  mQuery;
    int                  mQueryType;
    int                  mQueryCycles;
};

CommonCommandLineParser::CommonCommandLineParser (
    int          argc, 
    const char*  argv[])
    : mImpl (*new CommonCommandLineParserImpl (argc,argv))
{
}

CommonCommandLineParser::~CommonCommandLineParser ()
{
    delete &mImpl;
}

MamaSource*  CommonCommandLineParser::getSource ()
{
    if (!mImpl.mSource)
    {
        mImpl.mSource = new MamaSource ("default", 
                                          mImpl.mTportName, 
                                          mImpl.mSourceName,
                                          mImpl.mBridge);
        mImpl.mSource->getTransport()->setOutboundThrottle (
                                        mImpl.mThrottleRate, 
                                        MAMA_THROTTLE_DEFAULT);
    }    
    return mImpl.mSource;
}

MamaSource*  CommonCommandLineParser::getDictSource ()
{
    if (!mImpl.mDictSource)
    {
        mImpl.mDictSource = new MamaSource ("dict", 
                                              mImpl.mDictTportName 
                                                ? mImpl.mDictTportName 
                                                : mImpl.mTportName,
                                              mImpl.mDictSourceName,
                                              mImpl.mBridge);
    }
    return mImpl.mDictSource;
}

MamaSource*  CommonCommandLineParser::getOptionSource ()
{
    if (!mImpl.mOptionSource)
    {
        mImpl.mOptionSource = new MamaSource ("options", 
                                          mImpl.mOptionTportName 
                                            ? mImpl.mOptionTportName
                                            : mImpl.mTportName, 
                                          mImpl.mOptionSourceName,
                                          mImpl.mBridge);    
        mImpl.mOptionSource->getTransport()->setOutboundThrottle (
                                        mImpl.mThrottleRate, 
                                        MAMA_THROTTLE_DEFAULT);
    }
    return mImpl.mOptionSource;    
}

const char*  CommonCommandLineParser::getSymbolMapFile ()
{
    return mImpl.mSymbolMapFile;
}

const vector<const char*>&  CommonCommandLineParser::getSymbolList ()
{
    return mImpl.mSymbolList;
}

double  CommonCommandLineParser::getThrottleRate ()
{
    return mImpl.mThrottleRate;
}

int  CommonCommandLineParser::getNumThreads ()
{
    return mImpl.mThreads;
}

double  CommonCommandLineParser::getTimeout ()
{
    return mImpl.mTimeout;
}

MamaLogLevel  CommonCommandLineParser::getSubscLogLevel ()
{
    return mImpl.mSubscLogLevel;
}

bool CommonCommandLineParser::getUseWorldview ()
{
    return mImpl.mUseWorldView;
}

bool CommonCommandLineParser::getLogReqResp ()
{
    return mImpl.mLogReqResp;
}

const char* CommonCommandLineParser::getSymbology ()
{
    return mImpl.mSymbology;
}

const char* CommonCommandLineParser::getMiddleware ()
{
    return mImpl.mMiddleware;
}

int CommonCommandLineParser::getPrecision ()
{
  return mImpl.mPrecision;
}

int CommonCommandLineParser::getShutdownTime ()
{
  return mImpl.mShutdownTime;
}

bool CommonCommandLineParser::showDeltas ()
{
  return mImpl.mShowDeltas;
}

mamaBridge CommonCommandLineParser::getBridge ()
{
    return mImpl.mBridge;
}

bool CommonCommandLineParser::getSnapshot()
{
    return mImpl.mSnapShot;
}

bool  CommonCommandLineParser::getOptBool (char  option0)
{
    string option (&option0, 0, 1);
    OptionMap::iterator found = mImpl.mOptions.find (option);
    return (found != mImpl.mOptions.end ());
}

bool  CommonCommandLineParser::getOptBool (const char*  option0)
{
    string option (option0);
    OptionMap::iterator found = mImpl.mOptions.find (option);
    return (found != mImpl.mOptions.end ());
}

int  CommonCommandLineParser::getOptInt (char  option0, int defaultValue)
{
    string option (&option0, 0, 1);
    OptionMap::iterator found = mImpl.mOptions.find (option);
    if (found != mImpl.mOptions.end ())
    {
        const char* value = found->second;
        if (value)
            return strtol (value, NULL, 10);
    }
    return defaultValue;
}

int  CommonCommandLineParser::getOptInt (const char*  option0, int defaultValue)
{
    string option (option0);
    OptionMap::iterator found = mImpl.mOptions.find (option);
    if (found != mImpl.mOptions.end ())
    {
        const char* value = found->second;
        if (value)
            return strtol (value, NULL, 10);
    }
    return defaultValue;
}

const char*  CommonCommandLineParser::getOptString (char  option0)
{
    string option (&option0, 0, 1);
    OptionMap::iterator found = mImpl.mOptions.find (option);
    if (found != mImpl.mOptions.end ())
    {
        return found->second;
    }
    return NULL;
}

const char*  CommonCommandLineParser::getOptString (const char*  option0)
{
    string option (option0);
    OptionMap::iterator found = mImpl.mOptions.find (option);
    if (found != mImpl.mOptions.end ())
    {
        return found->second;
    }
    return NULL;
}

int CommonCommandLineParser::getChurnRate ()
{
    return mImpl.mChurnRate;
}

double CommonCommandLineParser::getTimerInterval ()
{
    return mImpl.mTimerInterval;
}

const char* CommonCommandLineParser::getLogFileName ()
{
    return mImpl.mLogFileName;    
}

const char*  CommonCommandLineParser::getQueryArg1 ()
{
    return mImpl.mQueryArg1;  
}

const char* CommonCommandLineParser::getQueryArg2 ()
{
    return mImpl.mQueryArg2;  
}

const char* CommonCommandLineParser::getQueryArg3 ()
{
    return mImpl.mQueryArg3;  
}

const char* CommonCommandLineParser::getQueryArg4 ()
{
    return mImpl.mQueryArg4;  
}

int CommonCommandLineParser::getQuery ()
{
    return mImpl.mQuery;  
}

int CommonCommandLineParser::getQueryType ()
{
    return mImpl.mQueryType;  
}


int CommonCommandLineParser::getQueries ()
{
    return mImpl.mQueryCycles;  
}

CommonCommandLineParser::CommonCommandLineParserImpl::CommonCommandLineParserImpl (
    int          argc, 
    const char*  argv[])
{
    mSource        = NULL;
    mDictSource    = NULL;
    mOptionSource  = NULL;
    mSymbolMapFile = NULL;
    mThrottleRate  = 500.0;
    mThreads       = 0;
    mTimeout       = 1.0;
    mChurnRate     = 0;
    mTimerInterval = 1.0; //Default Value
    mLogFileName   = NULL;
    mMiddleware    = "wmw";
    mPrecision     = 2;
    mShutdownTime  = 0;
    mShowDeltas    = false;
    mBridge        = NULL;
    int i          = 1;
    mUseWorldView  = false;
    
    mTportName        = NULL;
    mDictTportName    = NULL;
    mOptionTportName  = NULL;
    mSourceName       = "WOMBAT";
    mDictSourceName   = "WOMBAT";
    mOptionSourceName = "OPRA";
    mSnapShot         = false;
    
    mQueryArg1   = NULL;
    mQueryArg2   = NULL;
    mQueryArg3   = NULL;
	mQueryArg4   = NULL;
    mQuery       = 0;
    mQueryType   = 0;
    mQueryCycles = 1;
    
    while (i < argc)
    {
        bool handled = false;
        if ((strcmp (argv[i], "-OS") == 0) ||
            (strcmp (argv[i], "-optionSource") == 0) ||
            (strcmp (argv[i], "-option-source") == 0))
        {
            mOptionSourceName = argv[i + 1];
            handled = true;
        }
        else if ((strcmp (argv[i], "-S") == 0) ||
            (strcmp (argv[i], "-source") == 0))
        {
            mSourceName = argv[i + 1];
            handled = true;
        }
        else if ((strcmp (argv[i], "-DS") == 0) ||
            (strcmp (argv[i], "-dict-source") == 0) ||
            (strcmp (argv[i], "-dictSource") == 0) ||
            (strcmp (argv[i], "-d") == 0))
        {
            mDictSourceName = argv[i + 1];
            handled = true;
        }
        else if ((strcmp (argv[i], "-T") == 0) ||
                 (strcmp (argv[i], "-tport") == 0))
        {
            mTportName = argv[i + 1];
            handled = true;
        }
        else if ((strcmp (argv[i], "-DT") == 0) ||
            (strcmp (argv[i], "-dictTport") == 0) ||
            (strcmp (argv[i], "-dict-tport") == 0) ||
            (strcmp (argv[i], "-dict_tport") == 0))
        {
            mDictTportName = argv[i + 1];
            handled = true;
        }
        else if ((strcmp (argv[i], "-OT") == 0) ||
            (strcmp (argv[i], "-optionTport") == 0) ||
            (strcmp (argv[i], "-option-tport") == 0))
        {
            mOptionTportName = argv[i + 1];
            handled = true;
        }
        else if (strcmp (argv[i], "-s") == 0)
        {
            mSymbolList.push_back (argv[i + 1]);
            handled = true;
        }
        else if (strcmp (argv[i], "-threads") == 0)
        {
            mThreads = strtol (argv[i + 1], NULL, 10);
            handled = true;
        }
        else if ((strcmp (argv[i], "-r") == 0) ||
                 (strcmp (argv[i], "-rate") == 0))
        {
            mThrottleRate = strtod (argv[i + 1], NULL);
            handled = true;
        }
        else if (strcmp (argv[i], "-t") == 0)
        {
            mTimeout = strtod (argv[i + 1], NULL); 
            handled = true;
        }
        else if (strcmp (argv[i], "-mp") == 0)
        {
            mSymbolMapFile = argv[i + 1];
            handled = true;
        }
        else if (strcmp (argv[i], "-m") == 0)
        {
            mMiddleware = argv[i + 1];
            handled = true;
        }
        else if (strcmp (argv[i], "-precision") == 0)
        {
          mPrecision = strtol (argv[i + 1], NULL, 10);
          if(!mPrecision)
          {
            mPrecision=2;
          }
          if(mPrecision>6)
          {
            mPrecision=6;
          }
          handled = true;
        }
        else if (strcmp ("-shutdown", argv[i]) == 0)
        {
            mShutdownTime = atoi (argv[i + 1]);           
        }        
        else if (strcmp (argv[i], "-deltas") == 0)
        {
            mShowDeltas = true;
        }
        else if (strcmp (argv[i], "-churn") == 0)
        {
            mChurnRate = strtol (argv[i + 1], NULL, 10);
            handled = true;
        }
        else if (strcmp (argv[i], "-timerInterval") == 0)
        {
            mTimerInterval = strtod (argv[i + 1], NULL);
            handled = true;
        }
        else if (strcmp (argv[i], "-logfile") == 0)
        {
            mLogFileName = argv[i + 1];
            handled = true;
        }
        else if (strcmp (argv[i], "-f") == 0)
        {
            const char* filename = argv[i + 1];
            ifstream input (filename);
            if (!input)
            {
                cerr << "Cannot open file: " << filename << "\n";
                exit (1);
            }
            string symbol;
            input >> symbol;
            while (!input.eof ())
            {
                if (!symbol.empty ())
                {
                    // Note: we never free the memory allocated by
                    // strdup () in this example!
                    mSymbolList.push_back (strdup (symbol.c_str ()));
                }
                input >> symbol;
            }
            handled = true;
        }
        else if (strcmp (argv[i], "-v") == 0)
        {
            if (mama_getLogLevel () == MAMA_LOG_LEVEL_NORMAL)
            {
                mama_enableLogging (stderr, MAMA_LOG_LEVEL_FINE);
            }
            else if (mama_getLogLevel () == MAMA_LOG_LEVEL_FINE)
            {
                mama_enableLogging (stderr, MAMA_LOG_LEVEL_FINER);
            }
            else
            {
                mama_enableLogging (stderr, MAMA_LOG_LEVEL_FINEST);
            }
            handled = true;
        }
        else if (strcmp (argv[i], "-q") == 0)
        {
            if (gExampleLogLevel == EXAMPLE_LOG_LEVEL_NORMAL )
            {
                gExampleLogLevel = EXAMPLE_LOG_LEVEL_QUIET;
            }
            else if (gExampleLogLevel == EXAMPLE_LOG_LEVEL_QUIET)
            {
                gExampleLogLevel = EXAMPLE_LOG_LEVEL_QUIETER;
            }
            else if (gExampleLogLevel == EXAMPLE_LOG_LEVEL_QUIETER)
            {
                gExampleLogLevel = EXAMPLE_LOG_LEVEL_QUIETEST;
            }
            handled = true;
        }
        else if (strcmp (argv[i], "-V") == 0)
        {
            if (mSubscLogLevel == MAMA_LOG_LEVEL_NORMAL)
            {
                mSubscLogLevel = MAMA_LOG_LEVEL_FINE;
            }
            else if (mSubscLogLevel == MAMA_LOG_LEVEL_FINE)
            {
                mSubscLogLevel = MAMA_LOG_LEVEL_FINER;
            }
            else
            {
                mSubscLogLevel = MAMA_LOG_LEVEL_FINEST;
            }
            handled = true;
        }
        else if (strcmp (argv[i], "-W") == 0)
        {
            mUseWorldView = true;
            handled = true;     
        }
        else if (strcmp (argv[i], "-L") == 0)
        {
            mLogReqResp = true;
            handled = true;     
        }
        else if (strcmp (argv[i], "-Y") == 0)
        {
            mSymbology = argv[i+1];
            handled = true;     
        }
        else if ((strcmp (argv[i], "-?") == 0) ||
                 (strcmp (argv[i], "--help") == 0))
        {
            usage(1);
            handled = true;     
        }
        else if (strcmp (argv[i], "-1") == 0)
        {
            mSnapShot = true;
            handled = true;     
        }
        else if (strcmp (argv[i], "-Q") == 0)
        {
            mQuery = strtol (argv[i + 1], NULL, 10);     
        }
        else if (strcmp (argv[i], "-QT") == 0)
        {
            mQueryType = strtol (argv[i + 1], NULL, 10);     
        }
        else if (strcmp (argv[i], "-A1") == 0)
        {
            mQueryArg1 = argv[i + 1];
        }
        else if (strcmp (argv[i], "-A2") == 0)
        {
            mQueryArg2 = argv[i + 1];
        }
        else if (strcmp (argv[i], "-A3") == 0)
        {
            mQueryArg3 = argv[i + 1];
        }
		else if (strcmp (argv[i], "-A4") == 0)
        {
            mQueryArg4 = argv[i + 1];
        }
        else if (strcmp (argv[i], "-QC") == 0)
        {
            mQueryCycles =strtol (argv[i + 1], NULL, 10);     
        }
        if (argv[i][0] == '-')
        {
            // general option (may or may not have following arg)
            const char* opt = argv[i]+1;
            if (strlen (opt) > 0)
            {
                if ((i+1 < argc) && (argv[i+1][0] != '-'))
                {
                    // next arg does not begin with '-'
                    mOptions[opt] = argv[i+1];
                    i += 2;
                }
                else
                {
                    // no additional arg
                    mOptions[opt] = "1";
                    i++;
                }
            }
            handled = true;
        }

        if (!handled)
        {
            usage (1);
        }
    }
    
    mBridge = Mama::loadBridge (mMiddleware);
}
