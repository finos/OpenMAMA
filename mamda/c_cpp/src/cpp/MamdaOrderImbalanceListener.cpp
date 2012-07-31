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
#include <mama/mamacpp.h>
#include <mamda/MamdaOrderImbalanceListener.h>
#include <mamda/MamdaOrderImbalanceFields.h>
#include <mamda/MamdaOrderImbalanceHandler.h>
#include <mamda/MamdaOrderImbalanceType.h>
#include <mamda/MamdaDataException.h>
#include <mamda/MamdaCommonFields.h>
#include <mamda/MamdaSubscription.h>
#include "MamdaUtils.h"
#include <vector>
#include <string>

using namespace std;
using namespace Wombat;

namespace Wombat
{

    struct OrderImbalanceFieldUpdate
    {
        virtual void onUpdate (MamdaOrderImbalanceListener::MamdaOrderImbalanceListenerImpl&  impl,
                               const MamaMsgField&                                            field) = 0;
    };


    struct OrderImbalanceCache 
    {
        // The following fields are used for caching the offical order imbalance
        // and related fields.  These fields can be used by applications for
        // reference and will be passed for recaps.
        MamaPrice             mHighIndicationPrice;      MamdaFieldState     mHighIndicationPriceFieldState;
        MamaPrice             mLowIndicationPrice;       MamdaFieldState     mLowIndicationPriceFieldState;
        MamaPrice             mIndicationPrice;          MamdaFieldState     mIndicationPriceFieldState;
        int64_t               mBuyVolume;                MamdaFieldState     mBuyVolumeFieldState;
        int64_t               mSellVolume;               MamdaFieldState     mSellVolumeFieldState;
        int64_t               mMatchVolume;              MamdaFieldState     mMatchVolumeFieldState;
        string                mSecurityStatusQual;       MamdaFieldState     mSecurityStatusQualFieldState; 
        MamaPrice             mInsideMatchPrice;         MamdaFieldState     mInsideMatchPriceFieldState;
        MamaPrice             mFarClearingPrice;         MamdaFieldState     mFarClearingPriceFieldState;
        MamaPrice             mNearClearingPrice;        MamdaFieldState     mNearClearingPriceFieldState;
        char                  mNoClearingPrice;          MamdaFieldState     mNoClearingPriceFieldState;
        char                  mPriceVarInd;              MamdaFieldState     mPriceVarIndFieldState;
        char                  mCrossType;                MamdaFieldState     mCrossTypeFieldState;
        MamaDateTime          mEventTime;                MamdaFieldState     mEventTimeFieldState;
        mama_seqnum_t         mEventSeqNum;              MamdaFieldState     mEventSeqNumFieldState;
        MamaDateTime          mSrcTime;                  MamdaFieldState     mSrcTimeFieldState;
        MamaDateTime          mActTime;                  MamdaFieldState     mActTimeFieldState;
        mama_i32_t            mMsgType;                  MamdaFieldState     mMsgTypeFieldState;
        string                mIssueSymbol;              MamdaFieldState     mIssueSymbolFieldState;
        string                mPartId;                   MamdaFieldState     mPartIdFieldState;
        mama_seqnum_t         mSeqNum;                   MamdaFieldState     mSeqNumFieldState;
        string                mSecurityStatusOrig;       MamdaFieldState     mSecurityStatusOrigFieldState;
        MamaDateTime          mSecurityStatusTime;       MamdaFieldState     mSecurityStatusTimeFieldState;
        MamaDateTime          mAuctionTime;              MamdaFieldState     mAuctionTimeFieldState;
        MamaDateTime          mLineTime;                 MamdaFieldState     mLineTimeFieldState;
        MamaDateTime          mSendTime;                 MamdaFieldState     mSendTimeFieldState;
        string                mSymbol;                   MamdaFieldState     mSymbolFieldState;  
      

        bool                                          mIsOrderImbalance;
        MamdaOrderImbalanceType::OrderImbalanceType   mSecurityStatusQualValue;  
    };

    class MamdaOrderImbalanceListener::MamdaOrderImbalanceListenerImpl 
        : public MamaMsgFieldIterator
    {
    public:
        MamdaOrderImbalanceListenerImpl   (MamdaOrderImbalanceListener& listener); 
        ~MamdaOrderImbalanceListenerImpl  () {} 

        void clearCache                   (OrderImbalanceCache& cache); 

        void handleRecap                  (MamdaSubscription*   subscription,
                                           const MamaMsg&       msg); 

        void handleUpdate                 (MamdaSubscription*   subscription,
                                           const MamaMsg&       msg,
                                           mama_i32_t           msgType); 
       
        void onField                      (const MamaMsg&       msg,
                                           const MamaMsgField&  field,
                                           void*                closure);

        void invokeTransientHandler       (MamdaSubscription*   subscription,
                                           const MamaMsg&       msg); 

        bool evaluateMsgQual              (MamdaSubscription*   subscription,
                                           const MamaMsg&       msg); 

        void updateImbalaceOrderFields    (const MamaMsg&       msg); 

        void addHandler                   (MamdaOrderImbalanceHandler*       handler); 

        void updateFieldStates            ();

        typedef struct ImbalanceLock_
        {
           wthread_mutex_t   mImbalanceMutex;

        } ImbalanceLock;

        //thread locking    
        ImbalanceLock mImbalanceLock;

        MamdaOrderImbalanceListener& mListener;
        MamdaOrderImbalanceHandler*  mHandler;

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

        // The mUseTransientHandlers is used to determine whether or not the 
        // normal of transient handlers should be invoked for a message which 
        // is determined to be transient regardless of whether the transient
        // or regular cache is being used (mFilterTransient)
        bool           mUsePosDupAndOutOfSeqHandlers;

        // Used to record whether or not a message has been 
        // determined to be transient.
        bool           mIsTransientMsg;

        // The Order ImbalanceCacher Data Caches
        OrderImbalanceCache     mRegularCache;          // Regular update cache
        OrderImbalanceCache*    mTransientCache;        // Transient cache 
        OrderImbalanceCache&    mOrderImbalanceCache;   // Current cache in use

        // Message Qualifier - holds information provides in formation 
        // regarding duplicate and delayed status of message.
        MamaMsgQual   mMsgQual;     MamdaFieldState mMsgQualFieldState;

        static void initFieldUpdaters (); 
        static void initFieldUpdater  (const MamaFieldDescriptor*  fieldDesc,
                                       OrderImbalanceFieldUpdate*  updater); 
       

        static OrderImbalanceFieldUpdate**  mFieldUpdaters;
        static volatile uint16_t            mFieldUpdatersSize;
        static wthread_mutex_t              mImbalanceFieldUpdaterLockMutex;
        static bool                         mUpdatersComplete;

        struct FieldUpdateHighIndicationPrice;
        struct FieldUpdateLowIndicationPrice;
        struct FieldUpdateIndicationPrice;
        struct FieldUpdateBuyVolume;
        struct FieldUpdateSellVolume;
        struct FieldUpdateMatchVolume;
        struct FieldUpdateSecurityStatusQual;
        struct FieldUpdateMatchPrice;
        struct FieldUpdateFarClearingPrice;
        struct FieldUpdateNearClearingPrice;
        struct FieldUpdateNoClearingPrice;
        struct FieldUpdatePriceVarInd;
        struct FieldUpdateCrossType;
        struct FieldUpdateEventTime;
        struct FieldUpdateEventSeqNum;
        struct FieldUpdateSrcTime;
        struct FieldUpdateActTime;
        struct FieldUpdateMsgType;
        struct FieldUpdateIssueSymbol;
        struct FieldUpdatePartId;
        struct FieldUpdateSeqNum;
        struct FieldUpdateSecurityStatusOrig;
        struct FieldUpdateSecurityStatusTime;
        struct FieldUpdateAuctionTime;
        struct FieldUpdateLineTime;
        struct FieldUpdateSendTime;

        typedef vector<MamdaOrderImbalanceHandler*> HandlersList;
        HandlersList mHandlersList; 
        HandlersList::iterator mHandlersListIterator;
        
    private:
        void handleNoOrderImbalance (MamdaSubscription* subscription,
                                     const MamaMsg&     msg); 

        void handleOrderImbalance   (MamdaSubscription* subscription,
                                     const MamaMsg&     msg); 

        bool isImbalanceType        (const char*        securityStatus);  
    };


