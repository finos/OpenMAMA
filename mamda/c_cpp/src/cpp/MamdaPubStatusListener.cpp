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
#include <mamda/MamdaPubStatusListener.h>
#include <mamda/MamdaPubStatusFields.h>
#include <mamda/MamdaPubStatusHandler.h>
#include <mamda/MamdaSubscription.h>
#include <mama/mamacpp.h>
#include "MamdaUtils.h"
#include <iostream>
#include <string>

using std::string;

namespace Wombat
{

    struct PubStatusFieldUpdate
    {
        virtual void onUpdate (
            MamdaPubStatusListener::MamdaPubStatusListenerImpl&  impl,
            const MamaMsgField&                                  field) = 0;
    };

    class MamdaPubStatusListener::MamdaPubStatusListenerImpl :
        public MamaMsgFieldIterator
    {
    public:
        MamdaPubStatusListenerImpl  (
            MamdaPubStatusListener&  listener);
        ~MamdaPubStatusListenerImpl () {}

        void handlePubStatusUpdate (
            MamdaSubscription*  subscription,
            const MamaMsg&      msg,
            short               msgType);

        void handleUpdate (
            MamdaSubscription*  subscription,
            const MamaMsg&      msg);

        void onField (
            const MamaMsg&      msg,
            const MamaMsgField& field,
            void*               closure);

        void clearCache ();

        void updatePubStatusFields (
            const MamaMsg&      msg);

        void checkPubStatusCount (
            MamdaSubscription*  subscription,
            const MamaMsg&      msg,
            bool                checkForGap);

        void updateFieldStates ();

        MamdaPubStatusListener&   mListener;
        MamdaPubStatusHandler*    mHandler;

        string                    mSymbol;            MamdaFieldState  mSymbolFieldState;
        string                    mActDateStr;        MamdaFieldState  mActDateStrFieldState;
        string                    mActTimeStr;        MamdaFieldState  mActTimeStrFieldState;
        string                    mPubId;             MamdaFieldState  mPubIdFieldState;
        string                    mPubClass;          MamdaFieldState  mPubClassFieldState;
        string                    mPubHostName;       MamdaFieldState  mPubHostNameFieldState;
        string                    mStatusMsgType;     MamdaFieldState  mStatusMsgTypeFieldState;
        string                    mPubFtMode;         MamdaFieldState  mPubFtModeFieldState;
        string                    mPubMhMode;         MamdaFieldState  mPubMhModeFieldState;
        string                    mLineStatus;        MamdaFieldState  mLineStatusFieldState;
        uint64_t                  mStatusSeqNum;      MamdaFieldState  mStatusSeqNumFieldState;
        uint64_t                  mBeginGapSeqNum;    MamdaFieldState  mBeginGapSeqNumFieldState;
        uint64_t                  mEndGapSeqNum;      MamdaFieldState  mEndGapSeqNumFieldState;
        uint64_t                  mPubCacheSize;      MamdaFieldState  mPubCacheSizeFieldState;
        string                    mFreeText;          MamdaFieldState  mFreeTextFieldState;

        uint32_t                  mTmpPubStatusCount; MamdaFieldState  mTmpPubStatusCountFieldState;
        uint32_t                  mPubStatusCount;    MamdaFieldState  mPubStatusCountFieldState;

        long                      mGapBegin;          MamdaFieldState  mGapBeginFieldState;
        long                      mGapEnd;            MamdaFieldState  mGapEndFieldState;

        static void initFieldUpdaters ();
        static void initFieldUpdater (
            const MamaFieldDescriptor*  fieldDesc,
            PubStatusFieldUpdate*       updater);

        static void updateField (
            MamdaPubStatusListener::MamdaPubStatusListenerImpl&   impl,
            const MamaMsgField&                                   field);

        static PubStatusFieldUpdate**  mFieldUpdaters;
        static volatile uint16_t       mFieldUpdatersSize;
        static wthread_static_mutex_t         mPubStatusFieldUpdaterLockMutex;

        struct FieldUpdateSymbol;
        struct FieldUpdateActivityTime;
        struct FieldUpdateActivityDate;
        struct FieldUpdatePubId;
        struct FieldUpdatePubClass;
        struct FieldUpdatePubHostName;
        struct FieldUpdateStatusMsgType;
        struct FieldUpdatePubFtMode;
        struct FieldUpdatePubMhMode;
        struct FieldUpdateLineStatus;
        struct FieldUpdateStatusSeqNum;
        struct FieldUpdateBeginGapSeqNum;
        struct FieldUpdateEndGapSeqNum;
        struct FieldUpdatePubCacheSize;
        struct FieldUpdateFreeText;
    };

