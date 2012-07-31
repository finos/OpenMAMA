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
import com.wombat.mama.MamaReservedFields;

import java.util.Properties;

/**
 * Cache of common Quote related field descriptors.
 * This is required to be populated if using the MamdaQuoteListener.
 */
public class MamdaQuoteFields extends MamdaFields
{
    /**
     * @deprecated Use MamdaCommonFields.SYMBOL  
     **/
    public static MamaFieldDescriptor  SYMBOL                 = null;
    /** 
     * @deprecated Use MamdaCommonFields.ISSUE_SYMBOL
     */
    public static MamaFieldDescriptor  ISSUE_SYMBOL           = null;
    /**
     * @deprecated Use MamdaCommonFields.PART_ID
     */
    public static MamaFieldDescriptor  PART_ID                = null;
    /**
     * @deprecated Use MamdaCommonFields.SRC_TIME
     */
    public static MamaFieldDescriptor  SRC_TIME               = null;
    /**
     * @deprecated Use MamdaCommonFields.ACTIVITY_TIME
     */
    public static MamaFieldDescriptor  ACTIVITY_TIME          = null;
    /**
     * @deprecated Use MamdaCommonFields.LINE_TIME
     */
    public static MamaFieldDescriptor  LINE_TIME              = null;
    /**
     * @deprecated Use MamdaCommonFields.SEND_TIME
     */
    public static MamaFieldDescriptor  SEND_TIME              = null;
    /**
     * @deprecated Use MamdaCommonFields.PUB_ID
     */
    public static MamaFieldDescriptor  PUB_ID                       = null;
    public static MamaFieldDescriptor  BID_PRICE                    = null;
    public static MamaFieldDescriptor  BID_SIZE                     = null;
    public static MamaFieldDescriptor  BID_PART_ID                  = null;
    public static MamaFieldDescriptor  BID_DEPTH                    = null;
    public static MamaFieldDescriptor  BID_CLOSE_PRICE              = null;
    public static MamaFieldDescriptor  BID_CLOSE_DATE               = null;   
    public static MamaFieldDescriptor  BID_PREV_CLOSE_PRICE         = null;
    public static MamaFieldDescriptor  BID_PREV_CLOSE_DATE          = null;
    public static MamaFieldDescriptor  BID_HIGH                     = null;
    public static MamaFieldDescriptor  BID_LOW                      = null;

    public static MamaFieldDescriptor  ASK_PRICE                    = null;
    public static MamaFieldDescriptor  ASK_SIZE                     = null;
    public static MamaFieldDescriptor  ASK_PART_ID                  = null;
    public static MamaFieldDescriptor  ASK_DEPTH                    = null;    
    public static MamaFieldDescriptor  ASK_CLOSE_PRICE              = null;
    public static MamaFieldDescriptor  ASK_CLOSE_DATE               = null;
    public static MamaFieldDescriptor  ASK_PREV_CLOSE_PRICE         = null;
    public static MamaFieldDescriptor  ASK_PREV_CLOSE_DATE          = null;
    public static MamaFieldDescriptor  ASK_HIGH                     = null;
    public static MamaFieldDescriptor  ASK_LOW                      = null;

    public static MamaFieldDescriptor  QUOTE_SEQ_NUM                = null;
    public static MamaFieldDescriptor  QUOTE_DATE                   = null;
    public static MamaFieldDescriptor  QUOTE_TIME                   = null;
    public static MamaFieldDescriptor  QUOTE_QUAL                   = null;
    public static MamaFieldDescriptor  QUOTE_QUAL_NATIVE            = null;
    public static MamaFieldDescriptor  QUOTE_COUNT                  = null;  

