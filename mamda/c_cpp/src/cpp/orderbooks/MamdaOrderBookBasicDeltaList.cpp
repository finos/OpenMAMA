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

#include <mamda/MamdaOrderBookBasicDeltaList.h>
#include <list>
#include <iostream>
#include <map>

using std::list;

namespace Wombat
{

    typedef list<MamdaOrderBookBasicDelta*>         BasicDeltaList;

    typedef std::map <double,MamdaOrderBookBasicDelta*> BasicDeltaMap; // For levels only
    typedef std::map <double,BasicDeltaList*>           DeltaListMap; // For entries

    struct MamdaOrderBookBasicDeltaList::MamdaOrderBookBasicDeltaListImpl
    {
        MamdaOrderBookBasicDeltaListImpl();
            
        void checkSide (const MamdaOrderBookPriceLevel*  level);

        void conflateLevelDeltas (MamdaOrderBookEntry*              entry,
                                  MamdaOrderBookPriceLevel*         level,
                                  mama_quantity_t                   plDeltaSize,
                                  MamdaOrderBookPriceLevel::Action  plAction,
                                  MamdaOrderBookEntry::Action       entryAction);
                                 
        void conflateEntryDeltas (MamdaOrderBookEntry*              entry,
                                  MamdaOrderBookPriceLevel*         level,
                                  mama_quantity_t                   plDeltaSize,
                                  MamdaOrderBookPriceLevel::Action  plAction,
                                  MamdaOrderBookEntry::Action       entryAction);
                                                        
        void addEntryDelta       (BasicDeltaList&                   deltaList,
                                  MamdaOrderBookEntry*              entry,
                                  MamdaOrderBookPriceLevel*         level,
                                  mama_quantity_t                   plDeltaSize,
                                  MamdaOrderBookPriceLevel::Action  plAction,
                                  MamdaOrderBookEntry::Action       entryAction);
                                
        void applyEntryDelta     (BasicDeltaList&                   deltaList,
                                  MamdaOrderBookEntry*              entry,
                                  MamdaOrderBookPriceLevel*         level,
                                  mama_quantity_t                   plDeltaSize,
                                  MamdaOrderBookPriceLevel::Action  plAction,
                                  MamdaOrderBookEntry::Action       entryAction);
                                
        BasicDeltaMap*                               mBidLevelDeltas;
        BasicDeltaMap*                               mAskLevelDeltas;

        DeltaListMap*                                mBidEntryDeltas;
        DeltaListMap*                                mAskEntryDeltas;
        
        mutable BasicDeltaList*                      mDeltas;
        MamdaOrderBook*                              mBook;
        MamdaOrderBookBasicDeltaList::ModifiedSides  mModSides;
        bool                                         mKeepDeltas;
        bool                                         mConflateDeltas;
        bool                                         mProcessEntries;
        bool                                         mSendImmediately;
        mama_size_t                                  mSize;
    }; 

    MamdaOrderBookBasicDeltaList::MamdaOrderBookBasicDeltaList()
        : mImpl (*new MamdaOrderBookBasicDeltaListImpl)
    {
    }

    MamdaOrderBookBasicDeltaList::~MamdaOrderBookBasicDeltaList()
    {
        clear();
        delete mImpl.mDeltas;  // can safely be NULL
        delete &mImpl;
    }

    void MamdaOrderBookBasicDeltaList::clear()
    {
        if (mImpl.mDeltas)
        {
            BasicDeltaList::iterator end = mImpl.mDeltas->end();
            BasicDeltaList::iterator i   = mImpl.mDeltas->begin();
            for (; i != end; ++i)
            {
                delete *i;
            }
            mImpl.mDeltas->clear();
        }
        else if (mImpl.mBidLevelDeltas)
        {
            BasicDeltaMap::iterator end = mImpl.mBidLevelDeltas->end();
            BasicDeltaMap::iterator i   = mImpl.mBidLevelDeltas->begin();
            for (; i != end; ++i)
            {
                MamdaOrderBookBasicDelta* delta = i->second;
                delete delta;
            }
            mImpl.mBidLevelDeltas->clear();
            
            end = mImpl.mAskLevelDeltas->end();
            i   = mImpl.mAskLevelDeltas->begin();
            for (; i != end; ++i)
            {
                MamdaOrderBookBasicDelta* delta = i->second;
                delete delta;
            }
            mImpl.mAskLevelDeltas->clear();   
        }
        else if (mImpl.mBidEntryDeltas)
        {
            DeltaListMap::iterator levelEnd  = mImpl.mBidEntryDeltas->end();
            DeltaListMap::iterator levelIter = mImpl.mBidEntryDeltas->begin();
            for (; levelIter != levelEnd; ++levelIter)
            {
                BasicDeltaList*          deltas = levelIter->second;
                BasicDeltaList::iterator end    = deltas->end();
                BasicDeltaList::iterator i      = deltas->begin();
                for (; i != end; ++i)
                {
                    delete *i;
                }
                deltas->clear();
                delete deltas;
            }
            mImpl.mBidEntryDeltas->clear();
            
            levelEnd  = mImpl.mAskEntryDeltas->end();
            levelIter = mImpl.mAskEntryDeltas->begin();
            for (; levelIter != levelEnd; ++levelIter)
            {
                BasicDeltaList*          deltas = levelIter->second;
                BasicDeltaList::iterator end    = deltas->end();
                BasicDeltaList::iterator i      = deltas->begin();
                for (; i != end; ++i)
                {
                    delete *i;
                }
                deltas->clear();
                delete deltas;
            }
            mImpl.mAskEntryDeltas->clear();
        }
        mImpl.mBook = NULL;
        mImpl.mModSides = MamdaOrderBookBasicDeltaList::MOD_SIDES_NONE;
        mImpl.mSendImmediately = false;
        mImpl.mSize = 0;
    }

