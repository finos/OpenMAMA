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

#include <wombat/port.h>
#include <mamda/MamdaOrderBookListener.h>
#include <mamda/MamdaLock.h>
#include <mamda/MamdaOrderBookRecap.h>
#include <mamda/MamdaOrderBookSimpleDelta.h>
#include <mamda/MamdaOrderBookComplexDelta.h>
#include <mamda/MamdaOrderBookConcreteSimpleDelta.h>
#include <mamda/MamdaOrderBookConcreteComplexDelta.h>
#include <mamda/MamdaOrderBookGap.h>
#include <mamda/MamdaOrderBookFields.h>
#include <mamda/MamdaOrderBookHandler.h>
#include <mamda/MamdaOrderBookEntry.h>
#include <mamda/MamdaOrderBookEntryManager.h>
#include <mamda/MamdaCommonFields.h>
#include <mamda/MamdaSubscription.h>
#include <mamda/MamdaDataException.h>
#include <mamda/MamdaFieldState.h>
#include <MamdaUtils.h>
#include <mama/mamacpp.h>
#include <wombat/wtable.h>
#include <wombat/strutils.h>
#include <assert.h>
#include <string>
#include <map>
#include <deque>
#include <vector>
#include <iostream>
#include <math.h>

using std::map;
using std::string;
using std::deque;
using std::vector;
using std::cout;


namespace Wombat
{

    struct BookMsgFields
    {
        BookMsgFields()
            : mPriceLevelVector(NULL)
            , mNumLevels(1)
            , mBidMarketOrders (NULL)
            , mAskMarketOrders (NULL)
            , mEntryVector(NULL)
            , mPlNumAttach(1)
            , mBookPropertyFids (NULL)
            , mBookNumProps (0)
            , mPlPropertyFids (NULL)
            , mPlNumProps (0)
            , mEntryPropertyFids (NULL)
            , mEntryNumProps (0)
        { 
            clear();
        }
        ~BookMsgFields();
        void clear();
        void clearEntry();
        void clearPriceLevel();
        
        void clearEntries();

        mama_i16_t                  mMsgNum;                  MamdaFieldState mMsgNumFieldState; 
        mama_i16_t                  mMsgTotal;                MamdaFieldState mMsgTotalFieldState;

        // The following fields are used for caching the order book and
        // related fields.  These fields can be used by applications for
        // reference and will be passed for recaps.

        MamaDateTime                     mSrcTime;             MamdaFieldState mSrcTimeFieldState; 
        MamaDateTime                     mLineTime;            MamdaFieldState mLineTimeFieldState;
        MamaDateTime                     mSendTime;            MamdaFieldState mSendTimeFieldState;
        MamaDateTime                     mActivityTime;        MamdaFieldState mActivityTimeFieldState;
        MamaMsgQual                      mMsgQual;             MamdaFieldState mMsgQualFieldState;  
        MamaDateTime                     mBookTime;            MamdaFieldState mBookTimeFieldState; 
        mama_seqnum_t                    mMsgSeqNum;           MamdaFieldState mMsgSeqNumFieldState; 
        mama_u64_t                       mSenderId;            MamdaFieldState mSenderIdFieldState;  
                                                               MamdaFieldState mSymbolFieldState; 
                                                               MamdaFieldState mPartIdFieldState;
                                                               MamdaFieldState mEventSeqNumFieldState;
                                                               MamdaFieldState mEventTimeFieldState;

        mama_u8_t                          mBookType;
        // PriceLevels:
        const mamaMsg*                     mPriceLevelVector;  
        mama_size_t                        mNumLevels;         
        MamaPrice                          mPlPrice;           
        MamdaOrderBookPriceLevel::Side     mPlSide;            
        MamaDateTime                       mPlTime;            
        bool                               mHasPlTime;
        MamdaOrderBookPriceLevel::Action   mPlAction;          
        bool                               mHasPlAction;
        mama_f64_t                         mPlSize;
        bool                               mHasPlSize;
        mama_quantity_t                    mPlSizeChange;
        bool                               mHasPlSizeChange;
        mama_f32_t                         mPlNumEntries;      
        vector<mamaMsg>                    mPriceLevels;        /* for RV books */    
        mamaMsg                            mBidMarketOrders;   
        mamaMsg                            mAskMarketOrders;   
        bool                               mHasMarketOrders;   

        // Entries:
        const mamaMsg*                     mEntryVector;       
        mama_size_t                        mPlNumAttach;       
        const char*                        mEntryId;           
        mama_quantity_t                    mEntrySize;         
        MamaDateTime                       mEntryTime;         
        MamdaOrderBookEntry::Action        mEntryAction;       
        MamdaOrderBookTypes::Reason        mEntryReason;       
        vector<mamaMsg>                    mEntries;           /* for RV MamdaFieldStatebooks */
        mama_u16_t                         mEntryStatus;       
       
        // Properties 
        mama_fid_t*                        mBookPropertyFids;
        mama_size_t                        mBookNumProps;
        MamdaOrderBookTypes::PropMsgType   mBookPropMsgType;
        bool                               mBookPropertyFidsChanged;

        mama_fid_t*                        mPlPropertyFids;
        mama_size_t                        mPlNumProps;
        MamdaOrderBookTypes::PropMsgType   mPlPropMsgType;
        bool                               mPlPropertyFidsChanged;

        mama_fid_t*                        mEntryPropertyFids;
        mama_size_t                        mEntryNumProps;
        MamdaOrderBookTypes::PropMsgType   mEntryPropMsgType;
        bool                               mEntryPropertyFidsChanged;
    };


    class MamdaOrderBookListener::MamdaOrderBookListenerImpl
        : public MamdaOrderBookRecap
        , public MamdaOrderBookClear
        , public MamdaOrderBookGap
        , public MamdaOrderBookSimpleDelta
        , public MamdaOrderBookComplexDelta
        , public MamaTimerCallback
    {
    public:
        MamdaOrderBookListenerImpl (MamdaOrderBookListener& listener);
        MamdaOrderBookListenerImpl (MamdaOrderBookListener& listener,
                                    MamdaOrderBook*         fullBook);
        virtual ~MamdaOrderBookListenerImpl ();

        void clear ();

        void removeHandlers ();

        //! \throw<MamdaOrderBookException>
        void handleClear            (MamdaSubscription*  subscription,
                                     const MamaMsg&      msg);

        //! \throw<MamdaOrderBookException>
        void handleRecap            (MamdaSubscription*  subscription,
                                     const MamaMsg&      msg);

        //! \throw<MamdaOrderBookException>
        void handleUpdate           (MamdaSubscription*  subscription,
                                     const MamaMsg&      msg);
        
        void updateFieldStates ();
        /*
         * processBookMessage processes a MamaMsg containing a partial
         * or full order book and returns whether processing is complete
         * (since a legacy message format (native MDRV) allowed a message
         * to be split into multiple RV messages).
         *
         * @param msg      The MamaMsg to process.
         * @param isRecap  Whether to process the message as a recap (as opposed to an update).
         *
         * \throw<MamdaOrderBookException>
         */
        void processBookMessage     (MamdaSubscription*  subscription,
                                     const MamaMsg&      msg,
                                     bool                checkSeqNum);

        void processLevelMessage    (const mamaMsg&      msg);
        void processEntryMessage    (const mamaMsg&      msg);
        void processEntries         (MamdaOrderBookPriceLevel*         level,
                                     MamdaOrderBookPriceLevel::Action  plAction);

        /*
         * createDelta uses the information obtained from processBookMessage
         * to construct a MamdaOrderBookSimpleDelta or
         * MamdaOrderBookComplexDelta.
         *
         * @param isRecap  Whether to process the message as a recap (as opposed to an update).
         *
         * \throw<MamdaOrderBookException>
         */
        bool createDelta            (bool                isRecap);

        void processLevelPart1 (
            MamdaOrderBookPriceLevel*&          level,
            MamaPrice&                          plPrice,
            MamdaOrderBookPriceLevel::Side      plSide,
            MamdaOrderBookPriceLevel::Action    plAction,
            MamdaOrderBookPriceLevel::OrderType orderType,
            const MamaDateTime&                 plTime);

        void processLevelPart2 (
            MamdaOrderBookPriceLevel*           level,
            MamdaOrderBookPriceLevel::Action    plAction,
            mama_f64_t                          plSize,
            mama_quantity_t                     plSizeChange,
            mama_f32_t                          plNumEntries);

        void processEntry (
            MamdaOrderBookPriceLevel*           level,
            MamdaOrderBookPriceLevel::Action    plAction,
            MamdaOrderBookEntry::Action         entryAction,
            MamdaOrderBookTypes::Reason         entryReason,
            const char*                         id,
            mama_quantity_t                     size,
            const MamaDateTime&                 entryTime,
            mama_u16_t                          status);

        void addIgnoreEntryId    (const char*  id);
        void removeIgnoreEntryId (const char*  id);
        bool ignoreEntryId       (const char*  id) const;

        void invokeClearHandlers (MamdaSubscription*  subscription,
                                  const MamaMsg*      msg);
        void invokeRecapHandlers (MamdaSubscription*  subscription,
                                  const MamaMsg*      msg);
        void invokeDeltaHandlers (MamdaSubscription*  subscription,
                                  const MamaMsg*      msg);
        void invokeComplexDeltaHandlers (MamdaSubscription*  subscription,
                                  const MamaMsg*      msg);
        void invokeGapHandlers   (MamdaSubscription*  subscription,
                                  const MamaMsg*      msg);
        void setQuality          (MamdaSubscription*  subscription,
                                  mamaQuality         quality);

        void setProcessMarketOrders (bool  process);
        bool getProcessMarketOrders () const;

        void onTimer             (MamaTimer* timer);

        void forceInvokeDeltaHandlers ();
                                                      
        // Inherited from MamdaBasicRecap and MamdaBasicEvent
        const char*          getSymbol         () const;
        const char*          getPartId         () const;
        const MamaDateTime&  getSrcTime        () const;
        const MamaDateTime&  getActivityTime   () const;
        const MamaDateTime&  getLineTime       () const;
        const MamaDateTime&  getSendTime       () const;
        const MamaMsgQual&   getMsgQual        () const;
        const MamaDateTime&  getEventTime      () const;
        mama_seqnum_t        getEventSeqNum    () const;
        
        MamdaFieldState     getSymbolFieldState()       const;
        MamdaFieldState     getPartIdFieldState()       const;
        MamdaFieldState     getEventSeqNumFieldState()  const;
        MamdaFieldState     getEventTimeFieldState()    const;
        MamdaFieldState     getSrcTimeFieldState()      const;
        MamdaFieldState     getActivityTimeFieldState() const;
        MamdaFieldState     getLineTimeFieldState()     const;
        MamdaFieldState     getSendTimeFieldState()     const;
        MamdaFieldState     getMsgQualFieldState()      const;

        MamdaFieldState  getPlTimeFieldState () const;
        // Inherited from MamdaOrderBookRecap
        const MamdaOrderBook*  getOrderBook    () const;
        
        MamdaOrderBook*  getOrderBook    ();

        // Inherited from MamdaOrderBookGap
        mama_seqnum_t        getBeginGapSeqNum () const;
        mama_seqnum_t        getEndGapSeqNum   () const;

        MamdaOrderBookListener&        mListener;

        // Lock to protected the impl's full order book.    
        MamdaLock                      mFullBookLock;
        MamdaOrderBook*                mFullBook;
        bool                           mLocalFullBook;
        inline void acquireLock ()
            { ACQUIRE_WLOCK(mFullBookLock); }
        inline void releaseLock ()
            { RELEASE_WLOCK(mFullBookLock); }

        deque<MamdaOrderBookHandler*>  mHandlers;

        // Additional fields for gaps:
        mama_seqnum_t               mEventSeqNum;
        mama_seqnum_t               mGapBegin;
        mama_seqnum_t               mGapEnd;
        mama_u64_t                  mPrevSenderId;

        bool                        mGotInitial;
        bool                        mHaveSanityCheckedBookDict;
        bool                        mUpdateInconsistentBook;
        bool                        mUpdateStaleBook;
        bool                        mClearStaleBook;
        bool                        mProcessEntries;
        bool                        mProcessMarketOrders;
        bool                        mCalcDeltaForRecap;

        // Deltas
        void addDelta (MamdaOrderBookEntry*                 entry,
                       MamdaOrderBookPriceLevel*            level,
                       mama_quantity_t                      plDeltaSize,
                       MamdaOrderBookPriceLevel::Action     plAction,
                       MamdaOrderBookEntry::Action          entryAction);
        
        void addMarketOrderDelta (
                       MamdaOrderBookEntry*              entry,
                       MamdaOrderBookPriceLevel*         level,
                       mama_quantity_t                   plDeltaSize,
                       MamdaOrderBookPriceLevel::Action  plAction,
                       MamdaOrderBookEntry::Action       entryAction);
                                      
        MamdaOrderBookSimpleDelta*       mSimpleMarketOrderDelta;
        MamdaOrderBookComplexDelta*      mComplexDeltaMarketOrderDelta;
        
        mama_u32_t                  mCurrentDeltaCount;
        mama_u32_t                  mCurrentMarketOrderDeltaCount;

        // Fast entry lookups:
        MamdaOrderBookEntryManager* mEntryManager;
        bool                        mUniqueEntryIds;

        // Ignored entries...
        wtable_t                    mIgnoredEntries;
        bool                        mHaveEntries;

        // Conflation
        bool                        mIgnoreUpdate;
        bool                        mConflateDeltas;
        double                      mConflationInterval;
        MamaTimer*                  mConflationTimer;
        MamdaSubscription*          mSubscription;
       
        
        BookMsgFields  mBookMsgFields;

        struct BookFieldUpdate
        {
            virtual void onUpdate (
                MamdaOrderBookListener::MamdaOrderBookListenerImpl&  impl,
                const mamaMsgField&                             mamaField) = 0;
        };

        static void initFieldUpdaters ();
        static void initFieldUpdater (const MamaFieldDescriptor*  fieldDesc,
                                      BookFieldUpdate*            updater);
        static void updateField (
            MamdaOrderBookListenerImpl&  impl,
            const mamaMsgField&          field);
        static BookFieldUpdate**  mFieldUpdaters;
        static volatile uint16_t           mFieldUpdatersSize;
        static wthread_static_mutex_t   mOrderBookFieldUpdaterLockMutex;
        static bool mUpdatersComplete;

        // Iterated MamaMsg field handlers:
        struct FieldUpdateMsgSeqNum;
        struct FieldUpdateMsgNum;
        struct FieldUpdateMsgTotal;
        struct FieldUpdateSymbol;
        struct FieldUpdatePartId;
        struct FieldUpdateSrcTime;
        struct FieldUpdateLineTime;
        struct FieldUpdateBookTime;
        struct FieldUpdateSendTime;
        struct FieldUpdateActivityTime;
        struct FieldUpdateNumLevels;
        struct FieldUpdatePriceLevels;
        struct FieldUpdateBidMarketOrders;
        struct FieldUpdateAskMarketOrders;
        struct FieldUpdatePlPrice;
        struct FieldUpdatePlSide;
        struct FieldUpdatePlAction;
        struct FieldUpdatePlSize;
        struct FieldUpdatePlSizeChange;
        struct FieldUpdatePlTime;
        struct FieldUpdatePlNumEntries;
        struct FieldUpdatePlNumAttach;
        struct FieldUpdateEntries;
        struct FieldUpdateEntryId;
        struct FieldUpdateEntrySize;
        struct FieldUpdateEntryTime;
        struct FieldUpdateEntryStatus;
        struct FieldUpdateEntryAction;
        struct FieldUpdateEntryReason;
        struct FieldUpdateEntry;
        struct FieldUpdateLevel;
        struct FieldUpdateBookProperties;
        struct FieldUpdatePlProperties;
        struct FieldUpdateEntryProperties;
        struct FieldUpdateBookPropMsgType;
        struct FieldUpdatePlPropMsgType;
        struct FieldUpdateEntryPropMsgType;
        struct FieldUpdateSenderId;
    };

