/* $Id: subscription.c,v 1.152.4.15.2.8.2.13.2.9 2013/01/18 08:27:04 ianbell Exp $
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

/* *************************************************** */
/* Includes. */
/* *************************************************** */
#include <mama/mama.h>
#include <mamainternal.h>
#include <mama/subscmsgtype.h>
#include <mama/source.h>
#include <subscriptionimpl.h>
#include <publisherimpl.h>
#include <wombat/wtable.h>
#include <list.h>
#include <imagerequest.h>
#include <transportimpl.h>
#include <msgutils.h>
#include <plugin.h>
#include <dictionaryimpl.h>
#include <listenermsgcallback.h>
#include <bridge.h>
#include <string.h>
#include "msgutils.h"
#include "mama/msgqualifier.h"
#include "transportimpl.h"
#include <mama/statfields.h>
#include <mama/statscollector.h>
#include "wlock.h"
#include "wombat/wincompat.h"
#include "wombat/wInterlocked.h"
#include "queueimpl.h"
#include "property.h"
#include <mama/entitlement.h>
#include "entitlementinternal.h"
extern int gGenerateGlobalStats;
extern int gGenerateQueueStats;
extern int gGenerateTransportStats;

#define self ( (mamaSubscriptionImpl*)(subscription))
#define userSymbolFormatted self->mUserSymbol ? self->mUserSymbol : "", \
                            self->mUserSymbol ? ":" : ""
#define userSymbolFormattedImpl impl->mUserSymbol ? impl->mUserSymbol : "", impl->mUserSymbol ? ":" : ""


#define WOMBAT_SUBJECT_MAX 256

#define WOMBAT_SUBSCRIPTION_ROOT_NORMAL "_MD"
#define WOMBAT_SUBSCRIPTION_ROOT_DICT   "_MDDD"

#define DEFULAT_RETRIES  3
#define DEFUALT_TIMEOUT  10

#define DEFAULT_PRE_INITIAL_CACHE 10

#define MINIMUM_GROUP_SIZE_HINT 100

#define PREINITIALCACHESIZEPROPERTY     "mama.subscription.preinitialcachesize"
#define STATE_MACHINE_TRACE_PROPERTY    "mama.subscription.statetrace"

SubjectContext *
mamaSubscription_getSubjectContext (mamaSubscription subscription,
                                        const mamaMsg msg);

typedef struct mamaSubscriptionImpl_
{
    mamaSubscriptionType     mType;
    mamaServiceLevel         mServiceLevel;
    long                     mServiceLevelOpt;
    subscriptionBridge       mSubscBridge; /* SubscriberBridge */
    mamaTransport            mTransport;
    int                      mTransportIndex;    /* Transport bridge index */
    mamaQueue                mQueue;
    char*                    mSubscSymbol; /* or topic */
    char*                    mUserSymbol;
    mamaMsgCallbacks         mUserCallbacks;
    mamaWildCardMsgCallbacks mWcCallbacks;
    void*                    mClosure;
    short                    mIsThrottled;

    /* This lock is used to protect access to this structure during creation and destruction. */
    wLock                    mCreateDestroyLock;

    /* Marketdata specific */
    char*                   mSubscSource;
    char*                   mSubscRoot;
    void*                   mSubscHandle;
    wtable_t                mSubjects;
    imageRequest            mInitialRequest;
    imageRequest            mRecapRequest;
    int                     mRespondToNextRefresh; /* boolean */
    double                  mTimeout;
    double                  mRecapTimeout;
    int                     mRetries;
    int                     mAcceptMultipleInitials;
    char                    mEntitleSubject[WOMBAT_SUBJECT_MAX];
    int                     mRequiresInitial; /* boolean */
    short                   mLogStats;
    listenerMsgCallback     mCallback;

    dqStrategy              mDqStrategy;
    SubjectContext          mSubjectContext; /* For non-group/wc */
    SubjectContext          *mCurSubjectContext; /* For item closure */
    short                   mDqCause;
    const void*             mDqPlatformInfo;

    mamaSubscMsgType        mSubscMsgType;
    uint8_t                 mAppDataType;
    int                     mInitialCount;

    mamaPublisher           mSubscPublisher;
    MamaLogLevel            mDebugLevel;

    int                     mPreInitialCacheSize;
    uint16_t                mMsgQualFilter;
    int                     mExpectingInitial;
    int                     mStateMachineTrace;

    mamaBridgeImpl*         mBridgeImpl;

    int                     mGroupSizeHint;
    wombatThrottleAction    mAction;
    wildCardType            mWildCardType;

    /* The current state of the subscription, this should not be written to directly but should
     * instead be set by calling the mamaSubscriptionImpl_setState private function.
     */
    wInterlockedInt mState;

    /* The queue lock handle. */
    mamaQueueLockHandle mLockHandle;

} mamaSubscriptionImpl;

/* *************************************************** */
/* Private Function Prototypes. */
/* *************************************************** */

/**
 * This function is called to complete the setup of a basic subscription and will typically be invoked
 * by the throttle. It is at this stage that the bridge subscrition will be created.
 *
 * @param[in] subscription The subscritpion to process.
 * @return mama_status value.
 */
static mama_status mamaSubscriptionImpl_completeBasicInitialisation(mamaSubscription subscription);

/**
 * This function is called to complete the setup of a market data subscription and will typically be invoked
 * by the throttle. It is at this stage that the bridge subscrition will be created.
 *
 * @param[in] subscription The subscritpion to process.
 * @return mama_status value.
 */
static mama_status mamaSubscriptionImpl_completeMarketDataInitialisation(mamaSubscription subscription);

/**
 * This function will perform the work required to create a basic subscription and is used internally by the
 * various types of basic subscription. Note that the final setup function will not be placed on the throttle
 * but instead will be invoked directly. To throttle the basic subscription use the setup then activate calls.
 *
 * @param[in] subscription The subscription to create.
 * @param[in] transport The mama transport.
 * @param[in] queue The mama queue that will be used to hold messages for this subscription.
 * @param[in] callabcks Structure contains all of the callback function pointers.
 * @param[in] topic The topic to listen to.
 * @param[in] closure The user supplied closure.
 */
mama_status mamaSubscriptionImpl_createBasic(mamaSubscription subscription, mamaTransport transport, mamaQueue queue, const mamaMsgCallbacks *callbacks, const char *topic, void *closure);

/**
 * This function will perform final clean-up of the subscription including destroying the mutex and
 * deleting the impl. Note that this function will also set the state of the subscription to be
 * MAMA_SUBSCRIPTION_DEALLOCATED which will result in a finest level log message being written out.
 *
 * @param[in] impl The subscritpion impl to deallocate.
 */
void mamaSubscriptionImpl_deallocate(mamaSubscriptionImpl *impl);

/**
 * This function is called by the bridge whenever the bridge subscription has been completely destroyed. Note
 * that this will be done whether the MAMA subscription is being deactivated or destroyed. This function will
 * set the current state of the subscription and will also invoke the corresponding user callback function.
 * If the subscription is being destroyed then additional cleanup will also be performed. This function may
 * also invoke the onError callback if the subscription is in an invalid state, (i.e. neither deactivating or
 * destroying), whenever it is invoked.
 *
 * @param[in] subscription The subscritpion in question.
 * @param[in] closure The closure originally passed into the subscription create function.
 */
void MAMACALLTYPE mamaSubscriptionImpl_onSubscriptionDestroyed(mamaSubscription subscription, void *closure);

/**
 * This function will remove the subscription from the throttle queue. It should only be called whenever the
 * subscription is in the MAMA_SUBSCRIPTION_ACTIVATING state.
 * Note that this function should only be called under the subscription lock.
 *
 * @param[in] impl The subscritpion impl.
 * @return mama_status code can be one of the following:
 *      MAMA_STATUS_SUBSCRIPTION_INVALID_STATE - indicates that the throttle processed the subscription which this
 *                                               function was being called. The subscription is now in the state
 *                                               MAMA_SUBSCRIPTION_ACTIVATED.
 *      MAMA_STATUS_OK
 */
mama_status mamaSubscriptionImpl_removeFromThrottle(mamaSubscriptionImpl *impl);

/**
 * This function will atomically set the state of the subscription and write a corresponding finest level
 * log message.
 *
 * @param[in] impl The subscritpion impl to change the state of.
 * @param[in] state The new state.
 */
void mamaSubscriptionImpl_setState(mamaSubscriptionImpl *impl, mamaSubscriptionState state);

/**
 * This function will invoke the user callback function.
 *
 * @param[in] impl The subscritpion impl to change the state of.
 * @param[in] closure The closure will be passed back up to the onDestroy callback function.
 */
void mamaSubscriptionImpl_invokeDestroyedCallback(mamaSubscriptionImpl *impl);

const char *
mamaSubscription_getEntitleSubject (mamaSubscription subscription);

int
mamaSubscription_getRespondToNextRefresh (mamaSubscription subscription);

mama_status
mamaSubscription_setRespondToNextRefresh (mamaSubscription subscription,
                                          const int value);

void MAMACALLTYPE mamaSubscription_DestroyThroughQueueCB(mamaQueue Queue, void* closure);

void
mamaSubscription_resetRefreshForListener (mamaSubscription subscription);

void mamaSubscription_setIsThrottled (mamaSubscription  subscription,
                                      int               isThrottled);

mamaMsgCallbacks *
mamaSubscription_getWombatMsgCallbacks (mamaSubscription subscription);

void
mamaSubscription_setAcceptMultipleInitials (mamaSubscription subscription,
                                            int              accept);

void mamaSubscriptionImpl_clearTransport(mamaSubscription subscription);

/* Forward declarations */
static int
isEntitledToSymbol (
    const char* source,
    const char* symbol,
    mamaSubscription subscription,
    mamaTransport transport);

static void
mamaSubscription_initialize (
    mamaSubscription  subscription);

static mama_status
setSubscInfo (
    mamaSubscription  subscription,
    mamaTransport     transport,
    const char*       root,
    const char*       source,
    const char*       symbol);

static mama_status
clearSubscInfo (
    mamaSubscription  subscription);


static mama_status
getSubscribeMessage (
    mamaSubscription  subscription,
    mamaMsg*          msg);

static mama_status
configureForMultipleTopics (
    mamaSubscription  subscription);

static char* copyString (
    const char*   str);

static void  checkFree  (
    char**        str);

static char* determineMappedSymbol (
    mamaTransport  transport,
    const char*    symbol);

/*Used to destroy messages sent on the throttle*/
void MAMACALLTYPE sendCompleteCb (mamaPublisher publisher,
                mamaMsg       msg,
                mama_status   status,
                void*         closure);


mama_status mamaSubscription_activate_internal(mamaSubscriptionImpl *impl);
mama_status mamaSubscription_deactivate_internal(mamaSubscriptionImpl *impl);

/* end forward declarations */


/*
* Callback used for creating subscriptions from the throttle queue
*/
static void createAction (void *subscription, void *closure2)
{
    mamaSubscriptionImpl_completeMarketDataInitialisation (self);

}

static mama_status
mamaSubscription_create_ (
    mamaSubscription         subscription,
    mamaQueue                queue,
    const mamaMsgCallbacks*  callbacks,
    mamaSource               source,
    const char*              symbol,
    void*                    closure)
{
    mama_status status = mamaSubscription_setup (
        subscription,
        queue,
        callbacks,
        source,
        symbol,
        closure);
    if (MAMA_STATUS_OK == status)
    {
        status = mamaSubscription_activate (subscription);
    }
    return status;
}

mama_status
mamaSubscription_allocate (
            mamaSubscription*         result)
{
    const char * propValue;
    mamaSubscriptionImpl* impl  = NULL;

    impl = (mamaSubscriptionImpl*)calloc (1, sizeof (mamaSubscriptionImpl));
    if (!impl) return MAMA_STATUS_NOMEM;

    impl->mType                   = MAMA_SUBSC_TYPE_NORMAL;
    impl->mServiceLevel           = MAMA_SERVICE_LEVEL_REAL_TIME;
    impl->mServiceLevelOpt        = 0;
    impl->mSubscBridge            = NULL;
    impl->mTransport              = NULL;
    impl->mTransportIndex         = 0;
    impl->mQueue                  = NULL;
    impl->mSubscSymbol            = NULL;
    impl->mUserSymbol             = NULL;
    impl->mClosure                = NULL;
    impl->mIsThrottled            = 1;
    impl->mSubscSource            = NULL;
    impl->mSubscRoot              = NULL;
    impl->mSubscHandle            = NULL;
    impl->mSubjects               = NULL;
    impl->mInitialRequest         = NULL;
    impl->mRecapRequest           = NULL;
    impl->mRespondToNextRefresh   = 1;
    impl->mTimeout                = MAMA_SUBSCRIPTION_DEFAULT_TIMEOUT;
    impl->mRecapTimeout           = MAMA_SUBSCRIPTION_DEFAULT_RECAP_TIMEOUT;
    impl->mRetries                = MAMA_SUBSCRIPTION_DEFAULT_RETRIES;
    impl->mAcceptMultipleInitials = 0;
    impl->mEntitleSubject[0]      = '\0';
    impl->mRequiresInitial        = 1;
    impl->mCallback               = NULL;
    impl->mDqStrategy             = NULL;
    impl->mSubscMsgType           = MAMA_SUBSC_SUBSCRIBE;
    impl->mAppDataType            = MAMA_MD_DATA_TYPE_STANDARD;
    impl->mInitialCount           = 0;
    impl->mSubscPublisher         = NULL;
    impl->mDebugLevel             = MAMA_LOG_LEVEL_WARN;
    impl->mPreInitialCacheSize    = DEFAULT_PRE_INITIAL_CACHE;
    impl->mExpectingInitial       = 0;
    impl->mGroupSizeHint          = 0;
    impl->mStateMachineTrace      = 0;



    /* Create the mutex. */
    impl->mCreateDestroyLock = wlock_create();

    propValue = mama_getProperty (PREINITIALCACHESIZEPROPERTY);

    if (propValue)
    {
        impl->mPreInitialCacheSize = atoi (propValue);
        mama_log (MAMA_LOG_LEVEL_FINE, "PreInitialCacheSize set to %d", impl->mPreInitialCacheSize);
    }

    propValue = mama_getProperty(STATE_MACHINE_TRACE_PROPERTY);

    if (propValue) {
        impl->mStateMachineTrace = properties_GetPropertyValueAsBoolean(propValue);
        mama_log(MAMA_LOG_LEVEL_FINE, "Subscription state machine logging set to %d", impl->mStateMachineTrace);
    }

    /* Initialise the state. */
    wInterlocked_initialize(&impl->mState);

    /* Set the initial state of the subscription now that the memory has been allocated. */
    mamaSubscriptionImpl_setState(impl, MAMA_SUBSCRIPTION_ALLOCATED);

    *result = impl;


    return MAMA_STATUS_OK;
}

