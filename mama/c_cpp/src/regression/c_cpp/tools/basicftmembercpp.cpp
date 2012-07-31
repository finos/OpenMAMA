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

#include <mama/MamaFt.h>
#include <mama/MamaTimer.h>
#include <mama/MamaTransportMap.h>
#include <mama/log.h>
#include <iostream>
#include <stdlib.h>

using namespace Wombat;
using namespace std;

static  char* gUsageString[]=
{
    "MamaFtMember - Simple fault tolerance example.",
    "Demonstrates use of the MAMA FT API.",
    NULL
};

class FtMemberDemo : public MamaFtMemberCallback
                   , public MamaTimerCallback
{
public:
    FtMemberDemo          ();
    virtual ~FtMemberDemo () 
    {
    }

    virtual void onFtStateChange  (MamaFtMember*  member,
                                   const char*    groupName,
                                   mamaFtState    state);

    virtual void onTimer          (MamaTimer*     timer);
    virtual void onDestroy        (MamaTimer*     timer, void* closure);

    void init                     ();
    void printState               (const char*  context);
    void parseCommandLine         (int            argc,
                                   const char*    argv[]);   
       
private:
    void usage      (int exitStatus);
    MamaFtMember*   mFtMember;
    MamaTimer       mTimer;
    mamaFtState     mState;
    MamaTransport*  mTransport;
    mama_u32_t      mWeight;
    const char*     mGroup;
    mamaBridge      mBridgeImpl;
    MamaQueue*      mDefaultQueue;
    const char*     mTportName;
    mama_u32_t      mFtType;
    const char*     mMiddleware;
};

/******************************************************************************
 * Main
 *****************************************************************************/
int main (int argc, const char** argv)
{
    try
    {
        FtMemberDemo ftDemo;
        ftDemo.parseCommandLine (argc, argv);
        ftDemo.init ();
    }
    catch (MamaStatus status)
    {
        cerr << "Exception MamaStatus: " << status.toString () << endl;
    }
    return 0;
}

FtMemberDemo::FtMemberDemo ()
    : mState       (MAMA_FT_STATE_UNKNOWN)
    , mTransport   (NULL)
    , mWeight      (50)
    , mGroup       (NULL)
    , mBridgeImpl  (NULL) 
    , mTportName   ("ft")
    , mFtType      (MAMA_FT_TYPE_MULTICAST)
    , mMiddleware  ("wmw")
{
}

void FtMemberDemo::init ()
{
    mBridgeImpl = Mama::loadBridge (mMiddleware);
    
    Mama::open();
     
    mTransport = MamaTransportMap::findOrCreate (mTportName, mBridgeImpl);

    if (!mGroup)
    {
        mama_log (MAMA_LOG_LEVEL_NORMAL, "No FT group name specified");
        usage(1);
    }

    switch (mFtType)
    {
        case MAMA_FT_TYPE_MULTICAST:
            mFtMember = new MamaMulticastFtMember ();
        break;
        
        case MAMA_FT_TYPE_BRIDGE:
            mFtMember = new MamaBridgeFtMember ();
        break;
        default:
            mama_log (MAMA_LOG_LEVEL_NORMAL, "No FT type specified");
            usage(1);
        break;
    }
    
    
    mDefaultQueue = Mama::getDefaultEventQueue (mBridgeImpl);
    mTimer.create (mDefaultQueue, this, 2.0);  // two seconds

    mFtMember->setup (mDefaultQueue, 
                      this, 
                      mTransport, 
                      mGroup, 
                      mWeight, 
                      1.0, 
                      6.0);

    mFtMember->activate ();
    
    Mama::start (mBridgeImpl);
}

void FtMemberDemo::onFtStateChange (MamaFtMember*  member,
                                    const char*    groupName,
                                    mamaFtState    state)
{
    mState = state;
    printState ("FT Notification");
}

void FtMemberDemo::onTimer  (MamaTimer*  timer)
{
    printState ("Timer");
}

void FtMemberDemo::onDestroy  (MamaTimer*  timer, void* closure)
{
    printState ("Destroyed");
}

void FtMemberDemo::printState  (const char*  context)
{
    cout << "(" << context << ") Current state is: "
         << mamaFtStateToString (mState) << "\n";
    fflush(stdout);
}

void FtMemberDemo::parseCommandLine (int argc, const char* argv[])
{
    for (int i = 1; i < argc; )
    {
        if ((strcmp (argv[i], "-h") == 0) ||
            (strcmp (argv[i], "-?") == 0))
        {
            usage (0);
            i++;
        }
        else if ((strcmp (argv[i], "-g") == 0) ||
                 (strcmp (argv[i], "-group") == 0))
        {
            mGroup = argv[i+1];
            i += 2;
        }
        else if ((strcmp (argv[i], "-w") == 0) ||
                 (strcmp (argv[i], "-weight") == 0))
        {
            mWeight = strtoul (argv[i+1], NULL, 10);
            i += 2;
        }
        else if ((strcmp (argv[i], "-t") == 0) ||
                 (strcmp (argv[i], "-tport") == 0))
        {
            mTportName = argv[i+1];
            i += 2;
        }
        else if (strcmp (argv[i], "-v") == 0)
        {
            if (gMamaLogLevel == MAMA_LOG_LEVEL_NORMAL)
            {
                mama_enableLogging (stderr, MAMA_LOG_LEVEL_FINE);
            }
            else if (gMamaLogLevel == MAMA_LOG_LEVEL_FINE)
            {
                mama_enableLogging (stderr, MAMA_LOG_LEVEL_FINER);
            }
            else
            {
                mama_enableLogging (stderr, MAMA_LOG_LEVEL_FINEST);
            }
            i++;
        }
        else if (strcmp (argv[i], "-version") == 0)
        {
            mama_log (MAMA_LOG_LEVEL_NORMAL, Mama::getVersion (mBridgeImpl)); 
            exit (0);
        }
        else if (strcmp ("-m", argv[i]) == 0)
        {
            mMiddleware = argv[i+1];
            i += 2;               
        }
        else if (strcmp (argv[i], "-b") == 0)
        {
            mFtType = MAMA_FT_TYPE_BRIDGE;
            ++i;
        }
        else
        {
            usage(1);
        }
    } 
}

void FtMemberDemo::usage (int exitStatus)
{
   int i = 0;
   while (gUsageString[i] != NULL)
   {
       mama_log (MAMA_LOG_LEVEL_NORMAL, "%s", gUsageString[i++]);
   }
   exit(exitStatus);
}
