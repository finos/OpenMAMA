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

#include <mamda/MamdaSecStatusListener.h>
#include <mamda/MamdaSecStatusFields.h>
#include <mamda/MamdaSecStatusHandler.h>
#include <mamda/MamdaCommonFields.h>
#include <mamda/MamdaSubscription.h>
#include <mamda/MamdaSecurityStatus.h>
#include <mamda/MamdaSecurityStatusQual.h>
#include <mama/mamacpp.h>
#include "MamdaUtils.h"
#include <string>
#include <iostream>

using std::string;

namespace Wombat
{

    struct SecStatusFieldUpdate
    {
        virtual void onUpdate (
            MamdaSecStatusListener::MamdaSecStatusListenerImpl&  impl,
            const MamaMsgField&                                  field) = 0;
    };

    struct SecStatusCache
    {
        /*
         * NOTE: fields which are enums can be pubished as integers if feedhandler
         * uses mama-publish-enums-as-ints.  It may also be possible for a feed to
         * publish the numerical value as a string. All enumerated fields must be handled
         * by getting the value based on the field type.
         */

        string                  mIssueSymbol;              MamdaFieldState     mIssueSymbolFieldState;
        string                  mSymbol;                   MamdaFieldState     mSymbolFieldState;
        string                  mPartId;                   MamdaFieldState     mPartIdFieldState;
        MamaDateTime            mSrcTime;                  MamdaFieldState     mSrcTimeFieldState; 
        MamaDateTime            mActTime;                  MamdaFieldState     mActTimeFieldState; 
        MamaDateTime            mLineTime;                 MamdaFieldState     mLineTimeFieldState;
        MamaDateTime            mSendTime;                 MamdaFieldState     mSendTimeFieldState;
        MamaMsgQual             mMsgQual;                  MamdaFieldState     mMsgQualFieldState; 
        string                  mReason;                   MamdaFieldState     mReasonFieldState; 
        char                    mSecurityAction;           MamdaFieldState     mSecurityActionFieldState;
        string                  mSecurityType;             MamdaFieldState     mSecurityTypeFieldState; 
        MamdaSecurityStatus     mSecurityStatus;           MamdaFieldState     mSecurityStatusFieldState;
        char                    mShortSaleCircuitBreaker;  MamdaFieldState     mShortSaleCircuitBreakerFieldState;  
        MamdaSecurityStatusQual mSecurityStatusQual;       MamdaFieldState     mSecurityStatusQualFieldState;  
        string                  mSecurityStatusNative;     MamdaFieldState     mSecurityStatusNativeFieldState;
        MamaDateTime            mSecurityStatusTime;       MamdaFieldState     mSecurityStatusTimeFieldState; 
        string                  mFreeText;                 MamdaFieldState     mFreeTextFieldState; 
        mama_seqnum_t           mEventSeqNum;              MamdaFieldState     mEventSeqNumFieldState;
        mama_u32_t              mTmpSecStatusCount;        
        mama_u32_t              mSecStatusCount;           MamdaFieldState     mSecStatusCountFieldState;
        mama_seqnum_t           mGapBegin;                 MamdaFieldState     mGapBeginFieldState;
        mama_seqnum_t           mGapEnd;                   MamdaFieldState     mGapEndFieldState;
    };

    class MamdaSecStatusListener::MamdaSecStatusListenerImpl 
        : public MamaMsgFieldIterator
    {
    public:
        MamdaSecStatusListenerImpl  (MamdaSecStatusListener&  listener);
        ~MamdaSecStatusListenerImpl () {}

        void handleSecStatusUpdate        (MamdaSubscription*  subscription,
                                           const MamaMsg&      msg,
                                           short               msgType);

        void handleUpdate                 (MamdaSubscription*  subscription,
                                           const MamaMsg&      msg);

        void handleRecap                  (MamdaSubscription*  subscription,
                                           const MamaMsg&      msg);

        void onField                      (const MamaMsg&      msg,
                                           const MamaMsgField& field,
                                           void*               closure);

        void clearCache                   (SecStatusCache&     cache);

        void updateSecStatusFields        (const MamaMsg&      msg);

        void updateFieldStates            ();

        void checkSecStatusCount          (MamdaSubscription*  subscription,
                                           const MamaMsg&      msg,
                                           bool                checkForGap);

        MamdaSecStatusListener&   mListener;
        MamdaSecStatusHandler*    mHandler;

        typedef struct SecStatusLock_
        {
            wthread_mutex_t   mSecStatusMutex;

        } SecStatusLock;

        //thread locking    
        SecStatusLock             mSecStatusLock;

        // The SecStatusCache Data Caches
        SecStatusCache            mRegularCache;     // Regular update cache
        SecStatusCache            mTransientCache;   // Transient cache 
        SecStatusCache&           mSecStatusCache;   // Current cache in use

        static MamdaSecurityStatus      getSecurityStatus     (const MamaMsgField&  field);
        static MamdaSecurityStatusQual  getSecurityStatusQual (const MamaMsgField&  field);

        static void initFieldUpdaters ();
        static void initFieldUpdater  (
            const MamaFieldDescriptor*  fieldDesc,
            SecStatusFieldUpdate*       updater);

        static void updateField (
            MamdaSecStatusListener::MamdaSecStatusListenerImpl&  impl,
            const MamaMsgField&                                  field);

        static SecStatusFieldUpdate**  mFieldUpdaters;
        static volatile uint16_t       mFieldUpdatersSize;
        static wthread_static_mutex_t  mSecStatusFieldUpdaterLockMutex;
        static bool                    mUpdatersComplete;

        struct FieldUpdateSecStatusIssueSymbol;
        struct FieldUpdateSecStatusSrcTime;
        struct FieldUpdateSecStatusActTime;
        struct FieldUpdateSecStatusLineTime;
        struct FieldUpdateSecStatusPubId;
        struct FieldUpdateSecStatusPubClass;
        struct FieldUpdateSecStatusReason;
        struct FieldUpdateSecurityAction;
        struct FieldUpdateShortSaleCircuitBreaker;
        struct FieldUpdateSecurityType;
        struct FieldUpdateSecurityStatus;
        struct FieldUpdateSecurityStatusTime;
        struct FieldUpdateSecurityStatusQual;
        struct FieldUpdateSecurityStatusNative;
        struct FieldUpdateSecStatusFreeText;
        struct FieldUpdateSecStatusEventSeqNum;
        struct FieldUpdateMsgQual;

        bool updated;
    };

