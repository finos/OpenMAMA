/* $Id$
 *
 * OpenMAMA: The open middleware agnostic messaging API
 * Copyright (C) 2011 NYSE Inc.
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


/*=========================================================================
  =                             Includes                                  =
  =========================================================================*/

#include <stdlib.h>
#include <string.h>
#include <mama/mama.h>
#include <msgimpl.h>
#include "qpiddefs.h"
#include "qpidbridgefunctions.h"
#include "transport.h"
#include "msg.h"


/*=========================================================================
  =                              Macros                                   =
  =========================================================================*/

#define     QPID_MSG_PROPERTY_LEN     1024


/*=========================================================================
  =                Typedefs, structs, enums and globals                   =
  =========================================================================*/

typedef struct qpidBridgeMsgReplyHandle
{
    char                        mInboxName[QPID_MSG_PROPERTY_LEN];
    char                        mReplyTo[QPID_MSG_PROPERTY_LEN];
} qpidBridgeMsgReplyHandle;

typedef struct qpidBridgeMsgImpl
{
    mamaMsg                     mParent;
    qpidMsgType                 mMsgType;
    uint8_t                     mIsValid;
    qpidBridgeMsgReplyHandle    mReplyHandle;
    char                        mTargetSubject[QPID_MSG_PROPERTY_LEN];
    char                        mSendSubject[QPID_MSG_PROPERTY_LEN];
    char                        mDestination[QPID_MSG_PROPERTY_LEN];
} qpidBridgeMsgImpl;


/*=========================================================================
  =                  Private implementation prototypes                    =
  =========================================================================*/

/**
 * This is a local convenience function to avoid duplicate code when copying
 * a provided string to a supplied buffer.
 *
 * @param dest  The destination for the string to be set to.
 * @param value The value to set the destination string to. Node that if this
 *              is longer than QPID_MSG_PROPERTY_LEN, an error will be returned.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
static mama_status qpidBridgeMamaMsgImpl_setStringValue (char*          dest,
                                                         const char*    value);


/*=========================================================================
  =              Public interface implementation functions                =
  =========================================================================*/

/* Bridge specific implementations below here */
mama_status
qpidBridgeMamaMsg_create (msgBridge* msg, mamaMsg parent)
{
    qpidBridgeMsgImpl* impl = NULL;

    if (NULL == msg)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Null initialize the msgBridge pointer */
    *msg = NULL;

    /* Allocate memory for the implementation struct */
    impl = (qpidBridgeMsgImpl*) calloc (1, sizeof (qpidBridgeMsgImpl));
    if (NULL == impl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "qpidBridgeMamaMsg_create (): "
                  "Failed to allocate memory for bridge message.");
        return MAMA_STATUS_NOMEM;
    }

    /* Back reference the parent message */
    impl->mParent       = parent;
    impl->mIsValid      = 1;

    /* Populate the msgBridge pointer with the implementation */
    *msg = (msgBridge) impl;

    return MAMA_STATUS_OK;
}

int
qpidBridgeMamaMsg_isFromInbox (msgBridge msg)
{
    if (NULL == msg)
    {
        return -1;
    }
    if (QPID_MSG_INBOX_REQUEST == ((qpidBridgeMsgImpl*)msg)->mMsgType)
    {
        return 1;
    }

    return 0;
}

mama_status
qpidBridgeMamaMsg_destroy (msgBridge msg, int destroyMsg)
{
    if (NULL == msg)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    /* Free the underlying implementation */
    free (msg);

    return MAMA_STATUS_OK;
}

mama_status
qpidBridgeMamaMsg_destroyMiddlewareMsg (msgBridge msg)
{
    /*
     * The bridge message is never responsible for the memory associated with
     * the underlying middleware message (it's owned by publishers and
     * transports) so no need to do anything here
     */
    return MAMA_STATUS_OK;
}

