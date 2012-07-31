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

#include "wombat/port.h"
#include <mamda/MamdaOrderBookFields.h>
#include <mama/types.h>
#include <mamda/MamdaCommonFields.h>
#include <mama/mamacpp.h>

namespace Wombat
{

    static bool        initialised       = false;
    static int         theNumLevelFields = 0;
    static int         theNumEntryFields = 0;
    static mama_u16_t  theMaxFid         = 0;
    static const MamaDictionary* theDictionary = NULL;

    const MamaFieldDescriptor*  MamdaOrderBookFields::BOOK_TIME           = NULL;
    const MamaFieldDescriptor*  MamdaOrderBookFields::NUM_LEVELS          = NULL;
    const MamaFieldDescriptor*  MamdaOrderBookFields::BOOK_PROPERTIES     = NULL;
    const MamaFieldDescriptor*  MamdaOrderBookFields::BOOK_TYPE           = NULL;
    const MamaFieldDescriptor*  MamdaOrderBookFields::BOOK_PROP_MSG_TYPE  = NULL;
    const MamaFieldDescriptor*  MamdaOrderBookFields::PRICE_LEVELS        = NULL;
    const MamaFieldDescriptor*  MamdaOrderBookFields::PL_ACTION           = NULL;
    const MamaFieldDescriptor*  MamdaOrderBookFields::PL_PRICE            = NULL;
    const MamaFieldDescriptor*  MamdaOrderBookFields::PL_SIDE             = NULL;
    const MamaFieldDescriptor*  MamdaOrderBookFields::PL_SIZE             = NULL;
    const MamaFieldDescriptor*  MamdaOrderBookFields::PL_SIZE_CHANGE      = NULL;
    const MamaFieldDescriptor*  MamdaOrderBookFields::PL_TIME             = NULL;
    const MamaFieldDescriptor*  MamdaOrderBookFields::PL_NUM_ENTRIES      = NULL;
    const MamaFieldDescriptor*  MamdaOrderBookFields::PL_NUM_ATTACH       = NULL;
    const MamaFieldDescriptor*  MamdaOrderBookFields::PL_ENTRIES          = NULL;
    const MamaFieldDescriptor*  MamdaOrderBookFields::PL_PROPERTIES       = NULL;
    const MamaFieldDescriptor*  MamdaOrderBookFields::PL_PROP_MSG_TYPE    = NULL;
    const MamaFieldDescriptor*  MamdaOrderBookFields::ENTRY_ID            = NULL;
    const MamaFieldDescriptor*  MamdaOrderBookFields::ENTRY_ACTION        = NULL;
    const MamaFieldDescriptor*  MamdaOrderBookFields::ENTRY_REASON        = NULL;
    const MamaFieldDescriptor*  MamdaOrderBookFields::ENTRY_SIZE          = NULL;
    const MamaFieldDescriptor*  MamdaOrderBookFields::ENTRY_TIME          = NULL;
    const MamaFieldDescriptor*  MamdaOrderBookFields::ENTRY_STATUS        = NULL;
    const MamaFieldDescriptor*  MamdaOrderBookFields::ENTRY_PROPERTIES    = NULL;
    const MamaFieldDescriptor*  MamdaOrderBookFields::ENTRY_PROP_MSG_TYPE = NULL;
    const MamaFieldDescriptor*  MamdaOrderBookFields::BID_MARKET_ORDERS   = NULL;
    const MamaFieldDescriptor*  MamdaOrderBookFields::ASK_MARKET_ORDERS   = NULL;
    const MamaFieldDescriptor** MamdaOrderBookFields::PRICE_LEVEL         = NULL;
    const MamaFieldDescriptor** MamdaOrderBookFields::PL_ENTRY            = NULL;
    const MamaFieldDescriptor*  MamdaOrderBookFields::BOOK_CONTRIBUTORS   = NULL;

    int MamdaOrderBookFields::PRICE_LEVEL_LENGTH = 0;
    int MamdaOrderBookFields::PL_ENTRY_LENGTH = 0;