mama_status
mamaSubscription_setupBasic (
    mamaSubscription         subscription,
    mamaTransport            transport,
    mamaQueue                queue,
    const mamaMsgCallbacks*  callbacks,
    const char*              source,
    const char*              symbol,
    void*                    closure)
{
    const char*             root                    = WOMBAT_SUBSCRIPTION_ROOT_NORMAL;
    mamaSubscMsgType        subscMsgType            = MAMA_SUBSC_SUBSCRIBE;
    int                     acceptMultipleInitials  = 0;
    int                     checkSeqNumGaps         = 1;
    mama_status             status                  = MAMA_STATUS_OK;
    mamaEntitlementBridge   mamaEntBridge           = NULL;
    mama_status             entitlementStatus       = MAMA_STATUS_OK;

    if (!self)      return MAMA_STATUS_NULL_ARG;
    if (!transport) return MAMA_STATUS_INVALID_ARG;
    if (!queue)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaSubscription_setupBasic(): NULL"
                  " Queue");
        return MAMA_STATUS_INVALID_QUEUE;
    }

    /*The subscription gets the bridge impl from the transport*/
    self->mBridgeImpl             =
        mamaTransportImpl_getBridgeImpl (transport);

    if (!self->mBridgeImpl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,"mamaSubscription_setupBasic():"
                  "Could not get bridge impl from transport.");
        return MAMA_STATUS_NO_BRIDGE_IMPL;
    }

    {
        mamaBridgeImpl* bridge = mamaSubscription_getBridgeImpl(subscription);
        if (mamaBridgeImpl_areEntitlementsDeferred(bridge))
        {
            mama_log (MAMA_LOG_LEVEL_FINER,
                        "Entitlements checking at subscription creation deferred to %s bridge [%p]",
                        bridge->bridgeGetName(), bridge);
        }
        else
        {
            mama_status status = mamaTransportImpl_getEntitlementBridge(transport, &mamaEntBridge);
            if (NULL != mamaEntBridge)
            {
                mamaEntBridge->createSubscription(mamaEntBridge, &self->mSubjectContext);
            }
            else
            {
                mama_log(MAMA_LOG_LEVEL_ERROR,
                         "mamaSubscription_setupBasic(): Could not find entitlement bridge!");
                return MAMA_STATUS_NO_BRIDGE_IMPL;
            }
        }


    }

    if (!isEntitledToSymbol (source, symbol, self, transport))
    {
        setSubscInfo (self, transport, root, source, symbol);
        return MAMA_STATUS_NOT_ENTITLED;
    }

    self->mSubjectContext.mSymbol = copyString (symbol);

    /* mamaSubscMsgType is being removed in favor of a combination of
     * mamaServiceLevel, mamaSubscriptionType.  For backward
     * compatibility with older feed handlers, we still need to send
     * an appropirate mamaSubscMsgType. */
    switch (self->mServiceLevel)
    {
        case MAMA_SERVICE_LEVEL_REAL_TIME:
            break;
        case MAMA_SERVICE_LEVEL_SNAPSHOT:
            if (!self->mRequiresInitial) return MAMA_STATUS_INVALID_ARG;
            subscMsgType = MAMA_SUBSC_SNAPSHOT;
            {
                mamaBridgeImpl* bridge = mamaSubscription_getBridgeImpl(subscription);
                if (!(mamaBridgeImpl_areEntitlementsDeferred(bridge)))
                    self->mSubjectContext.mEntitlementBridge->setIsSnapshot(self->mSubjectContext.mEntitlementSubscription->mImpl, 1);
            }
            break;
        case MAMA_SERVICE_LEVEL_CONFLATED:/*fall through*/
        default:
            /* Conflation not yet supported! */
            return MAMA_STATUS_INVALID_ARG;
    }

    /*
    * Certain sub properties need to be set depending on the subscription
    * type.
    */
    switch (self->mType)
    {
        case MAMA_SUBSC_TYPE_NORMAL:
            break;
        case MAMA_SUBSC_TYPE_GROUP:
            acceptMultipleInitials = 1;
            checkSeqNumGaps = 1;
            break;
        case MAMA_SUBSC_TYPE_BOOK:
            acceptMultipleInitials = 0;
            checkSeqNumGaps = 1;
            break;
        case MAMA_SUBSC_TYPE_BASIC:
            /* Wrong "create" function call! */
            return MAMA_STATUS_INVALID_ARG;
        case MAMA_SUBSC_TYPE_DICTIONARY:
            root         = WOMBAT_SUBSCRIPTION_ROOT_DICT;
            subscMsgType = MAMA_SUBSC_DDICT_SNAPSHOT;
            break;
        case MAMA_SUBSC_TYPE_SYMBOL_LIST:
        case MAMA_SUBSC_TYPE_SYMBOL_LIST_NORMAL:
            acceptMultipleInitials = 1;
            checkSeqNumGaps = 0;
            symbol = "SYMBOL_LIST_NORMAL";
            break;
        case MAMA_SUBSC_TYPE_SYMBOL_LIST_GROUP:
            acceptMultipleInitials = 1;
            checkSeqNumGaps = 0;
              symbol = "SYMBOL_LIST_GROUP";
            break;
        case MAMA_SUBSC_TYPE_SYMBOL_LIST_BOOK:
            acceptMultipleInitials = 1;
            checkSeqNumGaps = 0;
            symbol = "SYMBOL_LIST_BOOK";
            break;
        default:
            return MAMA_STATUS_INVALID_ARG;
    }

    self->mClosure                      = closure;
    if (callbacks) /* NULL for wild cards */
    {
        self->mUserCallbacks.onCreate       = callbacks->onCreate;
        self->mUserCallbacks.onError        = callbacks->onError;
        self->mUserCallbacks.onMsg          = callbacks->onMsg;
        self->mUserCallbacks.onQuality      = callbacks->onQuality;
        self->mUserCallbacks.onGap          = callbacks->onGap;
        self->mUserCallbacks.onRecapRequest = callbacks->onRecapRequest;
        self->mUserCallbacks.onDestroy      = callbacks->onDestroy;
    }
    self->mAcceptMultipleInitials       = acceptMultipleInitials;
    self->mTransport                    = transport;
    mamaTransportImpl_getTransportIndex (transport, &self->mTransportIndex);
    mamaTransportImpl_nextTransportIndex (transport, source, symbol);
    self->mQueue                  = queue;
    self->mSubscMsgType           = subscMsgType;
    /* Treat 0 as a 'magic number' here. */
    if (self->mGroupSizeHint == 0) /*No group size hint has been set at all, or it has already been set to 0 by setGroupSizeHint (see below) */
    {
        self->mGroupSizeHint          = mamaTransportImpl_getGroupSizeHint (transport);
    }
    /* Do a single safety check here (rather than one in transport.c and one in setGroupSizeHint here) */
    if (self->mGroupSizeHint < MINIMUM_GROUP_SIZE_HINT)
    {
        self->mGroupSizeHint = MINIMUM_GROUP_SIZE_HINT;
    }

    setSubscInfo (self, transport, root, source, symbol);

    /*Create the publisher - needed for the image request object */
    if (MAMA_STATUS_OK!=(status=mamaPublisherImpl_createByIndex (
                                  &self->mSubscPublisher,
                                   self->mTransport,
                                   self->mTransportIndex,
                                   NULL,
                                   NULL,
                                   self->mSubscSymbol,
                                   self->mSubscSource,
                                   self->mSubscRoot,
                                   NULL)))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "Could not create publisher bridge. [%s]",
                  mamaStatus_stringForStatus (status));
        return status;
    }

    if (MAMA_STATUS_OK!=imageRequest_create (&self->mInitialRequest,
                        subscription,
                        &self->mSubjectContext,
                        self->mSubscPublisher,
                        mamaTransportImpl_getThrottle (self->mTransport,
                                                       MAMA_THROTTLE_DEFAULT)))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "Could not create image reqest. [%s]",
                  mamaStatus_stringForStatus (status));
        return status;
    }

    if (MAMA_STATUS_OK!=imageRequest_create (&self->mRecapRequest,
                        subscription,
                        &self->mSubjectContext,
                        self->mSubscPublisher,
                        mamaTransportImpl_getThrottle (self->mTransport,
                                                       MAMA_THROTTLE_RECAP)))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "Could not create recap image reqest. [%s]",
                  mamaStatus_stringForStatus (status));
        return status;
    }

    /*The listenermsgcallback processes data messages*/
    if (MAMA_STATUS_OK!=(status=listenerMsgCallback_create (&self->mCallback,
                                      (mamaSubscription)self)))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "Could not create msg listener. [%s]",
                  mamaStatus_stringForStatus (status));
        return status;
    }

    /*Create the DQ strategy - sequence number checking etc.*/
    if (MAMA_STATUS_OK!=(status=mamaPlugin_fireSubscriptionPostCreateHook (subscription)))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "Could not create DQ strategy. [%s]",
                  mamaStatus_stringForStatus (status));
        return status;
    }

    /* The subscription has now been fully set-up. */
    mamaSubscriptionImpl_setState(self, MAMA_SUBSCRIPTION_SETUP);

    return MAMA_STATUS_OK;
}

mama_status
mamaSubscription_setupBasicWildCard (
    mamaSubscription                  subscription,
    mamaTransport                     transport,
    mamaQueue                         queue,
    const mamaWildCardMsgCallbacks*   callbacks,
    const char*                       source,
    const char*                       symbol,
    void*                             closure)
{
    if (!self) return MAMA_STATUS_NULL_ARG;
    self->mWildCardType = symbol == NULL ? wc_transport : wc_wildcard;
    self->mWcCallbacks = *callbacks;
    /* So these get called when necessary */
    self->mUserCallbacks.onCreate = callbacks->onCreate;
    self->mUserCallbacks.onError  = callbacks->onError;
    self->mUserCallbacks.onDestroy  = callbacks->onDestroy;
    return mamaSubscription_setupBasic (
            subscription,
            transport,
            queue,
            NULL,
            source,
            symbol,
            closure);
}

mama_status mamaSubscription_activate_internal (mamaSubscriptionImpl *impl)
{
    mama_status status = MAMA_STATUS_OK;

    if (!impl->mTransport) return MAMA_STATUS_INVALID_ARG;

    /*We need to reset the context for each reuse of a subscription*/
    if (MAMA_STATUS_OK!=(status=dqContext_initializeContext (
                                    &impl->mSubjectContext.mDqContext,
                                    impl->mPreInitialCacheSize,
                                    impl->mRecapRequest)))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mamaSubscription_activate(): Failed to initialize DQ"
                  " context for subscription [%s]",
                  mamaStatus_stringForStatus(status));
        return status;
    }

    /*The subscription actually gets created on the throttle queue*/
    if (MAMA_STATUS_OK!=(status=mamaTransport_throttleAction (
                                  impl->mTransport,
                                  MAMA_THROTTLE_DEFAULT,
                                  createAction,
                                  impl,
                                  impl,
                                  NULL,
                                  0,
                                  &impl->mAction)))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mamaSubscription_activate(): Failed to throttle activate."
                  " [%s]", mamaStatus_stringForStatus(status));
        return status;
    }

    return MAMA_STATUS_OK;
}

mama_status
mamaSubscription_create (
    mamaSubscription         subscription,
    mamaQueue                queue,
    const mamaMsgCallbacks*  callbacks,
    mamaSource               source,
    const char*              symbol,
    void*                    closure)
{
    return mamaSubscription_create_ (
        subscription,
        queue,
        callbacks,
        source,
        symbol,
        closure);
}

mama_status
mamaSubscription_createSnapshot (
    mamaSubscription         subscription,
    mamaQueue                queue,
    const mamaMsgCallbacks*  callbacks,
    mamaSource               source,
    const char*              symbol,
    void*                    closure)
{
    if (!self) return MAMA_STATUS_NULL_ARG;

    /*Set the properties required for a snapshot subscription*/
    self->mType             = MAMA_SUBSC_TYPE_NORMAL;
    self->mServiceLevel     = MAMA_SERVICE_LEVEL_SNAPSHOT;
    self->mServiceLevelOpt  = 0;
    self->mRequiresInitial  = 1;
    self->mAppDataType      = MAMA_MD_DATA_TYPE_STANDARD;

    return mamaSubscription_create (
        subscription,
        queue,
        callbacks,
        source,
        symbol,
        closure);
}

