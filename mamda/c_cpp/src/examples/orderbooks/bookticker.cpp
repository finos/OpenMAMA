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

#include <mamda/MamdaOrderBookFields.h>
#include <mamda/MamdaSubscription.h>
#include <mamda/MamdaCommonFields.h>
#include <mamda/MamdaOrderBookHandler.h>
#include <mamda/MamdaOrderBookListener.h>
#include <mamda/MamdaOrderBook.h>
#include <mamda/MamdaOrderBookPriceLevel.h>
#include <mamda/MamdaOrderBookEntry.h>
#include <mamda/MamdaErrorListener.h>
#include <mamda/MamdaQualityListener.h>
#include <mama/MamaQueueGroup.h>
#include <mama/MamaSource.h>
#include <mama/mamacpp.h>

#include <iostream>
#include <vector>
#include <stdexcept>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "wombat/port.h"

#include "../parsecmd.h"
#include "../dictrequester.h"

using std::endl;
using std::vector;
using std::cerr;
using std::cout;

using namespace Wombat;
    
void usage (int exitStatus);

class PrettyPrint
{
public:
    PrettyPrint () : mShowEntries (false) {}
    virtual ~PrettyPrint () {}

    void prettyPrint (const MamdaOrderBook&  book)
    {
        if (mShowEntries)
            prettyPrintEntries (book);
        else
            prettyPrintLevels (book);
    }

    void prettyPrint (const MamdaOrderBookBasicDelta&  delta)
    {
        char timeStr[32];
        MamdaOrderBookEntry* entry = delta.getEntry();
        
        MamdaOrderBookPriceLevel*        level  = delta.getPriceLevel ();
        mama_quantity_t                  size   = delta.getPlDeltaSize ();
        MamdaOrderBookPriceLevel::Action action = delta.getPlDeltaAction();

        if (MamdaOrderBookPriceLevel::MAMDA_BOOK_LEVEL_LIMIT
            == level->getOrderType())
        {            
            double price = level->getPrice ();
            printf (" %7g %7.*f  %c \n", size, mPrecision, price, action);
        }
        else
        {
            printf (" %7g MARKET %c \n",size, action);

        }
    }

    void prettyPrintLevels (const MamdaOrderBook&  book)
    {
        printf ("Book for: %s\n", book.getSymbol ());
        printf ("%s | %s\n",
                "        Time     Num    Size   Price Act",
                "Act Price   Size    Num       Time  ");
        MamdaOrderBook::constBidIterator bidIter = book.bidBegin ();
        MamdaOrderBook::constBidIterator bidEnd  = book.bidEnd ();
        MamdaOrderBook::constAskIterator askIter = book.askBegin ();
        MamdaOrderBook::constAskIterator askEnd  = book.askEnd ();
        char timeStr[32];

      
        if (mShowMarketOrders)
        {
            printf ("    MARKET ORDERS ---------------------------------------------------------------\n");
            const MamdaOrderBookPriceLevel* marketOrders = NULL;

            if (marketOrders = book.getBidMarketOrders())
            {
                marketOrders->getTime().getAsFormattedString (timeStr, 32, "%T%;");
                printf ("   %12s %4d %7g  MARKET  %c  ",
                        timeStr,
                        marketOrders->getNumEntries (),
                        marketOrders->getSize (),
                        marketOrders->getAction ());
            }
            else
            {
                printf ("                                         ");
            }
            printf ("|");
            if (marketOrders = book.getAskMarketOrders())
            {
                marketOrders->getTime().getAsFormattedString (timeStr, 32, "%T%;");
                printf ("  %c  MARKET  %-7g %-6d %-12s   ",
                        marketOrders->getAction (),
                        marketOrders->getSize (),
                        marketOrders->getNumEntries (),
                        timeStr);
            }
            else
            {
                printf ("                                         ");
            }
            printf ("\n");
            printf ("    LIMIT ORDERS  ---------------------------------------------------------------\n");
        }
      
        while ((bidIter != bidEnd) || (askIter != askEnd))
        {
            if (bidIter != bidEnd)
            {
                const MamdaOrderBookPriceLevel* bidLevel = *bidIter;
                bidLevel->getTime().getAsFormattedString (timeStr, 32, "%T%;");
                printf ("   %12s %4d %7g %7.*f  %c  ",
                        timeStr,
                        bidLevel->getNumEntries (),
                        bidLevel->getSize (),
                        mPrecision,
                        bidLevel->getPrice (),
                        bidLevel->getAction ());
                ++bidIter;
            }
            else
            {
                printf ("                                         ");
            }
            printf ("|");
            if (askIter != askEnd)
            {
                const MamdaOrderBookPriceLevel* askLevel = *askIter;
                askLevel->getTime().getAsFormattedString (timeStr, 32, "%T%;");
                printf ("  %c  %-7.*f %-7g %-6d %-12s   ",
                        askLevel->getAction (),
                        mPrecision,
                        askLevel->getPrice (),
                        askLevel->getSize (),
                        askLevel->getNumEntries (),
                        timeStr);
                ++askIter;
            }
            printf ("\n");
        }
        printf ("\n");
    }

