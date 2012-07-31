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

#include <mamda/MamdaLock.h>
#include <mamda/MamdaOptionalConfig.h>
#include <mamda/MamdaMsgListener.h>
#include <mamda/MamdaOrderBook.h>
#include <mamda/MamdaSubscription.h>
#include <mamda/MamdaQuoteFields.h>
#include <mamda/MamdaCommonFields.h>
#include <mamda/MamdaOrderBookRecap.h>
#include <mamda/MamdaQuoteToBookListener.h>
#include <deque>

using std::deque;

struct QuoteCache
{   

    void initialize();
    
    string          mSymbol;
    MamaDateTime    mSrcTime;
    MamaDateTime    mActTime;
    MamaDateTime    mLineTime;
    MamaDateTime    mSendTime;
    MamaPrice       mBidPrice;
    mama_quantity_t mBidSize;
    MamaPrice       mAskPrice;
    mama_quantity_t mAskSize;

    MamaDateTime    mQuoteTime;
    MamaDateTime    mQuoteDate;
    string          mQuoteQualStr;
    string          mQuoteQualNative;

    MamaDateTime    mAskTime;
    MamaDateTime    mBidTime;

    mama_u32_t      mTmpQuoteCount;
    mama_u32_t      mQuoteCount;
    mama_seqnum_t   mQuoteSeqNum;

    bool            mLastGenericMsgWasQuote;
    bool            mGotBidPrice;
    bool            mGotAskPrice;
    bool            mGotBidSize;
    bool            mGotAskSize;
    
    mama_i16_t      mMsgNum;
    MamdaFieldState mMsgNumFieldState; 
    mama_i16_t      mMsgTotal;
    MamdaFieldState mMsgTotalFieldState;

    // The following fields are used for caching the order book and
    // related fields.  These fields can be used by applications for
    // reference and will be passed for recaps.

    MamdaFieldState mSrcTimeFieldState; 
    MamdaFieldState mLineTimeFieldState;
    MamdaFieldState mSendTimeFieldState;
    MamdaFieldState mActivityTimeFieldState;
    MamdaFieldState mMsgQualFieldState;  
    MamdaFieldState mBookTimeFieldState; 
    MamdaFieldState mMsgSeqNumFieldState; 
    MamdaFieldState mSenderIdFieldState;  
    MamdaFieldState mSymbolFieldState; 
    MamdaFieldState mPartIdFieldState;
    MamdaFieldState mEventSeqNumFieldState;
    MamdaFieldState mEventTimeFieldState;

};

struct QuoteToBookFieldUpdate
{
    virtual void onUpdate (
        MamdaQuoteToBookListenerImpl&  impl,
        const MamaMsgField&            field) = 0;
};

class MamdaQuoteToBookListenerImpl 
    : public MamaMsgFieldIterator
    , public MamdaOrderBookRecap
    , public MamdaOrderBookSimpleDelta
    , public MamdaOrderBookComplexDelta
    , public MamdaOrderBookClear
    , public MamdaOrderBookGap
{
public:
    MamdaQuoteToBookListenerImpl (MamdaQuoteToBookListener& listener,
                                  MamdaOrderBook*           fullBook);

    virtual ~MamdaQuoteToBookListenerImpl ();

    void removeHandlers ();
    void clear ();
   
    // Inherited from MamdaOrderBookRecap 
    const MamdaOrderBook* getOrderBook () const;

    // Inherited from MamdaOrderBookGap
    mama_seqnum_t        getBeginGapSeqNum () const;
    mama_seqnum_t        getEndGapSeqNum   () const;

    bool evaluateMsgQual (MamdaSubscription*  subscription,
                          const MamaMsg&      msg);

    void handleMessage (MamdaSubscription*  subscription,
                        const MamaMsg&      msg,
                        short               msgType);

    void handleRecap (MamdaSubscription*  subscription,
                      const MamaMsg&      msg);

    void handleQuote (MamdaSubscription*  subscription,
                      const MamaMsg&      msg);

    void updateQuoteFields (const MamaMsg&      msg);
    
    void updateFieldStates ();

    void addLevel (
        MamdaOrderBookPriceLevel*&        level,
        double                            plPrice,
        mama_f64_t                        plSize,
        MamdaOrderBookPriceLevel::Side    plSide,
        const MamaDateTime&               plTime);

    void updateLevel (
        MamdaOrderBookPriceLevel*         level,
        mama_f64_t                        plSize,
        mama_quantity_t                   plSizeChange,
        MamdaOrderBookPriceLevel::Side    plSide,
        const MamaDateTime&               plTime);

    void deleteLevel (
        MamdaOrderBookPriceLevel*         level,
        MamdaOrderBookPriceLevel::Side    plSide,
        const MamaDateTime&               plTime);


    void addDelta (MamdaOrderBookPriceLevel*         level,
                   mama_quantity_t                   plDeltaSize,
                   MamdaOrderBookPriceLevel::Action  plAction);

    void invokeRecapHandlers (MamdaSubscription*  subscription,
                              const MamaMsg*      msg);

    void invokeClearHandlers (MamdaSubscription*  subscription,
                              const MamaMsg*      msg);

    void invokeDeltaHandlers (MamdaSubscription*  subscription,
                              const MamaMsg*      msg);

    void invokeGapHandlers (MamdaSubscription*  subscription,
                            const MamaMsg*      msg);

    // Inherited from MamdaBasicRecap and MamdaBasicEvent
    const char*          getSymbol         () const;
    const char*          getPartId         () const;
    mama_seqnum_t        getEventSeqNum    () const;
    const MamaDateTime&  getEventTime      () const;
    const MamaDateTime&  getSrcTime        () const;
    const MamaDateTime&  getActivityTime   () const;
    const MamaDateTime&  getLineTime       () const;
    const MamaDateTime&  getSendTime       () const;
    const MamaMsgQual&   getMsgQual        () const;
    
    MamdaFieldState     getSymbolFieldState()       const;
    MamdaFieldState     getPartIdFieldState()       const;
    MamdaFieldState     getEventSeqNumFieldState()  const;
    MamdaFieldState     getEventTimeFieldState()    const;
    MamdaFieldState     getSrcTimeFieldState()      const;
    MamdaFieldState     getActivityTimeFieldState() const;
    MamdaFieldState     getLineTimeFieldState()     const;
    MamdaFieldState     getSendTimeFieldState()     const;
    MamdaFieldState     getMsgQualFieldState()      const;

    void  setQuality (MamdaSubscription*  sub,
                      mamaQuality         quality);

    void checkQuoteCount (MamdaSubscription*  subscription,
                          const MamaMsg&      msg);

    void onField      (const MamaMsg&      msg,
                       const MamaMsgField& field,
                       void*               closure);

    // Lock to protected the impl's full order book.    
    MamdaLock                      mFullBookLock;
    MamdaOrderBook*                mFullBook;
    bool                           mLocalFullBook;

    inline void acquireLock ()  { ACQUIRE_WLOCK(mFullBookLock); }
    inline void releaseLock ()  { RELEASE_WLOCK(mFullBookLock); }

    static void initFieldUpdaters ();
    static void initFieldUpdater (const MamaFieldDescriptor*  fieldDesc,
                                  QuoteToBookFieldUpdate*           updater);

    static QuoteToBookFieldUpdate**  mFieldUpdaters;
    static uint16_t            mFieldUpdatersSize;
    MamdaQuoteToBookListener&  mListener;
    MamdaSubscription*         mSubscription;
    mama_u32_t                 mCurrentDeltaCount;
    mama_seqnum_t              mEventSeqNum;
    mama_seqnum_t              mGapBegin;
    mama_seqnum_t              mGapEnd;
    bool                       mUpdateStaleBook;
    bool                       mClearStaleBook;
    bool                       mIsTransientMsg;
    bool                       mUpdateInconsistentBook;
    // If mResolvePossiblyDuplicate is set then the listen will atempt, 
    // based upon sequence number and event times, to determine whether
    // a quote marked as "possibly duplicate" is definately a duplicate,
    // in which case it is dropped.
    bool                       mResolvePossiblyDuplicate;
    int                        mQuoteSizeMultiplier;
    deque<MamdaOrderBookHandler*>  mHandlers;

    // Message Qualifier - holds information provides in formation 
    // regarding duplicate and delayed status of message.
    MamaMsgQual                mMsgQual;

    // The Quote Listener Data Caches
    QuoteCache                 mRegularCache;     // Regular update cache
    QuoteCache                 mTransientCache;   // Transient cache 
    QuoteCache&                mQuoteCache;       // Current cache in use

    struct FieldUpdateSymbol;
    struct FieldUpdateIssueSymbol;
    struct FieldUpdateQuoteSrcTime;
    struct FieldUpdateQuoteActTime;
    struct FieldUpdateQuoteLineTime;
    struct FieldUpdateQuoteSendTime;
    struct FieldUpdateBidPrice;
    struct FieldUpdateBidSize;
    struct FieldUpdateAskPrice;
    struct FieldUpdateAskSize;
    struct FieldUpdateQuoteTime;
    struct FieldUpdateQuoteSeqNum;
    struct FieldUpdateQuoteDate;
    struct FieldUpdateQuoteQual;
    struct FieldUpdateQuoteQualNative;
    struct FieldUpdateTmpQuoteCount;
    struct FieldUpdateAskTime;
    struct FieldUpdateBidTime;
};

