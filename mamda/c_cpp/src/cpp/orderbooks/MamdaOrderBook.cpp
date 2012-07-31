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

#include <iostream>
#include <cstdio>
#include <map>
#include <list>
#include <string>
#include <iterator>
#include <stdio.h>
#include <assert.h>
#include <float.h>
#include <math.h>

#include <mamda/MamdaOrderBook.h>
#include <mamda/MamdaOrderBookEntry.h>
#include <mamda/MamdaOrderBookBasicDelta.h>
#include <mamda/MamdaOrderBookBasicDeltaList.h>
#include <mamda/MamdaOrderBookSimpleDelta.h>
#include <mamda/MamdaOrderBookComplexDelta.h>
#include <mamda/MamdaOrderBookConcreteSimpleDelta.h>
#include <mamda/MamdaOrderBookConcreteComplexDelta.h>
#include "MamdaOrderBookWriter.h"
#include <wombat/wtable.h>

using std::map;
using std::list;
using std::string;

static bool sStrictChecking = false;

namespace Wombat
{
    typedef map<double,MamdaOrderBookPriceLevel*> PlMap;
    typedef list<MamdaOrderBookPriceLevel*>       PlList;
    typedef list<MamdaOrderBookEntry*>            EntryList;

    struct MamdaOrderBook::MamdaOrderBookImpl
    {
        MamdaOrderBookImpl (MamdaOrderBook&  book);
        ~MamdaOrderBookImpl ();

        //! \throw<MamdaOrderBookException>
        MamdaOrderBookPriceLevel* createLevel ();

        //! \throw<MamdaOrderBookException>
        void addLevel        (const MamdaOrderBookPriceLevel&  level);

        //! \throw<MamdaOrderBookException>
        void updateLevel        (const MamdaOrderBookPriceLevel&  level);
        
        //! \throw<MamdaOrderBookException>
        void deleteLevel     (const MamdaOrderBookPriceLevel&  level);

        //! \throw<MamdaOrderBookException>
        void applySide       (PlMap&                           bookSide,
                              const PlMap&                     deltaSide);

        //! \throw<MamdaOrderBookException>
        void applySide       (PlMap&                           bookSide,
                              const MamdaOrderBookBasicDelta&  delta);

        //! \throw<MamdaOrderBookException>
        void applyMarketOrderSide (
                              MamdaOrderBookPriceLevel::Side   side,
                              const MamdaOrderBookPriceLevel&  level);
         
        //! \throw<MamdaOrderBookException>
        void applyMarketOrderSide (
                              MamdaOrderBookPriceLevel::Side   side,
                              const MamdaOrderBookPriceLevel&  level,
                              const MamdaOrderBookBasicDelta&  delta);
                                                 
        //! \throw<MamdaOrderBookException>
        void addLevelSide    (PlMap&                           bookSide,
                              const MamdaOrderBookPriceLevel&  level);

        void addLevelSideNoCopy (
                             PlMap&                           bookSide,
                             const MamdaOrderBookPriceLevel&  level);
                            
        //! \throw<MamdaOrderBookException>
        void addLevelSide    (PlMap&                           bookSide,
                              const MamdaOrderBookBasicDelta&  delta);
                              
         //! \throw<MamdaOrderBookException>                      
        void addMarketOrderLevelSide (
                              MamdaOrderBookPriceLevel::Side   side,
                              const MamdaOrderBookPriceLevel&  level);
            
        //! \throw<MamdaOrderBookException>
        void updateLevelSide (PlMap&                           bookSide,
                              const MamdaOrderBookPriceLevel&  level);
       
        //! \throw<MamdaOrderBookException>
        void updateLevelSide (PlMap&                           bookSide,
                              const MamdaOrderBookBasicDelta&  delta);

        //! \throw<MamdaOrderBookException>
        void updateMarketOrderLevelSide (
                              MamdaOrderBookPriceLevel::Side   side,
                              const MamdaOrderBookPriceLevel&  level);
                                               
        //! \throw<MamdaOrderBookException>
        void applyLevelSide  (PlMap&                           bookSide,
                              const MamdaOrderBookPriceLevel&  level);

        //! \throw<MamdaOrderBookException>
        void applyLevelSide  (PlMap&                           bookSide,
                              const MamdaOrderBookBasicDelta&  delta);

        //! \throw<MamdaOrderBookException>
        void applyMarketOrderLevelSide (
                              MamdaOrderBookPriceLevel::Side   side,
                              const MamdaOrderBookPriceLevel&  level);
        
        //! \throw<MamdaOrderBookException>
        void applyMarketOrderLevelSide (
                              MamdaOrderBookPriceLevel::Side   side,
                              const MamdaOrderBookPriceLevel&  level,
                              const MamdaOrderBookBasicDelta&  delta);

        //! \throw<MamdaOrderBookException>
        void deleteLevelSide (PlMap&                           bookSide,
                              const MamdaOrderBookPriceLevel&  level);

        //! \throw<MamdaOrderBookException>
        void deleteLevelSide (PlMap&                           bookSide,
                              const MamdaOrderBookBasicDelta&  delta);

        //! \throw<MamdaOrderBookException>
        void deleteMarketOrderLevelSide (
                              MamdaOrderBookPriceLevel::Side  side,
                              const MamdaOrderBookPriceLevel&  level);
                              
        //! \throw<MamdaOrderBookException>
        void markAllDeleted  (
            PlMap&        bookSide);

        //! \throw<MamdaOrderBookException>
        void determineDiffs (
            PlMap&        result,
            const PlMap&  lhs,
            const PlMap&  rhs);

        MamdaOrderBookPriceLevel* findOrCreateLevel (
            PlMap&                             bookSide,
            double                             price,
            MamdaOrderBookPriceLevel::Side     side,
            MamdaOrderBookPriceLevel::Action&  plAction);

        
        MamdaOrderBookPriceLevel* findOrCreateLevel (
            PlMap&                             bookSide,
            MamaPrice&                         price,
            MamdaOrderBookPriceLevel::Side     side,
            MamdaOrderBookPriceLevel::Action&  plAction);

        MamdaOrderBookPriceLevel* findLevel (
            PlMap&                             bookSide,
            double                             price);

        MamdaOrderBookPriceLevel* findLevel (
            PlMap&                             bookSide,
            MamaPrice&                         price);

        MamdaOrderBookPriceLevel* getMarketOrdersSide (
            MamdaOrderBookPriceLevel::Side  side);
            
        MamdaOrderBookPriceLevel* getOrCreateMarketOrdersSide (
            MamdaOrderBookPriceLevel::Side  side);
            
        void addEntry (
            PlMap&                          bookSide,
            MamdaOrderBookEntry*            entry,
            double                          price,
            MamdaOrderBookPriceLevel::Side  side,
            const MamaDateTime&             eventTime,
            MamdaOrderBookBasicDelta*       delta);

        void addEntry (
            PlMap&                          bookSide,
            MamdaOrderBookEntry*            entry,
            MamaPrice&                      price,
            MamdaOrderBookPriceLevel::Side  side,
            const MamaDateTime&             eventTime,
            MamdaOrderBookBasicDelta*       delta);
            
        void updateEntry (
            MamdaOrderBookEntry*            entry,
            mama_quantity_t                 size,
            const MamaDateTime&             eventTime,
            MamdaOrderBookBasicDelta*       delta);

        void deleteEntry (
            MamdaOrderBookEntry*            entry,
            const MamaDateTime&             eventTime,
            MamdaOrderBookBasicDelta*       delta);

        void addEntriesFromBook (
            const MamdaOrderBookImpl&       bookImpl,
            MamdaOrderBookEntryFilter*      filter,
            MamdaOrderBookBasicDeltaList*   delta);

        void addPriceLevelsFromBookAsEntries (
            const MamdaOrderBookImpl&       bookImpl,
            const char*                     sourceId,
            MamdaOrderBookBasicDeltaList*   delta);

        void deleteEntriesFromSource (
            const MamaSource*               source,
            MamdaOrderBookBasicDeltaList*   delta);
            
        bool populateDelta (MamaMsg& msg);
        void populateRecap (MamaMsg& msg);
        
        void generateDeltaMsgs (bool generate);
        
        void addDelta (MamdaOrderBookEntry*                 entry,
                       MamdaOrderBookPriceLevel*            level,
                       mama_quantity_t                      plDeltaSize,
                       MamdaOrderBookPriceLevel::Action     plAction,
                       MamdaOrderBookEntry::Action          entryAction);

        bool reevaluate (
            /*MamdaOrderBookBasicDeltaList*   delta*/);

        void cleanupDetached ();
        void detach (MamdaOrderBookPriceLevel* level);
        void detach (MamdaOrderBookEntry* entry);

        bool operator==  (const MamdaOrderBookImpl&  rhs) const;
        bool operator!=  (const MamdaOrderBookImpl&  rhs) const;
        void assertEqual (const MamdaOrderBookImpl&  rhs) const;
        void assertEqual (const PlMap&  levels,
                          const PlMap&  rhsLevels) const;
        void assertEqual (const MamdaOrderBookPriceLevel* lhslevel,
                          const MamdaOrderBookPriceLevel* rhsLevel) const;
                          
        MamdaOrderBook&                 mBook;
        string                          mSymbol;
        string                          mPartId;
        string                          mBookContributors;
        PlMap                           mBidLevels;
        PlMap                           mAskLevels;
        MamdaOrderBookPriceLevel*       mBidMarketOrders;
        MamdaOrderBookPriceLevel*       mAskMarketOrders;
        MamaDateTime                    mBookTime;
        const MamaSourceDerivative*     mSourceDeriv;
        mamaQuality                     mQuality;
        void*                           mClosure;
        bool                            mIsConsistent;
        bool                            mCheckVisibility;
        bool                            mNeedsReevaluation;
        bool                            mHasPartId;
        bool                            mGenerateDeltas;
        bool                            mHasBookContributors;
        bool                            mBookContributorsModified;
        PlList                          mDetachedLevels;
        EntryList                       mDetachedEntries;
        MamdaOrderBookSimpleDelta*      mPublishSimpleDelta;
        MamdaOrderBookComplexDelta*     mPublishComplexDelta;
        MamdaOrderBookWriter*           mWriter;
        mama_u32_t                      mCurrentDeltaCount;
    };

    struct MamdaOrderBook::bidIterator::bidIteratorImpl
    {
        bidIteratorImpl () {}
        bidIteratorImpl (const PlMap::reverse_iterator&  iter,
                         const PlMap::reverse_iterator&  end,
                         const PlMap::reverse_iterator&  begin)
            : mPlMapIter  (iter)
            , mPlMapEnd   (end)
            , mPlMapBegin (begin)
            {}
        PlMap::reverse_iterator    mPlMapIter;
        PlMap::reverse_iterator    mPlMapEnd;
        PlMap::reverse_iterator    mPlMapBegin;
    };

    struct MamdaOrderBook::askIterator::askIteratorImpl
    {
        askIteratorImpl () {}
        askIteratorImpl (const PlMap::iterator&  iter,
                         const PlMap::iterator&  end,
                         const PlMap::iterator&  begin)
            : mPlMapIter  (iter)
            , mPlMapEnd   (end)
            , mPlMapBegin (begin)
            {}
        PlMap::iterator    mPlMapIter;
        PlMap::iterator    mPlMapEnd;
        PlMap::iterator    mPlMapBegin;
    };

    struct MamdaOrderBook::bidEntryIterator::bidEntryIteratorImpl
    {
        bidEntryIteratorImpl (const MamdaOrderBook::bidIterator&  bidIter,
		              const MamdaOrderBook::bidIterator&  bidEnd)
            : mBidIter (bidIter) 
            , mBidEnd  (bidEnd)
            {
                if (bidIter != bidEnd)
                {
                    mEntryIter = (*bidIter)->begin();
                }
            }
        bidEntryIteratorImpl (const bidEntryIteratorImpl& copy)
            : mBidIter   (copy.mBidIter)
            , mBidEnd    (copy.mBidEnd)
            , mEntryIter (copy.mEntryIter)
            {}
        ~bidEntryIteratorImpl () {}
        MamdaOrderBook::bidIterator         mBidIter;
        MamdaOrderBook::bidIterator         mBidEnd;
        MamdaOrderBookPriceLevel::iterator  mEntryIter;
    };

    struct MamdaOrderBook::askEntryIterator::askEntryIteratorImpl
    {
        askEntryIteratorImpl (const MamdaOrderBook::askIterator&  askIter,
                              const MamdaOrderBook::askIterator&  askEnd)
            : mAskIter (askIter) 
            , mAskEnd  (askEnd)
            {
                if (askIter != askEnd)
                {
                    mEntryIter = (*askIter)->begin();
                }
            }
        askEntryIteratorImpl (const askEntryIteratorImpl& copy)
            : mAskIter   (copy.mAskIter)
            , mAskEnd    (copy.mAskEnd)
            , mEntryIter (copy.mEntryIter)
            {}
        ~askEntryIteratorImpl () {}
        MamdaOrderBook::askIterator         mAskIter;
        MamdaOrderBook::askIterator         mAskEnd;
        MamdaOrderBookPriceLevel::iterator  mEntryIter;
    };

    MamdaOrderBook::MamdaOrderBook()
        : mImpl (*new MamdaOrderBookImpl(*this))
    {
    }

    MamdaOrderBook::~MamdaOrderBook()
    {
        clear();
        delete &mImpl;
    }

    MamdaOrderBook::MamdaOrderBook(const MamdaOrderBook& book)
        : mImpl (*new MamdaOrderBookImpl(*this))
    {
        copy(book);
    }

    MamdaOrderBook& MamdaOrderBook::operator= (const MamdaOrderBook& rhs)
    {
        if (&rhs != this)
        {
            copy (rhs);
        }
        return *this;
    }