    /**IMPLEMENTATIONS*/

    MamdaOrderImbalanceListener::MamdaOrderImbalanceListener ()
        : mImpl (*new MamdaOrderImbalanceListenerImpl(*this))
    {
        wthread_mutex_init (&mImpl.mImbalanceLock.mImbalanceMutex, NULL);
    }

    MamdaOrderImbalanceListener::~MamdaOrderImbalanceListener ()
    {
        wthread_mutex_destroy (&mImpl.mImbalanceLock.mImbalanceMutex);
	    /* Do not call wthread_mutex_destroy for the FieldUpdaterLockMutex here.  
	       If we do, it will not be initialized again if another listener is created 
	       after the first is destroyed. */
        /* wthread_mutex_destroy (&mImpl.mImbalanceFieldUpdaterLockMutex); */
        delete &mImpl;
    }

    void MamdaOrderImbalanceListener::addHandler (MamdaOrderImbalanceHandler*  handler)
    {
          mImpl.mHandler = handler;
    }

    MamdaOrderImbalanceListener::MamdaOrderImbalanceListenerImpl::MamdaOrderImbalanceListenerImpl (
        MamdaOrderImbalanceListener&  listener)
        : mListener                             (listener)
        , mHandler                              (NULL)
        , mTransientCache                       (NULL)
        , mOrderImbalanceCache                  (mRegularCache)
        , mUsePosDupAndOutOfSeqHandlers         (false)
        , mMsgQualFieldState                    (NOT_MODIFIED)
    {
        clearCache (mRegularCache);
    }

    void MamdaOrderImbalanceListener::MamdaOrderImbalanceListenerImpl::clearCache (
        OrderImbalanceCache& imbalanceCache)
    {
       imbalanceCache.mHighIndicationPrice.clear ();
       imbalanceCache.mLowIndicationPrice.clear  ();
       imbalanceCache.mIndicationPrice.clear     ();
       imbalanceCache.mBuyVolume                 = 0;
       imbalanceCache.mSellVolume                = 0;
       imbalanceCache.mMatchVolume               = 0;
       imbalanceCache.mSecurityStatusQual        = "";
       imbalanceCache.mSecurityStatusQualValue   = MamdaOrderImbalanceType::UNKNOWN;
       imbalanceCache.mInsideMatchPrice.clear    ();
       imbalanceCache.mFarClearingPrice.clear    ();
       imbalanceCache.mNearClearingPrice.clear   ();
       imbalanceCache.mNoClearingPrice           = 0;
       imbalanceCache.mPriceVarInd               = 0;
       imbalanceCache.mCrossType                 = 0;
       imbalanceCache.mEventTime.clear           ();
       imbalanceCache.mEventSeqNum               = 0;
       imbalanceCache.mSrcTime.clear             ();
       imbalanceCache.mActTime.clear             ();
       imbalanceCache.mMsgType                   = 0;
       imbalanceCache.mIssueSymbol               = "";
       imbalanceCache.mPartId                    = "";
       imbalanceCache.mSeqNum                    = 0;
       imbalanceCache.mSecurityStatusOrig        = "";
       imbalanceCache.mSecurityStatusTime.clear  ();
       imbalanceCache.mAuctionTime.clear         ();
       imbalanceCache.mLineTime.clear            ();
       imbalanceCache.mSendTime.clear            ();
       imbalanceCache.mSymbol                    = "";
       
       imbalanceCache.mHighIndicationPriceFieldState    = NOT_INITIALISED;
       imbalanceCache.mLowIndicationPriceFieldState     = NOT_INITIALISED;
       imbalanceCache.mIndicationPriceFieldState        = NOT_INITIALISED;
       imbalanceCache.mBuyVolumeFieldState              = NOT_INITIALISED;
       imbalanceCache.mSellVolumeFieldState             = NOT_INITIALISED;
       imbalanceCache.mMatchVolumeFieldState            = NOT_INITIALISED;
       imbalanceCache.mSecurityStatusQualFieldState     = NOT_INITIALISED;
       imbalanceCache.mInsideMatchPriceFieldState       = NOT_INITIALISED;
       imbalanceCache.mFarClearingPriceFieldState       = NOT_INITIALISED;
       imbalanceCache.mNearClearingPriceFieldState      = NOT_INITIALISED;
       imbalanceCache.mNoClearingPriceFieldState        = NOT_INITIALISED;
       imbalanceCache.mPriceVarIndFieldState            = NOT_INITIALISED;
       imbalanceCache.mCrossTypeFieldState              = NOT_INITIALISED;
       imbalanceCache.mEventTimeFieldState              = NOT_INITIALISED;
       imbalanceCache.mEventSeqNumFieldState            = NOT_INITIALISED;
       imbalanceCache.mSrcTimeFieldState                = NOT_INITIALISED;
       imbalanceCache.mActTimeFieldState                = NOT_INITIALISED;
       imbalanceCache.mMsgTypeFieldState                = NOT_INITIALISED;
       imbalanceCache.mIssueSymbolFieldState            = NOT_INITIALISED;
       imbalanceCache.mPartIdFieldState                 = NOT_INITIALISED;
       imbalanceCache.mSeqNumFieldState                 = NOT_INITIALISED;
       imbalanceCache.mSecurityStatusOrigFieldState     = NOT_INITIALISED;
       imbalanceCache.mSecurityStatusTimeFieldState     = NOT_INITIALISED;
       imbalanceCache.mAuctionTimeFieldState            = NOT_INITIALISED;
       imbalanceCache.mLineTimeFieldState               = NOT_INITIALISED;
       imbalanceCache.mSendTimeFieldState               = NOT_INITIALISED;
       imbalanceCache.mSymbolFieldState                 = NOT_INITIALISED;  
    }

    MamaPrice& MamdaOrderImbalanceListener::getHighIndicationPrice () const
    {
        return mImpl.mOrderImbalanceCache.mHighIndicationPrice;
    }

    MamaPrice& MamdaOrderImbalanceListener::getLowIndicationPrice () const
    {
        return mImpl.mOrderImbalanceCache.mLowIndicationPrice;
    }

    MamaPrice& MamdaOrderImbalanceListener::getImbalancePrice () const
    {
        return mImpl.mOrderImbalanceCache.mIndicationPrice;
    }

    int64_t MamdaOrderImbalanceListener::getBuyVolume () const
    {
        return mImpl.mOrderImbalanceCache.mBuyVolume;
    }

    int64_t MamdaOrderImbalanceListener::getSellVolume () const
    {
        return mImpl.mOrderImbalanceCache.mSellVolume;
    }

    int64_t MamdaOrderImbalanceListener::getMatchVolume () const
    {
        return mImpl.mOrderImbalanceCache.mMatchVolume;
    }

    const char* MamdaOrderImbalanceListener::getImbalanceState () const
    {
        return mImpl.mOrderImbalanceCache.mSecurityStatusQual.c_str();
    }

    MamaPrice&  MamdaOrderImbalanceListener::getMatchPrice () const
    {
        return mImpl.mOrderImbalanceCache.mInsideMatchPrice;
    }

    MamaPrice&  MamdaOrderImbalanceListener::getFarClearingPrice () const
    {
        return mImpl.mOrderImbalanceCache.mFarClearingPrice;
    }

    MamaPrice&  MamdaOrderImbalanceListener::getNearClearingPrice () const
    {
        return mImpl.mOrderImbalanceCache.mNearClearingPrice;
    }

    char  MamdaOrderImbalanceListener::getNoClearingPrice () const
    {
        return mImpl.mOrderImbalanceCache.mNoClearingPrice;
    }

    char  MamdaOrderImbalanceListener::getPriceVarInd  () const
    {
        return mImpl.mOrderImbalanceCache.mPriceVarInd;
    }

