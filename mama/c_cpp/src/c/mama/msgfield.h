/* $Id: msgfield.h,v 1.20.32.4 2011/09/01 16:34:38 emmapollock Exp $
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

#ifndef MamaMsgFieldH__
#define MamaMsgFieldH__

#include <mama/types.h>
#include <mama/status.h>
#include <mama/fielddesc.h>
#include <stdlib.h>

#include "wombat/port.h"

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * Create a field.
 *
 * @param msgField A pointer to the result.
 */
MAMAExpDLL
extern mama_status
mamaMsgField_create (mamaMsgField*  msgField);

/**
 * Destroy a message and free any resources associated with it.
 *
 * @param msgField The field.
 */
MAMAExpDLL
extern mama_status
mamaMsgField_destroy (mamaMsgField msgField);

/**
 * Get the field descriptor for this field.
 *
 * @param msgField The field.
 * @param result (out) Pointer to the result
 */
MAMAExpDLL
extern mama_status
mamaMsgField_getDescriptor(
    const mamaMsgField    msgField,
    mamaFieldDescriptor*  result);

/**
 * Get the field ID for this field.
 *
 * @param msgField The field.
 * @param result (out) Pointer to the result
 */
MAMAExpDLL
extern mama_status
mamaMsgField_getFid(
    const mamaMsgField  msgField,
    mama_fid_t*         result);

/**
 * Get the field name for this field.
 *
 * @param msgField The field.
 * @param result (out) Pointer to the result.
 */
MAMAExpDLL
extern mama_status
mamaMsgField_getName(
    const mamaMsgField  msgField,
    const char**        result);

/**
 * Get the field type for this field.
 *
 * @param msgField The field.
 * @param result (out) Pointer to the result
 */
MAMAExpDLL
extern mama_status
mamaMsgField_getType(
    const mamaMsgField  msgField,
    mamaFieldType*      result);

/**
 * Get the type name for this field. This is a human readable
 * representation of the type.
 *
 * @param msgField The field.
 * @param result (out) Pointer to the result.
 */
MAMAExpDLL
extern mama_status
mamaMsgField_getTypeName(
    const mamaMsgField  msgField,
    const char**        result);


/**
 * Get a boolean field.
 *
 * @param msgField The field.
 * @param result (out) Pointer to the result
 */
MAMAExpDLL
extern mama_status
mamaMsgField_getBool(
    const mamaMsgField  msgField,
    mama_bool_t*        result);

/**
 * Get a character field.
 *
 * @param msgField The field.
 * @param result (out) Pointer to the result
 */
MAMAExpDLL
extern mama_status
mamaMsgField_getChar(
    const mamaMsgField  msgField,
    char*               result);

/**
 * Get a I8, signed 8 bit integer, field.
 *
 * @param msgField The field.
 * @param result (out) Pointer to the result
 */
MAMAExpDLL
extern mama_status
mamaMsgField_getI8(
    const mamaMsgField  msgField,
    mama_i8_t*          result);

/**
 * Get a U8, unsigned 8 bit integer, field.
 *
 * @param msgField The field.
 * @param result (out) Pointer to the result
 */
MAMAExpDLL
extern mama_status
mamaMsgField_getU8(
    const mamaMsgField  msgField,
    mama_u8_t*          result);

/**
 * Get a I16, signed 16 bit integer, field.
 *
 * @param msgField The field.
 * @param result (out) Pointer to the result
 */
MAMAExpDLL
extern mama_status
mamaMsgField_getI16(
    const mamaMsgField  msgField,
    mama_i16_t*         result);

/**
 * Get a U16, unsigned 16 bit integer, field.
 *
 * @param msgField The field.
 * @param result (out) Pointer to the result
 */
MAMAExpDLL
extern mama_status
mamaMsgField_getU16(
    const mamaMsgField  msgField,
    mama_u16_t*         result);

