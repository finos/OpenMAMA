/* $Id: msg.h,v 1.75.4.2.14.11 2011/10/02 19:02:18 ianbell Exp $
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

#ifndef MamaMsgH__
#define MamaMsgH__

#include <mama/types.h>
#include <mama/status.h>
#include <mama/fielddesc.h>
#include <mama/msgtype.h>
#include "wombat/port.h"
#include <stdlib.h>

#define ENTITLE_FIELD_NAME  "wEntitleCode"
#define ENTITLE_FIELD_ID    496

#if defined(__cplusplus)
extern "C" {
#endif

struct wombatMsg_;

/**
 Enum to reference the available MAMA message payloads. */
typedef enum mamaPayloadType_
{
    MAMA_PAYLOAD_WOMBAT_MSG = 'W',
    MAMA_PAYLOAD_TIBRV      = 'R',
    MAMA_PAYLOAD_FAST       = 'F',
    MAMA_PAYLOAD_V5         = '5',
    MAMA_PAYLOAD_AVIS       = 'A',
    MAMA_PAYLOAD_UNKNOWN    = 'U'
} mamaPayloadType;

/**
 * Convert a mamaPayloadType value to a string.  Do no attempt to free the
 * string result.
 *
 * @param payloadType  The payloadType to convert.*/
MAMAExpDLL
extern const char*
mamaPayload_convertToString (mamaPayloadType payloadType);

/**
 * Create a mamaMsg.
 *
 * @param msg The location of a mamaMsg where to store the result.
 */
MAMAExpDLL
extern mama_status
mamaMsg_create (mamaMsg* msg);

/**
 * Create a mamaMsg.
 *
 * @param msg The location of a mamaMsg where to store the result.
 * @param id The identifier of the payload to be used.
 */
MAMAExpDLL
extern mama_status
mamaMsg_createForPayload (mamaMsg* msg, const char id);

/**
 * Create a mamaMsg.
 *
 * @param msg The location of a mamaMsg where to store the result.
 * @param id The payload bridge to be used.
 */
MAMAExpDLL
extern mama_status
mamaMsg_createForPayloadBridge (mamaMsg* msg, mamaPayloadBridge payloadBridge);

/**
 * Create a mamaMsg for a particular template ID
 *
 * @param msg The location of a mamaMsg where to store the result.
 * @param templateID The wombatMsg to encapsulate.
 */
MAMAExpDLL
extern mama_status
mamaMsg_createForTemplate (mamaMsg* msg, mama_u32_t templateId);

/**
 * Copy a mamaMsg. If mamaMsg_create() has not been called for
 * for the destination message object then this will be done
 * by the function.
 *
 * @param src The message to copy.
 * @param copy A pointer to the destination message.
 *
 */
MAMAExpDLL
extern mama_status
mamaMsg_copy (mamaMsg src, mamaMsg *copy);

/**
 * Clear a msg. All fields are removed.
 *
 * @param msg The mamaMsg to clear.
 */
MAMAExpDLL
extern mama_status
mamaMsg_clear (mamaMsg msg);

/**
 * TIB/RV only. Get the TIB/RV send subject from the message. For other
 * message types MAMA_STATUS_NOT_IMPLEMENTED will be returned.
 *
 * @param src The message to copy.
 * @param copy A pointer to the destination message.
 *
 */
MAMAExpDLL
mama_status
mamaMsg_getSendSubject (const mamaMsg msg, const char** subject);

/**
 * Destroy a message and free any resources associated with it.
 *
 * @param msg The message.
 */
MAMAExpDLL
extern mama_status
mamaMsg_destroy (mamaMsg msg);

/**
 *  Get the type of the message payload.
 *
 * @param msg The message.
 * @param payloadType The payload type.
 */
MAMAExpDLL
extern mama_status
mamaMsg_getPayloadType (mamaMsg msg, mamaPayloadType* payloadType);

/**
* Get the size of the underlying message in bytes.
* Only works for tibrvMsg and wombatmsg types.
*
* @param msg The MAMA message
* @param size (out) The size of the message in bytes
* @return Resulting status of the call
*/
MAMAExpDLL
extern mama_status
mamaMsg_getByteSize (
        const mamaMsg  msg,
        mama_size_t*   size);

/**
 * Add a new bool field.
 *
 * @param msg The message.
 * @param name The name.
 * @param fid  The field identifier.
 * @param value The value.
 */
MAMAExpDLL
extern mama_status
mamaMsg_addBool(
    mamaMsg         msg,
    const char*     name,
    mama_fid_t      fid,
    mama_bool_t     value);

/**
 * Add a new char field.
 *
 * @param msg The message.
 * @param name The name.
 * @param fid  The field identifier.
 * @param value The value.
 */
MAMAExpDLL
extern mama_status
mamaMsg_addChar(
    mamaMsg         msg,
    const char*     name,
    mama_fid_t      fid,
    char            value);

/**
 * Add a new 8 bit signed int field.
 *
 * @param msg The message.
 * @param name The name.
 * @param fid  The field identifier.
 * @param value The value.
 */
MAMAExpDLL
extern mama_status
mamaMsg_addI8(
    mamaMsg         msg,
    const char*     name,
    mama_fid_t      fid,
    mama_i8_t       value);

/**
 * Add a new 8 bit unsigned int field.
 *
 * @param msg The message.
 * @param name The name.
 * @param fid  The field identifier.
 * @param value The value.
 */
MAMAExpDLL
extern mama_status
mamaMsg_addU8(
    mamaMsg         msg,
    const char*     name,
    mama_fid_t      fid,
    mama_u8_t       value);

/**
 * Add a new 16 bit signed int field.
 *
 * @param msg The message.
 * @param name The name.
 * @param fid  The field identifier.
 * @param value The value.
 */
MAMAExpDLL
extern mama_status
mamaMsg_addI16(
    mamaMsg         msg,
    const char*     name,
    mama_fid_t      fid,
    mama_i16_t      value);

/**
 * Add a new 16 bit unsigned int field.
 *
 * @param msg The message.
 * @param name The name.
 * @param fid  The field identifier.
 * @param value The value.
 */
MAMAExpDLL
extern mama_status
mamaMsg_addU16(
    mamaMsg         msg,
    const char*     name,
    mama_fid_t      fid,
    mama_u16_t      value);

/**
 * Add a new 32 bit unsigned int field.
 *
 * @param msg The message.
 * @param name The name.
 * @param fid  The field identifier.
 * @param value The value.
 */
MAMAExpDLL
extern mama_status
mamaMsg_addI32(
    mamaMsg      msg,
    const char*  name,
    mama_fid_t   fid,
    mama_i32_t   value);


/**
 * Add a new 32 bit signed int field.
 *
 * @param msg The message.
 * @param name The name.
 * @param fid  The field identifier.
 * @param value The value.
 */
MAMAExpDLL
extern mama_status
mamaMsg_addU32(
    mamaMsg      msg,
    const char*  name,
    mama_fid_t   fid,
    mama_u32_t   value);

/**
 * Add a new I64 field.
 *
 * @param msg The message.
 * @param name The name.
 * @param fid  The field identifier.
 * @param value The value.
 */
MAMAExpDLL
extern mama_status
mamaMsg_addI64(
    mamaMsg      msg,
    const char*  name,
    mama_fid_t   fid,
    mama_i64_t   value);

/**
 * Add a new U64 field.
 *
 * @param msg The message.
 * @param name The name.
 * @param fid  The field identifier.
 * @param value The value.
 */
