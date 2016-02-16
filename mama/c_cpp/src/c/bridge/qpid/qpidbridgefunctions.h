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

#ifndef QPID_BRIDGE_FUNCTIONS__
#define QPID_BRIDGE_FUNCTIONS__

#include <mama/mama.h>
#include <bridge.h>

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * This file contains definitions for all of the QPID bridge functions which
 * will be used by the MAMA code when delegating calls to the bridge.
 */

 /*=========================================================================
  =                    Functions for the bridge                           =
  =========================================================================*/

/**
 * Each MAMA bridge created is expected to define its own underlying
 * implementation which is middleware-specific. This object is then tracked
 * within MAMA as a mamaBridge object, and used to access middleware-level
 * implementations of MAMA functions. This function is responsible for creating
 * this object and creating each of the function pointers required to
 * satisfy a MAMA bridge implementation.
 *
 * Requirement: Required
 *
 * @param result The function will populate this mamaBridge* with a reference
 *               to the middleware bridge just created.
 */
MAMAExpDLL
extern void
qpidBridge_createImpl (mamaBridge* result);

/**
 * Each MAMA bridge created is expected to define its own underlying
 * implementation which is middleware-specific. This object is then tracked
 * within MAMA as a mamaBridge object, and used to access middleware-level
 * implementations of MAMA functions. This function is responsible for creating
 * this object and creating each of the function pointers required to
 * satisfy a MAMA bridge implementation.
 *
 * Requirement: Required
 *
 * @return A mama_status indicating the success or failure of the bridge
 *         initialisation.
 */
MAMAExpDLL
extern mama_status
qpidBridge_init (mamaBridge bridgeImpl);

/**
 * This function is responsible for initializing all underlying structures
 * required for the bridge implementation including the initiation of the
 * default event queue and possibly also middleware specific timers depending on
 * whether the implementation's timers are likely to be global across the bridge
 * or local to each transport. Note that the queue should not yet be
 * dispatching - that will only happen when qpidBridge_start is called.
 *
 * Requirement: Required
 *
 * @param bridgeImpl The MAMA bridge implementation created in _createImpl which
 *                   is to be opened.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL
extern mama_status
qpidBridge_open (mamaBridge bridgeImpl);

/**
 * This function is responsible for destroying all objects created within this
 * bridge implementation on closing. Note that this function may not necessarily
 * be called by the same thread on which qpidBridge_start is called.
 *
 * Requirement: Required
 *
 * @param bridgeImpl The MAMA bridge implementation created in _createImpl which
 *                   is to be destroyed.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL
extern mama_status
qpidBridge_close (mamaBridge bridgeImpl);

/**
 * This function is responsible for starting the middleware bridge and will be
 * called after all MAMA transports have been created. Depending on the nature
 * of the underlying middleware implementation, this could be responsible for a
 * variety of tasks including firing off dispatch threads or initializing
 * connections, but at a minimum, it is required to commence dispatching on the
 * default event queue.
 *
 * Requirement: Required
 *
 * @param mamaQueue The default event queue for this bridge implementation
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL
extern mama_status
qpidBridge_start (mamaQueue defaultEventQueue);

/**
 * This function is responsible for stopping the middleware bridge. Depending on
 * the nature of the underlying middleware implementation, this could be
 * responsible for a variety of tasks including joining dispatch threads or
 * destroying connections but at a minimum, it is required to stop dispatching
 * on the default event queue. Note it is quite legal to call _start again after
 * stopping a bridge so the bridge should consider this.
 *
 * Requirement: Required
 *
 * @param mamaQueue The default event queue for this bridge implementation
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL
extern mama_status
qpidBridge_stop (mamaQueue defaultEventQueue);

/**
 * This function should return information about the current MAMA implementation
 * version, as well as that of any dependencies for which version information
 * is available.
 *
 * Requirement: Required
 *
 * @return const char* indicating the required version information.
 */
MAMAExpDLL
extern const char*
qpidBridge_getVersion (void);

/**
 * This function will return the name of this bridge implementation.
 *
 * Requirement: Required
 *
 * @return const char* representing the name of this bridge.
 */
MAMAExpDLL
extern const char*
qpidBridge_getName (void);

/**
 * This function is responsible for letting MAMA know which payload
 * implementations are default for this middleware. The first element in
 * each array returned represents the default payload which will be used when
 * loading this middleware. If not already loaded, these payloads will then be
 * loaded.
 *
 * Requirement: Required
 *
 * @param name The array of strings to populate with payload names supported
 * @param name The array of chars to populate with payload IDs supported (as
 *             defined in mama/msg.h)
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL
mama_status
qpidBridge_getDefaultPayloadId (char*** name, char** id);


/*=========================================================================
  =                    Functions for the mamaQueue                        =
  =========================================================================*/