/**
 * Get a I32, signed 32 bit integer, field.
 *
 * @param msgField The field.
 * @param result (out) Pointer to the result
 */
MAMAExpDLL
extern mama_status
mamaMsgField_getI32(
    const mamaMsgField  msgField,
    mama_i32_t*         result);

/**
 * Get a U32, unsigned 32 bit integer, field.
 *
 * @param msgField The field.
 * @param result (out) Pointer to the result
 */
MAMAExpDLL
extern mama_status
mamaMsgField_getU32(
    const mamaMsgField  msgField,
    mama_u32_t*         result);

/**
 * Get a I64, signed 64 bit integer, field.
 *
 * @param msgField The field.
 * @param result (out) Pointer to the result
 */
MAMAExpDLL
extern mama_status
mamaMsgField_getI64(
    const mamaMsgField  msgField,
    mama_i64_t*         result);

/**
 * Get a U64, unsigned 64 bit integer, field.
 *
 * @param msgField The field.
 * @param result (out) Pointer to the result
 */
MAMAExpDLL
extern mama_status
mamaMsgField_getU64(
    const mamaMsgField  msgField,
    mama_u64_t*         result);

/**
 * Get a F32, floating point 32 bit integer, field.
 *
 * @param msgField The field.
 * @param result (out) Pointer to the result
 */
MAMAExpDLL
extern mama_status
mamaMsgField_getF32(
    const mamaMsgField  msgField,
    mama_f32_t*         result);

/**
 * Get a F64, floating point 64 bit integer, field.
 *
 * @param msgField The field.
 * @param result (out) Pointer to the result
 */
MAMAExpDLL
extern mama_status
mamaMsgField_getF64(
    const mamaMsgField  msgField,
    mama_f64_t*         result);

/**
 * Get a const char * field.
 *
 * @param msgField The field.
 * @param result (out)  the string value.
 */
MAMAExpDLL
extern mama_status
mamaMsgField_getString(
    const mamaMsgField  msgField,
    const char**        result);

/**
 * Get an opaque field.
 *
 * @param msgField The field.
 * @param result (out)  The opaque value.
 * @param size  The size of the resulting opaque value.
 */
MAMAExpDLL
extern mama_status
mamaMsgField_getOpaque(
    const mamaMsgField  msgField,
    const void**        result,
    mama_size_t*        size);

/**
 * Get a MAMA date/time field.
 *
 * @param msgField The field.
 * @param result (out)  the output value.
 */
MAMAExpDLL
extern mama_status
mamaMsgField_getDateTime(
    const mamaMsgField  msgField,
    mamaDateTime        result);

/**
 * Get a MAMA price field.
 *
 * @param msgField The field.
 * @param result (out)  the output value.
 */
MAMAExpDLL
extern mama_status
mamaMsgField_getPrice(
    const mamaMsgField  msgField,
    mamaPrice           result);

/**
 * Get a MAMA submessage field.
 *
 * @param msgField The field.
 * @param result (out) the mamaMsg output value.
 */
MAMAExpDLL
extern mama_status
mamaMsgField_getMsg (
    const mamaMsgField msgField,
    mamaMsg*           result);

/**
 * Get a vector of booleans.
 *
 * @param msgField The message field.
 * @param result A pointer to the result.
 * @param size An integer pointer that will contain the length of
 * the
 * result when the method returns..
 */
MAMAExpDLL
extern mama_status
mamaMsgField_getVectorBool (
    const mamaMsgField   msgField,
    const mama_bool_t**  result,
    mama_size_t*         size);

/**
 * Get a vector of characters.
 *
 * @param msgField The message field.
 * @param result A pointer to the result.
 * @param size An integer pointer that will contain the length of
 * the
 * result when the method returns..
 */
MAMAExpDLL
extern mama_status
mamaMsgField_getVectorChar (
    const mamaMsgField   msgField,
    const char**         result,
    mama_size_t*         size);