    void MamdaOrderBookBasicDeltaList::add (
        MamdaOrderBookEntry*              entry,
        MamdaOrderBookPriceLevel*         level,
        mama_quantity_t                   plDeltaSize,
        MamdaOrderBookPriceLevel::Action  plAction,
        MamdaOrderBookEntry::Action       entryAction)
    {
        if (mImpl.mKeepDeltas)
        {
            if (mImpl.mConflateDeltas)
            {
                if (mImpl.mProcessEntries)
                {
                    mImpl.conflateEntryDeltas (entry, level, plDeltaSize,
                                                plAction, entryAction);
                }
                else
                {
                    mImpl.conflateLevelDeltas (entry, level, plDeltaSize,
                                                plAction, entryAction);
                }
            }   
            else
            {
                MamdaOrderBookBasicDelta* basicDelta = new MamdaOrderBookBasicDelta;
                basicDelta->set (entry, level, plDeltaSize, plAction, entryAction);
                
                if (!mImpl.mDeltas)
                    mImpl.mDeltas = new BasicDeltaList;
                mImpl.mDeltas->push_back (basicDelta);
            }
        }
        mImpl.checkSide (level);
    }

    void MamdaOrderBookBasicDeltaList::add (
        const MamdaOrderBookBasicDelta&  delta)
    {
        if (mImpl.mKeepDeltas)
        {
            if (mImpl.mConflateDeltas)
            {
                if (mImpl.mProcessEntries)
                {
                    mImpl.conflateEntryDeltas (delta.getEntry(),
                                               delta.getPriceLevel(),
                                               delta.getPlDeltaSize(),
                                               delta.getPlDeltaAction(),
                                               delta.getEntryDeltaAction());
                }
                else
                {
                    mImpl.conflateLevelDeltas (delta.getEntry(),
                                               delta.getPriceLevel(),
                                               delta.getPlDeltaSize(),
                                               delta.getPlDeltaAction(),
                                               delta.getEntryDeltaAction());
                }
            }
            else
            {
                MamdaOrderBookBasicDelta*  basicDelta =
                    new MamdaOrderBookBasicDelta (delta);
                if (!mImpl.mDeltas)
                    mImpl.mDeltas = new BasicDeltaList;
                mImpl.mDeltas->push_back (basicDelta);
            }
        }
        mImpl.checkSide (delta.getPriceLevel());
    }

    void MamdaOrderBookBasicDeltaList::setOrderBook (MamdaOrderBook* book)
    {
        mImpl.mBook = book;
    }

    MamdaOrderBook* MamdaOrderBookBasicDeltaList::getOrderBook () const
    {
        return mImpl.mBook;
    }

    mama_size_t MamdaOrderBookBasicDeltaList::getSize() const
    {
        if (mImpl.mDeltas)
        {  
            return mImpl.mDeltas->size();
        }
        else
        {
            return mImpl.mSize;
        }
    }

    void MamdaOrderBookBasicDeltaList::setKeepBasicDeltas (bool  keep)
    {
        mImpl.mKeepDeltas = keep;
    }

    bool MamdaOrderBookBasicDeltaList::getSendImmediately()
    {
        return mImpl.mSendImmediately;
    }

    MamdaOrderBookBasicDeltaList::ModifiedSides
    MamdaOrderBookBasicDeltaList::getModifiedSides () const
    {
        return mImpl.mModSides;
    }

    void MamdaOrderBookBasicDeltaList::fixPriceLevelActions()
    {
        if (mImpl.mDeltas)
        {
            {
                // First change all actions where first is an add.
                MamdaOrderBookBasicDelta*  firstSamePlDelta = NULL;
                BasicDeltaList::iterator end = mImpl.mDeltas->end();
                BasicDeltaList::iterator i   = mImpl.mDeltas->begin();
                for (; i != end; ++i)
                {
                    MamdaOrderBookBasicDelta* delta = *i;
                    MamdaOrderBookPriceLevel* pl = delta->getPriceLevel();
                    if (firstSamePlDelta &&
                        (pl == firstSamePlDelta->getPriceLevel()))
                    {
                        if (firstSamePlDelta->getPlDeltaAction() ==
                            MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD)
                        {
                            delta->setPlDeltaAction (
                                MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD);
                        }
                    }
                    else
                    {
                        firstSamePlDelta = delta;
                    }
                }
            }
            {
                // Next change all actions where last is a delete.
                MamdaOrderBookBasicDelta*  firstSamePlDelta = NULL;
                BasicDeltaList::reverse_iterator end = mImpl.mDeltas->rend();
                BasicDeltaList::reverse_iterator i   = mImpl.mDeltas->rbegin();
                for (; i != end; ++i)
                {
                    MamdaOrderBookBasicDelta* delta = *i;
                    MamdaOrderBookPriceLevel* pl = delta->getPriceLevel();
                    if (firstSamePlDelta &&
                        (pl == firstSamePlDelta->getPriceLevel()))
                    {
                        if (firstSamePlDelta->getPlDeltaAction() ==
                            MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_DELETE)
                        {
                            delta->setPlDeltaAction (
                                MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_DELETE);
                        }
                    }
                    else
                    {
                        firstSamePlDelta = delta;
                    }
                }
            }
        }
    }