    MamdaPubStatusListener::MamdaPubStatusListener ()
        : mImpl (*new MamdaPubStatusListenerImpl(*this))
    {
    }

    MamdaPubStatusListener::~MamdaPubStatusListener()
    {
	    /* Do not call wthread_mutex_destroy for the FieldUpdaterLockMutex here.  
	       If we do, it will not be initialized again if another listener is created 
	       after the first is destroyed. */
        /* wthread_mutex_destroy (&mImpl.mPubStatusFieldUpdaterLockMutex); */
        delete &mImpl;
    }

    void MamdaPubStatusListener::addHandler (MamdaPubStatusHandler*  handler)
    {
        mImpl.mHandler = handler;
    }

    const char* MamdaPubStatusListener::getSymbol() const
    {
        return mImpl.mSymbol.c_str();
    }

    const char* MamdaPubStatusListener::getActivityTime() const
    {
        return mImpl.mActTimeStr.c_str();
    }

    const char* MamdaPubStatusListener::getActivityDate() const
    {
        return mImpl.mActDateStr.c_str();
    }

    const char* MamdaPubStatusListener::getPubId() const
    {
        return mImpl.mPubId.c_str();
    }

    const char* MamdaPubStatusListener::getPubClass() const
    {
        return mImpl.mPubClass.c_str();
    }
    const char* MamdaPubStatusListener::getPubHostName() const
    {
        return mImpl.mPubHostName.c_str();
    }

    const char* MamdaPubStatusListener::getStatusMsgType() const
    {
        return mImpl.mStatusMsgType.c_str();
    }

    const char* MamdaPubStatusListener::getPubFtMode() const
    {
        return mImpl.mPubFtMode.c_str();
    }

    const char* MamdaPubStatusListener::getPubMhMode() const
    {
        return mImpl.mPubMhMode.c_str();
    }

    const char* MamdaPubStatusListener::getLineStatus() const
    {
        return mImpl.mLineStatus.c_str();
    }

    long MamdaPubStatusListener::getStatusSeqNum() const
    {
        return mImpl.mStatusSeqNum;
    }

    long MamdaPubStatusListener::getBeginGapSeqNum() const
    {
        return mImpl.mBeginGapSeqNum;
    }

    long MamdaPubStatusListener::getEndGapSeqNum() const
    {
        return mImpl.mEndGapSeqNum;
    }

    long MamdaPubStatusListener::getPubCacheSize() const
    {
        return mImpl.mPubCacheSize;
    }

    const char* MamdaPubStatusListener::getFreeText() const
    {
        return mImpl.mFreeText.c_str();
    }

    /*  FieldState Accessors        */
    MamdaFieldState MamdaPubStatusListener::getSymbolFieldState() const
    {
        return mImpl.mSymbolFieldState;
    }

    MamdaFieldState MamdaPubStatusListener::getActivityTimeFieldState() const
    {
        return mImpl.mActTimeStrFieldState;
    }

    MamdaFieldState MamdaPubStatusListener::getActivityDateFieldState() const
    {
        return mImpl.mActDateStrFieldState;
    }

    MamdaFieldState MamdaPubStatusListener::getPubIdFieldState() const
    {
        return mImpl.mPubIdFieldState;
    }

    MamdaFieldState MamdaPubStatusListener::getPubClassFieldState() const
    {
        return mImpl.mPubClassFieldState;
    }
    MamdaFieldState MamdaPubStatusListener::getPubHostNameFieldState() const
    {
        return mImpl.mPubHostNameFieldState;
    }

