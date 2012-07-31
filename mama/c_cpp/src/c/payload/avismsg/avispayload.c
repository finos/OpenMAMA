/* $Id: avispayload.c,v 1.1.2.17 2011/10/02 19:02:18 ianbell Exp $
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "payloadbridge.h"
#include "mama/mama.h"
#include "msgfieldimpl.h"

#include <avis/elvin.h>
#include <mama/mama.h>
#include <mama/reservedfields.h>
#include <bridge.h>
#include <wombat/strutils.h>

#include "avispayload.h"
#include "avismsgimpl.h"
#include "msgfield.h"
#include "msgfieldimpl.h"
#include "../../bridge/avis/avisdefs.h"
#include "platform.h"


#define avisPayloadImpl(msg) ((avisPayloadImpl*)msg)
#define avisPayload(msg) (((avisPayloadImpl*)msg)->mAvisMsg)

#define CHECK_PAYLOAD(msg) \
        do {  \
           if ((avisPayload(msg)) == 0) return MAMA_STATUS_NULL_ARG; \
         } while(0)

#define CHECK_NAME(name,fid) \
        do {  \
           if ((fid == 0) && (name == 0)) return MAMA_STATUS_NULL_ARG; \
           if ((fid == 0) && (strlen(name)== 0)) return MAMA_STATUS_INVALID_ARG; \
         } while(0)


#define CHECK_ITER(iter) \
        do {  \
           if (((avisMsgIteratorImpl*)(iter)) == 0) return MAMA_STATUS_NULL_ARG; \
         } while(0)

#define avisField(field) ((avisFieldPayload*)(field))

#define CHECK_FIELD(field) \
        do {  \
           if (avisField(field) == 0) return MAMA_STATUS_NULL_ARG; \
         } while(0)


msgFieldPayload
avismsgPayloadIter_get          (msgPayloadIter  iter,
                                msgFieldPayload field,
                                msgPayload      msg);
/******************************************************************************
* bridge functions
*******************************************************************************/
extern mama_status
avismsgPayload_destroyImpl (mamaPayloadBridge mamaPayloadBridge)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != mamaPayloadBridge)
    {
        /* Get the impl. */
        mamaPayloadBridgeImpl *impl = (mamaPayloadBridgeImpl *)mamaPayloadBridge;

        /* Free the impl. */
        free(impl);
    }

    return ret;
}

extern mama_status
avismsgPayload_createImpl (mamaPayloadBridge* result, char* identifier)
{
    mamaPayloadBridgeImpl*       impl    = NULL;
    mama_status             resultStatus = MAMA_STATUS_OK;

    if (!result) return MAMA_STATUS_NULL_ARG;

    impl = (mamaPayloadBridgeImpl*)calloc (1, sizeof (mamaPayloadBridgeImpl));
    if (!impl)
    {
        mama_log (MAMA_LOG_LEVEL_SEVERE, "avismsgPayload_createImpl(): "
                  "Could not allocate memory for payload impl.");
        return MAMA_STATUS_NULL_ARG;
    }

    INITIALIZE_PAYLOAD_BRIDGE (impl, avismsg);

    impl->mClosure = NULL;

    *result     = (mamaPayloadBridge)impl;
    *identifier = MAMA_PAYLOAD_AVIS;

    return resultStatus;
}

mamaPayloadType
avismsgPayload_getType ()
{
    return MAMA_PAYLOAD_AVIS;
}

/******************************************************************************
* general functions
*******************************************************************************/
mama_status
avismsgPayload_create (msgPayload* msg)
{

    Attributes*  avisMsg = attributes_create();
	avisPayloadImpl* newPayload = NULL;

    if (!msg)  return MAMA_STATUS_NULL_ARG;

    if (!avisMsg) {
        mama_log (MAMA_LOG_LEVEL_ERROR, "attributes_create() failed. Could not create Avis msg");
        return MAMA_STATUS_PLATFORM;
    }
    newPayload = (avisPayloadImpl*)calloc (1, sizeof(avisPayloadImpl));
    newPayload->mAvisMsg=avisMsg;

    *msg = newPayload;

    return MAMA_STATUS_OK;
}

