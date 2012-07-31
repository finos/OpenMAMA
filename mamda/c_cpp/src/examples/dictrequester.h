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

#ifndef DictRequestH
#define DictRequestH

#include <mamda/MamdaCommonFields.h>
#include <mamda/MamdaFields.h>
#include <mamda/MamdaFundamentalFields.h>
#include <mamda/MamdaOrderImbalanceFields.h>
#include <mamda/MamdaPubStatusFields.h>
#include <mamda/MamdaQuoteFields.h>
#include <mamda/MamdaSecStatusFields.h>
#include <mamda/MamdaTradeFields.h>

#include <mama/mamacpp.h>
#include <iostream>

using std::endl;
using namespace Wombat;

//Utility class used to obtain the DataDictionary on app startup.
class DictRequester : public MamaDictionaryCallback
{
public:
    DictRequester (mamaBridge bridge) 
                     : mDictionary (NULL)
                     , mBridge (bridge)
                     , mGotDictionary (false) {}
    virtual ~DictRequester ()
    {
        if (mDictionary) delete mDictionary;
    }

    // Exception thrown on failure
    void            requestDictionary (MamaSource* source);

    // Make sure to call requestDictionary first.
    MamaDictionary* getDictionary () {return mDictionary;}

    /*Callbacks from the MamaDictionaryCallback class*/
    virtual void onTimeout               (void);
    virtual void onError                 (const char* errMsg);
    virtual void onComplete              (void);

private:
    MamaDictionary* mDictionary;
    mamaBridge      mBridge;
    bool            mGotDictionary;
    DictRequester   (const DictRequester& copy);
    DictRequester&  operator= (const DictRequester& rhs);
};

#endif
