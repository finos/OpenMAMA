/* $Id: servicelevel.c,v 1.2.38.2 2011/08/10 14:53:25 nicholasmarriott Exp $
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

#include "mama/mama.h"
#include "mama/reservedfields.h"

mamaServiceLevel
mamaServiceLevel_getFromMsg (const mamaMsg msg)
{
    int32_t result = MAMA_SERVICE_LEVEL_UNKNOWN;

    mamaMsg_getI32 (msg,
                    MamaFieldServiceLevel.mName,
                    MamaFieldServiceLevel.mFid,
                    &result);

    return (mamaServiceLevel) result;
}

const char*
mamaServiceLevel_getFromMsgAsString (const mamaMsg msg)
{
    return mamaServiceLevel_toString (mamaServiceLevel_getFromMsg(msg));
}

const char*
mamaServiceLevel_toString (mamaServiceLevel type)
{
    switch (type)
    {
    case MAMA_SERVICE_LEVEL_REAL_TIME:
        return "real time";
    case MAMA_SERVICE_LEVEL_SNAPSHOT:
        return "snapshot";
    case MAMA_SERVICE_LEVEL_REPEATING_SNAPSHOT:
        return "repeating snapshot";
    case MAMA_SERVICE_LEVEL_CONFLATED:
        return "conflated";
    case MAMA_SERVICE_LEVEL_UNKNOWN:
        return "unknown";
    default:
        return "error";
    }
}