QuoteToBookFieldUpdate** MamdaQuoteToBookListenerImpl::mFieldUpdaters = NULL;
uint16_t  MamdaQuoteToBookListenerImpl::mFieldUpdatersSize = 0;


MamdaQuoteToBookListener::MamdaQuoteToBookListener (MamdaOrderBook*  fullBook)
    : MamdaOrderBookListener (fullBook)
    , mImpl (*new MamdaQuoteToBookListenerImpl(*this, fullBook))
{
}

MamdaQuoteToBookListener::~MamdaQuoteToBookListener ()
{
    delete &mImpl;
}

/**
* Add a specialized order book handler.  Currently, only one
* handler can (and must) be registered.
*
* @param handler The hadler registered to receive order book update
* callbacks.
*/
void MamdaQuoteToBookListener::addHandler (MamdaOrderBookHandler* handler)
{
    mImpl.mHandlers.push_back (handler);
}

void MamdaQuoteToBookListener::setProcessEntries (bool  process)
{
}

void MamdaQuoteToBookListener::addIgnoreEntryId (const char*  id)
{
}

void MamdaQuoteToBookListener::removeIgnoreEntryId (const char*  id)
{
}

void MamdaQuoteToBookListener::setUseEntryManager (bool  useManager)
{
}

void MamdaQuoteToBookListener::setEntryIdsAreUnique (bool  uniqueIds)
{
}

void MamdaQuoteToBookListener::setKeepBasicDeltas (bool  keep)
{
}

void MamdaQuoteToBookListener::setConflateDeltas  (bool conflate)
{
}

bool MamdaQuoteToBookListener::getConflateDeltas  ()
{
    return false;
}

void MamdaQuoteToBookListener::setConflationInterval (double interval)
{
}

void MamdaQuoteToBookListener::forceInvokeDeltaHandlers ()
{
}

void MamdaQuoteToBookListener::clearConflatedDeltas ()
{
}

void MamdaQuoteToBookListener::setProcessProperties (bool value)
{
}

bool MamdaQuoteToBookListener::getProcessProperties () const
{
    return false;
}

void MamdaQuoteToBookListener::getBookSnapShot (MamdaOrderBook&  result)
{
    ACQUIRE_RLOCK(mImpl.mFullBookLock);
    result = *(mImpl.mFullBook);
    RELEASE_RLOCK(mImpl.mFullBookLock);
}

void MamdaQuoteToBookListener::removeHandlers ()
{
    mImpl.removeHandlers();
}

void MamdaQuoteToBookListener::clear ()
{
    mImpl.clear();
}

const MamdaOrderBook* MamdaQuoteToBookListener::getOrderBook () const
{
    return mImpl.getOrderBook ();
}

mama_seqnum_t MamdaQuoteToBookListener::getBeginGapSeqNum () const
{
    return mImpl.getBeginGapSeqNum ();
}

mama_seqnum_t MamdaQuoteToBookListener::getEndGapSeqNum () const
{
    return mImpl.getEndGapSeqNum ();
}

const char* MamdaQuoteToBookListener::getSymbol () const
{
    return mImpl.getSymbol ();
}

const char* MamdaQuoteToBookListener::getPartId () const
{
    return mImpl.getPartId ();
}

mama_seqnum_t MamdaQuoteToBookListener::getEventSeqNum () const
{
    return mImpl.getEventSeqNum ();
}

const MamaDateTime&  MamdaQuoteToBookListener::getEventTime () const
{
    return mImpl.getEventTime();
}

const MamaDateTime&  MamdaQuoteToBookListener::getSrcTime () const
{
    return mImpl.getSrcTime();
}

const MamaDateTime&  MamdaQuoteToBookListener::getLineTime () const
{
    return mImpl.getLineTime();
}

const MamaDateTime&  MamdaQuoteToBookListener::getActivityTime () const
{
    return mImpl.getActivityTime();
}

const MamaDateTime&  MamdaQuoteToBookListener::getSendTime () const
{
    return mImpl.getSendTime();
}

const MamaMsgQual&   MamdaQuoteToBookListener::getMsgQual () const
{
    return mImpl.getMsgQual();
}

mama_u32_t           MamdaQuoteToBookListener::getQuoteCount () const
{
    return mImpl.mQuoteCache.mQuoteCount;
}

