/* $Id$
 *
 * OpenMAMA: The open middleware agnostic messaging API
 * Copyright (C) 2012 NYSE Technologies, Inc.
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

package com.wombat.mamda.orderbook;

import com.wombat.mamda.MamdaCommonFields;
import com.wombat.mamda.MamdaFields;
import com.wombat.mama.MamaFieldDescriptor;
import com.wombat.mama.MamaDictionary;

import java.util.Properties;

/**
* Maintains a cache of common orderbook related field descriptors.
*/

public class MamdaOrderBookFields extends MamdaFields
{
    public  static MamaFieldDescriptor   SRC_TIME            = null;
    public  static MamaFieldDescriptor   ACTIVITY_TIME       = null;
    public  static MamaFieldDescriptor   PART_ID             = null;
    public  static MamaFieldDescriptor   SEQNUM              = null;
    public  static MamaFieldDescriptor   BOOK_TIME           = null;
    public  static MamaFieldDescriptor   NUM_LEVELS          = null;
    public  static MamaFieldDescriptor   BOOK_PROPERTIES     = null;
    public  static MamaFieldDescriptor   BOOK_PROP_MSG_TYPE  = null;
    public  static MamaFieldDescriptor   PRICE_LEVELS        = null;
    public  static MamaFieldDescriptor   PL_ACTION           = null;
    public  static MamaFieldDescriptor   PL_PRICE            = null;
    public  static MamaFieldDescriptor   PL_SIDE             = null;
    public  static MamaFieldDescriptor   PL_SIZE             = null;
    public  static MamaFieldDescriptor   PL_SIZE_CHANGE      = null;
    public  static MamaFieldDescriptor   PL_TIME             = null;
    public  static MamaFieldDescriptor   PL_NUM_ENTRIES      = null;
    public  static MamaFieldDescriptor   PL_NUM_ATTACH       = null;
    public  static MamaFieldDescriptor   PL_ENTRIES          = null;
    public  static MamaFieldDescriptor   PL_PROPERTIES       = null;
    public  static MamaFieldDescriptor   PL_PROP_MSG_TYPE    = null;
    public  static MamaFieldDescriptor   ENTRY_ID            = null;
    public  static MamaFieldDescriptor   ENTRY_ACTION        = null;
    public  static MamaFieldDescriptor   ENTRY_REASON        = null;
    public  static MamaFieldDescriptor   ENTRY_SIZE          = null;
    public  static MamaFieldDescriptor   ENTRY_TIME          = null;
    public  static MamaFieldDescriptor   ENTRY_STATUS        = null;
    public  static MamaFieldDescriptor   ENTRY_PROPERTIES    = null;
    public  static MamaFieldDescriptor   ENTRY_PROP_MSG_TYPE = null;
    public  static MamaFieldDescriptor[] PRICE_LEVEL         = null;
    public  static MamaFieldDescriptor[] PL_ENTRY            = null;
    public  static MamaFieldDescriptor   BID_MARKET_ORDERS   = null;
    public  static MamaFieldDescriptor   ASK_MARKET_ORDERS   = null;
    public  static MamaFieldDescriptor   BOOK_TYPE           = null;

    private static int                   theNumLevelFields   = 0;
    private static int                   theNumEntryFields   = 0;
    private static int                   theMaxFid           = 0;
    public static MamaDictionary         theDictionary       = null;
    private static boolean               initialised         = false;

