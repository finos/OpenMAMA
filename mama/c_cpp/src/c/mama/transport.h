/* $Id: transport.h,v 1.34.4.2.4.1.2.3 2011/08/10 14:53:28 nicholasmarriott Exp $
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

#ifndef MamaTransportH__
#define MamaTransportH__

#if defined(__cplusplus)
extern "C" {
#endif

#include "mama/types.h"
#include "mama/status.h"
#include "mama/subscription.h"
#include "mama/msg.h"
#include "mama/symbolmap.h"
#include "mama/quality.h"

/**
 * MAMA passes this enumeration as a parameter to the mamaTransportCB if a
 * client installs one. The values passed depend on the underlying middleware:
 *
 * All Middleware: MAMA_TRANSPORT_QUALITY is passed when an the underlying
 * transport detects an event that may affect data quality. Depending on the
 * event, MAMA may also mark subscriptions as STALE or MAYBE_STALE.
 *
 * TIBRV: TIBRV invokes the callback with MAMA_TRANSPORT_DISCONNECT when the
 * transport disconnects from the RV daemon and MAMA_TRANSPORT_RECONNECT when
 * it reestablishes a connection to the daemon.
 *
 * Wombat Middleware: For Wombat Middleware MAMA distinguishes between
 * connections established from a transport to another transport and
 * connections established by the transport from another transport. In general
 * MAMA applications establish connection from a MAMA transport to a feed
 * handler (publisher); however, they may also accept connections from other
 * MAMA clients. Applications subscribe to topics and publisher messages over
 * all connections. When a transport successfully connects to another transport
 * (i.e. a feed handler) the mamaTransportCallback receives
 * MAMA_TRANSPORT_CONNECT if the connection succeeds immediately otherwise it
 * receives a MAMA_TRANSPORT_RECONNECT if it subsequently succeeds. Wombat
 * Middleware transports may be configured to retry failed connections. When a
 * connection that a transport establishes to another transport (the publisher
 * by convention) and the connection is lost the mamaTranpsortCallback
 * receives the MAMA_TRANSPORT_PUBLISHER_DISCONNECT event. When a transport
 * accepts a connection from a remote transport (the subscriber by convention)
 * it receives either a MAMA_TRANSPORT_ACCEPT or
 * MAMA_TRANSPORT_ACCEPT_RECONNECT event indicating that the remote client is
 * connecting or reconnecting. When the client disconnects MAMA passes the
 * MAMA_TRANSPORT_DISCONNECT value.
 */
typedef enum
{
    MAMA_TRANSPORT_CONNECT,
    MAMA_TRANSPORT_CONNECT_FAILED,
    MAMA_TRANSPORT_RECONNECT,
    MAMA_TRANSPORT_DISCONNECT,
    MAMA_TRANSPORT_ACCEPT,
    MAMA_TRANSPORT_ACCEPT_RECONNECT,
    MAMA_TRANSPORT_PUBLISHER_DISCONNECT,
    MAMA_TRANSPORT_QUALITY,
    MAMA_TRANSPORT_NAMING_SERVICE_CONNECT,
    MAMA_TRANSPORT_NAMING_SERVICE_DISCONNECT,
    MAMA_TRANSPORT_WRITE_QUEUE_HIGH_WATER_MARK,
    MAMA_TRANSPORT_WRITE_QUEUE_LOW_WATER_MARK
} mamaTransportEvent;

/**
 * MAMA passes this enumeration as a parameter to the mamaTransportTopicCB if a
 * client installs one.
 * Wombat Middleware: For connection based transports an event is invoked when a
 * client makes a subscription to a topic on a transport or unsubscribes from a topic
 * on a transport.
 */
typedef enum
{
    MAMA_TRANSPORT_TOPIC_SUBSCRIBED,
    MAMA_TRANSPORT_TOPIC_UNSUBSRCIBED
} mamaTransportTopicEvent;