/**
 * Get a vector of signed 8 bit integers.
 *
 * @param msgField The message field.
 * @param result A pointer to the result.
 * @param size An integer pointer that will contain the length of
 * the
 * result when the method returns..
 */
MAMAExpDLL
extern mama_status
mamaMsgField_getVectorI8 (
    const mamaMsgField   msgField,
    const mama_i8_t**    result,
    mama_size_t*         size);

/**
 * Get a vector of unsigned 8 bit integers.
 *
 * @param msgField The message field.
 * @param result A pointer to the result.
 * @param size An integer pointer that will contain the length of
 * the
 * result when the method returns..
 */
MAMAExpDLL
extern mama_status
mamaMsgField_getVectorU8 (
    const mamaMsgField   msgField,
    const mama_u8_t**    result,
    mama_size_t*         size);

/**
 * Get a vector of signed 16 bit integers.
 *
 * @param msgField The message field.
 * @param result A pointer to the result.
 * @param size An integer pointer that will contain the length of
 * the
 * result when the method returns..
 */
MAMAExpDLL
extern mama_status
mamaMsgField_getVectorI16 (
    const mamaMsgField   msgField,
    const mama_i16_t**   result,
    mama_size_t*         size);

/**
 * Get a vector of unsigned 16 bit integers.
 *
 * @param msgField The message field.
 * @param result A pointer to the result.
 * @param size An integer pointer that will contain the length of
 * the
 * result when the method returns..
 */
MAMAExpDLL
extern mama_status
mamaMsgField_getVectorU16 (
    const mamaMsgField   msgField,
    const mama_u16_t**   result,
    mama_size_t*         size);

/**
 * Get a vector of signed 32 bit integers.
 *
 * @param msgField The message field.
 * @param result A pointer to the result.
 * @param size An integer pointer that will contain the length of
 * the
 * result when the method returns..
 */
MAMAExpDLL
extern mama_status
mamaMsgField_getVectorI32 (
    const mamaMsgField   msgField,
    const mama_i32_t**   result,
    mama_size_t*         size);

/**
 * Get a vector of unsigned 32 bit integers.
 *
 * @param msgField The message field.
 * @param result A pointer to the result.
 * @param size An integer pointer that will contain the length of
 * the
 * result when the method returns..
 */
MAMAExpDLL
extern mama_status
mamaMsgField_getVectorU32 (
    const mamaMsgField   msgField,
    const mama_u32_t**   result,
    mama_size_t*         size);

/**
 * Get a vector of signed 64 bit integers.
 *
 * @param msgField The message field.
 * @param result A pointer to the result.
 * @param size An integer pointer that will contain the length of
 * the
 * result when the method returns..
 */
MAMAExpDLL
extern mama_status
mamaMsgField_getVectorI64 (
    const mamaMsgField   msgField,
    const mama_i64_t**   result,
    mama_size_t*         size);

/**
 * Get a vector of unsigned 64 bit integers.
 *
 * @param msgField The message field.
 * @param result A pointer to the result.
 * @param size An integer pointer that will contain the length of
 * the
 * result when the method returns..
 */
MAMAExpDLL
extern mama_status
mamaMsgField_getVectorU64 (
    const mamaMsgField   msgField,
    const mama_u64_t**   result,
    mama_size_t*         size);

/**
 * Get a vector of 32 bit floating point numbers.
 *
 * @param msgField The message field.
 * @param result A pointer to the result.
 * @param size An integer pointer that will contain the length of
 * the
 * result when the method returns..
 */
MAMAExpDLL
extern mama_status
mamaMsgField_getVectorF32 (
    const mamaMsgField   msgField,
    const mama_f32_t**   result,
    mama_size_t*         size);

/**
 * Get a vector of 64 bit floating point numbers.
 *
 * @param msgField The message field.
 * @param result A pointer to the result.
 * @param size An integer pointer that will contain the length of
 * the
 * result when the method returns..
 */