    void MamdaOrderBookBasicDeltaList::setConflateDeltas (bool conflate)
    {
        mImpl.mConflateDeltas = conflate;
    }

    void MamdaOrderBookBasicDeltaList::setProcessEntries (bool process)
    {
        mImpl.mProcessEntries = process;
    }

    void MamdaOrderBookBasicDeltaList::dump (ostream&  output) const
    {
        output << "ComplexUpdate:\n";

        MamdaOrderBookBasicDeltaList::iterator end = this->end();
        MamdaOrderBookBasicDeltaList::iterator i   = this->begin();
        for (; i != end; ++i)
        {
            MamdaOrderBookBasicDelta* delta = *i;
            output << "  ";
            delta->dump(output);
        }
    }

    MamdaOrderBookBasicDeltaList::MamdaOrderBookBasicDeltaListImpl::MamdaOrderBookBasicDeltaListImpl()
        : mBidLevelDeltas  (NULL)
        , mAskLevelDeltas  (NULL)
        , mBidEntryDeltas  (NULL)
        , mAskEntryDeltas  (NULL)
        , mDeltas          (NULL)
        , mBook            (NULL)
        , mModSides        (MamdaOrderBookBasicDeltaList::MOD_SIDES_NONE)
        , mKeepDeltas      (true)
        , mConflateDeltas  (false)
        , mProcessEntries  (true)
        , mSendImmediately (false)
    {
    }

    void MamdaOrderBookBasicDeltaList::MamdaOrderBookBasicDeltaListImpl::checkSide(
        const MamdaOrderBookPriceLevel*  level)
    {
        if (!level)
            return;
        if (level->getSide() == MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID)
        {
            switch (mModSides)
            {
            case MamdaOrderBookBasicDeltaList::MOD_SIDES_NONE:
                mModSides = MamdaOrderBookBasicDeltaList::MOD_SIDES_BID;
                break;
            case MamdaOrderBookBasicDeltaList::MOD_SIDES_ASK:
                mModSides = MamdaOrderBookBasicDeltaList::MOD_SIDES_BID_AND_ASK;
                break;
            default:
                break;
            }
        }
        else
        {
            switch (mModSides)
            {
            case MamdaOrderBookBasicDeltaList::MOD_SIDES_NONE:
                mModSides = MamdaOrderBookBasicDeltaList::MOD_SIDES_ASK;
                break;
            case MamdaOrderBookBasicDeltaList::MOD_SIDES_BID:
                mModSides = MamdaOrderBookBasicDeltaList::MOD_SIDES_BID_AND_ASK;
                break;
            default:
                break;
            }
        }
    }

    void
    MamdaOrderBookBasicDeltaList::MamdaOrderBookBasicDeltaListImpl::conflateEntryDeltas (
        MamdaOrderBookEntry*              entry,
        MamdaOrderBookPriceLevel*         level,
        mama_quantity_t                   plDeltaSize,
        MamdaOrderBookPriceLevel::Action  plAction,
        MamdaOrderBookEntry::Action       entryAction)
    {
        if (!mBidEntryDeltas)
            mBidEntryDeltas = new DeltaListMap;
        if (!mAskEntryDeltas)
            mAskEntryDeltas = new DeltaListMap;
            
        double price = level->getPrice();
        DeltaListMap* deltaSide =
            MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID == level->getSide()
            ? mBidEntryDeltas
            : mAskEntryDeltas;
                 
        DeltaListMap::iterator found = deltaSide->find (price);
        
        /* No deltas for this price level */
        if (found == deltaSide->end())
        {
            BasicDeltaList*  deltaList = new BasicDeltaList;
            deltaSide->insert (DeltaListMap::value_type(price,deltaList));

            addEntryDelta (*deltaList, entry, level,
                           plDeltaSize, plAction, entryAction);
            return;
        }
        
        BasicDeltaList*  deltaList = found->second;
        if (0 == deltaList->size())
        {
            addEntryDelta (*deltaList, entry, level,
                           plDeltaSize, plAction, entryAction);
            return;
        }
        
        /* Existing deltas for this level */
        switch (plAction)
        {
            case MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_DELETE:
            {
                BasicDeltaList::iterator end = deltaList->end();
                BasicDeltaList::iterator i   = deltaList->begin();
                
                MamdaOrderBookBasicDelta* basicDelta = *i;
                
                MamdaOrderBookPriceLevel::Action  firstAction =
                            basicDelta->getPlDeltaAction();
                for (; i != end; ++i)
                {
                    MamdaOrderBookBasicDelta* delta = *i;
                    delete delta;
                }
                mSize -= deltaList->size();
                deltaList->clear();
                /* If this level was not added during this conflation interval */
                if (MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD != firstAction)
                {
                    /* NULL entry because that way the client will delete all
                    entries */
                    addEntryDelta (*deltaList, NULL, level,
                                    plDeltaSize, plAction, entryAction);
                    mSendImmediately = true;
                    
                }
             
                break;
            }
            case MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD:
            {
                addEntryDelta (*deltaList, entry, level,
                                plDeltaSize, plAction, entryAction);
                break;
            }
            case MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_UPDATE:
            {
                applyEntryDelta (*deltaList, entry, level,
                                 plDeltaSize, plAction, entryAction);
                break;
            }
            case MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_UNKNOWN:
                break;
        }
    }