    char  MamdaOrderImbalanceListener::getCrossType () const
    {
        return mImpl.mOrderImbalanceCache.mCrossType;
    }

    MamaDateTime& MamdaOrderImbalanceListener::getEventTime  () const
    {   
        return mImpl.mOrderImbalanceCache.mEventTime;
    }

    mama_seqnum_t MamdaOrderImbalanceListener::getEventSeqNum () const
    {
        return mImpl.mOrderImbalanceCache.mEventSeqNum;
    }

    MamaDateTime& MamdaOrderImbalanceListener::getSrcTime  () const
    {
        return mImpl.mOrderImbalanceCache.mSrcTime;
    }
    MamaDateTime& MamdaOrderImbalanceListener::getActivityTime () const
    {
        return mImpl.mOrderImbalanceCache.mActTime;
    }
       
    mama_i32_t  MamdaOrderImbalanceListener::getMsgType () const
    {
        return mImpl.mOrderImbalanceCache.mMsgType;
    }

    const char*  MamdaOrderImbalanceListener::getIssueSymbol () const
    {
        return mImpl.mOrderImbalanceCache.mIssueSymbol.c_str();
    }

    const char*  MamdaOrderImbalanceListener::getPartId () const
    {
        return mImpl.mOrderImbalanceCache.mPartId.c_str();
    }

    mama_seqnum_t MamdaOrderImbalanceListener::getSeqNum () const
    {
        return mImpl.mOrderImbalanceCache.mSeqNum;
    }

    const char*   MamdaOrderImbalanceListener::getSecurityStatusOrig () const
    {
        return mImpl.mOrderImbalanceCache.mSecurityStatusOrig.c_str();
    }

    MamaDateTime& MamdaOrderImbalanceListener::getSecurityStatusTime () const
    {
        return mImpl.mOrderImbalanceCache.mSecurityStatusTime;
    }

    MamaDateTime&  MamdaOrderImbalanceListener::getAuctionTime () const
    {
        return mImpl.mOrderImbalanceCache.mAuctionTime;
    }

    MamaDateTime&  MamdaOrderImbalanceListener::getLineTime () const
    {
        return mImpl.mOrderImbalanceCache.mLineTime;
    }

    MamaDateTime&  MamdaOrderImbalanceListener::getSendTime () const
    {
        return mImpl.mOrderImbalanceCache.mSendTime;
    }

    const char*   MamdaOrderImbalanceListener::getSymbol () const
    {
        return mImpl.mOrderImbalanceCache.mSymbol.c_str();
    }

    /*      FieldState Accessors        */
    MamdaFieldState MamdaOrderImbalanceListener::getHighIndicationPriceFieldState() const
    {
      return mImpl.mOrderImbalanceCache.mHighIndicationPriceFieldState;
    }

    MamdaFieldState MamdaOrderImbalanceListener::getLowIndicationPriceFieldState() const
    {
      return mImpl.mOrderImbalanceCache.mLowIndicationPriceFieldState;
    }

    MamdaFieldState MamdaOrderImbalanceListener::getImbalancePriceFieldState() const
    {
      return mImpl.mOrderImbalanceCache.mIndicationPriceFieldState;
    }

    MamdaFieldState MamdaOrderImbalanceListener::getBuyVolumeFieldState() const
    {
      return mImpl.mOrderImbalanceCache.mBuyVolumeFieldState;
    }

    MamdaFieldState MamdaOrderImbalanceListener::getSellVolumeFieldState() const
    {
      return mImpl.mOrderImbalanceCache.mSellVolumeFieldState;
    }

    MamdaFieldState MamdaOrderImbalanceListener::getMatchVolumeFieldState() const
    {
      return mImpl.mOrderImbalanceCache.mMatchVolumeFieldState;
    }

    MamdaFieldState MamdaOrderImbalanceListener::getImbalanceStateFieldState() const
    {
      return mImpl.mOrderImbalanceCache.mSecurityStatusQualFieldState;
    }

    MamdaFieldState  MamdaOrderImbalanceListener::getMatchPriceFieldState() const
    {
      return mImpl.mOrderImbalanceCache.mInsideMatchPriceFieldState;
    }

    MamdaFieldState  MamdaOrderImbalanceListener::getFarClearingPriceFieldState() const
    {
      return mImpl.mOrderImbalanceCache.mFarClearingPriceFieldState;
    }

    MamdaFieldState  MamdaOrderImbalanceListener::getNearClearingPriceFieldState() const
    {
      return mImpl.mOrderImbalanceCache.mNearClearingPriceFieldState;
    }

    MamdaFieldState  MamdaOrderImbalanceListener::getNoClearingPriceFieldState() const
    {
      return mImpl.mOrderImbalanceCache.mNoClearingPriceFieldState;
    }

    MamdaFieldState  MamdaOrderImbalanceListener::getPriceVarIndFieldState() const
    {
      return mImpl.mOrderImbalanceCache.mPriceVarIndFieldState;
    }

    MamdaFieldState  MamdaOrderImbalanceListener::getCrossTypeFieldState() const
    {
      return mImpl.mOrderImbalanceCache.mCrossTypeFieldState;
    }

    MamdaFieldState MamdaOrderImbalanceListener::getEventTimeFieldState() const
    {   
      return mImpl.mOrderImbalanceCache.mEventTimeFieldState;
    }

    MamdaFieldState MamdaOrderImbalanceListener::getEventSeqNumFieldState() const
    {
      return mImpl.mOrderImbalanceCache.mEventSeqNumFieldState;
    }

    MamdaFieldState MamdaOrderImbalanceListener::getSrcTimeFieldState() const
    {
      return mImpl.mOrderImbalanceCache.mSrcTimeFieldState;
    }

    MamdaFieldState MamdaOrderImbalanceListener::getActivityTimeFieldState() const
    {
      return mImpl.mOrderImbalanceCache.mActTimeFieldState;
    }

    MamdaFieldState  MamdaOrderImbalanceListener::getMsgTypeFieldState() const
    {
      return mImpl.mOrderImbalanceCache.mMsgTypeFieldState;
    }

    MamdaFieldState  MamdaOrderImbalanceListener::getIssueSymbolFieldState() const
    {
      return mImpl.mOrderImbalanceCache.mIssueSymbolFieldState;
    }

    MamdaFieldState  MamdaOrderImbalanceListener::getPartIdFieldState() const
    {
      return mImpl.mOrderImbalanceCache.mPartIdFieldState;
    }

    MamdaFieldState MamdaOrderImbalanceListener::getSeqNumFieldState() const
    {
      return mImpl.mOrderImbalanceCache.mSeqNumFieldState;
    }

    MamdaFieldState   MamdaOrderImbalanceListener::getSecurityStatusOrigFieldState() const
    {
      return mImpl.mOrderImbalanceCache.mSecurityStatusOrigFieldState;
    }

    MamdaFieldState MamdaOrderImbalanceListener::getSecurityStatusTimeFieldState() const
    {
      return mImpl.mOrderImbalanceCache.mSecurityStatusTimeFieldState;
    }

    MamdaFieldState  MamdaOrderImbalanceListener::getAuctionTimeFieldState() const
    {
      return mImpl.mOrderImbalanceCache.mAuctionTimeFieldState;
    }

    MamdaFieldState  MamdaOrderImbalanceListener::getLineTimeFieldState() const
    {
      return mImpl.mOrderImbalanceCache.mLineTimeFieldState;
    }

    MamdaFieldState  MamdaOrderImbalanceListener::getSendTimeFieldState() const
    {
      return mImpl.mOrderImbalanceCache.mSendTimeFieldState;
    }

    MamdaFieldState   MamdaOrderImbalanceListener::getSymbolFieldState() const
    {
      return mImpl.mOrderImbalanceCache.mSymbolFieldState;
    }