MAMAExpDLL
extern mama_status
mamaMsg_addU64(
    mamaMsg      msg,
    const char*  name,
    mama_fid_t   fid,
    mama_u64_t   value);

/**
 * Add a new float (f32) field.
 *
 * @param msg The message.
 * @param name The name.
 * @param fid  The field identifier.
 * @param value The value.
 */
MAMAExpDLL
extern mama_status
mamaMsg_addF32(
    mamaMsg      msg,
    const char*  name,
    mama_fid_t   fid,
    mama_f32_t   value);

/**
 * Add a new f64 field.
 *
 * @param msg The message.
 * @param name The name.
 * @param fid  The field identifier.
 * @param value The value.
 */
MAMAExpDLL
extern mama_status
mamaMsg_addF64(
    mamaMsg      msg,
    const char*  name,
    mama_fid_t   fid,
    mama_f64_t   value);

/**
 * Add a const char * field.
 *
 * @param msg The message.
 * @param name The name.
 * @param fid  The field identifier.
 * @param value The value.
 */
MAMAExpDLL
extern mama_status
mamaMsg_addString(
    mamaMsg      msg,
    const char*  name,
    mama_fid_t   fid,
    const char*  value);

/**
 * Add an opaque field.
 *
 * @param msg The message.
 * @param name The name.
 * @param fid  The field identifier.
 * @param value The value.
 * @param size The size of the opaque in bytes
 */
MAMAExpDLL
extern mama_status
mamaMsg_addOpaque (
    mamaMsg      msg,
    const char*  name,
    mama_fid_t   fid,
    const void*  value,
    mama_size_t  size);

/**
 * Add a MAMA date/time field.
 *
 * @param msg The message.
 * @param name The name.
 * @param fid  The field identifier.
 * @param value The value.
 */
MAMAExpDLL
extern mama_status
mamaMsg_addDateTime (
    mamaMsg             msg,
    const char*         name,
    mama_fid_t          fid,
    const mamaDateTime  value);

/**
 * Add a MAMA price field.
 *
 * @param msg The message.
 * @param name The name.
 * @param fid  The field identifier.
 * @param value The value.
 */
MAMAExpDLL
extern mama_status
mamaMsg_addPrice (
    mamaMsg             msg,
    const char*         name,
    mama_fid_t          fid,
    const mamaPrice     value);

/**
 * Add a mamaMsg object to the message.
 *
 * @param msg The message to which the field is being added.
 * @param name The name identifier for the field (optional).
 * @param fid The field identifier
 * @param value The value of the field
 *
 * @return mama_status MAMA_STATUS_OK if the function executes successfully
 */
MAMAExpDLL
extern mama_status
mamaMsg_addMsg(
    mamaMsg       msg,
    const char*   name,
    mama_fid_t    fid,
    const mamaMsg value);

/**
 * Add an array of booleans to the message.
 *
 * @param msg The mamaMsg
 * @param name The name identifier for the field (optional).
 * @param fid The field identifier.
 * @param value[] Array of booleans.
 * @param numElements The number of elements in the array.
 */
MAMAExpDLL
extern mama_status
mamaMsg_addVectorBool (
    mamaMsg            msg,
    const char*        name,
    mama_fid_t         fid,
    const mama_bool_t  value[],
    mama_size_t        numElements);

/**
 * Add an array of characters to the message.
 *
 * @param msg The mamaMsg
 * @param name The name identifier for the field (optional).
 * @param fid The field identifier.
 * @param value[] Array of characters.
 * @param numElements The number of elements in the array.
 */
MAMAExpDLL
extern mama_status
mamaMsg_addVectorChar (
    mamaMsg            msg,
    const char*        name,
    mama_fid_t         fid,
    const char         value[],
    mama_size_t        numElements);

/**
 * Add an array of signed 8 bit integers to the message.
 *
 * @param msg The mamaMsg
 * @param name The name identifier for the field (optional).
 * @param fid The field identifier.
 * @param value[] Array of signed 8 bit integers.
 * @param numElements The number of elements in the array.
 */
MAMAExpDLL
extern mama_status
mamaMsg_addVectorI8 (
    mamaMsg            msg,
    const char*        name,
    mama_fid_t         fid,
    const mama_i8_t    value[],
    mama_size_t        numElements);

/**
 * Add an array of unsigned 8 bit integers to the message.
 *
 * @param msg The mamaMsg
 * @param name The name identifier for the field (optional).
 * @param fid The field identifier.
 * @param value[] Array of unsigned 8 bit integers.
 * @param numElements The number of elements in the array.
 */
MAMAExpDLL
extern mama_status
mamaMsg_addVectorU8 (
    mamaMsg          msg,
    const char*      name,
    mama_fid_t       fid,
    const mama_u8_t  value[],
    mama_size_t      numElements);

/**
 * Add an array of signed 16 bit integers to the message.
 *
 * @param msg The mamaMsg
 * @param name The name identifier for the field (optional).
 * @param fid The field identifier.
 * @param value[] Array of signed 16 bit integers.
 * @param numElements The number of elements in the array.
 */
MAMAExpDLL
extern mama_status
mamaMsg_addVectorI16 (
    mamaMsg            msg,
    const char*        name,
    mama_fid_t         fid,
    const mama_i16_t   value[],
    mama_size_t        numElements);

/**
 * Add an array of unsigned 16 bit integers to the message.
 *
 * @param msg The mamaMsg
 * @param name The name identifier for the field (optional).
 * @param fid The field identifier.
 * @param value[] Array of unsigned 16 bit integers.
 * @param numElements The number of elements in the array.
 */
MAMAExpDLL
extern mama_status
mamaMsg_addVectorU16 (
    mamaMsg            msg,
    const char*        name,
    mama_fid_t         fid,
    const mama_u16_t   value[],
    mama_size_t        numElements);

/**
 * Add an array of signed 32 bit integers to the message.
 *
 * @param msg The mamaMsg
 * @param name The name identifier for the field (optional).
 * @param fid The field identifier.
 * @param value[] Array of signed 32 bit integers.
 * @param numElements The number of elements in the array.
 */
MAMAExpDLL
extern mama_status
mamaMsg_addVectorI32 (
    mamaMsg            msg,
    const char*        name,
    mama_fid_t         fid,
    const mama_i32_t   value[],
    mama_size_t        numElements);

/**
 * Add an array of unsigned 32 bit integers to the message.
 *
 * @param msg The mamaMsg
 * @param name The name identifier for the field (optional).
 * @param fid The field identifier.
 * @param value[] Array of unsigned 32 bit integers.
 * @param numElements The number of elements in the array.
 */
MAMAExpDLL
extern mama_status
mamaMsg_addVectorU32 (
    mamaMsg            msg,
    const char*        name,
    mama_fid_t         fid,
    const mama_u32_t   value[],
    mama_size_t        numElements);

/**
 * Add an array of signed 64 bit integers to the message.
 *
 * @param msg The mamaMsg
 * @param name The name identifier for the field (optional).
 * @param fid The field identifier.
 * @param value[] Array of signed 64 bit integers.
 * @param numElements The number of elements in the array.
 */
MAMAExpDLL
extern mama_status
mamaMsg_addVectorI64 (
    mamaMsg            msg,
    const char*        name,
    mama_fid_t         fid,
    const mama_i64_t   value[],
    mama_size_t        numElements);