    MamdaSecStatusListener::MamdaSecStatusListener ()
        : mImpl (*new MamdaSecStatusListenerImpl(*this))
    {   
        wthread_mutex_init (&mImpl.mSecStatusLock.mSecStatusMutex, NULL);
    }

    MamdaSecStatusListener::~MamdaSecStatusListener()
    {
        wthread_mutex_destroy (&mImpl.mSecStatusLock.mSecStatusMutex);
	    /* Do not call wthread_mutex_destroy for the FieldUpdaterLockMutex here.  
	       If we do, it will not be initialized again if another listener is created 
	       after the first is destroyed. */
        /* wthread_mutex_destroy (&mImpl.mSecStatusFieldUpdaterLockMutex); */
        delete &mImpl;
    }

    void MamdaSecStatusListener::addHandler (MamdaSecStatusHandler*  handler)
    {
        mImpl.mHandler = handler;
    }

    const char* MamdaSecStatusListener::getIssueSymbol() const
    {
        return mImpl.mSecStatusCache.mIssueSymbol.c_str();
    }

    const char* MamdaSecStatusListener::getSymbol() const
    {
        return mImpl.mSecStatusCache.mSymbol.c_str();
    }

    const char* MamdaSecStatusListener::getPartId() const
    {
        return mImpl.mSecStatusCache.mPartId.c_str();
    }

    const MamaDateTime& MamdaSecStatusListener::getSrcTime() const
    {
        return mImpl.mSecStatusCache.mSrcTime;
    }
     
    const MamaDateTime& MamdaSecStatusListener::getActivityTime() const
    {
        return mImpl.mSecStatusCache.mActTime;
    }

    const MamaDateTime& MamdaSecStatusListener::getLineTime() const
    {
        return mImpl.mSecStatusCache.mLineTime;
    }

    const MamaDateTime& MamdaSecStatusListener::getSendTime() const
    {
        return mImpl.mSecStatusCache.mSendTime;
    }

    const MamaMsgQual& MamdaSecStatusListener::getMsgQual() const
    {
        return mImpl.mSecStatusCache.mMsgQual;
    }

    mama_seqnum_t MamdaSecStatusListener::getEventSeqNum() const
    {
        return mImpl.mSecStatusCache.mEventSeqNum;
    }

    const MamaDateTime& MamdaSecStatusListener::getEventTime() const
    {
        return mImpl.mSecStatusCache.mSecurityStatusTime;
    }

    const char* MamdaSecStatusListener::getReason() const
    {
        return mImpl.mSecStatusCache.mReason.c_str();
    }

    char MamdaSecStatusListener::getSecurityAction() const
    {
        return mImpl.mSecStatusCache.mSecurityAction;
    }

    char MamdaSecStatusListener::getShortSaleCircuitBreaker() const
    {
        return mImpl.mSecStatusCache.mShortSaleCircuitBreaker;
    }

    const char* MamdaSecStatusListener::getSecurityType() const
    {
        return mImpl.mSecStatusCache.mSecurityType.c_str();
    }

    const char* MamdaSecStatusListener::getSecurityStatus() const
    {
        return toString (mImpl.mSecStatusCache.mSecurityStatus);
    }

    const char* MamdaSecStatusListener::getSecurityStatusQual() const
    {
        return toString (mImpl.mSecStatusCache.mSecurityStatusQual);
    }

    const char* MamdaSecStatusListener::getSecurityStatusNative() const
    {
        return mImpl.mSecStatusCache.mSecurityStatusNative.c_str();
    }

    MamdaSecurityStatus MamdaSecStatusListener::getSecurityStatusEnum() const
    {
        return mImpl.mSecStatusCache.mSecurityStatus;
    }

    MamdaSecurityStatusQual MamdaSecStatusListener::getSecurityStatusQualifierEnum() const
    {
        return mImpl.mSecStatusCache.mSecurityStatusQual;
    }

    const char* MamdaSecStatusListener::getSecurityStatusStr() const
    {
        return toString (mImpl.mSecStatusCache.mSecurityStatus);
    }

    const char* MamdaSecStatusListener::getSecurityStatusQualStr() const
    {
        return toString (mImpl.mSecStatusCache.mSecurityStatusQual);
    }

