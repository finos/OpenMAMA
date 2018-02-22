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

#ifndef MamaSubscriptionImplH__
#define MamaSubscriptionImplH__

/* *************************************************** */
/* Includes. */
/* *************************************************** */
#include "bridge.h"
#include "dqstrategy.h"
#include "mama/mama.h"

#ifndef OPENMAMA_INTEGRATION
  #define OPENMAMA_INTEGRATION
#endif

#include <mama/integration/subscription.h>

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

/* The default timeout when requesting recap images.
 * Defaults to special value used to indicate that subscription
 * timeout should be used instead. */
#define MAMA_SUBSCRIPTION_DEFAULT_RECAP_TIMEOUT -1

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
    EntitleStatus       mEntitlement; /* default to NOT_DETERMINED! */
    int32_t             mEntitleCode;
    uint8_t             mEntitlementAlreadyVerified;
    /* The data quality context includes a recap request. */
    mamaDqContext       mDqContext;
    void*               mClosure;
    unsigned short      mInitialArrived;
    char*               mSymbol;
    int                 mImageCount;
    mamaEntitlementSubscription mEntitlementSubscription;
    mamaEntitlementBridge       mEntitlementBridge;
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
MAMAExpDLL
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
MAMAExpDLL
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
MAMAExpDLL
extern void
mamaSubscription_getAdvisoryCauseAndPlatformInfo(
    mamaSubscription subscription,
    short *cause,
    const void **platformInfo);

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
MAMAExpDLL
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
MAMAExpDLL
extern int
mamaSubscription_isExpectingUpdates(
    mamaSubscription subscription);

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
 * This function will stop waiting for responses for any outstanding initial or recap requests.
 *
 * @param[in] subscription The subscription.
 * @param[in] ctx The subscription context.
 */
MAMAExpDLL
void
mamaSubscription_stopWaitForResponse(
    mamaSubscription subscription,
    SubjectContext *ctx);

/**
 * This function will cause the transport callback to be invoked with a quality event.
 *
 * @param[in] subscription The subscription.
 */
MAMAExpDLL
extern void
mamaSubscription_unsetAllPossiblyStale(
    mamaSubscription subscription);

extern void
mamaSubscription_setIsThrottled(
    mamaSubscription  subscription,
    int               isThrottled);

extern mamaMsgCallbacks *
mamaSubscription_getWombatMsgCallbacks(
    mamaSubscription subscription);

extern void
mamaSubscription_setAcceptMultipleInitials(
    mamaSubscription subscription,
    int              accept);

void
mamaSubscriptionImpl_clearTransport(
    mamaSubscription subscription);

extern const char *
mamaSubscription_getEntitleSubject(
    mamaSubscription subscription);

extern int
mamaSubscription_getRespondToNextRefresh(
    mamaSubscription subscription);

extern mama_status
mamaSubscription_setRespondToNextRefresh(
    mamaSubscription subscription,
    const int value);

extern void
mamaSubscription_resetRefreshForListener(
    mamaSubscription subscription);

extern void MAMACALLTYPE
mamaSubscription_DestroyThroughQueueCB(
    mamaQueue Queue,
    void* closure);

MAMAExpDLL
mamaDqContext*
mamaSubscription_getDqContext(mamaSubscription subscription);

MAMAExpDLL
dqStrategy
mamaSubscription_getDqStrategy(mamaSubscription subscription);

MAMAExpDLL
mama_status
mamaSubscription_setDqStrategy(mamaSubscription subscription, dqStrategy strategy);

MAMAExpDLL
SubjectContext *
mamaSubscription_getSubjectContext (mamaSubscription subscription,
                                        const mamaMsg msg);
MAMAExpDLL
mama_status
mamaSubscription_setDqContext (mamaSubscription subscription, mamaDqContext dqContext);

MAMAExpDLL
mama_status
mamaSubscription_setRecapRequest(mamaSubscription subscription, imageRequest request);

MAMAExpDLL
mama_status
mamaSubscription_getRecapRequest(mamaSubscription subscription, imageRequest request);
#if defined(__cplusplus)
}
#endif

#endif