/**
 * Add an array of unsigned 64 bit integers to the message.
 *
 * @param msg The mamaMsg
 * @param name The name identifier for the field (optional).
 * @param fid The field identifier.
 * @param value[] Array of unsigned 64 bit integers.
 * @param numElements The number of elements in the array.
 */
MAMAExpDLL
extern mama_status
mamaMsg_addVectorU64 (
    mamaMsg            msg,
    const char*        name,
    mama_fid_t         fid,
    const mama_u64_t   value[],
    mama_size_t        numElements);

/**
 * Add an array of 32 bit floating point numbers to the message.
 *
 * @param msg The mamaMsg
 * @param name The name identifier for the field (optional).
 * @param fid The field identifier.
 * @param value[] Array of 32 bit floating point numbers.
 * @param numElements The number of elements in the array.
 */
MAMAExpDLL
extern mama_status
mamaMsg_addVectorF32 (
    mamaMsg            msg,
    const char*        name,
    mama_fid_t         fid,
    const mama_f32_t   value[],
    mama_size_t        numElements);

/**
 * Add an array of 64 bit floating point numbers to the message.
 *
 * @param msg The mamaMsg
 * @param name The name identifier for the field (optional).
 * @param fid The field identifier.
 * @param value[] Array of 64 bit floating point numbers.
 * @param numElements The number of elements in the array.
 */
MAMAExpDLL
extern mama_status
mamaMsg_addVectorF64 (
    mamaMsg            msg,
    const char*        name,
    mama_fid_t         fid,
    const mama_f64_t   value[],
    mama_size_t        numElements);

/**
 * Add an array of strings (char*) to the message.
 *
 * @param msg The mamaMsg
 * @param name The name identifier for the field (optional).
 * @param fid The field identifier.
 * @param value[] Array of strings (char*).
 * @param numElements The number of elements in the array.
 */
MAMAExpDLL
extern mama_status
mamaMsg_addVectorString (
    mamaMsg      msg,
    const char*  name,
    mama_fid_t   fid,
    const char*  value[],
    mama_size_t  numElements);

/**
 * Add an array of mamaMsg objects to the message.
 *
 * @param msg The message to which the field is being added.
 * @param name The name identifier for the field (optional).
 * @param fid The field identifier
 * @param value[] The value of the field.
 * @param numElements The number of elements in the mamaMsg array.
 *
 * @return mama_status MAMA_STATUS_OK if the function returns successfully.
 */
MAMAExpDLL
extern mama_status
mamaMsg_addVectorMsg(
    mamaMsg         msg,
    const char*     name,
    mama_fid_t      fid,
    const mamaMsg   value[],
    mama_size_t     numElements);

/**
 * Add an array of timestamps to the message.
 *
 * @param msg The mamaMsg
 * @param name The name identifier for the field (optional).
 * @param fid The field identifier.
 * @param value[] Array of timestamps.
 * @param numElements The number of elements in the array.
 */
MAMAExpDLL
extern mama_status
mamaMsg_addVectorDateTime (
    mamaMsg             msg,
    const char*         name,
    mama_fid_t          fid,
    const mamaDateTime  value[],
    mama_size_t         numElements);

/**
 * Add an array of prices to the message.
 *
 * @param msg The mamaMsg
 * @param name The name identifier for the field (optional).
 * @param fid The field identifier.
 * @param value[] Array of values.
 * @param numElements The number of elements in the array.
 */
MAMAExpDLL
extern mama_status
mamaMsg_addVectorPrice (
    mamaMsg             msg,
    const char*         name,
    mama_fid_t          fid,
    const mamaPrice     value[],
    mama_size_t         numElements);

/**
 * Update the value of an existing bool field. If the field does not exist
 * it is added.
 *
 * @param msg The message.
 * @param name The name.
 * @param fid  The field identifier.
 * @param value The new value.
 */
MAMAExpDLL
extern mama_status
mamaMsg_updateBool(
        mamaMsg      msg,
        const char*  name,
        mama_fid_t   fid,
        mama_bool_t  value);

/**
 * Update the value of an existing char field. If the field does not exist
 * it is added.
 *
 * @param msg The message.
 * @param name The name.
 * @param fid  The field identifier.
 * @param value The new value.
 */
MAMAExpDLL
extern mama_status
mamaMsg_updateChar(
        mamaMsg     msg,
        const char* name,
        mama_fid_t  fid,
        char        value);

/**
 * Update the value of an existing I8 field. If the field does not exist
 * it is added.
 *
 * @param msg The message.
 * @param name The name.
 * @param fid  The field identifier.
 * @param value The new value.
 */
MAMAExpDLL
extern mama_status
mamaMsg_updateI8(
        mamaMsg     msg,
        const char* name,
        mama_fid_t  fid,
        mama_i8_t   value);

/**
 * Update the value of an existing U8 field. If the field does not exist
 * it is added.
 *
 * @param msg The message.
 * @param name The name.
 * @param fid  The field identifier.
 * @param value The new value.
 */
MAMAExpDLL
extern mama_status
mamaMsg_updateU8(
        mamaMsg      msg,
        const char*  name,
        mama_fid_t   fid,
        mama_u8_t    value);

/**
 * Update the value of an existing I16 field. If the field does not exist
 * it is added.
 *
 * @param msg The message.
 * @param name The name.
 * @param fid  The field identifier.
 * @param value The new value.
 */
MAMAExpDLL
extern mama_status
mamaMsg_updateI16(
        mamaMsg      msg,
        const char*  name,
        mama_fid_t   fid,
        mama_i16_t   value);

/**
 * Update the value of an existing U16 field. If the field does not exist
 * it is added.
 *
 * @param msg The message.
 * @param name The name.
 * @param fid  The field identifier.
 * @param value The new value.
 */
MAMAExpDLL
extern mama_status
mamaMsg_updateU16(
        mamaMsg      msg,
        const char*  name,
        mama_fid_t   fid,
        mama_u16_t   value);

/**
 * Update the value of an existing I32 field. If the field does not exist
 * it is added.
 *
 * @param msg The message.
 * @param name The name.
 * @param fid  The field identifier.
 * @param value The new value.
 */
MAMAExpDLL
extern mama_status
mamaMsg_updateI32(
    mamaMsg      msg,
    const char*  name,
    mama_fid_t   fid,
    mama_i32_t   value);

/**
 * Update the value of an existing U32 field. If the field does not exist
 * it is added.
 *
 * @param msg The message.
 * @param name The name.
 * @param fid  The field identifier.
 * @param value The new value.
 */
MAMAExpDLL
extern mama_status
mamaMsg_updateU32(
    mamaMsg      msg,
    const char*  name,
    mama_fid_t   fid,
    mama_u32_t   value);

/**
 * Update the value of an existing I64 field. If the field does not exist
 * it is added.
 *
 * @param msg The message.
 * @param name The name.
 * @param fid  The field identifier.
 * @param value The new value.
 */
MAMAExpDLL
extern mama_status
mamaMsg_updateI64(
    mamaMsg      msg,
    const char*  name,
    mama_fid_t   fid,
    mama_i64_t   value);

/**
 * Update the value of an existing U64 field. If the field does not exist
 * it is added.
 *
 * @param msg The message.
 * @param name The name.
 * @param fid  The field identifier.
 * @param value The new value.
 */
MAMAExpDLL
extern mama_status
mamaMsg_updateU64(
    mamaMsg      msg,
    const char*  name,
    mama_fid_t   fid,
    mama_u64_t   value);

