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

/**
 * An example of MAMDA option chain "view" processing.  A "view" on an
 * option chain is the set of option contracts that fits into a
 * sliding set of strike prices and expiration cycles.  The strike
 * prices included in the view may change as the price of underlying
 * security changes.
 *
 * This example suffers from a minor lack of configurability: while
 * it does support subscriptions to multiple underlying symbols (with
 * multiple -s options), it does not allow a different source to be
 * specified for each option.  Of course, any real world example would
 * deal with this properly. 
 */

#include <mama/mamacpp.h>
#include <mamda/MamdaSubscription.h>
#include <mamda/MamdaOptionChain.h>
#include <mamda/MamdaOptionChainHandler.h>
#include <mamda/MamdaOptionChainListener.h>
#include <mamda/MamdaOptionChainView.h>
#include <mamda/MamdaOptionContract.h>
#include <mamda/MamdaOptionFields.h>
#include <mamda/MamdaQuoteFields.h>
#include <mamda/MamdaQuoteHandler.h>
#include <mamda/MamdaQuoteListener.h>
#include <mamda/MamdaTradeFields.h>
#include <mamda/MamdaTradeHandler.h>
#include <mamda/MamdaTradeListener.h>
#include <mamda/MamdaErrorListener.h>
#include <mamda/MamdaQualityListener.h>
#include <mamda/MamdaFundamentalHandler.h>
#include <mamda/MamdaFundamentalListener.h>
#include <mamda/MamdaFundamentals.h>
#include <mamda/MamdaFundamentalFields.h>
#include <stdexcept>
#include <iostream>
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

class OptionChainDisplay : public MamdaOptionChainHandler
                         , public MamdaErrorListener
                         , public MamdaQualityListener
{
public:
    OptionChainDisplay (MamdaOptionChainView&  view)
        : mView (view)
        , mGotRecap (false) {}

    virtual ~OptionChainDisplay () {}

    void onOptionChainRecap (
        MamdaSubscription*              subscription,
        MamdaOptionChainListener&       listener,
        const MamaMsg&                  msg,
        MamdaOptionChain&               chain)
    {
        mGotRecap = true;
        printView ();
    }

    void onOptionContractCreate (
        MamdaSubscription*              subscription,
        MamdaOptionChainListener&       listener,
        const MamaMsg&                  msg,
        MamdaOptionContract&            contract,
        MamdaOptionChain&               chain);

    void onOptionSeriesUpdate (
        MamdaSubscription*              subscription,
        MamdaOptionChainListener&       listener,
        const MamaMsg&                  msg,
        const MamdaOptionSeriesUpdate&  event,
        MamdaOptionChain&               chain)
    {
        printView ();
    }

    void onOptionChainGap (
        MamdaSubscription*              subscription,
        MamdaOptionChainListener&       listener,
        const MamaMsg&                  msg,
        MamdaOptionChain&               chain)
    {
        printView ();
    }

    void onError (
        MamdaSubscription*   subscription,
        MamdaErrorSeverity   severity,
        MamdaErrorCode       errorCode,
        const char*          errorStr) {}

    void onQuality (
        MamdaSubscription*   subscription,
        mamaQuality          quality) {}

    void printView       ();
    void printExpireStrikes (const MamdaOptionExpirationStrikes* expireStrikes);
    void printContractSet   (const MamdaOptionContractSet*       contract,
                             const char*                         putCallStr);
    void printContract      (const MamdaOptionContract*          contract);

private:
    MamdaOptionChainView&  mView;
    bool                   mGotRecap;
};


/**
 * OptionContractTicker is a class that we can use to handle updates
 * to individual option contracts.  This example just prints
 * individual trades and quotes.  Instances of this class are
 * associated with actual option contracts in the
 * OptionChainDisplay::onOptionSeriesUpdate method, above.  The trade
 * and quote information of the underlying is accessible via the chain
 * argument.
 */
