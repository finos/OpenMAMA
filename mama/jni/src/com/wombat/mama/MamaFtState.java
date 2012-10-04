/* $Id:
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

package com.wombat.mama;

public class MamaFtState
{
    public static final short   MAMA_FT_STATE_STANDBY  = 0;
    public static final short   MAMA_FT_STATE_ACTIVE   = 1;
    public static final short   MAMA_FT_STATE_UNKNOWN  = 99;

    public static String toString (final short ftState)
    {
        switch (ftState)
        {
            case MAMA_FT_STATE_STANDBY  :   return "FT_STATE_STANDBY";
            case MAMA_FT_STATE_ACTIVE   :   return "FT_STATE_ACTIVE";
        }

        return "FT_STATE_UNKNOWN";
    }
}
