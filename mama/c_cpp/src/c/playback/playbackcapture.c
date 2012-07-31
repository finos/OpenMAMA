/* $Id: playbackcapture.c,v 1.13.22.6 2011/10/02 19:02:18 ianbell Exp $
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

#include <signal.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <property.h>
#include "playbackcapture.h"
#include <unistd.h>

#define LENGTH 15
#define self ((mamaCaptureConfigImpl*)(mamaCapture))

/**********************************************************
Global variables
**********************************************************/
extern wproperty_t   gProperties;
static FILE*         myPlaybackFile;

/**
 *Method:  mamaCapture_getTimeDate
 *@param:  mamaPlaybackCapture
 *@Desc:   Gets timedate ""%H%M%S%m""for filename of the captured data.
 *@return:  char*
 */
static
char* mamaCapture_getTimeDate (mamaPlaybackCapture mamaCapture);
/**
 *Method: mamaCapture_setFileName
 *@param  mamaPlaybackCapture
 *@Desc:  If a filename is NULL it  generates the
 *        file name internally, this will generate a file
 *        with the format "DATETIME.playback" else
 *        the filename will be used..
 */
static
void mamaCapture_setFileName (mamaPlaybackCapture* mamaCapture,
                              const char** fileName);



/**
 *Allocates memory for mamaCapture, and Initialises
 *default parameters
 */
mama_status mamaCapture_allocate (mamaPlaybackCapture* mamaCapture)
{
    mamaCaptureConfigImpl* impl =
        (mamaCaptureConfigImpl*)calloc(1, sizeof(mamaCaptureConfigImpl));

    if (impl == NULL)
    {
        return MAMA_STATUS_NOMEM;
    }
    impl->myPlayBackFileName = NULL;
    *mamaCapture = (mamaPlaybackCapture*)impl;
    return MAMA_STATUS_OK;
}

int mamaCapture_closeFile (mamaPlaybackCapture mamaCapture)
{
    int result = -1;
    mamaCaptureConfigImpl* impl = (mamaCaptureConfigImpl*)mamaCapture;
    if (myPlaybackFile)
    {
        result= fclose (myPlaybackFile);
    }
    if (result != 0)
    {
        mama_log (MAMA_LOG_LEVEL_FINE,
                  "mamaCapture_closeFile: Error closing file %s",
                  impl->myPlayBackFileName);
        return EOF;
    }
    return result;
}

mama_status mamaCapture_deallocate (mamaPlaybackCapture mamaCapture)
{
    mama_status status = MAMA_STATUS_OK;
    if (!self) return MAMA_STATUS_NULL_ARG;

    if (self->myPlayBackFileName && self->mPlaybackFileNameAlloc)
    {
        free (self->myPlayBackFileName);
        self->myPlayBackFileName = NULL;
    }
    if (self->mamaTimeNow)
    {
        free (self->mamaTimeNow);
        self->mamaTimeNow = NULL;
    }
    free(self);
    return status;
}

int mamaCapture_openFile(mamaPlaybackCapture* mamaCapture,
                         const char* fileName)
{
    mama_status status = MAMA_STATUS_OK;
    mamaCaptureConfigImpl* impl = (mamaCaptureConfigImpl*)*mamaCapture;

    mamaCapture_setFileName (mamaCapture,&fileName);

    if ((impl->myPlayBackFileName[0] != '/') &&
        (impl->myPlayBackFileName[0] != '.'))
    {
        mama_log (MAMA_LOG_LEVEL_FINE,
                  "mamaCapture_openFile: opening file: %s",
                  impl->myPlayBackFileName);
        myPlaybackFile = fopen(impl->myPlayBackFileName,"wa");

        mama_log (MAMA_LOG_LEVEL_FINE,
                  "mamaCapture_openFile: file  %s succesfully opened.",
                  impl->myPlayBackFileName);
    }
    if (!(myPlaybackFile))
    {
        status = MAMA_STATUS_INVALID_ARG;
        mama_log (MAMA_LOG_LEVEL_FINE,
                  "mamaCapture_openFile: %s error: unable to open file %s",
                  mamaMsgStatus_stringForStatus(status),
                  impl->myPlayBackFileName);
    }
    return  status;
}

void mamaCapture_setFileName (mamaPlaybackCapture* mamaCapture,
                              const char** fileName)
{
    mamaCaptureConfigImpl* impl = (mamaCaptureConfigImpl*)*mamaCapture;
    char* temp;
    if(*fileName == NULL)
    {
        temp = (char*) calloc (MAX_BUFFER, sizeof(char));
        impl->mamaTimeNow = mamaCapture_getTimeDate(mamaCapture);
        strcat (temp, impl->mamaTimeNow);
        strcat (temp, ".playback");
        impl->myPlayBackFileName = (char*)temp;
        impl->mPlaybackFileNameAlloc = 1;
    }
    else
    {
        impl->myPlayBackFileName = (char*)(*fileName);
    }
}
mama_status mamaCapture_setTransportName (mamaPlaybackCapture* mamaCapture,
                                          const char* transport)
{
    mamaCaptureConfigImpl* impl = (mamaCaptureConfigImpl*)*mamaCapture;
    if (impl == NULL) return MAMA_STATUS_NULL_ARG;

    if (transport == NULL)
    {
        mama_logStdout (MAMA_LOG_LEVEL_NORMAL,
                        "MamaCapture WARN!!: Transport name is null" \
                        "Default transport \"WOMBAT\" will be used \n");

        strcpy (impl->myTransportName, DEFAULT_SOURCE);
    }
    else
    {
        strcpy (impl->myTransportName, transport);
    }
    return MAMA_STATUS_OK;
}

