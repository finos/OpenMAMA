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

#if defined (OPENMAMA_INTEGRATION) && ! defined (OPENMAMA_INTEGRATION_ENDPOINT_POOL_H__)
#define OPENMAMA_INTEGRATION_ENDPOINT_POOL_H__

#include <mama/integration/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

/*=========================================================================
  =                  Public implementation functions                      =
  =========================================================================*/

/**
 * This function is responsible for creating an endpoint pool implementation
 * with the supplied name and allocating all memory required for its operation.
 *
 * @param endpoints The pointer to be populated with the new endpoint pool
 *                  implementation.
 * @param name      The name of the endpoint pool to be created.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL extern mama_status
endpointPool_create                     (endpointPool_t*    endpoints,
                                         const char*        name);

/**
 * This function is responsible for destroying the endpoint pool implementation
 * provided and releasing all memory associated with the pool itself as well as
 * its underlying dependencies.
 *
 * @param endpoints The endpoint pool to be destroyed.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL extern mama_status
endpointPool_destroy                    (endpointPool_t     endpoints);

/**
 * This function is responsible for destroying the endpoint pool implementation
 * provided and releasing all memory associated with the pool itself as well as
 * its underlying dependencies. Includes callback to clean up cascading
 * dependencies.
 *
 * @param endpoints The endpoint pool to be destroyed.
 * @param callback  The callback to trigger to clean up the endpoint itself (or
 *                  NULL if not applicable).
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL extern mama_status
endpointPool_destroyWithCallback        (endpointPool_t     endpoints,
                                         endpointDestroyCb  callback);
/**
 * This will register a supplied endpoint in the pool according to the unique
 * identifier and topic provided. The combination of topic and identifier
 * must be unique.
 *
 * @param endpoints  The endpoint pool to be registered with.
 * @param topic      The topic to register using.
 * @param identifier The identifier to register using which must be unique.
 *                   Duplicates will not be registered.
 * @param opaque     The closure to be returned when this identifier is
 *                   retrieved.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL extern mama_status
endpointPool_registerWithIdentifier     (endpointPool_t     endpoints,
                                         const char*        topic,
                                         const char*        identifier,
                                         void*              opaque);

/**
 * This will register a supplied endpoint in the pool according to the topic
 * provided. In future implementations this may use uuid but for the moment,
 * it simply assumes that the closure provided is unique and uses its pointer
 * as the unique identifier (e.g. MAMA subscription pointer).
 *
 * @param endpoints  The endpoint pool to be registered with.
 * @param topic      The topic to register using.
 * @param identifier The identifier to be populated with the generated
 *                   identifier.
 * @param opaque     The closure to be returned when this identifier is
 *                   retrieved.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL extern mama_status
endpointPool_registerWithoutIdentifier  (endpointPool_t     endpoints,
                                         const char*        topic,
                                         const char**       identifier,
                                         void*              opaque);

/**
 * This will remove registration for the supplied topic / identifier from the
 * supplied endpoint pool and delete all underlying associations. Note the
 * opaque object supplied at creation time is still the responsibility of
 * the calling application to destroy.
 *
 * @param endpoints  The endpoint pool to be removed from.
 * @param topic      The topic to match using.
 * @param identifier The identifier to be removed.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL extern mama_status
endpointPool_unregister                 (endpointPool_t     endpoints,
                                         const char*        topic,
                                         const char*        identifier);

/**
 * This will return all registered endpoints using the supplied topic to match
 * against.
 *
 * @param endpoints  The endpoint pool to query.
 * @param opaque[]   The pointer to populate with the array of opaque endpoints
 *                   to populate.
 * @param count      Size to be populated with the number of array elements
 *                   found by the function.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL extern mama_status
endpointPool_getRegistered              (endpointPool_t     endpoints,
                                         const char*        topic,
                                         endpoint_t*        opaque[],
                                         size_t*            count);

/**
 * This will return the name of the endpoint to be used.
 *
 * @param endpoints  The endpoint pool to query.
 * @param name       The pointer to populate with the name of the supplied
 *                   endpoint pool.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL extern mama_status
endpointPool_getName                    (endpointPool_t     endpoints,
                                         const char**       name);

/**
 * This will determine if the supplied opaque content exists within the supplied
 * endpoint pool for the given topic (e.g. if a subscription already exists in
 * this pool)
 *
 * @param endpoints  The endpoint pool to query.
 * @param topic      The topic to match on.
 * @param content    The opaque content to look for.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL extern int
endpointPool_isRegistedByContent        (endpointPool_t     endpoints,
                                         const char*        topic,
                                         void*              content);

/**
 * This will find a list of endpoints in the pool according to the provided
 * topic and identifier.
 *
 * @param endpoints  The endpoint pool to query.
 * @param topic      The topic to match on.
 * @param identifier The identifier to match on.
 * @param endpoint   The matching endpoint to return.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL extern mama_status
endpointPool_getEndpointByIdentifiers   (endpointPool_t     endpoints,
                                         const char*        topic,
                                         const char*        identifier,
                                         endpoint_t*        endpoint);

#if defined(__cplusplus)
}
#endif

#endif /* OPENMAMA_INTEGRATION_ENDPOINT_POOL_H__ */
