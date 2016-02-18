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

#ifndef AVIS_BRIDGE_FUNCTIONS__
#define AVIS_BRIDGE_FUNCTIONS__

#include <mama/mama.h>
#include <bridge.h>

#if defined(__cplusplus)
extern "C" {
#endif

/*Definitions for all of the Avis bridge functions which will be used by the
 * code MAMA code when delegating calls to the bridge*/

 /*=========================================================================
  =                    Functions for the bridge                           =
  =========================================================================*/
MAMAExpDLL
extern void
avisBridge_createImpl (mamaBridge* result);

MAMAExpDLL
extern mama_status
avisBridge_init (mamaBridge bridgeImpl);

MAMAExpDLL
extern const char*
avisBridge_getVersion (void);

MAMAExpDLL
mama_status
avisBridge_getDefaultPayloadId (char***name, char** id);

MAMAExpDLL
extern mama_status
avisBridge_open (mamaBridge bridgeImpl);

MAMAExpDLL
extern mama_status
avisBridge_close (mamaBridge bridgeImpl);

MAMAExpDLL
extern mama_status
avisBridge_start (mamaQueue defaultEventQueue);

MAMAExpDLL
extern mama_status
avisBridge_stop (mamaQueue defaultEventQueue);

MAMAExpDLL
extern const char*
avisBridge_getName (void);


/*=========================================================================
  =                    Functions for the mamaQueue                        =
  =========================================================================*/
MAMAExpDLL
extern mama_status
avisBridgeMamaQueue_create (queueBridge *queue, mamaQueue parent);

MAMAExpDLL
extern mama_status
avisBridgeMamaQueue_create_usingNative (queueBridge *queue, mamaQueue parent, void* nativeQueue);

MAMAExpDLL
extern mama_status
avisBridgeMamaQueue_destroy (queueBridge queue);

MAMAExpDLL
extern mama_status
avisBridgeMamaQueue_getEventCount (queueBridge queue, size_t* count);

MAMAExpDLL
extern mama_status
avisBridgeMamaQueue_dispatch (queueBridge queue);

MAMAExpDLL
extern mama_status
avisBridgeMamaQueue_timedDispatch (queueBridge queue, uint64_t timeout);

MAMAExpDLL
extern mama_status
avisBridgeMamaQueue_dispatchEvent (queueBridge queue);

MAMAExpDLL
extern mama_status
avisBridgeMamaQueue_enqueueEvent (queueBridge        queue,
                                   mamaQueueEnqueueCB callback,
                                   void*              closure);

MAMAExpDLL
extern mama_status
avisBridgeMamaQueue_stopDispatch (queueBridge queue);

MAMAExpDLL
extern mama_status
avisBridgeMamaQueue_setEnqueueCallback (queueBridge        queue,
                                         mamaQueueEnqueueCB callback,
                                         void*              closure);

MAMAExpDLL
extern mama_status
avisBridgeMamaQueue_removeEnqueueCallback (queueBridge queue);

MAMAExpDLL
extern mama_status
avisBridgeMamaQueue_getNativeHandle (queueBridge queue,
                                    void**      result);

MAMAExpDLL
extern mama_status
avisBridgeMamaQueue_setLowWatermark (queueBridge queue,
                                    size_t      lowWatermark);
MAMAExpDLL
extern mama_status
avisBridgeMamaQueue_setHighWatermark (queueBridge queue,
                                     size_t      highWatermark);
/*=========================================================================
  =                    Functions for the mamaTransport                    =
  =========================================================================*/
MAMAExpDLL
extern int
avisBridgeMamaTransport_isValid (transportBridge transport);

MAMAExpDLL
extern mama_status
avisBridgeMamaTransport_destroy (transportBridge transport);

MAMAExpDLL
extern mama_status
avisBridgeMamaTransport_create (transportBridge* result,
                                 const char*      name,
                                 mamaTransport    parent);
MAMAExpDLL
extern mama_status
avisBridgeMamaTransport_forceClientDisconnect (transportBridge* transports,
                                              int              numTransports,
                                              const char*      ipAddress,
                                              uint16_t         port);
MAMAExpDLL
extern mama_status
avisBridgeMamaTransport_findConnection (transportBridge* transports,
                                       int              numTransports,
                                       mamaConnection*  result,
                                       const char*      ipAddress,
                                       uint16_t         port);
MAMAExpDLL
extern mama_status
avisBridgeMamaTransport_getAllConnections (transportBridge* transports,
                                          int              numTransports,
                                          mamaConnection** result,
                                          uint32_t*        len);

MAMAExpDLL
extern mama_status
avisBridgeMamaTransport_getAllConnectionsForTopic (transportBridge* transports,
                                                    int              numTransports,
                                                    const char*      topic,
                                                    mamaConnection** result,
                                                    uint32_t*        len);
MAMAExpDLL
extern mama_status
avisBridgeMamaTransport_freeAllConnections (transportBridge* transports,
                                           int              numTransports,
                                           mamaConnection*  connections,
                                           uint32_t         len);
MAMAExpDLL
extern mama_status
avisBridgeMamaTransport_getAllServerConnections (
                                        transportBridge*       transports,
                                        int                    numTransports,
                                        mamaServerConnection** result,
                                        uint32_t*              len);

MAMAExpDLL
extern mama_status
avisBridgeMamaTransport_freeAllServerConnections (
                                        transportBridge*        transports,
                                        int                     numTransports,
                                        mamaServerConnection*   connections,
                                        uint32_t                len);

MAMAExpDLL
extern mama_status
avisBridgeMamaTransport_getNumLoadBalanceAttributes (
                                    const char* name,
                                    int*        numLoadBalanceAttributes);
MAMAExpDLL
extern mama_status
avisBridgeMamaTransport_getLoadBalanceSharedObjectName (
                                    const char*  name,
                                    const char** loadBalanceSharedObjectName);

MAMAExpDLL
extern mama_status
avisBridgeMamaTransport_getLoadBalanceScheme (const char*    name,
                                             tportLbScheme* scheme);

MAMAExpDLL
extern mama_status
avisBridgeMamaTransport_sendMsgToConnection (
                                    transportBridge transport,
                                    mamaConnection  connection,
                                    mamaMsg         msg,
                                    const char*     topic);
MAMAExpDLL
extern mama_status
avisBridgeMamaTransport_isConnectionIntercepted (
                                    mamaConnection connection,
                                    uint8_t* result);

MAMAExpDLL
extern mama_status
avisBridgeMamaTransport_installConnectConflateMgr (
                                    transportBridge       transport,
                                    mamaConflationManager mgr,
                                    mamaConnection        connection,
                                    conflateProcessCb     processCb,
                                    conflateGetMsgCb      msgCb);

MAMAExpDLL
extern mama_status
avisBridgeMamaTransport_uninstallConnectConflateMgr (
                                    transportBridge       transport,
                                    mamaConflationManager mgr,
                                    mamaConnection        connection);

MAMAExpDLL
extern mama_status
avisBridgeMamaTransport_startConnectionConflation (
                                    transportBridge        transport,
                                    mamaConflationManager  mgr,
                                    mamaConnection         connection);

MAMAExpDLL
extern mama_status
avisBridgeMamaTransport_requestConflation (transportBridge* transports,
                                            int              numTransports);
MAMAExpDLL
extern mama_status
avisBridgeMamaTransport_requestEndConflation (transportBridge* transports,
                                               int              numTransports);

MAMAExpDLL
extern mama_status
avisBridgeMamaTransport_getNativeTransport (transportBridge transport,
                                             void**          result);
MAMAExpDLL
extern mama_status
avisBridgeMamaTransport_getNativeTransportNamingCtx (transportBridge transport,
                                                      void**          result);

/*=========================================================================
  =                    Functions for the mamaSubscription                 =
  =========================================================================*/
MAMAExpDLL
extern mama_status avisBridgeMamaSubscription_create
                               (subscriptionBridge* subsc_,
                                const char*         source,
                                const char*         symbol,
                                mamaTransport       transport,
                                mamaQueue           queue,
                                mamaMsgCallbacks    callback,
                                mamaSubscription    subscription,
                                void*               closure );

MAMAExpDLL
extern mama_status
avisBridgeMamaSubscription_createWildCard (
                                subscriptionBridge* subsc_,
                                const char*         source,
                                const char*         symbol,
                                mamaTransport       transport,
                                mamaQueue           queue,
                                mamaMsgCallbacks    callback,
                                mamaSubscription    subscription,
                                void*               closure );

MAMAExpDLL
extern mama_status avisBridgeMamaSubscription_mute
                                (subscriptionBridge subscriber);

MAMAExpDLL
extern  mama_status avisBridgeMamaSubscription_destroy
                                (subscriptionBridge subscriber);

MAMAExpDLL
extern int avisBridgeMamaSubscription_isValid
                                (subscriptionBridge bridge);

MAMAExpDLL
extern mama_status avisBridgeMamaSubscription_getSubject
                                (subscriptionBridge subscriber,
                                 const char**       subject);

MAMAExpDLL
extern int avisBridgeMamaSubscription_hasWildcards
                                (subscriptionBridge subscriber);

MAMAExpDLL
extern mama_status avisBridgeMamaSubscription_getPlatformError
                                (subscriptionBridge subsc, void** error);

MAMAExpDLL
extern mama_status avisBridgeMamaSubscription_setTopicClosure
                                (subscriptionBridge subsc, void* closure);

MAMAExpDLL
extern mama_status avisBridgeMamaSubscription_muteCurrentTopic
                                (subscriptionBridge subsc);

MAMAExpDLL
extern int avisBridgeMamaSubscription_isTportDisconnected
                                (subscriptionBridge subsc);

/*=========================================================================
  =                    Functions for the mamaTimer                        =
  =========================================================================*/
MAMAExpDLL
extern mama_status avisBridgeMamaTimer_create (timerBridge* timer,
                                              void*        nativeQueueHandle,
                                              mamaTimerCb  action,
                                              mamaTimerCb  onTimerDestroyed,
                                              mama_f64_t   interval,
                                              mamaTimer    parent,
                                              void*        closure);

MAMAExpDLL
extern mama_status avisBridgeMamaTimer_destroy (timerBridge timer);

MAMAExpDLL
extern mama_status avisBridgeMamaTimer_reset (timerBridge timer);

MAMAExpDLL
extern mama_status avisBridgeMamaTimer_setInterval (timerBridge timer,
                                                   mama_f64_t  interval);

MAMAExpDLL
extern mama_status avisBridgeMamaTimer_getInterval (timerBridge timer,
                                                   mama_f64_t* interval);

/*=========================================================================
  =                    Functions for the mamaIo                           =
  =========================================================================*/
MAMAExpDLL
extern mama_status
avisBridgeMamaIo_create (ioBridge*       result,
                        void*           nativeQueueHandle,
                        uint32_t        descriptor,
                        mamaIoCb        action,
                        mamaIoType      ioType,
                        mamaIo          parent,
                        void*           closure);

MAMAExpDLL
extern mama_status
avisBridgeMamaIo_destroy (ioBridge io);

MAMAExpDLL
extern mama_status
avisBridgeMamaIo_getDescriptor (ioBridge io, uint32_t* result);


/*=========================================================================
  =                    Functions for the mamaPublisher                    =
  =========================================================================*/
MAMAExpDLL
extern mama_status
avisBridgeMamaPublisher_createByIndex (
                               publisherBridge*  result,
                               mamaTransport     tport,
                               int               tportIndex,
                               const char*       topic,
                               const char*       source,
                               const char*       root,
                               mamaPublisher     parent);

MAMAExpDLL
extern mama_status
avisBridgeMamaPublisher_destroy (publisherBridge publisher);

MAMAExpDLL
extern mama_status
avisBridgeMamaPublisher_send (publisherBridge publisher, mamaMsg msg);

MAMAExpDLL
extern mama_status
avisBridgeMamaPublisher_sendReplyToInbox (publisherBridge publisher,
                                         mamaMsg         request,
                                         mamaMsg         reply);

MAMAExpDLL
extern mama_status
avisBridgeMamaPublisher_sendFromInbox (publisherBridge publisher,
                                      mamaInbox       inbox,
                                      mamaMsg         msg);
MAMAExpDLL
extern mama_status
avisBridgeMamaPublisher_sendFromInboxByIndex (publisherBridge publisher,
                                             int           tportIndex,
                                             mamaInbox     inbox,
                                             mamaMsg       msg);
MAMAExpDLL
extern mama_status
avisBridgeMamaPublisher_sendReplyToInboxHandle (publisherBridge publisher,
                                               void *          wmwReply,
                                               mamaMsg         reply);

/*=========================================================================
  =                    Functions for the mamaInbox                        =
  =========================================================================*/
MAMAExpDLL
extern mama_status
avisBridgeMamaInbox_create (
            inboxBridge*           bridge,
            mamaTransport          tport,
            mamaQueue              queue,
            mamaInboxMsgCallback   msgCB,
            mamaInboxErrorCallback errorCB,
            mamaInboxDestroyCallback onInboxDestroyed,
            void*                  closure,
            mamaInbox              parent);

MAMAExpDLL
extern mama_status
avisBridgeMamaInbox_createByIndex (
            inboxBridge*           bridge,
            mamaTransport          tport,
            int                    tportIndex,
            mamaQueue              queue,
            mamaInboxMsgCallback   msgCB,
            mamaInboxErrorCallback errorCB,
            mamaInboxDestroyCallback onInboxDestroyed,
            void*                  closure,
            mamaInbox              parent);

MAMAExpDLL
extern mama_status
avisBridgeMamaInbox_destroy (inboxBridge inbox);

/*=========================================================================
  =                    Functions for the mamaMsg                           =
  =========================================================================*/
MAMAExpDLL
extern mama_status
avisBridgeMamaMsg_create (msgBridge* msg, mamaMsg parent);

MAMAExpDLL
extern int
avisBridgeMamaMsg_isFromInbox (msgBridge msg);

MAMAExpDLL
extern mama_status
avisBridgeMamaMsg_destroy (msgBridge msg, int destroyMsg);

MAMAExpDLL
extern mama_status
avisBridgeMamaMsg_destroyMiddlewareMsg (msgBridge msg);

MAMAExpDLL
extern mama_status
avisBridgeMamaMsg_detach (msgBridge msg);

MAMAExpDLL
extern mama_status
avisBridgeMamaMsg_getPlatformError (msgBridge msg, void** error);

MAMAExpDLL
extern mama_status
avisBridgeMamaMsg_setSendSubject (msgBridge   msg,
                                   const char* symbol,
                                   const char* subject);

MAMAExpDLL
extern mama_status
avisBridgeMamaMsg_getNativeHandle (msgBridge msg, void** result);

MAMAExpDLL
extern mama_status
avisBridgeMamaMsg_duplicateReplyHandle (msgBridge msg, void** result);

MAMAExpDLL
extern mama_status
avisBridgeMamaMsg_copyReplyHandle (void* src, void** dest);

MAMAExpDLL
extern mama_status
avisBridgeMamaMsg_destroyReplyHandle (void* result);

MAMAExpDLL
extern mama_status
avisBridgeMamaMsgImpl_setReplyHandle (msgBridge msg, void* result);

MAMAExpDLL
extern mama_status
avisBridgeMamaMsgImpl_setReplyHandleAndIncrement (msgBridge msg, void* result);

MAMAExpDLL
extern mama_status
avisBridgeMamaMsgImpl_setAttributesAndSecure (msgBridge msg, void* attributes, uint8_t secure);

#if defined(__cplusplus)
}
#endif

#endif /*AVIS_BRIDGE_FUNCTIONS__*/