mama_status
avismsgPayload_createForTemplate (msgPayload*         msg,
                                mamaPayloadBridge       bridge,
                                mama_u32_t          templateId)
{

    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avismsgPayload_createFromByteBuffer(msgPayload* msg, mamaPayloadBridge bridge, const void* buffer, mama_size_t bufferLength)
{
    avisPayloadImpl* newPayload = (avisPayloadImpl*)calloc (1, sizeof(avisPayloadImpl));

    if (buffer != NULL)
       newPayload->mAvisMsg=(Attributes*)buffer;


    *msg=newPayload;

    return MAMA_STATUS_OK;
}

mama_status
avismsgPayload_copy              (const msgPayload    msg,
                                msgPayload*         copy)
{
    CHECK_PAYLOAD(msg);
    if (*copy == NULL) avismsgPayload_create (copy);

    avisPayload(*copy) = attributes_clone(avisPayload(msg));
    if (!*copy) return MAMA_STATUS_PLATFORM;
    return MAMA_STATUS_OK;
}

mama_status
avismsgPayload_clear             (msgPayload          msg)
{
    CHECK_PAYLOAD(msg);
    attributes_clear(avisPayload(msg));
    return MAMA_STATUS_OK;
}

mama_status
avismsgPayload_destroy           (msgPayload          msg)
{
    CHECK_PAYLOAD(msg);
    attributes_destroy(avisPayload(msg));

    avismsgPayloadIter_destroy(avisPayloadImpl(msg)->mIterator);
    free(avisPayloadImpl(msg)->mAvisField);
    free(avisPayloadImpl(msg)->mStringBuffer);
    free(avisPayloadImpl(msg));
    return MAMA_STATUS_OK;
}

mama_status
avismsgPayload_setParent (msgPayload          msg,                           
                       const mamaMsg       parent)
{
    avisPayloadImpl* impl = (avisPayloadImpl*) msg;
    if (!impl) return MAMA_STATUS_NULL_ARG;

    impl->mParent = parent;
    
    return MAMA_STATUS_OK;
}

mama_status
avismsgPayload_getByteSize       (const msgPayload    msg,
                                mama_size_t*        size)
{
    CHECK_PAYLOAD(msg);
    *size = 0;
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avismsgPayload_unSerialize (const msgPayload    msg,
                           const void*        buffer,
                           mama_size_t        bufferLength)
{
    avisPayloadImpl* impl = (avisPayloadImpl*) msg;
    char tempName[64];
    int32_t tempint32;
    int64_t tempint64;
    real64_t  tempreal64;

	uint32_t	currLen = 1;
    uint16_t    len  =0;
	uint8_t *	buffPos = (void*)buffer;

    if (!impl->mAvisMsg)
        impl->mAvisMsg = attributes_create();
    else
        attributes_clear(impl->mAvisMsg);

     buffPos+=1; // Skip payload identifier
    while (currLen < bufferLength)
    {
        ValueType type = (ValueType) *(int8_t *)(buffPos); buffPos+=1; currLen+=1;
        switch (type)
        {
            case TYPE_INT32:
                len= *(int16_t *)(buffPos); buffPos+=sizeof(int16_t); currLen+=sizeof(int16_t);
                memcpy (tempName, buffPos, len); buffPos+=len; currLen+=len;
                tempName[len]='\0';
                tempint32 = *(int32_t *)(buffPos); buffPos+=sizeof(int32_t); currLen+=sizeof(int32_t);
                avisMsg_setI32(impl->mAvisMsg, tempName, 0, tempint32);
                break;

            case TYPE_INT64:
                len= *(int16_t *)(buffPos);buffPos+=sizeof(int16_t); currLen+=sizeof(int16_t);
                memcpy (tempName, buffPos, len); buffPos+=len; currLen+=len;
                tempName[len]='\0';
                tempint64 = *(int64_t *)(buffPos); buffPos+=sizeof(int64_t); currLen+=sizeof(int64_t);
                avisMsg_setI64(impl->mAvisMsg, tempName, 0, tempint64);
                break;
            case TYPE_REAL64:
                len= *(int16_t *)(buffPos);buffPos+=sizeof(int16_t); currLen+=sizeof(int16_t);
                memcpy (tempName, buffPos, len); buffPos+=len; currLen+=len;
                tempName[len]='\0';
                tempreal64= *(real64_t*)(buffPos); buffPos+=sizeof(real64_t); currLen+=sizeof(real64_t);
                avisMsg_setF64(impl->mAvisMsg, tempName, 0, tempreal64);
                break;
            case TYPE_STRING:
                len= *(int16_t *)(buffPos);buffPos+=sizeof(int16_t); currLen+=sizeof(int16_t);
                memcpy (tempName, buffPos, len); buffPos+=len; currLen+=len;
                tempName[len]='\0';
                len= *(int16_t *)(buffPos);buffPos+=sizeof(int16_t); currLen+=sizeof(int16_t);
                if (!impl->mStringBuffer)
                {
                    impl->mStringBuffer= malloc(len+1);
                    impl->mStringBufferLen = len+1;
                }
                else if (impl->mStringBufferLen < (len+1))
                {
                    impl->mStringBuffer= realloc(impl->mStringBuffer, len+1);
                    impl->mStringBufferLen = len+1;
                }
                memcpy (impl->mStringBuffer, buffPos, len); buffPos+=len; currLen+=len;
                impl->mStringBuffer[len]='\0';
                avisMsg_setString(impl->mAvisMsg, tempName, 0, impl->mStringBuffer);
                break;
            case TYPE_OPAQUE:
                break;
        }
    }

    return MAMA_STATUS_OK;
}


mama_status
avismsgPayload_serialize     (const msgPayload    msg,
                                const void**        buffer,
                                mama_size_t*        bufferLength)
{
    avisPayloadImpl* impl = (avisPayloadImpl*) msg;
	uint8_t *			buffPos		= NULL;
    mama_status status = MAMA_STATUS_OK;
	uint16_t			len			= 0;
    uint32_t			currLen		= 0;
	avisFieldPayload*	currField	= NULL;

    if (!impl->mIterator)
    {
        status = avismsgPayloadIter_create((msgPayloadIter*) &impl->mIterator, msg);
    if (status != MAMA_STATUS_OK) return status;
    }

    status = avismsgPayloadIter_associate(impl->mIterator, msg);
    if (status != MAMA_STATUS_OK) {
       return status;
    }

    if (!impl->mAvisField)
    {
        status = avismsgFieldPayload_create((msgFieldPayload*) &impl->mAvisField);
        if (status  != MAMA_STATUS_OK)
            return status;
    }


    if (!impl->mBuffer)
    {
        impl->mBuffer = calloc (1, 200);
        impl->mBufferLen = 200;
    }

    buffPos = impl->mBuffer;
    currField = avismsgPayloadIter_begin(impl->mIterator, (msgFieldPayload) impl->mAvisField, msg);

    *(int8_t *)(buffPos) = MAMA_PAYLOAD_AVIS;
    buffPos+=1; currLen+=1;
    while (currField != NULL) {


        switch (currField->mValue->type)
        {
            case TYPE_INT32:
                len=(uint16_t)strlen(currField->mName);
                if (impl->mBufferLen < currLen+3+len+sizeof(int32_t))
                {
                    void*vp=realloc (impl->mBuffer, impl->mBufferLen+200);
                    impl->mBuffer = vp;
                    buffPos=(uint8_t *)impl->mBuffer;
					buffPos+=currLen;
                    impl->mBufferLen+=200;
                }
                *(int8_t *)(buffPos) = 1;   buffPos+=1;     currLen+=1;
                *(int16_t *)(buffPos) = len; buffPos+=2; currLen+=2;
                memcpy (buffPos, currField->mName, len); buffPos+=len; currLen+=len;
                *(int32_t *)(buffPos) = currField->mValue->value.int32; buffPos+=sizeof(int32_t); currLen+=sizeof(int32_t);
                break;
            case TYPE_INT64:
                len=strlen(currField->mName);
                if (impl->mBufferLen < currLen+3+len+sizeof(int64_t))
                {
                    void*vp=realloc (impl->mBuffer, impl->mBufferLen+200);
                    impl->mBuffer = vp;
                    buffPos=&impl->mBuffer;
					buffPos+=currLen;
                    impl->mBufferLen+=200;
                }
                *(int8_t *)(buffPos) = 2;   buffPos+=1;     currLen+=1;
                *(int16_t *)(buffPos) = len; buffPos+=2; currLen+=2;
                memcpy (buffPos, currField->mName, len); buffPos+=len; currLen+=len;
                *(int64_t *)(buffPos) = currField->mValue->value.int64; buffPos+=sizeof(int64_t); currLen+=sizeof(int64_t);
                break;
            case TYPE_REAL64:
                len=+ strlen(currField->mName);;
                if (impl->mBufferLen < currLen+3+len+sizeof(real64_t))
                {
                    void*vp=realloc (impl->mBuffer, impl->mBufferLen+200);
                    impl->mBuffer = vp;
                    buffPos=&impl->mBuffer;
					buffPos+=currLen;
                    impl->mBufferLen+=200;
                }
                *(int8_t *)(buffPos) = 3;   buffPos+=1;     currLen+=1;
                *(int16_t *)(buffPos) = len; buffPos+=2; currLen+=2;
                memcpy (buffPos, currField->mName, len); buffPos+=len; currLen+=len;
                *(real64_t *)(buffPos) = currField->mValue->value.real64; buffPos+=sizeof(real64_t); currLen+=sizeof(real64_t);
                break;
            case TYPE_STRING:
                len=strlen(currField->mName);
                if (impl->mBufferLen < currLen+5+len + strlen(currField->mValue->value.str))
                {
                    void*vp=realloc (impl->mBuffer, impl->mBufferLen+200);
                    impl->mBuffer = vp;
                    buffPos=&impl->mBuffer;
					buffPos+=currLen;
                    impl->mBufferLen+=200;
                }
                *(int8_t *)(buffPos) = 4;   buffPos+=1;     currLen+=1;
                *(int16_t *)(buffPos) = len; buffPos+=2; currLen+=2;
                memcpy (buffPos, currField->mName, len); buffPos+=len; currLen+=len;
                *(int16_t *)(buffPos) = strlen(currField->mValue->value.str); buffPos+=2; currLen+=2;
                memcpy (buffPos, currField->mValue->value.str, strlen(currField->mValue->value.str));
                buffPos+=strlen(currField->mValue->value.str); currLen+=strlen(currField->mValue->value.str);
                break;
            case TYPE_OPAQUE:
                break;

        }
        currField = avismsgPayloadIter_next(impl->mIterator, (msgFieldPayload) impl->mAvisField, msg);
    }

    *buffer = impl->mBuffer;
    *bufferLength = currLen;
    return MAMA_STATUS_OK;
}

mama_status
avismsgPayload_getByteBuffer     (const msgPayload    msg,
                                const void**        buffer,
                                mama_size_t*        bufferLength)
{
    avisPayloadImpl* impl = (avisPayloadImpl*)msg;
    CHECK_PAYLOAD(msg);

    *buffer = impl->mAvisMsg;


    return MAMA_STATUS_OK;
}

mama_status
avismsgPayload_setByteBuffer     (const msgPayload    msg,
                                mamaPayloadBridge       bridge,
                                const void*         buffer,
                                mama_size_t         bufferLength)
{
    avisPayloadImpl* impl = (avisPayloadImpl*)msg;
    CHECK_PAYLOAD(msg);

    impl->mAvisMsg=(Attributes*) buffer;


    return MAMA_STATUS_OK;
}

mama_status
avismsgPayload_getSendSubject     (const msgPayload    msg,
                                const char ** subject)
{
    CHECK_PAYLOAD(msg);
    return avismsgPayload_getString(msg, SUBJECT_FIELD_NAME, 0, subject);
}


mama_status
avismsgPayload_getNumFields      (const msgPayload    msg,
                                mama_size_t*        numFields)
{
    CHECK_PAYLOAD(msg);
    *numFields = attributes_size(avisPayload(msg));
    return MAMA_STATUS_OK;
}

const char*
avismsgPayload_toString          (const msgPayload    msg)
{
    avisPayloadImpl* impl = (avisPayloadImpl*)msg;
    mama_status status = MAMA_STATUS_OK;
	char *strpos =	NULL;
	bool comma = false;
	uint16_t curlen = 1;
	avisFieldPayload* currField = NULL;
    if (!impl->mIterator)
    {
        status = avismsgPayloadIter_create((msgPayloadIter*) &impl->mIterator, msg);
    if (status != MAMA_STATUS_OK) return NULL;
    }

    status = avismsgPayloadIter_associate(impl->mIterator, msg);
    if (status != MAMA_STATUS_OK) {
       return NULL;
    }

    if (!impl->mAvisField)
    {
        status = avismsgFieldPayload_create((msgFieldPayload*) &impl->mAvisField);
        if (status != MAMA_STATUS_OK)
            return NULL;
    }

    if (!impl->mStringBuffer)
    {
        impl->mStringBuffer= malloc(200);
        impl->mStringBufferLen = 200;
    }

    strpos =impl->mStringBuffer;
    sprintf (strpos, "%s", "{");strpos++;
    comma = false;
    curlen = 1;

    currField = avismsgPayloadIter_begin(impl->mIterator, (msgFieldPayload) impl->mAvisField, msg);
    while (currField != NULL) {
         char valueString[512];
         avisValue_getFieldAsString(currField->mValue, NULL, 0, valueString, sizeof(valueString));
         while ((curlen+strlen(valueString)+6+strlen(currField->mName)) >= impl->mStringBufferLen)
         {
            impl->mStringBuffer= realloc(impl->mStringBuffer, impl->mStringBufferLen+200);
            impl->mStringBufferLen += 200;
            strpos=&impl->mStringBuffer[curlen];
         }
         if (comma)
         {
             sprintf (strpos, "%s", ",");
             strpos++;curlen++;
         }
         sprintf (strpos, "%s", "["); strpos+=1;curlen+=1;
         sprintf (strpos, "%s",  currField->mName);strpos+=strlen(currField->mName);curlen+=strlen(currField->mName);
         sprintf (strpos, "%s", "]=");strpos+=2;curlen+=2;
         sprintf (strpos, "%s", valueString);strpos+=strlen(valueString);curlen+=strlen(valueString);
         comma = true;
         currField = avismsgPayloadIter_next(impl->mIterator, (msgFieldPayload) impl->mAvisField, msg);
    }

    sprintf (strpos, "%s",  "}");

    return impl->mStringBuffer;
}

mama_status
avismsgPayload_iterateFields (const msgPayload        msg,
                            const mamaMsg           parent,
                            mamaMsgField            field,
                            mamaMsgIteratorCb       cb,
                            void*                   closure)
{
    avisPayloadImpl* impl = (avisPayloadImpl*)msg;
    mama_status status = MAMA_STATUS_OK;
	avisFieldPayload* currField = NULL;

    if (!impl->mIterator)
    {
        status = avismsgPayloadIter_create((msgPayloadIter*) &impl->mIterator, msg);
        if (status != MAMA_STATUS_OK) return status;
    }

    status = avismsgPayloadIter_associate(impl->mIterator, msg);
    if (status != MAMA_STATUS_OK) {
       return status;
    }

    if (!impl->mAvisField)
    {
        status = avismsgFieldPayload_create((msgFieldPayload*) &impl->mAvisField);
        if (status != MAMA_STATUS_OK)
            return status;
    }

    currField = avismsgPayloadIter_begin(impl->mIterator, (msgFieldPayload) impl->mAvisField, msg);
    while (currField != NULL) {
         mamaMsgFieldImpl_setPayload (field, currField);
         (cb)(msg, field, closure);
         currField = avismsgPayloadIter_next(impl->mIterator, (msgFieldPayload) impl->mAvisField, msg);
    }

    return MAMA_STATUS_OK;
}

mama_status
avismsgPayload_getFieldAsString  (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                char*               buf,
                                mama_size_t         len)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name, fid);
    return avisMsg_getFieldAsString(avisPayload(msg), name, fid, buf, len);
}