/**
 * Enum to represent the different load balancing schemes available.
 * TPORT_LB_SCHEME_STATIC - keep the same lb sub tport for each subscription.
 * TPORT_LB_SCHEME_ROUND_ROBIN - round robin between the members of the
 *                               lb group for each subscription
 * TPORT_LB_SCHEME_API - let the user defined load balancing callback(s)
 *                       decide.
 * TPORT_LB_SCHEME_LIBRARY - use the load balancing in a external library
 */
typedef enum
{
    TPORT_LB_SCHEME_STATIC,
    TPORT_LB_SCHEME_ROUND_ROBIN,
    TPORT_LB_SCHEME_API,
    TPORT_LB_SCHEME_LIBRARY
} tportLbScheme;

/**
 * Invoked when transport disconnects, reconnects, or has a data quality event.
 *
 * @param tport The transport associated with the transport event
 * @param mamaTransportEvent The transport event
 * @param cause The cause of the transport event
 * @param platformInfo Info associated with the transport event
 * @param closure The closure argument to pass to the callback whenever
 * it is invoked.
 *
 * The cause and platformInfo are supplied only by some middlewares.
 * The information provided by platformInfo is middleware specific.
 * The following middlewares are supported:
 *
 * tibrv: provides the char* version of the tibrv advisory message.
 * wmw:   provides a pointer to a mamaConnection struct for the event
 */

typedef void (MAMACALLTYPE *mamaTransportCB)(mamaTransport tport,
                                             mamaTransportEvent,
                                             short cause,
                                             const void* platformInfo,
                                             void *closure);

/**
 * Invoked when topic is subscribed ot unsubcribed on that transport.
 *
 * @param tport The transport associated with the transport topic event
 * @param mamaTransportTopicEvent The transport topic event
 * @param topic The topic being subscribed or unsubscribed to
 * @param platformInfo Info associated with the transport topicevent
 * @param closure The closure argument to pass to the callback whenever
 * it is invoked.
 *
 * The platformInfo is supplied only by some middlewares.
 * The information provided by platformInfo is middleware specific.
 * The following middlewares are supported:
 *
 * wmw:   provides a pointer to a mamaConnection struct for the event
 */


typedef void (MAMACALLTYPE *mamaTransportTopicCB)(mamaTransport tport,
                                                  mamaTransportTopicEvent event,
                                                  const char* topic,
                                                  const void* platformInfo,
                                                  void *closure);



typedef void (*mamaTransportLbInitialCB)(int  numTransports,
                                         int* transportIndex);

typedef void (*mamaTransportLbCB)(int         curTransportIndex,
                                  int         numTransports,
                                  const char* source,
                                  const char* symbol,
                                  int*        nextTransportIndex);


/**
 * Return a text description of the transport event.
 */
MAMAExpDLL
extern const char*
mamaTransportEvent_toString (mamaTransportEvent event);

/**
 * Set the transport callback. It receives advisories when a transport
 * disconnects or reconnects. Passing NULL removes the callback.
 */
MAMAExpDLL
extern mama_status
mamaTransport_setTransportCallback (mamaTransport   transport,
                                    mamaTransportCB callback,
                                    void*           closure);
/**
 * Set the transport write queue high and low water mark values. The
 * MAMA_TRANSPORT_WRITE_QUEUE_HIGH_WATER_MARK and
 * MAMA_TRANSPORT_WRITE_QUEUE_HIGH_WATER_MARK events will be delivered though
 * the transport callback when the respective number of items are outstanding on
 * a clients write queue. 
 */
MAMAExpDLL
extern mama_status
mamaTransport_setWriteQueueWatermarks (mamaTransport transport,
                                       uint32_t      highWater,
                                       uint32_t      lowWater);

/**
 * Set the transport topic callback. It receives advisories when a client
 * subscribes or unsubscribes to a topic on the transport
 */
MAMAExpDLL
extern mama_status
mamaTransport_setTransportTopicCallback (mamaTransport transport,
                                         mamaTransportTopicCB callback,
                                         void* closure);