    const char* MamdaSecStatusListener::getFreeText() const
    {
        return mImpl.mSecStatusCache.mFreeText.c_str();
    }

    /*  FieldState Accessors    */

    MamdaFieldState MamdaSecStatusListener::getIssueSymbolFieldState() const
    {
        return mImpl.mSecStatusCache.mIssueSymbolFieldState;
    }

    MamdaFieldState MamdaSecStatusListener::getSymbolFieldState() const
    {
        return mImpl.mSecStatusCache.mSymbolFieldState;
    }

    MamdaFieldState MamdaSecStatusListener::getPartIdFieldState() const
    {
        return mImpl.mSecStatusCache.mPartIdFieldState;
    }

    MamdaFieldState MamdaSecStatusListener::getSrcTimeFieldState() const
    {
        return mImpl.mSecStatusCache.mSrcTimeFieldState;
    }
     
    MamdaFieldState MamdaSecStatusListener::getActivityTimeFieldState() const
    {
        return mImpl.mSecStatusCache.mActTimeFieldState;
    }

    MamdaFieldState MamdaSecStatusListener::getLineTimeFieldState() const
    {
        return mImpl.mSecStatusCache.mLineTimeFieldState;
    }

    MamdaFieldState MamdaSecStatusListener::getSendTimeFieldState() const
    {
        return mImpl.mSecStatusCache.mSendTimeFieldState;
    }

    MamdaFieldState MamdaSecStatusListener::getMsgQualFieldState() const
    {
        return mImpl.mSecStatusCache.mMsgQualFieldState;
    }

    MamdaFieldState MamdaSecStatusListener::getEventSeqNumFieldState() const
    {
        return mImpl.mSecStatusCache.mEventSeqNumFieldState;
    }

    MamdaFieldState MamdaSecStatusListener::getEventTimeFieldState() const
    {
        return mImpl.mSecStatusCache.mSecurityStatusTimeFieldState;
    }

    MamdaFieldState MamdaSecStatusListener::getReasonFieldState() const
    {
        return mImpl.mSecStatusCache.mReasonFieldState;
    }

    MamdaFieldState MamdaSecStatusListener::getSecurityActionFieldState() const
    {
        return mImpl.mSecStatusCache.mSecurityActionFieldState;
    }

    MamdaFieldState MamdaSecStatusListener::getShortSaleCircuitBreakerFieldState() const
    {
        return mImpl.mSecStatusCache.mShortSaleCircuitBreakerFieldState;
    }

    MamdaFieldState MamdaSecStatusListener::getSecurityTypeFieldState() const
    {
        return mImpl.mSecStatusCache.mSecurityTypeFieldState;
    }

    MamdaFieldState MamdaSecStatusListener::getSecurityStatusFieldState() const
    {
        return mImpl.mSecStatusCache.mSecurityStatusFieldState;
    }

    MamdaFieldState MamdaSecStatusListener::getSecurityStatusQualFieldState() const
    {
        return mImpl.mSecStatusCache.mSecurityStatusQualFieldState;
    }

    MamdaFieldState MamdaSecStatusListener::getSecurityStatusEnumFieldState() const
    {
        return mImpl.mSecStatusCache.mSecurityStatusFieldState;
    }

    MamdaFieldState MamdaSecStatusListener::getSecurityStatusQualifierEnumFieldState() const
    {
        return mImpl.mSecStatusCache.mSecurityStatusQualFieldState;
    }

    MamdaFieldState MamdaSecStatusListener::getSecurityStatusStrFieldState() const
    {
        return mImpl.mSecStatusCache.mSecurityStatusFieldState;
    }

    MamdaFieldState MamdaSecStatusListener::getSecurityStatusQualStrFieldState() const
    {
        return mImpl.mSecStatusCache.mSecurityStatusQualFieldState;
    }

    MamdaFieldState MamdaSecStatusListener::getFreeTextFieldState() const
    {
        return mImpl.mSecStatusCache.mFreeTextFieldState;
    }

    MamdaFieldState MamdaSecStatusListener::getSecurityStatusNativeFieldState() const
    {
        return mImpl.mSecStatusCache.mSecurityStatusNativeFieldState;
    }
    /*  End FieldState Accessors    */


    void MamdaSecStatusListener::onMsg (
        MamdaSubscription* subscription,
        const MamaMsg&     msg,
        short              msgType)
    {
        // If msg is an update message, invoke the
        // appropriate callback:

        switch (msgType)
        {
            case MAMA_MSG_TYPE_INITIAL:
            case MAMA_MSG_TYPE_RECAP:
            case MAMA_MSG_TYPE_PREOPENING:
            case MAMA_MSG_TYPE_UPDATE:
            case MAMA_MSG_TYPE_QUOTE:
            case MAMA_MSG_TYPE_TRADE:
            case MAMA_MSG_TYPE_BOOK_UPDATE:
            case MAMA_MSG_TYPE_MISC:
            case MAMA_MSG_TYPE_SEC_STATUS:
                mImpl.handleSecStatusUpdate (subscription, msg, msgType);
                break;
            default:
                // Not an update message (ignored)
                break;
        }
    }