/**
 * This function is responsible for creating the Qpid implementation of its
 * queue and allocating all memory required for it.
 *
 * The qpid implementation acts as a wrapper for a wombatQueue.
 *
 * Requirement: Required
 *
 * @param queueBridge The queue bridge implementation structure to be created
 *                    will populate this pointer upon completion.
 * @param parent      The qpid implementation will be accessed by the MAMA
 *                    application developer through mamaQueue paradigms. This
 *                    variable is a back reference to this implementation,
 *                    should its functionality be required at the implementation
 *                    level.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaQueue_create (queueBridge *queue, mamaQueue parent);

/**
 * This function is responsible for creating the Qpid implementation of its
 * queue and allocating all memory required for it. Unlike _create though,
 * this function assumes that a wombatQueue has already been created before
 * calling this function, so the implementation merely builds the implementation
 * pointing to the provided nativeQueue rather than create its own.
 *
 * Requirement: Required
 *
 * @param queue       The queue bridge implementation structure to be created
 *                    will populate this pointer upon completion.
 * @param parent      The qpid implementation will be accessed by the MAMA
 *                    application developer through mamaQueue paradigms. This
 *                    variable is a back reference to this implementation,
 *                    should its functionality be required at the implementation
 *                    level.
 * @param nativeQueue Reference to the already created wombatQueue.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaQueue_create_usingNative (queueBridge *queue, mamaQueue parent,
                                        void* nativeQueue);

/**
 * This function is responsible for destroying the qpid queue as allocated in
 * the create functions and any underlying dependencies.
 *
 * In the qpid implementation, this also removes any created wombatQueues
 *
 * Requirement:        Required
 *
 * @param queue        The queue bridge implementation to be destroyed.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaQueue_destroy (queueBridge queue);

/**
 * This function is responsible for returning the number of events currently
 * on this queue.
 *
 * Requirement:        Required
 *
 * @param queue        The queue bridge implementation structure.
 * @param count        Pointer to populate with the current size of the queue.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaQueue_getEventCount (queueBridge queue, size_t* count);

/**
 * After calling this function, the queue will begin to dispatch events to
 * their provided callbacks until the parent MAMA Queue advises otherwise or
 * an error occurs. If no error occurs and the MAMA Queue is not instructed to
 * stop dispatching, this will block indefinitely.
 *
 * Requirement:        Required
 *
 * @param queue        The queue bridge implementation structure.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaQueue_dispatch (queueBridge queue);

/**
 * Unlike _dispatch, this function will attempt to run dispatch once on the
 * queue, and report the result of each dispatch attempt up to the calling
 * application for processing. This method observes a timeout and will return
 * MAMA_STATUS_TIMEOUT in the event that this time period has elapsed before
 * dispatch has been completed.
 *
 * Requirement:        Required
 *
 * @param queue        The queue bridge implementation structure.
 * @param timeout      The timeout to be observed while dispatching in
 *                     milliseconds. In the event of a timeout,
 *                     MAMA_STATUS_TIMEOUT will be returned.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaQueue_timedDispatch (queueBridge queue, uint64_t timeout);

/**
 * This function will attempt to run dispatch once on the queue, and report the
 * result of each dispatch attempt up to the calling application for processing.
 * No timeout is observed when calling this function.
 *
 * Requirement:        Required
 *
 * @param queue        The queue bridge implementation structure.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaQueue_dispatchEvent (queueBridge queue);

/**
 * This function will enqueue the provided event for processing. The callback
 * will be invoked once the event's turn in the queue is reached, and the
 * closure will be made available to this callback for processing at that time.
 *
 * Requirement:        Required
 *
 * @param queue        The queue bridge implementation structure.
 * @param callback     The user defined callback to invoke once the event is to
 *                     be executed.
 * @param closure      The closure to be made available to the user defined
 *                     callback once the event is to be executed.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaQueue_enqueueEvent (queueBridge        queue,
                                  mamaQueueEnqueueCB callback,
                                  void*              closure);

/**
 * This function will instruct the queue to stop dispatching (i.e. to unblock
 * qpidBridgeMamaQueue_dispatch() ).
 *
 * Requirement:        Required
 *
 * @param queue        The queue bridge implementation structure.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaQueue_stopDispatch (queueBridge queue);

/**
 * This function will set an enqueue callback as filtered through from the MAMA
 * application to allow the MAMA application to react to an event being added
 * to this queue.
 *
 * Requirement:        Optional
 *
 * @param queue        The queue bridge implementation structure.
 * @param callback     Function pointer to a function which the application has
 *                     been provided for execution when an event is added to
 *                     the queue.
 * @param closure      The closure to be passed to the callback function during
 *                     execution
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaQueue_setEnqueueCallback (queueBridge        queue,
                                        mamaQueueEnqueueCB callback,
                                        void*              closure);

/**
 * This function will remove the callback provided via
 * qpidBridgeMamaQueue_setEnqueueCallback
 *
 * Requirement:        Required
 *
 * @param queue        The queue bridge implementation structure.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaQueue_removeEnqueueCallback (queueBridge queue);

/**
 * This will return a native pointer to qpidQueueBridge which is opaque outside
 * queue.c. This can then be used in functions implemented in the native queue
 * which support this type as an argument.
 *
 * Requirement:        Required
 *
 * @param queue        The queue bridge implementation structure.
 * @param result       The qpidQueueBridge pointer which will be populated by
 *                     the function and returned
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaQueue_getNativeHandle (queueBridge queue,
                                     void**      nativeHandle);

/**
 * This will set a high watermark for the queue as used by MAMA and its
 * respective callbacks to detect slow consumers and consequent recovery.
 *
 *
 * Requirement:         Optional
 *
 * @param queue         The queue bridge implementation structure.
 * @param highWatermark The high watermark value to be set
 *
 * @return mama_status  indicating whether the method succeeded or failed.
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaQueue_setHighWatermark (queueBridge queue,
                                      size_t      highWatermark);

/**
 * This will set a low watermark for the queue as used by MAMA and its
 * respective callbacks to detect slow consumers and consequent recovery.
 *
 *
 * Requirement:        Optional
 *
 * @param queue        The queue bridge implementation structure.
 * @param lowWatermark The low watermark value to be set
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaQueue_setLowWatermark (queueBridge queue,
                                     size_t      lowWatermark);


/*=========================================================================
  =                    Functions for the mamaTransport                    =
  =========================================================================*/

