/* $Id: //commsof_depot/platform/products/6.0/trunk/mama/c_cpp/src/cpp/fieldcache/MamaFieldCacheFieldTypes.cpp#1 $
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

#include <mama/fieldcache/MamaFieldCacheFieldTypes.h>
#include <mama/fieldcache/fieldcachefield.h>
#include <mama/price.h>
#include <string>
#include <sstream>

namespace Wombat
{

// MAMA_FIELD_TYPE_CHAR
template <>
char MamaFieldCacheFieldChar::get(const MamaFieldCacheField& field) const
{
    checkType(field);
    char res;
    mamaFieldCacheField_getChar(field.getCValue(), &res);
    return res;
}
template <>
void MamaFieldCacheFieldChar::set(MamaFieldCacheField& field, const char& value)
{
    checkType(field);
    mamaFieldCacheField_setChar(field.getCValue(), value);
}

// MAMA_FIELD_TYPE_BOOL
template <>
mama_bool_t MamaFieldCacheFieldBool::get(const MamaFieldCacheField& field) const
{
    checkType(field);
    mama_bool_t res;
    mamaFieldCacheField_getBool(field.getCValue(), &res);
    return res;
}
template <>
void MamaFieldCacheFieldBool::set(MamaFieldCacheField& field, const mama_bool_t& value)
{
    checkType(field);
    mamaFieldCacheField_setBool(field.getCValue(), value);
}

// MAMA_FIELD_TYPE_I8
template <>
mama_i8_t MamaFieldCacheFieldI8::get(const MamaFieldCacheField& field) const
{
    checkType(field);
    mama_i8_t res;
    mamaFieldCacheField_getI8(field.getCValue(), &res);
    return res;
}
template <>
void MamaFieldCacheFieldI8::set(MamaFieldCacheField& field, const mama_i8_t& value)
{
    checkType(field);
    mamaFieldCacheField_setI8(field.getCValue(), value);
}

// MAMA_FIELD_TYPE_U8
template <>
mama_u8_t MamaFieldCacheFieldU8::get(const MamaFieldCacheField& field) const
{
    checkType(field);
    mama_u8_t res;
    mamaFieldCacheField_getU8(field.getCValue(), &res);
    return res;
}
template <>
void MamaFieldCacheFieldU8::set(MamaFieldCacheField& field, const mama_u8_t& value)
{
    checkType(field);
    mamaFieldCacheField_setU8(field.getCValue(), value);
}

// MAMA_FIELD_TYPE_I16
template <>
mama_i16_t MamaFieldCacheFieldI16::get(const MamaFieldCacheField& field) const
{
    checkType(field);
    mama_i16_t res;
    mamaFieldCacheField_getI16(field.getCValue(), &res);
    return res;
}
template <>
void MamaFieldCacheFieldI16::set(MamaFieldCacheField& field, const mama_i16_t& value)
{
    checkType(field);
    mamaFieldCacheField_setI16(field.getCValue(), value);
}

// MAMA_FIELD_TYPE_U16
template <>
mama_u16_t MamaFieldCacheFieldU16::get(const MamaFieldCacheField& field) const
{
    checkType(field);
    mama_u16_t res;
    mamaFieldCacheField_getU16(field.getCValue(), &res);
    return res;
}
template <>
void MamaFieldCacheFieldU16::set(MamaFieldCacheField& field, const mama_u16_t& value)
{
    checkType(field);
    mamaFieldCacheField_setU16(field.getCValue(), value);
}

// MAMA_FIELD_TYPE_I32
template <>
mama_i32_t MamaFieldCacheFieldI32::get(const MamaFieldCacheField& field) const
{
    checkType(field);
    mama_i32_t res;
    mamaFieldCacheField_getI32(field.getCValue(), &res);
    return res;
}
template <>
void MamaFieldCacheFieldI32::set(MamaFieldCacheField& field, const mama_i32_t& value)
{
    checkType(field);
    mamaFieldCacheField_setI32(field.getCValue(), value);
}

// MAMA_FIELD_TYPE_U32
template <>
mama_u32_t MamaFieldCacheFieldU32::get(const MamaFieldCacheField& field) const
{
    checkType(field);
    mama_u32_t res;
    mamaFieldCacheField_getU32(field.getCValue(), &res);
    return res;
}
template <>
void MamaFieldCacheFieldU32::set(MamaFieldCacheField& field, const mama_u32_t& value)
{
    checkType(field);
    mamaFieldCacheField_setU32(field.getCValue(), value);
}

// MAMA_FIELD_TYPE_I64
template <>
mama_i64_t MamaFieldCacheFieldI64::get(const MamaFieldCacheField& field) const
{
    checkType(field);
    mama_i64_t res;
    mamaFieldCacheField_getI64(field.getCValue(), &res);
    return res;
}
template <>
void MamaFieldCacheFieldI64::set(MamaFieldCacheField& field, const mama_i64_t& value)
{
    checkType(field);
    mamaFieldCacheField_setI64(field.getCValue(), value);
}

// MAMA_FIELD_TYPE_U64
template <>
mama_u64_t MamaFieldCacheFieldU64::get(const MamaFieldCacheField& field) const
{
    checkType(field);
    mama_u64_t res;
    mamaFieldCacheField_getU64(field.getCValue(), &res);
    return res;
}
template <>
void MamaFieldCacheFieldU64::set(MamaFieldCacheField& field, const mama_u64_t& value)
{
    checkType(field);
    mamaFieldCacheField_setU64(field.getCValue(), value);
}

// MAMA_FIELD_TYPE_F32
template <>
mama_f32_t MamaFieldCacheFieldF32::get(const MamaFieldCacheField& field) const
{
    checkType(field);
    mama_f32_t res;
    mamaFieldCacheField_getF32(field.getCValue(), &res);
    return res;
}
template <>
void MamaFieldCacheFieldF32::set(MamaFieldCacheField& field, const mama_f32_t& value)
{
    checkType(field);
    mamaFieldCacheField_setF32(field.getCValue(), value);
}

// MAMA_FIELD_TYPE_F64
template <>
mama_f64_t MamaFieldCacheFieldF64::get(const MamaFieldCacheField& field) const
{
    checkType(field);
    mama_f64_t res;
    mamaFieldCacheField_getF64(field.getCValue(), &res);
    return res;
}
template <>
void MamaFieldCacheFieldF64::set(MamaFieldCacheField& field, const mama_f64_t& value)
{
    checkType(field);
    mamaFieldCacheField_setF64(field.getCValue(), value);
}

// MAMA_FIELD_TYPE_STRING
void MamaFieldCacheFieldString::set(MamaFieldCacheField& field,
                                    const char* value,
                                    mama_size_t len)
{
    checkType(field);
    mamaFieldCacheField_setString(field.getCValue(), value, len);
}
const char* MamaFieldCacheFieldString::get(const MamaFieldCacheField& field) const
{
    checkType(field);
    const char* res = NULL;
    mama_size_t len = 0;
    mamaFieldCacheField_getString(field.getCValue(), &res, &len);
    return res;
}

// MAMA_FIELD_TYPE_PRICE
void MamaFieldCacheFieldPrice::set(MamaFieldCacheField& field, const MamaPrice& value)
{
    checkType(field);
    mamaFieldCacheField_setPrice(field.getCValue(), value.getCValue());
}    
const MamaPrice& MamaFieldCacheFieldPrice::get(const MamaFieldCacheField& field) const
{
    checkType(field);
    mamaPrice res = NULL;
    mamaFieldCacheField_getPrice(field.getCValue(), &res);
    // res is pointer to the field in the cache - need to make a copy here
    // otherwise the object containing MamaPrice returned would destroy the C
    // field present in cache.
    mamaPrice_copy(mPrice.getCValue(), res);
    return mPrice;
}

// MAMA_FIELD_TYPE_TIME
void MamaFieldCacheFieldDateTime::set(MamaFieldCacheField& field, const MamaDateTime& value)
{
    checkType(field);
    mamaFieldCacheField_setDateTime(field.getCValue(), value.getCValue());
}    
const MamaDateTime& MamaFieldCacheFieldDateTime::get(const MamaFieldCacheField& field) const
{
    checkType(field);
    mamaDateTime res = NULL;
    mamaFieldCacheField_getDateTime(field.getCValue(), &res);
    // res is pointer to the field in the cache - need to make a copy here
    // otherwise the object containing MamaDateTime returned would destroy the C
    // field present in cache.
    mamaDateTime_copy(mDateTime.getCValue(), res);
    return mDateTime;
}

// MAMA_FIELD_TYPE_VECTOR_I8
template <>
void MamaFieldCacheFieldI8Vector::set(MamaFieldCacheField& field,
                                     const mama_i8_t* values,
                                     mama_size_t size)
{
    checkType(field);
    mamaFieldCacheField_setI8Vector(field.getCValue(), values, size);
}
template <>
void MamaFieldCacheFieldI8Vector::get(const MamaFieldCacheField& field,
                                      const mama_i8_t*& values,
                                      mama_size_t& size) const
{
    checkType(field);
    mamaFieldCacheField_getI8Vector(field.getCValue(), &values, &size);
}
template <>
const mama_i8_t& MamaFieldCacheFieldI8Vector::get(const MamaFieldCacheField& field,
                                                  mama_size_t index) const
{
    checkType(field);
    const mama_i8_t* values = NULL;
    mama_size_t size = 0;
    mamaFieldCacheField_getI8Vector(field.getCValue(), &values, &size);
    if (index < size)
        return values[index];
    throw std::out_of_range("MamaFieldCacheFieldI8Vector::get");
}

// MAMA_FIELD_TYPE_VECTOR_U8
template <>
void MamaFieldCacheFieldU8Vector::set(MamaFieldCacheField& field,
                                      const mama_u8_t* values,
                                      mama_size_t size)
{
    checkType(field);
    mamaFieldCacheField_setU8Vector(field.getCValue(), values, size);
}
template <>
void MamaFieldCacheFieldU8Vector::get(const MamaFieldCacheField& field,
                                      const mama_u8_t*& values,
                                      mama_size_t& size) const
{
    checkType(field);
    mamaFieldCacheField_getU8Vector(field.getCValue(), &values, &size);
}
template <>
const mama_u8_t& MamaFieldCacheFieldU8Vector::get(const MamaFieldCacheField& field,
                                                  mama_size_t index) const
{
    const mama_u8_t* values = NULL;
    mama_size_t size = 0;
    mamaFieldCacheField_getU8Vector(field.getCValue(), &values, &size);
    if (index < size)
        return values[index];
    throw std::out_of_range("MamaFieldCacheFieldU8Vector::get");
}

// MAMA_FIELD_TYPE_VECTOR_I16
template <>
void MamaFieldCacheFieldI16Vector::set(MamaFieldCacheField& field,
                                       const mama_i16_t* values,
                                       mama_size_t size)
{
    checkType(field);
    mamaFieldCacheField_setI16Vector(field.getCValue(), values, size);
}
template <>
void MamaFieldCacheFieldI16Vector::get(const MamaFieldCacheField& field,
                                       const mama_i16_t*& values,
                                       mama_size_t& size) const
{
    checkType(field);
    mamaFieldCacheField_getI16Vector(field.getCValue(), &values, &size);
}
template <>
const mama_i16_t& MamaFieldCacheFieldI16Vector::get(const MamaFieldCacheField& field,
                                                    mama_size_t index) const
{
    checkType(field);
    const mama_i16_t* values = NULL;
    mama_size_t size = 0;
    mamaFieldCacheField_getI16Vector(field.getCValue(), &values, &size);
    if (index < size)
        return values[index];
    throw std::out_of_range("MamaFieldCacheFieldI16Vector::get");
}

// MAMA_FIELD_TYPE_VECTOR_U16
template <>
void MamaFieldCacheFieldU16Vector::set(MamaFieldCacheField& field,
                                       const mama_u16_t* values,
                                       mama_size_t size)
{
    checkType(field);
    mamaFieldCacheField_setU16Vector(field.getCValue(), values, size);
}
template <>
void MamaFieldCacheFieldU16Vector::get(const MamaFieldCacheField& field,
                                       const mama_u16_t*& values,
                                       mama_size_t& size) const
{
    checkType(field);
    mamaFieldCacheField_getU16Vector(field.getCValue(), &values, &size);
}
template <>
const mama_u16_t& MamaFieldCacheFieldU16Vector::get(const MamaFieldCacheField& field,
                                                    mama_size_t index) const
{
    checkType(field);
    const mama_u16_t* values = NULL;
    mama_size_t size = 0;
    mamaFieldCacheField_getU16Vector(field.getCValue(), &values, &size);
    if (index < size)
        return values[index];
    throw std::out_of_range("MamaFieldCacheFieldU16Vector::get");
}

// MAMA_FIELD_TYPE_VECTOR_I32
template <>
void MamaFieldCacheFieldI32Vector::set(MamaFieldCacheField& field,
                                       const mama_i32_t* values,
                                       mama_size_t size)
{
    checkType(field);
    mamaFieldCacheField_setI32Vector(field.getCValue(), values, size);
}
template <>
void MamaFieldCacheFieldI32Vector::get(const MamaFieldCacheField& field,
                                       const mama_i32_t*& values,
                                       mama_size_t& size) const
{
    checkType(field);
    mamaFieldCacheField_getI32Vector(field.getCValue(), &values, &size);
}
template <>
const mama_i32_t& MamaFieldCacheFieldI32Vector::get(const MamaFieldCacheField& field,
                                                    mama_size_t index) const
{
    checkType(field);
    const mama_i32_t* values = NULL;
    mama_size_t size = 0;
    mamaFieldCacheField_getI32Vector(field.getCValue(), &values, &size);
    if (index < size)
        return values[index];
    throw std::out_of_range("MamaFieldCacheFieldI32Vector::get");
}

// MAMA_FIELD_TYPE_VECTOR_U32
template <>
void MamaFieldCacheFieldU32Vector::set(MamaFieldCacheField& field,
                                       const mama_u32_t* values,
                                       mama_size_t size)
{
    checkType(field);
    mamaFieldCacheField_setU32Vector(field.getCValue(), values, size);
}
template <>
void MamaFieldCacheFieldU32Vector::get(const MamaFieldCacheField& field,
                                       const mama_u32_t*& values,
                                       mama_size_t& size) const
{
    checkType(field);
    mamaFieldCacheField_getU32Vector(field.getCValue(), &values, &size);
}
template <>
const mama_u32_t& MamaFieldCacheFieldU32Vector::get(const MamaFieldCacheField& field,
                                                    mama_size_t index) const
{
    checkType(field);
    const mama_u32_t* values = NULL;
    mama_size_t size = 0;
    mamaFieldCacheField_getU32Vector(field.getCValue(), &values, &size);
    if (index < size)
        return values[index];
    throw std::out_of_range("MamaFieldCacheFieldU32Vector::get");
}

// MAMA_FIELD_TYPE_VECTOR_I64
template <>
void MamaFieldCacheFieldI64Vector::set(MamaFieldCacheField& field,
                                       const mama_i64_t* values,
                                       mama_size_t size)
{
    mamaFieldCacheField_setI64Vector(field.getCValue(), values, size);
}
template <>
void MamaFieldCacheFieldI64Vector::get(const MamaFieldCacheField& field,
                                       const mama_i64_t*& values,
                                       mama_size_t& size) const
{
    checkType(field);
    mamaFieldCacheField_getI64Vector(field.getCValue(), &values, &size);
}
template <>
const mama_i64_t& MamaFieldCacheFieldI64Vector::get(const MamaFieldCacheField& field,
                                                    mama_size_t index) const
{
    checkType(field);
    const mama_i64_t* values = NULL;
    mama_size_t size = 0;
    mamaFieldCacheField_getI64Vector(field.getCValue(), &values, &size);
    if (index < size)
        return values[index];
    throw std::out_of_range("MamaFieldCacheFieldI64Vector::get");
}

// MAMA_FIELD_TYPE_VECTOR_U64
template <>
void MamaFieldCacheFieldU64Vector::set(MamaFieldCacheField& field,
                                       const mama_u64_t* values,
                                       mama_size_t size)
{
    checkType(field);
    mamaFieldCacheField_setU64Vector(field.getCValue(), values, size);
}
template <>
void MamaFieldCacheFieldU64Vector::get(const MamaFieldCacheField& field,
                                       const mama_u64_t*& values,
                                       mama_size_t& size) const
{
    checkType(field);
    mamaFieldCacheField_getU64Vector(field.getCValue(), &values, &size);
}
template <>
const mama_u64_t& MamaFieldCacheFieldU64Vector::get(const MamaFieldCacheField& field,
                                                    mama_size_t index) const
{
    checkType(field);
    const mama_u64_t* values = NULL;
    mama_size_t size = 0;
    mamaFieldCacheField_getU64Vector(field.getCValue(), &values, &size);
    if (index < size)
        return values[index];
    throw std::out_of_range("MamaFieldCacheFieldU64Vector::get");
}

// MAMA_FIELD_TYPE_VECTOR_F32
template <>
void MamaFieldCacheFieldF32Vector::set(MamaFieldCacheField& field,
                                       const mama_f32_t* values,
                                       mama_size_t size)
{
    checkType(field);
    mamaFieldCacheField_setF32Vector(field.getCValue(), values, size);
}
template <>
void MamaFieldCacheFieldF32Vector::get(const MamaFieldCacheField& field,
                                       const mama_f32_t*& values,
                                       mama_size_t& size) const
{
    checkType(field);
    mamaFieldCacheField_getF32Vector(field.getCValue(), &values, &size);
}
template <>
const mama_f32_t& MamaFieldCacheFieldF32Vector::get(const MamaFieldCacheField& field,
                                                    mama_size_t index) const
{
    checkType(field);
    const mama_f32_t* values = NULL;
    mama_size_t size = 0;
    mamaFieldCacheField_getF32Vector(field.getCValue(), &values, &size);
    if (index < size)
        return values[index];
    throw std::out_of_range("MamaFieldCacheFieldF32Vector::get");
}

// MAMA_FIELD_TYPE_VECTOR_F64
template <>
void MamaFieldCacheFieldF64Vector::set(MamaFieldCacheField& field,
                                       const mama_f64_t* values,
                                       mama_size_t size)
{
    checkType(field);
    mamaFieldCacheField_setF64Vector(field.getCValue(), values, size);
}
template <>
void MamaFieldCacheFieldF64Vector::get(const MamaFieldCacheField& field,
                                       const mama_f64_t*& values,
                                       mama_size_t& size) const
{
    checkType(field);
    mamaFieldCacheField_getF64Vector(field.getCValue(), &values, &size);
}
template <>
const mama_f64_t& MamaFieldCacheFieldF64Vector::get(const MamaFieldCacheField& field,
                                                    mama_size_t index) const
{
    checkType(field);
    const mama_f64_t* values = NULL;
    mama_size_t size = 0;
    mamaFieldCacheField_getF64Vector(field.getCValue(), &values, &size);
    if (index < size)
        return values[index];
    throw std::out_of_range("MamaFieldCacheFieldF64Vector::get");
}

// MAMA_FIELD_TYPE_VECTOR_STRING
void MamaFieldCacheFieldStringVector::set(MamaFieldCacheField& field,
                                          const char** values,
                                          mama_size_t size)
{
    checkType(field);
    mamaFieldCacheField_setStringVector(field.getCValue(), values, size);
}
void MamaFieldCacheFieldStringVector::get(const MamaFieldCacheField& field,
                                          const char**& values,
                                          mama_size_t& size) const
{
    checkType(field);
    mamaFieldCacheField_getStringVector(field.getCValue(), &values, &size);
}
const char* MamaFieldCacheFieldStringVector::get(const MamaFieldCacheField& field,
                                                 mama_size_t index) const
{
    checkType(field);
    const char** values = NULL;
    mama_size_t size = 0;
    mamaFieldCacheField_getStringVector(field.getCValue(), &values, &size);
    if (index < size)
    {
        return values[index];
    }
    return NULL;
}

// MAMA_FIELD_TYPE_VECTOR_PRICE
void MamaFieldCacheFieldPriceVector::set(MamaFieldCacheField& field,
                                         const MamaPrice* values,
                                         mama_size_t size)
{
    checkType(field);
    mamaPrice* cvalues = new mamaPrice[size];
    for (mama_size_t i = 0; i < size; ++i)
    {
        cvalues[i] = values[i].getCValue();
    }
    mamaFieldCacheField_setPriceVector(field.getCValue(), cvalues, size);
    delete[] cvalues;
}
void MamaFieldCacheFieldPriceVector::get(const MamaFieldCacheField& field,
                                         const MamaPrice*& values,
                                         mama_size_t& size) const
{
    checkType(field);
    const mamaPrice* cvalues = NULL;
    mamaFieldCacheField_getPriceVector(field.getCValue(), &cvalues, &size);
    grow(size);
    for (mama_size_t i = 0; i < size; ++i)
    {
        // We should avoid the copy (if we could assign the CValue)
        mamaPrice_copy(mValues[i].getCValue(), cvalues[i]);
    }
    values = mValues;
}
const MamaPrice* MamaFieldCacheFieldPriceVector::get(const MamaFieldCacheField& field,
                                                     mama_size_t index) const
{
    checkType(field);
    const mamaPrice* values = NULL;
    mama_size_t size = 0;
    mamaFieldCacheField_getPriceVector(field.getCValue(), &values, &size);
    if (index < size)
    {
        // We should avoid the copy (if we could assign the CValue)
        mamaPrice_copy(mValue.getCValue(), values[index]);
        return &mValue;
    }
    return NULL;
}

// MAMA_FIELD_TYPE_VECTOR_TIME
void MamaFieldCacheFieldDateTimeVector::set(MamaFieldCacheField& field,
                                            const MamaDateTime* values,
                                            mama_size_t size)
{
    checkType(field);
    mamaDateTime* cvalues = new mamaDateTime[size];
    for (mama_size_t i = 0; i < size; ++i)
    {
        cvalues[i] = values[i].getCValue();
    }
    mamaFieldCacheField_setDateTimeVector(field.getCValue(), cvalues, size);
    delete[] cvalues;
}
void MamaFieldCacheFieldDateTimeVector::get(const MamaFieldCacheField& field,
                                            const MamaDateTime*& values,
                                            mama_size_t& size) const
{
    checkType(field);
    const mamaDateTime* cvalues = NULL;
    mamaFieldCacheField_getDateTimeVector(field.getCValue(), &cvalues, &size);
    grow(size);
    for (mama_size_t i = 0; i < size; ++i)
    {
        // We should avoid the copy (if we could assign the CValue)
        mamaDateTime_copy(mValues[i].getCValue(), cvalues[i]);
    }
    values = mValues;
}
const MamaDateTime* MamaFieldCacheFieldDateTimeVector::get(const MamaFieldCacheField& field,
                                                           mama_size_t index) const
{
    checkType(field);
    const mamaDateTime* values;
    mama_size_t size;
    mamaFieldCacheField_getDateTimeVector(field.getCValue(), &values, &size);
    if (index < size)
    {
        // We should avoid the copy (if we could assign the CValue)
        mamaDateTime_copy(mValue.getCValue(), values[index]);
        return &mValue;
    }
    return NULL;
}


void setFieldValue(MamaFieldCacheField& field, const char* value)
{
    MamaFieldCacheFieldString setField;
    setField.set(field, value);
}

void setFieldValue(MamaFieldCacheField& field, const MamaPrice& value)
{
    MamaFieldCacheFieldPrice setField;
    setField.set(field, value);
}

void setFieldValue(MamaFieldCacheField& field, const MamaDateTime& value)
{
    MamaFieldCacheFieldDateTime setField;
    setField.set(field, value);
}

void getFieldValue(const MamaFieldCacheField& field, const char*& value)
{
    MamaFieldCacheFieldString getField;
    value = getField.get(field);
}

void getFieldValue(const MamaFieldCacheField& field, MamaPrice& value)
{
    MamaFieldCacheFieldPrice getField;
    value = getField.get(field);
}

void getFieldValue(const MamaFieldCacheField& field, MamaDateTime& value)
{
    MamaFieldCacheFieldDateTime getField;
    value = getField.get(field);
}

void getFieldValue(const MamaFieldCacheField& field,
                   const char**& values,
                   mama_size_t& size)
{
    MamaFieldCacheFieldStringVector getField;
    getField.get(field, values, size);
}

void getFieldValue(const MamaFieldCacheField& field,
                   const MamaPrice*& values,
                   mama_size_t& size)
{
//    MamaFieldCacheFieldPriceVector getField;
//    getField.get(field, values, size);
    throw "getFieldValue for MamaPrice vector not implemented";
}

void getFieldValue(const MamaFieldCacheField& field,
                   const MamaDateTime*& values,
                   mama_size_t& size)
{
//    MamaFieldCacheFieldDateTimeVector getField;
//    getField.get(field, values, size);
    throw "getFieldValue for MamaDateTime vector not implemented";
}


void mamaFieldCacheFieldFromString(MamaFieldCacheField& field, const std::string& value)
{
    std::istringstream is(value);
    if (field.getType() == MAMA_FIELD_TYPE_STRING)
    {
        MamaFieldCacheFieldString f;
        f.set(field, value.c_str(), value.size());
        return;
    }
    if (field.getType() == MAMA_FIELD_TYPE_PRICE)
    {
        MamaPrice val;
        val.setFromString(value.c_str());
        MamaFieldCacheFieldPrice f;
        f.set(field, val);
        return;
    }
    if (field.getType() == MAMA_FIELD_TYPE_TIME)
    {
        MamaDateTime val;
        val.setFromString(value.c_str());
        MamaFieldCacheFieldDateTime f;
        f.set(field, val);
        return;
    }
    switch (field.getType())
    {
        case MAMA_FIELD_TYPE_CHAR:
        {
            char val;
            is >> val;
            setV(Type<MAMA_FIELD_TYPE_CHAR>(), field, val);
            break;
        }
        case MAMA_FIELD_TYPE_BOOL:
        {
            mama_bool_t val;
            is >> val;
            setV(Type<MAMA_FIELD_TYPE_BOOL>(), field, val);
            break;
        }
        case MAMA_FIELD_TYPE_I8:
        {
            mama_i8_t val;
            is >> val;
            setV(Type<MAMA_FIELD_TYPE_I8>(), field, val);
            break;
        }
        case MAMA_FIELD_TYPE_U8:
        {
            mama_u8_t val;
            is >> val;
            setV(Type<MAMA_FIELD_TYPE_U8>(), field, val);
            break;
        }
        case MAMA_FIELD_TYPE_I16:
        {
            mama_i16_t val;
            is >> val;
            setV(Type<MAMA_FIELD_TYPE_I16>(), field, val);
            break;
        }
        case MAMA_FIELD_TYPE_U16:
        {
            mama_u16_t val;
            is >> val;
            setV(Type<MAMA_FIELD_TYPE_U16>(), field, val);
            break;
        }
        case MAMA_FIELD_TYPE_I32:
        {
            mama_i32_t val;
            is >> val;
            setV(Type<MAMA_FIELD_TYPE_I32>(), field, val);
            break;
        }
        case MAMA_FIELD_TYPE_U32:
        {
            mama_u32_t val;
            is >> val;
            setV(Type<MAMA_FIELD_TYPE_U32>(), field, val);
            break;
        }
        case MAMA_FIELD_TYPE_I64:
        {
            mama_i64_t val;
            is >> val;
            setV(Type<MAMA_FIELD_TYPE_I64>(), field, val);
            break;
        }
        case MAMA_FIELD_TYPE_U64:
        {
            mama_u64_t val;
            is >> val;
            setV(Type<MAMA_FIELD_TYPE_U64>(), field, val);
            break;
        }
        case MAMA_FIELD_TYPE_F32:
        {
            mama_f32_t val;
            is >> val;
            setV(Type<MAMA_FIELD_TYPE_F32>(), field, val);
            break;
        }
        case MAMA_FIELD_TYPE_F64:
        {
            mama_f64_t val;
            is >> val;
            setV(Type<MAMA_FIELD_TYPE_F64>(), field, val);
            break;
        }
        default:
            break;
    }
}

} // namespace Wombat
