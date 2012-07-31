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

package com.wombat.mamda;

import com.wombat.mama.MamaFieldDescriptor;
import com.wombat.mama.MamaDictionary;

import java.util.Properties;

/**
 * Maintains a cache of common fundamental field descriptors.
 */
public class MamdaFundamentalFields extends MamdaFields
{
    public static MamaFieldDescriptor  SRC_TIME               = null;
    public static MamaFieldDescriptor  ACTIVITY_TIME          = null;
    public static MamaFieldDescriptor  SEND_TIME              = null;
    public static MamaFieldDescriptor  LINE_TIME              = null;
    public static MamaFieldDescriptor  PART_ID                = null;
    public static MamaFieldDescriptor  SYMBOL                 = null;
    public static MamaFieldDescriptor  CORP_ACT_TYPE          = null;
    public static MamaFieldDescriptor  DIVIDEND_PRICE         = null;
    public static MamaFieldDescriptor  DIVIDEND_FREQ          = null;
    public static MamaFieldDescriptor  DIVIDEND_EX_DATE       = null;
    public static MamaFieldDescriptor  DIVIDEND_PAY_DATE      = null;
    public static MamaFieldDescriptor  DIVIDEND_REC_DATE      = null;
    public static MamaFieldDescriptor  DIVIDEND_CURRENCY      = null;
    public static MamaFieldDescriptor  SHARES_OUT             = null;
    public static MamaFieldDescriptor  SHARES_FLOAT           = null;
    public static MamaFieldDescriptor  SHARES_AUTH            = null;
    public static MamaFieldDescriptor  EARN_PER_SHARE         = null;
    public static MamaFieldDescriptor  VOLATILITY             = null;
    public static MamaFieldDescriptor  PRICE_EARN_RATIO       = null;
    public static MamaFieldDescriptor  YIELD                  = null;
    public static MamaFieldDescriptor  MRKT_SEGM_NATIVE       = null;
    public static MamaFieldDescriptor  MRKT_SECT_NATIVE       = null;
    public static MamaFieldDescriptor  MRKT_SEGMENT           = null;
    public static MamaFieldDescriptor  MRKT_SECTOR            = null;
    public static MamaFieldDescriptor  RISK_FREE_RATE         = null;
    public static MamaFieldDescriptor  HIST_VOLATILITY        = null;

    private static boolean initialised = false;