/**
 * This function will simply return whether this transport has been successfully
 * created or not.
 *
 * Requirement:         Required
 *
 * @param transport     The queue bridge implementation structure.
 *
 * @return int equal to 1 if the transport is valid, otherwise it will return 0.
 */
MAMAExpDLL
extern int
qpidBridgeMamaTransport_isValid (transportBridge transport);

/**
 * This function is responsible for destroying the transport bridge entirely and
 * all dependencies it has created.
 *
 * Requirement:         Required
 *
 * @param transport     The queue bridge implementation structure.
 *
 * @return int equal to 1 if the transport is valid, otherwise it will return 0.
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaTransport_destroy (transportBridge transport);

/**
 * This function is responsible for creating the qpid transport bridge and all
 * underlying dependencies. Depending on the implementation, it may also
 * initialize some underlying dependencies which will later be fired when
 * qpidBridgeMamaTransport_destroy is called.
 *
 * Requirement:         Required
 *
 * @param result        The transport bridge created
 * @param name          The name of the transport to initialize (as defined in
 *                      mama.properties).
 * @param parent        The name of the parent MAMA Transport calling this
 *                      method
 *
 * @return mama_status  indicating whether the method succeeded or failed.
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaTransport_create (transportBridge* result,
                                const char*      name,
                                mamaTransport    parent);

/**
 * This function is not required in the QPID Bridge
 *
 * Requirement:         Optional
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaTransport_forceClientDisconnect (
                                transportBridge* transports,
                                int              numTransports,
                                const char*      ipAddress,
                                uint16_t         port);

/**
 * This function is not required in the QPID Bridge
 *
 * Requirement:         Optional
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaTransport_findConnection (transportBridge* transports,
                                        int              numTransports,
                                        mamaConnection*  result,
                                        const char*      ipAddress,
                                        uint16_t         port);

/**
 * This function is not required in the QPID Bridge
 *
 * Requirement:         Optional
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaTransport_getAllConnections (transportBridge* transports,
                                           int              numTransports,
                                           mamaConnection** result,
                                           uint32_t*        len);

/**
 * This function is not required in the QPID Bridge
 *
 * Requirement:         Optional
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaTransport_getAllConnectionsForTopic (transportBridge* transports,
                                                   int              numTransports,
                                                   const char*      topic,
                                                   mamaConnection** result,
                                                   uint32_t*        len);

/**
 * This function is not required in the QPID Bridge
 *
 * Requirement:         Optional
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaTransport_requestConflation (transportBridge* transports,
                                           int              numTransports);

/**
 * This function is not required in the QPID Bridge
 *
 * Requirement:         Optional
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaTransport_requestEndConflation (transportBridge* transports,
                                              int              numTransports);

/**
 * This function is not required in the QPID Bridge
 *
 * Requirement:         Optional
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaTransport_getAllServerConnections (
                                transportBridge*       transports,
                                int                    numTransports,
                                mamaServerConnection** result,
                                uint32_t*              len);

/**
 * This function is not required in the QPID Bridge
 *
 * Requirement:         Optional
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaTransport_freeAllServerConnections (
                                transportBridge*        transports,
                                int                     numTransports,
                                mamaServerConnection*   connections,
                                uint32_t                len);

/**
 * This function is not required in the QPID Bridge
 *
 * Requirement:         Optional
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaTransport_freeAllConnections (transportBridge* transports,
                                            int              numTransports,
                                            mamaConnection*  connections,
                                            uint32_t         len);

/**
 * This function is not required in the QPID Bridge
 *
 * Requirement:         Optional
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaTransport_getNumLoadBalanceAttributes (
                                const char* name,
                                int*        numLoadBalanceAttributes);

/**
 * This function is not required in the QPID Bridge
 *
 * Requirement:         Optional
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaTransport_getLoadBalanceSharedObjectName (
                                const char*  name,
                                const char** loadBalanceSharedObjectName);

/**
 * This function is not required in the QPID Bridge
 *
 * Requirement:         Optional
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaTransport_getLoadBalanceScheme (
                                const char*    name,
                                tportLbScheme* scheme);

/**
 * This function is not required in the QPID Bridge
 *
 * Requirement:         Optional
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaTransport_sendMsgToConnection (
                                transportBridge transport,
                                mamaConnection  connection,
                                mamaMsg         msg,
                                const char*     topic);

/**
 * This function is not required in the QPID Bridge
 *
 * Requirement:         Optional
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaTransport_isConnectionIntercepted (
                                mamaConnection connection,
                                uint8_t* result);

/**
 * This function is not required in the QPID Bridge
 *
 * Requirement:         Optional
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaTransport_installConnectConflateMgr (
                                transportBridge       transport,
                                mamaConflationManager mgr,
                                mamaConnection        connection,
                                conflateProcessCb     processCb,
                                conflateGetMsgCb      msgCb);

/**
 * This function is not required in the QPID Bridge
 *
 * Requirement:         Optional
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaTransport_uninstallConnectConflateMgr (
                                transportBridge       transport,
                                mamaConflationManager mgr,
                                mamaConnection        connection);

/**
 * This function is not required in the QPID Bridge
 *
 * Requirement:         Optional
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaTransport_startConnectionConflation (
                                transportBridge        transport,
                                mamaConflationManager  mgr,
                                mamaConnection         connection);

/**
 * This will return a native pointer to qpidTransportBridge which can then be
 * used in functions which expect a qpidTransportBridge* to be provided.
 *
 * Requirement:        Required
 *
 * @param transport    The transport bridge to get the native handle from
 * @param result       The qpidTransportBridge pointer which will be populated
 *                     by the function and returned
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaTransport_getNativeTransport (transportBridge transport,
                                            void**          result);

/**
 * This function is not required in the QPID Bridge
 *
 * Requirement:         Optional
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaTransport_getNativeTransportNamingCtx (transportBridge transport,
                                                     void**          result);


/*=========================================================================
  =                    Functions for the mamaSubscription                 =
  =========================================================================*/

