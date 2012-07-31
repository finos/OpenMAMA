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
 * Utility cache of <code>MamaFieldDescriptor</code>s which are used
 * internally by the API when accessing imbalance related fields from update
 * messages.
 * This class should be initialized prior to using the
 * <code>MamdaOrderImbalanceListener</code> by calling setDictionary() with a
 * valid dictionary object which contains imbalance related fields.
 */

public class MamdaOrderImbalanceFields extends MamdaFields
{
 
    public static MamaFieldDescriptor HIGH_INDICATION_PRICE = null;
    public static MamaFieldDescriptor LOW_INDICATION_PRICE  = null; 
    public static MamaFieldDescriptor INDICATION_PRICE      = null; 
    public static MamaFieldDescriptor BUY_VOLUME            = null;
    public static MamaFieldDescriptor SELL_VOLUME           = null;
    public static MamaFieldDescriptor MATCH_VOLUME          = null;
    public static MamaFieldDescriptor SECURITY_STATUS_QUAL  = null;
    public static MamaFieldDescriptor INSIDE_MATCH_PRICE    = null;
    public static MamaFieldDescriptor FAR_CLEARING_PRICE    = null;
    public static MamaFieldDescriptor NEAR_CLEARING_PRICE   = null; 
    public static MamaFieldDescriptor NO_CLEARING_PRICE     = null; 
    public static MamaFieldDescriptor PRICE_VAR_IND         = null;
    public static MamaFieldDescriptor CROSS_TYPE            = null;
   
    public static MamaFieldDescriptor SRC_TIME              = null;
    public static MamaFieldDescriptor ACTIVITY_TIME         = null;
    public static MamaFieldDescriptor SEND_TIME             = null;
    public static MamaFieldDescriptor LINE_TIME             = null;
    public static MamaFieldDescriptor MSG_TYPE              = null;
    public static MamaFieldDescriptor ISSUE_SYMBOL          = null;
    public static MamaFieldDescriptor PART_ID               = null;
    public static MamaFieldDescriptor SYMBOL                = null;
    public static MamaFieldDescriptor SEQ_NUM               = null;
    public static MamaFieldDescriptor SECURITY_STATUS_ORIG  = null;
    public static MamaFieldDescriptor SECURITY_STATUS_TIME  = null;
    public static MamaFieldDescriptor AUCTION_TIME          = null;
    
    public static int  MAX_FID = 0;
    private static boolean initialised = false;


