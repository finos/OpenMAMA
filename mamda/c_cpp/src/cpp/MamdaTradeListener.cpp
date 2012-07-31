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

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

#include <wombat/port.h>
#include <mamda/MamdaTradeListener.h>
#include <mamda/MamdaTradeFields.h>
#include <mamda/MamdaTradeHandler.h>
#include <mamda/MamdaSubscription.h>
#include <mamda/MamdaCommonFields.h>
#include <mamda/MamdaDataException.h>
#include <mamda/MamdaFieldState.h>
#include <mamda/MamdaTradeSide.h>
#include <mama/mamacpp.h>
#include "MamdaUtils.h"
#include <string>
#include <wombat/machine.h>

using std::string;

namespace Wombat
{

    struct TradeFieldUpdate
    {
        virtual void onUpdate (
            MamdaTradeListener::MamdaTradeListenerImpl&  impl,
            const MamaMsgField&                          field) = 0;
    };

    struct TradeCache
    {
        // The following fields are used for caching the offical last
        // price and related fields.  These fields can be used by
        // applications for reference and will be passed for recaps.
        string                mSymbol;                    MamdaFieldState  mSymbolFieldState;   
        string                mPartId;                    MamdaFieldState  mPartIdFieldState;  
        MamaDateTime          mSrcTime;                   MamdaFieldState  mSrcTimeFieldState; 
        MamaDateTime          mActTime;                   MamdaFieldState  mActTimeFieldState; 
        MamaDateTime          mLineTime;                  MamdaFieldState  mLineTimeFieldState;
        MamaDateTime          mSendTime;                  MamdaFieldState  mSendTimeFieldState;
        string                mPubId;                     MamdaFieldState  mPubIdFieldState;   
        bool                  mIsIrregular;               MamdaFieldState  mIsIrregularFieldState; 
        bool                  mWasIrregular;              MamdaFieldState  mWasIrregularFieldState;
        MamaPrice             mLastPrice;                 MamdaFieldState  mLastPriceFieldState; 
        mama_quantity_t       mLastVolume;                MamdaFieldState  mLastVolumeFieldState;
        string                mLastPartId;                MamdaFieldState  mLastPartIdFieldState;
        MamaDateTime          mLastTime;                  MamdaFieldState  mLastTimeFieldState;   
        MamaDateTime          mTradeDate;                 MamdaFieldState  mTradeDateFieldState; 
        MamdaTradeSide        mTmpSide;              
	    string                mSide;                      MamdaFieldState  mSideFieldState;
        MamaPrice             mIrregPrice;                MamdaFieldState  mIrregPriceFieldState; 
        mama_quantity_t       mIrregVolume;               MamdaFieldState  mIrregVolumeFieldState; 
        string                mIrregPartId;               MamdaFieldState  mIrregPartIdFieldState; 
        MamaDateTime          mIrregTime;                 MamdaFieldState  mIrregTimeFieldState;   
        mama_quantity_t       mAccVolume;                 MamdaFieldState  mAccVolumeFieldState;   
        mama_quantity_t       mOffExAccVolume;            MamdaFieldState  mOffExAccVolumeFieldState;
        mama_quantity_t       mOnExAccVolume;             MamdaFieldState  mOnExAccVolumeFieldState; 
        MamdaTradeDirection   mTradeDirection;            MamdaFieldState  mTradeDirectionFieldState;
        MamaPrice             mNetChange;                 MamdaFieldState  mNetChangeFieldState;     
        double                mPctChange;                 MamdaFieldState  mPctChangeFieldState;     
        MamaPrice             mOpenPrice;                 MamdaFieldState  mOpenPriceFieldState;     
        MamaPrice             mHighPrice;                 MamdaFieldState  mHighPriceFieldState;     
        MamaPrice             mLowPrice;                  MamdaFieldState  mLowPriceFieldState;      
        MamaPrice             mClosePrice;                MamdaFieldState  mClosePriceFieldState;    
        MamaPrice             mPrevClosePrice;            MamdaFieldState  mPrevClosePriceFieldState;
        MamaDateTime          mPrevCloseDate;             MamdaFieldState  mPrevCloseDateFieldState; 
        MamaPrice             mAdjPrevClose;              MamdaFieldState  mAdjPrevCloseFieldState;  
        mama_u32_t            mTradeCount;                MamdaFieldState  mTradeCountFieldState;    
        mama_quantity_t       mBlockVolume;               MamdaFieldState  mBlockVolumeFieldState;   
        mama_u32_t            mBlockCount;                MamdaFieldState  mBlockCountFieldState;
        double                mVwap;                      MamdaFieldState  mVwapFieldState;      
        double                mOffExVwap;                 MamdaFieldState  mOffExVwapFieldState; 
        double                mOnExVwap;                  MamdaFieldState  mOnExVwapFieldState;  
        double                mTotalValue;                MamdaFieldState  mTotalValueFieldState;
        double                mOffExTotalValue;           MamdaFieldState  mOffExTotalValueFieldState;
        double                mOnExTotalValue;            MamdaFieldState  mOnExTotalValueFieldState; 
        double                mStdDev;                    MamdaFieldState  mStdDevFieldState;  
        mama_u64_t            mOrderId;                   MamdaFieldState  mOrderIdFieldState; 
        bool                  mLastGenericMsgWasTrade;
        bool                  mGotTradeTime; 
        bool                  mGotTradePrice;
        bool                  mGotTradeSize; 
        bool                  mGotTradeCount;
        double                mStdDevSum;                 MamdaFieldState  mStdDevSumFieldState;      
        double                mStdDevSumSquares;          MamdaFieldState  mStdDevSumSquaresFieldState;
        string                mTradeUnits;                MamdaFieldState  mTradeUnitsFieldState;  
        mama_seqnum_t         mLastSeqNum;                MamdaFieldState  mLastSeqNumFieldState;  
        mama_seqnum_t         mHighSeqNum;                MamdaFieldState  mHighSeqNumFieldState;  
        mama_seqnum_t         mLowSeqNum;                 MamdaFieldState  mLowSeqNumFieldState;   
        mama_seqnum_t         mTotalVolumeSeqNum;         MamdaFieldState  mTotalVolumeSeqNumFieldState; 
        string                mCurrencyCode;              MamdaFieldState  mCurrencyCodeFieldState; 
        string                mUniqueId;                  MamdaFieldState  mUniqueIdFieldState;    
        string                mCorrTradeId;               MamdaFieldState  mCorrTradeIdFieldState;
        string                mTradeAction;               MamdaFieldState  mTradeActionFieldState;  
        bool                  mIsSnapshot;
        MamaPrice             mSettlePrice;               MamdaFieldState  mSettlePriceFieldState;  
        MamaDateTime          mSettleDate;                MamdaFieldState  mSettleDateFieldState;   
        MamaPrice             mOffExchangeTradePrice;     MamdaFieldState  mOffExchangeTradePriceFieldState; 
        MamaPrice             mOnExchangeTradePrice;      MamdaFieldState  mOnExchangeTradePriceFieldState;  
        string                mTradeId;                   MamdaFieldState  mTradeIdFieldState;
        string                mOrigTradeId;               MamdaFieldState  mOrigTradeIdFieldState;
        bool                  mGenericFlag;               MamdaFieldState  mGenericFlagFieldState;

        // The following fields are used for caching the last reported
        // trade, which might have been out of order.  The reason for
        // caching these values is to allow a configuration that passes
        // the minimum amount of data  (unchanged fields not sent).
        mama_seqnum_t         mEventSeqNum;               MamdaFieldState  mEventSeqNumFieldState;
        MamaDateTime          mEventTime;                 MamdaFieldState  mEventTimeFieldState;  
        MamaPrice             mTradePrice;                MamdaFieldState  mTradePriceFieldState; 
        mama_quantity_t       mTradeVolume;               MamdaFieldState  mTradeVolumeFieldState;
        string                mTradePartId;               MamdaFieldState  mTradePartIdFieldState;
        string                mTradeQualStr;              MamdaFieldState  mTradeQualStrFieldState; 
        string                mTradeQualNativeStr;        MamdaFieldState  mTradeQualNativeStrFieldState;
        mama_u32_t            mSellersSaleDays;           MamdaFieldState  mSellersSaleDaysFieldState; 
        char                  mStopStockInd;              MamdaFieldState  mStopStockIndFieldState;
        mama_u32_t            mTmpTradeCount;             MamdaFieldState  mTmpTradeCountFieldState;
        MamdaTradeExecVenue   mTradeExecVenue;            MamdaFieldState  mTradeExecVenueFieldState;

        // Additional fields for cancels/error/corrections:
        bool                  mIsCancel;                  
        mama_seqnum_t         mOrigSeqNum;                MamdaFieldState  mOrigSeqNumFieldState;
        MamaPrice             mOrigPrice;                 MamdaFieldState  mOrigPriceFieldState; 
        mama_quantity_t       mOrigVolume;                MamdaFieldState  mOrigVolumeFieldState;
        string                mOrigPartId;                MamdaFieldState  mOrigPartIdFieldState;
        string                mOrigQualStr;               MamdaFieldState  mOrigQualStrFieldState;
        string                mOrigQualNativeStr;         MamdaFieldState  mOrigQualNativeStrFieldState;
        mama_u32_t            mOrigSellersSaleDays;       MamdaFieldState  mOrigSellersSaleDaysFieldState;
        char                  mOrigStopStockInd;          MamdaFieldState  mOrigStopStockIndFieldState; 
        MamaPrice             mCorrPrice;                 MamdaFieldState  mCorrPriceFieldState; 
        mama_quantity_t       mCorrVolume;                MamdaFieldState  mCorrVolumeFieldState; 
        string                mCorrPartId;                MamdaFieldState  mCorrPartIdFieldState; 
        string                mCorrQualStr;               MamdaFieldState  mCorrQualStrFieldState;
        string                mCorrQualNativeStr;         MamdaFieldState  mCorrQualNativeStrFieldState;
        mama_u32_t            mCorrSellersSaleDays;       MamdaFieldState  mCorrSellersSaleDaysFieldState;
        char                  mCorrStopStockInd;          MamdaFieldState  mCorrStopStockIndFieldState;
        MamaDateTime          mCancelTime;                MamdaFieldState  mCancelTimeFieldState;
        MamaDateTime          mCorrTime;                  MamdaFieldState  mCorrTimeFieldState;
        char                  mShortSaleCircuitBreaker;   MamdaFieldState  mShortSaleCircuitBreakerFieldState;  
        char                  mOrigShortSaleCircuitBreaker;   MamdaFieldState  mOrigShortSaleCircuitBreakerFieldState;  
        char                  mCorrShortSaleCircuitBreaker;   MamdaFieldState  mCorrShortSaleCircuitBreakerFieldState;  
        

        // Additional fields for gaps:
        mama_seqnum_t         mGapBegin;                  MamdaFieldState  mGapBeginFieldState;
        mama_seqnum_t         mGapEnd;                    MamdaFieldState  mGapEndFieldState;

        // Additional fields for closing summaries:
        bool                  mIsIndicative;
    };


    class MamdaTradeListener::MamdaTradeListenerImpl : public MamaMsgFieldIterator
    {
    public:
        MamdaTradeListenerImpl  (MamdaTradeListener&  listener);
        ~MamdaTradeListenerImpl () {}

        void clearCache                   (TradeCache& tradeCache);

        void handleTradeMessage           (MamdaSubscription*  subscription,
                                           const MamaMsg&      msg,
                                           short               msgType);

        bool evaluateMsgQual              (MamdaSubscription*  subscription,
                                           const MamaMsg&      msg);

        void handleRecap                  (MamdaSubscription*  subscription,
                                           const MamaMsg&      msg);

        void handleTrade                  (MamdaSubscription*  subscription,
                                           const MamaMsg&      msg);

        void handleCancelOrError          (MamdaSubscription*  subscription,
                                           const MamaMsg&      msg,
                                           bool                isCancel);

        void handleCorrection             (MamdaSubscription*  subscription,
                                           const MamaMsg&      msg);

        void handleClosing                (MamdaSubscription*  subscription,
                                           const MamaMsg&      msg);

        void handleUpdate                 (MamdaSubscription*  subscription,
                                           const MamaMsg&      msg);

        void invokeTransientHandler       (MamdaSubscription*  subscription,
                                           const MamaMsg&      msg);

        void onField                      (const MamaMsg&      msg,
                                           const MamaMsgField& field,
                                           void*               closure);

        void updateTradeFields            (const MamaMsg&      msg);

        void checkTradeCount              (MamdaSubscription*  subscription,
                                           const MamaMsg&      msg,
                                           bool                checkForGap);

        void assertEqual                  (MamdaTradeListenerImpl& rhs);

        void updateFieldStates            ();

        void reset (void);

        typedef struct TradeUpdateLock_
        {
           wthread_mutex_t   mTradeUpdateLockMutex;

        } TradeUpdateLock;

        // Thread locking
        TradeUpdateLock mTradeUpdateLock;

        MamdaTradeListener&   mListener;
        MamdaTradeHandler*    mHandler;

        // Message Qualifier - holds information provides in formation 
        // regarding duplicate and delayed status of message.
        MamaMsgQual    mMsgQual;          MamdaFieldState mMsgQualFieldState;

        // If message is "Transient", i.e., possibly duplicate or delayed
        // and this flag is set then mTransientCache is used to store msg
        // contents - data kept separate from regular updates and does not
        // persist during to next update.
        bool           mProcessPosDupAndOutOfSeqAsTransient;

        // If mResolvePossiblyDuplicate is set then the listen will atempt, 
        // based upon sequence number and event times, to determine whether
        // a trade marked as "possibly duplicate" is definately a duplicate,
        // in which case it is dropped.
        bool           mResolvePossiblyDuplicate;


        // The mUsePosDupAndOutOfSeqHandlers is used to determine whether or not the 
        // duplicate and out of sequence handlers should be invoked for a message
        bool           mUsePosDupAndOutOfSeqHandlers;

        // Used to record whether or not a message has been 
        // determined to be transient.
        bool           mIsTransientMsg;

        // The Quote Listener Data Caches
        TradeCache     mRegularCache;     // Regular update cache
        TradeCache*    mTransientCache;   // Transient cache 
        TradeCache&    mTradeCache;       // Current cache in use
        bool           mIgnoreUpdate;

        bool           mCheckUpdatesForTrades;

        static MamdaTradeDirection  getTradeDirection (
            const MamaMsgField&  field);

        static MamdaTradeExecVenue  getTradeExecVenue (
            const MamaMsgField&  field);

        static void initFieldUpdaters ();
        static void initFieldUpdater (
            const MamaFieldDescriptor*  fieldDesc,
            TradeFieldUpdate*           updater);

        static void updateField (
            MamdaTradeListener::MamdaTradeListenerImpl&  impl,
            const MamaMsgField&                          field);

        static TradeFieldUpdate**     mFieldUpdaters;
        static volatile mama_fid_t    mFieldUpdatersSize;
        static wthread_static_mutex_t mTradeFieldUpdaterLockMutex;
        static bool                   mUpdatersComplete;

        struct FieldUpdateLastPrice;
        struct FieldUpdateLastVolume;
        struct FieldUpdateLastPartId;
        struct FieldUpdateLastTime;
        struct FieldUpdateTradeDate;
        struct FieldUpdateTradeSide;
        struct FieldUpdateAggressorSide;
        struct FieldUpdateTradeQualifier;
        struct FieldUpdateTradeQualifierNative;
        struct FieldUpdateSellersSaleDays;
        struct FieldUpdateStopStockInd;
        struct FieldUpdateIsIrregular;
        struct FieldUpdateIrregPrice;
        struct FieldUpdateIrregVolume;
        struct FieldUpdateIrregPartId;
        struct FieldUpdateIrregTime;
        struct FieldUpdateOrderId;
        struct FieldUpdateTradeSrcTime;
        struct FieldUpdateTradeSendTime;
        struct FieldUpdateTradeActTime;
        struct FieldUpdateTradeLineTime;
        struct FieldUpdateTradePubId;
        struct FieldUpdateTradeEventSeqNum;
        struct FieldUpdateTmpTradeCount;
        struct FieldUpdateAccVolume;
        struct FieldUpdateOffExAccVolume;
        struct FieldUpdateOnExAccVolume;
        struct FieldUpdateTradeDirection;
        struct FieldUpdateOpenPrice;
        struct FieldUpdateHighPrice;
        struct FieldUpdateLowPrice;
        struct FieldUpdateNetChange;
        struct FieldUpdatePctChange;
        struct FieldUpdateBlockCount;
        struct FieldUpdateBlockVolume;
        struct FieldUpdateVwap;
        struct FieldUpdateOffExVwap;
        struct FieldUpdateOnExVwap;
        struct FieldUpdateTotalValue;
        struct FieldUpdateOffExTotalValue;
        struct FieldUpdateOnExTotalValue;
        struct FieldUpdateStdDev;
        struct FieldUpdateStdDevSum;
        struct FieldUpdateStdDevSumSquares;
        struct FieldUpdateTradeUnits;
        struct FieldUpdateLastSeqNum;
        struct FieldUpdateHighSeqNum;
        struct FieldUpdateLowSeqNum;
        struct FieldUpdateTradeSeqNum;
        struct FieldUpdateTotalVolumeSeqNum;
        struct FieldUpdateCurrencyCode;
        struct FieldUpdateClosePrice;
        struct FieldUpdatePrevClosePrice;
        struct FieldUpdatePrevCloseDate;
        struct FieldUpdateAdjPrevClose;
        struct FieldUpdateOrigSeqNum;
        struct FieldUpdateOrigPrice;
        struct FieldUpdateOrigVolume;
        struct FieldUpdateOrigPartId;
        struct FieldUpdateOrigQualifier;
        struct FieldUpdateOrigQualifierNative ;
        struct FieldUpdateOrigSaleDays;
        struct FieldUpdateOrigStopStockInd;
        struct FieldUpdateCancelTime;
        struct FieldUpdateCorrPrice;
        struct FieldUpdateCorrVolume;
        struct FieldUpdateCorrPartId;
        struct FieldUpdateCorrQualifier;
        struct FieldUpdateCorrQualifierNative;
        struct FieldUpdateCorrSaleDays;
        struct FieldUpdateCorrStopStockInd;
        struct FieldUpdateCorrTime;
        struct FieldUpdateUniqueId;
        struct FieldUpdateTradeId;
        struct FieldUpdateOrigTradeId;
        struct FieldUpdateCorrTradeId;
        struct FieldUpdateTradeAction;
        struct FieldUpdateTradeExecVenue;
        struct FieldUpdateSettlePrice;
        struct FieldUpdateSettleDate;
        struct FieldUpdateOffExchangeTradePrice;
        struct FieldUpdateOnExchangeTradePrice;
        struct FieldUpdateTradeId;
        struct FieldUpdateOrigTradeId;
        struct FieldUpdateGenericFlag;
        struct FieldUpdateShortSaleCircuitBreaker;    
        struct FieldUpdateOrigShortSaleCircuitBreaker;    
        struct FieldUpdateCorrShortSaleCircuitBreaker;    
    };


    MamdaTradeListener::MamdaTradeListener ()
        : mImpl (*new MamdaTradeListenerImpl(*this))
    {
        wthread_mutex_init (&mImpl.mTradeUpdateLock.mTradeUpdateLockMutex, NULL);
    }

