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

#ifndef MamdaUncrossPriceIndH
#define MamdaUncrossPriceIndH

#include <mamda/MamdaConfig.h>

namespace Wombat
{

    /**
     * An enumeration representing the uncross price Ind.
     */
    enum MamdaUncrossPriceInd
    {
        UNCROSS_NONE,
        UNCROSS_INDICATIVE,      
        UNCROSS_FIRM,      
        UNCROSS_INSUFFICIENT_VOL
    };

    /**
     * Convert a MamdaUncrossPriceInd to an appropriate, displayable
     * string.
     *
     * @param securityStatus The uncross price Ind as an enumerated type.
     */
    MAMDAExpDLL const char* toString (
        MamdaUncrossPriceInd  securityStatus);

    /**
     * Convert a string representation of a uncross price Ind to the
     * enumeration.  This function is used internally for compatibility
     * with older feed handler configurations, which may send the field as
     * a string.
     *
     * @param uncrossPriceInd The uncross price Ind as a string.
     *
     * @return The uncross price Ind as an enumerated type.
     */
    MAMDAExpDLL MamdaUncrossPriceInd mamdaUncrossPriceIndFromString (
        const char* uncrossPriceInd);

}

#endif // MamdaUncrossPriceIndH