    void
    MamdaOrderBookBasicDeltaList::MamdaOrderBookBasicDeltaListImpl::addEntryDelta (
        BasicDeltaList&                   deltaList,
        MamdaOrderBookEntry*              entry,
        MamdaOrderBookPriceLevel*         level,
        mama_quantity_t                   plDeltaSize,
        MamdaOrderBookPriceLevel::Action  plAction,
        MamdaOrderBookEntry::Action       entryAction)
    {
        MamdaOrderBookBasicDelta* basicDelta = new MamdaOrderBookBasicDelta;
        basicDelta->set (entry, level, plDeltaSize, plAction, entryAction);
        deltaList.push_back (basicDelta);
        ++mSize; 
    } 

    /* Maybe change to a findOrCreate if needed */
    void
    MamdaOrderBookBasicDeltaList::MamdaOrderBookBasicDeltaListImpl::applyEntryDelta (
        BasicDeltaList&                   deltaList,
        MamdaOrderBookEntry*              entry,
        MamdaOrderBookPriceLevel*         level,
        mama_quantity_t                   plDeltaSize,
        MamdaOrderBookPriceLevel::Action  plAction,
        MamdaOrderBookEntry::Action       entryAction)
    {
        BasicDeltaList::iterator end = deltaList.end();
        BasicDeltaList::iterator i   = deltaList.begin();

        for (; i != end; ++i)
        {
            MamdaOrderBookBasicDelta* delta = *i;

            if (delta->getEntry() == entry)
            {
                MamdaOrderBookEntry::Action existingAction =
                        delta->getEntryDeltaAction();
                switch (entryAction)
                {
                    case MamdaOrderBookEntry::MAMDA_BOOK_ACTION_DELETE:
                    {
                        deltaList.erase (i);
                        delete delta;
                        --mSize; 
                        /* Do not add new entry if the existing one was added
                           this conflation interval */ 
                        if (MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD
                           != existingAction)
                        {   
                            addEntryDelta (deltaList, entry, level,
                                       plDeltaSize, plAction, entryAction);
                        }
                        return;
                    }
                    case MamdaOrderBookEntry::MAMDA_BOOK_ACTION_UPDATE:
                    case MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD:
                    { 
                        deltaList.erase (i);
                        delete delta;
                        --mSize; 
                        addEntryDelta (deltaList, entry, level,
                                       plDeltaSize, plAction, entryAction);
                        return;
                    }
                    case MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_UNKNOWN:
                        continue;
                }
            }
        }
        /* If not found then add the new delta */
        addEntryDelta (deltaList, entry, level,
                       plDeltaSize, plAction, entryAction);
        return;
    } 

    void
    MamdaOrderBookBasicDeltaList::MamdaOrderBookBasicDeltaListImpl::conflateLevelDeltas (
        MamdaOrderBookEntry*              entry,
        MamdaOrderBookPriceLevel*         level,
        mama_quantity_t                   plDeltaSize,
        MamdaOrderBookPriceLevel::Action  plAction,
        MamdaOrderBookEntry::Action       entryAction)
    {
        if (!mBidLevelDeltas)
            mBidLevelDeltas = new BasicDeltaMap;
        if (!mAskLevelDeltas)
            mAskLevelDeltas = new BasicDeltaMap;

        double price = level->getPrice();
        BasicDeltaMap* deltaSide =
            MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID == level->getSide()
            ? mBidLevelDeltas
            : mAskLevelDeltas;
                 
        BasicDeltaMap::iterator found = deltaSide->find(price);
        /* No deltas for this price level */
        if (found == deltaSide->end())
        {       
            MamdaOrderBookBasicDelta* basicDelta = new MamdaOrderBookBasicDelta;
            basicDelta->set (entry, level, plDeltaSize, plAction, entryAction);
            
            deltaSide->insert (BasicDeltaMap::value_type(price,basicDelta));
            ++mSize;
        }
        else
        {
            MamdaOrderBookBasicDelta* existingDelta = found->second;

            switch (plAction)
            {
                case MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_DELETE:
                    if (MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD ==
                        existingDelta->getPlDeltaAction())
                    {
                        delete existingDelta;
                        deltaSide->erase (found);
                         --mSize; 
                    }
                    else
                    {
                        existingDelta->setPlDeltaAction (
                            MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_DELETE);
                    }
                    break;
                case MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD:
                    existingDelta->setPlDeltaAction (
                            MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_UPDATE);
                          
                case MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_UPDATE: /* Fall through */
                    existingDelta->applyPlDeltaSize (plDeltaSize);
                    existingDelta->setPriceLevel (level);
                    break;
                case MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_UNKNOWN:
                    break;
            }
        }
    }