    MamdaTradeListener::~MamdaTradeListener()
    {
        wthread_mutex_destroy (&mImpl.mTradeUpdateLock.mTradeUpdateLockMutex);
	    /* Do not call wthread_mutex_destroy for the FieldUpdaterLockMutex here.  
	       If we do, it will not be initialized again if another listener is created 
	       after the first is destroyed. */
        /* wthread_mutex_destroy (&mImpl.mTradeFieldUpdaterLockMutex); */
        delete &mImpl;
    }

    void MamdaTradeListener::addHandler (MamdaTradeHandler*  handler)
    {
        mImpl.mHandler = handler;
    }

    void MamdaTradeListener::processPosDupAndOutOfSeqAsTransient (bool tf)
    {
        mImpl.mProcessPosDupAndOutOfSeqAsTransient = tf;
    }

    void MamdaTradeListener::resolvePossiblyDuplicate (bool tf)
    {
        mImpl.mResolvePossiblyDuplicate = tf;
    }

    void MamdaTradeListener::usePosDupAndOutOfSeqHandlers (bool tf)
    {
        mImpl.mUsePosDupAndOutOfSeqHandlers = tf;
    }

    const char* MamdaTradeListener::getSymbol() const
    {
        return mImpl.mTradeCache.mSymbol.c_str();
    }

    const char* MamdaTradeListener::getPartId() const
    {
        return mImpl.mTradeCache.mPartId.c_str();
    }

    const MamaDateTime& MamdaTradeListener::getSrcTime() const
    {
        return mImpl.mTradeCache.mSrcTime;
    }

    const MamaDateTime& MamdaTradeListener::getActivityTime() const
    {
        return mImpl.mTradeCache.mActTime;
    }

    const MamaDateTime& MamdaTradeListener::getLineTime() const
    {
        return mImpl.mTradeCache.mLineTime;
    }

    const MamaDateTime& MamdaTradeListener::getSendTime() const
    {
        return mImpl.mTradeCache.mSendTime;
    }

    const MamaMsgQual& MamdaTradeListener::getMsgQual() const
    {
        return mImpl.mMsgQual;
    }

    const char* MamdaTradeListener::getPubId() const
    {
        return mImpl.mTradeCache.mPubId.c_str();
    }

    mama_seqnum_t MamdaTradeListener::getEventSeqNum() const
    {
        return mImpl.mTradeCache.mEventSeqNum;
    }

    const MamaDateTime& MamdaTradeListener::getEventTime() const
    {
        return mImpl.mTradeCache.mEventTime;
    }

    const MamaPrice& MamdaTradeListener::getLastPrice() const
    {
        return mImpl.mTradeCache.mLastPrice;
    }

    mama_quantity_t MamdaTradeListener::getLastVolume() const
    {
        return mImpl.mTradeCache.mLastVolume;
    }

    const char* MamdaTradeListener::getLastPartId() const
    {
        return mImpl.mTradeCache.mLastPartId.c_str();
    }

    const MamaDateTime& MamdaTradeListener::getLastTime() const
    {
        return mImpl.mTradeCache.mLastTime;
    }

    const char* MamdaTradeListener::getIrregPartId() const
    {
        return mImpl.mTradeCache.mIrregPartId.c_str();
    }

    mama_quantity_t MamdaTradeListener::getIrregVolume() const
    {
        return mImpl.mTradeCache.mIrregVolume;
    }

    const MamaPrice& MamdaTradeListener::getIrregPrice() const
    {
        return mImpl.mTradeCache.mIrregPrice;
    }

    const MamaDateTime& MamdaTradeListener::getIrregTime() const
    {
        return mImpl.mTradeCache.mIrregTime;
    }

    const MamaDateTime& MamdaTradeListener::getTradeDate() const
    {
        return mImpl.mTradeCache.mTradeDate;
    }

    const char* MamdaTradeListener::getSide() const
    {
       return mImpl.mTradeCache.mSide.c_str();
    }

    mama_u32_t MamdaTradeListener::getTradeCount() const
    {
        return mImpl.mTradeCache.mTradeCount;
    }

    mama_quantity_t MamdaTradeListener::getAccVolume() const
    {
        return mImpl.mTradeCache.mAccVolume;
    }

    mama_quantity_t MamdaTradeListener::getOffExAccVolume() const
    {
        return mImpl.mTradeCache.mOffExAccVolume;
    }

    mama_quantity_t MamdaTradeListener::getOnExAccVolume() const
    {
        return mImpl.mTradeCache.mOnExAccVolume;
    }

    const MamaPrice& MamdaTradeListener::getNetChange() const
    {
        return mImpl.mTradeCache.mNetChange;
    }

    double MamdaTradeListener::getPctChange() const
    {
        return mImpl.mTradeCache.mPctChange;
    }

    MamdaTradeDirection MamdaTradeListener::getTradeDirection() const
    {
        return mImpl.mTradeCache.mTradeDirection;
    }

    const MamaPrice& MamdaTradeListener::getOpenPrice() const
    {
        return mImpl.mTradeCache.mOpenPrice;
    }

    const MamaPrice& MamdaTradeListener::getHighPrice() const
    {
        return mImpl.mTradeCache.mHighPrice;
    }

    const MamaPrice& MamdaTradeListener::getLowPrice() const
    {
        return mImpl.mTradeCache.mLowPrice;
    }

    const MamaPrice& MamdaTradeListener::getClosePrice() const
    {
        return mImpl.mTradeCache.mClosePrice;
    }

    const MamaPrice& MamdaTradeListener::getPrevClosePrice() const
    {
        return mImpl.mTradeCache.mPrevClosePrice;
    }

    const MamaPrice& MamdaTradeListener::getAdjPrevClosePrice() const
    {
        return mImpl.mTradeCache.mAdjPrevClose;
    }

    const MamaDateTime& MamdaTradeListener::getPrevCloseDate() const
    {
        return mImpl.mTradeCache.mPrevCloseDate;
    }

    const MamaPrice& MamdaTradeListener::getTradePrice() const
    {
        return mImpl.mTradeCache.mTradePrice;
    }

    mama_u32_t MamdaTradeListener::getBlockCount() const
    {
        return mImpl.mTradeCache.mBlockCount;
    }

    mama_quantity_t MamdaTradeListener::getBlockVolume() const
    {
        return mImpl.mTradeCache.mBlockVolume;
    }

    double MamdaTradeListener::getVwap() const
    {
        return mImpl.mTradeCache.mVwap;
    }

    double MamdaTradeListener::getOffExVwap() const
    {
        return mImpl.mTradeCache.mOffExVwap;
    }

    double MamdaTradeListener::getOnExVwap() const
    {
        return mImpl.mTradeCache.mOnExVwap;
    }

    double MamdaTradeListener::getTotalValue() const
    {
        return mImpl.mTradeCache.mTotalValue;
    }

    double MamdaTradeListener::getOffExTotalValue() const
    {
        return mImpl.mTradeCache.mOffExTotalValue;
    }

    double MamdaTradeListener::getOnExTotalValue() const
    {
        return mImpl.mTradeCache.mOnExTotalValue;
    }

    double MamdaTradeListener::getStdDev() const
    {
        return mImpl.mTradeCache.mStdDev;
    }

    double MamdaTradeListener::getStdDevSum() const
    {
        return mImpl.mTradeCache.mStdDevSum;
    }

    double MamdaTradeListener::getStdDevSumSquares() const
    {
        return mImpl.mTradeCache.mStdDevSumSquares;
    }

    const char* MamdaTradeListener::getTradeUnits() const
    {
        return mImpl.mTradeCache.mTradeUnits.c_str();
    }

    mama_seqnum_t MamdaTradeListener::getLastSeqNum() const
    {
        return mImpl.mTradeCache.mLastSeqNum;
    }

    mama_seqnum_t MamdaTradeListener::getHighSeqNum() const
    {
        return mImpl.mTradeCache.mHighSeqNum;
    }

    mama_seqnum_t MamdaTradeListener::getLowSeqNum() const
    {
        return mImpl.mTradeCache.mLowSeqNum;
    }

    mama_seqnum_t MamdaTradeListener::getTotalVolumeSeqNum() const
    {
        return mImpl.mTradeCache.mTotalVolumeSeqNum;
    }

    const char* MamdaTradeListener::getCurrencyCode() const
    {
        return mImpl.mTradeCache.mCurrencyCode.c_str();
    }

    const MamaPrice& MamdaTradeListener::getSettlePrice() const
    {
        return mImpl.mTradeCache.mSettlePrice;
    }

    const MamaDateTime& MamdaTradeListener::getSettleDate() const
    {
        return mImpl.mTradeCache.mSettleDate;
    }

    mama_u64_t MamdaTradeListener::getOrderId() const
    {
        return mImpl.mTradeCache.mOrderId;
    }

    const char* MamdaTradeListener::getUniqueId() const
    {
        return mImpl.mTradeCache.mUniqueId.c_str();
    }

    const char* MamdaTradeListener::getCorrTradeId() const
    {
        return mImpl.mTradeCache.mCorrTradeId.c_str();
    }

    const char* MamdaTradeListener::getTradeAction() const
    {
        return mImpl.mTradeCache.mTradeAction.c_str();
    }

    MamdaTradeExecVenue MamdaTradeListener::getTradeExecVenue() const
    {
        return mImpl.mTradeCache.mTradeExecVenue;
    }

    mama_quantity_t MamdaTradeListener::getTradeVolume() const
    {
        return mImpl.mTradeCache.mTradeVolume;
    }

    const char* MamdaTradeListener::getTradePartId() const
    {
        if (0 != mImpl.mTradeCache.mTradePartId.length())
        {
            return mImpl.mTradeCache.mTradePartId.c_str();
        }
        else
        {
            return mImpl.mTradeCache.mPartId.c_str();
        }
    }

    const char* MamdaTradeListener::getTradeQual() const
    {
        return mImpl.mTradeCache.mTradeQualStr.c_str();
    }

    const char* MamdaTradeListener::getTradeQualNative() const
    {
        return mImpl.mTradeCache.mTradeQualNativeStr.c_str();
    }

    mama_u32_t MamdaTradeListener::getTradeSellersSaleDays() const
    {
        return mImpl.mTradeCache.mSellersSaleDays;
    }

    char MamdaTradeListener::getTradeStopStock() const
    {
        return mImpl.mTradeCache.mStopStockInd;
    }

    bool MamdaTradeListener::getIsIrregular() const
    {
        return mImpl.mTradeCache.mIsIrregular;
    }

    mama_seqnum_t MamdaTradeListener::getBeginGapSeqNum() const
    {
        return mImpl.mTradeCache.mGapBegin;
    }

    mama_seqnum_t MamdaTradeListener::getEndGapSeqNum() const
    {
        return mImpl.mTradeCache.mGapEnd;
    }

    bool MamdaTradeListener::getIsCancel() const
    {
        return mImpl.mTradeCache.mIsCancel;
    }

    mama_seqnum_t MamdaTradeListener::getOrigSeqNum() const
    {
        return mImpl.mTradeCache.mOrigSeqNum;
    }

    const MamaPrice& MamdaTradeListener::getOrigPrice() const
    {
        return mImpl.mTradeCache.mOrigPrice;
    }

    mama_quantity_t MamdaTradeListener::getOrigVolume() const
    {
        return mImpl.mTradeCache.mOrigVolume;
    }

    const char* MamdaTradeListener::getOrigPartId() const
    {
        return mImpl.mTradeCache.mOrigPartId.c_str();
    }

    const char* MamdaTradeListener::getOrigQual() const
    {
        return mImpl.mTradeCache.mOrigQualStr.c_str();
    }

    const char* MamdaTradeListener::getOrigQualNative() const
    {
        return mImpl.mTradeCache.mOrigQualNativeStr.c_str();
    }

    mama_u32_t MamdaTradeListener::getOrigSellersSaleDays() const
    {
        return mImpl.mTradeCache.mOrigSellersSaleDays;
    }

    char MamdaTradeListener::getOrigStopStock() const
    {
        return mImpl.mTradeCache.mOrigStopStockInd;
    }

    const MamaPrice& MamdaTradeListener::getCorrPrice() const
    {
        return mImpl.mTradeCache.mCorrPrice;
    }

    mama_quantity_t MamdaTradeListener::getCorrVolume() const
    {
        return mImpl.mTradeCache.mCorrVolume;
    }

    const char* MamdaTradeListener::getCorrPartId() const
    {
        return mImpl.mTradeCache.mCorrPartId.c_str();
    }

    const char* MamdaTradeListener::getCorrQual() const
    {
        return mImpl.mTradeCache.mCorrQualStr.c_str();
    }

    const char* MamdaTradeListener::getCorrQualNative() const
    {
        return mImpl.mTradeCache.mCorrQualNativeStr.c_str();
    }

    mama_u32_t MamdaTradeListener::getCorrSellersSaleDays() const
    {
        return mImpl.mTradeCache.mCorrSellersSaleDays;
    }

    char MamdaTradeListener::getCorrStopStock() const
    {
        return mImpl.mTradeCache.mCorrStopStockInd;
    }

    bool MamdaTradeListener::getIsIndicative() const
    {
        return mImpl.mTradeCache.mIsIndicative;
    }

    const MamaPrice& MamdaTradeListener::getOffExchangeTradePrice() const
    {
        return mImpl.mTradeCache.mOffExchangeTradePrice;
    }

    const MamaPrice& MamdaTradeListener::getOnExchangeTradePrice() const
    {
        return mImpl.mTradeCache.mOnExchangeTradePrice;
    }

    const char* MamdaTradeListener::getTradeId() const
    {
        return mImpl.mTradeCache.mTradeId.c_str();
    }

    char MamdaTradeListener::getShortSaleCircuitBreaker() const
    {
        return mImpl.mTradeCache.mShortSaleCircuitBreaker;
    }

    char MamdaTradeListener::getOrigShortSaleCircuitBreaker() const
    {
        return mImpl.mTradeCache.mOrigShortSaleCircuitBreaker;
    }

    char MamdaTradeListener::getCorrShortSaleCircuitBreaker() const
    {
        return mImpl.mTradeCache.mCorrShortSaleCircuitBreaker;
    }

    const char* MamdaTradeListener::getOrigTradeId() const
    {
        return mImpl.mTradeCache.mOrigTradeId.c_str();
    }

    bool MamdaTradeListener::getGenericFlag() const
    {
        return mImpl.mTradeCache.mGenericFlag;
    }


    /*      FieldState Accessors        */

    MamdaFieldState MamdaTradeListener::getSymbolFieldState() const
    {
        return mImpl.mTradeCache.mSymbolFieldState;
    }

    MamdaFieldState MamdaTradeListener::getPartIdFieldState() const
    {
        return mImpl.mTradeCache.mPartIdFieldState;
    }

    MamdaFieldState MamdaTradeListener::getSrcTimeFieldState() const
    {
        return mImpl.mTradeCache.mSrcTimeFieldState;
    }

    MamdaFieldState MamdaTradeListener::getActivityTimeFieldState() const
    {
        return mImpl.mTradeCache.mActTimeFieldState;
    }

    MamdaFieldState MamdaTradeListener::getLineTimeFieldState() const
    {
        return mImpl.mTradeCache.mLineTimeFieldState;
    }

    MamdaFieldState MamdaTradeListener::getSendTimeFieldState() const
    {
        return mImpl.mTradeCache.mSendTimeFieldState;
    }

    MamdaFieldState MamdaTradeListener::getMsgQualFieldState() const
    {
        return mImpl.mMsgQualFieldState;
    }

    MamdaFieldState MamdaTradeListener::getPubIdFieldState() const
    {  
        return mImpl.mTradeCache.mPubIdFieldState;
    }

    MamdaFieldState MamdaTradeListener::getEventSeqNumFieldState() const
    {
        return mImpl.mTradeCache.mEventSeqNumFieldState;
    }

    MamdaFieldState MamdaTradeListener::getEventTimeFieldState() const
    {
        return mImpl.mTradeCache.mEventTimeFieldState;
    }

    MamdaFieldState MamdaTradeListener::getLastPriceFieldState() const
    {
        return mImpl.mTradeCache.mLastPriceFieldState;
    }

    MamdaFieldState MamdaTradeListener::getLastVolumeFieldState() const
    {
        return mImpl.mTradeCache.mLastVolumeFieldState;
    }

    MamdaFieldState MamdaTradeListener::getLastPartIdFieldState() const
    {
        return mImpl.mTradeCache.mLastPartIdFieldState;
    }

    MamdaFieldState MamdaTradeListener::getLastTimeFieldState() const
    {
        return mImpl.mTradeCache.mLastTimeFieldState;
    }

    MamdaFieldState MamdaTradeListener::getIrregPartIdFieldState() const
    {
        return mImpl.mTradeCache.mIrregPartIdFieldState;
    }

    MamdaFieldState MamdaTradeListener::getIrregVolumeFieldState() const
    {
        return mImpl.mTradeCache.mIrregVolumeFieldState;
    }

    MamdaFieldState MamdaTradeListener::getIrregPriceFieldState() const
    {
        return mImpl.mTradeCache.mIrregPriceFieldState;
    }

    MamdaFieldState MamdaTradeListener::getIrregTimeFieldState() const
    {
        return mImpl.mTradeCache.mIrregTimeFieldState;
    }

    MamdaFieldState MamdaTradeListener::getTradeDateFieldState() const
    {
        return mImpl.mTradeCache.mTradeDateFieldState;
    }

    MamdaFieldState MamdaTradeListener::getSideFieldState() const
    {
         return mImpl.mTradeCache.mSideFieldState;
    }

    MamdaFieldState MamdaTradeListener::getTradeCountFieldState() const
    {
        return mImpl.mTradeCache.mTradeCountFieldState;
    }

    MamdaFieldState MamdaTradeListener::getAccVolumeFieldState() const
    {
        return mImpl.mTradeCache.mAccVolumeFieldState;
    }

    MamdaFieldState MamdaTradeListener::getOffExAccVolumeFieldState() const
    {
        return mImpl.mTradeCache.mOffExAccVolumeFieldState;
    }

    MamdaFieldState MamdaTradeListener::getOnExAccVolumeFieldState() const
    {
        return mImpl.mTradeCache.mOnExAccVolumeFieldState;
    }

    MamdaFieldState MamdaTradeListener::getNetChangeFieldState() const
    {
        return mImpl.mTradeCache.mNetChangeFieldState;
    }

    MamdaFieldState MamdaTradeListener::getPctChangeFieldState() const
    {
        return mImpl.mTradeCache.mPctChangeFieldState;
    }

    MamdaFieldState MamdaTradeListener::getTradeDirectionFieldState() const
    {
        return mImpl.mTradeCache.mTradeDirectionFieldState;
    }

    MamdaFieldState MamdaTradeListener::getOpenPriceFieldState() const
    {
        return mImpl.mTradeCache.mOpenPriceFieldState;
    }

    MamdaFieldState MamdaTradeListener::getHighPriceFieldState() const
    {
        return mImpl.mTradeCache.mHighPriceFieldState;
    }

    MamdaFieldState MamdaTradeListener::getLowPriceFieldState() const
    {
        return mImpl.mTradeCache.mLowPriceFieldState;
    }

