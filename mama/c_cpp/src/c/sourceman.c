/* $Id: sourceman.c,v 1.10.22.2 2011/08/10 14:53:26 nicholasmarriott Exp $
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

#include <mama/sourceman.h>
#include <wombat/wtable.h>

#define SOURCE_TABLE_NAME "MAMA_SOURCE_TABLE"
#define SOURCE_TABLE_NUM_BUCKETS 11
#define WTABLE_SUCCESS 1

typedef struct mamaSourceManagerImpl_
{
    mamaQuality  myQuality;
    wtable_t     mySourceMap;
} mamaSourceManagerImpl;

/*Used for providing context when iterating all sources in a manager*/
typedef struct sourceIteratorContext
{
    void*                             myUserSuppliedClosure;
    mamaSourceManager_sourcesIteratorCb myUserSuppliedCallback;
    mamaSourceManager                   mySourceManager;
} sourceIteratorContext;

mama_status
mamaSourceManager_create (mamaSourceManager*  sourceManager)
{
    mamaSourceManagerImpl* impl = (mamaSourceManagerImpl*) 
                                  malloc (sizeof(mamaSourceManagerImpl));
    if (impl == NULL) return MAMA_STATUS_NOMEM;

    /*Stores all registered mamaSource objects keyed on name*/
    impl->mySourceMap = wtable_create  (SOURCE_TABLE_NAME,
                                        SOURCE_TABLE_NUM_BUCKETS);

    if (!impl->mySourceMap)
    {
        mama_log (MAMA_LOG_LEVEL_FINE, "mamaSourceManager_create:"
                                       " Could not create source map.");
        return MAMA_STATUS_NOMEM;
    }

    *sourceManager = (mamaSourceManager)impl;
    return MAMA_STATUS_OK;
}

mama_status
mamaSourceManager_destroy (mamaSourceManager  sourceManager)
{
    mamaSourceManagerImpl* impl = (mamaSourceManagerImpl*)sourceManager;
    if (!impl) return MAMA_STATUS_NULL_ARG;

    /*TODO - Is the manager responsible for destroying mamaSource objects?
     Yes if it created them I guess and no if it did not?*/
    if (impl->mySourceMap)
    {
        wtable_destroy (impl->mySourceMap);
    }

    free ((mamaSourceManagerImpl*)sourceManager);
    return MAMA_STATUS_OK;
}

mama_status
mamaSourceManager_createSource (mamaSourceManager  sourceManager,
                                const char*        name,
                                mamaSource*        source)
{
    mamaSource              retSource   = NULL;
    mama_status             status      = MAMA_STATUS_OK;
    mamaSourceManagerImpl*  impl        = (mamaSourceManagerImpl*)sourceManager;

    if (!impl) return MAMA_STATUS_NULL_ARG;
    if (!source || !name) return MAMA_STATUS_INVALID_ARG;

    /*First check if we have a source already under this name*/
    retSource = (mamaSource)wtable_lookup (impl->mySourceMap, name);

    /*If we don't already have one  - create a new one*/
    if (!retSource)
    {
        if (MAMA_STATUS_OK!=(status=mamaSource_create (&retSource)))
        {
            mama_log (MAMA_LOG_LEVEL_ERROR, "mamaSourceManager_createSource(): "
                      "Failed to create mamaSource [%d]", status);
            return status;
        }

         mamaSource_setId (retSource, name);
         
        /*If we have jsut created the source - add to the map*/
        if (WTABLE_SUCCESS!=wtable_insert (impl->mySourceMap,
                                           name,
                                           (void*)retSource))
        {
            mama_log (MAMA_LOG_LEVEL_ERROR, "mamaSourceManager_createSource(): "
                      "Failed to add source to map");
            mamaSource_destroy (retSource);
            return MAMA_STATUS_SYSTEM_ERROR;/*TODO - decent status to return!*/
        }
    }
    *source = retSource;
    return MAMA_STATUS_OK;
}

mama_status
mamaSourceManager_findOrCreateSource (mamaSourceManager  sourceManager,
                                      const char*        name,
                                      mamaSource*        source)
{
    mama_status status =  mamaSourceManager_findSource (sourceManager, 
                                                        name, 
                                                        source);                                              
    if (MAMA_STATUS_NOT_FOUND == status)
    {
        return mamaSourceManager_createSource (sourceManager,
                                               name,
                                               source);
    }
    return status;                                                
}

