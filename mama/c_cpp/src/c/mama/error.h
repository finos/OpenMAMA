/* $Id: error.h,v 1.4.24.5 2011/08/29 11:52:44 ianbell Exp $
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

#ifndef MamaErrorH__
#define MamaErrorH__

#include <mama/config.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef enum mamaError_
{
    MAMA_ERROR_DEFAULT         = 0,
    MAMA_ERROR_INITIAL_TIMEOUT = 1,
    MAMA_ERROR_RECAP_TIMEOUT   = 2,
    MAMA_ERROR_NOT_ENTITLED    = 3,
    MAMA_ERROR_NO_RESOLVER     = 4,
    MAMA_ERROR_UNKNOWN         = 999
} mamaError;


/**
 * Convert a string to a mamaError value.
 *
 * @param str  The str to convert.
 */
MAMAExpDLL
extern mamaError
mamaError_convertFromString (const char* str);

/**
 * Convert a mamaError value to a string.  Do no attempt to free the
 * string result.
 *
 * @param error  The mamaError to convert.
 */
MAMAExpDLL
extern const char*
mamaError_convertToString (mamaError error);


#if defined(__cplusplus)
}
#endif

#endif