    /**
     * Implementation of MamdaListener interface.
     */
    void MamdaOrderImbalanceListener::onMsg (MamdaSubscription* subscription,
                                             const MamaMsg&     msg,
                                             short              msgType)
    {               
       switch (msgType)
       {       
       case MAMA_MSG_TYPE_INITIAL:
       case MAMA_MSG_TYPE_RECAP:
       case MAMA_MSG_TYPE_UPDATE:
            {
                 mImpl.handleUpdate (subscription,msg,msgType);
            }
       break;
       default:
       break;
       }
    }

    void MamdaOrderImbalanceListener::MamdaOrderImbalanceListenerImpl::updateFieldStates()
    {
        if (mOrderImbalanceCache.mHighIndicationPriceFieldState  == MODIFIED)
            mOrderImbalanceCache.mHighIndicationPriceFieldState  =  NOT_MODIFIED;

        if (mOrderImbalanceCache.mLowIndicationPriceFieldState  == MODIFIED) 
            mOrderImbalanceCache.mLowIndicationPriceFieldState  =  NOT_MODIFIED;

        if (mOrderImbalanceCache.mIndicationPriceFieldState  == MODIFIED)    
            mOrderImbalanceCache.mIndicationPriceFieldState  =  NOT_MODIFIED;

        if (mOrderImbalanceCache.mBuyVolumeFieldState  == MODIFIED)        
            mOrderImbalanceCache.mBuyVolumeFieldState  =  NOT_MODIFIED;

        if (mOrderImbalanceCache.mSellVolumeFieldState  == MODIFIED) 
            mOrderImbalanceCache.mSellVolumeFieldState  =  NOT_MODIFIED;

        if (mOrderImbalanceCache.mMatchVolumeFieldState  == MODIFIED) 
            mOrderImbalanceCache.mMatchVolumeFieldState  =  NOT_MODIFIED;

        if (mOrderImbalanceCache.mSecurityStatusQualFieldState  == MODIFIED) 
            mOrderImbalanceCache.mSecurityStatusQualFieldState  =  NOT_MODIFIED;

        if (mOrderImbalanceCache.mInsideMatchPriceFieldState  == MODIFIED)   
            mOrderImbalanceCache.mInsideMatchPriceFieldState  =  NOT_MODIFIED;

        if (mOrderImbalanceCache.mFarClearingPriceFieldState  == MODIFIED)  
            mOrderImbalanceCache.mFarClearingPriceFieldState  =  NOT_MODIFIED;

        if (mOrderImbalanceCache.mNearClearingPriceFieldState  == MODIFIED) 
            mOrderImbalanceCache.mNearClearingPriceFieldState  =  NOT_MODIFIED;

        if (mOrderImbalanceCache.mNoClearingPriceFieldState  == MODIFIED)    
            mOrderImbalanceCache.mNoClearingPriceFieldState  =  NOT_MODIFIED;

        if (mOrderImbalanceCache.mPriceVarIndFieldState  == MODIFIED)      
            mOrderImbalanceCache.mPriceVarIndFieldState  =  NOT_MODIFIED;

        if (mOrderImbalanceCache.mCrossTypeFieldState  == MODIFIED)    
            mOrderImbalanceCache.mCrossTypeFieldState  =  NOT_MODIFIED;

        if (mOrderImbalanceCache.mEventTimeFieldState  == MODIFIED)  
            mOrderImbalanceCache.mEventTimeFieldState  =  NOT_MODIFIED;

        if (mOrderImbalanceCache.mEventSeqNumFieldState  == MODIFIED)
            mOrderImbalanceCache.mEventSeqNumFieldState  =  NOT_MODIFIED;

        if (mOrderImbalanceCache.mSrcTimeFieldState  == MODIFIED)      
            mOrderImbalanceCache.mSrcTimeFieldState  =  NOT_MODIFIED;

        if (mOrderImbalanceCache.mActTimeFieldState  == MODIFIED)  
            mOrderImbalanceCache.mActTimeFieldState  =  NOT_MODIFIED;

        if (mOrderImbalanceCache.mMsgTypeFieldState  == MODIFIED)
            mOrderImbalanceCache.mMsgTypeFieldState  =  NOT_MODIFIED;

        if (mOrderImbalanceCache.mIssueSymbolFieldState  == MODIFIED)
            mOrderImbalanceCache.mIssueSymbolFieldState  =  NOT_MODIFIED;

        if (mOrderImbalanceCache.mPartIdFieldState  == MODIFIED)       
            mOrderImbalanceCache.mPartIdFieldState  =  NOT_MODIFIED;

        if (mOrderImbalanceCache.mSeqNumFieldState  == MODIFIED)  
            mOrderImbalanceCache.mSeqNumFieldState  =  NOT_MODIFIED;

        if (mOrderImbalanceCache.mSecurityStatusOrigFieldState  == MODIFIED) 
            mOrderImbalanceCache.mSecurityStatusOrigFieldState  =  NOT_MODIFIED;

        if (mOrderImbalanceCache.mSecurityStatusTimeFieldState  == MODIFIED) 
            mOrderImbalanceCache.mSecurityStatusTimeFieldState  =  NOT_MODIFIED;

        if (mOrderImbalanceCache.mAuctionTimeFieldState  == MODIFIED)        
            mOrderImbalanceCache.mAuctionTimeFieldState  =  NOT_MODIFIED;

        if (mOrderImbalanceCache.mLineTimeFieldState  == MODIFIED)     
            mOrderImbalanceCache.mLineTimeFieldState  =  NOT_MODIFIED;

        if (mOrderImbalanceCache.mSendTimeFieldState  == MODIFIED)  
            mOrderImbalanceCache.mSendTimeFieldState  =  NOT_MODIFIED;

        if (mOrderImbalanceCache.mSymbolFieldState  == MODIFIED)    
            mOrderImbalanceCache.mSymbolFieldState  =  NOT_MODIFIED;

        if (mMsgQualFieldState == MODIFIED)
            mMsgQualFieldState = NOT_MODIFIED;
    }

    const MamaMsgQual& MamdaOrderImbalanceListener::getMsgQual() const
    {
        return mImpl.mMsgQual;
    }

    MamdaFieldState MamdaOrderImbalanceListener::getMsgQualFieldState() const
    {
        return mImpl.mMsgQualFieldState;
    }
    //end

    /** Create a class for each field*/
    struct MamdaOrderImbalanceListener::
    MamdaOrderImbalanceListenerImpl::FieldUpdateHighIndicationPrice 
        : public OrderImbalanceFieldUpdate
    {
        void onUpdate (MamdaOrderImbalanceListener::MamdaOrderImbalanceListenerImpl&  impl,
                       const MamaMsgField&                                            field)
        {
            field.getPrice(impl.mOrderImbalanceCache.mHighIndicationPrice );
            impl.mOrderImbalanceCache.mHighIndicationPriceFieldState = MODIFIED;
            impl.mOrderImbalanceCache.mIsOrderImbalance = true;
        }
    };
    struct MamdaOrderImbalanceListener::
    MamdaOrderImbalanceListenerImpl::FieldUpdateLowIndicationPrice 
        : public OrderImbalanceFieldUpdate
    {
        void onUpdate (MamdaOrderImbalanceListener::MamdaOrderImbalanceListenerImpl&  impl,
                       const MamaMsgField&                                            field)
        {
             field.getPrice(impl.mOrderImbalanceCache.mLowIndicationPrice);
             impl.mOrderImbalanceCache.mLowIndicationPriceFieldState = MODIFIED;
             impl.mOrderImbalanceCache.mIsOrderImbalance = true;
        }
    };

    struct MamdaOrderImbalanceListener::
    MamdaOrderImbalanceListenerImpl::FieldUpdateIndicationPrice 
        : public OrderImbalanceFieldUpdate
    {
        void onUpdate (MamdaOrderImbalanceListener::MamdaOrderImbalanceListenerImpl&  impl,
                       const MamaMsgField&                                            field)
        {
            field.getPrice(impl.mOrderImbalanceCache.mIndicationPrice);
            impl.mOrderImbalanceCache.mIndicationPriceFieldState = MODIFIED;
            impl.mOrderImbalanceCache.mIsOrderImbalance = true;
        }
    };
     
