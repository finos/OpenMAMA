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

#include <mamda/MamdaMultiSecurityManager.h>
#include <mamda/MamdaMultiSecurityHandler.h>
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
    typedef  deque<MamdaMultiSecurityHandler*>     MultiSecurityHandlerList;

    class MamdaMultiSecurityManager::MamdaMultiSecurityManagerImpl
    {
    public:
        MamdaMultiSecurityManagerImpl (
            MamdaMultiSecurityManager&  self,
            const char*                 symbol);

        ~MamdaMultiSecurityManagerImpl () {}

        void onMsg (
            MamdaSubscription*    subscription,
            const MamaMsg&        msg,
            short                 msgType);

        void handleFullImage (
            MamdaSubscription*    subscription,
            const MamaMsg&        msg,
            short                 msgType);

        void createListenerListAndNotify(
            MamdaSubscription*    subscription,
            const char*           symbol);

        void forwardMsg (
            MamdaSubscription*    subscription,
            const MamaMsg&        msg,
            short                 msgType);

        void forwardMsg (
            ListenerList&         listeners,
            MamdaSubscription*    subscription,
            const MamaMsg&        msg,
            short                 msgType);


        MamdaMultiSecurityManager&            mSelf;
        string                                mSymbol;
        MultiSecurityHandlerList              mHandlers;
        ListenerListMap                       mSecurityListeners;
    };


    MamdaMultiSecurityManager::MamdaMultiSecurityManager (const char*  symbol)
        : mImpl (*new MamdaMultiSecurityManagerImpl (*this, symbol))
    {
    }

    MamdaMultiSecurityManager::~MamdaMultiSecurityManager ()
    {
        delete &mImpl;
    }

    void MamdaMultiSecurityManager::addHandler (MamdaMultiSecurityHandler* handler)
    {
        mImpl.mHandlers.push_back (handler);
    }

    void MamdaMultiSecurityManager::addSecurityListener (
        MamdaMsgListener*  listener,
        const char*        securitySymbol)
    {
        mImpl.mSecurityListeners[securitySymbol].push_back (listener);
    }

    void MamdaMultiSecurityManager::onMsg (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg,
        short               msgType)
    {
        mImpl.onMsg (subscription, msg, msgType);
    }


    MamdaMultiSecurityManager::MamdaMultiSecurityManagerImpl::MamdaMultiSecurityManagerImpl(
        MamdaMultiSecurityManager&  self,
        const char*                 symbol)
        : mSelf   (self)
        , mSymbol (symbol)
    {
    }


    void MamdaMultiSecurityManager::MamdaMultiSecurityManagerImpl::onMsg (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg,
        short               msgType)
    {
        mama_log (MAMA_LOG_LEVEL_FINEST,
                  "MamdaMultiSecurityManager: got msg type=%d",
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
            // Now pass to other listeners (if any)
            forwardMsg (subscription, msg, msgType);
        }
        catch (MamdaDataException& e)
        {
            mama_log (MAMA_LOG_LEVEL_FINE,
                      "MamdaMultiSecurityManager: caught MamdaDataException: %s",
                      e.what());
        }
    }

    void MamdaMultiSecurityManager::MamdaMultiSecurityManagerImpl::handleFullImage (
        MamdaSubscription*    subscription,
        const MamaMsg&        msg,
        short                 msgType)
    {
        const char* symbol = msg.getString (MamdaCommonFields::ISSUE_SYMBOL);

        mama_log (MAMA_LOG_LEVEL_FINER,
                  "MamdaMultiSecurityManager: full image for (%s)",
                  symbol);

        // If we have not already encountered this symbol add a list to the map
        // for it. This list contains all listeners added for this symbol.
        if (mSecurityListeners.find(symbol) == mSecurityListeners.end())
        {
            mama_log (MAMA_LOG_LEVEL_FINE,
                      "MamdaMultiSecurityManager: found security record %s",
                      symbol);

            createListenerListAndNotify (subscription, symbol);
        }
    }

    void MamdaMultiSecurityManager::MamdaMultiSecurityManagerImpl::createListenerListAndNotify(
        MamdaSubscription*  subscription,
        const char*         symbol)
    {
        mSecurityListeners.insert(
            ListenerListMap::value_type (string (symbol), ListenerList()));

        //We notify any registered handlers of the new security.
        MultiSecurityHandlerList::iterator end = mHandlers.end();
        MultiSecurityHandlerList::iterator i   = mHandlers.begin();

        for (; i != end; ++i)
        {
            MamdaMultiSecurityHandler* handler = *i;
            handler->onSecurityCreate (subscription, 
                                       mSelf, 
                                       symbol, 
                                       false);
        }
    }    

    void MamdaMultiSecurityManager::MamdaMultiSecurityManagerImpl::forwardMsg (
        MamdaSubscription*    subscription,
        const MamaMsg&        msg,
        short                 msgType)
    {
        const char* symbol = msg.getString (MamdaCommonFields::ISSUE_SYMBOL);

        if (symbol)
        {
            ListenerListMap::iterator found = mSecurityListeners.find (symbol);

            if (found != mSecurityListeners.end())
            {
                forwardMsg (found->second, 
                            subscription, 
                            msg, 
                            msgType);
            }
            else
            {
                //This allows for new securities coming online intraday
                //which were not in the cache on startup.
                createListenerListAndNotify (subscription, symbol);        
            }
        }
    }

    void MamdaMultiSecurityManager::MamdaMultiSecurityManagerImpl::forwardMsg (
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

} // namespace
