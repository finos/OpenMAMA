/* $Id: playbackpublisher.c,v 1.14.32.3 2011/09/01 16:34:38 emmapollock Exp $
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

#include <ctype.h>
#include "playbackpublisher.h"
#include <mama/conflation/connection.h>

#define FOUND_CREATE_PUBLISHER    1
#define FOUND_CREATE_TRANSPORT    1
#define BUCKET_NUMBER             983 

static void mamaPlayback_rightCopy (char* input, char* output, 
                                    int start);

static mama_bool_t 
mamaPlayback_addPublisher (mamaPlaybackPublisher mamaPlayback,
                           char* key,
                           mamaPublisher publisher);
static mama_bool_t 
mamaPlayback_addTransport (mamaPlaybackPublisher mamaPlayback,
                           char* key,
                           mamaTransport transport);

static void freeTransportCb (wtable_t table,void *data,
                             const char* key, void *closure )
{
    if (key != NULL)
    {
        mamaTransport tport =  (mamaTransport)data;
        if (tport != NULL)
        {
            mamaTransport_destroy (tport);
            mama_logStdout (MAMA_LOG_LEVEL_FINE,
                            "Destroyed transport value (%p)\n",
                            tport);
            tport = NULL;
        }
    }
}

static int
injectDateTime (mamaDateTime dateTime, mamaMsg msg);

static void freePublisherCb (wtable_t table,void *data, 
                             const char* key, void *closure )
{
    if (key != NULL)
    {
        mamaPublisher publisher =  (mamaPublisher)data;
        if (publisher != NULL)
        {
            mamaPublisher_destroy (publisher);
            mama_logStdout (MAMA_LOG_LEVEL_FINE,
                            "Destroyed publisher from table @:  %p\n",
                            publisher);
            publisher = NULL;
        }
    }
}

mama_status mamaPlayback_allocate (mamaPlaybackPublisher* mamaPlayback)
{
    mamaFilePlaybackImpl* impl =
        (mamaFilePlaybackImpl*) calloc (1, sizeof(mamaFilePlaybackImpl));
    if (impl == NULL)  return MAMA_STATUS_NOMEM;
    
    impl->myPublisherTable = wtable_create ("PublishMap", BUCKET_NUMBER);
    impl->myTransportTable = wtable_create ("TransportMap", BUCKET_NUMBER);
    
    mamaPlaybackFileParser_allocate (&impl->myFileParser);
    if (impl->myFileParser == NULL)
    {
        mama_log (MAMA_LOG_LEVEL_NORMAL,
                  "File opening error : No memory for allocation\n");
        return MAMA_STATUS_NOMEM;
    }
    
    *mamaPlayback = (mamaPlaybackPublisher*)impl;
    return MAMA_STATUS_OK;
}  

mama_status mamaPlayback_deallocate (mamaPlaybackPublisher mamaPlayback)
{
    mamaFilePlaybackImpl* impl = (mamaFilePlaybackImpl*)mamaPlayback;
    
    if (impl == NULL) return MAMA_STATUS_INVALID_ARG;
    /* destroy publishers & transports before freeing the hash tables*/

    mamaPlayback_destroyPublishers (impl);
    mamaPlayback_destroyTransports (impl);

    if (impl->myTransportTable != NULL)
    {
        wtable_destroy (impl->myTransportTable);
        impl->myTransportTable = NULL;
    }
    
    if (impl->myPublisherTable != NULL)
    {
        wtable_destroy (impl->myPublisherTable);
        impl->myPublisherTable = NULL;
    }

    if (impl->myFileParser != NULL)
    {
        mamaPlaybackFileParser_deallocate (impl->myFileParser);
    }
    if (impl != NULL)
    {
        free (impl);
        impl = NULL;
    }

    return MAMA_STATUS_OK;
}

mama_status mamaPlayback_createPublisher (mamaPlaybackPublisher mamaPlayback,
                                          mamaTransport* transport,
                                          mamaPublisher* publisher,
                                          char* topic)
{
    mama_status status;

    status = mamaPublisher_create (publisher,
                                   *transport,
                                   topic,
                                   NULL,   /* Not needed for basic publishers */
                                   NULL);  /* Not needed for basic publishers */
     mama_logStdout (MAMA_LOG_LEVEL_FINE,
                     "PLAYBACK INFO!!!, Publisher created (%p)\n", 
                     publisher);
    if (status != MAMA_STATUS_OK)
    {
        mama_logStdout (MAMA_LOG_LEVEL_NORMAL,
                        "An error occured creating publisher: %s\n",
                        mamaStatus_stringForStatus (status));
        exit (status);
    }

    return status;
}