MAMAExpDLL
extern mama_status
mamaMsgField_getVectorF64 (
    const mamaMsgField   msgField,
    const mama_f64_t**   result,
    mama_size_t*         size);

/**
 * Get a vector of strings (char*).
 *
 * @param msgField The message field.
 * @param result A pointer to the result.
 * @param size An integer pointer that will contain the length of
 * the
 * result when the method returns..
 */
MAMAExpDLL
extern mama_status
mamaMsgField_getVectorString (
    const mamaMsgField   msgField,
    const char***        result,
    mama_size_t*         size);

/**
 * Get a vector of submessages.
 *
 * @param msgField The field.
 * @param result A pointer to the result.
 * @param size A pointer for the length of the result.
 */
MAMAExpDLL
extern mama_status
mamaMsgField_getVectorMsg (
    const mamaMsgField   msgField,
    const mamaMsg**      result,
    mama_size_t*         size);



/**
 * Convert the value of the specified field to a string. Caller must
 * provide a buffer for the value.
 *
 * @param field The field.
 * @param buf Buffer where the result will be copied.
 * @param length The length of the caller supplied buffer.
 */
MAMAExpDLL
extern mama_status
mamaMsgField_getAsString (
    const mamaMsgField  field,
    char*               buf,
    mama_size_t         length);

/**
 * Update the specified field with a new bool value.
 * Returns MAMA_WRONG_FIELD_TYPE if the existing field is not of type bool.
 *
 * @param field The mamaMsgField to be updated.
 * @param value The new value for the field.
 *
 * @return MAMA_STATUS_OK The function returned successfully.
 * @return MAMA_STATUS_WRONG_FIELD_TYPE The existing field type does not match the
 * type of the update function called.
 * @return MAMA_STATUS_NULL_ARG The field passed to the function is NULL.
 * @return MAMA_STATUS_INVALID_ARG The underlying bridge field is NULL.
 */
MAMAExpDLL
extern mama_status
mamaMsgField_updateBool (
    mamaMsgField  field,
    mama_bool_t   value);

/**
 * Update the specified field with a new char value.
 * Returns MAMA_WRONG_FIELD_TYPE if the existing field is not of type char.
 *
 * @param field The mamaMsgField to be updated.
 * @param value The new value for the field.
 *
 * @return MAMA_STATUS_OK The function returned successfully.
 * @return MAMA_STATUS_WRONG_FIELD_TYPE The existing field type does not match the
 * type of the update function called.
 * @return MAMA_STATUS_NULL_ARG The field passed to the function is NULL.
 * @return MAMA_STATUS_INVALID_ARG The underlying bridge field is NULL.
 */
MAMAExpDLL
extern mama_status
mamaMsgField_updateChar (
    mamaMsgField  field,
    char          value);

/**
 * Update the specified field with a new I8 value.
 * Returns MAMA_WRONG_FIELD_TYPE if the existing field is not of type I8.
 *
 * @param field The mamaMsgField to be updated.
 * @param value The new value for the field.
 *
 * @return MAMA_STATUS_OK The function returned successfully.
 * @return MAMA_STATUS_WRONG_FIELD_TYPE The existing field type does not match the
 * type of the update function called.
 * @return MAMA_STATUS_NULL_ARG The field passed to the function is NULL.
 * @return MAMA_STATUS_INVALID_ARG The underlying bridge field is NULL.
 */
MAMAExpDLL
extern mama_status
mamaMsgField_updateI8 (
    mamaMsgField  field,
    mama_i8_t     value);

/**
 * Update the specified field with a new U8 value.
 * Returns MAMA_WRONG_FIELD_TYPE if the existing field is not of type U8.
 *
 * @param field The mamaMsgField to be updated.
 * @param value The new value for the field.
 *
 * @return MAMA_STATUS_OK The function returned successfully.
 * @return MAMA_STATUS_WRONG_FIELD_TYPE The existing field type does not match the
 * type of the update function called.
 * @return MAMA_STATUS_NULL_ARG The field passed to the function is NULL.
 * @return MAMA_STATUS_INVALID_ARG The underlying bridge field is NULL.
 */
