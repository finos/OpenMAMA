/* $Id: //commsof_depot/platform/products/6.0/trunk/mama/c_cpp/src/c/fieldcache/fieldcachevector.h#1 $
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

#ifndef MamaFieldCacheVectorH__
#define MamaFieldCacheVectorH__

#include <mama/config.h>
#include <mama/types.h>
#include <mama/status.h>

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */

/* This structure describes a generic dynamic vector of items
    */
typedef struct mamaFieldCacheVectorImpl_
{
    /* Pointer to void* data */
    void** mData;
    /* size of the allocated array */
    mama_size_t mSize;
} mamaFieldCacheVectorImpl;

typedef mamaFieldCacheVectorImpl* mamaFieldCacheVector;

/**
 * Create a generic dynamic vector.
 * 
 * @param vector (out) The vector to create.
 * @return Result of the operation.
 */
mama_status
mamaFieldCacheVector_create(mamaFieldCacheVector* vector);

/**
 * Destroy a vector. No memory is freed as items might require specific destruction
 * functions. If items can be safely destroyed with free, then
 * <code>mamaFieldCacheVector_free</code> can be used to free all the items.
 * In all the other cases, the user is responsible for freeing the item's specific
 * allocated memory.
 * 
 * @param vector (in) The vector to destroy.
 * @return Result of the operation.
 */
mama_status
mamaFieldCacheVector_destroy(mamaFieldCacheVector vector);

/**
 * Free all not-NULL items in the vector using free. This can be used only if the
 * item was allocated with <code>malloc</code> or the like.
 * This function does not destroy the vector: mamaFieldCacheVector_destroy must be called anyway.
 * 
 * @param vector (in) The vector to free.
 * @return Result of the operation.
 */
mama_status
mamaFieldCacheVector_free(mamaFieldCacheVector vector);

/**
 * Set to NULL all items in the vector. This will not free any memory allocated for
 * the items. The size of the vector will not be modified.
 *
 * @param vector (in) The vector to clear.
 * @return Result of the operation.
 */
mama_status
mamaFieldCacheVector_clear(mamaFieldCacheVector vector);

/**
 * Set an item in the vector. Index must be in range (less than vector size).
 * 
 * @param vector (in) The vector to set the value to.
 * @param index (in) The index of the item in the vector.
 * @param item (in) The item to set.
 * @return Result of the operation.
 */
mama_status
mamaFieldCacheVector_set(mamaFieldCacheVector vector, mama_size_t index, void* item);

/**
 * Get an item from the vector. Index must be in range (less than vector size).
 * 
 * @param vector (in) The vector to get the value from.
 * @param index (in) The index of the item in the vector.
 * @param item (out) The item in the vector.
 * @return Result of the operation.
 */
mama_status
mamaFieldCacheVector_get(mamaFieldCacheVector vector, mama_size_t index, void** item);

/**
 * Get the size of the vector.
 * 
 * @param vector (in) The vector to get the size.
 * @param size (out) The size of the vector.
 * @return Result of the operation.
 */
mama_status
mamaFieldCacheVector_getSize(mamaFieldCacheVector vector, mama_size_t* size);

/**
 * Reallocate the vector to be able to contain at least newAllocSize.
 * 
 * @param vector (in) The vector to resize.
 * @param newAllocSize (in) The new size.
 * @return Result of the operation.
 */
mama_status
mamaFieldCacheVector_grow(mamaFieldCacheVector vector, mama_size_t newAllocSize);

#if defined(__cplusplus)
}
#endif

#endif	/* MamaFieldCacheVectorH__ */
