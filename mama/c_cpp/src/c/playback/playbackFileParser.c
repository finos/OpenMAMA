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

#include "playbackFileParser.h"
#include "wombat/property.h"

#include <wombat/strutils.h>
#include <mama/mama.h>
#include <mamainternal.h>

#define     FILE_PARSER_CONFIG_USE_MMAP     "mama.playbackfileparser.use_mmap"

static
mama_bool_t isNumeric( const char* str );
static mama_status
mamaPlaybackFileParser_createOrSetMsg (mamaPlaybackFileParser fileParser);

static void
mamaPlaybackFileParser_saveMsgLength (mamaPlaybackFileParser fileParser,
                                      char* msgHeader);


static
mama_status mamaPlaybackFileParser_init (mamaPlaybackFileParser  fileParser);


/****************************************************************************
Utility functions
****************************************************************************/

mama_bool_t isNumeric( const char* str )
{
    char*       validChars     = "0123456789";
    mama_bool_t isNumber       = 1;
    char c;
    int         index          = 0;

    for (index = 0; isNumber && (index < strlen(str)) == 1; index++)
    {
        c = str[index];
        if (strchr (validChars,c) == NULL)
        {
            isNumber = 0;
        }
    }
    return isNumber;
}

/*******************************************************************
Implementation
********************************************************************/
mama_status mamaPlaybackFileParser_init (mamaPlaybackFileParser
                                         fileParser)
{
    mamaPlaybackFileParserImpl* impl =
        (mamaPlaybackFileParserImpl*)fileParser;
    if (impl == NULL)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    impl->myMamaMsgLen  = 0;
    impl->myMaxMsgLen   = 0;
    impl->myMsgBuffer   = NULL;
    impl->myMamaMsg     = NULL;
    return MAMA_STATUS_OK;
}

mama_status
mamaPlaybackFileParser_allocate (mamaPlaybackFileParser* fileParser)
{
    mama_status status = MAMA_STATUS_OK;
    fileParserStatus fileStatus = FILE_PARSER_STATUS_OK;
    mamaPlaybackFileParserImpl* impl =
        (mamaPlaybackFileParserImpl*)
        calloc (1, sizeof(mamaPlaybackFileParserImpl));

    if (impl == NULL) return MAMA_STATUS_NOMEM;

    fileStatus = fileParser_allocate (&impl->myFileReader);
    if (FILE_PARSER_STATUS_OK != fileStatus)
    {
        return MAMA_STATUS_NOMEM;
    }

    mamaPlaybackFileParser_init (impl);
    *fileParser = (mamaPlaybackFileParserImpl*)impl;
    return status;
}

mama_status
mamaPlaybackFileParser_deallocate (mamaPlaybackFileParser fileParser)
{
    mama_status status = MAMA_STATUS_OK;
    mamaPlaybackFileParserImpl* impl =
        (mamaPlaybackFileParserImpl*)fileParser;
    if (impl == NULL) return MAMA_STATUS_NULL_ARG;

    if (impl->myMsgBuffer != NULL)
    {
        free((char*)impl->myMsgBuffer);
        impl->myMsgBuffer = NULL;
    }
    if (impl->myMamaMsg != NULL)
    {
        status = mamaMsg_destroy (impl->myMamaMsg);

        if (status != MAMA_STATUS_OK)
        {
            printf ("Error destroying msg: %s\n",
                    mamaStatus_stringForStatus (status));
            exit (status);
        }
    }
    if (impl->myFileReader != NULL)
    {
        fileParser_destroy (impl->myFileReader);
    }
    free (impl);
    impl = NULL;
    return MAMA_STATUS_OK;
}

