/* $Id: quality.c,v 1.4.24.2 2011/08/10 14:53:25 nicholasmarriott Exp $
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

#include "mama/quality.h"
#include <string.h>

mamaQuality
mamaQuality_convertFromString (const char*  str)
{
    if ((strcmp (str, "OK") == 0) ||
        (strcmp (str, "ok") == 0))
        return MAMA_QUALITY_OK;
    if ((strcmp (str, "STALE") == 0) ||
        (strcmp (str, "stale") == 0))
        return MAMA_QUALITY_STALE;
    if ((strcmp (str, "MAYBE_STALE") == 0) ||
        (strcmp (str, "maybe_stale") == 0))
        return MAMA_QUALITY_MAYBE_STALE;
    if ((strcmp (str, "PARTIAL_STALE") == 0) ||
        (strcmp (str, "partial_stale") == 0))
        return MAMA_QUALITY_PARTIAL_STALE;
    if ((strcmp (str, "DUPLICATE") == 0) ||
        (strcmp (str, "duplicate") == 0))
        return MAMA_QUALITY_DUPLICATE;
    return MAMA_QUALITY_UNKNOWN;
}

const char*
mamaQuality_convertToString (mamaQuality  quality)
{
    switch (quality)
    {
    case MAMA_QUALITY_OK:
        return "OK";
    case MAMA_QUALITY_MAYBE_STALE:
        return "MAYBE_STALE";
    case MAMA_QUALITY_STALE:
        return "STALE";
    case MAMA_QUALITY_PARTIAL_STALE:
        return "PARTIAL_STALE";
    case MAMA_QUALITY_DUPLICATE:
        return "DUPLICATE";
    default:
        return "UNKNOWN";
    }
}