    void MamdaOrderBook::clear(bool deleteLevels)
    {
        if (deleteLevels)
        {
            cleanupDetached ();
            if (!mImpl.mBidLevels.empty())
            {
                PlMap::iterator itr = mImpl.mBidLevels.begin();
                PlMap::iterator end = mImpl.mBidLevels.end();
                for (; itr != end; itr++)
                {
                    MamdaOrderBookPriceLevel* level = itr->second;
                    delete level;
                }
            }
            if (!mImpl.mAskLevels.empty())
            {
                PlMap::iterator itr = mImpl.mAskLevels.begin();
                PlMap::iterator end = mImpl.mAskLevels.end();
                for (; itr != end; itr++)
                {
                    MamdaOrderBookPriceLevel* level = itr->second;
                    delete level;
                }
            }

            if (mImpl.mBidMarketOrders)
            {
                delete mImpl.mBidMarketOrders;
                mImpl.mBidMarketOrders = NULL;
            }
            if (mImpl.mAskMarketOrders)
            {
                delete mImpl.mAskMarketOrders;
                mImpl.mAskMarketOrders = NULL;
            }
        }

        mImpl.mBidLevels.clear();
        mImpl.mAskLevels.clear();
        mImpl.mIsConsistent = true;
        mImpl.mNeedsReevaluation = false;

        // do not reset mImpl.mCheckVisibility
    }

    void MamdaOrderBook::setSymbol (const char*  symbol)
    {
        mImpl.mSymbol = symbol;
    }

    const char*  MamdaOrderBook::getSymbol () const
    {
        return mImpl.mSymbol.c_str();
    }

    void MamdaOrderBook::setPartId (const char* partId)
    {
        if (partId[0] == '\0')
            mImpl.mHasPartId = false;
        else
            mImpl.mHasPartId = true;
        mImpl.mPartId = partId;
    }

    const char* MamdaOrderBook::getPartId () const
    {
        return mImpl.mPartId.c_str();
    }

    bool MamdaOrderBook::hasPartId() const
    {
        return mImpl.mHasPartId;
    }

    void MamdaOrderBook::setBookContributors (const char* bookContributors)
    {
        if (bookContributors[0] == '\0')
        {
            mImpl.mHasBookContributors = false;
        }
        else
        {
            mImpl.mHasBookContributors = true;
        }
        mImpl.mBookContributors = bookContributors;
    }

    const char* MamdaOrderBook::getBookContributors () const
    {
        return mImpl.mBookContributors.c_str();
    }

    bool MamdaOrderBook::getBookContributorsModified () const
    {
        return mImpl.mBookContributorsModified;
    }

    void MamdaOrderBook::setBookContributorsModified (bool modified)
    {
        mImpl.mBookContributorsModified = modified;
    }

    bool MamdaOrderBook::hasBookContributors() const
    {
        return mImpl.mHasBookContributors;
    }

    MamdaOrderBookPriceLevel* MamdaOrderBook::findOrCreateLevel (
        double                          price,
        MamdaOrderBookPriceLevel::Side  side)
    {
        MamdaOrderBookPriceLevel::Action ignored;
        return findOrCreateLevel (price, side, ignored);
    }

    MamdaOrderBookPriceLevel* MamdaOrderBook::findOrCreateLevel (
        double                              price,
        MamdaOrderBookPriceLevel::Side      side,
        MamdaOrderBookPriceLevel::Action&   action)
    {
        if (side == MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID)
        {
            return mImpl.findOrCreateLevel (
                mImpl.mBidLevels, price, side, action);
        }
        else if (side == MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_ASK)
        {
            return mImpl.findOrCreateLevel (
                mImpl.mAskLevels, price, side, action);
        }
        else
        {
            char msg[1000];
            snprintf (msg, 1000,"MamdaOrderBook::findOrCreateLevel(): "
                      "invalid side provided: %c", (char)side);
            throw MamdaOrderBookException(msg);
        }
    }

    MamdaOrderBookPriceLevel* MamdaOrderBook::findOrCreateLevel (
        MamaPrice&                      price,
        MamdaOrderBookPriceLevel::Side  side)
    {
        MamdaOrderBookPriceLevel::Action ignored;
        return findOrCreateLevel (price, side, ignored);
    }

    MamdaOrderBookPriceLevel* MamdaOrderBook::findOrCreateLevel (
        MamaPrice&                          price,
        MamdaOrderBookPriceLevel::Side      side,
        MamdaOrderBookPriceLevel::Action&   action)
    {
        if (side == MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID)
        {
            return mImpl.findOrCreateLevel (
                mImpl.mBidLevels, price, side, action);
        }
        else if (side == MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_ASK)
        {
            return mImpl.findOrCreateLevel (
                mImpl.mAskLevels, price, side, action);
        }
        else
        {
            char msg[1000];
            snprintf (msg, 1000,"MamdaOrderBook::findOrCreateLevel(): "
                      "invalid side provided: %c", (char)side);
            throw MamdaOrderBookException(msg);
        }
    }

    MamdaOrderBookPriceLevel* MamdaOrderBook::findLevel (
        double                              price,
        MamdaOrderBookPriceLevel::Side      side)
    {
        if (side == MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID)
        {
            return mImpl.findLevel (
                mImpl.mBidLevels, price);
        }
        else if (side == MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_ASK)
        {
            return mImpl.findLevel (
                mImpl.mAskLevels, price);
        }
        else
        {
            char msg[1000];
            snprintf (msg, 1000,"MamdaOrderBook::findLevel(): "
                      "invalid side provided: %c", (char)side);
            throw MamdaOrderBookException(msg);
        }
    }

    MamdaOrderBookPriceLevel* MamdaOrderBook::findLevel (
        MamaPrice&                          price,
        MamdaOrderBookPriceLevel::Side      side)
    {
        if (side == MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID)
        {
            return mImpl.findLevel (
                mImpl.mBidLevels, price);
        }
        else if (side == MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_ASK)
        {
            return mImpl.findLevel (
                mImpl.mAskLevels, price);
        }
        else
        {
            char msg[1000];
            snprintf (msg, 1000,"MamdaOrderBook::findLevel(): "
                      "invalid side provided: %c", (char)side);
            throw MamdaOrderBookException(msg);
        }
    }

    void MamdaOrderBook::addLevel (const MamdaOrderBookPriceLevel&  level)
    {
        mImpl.addLevel (level);
    }

    void MamdaOrderBook::updateLevel (const MamdaOrderBookPriceLevel&  level)
    {
        mImpl.updateLevel (level);
    }

    void MamdaOrderBook::generateDeltaMsgs (bool generate)
    {
        mImpl.generateDeltaMsgs (generate);
    }

    bool MamdaOrderBook::populateDelta(MamaMsg& msg)
    {
        return mImpl.populateDelta(msg);
    }

    void MamdaOrderBook::populateRecap(MamaMsg& msg)
    {
        mImpl.populateRecap(msg);
    }

    void MamdaOrderBook::deleteLevel (const MamdaOrderBookPriceLevel&  level)
    {
        mImpl.deleteLevel (level);
    }

    void MamdaOrderBook::apply (const MamdaOrderBook&  deltaBook)
    {
        mImpl.mSymbol = deltaBook.getSymbol ();
        mImpl.applySide (mImpl.mBidLevels, deltaBook.mImpl.mBidLevels);
        mImpl.applySide (mImpl.mAskLevels, deltaBook.mImpl.mAskLevels);


        if (deltaBook.mImpl.mBidMarketOrders)
        {
            mImpl.applyMarketOrderSide (
                MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID,
                *deltaBook.mImpl.mBidMarketOrders);
        }
        if (deltaBook.mImpl.mAskMarketOrders)
        {
            mImpl.applyMarketOrderSide (
                MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_ASK,
                *deltaBook.mImpl.mAskMarketOrders);
        }
        if (deltaBook.hasPartId() || mImpl.mHasPartId)
        {
            // either new book has part id we need to apply, 
            // or we need to clear our part id
            setPartId (deltaBook.getPartId());
        }
    }

    void MamdaOrderBook::apply (const MamdaOrderBookBasicDelta&  delta)
    {
        MamdaOrderBookPriceLevel* level = delta.getPriceLevel();
        if (MamdaOrderBookPriceLevel::MAMDA_BOOK_LEVEL_MARKET ==
            level->getOrderType())
        {
            mImpl.applyMarketOrderSide (level->getSide(), *level, delta);
            return;
        }
        
        switch (level->getSide())
        {
            case MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID:
                mImpl.applySide (mImpl.mBidLevels, delta);
                break;
            case MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_ASK:
                mImpl.applySide (mImpl.mAskLevels, delta);
                break;
            default:
                throw MamdaOrderBookException (
                    "MamdaOrderBook::apply (simple delta): bad side");
        }
    }

    void MamdaOrderBook::apply (const MamdaOrderBookBasicDeltaList&  deltaList)
    {
        MamdaOrderBookBasicDeltaList::const_iterator  end  = deltaList.end();
        MamdaOrderBookBasicDeltaList::const_iterator  iter = deltaList.begin();
        for (; iter != end; ++iter)
        {
            apply (*(*iter));
        }
    }

    void MamdaOrderBook::copy (const MamdaOrderBook&  rhs)
    {
        clear();
        mImpl.mIsConsistent      = rhs.mImpl.mIsConsistent;
        mImpl.mCheckVisibility   = rhs.mImpl.mCheckVisibility;
        mImpl.mNeedsReevaluation = rhs.mImpl.mNeedsReevaluation;

        apply(rhs);
    }

    void MamdaOrderBook::setAsDeltaDeleted (
        const MamdaOrderBook&  bookToDelete)
    {
        copy (bookToDelete);
        mImpl.markAllDeleted (mImpl.mBidLevels);
        mImpl.markAllDeleted (mImpl.mAskLevels);
    }

    void MamdaOrderBook::setAsDeltaDifference (
        const MamdaOrderBook&  lhs,
        const MamdaOrderBook&  rhs)
    {
        clear();
        mImpl.determineDiffs (mImpl.mBidLevels,
                              lhs.mImpl.mBidLevels,
                              rhs.mImpl.mBidLevels);
        mImpl.determineDiffs (mImpl.mAskLevels,
                              lhs.mImpl.mAskLevels,
                              rhs.mImpl.mAskLevels);
    }

    size_t MamdaOrderBook::getTotalNumLevels() const
    {
        return mImpl.mBidLevels.size() + mImpl.mAskLevels.size();
    }

    size_t MamdaOrderBook::getNumBidLevels() const
    {
        return mImpl.mBidLevels.size();
    }

    size_t MamdaOrderBook::getNumAskLevels() const
    {
        return mImpl.mAskLevels.size();
    }

    MamdaOrderBookPriceLevel* MamdaOrderBook::getBidMarketOrders() const
    {
        return mImpl.mBidMarketOrders;
    }

    MamdaOrderBookPriceLevel* MamdaOrderBook::getAskMarketOrders() const
    {
        return mImpl.mAskMarketOrders;
    }

    MamdaOrderBookPriceLevel* MamdaOrderBook::getMarketOrdersSide (
            MamdaOrderBookPriceLevel::Side side)
    {
        return mImpl.getMarketOrdersSide (side);
    }

    MamdaOrderBookPriceLevel* MamdaOrderBook::getOrCreateMarketOrdersSide (
            MamdaOrderBookPriceLevel::Side side)
    {
        return mImpl.getOrCreateMarketOrdersSide (side);
    }

    const MamaDateTime& MamdaOrderBook::getBookTime() const
    {
        return mImpl.mBookTime; 
    }

    void MamdaOrderBook::setBookTime(const MamaDateTime&  bookTime) const
    {
        mImpl.mBookTime = bookTime;
    }

    void MamdaOrderBook::setSourceDerivative (const MamaSourceDerivative*  deriv)
    {
        mImpl.mSourceDeriv = deriv;
    }

    const MamaSourceDerivative* MamdaOrderBook::getSourceDerivative () const
    {
        return mImpl.mSourceDeriv;
    }

    const MamaSource* MamdaOrderBook::getSource () const
    {
        return mImpl.mSourceDeriv ? mImpl.mSourceDeriv->getBaseSource() : NULL;
    }

    void MamdaOrderBook::setQuality (mamaQuality  quality)
    {
        mImpl.mQuality = quality;
    }

    mamaQuality MamdaOrderBook::getQuality () const
    {
        return mImpl.mQuality;
    }

    void MamdaOrderBook::setClosure (void*  closure)
    {
        mImpl.mClosure = closure;
    }

    void*  MamdaOrderBook::getClosure () const
    {
        return mImpl.mClosure;
    }

    bool MamdaOrderBook::operator== (const MamdaOrderBook&  rhs) const
    {
        return mImpl == rhs.mImpl;
    }

    void MamdaOrderBook::addEntry (
        MamdaOrderBookEntry*            entry,
        double                          price,
        MamdaOrderBookPriceLevel::Side  side,
        const MamaDateTime&             eventTime,
        MamdaOrderBookBasicDelta*       delta)
    {
        if (side == MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID)
            mImpl.addEntry (mImpl.mBidLevels, entry, price, side, 
                            eventTime, delta);
        else if (side == MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_ASK)
            mImpl.addEntry (mImpl.mAskLevels, entry, price, side, 
                            eventTime, delta);
    }

    void MamdaOrderBook::addEntry (
        MamdaOrderBookEntry*            entry,
        MamaPrice&                      price,
        MamdaOrderBookPriceLevel::Side  side,
        const MamaDateTime&             eventTime,
        MamdaOrderBookBasicDelta*       delta)
    {
        if (side == MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID)
            mImpl.addEntry (mImpl.mBidLevels, entry, price, side, 
                            eventTime, delta);
        else if (side == MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_ASK)
            mImpl.addEntry (mImpl.mAskLevels, entry, price, side, 
                            eventTime, delta);
    }


