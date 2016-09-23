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

#ifndef MamaUtilsH__
#define MamaUtilsH__

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
 * Reads a symbol file and populates provided arrays with
 * strdup'd strings containing each topic found. Note that
 * the caller is responsible for freeing the memory of the
 * items returned and any arrays passed in should be NULL-
 * initialized.
 *
 * @param symbolFile The name of the symbol file to read in
 *     (format is expected to be a newline delimited ascii
 *     file with optional use of an additional pipe delimiter
 *     to parse level2 data (e.g. "level1sym|level2sym").
 * @param level1SymbolList Array of strings to populate with
 *     each symbol which is found.
 * @param level1SymbolListSize The number of array slots
 *     available to populate in level1SymbolList.
 * @param level1SymbolListCount If non-null, function will
 *     return the number of symbols inserted into
 *     level1SymbolList.
 * @param  level2SymbolList Array of strings to populate with
 *     each symbol which is found. If NULL is provided, no
 *     level2 parsing will be attempted.
 * @param level2SymbolListSize The number of array slots
 *     available to populate in level2SymbolList.
 * @param level2SymbolListCount  If non-null, function will
 *     return the number of symbols inserted into
 *     level2SymbolList.
 * @param maxLen  The maximum length of the mapped symbol (i.e.,
 * "result"), including trailing NULL.
 *
 * @return Statis code indicating success of failure
 */
MAMAExpDLL
extern mama_status
mamaUtils_readSymbolFile (const char*  symbolFile,
                          const char** level1SymbolList,
                          mama_size_t  level1SymbolListSize,
                          mama_size_t* level1SymbolListCount,
                          const char** level2SymbolList,
                          mama_size_t  level2SymbolListSize,
                          mama_size_t* level2SymbolListCount);

#if defined( __cplusplus )
}
#endif

#endif
