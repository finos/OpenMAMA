/* $Id: //commsof_depot/platform/products/6.0/trunk/mama/c_cpp/src/c/fieldcache/fieldcachefield.h#1 $
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

#ifndef MamaFieldCacheFieldH__
#define MamaFieldCacheFieldH__

#include <mama/config.h>
#include <mama/fielddesc.h>
#include <mama/types.h>
#include <mama/status.h>
#include <mama/fieldcache/fieldcachetypes.h>

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */

/**
 * This function will check if a specific mama field type can be used to create
 * a <code>mamaFieldCacheField</code>.
 *
 * @param type (in) The mamaFieldType to check.
 * @return Whether the type can be used to create a mamaFieldCache field.
 */
MAMAExpDLL
extern mama_bool_t
mamaFieldCacheField_isSupported(mamaFieldType type);

/**
 * This function will create a mamaFieldCacheField that can be stored in a
 * mamaFieldCache.
 *
 * @param field (out) The mamaFieldCacheField to create.
 * @param fid (in) The field id of the new field.
 * @param type (in) The type of the new field.
 * @param type (in) The name of the new field (optional: can be NULL).
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NOMEM
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_create(mamaFieldCacheField* field,
                           mama_fid_t fid,
                           mamaFieldType type,
                           const char* name);

/**
 * This function will destroy a <code>mamaFieldCacheField</code> previously
 * allocated by a call to <code>mamaFieldCacheField_create</code>.
 *
 * @param field (in) The field to destroy.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_destroy(mamaFieldCacheField field);

/**
 * This function will copy a <code>mamaFieldCacheField</code> into another one.
 *
 * @param field (in) The source field to copy from.
 * @param copy (out) The destination field to copy to.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_copy(const mamaFieldCacheField field, mamaFieldCacheField copy);

/**
 * This function will get the field id of a <code>mamaFieldCacheField</code>.
 *
 * @param field (in) The field.
 * @param fid (out) The field id of the field.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_getFid(mamaFieldCacheField field, mama_fid_t* fid);

/**
 * This function will get the field type of a <code>mamaFieldCacheField</code>.
 *
 * @param field (in) The field.
 * @param type (out) The type of the field.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_getType(mamaFieldCacheField field, mamaFieldType* type);

/**
 * This function will get the descriptor of a <code>mamaFieldCacheField</code>,
 * if present.
 *
 * @param field (in) The field.
 * @param name (out) The descriptor of the field (can be NULL).
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_getName(mamaFieldCacheField field, const char** name);

/**
 * This function will set the descriptor of a <code>mamaFieldCacheField</code>.
 *
 * @param field (in) The field.
 * @param desc (in) The <code>mamaFieldDescriptor</code> of the field (can be NULL).
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_setDescriptor(mamaFieldCacheField field,
                                  const mamaFieldDescriptor desc);

/**
 * This function will get the descriptor of a <code>mamaFieldCacheField</code>.
 *
 * @param field (in) The field.
 * @param desc (out) The <code>mamaFieldDescriptor</code> of the field (can be NULL).
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_getDescriptor(mamaFieldCacheField field,
                                  const mamaFieldDescriptor* desc);

/**
 * This function will return whether the field has been modified.
 *
 * @param field (in) The field.
 * @param modified (out) The modified state of the field.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_isModified(mamaFieldCacheField field, mama_bool_t* modified);


/**
 * This function will set the publish flag of a <code>mamaFieldCacheField</code>.
 * If set to false, the field will never be published.
 *
 * @param field (in) The field.
 * @param publish (in) The publish flag of the field.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_setPublish(mamaFieldCacheField field, mama_bool_t publish);

/**
 * This function will get the publish flag of a <code>mamaFieldCacheField</code>.
 *
 * @param field (in) The field.
 * @param publish (out) The publish flag of the field.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_getPublish(mamaFieldCacheField field, mama_bool_t* publish);

/**
 * This function will set the checkModifed flag of a <code>mamaFieldCacheField</code>.
 * If set to true, the field will be published only if modified.
 * 
 * @param field (in) The field.
 * @param check (in) The checkModified flag.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_setCheckModified(mamaFieldCacheField field, mama_bool_t check);

/**
 * The function will get the checkModified flag of a <code>mamaFieldCacheField</code>.
 * 
 * @param field (in) The field.
 * @param check (out) The checkModified flag of the field.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_getCheckModified(mamaFieldCacheField field, mama_bool_t* check);

/**
 * This function will get the value a <code>mamaFieldCacheField</code> of type BOOL.
 *
 * @param field (in) The field.
 * @param result (out) The value of the field.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_getBool (const mamaFieldCacheField field, mama_bool_t* result);

/**
 * This function will get the value a <code>mamaFieldCacheField</code> of type CHAR.
 *
 * @param field (in) The field.
 * @param result (out) The value of the field.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_getChar(const mamaFieldCacheField field, char* result);

/**
 * This function will get the value a <code>mamaFieldCacheField</code> of type I8.
 *
 * @param field (in) The field.
 * @param result (out) The value of the field.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_getI8(const mamaFieldCacheField field, mama_i8_t* result);

/**
 * This function will get the value a <code>mamaFieldCacheField</code> of type U8.
 *
 * @param field (in) The field.
 * @param result (out) The value of the field.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_getU8(const mamaFieldCacheField field, mama_u8_t* result);

/**
 * This function will get the value a <code>mamaFieldCacheField</code> of type I16.
 *
 * @param field (in) The field.
 * @param result (out) The value of the field.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_getI16(const mamaFieldCacheField field, mama_i16_t* result);

/**
 * This function will get the value a <code>mamaFieldCacheField</code> of type U16.
 *
 * @param field (in) The field.
 * @param result (out) The value of the field.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_getU16(const mamaFieldCacheField field, mama_u16_t* result);

/**
 * This function will get the value a <code>mamaFieldCacheField</code> of type I32.
 *
 * @param field (in) The field.
 * @param result (out) The value of the field.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_getI32(const mamaFieldCacheField field, mama_i32_t* result);

/**
 * This function will get the value a <code>mamaFieldCacheField</code> of type U32.
 *
 * @param field (in) The field.
 * @param result (out) The value of the field.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_getU32(const mamaFieldCacheField field, mama_u32_t* result);

/**
 * This function will get the value a <code>mamaFieldCacheField</code> of type I64.
 *
 * @param field (in) The field.
 * @param result (out) The value of the field.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_getI64(const mamaFieldCacheField field, mama_i64_t* result);

/**
 * This function will get the value a <code>mamaFieldCacheField</code> of type U64.
 *
 * @param field (in) The field.
 * @param result (out) The value of the field.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_getU64(const mamaFieldCacheField field, mama_u64_t* result);

/**
 * This function will get the value a <code>mamaFieldCacheField</code> of type F32.
 *
 * @param field (in) The field.
 * @param result (out) The value of the field.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_getF32(const mamaFieldCacheField field, mama_f32_t* result);

/**
 * This function will get the value a <code>mamaFieldCacheField</code> of type F64.
 *
 * @param field (in) The field.
 * @param result (out) The value of the field.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_getF64(const mamaFieldCacheField field, mama_f64_t* result);

/**
 * This function will get the value a <code>mamaFieldCacheField</code> of type STRING.
 *
 * @param field (in) The field.
 * @param result (out) The value of the field.
 * @param len (out) The length of the string.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_getString(const mamaFieldCacheField field,
                              const char** result,
                              mama_size_t* len);

/**
 * This function will get the value a <code>mamaFieldCacheField</code> of type PRICE.
 *
 * @param field (in) The field.
 * @param result (out) The value of the field.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_getPrice(const mamaFieldCacheField field,
                             const mamaPrice* result);

/**
 * This function will get the value a <code>mamaFieldCacheField</code> of type TIME.
 *
 * @param field (in) The field.
 * @param result (out) The value of the field.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_getDateTime(const mamaFieldCacheField field,
                                const mamaDateTime* result);

/**
 * This function will get the values a <code>mamaFieldCacheField</code> of type I8 vector.
 *
 * @param field (in) The field.
 * @param values (out) The array of values of the field.
 * @param size (out) The number of values.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_getI8Vector(const mamaFieldCacheField field,
                                const mama_i8_t** values,
                                mama_size_t* size);

/**
 * This function will get the values a <code>mamaFieldCacheField</code> of type U8 vector.
 *
 * @param field (in) The field.
 * @param values (out) The array of values of the field.
 * @param size (out) The number of values.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_getU8Vector(const mamaFieldCacheField field,
                                const mama_u8_t** values,
                                mama_size_t* size);

/**
 * This function will get the values a <code>mamaFieldCacheField</code> of type I16 vector.
 *
 * @param field (in) The field.
 * @param values (out) The array of values of the field.
 * @param size (out) The number of values.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_getI16Vector(const mamaFieldCacheField field,
                                 const mama_i16_t** values,
                                 mama_size_t* size);

/**
 * This function will get the values a <code>mamaFieldCacheField</code> of type U16 vector.
 *
 * @param field (in) The field.
 * @param values (out) The array of values of the field.
 * @param size (out) The number of values.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_getU16Vector(const mamaFieldCacheField field,
                                 const mama_u16_t** values,
                                 mama_size_t* size);

/**
 * This function will get the values a <code>mamaFieldCacheField</code> of type I32 vector.
 *
 * @param field (in) The field.
 * @param values (out) The array of values of the field.
 * @param size (out) The number of values.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_getI32Vector(const mamaFieldCacheField field,
                                 const mama_i32_t** values,
                                 mama_size_t* size);

/**
 * This function will get the values a <code>mamaFieldCacheField</code> of type U32 vector.
 *
 * @param field (in) The field.
 * @param values (out) The array of values of the field.
 * @param size (out) The number of values.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_getU32Vector(const mamaFieldCacheField field,
                                 const mama_u32_t** values,
                                 mama_size_t* size);

/**
 * This function will get the values a <code>mamaFieldCacheField</code> of type I64 vector.
 *
 * @param field (in) The field.
 * @param values (out) The array of values of the field.
 * @param size (out) The number of values.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_getI64Vector(const mamaFieldCacheField field,
                                 const mama_i64_t** values,
                                 mama_size_t* size);

/**
 * This function will get the values a <code>mamaFieldCacheField</code> of type U64 vector.
 *
 * @param field (in) The field.
 * @param values (out) The array of values of the field.
 * @param size (out) The number of values.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_getU64Vector(const mamaFieldCacheField field,
                                 const mama_u64_t** values,
                                 mama_size_t* size);

/**
 * This function will get the value a <code>mamaFieldCacheField</code> of type F32 vector.
 *
 * @param field (in) The field.
 * @param values (out) The array of values of the field.
 * @param size (out) The number of values.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_getF32Vector(const mamaFieldCacheField field,
                                 const mama_f32_t** values,
                                 mama_size_t* size);

/**
 * This function will get the value a <code>mamaFieldCacheField</code> of type F64 vector.
 *
 * @param field (in) The field.
 * @param values (out) The array of values of the field.
 * @param size (out) The number of values.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_getF64Vector(const mamaFieldCacheField field,
                                 const mama_f64_t** values,
                                 mama_size_t* size);

/**
 * This function will get the value a <code>mamaFieldCacheField</code> of type STRING vector.
 *
 * @param field (in) The field.
 * @param values (out) The array of values of the field.
 * @param size (out) The number of values.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_getStringVector(const mamaFieldCacheField field,
                                    const char*** values,
                                    mama_size_t* size);

/**
 * This function will get the value a <code>mamaFieldCacheField</code> of type PRICE vector.
 *
 * @param field (in) The field.
 * @param values (out) The array of values of the field.
 * @param size (out) The number of values.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_getPriceVector(const mamaFieldCacheField field,
                                   const mamaPrice** values,
                                   mama_size_t* size);

/**
 * This function will get the value a <code>mamaFieldCacheField</code> of type TIME vector.
 *
 * @param field (in) The field.
 * @param values (out) The array of values of the field.
 * @param size (out) The number of values.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_getDateTimeVector(const mamaFieldCacheField field,
                                      const mamaDateTime** values,
                                      mama_size_t* size);

/**
 * This function will set the value of a <code>mamaFieldCacheField</code> of type BOOL.
 * 
 * @param field (in) The field to set the value to.
 * @param value (in) The new value of the field.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_setBool(mamaFieldCacheField field, mama_bool_t value);

/**
 * This function will set the value of a <code>mamaFieldCacheField</code> of type CHAR.
 * 
 * @param field (in) The field to set the value to.
 * @param value (in) The new value of the field.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_setChar(mamaFieldCacheField field, char value);

/**
 * This function will set the value of a <code>mamaFieldCacheField</code> of type I8.
 * 
 * @param field (in) The field to set the value to.
 * @param value (in) The new value of the field.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_setI8(mamaFieldCacheField field, mama_i8_t value);

/**
 * This function will set the value of a <code>mamaFieldCacheField</code> of type U8.
 * 
 * @param field (in) The field to set the value to.
 * @param value (in) The new value of the field.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_setU8(mamaFieldCacheField field, mama_u8_t value);

/**
 * This function will set the value of a <code>mamaFieldCacheField</code> of type I16.
 * 
 * @param field (in) The field to set the value to.
 * @param value (in) The new value of the field.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_setI16(mamaFieldCacheField field, mama_i16_t value);

/**
 * This function will set the value of a <code>mamaFieldCacheField</code> of type U16.
 * 
 * @param field (in) The field to set the value to.
 * @param value (in) The new value of the field.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_setU16(mamaFieldCacheField field, mama_u16_t value);

/**
 * This function will set the value of a <code>mamaFieldCacheField</code> of type I32.
 * 
 * @param field (in) The field to set the value to.
 * @param value (in) The new value of the field.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_setI32(mamaFieldCacheField field, mama_i32_t value);

/**
 * This function will set the value of a <code>mamaFieldCacheField</code> of type U32.
 * 
 * @param field (in) The field to set the value to.
 * @param value (in) The new value of the field.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_setU32(mamaFieldCacheField field, mama_u32_t value);

/**
 * This function will set the value of a <code>mamaFieldCacheField</code> of type I64.
 * 
 * @param field (in) The field to set the value to.
 * @param value (in) The new value of the field.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_setI64(mamaFieldCacheField field, mama_i64_t value);

/**
 * This function will set the value of a <code>mamaFieldCacheField</code> of type U64.
 * 
 * @param field (in) The field to set the value to.
 * @param value (in) The new value of the field.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_setU64(mamaFieldCacheField field, mama_u64_t value);

/**
 * This function will set the value of a <code>mamaFieldCacheField</code> of type F32.
 * 
 * @param field (in) The field to set the value to.
 * @param value (in) The new value of the field.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_setF32(mamaFieldCacheField field, mama_f32_t value);

/**
 * This function will set the value of a <code>mamaFieldCacheField</code> of type F64.
 * 
 * @param field (in) The field to set the value to.
 * @param value (in) The new value of the field.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_setF64(mamaFieldCacheField field, mama_f64_t value);

/**
 * This function will set the value of a <code>mamaFieldCacheField</code> of type STRING.
 * If parameter len is 0 then the length of the string is automatically computed
 * using <code>strlen</code>.
 * 
 * @param field (in) The field to set the value to.
 * @param value (in) The new value of the field.
 * @param len (in) The length of the string. Can be 0.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_setString(const mamaFieldCacheField field,
                              const char* value,
                              mama_size_t len);

/**
 * This function will set the value of a <code>mamaFieldCacheField</code> of type PRICE.
 * 
 * @param field (in) The field to set the value to.
 * @param value (in) The new value of the field.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_setPrice(const mamaFieldCacheField field, const mamaPrice value);

/**
 * This function will set the value of a <code>mamaFieldCacheField</code> of type TIME.
 * 
 * @param field (in) The field to set the value to.
 * @param value (in) The new value of the field.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_setDateTime(const mamaFieldCacheField field,
                                const mamaDateTime value);

/**
 * This function will set the values of a <code>mamaFieldCacheField</code> of type I8 vector.
 * 
 * @param field (in) The field to set the value to.
 * @param value (in) The new value of the field.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_setI8Vector(const mamaFieldCacheField field,
                                const mama_i8_t* values,
                                mama_size_t size);

/**
 * This function will set the values of a <code>mamaFieldCacheField</code> of type U8 vector.
 * 
 * @param field (in) The field to set the value to.
 * @param value (in) The new value of the field.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_setU8Vector(const mamaFieldCacheField field,
                                const mama_u8_t* values,
                                mama_size_t size);

/**
 * This function will set the values of a <code>mamaFieldCacheField</code> of type I16 vector.
 * 
 * @param field (in) The field to set the value to.
 * @param value (in) The new value of the field.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_setI16Vector(const mamaFieldCacheField field,
                                 const mama_i16_t* values,
                                 mama_size_t size);

/**
 * This function will set the values of a <code>mamaFieldCacheField</code> of type U16 vector.
 * 
 * @param field (in) The field to set the value to.
 * @param value (in) The new value of the field.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_setU16Vector(const mamaFieldCacheField field,
                                 const mama_u16_t* values,
                                 mama_size_t size);

/**
 * This function will set the values of a <code>mamaFieldCacheField</code> of type I32 vector.
 * 
 * @param field (in) The field to set the value to.
 * @param value (in) The new value of the field.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_setI32Vector(const mamaFieldCacheField field,
                                 const mama_i32_t* values,
                                 mama_size_t size);

/**
 * This function will set the values of a <code>mamaFieldCacheField</code> of type U32 vector.
 * 
 * @param field (in) The field to set the value to.
 * @param value (in) The new value of the field.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_setU32Vector(const mamaFieldCacheField field,
                                 const mama_u32_t* values,
                                 mama_size_t size);

/**
 * This function will set the values of a <code>mamaFieldCacheField</code> of type I64 vector.
 * 
 * @param field (in) The field to set the value to.
 * @param value (in) The new value of the field.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_setI64Vector(const mamaFieldCacheField field,
                                 const mama_i64_t* values,
                                 mama_size_t size);

/**
 * This function will set the values of a <code>mamaFieldCacheField</code> of type U64 vector.
 * 
 * @param field (in) The field to set the value to.
 * @param value (in) The new value of the field.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_setU64Vector(const mamaFieldCacheField field,
                                 const mama_u64_t* values,
                                 mama_size_t size);

/**
 * This function will set the values of a <code>mamaFieldCacheField</code> of type F32 vector.
 * 
 * @param field (in) The field to set the value to.
 * @param value (in) The new value of the field.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_setF32Vector(const mamaFieldCacheField field,
                                 const mama_f32_t* values,
                                 mama_size_t size);

/**
 * This function will set the values of a <code>mamaFieldCacheField</code> of type F64 vector.
 * 
 * @param field (in) The field to set the value to.
 * @param value (in) The new value of the field.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_setF64Vector(const mamaFieldCacheField field,
                                 const mama_f64_t* values,
                                 mama_size_t size);

/**
 * This function will set the values of a <code>mamaFieldCacheField</code> of type STRING vector.
 * 
 * @param field (in) The field to set the value to.
 * @param value (in) The new value of the field.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_setStringVector(const mamaFieldCacheField field,
                                    const char** values,
                                    mama_size_t size);

/**
 * This function will set the values of a <code>mamaFieldCacheField</code> of type PRICE vector.
 * 
 * @param field (in) The field to set the value to.
 * @param value (in) The new value of the field.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_setPriceVector(const mamaFieldCacheField field,
                                   const mamaPrice* values,
                                   mama_size_t size);

/**
 * This function will set the values of a <code>mamaFieldCacheField</code> of type TIME vector.
 * 
 * @param field (in) The field to set the value to.
 * @param value (in) The new value of the field.
 * @return Resulting status of the call which can be
 *      MAMA_STATUS_NULL_ARG
 *      MAMA_STATUS_INVALID_ARG
 *      MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFieldCacheField_setDateTimeVector(const mamaFieldCacheField field,
                                      const mamaDateTime* values,
                                      mama_size_t size);

#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

#endif /* MamaFieldCacheFieldH__ */