    /**
     * Set the dictionary for common fundamental fields.
     * Maintains a cache of MamaFieldDescriptors for common fundamental
     * related fields. The <code>properties</code> parameter allows users
     * of the API to map the common dictionary names to something else
     * if they are being published under different names.
     *
     * @param dictionary A reference to a valid MamaDictionary
     * @param properties A Properties object containing field mappings. (See
     * MamdaFields for more information)
     */
    public static void setDictionary (MamaDictionary  dictionary,
                                      Properties      properties)
    {
        // already initialised
        if (initialised)
        {
            return;
        }

        String wSrcTime          = lookupFieldName (properties,
                                                    "wSrcTime");
        String wActivityTime     = lookupFieldName (properties,
                                                    "wActivityTime");
        String wSendTime         = lookupFieldName (properties,
                                                    "wSendTime");
        String wLineTime         = lookupFieldName (properties,
                                                    "wLineTime");
        String wPartId           = lookupFieldName (properties,
                                                    "wPartId");
        String wSymbol           = lookupFieldName (properties,
                                                    "wSymbol");
        String wCorpActType      = lookupFieldName (properties,
                                                    "wCorpActType");
        String wDividend         = lookupFieldName (properties,
                                                    "wDividend");
        String wDivFreq          = lookupFieldName (properties,
                                                    "wDivFreq");
        String wDivExDate        = lookupFieldName (properties,
                                                    "wDivExDate");
        String wDivPayDate       = lookupFieldName (properties,
                                                    "wDivPayDate");
        String wDivRecordDate    = lookupFieldName (properties,
                                                    "wDivRecordDate");
        String wDivCurrency      = lookupFieldName (properties,
                                                    "wDivCurrency");
        String wSharesOut        = lookupFieldName (properties,
                                                    "wSharesOut");
        String wSharesFloat      = lookupFieldName (properties,
                                                    "wSharesFloat");
        String wSharesAuth       = lookupFieldName (properties,
                                                    "wSharesAuth");
        String wEarnPerShare     = lookupFieldName (properties,
                                                    "wEarnPerShare");
        String wVolatility       = lookupFieldName (properties,
                                                    "wVolatility");
        String wPeRatio          = lookupFieldName (properties,
                                                    "wPeRatio");
        String wYield            = lookupFieldName (properties,
                                                    "wYield");
        String wMarketSegmentNative = lookupFieldName (properties,
                                                       "wMarketSegmentNative");
        String wMarketSectorNative  = lookupFieldName (properties,
                                                       "wMarketSectorNative");
        String wMarketSegment    = lookupFieldName (properties,
                                                    "wMarketSegment");
        String wMarketSector     = lookupFieldName (properties,
                                                    "wMarketSector");
        String wRiskFreeRate     = lookupFieldName (properties,
                                                    "wRiskFreeRate");
        String wHistVolatility   = lookupFieldName (properties,
                                                    "wHistVolatility");

        SRC_TIME            = dictionary.getFieldByName (wSrcTime);
        ACTIVITY_TIME       = dictionary.getFieldByName (wActivityTime);
        SEND_TIME           = dictionary.getFieldByName (wSendTime);
        LINE_TIME           = dictionary.getFieldByName (wLineTime);
        PART_ID             = dictionary.getFieldByName (wPartId);
        SYMBOL              = dictionary.getFieldByName (wSymbol);
        CORP_ACT_TYPE       = dictionary.getFieldByName (wCorpActType);
        DIVIDEND_PRICE      = dictionary.getFieldByName (wDividend);
        DIVIDEND_FREQ       = dictionary.getFieldByName (wDivFreq);
        DIVIDEND_EX_DATE    = dictionary.getFieldByName (wDivExDate);
        DIVIDEND_PAY_DATE   = dictionary.getFieldByName (wDivPayDate);
        DIVIDEND_REC_DATE   = dictionary.getFieldByName (wDivRecordDate);
        DIVIDEND_CURRENCY   = dictionary.getFieldByName (wDivCurrency);
        SHARES_OUT          = dictionary.getFieldByName (wSharesOut);
        SHARES_FLOAT        = dictionary.getFieldByName (wSharesFloat);
        SHARES_AUTH         = dictionary.getFieldByName (wSharesAuth);
        EARN_PER_SHARE      = dictionary.getFieldByName (wEarnPerShare);
        VOLATILITY          = dictionary.getFieldByName (wVolatility);
        PRICE_EARN_RATIO    = dictionary.getFieldByName (wPeRatio);
        YIELD               = dictionary.getFieldByName (wYield);
        MRKT_SEGM_NATIVE    = dictionary.getFieldByName (wMarketSegmentNative);
        MRKT_SECT_NATIVE    = dictionary.getFieldByName (wMarketSectorNative);
        MRKT_SEGMENT        = dictionary.getFieldByName (wMarketSegment);
        MRKT_SECTOR         = dictionary.getFieldByName (wMarketSector);
        RISK_FREE_RATE      = dictionary.getFieldByName (wRiskFreeRate);
        HIST_VOLATILITY     = dictionary.getFieldByName (wHistVolatility);

        initialised = true;
    }

    public static boolean isSet ()
    {
        return initialised;
    }

    /**
    * Reset the dictionary for fundamental update fields.
    */
    public static void reset ()
    {
        if (MamdaCommonFields.isSet())
        {
            MamdaCommonFields.reset ();
        }
        SRC_TIME            = null;
        ACTIVITY_TIME       = null;
        SEND_TIME           = null;
        LINE_TIME           = null;
        PART_ID             = null;
        SYMBOL              = null;
        CORP_ACT_TYPE       = null;
        DIVIDEND_PRICE      = null;
        DIVIDEND_FREQ       = null;
        DIVIDEND_EX_DATE    = null;
        DIVIDEND_PAY_DATE   = null;
        DIVIDEND_REC_DATE   = null;
        DIVIDEND_CURRENCY   = null;
        SHARES_OUT          = null;
        SHARES_FLOAT        = null;
        SHARES_AUTH         = null;
        EARN_PER_SHARE      = null;
        VOLATILITY          = null;
        PRICE_EARN_RATIO    = null;
        YIELD               = null;
        MRKT_SEGM_NATIVE    = null;
        MRKT_SECT_NATIVE    = null;
        MRKT_SEGMENT        = null;
        MRKT_SECTOR         = null;
        RISK_FREE_RATE      = null;
        HIST_VOLATILITY     = null;
        initialised         = false;
    }

}
