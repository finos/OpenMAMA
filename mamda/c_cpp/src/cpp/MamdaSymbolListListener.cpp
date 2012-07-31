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

#include <mamda/MamdaSymbolListListener.h>
#include <mamda/MamdaSymbolListFields.h>
#include <mamda/MamdaSymbolListHandler.h>
#include <mamda/MamdaCommonFields.h>
#include <mamda/MamdaSubscription.h>
#include <mamda/MamdaFieldState.h>
#include <mama/mamacpp.h>
#include "MamdaUtils.h"
#include <string>

#include <iostream>

using std::string;

namespace Wombat
{

    struct SymbolListFieldUpdate
    {
        virtual void onUpdate (
            MamdaSymbolListListener::MamdaSymbolListListenerImpl&  impl,
            const MamaMsgField&                                    field) = 0;
    };


    class MamdaSymbolListListener::MamdaSymbolListListenerImpl 
        : public MamaMsgFieldIterator
    {
    public:
        MamdaSymbolListListenerImpl  (MamdaSymbolListListener& listener);
        ~MamdaSymbolListListenerImpl () {}

        void handleSymbolListMessage      (MamdaSubscription*  subscription,
                                           const MamaMsg&      msg,
                                           short               msgType);

        void handleRecap                  (MamdaSubscription*  subscription,
                                           const MamaMsg&      msg);

        void handleUpdate                 (MamdaSubscription*  subscription,
                                          const MamaMsg&       msg);

        void onField                      (const MamaMsg&      msg,
                                           const MamaMsgField& field,
                                           void*               closure);

        void updateSymbolListFields       (const MamaMsg&      msg);
        void updateFieldStates            ();
        void updateSymbolList             (const MamaMsg&      msg);

        string gettoken (string& inString, 
                         string delimiter);

        MamdaSymbolListListener& mListener;

        vector<MamdaSymbolListHandler*>  mSymbolListHandlers;

        // The following fields are used for caching the offical quote and
        // related fields.  These fields can be used by applications for
        // reference and will be passed for recaps.
        string          mSymbol;                MamdaFieldState   mSymbolFieldState;
        string          mPartId;                MamdaFieldState   mPartIdFieldState;   
        string          mPubId;                 MamdaFieldState   mPubIdFieldState;    
        MamaDateTime    mSrcTime;               MamdaFieldState   mSrcTimeFieldState;  
        MamaDateTime    mActTime;               MamdaFieldState   mActTimeFieldState;  
        MamaDateTime    mLineTime;              MamdaFieldState   mLineTimeFieldState; 
        MamaDateTime    mSendTime;              MamdaFieldState   mSendTimeFieldState; 
        MamaDateTime    mEventTime;             MamdaFieldState   mEventTimeFieldState;
        mama_seqnum_t   mEventSeqNum;           MamdaFieldState   mEventSeqNumFieldState;   
        vector<string>  mSourcedSymbols;        MamdaFieldState   mSourcedSymbolsFieldState;
        MamaMsgQual     mMsgQual;               MamdaFieldState   mMsgQualFieldState;       
        long            mInitialCount;          MamdaFieldState   mInitialCountFieldState;  
        long            mInitialTotal;          MamdaFieldState   mInitialTotalFieldState;  
     
        static void initFieldUpdaters ();
        static void initFieldUpdater (
            const MamaFieldDescriptor*   fieldDesc,
            SymbolListFieldUpdate*       updater);

        static void updateField (
            MamdaSymbolListListener::MamdaSymbolListListenerImpl&  impl,
            const MamaMsgField&                                    field);

        static SymbolListFieldUpdate**  mFieldUpdaters;
        static volatile uint16_t        mFieldUpdatersSize;
        static wthread_static_mutex_t   mSymbolListFieldUpdaterLockMutex;
        static bool                     mUpdatersComplete;

        struct FieldUpdateSymbolListPubId;
        struct FieldUpdateSymbolListSrcTime;
        struct FieldUpdateSymbolListActTime;
        struct FieldUpdateSymbolListLineTime;
        struct FieldUpdateSymbolListTime;
        struct FieldUpdateSymbolListSeqNum;
        struct FieldUpdateMsgQual;
        struct FieldUpdateMsgTotal;
    };

