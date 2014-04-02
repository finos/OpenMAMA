/* $Id: //commsof_depot/platform/products/6.0/trunk/mama/c_cpp/src/c/fieldcache/fieldcachelist.h#1 $
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

#ifndef MamaFieldCacheListH__
#define MamaFieldCacheListH__

#include <mama/config.h>
#include <mama/types.h>
#include <mama/status.h>
#include "fieldcachevector.h"

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */

/* This structure describes a generic dynamic list of items
    */
typedef struct mamaFieldCacheListImpl_
{
    /* Underlying structure */
    mamaFieldCacheVector mVector;
    /* Position of the last item */
    mama_size_t mLast;
} mamaFieldCacheListImpl;

typedef mamaFieldCacheListImpl* mamaFieldCacheList;

/**
 * Create a generic list.
 *
 * @param lsit (out) The list to create.
 * @return Result of the operation.
 */
mama_status
mamaFieldCacheList_create(mamaFieldCacheList* list);

/**
 * Destroy a list. No memory is freed as items might require specific destruction
 * functions. If items can be safely destroyed with free, then
 * <code>mamaFieldCacheList_free</code> can be used to free all the items.
 * In all the other cases, the user is responsible for freeing the item's specific
 * allocated memory.
 *
 * @param list (in) The list to destroy.
 * @return Result of the operation.
 */
mama_status
mamaFieldCacheList_destroy(mamaFieldCacheList list);

/**
 * Free all not-NULL items in the list using free. This can be used only if the
 * item was allocated with <code>malloc</code> or the like.
 * This function does not destroy the list: mamaFieldCacheList_destroy must be called anyway.
 *
 * @param list (in) The list to free.
 * @return Result of the operation.
 */
mama_status
mamaFieldCacheList_free(mamaFieldCacheList list);

/**
 * Set to NULL all items in the lsit. This will not free any memory allocated for
 * the items.
 *
 * @param list (in) The list to clear.
 * @return Result of the operation.
 */
mama_status
mamaFieldCacheList_clear(mamaFieldCacheList list);

/**
 * Add an item in the list.
 *
 * @param list (in) The list to add the value to.
 * @param item (in) The item to add.
 * @return Result of the operation.
 */
mama_status
mamaFieldCacheList_add(mamaFieldCacheList list, void* item);

/**
 * Set an item in the list. Index must be in range (less than list size).
 *
 * @param list (in) The list to set the value to.
 * @param index (in) The index of the item in the list.
 * @param item (in) The item to set.
 * @return Result of the operation.
 */
mama_status
mamaFieldCacheList_set(mamaFieldCacheList list, mama_size_t index, void* item);

/**
 * Get an item from the list. Index must be in range (less than list size).
 *
 * @param list (in) The list to get the value from.
 * @param index (in) The index of the item in the list.
 * @param item (out) The item in the list.
 * @return Result of the operation.
 */
mama_status
mamaFieldCacheList_get(mamaFieldCacheList list, mama_size_t index, void** item);

/**
 * Get the size of the list.
 *
 * @param list (in) The list to get the size.
 * @param size (out) The size of the list.
 * @return Result of the operation.
 */
mama_status
mamaFieldCacheList_getSize(mamaFieldCacheList list, mama_size_t* size);

#if defined(__cplusplus)
}
#endif

#endif	/* MamaFieldCacheListH__ */
