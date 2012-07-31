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

#include <mamda/MamdaOrderBookPriceLevel.h>
#include  <wombat/wincompat.h>
#include <mamda/MamdaOrderBookExceptions.h>
#include <mamda/MamdaOrderBook.h>
#include <mamda/MamdaOrderBookEntry.h>
#include <mamda/MamdaOrderBookBasicDeltaList.h>
#include <mama/types.h>
#include <string>
#include <string.h>
#include <list>
#include <stdio.h>
#include <assert.h>

using std::strcpy;
using std::strlen;
using std::list;

static bool sStrictChecking = false;

namespace Wombat
{

    typedef list<MamdaOrderBookEntry*>  EntryList;

    struct MamdaOrderBookPriceLevel::MamdaOrderBookPriceLevelImpl
    {
        MamdaOrderBookPriceLevelImpl (MamdaOrderBookPriceLevel& level);
        MamdaOrderBookPriceLevelImpl (MamdaOrderBookPriceLevel& level,
                                      const MamdaOrderBookPriceLevelImpl& copy);
        MamdaOrderBookPriceLevelImpl (MamdaOrderBookPriceLevel& level,
                                      MamaPrice& price, Side side);
        MamdaOrderBookPriceLevelImpl (MamdaOrderBookPriceLevel& level,
                                      double price, Side side);
        ~MamdaOrderBookPriceLevelImpl ();

        bool operator==      (const MamdaOrderBookPriceLevelImpl&  rhs) const;
        void assertEqual     (const MamdaOrderBookPriceLevelImpl&  rhs) const;
        void clear           ();
        void copy            (const MamdaOrderBookPriceLevelImpl&  copy);
        void copyLevelOnly   (const MamdaOrderBookPriceLevelImpl&  copy);
        void addEntry        (MamdaOrderBookEntry*        entry);
        void updateEntry     (const MamdaOrderBookEntry&  entry);
        void removeEntryById (const MamdaOrderBookEntry&  entry);
        void removeEntry     (const MamdaOrderBookEntry*  entry);
        void checkNotExist   (const MamdaOrderBookEntry&  entry);
        void eraseEntryByIterator (
            EntryList*                           entryList,
            EntryList::iterator                  erasableIter,
            MamdaOrderBookEntry*                 entry);
        void addEntriesFromLevel (
            const MamdaOrderBookPriceLevelImpl&  level,
            MamdaOrderBookEntryFilter*           filter,
            MamdaOrderBookBasicDeltaList*        delta);
        void deleteEntriesFromSource (
            const MamaSource*                    source,
            MamdaOrderBookBasicDeltaList*        delta);
        bool reevaluate      ();
        void markAllDeleted  ();
        void throwError      (const char*  context) const;
        MamdaOrderBookEntry* findEntry          (const char*  id) const;
        MamdaOrderBookEntry* findOrCreateEntry  (const char*  id,
                                                 bool&        newEntry);
        MamdaOrderBookEntry* getEntryAtPosition (mama_u32_t   pos) const;

        void clearEntries (EntryList& entries);

        MamdaOrderBookPriceLevel&  mLevel;
        MamaPrice        mPrice;
        mama_quantity_t  mSize;
        mama_quantity_t  mSizeChange;
        mama_u32_t       mNumEntries;
        mama_u32_t       mNumEntriesTotal;
        Side             mSide;
        Action           mAction;
        MamaDateTime     mTime;
        EntryList*       mEntries;
        MamdaOrderBook*  mBook;
        OrderType        mOrderType;
        void*            mClosure;
    };

    MamdaOrderBookPriceLevel::MamdaOrderBookPriceLevel()
        : mImpl (*new MamdaOrderBookPriceLevelImpl(*this))
    {
    }

    MamdaOrderBookPriceLevel::MamdaOrderBookPriceLevel(
        const MamdaOrderBookPriceLevel&  copy)
        : mImpl (*new MamdaOrderBookPriceLevelImpl(*this,copy.mImpl))
    {
    }

    MamdaOrderBookPriceLevel::MamdaOrderBookPriceLevel(
        double   price,
        Side     side)
        : mImpl (*new MamdaOrderBookPriceLevelImpl(*this,price,side))
    {
    }

    MamdaOrderBookPriceLevel::MamdaOrderBookPriceLevel(
        MamaPrice&   price,
        Side     side)
        : mImpl (*new MamdaOrderBookPriceLevelImpl(*this,price,side))
    {
    }

    MamdaOrderBookPriceLevel::~MamdaOrderBookPriceLevel()
    {
        delete &mImpl;
    }

    MamdaOrderBookPriceLevel& MamdaOrderBookPriceLevel::operator= (
        const MamdaOrderBookPriceLevel& rhs)
    {
        if (this != &rhs)
        {
            mImpl.copy(rhs.mImpl);
        }
        return *this;
    }

    void MamdaOrderBookPriceLevel::clear ()
    {
        mImpl.clear();
    }

    void MamdaOrderBookPriceLevel::setPrice (double  price)
    {
        mImpl.mPrice.setValue(price);
    }

    void MamdaOrderBookPriceLevel::setPrice (MamaPrice&  price)
    {
        mImpl.mPrice = price;
    }

    void MamdaOrderBookPriceLevel::setSize (mama_quantity_t  size)
    {
        mImpl.mSize = size;
        if (mama_isQuantityNone (mImpl.mSize)) mImpl.mSize = 0.0;
    }

    void MamdaOrderBookPriceLevel::setSizeChange (mama_quantity_t  size)
    {
        mImpl.mSizeChange = size;
        if (mama_isQuantityNone (mImpl.mSizeChange)) mImpl.mSizeChange = 0.0;
    }

