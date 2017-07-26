/* $Id$
 *
 * OpenMAMA: The open middleware agnostic messaging API
 * Copyright (C) 2011 NYSE Inc.
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


/*=========================================================================
  =                             Includes                                  =
  =========================================================================*/

#include <mama/mama.h>
#include <string.h>
#include <wombat/wtable.h>
#include "endpointpool.h"


/*=========================================================================
  =                              Macros                                   =
  =========================================================================*/

#define     ENDPOINT_POOL_TABLE_CHUNK_SIZE          10
#define     ENDPOINT_POOL_BUFFER_CHUNK_SIZE         1024
#define     ENDPOINT_POOL_MAX_ID_LEN                128


/*=========================================================================
  =                Typedefs, structs, enums and globals                   =
  =========================================================================*/

typedef struct endpointPoolImpl
{
    char*       mName;
    wtable_t    mContainer;
    void*       mBuffer;
    size_t      mBufferIndex;       // 0-relative
    size_t      mBufferSize;
} endpointPoolImpl;

typedef struct endpointExistNode
{
    void*       mMatchWith;
    int         mResult;
} endpointExistNode;


/*=========================================================================
  =                  Private implementation prototypes                    =
  =========================================================================*/

/**
 * This function is responsible for extending the endpoint pool to the desired
 * size. Note that if provided with a size smaller than or equal to the current
 * size, it will do nothing, and it will only increase the buffer in blocks of
 * ENDPOINT_POOL_BUFFER_CHUNK_SIZE.
 *
 * @param endpoints The related endpoint pool implementation.
 * @param size      The required buffer size in bytes.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
static mama_status
endpointPoolImpl_extendBuffer           (endpointPoolImpl*  endpoints,
                                         size_t             size);

/**
 * This function is a callback which is triggered by endpointPool_getRegistered
 * and is called in order to append each endpoint found in the wtable to an
 * array which can then be iterated over by the caller.
 *
 * @param table     The related endpoint pool implementation.
 * @param data      The data for the wtable bucket being examined.
 * @param key       The key for the wtable bucket being examined.
 * @param closure   Contains the endpointPoolImpl to add the results to.
 */
static void
endpointPoolImpl_appendEachEndpoint     (wtable_t           table,
                                         void*              data,
                                         const char*        key,
                                         void*              closure);

/**
 * This function is a callback which is triggered by
 * endpointPool_isRegisteredByContent and is called in order to determine if the
 * provided element already exists as a data item in the existing table.
 *
 * @param table     The related endpoint pool implementation.
 * @param data      The data for the wtable bucket being examined.
 * @param key       The key for the wtable bucket being examined.
 * @param closure   Contains an endpointExistNode to match against / populate
 *                  upon match detection.
 */
static void
endpointPoolImpl_checkEndpointExists    (wtable_t           table,
                                         void*              data,
                                         const char*        key,
                                         void*              closure);

/**
 * This function is a callback which is triggered by endpointPool_destroy and
 * exists in order to clean up all child wtable elements on shutdown.
 *
 * @param table     The related endpoint pool implementation.
 * @param data      The data for the wtable bucket being examined.
 * @param key       The key for the wtable bucket being examined.
 * @param closure   Not used.
 */
static void
endpointPoolImpl_destroySubTable        (wtable_t           table,
                                         void*              data,
                                         const char*        key,
                                         void*              closure);

/*=========================================================================
  =               Public interface implementation functions               =
  =========================================================================*/

