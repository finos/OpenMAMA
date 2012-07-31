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

#include <wombat/wincompat.h>
#include <pthread.h>
#include <mamda/MamdaCurrencyListener.h>
#include <mamda/MamdaCurrencyHandler.h>
#include <mamda/MamdaCommonFields.h>
#include <mamda/MamdaCurrencyFields.h>
#include <mama/mamacpp.h>
#include "MamdaUtils.h"
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <string>

using std::string;

namespace Wombat
{

    struct CurrencyFieldUpdate
    {
        virtual void onUpdate (
            MamdaCurrencyListener::MamdaCurrencyListenerImpl&  impl,
            const MamaMsgField&                                field) = 0;
    };

    class MamdaCurrencyListener::MamdaCurrencyListenerImpl
        : public MamaMsgFieldIterator
    {
    public:
        MamdaCurrencyListenerImpl   (MamdaCurrencyListener&  listener);
        ~MamdaCurrencyListenerImpl  () {}

        void clearCache             ();

        void handleCurrencyMessage  (MamdaSubscription*    subscription,
                                     const MamaMsg&        msg,
                                     short                 msgType);

        void handleRecap            (MamdaSubscription*   subscription,
                                     const MamaMsg&       msg);

        void handleUpdate           (MamdaSubscription*   subscription,
                                     const MamaMsg&       msg);

        void onField                (const MamaMsg&       msg,
                                     const MamaMsgField&  field,
                                     void*                closure);

        void updateCurrencyFields   (const MamaMsg&       msg);

        void updateFieldStates      ();

        MamdaCurrencyListener&      mListener;
        MamdaCurrencyHandler*       mHandler;
        
        /*
         * NOTE: fields which are enums can be pubished as integers if feedhandler
         * uses mama-publish-enums-as-ints.  It may also be possible for a feed to
         * publish the numerical value as a string. All enumerated fields must be handled
         * by getting the value based on the field type.
         */

        // Basic Event Fields 
        string           mSymbol;              MamdaFieldState     mSymbolFieldState;      
        bool             mSymbolIsModified;            

        string           mPartId;              MamdaFieldState     mPartIdFieldState;    
        MamaDateTime     mSrcTime;             MamdaFieldState     mSrcTimeFieldState;     
        MamaDateTime     mActTime;             MamdaFieldState     mActivityTimeFieldState;
        MamaDateTime     mLineTime;            MamdaFieldState     mLineTimeFieldState;    
        MamaDateTime     mSendTime;            MamdaFieldState     mSendTimeFieldState;
        mama_seqnum_t    mEventSeqNum;         MamdaFieldState     mEventSeqNumFieldState;
        MamaDateTime     mEventTime;           MamdaFieldState     mEventTimeFieldState;
        MamaMsgQual      mMsgQual;             MamdaFieldState     mMsgQualFieldState;
        
        
        // Currency Fields 
        // The following fields are used for caching the last reported
        // fundamental equity pricing/analysis attributes, indicators and ratios.
        // The reason for caching these values is to allow a configuration that 
        // passes the minimum amount of data  (unchanged fields not sent).
        MamaPrice        mBidPrice;            MamdaFieldState     mBidPriceFieldState;
        MamaPrice        mAskPrice;            MamdaFieldState     mAskPriceFieldState;

        bool mInitialised;
        
        static void initFieldUpdaters  ();
        static void initFieldUpdater   (const MamaFieldDescriptor*  fieldDesc,
                                        CurrencyFieldUpdate*        updater);

        static void updateField (MamdaCurrencyListener::MamdaCurrencyListenerImpl&  impl,
                                 const MamaMsgField&                                field);


        static CurrencyFieldUpdate**        mFieldUpdaters;
        static volatile uint16_t            mFieldUpdatersSize;
        static wthread_mutex_t              mCurrencyFieldUpdaterLockMutex;
        static bool                         mUpdatersComplete;

        struct FieldUpdateSrcTime;
        struct FieldUpdateActTime;
        struct FieldUpdateLineTime;
        struct FieldUpdateBidPrice;
        struct FieldUpdateAskPrice;
    };


    MamdaCurrencyListener::MamdaCurrencyListener ()
        : mImpl (*new MamdaCurrencyListenerImpl(*this))
    {
    }