    struct MamdaOrderImbalanceListener::
    MamdaOrderImbalanceListenerImpl::FieldUpdateBuyVolume 
        : public OrderImbalanceFieldUpdate
    {
        void onUpdate (MamdaOrderImbalanceListener::MamdaOrderImbalanceListenerImpl&  impl,
                       const MamaMsgField&                                            field)
        {
            impl.mOrderImbalanceCache.mBuyVolume = field.getI64 ();
            impl.mOrderImbalanceCache.mBuyVolumeFieldState = MODIFIED;
            impl.mOrderImbalanceCache.mIsOrderImbalance = true;
        }
    };
     
    struct MamdaOrderImbalanceListener::
    MamdaOrderImbalanceListenerImpl::FieldUpdateSellVolume 
        : public OrderImbalanceFieldUpdate
    {
        void onUpdate (MamdaOrderImbalanceListener::MamdaOrderImbalanceListenerImpl&  impl,
                       const MamaMsgField&                                            field)
        {
            impl.mOrderImbalanceCache.mSellVolume = field.getI64 ();
            impl.mOrderImbalanceCache.mSellVolumeFieldState = MODIFIED;
            impl.mOrderImbalanceCache.mIsOrderImbalance = true;
        }
    };

    struct MamdaOrderImbalanceListener::
    MamdaOrderImbalanceListenerImpl::FieldUpdateMatchVolume 
        : public OrderImbalanceFieldUpdate
    {
        void onUpdate (MamdaOrderImbalanceListener::MamdaOrderImbalanceListenerImpl&  impl,
                       const MamaMsgField&                                            field)
        {
            impl.mOrderImbalanceCache.mMatchVolume =
                            field.getI64 ();
            impl.mOrderImbalanceCache.mMatchVolumeFieldState = MODIFIED;
            impl.mOrderImbalanceCache.mIsOrderImbalance = true;
        }
    };
      
    struct MamdaOrderImbalanceListener::
    MamdaOrderImbalanceListenerImpl::FieldUpdateSecurityStatusQual 
        : public OrderImbalanceFieldUpdate
    {
        void onUpdate (MamdaOrderImbalanceListener::MamdaOrderImbalanceListenerImpl&  impl,
                       const MamaMsgField&                                            field)
        {
            impl.mOrderImbalanceCache.mSecurityStatusQual =
                            field.getString ();     
            impl.mOrderImbalanceCache.mSecurityStatusQualFieldState = MODIFIED;
            impl.mOrderImbalanceCache.mSecurityStatusQualValue = 
            (MamdaOrderImbalanceType::OrderImbalanceType)MamdaOrderImbalanceType::stringToValue (field.getString());
            if (MamdaOrderImbalanceType::isMamdaOrderImbalanceType (impl.mOrderImbalanceCache.mSecurityStatusQualValue))
            {
                impl.mOrderImbalanceCache.mIsOrderImbalance = true;
            }
        }
    };
       
    struct MamdaOrderImbalanceListener::
    MamdaOrderImbalanceListenerImpl::FieldUpdateMatchPrice 
        : public OrderImbalanceFieldUpdate
    {
        void onUpdate (MamdaOrderImbalanceListener::MamdaOrderImbalanceListenerImpl&  impl,
                       const MamaMsgField&                                            field)
        {
            field.getPrice (impl.mOrderImbalanceCache.mInsideMatchPrice);
            impl.mOrderImbalanceCache.mInsideMatchPriceFieldState = MODIFIED;
            impl.mOrderImbalanceCache.mIsOrderImbalance = true;
        }
    };
        
    struct MamdaOrderImbalanceListener::
    MamdaOrderImbalanceListenerImpl::FieldUpdateFarClearingPrice 
        : public OrderImbalanceFieldUpdate
    {
        void onUpdate (MamdaOrderImbalanceListener::MamdaOrderImbalanceListenerImpl&  impl,
                       const MamaMsgField&                                            field)
        {
              field.getPrice (impl.mOrderImbalanceCache.mFarClearingPrice);
              impl.mOrderImbalanceCache.mFarClearingPriceFieldState = MODIFIED;
              impl.mOrderImbalanceCache.mIsOrderImbalance = true;
        }
    };
       
    struct MamdaOrderImbalanceListener::
    MamdaOrderImbalanceListenerImpl::FieldUpdateNearClearingPrice 
        : public OrderImbalanceFieldUpdate
    {
        void onUpdate (MamdaOrderImbalanceListener::MamdaOrderImbalanceListenerImpl&  impl,
                       const MamaMsgField&                                            field)
        {
             field.getPrice (impl.mOrderImbalanceCache.mNearClearingPrice);
             impl.mOrderImbalanceCache.mNearClearingPriceFieldState = MODIFIED;
             impl.mOrderImbalanceCache.mIsOrderImbalance = true;
        }
    };
        
    struct MamdaOrderImbalanceListener::
    MamdaOrderImbalanceListenerImpl::FieldUpdateNoClearingPrice 
        : public OrderImbalanceFieldUpdate
    {
        void onUpdate (MamdaOrderImbalanceListener::MamdaOrderImbalanceListenerImpl&  impl,
                       const MamaMsgField&                                            field)
        {
            // There is a bug in 2.14 FHs which can cause character fields to be sent as strings
            // FH property CharFieldAsStringField in 2.16-> can enable this behaviour
            // Adding support for this in MAMDA for client apps coded to expect this behaviour
            switch (field.getType())
            {
                case MAMA_FIELD_TYPE_CHAR :
                        impl.mOrderImbalanceCache.mNoClearingPrice = field.getChar ();
                        impl.mOrderImbalanceCache.mNoClearingPriceFieldState = MODIFIED;
                        impl.mOrderImbalanceCache.mIsOrderImbalance = true;
                        break;
                case MAMA_FIELD_TYPE_STRING :
                        impl.mOrderImbalanceCache.mNoClearingPrice = field.getString()[0];
                        impl.mOrderImbalanceCache.mNoClearingPriceFieldState = MODIFIED;
                        impl.mOrderImbalanceCache.mIsOrderImbalance = true;
                        break;
                default : break;
            }
        }
    };
     
    struct MamdaOrderImbalanceListener::
    MamdaOrderImbalanceListenerImpl::FieldUpdatePriceVarInd 
        : public OrderImbalanceFieldUpdate
    {
        void onUpdate (MamdaOrderImbalanceListener::MamdaOrderImbalanceListenerImpl&  impl,
                       const MamaMsgField&                                            field)
        {
            //There is a bug in 2.14 FHs which can cause character fields to be sent as strings
            //FH property CharFieldAsStringField in 2.16-> can enable this behaviour
            //Adding support for this in MAMDA for client apps coded to expect this behaviour
            switch (field.getType())
            {
                case MAMA_FIELD_TYPE_CHAR :
                        impl.mOrderImbalanceCache.mPriceVarInd = field.getChar ();
                        impl.mOrderImbalanceCache.mPriceVarIndFieldState = MODIFIED;
                        impl.mOrderImbalanceCache.mIsOrderImbalance = true;
                        break;
                case MAMA_FIELD_TYPE_STRING :
                        impl.mOrderImbalanceCache.mPriceVarInd = field.getString()[0];
                        impl.mOrderImbalanceCache.mPriceVarIndFieldState = MODIFIED;
                        impl.mOrderImbalanceCache.mIsOrderImbalance = true;
                        break;
                default : break;
            }
        }
    };
       
