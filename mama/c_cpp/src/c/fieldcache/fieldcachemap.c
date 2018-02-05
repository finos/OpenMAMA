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

#include "fieldcachemap.h"
#include "fieldcachemaparray.h"
#include <assert.h>

const mamaFieldCacheMapType gMamaFieldCacheMapType = MAMAFIELDCACHE_MAP_MODE_ARRAY;

mama_status mamaFieldCacheMap_create(mamaFieldCacheMap* map)
{
    mamaFieldCacheMap localMap = NULL;
    mama_status ret = MAMA_STATUS_NULL_ARG;

    assert(map);
    ret = MAMA_STATUS_INVALID_ARG;
    switch(gMamaFieldCacheMapType)
    {
        case MAMAFIELDCACHE_MAP_MODE_ARRAY:
            ret = mamaFieldCacheMapArray_create(&localMap);
            break;
        case MAMAFIELDCACHE_MAP_MODE_BINARY:
            break;
        default:
            break;
    }

    /* Return the map. */
    *map = localMap;

    return ret;
}

mama_status mamaFieldCacheMap_destroy(mamaFieldCacheMap map)
{
    assert(map);
    return map->mDestroyFunction(map);
}

mama_status
mamaFieldCacheMap_add(mamaFieldCacheMap map,mamaFieldCacheField field)
{
    assert(map);
    return map->mAddFunction(map, field);
}

mama_status
mamaFieldCacheMap_find(mamaFieldCacheMap map,
                       mama_fid_t fid,
                       mamaFieldType type,
                       const char* name,
                       mamaFieldCacheField* field)
{
    assert(map);
    return map->mFindFunction(map, fid, type, name, (mamaFieldCacheField*)field);
}

mama_status
mamaFieldCacheMap_clear(mamaFieldCacheMap map)
{
    assert(map);
    return map->mClearFunction(map);
}
