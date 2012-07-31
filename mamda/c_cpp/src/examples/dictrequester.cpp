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

#include "dictrequester.h"

void DictRequester::onTimeout (void)
{
    cerr << "Timed out waiting for data dictionary" << endl;
    Mama::stop (mBridge);
}
                                                                                                                    
void DictRequester::onError (const char* errMsg)
{
    cerr << "Error getting dictionary: " << errMsg << endl;
    Mama::stop (mBridge);
}
                                                                                                                    
void DictRequester::onComplete (void)
{
    mGotDictionary = true;

    //Setting all the dictionaries (the other setDictionary for
    //news, options and orderbooks which are shown below can be
    //set as user's discretion). This is just as example as
    //it is not necessary to set all the dictionaries for all
    //listeners but there are no adverse affects by doing so. 
    //Also note that calling setDictionary more than once does
    //not have any side effects - the relevant fields will only
    //be set once regardless.
    MamdaCommonFields::setDictionary         (*getDictionary());
    MamdaCommonFields::setDictionary         (*getDictionary());
    MamdaFundamentalFields::setDictionary    (*getDictionary());
    MamdaOrderImbalanceFields::setDictionary (*getDictionary());
    MamdaPubStatusFields::setDictionary      (*getDictionary());
    MamdaQuoteFields::setDictionary          (*getDictionary());
    MamdaSecStatusFields::setDictionary      (*getDictionary());
    MamdaTradeFields::setDictionary          (*getDictionary());

    // Can be included at user's discretion 
    //MamdaNewsFields::setDictionary (*getDictionary());
    //MamdaOptionFields::setDictionary (*getDictionary());
    //MamdaOrderBookFields::setDictionary (*getDictionary());

    Mama::stop (mBridge);
}

void DictRequester::requestDictionary (
    MamaSource*  source)
{
    
    mDictionary = new MamaDictionary;
    mDictionary->create (Mama::getDefaultEventQueue(mBridge),
                          this,
                          source);
    Mama::start (mBridge);
    if (!mGotDictionary)
    {
        throw "Failed to get data dictionary";
    }
}
