/* $Id: msgfield.c,v 1.1.2.2 2011/08/30 15:54:05 billtorpey Exp $
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

#include "mama/mama.h"
#include "msgfield.h"
#include <string.h>


#include "avismsgimpl.h"


#define avisField         ((bridgeAvisMamaMsgFieldImpl*)(msgField))
#define CHECK_FIELD(msgField) \
        do {  \
           if (avisField == 0) return MAMA_STATUS_NULL_ARG; \
         } while(0)

#define avisPayload       ((avisFieldPayload*) ((bridgeAvisMamaMsgFieldImpl*)(msgField))->mAvisField)
#define CHECK_PAYLOAD(msgField) \
        do {  \
           if (avisField == 0) return MAMA_STATUS_NULL_ARG; \
           if (avisPayload == 0) return MAMA_STATUS_NULL_ARG; \
         } while(0)

#define avisMsg           ((Attributes*) ((bridgeAvisMamaMsgFieldImpl*)(msgField))->mAvisMsg)
#define CHECK_MSG(msgField) \
        do {  \
           if (avisField == 0) return MAMA_STATUS_NULL_ARG; \
           if (avisPayload == 0) return MAMA_STATUS_NULL_ARG; \
           if (avisMsg == 0) return MAMA_STATUS_NULL_ARG; \
         } while(0)


/******************************************************************************
* general field functions
*******************************************************************************/

mama_status mamaMsgField_create (
    mamaMsgField*  field)
{
    bridgeAvisMamaMsgFieldImpl* impl =
        malloc (sizeof(bridgeAvisMamaMsgFieldImpl));
    *field = (mamaMsgField) impl;

    if (impl == NULL)
    {
        return MAMA_STATUS_NOMEM;
    }

    impl->mAvisField  = NULL;
    impl->mAvisMsg    = NULL;
    impl->mFieldDesc   = NULL;
    impl->mDictionary  = NULL;
    return MAMA_STATUS_OK;
}

mama_status mamaMsgField_destroy (
    mamaMsgField         msgField)
{
    CHECK_FIELD(field);
    free(avisPayload);
    free (avisField);
    return MAMA_STATUS_OK;
}

mama_status
mamaMsgField_getDescriptor(
    const mamaMsgField    msgField,
    mamaFieldDescriptor*  result)
{
	mama_status status = MAMA_STATUS_OK;
    CHECK_FIELD(msgField);

    /* The FD may have already been obtained for this field from
     * getName, getFid etc.*/
    if (avisField->mFieldDesc)
    {
        *result = avisField->mFieldDesc;
        return MAMA_STATUS_OK;
    }

    CHECK_PAYLOAD(msgField);
    if (avisField->mDictionary != NULL)
    {
        if (avisPayload->mName != NULL)
        {
            status = mamaDictionary_getFieldDescriptorByName
                (avisField->mDictionary,
                 &avisField->mFieldDesc,
                 avisPayload->mName);
        }
        else
        {
            status = MAMA_STATUS_INVALID_ARG;
        }

        if (status == MAMA_STATUS_OK)
        {
            *result = avisField->mFieldDesc;
        }
    }

    return status;
}