void MamdaQuoteToBookListener::resolvePossiblyDuplicate (bool tf)
{
    mImpl.mResolvePossiblyDuplicate = tf;
}

void MamdaQuoteToBookListener::setUpdateInconsistentBook (bool  update)
{
    mImpl.mUpdateInconsistentBook = update;
}

void MamdaQuoteToBookListener::setUpdateStaleBook (bool  update)
{
    mImpl.mUpdateStaleBook = update;
}

void MamdaQuoteToBookListener::setClearStaleBook (bool  clear)
{
    mImpl.mClearStaleBook = clear;
}

void MamdaQuoteToBookListener::setQuality (MamdaSubscription*  sub,
                                           mamaQuality         quality)
{
    mImpl.setQuality (sub, quality);
}

void MamdaQuoteToBookListener::setQuoteSizeMultiplier (int multiplier)
{
    mImpl.mQuoteSizeMultiplier = multiplier;
}

/*      FieldAccessors      */
MamdaFieldState MamdaQuoteToBookListener::getSymbolFieldState() const
{
    return mImpl.getSymbolFieldState ();
}

MamdaFieldState MamdaQuoteToBookListener::getPartIdFieldState() const
{
    return mImpl.getPartIdFieldState ();
}

MamdaFieldState  MamdaQuoteToBookListener::getEventSeqNumFieldState() const
{
    return mImpl.getEventSeqNumFieldState ();
}

MamdaFieldState MamdaQuoteToBookListener::getEventTimeFieldState() const
{
    return mImpl.getEventTimeFieldState ();
}

MamdaFieldState MamdaQuoteToBookListener::getSrcTimeFieldState() const
{
    return mImpl.getSrcTimeFieldState ();
}

MamdaFieldState MamdaQuoteToBookListener::getActivityTimeFieldState() const
{
    return mImpl.getActivityTimeFieldState ();
}

MamdaFieldState MamdaQuoteToBookListener::getLineTimeFieldState() const
{
    return mImpl.getLineTimeFieldState ();
}

MamdaFieldState MamdaQuoteToBookListener::getSendTimeFieldState() const
{
    return mImpl.getSendTimeFieldState ();
}

MamdaFieldState MamdaQuoteToBookListener::getMsgQualFieldState() const
{
    return mImpl.getMsgQualFieldState ();
}

void MamdaQuoteToBookListenerImpl::checkQuoteCount (
    MamdaSubscription*  subscription,
    const MamaMsg&      msg)
{
    // Check number of quotes for gaps
    mama_u32_t quoteCount = mQuoteCache.mTmpQuoteCount;
    mama_u16_t conflateCount = 0;
    if (!msg.tryU16 ("wConflateQuoteCount", 23, conflateCount))
    {
        conflateCount = 1;
    }

    if (quoteCount > 0)
    {
        if ((mQuoteCache.mQuoteCount > 0) &&
            (quoteCount > (mQuoteCache.mQuoteCount+conflateCount)))
        {
            mGapBegin = mQuoteCache.mQuoteCount+conflateCount;
            mGapEnd   = quoteCount-1;
            mQuoteCache.mQuoteCount = quoteCount;
            mFullBook->setIsConsistent (false);
            invokeGapHandlers (subscription, &msg);
        }
    }
    mQuoteCache.mQuoteCount = quoteCount;
}

void MamdaQuoteToBookListener::acquireReadLock ()  
{
    ACQUIRE_RLOCK(mImpl.mFullBookLock);
}

void MamdaQuoteToBookListener::releaseReadLock ()
{
    RELEASE_RLOCK(mImpl.mFullBookLock);
}

void MamdaQuoteToBookListener::acquireWriteLock ()
{
    ACQUIRE_WLOCK(mImpl.mFullBookLock);
}

void MamdaQuoteToBookListener::releaseWriteLock ()
{
    RELEASE_WLOCK(mImpl.mFullBookLock);
}

/**
* Implementation of MamdaListener interface.
*/
void MamdaQuoteToBookListener::onMsg (MamdaSubscription*  subscription,
                                      const MamaMsg&      msg,
                                      short               msgType)
{
    if (!mImpl.mSubscription)
    {
        mImpl.mSubscription = subscription;
    }
    try
    {
        mImpl.updateFieldStates();
        switch (msgType)
        {
        case MAMA_MSG_TYPE_PREOPENING:
        case MAMA_MSG_TYPE_QUOTE:
        case MAMA_MSG_TYPE_UPDATE:
        case MAMA_MSG_TYPE_TRADE:
            if ((mImpl.mFullBook->getQuality() != MAMA_QUALITY_OK) &&
                !mImpl.mUpdateStaleBook)
            {
                return;    
            }
            // fall through
        case MAMA_MSG_TYPE_SNAPSHOT:
        case MAMA_MSG_TYPE_INITIAL:
        case MAMA_MSG_TYPE_RECAP:
            mImpl.handleMessage (subscription, msg, msgType);
            break;
        default:
            break;
    }

    }
    catch (MamdaOrderBookException& e)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "MamdaQuoteToBookListener: caught exception for  %s : %s\n",
                  (subscription ? subscription->getSymbol() : "(nil)"),
                   e.what());
    }
    catch (MamaStatus& e)
    {
         mama_log (MAMA_LOG_LEVEL_ERROR,
                   "MamdaQuoteToBookListener: caught exception %s\n",
                   e.toString());
    }
}

MamdaQuoteToBookListenerImpl::MamdaQuoteToBookListenerImpl(
    MamdaQuoteToBookListener&  listener,
    MamdaOrderBook*          fullBook)
    : mListener             (listener)
    , mFullBook             (fullBook)    
    , mSubscription         (NULL)
    , mCurrentDeltaCount    (0)
    , mEventSeqNum          (0)
    , mGapBegin             (0)
    , mGapEnd               (0)
    , mUpdateStaleBook      (false)
    , mLocalFullBook        (false)
    , mClearStaleBook       (true)
    , mIsTransientMsg       (false)
    , mUpdateInconsistentBook (false)
    , mResolvePossiblyDuplicate (false)
    , mQuoteSizeMultiplier  (1)
    , mFullBookLock         (MamdaLock::SHARED, "Quote2BookListener(fullBook)")
    , mQuoteCache           (mRegularCache)
{
    if (!mFullBook)
    {
        mFullBook = new MamdaOrderBook;
        mLocalFullBook = true;
    }
    MamdaOrderBookComplexDelta::setProcessEntries (false);
    if (!mFieldUpdaters)
    {
        initFieldUpdaters ();
    }
    mQuoteCache.initialize();
}

MamdaQuoteToBookListenerImpl::~MamdaQuoteToBookListenerImpl()
{
    if (mLocalFullBook)
        delete mFullBook;    
}

void MamdaQuoteToBookListenerImpl::removeHandlers()
{
    mHandlers.clear ();
}

