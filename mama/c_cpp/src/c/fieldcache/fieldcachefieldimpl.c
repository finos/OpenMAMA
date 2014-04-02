/* $Id: //commsof_depot/platform/products/6.0/trunk/mama/c_cpp/src/c/fieldcache/fieldcachefieldimpl.c#1 $
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

#include "fieldcachefieldimpl.h"
#include <mama/price.h>
#include <mama/datetime.h>
#include <string.h> /* For memcpy */

/* Private functions - not exposed through the API interface */
/*
mama_status mamaFieldCacheField_getData(
        const mamaFieldCacheField field,
        void** data,
        mama_u32_t* size)
{
    if (!field)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    
    *data = field->mData;
    *size = field->mDataSize;
    
    return MAMA_STATUS_OK;
}
*/
mama_status mamaFieldCacheField_setDataValue(mamaFieldCacheField field,
                                             const void* data,
                                             mama_size_t size)
{
    if (field->mData.data && field->mDataSize != size)
    {
        free(field->mData.data);
        field->mData.data = NULL;
        field->mDataSize = 0;
    }
    if (data && size)
    {
        if (field->mDataSize != size)
        {
            field->mData.data = malloc(size);
            field->mDataSize = size;
        }
        if (field->mData.data)
        {
            memcpy(field->mData.data, data, size);
        }
    }

    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_setDataPointer(mamaFieldCacheField field,
                                               void* data,
                                               void** old)
{
    if (field->mData.data)
    {
        *old = field->mData.data;
        field->mData.data = NULL;
        field->mDataSize = 0;
    }
    if (data)
    {
        field->mData.data = data;
        field->mDataSize = 0;
    }

    return MAMA_STATUS_OK;
}

mama_status
mamaFieldCacheField_destroyPriceVector(mamaFieldCacheVector priceVector)
{
    mama_size_t i;
    mama_size_t size = 0;
    void* price = NULL;

    mamaFieldCacheVector_getSize(priceVector, &size);
    for (i = 0; i < size; i++)
    {
        mamaFieldCacheVector_get(priceVector, i, &price);
        mamaPrice_destroy((mamaPrice)price);
    }
    mamaFieldCacheVector_destroy(priceVector);
    return MAMA_STATUS_OK;
}

mama_status
mamaFieldCacheField_destroyDateTimeVector(mamaFieldCacheVector dateTimeVector)
{
    mama_size_t i;
    mama_size_t size = 0;
    void* dateTime = NULL;
    
    mamaFieldCacheVector_getSize(dateTimeVector, &size);
    for (i = 0; i < size; i++)
    {
        mamaFieldCacheVector_get(dateTimeVector, i, &dateTime);
        mamaDateTime_destroy((mamaDateTime)dateTime);
    }
    mamaFieldCacheVector_destroy(dateTimeVector);
    return MAMA_STATUS_OK;
}