    MamdaOrderBookEntry* MamdaOrderBook::addEntry (
        const char*                     entryId,
        mama_quantity_t                 entrySize,
        double                          price,
        MamdaOrderBookPriceLevel::Side  side,
        const MamaDateTime&             eventTime,
        const MamaSourceDerivative*     source,
        MamdaOrderBookBasicDelta*       delta)
    {
        MamdaOrderBookEntry*  entry = 
            new MamdaOrderBookEntry (entryId, entrySize,
                                     MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                                     eventTime,
                                     source);
        addEntry (entry, price, side, eventTime, delta);
        return entry;
    }

    MamdaOrderBookEntry* MamdaOrderBook::addEntry (
        const char*                     entryId,
        mama_quantity_t                 entrySize,
        MamaPrice&                      price,
        MamdaOrderBookPriceLevel::Side  side,
        const MamaDateTime&             eventTime,
        const MamaSourceDerivative*     source,
        MamdaOrderBookBasicDelta*       delta)
    {
        MamdaOrderBookEntry*  entry = 
            new MamdaOrderBookEntry (entryId, entrySize,
                                     MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                                     eventTime,
                                     source);
        addEntry (entry, price, side, eventTime, delta);
        return entry;
    }


    void MamdaOrderBook::updateEntry (
        MamdaOrderBookEntry*            entry,
        mama_quantity_t                 size,
        const MamaDateTime&             eventTime,
        MamdaOrderBookBasicDelta*       delta)
    {
        mImpl.updateEntry (entry, size, eventTime, delta);
    }

    void MamdaOrderBook::deleteEntry (
        MamdaOrderBookEntry*            entry,
        const MamaDateTime&             eventTime,
        MamdaOrderBookBasicDelta*       delta)
    {
        mImpl.deleteEntry (entry, eventTime, delta);
    }

    void MamdaOrderBook::addEntriesFromBook (
        const MamdaOrderBook*           book,
        MamdaOrderBookEntryFilter*      filter,
        MamdaOrderBookBasicDeltaList*   delta)
    {
        mImpl.addEntriesFromBook (book->mImpl, filter, delta);
    }

    void MamdaOrderBook::addPriceLevelsFromBookAsEntries (
        const MamdaOrderBook*           book,
        const char*                     sourceId,
        MamdaOrderBookBasicDeltaList*   delta)
    {
        mImpl.addPriceLevelsFromBookAsEntries (book->mImpl, sourceId, delta);
    }

    void MamdaOrderBook::deleteEntriesFromSource (
        const MamaSource*               source,
        MamdaOrderBookBasicDeltaList*   delta)
    {
        mImpl.deleteEntriesFromSource (source, delta);
    }

    bool MamdaOrderBook::reevaluate ()
    {
        return mImpl.reevaluate();
    }

    void MamdaOrderBook::setNeedsReevaluation (bool  need)
    {
        mImpl.mNeedsReevaluation = need;
    }

    bool MamdaOrderBook::getNeedsReevaluation () const
    {
        return mImpl.mNeedsReevaluation;
    }

    void MamdaOrderBook::setCheckSourceState (bool check)
    {
        mImpl.mCheckVisibility = check;
    }

    bool MamdaOrderBook::getCheckSourceState () const
    {
        return mImpl.mCheckVisibility;
    }

    MamdaOrderBookPriceLevel* MamdaOrderBook::getLevelAtPrice (
        double                          price,
        MamdaOrderBookPriceLevel::Side  side) const
    {
        if (MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID == side)
        {
            PlMap::const_iterator found = mImpl.mBidLevels.find(price);
            if (found != mImpl.mBidLevels.end())
                return found->second;
            else
                return NULL;
        }
        else if (MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_ASK == side)
        {
            PlMap::const_iterator found = mImpl.mAskLevels.find(price);
            if (found != mImpl.mAskLevels.end())
                return found->second;
            else
                return NULL;
        }
        return NULL;
    }

    MamdaOrderBookPriceLevel* MamdaOrderBook::getLevelAtPosition (
        mama_u32_t                      pos,
        MamdaOrderBookPriceLevel::Side  side) const
    {
        if (MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID == side)
        {
            mama_u32_t  i = 0;
            MamdaOrderBook::bidIterator end  = bidEnd();
            MamdaOrderBook::bidIterator iter = bidBegin();
            for (; iter != end; ++iter, ++i)
            {
                if (i == pos)
                    return *iter;
            }
        }
        else
        {
            mama_u32_t  i = 0;
            MamdaOrderBook::askIterator end  = askEnd();
            MamdaOrderBook::askIterator iter = askBegin();
            for (; iter != end; ++iter, ++i)
            {
                if (i == pos)
                    return *iter;
            }
        }
        return NULL;
    }

    MamdaOrderBookEntry* MamdaOrderBook::getEntryAtPosition (
        mama_u32_t                      pos,
        MamdaOrderBookPriceLevel::Side  side) const
    {
        // Remember: pos may be zero, which would mean we want the first
        // entry in a non-empty price level.
        if (MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID == side)
        {
            MamdaOrderBook::bidIterator end  = bidEnd();
            MamdaOrderBook::bidIterator iter = bidBegin();
            for (; iter != end; ++iter)
            {
                MamdaOrderBookPriceLevel* level = *iter;
                mama_u32_t numEntries = level->getNumEntries();
                if (pos >= numEntries)
                {
                    pos -= numEntries;
                    // ... and continue to next level.
                }
                else
                {
                    return level->getEntryAtPosition (pos);
                }
            }
        }
        else
        {
            MamdaOrderBook::askIterator end  = askEnd();
            MamdaOrderBook::askIterator iter = askBegin();
            for (; iter != end; ++iter)
            {
                MamdaOrderBookPriceLevel* level = *iter;
                mama_u32_t numEntries = level->getNumEntries();
                if (pos >= numEntries)
                {
                    pos -= numEntries;
                    // ... and continue to next level.
                }
                else
                {
                    return level->getEntryAtPosition (pos);
                }
            }
        }
        return NULL;
    }

    void MamdaOrderBook::assertEqual (const MamdaOrderBook&  rhs) const
    {
        mImpl.assertEqual (rhs.mImpl);
    }

    MamdaOrderBook::bidIterator MamdaOrderBook::bidBegin()
    {
        PlMap::reverse_iterator i = mImpl.mBidLevels.rbegin();
        while ((i != mImpl.mBidLevels.rend()) &&
               ((i->second->getSize() == 0) ))
            ++i;
        return bidIterator(
            bidIterator::bidIteratorImpl (i, mImpl.mBidLevels.rend(),
                                          mImpl.mBidLevels.rbegin()));
    }

    MamdaOrderBook::bidIterator MamdaOrderBook::bidEnd()
    {
        return bidIterator(
            bidIterator::bidIteratorImpl (mImpl.mBidLevels.rend(),
                                          mImpl.mBidLevels.rend(),
                                          mImpl.mBidLevels.rbegin()));
    }

    MamdaOrderBook::constBidIterator MamdaOrderBook::bidBegin() const
    {
        PlMap::reverse_iterator i = mImpl.mBidLevels.rbegin();
        while ((i != mImpl.mBidLevels.rend()) &&
               ((i->second->getSize() == 0) ))
        {
            ++i;
        }
        return constBidIterator(
            bidIterator::bidIteratorImpl (i, mImpl.mBidLevels.rend(),
                                          mImpl.mBidLevels.rbegin()));
    }

    MamdaOrderBook::constBidIterator MamdaOrderBook::bidEnd() const
    {
        return constBidIterator(
            bidIterator::bidIteratorImpl (mImpl.mBidLevels.rend(),
                                          mImpl.mBidLevels.rend(),
                                          mImpl.mBidLevels.rbegin()));
    }

    MamdaOrderBook::askIterator MamdaOrderBook::askBegin()
    {
        PlMap::iterator i = mImpl.mAskLevels.begin();
        while ((i != mImpl.mAskLevels.end()) &&
               ((i->second->getSize() == 0)))
            ++i;
        return askIterator(
            askIterator::askIteratorImpl (i, mImpl.mAskLevels.end(),
                                          mImpl.mAskLevels.begin()));
    }

    MamdaOrderBook::askIterator MamdaOrderBook::askEnd()
    {
        return askIterator(
            askIterator::askIteratorImpl (mImpl.mAskLevels.end(),
                                          mImpl.mAskLevels.end(),
                                          mImpl.mAskLevels.begin()));
    }

    MamdaOrderBook::constAskIterator MamdaOrderBook::askBegin() const
    {
        PlMap::iterator i = mImpl.mAskLevels.begin();
        while ((i != mImpl.mAskLevels.end()) &&
               ((i->second->getSize() == 0)))
            ++i;
        return constAskIterator(
            askIterator::askIteratorImpl (i, mImpl.mAskLevels.end(),
                                          mImpl.mAskLevels.begin()));
    }

    MamdaOrderBook::constAskIterator MamdaOrderBook::askEnd() const
    {
        return constAskIterator(
            askIterator::askIteratorImpl (mImpl.mAskLevels.end(),
                                          mImpl.mAskLevels.end(),
                                          mImpl.mAskLevels.begin()));
    }

    MamdaOrderBook::bidEntryIterator MamdaOrderBook::bidEntryBegin()
    {

        bidIterator start = bidBegin();
        bidIterator end   = bidEnd();
        
        while (start != end && (*start)->begin() == (*start)->end())
        {
            ++start;
        }

        return bidEntryIterator(
            bidEntryIterator::bidEntryIteratorImpl(start,end));
    }

    MamdaOrderBook::bidEntryIterator MamdaOrderBook::bidEntryEnd()
    {
        return bidEntryIterator(
            bidEntryIterator::bidEntryIteratorImpl(bidEnd(),bidEnd()));
    }

    MamdaOrderBook::askEntryIterator MamdaOrderBook::askEntryBegin()
    {
        askIterator start = askBegin();
        askIterator end   = askEnd();
        
        while (start != end && (*start)->begin() == (*start)->end())
        {
            ++start;
        }

        return askEntryIterator(
            askEntryIterator::askEntryIteratorImpl(start, end));
    }

    MamdaOrderBook::askEntryIterator MamdaOrderBook::askEntryEnd()
    {
        return askEntryIterator(
            askEntryIterator::askEntryIteratorImpl(askEnd(),askEnd()));
    }

    const MamdaOrderBook::bidEntryIterator MamdaOrderBook::bidEntryBegin() const
    {
        bidIterator start = bidBegin();
        bidIterator end   = bidEnd();
        
        while (start != end && ((*start)->begin() == (*start)->end()) )
        {
            ++start;
        }
        return bidEntryIterator(
            bidEntryIterator::bidEntryIteratorImpl(start,end));
    }

    const MamdaOrderBook::bidEntryIterator MamdaOrderBook::bidEntryEnd() const
    {
        return bidEntryIterator(
            bidEntryIterator::bidEntryIteratorImpl(bidEnd(),bidEnd()));
    }

    const MamdaOrderBook::askEntryIterator MamdaOrderBook::askEntryBegin() const
    {

        askIterator start = askBegin();
        askIterator end   = askEnd();
        
        while (start != end && (*start)->begin() == (*start)->end())
        {
            ++start;
        }
        return askEntryIterator(
            askEntryIterator::askEntryIteratorImpl(start,end));
    }

    const MamdaOrderBook::askEntryIterator MamdaOrderBook::askEntryEnd() const
    {
        return askEntryIterator(
            askEntryIterator::askEntryIteratorImpl(askEnd(),askEnd()));
    }

    void MamdaOrderBook::setIsConsistent (bool  isConsistent)
    {
        mImpl.mIsConsistent = isConsistent;
    }

    bool MamdaOrderBook::getIsConsistent () const
    {
        return mImpl.mIsConsistent;
    }

    bool MamdaOrderBook::getGenerateDeltaMsgs()
    {
        return mImpl.mGenerateDeltas;
    }

    void MamdaOrderBook::clearDeltaList()
    {
        mImpl.mPublishComplexDelta->clear();
        mImpl.mPublishSimpleDelta->clear();
        mImpl.mCurrentDeltaCount = 0;    
    }


    void MamdaOrderBook::addDelta (
        MamdaOrderBookEntry*              entry,
        MamdaOrderBookPriceLevel*         level,
        mama_quantity_t                   plDeltaSize,
        MamdaOrderBookPriceLevel::Action  plAction,
        MamdaOrderBookEntry::Action       entryAction)
        
    {
        return mImpl.addDelta(entry, level, plDeltaSize, plAction, entryAction);
    }    

