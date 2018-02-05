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

#ifndef MamdaOptionTypesH
#define MamdaOptionTypesH

namespace Wombat
{

    /**
     * Enumeration for indicating whether an option contract is a put or a
     * call.
     */
    enum MamdaOptionPutCall
    {
        MAMDA_PUT_CALL_CALL    = 'C',
        MAMDA_PUT_CALL_PUT     = 'P',
        MAMDA_PUT_CALL_UNKNOWN = 'Z'
    };

    /**
     * Enumeration for indicating the style of an individual option contract.
     */
    enum MamdaOptionExerciseStyle
    {
        MAMDA_EXERCISE_STYLE_AMERICAN = 'A',
        MAMDA_EXERCISE_STYLE_EUROPEAN = 'E',
        MAMDA_EXERCISE_STYLE_CAPPED   = 'C',
        MAMDA_EXERCISE_STYLE_UNKNOWN  = 'Z'
    };

} // namespace


#endif // MamdaOptionTypesH
