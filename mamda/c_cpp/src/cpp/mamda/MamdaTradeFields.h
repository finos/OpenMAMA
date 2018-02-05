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

#ifndef MamdaTradeFieldsH
#define MamdaTradeFieldsH

#include <mamda/MamdaConfig.h>
#include <mama/mamacpp.h>

namespace Wombat
{

    class MamaFieldDescriptor;
    class MamaDictionary;

    /**
     * Utility cache of <code>MamaFieldDescriptor</code>s which are used
     * internally  by the API when accessing trade related fields from 
     * update messages.
     * This class should be initialized prior to using the
     * <code>MamdaTradeListener</code> by calling setDictionary() with a
     * valid dictionary object which contains trade related fields.
     */
    class MAMDAExpDLL MamdaTradeFields
    {
    public:
        static const MamaFieldDescriptor*  TRADE_PRICE;
        static const MamaFieldDescriptor*  TRADE_DATE;
        static const MamaFieldDescriptor*  TRADE_SIDE;
        static const MamaFieldDescriptor*  AGGRESSOR_SIDE;
        static const MamaFieldDescriptor*  TRADE_TIME;
        static const MamaFieldDescriptor*  LAST_PRICE;
        static const MamaFieldDescriptor*  LAST_VOLUME;
        static const MamaFieldDescriptor*  LAST_DATE_TIME;
        static const MamaFieldDescriptor*  LAST_PART_ID;
        static const MamaFieldDescriptor*  LAST_DIRECTION;
        static const MamaFieldDescriptor*  NET_CHANGE;
        static const MamaFieldDescriptor*  PCT_CHANGE;
        static const MamaFieldDescriptor*  TRADE_SIZE;
        static const MamaFieldDescriptor*  TOTAL_VOLUME;
        static const MamaFieldDescriptor*  OFF_EXCHANGE_TOTAL_VOLUME;
        static const MamaFieldDescriptor*  ON_EXCHANGE_TOTAL_VOLUME;
        static const MamaFieldDescriptor*  TRADE_UNITS;
        static const MamaFieldDescriptor*  HIGH_PRICE;
        static const MamaFieldDescriptor*  LOW_PRICE;
        static const MamaFieldDescriptor*  OPEN_PRICE;
        static const MamaFieldDescriptor*  CLOSE_PRICE;
        static const MamaFieldDescriptor*  CLOSE_DATE;
        static const MamaFieldDescriptor*  PREV_CLOSE_PRICE;
        static const MamaFieldDescriptor*  PREV_CLOSE_DATE;
        static const MamaFieldDescriptor*  ADJ_PREV_CLOSE;
        static const MamaFieldDescriptor*  PREV_VOLUME;
        static const MamaFieldDescriptor*  TRADE_SEQNUM;
        static const MamaFieldDescriptor*  TRADE_QUALIFIER;
        static const MamaFieldDescriptor*  TRADE_PART_ID;
        static const MamaFieldDescriptor*  TOTAL_VALUE;
        static const MamaFieldDescriptor*  OFF_EXCHANGE_TOTAL_VALUE;
        static const MamaFieldDescriptor*  ON_EXCHANGE_TOTAL_VALUE;
        static const MamaFieldDescriptor*  VWAP;
        static const MamaFieldDescriptor*  OFF_EXCHANGE_VWAP;
        static const MamaFieldDescriptor*  ON_EXCHANGE_VWAP;
        static const MamaFieldDescriptor*  STD_DEV;
        static const MamaFieldDescriptor*  STD_DEV_SUM;
        static const MamaFieldDescriptor*  STD_DEV_SUM_SQUARES;
        static const MamaFieldDescriptor*  SALE_CONDITION;
        static const MamaFieldDescriptor*  SELLERS_SALE_DAYS;
        static const MamaFieldDescriptor*  IS_IRREGULAR;
        static const MamaFieldDescriptor*  IRREG_PART_ID;
        static const MamaFieldDescriptor*  IRREG_PRICE;
        static const MamaFieldDescriptor*  IRREG_SIZE;
        static const MamaFieldDescriptor*  IRREG_TIME;
        static const MamaFieldDescriptor*  ORIG_PART_ID;
        static const MamaFieldDescriptor*  ORIG_PRICE;
        static const MamaFieldDescriptor*  ORIG_SIZE;
        static const MamaFieldDescriptor*  ORIG_SEQNUM;
        static const MamaFieldDescriptor*  ORIG_TRADE_QUALIFIER;
        static const MamaFieldDescriptor*  ORIG_SALE_CONDITION;
        static const MamaFieldDescriptor*  ORIG_SELLERS_SALE_DAYS;
        static const MamaFieldDescriptor*  ORIG_STOP_STOCK_IND;
        static const MamaFieldDescriptor*  STOP_STOCK_IND;
        static const MamaFieldDescriptor*  CORR_PART_ID;
        static const MamaFieldDescriptor*  CORR_PRICE;
        static const MamaFieldDescriptor*  CORR_SIZE;
        static const MamaFieldDescriptor*  CORR_TRADE_QUALIFIER;
        static const MamaFieldDescriptor*  CORR_SALE_CONDITION;
        static const MamaFieldDescriptor*  CORR_TRADE_ID;
        static const MamaFieldDescriptor*  CORR_SELLERS_SALE_DAYS;
        static const MamaFieldDescriptor*  CORR_STOP_STOCK_IND;
        static const MamaFieldDescriptor*  CORR_TIME;
        static const MamaFieldDescriptor*  CANCEL_TIME;
        static const MamaFieldDescriptor*  TRADE_ID;
        static const MamaFieldDescriptor*  ORIG_TRADE_ID;
        static const MamaFieldDescriptor*  PRIMARY_EXCH;
        static const MamaFieldDescriptor*  TRADE_COUNT;
        static const MamaFieldDescriptor*  BLOCK_COUNT;
        static const MamaFieldDescriptor*  BLOCK_VOLUME;
        static const MamaFieldDescriptor*  ORDER_ID;
        static const MamaFieldDescriptor*  UPDATE_AS_TRADE;
        static const MamaFieldDescriptor*  CURRENCY_CODE;
        static const MamaFieldDescriptor*  SETTLE_PRICE;
        static const MamaFieldDescriptor*  SETTLE_DATE;
        static const MamaFieldDescriptor*  HIGH_SEQNUM;
        static const MamaFieldDescriptor*  LOW_SEQNUM;
        static const MamaFieldDescriptor*  LAST_SEQNUM;
        static const MamaFieldDescriptor*  TOTAL_VOLUME_SEQNUM;
        static const MamaFieldDescriptor*  UNIQUE_ID;
        static const MamaFieldDescriptor*  TRADE_ACTION;
        static const MamaFieldDescriptor*  TRADE_EXEC_VENUE;
        static const MamaFieldDescriptor*  OFF_EXCHANGE_TRADE_PRICE;
        static const MamaFieldDescriptor*  ON_EXCHANGE_TRADE_PRICE;
        static const MamaFieldDescriptor*  TRADE_CONTRIBUTORS;
        static const MamaFieldDescriptor*  GENERIC_FLAG;
        static const MamaFieldDescriptor*  TRADE_RECAPS;
        static const MamaFieldDescriptor*  SHORT_SALE_CIRCUIT_BREAKER;
        static const MamaFieldDescriptor*  ORIG_SHORT_SALE_CIRCUIT_BREAKER; 
        static const MamaFieldDescriptor*  CORR_SHORT_SALE_CIRCUIT_BREAKER; 

        // calling setDictionary will also set the Common Fields
        static void      setDictionary (const MamaDictionary&  dictionary);

        /**
        * Reset the dictionary for trade update fields
        */
        static void      reset     ();
        static uint16_t  getMaxFid ();
        static bool      isSet     ();
    };

}

#endif // MamdaTradeFieldsH