    void MamdaOrderBook::dump(ostream& output) const
    {
        output << "Dump: " << getSymbol() << "\n";

        int i = 0;

        constBidIterator bidEnd  = this->bidEnd();
        constBidIterator bidIter = bidBegin();

        for (i = 1; bidIter != bidEnd; ++bidIter, ++i)
        {
            const MamdaOrderBookPriceLevel* bidLevel = *bidIter;

            output << "Bid " << i << " | price=" << bidLevel->getPrice()
                   << " size="    << (int) bidLevel->getSize()
                   << " action="  << (char)bidLevel->getAction()
                   << " entries=" << bidLevel->getNumEntries()
                   << " time="    << (bidLevel->getTime()).getTimeAsString(); 

            if ((bidLevel->getTime()).hasDate())
                   output << " date="    << (bidLevel->getTime()).getDateAsString();

            output << "\n";

            MamdaOrderBookPriceLevel::const_iterator entryEnd  = bidLevel->end();
            MamdaOrderBookPriceLevel::const_iterator entryIter = bidLevel->begin();

            for (int j = 0; entryIter != entryEnd; ++entryIter, ++j)
            {
                const MamdaOrderBookEntry* entry = *entryIter;

                output << "      |    id=" << entry->getId()
                       << " size="   << (int) entry->getSize() 
                       << " action=" << (char)entry->getAction()
                       << " time="   << (entry->getTime()).getTimeAsString();

                if ((entry->getTime()).hasDate())
                       output << " date="   << (entry->getTime()).getDateAsString();

                output << "\n";
            }
        }

        constAskIterator askEnd  = this->askEnd();
        constAskIterator askIter = askBegin();

        for (i = 1; askIter != askEnd; ++askIter, ++i)
        {
            const MamdaOrderBookPriceLevel* askLevel = *askIter;

            output << "Ask " << i << " | price=" << askLevel->getPrice()
                   << " size="   << (int) askLevel->getSize() 
                   << " action=" << (char)askLevel->getAction()
                   << " entries=" << askLevel->getNumEntries()  
                   << " time="    << (askLevel->getTime()).getTimeAsString();

            if ((askLevel->getTime()).hasDate())
                   output << " date=" << (askLevel->getTime()).getDateAsString();

            output << "\n";

            MamdaOrderBookPriceLevel::const_iterator entryEnd  = askLevel->end();
            MamdaOrderBookPriceLevel::const_iterator entryIter = askLevel->begin();

            for (int j = 0; entryIter != entryEnd; ++entryIter, ++j)
            {
                const MamdaOrderBookEntry* entry = *entryIter;

                output << "      |    id=" << entry->getId()
                       << " size="   << (int) entry->getSize() 
                       << " action=" << (char)entry->getAction()
                       << " time="   << (entry->getTime()).getTimeAsString();

                if ((entry->getTime()).hasDate())
                       output << " date="   << (entry->getTime()).getDateAsString();

                output << "\n";
            }
        }
        
        if (mImpl.mAskMarketOrders)
        {
            output << "Ask Market Orders"
                   << " size="   << (int) mImpl.mAskMarketOrders->getSize()
                   << " action=" << (char)mImpl.mAskMarketOrders->getAction()
                   << " entries=" << mImpl.mAskMarketOrders->getNumEntries()
                   << " time="    << (mImpl.mAskMarketOrders->getTime()).getTimeAsString();

            if ((mImpl.mAskMarketOrders->getTime()).hasDate())
                   output << " date=" << (mImpl.mAskMarketOrders->getTime()).getDateAsString();

            output << "\n";

            MamdaOrderBookPriceLevel::const_iterator entryEnd  = mImpl.mAskMarketOrders->end();
            MamdaOrderBookPriceLevel::const_iterator entryIter = mImpl.mAskMarketOrders->begin();

            for (int j = 0; entryIter != entryEnd; ++entryIter, ++j)
            {
                const MamdaOrderBookEntry* entry = *entryIter;

                output << "      |    id=" << entry->getId()
                       << " size="   << (int) entry->getSize() 
                       << " action=" << (char)entry->getAction()
                       << " time="   << (entry->getTime()).getTimeAsString();

                if ((entry->getTime()).hasDate())
                       output << " date="   << (entry->getTime()).getDateAsString();

                output << "\n";
            }
        }

        if (mImpl.mBidMarketOrders)
        {
            output << "Bid Market Orders"
                   << " size="   << (int) mImpl.mBidMarketOrders->getSize()
                   << " action=" << (char)mImpl.mBidMarketOrders->getAction()
                   << " entries=" << mImpl.mBidMarketOrders->getNumEntries()
                   << " time="    << (mImpl.mBidMarketOrders->getTime()).getTimeAsString();

            if ((mImpl.mBidMarketOrders->getTime()).hasDate())
                   output << " date=" << (mImpl.mBidMarketOrders->getTime()).getDateAsString();

            output << "\n";

            MamdaOrderBookPriceLevel::const_iterator entryEnd  =
                                                    mImpl.mBidMarketOrders->end();
            MamdaOrderBookPriceLevel::const_iterator entryIter =
                                                    mImpl.mBidMarketOrders->begin();

            for (int j = 0; entryIter != entryEnd; ++entryIter, ++j)
            {
                const MamdaOrderBookEntry* entry = *entryIter;

                output << "      |    id=" << entry->getId()
                       << " size="   << (int) entry->getSize() 
                       << " action=" << (char)entry->getAction()
                       << " time="   << (entry->getTime()).getTimeAsString();

                if ((entry->getTime()).hasDate())
                       output << " date="   << (entry->getTime()).getDateAsString();

                output << "\n";
            }
        }
    }

    MamdaOrderBook::MamdaOrderBookImpl::MamdaOrderBookImpl (MamdaOrderBook& book)
        : mBook                     (book)
        , mBidMarketOrders          (NULL)
        , mAskMarketOrders          (NULL)
        , mSourceDeriv              (NULL)
        , mQuality                  (MAMA_QUALITY_UNKNOWN)
        , mClosure                  (NULL)
        , mIsConsistent             (true)
        , mCheckVisibility          (false)
        , mNeedsReevaluation        (false)
        , mHasPartId                (false)
        , mHasBookContributors      (false)
        , mGenerateDeltas           (false)
        , mBookContributorsModified (false)
        , mPublishSimpleDelta       (NULL)
        , mPublishComplexDelta      (NULL)
        , mWriter                   (NULL)
        , mCurrentDeltaCount        (0)
    {
    }

    MamdaOrderBook::MamdaOrderBookImpl::~MamdaOrderBookImpl ()
    {
        if (mWriter != NULL)
        {
            delete mWriter;
            mWriter = NULL;
        }
        if (mPublishComplexDelta)
        {
            mPublishComplexDelta->clear();
            delete mPublishComplexDelta;
            mPublishComplexDelta = NULL;
        }
        if (mPublishSimpleDelta)
        {
            mPublishSimpleDelta->clear();
            delete mPublishSimpleDelta;
            mPublishSimpleDelta = NULL;        
        }
    }

    void MamdaOrderBook::MamdaOrderBookImpl::generateDeltaMsgs(bool generate)
    {
        mGenerateDeltas = generate;

        if (generate)
        {
            if (!mPublishSimpleDelta)
                mPublishSimpleDelta = new MamdaOrderBookConcreteSimpleDelta();
            if (!mPublishComplexDelta)
                mPublishComplexDelta = new MamdaOrderBookConcreteComplexDelta();
            if (!mWriter)
                mWriter = new MamdaOrderBookWriter();

            mPublishComplexDelta->setConflateDeltas(true);
        }

        if (!generate)
        {
            if (mPublishSimpleDelta)
            {
                delete mPublishSimpleDelta;
                mPublishSimpleDelta = NULL;
            }
                
            if (mPublishComplexDelta)
            {
                delete mPublishComplexDelta;
                mPublishComplexDelta = NULL;
            }
            if (mWriter)
            {
                delete mWriter;
                mWriter = NULL;
            }
        }
        
    }

    bool MamdaOrderBook::MamdaOrderBookImpl::populateDelta(MamaMsg& msg)
    {
        if (mGenerateDeltas)
        {
            if (0 == mCurrentDeltaCount)
            {
                mama_log (MAMA_LOG_LEVEL_FINEST,
                          "MamdaOrderBook::populateDelta() Trying to create publisher msg from empty state");
                return false;
            }
            if (1 == mCurrentDeltaCount)
            {
                mWriter->populateMsg(msg, *mPublishSimpleDelta);
                mPublishSimpleDelta->clear();
                mCurrentDeltaCount = 0;
                mBook.cleanupDetached();
                return true;
            }
            else
            {
                mWriter->populateMsg(msg, *mPublishComplexDelta);
                mPublishComplexDelta->clear();
                mPublishSimpleDelta->clear();
                mBook.cleanupDetached();
                mCurrentDeltaCount = 0;
                return true;
            }
        }
        else
        {
            mama_log (MAMA_LOG_LEVEL_WARN,
                          "MamdaOrderBook::populateDelta() Order Book publishing not enabled");
            return false;
        }
    }

    void MamdaOrderBook::MamdaOrderBookImpl::populateRecap(MamaMsg& msg)
    {
        if (mGenerateDeltas)
        {
            mWriter->populateMsg(msg, mBook);
        }
        else
        {
            mama_log (MAMA_LOG_LEVEL_WARN,
                          "MamdaOrderBook::populateRecap() Order Book publishing not enabled");
        }
    }

    void MamdaOrderBook::MamdaOrderBookImpl::addLevel (
        const MamdaOrderBookPriceLevel&  level)
    {
        try
        {
            switch (level.getSide())
            {
                case MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID:
                    addLevelSideNoCopy (mBidLevels, level);
                    break;
                case MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_ASK:
                    addLevelSideNoCopy (mAskLevels, level);
                    break;
                case MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_UNKNOWN:
                    // explicitly not handled
                    break;
            }
        }
        catch (MamdaOrderBookException& e)
        {
            char errMsg[256];
            snprintf (errMsg, 256, "%s (price=%g)", e.what(), level.getPrice());
            throw MamdaOrderBookException (errMsg);
        }
    }

    void MamdaOrderBook::MamdaOrderBookImpl::updateLevel (
        const MamdaOrderBookPriceLevel&  level)
    {
        try
        {
            switch (level.getSide())
            {
                case MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID:
                    updateLevelSide (mBidLevels, level);
                    break;
                case MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_ASK:
                    updateLevelSide (mAskLevels, level);
                    break;
                case MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_UNKNOWN:
                    // explicitly not handled
                    break;
            }
        }
        catch (MamdaOrderBookException& e)
        {
            char errMsg[256];
            snprintf (errMsg, 256, "%s (price=%g)", e.what(), level.getPrice());
            throw MamdaOrderBookException (errMsg);
        }
    }

    void MamdaOrderBook::MamdaOrderBookImpl::deleteLevel (
        const MamdaOrderBookPriceLevel&  level)
    {
        try
        {
            switch (level.getSide())
            {
                case MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID:
                    deleteLevelSide (mBidLevels, level);
                    break;
                case MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_ASK:
                    deleteLevelSide (mAskLevels, level);
                    break;
                case MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_UNKNOWN:
                    // explicitly not handled
                    break;
            }
        }
        catch (MamdaOrderBookException& e)
        {
            char errMsg[256];
            snprintf (errMsg, 256, "%s (price=%g)", e.what(), level.getPrice());
            throw MamdaOrderBookException (errMsg);
        }
    }

    void MamdaOrderBook::MamdaOrderBookImpl::applySide (
        PlMap&        bookSide,
        const PlMap&  deltaSide)
    {
        PlMap::const_iterator end = deltaSide.end();
        PlMap::const_iterator i;
        for (i = deltaSide.begin(); i != end; ++i)
        {
            const MamdaOrderBookPriceLevel* level = i->second;
            try
            {
                switch (level->getAction())
                {
                    case MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD:
                        addLevelSide (bookSide, *level);
                        break;
                    case MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_UPDATE: 
                      applyLevelSide (bookSide, *level);
                        break;
                    case MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_DELETE:
                        deleteLevelSide (bookSide, *level);
                        break;
                    case MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_UNKNOWN:
                        // explicitly not handled
                        break;
                }
            }
            catch (MamdaOrderBookException& e)
            {
                char errMsg[256];
                snprintf (errMsg, 256, "%s (price=%g)",
                          e.what(), level->getPrice());
                throw MamdaOrderBookException (errMsg);
            }
        }
    }

    void MamdaOrderBook::MamdaOrderBookImpl::applySide (
        PlMap&                           bookSide,
        const MamdaOrderBookBasicDelta&  delta)
    {
        const MamdaOrderBookPriceLevel* level = delta.getPriceLevel();
        try
        {
            switch (delta.getPlDeltaAction())
            {
                case MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD:
                    addLevelSide (bookSide, delta);
                    break;
                case MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_UPDATE: 
                    applyLevelSide (bookSide, delta);
                    break;
                case MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_DELETE:
                    deleteLevelSide (bookSide, delta);
                    break;
                case MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_UNKNOWN:
                    // explicitly not handled
                    break;
            }
        }
        catch (MamdaOrderBookException& e)
        {
            char errMsg[256];
            snprintf (errMsg, 256, "%s (price=%g)",
                      e.what(), level->getPrice());
            throw MamdaOrderBookException (errMsg);
        }
    }

    void MamdaOrderBook::MamdaOrderBookImpl::applyMarketOrderSide (
        MamdaOrderBookPriceLevel::Side   side,
        const MamdaOrderBookPriceLevel&  level,
        const MamdaOrderBookBasicDelta&  delta)
    {
        try
        {
            switch (delta.getPlDeltaAction())
            {
                case MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD:
                    addMarketOrderLevelSide (side, level);
                    break;
                case MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_UPDATE:
                    applyMarketOrderLevelSide (side, level, delta);
                    break;
                case MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_DELETE:
                    deleteMarketOrderLevelSide (side, level);
                    break;
                case MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_UNKNOWN:
                    // explicitly not handled
                    break;
            }
        }
        catch (MamdaOrderBookException& e)
        {
            char errMsg[256];
            snprintf (errMsg, 256, "%s Market order",
                     e.what());
            throw MamdaOrderBookException (errMsg);
        }        
    }

    void MamdaOrderBook::MamdaOrderBookImpl::applyMarketOrderSide (
        MamdaOrderBookPriceLevel::Side   side,
        const MamdaOrderBookPriceLevel&  level)
    {
        try
        {
            switch (level.getAction())
            {
                case MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD:
                    addMarketOrderLevelSide (side, level);
                    break;
                case MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_UPDATE:
                    applyMarketOrderLevelSide (side, level);
                    break;
                case MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_DELETE:
                    deleteMarketOrderLevelSide (side, level);
                    break;
                case MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_UNKNOWN:
                        // explicitly not handled
                    break;
            }
        }
      catch (MamdaOrderBookException& e)
      {
        char errMsg[256];
        snprintf (errMsg, 256, "%s Market order",
                  e.what());
        throw MamdaOrderBookException (errMsg);
      }        
    }


