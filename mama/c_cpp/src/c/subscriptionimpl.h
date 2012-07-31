/* $Id: subscriptionimpl.h,v 1.41.4.2.2.2.4.5 2011/10/10 16:03:18 emmapollock Exp $
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

#ifndef MamaSubscriptionImplH__
#define MamaSubscriptionImplH__

/* *************************************************** */
/* Includes. */
/* *************************************************** */
#include "bridge.h"
#include "dqstrategy.h"
#include "mama/mama.h"

#ifdef WITH_ENTITLEMENTS
#include <OeaSubscription.h>
#endif

#if defined(__cplusplus)
extern "C" {
#endif

/* *************************************************** */
/* Definitions. */
/* *************************************************** */

/* The default number of retries when requesting initial and recap images. */
#define MAMA_SUBSCRIPTION_DEFAULT_RETRIES 2

/* The default timeout when requesting initial and recap images. */
#define MAMA_SUBSCRIPTION_DEFAULT_TIMEOUT 10.0

/* *************************************************** */
/* Enumerations. */
/* *************************************************** */

/* For subject hash table */
typedef enum
{
    NOT_DETERMINED = 0,
    IS_ENTITLED    = 1,
    NOT_ENTITLED   = 2
} EntitleStatus;


/* For Wildcard subscriptions. The wc_transport type is likely only available
 * for LDMA/RDMA WDF transports. It is a subscription to every message on the
 * transport that bypasses the naming service. Other implementations may
 * ingore this.
 */
typedef enum
{
    wc_none      = 0,   /* Not wildcard subsc */
    wc_transport = 1,   /* "Transport" wildcard */
    wc_wildcard  = 2    /* normal wildcard */

} wildCardType;

/* *************************************************** */
/* Structures. */
/* *************************************************** */

/* For wild card and group subscriptions subject hash table we track item/topic 
 * specific context information for each subject that we encounter. This
 * information includes whether the user is entitled to the subject, the
 * sequence number if available, and a user supplied closure. The user can set
 * the closure with mamaSubscription_setTopicClosure () and it will be passed
 * back to the user in the onMsg () callback. This avoids the client having to
 * maintain and access its own hash table.
 */

typedef struct SubjectContext_
{    
#ifdef WITH_ENTITLEMENTS
    EntitleStatus       mEntitlement; /* default to NOT_DETERMINED! */
    int32_t             mEntitleCode;
    uint8_t             mEntitlementAlreadyVerified;
    oeaSubscription*    mOeaSubscription;
#endif
    /* The data quality context includes a recap request. */
    mamaDqContext       mDqContext;
    void*               mClosure;
    unsigned short      mInitialArrived;
    char*               mSymbol;
    int                 mImageCount; 
    
} SubjectContext_;
    
/* *************************************************** */
/* Internal Function Prototypes. */
/* *************************************************** */

/**
 * This function will cancel the subscription preventing further update messages being
 * received from the bridge. Note that the subscription will still be active.
 *
 * @param[in] subscription The subscription. 
 *
 * @return mama_status value can be one of
 *              MAMA_STATUS_NO_BRIDGE_IMPL
 *              MAMA_STATUS_NULL_ARG 
 *              MAMA_STATUS_OK
 */
extern mama_status
mamaSubscription_cancel(
    mamaSubscription subscription);

/**
 * This function will verify that the sequence number in the supplied message
 * is valid. If not the quality of the subscription will change and a recap
 * request will be issued.
 *
 * @param[in] subscription The subscription.
 * @param[in] msg The message containing the sequence number.
 * @param[in] msgType The type of message.
 * @param[in] ctx The subscription context.
 */
void 
mamaSubscription_checkSeqNum(
    mamaSubscription subscription,
    mamaMsg msg, 
    int msgType,
    SubjectContext *ctx);

/**
 * This function will clear all references to the transport inside the subscription and its dependant
 * objects. This is used whenever the transport is destroyed before any subscriptions on it are fully
 * cleaned up.
 *
 * @oaram(in) subscription The subscription.
 *
 */
void mamaSubscription_clearTransport(
    mamaSubscription subscription);

/**
 * This function will invoke the user supplied callback to deliver the message.
 *
 * @param[in] subscription The subscription.
 * @param[in] msg The mesage to deliver.
 */
void mamaSubscription_forwardMsg(
    mamaSubscription subscription, 
    const mamaMsg msg);

/**
 * This function will return a flag indicating if the subscription accepts multiple
 * initial values, this can be the case with group or wildcard subscriptions.
 *
 * @param[in] subscription The subscription. 
 *
 * @return 1 indicates that multiple initials are accepted else 0.
 */
extern int
mamaSubscription_getAcceptMultipleInitials(
    mamaSubscription subscription);

/**
 * This function obtains the advisory cause from the tranpsort that this subscription runs on.
 * This is called after an advisory message comes in on the transport.
 *
 * @param[in] subscription The subscription.
 * @param[out] cause To return the cause.
 * @param[out] platformInfo to return the platform specific information.
 */      
extern void
mamaSubscription_getAdvisoryCauseAndPlatformInfo(
    mamaSubscription subscription,
    short *cause,
    const void **platformInfo);

/**
 * This function returns the bridge from the subscription.
 *
 * @param[in] subscription The subscription. 
 *
 * @return The bridge.
 */
MAMAExpDLL
extern mamaBridgeImpl*
mamaSubscription_getBridgeImpl(
    mamaSubscription subscription);           

/**
 * This function returns a flag indicating whether or not the subscription is expecting an initial to
 * be delivered.
 *
 * @param[in] subscription The subscription.
 * @param[out] expectingInitial 1 if the subscription is expecting an initial else 0.
 *
 * @return mama_status value can be one of
 *              MAMA_STATUS_NULL_ARG
 *              MAAM_STATUS_OK
 */              
mama_status mamaSubscription_getExpectingInitial(
    mamaSubscription subscription, 
    int *expectingInitial); 

/**
 * This function will return the count of initials received by the subscription.
 *
 * @param[in] subscription The subscription. 
 *
 * @return The current number of initials received.
 */
extern int
mamaSubscription_getInitialCount(
    mamaSubscription subscription);

/**
 * This function will return the data quality state of the subscription.
 *
 * @param[in] subscription The subscription.
 * @param[out] quality To return the quality state.
 *
 * @return mama_status value can be one of
 *              MAMA_STATUS_NULL_ARG
 *              MAAM_STATUS_OK
 */              
extern mama_status
mamaSubscription_getQuality(
    mamaSubscription subscription,
    mamaQuality *quality);

/**
 * This function returns a pointer to the subscription root.
 *
 * @param[in] subscription The subscription.
 *
 * @return Pointer to the root string, this must not be freed.
 */
const char *
mamaSubscription_getSubscRoot(
    mamaSubscription subscription);

/**
 * This function turns on or off stats logging for a subscription
 *
 * @param[in] subscription The subscription.
 * @param[in] logStats Whether to turn logging on or off *
 */
void 
mamaSubscription_setLogStats (mamaSubscription  subscription,
                              short             logStats);

/**
 * This function returns the stats logging status of the subscription.
 *
 * @param[in] subscription The subscription.
 *
 * @return Whether or not stats are being logged.
 */
short
mamaSubscription_getLogStats (mamaSubscription  subscription);


/**
 * This function returns the transport index used by the subscription.
 *
 * @param[in] subscription The subscription.
 * @param[out] tportIndex To return the transport index.
 *
 * @return mama_status value can be one of:
 *          MAMA_STATUS_NULL_ARG
 *          MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_getTransportIndex(
    mamaSubscription subscription, 
    int *tportIndex);

/**
 * This function returns the type of wildcard subscription.
 *
 * @param[in] subscription The subscription.
 *
 * @return The wildcard subscription enumeration value.
 */
MAMAExpDLL
extern wildCardType
mamaSubscription_getWildCardType(
    mamaSubscription subscription);

/**
 * This function returns a flag indicating if the subcription is a wildcard
 * subscription.
 *
 * @param[in] subscription The subscription.
 *
 * @return 1 if it is a wildcard subscription or else 0.
 */
int 
mamaSubscription_hasWildcards(
    mamaSubscription subsc);

/**
 * This function will increment the count of initials received by the subscription.
 *
 * @param[in] subscription The subscription. 
 */
extern void
mamaSubscription_incrementInitialCount(
    mamaSubscription subscription);

/**
 * This function will return a flag indicating if the subscription expects updates, certain
 * types of subscriptions, (for example snapshots), do not take updates.
 *
 * @param[in] subscription The subscription to check.
 *
 * @return 1 if the subscription is expecting updated or else 0.
 */
extern int
mamaSubscription_isExpectingUpdates(
    mamaSubscription subscription);

/**
 * This function determines if the transport backing up the subscription has been disconnected.
 *
 * @param[in] subscription The subscription.
 *
 * @return 1 if the transport has been disconnected else 0.
 */
MAMAExpDLL
extern int
mamaSubscription_isTportDisconnected(
    mamaSubscription subscription);

/**
 * This function will process an error by invoking the user supplied error callback.
 *
 * @param[in] subscription The subscription.
 * @param[in] deactivate If set to 0 the subscription will also be deactivated.
 *
 * @return mama_status value can be one of
 *              MAMA_STATUS_NO_BRIDGE_IMPL
 *              MAMA_STATUS_NULL_ARG
 *              MAAM_STATUS_OK
 */                            
MAMAExpDLL
extern mama_status
mamaSubscription_processErr(
    mamaSubscription subscription,
    int deactivate);

/**
 * This function will process a new message when it arrives and is typically called
 * by the bridge.
 *
 * @param[in] subscription The subscription.
 * @param[in] message The message to process.
 *
 * @return mama_status value can be one of
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_processMsg(
    mamaSubscription subscription, 
    mamaMsg message);

/**
 * This function will process a new message when it arrives and is typically called
 * by the bridge. This function is used for wildcard subscriptions.
 *
 * @param[in] subscription The subscription.
 * @param[in] message The message to process.
 * @param[in] topic The topic the message has been received for.
 * @param[in] topicClosue Symbol specific closure.
 *
 * @return mama_status value can be one of
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_processWildCardMsg(
    mamaSubscription subscription, 
    mamaMsg msg,
    const char *topic,
    void *topicClosure);

/**
 * This function will process a tport message when it arrives and is typically called
 * by the bridge. This function is used for wildcard subscriptions.
 *
 * @param[in] subscription The subscription.
 * @param[in] message The message to process. 
 * @param[in] topicClosue Symbol specific closure.
 *
 * @return mama_status value can be one of
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_processTportMsg(
    mamaSubscription subscription, 
    mamaMsg msg,
    void *topicClosure);

/**
 * This function sends a recap request for the subscription.
 *
 * @param[in] subscription The subscription. 
 *
 * @return mama_status value can be one of
 *              MAMA_STATUS_NOT_IMPLEMENTED - for group subscriptions.
 *              MAAM_STATUS_OK
 */
extern mama_status 
mamaSubscription_requestRecap(
    mamaSubscription subscription);

/**
 * This function returns a flag indicating whether a bridge subscription needs
 * to be established. Certain types of subscription such as snapshots do not need
 * a bridge subscription.
 *
 * @param[in] subscription The subscription.
 *
 * @return 1 if a bridge subscription is needed or else 0.
 */
int
mamaSubscription_requiresSubscribe(
    mamaSubscription subscription);

/**
 * This function will reset the count of initials received by the subscription
 * to 0.
 *
 * @param[in] subscription The subscription. 
 */
extern void
mamaSubscription_resetInitialCount(
    mamaSubscription subscription);

/**
 * This function will either ignore the refresh event, (if this subscription is the one that
 * originated it), or will reset the refresh count with the transport.
 *
 * @param[in] subscription The subscription.
 */
void 
mamaSubscription_respondToRefreshMessage(
    mamaSubscription subscription);

/**
 * This function will send a refresh message on behalf of this subscription. Note
 * that calling this will cause the next refresh message received to be ignored, (as this
 * subscription is the one sending it).
 *
 * @param[in] subscription The subscription. 
 * @param[in] msg The refresh message to send.
 *
 */
extern void
mamaSubscription_sendRefresh(
    mamaSubscription subscription, 
    const mamaMsg msg );

/**
 * This function sets a flag indicating whether or not the subscription is expecting an initial to
 * be delivered. This is typically used by the image request after an initial has been delivered.
 *
 * @param[in] subscription The subscription.
 * @param[in] expectingInitial 1 if the subscription is expecting an initial else 0.
 *
 * @return mama_status value can be one of
 *              MAMA_STATUS_NULL_ARG
 *              MAAM_STATUS_OK
 */
mama_status
mamaSubscription_setExpectingInitial(
    mamaSubscription subscription,
    int expectingInitial);

/**
 * This function will change the state of the subscription to possibly stale and will
 * result in the quality callback being invoked.
 *
 * @param[in] subscription The subscription.
 *
 * @return mama_status value can be one of:
 *          MAMA_STATUS_NULL_ARG
 *          MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_setPossiblyStale(
    mamaSubscription subscription);

/**
 * This function sets the transport index used by the subscription.
 *
 * @param[in] subscription The subscription.
 * @param[in] tportIndex The new transport index.
 *
 * @return mama_status value can be one of:
 *          MAMA_STATUS_NULL_ARG
 *          MAMA_STATUS_OK
 */
MAMAExpDLL 
extern mama_status 
mamaSubscription_setTransportIndex(
    mamaSubscription subscription, 
    int tportIndex);

/**
 * Set the parameters for a subscription that may be actually
 * activated later.  Activate the subscription using
 * mamaSubscription_activate().
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
 * @return mama_status value can be one of
 *              MAMA_STATUS_INVALID_ARG
 *              MAMA_STATUS_INVALID_QUEUE
 *              MAMA_STATUS_NO_BRIDGE_IMPL
 *              MAMA_STATUS_NOMEM
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_setupBasic(
    mamaSubscription          subscription, 
    mamaTransport             transport,
    mamaQueue                 queue,
    const mamaMsgCallbacks*   callbacks,
    const char*               source,
    const char*               symbol,
    void*                     closure);

/**
 * This function will stop waiting for responses for any outstanding initial or recap requests.
 *
 * @param[in] subscription The subscription.
 * @param[in] ctx The subscription context.
 */
void 
mamaSubscription_stopWaitForResponse(
    mamaSubscription subscription,
    SubjectContext *ctx);

/**
 * This function will cause the transport callback to be invoked with a quality event.
 *
 * @param[in] subscription The subscription. 
 */
extern void
mamaSubscription_unsetAllPossiblyStale(
    mamaSubscription subscription);


#if defined(__cplusplus)
}
#endif

#endif
