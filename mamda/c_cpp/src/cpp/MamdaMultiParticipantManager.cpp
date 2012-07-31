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

#include <mamda/MamdaMultiParticipantManager.h>
#include <mamda/MamdaMultiParticipantHandler.h>
#include <mamda/MamdaDataException.h>
#include <mamda/MamdaCommonFields.h>
#include <mamda/MamdaSubscription.h>
#include <deque>
#include <map>
#include <string>
#include <assert.h>

using std::deque;
using std::string;
using std::map;

namespace Wombat
{

    typedef  deque<MamdaMsgListener*>              ListenerList;
    typedef  map<string,ListenerList>              ListenerListMap;
    typedef  deque<MamdaMultiParticipantHandler*>  MultiPartHandlerList;

    class MamdaMultiParticipantManager::MamdaMultiParticipantManagerImpl
    {
    public:
        MamdaMultiParticipantManagerImpl (
            MamdaMultiParticipantManager&  self,
            const char*                    symbol);

        ~MamdaMultiParticipantManagerImpl () {}

        void onMsg (
            MamdaSubscription*    subscription,
            const MamaMsg&        msg,
            short                 msgType);

        void handleFullImage (
            MamdaSubscription*    subscription,
            const MamaMsg&        msg,
            short                 msgType);

        void notifyConsolidatedCreate (
            MamdaSubscription*    subscription);

        void createPartListAndNotify(
            MamdaSubscription*    subscription,
            const char*           partId);

        void forwardMsg (
            MamdaSubscription*    subscription,
            const MamaMsg&        msg,
            short                 msgType);

        void forwardMsg (
            ListenerList&         listeners,
            MamdaSubscription*    subscription,
            const MamaMsg&        msg,
            short                 msgType);

        /**
         * Returns the participant for the message. Note that the result
         * of getPartId() is only valid as long as msg is valid.
         *
         * @throw MamdaDataException
         */
        const char* getPartId (
            const MamaMsg&        msg);

        MamdaMultiParticipantManager&         mSelf;
        string                                mSymbol;
        MultiPartHandlerList                  mHandlers;
        ListenerList                          mConsListeners;
        ListenerListMap                       mPartListeners;
        bool                                  mGotConsolidated;
    };


    MamdaMultiParticipantManager::MamdaMultiParticipantManager (
        const char*  symbol)
        : mImpl (*new MamdaMultiParticipantManagerImpl (*this, symbol))
    {
    }

    MamdaMultiParticipantManager::~MamdaMultiParticipantManager ()
    {
        delete &mImpl;
    }

    void MamdaMultiParticipantManager::addHandler (
        MamdaMultiParticipantHandler*  handler)
    {
        mImpl.mHandlers.push_back (handler);
    }

    void MamdaMultiParticipantManager::addConsolidatedListener (
        MamdaMsgListener*  listener)
    {
        mImpl.mConsListeners.push_back (listener);
    }

    void MamdaMultiParticipantManager::addParticipantListener (
        MamdaMsgListener*  listener,
        const char*        partId)
    {
        mImpl.mPartListeners[partId].push_back (listener);
    }

    void MamdaMultiParticipantManager::onMsg (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg,
        short               msgType)
    {
        mImpl.onMsg (subscription, msg, msgType);
    }


    MamdaMultiParticipantManager::MamdaMultiParticipantManagerImpl::MamdaMultiParticipantManagerImpl(
        MamdaMultiParticipantManager&  self,
        const char*                    symbol)
        : mSelf             (self)
        , mSymbol           (symbol)
        , mGotConsolidated  (false)
    {
    }


    void MamdaMultiParticipantManager::MamdaMultiParticipantManagerImpl::onMsg (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg,
        short               msgType)
    {
        mama_log (MAMA_LOG_LEVEL_FINEST,
                  "MamdaMultiParticipantManager: got msg type=%d",
                  msgType);

        try
        {
            // First handle the message internally
            switch (msgType)
            {
                case MAMA_MSG_TYPE_INITIAL:
                case MAMA_MSG_TYPE_RECAP:
                    handleFullImage (subscription, msg, msgType);
                    break;
                case MAMA_MSG_TYPE_END_OF_INITIALS:
                    return;
                default:
                    break;
            }

            // Now pass to other listerners (if any)
            forwardMsg (subscription, msg, msgType);
        }
        catch (MamdaDataException& e)
        {
            mama_log (MAMA_LOG_LEVEL_FINE,
                      "MamdaMultiParticipantManager: caught MamdaDataException: %s",
                      e.what());
        }
        catch (MamaStatus& e)
        {
            mama_log (MAMA_LOG_LEVEL_FINE,
                      "MamdaMultiParticipantManager: caught MamaStatus: %s",
                      e.toString());
        }
        catch (...)
        {
            mama_log (MAMA_LOG_LEVEL_FINE,
                      "MamdaMultiParticipantManager: caught unknown exception");
        }
    }