/**
 * This will create a qpid subscription object, allocating all required memory
 * and dependencies for its operation.
 *
 * Requirement:        Required
 *
 * @param subscriber   This is a pointer which the function must populate to
 *                     provide a subscription instance up to MAMA for handling.
 * @param source       This is the name of the MAMA source under which this
 *                     subscription is to be made.
 * @param symbol       This is the name of the MAMA symbol under which this
 *                     subscription is to be made.
 * @param transport    This is a reference to the *MAMA* transport under which
 *                     this subscription is to be made.
 * @param queue        This is a reference to the *MAMA* event queue which will
 *                     be associated with this subscription.
 * @param callback     This is a reference to event callbacks which the MAMA
 *                     application developer has created and is now passing
 *                     through MAMA.
 * @param subscription This is a reference to the *MAMA* subscription which
 *                     this qpid subscription will be a member of.
 * @param closure      This is a reference closure provided to the subscription
 *                     during creation.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL
extern mama_status qpidBridgeMamaSubscription_create
                               (subscriptionBridge* subscriber,
                                const char*         source,
                                const char*         symbol,
                                mamaTransport       transport,
                                mamaQueue           queue,
                                mamaMsgCallbacks    callback,
                                mamaSubscription    subscription,
                                void*               closure );

/**
 * This function is not required in the QPID Bridge
 *
 * Requirement:         Optional
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaSubscription_createWildCard (
                                subscriptionBridge* subsc_,
                                const char*         source,
                                const char*         symbol,
                                mamaTransport       transport,
                                mamaQueue           queue,
                                mamaMsgCallbacks    callback,
                                mamaSubscription    subscription,
                                void*               closure );

/**
 * This will instruct this subscription to be "muted" which means that it will
 * no longer receive updates from any upstream dispatchers.
 *
 * Requirement:        Required
 *
 * @param subscriber   The subscription bridge to apply the mute to.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaSubscription_mute (subscriptionBridge subscriber);

/**
 * This will instruct this subscription to destroy itself including any
 * allocated memory and dependencies created during its life cycle.
 *
 * Requirement:        Required
 *
 * @param subscriber   The subscription bridge to destroy.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL
extern  mama_status
qpidBridgeMamaSubscription_destroy (subscriptionBridge subscriber);

/**
 * This function will advise the caller whether or not the provided subscription
 * bridge has been successfully created.
 *
 * Requirement:        Required
 *
 * @param subscriber   The subscription bridge to check
 *
 * @return int equal to 1 if valid, otherwise it will return 0.
 */
