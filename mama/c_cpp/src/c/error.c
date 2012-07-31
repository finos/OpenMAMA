/* $Id: error.c,v 1.4.24.5 2011/08/29 11:52:43 ianbell Exp $
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

#include <mama/error.h>
#include <string.h>

mamaError
mamaError_convertFromString (const char*  str)

{
    if ((strcmp (str, "DEFAULT") == 0) ||
        (strcmp (str, "default") == 0))
        return MAMA_ERROR_DEFAULT;

    if ((strcmp (str, "NO_RESOLVER") == 0) ||
        (strcmp (str, "no_resolver") == 0))
        return MAMA_ERROR_NO_RESOLVER;

    if ((strcmp (str, "INITIAL_TIMEOUT") == 0) ||
        (strcmp (str, "initial_timeout") == 0))
        return MAMA_ERROR_INITIAL_TIMEOUT;

    if ((strcmp (str, "RECAP_TIMEOUT") == 0) ||
        (strcmp (str, "recap_timeout") == 0))
        return MAMA_ERROR_RECAP_TIMEOUT;

    if ((strcmp (str, "NOT_ENTITLED") == 0) ||
        (strcmp (str, "not_entitled") == 0))
        return MAMA_ERROR_NOT_ENTITLED;

		return MAMA_ERROR_UNKNOWN;
}

const char*
mamaError_convertToString (mamaError  error)
{
    switch (error)
    {
    case MAMA_ERROR_DEFAULT:
        return "DEFAULT";
    case MAMA_ERROR_NO_RESOLVER:
        return "NO_RESOLVER";
    case MAMA_ERROR_INITIAL_TIMEOUT:
        return "INITIAL_TIMEOUT";
    case MAMA_ERROR_RECAP_TIMEOUT:
        return "RECAP_TIMEOUT";
    case MAMA_ERROR_NOT_ENTITLED:
        return "NOT_ENTITLED";
    default:
        return "UNKNOWN";
    }
}
