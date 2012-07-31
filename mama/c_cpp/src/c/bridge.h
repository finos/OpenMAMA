/* $Id: bridge.h,v 1.76.4.2.2.1.4.10 2011/09/07 11:01:05 ianbell Exp $
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

#ifndef MamaBridgeH__
#define MamaBridgeH__


#include "mama/mama.h"
#include "mama/io.h"
#include "mama/subscmsgtype.h"
#include "mamainternal.h"
#include "conflation/manager_int.h"

#if defined(__cplusplus)
extern "C" {
#endif

/* Used to convert a macro argument to a string */
#define xstr(s) str(s)
#define str(s) #s

/***** The types used to identify the bridge specific implementations ******/
typedef struct  queueBridge_*       queueBridge;
typedef struct  subscriptonBridge_* subscriptionBridge;
typedef struct  transportBridge_*   transportBridge;
typedef struct  timerBridge_*       timerBridge;
typedef struct  ioBridge_*          ioBridge;
typedef struct  publisherBridge_*   publisherBridge;
typedef struct  inboxBridge_*       inboxBridge;
typedef struct  msgBridge_*         msgBridge;

/* ******************************************************************************** */
/* Definitions. */
/* ******************************************************************************** */
/* This define is the name of the property containing the timeout value when destroying
 * the default queue.
 */

#define MAMA_BRIDGE_DEFAULT_QUEUE_TIMEOUT_PROPERTY 	"mama.defaultqueue.timeout"

/* The default timeout value when closing the default queue is 2s. */
#define MAMA_BRIDGE_DEFAULT_QUEUE_DEFAULT_TIMEOUT 	2000

/*
 Used in the bridge layer to set all appropriate functions
 on the mamaBridgeImpl struature

 @param implIdentifer The identifier for the middleware rv, lbm, wmw or avis
 @param bridgeImpl    Pointer to the mamaBridgeImpl structure
 */