    void MamdaOrderBookPriceLevel::setNumEntries (mama_u32_t  numEntries)
    {
        mImpl.mNumEntries      = numEntries;
        mImpl.mNumEntriesTotal = (mImpl.mEntries ?  mImpl.mEntries->size() : 
                                                        numEntries);
    }

    void MamdaOrderBookPriceLevel::setSide (Side  side)
    {
        mImpl.mSide = side;
    }

    void MamdaOrderBookPriceLevel::setAction (Action  action)
    {
        mImpl.mAction = action;
    }

    void MamdaOrderBookPriceLevel::setTime (const MamaDateTime&  time)
    {
        mImpl.mTime = time;
    }

    void MamdaOrderBookPriceLevel::setOrderType (OrderType orderType)
    {
        mImpl.mOrderType = orderType;
    }

    MamdaOrderBookPriceLevel::OrderType MamdaOrderBookPriceLevel::getOrderType () const
    {
        return mImpl.mOrderType;
    }

    void MamdaOrderBookPriceLevel::setDetails (
        const MamdaOrderBookPriceLevel& rhs)
    {
        mImpl.mSizeChange = rhs.mImpl.mSize - mImpl.mSize;
        if (mama_isQuantityNone (mImpl.mSizeChange)) mImpl.mSizeChange = 0.0;
        mImpl.mSize = rhs.mImpl.mSize;
        mImpl.mNumEntries = rhs.mImpl.mNumEntries;
        mImpl.mNumEntriesTotal = rhs.mImpl.mNumEntriesTotal;
        mImpl.mTime = rhs.mImpl.mTime;
        mImpl.mOrderType = rhs.mImpl.mOrderType;
    }

    void MamdaOrderBookPriceLevel::copy (const MamdaOrderBookPriceLevel&  rhs)
    {
        mImpl.copy (rhs.mImpl);
    }

    void MamdaOrderBookPriceLevel::copyLevelOnly (const MamdaOrderBookPriceLevel&  rhs)
    {
        mImpl.copyLevelOnly (rhs.mImpl);
    }

    void MamdaOrderBookPriceLevel::addEntry (MamdaOrderBookEntry*  entry)
    {
        mImpl.addEntry (entry);
    }

    void MamdaOrderBookPriceLevel::updateEntry (const MamdaOrderBookEntry&  entry)
    {
        mImpl.updateEntry (entry);
    }

    void MamdaOrderBookPriceLevel::removeEntryById (const MamdaOrderBookEntry&  entry)
    {
        mImpl.removeEntryById (entry);
    }

    void MamdaOrderBookPriceLevel::removeEntry (const MamdaOrderBookEntry*  entry)
    {
        mImpl.removeEntry (entry);
    }

    void MamdaOrderBookPriceLevel::addEntriesFromLevel (
        const MamdaOrderBookPriceLevel*  level,
        MamdaOrderBookEntryFilter*       filter,
        MamdaOrderBookBasicDeltaList*    delta)
    {    
        mImpl.addEntriesFromLevel (level->mImpl, filter, delta);
    }

    void MamdaOrderBookPriceLevel::deleteEntriesFromSource (
        const MamaSource*               source,
        MamdaOrderBookBasicDeltaList*   delta)
    {    
        mImpl.deleteEntriesFromSource (source, delta);
    }

    bool MamdaOrderBookPriceLevel::reevaluate ()
    {
        return mImpl.reevaluate ();
    }

    double MamdaOrderBookPriceLevel::getPrice () const
    {
        return mImpl.mPrice.getValue();
    }

    MamaPrice MamdaOrderBookPriceLevel::getMamaPrice () const
    {
        return mImpl.mPrice;
    }

    mama_quantity_t MamdaOrderBookPriceLevel::getSize () const
    {
        return mImpl.mSize;
    }

    mama_quantity_t MamdaOrderBookPriceLevel::getSizeChange () const
    {
        return mImpl.mSizeChange;
    }

    mama_u32_t MamdaOrderBookPriceLevel::getNumEntries () const
    {
        return mImpl.mNumEntries;
    }

    mama_u32_t MamdaOrderBookPriceLevel::getNumEntriesTotal () const
    {
        return mImpl.mNumEntriesTotal;
    }

    bool MamdaOrderBookPriceLevel::empty () const
    {
        return  !mImpl.mEntries || mImpl.mEntries->empty();
    }

    bool MamdaOrderBookPriceLevel::operator== (
        const MamdaOrderBookPriceLevel& rhs) const
    {
        return mImpl == rhs.mImpl;
    }

    void MamdaOrderBookPriceLevel::setOrderBook (MamdaOrderBook*  book)
    {
        mImpl.mBook = book;
    }

    MamdaOrderBook* MamdaOrderBookPriceLevel::getOrderBook () const
    {
        return mImpl.mBook;
    }

    const char* MamdaOrderBookPriceLevel::getSymbol() const
    {
        if (mImpl.mBook)
            return mImpl.mBook->getSymbol();
        else
            return NULL;
    }

    MamdaOrderBookEntry* MamdaOrderBookPriceLevel::findEntry (
        const char*  id) const
    {
        return mImpl.findEntry (id);
    }

    MamdaOrderBookEntry* MamdaOrderBookPriceLevel::findOrCreateEntry (
        const char*  id)
    {
        bool newEntry = false;
        return mImpl.findOrCreateEntry (id, newEntry);
    }

    MamdaOrderBookEntry* MamdaOrderBookPriceLevel::findOrCreateEntry (
        const char* id,
        bool&       newEntry)
    {
        return mImpl.findOrCreateEntry (id, newEntry);
    }


    MamdaOrderBookEntry* MamdaOrderBookPriceLevel::getEntryAtPosition (
        mama_u32_t  pos) const
    {
        return mImpl.getEntryAtPosition (pos);
    }