    void MamdaOrderBookFields::setDictionary (const MamaDictionary&  dictionary)
    {
        // already initialised
        if (initialised)
        {
            return;
        }

        //set the Common Fields
        if (!MamdaCommonFields::isSet())
        {
            MamdaCommonFields::setDictionary (dictionary);
        }

        BOOK_TIME          = dictionary.getFieldByName ("wBookTime");
        NUM_LEVELS         = dictionary.getFieldByName ("wNumLevels");
        BOOK_PROPERTIES    = dictionary.getFieldByName ("wBookPropFids");
        BOOK_TYPE          = dictionary.getFieldByName ("wBookType");
        BOOK_PROP_MSG_TYPE = dictionary.getFieldByName ("wBookPropMsgType");
        PRICE_LEVELS       = dictionary.getFieldByName ("wPriceLevels");
        PL_ACTION          = dictionary.getFieldByName ("wPlAction");
        PL_PRICE           = dictionary.getFieldByName ("wPlPrice");
        PL_SIDE            = dictionary.getFieldByName ("wPlSide");
        PL_SIZE            = dictionary.getFieldByName ("wPlSize");
        PL_SIZE_CHANGE     = dictionary.getFieldByName ("wPlSizeChange");
        PL_TIME            = dictionary.getFieldByName ("wPlTime");
        PL_NUM_ENTRIES     = dictionary.getFieldByName ("wPlNumEntries");
        PL_NUM_ATTACH      = dictionary.getFieldByName ("wPlNumAttach");
        PL_ENTRIES         = dictionary.getFieldByName ("wPlEntries");
        PL_PROPERTIES      = dictionary.getFieldByName ("wPlPropFids");
        PL_PROP_MSG_TYPE   = dictionary.getFieldByName ("wPlPropMsgType");
        ENTRY_ID           = dictionary.getFieldByName ("wEntryId");
        ENTRY_ACTION       = dictionary.getFieldByName ("wEntryAction");
        ENTRY_REASON       = dictionary.getFieldByName ("wEntryReason");
        ENTRY_SIZE         = dictionary.getFieldByName ("wEntrySize");
        ENTRY_TIME         = dictionary.getFieldByName ("wEntryTime");
        ENTRY_STATUS       = dictionary.getFieldByName ("wEntryStatus");
        ENTRY_PROPERTIES   = dictionary.getFieldByName ("wEntryPropFids");
        ENTRY_PROP_MSG_TYPE= dictionary.getFieldByName ("wEntryPropMsgType");
        BID_MARKET_ORDERS  = dictionary.getFieldByName ("wBidMarketOrders");
        ASK_MARKET_ORDERS  = dictionary.getFieldByName ("wAskMarketOrders");
        PRICE_LEVEL        = NULL;
        PRICE_LEVEL_LENGTH = 0;
        PL_ENTRY           = NULL;
        PL_ENTRY_LENGTH    = 0;
        BOOK_CONTRIBUTORS  = dictionary.getFieldByName ("wBookContributors");

        // Look for non-vector price level fields (as per legacy
        // MDRV).  First count them to get a maximum array size.
        theNumLevelFields = 0;
        while (true)
        {
            char  wPriceLevel[32];
            snprintf (wPriceLevel, 32, "wPriceLevel%d", (theNumLevelFields+1));
            if (dictionary.getFieldByName (wPriceLevel) == NULL)
                break;
            theNumLevelFields++;
        }
        if (theNumLevelFields > 0)
        {
            PRICE_LEVEL = new const MamaFieldDescriptor* [theNumLevelFields+1];
            PRICE_LEVEL_LENGTH = theNumLevelFields;
            for (int i = 1; i <= theNumLevelFields; i++)
            {
                char  wPriceLevel[32];
                snprintf (wPriceLevel, 32, "wPriceLevel%d", i);
                PRICE_LEVEL[i] = dictionary.getFieldByName (wPriceLevel);
            }
        }

        // Look for non-vector entry fields (as per legacy MDRV).
        // First count them to get a maximum array size.
        theNumEntryFields = 0;
        while (true)
        {
            char  wPlEntry[32];
            snprintf (wPlEntry, 32, "wPlEntry%d", (theNumEntryFields+1));
            if (dictionary.getFieldByName (wPlEntry) == NULL)
                break;
            theNumEntryFields++;
        }
        if (theNumEntryFields > 0)
        {
            PL_ENTRY = new const MamaFieldDescriptor* [theNumEntryFields+1];
            PL_ENTRY_LENGTH = theNumEntryFields;
            for (int i = 1; i <= theNumEntryFields; i++)
            {
                char  wPlEntry[32];
                snprintf (wPlEntry, 32, "wPlEntry%d", i);
                PL_ENTRY[i] = dictionary.getFieldByName (wPlEntry);
            }
        }

        theMaxFid = dictionary.getMaxFid();
        theDictionary = &dictionary;
        initialised = true;
    }

    void MamdaOrderBookFields::reset ()
    {
        initialised = false;

        // Reset the Common Fields
        if (MamdaCommonFields::isSet())
        {
            MamdaCommonFields::reset ();
        }

        theNumLevelFields  = 0;
        theNumEntryFields  = 0;
        theMaxFid          = 0;
        theDictionary      = NULL;
        BOOK_TIME          = NULL;
        NUM_LEVELS         = NULL;
        BOOK_PROPERTIES    = NULL;
        BOOK_PROP_MSG_TYPE = NULL;
        PRICE_LEVELS       = NULL;
        PL_ACTION          = NULL;
        PL_PRICE           = NULL;
        PL_SIDE            = NULL;
        PL_SIZE            = NULL;
        PL_SIZE_CHANGE     = NULL;
        PL_TIME            = NULL;
        PL_NUM_ENTRIES     = NULL;
        PL_NUM_ATTACH      = NULL;
        PL_ENTRIES         = NULL;
        PL_PROPERTIES      = NULL;
        PL_PROP_MSG_TYPE   = NULL;
        ENTRY_ID           = NULL;
        ENTRY_ACTION       = NULL;
        ENTRY_REASON       = NULL;
        ENTRY_SIZE         = NULL;
        ENTRY_TIME         = NULL;
        ENTRY_STATUS       = NULL;
        ENTRY_PROPERTIES   = NULL;
        ENTRY_PROP_MSG_TYPE= NULL;
        BID_MARKET_ORDERS  = NULL;
        ASK_MARKET_ORDERS  = NULL;
        PRICE_LEVEL        = NULL;
        PRICE_LEVEL_LENGTH = 0;
        PL_ENTRY           = NULL;
        PL_ENTRY_LENGTH    = 0;
        BOOK_CONTRIBUTORS  = NULL;
    }

    bool  MamdaOrderBookFields::isSet ()
    {
        return initialised;
    }

    mama_u16_t  MamdaOrderBookFields::getMaxFid()
    {
        return theMaxFid;
    }

    int MamdaOrderBookFields::getNumLevelFields ()
    {
        return theNumLevelFields;
    }

    int MamdaOrderBookFields::getNumEntryFields ()
    {
        return theNumEntryFields;
    }

    bool MamdaOrderBookFields::getHasVectorFields()
    {
        return (PRICE_LEVELS != NULL);
    }

    bool MamdaOrderBookFields::getHasFixedLevelFields()
    {
        return (theNumLevelFields > 0);
    }

    const MamaDictionary& MamdaOrderBookFields::getDictionary()
    {
        return *theDictionary;
    }

}