#define INITIALIZE_BRIDGE(bridgeImpl, implIdentifier)                          \
do                                                                             \
{                                                                              \
    bridgeImpl->mClosure            =   NULL;                                  \
    bridgeImpl->mNativeMsgBridge    =   NULL;                                  \
    /*mama.c function pointers*/                                               \
    bridgeImpl->bridgeOpen          =   implIdentifier ## Bridge_open;         \
    bridgeImpl->bridgeClose         =   implIdentifier ## Bridge_close;        \
    bridgeImpl->bridgeStart         =   implIdentifier ## Bridge_start;        \
    bridgeImpl->bridgeStop          =   implIdentifier ## Bridge_stop;         \
    bridgeImpl->bridgeGetVersion    =   implIdentifier ## Bridge_getVersion;   \
    bridgeImpl->bridgeGetName       =   implIdentifier ## Bridge_getName;      \
    bridgeImpl->bridgeGetDefaultPayloadId = 								   \
    				implIdentifier ## Bridge_getDefaultPayloadId;              \
    /*Queue related function pointers*/                                        \
    bridgeImpl->bridgeMamaQueueCreate   =                                      \
                    implIdentifier ## BridgeMamaQueue_create;                  \
    bridgeImpl->bridgeMamaQueueCreateUsingNative   =                           \
                    implIdentifier ## BridgeMamaQueue_create_usingNative;      \
    bridgeImpl->bridgeMamaQueueDestroy  =                                      \
                    implIdentifier ## BridgeMamaQueue_destroy;                 \
    bridgeImpl->bridgeMamaQueueGetEventCount    =                              \
                    implIdentifier ## BridgeMamaQueue_getEventCount;           \
    bridgeImpl->bridgeMamaQueueDispatch =                                      \
                    implIdentifier ## BridgeMamaQueue_dispatch;                \
    bridgeImpl->bridgeMamaQueueTimedDispatch    =                              \
                    implIdentifier ## BridgeMamaQueue_timedDispatch;           \
    bridgeImpl->bridgeMamaQueueDispatchEvent    =                              \
                    implIdentifier ## BridgeMamaQueue_dispatchEvent;           \
    bridgeImpl->bridgeMamaQueueEnqueueEvent    =                               \
                    implIdentifier ## BridgeMamaQueue_enqueueEvent;            \
    bridgeImpl->bridgeMamaQueueStopDispatch     =                              \
                    implIdentifier ## BridgeMamaQueue_stopDispatch;            \
    bridgeImpl->bridgeMamaQueueSetEnqueueCallback   =                          \
                    implIdentifier ## BridgeMamaQueue_setEnqueueCallback;      \
    bridgeImpl->bridgeMamaQueueRemoveEnqueueCallback    =                      \
                    implIdentifier ## BridgeMamaQueue_removeEnqueueCallback;   \
    bridgeImpl->bridgeMamaQueueGetNativeHandle  =                              \
                    implIdentifier ## BridgeMamaQueue_getNativeHandle;         \
    bridgeImpl->bridgeMamaQueueSetLowWatermark =                               \
                    implIdentifier ## BridgeMamaQueue_setLowWatermark;         \
    bridgeImpl->bridgeMamaQueueSetHighWatermark =                              \
                    implIdentifier ## BridgeMamaQueue_setHighWatermark;        \
    /*Transport related function pointers*/                                    \
    bridgeImpl->bridgeMamaTransportIsValid  =                                  \
                    implIdentifier ## BridgeMamaTransport_isValid;             \
    bridgeImpl->bridgeMamaTransportDestroy  =                                  \
                    implIdentifier ## BridgeMamaTransport_destroy;             \
    bridgeImpl->bridgeMamaTransportCreate   =                                  \
                    implIdentifier ## BridgeMamaTransport_create;              \
    bridgeImpl->bridgeMamaTransportForceClientDisconnect   =                   \
                    implIdentifier ## BridgeMamaTransport_forceClientDisconnect;\
    bridgeImpl->bridgeMamaTransportFindConnection   =                          \
                    implIdentifier ## BridgeMamaTransport_findConnection;      \
    bridgeImpl->bridgeMamaTransportGetAllConnections    =                      \
                    implIdentifier ## BridgeMamaTransport_getAllConnections;   \
    bridgeImpl->bridgeMamaTransportGetAllConnectionsForTopic    =              \
                    implIdentifier ## BridgeMamaTransport_getAllConnectionsForTopic; \
    bridgeImpl->bridgeMamaTransportFreeAllConnections   =                      \
                    implIdentifier ## BridgeMamaTransport_freeAllConnections;  \
    bridgeImpl->bridgeMamaTransportGetAllServerConnections    =                \
                implIdentifier ## BridgeMamaTransport_getAllServerConnections; \
    bridgeImpl->bridgeMamaTransportFreeAllServerConnections   =                \
                implIdentifier ## BridgeMamaTransport_freeAllServerConnections;\
    bridgeImpl->bridgeMamaTransportGetNumLoadBalanceAttributes =               \
            implIdentifier ## BridgeMamaTransport_getNumLoadBalanceAttributes; \
    bridgeImpl->bridgeMamaTransportGetLoadBalanceScheme  =                     \
                    implIdentifier ## BridgeMamaTransport_getLoadBalanceScheme;\
    bridgeImpl->bridgeMamaTransportGetLoadBalanceSharedObjectName  =           \
        implIdentifier ## BridgeMamaTransport_getLoadBalanceSharedObjectName;  \
    bridgeImpl->bridgeMamaTransportSendMsgToConnection =                       \
                implIdentifier ## BridgeMamaTransport_sendMsgToConnection;     \
    bridgeImpl->bridgeMamaTransportIsConnectionIntercepted =                   \
                implIdentifier ## BridgeMamaTransport_isConnectionIntercepted; \
    bridgeImpl->bridgeMamaTransportInstallConnectConflateMgr =                 \
            implIdentifier ## BridgeMamaTransport_installConnectConflateMgr;   \
    bridgeImpl->bridgeMamaTransportUninstallConnectConflateMgr =               \
            implIdentifier ## BridgeMamaTransport_uninstallConnectConflateMgr; \
    bridgeImpl->bridgeMamaTransportStartConnectionConflation =                 \
            implIdentifier ## BridgeMamaTransport_startConnectionConflation;   \
    bridgeImpl->bridgeMamaTransportRequestConflation =                         \
            implIdentifier ## BridgeMamaTransport_requestConflation;           \
    bridgeImpl->bridgeMamaTransportRequestEndConflation =                      \
            implIdentifier ## BridgeMamaTransport_requestEndConflation;        \
    bridgeImpl->bridgeMamaTransportGetNativeTransport   =                      \
            implIdentifier ## BridgeMamaTransport_getNativeTransport;          \
    bridgeImpl->bridgeMamaTransportGetNativeTransportNamingCtx =               \
            implIdentifier ## BridgeMamaTransport_getNativeTransportNamingCtx; \
    /*Subscription related function pointers*/                                 \
    bridgeImpl->bridgeMamaSubscriptionCreate    =                              \
                    implIdentifier ## BridgeMamaSubscription_create;           \
    bridgeImpl->bridgeMamaSubscriptionCreateWildCard    =                      \
                    implIdentifier ## BridgeMamaSubscription_createWildCard;   \
    bridgeImpl->bridgeMamaSubscriptionMute  =                                  \
                    implIdentifier ## BridgeMamaSubscription_mute;             \
    bridgeImpl->bridgeMamaSubscriptionDestroy   =                              \
                    implIdentifier ## BridgeMamaSubscription_destroy;          \
    bridgeImpl->bridgeMamaSubscriptionIsValid   =                              \
                    implIdentifier ## BridgeMamaSubscription_isValid;          \
    bridgeImpl->bridgeMamaSubscriptionHasWildcards  =                          \
                    implIdentifier ## BridgeMamaSubscription_hasWildcards;     \
    bridgeImpl->bridgeMamaSubscriptionGetPlatformError  =                      \
                    implIdentifier ## BridgeMamaSubscription_getPlatformError; \
    bridgeImpl->bridgeMamaSubscriptionSetTopicClosure  =                       \
                    implIdentifier ## BridgeMamaSubscription_setTopicClosure;  \
    bridgeImpl->bridgeMamaSubscriptionMuteCurrentTopic  =                      \
                    implIdentifier ## BridgeMamaSubscription_muteCurrentTopic; \
    bridgeImpl->bridgeMamaSubscriptionIsTportDisconnected  =                   \
                    implIdentifier ## BridgeMamaSubscription_isTportDisconnected;\
    /*Timer related function pointers*/                                        \
    bridgeImpl->bridgeMamaTimerCreate   =                                      \
                    implIdentifier ## BridgeMamaTimer_create;                  \
    bridgeImpl->bridgeMamaTimerDestroy  =                                      \
                    implIdentifier ## BridgeMamaTimer_destroy;                 \
    bridgeImpl->bridgeMamaTimerReset    =                                      \
                    implIdentifier ## BridgeMamaTimer_reset;                   \
    bridgeImpl->bridgeMamaTimerSetInterval  =                                  \
                    implIdentifier ## BridgeMamaTimer_setInterval;             \
    bridgeImpl->bridgeMamaTimerGetInterval  =                                  \
                    implIdentifier ## BridgeMamaTimer_getInterval;             \
    /*IO related function pointers*/                                           \
    bridgeImpl->bridgeMamaIoCreate  =                                          \
                    implIdentifier ## BridgeMamaIo_create;                     \
    bridgeImpl->bridgeMamaIoGetDescriptor   =                                  \
                    implIdentifier ## BridgeMamaIo_getDescriptor;              \
    bridgeImpl->bridgeMamaIoDestroy =                                          \
                    implIdentifier ## BridgeMamaIo_destroy;                    \
    /*Publisher related function pointers*/                                    \
    bridgeImpl->bridgeMamaPublisherCreate =                                    \
                    implIdentifier ## BridgeMamaPublisher_create;              \
    bridgeImpl->bridgeMamaPublisherCreateByIndex =                             \
                    implIdentifier ## BridgeMamaPublisher_createByIndex;       \
    bridgeImpl->bridgeMamaPublisherDestroy =                                   \
                    implIdentifier ## BridgeMamaPublisher_destroy;             \
    bridgeImpl->bridgeMamaPublisherSend =                                      \
                    implIdentifier ## BridgeMamaPublisher_send;                \
    bridgeImpl->bridgeMamaPublisherSendFromInbox    =                          \
                    implIdentifier ## BridgeMamaPublisher_sendFromInbox;       \
    bridgeImpl->bridgeMamaPublisherSendFromInboxByIndex    =                   \
                    implIdentifier ## BridgeMamaPublisher_sendFromInboxByIndex;\
    bridgeImpl->bridgeMamaPublisherSendReplyToInbox =                          \
                    implIdentifier ## BridgeMamaPublisher_sendReplyToInbox;    \
    bridgeImpl->bridgeMamaPublisherSendReplyToInboxHandle =                          \
                    implIdentifier ## BridgeMamaPublisher_sendReplyToInboxHandle;    \
    /*inbox related function pointers*/                                        \
    bridgeImpl->bridgeMamaInboxCreate   =                                      \
                    implIdentifier ## BridgeMamaInbox_create;                  \
    bridgeImpl->bridgeMamaInboxCreateByIndex =                                 \
                    implIdentifier ## BridgeMamaInbox_createByIndex;           \
    bridgeImpl->bridgeMamaInboxDestroy  =                                      \
                    implIdentifier ## BridgeMamaInbox_destroy;                 \
    /*msg related function pointers*/                                          \
    bridgeImpl->bridgeMamaMsgCreate     =                                      \
                    implIdentifier  ## BridgeMamaMsg_create;                   \
    bridgeImpl->bridgeMamaMsgDestroy    =                                      \
                    implIdentifier ## BridgeMamaMsg_destroy;                   \
    bridgeImpl->bridgeMamaMsgDestroyMiddlewareMsg    =                         \
                    implIdentifier ## BridgeMamaMsg_destroyMiddlewareMsg;      \
    bridgeImpl->bridgeMamaMsgDetach     =                                      \
                    implIdentifier ## BridgeMamaMsg_detach;                    \
    bridgeImpl->bridgeMamaMsgIsFromInbox        =                              \
                    implIdentifier ## BridgeMamaMsg_isFromInbox;               \
    bridgeImpl->bridgeMamaMsgGetPlatformError   =                              \
                    implIdentifier ## BridgeMamaMsg_getPlatformError;          \
    bridgeImpl->bridgeMamaMsgSetSendSubject   =                                \
                    implIdentifier ## BridgeMamaMsg_setSendSubject;            \
    bridgeImpl->bridgeMamaMsgGetNativeHandle   =                               \
                    implIdentifier ## BridgeMamaMsg_getNativeHandle;           \
    bridgeImpl->bridgeMamaMsgDuplicateReplyHandle    =                         \
                    implIdentifier ## BridgeMamaMsg_duplicateReplyHandle;      \
    bridgeImpl->bridgeMamaMsgCopyReplyHandle    =                               \
                    implIdentifier ## BridgeMamaMsg_copyReplyHandle;            \
    bridgeImpl->bridgeMamaMsgSetReplyHandle    =                               \
                    implIdentifier ## BridgeMamaMsgImpl_setReplyHandle;        \
   bridgeImpl->bridgeMamaMsgSetReplyHandleAndIncrement    =                    \
                    implIdentifier ## BridgeMamaMsgImpl_setReplyHandleAndIncrement;    \
    bridgeImpl->bridgeMamaMsgDestroyReplyHandle    =                         \
                    implIdentifier ## BridgeMamaMsg_destroyReplyHandle;        \
    /* Register the bridge with Mama */                                        \
    mamaInternal_registerBridge (                                              \
                    (mamaBridge)bridgeImpl,xstr(implIdentifier));              \
}                                                                              \
while(0)                                                                       \