class OptionContractTicker : public MamdaTradeHandler
                           , public MamdaQuoteHandler
                           , public MamdaFundamentalHandler
{
public:
    OptionContractTicker (MamdaOptionContract&  contract,
                          MamdaOptionChain&     chain,
                          OptionChainDisplay&   display)
        : mContract (contract)
        , mChain (chain)
        , mDisplay (display) {}

    virtual ~OptionContractTicker () {}

    void onTradeRecap (
        MamdaSubscription*      subscription,
        MamdaTradeListener&     listener,
        const MamaMsg&          msg,
        const MamdaTradeRecap&  recap)
    {
        // Refresh the "display" of this contract.
        mDisplay.printContract (&mContract);
    }

    void onTradeReport (
        MamdaSubscription*      subscription,
        MamdaTradeListener&     listener,
        const MamaMsg&          msg,
        const MamdaTradeReport& event,
        const MamdaTradeRecap&  recap)
    {
        // Do something with a specific trade report.
        mDisplay.printContract (&mContract);
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
        cout << "Option trade closing: " 
             << event.getClosePrice().getAsString() << " - " 
             << event.getIsIndicative() << "\n";
    }

    void onTradeOutOfSequence (
        MamdaSubscription*              subscription,
        MamdaTradeListener&             listener,
        const MamaMsg&                  msg,
        const MamdaTradeOutOfSequence&  event,
        const MamdaTradeRecap&          recap) 
    {
        cout << "Option out of sequence trade (" 
             << subscription->getSymbol ()
             << " / "                    << event.getMsgQual().getAsString()
             << "("                      << event.getMsgQual().getValue()
             << ")): underlying trade: " << event.getTradeVolume ()
             << " @ "                    << event.getTradePrice().getAsString()
             << " (seq#: "               << event.getEventSeqNum()
             << "; time: "               << event.getEventTime().getAsString()
             << ")\n"; 
    }

    void onTradePossiblyDuplicate (
        MamdaSubscription*                  subscription,
        MamdaTradeListener&                 listener,
        const MamaMsg&                      msg,
        const MamdaTradePossiblyDuplicate&  event,
        const MamdaTradeRecap&              recap) 
    {        
        cout << "Option possibly duplicate trade (" 
             << subscription->getSymbol ()
             << " / "                    << event.getMsgQual().getAsString()
             << "("                      << event.getMsgQual().getValue()
             << ")): underlying trade: " << event.getTradeVolume ()
             << " @ "                    << event.getTradePrice().getAsString()
             << " (seq#: "               << event.getEventSeqNum()
             << "; time: "               << event.getEventTime().getAsString()
             << ")\n";
    }

    void onQuoteRecap (
        MamdaSubscription*      subscription,
        MamdaQuoteListener&     listener,
        const MamaMsg&          msg,
        const MamdaQuoteRecap&  recap)
    {
        // Refresh the "display" of this contract.
        mDisplay.printContract (&mContract);
    }

    void onQuoteUpdate (
        MamdaSubscription*      subscription,
        MamdaQuoteListener&     listener,
        const MamaMsg&          msg,
        const MamdaQuoteUpdate& quote,
        const MamdaQuoteRecap&  recap)
    {
        // Do something with a specific quote.
        mDisplay.printContract (&mContract);
    }

    void onQuoteGap (
        MamdaSubscription*      subscription,
        MamdaQuoteListener&     listener,
        const MamaMsg&          msg,
        const MamdaQuoteGap&    event,
        const MamdaQuoteRecap&  recap) {}

    void onQuoteClosing (
        MamdaSubscription*        subscription,
        MamdaQuoteListener&       listener,
        const MamaMsg&            msg,
        const MamdaQuoteClosing&  event,
        const MamdaQuoteRecap&    recap)
    {
        cout << "Option quote closing: "
             << recap.getBidSize() << "x"
             << recap.getBidPrice().getAsString() << "   "
             << recap.getAskPrice().getAsString() << "x"
             << recap.getAskSize() << "\n";
    }

    void onQuoteOutOfSequence (
        MamdaSubscription*              subscription,
        MamdaQuoteListener&             listener,
        const MamaMsg&                  msg,
        const MamdaQuoteOutOfSequence&  event,
        const MamdaQuoteRecap&          recap) 
    {
        cout << "Option quote out of sequence: ("
             << event.getMsgQual().getAsString() << "("
             << event.getMsgQual().getValue() << ")): "
             << recap.getBidSize() << "x"
             << recap.getBidPrice().getAsString() << "   "
             << recap.getAskPrice().getAsString() << "x"
             << recap.getAskSize () 
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
   
        cout << "Option quote possibly duplicate: ("
             << event.getMsgQual().getAsString() << "("
             << event.getMsgQual().getValue() << ")): "
             << recap.getBidSize() << "x"
             << recap.getBidPrice().getAsString() << "   "
             << recap.getAskPrice().getAsString() << "x"
             << recap.getAskSize () 
             << " (seq#: " << event.getEventSeqNum ()
             << "; time: " << event.getEventTime ().getAsString()
             << ")\n";           
    }

    void onFundamentals (
        MamdaSubscription*        subscription,
        MamdaFundamentalListener& listener,
        const MamaMsg&            msg,
        const MamdaFundamentals&  fundas) {}

private:
    MamdaOptionContract&  mContract;
    MamdaOptionChain&     mChain;
    OptionChainDisplay&   mDisplay;
};


