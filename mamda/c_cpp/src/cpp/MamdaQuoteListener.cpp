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

    struct QuoteCache
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

        void clearCache                   (QuoteCache& quoteCache);

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
        QuoteCache     mRegularCache;     // Regular update cache
        QuoteCache*    mTransientCache;   // Transient cache 
        QuoteCache&    mQuoteCache;       // Current cache in use
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
        return mImpl.mQuoteCache.mSymbol.c_str();
    }

    const char* MamdaQuoteListener::getPartId() const
    {
        return mImpl.mQuoteCache.mPartId.c_str();
    }

    const MamaDateTime& MamdaQuoteListener::getSrcTime() const
    {
        return mImpl.mQuoteCache.mSrcTime;
    }

    const MamaDateTime& MamdaQuoteListener::getActivityTime() const
    {
        return mImpl.mQuoteCache.mActTime;
    }

    const MamaDateTime& MamdaQuoteListener::getLineTime() const
    {
        return mImpl.mQuoteCache.mLineTime;
    }

    const MamaDateTime& MamdaQuoteListener::getSendTime() const
    {
        return mImpl.mQuoteCache.mSendTime;
    }

    const MamaMsgQual& MamdaQuoteListener::getMsgQual() const
    {
        return mImpl.mMsgQual;
    }

    const char* MamdaQuoteListener::getPubId() const
    {
        return mImpl.mQuoteCache.mPubId.c_str();
    }

    const MamaPrice& MamdaQuoteListener::getBidPrice() const
    {
        return mImpl.mQuoteCache.mBidPrice;
    }

    mama_quantity_t MamdaQuoteListener::getBidSize() const
    {
        return mImpl.mQuoteCache.mBidSize;
    }

    mama_quantity_t MamdaQuoteListener::getBidDepth() const
    {
        return mImpl.mQuoteCache.mBidDepth;
    }

    const char* MamdaQuoteListener::getBidPartId() const
    {
        return mImpl.mQuoteCache.mBidPartId.c_str();
    }

    const MamaPrice& MamdaQuoteListener::getBidClosePrice() const
    {
        return mImpl.mQuoteCache.mBidClosePrice;
    }

    const MamaDateTime& MamdaQuoteListener::getBidCloseDate() const
    {
        return mImpl.mQuoteCache.mBidCloseDate;
    }

    const MamaPrice& MamdaQuoteListener::getBidPrevClosePrice() const
    {
        return mImpl.mQuoteCache.mBidPrevClosePrice;
    }

    const MamaDateTime& MamdaQuoteListener::getBidPrevCloseDate() const
    {
        return mImpl.mQuoteCache.mBidPrevCloseDate;
    }

    const MamaPrice& MamdaQuoteListener::getBidHigh() const
    {
        return mImpl.mQuoteCache.mBidHigh;
    }

    const MamaPrice& MamdaQuoteListener::getBidLow() const
    {
        return mImpl.mQuoteCache.mBidLow;
    }

    const MamaPrice& MamdaQuoteListener::getAskPrice() const
    {
        return mImpl.mQuoteCache.mAskPrice;
    }

    mama_quantity_t MamdaQuoteListener::getAskSize() const
    {
        return mImpl.mQuoteCache.mAskSize;
    }

    mama_quantity_t MamdaQuoteListener::getAskDepth() const
    {
        return mImpl.mQuoteCache.mAskDepth;
    }

    const char* MamdaQuoteListener::getAskPartId() const
    {
        return mImpl.mQuoteCache.mAskPartId.c_str();
    }

    const MamaPrice& MamdaQuoteListener::getAskClosePrice() const
    {
        return mImpl.mQuoteCache.mAskClosePrice;
    }

    const MamaDateTime& MamdaQuoteListener::getAskCloseDate() const
    {
        return mImpl.mQuoteCache.mAskCloseDate;
    }

    const MamaPrice& MamdaQuoteListener::getAskPrevClosePrice() const
    {
        return mImpl.mQuoteCache.mAskPrevClosePrice;
    }

    const MamaDateTime& MamdaQuoteListener::getAskPrevCloseDate() const
    {
        return mImpl.mQuoteCache.mAskPrevCloseDate;
    }

    const MamaPrice& MamdaQuoteListener::getAskHigh() const
    {
        return mImpl.mQuoteCache.mAskHigh;
    }

    const MamaPrice& MamdaQuoteListener::getAskLow() const
    {
        return mImpl.mQuoteCache.mAskLow;
    }

    const MamaPrice& MamdaQuoteListener::getQuoteMidPrice() const
    {
        if ((mImpl.mQuoteCache.mBidPrice > 0.0) && 
            (mImpl.mQuoteCache.mAskPrice > 0.0))
        {
            mImpl.mQuoteCache.mMidPrice.set (
                (mImpl.mQuoteCache.mAskPrice.getValue() +
                 mImpl.mQuoteCache.mBidPrice.getValue()) / 2.0);
        }
        else
        {
            mImpl.mQuoteCache.mBidPrice.clear();
        }
        return mImpl.mQuoteCache.mMidPrice;
    }

    mama_u32_t MamdaQuoteListener::getQuoteCount() const
    {
        return mImpl.mQuoteCache.mQuoteCount;
    }

    const char* MamdaQuoteListener::getQuoteQualStr() const
    {
        return mImpl.mQuoteCache.mQuoteQualStr.c_str();
    }

    const char* MamdaQuoteListener::getQuoteQualNative() const
    {
        return mImpl.mQuoteCache.mQuoteQualNative.c_str();
    }

    const MamaDateTime&   MamdaQuoteListener::getAskTime()           const
    {
        return mImpl.mQuoteCache.mAskTime;
    }

    const MamaDateTime&   MamdaQuoteListener::getBidTime()           const
    {
        return mImpl.mQuoteCache.mBidTime;
    }

    const char*  MamdaQuoteListener::getAskIndicator()      const
    {
        return mImpl.mQuoteCache.mAskIndicator.c_str();
    }

    const char*  MamdaQuoteListener::getBidIndicator()      const
    {
        return mImpl.mQuoteCache.mBidIndicator.c_str();
    }

    mama_u32_t  MamdaQuoteListener::getAskUpdateCount()    const
    {
        return mImpl.mQuoteCache.mAskUpdateCount;
    }

    mama_u32_t  MamdaQuoteListener::getBidUpdateCount()    const
    {
        return mImpl.mQuoteCache.mBidUpdateCount;
    }

    double  MamdaQuoteListener::getAskYield()          const
    {
        return mImpl.mQuoteCache.mAskYield;
    }

    double  MamdaQuoteListener::getBidYield()          const
    {
        return mImpl.mQuoteCache.mBidYield;
    }

    mama_seqnum_t MamdaQuoteListener::getEventSeqNum() const
    {
        return mImpl.mQuoteCache.mEventSeqNum;
    }

    const MamaDateTime& MamdaQuoteListener::getEventTime() const
    {
        return mImpl.mQuoteCache.mEventTime;
    }

    const MamaDateTime& MamdaQuoteListener::getQuoteDate() const
    {
        return mImpl.mQuoteCache.mQuoteDate;
    }
    mama_seqnum_t MamdaQuoteListener::getBeginGapSeqNum() const
    {
        return mImpl.mQuoteCache.mGapBegin;
    }

    mama_seqnum_t MamdaQuoteListener::getEndGapSeqNum() const
    {
        return mImpl.mQuoteCache.mGapEnd;
    }

    char MamdaQuoteListener::getShortSaleBidTick () const
    {
        return mImpl.mQuoteCache.mShortSaleBidTick;
    }

    const char*  MamdaQuoteListener::getBidSizesList()      const
    {
      return mImpl.mQuoteCache.mBidSizesList.c_str();
    }

    const char*  MamdaQuoteListener::getAskSizesList()      const
    {
      return mImpl.mQuoteCache.mAskSizesList.c_str();
    }

    char  MamdaQuoteListener::getShortSaleCircuitBreaker()      const
    {
      return mImpl.mQuoteCache.mShortSaleCircuitBreaker;
    }

    void MamdaQuoteListener::assertEqual (MamdaQuoteListener* rhs)
    {
            mImpl.assertEqual (rhs->mImpl);
    }


    /* fieldState Accessors     */

    MamdaFieldState MamdaQuoteListener::getSymbolFieldState() const
    {
        return mImpl.mQuoteCache.mSymbolFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getPartIdFieldState() const
    {
       return mImpl.mQuoteCache.mPartIdFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getSrcTimeFieldState() const
    {
        return mImpl.mQuoteCache.mSrcTimeFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getActivityTimeFieldState() const
    {
        return mImpl.mQuoteCache.mActTimeFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getLineTimeFieldState() const
    {
        return mImpl.mQuoteCache.mLineTimeFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getSendTimeFieldState() const
    {
        return mImpl.mQuoteCache.mSendTimeFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getMsgQualFieldState() const
    {
        return mImpl.mMsgQualFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getPubIdFieldState() const
    {
        return mImpl.mQuoteCache.mPubIdFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getBidPriceFieldState () const
    {
        return mImpl.mQuoteCache.mBidPriceFieldState;
    }
     
    MamdaFieldState MamdaQuoteListener::getBidSizeFieldState() const
    {
        return mImpl.mQuoteCache.mBidSizeFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getBidDepthFieldState() const
    {
        return mImpl.mQuoteCache.mBidDepthFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getBidPartIdFieldState() const
    {
        return mImpl.mQuoteCache.mBidPartIdFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getBidClosePriceFieldState() const
    {
        return mImpl.mQuoteCache.mBidClosePriceFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getBidCloseDateFieldState() const
    {
        return mImpl.mQuoteCache.mBidCloseDateFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getBidPrevClosePriceFieldState() const
    {
        return mImpl.mQuoteCache.mBidPrevClosePriceFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getBidPrevCloseDateFieldState() const
    {
        return mImpl.mQuoteCache.mBidPrevCloseDateFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getBidHighFieldState() const
    {
        return mImpl.mQuoteCache.mBidHighFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getBidLowFieldState() const
    {
        return mImpl.mQuoteCache.mBidLowFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getAskPriceFieldState() const
    {
        return mImpl.mQuoteCache.mAskPriceFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getAskSizeFieldState() const
    {
        return mImpl.mQuoteCache.mAskSizeFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getAskDepthFieldState() const
    {
        return mImpl.mQuoteCache.mAskDepthFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getAskPartIdFieldState() const
    {
        return mImpl.mQuoteCache.mAskPartIdFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getAskClosePriceFieldState() const
    {
        return mImpl.mQuoteCache.mAskClosePriceFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getAskCloseDateFieldState() const
    {
        return mImpl.mQuoteCache.mAskCloseDateFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getAskPrevClosePriceFieldState() const
    {
        return mImpl.mQuoteCache.mAskPrevClosePriceFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getAskPrevCloseDateFieldState() const
    {
        return mImpl.mQuoteCache.mAskPrevCloseDateFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getAskHighFieldState() const
    {
        return mImpl.mQuoteCache.mAskHighFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getAskLowFieldState() const
    {
        return mImpl.mQuoteCache.mAskLowFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getQuoteMidPriceFieldState() const
    {
        return mImpl.mQuoteCache.mMidPriceFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getQuoteCountFieldState() const
    {
        return mImpl.mQuoteCache.mQuoteCountFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getQuoteQualStrFieldState() const
    {
        return mImpl.mQuoteCache.mQuoteQualStrFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getQuoteQualNativeFieldState() const
    {
        return mImpl.mQuoteCache.mQuoteQualNativeFieldState;
    }

    MamdaFieldState   MamdaQuoteListener::getAskTimeFieldState() const
    {
        return mImpl.mQuoteCache.mAskTimeFieldState;
    }

    MamdaFieldState   MamdaQuoteListener::getBidTimeFieldState() const
    {
        return mImpl.mQuoteCache.mBidTimeFieldState;
    }

    MamdaFieldState  MamdaQuoteListener::getAskIndicatorFieldState() const
    {
        return mImpl.mQuoteCache.mAskIndicatorFieldState;
    }

    MamdaFieldState  MamdaQuoteListener::getBidIndicatorFieldState() const
    {
        return mImpl.mQuoteCache.mBidIndicatorFieldState;
    }

    MamdaFieldState  MamdaQuoteListener::getAskUpdateCountFieldState() const
    {
        return mImpl.mQuoteCache.mAskUpdateCountFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getBidUpdateCountFieldState() const
    {
        return mImpl.mQuoteCache.mBidUpdateCountFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getAskYieldFieldState() const
    {
        return mImpl.mQuoteCache.mAskYieldFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getBidYieldFieldState() const
    {
        return mImpl.mQuoteCache.mBidYieldFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getEventSeqNumFieldState() const
    {
        return mImpl.mQuoteCache.mEventSeqNumFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getEventTimeFieldState() const
    {
        return mImpl.mQuoteCache.mEventTimeFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getQuoteDateFieldState() const
    {
        return mImpl.mQuoteCache.mQuoteDateFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getBeginGapSeqNumFieldState() const
    {
        return mImpl.mQuoteCache.mGapBeginFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getEndGapSeqNumFieldState() const
    {
        return mImpl.mQuoteCache.mGapEndFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getShortSaleBidTickFieldState () const
    {
        return mImpl.mQuoteCache.mShortSaleBidTickFieldState;
    }

    MamdaFieldState  MamdaQuoteListener::getBidSizesListFieldState() const
    {
        return mImpl.mQuoteCache.mBidSizesListFieldState;
    }

    MamdaFieldState  MamdaQuoteListener::getAskSizesListFieldState() const
    {
        return mImpl.mQuoteCache.mAskSizesListFieldState;
    }

    MamdaFieldState MamdaQuoteListener::getShortSaleCircuitBreakerFieldState() const
    {
        return mImpl.mQuoteCache.mShortSaleCircuitBreakerFieldState;
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
        , mQuoteCache                           (mRegularCache)
    {
        clearCache (mRegularCache);
    }

    void MamdaQuoteListener::MamdaQuoteListenerImpl::clearCache (
        QuoteCache& quoteCache)
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
                    mTransientCache = new QuoteCache();
                }
                mQuoteCache = *mTransientCache;
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
                mQuoteCache = mRegularCache;
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
            if (mQuoteCache.mQuoteQualStr.compare ("Closing") != 0)
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
                           mQuoteCache.mLastGenericMsgWasQuote);
        }

        if (mHandler)
        {
            if (mQuoteCache.mLastGenericMsgWasQuote)
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
                mQuoteCache.mLastGenericMsgWasQuote = false;
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

        if (mQuoteCache.mPartIdFieldState            == MODIFIED)
            mQuoteCache.mPartIdFieldState            = NOT_MODIFIED;

        if (mQuoteCache.mSrcTimeFieldState           == MODIFIED)
            mQuoteCache.mSrcTimeFieldState           = NOT_MODIFIED;

        if (mQuoteCache.mActTimeFieldState           == MODIFIED)
            mQuoteCache.mActTimeFieldState           = NOT_MODIFIED;

        if (mQuoteCache.mLineTimeFieldState          == MODIFIED)
            mQuoteCache.mLineTimeFieldState          = NOT_MODIFIED;

        if (mQuoteCache.mSendTimeFieldState          == MODIFIED)
            mQuoteCache.mSendTimeFieldState          = NOT_MODIFIED;

        if (mQuoteCache.mPubIdFieldState             == MODIFIED)
            mQuoteCache.mPubIdFieldState             = NOT_MODIFIED;

        if (mQuoteCache.mBidPriceFieldState          == MODIFIED)
            mQuoteCache.mBidPriceFieldState          = NOT_MODIFIED;

        if (mQuoteCache.mBidSizeFieldState           == MODIFIED)
            mQuoteCache.mBidSizeFieldState           = NOT_MODIFIED;

        if (mQuoteCache.mBidDepthFieldState          == MODIFIED)
            mQuoteCache.mBidDepthFieldState          = NOT_MODIFIED;

        if (mQuoteCache.mBidPartIdFieldState         == MODIFIED)
            mQuoteCache.mBidPartIdFieldState         = NOT_MODIFIED;

        if (mQuoteCache.mBidClosePriceFieldState     == MODIFIED)
            mQuoteCache.mBidClosePriceFieldState     = NOT_MODIFIED;

        if (mQuoteCache.mBidCloseDateFieldState      == MODIFIED)
            mQuoteCache.mBidCloseDateFieldState      = NOT_MODIFIED;

        if (mQuoteCache.mBidPrevClosePriceFieldState == MODIFIED)
            mQuoteCache.mBidPrevClosePriceFieldState = NOT_MODIFIED;

        if (mQuoteCache.mBidPrevCloseDateFieldState  == MODIFIED)
            mQuoteCache.mBidPrevCloseDateFieldState  = NOT_MODIFIED;

        if (mQuoteCache.mBidHighFieldState           == MODIFIED)
            mQuoteCache.mBidHighFieldState           = NOT_MODIFIED;

        if (mQuoteCache.mBidLowFieldState            == MODIFIED)
            mQuoteCache.mBidLowFieldState            = NOT_MODIFIED;

        if (mQuoteCache.mAskPriceFieldState          == MODIFIED)
            mQuoteCache.mAskPriceFieldState          = NOT_MODIFIED;

        if (mQuoteCache.mAskSizeFieldState           == MODIFIED)
            mQuoteCache.mAskSizeFieldState           = NOT_MODIFIED;

        if (mQuoteCache.mAskDepthFieldState          == MODIFIED)
            mQuoteCache.mAskDepthFieldState          = NOT_MODIFIED;

        if (mQuoteCache.mAskClosePriceFieldState     == MODIFIED)
            mQuoteCache.mAskClosePriceFieldState     = NOT_MODIFIED;

        if (mQuoteCache.mAskCloseDateFieldState      == MODIFIED)
            mQuoteCache.mAskCloseDateFieldState      = NOT_MODIFIED;

        if (mQuoteCache.mAskPrevClosePriceFieldState == MODIFIED)
            mQuoteCache.mAskPrevClosePriceFieldState = NOT_MODIFIED;

        if (mQuoteCache.mAskPrevCloseDateFieldState  == MODIFIED)
            mQuoteCache.mAskPrevCloseDateFieldState  = NOT_MODIFIED;

        if (mQuoteCache.mAskHighFieldState           == MODIFIED)
            mQuoteCache.mAskHighFieldState           = NOT_MODIFIED;

        if (mQuoteCache.mAskLowFieldState            == MODIFIED)
            mQuoteCache.mAskLowFieldState            = NOT_MODIFIED;

        if (mQuoteCache.mMidPriceFieldState          == MODIFIED)
            mQuoteCache.mMidPriceFieldState          = NOT_MODIFIED;

        if (mQuoteCache.mEventSeqNumFieldState       == MODIFIED)
            mQuoteCache.mEventSeqNumFieldState       = NOT_MODIFIED;

        if (mQuoteCache.mEventTimeFieldState         == MODIFIED)
            mQuoteCache.mEventTimeFieldState         = NOT_MODIFIED;

        if (mQuoteCache.mQuoteDateFieldState         == MODIFIED)
            mQuoteCache.mQuoteDateFieldState         = NOT_MODIFIED;

        if (mQuoteCache.mQuoteQualStrFieldState      == MODIFIED)
            mQuoteCache.mQuoteQualStrFieldState      = NOT_MODIFIED;

        if (mQuoteCache.mQuoteQualNativeFieldState   == MODIFIED)
            mQuoteCache.mQuoteQualNativeFieldState   = NOT_MODIFIED;

        if (mQuoteCache.mAskTimeFieldState           == MODIFIED)
            mQuoteCache.mAskTimeFieldState           = NOT_MODIFIED;

        if (mQuoteCache.mBidTimeFieldState           == MODIFIED)
            mQuoteCache.mBidTimeFieldState           = NOT_MODIFIED;

        if (mQuoteCache.mAskIndicatorFieldState      == MODIFIED)
            mQuoteCache.mAskIndicatorFieldState      = NOT_MODIFIED;

        if (mQuoteCache.mBidIndicatorFieldState      == MODIFIED)
            mQuoteCache.mBidIndicatorFieldState      = NOT_MODIFIED;

        if (mQuoteCache.mAskUpdateCountFieldState    == MODIFIED)
            mQuoteCache.mAskUpdateCountFieldState    = NOT_MODIFIED;

        if (mQuoteCache.mBidUpdateCountFieldState    == MODIFIED)
            mQuoteCache.mBidUpdateCountFieldState    = NOT_MODIFIED;

        if (mQuoteCache.mAskYieldFieldState          == MODIFIED)
            mQuoteCache.mAskYieldFieldState          = NOT_MODIFIED;

        if (mQuoteCache.mBidYieldFieldState          == MODIFIED)
            mQuoteCache.mBidYieldFieldState          = NOT_MODIFIED;

        if (mQuoteCache.mTmpQuoteCountFieldState     == MODIFIED)
            mQuoteCache.mTmpQuoteCountFieldState     = NOT_MODIFIED;

        if (mQuoteCache.mQuoteCountFieldState        == MODIFIED)
            mQuoteCache.mQuoteCountFieldState        = NOT_MODIFIED;

        if (mQuoteCache.mGapBeginFieldState          == MODIFIED)
            mQuoteCache.mGapBeginFieldState          = NOT_MODIFIED;

        if (mQuoteCache.mGapEndFieldState            == MODIFIED)
            mQuoteCache.mGapEndFieldState            = NOT_MODIFIED;

        if (mQuoteCache.mAskSizesListFieldState      == MODIFIED)
            mQuoteCache.mAskSizesListFieldState      = NOT_MODIFIED;

        if (mQuoteCache.mBidSizesListFieldState      == MODIFIED)
            mQuoteCache.mBidSizesListFieldState      = NOT_MODIFIED;

        if (mQuoteCache.mShortSaleCircuitBreakerFieldState == MODIFIED)
            mQuoteCache.mShortSaleCircuitBreakerFieldState = NOT_MODIFIED; 
    }

    void MamdaQuoteListener::MamdaQuoteListenerImpl::updateQuoteFields (
        const MamaMsg&  msg)
    {   
        const char* symbol = NULL;
        const char* partId = NULL;
        
        getSymbolAndPartId (msg, symbol, partId);

        if (symbol) 
        {
          mQuoteCache.mSymbol = symbol;
          mQuoteCache.mSymbolFieldState = MODIFIED;
        }

        if (partId) 
        {
          mQuoteCache.mPartId = partId;
          mQuoteCache.mPartIdFieldState = MODIFIED;
        }

        mQuoteCache.mLastGenericMsgWasQuote = false;
        mQuoteCache.mGotBidPrice   = false;
        mQuoteCache.mGotAskPrice   = false;
        mQuoteCache.mGotBidSize    = false;
        mQuoteCache.mGotAskSize    = false;
        mQuoteCache.mGotBidDepth   = false;
        mQuoteCache.mGotAskDepth   = false;
        mQuoteCache.mGotBidPartId  = false;
        mQuoteCache.mGotAskPartId  = false;
        mQuoteCache.mGotQuoteCount = false;
        
        
        // Iterate over all of the fields in the message.
        wthread_mutex_lock (&mQuoteUpdateLock.mQuoteUpdateLockMutex);
        msg.iterateFields (*this, NULL, NULL);
        
        wthread_mutex_unlock (&mQuoteUpdateLock.mQuoteUpdateLockMutex);
        
        // Check certain special fields.
        if (mQuoteCache.mGotBidSize    || mQuoteCache.mGotAskSize   ||
            mQuoteCache.mGotBidDepth   || mQuoteCache.mGotAskDepth  ||
            mQuoteCache.mGotBidPrice   || mQuoteCache.mGotAskPrice  ||
            mQuoteCache.mGotBidPartId  || mQuoteCache.mGotAskPartId)
        {
            mQuoteCache.mLastGenericMsgWasQuote = true;
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
            mQuoteCache.mQuoteCount = mQuoteCache.mTmpQuoteCount;
            return;
        }

        // Check number of quotes for gaps
        mama_u32_t quoteCount    = mQuoteCache.mTmpQuoteCount;
        mama_u32_t conflateCount = 0;

        if (!msg.tryU32 ("wConflateQuoteCount", 23, conflateCount))
        {
            conflateCount = 1;
        }

        if (checkForGap && (mQuoteCache.mGotQuoteCount))
        {
            if ((mQuoteCache.mQuoteCount > 0) && 
                (quoteCount > (mQuoteCache.mQuoteCount+conflateCount)))
            {
                mQuoteCache.mGapBegin   = mQuoteCache.mQuoteCount + conflateCount;
                mQuoteCache.mGapEnd     = quoteCount-1;
                mQuoteCache.mQuoteCount = quoteCount;

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
        if ( mQuoteCache.mGotQuoteCount && 
             quoteCount > 0 &&
             (quoteCount == mQuoteCache.mQuoteCount) )
        {
            mIgnoreUpdate = true;
        }

        mQuoteCache.mQuoteCount = quoteCount;
    }

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateQuoteSrcTime 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getDateTime(impl.mQuoteCache.mSrcTime);
            impl.mQuoteCache.mSrcTimeFieldState = MODIFIED;
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateQuoteSendTime
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getDateTime(impl.mQuoteCache.mSendTime);
            impl.mQuoteCache.mSendTimeFieldState = MODIFIED;
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateQuoteActTime 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getDateTime(impl.mQuoteCache.mActTime);
            impl.mQuoteCache.mActTimeFieldState = MODIFIED;
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateQuoteLineTime 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getDateTime(impl.mQuoteCache.mLineTime);
            impl.mQuoteCache.mLineTimeFieldState = MODIFIED;
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateQuotePubId 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mQuoteCache.mPubId = field.getString();
            impl.mQuoteCache.mPubIdFieldState = MODIFIED;
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
            if (impl.mQuoteCache.mBidPrice != bidPrice)
            {
                impl.mQuoteCache.mBidPrice = bidPrice;
                impl.mQuoteCache.mGotBidPrice = true;
                impl.mQuoteCache.mBidPriceFieldState = MODIFIED;
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
            if (impl.mQuoteCache.mBidSize != bidSize)
            {
                impl.mQuoteCache.mBidSize = bidSize;
                impl.mQuoteCache.mGotBidSize = true;
                impl.mQuoteCache.mBidSizeFieldState = MODIFIED;
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
            if (impl.mQuoteCache.mBidDepth != bidDepth)
            {
                impl.mQuoteCache.mBidDepth = bidDepth;
                impl.mQuoteCache.mGotBidDepth = true;
                impl.mQuoteCache.mBidDepthFieldState = MODIFIED;
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
            if (impl.mQuoteCache.mBidPartId != bidPartId)
            {
                impl.mQuoteCache.mBidPartId = bidPartId;
                impl.mQuoteCache.mGotBidPartId = true;
                impl.mQuoteCache.mBidPartIdFieldState = MODIFIED;
            }
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateBidClosePrice 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getPrice (impl.mQuoteCache.mBidClosePrice);
            impl.mQuoteCache.mBidClosePriceFieldState = MODIFIED;
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateBidCloseDate 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getDateTime(impl.mQuoteCache.mBidCloseDate);
            impl.mQuoteCache.mBidCloseDateFieldState = MODIFIED;
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateBidPrevClosePrice 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getPrice (impl.mQuoteCache.mBidPrevClosePrice);
            impl.mQuoteCache.mBidPrevClosePriceFieldState = MODIFIED;
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateBidPrevCloseDate 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getDateTime(impl.mQuoteCache.mBidPrevCloseDate);
            impl.mQuoteCache.mBidPrevCloseDateFieldState = MODIFIED;
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateBidHigh
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getPrice(impl.mQuoteCache.mBidHigh);
            impl.mQuoteCache.mBidHighFieldState = MODIFIED;
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateBidLow
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getPrice(impl.mQuoteCache.mBidLow);
            impl.mQuoteCache.mBidLowFieldState = MODIFIED;
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
            if (impl.mQuoteCache.mAskPrice != askPrice)
            {
                impl.mQuoteCache.mAskPrice = askPrice;
                impl.mQuoteCache.mGotAskPrice = true;
                impl.mQuoteCache.mAskPriceFieldState = MODIFIED;
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
            if (impl.mQuoteCache.mAskSize != askSize)
            {
                impl.mQuoteCache.mAskSize = askSize;
                impl.mQuoteCache.mGotAskSize = true;
                impl.mQuoteCache.mAskSizeFieldState = MODIFIED;
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
            if (impl.mQuoteCache.mAskDepth != askDepth)
            {
                impl.mQuoteCache.mAskDepth = askDepth;
                impl.mQuoteCache.mGotAskDepth = true;
                impl.mQuoteCache.mAskDepthFieldState = MODIFIED;
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
            if (impl.mQuoteCache.mAskPartId != askPartId)
            {
                impl.mQuoteCache.mAskPartId = askPartId;
                impl.mQuoteCache.mGotAskPartId = true;
                impl.mQuoteCache.mAskPartIdFieldState = MODIFIED;
            }
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateAskClosePrice 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getPrice (impl.mQuoteCache.mAskClosePrice);
            impl.mQuoteCache.mAskClosePriceFieldState = MODIFIED;
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateAskCloseDate 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getDateTime(impl.mQuoteCache.mAskCloseDate);
            impl.mQuoteCache.mAskCloseDateFieldState = MODIFIED;
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateAskPrevClosePrice 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getPrice (impl.mQuoteCache.mAskPrevClosePrice);
            impl.mQuoteCache.mAskPrevClosePriceFieldState = MODIFIED;
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateAskPrevCloseDate 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getDateTime(impl.mQuoteCache.mAskPrevCloseDate);
            impl.mQuoteCache.mAskPrevCloseDateFieldState = MODIFIED;
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateAskHigh
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getPrice(impl.mQuoteCache.mAskHigh);
            impl.mQuoteCache.mAskHighFieldState = MODIFIED;
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateAskLow
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getPrice(impl.mQuoteCache.mAskLow);
            impl.mQuoteCache.mAskLowFieldState = MODIFIED;
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateQuoteSeqNum 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mQuoteCache.mEventSeqNum = field.getU32();
            impl.mQuoteCache.mEventSeqNumFieldState = MODIFIED;
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateQuoteTime 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getDateTime(impl.mQuoteCache.mEventTime);
            impl.mQuoteCache.mEventTimeFieldState = MODIFIED;
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateQuoteDate 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getDateTime(impl.mQuoteCache.mQuoteDate);
            impl.mQuoteCache.mQuoteDateFieldState = MODIFIED;
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateTmpQuoteCount 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mQuoteCache.mTmpQuoteCount = field.getU32();
            impl.mQuoteCache.mGotQuoteCount = MODIFIED;
            impl.mQuoteCache.mTmpQuoteCountFieldState = MODIFIED;
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateQuoteQual 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mQuoteCache.mQuoteQualStr = field.getString();
            impl.mQuoteCache.mQuoteQualStrFieldState = MODIFIED;
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
                impl.mQuoteCache.mQuoteQualNative = field.getString();
                impl.mQuoteCache.mQuoteQualNativeFieldState = MODIFIED;
            }
            else if (field.getType () == MAMA_FIELD_TYPE_CHAR)
            {
                impl.mQuoteCache.mQuoteQualNative = field.getChar();
                impl.mQuoteCache.mQuoteQualNativeFieldState = MODIFIED;
            }
        }
    };


    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateAskTime 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getDateTime(impl.mQuoteCache.mAskTime);
            impl.mQuoteCache.mAskTimeFieldState = MODIFIED;
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateBidTime 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            field.getDateTime(impl.mQuoteCache.mBidTime);
            impl.mQuoteCache.mBidTimeFieldState = MODIFIED;
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
                impl.mQuoteCache.mBidIndicator = field.getString();
                impl.mQuoteCache.mBidIndicatorFieldState = MODIFIED;
            }
            else if (field.getType () == MAMA_FIELD_TYPE_CHAR)
            {
                impl.mQuoteCache.mBidIndicator = field.getChar();
                impl.mQuoteCache.mBidIndicatorFieldState = MODIFIED;
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
                impl.mQuoteCache.mAskIndicator = field.getString();
                impl.mQuoteCache.mAskIndicatorFieldState = MODIFIED;
            }
            else if (field.getType () == MAMA_FIELD_TYPE_CHAR)
            {
                impl.mQuoteCache.mAskIndicator = field.getChar();
                impl.mQuoteCache.mAskIndicatorFieldState = MODIFIED;
            }
        }
    };



    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateBidUpdateCount 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mQuoteCache.mBidUpdateCount = field.getU32();
            impl.mQuoteCache.mBidUpdateCountFieldState = MODIFIED;
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateAskUpdateCount 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mQuoteCache.mAskUpdateCount = field.getU32();
            impl.mQuoteCache.mAskUpdateCountFieldState = MODIFIED;
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateAskYield 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mQuoteCache.mAskYield = field.getF64();
            impl.mQuoteCache.mAskYieldFieldState = MODIFIED;
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateBidYield 
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {
            impl.mQuoteCache.mBidYield = field.getF64();
            impl.mQuoteCache.mBidYieldFieldState = MODIFIED;
        }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateBidSizesList 
        : public QuoteFieldUpdate
    {
      void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                     const MamaMsgField&                          field)
      {
          impl.mQuoteCache.mBidSizesList = field.getString();
          impl.mQuoteCache.mBidSizesListFieldState = MODIFIED;
      }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateAskSizesList 
        : public QuoteFieldUpdate
    {
      void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                     const MamaMsgField&                          field)
      {
        impl.mQuoteCache.mAskSizesList = field.getString();
        impl.mQuoteCache.mAskSizesListFieldState = MODIFIED;
      }
    };

    struct MamdaQuoteListener::MamdaQuoteListenerImpl::FieldUpdateShortSaleCircuitBreaker
        : public QuoteFieldUpdate
    {
        void onUpdate (MamdaQuoteListener::MamdaQuoteListenerImpl&  impl,
                       const MamaMsgField&                          field)
        {       
            if (field.getChar() != impl.mQuoteCache.mShortSaleCircuitBreaker)
            {
                impl.mQuoteCache.mShortSaleCircuitBreaker = field.getChar();
                impl.mQuoteCache.mShortSaleCircuitBreakerFieldState = MODIFIED;
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
                    impl.mQuoteCache.mShortSaleBidTick = field.getChar ();
                    break;
                case MAMA_FIELD_TYPE_STRING :
                    impl.mQuoteCache.mShortSaleBidTick = field.getString() [0];
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
                    impl.mQuoteCache.mShortSaleBidTick = field.getChar ();
                    break;
                case MAMA_FIELD_TYPE_STRING :
                    impl.mQuoteCache.mShortSaleBidTick = field.getString() [0];
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
        if (mQuoteCache.mSymbol != rhs.mQuoteCache.mSymbol)
        {
            char msg[256];
            snprintf (msg, 256, "different symbols (%s != %s)",
                     mQuoteCache.mSymbol.c_str(), 
                     rhs.mQuoteCache.mSymbol.c_str());

            throw MamdaDataException (msg);
        }
        
        if (mQuoteCache.mBidPrice != rhs.mQuoteCache.mBidPrice)
        {
            char msg[256];
            snprintf (msg, 256, "different bidPrice (%s != %s)",
                     mQuoteCache.mBidPrice.getAsString(),
                     rhs.mQuoteCache.mBidPrice.getAsString());

            throw MamdaDataException (msg);
        }

        if (mQuoteCache.mBidPartId != rhs.mQuoteCache.mBidPartId)
        {
            char msg[256];
            snprintf (msg, 256, "different bidPardId (%s != %s)",
                     mQuoteCache.mBidPartId.c_str(),
                     rhs.mQuoteCache.mBidPartId.c_str());

            throw MamdaDataException (msg);
        }

        if (mQuoteCache.mAskPrice != rhs.mQuoteCache.mAskPrice)
        {   
            char msg[256];
            snprintf (msg, 256, "different askPrice (%s != %s)",
                     mQuoteCache.mAskPrice.getAsString(),
                     rhs.mQuoteCache.mAskPrice.getAsString());

            throw MamdaDataException (msg);             
        }
        
        if (mQuoteCache.mAskSize != rhs.mQuoteCache.mAskSize)
        {
            char msg[256];
            snprintf (msg, 256, "different askSize (%f != %f)",
                     mQuoteCache.mAskSize,
                     rhs.mQuoteCache.mAskSize);

            throw MamdaDataException (msg);             
        }

        if (mQuoteCache.mBidSize != rhs.mQuoteCache.mBidSize)
        {
            char msg[256];
            snprintf (msg, 256, "different bidSize (%f != %f)",
                     mQuoteCache.mBidSize,
                     rhs.mQuoteCache.mBidSize);

            throw MamdaDataException (msg);             
        }
        
        if (mQuoteCache.mPartId != rhs.mQuoteCache.mPartId)
        {
            char msg[256];
            snprintf (msg, 256, "different partId (%s != %s)",
                     mQuoteCache.mPartId.c_str(),
                     rhs.mQuoteCache.mPartId.c_str());

            throw MamdaDataException (msg);                     
        }
        
        if (mQuoteCache.mPubId != rhs.mQuoteCache.mPubId)
        {
            char msg[256];
            snprintf (msg, 256, "different pubId (%s != %s)",
                     mQuoteCache.mPubId.c_str(),
                     rhs.mQuoteCache.mPubId.c_str());

            throw MamdaDataException (msg);                     
        }
        
        if (mQuoteCache.mBidDepth != rhs.mQuoteCache.mBidDepth)
        {
            char msg[256];
            snprintf (msg, 256, "different bidDepth (%f != %f)",
                     mQuoteCache.mBidDepth,
                     rhs.mQuoteCache.mBidDepth);

            throw MamdaDataException (msg);             
        }    
       
        if (mQuoteCache.mBidClosePrice != rhs.mQuoteCache.mBidClosePrice)
        {
            char msg[256];
            snprintf (msg, 256, "different bidClosePrice (%s != %s)",
                     mQuoteCache.mBidClosePrice.getAsString(),
                     rhs.mQuoteCache.mBidClosePrice.getAsString());

            throw MamdaDataException (msg);             
        }
        
        if (mQuoteCache.mBidCloseDate != rhs.mQuoteCache.mBidCloseDate)
        {
            char msg[256];
            snprintf (msg, 256, "different bidCloseDate (%s != %s)",
                     mQuoteCache.mBidCloseDate.getAsString(),
                     rhs.mQuoteCache.mBidCloseDate.getAsString());

            throw MamdaDataException (msg);             
        }
        
        if (mQuoteCache.mBidPrevClosePrice != rhs.mQuoteCache.mBidPrevClosePrice)
        {
            char msg[256];
            snprintf (msg, 256, "different bidPrevClosePrice (%s != %s)",
                     mQuoteCache.mBidPrevClosePrice.getAsString(),
                     rhs.mQuoteCache.mBidPrevClosePrice.getAsString());

            throw MamdaDataException (msg);             
        }
        
        if (mQuoteCache.mBidPrevCloseDate != rhs.mQuoteCache.mBidPrevCloseDate)
        {
            char msg[256];
            snprintf (msg, 256, "different bidPrevCloseDate (%s != %s)",
                     mQuoteCache.mBidPrevCloseDate.getAsString(),
                     rhs.mQuoteCache.mBidPrevCloseDate.getAsString());

            throw MamdaDataException (msg);             
        }
        
        if (mQuoteCache.mBidHigh != rhs.mQuoteCache.mBidHigh)
        {
            char msg[256];
            snprintf (msg, 256, "different bidHigh (%s != %s)",
                     mQuoteCache.mBidHigh.getAsString(),
                     rhs.mQuoteCache.mBidHigh.getAsString());

            throw MamdaDataException (msg);
        }
        
        if (mQuoteCache.mBidLow != rhs.mQuoteCache.mBidLow)
        {
            char msg[256];
            snprintf (msg, 256, "different bidLow (%s != %s)",
                     mQuoteCache.mBidLow.getAsString(),
                     rhs.mQuoteCache.mBidLow.getAsString());

            throw MamdaDataException (msg);
        }
        
        if (mQuoteCache.mAskDepth != rhs.mQuoteCache.mAskDepth)
        {
            char msg[256];
            snprintf (msg, 256, "different askDepth (%f != %f)",
                     mQuoteCache.mAskDepth,
                     rhs.mQuoteCache.mAskDepth);

            throw MamdaDataException (msg);                     
        }
        
        if (mQuoteCache.mAskClosePrice != rhs.mQuoteCache.mAskClosePrice)
        {
            char msg[256];
            snprintf (msg, 256, "different askClosePrice (%s != %s)",
                     mQuoteCache.mAskClosePrice.getAsString(),
                     rhs.mQuoteCache.mAskClosePrice.getAsString());

            throw MamdaDataException (msg);        
        }
        
        if (mQuoteCache.mAskCloseDate != rhs.mQuoteCache.mAskCloseDate)
        {
            char msg[256];
            snprintf (msg, 256, "different askCloseDate (%s != %s)",
                     mQuoteCache.mAskCloseDate.getAsString(),
                     rhs.mQuoteCache.mAskCloseDate.getAsString());

            throw MamdaDataException (msg);        
        }
        
        if (mQuoteCache.mAskPrevClosePrice != rhs.mQuoteCache.mAskPrevClosePrice)
        {
            char msg[256];
            snprintf (msg, 256, "different askPrevClosePrice (%s != %s)",
                     mQuoteCache.mAskPrevClosePrice.getAsString(),
                     rhs.mQuoteCache.mAskPrevClosePrice.getAsString());

            throw MamdaDataException (msg);        
        }
        
        if (mQuoteCache.mAskPrevCloseDate != rhs.mQuoteCache.mAskPrevCloseDate)
        {
            char msg[256];
            snprintf (msg, 256, "different askPrevCloseDate (%s != %s)",
                     mQuoteCache.mAskPrevCloseDate.getAsString(),
                     rhs.mQuoteCache.mAskPrevCloseDate.getAsString());

            throw MamdaDataException (msg);        
        }
        
        if (mQuoteCache.mAskHigh != rhs.mQuoteCache.mAskHigh)
        {
            char msg[256];
            snprintf (msg, 256, "different askHigh (%s != %s)",
                     mQuoteCache.mAskHigh.getAsString(),
                     rhs.mQuoteCache.mAskHigh.getAsString());

            throw MamdaDataException (msg);        
        }
        
        if (mQuoteCache.mAskLow != rhs.mQuoteCache.mAskLow)
        {
            char msg[256];
            snprintf (msg, 256, "different askLow (%s != %s)",
                     mQuoteCache.mAskLow.getAsString(),
                     rhs.mQuoteCache.mAskLow.getAsString());

            throw MamdaDataException (msg);        
        }
        
        if (mQuoteCache.mMidPrice != rhs.mQuoteCache.mMidPrice)
        {
            char msg[256];
            snprintf (msg, 256, "different midPrice (%s != %s)",
                     mQuoteCache.mMidPrice.getAsString(),
                     rhs.mQuoteCache.mMidPrice.getAsString());

            throw MamdaDataException (msg);        
        }
        
        if (mQuoteCache.mQuoteQualStr != rhs.mQuoteCache.mQuoteQualStr)
        {
            char msg[256];
            snprintf (msg, 256, "different quoteQualStr (%s != %s)",
                     mQuoteCache.mQuoteQualStr.c_str(),
                     rhs.mQuoteCache.mQuoteQualStr.c_str());

            throw MamdaDataException (msg);        
        }
        
        if (mQuoteCache.mQuoteQualNative != rhs.mQuoteCache.mQuoteQualNative)
        {
            char msg[256];
            snprintf (msg, 256, "different quoteQualNative (%s != %s)",
                     mQuoteCache.mQuoteQualNative.c_str(),
                     rhs.mQuoteCache.mQuoteQualNative.c_str());

            throw MamdaDataException (msg);        
        }
        
        if (mQuoteCache.mAskIndicator != rhs.mQuoteCache.mAskIndicator)
        {
            char msg[256];
            snprintf (msg, 256, "different askIndicator (%s != %s)",
                     mQuoteCache.mAskIndicator.c_str(),
                     rhs.mQuoteCache.mAskIndicator.c_str());

            throw MamdaDataException (msg);                
        }
        
        if (mQuoteCache.mBidIndicator != rhs.mQuoteCache.mBidIndicator)
        {
            char msg[256];
            snprintf (msg, 256, "different bidIndicator (%s != %s)",
                     mQuoteCache.mBidIndicator.c_str(),
                     rhs.mQuoteCache.mBidIndicator.c_str());

            throw MamdaDataException (msg);                
        }
        
        if (mQuoteCache.mAskUpdateCount != rhs.mQuoteCache.mAskUpdateCount)
        {
            char msg[256];
            snprintf (msg, 256, "different askUpdateCount (%d != %d)",
                     mQuoteCache.mAskUpdateCount,
                     rhs.mQuoteCache.mAskUpdateCount);

            throw MamdaDataException (msg);                        
        }
        
        if (mQuoteCache.mBidUpdateCount != rhs.mQuoteCache.mBidUpdateCount)
        {
            char msg[256];
            snprintf (msg, 256, "different bidUpdateCount (%d != %d)",
                     mQuoteCache.mBidUpdateCount,
                     rhs.mQuoteCache.mBidUpdateCount);

            throw MamdaDataException (msg);                        
        }
        
        if (mQuoteCache.mAskYield != rhs.mQuoteCache.mAskYield)
        {
            char msg[256];
            snprintf (msg, 256, "different askYield (%f != %f)",
                     mQuoteCache.mAskYield,
                     rhs.mQuoteCache.mAskYield);

            throw MamdaDataException (msg);                                
        }
        
        if (mQuoteCache.mBidYield != rhs.mQuoteCache.mBidYield)
        {
            char msg[256];
            snprintf (msg, 256, "different bidYield (%f != %f)",
                     mQuoteCache.mBidYield,
                     rhs.mQuoteCache.mBidYield);

            throw MamdaDataException (msg);                                
        }
        
        if (mQuoteCache.mTmpQuoteCount != rhs.mQuoteCache.mTmpQuoteCount)
        {
            char msg[256];
            snprintf (msg, 256, "different tmpQuoteCount (%d != %d)",
                     mQuoteCache.mTmpQuoteCount,
                     rhs.mQuoteCache.mTmpQuoteCount);

            throw MamdaDataException (msg);        
        }
        
        if (mQuoteCache.mQuoteCount != rhs.mQuoteCache.mQuoteCount)
        {
            char msg[256];
            snprintf (msg, 256, "different quoteCount (%d != %d)",
                     mQuoteCache.mQuoteCount,
                     rhs.mQuoteCache.mQuoteCount);

            throw MamdaDataException (msg);        
        }
        
        if (mQuoteCache.mGapBegin != rhs.mQuoteCache.mGapBegin)
        {
            char msg[256];
            snprintf (msg, 256, "different gapBegin (%d != %d)",
                     mQuoteCache.mGapBegin,
                     rhs.mQuoteCache.mGapBegin);

            throw MamdaDataException (msg);        
        }
        
        if (mQuoteCache.mGapEnd != rhs.mQuoteCache.mGapEnd)
        {
            char msg[256];
            snprintf (msg, 256, "different gapEnd (%d != %d)",
                     mQuoteCache.mGapEnd,
                     rhs.mQuoteCache.mGapEnd);

            throw MamdaDataException (msg);        
        }
        
        if (mQuoteCache.mAskSizesList != rhs.mQuoteCache.mAskSizesList)
        {
          char msg[256];
          snprintf (msg, 256, "different askSizesList (%s != %s)",
                    mQuoteCache.mAskSizesList.c_str(),
                    rhs.mQuoteCache.mAskSizesList.c_str());

          throw MamdaDataException (msg);        
        }
        
        if (mQuoteCache.mBidSizesList != rhs.mQuoteCache.mBidSizesList)
        {
          char msg[256];
          snprintf (msg, 256, "different bidSizesList (%s != %s)",
                    mQuoteCache.mBidSizesList.c_str(),
                    rhs.mQuoteCache.mBidSizesList.c_str());

          throw MamdaDataException (msg);
        }
    }

} // namespace