/**
 * Update the value of an existing float field. If the field does not exist
 * it is added.
 *
 * @param msg The message.
 * @param name The name.
 * @param fid  The field identifier.
 * @param value The new value.
 */
MAMAExpDLL
extern mama_status
mamaMsg_updateF32(
    mamaMsg      msg,
    const char*  name,
    mama_fid_t   fid,
    mama_f32_t   value);

/**
 * Update the value of an existing f64 field. If the field does not exist
 * it is added.
 *
 * @param msg The message.
 * @param name The name.
 * @param fid  The field identifier.
 * @param value The new value.
 */
MAMAExpDLL
extern mama_status
mamaMsg_updateF64(
    mamaMsg      msg,
    const char*  name,
    mama_fid_t   fid,
    mama_f64_t   value);

/**
 * Update the value of an existing const char* field. If the field
 * does not exist it is added.
 *
 * @param msg The message.
 * @param name The name.
 * @param fid  The field identifier.
 * @param value The new value.
 */
MAMAExpDLL
extern mama_status
mamaMsg_updateString(
    mamaMsg      msg,
    const char*  name,
    mama_fid_t   fid,
    const char*  value);

/**
 * Update an opaque field.
 *
 * @param msg The message.
 * @param name The name.
 * @param fid  The field identifier.
 * @param value The value.
 * @param size The size of the opaque in bytes
 */
MAMAExpDLL
extern mama_status
mamaMsg_updateOpaque(
    mamaMsg      msg,
    const char*  name,
    mama_fid_t   fid,
    const void*  value,
    mama_size_t  size);

/**
 * Update a MAMA date/time field.
 *
 * @param msg The message.
 * @param name The name.
 * @param fid  The field identifier.
 * @param value The value.
 */
MAMAExpDLL
extern mama_status
mamaMsg_updateDateTime(
    mamaMsg             msg,
    const char*         name,
    mama_fid_t          fid,
    const mamaDateTime  value);

/**
 * Update a MAMA price field.
 *
 * @param msg The message.
 * @param name The name.
 * @param fid  The field identifier.
 * @param value The value.
 */
MAMAExpDLL
extern mama_status
mamaMsg_updatePrice(
    mamaMsg             msg,
    const char*         name,
    mama_fid_t          fid,
    const mamaPrice     value);

/**
 * Get a bool field.
 *
 * @param msg The message.
 * @param name The name
 * @param fid  The field identifier
 * @param result (out) Pointer to the result
 */
MAMAExpDLL
extern mama_status
mamaMsg_getBool(
    const mamaMsg  msg,
    const char*    name,
    mama_fid_t     fid,
    mama_bool_t*   result);

/**
 * Apply the contents of one message to another as updates.
 *
 * @param dest The message to update.
 * @param src The source messages.
 */
MAMAExpDLL
extern mama_status
mamaMsg_applyMsg (mamaMsg    dest, mamaMsg    src);

/**
 * Get the native message structure for the underlying message
 * This function is for internal NYSE Technologies use only.
 *
 * @param msg The message
 * @param nativeMsg The resulting native handle.
 * @return mama_status Returns MAMA_STATUS_OK if the call was successful.
 */
MAMAExpDLL
extern mama_status
mamaMsg_getNativeMsg (mamaMsg msg, void** nativeMsg);

/**
 * Get the reply handle for the message and increments the reference count for
 * the handle.
 *
 * @param msg The message
 * @param replyHandle The resulting reply handle.
 * @return mama_status Returns MAMA_STATUS_OK if the call was successful.
 */
MAMAExpDLL
extern mama_status
mamaMsg_getReplyHandle (mamaMsg msg, mamaMsgReply* replyHandle);

/**
 * Set the reply handle for the message and increments the reference count for
 * the handle.
 *
 * @param msg The message
 * @param replyHandle The reply handle for this message.
 * @return mama_status Returns MAMA_STATUS_OK if the call was successful.
 */
MAMAExpDLL
extern mama_status
mamaMsg_setReplyHandle (mamaMsg msg, mamaMsgReply replyHandle);

/**
 * Create a new copy of the src reply handle.
 *
 * @param src The reply handler to copy.
 * @param copy A pointer to the destination reply handle.
 * @return mama_status Returns MAMA_STATUS_OK if the call was successful.
 */
MAMAExpDLL
extern mama_status
mamaMsg_copyReplyHandle (mamaMsgReply src, mamaMsgReply* copy);

/**
 * Destroys a reply handle.
 * @param replyHandle The reply handle for this message.
 * @return mama_status Returns MAMA_STATUS_OK if the call was successful.
 */MAMAExpDLL
extern mama_status
mamaMsg_destroyReplyHandle (mamaMsgReply replyHandle);

/**
 * Update a sub-message field.
 *
 * @param msg The message.
 * @param fname The name
 * @param fid  The field identifier
 * @param subMsg The new value.
 */
MAMAExpDLL
extern mama_status
mamaMsg_updateSubMsg (
    mamaMsg               msg,
    const char*           fname,
    mama_fid_t            fid,
    const mamaMsg         subMsg);

/**
 * Update a vector message field.
 *
 * @param msg The message.
 * @param fname The name
 * @param fid  The field identifier
 * @param msgList The new value.
 * @param numElements the number of elements in the vector
 */
MAMAExpDLL
extern mama_status
mamaMsg_updateVectorMsg (
    mamaMsg               msg,
    const char*           fname,
    mama_fid_t            fid,
    const mamaMsg         msgList[],
    mama_size_t           numElements);

/**
 * Update a vector string field.
 *
 * @param msg The message.
 * @param fname The name
 * @param fid  The field identifier
 * @param strList The new value.
 * @param numElements the number of elements in the vector
 */
MAMAExpDLL
extern mama_status
mamaMsg_updateVectorString (
    mamaMsg               msg,
    const char*           fname,
    mama_fid_t            fid,
    const char*           strList[],
    mama_size_t           numElements);

/**
 * Update a vector bool field.
 *
 * @param msg The message.
 * @param fname The name
 * @param fid  The field identifier
 * @param boolList The new value.
 * @param numElements the number of elements in the vector
 */
MAMAExpDLL
extern mama_status
mamaMsg_updateVectorBool (
    mamaMsg               msg,
    const char*           fname,
    mama_fid_t            fid,
    const mama_bool_t     boolList[],
    mama_size_t           numElements);

/**
 * Update a vector char field.
 *
 * @param msg The message.
 * @param fname The name
 * @param fid  The field identifier
 * @param charList The new value.
 * @param numElements the number of elements in the vector
 */
MAMAExpDLL
extern mama_status
mamaMsg_updateVectorChar (
    mamaMsg               msg,
    const char*           fname,
    mama_fid_t            fid,
    const char            charList[],
    mama_size_t           numElements);

/**
 * Update a vector I8 field.
 *
 * @param msg The message.
 * @param fname The name
 * @param fid  The field identifier
 * @param i8List The new value.
 * @param numElements the number of elements in the vector
 */
MAMAExpDLL
extern mama_status
mamaMsg_updateVectorI8 (
    mamaMsg               msg,
    const char*           fname,
    mama_fid_t            fid,
    const mama_i8_t       i8List[],
    mama_size_t           numElements);

/**
 * Update a vector U8 field.
 *
 * @param msg The message.
 * @param fname The name
 * @param fid  The field identifier
 * @param u8List The new value.
 * @param numElements the number of elements in the vector
 */