MAMAExpDLL
extern int
qpidBridgeMamaSubscription_isValid (subscriptionBridge bridge);

/**
 * This function is not required in the QPID Bridge
 *
 * Requirement:         Optional
 */
MAMAExpDLL
extern int
qpidBridgeMamaSubscription_hasWildcards (subscriptionBridge subscriber);

/**
 * This function is not required in the QPID Bridge
 *
 * Requirement:         Optional
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaSubscription_getPlatformError (subscriptionBridge subsc,
                                             void** error);

/**
 * This function is not required in the QPID Bridge
 *
 * Requirement:         Optional
 */
MAMAExpDLL
extern int
qpidBridgeMamaSubscription_isTportDisconnected (subscriptionBridge subsc);

/**
 * This function is not required in the QPID Bridge
 *
 * Requirement:         Optional
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaSubscription_setTopicClosure (subscriptionBridge subsc,
                                            void* closure);

/**
 * This function is an alias for qpidBridgeMamaSubscription_mute()
 *
 * Requirement:         Optional
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaSubscription_muteCurrentTopic (subscriptionBridge subsc);


/*=========================================================================
  =                    Functions for the mamaTimer                        =
  =========================================================================*/

/**
 * This will create a qpid timer object, allocating all required memory and
 * dependencies for its operation.
 *
 * Requirement:              Required
 *
 * @param timer             This is a pointer which the function must populate
 *                          to provide a timer instance up to MAMA for handling.
 * @param nativeQueueHandle This is the name of the *qpidQueue* which is to be
 *                          used with this timer.
 * @param action            This is a callback which is to be fired when each
 *                          timer event is to be fired.
 * @param onTimerDestroyed  This is a callback which is to be fired when this
 *                          timer is to be destroyed.
 * @param interval          The timer period for this timer in seconds.
 * @param parent            This is a reference to the parent MAMA timer.
 * @param closure           This is a reference closure provided to the timer
 *                          during creation.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaTimer_create (timerBridge* timer,
                            void*        nativeQueueHandle,
                            mamaTimerCb  action,
                            mamaTimerCb  onTimerDestroyed,
                            mama_f64_t   interval,
                            mamaTimer    parent,
                            void*        closure);

/**
 * This will destroy the provided qpid timer object, removing all memory
 * created and destroying any dependencies created during its life cycle.
 *
 * Requirement:      Required
 *
 * @param timer      This is a pointer which the qpid timer to be destroyed
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaTimer_destroy (timerBridge timer);

/**
 * This will reset the provided qpid timer object in the event that parameters
 * change (e.g. a new time interval is provided after creation). This will
 * usually involve destroying and recreating the timer.
 *
 * Requirement:      Required
 *
 *@param timer      This is a pointer which the qpid timer to be reset.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaTimer_reset (timerBridge timer);

/**
 * This will set a new timer interval for the provided timer which will recur
 * until stopped.
 *
 * Requirement:      Required
 *
 * @param timer      This is a pointer which the qpid timer to be adjusted.
 * @param interval   The new time interval in seconds for this timer.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaTimer_setInterval (timerBridge timer, mama_f64_t interval);

/**
 * This will return the existing timer interval for the provided timer.
 *
 * Requirement:      Required
 *
 * @param timer      This is a pointer which the qpid timer to be adjusted.
 * @param interval   Pointer to populate with the current time interval
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaTimer_getInterval (timerBridge timer, mama_f64_t* interval);


/*=========================================================================
  =                    Functions for the mamaIo                           =
  =========================================================================*/

/**
 * This function is not required in the QPID Bridge
 *
 * Requirement:         Optional
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaIo_create (ioBridge*       result,
                         void*           nativeQueueHandle,
                         uint32_t        descriptor,
                         mamaIoCb        action,
                         mamaIoType      ioType,
                         mamaIo          parent,
                         void*           closure);

/**
 * This function is not required in the QPID Bridge
 *
 * Requirement:         Optional
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaIo_getDescriptor (ioBridge io, uint32_t* result);

/**
 * This function is not required in the QPID Bridge
 *
 * Requirement:         Optional
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaIo_destroy (ioBridge io);


/*=========================================================================
  =                    Functions for the mamaPublisher                    =
  =========================================================================*/