    /**
     * Set the dictionary for common orderbook fields.
     * Maintains a cache of MamaFieldDescriptors for common orderbook
     * related fields. The <code>properties</code> parameter allows users
     * of the API to map the common dictionary names to something else
     * if they are being published under different names.
     *
     * @param dictionary A reference to a valid MamaDictionary
     * @param properties A Properties object containing field mappings. (See
     * MamdaFields for more information)
     */
    public static void setDictionary (final MamaDictionary  dictionary,
                                      final Properties      properties)
    {
        // already initialised
        if (initialised)
        {
            return;
        }

        // initialise the common fields
        MamdaCommonFields.setDictionary (dictionary, null);

        theDictionary = dictionary;
        String wSrcTime          = lookupFieldName (properties,
                                                    "wSrcTime");
        String wActivityTime     = lookupFieldName (properties,
                                                    "wActivityTime");
        String wPartId           = lookupFieldName (properties,
                                                    "wPartId");
        String wSeqNum           = lookupFieldName (properties,
                                                    "wSeqNum");
        String wBookTime         = lookupFieldName (properties,
                                                    "wBookTime");
        String wNumLevels        = lookupFieldName (properties,
                                                    "wNumLevels");
        String wBookPropFids     = lookupFieldName (properties,
                                                    "wBookPropFids");
        String wBookPropMsgType  = lookupFieldName (properties,
                                                    "wBookPropMsgType");
        String wPriceLevels      = lookupFieldName (properties,
                                                    "wPriceLevels");
        String wPlAction         = lookupFieldName (properties,
                                                    "wPlAction");
        String wPlPrice          = lookupFieldName (properties,
                                                    "wPlPrice");
        String wPlSide           = lookupFieldName (properties,
                                                    "wPlSide");
        String wPlSize           = lookupFieldName (properties,
                                                    "wPlSize");
        String wPlSizeChange     = lookupFieldName (properties,
                                                    "wPlSizeChange");
        String wPlTime           = lookupFieldName (properties,
                                                    "wPlTime");
        String wPlNumEntries     = lookupFieldName (properties,
                                                    "wPlNumEntries");
        String wPlNumAttach      = lookupFieldName (properties,
                                                    "wPlNumAttach");
        String wPlEntries        = lookupFieldName (properties,
                                                    "wPlEntries");
        String wPlPropFids       = lookupFieldName (properties,
                                                    "wPlPropFids");
        String wPlPropMsgType    = lookupFieldName (properties,
                                                    "wPlPropMsgType");
        String wEntryId          = lookupFieldName (properties,
                                                    "wEntryId");
        String wEntryAction      = lookupFieldName (properties,
                                                    "wEntryAction");
        String wEntryReason      = lookupFieldName (properties,
                                                    "wEntryReason");
        String wEntrySize        = lookupFieldName (properties,
                                                    "wEntrySize");
        String wEntryTime        = lookupFieldName (properties,
                                                    "wEntryTime");
        String wEntryStatus      = lookupFieldName (properties,
                                                    "wEntryStatus");
        String wEntryPropFids    = lookupFieldName (properties,
                                                    "wEntryPropFids");
        String wEntryPropMsgType = lookupFieldName (properties,
                                                    "wEntryPropMsgType");
        String wBidMarketOrders  = lookupFieldName (properties,
                                                    "wBidMarketOrders");
        String wAskMarketOrders  = lookupFieldName (properties,
                                                    "wAskMarketOrders");
        String wBookType         = lookupFieldName (properties,
        											"wBookType");

        SRC_TIME           = dictionary.getFieldByName (wSrcTime);
        ACTIVITY_TIME      = dictionary.getFieldByName (wActivityTime);
        PART_ID            = dictionary.getFieldByName (wPartId);
        SEQNUM             = dictionary.getFieldByName (wSeqNum);
        BOOK_TIME          = dictionary.getFieldByName (wBookTime);
        NUM_LEVELS         = dictionary.getFieldByName (wNumLevels);
        PRICE_LEVELS       = dictionary.getFieldByName (wPriceLevels);
        BOOK_PROPERTIES = dictionary.getFieldByName (wBookPropFids);
        BOOK_PROP_MSG_TYPE = dictionary.getFieldByName (wBookPropMsgType);
        PL_ACTION          = dictionary.getFieldByName (wPlAction);
        PL_PRICE           = dictionary.getFieldByName (wPlPrice);
        PL_SIDE            = dictionary.getFieldByName (wPlSide);
        PL_SIZE            = dictionary.getFieldByName (wPlSize);
        PL_SIZE_CHANGE     = dictionary.getFieldByName (wPlSizeChange);
        PL_TIME            = dictionary.getFieldByName (wPlTime);
        PL_NUM_ENTRIES     = dictionary.getFieldByName (wPlNumEntries);
        PL_NUM_ATTACH      = dictionary.getFieldByName (wPlNumAttach);
        PL_ENTRIES         = dictionary.getFieldByName (wPlEntries);
        PL_PROPERTIES      = dictionary.getFieldByName (wPlPropFids);
        PL_PROP_MSG_TYPE   = dictionary.getFieldByName (wPlPropMsgType);
        ENTRY_ID           = dictionary.getFieldByName (wEntryId);
        ENTRY_ACTION       = dictionary.getFieldByName (wEntryAction);
        ENTRY_REASON       = dictionary.getFieldByName (wEntryReason);
        ENTRY_SIZE         = dictionary.getFieldByName (wEntrySize);
        ENTRY_TIME         = dictionary.getFieldByName (wEntryTime);
        ENTRY_STATUS       = dictionary.getFieldByName (wEntryStatus);
        ENTRY_PROPERTIES   = dictionary.getFieldByName (wEntryPropFids);
        ENTRY_PROP_MSG_TYPE= dictionary.getFieldByName (wEntryPropMsgType);
        BID_MARKET_ORDERS  = dictionary.getFieldByName (wBidMarketOrders);
        ASK_MARKET_ORDERS  = dictionary.getFieldByName (wAskMarketOrders);
        BOOK_TYPE          = dictionary.getFieldByName (wBookType);

        // Look for non-vector price level fields (as per legacy
        // MDRV).  First count them to get a maximum array size.
        theNumLevelFields = 0;
        while (true)
        {
            String wPriceLevel = lookupFieldName (properties,
                                 "wPriceLevel" + (theNumLevelFields+1));
            if (dictionary.getFieldByName (wPriceLevel) == null)
                break;
            theNumLevelFields++;
        }

        // Note we want to allocate a 0 length array if their are no entries.
        PRICE_LEVEL = new MamaFieldDescriptor[theNumLevelFields+1];
        for (int i = 1; i <= theNumLevelFields; i++)
        {
            String wPriceLevel =
                    lookupFieldName (properties, "wPriceLevel"+i);
                PRICE_LEVEL[i] = dictionary.getFieldByName (wPriceLevel);
        }

        // Look for non-vector entry fields (as per legacy MDRV).
        // First count them to get a maximum array size.
        theNumEntryFields = 0;
        while (true)
        {
            String wPlEntry = lookupFieldName (properties,
                                 "wPlEntry"+(theNumEntryFields+1));
            if (dictionary.getFieldByName (wPlEntry) == null)
                break;
            theNumEntryFields++;
        }

        // Note we want to allocate a 0 length array if their are no entries.
        PL_ENTRY = new MamaFieldDescriptor [theNumEntryFields+1];
        for (int i = 1; i <= theNumEntryFields; i++)
        {
            String wPlEntry = lookupFieldName (properties, "wPlEntry"+i);
            PL_ENTRY[i] = dictionary.getFieldByName (wPlEntry);
        }
        theMaxFid = dictionary.getMaxFid ();
        initialised = true;
    }

