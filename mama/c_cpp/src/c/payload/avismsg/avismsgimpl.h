/* $Id: avismsgimpl.h,v 1.1.2.8 2011/09/15 17:33:25 ianbell Exp $
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

#ifndef AVISMSGIMPL_H
#define AVISMSGIMPL_H

#include "bridge.h"
#include "payloadbridge.h"
#include "avis/attributes.h"

#if defined(__cplusplus)
extern "C" {
#endif
typedef struct avisPayload
{
    Attributes*                 mAvisMsg;
    struct avisFieldPayload*    mAvisField;
    mamaMsg                     mParent;

    void * 						mBuffer;
    uint16_t					mBufferLen;

    char * 						mStringBuffer;
    uint16_t					mStringBufferLen;

    struct avisIterator*        mIterator;
} avisPayloadImpl;

typedef struct avisFieldPayload
{
    const char*           mName;
    const Value*          mValue;
} avisFieldPayload;

typedef struct avisIterator
{
    AttributesIter*     mMsgIterator;
    Attributes*         mAvisMsg;
    avisFieldPayload*   mAvisField;
} avisIterator;


short
avis2MamaType( ValueType valueType );

mama_status
avisMsg_setBool(
        Attributes*     attributes,
        const char*     name,
        mama_fid_t      fid,
        mama_bool_t     value);


mama_status
avisMsg_setChar(
        Attributes*     attributes,
        const char*     name,
        mama_fid_t      fid,
        char            value);

mama_status
avisMsg_setI8(
        Attributes*     attributes,
        const char*     name,
        mama_fid_t      fid,
        int8_t          value);

mama_status
avisMsg_setU8(
        Attributes*     attributes,
        const char*     name,
        mama_fid_t      fid,
        uint8_t         value);

mama_status
avisMsg_setI16(
        Attributes*     attributes,
        const char*     name,
        mama_fid_t      fid,
        int16_t         value);

mama_status
avisMsg_setU16(
        Attributes*     attributes,
        const char*     name,
        mama_fid_t      fid,
        uint16_t        value);

mama_status
avisMsg_setI32(
    Attributes*         attributes,
    const char*         name,
    mama_fid_t          fid,
    int32_t             value);

mama_status
avisMsg_setU32(
        Attributes*     attributes,
        const char*     name,
        mama_fid_t      fid,
        uint32_t        value);

mama_status
avisMsg_setI64(
    Attributes*         attributes,
    const char*         name,
    mama_fid_t          fid,
    int64_t             value);

mama_status
avisMsg_setU64(
        Attributes*     attributes,
        const char*     name,
        mama_fid_t      fid,
        uint64_t        value);

mama_status
avisMsg_setF32(
   Attributes*     attributes,
   const char*     name,
   mama_fid_t      fid,
   mama_f32_t      value);

mama_status
avisMsg_setF64(
    Attributes*  attributes,
    const char*  name,
    mama_fid_t   fid,
    mama_f64_t   value);

mama_status
avisMsg_setString(
    Attributes*  attributes,
    const char*  name,
    mama_fid_t   fid,
    const char*  value);

mama_status
avisMsg_setOpaque(
        Attributes*  attributes,
        const char*  name,
        mama_fid_t   fid,
        const void*  value,
        size_t       size);

mama_status
avisMsg_setVectorBool(
    Attributes*  attributes,
    const char*        name,
    mama_fid_t         fid,
    const mama_bool_t  value[],
    size_t             numElements);

mama_status
avisMsg_setVectorChar (
    Attributes*  attributes,
    const char*        name,
    mama_fid_t         fid,
    const char         value[],
    size_t             numElements);

mama_status
avisMsg_setVectorI8 (
        Attributes*  attributes,
        const char*        name,
        mama_fid_t         fid,
        const int8_t       value[],
        size_t             numElements);

mama_status
avisMsg_setVectorU8 (
        Attributes*        attributes,
        const char*        name,
        mama_fid_t         fid,
        const uint8_t      value[],
        size_t             numElements);

mama_status
avisMsg_setVectorI16 (
        Attributes*        attributes,
        const char*        name,
        mama_fid_t         fid,
        const int16_t      value[],
        size_t             numElements);

mama_status
avisMsg_setVectorU16 (
        Attributes*        attributes,
        const char*        name,
        mama_fid_t         fid,
        const uint16_t     value[],
        size_t             numElements);

mama_status
avisMsg_setVectorI32 (
        Attributes*        attributes,
        const char*        name,
        mama_fid_t         fid,
        const int32_t      value[],
        size_t             numElements);


mama_status
avisMsg_setVectorU32 (
        Attributes*        attributes,
        const char*        name,
        mama_fid_t         fid,
        const uint32_t     value[],
        size_t             numElements);


mama_status
avisMsg_setVectorI64 (
        Attributes*        attributes,
        const char*        name,
        mama_fid_t         fid,
        const int64_t      value[],
        size_t             numElements);


mama_status
avisMsg_setVectorU64 (
        Attributes*        attributes,
        const char*        name,
        mama_fid_t         fid,
        const uint64_t     value[],
        size_t             numElements);


mama_status
avisMsg_setVectorF32 (
        Attributes*        attributes,
        const char*        name,
        mama_fid_t         fid,
        const mama_f32_t   value[],
        size_t             numElements);


mama_status
avisMsg_setVectorF64 (
        Attributes*        attributes,
        const char*        name,
        mama_fid_t         fid,
        const mama_f64_t   value[],
        size_t             numElements);


mama_status
avisMsg_setVectorString (
        Attributes*        attributes,
        const char*        name,
        mama_fid_t         fid,
        const char*        value[],
        size_t             numElements);


mama_status
avisMsg_setVectorDateTime (
    Attributes*  attributes,
    const char*         name,
    mama_fid_t          fid,
    const mamaDateTime  value[],
    size_t              numElements);

mama_status
avisMsg_setMsg(
        Attributes*        attributes,
        const char*        name,
        mama_fid_t         fid,
        const msgPayload   value);


mama_status
avisMsg_setVectorMsg(
        Attributes*        attributes,
        const char*        name,
        mama_fid_t         fid,
        const mamaMsg      value[],
        size_t             numElements);


mama_status
avisMsg_getBool(
        Attributes*   attributes,
        const char*   name,
        mama_fid_t    fid,
        mama_bool_t*  result);

mama_status
avisMsg_getChar(
        Attributes*   attributes,
        const char*   name,
        mama_fid_t    fid,
        char*         result);

mama_status
avisMsg_getI8(
        Attributes*   attributes,
        const char*   name,
        mama_fid_t    fid,
        int8_t*       result);

mama_status
avisMsg_getU8(
        Attributes*   attributes,
        const char*   name,
        mama_fid_t    fid,
        uint8_t*      result);

mama_status
avisMsg_getI16(
        Attributes*     attributes,
        const char*     name,
        mama_fid_t      fid,
        int16_t*        result);


mama_status
avisMsg_getU16(
        Attributes*     attributes,
        const char*     name,
        mama_fid_t      fid,
        uint16_t*       result);

mama_status
avisMsg_getI32(
    Attributes*  attributes,
    const char*  name,
    mama_fid_t   fid,
    int32_t*     result);

mama_status
avisMsg_getU32(
        Attributes*    attributes,
        const char*    name,
        mama_fid_t     fid,
        uint32_t*      result);


mama_status
avisMsg_getI64(
    Attributes*  attributes,
    const char*  name,
    mama_fid_t   fid,
    int64_t*     result);


mama_status
avisMsg_getU64(
        Attributes*    attributes,
        const char*    name,
        mama_fid_t     fid,
        uint64_t*      result);

mama_status
avisMsg_getF32(
        Attributes*    attributes,
        const char*    name,
        mama_fid_t     fid,
        mama_f32_t*    result);


mama_status
avisMsg_getF64(
    Attributes*  attributes,
    const char*  name,
    mama_fid_t   fid,
    mama_f64_t*  result);


mama_status
avisMsg_getString(
    Attributes*  attributes,
    const char*  name,
    mama_fid_t   fid,
    const char** result);


mama_status
avisMsg_getOpaque(
        Attributes*  attributes,
        const char*    name,
        mama_fid_t     fid,
        const void**   result,
        size_t*        size);


mama_status
avisMsg_getDateTime(
        Attributes*    attributes,
        const char*    name,
        mama_fid_t     fid,
        mamaDateTime   result);


mama_status
avisMsg_setDateTime(
        Attributes*         attributes,
        const char*         name,
        mama_fid_t          fid,
        const mamaDateTime  value);

mama_status
avisMsg_getVectorDateTime (
        Attributes*           attributes,
        const char*           name,
        mama_fid_t            fid,
        const mamaDateTime*   result,
        size_t*               resultLen);

mama_status
avisMsg_getVectorPrice (
        Attributes*           attributes,
        const char*           name,
        mama_fid_t            fid,
        const mamaPrice*      result,
        size_t*               resultLen);

mama_status
avisMsg_setPrice(
    Attributes*         attributes,
    const char*         name,
    mama_fid_t          fid,
    const mamaPrice     value);

mama_status
avisMsg_setVectorPrice (
    Attributes*  attributes,
    const char*         name,
    mama_fid_t          fid,
    const mamaPrice     value[],
    size_t              numElements);

mama_status
avisMsg_getPrice(
    Attributes*    attributes,
    const char*    name,
    mama_fid_t     fid,
    mamaPrice      result);

mama_status
avisMsg_getMsg (
    Attributes*      attributes,
    const char*      name,
    mama_fid_t       fid,
    msgPayload*      result);

mama_status
avisMsg_getVectorBool (
        Attributes*          attributes,
        const char*          name,
        mama_fid_t           fid,
        const mama_bool_t**  result,
        size_t*              resultLen);


mama_status
avisMsg_getVectorChar (
        Attributes*          attributes,
        const char*          name,
        mama_fid_t           fid,
        const char**         result,
        size_t*              resultLen);


mama_status
avisMsg_getVectorI8 (
        Attributes*          attributes,
        const char*          name,
        mama_fid_t           fid,
        const int8_t**       result,
        size_t*              resultLen);


mama_status
avisMsg_getVectorU8 (
        Attributes*          attributes,
        const char*          name,
        mama_fid_t           fid,
        const uint8_t**      result,
        size_t*              resultLen);


mama_status
avisMsg_getVectorI16 (
        Attributes*          attributes,
        const char*          name,
        mama_fid_t           fid,
        const int16_t**      result,
        size_t*              resultLen);


mama_status
avisMsg_getVectorU16 (
        Attributes*          attributes,
        const char*          name,
        mama_fid_t           fid,
        const uint16_t**     result,
        size_t*              resultLen);


mama_status
avisMsg_getVectorI32 (
        Attributes*          attributes,
        const char*          name,
        mama_fid_t           fid,
        const int32_t**      result,
        size_t*              resultLen);


mama_status
avisMsg_getVectorU32 (
        Attributes*          attributes,
        const char*          name,
        mama_fid_t           fid,
        const uint32_t**     result,
        size_t*              resultLen);


mama_status
avisMsg_getVectorI64 (
        Attributes*          attributes,
        const char*          name,
        mama_fid_t           fid,
        const int64_t**      result,
        size_t*              resultLen);


mama_status
avisMsg_getVectorU64 (
        Attributes*          attributes,
        const char*          name,
        mama_fid_t           fid,
        const uint64_t**     result,
        size_t*              resultLen);


mama_status
avisMsg_getVectorF32 (
        Attributes*          attributes,
        const char*          name,
        mama_fid_t           fid,
        const mama_f32_t**   result,
        size_t*              resultLen);


mama_status
avisMsg_getVectorF64 (
        Attributes*          attributes,
        const char*          name,
        mama_fid_t           fid,
        const mama_f64_t**   result,
        size_t*              resultLen);


mama_status
avisMsg_getVectorString (
        Attributes*    attributes,
        const char*    name,
        mama_fid_t     fid,
        const char***  result,
        size_t*        resultLen);


mama_status
avisMsg_getVectorMsg (
    Attributes*         attributes,
    const char*         name,
    mama_fid_t          fid,
    const msgPayload**  result,
    size_t*             resultLen);

mama_status
avisMsg_getFieldAsString(
    Attributes*  attributes,
    const char*  name,
    mama_fid_t   fid,
    char*        buf,
    size_t       len);



mama_status avisValue_getBool(const Value* pValue, mama_bool_t* result);
mama_status avisValue_getChar(const Value* pValue, char* result);
mama_status avisValue_getI8(const Value* pValue, int8_t* result);
mama_status avisValue_getU8(const Value* pValue, uint8_t* result);
mama_status avisValue_getI16(const Value* pValue, int16_t* result);
mama_status avisValue_getU16(const Value* pValue, uint16_t* result);
mama_status avisValue_getI32(const Value* pValue, int32_t* result);
mama_status avisValue_getU32(const Value* pValue, uint32_t* result);
mama_status avisValue_getI64(const Value* pValue, int64_t* result);
mama_status avisValue_getU64(const Value* pValue, uint64_t* result);
mama_status avisValue_getF32(const Value* pValue, mama_f32_t* result);
mama_status avisValue_getF64(const Value* pValue, mama_f64_t* result);
mama_status avisValue_getString(const Value* pValue, const char** result);
mama_status avisValue_getOpaque(const Value* pValue, const void** result, size_t* size);
mama_status avisValue_getDateTime(const Value* pValue, mamaDateTime result);
mama_status avisValue_getPrice(const Value* pValue, mamaPrice result);
mama_status avisValue_getMsg(const Value* pValue, mamaMsg* result);

mama_status avisValue_getFieldAsString(const Value* pValue, const char* name, mama_fid_t fid, char* buf, size_t len);

#if defined(__cplusplus)
}
#endif


#endif
