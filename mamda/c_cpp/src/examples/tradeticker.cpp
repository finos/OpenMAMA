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
#include <mamda/MamdaTradeFields.h>
#include <mamda/MamdaTradeHandler.h>
#include <mamda/MamdaTradeListener.h>
#include <mamda/MamdaErrorListener.h>
#include <mamda/MamdaQualityListener.h>
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

class TradeTicker : public MamdaTradeHandler
                  , public MamdaErrorListener
                  , public MamdaQualityListener
{
public:
    virtual ~TradeTicker () {}

    void onTradeRecap (
        MamdaSubscription*      subscription,
        MamdaTradeListener&     listener,
        const MamaMsg&          msg,
        const MamdaTradeRecap&  recap)
    {
        cout << "Trade Recap (" 
             << subscription->getSymbol () 
             << ") \n"
             << flush;            
    }

    void onTradeReport (
        MamdaSubscription*      subscription,
        MamdaTradeListener&     listener,
        const MamaMsg&          msg,
        const MamdaTradeReport& event,
        const MamdaTradeRecap&  recap)
    {
        cout << "Trade ("     << subscription->getSymbol ()
             << ":"           << recap.getTradeCount ()
             << "):  "        << event.getTradeVolume ()
             << " @ "         << event.getTradePrice().getAsString()
             << " (seq#: "    << event.getEventSeqNum ()
             << "; time: "    << event.getEventTime().getAsString()
             << "; acttime: " << event.getActivityTime().getAsString()
             << "; partId: "  << event.getTradePartId ()
             << "; qual: "    << event.getTradeQual ()
             << "; cond: "    << event.getTradeQualNative ()
             << "; irreg: "   << event.getIsIrregular () << ")\n"
             << flush;
    }

    void onTradeGap (
        MamdaSubscription*              subscription,
        MamdaTradeListener&             listener,
        const MamaMsg&                  msg,
        const MamdaTradeGap&            event,
        const MamdaTradeRecap&          recap)
    {
        cout << "Trade Gap (" 
             << subscription->getSymbol()
             << ") \n"
             << flush;        
    }

    void onTradeCancelOrError (
        MamdaSubscription*              subscription,
        MamdaTradeListener&             listener,
        const MamaMsg&                  msg,
        const MamdaTradeCancelOrError&  event,
        const MamdaTradeRecap&          recap)
    {        
        cout << "Trade Cancel (" 
             << subscription->getSymbol()
             << ") \n"
             << flush;
    }

    void onTradeCorrection (
        MamdaSubscription*              subscription,
        MamdaTradeListener&             listener,
        const MamaMsg&                  msg,
        const MamdaTradeCorrection&     event,
        const MamdaTradeRecap&          recap)
    {        
        cout << "Trade Correction (" 
             << subscription->getSymbol()
             << ") \n"
             << flush;
    }

    void onTradeClosing (
        MamdaSubscription*              subscription,
        MamdaTradeListener&             listener,
        const MamaMsg&                  msg,
        const MamdaTradeClosing&        event,
        const MamdaTradeRecap&          recap)
    {        
        cout << "Trade Closing ("
             << subscription->getSymbol()
             << ") \n"
             << flush;
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
             << ")\n"
             << flush;
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
             << ")\n"
             << flush;
    }

    void onError (
        MamdaSubscription*   subscription,
        MamdaErrorSeverity   severity,
        MamdaErrorCode       errorCode,
        const char*          errorStr)
    {        
        cout << "onError: " 
             << subscription->getSymbol()
             << " ("
             << errorStr
             << ") \n"
             << flush;        
    }

    void onQuality (
        MamdaSubscription*   subscription,
        mamaQuality          quality)
    {        
        cout << "onQuality: " 
             << subscription->getSymbol()
             << " ("
             << mamaQuality_convertToString (quality)
             << ") \n"
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
        MamdaTradeFields::setDictionary (*dictRequester.getDictionary());

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
            MamdaTradeListener* aTradeListener = new MamdaTradeListener;
            TradeTicker*        aTicker        = new TradeTicker;

            aTradeListener->addHandler (aTicker);
            aSubscription->addMsgListener (aTradeListener);
            aSubscription->addQualityListener (aTicker);
            aSubscription->addErrorListener (aTicker);
            aSubscription->create (queues.getNextQueue(), source, symbol);
        }

        Mama::start(bridge);
    }
    catch (MamaStatus &e)
    {
        // This exception can be thrown from Mama::open ()
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
    std::cerr << "Usage: tradeticker [-tport] tport_name [-m] middleware [-S] source [-s] symbol";
    exit (exitStatus);
}