    public static void setDictionary (MamaDictionary  dictionary,
                                      Properties      properties)
    {
        // already initialised
        if (initialised)
        {
            return;
        }

        String wHighIndicationPrice = lookupFieldName (properties, 
                                                       "wHighIndicationPrice");
        String wLowIndicationPrice  = lookupFieldName (properties,
                                                      "wLowIndicationPrice");
        String wIndicationPrice     = lookupFieldName (properties,
                                                       "wIndicationPrice");
        String wBuyVolume           = lookupFieldName (properties,
                                                       "wBuyVolume");
        String wSellVolume          = lookupFieldName (properties,
                                                       "wSellVolume");
        String wMatchVolume         = lookupFieldName (properties,
                                                       "wMatchVolume");
        String wSecStatusQual       = lookupFieldName (properties,
                                                       "wSecStatusQual");
        String wInsideMatchPrice    = lookupFieldName (properties,
                                                       "wInsideMatchPrice");
        String wFarClearingPrice    = lookupFieldName (properties,
                                                       "wFarClearingPrice");
        String wNearClearingPrice   = lookupFieldName (properties,
                                                       "wNearClearingPrice");
        String wNoClearingPrice     = lookupFieldName (properties,
                                                       "wNoClearingPrice");
        String wPriceVarInd         = lookupFieldName (properties,
                                                       "wPriceVarInd");
        String wCrossType           = lookupFieldName (properties,
                                                       "wCrossType");
        String wSrcTime             = lookupFieldName (properties,
                                                       "wSrcTime");
        String wMsgType             = lookupFieldName (properties,
                                                       "wMsgType");
        String wIssueSymbol         = lookupFieldName (properties,
                                                       "wIssueSymbol");
        String wPartId              = lookupFieldName (properties,
                                                       "wPartId");
        String wSymbol              = lookupFieldName (properties,
                                                       "wSymbol");
        String wSeqNum              = lookupFieldName (properties,
                                                       "wSeqNum");
        String wSecurityStatusOrig  = lookupFieldName (properties,
                                                       "wSecurityStatusOrig");
        String wSecurityStatusTime  = lookupFieldName (properties,
                                                       "wSecurityStatusTime");
        String wAuctionTime         = lookupFieldName (properties,
                                                       "wAuctionTime");
        String wActivityTime        = lookupFieldName (properties,
                                                       "wActivityTime");
        String wSendTime            = lookupFieldName (properties,
                                                       "wSendTime");
        String wLineTime            = lookupFieldName (properties,
                                                       "wLineTime");

        HIGH_INDICATION_PRICE = dictionary.getFieldByName (wHighIndicationPrice);
        LOW_INDICATION_PRICE  = dictionary.getFieldByName (wLowIndicationPrice);
        INDICATION_PRICE      = dictionary.getFieldByName (wIndicationPrice);
        BUY_VOLUME            = dictionary.getFieldByName (wBuyVolume);
        SELL_VOLUME           = dictionary.getFieldByName (wSellVolume);
        MATCH_VOLUME          = dictionary.getFieldByName (wMatchVolume);
        SECURITY_STATUS_QUAL  = dictionary.getFieldByName (wSecStatusQual);
        INSIDE_MATCH_PRICE    = dictionary.getFieldByName (wInsideMatchPrice);
        FAR_CLEARING_PRICE    = dictionary.getFieldByName (wFarClearingPrice);
        NEAR_CLEARING_PRICE   = dictionary.getFieldByName (wNearClearingPrice);
        NO_CLEARING_PRICE     = dictionary.getFieldByName (wNoClearingPrice);
        PRICE_VAR_IND         = dictionary.getFieldByName (wPriceVarInd);
        CROSS_TYPE            = dictionary.getFieldByName (wCrossType);
        SRC_TIME              = dictionary.getFieldByName (wSrcTime);
        MSG_TYPE              = dictionary.getFieldByName (wMsgType);
        ISSUE_SYMBOL          = dictionary.getFieldByName (wIssueSymbol);
        PART_ID               = dictionary.getFieldByName (wPartId);
        SYMBOL                = dictionary.getFieldByName (wSymbol);
        SEQ_NUM               = dictionary.getFieldByName (wSeqNum);
        SECURITY_STATUS_ORIG  = dictionary.getFieldByName (wSecurityStatusOrig);
        SECURITY_STATUS_TIME  = dictionary.getFieldByName (wSecurityStatusTime);
        AUCTION_TIME          = dictionary.getFieldByName (wAuctionTime);
        ACTIVITY_TIME         = dictionary.getFieldByName (wActivityTime);
        SEND_TIME             = dictionary.getFieldByName (wSendTime);
        LINE_TIME             = dictionary.getFieldByName (wLineTime);
        
        MAX_FID = dictionary.getMaxFid();
        initialised = true;
    }

    public static int getMaxFid ()
    {
        return MAX_FID;
    }

    public static boolean isSet ()
    {
        return initialised;
    }

    /**
    * Reset the dictionary for order imbalance update fields.
    */
    public static void reset ()
    {
        if (MamdaCommonFields.isSet())
        {
            MamdaCommonFields.reset ();
        }
        HIGH_INDICATION_PRICE = null;
        LOW_INDICATION_PRICE  = null;
        INDICATION_PRICE      = null;
        BUY_VOLUME            = null;
        SELL_VOLUME           = null;
        MATCH_VOLUME          = null;
        SECURITY_STATUS_QUAL  = null;
        INSIDE_MATCH_PRICE    = null;
        FAR_CLEARING_PRICE    = null;
        NEAR_CLEARING_PRICE   = null;
        NO_CLEARING_PRICE     = null;
        PRICE_VAR_IND         = null;
        CROSS_TYPE            = null;
        SRC_TIME              = null;
        MSG_TYPE              = null;
        ISSUE_SYMBOL          = null;
        PART_ID               = null;
        SYMBOL                = null;
        SEQ_NUM               = null;
        SECURITY_STATUS_ORIG  = null;
        SECURITY_STATUS_TIME  = null;
        AUCTION_TIME          = null;
        ACTIVITY_TIME         = null;
        SEND_TIME             = null;
        LINE_TIME             = null;
        initialised           = false;
        MAX_FID               = 0;
    }
}