/**
 * Allocate a transport structure. Do not free this memory, use
 * mamaTransport_destroy() instead.
 * The transport is not created until mamaTransport_create() is called.
 * Any transport properties should be set after calling allocate() and
 * prior to calling create()
 *
 * @param result The address to where the transport will be written.
 */
MAMAExpDLL
extern mama_status
mamaTransport_allocate (mamaTransport* result);

/**
 * Create a previously allocated transport.  Platform specific parameters
 * are read from the properties file <i>mama.properties</i>. The properties
 * file is located in the WOMBAT_PATH directory. The parameters are dependent
 * on the underlying messaging transport.
 * <p>
 * TIBRV: transports support the following: mama.tibrv.transport.name.service,
 * mama.tibrv.transport.name.network, and mama.tibrv.transport.name.daemon.
 * These correspond to the parameters for tibrvTransport_Create().
 * <p>
 * LBM: See the example mama.properties supplied with the release.
 *
 *
 * @param transport The previously allocated transport.
 * @param name The name of the transport in the mama.properties file.
 * @param bridgeImpl The middleware for which the transport is being created.
 */
MAMAExpDLL
extern mama_status
mamaTransport_create (mamaTransport transport,
                      const char*   name,
                      mamaBridge    bridgeImpl);

/**
 * Set the transport name. This can be used to set the name without
 * calling mamaTransport_create()
 *
 * @param transport The transport from which the name is being obtained.
 * @param name The name of the transport in the mama.properties file.
 */
MAMAExpDLL
extern mama_status
mamaTransport_setName (mamaTransport transport,
                       const char*   name);


/**
 * Get the transport name. This the name that was passed to the
 * mamaTransport_create() or mamaTransport_setName() function.
 *
 * @param transport The transport from which the name is being obtained.
 * @param result Pointer to the transport name string.
 */
MAMAExpDLL
extern mama_status
mamaTransport_getName (mamaTransport transport,
                       const char**  result);

MAMAExpDLL
const char *
mamaTransport_getMiddleware (mamaTransport transport);

/**
 * Enum to determine to which throttle a call applies. Currently, the
 * the default throttle, used by the publisher, and the initial value
 * request throttle are the same. Mama sends recap requests on a
 * separate throttle.
 */
typedef enum
{
    MAMA_THROTTLE_DEFAULT = 0,
    MAMA_THROTTLE_INITIAL = 1,
    MAMA_THROTTLE_RECAP   = 2
} mamaThrottleInstance;

/**
 * Get the outbound throttle rate. This the rate at which the transport sends
 * outbound messages to the feed handlers. It is also the rate at which new
 * subscriptions are created. Its purpose is to avoid flooding the network
 * with requests.
 *
 * @param transport the transport.
 * @param instance the instance
 * @param result Pointer to the resulting value in messages/second.
 */
MAMAExpDLL
extern mama_status
mamaTransport_getOutboundThrottle (mamaTransport transport,
                                   mamaThrottleInstance instance,
                                   double *result);

/**
 * Disable Refreshing.
 *
 * @param transport the transport instance
 * @param disable t/f.
 */
MAMAExpDLL
extern void
mamaTransport_disableRefresh (mamaTransport transport,
								uint8_t disable);

/**
 * Set the throttle rate.
 *
 * @param transport the transport.
 * @param instance the instance
 * @param outboundThrottle the rate in messages/second.
 */
MAMAExpDLL
extern mama_status
mamaTransport_setOutboundThrottle (mamaTransport transport,
                                   mamaThrottleInstance instance,
                                   double outboundThrottle);

/**
 * Set the symbol mapping function for a mamaTransport.
 *
 * @param transport The transport.
 * @param mapFunc The symbol mapping function.
 * @param closure The closure argument to pass to mappingFunc whenever
 * it is invoked.
 */
MAMAExpDLL
extern void
mamaTransport_setSymbolMapFunc (mamaTransport      transport,
                                mamaSymbolMapFunc  mapFunc,
                                void*              closure);

