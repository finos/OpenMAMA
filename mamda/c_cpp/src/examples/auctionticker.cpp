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
#include <mamda/MamdaAuctionListener.h>
#include <mamda/MamdaAuctionFields.h>
#include <mamda/MamdaAuctionHandler.h>
#include <mamda/MamdaAuctionRecap.h>
#include <mamda/MamdaAuctionUpdate.h>
#include <mamda/MamdaUncrossPriceInd.h>
#include <mamda/MamdaErrorListener.h>
#include <mamda/MamdaQualityListener.h>
#include <iostream>
#include <stdexcept>
#include <vector>
#include "parsecmd.h"
#include "mama/MamaQueueGroup.h"
#include "dictrequester.h"

using std::exception;
using std::vector;
using std::cerr;
using std::cout;

using namespace Wombat;

void usage (int exitStatus);

class AuctionTicker : public MamdaAuctionHandler
                           , public MamdaErrorListener
                           , public MamdaQualityListener
{
public:
    virtual ~AuctionTicker () {}
    
               
    void onAuctionRecap (
        MamdaSubscription*         subscription,
        MamdaAuctionListener&      listener,
        const MamaMsg&             msg,
        const MamdaAuctionRecap&   recap)
        {
             cout << "Auction Recap (" 
                  << subscription->getSymbol () 
                  << ") \n Uncross Price: "
                  << recap.getUncrossPrice().getAsString()
                  << "(" << recap.getUncrossPriceFieldState() << ")"
                  << ", Uncross Vol:"
                  << recap.getUncrossVolume()
                  << "(" << recap.getUncrossVolumeFieldState() << ")"
                  << ", Uncross Price Ind:"
                  << toString(recap.getUncrossPriceInd())
                  << "(" << recap.getUncrossPriceIndFieldState() << ")" 
                  << "\n"
                  << flush;         
        }
        
    void onAuctionUpdate ( 
        MamdaSubscription*          subscription,
        MamdaAuctionListener&      listener,
        const MamaMsg&              msg,
        const MamdaAuctionRecap&   recap,
        const MamdaAuctionUpdate&  update)
        
        {
             cout << "Auction Update (" 
                  << subscription->getSymbol () 
                  << ") \n Uncross Price: "
                  << update.getUncrossPrice().getAsString()
                  << "(" << update.getUncrossPriceFieldState() << ")"
                  << ", Uncross Vol:"
                  << update.getUncrossVolume()
                  << "(" << update.getUncrossVolumeFieldState() << ")"
                  << ", Uncross Price Ind:"
                  << toString(update.getUncrossPriceInd())
                  << "(" << update.getUncrossPriceIndFieldState() << ")" 
                  << "\n"
                  << flush;          
        }


    void onError (
        MamdaSubscription*   subscription,
        MamdaErrorSeverity   severity,
        MamdaErrorCode       errorCode,
        const char*          errorStr)
    {
        cout << "Error (" << subscription->getSymbol () << ")";
    }

    void onQuality (
        MamdaSubscription*   subscription,
        mamaQuality          quality)
    {
        cout << "Quality (" << subscription->getSymbol () << "): " << quality;
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
        const vector<const char*>&  symbolList = cmdLine.getSymbolList ();
        MamaSource*                 source     = cmdLine.getSource();
        MamaQueueGroup   queues (cmdLine.getNumThreads(), bridge);
        DictRequester    dictRequester (bridge);
        dictRequester.requestDictionary (cmdLine.getDictSource());

        MamdaAuctionFields::setDictionary (*dictRequester.getDictionary ());
        MamdaCommonFields::setDictionary (*dictRequester.getDictionary());
        
        const char* symbolMapFile = cmdLine.getSymbolMapFile ();
        if (symbolMapFile)
        {
            MamaSymbolMapFile* aMap = new MamaSymbolMapFile;
            if (MAMA_STATUS_OK == aMap->load (symbolMapFile))
            {
                source->getTransport()->setSymbolMap (aMap);
            }
        }

        for (vector<const char*>::const_iterator i = symbolList.begin ();
            i != symbolList.end ();
            ++i)
        {
            const char* symbol = *i;
            MamdaSubscription* aSubscription = new MamdaSubscription;
            MamdaAuctionListener* aListener = new MamdaAuctionListener();
            AuctionTicker* aTicker = new AuctionTicker();
            
            aListener->addHandler (aTicker);
            aSubscription->addMsgListener   (aListener);
            aSubscription->addQualityListener (aTicker);
            aSubscription->addErrorListener (aTicker);
            aSubscription->create (queues.getNextQueue(), source, symbol);   
        }

        Mama::start(bridge);
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

void usage (int exitStatus)
{
    std::cerr << "Usage: currencyticker [-tport] tport_name [-m] middleware [-S] source [-s] symbol [options] \n";   
    exit (exitStatus);
}



