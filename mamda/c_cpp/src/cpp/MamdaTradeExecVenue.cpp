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

#include <mamda/MamdaTradeExecVenue.h>
#include <string.h>

namespace Wombat
{

    static const char* TRADE_EXEC_VENUE_STR_UNKNOWN                 = "Unknown";
    static const char* TRADE_EXEC_VENUE_STR_ON_EXCHANGE             = "OnExchange";
    static const char* TRADE_EXEC_VENUE_STR_ON_EXCHANGE_ON_BOOK     = "OnExchangeOnBook";
    static const char* TRADE_EXEC_VENUE_STR_ON_EXCHANGE_OFF_BOOK    = "OnExchangeOffBook";
    static const char* TRADE_EXEC_VENUE_STR_OFF_EXCHANGE            = "OffExchange";
    static const char* TRADE_EXEC_VENUE_STR_SYSTEM_INTERNALISER     = "SystemInternaliser";
    static const char* TRADE_EXEC_VENUE_STR_ON_EXCHANGE_DARK_BOOK   = "OnExchangeDarkBook";

    const char* toString (MamdaTradeExecVenue  tradeExecVenue)
    {
        switch (tradeExecVenue)
        {
        case TRADE_EXEC_VENUE_UNKNOWN:               return TRADE_EXEC_VENUE_STR_UNKNOWN;
        case TRADE_EXEC_VENUE_ON_EXCHANGE:           return TRADE_EXEC_VENUE_STR_ON_EXCHANGE;
        case TRADE_EXEC_VENUE_ON_EXCHANGE_ON_BOOK:   return TRADE_EXEC_VENUE_STR_ON_EXCHANGE_ON_BOOK;
        case TRADE_EXEC_VENUE_ON_EXCHANGE_OFF_BOOK:  return TRADE_EXEC_VENUE_STR_ON_EXCHANGE_OFF_BOOK;
        case TRADE_EXEC_VENUE_OFF_EXCHANGE:          return TRADE_EXEC_VENUE_STR_OFF_EXCHANGE;
        case TRADE_EXEC_VENUE_SYSTEM_INTERNALISER:   return TRADE_EXEC_VENUE_STR_SYSTEM_INTERNALISER;
        case TRADE_EXEC_VENUE_ON_EXCHANGE_DARK_BOOK: return TRADE_EXEC_VENUE_STR_ON_EXCHANGE_DARK_BOOK;    
        }
        return "Error!";
    }

    MamdaTradeExecVenue mamdaTradeExecVenueFromString (const char*  tradeExecVenue)
    {
        // Older FH configurations sent strings:
        if (strcmp (tradeExecVenue, TRADE_EXEC_VENUE_STR_UNKNOWN) == 0)
            return TRADE_EXEC_VENUE_UNKNOWN;
        if (strcmp (tradeExecVenue, TRADE_EXEC_VENUE_STR_ON_EXCHANGE) == 0)
            return TRADE_EXEC_VENUE_ON_EXCHANGE;
        if (strcmp (tradeExecVenue, TRADE_EXEC_VENUE_STR_ON_EXCHANGE_OFF_BOOK) == 0)
            return TRADE_EXEC_VENUE_ON_EXCHANGE_OFF_BOOK;
        if (strcmp (tradeExecVenue, TRADE_EXEC_VENUE_STR_OFF_EXCHANGE) == 0)
            return TRADE_EXEC_VENUE_OFF_EXCHANGE;
        if (strcmp (tradeExecVenue, TRADE_EXEC_VENUE_STR_SYSTEM_INTERNALISER) == 0)
            return TRADE_EXEC_VENUE_SYSTEM_INTERNALISER;
        if (strcmp (tradeExecVenue, TRADE_EXEC_VENUE_STR_ON_EXCHANGE_DARK_BOOK) == 0)
            return TRADE_EXEC_VENUE_ON_EXCHANGE_DARK_BOOK;
        if (strcmp (tradeExecVenue, TRADE_EXEC_VENUE_STR_ON_EXCHANGE_ON_BOOK) == 0)
            return TRADE_EXEC_VENUE_ON_EXCHANGE_ON_BOOK;

        // A misconfigured FH might send numbers as strings:
        if (strcmp (tradeExecVenue,"0") == 0)
            return TRADE_EXEC_VENUE_UNKNOWN;
        if (strcmp (tradeExecVenue,"1") == 0)
            return TRADE_EXEC_VENUE_ON_EXCHANGE;
        if (strcmp (tradeExecVenue,"2") == 0)
            return TRADE_EXEC_VENUE_ON_EXCHANGE_ON_BOOK;
        if (strcmp (tradeExecVenue,"3") == 0)
            return TRADE_EXEC_VENUE_OFF_EXCHANGE;
        if (strcmp (tradeExecVenue,"4") == 0)
            return TRADE_EXEC_VENUE_SYSTEM_INTERNALISER;
        if (strcmp (tradeExecVenue,"5") == 0)
            return TRADE_EXEC_VENUE_ON_EXCHANGE_DARK_BOOK;
        if (strcmp (tradeExecVenue,"6") == 0)
            return TRADE_EXEC_VENUE_ON_EXCHANGE_OFF_BOOK;
        
        return TRADE_EXEC_VENUE_UNKNOWN;
    }

}
