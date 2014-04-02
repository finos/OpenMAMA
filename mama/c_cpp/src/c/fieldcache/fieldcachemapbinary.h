/* $Id: //commsof_depot/platform/products/6.0/trunk/mama/c_cpp/src/c/fieldcache/fieldcachemapbinary.h#1 $
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

#ifndef MamaFieldCacheMapBinaryH__
#define MamaFieldCacheMapBinaryH__

#include "fieldcachemap.h"

#if defined( __cplusplus )
extern "C" {
#endif /* defined( __cplusplus ) */

typedef struct mamaFieldCacheMapBinaryImpl_
{
    /* The base structure must appear first. */
    mamaFieldCacheMapImpl mBaseMap;

} mamaFieldCacheMapBinaryImpl;

typedef struct mamaFieldCacheMapBinaryImpl_* mamaFieldCacheMapBinary;

/* Map */
mama_status mamaFieldCacheMapBinary_create(mamaFieldCacheMap* map);

mama_status mamaFieldCacheMapBinary_destroy(mamaFieldCacheMap map);

mama_status mamaFieldCacheMapBinary_add(mamaFieldCacheMap map,
                                        mamaFieldCacheField field);

mama_status mamaFieldCacheMapBinary_find(mamaFieldCacheMap map,
                                         mama_fid_t fid,
                                         mamaFieldType type,
                                         const char* name,
                                         mamaFieldCacheField* field);

mama_status mamaFieldCacheMapBinary_clear(mamaFieldCacheMap map);

/* Iterator */
mama_status mamaFieldCacheMapIteratorBinary_create(mamaFieldCacheIterator* iterator,
                                                   mamaFieldCacheMap map);

mama_status mamaFieldCacheMapIteratorBinary_destroy(mamaFieldCacheIterator iterator);

mamaFieldCacheField mamaFieldCacheMapIteratorBinary_next(mamaFieldCacheIterator iterator);

mama_bool_t mamaFieldCacheMapIteratorBinary_hasNext(mamaFieldCacheIterator iterator);

mamaFieldCacheField mamaFieldCacheMapIteratorBinary_begin(mamaFieldCacheIterator iterator);

#if defined( __cplusplus )
}
#endif /* defined( __cplusplus ) */

#endif /* MamaFieldCacheMapBinaryH__ */