/**
 * UnderlyingTicker is a class that we can use to handle updates
 * to the underlying security.  This example just prints
 * individual trades and quotes.
 */
class UnderlyingTicker : public MamdaTradeHandler
                       , public MamdaQuoteHandler
                       , public MamdaFundamentalHandler
{
public:
    // If the UnderlyingTicker needs to refer to the chain, then
    // a constructor can be used to pass the chain object as follows:
    UnderlyingTicker (MamdaOptionChain& chain)
        : mChain (chain) {}

    void onTradeRecap (
        MamdaSubscription*      subscription,
        MamdaTradeListener&     listener,
        const MamaMsg&          msg,
        const MamdaTradeRecap&  recap) {}

    void onTradeReport (
        MamdaSubscription*      subscription,
        MamdaTradeListener&     listener,
        const MamaMsg&          msg,
        const MamdaTradeReport& trade,
        const MamdaTradeRecap&  recap)
    {
        cout << "Underlying trade: " << trade.getTradeVolume ()
             << " @ " << trade.getTradePrice().getAsString() << "\n";
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
        cout << "Underlying trade closing: " 
             << event.getClosePrice().getAsString() << " - " 
             << event.getIsIndicative() << "\n";
    }

    void onTradeOutOfSequence (
        MamdaSubscription*              subscription,
        MamdaTradeListener&             listener,
        const MamaMsg&                  msg,
        const MamdaTradeOutOfSequence&  event,
        const MamdaTradeRecap&          recap) 
    {
        cout << "Underlying out of sequence trade (" 
             << subscription->getSymbol ()
             << " / "                    << event.getMsgQual().getAsString()
             << "("                      << event.getMsgQual().getValue()
             << ")): underlying trade: " << event.getTradeVolume ()
             << " @ "                    << event.getTradePrice().getAsString()
             << " (seq#: "               << event.getEventSeqNum()
             << "; time: "               << event.getEventTime().getAsString()
             << ")\n";        
    }

    void onTradePossiblyDuplicate (
        MamdaSubscription*                  subscription,
        MamdaTradeListener&                 listener,
        const MamaMsg&                      msg,
        const MamdaTradePossiblyDuplicate&  event,
        const MamdaTradeRecap&              recap) 
    {
        cout << "Underlying possibly duplicate trade (" 
             << subscription->getSymbol ()
             << " / "                    << event.getMsgQual().getAsString()
             << "("                      << event.getMsgQual().getValue()
             << ")): underlying trade: " << event.getTradeVolume ()
             << " @ "                    << event.getTradePrice().getAsString()
             << " (seq#: "               << event.getEventSeqNum()
             << "; time: "               << event.getEventTime().getAsString()
             << ")\n";        
    }

    void onQuoteRecap (
        MamdaSubscription*      subscription,
        MamdaQuoteListener&     listener,
        const MamaMsg&          msg,
        const MamdaQuoteRecap&  recap) {}

    void onQuoteUpdate (
        MamdaSubscription*      subscription,
        MamdaQuoteListener&     listener,
        const MamaMsg&          msg,
        const MamdaQuoteUpdate& quote,
        const MamdaQuoteRecap&  recap)
    {
        cout << "Underlying quote: "
             << quote.getBidSize()  << "x"
             << quote.getBidPrice().getAsString() << "   "
             << quote.getAskPrice().getAsString() << "x"
             << quote.getAskSize() << "   "
             << "mid=" << recap.getQuoteMidPrice().getAsString() << "\n";
        StrikeSet strikeSet;
        mChain.getStrikesWithinPercent (strikeSet, 15.0,
                                        MAMDA_AT_THE_MONEY_COMPARE_MID_QUOTE);
        if (strikeSet.empty ())
            cout << "  no strikes within 15%\n";
        else
            cout << "  strikes within 15%: " << *strikeSet.begin ()
                 << " " << *strikeSet.rbegin () << "\n";
    }

    void onQuoteGap (
        MamdaSubscription*      subscription,
        MamdaQuoteListener&     listener,
        const MamaMsg&          msg,
        const MamdaQuoteGap&    event,
        const MamdaQuoteRecap&  recap) {}

    void onQuoteClosing (
        MamdaSubscription*        subscription,
        MamdaQuoteListener&       listener,
        const MamaMsg&            msg,
        const MamdaQuoteClosing&  event,
        const MamdaQuoteRecap&    recap)
    {
        cout << "Underlying quote closing: "
             << recap.getBidSize() << "x"
             << recap.getBidPrice().getAsString() << "   "
             << recap.getAskPrice().getAsString() << "x"
             << recap.getAskSize() << "\n";
    } 

    void onQuoteOutOfSequence (
        MamdaSubscription*              subscription,
        MamdaQuoteListener&             listener,
        const MamaMsg&                  msg,
        const MamdaQuoteOutOfSequence&  event,
        const MamdaQuoteRecap&          recap) 
    {
        
        cout << "Underlying quote out of sequence: ("
             << event.getMsgQual().getAsString() << "("
             << event.getMsgQual().getValue() << ")): "
             << recap.getBidSize() << "x"
             << recap.getBidPrice().getAsString() << "   "
             << recap.getAskPrice().getAsString() << "x"
             << recap.getAskSize () 
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
        cout << "Underlying quote possibly duplicate: ("
             << event.getMsgQual().getAsString() << "("
             << event.getMsgQual().getValue() << ")): "
             << recap.getBidSize() << "x"
             << recap.getBidPrice().getAsString() << "   "
             << recap.getAskPrice().getAsString() << "x"
             << recap.getAskSize () 
             << " (seq#: " << event.getEventSeqNum ()
             << "; time: " << event.getEventTime ().getAsString()
             << ")\n";
    }

    void onFundamentals (
        MamdaSubscription*        subscription,
        MamdaFundamentalListener& listener,
        const MamaMsg&            msg,
        const MamdaFundamentals&  fundas)
    {
        cout << "Underlying Fundamentals: "
             << fundas.getDividendPayDate() << " - "
             << fundas.getDividendPrice() << "\n";
    }

private:
    MamdaOptionChain&  mChain;
};


