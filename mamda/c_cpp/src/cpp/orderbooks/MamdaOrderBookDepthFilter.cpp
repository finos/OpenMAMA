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

#include <pthread.h>
#include <wombat/wincompat.h>
#include <mamda/MamdaLock.h>
#include <mamda/MamdaOptionalConfig.h>
#include <mamda/MamdaMsgListener.h>
#include <mamda/MamdaOrderBook.h>
#include <mamda/MamdaCommonFields.h>
#include <mamda/MamdaOrderBookRecap.h>
#include <mamda/MamdaOrderBookDepthFilter.h>
#include <deque>

using std::deque;

class MamdaOrderBookDepthFilterImpl
    : public MamdaOrderBookRecap
    , public MamdaOrderBookSimpleDelta
    , public MamdaOrderBookComplexDelta
    , public MamdaOrderBookClear
    , public MamdaOrderBookGap
{
public:
    MamdaOrderBookDepthFilterImpl (MamdaOrderBookDepthFilter& listener,
                                   MamdaOrderBookListener&    parent,                         
                                   size_t                     depth);

    virtual ~MamdaOrderBookDepthFilterImpl ();

    void removeHandlers ();
    void clear ();
   
    // Inherited from MamdaOrderBookRecap 
    const MamdaOrderBook* getOrderBook () const;

    // Inherited from MamdaOrderBookGap
    mama_seqnum_t        getBeginGapSeqNum () const { return mParent.getBeginGapSeqNum(); };
    mama_seqnum_t        getEndGapSeqNum   () const { return mParent.getEndGapSeqNum(); };

    void onBookRecap (
        MamdaSubscription*                  subscription,
        MamdaOrderBookListener&             listener,
        const MamaMsg*                      msg,
        const MamdaOrderBookComplexDelta*   delta,
        const MamdaOrderBookRecap&          event,
        const MamdaOrderBook&               book);

    void onBookDelta (
        MamdaSubscription*                  subscription,
        MamdaOrderBookListener&             listener,
        const MamaMsg*                      msg,
        const MamdaOrderBookSimpleDelta&    event,
        const MamdaOrderBook&               book);

    void onBookComplexDelta (
        MamdaSubscription*                  subscription,
        MamdaOrderBookListener&             listener,
        const MamaMsg*                      msg,
        const MamdaOrderBookComplexDelta&   event,
        const MamdaOrderBook&               book);

    void onBookClear (
        MamdaSubscription*                  subscription,
        MamdaOrderBookListener&             listener,
        const MamaMsg*                      msg,
        const MamdaOrderBookClear&          event,
        const MamdaOrderBook&               book);

    void onBookGap (
        MamdaSubscription*                  subscription,
        MamdaOrderBookListener&             listener,
        const MamaMsg*                      msg,
        const MamdaOrderBookGap&            event,
        const MamdaOrderBook&               book);
            
    void addDeltasFromLevel (
        MamdaOrderBookPriceLevel&    level);
        
    void addDelta (
        const MamdaOrderBookBasicDelta&  delta);
    
    void addDelta (
        MamdaOrderBookEntry*                entry,
        MamdaOrderBookPriceLevel*           level,
        mama_quantity_t                     plDeltaSize,
        MamdaOrderBookPriceLevel::Action    plAction,
        MamdaOrderBookEntry::Action         entryAction);

    void invokeRecapHandlers (
        MamdaSubscription*  subscription,
        const MamaMsg*      msg);
        
    void invokeClearHandlers (
        MamdaSubscription*  subscription,
        const MamaMsg*      msg);
        
    void invokeDeltaHandlers (
        MamdaSubscription*  subscription,
        const MamaMsg*      msg);

    void invokeGapHandlers (
        MamdaSubscription*  subscription,
        const MamaMsg*      msg);

    void processBidDelta (
        MamdaSubscription*                 subscription,
        MamdaOrderBookListener&            listener,
        const MamaMsg*                     msg,
        const MamdaOrderBookBasicDelta&    event,
        const MamdaOrderBook&              book,
        MamdaOrderBookPriceLevel&          deltaLevel);

    void processAskDelta (
        MamdaSubscription*                 subscription,
        MamdaOrderBookListener&            listener,
        const MamaMsg*                     msg,
        const MamdaOrderBookBasicDelta&    event,
        const MamdaOrderBook&              book,
        MamdaOrderBookPriceLevel&          deltaLevel);
        
    // Inherited from MamdaBasicRecap and MamdaBasicEvent
    const char*          getSymbol         () const { return mParent.getSymbol(); };
    const char*          getPartId         () const { return mParent.getPartId(); };
    mama_seqnum_t        getEventSeqNum    () const { return mParent.getEventSeqNum(); };
    const MamaDateTime&  getEventTime      () const { return mParent.getEventTime(); };
    const MamaDateTime&  getSrcTime        () const { return mParent.getSrcTime(); };
    const MamaDateTime&  getActivityTime   () const { return mParent.getActivityTime(); };
    const MamaDateTime&  getLineTime       () const { return mParent.getLineTime(); };
    const MamaDateTime&  getSendTime       () const { return mParent.getSendTime(); };
    const MamaMsgQual&   getMsgQual        () const { return mParent.getMsgQual(); };

    inline void acquireLock ();
    inline void releaseLock ();

    virtual MamdaFieldState     getSymbolFieldState()       const { return mParent.getSymbolFieldState();}
    virtual MamdaFieldState     getPartIdFieldState()       const { return mParent.getPartIdFieldState();}
    virtual MamdaFieldState     getEventSeqNumFieldState()  const { return mParent.getEventSeqNumFieldState();}
    virtual MamdaFieldState     getEventTimeFieldState()    const { return mParent.getEventTimeFieldState();}
    virtual MamdaFieldState     getSrcTimeFieldState()      const { return mParent.getSrcTimeFieldState();}
    virtual MamdaFieldState     getActivityTimeFieldState() const { return mParent.getActivityTimeFieldState();}
    virtual MamdaFieldState     getLineTimeFieldState()     const { return mParent.getLineTimeFieldState();}
    virtual MamdaFieldState     getSendTimeFieldState()     const { return mParent.getSendTimeFieldState();}
    virtual MamdaFieldState     getMsgQualFieldState()      const { return mParent.getMsgQualFieldState();}

    MamdaOrderBookListener&         mParent;
    MamdaOrderBookDepthFilter&      mListener;
    
    MamdaOrderBook*                 mTruncatedBook;
    const MamdaOrderBook*           mFullBook;
    
    deque<MamdaOrderBookHandler*>   mHandlers;
    mama_u32_t                      mCurrentDeltaCount;

    size_t                          mMaxDepth;
    MamdaOrderBook::bidIterator     mLastBidIter;
    MamdaOrderBook::askIterator     mLastAskIter;
    double                          mLastBidPrice;
    double                          mLastAskPrice;
    size_t                          mBidDepth;
    size_t                          mAskDepth;
   
};