    MamdaFieldState MamdaPubStatusListener::getStatusMsgTypeFieldState() const
    {
        return mImpl.mStatusMsgTypeFieldState;
    }

    MamdaFieldState MamdaPubStatusListener::getPubFtModeFieldState() const
    {
        return mImpl.mPubFtModeFieldState;
    }

    MamdaFieldState MamdaPubStatusListener::getPubMhModeFieldState() const
    {
        return mImpl.mPubMhModeFieldState;
    }

    MamdaFieldState MamdaPubStatusListener::getLineStatusFieldState() const
    {
        return mImpl.mLineStatusFieldState;
    }

    MamdaFieldState MamdaPubStatusListener::getStatusSeqNumFieldState() const
    {
        return mImpl.mStatusSeqNumFieldState;
    }

    MamdaFieldState MamdaPubStatusListener::getBeginGapSeqNumFieldState() const
    {
        return mImpl.mBeginGapSeqNumFieldState;
    }

    MamdaFieldState MamdaPubStatusListener::getEndGapSeqNumFieldState() const
    {
        return mImpl.mEndGapSeqNumFieldState;
    }

    MamdaFieldState MamdaPubStatusListener::getPubCacheSizeFieldState() const
    {
        return mImpl.mPubCacheSizeFieldState;
    }

    MamdaFieldState MamdaPubStatusListener::getFreeTextFieldState() const
    {
        return mImpl.mFreeTextFieldState;
    }
    /* End FieldState Accessors     */


    void MamdaPubStatusListener::onMsg (
        MamdaSubscription* subscription,
        const MamaMsg&     msg,
        short              msgType)
    {
        // If msg is an update message, invoke the
        // appropriate callback:
        switch (msgType)
        {
            case MAMA_MSG_TYPE_UPDATE:
            case MAMA_MSG_TYPE_RECAP:
                mImpl.handlePubStatusUpdate (subscription, msg, msgType);
                break;
            default:
                // Not an update message (ignored)
                break;
        }
    }

    MamdaPubStatusListener::MamdaPubStatusListenerImpl::MamdaPubStatusListenerImpl (
        MamdaPubStatusListener&  listener)
        : mListener          (listener)
        , mStatusSeqNum      (0)
        , mBeginGapSeqNum    (0)
        , mEndGapSeqNum      (0)
        , mPubCacheSize      (0)
        , mTmpPubStatusCount (0)
        , mPubStatusCount    (0)
        , mGapBegin          (0)
        , mGapEnd            (0)
    {
    }

    void MamdaPubStatusListener::
    MamdaPubStatusListenerImpl::handlePubStatusUpdate (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg,
        short               msgType)
    {
        // Ensure that the field handling is set up (once for all
        // MamdaPubStatusListener instances).
        if (mFieldUpdatersSize == 0)
        {
            wthread_static_mutex_lock (&mPubStatusFieldUpdaterLockMutex);

            if (mFieldUpdatersSize == 0)
            {
                if (!MamdaPubStatusFields::isSet())
                {
                     mama_log (MAMA_LOG_LEVEL_WARN,
                               "MamdaPubStatusListener: MamdaPubStatusFields::setDictionary() "
                               "has not been called");
                     wthread_static_mutex_unlock (&mPubStatusFieldUpdaterLockMutex);
                     return;
                }

                try
                {
                    initFieldUpdaters ();
                }
                catch (MamaStatus &e)
                {
                    mama_log (MAMA_LOG_LEVEL_WARN,
                              "MamdaPubStatusListener: Could not set field updaters: %s",
                              e.toString ());
                    wthread_static_mutex_unlock (&mPubStatusFieldUpdaterLockMutex);
                    return;
                }
            }

            wthread_static_mutex_unlock (&mPubStatusFieldUpdaterLockMutex);
        }

        // Message data does not persists between messages.
        clearCache ();

        // Process fields in message:
        updateFieldStates     ();
        updatePubStatusFields (msg);

        handleUpdate (subscription, msg);
    }

    void MamdaPubStatusListener::MamdaPubStatusListenerImpl::handleUpdate (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg)
    {
        if (mHandler)
        {
            mHandler->onPubStatusUpdate (subscription, mListener, msg);
        }
    }

