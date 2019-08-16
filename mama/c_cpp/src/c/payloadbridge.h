/* $Id$
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

#ifndef PayloadBridgeH__
#define PayloadBridgeH__


#if defined(__cplusplus)
extern "C" {
#endif

#include <mama/mama.h>
#include <mamainternal.h>
#include <mama/subscmsgtype.h>
#include <mama/integration/msg.h>

#define XSTR(s) STR(s)
#define STR(s) #s


/*===================================================================
 =             general bridge function pointers                 =
 ====================================================================*/

/*Called when loading/creating a bridge */
typedef mama_status
(*msgPayload_init)            (mamaPayloadBridge bridge, char* identifier);

MAMAIgnoreDeprecatedOpen
typedef mamaPayloadType
(*msgPayload_getType)         (void);
MAMAIgnoreDeprecatedClose

/*===================================================================
 =              msgPayload bridge function pointers                 =
 ====================================================================*/
typedef mama_status
(*msgPayload_create)           (msgPayload*         msg);
typedef mama_status
(*msgPayload_createForTemplate)(msgPayload*         msg,
                                mamaPayloadBridge       bridge,
                                mama_u32_t          templateId);
typedef mama_status
(*msgPayload_copy)             (const msgPayload    msg,
                                msgPayload*         copy);
typedef mama_status
(*msgPayload_clear)            (msgPayload          msg);
typedef mama_status
(*msgPayload_destroy)          (msgPayload          msg);
typedef mama_status
(*msgPayload_setParent)        (msgPayload          msg,
                                const mamaMsg       parent);
typedef mama_status
(*msgPayload_getByteSize)      (msgPayload          msg,
                                mama_size_t*        size);
typedef mama_status
(*msgPayload_getNumFields)     (const msgPayload    msg,
                                mama_size_t*        numFields);

typedef mama_status
(*msgPayload_getSendSubject)   (const msgPayload    msg,
                                const char**        subject);
typedef const char*
(*msgPayload_toString)         (const msgPayload    msg);
typedef mama_status
(*msgPayload_iterateFields)    (const msgPayload    msg,
                                const mamaMsg       parent,
                                mamaMsgField        field,
                                mamaMsgIteratorCb   cb,
                                void*               closure);

typedef mama_status
(*msgPayload_serialize)        (const msgPayload    msg,
                                const void**        buffer,
                                mama_size_t*        bufferLength);

typedef mama_status
(*msgPayload_deserialize)      (mamaPayloadBridge   payloadBridge,
                                const msgPayload    msg,
                                const void*         buffer,
                                mama_size_t         bufferLength);

typedef mama_status
(*msgPayload_unSerialize)      (const msgPayload    msg,
                                const void*         buffer,
                                mama_size_t         bufferLength);

typedef mama_status
(*msgPayload_getByteBuffer)    (const msgPayload    msg,
                                const void**        buffer,
                                mama_size_t*        bufferLength);

typedef mama_status
(*msgPayload_setByteBuffer)    (const msgPayload    msg,
                                mamaPayloadBridge   bridge,
                                const void*         buffer,
                                mama_size_t         bufferLength);

typedef mama_status
(*msgPayload_createFromByteBuffer) (
                                msgPayload*         msg,
                                mamaPayloadBridge       bridge,
                                const void*         buffer,
                                mama_size_t         bufferLength);
typedef mama_status
(*msgPayload_apply)            (msgPayload          dest,
                                const msgPayload    src);
typedef mama_status
(*msgPayload_getNativeMsg)     (const msgPayload    msg,
                                void**              nativeMsg);

typedef mama_status
(*msgPayload_getFieldAsString) (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                char*               buffer,
                                mama_size_t         len);
typedef mama_status
(*msgPayload_addBool)          (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_bool_t         value);
typedef mama_status
(*msgPayload_addChar)          (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                char                value);
typedef mama_status
(*msgPayload_addI8)            (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i8_t           value);
typedef mama_status
(*msgPayload_addU8)            (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u8_t           value);
typedef mama_status
(*msgPayload_addI16)           (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i16_t          value);
typedef mama_status
(*msgPayload_addU16)           (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u16_t          value);
typedef mama_status
(*msgPayload_addI32)           (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i32_t          value);
typedef mama_status
(*msgPayload_addU32)           (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u32_t          value);
typedef mama_status
(*msgPayload_addI64)           (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i64_t          value);
typedef mama_status
(*msgPayload_addU64)           (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u64_t          value);
typedef mama_status
(*msgPayload_addF32)           (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_f32_t          value);
typedef mama_status
(*msgPayload_addF64)           (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_f64_t          value);
typedef mama_status
(*msgPayload_addString)        (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const char*         value);
typedef mama_status
(*msgPayload_addOpaque)        (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const void*         value,
                                mama_size_t         size);
