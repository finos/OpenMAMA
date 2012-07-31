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

#include <mamda/MamdaTradeSide.h>
#include <string.h>

namespace Wombat
{

    static const char* TRADE_SIDE_UNKNOWN_STR = "Unknown";
    static const char* TRADE_SIDE_BUY_STR     = "Buy";
    static const char* TRADE_SIDE_SELL_STR    = "Sell";

    const char* toString (MamdaTradeSide  tradeSide)
    {
        switch (tradeSide)
        {
            case TRADE_SIDE_BUY:     return TRADE_SIDE_BUY_STR;
            case TRADE_SIDE_SELL:    return TRADE_SIDE_SELL_STR;   
            case TRADE_SIDE_UNKNOWN: return TRADE_SIDE_UNKNOWN_STR;
        } 
        return TRADE_SIDE_UNKNOWN_STR;
    }

    MamdaTradeSide mamdaTradeSideFromString (const char*  tradeSide)
    {
        // Older FH configurations sent strings:
        if (strcmp (tradeSide, TRADE_SIDE_BUY_STR) == 0)
            return TRADE_SIDE_BUY;
        if (strcmp (tradeSide, TRADE_SIDE_SELL_STR) == 0)
            return TRADE_SIDE_SELL;    
        if (strcmp (tradeSide, TRADE_SIDE_UNKNOWN_STR) == 0)
            return TRADE_SIDE_UNKNOWN;    
             
        // A misconfigured FH might send numbers as strings:
        if (strcmp (tradeSide,"0") == 0)
            return TRADE_SIDE_UNKNOWN;
        if (strcmp (tradeSide,"1") == 0)
            return TRADE_SIDE_BUY;
        if (strcmp (tradeSide,"2") == 0)
            return TRADE_SIDE_SELL;  
        return TRADE_SIDE_UNKNOWN;
    }

}