    MamdaSymbolListListener::MamdaSymbolListListener ()
        : mImpl (*new MamdaSymbolListListenerImpl(*this))
    {
    }

    MamdaSymbolListListener::~MamdaSymbolListListener ()
    {
	    /* Do not call wthread_mutex_destroy for the FieldUpdaterLockMutex here.  
	       If we do, it will not be initialized again if another listener is created 
	       after the first is destroyed. */
        /* wthread_mutex_destroy (&mImpl.mSymbolListFieldUpdaterLockMutex); */
        delete &mImpl;
    }

    void MamdaSymbolListListener::addHandler (MamdaSymbolListHandler* handler)
    {
        mImpl.mSymbolListHandlers.push_back (handler);
    }

    const char* MamdaSymbolListListener::getSymbol() const
    {
        return mImpl.mSymbol.c_str();
    }

    const char* MamdaSymbolListListener::getPartId() const
    {
        return mImpl.mPartId.c_str();
    }

    const MamaDateTime& MamdaSymbolListListener::getSrcTime() const
    {
        return mImpl.mSrcTime;
    }

    const MamaDateTime& MamdaSymbolListListener::getActivityTime() const
    {
        return mImpl.mActTime;
    }

    const MamaDateTime& MamdaSymbolListListener::getLineTime() const
    {
        return mImpl.mLineTime;
    }

    const MamaDateTime& MamdaSymbolListListener::getSendTime() const
    {
        return mImpl.mSendTime;
    }

    const char* MamdaSymbolListListener::getPubId() const
    {
        return mImpl.mPubId.c_str();
    }

    const MamaDateTime& MamdaSymbolListListener::getEventTime() const
    {
        return mImpl.mEventTime;
    } 

    mama_seqnum_t MamdaSymbolListListener::getEventSeqNum() const
    {
        return mImpl.mEventSeqNum;
    }

    vector<string> MamdaSymbolListListener::getSourcedSymbols() const
    {
        return mImpl.mSourcedSymbols;
    }

    const MamaMsgQual& MamdaSymbolListListener::getMsgQual() const
    {
        return mImpl.mMsgQual;
    }

    /*  FieldState Accessors    */

    MamdaFieldState MamdaSymbolListListener::getSymbolFieldState() const
    {
        return mImpl.mSymbolFieldState;
    }

    MamdaFieldState MamdaSymbolListListener::getPartIdFieldState() const
    {
        return mImpl.mPartIdFieldState;
    }

    MamdaFieldState MamdaSymbolListListener::getSrcTimeFieldState() const
    {
        return mImpl.mSrcTimeFieldState;
    }

    MamdaFieldState MamdaSymbolListListener::getActivityTimeFieldState() const
    {
        return mImpl.mActTimeFieldState;
    }

    MamdaFieldState MamdaSymbolListListener::getLineTimeFieldState() const
    {
        return mImpl.mLineTimeFieldState;
    }

    MamdaFieldState MamdaSymbolListListener::getSendTimeFieldState() const
    {
        return mImpl.mSendTimeFieldState;
    }

    MamdaFieldState MamdaSymbolListListener::getPubIdFieldState() const
    {
        return mImpl.mPubIdFieldState;
    }

    MamdaFieldState MamdaSymbolListListener::getEventTimeFieldState() const
    {
        return mImpl.mEventTimeFieldState;
    } 

    MamdaFieldState MamdaSymbolListListener::getEventSeqNumFieldState() const
    {
        return mImpl.mEventSeqNumFieldState;
    }

    MamdaFieldState MamdaSymbolListListener::getSourcedSymbolsFieldState() const
    {
        return mImpl.mSourcedSymbolsFieldState;
    }

    MamdaFieldState MamdaSymbolListListener::getMsgQualFieldState() const
    {
        return mImpl.mMsgQualFieldState;
    }

    /*  End FieldState Accessors    */


