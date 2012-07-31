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
#include <mamda/MamdaTradeFields.h>
#include <mamda/MamdaTradeHandler.h>
#include <mamda/MamdaTradeListener.h>
#include <mamda/MamdaErrorListener.h>
#include <mamda/MamdaQualityListener.h>
#include <mamda/MamdaSecStatusListener.h>
#include <mamda/MamdaSecStatusRecap.h>
#include <mamda/MamdaSecStatusHandler.h>
#include <mamda/MamdaSecStatusFields.h>
#include <iostream>
#include <stdexcept>
#include <vector>
#include "parsecmd.h"
#include "dictrequester.h"
#include <mama/MamaQueueGroup.h>

using std::exception;
using std::endl;
using std::vector;
using std::cerr;
using std::cout;

using namespace Wombat;

void usage (int exitStatus);


class ComboTicker : public MamdaQuoteHandler
                  , public MamdaTradeHandler
                  , public MamdaSecStatusHandler
                  , public MamdaErrorListener
                  , public MamdaQualityListener
{
public:
    ComboTicker (MamdaTradeListener&  tradeListener,
                 MamdaQuoteListener&  quoteListener,
                 MamdaSecStatusListener& secStatusListener)
        : mTradeListener (tradeListener)
        , mQuoteListener (quoteListener)
        , mSecStatusListener (secStatusListener) {}

    virtual ~ComboTicker () {}

    void onQuoteRecap (
        MamdaSubscription*      subscription,
        MamdaQuoteListener&     listener,
        const MamaMsg&          msg,
        const MamdaQuoteRecap&  recap)
    {
        if (gExampleLogLevel >= EXAMPLE_LOG_LEVEL_NORMAL)
        {
            cout << "Quote Recap (" << recap.getSymbol ()
                 << " ("            << recap.getPartId () << "))\n";
        }
    }

    void onQuoteUpdate (
        MamdaSubscription*      subscription,
        MamdaQuoteListener&     listener,
        const MamaMsg&          msg,
        const MamdaQuoteUpdate& quote,
        const MamdaQuoteRecap&  recap)
    {
        if (gExampleLogLevel >= EXAMPLE_LOG_LEVEL_NORMAL)
        {
            cout << "Quote ("       << recap.getSymbol ()
                 << " ("            << recap.getPartId () << ")"
                 << ":"             << recap.getQuoteCount ()
                 << "):  "          << quote.getBidPrice().getAsString()
                 << " "             << quote.getBidSize ()
                 << " X "           << quote.getAskSize ()
                 << " "             << quote.getAskPrice().getAsString()
                 << " (seq#: "      << quote.getEventSeqNum ()
                 << "; srcTime: "   << quote.getSrcTime().getAsString()
                 << "; lineTime: "  << quote.getLineTime().getAsString()
                 << "; quoteTime: " << quote.getEventTime().getAsString()
                 << "; qual: "      << quote.getQuoteQualStr ()
                 << ")\n"
                 << flush;
        }
    }

    void onQuoteGap (
        MamdaSubscription*      subscription,
        MamdaQuoteListener&     listener,
        const MamaMsg&          msg,
        const MamdaQuoteGap&    event,
        const MamdaQuoteRecap&  recap)
    {
        if (gExampleLogLevel >= EXAMPLE_LOG_LEVEL_NORMAL)
        {
            cout << "Quote gap (" << event.getBeginGapSeqNum () << "-" 
                << event.getEndGapSeqNum () << ")\n"
                << flush;
        }
    }

    void onQuoteClosing (
        MamdaSubscription*        subscription,
        MamdaQuoteListener&       listener,
        const MamaMsg&            msg,
        const MamdaQuoteClosing&  event,
        const MamdaQuoteRecap&    recap)
    {
        if (gExampleLogLevel >= EXAMPLE_LOG_LEVEL_NORMAL)
        {
            cout << "Closing quote ("  << subscription->getSymbol ()
                 << "):  "     << event.getBidClosePrice().getAsString()
                 << " X "      << event.getAskClosePrice().getAsString()
                 << " (seq#: " << event.getEventSeqNum ()
                 << "; time: " << event.getEventTime ().getAsString()
                 << ")\n"
                 << flush;
        }
    }

    void onQuoteOutOfSequence (
        MamdaSubscription*              subscription,
        MamdaQuoteListener&             listener,
        const MamaMsg&                  msg,
        const MamdaQuoteOutOfSequence&  event,
        const MamdaQuoteRecap&          recap)
    {
        if (gExampleLogLevel >= EXAMPLE_LOG_LEVEL_NORMAL)
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
    }

    void onQuotePossiblyDuplicate (
        MamdaSubscription*                  subscription,
        MamdaQuoteListener&                 listener,
        const MamaMsg&                      msg,
        const MamdaQuotePossiblyDuplicate&  event,
        const MamdaQuoteRecap&              recap)
    {
        if (gExampleLogLevel >= EXAMPLE_LOG_LEVEL_NORMAL)
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
    }

    void onTradeRecap (
        MamdaSubscription*      subscription,
        MamdaTradeListener&     listener,
        const MamaMsg&          msg,
        const MamdaTradeRecap&  recap)
    {
        if (gExampleLogLevel >= EXAMPLE_LOG_LEVEL_NORMAL)
        {
            cout << "Trade Recap (" << recap.getSymbol ()
                 << " ("            << recap.getPartId () << ")" << "): \n"
                 << flush;
        }
    }

    void onTradeReport (
        MamdaSubscription*      subscription,
        MamdaTradeListener&     listener,
        const MamaMsg&          msg,
        const MamdaTradeReport& event,
        const MamdaTradeRecap&  recap)
    {
        if (gExampleLogLevel >= EXAMPLE_LOG_LEVEL_NORMAL)
        {
            cout << "Trade ("       << recap.getSymbol ()
                 << " ("            << recap.getPartId () << ")"
                 << ":"             << recap.getTradeCount ()
                 << "):  "          << event.getTradeVolume ()
                 << " @ "           << event.getTradePrice().getAsString()
                 << " (seq#: "      << event.getEventSeqNum()
                 << "; partId: "    << event.getTradePartId()
                 << "; srcTime: "   << event.getSrcTime().getAsString()
                 << "; lineTime: "  << event.getLineTime().getAsString()
                 << "; tradeTime: " << event.getEventTime().getAsString()
                 << "; acttime: "   << event.getActivityTime().getAsString()
                 << "; qual: "      << event.getTradeQual()
                 << "; bid: "    << mQuoteListener.getBidPrice().getAsString()
                 << "; ask; "    << mQuoteListener.getAskPrice().getAsString()
                 << ")\n"
                 << flush;
        }
    }

    void onTradeGap (
        MamdaSubscription*              subscription,
        MamdaTradeListener&             listener,
        const MamaMsg&                  msg,
        const MamdaTradeGap&            event,
        const MamdaTradeRecap&          recap) {}

    void onTradeCancelOrError (
        MamdaSubscription*              subscription,
        MamdaTradeListener&             listener,
        const MamaMsg&                  msg,
        const MamdaTradeCancelOrError&  event,
        const MamdaTradeRecap&          recap) {}

    void onTradeCorrection (
        MamdaSubscription*              subscription,
        MamdaTradeListener&             listener,
        const MamaMsg&                  msg,
        const MamdaTradeCorrection&     event,
        const MamdaTradeRecap&          recap) {}

    void onTradeClosing (
        MamdaSubscription*              subscription,
        MamdaTradeListener&             listener,
        const MamaMsg&                  msg,
        const MamdaTradeClosing&        event,
        const MamdaTradeRecap&          recap)
    {
        if (gExampleLogLevel >= EXAMPLE_LOG_LEVEL_NORMAL)
        {
            cout << "Closing trade (" << subscription->getSymbol () << "): "
                 << "price: " << event.getClosePrice().getAsString()
#ifdef VC6
                 << "; accVol: " << (long)recap.getAccVolume ()
#else
                 << "; accVol: " << recap.getAccVolume ()
#endif
                 << "\n"
                 << flush;
         }
    }

    void onTradeOutOfSequence (
        MamdaSubscription*              subscription,
        MamdaTradeListener&             listener,
        const MamaMsg&                  msg,
        const MamdaTradeOutOfSequence&  event,
        const MamdaTradeRecap&          recap)
    {
        if (gExampleLogLevel >= EXAMPLE_LOG_LEVEL_NORMAL)
        {
            cout << "Out of sequence trade (" << subscription->getSymbol ()
                 << " / "         << event.getMsgQual().getAsString()
                 << "("           << event.getMsgQual().getValue()
                 << ")): price: " << event.getTradePrice().getAsString()
                 << "; accVol: "  << recap.getAccVolume()
                 << " (seq#: "    << event.getEventSeqNum()
                 << "; time: "    << event.getEventTime().getAsString()
                 << ")\n"
                 << flush;
        }
    }
    
    void onTradePossiblyDuplicate (
        MamdaSubscription*                  subscription,
        MamdaTradeListener&                 listener,
        const MamaMsg&                      msg,
        const MamdaTradePossiblyDuplicate&  event,
        const MamdaTradeRecap&              recap)
    {
        if (gExampleLogLevel >= EXAMPLE_LOG_LEVEL_NORMAL)
        {
            cout << "Possibly duplicate trade (" << subscription->getSymbol ()
                 << " / "         << event.getMsgQual().getAsString()
                 << "("           << event.getMsgQual().getValue()
                 << ")): price: " << event.getTradePrice().getAsString()
                 << "; accVol: "  << recap.getAccVolume ()
                 << " (seq#: "    << event.getEventSeqNum()
                 << "; time: "    << event.getEventTime().getAsString()
                 << ")\n"
                 << flush;
        }
    }

     void onSecStatusRecap (
        MamdaSubscription*      subscription,
        MamdaSecStatusListener& listener,
        const MamaMsg&          msg,
        MamdaSecStatusRecap&    recap)
    {
        cout << "SecStatus recap ("     << subscription->getSymbol ()
             << "):  Action:"     << recap.getSecurityAction ()
             << "    Status:"     << recap.getSecurityStatus ()
             <<  "   Symbol:"     << recap.getIssueSymbol ()
             << "\n";
    }

    void onSecStatusUpdate (
        MamdaSubscription*      subscription,
        MamdaSecStatusListener& listener,
        const MamaMsg&          msg)
    {
        cout << "SecStatus update ("     << subscription->getSymbol ()
             << "):  Action:"     << listener.getSecurityAction ()
             << "    Status:"     << listener.getSecurityStatus ()
             <<  "   Symbol:"     << listener.getIssueSymbol ();
    }
                            
    void onError (
        MamdaSubscription*   subscription,
        MamdaErrorSeverity   severity,
        MamdaErrorCode       errorCode,
        const char*          errorStr) {}

    void onQuality (
        MamdaSubscription*   subscription,
        mamaQuality          quality) {}

private:
    MamdaTradeListener&     mTradeListener;
    MamdaQuoteListener&     mQuoteListener;
    MamdaSecStatusListener& mSecStatusListener;
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
        MamdaCommonFields::setDictionary (*dictRequester.getDictionary ());
        MamdaTradeFields::setDictionary (*dictRequester.getDictionary ());
        MamdaQuoteFields::setDictionary (*dictRequester.getDictionary ());
        MamdaSecStatusFields::setDictionary (*dictRequester.getDictionary ());

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
            MamdaSubscription*      aSubscription      = new MamdaSubscription;
            MamdaTradeListener*     aTradeListener     = new MamdaTradeListener;
            MamdaQuoteListener*     aQuoteListener     = new MamdaQuoteListener;
            MamdaSecStatusListener* aSecStatusListener = new MamdaSecStatusListener;

            ComboTicker*        aTicker = new ComboTicker (*aTradeListener,
                                                           *aQuoteListener,
                                                           *aSecStatusListener);

            aQuoteListener->addHandler     (aTicker);
            aTradeListener->addHandler     (aTicker);
            aSecStatusListener->addHandler (aTicker);
            aSubscription->addMsgListener (aQuoteListener);
            aSubscription->addMsgListener (aTradeListener);
            aSubscription->addQualityListener (aTicker);
            aSubscription->addErrorListener   (aTicker);
            aSubscription->create (queues.getNextQueue(), source, symbol);
        }

        Mama::start (bridge);
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
    std::cerr << "Usage: comboticker [-tport ] tport_name [-m ] middleware [-S ] source [-s ] symbol";                 
    exit (exitStatus);
}