mama_status mamaPlayback_createTransport (mamaPlaybackPublisher mamaPlayback,
                                          mamaTransport* transport,
                                          char* transportName)
{
    mama_status status;
    mamaFilePlaybackImpl* impl = (mamaFilePlaybackImpl*)mamaPlayback;

    if (impl == NULL)
        return MAMA_STATUS_NULL_ARG;
    
    status = mamaTransport_allocate (transport);
    if (status != MAMA_STATUS_OK)
    {
        mama_logStdout (MAMA_LOG_LEVEL_FINE,
                        "mamaPlayback_createTransport: Error allocating transport: %s",
                        mamaStatus_stringForStatus (status));
        exit (status);
    }
    status = mamaTransport_create (*transport,
                                   transportName,impl->myBridge);
    if (status != MAMA_STATUS_OK)
    {
        mama_logStdout (MAMA_LOG_LEVEL_FINE,
                        "mamaTransport_create: Error creating transport: %s",
                        mamaStatus_stringForStatus (status));
        exit (status);
    }
    return status;
}

mama_status
mamaPlayback_setTransportSleep (mamaPlaybackPublisher mamaPlayback,
                                const int tportSleep)
{
    mamaFilePlaybackImpl* impl;

    if (mamaPlayback == NULL)
        return MAMA_STATUS_NULL_ARG;
    impl = (mamaFilePlaybackImpl*)mamaPlayback;

    impl->myTportSleep = tportSleep;

    return MAMA_STATUS_OK;
}

mama_bool_t 
mamaPlayback_findOrCreateTransport (mamaPlaybackPublisher mamaPlayback,
                                    char* transportName,
                                    mamaTransport* transport)
{
    int result                  = 1;
    mamaTransport tempTransport = NULL;
    mamaFilePlaybackImpl* impl  = (mamaFilePlaybackImpl*)mamaPlayback;

    tempTransport = (mamaTransport)wtable_lookup
        (impl->myTransportTable , transportName);
  
    if (tempTransport != NULL)
    {
        *transport  = tempTransport;
        mama_logStdout (MAMA_LOG_LEVEL_FINEST,
                        "findOrCreateTransport(): Transport found (%p)\n",
                        *transport);
        return  FOUND_CREATE_TRANSPORT;
    }
    else
    {
        /*create a new transport*/
        mama_log (MAMA_LOG_LEVEL_FINEST,
                  "creating transport - %s",
                  transportName);
        mamaPlayback_createTransport (impl, transport,
                                      transportName);
        mama_log (MAMA_LOG_LEVEL_FINEST,
                  "transport created (%p)",
                  *transport);
        result = mamaPlayback_addTransport (impl,
                                            transportName,
                                            *transport);
        if (result != 1)
        {
            mama_logStdout (MAMA_LOG_LEVEL_FINE,
                            "findOrCreateTransport:" \
                            "wtable_table insert transport error");
            exit (0);
        }

        if (impl->myTportSleep)
        {
            mama_logStdout (MAMA_LOG_LEVEL_FINE,
                            "Sleeping for %d second(s)", impl->myTportSleep);
            sleep (impl->myTportSleep);
        }

        return FOUND_CREATE_TRANSPORT;
    }
    return -1;
}

