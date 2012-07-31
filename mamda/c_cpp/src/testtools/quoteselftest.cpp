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

#include <mamda/MamdaErrorListener.h>
#include <mamda/MamdaQuoteChecker.h>
#include <mamda/MamdaQuoteFields.h>
#include <mama/MamaSource.h>
#include <mama/mamacpp.h>
#include <mama/MamaQueueGroup.h>
#include <iostream>
#include <vector>
#include <stdexcept>
#include "../examples/parsecmd.h"
#include "../examples/dictrequester.h"

using std::exception;
using std::endl;
using std::vector;
using std::cerr;
using std::cout;
using namespace Wombat;

void usage (int exitStatus);

class QuoteSelfTest : public MamdaCheckerHandler
{
public:
    QuoteSelfTest () {}
    virtual ~QuoteSelfTest() {}

    void onSuccess (MamdaCheckerType type)
    {
        if (gExampleLogLevel == EXAMPLE_LOG_LEVEL_NORMAL)
        {
            cout << "Successful check ("
                 << mamdaCheckTypeToString(type) << ")\n";
        }
    }

    void onInconclusive (MamdaCheckerType  type,
                         const char*       reason)
    {
        if (gExampleLogLevel == EXAMPLE_LOG_LEVEL_NORMAL)
        {
            cout << "Inconclusive check ("
                 << mamdaCheckTypeToString(type) << "): "
                 << ": " << reason << "\n";
        }
    }

    void onFailure (MamdaCheckerType  type,
                    const char*       reason,
                    const MamaMsg&    msg)
    {
        cout << "Failed check ("
             << mamdaCheckTypeToString(type) << "): "
             << reason << "\n";
        if (gExampleLogLevel >= EXAMPLE_LOG_LEVEL_QUIET)
        {
            cout << "Failed message: " << msg.toString() << "\n";
        }
    }
};

int main (int argc, const char** argv)
{
    try
    {
        CommonCommandLineParser     cmdLine (argc, argv);
        //Initialise the MAMA API
        mamaBridge bridge = cmdLine.getBridge();
        Mama::open ();
 
        const vector<const char*>&
                         symbolList        = cmdLine.getSymbolList ();
        MamaSource*      source            = cmdLine.getSource();
        MamaQueueGroup   queues (cmdLine.getNumThreads(), bridge);
        DictRequester    dictRequester (bridge);
        uint32_t         intervalSecs      = cmdLine.getOptInt('i');
        
        if (intervalSecs == 0)
        {
            intervalSecs = 5;
        }
        
        // Get and initialize the dictionary
        dictRequester.requestDictionary (cmdLine.getDictSource());
        MamdaQuoteFields::setDictionary (*dictRequester.getDictionary());
        
        for (vector<const char*>::const_iterator i = symbolList.begin();
            i != symbolList.end();
            ++i)
        {
            const char* symbol = *i;
            QuoteSelfTest* aSelfTest = new QuoteSelfTest;
            MamdaQuoteChecker*  aQuotehecker  =
                new MamdaQuoteChecker (queues.getNextQueue(), aSelfTest,
                                           source, symbol, intervalSecs);
        }
        Mama::start(bridge);
    }
    catch (MamaStatus &e)
    {
        // This exception can be thrown from Mama.open ()
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
    std::cerr << "Usage: quoteselftest [-tport] tport_name [-S] source [-s] symbol\n"; 
    exit(exitStatus);
}
