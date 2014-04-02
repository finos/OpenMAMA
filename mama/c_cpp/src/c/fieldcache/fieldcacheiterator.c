/* $Id: //commsof_depot/platform/products/6.0/trunk/mama/c_cpp/src/c/fieldcache/fieldcacheiterator.c#1 $
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

#include "fieldcacheiteratorimpl.h"
#include "fieldcachemaparray.h"
#include "fieldcacheimpl.h"

mama_status mamaFieldCacheIterator_create(mamaFieldCacheIterator* iterator,
                                          mamaFieldCache fieldCache)
{
    if (!iterator || !fieldCache)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    switch (gMamaFieldCacheMapType)
    {
    case MAMAFIELDCACHE_MAP_MODE_ARRAY:
        return mamaFieldCacheMapIteratorArray_create(iterator, fieldCache->mMap);
    default:
        break;
    }
    return MAMA_STATUS_INVALID_ARG;
}

mama_status mamaFieldCacheIterator_destroy(mamaFieldCacheIterator iterator)
{
    if (!iterator)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    return iterator->mDestroyFunction(iterator);
}

mamaFieldCacheField mamaFieldCacheIterator_next(mamaFieldCacheIterator iterator)
{
    if (!iterator)
    {
        return NULL;
    }
    return iterator->mFunctionNext(iterator);
}

mama_bool_t mamaFieldCacheIterator_hasNext(mamaFieldCacheIterator iterator)
{
    if (!iterator)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    return iterator->mFunctionHasNext(iterator);
}

mamaFieldCacheField mamaFieldCacheIterator_begin(mamaFieldCacheIterator iterator)
{
    if (!iterator)
    {
        return NULL;
    }
    return iterator->mFunctionBegin(iterator);
}
