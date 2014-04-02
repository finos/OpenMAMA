/* $Id: //commsof_depot/platform/products/6.0/trunk/mama/c_cpp/src/c/fieldcache/fieldcachemaparray.c#1 $
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

#include "fieldcachemaparray.h"
#include "fieldcacheimpl.h"
#include "fieldcachefieldimpl.h"
#include <mama/fieldcache/fieldcachefield.h>
#include <stdlib.h>

static const mama_u16_t MAP_ARRAY_CHUNK_SIZE = 64;

static mama_fid_t mamaFieldCacheMapArray_convertFid(mama_fid_t fid)
{
    static mama_fid_t mFields[0xFFFF];
    static mama_u16_t mCounter = 0;

    mama_fid_t ret = mFields[fid];
    if (ret != 0)
    {
        return ret - 1;
    }

    mFields[fid] = ++mCounter;
    return mCounter - 1;
}

mama_status mamaFieldCacheMapArray_create(mamaFieldCacheMap* map)
{
    mamaFieldCacheMapArray arrayMap =
            (mamaFieldCacheMapArray)calloc(1, sizeof(mamaFieldCacheMapArrayImpl));

    arrayMap->mFieldArray =
            (mamaFieldCacheField*)calloc(MAP_ARRAY_CHUNK_SIZE, sizeof(mamaFieldCacheField));
    if (!arrayMap->mFieldArray)
    {
        free(arrayMap);
        return MAMA_STATUS_NOMEM;
    }
    arrayMap->mSize = MAP_ARRAY_CHUNK_SIZE;

    arrayMap->mBaseMap.mAddFunction = mamaFieldCacheMapArray_add;
    arrayMap->mBaseMap.mClearFunction = mamaFieldCacheMapArray_clear;
    arrayMap->mBaseMap.mDestroyFunction = mamaFieldCacheMapArray_destroy;
    arrayMap->mBaseMap.mFindFunction = mamaFieldCacheMapArray_find;

    *map = (mamaFieldCacheMap)arrayMap;

    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheMapArray_destroy(mamaFieldCacheMap map)
{
    int i;

    /* Cast the map to an array map. */
    mamaFieldCacheMapArray arrayMap = (mamaFieldCacheMapArray)map;

    for (i = 0; i < arrayMap->mSize; i++)
    {
        mamaFieldCacheField field = arrayMap->mFieldArray[i];
        if (field)
        {
            mamaFieldCacheField_destroy(field);
        }
    }

    free(arrayMap->mFieldArray);
    arrayMap->mFieldArray = NULL;

    free((mamaFieldCacheMapArray)map);

    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheMapArray_add(mamaFieldCacheMap map,
                                       mamaFieldCacheField field)
{
    mamaFieldCacheMapArray arrayMap = NULL;
    mama_fid_t cacheFid = 0;
    mama_u16_t newSize;
    mamaFieldCacheField* newArray = NULL;
    int i;

    /* Cast the map to an array map. */
    arrayMap = (mamaFieldCacheMapArray)map;

    /* Check to see if the entry has already been added. */
    cacheFid = mamaFieldCacheMapArray_convertFid(field->mFid);
    if(cacheFid < arrayMap->mSize && arrayMap->mFieldArray[cacheFid] != NULL)
    {
        return MAMA_STATUS_INVALID_ARG;
    }

    /* Resize the array, if needed */
    if (cacheFid >= arrayMap->mSize)
    {
        newSize = arrayMap->mSize;
        do
        {
            newSize += MAP_ARRAY_CHUNK_SIZE;
        }
        while (newSize <= cacheFid);
        newArray = (mamaFieldCacheField*)realloc(
                (void*)arrayMap->mFieldArray,
                newSize * sizeof(mamaFieldCacheField));
        if (!newArray)
        {
            return MAMA_STATUS_NOMEM;
        }
        for (i = arrayMap->mSize; i < newSize ; i++)
        {
            newArray[i] = NULL;
        }
        arrayMap->mFieldArray = newArray;
        arrayMap->mSize = newSize;
    }

    arrayMap->mFieldArray[cacheFid] = field;

    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheMapArray_find(mamaFieldCacheMap map,
                                        mama_fid_t fid,
                                        mamaFieldType type,
                                        const char* name,
                                        mamaFieldCacheField* field)
{
    mamaFieldCacheMapArray arrayMap = NULL;
    mama_fid_t cacheFid = 0;

    /* Cast the map to an array map. */
    arrayMap = (mamaFieldCacheMapArray)map;

    cacheFid = mamaFieldCacheMapArray_convertFid(fid);
    if (cacheFid >= arrayMap->mSize)
    {
        return MAMA_STATUS_NOT_FOUND;
    }
    /* Get the field from the array. */
    *field = arrayMap->mFieldArray[cacheFid];
    return (*field == NULL) ? MAMA_STATUS_NOT_FOUND : MAMA_STATUS_OK;
}

mama_status mamaFieldCacheMapArray_clear(mamaFieldCacheMap map)
{
    int i;

    /* Cast the map to an array map. */
    mamaFieldCacheMapArray arrayMap = (mamaFieldCacheMapArray)map;

    for (i = 0; i < arrayMap->mSize; i++)
    {
        mamaFieldCacheField field = arrayMap->mFieldArray[i];
        if (field)
        {
            mamaFieldCacheField_destroy(field);
            arrayMap->mFieldArray[i] = NULL;
        }
    }
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheMapIteratorArray_create(mamaFieldCacheIterator* iterator,
                                                  mamaFieldCacheMap map)
{
    mamaFieldCacheMapArray arrayMap = (mamaFieldCacheMapArray)map;

    mamaFieldCacheMapIteratorArray arrayIterator =
            (mamaFieldCacheMapIteratorArray)calloc(1, sizeof(mamaFieldCacheMapIteratorArrayImpl));

    arrayIterator->mBaseIterator.mDestroyFunction = mamaFieldCacheMapIteratorArray_destroy;
    arrayIterator->mBaseIterator.mFunctionNext = mamaFieldCacheMapIteratorArray_next;
    arrayIterator->mBaseIterator.mFunctionHasNext = mamaFieldCacheMapIteratorArray_hasNext;
    arrayIterator->mBaseIterator.mFunctionBegin = mamaFieldCacheMapIteratorArray_begin;

    arrayIterator->mFieldCacheMap = arrayMap;
    arrayIterator->mCurrentIndex = -1;

    *iterator = (mamaFieldCacheIterator)arrayIterator;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheMapIteratorArray_destroy(mamaFieldCacheIterator iterator)
{
    free((mamaFieldCacheMapIteratorArray)iterator);
    return MAMA_STATUS_OK;
}

mamaFieldCacheField mamaFieldCacheMapIteratorArray_next(mamaFieldCacheIterator iterator)
{
    mama_i32_t i;
    mamaFieldCacheField nextField = NULL;
    mamaFieldCacheMapIteratorArray arrayIterator =
            (mamaFieldCacheMapIteratorArray)iterator;
    mamaFieldCacheMapArray arrayMap = (mamaFieldCacheMapArray)arrayIterator->mFieldCacheMap;
    
    for (i = arrayIterator->mCurrentIndex + 1; i < arrayMap->mSize; i++)
    {
        nextField = arrayMap->mFieldArray[i];
        if (nextField != NULL)
        {
            arrayIterator->mCurrentIndex = i;
            return nextField;
        }
    }

    return NULL;
}

mama_bool_t mamaFieldCacheMapIteratorArray_hasNext(mamaFieldCacheIterator iterator)
{
    mama_i32_t i;
    mamaFieldCacheMapIteratorArray arrayIterator =
            (mamaFieldCacheMapIteratorArray)iterator;
    mamaFieldCacheMapArray arrayMap = (mamaFieldCacheMapArray)arrayIterator->mFieldCacheMap;

    for (i = arrayIterator->mCurrentIndex + 1; i < arrayMap->mSize; i++)
    {
        if (arrayMap->mFieldArray[i] != NULL)
            return 1;
    }

    return 0;
}

mamaFieldCacheField mamaFieldCacheMapIteratorArray_begin(mamaFieldCacheIterator iterator)
{
    mama_i32_t i;
    mamaFieldCacheField nextField = NULL;
    mamaFieldCacheMapIteratorArray arrayIterator =
            (mamaFieldCacheMapIteratorArray)iterator;
    mamaFieldCacheMapArray arrayMap = (mamaFieldCacheMapArray)arrayIterator->mFieldCacheMap;

    for (i = 0; i < arrayMap->mSize; i++)
    {
        nextField = arrayMap->mFieldArray[i];
        if (nextField != NULL)
        {
            return nextField;
        }
    }

    return NULL;
}