/*===================================================================
 =                      Used in mama.c                              =
 ====================================================================*/
/*Called when loading/creating a bridge */
typedef void (*bridge_createImpl)(mamaBridge* result);

/*Called by mama_open()*/
typedef mama_status (*bridge_open)(mamaBridge bridgeImpl);

/*Called by mama_close()*/
typedef mama_status (*bridge_close)(mamaBridge bridgeImpl);

/*Called by mama_start()*/
typedef mama_status (*bridge_start)(mamaQueue defaultEventQueue);

/*Called by mama_stop()*/
typedef mama_status (*bridge_stop)(mamaQueue defaultEventQueue);

/*Called by mama_getVersion()*/
typedef const char* (*bridge_getVersion)(void);
typedef const char* (*bridge_getName)(void);
typedef mama_status (*bridge_getDefaultPayloadId)(char***name, char** id);

/*===================================================================
 =               mamaQueue bridge function pointers                 =
 ====================================================================*/
typedef mama_status (*bridgeMamaQueue_create)(queueBridge*    queue,
                                              mamaQueue       parent);

typedef mama_status (*bridgeMamaQueue_create_usingNative)(queueBridge* queue,
                                                          mamaQueue    parent,
                                                          void*        nativeQueue);

typedef mama_status (*bridgeMamaQueue_destroy)(queueBridge    queue);