mama_status
mama_createDictionary (
    mamaDictionary*            dictionary,
    mamaQueue                  queue,
    mamaDictionaryCallbackSet  dictionaryCallback,
    mamaSource                 source,
    double                     timeout,
    int                        retries,
    void*                      closure)
{
    mama_status         result       = MAMA_STATUS_OK;
    mamaSubscription    subscription = NULL;

    if (!queue) return MAMA_STATUS_INVALID_QUEUE;

    result = mamaDictionary_create (dictionary);
    if (result != MAMA_STATUS_OK) return result;

    result = mamaDictionary_setClosure (*dictionary, closure);
    if (result != MAMA_STATUS_OK) return result;

    if (dictionaryCallback.onComplete != NULL)
    {
        mamaDictionary_setCompleteCallback (*dictionary,
                                            dictionaryCallback.onComplete);
    }

    if (dictionaryCallback.onError != NULL)
    {
        mamaDictionary_setErrorCallback (*dictionary,
                                         dictionaryCallback.onError);
    }

    if (dictionaryCallback.onTimeout != NULL)
    {
        mamaDictionary_setTimeoutCallback (*dictionary,
                                           dictionaryCallback.onTimeout);
    }

    result  = mamaSubscription_allocate (&subscription);
    if (MAMA_STATUS_OK!=result)
    {
        mamaDictionary_destroy (*dictionary);
        return result;
    }

    /*Set the properties required for a dictionary subscription*/
    self->mType             = MAMA_SUBSC_TYPE_DICTIONARY;
    self->mServiceLevel     = MAMA_SERVICE_LEVEL_SNAPSHOT;
    self->mServiceLevelOpt  = 0;
    self->mRequiresInitial  = 1;
    self->mAppDataType      = MAMA_MD_DATA_TYPE_STANDARD;
    self->mTimeout          = timeout;
    self->mRetries          = retries;

    mamaDictionary_setSubscPtr (*dictionary, subscription);

    result = mamaSubscription_create_ (
        subscription,
        queue,
        mamaDictionary_getSubscCb (*dictionary),
        source,
        "DATA_DICT",
        (void*)(*dictionary));

    return result;
}

mama_status
mamaSubscription_createBasicWildCard (
    mamaSubscription                  subscription,
    mamaTransport                     transport,
    mamaQueue                         queue,
    const mamaWildCardMsgCallbacks*   callbacks,
    const char*                       source,
    const char*                       symbol,
    void*                             closure)
{
    char buf[MAMA_MAX_SYMBOL_LEN + MAMA_MAX_SOURCE_LEN + 2];
    if (!self) return MAMA_STATUS_NULL_ARG;
    if (symbol)
    {
        if (source)
            snprintf (buf, MAMA_MAX_SYMBOL_LEN + MAMA_MAX_SOURCE_LEN + 2, "%s.%s", source, symbol);
        else
            snprintf (buf, MAMA_MAX_SYMBOL_LEN + 1, "%s", symbol);

        self->mWildCardType = wc_wildcard;
    }
    else
    {
        snprintf (buf, MAMA_MAX_SOURCE_LEN + 1, "%s", source);
        self->mWildCardType = wc_transport;
    }
    self->mWcCallbacks = *callbacks;
    /* So these get called when necessary */
    self->mUserCallbacks.onCreate = callbacks->onCreate;
    self->mUserCallbacks.onError  = callbacks->onError;
    self->mUserCallbacks.onDestroy  = callbacks->onDestroy;

    return mamaSubscription_createBasic (
            subscription,
            transport,
            queue,
            NULL,
            buf,
            closure);
}

mama_status
mamaSubscription_createBasic (
    mamaSubscription         subscription,
    mamaTransport            transport,
    mamaQueue                queue,
    const mamaMsgCallbacks*  callbacks,
    const char*              topic,
    void*                    closure)
{
    return mamaSubscriptionImpl_createBasic(
        subscription,
        transport,
        queue,
        callbacks,
        topic,
        closure
    );
}

static void
mamaSubscription_initialize (mamaSubscription subscription)
{
    mamaStatsCollector queueStatsCollector     = NULL;
    mamaStatsCollector transportStatsCollector = NULL;

    mama_log (MAMA_LOG_LEVEL_FINE,
              "mamaSubscription_initialize(): "
              "Initializing subscription (%p) for source=%s, symbol=%s (%s)",
              subscription,
              self->mSubscSource != NULL ? self->mSubscSource : "",
              self->mSubscSymbol != NULL ? self->mSubscSymbol : "",
              self->mUserSymbol  != NULL ? self->mUserSymbol  : "");

    if (self->mSubscMsgType != MAMA_SUBSC_DDICT_SNAPSHOT &&
        self->mSubscMsgType != MAMA_SUBSC_SNAPSHOT)
    {
        /*Delegate to the correct bridge implementation*/
            mamaMsgCallbacks    cb;

            cb.onCreate = self->mUserCallbacks.onCreate;
            cb.onError = self->mUserCallbacks.onError;
            cb.onMsg = self->mUserCallbacks.onMsg;
            cb.onQuality = self->mUserCallbacks.onQuality;
            cb.onGap = self->mUserCallbacks.onGap;
            cb.onRecapRequest = self->mUserCallbacks.onRecapRequest;
            cb.onDestroy = mamaSubscriptionImpl_onSubscriptionDestroyed;

            self->mBridgeImpl->bridgeMamaSubscriptionCreate
                                      (&self->mSubscBridge,
                                       self->mSubscSource,
                                       self->mSubscSymbol,
                                       self->mTransport,
                                       self->mQueue,
                                       cb,
                                       self,
                                       self->mClosure);
    }
    if (self->mRequiresInitial)
    {

        mamaMsg subscribeMsg = NULL;
        self->mExpectingInitial = 1;
        self->mSubjectContext.mInitialArrived=0;
        getSubscribeMessage (subscription, &subscribeMsg);
        imageRequest_sendRequest (self->mInitialRequest,
                                 subscribeMsg,
                                 self->mTimeout,
                                 self->mRetries,
                                 0,/*Not throttled*/
                                 0);/*Not a recap*/
    }
    else if (mamaSubscription_requiresSubscribe (subscription))
    {
        mamaMsg subscribeMsg = NULL;
        getSubscribeMessage (subscription, &subscribeMsg);
        mamaPublisher_send (self->mSubscPublisher,
                           subscribeMsg);
        mamaMsg_destroy (subscribeMsg);
        mama_log (MAMA_LOG_LEVEL_FINE,
                   "Subscription send request without inbox().");
    }

    if (self->mBridgeImpl &&
        (self->mBridgeImpl->bridgeMamaSubscriptionHasWildcards
            (self->mSubscBridge) || self->mType == MAMA_SUBSC_TYPE_GROUP))
    {
        configureForMultipleTopics (subscription);
    }

    self->mLockHandle = mamaQueue_incrementObjectCount(self->mQueue, subscription);

    if (gGenerateQueueStats)
    {
        queueStatsCollector = mamaQueueImpl_getStatsCollector (self->mQueue);
        mamaStatsCollector_incrementStat (queueStatsCollector,
                                          MamaStatNumSubscriptions.mFid);
    }
    if (gGenerateTransportStats)
    {
        transportStatsCollector = mamaTransport_getStatsCollector (self->mTransport);
        mamaStatsCollector_incrementStat (transportStatsCollector,
                                          MamaStatNumSubscriptions.mFid);
    }
    if (mamaInternal_getGlobalStatsCollector() != NULL)
    {
        mamaStatsCollector_incrementStat (mamaInternal_getGlobalStatsCollector(),
                                          MamaStatNumSubscriptions.mFid);
    }
}

mama_status
getSubscribeMessage (mamaSubscription subscription, mamaMsg *msg)
{

    return msgUtils_createSubscriptionMessage (self,
                                               self->mSubscMsgType,
                                               msg,
                                               NULL);
}



const char *
mamaSubscription_getSubscRoot (mamaSubscription subscription)
{
    return self->mSubscRoot;
}

const char *
mamaSubscription_getEntitleSubject (mamaSubscription subscription)
{
    return self->mEntitleSubject;
}

int
mamaSubscription_getRespondToNextRefresh (mamaSubscription subscription)
{
    return self->mRespondToNextRefresh;
}

mama_status
mamaSubscription_setRespondToNextRefresh (mamaSubscription subscription,
                                          const int value)
{
    self->mRespondToNextRefresh = value;
    return MAMA_STATUS_OK;
}

void
mamaSubscription_resetRefreshForListener (mamaSubscription subscription)
{
    mamaTransportImpl_resetRefreshForListener (self->mTransport,
                                           self->mSubscHandle);
}


void MAMACALLTYPE sendCompleteCb (mamaPublisher publisher,
                mamaMsg       msg,
                mama_status   status,
                void*         closure)
{
    /*Destroy the message once it has been sent from the throttle*/
    if (msg)
    {
        if (MAMA_STATUS_OK!=mamaMsg_destroy (msg))
        {
            mama_log (MAMA_LOG_LEVEL_WARN, "mamaSubscription::"
                      "sendCompleteCb(): Could not destroy refresh message.");
        }
    }
}

void
mamaSubscription_sendRefresh (mamaSubscription subscription,
                                                const mamaMsg msg)
{
    const char* symbol  = NULL;
    const char* source  = NULL;

    if (!self && ! self->mBridgeImpl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaSubscription_sendRefresh():"
                  " Could not send refresh. NULL sub or bridge.");
        return;
    }

    if (self->mBridgeImpl->
                bridgeMamaSubscriptionHasWildcards (self->mSubscBridge))
    {
        return;
    }

    mamaSubscription_getSource (self, &source);
    mamaSubscription_getSymbol (self, &symbol);

    mama_log (MAMA_LOG_LEVEL_FINEST, "mamaSubscription_sendRefresh(): "
              "Sending refresh to: %s (%s)",
              source  == NULL ? "" : source,
              symbol  == NULL ? "" : symbol);

    /* We can't send the template because it sits on the throttle queue
       so we need our own copy. */
    mamaSubscription_setRespondToNextRefresh (self, 0);

    mamaPublisher_sendWithThrottle (self->mSubscPublisher,
                                    msg,
                                    sendCompleteCb,
                                    NULL /*No need to use closure*/);
}

void
mamaSubscription_respondToRefreshMessage (mamaSubscriptionImpl *impl)
{
    if (impl->mRespondToNextRefresh)
    {
        mamaTransportImpl_resetRefreshForListener (impl->mTransport,
                                                   impl->mSubscHandle);
    }
    else
    {
        impl->mRespondToNextRefresh = 1;
    }
}

int
mamaSubscription_hasWildcards (mamaSubscription subscription)
{
    if (!self) return 0;
    if (!self->mBridgeImpl) return 0;

    return self->mBridgeImpl->bridgeMamaSubscriptionHasWildcards
                            (self->mSubscBridge);
}

mama_status
mamaSubscription_setGroupSizeHint (mamaSubscription subscription, int groupSizeHint)
{
    self->mGroupSizeHint = groupSizeHint;
    return MAMA_STATUS_OK;
}

SubjectContext *
mamaSubscription_getSubjectContext (mamaSubscription subscription,
                                        const mamaMsg msg)
{
    const char       *sendSubject       = NULL;
    SubjectContext   *context           = NULL;
    const char*      issueSymbol        = NULL;
    mama_status      status             = MAMA_STATUS_OK;
    mama_status      entitlementStatus  = MAMA_STATUS_OK;

    if (!self->mSubjects) /* no group or wildcard */
    {
        return &self->mSubjectContext;
    }

    status = msgUtils_getIssueSymbol (msg, &sendSubject);

    if (!sendSubject)
    {
        const char* messageString = mamaMsg_toString(msg);
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mamaSubscription_getSubjectContext(): Could not get issue "
                  "symbol from group message [%s] [%s]. Cannot create/find context.",
                  "Message: [%s]", mamaStatus_stringForStatus (status),
                  self->mSubscSymbol == NULL ? "" : self->mSubscSymbol,
                  messageString == NULL ? "" : messageString);
        return NULL;
    }

    context = (SubjectContext*) wtable_lookup (self->mSubjects,
                                                (char *) sendSubject);

    if (context == NULL)
    {
        imageRequest recap = NULL;

        if ( (gMamaLogLevel >= MAMA_LOG_LEVEL_FINE) ||
            (mamaSubscription_checkDebugLevel (self, MAMA_LOG_LEVEL_FINE)))
        {
            mama_log ( MAMA_LOG_LEVEL_FINER, " (%p) Creating context for %s",
                           self, sendSubject);
        }

        context = (SubjectContext*)calloc (1, sizeof (SubjectContext));

        if (MAMA_STATUS_OK!=imageRequest_create (
                        &recap,
                        subscription,
                        context,
                        self->mSubscPublisher,
                        mamaTransportImpl_getThrottle (self->mTransport,
                                                       MAMA_THROTTLE_RECAP)))
        {
            mama_log (MAMA_LOG_LEVEL_ERROR,
                    "Could not create recap image reqest. [%s]",
                    mamaStatus_stringForStatus (status));
            return NULL;
        }

        dqContext_initializeContext (&context->mDqContext, self->mPreInitialCacheSize, recap);
        msgUtils_getIssueSymbol (msg, &issueSymbol);
        context->mSymbol = copyString (issueSymbol);
        {
            mamaBridgeImpl* bridge = mamaSubscription_getBridgeImpl(subscription);
            if (!(mamaBridgeImpl_areEntitlementsDeferred(bridge)))
            {

                mamaTransport tport;
                mamaEntitlementBridge entBridge;
                mamaSubscription_getTransport(subscription, &tport);
                mamaTransportImpl_getEntitlementBridge(tport, &entBridge);

                entBridge->createSubscription (entBridge, context);
            }
        }

        wtable_insert (self->mSubjects, (char*)sendSubject, (void*)context);
    }

    return context;
}

/* Normal case where we stop the response for a paricular context */
void
mamaSubscription_stopWaitForResponse (mamaSubscription subscription,
                                      SubjectContext* ctx)
{
    if(self->mInitialRequest != NULL)
    {
        imageRequest_stopWaitForResponse (self->mInitialRequest);
    }
    if(ctx->mDqContext.mRecapRequest != NULL)
    {
        imageRequest_stopWaitForResponse (ctx->mDqContext.mRecapRequest);
    }
}

int
mamaSubscription_isTportDisconnected(mamaSubscription subscription)
{
   return self->mBridgeImpl->bridgeMamaSubscriptionIsTportDisconnected(self->mSubscBridge);
}