    MamdaSecStatusListener::MamdaSecStatusListenerImpl::MamdaSecStatusListenerImpl (
        MamdaSecStatusListener&  listener)
        : mListener       (listener)
        , mHandler        (NULL)
        , mSecStatusCache (mRegularCache)
    {
        clearCache (mRegularCache);
    }

    void MamdaSecStatusListener::MamdaSecStatusListenerImpl::handleSecStatusUpdate (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg,
        short               msgType)
    {
        // Ensure that the field handling is set up (once for all
        // MamdaSecStatusListener instances).
        if (!mUpdatersComplete)
        {
            wthread_static_mutex_lock (&mSecStatusFieldUpdaterLockMutex);

            if (!mUpdatersComplete)
            {
                if (!MamdaSecStatusFields::isSet())
                {
                     mama_log (MAMA_LOG_LEVEL_WARN,
                               "MamdaSecStatusListener: MamdaSecStatusFields::setDictionary() "
                               "has not been called");
                     wthread_static_mutex_unlock (&mSecStatusFieldUpdaterLockMutex);
                     return;
                }

                try
                {
                    initFieldUpdaters ();
                }
                catch (MamaStatus &e)
                {
                    mama_log (MAMA_LOG_LEVEL_WARN,
                              "MamdaSecStatusListener: Could not set field updaters: %s",
                              e.toString ());
                    wthread_static_mutex_unlock (&mSecStatusFieldUpdaterLockMutex);
                    return;
                }
                mUpdatersComplete = true;
            }

            wthread_static_mutex_unlock (&mSecStatusFieldUpdaterLockMutex);
        }

        // Process fields in message:
        updateFieldStates     ();
        updateSecStatusFields (msg);

        switch (msgType)
        {
            case MAMA_MSG_TYPE_INITIAL:
            case MAMA_MSG_TYPE_RECAP:
            case MAMA_MSG_TYPE_PREOPENING:
                handleRecap (subscription, msg);
                break;
            default:
                handleUpdate (subscription, msg);
                break;
        }
    }

    void MamdaSecStatusListener::MamdaSecStatusListenerImpl::handleRecap (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg)
    {
        // Generic update 
        if (mHandler)
        {
            mHandler->onSecStatusRecap (subscription, 
                                        mListener, 
                                        msg,
                                        mListener);
        }
    }

    void MamdaSecStatusListener::MamdaSecStatusListenerImpl::handleUpdate (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg)
    {
        if (mHandler)
        {
            if (updated)
        		mHandler->onSecStatusUpdate (subscription, 
                                             mListener, 
                                             msg);
        }
    }

    void MamdaSecStatusListener::MamdaSecStatusListenerImpl::clearCache (
                    SecStatusCache& secStatusCache)
    {
        secStatusCache.mIssueSymbol             = "";
        secStatusCache.mSymbol                  = "";
        secStatusCache.mPartId                  = "";
        secStatusCache.mSrcTime.clear();
        secStatusCache.mActTime.clear();
        secStatusCache.mLineTime.clear();
        secStatusCache.mSendTime.clear();
        secStatusCache.mReason                  = "";
        secStatusCache.mSecurityAction          = ' ';
        secStatusCache.mShortSaleCircuitBreaker = ' ';

        secStatusCache.mSecurityType            = ""; 
        secStatusCache.mSecurityStatus          = SECURITY_STATUS_UNKNOWN;
        secStatusCache.mSecurityStatusQual      = SECURITY_STATUS_QUAL_UNKNOWN;
        secStatusCache.mSecurityStatusNative    = "";
        secStatusCache.mSecurityStatusTime.clear();
        secStatusCache.mFreeText                = ""; 
        
        secStatusCache.mIssueSymbolFieldState           = NOT_INITIALISED;
        secStatusCache.mSymbolFieldState                = NOT_INITIALISED;
        secStatusCache.mPartIdFieldState                = NOT_INITIALISED;
        secStatusCache.mSrcTimeFieldState               = NOT_INITIALISED;
        secStatusCache.mActTimeFieldState               = NOT_INITIALISED;
        secStatusCache.mLineTimeFieldState              = NOT_INITIALISED;
        secStatusCache.mSendTimeFieldState              = NOT_INITIALISED;
        secStatusCache.mMsgQualFieldState               = NOT_INITIALISED;
        secStatusCache.mGapEndFieldState                = NOT_INITIALISED;
        secStatusCache.mSecStatusCountFieldState        = NOT_INITIALISED;
        secStatusCache.mReasonFieldState                = NOT_INITIALISED;
        secStatusCache.mEventSeqNumFieldState           = NOT_INITIALISED;
        secStatusCache.mSecStatusCountFieldState        = NOT_INITIALISED;
        secStatusCache.mSecurityActionFieldState        = NOT_INITIALISED;
        secStatusCache.mShortSaleCircuitBreakerFieldState = NOT_INITIALISED;
        secStatusCache.mGapBeginFieldState              = NOT_INITIALISED;

        secStatusCache.mSecurityTypeFieldState          = NOT_INITIALISED;
        secStatusCache.mSecurityStatusFieldState        = NOT_INITIALISED;
        secStatusCache.mSecurityStatusQualFieldState    = NOT_INITIALISED;
        secStatusCache.mSecurityStatusNativeFieldState  = NOT_INITIALISED;
        secStatusCache.mSecurityStatusTimeFieldState    = NOT_INITIALISED;
        secStatusCache.mFreeTextFieldState              = NOT_INITIALISED;
    }

