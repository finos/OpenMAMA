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

#ifndef MamdaTradeExecVenueH
#define MamdaTradeExecVenueH

#include <mamda/MamdaConfig.h>
namespace Wombat
{

    /**
     * An enumeration representing trade execution venue.
     * The TRADE_DIR_ZERO_PLUS and
     * TRADE_DIR_ZERO_MINUS values indicate that there was no change in
     * tick direction, but the previous non-zero change (some number of
     * ticks ago) was up or down, respectively.
     */
    enum MamdaTradeExecVenue
    {
        TRADE_EXEC_VENUE_UNKNOWN,               /* toString() returns ""   */
        TRADE_EXEC_VENUE_ON_EXCHANGE,           /* toString() returns "+"  */
        TRADE_EXEC_VENUE_ON_EXCHANGE_OFF_BOOK,  /* toString() returns "-"  */
        TRADE_EXEC_VENUE_OFF_EXCHANGE,          /* toString() returns "0+" */
        TRADE_EXEC_VENUE_SYSTEM_INTERNALISER,
        TRADE_EXEC_VENUE_ON_EXCHANGE_DARK_BOOK,
        TRADE_EXEC_VENUE_ON_EXCHANGE_ON_BOOK
    };

    /**
     * Convert a MamdaTradeExecVenue to an appropriate, displayable
     * string.
     * 
     * @param tradeExecVenue The <code>MamdaTradeExecVenue</code> to stringify
     *
     * @return The stringified version of the <code>MamdaTradeExecVenue</code>
     */
    MAMDAExpDLL const char* toString (MamdaTradeExecVenue  tradeExecVenue);

    /**
     * Convert a string representation of a trade execution venue to the
     * enumeration.  This function is used internally for compatibility
     * with older feed handler configurations, which may send the field as
     * a string.
     *
     * @param tradeExecVenueStr The trade execution venue as a string.
     * 
     * @return The trade execution venue as an enumerated value.
     */
    MAMDAExpDLL MamdaTradeExecVenue mamdaTradeExecVenueFromString (
        const char*  tradeExecVenueStr);


}

#endif // MamdaTradeExecVenueH