mama_bool_t 
mamaPlayback_findOrCreatePublisher (mamaPlaybackPublisher mamaPlayback,
                                    char* keyName,
                                    mamaPublisher* publisher,
                                    const char delim)
{
    char*         temp         = NULL;
    int           nPos         = 0;
    char          topic          [BUFFER_SIZE];
    char          keyCopy        [BUFFER_SIZE];
    mamaTransport transport    = NULL;
    int           result       = 1;
    mamaPublisher tempPublisher= NULL;

    mamaFilePlaybackImpl* impl = (mamaFilePlaybackImpl*)mamaPlayback;
       
    tempPublisher = (mamaPublisher)wtable_lookup 
        (impl->myPublisherTable , keyName);
    
    if (tempPublisher != NULL)
    {
        *publisher = tempPublisher;
        mama_logStdout (MAMA_LOG_LEVEL_FINEST,
                        "findOrCreatePublisher(): Publisher found (%p)\n",
                        *publisher);
        return FOUND_CREATE_PUBLISHER;
    }
    else
    {
        strcpy (keyCopy, keyName);
        temp = strrchr (keyName,delim);
        nPos = temp - keyName;
        mamaPlayback_rightCopy (keyName,impl->mySymbol,nPos+1);
        keyName[nPos] = '\0';
        temp = strrchr (keyName,delim);
        nPos = temp - keyName;
        mamaPlayback_rightCopy (keyName,impl->myTransportName,nPos+1);
        keyName[nPos] = '\0';
        strcpy (impl->mySource,keyName);
        if (mamaPlayback_findOrCreateTransport (impl,impl->myTransportName,
                                                &transport) == -1)
        {
            /*Debug something wrong*/
            mama_log (MAMA_LOG_LEVEL_NORMAL,
                      "findOrCreatePublisher: Could not find or create" \
                      "transport");
            return -1;
        }

        sprintf(topic,"%s.%s", impl->mySource, impl->mySymbol); /**use fastCopyAndSlide*/
        mamaPlayback_createPublisher (impl, &transport,
                                      publisher,
                                      topic);
        result = mamaPlayback_addPublisher (impl, keyCopy,
                                            *publisher);
        if (result != 1)
        {
            mama_logStdout (MAMA_LOG_LEVEL_FINE,
                            "findOrCreatePublisher: publisher " \
                            "hash table insert error\n");
            exit (0);
        }
        return FOUND_CREATE_PUBLISHER;
    }
    return -1;
}

mama_status mamaPlayback_destroyPublishers (mamaPlaybackPublisher mamaPlayback)
{
    mamaFilePlaybackImpl* impl = (mamaFilePlaybackImpl*)mamaPlayback;
    if (impl == NULL) return MAMA_STATUS_NULL_ARG;

    wtable_clear_for_each (impl->myPublisherTable, 
                           freePublisherCb, NULL);
    return MAMA_STATUS_OK;
}

mama_status mamaPlayback_destroyTransports (mamaPlaybackPublisher mamaPlayback)
{
    mamaFilePlaybackImpl* impl = (mamaFilePlaybackImpl*)mamaPlayback;
    if (impl == NULL) return MAMA_STATUS_NULL_ARG;

    wtable_clear_for_each (impl->myTransportTable, 
                           freeTransportCb, NULL);
    return MAMA_STATUS_OK;
}

mama_status mamaPlayback_publishMsg (mamaPlaybackPublisher mamaPlayback,
                                     mamaPublisher publisher)
{
    mamaFilePlaybackImpl* impl = 
        (mamaFilePlaybackImpl*)mamaPlayback;
    if (impl == NULL) return MAMA_STATUS_NULL_ARG;
    
    if (publisher != NULL)
    {
        mamaPublisher_send (publisher, impl->myNewMessage);
        impl->myNumMsg++;
    }
    return MAMA_STATUS_OK;
}

mama_status mamaPlayback_openFile (mamaPlaybackPublisher mamaPlayback,
                                     const char* fileName)
{
    mamaFilePlaybackImpl* impl = (mamaFilePlaybackImpl*)mamaPlayback;
    
    if (impl == NULL) return MAMA_STATUS_NULL_ARG;
    
    if (fileName == NULL)
    {
        mama_log (MAMA_LOG_LEVEL_NORMAL,
                  "mamaPlayback_openFile: NULL ARGUEMENT EXCEPTION");
        exit (0);
    }
    if (mamaPlaybackFileParser_openFile (
            impl->myFileParser, (char*)fileName) == MAMA_STATUS_OK)
    {
        return MAMA_STATUS_OK;
    }
    return -1;
}


mama_status mamaPlayback_setBridge (mamaPlaybackPublisher mamaPlayback,
                                    mamaBridge            bridge)
{
    mamaFilePlaybackImpl* impl = (mamaFilePlaybackImpl*)mamaPlayback;
    
    if (impl == NULL) return MAMA_STATUS_NULL_ARG;
    impl->myBridge = bridge;
  
    return MAMA_STATUS_OK;
}