int main (int argc, const char** argv)
{
    try
    {
        // Process some command line arguments:
        CommonCommandLineParser     cmdLine (argc, argv);
        // Initialise the MAMA API
        mamaBridge bridge = cmdLine.getBridge();
        Mama::open ();
        
        const vector<const char*>&  symbolList    = cmdLine.getSymbolList ();
        MamaSource*                 source        = cmdLine.getSource();
        MamaSource*                 optionSource  = cmdLine.getOptionSource();
        MamaQueueGroup  queues (cmdLine.getNumThreads(), bridge);

        DictRequester   dictRequester (bridge);
        dictRequester.requestDictionary (cmdLine.getDictSource());
        MamdaTradeFields::setDictionary (*dictRequester.getDictionary());
        MamdaQuoteFields::setDictionary (*dictRequester.getDictionary());
        MamdaFundamentalFields::setDictionary (*dictRequester.getDictionary());
        MamdaOptionFields::setDictionary (*dictRequester.getDictionary());

        const char* symbolMapFile = cmdLine.getSymbolMapFile ();
        if (symbolMapFile)
        {
            MamaSymbolMapFile* aMap = new MamaSymbolMapFile;
            if (MAMA_STATUS_OK == aMap->load (symbolMapFile))
            {
                source->getTransport()->setSymbolMap (aMap);
                optionSource->getTransport()->setSymbolMap (aMap);
            }
        }

        for (vector<const char*>::const_iterator i = symbolList.begin ();
            i != symbolList.end ();
            ++i)
        {
            const char* symbol = *i;
            MamaQueue* queue = queues.getNextQueue ();

            // Create chain and listener objects.
            MamdaTradeListener* aBaseTradeListener = new MamdaTradeListener;
            MamdaQuoteListener* aBaseQuoteListener = new MamdaQuoteListener;
            MamdaFundamentalListener* aFundamentalListener
                = new MamdaFundamentalListener;
            MamdaOptionChain*   anOptionChain = new MamdaOptionChain ("ORCL");
            anOptionChain->setUnderlyingQuoteListener (aBaseQuoteListener);
            anOptionChain->setUnderlyingTradeListener (aBaseTradeListener);
            MamdaOptionChainListener*  anOptionListener =
                new MamdaOptionChainListener (anOptionChain);

            // Set up the view (could be configurable, of course):
            MamdaOptionChainView* anOptionView =
                new MamdaOptionChainView (*anOptionChain);
            anOptionView->setNumberOfExpirations (2);
            anOptionView->setStrikeRangeNumber (4);
            anOptionView->setJitterMargin (1.0);

            // Create our handlers (the UnderlyingTicker and
            // OptionChainDisplay could be a single class).
            UnderlyingTicker*   aBaseTicker = 
                new UnderlyingTicker (*anOptionChain);
            OptionChainDisplay*  aDisplay =
                new OptionChainDisplay (*anOptionView);

            // Create subscriptions for underlying and option chain:
            MamdaSubscription*  aBaseSubscription    = new MamdaSubscription;
            MamdaSubscription*  anOptionSubscription = new MamdaSubscription;

            // Register for underlying quote and trade events.
            aBaseTradeListener->addHandler (aBaseTicker);
            aBaseQuoteListener->addHandler (aBaseTicker);
            aFundamentalListener->addHandler (aBaseTicker);
            aBaseSubscription->addMsgListener (aBaseQuoteListener);
            aBaseSubscription->addMsgListener (aBaseTradeListener);
            aBaseSubscription->addMsgListener (aFundamentalListener);
            aBaseSubscription->create (queue, source, symbol);

            // Register for underlying option events.
            anOptionListener->addHandler (aDisplay);
            anOptionListener->addHandler (anOptionView);

            // We set the timeout to 1 for this example because we
            // currently use the timeout feature to determine when
            // to say that we have received all of the initials.

            anOptionSubscription->addMsgListener (anOptionListener);
            anOptionSubscription->addQualityListener (aDisplay);
            anOptionSubscription->addErrorListener (aDisplay);
            anOptionSubscription->setTimeout (1);
            anOptionSubscription->setType (MAMA_SUBSC_TYPE_GROUP);
            anOptionSubscription->create (queue, optionSource, symbol);
        }
        Mama::start(bridge);
    }
    catch (MamaStatus& e)
    {
        // This exception can be thrown from Mama::start ()
        // Mama::createTransport (transportName) and from
        // MamdaSubscription constructor when entitlements is enabled.
        cerr << "Exception in main (): " << e.toString () << endl;
        exit (1);
    }
    catch (exception& ex)
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
    std::cerr << "Usage: optionview [-tport] tport_name [-m] middleware [-S] source [-s] symbol "
                 "[-OS] option_source\n";
    exit (exitStatus);
}


