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
* Maintains a cache of common update field descriptors.
*/
public class MamdaCommonFields extends MamdaFields
{
    public static MamaFieldDescriptor  SYMBOL        = null;
    public static MamaFieldDescriptor  ISSUE_SYMBOL  = null;
    public static MamaFieldDescriptor  INDEX_SYMBOL  = null;
    public static MamaFieldDescriptor  PART_ID       = null;
    public static MamaFieldDescriptor  SEQ_NUM       = null;
    public static MamaFieldDescriptor  SRC_TIME      = null;
    public static MamaFieldDescriptor  LINE_TIME     = null;
    public static MamaFieldDescriptor  ACTIVITY_TIME = null;
    public static MamaFieldDescriptor  SEND_TIME     = null;
    public static MamaFieldDescriptor  PUB_ID        = null;
    public static MamaFieldDescriptor  MSG_QUAL      = null;
    public static MamaFieldDescriptor  MSG_SEQ_NUM   = null;
    public static MamaFieldDescriptor  MSG_NUM       = null;
    public static MamaFieldDescriptor  MSG_TOTAL     = null;
    public static MamaFieldDescriptor  SENDER_ID     = null;

    private static boolean initialised = false;

    /**
     * Set the dictionary for common update fields.
     * Maintains a cache of MamaFieldDescriptors for common update
     * related fields.
     * The <code>properties</code> parameter allows users
     * of the API to map the common dictionary names to
     * something else if they are being published under different
     * names.
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

        SYMBOL           = lookupField (dictionary, properties, "wSymbol");
        ISSUE_SYMBOL     = lookupField (dictionary, properties, "wIssueSymbol");
        INDEX_SYMBOL     = lookupField (dictionary, properties, "wIndexSymbol");
        PART_ID          = lookupField (dictionary, properties, "wPartId");
        SEQ_NUM          = lookupField (dictionary, properties, "wSeqNum");
        SRC_TIME         = lookupField (dictionary, properties, "wSrcTime");
        LINE_TIME        = lookupField (dictionary, properties, "wLineTime");
        ACTIVITY_TIME    = lookupField (dictionary, properties, "wActivityTime");
        PUB_ID           = lookupField (dictionary, properties, "wPubId");
        MSG_QUAL         = lookupField (dictionary, properties, "wMsgQual");
        MSG_SEQ_NUM      = lookupField (dictionary,
                                        properties,
                                        MamaReservedFields.SeqNum.getName ());
        MSG_NUM          = lookupField (dictionary,
                                        properties,
                                        MamaReservedFields.MsgNum.getName ());
        MSG_TOTAL        = lookupField (dictionary,
                                        properties,
                                        MamaReservedFields.MsgTotal.getName ());
        SEND_TIME        = lookupField (dictionary,
                                        properties,
                                        MamaReservedFields.SendTime.getName ());
        SENDER_ID        = lookupField (dictionary,
                                        properties,
                                        MamaReservedFields.SenderId.getName ());
        initialised = true;
    }

    private static MamaFieldDescriptor lookupField (MamaDictionary dictionary,
                                                    Properties     properties,
                                                    String         name)
    {
        return dictionary.getFieldByName (lookupFieldName (properties, name));
    }

    public static boolean isSet ()
    {
        return initialised;
    }
    
    public static void reset ()
    {
        SYMBOL           = null;
        ISSUE_SYMBOL     = null;
        INDEX_SYMBOL     = null;
        PART_ID          = null;
        SEQ_NUM          = null;
        SRC_TIME         = null;
        LINE_TIME        = null;
        ACTIVITY_TIME    = null;
        PUB_ID           = null;
        MSG_QUAL         = null;

        if (MSG_SEQ_NUM != null)
        {
            MSG_SEQ_NUM = null;
        }

        if (MSG_NUM != null)
        {
            MSG_NUM = null;
        }

        if (MSG_TOTAL != null)
        {
            MSG_TOTAL = null;
        }

        if (SEND_TIME != null)
        {
            SEND_TIME = null;
        }

        if (SENDER_ID != null)
        {
            SENDER_ID = null;
        }

        initialised      = false;
    }

}
