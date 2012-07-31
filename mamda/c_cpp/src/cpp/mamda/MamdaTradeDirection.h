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

#ifndef MamdaTradeDirectionH
#define MamdaTradeDirectionH

#include <mamda/MamdaConfig.h>

namespace Wombat
{

    /**
     * An enumeration representing trade tick direction, relative to the
     * previous "last" trade.  The TRADE_DIR_ZERO_PLUS and
     * TRADE_DIR_ZERO_MINUS values indicate that there was no change in
     * tick direction, but the previous non-zero change (some number of
     * ticks ago) was up or down, respectively.
     */
    enum MamdaTradeDirection
    {
        TRADE_DIR_ZERO,           /* toString() returns ""   */
        TRADE_DIR_PLUS,           /* toString() returns "+"  */
        TRADE_DIR_MINUS,          /* toString() returns "-"  */
        TRADE_DIR_ZERO_PLUS,      /* toString() returns "0+" */
        TRADE_DIR_ZERO_MINUS,     /* toString() returns "0-" */
        TRADE_DIR_NA,             /* toString() returns "NA" */
        TRADE_DIR_UNKNOWN = 99    /* toString() returns "ZZ" */
    };


    /**
     * Convert a MamdaTradeDirection to an appropriate, displayable
     * string.
     * 
     * @param tradeDir The <code>MamdaTradeDirection</code> to stringify
     *
     * @return The stringified version of the <code>MamdaTradeDirection</code>
     */
    MAMDAExpDLL const char* toString (MamdaTradeDirection  tradeDir);

    /**
     * Convert a string representation of a trade direction to the
     * enumeration.  This function is used internally for compatibility
     * with older feed handler configurations, which may send the field as
     * a string.
     *
     * @param tradeDirStr The trade direction as a string.
     * 
     * @return The trade direction as an enumerated value.
     */
    MAMDAExpDLL MamdaTradeDirection mamdaTradeDirectionFromString (const char*  tradeDirStr);

}

#endif // MamdaTradeDirectionH
