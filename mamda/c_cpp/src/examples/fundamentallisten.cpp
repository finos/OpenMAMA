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
#include <mamda/MamdaCommonFields.h>
#include <mamda/MamdaFundamentalFields.h>
#include <mamda/MamdaFundamentalHandler.h>
#include <mamda/MamdaFundamentalListener.h>
#include <mamda/MamdaFundamentals.h>
#include <iostream>
#include <vector>
#include <stdexcept>
#include "parsecmd.h"
#include "dictrequester.h"
#include "mama/MamaQueueGroup.h"

using std::exception;
using std::endl;
using std::vector;
using std::cerr;
using std::cout;

using namespace Wombat;

void usage (int exitStatus);

class FundamentalTicker : public MamdaFundamentalHandler
{
public:
    virtual ~FundamentalTicker () {}

    virtual void onFundamentals (
        MamdaSubscription*        subscription,
        MamdaFundamentalListener& listener,
        const MamaMsg&            msg,
        const MamdaFundamentals&  fundas)
    {
        cout << "DivFreq: "         << fundas.getDividendFrequency()
             << "\nMarketSegment: " << fundas.getMarketSegment()
             << "\nMarketSector: "  << fundas.getMarketSector()
             << "\n";
    }
};

int main (int argc, const char **argv)
{
    try
    {
        // Process some command line arguments:
        CommonCommandLineParser     cmdLine (argc, argv);
        // Initialise the MAMA API
        mamaBridge bridge = cmdLine.getBridge();
        Mama::open ();

        const vector<const char*>& symbolList = cmdLine.getSymbolList ();
        MamaSource*                source     = cmdLine.getSource();
        MamaQueueGroup   queues (cmdLine.getNumThreads(), bridge);

        DictRequester    dictRequester (bridge);
        dictRequester.requestDictionary (cmdLine.getDictSource());
        MamdaCommonFields::setDictionary (*dictRequester.getDictionary());
        MamdaFundamentalFields::setDictionary (*dictRequester.getDictionary());

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
            MamdaSubscription*  aSubscription  = new MamdaSubscription;
            MamdaFundamentalListener* aFundamentalListener = 
                new MamdaFundamentalListener;
            FundamentalTicker*    aTicker = new FundamentalTicker;

            aFundamentalListener->addHandler (aTicker);
            aSubscription->addMsgListener (aFundamentalListener);
            aSubscription->create (queues.getNextQueue(), source, symbol);
        }
        Mama::start (bridge);
    }
    catch (MamaStatus &e)
    {
        
        // This exception can be thrown from Mama.open (),
        // Mama::createTransport (transportName) and from 
        // MamdaSubscription constructor when entitlements is enabled.
        cerr << "MamaStatus exception in main (): " << e.toString () << endl;
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

    return 1;
}


void usage (int exitStatus)
{
    std::cerr << "Usage: fundamentallisten [-tport] tport_name [-m] middleware [-S] source [-s] symbol";                 
    exit (exitStatus);
}