    void MamdaSecStatusListener::
        MamdaSecStatusListenerImpl::updateFieldStates ()
    {
        if (mSecStatusCache.mIssueSymbolFieldState == MODIFIED)             
            mSecStatusCache.mIssueSymbolFieldState = NOT_MODIFIED; 
     
        if (mSecStatusCache.mSymbolFieldState == MODIFIED)             
            mSecStatusCache.mSymbolFieldState = NOT_MODIFIED; 

        if (mSecStatusCache.mPartIdFieldState == MODIFIED)          
            mSecStatusCache.mPartIdFieldState = NOT_MODIFIED;
     
        if (mSecStatusCache.mSrcTimeFieldState == MODIFIED)      
            mSecStatusCache.mSrcTimeFieldState = NOT_MODIFIED; 
     
        if (mSecStatusCache.mActTimeFieldState == MODIFIED)        
            mSecStatusCache.mActTimeFieldState = NOT_MODIFIED;  

        if (mSecStatusCache.mLineTimeFieldState == MODIFIED)       
            mSecStatusCache.mLineTimeFieldState = NOT_MODIFIED; 

        if (mSecStatusCache.mSendTimeFieldState == MODIFIED)       
            mSecStatusCache.mSendTimeFieldState = NOT_MODIFIED; 

        if (mSecStatusCache.mMsgQualFieldState == MODIFIED)        
            mSecStatusCache.mMsgQualFieldState = NOT_MODIFIED;  

        if (mSecStatusCache.mReasonFieldState == MODIFIED)         
            mSecStatusCache.mReasonFieldState = NOT_MODIFIED; 
     
        if (mSecStatusCache.mSecurityActionFieldState == MODIFIED)
            mSecStatusCache.mSecurityActionFieldState = NOT_MODIFIED; 

        if (mSecStatusCache.mShortSaleCircuitBreakerFieldState == MODIFIED)
            mSecStatusCache.mShortSaleCircuitBreakerFieldState = NOT_MODIFIED; 

        if (mSecStatusCache.mSecurityTypeFieldState == MODIFIED)    
            mSecStatusCache.mSecurityTypeFieldState = NOT_MODIFIED;  

        if (mSecStatusCache.mSecurityStatusFieldState == MODIFIED)  
            mSecStatusCache.mSecurityStatusFieldState = NOT_MODIFIED; 

        if (mSecStatusCache.mSecurityStatusQualFieldState == MODIFIED)   
            mSecStatusCache.mSecurityStatusQualFieldState = NOT_MODIFIED; 

        if (mSecStatusCache.mSecurityStatusNativeFieldState == MODIFIED)    
            mSecStatusCache.mSecurityStatusNativeFieldState = NOT_MODIFIED;

        if (mSecStatusCache.mSecurityStatusTimeFieldState == MODIFIED)      
            mSecStatusCache.mSecurityStatusTimeFieldState = NOT_MODIFIED; 

        if (mSecStatusCache.mFreeTextFieldState == MODIFIED)                
            mSecStatusCache.mFreeTextFieldState = NOT_MODIFIED;  

        if (mSecStatusCache.mEventSeqNumFieldState == MODIFIED)     
            mSecStatusCache.mEventSeqNumFieldState = NOT_MODIFIED; 

        if (mSecStatusCache.mSecStatusCountFieldState == MODIFIED)        
            mSecStatusCache.mSecStatusCountFieldState = NOT_MODIFIED; 

        if (mSecStatusCache.mGapBeginFieldState == MODIFIED)             
            mSecStatusCache.mGapBeginFieldState = NOT_MODIFIED; 

        if (mSecStatusCache.mGapEndFieldState == MODIFIED)         
            mSecStatusCache.mGapEndFieldState = NOT_MODIFIED; 
    }

    void MamdaSecStatusListener::
    MamdaSecStatusListenerImpl::updateSecStatusFields (
        const MamaMsg&  msg)
    {
        const char* symbol = NULL;
        const char* partId = NULL;

        getSymbolAndPartId (msg, symbol, partId);

        if (symbol) 
        {
          mSecStatusCache.mSymbol           = symbol;
          mSecStatusCache.mSymbolFieldState = MODIFIED;
        }

        if (partId) 
        {
          mSecStatusCache.mPartId           = partId;
          mSecStatusCache.mPartIdFieldState = MODIFIED;
        }

        wthread_mutex_lock (&mSecStatusLock.mSecStatusMutex);

        // Iterate over all of the fields in the message.
        updated = false;
        msg.iterateFields (*this, NULL, NULL);
        wthread_mutex_unlock (&mSecStatusLock.mSecStatusMutex);
    }

    void MamdaSecStatusListener::MamdaSecStatusListenerImpl::onField (
        const MamaMsg&       msg,
        const MamaMsgField&  field,
        void*                closure)
    {
        uint16_t fid = field.getFid();
        if (fid <= mFieldUpdatersSize)
        {
            SecStatusFieldUpdate* updater = mFieldUpdaters[fid];
            if (updater)
            {
                updater->onUpdate (*this, field);
            }
        }
    }