    void MamdaPubStatusListener::MamdaPubStatusListenerImpl::clearCache ()
    {
        mSymbol         = "";      mSymbolFieldState         = NOT_INITIALISED;
        mActDateStr     = "";      mActDateStrFieldState     = NOT_INITIALISED;
        mActTimeStr     = "";      mActTimeStrFieldState     = NOT_INITIALISED;
        mPubId          = "";      mPubIdFieldState          = NOT_INITIALISED;
        mPubClass       = "";      mPubClassFieldState       = NOT_INITIALISED;
        mPubHostName    = "";      mPubHostNameFieldState    = NOT_INITIALISED;
        mStatusMsgType  = "";      mStatusMsgTypeFieldState  = NOT_INITIALISED;
        mPubFtMode      = "";      mPubFtModeFieldState      = NOT_INITIALISED;
        mPubMhMode      = "";      mPubMhModeFieldState      = NOT_INITIALISED;
        mLineStatus     = "";      mLineStatusFieldState     = NOT_INITIALISED;
        mStatusSeqNum   = 0;       mStatusSeqNumFieldState   = NOT_INITIALISED;
        mBeginGapSeqNum = 0;       mBeginGapSeqNumFieldState = NOT_INITIALISED;
        mEndGapSeqNum   = 0;       mEndGapSeqNumFieldState   = NOT_INITIALISED;
        mPubCacheSize   = 0;       mPubCacheSizeFieldState   = NOT_INITIALISED;
        mFreeText       = "";      mFreeTextFieldState       = NOT_INITIALISED;
    }

    void MamdaPubStatusListener::MamdaPubStatusListenerImpl::updateFieldStates()
    {
        if (mSymbolFieldState         == MODIFIED)   mSymbolFieldState        = NOT_MODIFIED;
        if (mActDateStrFieldState     == MODIFIED)   mActDateStrFieldState    = NOT_MODIFIED;
        if (mActTimeStrFieldState     == MODIFIED)   mActTimeStrFieldState    = NOT_MODIFIED;
        if (mPubIdFieldState          == MODIFIED)   mPubIdFieldState         = NOT_MODIFIED;
        if (mPubClassFieldState       == MODIFIED)   mPubClassFieldState      = NOT_MODIFIED;
        if (mPubHostNameFieldState    == MODIFIED)   mPubHostNameFieldState   = NOT_MODIFIED;
        if (mStatusMsgTypeFieldState  == MODIFIED)   mStatusMsgTypeFieldState = NOT_MODIFIED;
        if (mPubFtModeFieldState      == MODIFIED)   mPubFtModeFieldState     = NOT_MODIFIED;
        if (mPubMhModeFieldState      == MODIFIED)   mPubMhModeFieldState     = NOT_MODIFIED;
        if (mLineStatusFieldState     == MODIFIED)   mLineStatusFieldState    = NOT_MODIFIED;
        if (mStatusSeqNumFieldState   == MODIFIED)   mStatusSeqNumFieldState  = NOT_MODIFIED;
        if (mBeginGapSeqNumFieldState == MODIFIED)   mBeginGapSeqNumFieldState= NOT_MODIFIED;
        if (mEndGapSeqNumFieldState   == MODIFIED)   mEndGapSeqNumFieldState  = NOT_MODIFIED;
        if (mPubCacheSizeFieldState   == MODIFIED)   mPubCacheSizeFieldState  = NOT_MODIFIED;
        if (mFreeTextFieldState       == MODIFIED)   mFreeTextFieldState      = NOT_MODIFIED;
      
    } 
      
      
    void MamdaPubStatusListener::MamdaPubStatusListenerImpl::updatePubStatusFields (
        const MamaMsg&  msg)
    { 
        // Iterate over all of the fields in the message.
        msg.iterateFields (*this, NULL, NULL);
    } 
      