    void prettyPrintEntries (const MamdaOrderBook&  book)
    {
        printf ("Book for: %s\n", book.getSymbol ());
        printf ("%s\n",
            "     ID/Num           Time       Size   Price");
        MamdaOrderBook::constBidIterator bidIter = book.bidBegin ();
        MamdaOrderBook::constBidIterator bidEnd  = book.bidEnd ();
        MamdaOrderBook::constAskIterator askIter = book.askBegin ();
        MamdaOrderBook::constAskIterator askEnd  = book.askEnd ();
        char timeStr[32];
        
        if (mShowMarketOrders)
        {
            const MamdaOrderBookPriceLevel* marketBidLevel =
                  book.getBidMarketOrders ();
            if (marketBidLevel)
            {
                marketBidLevel->getTime().getAsFormattedString (timeStr, 32, "%T%;");
                printf ("  Bid  %4d       %12s %7g  MARKET\n",
                        marketBidLevel->getNumEntries (),
                        timeStr,
                        marketBidLevel->getSize ());
                MamdaOrderBookPriceLevel::const_iterator end = marketBidLevel->end ();
                MamdaOrderBookPriceLevel::const_iterator i   = marketBidLevel->begin ();
                while (i != end)
                {
                    const MamdaOrderBookEntry* entry = *i;
                    const char*      id    = entry->getId ();
                    mama_quantity_t  size  = entry->getSize ();
                    entry->getTime().getAsFormattedString (timeStr, 32, "%T%;");
                    printf ("  %14s  %12s %7g  MARKET\n",
                            id, timeStr, size);
                    ++i;
                }
            }
        }

        if (mShowMarketOrders)
        {
            const MamdaOrderBookPriceLevel* marketAskLevel =
                book.getAskMarketOrders ();
            if (marketAskLevel)
            {
                marketAskLevel->getTime().getAsFormattedString (timeStr, 32, "%T%;");
                printf ("  Ask  %4d       %12s %7g  MARKET\n",
                        marketAskLevel->getNumEntries (),
                        timeStr,
                        marketAskLevel->getSize ());
                MamdaOrderBookPriceLevel::const_iterator end = marketAskLevel->end ();
                MamdaOrderBookPriceLevel::const_iterator i   = marketAskLevel->begin ();
                while (i != end)
                {
                    const MamdaOrderBookEntry* entry = *i;
                    const char*      id    = entry->getId ();
                    mama_quantity_t  size  = entry->getSize ();
                    entry->getTime().getAsFormattedString (timeStr, 32, "%T%;");
                    printf ("  %14s  %12s %7g  MARKET\n",
                            id, timeStr, size);
                    ++i;
                }   
            }
        }

        while (bidIter != bidEnd)
        {
            const MamdaOrderBookPriceLevel* bidLevel = *bidIter;
            bidLevel->getTime().getAsFormattedString (timeStr, 32, "%T%;");
            printf ("  Bid  %4d       %12s %7g %7.*f\n",
                    bidLevel->getNumEntries (),
                    timeStr,
                    bidLevel->getSize (),
                    mPrecision,
                    bidLevel->getPrice ());
            MamdaOrderBookPriceLevel::const_iterator end = bidLevel->end ();
            MamdaOrderBookPriceLevel::const_iterator i   = bidLevel->begin ();
            while (i != end)
            {
                const MamdaOrderBookEntry* entry = *i;
                const char*      id    = entry->getId ();
                mama_quantity_t  size  = entry->getSize ();
                double           price = bidLevel->getPrice ();
                entry->getTime().getAsFormattedString (timeStr, 32, "%T%;");
                printf ("  %14s  %12s %7g %7.*f\n", 
                        id, timeStr, size, mPrecision, price);
                ++i;
            }
            ++bidIter;
        }
        
        while (askIter != askEnd)
        {
            const MamdaOrderBookPriceLevel* askLevel = *askIter;
            askLevel->getTime().getAsFormattedString (timeStr, 32, "%T%;");
            printf ("  Ask  %4d       %12s %7g %7.*f\n",
                    askLevel->getNumEntries (),
                    timeStr,
                    askLevel->getSize (),
                    mPrecision,
                    askLevel->getPrice ());
            MamdaOrderBookPriceLevel::const_iterator end = askLevel->end ();
            MamdaOrderBookPriceLevel::const_iterator i   = askLevel->begin ();
            while (i != end)
            {
                const MamdaOrderBookEntry* entry = *i;
                const char*      id    = entry->getId ();
                mama_quantity_t  size  = entry->getSize ();
                double           price = askLevel->getPrice ();
                entry->getTime().getAsFormattedString (timeStr, 32, "%T%;");
                printf ("  %14s  %12s %7g %7.*f\n",
                        id, timeStr, size, mPrecision, price);
                ++i;
            }
            ++askIter;
        }
        
    }

