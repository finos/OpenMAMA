/* $Id: playbackFileParser.c,v 1.8.22.4 2011/09/14 15:09:05 ianbell Exp $
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

static void
rightCopy (char* input, char* output, int start);
static
mama_bool_t isNumeric( const char* str );
static mama_status
mamaPlaybackFileParser_createOrSetMsg (mamaPlaybackFileParser fileParser);

static void
mamaPlaybackFileParser_saveMsgLength (mamaPlaybackFileParser fileParser,
                                      char* msgHeader);


static
mama_status mamaPlaybackFileParser_init (mamaPlaybackFileParser  fileParser);

/**
 * Internally sets the  size of playback file to be parsed.
 * @param playbackFileParser object
 * @param filename - this could be either a relative or absolute,
 * filename with path.
 * @return mama_status
 */
static void
mamaPlaybackFileParser_setSize (mamaPlaybackFileParser fileParser,
                                char* fileName);

/****************************************************************************
Utility functions
****************************************************************************/
static void rightCopy (char* input, char* output, int start)
{
    int index  = 0;
    int i      = 0;
    int length = strlen (input);
    for (i = start; i<length; i++)
    {
        output[index]=input[i];
        index++;
    }
    output[index] =0;
}

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
    impl->myLastPos     = 0;
    impl->myFiledata    = NULL;
    impl->myFilePointer = NULL;
    impl->myMamaMsgLen  = 0;
    impl->myLastMsgLen  = 0;
    impl->myMsgBuffer   = NULL;
    return MAMA_STATUS_OK;
}

mama_status
mamaPlaybackFileParser_allocate (mamaPlaybackFileParser* fileParser)
{
    mama_status status = MAMA_STATUS_OK;
    mamaPlaybackFileParserImpl* impl =
        (mamaPlaybackFileParserImpl*)
        calloc (1, sizeof(mamaPlaybackFileParserImpl));

    if (impl == NULL) return MAMA_STATUS_NOMEM;
	mamaMsg_create(&impl->myMamaMsg);

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
    free (impl);
    impl = NULL;
    return MAMA_STATUS_OK;
}