mama_status mamaCapture_setFeedSource (mamaPlaybackCapture* mamaCapture,
                                       const char* sourceName)
{
    mamaCaptureConfigImpl* impl = (mamaCaptureConfigImpl*)*mamaCapture;
    if(sourceName == NULL)
    {
        mama_logStdout (MAMA_LOG_LEVEL_NORMAL,
                        "MamaCapture WARN!!: Feedhandler source is null" \
                        "Exiting!!!  \n");
        exit(0);
    }
    strcpy (impl->myFeedSourceName ,sourceName);
    return MAMA_STATUS_OK;
}

mama_status mamaCapture_getFeedSource (mamaPlaybackCapture* mamaCapture,
                                       const char** sourceName)
{
    mamaCaptureConfigImpl* impl = (mamaCaptureConfigImpl*)*mamaCapture;
    if (!self) return MAMA_STATUS_NULL_ARG;

    *sourceName= impl->myFeedSourceName;
    return MAMA_STATUS_OK;
}
/**
  Serializes the bytebuffer of any msg to file.
  The default delimeter (:) will be used.
  Each saved msg will be in the format SOURCE:TPORT:SYMBOL:MSGLEN:BUFFER
*/
mama_status mamaCapture_saveMamaMsg (mamaPlaybackCapture* mamaCapture,
                                     mamaMsg* msg)
{
    mamaCaptureConfigImpl* impl = (mamaCaptureConfigImpl*)*mamaCapture;

    const void* buffer       = NULL;
    mama_size_t bufferSize   = 0;
    const char*       temp   = NULL;
    mama_status status       = MAMA_STATUS_OK;
    char*       delim        = CAP_DELIMETER;
    char        bufferLength  [LENGTH];
    char        captureBuffer [MAX_BUFFER];

    if (impl == NULL) return MAMA_STATUS_NULL_ARG;


    if (MAMA_STATUS_OK != (status = mamaMsg_getByteBuffer
                           (*msg, &buffer, &bufferSize)))
    {
        mama_log (MAMA_LOG_LEVEL_NORMAL,
                  "mamaCapture_saveMamaMsg Error!!!:  " \
                  "failure at mamaMsg_getMsgByteBuffer \n");
        exit (0);
    }
    memset (bufferLength, '\0', LENGTH);

    /* Build up format and append bytebuffer */
    if(myPlaybackFile)
    {
        memset (captureBuffer,'\0', MAX_BUFFER);
        mamaCapture_getFeedSource(mamaCapture,&temp);
        strcat (captureBuffer, temp);
        strcat (captureBuffer, delim);
        mamaCapture_getTransportName (mamaCapture,&temp);
        strcat (captureBuffer, temp);
        strcat (captureBuffer, delim);
        mamaCapture_getSymbol (mamaCapture,&temp);
        strcat (captureBuffer, temp);
        strcat (captureBuffer, delim);
        memset (bufferLength, '\0', LENGTH);
        sprintf (bufferLength, "%u", bufferSize);
        strcat (captureBuffer, bufferLength);
        fwrite (captureBuffer, 1, strlen(captureBuffer), myPlaybackFile);
        fputc  ((char)29,myPlaybackFile);
        fwrite (buffer , bufferSize, 1, myPlaybackFile);
        fflush(myPlaybackFile);

    }
    else
    {
        mama_log(MAMA_LOG_LEVEL_NORMAL,
                 "mamaCapture_saveMamaMsg Error!!!: Capture file does not exist" \
                 "or has not been initialized\n");
        return MAMA_STATUS_NULL_ARG;
    }
    return MAMA_STATUS_OK;
}

char* mamaCapture_getTimeDate (mamaPlaybackCapture mamaCapture)
{
    time_t timer;
    struct tm *tmResult;
    char *timeBuffer;

    timer=time(NULL);
    tmResult = localtime (&timer);
    timeBuffer = calloc (MAX_BUFFER, sizeof(char));

    strftime (timeBuffer, MAX_BUFFER,
              "%H%M%S%m%d", tmResult);

    mama_logStdout (MAMA_LOG_LEVEL_FINE,"mamaCapture_getTimeDate: %s\n",
                    timeBuffer);
    return timeBuffer;
}

mama_status mamaCapture_setSymbol (mamaPlaybackCapture* mamaCapture,
                                   const char* symbolName)
{
    mamaCaptureConfigImpl* impl = (mamaCaptureConfigImpl*)*mamaCapture;
    if (!self) return MAMA_STATUS_NULL_ARG;

    if (symbolName == NULL)
    {
        mama_logStdout (MAMA_LOG_LEVEL_NORMAL,
                        "MamaCapture WARN!!: Symbol name (NULL) \n");
        return MAMA_STATUS_NULL_ARG;
    }
    strcpy(impl->mySymbolName, symbolName);
    return MAMA_STATUS_OK;
}

mama_status mamaCapture_getSymbol (mamaPlaybackCapture* mamaCapture,
                                   const char** symbol)
{
    mamaCaptureConfigImpl* impl = (mamaCaptureConfigImpl*)*mamaCapture;
    mama_status status = MAMA_STATUS_OK;
    if (impl == NULL) return MAMA_STATUS_NULL_ARG;
    *symbol = impl->mySymbolName;
    return status;
}

mama_status mamaCapture_getTransportName (mamaPlaybackCapture* mamaCapture,
                                          const char** transportName)
{
    mamaCaptureConfigImpl* impl = (mamaCaptureConfigImpl*)*mamaCapture;
    mama_status status = MAMA_STATUS_OK;
    if (!self) return MAMA_STATUS_NULL_ARG;

    *transportName = impl->myTransportName;
    return status;
}
/**End**/