typedef mama_status (*bridgeMamaQueue_getEventCount)(queueBridge  queue,
                                                     size_t*    count);

typedef mama_status (*bridgeMamaQueue_dispatch)(queueBridge queue);

typedef mama_status (*bridgeMamaQueue_timedDispatch)(queueBridge  queue,
                                                     uint64_t   timeout);

typedef mama_status (*bridgeMamaQueue_dispatchEvent)(queueBridge queue);

typedef mama_status (*bridgeMamaQueue_enqueueEvent)(queueBridge        queue,
                                                    mamaQueueEnqueueCB callback,
                                                    void*              closure);

typedef mama_status (*bridgeMamaQueue_stopDispatch)(queueBridge queue);

typedef mama_status (*bridgeMamaQueue_setEnqueueCallback)(
                                                queueBridge         queue,
                                                mamaQueueEnqueueCB callback,
                                                void*              closure);

typedef mama_status (*bridgeMamaQueue_removeEnqueueCallback)
                                               (queueBridge queue);
typedef mama_status (*bridgeMamaQueue_getNativeHandle)

                                               (queueBridge queue,
                                                void**      nativeHandle);

typedef mama_status (*bridgeMamaQueue_setHighWatermark)
                                    (queueBridge queue, size_t highWatermark);

typedef mama_status (*bridgeMamaQueue_setLowWatermark)
                                    (queueBridge queue, size_t lowWatermark);

/*===================================================================
 =               mamaTransport bridge function pointers             =
 ====================================================================*/
/*Return true is the transport is valid*/
typedef int (*bridgeMamaTransport_isValid)(transportBridge transport);

/*Destroy the transport bridge impl*/
typedef mama_status (*bridgeMamaTransport_destroy)(transportBridge transport);

/*Create a transport. The closure can be used in invoking client callbacks.*/
typedef mama_status (*bridgeMamaTransport_create)(transportBridge *result,
                                                  const char*      name,
                                                  mamaTransport    parent);
typedef mama_status (*bridgeMamaTransport_forceClientDisconnect)
                               (transportBridge* transports,
                                int              numTransports,
                                const char*      ipAddress,
                                uint16_t         port);
/* Find a connection with specified IP Address and Port. If the port is 0, the
 * call returns the first connection with the specified IP Address. If a
 * connection is not found the method returns MAMA_STATUS_NOT_FOUND and
 * *result == NULL. The caller is responsible for calling
 * mamaConnection_free().
 *
 * For middleware that does not provide access to per-connection information
 * (non WMW  middleware), the method returns MAMA_STATUS_NOT_IMPL. */
typedef mama_status (*bridgeMamaTransport_findConnection)
                               (transportBridge* transports,
                                int              numTransports,
                                mamaConnection*  result,
                                const char*      ipAddress,
                                uint16_t         port);

/* Return a list of all clients connected to this transport. The result and
 * len must be passed to mamaTransport_freeAllConnections() to free resources
 * allocated by this function.*/
typedef mama_status (*bridgeMamaTransport_getAllConnections)
                                (transportBridge* transports,
                                 int              numTransports,
                                 mamaConnection** result,
                                 uint32_t*        len);

typedef mama_status (*bridgeMamaTransport_getAllConnectionsForTopic)
                                (transportBridge* transports,
                                 int              numTransports,
                                 const char*      topic,
                                 mamaConnection** result,
                                 uint32_t*        len);

typedef mama_status (*bridgeMamaTransport_requestConflation)
                                (transportBridge* transports,
                                 int              numTransports);

typedef mama_status (*bridgeMamaTransport_requestEndConflation)
                                (transportBridge* transports,
                                 int              numTransports);

/* Return a list of all servers connected to this transport. The result and
 * len must be passed to mamaTransport_freeAllServerConnections() to free
 * resources allocated by this function. */
typedef mama_status (*bridgeMamaTransport_getAllServerConnections)
                                (transportBridge*       transports,
                                 int                    numTransports,
                                 mamaServerConnection** result,
                                 uint32_t*              len);

 /* Free resources allocated by mamaTransport_getAllServerConnections()*/