    void MamdaOrderBook::MamdaOrderBookImpl::addLevelSide (
        PlMap&                           bookSide,
        const MamdaOrderBookPriceLevel&  level)
    {
        double price = level.getPrice();
        PlMap::iterator found = bookSide.find(price);
        if (found == bookSide.end())
        {
            MamdaOrderBookPriceLevel* levelCopy =
                new MamdaOrderBookPriceLevel (level);
            bookSide.insert (PlMap::value_type(price, levelCopy));
            levelCopy->setOrderBook(&mBook);
            
            if (mGenerateDeltas)
            {
                addDelta(NULL, levelCopy, levelCopy->getSizeChange(), 
                         MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD, 
                         MamdaOrderBookEntry::MAMDA_BOOK_ACTION_UNKNOWN);
            }
        }
        else
        {
            updateLevelSide (bookSide, level);
            char errMsg[256];
            snprintf (errMsg, 256,
                      "addLevel(%s): price=%g already exists in book (size=%g)",
                      mSymbol.c_str(), price, found->second->getSize());
            throw MamdaOrderBookException (errMsg);
        }
    }

    void MamdaOrderBook::MamdaOrderBookImpl::addLevelSideNoCopy (
        PlMap&                           bookSide,
        const MamdaOrderBookPriceLevel&  level)
    {
        double price = level.getPrice();
        PlMap::iterator found = bookSide.find(price);
        if (found == bookSide.end())
        {
            bookSide.insert (PlMap::value_type(price,
                            (MamdaOrderBookPriceLevel*)&level));
        
            if (mGenerateDeltas)
            {
                addDelta(NULL, (MamdaOrderBookPriceLevel*)&level, level.getSizeChange(), 
                         MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD, 
                         MamdaOrderBookEntry::MAMDA_BOOK_ACTION_UNKNOWN);
            }  
        }
        else 
        {
            updateLevelSide (bookSide, level);
            char errMsg[256];
            snprintf (errMsg, 256,
                      "addLevel(%s): price=%g already exists in book (size=%g)",
                      mSymbol.c_str(), price, found->second->getSize());
            throw MamdaOrderBookException (errMsg);
        }
    }

    void MamdaOrderBook::MamdaOrderBookImpl::addLevelSide (
        PlMap&                           bookSide,
        const MamdaOrderBookBasicDelta&  delta)
    {
        const MamdaOrderBookPriceLevel* level = delta.getPriceLevel();
        double price = level->getPrice();
        PlMap::iterator found = bookSide.find(price);
        if (found == bookSide.end())
        {
            MamdaOrderBookPriceLevel* levelCopy =
                new MamdaOrderBookPriceLevel (*level);
            bookSide.insert (PlMap::value_type(price, levelCopy));
            levelCopy->setOrderBook(&mBook);
            
            if (mGenerateDeltas)
            {
                addDelta(delta.getEntry(), levelCopy, levelCopy->getSizeChange(), 
                         MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD, 
                         MamdaOrderBookEntry::MAMDA_BOOK_ACTION_UNKNOWN);
            }
        }
        else
        {
            updateLevelSide (bookSide, delta);
            char errMsg[256];
            snprintf (errMsg, 256,
                      "addLevel(%s): price=%g already exists in book (size=%g)",
                      mSymbol.c_str(), price, found->second->getSize());
            throw MamdaOrderBookException (errMsg);
        }
    }

    void MamdaOrderBook::MamdaOrderBookImpl::addMarketOrderLevelSide (
        MamdaOrderBookPriceLevel::Side   side,
        const MamdaOrderBookPriceLevel&  level)
    {
        MamdaOrderBookPriceLevel* bookLevel = getMarketOrdersSide (side);
        if (!bookLevel)
        {
            bookLevel = new MamdaOrderBookPriceLevel (level);
            bookLevel->setOrderBook(&mBook);
            if (MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID == side)
                mBidMarketOrders = bookLevel;
            else if (MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_ASK == side)
                mAskMarketOrders = bookLevel;
            else
            {
                char msg[1000];
                snprintf (msg, 1000,"MamdaOrderBook::addMarketOrderSide(): "
                                    "invalid side provided: %c", (char)side);
                throw MamdaOrderBookException(msg);
            }
        }
        else
        {
            updateMarketOrderLevelSide (side, level);
            char errMsg[256];
            snprintf (errMsg, 256,
                      "addMarketOrderSide(%s): "
                      "side=%c already exists in book (size=%g)",
                      mSymbol.c_str(), side, level.getSize());
            throw MamdaOrderBookException (errMsg);
        }  
    }

    void MamdaOrderBook::MamdaOrderBookImpl::updateLevelSide (
        PlMap&                           bookSide,
        const MamdaOrderBookPriceLevel&  level)
    {
        double price = level.getPrice();
        PlMap::iterator found = bookSide.find(price);
        if (found != bookSide.end())
        {
            found->second->copy (level);
            
            if (mGenerateDeltas)
            {
                addDelta(NULL, (MamdaOrderBookPriceLevel*)&level, level.getSizeChange(), 
                         MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_UPDATE, 
                         MamdaOrderBookEntry::MAMDA_BOOK_ACTION_UNKNOWN);
            }
        }
        else
        {
            addLevelSide (bookSide, level);
            char errMsg[256];
            snprintf (errMsg, 256,
                      "updateLevel(%s): price=%g does not exist in book",
                      mSymbol.c_str(), price);
            throw MamdaOrderBookException (errMsg);
        }
    }

    void MamdaOrderBook::MamdaOrderBookImpl::updateLevelSide (
        PlMap&                           bookSide,
        const MamdaOrderBookBasicDelta&  delta)
    {
        const MamdaOrderBookPriceLevel* level = delta.getPriceLevel();
        double price = level->getPrice();
        PlMap::iterator found = bookSide.find(price);
        if (found != bookSide.end())
        {
            /* Note: If the update does not contain all of the entries,
             * then we should have used applyLevelSide instead! */
            found->second->copy (*level);
            found->second->setOrderBook (&mBook);
            
            if (mGenerateDeltas)
            {
                addDelta(delta.getEntry(), delta.getPriceLevel(), level->getSizeChange(),
                         MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_UPDATE, 
                         MamdaOrderBookEntry::MAMDA_BOOK_ACTION_UNKNOWN);
            }
        }
        else
        {
            addLevelSide (bookSide, delta);
            char errMsg[256];
            snprintf (errMsg, 256,
                      "updateLevel(%s): price=%g does not exist in book",
                      mSymbol.c_str(), price);
            throw MamdaOrderBookException (errMsg);
        }
    }

    void MamdaOrderBook::MamdaOrderBookImpl::updateMarketOrderLevelSide(
        MamdaOrderBookPriceLevel::Side   side,
        const MamdaOrderBookPriceLevel&  level)
    {
        MamdaOrderBookPriceLevel* existingLevel = getMarketOrdersSide (side);
        if (existingLevel)
        {
            /* Note: If the update does not contain all of the entries,
             * then we should have used applyLevelSide instead! */
            existingLevel->copy (level);
            existingLevel->setOrderBook (&mBook);
        }
        else
        {
            addMarketOrderLevelSide (side, level);
            char errMsg[256];
            snprintf (errMsg, 256,
                      "updateMarketOrderSide(%s): "
                      "side=%c does not exist in book(size=%g)",
                      mSymbol.c_str(), side, level.getSize());
            throw MamdaOrderBookException (errMsg);
        }
    }

    void MamdaOrderBook::MamdaOrderBookImpl::applyLevelSide (
        PlMap&                           bookSide,
        const MamdaOrderBookPriceLevel&  level)
    {
        double price = level.getPrice();
        PlMap::iterator found = bookSide.find(price);
        if (found != bookSide.end())
        {
            MamdaOrderBookPriceLevel*  existingLevel = found->second;
            MamdaOrderBookPriceLevel::iterator end = level.end();
            MamdaOrderBookPriceLevel::iterator i   = level.begin();
            while (i != end)
            {
                MamdaOrderBookEntry* entry = new MamdaOrderBookEntry(**i);
                switch (entry->getAction())
                {
                    case MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD:
                        existingLevel->addEntry (entry);
                        break;
                    case MamdaOrderBookEntry::MAMDA_BOOK_ACTION_UPDATE:
                        existingLevel->updateEntry (*entry);
                        break;
                    case MamdaOrderBookEntry::MAMDA_BOOK_ACTION_DELETE:
                        existingLevel->removeEntryById (*entry);
                        break;
                    case MamdaOrderBookEntry::MAMDA_BOOK_ACTION_UNKNOWN:
                        // explicitly not handled
                        break;
                }
                ++i;
            }
            existingLevel->setDetails (level);
        }
        else
        {
            addLevelSide (bookSide, level);
            char errMsg[256];
            snprintf (errMsg, 256,
                      "applyLevel(%s): price=%g does not exist in book",
                      mSymbol.c_str(), price);
            throw MamdaOrderBookException (errMsg);
        }
    }

    void MamdaOrderBook::MamdaOrderBookImpl::applyLevelSide (
        PlMap&                           bookSide,
        const MamdaOrderBookBasicDelta&  delta)
    {
        const MamdaOrderBookPriceLevel* level = delta.getPriceLevel();
        double price = level->getPrice();
        PlMap::iterator found = bookSide.find(price);
        if (found != bookSide.end())
        {
            MamdaOrderBookPriceLevel*  existingLevel = found->second;
            MamdaOrderBookEntry*       entry = delta.getEntry();
            if (entry)
            {
                switch (delta.getEntryDeltaAction())
                {
                    case MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD:
                        existingLevel->addEntry (new MamdaOrderBookEntry(*entry));
                        break;
                    case MamdaOrderBookEntry::MAMDA_BOOK_ACTION_UPDATE:
                        existingLevel->updateEntry (*entry);
                        break;
                    case MamdaOrderBookEntry::MAMDA_BOOK_ACTION_DELETE:
                        existingLevel->removeEntryById (*entry);
                        break;
                    case MamdaOrderBookEntry::MAMDA_BOOK_ACTION_UNKNOWN:
                        // explicitly not handled
                        break;
                }
            }
            existingLevel->setDetails (*level);
        }
        else
        {
            addLevelSide (bookSide, delta);
            char errMsg[256];
            snprintf (errMsg, 256,
                      "applyLevel(%s): price=%g does not exist in book",
                      mSymbol.c_str(), price);
            throw MamdaOrderBookException (errMsg);
        }
    }

    void MamdaOrderBook::MamdaOrderBookImpl::applyMarketOrderLevelSide (
        MamdaOrderBookPriceLevel::Side   side,
        const MamdaOrderBookPriceLevel&  level)
    {
        MamdaOrderBookPriceLevel* existingLevel = getMarketOrdersSide (side);
        if (existingLevel)
        {
            MamdaOrderBookPriceLevel::iterator end = level.end();
            MamdaOrderBookPriceLevel::iterator i   = level.begin();
            while (i != end)
            {
                MamdaOrderBookEntry* entry = new MamdaOrderBookEntry(**i);
                switch (entry->getAction())
                {
                    case MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD:
                        existingLevel->addEntry (entry);
                        break;
                    case MamdaOrderBookEntry::MAMDA_BOOK_ACTION_UPDATE:
                        existingLevel->updateEntry (*entry);
                        break;
                    case MamdaOrderBookEntry::MAMDA_BOOK_ACTION_DELETE:
                        existingLevel->removeEntryById (*entry);
                        break;
                    case MamdaOrderBookEntry::MAMDA_BOOK_ACTION_UNKNOWN:
                        // explicitly not handled
                        break;
                }
                ++i;
            }
            existingLevel->setDetails (level);
        }
        else
        {
            addMarketOrderLevelSide (side, level);
            char errMsg[256];
            snprintf (errMsg, 256,
                      "updateMarketOrderSide(%s): "
                      "side=%c does not exist in book(size=%g)",
                      mSymbol.c_str(), side, level.getSize());
            throw MamdaOrderBookException (errMsg);
        }
    }

    void MamdaOrderBook::MamdaOrderBookImpl::applyMarketOrderLevelSide (
        MamdaOrderBookPriceLevel::Side   side,
        const MamdaOrderBookPriceLevel&  level,
        const MamdaOrderBookBasicDelta&  delta)
    {
        MamdaOrderBookPriceLevel* existingLevel = getMarketOrdersSide (side);
        if (existingLevel)
        {

            MamdaOrderBookEntry* entry = delta.getEntry();
            if (entry)
            {
                switch (delta.getEntryDeltaAction())
                {
                    case MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD:
                        existingLevel->addEntry (new MamdaOrderBookEntry(*entry));
                        break;
                    case MamdaOrderBookEntry::MAMDA_BOOK_ACTION_UPDATE:
                        existingLevel->updateEntry (*entry);
                        break;
                    case MamdaOrderBookEntry::MAMDA_BOOK_ACTION_DELETE:
                        existingLevel->removeEntryById (*entry);
                        break;
                    case MamdaOrderBookEntry::MAMDA_BOOK_ACTION_UNKNOWN:
                        // explicitly not handled
                        break;
                }
            }
            existingLevel->setDetails (level);
        }
        else
        {
            addMarketOrderLevelSide (side, level);
            char errMsg[256];
            snprintf (errMsg, 256,
                      "updateMarketOrderSide(%s): "
                      "side=%c does not exist in book(size=%g)",
                      mSymbol.c_str(), side, level.getSize());
            throw MamdaOrderBookException (errMsg);
        }
    }    

    void MamdaOrderBook::MamdaOrderBookImpl::deleteLevelSide (
        PlMap&                           bookSide,
        const MamdaOrderBookPriceLevel&  level)
    {
        double price = level.getPrice();
        PlMap::iterator found = bookSide.find(price);
        if (found != bookSide.end())
        {
            bookSide.erase(found);
            
            if (mGenerateDeltas)
            {
                addDelta(NULL, (MamdaOrderBookPriceLevel*)&level, level.getSizeChange(), 
                         MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_DELETE,
                         MamdaOrderBookEntry::MAMDA_BOOK_ACTION_UNKNOWN);
            }
        }
        else
        {
            char errMsg[256];
            snprintf (errMsg, 256,
                      "deleteLevel(%s): price=%g does not exist in book",
                      mSymbol.c_str(), price);
            throw MamdaOrderBookException (errMsg);
        }
    }