    void MamdaSymbolListListener::onMsg (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg,
        short               msgType)
    {
        // If msg is a quote-related message, invoke the
        // appropriate callback:
        if (subscription->checkDebugLevel (MAMA_LOG_LEVEL_FINE))
        {
            const char *contractSymbol = "N/A";
            msg.tryString (MamdaCommonFields::ISSUE_SYMBOL, contractSymbol);

            mama_forceLog (MAMA_LOG_LEVEL_FINE,
                           "MamdaSymbolListListener (%s.%s(%s)) onMsg(). "
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
                mImpl.handleSymbolListMessage (subscription, 
                                               msg, 
                                               msgType);
                break;
            case MAMA_MSG_TYPE_END_OF_INITIALS:
               mama_log (MAMA_LOG_LEVEL_FINE,
                         "End of initial symbol list messages: %i messages received", 
                         mImpl.mInitialCount); 
               if (mImpl.mInitialCount < mImpl.mInitialTotal)
               {
                  mama_log (MAMA_LOG_LEVEL_NORMAL, 
                            "Error: Did not get full symbol list");
               }
               break;
        }
    }

    MamdaSymbolListListener::MamdaSymbolListListenerImpl::MamdaSymbolListListenerImpl(
        MamdaSymbolListListener&  listener)
        : mListener      (listener)
        , mSymbol        ("")
        , mPartId        ("")
        , mPubId         ("")
        , mEventSeqNum   (0)
        , mInitialCount  (0)
        , mInitialTotal  (0)
        , mSymbolFieldState         (NOT_INITIALISED)
        , mPartIdFieldState         (NOT_INITIALISED)
        , mPubIdFieldState          (NOT_INITIALISED)
        , mSrcTimeFieldState        (NOT_INITIALISED)
        , mActTimeFieldState        (NOT_INITIALISED)
        , mLineTimeFieldState       (NOT_INITIALISED)
        , mSendTimeFieldState       (NOT_INITIALISED)
        , mEventTimeFieldState      (NOT_INITIALISED)
        , mEventSeqNumFieldState    (NOT_INITIALISED)
        , mSourcedSymbolsFieldState (NOT_INITIALISED)
        , mMsgQualFieldState        (NOT_INITIALISED)
        , mInitialCountFieldState   (NOT_INITIALISED)
        , mInitialTotalFieldState   (NOT_INITIALISED)
    {
    }

    void MamdaSymbolListListener::MamdaSymbolListListenerImpl::handleSymbolListMessage (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg,
        short               msgType)
    {
        // Ensure that the field handling is set up (once for all
        // MamdaSymbolListListener instances).
        if (!mUpdatersComplete)
        {
            wthread_static_mutex_lock (&mSymbolListFieldUpdaterLockMutex);

            if (!mUpdatersComplete)
            {
                if (!MamdaSymbolListFields::isSet())
                {
                     mama_log (MAMA_LOG_LEVEL_WARN,
                               "MamdaSymbolListListener: MamdaSymbolListFields::setDictionary() "
                               "has not been called.");

                     wthread_static_mutex_unlock (&mSymbolListFieldUpdaterLockMutex);
                     return;
                }

                try
                {
                    initFieldUpdaters ();
                }
                catch (MamaStatus &e)
                {
                    mama_log (MAMA_LOG_LEVEL_WARN,
                              "MamdaSymbolListListener: Could not set field updaters: %s",
                              e.toString ());

                    wthread_static_mutex_unlock (&mSymbolListFieldUpdaterLockMutex);
                    return;
                }
                mUpdatersComplete = true;
            }

            wthread_static_mutex_unlock (&mSymbolListFieldUpdaterLockMutex);
        }

        // Handle fields in message:
        updateFieldStates      ();
        updateSymbolListFields (msg);

        // Separate call to get the list of symbols
        updateSymbolList (msg);

        // Handle according to message type:
        switch (msgType)
        {
            case MAMA_MSG_TYPE_INITIAL:
                mInitialCount++;
            case MAMA_MSG_TYPE_RECAP:
                handleRecap (subscription, msg);
                break;
            case MAMA_MSG_TYPE_UPDATE:
                handleUpdate (subscription, msg);
                break;
        }
    }

    void MamdaSymbolListListener::MamdaSymbolListListenerImpl::handleRecap (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg)
    {
        if(subscription->checkDebugLevel (MAMA_LOG_LEVEL_FINE))
        {
            const char *issueSymbol = "N/A";
            msg.tryString (MamdaCommonFields::ISSUE_SYMBOL, issueSymbol);

            mama_forceLog (MAMA_LOG_LEVEL_FINE,
                           "MamdaSymbolListListener (%s.%s(%s)) handleRecap().\n",
                           issueSymbol,
                           subscription->getSource (),
                           subscription->getSymbol ());
        }

        unsigned int i = 0;
        for (; i< mSymbolListHandlers.size(); i++)
        {
            mSymbolListHandlers[i]->onSymbolListRecap (subscription, 
                                                       mListener, 
                                                       msg,
                                                       mListener);
        }
    }


    void MamdaSymbolListListener::MamdaSymbolListListenerImpl::handleUpdate (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg)
    {
        if(subscription->checkDebugLevel (MAMA_LOG_LEVEL_FINE))
        {
            const char *issueSymbol = "N/A";
            msg.tryString (MamdaCommonFields::ISSUE_SYMBOL, issueSymbol);

            mama_forceLog (MAMA_LOG_LEVEL_FINE,
                           "MamdaSymbolListListener (%s.%s(%s)) handleUpdate().\n",
                           issueSymbol,
                           subscription->getSource (),
                           subscription->getSymbol ());
        }

        unsigned int i = 0;
        for (; i< mSymbolListHandlers.size(); i++)
        {
            mSymbolListHandlers[i]->onSymbolListUpdate (subscription, 
                                                        mListener, 
                                                        msg, 
                                                        mListener);
        }
    }


    string MamdaSymbolListListener::MamdaSymbolListListenerImpl::gettoken (
        string&  inString, 
        string   delimiter)
    {
        string outString;

        if (!inString.empty())
        {
            string::size_type pos = inString.find (delimiter, 0);
            if (pos == string::npos)
            {
                outString = inString;
                inString  = "";
            }
            else
            {
                outString = inString.substr (0, (pos));
                inString  = inString.substr (pos + 1, inString.size() - pos);
            }
        }
        return outString;
    }


    void MamdaSymbolListListener::MamdaSymbolListListenerImpl::
        updateFieldStates()
    {
        if (mSymbolFieldState == MODIFIED)   
            mSymbolFieldState = NOT_MODIFIED;

        if (mPartIdFieldState == MODIFIED)   
            mPartIdFieldState = NOT_MODIFIED;

        if (mPubIdFieldState == MODIFIED)    
            mPubIdFieldState = NOT_MODIFIED; 

        if (mSrcTimeFieldState == MODIFIED)  
            mSrcTimeFieldState = NOT_MODIFIED;  
     
        if (mActTimeFieldState == MODIFIED)    
            mActTimeFieldState = NOT_MODIFIED;  
     
        if (mLineTimeFieldState == MODIFIED)   
            mLineTimeFieldState = NOT_MODIFIED; 
     
        if (mSendTimeFieldState == MODIFIED)   
            mSendTimeFieldState = NOT_MODIFIED;  

        if (mEventTimeFieldState == MODIFIED)  
            mEventTimeFieldState = NOT_MODIFIED; 

        if (mEventSeqNumFieldState == MODIFIED)
            mEventSeqNumFieldState = NOT_MODIFIED;  
      
        if (mSourcedSymbolsFieldState == MODIFIED)
            mSourcedSymbolsFieldState = NOT_MODIFIED; 

        if (mMsgQualFieldState == MODIFIED)     
            mMsgQualFieldState = NOT_MODIFIED;  

        if (mInitialCountFieldState == MODIFIED)
            mInitialCountFieldState = NOT_MODIFIED; 
      
        if (mInitialTotalFieldState == MODIFIED)     
            mInitialTotalFieldState = NOT_MODIFIED;   
    }   

    void MamdaSymbolListListener::MamdaSymbolListListenerImpl::updateSymbolListFields (
        const MamaMsg&  msg)
    {
        const char* symbol = NULL;
        const char* partId = NULL;

        getSymbolAndPartId (msg, symbol, partId);

        if (symbol) mSymbol = symbol;
        if (partId) mPartId = partId;

        // Iterate over all of the fields in the message.
        msg.iterateFields (*this, NULL, NULL);
    }

    void MamdaSymbolListListener::MamdaSymbolListListenerImpl::updateSymbolList (
        const MamaMsg&  msg)
    {
        const char* tmpSymbols = NULL;

        if (msg.tryString (MamaFieldSymbolList.mName, 
                           MamaFieldSymbolList.mFid, 
                           tmpSymbols))
        {
            mSourcedSymbols.clear();
            if (strlen(tmpSymbols) > 0)
            {
                string sourcedSymbols = string(tmpSymbols);
                while (!sourcedSymbols.empty())
                {
                    mSourcedSymbols.push_back (gettoken (sourcedSymbols,","));
                }  
            }
        }
    }


    void MamdaSymbolListListener::MamdaSymbolListListenerImpl::onField (
        const MamaMsg&       msg,
        const MamaMsgField&  field,
        void*                closure)
    {
        uint16_t fid = field.getFid();
        if (fid <= mFieldUpdatersSize)
        {
            SymbolListFieldUpdate* updater = mFieldUpdaters[fid];
            if (updater)
            {
                updater->onUpdate (*this, field);
            }
        }
    }

    struct MamdaSymbolListListener::MamdaSymbolListListenerImpl::FieldUpdateSymbolListSrcTime 
        : public SymbolListFieldUpdate
    {
        void onUpdate (MamdaSymbolListListener::MamdaSymbolListListenerImpl&  impl,
                       const MamaMsgField&                                    field)
        {
            field.getDateTime (impl.mSrcTime);
            impl.mSrcTimeFieldState = MODIFIED;
        }
    };

    struct MamdaSymbolListListener::MamdaSymbolListListenerImpl::FieldUpdateSymbolListActTime 
        : public SymbolListFieldUpdate
    {
        void onUpdate (MamdaSymbolListListener::MamdaSymbolListListenerImpl&  impl,
                       const MamaMsgField&                                    field)
        {
            field.getDateTime (impl.mActTime);
            impl.mActTimeFieldState = MODIFIED;
        }
    };

    struct MamdaSymbolListListener::MamdaSymbolListListenerImpl::FieldUpdateSymbolListLineTime
        : public SymbolListFieldUpdate
    {
        void onUpdate (MamdaSymbolListListener::MamdaSymbolListListenerImpl&  impl,
                       const MamaMsgField&                                    field)
        {
            field.getDateTime (impl.mLineTime);
            impl.mLineTimeFieldState = MODIFIED;
        }
    };

    struct MamdaSymbolListListener::MamdaSymbolListListenerImpl::FieldUpdateSymbolListPubId 
        : public SymbolListFieldUpdate
    {
        void onUpdate (MamdaSymbolListListener::MamdaSymbolListListenerImpl&  impl,
                       const MamaMsgField&                                    field)
        {
            impl.mPubId = field.getString();
            impl.mPubIdFieldState = MODIFIED;
        }
    };

    struct MamdaSymbolListListener::MamdaSymbolListListenerImpl::FieldUpdateSymbolListTime 
        : public SymbolListFieldUpdate
    {
        void onUpdate (MamdaSymbolListListener::MamdaSymbolListListenerImpl&  impl,
                       const MamaMsgField&                                    field)
        {
            field.getDateTime (impl.mEventTime);
            impl.mEventTimeFieldState = MODIFIED;
        }
    };

    struct MamdaSymbolListListener::MamdaSymbolListListenerImpl::FieldUpdateSymbolListSeqNum 
        : public SymbolListFieldUpdate
    {
        void onUpdate (MamdaSymbolListListener::MamdaSymbolListListenerImpl&  impl,
                       const MamaMsgField&                                    field)
        {
            impl.mEventSeqNum = field.getI64();
            impl.mEventSeqNumFieldState = MODIFIED;
        }
    };

    struct MamdaSymbolListListener::MamdaSymbolListListenerImpl::FieldUpdateMsgQual
        : public SymbolListFieldUpdate
    {
        void onUpdate (MamdaSymbolListListener::MamdaSymbolListListenerImpl&  impl,
                       const MamaMsgField&                                    field)
        {
            impl.mMsgQual.setValue (field.getU16());
            impl.mMsgQualFieldState = MODIFIED;
        }
    };

    struct MamdaSymbolListListener::MamdaSymbolListListenerImpl::FieldUpdateMsgTotal
        : public SymbolListFieldUpdate
    {
        void onUpdate (MamdaSymbolListListener::MamdaSymbolListListenerImpl&  impl,
                       const MamaMsgField&                                    field)
        {
            impl.mInitialTotal = field.getU32();
            impl.mInitialTotalFieldState = MODIFIED;
        }
    };

    SymbolListFieldUpdate** MamdaSymbolListListener::MamdaSymbolListListenerImpl::
    mFieldUpdaters = NULL;

    volatile uint16_t  MamdaSymbolListListener::MamdaSymbolListListenerImpl::
    mFieldUpdatersSize = 0;

    wthread_static_mutex_t MamdaSymbolListListener::MamdaSymbolListListenerImpl::
    mSymbolListFieldUpdaterLockMutex = WSTATIC_MUTEX_INITIALIZER;

    bool MamdaSymbolListListener::MamdaSymbolListListenerImpl::
    mUpdatersComplete = false;

    void MamdaSymbolListListener::MamdaSymbolListListenerImpl::initFieldUpdaters ()
    {
        if (!mFieldUpdaters)
        {
            mFieldUpdaters = 
            new SymbolListFieldUpdate* [MamdaSymbolListFields::getMaxFid() +1];

            mFieldUpdatersSize = MamdaSymbolListFields::getMaxFid();

            /* Use uint32_t instead of uint16_t to avoid infinite loop if max FID = 65535 */
            for (uint32_t i = 0; i <= mFieldUpdatersSize; ++i)
            {
                mFieldUpdaters[i] = NULL;
            }
        }

        initFieldUpdater (MamdaSymbolListFields::SRC_TIME,
                          new MamdaSymbolListListener::
                          MamdaSymbolListListenerImpl::FieldUpdateSymbolListSrcTime);
        initFieldUpdater (MamdaCommonFields::LINE_TIME,
                          new MamdaSymbolListListener::
                          MamdaSymbolListListenerImpl::FieldUpdateSymbolListLineTime);
        initFieldUpdater (MamdaSymbolListFields::EVENT_TIME,
                          new MamdaSymbolListListener::
                          MamdaSymbolListListenerImpl::FieldUpdateSymbolListTime);
        initFieldUpdater (MamdaSymbolListFields::EVENT_SEQ_NUM,
                          new MamdaSymbolListListener::
                          MamdaSymbolListListenerImpl::FieldUpdateSymbolListSeqNum);
        initFieldUpdater (MamdaSymbolListFields::ACTIVITY_TIME,
                          new MamdaSymbolListListener::
                          MamdaSymbolListListenerImpl::FieldUpdateSymbolListActTime);
        initFieldUpdater (MamdaSymbolListFields::PUB_ID,
                          new MamdaSymbolListListener::
                          MamdaSymbolListListenerImpl::FieldUpdateSymbolListPubId);
        initFieldUpdater (MamdaCommonFields::MSG_QUAL,
                          new MamdaSymbolListListener::
                          MamdaSymbolListListenerImpl::FieldUpdateMsgQual);
        initFieldUpdater (MamdaCommonFields::MSG_TOTAL,
                          new MamdaSymbolListListener::
                          MamdaSymbolListListenerImpl::FieldUpdateMsgTotal);
    }

    void MamdaSymbolListListener::MamdaSymbolListListenerImpl::initFieldUpdater (
        const MamaFieldDescriptor*  fieldDesc,
        SymbolListFieldUpdate*     updater)
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