typedef mama_status (*bridgeMamaTransport_freeAllServerConnections)
                                (transportBridge*      transports,
                                 int                   numTransports,
                                 mamaServerConnection* connections,
                                 uint32_t              len);

/* Free resources allocated by mamaTransport_getAllConnections().*/
typedef mama_status (*bridgeMamaTransport_freeAllConnections)
                                (transportBridge*       transports,
                                int                     numTransports,
                                mamaConnection*         connections,
                                uint32_t                len);

/* Return the number of load balance attributes (those which contain lb<n>).*/
typedef mama_status (*bridgeMamaTransport_getNumLoadBalanceAttributes)
                                (const char* name,
                                int*         numLoadBalanceAttributes);

/* Return the name of the load balance shared object attribute. This may be
* used to implement alternative load balance schemes beyond round robin. */
typedef mama_status (*bridgeMamaTransport_getLoadBalanceSharedObjectName)
                                (const char*  name,
                                 const char** loadBalanceSharedObjectName);

/*Return the load balancing scheme to be used */
typedef mama_status (*bridgeMamaTransport_getLoadBalanceScheme)
                                (const char*     name,
                                 tportLbScheme*  scheme);

typedef mama_status (*bridgeMamaTransport_sendMsgToConnection)
                                (transportBridge transport,
                                 mamaConnection  connection,
                                 mamaMsg         msg,
                                 const char*     topic);

/* Return true if the specified connection is intercepted. We should not need
 * the transport to determine this.*/
typedef mama_status (*bridgeMamaTransport_isConnectionIntercepted)
                                (mamaConnection connection,
                                 uint8_t* result);

/* Install a connection conflation manager. A subsequent call to
 * transportBridge_startConnectionConflation() will start instercepting
 * message for the specified connection. This method performs any setup
 * required. */
typedef mama_status (*bridgeMamaTransport_installConnectConflateMgr)
                                (transportBridge       handle,
                                 mamaConflationManager mgr,
                                 mamaConnection        connection,
                                 conflateProcessCb     processCb,
                                 conflateGetMsgCb      msgCb);

typedef mama_status (*bridgeMamaTransport_uninstallConnectConflateMgr)
                                (transportBridge       handle,
                                 mamaConflationManager mgr,
                                 mamaConnection        connection);

/* Start intercepting messages. */
typedef mama_status (*bridgeMamaTransport_startConnectionConflation)
                               (transportBridge        handle,
                                mamaConflationManager  mgr,
                                mamaConnection         connection);

typedef mama_status (*bridgeMamaTransport_getNativeTransport)
                                (transportBridge transport,
                                 void**          nativeTport);

typedef mama_status (*bridgeMamaTransport_getNativeTransportNamingCtx)
                                (transportBridge transport,
                                 void**          nativeTportNamingCtx);

/*===================================================================
 =               mamaSubscription bridge function pointers          =
 ====================================================================*/
/*Create the bridge subscription structure.*/
typedef mama_status (*bridgeMamaSubscription_create)
                               (subscriptionBridge* subsc_,
                                const char*         source,
                                const char*         symbol,
                                mamaTransport       transport,
                                mamaQueue           queue,
                                mamaMsgCallbacks    callback,
                                mamaSubscription    subscription,
                                void*               closure );

/*Create the bridge subscription structure for wildcard subscription .*/
typedef mama_status (*bridgeMamaSubscription_createWildCard)
                               (subscriptionBridge* subsc_,
                                const char*         source,
                                const char*         symbol,
                                mamaTransport       transport,
                                mamaQueue           queue,
                                mamaMsgCallbacks    callback,
                                mamaSubscription    subscription,
                                void*               closure);

/* Mute the subscriber.  No message callbacks will be sent.  A
 * subscription is deactivated as part of its destruction; however,
 * subscription destruction can be delayed and we don't want to
 * receive updates.*/
typedef mama_status (*bridgeMamaSubscription_mute)
                                (subscriptionBridge subscriber);

/* Destroy the subscriber. */
typedef mama_status (*bridgeMamaSubscription_destroy)
                                (subscriptionBridge subscriber);

/*Whether the subscription impl is valid*/
typedef int (*bridgeMamaSubscription_isValid)
                                (subscriptionBridge bridge);

/* Return true if the subject contains wildcards. Some messaging APIs may
 * not support wildcards. */
typedef int (*bridgeMamaSubscription_hasWildcards)
                                (subscriptionBridge subscriber);

/*Get the last error specific to the platform.*/
typedef mama_status (*bridgeMamaSubscription_getPlatformError)
                                (subscriptionBridge subsc, void** error);

/* Return true if the subject transport is disconnected. */
typedef int (*bridgeMamaSubscription_isTportDisconnected)
                                (subscriptionBridge subscriber);

/*Set the topic closure for transport or wild card subscriptions */
typedef mama_status (*bridgeMamaSubscription_setTopicClosure)
                                (subscriptionBridge subsc, void* closure);

/*Mute current topic for "transport subscriptions" WMW only */
typedef mama_status (*bridgeMamaSubscription_muteCurrentTopic)
                                (subscriptionBridge subsc);

/*===================================================================
 =               mamaTimer bridge function pointers          =
 ====================================================================*/
typedef mama_status (*bridgeMamaTimer_create)(timerBridge* timer,
                                              void*        nativeQueueHandle,
                                              mamaTimerCb  action,
                                              mamaTimerCb  onTimerDestroyed,
                                              mama_f64_t   interval,
                                              mamaTimer    parent,
                                              void*        closure);

