/* $Id: //commsof_depot/platform/products/6.0/trunk/mama/c_cpp/src/c/fieldcache/fieldcache.h#1 $
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

#ifndef MamaFieldCacheH__
#define MamaFieldCacheH__

#include <mama/config.h>
#include <mama/fielddesc.h>
#include <mama/status.h>
#include <mama/types.h>
#include <mama/fieldcache/fieldcachetypes.h>

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * This function will create the field cache and allocate all associated memory.
 * If this function succeeds then the cache should be destroyed by calling
 * mamaFieldCache_destroy.
 *
 * @param fieldCache (out) To return the field cache.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NOMEM
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCache_create(mamaFieldCache* fieldCache);

/**
 * This function will destroy a field cache previously allocated by a call to
 * mamaFieldCache_create.
 *
 * @param fieldCache (in) The field cache to destroy.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCache_destroy(mamaFieldCache fieldCache);

/**
 * This function will clear the field cache. The result is that the cache will
 * be empty (no fields).
 *
 * @param fieldCache (in) The field cache to clear.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCache_clear(mamaFieldCache fieldCache);

/**
 * This function will return the number of fields in the cache.
 * 
 * @param fieldCache (in) The field cache.
 * @param size (out) The number of fields in the cache.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCache_getSize(mamaFieldCache fieldCache, mama_size_t* size);

/**
 * This function will retrieve a field from the cache. If the field is not found,
 * the output parameter will be set to NULL and MAMA_STATUS_NOT_FOUND is returned.
 *
 * @param fieldCache (in) The field cache.
 * @param fid (in) The field id to look up.
 * @param name (in) The field name.
 * @param field (out) The field.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_NOT_FOUND
 *      MAMA_STATUS_OK
 *      Other error codes from the mama functions.
 */
MAMAExpDLL
extern mama_status
mamaFieldCache_find(mamaFieldCache fieldCache,
                    mama_fid_t fid,
                    const char* name,
                    mamaFieldCacheField* field);

/**
 * This function will retrieve a field from the cache. If it does not exist, a 
 * new one is created, added to the cache and returned.
 *
 * @param fieldCache (in) The field cache.
 * @param fid (in) The field id to look up.
 * @param type (in) The field type.
 * @param name (in) The field name.
 * @param field (out) The existing field, if found or new one, if not found.
 * @param existing (out) Flag to indicate whether the field was already present in the cache.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_NOMEM
 *      MAMA_STATUS_OK
 *      Other error codes from the mama functions.
 */
MAMAExpDLL
extern mama_status
mamaFieldCache_findOrAdd(mamaFieldCache fieldCache,
                         mama_fid_t fid,
                         mamaFieldType type,
                         const char* name,
                         mamaFieldCacheField* field,
                         mama_bool_t* existing);

/**
 * This function will set the modified flag for a field in the cache even if the
 * value has not changed.
 * The field must be present in the cache (No automatic check is performed).
 *
 * @param fieldCache (in) The field cache.
 * @param field (in) The field to set to modified.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCache_setModified(mamaFieldCache fieldCache,
                           mamaFieldCacheField field);

/**
 * Update the cache content with the field given.
 * If the field is already present, it will be updated, otherwise it will be
 * added to the cache.
 * 
 * @param fieldCache (in) The field cache.
 * @param field (in) The field to get the content to update in the cache.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCache_applyField(mamaFieldCache fieldCache,
                          const mamaFieldCacheField field);
/*
 * This function will automatically populate the field cache from the contents
 * of the supplied message. Fields within the messages will be added to the cache
 * if not already present. If the field is already present then the data will be
 * updated.
 *
 * @param fieldCache (in) The field cache to populate.
 * @param message (in) The <code>mamaMsg</code> to use to populate the cache.
 * @param dictionary (in) The dictionary (optional: can be NULL).
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG  
 *      MAMA_STATUS_OK
 *      Other error codes from the mama functions. 
 */
MAMAExpDLL
extern mama_status
mamaFieldCache_applyMessage(mamaFieldCache fieldCache,
                            const mamaMsg message,
                            mamaDictionary dictionary);