    void MamdaOrderBookPriceLevel::setClosure (
        void*  closure)
    {
        mImpl.mClosure = closure;
    }

    void* MamdaOrderBookPriceLevel::getClosure () const
    {
        return mImpl.mClosure;
    }

    void MamdaOrderBookPriceLevel::assertEqual (
        const MamdaOrderBookPriceLevel&  rhs) const
    {
        mImpl.assertEqual (rhs.mImpl);
    }

    MamdaOrderBookPriceLevel::Side MamdaOrderBookPriceLevel::getSide () const
    {
        return mImpl.mSide;
    }

    MamdaOrderBookPriceLevel::Action MamdaOrderBookPriceLevel::getAction () const
    {
        return mImpl.mAction;
    }

    const MamaDateTime& MamdaOrderBookPriceLevel::getTime () const
    {
        return mImpl.mTime;
    }

    MamdaOrderBookPriceLevel::MamdaOrderBookPriceLevelImpl::MamdaOrderBookPriceLevelImpl (
        MamdaOrderBookPriceLevel&  level)
        : mLevel      (level)
        , mPrice      (0.0)
        , mSize       (0.0)
        , mSizeChange (0.0)
        , mNumEntries (0)
        , mNumEntriesTotal (0)
        , mSide       (MAMDA_BOOK_SIDE_BID)
        , mAction     (MAMDA_BOOK_ACTION_ADD)
        , mEntries    (NULL)
        , mBook       (NULL)
        , mOrderType  (MAMDA_BOOK_LEVEL_LIMIT)
        , mClosure    (NULL)
    {
    }

    MamdaOrderBookPriceLevel::MamdaOrderBookPriceLevelImpl::MamdaOrderBookPriceLevelImpl (
        MamdaOrderBookPriceLevel& level,
        const MamdaOrderBookPriceLevelImpl&  copy)
        : mLevel      (level)
        , mPrice      (0.0)
        , mSize       (0.0)
        , mSizeChange (0.0)
        , mNumEntries (0)
        , mNumEntriesTotal (0)
        , mSide       (MAMDA_BOOK_SIDE_BID)
        , mAction     (MAMDA_BOOK_ACTION_ADD)
        , mEntries    (NULL)
        , mBook       (NULL)
        , mOrderType  (MAMDA_BOOK_LEVEL_LIMIT)
        , mClosure    (NULL)
    {
        MamdaOrderBookPriceLevelImpl::copy (copy);
    }

    MamdaOrderBookPriceLevel::MamdaOrderBookPriceLevelImpl::MamdaOrderBookPriceLevelImpl (
        MamdaOrderBookPriceLevel& level,
        MamaPrice&  price,
        Side    side)
        : mLevel      (level)
        , mPrice      (price)
        , mSize       (0.0)
        , mSizeChange (0.0)
        , mNumEntries (0)
        , mNumEntriesTotal (0)
        , mSide       (side)
        , mAction     (MAMDA_BOOK_ACTION_ADD)
        , mEntries    (NULL)
        , mBook       (NULL)
        , mOrderType  (MAMDA_BOOK_LEVEL_LIMIT)
        , mClosure    (NULL)
    {
    }

    MamdaOrderBookPriceLevel::MamdaOrderBookPriceLevelImpl::MamdaOrderBookPriceLevelImpl (
        MamdaOrderBookPriceLevel& level,
        double  price,
        Side    side)
        : mLevel      (level)
        , mPrice      (price)
        , mSize       (0.0)
        , mSizeChange (0.0)
        , mNumEntries (0)
        , mNumEntriesTotal (0)
        , mSide       (side)
        , mAction     (MAMDA_BOOK_ACTION_ADD)
        , mEntries    (NULL)
        , mBook       (NULL)
        , mOrderType  (MAMDA_BOOK_LEVEL_LIMIT)
        , mClosure    (NULL)
    {
    }

    MamdaOrderBookPriceLevel::MamdaOrderBookPriceLevelImpl::~MamdaOrderBookPriceLevelImpl ()
    {
        if (mEntries)
            clearEntries (*mEntries);

        delete mEntries;
    }

    bool MamdaOrderBookPriceLevel::MamdaOrderBookPriceLevelImpl::operator== (
        const MamdaOrderBookPriceLevelImpl& rhs) const
    {
        return (mPrice      == rhs.mPrice)      &&
               (mSize       == rhs.mSize)       &&
               (mNumEntries == rhs.mNumEntries) &&
               (mNumEntriesTotal == rhs.mNumEntriesTotal) &&
               (mSide       == rhs.mSide)       &&
               (mAction     == rhs.mAction)     &&
               (mTime       == rhs.mTime)       &&
               (mOrderType  == rhs.mOrderType)  &&
               ((!mEntries && !rhs.mEntries) ||  /* no entries or */
                ( mEntries &&  rhs.mEntries) &&  /* entries and entries equal */
                (*mEntries == *rhs.mEntries));
                                /* invokes MamdaOrderBookEntry::operator==() */
    }

#ifndef __sun
    bool MamdaOrderBookPriceLevel::sortEntriesByTime = false;

    static bool sortEntriesByTimeFunc ( MamdaOrderBookEntry* lhs, MamdaOrderBookEntry* rhs)
    {
        return (lhs->getTime() < rhs->getTime());
    }
#endif

