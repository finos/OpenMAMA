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

#ifndef MamdaFundamentalFieldsH
#define MamdaFundamentalFieldsH

#include <mamda/MamdaConfig.h>
#include <mama/mamacpp.h>

namespace Wombat
{

    class MamaFieldDescriptor;
    class MamaDictionary;


    class MAMDAExpDLL MamdaFundamentalFields
    {
    public:
        static const MamaFieldDescriptor*  CORP_ACT_TYPE;
        static const MamaFieldDescriptor*  DIVIDEND_PRICE;

        static const MamaFieldDescriptor*  DIVIDEND_FREQ;
        static const char*                 DIV_FREQ_NONE;
        static const char*                 DIV_FREQ_MONTHLY;
        static const char*                 DIV_FREQ_QUARTERLY;
        static const char*                 DIV_FREQ_SEMI_ANNUALLY;
        static const char*                 DIV_FREQ_ANNUALLY;
        static const char*                 DIV_FREQ_SPECIAL;
        static const char*                 DIV_FREQ_IRREGULAR;
        static const char*                 DIV_FREQ_INVALID;

        static const MamaFieldDescriptor*  DIVIDEND_EX_DATE;
        static const MamaFieldDescriptor*  DIVIDEND_PAY_DATE;
        static const MamaFieldDescriptor*  DIVIDEND_REC_DATE;
        static const MamaFieldDescriptor*  DIVIDEND_CURRENCY;
        static const MamaFieldDescriptor*  SHARES_OUT;
        static const MamaFieldDescriptor*  SHARES_FLOAT;
        static const MamaFieldDescriptor*  SHARES_AUTH;
        static const MamaFieldDescriptor*  EARN_PER_SHARE;
        static const MamaFieldDescriptor*  VOLATILITY;
        static const MamaFieldDescriptor*  PRICE_EARN_RATIO;
        static const MamaFieldDescriptor*  YIELD;
        static const MamaFieldDescriptor*  MRKT_SEGM_NATIVE;
        static const MamaFieldDescriptor*  MRKT_SECT_NATIVE;

        static const MamaFieldDescriptor*  MRKT_SEGMENT;
        static const char*                 MRKT_SEGMENT_NONE;
        static const char*                 MRKT_SEGMENT_EMPTY;

        static const MamaFieldDescriptor*  MRKT_SECTOR;
        static const char*                 MRKT_SECTOR_NONE;
        static const char*                 MRKT_SECTOR_EMPTY;

        static const MamaFieldDescriptor*  RISK_FREE_RATE;
        static const MamaFieldDescriptor*  HIST_VOLATILITY;

        // calling setDictionary will also set the Common Fields
        static void setDictionary (const MamaDictionary&  dictionary);

        /**
        * Reset the dictionary for fundamental update fields
        */
        static void      reset     ();
        static uint16_t  getMaxFid ();
        static bool      isSet     ();
    };

}

#endif // MamdaFundamentalFieldsH