    void MamdaPubStatusListener::MamdaPubStatusListenerImpl::onField (
        const MamaMsg&       msg,
        const MamaMsgField&  field,
        void*                closure)
    {
        uint16_t fid = field.getFid();
        if (fid <= mFieldUpdatersSize)
        {
            PubStatusFieldUpdate* updater = mFieldUpdaters[fid];
            if (updater)
            {
                updater->onUpdate (*this, field);
            }
        }
    }

    void MamdaPubStatusListener::MamdaPubStatusListenerImpl::checkPubStatusCount (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg,
        bool                checkForGap)
    {
        // Check number of sec status updates for gaps
        unsigned long secStatusCount = mTmpPubStatusCount;
        if (checkForGap && (secStatusCount > 0))
        {
            if ((mPubStatusCount > 0) && (secStatusCount > (mPubStatusCount+1)))
            {
                mGapBegin                 = mPubStatusCount +1;
                mGapEnd                   = secStatusCount  -1;
                mPubStatusCount           = mTmpPubStatusCount;

                mGapBeginFieldState       = MODIFIED;
                mGapEndFieldState         = MODIFIED;
                mPubStatusCountFieldState = MODIFIED;

                if (mHandler)
                {
                    mHandler->onPubStatusUpdate (subscription, mListener, msg);
                }
            }
        }
        mPubStatusCount           = mTmpPubStatusCount;
        mPubStatusCountFieldState = MODIFIED;
    }


    struct MamdaPubStatusListener::MamdaPubStatusListenerImpl::FieldUpdateSymbol
        : public PubStatusFieldUpdate
    {
        void onUpdate (MamdaPubStatusListener::MamdaPubStatusListenerImpl&  impl,
                       const MamaMsgField&                                  field)
        {
            impl.mSymbol = field.getString();
            impl.mSymbolFieldState = MODIFIED;
         }
    };

    struct MamdaPubStatusListener::MamdaPubStatusListenerImpl::FieldUpdateActivityTime 
        : public PubStatusFieldUpdate
    {
        void onUpdate (MamdaPubStatusListener::MamdaPubStatusListenerImpl&  impl,
                       const MamaMsgField&                                  field)
        {
            impl.mActTimeStr = field.getString();
            impl.mActTimeStrFieldState = MODIFIED;
        }
    };

    struct MamdaPubStatusListener::MamdaPubStatusListenerImpl::FieldUpdateActivityDate 
        : public PubStatusFieldUpdate
    {
        void onUpdate (MamdaPubStatusListener::MamdaPubStatusListenerImpl&  impl,
                       const MamaMsgField&                                  field)
        {
            impl.mActDateStr = field.getString();
            impl.mActDateStrFieldState = MODIFIED;
        }
    };

    struct MamdaPubStatusListener::MamdaPubStatusListenerImpl::FieldUpdatePubId 
        : public PubStatusFieldUpdate
    {
        void onUpdate (MamdaPubStatusListener::MamdaPubStatusListenerImpl&  impl,
                       const MamaMsgField&                                  field)
        {
            impl.mPubId = field.getString();
            impl.mPubIdFieldState = MODIFIED;
        }
    };

    struct MamdaPubStatusListener::MamdaPubStatusListenerImpl::FieldUpdatePubClass 
        : public PubStatusFieldUpdate
    {
        void onUpdate (MamdaPubStatusListener::MamdaPubStatusListenerImpl&  impl,
                       const MamaMsgField&                                  field)
        {
            impl.mPubClass = field.getString();
            impl.mPubClassFieldState = MODIFIED;
        }
    };

    struct MamdaPubStatusListener::MamdaPubStatusListenerImpl::FieldUpdatePubHostName 
        : public PubStatusFieldUpdate
    {
        void onUpdate (MamdaPubStatusListener::MamdaPubStatusListenerImpl&  impl,
                       const MamaMsgField&                                  field)
        {
            impl.mPubHostName = field.getString();
            impl.mPubHostNameFieldState = MODIFIED;
        }
    };

    struct MamdaPubStatusListener::MamdaPubStatusListenerImpl::FieldUpdateStatusMsgType 
        : public PubStatusFieldUpdate
    {
        void onUpdate (MamdaPubStatusListener::MamdaPubStatusListenerImpl&  impl,
                       const MamaMsgField&                                  field)
        {
            impl.mStatusMsgType = field.getString();
            impl.mStatusMsgTypeFieldState = MODIFIED;
        }
    };

