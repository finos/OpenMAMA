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

#include "MamdaOrderBookWriter.h"
#include <mamda/MamdaOrderBookFields.h>
#include <mamda/MamdaCommonFields.h>
#include <mamda/MamdaOrderBook.h>
#include <iostream>
#include <wombat/wincompat.h>
#include <stdlib.h>
#include <stdio.h>

namespace Wombat
{

    static char defaultPlAction             = MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD;
    static char defaultPlSide               = MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID;
    static char defaultEntryAction          = MamdaOrderBookEntry::MAMDA_BOOK_ACTION_DELETE;

    static mama_u32_t  defaultNumEntries    = 1;
    static int  defaultNumAttachedEntries   = 1;

    BookMsgHolder::~BookMsgHolder()
    {
        if (mMsgVector)
        {
            for (size_t i = 0; i < mMsgVectorSize; i++)
            {
                delete mMsgVector[i];
                mMsgVector[i] = NULL;
            }
            delete [] mMsgVector;
            mMsgVector = NULL;
            mMsgVectorSize = 0;
        }
    }

    void BookMsgHolder::clear()
    {
        if (mMsgVector)
        {
            for (size_t i = 0; i < mMsgVectorSize; i++)
            {
                 if (mMsgVector[i]) mMsgVector[i]->clear();
            }
        }
    }

    void BookMsgHolder::grow (mama_size_t newSize)
    {
        if (mMsgVectorSize >= newSize)  return;

        MamaMsg** newVector  = new MamaMsg*[newSize];
        if (mMsgVector)
        {
            for (size_t i = 0; i < mMsgVectorSize; i++)
                newVector[i]  = mMsgVector[i];

        }
        for (size_t i = mMsgVectorSize; i < newSize; i++)
        {
            newVector[i] = new MamaMsg;
            newVector[i]->create();
        }        
        if (mMsgVector) delete [] mMsgVector;
        mMsgVector = NULL;
            
        mMsgVector  = newVector;
        mMsgVectorSize = newSize;
    }

    void BookMsgHolder::print()
    {        
        if (mMsgVector)
        {
            for (size_t i = 0; i < mMsgVectorSize; i++)
            {
                printf("\n mMsgVector[%d] = %s \n", i, mMsgVector[i]->toString());
            }

        }
    }


    struct MamdaOrderBookWriter::MamdaOrderBookWriterImpl
    {
        MamdaOrderBookWriterImpl();

        void    populateMsg         (MamaMsg& msg, const MamdaOrderBookComplexDelta& delta);
        void    populateMsg         (MamaMsg& msg, const MamdaOrderBookSimpleDelta& delta);
        void    populateMsg         (MamaMsg& msg, const MamdaOrderBook& book);
                                
      
        void    addComplexDeltaFields   (MamaMsg&                          msg,
                                         const MamdaOrderBookComplexDelta& delta);
                
        void    addSimpleDeltaFields    (MamaMsg&                          msg,
                                         const MamdaOrderBookSimpleDelta&  delta);
                                        
        void    addBookLevels           (MamaMsg&               msg,
                                         const MamdaOrderBook&  book);                       
        
        void    addBookLevel            (MamaMsg&                          plMsg,
                                         const MamdaOrderBookPriceLevel*   level,
                                         mama_f64_t                        plDeltaSize,
                                         MamdaOrderBookPriceLevel::Action  plDeltaAction,
                                         const MamaDateTime*               bookTime);
                                        
        void    addBookLevelEntries     (MamaMsg&                          plMsg,
                                         const MamdaOrderBookPriceLevel&   level);
        
        void    addBookEntry            (MamaMsg&                          msg,
                                         const MamdaOrderBookEntry*        entry,
                                         MamdaOrderBookEntry::Action       entryDeltaAction,
                                         const MamaDateTime*               plTime);
        
        BookMsgHolder*                  mPricelevels;
        BookMsgHolder*                  mEntries;
    };

    MamdaOrderBookWriter::MamdaOrderBookWriter()
        : mImpl (*new MamdaOrderBookWriterImpl)
    {
        mImpl.mPricelevels = new BookMsgHolder();
        mImpl.mEntries = new BookMsgHolder();
    }

