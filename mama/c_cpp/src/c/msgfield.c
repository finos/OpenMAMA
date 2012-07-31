/* $Id: msgfield.c,v 1.18.4.1.14.4 2011/08/18 10:54:08 ianbell Exp $
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

#include <mama/mama.h>
#include "msgimpl.h"
#include "msgfieldimpl.h"
#include <string.h>

static mama_status
mamaMsgField_destroyLastVectorMsg (mamaMsgFieldImpl *impl)
{
    if (impl != NULL)
    {
        int i = 0;
        for (i = 0; i < impl->myLastVectorMsgLen; i++)
            mamaMsg_destroy (impl->myLastVectorMsg[i]);

        free (impl->myLastVectorMsg);
        impl->myLastVectorMsg    = NULL;
        impl->myLastVectorMsgLen = 0;

        for (i = 0; i < impl->myLastVectorPayloadMsgLen; i++)
            mamaMsg_destroy (impl->myLastVectorPayloadMsg[i]);

        free (impl->myLastVectorPayloadMsg);
        impl->myLastVectorPayloadMsg    = NULL;
        impl->myLastVectorPayloadMsgLen = 0;
    }
    return MAMA_STATUS_OK;
}

mama_status mamaMsgField_create (
    mamaMsgField*  field)
{
    mamaMsgFieldImpl* impl =
        malloc (sizeof(mamaMsgFieldImpl));
    *field = (mamaMsgField) impl;

    if (impl == NULL)
    {
        return MAMA_STATUS_NOMEM;
    }

    impl->myFieldDesc               = NULL;
    impl->myLastVectorMsg           = NULL;
    impl->myLastVectorMsgLen        = 0;
    impl->myLastVectorPayloadMsg    = NULL;
    impl->myLastVectorPayloadMsgLen = 0;
    impl->myPayload                 = NULL;
    impl->myPayloadBridge           = NULL;
    impl->mySubMsg                  = NULL;

    return MAMA_STATUS_OK;
}

mama_status mamaMsgField_destroy (
    mamaMsgField         field)
{
    mamaMsgFieldImpl* impl = (mamaMsgFieldImpl*)(field);
    mamaMsgField_destroyLastVectorMsg (impl);

    if (impl->mySubMsg != NULL)
    {
        mamaMsg_destroy (impl->mySubMsg);
        impl->mySubMsg = NULL;
    }

    free (impl);
    return MAMA_STATUS_OK;
}

mama_status
mamaMsgField_getDescriptor(
    const mamaMsgField    msgField,
    mamaFieldDescriptor*  result)
{
    mamaMsgFieldImpl* impl = (mamaMsgFieldImpl*)(msgField);
    if (!impl) return MAMA_STATUS_INVALID_ARG;


    if (impl->myFieldDesc)
    {
        *result = impl->myFieldDesc;
        return MAMA_STATUS_OK;
    }


    if (impl->myPayloadBridge)
    {
        mama_status status = MAMA_STATUS_OK;
        status = impl->myPayloadBridge->msgFieldPayloadGetDescriptor(
                                    impl->myPayload,
                                    impl->myDictionary,
                                    &impl->myFieldDesc);
        if (MAMA_STATUS_OK == status)
        {
            *result = impl->myFieldDesc;
        }
        return status;

    }

    return MAMA_STATUS_INVALID_ARG;
}

mama_status
mamaMsgField_getFid(
    const mamaMsgField  msgField,
    uint16_t*           result)
{
    mamaMsgFieldImpl* impl = (mamaMsgFieldImpl*)(msgField);
    if (!impl) return MAMA_STATUS_INVALID_ARG;

    if (impl->myPayloadBridge)
    {
        return impl->myPayloadBridge->msgFieldPayloadGetFid(
                                    impl->myPayload,
                                    impl->myDictionary,
                                    impl->myFieldDesc,
                                    result);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsgField_getName(
    const mamaMsgField  msgField,
    const char**        result)
{
    mamaMsgFieldImpl* impl = (mamaMsgFieldImpl*)(msgField);

    if (!impl) return MAMA_STATUS_INVALID_ARG;

    if (impl->myPayloadBridge)
    {
        return impl->myPayloadBridge->msgFieldPayloadGetName(
                                    impl->myPayload,
                                    impl->myDictionary,
                                    impl->myFieldDesc,
                                    result);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsgField_getType(
    const mamaMsgField    msgField,
    mamaFieldType*        result)
{


    mamaMsgFieldImpl* impl = (mamaMsgFieldImpl*)(msgField);
    if (!impl) return MAMA_STATUS_INVALID_ARG;

    if (impl->myPayloadBridge)
    {
        return impl->myPayloadBridge->msgFieldPayloadGetType (
                                    impl->myPayload, result);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsgField_getBool (
    const mamaMsgField  msgField,
    mama_bool_t*        result)
{
    mamaMsgFieldImpl* impl =
        (mamaMsgFieldImpl*)(msgField);

    if (!impl) return MAMA_STATUS_INVALID_ARG;

    if (impl->myPayloadBridge)
    {
        return impl->myPayloadBridge->msgFieldPayloadGetBool (
                                    impl->myPayload, result);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsgField_getChar (
    const mamaMsgField  msgField,
    char*               result)
{
    mamaMsgFieldImpl* impl =
        (mamaMsgFieldImpl*)(msgField);
    if (!impl) return MAMA_STATUS_INVALID_ARG;

    if (impl->myPayloadBridge)
    {
        return impl->myPayloadBridge->msgFieldPayloadGetChar (
                                    impl->myPayload, result);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsgField_getI8(
            const mamaMsgField  msgField,
            int8_t*             result)
{
    mamaMsgFieldImpl* impl =
        (mamaMsgFieldImpl*)(msgField);
    if (!impl) return MAMA_STATUS_INVALID_ARG;

    if (impl->myPayloadBridge)
    {
        return impl->myPayloadBridge->msgFieldPayloadGetI8 (
                                    impl->myPayload, result);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsgField_getU8(
            const mamaMsgField  msgField,
            uint8_t*            result)
{
    mamaMsgFieldImpl* impl =
        (mamaMsgFieldImpl*)(msgField);
    if (!impl) return MAMA_STATUS_INVALID_ARG;

    if (impl->myPayloadBridge)
    {
        return impl->myPayloadBridge->msgFieldPayloadGetU8 (
                                    impl->myPayload, result);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsgField_getI16(
            const mamaMsgField  msgField,
            int16_t*            result)
{
    mamaMsgFieldImpl* impl =
        (mamaMsgFieldImpl*)(msgField);
    if (!impl) return MAMA_STATUS_INVALID_ARG;

    if (impl->myPayloadBridge)
    {
        return impl->myPayloadBridge->msgFieldPayloadGetI16 (
                                    impl->myPayload, result);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsgField_getU16(
            const mamaMsgField  msgField,
            uint16_t*           result)
{
    mamaMsgFieldImpl* impl =
        (mamaMsgFieldImpl*)(msgField);
    if (!impl) return MAMA_STATUS_INVALID_ARG;

    if (impl->myPayloadBridge)
    {
        return impl->myPayloadBridge->msgFieldPayloadGetU16 (
                                    impl->myPayload, result);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsgField_getI32(
    const mamaMsgField  msgField,
    int32_t*            result)
{
    mamaMsgFieldImpl* impl =
        (mamaMsgFieldImpl*)(msgField);
    if (!impl) return MAMA_STATUS_INVALID_ARG;

    if (impl->myPayloadBridge)
    {
        return impl->myPayloadBridge->msgFieldPayloadGetI32 (
                                    impl->myPayload, result);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsgField_getU32(
            const mamaMsgField  msgField,
            uint32_t*           result)
{
    mamaMsgFieldImpl* impl =
        (mamaMsgFieldImpl*)(msgField);
    if (!impl) return MAMA_STATUS_INVALID_ARG;

    if (impl->myPayloadBridge)
    {
        return impl->myPayloadBridge->msgFieldPayloadGetU32 (
                                    impl->myPayload, result);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsgField_getI64(
    const mamaMsgField  msgField,
    int64_t*            result)
{
    mamaMsgFieldImpl* impl =
        (mamaMsgFieldImpl*)(msgField);
    if (!impl) return MAMA_STATUS_INVALID_ARG;

    if (impl->myPayloadBridge)
    {
        return impl->myPayloadBridge->msgFieldPayloadGetI64 (
                                    impl->myPayload, result);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsgField_getU64(
        const mamaMsgField  msgField,
        uint64_t*           result)
{
    mamaMsgFieldImpl* impl =
        (mamaMsgFieldImpl*)(msgField);
    if (!impl) return MAMA_STATUS_INVALID_ARG;

    if (impl->myPayloadBridge)
    {
        return impl->myPayloadBridge->msgFieldPayloadGetU64 (
                                    impl->myPayload, result);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsgField_getF32(
        const mamaMsgField  msgField,
        mama_f32_t*         result)
{
    mamaMsgFieldImpl* impl =
        (mamaMsgFieldImpl*)(msgField);
    if (!impl) return MAMA_STATUS_INVALID_ARG;

    if (impl->myPayloadBridge)
    {
        return impl->myPayloadBridge->msgFieldPayloadGetF32 (
                                    impl->myPayload, result);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsgField_getF64(
        const mamaMsgField  msgField,
        mama_f64_t*         result)
{
    mamaMsgFieldImpl* impl =
        (mamaMsgFieldImpl*)(msgField);
    if (!impl) return MAMA_STATUS_INVALID_ARG;

    if (impl->myPayloadBridge)
    {
        return impl->myPayloadBridge->msgFieldPayloadGetF64 (
                                    impl->myPayload, result);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsgField_getString(
    const mamaMsgField  msgField,
    const char**        result)
{
    mamaMsgFieldImpl* impl =
        (mamaMsgFieldImpl*)(msgField);
    if (!impl) return MAMA_STATUS_INVALID_ARG;

    if (impl->myPayloadBridge)
    {
        return impl->myPayloadBridge->msgFieldPayloadGetString (
                                    impl->myPayload, result);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsgField_getOpaque(
        const mamaMsgField  msgField,
        const void**        result,
        size_t*             size)
{
    mamaMsgFieldImpl* impl =
        (mamaMsgFieldImpl*)(msgField);
    if (!impl) return MAMA_STATUS_INVALID_ARG;

    if (impl->myPayloadBridge)
    {
        return impl->myPayloadBridge->msgFieldPayloadGetOpaque (
                                    impl->myPayload, result, size);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsgField_getDateTime(
    const mamaMsgField  msgField,
    mamaDateTime        result)
{
    mamaMsgFieldImpl* impl =
        (mamaMsgFieldImpl*)(msgField);
    if (!impl) return MAMA_STATUS_INVALID_ARG;

    if (impl->myPayloadBridge)
    {
        return impl->myPayloadBridge->msgFieldPayloadGetDateTime(
                                    impl->myPayload, result);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsgField_getPrice(
    const mamaMsgField  msgField,
    mamaPrice           result)
{
    mamaMsgFieldImpl* impl =
        (mamaMsgFieldImpl*)(msgField);
    if (!impl) return MAMA_STATUS_INVALID_ARG;

    if (impl->myPayloadBridge)
    {
        return impl->myPayloadBridge->msgFieldPayloadGetPrice (
                                    impl->myPayload, result);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsgField_getMsg (
    const mamaMsgField msgField,
    mamaMsg*           result)
{
    mamaMsgFieldImpl* impl =
        (mamaMsgFieldImpl*)(msgField);
    if (!impl) return MAMA_STATUS_INVALID_ARG;

    *result = NULL;

    if (!impl->mySubMsg)
    {
        mamaMsg_create (&impl->mySubMsg);
    }

    if (impl->myPayloadBridge)
    {
        msgPayload payload = NULL;
        mama_status status = MAMA_STATUS_OK;

        status = impl->myPayloadBridge->msgFieldPayloadGetMsg (
                                    impl->myPayload, &payload);
        if (MAMA_STATUS_OK != status) return status;

        mamaMsgImpl_setPayloadBridge (impl->mySubMsg, impl->myPayloadBridge);
        mamaMsgImpl_setPayload       (impl->mySubMsg, payload, 0);
        *result = impl->mySubMsg;
        return MAMA_STATUS_OK;
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsgField_getVectorChar (
    const mamaMsgField     msgField,
    const char**           result,
    size_t*                size)
{
    mamaMsgFieldImpl* impl =
        (mamaMsgFieldImpl*)(msgField);
    if (!impl) return MAMA_STATUS_INVALID_ARG;

    if (impl->myPayloadBridge)
    {
        return impl->myPayloadBridge->msgFieldPayloadGetVectorChar (
                                     impl->myPayload, result, size);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsgField_getVectorI8 (
    const mamaMsgField     msgField,
    const int8_t**         result,
    size_t*                size)
{
    mamaMsgFieldImpl* impl =
        (mamaMsgFieldImpl*)(msgField);
    if (!impl) return MAMA_STATUS_INVALID_ARG;

    if (impl->myPayloadBridge)
    {
        return impl->myPayloadBridge->msgFieldPayloadGetVectorI8 (
                                     impl->myPayload, result, size);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsgField_getVectorU8 (
    const mamaMsgField     msgField,
    const uint8_t**        result,
    size_t*                size)
{
    mamaMsgFieldImpl* impl =
        (mamaMsgFieldImpl*)(msgField);
    if (!impl) return MAMA_STATUS_INVALID_ARG;

    if (impl->myPayloadBridge)
    {
        return impl->myPayloadBridge->msgFieldPayloadGetVectorU8 (
                                     impl->myPayload, result, size);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsgField_getVectorI16 (
    const mamaMsgField     msgField,
    const int16_t**        result,
    size_t*                size)
{
    mamaMsgFieldImpl* impl =
        (mamaMsgFieldImpl*)(msgField);
    if (!impl) return MAMA_STATUS_INVALID_ARG;

    if (impl->myPayloadBridge)
    {
        return impl->myPayloadBridge->msgFieldPayloadGetVectorI16 (
                                     impl->myPayload, result, size);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsgField_getVectorU16 (
    const mamaMsgField     msgField,
    const uint16_t**       result,
    size_t*                size)
{
    mamaMsgFieldImpl* impl =
        (mamaMsgFieldImpl*)(msgField);
    if (!impl) return MAMA_STATUS_INVALID_ARG;

    if (impl->myPayloadBridge)
    {
        return impl->myPayloadBridge->msgFieldPayloadGetVectorU16 (
                                     impl->myPayload, result, size);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsgField_getVectorI32 (
    const mamaMsgField     msgField,
    const int32_t**        result,
    size_t*                size)
{
    mamaMsgFieldImpl* impl =
        (mamaMsgFieldImpl*)(msgField);
    if (!impl) return MAMA_STATUS_INVALID_ARG;

    if (impl->myPayloadBridge)
    {
        return impl->myPayloadBridge->msgFieldPayloadGetVectorI32 (
                                     impl->myPayload, result, size);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsgField_getVectorU32 (
    const mamaMsgField     msgField,
    const uint32_t**       result,
    size_t*                size)
{
    mamaMsgFieldImpl* impl =
        (mamaMsgFieldImpl*)(msgField);
    if (!impl) return MAMA_STATUS_INVALID_ARG;

    if (impl->myPayloadBridge)
    {
        return impl->myPayloadBridge->msgFieldPayloadGetVectorU32 (
                                     impl->myPayload, result, size);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsgField_getVectorI64 (
    const mamaMsgField     msgField,
    const int64_t**        result,
    size_t*                size)
{
    mamaMsgFieldImpl* impl =
        (mamaMsgFieldImpl*)(msgField);
    if (!impl) return MAMA_STATUS_INVALID_ARG;

    if (impl->myPayloadBridge)
    {
        return impl->myPayloadBridge->msgFieldPayloadGetVectorI64 (
                                     impl->myPayload, result, size);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsgField_getVectorU64 (
    const mamaMsgField     msgField,
    const uint64_t**       result,
    size_t*                size)
{
    mamaMsgFieldImpl* impl =
        (mamaMsgFieldImpl*)(msgField);
    if (!impl) return MAMA_STATUS_INVALID_ARG;

    if (impl->myPayloadBridge)
    {
        return impl->myPayloadBridge->msgFieldPayloadGetVectorU64 (
                                     impl->myPayload, result, size);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsgField_getVectorF32 (
    const mamaMsgField        msgField,
    const mama_f32_t**        result,
    size_t*                   size)
{
    mamaMsgFieldImpl* impl =
        (mamaMsgFieldImpl*)(msgField);
    if (!impl) return MAMA_STATUS_INVALID_ARG;

    if (impl->myPayloadBridge)
    {
        return impl->myPayloadBridge->msgFieldPayloadGetVectorF32 (
                                     impl->myPayload, result, size);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsgField_getVectorF64 (
    const mamaMsgField        msgField,
    const mama_f64_t**        result,
    size_t*                   size)
{
    mamaMsgFieldImpl* impl =
        (mamaMsgFieldImpl*)(msgField);
    if (!impl) return MAMA_STATUS_INVALID_ARG;

    if (impl->myPayloadBridge)
    {
        return impl->myPayloadBridge->msgFieldPayloadGetVectorF64 (
                                     impl->myPayload, result, size);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsgField_getVectorString (
        const mamaMsgField   msgField,
        const char***        result,
        size_t*              size)
{
    mamaMsgFieldImpl* impl =
        (mamaMsgFieldImpl*)(msgField);
    if (!impl) return MAMA_STATUS_INVALID_ARG;

    if (impl->myPayloadBridge)
    {
        return impl->myPayloadBridge->msgFieldPayloadGetVectorString (
                                    impl->myPayload, result, size);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsgField_getVectorMsg (
    const mamaMsgField     msgField,
    const mamaMsg**        result,
    mama_size_t*           resultLen)
{
    /* Initialise to error as NULL check will not cause status to be set */
    mama_status status        = MAMA_STATUS_OK;
    void*       vp   = NULL;
    mama_size_t size = 0;

    mamaMsgFieldImpl* impl = (mamaMsgFieldImpl*)(msgField);
    if (!impl) return MAMA_STATUS_NULL_ARG;

    *result    = NULL;
    *resultLen = 0;

    if (impl->myPayloadBridge)
    {
        msgPayload* payloadVector = NULL;
        size_t      i             = 0;

        if (MAMA_STATUS_OK == (status = impl->myPayloadBridge->msgFieldPayloadGetVectorMsg (
                    impl->myPayload,
                    (const msgPayload**) &payloadVector,
                    &size)))
        {
            /*Either create or reallocate the vector if it is not big enough*/
            if (!impl->myLastVectorPayloadMsg)
            {
                impl->myLastVectorPayloadMsg = (mamaMsg*)calloc(size,
                        sizeof(mamaMsg));

                if(impl->myLastVectorPayloadMsg == NULL)
                    return MAMA_STATUS_NOMEM;
            }
            else if (size > impl->myLastVectorPayloadMsgLen)
            {
                vp = realloc (impl->myLastVectorPayloadMsg,
                        size*sizeof(mamaMsg));

                if(vp == NULL)
                    return MAMA_STATUS_NOMEM;

                impl->myLastVectorPayloadMsg = (mamaMsg*)vp;
            }

            for (i = 0; (i != impl->myLastVectorPayloadMsgLen) && (i != size);
                 ++i)
            {
                mamaMsgImpl_setPayloadBridge (impl->myLastVectorPayloadMsg[i],
                                              impl->myPayloadBridge);
                /*
                 * We do not detach the middleware message so we do
                 * not own it. 
                 */
                mamaMsgImpl_setPayload       (impl->myLastVectorPayloadMsg[i],
                                              payloadVector[i],
                                              0);
            }

            for (; i != size; ++i)
            {
                /*
                 * We create from payload so payload owns
                 * We set owner to false to not delete the payload
                 */
                mamaMsgImpl_createForPayload (&(impl->myLastVectorPayloadMsg[i]),
                                  payloadVector[i],
                                  impl->myPayloadBridge,
                                  0);
            }

            /*Store the maxumim vector we have encountered*/
            if (size > impl->myLastVectorPayloadMsgLen)
            {
                impl->myLastVectorPayloadMsgLen = size;
            }

            *resultLen =  size;
            *result    = impl->myLastVectorPayloadMsg;
            return MAMA_STATUS_OK;
        }
        else
        {
            mama_log (MAMA_LOG_LEVEL_ERROR, "Failed to get payload msg vector from"
                                            "message");
            return status;
        }
    }
    return MAMA_STATUS_NULL_ARG;
}