mama_status
endpointPool_create (endpointPool_t* endpoints, const char* name)
{
    endpointPoolImpl* newEndpoints = NULL;

    if (NULL == endpoints || NULL == name)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    newEndpoints = (endpointPoolImpl*) calloc (1, sizeof (endpointPoolImpl));
    if (NULL == newEndpoints)
    {
        return MAMA_STATUS_NOMEM;
    }

    /* Take a copy of the name - could be a stack variable in caller */
    newEndpoints->mName = strdup (name);
    if (NULL == newEndpoints->mName)
    {
        /* Clean up on failure */
        free (newEndpoints);
        return MAMA_STATUS_NOMEM;
    }

    /* Allocate memory for the initial buffer */
    newEndpoints->mBuffer = calloc (1, ENDPOINT_POOL_BUFFER_CHUNK_SIZE);
    if (NULL == newEndpoints->mBuffer)
    {
        /* Clean up on failure */
        free (newEndpoints->mName);
        free (newEndpoints);
        return MAMA_STATUS_NOMEM;
    }

    /* On successful memory allocation, update the current limit */
    newEndpoints->mBufferSize = ENDPOINT_POOL_BUFFER_CHUNK_SIZE;

    /* Create the top level endpoints table */
    newEndpoints->mContainer = wtable_create (name,
                                              ENDPOINT_POOL_TABLE_CHUNK_SIZE);
    if (newEndpoints->mContainer == NULL)
    {
        /* Clean up on failure */
        free (newEndpoints->mBuffer);
        free (newEndpoints->mName);
        free (newEndpoints);
        return MAMA_STATUS_NOMEM;
    }

    /* Populate the return pointer with the newly created implementation */
    *endpoints = (endpointPool_t) newEndpoints;

    return MAMA_STATUS_OK;
}

mama_status
endpointPool_destroyWithCallback (endpointPool_t endpoints, endpointDestroyCb callback)
{
    endpointPoolImpl* impl = (endpointPoolImpl*) endpoints;

    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Destroy the main wtable and all its contents */
    if (NULL != impl->mContainer)
    {
        /* Destroy each sub table in this container (topic) */
        wtable_for_each (impl->mContainer,
                         endpointPoolImpl_destroySubTable,
                         (void*) callback);
        /* Free the strdup-ed keys still held by the wtable */
        wtable_free_all_xdata (impl->mContainer);
        /* Finally, destroy the wtable */
        wtable_destroy (impl->mContainer);
    }

    /* Destroy the buffer */
    if (NULL != impl->mBuffer)
    {
        free (impl->mBuffer);
    }

    /* Destroy the strdup-ed name */
    if (NULL != impl->mName)
    {
        free (impl->mName);
    }

    /* Destroy the implementation */
    free (impl);

    return MAMA_STATUS_OK;
}

mama_status
endpointPool_destroy (endpointPool_t endpoints)
{
    return endpointPool_destroyWithCallback (endpoints, NULL);
}

mama_status endpointPool_registerWithIdentifier (endpointPool_t     endpoints,
                                                 const char*        topic,
                                                 const char*        identifier,
                                                 void*              opaque)
{
    endpointPoolImpl*   impl             = (endpointPoolImpl*) endpoints;
    wtable_t            registeredTable  = NULL;
    void*               entry            = NULL;

    if (NULL == impl || NULL == topic || NULL == identifier)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Get the wtable representing the endpoints associated with this topic */
    registeredTable = wtable_lookup (impl->mContainer, topic);

    /* lookup will return NULL if no entry exists */
    if (NULL == registeredTable)
    {
        /* Allocate memory for the table */
        wtable_t newConsumerTable = NULL;

        /* Create a new sub table for the consumers of topic */
        newConsumerTable = wtable_create (topic, 1);

        /* Insert the address as a key for the table */
        wtable_insert (newConsumerTable, identifier, opaque);

        /* Insert this new table into the parent table */
        wtable_insert (impl->mContainer, topic, newConsumerTable);
    }
    /* We have an existing table for this topic */
    else
    {
        mama_log (MAMA_LOG_LEVEL_FINER,
                  "qpidBridgeMamaTransportImpl_dispatchThread(): "
                  "consumer_wtable does exist - reusing.");

        /* Check if there are any entries for this url */
        entry = wtable_lookup (registeredTable, identifier);

        /* If there is no entry for this identifier, create one */
        if(NULL == entry)
        {
            /* Insert the address as a key for the table */
            wtable_insert (registeredTable, identifier, opaque);
        }

    }
    return MAMA_STATUS_OK;
}