/*
 * This function will automatically populate the field cache from the contents
 * of the supplied mamaFieldCacheRecord. Fields within the record will be added
 * to the cache if not already present. If the field is already present then the
 * data will be updated.
 *
 * @param fieldCache (in) The field cache to populate.
 * @param record (in) The <code>mamaFieldCacheRecord</code> to use to populate the cache.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 *      Other error codes from the mama functions.
 */
MAMAExpDLL
extern mama_status
mamaFieldCache_applyRecord(mamaFieldCache fieldCache,
                           const mamaFieldCacheRecord record);

/**
 * This function will return all the fields in the cache by updating the provided
 * mamaMsg with the fields.
 * With this function the provided message will be fully populated with all the
 * fields in the cache.
 *
 * @param fieldCache (in) The field cache to obtain the fields from.
 * @param message (in) This <code>mamaMsg</code> will be populated with all the fields. 
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG 
 *      MAMA_STATUS_OK
 *      Other mama return codes.
 */
MAMAExpDLL
extern mama_status
mamaFieldCache_getFullMessage(mamaFieldCache fieldCache,mamaMsg message);

/**
 * This function will return the set of fields that have changed since the last time
 * this function was called.
 * With this function the provided message will be fully populated with only the
 * modified fields in the cache.
 *
 * @param fieldCache (in) The field cache to obtain the modified fields from.
 * @param message (in) This <code>mamMsg</code> will be populated with the modified fields. 
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_OK
 *      Other mama return codes.
 */
MAMAExpDLL
extern mama_status
mamaFieldCache_getDeltaMessage(mamaFieldCache fieldCache, mamaMsg message);

/**
 * This function will clear the list of all the modified fields and reset the modified flags
 * for all the modified fields in the cache.
 *
 * @param fieldCache (in) The field cache.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCache_clearModifiedFields(mamaFieldCache fieldCache);


/**
 * This function will set the value of the track modification state flag; if this
 * flag is set then changes to field values will be monitored. See the documentation
 * for more information on the operation of the track modification state flag.
 *
 * @param fieldCache (in) The field cache.
 * @param trackModified (in) The track mod state flag, this should be non-zero
 *                           to turn tracking on.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCache_setTrackModified(mamaFieldCache fieldCache, mama_bool_t trackModified);

/**
 * This function will return the value of the track modification state flag.
 *
 * @param fieldCache (in) The field cache.
 * @param trackModified (out) To return the track mod state flag, this will be
 *                            non-zero if tracking is turned on.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status 
mamaFieldCache_getTrackModified(mamaFieldCache fieldCache, mama_bool_t *trackModified);

/**
 * This function will set the value of the use-field-names state flag; if this
 * flag is set then... See the documentation
 * for more information on the operation of the use-field-names state flag.
 *
 * @param fieldCache (in) The field cache.
 * @param useFieldNames (in) The use field names state flag, this should be non-zero
 *                           to turn tracking on.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCache_setUseFieldNames(mamaFieldCache fieldCache, mama_bool_t useFieldNames);

/**
 * This function will return the value of the use-field-names state flag.
 *
 * @param fieldCache (in) The field cache.
 * @param useFieldNames (out) To return the use-field-names state flag, this will
 *                            be non-zero if use-field-names is turned on.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status 
mamaFieldCache_getUseFieldNames(mamaFieldCache fieldCache, mama_bool_t *useFieldNames);

/**
 * This function will enable/disable cache locking.
 *
 * @param fieldCache (in) The field cache.
 * @param enable (in) The flag for enabling/disabling locking.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCache_setUseLock(mamaFieldCache fieldCache, mama_bool_t enable);

/**
 * This function will determine whether the cache is using locking or not.
 *
 * @param fieldCache (in) The field cache.
 * @param locked (out) Indicates whether the cache is using locking or not.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCache_getUseLock(mamaFieldCache fieldCache, mama_bool_t* lockEnabled);

/**
 * This function will lock the cache.
 *
 * @param fieldCache (in) The field cache to lock.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCache_lock(mamaFieldCache fieldCache);

/**
 * This function will unlock the cache.
 *
 * @param fieldCache (in) The field cache to unlock.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCache_unlock(mamaFieldCache fieldCache);

#if defined(__cplusplus)
}
#endif

#endif /* MamaFieldCacheH__ */