    struct MamdaPubStatusListener::MamdaPubStatusListenerImpl::FieldUpdatePubFtMode 
        : public PubStatusFieldUpdate
    {
        void onUpdate (MamdaPubStatusListener::MamdaPubStatusListenerImpl&  impl,
                       const MamaMsgField&                                  field)
        {
            impl.mPubFtMode = field.getString();
            impl.mPubFtModeFieldState = MODIFIED;
        }
    };

    struct MamdaPubStatusListener::MamdaPubStatusListenerImpl::FieldUpdatePubMhMode 
        : public PubStatusFieldUpdate
    {
        void onUpdate (MamdaPubStatusListener::MamdaPubStatusListenerImpl&  impl,
                       const MamaMsgField&                                  field)
        {
            impl.mPubMhMode = field.getString();
            impl.mPubMhModeFieldState = MODIFIED;
        }
    };

    struct MamdaPubStatusListener::MamdaPubStatusListenerImpl::FieldUpdateLineStatus 
        : public PubStatusFieldUpdate
    {
        void onUpdate (MamdaPubStatusListener::MamdaPubStatusListenerImpl&  impl,
                       const MamaMsgField&                                  field)
        {
            impl.mLineStatus = field.getString();
            impl.mLineStatusFieldState = MODIFIED;
        }
    };

    struct MamdaPubStatusListener::MamdaPubStatusListenerImpl::FieldUpdateStatusSeqNum 
        : public PubStatusFieldUpdate
    {
        void onUpdate (MamdaPubStatusListener::MamdaPubStatusListenerImpl&  impl,
                       const MamaMsgField&                                  field)
        {
            impl.mStatusSeqNum = field.getU64();
            impl.mStatusSeqNumFieldState = MODIFIED;
        }
    };

    struct MamdaPubStatusListener::MamdaPubStatusListenerImpl::FieldUpdateBeginGapSeqNum 
        : public PubStatusFieldUpdate
    {
        void onUpdate (MamdaPubStatusListener::MamdaPubStatusListenerImpl&  impl,
                       const MamaMsgField&                                  field)
        {
            impl.mBeginGapSeqNum = field.getU64();
            impl.mBeginGapSeqNumFieldState = MODIFIED;
        }
    };

    struct MamdaPubStatusListener::MamdaPubStatusListenerImpl::FieldUpdateEndGapSeqNum 
        : public PubStatusFieldUpdate
    {
        void onUpdate (MamdaPubStatusListener::MamdaPubStatusListenerImpl&  impl,
                       const MamaMsgField&                                  field)
        {
            impl.mEndGapSeqNum = field.getU64();
            impl.mEndGapSeqNumFieldState = MODIFIED;
        }
    };

    struct MamdaPubStatusListener::MamdaPubStatusListenerImpl::FieldUpdatePubCacheSize 
        : public PubStatusFieldUpdate
    {
        void onUpdate (MamdaPubStatusListener::MamdaPubStatusListenerImpl&  impl,
                       const MamaMsgField&                                  field)
        {
            impl.mPubCacheSize = field.getU64();
            impl.mPubCacheSizeFieldState = MODIFIED;
        }
    };

    struct MamdaPubStatusListener::MamdaPubStatusListenerImpl::FieldUpdateFreeText 
        : public PubStatusFieldUpdate
    {
        void onUpdate (MamdaPubStatusListener::MamdaPubStatusListenerImpl&  impl,
                       const MamaMsgField&                                  field)
        {
            impl.mFreeText = field.getString();
            impl.mFreeTextFieldState = MODIFIED;
        }
    };


    PubStatusFieldUpdate**
        MamdaPubStatusListener::MamdaPubStatusListenerImpl::mFieldUpdaters = NULL;

    volatile  uint16_t  
        MamdaPubStatusListener::MamdaPubStatusListenerImpl::mFieldUpdatersSize = 0;