    void MamdaMultiParticipantManager::MamdaMultiParticipantManagerImpl::handleFullImage (
        MamdaSubscription*    subscription,
        const MamaMsg&        msg,
        short                 msgType)
    {
        // This is where we find out which participants are available.
        const char* partId = getPartId (msg);

        mama_log (MAMA_LOG_LEVEL_FINER,
                  "MamdaMultiParticipantManager: full image for %s (%s)",
                  subscription->getSymbol(), 
                  partId ? partId : "NULL");

        if (!partId)
        {
            notifyConsolidatedCreate (subscription);
        }
        else if (mPartListeners.find (partId) == mPartListeners.end())
        {
            mama_log (MAMA_LOG_LEVEL_FINE,
                      "MamdaMultiParticipantManager: found participant record %s",
                      partId);
            
            createPartListAndNotify (subscription, partId);
        }
    }


    void MamdaMultiParticipantManager::MamdaMultiParticipantManagerImpl::createPartListAndNotify(
        MamdaSubscription*  subscription,
        const char*         partId)
    {
        //This is the one chance that apps will receive the 
        //onParticipantCreate callback. 
        mPartListeners.insert(
                ListenerListMap::value_type (string (partId), ListenerList()));

        MultiPartHandlerList::iterator end = mHandlers.end();
        MultiPartHandlerList::iterator i   = mHandlers.begin();

        for (; i != end; ++i)
        {
            MamdaMultiParticipantHandler* handler = *i;
            handler->onParticipantCreate (subscription, 
                                          mSelf, 
                                          partId, 
                                          false);
        }
    }

    void MamdaMultiParticipantManager::MamdaMultiParticipantManagerImpl::notifyConsolidatedCreate(
        MamdaSubscription*  subscription)
    {
        if (!mGotConsolidated)
        {
            mGotConsolidated = true;

            mama_log (MAMA_LOG_LEVEL_FINE,
                      "MamdaMultiParticipantManager: found consolidated record");

            MultiPartHandlerList::iterator end = mHandlers.end();
            MultiPartHandlerList::iterator i   = mHandlers.begin();

            for (; i != end; ++i)
            {
                MamdaMultiParticipantHandler* handler = *i;
                handler->onConsolidatedCreate (subscription, mSelf);
            }
        }
    }


    void MamdaMultiParticipantManager::MamdaMultiParticipantManagerImpl::forwardMsg (
        MamdaSubscription*    subscription,
        const MamaMsg&        msg,
        short                 msgType)
    {
        const char* partId = getPartId (msg);

        if (partId)
        {
            ListenerListMap::iterator found = mPartListeners.find (partId);
            if (found != mPartListeners.end())
            {
                forwardMsg (found->second, 
                            subscription, 
                            msg, 
                            msgType);
            }
            else
            {
                // If there is no entry in the Map for this participant
                // then this is the first update for that participant!
                createPartListAndNotify (subscription, partId);
                found = mPartListeners.find (partId);

                //A listener may not have been registered!
                if (found != mPartListeners.end())
                {
                    forwardMsg (found->second, 
                                subscription, 
                                msg, 
                                msgType);
                }
            }
        }
        else
        {
            if(!mConsListeners.empty())
            {
                forwardMsg (mConsListeners, 
                            subscription, 
                            msg, 
                            msgType);
            }
            else
            {
                notifyConsolidatedCreate (subscription);

                //A listener may not have been registered!
                if(!mConsListeners.empty())
                {
                    forwardMsg (mConsListeners, 
                                subscription, 
                                msg, 
                                msgType);
                }
            }
        }
    }

    void MamdaMultiParticipantManager::MamdaMultiParticipantManagerImpl::forwardMsg (
        ListenerList&         listeners,
        MamdaSubscription*    subscription,
        const MamaMsg&        msg,
        short                 msgType)
    {
        ListenerList::iterator end = listeners.end();
        ListenerList::iterator i   = listeners.begin();
        for (; i != end; ++i)
        {
            MamdaMsgListener* listener = *i;
            listener->onMsg (subscription, 
                             msg, 
                             msgType);
        }
    }

    const char* MamdaMultiParticipantManager::MamdaMultiParticipantManagerImpl::getPartId (
        const MamaMsg&  msg)
    {
        const char* symbol = NULL;
        const char* partId = NULL;
         
        if (msg.tryString (MamdaCommonFields::PART_ID, partId))
        {
            if (strlen (partId) != 0)
                return partId;
        }      
        
        if (!msg.tryString (MamdaCommonFields::ISSUE_SYMBOL, symbol))
        {        
            if (!msg.tryString (MamdaCommonFields::INDEX_SYMBOL, symbol))
            {
                if (!msg.tryString (MamdaCommonFields::SYMBOL, symbol))
                {
                     mama_log (MAMA_LOG_LEVEL_FINEST, 
                               "getPartId: cannot extrapolate partid from update");
                     return NULL;
                }
            }
        }

        const char* lastDot = strrchr (symbol, '.');
        
        if (lastDot)
        {
            if (lastDot+1 != '\0')
                partId = lastDot +1;
        }
        
        if (!partId)
        {
            mama_log (MAMA_LOG_LEVEL_FINEST,
                      "getPartId: cannot extrapolate partid from update");
        }
                        
        return partId;
    }

} // namespace