MAMAExpDLL
extern mama_status
mamaMsg_updateVectorU8 (
    mamaMsg               msg,
    const char*           fname,
    mama_fid_t            fid,
    const mama_u8_t       u8List[],
    mama_size_t           numElements);

/**
 * Update a vector I16 field.
 *
 * @param msg The message.
 * @param fname The name
 * @param fid  The field identifier
 * @param i16List The new value.
 * @param numElements the number of elements in the vector
 */
MAMAExpDLL
extern mama_status
mamaMsg_updateVectorI16 (
    mamaMsg               msg,
    const char*           fname,
    mama_fid_t            fid,
    const mama_i16_t      i16List[],
    mama_size_t           numElements);

/**
 * Update a vector U16 field.
 *
 * @param msg The message.
 * @param fname The name
 * @param fid  The field identifier
 * @param u16List The new value.
 * @param numElements the number of elements in the vector
 */
MAMAExpDLL
extern mama_status
mamaMsg_updateVectorU16 (
    mamaMsg               msg,
    const char*           fname,
    mama_fid_t            fid,
    const mama_u16_t      u16List[],
    mama_size_t           numElements);

/**
 * Update a vector I32 field.
 *
 * @param msg The message.
 * @param fname The name
 * @param fid  The field identifier
 * @param i32List The new value.
 * @param numElements the number of elements in the vector
 */
MAMAExpDLL
extern mama_status
mamaMsg_updateVectorI32 (
    mamaMsg               msg,
    const char*           fname,
    mama_fid_t            fid,
    const mama_i32_t      i32List[],
    mama_size_t           numElements);

/**
 * Update a vector U32 field.
 *
 * @param msg The message.
 * @param fname The name
 * @param fid  The field identifier
 * @param u32List The new value.
 * @param numElements the number of elements in the vector
 */
MAMAExpDLL
extern mama_status
mamaMsg_updateVectorU32 (
    mamaMsg               msg,
    const char*           fname,
    mama_fid_t            fid,
    const mama_u32_t      u32List[],
    mama_size_t           numElements);

/**
 * Update a vector I64 field.
 *
 * @param msg The message.
 * @param fname The name
 * @param fid  The field identifier
 * @param i64List The new value.
 * @param numElements the number of elements in the vector
 */
MAMAExpDLL
extern mama_status
mamaMsg_updateVectorI64 (
    mamaMsg               msg,
    const char*           fname,
    mama_fid_t            fid,
    const mama_i64_t      i64List[],
    mama_size_t           numElements);

/**
 * Update a vector U64 field.
 *
 * @param msg The message.
 * @param fname The name
 * @param fid  The field identifier
 * @param u64List The new value.
 * @param numElements the number of elements in the vector
 */
MAMAExpDLL
extern mama_status
mamaMsg_updateVectorU64 (
    mamaMsg               msg,
    const char*           fname,
    mama_fid_t            fid,
    const mama_u64_t      u64List[],
    mama_size_t           numElements);

/**
 * Update a vector F32 field.
 *
 * @param msg The message.
 * @param fname The name
 * @param fid  The field identifier
 * @param f32List The new value.
 * @param numElements the number of elements in the vector
 */
MAMAExpDLL
extern mama_status
mamaMsg_updateVectorF32 (
    mamaMsg               msg,
    const char*           fname,
    mama_fid_t            fid,
    const mama_f32_t       f32List[],
    mama_size_t           numElements);

/**
 * Update a vector F64 field.
 *
 * @param msg The message.
 * @param fname The name
 * @param fid  The field identifier
 * @param f64List The new value.
 * @param numElements the number of elements in the vector
 */
MAMAExpDLL
extern mama_status
mamaMsg_updateVectorF64 (
    mamaMsg               msg,
    const char*           fname,
    mama_fid_t            fid,
    const mama_f64_t       f64List[],
    mama_size_t           numElements);

/**
 * Update a vector price field.
 *
 * @param msg The message.
 * @param fname The name
 * @param fid  The field identifier
 * @param priceList The new value.
 * @param numElements the number of elements in the vector
 */
MAMAExpDLL
extern mama_status
mamaMsg_updateVectorPrice (
    mamaMsg               msg,
    const char*           fname,
    mama_fid_t            fid,
    const mamaPrice*      priceList[],
    mama_size_t           numElements);

/**
 * Update a vector mamaDateTime field.
 *
 * @param msg The message.
 * @param fname The name
 * @param fid  The field identifier
 * @param timeList The new value.
 * @param numElements the number of elements in the vector
 */
MAMAExpDLL
extern mama_status
mamaMsg_updateVectorTime (
    mamaMsg               msg,
    const char*           fname,
    mama_fid_t            fid,
    const mamaDateTime    timeList[],
    mama_size_t           numElements);


/**
 * Get a char field.
 *
 * @param msg The message.
 * @param name The name
 * @param fid  The field identifier
 * @param result (out) Pointer to the result
 */
MAMAExpDLL
extern mama_status
mamaMsg_getChar(
    const mamaMsg  msg,
    const char*    name,
    mama_fid_t     fid,
    char*          result);

/**
 * Get a I8, signed 8 bit integer, field.
 *
 * @param msg The message.
 * @param name The name
 * @param fid  The field identifier
 * @param result (out) Pointer to the result
 */
MAMAExpDLL
extern mama_status
mamaMsg_getI8(
    const mamaMsg  msg,
    const char*    name,
    mama_fid_t     fid,
    mama_i8_t*     result);

/**
 * Get an unsigned 8-bit integer field
 *
 * @param msg The message.
 * @param name The name
 * @param fid  The field identifier
 * @param result (out) Pointer to the result
 */
MAMAExpDLL
extern mama_status
mamaMsg_getU8(
    const mamaMsg  msg,
    const char*    name,
    mama_fid_t     fid,
    mama_u8_t*     result);

/**
 * Get a I16, signed 16 bit integer, field.
 *
 * @param msg The message.
 * @param name The name
 * @param fid  The field identifier
 * @param result (out) Pointer to the result
 */
MAMAExpDLL
extern mama_status
mamaMsg_getI16(
    const mamaMsg   msg,
    const char*     name,
    mama_fid_t      fid,
    mama_i16_t*     result);

/**
 * Get an unsigned 16-bit integer field
 *
 * @param msg The message.
 * @param name The name
 * @param fid  The field identifier
 * @param result (out) Pointer to the result
 */
MAMAExpDLL
extern mama_status
mamaMsg_getU16(
        const mamaMsg   msg,
        const char*     name,
        mama_fid_t      fid,
        mama_u16_t*     result);

/**
 * Get a I32, signed 32 bit integer, field.
 *
 * @param msg The message.
 * @param name The name
 * @param fid  The field identifier
 * @param result (out) Pointer to the result
 */
MAMAExpDLL
extern mama_status
mamaMsg_getI32(
    const mamaMsg  msg,
    const char*    name,
    mama_fid_t     fid,
    mama_i32_t*    result);

/**
 * Get an unsigned 32-bit integer field
 *
 * @param msg The message.
 * @param name The name
 * @param fid  The field identifier
 * @param result (out) Pointer to the result
 */
MAMAExpDLL
extern mama_status
mamaMsg_getU32(
    const mamaMsg  msg,
    const char*    name,
    mama_fid_t     fid,
    mama_u32_t*    result);


/**
 * Get a I64, signed 64 bit integer, field.
 *
 * @param msg The message.
 * @param name The name
 * @param fid  The field identifier
 * @param result (out) Pointer to the result
 */
