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

#include <mamda/MamdaNewsManager.h>
#include <mamda/MamdaSubscription.h>
#include <mamda/MamdaCommonFields.h>
#include <mamda/MamdaNewsFields.h>
#include <mamda/MamdaErrorListener.h>
#include <mamda/MamdaQualityListener.h>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <mama/MamaQueueGroup.h>
#include <mama/MamaSource.h>
#include "../parsecmd.h"
#include "../dictrequester.h"

using std::vector;
using std::endl;
using std::cerr;
using std::cout;

using namespace Wombat;

void usage (int exitStatus);

/**
 * NewsTicker is an example of a user class that listens to news
 * headlines and occasionally requests the associated story.
 */

class NewsTicker : public MamdaNewsHeadlineHandler
                 , public MamdaNewsStoryHandler
                 , public MamdaErrorListener
                 , public MamdaQualityListener
{
public:
    NewsTicker() : mCounter(0) {}
    virtual ~NewsTicker() {}

    void onNewsHeadline (
        MamdaNewsManager&         manager,
        const MamaMsg&            msg,
        const MamdaNewsHeadline&  headline,
        void*                     closure)
    {
        cout << "NewsTicker: received headline"
             << "\n  Headline text:              "
             << headline.getHeadlineText()
             << "\n  Headline ID:                "
             << headline.getHeadlineId()
             << "\n  Priority:                   "
             << headline.getNewsPriority()
             << "\n  Story ID:                   "
             << headline.getNewsStoryId()
             << "\n  Original story ID:          "
             << headline.getNewsOrigStoryId()
             << "\n  Source:                     "
             << headline.getNewsSourceId()
             << "\n  Original Source:            "
             << headline.getNewsOrigSourceId()
             << "\n  Has story:                  "
             << headline.getNewsHasStory()
             << "\n  Revision number:            "
             << headline.getNewsStoryRevNumber()
             << "\n  Language:                   "
             << headline.getLanguageId()
             << "\n  Sequence number:            "
             << headline.getEventSeqNum()
             << "\n  News time:                  "
             << headline.getEventTime().getAsString()
             << "\n  Orig time:                  "
             << headline.getNewsOrigStoryTime().getAsString()
             << "\n  Source time:                "
             << headline.getSrcTime().getAsString()
             << "\n  Activity time:              "
             << headline.getActivityTime().getAsString();

        const char** strVals  = NULL;
        mama_size_t  i;
        mama_size_t  strCount = 0;
        headline.getIndustries (strVals, strCount);
        cout << "\n  Industry codes (" << strCount << "):         ";
        for (i = 0; i < strCount; i++)
        {
            cout << strVals[i] << " ";
        }

        headline.getMarketSectors (strVals, strCount);
        cout << "\n  Market sector codes (" << strCount << "):    ";
        for (i = 0; i < strCount; i++)
        {
            cout << strVals[i] << " ";
        }

        headline.getRegions (strVals, strCount);
        cout << "\n  Regions (" << strCount << "):                ";
        for (i = 0; i < strCount; i++)
        {
            cout << strVals[i] << " ";
        }

        headline.getCountries (strVals, strCount);
        cout << "\n  Country codes (" << strCount << "):          ";
        for (i = 0; i < strCount; i++)
        {
            cout << strVals[i] << " ";
        }

        headline.getProducts (strVals, strCount);
        cout << "\n  Product codes (" << strCount << "):          ";
        for (i = 0; i < strCount; i++)
        {
            cout << strVals[i] << " ";
        }

        headline.getTopics (strVals, strCount);
        cout << "\n  Topic codes (" << strCount << "):            ";
        for (i = 0; i < strCount; i++)
        {
            cout << strVals[i] << " ";
        }

        headline.getMiscCodes (strVals, strCount);
        cout << "\n  Miscellaneous codes (" << strCount << "):    ";
        for (i = 0; i < strCount; i++)
        {
            cout << strVals[i] << " ";
        }

        headline.getNativeCodes (strVals, strCount);
        cout << "\n  Native codes (" << strCount << "):           ";
        for (i = 0; i < strCount; i++)
        {
            cout << strVals[i] << " ";
        }

        headline.getNativeRelatedSymbols  (strVals, strCount);
        cout << "\n  Native related symbols (" << strCount << "): ";
        for (i = 0; i < strCount; i++)
        {
            cout << strVals[i] << " ";
        }
        cout << "\n";

        if ((mCounter++ % 5) == 0)
        {
            cout << "NewsTicker: requesting storyId "
                 << headline.getNewsStoryId() << "\n";
            manager.requestStory (headline, this, NULL);
        }
    }

    void onNewsStory (
        MamdaNewsManager&         manager,
        const MamaMsg&            msg,
        const MamdaNewsStory&     story,
        void*                     closure)
    {
        cout << "NewsTicker: got story ID " << story.getNewsStoryId()
             << " (status=" << MamdaNewsStory::toString(story.getStatus())
             << "):";

        const MamdaNewsHeadlineId* strVals  = NULL;
        mama_size_t  strCount = 0;
        story.getHeadlines (strVals, strCount);
        cout << "\n  Headline IDs:  ";
        for (mama_size_t i = 0; i < strCount; i++)
        {
            cout << strVals[i] << " ";
        }
        cout << "\n  Revision number:  " << story.getNewsStoryRevNumber();
        cout << "\n";

        switch (story.getStatus())
        {
            case MamdaNewsStory::FULL_STORY:
                cout << "   Story: " << story.getNewsStoryText() << "\n\n";
                break;
            case MamdaNewsStory::DELAYED_STORY:
                cout << "   Requesting full story again in 1 second\n";
                manager.requestStoryLater (story, this, 1.0, NULL);
                break;
            default:
                ;
        }
    }

    void onNewsStoryError (
        MamdaNewsManager&         manager,
        const MamaMsg&            msg,
        MamdaNewsStoryId          storyId,
        void*                     closure)
    {
        cout << "NewsTicker: could not fetch story ID " << storyId
             << ": reason???\n";
    }

    void onError (
        MamdaSubscription*   subscription,
        MamdaErrorSeverity   severity,
        MamdaErrorCode       code,
        const char*          errorStr)
    {
        cout << "NewsTicker:: OnError () sub =  " \
			 <<  subscription->getSymbol() << " errorStr = \"" 
             << errorStr << "\"\n";
    }

    void onQuality (
        MamdaSubscription*   subscription,
        mamaQuality          quality)
    {
    }

private:
    int mCounter;
};