/**
 * Return the symbol mapping function for a mamaTransport.
 *
 * @param transport The transport.
 * @return The symbol mapping function (could be NULL).
 */
MAMAExpDLL
extern mamaSymbolMapFunc
mamaTransport_getSymbolMapFunc (mamaTransport  transport);

/**
 * Return the symbol mapping function closure for a mamaTransport.
 *
 * @param transport The transport.
 * @return The symbol mapping function closure (could be NULL).
 */
MAMAExpDLL
extern void*
mamaTransport_getSymbolMapFuncClosure (mamaTransport  transport);

/**
 * Set the description for this transport.
 * If the underlying middleware supports the concept of a description
 * the values will be passed on. Otherwise MAMA will maintain the description
 * internally.
 *
 * @param transport The transport.
 * @param description The description for the transport. This will be copied.
 */
MAMAExpDLL
extern mama_status
mamaTransport_setDescription (mamaTransport transport, const char* description);

/**
 * Get the description attribute for the specified transport.
 * Note that a copy of the description is not returned. Do not change or
 * free this memory.
 *
 * @param transport The transport
 * @param description The address to where the description will be written.
 */
MAMAExpDLL
extern mama_status
mamaTransport_getDescription (mamaTransport transport, const char** description);

 /**
  * Set whether to invoke the quality callback for all subscriptions
  * whenever a data quality event occurs (the default), or
  * whether to invoke the quality callback only when data subsequently arrives for a
  * subscription.
  *
  * If set to true, an onQuality callback will be invoked for a subscription
  * whenever a data quality event occurs on the transport, even in between
  * updates for that description.  If set to false, the onQuality callback
  * will not be called when the data quality event occurs on the transport.
  * However, it will still be invoked when an update arrives for the subscription.
  *
  * @param transport The transport
  * @param invokeQualityForAllSubscs Whether to invoke quality callback for
  *                                  all subscriptions
  */
MAMAExpDLL
extern mama_status
mamaTransport_setInvokeQualityForAllSubscs (
                                      mamaTransport transport,
                                      int           invokeQualityForAllSubscs);

/**
 * Get whether the transport has been set to invoke the quality callback for
 * all subscriptions whenever a data quality event occurs.
 *
 * @param transport The transport
 * @param invokeQualityForAllSubscs Whether transport has been set to invoke
 *                                  quality callback for all subscriptions
 */
MAMAExpDLL
extern mama_status
mamaTransport_getInvokeQualityForAllSubscs (
                                const mamaTransport transport,
                                int*                invokeQualityForAllSubscs);

/**
 * Get the quality of data for the transport.
 *
 * Currently only implemented for the Tibco RV middleware.
 * Other middlewares always return STATUS_OK
 *
 * @param transport The transport
 * @param quality The quality of data for the transport

 */
MAMAExpDLL
extern mama_status
mamaTransport_getQuality (const mamaTransport transport,
                          mamaQuality*        quality);

/**
 * Destroy the transport.
 *
 * @param transport the transport.
 */
MAMAExpDLL
extern mama_status
mamaTransport_destroy (mamaTransport transport);

/**
 * Find a connection with specified IP Address and Port. If the port is 0, the
 * call returns the first connection with the specified IP Address. If a
 * connection is not found the method returns MAMA_STATUS_NOT_FOUND and
 * *result == NULL. The caller is responsible for calling
 * mamaConnection_free().
 *
 * For middleware that does not provide access to per-connection information
 * (non WMW  middleware), the method returns MAMA_STATUS_NOT_IMPL.
 */
MAMAExpDLL
extern mama_status
mamaTransport_findConnection (mamaTransport   transport,
                              mamaConnection* result,
                              const char*     ipAddress,
                              uint16_t        port);

/**
 * Return a list of all clients connected to this transport. The result and
 * len must be passed to mamaTransport_freeAllConnections() to free resources
 * allocated by this function.
 */
MAMAExpDLL
extern mama_status
mamaTransport_getAllConnections (mamaTransport    transport,
                                 mamaConnection** result,
                                 uint32_t*        len);