    MamdaFieldState MamdaTradeListener::getClosePriceFieldState() const
    {
        return mImpl.mTradeCache.mClosePriceFieldState;
    }

    MamdaFieldState MamdaTradeListener::getPrevClosePriceFieldState() const
    {
        return mImpl.mTradeCache.mPrevClosePriceFieldState;
    }

    MamdaFieldState MamdaTradeListener::getAdjPrevClosePriceFieldState() const
    {
        return mImpl.mTradeCache.mAdjPrevCloseFieldState;
    }

    MamdaFieldState MamdaTradeListener::getPrevCloseDateFieldState() const
    {
        return mImpl.mTradeCache.mPrevCloseDateFieldState;
    }

    MamdaFieldState MamdaTradeListener::getTradePriceFieldState() const
    {
        return mImpl.mTradeCache.mTradePriceFieldState;
    }

    MamdaFieldState MamdaTradeListener::getBlockCountFieldState() const
    {
        return mImpl.mTradeCache.mBlockCountFieldState;
    }

    MamdaFieldState MamdaTradeListener::getBlockVolumeFieldState() const
    {
        return mImpl.mTradeCache.mBlockVolumeFieldState;
    }

    MamdaFieldState MamdaTradeListener::getVwapFieldState() const
    {
        return mImpl.mTradeCache.mVwapFieldState;
    }

    MamdaFieldState MamdaTradeListener::getOffExVwapFieldState() const
    {
        return mImpl.mTradeCache.mOffExVwapFieldState;
    }

     MamdaFieldState MamdaTradeListener::getOnExVwapFieldState() const
    {
        return mImpl.mTradeCache.mOnExVwapFieldState;
    }

    MamdaFieldState MamdaTradeListener::getTotalValueFieldState() const
    {
        return mImpl.mTradeCache.mTotalValueFieldState;
    }

    MamdaFieldState MamdaTradeListener::getOffExTotalValueFieldState() const
    {
        return mImpl.mTradeCache.mOffExTotalValueFieldState;
    }

    MamdaFieldState MamdaTradeListener::getOnExTotalValueFieldState() const
    {
        return mImpl.mTradeCache.mOnExTotalValueFieldState;
    }

    MamdaFieldState MamdaTradeListener::getStdDevFieldState() const
    {
        return mImpl.mTradeCache.mStdDevFieldState;
    }

    MamdaFieldState MamdaTradeListener::getStdDevSumFieldState() const
    {
        return mImpl.mTradeCache.mStdDevSumFieldState;
    }

    MamdaFieldState MamdaTradeListener::getStdDevSumSquaresFieldState() const
    {
        return mImpl.mTradeCache.mStdDevSumSquaresFieldState;
    }

    MamdaFieldState MamdaTradeListener::getTradeUnitsFieldState() const
    {
        return mImpl.mTradeCache.mTradeUnitsFieldState;
    }

    MamdaFieldState MamdaTradeListener::getLastSeqNumFieldState() const
    {
        return mImpl.mTradeCache.mLastSeqNumFieldState;
    }

    MamdaFieldState MamdaTradeListener::getHighSeqNumFieldState() const
    {
        return mImpl.mTradeCache.mHighSeqNumFieldState;
    }

    MamdaFieldState MamdaTradeListener::getLowSeqNumFieldState() const
    {
        return mImpl.mTradeCache.mLowSeqNumFieldState;
    }

    MamdaFieldState MamdaTradeListener::getTotalVolumeSeqNumFieldState() const
    {
        return mImpl.mTradeCache.mTotalVolumeSeqNumFieldState;
    }

    MamdaFieldState MamdaTradeListener::getCurrencyCodeFieldState() const
    {
        return mImpl.mTradeCache.mCurrencyCodeFieldState;
    }

    MamdaFieldState MamdaTradeListener::getSettlePriceFieldState() const
    {
        return mImpl.mTradeCache.mSettlePriceFieldState;
    }

    MamdaFieldState MamdaTradeListener::getSettleDateFieldState() const
    {
        return mImpl.mTradeCache.mSettleDateFieldState;
    }

    MamdaFieldState MamdaTradeListener::getOrderIdFieldState() const
    {
        return mImpl.mTradeCache.mOrderIdFieldState;
    }

    MamdaFieldState MamdaTradeListener::getUniqueIdFieldState() const
    {
        return mImpl.mTradeCache.mUniqueIdFieldState;
    }

    MamdaFieldState MamdaTradeListener::getCorrTradeIdFieldState() const
    {
        return mImpl.mTradeCache.mCorrTradeIdFieldState;
    }

    MamdaFieldState MamdaTradeListener::getTradeActionFieldState() const
    {
        return mImpl.mTradeCache.mTradeActionFieldState;
    }

    MamdaFieldState MamdaTradeListener::getTradeExecVenueFieldState() const
    {
        return mImpl.mTradeCache.mTradeExecVenueFieldState;
    }

    MamdaFieldState MamdaTradeListener::getTradeVolumeFieldState() const
    {
        return mImpl.mTradeCache.mTradeVolumeFieldState;
    }

    MamdaFieldState MamdaTradeListener::getTradePartIdFieldState() const
    {
        return mImpl.mTradeCache.mTradePartIdFieldState;
    }

    MamdaFieldState MamdaTradeListener::getTradeQualFieldState() const
    {
        return mImpl.mTradeCache.mTradeQualStrFieldState;
    }

    MamdaFieldState MamdaTradeListener::getTradeQualNativeFieldState() const
    {
        return mImpl.mTradeCache.mTradeQualNativeStrFieldState;
    }

    MamdaFieldState MamdaTradeListener::getTradeSellersSaleDaysFieldState() const
    {
        return mImpl.mTradeCache.mSellersSaleDaysFieldState;
    }

    MamdaFieldState MamdaTradeListener::getTradeStopStockFieldState() const
    {
        return mImpl.mTradeCache.mStopStockIndFieldState;
    }

    MamdaFieldState MamdaTradeListener::getIsIrregularFieldState() const
    {
        return mImpl.mTradeCache.mIsIrregularFieldState;
    }

    MamdaFieldState MamdaTradeListener::getBeginGapSeqNumFieldState() const
    {
        return mImpl.mTradeCache.mGapBeginFieldState;
    }

    MamdaFieldState MamdaTradeListener::getEndGapSeqNumFieldState() const
    {
        return mImpl.mTradeCache.mGapEndFieldState;
    }

    MamdaFieldState MamdaTradeListener::getOrigSeqNumFieldState() const
    {
        return mImpl.mTradeCache.mOrigSeqNumFieldState;
    }

    MamdaFieldState MamdaTradeListener::getOrigPriceFieldState() const
    {
        return mImpl.mTradeCache.mOrigPriceFieldState;
    }

    MamdaFieldState MamdaTradeListener::getOrigVolumeFieldState() const
    {
        return mImpl.mTradeCache.mOrigVolumeFieldState;
    }

    MamdaFieldState MamdaTradeListener::getOrigPartIdFieldState() const
    {
        return mImpl.mTradeCache.mOrigPartIdFieldState;
    }

    MamdaFieldState MamdaTradeListener::getOrigQualFieldState() const
    {
        return mImpl.mTradeCache.mOrigQualStrFieldState;
    }

    MamdaFieldState MamdaTradeListener::getOrigQualNativeFieldState() const
    {
        return mImpl.mTradeCache.mOrigQualNativeStrFieldState;
    }

    MamdaFieldState MamdaTradeListener::getOrigSellersSaleDaysFieldState() const
    {
        return mImpl.mTradeCache.mOrigSellersSaleDaysFieldState;
    }

    MamdaFieldState MamdaTradeListener::getOrigStopStockFieldState() const
    {
        return mImpl.mTradeCache.mOrigStopStockIndFieldState;
    }

    MamdaFieldState MamdaTradeListener::getCorrPriceFieldState() const
    {
        return mImpl.mTradeCache.mCorrPriceFieldState;
    }

    MamdaFieldState MamdaTradeListener::getCorrVolumeFieldState() const
    {
        return mImpl.mTradeCache.mCorrVolumeFieldState;
    }

    MamdaFieldState MamdaTradeListener::getCorrPartIdFieldState() const
    {
        return mImpl.mTradeCache.mCorrPartIdFieldState;
    }

    MamdaFieldState MamdaTradeListener::getCorrQualFieldState() const
    {
        return mImpl.mTradeCache.mCorrQualStrFieldState;
    }

    MamdaFieldState MamdaTradeListener::getCorrQualNativeFieldState() const
    {
        return mImpl.mTradeCache.mCorrQualNativeStrFieldState;
    }

    MamdaFieldState MamdaTradeListener::getCorrSellersSaleDaysFieldState() const
    {
        return mImpl.mTradeCache.mCorrSellersSaleDaysFieldState;
    }

    MamdaFieldState MamdaTradeListener::getCorrStopStockFieldState() const
    {
        return mImpl.mTradeCache.mCorrStopStockIndFieldState;
    }

    MamdaFieldState MamdaTradeListener::getOffExchangeTradePriceFieldState() const
    {
        return mImpl.mTradeCache.mOffExchangeTradePriceFieldState;
    }

    MamdaFieldState MamdaTradeListener::getOnExchangeTradePriceFieldState() const
    {
        return mImpl.mTradeCache.mOnExchangeTradePriceFieldState;
    }

    MamdaFieldState MamdaTradeListener::getTradeIdFieldState() const
    {
        return mImpl.mTradeCache.mTradeIdFieldState;
    }

    MamdaFieldState MamdaTradeListener::getOrigTradeIdFieldState() const
    {
        return mImpl.mTradeCache.mOrigTradeIdFieldState;
    }

    MamdaFieldState MamdaTradeListener::getGenericFlagFieldState() const
    {
        return mImpl.mTradeCache.mGenericFlagFieldState;
    }

    MamdaFieldState MamdaTradeListener::getShortSaleCircuitBreakerFieldState() const
    {
        return mImpl.mTradeCache.mShortSaleCircuitBreakerFieldState;
    }

    MamdaFieldState MamdaTradeListener::getOrigShortSaleCircuitBreakerFieldState() const
    {
        return mImpl.mTradeCache.mOrigShortSaleCircuitBreakerFieldState;
    }

    MamdaFieldState MamdaTradeListener::getCorrShortSaleCircuitBreakerFieldState() const
    {
        return mImpl.mTradeCache.mCorrShortSaleCircuitBreakerFieldState;
    }

    /*  end FieldState Accessors    */


    void MamdaTradeListener::setCheckUpdatesForTrades (bool check)
    {
        mImpl.mCheckUpdatesForTrades = check;
    }

    void MamdaTradeListener::reset ()
    {
        mImpl.reset ();
    }

    void MamdaTradeListener::assertEqual (MamdaTradeListener* rhs)
    {
        mImpl.assertEqual (rhs->mImpl);
    }

    void MamdaTradeListener::onMsg (
        MamdaSubscription* subscription,
        const MamaMsg&     msg,
        short              msgType)
    {
        // If msg is a trade-related message, invoke the
        // appropriate callback:

        if(subscription->checkDebugLevel (MAMA_LOG_LEVEL_FINE))
        {
            const char *contractSymbol = "N/A";
            msg.tryString (MamdaCommonFields::ISSUE_SYMBOL, contractSymbol);

            mama_forceLog (MAMA_LOG_LEVEL_FINE,
                           "MamdaTradeListener (%s.%s(%s)) onMsg(). "
                           "msg type: %s msg status %s\n",
                           subscription->getSource (),
                           subscription->getSymbol (),
                           contractSymbol,
                           msg.getMsgTypeName (),
                           msg.getMsgStatusString ());
        }

        mImpl.mTradeCache.mIsSnapshot = false;
        
        switch (msgType)
        {
            case MAMA_MSG_TYPE_SNAPSHOT:
            case MAMA_MSG_TYPE_INITIAL:
            case MAMA_MSG_TYPE_RECAP:
                mImpl.mTradeCache.mIsSnapshot = true;
            case MAMA_MSG_TYPE_PREOPENING:
            case MAMA_MSG_TYPE_TRADE:
            case MAMA_MSG_TYPE_CANCEL:
            case MAMA_MSG_TYPE_ERROR:
            case MAMA_MSG_TYPE_CORRECTION:
            case MAMA_MSG_TYPE_CLOSING:
                mImpl.handleTradeMessage (subscription, msg, msgType);
                break;
            case MAMA_MSG_TYPE_UPDATE:
                if (mImpl.mCheckUpdatesForTrades)
                {
                    mImpl.handleTradeMessage (subscription, msg, msgType);
                }
                break;
            default:
                // Not a trade-related message (ignored)
                break;
        }
    }


    MamdaTradeListener::MamdaTradeListenerImpl::MamdaTradeListenerImpl(
        MamdaTradeListener&  listener)
        : mListener                             (listener)
        , mHandler                              (NULL)
        , mProcessPosDupAndOutOfSeqAsTransient  (false)
        , mResolvePossiblyDuplicate             (false)
        , mUsePosDupAndOutOfSeqHandlers         (false)
        , mIsTransientMsg                       (false)
        , mTransientCache                       (NULL)
        , mTradeCache                           (mRegularCache)
        , mIgnoreUpdate                         (false)
        , mCheckUpdatesForTrades                (true)
    {
        clearCache (mRegularCache);
    }

