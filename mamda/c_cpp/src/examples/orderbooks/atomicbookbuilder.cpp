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
#include <mamda/MamdaTradeFields.h>
#include <mamda/MamdaQuoteFields.h>
#include <mamda/MamdaCommonFields.h>
#include <mamda/MamdaOrderBookFields.h>
#include <mamda/MamdaBookAtomicBookHandler.h>
#include <mamda/MamdaBookAtomicLevelHandler.h>
#include <mamda/MamdaBookAtomicLevelEntryHandler.h>
#include <mamda/MamdaBookAtomicListener.h>
#include <mamda/MamdaErrorListener.h>
#include <mamda/MamdaQualityListener.h>
#include <mama/MamaQueueGroup.h>
#include <mama/MamaSource.h>
#include "../parsecmd.h"
#include "../dictrequester.h"
#include <iostream>
#include <vector>
#include <stdexcept>
#include <iomanip>

#include <mamda/MamdaOrderBook.h>
#include <mamda/MamdaOrderBookPriceLevel.h>

/*  *********************************************************************** */
/* This example program demonstrates the use of the MAMDA atomic book       */ 
/* listener to maintain an orderbook. Due to platform differences between   */ 
/* V5 and legacy platforms, a number of methods should not be accessed      */
/* when processing V5 entry updates using the atomic book listener          */
/* interface. This example program demonstrates how an orderbook can be     */
/* maintained by using the atomic book listener, independent of platform    */
/* (V5 or legacy) or updates being received (entry or level).               */

/* In this application, a MamdaOrderBook object is created and populated    */ 
/* as updates are received. For each update, the orderbook is printed.      */

/* The application handles the following scenarios:                         */
/*      1. V5 entry updates which have invalid level information            */
/*      2. Legacy entry updates which have fully structured book            */
/*      2. Level updates which have no entry information                    */

/* To handle all these cases, the application store processes levels and    */
/* entries. When a level callback is received, a MamdaOrderBookPriceLevel   */
/* is built but not applied to the book. If an entry callback is received   */
/* following this, the entry is applied to the book and the price level     */
/* is not applied to the book. If no entry callback is received, the        */
/* price level is applied to the book and in this manner the correct        */
/* book state is maintained.                                                */

/*  *********************************************************************** */

using  std::endl;
using  std::vector;
using  std::cerr;
using  std::cout;

using namespace Wombat;

void usage (int exitStatus);

/****************************************************************************
 * AtomicBookBuilder class implements the required base classes to receive
 * callbacks from MamdaBookAtomicListener. These are:
 *      - MamdaBookAtomicBookHandler to receive book callbacks
 *      - MamdaBookAtomicLevelHandler to recieve level callbacks
 *      - MamdaBookAtomicLevelEntryHandler to receive entry callbacks
 *
 * As per MAMDA Developers guide, when consuming V5 entry updates the 
 *  following methods should not be accessed on the MamdaBookAtomicLevel
 *  interface:
 *      - getPriceLevelSize()
 *      - getPriceLevelSizeChange()
 *      - getPriceLevelAction()
 *      - getPriceLevelNumEntries()
 *
 * As per MAMDA Developers guide, when consuming V5 entry updates the 
 *  following methods should not be accessed on the MamdaBookAtomicLevelEntry
 *  interface:
 *      - getPriceLevelSize()
 *      - getPriceLevelAction()
 *      - getPriceLevelNumEntries()
 ****************************************************************************/