mama_bool_t mamaPlayback_publishFromFile (mamaPlaybackPublisher mamaPlayback,
                                          const char delim,
                                          mamaDateTime dateTime)
{
    mamaPublisher publisher = NULL;
    char*          header    = NULL;
    char*         temp      = NULL;
    int           position  = 0;   
    mamaFilePlaybackImpl* impl = (mamaFilePlaybackImpl*)mamaPlayback;
    if (impl == NULL) return MAMA_STATUS_NULL_ARG;
   
    if (mamaPlaybackFileParser_getNextHeader (impl->myFileParser,
                                              &header))
    {
        if (!mamaPlaybackFileParser_getNextMsg (impl->myFileParser,
                                                &impl->myNewMessage))
        {
            mama_log (MAMA_LOG_LEVEL_FINE,
                      "PublisheFromFile: getNextMsg Error - !!!!!!");
            return -1;
        }
        temp = strrchr (header,delim);
        if (temp != NULL)
        {
            position = temp -header;
        }
        header[position]='\0';
        if (dateTime)
        {
            injectDateTime (dateTime, impl->myNewMessage);
        }
        if (mamaPlayback_findOrCreatePublisher (impl,header, 
                                                &publisher, delim) != -1)
        { 
            mamaPlayback_publishMsg (impl, publisher);
        }
    }
    else
    {
        return -1;
    }
    
    return MAMA_STATUS_OK;
}

mama_status mamaPlayback_closeFile (mamaPlaybackPublisher mamaPlayback)
{
    mamaFilePlaybackImpl* impl = (mamaFilePlaybackImpl*)mamaPlayback;
    if (impl == NULL) return MAMA_STATUS_NULL_ARG;

    if (impl->myFileParser != NULL)
    {
        return mamaPlaybackFileParser_closeFile(impl->myFileParser);
    }
    return MAMA_STATUS_OK;
}

mama_bool_t mamaPlayback_isEndOfFile (mamaPlaybackPublisher mamaPlayback)
{
    mamaFilePlaybackImpl* impl;

    if (mamaPlayback == NULL)
        return FALSE;
    impl = (mamaFilePlaybackImpl*)mamaPlayback;

    return mamaPlaybackFileParser_isEndOfFile (impl->myFileParser);
}

mama_status mamaPlayback_rewindFile (mamaPlaybackPublisher mamaPlayback)
{
    mama_status           status;
    mamaFilePlaybackImpl* impl;

    if (mamaPlayback == NULL)
        return FALSE;
    impl = (mamaFilePlaybackImpl*)mamaPlayback;

    status = mamaPlaybackFileParser_rewindFile (impl->myFileParser);
    if (status == MAMA_STATUS_OK)
    {
        impl->myNumMsg = 0;
        impl->myNumMsgLast = 0;
    }

    return status;
}

mama_status mamaPlayback_getSymbol (mamaPlaybackPublisher mamaPlayback,
                                    const char** symbol)
{
    mamaFilePlaybackImpl* impl = (mamaFilePlaybackImpl*)mamaPlayback;
    if (impl == NULL) return MAMA_STATUS_NULL_ARG;
    
    *symbol = impl->mySymbol;
    return MAMA_STATUS_OK;
}
mama_status mamaPlayback_getSource (mamaPlaybackPublisher mamaPlayback,
                                    const char** source)
{
    mamaFilePlaybackImpl* impl = (mamaFilePlaybackImpl*)mamaPlayback;
    if (impl == NULL) return MAMA_STATUS_NULL_ARG;
    
    *source = impl->mySource;
    return MAMA_STATUS_OK;
}

mama_status mamaPlayback_getTransportName (mamaPlaybackPublisher mamaPlayback,
                                           const char** transportName)
{
    mamaFilePlaybackImpl* impl = (mamaFilePlaybackImpl*)mamaPlayback;
    if (impl == NULL) return MAMA_STATUS_NULL_ARG;
    
    *transportName = impl->myTransportName;
    return MAMA_STATUS_OK;
}