    void MamdaOrderBookPriceLevel::MamdaOrderBookPriceLevelImpl::assertEqual (
        const MamdaOrderBookPriceLevelImpl& rhs) const
    {
        if (mPrice != rhs.mPrice)
            throwError ("price not equal");
        if (!mama_isQuantityEqual (mSize, rhs.mSize))
            throwError ("size not equal");
        if (mNumEntries != rhs.mNumEntries)
            throwError ("number of entries not equal");
        if (mNumEntriesTotal != rhs.mNumEntriesTotal)
            throwError ("number of total entries not equal");
        if (mSide != rhs.mSide)
            throwError ("side not equal");
        if (mAction != rhs.mAction)
            throwError ("action not equal");
        if (mTime != rhs.mTime) 
            throwError ("time not equal");
        if (mOrderType != rhs.mOrderType)
            throwError ("order type not equal");
        if ((mEntries && !rhs.mEntries) || (!mEntries && rhs.mEntries))
            throwError ("entries mismatch");
        if (mEntries && rhs.mEntries)
        {
            if (mEntries->size() != rhs.mEntries->size())
            {
                char context[256];
                snprintf (context, 256, "entries size mismatch (%ld!=%ld)",
                          (long)mEntries->size(), (long)rhs.mEntries->size());
                throwError (context);
            }
            EntryList::const_iterator lhsIter = mEntries->begin();
            EntryList::const_iterator rhsIter = rhs.mEntries->begin();
            size_t size = mEntries->size();
            for (size_t i = 0; i < size; ++i, ++lhsIter, ++rhsIter)
            {
                const MamdaOrderBookEntry* lhsEntry = *lhsIter;
                const MamdaOrderBookEntry* rhsEntry = *rhsIter;
                lhsEntry->assertEqual (*rhsEntry);
            }
        }
    }

    void MamdaOrderBookPriceLevel::MamdaOrderBookPriceLevelImpl::clear ()
    {
        mPrice.clear();
        mSize       = 0.0;
        mSizeChange = 0.0;
        mNumEntries = 0;
        mNumEntriesTotal = 0;
        mSide       = MAMDA_BOOK_SIDE_BID;
        mAction     = MAMDA_BOOK_ACTION_ADD;
        mOrderType  = MAMDA_BOOK_LEVEL_LIMIT;
        mTime.clear();
        if (mEntries)
        {
            clearEntries (*mEntries);
        }
        mBook       = NULL;
        mClosure    = NULL;
    }

    void MamdaOrderBookPriceLevel::MamdaOrderBookPriceLevelImpl::copy (
        const MamdaOrderBookPriceLevelImpl&  copy)
    {
        if (mEntries)
        {
            clearEntries (*mEntries);
        }
        if (copy.mEntries)
        {
            if (!mEntries)
                mEntries = new EntryList;
            mNumEntries = 0;
            mNumEntriesTotal = 0;
            EntryList::iterator end = copy.mEntries->end();
            EntryList::iterator i   = copy.mEntries->begin();
            while (i != end)
            {
                addEntry (new MamdaOrderBookEntry(**i));
                ++i;
            }
        }
        mPrice      = copy.mPrice;
        mSize       = copy.mSize;
        mSizeChange = copy.mSizeChange;
        mNumEntries = copy.mNumEntries;
        mNumEntriesTotal = copy.mNumEntriesTotal;
        mSide       = copy.mSide;
        mAction     = copy.mAction;
        mOrderType  = copy.mOrderType;
        mTime       = copy.mTime;
        mBook       = NULL;
        mClosure    = NULL;
    }

    void MamdaOrderBookPriceLevel::MamdaOrderBookPriceLevelImpl::copyLevelOnly (
        const MamdaOrderBookPriceLevelImpl&  copy)
    {
        mPrice      = copy.mPrice;
        mSize       = copy.mSize;
        mSizeChange = copy.mSizeChange;
        mNumEntries = copy.mNumEntries;
        mNumEntriesTotal = copy.mNumEntriesTotal;
        mSide       = copy.mSide;
        mAction     = copy.mAction;
        mTime       = copy.mTime;
        mBook       = NULL;
        mClosure    = NULL;
    }

    void MamdaOrderBookPriceLevel::MamdaOrderBookPriceLevelImpl::addEntry (
        MamdaOrderBookEntry*  entry)
    {
        if (!mEntries)
            mEntries = new EntryList;
        bool checkState = mBook ? mBook->getCheckSourceState() : false;
        mEntries->push_back (entry);
        if (!checkState || entry->isVisible())
        {
            mNumEntries++;
            mSize += entry->getSize();
        }
        mNumEntriesTotal++;
        entry->setPriceLevel (&mLevel);
        
        if ((mBook) && (mBook->getGenerateDeltaMsgs()))
        {
            //Need to set correct action based on numEntries in level
            MamdaOrderBookPriceLevel::Action plAction;
            if (mNumEntriesTotal > 1) 
            {
                plAction = MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_UPDATE;
            }
            else
            {
                plAction = MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD;
            }
            mBook->addDelta(entry, entry->getPriceLevel(), 
                             entry->getPriceLevel()->getSizeChange(),
                             plAction,
                             MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD);
        }
    }

    void MamdaOrderBookPriceLevel::MamdaOrderBookPriceLevelImpl::updateEntry (
        const MamdaOrderBookEntry&  entry)
    {
        if (!mEntries)
            mEntries = new EntryList;
        EntryList::iterator end = mEntries->end();
        EntryList::iterator i   = mEntries->begin();
        while (i != end)
        {
            MamdaOrderBookEntry* existingEntry = *i;
            if (existingEntry->equalId(entry.getId()))
            {
                // found it
                existingEntry->setDetails (entry);
                
                if ((mBook) && (mBook->getGenerateDeltaMsgs()))
                {
                    mBook->addDelta(const_cast<MamdaOrderBookEntry*>(&entry), entry.getPriceLevel(), 
                                     entry.getPriceLevel()->getSizeChange(),
                                     MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_UPDATE,
                                     MamdaOrderBookEntry::MAMDA_BOOK_ACTION_UPDATE);
                }
                return;
            }
            ++i;
        }
        if (sStrictChecking)
        {
            string errStr = string("attempted to update a non-existent entry: ") +
                entry.getId();
            throw MamdaOrderBookException(errStr);
        }
    }

