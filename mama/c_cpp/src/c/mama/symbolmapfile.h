/* $Id: symbolmapfile.h,v 1.7.32.3 2011/08/10 14:53:28 nicholasmarriott Exp $
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

#ifndef MamaSymbolMapFileH__
#define MamaSymbolMapFileH__

#if defined(__cplusplus)
extern "C" {
#endif

/* *************************************************** */
/* Includes. */
/* *************************************************** */
#include "mama/mama.h"

/* *************************************************** */
/* Forward declarations. */
/* *************************************************** */

/* The symbol map file impl. */
typedef void* mamaSymbolMapFile;

/* *************************************************** */
/* Public Function Prototypes. */
/* *************************************************** */

/**
 * Add additional individual symbology mapping.
 *
 * @param fileMap       Handle to the file-map to destroy.
 * @param fromSymbol    The from symbol.
 * @param toSymbol      The to symbol.
 */
MAMAExpDLL
extern void
mamaSymbolMapFile_addMap (
    mamaSymbolMapFile  fileMap,
    const char*        fromSymbol,
    const char*        toSymbol);

/**
 * Create a file-based symbol mapper.  Use mamaSymbolMapFile_load() to
 * open the file.  If the file exists, its contents should be a two
 * columns of data, with the columns separated by white space.  A
 * matching symbol of the left column is mapped to the symbol in the
 * right column.  when performing the mapping, if the symbol does not
 * match anything in the file (or the file cannot be found), then the
 * original symbol is used (i.e., no mapping).
 *
 * @param fileMap   The initialized file-map handle.
 *
 * @return mama_status return value can be one of
 *          MAMA_STATUS_NOMEM
 *          MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSymbolMapFile_create (
    mamaSymbolMapFile*  fileMap);

/**
 * Destroy the file-based symbol mapper.
 *
 * @param fileMap   Handle to the file-map to destroy.
 *
 * @return mama_status return value can be one of
 *          MAMA_STATUS_INVALID_ARG
 *          MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSymbolMapFile_destroy (
    mamaSymbolMapFile   fileMap);

/**
 * (Re)Load the map for the file-based symbol mapper.
 *
 * @param fileMap   Handle to the file-map to destroy.
 * @param fileName  The name of the file containing symbols to map.
 *
 * @return mama_status return value can be one of
 *          MAMA_STATUS_INVALID_ARG
 *          MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSymbolMapFile_load(
    mamaSymbolMapFile   fileMap,
    const char*         fileName);

/**
 * Perform a mapping from the file-based symbol mapper.
 *
 * @param fileMap       Handle to the file-map.
 * @param result        Returned "toSymbol" (feed side).
 * @param symbol        "fromSymbol" (client side).
 * @param maxLen        Max buffer length of result.
 *
 * @return 1 if a symbol could be mapped else 0.
 */
MAMAExpDLL
extern int
mamaSymbolMapFile_map (
    mamaSymbolMapFile  fileMap,
    char*              result,
    const char*        symbol,
    size_t             maxLen);

/**
 * Perform a reverse mapping from the file-based symbol mapper.
 *
 * @param fileMap       Handle to the file-map.
 * @param result        Returned "fromSymbol" (client side).
 * @param symbol        "toSymbol" (feed side).
 * @param maxLen        Max buffer length of result.
 */
MAMAExpDLL
extern int
mamaSymbolMapFile_revMap (
    mamaSymbolMapFile  fileMap,
    char*              result,
    const char*        symbol,
    size_t             maxLen);


#if defined(__cplusplus)
}
#endif

#endif /* MamaSymbolMapFileH__ */