    MamdaOrderBookListener::MamdaOrderBookListener (
        MamdaOrderBook*  fullBook)
        : mImpl (*new MamdaOrderBookListenerImpl(*this,fullBook))
    {
    }

    MamdaOrderBookListener::~MamdaOrderBookListener ()
    {
        delete &mImpl;
    }

    void MamdaOrderBookListener::addHandler (MamdaOrderBookHandler*  handler)
    {
        mImpl.mHandlers.push_back (handler);
    }

    void MamdaOrderBookListener::addIgnoreEntryId (const char*  entryId)
    {
        mImpl.addIgnoreEntryId (entryId);
    }

    void MamdaOrderBookListener::removeIgnoreEntryId (const char*  entryId)
    {
        mImpl.removeIgnoreEntryId (entryId);
    }

    void MamdaOrderBookListener::clear()
    {
        mImpl.clear();
    }

    void MamdaOrderBookListener::removeHandlers ()
    {
        mImpl.removeHandlers ();
    }

    const char* MamdaOrderBookListener::getSymbol () const
    {
        return mImpl.getSymbol ();
    }

    const char* MamdaOrderBookListener::getPartId () const
    {
        return mImpl.getPartId ();
    }

    const MamaDateTime& MamdaOrderBookListener::getSrcTime () const
    {
        return mImpl.getSrcTime ();
    }

    const MamaDateTime& MamdaOrderBookListener::getActivityTime () const
    {
        return mImpl.getActivityTime ();
    }

    const MamaDateTime& MamdaOrderBookListener::getLineTime () const
    {
        return mImpl.getLineTime ();
    }

    const MamaDateTime& MamdaOrderBookListener::getSendTime () const
    {
        return mImpl.getSendTime ();
    }

    const MamaMsgQual& MamdaOrderBookListener::getMsgQual () const
    {
        return mImpl.getMsgQual ();
    }

    const MamaDateTime& MamdaOrderBookListener::getEventTime () const
    {
        return mImpl.getEventTime ();
    }

    mama_seqnum_t MamdaOrderBookListener::getEventSeqNum () const
    {
        return mImpl.getEventSeqNum ();
    }

    const MamdaOrderBook* MamdaOrderBookListener::getOrderBook () const
    {
        return mImpl.getOrderBook ();
    }

    MamdaOrderBook* MamdaOrderBookListener::getOrderBook () 
    {
        return mImpl.getOrderBook ();
    }

    mama_seqnum_t MamdaOrderBookListener::getBeginGapSeqNum () const
    {
        return mImpl.getBeginGapSeqNum ();
    }

    mama_seqnum_t MamdaOrderBookListener::getEndGapSeqNum () const
    {
        return mImpl.getEndGapSeqNum ();
    }

    /*      FieldAccessors      */
    MamdaFieldState MamdaOrderBookListener::getSymbolFieldState() const
    {
        return mImpl.getSymbolFieldState ();
    }

    MamdaFieldState MamdaOrderBookListener::getPartIdFieldState() const
    {
        return mImpl.getPartIdFieldState ();
    }

    MamdaFieldState  MamdaOrderBookListener::getEventSeqNumFieldState() const
    {
        return mImpl.getEventSeqNumFieldState ();
    }

    MamdaFieldState MamdaOrderBookListener::getEventTimeFieldState() const
    {
        return mImpl.getEventTimeFieldState ();
    }

    MamdaFieldState MamdaOrderBookListener::getSrcTimeFieldState() const
    {
        return mImpl.getSrcTimeFieldState ();
    }

    MamdaFieldState MamdaOrderBookListener::getActivityTimeFieldState() const
    {
        return mImpl.getActivityTimeFieldState ();
    }

    MamdaFieldState MamdaOrderBookListener::getLineTimeFieldState() const
    {
        return mImpl.getLineTimeFieldState ();
    }

    MamdaFieldState MamdaOrderBookListener::getSendTimeFieldState() const
    {
        return mImpl.getSendTimeFieldState ();
    }

    MamdaFieldState MamdaOrderBookListener::getMsgQualFieldState() const
    {
        return mImpl.getMsgQualFieldState ();
    }

    /*  End FieldAccessors  */

    void MamdaOrderBookListener::setUseEntryManager (bool  useManager)
    {
        if (useManager && !mImpl.mEntryManager)
        {
            mImpl.mEntryManager = new MamdaOrderBookEntryManager (100);
        }
        else if (!useManager && mImpl.mEntryManager)
        {
            delete mImpl.mEntryManager;
            mImpl.mEntryManager = NULL;
        }
    }

    void MamdaOrderBookListener::setEntryIdsAreUnique (bool  uniqueIds)
    {
        mImpl.mUniqueEntryIds = uniqueIds;
    }

    void MamdaOrderBookListener::setKeepBasicDeltas (bool  keep)
    {
        mImpl.setKeepBasicDeltas(keep);
    }

    void MamdaOrderBookListener::setUpdateInconsistentBook (bool  update)
    {
        mImpl.mUpdateInconsistentBook = update;
    }

    void MamdaOrderBookListener::setUpdateStaleBook (bool  update)
    {
        mImpl.mUpdateStaleBook = update;
    }

    void MamdaOrderBookListener::setClearStaleBook (bool  clear)
    {
        mImpl.mClearStaleBook = clear;
    }

    void MamdaOrderBookListener::setQuality (MamdaSubscription*  sub,
                                             mamaQuality         quality)
    {
        mImpl.setQuality (sub, quality);
    }

    void MamdaOrderBookListener::setProcessEntries (bool  process)
    {
        mImpl.mProcessEntries = process;
        mImpl.MamdaOrderBookComplexDelta::setProcessEntries (process);
    }

    bool MamdaOrderBookListener::getProcessEntries() const
    {
        return mImpl.mProcessEntries;
    }

    void MamdaOrderBookListener::setProcessMarketOrders (bool  process)
    {
        mImpl.setProcessMarketOrders (process);
    }

    bool MamdaOrderBookListener::getProcessMarketOrders () const 
    {
        return mImpl.getProcessMarketOrders();
    }

    void MamdaOrderBookListener::onMsg (MamdaSubscription* subscription,
                                        const MamaMsg&     msg,
                                        short              msgType)
    {

        // Ensure that the field handling is set up (once for all
        // MamdaOrderBookListener instances).
        if (!mImpl.mUpdatersComplete)
        {
            wthread_static_mutex_lock (&mImpl.mOrderBookFieldUpdaterLockMutex);

            if (!mImpl.mUpdatersComplete)
            {
                if (!MamdaOrderBookFields::isSet())
                {
                     mama_log (MAMA_LOG_LEVEL_WARN,
                               "MamdaOrderBookListener: MamdaOrderBookFields::setDictionary() "
                               "has not been called.");
                     wthread_static_mutex_unlock (&mImpl.mOrderBookFieldUpdaterLockMutex);
                     return;
                }

                try
                {
                    mImpl.initFieldUpdaters ();
                }
                catch (MamaStatus &e)
                {
                    mama_log (MAMA_LOG_LEVEL_WARN,
                              "MamdaOrderBookListener: Could not set field updaters: %s",
                              e.toString ());
                    wthread_static_mutex_unlock (&mImpl.mOrderBookFieldUpdaterLockMutex);
                    return;
                }
                mImpl.mUpdatersComplete = true;
            }

            wthread_static_mutex_unlock (&mImpl.mOrderBookFieldUpdaterLockMutex);
        }

        // If msg is a orderBook-related message, invoke the
        // appropriate callback:
        if (gMamaLogLevel == MAMA_LOG_LEVEL_FINEST)
        mama_log (MAMA_LOG_LEVEL_FINEST,
                  "MamdaOrderBookListener: got message (type=%d)", msgType);
        if (!mImpl.mSubscription)
        {
            mImpl.mSubscription = subscription;
        }
        
        try
        {
            switch (msgType)
            {
            case MAMA_MSG_TYPE_BOOK_UPDATE:
                if ((mImpl.mFullBook->getQuality() != MAMA_QUALITY_OK) &&
                    !mImpl.mUpdateStaleBook)
                {
                    mama_log(MAMA_LOG_LEVEL_FINE,
                             "MamdaOrderBookListener: "
                             "ignoring update during stale status for %s",
                             (subscription ? getSymbol() : "(nil)"));
                    return;
                }
                mImpl.handleUpdate (subscription, msg);
                break;
            case MAMA_MSG_TYPE_BOOK_CLEAR:
                mImpl.handleClear (subscription, msg);
                break;
            case MAMA_MSG_TYPE_BOOK_INITIAL:     
            case MAMA_MSG_TYPE_BOOK_RECAP:
            case MAMA_MSG_TYPE_BOOK_SNAPSHOT:
                mImpl.handleRecap (subscription, msg);
                break;
            default:
                mama_log(MAMA_LOG_LEVEL_NORMAL,
                            "MamdaOrderBookListener: "
                            "ignoring invalid book message type %s for symbol %s",
                            mamaMsgType_stringForType( (mamaMsgType)msgType ),
                            (subscription ? getSymbol() : "(nil)"));
            }
        }
        catch (MamdaOrderBookException& e)
        {
            mama_log (MAMA_LOG_LEVEL_NORMAL,
                      "MamdaOrderBookListener: caught exception for %s: %s",
                      (subscription ? getSymbol() : "(nil)"), 
                      e.what());
            mama_log (MAMA_LOG_LEVEL_NORMAL,
                      "MamdaOrderBookListener: message was: %s",
                      msg.toString());
        }
        catch (MamaStatus& e)
        {
            mama_log (MAMA_LOG_LEVEL_NORMAL,
                      "MamdaOrderBookListener: caught MamaStatus exception: %s",
                      e.toString()); 
        }
        if (gMamaLogLevel == MAMA_LOG_LEVEL_FINEST)
            mama_log (MAMA_LOG_LEVEL_FINEST,
                  "MamdaOrderBookListener: done with message");
    }

    void MamdaOrderBookListener::setBookPropertyFids (mama_fid_t* fids, mama_size_t numFids)
    {

        mImpl.mBookMsgFields.mBookPropertyFids = fids;
        mImpl.mBookMsgFields.mBookNumProps = numFids;
        mImpl.mBookMsgFields.mBookPropertyFidsChanged = true;

    }

    void MamdaOrderBookListener::setLevelPropertyFids (mama_fid_t* fids, mama_size_t numFids)
    {
        mImpl.mBookMsgFields.mPlPropertyFids = fids;
        mImpl.mBookMsgFields.mPlNumProps = numFids;
        mImpl.mBookMsgFields.mPlPropertyFidsChanged = true;
    }

    void MamdaOrderBookListener::setEntryPropertyFids (mama_fid_t* fids, mama_size_t numFids)
    {
        mImpl.mBookMsgFields.mEntryPropertyFids = fids;
        mImpl.mBookMsgFields.mEntryNumProps = numFids;
        mImpl.mBookMsgFields.mEntryPropertyFidsChanged = true;
    }

    void MamdaOrderBookListener::setConflateDeltas (bool conflate)
    {
        mImpl.mConflateDeltas = conflate;
        mImpl.MamdaOrderBookComplexDelta::setConflateDeltas (conflate);
    }

    bool MamdaOrderBookListener::getConflateDeltas ()
    {
        return mImpl.mConflateDeltas;
    }

    void MamdaOrderBookListener::setConflationInterval (double interval)
    { 
        mImpl.mConflationInterval = interval;
    }

    MamdaOrderBookListener::MamdaOrderBookListenerImpl::MamdaOrderBookListenerImpl(
        MamdaOrderBookListener&  listener,
        MamdaOrderBook*          fullBook)
        : mListener (listener)
        , mFullBookLock (MamdaLock::SHARED, "MamdaOrderBookListener(fullBook)")
        , mFullBook (fullBook)
        , mLocalFullBook (false)
        , mEventSeqNum (0)
        , mGapBegin (0)
        , mGapEnd (0)
        , mGotInitial(false)
        , mHaveSanityCheckedBookDict (false)
        , mUpdateInconsistentBook (false)
        , mUpdateStaleBook (false)
        , mClearStaleBook (true)
        , mProcessEntries (true)
        , mProcessMarketOrders (false)
        , mCalcDeltaForRecap (false)
        , mSimpleMarketOrderDelta (NULL)
        , mComplexDeltaMarketOrderDelta (NULL)
        , mCurrentDeltaCount (0)
        , mCurrentMarketOrderDeltaCount (0)
        , mEntryManager (NULL)
        , mUniqueEntryIds (false)
        , mIgnoredEntries (NULL)
        , mHaveEntries (false) 
        , mIgnoreUpdate (false)
        , mConflateDeltas (false)
        , mConflationInterval (0.5)
        , mConflationTimer (NULL)
        , mSubscription (NULL)
    {
        if (!mFullBook)
        {
            mFullBook = new MamdaOrderBook;
            mLocalFullBook = true;
        }
        mBookMsgFields.clear();

    }

