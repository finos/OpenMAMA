/* $Id: source.c,v 1.10.34.4 2011/08/10 14:53:25 nicholasmarriott Exp $
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

#include "mama/source.h"
#include "mama/mama.h"
#include "mama/sourceman.h"
#include "string.h"

typedef struct mamaSourceImpl_
{
    mamaSource          myParent;
    mamaSourceManager   mySubSourceManager;
    mamaTransport       myMamaTransport;
    mamaQuality         myQuality;
    mamaSourceState     myState;
    mamaSymbology       mySymbology;
    const char*         myId;
    const char*         myMappedId;
    const char*         myDisplayId;
    const char*         mySymbolNameSpace;
    const char*         myTransportName;
    /*TODO A linked list of symbols/subscriptions?*/
} mamaSourceImpl;

mama_status
mamaSource_create (mamaSource*  source)
{
    mama_status     status  =   MAMA_STATUS_OK;
    mamaSourceImpl* impl    = (mamaSourceImpl*) malloc (sizeof(mamaSourceImpl));
    if (impl == NULL) return MAMA_STATUS_NOMEM;
    
    impl->myId                  = NULL;
    impl->myMappedId            = NULL;
    impl->myDisplayId           = NULL;
    impl->mySymbolNameSpace     = NULL;
    impl->myTransportName       = NULL;
    impl->mySubSourceManager    = NULL;
    impl->mySymbology           = NULL;
    mamaSource_clear(impl);

    /*Create a sub source manager*/
    if (MAMA_STATUS_OK!=(status=mamaSourceManager_create
                                (&impl->mySubSourceManager)))
    {
        mama_log (MAMA_LOG_LEVEL_FINE, "mamaSource_create: Could not create"
                                       "sub source manager. ");
        free (impl);
        *source = NULL;
        return status;
    }
    
    *source = (mamaSource)impl;
    
    return MAMA_STATUS_OK;
}

mama_status
mamaSource_destroy (mamaSource  source)
{
    mamaSourceImpl* impl = (mamaSourceImpl*)source;
    if (!impl) return MAMA_STATUS_NULL_ARG;

    mamaSource_clear (source);
    free (impl);
    return MAMA_STATUS_OK;
}

mama_status
mamaSource_clear (mamaSource  source)
{
    mamaSourceImpl* impl = (mamaSourceImpl*)source;
    if (!source)
        return MAMA_STATUS_INVALID_ARG;
    if (impl->myId)
    {
        free((void*)impl->myId);
        impl->myId = NULL;
    }
    if (impl->myMappedId)
    {
        free((void*)impl->myMappedId);
        impl->myMappedId = NULL;
    }
    if (impl->myDisplayId)
    {
        free((void*)impl->myDisplayId);
        impl->myDisplayId = NULL;
    }
    if (impl->mySymbolNameSpace)
    {
        free ((void*)impl->mySymbolNameSpace);
        impl->mySymbolNameSpace = NULL;
    }
    if (impl->myTransportName)
    {
        free ((void*)impl->myTransportName);
        impl->myTransportName = NULL;
    }
    if (impl->mySubSourceManager)
    {
        mamaSourceManager_destroy (impl->mySubSourceManager);
        impl->mySubSourceManager    = NULL;
    }
    impl->myParent              = NULL;
    impl->myMamaTransport       = NULL;
    impl->mySymbology           = NULL;
    impl->myQuality = MAMA_QUALITY_UNKNOWN;
    impl->myState   = MAMA_SOURCE_STATE_UNKNOWN;
    return MAMA_STATUS_OK;
}

mama_status
mamaSource_setId (mamaSource   source,
                  const char*  id)
{
    mamaSourceImpl* impl = (mamaSourceImpl*)source;
    if (!source)
        return MAMA_STATUS_INVALID_ARG;
    if (impl->myId)
    {
        free((void*)impl->myId);
        impl->myId = NULL;
    }
    if (id)
    {
        impl->myId = strdup (id);
        if (!impl->myId)
        {
            return MAMA_STATUS_NOMEM;
        }
    }
    return MAMA_STATUS_OK;
}

