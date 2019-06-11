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

#include <stdint.h>
#include <wombat/port.h>
#include <mamda/MamdaQuoteListener.h>
#include <mamda/MamdaQuoteFields.h>
#include <mamda/MamdaQuoteHandler.h>
#include <mamda/MamdaCommonFields.h>
#include <mamda/MamdaSubscription.h>
#include <mamda/MamdaDataException.h>
#include <mamda/MamdaFieldState.h>
#include <mama/mamacpp.h>
#include "MamdaUtils.h"
#include <string>

using std::string;

namespace Wombat
{

    struct QuoteFieldUpdate
    {
        virtual void onUpdate (
            MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
            const MamaMsgField&                          field) = 0;
    };

    struct QuoteListenerCache
    {
        // The following fields are used for caching the offical quote and
        // related fields.  These fields can be used by applications for
        // reference and will be passed for recaps.
        string           mSymbol;                     MamdaFieldState   mSymbolFieldState;   
        string           mPartId;                     MamdaFieldState   mPartIdFieldState;   
        MamaDateTime     mSrcTime;                    MamdaFieldState   mSrcTimeFieldState;  
        MamaDateTime     mActTime;                    MamdaFieldState   mActTimeFieldState;  
        MamaDateTime     mLineTime;                   MamdaFieldState   mLineTimeFieldState; 
        MamaDateTime     mSendTime;                   MamdaFieldState   mSendTimeFieldState; 
        string           mPubId;                      MamdaFieldState   mPubIdFieldState;
        MamaPrice        mBidPrice;                   MamdaFieldState   mBidPriceFieldState; 
        mama_quantity_t  mBidSize;                    MamdaFieldState   mBidSizeFieldState;
        mama_quantity_t  mBidDepth;                   MamdaFieldState   mBidDepthFieldState;
        string           mBidPartId;                  MamdaFieldState   mBidPartIdFieldState;
        MamaPrice        mBidClosePrice;              MamdaFieldState   mBidClosePriceFieldState; 
        MamaDateTime     mBidCloseDate;               MamdaFieldState   mBidCloseDateFieldState;

        MamaPrice        mBidPrevClosePrice;          MamdaFieldState   mBidPrevClosePriceFieldState;
        MamaDateTime     mBidPrevCloseDate;           MamdaFieldState   mBidPrevCloseDateFieldState; 
        MamaPrice        mBidHigh;                    MamdaFieldState   mBidHighFieldState;          
        MamaPrice        mBidLow;                     MamdaFieldState   mBidLowFieldState;           
        MamaPrice        mAskPrice;                   MamdaFieldState   mAskPriceFieldState;         
        mama_quantity_t  mAskSize;                    MamdaFieldState   mAskSizeFieldState;          
        mama_quantity_t  mAskDepth;                   MamdaFieldState   mAskDepthFieldState;         
        string           mAskPartId;                  MamdaFieldState   mAskPartIdFieldState;        
        MamaPrice        mAskClosePrice;              MamdaFieldState   mAskClosePriceFieldState;     
        MamaDateTime     mAskCloseDate;               MamdaFieldState   mAskCloseDateFieldState;     
        MamaPrice        mAskPrevClosePrice;          MamdaFieldState   mAskPrevClosePriceFieldState;
        MamaDateTime     mAskPrevCloseDate;           MamdaFieldState   mAskPrevCloseDateFieldState; 
        MamaPrice        mAskHigh;                    MamdaFieldState   mAskHighFieldState;          
        MamaPrice        mAskLow;                     MamdaFieldState   mAskLowFieldState;           
        MamaPrice        mMidPrice;                   MamdaFieldState   mMidPriceFieldState;         
        mama_seqnum_t    mEventSeqNum;                MamdaFieldState   mEventSeqNumFieldState;      
        MamaDateTime     mEventTime;                  MamdaFieldState   mEventTimeFieldState;        
        MamaDateTime     mQuoteDate;                  MamdaFieldState   mQuoteDateFieldState;        
        string           mQuoteQualStr;               MamdaFieldState   mQuoteQualStrFieldState;     
        string           mQuoteQualNative;            MamdaFieldState   mQuoteQualNativeFieldState;  

        MamaDateTime     mAskTime;                    MamdaFieldState   mAskTimeFieldState;          
        MamaDateTime     mBidTime;                    MamdaFieldState   mBidTimeFieldState;          
        string           mAskIndicator;               MamdaFieldState   mAskIndicatorFieldState;     
        string           mBidIndicator;               MamdaFieldState   mBidIndicatorFieldState;     
        mama_u32_t       mAskUpdateCount;             MamdaFieldState   mAskUpdateCountFieldState;   
        mama_u32_t       mBidUpdateCount;             MamdaFieldState   mBidUpdateCountFieldState;   
        double           mAskYield;                   MamdaFieldState   mAskYieldFieldState;         
        double           mBidYield;                   MamdaFieldState   mBidYieldFieldState;         

        mama_u32_t       mTmpQuoteCount;              MamdaFieldState   mTmpQuoteCountFieldState;    
        mama_u32_t       mQuoteCount;                 MamdaFieldState   mQuoteCountFieldState;
        mama_seqnum_t    mGapBegin;                   MamdaFieldState   mGapBeginFieldState;         
        mama_seqnum_t    mGapEnd;                     MamdaFieldState   mGapEndFieldState;           
        string           mBidSizesList;               MamdaFieldState   mBidSizesListFieldState;
        string           mAskSizesList;               MamdaFieldState   mAskSizesListFieldState;
        char             mShortSaleCircuitBreaker;    MamdaFieldState   mShortSaleCircuitBreakerFieldState;

        bool             mLastGenericMsgWasQuote;
        bool             mGotBidPrice;
        bool             mGotAskPrice;
        bool             mGotBidSize;
        bool             mGotAskSize;
        bool             mGotBidDepth;
        bool             mGotAskDepth;
        bool             mGotBidPartId;
        bool             mGotAskPartId;   
        bool             mGotQuoteCount;
        char             mShortSaleBidTick;           MamdaFieldState   mShortSaleBidTickFieldState;
    };


    class MamdaQuoteListener::MamdaQuoteListenerImpl 
        : public MamaMsgFieldIterator
    {
    public:
        MamdaQuoteListenerImpl  (MamdaQuoteListener& listener);
        ~MamdaQuoteListenerImpl () {}

        void clearCache                   (QuoteListenerCache& quoteCache);

        void handleQuoteMessage           (MamdaSubscription*  subscription,
                                           const MamaMsg&      msg,
                                           short               msgType);

        bool evaluateMsgQual              (MamdaSubscription*  subscription,
                                           const MamaMsg&      msg);

        void handleRecap                  (MamdaSubscription*  subscription,
                                           const MamaMsg&      msg);

        void handleQuote                  (MamdaSubscription*  subscription,
                                           const MamaMsg&      msg);

        void handleUpdate                 (MamdaSubscription*  subscription,
                                           const MamaMsg&      msg);

        void invokeTransientHandler       (MamdaSubscription*  subscription,
                                           const MamaMsg&      msg);

        void onField                      (const MamaMsg&      msg,
                                           const MamaMsgField& field,
                                           void*               closure);

        void updateQuoteFields            (const MamaMsg&      msg);

        void updateFieldStates            ();

        void checkQuoteCount              (MamdaSubscription*  subscription,
                                           const MamaMsg&      msg,
                                           bool                checkForGap);
        
        void assertEqual   (MamdaQuoteListenerImpl&  rhs);

        typedef struct QuoteUpdateLock_
        {
            wthread_mutex_t  mQuoteUpdateLockMutex;
        } QuoteUpdateLock;

        // Thread locking    
        QuoteUpdateLock mQuoteUpdateLock;

        MamdaQuoteListener& mListener;
        MamdaQuoteHandler*  mHandler;

        // Message Qualifier - holds information  
        // regarding duplicate and delayed status of message.
        MamaMsgQual    mMsgQual;       MamdaFieldState mMsgQualFieldState;

        // If message is "Transient", i.e., possibly duplicate or delayed
        // and this flag is set then mTransientCache is used to store msg
        // contents - data kept separate from regular updates and does not
        // persist during to next update.
        bool           mProcessPosDupAndOutOfSeqAsTransient;

        // If mResolvePossiblyDuplicate is set then the listener will attempt, 
        // based upon sequence number and event times, to determine whether
        // a quote marked as "possibly duplicate" is definately a duplicate,
        // in which case it is dropped.
        bool           mResolvePossiblyDuplicate;

        // The mUsePosDupAndOutOfSeqHandlers is used to determine whether or not the 
        // duplicate and out of sequence handlers should be invoked for a message
        bool           mUsePosDupAndOutOfSeqHandlers;

        // Used to record whether or not a message has been 
        // determined to be transient.
        bool           mIsTransientMsg;

        // The Quote Listener Data Caches
        QuoteListenerCache     mRegularCache;     // Regular update cache
        QuoteListenerCache*    mTransientCache;   // Transient cache
        QuoteListenerCache&    mQuoteListenerCache;       // Current cache in use
        bool           mIgnoreUpdate;

        static void initFieldUpdaters ();
        static void initFieldUpdater (
            const MamaFieldDescriptor*  fieldDesc,
            QuoteFieldUpdate*           updater);

        static void updateField (
            MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
            const MamaMsgField&                          field);

        static QuoteFieldUpdate**  mFieldUpdaters;
        static volatile uint16_t   mFieldUpdatersSize;
        static wthread_static_mutex_t     mQuoteFieldUpdaterLockMutex;
        static bool                mUpdatersComplete;

        struct FieldUpdateQuoteSrcTime;
        struct FieldUpdateQuoteActTime;
        struct FieldUpdateQuoteLineTime;
        struct FieldUpdateQuotePubId;
        struct FieldUpdateQuoteSendTime;
        struct FieldUpdateBidPrice;
        struct FieldUpdateBidSize;
        struct FieldUpdateBidDepth;
        struct FieldUpdateBidPartId;
        struct FieldUpdateBidClosePrice;
        struct FieldUpdateBidCloseDate;
        struct FieldUpdateBidPrevClosePrice;
        struct FieldUpdateBidPrevCloseDate;
        struct FieldUpdateBidHigh;
        struct FieldUpdateBidLow;
        struct FieldUpdateAskPrice;
        struct FieldUpdateAskSize;
        struct FieldUpdateAskDepth;
        struct FieldUpdateAskPartId;
        struct FieldUpdateAskClosePrice;
        struct FieldUpdateAskCloseDate;
        struct FieldUpdateAskPrevClosePrice;
        struct FieldUpdateAskPrevCloseDate;
        struct FieldUpdateAskHigh;
        struct FieldUpdateAskLow;
        struct FieldUpdateQuoteSeqNum;
        struct FieldUpdateQuoteTime;
        struct FieldUpdateQuoteDate;
        struct FieldUpdateTmpQuoteCount;
        struct FieldUpdateQuoteQual;
        struct FieldUpdateQuoteQualNative;

        struct FieldUpdateAskTime;
        struct FieldUpdateBidTime;
        struct FieldUpdateBidIndicator;
        struct FieldUpdateAskIndicator;
        struct FieldUpdateBidUpdateCount;
        struct FieldUpdateAskUpdateCount;
        struct FieldUpdateBidYield;
        struct FieldUpdateAskYield;
        struct FieldUpdateAskSizesList;
        struct FieldUpdateBidSizesList;

        struct FieldUpdateShortSaleBidTick;
        struct FieldUpdateBidTick;
        struct FieldUpdateShortSaleCircuitBreaker;
    };

    MamdaQuoteListener::MamdaQuoteListener ()
        : mImpl (*new MamdaQuoteListenerImpl(*this))
    {     
        wthread_mutex_init (&mImpl.mQuoteUpdateLock.mQuoteUpdateLockMutex, NULL);
    }

