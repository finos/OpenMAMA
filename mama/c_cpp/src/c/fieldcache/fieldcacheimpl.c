/* $Id: //commsof_depot/platform/products/6.0/trunk/mama/c_cpp/src/c/fieldcache/fieldcacheimpl.c#1 $
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
#include <mama/fieldcache/fieldcachefield.h>
#include <mama/msg.h>
#include <mama/msgfield.h>

mama_status mamaFieldCache_updateCacheFromMsgField(mamaFieldCache fieldCache,
                                                   const mamaMsgField messageField)
{
    mama_status ret;
    mama_fid_t fid;
    mamaFieldType type = MAMA_FIELD_TYPE_UNKNOWN;
    const char* name = NULL;
    mamaFieldDescriptor descriptor = NULL;
    mamaFieldCacheField field = NULL;

    ret = mamaMsgField_getFid(messageField, &fid);
    if (ret != MAMA_STATUS_OK)
    {
        return ret;
    }

    if (MAMA_STATUS_OK != mamaFieldCacheMap_find(fieldCache->mMap, fid, type, name, &field))
    {
        ret = mamaMsgField_getType(messageField, &type);
        if (ret != MAMA_STATUS_OK)
        {
            return ret;
        }
        if (fieldCache->mUseFieldNames)
        {
            mamaMsgField_getName(messageField, &name);
        }
        mamaFieldCacheField_create(&field, fid, type, name);
        ret = mamaFieldCacheMap_add(fieldCache->mMap, field);
        if (ret != MAMA_STATUS_OK)
        {
            return ret;
        }
        fieldCache->mSize++;

        mamaMsgField_getDescriptor(messageField, &descriptor);
        if (descriptor)
        {
            mamaFieldCacheField_setDescriptor(field, descriptor);
        }
    }
    else
    {
        type = field->mType;
    }

    if (fieldCache->mTrackModified && field->mPublish && !field->mIsModified)
    {
        field->mIsModified = 1;
        mamaFieldCacheList_add(fieldCache->mModifiedFields, (void*)field);
    }

    switch (type)
    {
        case MAMA_FIELD_TYPE_BOOL:
        {
            mama_bool_t result;
            mamaMsgField_getBool(messageField, &result);
            mamaFieldCacheField_setBool(field, result);
            break;
        }
        case MAMA_FIELD_TYPE_CHAR:
        {
            char value;
            mamaMsgField_getChar(messageField, &value);
            mamaFieldCacheField_setChar(field, value);
            break;
        }
        case MAMA_FIELD_TYPE_I8:
        {
            mama_i8_t value;
            mamaMsgField_getI8(messageField, &value);
            mamaFieldCacheField_setI8(field, value);
            break;
        }
        case MAMA_FIELD_TYPE_U8:
        {
            mama_u8_t value;
            mamaMsgField_getU8(messageField, &value);
            mamaFieldCacheField_setU8(field, value);
            break;
        }
        case MAMA_FIELD_TYPE_I16:
        {
            mama_i16_t value;
            mamaMsgField_getI16(messageField, &value);
            mamaFieldCacheField_setI16(field, value);
            break;
        }
        case MAMA_FIELD_TYPE_U16:
        {
            mama_u16_t value;
            mamaMsgField_getU16(messageField, &value);
            mamaFieldCacheField_setU16(field, value);
            break;
        }
        case MAMA_FIELD_TYPE_I32:
        {
            mama_i32_t value;
            mamaMsgField_getI32(messageField, &value);
            mamaFieldCacheField_setI32(field, value);
            break;
        }
        case MAMA_FIELD_TYPE_U32:
        {
            mama_u32_t value;
            mamaMsgField_getU32(messageField, &value);
            mamaFieldCacheField_setU32(field, value);
            break;
        }
        case MAMA_FIELD_TYPE_I64:
        {
            mama_i64_t value;
            mamaMsgField_getI64(messageField, &value);
            mamaFieldCacheField_setI64(field, value);
            break;
        }
        case MAMA_FIELD_TYPE_U64:
        {
            mama_u64_t value;
            mamaMsgField_getU64(messageField, &value);
            mamaFieldCacheField_setU64(field, value);
            break;
        }
        case MAMA_FIELD_TYPE_F32:
        {
            mama_f32_t value;
            mamaMsgField_getF32(messageField, &value);
            mamaFieldCacheField_setF32(field, value);
            break;
        }
        case MAMA_FIELD_TYPE_QUANTITY:
        case MAMA_FIELD_TYPE_F64:
        {
            mama_f64_t value;
            mamaMsgField_getF64(messageField, &value);
            mamaFieldCacheField_setF64(field, value);
            break;
        }
        case MAMA_FIELD_TYPE_STRING:
        {
            const char* value = NULL;
            mamaMsgField_getString(messageField, &value);
            mamaFieldCacheField_setString(field, value, 0);
            break;
        }
        case MAMA_FIELD_TYPE_PRICE:
        {
            mamaMsgField_getPrice(messageField, fieldCache->mReusablePrice);
            mamaFieldCacheField_setPrice(field, fieldCache->mReusablePrice);
            break;
        }
        case MAMA_FIELD_TYPE_TIME:
        {
            mamaMsgField_getDateTime(messageField, fieldCache->mReusableDateTime);
            mamaFieldCacheField_setDateTime(field, fieldCache->mReusableDateTime);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_I8:
        {
            const mama_i8_t* values = NULL;
            mama_size_t size = 0;
            mamaMsgField_getVectorI8(messageField, &values, &size);
            mamaFieldCacheField_setI8Vector(field, values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_U8:
        {
            const mama_u8_t* values = NULL;
            mama_size_t size = 0;
            mamaMsgField_getVectorU8(messageField, &values, &size);
            mamaFieldCacheField_setU8Vector(field, values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_I16:
        {
            const mama_i16_t* values = NULL;
            mama_size_t size = 0;
            mamaMsgField_getVectorI16(messageField, &values, &size);
            mamaFieldCacheField_setI16Vector(field, values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_U16:
        {
            const mama_u16_t* values = NULL;
            mama_size_t size = 0;
            mamaMsgField_getVectorU16(messageField, &values, &size);
            mamaFieldCacheField_setU16Vector(field, values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_I32:
        {
            const mama_i32_t* values = NULL;
            mama_size_t size = 0;
            mamaMsgField_getVectorI32(messageField, &values, &size);
            mamaFieldCacheField_setI32Vector(field, values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_U32:
        {
            const mama_u32_t* values = NULL;
            mama_size_t size = 0;
            mamaMsgField_getVectorU32(messageField, &values, &size);
            mamaFieldCacheField_setU32Vector(field, values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_I64:
        {
            const mama_i64_t* values = NULL;
            mama_size_t size = 0;
            mamaMsgField_getVectorI64(messageField, &values, &size);
            mamaFieldCacheField_setI64Vector(field, values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_U64:
        {
            const mama_u64_t* values = NULL;
            mama_size_t size = 0;
            mamaMsgField_getVectorU64(messageField, &values, &size);
            mamaFieldCacheField_setU64Vector(field, values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_F32:
        {
            const mama_f32_t* values = NULL;
            mama_size_t size = 0;
            mamaMsgField_getVectorF32(messageField, &values, &size);
            mamaFieldCacheField_setF32Vector(field, values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_F64:
        {
            const mama_f64_t* values = NULL;
            mama_size_t size = 0;
            mamaMsgField_getVectorF64(messageField, &values, &size);
            mamaFieldCacheField_setF64Vector(field, values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_STRING:
        {
            const char** values = NULL;
            mama_size_t size = 0;
            mamaMsgField_getVectorString(messageField, &values, &size);
            mamaFieldCacheField_setStringVector(field, values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_PRICE:
        {
            /* TODO: mamaMsgField does not provide methods to get price vector
            const mamaPrice* values = NULL;
            mama_size_t size;
            mamaMsgField_getVectorPrice(messageField, &values, &size);
            mamaFieldCacheField_setPriceVector(field, values, size);
             */
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_TIME:
        {
            /* TODO: mamaMsgField does not provide methods to get time vector
            const mamaDateTime* values = NULL;
            mama_size_t size;
            mamaMsgField_getVectorDateTime(messageField, &values, &size);
            mamaFieldCacheField_setDateTimeVector(field, values, size);
             */
            break;
        }
        default:
            break;
    }
    return ret;
}