class AtomicBookBuilder : public MamdaBookAtomicBookHandler
                        , public MamdaBookAtomicLevelHandler
                        , public MamdaBookAtomicLevelEntryHandler
                        , public MamdaErrorListener
                        , public MamdaQualityListener
{
private:
    MamdaOrderBook               mOrderBook;
    bool                         mShowEntries;
    bool                         mShowMarketOrders;
    MamdaOrderBookEntry*         mEntryPtr;
    MamdaOrderBookPriceLevel*    mLevelPtr;
    char*                        mSymbol;
    int                          mPrecision;

    MamdaOrderBookPriceLevel    mReusablePriceLevel;
    // No default constructor
    AtomicBookBuilder () {}

    /*
     * Helper function to print the levels in a MamdaOrderBook
     */
    void prettyPrintLevels (const MamdaOrderBook& book)
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

    /*
     * Helper function to print the entries in a MamdaOrderBook
     */
    void prettyPrintEntries (const MamdaOrderBook& book)
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

    /*
     * Helper function to print a MamdaOrderBook based on configuration
     */
    void prettyPrint (const MamdaOrderBook& book)
    {
        if (mShowEntries)
            prettyPrintEntries (book);
        else
            prettyPrintLevels (book);

        flush(cout);
    }

    /*
     * Helper function to print a MamdaOrderBookPriceLevel
     */
    void prettyPrint(
            MamdaSubscription*                  subscription,
            const MamdaOrderBookPriceLevel&     level)
    {
        // Print Entry Level Info
        const char* symbol        = subscription->getSymbol ();
        const char* time          = level.getTime().getTimeAsString();
        double      price         = level.getPrice ();
        char        side          = level.getSide();
        char        action        = level.getAction ();
        double      size          = level.getSize ();

        cout << "LEVEL "
            << symbol << " " 
            << time   << " " 
            << action << " " 
            << price  << " " 
            << side   << " "
            << size   << endl;
    }

    /*
     * Helper function to print a MamdaBookAtomicLevelEntry
     */
    void prettyPrint(
            MamdaSubscription*                  subscription,
            const MamdaBookAtomicLevelEntry&    levelEntry)
    {
        // Print Entry Level Info
        const char* symbol        = subscription->getSymbol ();
        const char* time          = levelEntry.getPriceLevelEntryTime().getTimeAsString();
        const char* entryId       = levelEntry.getPriceLevelEntryId ();
        char        entryAction   = levelEntry.getPriceLevelEntryAction ();
        double      price         = levelEntry.getPriceLevelPrice ();
        char        entrySide     = levelEntry.getPriceLevelSide();
        double      size          = levelEntry.getPriceLevelEntrySize ();

        cout << "ENTRY "
            << symbol      << " " 
            << time        << " " 
            << entryId     << " " 
            << entryAction << " " 
            << price       << " " 
            << entrySide   << " "
            << size        << endl;
    }
    
    /**
     * Helper function to apply a MamdaOrderBookPriceLevel to 
     * a MamdaOrderBook
     */
    void applyLevel (MamdaOrderBookPriceLevel& level)
    {
        if(mOrderBook.getQuality() == MAMA_QUALITY_OK)
        {
            switch(level.getAction())
            {
                case MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_UPDATE :
                    try
                    {
                        /*
                         * When in the order book the only Action which makes sense is
                         * ADD
                         */
                        level.setAction(MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD);

                        mOrderBook.updateLevel(level);
                    }
                    catch ( MamdaOrderBookException &e)
                    {
                        //Exception is thrown when updating level which does not exist
                        //level will be added so handled internally
                    }
                    break;
                case MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD :
                    try
                    {
                        mLevelPtr = new MamdaOrderBookPriceLevel(level);
                        /*
                         * When in the order book the only Action which makes sense is
                         * ADD
                         */
                        mLevelPtr->setAction(MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD);

                        mOrderBook.addLevel(*mLevelPtr);
                        //ownership of ptr passed to MamdaOrderBook
                    }
                    catch ( MamdaOrderBookException &e)
                    {
                        //Exception is thrown if adding a level already in book
                        //handled internally by updating level
                        delete mLevelPtr;
                    }
                    break;
                case MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_DELETE :
                    try
                    {
                        mOrderBook.deleteLevel(mReusablePriceLevel);
                    }
                    catch (MamdaOrderBookException &e)
                    {
                        //Thrown if the level cannot be found in the book
                        //No need for handling as level is deleted
                    }
                    break;
                default:
                    cout << "atomicbookbuilder: Unknown price level [" 
                        << level.getAction() << "]\n";
                    break;
            }

            mLevelPtr = NULL;
        }
    }

    /**
     * Helper function to apply a MamdaBookAtomicLevelEntry to 
     * a MamdaOrderBook
     */
    void applyEntry (const MamdaBookAtomicLevelEntry& levelEntry)
    {
        MamdaOrderBookEntry::Action entryAction;

        if(mOrderBook.getQuality() == MAMA_QUALITY_OK)
        {
            entryAction = (MamdaOrderBookEntry::Action) levelEntry.getPriceLevelEntryAction();

            switch(entryAction)
            {
                case MamdaOrderBookEntry::MAMDA_BOOK_ACTION_UPDATE :
                    try
                    {
                        //get the price level by price
                        mLevelPtr = mOrderBook.findLevel(
                                levelEntry.getPriceLevelPrice(),
                                (MamdaOrderBookPriceLevel::Side) levelEntry.getPriceLevelSide());
                        if(mLevelPtr != NULL)
                        {
                            //get the entry by id
                            mEntryPtr = mLevelPtr->findEntry(levelEntry.getPriceLevelEntryId());

                            if(mEntryPtr != NULL)
                            {
                                mOrderBook.updateEntry(
                                        mEntryPtr,
                                        levelEntry.getPriceLevelEntrySize(),
                                        levelEntry.getPriceLevelEntryTime(),
                                        (MamdaOrderBookBasicDelta*) NULL);
                                break;
                            }
                        }
                        /*
                         * intentional fall through to add the entry if 
                         * the entry or level cannot be found to update it
                         */
                    }
                    catch ( MamdaOrderBookInvalidEntry &e)
                    {
                        cout<< "atomicbookbuilder: could not update entry.\n";
                        cout << "Caught MamdaOrderBookInvalidEntry [" << e.what() << "]\n";
                    }
                    catch (MamdaOrderBookException &e)
                    {
                        cout<< "atomicbookbuilder: could not update entry.\n";
                        cout<< "Caught MamdaOrderBookException [" << e.what() << "[\n";
                    }
                case MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD :
                    mEntryPtr = mOrderBook.addEntry(
                            levelEntry.getPriceLevelEntryId(),
                            levelEntry.getPriceLevelEntrySize(),
                            levelEntry.getPriceLevelPrice(),
                            (MamdaOrderBookPriceLevel::Side) levelEntry.getPriceLevelSide(),
                            levelEntry.getPriceLevelEntryTime(),
                            (const MamaSourceDerivative*) NULL,
                            (MamdaOrderBookBasicDelta*) NULL);

                    mEntryPtr->setReason(
                            (MamdaOrderBookTypes::Reason)levelEntry.getPriceLevelEntryReason());
                    break;
                case MamdaOrderBookEntry::MAMDA_BOOK_ACTION_DELETE :
                    try
                    {
                        //get the price level by price
                        mLevelPtr = mOrderBook.findLevel(
                                levelEntry.getPriceLevelPrice(),
                                (MamdaOrderBookPriceLevel::Side) levelEntry.getPriceLevelSide());
                        if(mLevelPtr != NULL)
                        {
                            //get the entry by id
                            mEntryPtr = mLevelPtr->findEntry(levelEntry.getPriceLevelEntryId());

                            if(mEntryPtr != NULL)
                                mOrderBook.deleteEntry(
                                        mEntryPtr,
                                        levelEntry.getPriceLevelEntryTime(),
                                        (MamdaOrderBookBasicDelta*) NULL);
                        }
                    }
                    catch ( MamdaOrderBookInvalidEntry &e)
                    {
                        cout<< "atomicbookbuilder: could not delete entry.\n";
                        cout << "Caught MamdaOrderBookInvalidEntry [" << e.what() << "]\n";
                    }
                    catch (MamdaOrderBookException &e)
                    {
                        cout<< "atomicbookbuilder: could not delete entry.\n";
                        cout<< "Caught MamdaOrderBookException [" << e.what() << "[\n";
                    }
                    break;
                default:
                    cout << "atomicbookbuilder: Unknown entry action ["
                        << (char)entryAction << "]\n";
                    break;
            }

            mEntryPtr = NULL;
            mLevelPtr = NULL;
        }
    }

    /**
     * Helper function to store a MamdaBookAtomicLevel in 
     *  the resuabale MamdaOrderBookPriceLevel
     */
    void storeLevel(const MamdaBookAtomicLevel&  level)
    {
        mReusablePriceLevel.clear();
        mReusablePriceLevel.setPrice (level.getPriceLevelPrice());
        mReusablePriceLevel.setSide ((MamdaOrderBookPriceLevel::Side)level.getPriceLevelSide());
        mReusablePriceLevel.setTime (level.getPriceLevelTime());

        /**
         * As per the MAMDA Developers Guide, the following three accessors on a MamdaBookAtomicLevel
         *  object should not be used for V5 entry updates. Here, these calls are used and
         *  the resulting MamdaOrderBookPriceLevel is only used when the callbacks received
         *  indicate that the update was not a V5 entry update.
         */
        mReusablePriceLevel.setSize (level.getPriceLevelSize());
        mReusablePriceLevel.setAction ((MamdaOrderBookPriceLevel::Action) level.getPriceLevelAction());
        mReusablePriceLevel.setNumEntries ((mama_u32_t)level.getPriceLevelNumEntries());
    }

public:
    AtomicBookBuilder (
            const char* symbol) 
    : mSymbol           (strdup(symbol))
    , mEntryPtr         (NULL)
    , mLevelPtr         (NULL)
    , mShowEntries      (false)
    , mShowMarketOrders (false)
    {
        mOrderBook.setSymbol  (mSymbol);
        mOrderBook.setQuality (MAMA_QUALITY_OK);
    }

    virtual ~AtomicBookBuilder () 
    { 
        free(mSymbol);
    }

    void setPrecision (int precision)
    {
        mPrecision = precision;
    }

    void setShowMarketOrders (bool showMarketOrders)
    {
        mShowMarketOrders = showMarketOrders;
    }

    void setShowEntries (bool showEntries)
    {
        mShowEntries = showEntries;
    }

    /**
     * Method invoked before we start processing the first level in a message.
     * The book should be cleared when isRecap == true.
     */
    void onBookAtomicBeginBook (
        MamdaSubscription*           subscription,
        MamdaBookAtomicListener&     listener,
        bool                         isRecap) 

    {
        cout<< "BOOK BEGIN\n";
        if(isRecap)
            mOrderBook.clear();

        if(mOrderBook.getQuality() == MAMA_QUALITY_OK)
        {
            if(isRecap)
            {
                cout << "RECAP!!!\n";
            }
            else
            {
                cout << "DELTA!!!\n";
            }
        }
    }

    /**
     * Method invoked when an order book is cleared.
     */
    void onBookAtomicClear (
        MamdaSubscription*          subscription,
        MamdaBookAtomicListener&    listener,
        const MamaMsg&              msg)
    {
        mOrderBook.clear();
    }

    /**
     * Method invoked when a gap in orderBook reports is discovered.
     */
    void onBookAtomicGap (
        MamdaSubscription*                subscription,
        MamdaBookAtomicListener&          listener,
        const MamaMsg&                    msg,
        const MamdaBookAtomicGap&         event)
    {
        if (gExampleLogLevel == EXAMPLE_LOG_LEVEL_NORMAL)
        {
            cout << "Book gap for " <<  subscription->getSymbol ();
            cout << " (" << listener.getBeginGapSeqNum ();
            cout << "-" << listener.getEndGapSeqNum () << ")\n";
        }
    }

    /**
     * Method invoked when we stop processing the last level in a message. We 
     * invoke this method after the last entry for the level gets processed. 
     * The subscription may be destroyed from this callback.
     */
    void onBookAtomicEndBook (
        MamdaSubscription*           subscription,
        MamdaBookAtomicListener&     listener) 
    {
        /**
         * When level recap/delta is received, it is stored in a MamdaOrderBookPriceLevel.
         * If no entry deltas/recaps are received, then the price level should be applied
         *  to the book. 
         * The entry delta/recap callback will mark the price level with an UNKNOWN side to
         *  show that it does not need to be applied to the book
         */
        if(mReusablePriceLevel.getSide() != MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_UNKNOWN)
        {
            if (gExampleLogLevel == EXAMPLE_LOG_LEVEL_NORMAL 
                    && mOrderBook.getQuality() == MAMA_QUALITY_OK)
            {
                prettyPrint(subscription, mReusablePriceLevel);
            }
            applyLevel(mReusablePriceLevel);
            mReusablePriceLevel.setSide(MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_UNKNOWN);
        }

        if(mOrderBook.getQuality() == MAMA_QUALITY_OK && gExampleLogLevel == EXAMPLE_LOG_LEVEL_NORMAL)
        {
            prettyPrint(mOrderBook);
        }
        cout<< "BOOK END\n";
    }

    /**
     * Method invoked when a full refresh of the order book for the
     * security is available.  The reason for the invocation may be
     * any of the following:
     * - Initial image.
     * - Recap update (e.g., after server fault tolerant event or data
     *   quality event.)
     * - After stale status removed.
     */
    void onBookAtomicLevelRecap (
        MamdaSubscription*           subscription,
        MamdaBookAtomicListener&     listener,
        const MamaMsg&               msg,
        const MamdaBookAtomicLevel&  level)
    {
        /**
         * The level should only be processed when entires are not received
         *  i.e. for level only based feeds
         * If an entry was received on the previous level, then the level will
         *  have been marked with an UNKNOWN side and should not be applied to 
         *  the book
         */
        if(mReusablePriceLevel.getSide() != MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_UNKNOWN)
        {
            applyLevel (mReusablePriceLevel);
        }

        /**
         * Store the current level
         */
        storeLevel(level);
    }


    /**
     * Method invoked when an order book delta is reported.
     */
    void onBookAtomicLevelDelta (
        MamdaSubscription*           subscription,
        MamdaBookAtomicListener&     listener,
        const MamaMsg&               msg,
        const MamdaBookAtomicLevel&  level)
    {
        /**
         * The level should only be processed when entires are not received
         *  i.e. for level only based feeds
         * If an entry was received on the previous level, then the level will
         *  have been marked with an UNKNOWN side and should not be applied to 
         *  the book
         */
        if(mReusablePriceLevel.getSide() != MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_UNKNOWN)
        {
            if (gExampleLogLevel == EXAMPLE_LOG_LEVEL_NORMAL 
                    && mOrderBook.getQuality() == MAMA_QUALITY_OK)
            {
                prettyPrint(subscription, mReusablePriceLevel);
            }
            applyLevel(mReusablePriceLevel);
        }

        /**
         * Store the current level
         */
        storeLevel(level);
    }


    /**
     * Method invoked when a full refresh of the order book for the
     * security is available.  The reason for the invocation may be
     * any of the following:
     * - Initial image.
     * - Recap update (e.g., after server fault tolerant event or data
     *   quality event.)
     * - After stale status removed.
     */ 
    void onBookAtomicLevelEntryRecap (
        MamdaSubscription*                  subscription,
        MamdaBookAtomicListener&            listener,
        const MamaMsg&                      msg,
        const MamdaBookAtomicLevelEntry&    levelEntry)
    {
        applyEntry(levelEntry);

        /**
         * An entry has been processed on the level so mark the level with
         *  an unknown side so that it will not be applied to book
         */
        mReusablePriceLevel.setSide (MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_UNKNOWN);
    }

    /**
     * Method invoked when an order book delta is reported.
     */
    void onBookAtomicLevelEntryDelta (
        MamdaSubscription*                  subscription,
        MamdaBookAtomicListener&            listener,
        const MamaMsg&                      msg,
        const MamdaBookAtomicLevelEntry&    levelEntry)
    {
        if (gExampleLogLevel == EXAMPLE_LOG_LEVEL_NORMAL
                && mOrderBook.getQuality() == MAMA_QUALITY_OK)
        {
            prettyPrint (subscription, levelEntry);
        }
        applyEntry (levelEntry);

        /**
         * An entry has been processed on the level so mark the level with
         *  an unknown side so that it will not be applied to book
         */
        mReusablePriceLevel.setSide (MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_UNKNOWN);
    }

    void onError ( 
        MamdaSubscription*  subscription,
        MamdaErrorSeverity  severity,
        MamdaErrorCode      code,
        const char *        errorStr)
    {
        if (gExampleLogLevel == EXAMPLE_LOG_LEVEL_NORMAL)
        {
            cout << "atomicbookbuilder: ERROR: " << errorStr << "\n";
        }
    }

    void onQuality (MamdaSubscription* subscription, mamaQuality quality)
    {
        if (gExampleLogLevel == EXAMPLE_LOG_LEVEL_NORMAL)
        {
            cout << "atomicbookbuilder: QUALITY: " << quality << "\n";
        }

        mOrderBook.setQuality (quality);
    }
    
};

