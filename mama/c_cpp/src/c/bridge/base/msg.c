/*=========================================================================
  =                             Includes                                  =
  =========================================================================*/

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <mama/mama.h>
#include <mama/integration/bridge/base.h>
#include <msgimpl.h>
#include <wombat/memnode.h>
#include "basedefs.h"


/*=========================================================================
  =                              Macros                                   =
  =========================================================================*/

#define     BRIDGE_MSG_PROPERTY_LEN     1024


/*=========================================================================
  =                Typedefs, structs, enums and globals                   =
  =========================================================================*/

typedef struct baseBridgeMsgReplyHandle
{
    char                        mInboxName[BRIDGE_MSG_PROPERTY_LEN];
    char                        mReplyTo[BRIDGE_MSG_PROPERTY_LEN];
} baseBridgeMsgReplyHandle;

typedef struct baseBridgeMsgImpl
{
    mamaMsg                     mParent;
    baseMsgType                 mMsgType;
    uint8_t                     mIsValid;
    baseBridgeMsgReplyHandle    mReplyHandle;
    char                        mTargetSubject[BRIDGE_MSG_PROPERTY_LEN];
    char                        mSendSubject[BRIDGE_MSG_PROPERTY_LEN];
    char                        mDestination[BRIDGE_MSG_PROPERTY_LEN];
    void*                       mSerializedBuffer;
    size_t                      mSerializedBufferSize;
    size_t                      mPayloadSize;
} baseBridgeMsgImpl;


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
static mama_status baseBridgeMamaMsgImpl_setStringValue (char*          dest,
                                                         const char*    value);


/*=========================================================================
  =              Public interface implementation functions                =
  =========================================================================*/

/* Bridge specific implementations below here */
mama_status
baseBridgeMamaMsg_create (msgBridge* msg, mamaMsg parent)
{
    baseBridgeMsgImpl* impl   = NULL;
    mama_status        status = MAMA_STATUS_OK;

    if (NULL == msg || NULL == parent)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    status = baseBridgeMamaMsgImpl_createMsgOnly (msg);
    if (MAMA_STATUS_OK != status)
    {
    	return status;
    }

    /* Cast back to implementation to set parent */
    impl = (baseBridgeMsgImpl*) *msg;
    impl->mParent       = parent;

    return MAMA_STATUS_OK;
}

int
baseBridgeMamaMsg_isFromInbox (msgBridge msg)
{
    if (NULL == msg)
    {
        return -1;
    }
    if (BASE_MSG_INBOX_REQUEST == ((baseBridgeMsgImpl*)msg)->mMsgType)
    {
        return 1;
    }

    return 0;
}

mama_status
baseBridgeMamaMsg_destroy (msgBridge msg, int destroyMsg)
{
    if (NULL == msg)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    /* Free the underlying implementation */
    free (msg);

    return MAMA_STATUS_OK;
}

mama_status
baseBridgeMamaMsg_destroyMiddlewareMsg (msgBridge msg)
{
    /*
     * The bridge message is never responsible for the memory associated with
     * the underlying middleware message (it's owned by publishers and
     * transports) so no need to do anything here
     */
    if (NULL == msg)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    return MAMA_STATUS_OK;
}

mama_status
baseBridgeMamaMsg_detach (msgBridge msg)
{
    /*
     * The bridge message is never responsible for the memory associated with
     * the underlying middleware message (it's owned by publishers and
     * transports) so no need to do anything here
     */
    if (NULL == msg)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    return MAMA_STATUS_OK;
}

