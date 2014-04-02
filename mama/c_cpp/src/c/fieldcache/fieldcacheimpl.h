/* $Id: //commsof_depot/platform/products/6.0/trunk/mama/c_cpp/src/c/fieldcache/fieldcacheimpl.h#1 $
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

#ifndef MamaFieldCacheImplH__
#define MamaFieldCacheImplH__

#include <mama/types.h>
#include "fieldcachemap.h"
#include "fieldcachelist.h"
#include <wlock.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* This structure contains all of the types required to manage a field cache.
 */
typedef struct mamaFieldCacheImpl_
{   
    /* The map used to search and store entries in the cache. */
    mamaFieldCacheMap mMap;

    /* This iterator is used to iterate through a mama message whenever there
     * is no dictionary.
     */
    mamaMsgIterator mIterator;

    /* Lock object */
    wLock mLock;

    /* Size of the map */
    mama_size_t mSize;

    /* List of fields which must always be published in a delta msg */
    mamaFieldCacheList mAlwaysPublishFields;

    /* List of modified fields to be able to populate a delta msg without iterating
       the entire cache */
    mamaFieldCacheList mModifiedFields;

    /* Reusable mamaDateTime */
    mamaDateTime mReusableDateTime;

    /* Reusable mamaPrice */
    mamaPrice mReusablePrice;

    /* If this value is true, then the cache will keep track of modified fields */
    mama_bool_t mTrackModified;

    /* If this value is true, then the cache will use locking */
    mama_bool_t mUseLock;

    mama_bool_t mUseFieldNames;

    mama_bool_t mCachePayload;
    mamaMsg mCacheMsg;
} mamaFieldCacheImpl;

mama_status mamaFieldCache_updateCacheFromMsgField(mamaFieldCache fieldCache,
                                                   const mamaMsgField messageField);

mama_status mamaFieldCache_updateCacheFromField(mamaFieldCache fieldCache,
                                                const mamaFieldCacheField field);

mama_status mamaFieldCache_updateMsgField(mamaFieldCache fieldCache,
                                          mamaFieldCacheField field,
                                          mamaMsg message,
                                          mama_bool_t useUpdate);

#if defined(__cplusplus)
}
#endif

#endif /* MamaFieldCacheImplH__ */
