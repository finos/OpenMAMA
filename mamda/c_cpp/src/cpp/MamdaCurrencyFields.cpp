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

#include <mamda/MamdaCurrencyFields.h>
#include <mamda/MamdaCommonFields.h>
#include <mama/mamacpp.h>

namespace Wombat
{

    static uint16_t theMaxFid  = 0;
    static bool initialised    = false;

    const MamaFieldDescriptor*  MamdaCurrencyFields::BID_PRICE  = NULL;
    const MamaFieldDescriptor*  MamdaCurrencyFields::ASK_PRICE  = NULL;


    void MamdaCurrencyFields::setDictionary (const MamaDictionary&  dictionary)
    {
        // already initialised
        if (initialised)
        {
            return;
        }

        //set the Common Fields
        if(!MamdaCommonFields::isSet())
        {
            MamdaCommonFields::setDictionary (dictionary);
        }

        BID_PRICE  = dictionary.getFieldByName ("wBidPrice");
        ASK_PRICE  = dictionary.getFieldByName ("wAskPrice");

        theMaxFid = dictionary.getMaxFid();
        initialised = true;
    }

    void MamdaCurrencyFields::reset ()
    {
        initialised         = false;

        //reset the Common Fields
        if (MamdaCommonFields::isSet())
        {
            MamdaCommonFields::reset ();
        }

        theMaxFid   = 0;
        BID_PRICE   = NULL;
        ASK_PRICE   = NULL;

    }

    bool MamdaCurrencyFields::isSet ()
    {
        return initialised;
    }

    uint16_t  MamdaCurrencyFields::getMaxFid()
    {
        return theMaxFid;
    }

}