mama_status
avismsgPayload_apply            (msgPayload          dest,
                               const msgPayload    src)
{
	avisPayloadImpl* implSrc = (avisPayloadImpl*)src;
	mama_status status = MAMA_STATUS_OK;
	avisFieldPayload* currField = NULL;
    CHECK_PAYLOAD(dest);
    CHECK_PAYLOAD(src);


    if (!implSrc->mIterator)
    {
        status = avismsgPayloadIter_create((msgPayloadIter*) &implSrc->mIterator, src);
        if (status != MAMA_STATUS_OK) return status;
    }

    status = avismsgPayloadIter_associate(implSrc->mIterator, src);
    if (status != MAMA_STATUS_OK) {
       return status;
    }

    if (!implSrc->mAvisField)
    {
        status = avismsgFieldPayload_create((msgFieldPayload*) &implSrc->mAvisField);
        if (status != MAMA_STATUS_OK)
            return status;
    }

    currField = avismsgPayloadIter_begin(implSrc->mIterator, (msgFieldPayload) implSrc->mAvisField, src);
    while (currField != NULL) {
        switch (currField->mValue->type)
        {
            case TYPE_INT32: avismsgPayload_updateI32(dest, currField->mName, 0, currField->mValue->value.int32); break;
            case TYPE_INT64: avismsgPayload_updateI64(dest, currField->mName, 0, currField->mValue->value.int64); break;
            case TYPE_REAL64: avismsgPayload_updateF64(dest, currField->mName, 0, currField->mValue->value.real64); break;
            case TYPE_STRING: avismsgPayload_updateString(dest, currField->mName, 0, currField->mValue->value.str); break;
            case TYPE_OPAQUE: avismsgPayload_updateOpaque(dest, currField->mName, 0, currField->mValue->value.bytes.items, currField->mValue->value.bytes.item_count); break;
        }
        currField = avismsgPayloadIter_next(implSrc->mIterator, (msgFieldPayload) implSrc->mAvisField, src);
    }
    return MAMA_STATUS_OK;


}