mama_status mamaFieldCache_updateCacheFromField(mamaFieldCache fieldCache,
                                                const mamaFieldCacheField field)
{
    mama_status ret;
    mama_fid_t fid;
    mamaFieldType type;
    mama_bool_t alreadyModified = 0;
    const char* name = NULL;
    mamaFieldCacheField cachedField = NULL;

    ret = mamaFieldCacheField_getFid(field, &fid);
    if (ret != MAMA_STATUS_OK)
    {
        return ret;
    }

    ret = mamaFieldCacheField_getType(field, &type);
    if (ret != MAMA_STATUS_OK)
    {
        return ret;
    }

    mamaFieldCacheField_getName(field, &name);
    if (MAMA_STATUS_OK != mamaFieldCacheMap_find(fieldCache->mMap, fid, type, name, &cachedField))
    {
        ret = mamaFieldCacheField_create(&cachedField, fid, type, NULL);
        if (ret != MAMA_STATUS_OK)
        {
            return ret;
        }
        ret = mamaFieldCacheMap_add(fieldCache->mMap, cachedField);
        if (ret != MAMA_STATUS_OK)
        {
            return ret;
        }
        fieldCache->mSize++;
        if (field->mPublish && !field->mCheckModified)
        {   /* Always publish */
            mamaFieldCacheList_add(fieldCache->mAlwaysPublishFields, (void*)cachedField);
        }
        alreadyModified = 0;
    }
    else
    {
        alreadyModified = cachedField->mIsModified;
    }

    mamaFieldCacheField_copy(field, cachedField);

    if (cachedField->mPublish && cachedField->mCheckModified
        && !alreadyModified
        && fieldCache->mTrackModified)
    {
        cachedField->mIsModified = 1;
        mamaFieldCacheList_add(fieldCache->mModifiedFields, (void*)cachedField);
    }

    return ret;
}

