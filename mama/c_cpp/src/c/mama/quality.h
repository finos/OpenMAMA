/* $Id: quality.h,v 1.6.24.3 2011/08/10 14:53:28 nicholasmarriott Exp $
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

#ifndef MamaQualityH__
#define MamaQualityH__

#include "mama/config.h"


#if defined(__cplusplus)
extern "C" {
#endif

typedef enum mamaQuality_
{
    MAMA_QUALITY_OK             =  0,
    MAMA_QUALITY_MAYBE_STALE    =  1,
    MAMA_QUALITY_STALE          =  2,
    MAMA_QUALITY_PARTIAL_STALE  =  3,
    MAMA_QUALITY_FORCED_STALE   =  4,
    MAMA_QUALITY_DUPLICATE      =  5,
    MAMA_QUALITY_UNKNOWN        = 99
} mamaQuality;

/**
 * Convert a string to a mamaQuality value.
 *
 * @param str  The str to convert.
 */
MAMAExpDLL
extern mamaQuality
mamaQuality_convertFromString (const char*  str);

/**
 * Convert a mamaQuality value to a string.  Do no attempt to free the
 * string result.
 *
 * @param quality  The mamaQuality to convert.
 */
MAMAExpDLL
extern const char*
mamaQuality_convertToString (mamaQuality  quality);


#if defined(__cplusplus)
}
#endif

#endif