void mamaPlayback_printReport (mamaTimer timer , void* closure)
{
    char          timeStr[20];
    mamaDateTime  now;
    int           msgInterval= 0;
    mamaFilePlaybackImpl* impl = (mamaFilePlaybackImpl*)closure;
    if (impl->myNumMsg == 0)
        return;    /* No messages to report yet */
    msgInterval = impl->myNumMsg - impl->myNumMsgLast;

    mamaDateTime_create (&now);
    mamaDateTime_setToNow (now);
    mamaDateTime_getAsFormattedString (now, timeStr, 20, "%F %T");
    mamaDateTime_destroy (now);
    mama_logStdout (MAMA_LOG_LEVEL_NORMAL,
                    "%s, %-10ld, %-12ld\n", timeStr, msgInterval, impl->myNumMsg);
    impl->myNumMsgLast = impl->myNumMsg;
}

static mama_bool_t mamaPlayback_addPublisher (mamaPlaybackPublisher mamaPlayback,
                                              char* key,
                                              mamaPublisher publisher)
{
    int result = 1;
    mamaFilePlaybackImpl* impl = (mamaFilePlaybackImpl*)mamaPlayback;
    if (impl == NULL) return MAMA_STATUS_NULL_ARG;
    result = wtable_insert (impl->myPublisherTable,
                            key, publisher);
    if (result != 1)
    {
        return -1;
    }
    return result;
}
static mama_bool_t mamaPlayback_addTransport (mamaPlaybackPublisher mamaPlayback,
                                              char* key,
                                              mamaTransport transport)
{
    int result = 1;
    mamaFilePlaybackImpl* impl = (mamaFilePlaybackImpl*)mamaPlayback;
    if (impl == NULL) return MAMA_STATUS_NULL_ARG;
    result  = wtable_insert (impl->myTransportTable,
                             key, transport);
    if (result != 1)
    {
        return -1;
    }
    return result;
}
/**
 * @desc :  Copies content of input to output starting from
 *          specified location of the buffer.
 *
 */

void mamaPlayback_rightCopy (char* input, char* output, int start)
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

static int
injectDateTime (mamaDateTime dateTime,
                mamaMsg      msg)
{
    mama_status         status;
    /* Ref: getLatencyAsNum, mamatestc */
    static unsigned int fidSecs = 1051;
    static unsigned int fidMicro = 1052;
    mama_u32_t          seconds;
    mama_u32_t          microseconds;

    if (dateTime == NULL || msg == NULL)
        return 0;

    status = mamaDateTime_setToNow (dateTime);
    if (status != MAMA_STATUS_OK)
        return 0;

    status = mamaDateTime_getEpochTime (dateTime,
                                        &seconds,
                                        &microseconds,
                                        NULL);
    if (status != MAMA_STATUS_OK)
        return 0;

    /*"wPubTimeSec"*/
    status = mamaMsg_updateI64 (msg, NULL, fidSecs, seconds);
    if (status != MAMA_STATUS_OK)
        return 0;

    /*"wPubTimeMicroSec"*/
    status = mamaMsg_updateI64 (msg, NULL, fidMicro, microseconds);
    if (status != MAMA_STATUS_OK)
        return 0;

    return 1;
}

static void
mamaPlayback_printConnectionsForTransport (wtable_t    t,
                                           void*       data,
                                           const char* key,
                                           void*       closure)
{
    int             i;
    mamaTransport   transport = NULL;
    mamaConnection* result = NULL;
    uint32_t        len;
    mamaDateTime    now;
    char            timeStr[20];

    transport = (mamaTransport)data;
    mamaTransport_getAllConnections (transport, &result, &len);
    for (i = 0; i < len; i++)
    {
        mamaDateTime_create (&now);
        mamaDateTime_setToNow (now);
        mamaDateTime_getAsFormattedString (now, timeStr, 20, "%F %T");
        mamaDateTime_destroy (now);
        mama_logStdout (MAMA_LOG_LEVEL_NORMAL, "%s, %s\n",
                        timeStr, mamaConnection_toString (result[i]));
    }
    mamaTransport_freeAllConnections (transport, result, len);
}

void
mamaPlayback_printConnections (mamaPlaybackPublisher mamaPlayback)
{
    mamaFilePlaybackImpl* impl = (mamaFilePlaybackImpl*)mamaPlayback;
    wtable_for_each (impl->myTransportTable,
                     mamaPlayback_printConnectionsForTransport,
                     NULL);
}
