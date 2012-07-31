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

import java.util.logging.*;
import java.util.Properties;

/**
 * Common base class for all the MamdaXXXFields classes.
 *
 * Each of the subclasses maintains a cache of relevant MamaFieldDescriptors 
 * obtained from the MamaDictionary using the fields standard name.
 * If the dictionary is being published with non standard field name mappings
 * users can pass a <code>Properties</code> object to the corresponding 
 * <code>setDictionary()</code> method which contains field mappings.
 * <br>
 * Each mapping should have the following format:<br>
 * mamda.field.<common field name>=<mapped field name> e.g.<br>
 * mamda.field.wTradePrice=myMappedTradePrice where myMappedTradePrice is the
 * name of the field under which the trade price is being published by the
 * feedhandlers and which is being published by the dictionary to describe the
 * trade price field.
 */
public class MamdaFields
{
    private static Logger myLogger =
                           Logger.getLogger("com.wombat.mamda.MamdaFields");

    /**
     * Look up a given field name mapping from the Properties object.
     * If no map exists, then the default field name is return.
     */
    protected static String lookupFieldName (Properties  properties,
                                             String      defaultFieldName)
    {
        String result = defaultFieldName;
        if (properties != null)
        {
            String possibleFieldName = 
                properties.getProperty ("mamda.field." + defaultFieldName);
            if (possibleFieldName != null)
                result = possibleFieldName;
        }
        myLogger.fine ("mapped field: " + defaultFieldName + "=>" + result);
        return result;
    }
}