mama_status endpointPool_registerWithoutIdentifier (endpointPool_t  endpoints,
                                                    const char*     topic,
                                                    const char**    identifier,
                                                    void*           opaque)
{
    char    identifierBuffer[ENDPOINT_POOL_MAX_ID_LEN];
    int     bytesWritten = 0;

    if (  NULL == endpoints || NULL == topic || NULL == identifier
       || NULL == opaque)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Assume the pointer is unique and use it as the buffer identifier */
    bytesWritten = snprintf (identifierBuffer,
                             ENDPOINT_POOL_MAX_ID_LEN,
                             "%p",
                             opaque);

    /* Truncate string if would have overrun */
    if (bytesWritten <= ENDPOINT_POOL_MAX_ID_LEN)
    {
        identifierBuffer[ENDPOINT_POOL_MAX_ID_LEN-1] = '\0';
    }

    *identifier = strdup (identifierBuffer);

    return endpointPool_registerWithIdentifier (endpoints,
                                                topic,
                                                *identifier,
                                                opaque);
}

mama_status
endpointPool_unregister (endpointPool_t     endpoints,
                         const char*        topic,
                         const char*        identifier)
{
    endpointPoolImpl*   impl             = (endpointPoolImpl*) endpoints;
    wtable_t            registeredTable  = NULL;

    if (NULL == impl || NULL == topic || NULL == identifier)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Get the wtable representing the endpoints associated with this topic */
    registeredTable = wtable_lookup (impl->mContainer, topic);

    /* Container must have been nuked - that means we do nothing */
    if (NULL == registeredTable)
    {
        return MAMA_STATUS_OK;
    }

    /* This returns a pointer to the original object which we don't need */
    wtable_remove (registeredTable, identifier);

    return MAMA_STATUS_OK;
}

mama_status
endpointPool_getRegistered (endpointPool_t  endpoints,
                            const char*     topic,
                            endpoint_t*     opaque[],
                            size_t*         count)
{
    endpointPoolImpl*   impl                = (endpointPoolImpl*) endpoints;
    wtable_t            registeredTable     = NULL;

    if (NULL == impl || NULL == topic || NULL == opaque || NULL == count)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Clear provided values */
    *count  = 0;
    *opaque = NULL;

    /* Get the wtable representing the endpoints associated with this topic */
    registeredTable = wtable_lookup (impl->mContainer, topic);

    /* Container must have been nuked - nothing to return */
    if (registeredTable == NULL)
    {
        /* Already flagged to return count=0 results so this is not an error */
        return MAMA_STATUS_OK;
    }

    /* Reset the offset for iteration */
    impl->mBufferIndex = 0;

    /* Iterate over the table, appending the results to the buffer */
    wtable_for_each (registeredTable, endpointPoolImpl_appendEachEndpoint,
                     (void*)impl);

    /* Populate the return values */
    *count  = impl->mBufferIndex;
    *opaque = (void**)impl->mBuffer;

    return MAMA_STATUS_OK;
}

mama_status
endpointPool_getName (endpointPool_t    endpoints,
                      const char**      name)
{
    endpointPoolImpl* impl = (endpointPoolImpl*) endpoints;

    if (NULL == impl || NULL == name)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    if (NULL == impl->mName)
    {
        return MAMA_STATUS_PLATFORM;
    }

    /* Populate the return value */
    *name = impl->mName;

    return MAMA_STATUS_OK;
}

int
endpointPool_isRegistedByContent (endpointPool_t    endpoints,
                                  const char*       topic,
                                  void*             content)
{
    endpointPoolImpl*       impl             = (endpointPoolImpl*) endpoints;
    wtable_t                registeredTable  = NULL;

    /* The existNode will be used during wtable iteration to store the result */
    endpointExistNode       existNode;

    if (NULL == impl || NULL == topic || NULL == content)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Get the wtable representing the endpoints associated with this topic */
    registeredTable = wtable_lookup (impl->mContainer, topic);
    if (registeredTable == NULL)
    {
        return 0;
    }

    /* Iterate over the table, appending the results to the buffer */
    existNode.mMatchWith   = content;
    existNode.mResult      = 0;
    wtable_for_each (registeredTable,
                     endpointPoolImpl_checkEndpointExists,
                     (void*) &existNode);

    return 1;
}

