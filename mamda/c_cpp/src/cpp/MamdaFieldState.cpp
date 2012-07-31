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

#include <mamda/MamdaFieldState.h>
#include <string.h>

namespace Wombat
{

    static const char* MODIFIED_STR           = "Modified";
    static const char* NOT_MODIFIED_STR       = "Not Modified";
    static const char* NOT_INITIALISED_STR    = "Not Initialised";

    const char* toString (MamdaFieldState fieldState)
    {
        switch (fieldState)
        {
        case MODIFIED:              return MODIFIED_STR;
        case NOT_MODIFIED:          return NOT_MODIFIED_STR;
        case NOT_INITIALISED:       return NOT_INITIALISED_STR;
        }
        return "Error!";
    }

}