mama_status
mamaSource_setMappedId (mamaSource   source,
                         const char*  id)
{
    mamaSourceImpl* impl = (mamaSourceImpl*)source;
    if (!source)
        return MAMA_STATUS_INVALID_ARG;
    if (impl->myMappedId)
    {
        free((void*)impl->myMappedId);
        impl->myMappedId = NULL;
    }
    if (id)
    {
        impl->myMappedId = strdup (id);
        if (!impl->myMappedId)
        {
            return MAMA_STATUS_NOMEM;
        }
    }
    return MAMA_STATUS_OK;
}

mama_status
mamaSource_setDisplayId (mamaSource   source,
                         const char*  id)
{
    mamaSourceImpl* impl = (mamaSourceImpl*)source;
    if (!source)
        return MAMA_STATUS_INVALID_ARG;
    if (impl->myDisplayId)
    {
        free((void*)impl->myDisplayId);
        impl->myDisplayId = NULL;
    }
    if (id)
    {
        impl->myDisplayId = strdup (id);
        if (!impl->myDisplayId)
        {
            return MAMA_STATUS_NOMEM;
        }
    }
    return MAMA_STATUS_OK;
}

mama_status
mamaSource_setQuality (mamaSource   source,
                       mamaQuality  quality)
{
    mamaSourceImpl* impl = (mamaSourceImpl*)source;
    if (!source)
        return MAMA_STATUS_INVALID_ARG;
    impl->myQuality = quality;
    return MAMA_STATUS_OK;
}

mama_status
mamaSource_setState (mamaSource       source,
                     mamaSourceState  state)
{
    mamaSourceImpl* impl = (mamaSourceImpl*)source;
    if (!source)
        return MAMA_STATUS_INVALID_ARG;
    impl->myState = state;
    return MAMA_STATUS_OK;
}

mama_status
mamaSource_setParent (mamaSource       source,
                      mamaSource       parent)
{
    mama_status         status     = MAMA_STATUS_OK;
    mamaSourceImpl*     impl       = (mamaSourceImpl*)source;
    mamaSourceImpl*     parentImpl = (mamaSourceImpl*)parent;
    if (!impl)  return MAMA_STATUS_NULL_ARG;

    /*Set this source as our parent*/
    impl->myParent = parent;
    
    /* Now add ourself to the parents list of sub sources. */
    if (MAMA_STATUS_OK != (status=mamaSourceManager_addSource (
                    parentImpl->mySubSourceManager,
                    source)))
    {
        mama_log (MAMA_LOG_LEVEL_FINE, "Could not add to parent as sub source.");
        return status;
    }
    
    return MAMA_STATUS_OK;
}

mama_status
mamaSource_setSymbolNamespace (mamaSource  source,
                               const char* symbolNameSpace)
{
    mamaSourceImpl* impl = (mamaSourceImpl*)source;
    if (!impl) return MAMA_STATUS_NULL_ARG;

    if (impl->mySymbolNameSpace)
    {
        free ((void*)impl->mySymbolNameSpace);
        impl->mySymbolNameSpace = NULL;
    }

    if (symbolNameSpace)
    {
        impl->mySymbolNameSpace = strdup (symbolNameSpace);
        if (!impl->mySymbolNameSpace) return MAMA_STATUS_NOMEM;
    }
    
    return MAMA_STATUS_OK;
}

mama_status
mamaSource_setTransportName (mamaSource     source,
                             const char*    transportName)
{
    mamaSourceImpl* impl = (mamaSourceImpl*)source;
    if (!impl) return MAMA_STATUS_NULL_ARG;
   
    if (impl->myTransportName)
    {
        free ((void*)impl->myTransportName);
        impl->myTransportName = NULL;
    }

    if (transportName)
    {
        impl->myTransportName = strdup (transportName);
        if (!impl->myTransportName) return MAMA_STATUS_NOMEM;
    }
    
    return MAMA_STATUS_OK;
}

mama_status
mamaSource_setTransport (mamaSource     source,
                         mamaTransport  transport)
{
    mamaSourceImpl* impl = (mamaSourceImpl*)source;
    if (!impl) return MAMA_STATUS_NULL_ARG;

    impl->myMamaTransport = transport;
    
    return MAMA_STATUS_OK;
}

mama_status
mamaSource_setSymbology (mamaSource     source,
                         mamaSymbology  symbology)
{
    mamaSourceImpl* impl = (mamaSourceImpl*)source;
    if (!impl) return MAMA_STATUS_NULL_ARG;

    impl->mySymbology = symbology;
    
    return MAMA_STATUS_OK;
}