void MamdaQuoteToBookListenerImpl::clear()
{
    acquireLock();
    mQuoteCache.initialize();
    mEventSeqNum = 0;
    mGapBegin = 0;
    mGapEnd = 0;
    mFullBook->clear();
    releaseLock();
}

/*
* If wMsgQual (fid 21) is populated, perform actions based on whether its
* possible or definately a duplicate message 
* Sets mIsTransientMsg, mMsgQual
*/
bool MamdaQuoteToBookListenerImpl::evaluateMsgQual(
    MamdaSubscription*  subscription,
    const MamaMsg&      msg)
{
    bool isDuplicateMsg = false;
    mIsTransientMsg    = false;

    mMsgQual.clear();
    uint16_t msgQualVal = 0;
    // Does message contains a qualifier - need to parse this anyway.
    if (msg.tryU16 (MamaFieldMsgQual.mName,MamaFieldMsgQual.mFid ,msgQualVal))
    {
        mMsgQual.setValue(msgQualVal);

        // If qualifier indicates message is possbily duplicate
        // and the listener has been configure to attempt to 
        // resolve the ambiguity regarding the duplicate 
        // status of the message.
        if (mMsgQual.getIsPossiblyDuplicate() &&
            mResolvePossiblyDuplicate)
        {
            mama_seqnum_t seqNum = 0;
            MamaDateTime  eventTime;

            if (msg.tryU32 (MamdaQuoteFields::QUOTE_SEQ_NUM, seqNum) &&
                msg.tryDateTime (MamdaQuoteFields::QUOTE_TIME, eventTime))
            {
                // Only make a determination as to wheither or
                // not a PossiblyDuplicate msg is an actual duplicate
                // if the msg contains both a sequence number 
                // and event time.
                if ((seqNum < mRegularCache.mQuoteSeqNum) &&
                    (eventTime < mRegularCache.mQuoteTime))
                {
                    mMsgQual.setIsDefinatelyDuplicate (true);
                    mMsgQual.setIsPossiblyDuplicate  (false);
                }
            }
        }

        if (!(isDuplicateMsg = mMsgQual.getIsDefinatelyDuplicate()))
        {
            // If the message is possibly a duplicate or is 
            // out of sequence then is does not (on the whole)
            // qualify to update the regular record cache.
            mIsTransientMsg = (mMsgQual.getIsOutOfSequence() ||
                                mMsgQual.getIsPossiblyDuplicate());
        }
    }
    return isDuplicateMsg;
}

void MamdaQuoteToBookListenerImpl::updateQuoteFields (
    const MamaMsg&  msg)
{    
    mQuoteCache.mLastGenericMsgWasQuote = false;
    mQuoteCache.mGotBidPrice  = false;
    mQuoteCache.mGotAskPrice  = false;
    mQuoteCache.mGotBidSize   = false;
    mQuoteCache.mGotAskSize   = false;

    msg.iterateFields (*this, NULL, NULL); 

    // Check certain special fields.
    if (mQuoteCache.mGotBidSize    || mQuoteCache.mGotAskSize   ||
        mQuoteCache.mGotBidPrice   || mQuoteCache.mGotAskPrice)
    {
        mQuoteCache.mLastGenericMsgWasQuote = true;
    }
}

void MamdaQuoteToBookListenerImpl::handleMessage (MamdaSubscription*  subscription,
                                                  const MamaMsg&      msg,
                                                  short               msgType)
{
    if (!mFieldUpdaters)
    {
        initFieldUpdaters ();
    }

    if (evaluateMsgQual(subscription, msg))
    {
        return;
    }

    if (mIsTransientMsg)
    {
        mama_i32_t quoteCount = 0;
        if (msg.tryI32 (MamdaQuoteFields::QUOTE_COUNT, quoteCount))
           mQuoteCache.mQuoteCount = quoteCount;
        return;
    }

    acquireLock();
    mCurrentDeltaCount = 0;

    // use quote information to populate book and inform handler
    switch (msgType)
    {
    case MAMA_MSG_TYPE_INITIAL:
    case MAMA_MSG_TYPE_RECAP:
    case MAMA_MSG_TYPE_PREOPENING:
    case MAMA_MSG_TYPE_SNAPSHOT:
        handleRecap (subscription, msg);
        break;

    case MAMA_MSG_TYPE_QUOTE:
        updateQuoteFields (msg);
        handleQuote (subscription, msg);
        break;

    case MAMA_MSG_TYPE_UPDATE:
    case MAMA_MSG_TYPE_TRADE:
        updateQuoteFields (msg);

        if (mQuoteCache.mLastGenericMsgWasQuote)
        {
            handleQuote (subscription, msg);
        }
        break;
    }

    releaseLock();
}

void MamdaQuoteToBookListenerImpl::invokeRecapHandlers (
    MamdaSubscription*  subscription,
    const MamaMsg*      msg)
{
    deque<MamdaOrderBookHandler*>::iterator end = mHandlers.end();
    deque<MamdaOrderBookHandler*>::iterator i   = mHandlers.begin();
    for (; i != end; ++i)
    {
        MamdaOrderBookHandler* handler = *i;
        handler->onBookRecap (subscription, mListener, msg, NULL, *this,
                                  *mFullBook);
    }
}

void MamdaQuoteToBookListenerImpl::invokeClearHandlers (
    MamdaSubscription*  subscription,
    const MamaMsg*      msg)
{
    std::deque<MamdaOrderBookHandler*>::iterator end = mHandlers.end();
    std::deque<MamdaOrderBookHandler*>::iterator i   = mHandlers.begin();
    for (; i != end; ++i)
    {
        MamdaOrderBookHandler* handler = *i;
        handler->onBookClear (subscription, mListener, msg, *this,
                              *mFullBook);
    }
}

void MamdaQuoteToBookListenerImpl::invokeDeltaHandlers (
    MamdaSubscription*  subscription,
    const MamaMsg*      msg)
{
    if (mCurrentDeltaCount == 0)
    {
        // no deltas - no change to the top of book on this update.
        return;
    }
    deque<MamdaOrderBookHandler*>::iterator end = mHandlers.end();
    deque<MamdaOrderBookHandler*>::iterator i   = mHandlers.begin();
    for (; i != end; ++i)
    {
        MamdaOrderBookHandler* handler = *i;

        if (mCurrentDeltaCount == 1)
        {
            handler->onBookDelta (subscription, mListener, msg,
                                  *this, *mFullBook);
        }
        else
        {
            MamdaOrderBookComplexDelta::setOrderBook(mFullBook);
            handler->onBookComplexDelta (subscription, mListener, msg,
                                         *this, *mFullBook);
            MamdaOrderBookComplexDelta::clear();
        }
    }

    mFullBook->cleanupDetached();
    mCurrentDeltaCount =0;
}