MAMAExpDLL
extern mama_status
mamaMsg_getI64(
    const mamaMsg  msg,
    const char*    name,
    mama_fid_t     fid,
    mama_i64_t*    result);

/**
 * Get an unsigned 64-bit integer field
 *
 * @param msg The message.
 * @param name The name
 * @param fid  The field identifier
 * @param result (out) Pointer to the result
 */
MAMAExpDLL
extern mama_status
mamaMsg_getU64(
    const mamaMsg  msg,
    const char*    name,
    mama_fid_t     fid,
    mama_u64_t*    result);

/**
 * Get a float (f32) field.
 *
 * @param msg The message.
 * @param name The name.
 * @param fid The field identifier.
 * @param result (out) The double value.
 */
MAMAExpDLL
extern mama_status
mamaMsg_getF32(
    const mamaMsg  msg,
    const char*    name,
    mama_fid_t     fid,
    mama_f32_t*    result);

/**
 * Get a f64 field.
 *
 * @param msg The message.
 * @param name The name.
 * @param fid The field identifier.
 * @param result (out) The double value.
 */
MAMAExpDLL
extern mama_status
mamaMsg_getF64(
    const mamaMsg  msg,
    const char*    name,
    mama_fid_t     fid,
    mama_f64_t*    result);

/**
 * Get a const char * field.
 *
 * @param msg The message.
 * @param name The field name.
 * @param fid The field identifier.
 * @param result (out)  the string value.
 */
MAMAExpDLL
extern mama_status
mamaMsg_getString(
    const mamaMsg  msg,
    const char*    name,
    mama_fid_t     fid,
    const char**   result);

/**
 * Get an opaque field.
 *
 * @param msg The message.
 * @param name The field name.
 * @param fid The field identifier.
 * @param result (out) The opaque value.
 * @param size (out) Length in bytes of the opaque field.
 */
MAMAExpDLL
extern mama_status
mamaMsg_getOpaque(
    const mamaMsg  msg,
    const char*    name,
    mama_fid_t     fid,
    const void**   result,
    mama_size_t*   size);


/**
 * Get a MAMA msg field.
 *
 * @param msg The message.
 * @param name The name
 * @param fid  The field identifier
 * @param result (out) Pointer to the result.
 * The result contains the reusable field object of the
 * mamaMsg object. Applications calling this method will receive the same
 * reusable object for repeated calls on same mamaMsg object.
 */
MAMAExpDLL
extern mama_status
mamaMsg_getField(
    const mamaMsg  msg,
    const char*    name,
    mama_fid_t     fid,
    mamaMsgField*   result);

/**
 * Get a MAMA date/time field.
 *
 * @param msg The message.
 * @param name The name
 * @param fid  The field identifier
 * @param result (out) Pointer to the result
 */
MAMAExpDLL
extern mama_status
mamaMsg_getDateTime(
    const mamaMsg  msg,
    const char*    name,
    mama_fid_t     fid,
    mamaDateTime   result);

/**
 * Get the value of a MAMA date/time field in milliseconds
 *
 * @param msg The message.
 * @param name The name
 * @param fid  The field identifier
 * @param milliseconds (out) Pointer to the value in milliseconds
 */
MAMAExpDLL
extern
mama_status
mamaMsg_getDateTimeMSec(
    const mamaMsg  msg,
    const char*    name,
    mama_fid_t     fid,
    mama_u64_t*    milliseconds);

/**
 * Get a MAMA price field.
 *
 * @param msg The message.
 * @param name The name
 * @param fid  The field identifier
 * @param result (out) Pointer to the result
 */
MAMAExpDLL
extern mama_status
mamaMsg_getPrice(
    const mamaMsg  msg,
    const char*    name,
    mama_fid_t     fid,
    mamaPrice      result);

/**
 * Get a submessage.
 *
 * @param msg The message.
 * @param name The field name.
 * @param fid The field identifier.
 * @param result A pointer to the result.
 * result when the method returns..
 */
MAMAExpDLL
extern mama_status
mamaMsg_getMsg (
    const mamaMsg  msg,
    const char*    name,
    mama_fid_t     fid,
    mamaMsg*       result);

/**
 * Get a vector of booleans.
 *
 * @param msg The message.
 * @param name The field name.
 * @param fid The field identifier.
 * @param result A pointer to the result.
 * @param resultLen An integer pointer that will contain the length of the
 * result when the method returns..
 */
MAMAExpDLL
extern mama_status
mamaMsg_getVectorBool (
    const mamaMsg        msg,
    const char*          name,
    mama_fid_t           fid,
    const mama_bool_t**  result,
    mama_size_t*         resultLen);

/**
 * Get a vector of chars.
 *
 * @param msg The message.
 * @param name The field name.
 * @param fid The field identifier.
 * @param result A pointer to the result.
 * @param resultLen An integer pointer that will contain the length of the
 * result when the method returns..
 */
MAMAExpDLL
extern mama_status
mamaMsg_getVectorChar (
    const mamaMsg        msg,
    const char*          name,
    mama_fid_t           fid,
    const char**         result,
    mama_size_t*         resultLen);

/**
 * Get a vector of signed 8 bit integers.
 *
 * @param msg The message.
 * @param name The field name.
 * @param fid The field identifier.
 * @param result A pointer to the result.
 * @param resultLen An integer pointer that will contain the length of the
 * result when the method returns..
 */
MAMAExpDLL
extern mama_status
mamaMsg_getVectorI8 (
    const mamaMsg        msg,
    const char*          name,
    mama_fid_t           fid,
    const mama_i8_t**    result,
    mama_size_t*         resultLen);

/**
 * Get a vector of unsigned 8 bit integers.
 *
 * @param msg The message.
 * @param name The field name.
 * @param fid The field identifier.
 * @param result A pointer to the result.
 * @param resultLen An integer pointer that will contain the length of the
 * result when the method returns..
 */
MAMAExpDLL
extern mama_status
mamaMsg_getVectorU8 (
    const mamaMsg        msg,
    const char*          name,
    mama_fid_t           fid,
    const mama_u8_t**    result,
    mama_size_t*         resultLen);

/**
 * Get a vector of signed 16 bit integers.
 *
 * @param msg The message.
 * @param name The field name.
 * @param fid The field identifier.
 * @param result A pointer to the result.
 * @param resultLen An integer pointer that will contain the length of the
 * result when the method returns..
 */
MAMAExpDLL
extern mama_status
mamaMsg_getVectorI16 (
    const mamaMsg        msg,
    const char*          name,
    mama_fid_t           fid,
    const mama_i16_t**   result,
    mama_size_t*         resultLen);

/**
 * Get a vector of unsigned 16 bit integers.
 *
 * @param msg The message.
 * @param name The field name.
 * @param fid The field identifier.
 * @param result A pointer to the result.
 * @param resultLen An integer pointer that will contain the length of the
 * result when the method returns..
 */
MAMAExpDLL
extern mama_status
mamaMsg_getVectorU16 (
    const mamaMsg        msg,
    const char*          name,
    mama_fid_t           fid,
    const mama_u16_t**   result,
    mama_size_t*         resultLen);

/**
 * Get a vector of signed 32 bit integers.
 *
 * @param msg The message.
 * @param name The field name.
 * @param fid The field identifier.
 * @param result A pointer to the result.
 * @param resultLen An integer pointer that will contain the length of the
 * result when the method returns..
 */
MAMAExpDLL
extern mama_status
mamaMsg_getVectorI32 (
    const mamaMsg        msg,
    const char*          name,
    mama_fid_t           fid,
    const mama_i32_t**   result,
    mama_size_t*         resultLen);

