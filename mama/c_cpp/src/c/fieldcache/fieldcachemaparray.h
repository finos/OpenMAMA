/* $Id: //commsof_depot/platform/products/6.0/trunk/mama/c_cpp/src/c/fieldcache/fieldcachemaparray.h#1 $
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

#ifndef MamaFieldCacheMapArrayH__
#define MamaFieldCacheMapArrayH__

#include "fieldcachemap.h"
#include "fieldcacheiteratorimpl.h"

#if defined( __cplusplus )
extern "C" {
#endif /* defined( __cplusplus ) */

typedef struct mamaFieldCacheMapArrayImpl_
{
    /* The base structure must appear first. */
    mamaFieldCacheMapImpl mBaseMap;
    
    /* This array contains entries for the fields. */
    mamaFieldCacheField* mFieldArray;

    /* The size of the array. */
    mama_u16_t mSize;
} mamaFieldCacheMapArrayImpl;

typedef mamaFieldCacheMapArrayImpl* mamaFieldCacheMapArray;


typedef struct mamaFieldCacheMapIteratorArrayImpl_
{
    mamaFieldCacheIteratorImpl mBaseIterator;
    mamaFieldCacheMapArray mFieldCacheMap;
    mama_i32_t mCurrentIndex;
} mamaFieldCacheMapIteratorArrayImpl;

typedef mamaFieldCacheMapIteratorArrayImpl* mamaFieldCacheMapIteratorArray;


/* Map */
mama_status mamaFieldCacheMapArray_create(mamaFieldCacheMap* map);

mama_status mamaFieldCacheMapArray_destroy(mamaFieldCacheMap map);

mama_status mamaFieldCacheMapArray_add(mamaFieldCacheMap map,
                                       mamaFieldCacheField field);

mama_status mamaFieldCacheMapArray_find(mamaFieldCacheMap map,
                                        mama_fid_t fid,
                                        mamaFieldType type,
                                        const char* name,
                                        mamaFieldCacheField* field);

mama_status mamaFieldCacheMapArray_clear(mamaFieldCacheMap map);


/* Iterator */
mama_status mamaFieldCacheMapIteratorArray_create(mamaFieldCacheIterator* iterator,
                                                  mamaFieldCacheMap map);

mama_status mamaFieldCacheMapIteratorArray_destroy(mamaFieldCacheIterator iterator);

mamaFieldCacheField mamaFieldCacheMapIteratorArray_next(mamaFieldCacheIterator iterator);

mama_bool_t mamaFieldCacheMapIteratorArray_hasNext(mamaFieldCacheIterator iterator);

mamaFieldCacheField mamaFieldCacheMapIteratorArray_begin(mamaFieldCacheIterator iterator);

#if defined( __cplusplus )
}
#endif /* defined( __cplusplus ) */

#endif /* MamaFieldCacheMapArrayH__ */
