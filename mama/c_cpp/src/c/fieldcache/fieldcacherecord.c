/* $Id: //commsof_depot/platform/products/6.0/trunk/mama/c_cpp/src/c/fieldcache/fieldcacherecord.c#1 $
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

#include <mama/fieldcache/fieldcacherecord.h>
#include "fieldcacherecordimpl.h"

mama_status mamaFieldCacheRecord_create(mamaFieldCacheRecord* record)
{
    mama_status ret;
    mamaFieldCacheRecord localRecord = NULL;

    if (!record)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    localRecord = (mamaFieldCacheRecord)calloc(1, sizeof(mamaFieldCacheRecordImpl));
    if (!localRecord)
    {
        return MAMA_STATUS_NOMEM;
    }
    ret = mamaFieldCacheVector_create(&localRecord->mFields);
    if (ret != MAMA_STATUS_OK)
    {
        free(localRecord);
        return ret;
    }

    *record = localRecord;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheRecord_destroy(mamaFieldCacheRecord record)
{
    int i;
    void* field = NULL;

    if (!record)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    for (i = 0; i < record->mSize; i++)
    {
        mamaFieldCacheVector_get(record->mFields, i, &field);
        mamaFieldCacheField_destroy((mamaFieldCacheField)field);
    }
    mamaFieldCacheVector_destroy(record->mFields);
    free(record);
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheRecord_clear(mamaFieldCacheRecord record)
{
    int i;
    void* field = NULL;

    if (!record)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    for (i = 0; i < record->mSize; i++)
    {
        mamaFieldCacheVector_get(record->mFields, i, &field);
        mamaFieldCacheField_destroy((mamaFieldCacheField)field);
        mamaFieldCacheVector_set(record->mFields, i, NULL);
    }
    record->mSize = 0;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheRecord_getSize(mamaFieldCacheRecord record, mama_size_t* size)
{
    if (!record || !size)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    *size = record->mSize;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheRecord_add(mamaFieldCacheRecord record,
                                     mama_fid_t fid,
                                     mamaFieldType type,
                                     const char* name,
                                     mamaFieldCacheField* field)
{
    if (!record || !field)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    *field = NULL;
    mamaFieldCacheVector_grow(record->mFields, record->mSize + 10);

    mamaFieldCacheField_create(field, fid, type, name);
    mamaFieldCacheVector_set(record->mFields, record->mSize, *field);
    record->mSize++;
    return MAMA_STATUS_OK;
}

mama_status
mamaFieldCacheRecord_getField(mamaFieldCacheRecord record,
                              mama_size_t index,
                              mamaFieldCacheField* field)
{
    if (!record || !field)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (index >= record->mSize)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    mamaFieldCacheVector_get(record->mFields, index, (void**)field);
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheRecord_find(mamaFieldCacheRecord record,
                                      mama_fid_t fid,
                                      const char* name,
                                      mamaFieldCacheField* field)
{
    mama_status ret;
    int i;
    mama_fid_t fieldId;
    
    if (!record || !field)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    for (i = 0; i < record->mSize; i++)
    {
        fieldId = 0;
        mamaFieldCacheVector_get(record->mFields, i, (void**)field);
        ret = mamaFieldCacheField_getFid(*field, &fieldId);
        if (ret == MAMA_STATUS_OK && fid == fieldId)
        {
            return MAMA_STATUS_OK;
        }
    }
    *field = NULL;
    return MAMA_STATUS_NOT_FOUND;
}