    MamdaOrderBookWriter::~MamdaOrderBookWriter()
    {
        mImpl.mPricelevels->clear();
        mImpl.mEntries->clear();
        
        delete mImpl.mPricelevels;
        mImpl.mPricelevels = NULL; 

        delete mImpl.mEntries;
        mImpl.mEntries = NULL; 
        
        delete &mImpl;
    }

    void   MamdaOrderBookWriter::populateMsg (
        MamaMsg&                           msg,
        const MamdaOrderBookComplexDelta&  delta)
    {
        mImpl.populateMsg (msg, delta);
    }

    void   MamdaOrderBookWriter::populateMsg (
        MamaMsg&                          msg, 
        const MamdaOrderBookSimpleDelta&  delta)
    {
        mImpl.populateMsg (msg, delta);
    }

    void   MamdaOrderBookWriter::populateMsg (
        MamaMsg&               msg, 
        const MamdaOrderBook&  book)
    {
        mImpl.populateMsg (msg, book);
    }

    MamdaOrderBookWriter::MamdaOrderBookWriterImpl::MamdaOrderBookWriterImpl()
        : mPricelevels (NULL)
        , mEntries     (NULL)
    {
    }

    void MamdaOrderBookWriter::MamdaOrderBookWriterImpl::populateMsg (
        MamaMsg&                           msg, 
        const MamdaOrderBookComplexDelta&  delta)
    {
        mPricelevels->clear();
        mEntries->clear();

        msg.updateU8 (NULL, MamaFieldMsgType.mFid, MAMA_MSG_TYPE_BOOK_UPDATE);
        addComplexDeltaFields(msg, delta);
    }
        
    void MamdaOrderBookWriter::MamdaOrderBookWriterImpl::populateMsg (
        MamaMsg&                          msg, 
        const MamdaOrderBookSimpleDelta&  delta)
    {
        mPricelevels->clear();
        mEntries->clear();

        msg.updateU8 (NULL, MamaFieldMsgType.mFid, MAMA_MSG_TYPE_BOOK_UPDATE);
        addSimpleDeltaFields (msg, delta);
    }
        
    void MamdaOrderBookWriter::MamdaOrderBookWriterImpl::populateMsg (
        MamaMsg&               msg, 
        const MamdaOrderBook&  book)
    {  
        mPricelevels->clear();
        mEntries->clear();
         
        msg.updateU8 (NULL, MamaFieldMsgType.mFid, MAMA_MSG_TYPE_BOOK_INITIAL);
        addBookLevels (msg, book);
    }

