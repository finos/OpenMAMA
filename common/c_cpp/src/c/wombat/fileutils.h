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

#ifndef WOMBAT_FILEUTILS_H__
#define WOMBAT_FILEUTILS_H__


/*************************************************************************
 * Includes
**************************************************************************/

#include <wombat/port.h>

#if defined (__cplusplus)
extern "C"
{
#endif


/*************************************************************************
 * Typedefs, structs, enums and globals
**************************************************************************/

/**
* This method searches through a string of delimited file paths searching
* for a file in any one of those paths. Common usages would be searching
* through LD_LIBRARY_PATH or PATH to look for values.
*
* @param buffer     The string buffer to populate with the results
* @param bufferSize The number of bytes in buffer
* @param filename   This will refer to the filename which we are looking for.
* @param pathlist   This is a delimited string representing each path which
*                   is to be considered whils searching for filename.
* @param delim      This is a delimiter to use when tokenizing the pathlist.
*                   If NULL is provided, it will be detected.
*
* @return 1 if match found otherwise returns 0.
*/
COMMONExpDLL
int
fileUtils_findFileInPathList (char*         buffer,
                              size_t        bufferSize,
                              const char*   filename,
                              const char*   pathlist,
                              const char*   delim);

#if defined (__cplusplus)
}
#endif

#endif /* WOMBAT_FILEUTILS_H__ */