mama_status
mamaSubscription_setPossiblyStale (mamaSubscription subscription)
{
    short       cause;
    const void* platformInfo = NULL;
    mama_status status       = MAMA_STATUS_OK;
    dqState     state        = DQ_STATE_OK;

    if (!self) return MAMA_STATUS_NULL_ARG;
    if (MAMA_STATUS_OK!=(status=dqStrategy_getDqState (&self->mSubjectContext.mDqContext, &state)))
    {
        return status;
    }

    if (state == DQ_STATE_OK)
    {
        if (self->mUserCallbacks.onQuality != NULL)
        {
            mamaTransportImpl_getAdvisoryCauseAndPlatformInfo (self->mTransport,
                                                               &cause,
                                                               &platformInfo);

            self->mUserCallbacks.onQuality (subscription,
                                           MAMA_QUALITY_MAYBE_STALE,
                                           self->mSubscSymbol,
                                       cause,
                                       platformInfo,
                                       self->mClosure);
        }
        return dqStrategy_setPossiblyStale (&self->mSubjectContext.mDqContext);
    }

    return status;
}

mama_status
mamaSubscription_getQuality (mamaSubscription subscription,
                          mamaQuality*        quality)
{
    dqState     state        = DQ_STATE_NOT_ESTABLISHED;

    if (!self) return MAMA_STATUS_NULL_ARG;

    dqStrategy_getDqState (&self->mSubjectContext.mDqContext, &state);


    switch (state)
    {
        case DQ_STATE_OK:
            *quality = MAMA_QUALITY_OK;
            break;

        case DQ_STATE_WAITING_FOR_RECAP:
        case DQ_STATE_STALE_NO_RECAP:
            *quality = MAMA_QUALITY_STALE;
            break;

        case DQ_STATE_POSSIBLY_STALE:
            *quality = MAMA_QUALITY_MAYBE_STALE;
            break;

        case DQ_STATE_DUPLICATE:
            *quality = MAMA_QUALITY_DUPLICATE;
            break;


        default:
        case DQ_STATE_NOT_ESTABLISHED:
            *quality = MAMA_QUALITY_UNKNOWN;
            break;
    }

    return MAMA_STATUS_OK;
}
mama_status
mamaSubscription_requestRecap(mamaSubscription subscription)
{
    if (self->mType == MAMA_SUBSC_TYPE_GROUP)
        return MAMA_STATUS_NOT_IMPLEMENTED;
    else
        return dqStrategy_sendRecapRequest (self->mDqStrategy,
                                                NULL,
                                                &self->mSubjectContext.mDqContext);
}


void
mamaSubscription_unsetAllPossiblyStale (mamaSubscription subscription)
{
    mamaTransportImpl_unsetAllPossiblyStale (self->mTransport);
}

void
mamaSubscription_getAdvisoryCauseAndPlatformInfo (
                                                 mamaSubscription subscription,
                                                 short*           cause,
                                                 const  void**    platformInfo)
{
    mamaTransportImpl_getAdvisoryCauseAndPlatformInfo (self->mTransport,
                                                       cause, platformInfo);
}

mama_status
setSubscInfo (
    mamaSubscription  subscription,
    mamaTransport     transport,
    const char*       root,
    const char*       source,
    const char*       symbol)
{
    if (!self) return MAMA_STATUS_NULL_ARG;

    if (root != NULL)
    {
        checkFree (&self->mSubscRoot);
        self->mSubscRoot = copyString (root);
    }

    if (source != NULL)
    {
        checkFree (&self->mSubscSource);
        self->mSubscSource = copyString (source);
    }

    /*Also check for empty string - tibrv subject is badly formed
     unless the symbol is NULL, better to check here than on every
     send.*/
    if ( (symbol != NULL) && (strcmp (symbol,"")!=0))
    {
        checkFree (&self->mUserSymbol);
        checkFree (&self->mSubscSymbol);
        self->mUserSymbol  = copyString (symbol);
        self->mSubscSymbol = determineMappedSymbol (transport, symbol);
    }
    return MAMA_STATUS_OK;
}

mama_status
clearSubscInfo (
    mamaSubscription  subscription)
{
    if (!self) return MAMA_STATUS_NULL_ARG;
    checkFree (&self->mSubscRoot);
    checkFree (&self->mSubscSource);
    checkFree (&self->mUserSymbol);
    checkFree (&self->mSubscSymbol);
    return MAMA_STATUS_OK;
}

void
mamaSubscription_forwardMsg (mamaSubscription subscription,
                                const mamaMsg msg)
{
    self->mUserCallbacks.onMsg (
        subscription,
        msg,
        self->mClosure,
        self->mCurSubjectContext == NULL ? NULL
                                         : self->mCurSubjectContext->mClosure);
}

void mamaSubscription_clearTransport(mamaSubscription subscription)
{
    /* Cast the subscription to an impl. */
    mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;
    if(NULL != impl)
    {
        /* Clear the transport object. */
        impl->mTransport = NULL;
    }
}

static void freeCacheCb (
    wtable_t table, void *data, const char* key, void *closure)
{
    SubjectContext *ctx = (SubjectContext*)data;
    dqContext_cleanup (&(ctx->mDqContext));
    checkFree (&ctx->mSymbol);
    if (ctx->mEntitlementSubscription != NULL)
    {
        mamaEntitlementBridge_destroySubscription (ctx->mEntitlementSubscription);
        ctx->mEntitlementSubscription = NULL;
    }
    free (ctx);
    ctx = NULL;
}

static mama_status
mamaSubscription_cleanup (mamaSubscription subscription)
{
    /* Clear the transport pointer before doing any clean-up. This is to ensure that the destroy
     * functions for the image request and the strategy won't try and remove pending requests from
     * the transport. This has already been done during de-activation.
     */
    self->mTransport = NULL;

    if (self->mSubscPublisher)
    {
        mamaPublisher_destroy (self->mSubscPublisher);
        self->mSubscPublisher = NULL;
    }

    /* Destroy the initial request inbox. */
    if (self->mInitialRequest != NULL)
    {
        imageRequest_destroy (self->mInitialRequest);
        self->mInitialRequest = NULL;
    }

    if (self->mCallback)
    {
        listenerMsgCallback_destroy (self->mCallback);
        self->mCallback = NULL;
    }

    if (self->mSubjects != NULL)
    {
        wtable_clear_for_each (self->mSubjects, freeCacheCb, NULL);
        wtable_destroy (self->mSubjects);
        self->mSubjects = NULL;
    }
    else
    {
        if (self->mSubjectContext.mEntitlementSubscription != NULL)
        {
            /* Destroy will also close a subscription if it is open */
            mamaEntitlementBridge_destroySubscription (self->mSubjectContext.mEntitlementSubscription);
            self->mSubjectContext.mEntitlementSubscription = NULL;
        }
    }
    
    dqContext_cleanup (&self->mSubjectContext.mDqContext);
    self->mDqStrategy   = NULL;
    self->mRecapRequest = NULL;

    checkFree (&self->mSubjectContext.mSymbol);

    return MAMA_STATUS_OK;
}

mama_status mamaSubscription_deactivate_internal(mamaSubscriptionImpl *impl)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_OK;


    mama_log (MAMA_LOG_LEVEL_FINE, "mamaSubscription_deactivate(): %s%s Initiating deactivation of subscription (%p)", userSymbolFormattedImpl, impl);

    /* If any initial request is pending it must be canceled. */
    if (impl->mInitialRequest != NULL &&
        imageRequest_isWaitingForResponse (impl->mInitialRequest))
    {
        imageRequest_stopWaitForResponse (impl->mInitialRequest);
    }

    /* If a recap request is pending it must also be canceled. */
    if (impl->mRecapRequest != NULL &&
        imageRequest_isWaitingForResponse (impl->mRecapRequest))
    {
        imageRequest_stopWaitForResponse (impl->mRecapRequest);
    }

    /* Mute the subscription to prevent any more updates coming in, note that the subscription bridge will be NULL
     * for a snapshot or a dictionary subscription.
     */
    if (impl->mSubscBridge)
    {
        impl->mBridgeImpl->bridgeMamaSubscriptionMute (impl->mSubscBridge);
    }

    /* Write a log message. */
    if (gMamaLogLevel >= MAMA_LOG_LEVEL_FINE)
    {
        mama_log (MAMA_LOG_LEVEL_FINE,
                "deactivateAction(): (%s.%s (%s) (%p)): mSubscBridge=%p",
                  impl->mSubscSource == NULL ? "" : impl->mSubscSource,
                  impl->mSubscSymbol == NULL ? "" : impl->mSubscSymbol,
                  impl->mUserSymbol  == NULL ? "" : impl->mUserSymbol,
                  impl,
                  impl->mSubscBridge);
    }

    /* If this is a market data subscription then it will be listening to the transport as initials and
     * recaps will be issued. Any such requests must now be removed.
     */
    if((impl->mSubscMsgType != MAMA_SUBSC_DDICT_SNAPSHOT) && (impl->mSubscMsgType != MAMA_SUBSC_SNAPSHOT))
    {
        if(NULL != impl->mTransport)
        {
            /* Unregister as a transport listener. */
            mamaTransport_removeListener(impl->mTransport, impl->mSubscHandle);
        }
    }

    /* If this subscription has a bridge, (snapshot subscriptions do not have a bridge as they simply make
     * a point-to-point request for the initial without actually creating the subscription), then it must
     * now be destroyed.
     * The subscription will be cleaned up whenever the mamaSubscriptionImpl_onSubscriptionDestroyed callback
     * is invoked.
     */
    /* Note that the state of the subscription will be changed in the bridge callback. */
    if((NULL != impl->mSubscBridge) && (NULL != impl->mBridgeImpl))
    {

        /* Destroy the bridge subscription. */
        ret = impl->mBridgeImpl->bridgeMamaSubscriptionDestroy(impl->mSubscBridge);
        impl->mSubscBridge = NULL;
        /* Write a log message to maintain backward compatibility, note that the form of the this message is such as to maintain
         * backward compatibility.
         */
        if(ret != MAMA_STATUS_OK)
        {
            mama_log(MAMA_LOG_LEVEL_ERROR, "deactivateAction(): Could not destroy subscription bridge [%s]", mamaStatus_stringForStatus(ret));
        }
    }

    return MAMA_STATUS_OK;
}

void MAMACALLTYPE mamaSubscription_DestroyThroughQueueCB(mamaQueue Queue, void* closure)
{
    mama_status status;

    mamaSubscription psub    = (mamaSubscription) closure;
    mamaSubscriptionImpl* psubi = (mamaSubscriptionImpl*) closure;

    status    = mamaSubscription_destroy(psub);
    if (MAMA_STATUS_OK != status)
    {
        mama_log(MAMA_LOG_LEVEL_ERROR, "mamaSubscription_DestroyThroughQueueCB::Failed to destroy a subscription on %s.", psubi->mSubscSymbol);
        return;
    }
}

mama_status
mamaSubscription_destroyEx(mamaSubscription subscription)
{
    mama_status status;

    status    = mamaQueue_enqueueEvent(self->mQueue, mamaSubscription_DestroyThroughQueueCB , (void*)subscription);
    if (MAMA_STATUS_OK != status)
    {
        mama_log(MAMA_LOG_LEVEL_ERROR, "mamaSubscription_destroyEx::Failed to enqueue the destruction of a subscription on %s.", self->mSubscSymbol);
    }

    return status;
}

int mamaSubscription_isActive(mamaSubscription subscription)
{
    return mamaSubscription_isValid(subscription);
}

int mamaSubscription_isValid(mamaSubscription subscription)
{
    /* Returns. */
    int ret = 0;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl*)subscription;

        /* Write a log to indicate that this function is deprecated. */
        mama_log (MAMA_LOG_LEVEL_FINE, "mamaSubscription_isValid and mamaSubscription_isActive are deprecated and may produce in-accurate results, use mamaSubscription_getState instead.");

        /* The subscription is deemed to be valid if it has been activated. */
        if(MAMA_SUBSCRIPTION_ACTIVATED == wInterlocked_read(&impl->mState))
        {
            ret = 1;
        }
    }

    return ret;
}

void mamaSubscription_setIsThrottled (mamaSubscription  subscription,
                                    int               isThrottled)
{
    mamaSubscriptionImpl* impl = (mamaSubscriptionImpl*)subscription;
    if (impl)
    {
        impl->mIsThrottled = isThrottled;
    }
}

int mamaSubscription_isExpectingUpdates (mamaSubscription subscription)
{
    if (subscription)
    {
        switch (self->mServiceLevel)
        {
        case MAMA_SERVICE_LEVEL_REAL_TIME:
        case MAMA_SERVICE_LEVEL_CONFLATED:
        case MAMA_SERVICE_LEVEL_REPEATING_SNAPSHOT:
        case MAMA_SERVICE_LEVEL_UNKNOWN:
            return 1;
        case MAMA_SERVICE_LEVEL_SNAPSHOT:
            return 0;
        }
    }
    return 0;
}

static mama_status
configureForMultipleTopics (mamaSubscription subscription)
{
    self->mSubjects = wtable_create ("subjects", (self->mGroupSizeHint / 10));
    if (self->mSubjects == NULL)
    {
        return MAMA_STATUS_NOMEM;
    }

    return MAMA_STATUS_OK;
}

mama_status
mamaSubscription_getTransport (mamaSubscription  subscription,
                               mamaTransport*    result)
{
    if (!subscription) return MAMA_STATUS_INVALID_ARG;
    *result = self->mTransport;
    return MAMA_STATUS_OK;
}

mama_status
mamaSubscription_getQueue ( mamaSubscription subscription,
                           mamaQueue*       result)
{
    if (!subscription) return MAMA_STATUS_INVALID_ARG;
    *result = self->mQueue;
    return MAMA_STATUS_OK;
}

mamaMsgCallbacks*
mamaSubscription_getUserCallbacks ( mamaSubscription subscription)
{
    if (!subscription) return NULL;
    return (mamaMsgCallbacks*) &self->mUserCallbacks;
}

mama_status
mamaSubscription_getSource (
    mamaSubscription  subscription,
    const char**      source)
{
    if (!subscription) return MAMA_STATUS_NULL_ARG;
    if (self->mType == MAMA_SUBSC_TYPE_BASIC)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    *source = self->mSubscSource;
    return MAMA_STATUS_OK;
}

mama_status
mamaSubscription_getSymbol (
    mamaSubscription  subscription,
    const char**      symbol)
{
    if (!subscription) return MAMA_STATUS_NULL_ARG;
    *symbol = self->mUserSymbol;
    return MAMA_STATUS_OK;
}

