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

import com.wombat.mamda.*;
import java.util.logging.*;
import java.util.SortedMap;
import java.util.SortedSet;
import java.util.TreeMap;

/**
 * A class that represents a set of strike prices at a particular
 * expiration date.  Each strike price of which contains a set of
 * option contracts, each of which contains exchange-specific
 * contracts.  To access a contract set for a given strike price, use
 * the get method (inherited from TreeMap).
 */

public class MamdaOptionExpirationStrikes extends TreeMap
{
    private static Logger mLogger = Logger.getLogger (
        "com.wombat.mamda.options.MamdaOptionExpirationStrikes");

    public MamdaOptionExpirationStrikes()
    {
    }

    public MamdaOptionExpirationStrikes (MamdaOptionExpirationStrikes copy)
    {
        // Shallow copy.
        super(copy);
    }

    /**
     * Trim the current set of strike prices to the given set.
     */
    public void trimStrikes (SortedSet strikeSet)
    {
        Double lowStrike = (Double) strikeSet.first();
        Double highStrike = (Double) strikeSet.last();
        highStrike = new Double(highStrike.doubleValue() + 0.0001);
        final TreeMap trimmedStrikes =
            new TreeMap(subMap(lowStrike,highStrike));
        clear();
        putAll(trimmedStrikes);
    }
}