/**
 * Get a vector of unsigned 32 bit integers.
 *
 * @param msg The message.
 * @param name The field name.
 * @param fid The field identifier.
 * @param result A pointer to the result.
 * @param resultLen An integer pointer that will contain the length of the
 * result when the method returns..
 */
MAMAExpDLL
extern mama_status
mamaMsg_getVectorU32 (
    const mamaMsg        msg,
    const char*          name,
    mama_fid_t           fid,
    const mama_u32_t**   result,
    mama_size_t*         resultLen);

/**
 * Get a vector of signed 64 bit integers.
 *
 * @param msg The message.
 * @param name The field name.
 * @param fid The field identifier.
 * @param result A pointer to the result.
 * @param resultLen An integer pointer that will contain the length of the
 * result when the method returns..
 */
MAMAExpDLL
extern mama_status
mamaMsg_getVectorI64 (
    const mamaMsg        msg,
    const char*          name,
    mama_fid_t           fid,
    const mama_i64_t**   result,
    mama_size_t*         resultLen);

/**
 * Get a vector of unsigned 64 bit integers.
 *
 * @param msg The message.
 * @param name The field name.
 * @param fid The field identifier.
 * @param result A pointer to the result.
 * @param resultLen An integer pointer that will contain the length of the
 * result when the method returns..
 */
MAMAExpDLL
extern mama_status
mamaMsg_getVectorU64 (
    const mamaMsg        msg,
    const char*          name,
    mama_fid_t           fid,
    const mama_u64_t**   result,
    mama_size_t*         resultLen);

/**
 * Get a vector of 32 bit floating point numbers.
 *
 * @param msg The message.
 * @param name The field name.
 * @param fid The field identifier.
 * @param result A pointer to the result.
 * @param resultLen An integer pointer that will contain the length of the
 * result when the method returns..
 */
MAMAExpDLL
extern mama_status
mamaMsg_getVectorF32 (
    const mamaMsg        msg,
    const char*          name,
    mama_fid_t           fid,
    const mama_f32_t**   result,
    mama_size_t*         resultLen);

/**
 * Get a vector of 64 bit floating point numbers.
 *
 * @param msg The message.
 * @param name The field name.
 * @param fid The field identifier.
 * @param result A pointer to the result.
 * @param resultLen An integer pointer that will contain the length of the
 * result when the method returns..
 */
MAMAExpDLL
extern mama_status
mamaMsg_getVectorF64 (
    const mamaMsg        msg,
    const char*          name,
    mama_fid_t           fid,
    const mama_f64_t**   result,
    mama_size_t*         resultLen);

/**
 * Get a vector of strings (char*).
 *
 * @param msg The message.
 * @param name The field name.
 * @param fid The field identifier.
 * @param result A pointer to the result.
 * @param resultLen An integer pointer that will contain the length of the
 * result when the method returns..
 */
MAMAExpDLL
extern mama_status
mamaMsg_getVectorString (
    const mamaMsg   msg,
    const char*     name,
    mama_fid_t      fid,
    const char***   result,
    mama_size_t*    resultLen);

/**
 * Get a vector of timestamps.
 *
 * @param msg The message.
 * @param name The field name.
 * @param fid The field identifier.
 * @param result A pointer to the result.
 * @param resultLen An integer pointer that will contain the length of the
 * result when the method returns..
 */
MAMAExpDLL
extern mama_status
mamaMsg_getVectorDateTime (
    const mamaMsg         msg,
    const char*           name,
    mama_fid_t            fid,
    const mamaDateTime*   result,
    mama_size_t*          resultLen);

/**
 * Get a vector of prices.
 *
 * @param msg The message.
 * @param name The field name.
 * @param fid The field identifier.
 * @param result A pointer to the result.
 * @param resultLen An integer pointer that will contain the length of the
 * result when the method returns..
 */
MAMAExpDLL
extern mama_status
mamaMsg_getVectorPrice (
    const mamaMsg         msg,
    const char*           name,
    mama_fid_t            fid,
    const mamaPrice*      result,
    mama_size_t*          resultLen);

/**
 * Get a vector of submessages.
 *
 * @param msg The message.
 * @param name The field name.
 * @param fid The field identifier.
 * @param result A pointer to the result.
 * @param resultLen An integer pointer that will contain the length of the
 * result when the method returns..
 */
MAMAExpDLL
extern mama_status
mamaMsg_getVectorMsg (
    const mamaMsg        msg,
    const char*          name,
    mama_fid_t           fid,
    const mamaMsg**      result,
    mama_size_t*         resultLen);

/**
 * Returns the total number of fields in the message. Sub-messages count as
 * a single field.
 *
 *
 * @param msg The message.
 * @param numFields (out) The number of fields in the message.
 */
MAMAExpDLL
extern mama_status
mamaMsg_getNumFields(
    const mamaMsg  msg,
    mama_size_t*   numFields);

/**
 * Return a const char * representation the message. Must call
 * mamaMsg_freeString() to free memory allocated for string.
 *
 * @param msg The message.
 * @return A string representation of the message.
 */
MAMAExpDLL
extern const char*
mamaMsg_toString(
    const mamaMsg msg);

/**
 * Free the memory allocated by mamaMsg_toString.
 *
 * @param msg The message.
 * @param msgString The string allocated by mamaMsg_toString
 */
MAMAExpDLL
extern void
mamaMsg_freeString(
    const mamaMsg  msg,
    const char*    msgString );

/**
 * Get the entitle code for this message. The result defaults to 0 (no
 * entitlement) if the field is not present.
 *
 * @param msg The message.
 * @param code (out) The entitle code
 */
MAMAExpDLL
extern mama_status
mamaMsg_getEntitleCode(
    const mamaMsg  msg,
    mama_i32_t*    code );

/**
 * Get the sequence number for this message if available. The result defaults to
 * 0 and MAMA_STATUS_NOT_FOUND retuned if the field is not present.
 *
 * @param msg The message.
 * @param code (out) The sequence number.
 */
MAMAExpDLL
extern mama_status
mamaMsg_getSeqNum(
    const mamaMsg   msg,
    mama_seqnum_t*  seqNum);

/**
 * Extract the type from the supplied message.
 *
 *
 * @param msg The message.
 * @param msg
 * @return The type.
 */
MAMAExpDLL
extern mamaMsgType
mamaMsgType_typeForMsg (const mamaMsg msg);

/**
 * Return the type name.
 *
 * @param msg The message.
 * @return The type name.
 *
 */
MAMAExpDLL
extern const char* mamaMsgType_stringForMsg (const mamaMsg msg);

/**
 * Return the type name.
 *
 * @param type The message.
 * @return The type name.
 */
MAMAExpDLL
extern const char* mamaMsgType_stringForType (const mamaMsgType type);

/**
 * Convert the value of the specified field to a string. Caller must
 * provide a buffer for the value.
 *
 * @param msg The message.
 * @param fieldName The field name.
 * @param fid The field identifier.
 * @param buf The buffer where the resulting string will be copied.
 * @param length The length of the caller supplied buffer.
 */
MAMAExpDLL
extern mama_status
mamaMsg_getFieldAsString (
    const mamaMsg  msg,
    const char*    fieldName,
    mama_fid_t     fid,
    char*          buf,
    mama_size_t    length);

/**
 * Iterator callback method is invoked for each field in the message.
 *
 * @param msg The message.
 * @param field The field.
 * @param closure The closure specified in mamaMsg_iterateFields().
 *
 */
