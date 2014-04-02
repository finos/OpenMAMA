/* $Id: //commsof_depot/platform/products/6.0/trunk/mama/c_cpp/src/c/fieldcache/fieldcacherecord.h#1 $
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

#ifndef MamaFieldCacheRecordH__
#define MamaFieldCacheRecordH__

#include <mama/config.h>
#include <mama/fielddesc.h>
#include <mama/types.h>
#include <mama/status.h>
#include <mama/fieldcache/fieldcachetypes.h>
#include <mama/fieldcache/fieldcachefield.h>

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */

/**
 * This function will create a mamaFieldCacheRecord.
 *
 * @param record (in) The record to create.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NOMEM
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheRecord_create(mamaFieldCacheRecord* record);

/**
 * This function will destroy a <code>mamaFieldCacheRecord</code> previously
 * allocated by a call to <code>mamaFieldCacheRecord_create</code>.
 * Note that all the fields contained in the record will be destroyed.
 *
 * @param record (in) The record to destroy.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheRecord_destroy(mamaFieldCacheRecord record);

/**
 * This function will clear a <code>mamaFieldCacheRecord</code>.
 * Note that all the fields contained in the record will be destroyed.
 *
 * @param record (in) The record to destroy.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheRecord_clear(mamaFieldCacheRecord record);

/**
 * This function will return the number of fields stored in a record.
 *
 * @param record (in) The field to destroy.
 * @param size (out) The size of the record.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheRecord_getSize(mamaFieldCacheRecord record, mama_size_t* size);

/** This function will create and add a field to the record; the new field is
 *  returned. The field will be destroyed when the record is destroyed.
 *
 * @param record (in) The record to add the field to.
 * @param fid (in) The field id of the field to create and add to the record.
 * @param type (in) The type of the field to create and add to the record.
 * @param name (in) The name of the field to create and add to the record.
 * @param field (out) The new field added to the record.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheRecord_add(mamaFieldCacheRecord record,
                         mama_fid_t fid,
                         mamaFieldType type,
                         const char* name,
                         mamaFieldCacheField* field);

/** This function will retrieve a field from the record using a position in the Record
 *  as in a normal C array.
 *  This can be useful when iterating the fields of the record in a loop.
 *  Note that position is not significative in a <code>mamaFieldCacheRecord</code>.
 *
 * @param record (in) The record to get the field from.
 * @param index (in) The index of the field to get from the record.
 * @param field (out) The field, if found.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheRecord_getField(mamaFieldCacheRecord record,
                              mama_size_t index,
                              mamaFieldCacheField* field);

/** This function will retrieve a field from the record using the field id of the
 *  field to get.
 *
 * @param record (in) The record to get the field from.
 * @param fid (in) The field id of the field.
 * @param field (out) The field, if found.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_NOT_FOUND
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheRecord_find(mamaFieldCacheRecord record,
                          mama_fid_t fid,
                          const char* name,
                          mamaFieldCacheField* field);

#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

#endif /* MamaFieldCacheRecordH__ */
