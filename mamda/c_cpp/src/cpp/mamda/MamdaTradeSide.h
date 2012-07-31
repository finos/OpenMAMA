/* $Id$
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

#ifndef MamdaTradeSideH
#define MamdaTradeSideH

#include <mamda/MamdaConfig.h>

namespace Wombat
{

    /**
     * An enumeration representing trade side.
     * The value 0 maps to Unknown.
     * The value 1 maps to Buy.
     * The value 2 maps to Sell.
     */
    enum MamdaTradeSide
    {
        TRADE_SIDE_UNKNOWN = 0,
        TRADE_SIDE_BUY     = 1,  
        TRADE_SIDE_SELL    = 2
    };

    /**
     * Convert a MamdaTradeSide to an appropriate, displayable
     * string.
     * 
     * @param tradeSide The <code>MamdaTradeSide</code> to stringify
     *
     * @return The stringified version of the <code>MamdaTradeSide</code>
     */
    MAMDAExpDLL const char* toString (
        MamdaTradeSide  tradeSide);

    /**
     * Convert a string representation of a trade side to the
     * enumeration.  This function is used internally for compatibility
     * with older feed handler configurations, which may send the field as
     * a string.
     *
     * @param tradeSideStr The trade side as a string.
     * 
     * @return The trade side as an enumerated value.
     */
    MAMDAExpDLL MamdaTradeSide mamdaTradeSideFromString (
        const char*  tradeSideStr);

}

#endif // MamdaTradeDirectionH