    struct MamdaOrderBookBasicDeltaList::iterator::iteratorImpl
    {
        iteratorImpl (BasicDeltaList*  deltas,
                      BasicDeltaMap*   bidDeltas,
                      BasicDeltaMap*   askDeltas,
                      DeltaListMap*    bidEntryDeltas,
                      DeltaListMap*    askEntryDeltas)
            : mDeltas          (deltas)
            , mDeltaIterator   (NULL)
            , mBidLevelDeltas  (bidDeltas)
            , mAskLevelDeltas  (askDeltas)
            , mLevelIterator   (NULL)
            , mBidEntryDeltas  (bidEntryDeltas)
            , mAskEntryDeltas  (askEntryDeltas)
            , mMapIterator     (NULL)
            , mEntryIterator   (NULL)
            {}
            
        iteratorImpl (const iteratorImpl& copy)
            : mDeltas          (copy.mDeltas)
            , mDeltaIterator   (NULL)
            , mBidLevelDeltas  (copy.mBidLevelDeltas)
            , mAskLevelDeltas  (copy.mAskLevelDeltas)
            , mLevelIterator   (NULL)
            , mBidEntryDeltas  (copy.mBidEntryDeltas)
            , mAskEntryDeltas  (copy.mAskEntryDeltas)
            , mMapIterator     (NULL)
            {
                if (copy.mDeltaIterator)
                {
                    mDeltaIterator = new BasicDeltaList::iterator (*copy.mDeltaIterator);
                    mEntryIterator = NULL;
                }
                else if (copy.mLevelIterator)
                {
                    mLevelIterator = new BasicDeltaMap::iterator (*copy.mLevelIterator);
                    mEntryIterator = NULL;
                }
                else if (copy.mMapIterator)
                {
                    mMapIterator   = new DeltaListMap::iterator (*copy.mMapIterator);
                    if (*mMapIterator != mAskEntryDeltas->end())
                    {  
                        mEntryIterator = new BasicDeltaList::iterator (
                                            (*mMapIterator)->second->begin ());
                    }
                    else
                    {
                        mEntryIterator = NULL;
                    }
                }
            }
            
        iteratorImpl (BasicDeltaList*           deltas,
                      BasicDeltaList::iterator  iter)
            : mDeltas          (deltas)
            , mDeltaIterator   (new BasicDeltaList::iterator (iter))
            , mBidLevelDeltas  (NULL)
            , mAskLevelDeltas  (NULL)  
            , mLevelIterator   (NULL)
            , mBidEntryDeltas  (NULL)
            , mAskEntryDeltas  (NULL)
            , mMapIterator     (NULL)
            , mEntryIterator   (NULL)
            {}
            
        iteratorImpl (BasicDeltaMap*            bidDeltas,
                      BasicDeltaMap*            askDeltas,
                      BasicDeltaMap::iterator   iter)
            : mDeltas          (NULL)
            , mDeltaIterator   (NULL)
            , mBidLevelDeltas  (bidDeltas)
            , mAskLevelDeltas  (askDeltas)
            , mLevelIterator   (new BasicDeltaMap::iterator(iter))
            , mBidEntryDeltas  (NULL)
            , mAskEntryDeltas  (NULL)
            , mMapIterator     (NULL)
            , mEntryIterator   (NULL)
            {}

        iteratorImpl (DeltaListMap*            bidDeltas,
                      DeltaListMap*            askDeltas,
                      DeltaListMap::iterator   iter)
            : mDeltas          (NULL)
            , mDeltaIterator   (NULL)
            , mBidLevelDeltas  (NULL)
            , mAskLevelDeltas  (NULL)
            , mLevelIterator   (NULL)
            , mBidEntryDeltas  (bidDeltas)
            , mAskEntryDeltas  (askDeltas)
            , mMapIterator     (new DeltaListMap::iterator(iter))
            {
                mEntryIterator = NULL;
                while ((*mMapIterator != mAskEntryDeltas->end()) &&
                       (0 == (*mMapIterator)->second->size()))
                {
                    ++(*mMapIterator);
                    if  (*mMapIterator == mBidEntryDeltas->end())
                    {
                        *mMapIterator = mAskEntryDeltas->begin();
                    }
                }
                
                if (*mMapIterator != mAskEntryDeltas->end())
                {
                    mEntryIterator = new BasicDeltaList::iterator (
                                        (*mMapIterator)->second->begin ());
                }
                else
                {
                    mEntryIterator = NULL;
                }
            }
            
        ~iteratorImpl ()
        {
            if (mDeltaIterator)
            {
                delete mDeltaIterator;
                mDeltaIterator = NULL;
            }

            if (mLevelIterator)
            {
                delete mLevelIterator;
                mLevelIterator = NULL;    

            }

            if (mMapIterator)
            {
                delete mMapIterator;
                mMapIterator = NULL;
            }

            if (mEntryIterator)
            {
                delete mEntryIterator;
                mEntryIterator = NULL;    

            }
        }
        
