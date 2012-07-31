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
#include <mamda/MamdaMultiParticipantManager.h>
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
#include <iostream>
#include <stdexcept>
#include <vector>
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

/*
 * In this example, we create one ComboTicker for *each* of the
 * individual participants and consolidated member of the
 * MamdaMultiParticipantManager (i.e., one each for the BBO and each
 * participant).
 *
 * If the developer only wanted a subset of these members, then those
 * could be selected in his/her implementation of
 * MamdaMultiParticipantManager.  Similarly, the developer may not
 * want to create listeners for both trades and quotes for each
 * participant.
 *
 * Note: if trades are reported, they will be reported twice: once for
 * the participant and once for the consolidated.  Depending upon
 * exchange and oversight rules, there may be differences between the
 * trade reports for a participant versus the same report for the
 * consolidated.  For example, by certain rules, a trade may qualify
 * to update the official last price for an exchange but not the
 * consolidated last price.  The aggregate volume traded will, of
 * course, be different for each participant and the consolidated.
 */
class ComboTicker : public MamdaQuoteHandler
                  , public MamdaTradeHandler
                  , public MamdaErrorListener
                  , public MamdaQualityListener
{
public:
    ComboTicker (MamdaTradeListener&  tradeListener,
                 MamdaQuoteListener&  quoteListener)
        : mTradeListener (tradeListener)
        , mQuoteListener (quoteListener) {}

    virtual ~ComboTicker () {}

    void onQuoteRecap (
        MamdaSubscription*      subscription,
        MamdaQuoteListener&     listener,
        const MamaMsg&          msg,
        const MamdaQuoteRecap&  recap)
    {
        cout << "Quote Recap (" << recap.getSymbol ()
             << " ("            << recap.getPartId () << "))\n";
    }

    void onQuoteUpdate (
        MamdaSubscription*      subscription,
        MamdaQuoteListener&     listener,
        const MamaMsg&          msg,
        const MamdaQuoteUpdate& quote,
        const MamdaQuoteRecap&  recap)
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
             << ")\n";
    }

    void onQuoteGap (
        MamdaSubscription*      subscription,
        MamdaQuoteListener&     listener,
        const MamaMsg&          msg,
        const MamdaQuoteGap&    event,
        const MamdaQuoteRecap&  recap)
    {
        cout << "Quote gap (" << event.getBeginGapSeqNum () << "-" 
             << event.getEndGapSeqNum () << ")\n";
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
             << "; time: " << event.getEventTime().getAsString()
             << ")\n";
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
             << ")\n";
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
             << ")\n";
    }

    void onTradeRecap (
        MamdaSubscription*      subscription,
        MamdaTradeListener&     listener,
        const MamaMsg&          msg,
        const MamdaTradeRecap&  recap)
    {
        cout << "Trade Recap (" << recap.getSymbol ()
             << " ("            << recap.getPartId () << ")" << "): \n";
    }

    void onTradeReport (
        MamdaSubscription*      subscription,
        MamdaTradeListener&     listener,
        const MamaMsg&          msg,
        const MamdaTradeReport& event,
        const MamdaTradeRecap&  recap)
    {
        cout << "Trade ("       << recap.getSymbol ()
             << " ("            << recap.getPartId () << ")"
             << ":"             << recap.getTradeCount ()
             << "):  "          << event.getTradeVolume ()
             << " @ "           << event.getTradePrice().getAsString()
             << " (seq#: "      << event.getEventSeqNum ()
             << "; partId: "    << event.getTradePartId ()
             << "; srcTime: "   << event.getSrcTime().getAsString()
             << "; lineTime: "  << event.getLineTime().getAsString()
             << "; tradeTime: " << event.getEventTime().getAsString()
             << "; acttime: "   << event.getActivityTime().getAsString()
             << "; qual: "      << event.getTradeQual ()
             << "; bid: "       << mQuoteListener.getBidPrice().getAsString()
             << "; ask; "       << mQuoteListener.getAskPrice().getAsString()
             << ")\n";
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
        cout << "Closing trade (" << recap.getSymbol ()
             << " ("              << recap.getPartId () << ")): "
             << "price: "         << event.getClosePrice().getAsString()
#ifdef VC6
             << "; accVol: "      << (long)recap.getAccVolume ()
#else
             << "; accVol: "      << recap.getAccVolume ()
#endif
             << "\n";
    }

    void onTradeOutOfSequence (
        MamdaSubscription*              subscription,
        MamdaTradeListener&             listener,
        const MamaMsg&                  msg,
        const MamdaTradeOutOfSequence&  event,
        const MamdaTradeRecap&          recap)
    {
        cout << "Out of sequence trade (" << subscription->getSymbol ()
             << " / "         << event.getMsgQual().getAsString()
             << "("           << event.getMsgQual().getValue()
             << ")): price: " << event.getTradePrice().getAsString()
             << "; accVol: "  << recap.getAccVolume()
             << " (seq#: "    << event.getEventSeqNum()
             << "; time: "    << event.getEventTime().getAsString()
             << ")\n";
    }
    
    void onTradePossiblyDuplicate (
        MamdaSubscription*                  subscription,
        MamdaTradeListener&                 listener,
        const MamaMsg&                      msg,
        const MamdaTradePossiblyDuplicate&  event,
        const MamdaTradeRecap&              recap)
    {
        cout << "Possibly duplicate trade (" << subscription->getSymbol ()
             << " / "         << event.getMsgQual().getAsString()
             << "("           << event.getMsgQual().getValue()
             << ")): price: " << event.getTradePrice().getAsString()
             << "; accVol: "  << recap.getAccVolume ()
             << " (seq#: "    << event.getEventSeqNum()
             << "; time: "    << event.getEventTime().getAsString()
             << ")\n";
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
    MamdaTradeListener&  mTradeListener;
    MamdaQuoteListener&  mQuoteListener;
};

