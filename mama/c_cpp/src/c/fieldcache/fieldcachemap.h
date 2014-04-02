/* $Id: //commsof_depot/platform/products/6.0/trunk/mama/c_cpp/src/c/fieldcache/fieldcachemap.h#1 $
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

#ifndef MamaFieldMapH__
#define MamaFieldMapH__

#include <mama/config.h>
#include <mama/status.h>
#include <mama/types.h>
#include <mama/fielddesc.h>
#include <mama/fieldcache/fieldcachetypes.h>

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */
    
struct mamaFieldCacheMapImpl_;
typedef struct mamaFieldCacheMapImpl_ mamaFieldCacheMapImpl;
typedef mamaFieldCacheMapImpl* mamaFieldCacheMap;


/* The following defines are function pointers for use in the FieldCacheMap
 * structure.
 */
/* This function is used to destroy a search map. */
typedef mama_status (* fieldCacheMap_destroy)(mamaFieldCacheMap);

typedef mama_status (* fieldCacheMap_add)(
        mamaFieldCacheMap,
        mamaFieldCacheField);
/* This function is used to find fields in a search map. */
typedef mama_status (* fieldCacheMap_find)(
        mamaFieldCacheMap,
        mama_fid_t,
        mamaFieldType,
        const char*,
        mamaFieldCacheField*);

/* This function is used to clear a search map. */
typedef mama_status (* fieldCacheMap_clear)(mamaFieldCacheMap);


/* The search map contains function pointers and members common to the various 
 * search strategies.
 */
struct mamaFieldCacheMapImpl_
{
    /* This function is used to clear a map. */
    fieldCacheMap_clear mClearFunction;

    /* This function is used to destroy a map. */
    fieldCacheMap_destroy mDestroyFunction;

    /* This function is used to add new fields to a map. */
    fieldCacheMap_add mAddFunction;

    /* This function is used to find fields in a map. */
    fieldCacheMap_find mFindFunction;
};

typedef enum
{
    MAMAFIELDCACHE_MAP_MODE_ARRAY,
    MAMAFIELDCACHE_MAP_MODE_BINARY
} mamaFieldCacheMapType;

extern const mamaFieldCacheMapType gMamaFieldCacheMapType;
extern const mama_bool_t gUseMamaFieldCacheMapMonitor;
        
mama_status
mamaFieldCacheMap_create(mamaFieldCacheMap* map);

mama_status
mamaFieldCacheMap_destroy(mamaFieldCacheMap map);

mama_status
mamaFieldCacheMap_add(mamaFieldCacheMap map,
                      mamaFieldCacheField field);

mama_status
mamaFieldCacheMap_find(mamaFieldCacheMap map,
                       mama_fid_t fid,
                       mamaFieldType type,
                       const char* name,
                       mamaFieldCacheField* field);

mama_status
mamaFieldCacheMap_clear(mamaFieldCacheMap map);


#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

#endif /* MamaFieldMapH__ */