mama_status
avismsgPayload_getNativeMsg     (const msgPayload    msg,
                               void**              nativeMsg)
{
    CHECK_PAYLOAD(msg);
    *nativeMsg = msg;
    return MAMA_STATUS_OK;
}

/******************************************************************************
* add functions
*******************************************************************************/

mama_status
avismsgPayload_addBool           (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_bool_t         value)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name, fid);
    return avisMsg_setBool(avisPayload(msg), name, fid, value);
}

mama_status
avismsgPayload_addChar(
        msgPayload      msg,
        const char*     name,
        mama_fid_t      fid,
        char            value)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_setChar(avisPayload(msg), name, fid, value);
}

mama_status
avismsgPayload_addI8             (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i8_t           value)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_setI8(avisPayload(msg), name, fid, value);
}

mama_status
avismsgPayload_addU8             (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u8_t           value)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_setU8(avisPayload(msg), name, fid, value);
}

mama_status
avismsgPayload_addI16            (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i16_t          value)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_setI16(avisPayload(msg), name, fid, value);
}

mama_status
avismsgPayload_addU16            (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u16_t          value)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_setU16(avisPayload(msg), name, fid, value);
}

mama_status
avismsgPayload_addI32            (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i32_t          value)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_setI32(avisPayload(msg), name, fid, value);
}

mama_status
avismsgPayload_addU32            (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u32_t          value)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_setU32(avisPayload(msg), name, fid, value);
}

mama_status
avismsgPayload_addI64            (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i64_t          value)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_setI64(avisPayload(msg), name, fid, value);
}

mama_status
avismsgPayload_addU64            (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u64_t          value)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_setU64(avisPayload(msg), name, fid, value);
}

mama_status
avismsgPayload_addF32            (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_f32_t          value)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_setF32(avisPayload(msg), name, fid, value);
}

mama_status
avismsgPayload_addF64            (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_f64_t          value)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_setF64(avisPayload(msg), name, fid, value);
}

mama_status
avismsgPayload_addString         (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const char*         value)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_setString(avisPayload(msg), name, fid, value);
}

mama_status
avismsgPayload_addOpaque         (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const void*         value,
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_setOpaque(avisPayload(msg), name, fid, value, size);
}

mama_status
avismsgPayload_addDateTime       (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mamaDateTime  value)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_setDateTime(avisPayload(msg), name, fid, value);
}

mama_status
avismsgPayload_addPrice          (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mamaPrice     value)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_setPrice(avisPayload(msg), name, fid, value);
}

mama_status
avismsgPayload_addMsg            (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                msgPayload          value)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_setMsg(avisPayload(msg), name, fid, value);
}

/******************************************************************************
 * addVector... functions
 */

mama_status
avismsgPayload_addVectorBool     (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_bool_t   value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_setVectorBool(avisPayload(msg), name, fid, value, size);
}

mama_status
avismsgPayload_addVectorChar     (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const char          value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_setVectorChar(avisPayload(msg), name, fid, value, size);
}

mama_status
avismsgPayload_addVectorI8       (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_i8_t     value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_setVectorI8(avisPayload(msg), name, fid, value, size);
}

mama_status
avismsgPayload_addVectorU8       (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_u8_t     value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_setVectorU8(avisPayload(msg), name, fid, value, size);
}

mama_status
avismsgPayload_addVectorI16      (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_i16_t    value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_setVectorI16(avisPayload(msg), name, fid, value, size);
}

mama_status
avismsgPayload_addVectorU16      (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_u16_t    value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_setVectorU16(avisPayload(msg), name, fid, value, size);
}

mama_status
avismsgPayload_addVectorI32      (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_i32_t    value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_setVectorI32(avisPayload(msg), name, fid, value, size);
}

mama_status
avismsgPayload_addVectorU32      (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_u32_t    value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_setVectorU32(avisPayload(msg), name, fid, value, size);
}

mama_status
avismsgPayload_addVectorI64      (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const int64_t       value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_setVectorI64(avisPayload(msg), name, fid, value, size);
}

mama_status
avismsgPayload_addVectorU64      (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_u64_t    value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_setVectorU64(avisPayload(msg), name, fid, value, size);
}

mama_status
avismsgPayload_addVectorF32      (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_f32_t    value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_setVectorF32(avisPayload(msg), name, fid, value, size);
}

mama_status
avismsgPayload_addVectorF64      (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_f64_t    value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_setVectorF64(avisPayload(msg), name, fid, value, size);
}

mama_status
avismsgPayload_addVectorString   (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const char*         value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_setVectorString(avisPayload(msg), name, fid, value, size);
}

mama_status
avismsgPayload_addVectorMsg      (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mamaMsg       value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_setVectorMsg(avisPayload(msg), name, fid, value, size);
}

mama_status
avismsgPayload_addVectorDateTime (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mamaDateTime  value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_setVectorDateTime(avisPayload(msg), name, fid, value, size);
}

mama_status
avismsgPayload_addVectorPrice    (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mamaPrice     value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_setVectorPrice(avisPayload(msg), name, fid, value, size);
}

/******************************************************************************
* update functions
*******************************************************************************/
mama_status
avismsgPayload_updateBool        (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_bool_t         value)
{
    return avisMsg_setBool(avisPayload(msg), name, fid, value);
}

