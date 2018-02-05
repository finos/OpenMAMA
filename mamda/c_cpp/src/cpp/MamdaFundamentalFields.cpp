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

#include <mamda/MamdaFundamentalFields.h>
#include <mamda/MamdaCommonFields.h>
#include <mama/mamacpp.h>

namespace Wombat
{

    static uint16_t  theMaxFid    = 0;
    static bool      initialised  = false;

    const char* MamdaFundamentalFields::DIV_FREQ_NONE          = "0";
    const char* MamdaFundamentalFields::DIV_FREQ_MONTHLY       = "1M";
    const char* MamdaFundamentalFields::DIV_FREQ_QUARTERLY     = "3M";
    const char* MamdaFundamentalFields::DIV_FREQ_SEMI_ANNUALLY = "6M";
    const char* MamdaFundamentalFields::DIV_FREQ_ANNUALLY      = "1Y";
    const char* MamdaFundamentalFields::DIV_FREQ_SPECIAL       = "SP";
    const char* MamdaFundamentalFields::DIV_FREQ_IRREGULAR     = "NA";
    const char* MamdaFundamentalFields::DIV_FREQ_INVALID       = "ER";
    const char* MamdaFundamentalFields::MRKT_SEGMENT_NONE      = "None";
    const char* MamdaFundamentalFields::MRKT_SECTOR_NONE       = "None";

    const MamaFieldDescriptor*  MamdaFundamentalFields::CORP_ACT_TYPE       = NULL;
    const MamaFieldDescriptor*  MamdaFundamentalFields::DIVIDEND_PRICE      = NULL;
    const MamaFieldDescriptor*  MamdaFundamentalFields::DIVIDEND_FREQ       = NULL;
    const MamaFieldDescriptor*  MamdaFundamentalFields::DIVIDEND_EX_DATE    = NULL;
    const MamaFieldDescriptor*  MamdaFundamentalFields::DIVIDEND_PAY_DATE   = NULL;
    const MamaFieldDescriptor*  MamdaFundamentalFields::DIVIDEND_REC_DATE   = NULL;
    const MamaFieldDescriptor*  MamdaFundamentalFields::DIVIDEND_CURRENCY   = NULL;
    const MamaFieldDescriptor*  MamdaFundamentalFields::SHARES_OUT          = NULL;
    const MamaFieldDescriptor*  MamdaFundamentalFields::SHARES_FLOAT        = NULL;
    const MamaFieldDescriptor*  MamdaFundamentalFields::SHARES_AUTH         = NULL;
    const MamaFieldDescriptor*  MamdaFundamentalFields::EARN_PER_SHARE      = NULL;
    const MamaFieldDescriptor*  MamdaFundamentalFields::VOLATILITY          = NULL;
    const MamaFieldDescriptor*  MamdaFundamentalFields::PRICE_EARN_RATIO    = NULL;
    const MamaFieldDescriptor*  MamdaFundamentalFields::YIELD               = NULL;
    const MamaFieldDescriptor*  MamdaFundamentalFields::MRKT_SEGM_NATIVE    = NULL;
    const MamaFieldDescriptor*  MamdaFundamentalFields::MRKT_SECT_NATIVE    = NULL;
    const MamaFieldDescriptor*  MamdaFundamentalFields::MRKT_SEGMENT        = NULL;
    const MamaFieldDescriptor*  MamdaFundamentalFields::MRKT_SECTOR         = NULL;
    const MamaFieldDescriptor*  MamdaFundamentalFields::RISK_FREE_RATE      = NULL;
    const MamaFieldDescriptor*  MamdaFundamentalFields::HIST_VOLATILITY     = NULL;

    void MamdaFundamentalFields::setDictionary (const MamaDictionary&  dictionary)
    {
        // already initialised
        if (initialised)
        {
            return;
        }

        // Set the Common Fields
        if (!MamdaCommonFields::isSet())
        {
            MamdaCommonFields::setDictionary (dictionary);
        }

        CORP_ACT_TYPE       = dictionary.getFieldByName ("wCorpActType");
        DIVIDEND_PRICE      = dictionary.getFieldByName ("wDividend");
        DIVIDEND_FREQ       = dictionary.getFieldByName ("wDivFreq");
        DIVIDEND_EX_DATE    = dictionary.getFieldByName ("wDivExDate");
        DIVIDEND_PAY_DATE   = dictionary.getFieldByName ("wDivPayDate");
        DIVIDEND_REC_DATE   = dictionary.getFieldByName ("wDivRecordDate");
        DIVIDEND_CURRENCY   = dictionary.getFieldByName ("wDivCurrency");
        SHARES_OUT          = dictionary.getFieldByName ("wSharesOut");
        SHARES_FLOAT        = dictionary.getFieldByName ("wSharesFloat");
        SHARES_AUTH         = dictionary.getFieldByName ("wSharesAuth");
        EARN_PER_SHARE      = dictionary.getFieldByName ("wEarnPerShare");
        VOLATILITY          = dictionary.getFieldByName ("wVolatility");
        PRICE_EARN_RATIO    = dictionary.getFieldByName ("wPeRatio");
        YIELD               = dictionary.getFieldByName ("wYield");
        MRKT_SEGM_NATIVE    = dictionary.getFieldByName ("wMarketSegmentNative");
        MRKT_SECT_NATIVE    = dictionary.getFieldByName ("wMarketSectorNative");
        MRKT_SEGMENT        = dictionary.getFieldByName ("wMarketSegment");
        MRKT_SECTOR         = dictionary.getFieldByName ("wMarketSector");
        RISK_FREE_RATE      = dictionary.getFieldByName ("wRiskFreeRate");
        HIST_VOLATILITY     = dictionary.getFieldByName ("wHistVolatility");

        theMaxFid = dictionary.getMaxFid();
        initialised = true;
    }

    void MamdaFundamentalFields::reset ()
    {
        initialised         = false;

        //reset the Common Fields
        if (MamdaCommonFields::isSet())
        {
            MamdaCommonFields::reset ();
        }

        theMaxFid           = 0;
        CORP_ACT_TYPE       = NULL;
        DIVIDEND_PRICE      = NULL;
        DIVIDEND_FREQ       = NULL;
        DIVIDEND_EX_DATE    = NULL;
        DIVIDEND_PAY_DATE   = NULL;
        DIVIDEND_REC_DATE   = NULL;
        DIVIDEND_CURRENCY   = NULL;
        SHARES_OUT          = NULL;
        SHARES_FLOAT        = NULL;
        SHARES_AUTH         = NULL;
        EARN_PER_SHARE      = NULL;
        VOLATILITY          = NULL;
        PRICE_EARN_RATIO    = NULL;
        YIELD               = NULL;
        MRKT_SEGM_NATIVE    = NULL;
        MRKT_SECT_NATIVE    = NULL;
        MRKT_SEGMENT        = NULL;
        MRKT_SECTOR         = NULL;
        RISK_FREE_RATE      = NULL;
        HIST_VOLATILITY     = NULL;
    }

    bool MamdaFundamentalFields::isSet ()
    {
        return initialised;
    }

    uint16_t  MamdaFundamentalFields::getMaxFid()
    {
        return theMaxFid;
    }

}
