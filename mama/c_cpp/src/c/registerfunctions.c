/* $Id$
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

#include <bridge.h>
#include <registerfunctions.h>
#include <payloadbridge.h>


/**
 * Register function pointers associated with a specific middleware bridge.
 */
mama_status
mamaInternal_registerMiddlewareFunctions (LIB_HANDLE  bridgeLib,
                                          mamaBridge* bridge,
                                          const char* name)
{
    mama_status  status        = MAMA_STATUS_OK;
    void*        result        = NULL;
    char         functionName[256];

    /* Once the bridge has been successfully loaded, and the initialization
     * function called, we register each of the required bridge functions.
     */
    REGISTER_BRIDGE_FUNCTION (Bridge_start, bridgeStart, bridge_start);
    REGISTER_BRIDGE_FUNCTION (Bridge_stop,  bridgeStop,  bridge_stop);
    REGISTER_BRIDGE_FUNCTION (Bridge_open,  bridgeOpen,  bridge_open);
    REGISTER_BRIDGE_FUNCTION (Bridge_close, bridgeClose, bridge_close);

    /* General purpose functions */
    REGISTER_BRIDGE_FUNCTION (Bridge_getVersion, bridgeGetVersion, bridge_getVersion);
    REGISTER_BRIDGE_FUNCTION (Bridge_getName, bridgeGetName, bridge_getName);
    REGISTER_BRIDGE_FUNCTION (Bridge_getDefaultPayloadId, bridgeGetDefaultPayloadId,  bridge_getDefaultPayloadId);

    /* Queue related function pointers */
    REGISTER_BRIDGE_FUNCTION (BridgeMamaQueue_create, bridgeMamaQueueCreate, bridgeMamaQueue_create);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaQueue_create_usingNative, bridgeMamaQueueCreateUsingNative, bridgeMamaQueue_create_usingNative);
    REGISTER_BRIDGE_FUNCTION (BridgeMamaQueue_destroy, bridgeMamaQueueDestroy, bridgeMamaQueue_destroy);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaQueue_getEventCount, bridgeMamaQueueGetEventCount, bridgeMamaQueue_getEventCount);
    REGISTER_BRIDGE_FUNCTION (BridgeMamaQueue_dispatch, bridgeMamaQueueDispatch, bridgeMamaQueue_dispatch);
    REGISTER_BRIDGE_FUNCTION (BridgeMamaQueue_timedDispatch, bridgeMamaQueueTimedDispatch, bridgeMamaQueue_timedDispatch);
    REGISTER_BRIDGE_FUNCTION (BridgeMamaQueue_dispatchEvent, bridgeMamaQueueDispatchEvent, bridgeMamaQueue_dispatchEvent);
    REGISTER_BRIDGE_FUNCTION (BridgeMamaQueue_enqueueEvent, bridgeMamaQueueEnqueueEvent, bridgeMamaQueue_enqueueEvent);
    REGISTER_BRIDGE_FUNCTION (BridgeMamaQueue_stopDispatch, bridgeMamaQueueStopDispatch, bridgeMamaQueue_stopDispatch);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaQueue_setEnqueueCallback, bridgeMamaQueueSetEnqueueCallback, bridgeMamaQueue_setEnqueueCallback);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaQueue_removeEnqueueCallback, bridgeMamaQueueRemoveEnqueueCallback, bridgeMamaQueue_removeEnqueueCallback);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaQueue_getNativeHandle, bridgeMamaQueueGetNativeHandle, bridgeMamaQueue_getNativeHandle);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaQueue_setLowWatermark, bridgeMamaQueueSetLowWatermark, bridgeMamaQueue_setLowWatermark);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaQueue_setHighWatermark, bridgeMamaQueueSetHighWatermark, bridgeMamaQueue_setHighWatermark);

    /* Transport related function pointers */
    REGISTER_BRIDGE_FUNCTION (BridgeMamaTransport_isValid, bridgeMamaTransportIsValid, bridgeMamaTransport_isValid);
    REGISTER_BRIDGE_FUNCTION (BridgeMamaTransport_destroy, bridgeMamaTransportDestroy, bridgeMamaTransport_destroy);
    REGISTER_BRIDGE_FUNCTION (BridgeMamaTransport_create, bridgeMamaTransportCreate, bridgeMamaTransport_create);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaTransport_forceClientDisconnect, bridgeMamaTransportForceClientDisconnect, bridgeMamaTransport_forceClientDisconnect);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaTransport_findConnection, bridgeMamaTransportFindConnection, bridgeMamaTransport_findConnection);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaTransport_getAllConnections, bridgeMamaTransportGetAllConnections, bridgeMamaTransport_getAllConnections);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaTransport_getAllConnectionsForTopic, bridgeMamaTransportGetAllConnectionsForTopic, bridgeMamaTransport_getAllConnectionsForTopic);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaTransport_freeAllConnections, bridgeMamaTransportFreeAllConnections, bridgeMamaTransport_freeAllConnections);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaTransport_getAllServerConnections, bridgeMamaTransportGetAllServerConnections, bridgeMamaTransport_getAllServerConnections);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaTransport_freeAllServerConnections, bridgeMamaTransportFreeAllServerConnections, bridgeMamaTransport_freeAllServerConnections);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaTransport_getNumLoadBalanceAttributes, bridgeMamaTransportGetNumLoadBalanceAttributes, bridgeMamaTransport_getNumLoadBalanceAttributes);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaTransport_getLoadBalanceScheme, bridgeMamaTransportGetLoadBalanceScheme, bridgeMamaTransport_getLoadBalanceScheme);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaTransport_getLoadBalanceSharedObjectName, bridgeMamaTransportGetLoadBalanceSharedObjectName, bridgeMamaTransport_getLoadBalanceSharedObjectName);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaTransport_sendMsgToConnection, bridgeMamaTransportSendMsgToConnection, bridgeMamaTransport_sendMsgToConnection);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaTransport_isConnectionIntercepted, bridgeMamaTransportIsConnectionIntercepted, bridgeMamaTransport_isConnectionIntercepted);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaTransport_installConnectConflateMgr, bridgeMamaTransportInstallConnectConflateMgr, bridgeMamaTransport_installConnectConflateMgr);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaTransport_uninstallConnectConflateMgr, bridgeMamaTransportUninstallConnectConflateMgr, bridgeMamaTransport_uninstallConnectConflateMgr);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaTransport_startConnectionConflation, bridgeMamaTransportStartConnectionConflation, bridgeMamaTransport_startConnectionConflation);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaTransport_requestConflation, bridgeMamaTransportRequestConflation, bridgeMamaTransport_requestConflation);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaTransport_requestEndConflation, bridgeMamaTransportRequestEndConflation, bridgeMamaTransport_requestEndConflation);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaTransport_getNativeTransport, bridgeMamaTransportGetNativeTransport, bridgeMamaTransport_getNativeTransport);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaTransport_getNativeTransportNamingCtx, bridgeMamaTransportGetNativeTransportNamingCtx, bridgeMamaTransport_getNativeTransportNamingCtx);

    /* Subscription related function pointers */
    REGISTER_BRIDGE_FUNCTION (BridgeMamaSubscription_create, bridgeMamaSubscriptionCreate, bridgeMamaSubscription_create);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaSubscription_createWildCard, bridgeMamaSubscriptionCreateWildCard, bridgeMamaSubscription_createWildCard);
    REGISTER_BRIDGE_FUNCTION (BridgeMamaSubscription_mute, bridgeMamaSubscriptionMute, bridgeMamaSubscription_mute);
    REGISTER_BRIDGE_FUNCTION (BridgeMamaSubscription_destroy, bridgeMamaSubscriptionDestroy, bridgeMamaSubscription_destroy);
    REGISTER_BRIDGE_FUNCTION (BridgeMamaSubscription_isValid, bridgeMamaSubscriptionIsValid, bridgeMamaSubscription_isValid);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaSubscription_hasWildcards, bridgeMamaSubscriptionHasWildcards, bridgeMamaSubscription_hasWildcards);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaSubscription_getPlatformError, bridgeMamaSubscriptionGetPlatformError, bridgeMamaSubscription_getPlatformError);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaSubscription_setTopicClosure, bridgeMamaSubscriptionSetTopicClosure, bridgeMamaSubscription_setTopicClosure);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaSubscription_muteCurrentTopic, bridgeMamaSubscriptionMuteCurrentTopic, bridgeMamaSubscription_muteCurrentTopic);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaSubscription_isTportDisconnected, bridgeMamaSubscriptionIsTportDisconnected, bridgeMamaSubscription_isTportDisconnected);

    /* Timer related function pointers */
    REGISTER_BRIDGE_FUNCTION (BridgeMamaTimer_create, bridgeMamaTimerCreate, bridgeMamaTimer_create);
    REGISTER_BRIDGE_FUNCTION (BridgeMamaTimer_destroy, bridgeMamaTimerDestroy, bridgeMamaTimer_destroy);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaTimer_reset, bridgeMamaTimerReset, bridgeMamaTimer_reset);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaTimer_setInterval, bridgeMamaTimerSetInterval, bridgeMamaTimer_setInterval);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaTimer_getInterval, bridgeMamaTimerGetInterval, bridgeMamaTimer_getInterval);

    /* IO related function pointers */
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaIo_create, bridgeMamaIoCreate, bridgeMamaIo_create);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaIo_getDescriptor, bridgeMamaIoGetDescriptor, bridgeMamaIo_getDescriptor);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaIo_destroy, bridgeMamaIoDestroy, bridgeMamaIo_destroy);

    /* Publisher related function pointers */
    REGISTER_BRIDGE_FUNCTION (BridgeMamaPublisher_createByIndex, bridgeMamaPublisherCreateByIndex, bridgeMamaPublisher_createByIndex);
    REGISTER_BRIDGE_FUNCTION (BridgeMamaPublisher_destroy, bridgeMamaPublisherDestroy, bridgeMamaPublisher_destroy);
    REGISTER_BRIDGE_FUNCTION (BridgeMamaPublisher_send, bridgeMamaPublisherSend, bridgeMamaPublisher_send);
    REGISTER_BRIDGE_FUNCTION (BridgeMamaPublisher_sendFromInbox, bridgeMamaPublisherSendFromInbox, bridgeMamaPublisher_sendFromInbox);
    REGISTER_BRIDGE_FUNCTION (BridgeMamaPublisher_sendFromInboxByIndex, bridgeMamaPublisherSendFromInboxByIndex, bridgeMamaPublisher_sendFromInboxByIndex);
    REGISTER_BRIDGE_FUNCTION (BridgeMamaPublisher_sendReplyToInbox, bridgeMamaPublisherSendReplyToInbox, bridgeMamaPublisher_sendReplyToInbox);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaPublisher_sendReplyToInboxHandle, bridgeMamaPublisherSendReplyToInboxHandle, bridgeMamaPublisher_sendReplyToInboxHandle);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaPublisher_setUserCallbacks, bridgeMamaPublisherSetUserCallbacks, bridgeMamaPublisher_setUserCallbacks);

    /* Inbox related function pointers */
    REGISTER_BRIDGE_FUNCTION (BridgeMamaInbox_create, bridgeMamaInboxCreate, bridgeMamaInbox_create);
    REGISTER_BRIDGE_FUNCTION (BridgeMamaInbox_createByIndex, bridgeMamaInboxCreateByIndex, bridgeMamaInbox_createByIndex);
    REGISTER_BRIDGE_FUNCTION (BridgeMamaInbox_destroy, bridgeMamaInboxDestroy, bridgeMamaInbox_destroy);

    /* Msg related function pointers */
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaMsg_create, bridgeMamaMsgCreate, bridgeMamaMsg_create);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaMsg_destroy, bridgeMamaMsgDestroy, bridgeMamaMsg_destroy);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaMsg_destroyMiddlewareMsg, bridgeMamaMsgDestroyMiddlewareMsg, bridgeMamaMsg_destroyMiddlewareMsg);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaMsg_detach, bridgeMamaMsgDetach, bridgeMamaMsg_detach);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaMsg_isFromInbox, bridgeMamaMsgIsFromInbox, bridgeMamaMsg_isFromInbox);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaMsg_getPlatformError, bridgeMamaMsgGetPlatformError, bridgeMamaMsg_getPlatformError);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaMsg_setSendSubject, bridgeMamaMsgSetSendSubject, bridgeMamaMsg_setSendSubject);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaMsg_getNativeHandle, bridgeMamaMsgGetNativeHandle, bridgeMamaMsg_getNativeHandle);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaMsg_duplicateReplyHandle, bridgeMamaMsgDuplicateReplyHandle, bridgeMamaMsg_duplicateReplyHandle);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaMsg_copyReplyHandle, bridgeMamaMsgCopyReplyHandle, bridgeMamaMsg_copyReplyHandle);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaMsgImpl_setReplyHandle, bridgeMamaMsgSetReplyHandle, bridgeMamaMsgImpl_setReplyHandle);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaMsgImpl_setReplyHandleAndIncrement, bridgeMamaMsgSetReplyHandleAndIncrement, bridgeMamaMsgImpl_setReplyHandleAndIncrement);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (BridgeMamaMsg_destroyReplyHandle, bridgeMamaMsgDestroyReplyHandle, bridgeMamaMsg_destroyReplyHandle);

    /* Return success */
    return status;
}