mama_status
endpointPool_getEndpointByIdentifiers   (endpointPool_t     endpoints,
                                         const char*        topic,
                                         const char*        identifier,
                                         endpoint_t*        endpoint)
{
    endpointPoolImpl*       impl             = (endpointPoolImpl*) endpoints;
    wtable_t                registeredTable  = NULL;
    endpoint_t              existingEndpoint = NULL;

    /* Get the wtable representing the endpoints associated with this topic */
    registeredTable = wtable_lookup (impl->mContainer, topic);

    if (registeredTable == NULL)
    {
        return MAMA_STATUS_NOT_FOUND;
    }

    existingEndpoint = wtable_lookup (registeredTable, identifier);
    if (existingEndpoint == NULL)
    {
        return MAMA_STATUS_NOT_FOUND;
    }
    *endpoint = existingEndpoint;
    return MAMA_STATUS_OK;
}

/*=========================================================================
  =                  Private implementation functions                     =
  =========================================================================*/

mama_status
endpointPoolImpl_extendBuffer (endpointPoolImpl* impl, size_t size)
{
    size_t  newSize     = 0;
    void*   newBuffer   = NULL;

    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Don't need to do anything if buffer is already allocated */
    if (size <= impl->mBufferSize)
    {
        return MAMA_STATUS_OK;
    }

    /* Increment upfront by products of chunk size */
    newSize = impl->mBufferSize;
    while (newSize < size)
    {
        newSize += ENDPOINT_POOL_BUFFER_CHUNK_SIZE;
    }

    /* Reallocate memory as required */
    newBuffer = realloc (impl->mBuffer, newSize);
    if (NULL == newBuffer)
    {
        return MAMA_STATUS_NOMEM;
    }
    else
    {
        impl->mBuffer = newBuffer;
        return MAMA_STATUS_OK;
    }
}

void
endpointPoolImpl_appendEachEndpoint (wtable_t      table,
                                     void*         data,
                                     const char*   key,
                                     void*         closure)
{
    endpointPoolImpl*   impl        = (endpointPoolImpl*) closure;
    size_t              required    = sizeof(void*) * (impl->mBufferIndex +1);

    /* Get more space if required */
    if (required > impl->mBufferSize)
    {
        endpointPoolImpl_extendBuffer (impl, required);
    }

    /*
     * If the data element is null, return the key instead (upstream caller
     * expects this behaviour)
     */
    if (NULL == data)
    {
        data = (void*) key;
    }

    /* Update the next offset with the data */
    ((void**)impl->mBuffer)[impl->mBufferIndex] = data;

    /* Increment the offset */
    impl->mBufferIndex++;
}

void
endpointPoolImpl_checkEndpointExists (wtable_t     table,
                                      void*        data,
                                      const char*  key,
                                      void*        closure)
{
    endpointExistNode*  existNode   = (endpointExistNode*) closure;

    /* Mark as a success if this matches the supplied pointer */
    if (data == existNode->mMatchWith)
    {
        existNode->mResult = 1;
    }
}

void
endpointPoolImpl_destroySubTable (wtable_t     table,
                                  void*        data,
                                  const        char* key,
                                  void*        closure)
{
    if (NULL != closure)
    {
        /* Destroy each sub table in this container (topic) */
        wtable_for_each ((wtable_t)data,
                         endpointPoolImpl_destroySubTable,
                         closure);
    }

    /* Free all wtable strdup-ed keys but not the data */
    wtable_free_all_xdata ((wtable_t)data);

    /* Destroy the table itself */
    wtable_destroy((wtable_t)data);
}
