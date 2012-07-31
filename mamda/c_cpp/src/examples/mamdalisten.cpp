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
#include <mamda/MamdaMsgListener.h>
#include <mamda/MamdaErrorListener.h>
#include <mamda/MamdaQualityListener.h>
#include <iostream>
#include <stdexcept>
#include <vector>
#include "parsecmd.h"
#include "mama/MamaQueueGroup.h"
#include "dictrequester.h"

using std::exception;
using std::endl;
using std::vector;
using std::cerr;
using std::cout;

using namespace Wombat;

void usage (int exitStatus);

class ListenerCallback : public MamdaMsgListener
                       , public MamdaErrorListener
                       , public MamdaQualityListener
                       , public MamaMsgFieldIterator
{
public:
    ListenerCallback ():
        mDictionary (NULL) {}
    
    ~ListenerCallback () {}

    void onMsg (
        MamdaSubscription*   subscription,
        const MamaMsg&       msg,
        short                msgType);

    void onError (
        MamdaSubscription*   subscription,
        MamdaErrorSeverity   severity,
        MamdaErrorCode       errorCode,
        const char*          errorStr);

    void onQuality (
        MamdaSubscription*   subscription,
        mamaQuality          quality);

    void onField (
        const MamaMsg&             msg,
        const MamaMsgField& field,
        void*               closure);

    void setDictionary (
        const MamaDictionary* dictionary);

private:
    const MamaDictionary* mDictionary;
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
        ListenerCallback ticker;
        DictRequester    dictRequester (bridge);
        dictRequester.requestDictionary (cmdLine.getDictSource());
        ticker.setDictionary (dictRequester.getDictionary ());
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
            aSubscription->addMsgListener     (&ticker);
            aSubscription->addQualityListener (&ticker);
            aSubscription->addErrorListener   (&ticker);
            if (cmdLine.getSnapshot())
                aSubscription->setServiceLevel (MAMA_SERVICE_LEVEL_SNAPSHOT);
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
    std::cerr << "Usage: mamdalisten [-tport] tport_name [-m] middleware [-S] source [-s] symbol [options] \n";        
    std::cerr << "Options:" << std::endl;
    std::cerr << "  -1   Create snapshot subscriptions" << std::endl;

    exit (exitStatus);
}

void ListenerCallback::onMsg (
    MamdaSubscription*   subscription,
    const MamaMsg&       msg,
    short                msgType)
{
    cout << "Update (" << subscription->getSymbol () <<  "): \n";
    msg.iterateFields (*this, mDictionary, NULL);
}

void ListenerCallback::onError (
    MamdaSubscription*   subscription,
    MamdaErrorSeverity   severity,
    MamdaErrorCode       errorCode,
    const char*          errorStr)
{
    cout << "Error (" << subscription->getSymbol () << ")" << endl;
}

void ListenerCallback::onQuality (
    MamdaSubscription*   subscription,
    mamaQuality          quality)
{
    cout << "Quality (" << subscription->getSymbol () << "): " << quality << endl;
}

void ListenerCallback::onField (
    const MamaMsg&       msg,
    const MamaMsgField&  field,
    void*                closure)
{
    char  fieldValueStr[256];
    const MamaFieldDescriptor* desc = field.getDescriptor ();
    if (!desc) return;
    msg.getFieldAsString (desc, fieldValueStr,256);
    printf ("%20s | %3d | ", desc->getName (),     desc->getFid ());
    printf ("%20s | %s\n",   desc->getTypeName (), fieldValueStr);
}

void ListenerCallback::setDictionary (
        const MamaDictionary* dictionary)
{
    mDictionary = dictionary;
}
