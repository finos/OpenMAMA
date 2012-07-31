/* $Id: symbolmap.h,v 1.7.32.2 2011/08/10 14:53:28 nicholasmarriott Exp $
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

#ifndef MamaSymbolMapH__
#define MamaSymbolMapH__

/* *************************************************** */
/* Includes. */
/* *************************************************** */
#include <mama/types.h>

#if defined( __cplusplus )
extern "C"
{
#endif

/* *************************************************** */
/* Type Defines. */
/* *************************************************** */

/**
 * Prototype for symbol-mapping function.
 *
 * @param closure Caller supplied closure.
 * @param result  The resulting symbol after mapping.
 * @param symbol  The symbol to be mapped.
 * @param maxLen  The maximum length of the mapped symbol (i.e.,
 * "result"), including trailing NULL.
 *
 * @return Whether a mapping for the symbol was found.
 */
typedef int (MAMACALLTYPE *mamaSymbolMapFunc)(
	void *closure,
    char *result,
    const char *symbol,
    size_t maxLen); 

#if defined( __cplusplus )
}
#endif

#endif