/**
 * Return a list of all clients connected to this transport for a topic. The result and
 * len must be passed to mamaTransport_freeAllConnections() to free resources
 * allocated by this function.
 */
MAMAExpDLL
extern mama_status
mamaTransport_getAllConnectionsForTopic (mamaTransport    transport,
                                         const char*      topic,
                                         mamaConnection** result,
                                         uint32_t*        len);
/**
 * Free resources allocated by mamaTransport_getAllConnections().
 */
MAMAExpDLL
extern mama_status
mamaTransport_freeAllConnections (mamaTransport   transport,
                                  mamaConnection* result,
                                  uint32_t        len);

/**
 * Return a list of all servers connected to this transport. The result and
 * len must be passed to mamaTransport_freeAllServerConnections() to free
 * resources allocated by this function.
 */
MAMAExpDLL
extern mama_status
mamaTransport_getAllServerConnections (mamaTransport          transport,
                                       mamaServerConnection** result,
                                       uint32_t*              len);

/**
 * Free resources allocated by mamaTransport_getAllServerConnections().
 */
MAMAExpDLL
extern mama_status
mamaTransport_freeAllServerConnections (mamaTransport         transport,
                                        mamaServerConnection* result,
                                        uint32_t              len);

/**
 * Set the callback which will decide which member of the load balancing group
 * to use initially. If no callback is set then one will be chosen at random.
 */
MAMAExpDLL
extern mama_status
mamaTransport_setLbInitialCallback (mamaTransport            transport,
                                    mamaTransportLbInitialCB callback);

/**
 * Set the callback which will allocate the next member of the load balancing
 * group to use. The callback, if set, will be invoked for each new
 * subscription. If no callback is set then the currently selected member
 * the same one will be used for all subscriptions
 */
MAMAExpDLL
extern mama_status
mamaTransport_setLbCallback (mamaTransport     transport,
                             mamaTransportLbCB callback);

/**
 * Request that publishers conflate incoming messages. Publishers that support
 * conflation (currently only Wombat TCP middleware) will enable conflation
 * for this transport if possible.
 */
MAMAExpDLL
extern mama_status
mamaTransport_requestConflation (mamaTransport transport);

/**
 * Request that publishers stop conflating incoming messages for this
 * transport. Publishers that support conflation (currently only Wombat TCP
 * middleware) will stop conflating messages if possible. Note that the
 * publisher may continue to send conflated messages at its discretion.
 */
MAMAExpDLL
extern mama_status
mamaTransport_requestEndConflation (mamaTransport transport);

MAMAExpDLL
extern mama_status
mamaTransport_setDeactivateSubscriptionOnError (mamaTransport transport,
                                                    int deactivate);


MAMAExpDLL
extern int
mamaTransport_getDeactivateSubscriptionOnError (mamaTransport transport);

/**
 * Return a pointer tothe underlying native transport. Applications should
 * avoid this method if possible as it may result in non-portable, middleware
 * specific code. Callers must cast the nativeTport to the appropriate type
 * (ie. tibrvTransport*).
 *
 * Only implemented for TIBRV.
 *
 */
MAMAExpDLL
extern mama_status
mamaTransport_getNativeTransport (mamaTransport transport,
                                  int index,
                                  void** nativeTport);

/**
 * Return a pointer to the underlying native naming context the transport is using.
 * Applications should avoid this method if possible as it may result in non-portable,
 * middleware specific code.
 * Callers must cast the native naming context to the appropriate type
 * (ie. wmwns*).
 *
 * Only implemented for WMW version 5 and above.
 *
 */
MAMAExpDLL
extern mama_status
mamaTransport_getNativeTransportNamingCtx (mamaTransport transport,
                                           int index,
                                           void** nativeTportNamingCtx);

MAMAExpDLL
extern mamaStatsCollector*
mamaTransport_getStatsCollector (mamaTransport transport);

#if defined(__cplusplus)
}
#endif

#endif /* MamaTransportH__ */
