/* $Id: playbackpublisher.h,v 1.14.32.3 2011/09/01 16:34:38 emmapollock Exp $
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

#ifndef _PLAYBACK_PUBLISHER__
#define _PLAYBACK_PUBLISHER__


#if defined(__cplusplus)
extern "C" {
#endif

#include <mama/mama.h>
#ifndef __APPLE__
#include <malloc.h>
#endif
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <list.h>
#include <lookup2.h>
#include <wombat/wtable.h>
#include "playbackFileParser.h"
#include "wombat/port.h"

#define BUFFER_SIZE 128
typedef struct mamaFilePlayback_
{
    char          mySymbol[BUFFER_SIZE];
    char          mySource[BUFFER_SIZE];
    char          myTransportName[BUFFER_SIZE];
    wtable_t      myPublisherTable;
    wtable_t      myTransportTable;
    mamaMsg       myNewMessage;
    long          myNumMsg;
    long          myNumMsgLast;
    mamaPlaybackFileParser  myFileParser;
    int           myTportSleep;
    mamaBridge    myBridge;
}mamaFilePlaybackImpl;


typedef void*  mamaPlaybackPublisher;

/**
 * Method:  mamaplayback_destroyPublishers
 * @param:  mamaPlaybackPublisher
 * @Desc :  Destroys publishers allocated during msg publishing.
 * @return: status
 */
MAMAExpDLL
extern mama_status mamaPlayback_destroyPublishers (mamaPlaybackPublisher mamaPlayback);

/**
 * Method:  mamaplayback_destroyTransports
 * @param:  mamaPlaybackPublisher
 * @Desc :  Destroys transports allocated and used for msg publishing.
 * @return: status
 */
MAMAExpDLL
extern mama_status mamaPlayback_destroyTransports (mamaPlaybackPublisher mamaPlayback);

/**
 * Method: mamaplayback_publishMsg
 * @param: mamaPlaybackPublisher
 * @param: mamaPublisher
 * @desc : Publishes messages recreated from bytebuffer.
 * return: status
 */
MAMAExpDLL
extern mama_status mamaPlayback_publishMsg (mamaPlaybackPublisher mamaPlayback,
                                     mamaPublisher publisher);
/**
 * Method:  mamaplayback_publishFromFile
 * @param:  mamaPlaybackPublisher
 * @param:  delim
 * @param:  mamaDateTime
 * @desc :  publishes from the playback file specified . 
 *          The file should be in binary format [header:128 bytes]:[msglength]
 *          mamaMsg. Each mamaMsg is terminated by a newline. 
 *          Reading of file is done sequentially based on the sum of header 
 *          length and msglength.
 * @return: mama_bool_t
 */
MAMAExpDLL
extern mama_bool_t mamaPlayback_publishFromFile (mamaPlaybackPublisher mamaPlayback,
                                          const char delim,
                                          mamaDateTime dateTime);
/**
 * Method:  mamaplayback_allocate
 * @param:  mamaPlaybackPublisher - pointer to mamaPlaybackPublisher
 * @desc :  Creates the hash tables that are used for holding 
 *          previously allocated publishers and transports.
 * @return: status
 */
MAMAExpDLL
extern mama_status mamaPlayback_allocate (mamaPlaybackPublisher* mamaPlayback);

/**
 * Method:  mamaplayback_deallocate
 * @param:  mamaPlaybackPublisher
 * @desc :  Destroys the hash tables that were used for holding
 *          previously allocated publishers and transports.
 * @return: status
 */
MAMAExpDLL
extern mama_status mamaPlayback_deallocate (mamaPlaybackPublisher mamaPlayback);

/**
 * Method:  mamaplayback_findOrCreatePublisher
 * @param:  mamaPlaybackPublisher
 * @param:  keyName
 * @param:  mamaPublisher
 * @desc :  Each allocated publisher is held on a hash table, whose
 *          key values are hashed topics. Publishers
 *          are searched for using the key values.
 * @return: mama_bool_t
 */

MAMAExpDLL
extern mama_bool_t 
amaPlayback_findOrCreatePublisher (mamaPlaybackPublisher mamaPlayback,
                                   char* KeyName,
                                   mamaPublisher* publisher,
                                   const char delim);
/**
 * Method:  mamaplayback_findOrCreateTransport
 * @param:  mamaPlaybackPublisher
 * @param:  transportName
 * @param:  mamaTransport
 * @desc :  Each allocated transport is held on a hash table, whose
            key values are hashed topics. Transports
            are searched for using the key values.
 * @return: mama_bool_t
 */

/**
 * Method:  mamaPlayback_setTransportSleep
 * @desc :  Sets the delay to sleep after creating transport.
 * @param:  mamaPlaybackPublisher
 * @param:  tportSleep
 */
