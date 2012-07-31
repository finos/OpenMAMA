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

#include <mamda/MamdaUncrossPriceInd.h>
#include <string.h>

namespace Wombat
{

    static const char* UNCROSS_NONE_STR             = "None";
    static const char* UNCROSS_INDICATIVE_STR       = "I";
    static const char* UNCROSS_FIRM_STR             = "F";
    static const char* UNCROSS_INSUFFICIENT_VOL_STR = "V";

    const char* toString (MamdaUncrossPriceInd  uncrossPriceInd)
    {
        switch (uncrossPriceInd) 
        {
            case UNCROSS_NONE:              return UNCROSS_NONE_STR;
            case UNCROSS_INDICATIVE:        return UNCROSS_INDICATIVE_STR;
            case UNCROSS_FIRM:              return UNCROSS_FIRM_STR;
            case UNCROSS_INSUFFICIENT_VOL:  return UNCROSS_INSUFFICIENT_VOL_STR;
            default:
                return UNCROSS_NONE_STR;
        }
    }

    MamdaUncrossPriceInd mamdaUncrossPriceIndFromString (const char*  uncrossPriceInd)
    {
        if (uncrossPriceInd == NULL)
        {
           return UNCROSS_NONE;
        }

        // Older FH configurations sent strings:
        if (strcmp (uncrossPriceInd, UNCROSS_NONE_STR) == 0)
            return UNCROSS_NONE;
        if (strcmp (uncrossPriceInd, UNCROSS_INDICATIVE_STR) == 0)
            return UNCROSS_INDICATIVE;
        if (strcmp (uncrossPriceInd, UNCROSS_FIRM_STR) == 0)
            return UNCROSS_FIRM;
        if (strcmp (uncrossPriceInd, UNCROSS_INSUFFICIENT_VOL_STR) == 0)
            return UNCROSS_INSUFFICIENT_VOL;

        // A misconfigured FH might send numbers as strings:
        if (strcmp (uncrossPriceInd,"0") == 0)
            return UNCROSS_NONE;
        if (strcmp (uncrossPriceInd,"1") == 0)
            return UNCROSS_INDICATIVE;
        if (strcmp (uncrossPriceInd,"2") == 0)
            return UNCROSS_FIRM;
        if (strcmp (uncrossPriceInd,"3") == 0)
            return UNCROSS_INSUFFICIENT_VOL;

        return UNCROSS_NONE;
    }

}
