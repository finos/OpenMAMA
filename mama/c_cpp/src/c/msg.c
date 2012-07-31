
/* $Id: msg.c,v 1.86.4.5.2.1.4.23 2011/10/02 19:02:17 ianbell Exp $
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
#include <assert.h>

#include "mama/mama.h"
#include "mama/dictionary.h"
#include "mama/reservedfields.h"
#include "mama/msgqualifier.h"

#include "bridge.h"
#include "payloadbridge.h"
#include "queueimpl.h"
#include "msgimpl.h"
#include "msgfieldimpl.h"
#include "mamainternal.h"

#include "list.h"

#include "wombat/wincompat.h"


#define MAX_SUBJECT 256

#define NOFID 0

static const int INCLUDE_FIELD_NAME = (1 << 4);
static const int INCLUDE_FIELD_ID   = (1 << 2);

typedef struct mamaMsgIteratorImpl
{
    mamaMsgField            mCurrentField;
    msgPayloadIter          mPayloadIter;
    mamaPayloadBridgeImpl*  mPayloadBridge;
    mamaPayloadType         mPayloadType;
} mamaMsgIteratorImpl;

/**
* The mamaMsg implementation.
*/
typedef struct mamaMsgImpl_
{
    char                    mSymbol[MAX_SUBJECT];
    char                    mSource[MAX_SUBJECT];
    char                    mSubject[MAX_SUBJECT];

    msgPayload              mPayload;
    msgPayload              mPayloads[MAMA_PAYLOAD_MAX];
    /* Set of get/set/update methods to use for a non wmsg payload */
    mamaPayloadBridgeImpl*  mPayloadBridge;

    mamaMsg*                mLastVectorMsg;
    mama_size_t             mLastVectorMsgLen;

    mamaMsg*                mLastVectorPayloadMsg;
    mama_size_t             mLastVectorPayloadMsgLen;

    wList                   mNestedMessages;
    struct mamaMsgImpl_*    mParent;
    mamaQueue               mQueue;

    /*Reuseable field object for performance iteration*/
    mamaMsgField            mCurrentField;
    mamaDateTime            mCurrentDateTime;

    /*Hold onto the bridge impl for later use*/
    mamaBridgeImpl*         mBridgeImpl;
    /*The bridge specific message*/
    msgBridge               mBridgeMessage;
    /*If we have detached the middleware message we will own it
     and are responsible for destroying it*/
    int                     mMessageOwner;

    /*The context if this is a msg from the dqStrategy cache*/
    mamaDqContext*          mDqStrategyContext;
    mamaMsgStatus           mStatus;

} mamaMsgImpl;

/*================================================================
  = Static function definition
  ===============================================================*/
static mama_status
mamaMsgImpl_destroyLastVectorMsg (mamaMsgImpl *impl);

/*=================================================================
  = Public functions - defined in mama/msg.h
  ================================================================*/
mama_status
mamaMsg_destroy (mamaMsg msg)
{
    mamaMsgImpl* impl = (mamaMsgImpl*)msg;

    if (!impl) return MAMA_STATUS_NULL_ARG;

    if (impl->mLastVectorMsg != NULL || impl->mLastVectorPayloadMsg != NULL)
    {
        mamaMsgImpl_destroyLastVectorMsg (impl);
    }

    if (impl->mPayloadBridge && impl->mMessageOwner)
    {
        if (MAMA_STATUS_OK != impl->mPayloadBridge->msgPayloadDestroy (impl->mPayload))
        {
            mama_log (MAMA_LOG_LEVEL_ERROR, "mamaMsg_destroy(): "
                     "Could not clear message payload.");
        }

        impl->mPayloadBridge = NULL;

        /*set mMessageOwner to zero now the payload has been destroyed to prevent
          us destroying the underlying message again in the bridge specific function*/
        impl->mMessageOwner = 0;
    }

    if (impl->mNestedMessages != NULL)
    {
        mamaMsg nested = NULL;
        while (NULL!=(nested=(mamaMsg)list_pop_front (impl->mNestedMessages)))
        {
            mamaMsg_destroy (nested);
            list_free_element (impl->mNestedMessages, nested);
        }
        list_destroy (impl->mNestedMessages, NULL, NULL);
    }
    if (impl->mBridgeMessage)
    {
        if (impl->mBridgeImpl)
        {
            /*Invoke the bridge specific destroy function*/
            impl->mBridgeImpl->bridgeMamaMsgDestroy (
                    impl->mBridgeMessage, impl->mMessageOwner);
        }
        else
        {
            mama_log (MAMA_LOG_LEVEL_ERROR, "mamaMsg_destroy(): "
                      "Cannot destroy bridge message. NULL bridge");
        }
        impl->mBridgeMessage = NULL;
    }

    /*Destroy the reuseable field object*/
    if (impl->mCurrentField)
    {
        mamaMsgField_destroy (impl->mCurrentField);
    }

    impl->mDqStrategyContext = NULL;

    if (impl->mParent == NULL) free (impl);

    return MAMA_STATUS_OK;
}

mama_status
mamaMsg_clear (mamaMsg msg)
{
    mamaMsgImpl*    impl    =   (mamaMsgImpl*)msg;

    if (!impl)return MAMA_STATUS_NULL_ARG;

    if (impl->mLastVectorMsg != NULL || impl->mLastVectorPayloadMsg != NULL)
    {
        mamaMsgImpl_destroyLastVectorMsg(impl);
    }

    if (impl->mPayloadBridge)
    {
        if (MAMA_STATUS_OK != impl->mPayloadBridge->msgPayloadClear (impl->mPayload))
        {
            mama_log (MAMA_LOG_LEVEL_ERROR, "mamaMsg_clear(): "
                     "Could not clear message payload.");
        }
    }

    if (impl->mNestedMessages != NULL)
    {
        mamaMsg nested = NULL;
        while (NULL != (nested = (mamaMsg)list_pop_front (impl->mNestedMessages)))
        {
            mamaMsg_destroy (nested);
            list_free_element (impl->mNestedMessages, nested);
        }
        list_destroy (impl->mNestedMessages, NULL, NULL);
    }

    return MAMA_STATUS_OK;
}

mama_status
mamaMsg_detach (mamaMsg msg)
{
    mamaMsgImpl*    impl    =   (mamaMsgImpl*)msg;
    mama_status     status  =   MAMA_STATUS_OK;
    msgPayload      payload = NULL;

    if (!impl)              return MAMA_STATUS_NULL_ARG;
    if (!impl->mQueue)      return MAMA_STATUS_INVALID_QUEUE;
    if (!impl->mBridgeImpl) return MAMA_STATUS_NO_BRIDGE_IMPL;

    if (MAMA_STATUS_OK!=(status=mamaQueueImpl_detachMsg (impl->mQueue, msg)))
    {
        /*Message already logged in mamaQueueImpl_detachMsg*/
        return status;
    }
    
    /*We also need to detach the middleware bridge specific message*/
    if (MAMA_STATUS_OK!=(status=impl->mBridgeImpl->bridgeMamaMsgDetach
                (impl->mBridgeMessage)))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaMsg_detach(): "
                  "Could not detach bridge message.");
        return status;
    }
    /* Copy the payload */
    if (MAMA_STATUS_OK != (status =
       (msg->mPayloadBridge->msgPayloadCopy (impl->mPayload,
                                             &payload))))
    {
        mama_log(MAMA_LOG_LEVEL_ERROR,
                 "mamaMsg_detach() Failed. "
                 "Could not copy native payload [%d]", status);
        return status;
    }

    msg->mPayload = payload;
    msg->mPayloadBridge->msgPayloadSetParent (impl->mPayload, msg);
    
    /*If this is a dqStrategy cache message*/
    if (impl->mDqStrategyContext)
    {  

        if (MAMA_STATUS_OK!=(status=dqStrategyImpl_detachMsg (impl->mDqStrategyContext, msg)))
        {
            mama_log (MAMA_LOG_LEVEL_ERROR, "mamaMsg_detach(): "
                    "Could not detach cache message.");
            return status;
        }
    }

    /*We are now responsible for destroying the middleware message*/
    impl->mMessageOwner = 1; 

    return MAMA_STATUS_OK;
}

mama_status
mamaMsgImpl_setQueue (mamaMsg msg, mamaQueue queue)
{
    mamaMsgImpl*    impl    =   (mamaMsgImpl*)msg;
    if (!impl) return MAMA_STATUS_NULL_ARG;
    if (!queue) return MAMA_STATUS_INVALID_QUEUE;

    impl->mQueue = queue;

    return MAMA_STATUS_OK;
}

mama_status
mamaMsgImpl_setDqStrategyContext (mamaMsg msg, mamaDqContext* dqStrategyContext)
{
    mamaMsgImpl*    impl    =   (mamaMsgImpl*)msg;
    if (!impl) return MAMA_STATUS_NULL_ARG;
    if (!dqStrategyContext) return MAMA_STATUS_INVALID_ARG;

    impl->mDqStrategyContext = dqStrategyContext;

    return MAMA_STATUS_OK;
}

mama_status
mamaMsgImpl_setBridgeImpl (mamaMsg msg, mamaBridgeImpl* bridgeImpl)
{
    mamaMsgImpl*    impl    =   (mamaMsgImpl*)msg;
    mama_status     status  =   MAMA_STATUS_OK;

    if (!impl) return MAMA_STATUS_NULL_ARG;
    if (!bridgeImpl) return MAMA_STATUS_NO_BRIDGE_IMPL;

    impl->mBridgeImpl   =   bridgeImpl;

    /*
    Once we have the bridge impl we can create the underlying
    bridge message structure.
    This will be called when the reuseable message for a queue is created.
    */
    if (MAMA_STATUS_OK!=(status=bridgeImpl->bridgeMamaMsgCreate
                        (&impl->mBridgeMessage, msg)))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaMsgImpl_setBridgeImpl(): Could "
                  "not create bridge msg impl. [%d]", status);
        return status;
    }

    return MAMA_STATUS_OK;
}

mama_status
mamaMsgImpl_useBridgePayload (mamaMsg msg, mamaBridgeImpl* bridgeImpl)
{
    mamaMsgImpl_setBridgeImpl (msg, bridgeImpl);

    /* Save the bridge's default queue in the message as well, this may be
      overwritten later on. */
    mamaMsgImpl_setQueue(msg, bridgeImpl->mDefaultEventQueue);

    return MAMA_STATUS_OK;
}

mama_status
mamaMsg_getPayloadType (mamaMsg msg, mamaPayloadType* payloadType)
{
    mamaMsgImpl*    impl    = (mamaMsgImpl*)msg;

    if (!impl) return MAMA_STATUS_NULL_ARG;

    if (impl->mPayloadBridge)
    {
        *payloadType = impl->mPayloadBridge->msgPayloadGetType();
    }

    return MAMA_STATUS_OK;
}

mama_status
mamaMsgImpl_getPayload (const mamaMsg msg, msgPayload* payload)
{
    mamaMsgImpl*    impl    = (mamaMsgImpl*)msg;

    if (!impl || !payload) return MAMA_STATUS_NULL_ARG;

    *payload = impl->mPayload;
    
    return MAMA_STATUS_OK;    
}