    void MamdaTradeListener::MamdaTradeListenerImpl::clearCache (
        TradeCache& tradeCache)
    {
        tradeCache.mLastVolume               = 0.00;
        tradeCache.mIrregVolume              = 0;
        tradeCache.mIrregPartId              = "";
        tradeCache.mIrregTime.clear();
        tradeCache.mSendTime.clear();
        tradeCache.mAccVolume                = 0;
        tradeCache.mOffExAccVolume           = 0;
        tradeCache.mOnExAccVolume            = 0;
        tradeCache.mTradeDirection           = TRADE_DIR_UNKNOWN;
        tradeCache.mNetChange.clear();
        tradeCache.mPctChange                = 0.00;
        tradeCache.mOpenPrice.clear();
        tradeCache.mHighPrice.clear();
        tradeCache.mLowPrice.clear();
        tradeCache.mClosePrice.clear();
        tradeCache.mPrevClosePrice.clear();
        tradeCache.mPrevCloseDate.clear();
        tradeCache.mAdjPrevClose.clear();
        tradeCache.mTradeCount               = 0;
        tradeCache.mTmpSide                  = TRADE_SIDE_UNKNOWN;    
        tradeCache.mSide                     = "";
        tradeCache.mBlockVolume              = 0;
        tradeCache.mBlockCount               = 0;
        tradeCache.mVwap                     = 0.00;
        tradeCache.mOffExVwap                = 0.00;
        tradeCache.mOnExVwap                 = 0.00;
        tradeCache.mTotalValue               = 0.00;
        tradeCache.mOffExTotalValue          = 0.00;
        tradeCache.mOnExTotalValue           = 0.00;
        tradeCache.mStdDev                   = 0.00;
        tradeCache.mStdDevSum                = 0.00;
        tradeCache.mStdDevSumSquares         = 0.00;
        tradeCache.mTradeUnits               = "";
        tradeCache.mLastSeqNum               = 0;
        tradeCache.mHighSeqNum               = 0;
        tradeCache.mLowSeqNum                = 0;
        tradeCache.mTotalVolumeSeqNum        = 0;
        tradeCache.mCurrencyCode             = "";
        tradeCache.mOrderId                  = 0;
        tradeCache.mEventSeqNum              = 0;
        tradeCache.mEventTime.clear();
        tradeCache.mTradePrice.clear();
        tradeCache.mTradeVolume              = 0;
        tradeCache.mTradePartId              = "";
        tradeCache.mTradeQualStr             = "";
        tradeCache.mTradeQualNativeStr       = "";
        tradeCache.mSellersSaleDays          = 0;
        tradeCache.mStopStockInd             = ' ';
        tradeCache.mTmpTradeCount            = 0;
        tradeCache.mIsCancel                 = false;
        tradeCache.mOrigSeqNum               = 0;
        tradeCache.mOrigPrice.clear();
        tradeCache.mOrigVolume               = 0;
        tradeCache.mOrigPartId               = "";
        tradeCache.mOrigQualStr              = "";
        tradeCache.mOrigQualNativeStr        = "";
        tradeCache.mOrigSellersSaleDays      = 0;
        tradeCache.mOrigStopStockInd         = ' ';
        tradeCache.mCorrPrice.clear();
        tradeCache.mCorrVolume               = 0;
        tradeCache.mCorrPartId               = "";
        tradeCache.mCorrQualStr              = "";
        tradeCache.mCorrQualNativeStr        = "";
        tradeCache.mCorrSellersSaleDays      = 0;
        tradeCache.mCorrStopStockInd         = ' ';
        tradeCache.mCancelTime.clear();
        tradeCache.mCorrTime.clear();
        tradeCache.mGapBegin                 = 0;
        tradeCache.mGapEnd                   = 0;
        tradeCache.mIsIndicative             = 0;
        tradeCache.mIsIrregular              = false;
        tradeCache.mWasIrregular             = false;
        tradeCache.mGotTradeTime             = false;
        tradeCache.mGotTradePrice            = false;
        tradeCache.mGotTradeSize             = false;
        tradeCache.mGotTradeCount            = false;
        tradeCache.mLastGenericMsgWasTrade   = false;
        tradeCache.mUniqueId                 = "";   
        tradeCache.mCorrTradeId              = "";   
        tradeCache.mTradeAction              = "";
        tradeCache.mTradeExecVenue           = TRADE_EXEC_VENUE_UNKNOWN;
        tradeCache.mIsSnapshot               = false;
        tradeCache.mSettlePrice.clear();
        tradeCache.mSettleDate.clear();
        tradeCache.mOffExchangeTradePrice.clear();
        tradeCache.mOnExchangeTradePrice.clear();
        tradeCache.mTradeId                  = "";
        tradeCache.mOrigTradeId              = "";
        tradeCache.mGenericFlag              = false;
        tradeCache.mShortSaleCircuitBreaker  = ' ';
        tradeCache.mOrigShortSaleCircuitBreaker  = ' ';
        tradeCache.mCorrShortSaleCircuitBreaker  = ' ';

        tradeCache.mSymbolFieldState                = NOT_INITIALISED;
        tradeCache.mPartIdFieldState                = NOT_INITIALISED;
        tradeCache.mSrcTimeFieldState               = NOT_INITIALISED;
        tradeCache.mActTimeFieldState               = NOT_INITIALISED;
        tradeCache.mLineTimeFieldState              = NOT_INITIALISED;
        tradeCache.mSendTimeFieldState              = NOT_INITIALISED;
        tradeCache.mPubIdFieldState                 = NOT_INITIALISED;
        tradeCache.mIsIrregularFieldState           = NOT_INITIALISED;
        tradeCache.mWasIrregularFieldState          = NOT_INITIALISED;
        tradeCache.mLastPriceFieldState             = NOT_INITIALISED;
        tradeCache.mLastVolumeFieldState            = NOT_INITIALISED;
        tradeCache.mLastPartIdFieldState            = NOT_INITIALISED;
        tradeCache.mLastTimeFieldState              = NOT_INITIALISED;
        tradeCache.mTradeDateFieldState             = NOT_INITIALISED;
        tradeCache.mSideFieldState                  = NOT_INITIALISED;
        tradeCache.mIrregPriceFieldState            = NOT_INITIALISED;
        tradeCache.mIrregVolumeFieldState           = NOT_INITIALISED;
        tradeCache.mIrregPartIdFieldState           = NOT_INITIALISED;
        tradeCache.mIrregTimeFieldState             = NOT_INITIALISED;
        tradeCache.mAccVolumeFieldState             = NOT_INITIALISED;
        tradeCache.mOffExAccVolumeFieldState        = NOT_INITIALISED;
        tradeCache.mOnExAccVolumeFieldState         = NOT_INITIALISED;
        tradeCache.mTradeDirectionFieldState        = NOT_INITIALISED;
        tradeCache.mNetChangeFieldState             = NOT_INITIALISED;
        tradeCache.mPctChangeFieldState             = NOT_INITIALISED;
        tradeCache.mOpenPriceFieldState             = NOT_INITIALISED;
        tradeCache.mHighPriceFieldState             = NOT_INITIALISED;
        tradeCache.mLowPriceFieldState              = NOT_INITIALISED;
        tradeCache.mClosePriceFieldState            = NOT_INITIALISED;
        tradeCache.mPrevClosePriceFieldState        = NOT_INITIALISED;
        tradeCache.mPrevCloseDateFieldState         = NOT_INITIALISED;
        tradeCache.mAdjPrevCloseFieldState          = NOT_INITIALISED;
        tradeCache.mTradeCountFieldState            = NOT_INITIALISED;
        tradeCache.mBlockVolumeFieldState           = NOT_INITIALISED;
        tradeCache.mBlockCountFieldState            = NOT_INITIALISED;
        tradeCache.mVwapFieldState                  = NOT_INITIALISED;
        tradeCache.mOffExVwapFieldState             = NOT_INITIALISED;
        tradeCache.mOnExVwapFieldState              = NOT_INITIALISED;
        tradeCache.mTotalValueFieldState            = NOT_INITIALISED;
        tradeCache.mOffExTotalValueFieldState       = NOT_INITIALISED;
        tradeCache.mOnExTotalValueFieldState        = NOT_INITIALISED;
        tradeCache.mStdDevFieldState                = NOT_INITIALISED;
        tradeCache.mOrderIdFieldState               = NOT_INITIALISED;
        tradeCache.mStdDevSumFieldState             = NOT_INITIALISED;
        tradeCache.mStdDevSumSquaresFieldState      = NOT_INITIALISED;
        tradeCache.mTradeUnitsFieldState            = NOT_INITIALISED;
        tradeCache.mLastSeqNumFieldState            = NOT_INITIALISED;
        tradeCache.mHighSeqNumFieldState            = NOT_INITIALISED;
        tradeCache.mLowSeqNumFieldState             = NOT_INITIALISED;
        tradeCache.mTotalVolumeSeqNumFieldState     = NOT_INITIALISED;
        tradeCache.mCurrencyCodeFieldState          = NOT_INITIALISED;
        tradeCache.mUniqueIdFieldState              = NOT_INITIALISED;  
        tradeCache.mCorrTradeIdFieldState           = NOT_INITIALISED;
        tradeCache.mTradeActionFieldState           = NOT_INITIALISED;
        tradeCache.mSettlePriceFieldState           = NOT_INITIALISED;
        tradeCache.mSettleDateFieldState            = NOT_INITIALISED;
        tradeCache.mOffExchangeTradePriceFieldState = NOT_INITIALISED;
        tradeCache.mOnExchangeTradePriceFieldState  = NOT_INITIALISED; 
        tradeCache.mEventSeqNumFieldState           = NOT_INITIALISED;
        tradeCache.mEventTimeFieldState             = NOT_INITIALISED; 
        tradeCache.mTradePriceFieldState            = NOT_INITIALISED;
        tradeCache.mTradeVolumeFieldState           = NOT_INITIALISED;
        tradeCache.mTradePartIdFieldState           = NOT_INITIALISED;
        tradeCache.mTradeQualStrFieldState          = NOT_INITIALISED;
        tradeCache.mTradeQualNativeStrFieldState    = NOT_INITIALISED;
        tradeCache.mSellersSaleDaysFieldState       = NOT_INITIALISED;
        tradeCache.mStopStockIndFieldState          = NOT_INITIALISED;
        tradeCache.mTmpTradeCountFieldState         = NOT_INITIALISED;
        tradeCache.mTradeExecVenueFieldState        = NOT_INITIALISED;
        tradeCache.mGapBeginFieldState              = NOT_INITIALISED;
        tradeCache.mGapEndFieldState                = NOT_INITIALISED;
        
        tradeCache.mOrigSeqNumFieldState            = NOT_INITIALISED;
        tradeCache.mOrigPriceFieldState             = NOT_INITIALISED;
        tradeCache.mOrigVolumeFieldState            = NOT_INITIALISED;
        tradeCache.mOrigPartIdFieldState            = NOT_INITIALISED;
        tradeCache.mOrigQualStrFieldState           = NOT_INITIALISED;
        tradeCache.mOrigQualNativeStrFieldState     = NOT_INITIALISED;
        tradeCache.mOrigSellersSaleDaysFieldState   = NOT_INITIALISED;
        tradeCache.mOrigStopStockIndFieldState      = NOT_INITIALISED;
        tradeCache.mCorrPriceFieldState             = NOT_INITIALISED;
        tradeCache.mCorrVolumeFieldState            = NOT_INITIALISED;
        tradeCache.mCorrPartIdFieldState            = NOT_INITIALISED;
        tradeCache.mCorrQualStrFieldState           = NOT_INITIALISED;
        tradeCache.mCorrQualNativeStrFieldState     = NOT_INITIALISED;
        tradeCache.mCorrSellersSaleDaysFieldState   = NOT_INITIALISED;
        tradeCache.mCorrStopStockIndFieldState      = NOT_INITIALISED;
        tradeCache.mCorrTimeFieldState              = NOT_INITIALISED;
        tradeCache.mCancelTimeFieldState            = NOT_INITIALISED;
        tradeCache.mTradeIdFieldState               = NOT_INITIALISED;
        tradeCache.mOrigTradeIdFieldState           = NOT_INITIALISED;
        tradeCache.mGenericFlagFieldState           = NOT_INITIALISED;
        tradeCache.mShortSaleCircuitBreakerFieldState = NOT_INITIALISED;
        tradeCache.mOrigShortSaleCircuitBreakerFieldState = NOT_INITIALISED;
        tradeCache.mCorrShortSaleCircuitBreakerFieldState = NOT_INITIALISED;
    }

    void MamdaTradeListener::MamdaTradeListenerImpl::updateFieldStates()
    {
        if (mTradeCache.mSymbolFieldState == MODIFIED)        
            mTradeCache.mSymbolFieldState = NOT_MODIFIED;

        if (mTradeCache.mPartIdFieldState == MODIFIED)        
            mTradeCache.mPartIdFieldState = NOT_MODIFIED;  

        if (mTradeCache.mSrcTimeFieldState == MODIFIED)       
            mTradeCache.mSrcTimeFieldState = NOT_MODIFIED; 

        if (mTradeCache.mActTimeFieldState == MODIFIED)       
            mTradeCache.mActTimeFieldState = NOT_MODIFIED; 

        if (mTradeCache.mLineTimeFieldState == MODIFIED)      
            mTradeCache.mLineTimeFieldState = NOT_MODIFIED;

        if (mTradeCache.mSendTimeFieldState == MODIFIED)      
            mTradeCache.mSendTimeFieldState = NOT_MODIFIED;

        if (mTradeCache.mPubIdFieldState == MODIFIED)         
            mTradeCache.mPubIdFieldState = NOT_MODIFIED;   

        if (mTradeCache.mIsIrregularFieldState == MODIFIED)   
            mTradeCache.mIsIrregularFieldState = NOT_MODIFIED; 

        if (mTradeCache.mWasIrregularFieldState == MODIFIED)  
            mTradeCache.mWasIrregularFieldState = NOT_MODIFIED;

        if (mTradeCache.mLastPriceFieldState == MODIFIED)     
            mTradeCache.mLastPriceFieldState = NOT_MODIFIED; 

        if (mTradeCache.mLastVolumeFieldState == MODIFIED)    
            mTradeCache.mLastVolumeFieldState = NOT_MODIFIED;

        if (mTradeCache.mLastPartIdFieldState == MODIFIED)    
            mTradeCache.mLastPartIdFieldState = NOT_MODIFIED;

        if (mTradeCache.mLastTimeFieldState == MODIFIED)      
            mTradeCache.mLastTimeFieldState = NOT_MODIFIED;   

        if (mTradeCache.mTradeDateFieldState == MODIFIED)     
            mTradeCache.mTradeDateFieldState = NOT_MODIFIED;  

        if (mTradeCache.mSideFieldState == MODIFIED)     
            mTradeCache.mSideFieldState = NOT_MODIFIED;    
     
        if (mTradeCache.mIrregPriceFieldState == MODIFIED)      
            mTradeCache.mIrregPriceFieldState = NOT_MODIFIED; 

        if (mTradeCache.mIrregVolumeFieldState == MODIFIED)   
            mTradeCache.mIrregVolumeFieldState = NOT_MODIFIED; 

        if (mTradeCache.mIrregPartIdFieldState == MODIFIED)   
            mTradeCache.mIrregPartIdFieldState = NOT_MODIFIED; 

        if (mTradeCache.mIrregTimeFieldState == MODIFIED)     
            mTradeCache.mIrregTimeFieldState = NOT_MODIFIED;   

        if (mTradeCache.mAccVolumeFieldState == MODIFIED)       
            mTradeCache.mAccVolumeFieldState = NOT_MODIFIED;  
     
        if (mTradeCache.mOffExAccVolumeFieldState == MODIFIED)
            mTradeCache.mOffExAccVolumeFieldState = NOT_MODIFIED;

        if (mTradeCache.mOnExAccVolumeFieldState == MODIFIED)   
            mTradeCache.mOnExAccVolumeFieldState = NOT_MODIFIED; 

        if (mTradeCache.mTradeDirectionFieldState == MODIFIED)
            mTradeCache.mTradeDirectionFieldState = NOT_MODIFIED;

        if (mTradeCache.mNetChangeFieldState == MODIFIED)     
            mTradeCache.mNetChangeFieldState = NOT_MODIFIED;    
     
        if (mTradeCache.mPctChangeFieldState == MODIFIED)     
            mTradeCache.mPctChangeFieldState = NOT_MODIFIED;
         
        if (mTradeCache.mOpenPriceFieldState == MODIFIED)     
            mTradeCache.mOpenPriceFieldState = NOT_MODIFIED; 
        
        if (mTradeCache.mHighPriceFieldState == MODIFIED)     
            mTradeCache.mHighPriceFieldState = NOT_MODIFIED;  
       
        if (mTradeCache.mLowPriceFieldState == MODIFIED)      
            mTradeCache.mLowPriceFieldState = NOT_MODIFIED; 
         
        if (mTradeCache.mClosePriceFieldState == MODIFIED)    
            mTradeCache.mClosePriceFieldState = NOT_MODIFIED;  
      
        if (mTradeCache.mPrevClosePriceFieldState == MODIFIED)
            mTradeCache.mPrevClosePriceFieldState = NOT_MODIFIED;

        if (mTradeCache.mPrevCloseDateFieldState == MODIFIED) 
            mTradeCache.mPrevCloseDateFieldState = NOT_MODIFIED; 

        if (mTradeCache.mAdjPrevCloseFieldState == MODIFIED)  
            mTradeCache.mAdjPrevCloseFieldState = NOT_MODIFIED;  

        if (mTradeCache.mTradeCountFieldState == MODIFIED)    
            mTradeCache.mTradeCountFieldState = NOT_MODIFIED;    

        if (mTradeCache.mBlockVolumeFieldState == MODIFIED)   
            mTradeCache.mBlockVolumeFieldState = NOT_MODIFIED;   

        if (mTradeCache.mBlockCountFieldState == MODIFIED)    
            mTradeCache.mBlockCountFieldState = NOT_MODIFIED;

        if (mTradeCache.mVwapFieldState == MODIFIED)          
            mTradeCache.mVwapFieldState = NOT_MODIFIED;  
        
        if (mTradeCache.mOffExVwapFieldState == MODIFIED)     
            mTradeCache.mOffExVwapFieldState = NOT_MODIFIED; 

        if (mTradeCache.mOnExVwapFieldState == MODIFIED)      
            mTradeCache.mOnExVwapFieldState = NOT_MODIFIED;  

        if (mTradeCache.mTotalValueFieldState == MODIFIED)    
            mTradeCache.mTotalValueFieldState = NOT_MODIFIED;

        if (mTradeCache.mOffExTotalValueFieldState == MODIFIED)
            mTradeCache.mOffExTotalValueFieldState = NOT_MODIFIED;

        if (mTradeCache.mOnExTotalValueFieldState == MODIFIED) 
            mTradeCache.mOnExTotalValueFieldState = NOT_MODIFIED; 

        if (mTradeCache.mStdDevFieldState == MODIFIED)    
            mTradeCache.mStdDevFieldState = NOT_MODIFIED; 

        if (mTradeCache.mOrderIdFieldState == MODIFIED)   
            mTradeCache.mOrderIdFieldState = NOT_MODIFIED;

        if (mTradeCache.mStdDevSumFieldState == MODIFIED)      
            mTradeCache.mStdDevSumFieldState = NOT_MODIFIED;    
      
        if (mTradeCache.mStdDevSumSquaresFieldState == MODIFIED)   
            mTradeCache.mStdDevSumSquaresFieldState = NOT_MODIFIED;

        if (mTradeCache.mTradeUnitsFieldState == MODIFIED)          
            mTradeCache.mTradeUnitsFieldState = NOT_MODIFIED;  

        if (mTradeCache.mLastSeqNumFieldState == MODIFIED)      
            mTradeCache.mLastSeqNumFieldState = NOT_MODIFIED;  

        if (mTradeCache.mHighSeqNumFieldState == MODIFIED)      
            mTradeCache.mHighSeqNumFieldState = NOT_MODIFIED;  

        if (mTradeCache.mLowSeqNumFieldState == MODIFIED)       
            mTradeCache.mLowSeqNumFieldState = NOT_MODIFIED;   

        if (mTradeCache.mTotalVolumeSeqNumFieldState == MODIFIED)
            mTradeCache.mTotalVolumeSeqNumFieldState = NOT_MODIFIED; 

        if (mTradeCache.mCurrencyCodeFieldState == MODIFIED)        
            mTradeCache.mCurrencyCodeFieldState = NOT_MODIFIED; 

        if (mTradeCache.mUniqueIdFieldState == MODIFIED)         
            mTradeCache.mUniqueIdFieldState = NOT_MODIFIED; 

        if (mTradeCache.mTradeIdFieldState == MODIFIED)         
            mTradeCache.mTradeIdFieldState = NOT_MODIFIED;

        if (mTradeCache.mCorrTradeIdFieldState == MODIFIED)         
            mTradeCache.mCorrTradeIdFieldState = NOT_MODIFIED;

        if (mTradeCache.mOrigTradeIdFieldState == MODIFIED)         
            mTradeCache.mOrigTradeIdFieldState = NOT_MODIFIED;

        if (mTradeCache.mTradeActionFieldState == MODIFIED)      
            mTradeCache.mTradeActionFieldState = NOT_MODIFIED;  

        if (mTradeCache.mSettlePriceFieldState == MODIFIED)      
            mTradeCache.mSettlePriceFieldState = NOT_MODIFIED;  

        if (mTradeCache.mSettleDateFieldState == MODIFIED)       
            mTradeCache.mSettleDateFieldState = NOT_MODIFIED;   

        if (mTradeCache.mOffExchangeTradePriceFieldState == MODIFIED)
            mTradeCache.mOffExchangeTradePriceFieldState = NOT_MODIFIED;

        if (mTradeCache.mOnExchangeTradePriceFieldState == MODIFIED) 
            mTradeCache.mOnExchangeTradePriceFieldState = NOT_MODIFIED; 

        if (mTradeCache.mEventSeqNumFieldState == MODIFIED)          
            mTradeCache.mEventSeqNumFieldState = NOT_MODIFIED;

        if (mTradeCache.mEventTimeFieldState == MODIFIED)      
            mTradeCache.mEventTimeFieldState = NOT_MODIFIED;  

        if (mTradeCache.mTradePriceFieldState == MODIFIED)     
            mTradeCache.mTradePriceFieldState = NOT_MODIFIED; 

        if (mTradeCache.mTradeVolumeFieldState == MODIFIED)       
            mTradeCache.mTradeVolumeFieldState = NOT_MODIFIED;

        if (mTradeCache.mTradePartIdFieldState == MODIFIED)    
            mTradeCache.mTradePartIdFieldState = NOT_MODIFIED;

        if (mTradeCache.mTradeQualStrFieldState == MODIFIED)   
            mTradeCache.mTradeQualStrFieldState = NOT_MODIFIED; 

        if (mTradeCache.mTradeQualNativeStrFieldState == MODIFIED)
            mTradeCache.mTradeQualNativeStrFieldState = NOT_MODIFIED;

        if (mTradeCache.mSellersSaleDaysFieldState == MODIFIED)   
            mTradeCache.mSellersSaleDaysFieldState = NOT_MODIFIED; 

        if (mTradeCache.mStopStockIndFieldState == MODIFIED)      
            mTradeCache.mStopStockIndFieldState = NOT_MODIFIED;

        if (mTradeCache.mTmpTradeCountFieldState == MODIFIED)   
            mTradeCache.mTmpTradeCountFieldState = NOT_MODIFIED;

        if (mTradeCache.mTradeExecVenueFieldState == MODIFIED)   
            mTradeCache.mTradeExecVenueFieldState = NOT_MODIFIED;

        if (mTradeCache.mOrigSeqNumFieldState == MODIFIED)   
            mTradeCache.mOrigSeqNumFieldState = NOT_MODIFIED;

        if (mTradeCache.mOrigPriceFieldState == MODIFIED)     
            mTradeCache.mOrigPriceFieldState = NOT_MODIFIED; 

        if (mTradeCache.mOrigVolumeFieldState == MODIFIED)    
            mTradeCache.mOrigVolumeFieldState = NOT_MODIFIED;

        if (mTradeCache.mOrigPartIdFieldState == MODIFIED)    
            mTradeCache.mOrigPartIdFieldState = NOT_MODIFIED;

        if (mTradeCache.mOrigQualStrFieldState == MODIFIED)   
            mTradeCache.mOrigQualStrFieldState = NOT_MODIFIED;

        if (mTradeCache.mOrigQualNativeStrFieldState == MODIFIED) 
            mTradeCache.mOrigQualNativeStrFieldState = NOT_MODIFIED;

        if (mTradeCache.mOrigSellersSaleDaysFieldState == MODIFIED)  
            mTradeCache.mOrigSellersSaleDaysFieldState = NOT_MODIFIED;

        if (mTradeCache.mOrigStopStockIndFieldState == MODIFIED)   
            mTradeCache.mOrigStopStockIndFieldState = NOT_MODIFIED; 

        if (mTradeCache.mCorrPriceFieldState == MODIFIED)          
            mTradeCache.mCorrPriceFieldState = NOT_MODIFIED; 

        if (mTradeCache.mCorrVolumeFieldState == MODIFIED)    
            mTradeCache.mCorrVolumeFieldState = NOT_MODIFIED; 

        if (mTradeCache.mCorrPartIdFieldState == MODIFIED)     
            mTradeCache.mCorrPartIdFieldState = NOT_MODIFIED; 

        if (mTradeCache.mCorrQualStrFieldState == MODIFIED)    
            mTradeCache.mCorrQualStrFieldState = NOT_MODIFIED;

        if (mTradeCache.mCorrQualNativeStrFieldState == MODIFIED) 
            mTradeCache.mCorrQualNativeStrFieldState = NOT_MODIFIED;

        if (mTradeCache.mCorrSellersSaleDaysFieldState == MODIFIED)
            mTradeCache.mCorrSellersSaleDaysFieldState = NOT_MODIFIED;

        if (mTradeCache.mCorrStopStockIndFieldState == MODIFIED)   
            mTradeCache.mCorrStopStockIndFieldState = NOT_MODIFIED;

        if (mTradeCache.mCorrTimeFieldState == MODIFIED)          
            mTradeCache.mCorrTimeFieldState = NOT_MODIFIED;

        if (mTradeCache.mCancelTimeFieldState == MODIFIED)          
            mTradeCache.mCancelTimeFieldState = NOT_MODIFIED;

        if (mTradeCache.mGapBeginFieldState == MODIFIED)         
            mTradeCache.mGapBeginFieldState = NOT_MODIFIED;

        if (mTradeCache.mGapEndFieldState == MODIFIED)         
            mTradeCache.mGapEndFieldState = NOT_MODIFIED;

        if (mTradeCache.mTradeIdFieldState == MODIFIED)
            mTradeCache.mTradeIdFieldState = NOT_MODIFIED;

        if (mTradeCache.mOrigTradeIdFieldState == MODIFIED)
            mTradeCache.mOrigTradeIdFieldState = NOT_MODIFIED;

        if (mTradeCache.mGenericFlagFieldState == MODIFIED)
            mTradeCache.mGenericFlagFieldState = NOT_MODIFIED;

        if (mTradeCache.mShortSaleCircuitBreakerFieldState == MODIFIED)
            mTradeCache.mShortSaleCircuitBreakerFieldState = NOT_MODIFIED; 

        if (mTradeCache.mOrigShortSaleCircuitBreakerFieldState == MODIFIED)
            mTradeCache.mOrigShortSaleCircuitBreakerFieldState = NOT_MODIFIED; 

        if (mTradeCache.mCorrShortSaleCircuitBreakerFieldState == MODIFIED)
            mTradeCache.mCorrShortSaleCircuitBreakerFieldState = NOT_MODIFIED; 
    }
       
