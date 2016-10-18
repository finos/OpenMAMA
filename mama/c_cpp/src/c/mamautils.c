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

#include <mama/mama.h>
#include <mama/mamautils.h>
#include <wombat/strutils.h>

MAMAExpDLL
extern mama_status
mamaUtils_readSymbolFile (const char*  symbolFile,
                          const char** level1SymbolList,
                          mama_size_t  level1SymbolListSize,
                          mama_size_t* level1SymbolListCount,
                          const char** level2SymbolList,
                          mama_size_t  level2SymbolListSize,
                          mama_size_t* level2SymbolListCount)
{
    char        buf[MAMA_MAX_TOTAL_SYMBOL_LEN];
    FILE*       symFileHandle         = NULL;
    mama_size_t level1SymbolListIndex = 0;
    mama_size_t level2SymbolListIndex = 0;

    if (NULL == symbolFile || NULL == level1SymbolList)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (0 == strcmp(symbolFile, "") || 0 == level1SymbolListSize)
    {
        return MAMA_STATUS_INVALID_ARG;
    }

    symFileHandle = fopen (symbolFile, "r");
    if(symFileHandle == NULL)
    {
        return MAMA_STATUS_NOT_FOUND;
    }

    /* Always force last character in array to be terminator */
    buf[MAMA_MAX_TOTAL_SYMBOL_LEN-1] = '\0';

    /* Reset return array indices */
    while(!feof(symFileHandle))
    {
        /* Read line at a time (newlines are included, last char in array
           should remain untouched) */
        if(fgets(buf, MAMA_MAX_TOTAL_SYMBOL_LEN-1, symFileHandle))
        {
            size_t lineLength      = 0;
            char*  newlinePosition = 0;

            /* NULL-set any newline characters to trim them */
            if (NULL != (newlinePosition = strchr(buf, '\n')))
            {
                *newlinePosition = '\0';
            }
            if (NULL != (newlinePosition = strchr(buf, '\r')))
            {
                *newlinePosition = '\0';
            }

            lineLength = strlenEx(buf);
            if(lineLength > 0)
            {
                char* pipePosition = NULL;
                pipePosition = strchr(buf, '|');

                // If a pipe was found and receiver is expecting results
                if(pipePosition && level2SymbolList)
                {
                    *pipePosition = '\0';
                    level2SymbolList[level2SymbolListIndex++] = strdup (pipePosition+1);
                }
                else
                {
                    level1SymbolList[level1SymbolListIndex++] = strdup (buf);
                }
            }
        }
    }
    if (NULL != level1SymbolListCount)
    {
        *level1SymbolListCount = level1SymbolListIndex;
    }
    if (NULL != level2SymbolListCount)
    {
        *level2SymbolListCount = level2SymbolListIndex;
    }


    fclose (symFileHandle);
    return MAMA_STATUS_OK;
}

