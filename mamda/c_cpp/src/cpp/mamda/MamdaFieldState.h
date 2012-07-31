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

#ifndef MamdaFieldStateH
#define MamdaFieldStateH

#include <mamda/MamdaConfig.h>

namespace Wombat
{

    /**
     * An enumeration representing field state.
     * MODIFIED value indicates the field was updated in last tick,
     * NOT_MODIFIED value indicates that there was no change in the last tick,
     * NOT_INITIALISED value indicates that the field has never been updated.
     * 
     */
    enum MamdaFieldState
    {
        MODIFIED        = 2,
        NOT_MODIFIED    = 1,
        NOT_INITIALISED = 0
    };


    /**
     * Convert a MamdaFieldState to an appropriate, displayable
     * string.
     * 
     * @param mamdaFieldState The <code>MamdaFieldState</code> to stringify
     *
     * @return The stringified version of the <code>MamdaFieldState</code>
     */
    MAMDAExpDLL const char* toString (MamdaFieldState fieldState);

}

#endif // MamdaFieldStateH