    MamdaSecurityStatus
    MamdaSecStatusListener::MamdaSecStatusListenerImpl::getSecurityStatus(
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
                return (MamdaSecurityStatus) field.getU32();

            case MAMA_FIELD_TYPE_STRING:
                return mamdaSecurityStatusFromString (field.getString());

            default:
                mama_log (MAMA_LOG_LEVEL_FINE,
                          "Unhandled type %d for wSecurityStatus.  "
                          "Expected string or integer.", 
                          field.getType());
                break;
        }

        return SECURITY_STATUS_UNKNOWN;
    }

    MamdaSecurityStatusQual
    MamdaSecStatusListener::MamdaSecStatusListenerImpl::getSecurityStatusQual(
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
                return (MamdaSecurityStatusQual) field.getU32();

            case MAMA_FIELD_TYPE_STRING:
                return mamdaSecurityStatusQualFromString (field.getString());

            default:
                mama_log (MAMA_LOG_LEVEL_FINE,
                          "Unhandled type %d for wSecurityStatusQual.  "
                          "Expected string or integer.", 
                          field.getType());
                break;
        }

        return SECURITY_STATUS_QUAL_UNKNOWN;
    }

    void MamdaSecStatusListener::MamdaSecStatusListenerImpl::checkSecStatusCount (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg,
        bool                checkForGap)
    {
        // Check number of sec status updates for gaps
        mama_u32_t secStatusCount = mSecStatusCache.mTmpSecStatusCount;
        if (checkForGap && (secStatusCount > 0))
        {
            if ((mSecStatusCache.mSecStatusCount > 0) && 
                (secStatusCount > (mSecStatusCache.mSecStatusCount +1)))
            {
                mSecStatusCache.mSecStatusCount = mSecStatusCache.mTmpSecStatusCount;
                mSecStatusCache.mGapBegin       = mSecStatusCache.mSecStatusCount +1;
                mSecStatusCache.mGapEnd         = secStatusCount -1;

                mSecStatusCache.mGapBeginFieldState       = MODIFIED;
                mSecStatusCache.mGapEndFieldState         = MODIFIED;
                mSecStatusCache.mSecStatusCountFieldState = MODIFIED;

                if (mHandler)
                {
                    mHandler->onSecStatusUpdate (subscription, mListener, msg);
                }
            }
        }
        mSecStatusCache.mSecStatusCount           = mSecStatusCache.mTmpSecStatusCount;
        mSecStatusCache.mSecStatusCountFieldState = MODIFIED;
    }

    struct MamdaSecStatusListener::MamdaSecStatusListenerImpl::FieldUpdateSecStatusIssueSymbol 
        : public SecStatusFieldUpdate
    {
        void onUpdate (MamdaSecStatusListener::MamdaSecStatusListenerImpl&  impl,
                       const MamaMsgField&                                  field)
        {
            if (impl.mSecStatusCache.mIssueSymbol != field.getString())
            {
                impl.mSecStatusCache.mIssueSymbol = field.getString();
                impl.mSecStatusCache.mIssueSymbolFieldState = MODIFIED;
                impl.updated = true;
            }
        }
    };

    struct MamdaSecStatusListener::MamdaSecStatusListenerImpl::FieldUpdateSecStatusSrcTime 
        : public SecStatusFieldUpdate
    {
        void onUpdate (MamdaSecStatusListener::MamdaSecStatusListenerImpl&  impl,
                       const MamaMsgField&                                  field)
        {
            field.getDateTime(impl.mSecStatusCache.mSrcTime);
            impl.mSecStatusCache.mSrcTimeFieldState = MODIFIED;
        }
    };

    struct MamdaSecStatusListener::MamdaSecStatusListenerImpl::FieldUpdateSecStatusActTime 
        : public SecStatusFieldUpdate
    {
        void onUpdate (MamdaSecStatusListener::MamdaSecStatusListenerImpl&  impl,
                       const MamaMsgField&                                  field)
        {
            field.getDateTime(impl.mSecStatusCache.mActTime);
            impl.mSecStatusCache.mActTimeFieldState = MODIFIED;
        }
    };

    struct MamdaSecStatusListener::MamdaSecStatusListenerImpl::FieldUpdateSecStatusLineTime 
        : public SecStatusFieldUpdate
    {
        void onUpdate (MamdaSecStatusListener::MamdaSecStatusListenerImpl&  impl,
                       const MamaMsgField&                                  field)
        {
            field.getDateTime(impl.mSecStatusCache.mLineTime);
            impl.mSecStatusCache.mLineTimeFieldState = MODIFIED;
        }
    };

    struct MamdaSecStatusListener::MamdaSecStatusListenerImpl::FieldUpdateMsgQual 
        : public SecStatusFieldUpdate
    {
        void onUpdate (MamdaSecStatusListener::MamdaSecStatusListenerImpl&  impl,
                       const MamaMsgField&                                  field)
        {
            impl.mSecStatusCache.mMsgQual.setValue(field.getU16());
            impl.mSecStatusCache.mMsgQualFieldState = MODIFIED;
        }
    };

    struct MamdaSecStatusListener::MamdaSecStatusListenerImpl::FieldUpdateSecStatusReason 
        : public SecStatusFieldUpdate
    {
        void onUpdate (MamdaSecStatusListener::MamdaSecStatusListenerImpl&  impl,
                       const MamaMsgField&                                  field)
        {
            if (impl.mSecStatusCache.mReason != field.getString())
		    {
			    impl.mSecStatusCache.mReason = field.getString();
                impl.mSecStatusCache.mReasonFieldState = MODIFIED;
		    }
        }
    };

    struct MamdaSecStatusListener::MamdaSecStatusListenerImpl::FieldUpdateSecurityAction 
        : public SecStatusFieldUpdate
    {
        void onUpdate (MamdaSecStatusListener::MamdaSecStatusListenerImpl&  impl,
                       const MamaMsgField&                                  field)
        {
             // There is a bug in 2.14 FHs which can cause character fields to be sent as strings
             // FH property CharFieldAsStringField in 2.16-> can enable this behaviour
             // Adding support for this in MAMDA for client apps coded to expect this behaviour
             char result = ' ';
             switch (field.getType())
             {
                case MAMA_FIELD_TYPE_CHAR :
                    result = field.getChar();
                    break;
                case MAMA_FIELD_TYPE_STRING :
                    result = field.getString()[0];
                    break;
                default : break;
             }
             
		    if (impl.mSecStatusCache.mSecurityAction != result)
		    {
			    impl.mSecStatusCache.mSecurityAction = result;
                impl.mSecStatusCache.mSecurityActionFieldState = MODIFIED;
			    impl.updated = true;
		    }
        }
    };

    struct MamdaSecStatusListener::MamdaSecStatusListenerImpl::FieldUpdateShortSaleCircuitBreaker
        : public SecStatusFieldUpdate
    {
        void onUpdate (MamdaSecStatusListener::MamdaSecStatusListenerImpl&  impl,
                       const MamaMsgField&                                  field)
        {       
            if (field.getChar() != impl.mSecStatusCache.mShortSaleCircuitBreaker)
            {
                impl.mSecStatusCache.mShortSaleCircuitBreaker = field.getChar();
                impl.mSecStatusCache.mShortSaleCircuitBreakerFieldState = MODIFIED;
                impl.updated = true;
            } 
        }
    };

    struct MamdaSecStatusListener::MamdaSecStatusListenerImpl::FieldUpdateSecurityStatusTime 
        : public SecStatusFieldUpdate
    {
        void onUpdate (MamdaSecStatusListener::MamdaSecStatusListenerImpl&  impl,
                       const MamaMsgField&                                  field)
        {
            field.getDateTime(impl.mSecStatusCache.mSecurityStatusTime);
            impl.mSecStatusCache.mSecurityStatusTimeFieldState = MODIFIED;
        }
    };

    struct MamdaSecStatusListener::MamdaSecStatusListenerImpl::FieldUpdateSecurityType 
        : public SecStatusFieldUpdate
    {
      void onUpdate (MamdaSecStatusListener::MamdaSecStatusListenerImpl&  impl,
                     const MamaMsgField&                                  field)
        {
            if (impl.mSecStatusCache.mSecurityType != field.getString())
            {
                impl.mSecStatusCache.mSecurityType = field.getString();
                impl.mSecStatusCache.mSecurityTypeFieldState = MODIFIED;
                impl.updated = true;
            }
        }
    };



    struct MamdaSecStatusListener::MamdaSecStatusListenerImpl::FieldUpdateSecurityStatus 
        : public SecStatusFieldUpdate
    {
        void onUpdate (MamdaSecStatusListener::MamdaSecStatusListenerImpl&  impl,
                       const MamaMsgField&                                  field)
        {
            MamdaSecurityStatus status = impl.getSecurityStatus(field);
            if (impl.mSecStatusCache.mSecurityStatus != status)
            {
                impl.mSecStatusCache.mSecurityStatus = status;
                impl.mSecStatusCache.mSecurityStatusFieldState = MODIFIED;
                impl.updated = true;
            }
        }
    };

    struct MamdaSecStatusListener::MamdaSecStatusListenerImpl::FieldUpdateSecurityStatusQual 
        : public SecStatusFieldUpdate
    {
        void onUpdate (MamdaSecStatusListener::MamdaSecStatusListenerImpl&  impl,
                       const MamaMsgField&                                  field)
        {
            MamdaSecurityStatusQual statusQual = impl.getSecurityStatusQual(field);
            if (impl.mSecStatusCache.mSecurityStatusQual != statusQual)
            {
                impl.mSecStatusCache.mSecurityStatusQual = statusQual;
                impl.mSecStatusCache.mSecurityStatusQualFieldState = MODIFIED;
                impl.updated = true;
            }
        }
    };

    struct MamdaSecStatusListener::MamdaSecStatusListenerImpl::FieldUpdateSecurityStatusNative 
        : public SecStatusFieldUpdate
    {
        void onUpdate (MamdaSecStatusListener::MamdaSecStatusListenerImpl&  impl,
                       const MamaMsgField&                                  field)
        {
		    if (impl.mSecStatusCache.mSecurityStatusNative != field.getString())
		    {
			    impl.mSecStatusCache.mSecurityStatusNative = field.getString();
                impl.mSecStatusCache.mSecurityStatusNativeFieldState = MODIFIED;
			    impl.updated = true;
		    }
        }
    };

    struct MamdaSecStatusListener::MamdaSecStatusListenerImpl::FieldUpdateSecStatusFreeText 
        : public SecStatusFieldUpdate
    {
        void onUpdate (MamdaSecStatusListener::MamdaSecStatusListenerImpl&  impl,
                       const MamaMsgField&                                  field)
        {
            impl.mSecStatusCache.mFreeText = field.getString();
            impl.mSecStatusCache.mFreeTextFieldState = MODIFIED;
        }
    };

    struct MamdaSecStatusListener::MamdaSecStatusListenerImpl::FieldUpdateSecStatusEventSeqNum
        : public SecStatusFieldUpdate
    {
        void onUpdate (MamdaSecStatusListener::MamdaSecStatusListenerImpl&  impl,
                       const MamaMsgField&                                  field)
        {
            impl.mSecStatusCache.mEventSeqNum = field.getU32();
            impl.mSecStatusCache.mEventSeqNumFieldState = MODIFIED;
        }
    };


    SecStatusFieldUpdate**  MamdaSecStatusListener::MamdaSecStatusListenerImpl::
        mFieldUpdaters = NULL;

    volatile  uint16_t  MamdaSecStatusListener::MamdaSecStatusListenerImpl::
        mFieldUpdatersSize = 0;

    wthread_static_mutex_t  MamdaSecStatusListener::MamdaSecStatusListenerImpl::
        mSecStatusFieldUpdaterLockMutex = WSTATIC_MUTEX_INITIALIZER;

    bool MamdaSecStatusListener::MamdaSecStatusListenerImpl::
        mUpdatersComplete = false;

    void MamdaSecStatusListener::MamdaSecStatusListenerImpl::initFieldUpdaters ()
    {
        if (!mFieldUpdaters)
        {
            mFieldUpdaters = new SecStatusFieldUpdate* [MamdaSecStatusFields::getMaxFid() +1];
            mFieldUpdatersSize = MamdaSecStatusFields::getMaxFid();

            /* Use uint32_t instead of uint16_t to avoid infinite loop if max fid = 65535 */
            for (uint32_t i = 0; i <= mFieldUpdatersSize; ++i)
            {
                mFieldUpdaters[i] = NULL;
            }
        }

      	initFieldUpdater (MamdaCommonFields::ISSUE_SYMBOL,
                          new MamdaSecStatusListener::
		                  MamdaSecStatusListenerImpl::FieldUpdateSecStatusIssueSymbol);

        initFieldUpdater (MamdaCommonFields::SRC_TIME,
                          new MamdaSecStatusListener::
		                  MamdaSecStatusListenerImpl::FieldUpdateSecStatusSrcTime);

        initFieldUpdater (MamdaCommonFields::ACTIVITY_TIME,
                          new MamdaSecStatusListener::
		                  MamdaSecStatusListenerImpl::FieldUpdateSecStatusActTime);

        initFieldUpdater (MamdaCommonFields::LINE_TIME,
                          new MamdaSecStatusListener::
		                  MamdaSecStatusListenerImpl::FieldUpdateSecStatusLineTime);

        initFieldUpdater (MamdaCommonFields::MSG_QUAL,
                          new MamdaSecStatusListener::
		                  MamdaSecStatusListenerImpl::FieldUpdateMsgQual);

        initFieldUpdater (MamdaSecStatusFields::REASON,
                          new MamdaSecStatusListener::
		                  MamdaSecStatusListenerImpl::FieldUpdateSecStatusReason);

        initFieldUpdater (MamdaSecStatusFields::SECURITY_ACTION,
                          new MamdaSecStatusListener::
		                  MamdaSecStatusListenerImpl::FieldUpdateSecurityAction);

        initFieldUpdater (MamdaSecStatusFields::SHORT_SALE_CIRCUIT_BREAKER,
                          new MamdaSecStatusListener::
                          MamdaSecStatusListenerImpl::FieldUpdateShortSaleCircuitBreaker);

        initFieldUpdater (MamdaSecStatusFields::SECURITY_TYPE,
                          new MamdaSecStatusListener::
		                  MamdaSecStatusListenerImpl::FieldUpdateSecurityType);

        initFieldUpdater (MamdaSecStatusFields::SECURITY_STATUS,
                          new MamdaSecStatusListener::
		                  MamdaSecStatusListenerImpl::FieldUpdateSecurityStatus);

     	initFieldUpdater (MamdaSecStatusFields::SECURITY_STATUS_TIME,
                          new MamdaSecStatusListener::
            	          MamdaSecStatusListenerImpl::FieldUpdateSecurityStatusTime);

        initFieldUpdater (MamdaSecStatusFields::SECURITY_STATUS_QUAL,
                          new MamdaSecStatusListener::
		                  MamdaSecStatusListenerImpl::FieldUpdateSecurityStatusQual);

        initFieldUpdater (MamdaSecStatusFields::SECURITY_STATUS_NATIVE,
                          new MamdaSecStatusListener::
		                  MamdaSecStatusListenerImpl::FieldUpdateSecurityStatusNative);

        initFieldUpdater (MamdaSecStatusFields::FREE_TEXT,
                          new MamdaSecStatusListener::
		                  MamdaSecStatusListenerImpl::FieldUpdateSecStatusFreeText);

        initFieldUpdater (MamdaSecStatusFields::EVENT_SEQ_NUM,
                          new MamdaSecStatusListener::
                          MamdaSecStatusListenerImpl::FieldUpdateSecStatusEventSeqNum);
    }

    void MamdaSecStatusListener::MamdaSecStatusListenerImpl::initFieldUpdater (
        const MamaFieldDescriptor*  fieldDesc,
        SecStatusFieldUpdate*       updater)
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
