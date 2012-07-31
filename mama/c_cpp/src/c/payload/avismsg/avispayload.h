/* $Id: avispayload.h,v 1.1.2.4 2011/09/15 17:33:26 ianbell Exp $
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

#ifndef AVIS_MSGBRIDGE_H__
#define AVIS_MSGBRIDGE_H__

#include <mama/types.h>
#include "payloadbridge.h"

#if defined(__cplusplus)
extern "C" {
#endif


MAMAExpBridgeDLL
extern mama_status
avismsgPayload_destroyImpl       (mamaPayloadBridge mamaPayloadBridge);

MAMAExpBridgeDLL
extern mama_status
avismsgPayload_createImpl        (mamaPayloadBridge* result, char* identifier);

extern mamaPayloadType
avismsgPayload_getType           (void);

extern mama_status
avismsgPayload_create            (msgPayload*         msg);

extern mama_status
avismsgPayload_createForTemplate (msgPayload*         msg,
                                mamaPayloadBridge       bridge,
                                mama_u32_t          templateId);
extern mama_status
avismsgPayload_copy              (const msgPayload    msg,
                                msgPayload*         copy);
extern mama_status
avismsgPayload_clear             (msgPayload          msg);
extern mama_status
avismsgPayload_destroy           (msgPayload          msg);
extern mama_status
avismsgPayload_setParent         (msgPayload          msg,
                               const mamaMsg       parent);
extern mama_status
avismsgPayload_getByteSize       (const msgPayload    msg,
                                mama_size_t*        size);

extern mama_status
avismsgPayload_getNumFields      (const msgPayload    msg,
                                mama_size_t*        numFields);

extern mama_status
avismsgPayload_getSendSubject      (const msgPayload    msg,
                                const char **        subject);

extern const char*
avismsgPayload_toString          (const msgPayload    msg);
extern mama_status
avismsgPayload_iterateFields     (const msgPayload    msg,
                                const mamaMsg       parent,
                                mamaMsgField        field,
                                mamaMsgIteratorCb   cb,
                                void*               closure);
extern mama_status
avismsgPayload_getFieldAsString  (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                char*               buffer,
                                mama_size_t         len);

extern mama_status
avismsgPayload_serialize (const msgPayload    payload,
                           const void**        buffer,
                           mama_size_t*        bufferLength);

extern mama_status
avismsgPayload_unSerialize (const msgPayload    payload,
                           const void*        buffer,
                           mama_size_t        bufferLength);


extern mama_status
avismsgPayload_getByteBuffer     (const msgPayload    msg,
                                const void**        buffer,
                                mama_size_t*        bufferLength);
extern mama_status
avismsgPayload_setByteBuffer     (const msgPayload    msg,
                                mamaPayloadBridge       bridge,
                                const void*         buffer,
                                mama_size_t         bufferLength);

extern mama_status
avismsgPayload_createFromByteBuffer (msgPayload*          msg,
                                   mamaPayloadBridge        bridge,
                                   const void*          buffer,
                                   mama_size_t          bufferLength);
extern mama_status
avismsgPayload_apply             (msgPayload          dest,
                                const msgPayload    src);
extern mama_status
avismsgPayload_getNativeMsg     (const msgPayload    msg,
                               void**              nativeMsg);
extern mama_status
avismsgPayload_addBool           (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_bool_t         value);
extern mama_status
avismsgPayload_addChar           (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                char                value);
extern mama_status
avismsgPayload_addI8             (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i8_t           value);
extern mama_status
avismsgPayload_addU8             (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u8_t           value);
extern mama_status
avismsgPayload_addI16            (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i16_t          value);
extern mama_status
avismsgPayload_addU16            (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u16_t          value);
extern mama_status
avismsgPayload_addI32            (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i32_t          value);
extern mama_status
avismsgPayload_addU32            (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u32_t          value);
extern mama_status
avismsgPayload_addI64            (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i64_t          value);
extern mama_status
avismsgPayload_addU64            (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u64_t          value);
extern mama_status
avismsgPayload_addF32            (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_f32_t          value);
extern mama_status
avismsgPayload_addF64            (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_f64_t          value);
extern mama_status
avismsgPayload_addString         (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const char*         value);
extern mama_status
avismsgPayload_addOpaque         (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const void*         value,
                                mama_size_t         size);
extern mama_status
avismsgPayload_addDateTime       (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mamaDateTime  value);
extern mama_status
avismsgPayload_addPrice          (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mamaPrice     value);
extern mama_status
avismsgPayload_addMsg            (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                msgPayload          value);
extern mama_status
avismsgPayload_addVectorBool     (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_bool_t   value[],
                                mama_size_t         size);
extern mama_status
avismsgPayload_addVectorChar     (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const char          value[],
                                mama_size_t         size);
extern mama_status
avismsgPayload_addVectorI8       (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_i8_t     value[],
                                mama_size_t         size);
extern mama_status
avismsgPayload_addVectorU8       (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_u8_t     value[],
                                mama_size_t         size);
extern mama_status
avismsgPayload_addVectorI16      (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_i16_t    value[],
                                mama_size_t         size);
extern mama_status
avismsgPayload_addVectorU16      (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_u16_t    value[],
                                mama_size_t         size);
extern mama_status
avismsgPayload_addVectorI32      (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_i32_t    value[],
                                mama_size_t         size);
extern mama_status
avismsgPayload_addVectorU32      (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_u32_t    value[],
                                mama_size_t         size);
extern mama_status
avismsgPayload_addVectorI64      (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_i64_t    value[],
                                mama_size_t         size);
extern mama_status
avismsgPayload_addVectorU64      (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_u64_t    value[],
                                mama_size_t         size);
extern mama_status
avismsgPayload_addVectorF32      (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_f32_t    value[],
                                mama_size_t         size);
extern mama_status
avismsgPayload_addVectorF64      (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_f64_t    value[],
                                mama_size_t         size);
extern mama_status
avismsgPayload_addVectorString   (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const char*         value[],
                                mama_size_t         size);
extern mama_status
avismsgPayload_addVectorMsg      (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mamaMsg       value[],
                                mama_size_t         size);
extern mama_status
avismsgPayload_addVectorDateTime (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mamaDateTime  value[],
                                mama_size_t         size);
extern mama_status
avismsgPayload_addVectorPrice    (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mamaPrice     value[],
                                mama_size_t         size);
extern mama_status
avismsgPayload_updateBool        (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_bool_t         value);
extern mama_status
avismsgPayload_updateChar        (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                char                value);
extern mama_status
avismsgPayload_updateU8          (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u8_t           value);
extern mama_status
avismsgPayload_updateI8          (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i8_t           value);
extern mama_status
avismsgPayload_updateI16         (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i16_t          value);
extern mama_status
avismsgPayload_updateU16         (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u16_t          value);
extern mama_status
avismsgPayload_updateI32         (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i32_t          value);
extern mama_status
avismsgPayload_updateU32         (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u32_t          value);
extern mama_status
avismsgPayload_updateI64         (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i64_t          value);
extern mama_status
avismsgPayload_updateU64         (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u64_t          value);
extern mama_status
avismsgPayload_updateF32         (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_f32_t          value);
extern mama_status
avismsgPayload_updateF64
                               (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_f64_t          value);
extern mama_status
avismsgPayload_updateString      (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const char*         value);
extern mama_status
avismsgPayload_updateOpaque      (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const void*         value,
                                mama_size_t         size);
extern mama_status
avismsgPayload_updateDateTime    (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mamaDateTime  value);
extern mama_status
avismsgPayload_updatePrice       (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mamaPrice     value);
extern mama_status
avismsgPayload_getBool           (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_bool_t*        result);
extern mama_status
avismsgPayload_updateSubMsg      (msgPayload          msg,
                                const char*         fname,
                                mama_fid_t          fid,
                                const msgPayload    subMsg);
extern mama_status
avismsgPayload_updateVectorMsg   (msgPayload          msg,
                                const char*         fname,
                                mama_fid_t          fid,
                                const mamaMsg       value[],
                                mama_size_t         size);
extern mama_status
avismsgPayload_updateVectorString (msgPayload         msg,
                                const char*         fname,
                                mama_fid_t          fid,
                                const char*         value[],
                                mama_size_t         size);
extern mama_status
avismsgPayload_updateVectorBool  (msgPayload          msg,
                                const char*         fname,
                                mama_fid_t          fid,
                                const mama_bool_t   value[],
                                mama_size_t         size);
extern mama_status
avismsgPayload_updateVectorChar  (msgPayload          msg,
                                const char*         fname,
                                mama_fid_t          fid,
                                const char          value[],
                                mama_size_t         size);
extern mama_status
avismsgPayload_updateVectorI8    (msgPayload          msg,
                                const char*         fname,
                                mama_fid_t          fid,
                                const mama_i8_t     value[],
                                mama_size_t         size);
extern mama_status
avismsgPayload_updateVectorU8    (msgPayload          msg,
                                const char*         fname,
                                mama_fid_t          fid,
                                const mama_u8_t     value[],
                                mama_size_t         size);
extern mama_status
avismsgPayload_updateVectorI16   (msgPayload          msg,
                                const char*         fname,
                                mama_fid_t          fid,
                                const mama_i16_t    value[],
                                mama_size_t         size);
extern mama_status
avismsgPayload_updateVectorU16   (msgPayload          msg,
                                const char*         fname,
                                mama_fid_t          fid,
                                const mama_u16_t    value[],
                                mama_size_t         size);
extern mama_status
avismsgPayload_updateVectorI32   (msgPayload          msg,
                                const char*         fname,
                                mama_fid_t          fid,
                                const mama_i32_t    value[],
                                mama_size_t         size);
extern mama_status
avismsgPayload_updateVectorU32   (msgPayload          msg,
                                const char*         fname,
                                mama_fid_t          fid,
                                const mama_u32_t    value[],
                                mama_size_t         size);
extern mama_status
avismsgPayload_updateVectorI64   (msgPayload          msg,
                                const char*         fname,
                                mama_fid_t          fid,
                                const mama_i64_t    value[],
                                mama_size_t         size);
extern mama_status
avismsgPayload_updateVectorU64   (msgPayload          msg,
                                const char*         fname,
                                mama_fid_t          fid,
                                const mama_u64_t    value[],
                                mama_size_t         size);
extern mama_status
avismsgPayload_updateVectorF32   (msgPayload          msg,
                                const char*         fname,
                                mama_fid_t          fid,
                                const mama_f32_t    value[],
                                mama_size_t         size);
extern mama_status
avismsgPayload_updateVectorF64   (msgPayload          msg,
                                const char*         fname,
                                mama_fid_t          fid,
                                const mama_f64_t    value[],
                                mama_size_t         size);
extern mama_status
avismsgPayload_updateVectorPrice (msgPayload          msg,
                                const char*         fname,
                                mama_fid_t          fid,
                                const mamaPrice     value[],
                                mama_size_t         size);
extern mama_status
avismsgPayload_updateVectorTime  (msgPayload          msg,
                                const char*         fname,
                                mama_fid_t          fid,
                                const mamaDateTime  value[],
                                mama_size_t         size);
extern mama_status
avismsgPayload_getChar           (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                char*               result);
extern mama_status
avismsgPayload_getI8             (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i8_t*          result);
extern mama_status
avismsgPayload_getU8             (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u8_t*          result);
extern mama_status
avismsgPayload_getI16            (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i16_t*         result);
extern mama_status
avismsgPayload_getU16            (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u16_t*         result);
extern mama_status
avismsgPayload_getI32            (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i32_t*         result);
extern mama_status
avismsgPayload_getU32            (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u32_t*         result);
extern mama_status
avismsgPayload_getI64            (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i64_t*         result);
extern mama_status
avismsgPayload_getU64            (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u64_t*         result);
extern mama_status
avismsgPayload_getF32            (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_f32_t*         result);
extern mama_status
avismsgPayload_getF64            (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_f64_t*         result);
extern mama_status
avismsgPayload_getString         (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const char**        result);
extern mama_status
avismsgPayload_getOpaque         (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const void**        result,
                                mama_size_t*        size);
extern mama_status
avismsgPayload_getField          (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                msgFieldPayload*    result);
extern mama_status
avismsgPayload_getDateTime       (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mamaDateTime        result);
extern mama_status
avismsgPayload_getPrice          (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mamaPrice           result);
extern mama_status
avismsgPayload_getMsg            (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                msgPayload*         result);
extern mama_status
avismsgPayload_getVectorBool     (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_bool_t** result,
                                mama_size_t*        size);
extern mama_status
avismsgPayload_getVectorChar     (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const char**        result,
                                mama_size_t*        size);
extern mama_status
avismsgPayload_getVectorI8       (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_i8_t**   result,
                                mama_size_t*        size);
extern mama_status
avismsgPayload_getVectorU8       (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_u8_t**   result,
                                mama_size_t*        size);
extern mama_status
avismsgPayload_getVectorI16      (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_i16_t**  result,
                                mama_size_t*        size);
extern mama_status
avismsgPayload_getVectorU16      (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_u16_t**  result,
                                mama_size_t*        size);
extern mama_status
avismsgPayload_getVectorI32      (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_i32_t**  result,
                                mama_size_t*        size);
extern mama_status
avismsgPayload_getVectorU32      (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_u32_t**  result,
                                mama_size_t*        size);
extern mama_status
avismsgPayload_getVectorI64      (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_i64_t**  result,
                                mama_size_t*        size);
extern mama_status
avismsgPayload_getVectorU64      (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_u64_t**  result,
                                mama_size_t*        size);
extern mama_status
avismsgPayload_getVectorF32      (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_f32_t**  result,
                                mama_size_t*        size);
extern mama_status
avismsgPayload_getVectorF64      (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_f64_t**  result,
                                mama_size_t*        size);
extern mama_status
avismsgPayload_getVectorString   (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const char***       result,
                                mama_size_t*        size);
extern mama_status
avismsgPayload_getVectorDateTime (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mamaDateTime* result,
                                mama_size_t*        size);
extern mama_status
avismsgPayload_getVectorPrice    (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mamaPrice*    result,
                                mama_size_t*        size);
extern mama_status
avismsgPayload_getVectorMsg      (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const msgPayload**  result,
                                mama_size_t*        size);
extern mama_status
avismsgPayloadIter_create        (msgPayloadIter*     iter,
                                msgPayload          msg);
extern msgFieldPayload
avismsgPayloadIter_next          (msgPayloadIter      iter,
                                msgFieldPayload     field,
                                msgPayload          msg);
extern mama_bool_t
avismsgPayloadIter_hasNext       (msgPayloadIter      iter,
                                msgPayload          msg);
extern msgFieldPayload
avismsgPayloadIter_begin         (msgPayloadIter      iter,
                                msgFieldPayload     field,
                                msgPayload          msg);
extern msgFieldPayload
avismsgPayloadIter_end           (msgPayloadIter      iter,
                                msgPayload          msg);
extern mama_status
avismsgPayloadIter_associate     (msgPayloadIter      iter,
                                msgPayload          msg);
extern mama_status
avismsgPayloadIter_destroy       (msgPayloadIter      iter);

extern mama_status
avismsgFieldPayload_create       (msgFieldPayload*    field);

extern mama_status
avismsgFieldPayload_destroy      (msgFieldPayload     field);

extern mama_status
avismsgFieldPayload_getName      (const msgFieldPayload   field,
                                mamaDictionary          dict,
                                mamaFieldDescriptor     desc,
                                const char**            result);
extern mama_status
avismsgFieldPayload_getFid       (const msgFieldPayload   field,
                                mamaDictionary          dict,
                                mamaFieldDescriptor     desc,
                                uint16_t*               result);

extern mama_status
avismsgFieldPayload_getDescriptor(const msgFieldPayload   field,
                                mamaDictionary          dict,
                                mamaFieldDescriptor*    result);
extern mama_status
avismsgFieldPayload_getType      (const msgFieldPayload   field,
                                mamaFieldType*          result);
extern mama_status
avismsgFieldPayload_updateBool   (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_bool_t             value);
extern mama_status
avismsgFieldPayload_updateChar   (msgFieldPayload         field,
                                msgPayload              msg,
                                char                    value);
extern mama_status
avismsgFieldPayload_updateU8     (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_u8_t               value);
extern mama_status
avismsgFieldPayload_updateI8     (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_i8_t               value);
extern mama_status
avismsgFieldPayload_updateI16    (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_i16_t              value);
extern mama_status
avismsgFieldPayload_updateU16    (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_u16_t              value);
extern mama_status
avismsgFieldPayload_updateI32    (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_i32_t              value);
extern mama_status
avismsgFieldPayload_updateU32    (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_u32_t              value);
extern mama_status
avismsgFieldPayload_updateI64    (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_i64_t              value);
extern mama_status
avismsgFieldPayload_updateU64    (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_u64_t              value);
extern mama_status
avismsgFieldPayload_updateF32    (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_f32_t              value);
extern mama_status
avismsgFieldPayload_updateF64    (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_f64_t              value);
extern mama_status
avismsgFieldPayload_updateDateTime
                               (msgFieldPayload         field,
                                msgPayload              msg,
                                const mamaDateTime      value);
extern mama_status
avismsgFieldPayload_updatePrice  (msgFieldPayload         field,
                                msgPayload              msg,
                                const mamaPrice         value);
extern mama_status
avismsgFieldPayload_getBool      (const msgFieldPayload   field,
                                mama_bool_t*            result);
extern mama_status
avismsgFieldPayload_getChar      (const msgFieldPayload   field,
                                char*                   result);
extern mama_status
avismsgFieldPayload_getI8        (const msgFieldPayload   field,
                                mama_i8_t*              result);
extern mama_status
avismsgFieldPayload_getU8        (const msgFieldPayload   field,
                                mama_u8_t*              result);
extern mama_status
avismsgFieldPayload_getI16       (const msgFieldPayload   field,
                                mama_i16_t*             result);
extern mama_status
avismsgFieldPayload_getU16       (const msgFieldPayload   field,
                                mama_u16_t*             result);
extern mama_status
avismsgFieldPayload_getI32       (const msgFieldPayload   field,
                                mama_i32_t*             result);
extern mama_status
avismsgFieldPayload_getU32       (const msgFieldPayload   field,
                                mama_u32_t*             result);
extern mama_status
avismsgFieldPayload_getI64       (const msgFieldPayload   field,
                                mama_i64_t*             result);
extern mama_status
avismsgFieldPayload_getU64       (const msgFieldPayload   field,
                                mama_u64_t*             result);
extern mama_status
avismsgFieldPayload_getF32       (const msgFieldPayload   field,
                                mama_f32_t*             result);
extern mama_status
avismsgFieldPayload_getF64       (const msgFieldPayload   field,
                                mama_f64_t*             result);
extern mama_status
avismsgFieldPayload_getString    (const msgFieldPayload   field,
                                const char**            result);
extern mama_status
avismsgFieldPayload_getOpaque    (const msgFieldPayload   field,
                                const void**            result,
                                mama_size_t*            size);
extern mama_status
avismsgFieldPayload_getField     (const msgFieldPayload   field,
                                mamaMsgField*           result);
extern mama_status
avismsgFieldPayload_getDateTime  (const msgFieldPayload   field,
                                mamaDateTime            result);
extern mama_status
avismsgFieldPayload_getPrice     (const msgFieldPayload   field,
                                mamaPrice               result);
extern mama_status
avismsgFieldPayload_getMsg       (const msgFieldPayload   field,
                                msgPayload*             result);
extern mama_status
avismsgFieldPayload_getVectorBool
                               (const msgFieldPayload   field,
                                const mama_bool_t**     result,
                                mama_size_t*            size);
extern mama_status
avismsgFieldPayload_getVectorChar
                               (const msgFieldPayload   field,
                                const char**            result,
                                mama_size_t*            size);
extern mama_status
avismsgFieldPayload_getVectorI8
                               (const msgFieldPayload   field,
                                const mama_i8_t**       result,
                                mama_size_t*            size);
extern mama_status
avismsgFieldPayload_getVectorU8  (const msgFieldPayload   field,
                                const mama_u8_t**       result,
                                mama_size_t*            size);
extern mama_status
avismsgFieldPayload_getVectorI16 (const msgFieldPayload   field,
                                const mama_i16_t**      result,
                                mama_size_t*            size);
extern mama_status
avismsgFieldPayload_getVectorU16 (const msgFieldPayload   field,
                                const mama_u16_t**      result,
                                mama_size_t*            size);
extern mama_status
avismsgFieldPayload_getVectorI32 (const msgFieldPayload   field,
                                const mama_i32_t**      result,
                                mama_size_t*            size);
extern mama_status
avismsgFieldPayload_getVectorU32 (const msgFieldPayload   field,
                                const mama_u32_t**      result,
                                mama_size_t*            size);
extern mama_status
avismsgFieldPayload_getVectorI64 (const msgFieldPayload   field,
                                const mama_i64_t**      result,
                                mama_size_t*            size);
extern mama_status
avismsgFieldPayload_getVectorU64 (const msgFieldPayload   field,
                                const mama_u64_t**      result,
                                mama_size_t*            size);
 extern mama_status
avismsgFieldPayload_getVectorF32 (const msgFieldPayload   field,
                                const mama_f32_t**      result,
                                mama_size_t*            size);
extern mama_status
avismsgFieldPayload_getVectorF64 (const msgFieldPayload   field,
                                const mama_f64_t**      result,
                                mama_size_t*            size);
extern mama_status
avismsgFieldPayload_getVectorString
                               (const msgFieldPayload   field,
                                const char***           result,
                                mama_size_t*            size);
extern mama_status
avismsgFieldPayload_getVectorDateTime
                               (const msgFieldPayload   field,
                                const mamaDateTime*     result,
                                mama_size_t*            size);
extern mama_status
avismsgFieldPayload_getVectorPrice
                               (const msgFieldPayload   field,
                                const mamaPrice*        result,
                                mama_size_t*            size);
extern mama_status
avismsgFieldPayload_getVectorMsg (const msgFieldPayload   field,
                                const msgPayload**      result,
                                mama_size_t*            size);

extern mama_status
avismsgFieldPayload_getAsString (
    const msgFieldPayload   field,
	const msgPayload   msg,
    char*         buf,
    mama_size_t   len);

#if defined(__cplusplus)
}
#endif
#endif /* AVIS_MSG_IMPL_H__*/
