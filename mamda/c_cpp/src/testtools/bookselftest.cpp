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

//
// The purpose of this example is to illustrate use of the
// MamdaOrderBookChecker utility class, which periodically takes a
// snapshot of an order book that is also being managed by a
// MamdaOrderBookListener in real time.
//


#include <mamda/MamdaSubscription.h>
#include <mamda/MamdaOrderBookChecker.h>
#include <mamda/MamdaOrderBookFields.h>
#include <mamda/MamdaOrderBookHandler.h>
#include <mamda/MamdaOrderBookListener.h>
#include <mamda/MamdaErrorListener.h>
#include <mama/MamaSource.h>
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


class BookSelfTest : public MamdaOrderBookCheckerHandler
{
public:
    BookSelfTest () {}
    virtual ~BookSelfTest() {}

    void onSuccess (MamdaOrderBookCheckType  type)
    {
        if (gExampleLogLevel == EXAMPLE_LOG_LEVEL_NORMAL)
        {
            cout << "Successful check ("
                 << mamdaOrderBookCheckTypeToString(type) << ")\n";
        }
    }

    void onInconclusive (MamdaOrderBookCheckType  type,
                         const char*              reason)
    {
        if (gExampleLogLevel == EXAMPLE_LOG_LEVEL_NORMAL)
        {
            cout << "Inconclusive check ("
                 << mamdaOrderBookCheckTypeToString(type) << "): "
                 << ": " << reason << "\n";
        }
    }

    void onFailure (MamdaOrderBookCheckType  type,
                    const char*              reason,
                    const MamaMsg*           msg,
                    const MamdaOrderBook&    realTimeBook,
                    const MamdaOrderBook&    checkBook)
    {
        cout << "Failed check ("
             << mamdaOrderBookCheckTypeToString(type) << "): "
             << reason << "\n";
        if (gExampleLogLevel >= EXAMPLE_LOG_LEVEL_QUIET)
        {
            if (msg)
                cout << "Failed message: " << msg->toString() << "\n";
            cout << "Failed current book: ";  realTimeBook.dump(cout);
            cout << "Failed checking book: "; checkBook.dump(cout);
            cout << "\n";
        }
    }
};


int main (int argc, const char** argv)
{
    try
    {
        CommonCommandLineParser     cmdLine (argc, argv);
        // Initialise the MAMA API
        mamaBridge bridge = Mama::loadBridge (cmdLine.getMiddleware());
        Mama::open ();
               
        const vector<const char*>&
                         symbolList        = cmdLine.getSymbolList ();
        MamaSource*      source            = cmdLine.getSource();
        MamaQueueGroup   queues (cmdLine.getNumThreads(), bridge);
        DictRequester    dictRequester (bridge);
        uint32_t         intervalSecs      = cmdLine.getOptInt('i');

        if (intervalSecs == 0)
            intervalSecs = 5;

        // We might as well also enforce strict checking of order book updates.
        MamdaOrderBook::setStrictChecking (true);

        // Get and initialize the dictionary
        dictRequester.requestDictionary (cmdLine.getDictSource());
        MamdaOrderBookFields::setDictionary (*dictRequester.getDictionary());

        for (vector<const char*>::const_iterator i = symbolList.begin();
            i != symbolList.end();
            ++i)
        {
            const char* symbol = *i;
            BookSelfTest* aSelfTest = new BookSelfTest;
            MamdaOrderBookChecker*  aBookChecker  =
                new MamdaOrderBookChecker (queues.getNextQueue(), aSelfTest,
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
    std::cerr << "Usage: bookselftest [-S source] [-s symbol ...] " 
              << "[-threads x] [-m middleware] \n";
    exit(exitStatus);
}