    void MamdaOrderBookWriter::MamdaOrderBookWriterImpl::addSimpleDeltaFields (
        MamaMsg&                          msg,
        const MamdaOrderBookSimpleDelta&  delta)
    {
        MamdaOrderBookPriceLevel* pl = delta.getPriceLevel();
        mama_i16_t entryCount = 0;

        mPricelevels->grow(1);
                      
        addBookLevel (msg, pl,
                      delta.getPlDeltaSize(),
                      delta.getPlDeltaAction(),
                      &(delta.getEventTime()));

        if (delta.getEntry() != NULL)
        {
            entryCount = 1;
            addBookEntry (msg,
                          delta.getEntry(),
                          delta.getEntryDeltaAction(),
                          &(pl->getTime()));
        }

        msg.addI16 (NULL, MamdaOrderBookFields::NUM_LEVELS->getFid(),
                    (mama_i16_t)1);

        if (defaultNumAttachedEntries != entryCount)
        {
            msg.addI16 (NULL,
                        MamdaOrderBookFields::PL_NUM_ATTACH->getFid(),
                        (mama_i16_t)entryCount);
        }
        
        msg.addDateTime (NULL, MamdaOrderBookFields::BOOK_TIME->getFid(),
                         delta.getEventTime());
                         
        msg.addDateTime (NULL, MamdaCommonFields::SRC_TIME->getFid(),
                         delta.getSrcTime());
        
        if (delta.getEventSeqNum() != 0)
        {
            msg.addI64 (NULL, MamdaCommonFields::SEQ_NUM->getFid(),
                        (mama_i64_t)delta.getEventSeqNum());
        }

    }
    void MamdaOrderBookWriter::MamdaOrderBookWriterImpl::addComplexDeltaFields (
        MamaMsg&                            msg,
        const MamdaOrderBookComplexDelta&   delta)
    {
        // Make sure the reusable price level and entry submessage vectors
        // are large enough.  In the worst case, we would have one entry
        // in each of "delta-size" price levels or "delta-size" entries
        // within one price level, so let's make sure both vectors are
        // large enough.
        
        mPricelevels->grow(delta.getSize());
        mEntries->grow(delta.getSize());
                        
        MamdaOrderBookComplexDelta::iterator end = delta.end();
        MamdaOrderBookComplexDelta::iterator iter = delta.begin();
        
        MamdaOrderBookBasicDelta*  basicDelta       = NULL;
        MamdaOrderBookBasicDelta*  savedBasicDelta  = NULL;
        MamdaOrderBookPriceLevel*  pl               = NULL;
        MamdaOrderBookPriceLevel*  lastPl           = NULL;
        
        MamdaOrderBookPriceLevel*  flatEntryPl      = NULL;
        MamdaOrderBookBasicDelta*  flatEntryDelta   = NULL;
        
        size_t                     plCount    = 0;
        size_t                     entryCount = 0;
        
        for (; iter !=end; ++iter)
        {
            basicDelta = *iter;
            pl = basicDelta->getPriceLevel();
            
            if (lastPl != pl)
            {
                if (lastPl != NULL)
                {
                    MamaMsg& plMsg = *mPricelevels->mMsgVector[plCount];
               
                    addBookLevel  (plMsg, 
                                  lastPl, 
                                  savedBasicDelta->getPlDeltaSize(), 
                                  savedBasicDelta->getPlDeltaAction(),
                                  &(delta.getEventTime()));
                                      
                    if (1 == entryCount)
                    {
                        addBookEntry (plMsg,
                                     flatEntryDelta->getEntry(),
                                     flatEntryDelta->getEntryDeltaAction(), 
                                     &(flatEntryPl->getTime()));

                    }
                    if (entryCount > 1)
                    {
                        plMsg.addVectorMsg (NULL,
                                           MamdaOrderBookFields::PL_ENTRIES->getFid(),
                                           mEntries->mMsgVector,
                                           entryCount);
                    }
                    if (defaultNumAttachedEntries != entryCount)
                        plMsg.addI16 (NULL,
                                     MamdaOrderBookFields::PL_NUM_ATTACH->getFid(),
                                     (mama_i16_t) entryCount);
                    ++plCount;
                    entryCount = 0;
                }
                lastPl = pl;
                savedBasicDelta = basicDelta;
            }

            if (basicDelta->getEntry() != NULL)
            {
                MamaMsg& entMsg = *mEntries->mMsgVector[entryCount];

                addBookEntry (entMsg,
                            basicDelta->getEntry(),
                            basicDelta->getEntryDeltaAction(), 
                            &(pl->getTime()));
                ++entryCount;
                //save this delta and pl
                //used to flatten entry info into pl when no. of entries = 1
                flatEntryDelta = basicDelta;
                flatEntryPl = pl;
            }
        }    

        // Add the last entry vector and other price level fields to the
        // last price level message.
        MamaMsg& plMsg = *mPricelevels->mMsgVector[plCount];

        addBookLevel (plMsg, pl,
                      savedBasicDelta->getPlDeltaSize(),
                      savedBasicDelta->getPlDeltaAction(),
                      &(delta.getEventTime()));
        
        if (1 == entryCount)
        {
            addBookEntry (plMsg, basicDelta->getEntry(),
                          basicDelta->getEntryDeltaAction(),
                          &(pl->getTime()));

        }              
        if (entryCount > 1)
        {
            plMsg.addVectorMsg (NULL,
                               MamdaOrderBookFields::PL_ENTRIES->getFid(),
                               mEntries->mMsgVector,
                               entryCount);

        }
        
        if (defaultNumAttachedEntries != entryCount)
            plMsg.addI16 (NULL,
                         MamdaOrderBookFields::PL_NUM_ATTACH->getFid(),
                         (mama_i16_t) entryCount);
        ++plCount;

        if (1 == plCount)
        {
            addBookLevel (msg, pl,
                          basicDelta->getPlDeltaSize(),
                          basicDelta->getPlDeltaAction(),
                          &(delta.getEventTime()));
            if (1 == entryCount)
            {
                addBookEntry (msg, basicDelta->getEntry(),
                          basicDelta->getEntryDeltaAction(),
                          &(pl->getTime()));
            }
            if (entryCount > 1)
            {
                msg.addVectorMsg (NULL,
                               MamdaOrderBookFields::PL_ENTRIES->getFid(),
                               mEntries->mMsgVector,
                               entryCount);
            }
        }
        
        // Add the vector of plMsgs to the main msg if plCount > 1
        if (plCount > 1)
        {
        msg.addVectorMsg (NULL, MamdaOrderBookFields::PRICE_LEVELS->getFid(),
                          mPricelevels->mMsgVector,
                          plCount);   
        }
                          
        msg.addU32 (NULL, MamdaOrderBookFields::NUM_LEVELS->getFid(),
                    (mama_u32_t)plCount);             
        
        msg.addDateTime (NULL, MamdaOrderBookFields::BOOK_TIME->getFid(),
                         delta.getEventTime());
        
        msg.addDateTime (NULL, MamdaCommonFields::SRC_TIME->getFid(),
                         delta.getSrcTime());
                         
        if (delta.getEventSeqNum() != 0)
        {
            msg.addI64 (NULL, MamdaCommonFields::SEQ_NUM->getFid(),
                        (mama_i64_t)delta.getEventSeqNum());
        }
    }