MamdaOrderBookDepthFilter::MamdaOrderBookDepthFilter (MamdaOrderBookListener& parent,
                                                      size_t                  depth)
    : MamdaOrderBookListener ()
    , mImpl   (*new MamdaOrderBookDepthFilterImpl(*this, parent, depth))
{

}

MamdaOrderBookDepthFilter::~MamdaOrderBookDepthFilter ()
{
    delete &mImpl;
}

void MamdaOrderBookDepthFilter::addHandler (MamdaOrderBookHandler* handler)
{
    mImpl.mHandlers.push_back (handler);
}

void MamdaOrderBookDepthFilter::removeHandlers ()
{
    mImpl.removeHandlers();
}

const MamdaOrderBook* MamdaOrderBookDepthFilter::getOrderBook () const
{
    return mImpl.getOrderBook ();
}

const MamdaOrderBook* MamdaOrderBookDepthFilter::getFullOrderBook () const
{
    return mImpl.mParent.getOrderBook ();
}

void MamdaOrderBookDepthFilter::onBookRecap (
        MamdaSubscription*                 subscription,
        MamdaOrderBookListener&            listener,
        const MamaMsg*                     msg,
        const MamdaOrderBookComplexDelta*  delta,
        const MamdaOrderBookRecap&         event,
        const MamdaOrderBook&              book)
{
    mImpl.onBookRecap (subscription,
                       listener,
                       msg,
                       delta,
                       event,
                       book);
}

