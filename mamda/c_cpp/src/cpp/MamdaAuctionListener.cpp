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
#include <mamda/MamdaAuctionListener.h>
#include <mamda/MamdaAuctionHandler.h>
#include <mamda/MamdaCommonFields.h>
#include <mamda/MamdaAuctionFields.h>
#include <mamda/MamdaUncrossPriceInd.h>
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

    struct AuctionFieldUpdate
    {
        virtual void onUpdate (
            MamdaAuctionListener::MamdaAuctionListenerImpl&  impl,
            const MamaMsgField&                              field) = 0;
    };

    class MamdaAuctionListener::MamdaAuctionListenerImpl
        : public MamaMsgFieldIterator
    {
    public:

        MamdaAuctionListenerImpl   (MamdaAuctionListener&  listener);

        ~MamdaAuctionListenerImpl  () {}

        void clearCache            ();

        void handleAuctionMessage  (MamdaSubscription*  subscription,
                                    const MamaMsg&      msg,
                                    short               msgType);

        void handleRecap           (MamdaSubscription*  subscription,
                                    const MamaMsg&      msg);

        void handleUpdate          (MamdaSubscription*  subscription,
                                    const MamaMsg&      msg);

        void onField               (const MamaMsg&      msg,
                                    const MamaMsgField& field,
                                    void*               closure);
                                           
        static MamdaUncrossPriceInd getUncrossPriceInd (
            const MamaMsgField&  field);
        
        void updateAuctionFields   (const MamaMsg&      msg);
        void updateFieldStates     ();

        MamdaAuctionListener&  mListener;
        MamdaAuctionHandler*   mHandler;
        
        /*
         * NOTE: fields which are enums can be pubished as integers if feedhandler
         * uses mama-publish-enums-as-ints.  It may also be possible for a feed to
         * publish the numerical value as a string. All enumerated fields must be handled
         * by getting the value based on the field type.
         */

        // Basic Event Fields 
        string                 mSymbol;                 MamdaFieldState     mSymbolFieldState;      
        string                 mPartId;                 MamdaFieldState     mPartIdFieldState;    
        MamaDateTime           mSrcTime;                MamdaFieldState     mSrcTimeFieldState;     
        MamaDateTime           mActTime;                MamdaFieldState     mActTimeFieldState;
        MamaDateTime           mLineTime;               MamdaFieldState     mLineTimeFieldState;    
        MamaDateTime           mSendTime;               MamdaFieldState     mSendTimeFieldState;
        mama_seqnum_t          mEventSeqNum;            MamdaFieldState     mEventSeqNumFieldState;
        MamaDateTime           mEventTime;              MamdaFieldState     mEventTimeFieldState;
        MamaMsgQual            mMsgQual;                MamdaFieldState     mMsgQualFieldState;
        
        
        // Auction Fields 
        // The following fields are used for caching the last reported
        // fundamental equity pricing/analysis attributes, indicators and ratios.
        // The reason for caching these values is to allow a configuration that 
        // passes the minimum amount of data  (unchanged fields not sent).
        MamaPrice              mUncrossPrice;           MamdaFieldState     mUncrossPriceFieldState;
        mama_quantity_t        mUncrossVolume;          MamdaFieldState     mUncrossVolumeFieldState;
        MamdaUncrossPriceInd   mUncrossPriceInd;        MamdaFieldState     mUncrossPriceIndFieldState;
        MamaDateTime           mAuctionTime;            MamdaFieldState     mAuctionTimeFieldState;

        bool mInitialised;
        
        static void initFieldUpdaters ();

        static void initFieldUpdater  (const MamaFieldDescriptor*  fieldDesc,
                                       AuctionFieldUpdate*         updater);
        static void updateField (
            MamdaAuctionListener::MamdaAuctionListenerImpl&  impl,
            const MamaMsgField&                              field);

        static AuctionFieldUpdate**         mFieldUpdaters;
        static volatile uint16_t            mFieldUpdatersSize;
        static wthread_static_mutex_t       mAuctionFieldUpdaterLockMutex;

        struct FieldUpdateAuctionTime;
        struct FieldUpdateAuctionSrcTime;
        struct FieldUpdateAuctionActTime;
        struct FieldUpdateAuctionLineTime;  
        struct FieldUpdateAuctionSendTime;    
        
        struct FieldUpdateUncrossPrice;
        struct FieldUpdateUncrossVolume;
        struct FieldUpdateUncrossPriceInd;
    };


    MamdaAuctionListener::MamdaAuctionListener ()
        : mImpl (*new MamdaAuctionListenerImpl(*this))
    {
    }

    MamdaAuctionListener::~MamdaAuctionListener()
    {
        /* Do not call wthread_mutex_destroy for the FieldUpdaterLockMutex here.  
           If we do, it will not be initialized again if another listener is created 
           after the first is destroyed. */
        /* wthread_mutex_destroy (&mImpl.mAuctionFieldUpdaterLockMutex); */
        delete &mImpl;
    }

    void MamdaAuctionListener::addHandler (MamdaAuctionHandler*  handler)
    {
        mImpl.mHandler = handler;
    }

    const char* MamdaAuctionListener::getSymbol() const
    {
        return mImpl.mSymbol.c_str();
    }

    const char* MamdaAuctionListener::getPartId() const
    {
        return mImpl.mPartId.c_str();
    }

    const MamaDateTime& MamdaAuctionListener::getSrcTime() const
    {
        return mImpl.mSrcTime;
    }

    const MamaDateTime& MamdaAuctionListener::getActivityTime() const
    {
        return mImpl.mActTime;
    }

    const MamaDateTime& MamdaAuctionListener::getLineTime() const
    {
        return mImpl.mLineTime;
    }

    const MamaDateTime& MamdaAuctionListener::getSendTime() const
    {
        return mImpl.mSendTime;
    }

    const MamaDateTime& MamdaAuctionListener::getAuctionTime() const
    {
        return mImpl.mAuctionTime;
    }

    mama_seqnum_t MamdaAuctionListener::getEventSeqNum() const
    {
        return mImpl.mEventSeqNum;
    }

    const MamaDateTime& MamdaAuctionListener::getEventTime() const
    {
        return mImpl.mEventTime;
    }

    const MamaMsgQual& MamdaAuctionListener::getMsgQual() const
    {
        return mImpl.mMsgQual;
    }


    /*      FieldAccessors      */
    MamdaFieldState MamdaAuctionListener::getSymbolFieldState() const
    {
      return mImpl.mSymbolFieldState;
    }

    MamdaFieldState MamdaAuctionListener::getPartIdFieldState() const
    {
      return mImpl.mPartIdFieldState;
    }

    MamdaFieldState MamdaAuctionListener::getSrcTimeFieldState() const
    {
      return mImpl.mSrcTimeFieldState;
    }

    MamdaFieldState MamdaAuctionListener::getActivityTimeFieldState() const
    {
      return mImpl.mActTimeFieldState;
    }

    MamdaFieldState MamdaAuctionListener::getLineTimeFieldState() const
    {
      return mImpl.mLineTimeFieldState;
    }

    MamdaFieldState MamdaAuctionListener::getSendTimeFieldState() const
    {
      return mImpl.mSendTimeFieldState;
    }

    MamdaFieldState MamdaAuctionListener::getEventSeqNumFieldState() const
    {
        return mImpl.mEventSeqNumFieldState;
    }

    MamdaFieldState MamdaAuctionListener::getEventTimeFieldState() const
    {
        return mImpl.mEventTimeFieldState;
    }

    MamdaFieldState MamdaAuctionListener::getMsgQualFieldState() const
    {
        return mImpl.mMsgQualFieldState;
    }
    /*  End FieldAccessors  */


    const MamaPrice& MamdaAuctionListener::getUncrossPrice() const
    {
        return mImpl.mUncrossPrice;
    }

    mama_quantity_t MamdaAuctionListener::getUncrossVolume() const
    {
        return mImpl.mUncrossVolume;
    }

    MamdaUncrossPriceInd MamdaAuctionListener::getUncrossPriceInd() const
    {
        return mImpl.mUncrossPriceInd;
    }


    MamdaFieldState MamdaAuctionListener::getAuctionTimeFieldState() const
    {
      return mImpl.mAuctionTimeFieldState;
    }

    bool MamdaAuctionListener::isInitialised() const
    {
        return mImpl.mInitialised;
    }


    /*      FieldState Accessors        */
    MamdaFieldState MamdaAuctionListener::getUncrossPriceFieldState () const
    {
      return mImpl.mUncrossPriceFieldState;
    }
     
    MamdaFieldState MamdaAuctionListener::getUncrossVolumeFieldState () const
    {
      return mImpl.mUncrossVolumeFieldState;
    }

    MamdaFieldState MamdaAuctionListener::getUncrossPriceIndFieldState () const
    {
      return mImpl.mUncrossPriceIndFieldState;
    }
    /*      End FieldState Accessors    */


    void MamdaAuctionListener::onMsg (
        MamdaSubscription* subscription,
        const MamaMsg&     msg,
        short              msgType)
    { 
        // If msg is a trade-related message, invoke the
        // appropriate callback:
        if(subscription->checkDebugLevel (MAMA_LOG_LEVEL_FINE))
        {
            const char *contractSymbol = "N/A";

            msg.tryString (
                MamdaCommonFields::ISSUE_SYMBOL, 
                contractSymbol);

            mama_forceLog (
                MAMA_LOG_LEVEL_FINE,
                "MamdaAuctionListener (%s.%s(%s)) onMsg(). "
                "msg type: %s msg status %s\n",
                subscription->getSource (),
                subscription->getSymbol (),
                contractSymbol,
                msg.getMsgTypeName (),
                msg.getMsgStatusString ());
        }

        switch (msgType)
        {
            case MAMA_MSG_TYPE_INITIAL:
            case MAMA_MSG_TYPE_RECAP:
            case MAMA_MSG_TYPE_UPDATE:
                mImpl.handleAuctionMessage (
                    subscription, 
                    msg, 
                    msgType);
                break;
        }
    }


    MamdaAuctionListener::
    MamdaAuctionListenerImpl::MamdaAuctionListenerImpl(
        MamdaAuctionListener&  listener)
        : mListener (listener)
        , mHandler  (NULL)
    {
        clearCache();
    }

    void MamdaAuctionListener::MamdaAuctionListenerImpl::clearCache ()
    {
        mSymbol = "";
        mPartId = "";

        mSrcTime.clear      ();                
        mActTime.clear      ();                
        mLineTime.clear     ();               
        mSendTime.clear     ();       
        mEventTime.clear    ();         
        mAuctionTime.clear  ();
        mUncrossPrice.clear ();

        mUncrossVolume   = 0;
        mUncrossPriceInd = UNCROSS_NONE;

        mInitialised = false;

        mSymbolFieldState          = NOT_INITIALISED;
        mPartIdFieldState          = NOT_INITIALISED;
        mSrcTimeFieldState         = NOT_INITIALISED;
        mActTimeFieldState         = NOT_INITIALISED;
        mLineTimeFieldState        = NOT_INITIALISED;
        mSendTimeFieldState        = NOT_INITIALISED;    
        mEventTimeFieldState       = NOT_INITIALISED;    
        
        mUncrossPriceFieldState    = NOT_INITIALISED;     
        mUncrossVolumeFieldState   = NOT_INITIALISED;
        mUncrossPriceIndFieldState = NOT_INITIALISED; 
        mAuctionTimeFieldState     = NOT_INITIALISED;                
    }

    void MamdaAuctionListener::
    MamdaAuctionListenerImpl::handleAuctionMessage (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg,
        short               msgType)
    {
        mInitialised = true;
        // Ensure that the field handling is set up (once for all
        // MamdaAuctionListener instances).
        if (mFieldUpdatersSize == 0)
        {
            wthread_static_mutex_lock (&mAuctionFieldUpdaterLockMutex);

            if (mFieldUpdatersSize == 0)
            {
                if (!MamdaAuctionFields::isSet())
                {
                    mama_log (
                        MAMA_LOG_LEVEL_WARN,
                        "MamdaAuctionListener: MamdaAuctionFields::setDictionary() "
                        "has not been called.");
                    wthread_static_mutex_unlock (&mAuctionFieldUpdaterLockMutex);
                    return;
                }

                try
                {
                    initFieldUpdaters();
                }
                catch (MamaStatus &e)
                {
                    mama_log (
                        MAMA_LOG_LEVEL_WARN,
                        "MamdaAuctionListener: Could not set field updaters: %s",
                        e.toString ());
                    wthread_static_mutex_unlock (&mAuctionFieldUpdaterLockMutex);
                    return;
                }
            }

            wthread_static_mutex_unlock (&mAuctionFieldUpdaterLockMutex);
        }

        // Handle fields in message:
        updateFieldStates   ();
        updateAuctionFields (msg);

        // Handle according to message type:
        switch (msgType)
        {
            case MAMA_MSG_TYPE_INITIAL:
            case MAMA_MSG_TYPE_RECAP:
                handleRecap (subscription, msg);
                break;
            case MAMA_MSG_TYPE_UPDATE:
                handleUpdate (subscription, msg);
                break;
        }
    }

    void MamdaAuctionListener::
    MamdaAuctionListenerImpl::handleRecap (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg)
    {
        if (mHandler)
        {
            mHandler->onAuctionRecap (
                subscription, 
                mListener, 
                msg, 
                mListener);
        }
    }


    void MamdaAuctionListener::
    MamdaAuctionListenerImpl::handleUpdate (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg)
    {
        // should probably check if cache has changed 
        // prior to calling any handlers
        if (mHandler)
        {
            mHandler->onAuctionUpdate (
                subscription, 
                mListener, 
                msg, 
                mListener, 
                mListener);
        }
    }

    void MamdaAuctionListener::
        MamdaAuctionListenerImpl::updateFieldStates()
    {
        if (mSymbolFieldState == MODIFIED)    
            mSymbolFieldState = NOT_MODIFIED;
 
        if (mPartIdFieldState == MODIFIED)   
            mPartIdFieldState = NOT_MODIFIED;

        if (mSrcTimeFieldState == MODIFIED)
            mSrcTimeFieldState = NOT_MODIFIED;

        if (mActTimeFieldState == MODIFIED)
            mActTimeFieldState = NOT_MODIFIED;

        if (mLineTimeFieldState == MODIFIED)
            mLineTimeFieldState = NOT_MODIFIED;

        if (mSendTimeFieldState == MODIFIED)
            mSendTimeFieldState = NOT_MODIFIED;

        if (mEventSeqNumFieldState == MODIFIED)
            mEventSeqNumFieldState = NOT_MODIFIED;

        if (mEventTimeFieldState == MODIFIED)
            mEventTimeFieldState = NOT_MODIFIED;    
        
        if (mUncrossPriceFieldState == MODIFIED)    
            mUncrossPriceFieldState = NOT_MODIFIED;

        if (mUncrossVolumeFieldState == MODIFIED)    
            mUncrossVolumeFieldState = NOT_MODIFIED;

        if (mUncrossPriceIndFieldState == MODIFIED)    
            mUncrossPriceIndFieldState = NOT_MODIFIED;

        if (mAuctionTimeFieldState == MODIFIED)
            mAuctionTimeFieldState = NOT_MODIFIED;
    } 
      
    void MamdaAuctionListener::MamdaAuctionListenerImpl::updateAuctionFields (
        const MamaMsg&  msg)
    { 
        const char* symbol = NULL;
        const char* partId = NULL;
        
        getSymbolAndPartId (
            msg, 
            symbol, 
            partId);

        if (symbol) 
        {
            mSymbol           = symbol;
            mSymbolFieldState = MODIFIED;
        }

        if (partId) 
        {
            mPartId           = partId;
            mPartIdFieldState = MODIFIED;
        }

        // Iterate over all of the fields in the message.
        msg.iterateFields (*this, NULL, NULL);
    }

    void MamdaAuctionListener::MamdaAuctionListenerImpl::onField (
        const MamaMsg&       msg,
        const MamaMsgField&  field,
        void*                closure)
    {
        uint16_t fid = field.getFid();
        if (fid <= mFieldUpdatersSize)
        {
            AuctionFieldUpdate* updater = mFieldUpdaters[fid];
            if (updater)
            {
                updater->onUpdate (*this, field);
            }
        }
    }


    MamdaUncrossPriceInd
    MamdaAuctionListener::MamdaAuctionListenerImpl::getUncrossPriceInd(
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
                return (MamdaUncrossPriceInd) field.getU32();

            case MAMA_FIELD_TYPE_STRING:
                return mamdaUncrossPriceIndFromString (field.getString());

            default:
                mama_log (
                    MAMA_LOG_LEVEL_FINE,
                    "Unhandled type %d for wUncrossPriceInd.  "
                    "Expected string or integer.",
                    field.getType());
                break;
        }

        return UNCROSS_NONE;
    }


    struct MamdaAuctionListener::MamdaAuctionListenerImpl::FieldUpdateAuctionTime
        : public AuctionFieldUpdate
    {
        void onUpdate (
            MamdaAuctionListener::MamdaAuctionListenerImpl&  impl,
            const MamaMsgField&                              field)
        {
            field.getDateTime (impl.mAuctionTime);
            impl.mAuctionTimeFieldState = MODIFIED;
        }
    };

    struct MamdaAuctionListener::MamdaAuctionListenerImpl::FieldUpdateAuctionSrcTime 
        : public AuctionFieldUpdate
    {
        void onUpdate (
            MamdaAuctionListener::MamdaAuctionListenerImpl&  impl,
            const MamaMsgField&                              field)
        {
            field.getDateTime (impl.mSrcTime);
            impl.mSrcTimeFieldState = MODIFIED;
        }
    };

    struct MamdaAuctionListener::MamdaAuctionListenerImpl::FieldUpdateAuctionSendTime
        : public AuctionFieldUpdate
    {
        void onUpdate (
            MamdaAuctionListener::MamdaAuctionListenerImpl&  impl,
            const MamaMsgField&                              field)
        {
            field.getDateTime (impl.mSendTime);
            impl.mSendTimeFieldState = MODIFIED;
        }
    };

    struct MamdaAuctionListener::MamdaAuctionListenerImpl::FieldUpdateAuctionActTime 
        : public AuctionFieldUpdate
    {
        void onUpdate (
            MamdaAuctionListener::MamdaAuctionListenerImpl&  impl,
            const MamaMsgField&                              field)
        {
            field.getDateTime (impl.mActTime);
            impl.mActTimeFieldState = MODIFIED;
        }
    };

    struct MamdaAuctionListener::MamdaAuctionListenerImpl::FieldUpdateAuctionLineTime 
        : public AuctionFieldUpdate
    {
        void onUpdate (
            MamdaAuctionListener::MamdaAuctionListenerImpl&  impl,
            const MamaMsgField&                              field)
        {
            field.getDateTime (impl.mLineTime);
            impl.mLineTimeFieldState = MODIFIED;
        }
    };

    struct MamdaAuctionListener::MamdaAuctionListenerImpl::FieldUpdateUncrossPrice
        : public AuctionFieldUpdate
    {
        void onUpdate (
            MamdaAuctionListener::MamdaAuctionListenerImpl&  impl,
            const MamaMsgField&                              field)
        {
            MamaPrice uncrossPrice;
            field.getPrice (uncrossPrice);

            if ((impl.mUncrossPrice != uncrossPrice) && (uncrossPrice.getValue() != 0))
            {
                impl.mUncrossPrice           = uncrossPrice;
                impl.mUncrossPriceFieldState = MODIFIED;
            }
        }
    };

    struct MamdaAuctionListener::MamdaAuctionListenerImpl::FieldUpdateUncrossVolume 
        : public AuctionFieldUpdate
    {
        void onUpdate (
            MamdaAuctionListener::MamdaAuctionListenerImpl&  impl,
            const MamaMsgField&                              field)
        {
            mama_quantity_t uncrossVol = field.getF64();

            if (impl.mUncrossVolume != uncrossVol)
            {
                impl.mUncrossVolume           = uncrossVol;
                impl.mUncrossVolumeFieldState = MODIFIED;
            }
        }
    };

    struct MamdaAuctionListener::MamdaAuctionListenerImpl::FieldUpdateUncrossPriceInd 
        : public AuctionFieldUpdate
    {
        void onUpdate (
            MamdaAuctionListener::MamdaAuctionListenerImpl&  impl,
            const MamaMsgField&                              field)
        {
            MamdaUncrossPriceInd uncrossInd = impl.getUncrossPriceInd (field);

            if (impl.mUncrossPriceInd != uncrossInd)
            {
                impl.mUncrossPriceInd           = uncrossInd;
                impl.mUncrossPriceIndFieldState = MODIFIED;
            }
        }
    };

    AuctionFieldUpdate** 
        MamdaAuctionListener::MamdaAuctionListenerImpl::mFieldUpdaters     = NULL;

    volatile uint16_t 
        MamdaAuctionListener::MamdaAuctionListenerImpl::mFieldUpdatersSize = 0;

    wthread_static_mutex_t MamdaAuctionListener::MamdaAuctionListenerImpl::
        mAuctionFieldUpdaterLockMutex = WSTATIC_MUTEX_INITIALIZER;


    void MamdaAuctionListener::MamdaAuctionListenerImpl::initFieldUpdaters ()
    {
        if (!mFieldUpdaters)
        {
            mFieldUpdaters = 
                new AuctionFieldUpdate* [MamdaAuctionFields::getMaxFid() + 1];

            mFieldUpdatersSize = MamdaAuctionFields::getMaxFid();

            /* Use uint32_t instead of uint16_t to avoid infinite 
               loop if max FID = 65535 */
            for (uint32_t i = 0; i <= mFieldUpdatersSize; ++i)
            {
                mFieldUpdaters[i] = NULL;
            }
        }

        initFieldUpdater(
            MamdaAuctionFields::AUCTION_TIME, 
            new MamdaAuctionListener::MamdaAuctionListenerImpl::FieldUpdateAuctionTime);

        initFieldUpdater(
            MamdaCommonFields::SRC_TIME,
            new MamdaAuctionListener::MamdaAuctionListenerImpl::FieldUpdateAuctionSrcTime);

        initFieldUpdater(
            MamdaCommonFields::ACTIVITY_TIME,
            new MamdaAuctionListener::MamdaAuctionListenerImpl::FieldUpdateAuctionActTime);   
      
        initFieldUpdater(
            MamdaCommonFields::LINE_TIME,
            new MamdaAuctionListener::MamdaAuctionListenerImpl::FieldUpdateAuctionLineTime); 

        initFieldUpdater(
            MamdaCommonFields::SEND_TIME,
            new MamdaAuctionListener::MamdaAuctionListenerImpl::FieldUpdateAuctionSendTime);   
      
        initFieldUpdater(
            MamdaCommonFields::LINE_TIME,
            new MamdaAuctionListener::MamdaAuctionListenerImpl::FieldUpdateAuctionLineTime); 

        initFieldUpdater(
            MamdaAuctionFields::UNCROSS_PRICE, 
            new MamdaAuctionListener::MamdaAuctionListenerImpl::FieldUpdateUncrossPrice);

        initFieldUpdater(
            MamdaAuctionFields::UNCROSS_VOLUME,
            new MamdaAuctionListener::MamdaAuctionListenerImpl::FieldUpdateUncrossVolume); 
        
        initFieldUpdater(
            MamdaAuctionFields::UNCROSS_PRICE_IND,
            new MamdaAuctionListener::MamdaAuctionListenerImpl::FieldUpdateUncrossPriceInd);    
    }

    void MamdaAuctionListener::MamdaAuctionListenerImpl::initFieldUpdater (
        const MamaFieldDescriptor*  fieldDesc,
        AuctionFieldUpdate*           updater)
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