    struct MamdaOrderImbalanceListener::
    MamdaOrderImbalanceListenerImpl::FieldUpdateCrossType 
        : public OrderImbalanceFieldUpdate
    {
        void onUpdate (MamdaOrderImbalanceListener::MamdaOrderImbalanceListenerImpl&  impl,
                       const MamaMsgField&                                            field)
        {
            //There is a bug in 2.14 FHs which can cause character fields to be sent as strings
            //FH property CharFieldAsStringField in 2.16-> can enable this behaviour
            //Adding support for this in MAMDA for client apps coded to expect this behaviour
            switch (field.getType())
            {
                case MAMA_FIELD_TYPE_CHAR :
                        impl.mOrderImbalanceCache.mCrossType = field.getChar ();
                        impl.mOrderImbalanceCache.mCrossTypeFieldState = MODIFIED;
                        impl.mOrderImbalanceCache.mIsOrderImbalance = true;
                        break;
                case MAMA_FIELD_TYPE_STRING :
                        impl.mOrderImbalanceCache.mCrossType = field.getString()[0];
                        impl.mOrderImbalanceCache.mCrossTypeFieldState = MODIFIED;
                        impl.mOrderImbalanceCache.mIsOrderImbalance = true;
                        break;
                default : break;
            }
        }
    };
        
    struct MamdaOrderImbalanceListener::
    MamdaOrderImbalanceListenerImpl::FieldUpdateEventTime 
        : public OrderImbalanceFieldUpdate
    {
        void onUpdate (MamdaOrderImbalanceListener::MamdaOrderImbalanceListenerImpl&  impl,
                       const MamaMsgField&                                            field)
        {
             field.getDateTime (impl.mOrderImbalanceCache.mEventTime);
             impl.mOrderImbalanceCache.mEventTimeFieldState = MODIFIED;
        }
    };
        
    struct MamdaOrderImbalanceListener::
    MamdaOrderImbalanceListenerImpl::FieldUpdateEventSeqNum 
        : public OrderImbalanceFieldUpdate
    {
        void onUpdate (MamdaOrderImbalanceListener::MamdaOrderImbalanceListenerImpl&  impl,
                       const MamaMsgField&                                            field)
        {
             impl.mOrderImbalanceCache.mEventSeqNum =
                            field.getU32 ();
             impl.mOrderImbalanceCache.mEventSeqNumFieldState = MODIFIED;
        }
    };
        
    struct MamdaOrderImbalanceListener::
    MamdaOrderImbalanceListenerImpl::FieldUpdateSrcTime 
        : public OrderImbalanceFieldUpdate
    {
        void onUpdate (MamdaOrderImbalanceListener::MamdaOrderImbalanceListenerImpl&  impl,
                       const MamaMsgField&                                            field)
        {
            field.getDateTime (impl.mOrderImbalanceCache.mSrcTime);
            impl.mOrderImbalanceCache.mSrcTimeFieldState = MODIFIED;
        }
    };

    struct MamdaOrderImbalanceListener::
    MamdaOrderImbalanceListenerImpl::FieldUpdateActTime 
        : public OrderImbalanceFieldUpdate
    {
        void onUpdate (MamdaOrderImbalanceListener::MamdaOrderImbalanceListenerImpl&  impl,
                       const MamaMsgField&                                            field)
        {
             field.getDateTime (impl.mOrderImbalanceCache.mActTime);
             impl.mOrderImbalanceCache.mActTimeFieldState = MODIFIED;
        }
    };
       
    struct MamdaOrderImbalanceListener::
    MamdaOrderImbalanceListenerImpl::FieldUpdateMsgType 
        : public OrderImbalanceFieldUpdate
    {
        void onUpdate (MamdaOrderImbalanceListener::MamdaOrderImbalanceListenerImpl&  impl,
                       const MamaMsgField&                                            field)
        {
             impl.mOrderImbalanceCache.mMsgType =
                            field.getI32 ();
             impl.mOrderImbalanceCache.mMsgTypeFieldState = MODIFIED;
        }
    };   

    struct MamdaOrderImbalanceListener::
    MamdaOrderImbalanceListenerImpl::FieldUpdateIssueSymbol 
        : public OrderImbalanceFieldUpdate
    {
        void onUpdate (MamdaOrderImbalanceListener::MamdaOrderImbalanceListenerImpl&  impl,
                       const MamaMsgField&                                            field)
        {
             impl.mOrderImbalanceCache.mIssueSymbol =
                            field.getString ();
             impl.mOrderImbalanceCache.mIssueSymbolFieldState = MODIFIED;
        }
    };  
        
    struct MamdaOrderImbalanceListener::
    MamdaOrderImbalanceListenerImpl::FieldUpdatePartId 
        : public OrderImbalanceFieldUpdate
    {
        void onUpdate (MamdaOrderImbalanceListener::MamdaOrderImbalanceListenerImpl&  impl,
                       const MamaMsgField&                                            field)
        {
             impl.mOrderImbalanceCache.mPartId =
                            field.getString ();
             impl.mOrderImbalanceCache.mPartIdFieldState = MODIFIED;
        }
    }; 

    struct MamdaOrderImbalanceListener::
    MamdaOrderImbalanceListenerImpl::FieldUpdateSeqNum 
        : public OrderImbalanceFieldUpdate
    {
        void onUpdate (MamdaOrderImbalanceListener::MamdaOrderImbalanceListenerImpl&  impl,
                       const MamaMsgField&                                            field)
        {
             impl.mOrderImbalanceCache.mSeqNum =
                            field.getU32 ();
             impl.mOrderImbalanceCache.mSeqNumFieldState = MODIFIED;
        }
    };    

    struct MamdaOrderImbalanceListener::
    MamdaOrderImbalanceListenerImpl::FieldUpdateSecurityStatusOrig 
        : public OrderImbalanceFieldUpdate
    {
        void onUpdate (MamdaOrderImbalanceListener::MamdaOrderImbalanceListenerImpl&  impl,
                       const MamaMsgField&                                            field)
        {
             impl.mOrderImbalanceCache.mSecurityStatusOrig =
                            field.getString ();
             impl.mOrderImbalanceCache.mSecurityStatusOrigFieldState = MODIFIED;
        }
    };

    struct MamdaOrderImbalanceListener::
    MamdaOrderImbalanceListenerImpl::FieldUpdateSecurityStatusTime 
        : public OrderImbalanceFieldUpdate
    {
        void onUpdate (MamdaOrderImbalanceListener::MamdaOrderImbalanceListenerImpl&  impl,
                       const MamaMsgField&                                            field)
        {
             field.getDateTime (impl.mOrderImbalanceCache.mSecurityStatusTime);
             impl.mOrderImbalanceCache.mSecurityStatusTimeFieldState = MODIFIED;
        }
    };

    struct MamdaOrderImbalanceListener::
    MamdaOrderImbalanceListenerImpl::FieldUpdateAuctionTime 
        : public OrderImbalanceFieldUpdate
    {
        void onUpdate (MamdaOrderImbalanceListener::MamdaOrderImbalanceListenerImpl&  impl,
                       const MamaMsgField&                                            field)
        {
            field.getDateTime (impl.mOrderImbalanceCache.mAuctionTime);
            impl.mOrderImbalanceCache.mAuctionTimeFieldState = MODIFIED;
        }
    };

    struct MamdaOrderImbalanceListener::
    MamdaOrderImbalanceListenerImpl::FieldUpdateLineTime 
        : public OrderImbalanceFieldUpdate
    {
        void onUpdate (MamdaOrderImbalanceListener::MamdaOrderImbalanceListenerImpl&  impl,
                       const MamaMsgField&                                            field)
        {
           field.getDateTime ( impl.mOrderImbalanceCache.mLineTime);
           impl.mOrderImbalanceCache.mLineTimeFieldState = MODIFIED;
        }
    };
       
    struct MamdaOrderImbalanceListener::
    MamdaOrderImbalanceListenerImpl::FieldUpdateSendTime 
        : public OrderImbalanceFieldUpdate
    {
        void onUpdate (MamdaOrderImbalanceListener::MamdaOrderImbalanceListenerImpl&  impl,
                       const MamaMsgField&                                            field)
        {
             field.getDateTime (impl.mOrderImbalanceCache.mSendTime);
             impl.mOrderImbalanceCache.mSendTimeFieldState = MODIFIED;
        }
    };

    /***end*/

    OrderImbalanceFieldUpdate** MamdaOrderImbalanceListener::
        MamdaOrderImbalanceListenerImpl::mFieldUpdaters = NULL;