    void MamdaOrderBook::MamdaOrderBookImpl::deleteLevelSide (
        PlMap&                           bookSide,
        const MamdaOrderBookBasicDelta&  delta)
    {
        const MamdaOrderBookPriceLevel* level = delta.getPriceLevel();
        double price = level->getPrice();
        PlMap::iterator found = bookSide.find(price);
        if (found != bookSide.end())
        {
            bookSide.erase(found);
            
            if (mGenerateDeltas)
            {
                addDelta(delta.getEntry(), delta.getPriceLevel(), level->getSizeChange(),
                         MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_DELETE,
                         delta.getEntry()->getAction());
            }
        }
        else
        {
            char errMsg[256];
            snprintf (errMsg, 256,
                      "deleteLevel(%s): price=%g does not exist in book",
                      mSymbol.c_str(), price);
            throw MamdaOrderBookException (errMsg);
        }
    }

    void MamdaOrderBook::MamdaOrderBookImpl::deleteMarketOrderLevelSide (
        MamdaOrderBookPriceLevel::Side   side,
        const MamdaOrderBookPriceLevel&  level)
    {
        if (MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID == side)
        {
            delete mBidMarketOrders;
            mBidMarketOrders = NULL;
            return;
        }
        else if (MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_ASK == side)
        {
            delete mAskMarketOrders;
            mAskMarketOrders = NULL;
            return;
        }
        else
        {
            char errMsg[256];
            snprintf (errMsg, 256,
                      "deleteMarketOrderSide (%s): side%c does not exist in book",
                      mSymbol.c_str(), side);
            throw MamdaOrderBookException (errMsg);
        }
    }

    void MamdaOrderBook::MamdaOrderBookImpl::markAllDeleted (
        PlMap&        bookSide)
    {
        PlMap::iterator  endIter = bookSide.end();
        PlMap::iterator  iter    = bookSide.begin();
        for (; iter != endIter; ++iter)
        {
            MamdaOrderBookPriceLevel* level = iter->second;
            level->markAllDeleted();
        }
    }

    void MamdaOrderBook::MamdaOrderBookImpl::determineDiffs (
        PlMap&        resultSide,
        const PlMap&  lhs,
        const PlMap&  rhs)
    {
        PlMap::const_iterator  lhsIter = lhs.begin();
        PlMap::const_iterator  lhsEnd  = lhs.end();
        PlMap::const_iterator  rhsIter = rhs.begin();
        PlMap::const_iterator  rhsEnd  = rhs.end();

        while ((lhsIter != lhsEnd) || (rhsIter != rhsEnd))
        {
            const MamdaOrderBookPriceLevel* lhsLevel = NULL;
            const MamdaOrderBookPriceLevel* rhsLevel = NULL;
            double           lhsPrice = DBL_MIN;
            double           rhsPrice = DBL_MIN;
            mama_quantity_t  lhsSize  = 0;
            mama_quantity_t  rhsSize  = 0;

            if (lhsIter != lhsEnd)
            {
                lhsLevel = lhsIter->second;
                lhsPrice = lhsLevel->getPrice();
                lhsSize  = lhsLevel->getSize();
            }
            if (rhsIter != rhsEnd)
            {
                rhsLevel = rhsIter->second;
                rhsPrice = rhsLevel->getPrice();
                rhsSize  = rhsLevel->getSize();
            }

            if ((lhsPrice == rhsPrice) && (lhsSize == rhsSize))
            {
                // Usual scenario: both levels are the same
                ++lhsIter;
                ++rhsIter;
                continue;
            }

            if (lhsPrice == rhsPrice)
            {
                // Same price, different size.  Need to determine the
                // different entries.
                assert (lhsLevel != NULL);
                assert (rhsLevel != NULL);
                MamdaOrderBookPriceLevel* diffLevel = new MamdaOrderBookPriceLevel;
                diffLevel->setAsDifference (*lhsLevel, *rhsLevel);
                resultSide.insert (PlMap::value_type (lhsPrice, diffLevel));
                ++lhsIter;
                ++rhsIter;
                continue;
            }

            if (((lhsPrice > rhsPrice) && (rhsPrice != DBL_MIN)) ||
                (lhsPrice == DBL_MIN))
            {
                // RHS has an additional price level
                assert (rhsLevel != NULL);
                MamdaOrderBookPriceLevel* diffLevel = 
                    new MamdaOrderBookPriceLevel (*rhsLevel);
                resultSide.insert (PlMap::value_type (rhsPrice, diffLevel));
                ++rhsIter;
                continue;
            }
            else
            {
                // RHS does not have a price level that is on the LHS.
                // Copy the LHS level and mark all as deleted.
                assert (lhsLevel != NULL);
                MamdaOrderBookPriceLevel* diffLevel =
                    new MamdaOrderBookPriceLevel (*lhsLevel);
                diffLevel->markAllDeleted();
                resultSide.insert (PlMap::value_type (lhsPrice, diffLevel));
                ++lhsIter;
                continue;
            }
        }
    }

    MamdaOrderBookPriceLevel*
    MamdaOrderBook::MamdaOrderBookImpl::findOrCreateLevel (
        PlMap&                             bookSide,
        double                             price,
        MamdaOrderBookPriceLevel::Side     side,
        MamdaOrderBookPriceLevel::Action&  plAction)
    {
        PlMap::iterator found = bookSide.find(price);
        if (found == bookSide.end())
        {
            MamdaOrderBookPriceLevel*  level =
                new MamdaOrderBookPriceLevel (price, side);
            bookSide.insert (PlMap::value_type(price,level));
            level->setOrderBook (&mBook);
            plAction = MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD;
            
            if (mGenerateDeltas)
            {
                addDelta(NULL, level, NULL, 
                         MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD, 
                         MamdaOrderBookEntry::MAMDA_BOOK_ACTION_UNKNOWN);
            }
            return level;

        }
        else
        {
            MamdaOrderBookPriceLevel*  level = found->second;
            /* The level may exist but could be empty, so the plAction may
             * be ADD or UPDATE. */
            plAction =  MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_UPDATE;
            return level;
        }
    }

    MamdaOrderBookPriceLevel*
    MamdaOrderBook::MamdaOrderBookImpl::findOrCreateLevel (
        PlMap&                             bookSide,
        MamaPrice&                         price,
        MamdaOrderBookPriceLevel::Side     side,
        MamdaOrderBookPriceLevel::Action&  plAction)
    {
        PlMap::iterator found = bookSide.find(price.getValue());
        if (found == bookSide.end())
        {
            MamdaOrderBookPriceLevel*  level =
                new MamdaOrderBookPriceLevel (price, side);
            bookSide.insert (PlMap::value_type(price.getValue(), level));
            level->setOrderBook (&mBook);
            plAction = MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD;
            if (mGenerateDeltas)
            {
                addDelta(NULL, level, NULL, 
                         MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD, 
                         MamdaOrderBookEntry::MAMDA_BOOK_ACTION_UNKNOWN);
            }   
            return level;
        }
        else
        {
            MamdaOrderBookPriceLevel*  level = found->second;
            /* The level may exist but could be empty, so the plAction may
             * be ADD or UPDATE. */
            plAction = MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_UPDATE;
            return level;
        }
    }

    MamdaOrderBookPriceLevel*
    MamdaOrderBook::MamdaOrderBookImpl::findLevel (
        PlMap&                             bookSide,
        double                             price)
    {
        PlMap::iterator found = bookSide.find(price);
        if (found == bookSide.end())
        {
            return NULL;
        }
        else
        {
            return found->second;
        }
    }

    MamdaOrderBookPriceLevel*
    MamdaOrderBook::MamdaOrderBookImpl::findLevel (
        PlMap&                             bookSide,
        MamaPrice&                         price)
    {
        PlMap::iterator found = bookSide.find(price.getValue());
        if (found == bookSide.end())
        {
            return NULL;
        }
        else
        {
            return found->second;
        }
    }


    MamdaOrderBookPriceLevel*
    MamdaOrderBook::MamdaOrderBookImpl::getMarketOrdersSide (
            MamdaOrderBookPriceLevel::Side side) 
    {
        if (MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID == side)
            return mBidMarketOrders;
        else if (MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_ASK == side)
            return mAskMarketOrders;
        else
        {
            char msg[1000];
            snprintf (msg, 1000,"MamdaOrderBook::getMarketOrders(): "
                                "invalid side provided: %c", (char)side);
            throw MamdaOrderBookException(msg);
        } 
    }