    void MamdaOrderBookPriceLevel::MamdaOrderBookPriceLevelImpl::removeEntryById (
        const MamdaOrderBookEntry&  entry)
    {
        if (!mEntries)
            mEntries = new EntryList;
        EntryList::iterator end = mEntries->end();
        EntryList::iterator i   = mEntries->begin();
        while (i != end)
        {
            MamdaOrderBookEntry* existingEntry = *i;
            if (existingEntry->equalId(entry.getId()))
            {
                // found it
                eraseEntryByIterator (mEntries, i, existingEntry);
                return;
            }
            ++i;
        }
        if (sStrictChecking)
        {
            string errStr = string("attempted to delete a non-existent entry: ") +
                entry.getId();
            throw MamdaOrderBookException(errStr);
        }
    }

    void MamdaOrderBookPriceLevel::MamdaOrderBookPriceLevelImpl::removeEntry (
        const MamdaOrderBookEntry*  entry)
    {
        if (mEntries)
        {
            EntryList::iterator end = mEntries->end();
            EntryList::iterator i   = mEntries->begin();
            while (i != end)
            {
                MamdaOrderBookEntry* existingEntry = *i;
                if (existingEntry == entry)
                {
                    // found it
                    eraseEntryByIterator (mEntries, i, existingEntry);
                    return;
                }
                ++i;
            }
        }

        if (sStrictChecking)
        {
            string errStr = string("attempted to delete a non-existent entry: ") +
                entry->getId();
            throw MamdaOrderBookException(errStr);
        }
    }

    void MamdaOrderBookPriceLevel::MamdaOrderBookPriceLevelImpl::eraseEntryByIterator (
        EntryList*            entryList,
        EntryList::iterator   erasableIter,
        MamdaOrderBookEntry*  entry)
    {
        bool checkState = mBook ? mBook->getCheckSourceState() : false;
        mEntries->erase(erasableIter);
        mBook->detach (entry);
        if (!checkState || entry->isVisible())
        {
            mSize -= entry->getSize();
            if (mama_isQuantityNone (mSize)) mSize = 0.0;
            mNumEntries--;
        }
        mNumEntriesTotal--;
    }

    void MamdaOrderBookPriceLevel::MamdaOrderBookPriceLevelImpl::checkNotExist (
        const MamdaOrderBookEntry&  entry)
    {
        if (!mEntries || mEntries->empty())
            return;
        EntryList::iterator end = mEntries->end();
        EntryList::iterator i   = mEntries->begin();
        while (i != end)
        {
            MamdaOrderBookEntry* existingEntry = *i;
            if (existingEntry->equalId(entry.getId()))
            {
                // Found it, but it was not supposed to be here!
                string errStr = string("attempted to add an existent entry: ") +
                    entry.getId();
                throw MamdaOrderBookException(errStr);
            }
            ++i;
        }
    }

    void MamdaOrderBookPriceLevel::MamdaOrderBookPriceLevelImpl::addEntriesFromLevel (
        const MamdaOrderBookPriceLevelImpl&  levelImpl,
        MamdaOrderBookEntryFilter*           filter,
        MamdaOrderBookBasicDeltaList*        delta)
    {       
        if (!levelImpl.mEntries)
            return;
        EntryList::const_iterator end = levelImpl.mEntries->end();
        EntryList::const_iterator i   = levelImpl.mEntries->begin();
        for (; i != end; ++i)
        {
            const MamdaOrderBookEntry*  entry = *i;        
            MamdaOrderBookEntry* copyEntry = new MamdaOrderBookEntry (*entry);
            if (filter && !filter->checkEntry(copyEntry))
            {
                delete copyEntry;
            }
            else
            {
                copyEntry->setAction(MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD);
                addEntry (copyEntry);
            } 
            if(delta)
            {
                delta->add (copyEntry, &(mLevel), (entry->getSize()), 
                            MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD, 
                            MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD);
            }
        }
        mTime=levelImpl.mTime;
    }

    void MamdaOrderBookPriceLevel::MamdaOrderBookPriceLevelImpl::deleteEntriesFromSource (
        const MamaSource*               source,
        MamdaOrderBookBasicDeltaList*   delta)
    {
        if (!mEntries || mEntries->empty())
            return;
        EntryList::iterator  end = mEntries->end();
        EntryList::iterator  i   = mEntries->begin();
        EntryList::iterator  erasable;
        MamdaOrderBookEntry* entry;
        while (i != end)
        {
            entry = *i;
            if (entry->getSource()->isPartOf (source))
            {
                erasable = i;
                ++i;
                if (delta)
                {
                    delta->add (entry, &(mLevel), -(entry->getSize()), MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_UPDATE, 
                                MamdaOrderBookEntry::MAMDA_BOOK_ACTION_DELETE);
                }
                eraseEntryByIterator (mEntries, erasable, entry);
            }
            else
            {
                ++i;
            }
        }
    }

    bool MamdaOrderBookPriceLevel::MamdaOrderBookPriceLevelImpl::reevaluate ()
    {
        if (!mEntries || mEntries->empty())
            return false;
        mama_quantity_t  size       = 0.0;
        mama_u32_t       numEntries = 0;
        bool             changed    = false;
        bool checkState = mBook ? mBook->getCheckSourceState() : false;
        EntryList::iterator  end = mEntries->end();
        EntryList::iterator  i   = mEntries->begin();
        MamdaOrderBookEntry* entry;
        for (;
             i != end;
             ++i)
        {
            entry = *i;
            if (!checkState || entry->isVisible())
            {
                size += entry->getSize();
                numEntries++;
            }
        }
        if (!mama_isQuantityEqual (mSize,size))
        {
            mSize = size;
            changed = true;
        }
        if (mNumEntries != numEntries)
        {
            mNumEntries = numEntries;
            changed = true;
        }
        return changed;
    }

