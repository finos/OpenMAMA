/* $Id: symbolmapfile.c,v 1.12.24.6 2011/09/23 10:12:52 emmapollock Exp $
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

/* *************************************************** */
/* Includes. */
/* *************************************************** */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "mama/symbolmapfile.h"
#include "wombat/wtable.h"
#include "wombat/wincompat.h"
#include "fileutils.h"

/* *************************************************** */
/* Definitions. */
/* *************************************************** */
#define SYMBOLMAPFILE_PARSE_ERROR 1
#define SYMBOLMAPFILE_PARSE_OK 0
#define SYMBOLMAPFILE_BUFF_SIZE 1024

/* *************************************************** */
/* Type Definitions. */
/* *************************************************** */
/* *************************************************** */
/* Structures. */
/* *************************************************** */

typedef struct mamaSymbolMapFileImpl_
{
    wtable_t  mTable;
    wtable_t  mRevTable;

} mamaSymbolMapFileImpl;

typedef struct  parseData_
{
    int symbologyCnt;
    int mapCnt;
    int toSymbolCnt;
    int fromSymbolCnt;
    char* fromSymbol;
    char* toSymbol;
    mamaSymbolMapFileImpl* impl;
    int error;
} parseData;

/* *************************************************** */
/* Private Function Prototypes. */
/* *************************************************** */

void symbolMapFileImpl_findSymbolMap(char *buffer, const char **fromSymbol, const char **toSymbol);
mama_status symbolMapFileImpl_loadTxt (mamaSymbolMapFile fileMap, const char *fileName, FILE *file);

/* *************************************************** */
/* Private Functions. */
/* *************************************************** */
void symbolMapFileImpl_findSymbolMap(char *buffer, const char **fromSymbol, const char **toSymbol)
{
    char* c = buffer;
    *fromSymbol = NULL;
    *toSymbol   = NULL;

    /* Skip leading whitespace. */
    while (*c && isspace(*c))
    {
        c++;
    }
    *fromSymbol = c;

    /* Skip over non-whitespace. */
    while (*c && !isspace(*c))
    {
        c++;
    }
    if (*c)
    {
        /* Found whitespace separator.  Terminate the "from" string
         * and skip until non-whitespace. */
        *c = '\0';
        c++;
        while (*c && isspace(*c))
        {
            c++;
        }
        if (*c)
        {
            *toSymbol = c;
        }
    }
}

mama_status symbolMapFileImpl_loadTxt(mamaSymbolMapFile fileMap, const char *fileName, FILE *file)
{
    char         line [256];
    const char*  fromSymbol = NULL;
    const char*  toSymbol   = NULL;

    mamaSymbolMapFileImpl* impl = (mamaSymbolMapFileImpl*) fileMap;
    if (!impl || !fileName || !fileName[0])
        return MAMA_STATUS_INVALID_ARG;

    if (impl->mTable)
    {
        wtable_free_all (impl->mTable);
    }
    else
    {
    }
    if (impl->mRevTable)
    {
        wtable_free_all (impl->mRevTable);
    }
    else
    {
    }

    while (fgets(line, 256, file))
    {
        size_t  len = stripTrailingWhiteSpace (line);
        if (len > 0)
        {
            symbolMapFileImpl_findSymbolMap (line, &fromSymbol, &toSymbol);
            if (fromSymbol && toSymbol)
            {
                mama_log (MAMA_LOG_LEVEL_FINER,
                          "mamaSymbolMapFile: adding map: %s->%s",
                          fromSymbol, toSymbol);
                wtable_insert (impl->mTable, fromSymbol, strdup(toSymbol));
                wtable_insert (impl->mRevTable, toSymbol, strdup(fromSymbol));
            }
        }
    }

    return MAMA_STATUS_OK;
}

/* *************************************************** */
/* Public Functions. */
/* *************************************************** */

void mamaSymbolMapFile_addMap(mamaSymbolMapFile fileMap, const char *fromSymbol, const char *toSymbol)
{
    mamaSymbolMapFileImpl* impl = (mamaSymbolMapFileImpl*) fileMap;

    if (!impl)
        return;

    wtable_insert (impl->mTable, strdup(fromSymbol), strdup(toSymbol));
    wtable_insert (impl->mRevTable, strdup(toSymbol), strdup(fromSymbol));
}

mama_status mamaSymbolMapFile_create(mamaSymbolMapFile *fileMap)
{
    mamaSymbolMapFileImpl* impl =
        (mamaSymbolMapFileImpl*) calloc (1, sizeof(mamaSymbolMapFileImpl));
    if (!impl)
    {
        *fileMap = NULL;
        return MAMA_STATUS_NOMEM;
    }
    *fileMap = impl;
    impl->mTable    = wtable_create ("symbolMap", 10000);
    impl->mRevTable = wtable_create ("symbolRevMap", 10000);
    return MAMA_STATUS_OK;
}

mama_status mamaSymbolMapFile_destroy(mamaSymbolMapFile fileMap)
{
    mamaSymbolMapFileImpl* impl = (mamaSymbolMapFileImpl*) fileMap;
    if (!impl)
        return MAMA_STATUS_INVALID_ARG;

    if (impl->mTable) wtable_destroy (impl->mTable);
    if (impl->mRevTable) wtable_destroy (impl->mRevTable);

    free (impl);
    return MAMA_STATUS_OK;
}

mama_status mamaSymbolMapFile_load(mamaSymbolMapFile fileMap, const char *fileName)
{
    FILE*        f          = NULL;
    mama_status  rval       = MAMA_STATUS_OK;

    rval = openWombatFile(fileName, "r", &f);
    if (rval == MAMA_STATUS_OK)
    {
        rval = symbolMapFileImpl_loadTxt (fileMap, fileName, f);
    }
    return rval;
}

int mamaSymbolMapFile_map(mamaSymbolMapFile fileMap, char *result, const char *symbol, size_t maxLen)
{
    int rtnVal = 0;
    const char* toSymbol = NULL;
    mamaSymbolMapFileImpl* impl = (mamaSymbolMapFileImpl*) fileMap;
    if (!impl)
        return rtnVal;

    toSymbol = (const char*) wtable_lookup (impl->mTable, symbol);
    if (toSymbol)
    {
        rtnVal = 1;
        strncpy (result, toSymbol, maxLen);
    }
    else
    {
        strncpy (result, symbol, maxLen);
    }
    result[maxLen-1] = '\0';   /* Just in case strncpy hit max! */
    mama_log (MAMA_LOG_LEVEL_FINER,
              "mamaSymbolMapFile: applied map: %s->%s",
              symbol, result);

    return rtnVal;
}

int mamaSymbolMapFile_revMap(mamaSymbolMapFile  fileMap, char *result, const char *symbol, size_t maxLen)
{
    int rtnVal = 0;
    const char* fromSymbol = NULL;
    mamaSymbolMapFileImpl* impl = (mamaSymbolMapFileImpl*) fileMap;
    if (!impl)
        return rtnVal;

    fromSymbol = (const char*) wtable_lookup (impl->mRevTable, symbol);

    if (fromSymbol)
    {
        rtnVal = 1;
        strncpy (result, fromSymbol, maxLen);
    }
    else
    {
        strncpy (result, symbol, maxLen);
    }
    result[maxLen-1] = '\0';   /* Just in case strncpy hit max! */
    mama_log (MAMA_LOG_LEVEL_FINER,
              "mamaSymbolMapFile: applied map: %s<-%s",
              result, symbol);

    return rtnVal;
}