    void MamdaOrderBookWriter::MamdaOrderBookWriterImpl::addBookEntry (
        MamaMsg&                     msg,
        const MamdaOrderBookEntry*   entry,
        MamdaOrderBookEntry::Action  entryDeltaAction,
        const MamaDateTime*          plTime)
    {
        if (entry->getId() != NULL)
        {
            msg.addString (NULL, MamdaOrderBookFields::ENTRY_ID->getFid(),
                           entry->getId());
        }
        
        msg.addI8  (NULL, MamdaOrderBookFields::ENTRY_ACTION->getFid(),
                    entryDeltaAction);
        
        msg.addU32 (NULL, MamdaOrderBookFields::ENTRY_SIZE->getFid(),
                    (mama_u32_t)entry->getSize());
        
        if ((NULL == plTime) || (plTime->compare(entry->getTime())!=0 ))
        {
            msg.addDateTime (NULL, MamdaOrderBookFields::ENTRY_TIME->getFid(),
                             entry->getTime());
        }
        
        if (entry->getStatus() != 0)
        {
            msg.addU16 (NULL, MamdaOrderBookFields::ENTRY_STATUS->getFid(),
                        (mama_u16_t)entry->getStatus());
        }
    }

    void MamdaOrderBookWriter::MamdaOrderBookWriterImpl::addBookLevel (
        MamaMsg&                          msg,
        const MamdaOrderBookPriceLevel*   level,
        mama_f64_t                        plDeltaSize,
        MamdaOrderBookPriceLevel::Action  plDeltaAction,
        const MamaDateTime*               bookTime)
    {
        msg.addPrice(NULL, MamdaOrderBookFields::PL_PRICE->getFid(),
                   level->getMamaPrice());
        
        if (defaultPlAction != (char)plDeltaAction)
        {
            msg.addI8 (NULL, MamdaOrderBookFields::PL_ACTION->getFid(),
                       (char)plDeltaAction);
        }
        if (defaultPlSide != (char)level->getSide())
        {
            msg.addI8 (NULL, MamdaOrderBookFields::PL_SIDE->getFid(),
                      (char)level->getSide());
        }
            
        msg.addU32  (NULL, MamdaOrderBookFields::PL_SIZE->getFid(), 
                     level->getSize());
        
        msg.addU32  (NULL, MamdaOrderBookFields::PL_SIZE_CHANGE->getFid(),
                     (mama_u32_t)level->getSizeChange());
        
        if ((NULL == bookTime || (bookTime->compare (level->getTime())!=0)))
        {
            msg.addDateTime (NULL, MamdaOrderBookFields::PL_TIME->getFid(),
                             level->getTime());   
        }
        
        if (defaultNumEntries != level->getNumEntries())
        {
            msg.addU16 (NULL, MamdaOrderBookFields::PL_NUM_ENTRIES->getFid(),
                        level->getNumEntries());
        }
    }