typedef mama_status
(*msgPayload_addDateTime)      (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mamaDateTime  value);
typedef mama_status
(*msgPayload_addPrice)         (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mamaPrice     value);
typedef mama_status
(*msgPayload_addMsg)           (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mamaMsg             value);
typedef mama_status
(*msgPayload_addVectorBool)    (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_bool_t   value[],
                                mama_size_t         size);
typedef mama_status
(*msgPayload_addVectorChar)    (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const char          value[],
                                mama_size_t         size);
typedef mama_status
(*msgPayload_addVectorI8)      (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_i8_t     value[],
                                mama_size_t         size);
typedef mama_status
(*msgPayload_addVectorU8)      (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_u8_t     value[],
                                mama_size_t         size);
typedef mama_status
(*msgPayload_addVectorI16)     (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_i16_t    value[],
                                mama_size_t         size);
typedef mama_status
(*msgPayload_addVectorU16)     (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_u16_t    value[],
                                mama_size_t         size);
typedef mama_status
(*msgPayload_addVectorI32)     (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_i32_t    value[],
                                mama_size_t         size);
typedef mama_status
(*msgPayload_addVectorU32)     (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_u32_t    value[],
                                mama_size_t         size);
typedef mama_status
(*msgPayload_addVectorI64)     (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_i64_t    value[],
                                mama_size_t         size);
typedef mama_status
(*msgPayload_addVectorU64)     (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_u64_t    value[],
                                mama_size_t         size);
typedef mama_status
(*msgPayload_addVectorF32)     (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_f32_t    value[],
                                mama_size_t         size);
typedef mama_status
(*msgPayload_addVectorF64)     (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_f64_t    value[],
                                mama_size_t         size);
typedef mama_status
(*msgPayload_addVectorString)  (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const char*         value[],
                                mama_size_t         size);
typedef mama_status
(*msgPayload_addVectorMsg)     (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mamaMsg       value[],
                                mama_size_t         size);
typedef mama_status
(*msgPayload_addVectorDateTime)(msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mamaDateTime  value[],
                                mama_size_t         size);
typedef mama_status
(*msgPayload_addVectorPrice)   (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mamaPrice     value[],
                                mama_size_t         size);
typedef mama_status
(*msgPayload_updateBool)       (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_bool_t         value);
typedef mama_status
(*msgPayload_updateChar)       (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                char                value);
typedef mama_status
(*msgPayload_updateU8)         (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u8_t           value);
typedef mama_status
(*msgPayload_updateI8)         (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i8_t           value);
typedef mama_status
(*msgPayload_updateI16)        (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i16_t          value);
typedef mama_status
(*msgPayload_updateU16)        (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u16_t          value);
typedef mama_status
(*msgPayload_updateI32)        (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i32_t          value);
typedef mama_status
(*msgPayload_updateU32)        (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u32_t          value);
typedef mama_status
(*msgPayload_updateI64)        (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i64_t          value);
typedef mama_status
(*msgPayload_updateU64)        (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u64_t          value);
typedef mama_status
(*msgPayload_updateF32)        (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_f32_t          value);
typedef mama_status
(*msgPayload_updateF64)        (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_f64_t          value);
typedef mama_status
(*msgPayload_updateString)     (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const char*         value);
typedef mama_status
(*msgPayload_updateOpaque)     (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const void*         value,
                                mama_size_t         size);
typedef mama_status
(*msgPayload_updateDateTime)   (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mamaDateTime  value);
typedef mama_status
(*msgPayload_updatePrice)      (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mamaPrice     value);
typedef mama_status
(*msgPayload_getBool)          (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_bool_t*        result);
typedef mama_status
(*msgPayload_updateSubMsg)     (msgPayload          msg,
                                const char*         fname,
                                mama_fid_t          fid,
                                const mamaMsg       subMsg);
typedef mama_status
(*msgPayload_updateVectorMsg)  (msgPayload          msg,
                                const char*         fname,
                                mama_fid_t          fid,
                                const mamaMsg       value[],
                                mama_size_t         size);
typedef mama_status
(*msgPayload_updateVectorString)(msgPayload         msg,
                                const char*         fname,
                                mama_fid_t          fid,
                                const char*         strList[],
                                mama_size_t         size);
typedef mama_status
(*msgPayload_updateVectorBool) (msgPayload          msg,
                                const char*         fname,
                                mama_fid_t          fid,
                                const mama_bool_t   boolList[],
                                mama_size_t         size);
typedef mama_status
(*msgPayload_updateVectorChar) (msgPayload          msg,
                                const char*         fname,
                                mama_fid_t          fid,
                                const char          charList[],
                                mama_size_t         size);