    MamdaCurrencyListener::~MamdaCurrencyListener()
    {
        /* Do not call wthread_mutex_destroy for the FieldUpdaterLockMutex here.  
           If we do, it will not be initialized again if another listener is created 
           after the first is destroyed. */
        /* wthread_mutex_destroy (&mImpl.mCurrencyFieldUpdaterLockMutex); */
        delete &mImpl;
    }

    void MamdaCurrencyListener::addHandler (MamdaCurrencyHandler*  handler)
    {
        mImpl.mHandler = handler;
    }

    const char* MamdaCurrencyListener::getSymbol() const
    {
        return mImpl.mSymbol.c_str();
    }

    const char* MamdaCurrencyListener::getPartId() const
    {
        return mImpl.mPartId.c_str();
    }

    const MamaDateTime& MamdaCurrencyListener::getSrcTime() const
    {
        return mImpl.mSrcTime;
    }

    const MamaDateTime& MamdaCurrencyListener::getActivityTime() const
    {
        return mImpl.mActTime;
    }

    const MamaDateTime& MamdaCurrencyListener::getLineTime() const
    {
        return mImpl.mLineTime;
    }

    const MamaDateTime& MamdaCurrencyListener::getSendTime() const
    {
        return mImpl.mSendTime;
    }

    mama_seqnum_t MamdaCurrencyListener::getEventSeqNum() const
    {
        return mImpl.mEventSeqNum;
    }

    const MamaDateTime& MamdaCurrencyListener::getEventTime() const
    {
        return mImpl.mEventTime;
    }

    const MamaMsgQual& MamdaCurrencyListener::getMsgQual() const
    {
        return mImpl.mMsgQual;
    }

    const MamaPrice& MamdaCurrencyListener::getBidPrice() const
    {
        return mImpl.mBidPrice;
    }

    const MamaPrice& MamdaCurrencyListener::getAskPrice() const
    {
        return mImpl.mAskPrice;
    }

    bool MamdaCurrencyListener::isInitialised() const
    {
        return mImpl.mInitialised;
    }


    /*      FieldAccessors      */
    MamdaFieldState MamdaCurrencyListener::getSymbolFieldState() const
    {
        return mImpl.mSymbolFieldState;
    }

    MamdaFieldState MamdaCurrencyListener::getPartIdFieldState() const
    {
        return mImpl.mPartIdFieldState;
    }

    MamdaFieldState MamdaCurrencyListener::getSrcTimeFieldState() const
    {
        return mImpl.mSrcTimeFieldState;
    }

    MamdaFieldState MamdaCurrencyListener::getActivityTimeFieldState() const
    {
        return mImpl.mActivityTimeFieldState;
    }

    MamdaFieldState MamdaCurrencyListener::getLineTimeFieldState() const
    {
        return mImpl.mLineTimeFieldState;
    }

    MamdaFieldState MamdaCurrencyListener::getSendTimeFieldState() const
    {
        return mImpl.mSendTimeFieldState;
    }

    MamdaFieldState MamdaCurrencyListener::getEventSeqNumFieldState() const
    {
        return mImpl.mEventSeqNumFieldState;
    }

    MamdaFieldState MamdaCurrencyListener::getEventTimeFieldState() const
    {
        return mImpl.mEventTimeFieldState;
    }

    MamdaFieldState MamdaCurrencyListener::getMsgQualFieldState() const
    {
        return mImpl.mMsgQualFieldState;
    }

    MamdaFieldState MamdaCurrencyListener::getBidPriceFieldState () const
    {
        return mImpl.mBidPriceFieldState;
    }
     
    MamdaFieldState MamdaCurrencyListener::getAskPriceFieldState () const
    {
        return mImpl.mAskPriceFieldState;
    }
    /*  End FieldAccessors  */


    void MamdaCurrencyListener::onMsg (
        MamdaSubscription* subscription,
        const MamaMsg&     msg,
        short              msgType)
    { 
        // If msg is a trade-related message, invoke the
        // appropriate callback:
        if(subscription->checkDebugLevel(MAMA_LOG_LEVEL_FINE))
        {
            const char *contractSymbol = "N/A";
            msg.tryString (MamdaCommonFields::ISSUE_SYMBOL, contractSymbol);

            mama_forceLog (
                MAMA_LOG_LEVEL_FINE,
                "MamdaCurrencyListener (%s.%s(%s)) onMsg(). "
                "msg type: %s msg status %s\n",
                subscription->getSource (),
                subscription->getSymbol (),
                contractSymbol,
                msg.getMsgTypeName (),
                msg.getMsgStatusString ());
        }

        switch (msgType)
        {
        case MAMA_MSG_TYPE_QUOTE:
        case MAMA_MSG_TYPE_INITIAL:
        case MAMA_MSG_TYPE_RECAP:
        case MAMA_MSG_TYPE_UPDATE:
            mImpl.handleCurrencyMessage (subscription, msg, msgType);
            break;
        }
    }