/**
 * This will create a new publisher by index. The index is to be used for load
 * balancing purposes during publishing. This will create a new publisher
 * according to the provided transport, topic, source and queue.
 *
 * Requirement:             Required
 *
 * @param result            This is the qpid publisher pointer to populate upon
 *                          creation
 * @param tport             MAMA transport over which this is to be published
 * @param tportIndex        Transport index (0 for no load balancing)
 * @param topic             MAMA topic to publish onto
 * @param source            MAMA Source name to publish onto
 * @param root              Root name (e.g. _MD)
 * @param parent            Reference to the parent MAMA publisher
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaPublisher_createByIndex (
                               publisherBridge*  result,
                               mamaTransport     tport,
                               int               tportIndex,
                               const char*       topic,
                               const char*       source,
                               const char*       root,
                               mamaPublisher     parent);

/**
 * This will destroy the qpid publisher and all dependencies created during its
 * life cycle.
 *
 * Requirement:      Required
 *
 * @param publisher  This is the qpid publisher implementation to destroy
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaPublisher_destroy (publisherBridge publisher);

/**
 * This will send the provided MAMA Message over the provided qpid publisher.
 *
 * Requirement:      Required
 *
 * @param publisher  This is the qpid publisher implementation to use
 * @param msg        This is the MAMA message to publish
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaPublisher_send (publisherBridge publisher, mamaMsg msg);

/**
 * This method will be called when MAMA detects a message coming in which
 * originated from an inbox request, forwards it to the MAMA application, then
 * reaches this function after reply population. This function is responsible
 * for processing the request, parsing it accordingly and responding to the
 * inbox request.
 *
 * Requirement:      Required
 *
 * @param publisher  This is the qpid publisher implementation to use
 * @param request    This is the MAMA message which constitutes the request
 * @param reply      This is the MAMA message which constitutes the reply
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaPublisher_sendReplyToInbox (publisherBridge publisher,
                                          void*           request,
                                          mamaMsg         reply);

/**
 * This function is not required in the QPID Bridge. If implemented, it is
 * responsible for allowing the publisher to send further subsequent updates
 * to the provided inbox even after the initial reply.
 *
 * Requirement:         Optional
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaPublisher_sendReplyToInboxHandle (publisherBridge publisher,
                                                void*           wmwReply,
                                                mamaMsg         reply);

/**
 * This method will be called when MAMA has already created an inbox and now
 * wishes to send a request from it. This method is responsible for sending
 * the message over the middleware in such a way that the receiver can
 * appropriately respond.
 *
 * In qpid, this is done using the properties meta data provided adjacent to the
 * pn_message_t main body.
 *
 * Requirement:      Required
 *
 * @param publisher  This is the qpid publisher implementation to use
 * @param tportIndex This is the transport index used
 * @param inbox      This is the MAMA inbox to send from
 * @param msg        This is the MAMA message to send
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaPublisher_sendFromInboxByIndex (publisherBridge   publisher,
                                              int               tportIndex,
                                              mamaInbox         inbox,
                                              mamaMsg           msg);

/**
 * This method will be called when MAMA has already created an inbox and now
 * wishes to send a request from it. This method is responsible for sending
 * the message over the middleware in such a way that the receiver can
 * appropriately respond.
 *
 * In qpid, this is done using the properties meta data provided adjacent to the
 * pn_message_t main body.
 *
 * Requirement:      Required
 *
 * @param publisher  This is the qpid publisher implementation to use
 * @param inbox      This is the MAMA inbox to send from
 * @param msg        This is the MAMA message to send
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaPublisher_sendFromInbox (publisherBridge publisher,
                                       mamaInbox       inbox,
                                       mamaMsg         msg);


/*=========================================================================
  =                    Functions for the mamaInbox                        =
  =========================================================================*/

