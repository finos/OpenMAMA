/* $Id: playbackcapture.h,v 1.12.22.3 2011/09/01 16:34:38 emmapollock Exp $
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

#ifndef MAMA_CAPTURE_PLAYBACK__
#define MAMA_CAPTURE_PLAYBACK__


#include <mama/mama.h>
#include <wlock.h>
#include <stdlib.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define DEFAULT_SOURCE     "WOMBAT"
#define YES                "yes"
#define MAX_BUFFER          128
#define CAP_DELIMETER    ":"

typedef struct mamaCaptureConifg_
{
    char*          myPlayBackFileName;
    char           myFeedSourceName[MAX_BUFFER];
    char           mySymbolName[MAX_BUFFER];
    char           myTransportName[MAX_BUFFER];
    char*          mamaTimeNow;
    int            mPlaybackFileNameAlloc;
    mamaBridge     myBridge;
}mamaCaptureConfigImpl;

typedef void*  mamaPlaybackCapture; 

/**
 *Method:  mamaCapture_deallocate
 *@param:  mamaPlaybackCapture 
 *@Desc:   clean up any allocated memory 
 *@return: status
 */
MAMAExpDLL
extern mama_status mamaCapture_deallocate (mamaPlaybackCapture mamaCapture);

/**
 *Method:  mamaCapture_allocate
 *@param:  mamaPlaybackCapture
 *@Descr:  allocates memory and initialized
 *         variables with their default values.
 *@return: int
 */
MAMAExpDLL
extern mama_status mamaCapture_allocate (mamaPlaybackCapture* mamaCapture);

/**
 *Method: mamaCapture_setPlaybackTransportName
 *@param: mamaPlaybackCapture
 *@param: transport
 *@Desc:  Sets the underlying transport name
 *        if specified else the 
 *        default transport name "WOMBAT" will be used
 *@return status
 */
MAMAExpDLL
extern mama_status mamaCapture_setTransportName (mamaPlaybackCapture* mamaCapture,
                                                 const char* transport);

/**
 *Method:  mamaCapture_setFeedSource
 *@param:  sourcename
 *@Desc:   Sets the underlying SOURCE for FH, 
 *         this cannot be null, 
 *         a null value should throw an exception
 *@return: status
 */
MAMAExpDLL
extern mama_status mamaCapture_setFeedSource (mamaPlaybackCapture* mamaCapture,
                                              const char* sourceName);

/**
 *Method:  mamaCapture_getFeedSource
 *@param:  mamaPlaybackCapture
 *@parma:  source - address of pointer to assign source 
 *         name to.
 *Desc:    Gets the underlying "SOURCE" name for the FH.
 *@return: status
 */
MAMAExpDLL
extern mama_status mamaCapture_getFeedSource (mamaPlaybackCapture* mamaCapture,
                                              const char** source);

/**
 *Method: mamaCapture_saveMamaMsg
 *@param: mamaPlaybackCapture
 *@param: msg The mamaMsg recieved from subscription.
 *@Desc:  Saves the mamaMsg to the specified playback file
 *        as a bytebuffer.
 */
MAMAExpDLL
extern mama_status mamaCapture_saveMamaMsg (mamaPlaybackCapture* mamaCapture,
                                            mamaMsg* msg);

/**
 *Method:  mamaCapture_setSymbol
 *@param:  mamaPlaybackCapture
 *@param:  symbolName The symbol name that was subscribed to.
 *@Desc:   Set the symbol name which we are interested in.
 *@return: status
 */
MAMAExpDLL
extern mama_status mamaCapture_setSymbol (mamaPlaybackCapture* mamaCapture,
                                          const char* symbolName);

/**
 *Method:  mamaCapture_getSymbol
 *@param:  mamaPlaybackCapture
 *@param:  symbol - address where to assign symbol to.
 *@Desc:   Get the symbolname that we subscribed to.
 *@return: status
 */
MAMAExpDLL
extern mama_status mamaCapture_getSymbol (mamaPlaybackCapture* mamaCapture,
                                          const char** symbol);

/**
 *Method:  mamaCapture_getTransportName
 *@param:  mamaPlaybackCapture
 *@param:  transportName
 *@Desc:   Get the transport name being used for subscription.
 *@return: status
 */
MAMAExpDLL
extern mama_status mamaCapture_getTransportName(mamaPlaybackCapture* mamaCapture,
                                                const char** transport);

/**
 *Method: mamaCapture_openFile
 *@param: mamaPlaybackCapture
 *@param: fileName
 *@Desc:  Opens file used for saving captured data.
 *return: int 
 */
MAMAExpDLL
extern int mamaCapture_openFile(mamaPlaybackCapture* mamaCapture,
                                const char* fileName);

/**
 *Method: mamaCapture_closeFile
 *@param: mamaPlaybackCapture
 *@Desc:  closes the file 
 *@return int
 */
MAMAExpDLL
extern int mamaCapture_closeFile(mamaPlaybackCapture mamaCapture);



#if defined(__cplusplus)
}
#endif /*_cplusplus*/


#endif