mama_status
mamaSourceManager_findSource (mamaSourceManager  sourceManager,
                              const char*        name,
                              mamaSource*        source)
{
    mamaSource retSource = NULL;
    mamaSourceManagerImpl* impl = (mamaSourceManagerImpl*)sourceManager;

    if (!impl) return MAMA_STATUS_NULL_ARG;
    if (!name) return MAMA_STATUS_INVALID_ARG;

    retSource = wtable_lookup (impl->mySourceMap, name);

    if (!retSource)
    {
        *source = NULL;
        return MAMA_STATUS_NOT_FOUND;
    }

    *source = retSource;

    return MAMA_STATUS_OK;
}

mama_status
mamaSourceManager_addSource (mamaSourceManager  sourceManager,
                             mamaSource         source)
{
    const char*            sourceId       = NULL;
    mamaSourceManagerImpl* impl           = (mamaSourceManagerImpl*)
                                            sourceManager;

    if (!impl) return MAMA_STATUS_NULL_ARG;
    if (!source) return MAMA_STATUS_INVALID_ARG;
    
    /*Use the id of the supplied source as the key in the map*/
    mamaSource_getId (source, &sourceId);
    if (!sourceId) return MAMA_STATUS_SYSTEM_ERROR;/*TODO - new error code*/
  
    return mamaSourceManager_addSourceWithName (sourceManager,
                                                source,
                                                sourceId);
}

mama_status
mamaSourceManager_addSourceWithName (mamaSourceManager  sourceManager,
                                     mamaSource         source,
                                     const char*        name)
{
    mamaSource             existingSource = NULL;
    mamaSourceManagerImpl* impl           = (mamaSourceManagerImpl*)
                                            sourceManager;

    if (!impl) return MAMA_STATUS_NULL_ARG;
    if (!source || !name) return MAMA_STATUS_INVALID_ARG;

    /*Do we already have a source registered with this key?*/
    if (MAMA_STATUS_OK==mamaSourceManager_findSource (sourceManager,
                                                      name,
                                                      &existingSource))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaSourceManager_addSource(): "
                "Duplicate Source Key %s.", name);
        return MAMA_STATUS_SYSTEM_ERROR;/*TODO - decent error code*/
    }
    
    mamaSource_setMappedId (source, name);

    if (WTABLE_SUCCESS!=wtable_insert (impl->mySourceMap,
                                       name,
                                       (void*)source))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaSourceManager_addSource(): "
                "Failed to add source to map");
        return MAMA_STATUS_SYSTEM_ERROR;/*TODO - decent status to return!*/
    }
    return MAMA_STATUS_OK;
}

/*
* Used to provide iteration over all the sources in the group.
*/
static void mamaSourceManagerImpl_iterateGroupTableCb (wtable_t       t,
                                                     void*          data,
                                                     const char*    key,
                                                     void*          closure)
{
    sourceIteratorContext* iteratorContextData = (sourceIteratorContext*)closure;
    mamaSource   aSource     = (mamaSource)data;

    iteratorContextData->myUserSuppliedCallback (
                                iteratorContextData->mySourceManager,
                                aSource,
                                iteratorContextData->myUserSuppliedClosure);
}

mama_status
mamaSourceManager_iterateSources (mamaSourceManager                     sourceManager,
                                mamaSourceManager_sourcesIteratorCb   callback,
                                void*                               closure)
{
    mamaSourceManagerImpl* impl   =   (mamaSourceManagerImpl*)sourceManager;
    sourceIteratorContext iteratorContext;
    if (!impl) return MAMA_STATUS_NULL_ARG;
    if (!callback) return MAMA_STATUS_INVALID_ARG;

    /*Data to be passed as closure to the wtable iterator*/
    iteratorContext.myUserSuppliedClosure  =   closure;
    iteratorContext.myUserSuppliedCallback =   callback;
    iteratorContext.mySourceManager          =   sourceManager;

    wtable_for_each (impl->mySourceMap,
                     mamaSourceManagerImpl_iterateGroupTableCb,
                     &iteratorContext);

    return MAMA_STATUS_OK;
}