void MamdaQuoteToBookListenerImpl::invokeGapHandlers (
    MamdaSubscription*  subscription,
    const MamaMsg*      msg)
{
    deque<MamdaOrderBookHandler*>::iterator end = mHandlers.end();
    deque<MamdaOrderBookHandler*>::iterator i   = mHandlers.begin();
    for (; i != end; ++i)
    {
        MamdaOrderBookHandler* handler = *i;
        handler->onBookGap (subscription, mListener, msg, *this, *mFullBook);
    }
}

void MamdaQuoteToBookListenerImpl::handleRecap (MamdaSubscription*  subscription,
                                const MamaMsg&      msg)
{
    // Clear the book
    mFullBook->clear();

    // Clear ask/bid 
    mQuoteCache.mBidPrice = 0.0;
    mQuoteCache.mBidSize  = 0.0;
    mQuoteCache.mAskPrice = 0.0;
    mQuoteCache.mAskSize  = 0.0;

    // get all the fields out of the message
    updateQuoteFields (msg);

    checkQuoteCount (subscription, msg);

    if (mFullBook->getIsConsistent()==false)
    {
        mama_log (MAMA_LOG_LEVEL_NORMAL,
            "Received Recap: Book now consistent for  %s\n",
            (subscription ? subscription->getSymbol() : "no symbol"));
    }

    MamdaOrderBookPriceLevel* level = NULL;
    if (mQuoteCache.mGotBidSize || mQuoteCache.mGotBidPrice)
    {
        addLevel (level, 
                  mQuoteCache.mBidPrice.getValue(), 
                  mQuoteCache.mBidSize, 
                  MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID,
                  mQuoteCache.mQuoteTime);
    }

    if (mQuoteCache.mGotAskSize || mQuoteCache.mGotAskPrice)
    {
        addLevel (level, 
                  mQuoteCache.mAskPrice.getValue(), 
                  mQuoteCache.mAskSize, 
                  MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_ASK,
                  mQuoteCache.mQuoteTime);
    }

    mFullBook->setIsConsistent (true);
    setQuality (subscription, MAMA_QUALITY_OK);
    invokeRecapHandlers (subscription, &msg);
    MamdaOrderBookComplexDelta::clear();
}

void MamdaQuoteToBookListenerImpl::handleQuote (MamdaSubscription*  subscription,
                                                const MamaMsg&      msg)
{
    checkQuoteCount (subscription, msg);

    MamdaOrderBookPriceLevel* level = NULL;
    if (mQuoteCache.mGotBidSize || mQuoteCache.mGotBidPrice)
    {
        // get current level
        MamdaOrderBook::bidIterator bidIter = mFullBook->bidBegin();
        if (bidIter == mFullBook->bidEnd())
            level = NULL;
        else
            level = *bidIter;
        if (level == NULL)
        {
            if (mQuoteCache.mBidSize == (mama_quantity_t)0 || mQuoteCache.mBidPrice.isZero())
            {
                if (mQuoteCache.mBidSize == (mama_quantity_t)0 && mQuoteCache.mBidPrice.isZero())
                {
                    mama_log (MAMA_LOG_LEVEL_WARN,
                              "MamdaQuoteToBookListener: Got bid update, but price and size are 0\n");
                }
            }
            else
            {
                addLevel (level, 
                          mQuoteCache.mBidPrice.getValue(), 
                          mQuoteCache.mBidSize, 
                          MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID, 
                          mQuoteCache.mQuoteTime);
            }
        }
        else
        {
            if ((mQuoteCache.mBidSize == 0) || (mQuoteCache.mBidPrice.isZero()))
            {
                deleteLevel (level, MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID, mQuoteCache.mQuoteTime);
            }
            else if (mQuoteCache.mBidPrice == level->getPrice())
            {
                updateLevel (level,  
                             mQuoteCache.mBidSize, 
                             mQuoteCache.mBidSize - level->getSize(), 
                             MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID, 
                             mQuoteCache.mQuoteTime);
            }
            else
            {
                deleteLevel (level, 
                             MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID, 
                             mQuoteCache.mQuoteTime);
                addLevel (level, 
                          mQuoteCache.mBidPrice.getValue(), 
                          mQuoteCache.mBidSize, 
                          MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID,
                          mQuoteCache.mQuoteTime);
            }
        }
    } 

    if (mQuoteCache.mGotAskSize || mQuoteCache.mGotAskPrice)
    {
        // get current level
        MamdaOrderBook::askIterator askIter = mFullBook->askBegin();
        if (askIter == mFullBook->askEnd())
            level = NULL;
        else
            level = *askIter;
        if (level == NULL)
        {
            if (mQuoteCache.mAskSize == (mama_quantity_t)0 || mQuoteCache.mAskPrice.isZero())
            {
                if (mQuoteCache.mAskSize == (mama_quantity_t)0 && mQuoteCache.mAskPrice.isZero())
                {
                    mama_log (MAMA_LOG_LEVEL_WARN,
                        "MamdaQuoteToBookListener: Got ask update, but price and size are 0\n");
                }
            }
            else
            {
                addLevel (level, 
                          mQuoteCache.mAskPrice.getValue(), 
                          mQuoteCache.mAskSize, 
                          MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_ASK, 
                          mQuoteCache.mQuoteTime);
            }
        }
        else
        {
            if ((mQuoteCache.mAskSize == 0) || (mQuoteCache.mAskPrice.isZero()))
            {
                deleteLevel (level, MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_ASK, mQuoteCache.mQuoteTime);
            }
            else if (mQuoteCache.mAskPrice == level->getPrice())
            {
                updateLevel (level, 
                             mQuoteCache.mAskSize, 
                             mQuoteCache.mAskSize - level->getSize(),
                             MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_ASK, 
                             mQuoteCache.mQuoteTime);
            }
            else
            {
                deleteLevel (level, 
                             MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_ASK, 
                             mQuoteCache.mQuoteTime);
                addLevel (level, 
                          mQuoteCache.mAskPrice.getValue(), 
                          mQuoteCache.mAskSize, 
                          MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_ASK, 
                          mQuoteCache.mQuoteTime);
            }
        }
    }
    
    if (mUpdateInconsistentBook || mFullBook->getIsConsistent())
    {
        invokeDeltaHandlers (subscription, &msg);
    } 
}

void MamdaQuoteToBookListenerImpl::addLevel (
    MamdaOrderBookPriceLevel*&        level,
    double                            plPrice,
    mama_f64_t                        plSize,
    MamdaOrderBookPriceLevel::Side    plSide,
    const MamaDateTime&               plTime)
{
    level = mFullBook->findOrCreateLevel (plPrice, plSide);
    level->setTime (plTime);
    level->setSize (plSize);
    addDelta (level, plSize, MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD);
}

void MamdaQuoteToBookListenerImpl::updateLevel (
    MamdaOrderBookPriceLevel*         level,
    mama_f64_t                        plSize,
    mama_quantity_t                   plSizeChange,
    MamdaOrderBookPriceLevel::Side    plSide,
    const MamaDateTime&               plTime)
{
    level->setTime (plTime);
    level->setSize (plSize);
    addDelta (level, plSizeChange, MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_UPDATE);
}