    MamdaCurrencyListener::
    MamdaCurrencyListenerImpl::MamdaCurrencyListenerImpl(
        MamdaCurrencyListener&  listener)
        : mListener (listener)
        , mHandler (NULL)
    {
        clearCache();
    }

    void MamdaCurrencyListener::MamdaCurrencyListenerImpl::clearCache ()
    {
        mSymbol = "";
        mPartId = "";
        mAskPrice.clear();
        mBidPrice.clear();

        mInitialised = false;

        mSymbolFieldState   = NOT_INITIALISED;
        mPartIdFieldState   = NOT_INITIALISED;
        mAskPriceFieldState = NOT_INITIALISED;
        mBidPriceFieldState = NOT_INITIALISED;     
    }

    void MamdaCurrencyListener::MamdaCurrencyListenerImpl::handleCurrencyMessage (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg,
        short               msgType)
    {
        mInitialised = true;
        // Ensure that the field handling is set up (once for all
        // MamdaCurrencyListener instances).
        if (!mUpdatersComplete)
        {
            wthread_mutex_lock (&mCurrencyFieldUpdaterLockMutex);

            if (!mUpdatersComplete)
            {
                if (!MamdaCurrencyFields::isSet())
                {
                     mama_log (MAMA_LOG_LEVEL_WARN,
                               "MamdaCurrencyListener: MamdaCurrencyFields::setDictionary() "
                               "has not been called.");
                     wthread_mutex_unlock (&mCurrencyFieldUpdaterLockMutex);
                     return;
                }

                try
                {
                    initFieldUpdaters ();
                }
                catch (MamaStatus &e)
                {
                    mama_log (MAMA_LOG_LEVEL_WARN,
                              "MamdaCurrencyListener: Could not set field updaters: %s",
                              e.toString ());
                    wthread_mutex_unlock (&mCurrencyFieldUpdaterLockMutex);
                    return;
                }
                mUpdatersComplete = true;
            }

            wthread_mutex_unlock (&mCurrencyFieldUpdaterLockMutex);
        }

        // Handle fields in message:
        updateFieldStates    ();
        updateCurrencyFields (msg);

        // Handle according to message type:
        switch (msgType)
        {
        case MAMA_MSG_TYPE_INITIAL:
        case MAMA_MSG_TYPE_RECAP:
            handleRecap (subscription, msg);
            break;
        case MAMA_MSG_TYPE_UPDATE:
        case MAMA_MSG_TYPE_QUOTE:
            handleUpdate (subscription, msg);
            break;
        }
    }

    void MamdaCurrencyListener::
    MamdaCurrencyListenerImpl::handleRecap (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg)
    {
        if (mHandler)
        {
            mHandler->onCurrencyRecap (subscription, 
                                       mListener, 
                                       msg, 
                                       mListener);
        }
    }


    void MamdaCurrencyListener::
    MamdaCurrencyListenerImpl::handleUpdate (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg)
    {
        if (mHandler)
        {
            mHandler->onCurrencyUpdate (subscription, 
                                        mListener, 
                                        msg, 
                                        mListener, 
                                        mListener);
        }
    }

    void MamdaCurrencyListener::
        MamdaCurrencyListenerImpl::updateFieldStates()
    {
        if (mSymbolFieldState == MODIFIED)    
            mSymbolFieldState = NOT_MODIFIED;

        if (mPartIdFieldState == MODIFIED)   
            mPartIdFieldState = NOT_MODIFIED;
      
        if (mBidPriceFieldState == MODIFIED)    
            mBidPriceFieldState = NOT_MODIFIED;

        if (mAskPriceFieldState == MODIFIED)    
            mAskPriceFieldState = NOT_MODIFIED;  

    } 
      