mama_status
mamaPlaybackFileParser_openFile (mamaPlaybackFileParser fileParser,
                                 char* fileName)
{
    mama_status status = MAMA_STATUS_OK;
    fileParserStatus fileStatus = FILE_PARSER_STATUS_OK;

    mamaPlaybackFileParserImpl* impl =
        (mamaPlaybackFileParserImpl*)fileParser;
    if (impl == NULL)  return MAMA_STATUS_NULL_ARG;
    if (impl->myFileReader == NULL) return MAMA_STATUS_INVALID_ARG;

    if (fileName != NULL)
    {
        const char* useMmap = properties_Get (mamaInternal_getProperties(),
            FILE_PARSER_CONFIG_USE_MMAP);
        fileParserType parserType = FILE_PARSER_TYPE_UNKNOWN;

        mama_log (MAMA_LOG_LEVEL_FINE,
                  "openFile: checking for file: %s", fileName);

        /* If use_mmap is specified and set to false */
        if (useMmap != NULL && 0 == strtobool (useMmap))
        {
            mama_log (MAMA_LOG_LEVEL_FINE,
                  "openFile: opening %s in file streaming mode", fileName);
            parserType = FILE_PARSER_TYPE_FILE_STREAM;
        }
        else
        {
            mama_log (MAMA_LOG_LEVEL_FINE,
                  "openFile: opening %s in mmap mode", fileName);
            parserType = FILE_PARSER_TYPE_MMAP;
        }

        fileStatus = fileParser_create (impl->myFileReader,
            parserType, fileName);
        if (fileStatus != FILE_PARSER_STATUS_OK)
        {
            mama_log (MAMA_LOG_LEVEL_ERROR,
                  "openFile: Failed to create file reader for file: %s (%s)",
                  fileName,
                  fileParser_stringForStatus(fileStatus));
            return MAMA_STATUS_PLATFORM;
        }
        impl->myFileSize = fileParser_getFileSize (impl->myFileReader);
    }
    else
    {
        status = MAMA_STATUS_NULL_ARG;
    }
    return status;
}
mama_status
mamaPlaybackFileParser_closeFile (mamaPlaybackFileParser fileParser)
{
    mamaPlaybackFileParserImpl* impl =
        (mamaPlaybackFileParserImpl*)fileParser;
    if (impl == NULL)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    fileParser_closeFile (impl->myFileReader);

    return MAMA_STATUS_OK;
}

mama_bool_t
mamaPlaybackFileParser_getNextHeader (mamaPlaybackFileParser fileParser,
                                      char** header)
{
     mamaPlaybackFileParserImpl* impl =
         (mamaPlaybackFileParserImpl*)fileParser;
     int   result           = TRUE;
     char  delim            = (char)DELIMETER;
     int   i                = 0;
     uint64_t bytesCopied   = 0;
     if (impl == NULL)
     {
         mama_log (MAMA_LOG_LEVEL_FINE,
                   "getNextHeader: Null Pointer Exception !!!!!\n");
         result = FALSE;
         return result;
     }

     /*clear contents*/
     memset (impl->myBlockHeader,0,HEADER_SIZE);

     if (mamaPlaybackFileParser_isEndOfFile (impl))
     {
         mama_log (MAMA_LOG_LEVEL_FINEST,
                   "End-of-file");
         return FALSE;
     }

     fileParser_readFileToBufferUntilCharacter (impl->myFileReader,
                            (void*) impl->myBlockHeader,
                            HEADER_SIZE,
                            delim,
                            &bytesCopied);

     impl->myBlockHeader[bytesCopied] = '\0';

     mamaPlaybackFileParser_saveMsgLength (impl,
                                           impl->myBlockHeader);
     *header = impl->myBlockHeader;

     return result;
}
mama_bool_t
mamaPlaybackFileParser_getNextMsg (mamaPlaybackFileParser fileParser,
                                   mamaMsg* msg)
{
    fileParserStatus fileStatus = FILE_PARSER_STATUS_OK;
    uint64_t bytesCopied = 0;
    mamaPlaybackFileParserImpl* impl =
        (mamaPlaybackFileParserImpl*)fileParser;

    if (impl->myMsgBuffer == NULL)
    {
        impl->myMaxMsgLen = impl->myMamaMsgLen;
        impl->myMsgBuffer = (char*) calloc (impl->myMamaMsgLen
                                            , sizeof(char));
    }
    else
    {
        if (impl->myMaxMsgLen < impl->myMamaMsgLen)
        {
            impl->myMsgBuffer = (char*)
                realloc ((char*)impl->myMsgBuffer, impl->myMamaMsgLen);
            impl->myMaxMsgLen = impl->myMamaMsgLen;
        }
    }

    if (mamaPlaybackFileParser_isEndOfFile (impl))
        return -1; /*end of file*/

    fileStatus = fileParser_readFileToBuffer (impl->myFileReader,
                            (void*) impl->myMsgBuffer,
                            impl->myMamaMsgLen,
                            &bytesCopied);

    if (MAMA_STATUS_NOMEM ==
        mamaPlaybackFileParser_createOrSetMsg (impl))
    {
        mama_log (MAMA_LOG_LEVEL_NORMAL,
                  "publishFromFile(): MamaMsg failed to " \
                  "create or set");
        return -1;
    }
    *msg = impl->myMamaMsg;
    return TRUE;
}