MAMAExpDLL
extern mama_status
mamaMsgField_updateU8 (
    mamaMsgField  field,
    mama_u8_t     value);

/**
 * Update the specified field with a new I16 value.
 * Returns MAMA_WRONG_FIELD_TYPE if the existing field is not of type I16.
 *
 * @param field The mamaMsgField to be updated.
 * @param value The new value for the field.
 *
 * @return MAMA_STATUS_OK The function returned successfully.
 * @return MAMA_STATUS_WRONG_FIELD_TYPE The existing field type does not match the
 * type of the update function called.
 * @return MAMA_STATUS_NULL_ARG The field passed to the function is NULL.
 * @return MAMA_STATUS_INVALID_ARG The underlying bridge field is NULL.
 */
MAMAExpDLL
extern mama_status
mamaMsgField_updateI16 (
    mamaMsgField  field,
    mama_i16_t    value);

/**
 * Update the specified field with a new U16 value.
 * Returns MAMA_WRONG_FIELD_TYPE if the existing field is not of type U16.
 *
 * @param field The mamaMsgField to be updated.
 * @param value The new value for the field.
 *
 * @return MAMA_STATUS_OK The function returned successfully.
 * @return MAMA_STATUS_WRONG_FIELD_TYPE The existing field type does not match the
 * type of the update function called.
 * @return MAMA_STATUS_NULL_ARG The field passed to the function is NULL.
 * @return MAMA_STATUS_INVALID_ARG The underlying bridge field is NULL.
 */
MAMAExpDLL
extern mama_status
mamaMsgField_updateU16 (
    mamaMsgField  field,
    mama_u16_t    value);

/**
 * Update the specified field with a new I32 value.
 * Returns MAMA_WRONG_FIELD_TYPE if the existing field is not of type I32.
 *
 * @param field The mamaMsgField to be updated.
 * @param value The new value for the field.
 *
 * @return MAMA_STATUS_OK The function returned successfully.
 * @return MAMA_STATUS_WRONG_FIELD_TYPE The existing field type does not match the
 * type of the update function called.
 * @return MAMA_STATUS_NULL_ARG The field passed to the function is NULL.
 * @return MAMA_STATUS_INVALID_ARG The underlying bridge field is NULL.
 */
MAMAExpDLL
extern mama_status
mamaMsgField_updateI32 (
    mamaMsgField  field,
    mama_i32_t    value);

/**
 * Update the specified field with a new U32 value.
 * Returns MAMA_WRONG_FIELD_TYPE if the existing field is not of type U32.
 *
 * @param field The mamaMsgField to be updated.
 * @param value The new value for the field.
 *
 * @return MAMA_STATUS_OK The function returned successfully.
 * @return MAMA_STATUS_WRONG_FIELD_TYPE The existing field type does not match the
 * type of the update function called.
 * @return MAMA_STATUS_NULL_ARG The field passed to the function is NULL.
 * @return MAMA_STATUS_INVALID_ARG The underlying bridge field is NULL.
 */
MAMAExpDLL
extern mama_status
mamaMsgField_updateU32 (
    mamaMsgField  field,
    mama_u32_t    value);

/**
 * Update the specified field with a new I64 value.
 * Returns MAMA_WRONG_FIELD_TYPE if the existing field is not of type I64.
 *
 * @param field The mamaMsgField to be updated.
 * @param value The new value for the field.
 *
 * @return MAMA_STATUS_OK The function returned successfully.
 * @return MAMA_STATUS_WRONG_FIELD_TYPE The existing field type does not match the
 * type of the update function called.
 * @return MAMA_STATUS_NULL_ARG The field passed to the function is NULL.
 * @return MAMA_STATUS_INVALID_ARG The underlying bridge field is NULL.
 */
MAMAExpDLL
extern mama_status
mamaMsgField_updateI64 (
    mamaMsgField  field,
    mama_i64_t    value);