const char*
mamaPayload_convertToString (mamaPayloadType payloadType)
{
    switch (payloadType)
    {
        case MAMA_PAYLOAD_WOMBAT_MSG:
            return "WombatMsg";
        case MAMA_PAYLOAD_TIBRV:
            return "TIBRV";
        case MAMA_PAYLOAD_FAST:
            return "FAST";
        case MAMA_PAYLOAD_V5:
            return "V5";
        case MAMA_PAYLOAD_AVIS:
            return "AVIS";
        default:
            return "unknown";
    }
}

mama_status
mamaMsg_createFromByteBuffer (
        mamaMsg*       msg,
        const void*    buffer,
        mama_size_t    bufferLength)
{
    mamaMsg_createForPayload (msg, (char) ((const char*)buffer) [0]);

    return (mamaMsg_setNewBuffer (*msg, (void*)buffer, bufferLength));
}

mama_status
mamaMsgImpl_setPayloadBridge (mamaMsg msg, mamaPayloadBridgeImpl* payloadBridge)
{
    mamaMsgImpl*    impl    =   (mamaMsgImpl*)msg;
    if (!impl) return MAMA_STATUS_NULL_ARG;
    impl->mPayloadBridge  =   payloadBridge;
    return MAMA_STATUS_OK;
}

mama_status
mamaMsgImpl_setPayload (mamaMsg msg, msgPayload payload, short owner)
{
    mamaMsgImpl*    impl    =   (mamaMsgImpl*)msg;
    if (!impl) return MAMA_STATUS_NULL_ARG;

    if (impl->mNestedMessages != NULL)
    {
        mamaMsg nested = NULL;
        while (NULL !=
              (nested = (mamaMsg)list_pop_front (impl->mNestedMessages)))
        {
            mamaMsg_destroy (nested);
            list_free_element (impl->mNestedMessages, nested);
        }
        /* Do not destroy the list. We can reuse the memory! */
    }

    impl->mPayload      = payload;
    impl->mMessageOwner = owner; 
    impl->mPayloadBridge->msgPayloadSetParent (impl->mPayload, msg);
    
    return MAMA_STATUS_OK;
}


mama_status
mamaMsg_createForTemplate (mamaMsg* msg, mama_u32_t templateId)
{
    return MAMA_STATUS_NOT_INSTALLED;
}

mama_status
mamaMsg_createForPayload (mamaMsg* msg,  const char id)
{
    return mamaMsg_createForPayloadBridge (msg, mamaInternal_findPayload(id));
}

mama_status
mamaMsg_createForPayloadBridge (mamaMsg* msg, mamaPayloadBridge payloadBridge)
{
    msgPayload payload;
    mama_status status = MAMA_STATUS_OK;

    if (MAMA_STATUS_OK !=
       (status = payloadBridge->msgPayloadCreate (&payload)))
    {
        *msg = NULL;
        return status;
    }

    return mamaMsgImpl_createForPayload  (msg,
                                          payload,
                                          payloadBridge,
                                          1);
}


mama_status
mamaMsgImpl_setMsgBuffer(mamaMsg     msg,
                         const void* data,
                         uint32_t    len, char id)
{
    /*Create the mamaMsg impl */
    mamaMsgImpl*    impl        = (mamaMsgImpl*)msg;
    mama_status     status      = MAMA_STATUS_OK;
    msgPayload      payload     = NULL;
    mamaPayloadBridgeImpl* newPayloadBridge = NULL;
    if (impl == NULL)
    {
        mama_log (MAMA_LOG_LEVEL_WARN,
                  "mamaMsgImpl_setMsgBuffer(): NULL mamaMsg.");
        return MAMA_STATUS_NULL_ARG;
    }

    if (impl->mNestedMessages != NULL)
    {
        mamaMsg nested = NULL;
        while (NULL !=
              (nested = (mamaMsg)list_pop_front (impl->mNestedMessages)))
        {
            mamaMsg_destroy (nested);
            list_free_element (impl->mNestedMessages, nested);
        }
        /* Do not destroy the list. We can reuse the memory! */
    }

    /*If there is an existing message buffer which we own destroy it*/
    if (impl->mBridgeImpl &&
            impl->mBridgeMessage &&
                impl->mMessageOwner)
    {
        if (MAMA_STATUS_OK!=
        impl->mBridgeImpl->bridgeMamaMsgDestroyMiddlewareMsg (
                                        impl->mBridgeMessage))
        {
            mama_log (MAMA_LOG_LEVEL_ERROR,
                      "mamaMsgImpl_setMsgBuffer(): "
                      "Could not destroy bridge message.");
        }
        impl->mBridgeMessage = NULL;
    }
    impl->mMessageOwner = 0;

    if (id == '\0')
        id = (char) ((const char*)data) [0];

    impl->mPayloadBridge = mamaInternal_findPayload(id);
    impl->mPayload = impl->mPayloads[(uint8_t)id];

    if (!impl->mPayloadBridge) return MAMA_STATUS_NO_BRIDGE_IMPL;

    if (!impl->mPayload)
    {
        if (MAMA_STATUS_OK!= (status =
           impl->mPayloadBridge->msgPayloadCreateFromByteBuffer (
                &payload,
                impl->mPayloadBridge,
                data,
                len)))
        {
            return status;
        }
        impl->mPayloads[(uint8_t)id] = payload;
         /* The middleware does not own this message */
        return mamaMsgImpl_setPayload (msg, payload, 0);
    }
    if (MAMA_STATUS_OK!= (status =
        impl->mPayloadBridge->msgPayloadSetByteBuffer (impl->mPayload,
                                                       impl->mPayloadBridge,
                                                       data,
                                                       len)))
    {
        return status;
    }

    return status;
}

static mama_status
mamaMsgImpl_createNestedForPayload (mamaMsg*        result,
                                    mamaMsgImpl*    parent,
                                    msgPayload      payload)
{
    mamaMsgImpl* impl;

    if (parent->mNestedMessages == NULL)
    {
        parent->mNestedMessages = list_create (sizeof (mamaMsgImpl));
    }

    impl = (mamaMsgImpl*)list_allocate_element (parent->mNestedMessages);

    if (impl == NULL)
    {

        mama_log (MAMA_LOG_LEVEL_ERROR,
            "mamaMsgImpl_createNestedForPayload(): "
            "Could not create bridge msg.");

        return MAMA_STATUS_NOMEM;
    }

    list_push_front (parent->mNestedMessages, impl);
    *result = (mamaMsg)impl;

    mamaMsgField_create (&impl->mCurrentField);
    impl->mCurrentField->myPayloadBridge = parent->mPayloadBridge;
    impl->mPayloadBridge    = parent->mPayloadBridge;
    impl->mPayload          = payload;
    impl->mParent           = parent;
    impl->mPayloadBridge->msgPayloadSetParent (impl->mPayload, impl);

    return MAMA_STATUS_OK;
}


mama_status
mamaMsgImpl_setMessageOwner (mamaMsg msg,
                             short   owner)
{
    mama_status status  =   MAMA_STATUS_OK;

    mamaMsgImpl* impl   =   (mamaMsgImpl*)msg;
    impl->mMessageOwner = owner; 

    return status;
}

mama_status
mamaMsgImpl_getMessageOwner (mamaMsg msg,
                             short*  owner)
{
    mama_status status  =   MAMA_STATUS_OK;

    mamaMsgImpl* impl   =   (mamaMsgImpl*)msg;
    *owner = impl->mMessageOwner;
    
    return status;
}

mama_status
mamaMsgImpl_createForPayload (mamaMsg*                  msg,
                              msgPayload                payload,
                              mamaPayloadBridgeImpl*    payloadBridge,
                              short                     owner)
{
    mama_status status  =   MAMA_STATUS_OK;
    mamaMsgImpl* impl   =   NULL;

    if (!msg)     return MAMA_STATUS_NULL_ARG;

    /* Create the mamaMsg impl */
    impl = (mamaMsgImpl*)calloc (1, sizeof (mamaMsgImpl));

    if (!impl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                 "mamaMsgImpl_createForPayload(): "
                 "Could not create bridge msg.");
        return MAMA_STATUS_NOMEM;
    }

    if(MAMA_STATUS_OK!=(status=mamaMsgField_create (&impl->mCurrentField)))
    {
        free (impl);
        *msg = NULL;
        return status;
    }
    *msg = (mamaMsg)impl;

    impl->mPayload                =  payload;
    impl->mPayloadBridge          =  payloadBridge;
    /*These will be set later if necessary*/
    impl->mBridgeImpl             =  NULL;
    impl->mBridgeMessage          =  NULL;
    impl->mMessageOwner           =  owner;
    impl->mDqStrategyContext      =  NULL;
    
    /* The payloadBridge and payload are optional for this function */
    if (payloadBridge) 
    {
        impl->mPayloadBridge->msgPayloadSetParent (impl->mPayload, impl);
    }

    return status;
}

mama_status
mamaMsg_copy (mamaMsg src, mamaMsg* copy)
{
    mamaMsgImpl*  source = (mamaMsgImpl*)src;

    mama_status  status = MAMA_STATUS_OK;

    if (!source) return MAMA_STATUS_NULL_ARG;

    if (source->mPayloadBridge)
    {
        msgPayload payload = NULL;

        /* Copy the payload first */
        if (MAMA_STATUS_OK != (status =
            (source->mPayloadBridge->msgPayloadCopy (source->mPayload,
                                                     &payload))))
        {
            mama_log(MAMA_LOG_LEVEL_ERROR,
                        "mamaMsg_copy() Failed. "
                        "Could not copy native payload [%d]", status);
            return status;
        }

        /* mamaMsg_create() may or may not have already been called on the
           copy message */
        if (*copy == NULL)
        {
            if (MAMA_STATUS_OK != (status =
                mamaMsgImpl_createForPayload (copy, payload, source->mPayloadBridge, 1)))
            {
                mama_log(MAMA_LOG_LEVEL_ERROR,
                            "mamaMsg_copy Failed. "
                            "Could not create for native payload [%d]", status);
                return status;
            }
        }
        else
        {
            mamaMsgImpl_setPayloadBridge (*copy, source->mPayloadBridge);
            mamaMsgImpl_setPayload       (*copy, payload, 1);
        }
    }


    if (status == MAMA_STATUS_OK)
    {
        mamaMsgImpl* newImpl = (mamaMsgImpl*)(*copy);

        if (strlen (source->mSymbol))
            strncpy (newImpl->mSymbol, source->mSymbol, MAX_SUBJECT);
        if (strlen (source->mSource))
            strncpy (newImpl->mSource, source->mSource, MAX_SUBJECT);
    }
    return status;
}

mama_status
mamaMsg_create (mamaMsg* msg)
{
    mama_status       status  = MAMA_STATUS_OK;
    mamaPayloadBridge bridge  = mamaInternal_getDefaultPayload ();
    msgPayload        payload = NULL;

    if (bridge)
    {
        if (MAMA_STATUS_OK != (status = bridge->msgPayloadCreate (&payload)))
        {
            *msg = NULL;
            return status;
        }
    }
    return mamaMsgImpl_createForPayload  (msg,
                                          payload,
                                          bridge,
                                          1);
}

