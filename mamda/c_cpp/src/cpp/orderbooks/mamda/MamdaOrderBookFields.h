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

#ifndef MamdaOrderBookFieldsH
#define MamdaOrderBookFieldsH

#include <mamda/MamdaOptionalConfig.h>
#include <mama/types.h>

namespace Wombat
{

    class MamaFieldDescriptor;
    class MamaDictionary;

    /**
     * Utility cache of <code>MamaFieldDescriptor</code>s which are used
     * internally by the API when accessing orderbook related fields from update
     * messages.
     * This class should be initialized prior to using the
     * <code>MamdaOrderBookListener</code> by calling <code>setDictionary()</code>
     * with a valid dictionary object which contains orderbook related fields.
     */
    class MAMDAOPTExpDLL MamdaOrderBookFields
    {
    public:
        static const MamaFieldDescriptor*  BOOK_TIME;
        static const MamaFieldDescriptor*  NUM_LEVELS;
        static const MamaFieldDescriptor*  BOOK_PROPERTIES;
        static const MamaFieldDescriptor*  BOOK_TYPE;
        static const MamaFieldDescriptor*  BOOK_PROP_MSG_TYPE;
        static const MamaFieldDescriptor*  PRICE_LEVELS;
        static const MamaFieldDescriptor*  PL_ACTION;
        static const MamaFieldDescriptor*  PL_PRICE;
        static const MamaFieldDescriptor*  PL_SIDE;
        static const MamaFieldDescriptor*  PL_SIZE;
        static const MamaFieldDescriptor*  PL_SIZE_CHANGE;
        static const MamaFieldDescriptor*  PL_TIME;
        static const MamaFieldDescriptor*  PL_NUM_ENTRIES;
        static const MamaFieldDescriptor*  PL_NUM_ATTACH;
        static const MamaFieldDescriptor*  PL_ENTRIES;
        static const MamaFieldDescriptor*  PL_PROPERTIES;
        static const MamaFieldDescriptor*  PL_PROP_MSG_TYPE;
        static const MamaFieldDescriptor*  ENTRY_ID;
        static const MamaFieldDescriptor*  ENTRY_ACTION;
        static const MamaFieldDescriptor*  ENTRY_REASON;
        static const MamaFieldDescriptor*  ENTRY_SIZE;
        static const MamaFieldDescriptor*  ENTRY_TIME;
        static const MamaFieldDescriptor*  ENTRY_STATUS;
        static const MamaFieldDescriptor*  ENTRY_PROPERTIES;
        static const MamaFieldDescriptor*  ENTRY_PROP_MSG_TYPE;
        static const MamaFieldDescriptor*  BID_MARKET_ORDERS;
        static const MamaFieldDescriptor*  ASK_MARKET_ORDERS;
        static const MamaFieldDescriptor** PRICE_LEVEL;
        static int                         PRICE_LEVEL_LENGTH;
        static const MamaFieldDescriptor** PL_ENTRY;
        static int                         PL_ENTRY_LENGTH;
        static const MamaFieldDescriptor*  BOOK_CONTRIBUTORS;

        
        // calling setDictionary will also set the Common Fields
        static void setDictionary (const MamaDictionary&  dictionary);
        static void reset ();
        static mama_u16_t  getMaxFid();
        static bool isSet ();

        /**
         * Internal.  Returns the number of fixed (non-vector) price level
         * fields.
         */
        static int getNumLevelFields ();

        /**
         * Internal.  Returns the number of fixed (non-vector) entry
         * fields.
         */
        static int getNumEntryFields ();

        /**
         * Internal.  Returns whether the dictionary contains vector
         * fields (important when unpacking order book messages).
         */
        static bool getHasVectorFields();

        /**
         * Internal.  Returns whether the dictionary contains fixed price
         * level fields (important when unpacking order book messages).
         */
        static bool getHasFixedLevelFields();

        /**
         * Get the dictionary used to setup the fields */
        static const MamaDictionary& getDictionary();
    };

} // namespace Wombat

#endif // MamdaOrderBookFieldsH
