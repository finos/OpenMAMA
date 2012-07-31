/* $Id: msgimpl.h,v 1.28.4.1.16.8 2011/10/02 19:02:17 ianbell Exp $
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

#ifndef MamaMsgImplH__
#define MamaMsgImplH__

#include "wombat/wConfig.h"
#include "bridge.h"
#include "payloadbridge.h"
#include "dqstrategy.h"

#define MAX_SUBJECT 256

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct mamaMsgReplyImpl_
{
    mamaBridgeImpl*     mBridgeImpl;
    void *              replyHandle;
}mamaMsgReplyImpl;

MAMAExpDLL extern mama_status
mamaMsgImpl_createForPayload (mamaMsg*               msg,
                              msgPayload             payload,
                              mamaPayloadBridgeImpl* payloadBridge,
                              short                  noDeletePayload);

/*Used by each bridge to set the incoming data on an existing message */
MAMAExpDLL extern mama_status
mamaMsgImpl_setMsgBuffer(mamaMsg        msg,
                         const void*    data,
                         uint32_t       len,
                         char id);

/*Used by the bridge publishers.*/
MAMAExpDLL extern mama_status
mamaMsgImpl_getSubject (const mamaMsg msg, const char** result);

/*Associate a queue with a message - this is the queue on which the
 reuseable message is associated. Needed in order to detach a message*/
MAMAExpDLL extern mama_status
mamaMsgImpl_setQueue (mamaMsg msg, mamaQueue queue);

/*Associate a subscription context with a message - this is needed to
 inform the cache if a cache message has been detached*/
MAMAExpDLL extern mama_status
mamaMsgImpl_setDqStrategyContext (mamaMsg msg, mamaDqContext* dqStrategyContext);

/*Set the bridge struct of function pointers on the message*/
MAMAExpDLL extern mama_status
mamaMsgImpl_setBridgeImpl (mamaMsg msg, mamaBridgeImpl* bridgeImpl);

/*Get the bridge msg impl from the parent mamaMsg. This will have been set
 * when the mamaMsg called BridgeMamaMsg_create. It can only do this when it
 * is given the bridge impl object.*/
MAMAExpDLL extern mama_status
mamaMsgImpl_getBridgeMsg (mamaMsg msg, msgBridge* bridgeMsg);

/*Get the payload msg buffer from the parent mamaMsg. */
MAMAExpDLL extern mama_status
mamaMsgImpl_getPayloadBuffer(const mamaMsg  msg,
        const void**   buffer,
        mama_size_t*   bufferLength);

/*Get the underlying payload parent mamaMsg. */
MAMAExpDLL extern mama_status
mamaMsgImpl_getPayload (const mamaMsg msg, msgPayload* payload);

MAMAExpDLL
extern mama_status
mamaMsgImpl_setPayloadBridge (mamaMsg msg, mamaPayloadBridgeImpl* payloadBridge);

/*Use the native payload format of the bridge, if any */
MAMAExpDLL
extern mama_status
mamaMsgImpl_useBridgePayload (mamaMsg msg, mamaBridgeImpl* bridgeImpl);


MAMAExpDLL
extern mama_status
mamaMsgImpl_setPayload (mamaMsg msg, msgPayload payload, short ownsPayload);

MAMAExpDLL
mama_status
mamaMsgImpl_setMessageOwner (mamaMsg msg, short owner);

MAMAExpDLL
mama_status
mamaMsgImpl_getMessageOwner (mamaMsg msg, short* owner);

/* Build up the subject for the message, and set it in the underlying message
 * if needed (currently RV only)
 *
 * @param root A constant, predetermined prefix to avoid potential conflicts
 * with other fields. (_MD. for a normal subscription).
 * @param source The source name of the feed handler that will service the
 * request.
 * @param symbol The symbol to which to subscribe. May be null in some cases
 * like dictionary subsriptions.
 * @param modifyMessage add the required fields to the message if true.
 *     this can be inefficient on some platforms so we don't need to do
 *     it for inbound messages.
 *
 * @param msg The message.
 */
MAMAExpDLL extern mama_status
mamaMsgImpl_setSubscInfo (mamaMsg     msg,
                          const char* root,
                          const char* source,
                          const char* symbol,
                          int         modifyMessage);


extern MAMAExpDLL mama_status
mamaMsgImpl_copyWmsgPayload (mamaMsg msg);


MAMAExpDLL
mama_status
mamaMsgImpl_setStatus (mamaMsg msg, mamaMsgStatus status);

MAMAExpDLL
mama_status
mamaMsgImpl_getStatus (mamaMsg msg, mamaMsgStatus* status);

MAMAExpDLL
mamaMsgStatus
mamaMsgImpl_getStatusFromMsg (mamaMsg msg);


#if defined(__cplusplus)
}
#endif

#endif /* MamaMsgImplH__ */