mama_status
qpidBridgeMamaMsg_detach (msgBridge msg)
{
    /*
     * The bridge message is never responsible for the memory associated with
     * the underlying middleware message (it's owned by publishers and
     * transports) so no need to do anything here
     */
    return MAMA_STATUS_OK;
}

mama_status
qpidBridgeMamaMsg_getPlatformError (msgBridge msg, void** error)
{
    /* Null initialize the error return */
    if (NULL != error)
    {
        *error  = NULL;
    }
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
qpidBridgeMamaMsg_setSendSubject (msgBridge   msg,
                                  const char* symbol,
                                  const char* subject)
{
    qpidBridgeMsgImpl* impl     = (qpidBridgeMsgImpl*) msg;
    mama_status        status   = MAMA_STATUS_OK;

    if (NULL == impl || NULL == symbol || (NULL == symbol && NULL == subject))
    {
        return MAMA_STATUS_NULL_ARG;
    }

    status = qpidBridgeMamaMsgImpl_setStringValue (impl->mSendSubject, symbol);
    if (MAMA_STATUS_OK != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "qpidBridgeMamaMsg_setSendSubject(): "
                  "Could not set send subject: %s",
                  mamaStatus_stringForStatus(status));
        return MAMA_STATUS_PLATFORM;
    }

    /* Update the MAMA message with the send subject if it has a parent */
    if (NULL != impl->mParent)
    {
        status = mamaMsg_updateString (impl->mParent,
                                       MamaFieldSubscSymbol.mName,
                                       MamaFieldSubscSymbol.mFid,
                                       symbol);
    }
    return status;
}

mama_status
qpidBridgeMamaMsg_getNativeHandle (msgBridge msg, void** result)
{
    qpidBridgeMsgImpl* impl = (qpidBridgeMsgImpl*) msg;
    if (NULL == impl || NULL == result)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    *result = impl;
    return MAMA_STATUS_OK;
}

mama_status
qpidBridgeMamaMsg_duplicateReplyHandle (msgBridge msg, void** result)
{
    qpidBridgeMsgImpl*          impl        = (qpidBridgeMsgImpl*) msg;
    qpidBridgeMsgReplyHandle*   replyHandle = NULL;

    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    replyHandle = (qpidBridgeMsgReplyHandle*) calloc (1,
		                  sizeof (struct qpidBridgeMsgReplyHandle));

    if (NULL == replyHandle)
    {
        return MAMA_STATUS_NOMEM;
    }

    qpidBridgeMamaMsgReplyHandleImpl_setReplyTo (
            replyHandle,
            impl->mReplyHandle.mReplyTo);

    qpidBridgeMamaMsgReplyHandleImpl_setInboxName (
            replyHandle,
            impl->mReplyHandle.mInboxName);

    *result = replyHandle;

    return MAMA_STATUS_OK;
}

mama_status
qpidBridgeMamaMsg_copyReplyHandle (void* src, void** dest)
{
    qpidBridgeMsgReplyHandle* impl        = (qpidBridgeMsgReplyHandle*)src;
    qpidBridgeMsgReplyHandle* replyHandle = NULL;

    if (NULL == impl || NULL == dest)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    replyHandle = (qpidBridgeMsgReplyHandle*) calloc (1,
		                  sizeof (struct qpidBridgeMsgReplyHandle));

    if (NULL == replyHandle)
    {
        return MAMA_STATUS_NOMEM;
    }

    qpidBridgeMamaMsgReplyHandleImpl_setReplyTo   (replyHandle,
                                                   impl->mReplyTo);

    qpidBridgeMamaMsgReplyHandleImpl_setInboxName (replyHandle,
                                                   impl->mInboxName);

    *dest = replyHandle;

    return MAMA_STATUS_OK;
}

mama_status
qpidBridgeMamaMsg_destroyReplyHandle (void* result)
{
    /* What do we do here if the replyHandle is attached to a message? */
    if (NULL == result)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    free (result);
    return MAMA_STATUS_OK;
}