        BasicDeltaList* const       mDeltas;
        BasicDeltaList::iterator*   mDeltaIterator;

        BasicDeltaMap*  const       mBidLevelDeltas;
        BasicDeltaMap*  const       mAskLevelDeltas;
        BasicDeltaMap::iterator*    mLevelIterator;

        DeltaListMap*   const       mBidEntryDeltas;
        DeltaListMap*   const       mAskEntryDeltas;
        DeltaListMap::iterator*     mMapIterator;
        BasicDeltaList::iterator*   mEntryIterator;
    };

    MamdaOrderBookBasicDeltaList::iterator::iterator ()
        : mImpl (*new iteratorImpl(mImpl.mDeltas,
                                   mImpl.mBidLevelDeltas, 
                                   mImpl.mAskLevelDeltas,
                                   mImpl.mBidEntryDeltas,
                                   mImpl.mAskEntryDeltas))
    {
    }

    MamdaOrderBookBasicDeltaList::iterator::iterator (const iterator& copy)
        : mImpl (*new iteratorImpl(copy.mImpl))
    {
    }

    MamdaOrderBookBasicDeltaList::iterator::iterator (const iteratorImpl& copy)
        : mImpl (*new iteratorImpl(copy))
    {
    }

    MamdaOrderBookBasicDeltaList::iterator::iterator (iteratorImpl& copy)
        : mImpl (*new iteratorImpl(copy))
    {
    }

    MamdaOrderBookBasicDeltaList::iterator::~iterator ()
    {
        delete &mImpl;
    }

    MamdaOrderBookBasicDeltaList::iterator MamdaOrderBookBasicDeltaList::begin()
    {
        if (!mImpl.mKeepDeltas)
            throw MamdaOrderBookException (
                "attempt to iterate over unsaved basic deltas");
        if (mImpl.mConflateDeltas)
        {
            if (mImpl.mProcessEntries)
            {
                if (!mImpl.mBidEntryDeltas)
                    mImpl.mBidEntryDeltas = new DeltaListMap;
                if (!mImpl.mAskEntryDeltas)
                    mImpl.mAskEntryDeltas = new DeltaListMap;
        
                if (0 != mImpl.mBidEntryDeltas->size())
                {
                    return iterator(iterator::iteratorImpl(
                                                mImpl.mBidEntryDeltas,
                                                mImpl.mAskEntryDeltas,
                                                mImpl.mBidEntryDeltas->begin()));
                }
                else
                {
                    return iterator(iterator::iteratorImpl(
                                                mImpl.mBidEntryDeltas,
                                                mImpl.mAskEntryDeltas,
                                                mImpl.mAskEntryDeltas->begin()));
                }
            }
            else
            {
                if (!mImpl.mBidLevelDeltas)
                    mImpl.mBidLevelDeltas = new BasicDeltaMap;
                if (!mImpl.mAskLevelDeltas)
                    mImpl.mAskLevelDeltas = new BasicDeltaMap;
        
                if (0 != mImpl.mBidLevelDeltas->size())
                {
                    return iterator(iterator::iteratorImpl(
                                                mImpl.mBidLevelDeltas,
                                                mImpl.mAskLevelDeltas,
                                                mImpl.mBidLevelDeltas->begin()));
                }
                else
                {
                    return iterator(iterator::iteratorImpl(
                                                    mImpl.mBidLevelDeltas,
                                                    mImpl.mAskLevelDeltas,
                                                    mImpl.mAskLevelDeltas->begin()));
                }
            }
        }
        else
        {
            if (!mImpl.mDeltas)
                mImpl.mDeltas = new BasicDeltaList;
            return iterator(iterator::iteratorImpl(mImpl.mDeltas,
                                                mImpl.mDeltas->begin()));
        }
    }

    MamdaOrderBookBasicDeltaList::iterator MamdaOrderBookBasicDeltaList::end()
    {
        if (!mImpl.mKeepDeltas)
            throw MamdaOrderBookException (
                "attempt to iterate over unsaved basic deltas");
        if (mImpl.mConflateDeltas)
        {
            if (mImpl.mProcessEntries)
            {
                if (!mImpl.mBidEntryDeltas)
                    mImpl.mBidEntryDeltas = new DeltaListMap;
                if (!mImpl.mAskEntryDeltas)
                    mImpl.mAskEntryDeltas = new DeltaListMap;
        
                return iterator(iterator::iteratorImpl(mImpl.mBidEntryDeltas,
                                                mImpl.mAskEntryDeltas,
                                                mImpl.mAskEntryDeltas->end()));
            }
            else
            {
                if (!mImpl.mBidLevelDeltas)
                    mImpl.mBidLevelDeltas = new BasicDeltaMap;
                if (!mImpl.mAskLevelDeltas)
                    mImpl.mAskLevelDeltas = new BasicDeltaMap;
                return iterator(iterator::iteratorImpl(mImpl.mBidLevelDeltas,
                                                mImpl.mAskLevelDeltas,
                                                mImpl.mAskLevelDeltas->end()));
            }
        }
        else
        {
            if (!mImpl.mDeltas)
                mImpl.mDeltas = new BasicDeltaList;
            return iterator(iterator::iteratorImpl(mImpl.mDeltas,
                                                mImpl.mDeltas->end()));
        }
    }