typedef void(MAMACALLTYPE *mamaMsgIteratorCb)(const mamaMsg       msg,
                                 const mamaMsgField  field,
                                 void*               closure);

/**
 * Invoke the specified callback for each field in the message.
 *
 * @param msg The message.
 * @param callback The msgIteratorCb to invoke.
 * @param dict The dictionary.
 * @param closure The user specified closure passed to the callback.
 */
MAMAExpDLL
extern mama_status
mamaMsg_iterateFields (const mamaMsg         msg,
                       mamaMsgIteratorCb     callback,
                       const mamaDictionary  dict,
                       void*                 closure);

/**
 * Return the middleware specific platform error. When a mamaSubscription_
 * method returns MAMA_STATUS_PLATFORM the error will be the result from the
 * underlying platform.
 *
 * @param msg The message.
 * @param error (out) The platform dependent result.
 */
MAMAExpDLL
extern mama_status
mamaMsg_getPlatformError (const mamaMsg  msg,
                          void**         error);



/**
 * Return true if this message was sent from a client's inbox.
 */
MAMAExpDLL
extern int
mamaMsg_isFromInbox (const mamaMsg msg);

/**
  * Create a mamaMsg from the provided byte buffer. The application
  * is responsible for destroying the message. In this function a copy of
  * the buffer isn't made and the client must maintain it until after the
  * message has been destroyed using the mamaMsg_destroy function.
  * The function can determine from the buffer whether it is a wombatmsg
  * or the native format for the transport being used.
  *
  * Any transport differences are detailed below.
  *
  * rv: The bufferLength parameter is not required.
  *
  * @param msg The mamaMsg object
  * @param buffer The byte array containing the wire format of the message
  * @param bufferLength The length, in bytes, of the supplied buffer
  *
  * @return mama_status The outcome of the operation
  */
MAMAExpDLL
extern mama_status
mamaMsg_createFromByteBuffer (
            mamaMsg*       msg,
            const void*    buffer,
            mama_size_t    bufferLength);


/**
  * Return the internal message buffer as an array of bytes which is
  * suitable for writing to a file. Do not modify the returned byte
  * buffer as this is the internal message buffer and remains part of
  * the message until it is destroyed. The buffer will be written to
  * the address supplied by the client.
  * The buffer will contain the wire format of the underlying message
  * implementation but does not contain delivery information such as
  * source, topic etc.
  *
  * @param msg The mamaMsg object
  * @param buffer The byte array containing the message wire format
  * @param bufferLength The length, in bytes, of the returned buffer
  *
  * @return mama_status The outcome of the operation
  */
MAMAExpDLL
extern mama_status
mamaMsg_getByteBuffer (const mamaMsg  msg,
                       const void**   buffer,
                       mama_size_t*   bufferLength);

/**
 * Normally the Mama API owns incoming mamaMsg objects and they go out of scope
 * when the message callback returns. Calling this method from the message
 * callback transfers ownership, and responsibility for calling
 * mamaMsg_destroy() to the caller.
 */
MAMAExpDLL
extern mama_status
mamaMsg_detach (mamaMsg  msg);

/**
 * Return true if this message is definitely a duplicate message. This
 * condition will not occur with the current feed handlers.
 */
MAMAExpDLL
mama_status
mamaMsg_getIsDefinitelyDuplicate (mamaMsg msg, int* result);

/**
 * Return true if this message is possibly a duplicate message. This may
 * occur in the event of a fault tolerant feed handler take over where the
 * feed handler replays messages to prevent gaps.
 */
MAMAExpDLL
mama_status
mamaMsg_getIsPossiblyDuplicate (mamaMsg msg, int* result);

/**
 * Return true if the message is possibly delayed. This condition may be
 * true during a fault-tolerant take over.
 */
MAMAExpDLL
mama_status
mamaMsg_getIsPossiblyDelayed (mamaMsg msg, int* result);

/**
 * Return true if the message is delayed. This condition may be
 * true during a faul tolerant take over.
 */
MAMAExpDLL
mama_status
mamaMsg_getIsDefinitelyDelayed (mamaMsg msg, int* result);

/**
 * Return true when the FH sends the message out of sequence.
 */
MAMAExpDLL
mama_status
mamaMsg_getIsOutOfSequence (mamaMsg msg, int* result);

/**
 * Set a new buffer for an existing mamaMsg. This approach is faster than
 * creating a new message for buffers as the message can reuse memory
 * allocated during previous use.
 *
 * @param msg       The mamaMsg that will receive the new buffer.
 * @param buffer    The new buffer to use for this message.
 * @param size      The size of the buffer.
 */
MAMAExpDLL
mama_status
mamaMsg_setNewBuffer (mamaMsg      msg,
                      void*         buffer,
                      mama_size_t   size);

/**
 * Get the native middleware message handle.  This is only
 * intended for internal use.
 */
MAMAExpDLL
mama_status
mamaMsg_getNativeHandle (const mamaMsg  msg,
                         void**         result);

/**
 * Creates a new iterator for use with a mamaMsg
 *
 * @param iterator  Pointer to memory to be used for iterator
 * @param dict      The mamaDictionary to be associated with the iterator
 *
 * @return mama_status The outcome of the operation
 */
MAMAExpDLL
mama_status
mamaMsgIterator_create (mamaMsgIterator* iterator, mamaDictionary dict);
/**
 * Associate an iterator for use with a mamaMsg
 *
 * @param msg       The mamaMsg that will use the iterator
 * @param iterator iterator for use with msg
 *
 * @return mama_status The outcome of the operation
 */

MAMAExpDLL
mama_status mamaMsgIterator_associate (mamaMsgIterator iterator, mamaMsg msg);
/**
 * Associate a mamaDictionary for use with an iterator
 *
 * @param iterator       The iterator that will use the dictionary
 * @param dict           The dictionary to be used
 *
 * @return mama_status The outcome of the operation.
 */
MAMAExpDLL
mama_status mamaMsgIterator_setDict (mamaMsgIterator iterator,
                                     mamaDictionary dict);
/**
 * Return the next field in the mamaMsg
 *
 * @param iterator       The iterator being used
 *
 * @return mamaMsgField The next field in the message
 */
MAMAExpDLL
mamaMsgField
mamaMsgIterator_next (mamaMsgIterator iterator);

/**
 * Return whether there is a next next field in the mamaMsg
 *
 * @param iterator   The iterator being used
 *
 * @return bool       Does the message have another field
 */
MAMAExpDLL
mama_bool_t
mamaMsgIterator_hasNext (mamaMsgIterator iterator);

/**
 * Set iterator to first field in mamaMsg
 *
 * @param iterator       The iterator being used
 *
 * @return mamaMsgField The first field in the message
 */
MAMAExpDLL
mamaMsgField
mamaMsgIterator_begin (mamaMsgIterator iterator);
/**
 * Set iterator to last field in mamaMsg
 *
 * @param iterator The iterator being used
 *
 * @return mamaMsgField  The last field in the message
 */
MAMAExpDLL
mamaMsgField
mamaMsgIterator_end (mamaMsgIterator iterator);

/**
 * Destroy iterator
 *
 * @param iterator The iterator being destroyed
 *
 */
MAMAExpDLL
mama_status
mamaMsgIterator_destroy (mamaMsgIterator iterator);

#if defined(__cplusplus)
}
#endif

#endif /* MAMA_H__ */