    void MamdaTradeListener::MamdaTradeListenerImpl::handleTradeMessage (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg,
        short               msgType)
    {   
        // Ensure that the field handling is set up (once for all
        // MamdaTradeListener instances).

        if (!mUpdatersComplete)
        {
            wthread_static_mutex_lock (&mTradeFieldUpdaterLockMutex);
        
            if (!mUpdatersComplete)
            {
                if (!MamdaTradeFields::isSet())
                {
                     mama_log (MAMA_LOG_LEVEL_WARN,
                               "MamdaTradeListener: MamdaTradeFields::setDictionary() "
                               "has not been called.");
                     wthread_static_mutex_unlock (&mTradeFieldUpdaterLockMutex);
                     return;
                }
        
                try
                {
                    initFieldUpdaters ();
                }
                catch (MamaStatus &e)
                {
                    mama_log (MAMA_LOG_LEVEL_WARN,
                              "MamdaTradeListener: Could not set field updaters: %s",
                              e.toString ());
                    wthread_static_mutex_unlock (&mTradeFieldUpdaterLockMutex);
                    return;
                }
                mUpdatersComplete = true;
            }
        
            wthread_static_mutex_unlock (&mTradeFieldUpdaterLockMutex);
        }
        
        // Determine if this message is a duplicate or a tranient record
        // which should not update the regular cache.
        bool isDuplicateMsg = evaluateMsgQual (subscription, msg);
        
        if (!isDuplicateMsg)
        {
            if (mIsTransientMsg && mProcessPosDupAndOutOfSeqAsTransient)
            {
                // Use Transient Cache.
                if (mTransientCache == NULL)
                {
                    mTransientCache = new TradeCache ();
                }
                mTradeCache = *mTransientCache;
            }
        
            // Copy IsIrregular value as it needs to be reset for recaps
            mTradeCache.mWasIrregular = mTradeCache.mIsIrregular;
            mTradeCache.mWasIrregularFieldState = MODIFIED;
        
            // Process fields in message:
            updateFieldStates ();
            updateTradeFields (msg);

            // Handle according to message type:
            switch (msgType)
            {
                case MAMA_MSG_TYPE_INITIAL:
                case MAMA_MSG_TYPE_RECAP:
                case MAMA_MSG_TYPE_PREOPENING:
                    handleRecap (subscription, msg);
                    break;

                case MAMA_MSG_TYPE_SNAPSHOT:
                    handleRecap (subscription, msg);
                    break;

                case MAMA_MSG_TYPE_TRADE:
                    handleTrade (subscription, msg);
                    break;

                case MAMA_MSG_TYPE_CANCEL:
                    handleCancelOrError (subscription, msg, true);
                    break;

                case MAMA_MSG_TYPE_ERROR:
                    handleCancelOrError (subscription, msg, false);
                    break;

                case MAMA_MSG_TYPE_CORRECTION:
                    handleCorrection (subscription, msg);
                    break;

                case MAMA_MSG_TYPE_CLOSING:
                    handleClosing (subscription, msg);
                    break;

                case MAMA_MSG_TYPE_UPDATE:
                    handleUpdate (subscription, msg);
                    break;
            }

            if (mIsTransientMsg && mProcessPosDupAndOutOfSeqAsTransient)
            {
                mTradeCache = mRegularCache;
                clearCache(*mTransientCache);
            }
        }
        else
        {
            if(subscription->checkDebugLevel(MAMA_LOG_LEVEL_FINE))
            {
                const char *contractSymbol = "N/A";
                msg.tryString (MamdaCommonFields::ISSUE_SYMBOL, contractSymbol);

                mama_forceLog (MAMA_LOG_LEVEL_FINE,
                               "MamdaTradeListener (%s.%s(%s)) "
                               "Duplicate message NOT processed.\n",
                               contractSymbol,
                               subscription->getSource (),
                               subscription->getSymbol ());
            }
        }
    }