typedef mama_status (*bridgeMamaTimer_destroy)(timerBridge timer);

typedef mama_status (*bridgeMamaTimer_reset)(timerBridge timer);

typedef mama_status (*bridgeMamaTimer_setInterval)(timerBridge timer,
                                                   mama_f64_t  interval);

typedef mama_status (*bridgeMamaTimer_getInterval)(timerBridge timer,
                                                   mama_f64_t* interval);

/*===================================================================
 =               mamaIo bridge function pointers          =
 ====================================================================*/
typedef mama_status (*bridgeMamaIo_create)(ioBridge*    result,
                                           void*        nativeQueueHandle,
                                           uint32_t     descriptor,
                                           mamaIoCb     action,
                                           mamaIoType   ioType,
                                           mamaIo       parent,
                                           void*        closure);

typedef mama_status (*bridgeMamaIo_getDescriptor)(ioBridge io, uint32_t* d);

typedef mama_status (*bridgeMamaIo_destroy)(ioBridge io);

/*===================================================================
 =               mamaPublisher bridge function pointers          =
 ====================================================================*/

typedef mama_status
(*bridgeMamaPublisher_createByIndex)(publisherBridge*   result,
                                     mamaTransport      tport,
                                     int                tportIndex,
                                     const char*        topic,
                                     const char*        source,
                                     const char*        root,
                                     void*              nativeQueueHandle,
                                     mamaPublisher      parent);

typedef mama_status
(*bridgeMamaPublisher_create)(publisherBridge*  result,
                              mamaTransport     tport,
                              const char*       topic,
                              const char*       source,
                              const char*       root,
                              void*             nativeQueueHandle,
                              mamaPublisher     parent);

typedef mama_status
(*bridgeMamaPublisher_destroy)(publisherBridge publisher);

typedef mama_status
(*bridgeMamaPublisher_send)(publisherBridge publisher,
                            mamaMsg         msg);

typedef mama_status
(*bridgeMamaPublisher_sendReplyToInbox)(publisherBridge publisher,
                                        void *         request,
                                        mamaMsg         reply);

typedef mama_status
(*bridgeMamaPublisher_sendReplyToInboxHandle)(publisherBridge publisher,
                                              void*           request,
                                              mamaMsg         reply);

typedef mama_status
(*bridgeMamaPublisher_sendFromInboxByIndex)(publisherBridge publisher,
                                            int             tportIndex,
                                            mamaInbox       inbox,
                                            mamaMsg         msg);
typedef mama_status
(*bridgeMamaPublisher_sendFromInbox)(publisherBridge publisher,
                                     mamaInbox       inbox,
                                     mamaMsg         msg);


/*===================================================================
 =               mamaInbox bridge function pointers                 =
 ====================================================================*/
typedef mama_status
(*bridgeMamaInbox_create)(
        inboxBridge*           bridge,
        mamaTransport          tport,
        mamaQueue              queue,
        mamaInboxMsgCallback   msgCB,
        mamaInboxErrorCallback errorCB,
        mamaInboxDestroyCallback onInboxDestroyed,
        void*                  closure,
        mamaInbox              parent);

typedef mama_status
(*bridgeMamaInbox_createByIndex)
        (inboxBridge*           bridge,
         mamaTransport          tport,
         int                    tportIndex,
         mamaQueue              queue,
         mamaInboxMsgCallback   msgCB,
         mamaInboxErrorCallback errorCB,
         mamaInboxDestroyCallback onInboxDestroyed,
         void*                  closure,
         mamaInbox              parent);

typedef mama_status
(*bridgeMamaInbox_destroy)(inboxBridge inbox);

/*===================================================================
 =               mamaMsg bridge function pointers                   =
 ====================================================================*/
typedef mama_status
(*bridgeMamaMsg_create)(msgBridge* msg, mamaMsg parent);

typedef int
(*bridgeMamaMsg_isFromInbox)(msgBridge msg);

typedef mama_status
(*bridgeMamaMsg_destroy)(msgBridge msg, int destroyMsg);

typedef mama_status
(*bridgeMamaMsg_destroyMiddlewareMsg)(msgBridge msg);

typedef mama_status
(*bridgeMamaMsg_detach)(msgBridge msg);

typedef mama_status
(*bridgeMamaMsg_getPlatformError)(msgBridge msg, void** error);

typedef mama_status
(*bridgeMamaMsg_setSendSubject) (msgBridge   msg,
                                 const char* symbol,
                                 const char* subject);

typedef mama_status
(*bridgeMamaMsg_getNativeHandle) (msgBridge msg, void** result);

typedef mama_status
(*bridgeMamaMsg_duplicateReplyHandle) (msgBridge msg, void** result);

typedef mama_status
(*bridgeMamaMsg_copyReplyHandle) (void* src, void** dest);

typedef mama_status
(*bridgeMamaMsgImpl_setReplyHandle) (msgBridge msg, void* result);

typedef mama_status
(*bridgeMamaMsgImpl_setReplyHandleAndIncrement) (msgBridge msg, void* result);

typedef mama_status
(*bridgeMamaMsg_destroyReplyHandle) (void* result);

/*
   The structure of function pointers to specific implementations of bridge
   functions.

   Also contains the default event queue used for each implementation.
   (mama_start() needs to be called for each implementation
    mama_open() needs to be called for each implementation)
 */