typedef mama_status
(*msgPayload_updateVectorI8)   (msgPayload          msg,
                                const char*         fname,
                                mama_fid_t          fid,
                                const mama_i8_t     i8List[],
                                mama_size_t         size);
typedef mama_status
(*msgPayload_updateVectorU8)   (msgPayload          msg,
                                const char*         fname,
                                mama_fid_t          fid,
                                const mama_u8_t     u8List[],
                                mama_size_t         size);
typedef mama_status
(*msgPayload_updateVectorI16)  (msgPayload          msg,
                                const char*         fname,
                                mama_fid_t          fid,
                                const mama_i16_t    i16List[],
                                mama_size_t         size);
typedef mama_status
(*msgPayload_updateVectorU16)  (msgPayload          msg,
                                const char*         fname,
                                mama_fid_t          fid,
                                const mama_u16_t    u16List[],
                                mama_size_t         size);
typedef mama_status
(*msgPayload_updateVectorI32)  (msgPayload          msg,
                                const char*         fname,
                                mama_fid_t          fid,
                                const mama_i32_t    i32List[],
                                mama_size_t         size);
typedef mama_status
(*msgPayload_updateVectorU32)  (msgPayload          msg,
                                const char*         fname,
                                mama_fid_t          fid,
                                const mama_u32_t    u32List[],
                                mama_size_t         size);
typedef mama_status
(*msgPayload_updateVectorI64)  (msgPayload          msg,
                                const char*         fname,
                                mama_fid_t          fid,
                                const mama_i64_t    i64List[],
                                mama_size_t         size);
typedef mama_status
(*msgPayload_updateVectorU64)  (msgPayload          msg,
                                const char*         fname,
                                mama_fid_t          fid,
                                const mama_u64_t    u64List[],
                                mama_size_t         size);
typedef mama_status
(*msgPayload_updateVectorF32)  (msgPayload          msg,
                                const char*         fname,
                                mama_fid_t          fid,
                                const mama_f32_t    f32List[],
                                mama_size_t         size);
typedef mama_status
(*msgPayload_updateVectorF64)  (msgPayload          msg,
                                const char*         fname,
                                mama_fid_t          fid,
                                const mama_f64_t    f64List[],
                                mama_size_t         size);
typedef mama_status
(*msgPayload_updateVectorPrice)(msgPayload          msg,
                                const char*         fname,
                                mama_fid_t          fid,
                                const mamaPrice     priceList[],
                                mama_size_t         size);
typedef mama_status
(*msgPayload_updateVectorTime) (msgPayload          msg,
                                const char*         fname,
                                mama_fid_t          fid,
                                const mamaDateTime  timeList[],
                                mama_size_t         size);
typedef mama_status
(*msgPayload_getChar)          (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                char*               result);
typedef mama_status
(*msgPayload_getI8)            (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i8_t*          result);
typedef mama_status
(*msgPayload_getU8)            (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u8_t*          result);
typedef mama_status
(*msgPayload_getI16)           (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i16_t*         result);
typedef mama_status
(*msgPayload_getU16)           (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u16_t*         result);
typedef mama_status
(*msgPayload_getI32)           (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i32_t*         result);
typedef mama_status
(*msgPayload_getU32)           (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u32_t*         result);
typedef mama_status
(*msgPayload_getI64)           (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i64_t*         result);
typedef mama_status
(*msgPayload_getU64)           (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u64_t*         result);
typedef mama_status
(*msgPayload_getF32)           (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_f32_t*         result);
typedef mama_status
(*msgPayload_getF64)           (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_f64_t*         result);
typedef mama_status
(*msgPayload_getString)        (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const char**        result);
typedef mama_status
(*msgPayload_getOpaque)        (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const void**        result,
                                mama_size_t*        size);
typedef mama_status
(*msgPayload_getField)         (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                msgFieldPayload*    result);
typedef mama_status
(*msgPayload_getDateTime)      (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mamaDateTime        result);
typedef mama_status
(*msgPayload_getPrice)         (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mamaPrice           result);
typedef mama_status
(*msgPayload_getMsg)           (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                msgPayload*         result);
typedef mama_status
(*msgPayload_getVectorBool)    (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_bool_t** result,
                                mama_size_t*        size);
typedef mama_status
(*msgPayload_getVectorChar)    (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const char**        result,
                                mama_size_t*        size);
typedef mama_status
(*msgPayload_getVectorI8)      (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_i8_t**   result,
                                mama_size_t*        size);
typedef mama_status
(*msgPayload_getVectorU8)      (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_u8_t**   result,
                                mama_size_t*        size);
typedef mama_status
(*msgPayload_getVectorI16)     (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_i16_t**  result,
                                mama_size_t*        size);
typedef mama_status
(*msgPayload_getVectorU16)     (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_u16_t**  result,
                                mama_size_t*        size);