    volatile uint16_t MamdaOrderImbalanceListener::
        MamdaOrderImbalanceListenerImpl::mFieldUpdatersSize = 0;

    pthread_mutex_t MamdaOrderImbalanceListener::
        MamdaOrderImbalanceListenerImpl::mImbalanceFieldUpdaterLockMutex 
        = PTHREAD_MUTEX_INITIALIZER;

    bool MamdaOrderImbalanceListener::
        MamdaOrderImbalanceListenerImpl::mUpdatersComplete = false;

    void MamdaOrderImbalanceListener::MamdaOrderImbalanceListenerImpl::initFieldUpdaters ()
    {
        if (!mFieldUpdaters)
        {
            mFieldUpdaters = 
                new OrderImbalanceFieldUpdate* [MamdaOrderImbalanceFields::getMaxFid() +1];

            mFieldUpdatersSize = MamdaOrderImbalanceFields::getMaxFid();

            /* Use a uint32_t instead of uint16_t to avoid inifinite loop */
            for (uint32_t i = 0; i <= mFieldUpdatersSize; ++i)
            {
                mFieldUpdaters[i] = NULL;
            }
        }
        
        initFieldUpdater (MamdaOrderImbalanceFields::SRC_TIME,
                          new MamdaOrderImbalanceListener::
		                  MamdaOrderImbalanceListenerImpl::FieldUpdateSrcTime);

        initFieldUpdater (MamdaOrderImbalanceFields::HIGH_INDICATION_PRICE,
                          new MamdaOrderImbalanceListener::
		                  MamdaOrderImbalanceListenerImpl::FieldUpdateHighIndicationPrice);

        initFieldUpdater (MamdaOrderImbalanceFields::LOW_INDICATION_PRICE,
                          new MamdaOrderImbalanceListener::
		                  MamdaOrderImbalanceListenerImpl::FieldUpdateLowIndicationPrice);

        initFieldUpdater (MamdaOrderImbalanceFields::INDICATION_PRICE,
                          new MamdaOrderImbalanceListener::
                          MamdaOrderImbalanceListenerImpl::FieldUpdateIndicationPrice);

        initFieldUpdater (MamdaOrderImbalanceFields::BUY_VOLUME,
                          new MamdaOrderImbalanceListener::
                          MamdaOrderImbalanceListenerImpl::FieldUpdateBuyVolume); 

        initFieldUpdater (MamdaOrderImbalanceFields::SELL_VOLUME,
                          new MamdaOrderImbalanceListener::
                          MamdaOrderImbalanceListenerImpl::FieldUpdateSellVolume);

        initFieldUpdater (MamdaOrderImbalanceFields::MATCH_VOLUME,
                          new MamdaOrderImbalanceListener::
                          MamdaOrderImbalanceListenerImpl::FieldUpdateMatchVolume);

        initFieldUpdater (MamdaOrderImbalanceFields::SECURITY_STATUS_QUAL,
                          new MamdaOrderImbalanceListener::
                          MamdaOrderImbalanceListenerImpl::FieldUpdateSecurityStatusQual);

        initFieldUpdater (MamdaOrderImbalanceFields::INSIDE_MATCH_PRICE,
                          new MamdaOrderImbalanceListener::
                          MamdaOrderImbalanceListenerImpl::FieldUpdateMatchPrice);

        initFieldUpdater (MamdaOrderImbalanceFields::FAR_CLEARING_PRICE,
                          new MamdaOrderImbalanceListener::
                          MamdaOrderImbalanceListenerImpl::FieldUpdateFarClearingPrice);

        initFieldUpdater (MamdaOrderImbalanceFields::NEAR_CLEARING_PRICE,
                          new MamdaOrderImbalanceListener::
                          MamdaOrderImbalanceListenerImpl::FieldUpdateNearClearingPrice);

        initFieldUpdater (MamdaOrderImbalanceFields::NO_CLEARING_PRICE,
                          new MamdaOrderImbalanceListener::
                          MamdaOrderImbalanceListenerImpl::FieldUpdateNoClearingPrice);

        initFieldUpdater (MamdaOrderImbalanceFields::PRICE_VAR_IND,
                          new MamdaOrderImbalanceListener::
                          MamdaOrderImbalanceListenerImpl::FieldUpdatePriceVarInd);

        initFieldUpdater (MamdaOrderImbalanceFields::CROSS_TYPE,
                          new MamdaOrderImbalanceListener::
                          MamdaOrderImbalanceListenerImpl::FieldUpdateCrossType);  
      
        initFieldUpdater (MamdaOrderImbalanceFields::ACTIVITY_TIME,
                          new MamdaOrderImbalanceListener::
                          MamdaOrderImbalanceListenerImpl::FieldUpdateActTime); 

        initFieldUpdater (MamdaOrderImbalanceFields::MSG_TYPE,
                          new MamdaOrderImbalanceListener::
                          MamdaOrderImbalanceListenerImpl::FieldUpdateMsgType);

        initFieldUpdater (MamdaOrderImbalanceFields::ISSUE_SYMBOL,
                          new MamdaOrderImbalanceListener::
                          MamdaOrderImbalanceListenerImpl::FieldUpdateIssueSymbol);

        initFieldUpdater (MamdaOrderImbalanceFields::PART_ID,
                          new MamdaOrderImbalanceListener::
                          MamdaOrderImbalanceListenerImpl::FieldUpdatePartId);

        initFieldUpdater (MamdaOrderImbalanceFields::SEQ_NUM,
                          new MamdaOrderImbalanceListener::
                          MamdaOrderImbalanceListenerImpl::FieldUpdateSeqNum);

        initFieldUpdater (MamdaOrderImbalanceFields::SECURITY_STATUS_ORIG,
                          new MamdaOrderImbalanceListener::
                          MamdaOrderImbalanceListenerImpl::FieldUpdateSecurityStatusOrig);

        initFieldUpdater (MamdaOrderImbalanceFields::SECURITY_STATUS_TIME,
                          new MamdaOrderImbalanceListener::
                          MamdaOrderImbalanceListenerImpl::FieldUpdateSecurityStatusTime);

        initFieldUpdater (MamdaOrderImbalanceFields::AUCTION_TIME,
                          new MamdaOrderImbalanceListener::
                          MamdaOrderImbalanceListenerImpl::FieldUpdateAuctionTime);

        initFieldUpdater (MamdaOrderImbalanceFields::LINE_TIME,
                          new MamdaOrderImbalanceListener::
                          MamdaOrderImbalanceListenerImpl::FieldUpdateLineTime);

        initFieldUpdater (MamdaOrderImbalanceFields::SEND_TIME,
                          new MamdaOrderImbalanceListener::
                          MamdaOrderImbalanceListenerImpl::FieldUpdateSendTime);
    }

    void MamdaOrderImbalanceListener::MamdaOrderImbalanceListenerImpl::initFieldUpdater (
        const MamaFieldDescriptor*  fieldDesc,
        OrderImbalanceFieldUpdate*  updater)
    {
        if (!fieldDesc)
            return;

        uint16_t fid = fieldDesc->getFid();
        if (fid <= mFieldUpdatersSize)
        {
            mFieldUpdaters[fid] = updater;
        }
    }

    /* Handle onField */
    void MamdaOrderImbalanceListener::MamdaOrderImbalanceListenerImpl::onField (
        const MamaMsg&       msg,
        const MamaMsgField&  field,
        void*                closure)
    {
        uint16_t fid = field.getFid();
        if (fid <= mFieldUpdatersSize)
        {
            OrderImbalanceFieldUpdate* updater = mFieldUpdaters[fid];
            if (updater)
            {
                updater->onUpdate (*this, field);
            }
        }
    }

