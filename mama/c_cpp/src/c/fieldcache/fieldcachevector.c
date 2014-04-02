/* $Id: //commsof_depot/platform/products/6.0/trunk/mama/c_cpp/src/c/fieldcache/fieldcachevector.c#1 $
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

#include "fieldcachevector.h"

mama_status
mamaFieldCacheVector_create(mamaFieldCacheVector* vector)
{
    *vector = (mamaFieldCacheVector)calloc(1, sizeof(mamaFieldCacheVectorImpl));
    return MAMA_STATUS_OK;
}

mama_status
mamaFieldCacheVector_destroy(mamaFieldCacheVector vector)
{
    if (vector->mData)
    {
        free(vector->mData);
    }
    free(vector);
    return MAMA_STATUS_OK;
}

mama_status
mamaFieldCacheVector_free(mamaFieldCacheVector vector)
{
    int i;

    if (!vector->mData)
        return MAMA_STATUS_OK;

    for (i = 0; i < vector->mSize; i++)
    {
        if (vector->mData[i])
            free(vector->mData[i]);
    }
    return MAMA_STATUS_OK;
}

mama_status
mamaFieldCacheVector_clear(mamaFieldCacheVector vector)
{
    int i;

    if (!vector->mData)
        return MAMA_STATUS_OK;

    for (i = 0; i < vector->mSize; i++)
    {
        vector->mData[i] = NULL;
    }
    return MAMA_STATUS_OK;
}

mama_status
mamaFieldCacheVector_set(mamaFieldCacheVector vector, mama_size_t index, void* item)
{
    if (index >= vector->mSize)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    vector->mData[index] = item;
    return MAMA_STATUS_OK;
}

mama_status
mamaFieldCacheVector_get(mamaFieldCacheVector vector, mama_size_t index, void** item)
{
    if (index >= vector->mSize)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    *item = vector->mData[index];
    return MAMA_STATUS_OK;
}

mama_status
mamaFieldCacheVector_getSize(mamaFieldCacheVector vector, mama_size_t* size)
{
    *size = vector->mSize;
    return MAMA_STATUS_OK;
}

mama_status
mamaFieldCacheVector_grow(mamaFieldCacheVector vector, mama_size_t newAllocSize)
{
    int i;
    
    if (vector->mSize > newAllocSize)
    {
        return MAMA_STATUS_OK;
    }
    if (!vector->mData)
    {
        vector->mData = (void**)
                malloc(sizeof(void*)*newAllocSize);
    }
    else
    {
        vector->mData = (void**)
                realloc((void*)vector->mData, sizeof(void*)*newAllocSize);
    }
    for (i = vector->mSize; i < newAllocSize; i++)
    {
        vector->mData[i] = NULL;
    }
    vector->mSize = newAllocSize;
    return MAMA_STATUS_OK;
}