    MamdaOrderBookPriceLevel*
    MamdaOrderBook::MamdaOrderBookImpl::getOrCreateMarketOrdersSide (
            MamdaOrderBookPriceLevel::Side side) 
    {
        if (MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID == side)
        {
            if (!mBidMarketOrders)
            {
                mBidMarketOrders = new MamdaOrderBookPriceLevel (
                        0.0,
                        MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
                        
                mBidMarketOrders->setOrderBook (&mBook);
                mBidMarketOrders->setOrderType (
                                MamdaOrderBookPriceLevel::MAMDA_BOOK_LEVEL_MARKET);
            }

            return mBidMarketOrders;
        }
        else if (MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_ASK == side)
        {
            if (!mAskMarketOrders)
            {
                mAskMarketOrders = new MamdaOrderBookPriceLevel (
                        0.0,
                        MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_ASK);
                        
                mAskMarketOrders->setOrderBook (&mBook);
                mAskMarketOrders->setOrderType (
                                MamdaOrderBookPriceLevel::MAMDA_BOOK_LEVEL_MARKET);
            }
            return mAskMarketOrders;
        }
        else
        {
            char msg[1000];
            snprintf (msg, 1000,"MamdaOrderBook::getOrCreateMarketOrdersSide(): "
                                "invalid side provided: %c", (char)side);
            throw MamdaOrderBookException(msg);
        } 
    }

    bool MamdaOrderBook::MamdaOrderBookImpl::operator== (
        const MamdaOrderBookImpl& rhs) const
    {
        return (mSymbol == rhs.mSymbol) &&
            (mPartId == rhs.mPartId) &&
            (mBidLevels == rhs.mBidLevels) &&
            (mAskLevels == rhs.mAskLevels) &&
            (mBidMarketOrders == rhs.mBidMarketOrders) &&
            (mAskMarketOrders == rhs.mAskMarketOrders);
    }

    bool MamdaOrderBook::MamdaOrderBookImpl::operator!= (
        const MamdaOrderBookImpl&  rhs) const
    {
        return ! operator== (rhs);
    }

    void MamdaOrderBook::MamdaOrderBookImpl::addEntry (
        PlMap&                          bookSide,
        MamdaOrderBookEntry*            entry,
        double                          price,
        MamdaOrderBookPriceLevel::Side  side,
        const MamaDateTime&             eventTime,
        MamdaOrderBookBasicDelta*       delta)
    {
        MamdaOrderBookPriceLevel::Action  plAction;
        MamdaOrderBookPriceLevel* level = findOrCreateLevel (
                                                bookSide, price, side, plAction);
        mama_quantity_t plSizeDelta = 0.0;
        if (!mCheckVisibility || entry->isVisible())
        {
            plSizeDelta = entry->getSize();
            level->setTime (eventTime);
        }
        level->setOrderBook(&mBook);

        //add entry to level - this will add delta to publishing list 
        //if publishing enabled
        level->addEntry (entry);

        if (delta)
        {
            delta->set (entry, level, plSizeDelta, plAction,
                        MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD);
        }
    }

    void MamdaOrderBook::MamdaOrderBookImpl::addEntry (
        PlMap&                          bookSide,
        MamdaOrderBookEntry*            entry,
        MamaPrice&                      price,
        MamdaOrderBookPriceLevel::Side  side,
        const MamaDateTime&             eventTime,
        MamdaOrderBookBasicDelta*       delta)
    {
        MamdaOrderBookPriceLevel::Action  plAction;
        MamdaOrderBookPriceLevel* level = findOrCreateLevel (
                                                bookSide, price, side, plAction);
        mama_quantity_t plSizeDelta = 0.0;
        if (!mCheckVisibility || entry->isVisible())
        {
            plSizeDelta = entry->getSize();
            level->setTime (eventTime);
        }
        //add entry to level - this will add delta to publishing list
        level->addEntry (entry);

        if (delta)
        {
            delta->set (entry, level, plSizeDelta, plAction,
                        MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD);
        }
    }

    void MamdaOrderBook::MamdaOrderBookImpl::updateEntry (
        MamdaOrderBookEntry*            entry,
        mama_quantity_t                 size,
        const MamaDateTime&             eventTime,
        MamdaOrderBookBasicDelta*       delta)
    {
        MamdaOrderBookPriceLevel* level = entry->getPriceLevel();
        if (!level)
            throw MamdaOrderBookInvalidEntry (entry,
                                              "MamdaOrderBook::updateEntry()");
        mama_quantity_t plSizeDelta = 0.0;
        if (size != entry->getSize())
        {
            // For some reason, we can get updates that do not change the
            // size and so we also don't want to change the time.
            if (!mCheckVisibility || entry->isVisible())
            {
                plSizeDelta = size - entry->getSize();
                level->setSize (level->getSize() + plSizeDelta);
                level->setTime (eventTime);
            }
            entry->setSize (size);
            entry->setTime (eventTime);
        }

        if (delta)
        {
            delta->set (entry, level, plSizeDelta,
                        MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_UPDATE,
                        MamdaOrderBookEntry::MAMDA_BOOK_ACTION_UPDATE);
        }

        if (mGenerateDeltas)
        {
            addDelta(entry, level, plSizeDelta, 
                     MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_UPDATE, 
                     MamdaOrderBookEntry::MAMDA_BOOK_ACTION_UPDATE);
        }  
    }

    void MamdaOrderBook::MamdaOrderBookImpl::deleteEntry (
        MamdaOrderBookEntry*            entry,
        const MamaDateTime&             eventTime,
        MamdaOrderBookBasicDelta*       delta)
    {
        MamdaOrderBookPriceLevel* level = entry->getPriceLevel();
        if (!level)
            throw MamdaOrderBookInvalidEntry (entry,
                                              "MamdaOrderBook::deleteEntry()");
        mama_quantity_t plSizeDelta = 0.0;
        if (!mCheckVisibility || entry->isVisible())
        {
            plSizeDelta = -entry->getSize();
            level->setTime (eventTime);
        }
        level->removeEntry (entry);

        if (level->empty())
        {
            //this will detach level and add publishing delta to publish delta list
            detach (level);
        }

        if (delta)
        {
            MamdaOrderBookPriceLevel::Action plAction =
                MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_UPDATE;
            if (mama_isQuantityNone (level->getSize()))
            {
                plAction = MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_DELETE;
            }
            
            delta->set (entry, level, plSizeDelta, plAction,
                        MamdaOrderBookEntry::MAMDA_BOOK_ACTION_DELETE);
        
            //add delta to list if level not already deleted
            if ((!level->empty()) && (mGenerateDeltas))
            {
                addDelta(entry, level, plSizeDelta, plAction, 
                         MamdaOrderBookEntry::MAMDA_BOOK_ACTION_DELETE);
            } 
        }
    }

    void MamdaOrderBook::MamdaOrderBookImpl::addEntriesFromBook (
        const MamdaOrderBookImpl&       bookImpl,
        MamdaOrderBookEntryFilter*      filter,
        MamdaOrderBookBasicDeltaList*   delta)
    {
        MamdaOrderBookPriceLevel::Action  ignored;

        PlMap::const_iterator  bidEnd  = bookImpl.mBidLevels.end();
        PlMap::const_iterator  bidIter = bookImpl.mBidLevels.begin();

        for (; bidIter != bidEnd; ++ bidIter)
        {
            const MamdaOrderBookPriceLevel* bookLevel = bidIter->second;
            MamdaOrderBookPriceLevel* level = findOrCreateLevel (
                mBidLevels, bookLevel->getPrice(), bookLevel->getSide(), ignored);
            level->addEntriesFromLevel (bookLevel, filter, delta);
        }

        PlMap::const_iterator  askEnd  = bookImpl.mAskLevels.end();
        PlMap::const_iterator  askIter = bookImpl.mAskLevels.begin();

        for (; askIter != askEnd; ++ askIter)
        {
            const MamdaOrderBookPriceLevel* bookLevel = askIter->second;
            MamdaOrderBookPriceLevel* level = findOrCreateLevel (
                mAskLevels, bookLevel->getPrice(), bookLevel->getSide(), ignored);
            level->addEntriesFromLevel (bookLevel, filter, delta);
        }
    }

    void MamdaOrderBook::MamdaOrderBookImpl::addDelta (
        MamdaOrderBookEntry*              entry,
        MamdaOrderBookPriceLevel*         level,
        mama_quantity_t                   plDeltaSize,
        MamdaOrderBookPriceLevel::Action  plAction,
        MamdaOrderBookEntry::Action       entryAction)
    {
        ++mCurrentDeltaCount;
        if (1 == mCurrentDeltaCount)
        {        
            /* This is number one, so save the "simple" delta. */
            mPublishSimpleDelta->set (
                entry, level, plDeltaSize, plAction, entryAction);
        }
        else if (2 == mCurrentDeltaCount)
        {
            /* This is number two, so copy the saved "simple" delta to the
             * "complex" delta and add the current one. */
            mPublishComplexDelta->clear();
            mPublishComplexDelta->add (*mPublishSimpleDelta);
            mPublishComplexDelta->add (
                entry, level, plDeltaSize, plAction, entryAction);
        }
        else
        {
            /* This is number greater than two, so add the current delta. */
            mPublishComplexDelta->add (
                entry, level, plDeltaSize, plAction, entryAction);
        }
    }

    void MamdaOrderBook::MamdaOrderBookImpl::addPriceLevelsFromBookAsEntries (
        const MamdaOrderBookImpl&       bookImpl,
        const char*                     sourceId,
        MamdaOrderBookBasicDeltaList*   delta)
    {
        PlMap::const_iterator  bidEnd  = bookImpl.mBidLevels.end();
        PlMap::const_iterator  bidIter = bookImpl.mBidLevels.begin();

        for (; bidIter != bidEnd; ++ bidIter)
        {
            const MamdaOrderBookPriceLevel* bookLevel = bidIter->second;
            MamdaOrderBookEntry*  entry =
                new MamdaOrderBookEntry (sourceId, bookLevel->getSize(), 
                                       MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                                       bookLevel->getTime(),
                                       bookImpl.mSourceDeriv);
            if (delta)
            {
                MamdaOrderBookBasicDelta  basicDelta;
                addEntry (mBidLevels, entry, bookLevel->getPrice(),
                          bookLevel->getSide(), bookLevel->getTime(),
                          &basicDelta);
                delta->add (basicDelta);
            }
            else
            {
                addEntry (mBidLevels, entry, bookLevel->getPrice(),
                          bookLevel->getSide(), bookLevel->getTime(), NULL);
            }
        }

        PlMap::const_iterator  askEnd  = bookImpl.mAskLevels.end();
        PlMap::const_iterator  askIter = bookImpl.mAskLevels.begin();

        for (; askIter != askEnd; ++ askIter)
        {
            const MamdaOrderBookPriceLevel* bookLevel = askIter->second;
            MamdaOrderBookEntry*  entry =
                new MamdaOrderBookEntry (sourceId, bookLevel->getSize(), 
                                       MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                                       bookLevel->getTime(),
                                       bookImpl.mSourceDeriv);
            if (delta)
            {
                MamdaOrderBookBasicDelta  basicDelta;
                addEntry (mAskLevels, entry, bookLevel->getPrice(),
                          bookLevel->getSide(), bookLevel->getTime(),
                          &basicDelta);
                delta->add (basicDelta);
            }
            else
            {
                addEntry (mAskLevels, entry, bookLevel->getPrice(),
                          bookLevel->getSide(), bookLevel->getTime(), NULL);
            }
        }
    }

    void MamdaOrderBook::MamdaOrderBookImpl::deleteEntriesFromSource (
        const MamaSource*               source,
        MamdaOrderBookBasicDeltaList*   delta)
    {
        PlMap::iterator  bidEnd  = mBidLevels.end();
        PlMap::iterator  bidIter = mBidLevels.begin();

        for (; bidIter != bidEnd; ++ bidIter)
        {
            bidIter->second->deleteEntriesFromSource (source, delta);
        }

        PlMap::iterator  askEnd  = mAskLevels.end();
        PlMap::iterator  askIter = mAskLevels.begin();

        for (; askIter != askEnd; ++ askIter)
        {
            askIter->second->deleteEntriesFromSource (source, delta);
        }
    }

    bool MamdaOrderBook::MamdaOrderBookImpl::reevaluate ()
    {
        if (!mCheckVisibility)
            return false;

        bool changed = false;

        PlMap::iterator  bidEnd  = mBidLevels.end();
        PlMap::iterator  bidIter = mBidLevels.begin();

        for (; bidIter != bidEnd; ++ bidIter)
        {
            changed = bidIter->second->reevaluate() || changed;
        }

        PlMap::iterator  askEnd  = mAskLevels.end();
        PlMap::iterator  askIter = mAskLevels.begin();

        for (; askIter != askEnd; ++ askIter)
        {
            changed = askIter->second->reevaluate() || changed;
        }

        if (mBidMarketOrders)
            changed = mBidMarketOrders->reevaluate() || changed;
        
        if (mAskMarketOrders)
            changed = mAskMarketOrders->reevaluate() || changed;
        
        return changed;
    }

    void MamdaOrderBook::MamdaOrderBookImpl::assertEqual (
        const MamdaOrderBookImpl& rhs) const
    {
        if ((mSymbol != "" ) && (mSymbol != rhs.mSymbol))
            throw MamdaOrderBookException("different symbols");

        if ((mPartId != "" ) && (mPartId != rhs.mPartId))
                throw MamdaOrderBookException("different participant ids");

        assertEqual (mBidLevels, rhs.mBidLevels);
        assertEqual (mAskLevels, rhs.mAskLevels);
        assertEqual (mBidMarketOrders, rhs.mBidMarketOrders);
        assertEqual (mAskMarketOrders, rhs.mAskMarketOrders);
    }

    void MamdaOrderBook::MamdaOrderBookImpl::assertEqual (
        const MamdaOrderBookPriceLevel* lhsLevel,
        const MamdaOrderBookPriceLevel* rhsLevel) const
    {
        if (!lhsLevel)
        {
            if (!rhsLevel) return;
            
            char errorMsg[256];
            snprintf (errorMsg, 256,
                      "market order levels not equal ([Empty]/%ld)",
                      (long)rhsLevel->getSize());
            throw MamdaOrderBookException(errorMsg);
        }

        if (!rhsLevel)
        {
            char errorMsg[256];
            snprintf (errorMsg, 256,
                      "market order levels not equal ([%ld/[Empty])",
                      (long)lhsLevel->getSize());
            throw MamdaOrderBookException(errorMsg);
        }
        lhsLevel->assertEqual (*rhsLevel);
    }
     
    void MamdaOrderBook::MamdaOrderBookImpl::assertEqual (
        const PlMap&  lhsLevels,
        const PlMap&  rhsLevels) const
    {
        if (lhsLevels.size() != rhsLevels.size())
        {
            char errorMsg[256];
            snprintf (errorMsg, 256,
                      "number of price levels do not add up (%ld/%ld)",
                      (long)lhsLevels.size(),
                      (long)rhsLevels.size());
            throw MamdaOrderBookException(errorMsg);
        }

        PlMap::const_iterator lhsIter = lhsLevels.begin();
        PlMap::const_iterator rhsIter = rhsLevels.begin();

        size_t size = lhsLevels.size();
        for (size_t i = 0; i < size; ++i, ++lhsIter, ++rhsIter)
        {
            const MamdaOrderBookPriceLevel* lhsLevel = lhsIter->second;
            const MamdaOrderBookPriceLevel* rhsLevel = rhsIter->second;
            lhsLevel->assertEqual (*rhsLevel);
        }
    }

    MamdaOrderBook::bidIterator::bidIterator ()
        : mImpl (*new bidIteratorImpl)
    {
    }

    MamdaOrderBook::bidIterator::bidIterator (const bidIterator& copy)
        : mImpl (*new bidIteratorImpl(copy.mImpl.mPlMapIter,
                                      copy.mImpl.mPlMapEnd,
                                      copy.mImpl.mPlMapBegin))
    {
    }

    MamdaOrderBook::bidIterator::bidIterator (const bidIteratorImpl& copy)
        : mImpl (*new bidIteratorImpl(copy))
    {
    }

    MamdaOrderBook::bidIterator::~bidIterator ()
    {
        delete &mImpl;
    }

    MamdaOrderBook::bidIterator& MamdaOrderBook::bidIterator::operator= (const bidIterator& rhs)
    {
        if (&rhs != this)
        {
            mImpl.mPlMapIter = rhs.mImpl.mPlMapIter;
            mImpl.mPlMapEnd  = rhs.mImpl.mPlMapEnd;
        }
        return *this;
    }

    const MamdaOrderBook::bidIterator& MamdaOrderBook::bidIterator::operator++ () const
    {
        if (mImpl.mPlMapIter != mImpl.mPlMapEnd)
        {
            do
            {
                ++mImpl.mPlMapIter;
            }
            while ((mImpl.mPlMapIter != mImpl.mPlMapEnd) &&
                   ((mImpl.mPlMapIter->second->getSize() == 0)));
        }
        return *this;
    }

    MamdaOrderBook::bidIterator& MamdaOrderBook::bidIterator::operator++ ()
    {
        if (mImpl.mPlMapIter != mImpl.mPlMapEnd)
        {
            do
            {
                ++mImpl.mPlMapIter;
            }
            while ((mImpl.mPlMapIter != mImpl.mPlMapEnd) &&
                   ((mImpl.mPlMapIter->second->getSize() == 0)));
        }
        return *this;
    }

    const MamdaOrderBook::bidIterator& MamdaOrderBook::bidIterator::operator-- () const
    {
        if (mImpl.mPlMapIter != mImpl.mPlMapBegin)
        {
            do
            {
                --mImpl.mPlMapIter;
            }
            while ((mImpl.mPlMapIter != mImpl.mPlMapBegin) &&
                   ((mImpl.mPlMapIter->second->getSize() == 0)));
        }
        return *this;
    }

    MamdaOrderBook::bidIterator& MamdaOrderBook::bidIterator::operator-- ()
    {
        if (mImpl.mPlMapIter != mImpl.mPlMapBegin)
        {
            do
            {
                --mImpl.mPlMapIter;
            }
            while ((mImpl.mPlMapIter != mImpl.mPlMapBegin) &&
                   ((mImpl.mPlMapIter->second->getSize() == 0)));
        }
        return *this;
    }

    bool MamdaOrderBook::bidIterator::operator== (const bidIterator& rhs) const
    {
        return ((&rhs == this) || (mImpl.mPlMapIter == rhs.mImpl.mPlMapIter));
    }

    bool MamdaOrderBook::bidIterator::operator!= (const bidIterator& rhs) const
    {
        return !operator==(rhs);
    }

    MamdaOrderBookPriceLevel* MamdaOrderBook::bidIterator::operator*()
    {
        return mImpl.mPlMapIter->second;
    }

    const MamdaOrderBookPriceLevel* MamdaOrderBook::bidIterator::operator*() const
    {
        return mImpl.mPlMapIter->second;
    }

    MamdaOrderBook::askIterator::askIterator ()
        : mImpl (*new askIteratorImpl)
    {
    }

    MamdaOrderBook::askIterator::askIterator (const askIterator& copy)
        : mImpl (*new askIteratorImpl(copy.mImpl.mPlMapIter,
                                      copy.mImpl.mPlMapEnd,
                                      copy.mImpl.mPlMapBegin))
    {
    }

    MamdaOrderBook::askIterator::askIterator (const askIteratorImpl& copy)
        : mImpl (*new askIteratorImpl(copy))
    {
    }

    MamdaOrderBook::askIterator::~askIterator ()
    {
        delete &mImpl;
    }

    MamdaOrderBook::askIterator& MamdaOrderBook::askIterator::operator= (const askIterator& rhs)
    {
        if (&rhs != this)
        {
            mImpl.mPlMapIter = rhs.mImpl.mPlMapIter;
            mImpl.mPlMapEnd  = rhs.mImpl.mPlMapEnd;
        }
        return *this;
    }

    const MamdaOrderBook::askIterator& MamdaOrderBook::askIterator::operator++ () const
    {
        if (mImpl.mPlMapIter != mImpl.mPlMapEnd)
        {
            do
            {
                ++mImpl.mPlMapIter;
            }
            while ((mImpl.mPlMapIter != mImpl.mPlMapEnd) &&
                   ((mImpl.mPlMapIter->second->getSize() == 0)));
        }
        return *this;
    }

    MamdaOrderBook::askIterator& MamdaOrderBook::askIterator::operator++ ()
    {
        if (mImpl.mPlMapIter != mImpl.mPlMapEnd)
        {
            do
            {
                ++mImpl.mPlMapIter;
            }
            while ((mImpl.mPlMapIter != mImpl.mPlMapEnd) &&
                   ((mImpl.mPlMapIter->second->getSize() == 0)));
        }
        return *this;
    }

    const MamdaOrderBook::askIterator& MamdaOrderBook::askIterator::operator-- () const
    {
        if (mImpl.mPlMapIter != mImpl.mPlMapBegin)
        {
            do
            {
                --mImpl.mPlMapIter;
            }
            while ((mImpl.mPlMapIter != mImpl.mPlMapBegin) &&
                   ((mImpl.mPlMapIter->second->getSize() == 0)));
        }
        return *this;
    }

    MamdaOrderBook::askIterator& MamdaOrderBook::askIterator::operator-- ()
    {
        if (mImpl.mPlMapIter != mImpl.mPlMapBegin)
        {
            do
            {
                --mImpl.mPlMapIter;
            }
            while ((mImpl.mPlMapIter != mImpl.mPlMapBegin) &&
                   ((mImpl.mPlMapIter->second->getSize() == 0)));
        }
        return *this;
    }

    bool MamdaOrderBook::askIterator::operator== (const askIterator& rhs) const
    {
        return ((&rhs == this) || (mImpl.mPlMapIter == rhs.mImpl.mPlMapIter));
    }

    bool MamdaOrderBook::askIterator::operator!= (const askIterator& rhs) const
    {
        return !operator==(rhs);
    }

    MamdaOrderBookPriceLevel* MamdaOrderBook::askIterator::operator*()
    {
        return mImpl.mPlMapIter->second;
    }

    const MamdaOrderBookPriceLevel* MamdaOrderBook::askIterator::operator*() const
    {
        return mImpl.mPlMapIter->second;
    }

    MamdaOrderBook::bidEntryIterator::bidEntryIterator (
        const bidEntryIteratorImpl& copy)
        : mImpl (*new bidEntryIteratorImpl(copy))
    {
    }

    MamdaOrderBook::bidEntryIterator::bidEntryIterator (
        const bidEntryIterator& copy)
        : mImpl (*new bidEntryIteratorImpl(copy.mImpl))
    {
    }

    MamdaOrderBook::bidEntryIterator::~bidEntryIterator ()
    {
        delete &mImpl;
    }

    MamdaOrderBook::bidEntryIterator& MamdaOrderBook::bidEntryIterator::operator= (const bidEntryIterator& rhs)
    {
        if (&rhs != this)
        {
            mImpl.mBidIter   = rhs.mImpl.mBidIter;
            mImpl.mBidIter   = rhs.mImpl.mBidIter;
            mImpl.mEntryIter = rhs.mImpl.mEntryIter;
        }
        return *this;
    }

    const MamdaOrderBook::bidEntryIterator&
    MamdaOrderBook::bidEntryIterator::operator++ () const
    {
        if (++mImpl.mEntryIter == (*mImpl.mBidIter)->end ()) 
        {
            // Find the next level with entries.
            do
            {
                ++mImpl.mBidIter;
            }
            while ((mImpl.mBidIter != mImpl.mBidEnd) && 
		     ((*mImpl.mBidIter)->begin() == (*mImpl.mBidIter)->end()));

            if (mImpl.mBidIter != mImpl.mBidEnd) // Found one.
                mImpl.mEntryIter = (*mImpl.mBidIter)->begin();
        }
        return *this;
    }

    MamdaOrderBook::bidEntryIterator&
    MamdaOrderBook::bidEntryIterator::operator++ ()
    {
        if (++mImpl.mEntryIter == (*mImpl.mBidIter)->end ()) 
        {
            do
            {
                ++mImpl.mBidIter;
            }
            while ((mImpl.mBidIter != mImpl.mBidEnd) && 
     		((*mImpl.mBidIter)->begin() == (*mImpl.mBidIter)->end()));

            if (mImpl.mBidIter != mImpl.mBidEnd) // Found one.
                mImpl.mEntryIter = (*mImpl.mBidIter)->begin();
        }
        return *this;
    }

    bool MamdaOrderBook::bidEntryIterator::operator== (const bidEntryIterator& rhs) const
    {
        // End is a special case
        return (&rhs == this) || (mImpl.mEntryIter == rhs.mImpl.mEntryIter) ||
            ((rhs.mImpl.mBidIter == mImpl.mBidEnd && mImpl.mBidIter == mImpl.mBidEnd));
    }

    bool MamdaOrderBook::bidEntryIterator::operator!= (const bidEntryIterator& rhs) const
    {
        return !operator==(rhs);
    }

    MamdaOrderBookEntry* MamdaOrderBook::bidEntryIterator::operator*()
    {
        return *mImpl.mEntryIter;
    }

    const MamdaOrderBookEntry* MamdaOrderBook::bidEntryIterator::operator*() const
    {
        return *mImpl.mEntryIter;
    }

    MamdaOrderBook::askEntryIterator::askEntryIterator (
        const askEntryIteratorImpl& copy)
        : mImpl (*new askEntryIteratorImpl(copy))
    {
    }

    MamdaOrderBook::askEntryIterator::askEntryIterator (
        const askEntryIterator& copy)
        : mImpl (*new askEntryIteratorImpl(copy.mImpl))
    {
    }

    MamdaOrderBook::askEntryIterator::~askEntryIterator ()
    {
        delete &mImpl;
    }

    MamdaOrderBook::askEntryIterator& MamdaOrderBook::askEntryIterator::operator= (const askEntryIterator& rhs)
    {
        if (&rhs != this)
        {
            mImpl.mAskIter   = rhs.mImpl.mAskIter;
            mImpl.mAskIter   = rhs.mImpl.mAskIter;
            mImpl.mEntryIter = rhs.mImpl.mEntryIter;
        }
        return *this;
    }

    const MamdaOrderBook::askEntryIterator&
    MamdaOrderBook::askEntryIterator::operator++ () const
    {
        if (++mImpl.mEntryIter == (*mImpl.mAskIter)->end ()) 
        {
	     do
            {
                ++mImpl.mAskIter;
            }
            while ((mImpl.mAskIter != mImpl.mAskEnd) &&
		     ((*mImpl.mAskIter)->begin() == (*mImpl.mAskIter)->end())); 

            if (mImpl.mAskIter != mImpl.mAskEnd) // Found one.
                mImpl.mEntryIter = (*mImpl.mAskIter)->begin();
        }
        return *this;
    }

    MamdaOrderBook::askEntryIterator&
    MamdaOrderBook::askEntryIterator::operator++ ()
    {
        if (++mImpl.mEntryIter == (*mImpl.mAskIter)->end ()) 
        {
            // Find the next level with entries.
            do
            {
                ++mImpl.mAskIter;
            }
            while ((mImpl.mAskIter != mImpl.mAskEnd) && 
		     ((*mImpl.mAskIter)->begin() == (*mImpl.mAskIter)->end()));

            if (mImpl.mAskIter != mImpl.mAskEnd) // Found one.
                mImpl.mEntryIter = (*mImpl.mAskIter)->begin();
        }
        return *this;
    }

    bool MamdaOrderBook::askEntryIterator::operator== (const askEntryIterator& rhs) const
    {
        // End is a special case
        return (&rhs == this) || (mImpl.mEntryIter == rhs.mImpl.mEntryIter) ||
            ((rhs.mImpl.mAskIter == mImpl.mAskEnd && mImpl.mAskIter == mImpl.mAskEnd));
    }

    bool MamdaOrderBook::askEntryIterator::operator!= (const askEntryIterator& rhs) const
    {
        return !operator==(rhs);
    }

    MamdaOrderBookEntry* MamdaOrderBook::askEntryIterator::operator*()
    {
        return *mImpl.mEntryIter;
    }

    const MamdaOrderBookEntry* MamdaOrderBook::askEntryIterator::operator*() const
    {
        return *mImpl.mEntryIter;
    }

    void MamdaOrderBook::setStrictChecking (bool strict)
    {
        sStrictChecking = strict;
        MamdaOrderBookPriceLevel::setStrictChecking (strict);
        MamdaOrderBookEntry::setStrictChecking (strict);
    }

    void  MamdaOrderBook::cleanupDetached ()
    {
        mImpl.cleanupDetached();
    }

    void  MamdaOrderBook::MamdaOrderBookImpl::cleanupDetached ()
    {
        if (!mDetachedLevels.empty())
        {
            PlList::iterator itr = mDetachedLevels.begin();
            PlList::iterator end = mDetachedLevels.end();
            for (; itr != end; itr++)
            {
                MamdaOrderBookPriceLevel* level = *itr;
                delete level;
            }
            mDetachedLevels.clear();
        }
        if (!mDetachedEntries.empty())
        {
            EntryList::iterator itr = mDetachedEntries.begin();
            EntryList::iterator end = mDetachedEntries.end();
            for (; itr != end; itr++)
            {
                MamdaOrderBookEntry* entry = *itr;
                delete entry;
            }
            mDetachedEntries.clear();
        }
    }

    void  MamdaOrderBook::detach (
            MamdaOrderBookEntry* entry)
    {
        mImpl.detach(entry);
    }

    void  MamdaOrderBook::detach (
            MamdaOrderBookPriceLevel* level)
    {
        mImpl.detach (level);
    }

    void  MamdaOrderBook::MamdaOrderBookImpl::detach (
            MamdaOrderBookEntry* entry)
    {
        mDetachedEntries.push_back (entry);
        
        if (mGenerateDeltas)
        {
            MamdaOrderBookPriceLevel::Action plAction;
            if (0 == entry->getPriceLevel()->getNumEntriesTotal()) 
            {
                plAction = MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_DELETE;
            }
            else
            {
                plAction = MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_UPDATE;
            }
                
            addDelta(entry, entry->getPriceLevel(), 
                     entry->getPriceLevel()->getSizeChange(), 
                     plAction, 
                     MamdaOrderBookEntry::MAMDA_BOOK_ACTION_DELETE);
        } 
    }


    void  MamdaOrderBook::MamdaOrderBookImpl::detach (
            MamdaOrderBookPriceLevel* level)
    {
        if (level->getOrderType() == MamdaOrderBookPriceLevel::MAMDA_BOOK_LEVEL_MARKET)
        {
            switch (level->getSide())
            {
                case MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID:
                {
                    level  = NULL;
                    mBidMarketOrders = NULL;
                    return;
                }

                case MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_ASK:
                {
                    level  = NULL;
                    mAskMarketOrders = NULL;
                    return;
                }

                case MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_UNKNOWN:
                {
                    char msg[1000];
                    const char* sourceId = "none";
                    if (mSourceDeriv)
                        sourceId = mSourceDeriv->getBaseSource()->getDisplayId();
                        snprintf (msg, 1000,"MamdaOrderBook::detach(%s:%s) attempted to detach"
                                  " MARKET order price level %g with an unknown side",
                                  sourceId, mSymbol.c_str(), level->getPrice());

                    return;
                }
            }
        }

        // Remove the level from the relevant side
        switch (level->getSide())
        {
            case MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID:
            {
                PlMap::iterator found = mBidLevels.find (level->getPrice());
                if (found == mBidLevels.end())
                {
                    char msg[1000];
                    const char* sourceId = "none";
                    if (mSourceDeriv)
                        sourceId = mSourceDeriv->getBaseSource()->getDisplayId();
                    snprintf (msg, 1000,"MamdaOrderBook::detach(%s:%s) attempted to "
                            "detach price level %g (bid) which does not exist in "
                            "the book!", sourceId, mSymbol.c_str(), level->getPrice());

                    return;
                }

                mBidLevels.erase (found);
                break;
            }
            case MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_ASK:
            {
                PlMap::iterator found = mAskLevels.find (level->getPrice());
                if (found == mAskLevels.end())
                {
                    char msg[1000];
                    const char* sourceId = "none";
                    if (mSourceDeriv)
                        sourceId = mSourceDeriv->getBaseSource()->getDisplayId();
                    snprintf (msg, 1000,"MamdaOrderBook::detach(%s:%s) attempted to "
                            "detach price level %g (ask) which does not exist in "
                            "the book!", sourceId, mSymbol.c_str(), level->getPrice());
                    //throw MamdaOrderBookException(msg);

                    return;
                }

                mAskLevels.erase (found);
                break;
            }
            case MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_UNKNOWN:
            {
                char msg[1000];
                const char* sourceId = "none";
                if (mSourceDeriv)
                    sourceId = mSourceDeriv->getBaseSource()->getDisplayId();
                snprintf (msg, 1000,"MamdaOrderBook::detach(%s:%s) attempted to detach"
                        " price level %g with an unknown side",
                        sourceId, mSymbol.c_str(), level->getPrice());
                //throw MamdaOrderBookException(msg);

                return;
            }
        }
        mDetachedLevels.push_back(level);
    }

}