    wthread_static_mutex_t MamdaPubStatusListener::MamdaPubStatusListenerImpl::
        mPubStatusFieldUpdaterLockMutex = WSTATIC_MUTEX_INITIALIZER;

    void MamdaPubStatusListener::MamdaPubStatusListenerImpl::initFieldUpdaters ()
    {
        if (!mFieldUpdaters)
        {
            mFieldUpdaters = new PubStatusFieldUpdate*[MamdaPubStatusFields::getMaxFid() +1];
            mFieldUpdatersSize = MamdaPubStatusFields::getMaxFid();

            /* Use uint32_t instead of uint16_t to avoid infinite loop if max FID = 65535 */
            for (uint32_t i = 0; i <= mFieldUpdatersSize; ++i)
            {
                mFieldUpdaters[i] = NULL;
            }
        }

        initFieldUpdater (MamdaPubStatusFields::SYMBOL,
                          new MamdaPubStatusListener::
		                  MamdaPubStatusListenerImpl::FieldUpdateSymbol);

        initFieldUpdater (MamdaPubStatusFields::ACTIVITY_TIME,
                          new MamdaPubStatusListener::
		                  MamdaPubStatusListenerImpl::FieldUpdateActivityTime);

        initFieldUpdater (MamdaPubStatusFields::ACTIVITY_DATE,
                          new MamdaPubStatusListener::
		                  MamdaPubStatusListenerImpl::FieldUpdateActivityDate);

        initFieldUpdater (MamdaPubStatusFields::PUB_ID,
                          new MamdaPubStatusListener::
		                  MamdaPubStatusListenerImpl::FieldUpdatePubId);

        initFieldUpdater (MamdaPubStatusFields::PUB_CLASS,
                          new MamdaPubStatusListener::
		                  MamdaPubStatusListenerImpl::FieldUpdatePubClass);

        initFieldUpdater (MamdaPubStatusFields::PUB_HOST_NAME,
                          new MamdaPubStatusListener::
		                  MamdaPubStatusListenerImpl::FieldUpdatePubHostName);

        initFieldUpdater (MamdaPubStatusFields::STATUS_MSG_TYPE,
                          new MamdaPubStatusListener::
		                  MamdaPubStatusListenerImpl::FieldUpdateStatusMsgType);

        initFieldUpdater (MamdaPubStatusFields::PUB_FT_MODE,
                          new MamdaPubStatusListener::
		                  MamdaPubStatusListenerImpl::FieldUpdatePubFtMode);

        initFieldUpdater (MamdaPubStatusFields::PUB_MH_MODE,
                          new MamdaPubStatusListener::
		                  MamdaPubStatusListenerImpl::FieldUpdatePubMhMode);

        initFieldUpdater (MamdaPubStatusFields::LINE_STATUS,
                          new MamdaPubStatusListener::
		                  MamdaPubStatusListenerImpl::FieldUpdateLineStatus);

        initFieldUpdater (MamdaPubStatusFields::STATUS_SEQ_NUM,
                          new MamdaPubStatusListener::
		                  MamdaPubStatusListenerImpl::FieldUpdateStatusSeqNum);

        initFieldUpdater (MamdaPubStatusFields::MSG_GAP_BEGIN,
                          new MamdaPubStatusListener::
		                  MamdaPubStatusListenerImpl::FieldUpdateBeginGapSeqNum);

        initFieldUpdater (MamdaPubStatusFields::MSG_GAP_END,
                          new MamdaPubStatusListener::
		                  MamdaPubStatusListenerImpl::FieldUpdateEndGapSeqNum);

        initFieldUpdater (MamdaPubStatusFields::PUB_CACHE_SIZE,
                          new MamdaPubStatusListener::
		                  MamdaPubStatusListenerImpl::FieldUpdatePubCacheSize);

        initFieldUpdater (MamdaPubStatusFields::FREE_TEXT,
                          new MamdaPubStatusListener::
		                  MamdaPubStatusListenerImpl::FieldUpdateFreeText);
    }

    void MamdaPubStatusListener::MamdaPubStatusListenerImpl::initFieldUpdater (
        const MamaFieldDescriptor*  fieldDesc,
        PubStatusFieldUpdate*       updater)
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
