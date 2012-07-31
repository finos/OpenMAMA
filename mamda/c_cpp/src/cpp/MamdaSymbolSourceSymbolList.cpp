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

#include <mamda/MamdaSymbolSourceSymbolList.h>
#include <mamda/MamdaSymbolSourceHandler.h>
#include <mamda/MamdaSymbolListFields.h>
#include <mamda/MamdaSymbolListListener.h>
#include <mamda/MamdaSymbolListSymbolSourceAdapter.h>
#include <mamda/MamdaSubscription.h>
#include <mamda/MamdaErrorListener.h>
#include "MamdaUtils.h"
#include <string>
#include <iostream>

using std::string;

namespace Wombat
{

    class MamdaSymbolSourceSymbolList::MamdaSymbolSourceSymbolListImpl 
        : public MamdaSymbolSourceHandler
    {
    public:
        MamdaSymbolSourceSymbolListImpl (MamdaSymbolSourceSymbolList& listener);
        virtual ~MamdaSymbolSourceSymbolListImpl () {}

        void create (MamaQueue*            queue,
                     MamaSource*           source,
                     const char*           symbol,
                     const MamaDictionary* dict,
                     void*                 closure);

        void onSymbol (const MamdaSymbolSourceEvent&  event);

        MamdaSubscription*    mSymbolListSubsc;
        MamaQueue*            mQueue;
        MamaSource*           mSource;
        mamaSubscriptionType  mType;
        double                mTimeout;
        string                mSymbol;
        void*                 mClosure;

        MamdaSymbolSourceSymbolList&       mSymbolSource;
        string                             mSourcedSymbol;
        vector<MamdaSymbolSourceHandler*>  mSymbolSourceHandlers;
    };

    class SymbolListErrorListener : public MamdaErrorListener
    {
    public:
        void onError (MamdaSubscription*  subscription,
                      MamdaErrorSeverity  severity,
                      MamdaErrorCode      errorCode,
                      const char*         errorStr)
        {
            mama_log (MAMA_LOG_LEVEL_NORMAL, 
                      "Error subscribing to symbol list: %s", 
                      errorStr);
        }
    };

    MamdaSymbolSourceSymbolList::MamdaSymbolSourceSymbolList ()
        : mImpl (*new MamdaSymbolSourceSymbolListImpl(*this))
    {
    }

    MamdaSymbolSourceSymbolList::~MamdaSymbolSourceSymbolList ()
    {
        delete &mImpl;
    }

    void MamdaSymbolSourceSymbolList::create(
        MamaQueue*            queue,
        MamaSource*           source,
        const char*           symbol,
        const MamaDictionary* dict,
        void*                 closure)
    {
        mImpl.create (queue, 
                      source, 
                      symbol, 
                      dict, 
                      closure);
    }

    void MamdaSymbolSourceSymbolList::setSymbolListName (const char* symbol)
    {
        mImpl.mSymbol = symbol;
    }

    const char* MamdaSymbolSourceSymbolList::getSymbolListName() const
    {
        return mImpl.mSymbol.c_str();
    }

    void MamdaSymbolSourceSymbolList::setType (mamaSubscriptionType  type)
    {
        if ((type == MAMA_SUBSC_TYPE_SYMBOL_LIST_GROUP) ||
            (type == MAMA_SUBSC_TYPE_SYMBOL_LIST_BOOK))
        {
            mImpl.mType = type;
        }
    }

    void MamdaSymbolSourceSymbolList::setSubscriptionTimeout (double timeout)
    {
        mImpl.mTimeout = timeout;
    }

    mamaSubscriptionType MamdaSymbolSourceSymbolList::getType() const
    {
        return mImpl.mType;
    }

    void MamdaSymbolSourceSymbolList::addHandler (MamdaSymbolSourceHandler* handler)
    {
        mImpl.mSymbolSourceHandlers.push_back (handler);
    }

    const char* MamdaSymbolSourceSymbolList::getSourcedSymbol() const
    {
        return mImpl.mSourcedSymbol.c_str();
    }

    MamdaSymbolSourceSymbolList::MamdaSymbolSourceSymbolListImpl::
    MamdaSymbolSourceSymbolListImpl (
        MamdaSymbolSourceSymbolList&  source)
        : mSymbolListSubsc       (NULL)
        , mQueue                 (NULL)
        , mSource                (NULL)
        , mType                  (MAMA_SUBSC_TYPE_SYMBOL_LIST_NORMAL)
        , mTimeout               (60.0) 
        , mSymbol                ("")
        , mClosure               (NULL)
        , mSymbolSource          (source)
        , mSourcedSymbol         ("")
    {
        mSymbolListSubsc                            = new MamdaSubscription;
        MamdaSymbolListListener* symListListener    = new MamdaSymbolListListener();
        SymbolListErrorListener* errorListener      = new SymbolListErrorListener();
        MamdaSymbolListSymbolSourceAdapter* adapter = new MamdaSymbolListSymbolSourceAdapter();

        mSymbolListSubsc->addMsgListener   (symListListener);
        mSymbolListSubsc->addErrorListener (errorListener);
        symListListener->addHandler        (adapter);
        adapter->addHandler                (this);
    }


    void MamdaSymbolSourceSymbolList::MamdaSymbolSourceSymbolListImpl::create (
        MamaQueue*            queue,
        MamaSource*           source,
        const char*           symbol,
        const MamaDictionary* dict,
        void*                 closure)
    {
        mQueue      = queue;
        mSource     = source;
        mClosure    = closure;

        if (symbol)
        {
            mSymbol = symbol;
        }

        MamdaSymbolListFields::setDictionary(*dict);

        mSymbolListSubsc->setType    (mType);
        mSymbolListSubsc->setTimeout (mTimeout);
        mSymbolListSubsc->create     (queue, source, mSymbol.c_str());
    }

    void MamdaSymbolSourceSymbolList::MamdaSymbolSourceSymbolListImpl::onSymbol (
        const MamdaSymbolSourceEvent&  event)
    {
        mSourcedSymbol = event.getSourcedSymbol();
        for (unsigned int i=0; i < mSymbolSourceHandlers.size(); i++)
        {
            mSymbolSourceHandlers[i]->onSymbol (mSymbolSource);
        }
    }

} // namespace