    void MamdaOrderBookPriceLevel::markAllDeleted ()
    {
        setSizeChange (-getSize());
        setSize (0);
        setNumEntries (0);
        setAction (MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_DELETE);

        MamdaOrderBookPriceLevel::iterator entryEnd  = end();
        MamdaOrderBookPriceLevel::iterator entryIter = begin();

        for (;  entryIter != entryEnd;  ++entryIter)
        {
            MamdaOrderBookEntry*  entry = *entryIter;
            entry->setSize (0);
            entry->setAction (MamdaOrderBookEntry::MAMDA_BOOK_ACTION_DELETE);
        }
    }

    void MamdaOrderBookPriceLevel::setAsDifference (
        const MamdaOrderBookPriceLevel&  lhs,
        const MamdaOrderBookPriceLevel&  rhs)
    {
        assert (lhs.getPrice() == rhs.getPrice());

        MamdaOrderBookPriceLevel::iterator lhsEnd  = end();
        MamdaOrderBookPriceLevel::iterator lhsIter = begin();
        MamdaOrderBookPriceLevel::iterator rhsEnd  = end();
        MamdaOrderBookPriceLevel::iterator rhsIter = begin();

        while ((lhsIter != lhsEnd) && (rhsIter != rhsEnd))
        {
            const char*      lhsId   = NULL;
            const char*      rhsId   = NULL;
            mama_quantity_t  lhsSize = 0.0;
            mama_quantity_t  rhsSize = 0.0;

            if (lhsIter != lhsEnd)
            {
                const MamdaOrderBookEntry* entry = *lhsIter;
                lhsId   = entry->getId();
                lhsSize = entry->getSize();
            }
            if (rhsIter != rhsEnd)
            {
                const MamdaOrderBookEntry* entry = *rhsIter;
                rhsId   = entry->getId();
                rhsSize = entry->getSize();
            }

            if (lhsId && rhsId)
            {
                if (strcmp(lhsId,rhsId)==0)
                {
                    // Same ID, maybe different size.
                    if (mama_isQuantityEqual (lhsSize, rhsSize))
                    {
                        MamdaOrderBookEntry* updateEntry =
                            new MamdaOrderBookEntry (**rhsIter);
                        updateEntry->setAction(
                            MamdaOrderBookEntry::MAMDA_BOOK_ACTION_UPDATE);
                        addEntry (updateEntry);
                    }
                    ++lhsIter;
                    ++rhsIter;
                    continue;
                }
                else
                {
                    // Different ID (either something exists on the LHS
                    // and not on RHS or vice versa).
                    MamdaOrderBookPriceLevel::const_iterator rhsFound = 
                        findEntryAfter (rhsIter, lhsId);
                    if (rhsFound != rhsEnd)
                    {
                        // The ID from the LHS was found on the RHS, so
                        // there must have been additional entries on the
                        // RHS, which we now need to add.
                        do
                        {
                            MamdaOrderBookEntry* entry =
                                new MamdaOrderBookEntry(**rhsIter);
                            addEntry (entry);
                            ++rhsIter;
                        }
                        while (rhsIter != rhsFound);
                    }
                    else
                    {
                        // The ID from the LHS was not present on the RHS,
                        // so add the LHS entry.  Note: it would probably
                        // be faster to iterate over the LHS side rather
                        // than begin the loop again.
                        MamdaOrderBookEntry* entry =
                            new MamdaOrderBookEntry(**lhsIter);
                        addEntry (entry);
                        ++lhsIter;
                    }
                }
            }
        }

        setPrice (rhs.getPrice());
        setSizeChange (rhs.getSize() - lhs.getSize());
        setSize (rhs.getSize());
        setNumEntries (rhs.getNumEntries());
        setAction (MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_UPDATE);
        setTime (rhs.getTime());
        setSide (rhs.getSide());
    }

    MamdaOrderBookEntry*
    MamdaOrderBookPriceLevel::MamdaOrderBookPriceLevelImpl::findEntry (
        const char*  id) const
    {
        if (mEntries)
        {
            EntryList::iterator end  = mEntries->end();
            EntryList::iterator iter = mEntries->begin();
            for (; iter != end; ++iter)
            {
                MamdaOrderBookEntry*  entry = *iter;
                if (strcmp (id, entry->getId()) == 0)
                {
                    return entry;
                }
            }
        }
        return NULL;
    }

    MamdaOrderBookEntry*
    MamdaOrderBookPriceLevel::MamdaOrderBookPriceLevelImpl::findOrCreateEntry (
        const char* id,
        bool&       newEntry)
    {
        if (mEntries)
        {
            EntryList::iterator end  = mEntries->end();
            EntryList::iterator iter = mEntries->begin();
            for (; iter != end; ++iter)
            {
                MamdaOrderBookEntry*  entry = *iter;
                if (strcmp (id, entry->getId()) == 0)
                {
                    newEntry = false;
                    return entry;
                }
            }
        }
        else
        {
            mEntries = new EntryList;
        }
        MamdaOrderBookEntry*  entry = new MamdaOrderBookEntry;
        entry->setId (id);
        entry->setAction (MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD);
        entry->setPriceLevel (&mLevel);
        mEntries->push_back (entry);
        mNumEntries++;
        mNumEntriesTotal++;
        newEntry = true;

        if (mBook->getGenerateDeltaMsgs())
        {
            //Need to set correct action based on numEntries in level
            MamdaOrderBookPriceLevel::Action plAction;
            if (0 == mNumEntriesTotal) 
            {
                plAction = MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD;
            }
            else
            {
                plAction = MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_UPDATE;
            }
            mBook->addDelta(entry, entry->getPriceLevel(), 
                             entry->getPriceLevel()->getSizeChange(),
                             plAction,
                             MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD);
        }
        return entry;
    }

