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
MAMAExpBridgeDLL
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
MAMAExpBridgeDLL
extern mama_status
qpidBridge_init (mamaBridge bridgeImpl);

/**
 * This function should return information about the current MAMA implementation
 * version, as well as that of any dependencies for which version information
 * is available.
 *
 * Requirement: Required
 *
 * @return const char* indicating the required version information.
 */
MAMAExpBridgeDLL
extern const char*
qpidBridge_getVersion (void);

/**
 * This function will return the name of this bridge implementation.
 *
 * Requirement: Required
 *
 * @return const char* representing the name of this bridge.
 */
MAMAExpBridgeDLL
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
MAMAExpBridgeDLL
mama_status
qpidBridge_getDefaultPayloadId (char*** name, char** id);


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
MAMAExpBridgeDLL
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
MAMAExpBridgeDLL
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
MAMAExpBridgeDLL
extern mama_status
qpidBridgeMamaTransport_create (transportBridge* result,
                                const char*      name,
                                mamaTransport    parent);


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
MAMAExpBridgeDLL
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
MAMAExpBridgeDLL
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
MAMAExpBridgeDLL
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
MAMAExpBridgeDLL
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
MAMAExpBridgeDLL
extern int
qpidBridgeMamaSubscription_isValid (subscriptionBridge bridge);

/**
 * This function is not required in the QPID Bridge
 *
 * Requirement:         Optional
 */
MAMAExpBridgeDLL
extern int
qpidBridgeMamaSubscription_hasWildcards (subscriptionBridge subscriber);

/**
 * This function is not required in the QPID Bridge
 *
 * Requirement:         Optional
 */
MAMAExpBridgeDLL
extern mama_status
qpidBridgeMamaSubscription_getPlatformError (subscriptionBridge subsc,
                                             void** error);

/**
 * This function is not required in the QPID Bridge
 *
 * Requirement:         Optional
 */
MAMAExpBridgeDLL
extern int
qpidBridgeMamaSubscription_isTportDisconnected (subscriptionBridge subsc);

/**
 * This function is not required in the QPID Bridge
 *
 * Requirement:         Optional
 */
MAMAExpBridgeDLL
extern mama_status
qpidBridgeMamaSubscription_setTopicClosure (subscriptionBridge subsc,
                                            void* closure);

/**
 * This function is an alias for qpidBridgeMamaSubscription_mute()
 *
 * Requirement:         Optional
 */
MAMAExpBridgeDLL
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
MAMAExpBridgeDLL
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
MAMAExpBridgeDLL
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
MAMAExpBridgeDLL
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
MAMAExpBridgeDLL
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
MAMAExpBridgeDLL
extern mama_status
qpidBridgeMamaTimer_getInterval (timerBridge timer, mama_f64_t* interval);


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
MAMAExpBridgeDLL
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
MAMAExpBridgeDLL
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
MAMAExpBridgeDLL
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
MAMAExpBridgeDLL
extern mama_status
qpidBridgeMamaPublisher_sendReplyToInbox (publisherBridge publisher,
                                          mamaMsg         request,
                                          mamaMsg         reply);

/**
 * This function is not required in the QPID Bridge. If implemented, it is
 * responsible for allowing the publisher to send further subsequent updates
 * to the provided inbox even after the initial reply.
 *
 * Requirement:         Optional
 */
MAMAExpBridgeDLL
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
MAMAExpBridgeDLL
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
MAMAExpBridgeDLL
extern mama_status
qpidBridgeMamaPublisher_sendFromInbox (publisherBridge publisher,
                                       mamaInbox       inbox,
                                       mamaMsg         msg);

/**
 * This method informs the middleware bridge implementation of the callbacks
 * to use when various situations are encountered as defined by
 * mamaPublisherCallbacks.
 *
 * In qpid, this won't wire error callbacks yet because trackers have not yet
 * been written for error conditions. That will tie in with github issue
 * https://github.com/OpenMAMA/OpenMAMA/issues/65.
 *
 * Requirement:      Optional
 *
 * @param publisher  This is the qpid publisher implementation to use
 * @param queue      The publisher queue
 * @param cb         The callbacks to trigger
 * @param closure    The closure to send back to any callbacks triggered
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
MAMAExpBridgeDLL
extern mama_status
qpidBridgeMamaPublisher_setUserCallbacks (publisherBridge         publisher,
                                          mamaQueue               queue,
                                          mamaPublisherCallbacks* cb,
                                          void*                   closure);


#if defined(__cplusplus)
}
#endif

#endif /*QPID_BRIDGE_FUNCTIONS__*/