mama_status
mamaSubscription_setSymbol (
    mamaSubscription  subscription,
    const char*       symbol)
{
    if (!subscription) return MAMA_STATUS_NULL_ARG;
    checkFree (&self->mUserSymbol);
    if (!symbol)
    {
        symbol = "";
    }
    self->mUserSymbol = strdup(symbol);
    return MAMA_STATUS_OK;
}

mama_status
mamaSubscription_setSubscriptionType (
    mamaSubscription      subscription,
    mamaSubscriptionType  type)
{
    if (!self) return MAMA_STATUS_NULL_ARG;
    self->mType = type;
    return MAMA_STATUS_OK;
}

mama_status
mamaSubscription_getSubscriptionType ( mamaSubscription       subscription,
                                      mamaSubscriptionType*  type)
{
    if (!self) return MAMA_STATUS_NULL_ARG;
    *type = self->mType;
    return MAMA_STATUS_OK;
}

mama_status
mamaSubscription_setServiceLevel (
        mamaSubscription  subscription,
        mamaServiceLevel  serviceLevel,
        long              serviceLevelOpt)
{
    if (!self) return MAMA_STATUS_NULL_ARG;
    self->mServiceLevel     = serviceLevel;
    self->mServiceLevelOpt  = serviceLevelOpt;
    return MAMA_STATUS_OK;
}

mama_status
mamaSubscription_getServiceLevel (
        mamaSubscription  subscription,
        mamaServiceLevel* serviceLevel)
{
    if (!subscription) return MAMA_STATUS_NULL_ARG;
    *serviceLevel = self->mServiceLevel;
    return MAMA_STATUS_OK;
}

mama_status
mamaSubscription_getServiceLevelOpt (
        mamaSubscription  subscription,
        long*             serviceLevelOpt)
{
    if (!subscription) return MAMA_STATUS_NULL_ARG;
    *serviceLevelOpt = self->mServiceLevelOpt;
    return MAMA_STATUS_OK;
}

mama_status
mamaSubscription_setRequiresInitial (
        mamaSubscription  subscription,
        int               requiresInitial)
{
    if (!subscription) return MAMA_STATUS_NULL_ARG;
    self->mRequiresInitial = requiresInitial;
    return MAMA_STATUS_OK;
}

mama_status
mamaSubscription_getRequiresInitial (
        mamaSubscription  subscription,
        int*              requiresInitial)
{
    if (!subscription) return MAMA_STATUS_NULL_ARG;
    *requiresInitial = self->mRequiresInitial;
    return MAMA_STATUS_OK;
}

mama_status
mamaSubscription_getPlatformError (
            mamaSubscription subscription,
            void**           error)
{
    if (!self) return MAMA_STATUS_NULL_ARG;
    if (!self->mBridgeImpl) return MAMA_STATUS_NO_BRIDGE_IMPL;

    return self->mBridgeImpl->bridgeMamaSubscriptionGetPlatformError
            (self->mSubscBridge, error);
}

mama_status
mamaSubscription_setExpectingInitial (
        mamaSubscription  subscription,
        int               expectingInitial)
{
    if (!subscription) return MAMA_STATUS_NULL_ARG;
    self->mExpectingInitial = expectingInitial;
    return MAMA_STATUS_OK;
}

mama_status
mamaSubscription_getExpectingInitial (
        mamaSubscription  subscription,
        int*              expectingInitial)
{
    if (!subscription) return MAMA_STATUS_NULL_ARG;
    *expectingInitial = self->mExpectingInitial;
    return MAMA_STATUS_OK;
}

mama_status
mamaSubscription_getReceivedInitial (
        mamaSubscription  subscription,
        int*              receivedInitial)
{
    if (!subscription) return MAMA_STATUS_NULL_ARG;
    *receivedInitial = 0;
    if (self->mSubjectContext.mInitialArrived == 1)
    {
        *receivedInitial = 1;
    }
    return MAMA_STATUS_OK;
}

mama_status
mamaSubscription_getClosure (
    mamaSubscription  subscription,
    void**            closure)
{
    if (!self) return MAMA_STATUS_NULL_ARG;
    *closure = self->mClosure;
    return MAMA_STATUS_OK;
}

mama_status
mamaSubscription_getItemClosure (
    mamaSubscription  subscription,
    void**            closure)
{
    if ( subscription == NULL || self->mCurSubjectContext == NULL)
    {
        *closure = NULL;
        return MAMA_STATUS_INVALID_ARG;
    }
    *closure = self->mCurSubjectContext->mClosure;
    return MAMA_STATUS_OK;
}

mama_status
mamaSubscription_getSubscSymbol (
    mamaSubscription  subscription,
    const char**      symbol)
{
    if (!self) return MAMA_STATUS_NULL_ARG;
    *symbol = self->mSubscSymbol;
    return MAMA_STATUS_OK;
}

mama_status
mamaSubscription_setTimeout ( mamaSubscription subscription,
                             double timeout)
{
    if (!self) return MAMA_STATUS_NULL_ARG;
    if ( self->mType == MAMA_SUBSC_TYPE_BASIC)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    self->mTimeout = timeout;
    return MAMA_STATUS_OK;
}

mama_status
mamaSubscription_getTimeout (mamaSubscription subscription, double *val)
{
    if (!self) return MAMA_STATUS_NULL_ARG;
    if ( self->mType == MAMA_SUBSC_TYPE_BASIC)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    *val = self->mTimeout;
    return MAMA_STATUS_OK;
}

mama_status
mamaSubscription_setRecapTimeout ( mamaSubscription subscription,
                                   double timeout)
{
    if (!self) return MAMA_STATUS_NULL_ARG;
    if ( self->mType == MAMA_SUBSC_TYPE_BASIC)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    self->mRecapTimeout = timeout;
    return MAMA_STATUS_OK;
}

mama_status
mamaSubscription_getRecapTimeout (mamaSubscription subscription, double *val)
{
    if (!self) return MAMA_STATUS_NULL_ARG;
    if ( self->mType == MAMA_SUBSC_TYPE_BASIC)
    {
        return MAMA_STATUS_INVALID_ARG;
    }

    /* If mamaSubscription_setRecap hasn't been called, default to mTimeout. */
    if (MAMA_SUBSCRIPTION_DEFAULT_RECAP_TIMEOUT == self->mRecapTimeout)
    {
        self->mRecapTimeout = self->mTimeout;
    }

    *val = self->mRecapTimeout;
    return MAMA_STATUS_OK;
}

mama_status
mamaSubscription_setRetries ( mamaSubscription subscription,
                             int retries)
{
    if (!self) return MAMA_STATUS_NULL_ARG;
    if ( self->mType == MAMA_SUBSC_TYPE_BASIC)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    self->mRetries = retries;
    return MAMA_STATUS_OK;
}

mama_status
mamaSubscription_getRetries ( mamaSubscription subscription, int *val)
{
    if (!self) return MAMA_STATUS_NULL_ARG;
    if ( self->mType == MAMA_SUBSC_TYPE_BASIC)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    *val = self->mRetries;
    return MAMA_STATUS_OK;
}

mama_status
mamaSubscription_setAppDataType (mamaSubscription  subscription,
                                 uint8_t           appDataType)
{
    if (!self) return MAMA_STATUS_NULL_ARG;
    self->mAppDataType = appDataType;
    return MAMA_STATUS_OK;
}

mama_status
mamaSubscription_getAppDataType (mamaSubscription  subscription,
                                 uint8_t*          val)
{
    if (!self) return MAMA_STATUS_NULL_ARG;
    *val = self->mAppDataType;
    return MAMA_STATUS_OK;
}

mama_status
mamaSubscription_setMsgQualifierFilter (mamaSubscription subscription,
                                        int ignoreDefinitelyDuplicate,
                                        int ignorePossiblyDuplicate,
                                        int ignoreDefinitelyDelyaed,
                                        int ignorePossiblyDelayed,
                                        int ignoreOutOfSequence)
{
    if (!self) return MAMA_STATUS_NULL_ARG;

    if (ignoreDefinitelyDuplicate)
    {
        self->mMsgQualFilter |= MAMA_MSG_QUAL_DEFINITELY_DUPLICATE;
    }
    else
    {
        self->mMsgQualFilter &= ~MAMA_MSG_QUAL_DEFINITELY_DUPLICATE;
    }

    if (ignorePossiblyDuplicate)
    {
        self->mMsgQualFilter |= MAMA_MSG_QUAL_POSSIBLY_DUPLICATE;
    }
    else
    {
        self->mMsgQualFilter &= ~MAMA_MSG_QUAL_POSSIBLY_DUPLICATE;
    }

    if (ignoreDefinitelyDelyaed)
    {
        self->mMsgQualFilter |= MAMA_MSG_QUAL_DEFINITELY_DELAYED;
    }
    else
    {
        self->mMsgQualFilter &= ~MAMA_MSG_QUAL_DEFINITELY_DELAYED;
    }

    if (ignorePossiblyDelayed)
    {
        self->mMsgQualFilter |= MAMA_MSG_QUAL_POSSIBLY_DELAYED;
    }
    else
    {
        self->mMsgQualFilter &= ~MAMA_MSG_QUAL_POSSIBLY_DELAYED;
    }

    if (ignoreOutOfSequence)
    {
        self->mMsgQualFilter |= MAMA_MSG_QUAL_OUT_OF_SEQUENCE;
    }
    else
    {
        self->mMsgQualFilter &= ~MAMA_MSG_QUAL_OUT_OF_SEQUENCE;
    }
    return MAMA_STATUS_OK;
}

mama_status
mamaSubscription_getMsgQualifierFilter (mamaSubscription subscription,
                                        int *ignoreDefinitelyDuplicate,
                                        int *ignorePossiblyDuplicate,
                                        int *ignoreDefinitelyDelyaed,
                                        int *ignorePossiblyDelayed,
                                        int *ignoreOutOfSequence)
{
    if (!self) return MAMA_STATUS_NULL_ARG;

    *ignoreDefinitelyDuplicate =
        self->mMsgQualFilter & MAMA_MSG_QUAL_DEFINITELY_DUPLICATE;
    *ignorePossiblyDuplicate =
        self->mMsgQualFilter & MAMA_MSG_QUAL_POSSIBLY_DUPLICATE;
    *ignoreDefinitelyDelyaed =
        self->mMsgQualFilter & MAMA_MSG_QUAL_DEFINITELY_DELAYED;
    *ignorePossiblyDelayed =
        self->mMsgQualFilter & MAMA_MSG_QUAL_POSSIBLY_DELAYED;
    *ignoreOutOfSequence =
        self->mMsgQualFilter & MAMA_MSG_QUAL_OUT_OF_SEQUENCE;

    return MAMA_STATUS_OK;
}


mama_status
mamaSubscription_processErr (mamaSubscription subscription, int deactivate)
{
    const char*        source = NULL;
    const char*        symbol = NULL;
    void*              closure = NULL;

    mamaMsgCallbacks* callbacks = mamaSubscription_getUserCallbacks(subscription);
    if (deactivate)
    {
        mamaSubscription_deactivate (subscription);
    }

    mamaSubscription_getSource   (subscription, &source);
    mamaSubscription_getSymbol   (subscription, &symbol);
    mamaSubscription_getClosure  (subscription, &closure);

    if( gMamaLogLevel >= MAMA_LOG_LEVEL_FINER )
    {
        mama_log (MAMA_LOG_LEVEL_FINER,
                    "mamaSubscription_processErr(): Symbol %s. Subscription deactivated %d", symbol, deactivate);
    }

    /* The error cause should have been set by the calling function */

    callbacks->onError(subscription,
                        MAMA_STATUS_TIMEOUT,
                        (void*)MAMA_MSG_STATUS_OK,
                        symbol,
                        closure);

    return MAMA_STATUS_OK;
}

mama_status
mamaSubscription_processTportMsg( mamaSubscription subscription,
                                  mamaMsg msg,
                                  void* topicClosure)
{
    mamaBridgeImpl* bridge  = NULL;
    int             allowed = 0;

    if ( (gMamaLogLevel >= MAMA_LOG_LEVEL_FINEST) ||
            (mamaSubscription_checkDebugLevel (subscription,
                                               MAMA_LOG_LEVEL_FINEST)))
    {
        const char* text = mamaMsg_toString(msg);
        mama_forceLog (MAMA_LOG_LEVEL_FINEST,
                "mamaSubscription_processMsg(): %s%s msg = %s subsc (%p)",
                userSymbolFormatted,
                text,
                subscription);
    }

    bridge = mamaSubscription_getBridgeImpl(subscription);

    if (!mamaBridgeImpl_areEntitlementsDeferred(bridge)) 
    {
        allowed = self->mSubjectContext.mEntitlementBridge->isAllowed(self->mSubjectContext.mEntitlementSubscription, 
                                                            self->mSubjectContext.mSymbol);
        if (!allowed)
        {
            self->mWcCallbacks.onError (self,
                    MAMA_STATUS_NOT_ENTITLED,
                    NULL,
                    self->mUserSymbol,
                    self->mClosure);
            return MAMA_STATUS_NOT_ENTITLED;
        }
    }

    self->mWcCallbacks.onMsg (
           subscription, 
           msg, 
           NULL,
           self->mClosure, 
           topicClosure);

    /*Do not access subscription here as it mey have been deleted/destroyed*/
    return MAMA_STATUS_OK;
}