class MultiParticipantExample : public MamdaMultiParticipantHandler
{
public:
    void onConsolidatedCreate (
        MamdaSubscription*             subscription,
        MamdaMultiParticipantManager&  manager)
    {
        // Create a trade and quote listener for the consolidated
        // member.
        MamdaTradeListener* aTradeListener = new MamdaTradeListener;
        MamdaQuoteListener* aQuoteListener = new MamdaQuoteListener;
        ComboTicker*        aTicker = new ComboTicker (*aTradeListener,
                                                       *aQuoteListener);
        aQuoteListener->addHandler (aTicker);
        aTradeListener->addHandler (aTicker);
        manager.addConsolidatedListener (aQuoteListener);
        manager.addConsolidatedListener (aTradeListener);
    }

    void onParticipantCreate (
        MamdaSubscription*             subscription,
        MamdaMultiParticipantManager&  manager,
        const char*                    partId,
        bool                           isPrimary)
    {
        // Create a trade and quote listener for the regional member.
        MamdaTradeListener* aTradeListener = new MamdaTradeListener;
        MamdaQuoteListener* aQuoteListener = new MamdaQuoteListener;
        ComboTicker*        aTicker = new ComboTicker (*aTradeListener,
                                                       *aQuoteListener);
        aQuoteListener->addHandler (aTicker);
        aTradeListener->addHandler (aTicker);
        manager.addParticipantListener (aQuoteListener, partId);
        manager.addParticipantListener (aTradeListener, partId);
        // Note that these listeners could have been added up front
        // (and this callback left empty) if the exchange IDs are
        // known before handle.
    }
};

int main (int argc, const char* argv[])
{
    try
    {
        CommonCommandLineParser     cmdLine (argc, argv);
        // Initialise the MAMA API
        mamaBridge bridge = cmdLine.getBridge();
        Mama::open ();
 
        const vector<const char*>&  symbolList    = cmdLine.getSymbolList ();
        MamaSource*                 source        = cmdLine.getSource();
        MamaQueueGroup  queues (cmdLine.getNumThreads(), bridge);

        double  throttleRate  = cmdLine.getThrottleRate ();
        if ((throttleRate > 100.0) || (throttleRate <= 0.0))
        {
            // We don't really want to swamp the FHs with these types of
            // subscriptions.  
            throttleRate = 100.0;
        }
        source->getTransport()->setOutboundThrottle (throttleRate, 
                                                     MAMA_THROTTLE_DEFAULT);

        DictRequester   dictRequester (bridge);
        dictRequester.requestDictionary (cmdLine.getDictSource());
        MamdaCommonFields::setDictionary (*dictRequester.getDictionary ());
        MamdaTradeFields::setDictionary (*dictRequester.getDictionary ());
        MamdaQuoteFields::setDictionary (*dictRequester.getDictionary ());

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
            MultiParticipantExample*  anExampleHandler =
                new MultiParticipantExample;
            MamdaMultiParticipantManager*  aConsManager =
                new MamdaMultiParticipantManager (symbol);

            aConsManager->addHandler (anExampleHandler);
            aSubscription->addMsgListener (aConsManager);
            aSubscription->setType (MAMA_SUBSC_TYPE_GROUP);
            aSubscription->create (queues.getNextQueue(), source, symbol);
        }
        Mama::start(bridge);
    }
    catch (MamaStatus &e)
    {
        
        // This exception can be thrown from Mama::start (),
        // Mama::createTransport (transportName) and from
        // MamdaSubscription constructor when entitlements is
        // enabled.
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
    std::cerr << "Usage: multipartticker [-tport] tport_name [-m] middleware [-S] source [-s] symbol ";              
    exit (exitStatus);
}