mama_status
mamaMsgField_getFid(
    const mamaMsgField  msgField,
    mama_fid_t*         result)
{
    CHECK_FIELD(field);

    *result = 0;
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
mamaMsgField_getName(
    const mamaMsgField  msgField,
    const char**        result)
{
    CHECK_PAYLOAD(msgField);

    *result = avisPayload->mName;
    if ((avisPayload->mName == NULL) || (strlen(avisPayload->mName) == 0))
       return MAMA_STATUS_INVALID_ARG;
    return MAMA_STATUS_OK;
}

mama_status
mamaMsgField_getType(
    const mamaMsgField  msgField,
    mamaFieldType*      result)
{
    CHECK_PAYLOAD(msgField);

    *result = avis2MamaType(avisPayload->mValue->type);
    if (*result == MAMA_FIELD_TYPE_UNKNOWN)
       return MAMA_STATUS_INVALID_ARG;
    return MAMA_STATUS_OK;
}

mama_status
mamaMsgField_getBool (
    mamaMsgField  msgField,
    mama_bool_t*  result)
{
    CHECK_PAYLOAD(msgField);
    return avisValue_getBool(avisPayload->mValue, result);
}

mama_status
mamaMsgField_getChar (
    mamaMsgField  msgField,
    char*         result)
{
    CHECK_PAYLOAD(msgField);
    return avisValue_getChar(avisPayload->mValue, result);
}

mama_status
mamaMsgField_getI8(
    mamaMsgField  msgField,
    int8_t*       result)
{
    CHECK_PAYLOAD(msgField);
    return avisValue_getI8(avisPayload->mValue, result);
}

mama_status
mamaMsgField_getU8(
    mamaMsgField  msgField,
    uint8_t*      result)
{
    CHECK_PAYLOAD(msgField);
    return avisValue_getU8(avisPayload->mValue, result);
}

mama_status
mamaMsgField_getI16(
    mamaMsgField  msgField,
    int16_t*      result)
{
    CHECK_PAYLOAD(msgField);
    return avisValue_getI16(avisPayload->mValue, result);
}

mama_status
mamaMsgField_getU16(
    mamaMsgField  msgField,
    uint16_t*     result)
{
    CHECK_PAYLOAD(msgField);
    return avisValue_getU16(avisPayload->mValue, result);
}

mama_status
mamaMsgField_getI32(
    mamaMsgField  msgField,
    int32_t*      result)
{
    CHECK_PAYLOAD(msgField);
    return avisValue_getI32(avisPayload->mValue, result);
}

mama_status
mamaMsgField_getU32(
    mamaMsgField  msgField,
    uint32_t*     result)
{
    CHECK_PAYLOAD(msgField);
    return avisValue_getU32(avisPayload->mValue, result);
}

mama_status
mamaMsgField_getI64(
    mamaMsgField  msgField,
    int64_t*      result)
{
    CHECK_PAYLOAD(msgField);
    return avisValue_getI64(avisPayload->mValue, result);
}

mama_status
mamaMsgField_getU64(
    mamaMsgField  msgField,
    uint64_t*     result)
{
    CHECK_PAYLOAD(msgField);
    return avisValue_getU64(avisPayload->mValue, result);
}

mama_status
mamaMsgField_getF32(
        mamaMsgField  msgField,
        mama_f32_t*   result)
{
    CHECK_PAYLOAD(msgField);
    return avisValue_getF32(avisPayload->mValue, result);
}

mama_status
mamaMsgField_getF64(
        mamaMsgField  msgField,
        mama_f64_t*   result)
{
    CHECK_PAYLOAD(msgField);
    return avisValue_getF64(avisPayload->mValue, result);
}

mama_status
mamaMsgField_getString(
    mamaMsgField  msgField,
    const char**  result)
{
    CHECK_PAYLOAD(msgField);
    return avisValue_getString(avisPayload->mValue, result);
}

mama_status
mamaMsgField_getOpaque(
        mamaMsgField  msgField,
        const void**  result,
        size_t*       size)
{
    CHECK_PAYLOAD(msgField);
    return avisValue_getOpaque(avisPayload->mValue, result, size);
}

mama_status
mamaMsgField_getDateTime(
            mamaMsgField  msgField,
                mamaDateTime  result)
{
    CHECK_PAYLOAD(msgField);
    return avisValue_getDateTime(avisPayload->mValue, result);
}

mama_status
mamaMsgField_getPrice(
    mamaMsgField  msgField,
    mamaPrice     result)
{
    CHECK_PAYLOAD(msgField);
    return avisValue_getPrice(avisPayload->mValue, result);
}

mama_status
mamaMsgField_getMsg (
    const mamaMsgField msgField,
    mamaMsg*           result)
{
    CHECK_PAYLOAD(msgField);
    return avisValue_getMsg(avisPayload->mValue, result);
}

mama_status
mamaMsgField_getVectorChar (
    mamaMsgField     msgField,
    const char**     result,
    size_t*          size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
mamaMsgField_getVectorI8 (
    mamaMsgField     msgField,
    const int8_t**   result,
    size_t*          size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
mamaMsgField_getVectorU8 (
    mamaMsgField     msgField,
    const uint8_t**  result,
    size_t*          size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
mamaMsgField_getVectorI16 (
    mamaMsgField     msgField,
    const int16_t**  result,
    size_t*          size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
mamaMsgField_getVectorU16 (
    mamaMsgField     msgField,
    const uint16_t** result,
    size_t*          size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
mamaMsgField_getVectorI32 (
    mamaMsgField     msgField,
    const int32_t**  result,
    size_t*          size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
mamaMsgField_getVectorU32 (
    mamaMsgField     msgField,
    const uint32_t** result,
    size_t*          size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
mamaMsgField_getVectorI64 (
    mamaMsgField     msgField,
    const int64_t**  result,
    size_t*          size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
mamaMsgField_getVectorU64 (
    mamaMsgField     msgField,
    const uint64_t** result,
    size_t*          size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
mamaMsgField_getVectorF32 (
    mamaMsgField        msgField,
    const mama_f32_t**  result,
    size_t*             size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
mamaMsgField_getVectorF64 (
    mamaMsgField        msgField,
    const mama_f64_t**  result,
    size_t*             size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
mamaMsgField_getVectorString (
    mamaMsgField   msgField,
    const char***  result,
    size_t*        size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
mamaMsgField_getVectorMsg (
    const mamaMsgField     msgField,
    const mamaMsg**        result,
    mama_size_t*          size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
mamaMsgField_getAsString (
    mamaMsgField  msgField,
    char *        buf,
    size_t        len)
{
    CHECK_PAYLOAD(msgField);
    return avisValue_getFieldAsString(avisPayload->mValue, avisPayload->mName, 0, buf,len);
}

mama_status
mamaMsgField_updateBool (
    const mamaMsgField  msgField,
    mama_bool_t         value)
{
    CHECK_MSG(msgField);
    return avisMsg_setBool(avisMsg, avisPayload->mName, 0, value);
}

mama_status
mamaMsgField_updateChar (
    const mamaMsgField  msgField,
    char                value)
{
    CHECK_MSG(msgField);
    return avisMsg_setChar(avisMsg, avisPayload->mName, 0, value);
}

mama_status
mamaMsgField_updateI8 (
    const mamaMsgField  msgField,
    mama_i8_t           value)
{
    CHECK_MSG(msgField);
    return avisMsg_setI8(avisMsg, avisPayload->mName, 0, value);
}

mama_status
mamaMsgField_updateU8 (
    const mamaMsgField  msgField,
    mama_u8_t           value)
{
    CHECK_MSG(msgField);
    return avisMsg_setU8(avisMsg, avisPayload->mName, 0, value);
}

mama_status
mamaMsgField_updateI16 (
    const mamaMsgField  msgField,
    mama_i16_t          value)
{
    CHECK_MSG(msgField);
    return avisMsg_setI16(avisMsg, avisPayload->mName, 0, value);
}

mama_status
mamaMsgField_updateU16 (
    const mamaMsgField  msgField,
    mama_u16_t          value)
{
    CHECK_MSG(msgField);
    return avisMsg_setU16(avisMsg, avisPayload->mName, 0, value);
}

mama_status
mamaMsgField_updateI32 (
    const mamaMsgField  msgField,
    mama_i32_t          value)
{
    CHECK_MSG(msgField);
    return avisMsg_setI32(avisMsg, avisPayload->mName, 0, value);
}

mama_status
mamaMsgField_updateU32 (
    const mamaMsgField  msgField,
    mama_u32_t          value)
{
    CHECK_MSG(msgField);
    return avisMsg_setU32(avisMsg, avisPayload->mName, 0, value);
}

mama_status
mamaMsgField_updateI64 (
    const mamaMsgField  msgField,
    mama_i64_t          value)
{
    CHECK_MSG(msgField);
    return avisMsg_setI64(avisMsg, avisPayload->mName, 0, value);
}

mama_status
mamaMsgField_updateU64 (
    const mamaMsgField  msgField,
    mama_u64_t          value)
{
    CHECK_MSG(msgField);
    return avisMsg_setU64(avisMsg, avisPayload->mName, 0, value);
}

mama_status
mamaMsgField_updateF32 (
            const mamaMsgField  msgField,
            mama_f32_t          value)
{
    CHECK_MSG(msgField);
    return avisMsg_setF32(avisMsg, avisPayload->mName, 0, value);
}

mama_status
mamaMsgField_updateF64 (
            const mamaMsgField  msgField,
            mama_f64_t          value)
{
    CHECK_MSG(msgField);
    return avisMsg_setF64(avisMsg, avisPayload->mName, 0, value);
}

mama_status
mamaMsgField_updateDateTime (
            const mamaMsgField  msgField,
            const mamaDateTime  value)
{
    CHECK_MSG(msgField);
    return avisMsg_setDateTime(avisMsg, avisPayload->mName, 0, value);
}

mama_status
mamaMsgField_updatePrice (
            const mamaMsgField  msgField,
            const mamaPrice     value)
{
    CHECK_MSG(msgField);
    return avisMsg_setPrice(avisMsg, avisPayload->mName, 0, value);
}

#if 0
void bridgeAvisMamaMsgField_setAvisField (
    mamaMsgField         msgField,
    avis_notification_t notification,
    mamaFieldDescriptor  desc)
{
    bridgeAvisMamaMsgFieldImpl* impl =
        (bridgeAvisMamaMsgFieldImpl*)(msgField);
    impl->myNotification = notification;
    impl->myFieldDesc    = desc;
    impl->myDictionary   = NULL; /*We will have the descriptor from the iteration*/
}
#endif