int main (int argc, const char** argv)
{
    MamaQueueGroup*             queues = NULL;
    mamaBridge                  bridge = NULL;
    try
    {
        // Initialize the MAMA API
        CommonCommandLineParser  cmdLine (argc, argv);
        bridge = cmdLine.getBridge();
        Mama::open ();
        DictRequester               dictRequester (bridge);
        const vector<const char*>&  symbolList    = cmdLine.getSymbolList ();
        int                         threads       = cmdLine.getNumThreads ();
        MamaSource*                 source        = cmdLine.getSource();
        bool             			processMarketOrders = cmdLine.getOptBool ('k');

        queues = new MamaQueueGroup (threads, bridge);

        dictRequester.requestDictionary     (cmdLine.getDictSource());
        MamdaCommonFields::setDictionary    (*dictRequester.getDictionary ());
        MamdaTradeFields::setDictionary     (*dictRequester.getDictionary ());
        MamdaQuoteFields::setDictionary     (*dictRequester.getDictionary ());
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

        bool showEntries      = cmdLine.getOptBool ('e');
        bool showMarketOrders = cmdLine.getOptBool ('k');
        int  precision = cmdLine.getPrecision();


        for (vector < const char*>::const_iterator i = symbolList.begin ();
            i != symbolList.end (); ++i)
        {
            const char* symbol =*i;
            MamdaSubscription*       aSubscription = new MamdaSubscription;
            MamdaBookAtomicListener* aBookListener = new MamdaBookAtomicListener;
            AtomicBookBuilder*       aBuilder       = new AtomicBookBuilder(symbol);

            /*
             * Add the book handler to recieve book begin and end callbacks
             */
            aBookListener->addBookHandler (aBuilder);

           
            /*
             * Add the level handler to handle level updates where available
             */
            aBookListener->addLevelHandler (aBuilder);

            /*
             * Add the entry handler to handle entry updates where available
             */
            aBookListener->addLevelEntryHandler (aBuilder);

            aBuilder->setShowEntries      (showEntries);
            aBuilder->setShowMarketOrders (showMarketOrders);
            aBuilder->setPrecision        (precision);

            aSubscription->addMsgListener     (aBookListener);
            aSubscription->addQualityListener (aBuilder);
            aSubscription->addErrorListener   (aBuilder);
            aSubscription->setType            (MAMA_SUBSC_TYPE_BOOK);
            aSubscription->setMdDataType      (MAMA_MD_DATA_TYPE_ORDER_BOOK);
            aSubscription->create             (queues->getNextQueue(), source, symbol);
            aBookListener->setProcessMarketOrders (processMarketOrders);
        }
        
        // Dispatch on the default MAMA queue
        Mama::start(bridge);
    }
    catch (MamaStatus& e)
    {
        cerr << "Exception in main (): " << e.toString () << endl;
        exit(EXIT_FAILURE);
    }
    catch (std::exception& ex)
    {
        cerr << "Exception in main (): " << ex.what () << endl;
        exit(EXIT_FAILURE);
    }
    catch (...)
    {
        cerr << "Unknown Exception in main ()." << endl;
        exit(EXIT_FAILURE);
    }
    
    return 0;
}

void usage (int exitStatus)
{
    std::cerr << "Usage: atomicbookticker ";
    std::cerr << "[-S source] [-m middleware] [-tport transport] -s symbol [-s symbol ...] ";
    std::cerr << "[-threads x]\n";
    exit (exitStatus);
}
