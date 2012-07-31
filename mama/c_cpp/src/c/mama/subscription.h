/* $Id: subscription.h,v 1.47.4.3.2.4.4.5 2011/10/02 19:02:18 ianbell Exp $
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

#ifndef MamaSubscriptionH__
#define MamaSubscriptionH__

/* *************************************************** */
/* Includes. */
/* *************************************************** */
#include "mama/marketdata.h"
#include "mama/msg.h"
#include "mama/quality.h"
#include "mama/servicelevel.h"
#include "mama/status.h"
#include "mama/subscriptiontype.h"
#include "mama/transport.h"
#include "mama/types.h"

#if defined (__cplusplus)
extern "C" {
#endif

/* *************************************************** */
/* Enumerations. */
/* *************************************************** */

/**
* MAMA can cache messages which arrive prior to an initial value. There are
* two options in how and when these message are used.
*
* PRE_INITIAL_SCHEME_ON_GAP (default) MAMA waits until the message after the
* initialvalue and only tries to use the cache if a gap is detected.
* This reduces the need to reap the subscription.
*
* PRE_INITIAL_SCHEME_ON_INITIAL MAMA checks the pre-initial cache to see if
* there are any messages which sequentially follow the initial value and pass
* them to the application.
*/
typedef enum
{
    PRE_INITIAL_SCHEME_ON_GAP,
    PRE_INITIAL_SCHEME_ON_INITIAL

} preInitialScheme;

/**
 * The enumeration describes the state of the subsubscription at any time.
 * The state can be obtained by calling the mamaSubscription_getState function.
 * A string representation of this state can be obtained by calling the
 * mamaSubscription_stringForState function.
 */
typedef enum
{
    /* The state of the subscription is unknown. */
    MAMA_SUBSCRIPTION_UNKNOWN       = 0,

    /* The subscription has been allocated in memory. */
    MAMA_SUBSCRIPTION_ALLOCATED     = 1,

    /* Inital setup work has been done, mamaSubscription_activate must still be called. Note that this state is only valid
     * for market data subscriptions.
     */
    MAMA_SUBSCRIPTION_SETUP         = 2,

    /* The subscription is now on the throttle queue waiting to be fully activated. */
    MAMA_SUBSCRIPTION_ACTIVATING    = 3,

    /* The subscription is now fully activated and is processing messages. */
    MAMA_SUBSCRIPTION_ACTIVATED     = 4,

    /* The subscription is being de-activated, it will not be fully deactivated until the onDestroy callback is received. */
    MAMA_SUBSCRIPTION_DEACTIVATING  = 5,

    /* The subscription has been de-activated, messages are no longer being processed. */
    MAMA_SUBSCRIPTION_DEACTIVATED   = 6,

    /* The subscription is being destroyed, it will not be fully destroyed until the onDestroy callback is received. */
    MAMA_SUBSCRIPTION_DESTROYING    = 7,

    /* The subscription has been fully destroyed. */
    MAMA_SUBSCRIPTION_DESTROYED     = 8,

    /* The subscription is in the process of being de-allocated, this state is only valid if the mamaSubscription_deallocate
     * function is called while the subscription is being destroyed.
     */
    MAMA_SUBSCRIPTION_DEALLOCATING  = 9,

    /* The subscription has been de-allocated, this state is only supported so that the log entry will whenever the subscription
     * has finally been freed.
     */
    MAMA_SUBSCRIPTION_DEALLOCATED   = 10

} mamaSubscriptionState;

typedef enum
{
    DQ_SCHEME_DELIVER_ALL,
    DQ_SCHEME_INGORE_DUPS
} dqStartegyScheme;


typedef enum
{
    DQ_FT_DO_NOT_WAIT_FOR_RECAP,
    DQ_FT_WAIT_FOR_RECAP
}dqftStrategyScheme;
/* *************************************************** */
/* Type Defines. */
/* *************************************************** */

/**
 * Function invoked when subscription creation is complete, and before any calls
 * to <code>wombat_subscriptionOnMsgCB</code>.
 *
 * @param subscription The subscription.
 * @param closure The closure passed to the mamaSubscription_create function.
 */
typedef void (MAMACALLTYPE *wombat_subscriptionCreateCB)(
    mamaSubscription subscription,
    void *closure);

/**
 * Function invoked when a subscription has been completely destroyed or deactivated,
 * the client can have confidence that no further messages will be placed on the queue
 * for this subscription.
 *
 * @param subscription the <code>MamaSubscription</code>.
 * @param closure The closure passed to the mamaSubscription_create function.
 */
typedef void (MAMACALLTYPE *wombat_subscriptionDestroyCB)(
    mamaSubscription subscription,
    void *closure);

/**
 * Invoked if an error occurs during prior to subscription creation or if the
 * subscription receives a message for an unentitled subject.
 * <p>
 * If the status is <code>MAMA_MSG_STATUS_NOT_ENTITTLED</code> the subject
 * parameter is the specific unentitled subject. If the subscription subject
 * contains wildcards, the subscription may still receive messages for other
 * entitled subjects. Note wildcard subscriptions are not supported on  all
 * platforms.
 *
 * @param subscription The subscription.
 * @param status The error code.
 * @param platformError Third party, platform specific messaging error.
 * @param subject The subject for NOT_ENTITLED
 * @param closure The closure passed to the mamaSubscription_create function.
 */
typedef void (MAMACALLTYPE *wombat_subscriptionErrorCB)(
    mamaSubscription subscription,
    mama_status status,
    void *platformError,
    const char *subject,
    void *closure);

/**
 * Function invoked when a sequence number gap is detected. At this
 * point the topic is considered stale and the subscription will not
 * receive further messages until the feed handler satisfies a
 * recap request.
 *
 * @param subscription the <code>MamaSubscription</code>.
 * @param closure The closure passed to the mamaSubscription_create function.
 */
typedef void (MAMACALLTYPE *wombat_subscriptionGapCB)(
    mamaSubscription subscription,
    void *closure);

/**
 * Invoked when a message arrives.
 *
 * @param subscription the <code>MamaSubscription</code>.
 * @param msg The mamaMsg.
 * @param closure The closure passed to the mamaSubscription_create function.
 * @param itemClosure The item closure for the subscription can be set with
 *                    mamaSubscription_setItemClosure.
 */
typedef void (MAMACALLTYPE *wombat_subscriptionOnMsgCB)(
    mamaSubscription subscription,
    mamaMsg msg,
    void *closure,
    void *itemClosure);

/**
 * Invoked to indicate a data quality event.
 *
 * @param subscription the <code>MamaSubscription</code>.
 * @param quality The new quality.
 * @param symbol The symbol.
 * @param cause The cause of the data quality event.
 * @param platformInfo Info associated with the data quality event.
 * @param closure The closure passed to the mamaSubscription_create function.
 *
 * The cause and platformInfo are supplied only by some middlewares.
 * The information provided by platformInfo is middleware specific.
 * The following middlewares are supported:
 *
 * tibrv: provides the char* version of the tibrv advisory message.
 */
typedef void (MAMACALLTYPE *wombat_subscriptionQualityCB)(
    mamaSubscription subscription,
    mamaQuality quality,
    const char *symbol,
    short cause,
    const void *platformInfo,
    void *closure);

/**
 * Function invoked when a recap is requested upon detecting a
 * sequence number gap.
 *
 * @param subscription the <code>MamaSubscription</code>.
 * @param closure The closure passed to the mamaSubscription_create function.
 */
typedef void (MAMACALLTYPE *wombat_subscriptionRecapCB)(
    mamaSubscription subscription,
    void *closure);

/**
 * Invoked when a message arrives for a wild card subscrption.
 *
 * @param subscription the <code>MamaSubscription</code>.
 * @param msg The mamaMsg.
 * @param topic The symbol.
 * @param closure The closure passed to the mamaSubscription_create function.
 * @param itemClosure Symbol specific closure.
 */
typedef void (MAMACALLTYPE *wombat_subscriptionWildCardOnMsgCB)(
    mamaSubscription subscription,
    mamaMsg msg,
    const char *topic,
    void *closure,
    void *itemClosure);

/* *************************************************** */
/* Structures. */
/* *************************************************** */

/**
 * A convenience structure for passing the callbacks to the subscription
 * factory methods.
 */
typedef struct mamaMsgCallbacks_
{
    wombat_subscriptionCreateCB   onCreate;
    wombat_subscriptionErrorCB    onError;
    wombat_subscriptionOnMsgCB    onMsg;
    wombat_subscriptionQualityCB  onQuality;
    wombat_subscriptionGapCB      onGap;
    wombat_subscriptionRecapCB    onRecapRequest;
	wombat_subscriptionDestroyCB  onDestroy;
} mamaMsgCallbacks;

/**
 * A convenience structure for passing the callbacks to wild card subscription
 * factory methods.
 */
typedef struct mamaWildCardMsgCallbacks_
{
    wombat_subscriptionCreateCB        onCreate;
    wombat_subscriptionErrorCB         onError;
    wombat_subscriptionWildCardOnMsgCB onMsg;
    wombat_subscriptionDestroyCB       onDestroy;
} mamaWildCardMsgCallbacks;

/* *************************************************** */
/* Public Function Prototypes. */
/* *************************************************** */

/**
 * Activate a subscription that has been set up by
 * mamaSubscription_setup().
 * Subscription creation actually occurs on the throttle queue.
 * An individual subscription cannot be assumed to be fully created until
 * its onCreate() callback has been successfully invoked.
 * The subscription rate can be governed via the
 * <code>mamaTransport_setOutboundThrottle ()</code> function.
 * Any subscription properties should be set prior to calling this function.
 *
 * @param[in] subscription The subscription.
 *
 * @return Mama status code can be one of:
 *      MAMA_STATUS_SUBSCRIPTION_INVALID_TYPE - the type of subscription does not support activation.
 *                                              This will be returned if activate is called for a
 *                                              basic subscription.
 *      MAMA_STATUS_SUBSCRIPTION_INVALID_STATE - the subscription cannot be activated as it is in the
 *                                              wrong state.
 *      MAMA_STATUS_OK.
 */
MAMAExpDLL
extern mama_status
mamaSubscription_activate(
    mamaSubscription subscription);

/**
 * Allocate memory for a new subscription. The subscription is not
 * actually created until a call to one of the create() functions is made.
 * Memory must be freed using the <code>mamaSubscription_deallocate()</code>
 * function.
 *
 * @param[out] result Where the address of the new subscription will be
 * written.
 *
 * @return mama_status value can be one of
 *          MAMA_STATUS_NOMEM
 *          MAMA_STATUS_NULL_ARG
 *          MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_allocate(
    mamaSubscription *result);

/**
 * Return whether the debug level for this subscription equals or
 * exceeds some level.
 *
 * @param[in] subscription The subscription.
 * @param[in] level The debug level to check.
 *
 * @return whether the level equals or exceeds the set level for this
 * subscription.
 */
MAMAExpDLL
extern int
mamaSubscription_checkDebugLevel(
    mamaSubscription subscription,
    MamaLogLevel level);

/**
 * Create and activate subscription.  This is effectively a pair of
 * calls to mamaSubscription_setup() and mamaSubscription_activate().
 *
 * @param[in] subscription The subscription.
 * @param[in] queue The mama queue.
 * @param[in] callbacks The mamaMsgCallbacks structure containing the callback functions.
 * @param[in] source The mamaSource identifying the publisher for this symbol.
 * @param[in] symbol The symbol name.
 * @param[in] closure The closure will be passed to subsequent callback invocations for this subscription.
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_INVALID_ARG
 *              MAMA_STATUS_INVALID_QUEUE
 *              MAMA_STATUS_SUBSCRIPTION_INVALID_STATE
 *              MAMA_STATUS_NO_BRIDGE_IMPL
 *              MAMA_STATUS_NOMEM
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_create (
    mamaSubscription          subscription,
    mamaQueue                 queue,
    const mamaMsgCallbacks*   callbacks,
    mamaSource                source,
    const char*               symbol,
    void*                     closure);

/**
 * Create a basic subscription without marketdata semantics.
 *
 * @param[in] subscription The subscription.
 * @param[in] transport The transport to use. Must be a basic transport.
 * @param[in] queue The mama queue.
 * @param[in] callbacks The mamaMsgCallbacks structure containing the callback
 * functions.
 * @param[in] symbol The symbol on which message are being published.
 * @param[in] closure The closure will be passed to subsequent callback
 * invocations for this subscription.
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_INVALID_ARG
 *              MAMA_STATUS_INVALID_QUEUE
 *              MAMA_STATUS_SUBSCRIPTION_INVALID_STATE
 *              MAMA_STATUS_NO_BRIDGE_IMPL
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_createBasic (
    mamaSubscription          subscription,
    mamaTransport             transport,
    mamaQueue                 queue,
    const mamaMsgCallbacks*   callbacks,
    const char*               symbol,
    void*                     closure);

/**
 * Crate and activate  a wildcard  subscription that may be actually
 * activated later.  Activate the subscription using
 * mamaSubscription_activate().
 *
 * The topic must be a valid wildcard topic for the underlying middleware.
 *
 * Applications may set and retireve per-topic closures using
 * mamaSubscription_setItemClosure() however this method is only guaranteed to
 * set the correct closure if called while in the msg callback. It will be the
 * topicClosure argument for subsequent callback invocations for the current
 * topic. The topicClosure argument to the callback is NULL prior to an
 * application setting the closure for the topic.
 *
 * NOTE: For WMW a source with a NULL symbol parameter creates a "transport"
 * subscription that receives all messages on the transport and bypasses the
 * naming service. A publishing transport can be assigned a name with the
 * publish_name property.
 *
 * @param[in] subscription The subscription.
 * @param[in] transport The transport to use.
 * @param[in] queue The mama queue.
 * @param[in] callbacks The mamaMsgCallbacks structure containing the callback
 * functions.
 * @param[in] source The source name of the feed handler to provide the
 * subscription.
 * @param[in] symbol The symbol name.
 * @param[in] closure The closure will be passed to subsequent callback
 * invocations for this subscription.
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_INVALID_ARG
 *              MAMA_STATUS_INVALID_QUEUE
 *              MAMA_STATUS_SUBSCRIPTION_INVALID_STATE
 *              MAMA_STATUS_NO_BRIDGE_IMPL
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_createBasicWildCard(
    mamaSubscription                 subscription,
    mamaTransport                    transport,
    mamaQueue                        queue,
    const mamaWildCardMsgCallbacks*  callbacks,
    const char*                      source,
    const char*                      symbol,
    void*                            closure);

/**
 * Create a snapshot subscription for initial value only (no updates).
 * This function is equivalent to mamaSubscription_create () with
 * svcLevel set to MAMA_SERVICE_LEVEL_SNAPSHOT and default arguments
 * for type, svcLevelOpt, requiresInitial, retries, timeout.
 *
 * @param[in] subscription The subscription.
 * @param[in] queue The mama queue.
 * @param[in] callbacks The mamaMsgCallbacks structure containing the callback functions.
 * @param[in] source The mamaSource identifying the publisher of data for the specified symbol.
 * @param[in] symbol The symbol name.
 * @param[in] closure The closure will be passed to subsequent callback invocations for this subscription.
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_INVALID_ARG
 *              MAMA_STATUS_INVALID_QUEUE
 *              MAMA_STATUS_SUBSCRIPTION_INVALID_STATE
 *              MAMA_STATUS_NO_BRIDGE_IMPL
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_createSnapshot(
    mamaSubscription          subscription,
    mamaQueue                 queue,
    const mamaMsgCallbacks*   callbacks,
    mamaSource                source,
    const char*               symbol,
    void*                     closure);

/**
 * Deactivate a subscription.  The subscription can be reactivated if
 * desired using mamaSubscription_activate().
 * Note that the subscription will not be fully deactivated until the
 * onDestroy callback is received.
 *
 * @param[in] subscription The subscription.
 *
 * @return mama status code can be one of:
 *      MAMA_STATUS_SUBSCRIPTION_INVALID_TYPE - the type of subscription does not support deactivation.
 *                                              This will be returned if deactivate is called for a
 *                                              basic subscription.
 *      MAMA_STATUS_SUBSCRIPTION_INVALID_STATE - the subscription cannot be deactivated as it is in the
 *                                              wrong state.
 *      MAMA_STATUS_OK.
 */
MAMAExpDLL
extern mama_status
mamaSubscription_deactivate(
    mamaSubscription subscription);

/**
 * Free the memory for a mamaSubscription which was allocated via
 * a call to <code>mamaSubscription_allocate()</code>
 * This function will call <code>mamaSubscription_destroy()</code> if the subscription has
 * not already been destroyed.
 *
 * @param[in] subscription The subscription to be deallocated.
 *
 * @return mama status code can be one of:
 *              MAMA_STATUS_SUBSCRIPTION_INVALID_STATE
 *              MAMA_STATS_NULL_ARG
 *              MAMA_STATUS_OK.
 */
MAMAExpDLL
extern mama_status
mamaSubscription_deallocate(
    mamaSubscription subscription);

/**
 * Destroy the subscription. <p>
 * This function must be called to destroy resources associated with the
 * subscription. It sends an unsubscribe to the feed handler for subscription
 * management.
 * This function does not free the memory associated with the subscription.
 * mamaSubscription_create() can be called again after this function has
 * been called.
 * This function must be called from the same thread dispatching on the
 * associated event queue unless both the default queue and dispatch queue are
 * not actively dispatching.
 *
 * @param[in] subscription The subscription.
 *
 * @return mama_status value can be one of
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_SUBSCRIPTION_INVALID_STATE
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_destroy(
    mamaSubscription subscription);

/**
 * This function will destroy the subscription and can be called from
 * any thread.
 * Note that the subscription will not be fully destroyed until the
 * onDestroy callback is received.
 * To destroy from the dispatching thread the mamaSubscription_destroy
 * function should be used in preference.
 *
 * @param[in] subscription The subscription.
 *
 * @return mama_status value can be one of
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_destroyEx(
    mamaSubscription subscription);

/**
 * Retrieve the application-specific data type.
 *
 * @param[in] subscription The subscription.
 * @param[out] appDataType A pointer to a uint8_t to hold the value.
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_getAppDataType (
    mamaSubscription subscription,
    uint8_t *appDataType);

/**
 * Return the closure.
 *
 * @param[in] subscription The subscription.
 * @param[out] closure holds the result.
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_getClosure(
    mamaSubscription subscription,
    void **closure);

/**
 * Return the debug level for this subscription.
 *
 * @param[in] subscription The subscription.
 *
 * @return the debug level for this subscription.
 */
MAMAExpDLL
extern MamaLogLevel
mamaSubscription_getDebugLevel(
    mamaSubscription subscription);


/**
 * Return the item closure for the current message for the subscription.
 * See <code>setItemClosure</code> for more detail.
 *
 * @param[in] subscription The subscription.
 * @param[out] closure The address to where the closure should be written.
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_getItemClosure (
    mamaSubscription subscription,
    void **closure);

/**
 * Get the filters that discard message according to the message
 * qualifier.
 *
 * @param[in] subscription The subscription from which the filer is being obtained.
 *
 * @param[out] ignoreDefinitelyDuplicate If true callbacks will not be invoked
 * for messages where MamaMsg::getIsDefinitelyDuplicate returns true.
 *
 * @param[out] ignorePossiblyDuplicate If true callbacks will not be invoked
 * for messages where MamaMsg::getIsPossiblyDuplicate returns true.
 *
 * @param[out] ignoreDefinitelyDelayed If true callbacks will not be invoked
 * for messages where MamaMsg::getIsDefinitelyDelayed returns true.
 *
 * @param[out] ignorePossiblyDelayed If true callbacks will not be invoked
 * for messages where MamaMsg::getIsPossiblyDelayed returns true.
 *
 * @param[out] ignoreOutOfSequence If true callbacks will not be invoked for
 * messages where MamaMsg::getIsOutOfSequence returns true.
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
mama_status
mamaSubscription_getMsgQualifierFilter(
    mamaSubscription subscription,
    int *ignoreDefinitelyDuplicate,
    int *ignorePossiblyDuplicate,
    int *ignoreDefinitelyDelayed,
    int *ignorePossiblyDelayed,
    int *ignoreOutOfSequence);

/**
 * Return the middleware-specific platform error. When a mamaSubscription_
 * method return MAMA_STATUS_PLATFORM the error will be the result from the
 * underlying platform.
 *
 * @param[in] subsc The subscription.
 * @param[out] error a pointer to a void pointer to hold the result.
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_NO_BRIDGE_IMPL
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_getPlatformError(
    mamaSubscription subsc,
    void **error);

/**
 * Return the initial value cache size.
 *
 * @param[in] subscription The subscription.
 * @param[out] result A pointer to an int for the result.
 *
 * @return mama_status value can be one of
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_getPreIntitialCacheSize(
    mamaSubscription subscription,
    int *result);

/**
 * Return the <code>mamaQueue</code> for this subscription.
 *
 * @param[in] subscription The subscription.
 * @param[out] queue  A pointer to hold the queue.
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_getQueue(
    mamaSubscription subscription,
    mamaQueue *queue);

/**
 * Returns a value of 1 or 0 indicating whether the subscription has
 * received an initial.
 *
 * @param[in] subscription The subscription
 * @param[out] receivedInitial Will be 1 if an initial has been recieved else 0.
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_getReceivedInitial(
    mamaSubscription subscription,
    int *receivedInitial);

/**
 * Whether the specified subscription will attempt to recover from sequence
 * number gaps.
 *
 * @param subscription[in] The subscription
 * @param doesRecover[out] 0 - does not recover, 1 - does attempt to recover
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_getRecoverGaps (
    mamaSubscription subscription,
    int *doesRecover);

/**
 * Returns a value of 1 or 0 indicating whether this subscription is
 * interested in initial values.
 *
 * @param[in] subscription The subscription
 * @param[out] requiresInitial 1 if an initial is required else 0.
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_getRequiresInitial(
    mamaSubscription subscription,
    int *requiresInitial);

/**
 * Retrieve the retries.
 *
 * @param[in] subscription The subscription.
 * @param[out] retries A pointer to a int to hold the value.
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_getRetries(
    mamaSubscription subscription,
    int *retries);

/**
 * Get the service level for the specified subscription.
 *
 * @param[in] subscription The subscription for which the service level is being
 * obtained.
 * @param[out] serviceLevel Address to where the service level will be written
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_getServiceLevel(
    mamaSubscription subscription,
    mamaServiceLevel *serviceLevel);

/**
 * Get the service level options for the specified subscription.
 *
 * @param[in] subscription The subscription for which the service level is being
 * obtained.
 * @param[out] serviceLevel Address to where the service level options will be written
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_getServiceLevelOpt(
    mamaSubscription subscription,
    long *serviceLevel);


/**
 * Return the source for the specified subscription.
 * Note that this function just returns a pointer to the source inside the
 * subscription and does not allocate any memory.
 *
 * @param[in] subscription The subscription.
 * @param[out] source holds the result, (do not free).
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_getSource(
    mamaSubscription  subscription,
    const char**      source);

/**
 * This function will return the current state of the subscription, this function
 * should be used in preference to the mamaSubscription_isActive or mamaSubscription_isValid
 *                          functions.
 * This function is thread-safe.
 *
 * @param[in] subscription The subscription to return the state for.
 * @param[out] state To return the state, this can be one of the mamaSubscriptionState enumeration values.
 *
 * @return mama_status return code can be one of:
 *      MAMA_STATUS_OK
 *      MAMA_STATUS_NULL_ARG
 */
MAMAExpDLL
extern mama_status
mamaSubscription_getState(
    mamaSubscription subscription,
    mamaSubscriptionState *state);

/**
 * Returns the underlying <code>mamaSubscriptionType</code> for the specified
 * subscription.
 *
 * @param[in] subscription The subscription for which the type will be returned.
 * @param[out] type Address to which the type will be written.
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_getSubscriptionType(
    mamaSubscription subscription,
    mamaSubscriptionType *type);

/**
 * Return the symbol as subscribed to (i.e., as mapped by the mapping
 * function, if any, for the mamaTransport).
 * Note that this function just returns a pointer to the symbol inside the
 * subscription and does not allocate any memory.
 *
 * @param[in] subscription The subscription.
 * @param[out] symbol holds the result, do not free.
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_getSubscSymbol(
    mamaSubscription subscription,
    const char **symbol);

/**
 * Return the symbol as provided by the user (before any symbol mapping).
 * Note that this function just returns a pointer to the symbol inside the
 * subscription and does not allocate any memory.
 *
 * @param[in] subscription The subscription.
 * @param[out] symbol holds the result, (do not free).
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_getSymbol(
    mamaSubscription subscription,
    const char **symbol);

/**
 * Retrieve the timeout.
 *
 * @param[in] subscription The subscription.
 * @param[out] timeout A pointer to a double to hold the value.
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_getTimeout(
    mamaSubscription subscription,
    double *timeout);

/**
 * Return the <code>mamaTransport</code> for this subscription.
 *
 * @param[in] subscription  The subscription.
 * @param[out] transport     A pointer to hold the transport.
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_getTransport (
    mamaSubscription subscription,
    mamaTransport *transport);

/**
 * Returns a pointer to the <code>mamaMsgCallbacks</code> structure.
 *
 * @param[in] subscription The <code>mamaSubscription</code>
 *
 * @return pointer to the callbacks container structure
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mamaMsgCallbacks *
mamaSubscription_getUserCallbacks(
    mamaSubscription subscription);

/**
 * Returns whether the subscription is active, note that this function has
 * been deprecated, use mamaSubscription_getState instead.
 *
 * @param[in] subscription The subscription.
 *
 * @return whether the subscription is active.
 */
MAMAExpDLL
extern int
mamaSubscription_isActive (
    mamaSubscription subscription);

/**
 * Return whether the subscription is valid, note that this function has
 * been deprecated, use mamaSubscription_getState instead.
 *
 * @param[in] subscription The subscription
 *
 * @return whether the subscription is valid.
 */
MAMAExpDLL
extern int
mamaSubscription_isValid (
    mamaSubscription subscription);

/**
 * Mute the current topic for wildcard subscriptions. Applications should only
 * invoke this method from the message callback. When invoked for a WMW
 * transport subsription, the subscription will cease receiving calbacks on the current
 * topic.
 *
 * This method only works for WMW "transport" subscriptions which are Wild card
 * subscriptions created with a source of a named publisher, and a NULL topic.
 *
 * @param[in] subscription The subscription
 *
 * @return mama_status return code can be one of:
 *          MAMA_STATUS_NO_BRIDGE_IMPL
 *          MAMA_STATUS_NULL_ARG
 *          MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_muteCurrentTopic(
    mamaSubscription subscription);

/**
 * Set the application-specific data type.
 *
 * @param[in] subscription The subscription.
 * @param[in] appDataType The application-specific data type.
 *
 * @return mama_status return code can be one of:
 *
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_setAppDataType(
    mamaSubscription subscription,
    uint8_t appDataType);

/**
 * Set the debug level for this subscription.
 *
 * @param[in] subscription The subscription.
 * @param[in] level        The new debug level.
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_setDebugLevel (
    mamaSubscription subscription,
    MamaLogLevel level);

/**
 * A hint as to the expected size of size of groups when making group subscriptions.
 *
 * @param[in] subscription The subscription
 * @param[in] groupSizeHint The size of the groups
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_setGroupSizeHint (
    mamaSubscription subscription,
    int groupSizeHint);

/**
 * Set the item closure for group subscriptions.
 *
 * Group subscriptions receive updates for multiple symbols. This method allows
 * calls to set a per-symbol closure which will be passed as the fourth argument
 * to subsequent calls to the onMsg callback. This method may only be called
 * during the onMsg callback.
 *
 * This method also sets per-topic closures for wildcard subscriptions.
 *
 * Setting the item closure for a non-group subscription provides a second
 * closure.
 *
 * @param[in] subscription The subscription.
 * @param[in] closure The new item closure.
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_setItemClosure (
    mamaSubscription subscription,
    void *closure);

/**
 * Set a filter to discard messages.
 *
 * @param[in] subscription The subscription on which the filter is being set.
 *
 * @param[in] ignoreDefinitelyDuplicate If true callbacks will not be invoked
 * for messages where MamaMsg::getIsDefinitelyDuplicate returns true.
 *
 * @param[in] ignorePossiblyDuplicate If true callbacks will not be invoked
 * for messages where MamaMsg::getIsPossiblyDuplicate returns true.
 *
 * @param[in] ignoreDefinitelyDelayed If true callbacks will not be invoked
 * for messages where MamaMsg::getIsDefinitelyDelayed returns true.
 *
 * @param[in] ignorePossiblyDelayed If true callbacks will not be invoked
 * for messages where MamaMsg::getIsPossiblyDelayed returns true.
 *
 * @param[in] ignoreOutOfSequence If true callbacks will not be invoked for
 * messages where MamaMsg::getIsOutOfSequence returns true.
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
mama_status
mamaSubscription_setMsgQualifierFilter(
    mamaSubscription subscription,
    int ignoreDefinitelyDuplicate,
    int ignorePossiblyDuplicate,
    int ignoreDefinitelyDelayed,
    int ignorePossiblyDelayed,
    int ignoreOutOfSequence);

/**
 * Set the number of messages to cache for each symbol before the initial
 * value arrives. This allows the subscription to recover when the initial
 * value arrives late (after a subsequent trade or quote already arrived).
 *
 * For group subscription, a separate cache is used for each group member.
 *
 * The default is 10.
 *
 * @param[in] subscription The subscription.
 * @param[in] cacheSize The size of the cache.
 *
 * @return mama_status value can be one of
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_setPreIntitialCacheSize(
    mamaSubscription subscription,
    int cacheSize);

/**
 * Whether a subscription should attempt to recover from
 * sequence number gaps.
 *
 * @param[in] subscription The subscription
 * @param[out] doesRecover 0 indicates not to recover. 1 The subscription will
 * attempt to recover via a recap request.
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_setRecoverGaps (
    mamaSubscription subscription,
    int              doesRecover);

/**
 * Whether an initial value is required for the specified subscription.
 * This only applies to market data subscriptions and not to basic
 * subscriptions.
 * Default value of 1 indicating that initial values are required.
 *
 * @param[in] subscription The subscription
 * @param[in] requiresInitial [1|0] Whether to request an initial value or not.
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_setRequiresInitial (
    mamaSubscription subscription,
    int requiresInitial);

/**
 * Set the service level. This method must be invoked before createXXX ().
 *
 * @param[in] subscription The subscription for which the service level is being
 * set.
 * @param[in] serviceLevel The service level of the subscription (real time,
 * snapshot, etc.). The default is MAMA_SERVICE_LEVEL_REAL_TIME.
 * @param[in] serviceLevelOpt An optional argument for certain service
 * levels. (Defaults to 0 - for future use)
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_setServiceLevel(
    mamaSubscription subscription,
    mamaServiceLevel serviceLevel,
    long serviceLevelOpt);

/**
 * Set the number of retries when requesting recaps.
 *
 * @param[in] subscription The subscription.
 * @param[in] retries The number of retries.
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_setRetries (
    mamaSubscription subscription,
    int retries);

/**
 * Set the subscription type for the subscription being created.
 * If not called the subscription type defaults to MAMA_SUBSC_TYPE_NORMAL.
 * See <code>mamaSubscriptionType</code> enum for valid values.
 *
 * @param[in] subscription The subscription for which the type is being set.
 * @param[in] type The type of the subscription being created.
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_setSubscriptionType(
    mamaSubscription subscription,
    mamaSubscriptionType type);

/**
 * Set the symbol (e.g. to change symbol mapping).
 *
 * @param[in] subscription The subscription.
 * @param[in] symbol.
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_NOMEM
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_setSymbol (
    mamaSubscription subscription,
    const char *symbol);

/**
 * Set the timeout for this subscription. The timeout is used for
 * requesting recaps.
 *
 * @param[in] subscription The subscription.
 * @param[in] timeout The timeout in seconds.
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_setTimeout (
    mamaSubscription subscription,
    double timeout);

/**
 * Set the parameters for a subscription that may be actually
 * activated later.  Activate the subscription using
 * mamaSubscription_activate().
 *
 * @param[in] subscription The subscription.
 * @param[in] queue The mama queue.
 * @param[in] callbacks The mamaMsgCallbacks structure containing the callback
 * functions.
 * @param[in] source The MAMA source to use.
 * @param[in] symbol The symbol name.
 * @param[in] closure The closure will be passed to subsequent callback
 * invocations for this subscription.
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_INVALID_ARG
 *              MAMA_STATUS_INVALID_QUEUE
 *              MAMA_STATUS_SUBSCRIPTION_INVALID_STATE
 *              MAMA_STATUS_NO_BRIDGE_IMPL
 *              MAMA_STATUS_NOMEM
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_setup(
    mamaSubscription          subscription,
    mamaQueue                 queue,
    const mamaMsgCallbacks*   callbacks,
    mamaSource                source,
    const char*               symbol,
    void*                     closure);

/**
 * Set the parameters for a subscription that may be actually
 * activated later.  Activate the subscription using
 * mamaSubscription_activate().
 *
 * @param[in] subscription The subscription.
 * @param[in] transport The MAMA transport.
 * @param[in] queue The mama queue.
 * @param[in] callbacks The mamaMsgCallbacks structure containing the callback
 * functions.
 * @param[in] sourceName The source name..
 * @param[in] symbol The symbol name.
 * @param[in] closure The closure will be passed to subsequent callback
 * invocations for this subscription.
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_INVALID_ARG
 *              MAMA_STATUS_INVALID_QUEUE
 *              MAMA_STATUS_SUBSCRIPTION_INVALID_STATE
 *              MAMA_STATUS_NO_BRIDGE_IMPL
 *              MAMA_STATUS_NOMEM
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_setup2(
    mamaSubscription subscription,
    mamaTransport transport,
    mamaQueue queue,
    const mamaMsgCallbacks *callbacks,
    const char *sourceName,
    const char *symbol,
    void *closure);

/**
 * Set the parameters for a wildcard  subscription that may be actually
 * activated later.  Activate the subscription using
 * mamaSubscription_activate().
 *
 * The topic must be a valid wildcard topic for the underlying middleware.
 *
 * Applications may set and retireve per-topic closures using
 * mamaSubscription_setItemClosure() however this method is only guaranteed to
 * set the correct closure if called while in the msg callback. It will be the
 * topicClosure argument for subsequent callback invocations for the current
 * topic. The topicClosure argument to the callback is NULL prior to an
 * application setting the closure for the topic.
 *
 * @param subscription The subscription.
 * @param transport The transport to use.
 * @param queue The mama queue.
 * @param callbacks The mamaMsgCallbacks structure containing the callback
 * functions.
 * @param source The source name of the feed handler to provide the
 * subscription.
 * @param symbol The symbol name.
 * @param closure The closure will be passed to subsequent callback
 * invocations for this subscription.
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_INVALID_ARG
 *              MAMA_STATUS_INVALID_QUEUE
 *              MAMA_STATUS_SUBSCRIPTION_INVALID_STATE
 *              MAMA_STATUS_NO_BRIDGE_IMPL
 *              MAMA_STATUS_NOMEM
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_setupBasicWildCard (
    mamaSubscription                 subscription,
    mamaTransport                    transport,
    mamaQueue                        queue,
    const mamaWildCardMsgCallbacks*  callbacks,
    const char*                      source,
    const char*                      symbol,
    void*                            closure);

/**
 * This function will return the string representation of the subscription state. The subscription state
 * can be obtained by calling the mamaSubscription_getState function.
 *
 * @param[in] state The state to obtain the string representation for.
 *
 * @return A string representation of the state code or "State not recognised" if it is not recognised.
 */
MAMAExpDLL
extern const char*
mamaSubscription_stringForState(
    mamaSubscriptionState state);

#if defined (__cplusplus)
}
#endif

#endif