/**
 * This method will create a MAMA Inbox compliant qpid inbox implementation
 * including allocating the memory required, creating the subscription required
 * and registering the provided callbacks against these subscription.
 *
 * In qpid, this is implemented using topic resolution techniques, but other
 * implementations may prefer to use request-response methods.
 *
 * Requirement:             Required
 *
 * @param bridge            This is a pointer to be populated with the newly
 *                          created qpid inbox implementation.
 * @param tport             This is the MAMA transport to be used as a medium
 * @param queue             This is the MAMA queue to use
 * @param msgCB             This is the MAMA on message callback
 * @param errorCB           This is the MAMA on error callback
 * @param onInboxDestroyed  This is the MAMA destructor callback
 * @param closure           This is the closure to be referenced throughout this
 *                          inbox instance.
 * @param parent            This is the parent mamaInbox which the qpid inbox
 *                          implementation is to belong to
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaInbox_create (
            inboxBridge*                bridge,
            mamaTransport               tport,
            mamaQueue                   queue,
            mamaInboxMsgCallback        msgCB,
            mamaInboxErrorCallback      errorCB,
            mamaInboxDestroyCallback    onInboxDestroyed,
            void*                       closure,
            mamaInbox                   parent);

/**
 * This method will create a MAMA Inbox compliant qpid inbox implementation
 * including allocating the memory required, creating the subscription required
 * and registering the provided callbacks against these subscription.
 *
 * In qpid, this is implemented using topic resolution techniques, but other
 * implementations may prefer to use request-response methods.
 *
 * Requirement:             Required
 *
 * @param bridge            This is a pointer to be populated with the newly
 *                          created qpid inbox implementation.
 * @param tport             This is the MAMA transport to be used as a medium
 * @param tportIndex        This is the MAMA transport index to be used
 * @param queue             This is the MAMA queue to use
 * @param msgCB             This is the MAMA on message callback
 * @param errorCB           This is the MAMA on error callback
 * @param onInboxDestroyed  This is the MAMA destructor callback
 * @param closure           This is the closure to be referenced throughout this
 *                          inbox instance.
 * @param parent            This is the parent mamaInbox which the qpid inbox
 *                          implementation is to belong to
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaInbox_createByIndex (
            inboxBridge*                bridge,
            mamaTransport               tport,
            int                         tportIndex,
            mamaQueue                   queue,
            mamaInboxMsgCallback        msgCB,
            mamaInboxErrorCallback      errorCB,
            mamaInboxDestroyCallback    onInboxDestroyed,
            void*                       closure,
            mamaInbox                   parent);

/**
 * This will destroy the qpid inbox and all dependencies created during its
 * life cycle.
 *
 * Requirement:     Required
 *
 * @param inbox     This is the qpid inbox implementation to destroy
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaInbox_destroy (inboxBridge inbox);


/*=========================================================================
  =                    Functions for the msgBridge                        =
  =========================================================================*/