    void MamdaOrderBookWriter::MamdaOrderBookWriterImpl::addBookLevels (
        MamaMsg&               msg,
        const MamdaOrderBook&  book)
    {
        mama_u32_t numLevels = book.getTotalNumLevels();
        if (0 == numLevels)
        {
            /* Just add a zero NumLevels field. */
            msg.addI16 (NULL, MamdaOrderBookFields::NUM_LEVELS->getFid(),
                        (mama_i16_t)0);
            return;
        }

        mPricelevels->grow(numLevels);
        size_t plCount = 0;

        // Add order book fields
        MamdaOrderBook::constBidIterator plBidEnd   = book.bidEnd();
        MamdaOrderBook::bidIterator plBidIter       = book.bidBegin();
        for (; plBidIter != plBidEnd; ++plBidIter, ++plCount)
        {
            MamdaOrderBookPriceLevel* pl = *plBidIter;
            MamaMsg&  plMsg = *mPricelevels->mMsgVector[plCount];

            addBookLevel (plMsg, pl, 0.0,
                          MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD,
                          &(book.getBookTime()));
            
            addBookLevelEntries (plMsg, *pl);
        }
        MamdaOrderBook::constAskIterator plAskEnd   = book.askEnd();
        MamdaOrderBook::askIterator plAskIter       = book.askBegin();
        for (; plAskIter != plAskEnd; ++plAskIter, ++plCount)
        {
            MamdaOrderBookPriceLevel* pl = *plAskIter;
            MamaMsg&  plMsg = *mPricelevels->mMsgVector[plCount];

            addBookLevel (plMsg, pl, 0.0,
                          MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD,
                          &(book.getBookTime()));

            addBookLevelEntries (plMsg, *pl);
        }

        // Add the vector of plMsgs to the main msg
        msg.addVectorMsg (NULL, MamdaOrderBookFields::PRICE_LEVELS->getFid(),
                          mPricelevels->mMsgVector,
                          plCount);
                  
        msg.addI16 (NULL, MamdaOrderBookFields::NUM_LEVELS->getFid(),
                    (mama_i16_t)plCount);

        msg.addString (NULL, MamdaCommonFields::SYMBOL->getFid(),
                       book.getSymbol());
        
        if (book.hasPartId())
        {
            msg.addString (NULL, MamdaCommonFields::PART_ID->getFid(),
                           book.getPartId());    
        }
            
        msg.addDateTime (NULL, MamdaOrderBookFields::BOOK_TIME->getFid(),
                         book.getBookTime());

    }

    void MamdaOrderBookWriter::MamdaOrderBookWriterImpl::addBookLevelEntries (
        MamaMsg&                         plMsg,
        const MamdaOrderBookPriceLevel&  level)
    {
        // Add a vector of submsgs for each price level entry
        mama_u32_t numEntriesTotal = level.getNumEntriesTotal();
        // Make sure the reusable sub-submessage vector is large enough
        mEntries->grow(numEntriesTotal);
      
        mama_u32_t entCount = 0;
        MamdaOrderBookPriceLevel::iterator end = level.end();
        MamdaOrderBookPriceLevel::iterator e = level.begin();

        MamdaOrderBookEntry* ent = NULL;
        for (; e != end; ++e)
        {
            ent = *e;
            if (0 == ent->getSize())
                continue;

            MamaMsg& entMsg = *mEntries->mMsgVector[entCount];

            addBookEntry (entMsg, ent,
                          MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                          &(level.getTime()));
            entCount++;        
        }

        if (1 == entCount)
        {
            addBookEntry (plMsg, ent,
                          MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                          &(level.getTime()));
        }

        // Add entMsg vector to the plMsg
        if (entCount>1)
        {
            plMsg.addVectorMsg (NULL, MamdaOrderBookFields::PL_ENTRIES->getFid(),
                               mEntries->mMsgVector,
                               entCount);
        }
        
        if (defaultNumAttachedEntries != (mama_i16_t) entCount)
            plMsg.addI16 (NULL, MamdaOrderBookFields::PL_NUM_ATTACH->getFid(),
                        (mama_i16_t)entCount);
    }

} //end namespace