void OptionChainDisplay::onOptionContractCreate (
    MamdaSubscription*          subscription,
    MamdaOptionChainListener&   listener,
    const MamaMsg&              msg,
    MamdaOptionContract&        contract,
    MamdaOptionChain&           chain)
{
    // (Optional: create a trade/quote handler for the individual
    // option contract.)
    OptionContractTicker* aTicker = 
        new OptionContractTicker (contract, chain, *this);
    contract.addQuoteHandler (aTicker);
    contract.addTradeHandler (aTicker);
    contract.addFundamentalHandler (aTicker);
}


void OptionChainDisplay::printView ()
{
    if (!mGotRecap)
        return;

    cout << "Chain: " << mView.getSymbol () << "\n";

    // Loop through the expiration date sets (within the view):
    const MamdaOptionExpirationDateSet& expireDates =
        mView.getExpireDateSet ();
    MamdaOptionExpirationDateSet::const_iterator expireEnd = expireDates.end ();
    MamdaOptionExpirationDateSet::const_iterator expireIter =
                                                           expireDates.begin ();
    while (expireIter != expireEnd)
    {
        const MamdaOptionExpirationStrikes* expireStrikes = expireIter->second;
        printExpireStrikes (expireStrikes);
        ++expireIter;
    }

    cout << "\nDone with chain\n";
}