    MamdaOrderBookListener::MamdaOrderBookListenerImpl::~MamdaOrderBookListenerImpl()
    {
        if (mLocalFullBook)
        { 
                delete mFullBook;
        }
        if (mIgnoredEntries) wtable_destroy(mIgnoredEntries);
        if (mEntryManager) delete mEntryManager;
        if (mSimpleMarketOrderDelta) delete mSimpleMarketOrderDelta;
        if (mComplexDeltaMarketOrderDelta) delete mComplexDeltaMarketOrderDelta;
        if (mConflationTimer)
        {
            mConflationTimer->destroy();
            delete (mConflationTimer);
            mConflationTimer = NULL;
        }

    }

    BookMsgFields::~BookMsgFields()
    {
    }

    void MamdaOrderBookListener::MamdaOrderBookListenerImpl::clear()
    {
        mBookMsgFields.clear();
        mEventSeqNum = 0;
        mGapBegin = 0;
        mGapEnd = 0;
        mGotInitial = false;

        acquireLock();
        mFullBook->clear();
        releaseLock();

        if (mEntryManager)
        {
            mEntryManager->clear();
        }
    }

    void MamdaOrderBookListener::MamdaOrderBookListenerImpl::removeHandlers ()
    {
        mHandlers.clear ();
    }

    void MamdaOrderBookListener::MamdaOrderBookListenerImpl::handleClear (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg)
    {
        mama_log (MAMA_LOG_LEVEL_FINE,
                  "MamdaOrderBookListener: handling CLEAR for order book %s",
                  getSymbol());

        acquireLock();
        processBookMessage (subscription, msg, true);
        mFullBook->clear();
        mFullBook->setIsConsistent (true);
        if (mEntryManager)
        {
            mEntryManager->clear();
        }   
        MamdaOrderBookBasicDelta::clear();
        MamdaOrderBookComplexDelta::clear();

        invokeClearHandlers (subscription, &msg);
        releaseLock();
        mGotInitial = true;
    }

    void MamdaOrderBookListener::MamdaOrderBookListenerImpl::handleRecap (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg)
    {
        mama_log (MAMA_LOG_LEVEL_FINE,
                "MamdaOrderBookListener: handling INITIAL/RECAP for order book %s",
                getSymbol());

        bool complete = false;

        acquireLock();
        
        if (mFullBook->getIsConsistent()==false)
        {
            mama_log (MAMA_LOG_LEVEL_NORMAL,
                      "Received Recap: Book now consistent for [%s]",
                      (subscription ? getSymbol() : "no symbol"));
        }
        
        processBookMessage (subscription, msg, true);
        updateFieldStates();
        try
        {
            complete = createDelta (true);
            mFullBook->setIsConsistent (true);
            mPrevSenderId = mBookMsgFields.mSenderId;
        }
        catch (MamdaOrderBookException& e)
        {
            mama_log (MAMA_LOG_LEVEL_NORMAL,
                      "MamdaOrderBookListener: caught exception for %s: %s",
                      (subscription ? getSymbol() : "(nil)"), 
                      e.what());
        }
        catch (MamaStatus& e)
        {
            mama_log (MAMA_LOG_LEVEL_NORMAL,
                      "MamdaOrderBookListener: caught MamaStatus exception: %s",
                      e.toString()); 
        }

        if (complete)
        {
            // Always call the recap handler even in exceptional or error
            // situations
            setQuality (subscription, MAMA_QUALITY_OK);
            invokeRecapHandlers (subscription, &msg);
            /* Have to do this for conflation. For recaps a delta is created but
               used not */   
            MamdaOrderBookComplexDelta::clear();
        }
        releaseLock ();
        mGotInitial = true;
    }

    void MamdaOrderBookListener::MamdaOrderBookListenerImpl::handleUpdate (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg)
    {
        if (MAMA_LOG_LEVEL_FINEST == gMamaLogLevel)
            mama_log (MAMA_LOG_LEVEL_FINEST,
                  "MamdaOrderBookListener: handling update for order book %s",
                  getSymbol());
                  
        if (!mGotInitial)
        {
            throw MamdaOrderBookException ("got update before initial/recap");
        }

        acquireLock();
        processBookMessage (subscription, msg, false);
        
        if (mIgnoreUpdate)
        {  
            mIgnoreUpdate = false;
            releaseLock();
            return;
        }
        else
           updateFieldStates();
        
        try
        {
            createDelta (false);
        }
        catch (MamdaOrderBookException& e)
        {
            mama_log (MAMA_LOG_LEVEL_NORMAL,
                      "MamdaOrderBookListener: caught exception for %s: %s",
                      (subscription ? getSymbol() : "(nil)"), 
                      e.what());
        }
        catch (MamaStatus& e)
        {
            mama_log (MAMA_LOG_LEVEL_NORMAL,
                      "MamdaOrderBookListener: caught MamaStatus exception: %s",
                      e.toString()); 
        }

        if (!mHandlers.empty())
        {
            if (mUpdateInconsistentBook || mFullBook->getIsConsistent())
            {
                if (mConflateDeltas &&
                    !MamdaOrderBookComplexDelta::getSendImmediately())
                {
                    if (mConflationTimer)
                        return;

                    mConflationTimer = new MamaTimer ();
                    mConflationTimer->create (subscription->getQueue(),
                                               this,
                                               mConflationInterval,
                                               (void*) subscription);
                }
                else
                {
                    invokeDeltaHandlers (subscription, &msg);
                }
            }
            else
            {
                mama_log (MAMA_LOG_LEVEL_FINE,
                          "MamdaOrderBookListener: not forwarding update for "
                          "inconsistent order book %s",
                          getSymbol());
            }
        }

        releaseLock();
    }

    extern "C" 
    {
        static void MAMACALLTYPE msgItCb (
            const mamaMsg       msg,
            const mamaMsgField  field,
            void*               closure)
        {
            MamdaOrderBookListener::MamdaOrderBookListenerImpl* listenerImpl
                = (MamdaOrderBookListener::MamdaOrderBookListenerImpl*) (closure);
            
            mama_fid_t fid = 0;
            mamaMsgField_getFid (field, &fid);
            if (fid <= listenerImpl->mFieldUpdatersSize)
            {
                MamdaOrderBookListener::MamdaOrderBookListenerImpl::
                    BookFieldUpdate* updater = listenerImpl->mFieldUpdaters[fid];
                if (updater)
                {
                    updater->onUpdate (*listenerImpl, field);
                    return; // We found the field.
                }
            }
        }
    }

    void MamdaOrderBookListener::MamdaOrderBookListenerImpl::processBookMessage (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg,
        bool                isRecap)
    {

        mBookMsgFields.clear();
        
        mBookMsgFields.mBookType = 0;
        msg.tryU8 (MamdaOrderBookFields::BOOK_TYPE, mBookMsgFields.mBookType);

        // Iterate over all of the fields in the message.
        mamaMsg mama_msg = msg.getUnderlyingMsg();
        mamaMsg_iterateFields (mama_msg, msgItCb, NULL, this);
        if (mBookMsgFields.mBookTime.empty ())
        {
            mBookMsgFields.mBookTime =
                mBookMsgFields.mSrcTime;
        }
        mFullBook->setBookTime(mBookMsgFields.mBookTime);

    #ifdef WITH_UNITTESTS    
        if (0 == mBookMsgFields.mMsgSeqNum)
            msg.tryU32(MamdaCommonFields::MSG_SEQ_NUM, mBookMsgFields.mMsgSeqNum);
    #endif    

        if (mBookMsgFields.mMsgNum == 1)
        {
            // Only clear the book/delta if this is the first message in the
            // set of updates.
            mCurrentMarketOrderDeltaCount = 0;

            // Always reset the delta count if we're not conflating. When
            // conflating deltas persist between updates, so only reset
            // when it's a recap
            if (!mConflateDeltas)
            {
                mCurrentDeltaCount = 0;  
            }
            if (isRecap)
            {
                mCurrentDeltaCount = 0;

                mFullBook->clear();
                if (mEntryManager)
                {
                    mEntryManager->clear();
                }
            }
        }
        
        mama_seqnum_t  seqNum = mBookMsgFields.mMsgSeqNum;
        mama_u64_t senderId = mBookMsgFields.mSenderId;
        if (!isRecap && (seqNum == mEventSeqNum))
        {
            mama_log (MAMA_LOG_LEVEL_FINE,
                  "MamdaOrderBookListener: ignoring update after initial "
                  "(seqnum=%d)", seqNum);
            mIgnoreUpdate = true;
        }
        else if (!isRecap && (seqNum != 0) && (seqNum != (mEventSeqNum + 1)))
        {
            mGapBegin    = mEventSeqNum + 1;
            mGapEnd      = seqNum - 1;
            mEventSeqNum = seqNum;
            invokeGapHandlers (subscription, &msg);
            if (senderId == mPrevSenderId)
            {
                mFullBook->setIsConsistent (false);
            }
        }
        mPrevSenderId = senderId;
        mEventSeqNum = seqNum;
    }

    void MamdaOrderBookListener::MamdaOrderBookListenerImpl::updateFieldStates()
    {
        if (mBookMsgFields.mSymbolFieldState == MODIFIED)    
            mBookMsgFields.mSymbolFieldState = NOT_MODIFIED; 
        if (mBookMsgFields.mPartIdFieldState == MODIFIED)   
            mBookMsgFields.mPartIdFieldState = NOT_MODIFIED;
        if (mBookMsgFields.mEventSeqNumFieldState == MODIFIED)    
            mBookMsgFields.mEventSeqNumFieldState = NOT_MODIFIED; 
        if (mBookMsgFields.mEventTimeFieldState == MODIFIED)   
            mBookMsgFields.mEventTimeFieldState = NOT_MODIFIED;
        if (mBookMsgFields.mSrcTimeFieldState == MODIFIED)    
            mBookMsgFields.mSrcTimeFieldState = NOT_MODIFIED;
        if (mBookMsgFields.mActivityTimeFieldState == MODIFIED)
            mBookMsgFields.mActivityTimeFieldState = NOT_MODIFIED;
        if (mBookMsgFields.mLineTimeFieldState == MODIFIED)     
            mBookMsgFields.mLineTimeFieldState = NOT_MODIFIED;  
        if (mBookMsgFields.mSendTimeFieldState == MODIFIED)    
            mBookMsgFields.mSendTimeFieldState = NOT_MODIFIED; 
        if (mBookMsgFields.mMsgQualFieldState == MODIFIED)   
            mBookMsgFields.mMsgQualFieldState = NOT_MODIFIED; 

    }

    void MamdaOrderBookListener::MamdaOrderBookListenerImpl::processLevelMessage (
        const mamaMsg&      msg)
    {
        // Iterate over all of the fields in a price level submessage.
        mBookMsgFields.mEntryVector = NULL;

        mBookMsgFields.clearEntries();

        // Iterate over all of the fields in the message.
        mamaMsg_iterateFields (msg, msgItCb, NULL, this);
        if ((!mBookMsgFields.mHasPlTime) && (!mBookMsgFields.mBookTime.empty()))
        {
            mBookMsgFields.mPlTime = mBookMsgFields.mBookTime;        
        }
    }

    void MamdaOrderBookListener::MamdaOrderBookListenerImpl::processEntryMessage (
        const mamaMsg&      msg)
    {
        // Iterate over all of the fields in an entry submessage.
        mamaMsg_iterateFields (msg, msgItCb, NULL, this);
        if (mBookMsgFields.mEntryTime.empty ())
        {
            mBookMsgFields.mEntryTime = mBookMsgFields.mPlTime;
        }
    }

