/* $Id:
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

package com.wombat.mama;

/**
 * Service level of a MAMA subscription.  Provides an enumeration that
 * represents the service level (a) requested by a subscribing
 * application, or (b) provided by the publisher.  In the future, the
 * service level provided to the application might change dynamically,
 * depending upon infrastructure capacity.  So, even though the
 * requested service level is "real time", if service deteriorates for
 * any reason, the subscription may automatically switch to a
 * conflated level (after notifying the application of the switch).
 */

public class MamaServiceLevel
{
    public static final short REAL_TIME          = 0;
    public static final short SNAPSHOT           = 1;
    public static final short REPEATING_SNAPSHOT = 2;
    public static final short CONFLATED          = 5;
    public static final short UNKNOWN            = 99;

    /**
     * Return a string representation of a MAMA service level.
     */
    public static String toString (short serviceLevel)
    {
        switch (serviceLevel)
        {
        case REAL_TIME:            return "real time";
        case SNAPSHOT:             return "snapshot";
        case REPEATING_SNAPSHOT:   return "repeating snapshot";
        case CONFLATED:            return "conflated";
        default:                   return "Unknown";
        }
    }
}
