/* $Id: //commsof_depot/platform/products/6.0/trunk/mama/c_cpp/src/c/fieldcache/fieldcachefield.c#1 $
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
#include "fieldcachevector.h"
#include <mama/fieldcache/fieldcachefield.h>
#include <mama/price.h>
#include <mama/datetime.h>
#include <stdio.h>

mama_bool_t mamaFieldCacheField_isSupported(mamaFieldType type)
{
    switch (type)
    {
        case MAMA_FIELD_TYPE_MSG:
        case MAMA_FIELD_TYPE_VECTOR_MSG:
        case MAMA_FIELD_TYPE_UNKNOWN:
        case MAMA_FIELD_TYPE_COLLECTION:
        case MAMA_FIELD_TYPE_OPAQUE:
            return 0;
        default:
            return 1;
    }
}

mama_status mamaFieldCacheField_create(mamaFieldCacheField* field,
                                       mama_fid_t fid,
                                       mamaFieldType type,
                                       const char* name)
{
    mamaFieldCacheField localField = NULL;

    if (!field)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    if (!mamaFieldCacheField_isSupported(type))
    {
        return MAMA_STATUS_INVALID_ARG;
    }

    localField = (mamaFieldCacheField)calloc(1, sizeof(mamaFieldCacheFieldImpl));
    if (!localField)
    {
        return MAMA_STATUS_NOMEM;
    }

    localField->mFid = fid;
    localField->mType = type;
    if (name)
    {
        localField->mName = strdup(name);
    }
    localField->mPublish = 1;
    localField->mCheckModified = 1;

    *field = localField;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_destroy(mamaFieldCacheField field)
{
    if (!field)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    if (field->mName)
    {
        free((void*)field->mName);
    }

    if (field->mDataSize)
    {
        free(field->mData.data);
    }
    else
    {
        switch (field->mType)
        {
        case MAMA_FIELD_TYPE_PRICE:
            mamaPrice_destroy((mamaPrice)field->mData.data);
            break;
        case MAMA_FIELD_TYPE_TIME:
            mamaDateTime_destroy((mamaDateTime)field->mData.data);
            break;
        case MAMA_FIELD_TYPE_VECTOR_STRING:
            if (field->mData.data)
            {
                mamaFieldCacheVector_free((mamaFieldCacheVector)field->mData.data);
                mamaFieldCacheVector_destroy((mamaFieldCacheVector)field->mData.data);
            }
            break;
        case MAMA_FIELD_TYPE_VECTOR_PRICE:
            if (field->mData.data)
            {
                mamaFieldCacheField_destroyPriceVector(
                        (mamaFieldCacheVector)field->mData.data);
            }
            break;
        case MAMA_FIELD_TYPE_VECTOR_TIME:
            if (field->mData.data)
            {
                mamaFieldCacheField_destroyDateTimeVector(
                        (mamaFieldCacheVector)field->mData.data);
            }
            break;
        default: /* something is wrong here */
            break;
        }
    }
  
    free(field);

    return MAMA_STATUS_OK;
}