mama_status
mamaSubscription_processWildCardMsg( mamaSubscription subscription,
                                     mamaMsg msg,
                                     const char* topic,
                                     void* topicClosure)
{
    mamaBridgeImpl*         bridge      = NULL;
    mamaEntitlementBridge   entBridge   = NULL;
    int                     allowed     = 0;

    if ( (gMamaLogLevel >= MAMA_LOG_LEVEL_FINEST) ||
            (mamaSubscription_checkDebugLevel (subscription,
                                               MAMA_LOG_LEVEL_FINEST)))
    {
        const char* text = mamaMsg_toString(msg);
        mama_forceLog (MAMA_LOG_LEVEL_FINEST,
                "mamaSubscription_processWildCardMsg(): %s%s msg = %s subsc (%p)",
                userSymbolFormatted,
                text,
                subscription);
    }

    bridge = mamaSubscription_getBridgeImpl(subscription);

    if (!mamaBridgeImpl_areEntitlementsDeferred(bridge))
    {
        entBridge = self->mSubjectContext.mEntitlementBridge;

        if (NULL != entBridge)
        {
            allowed = entBridge->isAllowed(self->mSubjectContext.mEntitlementSubscription,
                                                                self->mSubjectContext.mSymbol);
            if (!allowed)
            {
                self->mWcCallbacks.onError (self,
                        MAMA_STATUS_NOT_ENTITLED,
                        NULL,
                        self->mUserSymbol,
                        self->mClosure);
                return MAMA_STATUS_NOT_ENTITLED;
            }
        }
    }

    self->mWcCallbacks.onMsg (
           subscription, 
           msg, 
           topic,
           self->mClosure, 
           topicClosure);

    /* Do not access subscription here as it may have been deleted/destroyed */
    return MAMA_STATUS_OK;
}

mama_status
mamaSubscription_processMsg (mamaSubscription subscription, mamaMsg msg)
{
    const listenerMsgCallback   callback  = self->mCallback;
    mamaBridgeImpl*             bridge    = NULL;
    mamaEntitlementBridge       entBridge = NULL;
    int                         allowed   = 0;
    self->mCurSubjectContext = mamaSubscription_getSubjectContext (self, msg);

    if ( (gMamaLogLevel >= MAMA_LOG_LEVEL_FINEST) ||
            (mamaSubscription_checkDebugLevel (subscription,
                                               MAMA_LOG_LEVEL_FINEST)))
    {
        const char* text = mamaMsg_toString(msg);
        mama_log (MAMA_LOG_LEVEL_FINEST,
                "mamaSubscription_processMsg(): %s%s msg = %s subsc (%p)",
                userSymbolFormatted,
                text,
                subscription);
    }

    if (callback != NULL) /* TYPE is a MARKETDATA type */
    {
        if (self->mMsgQualFilter) /* we are discarding something */
        {
            mama_u16_t filter;
            if (mamaMsg_getU16 (msg, MamaFieldMsgQual.mName,
                        MamaFieldMsgQual.mFid, &filter) == MAMA_STATUS_OK)
            {
                if (self->mMsgQualFilter & filter)
                {
                    /* Ignore it. */
                    return MAMA_STATUS_OK;
                }
            }

        }
        listenerMsgCallback_processMsg (callback, msg,
                self->mCurSubjectContext);
    }
    else
    {
        bridge = mamaSubscription_getBridgeImpl(subscription);

        if (!mamaBridgeImpl_areEntitlementsDeferred(bridge)) 
        {
            entBridge = self->mSubjectContext.mEntitlementBridge;

            if (NULL != entBridge) /* entitlementBridge will not be set for dict subscription, skip check. */
            {
                allowed = entBridge->isAllowed(self->mSubjectContext.mEntitlementSubscription, 
                                                                    self->mSubjectContext.mSymbol);
                if (!allowed)
                {
                    mama_log (MAMA_LOG_LEVEL_FINER, "mamaSubscription_processMsg(): "
                              "Not Entitled code: subsc (%p) %s",
                              subscription, self->mUserSymbol);

                    mama_setLastError (MAMA_ERROR_NOT_ENTITLED);

                    self->mUserCallbacks.onError (self,
                            MAMA_STATUS_NOT_ENTITLED,
                            NULL,
                            self->mUserSymbol,
                            self->mClosure);
                    return MAMA_STATUS_NOT_ENTITLED;
                }
            }
        }

        if (gGenerateQueueStats)
        {
            mamaStatsCollector queueStatsCollector ;
            if (queueStatsCollector = mamaQueueImpl_getStatsCollector (self->mQueue))
                mamaStatsCollector_incrementStat (queueStatsCollector, MamaStatNumMessages.mFid);
        }

        if (gGenerateTransportStats)
        {
            mamaStatsCollector tportStatsCollector ;
            if (tportStatsCollector = mamaTransport_getStatsCollector (self->mTransport))
                 mamaStatsCollector_incrementStat (tportStatsCollector, MamaStatNumMessages.mFid);
        }


        if (mamaInternal_getGlobalStatsCollector() != NULL)
        {
            mamaStatsCollector_incrementStat (mamaInternal_getGlobalStatsCollector(),
                                              MamaStatNumMessages.mFid);
        }

        mamaSubscription_forwardMsg (self, msg);

        /*Do not access subscription here as it mey have been deleted/destroyed*/
    }
    return MAMA_STATUS_OK;
}

mama_status
mamaSubscription_muteCurrentTopic (mamaSubscription subscription)
{
    if ( subscription == NULL)
    {
        return MAMA_STATUS_INVALID_ARG;
    }

    if (self->mWildCardType == wc_transport)
    {
        return self->mBridgeImpl->bridgeMamaSubscriptionMuteCurrentTopic
                (self->mSubscBridge);
    }

    return MAMA_STATUS_NO_BRIDGE_IMPL;
}

mama_status
mamaSubscription_setItemClosure (
    mamaSubscription subscription,
    void*            closure)
{
    if ( subscription == NULL )
    {
        return MAMA_STATUS_INVALID_ARG;
    }

    if (self->mWildCardType == wc_none)
    {
        /* Group subscription */
        if (self->mCurSubjectContext == NULL) return MAMA_STATUS_INVALID_ARG;
        self->mCurSubjectContext->mClosure = closure;
    }
    else
    {
        /* Wild Card */
        return self->mBridgeImpl->bridgeMamaSubscriptionSetTopicClosure
            (self->mSubscBridge, closure);
    }

    return MAMA_STATUS_OK;
}

mamaMsgCallbacks *
mamaSubscription_getWombatMsgCallbacks (mamaSubscription subscription)
{
    return (mamaMsgCallbacks*)&self->mUserCallbacks;
}


void
mamaSubscription_setAcceptMultipleInitials (mamaSubscription subscription,
                                            int              accept)
{
    self->mAcceptMultipleInitials = accept;
}

int
mamaSubscription_getAcceptMultipleInitials (mamaSubscription subscription)
{
    return self->mAcceptMultipleInitials;
}

int
mamaSubscription_getInitialCount (mamaSubscription subscription)
{
    return self->mInitialCount;
}

void
mamaSubscription_incrementInitialCount (mamaSubscription subscription)
{
    self->mInitialCount++;
}

void
mamaSubscription_resetInitialCount (mamaSubscription subscription)
{
    self->mInitialCount=0;
}

mama_status
mamaSubscription_setDebugLevel (
    mamaSubscription  subscription,
    MamaLogLevel      level)
{
    mama_status ret = MAMA_STATUS_NULL_ARG;
    mamaSubscriptionImpl* impl = (mamaSubscriptionImpl*)subscription;
    if (impl)
    {
        impl->mDebugLevel = level;
        ret = MAMA_STATUS_OK;
    }
    return ret;
}

MamaLogLevel
mamaSubscription_getDebugLevel (
    mamaSubscription  subscription)
{
    mamaSubscriptionImpl* impl = (mamaSubscriptionImpl*)subscription;
    return (impl) ? impl->mDebugLevel : MAMA_LOG_LEVEL_WARN;
}

mama_status
mamaSubscription_setPreIntitialCacheSize (
    mamaSubscription  subscription,
    int               cacheSize)
{
    mamaSubscriptionImpl* impl = (mamaSubscriptionImpl*)subscription;
    impl->mPreInitialCacheSize = cacheSize < 0 ? 0 : cacheSize;
    return MAMA_STATUS_OK;
}

mama_status
mamaSubscription_getPreIntitialCacheSize (
    mamaSubscription  subscription,
    int*              result)
{
    mamaSubscriptionImpl* impl = (mamaSubscriptionImpl*)subscription;
    *result = impl->mPreInitialCacheSize;
    return MAMA_STATUS_OK;
}

int
mamaSubscription_requiresSubscribe (mamaSubscription subscription)
{
    if (!self) return 0;
    if (!self->mBridgeImpl) return 0;
    /* Bridge will be NULL for snapshots */
    if (self->mSubscBridge != NULL &&
        self->mBridgeImpl->bridgeMamaSubscriptionIsValid (self->mSubscBridge) &&
        !self->mBridgeImpl->bridgeMamaSubscriptionHasWildcards
                                            (self->mSubscBridge))
    {
        return 1;
    }

    return 0;
}

int
mamaSubscription_checkDebugLevel (
    mamaSubscription  subscription,
    MamaLogLevel      level)
{
    mamaSubscriptionImpl* impl = (mamaSubscriptionImpl*)subscription;
    if (impl && (impl->mDebugLevel >= level))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

mama_status
mamaSubscription_getTransportIndex (
    mamaSubscription subscription,
    int*             tportIndex)
{
    mamaSubscriptionImpl* impl = (mamaSubscriptionImpl*)subscription;
    if (!impl) return MAMA_STATUS_INVALID_ARG;

    *tportIndex = impl->mTransportIndex;
    return MAMA_STATUS_OK;
}

mama_status
mamaSubscription_setTransportIndex (
    mamaSubscription subscription,
    int              tportIndex)
{
    mamaSubscriptionImpl* impl = (mamaSubscriptionImpl*)subscription;
    if (!impl) return MAMA_STATUS_INVALID_ARG;

    impl->mTransportIndex = tportIndex;
    return MAMA_STATUS_OK;
}



static int
isEntitledToSymbol (const char *source, const char*symbol, mamaSubscription subscription, mamaTransport transport)
{
    int                   result = 0;
    char                  subject[WOMBAT_SUBJECT_MAX];
    mamaBridgeImpl*       bridge = NULL;
    mamaEntitlementBridge entBridge = NULL;
    entitlementSubscriptionHandle handle = NULL;

    snprintf (subject, WOMBAT_SUBJECT_MAX, "%s.%s", source, symbol);

    bridge = mamaSubscription_getBridgeImpl(subscription);

    if (mamaBridgeImpl_areEntitlementsDeferred(bridge)) /* deferring entitlements. */
    {
        return 1;
    }

    mamaTransportImpl_getEntitlementBridge(transport, &entBridge);

    if (NULL != self->mSubjectContext.mEntitlementSubscription)
    {
        handle = self->mSubjectContext.mEntitlementSubscription->mImpl;
    }

    result = entBridge->isAllowed(handle, subject);

    return result;
}

char* copyString (const char*  str)
{
    size_t len;
    char* result;

    if (!str)
    {
        str = "";
    }

    /* Windows does not like strdup */
    len = strlen (str) + 1;
    result = (char*)calloc (len, sizeof (char));
    strncpy (result, str, len);
    return result;
}

void checkFree (char**  str)
{
    if (*str)
    {
        free (*str);
        *str = NULL;
    }
}

char* determineMappedSymbol (mamaTransport  transport,
                             const char*    symbol)
{
    mamaSymbolMapFunc mapFunc = mamaTransport_getSymbolMapFunc (transport);
    void*  mapFuncClosure = mamaTransport_getSymbolMapFuncClosure (transport);
    if (!mapFunc)
    {
        return copyString (symbol);
    }
    else
    {
        /* We'd rather not create this temporary buffer, but
         * unfortunately, we currently need to create the mapped
         * symbol on the heap.  Perhaps mSubscSymbol and mUserSymbol
         * could be allocated as array members in the struct?
         */
        char tmpSymbol[256];
        (*mapFunc)(mapFuncClosure, tmpSymbol, symbol, 256);
        return copyString (tmpSymbol);
    }
}

mamaBridgeImpl*
mamaSubscription_getBridgeImpl (mamaSubscription subscription)
{
    if (!self) return NULL;

    return self->mBridgeImpl;
}

subscriptionBridge
mamaSubscription_getSubscriptionBridge (mamaSubscription subscription)
{
    if (!self) return NULL;

    return self->mSubscBridge;
}

wildCardType
mamaSubscription_getWildCardType( mamaSubscription subscription)
{
    return self->mWildCardType;
}

/* *************************************************** */
/* Internal Functions. */
/* *************************************************** */

void mamaSubscriptionImpl_clearTransport(mamaSubscription subscription)
{
    /* Cast the subscription to an impl. */
    mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;
    if(NULL != impl)
    {
        /* Clear the transport object. */
        impl->mTransport = NULL;
    }
}

/* *************************************************** */
/* Public Functions. */
/* *************************************************** */

mama_status mamaSubscription_activate(mamaSubscription subscription)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Lock the mutex. */
        wlock_lock(impl->mCreateDestroyLock);

        /* Activate is not supported for a basic subscription. */
        ret = MAMA_STATUS_SUBSCRIPTION_INVALID_TYPE;
        if(MAMA_SUBSC_TYPE_BASIC != impl->mType)
        {
            /* The next action will depend on the current state of the subscription. */
            switch(wInterlocked_read(&impl->mState))
            {
                    /* Activate the subscription. */
                case MAMA_SUBSCRIPTION_DEACTIVATED:
                case MAMA_SUBSCRIPTION_SETUP:

                    /* Set the state to indicate that the subscription is in the process of being activated. */
                    mamaSubscriptionImpl_setState(impl, MAMA_SUBSCRIPTION_ACTIVATING);

                    /* Activate the subscription. */
                    ret = mamaSubscription_activate_internal(impl);
                    break;

                    /* In the following states the subscription is already active so don't need to do anything. */
                case MAMA_SUBSCRIPTION_ACTIVATED:
                case MAMA_SUBSCRIPTION_ACTIVATING:
                case MAMA_SUBSCRIPTION_REACTIVATING:
                    ret = MAMA_STATUS_OK;
                    break;

                    /* We are currently de-activating, re-activate as soon as this process is complete. */
                case MAMA_SUBSCRIPTION_DEACTIVATING:
                    mamaSubscriptionImpl_setState(impl, MAMA_SUBSCRIPTION_REACTIVATING);
                    ret = MAMA_STATUS_OK;
                    break;

                    /* The following states are invalid, the subscription can only be activated if it is fully set-up. */
                case MAMA_SUBSCRIPTION_ALLOCATED:
                case MAMA_SUBSCRIPTION_DESTROYING:
                case MAMA_SUBSCRIPTION_DESTROYED:
                default:

                    ret = MAMA_STATUS_SUBSCRIPTION_INVALID_STATE;
                    break;
            }
        }

        /* Unlock the mutex. */
        wlock_unlock(impl->mCreateDestroyLock);
    }

    return ret;
}