    MamdaQuoteListener::~MamdaQuoteListener ()
    {
        wthread_mutex_destroy (&mImpl.mQuoteUpdateLock.mQuoteUpdateLockMutex);
	    /* Do not call wthread_mutex_destroy for the FieldUpdaterLockMutex here.  
	       If we do, it will not be initialized again if another listener is created 
	       after the first is destroyed. */
        /* wthread_mutex_destroy (&mImpl.mQuoteFieldUpdaterLockMutex); */
        delete &mImpl;
    }

    void MamdaQuoteListener::addHandler (MamdaQuoteHandler*  handler)
    {
        mImpl.mHandler = handler;
    }

    void MamdaQuoteListener::processPosDupAndOutOfSeqAsTransient (bool tf)
    {
        mImpl.mProcessPosDupAndOutOfSeqAsTransient = tf;
    }

    void MamdaQuoteListener::resolvePossiblyDuplicate (bool tf)
    {
        mImpl.mResolvePossiblyDuplicate = tf;
    }

    void MamdaQuoteListener::usePosDupAndOutOfSeqHandlers (bool tf)
    {
        mImpl.mUsePosDupAndOutOfSeqHandlers = tf;
    }

    const char* MamdaQuoteListener::getSymbol() const
    {
        return mImpl.mQuoteListenerCache.mSymbol.c_str();
    }

    const char* MamdaQuoteListener::getPartId() const
    {
        return mImpl.mQuoteListenerCache.mPartId.c_str();
    }

    const MamaDateTime& MamdaQuoteListener::getSrcTime() const
    {
        return mImpl.mQuoteListenerCache.mSrcTime;
    }

    const MamaDateTime& MamdaQuoteListener::getActivityTime() const
    {
        return mImpl.mQuoteListenerCache.mActTime;
    }

    const MamaDateTime& MamdaQuoteListener::getLineTime() const
    {
        return mImpl.mQuoteListenerCache.mLineTime;
    }

    const MamaDateTime& MamdaQuoteListener::getSendTime() const
    {
        return mImpl.mQuoteListenerCache.mSendTime;
    }

    const MamaMsgQual& MamdaQuoteListener::getMsgQual() const
    {
        return mImpl.mMsgQual;
    }

    const char* MamdaQuoteListener::getPubId() const
    {
        return mImpl.mQuoteListenerCache.mPubId.c_str();
    }

    const MamaPrice& MamdaQuoteListener::getBidPrice() const
    {
        return mImpl.mQuoteListenerCache.mBidPrice;
    }

    mama_quantity_t MamdaQuoteListener::getBidSize() const
    {
        return mImpl.mQuoteListenerCache.mBidSize;
    }

    mama_quantity_t MamdaQuoteListener::getBidDepth() const
    {
        return mImpl.mQuoteListenerCache.mBidDepth;
    }

    const char* MamdaQuoteListener::getBidPartId() const
    {
        return mImpl.mQuoteListenerCache.mBidPartId.c_str();
    }

    const MamaPrice& MamdaQuoteListener::getBidClosePrice() const
    {
        return mImpl.mQuoteListenerCache.mBidClosePrice;
    }

    const MamaDateTime& MamdaQuoteListener::getBidCloseDate() const
    {
        return mImpl.mQuoteListenerCache.mBidCloseDate;
    }

    const MamaPrice& MamdaQuoteListener::getBidPrevClosePrice() const
    {
        return mImpl.mQuoteListenerCache.mBidPrevClosePrice;
    }

    const MamaDateTime& MamdaQuoteListener::getBidPrevCloseDate() const
    {
        return mImpl.mQuoteListenerCache.mBidPrevCloseDate;
    }

    const MamaPrice& MamdaQuoteListener::getBidHigh() const
    {
        return mImpl.mQuoteListenerCache.mBidHigh;
    }

    const MamaPrice& MamdaQuoteListener::getBidLow() const
    {
        return mImpl.mQuoteListenerCache.mBidLow;
    }

    const MamaPrice& MamdaQuoteListener::getAskPrice() const
    {
        return mImpl.mQuoteListenerCache.mAskPrice;
    }

    mama_quantity_t MamdaQuoteListener::getAskSize() const
    {
        return mImpl.mQuoteListenerCache.mAskSize;
    }

    mama_quantity_t MamdaQuoteListener::getAskDepth() const
    {
        return mImpl.mQuoteListenerCache.mAskDepth;
    }

    const char* MamdaQuoteListener::getAskPartId() const
    {
        return mImpl.mQuoteListenerCache.mAskPartId.c_str();
    }

    const MamaPrice& MamdaQuoteListener::getAskClosePrice() const
    {
        return mImpl.mQuoteListenerCache.mAskClosePrice;
    }

    const MamaDateTime& MamdaQuoteListener::getAskCloseDate() const
    {
        return mImpl.mQuoteListenerCache.mAskCloseDate;
    }

    const MamaPrice& MamdaQuoteListener::getAskPrevClosePrice() const
    {
        return mImpl.mQuoteListenerCache.mAskPrevClosePrice;
    }

    const MamaDateTime& MamdaQuoteListener::getAskPrevCloseDate() const
    {
        return mImpl.mQuoteListenerCache.mAskPrevCloseDate;
    }

    const MamaPrice& MamdaQuoteListener::getAskHigh() const
    {
        return mImpl.mQuoteListenerCache.mAskHigh;
    }

    const MamaPrice& MamdaQuoteListener::getAskLow() const
    {
        return mImpl.mQuoteListenerCache.mAskLow;
    }

    const MamaPrice& MamdaQuoteListener::getQuoteMidPrice() const
    {
        if ((mImpl.mQuoteListenerCache.mBidPrice > 0.0) && 
            (mImpl.mQuoteListenerCache.mAskPrice > 0.0))
        {
            mImpl.mQuoteListenerCache.mMidPrice.set (
                (mImpl.mQuoteListenerCache.mAskPrice.getValue() +
                 mImpl.mQuoteListenerCache.mBidPrice.getValue()) / 2.0);
        }
        else
        {
            mImpl.mQuoteListenerCache.mBidPrice.clear();
        }
        return mImpl.mQuoteListenerCache.mMidPrice;
    }

    mama_u32_t MamdaQuoteListener::getQuoteCount() const
    {
        return mImpl.mQuoteListenerCache.mQuoteCount;
    }

    const char* MamdaQuoteListener::getQuoteQualStr() const
    {
        return mImpl.mQuoteListenerCache.mQuoteQualStr.c_str();
    }

    const char* MamdaQuoteListener::getQuoteQualNative() const
    {
        return mImpl.mQuoteListenerCache.mQuoteQualNative.c_str();
    }

    const MamaDateTime&   MamdaQuoteListener::getAskTime()           const
    {
        return mImpl.mQuoteListenerCache.mAskTime;
    }

    const MamaDateTime&   MamdaQuoteListener::getBidTime()           const
    {
        return mImpl.mQuoteListenerCache.mBidTime;
    }

    const char*  MamdaQuoteListener::getAskIndicator()      const
    {
        return mImpl.mQuoteListenerCache.mAskIndicator.c_str();
    }

    const char*  MamdaQuoteListener::getBidIndicator()      const
    {
        return mImpl.mQuoteListenerCache.mBidIndicator.c_str();
    }

    mama_u32_t  MamdaQuoteListener::getAskUpdateCount()    const
    {
        return mImpl.mQuoteListenerCache.mAskUpdateCount;
    }

    mama_u32_t  MamdaQuoteListener::getBidUpdateCount()    const
    {
        return mImpl.mQuoteListenerCache.mBidUpdateCount;
    }

    double  MamdaQuoteListener::getAskYield()          const
    {
        return mImpl.mQuoteListenerCache.mAskYield;
    }

    double  MamdaQuoteListener::getBidYield()          const
    {
        return mImpl.mQuoteListenerCache.mBidYield;
    }

    mama_seqnum_t MamdaQuoteListener::getEventSeqNum() const
    {
        return mImpl.mQuoteListenerCache.mEventSeqNum;
    }

    const MamaDateTime& MamdaQuoteListener::getEventTime() const
    {
        return mImpl.mQuoteListenerCache.mEventTime;
    }

    const MamaDateTime& MamdaQuoteListener::getQuoteDate() const
    {
        return mImpl.mQuoteListenerCache.mQuoteDate;
    }
    mama_seqnum_t MamdaQuoteListener::getBeginGapSeqNum() const
    {
        return mImpl.mQuoteListenerCache.mGapBegin;
    }

    mama_seqnum_t MamdaQuoteListener::getEndGapSeqNum() const
    {
        return mImpl.mQuoteListenerCache.mGapEnd;
    }

    char MamdaQuoteListener::getShortSaleBidTick () const
    {
        return mImpl.mQuoteListenerCache.mShortSaleBidTick;
    }

    const char*  MamdaQuoteListener::getBidSizesList()      const
    {
      return mImpl.mQuoteListenerCache.mBidSizesList.c_str();
    }

    const char*  MamdaQuoteListener::getAskSizesList()      const
    {
      return mImpl.mQuoteListenerCache.mAskSizesList.c_str();
    }

    char  MamdaQuoteListener::getShortSaleCircuitBreaker()      const
    {
      return mImpl.mQuoteListenerCache.mShortSaleCircuitBreaker;
    }

    void MamdaQuoteListener::assertEqual (MamdaQuoteListener* rhs)
    {
            mImpl.assertEqual (rhs->mImpl);
    }


    /* fieldState Accessors     */