/**
 * This will create a qpid bridge message and all dependencies required for
 * its operation.
 *
 * The Qpid implementation currently doesn't depend on the qpid bridge message
 * structure for things like inbox detection or reply handle caching and instead
 * uses it as a reusable buffer for deserialization for now. This is likely to
 * change.
 *
 * Requirement:     Required
 *
 * @param msg       This is the a pointer to populate with the newly created
 *                  bridge message implementation
 * @param parent    This is a reference to the MAMA message which this bridge
 *                  message belongs to, should it be required at a later point
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaMsg_create (msgBridge* msg, mamaMsg parent);

/**
 * This will detect if the bridge message being received originates from an
 * inbox request or not.
 *
 * The Qpid implementation simply returns 1 at the moment as the bridge message
 * is not used for inbox message path traversal but this is likely to change.
 *
 * Requirement:     Required
 *
 * @param msg       This is the bridge message to be analysed
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL
extern int
qpidBridgeMamaMsg_isFromInbox (msgBridge msg);

/**
 * This will destroy the qpid inbox and all dependencies created during its
 * life cycle. There is an additional boolean destroyMsg provided. Providing 0
 * to this value will simply destroy the bridge implementation struct, whereas
 * providing 1 will also destroy all underlying buffers created or referenced.
 *
 * Requirement:      Required
 *
 * @param msg        This is the qpid bridge message to destroy
 * @param destroyMsg This is an additional boolean to determine whether or not
 *                   to destroy the underlying message buffers too as well as the
 *                   bridge implementation itself.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaMsg_destroy (msgBridge msg, int destroyMsg);

/**
 * This will destroy underlying message buffers associated with this bridge
 * message, but will not delete the bridge implementation itself.
 *
 * Requirement:     Required
 *
 * @param msg       This is the qpid bridge message to destroy middleware
 *                  buffers associated with.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaMsg_destroyMiddlewareMsg (msgBridge msg);

/**
 * This should detach the bridge message's ties to any underlying middleware
 * middleware level constructs so it can be reused on its own at a later time.
 *
 * In the qpid implementation, because the qpid bridge currently doesn't have
 * any implicit ties to the underlying middleware, this implementation currently
 * doesn't need to do anything.
 *
 * Requirement:     Required
 *
 * @param msg       This is the qpid bridge message to detach
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaMsg_detach (msgBridge msg);

/**
 * This should return any errors associated with the underlying platform
 * bridge message. Despite the void** prototype, a char** should be returned
 * here as the error is usually interpreted as a string when called from MAMA.
 *
 * In the qpid implementation, this currently does nothing as the bridge message
 * isn't responsible for doing any qpid message parsing which will result in
 * a qpid proton specific error message.
 *
 * Requirement:     Required
 *
 * @param msg       This is the qpid bridge message to get the error from.
 * @param error     This is the pointer to populate with an error message,
 *                  or set to NULL if not available or applicable.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaMsg_getPlatformError (msgBridge msg, void** error);

/**
 * This should set the send subject to be used for publication onto the
 * middleware so that the destination can correctly identify the message.
 *
 * In the qpid implementation, this currently populates the MAMA message
 * with the subscription symbol provided. When bridge message is used more
 * for proton middleware message encapsulation, this will also set the meta
 * data required in the proton message to identify the subject.
 *
 * Requirement:     Required
 *
 * @param msg       This is the qpid bridge message to set the send subject for.
 * @param symbol    This is the symbol to send to
 * @param error     This is the complete subject used to identify the message
 *                  within the middleware (usually a combination of root, source
 *                  and symbol).
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaMsg_setSendSubject (msgBridge   msg,
                                  const char* symbol,
                                  const char* subject);

/**
 * This will return a native handle to the native qpidMsgImpl data structure
 * which is opaque outside of msg.c, so all interpretation methods to work
 * with this should be added to this bridge implementation's msg.c.
 *
 * Requirement:     Required
 *
 * @param msg       This is the qpid bridge message to get the handle for.
 * @param result    This is the pointer to populate with the native bridge
 *                  handle.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaMsg_getNativeHandle (msgBridge msg, void** result);

/**
 * In the case of request / reply functionality, once the reply has been issued,
 * there is every chance that the bridge implementation will overwrite the
 * buffer previously used for the reply in the interest of keeping memory
 * footprint low. This function is responsible for duplicating this reply object
 * so it could (for example) be published and destroyed within a separate event
 * queue.
 *
 * As qpid doesn't currently use the bridge message for reply handles, this
 * method currently does nothing.
 *
 * Requirement:     Required
 *
 * @param msg       This is the qpid bridge message to get the handle for.
 * @param result    This is the pointer to populate with the reply handle
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaMsg_duplicateReplyHandle (msgBridge msg, void** result);

/**
 * In the case of request / reply functionality, once the reply has been issued,
 * there is every chance that the bridge implementation will overwrite the
 * buffer previously used for the reply in the interest of keeping memory
 * footprint low. This function is responsible for further duplication of this
 * reply object after retrieval via qpidBridgeMamaMsg_duplicateReplyHandle ()
 * so it could (for example) be used to populate a *stream* of events targeted
 * to the same inbox multiple times (e.g. application level heart beats)
 *
 * As qpid doesn't currently use the bridge message for reply handles, this
 * method currently does nothing.
 *
 * Requirement:     Required
 *
 * @param src       This is the original reply handle to copy
 * @param result    This is a pointer to be populated with the newly copied
 *                  reply handle.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaMsg_copyReplyHandle (void* src, void** dest);

/**
 * This function allows MAMA to send any MAMA message to a cached reply handle
 * by setting its reply handle prior to publication. The handle should have been
 * procured using qpidBridgeMamaMsg_duplicateReplyHandle or
 * qpidBridgeMamaMsg_copyReplyHandle and this function will ensure that the
 * provided bridge message now uses the provided reply handle.
 *
 * As qpid doesn't currently use the bridge message for reply handles, this
 * method currently does nothing.
 *
 * Requirement:     Required
 *
 * @param msg       This is the bridge message to update the reply handle for.
 * @param handle    This is the reply handle to use.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaMsgImpl_setReplyHandle (msgBridge msg, void* handle);

/**
 * This function allows MAMA to send any MAMA message to a cached reply handle
 * by setting its reply handle prior to publication. The handle should have been
 * procured using qpidBridgeMamaMsg_duplicateReplyHandle or
 * qpidBridgeMamaMsg_copyReplyHandle and this function will ensure that the
 * provided bridge message now uses the provided reply handle. In addition to
 * this, this function should also destroy the underlying reply handle where
 * applicable.
 *
 * As qpid doesn't currently use the bridge message for reply handles, this
 * method currently does nothing.
 *
 * Requirement:     Required
 *
 * @param msg       This is the bridge message to update the reply handle for.
 * @param handle    This is the reply handle to use.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaMsgImpl_setReplyHandleAndIncrement (msgBridge msg, void* handle);

/**
 * If the MAMA application developer has used duplicateReplyHandle, they have
 * assumed responsibility for the memory allocated during this process. This
 * function is responsible for destroying the reply handle as it will always be
 * implementation specific and opaque to the MAMA application developer.
 *
 * As qpid doesn't currently use the bridge message for reply handles, this
 * method currently does nothing.
 *
 * Requirement:     Required
 *
 * @param handle    This is the reply handle to destroy
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpDLL
extern mama_status
qpidBridgeMamaMsg_destroyReplyHandle (void* handle);

#if defined(__cplusplus)
}
#endif

#endif /*QPID_BRIDGE_FUNCTIONS__*/