mama_status mamaSubscription_deactivate(mamaSubscription subscription)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    wombatThrottle throttle = NULL;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Deactivate is not supported for a basic subscription. */
        ret = MAMA_STATUS_SUBSCRIPTION_INVALID_TYPE;
        if(MAMA_SUBSC_TYPE_BASIC != impl->mType)
        {
            if (impl->mTransport)
                throttle = mamaTransportImpl_getThrottle(impl->mTransport,
                        MAMA_THROTTLE_DEFAULT);

            if(NULL != throttle)
            {
               wombatThrottle_lock(throttle);
            }

            wlock_lock(impl->mCreateDestroyLock);

            /* The next action will depend on the current state of the subscription. */
            switch(wInterlocked_read(&impl->mState))
            {
                    /* The subscription is waiting on the throttle. */
                case MAMA_SUBSCRIPTION_ACTIVATING:
                     wombatThrottle_removeAction(throttle, impl->mAction);
                     impl->mAction = NULL;
                     mamaSubscriptionImpl_setState(impl, MAMA_SUBSCRIPTION_DEACTIVATED);
                     ret = MAMA_STATUS_OK;
                     break;

                case MAMA_SUBSCRIPTION_REACTIVATING:
                    mamaSubscriptionImpl_setState(impl, MAMA_SUBSCRIPTION_DEACTIVATING);
                    break;

                    /* Deactivate the subscription. */
                case MAMA_SUBSCRIPTION_ACTIVATED:
                    /* Set the state to indicate that the subscription is in the process of being deactivated. */
                    mamaSubscriptionImpl_setState(impl, MAMA_SUBSCRIPTION_DEACTIVATING);
                    /* Deactivate the subscription, clean-up will be performed on the callback. */
                    ret = mamaSubscription_deactivate_internal(impl);
                    if (impl->mSubscMsgType == MAMA_SUBSC_DDICT_SNAPSHOT ||
                        impl->mSubscMsgType == MAMA_SUBSC_SNAPSHOT)
                    {
                        /* Snapshot subscriptions don't have a bridge and are transitioned to deactivated
                         * immediately after deactivating.
                         * Also, since there is no bridge subscription callback to be called later with
                         * mamaSubscriptionImpl_onSubscriptionDestroyed, the necessary actions for deactivated
                         * state are performed here. Namely the queue object count is decremented.
                         */
                        if(NULL != impl->mQueue)
                        {
                            mamaQueue_decrementObjectCount(&impl->mLockHandle, impl->mQueue);
                        }
                        mamaSubscriptionImpl_setState(impl, MAMA_SUBSCRIPTION_DEACTIVATED);
                    }
                    break;

                case MAMA_SUBSCRIPTION_DEACTIVATING:
                case MAMA_SUBSCRIPTION_DEACTIVATED:
                    ret = MAMA_STATUS_OK;
                    break;

                    /* All other states are invalid. */
                case MAMA_SUBSCRIPTION_ALLOCATED:
                case MAMA_SUBSCRIPTION_SETUP:
                case MAMA_SUBSCRIPTION_DESTROYING:
                case MAMA_SUBSCRIPTION_DESTROYED:
                default:
                    ret = MAMA_STATUS_SUBSCRIPTION_INVALID_STATE;
                    break;
            }

            wlock_unlock(impl->mCreateDestroyLock);

            if(NULL != throttle)
            {
               wombatThrottle_unlock(throttle);
            }
        }
        else
        {
            wlock_lock(impl->mCreateDestroyLock);
            /* The next action will depend on the current state of the subscription. */
            switch(wInterlocked_read(&impl->mState))
            {
                /* Deactivate the subscription. */
                case MAMA_SUBSCRIPTION_ACTIVATED:
                    mamaSubscriptionImpl_setState(impl, MAMA_SUBSCRIPTION_DEACTIVATING);
                    ret = mamaSubscription_deactivate_internal(impl);
                    break;

                case MAMA_SUBSCRIPTION_DEACTIVATING:
                case MAMA_SUBSCRIPTION_DEACTIVATED:
                    ret = MAMA_STATUS_OK;
                    break;

                    /* All other states are invalid. */
                case MAMA_SUBSCRIPTION_ALLOCATED:
                case MAMA_SUBSCRIPTION_SETUP:
                case MAMA_SUBSCRIPTION_DESTROYING:
                case MAMA_SUBSCRIPTION_DESTROYED:
                default:
                    ret = MAMA_STATUS_SUBSCRIPTION_INVALID_STATE;
                    break;
            }

            wlock_unlock(impl->mCreateDestroyLock);
        }
    }

    return ret;
}

mama_status mamaSubscription_deallocate(mamaSubscription subscription)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Lock the mutex. */
        wlock_lock(impl->mCreateDestroyLock);

        /* The next action will depend on the current state of the subscription. */
        switch(wInterlocked_read(&impl->mState))
        {
                /* The subscription is in the process of being destroyed, change the state to deallocating and clean-up
                 * will be performed once the destroy callback comes in from the middleware.
                 */
            case MAMA_SUBSCRIPTION_DESTROYING:
                mamaSubscriptionImpl_setState(impl, MAMA_SUBSCRIPTION_DEALLOCATING);
                ret = MAMA_STATUS_OK;
                break;

                /* The subscription has already been destroyed or has just been allocated, clean-up can now be performed. */
            case MAMA_SUBSCRIPTION_ALLOCATED:
            case MAMA_SUBSCRIPTION_DESTROYED:
                /* Delete the subscription. */
                mamaSubscriptionImpl_deallocate(impl);
                /* Return from here to prevent the mutex being unlocked. */
                return MAMA_STATUS_OK;
                break;

                /* All other states are invalid. */
            default:
                mama_log (MAMA_LOG_LEVEL_WARN,"Subscription %p: Could not deallocate mamaSubscription as it has not been destroyed.", subscription);
                ret = MAMA_STATUS_SUBSCRIPTION_INVALID_STATE;
                break;
        }

        /* Unlock the mutex. */
        wlock_unlock(impl->mCreateDestroyLock);
    }

    return ret;
}

mama_status mamaSubscription_destroy(mamaSubscription subscription)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        mamaSubscription_deactivate(subscription);

         wlock_lock(impl->mCreateDestroyLock);

        /* The next action will depend on the current state of the subscription. */
        switch(wInterlocked_read(&impl->mState))
        {
                /* Otherwise the subscription has been correctly removed from the throttle. */
                /* Fall through to perform the remaining clean-up. */

                /* For the following states the subscription is not active, simply perform clean-up. */
            case MAMA_SUBSCRIPTION_SETUP:
            case MAMA_SUBSCRIPTION_DEACTIVATED:
                 mamaSubscription_cleanup(subscription);
                 mamaSubscriptionImpl_setState(impl, MAMA_SUBSCRIPTION_DESTROYED);
                  wlock_unlock(impl->mCreateDestroyLock);
                 mamaSubscriptionImpl_invokeDestroyedCallback(subscription);
                 return MAMA_STATUS_OK;
                break;


                /* The subscription is currently de-activating, simply change this to destroying to ensure clean-up is performed
                 * whenever the mamaSubscriptionImpl_onSubscriptionDestroyed function is called.
                 */
            case MAMA_SUBSCRIPTION_DEACTIVATING:
                mamaSubscriptionImpl_setState(impl, MAMA_SUBSCRIPTION_DESTROYING);
                ret = MAMA_STATUS_OK;
                break;

                /* The subscription is in the process of being destroyed, just return and the callback will be invoked by the
                 * middleware thread.
                 */
            case MAMA_SUBSCRIPTION_DESTROYING:
            case MAMA_SUBSCRIPTION_DESTROYED:
            case MAMA_SUBSCRIPTION_ALLOCATED:
                ret = MAMA_STATUS_OK;
                break;

                /* All other states are invalid. */
            default:
                ret = MAMA_STATUS_SUBSCRIPTION_INVALID_STATE;
                break;
        }

        wlock_unlock(impl->mCreateDestroyLock);
    }

    return ret;
}

mama_status mamaSubscription_getState(mamaSubscription subscription, mamaSubscriptionState *state)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if((NULL != subscription) && (NULL != state))
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Return the state. */
        *state = wInterlocked_read(&impl->mState);
        ret = MAMA_STATUS_OK;
    }

    return ret;
}

mama_status mamaSubscription_setup(mamaSubscription subscription, mamaQueue queue, const mamaMsgCallbacks *callbacks, mamaSource source, const char *symbol, void *closure)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if((NULL != subscription) && (NULL != source))
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Lock the mutex. */
        wlock_lock(impl->mCreateDestroyLock);

        /* Allocated and destroyed are the only 2 valid states for this function call. */
        ret = MAMA_STATUS_SUBSCRIPTION_INVALID_STATE;
        if((MAMA_SUBSCRIPTION_ALLOCATED == wInterlocked_read(&impl->mState)) || (MAMA_SUBSCRIPTION_DESTROYED == wInterlocked_read(&impl->mState)))
        {
            /* Get the transport from the source object. */
            mamaTransport transport = NULL;
            ret = mamaSource_getTransport(source, &transport);
            if(MAMA_STATUS_OK == ret)
            {
                /* Get the symbol namespace. */
                const char *sourceName = NULL;
                ret = mamaSource_getSymbolNamespace(source, &sourceName);
                if(MAMA_STATUS_OK == ret)
                {
                    /* Perform the basic setup, note that the state will be changed in here. */
                    ret = mamaSubscription_setupBasic(subscription, transport, queue, callbacks, sourceName, symbol, closure);
                }
            }
        }

        /* Unlock the mutex. */
        wlock_unlock(impl->mCreateDestroyLock);
    }

    return ret;
}

mama_status mamaSubscription_setup2(mamaSubscription subscription, mamaTransport transport, mamaQueue queue, const mamaMsgCallbacks *callbacks, const char *sourceName, const char *symbol, void *closure)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if((NULL != subscription) && (NULL != transport) && (NULL != queue))
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Lock the mutex. */
        wlock_lock(impl->mCreateDestroyLock);

        /* Allocated and destroyed are the only 2 valid states for this function call. */
        ret = MAMA_STATUS_SUBSCRIPTION_INVALID_STATE;
        if((MAMA_SUBSCRIPTION_ALLOCATED == wInterlocked_read(&impl->mState)) || (MAMA_SUBSCRIPTION_DESTROYED == wInterlocked_read(&impl->mState)))
        {
            /* Perform the basic setup, note that the state will be changed in here. */
            ret = mamaSubscription_setupBasic(subscription, transport, queue, callbacks, sourceName, symbol, closure);
        }

        /* Unlock the mutex. */
        wlock_unlock(impl->mCreateDestroyLock);
    }

    return ret;
}

const char* mamaSubscription_stringForState(mamaSubscriptionState state)
{
    /* Switch the state and simply return a string representation of the error code. */
    switch(state)
    {
        case MAMA_SUBSCRIPTION_UNKNOWN: return "MAMA_SUBSCRIPTION_UNKNOWN";
        case MAMA_SUBSCRIPTION_ALLOCATED: return "MAMA_SUBSCRIPTION_ALLOCATED";
        case MAMA_SUBSCRIPTION_SETUP: return "MAMA_SUBSCRIPTION_SETUP";
        case MAMA_SUBSCRIPTION_ACTIVATING: return "MAMA_SUBSCRIPTION_ACTIVATING";
        case MAMA_SUBSCRIPTION_ACTIVATED: return "MAMA_SUBSCRIPTION_ACTIVATED";
        case MAMA_SUBSCRIPTION_DEACTIVATING: return "MAMA_SUBSCRIPTION_DEACTIVATING";
        case MAMA_SUBSCRIPTION_DEACTIVATED: return "MAMA_SUBSCRIPTION_DEACTIVATED";
        case MAMA_SUBSCRIPTION_REACTIVATING: return "MAMA_SUBSCRIPTION_REACTIVATING";
        case MAMA_SUBSCRIPTION_DESTROYING: return "MAMA_SUBSCRIPTION_DESTROYING";
        case MAMA_SUBSCRIPTION_DESTROYED: return "MAMA_SUBSCRIPTION_DESTROYED";
        case MAMA_SUBSCRIPTION_DEALLOCATING: return "MAMA_SUBSCRIPTION_DEALLOCATING";
        case MAMA_SUBSCRIPTION_DEALLOCATED: return "MAMA_SUBSCRIPTION_DEALLOCATED";
    }

    return "State not recognised";
}

/* *************************************************** */
/* Internal Functions. */
/* *************************************************** */

mama_status mamaSubscription_cancel(mamaSubscription subscription)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Only continue if the bridge is valid. */
        ret = MAMA_STATUS_NO_BRIDGE_IMPL;
        if(NULL != impl->mBridgeImpl)
        {
            /* Mute the subscription, this means that the onMsg callback will no longer be invoked
             * if anything comes in.
             */
            ret = impl->mBridgeImpl->bridgeMamaSubscriptionMute(impl->mSubscBridge);
        }
    }

    return ret;
}

/* *************************************************** */
/* Private Functions. */
/* *************************************************** */