void OptionChainDisplay::printExpireStrikes (
    const MamdaOptionExpirationStrikes*  expireStrikes)
{
    // Loop through the strike prices (within the view):
    MamdaOptionExpirationStrikes::const_iterator strikeEnd = 
        expireStrikes->end ();
    MamdaOptionExpirationStrikes::const_iterator strikeIter = 
        expireStrikes->begin ();
    while (strikeIter != strikeEnd)
    {
        const MamdaOptionStrikeSet* strikeSet = strikeIter->second;

        const MamdaOptionContractSet* callContracts = strikeSet->getCallSet ();
        const MamdaOptionContractSet* putContracts  = strikeSet->getPutSet ();
        const char*  expireDateStr = strikeSet->getExpireDateStr ();
        double       strikePrice   = strikeSet->getStrikePrice ();

        cout << "\nStrike: " << expireDateStr << "  " << strikePrice << "\n";
        printContractSet (callContracts, "Call");
        printContractSet (putContracts,  "Put");
        ++strikeIter;
    }
}

void OptionChainDisplay::printContractSet (
    const MamdaOptionContractSet*  contractSet,
    const char*                    putCallStr)
{
    if (!contractSet)
        return;
    const MamdaOptionContract*  bbo   = contractSet->getBboContract ();
    const MamdaOptionContract*  wbbo  = contractSet->getWombatBboContract ();

    cout << "  " << putCallStr << " options:\n";
    printContract (bbo);
    printContract (wbbo);

    MamdaOptionContractSet::const_iterator end = contractSet->end ();
    MamdaOptionContractSet::const_iterator i;
    for (i = contractSet->begin (); i != end; ++i)
    {
        printContract (i->second);
    }
}

void OptionChainDisplay::printContract (const MamdaOptionContract*  contract)
{
    if (!mGotRecap)
        return;
    if (!contract)
        return;

    const MamdaTradeRecap& tradeRecap     = contract->getTradeInfo ();
    const MamdaQuoteRecap& quoteRecap     = contract->getQuoteInfo ();
    const MamdaFundamentals& fundamentals = contract->getFundamentalsInfo ();
    const char*       symbol       = contract->getSymbol ();
    const char*       exchange     = contract->getExchange ();
    const char*       expireDate   = contract->getExpireDateStr ();
    double            strikePrice  = contract->getStrikePrice ();
    long              openInterest = contract->getOpenInterest ();
    const MamaPrice&  lastPrice    = tradeRecap.getLastPrice ();
    mama_quantity_t   accVolume    = tradeRecap.getAccVolume ();
    const MamaPrice&  bidPrice     = quoteRecap.getBidPrice ();
    const MamaPrice&  askPrice     = quoteRecap.getAskPrice ();
    double            histVola     = fundamentals.getHistoricalVolatility ();
    double            riskFreeRate = fundamentals.getRiskFreeRate ();

    printf ("    %c%c%c%c%c%c%c %6g (%-6s %5s) | %6g | %6g | %6g | %6d\n",
            expireDate[0], expireDate[1], expireDate[2], expireDate[3], 
            expireDate[4], expireDate[5], expireDate[6],
            strikePrice,
            symbol, exchange,
            lastPrice.getValue(),
            bidPrice.getValue(),
            askPrice.getValue(),
            accVolume);
}