int main (int argc, const char **argv)
{
    try
    {
        MamaQueueGroup*     queues         = NULL;
        MamdaNewsManager*   aNewsManager   = new MamdaNewsManager;
        NewsTicker*         aTicker        = new NewsTicker;
        mamaBridge          bridge         = NULL;

        CommonCommandLineParser     cmdLine (argc, argv);
        
        bridge = cmdLine.getBridge();
        Mama::open();
        
        const vector<const char*>&  symbolList    = cmdLine.getSymbolList();
        int                         threads       = cmdLine.getNumThreads();
        MamaSource*                 source        = cmdLine.getSource();

        queues = new MamaQueueGroup(threads, bridge);
        DictRequester    dictRequester (bridge);
        dictRequester.requestDictionary (cmdLine.getDictSource());
        MamdaCommonFields::setDictionary (*dictRequester.getDictionary());
        MamdaNewsFields::setDictionary (*dictRequester.getDictionary());
        
        aNewsManager->addBroadcastHeadlineHandler (aTicker);
        aNewsManager->addBroadcastStoryHandler (aTicker);
        aNewsManager->addQualityHandler (aTicker);
        aNewsManager->addErrorHandler (aTicker);
 
        for (vector<const char*>::const_iterator i = symbolList.begin();
             i != symbolList.end();
             ++i)
        {
            const char* symbol = *i;
            aNewsManager->addBroadcastHeadlineSource (
                queues->getNextQueue(), source, symbol, NULL);
        }

        aNewsManager->addBroadcastStorySource (
        queues->getNextQueue(), source, "STORIES", NULL);

		Mama::start (bridge);
        delete queues;
    }
    catch (MamaStatus& e)
    {
        /* This exception can be thrown from Mama::open(),
         * MamaTransport::create(transportName) and from
         * MamdaSubscription constructor when entitlements is
         * enabled. */
        cerr << "Exception in main(): " << e.toString() << endl;
        exit (1);
    }
    catch (std::exception& e)
    {
       cerr << "Exception in main(): " << e.what() << endl;
       exit (1);
    }
    catch (...)
    {
        cerr << "Unknown Exception in main()." << endl;
        exit (1);
    }
}


void usage (int exitStatus)
{
    std::cerr << "Usage: newsticker -m middleware [-S source] -s symbol ";
                 "[-s symbol ...] \n";
    exit(exitStatus);
}
