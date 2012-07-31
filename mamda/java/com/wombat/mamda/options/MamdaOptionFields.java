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

package com.wombat.mamda.options;

import com.wombat.mamda.MamdaCommonFields;
import com.wombat.mamda.MamdaFields;
import com.wombat.mama.MamaFieldDescriptor;
import com.wombat.mama.MamaDictionary;

import java.util.Properties;

/**
* Maintains a cache of common options related field descriptors.
*/

public class MamdaOptionFields extends MamdaFields
{
    public static MamaFieldDescriptor  CONTRACT_SYMBOL        = null;
    public static MamaFieldDescriptor  UNDERLYING_SYMBOL      = null;
    public static MamaFieldDescriptor  EXPIRATION_DATE        = null;
    public static MamaFieldDescriptor  STRIKE_PRICE           = null;
    public static MamaFieldDescriptor  PUT_CALL               = null;
    public static MamaFieldDescriptor  OPEN_INTEREST          = null;
    public static MamaFieldDescriptor  EXERCISE_STYLE         = null;

    private static boolean initialised = false;

    /**
     * Set the dictionary for common options fields.
     * Maintains a cache of MamaFieldDescriptors for common options
     * related fields. The <code>properties</code> parameter allows users
     * of the API to map the common dictionary names to something else
     * if they are being published under different names.
     *
     * @param dictionary A reference to a valid MamaDictionary
     * @param properties A Properties object containing field mappings. (See
     * MamdaFields for more information)
     */
    public static void setDictionary (final MamaDictionary  dictionary,
                                      final Properties      properties)
    {
        // already initialised
        if (initialised)
        {
            return;
        }

        String wIssueSymbol      = lookupFieldName (properties,
                                                    "wIssueSymbol");
        String wUnderlyingSymbol = lookupFieldName (properties,
                                                    "wUnderlyingSymbol");
        String wExpirationDate   = lookupFieldName (properties,
                                                    "wExpirationDate");
        String wStrikePrice      = lookupFieldName (properties,
                                                    "wStrikePrice");
        String wPutCall          = lookupFieldName (properties,
                                                    "wPutCall");
        String wOpenInterest     = lookupFieldName (properties,
                                                    "wOpenInterest");
        String wExerciseStyle    = lookupFieldName (properties,
                                                    "wExerciseStyle");

        CONTRACT_SYMBOL       = dictionary.getFieldByName (wIssueSymbol);
        UNDERLYING_SYMBOL     = dictionary.getFieldByName (wUnderlyingSymbol);
        EXPIRATION_DATE       = dictionary.getFieldByName (wExpirationDate);
        STRIKE_PRICE          = dictionary.getFieldByName (wStrikePrice);
        PUT_CALL              = dictionary.getFieldByName (wPutCall);
        OPEN_INTEREST         = dictionary.getFieldByName (wOpenInterest);
        EXERCISE_STYLE        = dictionary.getFieldByName (wExerciseStyle);

        initialised = true;
    }

    public static boolean isSet ()
    {
        return initialised;
    }
    
    public static void reset ()
    {
        if (MamdaCommonFields.isSet())
        {
            MamdaCommonFields.reset();
        }
        CONTRACT_SYMBOL       = null;
        UNDERLYING_SYMBOL     = null;
        EXPIRATION_DATE       = null;
        STRIKE_PRICE          = null;
        PUT_CALL              = null;
        OPEN_INTEREST         = null;
        EXERCISE_STYLE        = null;
        initialised           = false;
    }

}