mama_status
qpidBridgeMamaMsgImpl_setReplyHandle (msgBridge msg, void* handle)
{
    /* Do we assume we now own the memory of the handle here? Should we
     * free it when we have copied? Or indeed, do we need the copy? */
    qpidBridgeMsgImpl*        impl        = (qpidBridgeMsgImpl*) msg;
    qpidBridgeMsgReplyHandle* replyHandle = (qpidBridgeMsgReplyHandle*) handle;

    if (NULL == impl || NULL == replyHandle)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    qpidBridgeMamaMsgReplyHandleImpl_setReplyTo   (&impl->mReplyHandle,
                                                   replyHandle->mReplyTo);

    qpidBridgeMamaMsgReplyHandleImpl_setInboxName (&impl->mReplyHandle,
                                                   replyHandle->mInboxName);

    return MAMA_STATUS_OK;
}

mama_status
qpidBridgeMamaMsgImpl_setReplyHandleAndIncrement (msgBridge msg, void* result)
{
    return qpidBridgeMamaMsgImpl_setReplyHandle (msg, result);
}


/*=========================================================================
  =                  Public implementation functions                      =
  =========================================================================*/

mama_status qpidBridgeMamaMsgImpl_isValid (msgBridge    msg,
                                           uint8_t*     result)
{
    qpidBridgeMsgImpl* impl   = (qpidBridgeMsgImpl*) msg;
    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
        *result = 0;
    }

    *result = impl->mIsValid;
    return MAMA_STATUS_OK;
}

mama_status qpidBridgeMamaMsgImpl_setMsgType (msgBridge     msg,
                                              qpidMsgType   type)
{
    qpidBridgeMsgImpl*  impl        = (qpidBridgeMsgImpl*) msg;

    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    impl->mMsgType = type;
    return MAMA_STATUS_OK;
}

mama_status qpidBridgeMamaMsgImpl_getMsgType (msgBridge     msg,
                                              qpidMsgType*  type)
{
    qpidBridgeMsgImpl*  impl        = (qpidBridgeMsgImpl*) msg;

    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    *type = impl->mMsgType;
    return MAMA_STATUS_OK;
}

mama_status qpidBridgeMamaMsgImpl_setInboxName (msgBridge   msg,
                                                const char* value)
{
    qpidBridgeMsgImpl*  impl        = (qpidBridgeMsgImpl*) msg;

    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    return qpidBridgeMamaMsgReplyHandleImpl_setInboxName (
                   (void*) &impl->mReplyHandle,
                   value);
}

mama_status qpidBridgeMamaMsgImpl_getInboxName (msgBridge   msg,
                                                char**      value)
{
    qpidBridgeMsgImpl*  impl        = (qpidBridgeMsgImpl*) msg;

    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    return qpidBridgeMamaMsgReplyHandleImpl_getInboxName (
                   (void*) &impl->mReplyHandle,
                   value);
}

mama_status qpidBridgeMamaMsgImpl_setReplyTo (msgBridge     msg,
                                              const char*   value)
{
    qpidBridgeMsgImpl*  impl        = (qpidBridgeMsgImpl*) msg;

    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    return qpidBridgeMamaMsgReplyHandleImpl_setReplyTo (
                   (void*) &impl->mReplyHandle,
                   value);
}

mama_status qpidBridgeMamaMsgImpl_getReplyTo (msgBridge     msg,
                                              char**        value)
{
    qpidBridgeMsgImpl*  impl        = (qpidBridgeMsgImpl*) msg;

    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    return qpidBridgeMamaMsgReplyHandleImpl_getReplyTo (
                   (void*) &impl->mReplyHandle,
                   value);
}