mama_status
avismsgPayload_updateChar        (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                char                value)
{
    return avisMsg_setChar(avisPayload(msg), name, fid, value);
}

mama_status
avismsgPayload_updateU8          (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u8_t           value)
{
    return avisMsg_setI8(avisPayload(msg), name, fid, value);
}

mama_status
avismsgPayload_updateI8          (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i8_t           value)
{
    return avisMsg_setU8(avisPayload(msg), name, fid, value);
}

mama_status
avismsgPayload_updateI16         (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i16_t          value)
{
    return avisMsg_setI16(avisPayload(msg), name, fid, value);
}

mama_status
avismsgPayload_updateU16         (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u16_t          value)
{
    return avisMsg_setU16(avisPayload(msg), name, fid, value);
}

mama_status
avismsgPayload_updateI32         (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i32_t          value)
{
    return avisMsg_setI32(avisPayload(msg), name, fid, value);
}

mama_status
avismsgPayload_updateU32         (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u32_t          value)
{
    return avisMsg_setU32(avisPayload(msg), name, fid, value);
}

mama_status
avismsgPayload_updateI64         (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i64_t          value)
{
    return avisMsg_setI64(avisPayload(msg), name, fid, value);
}

mama_status
avismsgPayload_updateU64         (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u64_t          value)
{
    return avisMsg_setU64(avisPayload(msg), name, fid, value);
}

mama_status
avismsgPayload_updateF32         (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_f32_t          value)
{
    return avisMsg_setF32(avisPayload(msg), name, fid, value);
}

mama_status
avismsgPayload_updateF64         (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_f64_t          value)
{
    return avisMsg_setF64(avisPayload(msg), name, fid, value);
}

mama_status
avismsgPayload_updateString      (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const char*         value)
{
    return avisMsg_setString(avisPayload(msg), name, fid, value);
}

mama_status
avismsgPayload_updateOpaque      (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const void*         value,
                                mama_size_t         size)
{
    return avisMsg_setOpaque(avisPayload(msg), name, fid, value, size);
}

mama_status
avismsgPayload_updateDateTime    (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mamaDateTime  value)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_setDateTime(avisPayload(msg), name, fid, value);
}

mama_status
avismsgPayload_updatePrice       (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mamaPrice     value)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_setPrice(avisPayload(msg), name, fid, value);
}

mama_status
avismsgPayload_updateSubMsg      (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const msgPayload    value)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return MAMA_STATUS_NOT_IMPLEMENTED;
}


/******************************************************************************
 * updateVector... functions
 */

mama_status
avismsgPayload_updateVectorMsg   (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mamaMsg       value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_setVectorMsg(avisPayload(msg), name, fid, value, size);
}

mama_status
avismsgPayload_updateVectorString (msgPayload         msg,
                                 const char*        name,
                                 mama_fid_t         fid,
                                 const char*        strList[],
                                 mama_size_t        size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_setVectorString(avisPayload(msg), name, fid, strList, size);
}

mama_status
avismsgPayload_updateVectorBool  (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_bool_t   boolList[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_setVectorBool(avisPayload(msg), name, fid, boolList, size);
}

mama_status
avismsgPayload_updateVectorChar  (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const char          value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_setVectorChar(avisPayload(msg), name, fid, value, size);
}

mama_status
avismsgPayload_updateVectorI8    (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_i8_t     value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_setVectorI8(avisPayload(msg), name, fid, value, size);
}

mama_status
avismsgPayload_updateVectorU8    (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_u8_t     value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_setVectorU8(avisPayload(msg), name, fid, value, size);
}

mama_status
avismsgPayload_updateVectorI16   (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_i16_t    value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_setVectorI16(avisPayload(msg), name, fid, value, size);
}

mama_status
avismsgPayload_updateVectorU16   (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_u16_t    value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_setVectorU16(avisPayload(msg), name, fid, value, size);
}

mama_status
avismsgPayload_updateVectorI32   (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_i32_t    value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_setVectorI32(avisPayload(msg), name, fid, value, size);
}

mama_status
avismsgPayload_updateVectorU32   (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_u32_t    value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_setVectorU32(avisPayload(msg), name, fid, value, size);
}

mama_status
avismsgPayload_updateVectorI64   (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_i64_t    value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_setVectorI64(avisPayload(msg), name, fid, value, size);
}

mama_status
avismsgPayload_updateVectorU64   (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_u64_t    value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_setVectorU64(avisPayload(msg), name, fid, value, size);
}

mama_status
avismsgPayload_updateVectorF32   (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_f32_t    value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_setVectorF32(avisPayload(msg), name, fid, value, size);
}

mama_status
avismsgPayload_updateVectorF64   (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_f64_t    value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_setVectorF64(avisPayload(msg), name, fid, value, size);
}

mama_status
avismsgPayload_updateVectorPrice (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mamaPrice     value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_setVectorPrice(avisPayload(msg), name, fid, value, size);
}

mama_status
avismsgPayload_updateVectorTime  (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mamaDateTime  value[],
                                mama_size_t         size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_setVectorDateTime(avisPayload(msg), name, fid, value, size);
}

/******************************************************************************
 * get... functions
 */

mama_status
avismsgPayload_getBool           (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_bool_t*        mamaResult)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_getBool(avisPayload(msg), name, fid, mamaResult);
}

mama_status
avismsgPayload_getChar           (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                char*               result)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_getChar(avisPayload(msg), name, fid, result);
}

mama_status
avismsgPayload_getI8             (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i8_t*          result)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_getI8(avisPayload(msg), name, fid, result);
}

mama_status
avismsgPayload_getU8             (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u8_t*          result)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_getU8(avisPayload(msg), name, fid, result);
}

mama_status
avismsgPayload_getI16            (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i16_t*         result)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_getI16(avisPayload(msg), name, fid, result);
}

mama_status
avismsgPayload_getU16            (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u16_t*         result)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_getU16(avisPayload(msg), name, fid, result);
}

mama_status
avismsgPayload_getI32            (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i32_t*         result)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_getI32(avisPayload(msg), name, fid, result);
}

mama_status
avismsgPayload_getU32            (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u32_t*         result)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_getU32(avisPayload(msg), name, fid, result);
}

mama_status
avismsgPayload_getI64            (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_i64_t*         mamaResult)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_getI64(avisPayload(msg), name, fid, mamaResult);
}

mama_status
avismsgPayload_getU64            (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_u64_t*         mamaResult)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_getU64(avisPayload(msg), name, fid, mamaResult);
}

mama_status
avismsgPayload_getF32            (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_f32_t*         result)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_getF32(avisPayload(msg), name, fid, result);
}

mama_status
avismsgPayload_getF64            (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mama_f64_t*         result)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_getF64(avisPayload(msg), name, fid, result);
}

mama_status
avismsgPayload_getString         (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const char**        result)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_getString(avisPayload(msg), name, fid, result);
}

mama_status
avismsgPayload_getOpaque         (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const void**        result,
                                mama_size_t*        size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_getOpaque(avisPayload(msg), name, fid, result, size);
}

