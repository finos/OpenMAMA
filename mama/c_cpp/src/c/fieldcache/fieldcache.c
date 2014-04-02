/* $Id: //commsof_depot/platform/products/6.0/trunk/mama/c_cpp/src/c/fieldcache/fieldcache.c#1 $
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

#include "fieldcacheimpl.h"
#include "fieldcachefieldimpl.h"
#include "fieldcacherecordimpl.h"
#include <mamainternal.h>
#include <mama/fieldcache/fieldcache.h>
#include <mama/fieldcache/fieldcachefield.h>
#include <mama/fieldcache/fieldcacheiterator.h>
#include <mama/fieldcache/fieldcacherecord.h>
#include <mama/price.h>
#include <mama/datetime.h>
#include <mama/msg.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#define FIELD_CACHE_LOCK(cache) if (cache->mUseLock) { wlock_lock(cache->mLock); }
#define FIELD_CACHE_UNLOCK(cache) if (cache->mUseLock) { wlock_unlock(cache->mLock); }

mama_status mamaFieldCache_create(mamaFieldCache* fieldCache)
{
    mama_status ret = MAMA_STATUS_OK;
    mamaFieldCache localCache = NULL;
    const char * propstring = NULL;

    if(!fieldCache)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    
    localCache = (mamaFieldCache)calloc(1, sizeof(mamaFieldCacheImpl));
    if(!localCache)
    {
        return MAMA_STATUS_NOMEM;
    }
    
    localCache->mLock = wlock_create();

    propstring = properties_Get (mamaInternal_getProperties (), "mama.fieldcache.type");
    if (propstring != NULL && strcmp (propstring, "payload") == 0)
    {
        localCache->mCachePayload = 1;
    }
    else
    {
        ret = mamaFieldCacheMap_create(&localCache->mMap);
        if(ret != MAMA_STATUS_OK)
        {
            mamaFieldCache_destroy(localCache);
            return ret;
        }

        mamaFieldCacheList_create(&localCache->mAlwaysPublishFields);
        mamaFieldCacheList_create(&localCache->mModifiedFields);

        mamaDateTime_create(&localCache->mReusableDateTime);
        mamaPrice_create(&localCache->mReusablePrice);
        localCache->mTrackModified = 1;
    }

    /* Write back the field cache pointer */
    *fieldCache = localCache;

    return ret;
}

mama_status mamaFieldCache_destroy(mamaFieldCache fieldCache)
{
    if (!fieldCache)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    if (fieldCache->mCachePayload)
    {
        if (fieldCache->mCacheMsg)
        {
            mamaMsg_destroy(fieldCache->mCacheMsg);
        }
    }
    else
    {
        mamaFieldCacheMap_destroy(fieldCache->mMap);
        if (fieldCache->mIterator)
        {
            mamaMsgIterator_destroy(fieldCache->mIterator);
        }
        mamaFieldCacheList_destroy(fieldCache->mAlwaysPublishFields);
        mamaFieldCacheList_destroy(fieldCache->mModifiedFields);

        mamaDateTime_destroy(fieldCache->mReusableDateTime);
        mamaPrice_destroy(fieldCache->mReusablePrice);
    }

    wlock_destroy(fieldCache->mLock);

    free(fieldCache);

    return MAMA_STATUS_OK;
}


