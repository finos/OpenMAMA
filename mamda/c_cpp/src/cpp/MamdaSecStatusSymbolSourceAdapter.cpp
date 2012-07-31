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

#include <mamda/MamdaSecStatusSymbolSourceAdapter.h>
#include <mamda/MamdaSymbolSourceHandler.h>
#include <mamda/MamdaSubscription.h>
#include <mama/mamacpp.h>
#include "MamdaUtils.h"
#include <string>

using std::string;

namespace Wombat
{

class MamdaSecStatusSymbolSourceAdapter::MamdaSecStatusSymbolSourceAdapterImpl 
{
public:
    MamdaSecStatusSymbolSourceAdapterImpl  (
        MamdaSecStatusSymbolSourceAdapter& listener);

    ~MamdaSecStatusSymbolSourceAdapterImpl () {}

    void handleSecStatusUpdate (
        MamdaSubscription*       subscription,
        MamdaSecStatusListener&  listener,
        const MamaMsg&            msg);


    MamdaSecStatusSymbolSourceAdapter&  mAdapter;
    vector<MamdaSymbolSourceHandler*>   mSymbolSourceHandlers;
    string                              mSourcedSymbol;
};

MamdaSecStatusSymbolSourceAdapter::MamdaSecStatusSymbolSourceAdapter ()
    : mImpl (*new MamdaSecStatusSymbolSourceAdapterImpl(*this))
{
}

MamdaSecStatusSymbolSourceAdapter::~MamdaSecStatusSymbolSourceAdapter ()
{
    delete &mImpl;
}

void MamdaSecStatusSymbolSourceAdapter::addHandler (
    MamdaSymbolSourceHandler*  handler)
{
    mImpl.mSymbolSourceHandlers.push_back (handler);
}

const char* MamdaSecStatusSymbolSourceAdapter::getSourcedSymbol() const
{
    return mImpl.mSourcedSymbol.c_str();
}

void MamdaSecStatusSymbolSourceAdapter::onSecStatusRecap (
    MamdaSubscription*       subscription,
    MamdaSecStatusListener&  listener,
    const MamaMsg&           msg,
    MamdaSecStatusRecap&     recap)
{
}

void MamdaSecStatusSymbolSourceAdapter::onSecStatusUpdate (
    MamdaSubscription*       subscription,
    MamdaSecStatusListener&  listener,
    const MamaMsg&            msg)
{
    if (listener.getSecurityAction() == 'A')  
    {
        mImpl.handleSecStatusUpdate (subscription, listener, msg);
    }
}

MamdaSecStatusSymbolSourceAdapter::
MamdaSecStatusSymbolSourceAdapterImpl::MamdaSecStatusSymbolSourceAdapterImpl(
    MamdaSecStatusSymbolSourceAdapter&  listener)
    : mAdapter        (listener)
    , mSourcedSymbol  ("")
{
}

void MamdaSecStatusSymbolSourceAdapter::
MamdaSecStatusSymbolSourceAdapterImpl::handleSecStatusUpdate (
    MamdaSubscription*       subscription,
    MamdaSecStatusListener&  listener,
    const MamaMsg&            msg)
{
    mSourcedSymbol = listener.getIssueSymbol();
    for (unsigned int i=0; i<mSymbolSourceHandlers.size(); i++)
    {
        // Invoke handler
        mSymbolSourceHandlers[i]->onSymbol(mAdapter);
    }
}


} // namespace
