/*
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

#include <wombat/fileutils.h>
#include <wombat/environment.h>
#include <stdio.h>

int
fileUtils_findFileInPathList (char*         buffer,
                              size_t        bufferSize,
                              const char*   filename,
                              const char*   pathlist,
                              const char*   delim)
{
    char *token = NULL;
    char delimiters[16];

    if (NULL == pathlist)
    {
        return 0;
    }

    if (NULL == delim)
    {
        snprintf(delimiters, sizeof(delimiters), "%c", PATH_DELIM);
    }
    else
    {
        strncpy (delimiters, delim, sizeof(delimiters));
    }

    token = strtok ((char*) pathlist, delimiters);
    while (token != NULL) {
        /* Only consider non-zero-length strings */
        if (strlen(token) > 0)
        {
            FILE * file;
            /* Build absolute path */
            snprintf (buffer, bufferSize, "%s%s%s", token, PATHSEP, filename);
            file = fopen(buffer, "r");
            if (file) {
                /* Close this handle and let the caller open if it wants to */
                fclose(file);
                return 1;
            }
        }

        /* Move onto next element in path list */
        token = strtok (NULL, delimiters);
    }
    strncpy(buffer, "", bufferSize);
    return 0;
}