typedef mama_status
(*msgPayload_getVectorI32)     (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_i32_t**  result,
                                mama_size_t*        size);
typedef mama_status
(*msgPayload_getVectorU32)     (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_u32_t**  result,
                                mama_size_t*        size);
typedef mama_status
(*msgPayload_getVectorI64)     (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_i64_t**  result,
                                mama_size_t*        size);
typedef mama_status
(*msgPayload_getVectorU64)     (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_u64_t**  result,
                                mama_size_t*        size);
typedef mama_status
(*msgPayload_getVectorF32)     (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_f32_t**  result,
                                mama_size_t*        size);
typedef mama_status
(*msgPayload_getVectorF64)     (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_f64_t**  result,
                                mama_size_t*        size);
typedef mama_status
(*msgPayload_getVectorString)  (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const char***       result,
                                mama_size_t*        size);
typedef mama_status
(*msgPayload_getVectorDateTime)(const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mamaDateTime** result,
                                mama_size_t*        size);
typedef mama_status
(*msgPayload_getVectorPrice)   (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mamaPrice**   result,
                                mama_size_t*        size);
typedef mama_status
(*msgPayload_getVectorMsg)     (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const msgPayload**  result,
                                mama_size_t*        size);
/*===================================================================
 =              msgFieldPayload bridge function pointers             =
 ====================================================================*/
typedef mama_status
(*msgFieldPayload_create)      (msgFieldPayload*        field);

typedef mama_status
(*msgFieldPayload_destroy)     (msgFieldPayload         field);

typedef mama_status
(*msgFieldPayload_getType)     (const msgFieldPayload   field,
                                mamaFieldType*          result);
typedef mama_status
(*msgFieldPayload_getName)     (msgFieldPayload         field,
                                mamaDictionary          dict,
                                mamaFieldDescriptor     desc,
                                const char**            result);
typedef mama_status
(*msgFieldPayload_getFid)      (const msgFieldPayload   field,
                                mamaDictionary          dict,
                                mamaFieldDescriptor     desc,
                                uint16_t*               result);
typedef mama_status
(*msgFieldPayload_getDescriptor)(const msgFieldPayload  field,
                                mamaDictionary          dict,
                                mamaFieldDescriptor*    result);
typedef mama_status
(*msgFieldPayload_updateBool)  (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_bool_t             value);
typedef mama_status
(*msgFieldPayload_updateChar)  (msgFieldPayload         field,
                                msgPayload              msg,
                                char                    value);
typedef mama_status
(*msgFieldPayload_updateU8)    (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_u8_t               value);
typedef mama_status
(*msgFieldPayload_updateI8)    (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_i8_t               value);
typedef mama_status
(*msgFieldPayload_updateI16)   (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_i16_t              value);
typedef mama_status
(*msgFieldPayload_updateU16)   (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_u16_t              value);
typedef mama_status
(*msgFieldPayload_updateI32)   (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_i32_t              value);
typedef mama_status
(*msgFieldPayload_updateU32)   (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_u32_t              value);
typedef mama_status
(*msgFieldPayload_updateI64)   (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_i64_t              value);
typedef mama_status
(*msgFieldPayload_updateU64)   (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_u64_t              value);
typedef mama_status
(*msgFieldPayload_updateF32)   (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_f32_t              value);
typedef mama_status
(*msgFieldPayload_updateF64)   (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_f64_t              value);
typedef mama_status
(*msgFieldPayload_updateString)(msgFieldPayload         field,
                                msgPayload              msg,
                                const char*             value);
typedef mama_status
(*msgFieldPayload_updateDateTime)
                               (msgFieldPayload         field,
                                msgPayload              msg,
                                const mamaDateTime      value);
typedef mama_status
(*msgFieldPayload_updatePrice) (msgFieldPayload         field,
                                msgPayload              msg,
                                const mamaPrice         value);
typedef mama_status
(*msgFieldPayload_updateSubMsg)(msgFieldPayload         field,
                                msgPayload              msg,
                                const msgPayload        subMsg);
typedef mama_status
(*msgFieldPayload_getBool)     (const msgFieldPayload   field,
                                mama_bool_t*            result);
typedef mama_status
(*msgFieldPayload_getChar)     (const msgFieldPayload   field,
                                char*                   result);
typedef mama_status
(*msgFieldPayload_getI8)       (const msgFieldPayload   field,
                                mama_i8_t*              result);
typedef mama_status
(*msgFieldPayload_getU8)       (const msgFieldPayload   field,
                                mama_u8_t*              result);
typedef mama_status
(*msgFieldPayload_getI16)      (const msgFieldPayload   field,
                                mama_i16_t*             result);
typedef mama_status
(*msgFieldPayload_getU16)      (const msgFieldPayload   field,
                                 mama_u16_t*            result);
