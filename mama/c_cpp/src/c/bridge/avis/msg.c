/* $Id: msg.c,v 1.1.2.8 2011/10/10 16:02:16 emmapollock Exp $
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

#include "wombat/port.h"
#include <stdlib.h>
#include <string.h>

#include <avis/elvin.h>

#include <mama/mama.h>
#include <msgimpl.h>
#include "avisdefs.h"
#include "avisbridgefunctions.h"


typedef struct avisMsgImpl
{
    Attributes*     mAvisMsg;
    mamaMsg         mParent;
    bool            mSecure;
} avisMsgImpl;

#define avisMsg(msg) ((avisMsgImpl*)(msg))
#define CHECK_MSG(msg) \
        do {  \
           if ((avisMsg(msg)) == 0) return MAMA_STATUS_NULL_ARG; \
           if ((avisMsg(msg))->mAvisMsg == 0) return MAMA_STATUS_INVALID_ARG; \
         } while(0)


/******************************************************************************
 * Message Creation
 */

mama_status
avisBridgeMamaMsg_create (msgBridge* msg, mamaMsg parent)
{
    avisMsgImpl* impl;
    if (avisMsg(msg) == NULL) return MAMA_STATUS_NULL_ARG;
    *msg = NULL;

    impl = (avisMsgImpl*) calloc(1, sizeof(avisMsgImpl));
    if (!impl) return MAMA_STATUS_NOMEM;

    mamaMsg_getNativeMsg(parent, (void**)&impl->mAvisMsg);
    impl->mParent = parent;
    *msg = (msgBridge) impl;
    return MAMA_STATUS_OK;
}


mama_status
avisBridgeMamaMsg_destroy (msgBridge msg, int destroyMsg)
{
    CHECK_MSG(msg);
    if (destroyMsg)
    {
       avisBridgeMamaMsg_destroyMiddlewareMsg(msg);
    }
    free(avisMsg(msg));

    return MAMA_STATUS_OK;
}

mama_status
avisBridgeMamaMsg_destroyMiddlewareMsg (msgBridge msg)
{
    CHECK_MSG(msg);
    attributes_destroy(avisMsg(msg)->mAvisMsg);
    avisMsg(msg)->mAvisMsg = NULL;

    return MAMA_STATUS_OK;
}

mama_status
avisBridgeMamaMsg_detach (msgBridge msg)
{
    CHECK_MSG(msg);
    avisMsg(msg)->mAvisMsg = attributes_clone(avisMsg(msg)->mAvisMsg);

    return MAMA_STATUS_OK;
}

mama_status
avisBridgeMamaMsg_getPlatformError (msgBridge msg, void** error)
{
    if (error) *error  = NULL;
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisBridgeMamaMsgImpl_setReplyHandle (msgBridge msg, void* result)
{
    mama_status status = MAMA_STATUS_OK;
    CHECK_MSG(msg);
    if (MAMA_STATUS_OK != (status = mamaMsg_updateString(avisMsg(msg)->mParent, INBOX_FIELD_NAME, 0, (const char*) result))) {
        return status;
    }

    return MAMA_STATUS_OK;
}

mama_status
avisBridgeMamaMsgImpl_setReplyHandleAndIncrement (msgBridge msg, void* result)
{
    // NOTE: reply handles are not reference counted with avis -- they are simply strings
    return avisBridgeMamaMsgImpl_setReplyHandle(msg, result);
}

int
avisBridgeMamaMsg_isFromInbox (msgBridge msg)
{
    const char* dummy;
    return (mamaMsg_getString(avisMsg(msg)->mParent, INBOX_FIELD_NAME, 0, &dummy) == MAMA_STATUS_OK) ? 1 : 0;
}

mama_status
avisBridgeMamaMsg_setSendSubject (msgBridge   msg,
                                   const char* symbol,
                                   const char* subject)
{
    mama_status status = MAMA_STATUS_OK;
    CHECK_MSG(msg);

    if (MAMA_STATUS_OK != (status = mamaMsg_updateString(avisMsg(msg)->mParent, SUBJECT_FIELD_NAME, 0, subject))) {
        return status;
    }

    mamaMsg_updateString (avisMsg(msg)->mParent,
                              MamaFieldSubscSymbol.mName,
                              MamaFieldSubscSymbol.mFid,
                              symbol);

    return MAMA_STATUS_OK;
}

mama_status
avisBridgeMamaMsg_getNativeHandle (msgBridge msg, void** result)
{
    CHECK_MSG(msg);
    *result = avisMsg(msg)->mAvisMsg;
    return MAMA_STATUS_OK;
}

mama_status
avisBridgeMamaMsg_duplicateReplyHandle (msgBridge msg, void** result)
{
    const char* replyAddr;
    mama_status status = MAMA_STATUS_OK;
    CHECK_MSG(msg);
    if (MAMA_STATUS_OK != (status = mamaMsg_getString(avisMsg(msg)->mParent, INBOX_FIELD_NAME, 0, &replyAddr))) {
        return status;
    }

    *result = (void*) strdup(replyAddr);
    return MAMA_STATUS_OK;
}

mama_status
avisBridgeMamaMsg_copyReplyHandle (void* src, void** dest)
{
    const char* replyAddr = (const char*) src;
    *dest = (void*) strdup(replyAddr);
    return MAMA_STATUS_OK;
}

mama_status
avisBridgeMamaMsg_destroyReplyHandle (void* result)
{
    char* replyAddr = (char*) result;
    free(replyAddr);
    return MAMA_STATUS_OK;
}
mama_status
avisBridgeMamaMsgImpl_setAttributesAndSecure (msgBridge msg, void* attributes, uint8_t secure)
{
    avisMsg(msg)->mAvisMsg=(Attributes*)attributes;
    avisMsg(msg)->mSecure=secure;
    return MAMA_STATUS_OK;
}
