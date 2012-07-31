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
#include <mamda/MamdaQuoteFields.h>
#include <mamda/MamdaQuoteHandler.h>
#include <mamda/MamdaQuoteListener.h>
#include <mamda/MamdaErrorListener.h>
#include <mamda/MamdaQualityListener.h>
#include <stdexcept>
#include <iostream>
#include <vector>
#include "parsecmd.h"
#include "dictrequester.h"
#include "mama/MamaQueueGroup.h"

using std::vector;
using std::exception;
using std::endl;
using std::cerr;
using std::cout;

using namespace Wombat;

void usage (int exitStatus);

class QuoteTicker : public MamdaQuoteHandler
                  , public MamdaErrorListener
                  , public MamdaQualityListener
{
public:
    virtual ~QuoteTicker () {}

    void onQuoteRecap (
        MamdaSubscription*      subscription,
        MamdaQuoteListener&     listener,
        const MamaMsg&          msg,
        const MamdaQuoteRecap&  recap)
    {
        cout << "Quote Recap (" << subscription->getSymbol () << "):\n"
          << flush;
    }

    void onQuoteUpdate (
        MamdaSubscription*      subscription,
        MamdaQuoteListener&     listener,
        const MamaMsg&          msg,
        const MamdaQuoteUpdate& quote,
        const MamdaQuoteRecap&  recap)
    {
        cout << "Quote ("  << subscription->getSymbol ()
             << ":"        << recap.getQuoteCount ()
             << "):  "     << quote.getBidPrice().getAsString()
             << " "             << quote.getBidSize ()
             << " X "           << quote.getAskSize ()
             << " " << quote.getAskPrice().getAsString()
             << " (seq#: " << quote.getEventSeqNum ()
             << "; time: " << quote.getEventTime().getAsString()
             << "; qual: " << quote.getQuoteQualStr ()
             << ")\n"
             << flush;
    }

    void onQuoteGap (
        MamdaSubscription*      subscription,
        MamdaQuoteListener&     listener,
        const MamaMsg&          msg,
        const MamdaQuoteGap&    event,
        const MamdaQuoteRecap&  recap)
    {
        cout << "Quote gap (" << event.getBeginGapSeqNum () << "-" 
             << event.getEndGapSeqNum () << ")\n"
             << flush;
    }

    void onQuoteClosing (
        MamdaSubscription*        subscription,
        MamdaQuoteListener&       listener,
        const MamaMsg&            msg,
        const MamdaQuoteClosing&  event,
        const MamdaQuoteRecap&    recap)
    {
        cout << "Closing quote ("  << subscription->getSymbol ()
             << "):  "     << event.getBidClosePrice().getAsString()
             << " X "      << event.getAskClosePrice().getAsString()
             << " (seq#: " << event.getEventSeqNum ()
             << "; time: " << event.getEventTime ().getAsString()
             << ")\n"
             << flush;
    }

    void onQuoteOutOfSequence (
        MamdaSubscription*              subscription,
        MamdaQuoteListener&             listener,
        const MamaMsg&                  msg,
        const MamdaQuoteOutOfSequence&  event,
        const MamdaQuoteRecap&          recap)
    {
        cout << "Out of sequence quote ("  << subscription->getSymbol ()
             << " / "      << event.getMsgQual().getAsString()
             << "("        << event.getMsgQual().getValue()
             << ")):  "    << event.getBidPrice().getAsString()
             << " X "      << event.getAskPrice().getAsString()
             << " (seq#: " << event.getEventSeqNum ()
             << "; time: " << event.getEventTime ().getAsString()
             << ")\n"
             << flush;
    }

    void onQuotePossiblyDuplicate (
        MamdaSubscription*                  subscription,
        MamdaQuoteListener&                 listener,
        const MamaMsg&                      msg,
        const MamdaQuotePossiblyDuplicate&  event,
        const MamdaQuoteRecap&              recap)
    {
        cout << "Possibly duplicate quote ("  << subscription->getSymbol ()
             << " / "      << event.getMsgQual().getAsString()
             << "("        << event.getMsgQual().getValue()
             << ")):  "    << event.getBidPrice().getAsString()
             << " X "      << event.getAskPrice().getAsString()
             << " (seq#: " << event.getEventSeqNum ()
             << "; time: " << event.getEventTime ().getAsString()
             << ")\n"
             << flush;
    }

    void onError (
        MamdaSubscription*   subscription,
        MamdaErrorSeverity   severity,
        MamdaErrorCode       code,
        const char*          errorStr)
    {
        cout << "Quote Error ("  << subscription->getSymbol () << ")" << endl
            << flush;
    }

    void onQuality (
        MamdaSubscription*   subscription,
        mamaQuality          quality)
    {
        cout << "Quote Quality: "  
            << subscription->getSymbol () 
            << "(" 
            << mamaQuality_convertToString (quality)
            << ")\n"
            << flush;
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
        
        const vector<const char*>& symbolList = cmdLine.getSymbolList ();
        MamaSource*                source     = cmdLine.getSource();
        MamaQueueGroup   queues (cmdLine.getNumThreads(), bridge);

        DictRequester    dictRequester (bridge);
        dictRequester.requestDictionary (cmdLine.getDictSource());
        MamdaCommonFields::setDictionary (*dictRequester.getDictionary());
        MamdaQuoteFields::setDictionary (*dictRequester.getDictionary());

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
            MamdaQuoteListener* aQuoteListener = new MamdaQuoteListener;
            QuoteTicker*        aTicker        = new QuoteTicker;

            aQuoteListener->addHandler (aTicker);
            aSubscription->addMsgListener (aQuoteListener);
            aSubscription->addQualityListener (aTicker);
            aSubscription->addErrorListener (aTicker);
            aSubscription->create (queues.getNextQueue(), source, symbol);
        }

        Mama::start(bridge);
    }
    catch (MamaStatus &e)
    {
        // This exception can be thrown from Mama::open (),
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
    std::cerr << "Usage: quoteticker [-tport]tport_name [-m] middleware [-S] source [-s] symbol\n";
    exit (exitStatus);
}