    void setPrecision (int  precision)
    {
      mPrecision = precision;
    }
    
    void setShowEntries (bool  showEntries)
    {
        mShowEntries = showEntries;
    }

    void setShowMarketOrders (bool  showMarketOrders)
    {
        mShowMarketOrders = showMarketOrders;
    }


private:
    bool  mShowEntries;
    bool  mShowMarketOrders;
    int   mPrecision;

};

class BookTicker : public MamdaOrderBookHandler
                 , public MamdaErrorListener
                 , public MamdaQualityListener
                 , public PrettyPrint
{
public:
    BookTicker () : PrettyPrint (), mShowDeltas (false) {}
    virtual ~BookTicker () {}

    void onBookRecap (
        MamdaSubscription*                 subscription,
        MamdaOrderBookListener&            listener,
        const MamaMsg*                     msg,
        const MamdaOrderBookComplexDelta*  delta,
        const MamdaOrderBookRecap&         recap,
        const MamdaOrderBook&              book)
    {
        if (gExampleLogLevel >= EXAMPLE_LOG_LEVEL_NORMAL)
        {
            mama_seqnum_t seqNum = (msg) ? msg->getSeqNum() : 0;
            cout << "RECAP!!!  (seq# " << seqNum << ")\n";
            prettyPrint (book);
        }
    }
    
    void onBookDelta (
        MamdaSubscription*                 subscription,
        MamdaOrderBookListener&            listener,
        const MamaMsg*                     msg,
        const MamdaOrderBookSimpleDelta&   delta,
        const MamdaOrderBook&              book)
    {
        displayDelta (subscription, listener, msg, delta, book,
                      MamdaOrderBookPriceLevel::MAMDA_BOOK_LEVEL_LIMIT);

   }

    void displayDelta (
        MamdaSubscription*                  subscription,
        MamdaOrderBookListener&             listener,
        const MamaMsg*                      msg,
        const MamdaOrderBookSimpleDelta&    delta,
        const MamdaOrderBook&               book,
        MamdaOrderBookPriceLevel::OrderType orderType)
    {
        if (gExampleLogLevel >= EXAMPLE_LOG_LEVEL_NORMAL)
        {
            mama_seqnum_t seqNum = (msg) ? msg->getSeqNum() : 0;
            
            cout << (orderType ==
                     MamdaOrderBookPriceLevel::MAMDA_BOOK_LEVEL_MARKET ? "MARKET " : "")
                 << "DELTA!!!  (seq# " << seqNum << ")\n";
            
            

            if (const MamdaOrderBookEntry * const entr = delta.getEntry ())
            {
                std::cout << entr->getOrderBook()->getSymbol () 
                          << ' ' << (entr->getTime()).getAsString() 
                          << ' ' << entr->getId() 
                          << ' ' << entr->getUniqueId () 
                          << ' ' << (char)delta.getEntryDeltaAction() 
                          << ' ' << (entr->getPrice())
                          << ' ' << (char)entr->getSide() 
                          << ' ' << entr->getSize () << '\n';
            }

            if (mShowDeltas) 
            {
                prettyPrint (delta);
            }

            prettyPrint (book);
        }
        flush (cout);
    }

    void onBookComplexDelta (
        MamdaSubscription*                 subscription,
        MamdaOrderBookListener&            listener,
        const MamaMsg*                     msg,
        const MamdaOrderBookComplexDelta&  delta,
        const MamdaOrderBook&              book)
    {
        displayComplexDelta (subscription, listener, msg, delta, book,
                             MamdaOrderBookPriceLevel::MAMDA_BOOK_LEVEL_LIMIT);
    }

    void displayComplexDelta (
        MamdaSubscription*                  subscription,
        MamdaOrderBookListener&             listener,
        const MamaMsg*                      msg,
        const MamdaOrderBookComplexDelta&   delta,
        const MamdaOrderBook&               book,
        MamdaOrderBookPriceLevel::OrderType orderType)    
    {
        if (gExampleLogLevel >= EXAMPLE_LOG_LEVEL_NORMAL)
        {
            mama_seqnum_t seqNum = (msg) ? msg->getSeqNum() : 0;
           cout << (orderType ==
                    MamdaOrderBookPriceLevel::MAMDA_BOOK_LEVEL_MARKET ? "MARKET " : "")
                << "COMPLEX DELTA!!!  (seq# " << seqNum << ")\n";
            MamdaOrderBookComplexDelta::iterator end = delta.end();
            MamdaOrderBookComplexDelta::iterator i   = delta.begin();
            if (mShowDeltas)
            {
                for (; i != end; ++i)
                {
                    MamdaOrderBookBasicDelta*  basicDelta = *i;
                    prettyPrint (*basicDelta);
                }
            }
            prettyPrint (book);
        }
        flush (cout);
    }

    void onBookClear (
        MamdaSubscription*          subscription,
        MamdaOrderBookListener&     listener,
        const MamaMsg*              msg,
        const MamdaOrderBookClear&  clear,
        const MamdaOrderBook&       book)
    {
        if (gExampleLogLevel >= EXAMPLE_LOG_LEVEL_NORMAL)
        {
            mama_seqnum_t seqNum = (msg) ? msg->getSeqNum() : 0;
            cout << "CLEAR!!!  (seq# " << seqNum << ")\n";
            prettyPrint (book);
        }
        flush (cout);
    }

    void onBookGap (
        MamdaSubscription*          subscription,
        MamdaOrderBookListener&     listener,
        const MamaMsg*              msg,
        const MamdaOrderBookGap&    event,
        const MamdaOrderBook&       book)
    {
        cout << "Book gap for " << subscription->getSymbol()
             << " (" << event.getBeginGapSeqNum()
             << "-"  << event.getEndGapSeqNum() << ")\n";
        flush (cout);
    }

    void onError (
        MamdaSubscription*   subscription,
        MamdaErrorSeverity   severity,
        MamdaErrorCode       code,
        const char*          errorStr)
    {
        if (gExampleLogLevel >= EXAMPLE_LOG_LEVEL_NORMAL)
        {
            cout << "bookticker: ERROR: " << errorStr << "\n";
        }
        flush (cout);
    }

    void onQuality (
        MamdaSubscription*   subscription,
        mamaQuality          quality)
    {
        if (gExampleLogLevel >= EXAMPLE_LOG_LEVEL_NORMAL)
        {
            cout << "bookticker: QUALITY: " << quality << "\n";
        }
        flush (cout);
    }

    void setShowDeltas (bool  showDeltas)
    {
        mShowDeltas = showDeltas;
    }

private:
    bool                 mShowDeltas;
};