void MamdaOrderBookDepthFilter::onBookDelta (
        MamdaSubscription*                 subscription,
        MamdaOrderBookListener&            listener,
        const MamaMsg*                     msg,
        const MamdaOrderBookSimpleDelta&   event,
        const MamdaOrderBook&              book)
{
    mImpl.onBookDelta (subscription,
                        listener,
                        msg,
                        event,
                        book);
}

 void MamdaOrderBookDepthFilter::onBookComplexDelta (
        MamdaSubscription*                 subscription,
        MamdaOrderBookListener&            listener,
        const MamaMsg*                     msg,
        const MamdaOrderBookComplexDelta&  event,
        const MamdaOrderBook&              book)
{
    mImpl.onBookComplexDelta (subscription,
                               listener,
                               msg,
                               event,
                               book);
}

void MamdaOrderBookDepthFilter::onBookClear (
        MamdaSubscription*                  subscription,
        MamdaOrderBookListener&             listener,
        const MamaMsg*                      msg,
        const MamdaOrderBookClear&          event,
        const MamdaOrderBook&               book)
{
    mImpl.onBookClear (subscription,
                       listener,
                       msg,
                       event,
                       book);
}

 void MamdaOrderBookDepthFilter::onBookGap (
        MamdaSubscription*                  subscription,
        MamdaOrderBookListener&             listener,
        const MamaMsg*                      msg,
        const MamdaOrderBookGap&            event,
        const MamdaOrderBook&               book)
{
    mImpl.onBookGap (subscription,
                     listener,
                     msg,
                     event,
                     book);
}

MamdaOrderBookDepthFilterImpl::MamdaOrderBookDepthFilterImpl(
    MamdaOrderBookDepthFilter&  listener,
    MamdaOrderBookListener&     parent, 
    size_t                      depth)
    : mParent            (parent)
    , mListener          (listener)
    , mTruncatedBook     (NULL)
    , mFullBook          (NULL)
    , mCurrentDeltaCount (0)
    , mMaxDepth          (depth)
    , mBidDepth          (0)
    , mAskDepth          (0)
{
    mTruncatedBook = new MamdaOrderBook;
}

MamdaOrderBookDepthFilterImpl::~MamdaOrderBookDepthFilterImpl()
{
    delete mTruncatedBook;
    mTruncatedBook = NULL;
}

void MamdaOrderBookDepthFilterImpl::removeHandlers()
{
    mHandlers.clear ();
}

void MamdaOrderBookDepthFilterImpl::onBookRecap (
        MamdaSubscription*                 subscription,
        MamdaOrderBookListener&            listener,
        const MamaMsg*                     msg,
        const MamdaOrderBookComplexDelta*  delta,
        const MamdaOrderBookRecap&         event,
        const MamdaOrderBook&              book)
{
    if (!mFullBook) mFullBook = &book;

    /* Clear but don't delete the levels as the levels objects
       belong to the book in the main listener */
    mTruncatedBook->clear(false);
    
    mLastBidIter = book.bidBegin ();
    MamdaOrderBook::bidIterator bidEnd  = book.bidEnd ();
 
    for (mBidDepth = 0;
        (mLastBidIter != bidEnd) && (mBidDepth != mMaxDepth);
        ++mBidDepth, ++mLastBidIter)
    {
        mTruncatedBook->addLevel (**mLastBidIter);
    }

    --mLastBidIter;
    mLastBidPrice = (*mLastBidIter)->getPrice();
    
    mLastAskIter = book.askBegin ();
    MamdaOrderBook::askIterator askEnd  = book.askEnd ();

    for (mAskDepth = 0;
        (mLastAskIter != askEnd) && (mAskDepth != mMaxDepth);
         ++mAskDepth, ++mLastAskIter)
    {
        mTruncatedBook->addLevel (**mLastAskIter);
    }

    --mLastAskIter;
    mLastAskPrice = (*mLastAskIter)->getPrice();
    
    invokeRecapHandlers (subscription, msg);
}

void MamdaOrderBookDepthFilterImpl::onBookDelta (
        MamdaSubscription*                 subscription,
        MamdaOrderBookListener&            listener,
        const MamaMsg*                     msg,
        const MamdaOrderBookSimpleDelta&   event,
        const MamdaOrderBook&              book)
{
    MamdaOrderBookPriceLevel* deltaLevel     = event.getPriceLevel();
    MamdaOrderBookPriceLevel::Side deltaSide = deltaLevel->getSide();
   
    if (MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID == deltaSide)
    {
        processBidDelta (subscription,
                         listener,
                         msg,
                         event,
                         book,
                         *deltaLevel);
    }
    else 
    {
        processAskDelta (subscription,
                         listener,
                         msg,
                         event,
                         book,
                         *deltaLevel);
    }
    invokeDeltaHandlers (subscription, msg);
}