    bool MamdaOrderBookListener::MamdaOrderBookListenerImpl::createDelta(
        bool  isRecap)
    {
        if ((mBookMsgFields.mNumLevels == 0) &&
            (!mBookMsgFields.mHasMarketOrders))
        {
            mama_log (MAMA_LOG_LEVEL_FINEST,
                        "MamdaOrderBookListener::createDelta:- No price levels in update");                    
            return true;
        }
        if (mProcessMarketOrders)
        {
          if (mBookMsgFields.mBidMarketOrders) 
          {
            mBookMsgFields.clearPriceLevel();
            processLevelMessage (mBookMsgFields.mBidMarketOrders);
            MamdaOrderBookPriceLevel*  level = NULL;
            processLevelPart1 (level,
                               mBookMsgFields.mPlPrice,
                               mBookMsgFields.mPlSide,
                               mBookMsgFields.mPlAction,
                               MamdaOrderBookPriceLevel::MAMDA_BOOK_LEVEL_MARKET,
                               mBookMsgFields.mPlTime);
            processEntries    (level, mBookMsgFields.mPlAction);
            processLevelPart2 (level,
                               mBookMsgFields.mPlAction,
                               mBookMsgFields.mPlSize,
                               mBookMsgFields.mPlSizeChange,
                               mBookMsgFields.mPlNumEntries);
          }
          if (mBookMsgFields.mAskMarketOrders)
          {
            mBookMsgFields.clearPriceLevel();
            processLevelMessage (mBookMsgFields.mAskMarketOrders);
            MamdaOrderBookPriceLevel*  level = NULL;
            processLevelPart1 (level,
                               mBookMsgFields.mPlPrice,
                               mBookMsgFields.mPlSide,
                               mBookMsgFields.mPlAction,
                               MamdaOrderBookPriceLevel::MAMDA_BOOK_LEVEL_MARKET,
                               mBookMsgFields.mPlTime);
            processEntries    (level, mBookMsgFields.mPlAction);
            processLevelPart2 (level,
                               mBookMsgFields.mPlAction,
                               mBookMsgFields.mPlSize,
                               mBookMsgFields.mPlSizeChange,
                               mBookMsgFields.mPlNumEntries);
          }
          //Single Market Order Update
          if ((mBookMsgFields.mNumLevels == 0) && (mBookMsgFields.mHasMarketOrders))
            return true;
        }

        if (mBookMsgFields.mPriceLevelVector == NULL &&
            mBookMsgFields.mPriceLevels.empty ())
        {
            // Single, flat price level
            MamdaOrderBookPriceLevel* level = NULL;
            if (!mBookMsgFields.mHasPlTime)
            {
                mBookMsgFields.mPlTime = mBookMsgFields.mBookTime;
            }
            processLevelPart1 (level,
                               mBookMsgFields.mPlPrice,
                               mBookMsgFields.mPlSide,
                               mBookMsgFields.mPlAction,
                               MamdaOrderBookPriceLevel::MAMDA_BOOK_LEVEL_LIMIT,
                               mBookMsgFields.mPlTime);
            if (!level)
            {
                //level not found
                mama_log (MAMA_LOG_LEVEL_FINE,
                          " received delete for unknown price level (%.2f)",
                          mBookMsgFields.mPlPrice.getValue());
            }
            
            else
            {
                processEntries (level, mBookMsgFields.mPlAction);
                processLevelPart2 (level,
                                mBookMsgFields.mPlAction,
                                mBookMsgFields.mPlSize,
                                mBookMsgFields.mPlSizeChange,
                                mBookMsgFields.mPlNumEntries);
            }
        }
        else
        {
            if (mBookMsgFields.mPriceLevelVector != NULL)
            {
                // wombatMsg
                for (mama_size_t i = 0; i < mBookMsgFields.mNumLevels; ++i)
                {
                    const mamaMsg levelMsg = mBookMsgFields.mPriceLevelVector[i];
                    if (levelMsg)
                    {
                        mBookMsgFields.clearPriceLevel();
                        processLevelMessage (levelMsg);
                        MamdaOrderBookPriceLevel*  level = NULL;
                        processLevelPart1 (level,
                                           mBookMsgFields.mPlPrice,
                                           mBookMsgFields.mPlSide,
                                           mBookMsgFields.mPlAction,
                                           MamdaOrderBookPriceLevel::MAMDA_BOOK_LEVEL_LIMIT,
                                           mBookMsgFields.mPlTime);
                        if (!level)
                        {
                            // level not found
                            mama_log (MAMA_LOG_LEVEL_FINE,
                                      " received delete for unknown price level (%.2f)",
                                      mBookMsgFields.mPlPrice.getValue());
                        }
                        else
                        {
                            processEntries (level, mBookMsgFields.mPlAction);
                            processLevelPart2 (level,
                                            mBookMsgFields.mPlAction,
                                            mBookMsgFields.mPlSize,
                                            mBookMsgFields.mPlSizeChange,
                                            mBookMsgFields.mPlNumEntries);
                        }
                    }
                }
            }
            else
            {
                // RV message
                vector<mamaMsg>::iterator i = mBookMsgFields.mPriceLevels.begin();
                vector<mamaMsg>::iterator end = mBookMsgFields.mPriceLevels.end();
                for (; i != end; ++i)
                {
                    processLevelMessage (*i);
                    MamdaOrderBookPriceLevel*  level = NULL;
                    processLevelPart1 (level,
                                       mBookMsgFields.mPlPrice,
                                       mBookMsgFields.mPlSide,
                                       mBookMsgFields.mPlAction,
                                       MamdaOrderBookPriceLevel::MAMDA_BOOK_LEVEL_LIMIT,
                                       mBookMsgFields.mPlTime);
                    if (!level)
                    {
                        //level not found
                        mama_log (MAMA_LOG_LEVEL_FINE,
                                  " received delete for unknown price level (%.2f)",
                                  mBookMsgFields.mPlPrice.getValue());
                    }
                    else
                    {
                        processEntries (level, mBookMsgFields.mPlAction);
                        processLevelPart2 (level,
                                        mBookMsgFields.mPlAction,
                                        mBookMsgFields.mPlSize,
                                        mBookMsgFields.mPlSizeChange,
                                        mBookMsgFields.mPlNumEntries);
                    }
                }
            }
        }
        return mBookMsgFields.mMsgNum == mBookMsgFields.mMsgTotal;
    }

    void MamdaOrderBookListener::MamdaOrderBookListenerImpl::processEntries (
        MamdaOrderBookPriceLevel*         level,
        MamdaOrderBookPriceLevel::Action  plAction)
    {
        if ((!mProcessEntries && !mBookMsgFields.mBookType ==1) ||     mBookMsgFields.mBookType==2 )
        {
            return;
        }

        if (mBookMsgFields.mPlNumAttach == 0)
        {
            // No entries in level delta

            if (MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_DELETE != plAction)
            {
                // No entry delta; just this price level delta.
                addDelta (NULL, level, mBookMsgFields.mPlSizeChange, plAction,
                      MamdaOrderBookEntry::MAMDA_BOOK_ACTION_UNKNOWN);
                  return;
            }
            
            MamdaOrderBookPriceLevel::iterator end = level->end ();
            MamdaOrderBookPriceLevel::iterator i   = level->begin ();
            
            // Received PriceLevel Delete for priceLevel with no entries
            if (end == i)
            {

                // No entry delta; just this price level delta.
                addDelta (NULL, level, mBookMsgFields.mPlSizeChange, plAction,
                      MamdaOrderBookEntry::MAMDA_BOOK_ACTION_UNKNOWN);
                return;
            }

            // Special case where we receive a delete, we are processing
            // entries but the message has no entries.
            // Assume that all entries in the book are to be deleted and
            // build up the delta using the actual entries in the cached level.
            do 
            {
                MamdaOrderBookEntry* entry = *i;
                
                // Increment here before removing element otherwise the
                // iterator is invalidated
                ++i;
                
                // Don't call processEntry as we already have the entry we want
                // so we can skip stright to removing the entry and building up
                // the delta
                entry->setAction (MamdaOrderBookEntry::MAMDA_BOOK_ACTION_DELETE);
                if (mEntryManager)
                {
                    mEntryManager->removeEntry (entry);
                    entry->setManager (NULL);
                }
                level->removeEntry (entry);
                addDelta (entry, level, mBookMsgFields.mPlSizeChange,
                        MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_DELETE,
                        MamdaOrderBookEntry::MAMDA_BOOK_ACTION_DELETE);
            }
            while (i != end);

            return;
        }

        if (mBookMsgFields.mEntryVector == NULL && 
            mBookMsgFields.mEntries.empty ()    &&
            mBookMsgFields.mEntryId     != NULL)
        {
            // Single, flat entry
            if (mBookMsgFields.mEntryTime.empty ())
            {
                mBookMsgFields.mEntryTime = mBookMsgFields.mPlTime;
            }
            processEntry (level,
                          plAction,
                          mBookMsgFields.mEntryAction,
                          mBookMsgFields.mEntryReason,
                          mBookMsgFields.mEntryId,
                          mBookMsgFields.mEntrySize,
                          mBookMsgFields.mEntryTime,
                          mBookMsgFields.mEntryStatus);
            mBookMsgFields.clearEntry();
        }
        else
        {
            if (mBookMsgFields.mEntryVector != NULL)
            {
                for (mama_size_t i = 0; i < mBookMsgFields.mPlNumAttach; ++i)
                {
                    const mamaMsg entryMsg = mBookMsgFields.mEntryVector[i];
                    if (entryMsg)
                    {
                        processEntryMessage (entryMsg);
                        processEntry (level,
                                      plAction,
                                      mBookMsgFields.mEntryAction,
                                      mBookMsgFields.mEntryReason,
                                      mBookMsgFields.mEntryId,
                                      mBookMsgFields.mEntrySize,
                                      mBookMsgFields.mEntryTime,
                                      mBookMsgFields.mEntryStatus);
                        mBookMsgFields.clearEntry();                                  
                    }
                }
            }
            else // RV Message
            {
                vector<mamaMsg>::iterator i = mBookMsgFields.mEntries.begin ();
                vector<mamaMsg>::iterator end = mBookMsgFields.mEntries.end ();
                for (; i != end; ++i)
                {
                    processEntryMessage (*i);
                    processEntry (level,
                                  plAction,
                                  mBookMsgFields.mEntryAction,
                                  mBookMsgFields.mEntryReason,
                                  mBookMsgFields.mEntryId,
                                  mBookMsgFields.mEntrySize,
                                  mBookMsgFields.mEntryTime,
                                  mBookMsgFields.mEntryStatus);
                    mBookMsgFields.clearEntry();
                }
            }
        }
    }

    void MamdaOrderBookListener::MamdaOrderBookListenerImpl::processLevelPart1 (
        MamdaOrderBookPriceLevel*&          level,
        MamaPrice&                          plPrice,
        MamdaOrderBookPriceLevel::Side      plSide,
        MamdaOrderBookPriceLevel::Action    plAction,
        MamdaOrderBookPriceLevel::OrderType orderType,
        const MamaDateTime&                 plTime)
    {
        if (MamdaOrderBookPriceLevel::MAMDA_BOOK_LEVEL_MARKET == orderType)
        {
            level = mFullBook->getOrCreateMarketOrdersSide (plSide);
            level->setPrice(plPrice);
            if (plAction == MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_DELETE)
            {
                //We are deleting a level we've just created - FIX THIS!!!
                mFullBook->detach (level);
            }
        }
        else
        {
            if (MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_DELETE == plAction)
            {
                level = mFullBook->findLevel (plPrice, plSide);
                if (!level)
                {
                    return;
                }
                mFullBook->detach (level);
            }
            else
            {
                //Check if level exists
                MamdaOrderBookPriceLevel::Action findLevelAction = plAction;
                level = mFullBook->findOrCreateLevel (plPrice, plSide, findLevelAction);
                if (plAction != findLevelAction)
                {
                    //Fix the level action for the delta
                    plAction = findLevelAction;
                    mBookMsgFields.mPlAction = findLevelAction;
                }
            }
        }

        if (!plTime.empty())
        {
            level->setTime (plTime);
        }
    }

    void MamdaOrderBookListener::MamdaOrderBookListenerImpl::processLevelPart2 (
        MamdaOrderBookPriceLevel*           level,
        MamdaOrderBookPriceLevel::Action    plAction,
        mama_f64_t                          plSize,
        mama_quantity_t                     plSizeChange,
        mama_f32_t                          plNumEntries)
    {
        // Fix up explicit numbers provided by the message.
        level->setSize       (plSize);
        if (!mHaveEntries)
            level->setNumEntries ((mama_u32_t)plNumEntries);

        if ((!mProcessEntries && !mBookMsgFields.mBookType ==1) || mBookMsgFields.mBookType==2)
        {
            // No entry delta; just this price level delta.
            addDelta (NULL, level, plSizeChange, plAction,
                        MamdaOrderBookEntry::MAMDA_BOOK_ACTION_UNKNOWN);
                        
            if (mFullBook->getGenerateDeltaMsgs())
            {
                mFullBook->addDelta(NULL, level, plSizeChange, plAction,
                                 MamdaOrderBookEntry::MAMDA_BOOK_ACTION_UNKNOWN);
            }
        }

        //V5 Entry Book
        if (mBookMsgFields.mBookType ==1)
        {
            if(!mHaveEntries)
            {
                //Remove a plAction Add/Update level which got inserted by a delete entry delta
                mFullBook->detach (level);
            }
        }
    }

    void MamdaOrderBookListener::MamdaOrderBookListenerImpl::processEntry (
        MamdaOrderBookPriceLevel*           level,
        MamdaOrderBookPriceLevel::Action    plAction,
        MamdaOrderBookEntry::Action         entryAction,
        MamdaOrderBookTypes::Reason         reason,
        const char*                         id,
        mama_quantity_t                     size,
        const MamaDateTime&                 entryTime,
        mama_u16_t                          status)
    { 
        // We have to connect this entry to the price level, even if this
        // is a deletion and even if this is a deletion of an entry that
        // did not even previously exist.  The delta levels and entries
        // always need to have been connected to the book because
        // otherwise calls like MamdaOrderBookEntry::getOrderBook() will
        // fail.
        bool newEntry = false;
        MamdaOrderBookEntry*  entry = NULL;
        if (id == NULL || strcmp(id, "") == 0)
        {
            mama_log (MAMA_LOG_LEVEL_FINE,
                " Invalid value for Entry Id - unable to process entry");
            return;
        }
        if (mEntryManager)
        {
            if (mUniqueEntryIds)
            {
                entry = mEntryManager->findEntry (id, false);
                if (entryAction == MamdaOrderBookEntry::MAMDA_BOOK_ACTION_DELETE)
                {
                    if (entry)
                    {
                        mEntryManager->removeEntry (id);
                        entry->setManager (NULL);
                    }
                    else
                    {
                        //V5 Entry Book
                        if (1 == mBookMsgFields.mBookType)
                        {
                            //size will be unchanged
                            if (!mBookMsgFields.mHasPlSize)
                            {
                                mBookMsgFields.mPlSize = level->getSize ();
                            }
                        }
                        mama_log (MAMA_LOG_LEVEL_FINE,
                            " received delete for unknown entry (%s) for price level (%g)",
                            id, level->getPrice());
                        return;
                    }
                }
                if (!entry)
                {
                    entry = new MamdaOrderBookEntry;
                    entry->setId (id);
                    entry->setAction (MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD);
                    entry->setSourceDerivative (mFullBook->getSourceDerivative());
                    entry->setManager (mEntryManager);
                    level->addEntry (entry);
                    mEntryManager->addEntry (entry);

                    newEntry = true;
                    if (MamdaOrderBookEntry::MAMDA_BOOK_ACTION_UPDATE == entryAction)
                    {
                        //Fix the entry action
                        entryAction = MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD;
                    }
                }
                if (entryAction == MamdaOrderBookEntry::MAMDA_BOOK_ACTION_DELETE)
                {
                    mEntryManager->removeEntry (id);
                    entry->setManager (NULL); 
                }
            }
            else
            {
                size_t  lenAvail = 63;
                // uniqueId is for the entry manager
                char uniqueId[64];  /* catenate the price to the symbol */
                char* uniqueIdPtr = uniqueId;
                wmFastCopyAndShiftStr (&uniqueIdPtr, &lenAvail, id);
                wmFastPrintAndShiftF64 (&uniqueIdPtr, &lenAvail,
                                        level->getPrice(), 4); 
                uniqueIdPtr[0] = (char)level->getSide();
                uniqueIdPtr[1] = '\0';

                entry = mEntryManager->findEntry (uniqueId, false);
                if (entryAction == MamdaOrderBookEntry::MAMDA_BOOK_ACTION_DELETE)
                {
                    if (entry)
                    {
                        mEntryManager->removeEntry (uniqueId);
                        entry->setManager (NULL);
                    }
                    else
                    {
                        //V5 Entry Book
                        if (1 == mBookMsgFields.mBookType)
                        {
                            //size will be unchanged
                            if (!mBookMsgFields.mHasPlSize)
                            {
                                mBookMsgFields.mPlSize = level->getSize ();
                            }
                        }
                        mama_log (MAMA_LOG_LEVEL_FINE,
                            " received delete for unknown entry (%s) for price level (%g)",
                            id, level->getPrice());
                        return;
                    }
                }
                if (!entry)
                {   entry = new MamdaOrderBookEntry;
                    entry->setId (id);
                    entry->setUniqueId (uniqueId);
                    entry->setAction (MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD);
                    entry->setSourceDerivative (mFullBook->getSourceDerivative());
                    entry->setManager (mEntryManager);
                    level->addEntry (entry);
                    mEntryManager->addEntry (entry, uniqueId);

                    newEntry = true;
                    if (MamdaOrderBookEntry::MAMDA_BOOK_ACTION_UPDATE == entryAction)
                    {
                        //Fix the entry action
                        entryAction = MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD;
                    }
                }
                if (entryAction == MamdaOrderBookEntry::MAMDA_BOOK_ACTION_DELETE)
                {
                    mEntryManager->removeEntry (uniqueId);
                    entry->setManager (NULL);
                }
            }
        }
        else
        {
            if (entryAction == MamdaOrderBookEntry::MAMDA_BOOK_ACTION_DELETE)
            {
                entry = level->findEntry (id);
                if (!entry)
                {
                    //V5 Entry Book
                    if (1 == mBookMsgFields.mBookType)
                    {
                        //size will be unchanged
                        if (!mBookMsgFields.mHasPlSize)
                        {
                            mBookMsgFields.mPlSize = level->getSize ();
                        }
                    }
                    mama_log (MAMA_LOG_LEVEL_FINE,
                        " received delete for unknown entry (%s) for price level (%g)",
                        id, level->getPrice());
                    return;
                }
            }
            else
            {
                entry = level->findOrCreateEntry (id, newEntry);
                if ((newEntry) && (MamdaOrderBookEntry::MAMDA_BOOK_ACTION_UPDATE == entryAction))
                {
                    //Fix the entry action
                    entryAction = MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD;
                }
            }
        }

        //Disabled 18/12/09 to be more forgiving of v5 feeds which
        //fail to send wBookType identifying msg as an entry book
        if (!mBookMsgFields.mHasPlSizeChange)
        {
            if (newEntry)
            {
                //Add
                mBookMsgFields.mPlSizeChange += size;
            }
            else
            {
                //Update
                mBookMsgFields.mPlSizeChange += (size - entry->getSize ());
            }
        }

        if (!mBookMsgFields.mHasPlSize)
        {
            if (newEntry)
            {
                //Add
                mBookMsgFields.mPlSize = (level->getSize () + size);
            }
            else
            {
                //Update
                mBookMsgFields.mPlSize = (level->getSize () + size - entry->getSize ());
            }
        }

        //Apply new size
        entry->setSize (size);
        entry->setTime (entryTime);
        entry->setStatus (status);
        entry->setReason (reason);

        if (!mBookMsgFields.mHasPlTime)
        {
            if (entryTime > level->getTime())
                level->setTime (entryTime);
        }

        if (entryAction == MamdaOrderBookEntry::MAMDA_BOOK_ACTION_DELETE)
        {
            // should this be in the "else" part, above?
            level->removeEntry (entry);
            // We will delete it later

            //V5 Entry Book
            if (mBookMsgFields.mBookType ==1)
            {
                if (0 == level->getNumEntriesTotal())
                {
                    //Fix the level action for the delta - if there are no entries
                    //set the action to delete
                    plAction = MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_DELETE;
                    mBookMsgFields.mPlAction = MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_DELETE;
                    mFullBook->detach (level);
                }
            }
        }

        if (gMamaLogLevel >= MAMA_LOG_LEVEL_FINEST)
        {
            mama_log (MAMA_LOG_LEVEL_FINEST, 
                      "  found delta entry (%s) for price level (%g) "
                      "side=%c, plAct=%c, plSize=%g, entAct=%c, entSize=%g",
                      entry->getId(), level->getPrice(), level->getSide(),
                      (char)plAction, level->getSize(),
                      (char)entryAction, entry->getSize());
        }

        addDelta (entry, level, mBookMsgFields.mPlSizeChange, plAction, entryAction);

        if (mFullBook->getGenerateDeltaMsgs())
        {
            mFullBook->addDelta(entry, level, mBookMsgFields.mPlSizeChange, 
                                 plAction, entryAction);
        }
        
        mHaveEntries = true; 
    }