mama_status mamaFieldCache_clear(mamaFieldCache fieldCache)
{
    if (!fieldCache)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    FIELD_CACHE_LOCK(fieldCache);
    /* TODO: should we implement this function using mamaMsg methods??? */
    if (fieldCache->mCachePayload)
    {
         if (fieldCache->mCacheMsg)
             mamaMsg_clear(fieldCache->mCacheMsg);
    }
    else
    {
        mamaFieldCacheMap_clear(fieldCache->mMap);
        mamaFieldCacheList_clear(fieldCache->mAlwaysPublishFields);
        mamaFieldCacheList_clear(fieldCache->mModifiedFields);
        fieldCache->mSize = 0;
    }
    FIELD_CACHE_UNLOCK(fieldCache);
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCache_getSize(mamaFieldCache fieldCache, mama_size_t* size)
{
    if (!fieldCache || !size)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    *size = fieldCache->mSize;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCache_find(
        mamaFieldCache fieldCache,
        mama_fid_t fid,
        const char* name,
        mamaFieldCacheField* field)
{
    mama_status ret;
    if (!fieldCache || !field)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (fieldCache->mCachePayload)
    {
        return MAMA_STATUS_NOT_IMPLEMENTED;
    }
    FIELD_CACHE_LOCK(fieldCache);
    ret = mamaFieldCacheMap_find(fieldCache->mMap,
                                 fid,
                                 MAMA_FIELD_TYPE_UNKNOWN,
                                 name,
                                 field);
    FIELD_CACHE_UNLOCK(fieldCache);
    return ret;
}

mama_status mamaFieldCache_findOrAdd(
        mamaFieldCache fieldCache,
        mama_fid_t fid,
        mamaFieldType type,
        const char* name,
        mamaFieldCacheField* field,
        mama_bool_t* existing)
{
    mama_status ret = MAMA_STATUS_OK;

    if (!fieldCache || !field || !existing)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (fieldCache->mCachePayload)
    {
        return MAMA_STATUS_NOT_IMPLEMENTED;
    }

    FIELD_CACHE_LOCK(fieldCache);
    ret = mamaFieldCacheMap_find(fieldCache->mMap, fid, type, name, field);
    if (ret == MAMA_STATUS_OK && *field)
    {
        *existing = 1;
        FIELD_CACHE_UNLOCK(fieldCache);
        return ret;
    }
    *existing = 0;
    ret = mamaFieldCacheField_create(field, fid, type, name);
    if (ret != MAMA_STATUS_OK)
    {
        FIELD_CACHE_UNLOCK(fieldCache);
        return ret;
    }
    ret = mamaFieldCacheMap_add(fieldCache->mMap, *field);
    if (ret == MAMA_STATUS_OK)
    {
        fieldCache->mSize++;
    }
    FIELD_CACHE_UNLOCK(fieldCache);
    return ret;
}

/* There is no check that field is actually present in the field cache */
mama_status mamaFieldCache_setModified(mamaFieldCache fieldCache,
                                       mamaFieldCacheField field)
{
    if (!fieldCache || !field)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (fieldCache->mTrackModified == 0 || field->mIsModified)
    {
        return MAMA_STATUS_OK;
    }

    field->mIsModified = 1;
    mamaFieldCacheList_add(fieldCache->mModifiedFields, (void*)field);
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCache_applyField(mamaFieldCache fieldCache,
                                      const mamaFieldCacheField field)
{
    mama_status ret = MAMA_STATUS_OK;
    if (!fieldCache || !field)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (fieldCache->mCachePayload)
    {
        return MAMA_STATUS_NOT_IMPLEMENTED;
    }
    FIELD_CACHE_LOCK(fieldCache);
    ret = mamaFieldCache_updateCacheFromField(fieldCache, field);
    FIELD_CACHE_UNLOCK(fieldCache);
    return ret;
}

mama_status mamaFieldCache_applyMessage(
        mamaFieldCache fieldCache,
        const mamaMsg message,
        mamaDictionary dictionary)
{
    mama_status ret = MAMA_STATUS_OK;
    mamaMsgField nextField = NULL;

    if (!fieldCache || !message)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    if (fieldCache->mCachePayload)
    {
        if (!fieldCache->mCacheMsg)
        {
	   		ret=mamaMsg_copy(message, &fieldCache->mCacheMsg);
        }
		else
		{
        	ret=mamaMsg_applyMsg(fieldCache->mCacheMsg, message);
		
		}	
        return ret;
    }

    if(!fieldCache->mIterator)
    {
        ret = mamaMsgIterator_create(&fieldCache->mIterator, dictionary);
        if(ret != MAMA_STATUS_OK)
        {
            return ret;
        }
    }

    ret = mamaMsgIterator_associate(fieldCache->mIterator, message);
    if(ret != MAMA_STATUS_OK)
    {
        return ret;
    }
    FIELD_CACHE_LOCK(fieldCache);
    nextField = mamaMsgIterator_next(fieldCache->mIterator);
    while(nextField && ret == MAMA_STATUS_OK)
    {
        ret = mamaFieldCache_updateCacheFromMsgField(fieldCache, nextField);
        nextField = mamaMsgIterator_next(fieldCache->mIterator);
    }
    FIELD_CACHE_UNLOCK(fieldCache);

    return ret;
}

mama_status mamaFieldCache_applyRecord(
        mamaFieldCache fieldCache,
        const mamaFieldCacheRecord record)
{
    mama_status ret = MAMA_STATUS_OK;
    mamaFieldCacheField newField;
    mama_size_t size = 0;
    mama_size_t counter;

    if (!fieldCache || !record)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (fieldCache->mCachePayload)
    {
        return MAMA_STATUS_NOT_IMPLEMENTED;
    }

    mamaFieldCacheRecord_getSize(record, &size);
    for(counter = 0; counter < size; counter++)
    {
        /* Reset output parameter, because if it is not found it is not overwritten */
        newField = NULL;
        mamaFieldCacheRecord_getField(record, counter, &newField);
        if (newField)
        {
            ret = mamaFieldCache_updateCacheFromField(fieldCache, newField);
        }
    }

    return ret;
}

mama_status mamaFieldCache_getFullMessage(
        mamaFieldCache fieldCache,
        mamaMsg message)
{
    mama_bool_t publish;
    mamaFieldCacheField field = NULL;
    mamaFieldCacheIterator iterator = NULL;
    mama_size_t numFields = 0;

    if (!fieldCache || !message)
    {
        return MAMA_STATUS_NULL_ARG;
    }
   
    if (fieldCache->mCacheMsg)
    {
        return mamaMsg_copy (fieldCache->mCacheMsg, &message);
    }

    mamaMsg_getNumFields(message, &numFields);
    mamaFieldCacheIterator_create(&iterator, fieldCache);    
    FIELD_CACHE_LOCK(fieldCache);
    while (mamaFieldCacheIterator_hasNext(iterator))
    {
        field = mamaFieldCacheIterator_next(iterator);
        mamaFieldCacheField_getPublish(field, &publish);
        if (publish)
        {
            mamaFieldCache_updateMsgField(fieldCache, field, message, numFields);
        }
    }
    FIELD_CACHE_UNLOCK(fieldCache);
    mamaFieldCacheIterator_destroy(iterator);
    
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCache_getDeltaMessage(
        mamaFieldCache fieldCache,
        mamaMsg message)
{
    mamaFieldCacheField field = NULL;
    mama_size_t numFields = 0;
    mama_size_t numMsgFields = 0;
    mama_size_t i;

    if (!fieldCache || !message)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (!fieldCache->mTrackModified)
    {
        return mamaFieldCache_getFullMessage(fieldCache, message);
    }

    mamaMsg_getNumFields(message, &numMsgFields);

    FIELD_CACHE_LOCK(fieldCache);
    mamaFieldCacheList_getSize(fieldCache->mAlwaysPublishFields, &numFields);
    for (i = 0; i < numFields; ++i)
    {
        void* tmpField = NULL;
        mamaFieldCacheList_get(fieldCache->mAlwaysPublishFields, i, &tmpField);
        assert(tmpField);
        field = (mamaFieldCacheField)tmpField;
        mamaFieldCache_updateMsgField(fieldCache, field, message, numMsgFields);
        field->mIsModified = 0;
    }

    mamaFieldCacheList_getSize(fieldCache->mModifiedFields, &numFields);
    for (i = 0; i < numFields; ++i)
    {
        void* tmpField = NULL;
        mamaFieldCacheList_get(fieldCache->mModifiedFields, i, &tmpField);
        assert(tmpField);
        field = (mamaFieldCacheField)tmpField;
        mamaFieldCache_updateMsgField(fieldCache, field, message, numMsgFields);
        field->mIsModified = 0;
    }
    mamaFieldCacheList_clear(fieldCache->mModifiedFields);
    FIELD_CACHE_UNLOCK(fieldCache);

    return MAMA_STATUS_OK;
}

mama_status
mamaFieldCache_clearModifiedFields(mamaFieldCache fieldCache)
{
    mamaFieldCacheField field = NULL;
    mama_size_t numFields = 0;
    mama_size_t i;

    if (!fieldCache)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (!fieldCache->mTrackModified)
    {
        return MAMA_STATUS_OK;
    }

    mamaFieldCacheList_getSize(fieldCache->mModifiedFields, &numFields);
    for (i = 0; i < numFields; ++i)
    {
        void* tmpField = NULL;
        mamaFieldCacheList_get(fieldCache->mModifiedFields, i, &tmpField);
        assert(tmpField);
        field = (mamaFieldCacheField)tmpField;
        field->mIsModified = 0;
    }
    mamaFieldCacheList_clear(fieldCache->mModifiedFields);

    return MAMA_STATUS_OK;
}

mama_status mamaFieldCache_setTrackModified(
    mamaFieldCache fieldCache, 
    mama_bool_t trackModified)
{
    if (!fieldCache)
    {
        return MAMA_STATUS_NULL_ARG;
    }
	if (!fieldCache->mCachePayload)
    {
    	fieldCache->mTrackModified = trackModified;
    }
	return MAMA_STATUS_OK;
}

mama_status mamaFieldCache_getTrackModified(
    mamaFieldCache fieldCache, 
    mama_bool_t* trackModified)
{
    if (!fieldCache || !trackModified)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    *trackModified = fieldCache->mTrackModified;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCache_setUseFieldNames(
        mamaFieldCache fieldCache, 
        mama_bool_t useFieldNames)
{
    if (!fieldCache)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    fieldCache->mUseFieldNames = useFieldNames;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCache_getUseFieldNames(
        mamaFieldCache fieldCache, 
        mama_bool_t* useFieldNames)
{
    if (!fieldCache || !useFieldNames)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    *useFieldNames = fieldCache->mUseFieldNames;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCache_setUseLock(mamaFieldCache fieldCache, mama_bool_t enable)
{
    if (!fieldCache)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    fieldCache->mUseLock = enable;
    return MAMA_STATUS_OK;
}

mama_status
mamaFieldCache_getUseLock(mamaFieldCache fieldCache, mama_bool_t* lockEnabled)
{
    if (!fieldCache || !lockEnabled)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    *lockEnabled = fieldCache->mUseLock;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCache_lock(mamaFieldCache fieldCache)
{
    if (!fieldCache)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    FIELD_CACHE_LOCK(fieldCache);
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCache_unlock(mamaFieldCache fieldCache)
{
    if (!fieldCache)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    FIELD_CACHE_UNLOCK(fieldCache);
    return MAMA_STATUS_OK;
}