mama_status mamaFieldCache_updateMsgField(mamaFieldCache fieldCache,
                                          mamaFieldCacheField field,
                                          mamaMsg message,
                                          mama_bool_t useUpdate)
{
    mama_status status = MAMA_STATUS_OK;
    mama_fid_t fid;
    mamaFieldType type;
    const char* name = NULL;

    mamaFieldCacheField_getFid(field, &fid);
    mamaFieldCacheField_getType(field, &type);
    if (fieldCache->mUseFieldNames)
    {
        mamaFieldCacheField_getName(field, &name);
    }

    switch (type)
    {
        case MAMA_FIELD_TYPE_BOOL:
        {
            mama_bool_t value;
            mamaFieldCacheField_getBool(field, &value);
            status = useUpdate ? mamaMsg_updateBool(message, name, fid, value)
                               : mamaMsg_addBool(message, name, fid, value);
            break;
        }
        case MAMA_FIELD_TYPE_CHAR:
        {
            char value;
            mamaFieldCacheField_getChar(field, &value);
            status = useUpdate ? mamaMsg_updateChar(message, name, fid, value)
                               : mamaMsg_addChar(message, name, fid, value);
            break;
        }
        case MAMA_FIELD_TYPE_I8:
        {
            mama_i8_t value;
            mamaFieldCacheField_getI8(field, &value);
            status = useUpdate ? mamaMsg_updateI8(message, name, fid, value)
                               : mamaMsg_addI8(message, name, fid, value);
            break;
        }
        case MAMA_FIELD_TYPE_U8:
        {
            mama_u8_t value;
            mamaFieldCacheField_getU8(field, &value);
            status = useUpdate ? mamaMsg_updateU8(message, name, fid, value)
                               : mamaMsg_addU8(message, name, fid, value);
            break;
        }
        case MAMA_FIELD_TYPE_I16:
        {
            mama_i16_t value;
            mamaFieldCacheField_getI16(field, &value);
            status = useUpdate ? mamaMsg_updateI16(message, name, fid, value)
                               : mamaMsg_addI16(message, name, fid, value);
            break;
        }
        case MAMA_FIELD_TYPE_U16:
        {
            mama_u16_t value;
            mamaFieldCacheField_getU16(field, &value);
            status = useUpdate ? mamaMsg_updateU16(message, name, fid, value)
                               : mamaMsg_addU16(message, name, fid, value);
            break;
        }
        case MAMA_FIELD_TYPE_I32:
        {
            mama_i32_t value;
            mamaFieldCacheField_getI32(field, &value);
            status = useUpdate ? mamaMsg_updateI32(message, name, fid, value)
                               : mamaMsg_addI32(message, name, fid, value);
            break;
        }
        case MAMA_FIELD_TYPE_U32:
        {
            mama_u32_t value;
            mamaFieldCacheField_getU32(field, &value);
            status = useUpdate ? mamaMsg_updateU32(message, name, fid, value)
                               : mamaMsg_addU32(message, name, fid, value);
            break;
        }
        case MAMA_FIELD_TYPE_I64:
        {
            mama_i64_t value;
            mamaFieldCacheField_getI64(field, &value);
            status = useUpdate ? mamaMsg_updateI64(message, name, fid, value)
                               : mamaMsg_addI64(message, name, fid, value);
            break;
        }
        case MAMA_FIELD_TYPE_U64:
        {
            mama_u64_t value;
            mamaFieldCacheField_getU64(field, &value);
            status = useUpdate ? mamaMsg_updateU64(message, name, fid, value)
                               : mamaMsg_addU64(message, name, fid, value);
            break;
        }
        case MAMA_FIELD_TYPE_F32:
        {
            mama_f32_t value;
            mamaFieldCacheField_getF32(field, &value);
            status = useUpdate ? mamaMsg_updateF32(message, name, fid, value)
                               : mamaMsg_addF32(message, name, fid, value);
            break;
        }
        case MAMA_FIELD_TYPE_QUANTITY:
        case MAMA_FIELD_TYPE_F64:
        {
            mama_f64_t value;
            mamaFieldCacheField_getF64(field, &value);
            status = useUpdate ? mamaMsg_updateF64(message, name, fid, value)
                               : mamaMsg_addF64(message, name, fid, value);
            break;
        }
        case MAMA_FIELD_TYPE_STRING:
        {
            const char* value = NULL;
            mama_size_t len;
            mamaFieldCacheField_getString(field, &value, &len);
            if (!value)
            {
                return MAMA_STATUS_INVALID_ARG;
            }
            status = useUpdate ? mamaMsg_updateString(message, name, fid, value)
                                : mamaMsg_addString(message, name, fid, value);
            break;
        }
        case MAMA_FIELD_TYPE_PRICE:
        {
            const mamaPrice value = NULL;
            mamaFieldCacheField_getPrice(field, &value);
            if (!value)
            {
                return MAMA_STATUS_INVALID_ARG;
            }
            status = useUpdate ? mamaMsg_updatePrice(message, name, fid, value)
                               : mamaMsg_addPrice(message, name, fid, value);
            break;
        }
        case MAMA_FIELD_TYPE_TIME:
        {
            const mamaDateTime value = NULL;
            mamaFieldCacheField_getDateTime(field, &value);
            if (!value)
            {
                return MAMA_STATUS_INVALID_ARG;
            }
            status = useUpdate ? mamaMsg_updateDateTime(message, name, fid, value)
                               : mamaMsg_addDateTime(message, name, fid, value);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_I8:
        {
            const mama_i8_t* values = NULL;
            mama_size_t size = 0;
            mamaFieldCacheField_getI8Vector(field, &values, &size);
            if (!values)
            {
                return MAMA_STATUS_INVALID_ARG;
            }
            status = useUpdate ? mamaMsg_updateVectorI8(message, name, fid, values, size)
                               : mamaMsg_addVectorI8(message, name, fid, values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_U8:
        {
            const mama_u8_t* values = NULL;
            mama_size_t size = 0;
            mamaFieldCacheField_getU8Vector(field, &values, &size);
            if (!values)
            {
                return MAMA_STATUS_INVALID_ARG;
            }
            status = useUpdate ? mamaMsg_updateVectorU8(message, name, fid, values, size)
                               : mamaMsg_addVectorU8(message, name, fid, values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_I16:
        {
            const mama_i16_t* values = NULL;
            mama_size_t size = 0;
            mamaFieldCacheField_getI16Vector(field, &values, &size);
            if (!values)
            {
                return MAMA_STATUS_INVALID_ARG;
            }
            status = useUpdate ? mamaMsg_updateVectorI16(message, name, fid, values, size)
                               : mamaMsg_addVectorI16(message, name, fid, values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_U16:
        {
            const mama_u16_t* values = NULL;
            mama_size_t size = 0;
            mamaFieldCacheField_getU16Vector(field, &values, &size);
            if (!values)
            {
                return MAMA_STATUS_INVALID_ARG;
            }
            status = useUpdate ? mamaMsg_updateVectorU16(message, name, fid, values, size)
                               : mamaMsg_addVectorU16(message, name, fid, values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_I32:
        {
            const mama_i32_t* values = NULL;
            mama_size_t size = 0;
            mamaFieldCacheField_getI32Vector(field, &values, &size);
            if (!values)
            {
                return MAMA_STATUS_INVALID_ARG;
            }
            status = useUpdate ? mamaMsg_updateVectorI32(message, name, fid, values, size)
                               : mamaMsg_addVectorI32(message, name, fid, values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_U32:
        {
            const mama_u32_t* values = NULL;
            mama_size_t size = 0;
            mamaFieldCacheField_getU32Vector(field, &values, &size);
            if (!values)
            {
                return MAMA_STATUS_INVALID_ARG;
            }
            status = useUpdate ? mamaMsg_updateVectorU32(message, name, fid, values, size)
                               : mamaMsg_addVectorU32(message, name, fid, values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_I64:
        {
            const mama_i64_t* values = NULL;
            mama_size_t size = 0;
            mamaFieldCacheField_getI64Vector(field, &values, &size);
            if (!values)
            {
                return MAMA_STATUS_INVALID_ARG;
            }
            status = useUpdate ? mamaMsg_updateVectorI64(message, name, fid, values, size)
                               : mamaMsg_addVectorI64(message, name, fid, values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_U64:
        {
            const mama_u64_t* values = NULL;
            mama_size_t size = 0;
            mamaFieldCacheField_getU64Vector(field, &values, &size);
            if (!values)
            {
                return MAMA_STATUS_INVALID_ARG;
            }
            status = useUpdate ? mamaMsg_updateVectorU64(message, name, fid, values, size)
                               : mamaMsg_addVectorU64(message, name, fid, values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_F32:
        {
            const mama_f32_t* values = NULL;
            mama_size_t size = 0;
            mamaFieldCacheField_getF32Vector(field, &values, &size);
            if (!values)
            {
                return MAMA_STATUS_INVALID_ARG;
            }
            status = useUpdate ? mamaMsg_updateVectorF32(message, name, fid, values, size)
                               : mamaMsg_addVectorF32(message, name, fid, values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_F64:
        {
            const mama_f64_t* values = NULL;
            mama_size_t size = 0;
            mamaFieldCacheField_getF64Vector(field, &values, &size);
            if (!values)
            {
                return MAMA_STATUS_INVALID_ARG;
            }
            status = useUpdate ? mamaMsg_updateVectorF64(message, name, fid, values, size)
                               : mamaMsg_addVectorF64(message, name, fid, values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_STRING:
        {
            const char** values = NULL;
            mama_size_t size = 0;
            mamaFieldCacheField_getStringVector(field, &values, &size);
            if (!values)
            {
                return MAMA_STATUS_INVALID_ARG;
            }
            status = useUpdate ? mamaMsg_updateVectorString(message, name, fid, values, size)
                               : mamaMsg_addVectorString(message, name, fid, values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_PRICE:
        {
            const mamaPrice* values = NULL;
            mama_size_t size = 0;
            mamaFieldCacheField_getPriceVector(field, &values, &size);
            if (!values)
            {
                return MAMA_STATUS_INVALID_ARG;
            }
            status = /*useUpdate ? mamaMsg_updateVectorPrice(message, name, fid, values, size)
                               : */mamaMsg_addVectorPrice(message, name, fid, values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_TIME:
        {
            const mamaDateTime* values = NULL;
            mama_size_t size = 0;
            mamaFieldCacheField_getDateTimeVector(field, &values, &size);
            if (!values)
            {
                return MAMA_STATUS_INVALID_ARG;
            }
            status = /*useUpdate ? mamaMsg_updateVectorTime(message, name, fid, values, size)
                               : */mamaMsg_addVectorDateTime(message, name, fid, values, size);
            break;
        }
        default:
            status = MAMA_STATUS_NOT_FOUND;
            break;
    }
    return status;
}
