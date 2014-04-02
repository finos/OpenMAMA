/* $Id: //commsof_depot/platform/products/6.0/trunk/mama/c_cpp/src/c/fieldcache/fieldcacheiterator.h#1 $
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

#ifndef MamaFieldCacheIteratorH__
#define MamaFieldCacheIteratorH__

#include <mama/config.h>
#include <mama/status.h>
#include <mama/types.h>
#include <mama/fieldcache/fieldcachetypes.h>

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */

/**
 * This function will create an iterator to the given field cache and allocate all
 * associated memory. If this function succeeds then the iterator should be destroyed
 * by calling mamaFieldCacheIterator_destroy.
 * 
 * @param iterator (out) The iterator to create.
 * @param fieldCache (in) The cache to use to iterate.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheIterator_create(mamaFieldCacheIterator* iterator,
                              mamaFieldCache fieldCache);

/**
 * This function will destroy a field cache iterator previously allocated by a call
 * to mamaFieldCacheIterator_create.
 * 
 * @param iterator (in) The iterator to destroy.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheIterator_destroy(mamaFieldCacheIterator iterator);

/**
 * This function will return the next field in the cache. If no other fields are
 * present it will return NULL.
 * 
 * @param iterator (in) The iterator to get the field from.
 * @return A mamaFieldCacheField pointing to the next field.
 */
MAMAExpDLL
extern mamaFieldCacheField
mamaFieldCacheIterator_next(mamaFieldCacheIterator iterator);

/**
 * This function will return a flag indicating if there are remaining fields to
 * iterate.
 * 
 * @param iterator (in) The iterator to check for other fields.
 * @return A flag indicating if there are other fields to iterate.
 */
MAMAExpDLL
extern mama_bool_t
mamaFieldCacheIterator_hasNext(mamaFieldCacheIterator iterator);

/**
 * This function will return the first field in the cache. If the cache is empty,
 * a NULL field is returned.
 * 
 * @param iterator (in) The iterator to get the first field from.
 * @return The first field in cache.
 */
MAMAExpDLL
extern mamaFieldCacheField
mamaFieldCacheIterator_begin(mamaFieldCacheIterator iterator);

#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

#endif /* MamaFieldCacheIteratorH__ */