typedef mama_status
(*msgFieldPayload_getI32)      (const msgFieldPayload   field,
                                mama_i32_t*             result);
typedef mama_status
(*msgFieldPayload_getU32)      (const msgFieldPayload   field,
                                mama_u32_t*             result);
typedef mama_status
(*msgFieldPayload_getI64)      (const msgFieldPayload   field,
                                mama_i64_t*             result);
typedef mama_status
(*msgFieldPayload_getU64)      (const msgFieldPayload   field,
                                mama_u64_t*             result);
typedef mama_status
(*msgFieldPayload_getF32)      (const msgFieldPayload   field,
                                mama_f32_t*             result);
typedef mama_status
(*msgFieldPayload_getF64)      (const msgFieldPayload   field,
                                mama_f64_t*             result);
typedef mama_status
(*msgFieldPayload_getString)   (const msgFieldPayload   field,
                                const char**            result);
typedef mama_status
(*msgFieldPayload_getOpaque)   (const msgFieldPayload   field,
                                const void**            result,
                                mama_size_t*            size);
typedef mama_status
(*msgFieldPayload_getDateTime) (const msgFieldPayload   field,
                                mamaDateTime            result);
typedef mama_status
(*msgFieldPayload_getPrice)    (const msgFieldPayload   field,
                                mamaPrice               result);
typedef mama_status
(*msgFieldPayload_getMsg)      (const msgFieldPayload   field,
                                msgPayload*             result);
typedef mama_status
(*msgFieldPayload_getVectorBool)
                               (const msgFieldPayload   field,
                                const mama_bool_t**     result,
                                mama_size_t*            size);
typedef mama_status
(*msgFieldPayload_getVectorChar)
                               (const msgFieldPayload   field,
                                const char**            result,
                                mama_size_t*            size);
typedef mama_status
(*msgFieldPayload_getVectorI8)
                               (const msgFieldPayload   field,
                                const mama_i8_t**       result,
                                mama_size_t*            size);
typedef mama_status
(*msgFieldPayload_getVectorU8) (const msgFieldPayload   field,
                                const mama_u8_t**       result,
                                mama_size_t*            size);
typedef mama_status
(*msgFieldPayload_getVectorI16)(const msgFieldPayload   field,
                                const mama_i16_t**      result,
                                mama_size_t*            size);
typedef mama_status
(*msgFieldPayload_getVectorU16)(const msgFieldPayload   field,
                                const mama_u16_t**      result,
                                mama_size_t*            size);
typedef mama_status
(*msgFieldPayload_getVectorI32)(const msgFieldPayload   field,
                                const mama_i32_t**      result,
                                mama_size_t*            size);
typedef mama_status
(*msgFieldPayload_getVectorU32)(const msgFieldPayload   field,
                                const mama_u32_t**      result,
                                mama_size_t*            size);
typedef mama_status
(*msgFieldPayload_getVectorI64)(const msgFieldPayload   field,
                                const mama_i64_t**      result,
                                mama_size_t*            size);
typedef mama_status
(*msgFieldPayload_getVectorU64)(const msgFieldPayload   field,
                                const mama_u64_t**      result,
                                mama_size_t*            size);
 typedef mama_status
(*msgFieldPayload_getVectorF32)(const msgFieldPayload   field,
                                const mama_f32_t**      result,
                                mama_size_t*            size);
typedef mama_status
(*msgFieldPayload_getVectorF64)(const msgFieldPayload   field,
                                const mama_f64_t**      result,
                                mama_size_t*            size);
typedef mama_status
(*msgFieldPayload_getVectorString)
                               (const msgFieldPayload   field,
                                const char***           result,
                                mama_size_t*            size);
typedef mama_status
(*msgFieldPayload_getVectorDateTime)
                               (const msgFieldPayload   field,
                                const mamaDateTime*     result,
                                mama_size_t*            size);
typedef mama_status
(*msgFieldPayload_getVectorPrice)
                               (const msgFieldPayload   field,
                                const mamaPrice*        result,
                                mama_size_t*            size);
typedef mama_status
(*msgFieldPayload_getVectorMsg)(const msgFieldPayload   field,
                                const msgPayload**      result,
                                mama_size_t*            size);

typedef mama_status
(*msgFieldPayload_getAsString) (const msgFieldPayload   field,
                                const msgPayload        msg,
                                char*                   buffer,
                                mama_size_t             len);

/*===================================================================
 =              msgPayloadIter bridge function pointers             =
 ====================================================================*/
typedef mama_status
(*msgPayloadIter_create)       (msgPayloadIter*         iter,
                                msgPayload              msg);
typedef msgFieldPayload
(*msgPayloadIter_next)         (msgPayloadIter          iter,
                                msgFieldPayload         field,
                                msgPayload              msg);