int main (int argc, const char **argv)
{
    try
    {
        CommonCommandLineParser  cmdLine (argc, argv);
        // Initialise the MAMA API
        mamaBridge bridge = cmdLine.getBridge();
        Mama::open ();

        const vector<const char*>&
                         symbolList          = cmdLine.getSymbolList ();
        double           throttleRate        = cmdLine.getThrottleRate ();
        int              snapshotInterval    = cmdLine.getOptInt    ("snapshot");
        int              precision           = cmdLine.getPrecision ();
        bool             processEntries      = cmdLine.getOptBool   ('e');
        bool             strictChecking      = !cmdLine.getOptBool  ('C');
        bool             processMarketOrders = cmdLine.getOptBool   ('k');
        bool             showDeltas          = cmdLine.showDeltas   ();
        MamaSource*      source              = cmdLine.getSource    ();
        MamaQueueGroup   queues (cmdLine.getNumThreads(), bridge);
        DictRequester    dictRequester (bridge);

        // We might as well enforce strict checking of order book updates
        // (at the expense of some performance).
        if (strictChecking)
        {
            cout << "MamdaOrderBook strict checking is on" << endl;
            MamdaOrderBook::setStrictChecking (true);
        }
        else
        {
            cout << "MamdaOrderBook strict checking is off" << endl;
            MamdaOrderBook::setStrictChecking (false);
        }

        // Get and initialize the dictionary
        dictRequester.requestDictionary     (cmdLine.getDictSource());
        MamdaCommonFields::setDictionary    (*dictRequester.getDictionary());
        MamdaOrderBookFields::setDictionary (*dictRequester.getDictionary ());

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
            MamdaSubscription*      aSubscription = new MamdaSubscription;
            MamdaOrderBookListener* aBookListener = new MamdaOrderBookListener;
            aSubscription->addMsgListener         (aBookListener);
            aBookListener->setProcessMarketOrders (processMarketOrders);
            aBookListener->setProcessEntries      (processEntries);

            BookTicker* aTicker = new BookTicker;

            aBookListener->addHandler         (aTicker);
            aSubscription->addQualityListener (aTicker);
            aSubscription->addErrorListener   (aTicker);

            aTicker->setShowEntries      (processEntries);
            aTicker->setShowMarketOrders (processMarketOrders);
            aTicker->setShowDeltas       (showDeltas);
            aTicker->setPrecision        (precision);
            
            aSubscription->setType (MAMA_SUBSC_TYPE_BOOK);
            aSubscription->setMdDataType (MAMA_MD_DATA_TYPE_ORDER_BOOK);
            
            aSubscription->create (queues.getNextQueue(), source, symbol);
        }

        Mama::start (bridge);
    }
    catch (MamaStatus &e)
    {  
        // This exception can be thrown from Mama.open ()
        // Mama::createTransport (transportName) and from
        // MamdaSubscription constructor when entitlements is enabled.
        cerr << "MamaStatus exception in main (): " << e.toString () << endl;
        exit (1);
    }
    catch (std::exception &ex)
    {
        cerr << "Exception in main (): " << ex.what () << endl;
        exit (1);
    }
    catch (...)
    {
        cerr << "Unknown Exception in main ()." << endl;
        exit (1);
    }

    exit (1);
}

void usage (int exitStatus)
{
    std::cerr << "Usage: bookticker [-S source] -s symbol [-s symbol ...]\n " 
              << "[-threads x] [-deltas] (For showing Deltas) [-e] (For showing Entries)\n"
              << "[-k] (process market orders) [-tport] [-dict_tport] \n";
    exit (exitStatus);
}