mama_status mamaSubscriptionImpl_completeBasicInitialisation(mamaSubscription subscription)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;

    /* Write the log message, this must be done before anything else to ensure backward compatibility. */
    mama_log(MAMA_LOG_LEVEL_FINE, "setupBasic(): %s%s subscription (%p)", userSymbolFormatted, subscription);

    /* Ensure the subscription is valid. */
    if(NULL != subscription)
    {
        /* Obtain the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        ret = MAMA_STATUS_SUBSCRIPTION_INVALID_STATE;
        if(MAMA_SUBSCRIPTION_ACTIVATING == wInterlocked_read(&impl->mState))
        {
            /* Fill in the callback structure, note that the destroy callback is a private function as clean-up
             * must be performed internally before the user callback can be invoked.
             */
            mamaMsgCallbacks cb;
            memset(&cb, 0, sizeof(cb));

            /* Fill in the fields. */
            cb.onCreate         = impl->mUserCallbacks.onCreate;
            cb.onError          = impl->mUserCallbacks.onError;
            cb.onMsg            = impl->mUserCallbacks.onMsg;
            cb.onQuality        = impl->mUserCallbacks.onQuality;
            cb.onGap            = impl->mUserCallbacks.onGap;
            cb.onRecapRequest   = impl->mUserCallbacks.onRecapRequest;
            cb.onDestroy        = mamaSubscriptionImpl_onSubscriptionDestroyed;

            /* Delegate to the correct bridge implementation. */
            switch (impl->mWildCardType)
            {
                case wc_none:
                        ret = impl->mBridgeImpl->bridgeMamaSubscriptionCreate(
                            &impl->mSubscBridge,
                            impl->mSubscSymbol,
                            NULL,
                            impl->mTransport,
                            impl->mQueue,
                            cb,
                            impl,
                            impl->mClosure);
                    break;

                case wc_transport:
                case wc_wildcard:
                    ret = impl->mBridgeImpl->bridgeMamaSubscriptionCreateWildCard(
                        &impl->mSubscBridge,
                        impl->mSubscSymbol,
                        NULL,
                        impl->mTransport,
                        impl->mQueue,
                        cb,
                        impl,
                        impl->mClosure);
                    break;
            }

            if(MAMA_STATUS_OK == ret)
            {

                self->mLockHandle = mamaQueue_incrementObjectCount(impl->mQueue, subscription);
                /* The subscription is now active, set this before the onCreate callback. */
                mamaSubscriptionImpl_setState(impl, MAMA_SUBSCRIPTION_ACTIVATED);

            }
        }
    }

    return ret;
}

mama_status mamaSubscriptionImpl_completeMarketDataInitialisation(mamaSubscription subscription)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;

    /* Write the log message, this must be done before anything else to ensure backward compatibility. */
    mama_log (MAMA_LOG_LEVEL_FINE, "mamaSubscription::setup() %s%s setup Marketdata subscription (%p)", userSymbolFormatted, subscription);

    /* Ensure the subscription is valid. */
    if(NULL != subscription)
    {
        /* Obtain the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* It is possible that the subscription has been destroyed while the throttle thread is waiting at the
         * mutex lock statement above. Therefore an additional check on the status must now be made, execution
         * should only continue if the status is activating.
         */

        /* Acquire the mutex. */
        wlock_lock(impl->mCreateDestroyLock);

        self->mAction = NULL;

        ret = MAMA_STATUS_OK;
        if(MAMA_SUBSCRIPTION_ACTIVATING == wInterlocked_read(&impl->mState))
        {
            impl->mEntitleSubject[0] = '\0';

            if (impl->mSubscSource != NULL && strlen (impl->mSubscSource) > 0 &&
                impl->mSubscSymbol != NULL && strlen (impl->mSubscSymbol) > 0)
            {
                snprintf (impl->mEntitleSubject, WOMBAT_SUBJECT_MAX, "%s.%s", impl->mSubscSource, impl->mSubscSymbol);
            }
            else if (impl->mSubscSymbol != NULL && strlen (impl->mSubscSymbol) > 0)
            {
                snprintf (impl->mEntitleSubject, WOMBAT_SUBJECT_MAX, "%s", impl->mSubscSymbol);
            }

            /* Add the subscription to the list of active subscriptions on the transport. This is only done if the
             * subscription is receiving regular updates. The transport uses its list for actions like refreshing.
             */
            if (impl->mSubscMsgType != MAMA_SUBSC_DDICT_SNAPSHOT &&
                impl->mSubscMsgType != MAMA_SUBSC_SNAPSHOT)
            {
                ret = mamaTransport_addSubscription (self->mTransport, self, &self->mSubscHandle);
            }

            if(MAMA_STATUS_OK == ret)
            {
                /* Complete the initialisation of the subscription. */
                mamaSubscription_initialize(impl);

                /* The subscription is now active. */
                mamaSubscriptionImpl_setState(impl, MAMA_SUBSCRIPTION_ACTIVATED);
            }
        }
        else
        {
            mama_log(MAMA_LOG_LEVEL_WARN, "Subscription %p came off throttle in state %s.", impl, mamaSubscription_stringForState(wInterlocked_read(&impl->mState)));
        }
        /* Unlock the mutex. */
        wlock_unlock(impl->mCreateDestroyLock);

        self->mUserCallbacks.onCreate (self, self->mClosure);
    }

    return ret;
}

mama_status mamaSubscriptionImpl_createBasic(
    mamaSubscription        subscription,
    mamaTransport           transport,
    mamaQueue               queue,
    const mamaMsgCallbacks* callbacks,
    const char*             topic,
    void*                   closure)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_INVALID_ARG;
    if((NULL != subscription) && (NULL != transport) && (NULL != topic) && (0 != strlen(topic)))
    {
        /* Return a different error code if the queue has not been supplied. */
        ret = MAMA_STATUS_INVALID_QUEUE;
        if(NULL != queue)
        {
            /* Get the impl. */
            mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

            /* Lock the mutex. */
            wlock_lock(impl->mCreateDestroyLock);

            /* Allocated and destroyed are the only 2 valid states for this function call. */
            ret = MAMA_STATUS_SUBSCRIPTION_INVALID_STATE;
            if((MAMA_SUBSCRIPTION_ALLOCATED == wInterlocked_read(&impl->mState)) || (MAMA_SUBSCRIPTION_DESTROYED == wInterlocked_read(&impl->mState)))
            {
                /* Get the bridge impl from the transport. */
                ret = MAMA_STATUS_NO_BRIDGE_IMPL;
                impl->mBridgeImpl = mamaTransportImpl_getBridgeImpl(transport);

                /* If the bridge isn't valid then write a log message. */
                if(NULL == impl->mBridgeImpl)
                {
                    mama_log (MAMA_LOG_LEVEL_ERROR,"mamaSubscription_setupBasic(): Could not get bridge impl from transport.");
                }
                else
                {
                    /* Initialise the impl. */
                    impl->mClosure              = closure;
                    impl->mQueue                = queue;
                    impl->mRequiresInitial      = 0;
                    impl->mSubscSymbol          = determineMappedSymbol (transport, topic);
                    impl->mTransport            = transport;
                    impl->mType                 = MAMA_SUBSC_TYPE_BASIC;
                    impl->mUserSymbol           = strdup (topic);

                    /* The callback structure will be NULL for wildcard subscriptions. */
                    if(NULL != callbacks)
                    {
                        impl->mUserCallbacks.onCreate       = callbacks->onCreate;
                        impl->mUserCallbacks.onError        = callbacks->onError;
                        impl->mUserCallbacks.onMsg          = callbacks->onMsg;
                        impl->mUserCallbacks.onQuality      = callbacks->onQuality;
                        impl->mUserCallbacks.onGap          = callbacks->onGap;
                        impl->mUserCallbacks.onRecapRequest = callbacks->onRecapRequest;
                        impl->mUserCallbacks.onDestroy      = callbacks->onDestroy;
                    }

                    /* The subscription has now been setup and is awaiting final processing by the throttle. */
                    mamaSubscriptionImpl_setState(impl, MAMA_SUBSCRIPTION_ACTIVATING);

                    /* Unlock the mutex, the lock will be taken again in the setup basic function, it must be released
                     * however before the onCreate callback can be fired in case the subscription is deactivated inside
                     * the callback
                     . */

                    /* The basic subscription is not throttled */
                    ret = mamaSubscriptionImpl_completeBasicInitialisation(impl);
                }
            }
            wlock_unlock(impl->mCreateDestroyLock);

            if (ret == MAMA_STATUS_OK)
                impl->mUserCallbacks.onCreate(impl, impl->mClosure);
        }

    }

    return ret;
}

void mamaSubscriptionImpl_deallocate(mamaSubscriptionImpl *impl)
{
    /* Set the state to be de-allocated to at least show in the log that it has been completely removed. */
    mamaSubscriptionImpl_setState(impl, MAMA_SUBSCRIPTION_DEALLOCATED);

    clearSubscInfo (impl);

    /* Destroy the mutex. */
    wlock_destroy(impl->mCreateDestroyLock);

    /* Destroy the state. */
    wInterlocked_destroy(&impl->mState);

    mamaEntitlementBridge_destroySubscription (impl->mSubjectContext.mEntitlementSubscription);

    /* Free the subscription impl. */
    free(impl);
}

void MAMACALLTYPE mamaSubscriptionImpl_onSubscriptionDestroyed(mamaSubscription subscription, void *closure)
{
    /* Obtain the impl from the subscription object. */
    mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;
    if(NULL != impl)
    {

        if(NULL != impl->mQueue)
            mamaQueue_decrementObjectCount(&impl->mLockHandle, impl->mQueue);

        /* Lock the mutex. */
        wlock_lock(impl->mCreateDestroyLock);


        /* The next action will depend on the current state of the subscription. */
        switch(wInterlocked_read(&impl->mState))
        {
                /* The subscription is being deactivated. */
            case MAMA_SUBSCRIPTION_DEACTIVATING:
                /* Change the state. */
                mamaSubscriptionImpl_setState(impl, MAMA_SUBSCRIPTION_DEACTIVATED);
                break;

                /* The subscription is being deallocated, i.e. mamaSubscription_deallocate has been called
                 * before the destroy callback has come in from the bridge.
                 */
            case MAMA_SUBSCRIPTION_DEALLOCATING :
                 mamaSubscription_cleanup(subscription);
                 wlock_unlock(impl->mCreateDestroyLock);
                 mamaSubscriptionImpl_invokeDestroyedCallback(impl);
                /* Delete the subscription. */
                mamaSubscriptionImpl_deallocate(impl);
                return;
                break;

                /* The subscription is being destroyed. */
            case MAMA_SUBSCRIPTION_DESTROYING :
                 mamaSubscription_cleanup(subscription);
                 mamaSubscriptionImpl_setState(impl, MAMA_SUBSCRIPTION_DESTROYED);
                  wlock_unlock(impl->mCreateDestroyLock);
                 mamaSubscriptionImpl_invokeDestroyedCallback(impl);
                 return;
                break;

                /* The subscription must be de-activated then re-activated. */
            case MAMA_SUBSCRIPTION_REACTIVATING:
                /* Change the state. */
                mamaSubscriptionImpl_setState(impl, MAMA_SUBSCRIPTION_DEACTIVATED);
                /* Re-activate the subscription. */
                mamaSubscription_activate(subscription);
                break;

                /* Otherwise the subscription is in an invalid state. */
            default:
                {
                    /* Log a message. */
                    int state = wInterlocked_read(&impl->mState);
                    mama_log(MAMA_LOG_LEVEL_ERROR, "Subscription %p is at the invalid state %s, (0x%X).", impl, mamaSubscription_stringForState(state), state);
                }
                break;
        }
       /* Unlock the mutex before the callback is invoked. */
        wlock_unlock(impl->mCreateDestroyLock);
    }
}

void mamaSubscriptionImpl_setState(mamaSubscriptionImpl *impl, mamaSubscriptionState state)
{
    /* Set the state using an atomic operation so it will be thread safe. */
    wInterlocked_set(state, &impl->mState);
    if (impl->mStateMachineTrace)
        mama_log(MAMA_LOG_LEVEL_ERROR, "Subscription %p is now at state %s.", impl, mamaSubscription_stringForState(state));
    else
        mama_log(MAMA_LOG_LEVEL_FINEST, "Subscription %p is now at state %s.", impl, mamaSubscription_stringForState(state));
}

void mamaSubscriptionImpl_invokeDestroyedCallback(mamaSubscriptionImpl *impl)
{
    if(NULL != impl->mUserCallbacks.onDestroy)
    {
        (*impl->mUserCallbacks.onDestroy)((mamaSubscription)impl, impl->mClosure);
    }
}

void
mamaSubscription_setLogStats (mamaSubscription  subscription,
                              short             logStats)
{
    mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;
    impl->mLogStats = logStats;
}
short
mamaSubscription_getLogStats (mamaSubscription  subscription)
{
    /* Get the impl. */
    mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;
    return impl->mLogStats;
}

mamaDqContext*
mamaSubscription_getDqContext(mamaSubscription subscription)
{
    mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;
    return &impl->mSubjectContext.mDqContext;
}

mama_status
mamaSubscription_setDqContext(mamaSubscription subscription, mamaDqContext dqContext)
{
    if(subscription != NULL)
    {
        subscription->mSubjectContext.mDqContext = dqContext;
        return MAMA_STATUS_OK;
    }
        return MAMA_STATUS_NULL_ARG;
}

dqStrategy
mamaSubscription_getDqStrategy(mamaSubscription subscription)
{
    if(subscription != NULL)
    {
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;
        return impl->mDqStrategy;
    }
}

mama_status
mamaSubscription_setDqStrategy(mamaSubscription subscription, dqStrategy strategy)
{
    if (subscription != NULL)
    {
        self->mDqStrategy =  strategy;
        return MAMA_STATUS_OK;
    }
        return MAMA_STATUS_NULL_ARG;
}

mama_status
mamaSubscription_setRecapRequest(mamaSubscription subscription, imageRequest request)
{
    if(NULL != subscription && NULL != request)   
    {
       subscription->mRecapRequest = request;
       return MAMA_STATUS_OK;
    }

    return MAMA_STATUS_NULL_ARG;
}
mama_status
mamaSubscription_getRecapRequest(mamaSubscription subscription, imageRequest request)
{
    if(NULL != subscription && NULL != request)   
    {
       request = subscription->mRecapRequest;
       return MAMA_STATUS_OK;
    }

    return MAMA_STATUS_NULL_ARG;
}