mama_status
baseBridgeMamaMsg_getPlatformError (msgBridge msg, void** error)
{
    /* Null initialize the error return */
    if (NULL != error)
    {
        *error  = NULL;
    }
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
baseBridgeMamaMsg_setSendSubject (msgBridge   msg,
                                  const char* symbol,
                                  const char* subject)
{
    baseBridgeMsgImpl* impl     = (baseBridgeMsgImpl*) msg;
    mama_status        status   = MAMA_STATUS_OK;

    if (NULL == impl || NULL == symbol)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    status = baseBridgeMamaMsgImpl_setStringValue (impl->mSendSubject, symbol);
    if (MAMA_STATUS_OK != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "baseBridgeMamaMsg_setSendSubject(): "
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
baseBridgeMamaMsg_getNativeHandle (msgBridge msg, void** result)
{
    baseBridgeMsgImpl* impl = (baseBridgeMsgImpl*) msg;
    if (NULL == impl || NULL == result)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    *result = impl;
    return MAMA_STATUS_OK;
}

mama_status
baseBridgeMamaMsg_duplicateReplyHandle (msgBridge msg, void** result)
{
    baseBridgeMsgImpl*          impl        = (baseBridgeMsgImpl*) msg;
    baseBridgeMsgReplyHandle*   replyHandle = NULL;

    if (NULL == impl || NULL == result)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    replyHandle = (baseBridgeMsgReplyHandle*) calloc (1,
		                  sizeof (struct baseBridgeMsgReplyHandle));

    if (NULL == replyHandle)
    {
        return MAMA_STATUS_NOMEM;
    }

    baseBridgeMamaMsgReplyHandleImpl_setReplyTo (
            replyHandle,
            impl->mReplyHandle.mReplyTo);

    baseBridgeMamaMsgReplyHandleImpl_setInboxName (
            replyHandle,
            impl->mReplyHandle.mInboxName);

    *result = replyHandle;

    return MAMA_STATUS_OK;
}

mama_status
baseBridgeMamaMsg_copyReplyHandle (void* src, void** dest)
{
    baseBridgeMsgReplyHandle* impl        = (baseBridgeMsgReplyHandle*)src;
    baseBridgeMsgReplyHandle* replyHandle = NULL;

    if (NULL == impl || NULL == dest)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    replyHandle = (baseBridgeMsgReplyHandle*) calloc (1,
		                  sizeof (struct baseBridgeMsgReplyHandle));

    if (NULL == replyHandle)
    {
        return MAMA_STATUS_NOMEM;
    }

    baseBridgeMamaMsgReplyHandleImpl_setReplyTo   (replyHandle,
                                                   impl->mReplyTo);

    baseBridgeMamaMsgReplyHandleImpl_setInboxName (replyHandle,
                                                   impl->mInboxName);

    *dest = replyHandle;

    return MAMA_STATUS_OK;
}

mama_status
baseBridgeMamaMsg_destroyReplyHandle (void* result)
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
baseBridgeMamaMsgImpl_setReplyHandle (msgBridge msg, void* handle)
{
    /* Do we assume we now own the memory of the handle here? Should we
     * free it when we have copied? Or indeed, do we need the copy? */
    baseBridgeMsgImpl*        impl        = (baseBridgeMsgImpl*) msg;
    baseBridgeMsgReplyHandle* replyHandle = (baseBridgeMsgReplyHandle*) handle;

    if (NULL == impl || NULL == replyHandle)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    baseBridgeMamaMsgReplyHandleImpl_setReplyTo   (&impl->mReplyHandle,
                                                   replyHandle->mReplyTo);

    baseBridgeMamaMsgReplyHandleImpl_setInboxName (&impl->mReplyHandle,
                                                   replyHandle->mInboxName);

    return MAMA_STATUS_OK;
}

mama_status
baseBridgeMamaMsgImpl_setReplyHandleAndIncrement (msgBridge msg, void* result)
{
    return baseBridgeMamaMsgImpl_setReplyHandle (msg, result);
}


/*=========================================================================
  =                  Public implementation functions                      =
  =========================================================================*/

mama_status
baseBridgeMamaMsgImpl_isValid (msgBridge    msg,
                               uint8_t*     result)
{
    baseBridgeMsgImpl* impl   = (baseBridgeMsgImpl*) msg;
    if (NULL == impl)
    {
        *result = 0;
        return MAMA_STATUS_NULL_ARG;
    }

    *result = impl->mIsValid;
    return MAMA_STATUS_OK;
}

mama_status
baseBridgeMamaMsgImpl_setMsgType (msgBridge     msg,
                                  baseMsgType   type)
{
    baseBridgeMsgImpl*  impl        = (baseBridgeMsgImpl*) msg;

    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    impl->mMsgType = type;
    return MAMA_STATUS_OK;
}

mama_status
baseBridgeMamaMsgImpl_getMsgType (msgBridge     msg,
                                  baseMsgType*  type)
{
    baseBridgeMsgImpl*  impl        = (baseBridgeMsgImpl*) msg;

    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    *type = impl->mMsgType;
    return MAMA_STATUS_OK;
}

mama_status
baseBridgeMamaMsgImpl_setInboxName (msgBridge   msg,
                                    const char* value)
{
    baseBridgeMsgImpl*  impl        = (baseBridgeMsgImpl*) msg;

    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    return baseBridgeMamaMsgReplyHandleImpl_setInboxName (
                   (void*) &impl->mReplyHandle,
                   value);
}

mama_status
baseBridgeMamaMsgImpl_getInboxName (msgBridge   msg,
                                    char**      value)
{
    baseBridgeMsgImpl*  impl        = (baseBridgeMsgImpl*) msg;

    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    return baseBridgeMamaMsgReplyHandleImpl_getInboxName (
                   (void*) &impl->mReplyHandle,
                   value);
}

mama_status
baseBridgeMamaMsgImpl_setReplyTo (msgBridge     msg,
                                  const char*   value)
{
    baseBridgeMsgImpl*  impl        = (baseBridgeMsgImpl*) msg;

    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    return baseBridgeMamaMsgReplyHandleImpl_setReplyTo (
                   (void*) &impl->mReplyHandle,
                   value);
}

mama_status
baseBridgeMamaMsgImpl_getReplyTo (msgBridge     msg,
                                  char**        value)
{
    baseBridgeMsgImpl*  impl        = (baseBridgeMsgImpl*) msg;

    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    return baseBridgeMamaMsgReplyHandleImpl_getReplyTo (
                   (void*) &impl->mReplyHandle,
                   value);
}

mama_status
baseBridgeMamaMsgImpl_setTargetSubject (msgBridge   msg,
                                        const char* value)
{
    baseBridgeMsgImpl*  impl        = (baseBridgeMsgImpl*) msg;

    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    return baseBridgeMamaMsgImpl_setStringValue (impl->mTargetSubject,
                                                 value);
}

mama_status
baseBridgeMamaMsgImpl_getTargetSubject (msgBridge   msg,
                                        char**      value)
{
    baseBridgeMsgImpl*  impl        = (baseBridgeMsgImpl*) msg;

    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    *value = impl->mTargetSubject;
    return MAMA_STATUS_OK;
}

mama_status
baseBridgeMamaMsgImpl_setDestination (msgBridge     msg,
                                      const char*   value)
{
    baseBridgeMsgImpl*  impl        = (baseBridgeMsgImpl*) msg;

    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    return baseBridgeMamaMsgImpl_setStringValue (impl->mDestination,
                                                 value);
}

mama_status
baseBridgeMamaMsgImpl_getDestination (msgBridge     msg,
                                      char**        value)
{
    baseBridgeMsgImpl*  impl        = (baseBridgeMsgImpl*) msg;

    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    *value = impl->mDestination;
    return MAMA_STATUS_OK;
}

mama_status
baseBridgeMamaMsgImpl_getSendSubject (msgBridge     msg,
                                      char**        value)
{
    baseBridgeMsgImpl*  impl        = (baseBridgeMsgImpl*) msg;

    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    *value = impl->mSendSubject;
    return MAMA_STATUS_OK;
}

mama_status
baseBridgeMamaMsgReplyHandleImpl_getInboxName (void*  replyHandle,
                                               char** value)
{
    baseBridgeMsgReplyHandle* impl = (baseBridgeMsgReplyHandle*)replyHandle;

    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    *value = impl->mInboxName;
    return MAMA_STATUS_OK;
}

mama_status
baseBridgeMamaMsgReplyHandleImpl_setInboxName (
                    void*       replyHandle,
                    const char* value)
{
    baseBridgeMsgReplyHandle*  impl   = (baseBridgeMsgReplyHandle*)replyHandle;

    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    return baseBridgeMamaMsgImpl_setStringValue (impl->mInboxName,
                                                 value);
}

mama_status
baseBridgeMamaMsgImpl_getReplyHandle (
        msgBridge     msg,
        void**       replyHandle)
{
    baseBridgeMsgImpl*  impl        = (baseBridgeMsgImpl*) msg;

    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    *replyHandle = &impl->mReplyHandle;
    return MAMA_STATUS_OK;
}

mama_status
baseBridgeMamaMsgReplyHandleImpl_getReplyTo (void*  replyHandle,
                                                         char** value)
{
    baseBridgeMsgReplyHandle* impl = (baseBridgeMsgReplyHandle*) replyHandle;

    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    *value = impl->mReplyTo;
    return MAMA_STATUS_OK;
}

mama_status
baseBridgeMamaMsgReplyHandleImpl_setReplyTo (
                    void*           replyHandle,
                    const char*     value)
{
    baseBridgeMsgReplyHandle*  impl   = (baseBridgeMsgReplyHandle*) replyHandle;

    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    return baseBridgeMamaMsgImpl_setStringValue (impl->mReplyTo, value);
}

/* Non-interface version of create which permits null parent */
mama_status
baseBridgeMamaMsgImpl_createMsgOnly (msgBridge* msg)
{
    baseBridgeMsgImpl* impl = NULL;

    if (NULL == msg)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Null initialize the msgBridge pointer */
    *msg = NULL;

    /* Allocate memory for the implementation struct */
    impl = (baseBridgeMsgImpl*) calloc (1, sizeof (baseBridgeMsgImpl));
    if (NULL == impl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "baseBridgeMamaMsg_create (): "
                  "Failed to allocate memory for bridge message.");
        return MAMA_STATUS_NOMEM;
    }

    /* Back reference the parent message */
    impl->mIsValid      = 1;

    /* Populate the msgBridge pointer with the implementation */
    *msg = (msgBridge) impl;

    return MAMA_STATUS_OK;
}

mama_status
baseBridgeMamaMsgImpl_getSerializationBuffer (msgBridge    msg,
                                              void**       buffer,
                                              size_t       size)
{
    baseBridgeMsgImpl*  impl        = (baseBridgeMsgImpl*) msg;

    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Allocate more memory if necessary */
    allocateBufferMemory (&impl->mSerializedBuffer,
                          &impl->mSerializedBufferSize,
                          size);

    if (size > impl->mSerializedBufferSize)
    {
        *buffer = NULL;
        return MAMA_STATUS_NOMEM;
    }
    else
    {
        *buffer = impl->mSerializedBuffer;
        return MAMA_STATUS_OK;
    }

}

mama_status
baseBridgeMamaMsgImpl_getPayloadSize (msgBridge    msg,
                                      size_t*      size)
{
    baseBridgeMsgImpl*  impl        = (baseBridgeMsgImpl*) msg;

    if (NULL == impl || NULL == size)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    *size = impl->mPayloadSize;
}

mama_status
baseBridgeMamaMsgImpl_setPayloadSize (msgBridge    msg,
                                      size_t       size)
{
    baseBridgeMsgImpl*  impl        = (baseBridgeMsgImpl*) msg;

    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    impl->mPayloadSize = size;
}

/*=========================================================================
  =                  Private implementation functions                     =
  =========================================================================*/

mama_status
baseBridgeMamaMsgImpl_setStringValue (char*         dest,
                                      const char*   value)
{
    strncpy (dest, value, BRIDGE_MSG_PROPERTY_LEN);

    /* ISO C - remaining bytes from strncpy are null unless overrun occurred */
    if (dest[BRIDGE_MSG_PROPERTY_LEN - 1] != '\0')
    {
        /* Terminate string to at least make it usable (though truncated) */
        dest[BRIDGE_MSG_PROPERTY_LEN - 1] = '\0';
        mama_log (MAMA_LOG_LEVEL_WARN,
                  "baseBridgeMamaMsgImpl_setStringValue(): "
                  "Unable to set value '%s': Property too long for buffer. ",
                  "Truncated to '%s'",
                  value,
                  dest);
        return MAMA_STATUS_PROPERTY_TOO_LONG;
    }
    return MAMA_STATUS_OK;
}
