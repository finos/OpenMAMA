/* $Id: playbackFileParser.h,v 1.4.32.3 2011/09/01 16:34:38 emmapollock Exp $
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

#ifndef _PLAYBACK_FILE_PARSER__
#define _PLAYBACK_FILE_PARSER__


#if defined(__cplusplus)
extern "C" {
#endif

#include "wombat/port.h"

#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>

#include <sys/stat.h>
#include <mama/mama.h>

#define  DELIMETER   29
#define  HEADER_SIZE 128
#define  TRUE        1
#define  FALSE       0


typedef struct mamaPlaybackFileParser_
{
    char            myBlockHeader[HEADER_SIZE];
    mamaMsg         myMamaMsg;
    int             myFileDescriptor; 
    void*           myFiledata;
    off_t           myFileSize;
    char*           myFilePointer;
    int             myLastPos;
    mama_size_t     myMamaMsgLen;
    mama_size_t     myLastMsgLen;
    char*     myMsgBuffer;
}mamaPlaybackFileParserImpl;
/*************************************************************************
 * Declarations
**************************************************************************/
typedef void* mamaPlaybackFileParser;
/**
 * Opens the captured playback file for parsing.
 * @param playbackFileParser object
 * @param filename - this could be either a relative or absolute
                     filename with path.
 * @return mama_status
 */
MAMAExpDLL
extern mama_status
mamaPlaybackFileParser_openFile (mamaPlaybackFileParser fileParser,
                                 char* fileName);
/**
 * Closes the captured playback file that was previously opened.
 * @param playbackFileParser object
 * @return mama_status
 */
MAMAExpDLL
extern mama_status
mamaPlaybackFileParser_closeFile (mamaPlaybackFileParser fileParser);

/**
 * Returns the next header SOURCE:TPORT:SYMBOL:MSGLEN.
 * @param playbackFileParser object
 * @param header -  address of where the header should be put.
 *                  If success true is returned else false
 * @return mama_bool_t
 */
MAMAExpDLL
extern mama_bool_t
mamaPlaybackFileParser_getNextHeader (mamaPlaybackFileParser fileParser,
                                      char** header);
/**
 * Returns the next mamaMsg from the file
 * @param playbackFileParser object
 * @param msg -  address of where the msg should go.
 *               If success true is returned else false
 * @return mama_bool_t
 */
MAMAExpDLL
extern mama_bool_t
mamaPlaybackFileParser_getNextMsg (mamaPlaybackFileParser fileParser,
                                   mamaMsg* msg);

/**
 * Is the playback file finished
 * @param playbackFileParser object
 * @return mama_bool_t
 */
MAMAExpDLL
extern mama_bool_t
mamaPlaybackFileParser_isEndOfFile (mamaPlaybackFileParser fileParser);

/**
 * Returns the playback file to the beginning
 * @param mamaPlaybackFileParser object
 * @return mama_status
 */
MAMAExpDLL
extern mama_status
mamaPlaybackFileParser_rewindFile (mamaPlaybackFileParser fileParser);

/**
 * Allocate memory for an instance of the file parser
 * @param playbackFileParser
 * @return mama_status
 */
MAMAExpDLL
extern mama_status
mamaPlaybackFileParser_allocate (mamaPlaybackFileParser* fileParser);
/**
 * Dellocate the memory for an instance of the file parser
 * @param playbackFileParser
 * @return mama_status
 */
MAMAExpDLL
extern mama_status
mamaPlaybackFileParser_deallocate (mamaPlaybackFileParser fileParser);

#if defined(__cplusplus)
}
#endif

#endif