    MamdaOrderBookEntry*
    MamdaOrderBookPriceLevel::MamdaOrderBookPriceLevelImpl::getEntryAtPosition (
        mama_u32_t  pos) const
    {
        // Remember: pos may be zero, which would mean we want the first
        // entry.
        if (!mEntries)
            return NULL;
        bool checkState = mBook ? mBook->getCheckSourceState() : false;
        mama_u32_t  i = 0;
        EntryList::iterator   end  = mEntries->end();
        EntryList::iterator   iter = mEntries->begin();
        MamdaOrderBookEntry*  entry;
        for (; iter != end; ++iter)
        {
            entry = *iter;
            if (checkState && !entry->isVisible())
                continue;
            if (pos == i)
                return entry;
            i++;
        }
        return NULL;
    }

    struct MamdaOrderBookPriceLevel::iterator::iteratorImpl
    {
        iteratorImpl ()
            : mEntries  (NULL)
            {}
        iteratorImpl (const iteratorImpl& copy)
            : mEntries  (copy.mEntries)
            , mIterator (copy.mIterator)
            {}
        iteratorImpl (EntryList*  entries,
                      EntryList::iterator  iter)
            : mEntries  (entries)
            , mIterator (iter)
            {}
        ~iteratorImpl () {}
        EntryList*          mEntries;
        EntryList::iterator mIterator;
    };

    MamdaOrderBookPriceLevel::iterator::iterator ()
        : mImpl (*new iteratorImpl)
    {
    }

    MamdaOrderBookPriceLevel::iterator::iterator (const iterator& copy)
        : mImpl (*new iteratorImpl(copy.mImpl.mEntries, copy.mImpl.mIterator))
    {
    }

    MamdaOrderBookPriceLevel::iterator::iterator (const iteratorImpl& copy)
        : mImpl (*new iteratorImpl(copy.mEntries, copy.mIterator))
    {}


    MamdaOrderBookPriceLevel::iterator::~iterator ()
    {
        delete &mImpl;
    }

    MamdaOrderBookPriceLevel::iterator MamdaOrderBookPriceLevel::begin()
    {
        if (!mImpl.mEntries)
        {
            mImpl.mEntries = new EntryList;
        }
        bool checkState = mImpl.mBook ? mImpl.mBook->getCheckSourceState() : false;
#ifndef __sun
        if (sortEntriesByTime)
            mImpl.mEntries->sort(&sortEntriesByTimeFunc);
#endif
        EntryList::iterator   end = mImpl.mEntries->end();
        EntryList::iterator   beg = mImpl.mEntries->begin();
        MamdaOrderBookEntry*  entry;
        while (beg != end)
        { 
            entry = *beg;
            if (!checkState || entry->isVisible())
                break;
            ++beg;
        }
        return iterator(iterator::iteratorImpl(mImpl.mEntries, beg));
    }

    MamdaOrderBookPriceLevel::iterator& MamdaOrderBookPriceLevel::begin(iterator& reuse)
    {
        if (!mImpl.mEntries)
            mImpl.mEntries = new EntryList;
        bool checkState = mImpl.mBook ? mImpl.mBook->getCheckSourceState() : false;
#ifndef __sun
        if (sortEntriesByTime)
            mImpl.mEntries->sort(&sortEntriesByTimeFunc);
#endif
        EntryList::iterator end = mImpl.mEntries->end();
        EntryList::iterator beg = mImpl.mEntries->begin();
        MamdaOrderBookEntry*  entry;
        while (beg != end)
        {
            entry = *beg;
            if (!checkState || entry->isVisible())
                break;
            ++beg;
        }
        reuse.mImpl.mEntries = mImpl.mEntries;
        reuse.mImpl.mIterator = beg;
        return reuse;
    }

    MamdaOrderBookPriceLevel::iterator MamdaOrderBookPriceLevel::end()
    {   
        if (!mImpl.mEntries)
        {   
            mImpl.mEntries = new EntryList;
        }
        return iterator(iterator::iteratorImpl(mImpl.mEntries,
                                               mImpl.mEntries->end()));
    }

    MamdaOrderBookPriceLevel::iterator& MamdaOrderBookPriceLevel::end(iterator& reuse)
    {
        if (!mImpl.mEntries)
            mImpl.mEntries = new EntryList;
        reuse.mImpl.mEntries = mImpl.mEntries;
        reuse.mImpl.mIterator = mImpl.mEntries->end();
        return reuse;
    }

    MamdaOrderBookPriceLevel::const_iterator MamdaOrderBookPriceLevel::begin() const
    {
        if (!mImpl.mEntries)
            mImpl.mEntries = new EntryList;
        bool checkState = mImpl.mBook ? mImpl.mBook->getCheckSourceState() : false;
#ifndef __sun
        if (sortEntriesByTime)
            mImpl.mEntries->sort(&sortEntriesByTimeFunc);
#endif
        EntryList::iterator end = mImpl.mEntries->end();
        EntryList::iterator beg = mImpl.mEntries->begin();
        const MamdaOrderBookEntry*  entry;
        while (beg != end)
        {
            entry = *beg;
            if (!checkState || entry->isVisible())
                break;
            ++beg;
        }
        return const_iterator(iterator::iteratorImpl(mImpl.mEntries, beg));
    }

