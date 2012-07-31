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
#include <mamda/MamdaTradeFields.h>
#include <mamda/MamdaQuoteFields.h>
#include <mamda/MamdaCommonFields.h>
#include <mamda/MamdaOrderBookFields.h>
#include <mamda/MamdaBookAtomicBookHandler.h>
#include <mamda/MamdaBookAtomicLevelHandler.h>
#include <mamda/MamdaBookAtomicLevelEntryHandler.h>
#include <mamda/MamdaBookAtomicListener.h>
#include <mamda/MamdaErrorListener.h>
#include <mamda/MamdaQualityListener.h>
#include <mama/MamaQueueGroup.h>
#include <mama/MamaSource.h>
#include "../parsecmd.h"
#include "../dictrequester.h"
#include <iostream>
#include <vector>
#include <stdexcept>
#include <iomanip>


using  std::endl;
using  std::vector;
using  std::cerr;
using  std::cout;


using namespace Wombat;

void usage (int exitStatus);

class AtomicBookTicker : public MamdaBookAtomicBookHandler,
                         public MamdaBookAtomicLevelHandler,
                         public MamdaBookAtomicLevelEntryHandler,
                         public MamdaErrorListener,
                         public MamdaQualityListener
{
public:
    virtual ~AtomicBookTicker () {}

    void onBookAtomicLevelRecap (
        MamdaSubscription*           subscription,
        MamdaBookAtomicListener&     listener,
        const MamaMsg&               msg,
        const MamdaBookAtomicLevel&  level)
    {
        if (gExampleLogLevel == EXAMPLE_LOG_LEVEL_NORMAL)
        {
            prettyPrint (subscription, level);
        }
    }
  
    void onBookAtomicBeginBook (
        MamdaSubscription*           subscription,
        MamdaBookAtomicListener&     listener,
        bool                         isRecap) 

    {
        if (gExampleLogLevel == EXAMPLE_LOG_LEVEL_NORMAL)
        {
            cout << "BEGIN BOOK - " <<  subscription->getSymbol () << endl;
        }
    }

    void onBookAtomicClear (
        MamdaSubscription*           subscription,
        MamdaBookAtomicListener&     listener,
        const MamaMsg&               msg)
    {
        if (gExampleLogLevel == EXAMPLE_LOG_LEVEL_NORMAL)
        {
            cout << "BOOK CLEAR - " <<  subscription->getSymbol () << endl << endl;
        }
    }
  
    void onBookAtomicGap (
        MamdaSubscription*           subscription,
        MamdaBookAtomicListener&     listener,
        const MamaMsg&               msg,
        const MamdaBookAtomicGap&    event)
    {
        if (gExampleLogLevel == EXAMPLE_LOG_LEVEL_NORMAL)
        {
            cout << "BOOK GAP - "    <<  subscription->getSymbol ()   << endl;
            cout << "\t Begin Gap: " << listener.getBeginGapSeqNum () << endl;
            cout << "\t End Gap  : " << listener.getEndGapSeqNum ()   << endl << endl;
        }
    }

  
    void onBookAtomicEndBook (
        MamdaSubscription*           subscription,
        MamdaBookAtomicListener&     listener) 
    {
        if (gExampleLogLevel == EXAMPLE_LOG_LEVEL_NORMAL)
        {
            cout << endl;
            if ((listener.getPriceLevelNumLevels () == 0) && (!listener.getHasMarketOrders()))
            {
                cout << "END BOOK - " <<  subscription->getSymbol ();
                cout << " - empty book message !!!" << endl << endl;
            }
            else
            {
                cout << "END BOOK - " <<  subscription->getSymbol () << endl << endl;
            }
            cout << endl;
        }
    }

    void onBookAtomicLevelEntryRecap (
        MamdaSubscription*                  subscription,
        MamdaBookAtomicListener&            listener,
        const MamaMsg&                      msg,
        const MamdaBookAtomicLevelEntry&    levelEntry)
    {
        if (gExampleLogLevel == EXAMPLE_LOG_LEVEL_NORMAL)
        {
            prettyPrint (subscription, levelEntry);
        }
    }

    void onBookAtomicLevelDelta (
        MamdaSubscription*                  subscription,
        MamdaBookAtomicListener&            listener,
        const MamaMsg&                      msg,
        const MamdaBookAtomicLevel&         level)
    {
        if (gExampleLogLevel == EXAMPLE_LOG_LEVEL_NORMAL)
        {
            prettyPrint (subscription, level);
        }
    }

    void onBookAtomicLevelEntryDelta (
        MamdaSubscription*                  subscription,
        MamdaBookAtomicListener&            listener,
        const MamaMsg&                      msg,
        const MamdaBookAtomicLevelEntry&    levelEntry)
    {
        if (gExampleLogLevel == EXAMPLE_LOG_LEVEL_NORMAL)
        {
            prettyPrint (subscription, levelEntry);
        }
    }

    void onError ( 
        MamdaSubscription*  subscription,
        MamdaErrorSeverity  severity,
        MamdaErrorCode      code,
        const char *        errorStr)
    {
        if (gExampleLogLevel == EXAMPLE_LOG_LEVEL_NORMAL)
        {
            cout << "atomicbookticker: ERROR: " << errorStr << "\n";
        }
    }

    void onQuality (MamdaSubscription* subscription, mamaQuality quality)
    {
        if (gExampleLogLevel == EXAMPLE_LOG_LEVEL_NORMAL)
        {
            cout << "atomicbookticker: QUALITY: " << quality << "\n";
        }
    }

    void prettyPrint (MamdaSubscription*                subscription,
                      const MamdaBookAtomicLevelEntry&  levelEntry)
    {
        // Print Entry Level Info
        const char* symbol        = subscription->getSymbol ();
        long        actNumEntries = levelEntry.getPriceLevelActNumEntries ();
        char        entryAction   = levelEntry.getPriceLevelEntryAction ();
        const char* entryId       = levelEntry.getPriceLevelEntryId ();
        long        entrySize     = levelEntry.getPriceLevelEntrySize ();

        cout << "\tENTRY | " << symbol << " | " << actNumEntries << " | ";
        cout << entryAction << " | " << entryId << " | " << entrySize << endl;
    }

    void prettyPrint (MamdaSubscription*           subscription, 
                      const MamdaBookAtomicLevel&  level)
    {
        // Price Level Info
        const char* symbol     = subscription->getSymbol ();
        double      price      = level.getPriceLevelPrice ();
        double      size       = level.getPriceLevelSize ();
        char        action     = level.getPriceLevelAction ();
        char        side       = level.getPriceLevelSide ();
        float       numEntries = level.getPriceLevelNumEntries ();
        const char* time       = level.getPriceLevelTime().getTimeAsString();

        cout << "\nLEVEL | " << symbol << " | ";

        // price is printed with 2 d.p.
        cout << setiosflags(ios::fixed) << setprecision(2) << price << setprecision(0) << " | ";

        cout << size << " | " << action << " | " << side << " | " << numEntries << " | " << time << endl;
    }
};