void MamdaQuoteToBookListenerImpl::deleteLevel (
    MamdaOrderBookPriceLevel*         level,
    MamdaOrderBookPriceLevel::Side    plSide,
    const MamaDateTime&               plTime)
{
    mFullBook->detach (level); // remove level from book, but still use level for indicating change
    level->setTime (plTime);
    level->setSize (0); 
    addDelta (level, (0-level->getSize()), MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_DELETE);
}

void MamdaQuoteToBookListenerImpl::addDelta (MamdaOrderBookPriceLevel*         level,
                   mama_quantity_t                   plDeltaSize,
                   MamdaOrderBookPriceLevel::Action  plAction)
{
    ++mCurrentDeltaCount;
    if (mCurrentDeltaCount == 1)
    {
        // This is number one, so save the "simple" delta. 
        MamdaOrderBookSimpleDelta::set (
            NULL, level, plDeltaSize, plAction, MamdaOrderBookEntry::MAMDA_BOOK_ACTION_UNKNOWN);
    }
    else if (mCurrentDeltaCount == 2)
    {
        // This is number two, so copy the saved "simple" delta to the
        // "complex" delta and add the current one. 
        MamdaOrderBookComplexDelta::clear();
        MamdaOrderBookComplexDelta::setOrderBook (mFullBook);
        MamdaOrderBookComplexDelta::add (*this);
        MamdaOrderBookComplexDelta::add (
            NULL, level, plDeltaSize, plAction, MamdaOrderBookEntry::MAMDA_BOOK_ACTION_UNKNOWN);
    }
    else
    {
        // This is number greater than two, so add the current delta. 
        MamdaOrderBookComplexDelta::add (
            NULL, level, plDeltaSize, plAction, MamdaOrderBookEntry::MAMDA_BOOK_ACTION_UNKNOWN);
    }
}

void MamdaQuoteToBookListenerImpl::setQuality (
    MamdaSubscription*  sub,
    mamaQuality         quality)
{
    if (mFullBook->getQuality() == quality)
        return;  // no change

    mFullBook->setQuality (quality);
    switch (quality)
    {
    case MAMA_QUALITY_OK:
        break;
    case MAMA_QUALITY_STALE:
    case MAMA_QUALITY_MAYBE_STALE:
    case MAMA_QUALITY_PARTIAL_STALE:
    case MAMA_QUALITY_FORCED_STALE:
    case MAMA_QUALITY_UNKNOWN:
        if (mClearStaleBook)
        {
            acquireLock();
            clear();
            invokeClearHandlers (sub, NULL);
            releaseLock();
        }
    }
}

MamdaFieldState 
MamdaQuoteToBookListenerImpl::getSymbolFieldState() const
{
    return mQuoteCache.mSymbolFieldState;
}

MamdaFieldState
MamdaQuoteToBookListenerImpl::getPartIdFieldState() const
{
    return mQuoteCache.mPartIdFieldState;
} 

MamdaFieldState  
MamdaQuoteToBookListenerImpl::getEventSeqNumFieldState() const
{
    return mQuoteCache.mEventSeqNumFieldState;
}

MamdaFieldState 
MamdaQuoteToBookListenerImpl::getEventTimeFieldState() const
{
    return mQuoteCache.mEventTimeFieldState;
}

MamdaFieldState 
MamdaQuoteToBookListenerImpl::getSrcTimeFieldState() const
{
    return mQuoteCache.mSrcTimeFieldState;
}

MamdaFieldState 
MamdaQuoteToBookListenerImpl::getActivityTimeFieldState() const
{
    return mQuoteCache.mActivityTimeFieldState;
}

MamdaFieldState 
MamdaQuoteToBookListenerImpl::getLineTimeFieldState() const
{
    return mQuoteCache.mLineTimeFieldState;
}

MamdaFieldState 
MamdaQuoteToBookListenerImpl::getSendTimeFieldState() const
{
    return mQuoteCache.mSendTimeFieldState;
}

MamdaFieldState 
MamdaQuoteToBookListenerImpl::getMsgQualFieldState() const
{
    return mQuoteCache.mMsgQualFieldState;
}

const MamdaOrderBook* MamdaQuoteToBookListenerImpl::getOrderBook () const
{
    if (mFullBook)
    {
        return mFullBook;
    }
    else
    {
        throw MamdaOrderBookException (
            "Attempt to access a NULL full order book");
    }
}

mama_seqnum_t MamdaQuoteToBookListenerImpl::getBeginGapSeqNum() const
{
    return mGapBegin;
}

mama_seqnum_t MamdaQuoteToBookListenerImpl::getEndGapSeqNum() const
{
    return mGapEnd;
}

const char* MamdaQuoteToBookListenerImpl::getSymbol() const
{
    if (mFullBook)
        return mFullBook->getSymbol();
    else
        return "unknown-symbol";
}

const char* MamdaQuoteToBookListenerImpl::getPartId() const
{
    return "";
}

mama_seqnum_t MamdaQuoteToBookListenerImpl::getEventSeqNum() const
{
    return mEventSeqNum;
}

const MamaDateTime& MamdaQuoteToBookListenerImpl::getEventTime() const
{
    return mQuoteCache.mQuoteTime;
}

const MamaDateTime& MamdaQuoteToBookListenerImpl::getSrcTime() const
{
    return mQuoteCache.mSrcTime;
}

const MamaDateTime& MamdaQuoteToBookListenerImpl::getLineTime() const
{
    return mQuoteCache.mLineTime;
}

const MamaDateTime& MamdaQuoteToBookListenerImpl::getActivityTime() const
{
    return mQuoteCache.mActTime;
}

const MamaDateTime& MamdaQuoteToBookListenerImpl::getSendTime() const
{
    return mQuoteCache.mSendTime;
}

const MamaMsgQual&  MamdaQuoteToBookListenerImpl::getMsgQual() const
{
    return mMsgQual;
}


void MamdaQuoteToBookListenerImpl::onField (const MamaMsg&      msg,
                                            const MamaMsgField& field,
                                            void*               closure)
{
    uint16_t fid = field.getFid();
    if (fid <= mFieldUpdatersSize)
    {
        QuoteToBookFieldUpdate* updater = mFieldUpdaters[fid];
        if (updater)
        {
            updater->onUpdate (*this, field);
        }
    }
}

struct MamdaQuoteToBookListenerImpl::FieldUpdateSymbol
    : public QuoteToBookFieldUpdate
{
    void onUpdate (MamdaQuoteToBookListenerImpl&  impl,
                   const MamaMsgField&            field)
    {
        if (impl.mFullBook && impl.mFullBook->getSymbol()[0]=='\0')
        {
            const char* result = NULL;
            result = field.getString();
            if (result)
            {
                impl.mFullBook->setSymbol (result);
            }
        }
    }
};