    void MamdaCurrencyListener::
    MamdaCurrencyListenerImpl::updateCurrencyFields (
        const MamaMsg&  msg)
    { 
        const char* symbol = NULL;
        const char* partId = NULL;
        
        getSymbolAndPartId (msg, symbol, partId);

        if (symbol) 
        {
            mSymbol = symbol;
            mSymbolFieldState = MODIFIED;
        }

        if (partId) 
        {
            mPartId = partId;
            mPartIdFieldState = MODIFIED;
        }

        // Iterate over all of the fields in the message.
        msg.iterateFields (*this, NULL, NULL);
    }

    void MamdaCurrencyListener::MamdaCurrencyListenerImpl::onField (
        const MamaMsg&       msg,
        const MamaMsgField&  field,
        void*                closure)
    {
        uint16_t fid = field.getFid();
        if (fid <= mFieldUpdatersSize)
        {
            CurrencyFieldUpdate* updater = mFieldUpdaters[fid];
            if (updater)
            {
                updater->onUpdate (*this, field);
            }
        }
    }

    struct MamdaCurrencyListener::MamdaCurrencyListenerImpl::FieldUpdateBidPrice
        : public CurrencyFieldUpdate
    {
        void onUpdate (MamdaCurrencyListener::MamdaCurrencyListenerImpl&  impl,
                       const MamaMsgField&                                field)
        {
            MamaPrice bidPrice;
            field.getPrice (bidPrice);

            if ((impl.mBidPrice != bidPrice) && (bidPrice.getValue() != 0))
            {
                impl.mBidPrice           = bidPrice;
                impl.mBidPriceFieldState = MODIFIED;
            }
        }
    };

    struct MamdaCurrencyListener::MamdaCurrencyListenerImpl::FieldUpdateAskPrice 
        : public CurrencyFieldUpdate
    {
        void onUpdate (MamdaCurrencyListener::MamdaCurrencyListenerImpl&  impl,
                       const MamaMsgField&                                field)
        {
            MamaPrice askPrice;
            field.getPrice (askPrice);
            if ((impl.mAskPrice != askPrice) && (askPrice.getValue() != 0))
            {
                impl.mAskPrice           = askPrice;
                impl.mAskPriceFieldState = MODIFIED;
            }
        }
    };

    CurrencyFieldUpdate** 
        MamdaCurrencyListener::MamdaCurrencyListenerImpl::mFieldUpdaters = NULL;

    volatile uint16_t 
        MamdaCurrencyListener::MamdaCurrencyListenerImpl::mFieldUpdatersSize = 0;

    pthread_mutex_t 
        MamdaCurrencyListener::MamdaCurrencyListenerImpl::mCurrencyFieldUpdaterLockMutex 
        = PTHREAD_MUTEX_INITIALIZER;

    bool MamdaCurrencyListener::MamdaCurrencyListenerImpl::mUpdatersComplete = false;

    void MamdaCurrencyListener::MamdaCurrencyListenerImpl::initFieldUpdaters ()
    {
        if (!mFieldUpdaters)
        {
            mFieldUpdaters = 
                new CurrencyFieldUpdate* [MamdaCurrencyFields::getMaxFid() +1];
            mFieldUpdatersSize = MamdaCurrencyFields::getMaxFid();

            /* Use uint32_t instead of uint16_t to avoid infinite loop if max FID = 65535 */
            for (uint32_t i = 0; i <= mFieldUpdatersSize; ++i)
            {
                mFieldUpdaters[i] = NULL;
            }
        }

        initFieldUpdater (MamdaCurrencyFields::BID_PRICE,
                          new MamdaCurrencyListener::
                          MamdaCurrencyListenerImpl::FieldUpdateBidPrice);

        initFieldUpdater (MamdaCurrencyFields::ASK_PRICE,
                          new MamdaCurrencyListener::
                          MamdaCurrencyListenerImpl::FieldUpdateAskPrice);         
       
    }

    void MamdaCurrencyListener::MamdaCurrencyListenerImpl::initFieldUpdater (
        const MamaFieldDescriptor*  fieldDesc,
        CurrencyFieldUpdate*        updater)
    {
        if (!fieldDesc)
            return;

        uint16_t fid = fieldDesc->getFid();
        if (fid <= mFieldUpdatersSize)
        {
            mFieldUpdaters[fid] = updater;
        }
    }

} // namespace