MAMAExpDLL
extern mama_status
mamaPlayback_setTransportSleep (mamaPlaybackPublisher mamaPlayback,
                                const int tportSleep);

MAMAExpDLL
extern mama_bool_t 
mamaPlayback_findOrCreateTransport (mamaPlaybackPublisher mamaPlayback,
                                    char* transportName,
                                    mamaTransport* transport);
/**
 * Method:  mamaplayback_getSymbol
 * @param:  mamaPlaybackPublisher
 * @param:  symbol
 * @desc :  Returns the symbol for a given mamaplaybackHeader
 * @return: status
 */

MAMAExpDLL
extern mama_status mamaPlayback_getSymbol (mamaPlaybackPublisher mamaPlayback,
                                    const char** symbol);
/**
 * Method:  mamaplayback_getSource
 * @param:  mamaPlaybackPublisher
 * @param:  source
 * @desc :  Returns the source for a given mamaplaybackHeader
 * @return: status
 */
MAMAExpDLL
extern mama_status mamaPlayback_getSource (mamaPlaybackPublisher mamaPlayback,
                                    const char** source);
/**
 * Method:  mamaplayback_getTransportName
 * @param:  mamaPlaybackPublisher
 * @param:  transportName
 * @desc :  Returns the transport name for a given mamaplaybackHeader
 * @return: mama_status
 */
MAMAExpDLL
extern mama_status mamaPlayback_getTransportName (mamaPlaybackPublisher mamaPlayback,
                                           const char** transportName);

/**
 * Method:  mamaPlayback_openFile
 * @param:  mamaPlaybackPublisher
 * @param:  fileName
 * @desc :  opens the playback file.
 * @return: status
 */ 
MAMAExpDLL
extern mama_status mamaPlayback_openFile (mamaPlaybackPublisher mamaPlayback,
                                     const char* fileName);

/**
 * Method:  mamaplayback_closeFile
 * @param:  mamaPlaybackPublisher
 * @desc :  Closes the playback file if open.
 * @return: mama_status
 */
MAMAExpDLL
extern mama_status mamaPlayback_closeFile (mamaPlaybackPublisher mamaPlayback);

/**
 * Method:  mamaPlayback_isEndOfFile
 * @param:  mamaPlaybackPublisher
 * @desc :  Is the playback file finished?
 * @return: mama_bool_t
 */
MAMAExpDLL
extern mama_bool_t mamaPlayback_isEndOfFile (mamaPlaybackPublisher mamaPlayback);

/**
 * Method:  mamaPlayback_rewindFile
 * @param:  mamaPlaybackPublisher
 * @desc :  Returns the playback file to the beginning.
 * @return: mama_status
 */
MAMAExpDLL
extern mama_status mamaPlayback_rewindFile (mamaPlaybackPublisher mamaPlayback);

/**
 * Method:  mamaplayback_createPublisher
 * @desc :  Creates a mamaPublisher for the topic.
 * @param:  mamaPlaybackPublisher
 * @param:  mamaTransport
 * @param:  mamaPublisher
 * @param:  topic
 * @return: mama_status
 */

MAMAExpDLL
extern mama_status mamaPlayback_createPublisher (mamaPlaybackPublisher mamaPlayback,
                                          mamaTransport* transport,
                                          mamaPublisher* publisher,
                                          char* topic);

/**
 * Method: mamaplayback_createTransport
 * @desc : Creates a mamaTransport 
 * @param: mamaPlaybackPublisher
 * @param: mamaTransport
 * @param: transportName
 * @return mama_status
 */
MAMAExpDLL
extern mama_status mamaPlayback_createTransport (mamaPlaybackPublisher mamaPlayback,
                                          mamaTransport* transport,
                                          char* transportName);
/**
 * Method:  printReport
 * @param:  mamaTimer
 * @param:  closure
 * @desc :  Prints report for publishing rates. 
 *          The interval for report printing
 *          is 1sec, but can be changed to any value > 1 sec 
 *          through command parameters at application start.
 *
 */
MAMAExpDLL
extern void  mamaPlayback_printReport (mamaTimer timer , void* closure);

/**
 * Method:  printConnections
 * @param:  mamaPlaybackPublisher
 * @desc :  Prints report for publishing connections.
 */
MAMAExpDLL
extern void  mamaPlayback_printConnections (mamaPlaybackPublisher mamaPlayback);


/**
 * Method: mamaCaptuer_setBridge
 * @param: mamaPlaybackPublisher
 * @param: mamaBridge
 * @Desc:  Sets the bridge to use for publishing on
 * @return mama_status
 */
MAMAExpDLL
extern mama_status mamaPlayback_setBridge (mamaPlaybackPublisher mamaPlayback, 
                                           mamaBridge bridge);

#if defined(__cplusplus)
}
#endif

#endif