    MamdaOrderBookBasicDeltaList::const_iterator MamdaOrderBookBasicDeltaList::begin() const
    {
        if (!mImpl.mKeepDeltas)
            throw MamdaOrderBookException (
                "attempt to iterate over unsaved basic deltas");
        if (mImpl.mConflateDeltas)
        {
            if (mImpl.mProcessEntries)
            {
                if (!mImpl.mBidEntryDeltas)
                    mImpl.mBidEntryDeltas = new DeltaListMap;
                if (!mImpl.mAskEntryDeltas)
                    mImpl.mAskEntryDeltas = new DeltaListMap;

                if (0 != mImpl.mBidEntryDeltas->size())
                {
                    return const_iterator(iterator::iteratorImpl(
                                                mImpl.mBidEntryDeltas,
                                                mImpl.mAskEntryDeltas,
                                                mImpl.mBidEntryDeltas->begin()));
                }
                else
                {
                    return const_iterator(iterator::iteratorImpl(
                                                mImpl.mBidEntryDeltas,
                                                mImpl.mAskEntryDeltas,
                                                mImpl.mAskEntryDeltas->begin()));
                }
            }
            else
            {

                if (!mImpl.mBidLevelDeltas)
                    mImpl.mBidLevelDeltas = new BasicDeltaMap;
                if (!mImpl.mAskLevelDeltas)
                    mImpl.mAskLevelDeltas = new BasicDeltaMap;
                
                /* Start at the ask side if the bid side is empty */
                if (0 != mImpl.mBidLevelDeltas->size())
                {
                    return const_iterator(iterator::iteratorImpl(
                                                    mImpl.mBidLevelDeltas,
                                                    mImpl.mAskLevelDeltas,
                                                    mImpl.mBidLevelDeltas->begin()));
                }
                else
                {
                    return const_iterator(iterator::iteratorImpl(
                                                    mImpl.mBidLevelDeltas,
                                                    mImpl.mAskLevelDeltas,
                                                    mImpl.mAskLevelDeltas->begin()));
                }
            }
        }
        else
        {
            if (!mImpl.mDeltas)
                mImpl.mDeltas = new BasicDeltaList;
            return const_iterator(iterator::iteratorImpl(mImpl.mDeltas,
                                                mImpl.mDeltas->begin()));
        }
    }

    MamdaOrderBookBasicDeltaList::const_iterator MamdaOrderBookBasicDeltaList::end() const
    {
        if (!mImpl.mKeepDeltas)
            throw MamdaOrderBookException (
                "attempt to iterate over unsaved basic deltas");
        if (mImpl.mConflateDeltas)
        {
            if (mImpl.mProcessEntries)
            {
                if (!mImpl.mBidEntryDeltas)
                    mImpl.mBidEntryDeltas = new DeltaListMap;
                if (!mImpl.mAskEntryDeltas)
                    mImpl.mAskEntryDeltas = new DeltaListMap;
                return const_iterator(iterator::iteratorImpl(mImpl.mBidEntryDeltas,
                                                mImpl.mAskEntryDeltas,
                                                mImpl.mAskEntryDeltas->end()));
            }
            else
            {
                if (!mImpl.mBidLevelDeltas)
                    mImpl.mBidLevelDeltas = new BasicDeltaMap;
                if (!mImpl.mAskLevelDeltas)
                    mImpl.mAskLevelDeltas = new BasicDeltaMap;
                return const_iterator(iterator::iteratorImpl(mImpl.mBidLevelDeltas,
                                                mImpl.mAskLevelDeltas,
                                                mImpl.mAskLevelDeltas->end()));
            }
           
        }
        else
        {
            if (!mImpl.mDeltas)
                mImpl.mDeltas = new BasicDeltaList;
            return const_iterator (iterator::iteratorImpl (mImpl.mDeltas,
                                                mImpl.mDeltas->end()));
        }
    }

    MamdaOrderBookBasicDeltaList::iterator&
    MamdaOrderBookBasicDeltaList::iterator::operator= (const iterator& rhs)
    {  
        if (&rhs != this)
        {
            if (rhs.mImpl.mDeltaIterator)
            {
                mImpl.mDeltaIterator = rhs.mImpl.mDeltaIterator;
            }
            else if (rhs.mImpl.mLevelIterator)
            {
                mImpl.mLevelIterator = rhs.mImpl.mLevelIterator;
            }
            else if (rhs.mImpl.mMapIterator)
            {
                mImpl.mMapIterator = rhs.mImpl.mMapIterator;
            }
        }
        return *this;
    }

