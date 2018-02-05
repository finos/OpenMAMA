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

#include <mamda/MamdaAuctionFields.h>
#include <mamda/MamdaCommonFields.h>
#include <mama/mamacpp.h>

namespace Wombat
{
    static uint16_t  theMaxFid     = 0;
    static bool      initialised   = false;

    const MamaFieldDescriptor*  MamdaAuctionFields::UNCROSS_PRICE       = NULL;
    const MamaFieldDescriptor*  MamdaAuctionFields::UNCROSS_VOLUME      = NULL;
    const MamaFieldDescriptor*  MamdaAuctionFields::UNCROSS_PRICE_IND   = NULL;
    const MamaFieldDescriptor*  MamdaAuctionFields::AUCTION_TIME        = NULL;


    void MamdaAuctionFields::setDictionary (const MamaDictionary&  dictionary)
    {
        // Already initialised
        if (initialised)
        {
            return;
        }

        // Set the Common Fields
        if(!MamdaCommonFields::isSet ())
        {
            MamdaCommonFields::setDictionary (dictionary);
        }

        UNCROSS_PRICE            = dictionary.getFieldByName ("wUncrossPrice");
        UNCROSS_VOLUME           = dictionary.getFieldByName ("wUncrossVolume");
        UNCROSS_PRICE_IND        = dictionary.getFieldByName ("wUncrossPriceInd");
        AUCTION_TIME             = dictionary.getFieldByName ("wAuctionTime");

        theMaxFid   = dictionary.getMaxFid();
        initialised = true;
    }

    void MamdaAuctionFields::reset ()
    {
        initialised         = false;

        //reset the Common Fields
        if (MamdaCommonFields::isSet ())
        {
            MamdaCommonFields::reset ();
        }

        theMaxFid           = 0;
        UNCROSS_PRICE       = NULL;
        UNCROSS_VOLUME      = NULL;
        UNCROSS_PRICE_IND   = NULL;
        AUCTION_TIME        = NULL;
    }

    bool MamdaAuctionFields::isSet ()
    {
        return initialised;
    }

    uint16_t  MamdaAuctionFields::getMaxFid ()
    {
        return theMaxFid;
    }

} // namespace



