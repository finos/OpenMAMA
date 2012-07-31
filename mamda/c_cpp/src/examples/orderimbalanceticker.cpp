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
#include <mamda/MamdaOrderImbalanceListener.h>
#include <mamda/MamdaOrderImbalanceFields.h>
#include <mamda/MamdaOrderImbalanceHandler.h>
#include <mamda/MamdaOrderImbalanceRecap.h>
#include <mamda/MamdaOrderImbalanceUpdate.h>
#include <mamda/MamdaOrderImbalanceSide.h>
#include <mamda/MamdaOrderImbalanceType.h>
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

class OrderImbalanceTicker : public MamdaOrderImbalanceHandler
                           , public MamdaErrorListener
                           , public MamdaQualityListener
{
public:
    virtual ~OrderImbalanceTicker () {}
    
    void onOrderImbalance ( 
        MamdaSubscription*           subscription,
        MamdaOrderImbalanceListener& listener,
        const MamaMsg&               msg,
        MamdaOrderImbalanceRecap&    imbalance,
        MamdaOrderImbalanceUpdate&   update)
    {
         cout << "OrderImbalance (" << subscription->getSymbol ()
         << ") (SecStatus:" << listener.getImbalanceState()
         << "; SellVol:" << listener.getSellVolume()
         << "; BuyVol:"  <<  listener.getBuyVolume()
         << "; MatchVol:" <<  listener.getMatchVolume()
         << "; InsideMatchPrice:" << listener.getMatchPrice().getValue()
         << "; FarClPrice:" << listener.getFarClearingPrice().getValue()
         << "; NearClPrice:" << listener.getNearClearingPrice().getValue()
         << "; HighIndPrice:" <<  listener.getHighIndicationPrice().getValue()
         << "; LowIndPrice:" <<  listener.getLowIndicationPrice().getValue() << ")\n"
         << flush;  
    }
        
        
    void onNoOrderImbalance (
        MamdaSubscription*           subscription,
        MamdaOrderImbalanceListener& listener,
        const MamaMsg&               msg,
        MamdaOrderImbalanceRecap&    imbalance,
        MamdaOrderImbalanceUpdate&   update)
    {
         cout << "noOrderImbalance (" << subscription->getSymbol ()
         << ") (SecStatus: " << listener.getImbalanceState()
         << "; SellVol" << listener.getSellVolume()
         << "; BuyVol:  "  <<  listener.getBuyVolume()
         << "; MatchVol: " <<  listener.getMatchVolume()
         << "; InsideMatchPrice: " << listener.getMatchPrice().getValue()
         << "; FarClPrice: " << listener.getFarClearingPrice().getValue()
         << "; NearClPrice: " << listener.getNearClearingPrice().getValue()
         << "; HighIndPrice: " <<  listener.getHighIndicationPrice().getValue()
         << "; LowIndPrice: " <<  listener.getLowIndicationPrice().getValue() << ")\n"
         << flush;  
    }
        
    void onOrderImbalanceRecap (
        MamdaSubscription*           subscription,
        MamdaOrderImbalanceListener& listener,
        const MamaMsg&               msg,
        MamdaOrderImbalanceRecap&    imbalance)
    {
         cout << "OrderImbalance Recap (" 
         << subscription->getSymbol () 
         << ") \n"
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

        MamdaOrderImbalanceFields::setDictionary (*dictRequester.getDictionary ());
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
            MamdaOrderImbalanceListener* aListener = new MamdaOrderImbalanceListener();
            OrderImbalanceTicker* aTicker = new OrderImbalanceTicker();
            
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
    std::cerr << "Usage: orderimbalanceticker [-tport] tport_name [-m] middleware [-S] source [-s] symbol [options] \n";   
    exit (exitStatus);
}