mama_status
avismsgPayload_getField          (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                msgFieldPayload*    result)
{
	avisPayloadImpl* impl = (avisPayloadImpl*)msg;
	char tempName[64];
	char* id = (char*) name;
	Value* pValue = NULL;
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name, fid);

    if ( (!impl->mAvisField) && (avismsgFieldPayload_create((msgFieldPayload*) &impl->mAvisField) != MAMA_STATUS_OK) ) {
        return MAMA_STATUS_PLATFORM;
    }

	if (fid!=0)
	{
		snprintf (tempName, 63, "%d", fid);
		id=tempName;
	}
    pValue = attributes_get(avisPayload(msg), id);
    if ((!pValue) &&(name))
    {
   		pValue = attributes_get(avisPayload(msg), name);
   		id = (char*)name;
    }
    if (!pValue) {
      result = NULL;
      return MAMA_STATUS_NOT_FOUND;
    }

    impl->mAvisField->mName=id;
    impl->mAvisField->mValue=pValue;

    *result = impl->mAvisField;
    return MAMA_STATUS_OK;
}

mama_status
avismsgPayload_getDateTime       (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mamaDateTime        result)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_getDateTime(avisPayload(msg), name, fid, result);
}

mama_status
avismsgPayload_getPrice          (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                mamaPrice           result)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_getPrice(avisPayload(msg), name, fid, result);
}

mama_status
avismsgPayload_getMsg            (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                msgPayload*         result)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_getMsg(avisPayload(msg), name, fid, result);
}

/******************************************************************************
 * getVector... functions
 */

mama_status
avismsgPayload_getVectorBool     (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_bool_t** result,
                                mama_size_t*        size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_getVectorBool(avisPayload(msg), name, fid, result, size);
}

mama_status
avismsgPayload_getVectorChar     (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const char**        result,
                                mama_size_t*        size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_getVectorChar(avisPayload(msg), name, fid, result, size);
}

mama_status
avismsgPayload_getVectorI8       (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_i8_t**   result,
                                mama_size_t*        size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_getVectorI8(avisPayload(msg), name, fid, result, size);
}

mama_status
avismsgPayload_getVectorU8       (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_u8_t**   result,
                                mama_size_t*        size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_getVectorU8(avisPayload(msg), name, fid, result, size);
}

mama_status
avismsgPayload_getVectorI16      (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_i16_t**  result,
                                mama_size_t*        size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_getVectorI16(avisPayload(msg), name, fid, result, size);
}

mama_status
avismsgPayload_getVectorU16      (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_u16_t**  result,
                                mama_size_t*        size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_getVectorU16(avisPayload(msg), name, fid, result, size);
}

mama_status
avismsgPayload_getVectorI32      (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_i32_t**  result,
                                mama_size_t*        size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_getVectorI32(avisPayload(msg), name, fid, result, size);
}

mama_status
avismsgPayload_getVectorU32      (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_u32_t**  result,
                                mama_size_t*        size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_getVectorU32(avisPayload(msg), name, fid, result, size);
}

mama_status
avismsgPayload_getVectorI64      (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_i64_t**  result,
                                mama_size_t*        size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_getVectorI64(avisPayload(msg), name, fid, result, size);
}

mama_status
avismsgPayload_getVectorU64      (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_u64_t**  result,
                                mama_size_t*        size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_getVectorU64(avisPayload(msg), name, fid, result, size);
}

mama_status
avismsgPayload_getVectorF32      (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_f32_t**  result,
                                mama_size_t*        size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_getVectorF32(avisPayload(msg), name, fid, result, size);
}

mama_status
avismsgPayload_getVectorF64      (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_f64_t**  result,
                                mama_size_t*        size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_getVectorF64(avisPayload(msg), name, fid, result, size);
}

mama_status
avismsgPayload_getVectorString   (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const char***       result,
                                mama_size_t*        size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_getVectorString(avisPayload(msg), name, fid, result, size);
}

mama_status
avismsgPayload_getVectorDateTime (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mamaDateTime* result,
                                mama_size_t*        size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_getVectorDateTime(avisPayload(msg), name, fid, result, size);
}

mama_status
avismsgPayload_getVectorPrice    (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mamaPrice*    result,
                                mama_size_t*        size)
{
    CHECK_PAYLOAD(msg);
    CHECK_NAME(name,fid);
    return avisMsg_getVectorPrice(avisPayload(msg), name, fid, result, size);
}


mama_status
avismsgPayload_getVectorMsg      (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const msgPayload**  result,
                                mama_size_t*        size)
{
    CHECK_PAYLOAD(msg);

    return avisMsg_getVectorMsg(avisPayload(msg), name, fid, result, size);
}

/******************************************************************************
* iterator functions
*******************************************************************************/
mama_status
avismsgPayloadIter_create        (msgPayloadIter* iter,
                                msgPayload      msg)
{
    mama_status status = MAMA_STATUS_OK;
	avisIterator* impl = NULL;
    CHECK_PAYLOAD(msg);

    impl = calloc (1, sizeof (avisIterator));
    if (!impl) return (MAMA_STATUS_NOMEM);

    impl->mMsgIterator = (AttributesIter*) avis_emalloc(sizeof(AttributesIter));
    if (!impl->mMsgIterator) {
        free (impl);
        return (MAMA_STATUS_NOMEM);
    }

    if (!impl->mAvisField)
    {
        status = avismsgFieldPayload_create((msgFieldPayload*) &impl->mAvisField);
        if (status != MAMA_STATUS_OK)
            return status;
    }

    *iter = (msgPayloadIter) impl;
    return (status);
}

msgFieldPayload
avismsgPayloadIter_get          (msgPayloadIter  iter,
                                msgFieldPayload field,
                                msgPayload      msg)
{
	avisIterator* impl = (avisIterator*) iter;
    if (!iter || !msg || !field) return NULL;

    avisField(field)->mName = attributes_iter_name(impl->mMsgIterator);
    avisField(field)->mValue = attributes_iter_value(impl->mMsgIterator);

    if ((strcmp(SUBJECT_FIELD_NAME, avisField(field)->mName) == 0) ||
        (strcmp(INBOX_FIELD_NAME, avisField(field)->mName)== 0))
            return (avismsgPayloadIter_next(iter,field,msg));

    return field;
}

msgFieldPayload
avismsgPayloadIter_next          (msgPayloadIter  iter,
                                msgFieldPayload field,
                                msgPayload      msg)
{
	avisIterator* impl = (avisIterator*) iter;
    if (!iter || !msg || !field) return NULL;

    if (!attributes_iter_next(impl->mMsgIterator))
        return NULL;

    return avismsgPayloadIter_get(iter, impl->mAvisField, msg);
}

mama_bool_t
avismsgPayloadIter_hasNext       (msgPayloadIter iter,
                                msgPayload     msg)
{
    avisIterator* impl = (avisIterator*) iter;
    if (!impl) return false;

    return attributes_iter_has_next(impl->mMsgIterator);
}