typedef struct mamaBridgeImpl
{
    /* Used by mama_start() and mama_stop(). */
    unsigned int mRefCount;

    /*The default event queue is now middleware specific. (Originally global)*/
    mamaQueue mDefaultEventQueue;

    /*The internal queue is for use by useagelogger / prop server etc so
    they are not affected by starting stopping default queue*/
    mamaQueue mInternalEventQueue;
    mamaDispatcher mInternalDispatcher;

    /*Associate arbitrary data with a bridge impl. Needed for the C++ wrapper*/
    void*     mClosure;

    /* The set of methods used to access/populate a message in native format */
    mamaPayloadBridge mNativeMsgBridge;

    /* Optional bridge info callback (LBM) */
    bridgeInfoCallback mBridgeInfoCallback;

    /* Needed to call the C++ bridge callback.  Can't use the closure since it's
       used when getting the default queue */
    void*     mCppCallback;

    /*Used in mama.c*/
    bridge_open         			bridgeOpen;
    bridge_close        			bridgeClose;
    bridge_start        			bridgeStart;
    bridge_stop         			bridgeStop;
    bridge_getVersion   			bridgeGetVersion;
    bridge_getName      			bridgeGetName;
    bridge_getDefaultPayloadId   	bridgeGetDefaultPayloadId;

    /*Queue bridge functions*/
    bridgeMamaQueue_create                  bridgeMamaQueueCreate;
    bridgeMamaQueue_create_usingNative      bridgeMamaQueueCreateUsingNative;
    bridgeMamaQueue_destroy                 bridgeMamaQueueDestroy;
    bridgeMamaQueue_getEventCount           bridgeMamaQueueGetEventCount;
    bridgeMamaQueue_dispatch                bridgeMamaQueueDispatch;
    bridgeMamaQueue_timedDispatch           bridgeMamaQueueTimedDispatch;
    bridgeMamaQueue_dispatchEvent           bridgeMamaQueueDispatchEvent;
    bridgeMamaQueue_enqueueEvent            bridgeMamaQueueEnqueueEvent;
    bridgeMamaQueue_stopDispatch            bridgeMamaQueueStopDispatch;
    bridgeMamaQueue_setEnqueueCallback      bridgeMamaQueueSetEnqueueCallback;
    bridgeMamaQueue_removeEnqueueCallback
                                bridgeMamaQueueRemoveEnqueueCallback;
    bridgeMamaQueue_getNativeHandle         bridgeMamaQueueGetNativeHandle;
    bridgeMamaQueue_setLowWatermark         bridgeMamaQueueSetLowWatermark;
    bridgeMamaQueue_setHighWatermark        bridgeMamaQueueSetHighWatermark;

    /*Transport bridge functions*/
    bridgeMamaTransport_isValid             bridgeMamaTransportIsValid;
    bridgeMamaTransport_destroy             bridgeMamaTransportDestroy;
    bridgeMamaTransport_create              bridgeMamaTransportCreate;
    bridgeMamaTransport_forceClientDisconnect
                                bridgeMamaTransportForceClientDisconnect;
    bridgeMamaTransport_findConnection      bridgeMamaTransportFindConnection;
    bridgeMamaTransport_getAllConnections
                                bridgeMamaTransportGetAllConnections;
    bridgeMamaTransport_getAllConnectionsForTopic
                                bridgeMamaTransportGetAllConnectionsForTopic;
    bridgeMamaTransport_freeAllConnections
                                bridgeMamaTransportFreeAllConnections;
    bridgeMamaTransport_getAllServerConnections
                                bridgeMamaTransportGetAllServerConnections;
    bridgeMamaTransport_freeAllServerConnections
                                bridgeMamaTransportFreeAllServerConnections;
    bridgeMamaTransport_getNumLoadBalanceAttributes
                                bridgeMamaTransportGetNumLoadBalanceAttributes;
    bridgeMamaTransport_getLoadBalanceSharedObjectName
                            bridgeMamaTransportGetLoadBalanceSharedObjectName;
    bridgeMamaTransport_getLoadBalanceScheme
                                bridgeMamaTransportGetLoadBalanceScheme;
    bridgeMamaTransport_sendMsgToConnection
                                bridgeMamaTransportSendMsgToConnection;
    bridgeMamaTransport_isConnectionIntercepted
                                bridgeMamaTransportIsConnectionIntercepted;
    bridgeMamaTransport_installConnectConflateMgr
                                bridgeMamaTransportInstallConnectConflateMgr;
    bridgeMamaTransport_uninstallConnectConflateMgr
                            bridgeMamaTransportUninstallConnectConflateMgr;
    bridgeMamaTransport_startConnectionConflation
                                bridgeMamaTransportStartConnectionConflation;
    bridgeMamaTransport_requestConflation
                                bridgeMamaTransportRequestConflation;
    bridgeMamaTransport_requestEndConflation
                                bridgeMamaTransportRequestEndConflation;
    bridgeMamaTransport_getNativeTransport
                                bridgeMamaTransportGetNativeTransport;
    bridgeMamaTransport_getNativeTransportNamingCtx
                                bridgeMamaTransportGetNativeTransportNamingCtx;

    /*Subscription bridge functions*/
    bridgeMamaSubscription_create           bridgeMamaSubscriptionCreate;
    bridgeMamaSubscription_createWildCard   bridgeMamaSubscriptionCreateWildCard;
    bridgeMamaSubscription_mute             bridgeMamaSubscriptionMute;
    bridgeMamaSubscription_destroy          bridgeMamaSubscriptionDestroy;
    bridgeMamaSubscription_isValid          bridgeMamaSubscriptionIsValid;
    bridgeMamaSubscription_hasWildcards     bridgeMamaSubscriptionHasWildcards;
    bridgeMamaSubscription_getPlatformError bridgeMamaSubscriptionGetPlatformError;
    bridgeMamaSubscription_setTopicClosure  bridgeMamaSubscriptionSetTopicClosure;
    bridgeMamaSubscription_muteCurrentTopic bridgeMamaSubscriptionMuteCurrentTopic;
    bridgeMamaSubscription_isTportDisconnected bridgeMamaSubscriptionIsTportDisconnected;

    /*Timer bridge functions*/
    bridgeMamaTimer_create                  bridgeMamaTimerCreate;
    bridgeMamaTimer_destroy                 bridgeMamaTimerDestroy;
    bridgeMamaTimer_reset                   bridgeMamaTimerReset;
    bridgeMamaTimer_setInterval             bridgeMamaTimerSetInterval;
    bridgeMamaTimer_getInterval             bridgeMamaTimerGetInterval;

    /*IO bridge functions*/
    bridgeMamaIo_create                     bridgeMamaIoCreate;
    bridgeMamaIo_getDescriptor              bridgeMamaIoGetDescriptor;
    bridgeMamaIo_destroy                    bridgeMamaIoDestroy;

    /*mamaPublisher bridge functions*/
    bridgeMamaPublisher_create              bridgeMamaPublisherCreate;
    bridgeMamaPublisher_createByIndex       bridgeMamaPublisherCreateByIndex;
    bridgeMamaPublisher_destroy             bridgeMamaPublisherDestroy;
    bridgeMamaPublisher_send                bridgeMamaPublisherSend;
    bridgeMamaPublisher_sendFromInbox       bridgeMamaPublisherSendFromInbox;
    bridgeMamaPublisher_sendFromInboxByIndex
                                        bridgeMamaPublisherSendFromInboxByIndex;
    bridgeMamaPublisher_sendReplyToInbox
                                bridgeMamaPublisherSendReplyToInbox;
    bridgeMamaPublisher_sendReplyToInboxHandle
                                bridgeMamaPublisherSendReplyToInboxHandle;
    /*mamaInbox bridge functions*/
    bridgeMamaInbox_create                  bridgeMamaInboxCreate;
    bridgeMamaInbox_createByIndex           bridgeMamaInboxCreateByIndex;
    bridgeMamaInbox_destroy                 bridgeMamaInboxDestroy;

    /*mamaMsg bridge functions*/
    bridgeMamaMsg_create                    bridgeMamaMsgCreate;
    bridgeMamaMsg_isFromInbox               bridgeMamaMsgIsFromInbox;
    bridgeMamaMsg_getPlatformError          bridgeMamaMsgGetPlatformError;
    bridgeMamaMsg_detach                    bridgeMamaMsgDetach;
    bridgeMamaMsg_destroy                   bridgeMamaMsgDestroy;
    bridgeMamaMsg_destroyMiddlewareMsg      bridgeMamaMsgDestroyMiddlewareMsg;
    bridgeMamaMsg_setSendSubject            bridgeMamaMsgSetSendSubject;
    bridgeMamaMsg_getNativeHandle           bridgeMamaMsgGetNativeHandle;
    bridgeMamaMsg_duplicateReplyHandle      bridgeMamaMsgDuplicateReplyHandle;
    bridgeMamaMsg_copyReplyHandle           bridgeMamaMsgCopyReplyHandle;
    bridgeMamaMsgImpl_setReplyHandle        bridgeMamaMsgSetReplyHandle;
    bridgeMamaMsgImpl_setReplyHandle        bridgeMamaMsgSetReplyHandleAndIncrement;
    bridgeMamaMsg_destroyReplyHandle        bridgeMamaMsgDestroyReplyHandle;
} mamaBridgeImpl;