    MamdaOrderBookPriceLevel::const_iterator& MamdaOrderBookPriceLevel::begin(const_iterator& reuse) const
    {
        if (!mImpl.mEntries)
            mImpl.mEntries = new EntryList;
        bool checkState = mImpl.mBook ? mImpl.mBook->getCheckSourceState() : false;
#ifndef __sun
        if (sortEntriesByTime)
            mImpl.mEntries->sort(&sortEntriesByTimeFunc);
#endif
        EntryList::iterator end = mImpl.mEntries->end();
        EntryList::iterator beg = mImpl.mEntries->begin();
        const MamdaOrderBookEntry*  entry;
        while (beg != end)
        {
            entry = *beg;
            if (!checkState || entry->isVisible())
                break;
            ++beg;
        }
        reuse.mImpl.mEntries = mImpl.mEntries;
        reuse.mImpl.mIterator = beg;
        return reuse;
    }

    MamdaOrderBookPriceLevel::const_iterator MamdaOrderBookPriceLevel::end() const
    {
        if (!mImpl.mEntries)
            mImpl.mEntries = new EntryList;
        return const_iterator(iterator::iteratorImpl(mImpl.mEntries,
                                                     mImpl.mEntries->end()));
    }

    MamdaOrderBookPriceLevel::const_iterator& MamdaOrderBookPriceLevel::end(const_iterator& reuse) const
    {
        if (!mImpl.mEntries)
            mImpl.mEntries = new EntryList;
        reuse.mImpl.mEntries = mImpl.mEntries;
        reuse.mImpl.mIterator = mImpl.mEntries->end();
        return reuse;
    }


    MamdaOrderBookPriceLevel::iterator&
    MamdaOrderBookPriceLevel::iterator::operator= (const iterator& rhs)
    {
        if (&rhs != this)
        {
            mImpl.mEntries  = rhs.mImpl.mEntries;
            mImpl.mIterator = rhs.mImpl.mIterator;
        }        
        return *this;
    }

    const MamdaOrderBookPriceLevel::iterator&
    MamdaOrderBookPriceLevel::iterator::operator++ () const
    {
        EntryList::iterator end = mImpl.mEntries->end();
        if (mImpl.mEntries && mImpl.mIterator != end)
        {
            const MamdaOrderBookEntry*  entry;
            while (true)
            {
                ++mImpl.mIterator;
                if (mImpl.mIterator == end)
                    break;
                entry = *mImpl.mIterator;
                if (!entry->getCheckSourceState() || entry->isVisible())
                    break;
            }
        }
        return *this;
    }

    MamdaOrderBookPriceLevel::iterator&
    MamdaOrderBookPriceLevel::iterator::operator++ ()
    {
        EntryList::iterator end = mImpl.mEntries->end();
        if (mImpl.mEntries && mImpl.mIterator != end)
        {
            const MamdaOrderBookEntry*  entry;
            while (true)
            {
                ++mImpl.mIterator;
                if (mImpl.mIterator == end)
                    break;
                entry = *mImpl.mIterator;
                if (!entry->getCheckSourceState() || entry->isVisible())
                    break;
            }
        }
        return *this;
    }

    bool MamdaOrderBookPriceLevel::iterator::operator== (const iterator& rhs) const
    {
        return ((&rhs == this) || (mImpl.mEntries && rhs.mImpl.mEntries && 
                                   mImpl.mIterator == rhs.mImpl.mIterator));
    }

    bool MamdaOrderBookPriceLevel::iterator::operator!= (const iterator& rhs) const
    {
        return !operator==(rhs);
    }

    MamdaOrderBookEntry*
    MamdaOrderBookPriceLevel::iterator::operator*()
    {
        return mImpl.mEntries ? *mImpl.mIterator : NULL;
    }

    const MamdaOrderBookEntry*
    MamdaOrderBookPriceLevel::iterator::operator*() const
    {
        return mImpl.mEntries ? *mImpl.mIterator : NULL;
    }

    MamdaOrderBookPriceLevel::iterator MamdaOrderBookPriceLevel::findEntryAfter (
        iterator&    start,
        const char*  id)
    {
        iterator result = start;
        iterator end    = this->end();
        while (result != end)
        {
            const MamdaOrderBookEntry* entry = *result;
            if (strcmp (id, entry->getId()) == 0)
            {
                return result;
            }
            ++result;
        }
        return result;
    }

    MamdaOrderBookPriceLevel::const_iterator MamdaOrderBookPriceLevel::findEntryAfter (
        const_iterator&  start,
        const char*      id) const
    {
        const_iterator i   = start;
        const_iterator end = this->end();
        while (i != end)
        {
            const MamdaOrderBookEntry* entry = *i;
            if (strcmp (id, entry->getId()) == 0)
            {
                return i;
            }
            ++i;
        }
        return i;
    }

    void MamdaOrderBookPriceLevel::MamdaOrderBookPriceLevelImpl::throwError (
        const char*  context) const
    {
        char errMsg[256];
        snprintf (errMsg, 256, "%s (price=%g, side=%c)", context, mPrice.getValue(), mSide);
        throw MamdaOrderBookException (errMsg);
    }

    void MamdaOrderBookPriceLevel::MamdaOrderBookPriceLevelImpl::clearEntries (
        EntryList& entries)
    {
        EntryList::iterator itr   = entries.begin();
        EntryList::iterator end   = entries.end();
        for (; itr != end; itr++)
        {
            delete *itr;
        }
        entries.clear();
    }

    void MamdaOrderBookPriceLevel::setStrictChecking (bool strict)
    {
        sStrictChecking = strict;
    }

}