msgFieldPayload
avismsgPayloadIter_begin         (msgPayloadIter  iter,
                                msgFieldPayload field,
                                msgPayload      msg)
{
    avisIterator* impl = (avisIterator*) iter;
    if (!impl) return NULL;

    attributes_iter_init(impl->mMsgIterator, impl->mAvisMsg);
    return avismsgPayloadIter_get(iter, impl->mAvisField, msg);
}

msgFieldPayload
avismsgPayloadIter_end           (msgPayloadIter iter,
                                msgPayload     msg)
{
    return NULL;
}

mama_status
avismsgPayloadIter_associate      (msgPayloadIter iter,
                                 msgPayload     msg)
{
    avisIterator* impl = (avisIterator*) iter;
    CHECK_PAYLOAD(msg);

    if (!impl) return MAMA_STATUS_NULL_ARG;

    attributes_iter_init(impl->mMsgIterator, avisPayload(msg));
    impl->mAvisMsg = avisPayload(msg);
    return MAMA_STATUS_OK;
}

mama_status
avismsgPayloadIter_destroy       (msgPayloadIter iter)
{
    avisIterator* impl = (avisIterator*) iter;
    if (!impl) return MAMA_STATUS_NULL_ARG;

    if (impl->mMsgIterator)
       attributes_iter_destroy(impl->mMsgIterator);
    free(impl);

    return MAMA_STATUS_OK;
}

/******************************************************************************
* general field functions
*******************************************************************************/
mama_status
avismsgFieldPayload_create      (msgFieldPayload*   field)
{
    avisFieldPayload* impl = calloc (1, sizeof(avisFieldPayload));

    *field = (msgFieldPayload) impl;
    if (impl == NULL) return MAMA_STATUS_NOMEM;

    return MAMA_STATUS_OK;
}

mama_status
avismsgFieldPayload_destroy      (msgFieldPayload   field)
{
    CHECK_FIELD(field);

    free(avisField(field));
    return MAMA_STATUS_OK;
}

mama_status
avismsgFieldPayload_getName      (const msgFieldPayload   field,
                                mamaDictionary          dict,
                                mamaFieldDescriptor     desc,
                                const char**            result)
{
	uint16_t fid =0;
    CHECK_FIELD(field);

    fid = atoi(avisField(field)->mName);
    if (fid!=0)
    {
    	if (!desc)
		{
    		if (dict)
			{
				mama_status status = MAMA_STATUS_OK;
				if(MAMA_STATUS_OK!=
						(status=mamaDictionary_getFieldDescriptorByFid
									(dict,
									 &desc,
									 fid)))
				{
					return status;
				}
				*result = mamaFieldDescriptor_getName (desc);
			}
			else
			{
				*result = avisField(field)->mName;
			}
		}
    	else
    		*result = mamaFieldDescriptor_getName (desc);
    }
    else
    	*result = avisField(field)->mName;

    if (!*result)
        return MAMA_STATUS_INVALID_ARG;

    return MAMA_STATUS_OK;
}

mama_status
avismsgFieldPayload_getFid       (const msgFieldPayload   field,
                                mamaDictionary          dict,
                                mamaFieldDescriptor     desc,
                                uint16_t*               result)
{
	uint16_t fid =0;
    CHECK_FIELD(field);

    fid = atoi(avisField(field)->mName);
    if (fid==0)
    {
    	if (!desc)
		{
    		if (dict)
			{
				mama_status status = MAMA_STATUS_OK;
				if(MAMA_STATUS_OK!=
						(status=mamaDictionary_getFieldDescriptorByName
									(dict,
									 &desc,
									 avisField(field)->mName)))
				{
					return status;
				}
				*result = mamaFieldDescriptor_getFid (desc);
			}
			else
			{
				*result = 0;
			}
		}
    	else
    		*result = mamaFieldDescriptor_getFid (desc);
    }
    else
        *result = fid;

    return MAMA_STATUS_OK;
}


mama_status
avismsgFieldPayload_getDescriptor(const msgFieldPayload   field,
                                mamaDictionary          dict,
                                mamaFieldDescriptor*    result)
{
    uint16_t fid = 0;
    CHECK_FIELD(field);
    if ((avisField(field)->mName == 0) || (strlen(avisField(field)->mName) == 0))
        return MAMA_STATUS_INVALID_ARG;

    fid = atoi(avisField(field)->mName);

    if (fid != 0)
        return mamaDictionary_getFieldDescriptorByFid(dict, result, fid);
    else
    return mamaDictionary_getFieldDescriptorByName(dict, result, avisField(field)->mName);
}


mama_status
avismsgFieldPayload_getType      (msgFieldPayload         field,
                                mamaFieldType*          result)
{
    CHECK_FIELD(field);
    if (avisField(field)->mValue == 0)
        return MAMA_STATUS_INVALID_ARG;

    *result = avis2MamaType(avisField(field)->mValue->type);
    if (*result == MAMA_FIELD_TYPE_UNKNOWN)
        return MAMA_STATUS_INVALID_ARG;
    return MAMA_STATUS_OK;
}

/******************************************************************************
* field update functions
*******************************************************************************/
mama_status
avismsgFieldPayload_updateBool   (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_bool_t             value)
{
    return avisMsg_setBool(avisPayload(msg), avisField(field)->mName, 0, value);
}

mama_status
avismsgFieldPayload_updateChar   (msgFieldPayload         field,
                                msgPayload              msg,
                                char                    value)
{
    return avisMsg_setChar(avisPayload(msg), avisField(field)->mName, 0, value);
}

mama_status
avismsgFieldPayload_updateU8     (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_u8_t               value)
{
    return avisMsg_setU8(avisPayload(msg), avisField(field)->mName, 0, value);
}

mama_status
avismsgFieldPayload_updateI8     (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_i8_t               value)
{
    return avisMsg_setI8(avisPayload(msg), avisField(field)->mName, 0, value);
}

mama_status
avismsgFieldPayload_updateI16    (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_i16_t              value)
{
    return avisMsg_setI16(avisPayload(msg), avisField(field)->mName, 0, value);
}

mama_status
avismsgFieldPayload_updateU16    (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_u16_t              value)
{
    return avisMsg_setU16(avisPayload(msg), avisField(field)->mName, 0, value);
}

mama_status
avismsgFieldPayload_updateI32    (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_i32_t              value)
{
    return avisMsg_setI32(avisPayload(msg), avisField(field)->mName, 0, value);
}

mama_status
avismsgFieldPayload_updateU32    (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_u32_t              value)
{
    return avisMsg_setU32(avisPayload(msg), avisField(field)->mName, 0, value);
}

mama_status
avismsgFieldPayload_updateI64    (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_i64_t              value)
{
    return avisMsg_setI64(avisPayload(msg), avisField(field)->mName, 0, value);
}