mama_status
mamaFieldCacheField_copy(const mamaFieldCacheField field, mamaFieldCacheField copy)
{
    if (!field || !copy)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field == copy)
    {
        return MAMA_STATUS_OK;
    }
    if (field->mFid != copy->mFid || field->mType != copy->mType)
    {
        return MAMA_STATUS_INVALID_ARG;
    }

    copy->mIsModified = field->mIsModified;
    copy->mPublish = field->mPublish;
    copy->mCheckModified = field->mCheckModified;
    copy->mDescriptor = field->mDescriptor;
    if (field->mName)
    {
        if (copy->mName)
        {
            free((void*)copy->mName);
        }
        copy->mName = strdup(field->mName);
    }
    switch (field->mType)
    {
        case MAMA_FIELD_TYPE_BOOL:
        {
            mama_bool_t result;
            mamaFieldCacheField_getBool(field, &result);
            mamaFieldCacheField_setBool(copy, result);
            break;
        }
        case MAMA_FIELD_TYPE_CHAR:
        {
            char value;
            mamaFieldCacheField_getChar(field, &value);
            mamaFieldCacheField_setChar(copy, value);
            break;
        }
        case MAMA_FIELD_TYPE_I8:
        {
            mama_i8_t value;
            mamaFieldCacheField_getI8(field, &value);
            mamaFieldCacheField_setI8(copy, value);
            break;
        }
        case MAMA_FIELD_TYPE_U8:
        {
            mama_u8_t value;
            mamaFieldCacheField_getU8(field, &value);
            mamaFieldCacheField_setU8(copy, value);
            break;
        }
        case MAMA_FIELD_TYPE_I16:
        {
            mama_i16_t value;
            mamaFieldCacheField_getI16(field, &value);
            mamaFieldCacheField_setI16(copy, value);
            break;
        }
        case MAMA_FIELD_TYPE_U16:
        {
            mama_u16_t value;
            mamaFieldCacheField_getU16(field, &value);
            mamaFieldCacheField_setU16(copy, value);
            break;
        }
        case MAMA_FIELD_TYPE_I32:
        {
            mama_i32_t value;
            mamaFieldCacheField_getI32(field, &value);
            mamaFieldCacheField_setI32(copy, value);
            break;
        }
        case MAMA_FIELD_TYPE_U32:
        {
            mama_u32_t value;
            mamaFieldCacheField_getU32(field, &value);
            mamaFieldCacheField_setU32(copy, value);
            break;
        }
        case MAMA_FIELD_TYPE_I64:
        {
            mama_i64_t value;
            mamaFieldCacheField_getI64(field, &value);
            mamaFieldCacheField_setI64(copy, value);
            break;
        }
        case MAMA_FIELD_TYPE_U64:
        {
            mama_u64_t value;
            mamaFieldCacheField_getU64(field, &value);
            mamaFieldCacheField_setU64(copy, value);
            break;
        }
        case MAMA_FIELD_TYPE_F32:
        {
            mama_f32_t value;
            mamaFieldCacheField_getF32(field, &value);
            mamaFieldCacheField_setF32(copy, value);
            break;
        }
        case MAMA_FIELD_TYPE_QUANTITY:
        case MAMA_FIELD_TYPE_F64:
        {
            mama_f64_t value;
            mamaFieldCacheField_getF64(field, &value);
            mamaFieldCacheField_setF64(copy, value);
            break;
        }
        case MAMA_FIELD_TYPE_STRING:
        {
            const char* value;
            mama_size_t len;
            mamaFieldCacheField_getString(field, &value, &len);
            mamaFieldCacheField_setString(copy, value, len);
            break;
        }
        case MAMA_FIELD_TYPE_PRICE:
        {
            mamaPrice value = NULL;
            const mamaPrice* pvalue = &value;
            mamaFieldCacheField_getPrice(field, pvalue);
            mamaFieldCacheField_setPrice(copy, value);
            break;
        }
        case MAMA_FIELD_TYPE_TIME:
        {
            mamaDateTime value = NULL;
            const mamaDateTime* pvalue = &value;
            mamaFieldCacheField_getDateTime(field, pvalue);
            mamaFieldCacheField_setDateTime(copy, value);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_I8:
        {
            const mama_i8_t* values = NULL;
            mama_size_t size;
            mamaFieldCacheField_getI8Vector(field, &values, &size);
            mamaFieldCacheField_setI8Vector(copy, values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_U8:
        {
            const mama_u8_t* values = NULL;
            mama_size_t size;
            mamaFieldCacheField_getU8Vector(field, &values, &size);
            mamaFieldCacheField_setU8Vector(copy, values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_I16:
        {
            const mama_i16_t* values = NULL;
            mama_size_t size;
            mamaFieldCacheField_getI16Vector(field, &values, &size);
            mamaFieldCacheField_setI16Vector(copy, values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_U16:
        {
            const mama_u16_t* values = NULL;
            mama_size_t size;
            mamaFieldCacheField_getU16Vector(field, &values, &size);
            mamaFieldCacheField_setU16Vector(copy, values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_I32:
        {
            const mama_i32_t* values = NULL;
            mama_size_t size;
            mamaFieldCacheField_getI32Vector(field, &values, &size);
            mamaFieldCacheField_setI32Vector(copy, values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_U32:
        {
            const mama_u32_t* values = NULL;
            mama_size_t size;
            mamaFieldCacheField_getU32Vector(field, &values, &size);
            mamaFieldCacheField_setU32Vector(copy, values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_I64:
        {
            const mama_i64_t* values = NULL;
            mama_size_t size;
            mamaFieldCacheField_getI64Vector(field, &values, &size);
            mamaFieldCacheField_setI64Vector(copy, values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_U64:
        {
            const mama_u64_t* values = NULL;
            mama_size_t size;
            mamaFieldCacheField_getU64Vector(field, &values, &size);
            mamaFieldCacheField_setU64Vector(copy, values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_F32:
        {
            const mama_f32_t* values = NULL;
            mama_size_t size;
            mamaFieldCacheField_getF32Vector(field, &values, &size);
            mamaFieldCacheField_setF32Vector(copy, values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_F64:
        {
            const mama_f64_t* values = NULL;
            mama_size_t size;
            mamaFieldCacheField_getF64Vector(field, &values, &size);
            mamaFieldCacheField_setF64Vector(copy, values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_STRING:
        {
            const char** values = NULL;
            mama_size_t size;
            mamaFieldCacheField_getStringVector(field, &values, &size);
            mamaFieldCacheField_setStringVector(copy, values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_PRICE:
        {
            const mamaPrice* values = NULL;
            mama_size_t size;
            mamaFieldCacheField_getPriceVector(field, &values, &size);
            mamaFieldCacheField_setPriceVector(copy, values, size);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_TIME:
        {
            const mamaDateTime* values = NULL;
            mama_size_t size;
            mamaFieldCacheField_getDateTimeVector(field, &values, &size);
            mamaFieldCacheField_setDateTimeVector(copy, values, size);
            break;
        }
        default:
            break;
    }
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_getFid(mamaFieldCacheField field, mama_fid_t* fid)
{
    if (!field || !fid)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    *fid = field->mFid;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_getType(mamaFieldCacheField field, mamaFieldType* type)
{
    if (!field || !type)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    *type = field->mType;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_getName(mamaFieldCacheField field, const char** name)
{
    if (!field || !name)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mName)
    {
        *name = field->mName;
    }
    else if (field->mDescriptor)
    {
        *name = mamaFieldDescriptor_getName(field->mDescriptor);
    }
    else
    {
        *name = NULL;
    }
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_setDescriptor(mamaFieldCacheField field,
                                              const mamaFieldDescriptor desc)
{
    if (!field)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    field->mDescriptor = desc;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_getDescriptor(mamaFieldCacheField field,
                                              const mamaFieldDescriptor* desc)
{
    if (!field || !desc)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    *(mamaFieldDescriptor*)desc = field->mDescriptor;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_isModified(mamaFieldCacheField field,
                                           mama_bool_t* modified)
{
    if (!field || !modified)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    *modified = field->mIsModified;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_setPublish(mamaFieldCacheField field,
                                           mama_bool_t publish)
{
    if (!field)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    field->mPublish = publish;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_getPublish(mamaFieldCacheField field,
                                           mama_bool_t* publish)
{
    if (!field || !publish)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    *publish = field->mPublish;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_setCheckModified(mamaFieldCacheField field,
                                                 mama_bool_t check)
{
    if (!field)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    field->mCheckModified = check;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_getCheckModified(mamaFieldCacheField field,
                                                 mama_bool_t* check)
{
    if (!field || !check)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    *check = field->mCheckModified;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_getBool(const mamaFieldCacheField field,
                                        mama_bool_t* result)
{
    if (!field || !result)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_BOOL)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    *result = field->mData.abool;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_getChar(const mamaFieldCacheField field,
                                        char* result)
{
    if (!field || !result)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_CHAR)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    *result = field->mData.chr;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_getI8(const mamaFieldCacheField field,
                                      mama_i8_t* result)
{
    if (!field || !result)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_I8)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    *result = field->mData.i8;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_getU8(const mamaFieldCacheField field,
                                      mama_u8_t* result)
{
    if (!field || !result)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_U8)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    *result = field->mData.u8;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_getI16(const mamaFieldCacheField field,
                                       mama_i16_t* result)
{
    if (!field || !result)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_I16)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    *result = field->mData.i16;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_getU16(const mamaFieldCacheField field,
                                       mama_u16_t* result)
{
    if (!field || !result)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_U16)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    *result = field->mData.u16;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_getI32(const mamaFieldCacheField field,
                                       mama_i32_t* result)
{
    if (!field || !result)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_I32)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    *result = field->mData.i32;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_getU32(const mamaFieldCacheField field,
                                       mama_u32_t* result)
{
    if (!field || !result)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_U32)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    *result = field->mData.u32;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_getI64(const mamaFieldCacheField field,
                                       mama_i64_t* result)
{
    if (!field || !result)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_I64)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    *result = field->mData.i64;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_getU64(const mamaFieldCacheField field,
                                       mama_u64_t* result)
{
    if (!field || !result)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_U64)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    *result = field->mData.u64;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_getF32(const mamaFieldCacheField field,
                                       mama_f32_t* result)
{
    if (!field || !result)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_F32)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    *result = field->mData.f32;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_getF64(const mamaFieldCacheField field,
                                       mama_f64_t* result)
{
    if (!field || !result)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_F64)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    *result = field->mData.f64;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_getString(const mamaFieldCacheField field,
                                          const char** result,
                                          mama_size_t* len)
{
    if (!field || !result)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_STRING)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    *result = field->mData.str; /* NOT COPYING */
    *len = field->mVectorSize;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_getPrice(const mamaFieldCacheField field,
                                         const mamaPrice* result)
{
    if (!field || !result)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_PRICE)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    *(mamaPrice*)result = (mamaPrice)field->mData.data; /* NOT COPYING */
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_getDateTime(const mamaFieldCacheField field,
                                            const mamaDateTime* result)
{
    if (!field || !result)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_TIME)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    *(mamaDateTime*)result = (mamaDateTime)field->mData.data; /* NOT COPYING */
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_getI8Vector(const mamaFieldCacheField field,
                                            const mama_i8_t** values,
                                            mama_size_t* size)
{
    if (!field || !values || !size)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_VECTOR_I8)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    *values = (mama_i8_t*)field->mData.data; /* NOT COPYING */
    *size = field->mVectorSize;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_getU8Vector(const mamaFieldCacheField field,
                                            const mama_u8_t** values,
                                            mama_size_t* size)
{
    if (!field || !values || !size)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_VECTOR_U8)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    *values = (mama_u8_t*)field->mData.data; /* NOT COPYING */
    *size = field->mVectorSize;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_getI16Vector(const mamaFieldCacheField field,
                                             const mama_i16_t** values,
                                             mama_size_t* size)
{
    if (!field || !values || !size)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_VECTOR_I16)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    *values = (mama_i16_t*)field->mData.data; /* NOT COPYING */
    *size = field->mVectorSize;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_getU16Vector(const mamaFieldCacheField field,
                                             const mama_u16_t** values,
                                             mama_size_t* size)
{
    if (!field || !values || !size)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_VECTOR_U16)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    *values = (mama_u16_t*)field->mData.data; /* NOT COPYING */
    *size = field->mVectorSize;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_getI32Vector(const mamaFieldCacheField field,
                                             const mama_i32_t** values,
                                             mama_size_t* size)
{
    if (!field || !values || !size)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_VECTOR_I32)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    *values = (mama_i32_t*)field->mData.data; /* NOT COPYING */
    *size = field->mVectorSize;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_getU32Vector(const mamaFieldCacheField field,
                                             const mama_u32_t** values,
                                             mama_size_t* size)
{
    if (!field || !values || !size)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_VECTOR_U32)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    *values = (mama_u32_t*)field->mData.data; /* NOT COPYING */
    *size = field->mVectorSize;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_getI64Vector(const mamaFieldCacheField field,
                                             const mama_i64_t** values,
                                             mama_size_t* size)
{
    if (!field || !values || !size)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_VECTOR_I64)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    *values = (mama_i64_t*)field->mData.data; /* NOT COPYING */
    *size = field->mVectorSize;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_getU64Vector(const mamaFieldCacheField field,
                                             const mama_u64_t** values,
                                             mama_size_t* size)
{
    if (!field || !values || !size)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_VECTOR_U64)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    *values = (mama_u64_t*)field->mData.data; /* NOT COPYING */
    *size = field->mVectorSize;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_getF32Vector(const mamaFieldCacheField field,
                                             const mama_f32_t** values,
                                             mama_size_t* size)
{
    if (!field || !values || !size)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_VECTOR_F32)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    *values = (mama_f32_t*)field->mData.data; /* NOT COPYING */
    *size = field->mVectorSize;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_getF64Vector(const mamaFieldCacheField field,
                                             const mama_f64_t** values,
                                             mama_size_t* size)
{
    if (!field || !values || !size)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_VECTOR_F64)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    *values = (mama_f64_t*)field->mData.data; /* NOT COPYING */
    *size = field->mVectorSize;
    return MAMA_STATUS_OK; 
}

mama_status mamaFieldCacheField_getStringVector(const mamaFieldCacheField field,
                                                const char*** values,
                                                mama_size_t* size)
{
    mamaFieldCacheVector vector = NULL;

    if (!field || !values || !size)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_VECTOR_STRING)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    vector = (mamaFieldCacheVector)field->mData.data;
    if (vector)
    {
        *values = (const char**)vector->mData; /* NOT COPYING */
        *size = field->mVectorSize;
    }
    else
    {
        *values = NULL;
        *size = 0;
    }
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_getPriceVector(const mamaFieldCacheField field,
                                               const mamaPrice** values,
                                               mama_size_t* size)
{
    mamaFieldCacheVector vector = NULL;

    if (!field || !values || !size)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_VECTOR_PRICE)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    vector = (mamaFieldCacheVector)field->mData.data;
    if (vector)
    {
        *values = (mamaPrice*)vector->mData; /* NOT COPYING */
        *size = field->mVectorSize;
    }
    else
    {
        *values = NULL;
        *size = 0;
    }
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_getDateTimeVector(const mamaFieldCacheField field,
                                                  const mamaDateTime** values,
                                                  mama_size_t* size)
{
    mamaFieldCacheVector vector = NULL;

    if (!field || !values || !size)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_VECTOR_TIME)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    vector = (mamaFieldCacheVector)field->mData.data;
    if (vector)
    {
        *values = (mamaDateTime*)vector->mData; /* NOT COPYING */
        *size = field->mVectorSize;
    }
    else
    {
        *values = NULL;
        *size = 0;
    }
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_setBool(mamaFieldCacheField field, mama_bool_t value)
{
    if (!field)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_BOOL)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    field->mData.abool = value;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_setChar(mamaFieldCacheField field, char value)
{
    if (!field)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_CHAR)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    field->mData.chr = value;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_setI8(mamaFieldCacheField field, mama_i8_t value)
{
    if (!field)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_I8)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    field->mData.i8 = value;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_setU8(mamaFieldCacheField field, mama_u8_t value)
{
    if (!field)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_U8)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    field->mData.u8 = value;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_setI16(mamaFieldCacheField field, mama_i16_t value)
{
    if (!field)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_I16)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    field->mData.i16 = value;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_setU16(mamaFieldCacheField field, mama_u16_t value)
{
    if (!field)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_U16)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    field->mData.u16 = value;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_setI32(mamaFieldCacheField field, mama_i32_t value)
{
    if (!field)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_I32)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    field->mData.i32 = value;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_setU32(mamaFieldCacheField field, mama_u32_t value)
{
    if (!field)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_U32)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    field->mData.u32 = value;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_setI64(mamaFieldCacheField field, mama_i64_t value)
{
    if (!field)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_I64)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    field->mData.i64 = value;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_setU64(mamaFieldCacheField field, mama_u64_t value)
{
    if (!field)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_U64)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
   field->mData.u64 = value;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_setF32(mamaFieldCacheField field, mama_f32_t value)
{
    if (!field)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_F32)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    field->mData.f32 = value;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_setF64(mamaFieldCacheField field, mama_f64_t value)
{
    if (!field)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_F64)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    field->mData.f64 = value;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_setString(const mamaFieldCacheField field,
                                          const char* value,
                                          mama_size_t len)
{
    if (!field)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_STRING)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    if (len == 0 && value)
    {
        len = strlen(value);
    }
    field->mVectorSize = len;
    return mamaFieldCacheField_setDataValue(
            field,
            value,/* Copy also string terminator */
            value ? len + 1 : 0);
}

mama_status mamaFieldCacheField_setPrice(const mamaFieldCacheField field,
                                         const mamaPrice value)
{
    mamaPrice price = NULL;
    void* old = NULL;
    
    if (!field || !value)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_PRICE)
    {
        return MAMA_STATUS_INVALID_ARG;
    }

    if (!field->mData.data)
    {
        mamaPrice_create(&price);
        mamaFieldCacheField_setDataPointer(field, (void*)price, &old);
    }
    mamaPrice_copy((mamaPrice)field->mData.data, value);
    
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_setDateTime(const mamaFieldCacheField field,
                                            const mamaDateTime value)
{
    mamaDateTime dateTime = NULL;
    void* old = NULL;
    
    if (!field || !value)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_TIME)
    {
        return MAMA_STATUS_INVALID_ARG;
    }

    if (!field->mData.data)
    {
        mamaDateTime_create(&dateTime);
        mamaFieldCacheField_setDataPointer(field, (void*)dateTime, &old);
    }
    mamaDateTime_copy((mamaDateTime)field->mData.data, value);
    
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_setI8Vector(const mamaFieldCacheField field,
                                            const mama_i8_t* values,
                                            mama_size_t size)
{
    if (!field || !values)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_VECTOR_I8)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    field->mVectorSize = size;
    return mamaFieldCacheField_setDataValue(field, values, size * sizeof(mama_i8_t));
}

mama_status mamaFieldCacheField_setU8Vector(const mamaFieldCacheField field,
                                            const mama_u8_t* values,
                                            mama_size_t size)
{
    if (!field || !values)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_VECTOR_U8)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    field->mVectorSize = size;
    return mamaFieldCacheField_setDataValue(field, values, size * sizeof(mama_i8_t));
}

mama_status mamaFieldCacheField_setI16Vector(const mamaFieldCacheField field,
                                             const mama_i16_t* values,
                                             mama_size_t size)
{
    if (!field || !values)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_VECTOR_I16)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    field->mVectorSize = size;
    return mamaFieldCacheField_setDataValue(field, values, size * sizeof(mama_i16_t));
}

mama_status mamaFieldCacheField_setU16Vector(const mamaFieldCacheField field,
                                             const mama_u16_t* values,
                                             mama_size_t size)
{
    if (!field || !values)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_VECTOR_U16)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    field->mVectorSize = size;
    return mamaFieldCacheField_setDataValue(field, values, size * sizeof(mama_u16_t));
}

mama_status mamaFieldCacheField_setI32Vector(const mamaFieldCacheField field,
                                             const mama_i32_t* values,
                                             mama_size_t size)
{
    if (!field || !values)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_VECTOR_I32)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    field->mVectorSize = size;
    return mamaFieldCacheField_setDataValue(field, values, size * sizeof(mama_i32_t));
}

mama_status mamaFieldCacheField_setU32Vector(const mamaFieldCacheField field,
                                             const mama_u32_t* values,
                                             mama_size_t size)
{
    if (!field || !values)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_VECTOR_U32)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    field->mVectorSize = size;
    return mamaFieldCacheField_setDataValue(field, values, size * sizeof(mama_u32_t));
}

mama_status mamaFieldCacheField_setI64Vector(const mamaFieldCacheField field,
                                             const mama_i64_t* values,
                                             mama_size_t size)
{
    if (!field || !values)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_VECTOR_I64)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    field->mVectorSize = size;
    return mamaFieldCacheField_setDataValue(field, values, size * sizeof(mama_i64_t));
}

mama_status mamaFieldCacheField_setU64Vector(const mamaFieldCacheField field,
                                             const mama_u64_t* values,
                                             mama_size_t size)
{
    if (!field || !values)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_VECTOR_U64)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    field->mVectorSize = size;
    return mamaFieldCacheField_setDataValue(field, values, size * sizeof(mama_u64_t));
}

mama_status mamaFieldCacheField_setF32Vector(const mamaFieldCacheField field,
                                             const mama_f32_t* values,
                                             mama_size_t size)
{
    if (!field || !values)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_VECTOR_F32)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    field->mVectorSize = size;
    return mamaFieldCacheField_setDataValue(field, values, size * sizeof(mama_f32_t));
}

mama_status mamaFieldCacheField_setF64Vector(const mamaFieldCacheField field,
                                             const mama_f64_t* values,
                                             mama_size_t size)
{
    if (!field || !values)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_VECTOR_F64)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    field->mVectorSize = size;
    return mamaFieldCacheField_setDataValue(field, values, size * sizeof(mama_f64_t));
}

mama_status mamaFieldCacheField_setStringVector(const mamaFieldCacheField field,
                                                const char** values,
                                                mama_size_t size)
{
    mama_size_t counter;
    mamaFieldCacheVector vector = NULL;
    void* string = NULL;

    if (!field || !values)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_VECTOR_STRING)
    {
        return MAMA_STATUS_INVALID_ARG;
    }

    if (!field->mData.data)
    {
        mamaFieldCacheVector_create(&vector);
        field->mData.data = (void*)vector;
    }
    mamaFieldCacheVector_grow(field->mData.data, size);
    for (counter = 0; counter < size; counter++)
    {
        mamaFieldCacheVector_get((mamaFieldCacheVector)field->mData.data, counter, &string);
        if (string)
        {
            free(string);
        }
        mamaFieldCacheVector_set((mamaFieldCacheVector)field->mData.data,
                                 counter,
                                 (void*)strdup(values[counter]));
    }
    field->mVectorSize = size;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_setPriceVector(const mamaFieldCacheField field,
                                               const mamaPrice* values,
                                               mama_size_t size)
{
    mama_size_t index;
    mamaFieldCacheVector vector = NULL;
    mamaPrice price = NULL;

    if (!field || !values)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_VECTOR_PRICE)
    {
        return MAMA_STATUS_INVALID_ARG;
    }

    if (!field->mData.data)
    {
        mamaFieldCacheVector_create(&vector);
        field->mData.data = (void*)vector;
    }
    mamaFieldCacheVector_grow(field->mData.data, size);
    for (index = 0; index < size; index++)
    {
        mamaFieldCacheVector_get((mamaFieldCacheVector)field->mData.data,
                                 index,
                                 (void**)&price);
        if (!price)
        {
            mamaPrice_create(&price);
        }
        mamaPrice_copy(price, values[index]);
        mamaFieldCacheVector_set((mamaFieldCacheVector)field->mData.data,
                                 index,
                                 (void*)price);

    }
    field->mVectorSize = size;
    return MAMA_STATUS_OK;
}

mama_status mamaFieldCacheField_setDateTimeVector(const mamaFieldCacheField field,
                                                  const mamaDateTime* values,
                                                  mama_size_t size)
{
    mama_size_t index;
    mamaFieldCacheVector vector = NULL;
    void* item = NULL;
    mamaDateTime dateTime = NULL;

    if (!field || !values)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (field->mType != MAMA_FIELD_TYPE_VECTOR_TIME)
    {
        return MAMA_STATUS_INVALID_ARG;
    }

    if (!field->mData.data)
    {
        mamaFieldCacheVector_create(&vector);
        field->mData.data = (void*)vector;
    }
    mamaFieldCacheVector_grow(field->mData.data, size);
    for (index = 0; index < size; index++)
    {
        mamaFieldCacheVector_get((mamaFieldCacheVector)field->mData.data,
                                 index,
                                 &item);
        dateTime = (mamaDateTime)item;
        if (!dateTime)
        {
            mamaDateTime_create(&dateTime);
        }
        mamaDateTime_copy(dateTime, values[index]);
        mamaFieldCacheVector_set((mamaFieldCacheVector)field->mData.data,
                                 index,
                                 (void*)dateTime);
    }
    field->mVectorSize = size;
    return MAMA_STATUS_OK;
}