int main (int argc, const char** argv)
{
    MamaQueueGroup*             queues = NULL;
    mamaBridge                  bridge = NULL;
    try
    {
        // Initialize the MAMA API
        CommonCommandLineParser  cmdLine (argc, argv);
        bridge = cmdLine.getBridge();
        Mama::open ();
        DictRequester               dictRequester (bridge);
        const vector<const char*>&  symbolList    = cmdLine.getSymbolList ();
        int                         threads       = cmdLine.getNumThreads ();
        MamaSource*                 source        = cmdLine.getSource();
        bool             			processMarketOrders = cmdLine.getOptBool ('k');

        queues = new MamaQueueGroup (threads, bridge);

        dictRequester.requestDictionary     (cmdLine.getDictSource());
        MamdaCommonFields::setDictionary    (*dictRequester.getDictionary ());
        MamdaTradeFields::setDictionary     (*dictRequester.getDictionary ());
        MamdaQuoteFields::setDictionary     (*dictRequester.getDictionary ());
        MamdaOrderBookFields::setDictionary (*dictRequester.getDictionary ());

        const char* symbolMapFile = cmdLine.getSymbolMapFile ();
        if (symbolMapFile)
        {
            MamaSymbolMapFile* aMap = new MamaSymbolMapFile;
            if (MAMA_STATUS_OK == aMap->load (symbolMapFile))
            {
                source->getTransport()->setSymbolMap (aMap);
            }
        }

        for (vector < const char*>::const_iterator i = symbolList.begin ();
            i != symbolList.end (); ++i)
        {
            const char* symbol =*i;
            MamdaSubscription*       aSubscription = new MamdaSubscription;
            MamdaBookAtomicListener* aBookListener = new MamdaBookAtomicListener;
            AtomicBookTicker*        aTicker       = new AtomicBookTicker;

            aBookListener->addBookHandler (aTicker);
            aBookListener->addLevelHandler (aTicker);
            if (cmdLine.getOptBool ('e'))
            {// Entries
                aBookListener->addLevelEntryHandler (aTicker);
            }

            aSubscription->addMsgListener     (aBookListener);
            aSubscription->addQualityListener (aTicker);
            aSubscription->addErrorListener   (aTicker);
            aSubscription->setType            (MAMA_SUBSC_TYPE_BOOK);
            aSubscription->setMdDataType      (MAMA_MD_DATA_TYPE_ORDER_BOOK);
            aSubscription->create             (queues->getNextQueue(), source, symbol);
            aBookListener->setProcessMarketOrders (processMarketOrders);
        }
        
        // Dispatch on the default MAMA queue
        Mama::start(bridge);
    }
    catch (MamaStatus& e)
    {
        cerr << "Exception in main (): " << e.toString () << endl;
        exit(EXIT_FAILURE);
    }
    catch (std::exception& ex)
    {
        cerr << "Exception in main (): " << ex.what () << endl;
        exit(EXIT_FAILURE);
    }
    catch (...)
    {
        cerr << "Unknown Exception in main ()." << endl;
        exit(EXIT_FAILURE);
    }
    
    return 0;
}

void usage (int exitStatus)
{
    std::cerr << "Usage: atomicbookticker ";
    std::cerr << "[-S source] [-m middleware] [-tport transport] -s symbol [-s symbol ...] ";
    std::cerr << "[-threads x]\n";
    exit (exitStatus);
}
