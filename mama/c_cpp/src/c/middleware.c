/* $Id: middleware.c,v 1.2.32.4 2011/08/22 16:28:26 emmapollock Exp $
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

#include "wombat/port.h"
#include <mama/middleware.h>
#include <string.h>

mamaMiddleware
mamaMiddleware_convertFromString (const char*  str)
{
    if (!str)
        return MAMA_MIDDLEWARE_UNKNOWN;
    
    if (strcasecmp (str, "wmw") == 0)
        return MAMA_MIDDLEWARE_WMW;
        
    if (strcasecmp (str, "lbm") == 0)
        return MAMA_MIDDLEWARE_LBM;
        
    if (strcasecmp (str, "tibrv") == 0)
        return MAMA_MIDDLEWARE_TIBRV;

    if (strcasecmp (str, "avis") == 0)
        return MAMA_MIDDLEWARE_AVIS;


    return MAMA_MIDDLEWARE_UNKNOWN;
}

/* Returns lowercase for use in library and function names */
const char*
mamaMiddleware_convertToString (mamaMiddleware  middleware)
{
    switch (middleware)
    {
        case MAMA_MIDDLEWARE_WMW:
            return "wmw";
        case MAMA_MIDDLEWARE_LBM:
            return "lbm";
        case MAMA_MIDDLEWARE_TIBRV:
            return "tibrv";
        case MAMA_MIDDLEWARE_AVIS:
            return "AVIS";
        default:
            return "unknown";
    }
}