struct MamdaQuoteToBookListenerImpl::FieldUpdateIssueSymbol
    : public QuoteToBookFieldUpdate
{
    void onUpdate (MamdaQuoteToBookListenerImpl&  impl,
                   const MamaMsgField&            field)
    {
        if (impl.mFullBook && impl.mFullBook->getSymbol()[0]=='\0')
        {
            const char* result = NULL;
            result = field.getString();
            if (result)
            {
                impl.mFullBook->setSymbol (result);
            }
        }
    }
};

struct MamdaQuoteToBookListenerImpl::FieldUpdateQuoteSrcTime
    : public QuoteToBookFieldUpdate
{
    void onUpdate (MamdaQuoteToBookListenerImpl&  impl,
                   const MamaMsgField&            field)
    {
;
        field.getDateTime(impl.mQuoteCache.mSrcTime);
        impl.mQuoteCache.mSrcTimeFieldState = MODIFIED;
    }
};

struct MamdaQuoteToBookListenerImpl::FieldUpdateQuoteActTime
    : public QuoteToBookFieldUpdate
{
    void onUpdate (MamdaQuoteToBookListenerImpl&  impl,
                   const MamaMsgField&            field)
    {
        field.getDateTime(impl.mQuoteCache.mActTime);
        impl.mQuoteCache.mActivityTimeFieldState = MODIFIED;
    }
};

struct MamdaQuoteToBookListenerImpl::FieldUpdateQuoteLineTime
    : public QuoteToBookFieldUpdate
{
    void onUpdate (MamdaQuoteToBookListenerImpl&  impl,
                   const MamaMsgField&            field)
    {
        field.getDateTime(impl.mQuoteCache.mLineTime);
        impl.mQuoteCache.mLineTimeFieldState = MODIFIED;
    }
};

struct MamdaQuoteToBookListenerImpl::FieldUpdateQuoteSendTime
    : public QuoteToBookFieldUpdate
{
    void onUpdate (MamdaQuoteToBookListenerImpl&  impl,
                   const MamaMsgField&            field)
    {
        field.getDateTime(impl.mQuoteCache.mSendTime);
        impl.mQuoteCache.mSendTimeFieldState = MODIFIED;
    }
};
    
struct MamdaQuoteToBookListenerImpl::FieldUpdateBidPrice
    : public QuoteToBookFieldUpdate
{
    void onUpdate (MamdaQuoteToBookListenerImpl&  impl,
                   const MamaMsgField&            field)
    {
        MamaPrice bidPrice;
        field.getPrice(bidPrice);

        if (impl.mQuoteCache.mBidPrice != bidPrice)
        {
            impl.mQuoteCache.mBidPrice = bidPrice;
            impl.mQuoteCache.mGotBidPrice = true;
        }
    }
};

struct MamdaQuoteToBookListenerImpl::FieldUpdateBidSize
    : public QuoteToBookFieldUpdate
{   
    void onUpdate (MamdaQuoteToBookListenerImpl&  impl,
                   const MamaMsgField&            field)
    {
        mama_quantity_t bidSize = field.getF64() * impl.mQuoteSizeMultiplier;

        if (impl.mQuoteCache.mBidSize != bidSize)
        {
            impl.mQuoteCache.mBidSize = bidSize;
            impl.mQuoteCache.mGotBidSize = true;
        }
    }
};

struct MamdaQuoteToBookListenerImpl::FieldUpdateAskPrice
    : public QuoteToBookFieldUpdate
{
    void onUpdate (MamdaQuoteToBookListenerImpl&  impl,
                   const MamaMsgField&            field)
    {
        MamaPrice askPrice;
        field.getPrice (askPrice);

        if (impl.mQuoteCache.mAskPrice != askPrice)
        {
            impl.mQuoteCache.mAskPrice = askPrice;
            impl.mQuoteCache.mGotAskPrice = true;
        }
    }
};

struct MamdaQuoteToBookListenerImpl::FieldUpdateAskSize 
    : public QuoteToBookFieldUpdate
{
    void onUpdate (MamdaQuoteToBookListenerImpl&  impl,
                   const MamaMsgField&            field)
    {
        mama_quantity_t askSize = field.getF64() * impl.mQuoteSizeMultiplier;

        if (impl.mQuoteCache.mAskSize != askSize)
        {
            impl.mQuoteCache.mAskSize = askSize;
            impl.mQuoteCache.mGotAskSize = true;
        }
    }
};

struct MamdaQuoteToBookListenerImpl::FieldUpdateQuoteTime
    : public QuoteToBookFieldUpdate
{
    void onUpdate (MamdaQuoteToBookListenerImpl&  impl,
                   const MamaMsgField&            field)
    {
        field.getDateTime(impl.mQuoteCache.mQuoteTime);
    }
};

struct MamdaQuoteToBookListenerImpl::FieldUpdateQuoteSeqNum
    : public QuoteToBookFieldUpdate
{
    void onUpdate (MamdaQuoteToBookListenerImpl&  impl,
                   const MamaMsgField&            field)
    {
        impl.mQuoteCache.mQuoteSeqNum = field.getU32();
    }
};

struct MamdaQuoteToBookListenerImpl::FieldUpdateQuoteDate
    : public QuoteToBookFieldUpdate
{   
    void onUpdate (MamdaQuoteToBookListenerImpl&  impl,
                   const MamaMsgField&            field)
    {
        field.getDateTime(impl.mQuoteCache.mQuoteDate);
    }
};  

struct MamdaQuoteToBookListenerImpl::FieldUpdateQuoteQual
    : public QuoteToBookFieldUpdate
{   
    void onUpdate (MamdaQuoteToBookListenerImpl&  impl,
                   const MamaMsgField&            field)
    {
        impl.mQuoteCache.mQuoteQualStr = field.getString();
    }
};

struct MamdaQuoteToBookListenerImpl::FieldUpdateQuoteQualNative
    : public QuoteToBookFieldUpdate
{   
    void onUpdate (MamdaQuoteToBookListenerImpl&  impl,
                   const MamaMsgField&            field)
    {
        if (field.getType () == MAMA_FIELD_TYPE_STRING)
        {
            impl.mQuoteCache.mQuoteQualNative = field.getString();
        }
        else if (field.getType () == MAMA_FIELD_TYPE_CHAR)
        {
            impl.mQuoteCache.mQuoteQualNative = field.getChar();
        }
    }
};
    
struct MamdaQuoteToBookListenerImpl::FieldUpdateTmpQuoteCount
    : public QuoteToBookFieldUpdate
{
    void onUpdate (MamdaQuoteToBookListenerImpl&  impl,
                   const MamaMsgField&            field)
    {
        impl.mQuoteCache.mTmpQuoteCount = field.getU32();
    }
};

