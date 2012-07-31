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

#include <mama/mamacpp.h>
#include <mamda/MamdaSubscription.h>
#include <mamda/MamdaMsgListener.h>
#include <mamda/MamdaErrorListener.h>
#include <mamda/MamdaQualityListener.h>
#include <mamda/MamdaOrderBookListener.h>
#include <mamda/MamdaOrderBookFields.h>
#include <mamda/MamdaOrderBookHandler.h>
#include <iostream>
#include <stdexcept>
#include <vector>
#include "../parsecmd.h"
#include "mama/MamaQueueGroup.h"
#include "../dictrequester.h"
#include <list>
#include <fstream>

using std::exception;
using std::endl;
using std::vector;
using std::cerr;
using std::cout;
using std::list;

using namespace Wombat;

void usage (int exitStatus);

list <MamdaSubscription *> mSubscriptionList;
list <MamdaSubscription *> ::iterator subscriptionListIterator;

MamaSource *gSource;

unsigned int gChurnRate      = 0;
double       gChurnInterval  = 1.0;
unsigned int gChurnStats     = 0;
unsigned int gRecapStats     = 0;
unsigned int gUpdateStats    = 0;
unsigned int gBookGapStats   = 0;
unsigned int gQualityStats   = 0;

fstream gMyFile;

class ListenerCallback : public MamdaOrderBookHandler
                       , public MamdaErrorListener
                       , public MamdaQualityListener
{
public:
    ListenerCallback ():
        mDictionary (NULL) {}
    
    ~ListenerCallback () {}

    void onBookRecap (
        MamdaSubscription*                 subscription,
        MamdaOrderBookListener&            listener,
        const MamaMsg*                     msg,
        const MamdaOrderBookComplexDelta*  delta,
        const MamdaOrderBookRecap&         event,
        const MamdaOrderBook&              book)
    {
        ++gRecapStats;
    }
    
    void onBookDelta (
        MamdaSubscription*                 subscription,
        MamdaOrderBookListener&            listener,
        const MamaMsg*                     msg,
        const MamdaOrderBookSimpleDelta&   event,
        const MamdaOrderBook&              book)
    {
        ++gUpdateStats;
    }
    
     void onBookComplexDelta (
        MamdaSubscription*                 subscription,
        MamdaOrderBookListener&            listener,
        const MamaMsg*                     msg,
        const MamdaOrderBookComplexDelta&  event,
        const MamdaOrderBook&              book)
     {
         cout<<"\nComplexDelta";
     }
     
    void onBookGap (
        MamdaSubscription*          subscription,
        MamdaOrderBookListener&     listener,
        const MamaMsg*              msg,
        const MamdaOrderBookGap&    event,
        const MamdaOrderBook&       book)
    {
        ++gBookGapStats;
    }
    
    void onBookClear (
        MamdaSubscription*          subscription,
        MamdaOrderBookListener&     listener,
        const MamaMsg*              msg,
        const MamdaOrderBookClear&  event,
        const MamdaOrderBook&       book)
    {
        cout<<"\nBookClear";
    }
    
    void onError (
        MamdaSubscription*   subscription,
        MamdaErrorSeverity   severity,
        MamdaErrorCode       errorCode,
        const char*          errorStr)
    {
        cout<<"\nonError";
    }
    
    void onQuality (
        MamdaSubscription*   subscription,
        mamaQuality          quality)
    {
        ++gQualityStats;
    }
    
    void setDictionary (
        const MamaDictionary* dictionary)
    {
        mDictionary = dictionary;
    }
    
private:
    const MamaDictionary* mDictionary;
};

class BookChurn 
{
public:
    BookChurn ()
    {
        logFileName = NULL;
    }
    
    ~BookChurn () {}
    
    void subscribeToBooks (const char* symbol, MamaQueue* queue);
    void initialize (CommonCommandLineParser& cmdLine, mamaBridge bridge); 
    
private:
    ListenerCallback    mChurn;
    MamaTimer           churnTimer;
    MamaTimer           statsTimer;
    MamaQueueGroup*     queues;
    DictRequester*      dictRequester;
    const char*         symbolMapFile;
    MamaSymbolMapFile*  aMap;     
    const char*         logFileName;
    MamaQueue*          defaultQueue;
};

class ChurnTimerCallback : public MamaTimerCallback
                         , public BookChurn
{
private:
    int         churnIndex;
    const char* symbol;
    int         churnRate;
    int         randomNo;
    MamaQueue*  mDefaultQueue;
    
public:
    ChurnTimerCallback (MamaQueue* defaultQueue)
        : mDefaultQueue (defaultQueue) {}
    ~ChurnTimerCallback() {}
    
    void onTimer (MamaTimer* timer)
    {
        churnRate = gChurnRate;
        while (churnRate > 0)
        {
            //Random Number Generation
            randomNo = rand() % mSubscriptionList.size();
            churnIndex = 0;
            for (subscriptionListIterator = mSubscriptionList.begin ();
                    subscriptionListIterator != mSubscriptionList.end ();
                    subscriptionListIterator++)
            {
                while (churnIndex != randomNo)
                {
                    ++churnIndex;
                    ++subscriptionListIterator;
                }
                //Destroying the Subscription
                vector<MamdaMsgListener*> mMsgListeners = 
                    ((MamdaSubscription*)*subscriptionListIterator)->getMsgListeners();
                ((MamdaSubscription*)*subscriptionListIterator)->destroy();
                unsigned long size = mMsgListeners.size();
                for (unsigned long i = 0; i < size; ++i)
                {
                    MamdaMsgListener* listener = mMsgListeners[i];
                    delete (listener);
                }
                break;
            }
            //Removing the Subscription from the linked list
            symbol = ((MamdaSubscription*)*subscriptionListIterator)->getSymbol(); 
            mSubscriptionList.remove ((MamdaSubscription*)*subscriptionListIterator);
            //Subscribing to the particular symbol
            subscribeToBooks(symbol, mDefaultQueue);
            --churnRate;
            ++gChurnStats;
        }
    }
};