    void MamdaOrderImbalanceListener::MamdaOrderImbalanceListenerImpl::handleRecap (
        MamdaSubscription*  subscription, 
        const MamaMsg&      msg)
    {
         if(subscription->checkDebugLevel(MAMA_LOG_LEVEL_FINE))
        {
            const char *contractSymbol = "N/A";

            msg.tryString (MamdaCommonFields::ISSUE_SYMBOL, contractSymbol);

            mama_forceLog (MAMA_LOG_LEVEL_FINE,
                           "MamdaOrderImbalanceListener (%s.%s(%s)) handleRecap().\n",
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
                mHandler->onOrderImbalanceRecap (subscription, 
                                                 mListener, 
                                                 msg, 
                                                 mListener);
            }
        }
    }
    void MamdaOrderImbalanceListener::MamdaOrderImbalanceListenerImpl::handleUpdate (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg,
        mama_i32_t          msgType)
    {
         const char* securityStatus = NULL;

        // Ensure that the field handling is set up (once for all
        // MamdaQuoteListener instances).
        if (!mUpdatersComplete)
        {
            wthread_mutex_lock (&mImbalanceFieldUpdaterLockMutex);

            if (!mUpdatersComplete)
            {
                if (!MamdaOrderImbalanceFields::isSet())
                {
                     mama_log (MAMA_LOG_LEVEL_WARN,
                               "MamdaOrderImbalanceListener: MamdaOrderImbalanceFields::setDictionary() "
                               "has not been called.");
                     wthread_mutex_unlock (&mImbalanceFieldUpdaterLockMutex);
                     return;
                }

                try
                {
                    initFieldUpdaters ();
                }
                catch (MamaStatus &e)
                {
                    mama_log (MAMA_LOG_LEVEL_WARN,
                              "MamdaOrderImbalanceListener: Could not set field updaters: %s",
                              e.toString ());
                    wthread_mutex_unlock (&mImbalanceFieldUpdaterLockMutex);
                    return;
                }
                mUpdatersComplete = true;
            }

            wthread_mutex_unlock (&mImbalanceFieldUpdaterLockMutex);
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
                    mTransientCache = new OrderImbalanceCache();
                }
                mOrderImbalanceCache = *mTransientCache;
            }
        
            wthread_mutex_lock (&mImbalanceLock.mImbalanceMutex);     

            mOrderImbalanceCache.mIsOrderImbalance = false;
            
            updateFieldStates();     
            
            //update all fields
            msg.iterateFields (*this, NULL, NULL);

            wthread_mutex_unlock (&mImbalanceLock.mImbalanceMutex);        
                 
            switch (msgType)
            {
                case MAMA_MSG_TYPE_INITIAL:
                case MAMA_MSG_TYPE_RECAP:
                    handleRecap (subscription, msg);
                break;
                case MAMA_MSG_TYPE_UPDATE:
                {
                    if (mOrderImbalanceCache.mIsOrderImbalance)
                    {
                        if ((MamdaOrderImbalanceType::isMamdaImbalanceOrder (mOrderImbalanceCache.mSecurityStatusQualValue)) ||
                                (mOrderImbalanceCache.mSecurityStatusQualValue == MamdaOrderImbalanceType::UNKNOWN))
                        {
                            handleOrderImbalance (subscription, msg); 
                        }
                        else
                        {
                            handleNoOrderImbalance (subscription, msg); 
                        }
                    }
                }
                break;
                default:
                break;
            }
            
            if (mIsTransientMsg && mProcessPosDupAndOutOfSeqAsTransient)
            {
                mOrderImbalanceCache = mRegularCache;
                clearCache (*mTransientCache);
            }
        }
        else
        {
            if(subscription->checkDebugLevel (MAMA_LOG_LEVEL_FINE))
            {
                const char *contractSymbol = "N/A";
                msg.tryString (MamdaOrderImbalanceFields::ISSUE_SYMBOL, contractSymbol);

                mama_forceLog (MAMA_LOG_LEVEL_FINE,
                               "MamdaOrderImbalanceListener (%s.%s(%s)) "
                               "Duplicate message NOT processed.\n",
                               contractSymbol,
                               subscription->getSource (),
                               subscription->getSymbol ());
            }
        }    
        
    }

    bool MamdaOrderImbalanceListener::MamdaOrderImbalanceListenerImpl::isImbalanceType (
        const char*  securityStatus)
    {
            int value = MamdaOrderImbalanceType::stringToValue (securityStatus);
            return MamdaOrderImbalanceType::isMamdaOrderImbalanceType (value);
    }

    void MamdaOrderImbalanceListener::MamdaOrderImbalanceListenerImpl::invokeTransientHandler (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg)
    {
        const char *contractSymbol = "N/A";
        msg.tryString (MamdaOrderImbalanceFields::ISSUE_SYMBOL, contractSymbol);

        mama_forceLog (MAMA_LOG_LEVEL_FINE,
                       "MamdaOrderImbalanceListener (%s.%s(%s)) handleTransientData(): "
                       "update: Transient Msg neither Out-of-Sequence "
                       "nor Possibly Stale",
                       subscription->getSource (),
                       subscription->getSymbol (),
                       contractSymbol);
    }

    void MamdaOrderImbalanceListener::MamdaOrderImbalanceListenerImpl::handleOrderImbalance (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg)
    {
       if(subscription->checkDebugLevel(MAMA_LOG_LEVEL_FINE))
       {
            const char *contractSymbol = "N/A";
            msg.tryString (MamdaOrderImbalanceFields::ISSUE_SYMBOL, contractSymbol);
            mama_forceLog (
                MAMA_LOG_LEVEL_FINE,
                "MamdaOrderImabalanceListener (%s.%s(%s)) handleOrderImbalance().\n",
                contractSymbol,
                subscription->getSource (),
                subscription->getSymbol ());
       }

       if (mUsePosDupAndOutOfSeqHandlers && mIsTransientMsg)
       {
           invokeTransientHandler(subscription, msg);
       }
       else
       {
           mHandler->onOrderImbalance (subscription,
                                       mListener,
                                       msg,
                                       mListener,
                                       mListener);
       }
    }
       
    void MamdaOrderImbalanceListener::MamdaOrderImbalanceListenerImpl::handleNoOrderImbalance (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg)
    {

       if (subscription->checkDebugLevel (MAMA_LOG_LEVEL_FINE))
       {
            const char *contractSymbol = "N/A";
            msg.tryString (MamdaOrderImbalanceFields::ISSUE_SYMBOL, contractSymbol);

            mama_forceLog (MAMA_LOG_LEVEL_FINE,
                           "MamdaOrderImabalanceListener (%s.%s(%s)) handleOrderImbalance().\n",
                           contractSymbol,
                           subscription->getSource (),
                           subscription->getSymbol ());
       }

       if (mUsePosDupAndOutOfSeqHandlers && mIsTransientMsg)
       {
           invokeTransientHandler(subscription, msg);
       }
       else
       {
           mHandler->onNoOrderImbalance (subscription,
                                         mListener, 
                                         msg, 
                                         mListener, 
                                         mListener);
       }
    }

    bool MamdaOrderImbalanceListener::MamdaOrderImbalanceListenerImpl::evaluateMsgQual (    
        MamdaSubscription*  subscription,
        const MamaMsg&      msg)
    {
        bool isDuplicateMsg = false;
        mIsTransientMsg     = false;

        mMsgQual.clear();   
        uint16_t msgQualVal = 0;

        // Does message contains a qualifier- need to parse this anyway.
        // Need Fid as reserved field may not be in data dictionary
        if (msg.tryU16 (MamaFieldMsgQual.mName, 
                        MamaFieldMsgQual.mFid, 
                        msgQualVal))
        {
            mMsgQual.setValue (msgQualVal);
            mMsgQualFieldState = MODIFIED;

            // If qualifier indicates messages is possbily duplicate
            // and the listener has been configure to attempt to 
            // resolve the ambiguity regarding the duplicate 
            // status of the message.
            if (mMsgQual.getIsPossiblyDuplicate() && mResolvePossiblyDuplicate)
            {
                mama_seqnum_t seqNum = 0;
                MamaDateTime  eventTime;
                
                if (msg.tryU32 (MamdaOrderImbalanceFields::SEQ_NUM, seqNum) &&
                    msg.tryDateTime (MamdaOrderImbalanceFields::SRC_TIME, eventTime)) 
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

}//namespace