mama_status
mamaMsgField_getAsString (
    const mamaMsgField  msgField,
    char *              buf,
    size_t              len)
{
    mamaMsgFieldImpl* impl    = (mamaMsgFieldImpl*)(msgField);
    msgPayload        payload = NULL;
    if (!impl) return MAMA_STATUS_INVALID_ARG;
    mamaMsgImpl_getPayload (impl->myMsg, &payload);

    if (impl->myPayloadBridge)
    {
        return impl->myPayloadBridge->msgFieldPayloadGetAsString (
                                    impl->myPayload,
                                    payload,
                                    buf, len-1);

    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsgField_updateBool (
    mamaMsgField  field,
    mama_bool_t   value)
{
    mamaMsgFieldImpl* impl = (mamaMsgFieldImpl*)field;
    short            owner = 0;
    msgPayload        payload = NULL;

    if (!impl) return MAMA_STATUS_NULL_ARG;

    mamaMsgImpl_getMessageOwner (impl->myMsg, &owner);
    if (!owner) return MAMA_STATUS_NOT_MODIFIABLE;
    
    mamaMsgImpl_getPayload (impl->myMsg, &payload);
    if (impl->myPayloadBridge)
    {
        return impl->myPayloadBridge->msgFieldPayloadUpdateBool (
                                    impl->myPayload,
                                    payload,
                                    value);

    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsgField_updateChar (
    mamaMsgField  field,
    char          value)
{
    mamaMsgFieldImpl* impl = (mamaMsgFieldImpl*)field;
    short             owner   = 0;
    msgPayload        payload = NULL;

    if (!impl) return MAMA_STATUS_NULL_ARG;

    mamaMsgImpl_getMessageOwner (impl->myMsg, &owner);
    if (!owner) return MAMA_STATUS_NOT_MODIFIABLE;
    
    mamaMsgImpl_getPayload (impl->myMsg, &payload);
    if (impl->myPayloadBridge)
    {
        return impl->myPayloadBridge->msgFieldPayloadUpdateChar (
                                        impl->myPayload,
                                        payload,
                                        value);

    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsgField_updateI8 (
    mamaMsgField  field,
    mama_i8_t     value)
{
    mamaMsgFieldImpl* impl = (mamaMsgFieldImpl*)field;
    short             owner   = 0;
    msgPayload        payload = NULL;

    if (!impl) return MAMA_STATUS_NULL_ARG;

    mamaMsgImpl_getMessageOwner (impl->myMsg, &owner);
    if (!owner) return MAMA_STATUS_NOT_MODIFIABLE;
    
    mamaMsgImpl_getPayload (impl->myMsg, &payload);
    if (impl->myPayloadBridge)
    {
        return impl->myPayloadBridge->msgFieldPayloadUpdateI8 (
                                        impl->myPayload,
                                        payload,
                                        value);

    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsgField_updateU8 (
    mamaMsgField  field,
    mama_u8_t     value)
{
    mamaMsgFieldImpl* impl = (mamaMsgFieldImpl*)field;
    short             owner   = 0;
    msgPayload        payload = NULL;

    if (!impl) return MAMA_STATUS_NULL_ARG;

    mamaMsgImpl_getMessageOwner (impl->myMsg, &owner);
    if (!owner) return MAMA_STATUS_NOT_MODIFIABLE;
    
    mamaMsgImpl_getPayload (impl->myMsg, &payload);
    if (impl->myPayloadBridge)
    {
        return impl->myPayloadBridge->msgFieldPayloadUpdateI8 (
                                            impl->myPayload,
                                            payload,
                                            value);

    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsgField_updateI16 (
    mamaMsgField  field,
    mama_i16_t    value)
{
    mamaMsgFieldImpl* impl = (mamaMsgFieldImpl*)field;
    short             owner   = 0;
    msgPayload        payload = NULL;

    if (!impl) return MAMA_STATUS_NULL_ARG;

    mamaMsgImpl_getMessageOwner (impl->myMsg, &owner);
    if (!owner) return MAMA_STATUS_NOT_MODIFIABLE;
    
    mamaMsgImpl_getPayload (impl->myMsg, &payload);
    if (impl->myPayloadBridge)
    {
        return impl->myPayloadBridge->msgFieldPayloadUpdateI16 (
                                            impl->myPayload,
                                            payload,
                                            value);

    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsgField_updateU16 (
    mamaMsgField  field,
    mama_u16_t    value)
{
    mamaMsgFieldImpl* impl = (mamaMsgFieldImpl*)field;
    short             owner   = 0;
    msgPayload        payload = NULL;

    if (!impl) return MAMA_STATUS_NULL_ARG;

    mamaMsgImpl_getMessageOwner (impl->myMsg, &owner);
    if (!owner) return MAMA_STATUS_NOT_MODIFIABLE;
    
    mamaMsgImpl_getPayload (impl->myMsg, &payload);
    if (impl->myPayloadBridge)
    {
        return impl->myPayloadBridge->msgFieldPayloadUpdateU16 (
                                            impl->myPayload,
                                            payload,
                                            value);

    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsgField_updateI32 (
    mamaMsgField  field,
    mama_i32_t    value)
{
    mamaMsgFieldImpl* impl = (mamaMsgFieldImpl*)field;
    short             owner   = 0;
    msgPayload        payload = NULL;

    if (!impl) return MAMA_STATUS_NULL_ARG;

    mamaMsgImpl_getMessageOwner (impl->myMsg, &owner);
    if (!owner) return MAMA_STATUS_NOT_MODIFIABLE;
    
    mamaMsgImpl_getPayload (impl->myMsg, &payload);
    if (impl->myPayloadBridge)
    {
        return impl->myPayloadBridge->msgFieldPayloadUpdateI32 (
                                                impl->myPayload,
                                                    payload,
                                                value);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsgField_updateU32 (
    mamaMsgField        field,
    mama_u32_t          value)
{
    mamaMsgFieldImpl* impl = (mamaMsgFieldImpl*)field;
    short             owner   = 0;
    msgPayload        payload = NULL;

    if (!impl) return MAMA_STATUS_NULL_ARG;

    mamaMsgImpl_getMessageOwner (impl->myMsg, &owner);
    if (!owner) return MAMA_STATUS_NOT_MODIFIABLE;
    
    mamaMsgImpl_getPayload (impl->myMsg, &payload);
    if (impl->myPayloadBridge)
    {
        return impl->myPayloadBridge->msgFieldPayloadUpdateU32 (
                                                    impl->myPayload,
                                                    payload,
                                                    value);

    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsgField_updateI64 (
    mamaMsgField       field,
    mama_i64_t         value)
{
    mamaMsgFieldImpl* impl = (mamaMsgFieldImpl*)field;
    short             owner   = 0;
    msgPayload        payload = NULL;

    if (!impl) return MAMA_STATUS_NULL_ARG;

    mamaMsgImpl_getMessageOwner (impl->myMsg, &owner);
    if (!owner) return MAMA_STATUS_NOT_MODIFIABLE;
    
    mamaMsgImpl_getPayload (impl->myMsg, &payload);
    if (impl->myPayloadBridge)
    {
        return impl->myPayloadBridge->msgFieldPayloadUpdateI64 (
                                                    impl->myPayload,
                                                    payload,
                                                    value);

    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsgField_updateU64 (
    const mamaMsgField  field,
    mama_u64_t          value)
{
    mamaMsgFieldImpl* impl = (mamaMsgFieldImpl*)field;
    short             owner   = 0;
    msgPayload        payload = NULL;

    if (!impl) return MAMA_STATUS_NULL_ARG;

    mamaMsgImpl_getMessageOwner (impl->myMsg, &owner);
    if (!owner) return MAMA_STATUS_NOT_MODIFIABLE;
    
    mamaMsgImpl_getPayload (impl->myMsg, &payload);
    if (impl->myPayloadBridge)
    {
        return impl->myPayloadBridge->msgFieldPayloadUpdateU64 (
                                                    impl->myPayload,
                                                    payload,
                                                    value);

    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsgField_updateF32 (
            const mamaMsgField  field,
            mama_f32_t          value)
{
    mamaMsgFieldImpl* impl = (mamaMsgFieldImpl*)field;
    short             owner   = 0;
    msgPayload        payload = NULL;

    if (!impl) return MAMA_STATUS_NULL_ARG;

    mamaMsgImpl_getMessageOwner (impl->myMsg, &owner);
    if (!owner) return MAMA_STATUS_NOT_MODIFIABLE;
    
    mamaMsgImpl_getPayload (impl->myMsg, &payload);
    if (impl->myPayloadBridge)
    {
        return impl->myPayloadBridge->msgFieldPayloadUpdateF32 (
                                                    impl->myPayload,
                                                    payload,
                                                    value);

    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsgField_updateF64 (
            mamaMsgField  field,
            mama_f64_t    value)
{
    mamaMsgFieldImpl* impl = (mamaMsgFieldImpl*)field;
    short             owner   = 0;
    msgPayload        payload = NULL;

    if (!impl) return MAMA_STATUS_NULL_ARG;

    mamaMsgImpl_getMessageOwner (impl->myMsg, &owner);
    if (!owner) return MAMA_STATUS_NOT_MODIFIABLE;
    
    mamaMsgImpl_getPayload (impl->myMsg, &payload);
    if (impl->myPayloadBridge)
    {
        return impl->myPayloadBridge->msgFieldPayloadUpdateF64 (
                                                    impl->myPayload,
                                                    payload,
                                                    value);

    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsgField_updateDateTime (
            mamaMsgField        field,
            const mamaDateTime  value)
{
    mamaMsgFieldImpl* impl = (mamaMsgFieldImpl*)field;
    short             owner   = 0;
    msgPayload        payload = NULL;

    if (!impl) return MAMA_STATUS_NULL_ARG;

    mamaMsgImpl_getMessageOwner (impl->myMsg, &owner);
    if (!owner) return MAMA_STATUS_NOT_MODIFIABLE;
    
    mamaMsgImpl_getPayload (impl->myMsg, &payload);
    if (impl->myPayloadBridge)
    {
        return impl->myPayloadBridge->msgFieldPayloadUpdateDateTime (
                                                          impl->myPayload,
                                                          payload,
                                                          value);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsgField_updatePrice (
            mamaMsgField           field,
            const mamaPrice        value)
{
    mamaMsgFieldImpl* impl = (mamaMsgFieldImpl*)field;
    short             owner   = 0;
    msgPayload        payload = NULL;

    if (!impl) return MAMA_STATUS_NULL_ARG;

    mamaMsgImpl_getMessageOwner (impl->myMsg, &owner);
    if (!owner) return MAMA_STATUS_NOT_MODIFIABLE;
    
    mamaMsgImpl_getPayload (impl->myMsg, &payload);
    if (impl->myPayloadBridge)
    {
        return impl->myPayloadBridge->msgFieldPayloadUpdatePrice (
                                                          impl->myPayload,
                                                          payload,
                                                          value);

    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsgField_getTypeName(
    const mamaMsgField    msgField,
    const char**          result)
{
    mamaFieldType  type;
    mama_status status = mamaMsgField_getType (msgField, &type);
    if (MAMA_STATUS_OK == status)
    {
        *result = mamaFieldTypeToString (type);
    }
    return status;
}

mama_status
mamaMsgFieldImpl_setPayloadBridge (mamaMsgField             msgField,
                                   mamaPayloadBridgeImpl*   payloadBridge)
{
    mamaMsgFieldImpl* impl = (mamaMsgFieldImpl*)msgField;
    if (!impl) return MAMA_STATUS_NULL_ARG;
    impl->myPayloadBridge  =   payloadBridge;

    return MAMA_STATUS_OK;
}

mama_status
mamaMsgFieldImpl_setPayload (mamaMsgField           msgField,
                             msgFieldPayload        payload)
{
    mamaMsgFieldImpl* impl = (mamaMsgFieldImpl*)msgField;
    if (!impl) return MAMA_STATUS_NULL_ARG;

    impl->myPayload  = payload;
    impl->myFieldDesc = NULL;
    return MAMA_STATUS_OK;
}

mama_status
mamaMsgFieldImpl_setFieldDesc (mamaMsgField         msgField,
                               mamaFieldDescriptor  descriptor)
{
    mamaMsgFieldImpl* impl = (mamaMsgFieldImpl*)msgField;
    if (!impl) return MAMA_STATUS_NULL_ARG;

    impl->myFieldDesc = descriptor;
    return MAMA_STATUS_OK;

}