/**
 * Update the specified field with a new U64 value.
 * Returns MAMA_WRONG_FIELD_TYPE if the existing field is not of type U64.
 *
 * @param field The mamaMsgField to be updated.
 * @param value The new value for the field.
 *
 * @return MAMA_STATUS_OK The function returned successfully.
 * @return MAMA_STATUS_WRONG_FIELD_TYPE The existing field type does not match the
 * type of the update function called.
 * @return MAMA_STATUS_NULL_ARG The field passed to the function is NULL.
 * @return MAMA_STATUS_INVALID_ARG The underlying bridge field is NULL.
 */
MAMAExpDLL
extern mama_status
mamaMsgField_updateU64 (
    mamaMsgField  field,
    mama_u64_t    value);

/**
 * Update the specified field with a new F32 value.
 * Returns MAMA_WRONG_FIELD_TYPE if the existing field is not of type F32.
 *
 * @param field The mamaMsgField to be updated.
 * @param value The new value for the field.
 *
 * @return MAMA_STATUS_OK The function returned successfully.
 * @return MAMA_STATUS_WRONG_FIELD_TYPE The existing field type does not match the
 * type of the update function called.
 * @return MAMA_STATUS_NULL_ARG The field passed to the function is NULL.
 * @return MAMA_STATUS_INVALID_ARG The underlying bridge field is NULL.
 */
MAMAExpDLL
extern mama_status
mamaMsgField_updateF32 (
    mamaMsgField  field,
    mama_f32_t    value);


/**
 * Update the specified field with a new F64 value.
 * Returns MAMA_WRONG_FIELD_TYPE if the existing field is not of type F32.
 *
 * @param field The mamaMsgField to be updated.
 * @param value The new value for the field.
 *
 * @return MAMA_STATUS_OK The function returned successfully.
 * @return MAMA_STATUS_WRONG_FIELD_TYPE The existing field type does not match the
 * type of the update function called.
 * @return MAMA_STATUS_NULL_ARG The field passed to the function is NULL.
 * @return MAMA_STATUS_INVALID_ARG The underlying bridge field is NULL.
 */
MAMAExpDLL
extern mama_status
mamaMsgField_updateF64 (
    mamaMsgField  field,
    mama_f64_t    value);

/**
 * Update the specified field with a new MamaDateTime value.
 * Returns MAMA_WRONG_FIELD_TYPE if the existing field is not of type F32.
 *
 * @param field The mamaMsgField to be updated.
 * @param value The new value for the field.
 *
 * @return MAMA_STATUS_OK The function returned successfully.
 * @return MAMA_STATUS_WRONG_FIELD_TYPE The existing field type does not match the
 * type of the update function called.
 * @return MAMA_STATUS_NULL_ARG The field passed to the function is NULL.
 * @return MAMA_STATUS_INVALID_ARG The underlying bridge field is NULL.
 */
MAMAExpDLL
extern mama_status
mamaMsgField_updateDateTime (
    mamaMsgField        field,
    const mamaDateTime  value);

/**
 * Update the specified field with a new MamaDateTime value.
 * Returns MAMA_WRONG_FIELD_TYPE if the existing field is not of type F32.
 *
 * @param field The mamaMsgField to be updated.
 * @param value The new value for the field.
 *
 * @return MAMA_STATUS_OK The function returned successfully.
 * @return MAMA_STATUS_WRONG_FIELD_TYPE The existing field type does not match the
 * type of the update function called.
 * @return MAMA_STATUS_NULL_ARG The field passed to the function is NULL.
 * @return MAMA_STATUS_INVALID_ARG The underlying bridge field is NULL.
 */
MAMAExpDLL
extern mama_status
mamaMsgField_updatePrice (
    mamaMsgField    field,
    const mamaPrice value);

#if defined(__cplusplus)
}
#endif

#endif /* MamaMsgFieldH__ */
