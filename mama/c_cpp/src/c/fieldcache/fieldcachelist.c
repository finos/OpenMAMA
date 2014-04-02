/* $Id: //commsof_depot/platform/products/6.0/trunk/mama/c_cpp/src/c/fieldcache/fieldcachelist.c#1 $
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

#include "fieldcachelist.h"

#define LIST_GROW_CHUNK_SIZE 32

mama_status
mamaFieldCacheList_create(mamaFieldCacheList* list)
{
    *list = (mamaFieldCacheList)calloc(1, sizeof(mamaFieldCacheListImpl));
    (*list)->mLast = -1;
    return mamaFieldCacheVector_create(&(*list)->mVector);
}

mama_status
mamaFieldCacheList_destroy(mamaFieldCacheList list)
{
    mamaFieldCacheVector_destroy(list->mVector);
    free(list);
    return MAMA_STATUS_OK;
}

mama_status
mamaFieldCacheList_free(mamaFieldCacheList list)
{
    list->mLast = -1;
    return mamaFieldCacheVector_free(list->mVector);
}

mama_status
mamaFieldCacheList_clear(mamaFieldCacheList list)
{
    list->mLast = -1;
    return mamaFieldCacheVector_clear(list->mVector);
}

mama_status
mamaFieldCacheList_add(mamaFieldCacheList list, void* item)
{
    mama_size_t vectorSize = 0;

    mamaFieldCacheVector_getSize(list->mVector, &vectorSize);
    ++list->mLast;
    if (list->mLast >= vectorSize)
    {
        mamaFieldCacheVector_grow(list->mVector, vectorSize + LIST_GROW_CHUNK_SIZE);
    }
    return mamaFieldCacheVector_set(list->mVector, list->mLast, item);
}

mama_status
mamaFieldCacheList_set(mamaFieldCacheList list, mama_size_t index, void* item)
{
    if (index > list->mLast)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    return mamaFieldCacheVector_set(list->mVector, index, item);
}

mama_status
mamaFieldCacheList_get(mamaFieldCacheList list, mama_size_t index, void** item)
{
    if (index > list->mLast)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    return mamaFieldCacheVector_get(list->mVector, index, item);
}

mama_status
mamaFieldCacheList_getSize(mamaFieldCacheList list, mama_size_t* size)
{
    *size = list->mLast + 1;
    return MAMA_STATUS_OK;
}