void MamdaOrderBookDepthFilterImpl::onBookComplexDelta (
        MamdaSubscription*                 subscription,
        MamdaOrderBookListener&            listener,
        const MamaMsg*                     msg,
        const MamdaOrderBookComplexDelta&  event,
        const MamdaOrderBook&              book)
{
    MamdaOrderBookComplexDelta::const_iterator end = event.end();
    MamdaOrderBookComplexDelta::const_iterator i   = event.begin();
    for (; i != end; ++i)
    {
        const MamdaOrderBookBasicDelta*   basicDelta = *i;
        MamdaOrderBookPriceLevel*         deltaLevel  = basicDelta->getPriceLevel();
        MamdaOrderBookPriceLevel::Side    deltaSide   = deltaLevel->getSide();
   
        if (MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID == deltaSide)
        {
            processBidDelta (subscription,
                             listener,
                             msg,
                             *basicDelta,
                             book,
                             *deltaLevel);
        }
        else
        {
            processAskDelta (subscription,
                             listener,
                             msg,
                             *basicDelta,
                             book,
                             *deltaLevel);
        }
        
    }   
    invokeDeltaHandlers (subscription, msg);         
}


void MamdaOrderBookDepthFilterImpl::processBidDelta (
        MamdaSubscription*                 subscription,
        MamdaOrderBookListener&            listener,
        const MamaMsg*                     msg,
        const MamdaOrderBookBasicDelta&    event,
        const MamdaOrderBook&              book,
        MamdaOrderBookPriceLevel&          deltaLevel)
{
    double deltaPrice = deltaLevel.getPrice();
    /* Ignore if this out of scope and we have filled the max depth */
    if ((deltaPrice < mLastBidPrice) && (mBidDepth == mMaxDepth))
    {
        return;
    }
    
    switch  (event.getPlDeltaAction())
    {
    case MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_UPDATE:
        /* Just pass this delta on untouched */
        addDelta (event);
        break;
            
    case MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD:
        mTruncatedBook->addLevel (deltaLevel);
        
        if (mBidDepth == mMaxDepth)
        {
            addDelta (event);
            /* Delete the last price level in the view as it goes out of scope */
            mLastBidIter = --(mTruncatedBook->bidEnd());
            
            MamdaOrderBookPriceLevel* levelToDelete = *mLastBidIter;
            --mLastBidIter;
            mLastBidPrice = (*mLastBidIter)->getPrice();
            addDelta (NULL, levelToDelete, levelToDelete->getSize(),
                      MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_DELETE,
                      MamdaOrderBookEntry::MAMDA_BOOK_ACTION_UNKNOWN);
            mTruncatedBook->deleteLevel (*levelToDelete);
        }
        else
        {
            /*We haven't got reached the max depth yet */
            if (deltaPrice < mLastBidPrice)
            {
                mLastBidPrice = deltaPrice;
            }
            
            ++mBidDepth;
            addDelta (event);
        }
        break;
    
    case MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_DELETE:
        mTruncatedBook->deleteLevel (deltaLevel);
        addDelta (event);
        /* -1 as positions start at 0*/
        MamdaOrderBookPriceLevel* level = book.getLevelAtPosition (
                                        mBidDepth -1,
                                        MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
        if (level)
        {
            /* We have more levels in the full book so bring them into view.*/    
            mTruncatedBook->addLevel (*level);
   
            /* generate the delta from the level in the full book */
            addDeltasFromLevel (*level);
            mLastBidPrice = level->getPrice();
        }
        else
        {
            --mBidDepth;
        }
        break;
    case MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_UNKNOWN:
        break;
    }
}

void MamdaOrderBookDepthFilterImpl::processAskDelta (
        MamdaSubscription*                 subscription,
        MamdaOrderBookListener&            listener,
        const MamaMsg*                     msg,
        const MamdaOrderBookBasicDelta&    event,
        const MamdaOrderBook&              book,
        MamdaOrderBookPriceLevel&          deltaLevel)
{
    double deltaPrice = deltaLevel.getPrice();
    /* Ignore if this out of scope and we have filled the max */
    if ((deltaPrice > mLastAskPrice) && (mAskDepth == mMaxDepth))
    {
        return;
    }
    
    switch  (event.getPlDeltaAction())
    {
    case MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_UPDATE:
        /* Just pass this delta on untouched */
        addDelta (event);
        break;
            
    case MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD:
        mTruncatedBook->addLevel (deltaLevel);
        if (mAskDepth == mMaxDepth)
        {
            addDelta (event);

            mLastAskIter = --(mTruncatedBook->askEnd());
            /* Delete the last price level in the view as it goes out of scope */
            MamdaOrderBookPriceLevel* levelToDelete = *mLastAskIter;
            --mLastAskIter;
            mLastAskPrice = (*mLastAskIter)->getPrice();
            addDelta (NULL, levelToDelete, levelToDelete->getSize(),
                        MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_DELETE,
                        MamdaOrderBookEntry::MAMDA_BOOK_ACTION_UNKNOWN);
            mTruncatedBook->deleteLevel (*levelToDelete);
            
            invokeDeltaHandlers (subscription, msg);
        }
        else
        {
            if (deltaPrice > mLastAskPrice)
            {
                mLastAskPrice = deltaPrice;
            }
            
            ++mAskDepth;
            addDelta (event);
        }
        break;
    
    case MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_DELETE:
        mTruncatedBook->deleteLevel (deltaLevel);
        addDelta (event);
        
        /* -1 as positions start at 0*/
        MamdaOrderBookPriceLevel* level = book.getLevelAtPosition (
                                        mAskDepth-1, 
                                        MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_ASK);
        if (level)
        {
            /* We have more levels in the full book so bring them into view.*/    
            mTruncatedBook->addLevel (*level);
   
            /* generate the delta from the level in the full book */
            addDeltasFromLevel (*level);
            mLastAskPrice = level->getPrice();
        }
        else
        {
            --mAskDepth;
        }
        break;
    case MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_UNKNOWN:
        break;
    }
           
}

void MamdaOrderBookDepthFilterImpl::onBookClear (
        MamdaSubscription*                  subscription,
        MamdaOrderBookListener&             listener,
        const MamaMsg*                      msg,
        const MamdaOrderBookClear&          event,
        const MamdaOrderBook&               book)
{
    mTruncatedBook->clear(false);
    invokeClearHandlers (subscription, msg);
}

 void MamdaOrderBookDepthFilterImpl::onBookGap (
        MamdaSubscription*                  subscription,
        MamdaOrderBookListener&             listener,
        const MamaMsg*                      msg,
        const MamdaOrderBookGap&            event,
        const MamdaOrderBook&               book)
{
    invokeGapHandlers (subscription, msg);
}

void MamdaOrderBookDepthFilterImpl::clear()
{
    mTruncatedBook->clear();
  
}

void MamdaOrderBookDepthFilterImpl::invokeRecapHandlers (
    MamdaSubscription*  subscription,
    const MamaMsg*      msg)
{
    deque<MamdaOrderBookHandler*>::iterator end = mHandlers.end();
    deque<MamdaOrderBookHandler*>::iterator i   = mHandlers.begin();
    for (; i != end; ++i)
    {
        MamdaOrderBookHandler* handler = *i;
        handler->onBookRecap (subscription, mListener, msg, NULL, *this,
                                  *mTruncatedBook);
    }
}

void MamdaOrderBookDepthFilterImpl::invokeClearHandlers (
    MamdaSubscription*  subscription,
    const MamaMsg*      msg)
{
    std::deque<MamdaOrderBookHandler*>::iterator end = mHandlers.end();
    std::deque<MamdaOrderBookHandler*>::iterator i   = mHandlers.begin();
    for (; i != end; ++i)
    {
        MamdaOrderBookHandler* handler = *i;
        handler->onBookClear (subscription, mListener, msg, *this,
                              *mTruncatedBook);
    }
}

void MamdaOrderBookDepthFilterImpl::invokeDeltaHandlers (
    MamdaSubscription*  subscription,
    const MamaMsg*      msg)
{
    
    if (0 == mCurrentDeltaCount) return;
    
    deque<MamdaOrderBookHandler*>::iterator end = mHandlers.end();
    deque<MamdaOrderBookHandler*>::iterator i   = mHandlers.begin();
    for (; i != end; ++i)
    {
        MamdaOrderBookHandler* handler = *i;
        
        if (1 == mCurrentDeltaCount)
        {
            handler->onBookDelta (subscription, mListener, msg,
                                  *this, *mTruncatedBook);
        }
        else
        {
            MamdaOrderBookComplexDelta::setOrderBook(mTruncatedBook);
            handler->onBookComplexDelta (subscription, mListener, msg,
                                         *this, *mTruncatedBook);

        }
    }

    MamdaOrderBookBasicDelta::clear();
    MamdaOrderBookComplexDelta::clear();
    mCurrentDeltaCount = 0;
}

void MamdaOrderBookDepthFilterImpl::invokeGapHandlers (
    MamdaSubscription*  subscription,
    const MamaMsg*      msg)
{
    deque<MamdaOrderBookHandler*>::iterator end = mHandlers.end();
    deque<MamdaOrderBookHandler*>::iterator i   = mHandlers.begin();
    for (; i != end; ++i)
    {
        MamdaOrderBookHandler* handler = *i;
        handler->onBookGap (subscription, mListener, msg, *this, *mTruncatedBook);
    }
}

void MamdaOrderBookDepthFilterImpl::addDelta (
    const MamdaOrderBookBasicDelta&  delta)
{
    if (mListener.getProcessEntries ())
    {
        addDelta (delta.getEntry(), delta.getPriceLevel(), delta.getPlDeltaSize(),
                  delta.getPlDeltaAction(), delta.getEntryDeltaAction());
    }
    else
    {
        addDelta (NULL, delta.getPriceLevel(), delta.getPlDeltaSize(),
                  delta.getPlDeltaAction(),
                  MamdaOrderBookEntry::MAMDA_BOOK_ACTION_UNKNOWN);
    }
}

void MamdaOrderBookDepthFilterImpl::addDeltasFromLevel (
    MamdaOrderBookPriceLevel&    level)
{    
    if (!mListener.getProcessEntries())
    {
        addDelta (NULL, &level, level.getSize(),
                  MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD,
                  MamdaOrderBookEntry::MAMDA_BOOK_ACTION_UNKNOWN);
        return;
    }
    
    MamdaOrderBookPriceLevel::iterator i   = level.begin();
    MamdaOrderBookPriceLevel::iterator end = level.end();

    size_t j = 0;

    for (;i != end; ++i)
    {   
        MamdaOrderBookEntry* entry = *i;
        addDelta (entry, &level, entry->getSize(),
                  (0 == j++) ? MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD
                             : MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_UPDATE,                          
                  MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD);
    }
}

void MamdaOrderBookDepthFilterImpl::addDelta (
    MamdaOrderBookEntry*              entry,
    MamdaOrderBookPriceLevel*         level,
    mama_quantity_t                   plDeltaSize,
    MamdaOrderBookPriceLevel::Action  plAction,
    MamdaOrderBookEntry::Action       entryAction)
{
    ++mCurrentDeltaCount;

    if (mCurrentDeltaCount == 1)
    {        
        /* This is number one, so save the "simple" delta. */
        MamdaOrderBookSimpleDelta::set (
            entry, level, plDeltaSize, plAction, entryAction);
    }
    else if (mCurrentDeltaCount == 2)
    {
        /* This is number two, so copy the saved "simple" delta to the
         * "complex" delta and add the current one. */
        MamdaOrderBookComplexDelta::clear();
        MamdaOrderBookComplexDelta::setOrderBook (mTruncatedBook);
        MamdaOrderBookComplexDelta::add (*this);
        MamdaOrderBookComplexDelta::add (
            entry, level, plDeltaSize, plAction, entryAction);
    }
    else
    {
        /* This is number greater than two, so add the current delta. */
        MamdaOrderBookComplexDelta::add (
            entry, level, plDeltaSize, plAction, entryAction);
    }
}

const MamdaOrderBook* MamdaOrderBookDepthFilterImpl::getOrderBook () const
{
    if (mTruncatedBook)
    {
        return mTruncatedBook;
    }
    else
    {
        throw MamdaOrderBookException (
            "Attempt to access a NULL full order book");
    }
}