class StatsTimerCallback : public MamaTimerCallback
{
    public:
        StatsTimerCallback () {}
        ~StatsTimerCallback() {}

        void onTimer (MamaTimer* timer)
        {
            cout<<"\nChurnStats = "<<gChurnStats;
            cout<<" RecapStats = "<<gRecapStats;
            cout<<" UpdateStats = "<<gUpdateStats;
            cout<<" BookGapStats = "<<gBookGapStats;
            cout<<" QualityStats = "<<gQualityStats;
            if (gMyFile.is_open())
            {
                gMyFile<<gChurnStats<<","<<gRecapStats<<","<<gUpdateStats<<","<<gBookGapStats<<","<<gQualityStats<<"\n";
                gMyFile<<flush;
            }
            gChurnStats   = 0;
            gRecapStats   = 0;
            gUpdateStats  = 0;
            gBookGapStats = 0;
            gQualityStats = 0;
        }
};

int main (int argc, const char **argv)
{
    try
    {
        CommonCommandLineParser     cmdLine (argc, argv);
        // Initialise the MAMA API
        mamaBridge bridge = cmdLine.getBridge();
        Mama::open ();
        BookChurn mBookChurn;
        mBookChurn.initialize (cmdLine, bridge);
        Mama::start(bridge);
        gMyFile.close();
    }
    catch (MamaStatus &e)
    {
        // This exception can be thrown from Mama.open (),
        // Mama::createTransport (transportName) and from
        // MamdaSubscription constructor when entitlements is enabled.
        cerr << "Exception in main (): " << e.toString () << endl;
        exit (1);
    }
    catch (exception &ex)
    {
        cerr << "Exception in main (): " << ex.what () << endl;
        exit (1);
    }
    catch (...)
    {
        cerr << "Unknown Exception in main ()." << endl;
        exit (1);
    }
}

void BookChurn::initialize (CommonCommandLineParser& cmdLine, mamaBridge bridge)
{
    dictRequester   = new DictRequester (bridge);
    gSource         = cmdLine.getSource();
    queues          = new MamaQueueGroup (cmdLine.getNumThreads(), bridge);
    dictRequester->requestDictionary (cmdLine.getDictSource());
    mChurn.setDictionary (dictRequester->getDictionary());
    MamdaOrderBookFields::setDictionary (*(dictRequester->getDictionary ()));
    symbolMapFile   = cmdLine.getSymbolMapFile ();
    gChurnRate      = cmdLine.getChurnRate ();
    gChurnInterval  = cmdLine.getTimerInterval (); 
    logFileName     = cmdLine.getLogFileName ();

    defaultQueue = Mama::getDefaultEventQueue (bridge);

    const vector<const char*>&  symbolList = cmdLine.getSymbolList ();

    if (logFileName != NULL)
    {
        gMyFile.open (logFileName,ios::out);
    
        if (gMyFile.is_open())
        {
            gMyFile<<"ChurnStas"<<","<<"RecapStas"<<","<<"UpdateStats"<<","<<"BookGapStats"<<","<<"QualityStats"<<"\n";
        }
    }

    if (symbolMapFile)
    {
        aMap = new MamaSymbolMapFile;
        
        if (MAMA_STATUS_OK == aMap->load (symbolMapFile))
        {
            gSource->getTransport()->setSymbolMap (aMap);
        }
    }

    for (vector<const char*>::const_iterator i = symbolList.begin ();
         i != symbolList.end ();
         ++i)
    {
        const char* symbol = *i;
        subscribeToBooks (symbol, defaultQueue);
    }
    //Churn Timer   
    ChurnTimerCallback* churnCallback = new ChurnTimerCallback(defaultQueue); 
    churnTimer.create (defaultQueue,churnCallback,gChurnInterval,NULL);
    //Stats Timer
    StatsTimerCallback* statsCallback = new StatsTimerCallback();
    statsTimer.create (defaultQueue,statsCallback,1.0,NULL);
}

void BookChurn::subscribeToBooks (const char* symbol, MamaQueue* queue)
{
    MamdaSubscription*      aSubscription   = new MamdaSubscription;
    MamdaOrderBookListener* aBookListener   = new MamdaOrderBookListener;
    aSubscription->addMsgListener (aBookListener);
    aBookListener->addHandler  (&mChurn);
    aSubscription->addQualityListener (&mChurn);
    aSubscription->addErrorListener (&mChurn);
    aSubscription->setType (MAMA_SUBSC_TYPE_BOOK);
    aSubscription->setMdDataType (MAMA_MD_DATA_TYPE_ORDER_BOOK);
    aSubscription->create (queue, gSource, symbol);
    mSubscriptionList.push_back (aSubscription);
}

void usage (int exitStatus)
{
    std::cerr << "Usage: bookchurn -m middleware [-S source] -s symbol [-s symbol ...] [-churn churnRate] [-logfile filename] [-timerInterval interval]\n";
    exit (exitStatus);
}