    /*Looks like this field can be sent as either from the feeds*/
    public static MamaFieldDescriptor  SHORT_SALE_BID_TICK          = null;
    public static MamaFieldDescriptor  BID_TICK                     = null;
    
    
    public static MamaFieldDescriptor  ASK_TIME                     = null;
    public static MamaFieldDescriptor  BID_TIME                     = null;
    public static MamaFieldDescriptor  ASK_INDICATOR                = null;
    public static MamaFieldDescriptor  BID_INDICATOR                = null;
    public static MamaFieldDescriptor  ASK_UPDATE_COUNT             = null;
    public static MamaFieldDescriptor  BID_UPDATE_COUNT             = null;
    public static MamaFieldDescriptor  ASK_YIELD                    = null;
    public static MamaFieldDescriptor  BID_YIELD                    = null;
    public static MamaFieldDescriptor  ASK_SIZES_LIST               = null;
    public static MamaFieldDescriptor  BID_SIZES_LIST               = null;
    public static MamaFieldDescriptor  SHORT_SALE_CIRCUIT_BREAKER   = null;
    
    public static int  MAX_FID = 0;
    private static boolean initialised = false;
    /**
     * Set the dictionary for common quote fields.
     * Maintains a cache of MamaFieldDescriptors for common quote
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

        MamdaCommonFields.setDictionary (dictionary, null);

        String wSymbol           = lookupFieldName (properties,
                                                    "wSymbol");
        String wIssueSymbol      = lookupFieldName (properties,
                                                    "wIssueSymbol");
        String wPartId           = lookupFieldName (properties,
                                                    "wPartId");
        String wSrcTime          = lookupFieldName (properties,
                                                    "wSrcTime");
        String wActivityTime     = lookupFieldName (properties,
                                                    "wActivityTime");
        String wLineTime         = lookupFieldName (properties,
                                                    "wLineTime");
        String MamaSendTime      = lookupFieldName (properties,
                                                    MamaReservedFields.SendTime.getName());
        String wPubId            = lookupFieldName (properties,
                                                    "wPubId");

        String wBidPrice         = lookupFieldName (properties,
                                                    "wBidPrice");
        String wBidSize          = lookupFieldName (properties,
                                                    "wBidSize");
        String wBidPartId        = lookupFieldName (properties,
                                                    "wBidPartId");
        String wBidDepth         = lookupFieldName (properties,
                                                    "wBidDepth"); 
        String wBidClose         = lookupFieldName (properties,
                                                    "wBidClose");
        String wBidCloseDate     = lookupFieldName (properties,
                                                    "wBidCloseDate");
        String wPrevBidClose     = lookupFieldName (properties,
                                                    "wPrevBidClose");
        String wPrevBidCloseDate = lookupFieldName (properties,
                                                    "wPrevBidCloseDate");
        String wBidHigh          = lookupFieldName (properties,
                                                    "wBidHigh");
        String wBidLow           = lookupFieldName (properties,
                                                    "wBidLow");

        String wAskPrice         = lookupFieldName (properties,
                                                    "wAskPrice");
        String wAskSize          = lookupFieldName (properties,
                                                    "wAskSize");
        String wAskPartId        = lookupFieldName (properties,
                                                    "wAskPartId");
        String wAskDepth         = lookupFieldName (properties,
                                                    "wAskDepth");
        String wAskClose         = lookupFieldName (properties,
                                                    "wAskClose");
        String wAskCloseDate     = lookupFieldName (properties,
                                                    "wAskCloseDate");
        String wPrevAskClose     = lookupFieldName (properties,
                                                    "wPrevAskClose");
        String wPrevAskCloseDate = lookupFieldName (properties,
                                                    "wPrevAskCloseDate");
        String wAskHigh          = lookupFieldName (properties,
                                                    "wAskHigh");
        String wAskLow           = lookupFieldName (properties,
                                                    "wAskLow");

        String wQuoteSeqNum      = lookupFieldName (properties,
                                                    "wQuoteSeqNum");
        String wQuoteDate        = lookupFieldName (properties,
                                                    "wQuoteDate");
        String wQuoteTime        = lookupFieldName (properties,
                                                    "wQuoteTime");
        String wQuoteQualifier   = lookupFieldName (properties,
                                                    "wQuoteQualifier");
        String wCondition        = lookupFieldName (properties,
                                                    "wCondition");
        String wQuoteCount       = lookupFieldName (properties,
                                                    "wQuoteCount");
        String wShortSaleBidTick = lookupFieldName (properties,
                                                    "wShortSaleBidTick");
        String wBidTick          = lookupFieldName (properties, 
                                                    "wBidTick");
                                                    
        String wAskTime          = lookupFieldName (properties, 
                                                    "wAskTime");
        String wBidTime          = lookupFieldName (properties, 
                                                    "wBidTime");
        String wAskIndicator     = lookupFieldName (properties, 
                                                    "wAskIndicator");
        String wBidIndicator     = lookupFieldName (properties, 
                                                    "wBidIndicator");
        String wAskUpdateCount   = lookupFieldName (properties, 
                                                    "wAskUpdateCount");
        String wBidUpdateCount   = lookupFieldName (properties, 
                                                    "wBidUpdateCount");
        String wAskYield         = lookupFieldName (properties, 
                                                    "wAskYield");
        String wBidYield         = lookupFieldName (properties, 
                                                    "wBidYield");
        String wBidSizesList     = lookupFieldName (properties, 
                                                    "wBidSizesList");
        String wAskSizesList     = lookupFieldName (properties, 
                                                    "wAskSizesList");
        String wShortSaleCircuitBreaker = lookupFieldName (properties,
                                                           "wShortSaleCircuitBreaker");        
        
        SYMBOL                     = dictionary.getFieldByName (wSymbol);
        ISSUE_SYMBOL               = dictionary.getFieldByName (wIssueSymbol);
        PART_ID                    = dictionary.getFieldByName (wPartId);
        SRC_TIME                   = dictionary.getFieldByName (wSrcTime);
        ACTIVITY_TIME              = dictionary.getFieldByName (wActivityTime);
        LINE_TIME                  = dictionary.getFieldByName (wLineTime);
        SEND_TIME                  = dictionary.getFieldByName (MamaSendTime);
        PUB_ID                     = dictionary.getFieldByName (wPubId);
        BID_PRICE                  = dictionary.getFieldByName (wBidPrice);
        BID_SIZE                   = dictionary.getFieldByName (wBidSize);
        BID_PART_ID                = dictionary.getFieldByName (wBidPartId);  
        BID_DEPTH                  = dictionary.getFieldByName (wBidDepth);
        BID_CLOSE_PRICE            = dictionary.getFieldByName (wBidClose);
        BID_CLOSE_DATE             = dictionary.getFieldByName (wBidCloseDate);      
        BID_PREV_CLOSE_PRICE       = dictionary.getFieldByName (wPrevBidClose);
        BID_PREV_CLOSE_DATE        = dictionary.getFieldByName (wPrevBidCloseDate);
        BID_HIGH                   = dictionary.getFieldByName (wBidHigh);
        BID_LOW                    = dictionary.getFieldByName (wBidLow);

        ASK_PRICE                  = dictionary.getFieldByName (wAskPrice);
        ASK_SIZE                   = dictionary.getFieldByName (wAskSize);
        ASK_PART_ID                = dictionary.getFieldByName (wAskPartId);
        ASK_DEPTH                  = dictionary.getFieldByName (wAskDepth);
        ASK_CLOSE_PRICE            = dictionary.getFieldByName (wAskClose);
        ASK_CLOSE_DATE             = dictionary.getFieldByName (wAskCloseDate);
        ASK_PREV_CLOSE_PRICE       = dictionary.getFieldByName (wPrevAskClose);
        ASK_PREV_CLOSE_DATE        = dictionary.getFieldByName (wPrevAskCloseDate);
        ASK_HIGH                   = dictionary.getFieldByName (wAskHigh);
        ASK_LOW                    = dictionary.getFieldByName (wAskLow);

        QUOTE_SEQ_NUM              = dictionary.getFieldByName (wQuoteSeqNum);
        QUOTE_DATE                 = dictionary.getFieldByName (wQuoteDate);
        QUOTE_TIME                 = dictionary.getFieldByName (wQuoteTime);
        QUOTE_QUAL                 = dictionary.getFieldByName (wQuoteQualifier);
        QUOTE_QUAL_NATIVE          = dictionary.getFieldByName (wCondition);
        QUOTE_COUNT                = dictionary.getFieldByName (wQuoteCount);

        SHORT_SALE_BID_TICK        = dictionary.getFieldByName (wShortSaleBidTick);
        BID_TICK                   = dictionary.getFieldByName (wBidTick);
        
        ASK_TIME                   = dictionary.getFieldByName (wAskTime);
        BID_TIME                   = dictionary.getFieldByName (wBidTime);
        ASK_INDICATOR              = dictionary.getFieldByName (wAskIndicator);
        BID_INDICATOR              = dictionary.getFieldByName (wBidIndicator);
        ASK_UPDATE_COUNT           = dictionary.getFieldByName (wAskUpdateCount);
        BID_UPDATE_COUNT           = dictionary.getFieldByName (wBidUpdateCount);
        ASK_YIELD                  = dictionary.getFieldByName (wAskYield);
        BID_YIELD                  = dictionary.getFieldByName (wBidYield);
        ASK_SIZES_LIST             = dictionary.getFieldByName (wAskSizesList);
        BID_SIZES_LIST             = dictionary.getFieldByName (wBidSizesList);
        SHORT_SALE_CIRCUIT_BREAKER = dictionary.getFieldByName (wShortSaleCircuitBreaker);

        MAX_FID = dictionary.getMaxFid();
        initialised = true;
    }


public static void reset ()
    {
        if (MamdaCommonFields.isSet())
        {
            MamdaCommonFields.reset ();
        }
      
        initialised          = false;
        MAX_FID              = 0;
    
        SYMBOL                      = null;
        ISSUE_SYMBOL                = null;
        PART_ID                     = null;
        SRC_TIME                    = null;
        ACTIVITY_TIME               = null;
        LINE_TIME                   = null;
        SEND_TIME                   = null;
        PUB_ID                      = null;
        BID_PRICE                   = null;
        BID_SIZE                    = null;
        BID_DEPTH                   = null;
        BID_PART_ID                 = null;
        BID_CLOSE_PRICE             = null;
        BID_CLOSE_DATE              = null;     
        BID_PREV_CLOSE_PRICE        = null;
        BID_PREV_CLOSE_DATE         = null;
        BID_HIGH                    = null;
        BID_LOW                     = null;
        ASK_PRICE                   = null;
        ASK_SIZE                    = null;
        ASK_DEPTH                   = null;
        ASK_PART_ID                 = null;
        ASK_CLOSE_PRICE             = null;
        ASK_CLOSE_DATE              = null;
        ASK_PREV_CLOSE_PRICE        = null;
        ASK_PREV_CLOSE_DATE         = null;
        ASK_HIGH                    = null;
        ASK_LOW                     = null;
        QUOTE_SEQ_NUM               = null;
        QUOTE_TIME                  = null;
        QUOTE_DATE                  = null;
        QUOTE_QUAL                  = null;
        QUOTE_QUAL_NATIVE           = null;
        QUOTE_COUNT                 = null;
        SHORT_SALE_BID_TICK         = null;
        BID_TICK                    = null;
    
        ASK_TIME                    = null;
        BID_TIME                    = null;
        ASK_INDICATOR               = null;
        BID_INDICATOR               = null;
        ASK_UPDATE_COUNT            = null;
        BID_UPDATE_COUNT            = null;
        ASK_YIELD                   = null;
        BID_YIELD                   = null;
        BID_SIZES_LIST              = null;
        ASK_SIZES_LIST              = null;
        SHORT_SALE_CIRCUIT_BREAKER = null;
    }

    public static int getMaxFid ()
    {
        return MAX_FID;
    }

    public static boolean isSet ()
    {
        return initialised;
    }

}
