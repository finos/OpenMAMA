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

#include <mamda/MamdaTradeDirection.h>
#include <string.h>

namespace Wombat
{

    static const char* TRADE_DIR_STR_ZERO       = "";
    static const char* TRADE_DIR_STR_PLUS       = "+";
    static const char* TRADE_DIR_STR_MINUS      = "-";
    static const char* TRADE_DIR_STR_ZERO_PLUS  = "0+";
    static const char* TRADE_DIR_STR_ZERO_MINUS = "0-";
    static const char* TRADE_DIR_STR_NA         = "NA";
    static const char* TRADE_DIR_STR_UNKNOWN    = "ZZ";


    const char* toString (MamdaTradeDirection  tradeDir)
    {
        switch (tradeDir)
        {
            case TRADE_DIR_ZERO:        return TRADE_DIR_STR_ZERO;
            case TRADE_DIR_PLUS:        return TRADE_DIR_STR_PLUS;
            case TRADE_DIR_MINUS:       return TRADE_DIR_STR_MINUS;
            case TRADE_DIR_ZERO_PLUS:   return TRADE_DIR_STR_ZERO_PLUS;
            case TRADE_DIR_ZERO_MINUS:  return TRADE_DIR_STR_ZERO_MINUS;
            case TRADE_DIR_NA:          return TRADE_DIR_STR_NA;
            case TRADE_DIR_UNKNOWN:     return TRADE_DIR_STR_UNKNOWN;
        }
        return "Error!";
    }

    MamdaTradeDirection mamdaTradeDirectionFromString (const char*  tradeDir)
    {
        // Older FH configurations sent strings:
        if (strcmp (tradeDir, TRADE_DIR_STR_ZERO) == 0)
            return TRADE_DIR_ZERO;
        if (strcmp (tradeDir, TRADE_DIR_STR_PLUS) == 0)
            return TRADE_DIR_PLUS;
        if (strcmp (tradeDir, TRADE_DIR_STR_MINUS) == 0)
            return TRADE_DIR_MINUS;
        if (strcmp (tradeDir, TRADE_DIR_STR_ZERO_PLUS) == 0)
            return TRADE_DIR_ZERO_PLUS;
        if (strcmp (tradeDir, TRADE_DIR_STR_ZERO_MINUS) == 0)
            return TRADE_DIR_ZERO_MINUS;
        if (strcmp (tradeDir, TRADE_DIR_STR_NA) == 0)
            return TRADE_DIR_NA;

        // A misconfigured FH might send numbers as strings:
        if (strcmp (tradeDir,"0") == 0)
            return TRADE_DIR_ZERO;
        if (strcmp (tradeDir,"1") == 0)
            return TRADE_DIR_PLUS;
        if (strcmp (tradeDir,"2") == 0)
            return TRADE_DIR_MINUS;
        if (strcmp (tradeDir,"3") == 0)
            return TRADE_DIR_ZERO_PLUS;
        if (strcmp (tradeDir,"4") == 0)
            return TRADE_DIR_ZERO_MINUS;
        if (strcmp (tradeDir,"5") == 0)
            return TRADE_DIR_NA;

        return TRADE_DIR_UNKNOWN;
    }

}