    void MamdaOrderBookListener::MamdaOrderBookListenerImpl::addIgnoreEntryId (
        const char*  id)
    {
        if (!mIgnoredEntries)
        {
            mIgnoredEntries = wtable_create ("ign.ents", 5);
        }
        wtable_insert (mIgnoredEntries, id, (caddr_t)1);
    }

    void MamdaOrderBookListener::MamdaOrderBookListenerImpl::removeIgnoreEntryId (
        const char*  id)
    {
        if (mIgnoredEntries)
        {
            wtable_remove (mIgnoredEntries, id);
        }
    }

    bool MamdaOrderBookListener::MamdaOrderBookListenerImpl::ignoreEntryId (
        const char*  id) const
    {
        if (!mIgnoredEntries)
            return false;
        else
            return wtable_lookup (mIgnoredEntries, id);
    }

    void MamdaOrderBookListener::MamdaOrderBookListenerImpl::invokeClearHandlers (
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

    void MamdaOrderBookListener::MamdaOrderBookListenerImpl::invokeRecapHandlers (
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

    void MamdaOrderBookListener::MamdaOrderBookListenerImpl::invokeDeltaHandlers (
        MamdaSubscription*  subscription,
        const MamaMsg*      msg)
    {
        if (mCurrentDeltaCount == 0)
        {
            mama_log (MAMA_LOG_LEVEL_NORMAL,
                        "MamdaOrderBookListener: got message with no delta: %s",
                        msg->toString());
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
            else if (mCurrentDeltaCount > 1)
            {
                MamdaOrderBookComplexDelta::setOrderBook(mFullBook);
                handler->onBookComplexDelta (subscription, mListener, msg,
                                             *this, *mFullBook);
                MamdaOrderBookComplexDelta::clear();                             
            }
        }

        if (!mFullBook->getGenerateDeltaMsgs())
        {
            mFullBook->cleanupDetached();
        }
        mCurrentDeltaCount =0;
    }

    void MamdaOrderBookListener::MamdaOrderBookListenerImpl::invokeComplexDeltaHandlers (
        MamdaSubscription*  subscription,
        const MamaMsg*      msg)
    {
        MamdaOrderBookComplexDelta::setOrderBook(mFullBook);
        deque<MamdaOrderBookHandler*>::iterator end = mHandlers.end();
        deque<MamdaOrderBookHandler*>::iterator i   = mHandlers.begin();
        for (; i != end; ++i)
        {
            MamdaOrderBookHandler* handler = *i;
            handler->onBookComplexDelta (subscription, mListener, msg,
                                         *this, *mFullBook);
        }
    }

    void MamdaOrderBookListener::MamdaOrderBookListenerImpl::invokeGapHandlers (
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

    void MamdaOrderBookListener::MamdaOrderBookListenerImpl::setQuality (
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
                clear();
                invokeClearHandlers (sub, NULL);
            }
        case MAMA_QUALITY_DUPLICATE: break;
        }
    }

    const char*
    MamdaOrderBookListener::MamdaOrderBookListenerImpl::getSymbol() const
    {
        if (mFullBook)
            return mFullBook->getSymbol();
        else
            return "unknown-symbol";
    }

    const char*
    MamdaOrderBookListener::MamdaOrderBookListenerImpl::getPartId() const
    {
        return "";
    }

    const MamaDateTime&
    MamdaOrderBookListener::MamdaOrderBookListenerImpl::getSrcTime() const
    {
        return mBookMsgFields.mSrcTime;
    }

    const MamaDateTime&
    MamdaOrderBookListener::MamdaOrderBookListenerImpl::getActivityTime() const
    {
        return mBookMsgFields.mActivityTime;
    }

    const MamaDateTime&
    MamdaOrderBookListener::MamdaOrderBookListenerImpl::getLineTime() const
    {
        return mBookMsgFields.mLineTime;
    }

    const MamaDateTime&
    MamdaOrderBookListener::MamdaOrderBookListenerImpl::getSendTime() const
    {
        return mBookMsgFields.mSendTime;
    }

    const MamaMsgQual&
    MamdaOrderBookListener::MamdaOrderBookListenerImpl::getMsgQual() const
    {
        return mBookMsgFields.mMsgQual;
    }

    mama_seqnum_t
    MamdaOrderBookListener::MamdaOrderBookListenerImpl::getEventSeqNum() const
    {
        return mEventSeqNum;
    }

    const MamaDateTime&
    MamdaOrderBookListener::MamdaOrderBookListenerImpl::getEventTime() const
    {
        return mBookMsgFields.mBookTime;
    }

    /*  FieldState Accessors    */

    MamdaFieldState 
    MamdaOrderBookListener::MamdaOrderBookListenerImpl::getSymbolFieldState() const
    {
        return mBookMsgFields.mSymbolFieldState;
    }

    MamdaFieldState
    MamdaOrderBookListener::MamdaOrderBookListenerImpl::getPartIdFieldState() const
    {
        return mBookMsgFields.mPartIdFieldState;
    }

    MamdaFieldState  
    MamdaOrderBookListener::MamdaOrderBookListenerImpl::getEventSeqNumFieldState() const
    {
        return mBookMsgFields.mEventSeqNumFieldState;
    }

    MamdaFieldState 
    MamdaOrderBookListener::MamdaOrderBookListenerImpl::getEventTimeFieldState() const
    {
        return mBookMsgFields.mEventTimeFieldState;
    }

    MamdaFieldState 
    MamdaOrderBookListener::MamdaOrderBookListenerImpl::getSrcTimeFieldState() const
    {
        return mBookMsgFields.mSrcTimeFieldState;
    }

    MamdaFieldState 
    MamdaOrderBookListener::MamdaOrderBookListenerImpl::getActivityTimeFieldState() const
    {
        return mBookMsgFields.mActivityTimeFieldState;
    }

    MamdaFieldState 
    MamdaOrderBookListener::MamdaOrderBookListenerImpl::getLineTimeFieldState() const
    {
        return mBookMsgFields.mLineTimeFieldState;
    }

    MamdaFieldState 
    MamdaOrderBookListener::MamdaOrderBookListenerImpl::getSendTimeFieldState() const
    {
        return mBookMsgFields.mSendTimeFieldState;
    }

    MamdaFieldState 
    MamdaOrderBookListener::MamdaOrderBookListenerImpl::getMsgQualFieldState() const
    {
        return mBookMsgFields.mMsgQualFieldState;
    }


    /*  End FieldAccessors  */

    const MamdaOrderBook*
    MamdaOrderBookListener::MamdaOrderBookListenerImpl::getOrderBook () const
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

    MamdaOrderBook*
    MamdaOrderBookListener::MamdaOrderBookListenerImpl::getOrderBook ()
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

    mama_seqnum_t
    MamdaOrderBookListener::MamdaOrderBookListenerImpl::getBeginGapSeqNum() const
    {
        return mGapBegin;
    }

    mama_seqnum_t
    MamdaOrderBookListener::MamdaOrderBookListenerImpl::getEndGapSeqNum() const
    {
        return mGapEnd;
    }

    void MamdaOrderBookListener::MamdaOrderBookListenerImpl::setProcessMarketOrders (
        bool  process)
    {
        mProcessMarketOrders = process;
        if (!mSimpleMarketOrderDelta)
        {
            mSimpleMarketOrderDelta = new MamdaOrderBookConcreteSimpleDelta();
        }
        if (!mComplexDeltaMarketOrderDelta)
        {
            mComplexDeltaMarketOrderDelta = new MamdaOrderBookConcreteComplexDelta();    
        }
    }

    bool MamdaOrderBookListener::MamdaOrderBookListenerImpl::getProcessMarketOrders () const 
    {
        return mProcessMarketOrders;
    }

    void MamdaOrderBookListener::MamdaOrderBookListenerImpl::addDelta (
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
            MamdaOrderBookComplexDelta::setOrderBook (mFullBook);
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

    void MamdaOrderBookListener::MamdaOrderBookListenerImpl::addMarketOrderDelta (
        MamdaOrderBookEntry*              entry,
        MamdaOrderBookPriceLevel*         level,
        mama_quantity_t                   plDeltaSize,
        MamdaOrderBookPriceLevel::Action  plAction,
        MamdaOrderBookEntry::Action       entryAction)
    {
        ++mCurrentMarketOrderDeltaCount;

        if (mCurrentMarketOrderDeltaCount == 1)
        {
            /* This is number one, so save the "simple" delta. */
            mSimpleMarketOrderDelta->set (
                entry, level, plDeltaSize, plAction, entryAction);
        }
        else if (mCurrentMarketOrderDeltaCount == 2)
        {
            /* This is number two, so copy the saved "simple" delta to the
             * "complex" delta and add the current one. */
            mComplexDeltaMarketOrderDelta->clear();
            mComplexDeltaMarketOrderDelta->setOrderBook (mFullBook);
            mComplexDeltaMarketOrderDelta->add (*this);
            mComplexDeltaMarketOrderDelta->add (
                entry, level, plDeltaSize, plAction, entryAction);
        }
        else
        {
            /* This is number greater than two, so add the current delta. */
            mComplexDeltaMarketOrderDelta->add (
                entry, level, plDeltaSize, plAction, entryAction);
        }
    }

    /* Conflation timer */
    void MamdaOrderBookListener::MamdaOrderBookListenerImpl::onTimer (
        MamaTimer* timer)
    {
        MamdaSubscription* subscription = (MamdaSubscription*) timer->getClosure();
        if (gMamaLogLevel == MAMA_LOG_LEVEL_FINEST)
                mama_log (MAMA_LOG_LEVEL_FINEST,
                          "MamdaOrderBookListener: conflation timer fired for %s",
                          subscription? getSymbol() : "(nil)");
        acquireLock();     
        if (0 != mCurrentDeltaCount) 
        {   /* Only invoke if the complex delta actually has any deltas in it.
               Empty deltas can occur when an add and a delete for the same
               entry occur in the same interval */
            
            if ((mCurrentDeltaCount == 1) ||
                (this->MamdaOrderBookComplexDelta::getSize() != 0))
            {
                invokeDeltaHandlers (subscription, NULL);
            }              
            mFullBook->cleanupDetached();     
        }

        releaseLock();
        mConflationTimer->destroy();
        delete (mConflationTimer);
        mConflationTimer = NULL;

    }

    void MamdaOrderBookListener::clearConflatedDeltas ()
    {
        /* Just reset the counter and the deltas will cleared
        on next update */
        mImpl.mCurrentDeltaCount = 0;
    }

    void MamdaOrderBookListener::forceInvokeDeltaHandlers ()
    {
        mImpl.forceInvokeDeltaHandlers();
    }

    void MamdaOrderBookListener::MamdaOrderBookListenerImpl::forceInvokeDeltaHandlers ()
    {
        if (gMamaLogLevel == MAMA_LOG_LEVEL_FINEST)
                mama_log (MAMA_LOG_LEVEL_FINEST,
                          "MamdaOrderBookListener::forceInvokeDeltaHandlers");

        if (0 != mCurrentDeltaCount)
        {
            /* Only invoke if the complex delta actually has any deltas in it.
                Empty deltas can occur when an add and a delete for the same
                entry occur in the same interval */
            if ((mCurrentDeltaCount == 1) ||
                (this->MamdaOrderBookComplexDelta::getSize() != 0))
            {
                invokeDeltaHandlers (mSubscription, NULL /*msg*/ );
            }
            mFullBook->cleanupDetached();
            mCurrentDeltaCount = 0;
        }
    }

    void MamdaOrderBookListener::acquireReadLock ()
    {
        ACQUIRE_RLOCK(mImpl.mFullBookLock);
    }

    void MamdaOrderBookListener::releaseReadLock ()
    {
        RELEASE_RLOCK(mImpl.mFullBookLock);
    }

    void MamdaOrderBookListener::acquireWriteLock ()
    {
        ACQUIRE_WLOCK(mImpl.mFullBookLock);
    }

    void MamdaOrderBookListener::releaseWriteLock ()
    {
        RELEASE_WLOCK(mImpl.mFullBookLock);
    }

    void MamdaOrderBookListener::getBookSnapShot(MamdaOrderBook&  snapshot)
    {
        ACQUIRE_RLOCK(mImpl.mFullBookLock);
        snapshot = *(mImpl.mFullBook);
        RELEASE_RLOCK(mImpl.mFullBookLock);
    }

    /**** WARNING ***/
    /*** Currently not checking return value from mamaMsgField_* calls */

    struct MamdaOrderBookListener::MamdaOrderBookListenerImpl::FieldUpdateMsgSeqNum 
        : public MamdaOrderBookListener::MamdaOrderBookListenerImpl::BookFieldUpdate
    {
        void onUpdate (MamdaOrderBookListener::MamdaOrderBookListenerImpl&  impl,
                       const mamaMsgField&                                  field)
        {
            impl.mBookMsgFields.mMsgSeqNum = 0;
            mamaMsgField_getU32 (field, &(impl.mBookMsgFields.mMsgSeqNum));
            
        }
    };

    struct MamdaOrderBookListener::MamdaOrderBookListenerImpl::FieldUpdateMsgNum 
        : public MamdaOrderBookListener::MamdaOrderBookListenerImpl::BookFieldUpdate
    {
        void onUpdate (MamdaOrderBookListener::MamdaOrderBookListenerImpl&  impl,
                       const mamaMsgField&                                  field)
        {
            
            impl.mBookMsgFields.mMsgNum = 0;
            mamaMsgField_getI16 (field, &(impl.mBookMsgFields.mMsgNum));
        }
    };

    struct MamdaOrderBookListener::MamdaOrderBookListenerImpl::FieldUpdateMsgTotal 
        : public MamdaOrderBookListener::MamdaOrderBookListenerImpl::BookFieldUpdate
    {
        void onUpdate (MamdaOrderBookListener::MamdaOrderBookListenerImpl&  impl,
                       const mamaMsgField&                                  field)
        {
            impl.mBookMsgFields.mMsgTotal = 0;
            mamaMsgField_getI16 (field, &(impl.mBookMsgFields.mMsgTotal));
        }
    };

    struct MamdaOrderBookListener::MamdaOrderBookListenerImpl::FieldUpdateSymbol 
        : public MamdaOrderBookListener::MamdaOrderBookListenerImpl::BookFieldUpdate
    {
        void onUpdate (MamdaOrderBookListener::MamdaOrderBookListenerImpl&  impl,
                       const mamaMsgField&                                  field)
        {
            /* We only need to set this value once for a book.
             Annoyingly the stl string will return a 0 length char* instead of 
             a NULL pointer when the string has no value*/
            if (impl.mFullBook && strlen(impl.mFullBook->getSymbol())==0)
            {
                const char* result = NULL;
                mamaMsgField_getString (field, &result);
                if (result)
                {
                    impl.mFullBook->setSymbol (result);
                }
            }
        }
    };

    struct MamdaOrderBookListener::MamdaOrderBookListenerImpl::FieldUpdatePartId 
        : public MamdaOrderBookListener::MamdaOrderBookListenerImpl::BookFieldUpdate
    {
        void onUpdate (MamdaOrderBookListener::MamdaOrderBookListenerImpl&  impl,
                       const mamaMsgField&                                  field)
        {
            if (impl.mFullBook && (impl.mFullBook->hasPartId() == false))
            {
                const char* result = NULL;
                mamaMsgField_getString (field, &result);
                if (result)
                {
                    impl.mFullBook->setPartId (result);
                }
            }
        }
    };

    struct MamdaOrderBookListener::MamdaOrderBookListenerImpl::FieldUpdateSrcTime 
        : public MamdaOrderBookListener::MamdaOrderBookListenerImpl::BookFieldUpdate
    {
        void onUpdate (MamdaOrderBookListener::MamdaOrderBookListenerImpl&  impl,
                       const mamaMsgField&                                  field)
        {
            mamaMsgField_getDateTime (field, impl.mBookMsgFields.mSrcTime.getCValue());
            impl.mBookMsgFields.mSrcTimeFieldState = MODIFIED;
        }
    };

    struct MamdaOrderBookListener::MamdaOrderBookListenerImpl::FieldUpdateLineTime 
        : public MamdaOrderBookListener::MamdaOrderBookListenerImpl::BookFieldUpdate
    {
        void onUpdate (MamdaOrderBookListener::MamdaOrderBookListenerImpl&  impl,
                       const mamaMsgField&                                  field)
        {
            mamaMsgField_getDateTime (field,
                                      impl.mBookMsgFields.mLineTime.getCValue());
            impl.mBookMsgFields.mLineTimeFieldState = MODIFIED;
        }
    };

    struct MamdaOrderBookListener::MamdaOrderBookListenerImpl::FieldUpdateBookTime 
        : public MamdaOrderBookListener::MamdaOrderBookListenerImpl::BookFieldUpdate
    {
        void onUpdate (MamdaOrderBookListener::MamdaOrderBookListenerImpl&  impl,
                       const mamaMsgField&                                  field)
        {
            mamaMsgField_getDateTime (field, impl.mBookMsgFields.mBookTime.getCValue());
        }
    };

    struct MamdaOrderBookListener::MamdaOrderBookListenerImpl::FieldUpdateSendTime
        : public MamdaOrderBookListener::MamdaOrderBookListenerImpl::BookFieldUpdate
    {
        void onUpdate (MamdaOrderBookListener::MamdaOrderBookListenerImpl&  impl,
                       const mamaMsgField&                                  field)
        {
            mamaMsgField_getDateTime (field,
                                      impl.mBookMsgFields.mSendTime.getCValue());
            impl.mBookMsgFields.mSendTimeFieldState = MODIFIED;
        }
    };

    struct MamdaOrderBookListener::MamdaOrderBookListenerImpl::FieldUpdateActivityTime
        : public MamdaOrderBookListener::MamdaOrderBookListenerImpl::BookFieldUpdate
    {
        void onUpdate (MamdaOrderBookListener::MamdaOrderBookListenerImpl&  impl,
                       const mamaMsgField&                                  field)
        {
            mamaMsgField_getDateTime (field,
                                      impl.mBookMsgFields.mActivityTime.getCValue());
            impl.mBookMsgFields.mActivityTimeFieldState = MODIFIED;
        }
    };

    struct MamdaOrderBookListener::MamdaOrderBookListenerImpl::FieldUpdateNumLevels 
        : public MamdaOrderBookListener::MamdaOrderBookListenerImpl::BookFieldUpdate
    {
        void onUpdate (MamdaOrderBookListener::MamdaOrderBookListenerImpl&  impl,
                       const mamaMsgField&                                  field)
        {
            mama_u32_t val = 0;
            mamaMsgField_getU32 (field, &val);
            impl.mBookMsgFields.mNumLevels = val;
        }
    };

    struct MamdaOrderBookListener::MamdaOrderBookListenerImpl::FieldUpdatePriceLevels 
        : public MamdaOrderBookListener::MamdaOrderBookListenerImpl::BookFieldUpdate
    {
        void onUpdate (MamdaOrderBookListener::MamdaOrderBookListenerImpl&  impl,
                       const mamaMsgField&                                  field)
        {
            /*This field being null indicates that no vector of levels was found*/
            impl.mBookMsgFields.mPriceLevelVector = NULL;
            mamaMsgField_getVectorMsg (
                field, 
                &impl.mBookMsgFields.mPriceLevelVector,
                &impl.mBookMsgFields.mNumLevels);
        }
    };

    struct MamdaOrderBookListener::MamdaOrderBookListenerImpl::FieldUpdateBidMarketOrders 
        : public MamdaOrderBookListener::MamdaOrderBookListenerImpl::BookFieldUpdate
    {
        void onUpdate (MamdaOrderBookListener::MamdaOrderBookListenerImpl&  impl,
                       const mamaMsgField&                                  field)
        {
             /* Don't extract if we don't care about market orders */
            if (impl.mProcessMarketOrders)
            {
                /*This field being null indicates that market orders weren't found */
                impl.mBookMsgFields.mBidMarketOrders = NULL;
                const mamaMsg* msgv;
                mama_size_t   size;
                mamaMsgField_getVectorMsg (field, &msgv, &size);
                /* check that we only have 1 msg */
                mamaMsg_copy (msgv[0], &impl.mBookMsgFields.mBidMarketOrders);
                impl.mBookMsgFields.mHasMarketOrders = true;
            }
        }
    };

    struct MamdaOrderBookListener::MamdaOrderBookListenerImpl::FieldUpdateAskMarketOrders
        : public MamdaOrderBookListener::MamdaOrderBookListenerImpl::BookFieldUpdate
    {
        void onUpdate (MamdaOrderBookListener::MamdaOrderBookListenerImpl&  impl,
                       const mamaMsgField&                                  field)
        {
            if (impl.mProcessMarketOrders)
            {
                /*This field being null indicates that market orders weren't found */
                impl.mBookMsgFields.mAskMarketOrders = NULL;
                const mamaMsg* msgv;
                mama_size_t   size;
                mamaMsgField_getVectorMsg (field, &msgv, &size);
                /* check that we only have 1 msg */
                mamaMsg_copy (msgv[0], &impl.mBookMsgFields.mAskMarketOrders);
                impl.mBookMsgFields.mHasMarketOrders = true;
            }
        }
    };

    struct MamdaOrderBookListener::MamdaOrderBookListenerImpl::FieldUpdatePlPrice
        : public MamdaOrderBookListener::MamdaOrderBookListenerImpl::BookFieldUpdate
    {
        void onUpdate (MamdaOrderBookListener::MamdaOrderBookListenerImpl&  impl,
                       const mamaMsgField&                                  field)
        {
            impl.mBookMsgFields.mPlPrice.clear();
            mamaMsgField_getPrice (field, impl.mBookMsgFields.mPlPrice.getCValue());
            //round to 8 d.p.
            double price = floor(100000000*(impl.mBookMsgFields.mPlPrice.getValue()) + 0.5) / 100000000;
            impl.mBookMsgFields.mPlPrice.setValue(price);
        }
    };

    struct MamdaOrderBookListener::MamdaOrderBookListenerImpl::FieldUpdatePlSide
        : public MamdaOrderBookListener::MamdaOrderBookListenerImpl::BookFieldUpdate
    {
        void onUpdate (MamdaOrderBookListener::MamdaOrderBookListenerImpl&  impl,
                       const mamaMsgField&                                  field)
        {
            //There is a bug in 2.14 FHs which can cause character fields to be sent as strings
            //FH property CharFieldAsStringField in 2.16-> can enable this behaviour
            //Adding support for this in MAMDA for client apps coded to expect this behaviour
            mamaFieldType type;
            mamaMsgField_getType (field, &type);

            if (type == MAMA_FIELD_TYPE_STRING)
            {
                const char* resultString = NULL;
                mamaMsgField_getString (field, &resultString);
                impl.mBookMsgFields.mPlSide = 
                    (MamdaOrderBookPriceLevel::Side) resultString [0];
            }
            else
            {        
                char result = ' ';
                mamaMsgField_getChar (field, &(result));
                impl.mBookMsgFields.mPlSide =
                    (MamdaOrderBookPriceLevel::Side) result;
            }
        }
    };

    struct MamdaOrderBookListener::MamdaOrderBookListenerImpl::FieldUpdatePlAction
        : public MamdaOrderBookListener::MamdaOrderBookListenerImpl::BookFieldUpdate
    {
        void onUpdate (MamdaOrderBookListener::MamdaOrderBookListenerImpl&  impl,
                       const mamaMsgField&                                  field)
        {
            //There is a bug in 2.14 FHs which can cause character fields to be sent as strings
            //FH property CharFieldAsStringField in 2.16-> can enable this behaviour
            //Adding support for this in MAMDA for client apps coded to expect this behaviour
            mamaFieldType type;
            mamaMsgField_getType (field, &type);
            
            if (type == MAMA_FIELD_TYPE_STRING)
            {
                const char* resultString = NULL;
                mamaMsgField_getString (field, &resultString);
                impl.mBookMsgFields.mPlAction =
                    (MamdaOrderBookPriceLevel::Action) resultString [0];
            }
            else
            {   
                char result = ' ';
                mamaMsgField_getChar (field, &(result));
                impl.mBookMsgFields.mPlAction = 
                    (MamdaOrderBookPriceLevel::Action) result;
            }
            impl.mBookMsgFields.mHasPlAction = true;
        }
    };

    struct MamdaOrderBookListener::MamdaOrderBookListenerImpl::FieldUpdatePlSize
        : public MamdaOrderBookListener::MamdaOrderBookListenerImpl::BookFieldUpdate
    {
        void onUpdate (MamdaOrderBookListener::MamdaOrderBookListenerImpl&  impl,
                       const mamaMsgField&                                  field)
        {
            impl.mBookMsgFields.mPlSize = 0.0;
            mamaMsgField_getF64 (field, &(impl.mBookMsgFields.mPlSize));
            impl.mBookMsgFields.mHasPlSize = true;
        }
    };

    struct MamdaOrderBookListener::MamdaOrderBookListenerImpl::FieldUpdatePlSizeChange
        : public MamdaOrderBookListener::MamdaOrderBookListenerImpl::BookFieldUpdate
    {
        void onUpdate (MamdaOrderBookListener::MamdaOrderBookListenerImpl&  impl,
                       const mamaMsgField&                                  field)
        {
            impl.mBookMsgFields.mPlSizeChange = 0.0;
            mamaMsgField_getF64 (field, &(impl.mBookMsgFields.mPlSizeChange));
            impl.mBookMsgFields.mHasPlSizeChange = true;
        }
    };

    struct MamdaOrderBookListener::MamdaOrderBookListenerImpl::FieldUpdatePlTime 
        : public MamdaOrderBookListener::MamdaOrderBookListenerImpl::BookFieldUpdate
    {
        void onUpdate (MamdaOrderBookListener::MamdaOrderBookListenerImpl&  impl,
                       const mamaMsgField&                                  field)
        {
            mamaMsgField_getDateTime (field, impl.mBookMsgFields.mPlTime.getCValue());
            impl.mBookMsgFields.mHasPlTime = true;
        }
    };

    struct MamdaOrderBookListener::MamdaOrderBookListenerImpl::FieldUpdatePlNumEntries
        : public MamdaOrderBookListener::MamdaOrderBookListenerImpl::BookFieldUpdate
    {
        void onUpdate (MamdaOrderBookListener::MamdaOrderBookListenerImpl&  impl,
                       const mamaMsgField&                                  field)
        {
            impl.mBookMsgFields.mPlNumEntries = 0.0f;
            mamaMsgField_getF32 (field, &(impl.mBookMsgFields.mPlNumEntries));
        }
    };

    struct MamdaOrderBookListener::MamdaOrderBookListenerImpl::FieldUpdatePlNumAttach
        : public MamdaOrderBookListener::MamdaOrderBookListenerImpl::BookFieldUpdate
    {
        void onUpdate (MamdaOrderBookListener::MamdaOrderBookListenerImpl&  impl,
                       const mamaMsgField&                                  field)
        {
            if (impl.mProcessEntries || impl.mBookMsgFields.mBookType == 1)
            {
            impl.mBookMsgFields.mPlNumAttach = 0;
            mama_u32_t val = 0;
            mamaMsgField_getU32 (field, &val);
            impl.mBookMsgFields.mPlNumAttach = val;
            }
        }
    };

    struct MamdaOrderBookListener::MamdaOrderBookListenerImpl::FieldUpdateEntries
        : public MamdaOrderBookListener::MamdaOrderBookListenerImpl::BookFieldUpdate
    {
        void onUpdate (MamdaOrderBookListener::MamdaOrderBookListenerImpl&  impl,
                       const mamaMsgField&                                  field)
        {
            if (impl.mProcessEntries || impl.mBookMsgFields.mBookType == 1)
            {
                impl.mBookMsgFields.mEntryVector = NULL;
                /*This being NULL indicates that no vector of entries was found*/
                mamaMsgField_getVectorMsg (
                    field, 
                    &impl.mBookMsgFields.mEntryVector,
                    &impl.mBookMsgFields.mPlNumAttach);
            }
        }
    };

    struct MamdaOrderBookListener::MamdaOrderBookListenerImpl::FieldUpdateEntryId
        : public MamdaOrderBookListener::MamdaOrderBookListenerImpl::BookFieldUpdate
    {
        void onUpdate (MamdaOrderBookListener::MamdaOrderBookListenerImpl&  impl,
                       const mamaMsgField&                                  field)
        {
            if (impl.mProcessEntries || impl.mBookMsgFields.mBookType == 1)
            {
                impl.mBookMsgFields.mEntryId = NULL;
                mamaMsgField_getString (field, &impl.mBookMsgFields.mEntryId);
            }
        }
    };

    struct MamdaOrderBookListener::MamdaOrderBookListenerImpl::FieldUpdateEntrySize
        : public MamdaOrderBookListener::MamdaOrderBookListenerImpl::BookFieldUpdate
    {
        void onUpdate (MamdaOrderBookListener::MamdaOrderBookListenerImpl&  impl,
                       const mamaMsgField&                                  field)
        {
            if (impl.mProcessEntries || impl.mBookMsgFields.mBookType == 1)
            {
                impl.mBookMsgFields.mEntrySize = 0.0;
                mamaMsgField_getF64 (field, &(impl.mBookMsgFields.mEntrySize));
            }
        }
    };

    struct MamdaOrderBookListener::MamdaOrderBookListenerImpl::FieldUpdateEntryTime
        : public MamdaOrderBookListener::MamdaOrderBookListenerImpl::BookFieldUpdate
    {
        void onUpdate (MamdaOrderBookListener::MamdaOrderBookListenerImpl&  impl,
                       const mamaMsgField&                                  field)
        {
            if (impl.mProcessEntries || impl.mBookMsgFields.mBookType == 1)
                mamaMsgField_getDateTime (field, 
                                      impl.mBookMsgFields.mEntryTime.getCValue());
        }
    };

    struct MamdaOrderBookListener::MamdaOrderBookListenerImpl::FieldUpdateEntryAction
        : public MamdaOrderBookListener::MamdaOrderBookListenerImpl::BookFieldUpdate
    {
        void onUpdate (MamdaOrderBookListener::MamdaOrderBookListenerImpl&  impl,
                       const mamaMsgField&                                  field)
        {
            if (impl.mProcessEntries || impl.mBookMsgFields.mBookType == 1)
            {
                // There is a bug in 2.14 FHs which can cause character fields to be 
                // sent as strings. FH property CharFieldAsStringField in 2.16-> can 
                // enable this behaviour. Adding support for this in MAMDA for client 
                // apps coded to expect this behaviour
                mamaFieldType type;
                mamaMsgField_getType (field, &type);
        
                if (type == MAMA_FIELD_TYPE_STRING)
                {
                    const char* resultString = NULL;
                    mamaMsgField_getString (field, &resultString);
                    impl.mBookMsgFields.mEntryAction = 
                        (MamdaOrderBookEntry::Action) resultString [0];
                }
                else
                {
                    char result = ' ';
                    mamaMsgField_getChar (field, &(result));
                    impl.mBookMsgFields.mEntryAction =
                        (MamdaOrderBookEntry::Action) result;
                }
            }
        }
    };

    struct MamdaOrderBookListener::MamdaOrderBookListenerImpl::FieldUpdateEntryReason
        : public MamdaOrderBookListener::MamdaOrderBookListenerImpl::BookFieldUpdate
    {
        void onUpdate (MamdaOrderBookListener::MamdaOrderBookListenerImpl&  impl,
                       const mamaMsgField&                                  field)
        {
            if (impl.mProcessEntries || impl.mBookMsgFields.mBookType == 1)
            {
                //There is a bug in 2.14 FHs which can cause character fields to
                // be sent as strings FH property CharFieldAsStringField in 2.16-> 
                // can enable this behaviour. Adding support for this in MAMDA for 
                // client apps coded to expect this behaviour
                mamaFieldType type;
                mamaMsgField_getType (field, &type);
        
                if (type == MAMA_FIELD_TYPE_STRING)
                {
                    const char* resultString = NULL;
                    mamaMsgField_getString (field, &resultString);
                    impl.mBookMsgFields.mEntryReason = 
                        (MamdaOrderBookTypes::Reason) resultString [0];
                }
                else
                {
                    char result = ' ';
                    mamaMsgField_getChar (field, &(result));
                    impl.mBookMsgFields.mEntryReason =
                        (MamdaOrderBookTypes::Reason) result;
                }
            }
        }
    };

    struct MamdaOrderBookListener::MamdaOrderBookListenerImpl::FieldUpdateEntryStatus
        : public MamdaOrderBookListener::MamdaOrderBookListenerImpl::BookFieldUpdate
    {
        void onUpdate (MamdaOrderBookListener::MamdaOrderBookListenerImpl&  impl,
                       const mamaMsgField&                                  field)
        {
            mama_u16_t result = 0;
            mamaMsgField_getU16 (field, &(result));
            impl.mBookMsgFields.mEntryStatus = result;
        }
    };

    struct MamdaOrderBookListener::MamdaOrderBookListenerImpl::FieldUpdateEntry
        : public MamdaOrderBookListener::MamdaOrderBookListenerImpl::BookFieldUpdate
    {
        void onUpdate (MamdaOrderBookListener::MamdaOrderBookListenerImpl&  impl,
                       const mamaMsgField&                                  field)
        {
            mamaMsg msg = NULL;
            mamaMsg copy = NULL;
            mamaMsgField_getMsg (field, &msg);
            mamaMsg_copy (msg, &copy);
            impl.mBookMsgFields.mEntries.push_back(copy);
        }
    };

    struct MamdaOrderBookListener::MamdaOrderBookListenerImpl::FieldUpdateLevel
        : public MamdaOrderBookListener::MamdaOrderBookListenerImpl::BookFieldUpdate
    {
        void onUpdate (MamdaOrderBookListener::MamdaOrderBookListenerImpl&  impl,
                       const mamaMsgField&                                  field)
        {
            mamaMsg msg = NULL;
            mamaMsg copy = NULL;
            mamaMsgField_getMsg (field, &msg);
            mamaMsg_copy (msg, &copy);
            impl.mBookMsgFields.mPriceLevels.push_back(copy);
        }
    };

    struct MamdaOrderBookListener::MamdaOrderBookListenerImpl::FieldUpdateBookProperties
        : public MamdaOrderBookListener::MamdaOrderBookListenerImpl::BookFieldUpdate
    {
        void onUpdate (MamdaOrderBookListener::MamdaOrderBookListenerImpl&  impl,
                       const mamaMsgField&                                  field)
        {
            mamaMsgField_getVectorU16 (
                field,
                (const mama_u16_t**)(&impl.mBookMsgFields.mBookPropertyFids),
                &impl.mBookMsgFields.mBookNumProps);
                impl.mBookMsgFields.mBookPropertyFidsChanged = true;
        }
    };

    struct MamdaOrderBookListener::MamdaOrderBookListenerImpl::FieldUpdatePlProperties
        : public MamdaOrderBookListener::MamdaOrderBookListenerImpl::BookFieldUpdate
    {
        void onUpdate (MamdaOrderBookListener::MamdaOrderBookListenerImpl&  impl,
                       const mamaMsgField&                                  field)
        {
            mamaMsgField_getVectorU16 (
                field,
                (const mama_u16_t**)(&impl.mBookMsgFields.mPlPropertyFids),
                &impl.mBookMsgFields.mPlNumProps);
                impl.mBookMsgFields.mPlPropertyFidsChanged = true;

        }
    };

    struct MamdaOrderBookListener::MamdaOrderBookListenerImpl::FieldUpdateEntryProperties
        : public MamdaOrderBookListener::MamdaOrderBookListenerImpl::BookFieldUpdate
    {
        void onUpdate (MamdaOrderBookListener::MamdaOrderBookListenerImpl&  impl,
                       const mamaMsgField&                                  field)
        {
            mamaMsgField_getVectorU16 (
                field,
                (const mama_u16_t**)(&impl.mBookMsgFields.mEntryPropertyFids),
                &impl.mBookMsgFields.mEntryNumProps);
                impl.mBookMsgFields.mEntryPropertyFidsChanged = true;
        }
    };

    struct MamdaOrderBookListener::MamdaOrderBookListenerImpl::FieldUpdateBookPropMsgType
        : public MamdaOrderBookListener::MamdaOrderBookListenerImpl::BookFieldUpdate
    {
        void onUpdate (MamdaOrderBookListener::MamdaOrderBookListenerImpl&  impl,
                       const mamaMsgField&                                  field)
        {
            //There is a bug in 2.14 FHs which can cause character fields to be sent as strings
            //FH property CharFieldAsStringField in 2.16-> can enable this behaviour
            //Adding support for this in MAMDA for client apps coded to expect this behaviour
            mamaFieldType type;
            mamaMsgField_getType (field, &type);

            if (type == MAMA_FIELD_TYPE_STRING)
            {
                const char* resultString = NULL;
                mamaMsgField_getString (field, &resultString);
                impl.mBookMsgFields.mBookPropMsgType = (MamdaOrderBookTypes::PropMsgType) resultString [0];
            }
            else
            {
                mamaMsgField_getChar (field,
                                      (char*)(&impl.mBookMsgFields.mBookPropMsgType));
            }
        }
    };

    struct MamdaOrderBookListener::MamdaOrderBookListenerImpl::FieldUpdatePlPropMsgType
        : public MamdaOrderBookListener::MamdaOrderBookListenerImpl::BookFieldUpdate
    {
        void onUpdate (MamdaOrderBookListener::MamdaOrderBookListenerImpl&  impl,
                       const mamaMsgField&                                  field)
        {
            //There is a bug in 2.14 FHs which can cause character fields to be sent as strings
            //FH property CharFieldAsStringField in 2.16-> can enable this behaviour
            //Adding support for this in MAMDA for client apps coded to expect this behaviour
            mamaFieldType type;
            mamaMsgField_getType (field, &type);

            if (type == MAMA_FIELD_TYPE_STRING)
            {
                const char* resultString = NULL;
                mamaMsgField_getString (field, &resultString);
                impl.mBookMsgFields.mPlPropMsgType = (MamdaOrderBookTypes::PropMsgType) resultString [0];
            }
            else
            { 
                mamaMsgField_getChar (field,
                                      (char*)(&impl.mBookMsgFields.mPlPropMsgType));
            }
        }
    };

    struct MamdaOrderBookListener::MamdaOrderBookListenerImpl::FieldUpdateEntryPropMsgType
        : public MamdaOrderBookListener::MamdaOrderBookListenerImpl::BookFieldUpdate
    {
        void onUpdate (MamdaOrderBookListener::MamdaOrderBookListenerImpl&  impl,
                       const mamaMsgField&                                  field)
        {
            //There is a bug in 2.14 FHs which can cause character fields to be sent as strings
            //FH property CharFieldAsStringField in 2.16-> can enable this behaviour
            //Adding support for this in MAMDA for client apps coded to expect this behaviour
            mamaFieldType type;
            mamaMsgField_getType (field, &type);

            if (type == MAMA_FIELD_TYPE_STRING)
            {
                const char* resultString = NULL;
                mamaMsgField_getString (field, &resultString);
                impl.mBookMsgFields.mEntryPropMsgType = (MamdaOrderBookTypes::PropMsgType) resultString [0];
            }
            else
            {
                mamaMsgField_getChar (field,
                                      (char*)(&impl.mBookMsgFields.mEntryPropMsgType));
            }
        }
    };

    struct MamdaOrderBookListener::MamdaOrderBookListenerImpl::FieldUpdateSenderId
        : public MamdaOrderBookListener::MamdaOrderBookListenerImpl::BookFieldUpdate
    {
        void onUpdate (MamdaOrderBookListener::MamdaOrderBookListenerImpl&  impl,
                       const mamaMsgField&                                  field)
        {
            impl.mBookMsgFields.mSenderId = 0;
            mamaMsgField_getU64 (field, &(impl.mBookMsgFields.mSenderId));
            
        }
    };

    MamdaOrderBookListener::MamdaOrderBookListenerImpl::BookFieldUpdate**
      MamdaOrderBookListener::MamdaOrderBookListenerImpl::mFieldUpdaters = NULL;
    volatile uint16_t
      MamdaOrderBookListener::MamdaOrderBookListenerImpl::mFieldUpdatersSize = 0;

    wthread_static_mutex_t MamdaOrderBookListener::MamdaOrderBookListenerImpl::
        mOrderBookFieldUpdaterLockMutex = WSTATIC_MUTEX_INITIALIZER;

    bool MamdaOrderBookListener::MamdaOrderBookListenerImpl::mUpdatersComplete = false;


    void MamdaOrderBookListener::MamdaOrderBookListenerImpl::initFieldUpdaters ()
    {
        int i;
        if (!mFieldUpdaters)
        {
            mFieldUpdaters = 
                new BookFieldUpdate*[MamdaOrderBookFields::getMaxFid()+1];
            mFieldUpdatersSize = MamdaOrderBookFields::getMaxFid();
            for (uint32_t i = 0; i <= mFieldUpdatersSize; ++i)
            {
                mFieldUpdaters[i] = NULL;
            }
        }

        initFieldUpdater(MamdaCommonFields::MSG_SEQ_NUM,
                         new MamdaOrderBookListener::
                         MamdaOrderBookListenerImpl::FieldUpdateMsgSeqNum);
        initFieldUpdater(MamdaCommonFields::MSG_NUM,
                         new MamdaOrderBookListener::
                         MamdaOrderBookListenerImpl::FieldUpdateMsgNum);
        initFieldUpdater(MamdaCommonFields::MSG_TOTAL,
                         new MamdaOrderBookListener::
                         MamdaOrderBookListenerImpl::FieldUpdateMsgTotal);
        initFieldUpdater(MamdaCommonFields::SYMBOL,
                         new MamdaOrderBookListener::
                         MamdaOrderBookListenerImpl::FieldUpdateSymbol);
        initFieldUpdater(MamdaCommonFields::PART_ID,
                         new MamdaOrderBookListener::
                         MamdaOrderBookListenerImpl::FieldUpdatePartId);
        initFieldUpdater(MamdaCommonFields::SRC_TIME,
                         new MamdaOrderBookListener::
                         MamdaOrderBookListenerImpl::FieldUpdateSrcTime);
        initFieldUpdater(MamdaCommonFields::LINE_TIME,
                         new MamdaOrderBookListener::
                         MamdaOrderBookListenerImpl::FieldUpdateLineTime);
        initFieldUpdater(MamdaCommonFields::SENDER_ID,
                         new MamdaOrderBookListener::
                         MamdaOrderBookListenerImpl::FieldUpdateSenderId);
        initFieldUpdater(MamdaOrderBookFields::BOOK_TIME,
                         new MamdaOrderBookListener::
                         MamdaOrderBookListenerImpl::FieldUpdateBookTime);
        initFieldUpdater(MamdaCommonFields::SEND_TIME,
                         new MamdaOrderBookListener::
                         MamdaOrderBookListenerImpl::FieldUpdateSendTime);
        initFieldUpdater(MamdaCommonFields::ACTIVITY_TIME,
                         new MamdaOrderBookListener::
                         MamdaOrderBookListenerImpl::FieldUpdateActivityTime);
        initFieldUpdater(MamdaOrderBookFields::NUM_LEVELS,
                         new MamdaOrderBookListener::
                         MamdaOrderBookListenerImpl::FieldUpdateNumLevels);
        initFieldUpdater(MamdaOrderBookFields::PRICE_LEVELS,
                         new MamdaOrderBookListener::
                         MamdaOrderBookListenerImpl::FieldUpdatePriceLevels);
        initFieldUpdater(MamdaOrderBookFields::PL_PRICE,
                         new MamdaOrderBookListener::
                         MamdaOrderBookListenerImpl::FieldUpdatePlPrice);
        initFieldUpdater(MamdaOrderBookFields::PL_SIDE,
                         new MamdaOrderBookListener::
                         MamdaOrderBookListenerImpl::FieldUpdatePlSide);
        initFieldUpdater(MamdaOrderBookFields::PL_ACTION,
                         new MamdaOrderBookListener::
                         MamdaOrderBookListenerImpl::FieldUpdatePlAction);
        initFieldUpdater(MamdaOrderBookFields::PL_SIZE,
                         new MamdaOrderBookListener::
                         MamdaOrderBookListenerImpl::FieldUpdatePlSize);
        initFieldUpdater(MamdaOrderBookFields::PL_SIZE_CHANGE,
                         new MamdaOrderBookListener::
                         MamdaOrderBookListenerImpl::FieldUpdatePlSizeChange);
        initFieldUpdater(MamdaOrderBookFields::PL_TIME,
                         new MamdaOrderBookListener::
                         MamdaOrderBookListenerImpl::FieldUpdatePlTime);
        initFieldUpdater(MamdaOrderBookFields::PL_NUM_ENTRIES,
                         new MamdaOrderBookListener::
                         MamdaOrderBookListenerImpl::FieldUpdatePlNumEntries);
        initFieldUpdater(MamdaOrderBookFields::PL_NUM_ATTACH,
                         new MamdaOrderBookListener::
                         MamdaOrderBookListenerImpl::FieldUpdatePlNumAttach);
        initFieldUpdater(MamdaOrderBookFields::PL_ENTRIES,
                         new MamdaOrderBookListener::
                         MamdaOrderBookListenerImpl::FieldUpdateEntries);
        initFieldUpdater(MamdaOrderBookFields::ENTRY_ID,
                         new MamdaOrderBookListener::
                         MamdaOrderBookListenerImpl::FieldUpdateEntryId);
        initFieldUpdater(MamdaOrderBookFields::ENTRY_SIZE,
                         new MamdaOrderBookListener::
                         MamdaOrderBookListenerImpl::FieldUpdateEntrySize);
        initFieldUpdater(MamdaOrderBookFields::ENTRY_TIME,
                         new MamdaOrderBookListener::
                         MamdaOrderBookListenerImpl::FieldUpdateEntryTime);
        initFieldUpdater(MamdaOrderBookFields::ENTRY_ACTION,
                         new MamdaOrderBookListener::
                         MamdaOrderBookListenerImpl::FieldUpdateEntryAction);
        initFieldUpdater(MamdaOrderBookFields::ENTRY_REASON,
                         new MamdaOrderBookListener::
                         MamdaOrderBookListenerImpl::FieldUpdateEntryReason);
        initFieldUpdater(MamdaOrderBookFields::BOOK_PROPERTIES,
                         new MamdaOrderBookListener::
                         MamdaOrderBookListenerImpl::FieldUpdateBookProperties);
        initFieldUpdater(MamdaOrderBookFields::PL_PROPERTIES,
                         new MamdaOrderBookListener::
                         MamdaOrderBookListenerImpl::FieldUpdatePlProperties);
        initFieldUpdater(MamdaOrderBookFields::ENTRY_PROPERTIES,
                         new MamdaOrderBookListener::
                         MamdaOrderBookListenerImpl::FieldUpdateEntryProperties);
        initFieldUpdater(MamdaOrderBookFields::BOOK_PROP_MSG_TYPE,
                         new MamdaOrderBookListener::
                         MamdaOrderBookListenerImpl::FieldUpdateBookPropMsgType);
        initFieldUpdater(MamdaOrderBookFields::PL_PROP_MSG_TYPE,
                         new MamdaOrderBookListener::
                         MamdaOrderBookListenerImpl::FieldUpdatePlPropMsgType);
        initFieldUpdater(MamdaOrderBookFields::ENTRY_PROP_MSG_TYPE,
                         new MamdaOrderBookListener::
                         MamdaOrderBookListenerImpl::FieldUpdateEntryPropMsgType);
        initFieldUpdater(MamdaOrderBookFields::ENTRY_STATUS,
                         new MamdaOrderBookListener::
                         MamdaOrderBookListenerImpl::FieldUpdateEntryStatus);               
        initFieldUpdater(MamdaOrderBookFields::BID_MARKET_ORDERS,
                         new MamdaOrderBookListener::
                         MamdaOrderBookListenerImpl::FieldUpdateBidMarketOrders);
        initFieldUpdater(MamdaOrderBookFields::ASK_MARKET_ORDERS,
                         new MamdaOrderBookListener::
                         MamdaOrderBookListenerImpl::FieldUpdateAskMarketOrders);                 
                         
        // For books that don't support vector message fields (RV messages)
        MamdaOrderBookListenerImpl::FieldUpdateEntry* entryUpdater = 
            new MamdaOrderBookListenerImpl::FieldUpdateEntry;

        for (i = 1; i < MamdaOrderBookFields::getNumEntryFields (); ++i)
        {
            initFieldUpdater (MamdaOrderBookFields::PL_ENTRY[i],
                              entryUpdater);
        }

        MamdaOrderBookListenerImpl::FieldUpdateLevel* levelUpdater = 
            new MamdaOrderBookListenerImpl::FieldUpdateLevel;
        for (i = 1; i < MamdaOrderBookFields::getNumLevelFields (); ++i)
        {
            initFieldUpdater (MamdaOrderBookFields::PRICE_LEVEL[i],
                              levelUpdater);
        }
    }

    void MamdaOrderBookListener::MamdaOrderBookListenerImpl::initFieldUpdater (
        const MamaFieldDescriptor*  fieldDesc,
        BookFieldUpdate*            updater)
    {
        if (!fieldDesc)
        {
            return;
        }

        uint16_t fid = fieldDesc->getFid();
        if (fid <= mFieldUpdatersSize)
        {
            mFieldUpdaters[fid] = updater;
        }
    }

    void BookMsgFields::clearEntry()
    {
        mEntryId           = NULL;
        mEntrySize         = 0;
        mEntryStatus       = 0;
        mEntryAction       = MamdaOrderBookEntry::MAMDA_BOOK_ACTION_DELETE;
        mEntryReason       = MamdaOrderBookTypes::MAMDA_BOOK_REASON_UNKNOWN;
        mEntryTime.clear();
    }   
        
    void BookMsgFields::clearPriceLevel()
    {   
        mPlPrice.clear();
        mPlSize            = 0.0;
        mHasPlSize         = false;
        mPlSizeChange      = 0.0;
        mHasPlSizeChange   = false;
        mPlSide            = MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID;
        mPlAction          = MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD;
        mHasPlAction       = false;
        mPlNumEntries      = 1;
        mPlNumAttach       = 1;
        mEntryVector       = NULL;
        mPlTime.clear ();
        mHasPlTime         = false;
    }
            
    void BookMsgFields::clear()
    {
        mMsgSeqNum         = 0;
        mSenderId          = 0;
        mBookType          = 0;
        mMsgNum            = 1;
        mMsgTotal          = 1;
        mPlPrice.clear();
        mPlSize            = 0.0;
        mHasPlSize         = false;
        mPlSizeChange      = 0.0;
        mHasPlSizeChange   = false;
        mPlSide            = MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID;
        mPlAction          = MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD;
        mHasPlAction       = false;
        mPlNumEntries      = 1;
        mPlNumAttach       = 1;
        mNumLevels         = 1;
        mEntryId           = NULL;
        mEntrySize         = 0;
        mEntryAction       = MamdaOrderBookEntry::MAMDA_BOOK_ACTION_DELETE;
        mEntryReason       = MamdaOrderBookTypes::MAMDA_BOOK_REASON_UNKNOWN;
        mHasPlAction       = false;
        mEntryStatus       = 0;
        mSrcTime.clear();
        mBookTime.clear();
        mPlTime.clear ();
        mHasPlTime         = false;
        mEntryTime.clear();
        mLineTime.clear();
        /*Must be initialized to NULL. Used to determine whether the message is
         * flattened or not*/
        mPriceLevelVector  = NULL;
        mEntryVector       = NULL;

        mSymbolFieldState          = NOT_INITIALISED;
        mPartIdFieldState          = NOT_INITIALISED;
        mEventSeqNumFieldState     = NOT_INITIALISED;
        mEventTimeFieldState       = NOT_INITIALISED;
        mSrcTimeFieldState         = NOT_INITIALISED;
        mActivityTimeFieldState    = NOT_INITIALISED;
        mLineTimeFieldState        = NOT_INITIALISED;
        mSendTimeFieldState        = NOT_INITIALISED;
        mMsgQualFieldState         = NOT_INITIALISED;

                            
        

        if (mBidMarketOrders)
        {
            mamaMsg_destroy (mBidMarketOrders);
            mBidMarketOrders = NULL;
        }

        if (mAskMarketOrders)
        {
            mamaMsg_destroy (mAskMarketOrders);
            mAskMarketOrders = NULL;
        }
        mHasMarketOrders = false;

        mPlPropertyFidsChanged = false;
        mEntryPropertyFidsChanged = false;
        mBookPropertyFidsChanged = false;
        mPlPropMsgType    = MamdaOrderBookTypes::MAMDA_PROP_MSG_TYPE_UPDATE;
        mEntryPropMsgType = MamdaOrderBookTypes::MAMDA_PROP_MSG_TYPE_UPDATE;
        mBookPropMsgType  = MamdaOrderBookTypes::MAMDA_PROP_MSG_TYPE_UPDATE;

        vector<mamaMsg>::iterator i = mPriceLevels.begin();
        vector<mamaMsg>::iterator end = mPriceLevels.end();
        for (; i != end; ++i)
        {
            mamaMsg_destroy (*i);
        }
        mPriceLevels.clear ();
        clearEntries();
    }

    void BookMsgFields::clearEntries()
    {
        if (!mEntries.empty())
        {
            vector<mamaMsg>::iterator i = mEntries.begin();
            vector<mamaMsg>::iterator end = mEntries.end();
            for (; i != end; ++i)
            {
                mamaMsg_destroy (*i);
            }
            mEntries.clear ();
        }
    }

} // namespace