    MamdaFieldState MamdaQuoteListener::getSymbolFieldState() const
    {
        return mImpl.mQuoteListenerCache.mSymbolFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getPartIdFieldState() const
    {
       return mImpl.mQuoteListenerCache.mPartIdFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getSrcTimeFieldState() const
    {
        return mImpl.mQuoteListenerCache.mSrcTimeFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getActivityTimeFieldState() const
    {
        return mImpl.mQuoteListenerCache.mActTimeFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getLineTimeFieldState() const
    {
        return mImpl.mQuoteListenerCache.mLineTimeFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getSendTimeFieldState() const
    {
        return mImpl.mQuoteListenerCache.mSendTimeFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getMsgQualFieldState() const
    {
        return mImpl.mMsgQualFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getPubIdFieldState() const
    {
        return mImpl.mQuoteListenerCache.mPubIdFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getBidPriceFieldState () const
    {
        return mImpl.mQuoteListenerCache.mBidPriceFieldState;
    }
     
    MamdaFieldState MamdaQuoteListener::getBidSizeFieldState() const
    {
        return mImpl.mQuoteListenerCache.mBidSizeFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getBidDepthFieldState() const
    {
        return mImpl.mQuoteListenerCache.mBidDepthFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getBidPartIdFieldState() const
    {
        return mImpl.mQuoteListenerCache.mBidPartIdFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getBidClosePriceFieldState() const
    {
        return mImpl.mQuoteListenerCache.mBidClosePriceFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getBidCloseDateFieldState() const
    {
        return mImpl.mQuoteListenerCache.mBidCloseDateFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getBidPrevClosePriceFieldState() const
    {
        return mImpl.mQuoteListenerCache.mBidPrevClosePriceFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getBidPrevCloseDateFieldState() const
    {
        return mImpl.mQuoteListenerCache.mBidPrevCloseDateFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getBidHighFieldState() const
    {
        return mImpl.mQuoteListenerCache.mBidHighFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getBidLowFieldState() const
    {
        return mImpl.mQuoteListenerCache.mBidLowFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getAskPriceFieldState() const
    {
        return mImpl.mQuoteListenerCache.mAskPriceFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getAskSizeFieldState() const
    {
        return mImpl.mQuoteListenerCache.mAskSizeFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getAskDepthFieldState() const
    {
        return mImpl.mQuoteListenerCache.mAskDepthFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getAskPartIdFieldState() const
    {
        return mImpl.mQuoteListenerCache.mAskPartIdFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getAskClosePriceFieldState() const
    {
        return mImpl.mQuoteListenerCache.mAskClosePriceFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getAskCloseDateFieldState() const
    {
        return mImpl.mQuoteListenerCache.mAskCloseDateFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getAskPrevClosePriceFieldState() const
    {
        return mImpl.mQuoteListenerCache.mAskPrevClosePriceFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getAskPrevCloseDateFieldState() const
    {
        return mImpl.mQuoteListenerCache.mAskPrevCloseDateFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getAskHighFieldState() const
    {
        return mImpl.mQuoteListenerCache.mAskHighFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getAskLowFieldState() const
    {
        return mImpl.mQuoteListenerCache.mAskLowFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getQuoteMidPriceFieldState() const
    {
        return mImpl.mQuoteListenerCache.mMidPriceFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getQuoteCountFieldState() const
    {
        return mImpl.mQuoteListenerCache.mQuoteCountFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getQuoteQualStrFieldState() const
    {
        return mImpl.mQuoteListenerCache.mQuoteQualStrFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getQuoteQualNativeFieldState() const
    {
        return mImpl.mQuoteListenerCache.mQuoteQualNativeFieldState;
    }

    MamdaFieldState   MamdaQuoteListener::getAskTimeFieldState() const
    {
        return mImpl.mQuoteListenerCache.mAskTimeFieldState;
    }

    MamdaFieldState   MamdaQuoteListener::getBidTimeFieldState() const
    {
        return mImpl.mQuoteListenerCache.mBidTimeFieldState;
    }

    MamdaFieldState  MamdaQuoteListener::getAskIndicatorFieldState() const
    {
        return mImpl.mQuoteListenerCache.mAskIndicatorFieldState;
    }

    MamdaFieldState  MamdaQuoteListener::getBidIndicatorFieldState() const
    {
        return mImpl.mQuoteListenerCache.mBidIndicatorFieldState;
    }

    MamdaFieldState  MamdaQuoteListener::getAskUpdateCountFieldState() const
    {
        return mImpl.mQuoteListenerCache.mAskUpdateCountFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getBidUpdateCountFieldState() const
    {
        return mImpl.mQuoteListenerCache.mBidUpdateCountFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getAskYieldFieldState() const
    {
        return mImpl.mQuoteListenerCache.mAskYieldFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getBidYieldFieldState() const
    {
        return mImpl.mQuoteListenerCache.mBidYieldFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getEventSeqNumFieldState() const
    {
        return mImpl.mQuoteListenerCache.mEventSeqNumFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getEventTimeFieldState() const
    {
        return mImpl.mQuoteListenerCache.mEventTimeFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getQuoteDateFieldState() const
    {
        return mImpl.mQuoteListenerCache.mQuoteDateFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getBeginGapSeqNumFieldState() const
    {
        return mImpl.mQuoteListenerCache.mGapBeginFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getEndGapSeqNumFieldState() const
    {
        return mImpl.mQuoteListenerCache.mGapEndFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getShortSaleBidTickFieldState () const
    {
        return mImpl.mQuoteListenerCache.mShortSaleBidTickFieldState;
    }

    MamdaFieldState  MamdaQuoteListener::getBidSizesListFieldState() const
    {
        return mImpl.mQuoteListenerCache.mBidSizesListFieldState;
    }

    MamdaFieldState  MamdaQuoteListener::getAskSizesListFieldState() const
    {
        return mImpl.mQuoteListenerCache.mAskSizesListFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getShortSaleCircuitBreakerFieldState() const
    {
        return mImpl.mQuoteListenerCache.mShortSaleCircuitBreakerFieldState;
    }

    /* End isModified Accessors     */


    void MamdaQuoteListener::onMsg (
        MamdaSubscription* subscription,
        const MamaMsg&     msg,
        short              msgType)
    {
        // If msg is a quote-related message, invoke the
        // appropriate callback:
        if (subscription->checkDebugLevel (MAMA_LOG_LEVEL_FINE))
        {
            const char *contractSymbol = "N/A";
            msg.tryString (MamdaCommonFields::ISSUE_SYMBOL, contractSymbol);

            mama_forceLog (MAMA_LOG_LEVEL_FINE,
                           "MamdaQuoteListener (%s.%s(%s)) onMsg(). "
                           "msg type: %s msg status %s\n",
                           subscription->getSource (),
                           subscription->getSymbol (),
                           contractSymbol,
                           msg.getMsgTypeName (),
                           msg.getMsgStatusString ());
        }


        switch (msgType)
        {
            case MAMA_MSG_TYPE_SNAPSHOT:
            case MAMA_MSG_TYPE_INITIAL:
            case MAMA_MSG_TYPE_RECAP:
            case MAMA_MSG_TYPE_PREOPENING:
            case MAMA_MSG_TYPE_QUOTE:
            case MAMA_MSG_TYPE_UPDATE:
            case MAMA_MSG_TYPE_TRADE:
                mImpl.handleQuoteMessage (subscription, msg, msgType);
                break;
        }
    }

    MamdaQuoteListener::MamdaQuoteListenerImpl::MamdaQuoteListenerImpl(
        MamdaQuoteListener& listener)
        : mListener                             (listener)
        , mHandler                              (NULL)
        , mProcessPosDupAndOutOfSeqAsTransient  (false)
        , mResolvePossiblyDuplicate             (false)
        , mUsePosDupAndOutOfSeqHandlers         (false)
        , mIsTransientMsg                       (false)
        , mTransientCache                       (NULL)
        , mQuoteListenerCache                   (mRegularCache)
    {
        clearCache (mRegularCache);
    }

    void MamdaQuoteListener::MamdaQuoteListenerImpl::clearCache (
        QuoteListenerCache& quoteCache)
    {
        quoteCache.mSymbol                 = "";    
        quoteCache.mPartId                 = "";    
        quoteCache.mSrcTime.clear          ();                
        quoteCache.mActTime.clear          ();                
        quoteCache.mLineTime.clear         ();               
        quoteCache.mSendTime.clear         ();               
        quoteCache.mPubId                  = "";    
        quoteCache.mBidPrice.clear         ();              
        quoteCache.mBidSize                = 0;     
        quoteCache.mBidDepth               = 0;     
        quoteCache.mBidPartId              = "";    
        quoteCache.mBidClosePrice.clear    ();          
        quoteCache.mBidCloseDate.clear     ();           
        quoteCache.mBidPrevClosePrice.clear();      
        quoteCache.mBidPrevCloseDate.clear ();       
        quoteCache.mBidHigh.clear          ();                
        quoteCache.mBidLow.clear           ();                 
        quoteCache.mAskPrice.clear         ();               
        quoteCache.mAskSize                = 0;     
        quoteCache.mAskDepth               = 0;     
        quoteCache.mAskClosePrice.clear    ();          
        quoteCache.mAskCloseDate.clear     ();           
        quoteCache.mAskPrevClosePrice.clear();      
        quoteCache.mAskPrevCloseDate.clear ();       
        quoteCache.mAskHigh.clear          ();                
        quoteCache.mAskLow.clear           ();                 
        quoteCache.mMidPrice.clear         ();               
        quoteCache.mEventSeqNum            = 0;     
        quoteCache.mEventTime.clear        ();              
        quoteCache.mQuoteDate.clear        ();              
        quoteCache.mQuoteQualStr           = "";    
        quoteCache.mQuoteQualNative        = "";    

        quoteCache.mAskTime.clear          ();                
        quoteCache.mBidTime.clear          ();                
        quoteCache.mAskIndicator           = "";    
        quoteCache.mBidIndicator           = "";    
        quoteCache.mAskUpdateCount         = 0;     
        quoteCache.mBidUpdateCount         = 0;     
        quoteCache.mAskYield               = 0.0;   
        quoteCache.mBidYield               = 0.0;   

        quoteCache.mTmpQuoteCount          = 0;     
        quoteCache.mQuoteCount             = 0;     
        quoteCache.mGapBegin               = 0;     
        quoteCache.mGapEnd                 = 0;      

        quoteCache.mLastGenericMsgWasQuote = false;
        quoteCache.mGotBidPrice            = false;
        quoteCache.mGotAskPrice            = false;
        quoteCache.mGotBidSize             = false;
        quoteCache.mGotAskSize             = false;
        quoteCache.mGotBidDepth            = false;
        quoteCache.mGotAskDepth            = false;
        quoteCache.mGotBidPartId           = false;
        quoteCache.mGotAskPartId           = false;
        quoteCache.mGotQuoteCount          = false;
        quoteCache.mShortSaleBidTick       = 'Z';
        quoteCache.mAskSizesList           = "";
        quoteCache.mBidSizesList           = "";
        quoteCache.mShortSaleCircuitBreaker= ' ';


        quoteCache.mSymbolFieldState            = NOT_INITIALISED;
        quoteCache.mPartIdFieldState            = NOT_INITIALISED;
        quoteCache.mSrcTimeFieldState           = NOT_INITIALISED;
        quoteCache.mActTimeFieldState           = NOT_INITIALISED;
        quoteCache.mLineTimeFieldState          = NOT_INITIALISED;
        quoteCache.mSendTimeFieldState          = NOT_INITIALISED;
        quoteCache.mPubIdFieldState             = NOT_INITIALISED;
        quoteCache.mBidPriceFieldState          = NOT_INITIALISED;
        quoteCache.mBidSizeFieldState           = NOT_INITIALISED;
        quoteCache.mBidDepthFieldState          = NOT_INITIALISED;
        quoteCache.mBidPartIdFieldState         = NOT_INITIALISED;
        quoteCache.mBidClosePriceFieldState     = NOT_INITIALISED;
        quoteCache.mBidCloseDateFieldState      = NOT_INITIALISED;
        quoteCache.mBidPrevClosePriceFieldState = NOT_INITIALISED;
        quoteCache.mBidPrevCloseDateFieldState  = NOT_INITIALISED;
        quoteCache.mBidHighFieldState           = NOT_INITIALISED;
        quoteCache.mBidLowFieldState            = NOT_INITIALISED;
        quoteCache.mAskPriceFieldState          = NOT_INITIALISED;
        quoteCache.mAskSizeFieldState           = NOT_INITIALISED;
        quoteCache.mAskDepthFieldState          = NOT_INITIALISED;
        quoteCache.mAskClosePriceFieldState     = NOT_INITIALISED;
        quoteCache.mAskCloseDateFieldState      = NOT_INITIALISED;
        quoteCache.mAskPrevClosePriceFieldState = NOT_INITIALISED;
        quoteCache.mAskPrevCloseDateFieldState  = NOT_INITIALISED;
        quoteCache.mAskHighFieldState           = NOT_INITIALISED;
        quoteCache.mAskLowFieldState            = NOT_INITIALISED;
        quoteCache.mMidPriceFieldState          = NOT_INITIALISED;
        quoteCache.mEventSeqNumFieldState       = NOT_INITIALISED;
        quoteCache.mEventTimeFieldState         = NOT_INITIALISED;
        quoteCache.mQuoteDateFieldState         = NOT_INITIALISED;
        quoteCache.mQuoteQualStrFieldState      = NOT_INITIALISED;
        quoteCache.mQuoteQualNativeFieldState   = NOT_INITIALISED;

        quoteCache.mAskTimeFieldState           = NOT_INITIALISED;
        quoteCache.mBidTimeFieldState           = NOT_INITIALISED;
        quoteCache.mAskIndicatorFieldState      = NOT_INITIALISED;
        quoteCache.mBidIndicatorFieldState      = NOT_INITIALISED;
        quoteCache.mAskUpdateCountFieldState    = NOT_INITIALISED;
        quoteCache.mBidUpdateCountFieldState    = NOT_INITIALISED;
        quoteCache.mAskYieldFieldState          = NOT_INITIALISED;
        quoteCache.mBidYieldFieldState          = NOT_INITIALISED;

        quoteCache.mTmpQuoteCountFieldState     = NOT_INITIALISED;
        quoteCache.mQuoteCountFieldState        = NOT_INITIALISED;
        quoteCache.mGapBeginFieldState          = NOT_INITIALISED;
        quoteCache.mGapEndFieldState            = NOT_INITIALISED;
        quoteCache.mAskSizesListFieldState      = NOT_INITIALISED;
        quoteCache.mBidSizesListFieldState      = NOT_INITIALISED;
        quoteCache.mShortSaleCircuitBreakerFieldState = NOT_INITIALISED;
      
        quoteCache.mLastGenericMsgWasQuote         = false;
        quoteCache.mGotBidPrice                    = false;
        quoteCache.mGotAskPrice                    = false;
        quoteCache.mGotBidSize                     = false;
        quoteCache.mGotAskSize                     = false;
        quoteCache.mGotBidDepth                    = false;
        quoteCache.mGotAskDepth                    = false;
        quoteCache.mGotBidPartId                   = false;
        quoteCache.mGotAskPartId                   = false;
        quoteCache.mGotQuoteCount                  = false;
        quoteCache.mShortSaleBidTick               = 'Z';
    } 
      
      
    void MamdaQuoteListener::MamdaQuoteListenerImpl::handleQuoteMessage (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg,
        short               msgType)
    { 
        // Ensure that the field handling is set up (once for all
        // MamdaQuoteListener instances).
        if (!mUpdatersComplete)
        {
            wthread_static_mutex_lock (&mQuoteFieldUpdaterLockMutex);
      
            if (!mUpdatersComplete)
            {
                if (!MamdaQuoteFields::isSet())
                {
                     mama_log (MAMA_LOG_LEVEL_WARN,
                               "MamdaQuoteListener: MamdaQuoteFields::setDictionary() "
                               "has not been called.");
                     wthread_static_mutex_unlock (&mQuoteFieldUpdaterLockMutex);
                     return;
                }
      
                try
                {
                    initFieldUpdaters ();
                }
                catch (MamaStatus &e)
                {
                    mama_log (MAMA_LOG_LEVEL_WARN,
                              "MamdaQuoteListener: Could not set field updaters: %s",
                              e.toString ());
                    wthread_static_mutex_unlock (&mQuoteFieldUpdaterLockMutex);
                    return;
                }
                mUpdatersComplete = true;
            }
            wthread_static_mutex_unlock (&mQuoteFieldUpdaterLockMutex);
        }

        // Determine if this message is a duplicate or a transient record 
        // which should not update the regular cache.
        bool isDuplicateMsg = evaluateMsgQual(subscription, msg);        
            
        if (!isDuplicateMsg)
        {
            if (mIsTransientMsg && mProcessPosDupAndOutOfSeqAsTransient)
            {
                // Use Transient Cache.
                if (mTransientCache == NULL)
                {
                    mTransientCache = new QuoteListenerCache();
                }
                mQuoteListenerCache = *mTransientCache;
            }

            // Handle fields in message:
            updateFieldStates ();
            updateQuoteFields (msg);
            
            // Handle according to message type:
            switch (msgType)
            {
                case MAMA_MSG_TYPE_INITIAL:
                case MAMA_MSG_TYPE_RECAP:
                case MAMA_MSG_TYPE_PREOPENING:
                case MAMA_MSG_TYPE_SNAPSHOT:
                    handleRecap (subscription, msg);
                    break;
                case MAMA_MSG_TYPE_QUOTE:
                    handleQuote (subscription, msg);
                    break;
                case MAMA_MSG_TYPE_UPDATE:
                case MAMA_MSG_TYPE_TRADE:
                    handleUpdate (subscription, msg);
                    break;
            }
            
            if (mIsTransientMsg && mProcessPosDupAndOutOfSeqAsTransient)
            {
                mQuoteListenerCache = mRegularCache;
                clearCache(*mTransientCache);
            }
        }
        else
        {
            if (subscription->checkDebugLevel (MAMA_LOG_LEVEL_FINE))
            {
                const char *contractSymbol = "N/A";
                msg.tryString (MamdaCommonFields::ISSUE_SYMBOL, contractSymbol);

                mama_forceLog (MAMA_LOG_LEVEL_FINE,
                               "MamdaQuoteListener (%s.%s(%s)) "
                               "Duplicate message NOT processed.\n",
                               contractSymbol,
                               subscription->getSource (),
                               subscription->getSymbol ());
            }
        }
    }

    bool MamdaQuoteListener::MamdaQuoteListenerImpl::evaluateMsgQual(    
        MamdaSubscription*  subscription,
        const MamaMsg&      msg)
    {
        bool isDuplicateMsg = false;
        mIsTransientMsg     = false;

        mMsgQual.clear();   
        uint16_t msgQualVal = 0;

        // Does message contains a qualifier - need to parse this anyway.
        if (msg.tryU16 (MamaFieldMsgQual.mName, MamaFieldMsgQual.mFid ,msgQualVal))
        {
            mMsgQual.setValue(msgQualVal);

            // If qualifier indicates message is possbily duplicate
            // and the listener has been configured to attempt to 
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
                    // Only make a determination as to whether or
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

    void MamdaQuoteListener::MamdaQuoteListenerImpl::handleRecap (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg)
    {
        if (subscription->checkDebugLevel (MAMA_LOG_LEVEL_FINE))
        {
            const char *contractSymbol = "N/A";
            msg.tryString (MamdaCommonFields::ISSUE_SYMBOL, contractSymbol);
            mama_forceLog (
                MAMA_LOG_LEVEL_FINE,
                "MamdaQuoteListener (%s.%s(%s)) handleRecap().\n",
                contractSymbol,
                subscription->getSource (),
                subscription->getSymbol ());
        }

        checkQuoteCount (subscription, msg, false);

        if (mHandler)
        {
            if (mUsePosDupAndOutOfSeqHandlers && mIsTransientMsg)
            {
                invokeTransientHandler(subscription, msg);
            }
            else
            {
                mHandler->onQuoteRecap (subscription, 
                                        mListener, 
                                        msg, 
                                        mListener);
            }
        }
    }

    void MamdaQuoteListener::MamdaQuoteListenerImpl::handleQuote (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg)
    {
        checkQuoteCount (subscription, msg, true);

        if (mIgnoreUpdate)
        {
            mIgnoreUpdate = false;
            return;
        }

        if (mHandler)
        {
            if (mQuoteListenerCache.mQuoteQualStr.compare ("Closing") != 0)
            {
                if (subscription->checkDebugLevel (MAMA_LOG_LEVEL_FINE))
                {
                    const char *contractSymbol = "N/A";
                    msg.tryString (MamdaCommonFields::ISSUE_SYMBOL, 
                                   contractSymbol);

                    mama_forceLog (MAMA_LOG_LEVEL_FINE,
                                   "MamdaQuoteListener (%s.%s(%s)) handleQuote(): update.\n",
                                   subscription->getSource (),
                                   subscription->getSymbol (),
                                   contractSymbol);
                }

                if (mUsePosDupAndOutOfSeqHandlers && mIsTransientMsg)
                {
                    invokeTransientHandler(subscription, msg);
                }
                else
                {
                    mHandler->onQuoteUpdate (subscription, 
                                             mListener, 
                                             msg, 
                                             mListener, 
                                             mListener);
                }
            }
            else
            {
                if (subscription->checkDebugLevel (MAMA_LOG_LEVEL_FINE))
                {
                    const char *contractSymbol = "N/A";
                    msg.tryString (MamdaCommonFields::ISSUE_SYMBOL, 
                                   contractSymbol);

                    mama_forceLog (MAMA_LOG_LEVEL_FINE,
                                   "MamdaQuoteListener (%s.%s(%s)) handleQuote(): closing.\n",
                                   subscription->getSource (),
                                   subscription->getSymbol (),
                                   contractSymbol);
                }

                if (mUsePosDupAndOutOfSeqHandlers && mIsTransientMsg)
                {
                    invokeTransientHandler(subscription, msg);
                }
                else
                {
                    mHandler->onQuoteClosing (subscription, 
                                              mListener, 
                                              msg, 
                                              mListener, 
                                              mListener);
                }
            }
        }
    }

    void MamdaQuoteListener::MamdaQuoteListenerImpl::handleUpdate (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg)
    {
        // Generic update (might be a quote)
        checkQuoteCount (subscription, msg, true);

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
                           "MamdaQuoteListener (%s.%s(%s)) handleUpdate(): %d.\n",
                           subscription->getSource (),
                           subscription->getSymbol (),
                           contractSymbol,
                           mQuoteListenerCache.mLastGenericMsgWasQuote);
        }

        if (mHandler)
        {
            if (mQuoteListenerCache.mLastGenericMsgWasQuote)
            {
                if (mUsePosDupAndOutOfSeqHandlers && mIsTransientMsg)
                {
                    invokeTransientHandler(subscription, msg);
                }
                else
                {
                    mHandler->onQuoteUpdate (subscription, 
                                             mListener, 
                                             msg, 
                                             mListener, 
                                             mListener);
                }
                mQuoteListenerCache.mLastGenericMsgWasQuote = false;
            }
        }
    }

    void MamdaQuoteListener::MamdaQuoteListenerImpl::invokeTransientHandler(
        MamdaSubscription*  subscription,
        const MamaMsg&      msg)
     {
         bool handled = false;
         if (mMsgQual.getIsOutOfSequence())
         {
             handled = true;
             mHandler->onQuoteOutOfSequence(subscription,
                                            mListener,
                                            msg, 
                                            mListener, 
                                            mListener);
         }
         if (mMsgQual.getIsPossiblyDuplicate())
         {
             handled = true;
             mHandler->onQuotePossiblyDuplicate (subscription, 
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
                            "MamdaQuoteListener (%s.%s(%s)) handleQuote(): "
                            "update: Transient Msg neither Out-of-Sequence "
                            "nor Possibly Stale",
                            subscription->getSource (),
                            subscription->getSymbol (),
                            contractSymbol);
         }
     }

    void MamdaQuoteListener::MamdaQuoteListenerImpl::updateFieldStates ()
    {

        if (mQuoteListenerCache.mPartIdFieldState            == MODIFIED)
            mQuoteListenerCache.mPartIdFieldState            = NOT_MODIFIED;

        if (mQuoteListenerCache.mSrcTimeFieldState           == MODIFIED)
            mQuoteListenerCache.mSrcTimeFieldState           = NOT_MODIFIED;

        if (mQuoteListenerCache.mActTimeFieldState           == MODIFIED)
            mQuoteListenerCache.mActTimeFieldState           = NOT_MODIFIED;

        if (mQuoteListenerCache.mLineTimeFieldState          == MODIFIED)
            mQuoteListenerCache.mLineTimeFieldState          = NOT_MODIFIED;

        if (mQuoteListenerCache.mSendTimeFieldState          == MODIFIED)
            mQuoteListenerCache.mSendTimeFieldState          = NOT_MODIFIED;

        if (mQuoteListenerCache.mPubIdFieldState             == MODIFIED)
            mQuoteListenerCache.mPubIdFieldState             = NOT_MODIFIED;

        if (mQuoteListenerCache.mBidPriceFieldState          == MODIFIED)
            mQuoteListenerCache.mBidPriceFieldState          = NOT_MODIFIED;

        if (mQuoteListenerCache.mBidSizeFieldState           == MODIFIED)
            mQuoteListenerCache.mBidSizeFieldState           = NOT_MODIFIED;

        if (mQuoteListenerCache.mBidDepthFieldState          == MODIFIED)
            mQuoteListenerCache.mBidDepthFieldState          = NOT_MODIFIED;

        if (mQuoteListenerCache.mBidPartIdFieldState         == MODIFIED)
            mQuoteListenerCache.mBidPartIdFieldState         = NOT_MODIFIED;

        if (mQuoteListenerCache.mBidClosePriceFieldState     == MODIFIED)
            mQuoteListenerCache.mBidClosePriceFieldState     = NOT_MODIFIED;

        if (mQuoteListenerCache.mBidCloseDateFieldState      == MODIFIED)
            mQuoteListenerCache.mBidCloseDateFieldState      = NOT_MODIFIED;

        if (mQuoteListenerCache.mBidPrevClosePriceFieldState == MODIFIED)
            mQuoteListenerCache.mBidPrevClosePriceFieldState = NOT_MODIFIED;

        if (mQuoteListenerCache.mBidPrevCloseDateFieldState  == MODIFIED)
            mQuoteListenerCache.mBidPrevCloseDateFieldState  = NOT_MODIFIED;

        if (mQuoteListenerCache.mBidHighFieldState           == MODIFIED)
            mQuoteListenerCache.mBidHighFieldState           = NOT_MODIFIED;

        if (mQuoteListenerCache.mBidLowFieldState            == MODIFIED)
            mQuoteListenerCache.mBidLowFieldState            = NOT_MODIFIED;

        if (mQuoteListenerCache.mAskPriceFieldState          == MODIFIED)
            mQuoteListenerCache.mAskPriceFieldState          = NOT_MODIFIED;

        if (mQuoteListenerCache.mAskSizeFieldState           == MODIFIED)
            mQuoteListenerCache.mAskSizeFieldState           = NOT_MODIFIED;

        if (mQuoteListenerCache.mAskDepthFieldState          == MODIFIED)
            mQuoteListenerCache.mAskDepthFieldState          = NOT_MODIFIED;

        if (mQuoteListenerCache.mAskClosePriceFieldState     == MODIFIED)
            mQuoteListenerCache.mAskClosePriceFieldState     = NOT_MODIFIED;

        if (mQuoteListenerCache.mAskCloseDateFieldState      == MODIFIED)
            mQuoteListenerCache.mAskCloseDateFieldState      = NOT_MODIFIED;

        if (mQuoteListenerCache.mAskPrevClosePriceFieldState == MODIFIED)
            mQuoteListenerCache.mAskPrevClosePriceFieldState = NOT_MODIFIED;

        if (mQuoteListenerCache.mAskPrevCloseDateFieldState  == MODIFIED)
            mQuoteListenerCache.mAskPrevCloseDateFieldState  = NOT_MODIFIED;

        if (mQuoteListenerCache.mAskHighFieldState           == MODIFIED)
            mQuoteListenerCache.mAskHighFieldState           = NOT_MODIFIED;

        if (mQuoteListenerCache.mAskLowFieldState            == MODIFIED)
            mQuoteListenerCache.mAskLowFieldState            = NOT_MODIFIED;

        if (mQuoteListenerCache.mMidPriceFieldState          == MODIFIED)
            mQuoteListenerCache.mMidPriceFieldState          = NOT_MODIFIED;

        if (mQuoteListenerCache.mEventSeqNumFieldState       == MODIFIED)
            mQuoteListenerCache.mEventSeqNumFieldState       = NOT_MODIFIED;

        if (mQuoteListenerCache.mEventTimeFieldState         == MODIFIED)
            mQuoteListenerCache.mEventTimeFieldState         = NOT_MODIFIED;

        if (mQuoteListenerCache.mQuoteDateFieldState         == MODIFIED)
            mQuoteListenerCache.mQuoteDateFieldState         = NOT_MODIFIED;

        if (mQuoteListenerCache.mQuoteQualStrFieldState      == MODIFIED)
            mQuoteListenerCache.mQuoteQualStrFieldState      = NOT_MODIFIED;

        if (mQuoteListenerCache.mQuoteQualNativeFieldState   == MODIFIED)
            mQuoteListenerCache.mQuoteQualNativeFieldState   = NOT_MODIFIED;

        if (mQuoteListenerCache.mAskTimeFieldState           == MODIFIED)
            mQuoteListenerCache.mAskTimeFieldState           = NOT_MODIFIED;

        if (mQuoteListenerCache.mBidTimeFieldState           == MODIFIED)
            mQuoteListenerCache.mBidTimeFieldState           = NOT_MODIFIED;

        if (mQuoteListenerCache.mAskIndicatorFieldState      == MODIFIED)
            mQuoteListenerCache.mAskIndicatorFieldState      = NOT_MODIFIED;

        if (mQuoteListenerCache.mBidIndicatorFieldState      == MODIFIED)
            mQuoteListenerCache.mBidIndicatorFieldState      = NOT_MODIFIED;

        if (mQuoteListenerCache.mAskUpdateCountFieldState    == MODIFIED)
            mQuoteListenerCache.mAskUpdateCountFieldState    = NOT_MODIFIED;

        if (mQuoteListenerCache.mBidUpdateCountFieldState    == MODIFIED)
            mQuoteListenerCache.mBidUpdateCountFieldState    = NOT_MODIFIED;

        if (mQuoteListenerCache.mAskYieldFieldState          == MODIFIED)
            mQuoteListenerCache.mAskYieldFieldState          = NOT_MODIFIED;

        if (mQuoteListenerCache.mBidYieldFieldState          == MODIFIED)
            mQuoteListenerCache.mBidYieldFieldState          = NOT_MODIFIED;

        if (mQuoteListenerCache.mTmpQuoteCountFieldState     == MODIFIED)
            mQuoteListenerCache.mTmpQuoteCountFieldState     = NOT_MODIFIED;

        if (mQuoteListenerCache.mQuoteCountFieldState        == MODIFIED)
            mQuoteListenerCache.mQuoteCountFieldState        = NOT_MODIFIED;

        if (mQuoteListenerCache.mGapBeginFieldState          == MODIFIED)
            mQuoteListenerCache.mGapBeginFieldState          = NOT_MODIFIED;

        if (mQuoteListenerCache.mGapEndFieldState            == MODIFIED)
            mQuoteListenerCache.mGapEndFieldState            = NOT_MODIFIED;

        if (mQuoteListenerCache.mAskSizesListFieldState      == MODIFIED)
            mQuoteListenerCache.mAskSizesListFieldState      = NOT_MODIFIED;

        if (mQuoteListenerCache.mBidSizesListFieldState      == MODIFIED)
            mQuoteListenerCache.mBidSizesListFieldState      = NOT_MODIFIED;

        if (mQuoteListenerCache.mShortSaleCircuitBreakerFieldState == MODIFIED)
            mQuoteListenerCache.mShortSaleCircuitBreakerFieldState = NOT_MODIFIED;
    }

    void MamdaQuoteListener::MamdaQuoteListenerImpl::updateQuoteFields (
        const MamaMsg&  msg)
    {   
        const char* symbol = NULL;
        const char* partId = NULL;
        
        getSymbolAndPartId (msg, symbol, partId);

        if (symbol) 
        {
          mQuoteListenerCache.mSymbol = symbol;
          mQuoteListenerCache.mSymbolFieldState = MODIFIED;
        }

        if (partId) 
        {
          mQuoteListenerCache.mPartId = partId;
          mQuoteListenerCache.mPartIdFieldState = MODIFIED;
        }

        mQuoteListenerCache.mLastGenericMsgWasQuote = false;
        mQuoteListenerCache.mGotBidPrice   = false;
        mQuoteListenerCache.mGotAskPrice   = false;
        mQuoteListenerCache.mGotBidSize    = false;
        mQuoteListenerCache.mGotAskSize    = false;
        mQuoteListenerCache.mGotBidDepth   = false;
        mQuoteListenerCache.mGotAskDepth   = false;
        mQuoteListenerCache.mGotBidPartId  = false;
        mQuoteListenerCache.mGotAskPartId  = false;
        mQuoteListenerCache.mGotQuoteCount = false;
        
        
        // Iterate over all of the fields in the message.
        wthread_mutex_lock (&mQuoteUpdateLock.mQuoteUpdateLockMutex);
        msg.iterateFields (*this, NULL, NULL);
        
        wthread_mutex_unlock (&mQuoteUpdateLock.mQuoteUpdateLockMutex);
        
        // Check certain special fields.
        if (mQuoteListenerCache.mGotBidSize    || mQuoteListenerCache.mGotAskSize   ||
            mQuoteListenerCache.mGotBidDepth   || mQuoteListenerCache.mGotAskDepth  ||
            mQuoteListenerCache.mGotBidPrice   || mQuoteListenerCache.mGotAskPrice  ||
            mQuoteListenerCache.mGotBidPartId  || mQuoteListenerCache.mGotAskPartId)
        {
            mQuoteListenerCache.mLastGenericMsgWasQuote = true;
        }
    }   
        
    void MamdaQuoteListener::MamdaQuoteListenerImpl::onField (
        const MamaMsg&       msg,
        const MamaMsgField&  field,
        void*                closure)
    {
        uint16_t fid = field.getFid();
        if (fid <= mFieldUpdatersSize)
        {
            QuoteFieldUpdate* updater = mFieldUpdaters[fid];
            if (updater)
            {
                updater->onUpdate (*this, field);
            }
        }
    }

    void MamdaQuoteListener::MamdaQuoteListenerImpl::checkQuoteCount (
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
            mQuoteListenerCache.mQuoteCount = mQuoteListenerCache.mTmpQuoteCount;
            return;
        }

        // Check number of quotes for gaps
        mama_u32_t quoteCount    = mQuoteListenerCache.mTmpQuoteCount;
        mama_u32_t conflateCount = 0;

        if (!msg.tryU32 ("wConflateQuoteCount", 23, conflateCount))
        {
            conflateCount = 1;
        }

        if (checkForGap && (mQuoteListenerCache.mGotQuoteCount))
        {
            if ((mQuoteListenerCache.mQuoteCount > 0) && 
                (quoteCount > (mQuoteListenerCache.mQuoteCount+conflateCount)))
            {
                mQuoteListenerCache.mGapBegin   = mQuoteListenerCache.mQuoteCount + conflateCount;
                mQuoteListenerCache.mGapEnd     = quoteCount-1;
                mQuoteListenerCache.mQuoteCount = quoteCount;

                if (mHandler)
                {
                    if (subscription->checkDebugLevel (MAMA_LOG_LEVEL_FINE))
                    {
                        const char *contractSymbol = "N/A";
                        msg.tryString (MamdaCommonFields::ISSUE_SYMBOL, 
                                       contractSymbol);

                        mama_forceLog (MAMA_LOG_LEVEL_FINE,
                                       "MamdaQuoteListener (%s.%s(%s)) onQuoteGap().\n",
                                       subscription->getSource (),
                                       subscription->getSymbol (),
                                       contractSymbol);
                    }
                    mHandler->onQuoteGap (subscription, 
                                          mListener, 
                                          msg,
                                          mListener,
                                          mListener);
                }
            }
        }
       
        /* Check for duplicate messages.  Only check if quoteCount was int the
            latest message in case quote count isn't being sent down. */ 
        if ( mQuoteListenerCache.mGotQuoteCount && 
             quoteCount > 0 &&
             (quoteCount == mQuoteListenerCache.mQuoteCount) )
        {
            mIgnoreUpdate = true;
        }

        mQuoteListenerCache.mQuoteCount = quoteCount;
    }

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateQuoteSrcTime 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getDateTime(impl.mQuoteListenerCache.mSrcTime);
            impl.mQuoteListenerCache.mSrcTimeFieldState = MODIFIED;
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateQuoteSendTime
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getDateTime(impl.mQuoteListenerCache.mSendTime);
            impl.mQuoteListenerCache.mSendTimeFieldState = MODIFIED;
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateQuoteActTime 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getDateTime(impl.mQuoteListenerCache.mActTime);
            impl.mQuoteListenerCache.mActTimeFieldState = MODIFIED;
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateQuoteLineTime 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getDateTime(impl.mQuoteListenerCache.mLineTime);
            impl.mQuoteListenerCache.mLineTimeFieldState = MODIFIED;
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateQuotePubId 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mQuoteListenerCache.mPubId = field.getString();
            impl.mQuoteListenerCache.mPubIdFieldState = MODIFIED;
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateBidPrice 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            MamaPrice bidPrice;
            field.getPrice (bidPrice);
            if (impl.mQuoteListenerCache.mBidPrice != bidPrice ||
                impl.mQuoteListenerCache.mBidPriceFieldState == NOT_INITIALISED)
            {
                impl.mQuoteListenerCache.mBidPrice = bidPrice;
                impl.mQuoteListenerCache.mGotBidPrice = true;
                impl.mQuoteListenerCache.mBidPriceFieldState = MODIFIED;
            }
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateBidSize 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            mama_quantity_t bidSize = field.getF64();
            if (impl.mQuoteListenerCache.mBidSize != bidSize)
            {
                impl.mQuoteListenerCache.mBidSize = bidSize;
                impl.mQuoteListenerCache.mGotBidSize = true;
                impl.mQuoteListenerCache.mBidSizeFieldState = MODIFIED;
            }
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateBidDepth 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            mama_quantity_t bidDepth = field.getF64();
            if (impl.mQuoteListenerCache.mBidDepth != bidDepth)
            {
                impl.mQuoteListenerCache.mBidDepth = bidDepth;
                impl.mQuoteListenerCache.mGotBidDepth = true;
                impl.mQuoteListenerCache.mBidDepthFieldState = MODIFIED;
            }
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateBidPartId 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            const char* bidPartId = field.getString();
            if (impl.mQuoteListenerCache.mBidPartId != bidPartId)
            {
                impl.mQuoteListenerCache.mBidPartId = bidPartId;
                impl.mQuoteListenerCache.mGotBidPartId = true;
                impl.mQuoteListenerCache.mBidPartIdFieldState = MODIFIED;
            }
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateBidClosePrice 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getPrice (impl.mQuoteListenerCache.mBidClosePrice);
            impl.mQuoteListenerCache.mBidClosePriceFieldState = MODIFIED;
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateBidCloseDate 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getDateTime(impl.mQuoteListenerCache.mBidCloseDate);
            impl.mQuoteListenerCache.mBidCloseDateFieldState = MODIFIED;
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateBidPrevClosePrice 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getPrice (impl.mQuoteListenerCache.mBidPrevClosePrice);
            impl.mQuoteListenerCache.mBidPrevClosePriceFieldState = MODIFIED;
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateBidPrevCloseDate 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getDateTime(impl.mQuoteListenerCache.mBidPrevCloseDate);
            impl.mQuoteListenerCache.mBidPrevCloseDateFieldState = MODIFIED;
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateBidHigh
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getPrice(impl.mQuoteListenerCache.mBidHigh);
            impl.mQuoteListenerCache.mBidHighFieldState = MODIFIED;
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateBidLow
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getPrice(impl.mQuoteListenerCache.mBidLow);
            impl.mQuoteListenerCache.mBidLowFieldState = MODIFIED;
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateAskPrice 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            MamaPrice askPrice;
            field.getPrice (askPrice);
            if (impl.mQuoteListenerCache.mAskPrice != askPrice)
            {
                impl.mQuoteListenerCache.mAskPrice = askPrice;
                impl.mQuoteListenerCache.mGotAskPrice = true;
                impl.mQuoteListenerCache.mAskPriceFieldState = MODIFIED;
            }
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateAskSize 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            mama_quantity_t askSize = field.getF64();
            if (impl.mQuoteListenerCache.mAskSize != askSize)
            {
                impl.mQuoteListenerCache.mAskSize = askSize;
                impl.mQuoteListenerCache.mGotAskSize = true;
                impl.mQuoteListenerCache.mAskSizeFieldState = MODIFIED;
            }
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateAskDepth 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            mama_quantity_t askDepth = field.getF64();
            if (impl.mQuoteListenerCache.mAskDepth != askDepth)
            {
                impl.mQuoteListenerCache.mAskDepth = askDepth;
                impl.mQuoteListenerCache.mGotAskDepth = true;
                impl.mQuoteListenerCache.mAskDepthFieldState = MODIFIED;
            }
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateAskPartId 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            const char* askPartId = field.getString();
            if (impl.mQuoteListenerCache.mAskPartId != askPartId)
            {
                impl.mQuoteListenerCache.mAskPartId = askPartId;
                impl.mQuoteListenerCache.mGotAskPartId = true;
                impl.mQuoteListenerCache.mAskPartIdFieldState = MODIFIED;
            }
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateAskClosePrice 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getPrice (impl.mQuoteListenerCache.mAskClosePrice);
            impl.mQuoteListenerCache.mAskClosePriceFieldState = MODIFIED;
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateAskCloseDate 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getDateTime(impl.mQuoteListenerCache.mAskCloseDate);
            impl.mQuoteListenerCache.mAskCloseDateFieldState = MODIFIED;
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateAskPrevClosePrice 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getPrice (impl.mQuoteListenerCache.mAskPrevClosePrice);
            impl.mQuoteListenerCache.mAskPrevClosePriceFieldState = MODIFIED;
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateAskPrevCloseDate 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getDateTime(impl.mQuoteListenerCache.mAskPrevCloseDate);
            impl.mQuoteListenerCache.mAskPrevCloseDateFieldState = MODIFIED;
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateAskHigh
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getPrice(impl.mQuoteListenerCache.mAskHigh);
            impl.mQuoteListenerCache.mAskHighFieldState = MODIFIED;
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateAskLow
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getPrice(impl.mQuoteListenerCache.mAskLow);
            impl.mQuoteListenerCache.mAskLowFieldState = MODIFIED;
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateQuoteSeqNum 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mQuoteListenerCache.mEventSeqNum = field.getU32();
            impl.mQuoteListenerCache.mEventSeqNumFieldState = MODIFIED;
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateQuoteTime 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getDateTime(impl.mQuoteListenerCache.mEventTime);
            impl.mQuoteListenerCache.mEventTimeFieldState = MODIFIED;
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateQuoteDate 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getDateTime(impl.mQuoteListenerCache.mQuoteDate);
            impl.mQuoteListenerCache.mQuoteDateFieldState = MODIFIED;
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateTmpQuoteCount 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mQuoteListenerCache.mTmpQuoteCount = field.getU32();
            impl.mQuoteListenerCache.mGotQuoteCount = true;
            impl.mQuoteListenerCache.mTmpQuoteCountFieldState = MODIFIED;
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateQuoteQual 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mQuoteListenerCache.mQuoteQualStr = field.getString();
            impl.mQuoteListenerCache.mQuoteQualStrFieldState = MODIFIED;
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateQuoteQualNative 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            if (field.getType () == MAMA_FIELD_TYPE_STRING)
            {
                impl.mQuoteListenerCache.mQuoteQualNative = field.getString();
                impl.mQuoteListenerCache.mQuoteQualNativeFieldState = MODIFIED;
            }
            else if (field.getType () == MAMA_FIELD_TYPE_CHAR)
            {
                impl.mQuoteListenerCache.mQuoteQualNative = field.getChar();
                impl.mQuoteListenerCache.mQuoteQualNativeFieldState = MODIFIED;
            }
        }
    };


    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateAskTime 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getDateTime(impl.mQuoteListenerCache.mAskTime);
            impl.mQuoteListenerCache.mAskTimeFieldState = MODIFIED;
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateBidTime 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getDateTime(impl.mQuoteListenerCache.mBidTime);
            impl.mQuoteListenerCache.mBidTimeFieldState = MODIFIED;
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateBidIndicator 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            if (field.getType () == MAMA_FIELD_TYPE_STRING)
            {
                impl.mQuoteListenerCache.mBidIndicator = field.getString();
                impl.mQuoteListenerCache.mBidIndicatorFieldState = MODIFIED;
            }
            else if (field.getType () == MAMA_FIELD_TYPE_CHAR)
            {
                impl.mQuoteListenerCache.mBidIndicator = field.getChar();
                impl.mQuoteListenerCache.mBidIndicatorFieldState = MODIFIED;
            }
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateAskIndicator 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            if (field.getType () == MAMA_FIELD_TYPE_STRING)
            {
                impl.mQuoteListenerCache.mAskIndicator = field.getString();
                impl.mQuoteListenerCache.mAskIndicatorFieldState = MODIFIED;
            }
            else if (field.getType () == MAMA_FIELD_TYPE_CHAR)
            {
                impl.mQuoteListenerCache.mAskIndicator = field.getChar();
                impl.mQuoteListenerCache.mAskIndicatorFieldState = MODIFIED;
            }
        }
    };



    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateBidUpdateCount 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mQuoteListenerCache.mBidUpdateCount = field.getU32();
            impl.mQuoteListenerCache.mBidUpdateCountFieldState = MODIFIED;
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateAskUpdateCount 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mQuoteListenerCache.mAskUpdateCount = field.getU32();
            impl.mQuoteListenerCache.mAskUpdateCountFieldState = MODIFIED;
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateAskYield 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mQuoteListenerCache.mAskYield = field.getF64();
            impl.mQuoteListenerCache.mAskYieldFieldState = MODIFIED;
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateBidYield 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mQuoteListenerCache.mBidYield = field.getF64();
            impl.mQuoteListenerCache.mBidYieldFieldState = MODIFIED;
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateBidSizesList 
        : public QuoteFieldUpdate
    {
      void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                     const MamaMsgField&                          field)
      {
          impl.mQuoteListenerCache.mBidSizesList = field.getString();
          impl.mQuoteListenerCache.mBidSizesListFieldState = MODIFIED;
      }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateAskSizesList 
        : public QuoteFieldUpdate
    {
      void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                     const MamaMsgField&                          field)
      {
        impl.mQuoteListenerCache.mAskSizesList = field.getString();
        impl.mQuoteListenerCache.mAskSizesListFieldState = MODIFIED;
      }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateShortSaleCircuitBreaker
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {       
            if (field.getChar() != impl.mQuoteListenerCache.mShortSaleCircuitBreaker)
            {
                impl.mQuoteListenerCache.mShortSaleCircuitBreaker = field.getChar();
                impl.mQuoteListenerCache.mShortSaleCircuitBreakerFieldState = MODIFIED;
            } 
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateShortSaleBidTick
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            //There is a bug in 2.14 FHs which can cause character fields to be sent as strings
            //FH property CharFieldAsStringField in 2.16-> can enable this behaviour
            //Adding support for this in MAMDA for client apps coded to expect this behaviour
            switch (field.getType())
            {
                case MAMA_FIELD_TYPE_CHAR :
                    impl.mQuoteListenerCache.mShortSaleBidTick = field.getChar ();
                    break;
                case MAMA_FIELD_TYPE_STRING :
                    impl.mQuoteListenerCache.mShortSaleBidTick = field.getString() [0];
                    break;
                default : 
                    break;
            }
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateBidTick
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            //There is a bug in 2.14 FHs which can cause character fields to be sent as strings
            //FH property CharFieldAsStringField in 2.16-> can enable this behaviour
            //Adding support for this in MAMDA for client apps coded to expect this behaviour
            switch (field.getType())
            {
                case MAMA_FIELD_TYPE_CHAR :
                    impl.mQuoteListenerCache.mShortSaleBidTick = field.getChar ();
                    break;
                case MAMA_FIELD_TYPE_STRING :
                    impl.mQuoteListenerCache.mShortSaleBidTick = field.getString() [0];
                    break;
                default : 
                    break;
            }
        }
    };


    QuoteFieldUpdate** MamdaQuoteListener::MamdaQuoteListenerImpl::mFieldUpdaters     = NULL;
    volatile uint16_t  MamdaQuoteListener::MamdaQuoteListenerImpl::mFieldUpdatersSize = 0;

    wthread_static_mutex_t MamdaQuoteListener::MamdaQuoteListenerImpl::mQuoteFieldUpdaterLockMutex 
            = WSTATIC_MUTEX_INITIALIZER;

    bool MamdaQuoteListener::MamdaQuoteListenerImpl::mUpdatersComplete = false;


    void MamdaQuoteListener::MamdaQuoteListenerImpl::initFieldUpdaters ()
    {
        if (!mFieldUpdaters)
        {
            mFieldUpdaters = new QuoteFieldUpdate* [MamdaQuoteFields::getMaxFid() +1];
            mFieldUpdatersSize = MamdaQuoteFields::getMaxFid();

            /* Use uint32_t instead of uint16_t to avoid infinite loop if max FID = 65535 */
            for (uint32_t i = 0; i <= mFieldUpdatersSize; ++i)
            {
                mFieldUpdaters[i] = NULL;
            }
        }

        initFieldUpdater(MamdaCommonFields::SRC_TIME,
                         new MamdaQuoteListener::
		                 MamdaQuoteListenerImpl::FieldUpdateQuoteSrcTime);

        initFieldUpdater(MamdaCommonFields::ACTIVITY_TIME,
                         new MamdaQuoteListener::
		                 MamdaQuoteListenerImpl::FieldUpdateQuoteActTime);

        initFieldUpdater(MamdaCommonFields::LINE_TIME,
                         new MamdaQuoteListener::
		                 MamdaQuoteListenerImpl::FieldUpdateQuoteLineTime);

        initFieldUpdater(MamdaCommonFields::PUB_ID,
                         new MamdaQuoteListener::
		                 MamdaQuoteListenerImpl::FieldUpdateQuotePubId);

        initFieldUpdater(MamdaCommonFields::SEND_TIME,
                         new MamdaQuoteListener::
                         MamdaQuoteListenerImpl::FieldUpdateQuoteSendTime);

        initFieldUpdater(MamdaQuoteFields::BID_PRICE,
                         new MamdaQuoteListener::
		                 MamdaQuoteListenerImpl::FieldUpdateBidPrice);

        initFieldUpdater(MamdaQuoteFields::BID_SIZE,
                         new MamdaQuoteListener::
		                 MamdaQuoteListenerImpl::FieldUpdateBidSize);

        initFieldUpdater(MamdaQuoteFields::BID_DEPTH,
                         new MamdaQuoteListener::
		                 MamdaQuoteListenerImpl::FieldUpdateBidDepth);

        initFieldUpdater(MamdaQuoteFields::BID_PART_ID,
                         new MamdaQuoteListener::
		                 MamdaQuoteListenerImpl::FieldUpdateBidPartId);

        initFieldUpdater(MamdaQuoteFields::BID_CLOSE_PRICE,
                         new MamdaQuoteListener::
		                 MamdaQuoteListenerImpl::FieldUpdateBidClosePrice);

        initFieldUpdater(MamdaQuoteFields::BID_CLOSE_DATE,
                         new MamdaQuoteListener::
		                 MamdaQuoteListenerImpl::FieldUpdateBidCloseDate);

        initFieldUpdater(MamdaQuoteFields::BID_PREV_CLOSE_PRICE,
                         new MamdaQuoteListener::
		                 MamdaQuoteListenerImpl::FieldUpdateBidPrevClosePrice);

        initFieldUpdater(MamdaQuoteFields::BID_PREV_CLOSE_DATE,
                         new MamdaQuoteListener::
		                 MamdaQuoteListenerImpl::FieldUpdateBidPrevCloseDate);

        initFieldUpdater(MamdaQuoteFields::BID_HIGH,
                         new MamdaQuoteListener::
		                 MamdaQuoteListenerImpl::FieldUpdateBidHigh);

        initFieldUpdater(MamdaQuoteFields::BID_LOW,
                         new MamdaQuoteListener::
		                 MamdaQuoteListenerImpl::FieldUpdateBidLow);

        initFieldUpdater(MamdaQuoteFields::ASK_PRICE,
                         new MamdaQuoteListener::
		                 MamdaQuoteListenerImpl::FieldUpdateAskPrice);

        initFieldUpdater(MamdaQuoteFields::ASK_SIZE,
                         new MamdaQuoteListener::
		                 MamdaQuoteListenerImpl::FieldUpdateAskSize);

        initFieldUpdater(MamdaQuoteFields::ASK_DEPTH,
                         new MamdaQuoteListener::
		                 MamdaQuoteListenerImpl::FieldUpdateAskDepth);

        initFieldUpdater(MamdaQuoteFields::ASK_PART_ID,
                         new MamdaQuoteListener::
		                 MamdaQuoteListenerImpl::FieldUpdateAskPartId);

        initFieldUpdater(MamdaQuoteFields::ASK_CLOSE_PRICE,
                         new MamdaQuoteListener::
		                 MamdaQuoteListenerImpl::FieldUpdateAskClosePrice);

        initFieldUpdater(MamdaQuoteFields::ASK_CLOSE_DATE,
                         new MamdaQuoteListener::
		                 MamdaQuoteListenerImpl::FieldUpdateAskCloseDate);

        initFieldUpdater(MamdaQuoteFields::ASK_PREV_CLOSE_PRICE,
                         new MamdaQuoteListener::
		                 MamdaQuoteListenerImpl::FieldUpdateAskPrevClosePrice);

        initFieldUpdater(MamdaQuoteFields::ASK_PREV_CLOSE_DATE,
                         new MamdaQuoteListener::
		                 MamdaQuoteListenerImpl::FieldUpdateAskPrevCloseDate);

        initFieldUpdater(MamdaQuoteFields::ASK_HIGH,
                         new MamdaQuoteListener::
		                 MamdaQuoteListenerImpl::FieldUpdateAskHigh);

        initFieldUpdater(MamdaQuoteFields::ASK_LOW,
                         new MamdaQuoteListener::
		                 MamdaQuoteListenerImpl::FieldUpdateAskLow);

        initFieldUpdater(MamdaQuoteFields::QUOTE_SEQ_NUM,
                         new MamdaQuoteListener::
		                 MamdaQuoteListenerImpl::FieldUpdateQuoteSeqNum);

        initFieldUpdater(MamdaQuoteFields::QUOTE_TIME,
                         new MamdaQuoteListener::
		                 MamdaQuoteListenerImpl::FieldUpdateQuoteTime);

        initFieldUpdater(MamdaQuoteFields::QUOTE_DATE,
                         new MamdaQuoteListener::
		                 MamdaQuoteListenerImpl::FieldUpdateQuoteDate);

        initFieldUpdater(MamdaQuoteFields::QUOTE_QUAL,
                         new MamdaQuoteListener::
		                 MamdaQuoteListenerImpl::FieldUpdateQuoteQual);

        initFieldUpdater(MamdaQuoteFields::QUOTE_QUAL_NATIVE,
                         new MamdaQuoteListener::
		                 MamdaQuoteListenerImpl::FieldUpdateQuoteQualNative);

        initFieldUpdater(MamdaQuoteFields::QUOTE_COUNT,
                         new MamdaQuoteListener::
		                 MamdaQuoteListenerImpl::FieldUpdateTmpQuoteCount);

        initFieldUpdater(MamdaQuoteFields::SHORT_SALE_BID_TICK,
                         new MamdaQuoteListener::
                         MamdaQuoteListenerImpl::FieldUpdateShortSaleBidTick);

        initFieldUpdater(MamdaQuoteFields::BID_TICK,
                         new MamdaQuoteListener::
                         MamdaQuoteListenerImpl::FieldUpdateBidTick);

        initFieldUpdater(MamdaQuoteFields::ASK_TIME,
                         new MamdaQuoteListener::
                         MamdaQuoteListenerImpl::FieldUpdateAskTime);

        initFieldUpdater(MamdaQuoteFields::BID_TIME,
                         new MamdaQuoteListener::
                         MamdaQuoteListenerImpl::FieldUpdateBidTime);

        initFieldUpdater(MamdaQuoteFields::ASK_INDICATOR,
                         new MamdaQuoteListener::
                         MamdaQuoteListenerImpl::FieldUpdateAskIndicator);

        initFieldUpdater(MamdaQuoteFields::BID_INDICATOR,
                         new MamdaQuoteListener::
                         MamdaQuoteListenerImpl::FieldUpdateBidIndicator);

        initFieldUpdater(MamdaQuoteFields::ASK_UPDATE_COUNT,
                         new MamdaQuoteListener::
                         MamdaQuoteListenerImpl::FieldUpdateAskUpdateCount);

        initFieldUpdater(MamdaQuoteFields::BID_UPDATE_COUNT,
                         new MamdaQuoteListener::
                         MamdaQuoteListenerImpl::FieldUpdateBidUpdateCount);

        initFieldUpdater(MamdaQuoteFields::ASK_YIELD,
                         new MamdaQuoteListener::
                         MamdaQuoteListenerImpl::FieldUpdateAskYield);

        initFieldUpdater(MamdaQuoteFields::BID_YIELD,
                         new MamdaQuoteListener::
                         MamdaQuoteListenerImpl::FieldUpdateBidYield);

        initFieldUpdater(MamdaQuoteFields::BID_SIZES_LIST,
                         new MamdaQuoteListener::
                         MamdaQuoteListenerImpl::FieldUpdateBidSizesList);

        initFieldUpdater(MamdaQuoteFields::ASK_SIZES_LIST,
                         new MamdaQuoteListener::
                         MamdaQuoteListenerImpl::FieldUpdateAskSizesList);

        initFieldUpdater(MamdaQuoteFields::SHORT_SALE_CIRCUIT_BREAKER,
                         new MamdaQuoteListener::
                         MamdaQuoteListenerImpl::FieldUpdateShortSaleCircuitBreaker);
    }

    void MamdaQuoteListener::MamdaQuoteListenerImpl::initFieldUpdater (
        const MamaFieldDescriptor*  fieldDesc,
        QuoteFieldUpdate*           updater)
    {
        if (!fieldDesc)
            return;

        uint16_t fid = fieldDesc->getFid();
        if (fid <= mFieldUpdatersSize)
        {
            mFieldUpdaters[fid] = updater;
        }
    }

    void MamdaQuoteListener::MamdaQuoteListenerImpl::assertEqual (
                MamdaQuoteListenerImpl& rhs)
    {
        if (mQuoteListenerCache.mSymbol != rhs.mQuoteListenerCache.mSymbol)
        {
            char msg[256];
            snprintf (msg, 256, "different symbols (%s != %s)",
                     mQuoteListenerCache.mSymbol.c_str(), 
                     rhs.mQuoteListenerCache.mSymbol.c_str());

            throw MamdaDataException (msg);
        }
        
        if (mQuoteListenerCache.mBidPrice != rhs.mQuoteListenerCache.mBidPrice)
        {
            char msg[256];
            snprintf (msg, 256, "different bidPrice (%s != %s)",
                     mQuoteListenerCache.mBidPrice.getAsString(),
                     rhs.mQuoteListenerCache.mBidPrice.getAsString());

            throw MamdaDataException (msg);
        }

        if (mQuoteListenerCache.mBidPartId != rhs.mQuoteListenerCache.mBidPartId)
        {
            char msg[256];
            snprintf (msg, 256, "different bidPardId (%s != %s)",
                     mQuoteListenerCache.mBidPartId.c_str(),
                     rhs.mQuoteListenerCache.mBidPartId.c_str());

            throw MamdaDataException (msg);
        }

        if (mQuoteListenerCache.mAskPrice != rhs.mQuoteListenerCache.mAskPrice)
        {   
            char msg[256];
            snprintf (msg, 256, "different askPrice (%s != %s)",
                     mQuoteListenerCache.mAskPrice.getAsString(),
                     rhs.mQuoteListenerCache.mAskPrice.getAsString());

            throw MamdaDataException (msg);             
        }
        
        if (mQuoteListenerCache.mAskSize != rhs.mQuoteListenerCache.mAskSize)
        {
            char msg[256];
            snprintf (msg, 256, "different askSize (%f != %f)",
                     mQuoteListenerCache.mAskSize,
                     rhs.mQuoteListenerCache.mAskSize);

            throw MamdaDataException (msg);             
        }

        if (mQuoteListenerCache.mBidSize != rhs.mQuoteListenerCache.mBidSize)
        {
            char msg[256];
            snprintf (msg, 256, "different bidSize (%f != %f)",
                     mQuoteListenerCache.mBidSize,
                     rhs.mQuoteListenerCache.mBidSize);

            throw MamdaDataException (msg);             
        }
        
        if (mQuoteListenerCache.mPartId != rhs.mQuoteListenerCache.mPartId)
        {
            char msg[256];
            snprintf (msg, 256, "different partId (%s != %s)",
                     mQuoteListenerCache.mPartId.c_str(),
                     rhs.mQuoteListenerCache.mPartId.c_str());

            throw MamdaDataException (msg);                     
        }
        
        if (mQuoteListenerCache.mPubId != rhs.mQuoteListenerCache.mPubId)
        {
            char msg[256];
            snprintf (msg, 256, "different pubId (%s != %s)",
                     mQuoteListenerCache.mPubId.c_str(),
                     rhs.mQuoteListenerCache.mPubId.c_str());

            throw MamdaDataException (msg);                     
        }
        
        if (mQuoteListenerCache.mBidDepth != rhs.mQuoteListenerCache.mBidDepth)
        {
            char msg[256];
            snprintf (msg, 256, "different bidDepth (%f != %f)",
                     mQuoteListenerCache.mBidDepth,
                     rhs.mQuoteListenerCache.mBidDepth);

            throw MamdaDataException (msg);             
        }    
       
        if (mQuoteListenerCache.mBidClosePrice != rhs.mQuoteListenerCache.mBidClosePrice)
        {
            char msg[256];
            snprintf (msg, 256, "different bidClosePrice (%s != %s)",
                     mQuoteListenerCache.mBidClosePrice.getAsString(),
                     rhs.mQuoteListenerCache.mBidClosePrice.getAsString());

            throw MamdaDataException (msg);             
        }
        
        if (mQuoteListenerCache.mBidCloseDate != rhs.mQuoteListenerCache.mBidCloseDate)
        {
            char msg[256];
            snprintf (msg, 256, "different bidCloseDate (%s != %s)",
                     mQuoteListenerCache.mBidCloseDate.getAsString(),
                     rhs.mQuoteListenerCache.mBidCloseDate.getAsString());

            throw MamdaDataException (msg);             
        }
        
        if (mQuoteListenerCache.mBidPrevClosePrice != rhs.mQuoteListenerCache.mBidPrevClosePrice)
        {
            char msg[256];
            snprintf (msg, 256, "different bidPrevClosePrice (%s != %s)",
                     mQuoteListenerCache.mBidPrevClosePrice.getAsString(),
                     rhs.mQuoteListenerCache.mBidPrevClosePrice.getAsString());

            throw MamdaDataException (msg);             
        }
        
        if (mQuoteListenerCache.mBidPrevCloseDate != rhs.mQuoteListenerCache.mBidPrevCloseDate)
        {
            char msg[256];
            snprintf (msg, 256, "different bidPrevCloseDate (%s != %s)",
                     mQuoteListenerCache.mBidPrevCloseDate.getAsString(),
                     rhs.mQuoteListenerCache.mBidPrevCloseDate.getAsString());

            throw MamdaDataException (msg);             
        }
        
        if (mQuoteListenerCache.mBidHigh != rhs.mQuoteListenerCache.mBidHigh)
        {
            char msg[256];
            snprintf (msg, 256, "different bidHigh (%s != %s)",
                     mQuoteListenerCache.mBidHigh.getAsString(),
                     rhs.mQuoteListenerCache.mBidHigh.getAsString());

            throw MamdaDataException (msg);
        }
        
        if (mQuoteListenerCache.mBidLow != rhs.mQuoteListenerCache.mBidLow)
        {
            char msg[256];
            snprintf (msg, 256, "different bidLow (%s != %s)",
                     mQuoteListenerCache.mBidLow.getAsString(),
                     rhs.mQuoteListenerCache.mBidLow.getAsString());

            throw MamdaDataException (msg);
        }
        
        if (mQuoteListenerCache.mAskDepth != rhs.mQuoteListenerCache.mAskDepth)
        {
            char msg[256];
            snprintf (msg, 256, "different askDepth (%f != %f)",
                     mQuoteListenerCache.mAskDepth,
                     rhs.mQuoteListenerCache.mAskDepth);

            throw MamdaDataException (msg);                     
        }
        
        if (mQuoteListenerCache.mAskClosePrice != rhs.mQuoteListenerCache.mAskClosePrice)
        {
            char msg[256];
            snprintf (msg, 256, "different askClosePrice (%s != %s)",
                     mQuoteListenerCache.mAskClosePrice.getAsString(),
                     rhs.mQuoteListenerCache.mAskClosePrice.getAsString());

            throw MamdaDataException (msg);        
        }
        
        if (mQuoteListenerCache.mAskCloseDate != rhs.mQuoteListenerCache.mAskCloseDate)
        {
            char msg[256];
            snprintf (msg, 256, "different askCloseDate (%s != %s)",
                     mQuoteListenerCache.mAskCloseDate.getAsString(),
                     rhs.mQuoteListenerCache.mAskCloseDate.getAsString());

            throw MamdaDataException (msg);        
        }
        
        if (mQuoteListenerCache.mAskPrevClosePrice != rhs.mQuoteListenerCache.mAskPrevClosePrice)
        {
            char msg[256];
            snprintf (msg, 256, "different askPrevClosePrice (%s != %s)",
                     mQuoteListenerCache.mAskPrevClosePrice.getAsString(),
                     rhs.mQuoteListenerCache.mAskPrevClosePrice.getAsString());

            throw MamdaDataException (msg);        
        }
        
        if (mQuoteListenerCache.mAskPrevCloseDate != rhs.mQuoteListenerCache.mAskPrevCloseDate)
        {
            char msg[256];
            snprintf (msg, 256, "different askPrevCloseDate (%s != %s)",
                     mQuoteListenerCache.mAskPrevCloseDate.getAsString(),
                     rhs.mQuoteListenerCache.mAskPrevCloseDate.getAsString());

            throw MamdaDataException (msg);        
        }
        
        if (mQuoteListenerCache.mAskHigh != rhs.mQuoteListenerCache.mAskHigh)
        {
            char msg[256];
            snprintf (msg, 256, "different askHigh (%s != %s)",
                     mQuoteListenerCache.mAskHigh.getAsString(),
                     rhs.mQuoteListenerCache.mAskHigh.getAsString());

            throw MamdaDataException (msg);        
        }
        
        if (mQuoteListenerCache.mAskLow != rhs.mQuoteListenerCache.mAskLow)
        {
            char msg[256];
            snprintf (msg, 256, "different askLow (%s != %s)",
                     mQuoteListenerCache.mAskLow.getAsString(),
                     rhs.mQuoteListenerCache.mAskLow.getAsString());

            throw MamdaDataException (msg);        
        }
        
        if (mQuoteListenerCache.mMidPrice != rhs.mQuoteListenerCache.mMidPrice)
        {
            char msg[256];
            snprintf (msg, 256, "different midPrice (%s != %s)",
                     mQuoteListenerCache.mMidPrice.getAsString(),
                     rhs.mQuoteListenerCache.mMidPrice.getAsString());

            throw MamdaDataException (msg);        
        }
        
        if (mQuoteListenerCache.mQuoteQualStr != rhs.mQuoteListenerCache.mQuoteQualStr)
        {
            char msg[256];
            snprintf (msg, 256, "different quoteQualStr (%s != %s)",
                     mQuoteListenerCache.mQuoteQualStr.c_str(),
                     rhs.mQuoteListenerCache.mQuoteQualStr.c_str());

            throw MamdaDataException (msg);        
        }
        
        if (mQuoteListenerCache.mQuoteQualNative != rhs.mQuoteListenerCache.mQuoteQualNative)
        {
            char msg[256];
            snprintf (msg, 256, "different quoteQualNative (%s != %s)",
                     mQuoteListenerCache.mQuoteQualNative.c_str(),
                     rhs.mQuoteListenerCache.mQuoteQualNative.c_str());

            throw MamdaDataException (msg);        
        }
        
        if (mQuoteListenerCache.mAskIndicator != rhs.mQuoteListenerCache.mAskIndicator)
        {
            char msg[256];
            snprintf (msg, 256, "different askIndicator (%s != %s)",
                     mQuoteListenerCache.mAskIndicator.c_str(),
                     rhs.mQuoteListenerCache.mAskIndicator.c_str());

            throw MamdaDataException (msg);                
        }
        
        if (mQuoteListenerCache.mBidIndicator != rhs.mQuoteListenerCache.mBidIndicator)
        {
            char msg[256];
            snprintf (msg, 256, "different bidIndicator (%s != %s)",
                     mQuoteListenerCache.mBidIndicator.c_str(),
                     rhs.mQuoteListenerCache.mBidIndicator.c_str());

            throw MamdaDataException (msg);                
        }
        
        if (mQuoteListenerCache.mAskUpdateCount != rhs.mQuoteListenerCache.mAskUpdateCount)
        {
            char msg[256];
            snprintf (msg, 256, "different askUpdateCount (%d != %d)",
                     mQuoteListenerCache.mAskUpdateCount,
                     rhs.mQuoteListenerCache.mAskUpdateCount);

            throw MamdaDataException (msg);                        
        }
        
        if (mQuoteListenerCache.mBidUpdateCount != rhs.mQuoteListenerCache.mBidUpdateCount)
        {
            char msg[256];
            snprintf (msg, 256, "different bidUpdateCount (%d != %d)",
                     mQuoteListenerCache.mBidUpdateCount,
                     rhs.mQuoteListenerCache.mBidUpdateCount);

            throw MamdaDataException (msg);                        
        }
        
        if (mQuoteListenerCache.mAskYield != rhs.mQuoteListenerCache.mAskYield)
        {
            char msg[256];
            snprintf (msg, 256, "different askYield (%f != %f)",
                     mQuoteListenerCache.mAskYield,
                     rhs.mQuoteListenerCache.mAskYield);

            throw MamdaDataException (msg);                                
        }
        
        if (mQuoteListenerCache.mBidYield != rhs.mQuoteListenerCache.mBidYield)
        {
            char msg[256];
            snprintf (msg, 256, "different bidYield (%f != %f)",
                     mQuoteListenerCache.mBidYield,
                     rhs.mQuoteListenerCache.mBidYield);

            throw MamdaDataException (msg);                                
        }
        
        if (mQuoteListenerCache.mTmpQuoteCount != rhs.mQuoteListenerCache.mTmpQuoteCount)
        {
            char msg[256];
            snprintf (msg, 256, "different tmpQuoteCount (%d != %d)",
                     mQuoteListenerCache.mTmpQuoteCount,
                     rhs.mQuoteListenerCache.mTmpQuoteCount);

            throw MamdaDataException (msg);        
        }
        
        if (mQuoteListenerCache.mQuoteCount != rhs.mQuoteListenerCache.mQuoteCount)
        {
            char msg[256];
            snprintf (msg, 256, "different quoteCount (%d != %d)",
                     mQuoteListenerCache.mQuoteCount,
                     rhs.mQuoteListenerCache.mQuoteCount);

            throw MamdaDataException (msg);        
        }
        
        if (mQuoteListenerCache.mGapBegin != rhs.mQuoteListenerCache.mGapBegin)
        {
            char msg[256];
            snprintf (msg, 256, "different gapBegin (%d != %d)",
                     mQuoteListenerCache.mGapBegin,
                     rhs.mQuoteListenerCache.mGapBegin);

            throw MamdaDataException (msg);        
        }
        
        if (mQuoteListenerCache.mGapEnd != rhs.mQuoteListenerCache.mGapEnd)
        {
            char msg[256];
            snprintf (msg, 256, "different gapEnd (%d != %d)",
                     mQuoteListenerCache.mGapEnd,
                     rhs.mQuoteListenerCache.mGapEnd);

            throw MamdaDataException (msg);        
        }
        
        if (mQuoteListenerCache.mAskSizesList != rhs.mQuoteListenerCache.mAskSizesList)
        {
          char msg[256];
          snprintf (msg, 256, "different askSizesList (%s != %s)",
                    mQuoteListenerCache.mAskSizesList.c_str(),
                    rhs.mQuoteListenerCache.mAskSizesList.c_str());

          throw MamdaDataException (msg);        
        }
        
        if (mQuoteListenerCache.mBidSizesList != rhs.mQuoteListenerCache.mBidSizesList)
        {
          char msg[256];
          snprintf (msg, 256, "different bidSizesList (%s != %s)",
                    mQuoteListenerCache.mBidSizesList.c_str(),
                    rhs.mQuoteListenerCache.mBidSizesList.c_str());

          throw MamdaDataException (msg);
        }
    }

} // namespace