    const MamdaOrderBookBasicDeltaList::iterator&
    MamdaOrderBookBasicDeltaList::iterator::operator++ () const
    {
        if (mImpl.mDeltaIterator)
        {
            if (*mImpl.mDeltaIterator != mImpl.mDeltas->end())
            {
                ++(*mImpl.mDeltaIterator);
            }
        }
        else if (mImpl.mLevelIterator)
        {
            if (*mImpl.mLevelIterator != mImpl.mAskLevelDeltas->end())
            {
                ++(*mImpl.mLevelIterator);
                 if (*mImpl.mLevelIterator == mImpl.mBidLevelDeltas->end())
                 {
                    *mImpl.mLevelIterator = mImpl.mAskLevelDeltas->begin();
                 }
            }
        }
        else if (mImpl.mMapIterator)
        {
            if (*mImpl.mMapIterator == mImpl.mAskEntryDeltas->end())
            {  
                return *this;
            }    
            ++(*mImpl.mEntryIterator);
            /* If this entry delta is the last one for this level */
            if ((*mImpl.mEntryIterator) == (*mImpl.mMapIterator)->second->end ())
            {  
                do
                {
                    ++(*mImpl.mMapIterator);
        
                    if  ((*mImpl.mMapIterator) == mImpl.mBidEntryDeltas->end())
                    {   
                        *mImpl.mMapIterator = mImpl.mAskEntryDeltas->begin();
                    }
                } /* Ignore empty levels */
                while ((*mImpl.mMapIterator != mImpl.mAskEntryDeltas->end()) &&
                       (0 == (*mImpl.mMapIterator)->second->size()));
                      
                if (*mImpl.mMapIterator != mImpl.mAskEntryDeltas->end())
                {    
                    *mImpl.mEntryIterator = (*mImpl.mMapIterator)->second->begin ();
                }
            }   
        }
        return *this;
    }

    MamdaOrderBookBasicDeltaList::iterator&
    MamdaOrderBookBasicDeltaList::iterator::operator++ ()
    {
        if (mImpl.mDeltaIterator)
        {
            if (*mImpl.mDeltaIterator != mImpl.mDeltas->end())
            {
                ++(*mImpl.mDeltaIterator);
            }
        }
        else if (mImpl.mLevelIterator)
        {
            if (*mImpl.mLevelIterator != mImpl.mAskLevelDeltas->end())
            {
                ++(*mImpl.mLevelIterator);
                 if (*mImpl.mLevelIterator == mImpl.mBidLevelDeltas->end())
                 {
                    *mImpl.mLevelIterator = mImpl.mAskLevelDeltas->begin();
                 }
            }
        }
        else if (mImpl.mMapIterator)
        {
            if (*mImpl.mMapIterator == mImpl.mAskEntryDeltas->end())
            {  
                return *this;
            }    
            ++(*mImpl.mEntryIterator);
            /* If this entry delta is the last one for this level */
            if ((*mImpl.mEntryIterator) == (*mImpl.mMapIterator)->second->end ())
            {  
                do
                {
                    ++(*mImpl.mMapIterator);
        
                    if  ((*mImpl.mMapIterator) == mImpl.mBidEntryDeltas->end())
                    {
                        *mImpl.mMapIterator = mImpl.mAskEntryDeltas->begin();
                    }
                } /* Ignore empty levels */
                while ((*mImpl.mMapIterator != mImpl.mAskEntryDeltas->end()) &&
                       (0 == (*mImpl.mMapIterator)->second->size()));
                      
                if (*mImpl.mMapIterator != mImpl.mAskEntryDeltas->end())
                { 
                    *mImpl.mEntryIterator = (*mImpl.mMapIterator)->second->begin ();
                }
            }   
        }
        return *this;
    }

    bool MamdaOrderBookBasicDeltaList::iterator::operator== (const iterator& rhs) const
    {
        if (&rhs == this)
            return true;
        else if (mImpl.mDeltaIterator)
            return (*mImpl.mDeltaIterator == *rhs.mImpl.mDeltaIterator);
        else if (mImpl.mLevelIterator)
            return (*mImpl.mLevelIterator == *rhs.mImpl.mLevelIterator);
        else if (mImpl.mMapIterator)
        {
            if ((rhs.mImpl.mEntryIterator) &&
                (*mImpl.mEntryIterator ==  *rhs.mImpl.mEntryIterator))
                return true;
                
            /* special case of the end interator*/
            return  (mImpl.mAskEntryDeltas->end() == *mImpl.mMapIterator &&
                     mImpl.mAskEntryDeltas->end() == *rhs.mImpl.mMapIterator);
        }
        else
            return false;
    }

    bool MamdaOrderBookBasicDeltaList::iterator::operator!= (const iterator& rhs) const
    {
        return !operator==(rhs);
    }

    MamdaOrderBookBasicDelta*
    MamdaOrderBookBasicDeltaList::iterator::operator*()
    {

        if (mImpl.mDeltaIterator)
        {   
            return *(*mImpl.mDeltaIterator);
        }
        else if (mImpl.mLevelIterator)
        {  
           return (*(*mImpl.mLevelIterator)).second;
        }
        else if (mImpl.mEntryIterator)
        {  
           return *(*mImpl.mEntryIterator);
        }  
        return NULL;
    }

    const MamdaOrderBookBasicDelta*
    MamdaOrderBookBasicDeltaList::iterator::operator*() const
    {  
        if (mImpl.mDeltaIterator)
        {      
            return *(*mImpl.mDeltaIterator);
        }
        else if (mImpl.mLevelIterator)
        {   
           return (*(*mImpl.mLevelIterator)).second;
        }
        else if (mImpl.mEntryIterator)
        {    
           return *(*mImpl.mEntryIterator);
        }  
        return NULL;
    }   

} // namespace