mama_bool_t
mamaPlaybackFileParser_isEndOfFile (mamaPlaybackFileParser fileParser)
{
    mamaPlaybackFileParserImpl* impl;

    if (fileParser == NULL)
        return FALSE;
    impl = (mamaPlaybackFileParserImpl*)fileParser;

    return (mama_bool_t) fileParser_isEndOfFile (impl->myFileReader);
}

mama_status
mamaPlaybackFileParser_rewindFile (mamaPlaybackFileParser fileParser)
{
    mamaPlaybackFileParserImpl* impl;

    if (fileParser == NULL)
        return MAMA_STATUS_NULL_ARG;
    impl = (mamaPlaybackFileParserImpl*)fileParser;

    fileParser_rewindFile (impl->myFileReader);

    return MAMA_STATUS_OK;
}

void mamaPlaybackFileParser_saveMsgLength (mamaPlaybackFileParser
                                           fileParser,
                                           char* msgHeader)
{
    mamaPlaybackFileParserImpl* impl =
        (mamaPlaybackFileParserImpl*)fileParser;
    char  delim       = (char)':';
    char* temp        = NULL;
    int   msgLength   = 0;

    temp = strrchr (msgHeader, delim);
    if (temp == NULL)
    {
        mama_log (MAMA_LOG_LEVEL_FINE,
                  "delimeter [:] not found in header\n");
        return;
    }
    else
    {
        /* Move the pointer past the delimiter */
        temp++;
    }

    if (isNumeric (temp))
    {
        msgLength  = atol (temp);
    }
    impl->myMamaMsgLen = msgLength;
}

mama_status
mamaPlaybackFileParser_createOrSetMsg (mamaPlaybackFileParser fileParser)
{
    mama_status status = MAMA_STATUS_OK;
    mamaPlaybackFileParserImpl* impl =
        (mamaPlaybackFileParserImpl*)fileParser;
    if (impl == NULL) return MAMA_STATUS_NULL_ARG;

    if (impl->myMamaMsg == NULL)
    {
        status =
            mamaMsg_createFromByteBuffer (&impl->myMamaMsg,
                                          impl->myMsgBuffer,
                                          impl->myMamaMsgLen);
        if (status != MAMA_STATUS_OK)
        {
            mama_logStdout (MAMA_LOG_LEVEL_NORMAL,
                            "createOrSetMsg():  mamaMsg " \
                            "createFromByte error.\n");
            return MAMA_STATUS_NOMEM;
        }
    }
    else
    {
        if (MAMA_STATUS_OK !=
            mamaMsg_setNewBuffer (impl->myMamaMsg,
                                  (void*)impl->myMsgBuffer,
                                  impl->myMamaMsgLen))
        {
            mama_logStdout (MAMA_LOG_LEVEL_NORMAL,
                            "createOrSetMsg(): mamaMsg " \
                            "setNewBuffer error.\n");
            return MAMA_STATUS_NOMEM;
        }
    }
    return status;
}