typedef mama_bool_t
(*msgPayloadIter_hasNext)      (msgPayloadIter          iter,
                                msgPayload              msg);
typedef msgFieldPayload
(*msgPayloadIter_begin)        (msgPayloadIter          iter,
                                msgFieldPayload         field,
                                msgPayload              msg);
typedef msgFieldPayload
(*msgPayloadIter_end)          (msgPayloadIter          iter,
                                msgPayload              msg);
typedef mama_status
(*msgPayloadIter_associate)    (msgPayloadIter          iter,
                                msgPayload              msg);
typedef mama_status
(*msgPayloadIter_destroy)      (msgPayloadIter          iter);

typedef struct mamaPayloadBridgeImpl_
{
    /*Associate arbitrary data. Used for the wfast state */
    void*     mClosure;

    msgPayload_create                   msgPayloadCreate;
    msgPayload_createForTemplate        msgPayloadCreateForTemplate;
    msgPayload_getType                  msgPayloadGetType;
    msgPayload_copy                     msgPayloadCopy;
    msgPayload_clear                    msgPayloadClear;
    msgPayload_destroy                  msgPayloadDestroy;
    msgPayload_setParent                msgPayloadSetParent;
    msgPayload_getByteSize              msgPayloadGetByteSize;
    msgPayload_getNumFields             msgPayloadGetNumFields;
    msgPayload_getSendSubject           msgPayloadGetSendSubject;
    msgPayload_toString                 msgPayloadToString;
    msgPayload_iterateFields            msgPayloadIterateFields;
    msgPayload_serialize                msgPayloadSerialize;
    msgPayload_deserialize              msgPayloadDeserialize;
    msgPayload_unSerialize              msgPayloadUnSerialize;
    msgPayload_getByteBuffer            msgPayloadGetByteBuffer;
    msgPayload_setByteBuffer            msgPayloadSetByteBuffer;
    msgPayload_createFromByteBuffer     msgPayloadCreateFromByteBuffer;
    msgPayload_apply                    msgPayloadApply;
    msgPayload_getNativeMsg             msgPayloadGetNativeMsg;
    msgPayload_getFieldAsString         msgPayloadGetFieldAsString;
    msgPayload_addBool                  msgPayloadAddBool;
    msgPayload_addChar                  msgPayloadAddChar;
    msgPayload_addI8                    msgPayloadAddI8;
    msgPayload_addU8                    msgPayloadAddU8;
    msgPayload_addI16                   msgPayloadAddI16;
    msgPayload_addU16                   msgPayloadAddU16;
    msgPayload_addI32                   msgPayloadAddI32;
    msgPayload_addU32                   msgPayloadAddU32;
    msgPayload_addI64                   msgPayloadAddI64;
    msgPayload_addU64                   msgPayloadAddU64;
    msgPayload_addF32                   msgPayloadAddF32;
    msgPayload_addF64                   msgPayloadAddF64;
    msgPayload_addString                msgPayloadAddString;
    msgPayload_addOpaque                msgPayloadAddOpaque;
    msgPayload_addDateTime              msgPayloadAddDateTime;
    msgPayload_addPrice                 msgPayloadAddPrice;
    msgPayload_addMsg                   msgPayloadAddMsg;
    msgPayload_addVectorBool            msgPayloadAddVectorBool;
    msgPayload_addVectorChar            msgPayloadAddVectorChar;
    msgPayload_addVectorI8              msgPayloadAddVectorI8;
    msgPayload_addVectorU8              msgPayloadAddVectorU8;
    msgPayload_addVectorI16             msgPayloadAddVectorI16;
    msgPayload_addVectorU16             msgPayloadAddVectorU16;
    msgPayload_addVectorI32             msgPayloadAddVectorI32;
    msgPayload_addVectorU32             msgPayloadAddVectorU32;
    msgPayload_addVectorI64             msgPayloadAddVectorI64;
    msgPayload_addVectorU64             msgPayloadAddVectorU64;
    msgPayload_addVectorF32             msgPayloadAddVectorF32;
    msgPayload_addVectorF64             msgPayloadAddVectorF64;
    msgPayload_addVectorString          msgPayloadAddVectorString;
    msgPayload_addVectorMsg             msgPayloadAddVectorMsg;
    msgPayload_addVectorDateTime        msgPayloadAddVectorDateTime;
    msgPayload_addVectorPrice           msgPayloadAddVectorPrice;
    msgPayload_updateBool               msgPayloadUpdateBool;
    msgPayload_updateChar               msgPayloadUpdateChar;
    msgPayload_updateU8                 msgPayloadUpdateU8;
    msgPayload_updateI8                 msgPayloadUpdateI8;
    msgPayload_updateI16                msgPayloadUpdateI16;
    msgPayload_updateU16                msgPayloadUpdateU16;
    msgPayload_updateI32                msgPayloadUpdateI32;
    msgPayload_updateU32                msgPayloadUpdateU32;
    msgPayload_updateI64                msgPayloadUpdateI64;
    msgPayload_updateU64                msgPayloadUpdateU64;
    msgPayload_updateF32                msgPayloadUpdateF32;
    msgPayload_updateF64                msgPayloadUpdateF64;
    msgPayload_updateString             msgPayloadUpdateString;
    msgPayload_updateOpaque             msgPayloadUpdateOpaque;
    msgPayload_updateDateTime           msgPayloadUpdateDateTime;
    msgPayload_updatePrice              msgPayloadUpdatePrice;
    msgPayload_updateSubMsg             msgPayloadUpdateSubMsg;
    msgPayload_updateVectorMsg          msgPayloadUpdateVectorMsg;
    msgPayload_updateVectorString       msgPayloadUpdateVectorString;
    msgPayload_updateVectorBool         msgPayloadUpdateVectorBool;
    msgPayload_updateVectorChar         msgPayloadUpdateVectorChar;
    msgPayload_updateVectorI8           msgPayloadUpdateVectorI8;
    msgPayload_updateVectorU8           msgPayloadUpdateVectorU8;
    msgPayload_updateVectorI16          msgPayloadUpdateVectorI16;
    msgPayload_updateVectorU16          msgPayloadUpdateVectorU16;
    msgPayload_updateVectorI32          msgPayloadUpdateVectorI32;
    msgPayload_updateVectorU32          msgPayloadUpdateVectorU32;
    msgPayload_updateVectorI64          msgPayloadUpdateVectorI64;
    msgPayload_updateVectorU64          msgPayloadUpdateVectorU64;
    msgPayload_updateVectorF32          msgPayloadUpdateVectorF32;
    msgPayload_updateVectorF64          msgPayloadUpdateVectorF64;
    msgPayload_updateVectorPrice        msgPayloadUpdateVectorPrice;
    msgPayload_updateVectorTime         msgPayloadUpdateVectorTime;
    msgPayload_getBool                  msgPayloadGetBool;
    msgPayload_getChar                  msgPayloadGetChar;
    msgPayload_getI8                    msgPayloadGetI8;
    msgPayload_getU8                    msgPayloadGetU8;
    msgPayload_getI16                   msgPayloadGetI16;
    msgPayload_getU16                   msgPayloadGetU16;
    msgPayload_getI32                   msgPayloadGetI32;
    msgPayload_getU32                   msgPayloadGetU32;
    msgPayload_getI64                   msgPayloadGetI64;
    msgPayload_getU64                   msgPayloadGetU64;
    msgPayload_getF32                   msgPayloadGetF32;
    msgPayload_getF64                   msgPayloadGetF64;
    msgPayload_getString                msgPayloadGetString;
    msgPayload_getOpaque                msgPayloadGetOpaque;
    msgPayload_getField                 msgPayloadGetField;
    msgPayload_getDateTime              msgPayloadGetDateTime;
    msgPayload_getPrice                 msgPayloadGetPrice;
    msgPayload_getMsg                   msgPayloadGetMsg;
    msgPayload_getVectorBool            msgPayloadGetVectorBool;
    msgPayload_getVectorChar            msgPayloadGetVectorChar;
    msgPayload_getVectorI8              msgPayloadGetVectorI8;
    msgPayload_getVectorU8              msgPayloadGetVectorU8;
    msgPayload_getVectorI16             msgPayloadGetVectorI16;
    msgPayload_getVectorU16             msgPayloadGetVectorU16;
    msgPayload_getVectorI32             msgPayloadGetVectorI32;
    msgPayload_getVectorU32             msgPayloadGetVectorU32;
    msgPayload_getVectorI64             msgPayloadGetVectorI64;
    msgPayload_getVectorU64             msgPayloadGetVectorU64;
    msgPayload_getVectorF32             msgPayloadGetVectorF32;
    msgPayload_getVectorF64             msgPayloadGetVectorF64;
    msgPayload_getVectorString          msgPayloadGetVectorString;
    msgPayload_getVectorDateTime        msgPayloadGetVectorDateTime;
    msgPayload_getVectorPrice           msgPayloadGetVectorPrice;
    msgPayload_getVectorMsg             msgPayloadGetVectorMsg;
    msgFieldPayload_create              msgFieldPayloadCreate;
    msgFieldPayload_destroy             msgFieldPayloadDestroy;
    msgFieldPayload_getName             msgFieldPayloadGetName;
    msgFieldPayload_getFid              msgFieldPayloadGetFid;
    msgFieldPayload_getDescriptor       msgFieldPayloadGetDescriptor;
    msgFieldPayload_getType             msgFieldPayloadGetType;
    msgFieldPayload_updateBool          msgFieldPayloadUpdateBool;
    msgFieldPayload_updateChar          msgFieldPayloadUpdateChar;
    msgFieldPayload_updateU8            msgFieldPayloadUpdateU8;
    msgFieldPayload_updateI8            msgFieldPayloadUpdateI8;
    msgFieldPayload_updateI16           msgFieldPayloadUpdateI16;
    msgFieldPayload_updateU16           msgFieldPayloadUpdateU16;
    msgFieldPayload_updateI32           msgFieldPayloadUpdateI32;
    msgFieldPayload_updateU32           msgFieldPayloadUpdateU32;
    msgFieldPayload_updateI64           msgFieldPayloadUpdateI64;
    msgFieldPayload_updateU64           msgFieldPayloadUpdateU64;
    msgFieldPayload_updateF32           msgFieldPayloadUpdateF32;
    msgFieldPayload_updateF64           msgFieldPayloadUpdateF64;
    msgFieldPayload_updateString        msgFieldPayloadUpdateString;
    msgFieldPayload_updateDateTime      msgFieldPayloadUpdateDateTime;
    msgFieldPayload_updatePrice         msgFieldPayloadUpdatePrice;
    msgFieldPayload_getBool             msgFieldPayloadGetBool;
    msgFieldPayload_getChar             msgFieldPayloadGetChar;
    msgFieldPayload_getI8               msgFieldPayloadGetI8;
    msgFieldPayload_getU8               msgFieldPayloadGetU8;
    msgFieldPayload_getI16              msgFieldPayloadGetI16;
    msgFieldPayload_getU16              msgFieldPayloadGetU16;
    msgFieldPayload_getI32              msgFieldPayloadGetI32;
    msgFieldPayload_getU32              msgFieldPayloadGetU32;
    msgFieldPayload_getI64              msgFieldPayloadGetI64;
    msgFieldPayload_getU64              msgFieldPayloadGetU64;
    msgFieldPayload_getF32              msgFieldPayloadGetF32;
    msgFieldPayload_getF64              msgFieldPayloadGetF64;
    msgFieldPayload_getString           msgFieldPayloadGetString;
    msgFieldPayload_getOpaque           msgFieldPayloadGetOpaque;
    msgFieldPayload_getDateTime         msgFieldPayloadGetDateTime;
    msgFieldPayload_getPrice            msgFieldPayloadGetPrice;
    msgFieldPayload_getMsg              msgFieldPayloadGetMsg;
    msgFieldPayload_getVectorBool       msgFieldPayloadGetVectorBool;
    msgFieldPayload_getVectorChar       msgFieldPayloadGetVectorChar;
    msgFieldPayload_getVectorI8         msgFieldPayloadGetVectorI8;
    msgFieldPayload_getVectorU8         msgFieldPayloadGetVectorU8;
    msgFieldPayload_getVectorI16        msgFieldPayloadGetVectorI16;
    msgFieldPayload_getVectorU16        msgFieldPayloadGetVectorU16;
    msgFieldPayload_getVectorI32        msgFieldPayloadGetVectorI32;
    msgFieldPayload_getVectorU32        msgFieldPayloadGetVectorU32;
    msgFieldPayload_getVectorI64        msgFieldPayloadGetVectorI64;
    msgFieldPayload_getVectorU64        msgFieldPayloadGetVectorU64;
    msgFieldPayload_getVectorF32        msgFieldPayloadGetVectorF32;
    msgFieldPayload_getVectorF64        msgFieldPayloadGetVectorF64;
    msgFieldPayload_getVectorString     msgFieldPayloadGetVectorString;
    msgFieldPayload_getVectorDateTime   msgFieldPayloadGetVectorDateTime;
    msgFieldPayload_getVectorPrice      msgFieldPayloadGetVectorPrice;
    msgFieldPayload_getVectorMsg        msgFieldPayloadGetVectorMsg;
    msgFieldPayload_getAsString         msgFieldPayloadGetAsString;

    msgPayloadIter_create               msgPayloadIterCreate;
    msgPayloadIter_next                 msgPayloadIterNext;
    msgPayloadIter_hasNext              msgPayloadIterHasNext;
    msgPayloadIter_begin                msgPayloadIterBegin;
    msgPayloadIter_end                  msgPayloadIterEnd;
    msgPayloadIter_associate            msgPayloadIterAssociate;
    msgPayloadIter_destroy              msgPayloadIterDestroy;

    void*                               closure;

    /* Back reference to parent library itself */
    mamaPayloadLib*                     payloadLib;

} mamaPayloadBridgeImpl;


#if defined (__cplusplus)
}
#endif

#endif /* mamaPayloadBridgeH__ */