mama_status qpidBridgeMamaMsgImpl_setTargetSubject (msgBridge   msg,
                                                    const char* value)
{
    qpidBridgeMsgImpl*  impl        = (qpidBridgeMsgImpl*) msg;

    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    return qpidBridgeMamaMsgImpl_setStringValue (impl->mTargetSubject,
                                                 value);
}

mama_status qpidBridgeMamaMsgImpl_getTargetSubject (msgBridge   msg,
                                                    char**      value)
{
    qpidBridgeMsgImpl*  impl        = (qpidBridgeMsgImpl*) msg;

    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    *value = impl->mTargetSubject;
    return MAMA_STATUS_OK;
}

mama_status qpidBridgeMamaMsgImpl_setDestination (msgBridge     msg,
                                                  const char*   value)
{
    qpidBridgeMsgImpl*  impl        = (qpidBridgeMsgImpl*) msg;

    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    return qpidBridgeMamaMsgImpl_setStringValue (impl->mDestination,
                                                 value);
}

mama_status qpidBridgeMamaMsgImpl_getDestination (msgBridge     msg,
                                                  char**        value)
{
    qpidBridgeMsgImpl*  impl        = (qpidBridgeMsgImpl*) msg;

    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    *value = impl->mDestination;
    return MAMA_STATUS_OK;
}

mama_status qpidBridgeMamaMsgImpl_getSendSubject (msgBridge     msg,
                                                  char**        value)
{
    qpidBridgeMsgImpl*  impl        = (qpidBridgeMsgImpl*) msg;

    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    *value = impl->mSendSubject;
    return MAMA_STATUS_OK;
}

mama_status qpidBridgeMamaMsgReplyHandleImpl_getInboxName (void*  replyHandle,
                                                           char** value)
{
    qpidBridgeMsgReplyHandle* impl = (qpidBridgeMsgReplyHandle*)replyHandle;

    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    *value = impl->mInboxName;
    return MAMA_STATUS_OK;
}

mama_status qpidBridgeMamaMsgReplyHandleImpl_setInboxName (
                    void*       replyHandle,
                    const char* value)
{
    qpidBridgeMsgReplyHandle*  impl   = (qpidBridgeMsgReplyHandle*)replyHandle;

    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    return qpidBridgeMamaMsgImpl_setStringValue (impl->mInboxName,
                                                 value);
}

mama_status qpidBridgeMamaMsgReplyHandleImpl_getReplyTo (void*  replyHandle,
                                                         char** value)
{
    qpidBridgeMsgReplyHandle* impl = (qpidBridgeMsgReplyHandle*) replyHandle;

    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    *value = impl->mReplyTo;
    return MAMA_STATUS_OK;
}

mama_status qpidBridgeMamaMsgReplyHandleImpl_setReplyTo (
                    void*           replyHandle,
                    const char*     value)
{
    qpidBridgeMsgReplyHandle*  impl   = (qpidBridgeMsgReplyHandle*) replyHandle;

    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    return qpidBridgeMamaMsgImpl_setStringValue (impl->mReplyTo, value);
}
/*=========================================================================
  =                  Private implementation functions                     =
  =========================================================================*/

mama_status qpidBridgeMamaMsgImpl_setStringValue (char*         dest,
                                                  const char*   value)
{
    strncpy (dest, value, QPID_MSG_PROPERTY_LEN);

    /* ISO C - remaining bytes from strncpy are null unless overrun occurred */
    if (dest[QPID_MSG_PROPERTY_LEN - 1] != '\0')
    {
        /* Terminate string to at least make it usable (though truncated) */
        dest[QPID_MSG_PROPERTY_LEN - 1] = '\0';
        mama_log (MAMA_LOG_LEVEL_WARN,
                  "qpidBridgeMamaMsgImpl_setStringValue(): "
                  "Unable to set value '%s': Property too long for buffer. ",
                  "Truncated to '%s'",
                  value,
                  dest);
        return MAMA_STATUS_PROPERTY_TOO_LONG;
    }
    return MAMA_STATUS_OK;
}