mama_status
mamaSource_getId (const mamaSource source,
                  const char**     id)
{
    mamaSourceImpl* impl = (mamaSourceImpl*)source;
    if (!source)
        return MAMA_STATUS_INVALID_ARG;
    *id = impl->myId;
    return MAMA_STATUS_OK;
}

mama_status
mamaSource_getMappedId (const mamaSource source,
                        const char**     id)
{
    mamaSourceImpl* impl = (mamaSourceImpl*)source;
    if (!source) return MAMA_STATUS_INVALID_ARG;

    *id = impl->myMappedId;

    return MAMA_STATUS_OK;
}

mama_status
mamaSource_getDisplayId (const mamaSource source,
                         const char**     id)
{
    mamaSourceImpl* impl = (mamaSourceImpl*)source;
    if (!source) return MAMA_STATUS_INVALID_ARG;

    *id = impl->myDisplayId;

    return MAMA_STATUS_OK;
}

mama_status
mamaSource_getQuality (const mamaSource source,
                       mamaQuality*     quality)
{
    mamaSourceImpl* impl = (mamaSourceImpl*)source;
    if (!source) return MAMA_STATUS_INVALID_ARG;

    *quality = impl->myQuality;

    return MAMA_STATUS_OK;
}

mama_status
mamaSource_getState (const mamaSource   source,
                     mamaSourceState*   state)
{
    mamaSourceImpl* impl = (mamaSourceImpl*)source;
    if (!source) return MAMA_STATUS_NULL_ARG;

    *state = impl->myState;

    return MAMA_STATUS_OK;
}

const char*
mamaSource_getStateAsString (const mamaSource  source)
{
    mamaSourceImpl* impl = (mamaSourceImpl*)source;
    if (!impl) return NULL;

    switch (impl->myState)
    {
        case MAMA_SOURCE_STATE_OFF:
            return "SOURCE_STATE_OFF";
        case MAMA_SOURCE_STATE_OK:
            return "SOURCE_STATUS_OK";
        case MAMA_SOURCE_STATE_UNKNOWN:
            return "SOURCE_STATE_UNKNOWN";
        default:
            return "SOURCE_STATE_UNDEFINED";
    }
}

mama_status
mamaSource_getParent (const mamaSource   source,
                      mamaSource*        parent)
{
    mamaSourceImpl* impl = (mamaSourceImpl*)source;
    if (!source) return MAMA_STATUS_NULL_ARG;

    *parent = impl->myParent;

    return MAMA_STATUS_OK;
}

mama_status
mamaSource_getSymbolNamespace (const mamaSource source,
                               const char**     symbolNameSpace)
{
    mamaSourceImpl* impl = (mamaSourceImpl*)source;
    if (!impl) return MAMA_STATUS_NULL_ARG;

    *symbolNameSpace = impl->mySymbolNameSpace;

    return MAMA_STATUS_OK;
}

mama_status
mamaSource_getTransportName (const mamaSource source,
                             const char**     transportName)
{
    mamaSourceImpl* impl = (mamaSourceImpl*)source;
    if (!impl) return MAMA_STATUS_NULL_ARG;

    *transportName = impl->myTransportName;
    
    return MAMA_STATUS_OK;
}

mama_status
mamaSource_getTransport (const mamaSource source,
                         mamaTransport*   transport)
{
    mamaSourceImpl* impl = (mamaSourceImpl*)source;
    if (!impl) return MAMA_STATUS_NULL_ARG;

    *transport = impl->myMamaTransport;

    return MAMA_STATUS_OK;
}

mama_status
mamaSource_getSymbology (const mamaSource source,
                         mamaSymbology*   symbology)
{
    mamaSourceImpl* impl = (mamaSourceImpl*)source;
    if (!impl) return MAMA_STATUS_NULL_ARG;

    *symbology = impl->mySymbology;

    return MAMA_STATUS_OK;
}

mama_status
mamaSource_getSubSourceManager (const mamaSource   source,
                                mamaSourceManager* subSourceManager)
{
    mamaSourceImpl* impl = (mamaSourceImpl*)source;
    if (!source) return MAMA_STATUS_INVALID_ARG;

    *subSourceManager = impl->mySubSourceManager;
    
    return MAMA_STATUS_OK;
}