mama_status
mamaPlaybackFileParser_openFile (mamaPlaybackFileParser fileParser,
                                 char* fileName)
{
    mama_status status = MAMA_STATUS_OK;
    
    mamaPlaybackFileParserImpl* impl =
        (mamaPlaybackFileParserImpl*)fileParser;
    if (impl == NULL)  return MAMA_STATUS_NULL_ARG;

    if (fileName != NULL)
    {
        mama_log (MAMA_LOG_LEVEL_FINE,
                  "openFile: checking for file: %s", fileName);
        mamaPlaybackFileParser_setSize (impl,fileName);

        if ((impl->myFileDescriptor = (open (fileName,
                                             /*O_RDWR*/O_RDONLY | O_NONBLOCK,
                                             0))) >= 0)
        {
            impl->myFiledata = mmap (NULL, impl->myFileSize,
                                     PROT_READ /*| PROT_WRITE*/,
                                     MAP_SHARED,
                                     impl->myFileDescriptor,
                                     0);
            if (!impl->myFiledata)
            {
                mama_log(MAMA_LOG_LEVEL_NORMAL,
                         "memory mapping failed?\n");
                exit(1);
            }
            impl->myFilePointer= (char*)impl->myFiledata;
        }
        else
        {
            status = MAMA_STATUS_PLATFORM;
        }
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
    munmap(impl->myFiledata, impl->myFileSize);
    return MAMA_STATUS_OK;
}

void
mamaPlaybackFileParser_setSize (mamaPlaybackFileParser fileParser,
                                char* fileName)
{
    /*
      use stat system call to get the size of the given file.
    */
    struct stat fileInfo;
    int result = 0;
    mamaPlaybackFileParserImpl* impl =
        (mamaPlaybackFileParserImpl*)fileParser;
    if (impl == NULL)
    {
        mama_log (MAMA_LOG_LEVEL_FINE,
                  "setSize : Null pointer exception !!!!!!\n!");
        return;
    }
    result = stat(fileName, &fileInfo);
    if (result)
    {
        mama_log (MAMA_LOG_LEVEL_NORMAL,
                  "File - %s does not exist or access is denied.\n",
                  fileName);
        exit(1);
    }
    impl->myFileSize = fileInfo.st_size;
}

mama_bool_t
mamaPlaybackFileParser_getNextHeader (mamaPlaybackFileParser fileParser,
                                      char** header)
{
     mamaPlaybackFileParserImpl* impl =
         (mamaPlaybackFileParserImpl*)fileParser;
     int   result = TRUE;
     char  delim  = (char)DELIMETER;
     int   i      = 0;
     if (impl == NULL)
     {
         mama_log (MAMA_LOG_LEVEL_FINE,
                   "getNextHeader: Null Pointer Exception !!!!!\n");
         result = FALSE;
         return result;
     }

     /*clear contents*/
     memset (impl->myBlockHeader,0,HEADER_SIZE);

     impl->myFilePointer = (char*)impl->myFiledata+
          impl->myLastPos;

     if (impl->myLastPos >= impl->myFileSize )
     {
         mama_log (MAMA_LOG_LEVEL_FINEST,
                   "End-of-file");
         return FALSE;
     }

     while (*impl->myFilePointer != delim)
     {
        impl->myBlockHeader[i]= *impl->myFilePointer;
        ++impl->myFilePointer;
        i++;
     }

     impl->myLastPos += i;
     impl->myBlockHeader[i++] = '\0';

     mamaPlaybackFileParser_saveMsgLength (impl,
                                           impl->myBlockHeader);
     *header = impl->myBlockHeader;

     return result;
}
mama_bool_t
mamaPlaybackFileParser_getNextMsg (mamaPlaybackFileParser fileParser,
                                   mamaMsg* msg)
{
	long i, counter, msgEnd ;

    mamaPlaybackFileParserImpl* impl =
        (mamaPlaybackFileParserImpl*)fileParser;
    i = counter = msgEnd = 0;

    if (impl->myMsgBuffer == NULL)
    {
        impl->myLastMsgLen = impl->myMamaMsgLen;
        impl->myMsgBuffer = (char*) calloc (impl->myMamaMsgLen
                                            , sizeof(char));
    }
    else
    {
        if (impl->myLastMsgLen < impl->myMamaMsgLen)
        {
            impl->myMsgBuffer = (char*)
                realloc ((char*)impl->myMsgBuffer, impl->myMamaMsgLen);
        }
        impl->myLastMsgLen = impl->myMamaMsgLen;
    }
    impl->myFilePointer = (char*)impl->myFiledata+
                           impl->myLastPos + 1 ;
    /*moved filepointer past the GS delim*/

    if (impl->myLastPos >= impl->myFileSize )
        return -1; /*end of file*/

    msgEnd = impl->myLastPos + impl->myMamaMsgLen;

    for (i = impl->myLastPos ; i < msgEnd ; i++)
    {
        impl->myMsgBuffer[counter] = *impl->myFilePointer;
        ++impl->myFilePointer;
        counter++;
    }

    impl->myLastPos += impl->myMamaMsgLen + 1;
     /* moved forward to  begining of next header*/

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

    return impl->myLastPos >= impl->myFileSize;
}

mama_status
mamaPlaybackFileParser_rewindFile (mamaPlaybackFileParser fileParser)
{
    mamaPlaybackFileParserImpl* impl;

    if (fileParser == NULL)
        return MAMA_STATUS_NULL_ARG;
    impl = (mamaPlaybackFileParserImpl*)fileParser;

    impl->myFilePointer = (char*)impl->myFiledata;
    impl->myLastPos = 0;

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
    int   nPos;
    char  buffer      [HEADER_SIZE];
    int   msgLength   = 0;

    temp = strrchr (msgHeader, delim);
    if (temp == NULL)
    {
        mama_log (MAMA_LOG_LEVEL_FINE,
                  "delimeter [:] not found in header\n");
        return;
    }
    nPos = temp - msgHeader;
    memset(buffer, 0,HEADER_SIZE);
    rightCopy (msgHeader, buffer, nPos+1);
    if (isNumeric (buffer))
    {
        msgLength  = atol (buffer);
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