/*Functions for internal use only. Will be used from the C++ layer.*/

/**
 * This function will return the timeout value to be used when destroying the default queue.
 * The value is read from the properties file, (MAMA_BRIDGE_DEFAULT_QUEUE_TIMEOUT_PROPERTY),
 * but will default to MAMA_BRIDGE_DEFAULT_QUEUE_DEFAULT_TIMEOUT if the property is missing.
 *
 * @return The timeout value.
 */
MAMAExpDLL
extern int
mamaBridgeImpl_getDefaultQueueTimeout(void);

MAMAExpDLL
extern mama_status
mamaBridgeImpl_setClosure (mamaBridge bridgeImpl, void* closure);

MAMAExpDLL
extern mama_status
mamaBridgeImpl_setCppCallback (mamaBridge bridgeImpl, void* cppCallback);

MAMAExpDLL
extern mama_status
mamaBridgeImpl_getClosure (mamaBridge bridgeImpl, void** closure);

MAMAExpDLL
mama_status
mamaBridgeImpl_getInternalEventQueue (mamaBridge bridgeImpl, mamaQueue* internalQueue);

MAMAExpDLL
mama_status
mamaBridgeImpl_stopInternalEventQueue (mamaBridge bridgeImpl);

#if defined(__cplusplus)
}
#endif

#endif /* MamaBridgeH__ */
