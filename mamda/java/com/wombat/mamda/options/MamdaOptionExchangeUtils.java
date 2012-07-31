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


/**
 * A class with static utility functions for dealing with exchanges.
 */

public class MamdaOptionExchangeUtils
{
    /**
     * Return whether the exchange ID is the one used to represent the
     * best bid and offer.  Currently hardcoded to match "", "BBO" and
     * "Z".
     */
    static public boolean isBbo (String exchange)
    {
        return (exchange == null)  || exchange.equals("") ||
            exchange.equals("BBO") || exchange.equals("Z");
    }

    /**
     * Return whether the exchange ID is the one used to represent the
     * NYSE Technolgoies calculated best bid and offer.  Currently hardcoded to
     * match "BBO".
     */
    static public boolean isWombatBbo (String exchange)
    {
        return (exchange.equals("WBBO"));
    }

}