struct MamdaQuoteToBookListenerImpl::FieldUpdateAskTime
    : public QuoteToBookFieldUpdate
{
    void onUpdate (MamdaQuoteToBookListenerImpl&  impl,
                   const MamaMsgField&            field)
    {
        field.getDateTime(impl.mQuoteCache.mAskTime);
    }
};
    
struct MamdaQuoteToBookListenerImpl::FieldUpdateBidTime
    : public QuoteToBookFieldUpdate
{
    void onUpdate (MamdaQuoteToBookListenerImpl&  impl,
                   const MamaMsgField&            field)
    {
        field.getDateTime(impl.mQuoteCache.mBidTime);
    }
};



void MamdaQuoteToBookListenerImpl::initFieldUpdaters ()
{
    if (!mFieldUpdaters)
    {
        // create empty placeholders for each fid
        mFieldUpdaters =
            new QuoteToBookFieldUpdate*[MamdaQuoteFields::getMaxFid()+1];
        mFieldUpdatersSize = MamdaQuoteFields::getMaxFid();
        for (uint16_t i = 0; i <= mFieldUpdatersSize; ++i)
        {
            mFieldUpdaters[i] = NULL;
        }
    }

    // create callbacks for interested fids, so we can record field values
    initFieldUpdater(MamdaCommonFields::SYMBOL,
                     new MamdaQuoteToBookListenerImpl::FieldUpdateSymbol);
    initFieldUpdater(MamdaCommonFields::ISSUE_SYMBOL,
                     new MamdaQuoteToBookListenerImpl::FieldUpdateIssueSymbol);
    initFieldUpdater(MamdaCommonFields::SRC_TIME,
                     new MamdaQuoteToBookListenerImpl::FieldUpdateQuoteSrcTime);
    initFieldUpdater(MamdaCommonFields::ACTIVITY_TIME,
                     new MamdaQuoteToBookListenerImpl::FieldUpdateQuoteActTime);
    initFieldUpdater(MamdaCommonFields::LINE_TIME,
                     new MamdaQuoteToBookListenerImpl::FieldUpdateQuoteLineTime);
    initFieldUpdater(MamdaCommonFields::SEND_TIME,
                     new MamdaQuoteToBookListenerImpl::FieldUpdateQuoteSendTime);
    initFieldUpdater(MamdaQuoteFields::BID_PRICE,
                     new MamdaQuoteToBookListenerImpl::FieldUpdateBidPrice);
    initFieldUpdater(MamdaQuoteFields::BID_SIZE,
                     new MamdaQuoteToBookListenerImpl::FieldUpdateBidSize);
    initFieldUpdater(MamdaQuoteFields::ASK_PRICE,
                     new MamdaQuoteToBookListenerImpl::FieldUpdateAskPrice);
    initFieldUpdater(MamdaQuoteFields::ASK_SIZE,
                     new MamdaQuoteToBookListenerImpl::FieldUpdateAskSize);
    initFieldUpdater(MamdaQuoteFields::QUOTE_TIME,
                     new MamdaQuoteToBookListenerImpl::FieldUpdateQuoteTime);
    initFieldUpdater(MamdaQuoteFields::QUOTE_SEQ_NUM,
                     new MamdaQuoteToBookListenerImpl::FieldUpdateQuoteSeqNum);
    initFieldUpdater(MamdaQuoteFields::QUOTE_DATE,
                     new MamdaQuoteToBookListenerImpl::FieldUpdateQuoteDate);
    initFieldUpdater(MamdaQuoteFields::QUOTE_QUAL,
                     new MamdaQuoteToBookListenerImpl::FieldUpdateQuoteQual);
    initFieldUpdater(MamdaQuoteFields::QUOTE_QUAL_NATIVE,
                     new MamdaQuoteToBookListenerImpl::FieldUpdateQuoteQualNative);
    initFieldUpdater(MamdaQuoteFields::QUOTE_COUNT,
                     new MamdaQuoteToBookListenerImpl::FieldUpdateTmpQuoteCount);
    initFieldUpdater(MamdaQuoteFields::ASK_TIME,
                     new MamdaQuoteToBookListenerImpl::FieldUpdateAskTime);
    initFieldUpdater(MamdaQuoteFields::BID_TIME,
                     new MamdaQuoteToBookListenerImpl::FieldUpdateBidTime);
}

void MamdaQuoteToBookListenerImpl::initFieldUpdater (
    const MamaFieldDescriptor*  fieldDesc,
    QuoteToBookFieldUpdate*     updater)
{
    if (!fieldDesc)
        return;

    uint16_t fid = fieldDesc->getFid();
    if (fid <= mFieldUpdatersSize)
    {
        mFieldUpdaters[fid] = updater;
    }
}

void MamdaQuoteToBookListenerImpl::updateFieldStates()
{
    if (mQuoteCache.mSymbolFieldState == MODIFIED)    
        mQuoteCache.mSymbolFieldState = NOT_MODIFIED; 
    if (mQuoteCache.mPartIdFieldState == MODIFIED)   
        mQuoteCache.mPartIdFieldState = NOT_MODIFIED;
    if (mQuoteCache.mEventSeqNumFieldState == MODIFIED)    
        mQuoteCache.mEventSeqNumFieldState = NOT_MODIFIED; 
    if (mQuoteCache.mEventTimeFieldState == MODIFIED)   
        mQuoteCache.mEventTimeFieldState = NOT_MODIFIED;
    if (mQuoteCache.mSrcTimeFieldState == MODIFIED)    
        mQuoteCache.mSrcTimeFieldState = NOT_MODIFIED;
    if (mQuoteCache.mActivityTimeFieldState == MODIFIED)
        mQuoteCache.mActivityTimeFieldState = NOT_MODIFIED;
    if (mQuoteCache.mLineTimeFieldState == MODIFIED)     
        mQuoteCache.mLineTimeFieldState = NOT_MODIFIED;  
    if (mQuoteCache.mSendTimeFieldState == MODIFIED)    
        mQuoteCache.mSendTimeFieldState = NOT_MODIFIED; 
    if (mQuoteCache.mMsgQualFieldState == MODIFIED)   
        mQuoteCache.mMsgQualFieldState = NOT_MODIFIED; 

}

void QuoteCache::initialize()
{
    mSrcTime.clear();
    mActTime.clear();
    mLineTime.clear();
    mSendTime.clear();
    mBidPrice = 0.0;
    mBidSize  = 0.0;
    mAskPrice = 0.0;
    mAskSize  = 0.0;
    mQuoteTime.clear();
    mQuoteDate.clear();
    mAskTime.clear();
    mBidTime.clear();
    mTmpQuoteCount = 0;
    mQuoteCount    = 0;
    mQuoteSeqNum   = 0;
    mLastGenericMsgWasQuote = false;
    mGotBidPrice = false;
    mGotAskPrice = false;
    mGotBidSize  = false;
    mGotAskSize  = false; 
}

