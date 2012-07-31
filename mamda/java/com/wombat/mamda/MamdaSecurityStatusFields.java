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
 * Maintains a cache of common security status related FieldDescriptors.
 */

public class MamdaSecurityStatusFields extends MamdaFields
{
    public static MamaFieldDescriptor  SRC_TIME               = null;
    public static MamaFieldDescriptor  ACTIVITY_TIME          = null;
    public static MamaFieldDescriptor  SEND_TIME              = null;
    public static MamaFieldDescriptor  LINE_TIME              = null;
    public static MamaFieldDescriptor  SECURITY_STATUS        = null;
    public static MamaFieldDescriptor  SECURITY_STATUS_QUAL   = null;
    public static MamaFieldDescriptor  SECURITY_STATUS_TIME   = null;
    public static MamaFieldDescriptor  SECURITY_STATUS_ORIG   = null;
    public static MamaFieldDescriptor  SHORT_SALE_CIRCUIT_BREAKER = null;
    public static MamaFieldDescriptor  SEQNUM                 = null;
    public static MamaFieldDescriptor  PART_ID                = null;
    public static MamaFieldDescriptor  SYMBOL                 = null;
    public static MamaFieldDescriptor  ISSUE_SYMBOL           = null;
    public static MamaFieldDescriptor  REASON                 = null;
    public static int      MAX_FID     = 0;
    private static boolean initialised = false;

    /**
     * Set the dictionary for common security status fields.
     * Maintains a cache of MamaFieldDescriptors for common security status
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

        String wSrcTime            = lookupFieldName (properties,
                                                      "wSrcTime");
        String wActivityTime       = lookupFieldName (properties,
                                                      "wActivityTime");
        String wSendTime           = lookupFieldName (properties, 
                                                      "wSendTime");
        String wLineTime           = lookupFieldName (properties,
                                                      "wLineTime");
        String wSecurityStatus     = lookupFieldName (properties,
                                                      "wSecurityStatus");
        String wSecStatusQual      = lookupFieldName (properties,
                                                      "wSecStatusQual");
        String wSecurityStatusTime = lookupFieldName (properties,
                                                      "wSecurityStatusTime");
        String wSecurityStatusOrig = lookupFieldName (properties,
                                                      "wSecurityStatusOrig");
        String wShortSaleCircuitBreaker = lookupFieldName(properties,
                                                      "wShortSaleCircuitBreaker");
        String wSeqNum             = lookupFieldName (properties,
                                                      "wSeqNum");
        String wPartId             = lookupFieldName (properties,
                                                      "wPartId");
        String wSymbol             = lookupFieldName (properties,
                                                      "wSymbol");
        String wIssueSymbol        = lookupFieldName (properties,
                                                      "wIssueSymbol");
        String wReason             = lookupFieldName (properties,
                                                      "wReason");

        SRC_TIME              = dictionary.getFieldByName (wSrcTime);
        ACTIVITY_TIME         = dictionary.getFieldByName (wActivityTime);
        SEND_TIME             = dictionary.getFieldByName (wSendTime);
        LINE_TIME             = dictionary.getFieldByName (wLineTime);
        SECURITY_STATUS       = dictionary.getFieldByName (wSecurityStatus);
        SECURITY_STATUS_QUAL  = dictionary.getFieldByName (wSecStatusQual);
        SECURITY_STATUS_TIME  = dictionary.getFieldByName (wSecurityStatusTime);
        SECURITY_STATUS_ORIG  = dictionary.getFieldByName (wSecurityStatusOrig);
        SHORT_SALE_CIRCUIT_BREAKER = dictionary.getFieldByName(wShortSaleCircuitBreaker);
        SEQNUM                = dictionary.getFieldByName (wSeqNum);
        PART_ID               = dictionary.getFieldByName (wPartId);
        SYMBOL                = dictionary.getFieldByName (wSymbol);
        ISSUE_SYMBOL          = dictionary.getFieldByName (wIssueSymbol);
        REASON                = dictionary.getFieldByName (wReason);

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
     * Reset the dictionary for security status update fields.
     */
    public static void reset ()
    {
        if (MamdaCommonFields.isSet())
        {
            MamdaCommonFields.reset ();
        }
        SRC_TIME              = null;
        ACTIVITY_TIME         = null;
        SEND_TIME             = null;
        LINE_TIME             = null;
        SECURITY_STATUS       = null;
        SECURITY_STATUS_QUAL  = null;
        SECURITY_STATUS_TIME  = null;
        SECURITY_STATUS_ORIG  = null;
        SHORT_SALE_CIRCUIT_BREAKER = null;
        SEQNUM                = null;
        PART_ID               = null;
        SYMBOL                = null;
        ISSUE_SYMBOL          = null;
        REASON                = null;

        MAX_FID               = 0;
        initialised           = false;
    }
}