/**
 * Register function pointers associated with a specific payload.
 */
mama_status
mamaInternal_registerPayloadFunctions (LIB_HANDLE         bridgeLib,
                                       mamaPayloadBridge* bridge,
                                       const char*        name)
{
    mama_status status  = MAMA_STATUS_OK;
    void* result        = NULL;
    char  functionName[256];

    /* Once the payload has been successfully loaded, and the initialization
     * function run, we register the rest of the payload functions.
     */
    REGISTER_BRIDGE_FUNCTION (Payload_create, msgPayloadCreate, msgPayload_create);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_getType, msgPayloadGetType, msgPayload_getType);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_createForTemplate, msgPayloadCreateForTemplate, msgPayload_createForTemplate);
    REGISTER_BRIDGE_FUNCTION (Payload_copy, msgPayloadCopy, msgPayload_copy);
    REGISTER_BRIDGE_FUNCTION (Payload_clear, msgPayloadClear, msgPayload_clear);
    REGISTER_BRIDGE_FUNCTION (Payload_destroy, msgPayloadDestroy, msgPayload_destroy);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_setParent, msgPayloadSetParent, msgPayload_setParent);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_getByteSize, msgPayloadGetByteSize, msgPayload_getByteSize);
    REGISTER_BRIDGE_FUNCTION (Payload_getNumFields, msgPayloadGetNumFields, msgPayload_getNumFields);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_getSendSubject, msgPayloadGetSendSubject, msgPayload_getSendSubject);
    REGISTER_BRIDGE_FUNCTION (Payload_toString, msgPayloadToString, msgPayload_toString);
    REGISTER_BRIDGE_FUNCTION (Payload_iterateFields, msgPayloadIterateFields, msgPayload_iterateFields);
    REGISTER_BRIDGE_FUNCTION (Payload_getByteBuffer, msgPayloadGetByteBuffer, msgPayload_getByteBuffer);
    REGISTER_BRIDGE_FUNCTION (Payload_unSerialize, msgPayloadUnSerialize, msgPayload_unSerialize);
    REGISTER_BRIDGE_FUNCTION (Payload_serialize, msgPayloadSerialize, msgPayload_serialize);
    REGISTER_BRIDGE_FUNCTION (Payload_setByteBuffer, msgPayloadSetByteBuffer, msgPayload_setByteBuffer);
    REGISTER_BRIDGE_FUNCTION (Payload_createFromByteBuffer, msgPayloadCreateFromByteBuffer, msgPayload_createFromByteBuffer);
    REGISTER_BRIDGE_FUNCTION (Payload_apply, msgPayloadApply, msgPayload_apply);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_getNativeMsg, msgPayloadGetNativeMsg, msgPayload_getNativeMsg);
    REGISTER_BRIDGE_FUNCTION (Payload_getFieldAsString, msgPayloadGetFieldAsString, msgPayload_getFieldAsString);

    /* Add methods */
    REGISTER_BRIDGE_FUNCTION (Payload_addBool, msgPayloadAddBool, msgPayload_addBool);
    REGISTER_BRIDGE_FUNCTION (Payload_addChar, msgPayloadAddChar, msgPayload_addChar);
    REGISTER_BRIDGE_FUNCTION (Payload_addI8, msgPayloadAddI8, msgPayload_addI8);
    REGISTER_BRIDGE_FUNCTION (Payload_addU8, msgPayloadAddU8, msgPayload_addU8);
    REGISTER_BRIDGE_FUNCTION (Payload_addI16, msgPayloadAddI16, msgPayload_addI16);
    REGISTER_BRIDGE_FUNCTION (Payload_addU16, msgPayloadAddU16, msgPayload_addU16);
    REGISTER_BRIDGE_FUNCTION (Payload_addI32, msgPayloadAddI32, msgPayload_addI32);
    REGISTER_BRIDGE_FUNCTION (Payload_addU32, msgPayloadAddU32, msgPayload_addU32);
    REGISTER_BRIDGE_FUNCTION (Payload_addI64, msgPayloadAddI64, msgPayload_addI64);
    REGISTER_BRIDGE_FUNCTION (Payload_addU64, msgPayloadAddU64, msgPayload_addU64);
    REGISTER_BRIDGE_FUNCTION (Payload_addF32, msgPayloadAddF32, msgPayload_addF32);
    REGISTER_BRIDGE_FUNCTION (Payload_addF64, msgPayloadAddF64, msgPayload_addF64);
    REGISTER_BRIDGE_FUNCTION (Payload_addString, msgPayloadAddString, msgPayload_addString);
    REGISTER_BRIDGE_FUNCTION (Payload_addOpaque, msgPayloadAddOpaque, msgPayload_addOpaque);
    REGISTER_BRIDGE_FUNCTION (Payload_addDateTime, msgPayloadAddDateTime, msgPayload_addDateTime);
    REGISTER_BRIDGE_FUNCTION (Payload_addPrice, msgPayloadAddPrice, msgPayload_addPrice);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_addMsg, msgPayloadAddMsg, msgPayload_addMsg);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_addVectorBool, msgPayloadAddVectorBool, msgPayload_addVectorBool);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_addVectorChar, msgPayloadAddVectorChar, msgPayload_addVectorChar);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_addVectorI8, msgPayloadAddVectorI8, msgPayload_addVectorI8);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_addVectorU8, msgPayloadAddVectorU8, msgPayload_addVectorU8);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_addVectorI16, msgPayloadAddVectorI16, msgPayload_addVectorI16);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_addVectorU16, msgPayloadAddVectorU16, msgPayload_addVectorU16);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_addVectorI32, msgPayloadAddVectorI32, msgPayload_addVectorI32);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_addVectorU32, msgPayloadAddVectorU32, msgPayload_addVectorU32);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_addVectorI64, msgPayloadAddVectorI64, msgPayload_addVectorI64);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_addVectorU64, msgPayloadAddVectorU64, msgPayload_addVectorU64);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_addVectorF32, msgPayloadAddVectorF32, msgPayload_addVectorF32);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_addVectorF64, msgPayloadAddVectorF64, msgPayload_addVectorF64);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_addVectorString, msgPayloadAddVectorString, msgPayload_addVectorString);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_addVectorMsg, msgPayloadAddVectorMsg, msgPayload_addVectorMsg);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_addVectorDateTime, msgPayloadAddVectorDateTime, msgPayload_addVectorDateTime);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_addVectorPrice, msgPayloadAddVectorPrice, msgPayload_addVectorPrice);

    /* Update methods */
    REGISTER_BRIDGE_FUNCTION (Payload_updateBool, msgPayloadUpdateBool, msgPayload_updateBool);
    REGISTER_BRIDGE_FUNCTION (Payload_updateChar, msgPayloadUpdateChar, msgPayload_updateChar);
    REGISTER_BRIDGE_FUNCTION (Payload_updateU8, msgPayloadUpdateU8, msgPayload_updateU8);
    REGISTER_BRIDGE_FUNCTION (Payload_updateI8, msgPayloadUpdateI8, msgPayload_updateI8);
    REGISTER_BRIDGE_FUNCTION (Payload_updateI16, msgPayloadUpdateI16, msgPayload_updateI16);
    REGISTER_BRIDGE_FUNCTION (Payload_updateU16, msgPayloadUpdateU16, msgPayload_updateU16);
    REGISTER_BRIDGE_FUNCTION (Payload_updateI32, msgPayloadUpdateI32, msgPayload_updateI32);
    REGISTER_BRIDGE_FUNCTION (Payload_updateU32, msgPayloadUpdateU32, msgPayload_updateU32);
    REGISTER_BRIDGE_FUNCTION (Payload_updateI64, msgPayloadUpdateI64, msgPayload_updateI64);
    REGISTER_BRIDGE_FUNCTION (Payload_updateU64, msgPayloadUpdateU64, msgPayload_updateU64);
    REGISTER_BRIDGE_FUNCTION (Payload_updateF32, msgPayloadUpdateF32, msgPayload_updateF32);
    REGISTER_BRIDGE_FUNCTION (Payload_updateF64, msgPayloadUpdateF64, msgPayload_updateF64);
    REGISTER_BRIDGE_FUNCTION (Payload_updateString, msgPayloadUpdateString, msgPayload_updateString);
    REGISTER_BRIDGE_FUNCTION (Payload_updateOpaque, msgPayloadUpdateOpaque, msgPayload_updateOpaque);
    REGISTER_BRIDGE_FUNCTION (Payload_updateDateTime, msgPayloadUpdateDateTime, msgPayload_updateDateTime);
    REGISTER_BRIDGE_FUNCTION (Payload_updatePrice, msgPayloadUpdatePrice, msgPayload_updatePrice);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_updateSubMsg, msgPayloadUpdateSubMsg, msgPayload_updateSubMsg);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_updateVectorMsg, msgPayloadUpdateVectorMsg, msgPayload_updateVectorMsg);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_updateVectorString, msgPayloadUpdateVectorString, msgPayload_updateVectorString);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_updateVectorBool, msgPayloadUpdateVectorBool, msgPayload_updateVectorBool);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_updateVectorChar, msgPayloadUpdateVectorChar, msgPayload_updateVectorChar);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_updateVectorI8, msgPayloadUpdateVectorI8, msgPayload_updateVectorI8);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_updateVectorU8, msgPayloadUpdateVectorU8, msgPayload_updateVectorU8);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_updateVectorI16, msgPayloadUpdateVectorI16, msgPayload_updateVectorI16);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_updateVectorU16, msgPayloadUpdateVectorU16, msgPayload_updateVectorU16);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_updateVectorI32, msgPayloadUpdateVectorI32, msgPayload_updateVectorI32);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_updateVectorU32, msgPayloadUpdateVectorU32, msgPayload_updateVectorU32);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_updateVectorI64, msgPayloadUpdateVectorI64, msgPayload_updateVectorI64);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_updateVectorU64, msgPayloadUpdateVectorU64, msgPayload_updateVectorU64);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_updateVectorF32, msgPayloadUpdateVectorF32, msgPayload_updateVectorF32);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_updateVectorF64, msgPayloadUpdateVectorF64, msgPayload_updateVectorF64);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_updateVectorPrice, msgPayloadUpdateVectorPrice, msgPayload_updateVectorPrice);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_updateVectorPrice, msgPayloadUpdateVectorTime, msgPayload_updateVectorTime);

    /* Get methods */
    REGISTER_BRIDGE_FUNCTION (Payload_getBool, msgPayloadGetBool, msgPayload_getBool);
    REGISTER_BRIDGE_FUNCTION (Payload_getChar, msgPayloadGetChar, msgPayload_getChar);
    REGISTER_BRIDGE_FUNCTION (Payload_getI8, msgPayloadGetI8, msgPayload_getI8);
    REGISTER_BRIDGE_FUNCTION (Payload_getU8, msgPayloadGetU8, msgPayload_getU8);
    REGISTER_BRIDGE_FUNCTION (Payload_getI16, msgPayloadGetI16, msgPayload_getI16);
    REGISTER_BRIDGE_FUNCTION (Payload_getU16, msgPayloadGetU16, msgPayload_getU16);
    REGISTER_BRIDGE_FUNCTION (Payload_getI32, msgPayloadGetI32, msgPayload_getI32);
    REGISTER_BRIDGE_FUNCTION (Payload_getU32, msgPayloadGetU32, msgPayload_getU32);
    REGISTER_BRIDGE_FUNCTION (Payload_getI64, msgPayloadGetI64, msgPayload_getI64);
    REGISTER_BRIDGE_FUNCTION (Payload_getU64, msgPayloadGetU64, msgPayload_getU64);
    REGISTER_BRIDGE_FUNCTION (Payload_getF32, msgPayloadGetF32, msgPayload_getF32);
    REGISTER_BRIDGE_FUNCTION (Payload_getF64, msgPayloadGetF64, msgPayload_getF64);
    REGISTER_BRIDGE_FUNCTION (Payload_getString, msgPayloadGetString, msgPayload_getString);
    REGISTER_BRIDGE_FUNCTION (Payload_getOpaque, msgPayloadGetOpaque, msgPayload_getOpaque);
    REGISTER_BRIDGE_FUNCTION (Payload_getField, msgPayloadGetField, msgPayload_getField);
    REGISTER_BRIDGE_FUNCTION (Payload_getDateTime, msgPayloadGetDateTime, msgPayload_getDateTime);
    REGISTER_BRIDGE_FUNCTION (Payload_getPrice, msgPayloadGetPrice, msgPayload_getPrice);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_getMsg, msgPayloadGetMsg, msgPayload_getMsg);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_getVectorBool, msgPayloadGetVectorBool, msgPayload_getVectorBool);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_getVectorChar, msgPayloadGetVectorChar, msgPayload_getVectorChar);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_getVectorI8, msgPayloadGetVectorI8, msgPayload_getVectorI8);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_getVectorU8, msgPayloadGetVectorU8, msgPayload_getVectorU8);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_getVectorI16, msgPayloadGetVectorI16, msgPayload_getVectorI16);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_getVectorU16, msgPayloadGetVectorU16, msgPayload_getVectorU16);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_getVectorI32, msgPayloadGetVectorI32, msgPayload_getVectorI32);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_getVectorU32, msgPayloadGetVectorU32, msgPayload_getVectorU32);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_getVectorI64, msgPayloadGetVectorI64, msgPayload_getVectorI64);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_getVectorU64, msgPayloadGetVectorU64, msgPayload_getVectorU64);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_getVectorF32, msgPayloadGetVectorF32, msgPayload_getVectorF32);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_getVectorF64, msgPayloadGetVectorF64, msgPayload_getVectorF64);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_getVectorString, msgPayloadGetVectorString, msgPayload_getVectorString);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_getVectorDateTime, msgPayloadGetVectorDateTime, msgPayload_getVectorDateTime);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_getVectorPrice, msgPayloadGetVectorPrice, msgPayload_getVectorPrice);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (Payload_getVectorMsg, msgPayloadGetVectorMsg, msgPayload_getVectorMsg);

    /* msgIter methods */
    REGISTER_BRIDGE_FUNCTION (PayloadIter_create, msgPayloadIterCreate, msgPayloadIter_create);
    REGISTER_BRIDGE_FUNCTION (PayloadIter_next, msgPayloadIterNext, msgPayloadIter_next);
    REGISTER_BRIDGE_FUNCTION (PayloadIter_hasNext, msgPayloadIterHasNext, msgPayloadIter_hasNext);
    REGISTER_BRIDGE_FUNCTION (PayloadIter_begin, msgPayloadIterBegin, msgPayloadIter_begin);
    REGISTER_BRIDGE_FUNCTION (PayloadIter_end, msgPayloadIterEnd, msgPayloadIter_end);
    REGISTER_BRIDGE_FUNCTION (PayloadIter_associate, msgPayloadIterAssociate, msgPayloadIter_associate);
    REGISTER_BRIDGE_FUNCTION (PayloadIter_destroy, msgPayloadIterDestroy, msgPayloadIter_destroy);

    /* msgField methods */
    REGISTER_BRIDGE_FUNCTION (FieldPayload_create, msgFieldPayloadCreate, msgFieldPayload_create);
    REGISTER_BRIDGE_FUNCTION (FieldPayload_destroy, msgFieldPayloadDestroy, msgFieldPayload_destroy);
    REGISTER_BRIDGE_FUNCTION (FieldPayload_getName, msgFieldPayloadGetName, msgFieldPayload_getName);
    REGISTER_BRIDGE_FUNCTION (FieldPayload_getFid, msgFieldPayloadGetFid, msgFieldPayload_getFid);
    REGISTER_BRIDGE_FUNCTION (FieldPayload_getDescriptor, msgFieldPayloadGetDescriptor, msgFieldPayload_getDescriptor);
    REGISTER_BRIDGE_FUNCTION (FieldPayload_getType, msgFieldPayloadGetType, msgFieldPayload_getType);
    REGISTER_BRIDGE_FUNCTION (FieldPayload_updateBool, msgFieldPayloadUpdateBool, msgFieldPayload_updateBool);
    REGISTER_BRIDGE_FUNCTION (FieldPayload_updateChar, msgFieldPayloadUpdateChar, msgFieldPayload_updateChar);
    REGISTER_BRIDGE_FUNCTION (FieldPayload_updateU8, msgFieldPayloadUpdateU8, msgFieldPayload_updateU8);
    REGISTER_BRIDGE_FUNCTION (FieldPayload_updateI8, msgFieldPayloadUpdateI8, msgFieldPayload_updateI8);
    REGISTER_BRIDGE_FUNCTION (FieldPayload_updateI16, msgFieldPayloadUpdateI16, msgFieldPayload_updateI16);
    REGISTER_BRIDGE_FUNCTION (FieldPayload_updateU16, msgFieldPayloadUpdateU16, msgFieldPayload_updateU16);
    REGISTER_BRIDGE_FUNCTION (FieldPayload_updateI32, msgFieldPayloadUpdateI32, msgFieldPayload_updateI32);
    REGISTER_BRIDGE_FUNCTION (FieldPayload_updateU32, msgFieldPayloadUpdateU32, msgFieldPayload_updateU32);
    REGISTER_BRIDGE_FUNCTION (FieldPayload_updateI64, msgFieldPayloadUpdateI64, msgFieldPayload_updateI64);
    REGISTER_BRIDGE_FUNCTION (FieldPayload_updateU64, msgFieldPayloadUpdateU64, msgFieldPayload_updateU64);
    REGISTER_BRIDGE_FUNCTION (FieldPayload_updateF32, msgFieldPayloadUpdateF32, msgFieldPayload_updateF32);
    REGISTER_BRIDGE_FUNCTION (FieldPayload_updateF64, msgFieldPayloadUpdateF64, msgFieldPayload_updateF64);
    REGISTER_BRIDGE_FUNCTION (FieldPayload_updateDateTime, msgFieldPayloadUpdateDateTime, msgFieldPayload_updateDateTime);
    REGISTER_BRIDGE_FUNCTION (FieldPayload_updatePrice, msgFieldPayloadUpdatePrice, msgFieldPayload_updatePrice);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (FieldPayload_updateString, msgFieldPayloadUpdateString, msgFieldPayload_updateString);
    REGISTER_BRIDGE_FUNCTION (FieldPayload_getBool, msgFieldPayloadGetBool, msgFieldPayload_getBool);
    REGISTER_BRIDGE_FUNCTION (FieldPayload_getChar, msgFieldPayloadGetChar, msgFieldPayload_getChar);
    REGISTER_BRIDGE_FUNCTION (FieldPayload_getI8, msgFieldPayloadGetI8, msgFieldPayload_getI8);
    REGISTER_BRIDGE_FUNCTION (FieldPayload_getU8, msgFieldPayloadGetU8, msgFieldPayload_getU8);
    REGISTER_BRIDGE_FUNCTION (FieldPayload_getI16, msgFieldPayloadGetI16, msgFieldPayload_getI16);
    REGISTER_BRIDGE_FUNCTION (FieldPayload_getU16, msgFieldPayloadGetU16, msgFieldPayload_getU16);
    REGISTER_BRIDGE_FUNCTION (FieldPayload_getI32, msgFieldPayloadGetI32, msgFieldPayload_getI32);
    REGISTER_BRIDGE_FUNCTION (FieldPayload_getU32, msgFieldPayloadGetU32, msgFieldPayload_getU32);
    REGISTER_BRIDGE_FUNCTION (FieldPayload_getI64, msgFieldPayloadGetI64, msgFieldPayload_getI64);
    REGISTER_BRIDGE_FUNCTION (FieldPayload_getU64, msgFieldPayloadGetU64, msgFieldPayload_getU64);
    REGISTER_BRIDGE_FUNCTION (FieldPayload_getF32, msgFieldPayloadGetF32, msgFieldPayload_getF32);
    REGISTER_BRIDGE_FUNCTION (FieldPayload_getF64, msgFieldPayloadGetF64, msgFieldPayload_getF64);
    REGISTER_BRIDGE_FUNCTION (FieldPayload_getString, msgFieldPayloadGetString, msgFieldPayload_getString);
    REGISTER_BRIDGE_FUNCTION (FieldPayload_getOpaque, msgFieldPayloadGetOpaque, msgFieldPayload_getOpaque);
    REGISTER_BRIDGE_FUNCTION (FieldPayload_getDateTime, msgFieldPayloadGetDateTime, msgFieldPayload_getDateTime);
    REGISTER_BRIDGE_FUNCTION (FieldPayload_getPrice, msgFieldPayloadGetPrice, msgFieldPayload_getPrice);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (FieldPayload_getMsg, msgFieldPayloadGetMsg, msgFieldPayload_getMsg);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (FieldPayload_getVectorBool, msgFieldPayloadGetVectorBool, msgFieldPayload_getVectorBool);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (FieldPayload_getVectorChar, msgFieldPayloadGetVectorChar, msgFieldPayload_getVectorChar);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (FieldPayload_getVectorI8, msgFieldPayloadGetVectorI8, msgFieldPayload_getVectorI8);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (FieldPayload_getVectorU8, msgFieldPayloadGetVectorU8, msgFieldPayload_getVectorU8);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (FieldPayload_getVectorI16, msgFieldPayloadGetVectorI16, msgFieldPayload_getVectorI16);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (FieldPayload_getVectorU16, msgFieldPayloadGetVectorU16, msgFieldPayload_getVectorU16);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (FieldPayload_getVectorI32, msgFieldPayloadGetVectorI32, msgFieldPayload_getVectorI32);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (FieldPayload_getVectorU32, msgFieldPayloadGetVectorU32, msgFieldPayload_getVectorU32);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (FieldPayload_getVectorI64, msgFieldPayloadGetVectorI64, msgFieldPayload_getVectorI64);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (FieldPayload_getVectorU64, msgFieldPayloadGetVectorU64, msgFieldPayload_getVectorU64);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (FieldPayload_getVectorF32, msgFieldPayloadGetVectorF32, msgFieldPayload_getVectorF32);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (FieldPayload_getVectorF64, msgFieldPayloadGetVectorF64, msgFieldPayload_getVectorF64);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (FieldPayload_getVectorString, msgFieldPayloadGetVectorString, msgFieldPayload_getVectorString);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (FieldPayload_getVectorDateTime, msgFieldPayloadGetVectorDateTime, msgFieldPayload_getVectorDateTime);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (FieldPayload_getVectorPrice, msgFieldPayloadGetVectorPrice, msgFieldPayload_getVectorPrice);
    REGISTER_OPTIONAL_BRIDGE_FUNCTION (FieldPayload_getVectorMsg, msgFieldPayloadGetVectorMsg, msgFieldPayload_getVectorMsg);
    REGISTER_BRIDGE_FUNCTION (FieldPayload_getAsString, msgFieldPayloadGetAsString, msgFieldPayload_getAsString);

    return status;
}
