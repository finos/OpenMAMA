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

#include <mamda/MamdaSymbolSourceAnnounce.h>
#include <mamda/MamdaSymbolSourceHandler.h>
#include <mamda/MamdaSecStatusFields.h>
#include <mamda/MamdaSecStatusListener.h>
#include <mamda/MamdaSecStatusSymbolSourceAdapter.h>
#include <mamda/MamdaSubscription.h>
#include "MamdaUtils.h"
#include <string>

using std::string;

namespace Wombat
{

    class MamdaSymbolSourceAnnounce::MamdaSymbolSourceAnnounceImpl 
        : public MamdaSymbolSourceHandler
    {
    public:
        MamdaSymbolSourceAnnounceImpl (
            MamdaSymbolSourceAnnounce& listener);
        virtual ~MamdaSymbolSourceAnnounceImpl() {}

        void create (
            MamaQueue*            queue,
            MamaSource*           source,
            const char*           symbol,
            const MamaDictionary* dict,
            void*                 closure);

        void onSymbol (const MamdaSymbolSourceEvent&  event);

        MamdaSubscription*    mAnnounceSubsc;
        MamaQueue*            mQueue;
        MamaSource*           mSource;
        string                mSymbol;
        void*                 mClosure;

        MamdaSymbolSourceAnnounce&         mSymbolSource;
        string                             mSourcedSymbol;
        vector<MamdaSymbolSourceHandler*>  mSymbolSourceHandlers;
    };

    MamdaSymbolSourceAnnounce::MamdaSymbolSourceAnnounce ()
        : mImpl (*new MamdaSymbolSourceAnnounceImpl(*this))
    {
    }

    MamdaSymbolSourceAnnounce::~MamdaSymbolSourceAnnounce ()
    {
        delete &mImpl;
    }

    void MamdaSymbolSourceAnnounce::create(
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

    void MamdaSymbolSourceAnnounce::addHandler (MamdaSymbolSourceHandler* handler)
    {
        mImpl.mSymbolSourceHandlers.push_back(handler);
    }

    const char* MamdaSymbolSourceAnnounce::getSourcedSymbol() const
    {
        return mImpl.mSourcedSymbol.c_str();
    }

    MamdaSymbolSourceAnnounce::MamdaSymbolSourceAnnounceImpl::MamdaSymbolSourceAnnounceImpl (
        MamdaSymbolSourceAnnounce&  source)
        : mAnnounceSubsc  (NULL)
        , mQueue          (NULL)
        , mSource         (NULL)
        , mSymbol         ("")
        , mClosure        (NULL)
        , mSymbolSource   (source)
        , mSourcedSymbol  ("")
    {
        mAnnounceSubsc                             = new MamdaSubscription;
        MamdaSecStatusListener* secStatusListener  = new MamdaSecStatusListener();
        MamdaSecStatusSymbolSourceAdapter* adapter = new MamdaSecStatusSymbolSourceAdapter();

        mAnnounceSubsc->addMsgListener (secStatusListener);
        secStatusListener->addHandler  (adapter);
        adapter->addHandler (this);
    }


    void MamdaSymbolSourceAnnounce::MamdaSymbolSourceAnnounceImpl::create (
        MamaQueue*            queue,
        MamaSource*           source,
        const char*           symbol,
        const MamaDictionary* dict,
        void*                 closure)
    {
        mQueue      = queue;
        mSource     = source;
        mSymbol     = symbol;
        mClosure    = closure;

        MamdaSecStatusFields::setDictionary (*dict);
        mAnnounceSubsc->create (queue, source, symbol);
    }


    void MamdaSymbolSourceAnnounce::MamdaSymbolSourceAnnounceImpl::onSymbol (
        const MamdaSymbolSourceEvent&  event)
    {
        mSourcedSymbol = event.getSourcedSymbol();
        for (unsigned int i=0; i < mSymbolSourceHandlers.size(); i++)
        {
            mSymbolSourceHandlers[i]->onSymbol(mSymbolSource);
        }
    }

} // namespace