    /**
     * Internal.  Returns the number of fixed (non-vector) price level
     * fields.
     */
    static public int getNumLevelFields ()
    {
        return theNumLevelFields;
    }

    /**
     * Internal.  Returns the number of fixed (non-vector) entry
     * fields.
     */
    static public int getNumEntryFields ()
    {
        return theNumEntryFields;
    }

    /**
     * Internal.  Returns whether the dictionary contains vector
     * fields (important when unpacking order book messages).
     */
    static public boolean getHasVectorFields()
    {
        return (PRICE_LEVELS != null);
    }

    /**
     * Internal.  Returns whether the dictionary contains fixed price
     * level fields (important when unpacking order book messages).
     */
    static public boolean getHasFixedLevelFields()
    {
        return (theNumLevelFields > 0);
    }

    public static int getMaxFid ()
    {
        return theMaxFid;
    }

    public static boolean isSet ()
    {
        return initialised;
    }
    /**
     * Reset the dictionary for order book update fields.
     */
    public static void reset ()
    {
        if (MamdaCommonFields.isSet())
        {
            MamdaCommonFields.reset ();
        }
        SRC_TIME           = null;
        ACTIVITY_TIME      = null;
        PART_ID            = null;
        SEQNUM             = null;
        BOOK_TIME          = null;
        NUM_LEVELS         = null;
        PRICE_LEVELS       = null;
        BOOK_PROPERTIES    = null;
        BOOK_PROP_MSG_TYPE = null;
        PL_ACTION          = null;
        PL_PRICE           = null;
        PL_SIDE            = null;
        PL_SIZE            = null;
        PL_SIZE_CHANGE     = null;
        PL_TIME            = null;
        PL_NUM_ENTRIES     = null;
        PL_NUM_ATTACH      = null;
        PL_ENTRIES         = null;
        PL_PROPERTIES      = null;
        PL_PROP_MSG_TYPE   = null;
        ENTRY_ID           = null;
        ENTRY_ACTION       = null;
        ENTRY_REASON       = null;
        ENTRY_SIZE         = null;
        ENTRY_TIME         = null;
        ENTRY_STATUS       = null;
        ENTRY_PROPERTIES   = null;
        ENTRY_PROP_MSG_TYPE= null;
        BID_MARKET_ORDERS  = null;
        ASK_MARKET_ORDERS  = null;
        PRICE_LEVEL        = null;
        PL_ENTRY           = null;
        theMaxFid          = 0;
        initialised        = false;
        theNumLevelFields  = 0;
        theNumEntryFields  = 0;
    }
}