mama_status
avismsgFieldPayload_updateU64    (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_u64_t              value)
{
    return avisMsg_setU64(avisPayload(msg), avisField(field)->mName, 0, value);
}

mama_status
avismsgFieldPayload_updateF32    (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_f32_t              value)
{
    return avisMsg_setF32(avisPayload(msg), avisField(field)->mName, 0, value);
}

mama_status
avismsgFieldPayload_updateF64    (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_f64_t              value)
{
    return avisMsg_setF64(avisPayload(msg), avisField(field)->mName, 0, value);
}

mama_status
avismsgFieldPayload_updateDateTime
                               (msgFieldPayload         field,
                                msgPayload              msg,
                                const mamaDateTime      value)
{
    return avisMsg_setDateTime(avisPayload(msg), avisField(field)->mName, 0, value);
}

mama_status
avismsgFieldPayload_updatePrice  (msgFieldPayload         field,
                                msgPayload              msg,
                                const mamaPrice         value)
{
    return avisMsg_setPrice(avisPayload(msg), avisField(field)->mName, 0, value);
}


/******************************************************************************
* field get functions
*******************************************************************************/
mama_status
avismsgFieldPayload_getBool      (const msgFieldPayload   field,
                                mama_bool_t*            result)
{
    return avisValue_getBool(avisField(field)->mValue, result);
}

mama_status
avismsgFieldPayload_getChar      (const msgFieldPayload   field,
                                char*                   result)
{
    return avisValue_getChar(avisField(field)->mValue, result);
}

mama_status
avismsgFieldPayload_getI8        (const msgFieldPayload   field,
                                mama_i8_t*              result)
{
    return avisValue_getI8(avisField(field)->mValue, result);
}

mama_status
avismsgFieldPayload_getU8        (const msgFieldPayload   field,
                                mama_u8_t*              result)
{
    return avisValue_getU8(avisField(field)->mValue, result);
}

mama_status
avismsgFieldPayload_getI16       (const msgFieldPayload   field,
                                mama_i16_t*             result)
{
    return avisValue_getI16(avisField(field)->mValue, result);
}

mama_status
avismsgFieldPayload_getU16       (const msgFieldPayload   field,
                                mama_u16_t*             result)
{
    return avisValue_getU16(avisField(field)->mValue, result);
}

mama_status
avismsgFieldPayload_getI32       (const msgFieldPayload   field,
                                mama_i32_t*             result)
{
    return avisValue_getI32(avisField(field)->mValue, result);
}

mama_status
avismsgFieldPayload_getU32       (const msgFieldPayload   field,
                                mama_u32_t*             result)
{
    return avisValue_getU32(avisField(field)->mValue, result);
}

mama_status
avismsgFieldPayload_getI64       (const msgFieldPayload   field,
                                mama_i64_t*             result)
{
    return avisValue_getI64(avisField(field)->mValue, result);
}

mama_status
avismsgFieldPayload_getU64       (const msgFieldPayload   field,
                                mama_u64_t*             result)
{
    return avisValue_getU64(avisField(field)->mValue, result);
}

mama_status
avismsgFieldPayload_getF32       (const msgFieldPayload   field,
                                mama_f32_t*             result)
{
    return avisValue_getF32(avisField(field)->mValue, result);
}

mama_status
avismsgFieldPayload_getF64       (const msgFieldPayload   field,
                                mama_f64_t*             result)
{
    return avisValue_getF64(avisField(field)->mValue, result);
}

mama_status
avismsgFieldPayload_getString    (const msgFieldPayload   field,
                                const char**            result)
{
    return avisValue_getString(avisField(field)->mValue, result);
}

mama_status
avismsgFieldPayload_getOpaque    (const msgFieldPayload   field,
                                const void**            result,
                                mama_size_t*            size)
{
    return avisValue_getOpaque(avisField(field)->mValue, result, size);
}

mama_status
avismsgFieldPayload_getDateTime  (const msgFieldPayload   field,
                                mamaDateTime            result)
{
    return avisValue_getDateTime(avisField(field)->mValue, result);
}

mama_status
avismsgFieldPayload_getPrice     (const msgFieldPayload   field,
                                mamaPrice               result)
{
    return avisValue_getPrice(avisField(field)->mValue, result);
}

mama_status
avismsgFieldPayload_getMsg       (const msgFieldPayload   field,
                                msgPayload*             result)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avismsgFieldPayload_getVectorBool(const msgFieldPayload   field,
                                const mama_bool_t**     result,
                                mama_size_t*            size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avismsgFieldPayload_getVectorChar(const msgFieldPayload   field,
                                const char**            result,
                                mama_size_t*            size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avismsgFieldPayload_getVectorI8  (const msgFieldPayload   field,
                                const mama_i8_t**       result,
                                mama_size_t*            size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avismsgFieldPayload_getVectorU8  (const msgFieldPayload   field,
                                const mama_u8_t**       result,
                                mama_size_t*            size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avismsgFieldPayload_getVectorI16 (const msgFieldPayload   field,
                                const mama_i16_t**      result,
                                mama_size_t*            size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avismsgFieldPayload_getVectorU16 (const msgFieldPayload   field,
                                const mama_u16_t**      result,
                                mama_size_t*            size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avismsgFieldPayload_getVectorI32 (const msgFieldPayload   field,
                                const mama_i32_t**      result,
                                mama_size_t*            size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avismsgFieldPayload_getVectorU32 (const msgFieldPayload   field,
                                const mama_u32_t**      result,
                                mama_size_t*            size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avismsgFieldPayload_getVectorI64 (const msgFieldPayload   field,
                                const mama_i64_t**      result,
                                mama_size_t*            size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avismsgFieldPayload_getVectorU64 (const msgFieldPayload   field,
                                const mama_u64_t**      result,
                                mama_size_t*            size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

 mama_status
avismsgFieldPayload_getVectorF32 (const msgFieldPayload   field,
                                const mama_f32_t**      result,
                                mama_size_t*            size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avismsgFieldPayload_getVectorF64 (const msgFieldPayload   field,
                                const mama_f64_t**      result,
                                mama_size_t*            size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avismsgFieldPayload_getVectorString
                               (const msgFieldPayload   field,
                                const char***           result,
                                mama_size_t*            size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avismsgFieldPayload_getVectorDateTime
                               (const msgFieldPayload   field,
                                const mamaDateTime*     result,
                                mama_size_t*            size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avismsgFieldPayload_getVectorPrice
                               (const msgFieldPayload   field,
                                const mamaPrice*        result,
                                mama_size_t*            size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avismsgFieldPayload_getVectorMsg (const msgFieldPayload   field,
                                const msgPayload**      result,
                                mama_size_t*            size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avismsgFieldPayload_getAsString (
    const msgFieldPayload   field,
    const msgPayload   msg,
    char*         buf,
    mama_size_t   len)
{
    return avisValue_getFieldAsString(avisField(field)->mValue, avisField(field)->mName, 0, buf, len);
}
