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

#ifndef MamdaQuoteFieldsH
#define MamdaQuoteFieldsH

#include <mamda/MamdaConfig.h>
#include <mama/mamacpp.h>

namespace Wombat
{

    class MamaFieldDescriptor;
    class MamaDictionary;

    /**
     * Utility cache of <code>MamaFieldDescriptor</code>s which are used
     * internally by the API when accessing quote related fields from update
     * messages.
     * This class should be initialized prior to using the
     * <code>MamdaQuoteListener</code> by calling setDictionary() with a
     * valid dictionary object which contains quote related fields.
     */
    class MAMDAExpDLL MamdaQuoteFields
    {
    public:
        static const MamaFieldDescriptor*  BID_PRICE;
        static const MamaFieldDescriptor*  BID_SIZE;
        static const MamaFieldDescriptor*  BID_DEPTH;
        static const MamaFieldDescriptor*  BID_PART_ID;
        static const MamaFieldDescriptor*  BID_CLOSE_PRICE;
        static const MamaFieldDescriptor*  BID_CLOSE_DATE;
        static const MamaFieldDescriptor*  BID_PREV_CLOSE_PRICE;
        static const MamaFieldDescriptor*  BID_PREV_CLOSE_DATE;
        static const MamaFieldDescriptor*  BID_HIGH;
        static const MamaFieldDescriptor*  BID_LOW;
        static const MamaFieldDescriptor*  ASK_PRICE;
        static const MamaFieldDescriptor*  ASK_SIZE;
        static const MamaFieldDescriptor*  ASK_DEPTH;
        static const MamaFieldDescriptor*  ASK_PART_ID;
        static const MamaFieldDescriptor*  ASK_CLOSE_PRICE;
        static const MamaFieldDescriptor*  ASK_CLOSE_DATE;
        static const MamaFieldDescriptor*  ASK_PREV_CLOSE_PRICE;
        static const MamaFieldDescriptor*  ASK_PREV_CLOSE_DATE;
        static const MamaFieldDescriptor*  ASK_HIGH;
        static const MamaFieldDescriptor*  ASK_LOW;
        static const MamaFieldDescriptor*  QUOTE_SEQ_NUM;
        static const MamaFieldDescriptor*  QUOTE_TIME;
        static const MamaFieldDescriptor*  QUOTE_DATE;
        static const MamaFieldDescriptor*  QUOTE_QUAL;
        static const MamaFieldDescriptor*  QUOTE_QUAL_NATIVE;
        static const MamaFieldDescriptor*  QUOTE_COUNT;
        static const MamaFieldDescriptor*  MID_PRICE;
        static const MamaFieldDescriptor*  SHORT_SALE_BID_TICK;
        static const MamaFieldDescriptor*  BID_TICK;
        
        static const MamaFieldDescriptor*  ASK_TIME;
        static const MamaFieldDescriptor*  BID_TIME;
        static const MamaFieldDescriptor*  ASK_INDICATOR;
        static const MamaFieldDescriptor*  BID_INDICATOR;
        static const MamaFieldDescriptor*  ASK_UPDATE_COUNT;
        static const MamaFieldDescriptor*  BID_UPDATE_COUNT;
        static const MamaFieldDescriptor*  ASK_YIELD;
        static const MamaFieldDescriptor*  BID_YIELD;
        static const MamaFieldDescriptor*  BID_SIZES_LIST;
        static const MamaFieldDescriptor*  ASK_SIZES_LIST;
        static const MamaFieldDescriptor*  BOOK_CONTRIBUTORS;
        static const MamaFieldDescriptor*  SHORT_SALE_CIRCUIT_BREAKER;

        // calling setDictionary will also set the Common Fields    
        static void setDictionary (const MamaDictionary&  dictionary);

        /**
        * Reset the dictionary for quote update fields
        */
        static void      reset     ();
        static uint16_t  getMaxFid ();
        static bool      isSet     ();
    };

} // namespace Wombat

#endif // MamdaQuoteFieldsH