mama_status
mamaMsg_getSendSubject (const mamaMsg msg, const char** subject)
{
    mamaMsgImpl*    impl = (mamaMsgImpl*)msg;

    if (!impl) return MAMA_STATUS_NULL_ARG;
    if (!subject) return MAMA_STATUS_INVALID_ARG;

    if (impl->mPayloadBridge)
    {
        return impl->mPayloadBridge->msgPayloadGetSendSubject(impl->mPayload,
                                                             subject);
    }

    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
mamaMsgImpl_getSubject (const mamaMsg msg, const char** result)
{
    mamaMsgImpl* impl = (mamaMsgImpl*)msg;
    if (!impl) return MAMA_STATUS_NULL_ARG;
    if (!result) return MAMA_STATUS_INVALID_ARG;

    *result  = impl->mSubject;

    return MAMA_STATUS_OK;
}

mama_status
mamaMsgImpl_setStatus (mamaMsg msg, mamaMsgStatus status)
{
    mamaMsgImpl* impl = (mamaMsgImpl*)msg;
    if (!impl) return MAMA_STATUS_NULL_ARG;

    impl->mStatus = status;

    return MAMA_STATUS_OK;
}

mama_status
mamaMsgImpl_getStatus (mamaMsg msg, mamaMsgStatus* status)
{
    mamaMsgImpl* impl = (mamaMsgImpl*)msg;
    if (!impl) return MAMA_STATUS_NULL_ARG;
    if (!status) return MAMA_STATUS_INVALID_ARG;

    *status  = impl->mStatus;

    return MAMA_STATUS_OK;
}

mamaMsgStatus
mamaMsgImpl_getStatusFromMsg (mamaMsg msg)
{
    mamaMsgImpl* impl = (mamaMsgImpl*)msg;
    int32_t result = MAMA_MSG_STATUS_UNKNOWN;
    if (mamaMsg_getI32 (msg,
                    MamaFieldMsgStatus.mName,
                    MamaFieldMsgStatus.mFid,
                    &result) != MAMA_STATUS_OK)
        result = MAMA_MSG_STATUS_UNKNOWN;

    impl->mStatus = (mamaMsgStatus) result;
    return (mamaMsgStatus) result;
}

mama_status
mamaMsg_getNumFields (const mamaMsg msg, mama_size_t* result)
{
    mamaMsgImpl*    impl = (mamaMsgImpl*)msg;

    if (!impl) return MAMA_STATUS_NULL_ARG;
    if (!result) return MAMA_STATUS_INVALID_ARG;

    if (impl->mPayloadBridge)
    {
        return impl->mPayloadBridge->msgPayloadGetNumFields (impl->mPayload,
                                                             result);
    }

    return MAMA_STATUS_INVALID_ARG;
}

mama_status
mamaMsg_getByteSize (const mamaMsg msg, mama_size_t* size)
{
    mamaMsgImpl*    impl     =   (mamaMsgImpl*)msg;

    if (!msg)   return MAMA_STATUS_NULL_ARG;
    if (!size)  return MAMA_STATUS_INVALID_ARG;

    *size = 0; /* for 64 bit */

    if (impl->mPayloadBridge)
    {
        return impl->mPayloadBridge->msgPayloadGetByteSize (impl->mPayload,
                                                            size);
    }

    return MAMA_STATUS_OK;
}

mama_status
mamaMsg_addBool(
    mamaMsg       msg,
    const char*   name,
    mama_fid_t    fid,
    mama_bool_t   value)
{
    mamaMsgImpl*    impl     = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadAddBool (impl->mPayload,
                                                    name,
                                                    fid,
                                                    value);
}

mama_status
mamaMsg_addChar(
        mamaMsg         msg,
        const char*     name,
        mama_fid_t      fid,
        char            value)
{
    mamaMsgImpl*    impl     = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadAddChar (impl->mPayload,
                                                    name,
                                                    fid,
                                                    value);
}

mama_status
mamaMsg_addI8(
        mamaMsg         msg,
        const char*     name,
        mama_fid_t      fid,
        mama_i8_t       value)
{
    mamaMsgImpl*    impl     = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadAddI8 (impl->mPayload,
                                                  name,
                                                  fid,
                                                  value);
}

mama_status
mamaMsg_addU8(
        mamaMsg         msg,
        const char*     name,
        mama_fid_t      fid,
        mama_u8_t       value)
{
    mamaMsgImpl*    impl     = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadAddU8 (impl->mPayload,
                                                  name,
                                                  fid,
                                                  value);
}

mama_status
mamaMsg_addI16(
        mamaMsg         msg,
        const char*     name,
        mama_fid_t      fid,
        mama_i16_t      value)
{
    mamaMsgImpl*    impl     = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadAddI16 (impl->mPayload,
                                                   name,
                                                   fid,
                                                   value);
}

mama_status
mamaMsg_addU16(
    mamaMsg         msg,
    const char*     name,
    mama_fid_t      fid,
    mama_u16_t      value)
{
    mamaMsgImpl*    impl     = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadAddU16 (impl->mPayload,
                                                   name,
                                                   fid,
                                                   value);


}

mama_status
mamaMsg_addI32 (
    mamaMsg      msg,
    const char*  name,
    mama_fid_t   fid,
    mama_i32_t   value)
{
    mamaMsgImpl*    impl     = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadAddI32 (impl->mPayload,
                                                   name,
                                                   fid,
                                                   value);
}

mama_status
mamaMsg_addU32(
    mamaMsg      msg,
    const char*  name,
    mama_fid_t   fid,
    mama_u32_t   value)
{
    mamaMsgImpl*    impl     = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadAddU32 (impl->mPayload,
                                                   name,
                                                   fid,
                                                   value);
}

mama_status
mamaMsg_addI64 (
    mamaMsg      msg,
    const char*  name,
    mama_fid_t   fid,
    mama_i64_t   value)
{
    mamaMsgImpl*    impl     = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadAddI64 (impl->mPayload,
                                                   name,
                                                   fid,
                                                   value);
}

mama_status
mamaMsg_addU64(
    mamaMsg      msg,
    const char*  name,
    mama_fid_t   fid,
    mama_u64_t   value)
{
    mamaMsgImpl*    impl     = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadAddU64 (impl->mPayload,
                                                   name,
                                                   fid,
                                                   value);
}

mama_status
mamaMsg_addF32(
    mamaMsg      msg,
    const char*  name,
    mama_fid_t   fid,
    mama_f32_t   value)
{
    mamaMsgImpl*    impl     = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadAddF32 (impl->mPayload,
                                                   name,
                                                   fid,
                                                   value);
}

mama_status
mamaMsg_addF64 (
    mamaMsg      msg,
    const char*  name,
    mama_fid_t   fid,
    mama_f64_t   value)
{
    mamaMsgImpl*    impl     = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadAddF64 (impl->mPayload,
                                                   name,
                                                   fid,
                                                   value);
}

mama_status
mamaMsg_addString (
    mamaMsg      msg,
    const char*  name,
    mama_fid_t   fid,
    const char*  value)
{
    mamaMsgImpl*    impl     = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadAddString (impl->mPayload,
                                                      name,
                                                      fid,
                                                      value);
}

mama_status
mamaMsg_addOpaque(
    mamaMsg      msg,
    const char*  name,
    mama_fid_t   fid,
    const void*  value,
    mama_size_t  size)
{
    mamaMsgImpl*    impl     = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadAddOpaque (impl->mPayload,
                                                      name,
                                                      fid,
                                                      value,
                                                      size);
}

mama_status
mamaMsg_addDateTime(
    mamaMsg             msg,
    const char*         name,
    mama_fid_t          fid,
    const mamaDateTime  value)
{
    mamaMsgImpl*    impl     = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadAddDateTime (impl->mPayload,
                                                        name,
                                                        fid,
                                                        value);
}

mama_status
mamaMsg_addPrice(
    mamaMsg             msg,
    const char*         name,
    mama_fid_t          fid,
    const mamaPrice     value)
{
    mamaMsgImpl*    impl     = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadAddPrice (impl->mPayload,
                                                     name,
                                                     fid,
                                                     value);
}

mama_status
mamaMsg_addMsg(
    mamaMsg       msg,
    const char*   name,
    mama_fid_t    fid,
    const mamaMsg value)
{
    mamaMsgImpl*    impl     = (mamaMsgImpl*)msg;
    mamaMsgImpl*    subMsg   = (mamaMsgImpl*)value;
    if (!impl || !subMsg || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;

    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

   return impl->mPayloadBridge->msgPayloadAddMsg (impl->mPayload,
                                                       name,
                                                       fid,
                                                       subMsg->mPayload);


}

mama_status
mamaMsg_addVectorBool (
    mamaMsg            msg,
    const char*        name,
    mama_fid_t         fid,
    const mama_bool_t  value[],
    mama_size_t        numElements)
{
    mamaMsgImpl*    impl     = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadAddVectorBool (impl->mPayload,
                                                          name,
                                                          fid,
                                                          value,
                                                          numElements);
}

mama_status
mamaMsg_addVectorChar (
    mamaMsg            msg,
    const char*        name,
    mama_fid_t         fid,
    const char         value[],
    mama_size_t        numElements)
{
    mamaMsgImpl*    impl     = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadAddVectorChar (impl->mPayload,
                                                          name,
                                                          fid,
                                                          value,
                                                          numElements);
}

mama_status
mamaMsg_addVectorI8 (
    mamaMsg            msg,
    const char*        name,
    mama_fid_t         fid,
    const mama_i8_t    value[],
    mama_size_t        numElements)
{
    mamaMsgImpl*    impl     = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadAddVectorI8 (impl->mPayload,
                                                        name,
                                                        fid,
                                                        value,
                                                        numElements);
}

mama_status
mamaMsg_addVectorU8 (
    mamaMsg            msg,
    const char*        name,
    mama_fid_t         fid,
    const mama_u8_t    value[],
    mama_size_t        numElements)
{
    mamaMsgImpl*    impl     = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadAddVectorU8 (impl->mPayload,
                                                        name,
                                                        fid,
                                                        value,
                                                        numElements);
}

mama_status
mamaMsg_addVectorI16 (
    mamaMsg            msg,
    const char*        name,
    mama_fid_t         fid,
    const mama_i16_t   value[],
    mama_size_t        numElements)
{
    mamaMsgImpl*    impl     = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadAddVectorI16 (impl->mPayload,
                                                         name,
                                                         fid,
                                                         value,
                                                         numElements);
}

mama_status
mamaMsg_addVectorU16 (
    mamaMsg            msg,
    const char*        name,
    mama_fid_t         fid,
    const mama_u16_t   value[],
    mama_size_t        numElements)
{
    mamaMsgImpl*    impl     = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadAddVectorU16 (impl->mPayload,
                                                         name,
                                                         fid,
                                                         value,
                                                         numElements);
}

mama_status
mamaMsg_addVectorI32 (
        mamaMsg            msg,
        const char*        name,
        mama_fid_t         fid,
        const mama_i32_t   value[],
        mama_size_t        numElements)
{
    mamaMsgImpl*    impl     = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadAddVectorI32 (impl->mPayload,
                                                         name,
                                                         fid,
                                                         value,
                                                         numElements);
}

mama_status
mamaMsg_addVectorU32 (
        mamaMsg            msg,
        const char*        name,
        mama_fid_t         fid,
        const mama_u32_t   value[],
        mama_size_t        numElements)
{
    mamaMsgImpl*    impl     = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadAddVectorU32 (impl->mPayload,
                                                         name,
                                                         fid,
                                                         value,
                                                         numElements);
}

mama_status
mamaMsg_addVectorI64 (
        mamaMsg            msg,
        const char*        name,
        mama_fid_t         fid,
        const mama_i64_t   value[],
        mama_size_t        numElements)
{
    mamaMsgImpl*    impl     = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadAddVectorI64 (impl->mPayload,
                                                         name,
                                                         fid,
                                                         value,
                                                         numElements);
}

mama_status
mamaMsg_addVectorU64 (
        mamaMsg            msg,
        const char*        name,
        mama_fid_t         fid,
        const mama_u64_t   value[],
        mama_size_t        numElements)
{
    mamaMsgImpl*    impl     = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadAddVectorU64 (impl->mPayload,
                                                         name,
                                                         fid,
                                                         value,
                                                         numElements);
}

mama_status
mamaMsg_addVectorF32 (
        mamaMsg            msg,
        const char*        name,
        mama_fid_t         fid,
        const mama_f32_t   value[],
        mama_size_t        numElements)
{
    mamaMsgImpl*    impl     = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadAddVectorF32 (impl->mPayload,
                                                         name,
                                                         fid,
                                                         value,
                                                         numElements);
}

mama_status
mamaMsg_addVectorF64 (
        mamaMsg            msg,
        const char*        name,
        mama_fid_t         fid,
        const mama_f64_t   value[],
        mama_size_t        numElements)
{
    mamaMsgImpl*    impl     = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadAddVectorF64 (impl->mPayload,
                                                         name,
                                                         fid,
                                                         value,
                                                         numElements);
}

mama_status
mamaMsg_addVectorString (
        mamaMsg      msg,
        const char*  name,
        mama_fid_t   fid,
        const char*  value[],
        mama_size_t  numElements)
{
    mamaMsgImpl*    impl     = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadAddVectorString (impl->mPayload,
                                                            name,
                                                            fid,
                                                            value,
                                                            numElements);
}

mama_status
mamaMsg_addVectorDateTime (
    mamaMsg             msg,
    const char*         name,
    mama_fid_t          fid,
    const mamaDateTime  value[],
    mama_size_t         numElements)
{
    mamaMsgImpl* impl = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadAddVectorDateTime (
                                                        impl->mPayload,
                                                        name,
                                                        fid,
                                                        value,
                                                        numElements);
}

mama_status
mamaMsg_addVectorPrice (
    mamaMsg             msg,
    const char*         name,
    mama_fid_t          fid,
    const mamaPrice     value[],
    mama_size_t         numElements)
{
    mamaMsgImpl* impl = (mamaMsgImpl*)msg;
    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadAddVectorPrice (impl->mPayload,
                                                           name,
                                                           fid,
                                                           value,
                                                           numElements);
}

mama_status
mamaMsg_addVectorMsg(
        mamaMsg         msg,
        const char*     name,
        mama_fid_t      fid,
        const mamaMsg   value[],
        mama_size_t     numElements)
{
    mamaMsgImpl* impl         = (mamaMsgImpl*)msg;

    /* Check the arguments. */
    if((!impl) || !impl->mPayloadBridge || (NULL == value))
    {
        return MAMA_STATUS_NULL_ARG;
    }

    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;
    return impl->mPayloadBridge->msgPayloadAddVectorMsg (impl->mPayload,
                                                         name,
                                                         fid,
                                                         value,
                                                         numElements);
}

mama_status
mamaMsg_getBool(
        const mamaMsg msg,
        const char*   name,
        mama_fid_t    fid,
        mama_bool_t*  result)
{
    mamaMsgImpl*    impl    = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;

    return impl->mPayloadBridge->msgPayloadGetBool (impl->mPayload,
                                                    name,
                                                    fid,
                                                    result);
}

mama_status
mamaMsg_getChar(
        const mamaMsg msg,
        const char*   name,
        mama_fid_t    fid,
        char*         result)
{
    mamaMsgImpl*    impl      = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;

    return impl->mPayloadBridge->msgPayloadGetChar (impl->mPayload,
                                                    name,
                                                    fid,
                                                    result);
}

mama_status
mamaMsg_getI8(
        const mamaMsg msg,
        const char*   name,
        mama_fid_t    fid,
        mama_i8_t*    result)
{
    mamaMsgImpl*    impl    = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;

    return impl->mPayloadBridge->msgPayloadGetI8 (impl->mPayload,
                                                  name,
                                                  fid,
                                                  result);
}

mama_status
mamaMsg_getU8(
        const mamaMsg msg,
        const char*   name,
        mama_fid_t    fid,
        mama_u8_t*       result)
{
    mamaMsgImpl*    impl    = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;

    return impl->mPayloadBridge->msgPayloadGetU8 (impl->mPayload,
                                                  name,
                                                  fid,
                                                  result);
}

mama_status
mamaMsg_getI16(
        const mamaMsg   msg,
        const char*     name,
        mama_fid_t      fid,
        mama_i16_t*     result)
{
    mamaMsgImpl*    impl    = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;

    return impl->mPayloadBridge->msgPayloadGetI16 (impl->mPayload,
                                                   name,
                                                   fid,
                                                   result);
}

mama_status
mamaMsg_getU16(
        const mamaMsg   msg,
        const char*     name,
        mama_fid_t      fid,
        mama_u16_t*     result)
{
    mamaMsgImpl*    impl    = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;

    return impl->mPayloadBridge->msgPayloadGetU16 (impl->mPayload,
                                                   name,
                                                   fid,
                                                   result);
}

mama_status
mamaMsg_getI32 (
    const mamaMsg   msg,
    const char*     name,
    mama_fid_t      fid,
    mama_i32_t*     result)
{
    mamaMsgImpl*    impl    = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;

    return impl->mPayloadBridge->msgPayloadGetI32 (impl->mPayload,
                                                   name,
                                                   fid,
                                                   result);
}

mama_status
mamaMsg_getU32(
        const mamaMsg  msg,
        const char*    name,
        mama_fid_t     fid,
        mama_u32_t*    result)
{
    mamaMsgImpl*    impl    = (mamaMsgImpl*)msg;

    if (!impl) return MAMA_STATUS_NULL_ARG;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;

    return impl->mPayloadBridge->msgPayloadGetU32 (impl->mPayload,
                                                   name,
                                                   fid,
                                                   result);
}

mama_status
mamaMsg_getI64 (
    const mamaMsg       msg,
    const char*         name,
    mama_fid_t          fid,
    mama_i64_t*         result)
{
    mamaMsgImpl*    impl    = (mamaMsgImpl*)msg;

    if (!impl) return MAMA_STATUS_NULL_ARG;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;

    return impl->mPayloadBridge->msgPayloadGetI64 (impl->mPayload,
                                                   name,
                                                   fid,
                                                   result);
}

mama_status
mamaMsg_getU64(
    const mamaMsg  msg,
    const char*    name,
    mama_fid_t     fid,
    mama_u64_t*    result)
{
    mamaMsgImpl*    impl    = (mamaMsgImpl*)msg;

    if (!impl) return MAMA_STATUS_NULL_ARG;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;

    return impl->mPayloadBridge->msgPayloadGetU64 (impl->mPayload,
                                                   name,
                                                   fid,
                                                   result);
}

mama_status
mamaMsg_getF32(
    const mamaMsg  msg,
    const char*    name,
    mama_fid_t     fid,
    mama_f32_t*    result)
{
    mamaMsgImpl*    impl    = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;

    return impl->mPayloadBridge->msgPayloadGetF32 (impl->mPayload,
                                                   name,
                                                   fid,
                                                   result);
}

mama_status
mamaMsg_getF64 (
    const mamaMsg  msg,
    const char*    name,
    mama_fid_t     fid,
    mama_f64_t*    result)
{
    mamaMsgImpl*    impl    = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;

    return impl->mPayloadBridge->msgPayloadGetF64 (impl->mPayload,
                                                   name,
                                                   fid,
                                                   result);
}

mama_status
mamaMsg_getString (
    const mamaMsg  msg,
    const char*    name,
    mama_fid_t     fid,
    const char**   result)
{
    mamaMsgImpl*    impl    = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;

    return impl->mPayloadBridge->msgPayloadGetString (impl->mPayload,
                                                      name,
                                                      fid,
                                                      result);
}

mama_status
mamaMsg_getOpaque(
    const mamaMsg  msg,
    const char*    name,
    mama_fid_t     fid,
    const void**   result,
    mama_size_t*   size)
{
    mamaMsgImpl*    impl    = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;

    return impl->mPayloadBridge->msgPayloadGetOpaque (impl->mPayload,
                                                      name,
                                                      fid,
                                                      result,
                                                      size);
}

mama_status
mamaMsg_getField(
    const mamaMsg  msg,
    const char*    name,
    mama_fid_t     fid,
    mamaMsgField*  result)
{
    mamaMsgImpl*        impl        = (mamaMsgImpl*)msg;
    mamaMsgFieldImpl*   mamaField   = NULL;
    mama_status         status      = MAMA_STATUS_OK;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;

    mamaField = (mamaMsgFieldImpl*)impl->mCurrentField;

    if (MAMA_STATUS_OK!=(status = impl->mPayloadBridge->msgPayloadGetField (
                                    impl->mPayload,
                                    name,
                                    fid,
                                    &impl->mCurrentField->myPayload)))
    {
        return status;
    }
    impl->mCurrentField->myPayloadBridge = impl->mPayloadBridge;
    impl->mCurrentField->myMsg           = impl;

    mamaField = (mamaMsgFieldImpl*)impl->mCurrentField;
    mamaField->myDictionary  = NULL;
    mamaField->myFieldDesc   = NULL;

    *result = impl->mCurrentField;

    return MAMA_STATUS_OK;
}

mama_status
mamaMsg_getDateTimeMSec(
    const mamaMsg  msg,
    const char*    name,
    mama_fid_t     fid,
    mama_u64_t*    milliseconds)
{
    mama_status status = MAMA_STATUS_OK;
    mamaMsgImpl*     impl   = (mamaMsgImpl*)msg;

    if (!impl->mCurrentDateTime)
        mamaDateTime_create(&impl->mCurrentDateTime);

    status = mamaMsg_getDateTime (msg, name, fid, impl->mCurrentDateTime);
    if (status == MAMA_STATUS_OK)
        status = mamaDateTime_getEpochTimeMilliseconds (impl->mCurrentDateTime, milliseconds);

    return status;
}

mama_status
mamaMsg_getDateTime(
    const mamaMsg  msg,
    const char*    name,
    mama_fid_t     fid,
    mamaDateTime   result)
{
    mamaMsgImpl*     impl   = (mamaMsgImpl*)msg;

    if (!impl) return MAMA_STATUS_NULL_ARG;

    if (impl->mPayloadBridge)
    {
        return impl->mPayloadBridge->msgPayloadGetDateTime (impl->mPayload,
                                                            name,
                                                            fid,
                                                            result);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsg_getPrice(
    const mamaMsg  msg,
    const char*    name,
    mama_fid_t     fid,
    mamaPrice      result)
{
    mamaMsgImpl* impl = (mamaMsgImpl*)msg;

    if (!impl) return MAMA_STATUS_NULL_ARG;

    if (impl->mPayloadBridge)
    {
        return impl->mPayloadBridge->msgPayloadGetPrice (impl->mPayload,
                                                         name,
                                                         fid,
                                                         result);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsg_updateBool(
        mamaMsg     msg,
        const char* name,
        mama_fid_t  fid,
        mama_bool_t value)
{
    mamaMsgImpl*    impl    = (mamaMsgImpl*)msg;

    if (!impl) return MAMA_STATUS_OK;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    if (impl->mPayloadBridge)
    {
        return impl->mPayloadBridge->msgPayloadUpdateBool (impl->mPayload,
                                                           name,
                                                           fid,
                                                           value);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsg_updateChar(
        mamaMsg     msg,
        const char* name,
        mama_fid_t  fid,
        char        value)
{
    mamaMsgImpl*    impl    = (mamaMsgImpl*)msg;

    if (!impl) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    if (impl->mPayloadBridge)
    {
        return impl->mPayloadBridge->msgPayloadUpdateChar (impl->mPayload,
                                                           name,
                                                           fid,
                                                           value);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsg_updateI8(
        mamaMsg     msg,
        const char* name,
        mama_fid_t  fid,
        mama_i8_t   value)
{
    mamaMsgImpl*    impl    = (mamaMsgImpl*)msg;

    if (!impl) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    if (impl->mPayloadBridge)
    {
        return impl->mPayloadBridge->msgPayloadUpdateI8 (impl->mPayload,
                                                         name,
                                                         fid,
                                                         value);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsg_updateU8(
        mamaMsg     msg,
        const char* name,
        mama_fid_t  fid,
        mama_u8_t   value)
{
    mamaMsgImpl*    impl    = (mamaMsgImpl*)msg;

    if (!impl) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    if (impl->mPayloadBridge)
    {
        return impl->mPayloadBridge->msgPayloadUpdateU8 (impl->mPayload,
                                                         name,
                                                         fid,
                                                         value);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsg_updateI16(
        mamaMsg     msg,
        const char* name,
        mama_fid_t  fid,
        mama_i16_t  value)
{
    mamaMsgImpl*    impl    = (mamaMsgImpl*)msg;

    if (!impl) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    if (impl->mPayloadBridge)
    {
        return impl->mPayloadBridge->msgPayloadUpdateI16 (impl->mPayload,
                                                          name,
                                                          fid,
                                                          value);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsg_updateU16(
        mamaMsg     msg,
        const char* name,
        mama_fid_t  fid,
        mama_u16_t  value)
{
    mamaMsgImpl*    impl    = (mamaMsgImpl*)msg;

    if (!impl) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    if (impl->mPayloadBridge)
    {
        return impl->mPayloadBridge->msgPayloadUpdateU16 (impl->mPayload,
                                                          name,
                                                          fid,
                                                          value);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsg_updateI32 (
    mamaMsg      msg,
    const char*  name,
    mama_fid_t   fid,
    mama_i32_t   value)
{
    mamaMsgImpl*    impl    = (mamaMsgImpl*)msg;

    if (!impl) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    if (impl->mPayloadBridge)
    {
        return impl->mPayloadBridge->msgPayloadUpdateI32 (impl->mPayload,
                                                          name,
                                                          fid,
                                                          value);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsg_updateU32(
        mamaMsg      msg,
        const char*  name,
        mama_fid_t   fid,
        mama_u32_t   value)
{
    mamaMsgImpl*    impl    = (mamaMsgImpl*)msg;

    if (!impl) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    if (impl->mPayloadBridge)
    {
        return impl->mPayloadBridge->msgPayloadUpdateU32 (impl->mPayload,
                                                          name,
                                                          fid,
                                                          value);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsg_updateI64 (
    mamaMsg      msg,
    const char*  name,
    mama_fid_t   fid,
    mama_i64_t   value)
{
    mamaMsgImpl*    impl    = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadUpdateU64 (impl->mPayload,
                                                      name,
                                                      fid,
                                                      value);
}

mama_status
mamaMsg_updateU64(
    mamaMsg      msg,
    const char*  name,
    mama_fid_t   fid,
    mama_u64_t   value)
{
    mamaMsgImpl*    impl    = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadUpdateU64 (impl->mPayload,
                                                      name,
                                                      fid,
                                                      value);
}

mama_status
mamaMsg_updateF32(
    mamaMsg      msg,
    const char*  name,
    mama_fid_t   fid,
    mama_f32_t   value)
{
    mamaMsgImpl*    impl    = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadUpdateF32 (impl->mPayload,
                                                      name,
                                                      fid,
                                                      value);
}

mama_status
mamaMsg_updateF64 (
    mamaMsg      msg,
    const char*  name,
    mama_fid_t   fid,
    mama_f64_t   value)
{
    mamaMsgImpl*    impl    = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadUpdateF64 (impl->mPayload,
                                                      name,
                                                      fid,
                                                      value);
}

mama_status
mamaMsg_updateString (
    mamaMsg      msg,
    const char*  name,
    mama_fid_t   fid,
    const char*  value)
{
    mamaMsgImpl*    impl    =  (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadUpdateString (impl->mPayload,
                                                         name,
                                                         fid,
                                                         value);
}

mama_status
mamaMsg_applyMsg (mamaMsg msg, mamaMsg src)
{
    mamaMsgImpl*    impl    = (mamaMsgImpl*)msg;
    mamaMsgImpl*    source  = (mamaMsgImpl*)src;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadApply (impl->mPayload,
                                                  source->mPayload);
}

mama_status
mamaMsg_getNativeMsg (mamaMsg msg, void** nativeMsg)
{
    mamaMsgImpl*    impl    = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;

    return impl->mPayloadBridge->msgPayloadGetNativeMsg (impl->mPayload, nativeMsg);
}

mama_status
mamaMsg_updateSubMsg (
    mamaMsg         msg,
    const char*     name,
    mama_fid_t      fid,
    const mamaMsg   value)
{
    mamaMsgImpl*    impl    = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadUpdateSubMsg (impl->mPayload,
                                                         name,
                                                         fid,
                                                         value);
}

mama_status
mamaMsg_updateOpaque (
    mamaMsg     msg,
    const char* name,
    mama_fid_t  fid,
    const void* value,
    mama_size_t size)
{
    mamaMsgImpl*    impl    = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadUpdateOpaque (impl->mPayload,
                                                         name,
                                                         fid,
                                                         value,
                                                         size);
}

mama_status
mamaMsg_updateDateTime(
    mamaMsg             msg,
    const char*         name,
    mama_fid_t          fid,
    const mamaDateTime  value)
{
    mamaMsgImpl*    impl    = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadUpdateDateTime (impl->mPayload,
                                                           name,
                                                           fid,
                                                           value);
}

mama_status
mamaMsg_updatePrice(
    mamaMsg             msg,
    const char*         name,
    mama_fid_t          fid,
    const mamaPrice     value)
{
    mamaMsgImpl*    impl    = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadUpdatePrice (impl->mPayload,
                                                        name,
                                                        fid,
                                                        value);
}

mama_status
mamaMsg_updateVectorMsg (
    mamaMsg               msg,
    const char*           name,
    mama_fid_t            fid,
    const mamaMsg         value[],
    mama_size_t           numElements)
{
    mamaMsgImpl*    impl        = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadUpdateVectorMsg (impl->mPayload,
                                                            name,
                                                            fid,
                                                            value,
                                                            numElements);
}

mama_status
mamaMsg_updateVectorString (
    mamaMsg               msg,
    const char*           name,
    mama_fid_t            fid,
    const char*           value[],
    mama_size_t           numElements)
{
    mamaMsgImpl*    impl    = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadUpdateVectorString (
                                                            impl->mPayload,
                                                            name,
                                                            fid,
                                                            value,
                                                            numElements);
}

mama_status
mamaMsg_updateVectorBool (
    mamaMsg               msg,
    const char*           name,
    mama_fid_t            fid,
    const mama_bool_t     value[],
    mama_size_t           numElements)
{
    mamaMsgImpl*    impl    = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadUpdateVectorBool (
                                                            impl->mPayload,
                                                            name,
                                                            fid,
                                                            value,
                                                            numElements);
}

mama_status
mamaMsg_updateVectorChar (
    mamaMsg               msg,
    const char*           name,
    mama_fid_t            fid,
    const char            value[],
    mama_size_t           numElements)
{
    mamaMsgImpl*    impl    = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadUpdateVectorChar (
                                                            impl->mPayload,
                                                            name,
                                                            fid,
                                                            value,
                                                            numElements);
}

mama_status
mamaMsg_updateVectorI8 (
    mamaMsg               msg,
    const char*           name,
    mama_fid_t            fid,
    const mama_i8_t       value[],
    mama_size_t           numElements)
{
    mamaMsgImpl*    impl    = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadUpdateVectorI8 (impl->mPayload,
                                                           name,
                                                           fid,
                                                           value,
                                                           numElements);
}

mama_status
mamaMsg_updateVectorU8 (
    mamaMsg               msg,
    const char*           name,
    mama_fid_t            fid,
    const mama_u8_t       value[],
    mama_size_t           numElements)
{
    mamaMsgImpl*    impl    = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadUpdateVectorU8 (impl->mPayload,
                                                           name,
                                                           fid,
                                                           value,
                                                           numElements);

}

mama_status
mamaMsg_updateVectorI16 (
    mamaMsg               msg,
    const char*           name,
    mama_fid_t            fid,
    const mama_i16_t      value[],
    mama_size_t           numElements)
{
    mamaMsgImpl*    impl    = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadUpdateVectorI16 (impl->mPayload,
                                                            name,
                                                            fid,
                                                            value,
                                                            numElements);
}

mama_status
mamaMsg_updateVectorU16 (
    mamaMsg               msg,
    const char*           name,
    mama_fid_t            fid,
    const mama_u16_t      value[],
    mama_size_t           numElements)
{
    mamaMsgImpl*    impl    = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadUpdateVectorU16 (impl->mPayload,
                                                            name,
                                                            fid,
                                                            value,
                                                            numElements);
}

mama_status
mamaMsg_updateVectorI32 (
    mamaMsg               msg,
    const char*           name,
    mama_fid_t            fid,
    const mama_i32_t      value[],
    mama_size_t           numElements)
{
    mamaMsgImpl*    impl    = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadUpdateVectorI32 (impl->mPayload,
                                                            name,
                                                            fid,
                                                            value,
                                                            numElements);
}

mama_status
mamaMsg_updateVectorU32 (
    mamaMsg               msg,
    const char*           name,
    mama_fid_t            fid,
    const mama_u32_t      value[],
    mama_size_t           numElements)
{
    mamaMsgImpl*    impl    = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadUpdateVectorU32 (impl->mPayload,
                                                            name,
                                                            fid,
                                                            value,
                                                            numElements);
}

mama_status
mamaMsg_updateVectorI64 (
    mamaMsg               msg,
    const char*           name,
    mama_fid_t            fid,
    const mama_i64_t      value[],
    mama_size_t           numElements)
{
    mamaMsgImpl*    impl    = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadUpdateVectorI64 (impl->mPayload,
                                                            name,
                                                            fid,
                                                            value,
                                                            numElements);
}

mama_status
mamaMsg_updateVectorU64 (
    mamaMsg               msg,
    const char*           name,
    mama_fid_t            fid,
    const mama_u64_t      value[],
    mama_size_t           numElements)
{
    mamaMsgImpl*    impl    = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadUpdateVectorU64 (impl->mPayload,
                                                            name,
                                                            fid,
                                                            value,
                                                            numElements);
}

mama_status
mamaMsg_updateVectorF32 (
    mamaMsg               msg,
    const char*           name,
    mama_fid_t            fid,
    const mama_f32_t      value[],
    mama_size_t           numElements)
{
    mamaMsgImpl*    impl    = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadUpdateVectorF32 (impl->mPayload,
                                                            name,
                                                            fid,
                                                            value,
                                                            numElements);
}

mama_status
mamaMsg_updateVectorF64 (
    mamaMsg               msg,
    const char*           name,
    mama_fid_t            fid,
    const mama_f64_t      value[],
    mama_size_t           numElements)
{
    mamaMsgImpl*    impl    = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;
    if (!impl->mMessageOwner) return MAMA_STATUS_NOT_MODIFIABLE;

    return impl->mPayloadBridge->msgPayloadUpdateVectorF64 (impl->mPayload,
                                                            name,
                                                            fid,
                                                            value,
                                                            numElements);

}

mama_status
mamaMsg_getEntitleCode (const mamaMsg  msg, mama_i32_t* entitleCode)

{
    *entitleCode = 0;

    return mamaMsg_getI32 (msg,
                           ENTITLE_FIELD_NAME,
                           ENTITLE_FIELD_ID,
                           entitleCode);
}

mama_status
mamaMsg_getPlatformError (const mamaMsg msg, void** error)
{
    mamaMsgImpl* impl   = (mamaMsgImpl*)msg;
    mama_status  status = MAMA_STATUS_OK;

    if (!impl) return MAMA_STATUS_NULL_ARG;
    if (!error) return MAMA_STATUS_INVALID_ARG;
    if (!impl->mBridgeImpl) return MAMA_STATUS_NO_BRIDGE_IMPL;

    if (MAMA_STATUS_OK!=
            (status=impl->mBridgeImpl->bridgeMamaMsgGetPlatformError
                                    (impl->mBridgeMessage, error)))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaMsg_getPlatformError(): "
                  "Could not get platform error.");
        return status;
    }
    return MAMA_STATUS_OK;
}

mama_status
mamaMsgImpl_setSubscInfo (mamaMsg      msg,
                          const char*  root,
                          const char*  source,
                          const char*  symbol,
                          int           modifyMessage)
{
    mamaMsgImpl* impl = (mamaMsgImpl*)msg;

    if (!impl) return MAMA_STATUS_NULL_ARG;

    if (root != NULL && source != NULL )
    {
        snprintf (impl->mSubject,  MAX_SUBJECT,"%s.%s", root, source);
    }
    else if (source != NULL && symbol != NULL )
    {
        snprintf (impl->mSubject, MAX_SUBJECT, "%s.%s", source, symbol);
        snprintf (impl->mSource,  MAX_SUBJECT,       "%s",    source);
    }
    else if( symbol != NULL ) /* We only have a symbol */
    {
        snprintf (impl->mSubject, MAX_SUBJECT, "%s", symbol);
    }

    if (symbol != NULL)
    {
        snprintf (impl->mSymbol, MAX_SUBJECT, "%s", symbol);

        if (modifyMessage && impl->mBridgeImpl)
        {
             impl->mBridgeImpl->bridgeMamaMsgSetSendSubject (
                                                impl->mBridgeMessage,
                                                symbol,
                                                impl->mSubject);
        }
    }

    return MAMA_STATUS_OK;
}

mama_status
mamaMsg_getFieldAsString (
    const mamaMsg       msg,
    const char*         fieldName,
    mama_fid_t          fid,
    char*               buf,
    mama_size_t         len)
{
    mamaMsgImpl*    impl    = (mamaMsgImpl*)msg;

    if (!impl) return MAMA_STATUS_NULL_ARG;
    if (impl->mPayloadBridge)
    {
        return impl->mPayloadBridge->msgPayloadGetFieldAsString (impl->mPayload,
                                                                 fieldName,
                                                                 fid,
                                                                 buf,
                                                                 len);

    }
    return MAMA_STATUS_NULL_ARG;
}

const char*
mamaMsg_toString (const mamaMsg msg)
{
    mamaMsgImpl* impl = (mamaMsgImpl*)msg;
    if (!impl) return NULL;

    if (impl->mPayloadBridge)
    {
        return impl->mPayloadBridge->msgPayloadToString (impl->mPayload);
    }
    return NULL;
}

void
mamaMsg_freeString (const mamaMsg msg,  const char* msgString)
{

}

mama_status
mamaMsg_iterateFields (const mamaMsg            msg,
                       mamaMsgIteratorCb        cb,
                       const mamaDictionary     dict,
                       void*                    closure)
{
    mamaMsgImpl* impl = (mamaMsgImpl*)(msg);

    if (!impl) return MAMA_STATUS_NULL_ARG;

    impl->mCurrentField->myDictionary  = dict;

    if (impl->mPayloadBridge)
    {
        impl->mCurrentField->myPayloadBridge = impl->mPayloadBridge;
        impl->mCurrentField->myMsg           = impl;
        return (impl->mPayloadBridge->msgPayloadIterateFields (impl->mPayload,
                                                       msg,
                                                       impl->mCurrentField,
                                                       cb,
                                                       closure));
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsg_getMsg (
    const mamaMsg    msg,
    const char*      name,
    mama_fid_t       fid,
    mamaMsg*         result)
{
    mamaMsgImpl* impl = (mamaMsgImpl*)(msg);

    msgPayload payload = NULL;
    *result = 0;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;

    if (MAMA_STATUS_OK != impl->mPayloadBridge->msgPayloadGetMsg (
                                                   impl->mPayload,
                                                   name,
                                                   fid,
                                                   &payload))
    {
        return MAMA_STATUS_NOT_FOUND;
    }
    mamaMsgImpl_createNestedForPayload (result, impl, payload);
    return MAMA_STATUS_OK;
}

mama_status
mamaMsg_getVectorBool (
        const mamaMsg        msg,
        const char*          name,
        mama_fid_t           fid,
        const mama_bool_t**  result,
        mama_size_t*         resultLen)
{
    mamaMsgImpl*    impl     = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;

    return
    impl->mPayloadBridge->msgPayloadGetVectorBool (impl->mPayload,
                                                   name,
                                                   fid,
                                                   result,
                                                   resultLen);
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsg_getVectorChar (
        const mamaMsg        msg,
        const char*          name,
        mama_fid_t           fid,
        const char**         result,
        mama_size_t*         resultLen)
{
    mamaMsgImpl*    impl     = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;

    return
    impl->mPayloadBridge->msgPayloadGetVectorChar (impl->mPayload,
                                                   name,
                                                   fid,
                                                   result,
                                                   resultLen);
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsg_getVectorI8 (
        const mamaMsg        msg,
        const char*          name,
        mama_fid_t           fid,
        const mama_i8_t**    result,
        mama_size_t*         resultLen)
{
    mamaMsgImpl*    impl     = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;

    return
    impl->mPayloadBridge->msgPayloadGetVectorI8   (impl->mPayload,
                                                   name,
                                                   fid,
                                                   result,
                                                   resultLen);
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsg_getVectorU8 (
        const mamaMsg        msg,
        const char*          name,
        mama_fid_t           fid,
        const mama_u8_t**    result,
        mama_size_t*         resultLen)
{
    mamaMsgImpl*    impl     = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;

    return
    impl->mPayloadBridge->msgPayloadGetVectorU8   (impl->mPayload,
                                                   name,
                                                   fid,
                                                   result,
                                                   resultLen);
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsg_getVectorI16 (
        const mamaMsg        msg,
        const char*          name,
        mama_fid_t           fid,
        const mama_i16_t**   result,
        mama_size_t*         resultLen)
{
    mamaMsgImpl*    impl     = (mamaMsgImpl*)msg;

    if (!impl || !impl->mPayloadBridge) return MAMA_STATUS_NULL_ARG;

    return
    impl->mPayloadBridge->msgPayloadGetVectorI16  (impl->mPayload,
                                                   name,
                                                   fid,
                                                   result,
                                                   resultLen);
}

mama_status
mamaMsg_getVectorU16 (
        const mamaMsg        msg,
        const char*          name,
        mama_fid_t           fid,
        const mama_u16_t**   result,
        mama_size_t*         resultLen)
{
    mamaMsgImpl*    impl     = (mamaMsgImpl*)msg;

    if (!impl) return MAMA_STATUS_NULL_ARG;

    if (impl->mPayloadBridge)
    {
        return
        impl->mPayloadBridge->msgPayloadGetVectorU16  (impl->mPayload,
                                                       name,
                                                       fid,
                                                       result,
                                                       resultLen);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsg_getVectorI32 (
        const mamaMsg        msg,
        const char*          name,
        mama_fid_t           fid,
        const mama_i32_t**      result,
        mama_size_t*         resultLen)
{
    mamaMsgImpl*    impl     = (mamaMsgImpl*)msg;

    if (!impl) return MAMA_STATUS_NULL_ARG;

    if (impl->mPayloadBridge)
    {
        return
        impl->mPayloadBridge->msgPayloadGetVectorI32  (impl->mPayload,
                                                       name,
                                                       fid,
                                                       result,
                                                       resultLen);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsg_getVectorU32 (
        const mamaMsg        msg,
        const char*          name,
        mama_fid_t           fid,
        const mama_u32_t**     result,
        mama_size_t*         resultLen)
{
    mamaMsgImpl*    impl     = (mamaMsgImpl*)msg;

    if (!impl) return MAMA_STATUS_NULL_ARG;

    if (impl->mPayloadBridge)
    {
        return
        impl->mPayloadBridge->msgPayloadGetVectorU32  (impl->mPayload,
                                                       name,
                                                       fid,
                                                       result,
                                                       resultLen);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsg_getVectorI64 (
        const mamaMsg        msg,
        const char*          name,
        mama_fid_t           fid,
        const mama_i64_t**      result,
        mama_size_t*         resultLen)
{
    mamaMsgImpl*    impl     = (mamaMsgImpl*)msg;

    if (!impl) return MAMA_STATUS_NULL_ARG;

    if (impl->mPayloadBridge)
    {
        return
        impl->mPayloadBridge->msgPayloadGetVectorI64  (impl->mPayload,
                                                       name,
                                                       fid,
                                                       result,
                                                       resultLen);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsg_getVectorU64 (
        const mamaMsg        msg,
        const char*          name,
        mama_fid_t           fid,
        const mama_u64_t**   result,
        mama_size_t*         resultLen)
{
    mamaMsgImpl*    impl     = (mamaMsgImpl*)msg;

    if (!impl) return MAMA_STATUS_NULL_ARG;

    if (impl->mPayloadBridge)
    {
        return
        impl->mPayloadBridge->msgPayloadGetVectorU64  (impl->mPayload,
                                                       name,
                                                       fid,
                                                       result,
                                                       resultLen);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsg_getVectorF32 (
        const mamaMsg        msg,
        const char*          name,
        mama_fid_t           fid,
        const mama_f32_t**   result,
        mama_size_t*         resultLen)
{
    mamaMsgImpl*    impl     = (mamaMsgImpl*)msg;

    if (!impl) return MAMA_STATUS_NULL_ARG;

    if (impl->mPayloadBridge)
    {
        return
        impl->mPayloadBridge->msgPayloadGetVectorF32  (impl->mPayload,
                                                       name,
                                                       fid,
                                                       result,
                                                       resultLen);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsg_getVectorF64 (
        const mamaMsg        msg,
        const char*          name,
        mama_fid_t           fid,
        const mama_f64_t**   result,
        mama_size_t*         resultLen)
{
    mamaMsgImpl*    impl     = (mamaMsgImpl*)msg;

    if (!impl) return MAMA_STATUS_NULL_ARG;

    if (impl->mPayloadBridge)
    {
        return
        impl->mPayloadBridge->msgPayloadGetVectorF64  (impl->mPayload,
                                                       name,
                                                       fid,
                                                       result,
                                                       resultLen);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsg_getVectorString (
        const mamaMsg  msg,
        const char*    name,
        mama_fid_t     fid,
        const char***  result,
        mama_size_t*   resultLen)
{
    mamaMsgImpl*    impl     = (mamaMsgImpl*)msg;

    if (!impl) return MAMA_STATUS_NULL_ARG;

    if (impl->mPayloadBridge)
    {
        return
        impl->mPayloadBridge->msgPayloadGetVectorString (impl->mPayload,
                                                         name,
                                                         fid,
                                                         result,
                                                         resultLen);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsg_getVectorDateTime (
    const mamaMsg         msg,
    const char*           name,
    mama_fid_t            fid,
    const mamaDateTime*   result,
    mama_size_t*          resultLen)
{
    mamaMsgImpl*    impl     = (mamaMsgImpl*)msg;

    if (!impl) return MAMA_STATUS_NULL_ARG;

    if (impl->mPayloadBridge)
    {
        return
        impl->mPayloadBridge->msgPayloadGetVectorDateTime (impl->mPayload,
                                                         name,
                                                         fid,
                                                         result,
                                                         resultLen);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsg_getVectorPrice (
    const mamaMsg         msg,
    const char*           name,
    mama_fid_t            fid,
    const mamaPrice*      result,
    mama_size_t*          resultLen)
{
    mamaMsgImpl*    impl     = (mamaMsgImpl*)msg;

    if (!impl) return MAMA_STATUS_NULL_ARG;
    {
        return
        impl->mPayloadBridge->msgPayloadGetVectorPrice  (impl->mPayload,
                                                         name,
                                                         fid,
                                                         result,
                                                         resultLen);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsg_getVectorMsg (
    const mamaMsg    msg,
    const char*      name,
    mama_fid_t       fid,
    const mamaMsg**  result0,
    mama_size_t*     resultLen)
{
    void*       vp                  = NULL;
    int i;

    mamaMsgImpl* impl = (mamaMsgImpl*)msg;

    if (!impl) return MAMA_STATUS_NULL_ARG;
    if (!result0) return MAMA_STATUS_INVALID_ARG;

    *result0   = NULL;
    *resultLen = 0;

    if (impl->mPayloadBridge)
    {
        size_t      payloadVectorSize = 0;
        mama_status status            = MAMA_STATUS_OK;
        msgPayload* payloadVector     = NULL;

        if (MAMA_STATUS_OK != (status =
                impl->mPayloadBridge->msgPayloadGetVectorMsg (impl->mPayload,
                        name,
                        fid,
                        (const msgPayload**) &payloadVector,
                        &payloadVectorSize)))
        {
            return status;
        }

        /*Either create or reallocate the vector if it is not big enough*/
        if (!impl->mLastVectorPayloadMsg)
        {
            impl->mLastVectorPayloadMsg = (mamaMsg*)calloc(payloadVectorSize,
                    sizeof(mamaMsg));

            if(impl->mLastVectorPayloadMsg == NULL)
                return MAMA_STATUS_NOMEM;
        }
        else if (payloadVectorSize > impl->mLastVectorPayloadMsgLen)
        {
            vp = realloc (impl->mLastVectorPayloadMsg,
                    payloadVectorSize * sizeof(mamaMsg));

            if(vp == NULL)
                return MAMA_STATUS_NOMEM;

            impl->mLastVectorPayloadMsg = (mamaMsg*) vp;
        }

        for (i = 0; (i != impl->mLastVectorPayloadMsgLen) && (i != payloadVectorSize);
             ++i)
        {
            mamaMsgImpl_setPayloadBridge (impl->mLastVectorPayloadMsg[i],
                                          impl->mPayloadBridge);
            /*
             * We do not detach the middle ware message so we do
             * not own it.
             */
            mamaMsgImpl_setPayload       (impl->mLastVectorPayloadMsg[i],
                                          payloadVector[i],
                                          0);
        }

        for (; i != payloadVectorSize; ++i)
        {
            /*
             * We create from payload so payload owns
             * We set owner to false to not delete the payload
             */
            mamaMsgImpl_createForPayload (&(impl->mLastVectorPayloadMsg[i]),
                              payloadVector[i],
                              impl->mPayloadBridge,
                              0);
        }

        /*Store the maxumim vector we have encountered*/
        if (payloadVectorSize > impl->mLastVectorPayloadMsgLen)
        {
            impl->mLastVectorPayloadMsgLen = payloadVectorSize;
        }

        *resultLen =  payloadVectorSize;
        *result0    = impl->mLastVectorPayloadMsg;
        return MAMA_STATUS_OK;
    }
    return MAMA_STATUS_NULL_ARG;
}

int
mamaMsg_isFromInbox (const mamaMsg msg)
{
    mamaMsgImpl* impl = (mamaMsgImpl*)msg;
    if (!impl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mamaMsg_isFromInbox(): NULL message.");
        return 0;
    }

    if (!impl->mBridgeImpl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                 "mamaMsg_isFromInbox(): NULL bridge.");
        return 0;
    }

    if (!impl->mBridgeMessage)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mamaMsg_isFromInbox(): NULL bridge message");
        return 0;
    }
    return impl->mBridgeImpl->bridgeMamaMsgIsFromInbox (msg->mBridgeMessage);
}



mama_status
mamaMsg_getByteBuffer (
        const mamaMsg  msg,
        const void**   buffer,
        mama_size_t*   bufferLength)
{
    mamaMsgImpl*    impl        =   (mamaMsgImpl*)msg;

    if (!impl) return MAMA_STATUS_NULL_ARG;

    if (impl->mPayloadBridge)
    {
        return
        impl->mPayloadBridge->msgPayloadSerialize   (impl->mPayload,
                                                         buffer,
                                                         bufferLength);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsgImpl_getPayloadBuffer (
        const mamaMsg  msg,
        const void**   buffer,
        mama_size_t*   bufferLength)
{
    mamaMsgImpl*    impl        =   (mamaMsgImpl*)msg;

    if (!impl) return MAMA_STATUS_NULL_ARG;

    if (impl->mPayloadBridge)
    {
        return
        impl->mPayloadBridge->msgPayloadGetByteBuffer (impl->mPayload,
                                                       buffer,
                                                       bufferLength);
    }
    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsg_setNewBuffer (mamaMsg msg, void* buffer,
                      mama_size_t size)
{
    mamaMsgImpl*    impl        =   (mamaMsgImpl*)msg;

    if (!impl) return MAMA_STATUS_NULL_ARG;

    impl->mPayloadBridge = mamaInternal_findPayload( (char) ((const char*)buffer) [0]);

    if (impl->mPayloadBridge)
    {
        return
        impl->mPayloadBridge->msgPayloadUnSerialize (impl->mPayload,
                                                     buffer,
                                                     size);
    }

    return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaMsg_getNativeHandle (const mamaMsg  msg,
                         void**         result)
{
    mamaMsgImpl* impl = (mamaMsgImpl*)msg;
    if (!impl) return MAMA_STATUS_NULL_ARG;
    return impl->mBridgeImpl->bridgeMamaMsgGetNativeHandle (
                        impl->mBridgeMessage, result);
}

mama_status
mamaMsgImpl_getBridgeMsg (mamaMsg msg, msgBridge* bridgeMsg)
{
    mamaMsgImpl* impl = (mamaMsgImpl*)msg;
    if (!impl) return MAMA_STATUS_NULL_ARG;
    if (!impl->mBridgeMessage) return MAMA_STATUS_INVALID_ARG;
    *bridgeMsg = impl->mBridgeMessage;
    return MAMA_STATUS_OK;
}

mama_status
mamaMsg_setReplyHandle (mamaMsg msg, mamaMsgReply handle)
{
    mamaMsgReplyImpl* reply = (mamaMsgReplyImpl*)handle;
    mamaMsgImpl* impl = (mamaMsgImpl*)msg;

    if (!impl) return MAMA_STATUS_NULL_ARG;

    mamaMsgImpl_setBridgeImpl(msg, reply->mBridgeImpl);

    return reply->mBridgeImpl->bridgeMamaMsgSetReplyHandleAndIncrement
        (impl->mBridgeMessage, reply->replyHandle);
}

mama_status
mamaMsg_getReplyHandle (mamaMsg msg, mamaMsgReply* handle)
{
    mamaMsgReplyImpl* reply = NULL;
    mamaMsgImpl* impl = (mamaMsgImpl*)msg;
    if (!impl) return MAMA_STATUS_NULL_ARG;

    reply = calloc (1, sizeof (mamaMsgReplyImpl));

    reply->mBridgeImpl = impl->mBridgeImpl;
    *handle =reply;
    return impl->mBridgeImpl->bridgeMamaMsgDuplicateReplyHandle
        (impl->mBridgeMessage, &reply->replyHandle);
}

mama_status
mamaMsg_copyReplyHandle (mamaMsgReply srcHandle, mamaMsgReply* copy)
{
    mamaMsgReplyImpl* reply = (mamaMsgReplyImpl*)srcHandle;
    mamaMsgReplyImpl* newcopy = calloc (1, sizeof (mamaMsgReplyImpl));

    newcopy->mBridgeImpl = reply->mBridgeImpl;

    *copy = newcopy;

    return reply->mBridgeImpl->bridgeMamaMsgCopyReplyHandle
        (reply->replyHandle, &newcopy->replyHandle);
}


mama_status
mamaMsg_destroyReplyHandle (mamaMsgReply handle)
{
    mamaMsgReplyImpl* reply = (mamaMsgReplyImpl*)handle;
    if (!reply) return MAMA_STATUS_NULL_ARG;


    reply->mBridgeImpl->bridgeMamaMsgDestroyReplyHandle
        (reply->replyHandle);

    free (reply);

    return MAMA_STATUS_OK;
}

mama_status
mamaMsg_getSeqNum (const mamaMsg msg, mama_seqnum_t* rval)
{
    mama_i64_t  seqNum = 0;
    mama_status status = mamaMsg_getI64 (msg,
                                         MamaFieldSeqNum.mName,
                                         MamaFieldSeqNum.mFid,
                                         &seqNum);
    *rval = seqNum;
    return status;
}

mama_status
mamaMsg_getIsDefinitelyDuplicate (mamaMsg msg, int* result)
{
    mama_u16_t msgQual;
    mama_status status = MAMA_STATUS_OK;
    *result = 0; /* If we can't find the field the result is false */

    if (MAMA_STATUS_OK!=(status=mamaMsg_getU16 (msg,
                        MamaFieldMsgQual.mName,
                        MamaFieldMsgQual.mFid,
                        &msgQual)))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mamaMsg_getIsDefinitelyDuplicate() Failed. [%d]",
                  status);
        return status;
    }
    *result = msgQual & MAMA_MSG_QUAL_DEFINITELY_DUPLICATE;
    return MAMA_STATUS_OK;
}

mama_status
mamaMsg_getIsPossiblyDuplicate (mamaMsg msg, int* result)
{
    mama_u16_t msgQual;
    mama_status status = MAMA_STATUS_OK;
    *result = 0; /* If we can't find the field the result is false */

    if (MAMA_STATUS_OK!=(status=mamaMsg_getU16 (msg,
                        MamaFieldMsgQual.mName,
                        MamaFieldMsgQual.mFid,
                        &msgQual)))
    {
        mama_log (MAMA_LOG_LEVEL_FINEST,
                  "mamaMsg_getIsPossiblyDuplicate() Failed. [%d]",
                  status);
        return status;
    }
    *result = msgQual & MAMA_MSG_QUAL_POSSIBLY_DUPLICATE;
    return MAMA_STATUS_OK;
}

mama_status
mamaMsg_getIsPossiblyDelayed (mamaMsg msg, int* result)
{
    mama_u16_t msgQual;
    mama_status status = MAMA_STATUS_OK;
    *result = 0; /* If we can't find the field the result is false */

    if (MAMA_STATUS_OK!=(status=mamaMsg_getU16 (msg,
                        MamaFieldMsgQual.mName,
                        MamaFieldMsgQual.mFid,
                        &msgQual)))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mamaMsg_getIsPossiblyDelayed() Failed. [%d]",
                  status);
        return status;
    }
    *result = msgQual & MAMA_MSG_QUAL_POSSIBLY_DELAYED;
    return MAMA_STATUS_OK;
}

mama_status
mamaMsg_getIsDefinitelyDelayed (mamaMsg msg, int* result)
{
    mama_u16_t msgQual;
    mama_status status = MAMA_STATUS_OK;
    *result = 0; /* If we can't find the field the result is false */

    if (MAMA_STATUS_OK!=(status=mamaMsg_getU16 (msg,
                        MamaFieldMsgQual.mName,
                        MamaFieldMsgQual.mFid,
                        &msgQual)))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mamaMsg_getIsDefinitelyDelayed() Failed. [%d]",
                  status);
        return status;
    }
    *result = msgQual & MAMA_MSG_QUAL_DEFINITELY_DELAYED;
    return MAMA_STATUS_OK;
}

mama_status
mamaMsg_getIsOutOfSequence (mamaMsg msg, int* result)
{
    mama_u16_t  msgQual;
    mama_status status = MAMA_STATUS_OK;
    *result = 0; /* If we can't find the field the result is false */

    if (MAMA_STATUS_OK!=(status=mamaMsg_getU16 (msg,
                        MamaFieldMsgQual.mName,
                        MamaFieldMsgQual.mFid,
                        &msgQual)))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mamaMsg_getIsOutOfSequence() Failed. [%d]",
                  status);
        return status;
    }
    *result = msgQual & MAMA_MSG_QUAL_OUT_OF_SEQUENCE;
    return MAMA_STATUS_OK;
}

mama_status
mamaMsgIterator_associate (mamaMsgIterator iterator, mamaMsg msg)
{
    mamaMsgIteratorImpl* itrImpl     = (mamaMsgIteratorImpl*) iterator;
    mamaMsgImpl*         msgImpl     = (mamaMsgImpl*) msg;

    if (!msgImpl) return MAMA_STATUS_NULL_ARG;
    if (!itrImpl) return MAMA_STATUS_NULL_ARG;

    if (msgImpl->mPayloadBridge)
    {
        itrImpl->mCurrentField->myPayloadBridge = msgImpl->mPayloadBridge;
        itrImpl->mCurrentField->myMsg           = msgImpl;
        itrImpl->mPayloadBridge                 = msgImpl->mPayloadBridge;

        /* Create the native payload iter if it hasn't been created already */
        if (!itrImpl->mPayloadIter)
        {
            msgImpl->mPayloadBridge->msgPayloadIterCreate (
                                            &itrImpl->mPayloadIter,
                                            msgImpl->mPayload);
        }
        msgImpl->mPayloadBridge->msgPayloadIterAssociate (
                                            itrImpl->mPayloadIter,
                                            msgImpl->mPayload);
    }

    return MAMA_STATUS_OK;
}

mama_status
mamaMsgIterator_setDict (mamaMsgIterator iterator, mamaDictionary dict)
{
    mamaMsgIteratorImpl* impl = (mamaMsgIteratorImpl*)iterator;
    if (!impl) return MAMA_STATUS_NULL_ARG;

    impl->mCurrentField->myDictionary = dict;
    return (MAMA_STATUS_OK);
}

mama_status
mamaMsgIterator_create (mamaMsgIterator* iterator, mamaDictionary dict)
{
    mama_status status;
    mamaMsgIteratorImpl* localiterator = NULL;

    localiterator = calloc (1, sizeof (mamaMsgIteratorImpl));

    if (!localiterator)
        return (MAMA_STATUS_NOMEM);

    if (MAMA_STATUS_OK!=
       (status=mamaMsgField_create (&(localiterator->mCurrentField))))
    {
        free (localiterator);
        return (status);
    }
    localiterator->mCurrentField->myDictionary = dict;
    *iterator =(mamaMsgIterator) localiterator;

    return (MAMA_STATUS_OK);
}

mama_status mamaMsgIterator_destroy (mamaMsgIterator iterator)
{
    mamaMsgIteratorImpl* impl = (mamaMsgIteratorImpl*)iterator;
    if (!impl) return MAMA_STATUS_NULL_ARG;

    if (impl->mPayloadBridge)
    {
        if (impl->mPayloadIter)
           impl->mPayloadBridge->msgPayloadIterDestroy (
                                            impl->mPayloadIter);
    }

    if (impl->mCurrentField)
        mamaMsgField_destroy (impl->mCurrentField);
    free (impl);
    return MAMA_STATUS_OK;
}

mamaMsgField
mamaMsgIterator_next (mamaMsgIterator iterator)
{
    mamaMsgIteratorImpl*     impl = (mamaMsgIteratorImpl*)iterator;
    mamaMsgFieldImpl* currentField = (mamaMsgFieldImpl*) impl->mCurrentField;

    if (!impl)
        return (NULL);

    if (impl->mPayloadBridge)
    {
        msgFieldPayload msgField = NULL;
        msgPayload      payload  = NULL;
        
        mamaMsgImpl_getPayload (currentField->myMsg, &payload); 

        if (NULL == (msgField =
           (impl->mPayloadBridge->msgPayloadIterNext (
                                            impl->mPayloadIter,
                                            currentField->myPayload,
                                            payload))))
        {
            return NULL;
        }

        currentField->myPayload     = msgField;
        currentField->myFieldDesc   = NULL;

        return currentField;
    }
    return (NULL);
}

mama_bool_t
mamaMsgIterator_hasNext (mamaMsgIterator iterator)
{
    mamaMsgIteratorImpl* impl = (mamaMsgIteratorImpl*)iterator;
    mamaMsgFieldImpl* currentField = (mamaMsgFieldImpl*)
                                      impl->mCurrentField;
    if (!impl)
        return 0;

    if (impl->mPayloadBridge)
    {
        msgPayload payload  = NULL;
        
        mamaMsgImpl_getPayload (currentField->myMsg, &payload); 
        return impl->mPayloadBridge->msgPayloadIterHasNext (
                                        impl->mPayloadIter,
                                        payload);

    }
    return (0);
}

mamaMsgField
mamaMsgIterator_begin (mamaMsgIterator iterator)
{
    mamaMsgIteratorImpl* impl = (mamaMsgIteratorImpl*)iterator;
    mamaMsgFieldImpl* currentField = (mamaMsgFieldImpl*)
                                      impl->mCurrentField;
    if (!impl)
        return (NULL);

    if (impl->mPayloadBridge)
    {
        msgFieldPayload msgField = NULL;
        msgPayload      payload  = NULL;
        
        mamaMsgImpl_getPayload (currentField->myMsg, &payload); 

        if (NULL == (msgField =
           (impl->mPayloadBridge->msgPayloadIterBegin (
                                    impl->mPayloadIter,
                                    currentField->myPayload,
                                    payload))))
        {
            return NULL;
        }
        currentField->myPayload     = msgField;
        currentField->myFieldDesc   = NULL;

        return currentField;
    }

    return (NULL);
}

/*===========================================================
  = Static function definitions
  ==========================================================*/
mama_status
mamaMsgImpl_destroyLastVectorMsg (mamaMsgImpl *impl)
{
    int i = 0;

    for( i = 0; i < impl->mLastVectorMsgLen; i++ )
    {
        mamaMsg_destroy( impl->mLastVectorMsg[i] );
    }

    free (impl->mLastVectorMsg);

    impl->mLastVectorMsg    = NULL;
    impl->mLastVectorMsgLen = 0;

    for( i = 0; i < impl->mLastVectorPayloadMsgLen; i++ )
    {
        mamaMsg_destroy( impl->mLastVectorPayloadMsg[i] );
    }

    free (impl->mLastVectorPayloadMsg);

    impl->mLastVectorPayloadMsg    = NULL;
    impl->mLastVectorPayloadMsgLen = 0;

    return MAMA_STATUS_OK;
}

