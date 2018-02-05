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

#include <mamda/MamdaSymbolListSymbolSourceAdapter.h>
#include <mamda/MamdaSymbolSourceHandler.h>
#include <mamda/MamdaSubscription.h>
#include <mama/mamacpp.h>
#include "MamdaUtils.h"
#include <string>

using std::string;

namespace Wombat
{

    class MamdaSymbolListSymbolSourceAdapter::MamdaSymbolListSymbolSourceAdapterImpl 
    {
    public:
        MamdaSymbolListSymbolSourceAdapterImpl (
            MamdaSymbolListSymbolSourceAdapter& listener);

        ~MamdaSymbolListSymbolSourceAdapterImpl() {}

        void handleSymbolListUpdate (
            MamdaSubscription*        subscription,
            MamdaSymbolListListener&  listener,
            const MamaMsg&            msg);

        MamdaSymbolListSymbolSourceAdapter&  mAdapter;
        vector<MamdaSymbolSourceHandler*>    mSymbolSourceHandlers;
        string                               mSourcedSymbol;
    };

    MamdaSymbolListSymbolSourceAdapter::MamdaSymbolListSymbolSourceAdapter ()
        : mImpl (*new MamdaSymbolListSymbolSourceAdapterImpl(*this))
    {
    }

    MamdaSymbolListSymbolSourceAdapter::~MamdaSymbolListSymbolSourceAdapter ()
    {
        delete &mImpl;
    }

    void MamdaSymbolListSymbolSourceAdapter::addHandler (
        MamdaSymbolSourceHandler*  handler)
    {
        mImpl.mSymbolSourceHandlers.push_back (handler);
    }

    const char* MamdaSymbolListSymbolSourceAdapter::getSourcedSymbol() const
    {
        return mImpl.mSourcedSymbol.c_str();
    }

    void MamdaSymbolListSymbolSourceAdapter::onSymbolListRecap(
        MamdaSubscription*                subscription,
        MamdaSymbolListListener&          listener,
        const MamaMsg&                    msg,
        const MamdaSymbolListRecap&       recap)
    {
        mImpl.handleSymbolListUpdate (subscription, listener, msg);
    }
      
    void MamdaSymbolListSymbolSourceAdapter::onSymbolListUpdate(
        MamdaSubscription*                subscription,
        MamdaSymbolListListener&          listener,
        const MamaMsg&                    msg,
        const MamdaSymbolListUpdate&      update)
    {
        mImpl.handleSymbolListUpdate (subscription, listener, msg);
    }

    MamdaSymbolListSymbolSourceAdapter::MamdaSymbolListSymbolSourceAdapterImpl::
    MamdaSymbolListSymbolSourceAdapterImpl(
        MamdaSymbolListSymbolSourceAdapter&  listener)
        : mAdapter            (listener)
        , mSourcedSymbol      ("")
    {
    }

    void MamdaSymbolListSymbolSourceAdapter::MamdaSymbolListSymbolSourceAdapterImpl::
    handleSymbolListUpdate (
        MamdaSubscription*        subscription,
        MamdaSymbolListListener&  listener,
        const MamaMsg&            msg)
    {
        vector<string> symbols =  listener.getSourcedSymbols();

        unsigned int numHandlers = mSymbolSourceHandlers.size();
        if (numHandlers > 0)
        {
            for (unsigned int i=0; i<symbols.size(); i++)
            {
                mSourcedSymbol = symbols[i];
                for (unsigned int j=0; j<numHandlers; j++)
                {
                    // Invoke handler
                    mSymbolSourceHandlers[j]->onSymbol(mAdapter);
                }
            }
        }
    }

} // namespace