    bool MamdaTradeListener::MamdaTradeListenerImpl::evaluateMsgQual(
        MamdaSubscription*  subscription,
        const MamaMsg&      msg)
    {
        bool isDuplicateMsg = false;
        mIsTransientMsg    = false;

        mMsgQual.clear();
        uint16_t msgQualVal = 0;

        // Does message contains a qualifier- need to parse this anyway.
        // Need Fid as reserved field may not be in data dictionary
        if (msg.tryU16 (MamaFieldMsgQual.mName, MamaFieldMsgQual.mFid, msgQualVal))
        {
            mMsgQual.setValue(msgQualVal);

            // If qualifier indicates messages is possbily duplicate
            // and the listener has been configure to attempt to 
            // resolve the ambiguity regarding the duplicate 
            // status of the message.
            if (mMsgQual.getIsPossiblyDuplicate() && mResolvePossiblyDuplicate)
            {
                mama_seqnum_t seqNum = 0;
                MamaDateTime  eventTime;

                if (msg.tryU32 (MamdaTradeFields::TRADE_SEQNUM, seqNum) &&
                    msg.tryDateTime (MamdaTradeFields::TRADE_TIME, eventTime)) 
                {
                    // Only make a determination as to wheither or
                    // not a PossiblyDuplicate msg is an actual duplicate
                    // if the msg contains both a sequence number 
                    // and event time.
                    if ((seqNum < mRegularCache.mEventSeqNum) && 
                        (eventTime < mRegularCache.mEventTime))
                    {
                        mMsgQual.setIsDefinatelyDuplicate (true);
                        mMsgQual.setIsPossiblyDuplicate   (false);
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

    void MamdaTradeListener::MamdaTradeListenerImpl::handleRecap (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg)
    {
        if (subscription->checkDebugLevel (MAMA_LOG_LEVEL_FINE))
        {
            const char *contractSymbol = "N/A";
            msg.tryString (MamdaCommonFields::ISSUE_SYMBOL, contractSymbol);

            mama_forceLog (MAMA_LOG_LEVEL_FINE,
                           "MamdaTradeListener (%s.%s(%s)) handleRecap().\n",
                           contractSymbol,
                           subscription->getSource (),
                           subscription->getSymbol ());
        }

        checkTradeCount (subscription, msg, false);
        if (mHandler)
        {
            if (mUsePosDupAndOutOfSeqHandlers && mIsTransientMsg)
            {
                invokeTransientHandler(subscription, msg);
            }
            else
            {
                mHandler->onTradeRecap (subscription, 
                                        mListener, 
                                        msg, 
                                        mListener);
            }
        }
    }

    void MamdaTradeListener::MamdaTradeListenerImpl::handleTrade (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg)
    {
        checkTradeCount (subscription, msg, true);

        if (mIgnoreUpdate)
        {
            mIgnoreUpdate = false;
            return;
        }

        if(subscription->checkDebugLevel(MAMA_LOG_LEVEL_FINE))
        {
            const char *contractSymbol = "N/A";
            msg.tryString (MamdaCommonFields::ISSUE_SYMBOL, contractSymbol);

            mama_forceLog (MAMA_LOG_LEVEL_FINE,
                           "MamdaTradeListener (%s.%s(%s)) handleTrade().\n",
                           contractSymbol,
                           subscription->getSource (),
                           subscription->getSymbol ());
        }

        if (mHandler)
        {
            if (mUsePosDupAndOutOfSeqHandlers && mIsTransientMsg)
            {
                invokeTransientHandler(subscription, msg);
            }
            else
            {
                mHandler->onTradeReport (subscription, 
                                         mListener, 
                                         msg, 
                                         mListener, 
                                         mListener);
            }
        }
    }

    void MamdaTradeListener::MamdaTradeListenerImpl::handleCancelOrError (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg,
        bool                isCancel)
    {
        checkTradeCount (subscription, msg, true);

        if (mIgnoreUpdate)
        {
            mIgnoreUpdate = false;
            return;
        }

        if (subscription->checkDebugLevel (MAMA_LOG_LEVEL_FINE))
        {
            const char *contractSymbol = "N/A";
            msg.tryString (MamdaCommonFields::ISSUE_SYMBOL, contractSymbol);

            mama_forceLog (MAMA_LOG_LEVEL_FINE,
                           "MamdaTradeListener (%s.%s(%s)) handleCancelOrError(): %d\n",
                           contractSymbol,
                           subscription->getSource (),
                           subscription->getSymbol (),
                           isCancel);
        }

        mTradeCache.mIsCancel = isCancel;

        if (!mTradeCache.mCancelTime.empty())
        {
            mTradeCache.mEventTime = mTradeCache.mCancelTime;
            mTradeCache.mEventTimeFieldState = MODIFIED;
        }

        if (mHandler)
        {
            if (mUsePosDupAndOutOfSeqHandlers && mIsTransientMsg)
            {
                invokeTransientHandler(subscription, msg);
            }
            else
            {
                mHandler->onTradeCancelOrError (subscription, 
                                                mListener, 
                                                msg, 
                                                mListener, 
                                                mListener);
            }
        }
    }

    void MamdaTradeListener::MamdaTradeListenerImpl::handleCorrection (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg)
    {
        checkTradeCount (subscription, msg, true);

        if (mIgnoreUpdate)
        {
            mIgnoreUpdate = false;
            return;
        }

        if (subscription->checkDebugLevel (MAMA_LOG_LEVEL_FINE))
        {
            const char *contractSymbol = "N/A";
            msg.tryString (MamdaCommonFields::ISSUE_SYMBOL, contractSymbol);

            mama_forceLog (MAMA_LOG_LEVEL_FINE,
                           "MamdaTradeListener (%s.%s(%s)) handleCorrection()\n",
                           contractSymbol,
                           subscription->getSource (),
                           subscription->getSymbol ());
        }

        if(!mTradeCache.mCorrTime.empty())
        {
            mTradeCache.mEventTime = mTradeCache.mCorrTime;
            mTradeCache.mEventTimeFieldState = MODIFIED;
        }

        if (mHandler)
        {
            if (mUsePosDupAndOutOfSeqHandlers && mIsTransientMsg)
            {
                invokeTransientHandler(subscription, msg);
            }
            else
            {
                mHandler->onTradeCorrection (subscription, 
                                             mListener, 
                                             msg, 
                                             mListener, 
                                             mListener);
            }
        }
    }

    void MamdaTradeListener::MamdaTradeListenerImpl::handleClosing (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg)
    {
        if (subscription->checkDebugLevel (MAMA_LOG_LEVEL_FINE))
        {
            const char *contractSymbol = "N/A";
            msg.tryString (MamdaCommonFields::ISSUE_SYMBOL, contractSymbol);

            mama_forceLog (MAMA_LOG_LEVEL_FINE,
                           "MamdaTradeListener (%s.%s(%s)) handleClosing()\n",
                           contractSymbol,
                           subscription->getSource (),
                           subscription->getSymbol ());
        }

        if (mHandler)
        {
            if (mUsePosDupAndOutOfSeqHandlers && mIsTransientMsg)
            {
                invokeTransientHandler(subscription, msg);
            }
            else
            {
                mHandler->onTradeClosing (subscription, 
                                          mListener, 
                                          msg, 
                                          mListener, 
                                          mListener);
            }
        }
    }

    void MamdaTradeListener::MamdaTradeListenerImpl::handleUpdate (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg)
    {
        checkTradeCount (subscription, msg, true);

        if (mIgnoreUpdate)
        {
            mIgnoreUpdate = false;
            return;
        }

        if (subscription->checkDebugLevel (MAMA_LOG_LEVEL_FINE))
        {
            const char *contractSymbol = "N/A";
            msg.tryString (MamdaCommonFields::ISSUE_SYMBOL, contractSymbol);

            mama_forceLog (MAMA_LOG_LEVEL_FINE,
                           "MamdaTradeListener (%s.%s(%s)) handleUpdate(). "
                           "tradeCount = %d : tmpTradeCount = %d\n",
                           contractSymbol,
                           subscription->getSource (),
                           subscription->getSymbol (),
                           mTradeCache.mTradeCount, 
                           mTradeCache.mTmpTradeCount);
        }

        if (mHandler)
        {
            if (mUsePosDupAndOutOfSeqHandlers && mIsTransientMsg)
            {
                invokeTransientHandler(subscription, msg);
            }
            else
            {
                // Generic update (might even be a trade)
                if (mTradeCache.mLastGenericMsgWasTrade)
                {
                    /* We just received a new sequential trade -
                       but not marked as a trade. */
                    mHandler->onTradeReport (subscription, 
                                             mListener, 
                                             msg,
                                             mListener, 
                                             mListener);
                }
            }
        }
    }

    void MamdaTradeListener::MamdaTradeListenerImpl::invokeTransientHandler(
        MamdaSubscription*  subscription,
        const MamaMsg&      msg)
     {
         bool handled = false;
         if (mMsgQual.getIsOutOfSequence())
         {
             handled = true;
             mHandler->onTradeOutOfSequence (subscription, 
                                             mListener, 
                                             msg, 
                                             mListener, 
                                             mListener);
         }
         if (mMsgQual.getIsPossiblyDuplicate())
         {
             handled = true;
             mHandler->onTradePossiblyDuplicate (subscription, 
                                                 mListener, 
                                                 msg, 
                                                 mListener, 
                                                 mListener);
         }
         if (!handled)
         {
             const char *contractSymbol = "N/A";
             msg.tryString (MamdaCommonFields::ISSUE_SYMBOL, contractSymbol);

             mama_forceLog (MAMA_LOG_LEVEL_FINE,
                            "MamdaTradeListener (%s.%s(%s)) handleTrade(): "
                            "update: Transient Msg neither Out-of-Sequence "
                            "nor Possibly Stale",
                            subscription->getSource (),
                            subscription->getSymbol (),
                            contractSymbol);
         }
     }

    void MamdaTradeListener::MamdaTradeListenerImpl::updateTradeFields (
        const MamaMsg&  msg)
    {
        const char* symbol = NULL;
        const char* partId = NULL;

        getSymbolAndPartId (msg, symbol, partId);

        if (symbol) 
        {
          mTradeCache.mSymbol           = symbol;
          mTradeCache.mSymbolFieldState = MODIFIED;
        }      

        if (partId) 
        {
          mTradeCache.mPartId           = partId;
          mTradeCache.mPartIdFieldState = MODIFIED;
        }

        mTradeCache.mLastGenericMsgWasTrade   = false;
        mTradeCache.mGotTradeTime             = false;
        mTradeCache.mGotTradePrice            = false;
        mTradeCache.mGotTradeSize             = false;
        mTradeCache.mGotTradeCount            = false;

        // Iterate over all of the fields in the message.
        wthread_mutex_lock (&mTradeUpdateLock.mTradeUpdateLockMutex);
        msg.iterateFields (*this, NULL, NULL);
        wthread_mutex_unlock (&mTradeUpdateLock.mTradeUpdateLockMutex);


        // Check certain special fields.
        if (mTradeCache.mIsIrregular)
        {
            // This is an irregular trade; use the "irreg" fields, not "last"
            mTradeCache.mTradePrice   = mTradeCache.mIrregPrice;
            mTradeCache.mTradeVolume  = mTradeCache.mIrregVolume;
            mTradeCache.mTradePartId  = mTradeCache.mIrregPartId;

            mTradeCache.mTradePriceFieldState  = MODIFIED;
            mTradeCache.mTradeVolumeFieldState = MODIFIED;
            mTradeCache.mTradePartIdFieldState = MODIFIED;

            if(mTradeCache.mIrregTime.empty())
            {
                // Some feeds only use the one time field
                mTradeCache.mEventTime           = mTradeCache.mLastTime;
                mTradeCache.mEventTimeFieldState = MODIFIED;
            }
            else
            {
                mTradeCache.mEventTime           = mTradeCache.mIrregTime;
                mTradeCache.mEventTimeFieldState = MODIFIED;
            }
        }
        else
        {
            // This is a regular trade; use the "last" fields, not "irreg"
            mTradeCache.mTradePrice   = mTradeCache.mLastPrice;
            mTradeCache.mTradeVolume  = mTradeCache.mLastVolume;
            mTradeCache.mTradePartId  = mTradeCache.mLastPartId;
            mTradeCache.mEventTime    = mTradeCache.mLastTime;

            mTradeCache.mTradePriceFieldState  = MODIFIED;
            mTradeCache.mTradeVolumeFieldState = MODIFIED;
            mTradeCache.mTradePartIdFieldState = MODIFIED;
            mTradeCache.mEventTimeFieldState   = MODIFIED;
        }

        if( mTradeCache.mGotTradeTime ||
            mTradeCache.mGotTradePrice ||
            mTradeCache.mGotTradeSize )
        {
            mTradeCache.mLastGenericMsgWasTrade = true;
        }
    }

    void MamdaTradeListener::MamdaTradeListenerImpl::onField (
        const MamaMsg&       msg,
        const MamaMsgField&  field,
        void*                closure)
    {
        mama_fid_t fid = field.getFid();

        if (fid <= mFieldUpdatersSize)
        {
            TradeFieldUpdate* updater = mFieldUpdaters[fid];
            if (updater)
            {
                updater->onUpdate (*this, field);
            }
        }
    }

    void MamdaTradeListener::MamdaTradeListenerImpl::checkTradeCount (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg,
        bool                checkForGap)
    {
        mIgnoreUpdate = false;

        // If this is not a regular update, i.e., has a Msg Qualifier
        // indicating that it shouldn't update the regular cache but
        // the event merely passed on to registered callbacks then
        // don't do any gap related processing.
        if (mIsTransientMsg)
        {
            mTradeCache.mTradeCount           = mTradeCache.mTmpTradeCount;
            mTradeCache.mTradeCountFieldState = MODIFIED;
            return;
        }

        // Check number of trades for gaps
        mama_u32_t tradeCount    = mTradeCache.mTmpTradeCount;
        mama_u32_t conflateCount = 0;

        if (!msg.tryU32 ("wConflateTradeCount", 24, conflateCount))
        {
            conflateCount = 1;
        }

        if (checkForGap && mTradeCache.mGotTradeCount)
        {
            if ((mTradeCache.mTradeCount > 0) && 
                (tradeCount > (mTradeCache.mTradeCount+conflateCount)))
            {
                mTradeCache.mGapBegin   = mTradeCache.mTradeCount+conflateCount;
                mTradeCache.mGapEnd     = tradeCount-1;
                mTradeCache.mTradeCount = mTradeCache.mTmpTradeCount;

                mTradeCache.mGapBeginFieldState   = MODIFIED;
                mTradeCache.mGapEndFieldState     = MODIFIED;
                mTradeCache.mTradeCountFieldState = MODIFIED;

                if (mHandler)
                {
                    if (subscription->checkDebugLevel (MAMA_LOG_LEVEL_FINE))
                    {
                        const char *contractSymbol = "N/A";
                        msg.tryString (MamdaCommonFields::ISSUE_SYMBOL, contractSymbol);

                        mama_forceLog (MAMA_LOG_LEVEL_FINE,
                                       "MamdaTradeListener (%s.%s(%s)) onTradeGap().\n",
                                       subscription->getSource (),
                                       subscription->getSymbol (),
                                       contractSymbol);
                    }
                    mHandler->onTradeGap (subscription, 
                                          mListener, 
                                          msg,
                                          mListener, 
                                          mListener);
                }
            }
        }

        /* Check for duplicates.  Only check if the trade count is in the latest message */ 
        if (mTradeCache.mGotTradeCount &&
            tradeCount > 0
            && (tradeCount == mTradeCache.mTradeCount) )
        {
            mIgnoreUpdate = true;
        }

        mTradeCache.mTradeCount           = mTradeCache.mTmpTradeCount;
        mTradeCache.mTradeCountFieldState = MODIFIED;
    }

    MamdaTradeDirection MamdaTradeListener::MamdaTradeListenerImpl::getTradeDirection(
        const MamaMsgField&  field)
    {
        switch (field.getType())
        {
            case MAMA_FIELD_TYPE_I8:
            case MAMA_FIELD_TYPE_U8:
            case MAMA_FIELD_TYPE_I16:
            case MAMA_FIELD_TYPE_U16:
            case MAMA_FIELD_TYPE_I32:
            case MAMA_FIELD_TYPE_U32:
                return (MamdaTradeDirection) field.getU32();

            case MAMA_FIELD_TYPE_STRING:
                return mamdaTradeDirectionFromString (field.getString());

            default:
                break;
        }
        return TRADE_DIR_UNKNOWN;
    }

    MamdaTradeExecVenue MamdaTradeListener::MamdaTradeListenerImpl::getTradeExecVenue(
        const MamaMsgField&  field)
    {
        switch (field.getType())
        {
            case MAMA_FIELD_TYPE_I8:
            case MAMA_FIELD_TYPE_U8:
            case MAMA_FIELD_TYPE_I16:
            case MAMA_FIELD_TYPE_U16:
            case MAMA_FIELD_TYPE_I32:
            case MAMA_FIELD_TYPE_U32:
                return (MamdaTradeExecVenue) field.getU32();

            case MAMA_FIELD_TYPE_STRING:
                return mamdaTradeExecVenueFromString (field.getString());

            default:
                break;
        }
        return TRADE_EXEC_VENUE_UNKNOWN;
    }

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateLastPrice 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getPrice (impl.mTradeCache.mLastPrice);
            impl.mTradeCache.mGotTradePrice       = true;
            impl.mTradeCache.mLastPriceFieldState = MODIFIED;
            if (!impl.mTradeCache.mIsSnapshot)
            {
                impl.mTradeCache.mIsIrregular = false;
            }
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateLastVolume 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mTradeCache.mLastVolume           = field.getF64();
            impl.mTradeCache.mGotTradeSize         = true;
            impl.mTradeCache.mLastVolumeFieldState = MODIFIED;
            if (!impl.mTradeCache.mIsSnapshot)
            {
                impl.mTradeCache.mIsIrregular = false;
            }
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateLastPartId 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mTradeCache.mLastPartId           = field.getString();
            impl.mTradeCache.mLastPartIdFieldState = MODIFIED;
            if (!impl.mTradeCache.mIsSnapshot)
            {
                impl.mTradeCache.mIsIrregular = false;
            }
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateLastTime 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            MamaDateTime temp;
            field.getDateTime(temp);
            if (temp.hasTime())
            {
                impl.mTradeCache.mLastTime           = temp;
                impl.mTradeCache.mLastTimeFieldState = MODIFIED;
                impl.mTradeCache.mGotTradeTime       = true;
            }
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateTradeDate
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getDateTime(impl.mTradeCache.mTradeDate);
            impl.mTradeCache.mTradeDateFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateTradeSide
        : public TradeFieldUpdate
    {      
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {  
	     	switch (field.getType())
		    {
			    case MAMA_FIELD_TYPE_I8:
			    case MAMA_FIELD_TYPE_U8:
			    case MAMA_FIELD_TYPE_I16:
			    case MAMA_FIELD_TYPE_U16:
			    case MAMA_FIELD_TYPE_I32:
			    case MAMA_FIELD_TYPE_U32:
				    impl.mTradeCache.mSide.assign (toString ((MamdaTradeSide) field.getU32()));              
				    impl.mTradeCache.mSideFieldState = MODIFIED; 
                    break;
			    case MAMA_FIELD_TYPE_STRING:
				    impl.mTradeCache.mSide = field.getString();
				    impl.mTradeCache.mSideFieldState = MODIFIED; 
                    break;
			    default:
				    break;
		    }
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateAggressorSide
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {  
		    impl.mTradeCache.mSide.clear();
            impl.mTradeCache.mSide.push_back(field.getChar());
            impl.mTradeCache.mSideFieldState = MODIFIED;         
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateTradeQualifier 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mTradeCache.mTradeQualStr           = field.getString();
            impl.mTradeCache.mTradeQualStrFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateTradeQualifierNative 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mTradeCache.mTradeQualNativeStr           = field.getString();
            impl.mTradeCache.mTradeQualNativeStrFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateSellersSaleDays 
        : public TradeFieldUpdate
    {

        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mTradeCache.mSellersSaleDays           = field.getU32();
            impl.mTradeCache.mSellersSaleDaysFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateStopStockInd 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            // There is a bug in 2.14 FHs which can cause character fields to be sent as strings
            // FH property CharFieldAsStringField in 2.16-> can enable this behaviour
            // Adding support for this in MAMDA for client apps coded to expect this behaviour
            switch (field.getType())
            {
                case MAMA_FIELD_TYPE_CHAR :
                    impl.mTradeCache.mStopStockInd           = field.getChar();
                    impl.mTradeCache.mStopStockIndFieldState = MODIFIED;
                    break;
                case MAMA_FIELD_TYPE_STRING :
                    impl.mTradeCache.mStopStockInd           = field.getString() [0];
                    impl.mTradeCache.mStopStockIndFieldState = MODIFIED;
                    break;
                default : break;
            }
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateIsIrregular 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mTradeCache.mIsIrregular           = field.getBool();
            impl.mTradeCache.mIsIrregularFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateIrregPrice 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getPrice (impl.mTradeCache.mIrregPrice);
            impl.mTradeCache.mIrregPriceFieldState = MODIFIED;
            impl.mTradeCache.mGotTradePrice        = true;

            if (!impl.mTradeCache.mIsSnapshot && impl.mTradeCache.mIrregPrice != 0.0
                && impl.mTradeCache.mIsIrregular == false)
            {
                impl.mTradeCache.mIsIrregular           = true;
                impl.mTradeCache.mIsIrregularFieldState = MODIFIED;
            }
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateIrregVolume 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mTradeCache.mIrregVolume           = field.getF64();
            impl.mTradeCache.mIrregVolumeFieldState = MODIFIED;
            impl.mTradeCache.mGotTradeSize          = true;

            if (!impl.mTradeCache.mIsSnapshot && impl.mTradeCache.mIrregVolume != 0
                && impl.mTradeCache.mIsIrregular == false)
            {
                impl.mTradeCache.mIsIrregular           = true;
                impl.mTradeCache.mIsIrregularFieldState = MODIFIED;
            }
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateIrregPartId 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mTradeCache.mIrregPartId           = field.getString();
            impl.mTradeCache.mIrregPartIdFieldState = MODIFIED;

            if (!impl.mTradeCache.mIsSnapshot && impl.mTradeCache.mIrregPartId != ""
                && impl.mTradeCache.mIsIrregular == false)
            {
                impl.mTradeCache.mIsIrregular           = true;
                impl.mTradeCache.mIsIrregularFieldState = MODIFIED;
            }
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateIrregTime 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getDateTime(impl.mTradeCache.mIrregTime);
            impl.mTradeCache.mIrregTimeFieldState = MODIFIED;
            impl.mTradeCache.mGotTradeTime        = true;

            if (!impl.mTradeCache.mIsSnapshot && impl.mTradeCache.mIrregTime.hasTime()
                && impl.mTradeCache.mIsIrregular == false)
            {
                impl.mTradeCache.mIsIrregular           = true;
                impl.mTradeCache.mIsIrregularFieldState = MODIFIED;
            }
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateOrderId 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mTradeCache.mOrderId           = field.getI64();
            impl.mTradeCache.mOrderIdFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateTradeSrcTime 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getDateTime(impl.mTradeCache.mSrcTime);
            impl.mTradeCache.mSrcTimeFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateTradeActTime 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getDateTime(impl.mTradeCache.mActTime);
            impl.mTradeCache.mActTimeFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateTradeSendTime
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getDateTime(impl.mTradeCache.mSendTime);
            impl.mTradeCache.mSendTimeFieldState = MODIFIED;
        }
    };


    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateTradeLineTime 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getDateTime(impl.mTradeCache.mLineTime);
            impl.mTradeCache.mLineTimeFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateTradePubId 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mTradeCache.mPubId           = field.getString();
            impl.mTradeCache.mPubIdFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateTradeEventSeqNum 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mTradeCache.mEventSeqNum           = field.getU32();
            impl.mTradeCache.mEventSeqNumFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateTmpTradeCount 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mTradeCache.mTmpTradeCount           = field.getU32();
            impl.mTradeCache.mTmpTradeCountFieldState = MODIFIED;
            impl.mTradeCache.mGotTradeCount           = true;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateAccVolume 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mTradeCache.mAccVolume           = field.getF64();
            impl.mTradeCache.mAccVolumeFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateOffExAccVolume 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mTradeCache.mOffExAccVolume           = field.getF64();
            impl.mTradeCache.mOffExAccVolumeFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateOnExAccVolume 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mTradeCache.mOnExAccVolume           = field.getF64();
            impl.mTradeCache.mOnExAccVolumeFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateTradeDirection 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mTradeCache.mTradeDirection           = impl.getTradeDirection(field);
            impl.mTradeCache.mTradeDirectionFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateOpenPrice 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getPrice (impl.mTradeCache.mOpenPrice);
            impl.mTradeCache.mOpenPriceFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateHighPrice 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getPrice (impl.mTradeCache.mHighPrice);
            impl.mTradeCache.mHighPriceFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateLowPrice 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getPrice (impl.mTradeCache.mLowPrice);
            impl.mTradeCache.mLowPriceFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateNetChange 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getPrice (impl.mTradeCache.mNetChange);
            impl.mTradeCache.mNetChangeFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdatePctChange 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mTradeCache.mPctChange           = field.getF64();
            impl.mTradeCache.mPctChangeFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateBlockCount 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mTradeCache.mBlockCount           = field.getU32();
            impl.mTradeCache.mBlockCountFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateBlockVolume 
        : public TradeFieldUpdate
    {

        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mTradeCache.mBlockVolume           = field.getF64();
            impl.mTradeCache.mBlockVolumeFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateVwap 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mTradeCache.mVwap           = field.getF64();
            impl.mTradeCache.mVwapFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateOffExVwap
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mTradeCache.mOffExVwap           = field.getF64();
            impl.mTradeCache.mOffExVwapFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateOnExVwap
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mTradeCache.mOnExVwap           = field.getF64();
            impl.mTradeCache.mOnExVwapFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateTotalValue 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mTradeCache.mTotalValue           = field.getF64();
            impl.mTradeCache.mTotalValueFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateOffExTotalValue
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mTradeCache.mOffExTotalValue           = field.getF64();
            impl.mTradeCache.mOffExTotalValueFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateOnExTotalValue
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mTradeCache.mOnExTotalValue           = field.getF64();
            impl.mTradeCache.mOnExTotalValueFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateStdDev 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mTradeCache.mStdDev           = field.getF64();
            impl.mTradeCache.mStdDevFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateStdDevSum
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mTradeCache.mStdDevSum           = field.getF64();
            impl.mTradeCache.mStdDevSumFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateStdDevSumSquares
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mTradeCache.mStdDevSumSquares           = field.getF64();
            impl.mTradeCache.mStdDevSumSquaresFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateTradeUnits
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mTradeCache.mTradeUnits           = field.getString();
            impl.mTradeCache.mTradeUnitsFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateLastSeqNum
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mTradeCache.mLastSeqNum           = field.getU32();
            impl.mTradeCache.mLastSeqNumFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateHighSeqNum
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mTradeCache.mHighSeqNum           = field.getU32();
            impl.mTradeCache.mHighSeqNumFieldState = MODIFIED;
        }
    };
     
    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateLowSeqNum
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mTradeCache.mLowSeqNum           = field.getU32();
            impl.mTradeCache.mLowSeqNumFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateTotalVolumeSeqNum
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mTradeCache.mTotalVolumeSeqNum           = field.getU32();
            impl.mTradeCache.mTotalVolumeSeqNumFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateCurrencyCode
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mTradeCache.mCurrencyCode           = field.getString();
            impl.mTradeCache.mCurrencyCodeFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateClosePrice 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getPrice (impl.mTradeCache.mClosePrice);
            impl.mTradeCache.mClosePriceFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdatePrevClosePrice 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getPrice (impl.mTradeCache.mPrevClosePrice);
            impl.mTradeCache.mPrevClosePriceFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdatePrevCloseDate 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getDateTime(impl.mTradeCache.mPrevCloseDate);
            impl.mTradeCache.mPrevCloseDateFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateAdjPrevClose 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getPrice (impl.mTradeCache.mAdjPrevClose);
            impl.mTradeCache.mAdjPrevCloseFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateOrigSeqNum 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mTradeCache.mOrigSeqNum           = field.getU32();
            impl.mTradeCache.mOrigSeqNumFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateOrigPrice 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getPrice (impl.mTradeCache.mOrigPrice);
            impl.mTradeCache.mOrigPriceFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateOrigVolume 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mTradeCache.mOrigVolume           = field.getF64();
            impl.mTradeCache.mOrigVolumeFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateOrigPartId 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mTradeCache.mOrigPartId           = field.getString();
            impl.mTradeCache.mOrigPartIdFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateOrigQualifier 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mTradeCache.mOrigQualStr           = field.getString();
            impl.mTradeCache.mOrigQualStrFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateOrigQualifierNative 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mTradeCache.mOrigQualNativeStr           = field.getString();
            impl.mTradeCache.mOrigQualNativeStrFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateOrigSaleDays 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {

            impl.mTradeCache.mOrigSellersSaleDays           = field.getU32();
            impl.mTradeCache.mOrigSellersSaleDaysFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateOrigStopStockInd 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            // There is a bug in 2.14 FHs which can cause character fields to be sent as strings
            // FH property CharFieldAsStringField in 2.16-> can enable this behaviour
            // Adding support for this in MAMDA for client apps coded to expect this behaviour
            switch (field.getType())
            {
                case MAMA_FIELD_TYPE_CHAR :
                    impl.mTradeCache.mOrigStopStockInd           = field.getChar();
                    impl.mTradeCache.mOrigStopStockIndFieldState = MODIFIED;
                    break;
                case MAMA_FIELD_TYPE_STRING :
                    impl.mTradeCache.mOrigStopStockInd           = field.getString() [0];
                    impl.mTradeCache.mOrigStopStockIndFieldState = MODIFIED;
                    break;
                default : break;
            }
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateCancelTime 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getDateTime(impl.mTradeCache.mCancelTime);
            impl.mTradeCache.mCancelTimeFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateCorrPrice 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getPrice (impl.mTradeCache.mCorrPrice);
            impl.mTradeCache.mCorrPriceFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateCorrVolume 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mTradeCache.mCorrVolume           = field.getF64();
            impl.mTradeCache.mCorrVolumeFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateCorrPartId 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mTradeCache.mCorrPartId           = field.getString();
            impl.mTradeCache.mCorrPartIdFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateCorrQualifier 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mTradeCache.mCorrQualStr           = field.getString();
            impl.mTradeCache.mCorrQualStrFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateCorrQualifierNative 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mTradeCache.mCorrQualNativeStr           = field.getString();
            impl.mTradeCache.mCorrQualNativeStrFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateCorrSaleDays 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mTradeCache.mCorrSellersSaleDays           = field.getU32();
            impl.mTradeCache.mCorrSellersSaleDaysFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateCorrStopStockInd 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            // There is a bug in 2.14 FHs which can cause character fields to be sent as strings
            // FH property CharFieldAsStringField in 2.16-> can enable this behaviour
            // Adding support for this in MAMDA for client apps coded to expect this behaviour
            switch (field.getType ())
            {
                case MAMA_FIELD_TYPE_CHAR :
                    impl.mTradeCache.mCorrStopStockInd           = field.getChar();
                    impl.mTradeCache.mCorrStopStockIndFieldState = MODIFIED;
                    break;
                case MAMA_FIELD_TYPE_STRING :
                    impl.mTradeCache.mCorrStopStockInd           = field.getString() [0];
                    impl.mTradeCache.mCorrStopStockIndFieldState = MODIFIED;
                    break;
                default : break;
            }
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateCorrTime 
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getDateTime (impl.mTradeCache.mCorrTime);
            impl.mTradeCache.mCorrTimeFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateUniqueId
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mTradeCache.mUniqueId           = field.getString();
            impl.mTradeCache.mUniqueIdFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateCorrTradeId
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mTradeCache.mCorrTradeId           = field.getString();
            impl.mTradeCache.mCorrTradeIdFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateTradeAction
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mTradeCache.mTradeAction           = field.getString();
            impl.mTradeCache.mTradeActionFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateTradeExecVenue
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mTradeCache.mTradeExecVenue           = impl.getTradeExecVenue(field);
            impl.mTradeCache.mTradeExecVenueFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateSettlePrice
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getPrice (impl.mTradeCache.mSettlePrice);
            impl.mTradeCache.mSettlePriceFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateSettleDate
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getDateTime (impl.mTradeCache.mSettleDate);
            impl.mTradeCache.mSettleDateFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateOffExchangeTradePrice
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getPrice (impl.mTradeCache.mOffExchangeTradePrice);
            impl.mTradeCache.mOffExchangeTradePriceFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateOnExchangeTradePrice
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getPrice (impl.mTradeCache.mOnExchangeTradePrice);
            impl.mTradeCache.mOnExchangeTradePriceFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateTradeId
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
	        switch (field.getType())
            {
                case MAMA_FIELD_TYPE_STRING:
                    impl.mTradeCache.mTradeId           = field.getString();
                    impl.mTradeCache.mTradeIdFieldState = MODIFIED;            
                    break;
                default: 
                    char tradeId[64];
                    mama_size_t length;
                    field.getAsString (tradeId, length);
                    impl.mTradeCache.mTradeId           = tradeId;
                    impl.mTradeCache.mTradeIdFieldState = MODIFIED;
		        break;
            }
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateOrigTradeId
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            switch (field.getType())
            {
                case MAMA_FIELD_TYPE_STRING:
                    impl.mTradeCache.mOrigTradeId           = field.getString();
                    impl.mTradeCache.mOrigTradeIdFieldState = MODIFIED;            
                    break;
                default: 
                    char origTradeId[64];
                    mama_size_t length;
                    field.getAsString (origTradeId, length);
                    impl.mTradeCache.mOrigTradeId           = origTradeId;
                    impl.mTradeCache.mOrigTradeIdFieldState = MODIFIED;
                    break;
            }
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateGenericFlag
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mTradeCache.mGenericFlag           = field.getBool();
            impl.mTradeCache.mGenericFlagFieldState = MODIFIED;
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateShortSaleCircuitBreaker
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {       
            if (field.getChar() != impl.mTradeCache.mShortSaleCircuitBreaker)
            {
                impl.mTradeCache.mShortSaleCircuitBreaker           = field.getChar();
                impl.mTradeCache.mShortSaleCircuitBreakerFieldState = MODIFIED;
            } 
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateOrigShortSaleCircuitBreaker
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {       
            if (field.getChar() != impl.mTradeCache.mOrigShortSaleCircuitBreaker)
            {
                impl.mTradeCache.mOrigShortSaleCircuitBreaker           = field.getChar();
                impl.mTradeCache.mOrigShortSaleCircuitBreakerFieldState = MODIFIED;
            } 
        }
    };

    struct MamdaTradeListener::MamdaTradeListenerImpl::FieldUpdateCorrShortSaleCircuitBreaker
        : public TradeFieldUpdate
    {
        void onUpdate (MamdaTradeListener::MamdaTradeListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {       
            if (field.getChar() != impl.mTradeCache.mCorrShortSaleCircuitBreaker)
            {
                impl.mTradeCache.mCorrShortSaleCircuitBreaker           = field.getChar();
                impl.mTradeCache.mCorrShortSaleCircuitBreakerFieldState = MODIFIED;
            } 
        }
    };

    TradeFieldUpdate** MamdaTradeListener::MamdaTradeListenerImpl::
        mFieldUpdaters = NULL;

    volatile mama_fid_t  MamdaTradeListener::MamdaTradeListenerImpl::
        mFieldUpdatersSize = 0;

    wthread_static_mutex_t MamdaTradeListener::MamdaTradeListenerImpl::
        mTradeFieldUpdaterLockMutex = WSTATIC_MUTEX_INITIALIZER;

    bool MamdaTradeListener::MamdaTradeListenerImpl::mUpdatersComplete = false;


    void MamdaTradeListener::MamdaTradeListenerImpl::initFieldUpdaters ()
    {
        if (!mFieldUpdaters)
        {
            mFieldUpdaters = 
                new TradeFieldUpdate* [MamdaTradeFields::getMaxFid() +1];

            mFieldUpdatersSize = MamdaTradeFields::getMaxFid();

            /* Use uint32_t instead of uint16_t to avoid infinite loop if max FID = 65535 */
            for (uint32_t i = 0; i <= mFieldUpdatersSize; ++i)
            {
                mFieldUpdaters[i] = NULL;
            }
        }

        initFieldUpdater (MamdaCommonFields::SRC_TIME,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateTradeSrcTime);

        initFieldUpdater (MamdaCommonFields::ACTIVITY_TIME,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateTradeActTime);

        initFieldUpdater (MamdaCommonFields::SEND_TIME,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateTradeSendTime);

        initFieldUpdater (MamdaCommonFields::LINE_TIME,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateTradeLineTime);

        initFieldUpdater (MamdaCommonFields::PUB_ID,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateTradePubId);

        initFieldUpdater (MamdaTradeFields::TRADE_PRICE,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateLastPrice);

        initFieldUpdater (MamdaTradeFields::TRADE_DATE,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateTradeDate);

        initFieldUpdater (MamdaTradeFields::TRADE_SIDE,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateTradeSide); 
      
        initFieldUpdater (MamdaTradeFields::AGGRESSOR_SIDE,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateAggressorSide); 
            
        initFieldUpdater (MamdaTradeFields::TRADE_TIME,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateLastTime);

        initFieldUpdater (MamdaTradeFields::LAST_DIRECTION,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateTradeDirection);

        initFieldUpdater (MamdaTradeFields::NET_CHANGE,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateNetChange);

        initFieldUpdater (MamdaTradeFields::PCT_CHANGE,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdatePctChange);

        initFieldUpdater (MamdaTradeFields::TRADE_SIZE,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateLastVolume);

        initFieldUpdater (MamdaTradeFields::TOTAL_VOLUME,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateAccVolume);

        initFieldUpdater (MamdaTradeFields::OFF_EXCHANGE_TOTAL_VOLUME,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateOffExAccVolume);

        initFieldUpdater (MamdaTradeFields::ON_EXCHANGE_TOTAL_VOLUME,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateOnExAccVolume);

        initFieldUpdater (MamdaTradeFields::TRADE_UNITS,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateTradeUnits);

        initFieldUpdater (MamdaTradeFields::HIGH_PRICE,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateHighPrice);

        initFieldUpdater (MamdaTradeFields::LOW_PRICE,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateLowPrice);

        initFieldUpdater (MamdaTradeFields::OPEN_PRICE,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateOpenPrice);

        initFieldUpdater (MamdaTradeFields::CLOSE_PRICE,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateClosePrice);

        initFieldUpdater (MamdaTradeFields::PREV_CLOSE_PRICE,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdatePrevClosePrice);

        initFieldUpdater (MamdaTradeFields::PREV_CLOSE_DATE,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdatePrevCloseDate);

        initFieldUpdater (MamdaTradeFields::ADJ_PREV_CLOSE,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateAdjPrevClose);

        initFieldUpdater (MamdaTradeFields::TRADE_SEQNUM,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateTradeEventSeqNum);

        initFieldUpdater (MamdaTradeFields::TRADE_QUALIFIER,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateTradeQualifier);

        initFieldUpdater (MamdaTradeFields::TRADE_PART_ID,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateLastPartId);

        initFieldUpdater (MamdaTradeFields::TOTAL_VALUE,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateTotalValue);

        initFieldUpdater (MamdaTradeFields::OFF_EXCHANGE_TOTAL_VALUE,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateOffExTotalValue);

        initFieldUpdater (MamdaTradeFields::ON_EXCHANGE_TOTAL_VALUE,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateOnExTotalValue);

        initFieldUpdater (MamdaTradeFields::VWAP,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateVwap);

        initFieldUpdater (MamdaTradeFields::OFF_EXCHANGE_VWAP,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateOffExVwap);

        initFieldUpdater (MamdaTradeFields::ON_EXCHANGE_VWAP,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateOnExVwap);

        initFieldUpdater (MamdaTradeFields::STD_DEV,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateStdDev);

        initFieldUpdater (MamdaTradeFields::STD_DEV_SUM,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateStdDevSum);

        initFieldUpdater (MamdaTradeFields::STD_DEV_SUM_SQUARES,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateStdDevSumSquares);

        initFieldUpdater (MamdaTradeFields::LAST_SEQNUM,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateLastSeqNum);

        initFieldUpdater (MamdaTradeFields::HIGH_SEQNUM,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateHighSeqNum);

        initFieldUpdater (MamdaTradeFields::LOW_SEQNUM,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateLowSeqNum);

        initFieldUpdater (MamdaTradeFields::TOTAL_VOLUME_SEQNUM,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateTotalVolumeSeqNum);

        initFieldUpdater (MamdaTradeFields::CURRENCY_CODE,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateCurrencyCode);

        initFieldUpdater (MamdaTradeFields::SALE_CONDITION,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateTradeQualifierNative);

        initFieldUpdater (MamdaTradeFields::SELLERS_SALE_DAYS,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateSellersSaleDays);

        initFieldUpdater (MamdaTradeFields::IS_IRREGULAR,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateIsIrregular);

        initFieldUpdater (MamdaTradeFields::IRREG_PART_ID,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateIrregPartId);

        initFieldUpdater (MamdaTradeFields::IRREG_PRICE,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateIrregPrice);

        initFieldUpdater (MamdaTradeFields::IRREG_SIZE,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateIrregVolume);

        initFieldUpdater (MamdaTradeFields::IRREG_TIME,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateIrregTime);

        initFieldUpdater (MamdaTradeFields::ORIG_PART_ID,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateOrigPartId);

        initFieldUpdater (MamdaTradeFields::ORIG_PRICE,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateOrigPrice);

        initFieldUpdater (MamdaTradeFields::ORIG_SIZE,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateOrigVolume);

        initFieldUpdater (MamdaTradeFields::ORIG_SEQNUM,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateOrigSeqNum);

        initFieldUpdater (MamdaTradeFields::ORIG_TRADE_QUALIFIER,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateOrigQualifier);

        initFieldUpdater (MamdaTradeFields::ORIG_SALE_CONDITION,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateOrigQualifierNative);

        initFieldUpdater (MamdaTradeFields::ORIG_SELLERS_SALE_DAYS,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateOrigSaleDays);

        initFieldUpdater (MamdaTradeFields::ORIG_STOP_STOCK_IND,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateOrigStopStockInd);

        initFieldUpdater (MamdaTradeFields:: STOP_STOCK_IND,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateStopStockInd);

        initFieldUpdater (MamdaTradeFields::CORR_PART_ID,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateCorrPartId);

        initFieldUpdater (MamdaTradeFields::CORR_PRICE,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateCorrPrice);

        initFieldUpdater (MamdaTradeFields::CORR_SIZE,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateCorrVolume);

        initFieldUpdater (MamdaTradeFields::CORR_TRADE_QUALIFIER,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateCorrQualifier);

        initFieldUpdater (MamdaTradeFields::CORR_SALE_CONDITION,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateCorrQualifierNative);

        initFieldUpdater (MamdaTradeFields::CORR_SELLERS_SALE_DAYS,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateCorrSaleDays);

        initFieldUpdater (MamdaTradeFields::CORR_STOP_STOCK_IND,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateCorrStopStockInd);

        initFieldUpdater (MamdaTradeFields::CORR_TIME,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateCorrTime);

        initFieldUpdater (MamdaTradeFields::CANCEL_TIME,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateCancelTime);

        initFieldUpdater (MamdaTradeFields::CORR_TRADE_ID,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateCorrTradeId);   
        
        initFieldUpdater (MamdaTradeFields::TRADE_ID,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateTradeId);

        initFieldUpdater (MamdaTradeFields::ORIG_TRADE_ID,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateOrigTradeId);

        initFieldUpdater (MamdaTradeFields::TRADE_COUNT,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateTmpTradeCount);

        initFieldUpdater (MamdaTradeFields::BLOCK_COUNT,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateBlockCount);

        initFieldUpdater (MamdaTradeFields::BLOCK_VOLUME,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateBlockVolume);

        initFieldUpdater (MamdaTradeFields::ORDER_ID,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateOrderId);

        initFieldUpdater (MamdaTradeFields::UNIQUE_ID,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateUniqueId);

        initFieldUpdater (MamdaTradeFields::TRADE_ACTION, 
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateTradeAction);

        initFieldUpdater (MamdaTradeFields::TRADE_EXEC_VENUE,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateTradeExecVenue);

        initFieldUpdater (MamdaTradeFields::SETTLE_PRICE,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateSettlePrice);

        initFieldUpdater (MamdaTradeFields::SETTLE_DATE,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateSettleDate);

        initFieldUpdater (MamdaTradeFields::OFF_EXCHANGE_TRADE_PRICE,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateOffExchangeTradePrice);

        initFieldUpdater (MamdaTradeFields::ON_EXCHANGE_TRADE_PRICE,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateOnExchangeTradePrice);

        initFieldUpdater (MamdaTradeFields::GENERIC_FLAG,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateGenericFlag);

        initFieldUpdater (MamdaTradeFields::SHORT_SALE_CIRCUIT_BREAKER,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateShortSaleCircuitBreaker); 
               
        initFieldUpdater (MamdaTradeFields::ORIG_SHORT_SALE_CIRCUIT_BREAKER,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateOrigShortSaleCircuitBreaker);  

        initFieldUpdater (MamdaTradeFields::CORR_SHORT_SALE_CIRCUIT_BREAKER,
                          new MamdaTradeListener::
                          MamdaTradeListenerImpl::FieldUpdateCorrShortSaleCircuitBreaker);               
    }

    void MamdaTradeListener::MamdaTradeListenerImpl::initFieldUpdater (
        const MamaFieldDescriptor*  fieldDesc,
        TradeFieldUpdate*           updater)
    {
        if (!fieldDesc)
            return;

        mama_fid_t fid = fieldDesc->getFid();
        if (fid <= mFieldUpdatersSize)
        {
            mFieldUpdaters[fid] = updater;
        }
    }

    void MamdaTradeListener::MamdaTradeListenerImpl::reset()
    {
        mTradeCache = mRegularCache;
        clearCache (mRegularCache);
        if (mTransientCache)
        {
            clearCache (*mTransientCache);
        }
    }

    void MamdaTradeListener::MamdaTradeListenerImpl::assertEqual (
        MamdaTradeListenerImpl& rhs)
    {
        if (mTradeCache.mSymbol != rhs.mTradeCache.mSymbol)
        {
            char msg[256];
            snprintf (msg, 256, "different symbols (%s != %s)",
                      mTradeCache.mSymbol.c_str(),
                      rhs.mTradeCache.mSymbol.c_str());
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mPartId != rhs.mTradeCache.mPartId)
        {
            char msg[256];
            snprintf (msg, 256, "different partId (%s != %s)",
                      mTradeCache.mPartId.c_str(),
                      rhs.mTradeCache.mPartId.c_str());
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mPubId != rhs.mTradeCache.mPubId)
        {
            char msg[256];
            snprintf (msg, 256, "different pubId (%s != %s)",
                      mTradeCache.mPubId.c_str(),
                      rhs.mTradeCache.mPubId.c_str());
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mIsIrregular != rhs.mTradeCache.mIsIrregular)
        {
            char msg[256];
            snprintf (msg, 256, "different IsIrregular (%d != %d)",
                      mTradeCache.mIsIrregular,
                      rhs.mTradeCache.mIsIrregular);
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mLastPrice != rhs.mTradeCache.mLastPrice)
        {
            char msg[256];
            snprintf (msg, 256, "different lastPrice (%s != %s)",
                      mTradeCache.mLastPrice.getAsString(),
                      rhs.mTradeCache.mLastPrice.getAsString());
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mLastVolume != rhs.mTradeCache.mLastVolume)
        {
            char msg[256];
            snprintf (msg, 256, "different lastVolume (%f != %f)",
                      mTradeCache.mLastVolume,
                      rhs.mTradeCache.mLastVolume);
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mLastPartId != rhs.mTradeCache.mLastPartId)
        {
            char msg[256];
            snprintf (msg, 256, "different lastPartId (%s != %s)",
                      mTradeCache.mLastPartId.c_str(),
                      rhs.mTradeCache.mLastPartId.c_str());
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mLastTime != rhs.mTradeCache.mLastTime)
        {
            char msg[256];
            snprintf (msg, 256, "different lastTime (%s != %s)",
                      mTradeCache.mLastTime.getAsString(),
                      rhs.mTradeCache.mLastTime.getAsString());
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mTradeDate != rhs.mTradeCache.mTradeDate)
        {
            char msg[256];
            snprintf (msg, 256, "different tradeDate (%s != %s)",
                      mTradeCache.mTradeDate.getAsString(),
                      rhs.mTradeCache.mTradeDate.getAsString());
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mSide != rhs.mTradeCache.mSide)
        {       
            char msg[256];
            snprintf (msg, 256, "different Side (%s != %s)",
                     mTradeCache.mSide.c_str(),
                     rhs.mTradeCache.mSide.c_str());
            throw MamdaDataException (msg);      
        }    

        if (mTradeCache.mIrregPrice != rhs.mTradeCache.mIrregPrice)
        {
            char msg[256];
            snprintf (msg, 256, "different IrregPrice (%s != %s)",
                      mTradeCache.mIrregPrice.getAsString(),
                      rhs.mTradeCache.mIrregPrice.getAsString());
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mIrregVolume != rhs.mTradeCache.mIrregVolume)
        {
            char msg[256];
            snprintf (msg, 256, "different IrregVolume (%f != %f)",
                      mTradeCache.mIrregVolume,
                      rhs.mTradeCache.mIrregVolume);
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mIrregPartId != rhs.mTradeCache.mIrregPartId)
        {
            char msg[256];
            snprintf (msg, 256, "different IrregPartId (%s != %s)",
                      mTradeCache.mIrregPartId.c_str(),
                      rhs.mTradeCache.mIrregPartId.c_str());
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mIrregTime != rhs.mTradeCache.mIrregTime)
        {
            char msg[256];
            snprintf (msg, 256, "different IrregTime (%s != %s)",
                      mTradeCache.mIrregTime.getAsString(),
                      rhs.mTradeCache.mIrregTime.getAsString());
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mAccVolume != rhs.mTradeCache.mAccVolume)
        {
            char msg[256];
            snprintf (msg, 256, "different AccVolume (%f != %f)",
                      mTradeCache.mAccVolume,
                      rhs.mTradeCache.mAccVolume);
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mOffExAccVolume != rhs.mTradeCache.mOffExAccVolume)
        {
            char msg[256];
            snprintf (msg, 256, "different OffExAccVolume (%f != %f)",
                      mTradeCache.mOffExAccVolume,
                      rhs.mTradeCache.mOffExAccVolume);
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mOnExAccVolume != rhs.mTradeCache.mOnExAccVolume)
        {
            char msg[256];
            snprintf (msg, 256, "different OnExAccVolume (%f != %f)",
                      mTradeCache.mOnExAccVolume,
                      rhs.mTradeCache.mOnExAccVolume);
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mNetChange != rhs.mTradeCache.mNetChange)
        {
            char msg[256];
            snprintf (msg, 256, "different NetChange (%s != %s)",
                      mTradeCache.mNetChange.getAsString(),
                      rhs.mTradeCache.mNetChange.getAsString());
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mPctChange != rhs.mTradeCache.mPctChange)
        {
            char msg[256];
            snprintf (msg, 256, "different PctChange (%f != %f)",
                      mTradeCache.mPctChange,
                      rhs.mTradeCache.mPctChange);
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mOpenPrice != rhs.mTradeCache.mOpenPrice)
        {
            char msg[256];
            snprintf (msg, 256, "different OpenPrice (%s != %s)",
                      mTradeCache.mOpenPrice.getAsString(),
                      rhs.mTradeCache.mOpenPrice.getAsString());
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mHighPrice != rhs.mTradeCache.mHighPrice)
        {
            char msg[256];
            snprintf (msg, 256, "different HighPrice (%s != %s)",
                      mTradeCache.mHighPrice.getAsString(),
                      rhs.mTradeCache.mHighPrice.getAsString());
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mLowPrice != rhs.mTradeCache.mLowPrice)
        {
            char msg[256];
            snprintf (msg, 256, "different LowPrice (%s != %s)",
                      mTradeCache.mLowPrice.getAsString(),
                      rhs.mTradeCache.mLowPrice.getAsString());
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mClosePrice != rhs.mTradeCache.mClosePrice)
        {
            char msg[256];
            snprintf (msg, 256, "different ClosePrice (%s != %s)",
                      mTradeCache.mClosePrice.getAsString(),
                      rhs.mTradeCache.mClosePrice.getAsString());
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mPrevClosePrice != rhs.mTradeCache.mPrevClosePrice)
        {
            char msg[256];
            snprintf (msg, 256, "different PrevClosePrice (%s != %s)",
                      mTradeCache.mPrevClosePrice.getAsString(),
                      rhs.mTradeCache.mPrevClosePrice.getAsString());
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mPrevCloseDate != rhs.mTradeCache.mPrevCloseDate)
        {
            char msg[256];
            snprintf (msg, 256, "different PrevCloseDate (%s != %s)",
                      mTradeCache.mPrevCloseDate.getAsString(),
                      rhs.mTradeCache.mPrevCloseDate.getAsString());
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mAdjPrevClose != rhs.mTradeCache.mAdjPrevClose)
        {
            char msg[256];
            snprintf (msg, 256, "different AdjPrevClose (%s != %s)",
                      mTradeCache.mAdjPrevClose.getAsString(),
                      rhs.mTradeCache.mAdjPrevClose.getAsString());
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mTradeCount != rhs.mTradeCache.mTradeCount)
        {
            char msg[256];
            snprintf (msg, 256, "different TradeCount (%d != %d)",
                      mTradeCache.mTradeCount,
                      rhs.mTradeCache.mTradeCount);
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mBlockVolume != rhs.mTradeCache.mBlockVolume)
        {
            char msg[256];
            snprintf (msg, 256, "different BlockVolume (%f != %f)",
                      mTradeCache.mBlockVolume,
                      rhs.mTradeCache.mBlockVolume);
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mBlockCount != rhs.mTradeCache.mBlockCount)
        {
            char msg[256];
            snprintf (msg, 256, "different BlockCount (%d != %d)",
                      mTradeCache.mBlockCount,
                      rhs.mTradeCache.mBlockCount);
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mVwap != rhs.mTradeCache.mVwap)
        {
            char msg[256];
            snprintf (msg, 256, "different Vwap (%f != %f)",
                      mTradeCache.mVwap,
                      rhs.mTradeCache.mVwap);
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mOffExVwap != rhs.mTradeCache.mOffExVwap)
        {
            char msg[256];
            snprintf (msg, 256, "different OffExVwap (%f != %f)",
                      mTradeCache.mOffExVwap,
                      rhs.mTradeCache.mOffExVwap);
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mOnExVwap != rhs.mTradeCache.mOnExVwap)
        {
            char msg[256];
            snprintf (msg, 256, "different OnExVwap (%f != %f)",
                      mTradeCache.mOnExVwap,
                      rhs.mTradeCache.mOnExVwap);
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mTotalValue != rhs.mTradeCache.mTotalValue)
        {
            char msg[256];
            snprintf (msg, 256, "different TotalValue (%f != %f)",
                      mTradeCache.mTotalValue,
                      rhs.mTradeCache.mTotalValue);
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mOffExTotalValue != rhs.mTradeCache.mOffExTotalValue)
        {
            char msg[256];
            snprintf (msg, 256, "different OffExTotalValue (%f != %f)",
                      mTradeCache.mOffExTotalValue,
                      rhs.mTradeCache.mOffExTotalValue);
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mOnExTotalValue != rhs.mTradeCache.mOnExTotalValue)
        {
            char msg[256];
            snprintf (msg, 256, "different OnExTotalValue (%f != %f)",
                      mTradeCache.mOnExTotalValue,
                      rhs.mTradeCache.mOnExTotalValue);
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mStdDev != rhs.mTradeCache.mStdDev)
        {
            char msg[256];
            snprintf (msg, 256, "different StdDev (%f != %f)",
                      mTradeCache.mStdDev,
                      rhs.mTradeCache.mStdDev);
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mStdDevSum != rhs.mTradeCache.mStdDevSum)
        {
            char msg[256];
            snprintf (msg, 256, "different StdDevSum (%f != %f)",
                      mTradeCache.mStdDevSum,
                      rhs.mTradeCache.mStdDevSum);
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mStdDevSumSquares != rhs.mTradeCache.mStdDevSumSquares)
        {
            char msg[256];
            snprintf (msg, 256, "different StdDevSumSquares (%f != %f)",
                      mTradeCache.mStdDevSumSquares,
                      rhs.mTradeCache.mStdDevSumSquares);
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mTradeUnits != rhs.mTradeCache.mTradeUnits)
        {
            char msg[256];
            snprintf (msg, 256, "different TradeUnits (%s != %s)",
                      mTradeCache.mTradeUnits.c_str(),
                      rhs.mTradeCache.mTradeUnits.c_str());
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mLastSeqNum != rhs.mTradeCache.mLastSeqNum)
        {
            char msg[256];
            snprintf (msg, 256, "different LastSeqNum (%d != %d)",
                      mTradeCache.mLastSeqNum,
                      rhs.mTradeCache.mLastSeqNum);
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mHighSeqNum != rhs.mTradeCache.mHighSeqNum)
        {
            char msg[256];
            snprintf (msg, 256, "different HighSeqNum (%d != %d)",
                      mTradeCache.mHighSeqNum,
                      rhs.mTradeCache.mHighSeqNum);
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mLowSeqNum != rhs.mTradeCache.mLowSeqNum)
        {
            char msg[256];
            snprintf (msg, 256, "different LowSeqNum (%d != %d)",
                      mTradeCache.mHighSeqNum,
                      rhs.mTradeCache.mHighSeqNum);
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mTotalVolumeSeqNum != rhs.mTradeCache.mTotalVolumeSeqNum)
        {
            char msg[256];
            snprintf (msg, 256, "different TotalVolumeSeqNum (%d != %d)",
                      mTradeCache.mTotalVolumeSeqNum,
                      rhs.mTradeCache.mTotalVolumeSeqNum);
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mCurrencyCode != rhs.mTradeCache.mCurrencyCode)
        {
            char msg[256];
            snprintf (msg, 256, "different CurrencyCode (%s != %s)",
                      mTradeCache.mCurrencyCode.c_str(),
                      rhs.mTradeCache.mCurrencyCode.c_str());
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mSettlePrice != rhs.mTradeCache.mSettlePrice)
        {
            char msg[256];
            snprintf (msg, 256, "different SettlePrice (%s != %s)",
                      mTradeCache.mSettlePrice.getAsString(),
                      rhs.mTradeCache.mSettlePrice.getAsString());
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mSettleDate != rhs.mTradeCache.mSettleDate)
        {
            char msg[256];
            snprintf (msg, 256, "different SettleDate (%s != %s)",
                      mTradeCache.mSettleDate.getAsString(),
                      rhs.mTradeCache.mSettleDate.getAsString());
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mOrderId != rhs.mTradeCache.mOrderId)
        {
            char msg[256];
            snprintf (msg, 256, "different OrderId (%" PRId64 "!= %" PRId64 ")",
                      mTradeCache.mOrderId,
                      rhs.mTradeCache.mOrderId);
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mUniqueId != rhs.mTradeCache.mUniqueId)
        {
            char msg[256];
            snprintf (msg, 256, "different UniqueId {%s != %s)",
                      mTradeCache.mUniqueId.c_str(),
                      rhs.mTradeCache.mUniqueId.c_str());
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mTradeId != rhs.mTradeCache.mTradeId)
        {
            char msg[256];
            snprintf (msg, 256, "different mTradeId {%s != %s)",
                      mTradeCache.mTradeId.c_str(),
                      rhs.mTradeCache.mTradeId.c_str());
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mOrigTradeId != rhs.mTradeCache.mOrigTradeId)
        {
            char msg[256];
            snprintf (msg, 256, "different mTradeId {%s != %s)",
                      mTradeCache.mOrigTradeId.c_str(),
                      rhs.mTradeCache.mOrigTradeId.c_str());
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mCorrTradeId != rhs.mTradeCache.mCorrTradeId)
        {
            char msg[256];
            snprintf (msg, 256, "different mTradeId {%s != %s)",
                      mTradeCache.mCorrTradeId.c_str(),
                      rhs.mTradeCache.mCorrTradeId.c_str());
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mTradeAction != rhs.mTradeCache.mTradeAction)
        {
            char msg[256];
            snprintf (msg, 256, "different TradeAction (%s != %s)",
                      mTradeCache.mTradeAction.c_str(),
                      rhs.mTradeCache.mTradeAction.c_str());
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mTradeExecVenue != rhs.mTradeCache.mTradeExecVenue)
        {   
            char msg[256];
            snprintf (msg, 256, "different TradeExecVenue (%s != %s)",
                      mTradeCache.mTradeExecVenue,
                      rhs.mTradeCache.mTradeExecVenue);
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mTradePrice != rhs.mTradeCache.mTradePrice)
        {
            char msg[256];
            snprintf (msg, 256, "different TradePrice (%s != %s)",
                      mTradeCache.mTradePrice.getAsString(),
                      rhs.mTradeCache.mTradePrice.getAsString());
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mTradeVolume != rhs.mTradeCache.mTradeVolume)
        {
            char msg[256];
            snprintf (msg, 256, "different TradeVolume (%f != %f)",
                      mTradeCache.mTradeVolume,
                      rhs.mTradeCache.mTradeVolume);
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mTradePartId != rhs.mTradeCache.mTradePartId)
        {
            char msg[256];
            snprintf (msg, 256, "different TradePartId (%s != %s)",
                      mTradeCache.mTradePartId.c_str(),
                      rhs.mTradeCache.mTradePartId.c_str());
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mTradeQualStr != rhs.mTradeCache.mTradeQualStr)
        {
            char msg[256];
            snprintf (msg, 256, "different TradeQualStr (%s != %s)",
                      mTradeCache.mTradeQualStr.c_str(),
                      rhs.mTradeCache.mTradeQualStr.c_str());
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mTradeQualNativeStr != rhs.mTradeCache.mTradeQualNativeStr)
        {
            char msg[256];
            snprintf (msg, 256, "different TradeQualNativeStr (%s != %s)",
                      mTradeCache.mTradeQualNativeStr.c_str(),
                      rhs.mTradeCache.mTradeQualNativeStr.c_str());
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mSellersSaleDays != rhs.mTradeCache.mSellersSaleDays)
        {
            char msg[256];
            snprintf (msg, 256, "different SellersSaleDays (%d != %d)",
                      mTradeCache.mSellersSaleDays,
                      rhs.mTradeCache.mSellersSaleDays);
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mTmpTradeCount != rhs.mTradeCache.mTmpTradeCount)
        {
            char msg[256];
            snprintf (msg, 256, "different TmpTradeDays (%d != %d)",
                      mTradeCache.mTmpTradeCount,
                      rhs.mTradeCache.mTmpTradeCount);
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mStopStockInd != rhs.mTradeCache.mStopStockInd)
        {
            char msg[256];
            snprintf (msg, 256, "different StopStockInt (%c != %c)",
                      mTradeCache.mStopStockInd,
                      rhs.mTradeCache.mStopStockInd);
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mOrigPrice != rhs.mTradeCache.mOrigPrice)
        {
            char msg[256];
            snprintf (msg, 256, "different OrigPrice (%s != %s)",
                      mTradeCache.mOrigPrice.getAsString(),
                      rhs.mTradeCache.mOrigPrice.getAsString());
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mOrigVolume != rhs.mTradeCache.mOrigVolume)
        {
            char msg[256];
            snprintf (msg, 256, "different OrigVolume (%f != %f)",
                      mTradeCache.mOrigVolume,
                      rhs.mTradeCache.mOrigVolume);
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mOrigPartId != rhs.mTradeCache.mOrigPartId)
        {
            char msg[256];
            snprintf (msg, 256, "different OrigPartId (%s != %s)",
                      mTradeCache.mOrigPartId.c_str(),
                      rhs.mTradeCache.mOrigPartId.c_str());
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mOrigQualStr != rhs.mTradeCache.mOrigQualStr)
        {
            char msg[256];
            snprintf (msg, 256, "different OrigQualStr (%s != %s)",
                      mTradeCache.mOrigQualStr.c_str(),
                      rhs.mTradeCache.mOrigQualStr.c_str());
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mOrigQualNativeStr != rhs.mTradeCache.mOrigQualNativeStr)
        {
            char msg[256];
            snprintf (msg, 256, "different OrigQualNativeStr (%s != %s)",
                      mTradeCache.mOrigQualNativeStr.c_str(),
                      rhs.mTradeCache.mOrigQualNativeStr.c_str());
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mOrigSellersSaleDays != rhs.mTradeCache.mOrigSellersSaleDays)
        {
            char msg[256];
            snprintf (msg, 256, "different OrigSellersSaleDays (%d != %d)",
                      mTradeCache.mOrigSellersSaleDays,
                      rhs.mTradeCache.mOrigSellersSaleDays);
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mOrigStopStockInd != rhs.mTradeCache.mOrigStopStockInd)
        {
            char msg[256];
            snprintf (msg, 256, "different OrigStopStockInd (%c != %c)",
                      mTradeCache.mOrigStopStockInd,
                      rhs.mTradeCache.mOrigStopStockInd);
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mCorrPrice != rhs.mTradeCache.mCorrPrice)
        {
            char msg[256];
            snprintf (msg, 256, "different CorrPrice (%s != %s)",
                      mTradeCache.mCorrPrice.getAsString(),
                      rhs.mTradeCache.mCorrPrice.getAsString());
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mCorrVolume != rhs.mTradeCache.mCorrVolume)
        {
            char msg[256];
            snprintf (msg, 256, "different CorrVolume (%f != %f)",
                      mTradeCache.mCorrVolume,
                      rhs.mTradeCache.mCorrVolume);
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mCorrPartId != rhs.mTradeCache.mCorrPartId)
        {
            char msg[256];
            snprintf (msg, 256, "different CorrPartId (%s != %s)",
                      mTradeCache.mCorrPartId.c_str(),
                      rhs.mTradeCache.mCorrPartId.c_str());
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mCorrQualStr != rhs.mTradeCache.mCorrQualStr)
        {
            char msg[256];
            snprintf (msg, 256, "different CorrQualStr (%s != %s)",
                      mTradeCache.mCorrQualStr.c_str(),
                      rhs.mTradeCache.mCorrQualStr.c_str());
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mCorrQualNativeStr != rhs.mTradeCache.mCorrQualNativeStr)
        {
            char msg[256];
            snprintf (msg, 256, "different CorrQualStr (%s != %s)",
                      mTradeCache.mCorrQualNativeStr.c_str(),
                      rhs.mTradeCache.mCorrQualNativeStr.c_str());
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mCorrSellersSaleDays != rhs.mTradeCache.mCorrSellersSaleDays)
        {
            char msg[256];
            snprintf (msg, 256, "different CorrSellersSaleDays (%d != %d)",
                      mTradeCache.mCorrSellersSaleDays,
                      rhs.mTradeCache.mCorrSellersSaleDays);
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mCorrStopStockInd != rhs.mTradeCache.mCorrStopStockInd)
        {
            char msg[256];
            snprintf (msg, 256, "different CorrStopStockInt (%c != %c)",
                      mTradeCache.mCorrStopStockInd,
                      rhs.mTradeCache.mCorrStopStockInd);
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mCancelTime != rhs.mTradeCache.mCancelTime)
        {
            char msg[256];
            snprintf (msg, 256, "different CancelTime (%s != %s)",
                      mTradeCache.mCancelTime.getAsString(),
                      rhs.mTradeCache.mCancelTime.getAsString());
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mOffExchangeTradePrice != rhs.mTradeCache.mOffExchangeTradePrice)
        {
            char msg[256];
            snprintf (msg, 256, "different OffExchangeTradePrice (%s != %s)",
                      mTradeCache.mOffExchangeTradePrice.getAsString(),
                      rhs.mTradeCache.mOffExchangeTradePrice.getAsString());
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mOnExchangeTradePrice != rhs.mTradeCache.mOnExchangeTradePrice)
        {
            char msg[256];
            snprintf (msg, 256, "different OnExchangeTradePrice (%s != %s)",
                      mTradeCache.mOnExchangeTradePrice.getAsString(),
                      rhs.mTradeCache.mOnExchangeTradePrice.getAsString());
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mTradeId != rhs.mTradeCache.mTradeId)
        {
            char msg[256];
            snprintf (msg, 256, "different TradeId (%s != %s)",
                      mTradeCache.mTradeId.c_str(),
                      rhs.mTradeCache.mTradeId.c_str());
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mOrigTradeId != rhs.mTradeCache.mOrigTradeId)
        {
            char msg[256];
            snprintf (msg, 256, "different OrigTradeId (%s != %s)",
                      mTradeCache.mOrigTradeId.c_str(),
                      rhs.mTradeCache.mOrigTradeId.c_str());
            throw MamdaDataException (msg);
        }

        if (mTradeCache.mGenericFlag != rhs.mTradeCache.mGenericFlag)
        {
            char msg[256];
            snprintf (msg, 256, "different GenericFlag (%d != %d)",
                      mTradeCache.mGenericFlag,
                      rhs.mTradeCache.mGenericFlag);
            throw MamdaDataException (msg);
        }
    }

} // namespace
