/* $Id: subscription.c,v 1.152.4.15.2.8.2.10 2011/10/10 16:03:18 emmapollock Exp $
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
#include "bridge.h"
#include "dictionaryimpl.h"
#include "dqstrategy.h"
#include "imagerequest.h"
#include "list.h"
#include "listenermsgcallback.h"
#include "mama/mama.h"
#include "mama/msgqualifier.h"
#include "mama/source.h"
#include "mama/statfields.h"
#include "mama/statscollector.h"
#include "mama/subscmsgtype.h"
#include "mamainternal.h"
#include "msgutils.h"
#include "property.h"
#include "publisherimpl.h"
#include "queueimpl.h"
#include "subscriptionimpl.h"
#include "transportimpl.h"
#include "wlock.h"
#include "wombat/wincompat.h"
#include "wombat/wInterlocked.h"
#include "wombat/wtable.h"

#ifdef WITH_ENTITLEMENTS

#include <OeaClient.h>
#include <OeaSubscription.h>
extern oeaClient *   gEntitlementClient;

#endif /* WITH_ENTITLEMENTS */

/* *************************************************** */
/* Definitions. */
/* *************************************************** */

#define WOMBAT_SUBJECT_MAX 256

/* The symbol used for dictionary subscriptions. */
#define MAMA_SUBSCRIPTION_DICTIONARY_SYMBOL "DATA_DICT"

/* The default size of the pre-initial cache. */
#define MAMA_SUBSCRIPTION_DEFAULT_PREINITIALCACHESIZE 10

/* The minimum value for the group size hint. */
#define MAMA_SUBSCRIPTION_MINIMUM_GROUP_SIZE_HINT 100

/* The name of the pre-initial cache size property. */
#define MAMA_SUBSCRIPTION_PROPNAME_PREINITIALCACHESIZE "mama.subscription.preinitialcachesize"

/* The root to use for a dictionary subscription. */
#define MAMA_SUBSCRIPTION_ROOT_DICT   "_MDDD"

/* The root to use for a normal subscription. */
#define MAMA_SUBSCRIPTION_ROOT_NORMAL "_MD"

/* *************************************************** */
/* External Variables. */
/* *************************************************** */

/* The following flags indicate if stats are in use. */
extern int gGenerateGlobalStats;
extern int gGenerateQueueStats;
extern int gGenerateTransportStats;

/* *************************************************** */
/* Structures. */
/* *************************************************** */

/* This structure contains all the fields required to implement the subscription. */
typedef struct mamaSubscriptionImpl_
{
    /* This flag indicates whether the subscription accepts multiple initials, this is typically
     * set for group and wildcard subscriptions.
     */
    int mAcceptMultipleInitials;

    /* Market data subscriptions are created on the throttle, this member holds the
     * throttle action and is needed in case the subscription is deactivated before
     * the create action is processed.
     */
    wombatThrottleAction mAction;

    /* The application specific data type, note that this value is not actually used by the subscription. */
    uint8_t mAppDataType;

    /* The bridge. */
    mamaBridgeImpl *mBridgeImpl;

    /* The listener callback actually handles processing of the messages. */
    listenerMsgCallback mCallback;

    /* The closure provided to the setup / create functions. */
    void *mClosure;

    /* This lock is used to protect access to this structure during creation and destruction. */
    wLock mCreateDestroyLock;

    /* This points to the context for the current symbol when dealing with a subscription
     * where there are multiple symbols, (for example a group or wildcard subscription).
     */
    SubjectContext *mCurSubjectContext;

    /* The debug level is used when writing out log messages. */
    MamaLogLevel mDebugLevel;

    /* The data quality strategy is used for actions such as sequence number checking. */
    dqStrategy mDqStrategy;

    /* Set to 1 if this type of subscription is expecting an initial to be delivered. */
    int mExpectingInitial;

    /* False if it is __MAMA_CM sub */
    short mLogStats;

    /* Group size hint is obtained from the transport when using group subscriptions. */
    int mGroupSizeHint;

    /* Some types of subscritpion such as symbol list subscriptions expect multiple initials,
     * this holds the number that have been delivered.
     */
    int mInitialCount;

    /* This image request is used to obtain initials. */
    imageRequest mInitialRequest;

    /* The queue lock handle. */
    mamaQueueLockHandle mLockHandle;

    /* This is used to filter out messages for example duplicates or out of sequence messages. */
    uint16_t mMsgQualFilter;

    /* The size of the pre-initial cache, this can be configured using the property defined in
     * MAMA_SUBSCRIPTION_PROPNAME_PREINITIALCACHESIZE.
     */
    int mPreInitialCacheSize;

    /* The queue that the subscription event are being processed on. */
    mamaQueue mQueue;

    /* This image request is used to obtain recaps. */
    imageRequest mRecapRequest;

    /* If this flag is set to 1 then the subscription will check sequence numbers and recap
     * requests will be issued whenever gaps are detected.
     */
    int mRecoverGaps;

    /* This flag is set to 1 if the subscription requires an initial, note that basic
     * subscriptions do not use initials.
     */
    int mRequiresInitial;

    /* Indicates whether the next refresh message should be responded to or ignored. Refreshes are
     * ignored if it is this subscription that has generated the refresh.
     */
    int mRespondToNextRefresh;

    /* The number of times the initial or recap image requests will be retried before an error is returned. */
    int mRetries;

    /* The service level, for example snapshot or normal. */
    mamaServiceLevel mServiceLevel;

    /* Custom options associated with the service level. */
    long mServiceLevelOpt;

    /* The current state of the subscription, this should not be written to directly but should
     * instead be set by calling the mamaSubscriptionImpl_setState private function.
     */
    wInterlockedInt mState;

    /* The subject context holds the data quality context as well as the symbol.
     * This is only valid for subscriptions where there is only 1 symbol.
     */
    SubjectContext mSubjectContext;

    /* This table contaisn all of the subscription contexts where there are multiple symbols, (for example
     * in a group subscription.
     */
    wtable_t mSubjects;

    /* The bridge subscription is responsible for actually delivering the messages. */
    subscriptionBridge mSubscBridge;

    /* Used when refresh operations are requested to identify the subscription that
     * sent the refresh.
     */
    void *mSubscHandle;

    /* The type of message used by the subscription, for example snapshot, dictionary etc. */
    mamaSubscMsgType mSubscMsgType;

    /* The publisher is used to send initial and recap requests. */
    mamaPublisher mSubscPublisher;

    /* The subscription root. */
    char *mSubscRoot;

    /* The subscription source. */
    char *mSubscSource;

    /* This is the main symbol for the subscription, note that this is not necessarily the same
     * as the symbol passed into the create function if symbol mapping is being turned on.
     */
    char *mSubscSymbol;

    /* The image request timeout in seconds for initials and recaps. */
    double mTimeout;

    /* The transport that the subscription is using, this is required to obtain the throttle. */
    mamaTransport mTransport;

    /* The index of the transport bridge. */
    int mTransportIndex;

    /* The type of the subscription. */
    mamaSubscriptionType mType;

    /* The structure of callback function pointers, note that wildcard subscriptions use
     * a separate structure - mWcCallbacks.
     */
    mamaMsgCallbacks mUserCallbacks;

    /* This is the original symbol passed into the create function. */
    char *mUserSymbol;

    /* The structure of callback function pointers when using wildcards. */
    mamaWildCardMsgCallbacks mWcCallbacks;

    /* Type specific to a wildcard subscription. */
    wildCardType mWildCardType;

} mamaSubscriptionImpl;

/* *************************************************** */
/* Private Function Prototypes. */
/* *************************************************** */

/**
 * This function performs the work of activating a subscription and should be called after the
  * subscription state has been verified.
  *
  * @param[in] impl The subscription impl.
  *
  * @return mama_status return code can be one of:
  *         MAMA_STATUS_NULL_ARG
  *         MAMA_STATUS_OK
  */
mama_status mamaSubscriptionImpl_activate(mamaSubscriptionImpl *impl);

/**
 * This function will check the user supplied symbol and return a copy of the symbol that
 * will actually be used. Certain types of subscriptions such as the symbol list will
 * replace the user symbol with a predefined value.
 *
 * @param[in] impl The subscription impl.
 * @param[in] symbol The original user symbol.
 *
 * @return The updated symbol delete using free.
 */
char *mamaSubscriptionImpl_checkSymbol(mamaSubscriptionImpl *impl, const char *symbol);

/**
 * This function will perfrom cleanup by destroying many of the objects in the subscription.
 *
 * @param[in] impl The subscription impl.
 *
 */
void mamaSubscriptionImpl_cleanup(mamaSubscriptionImpl *impl);

/**
 * This function is called to complete the setup of a basic subscription and will typically be invoked
 * by the throttle. It is at this stage that the bridge subscrition will be created.
 *
 * @param[in] subscription The subscritpion to process.
 * @return mama_status value.
 */
mama_status mamaSubscriptionImpl_completeBasicInitialisation(mamaSubscription subscription);

/**
 * This function is called to complete the setup of a market data subscription and will typically be invoked
 * by the throttle. It is at this stage that the bridge subscrition will be created.
 *
 * @param[in] subscription The subscritpion to process.
 * @return mama_status value.
 */
mama_status mamaSubscriptionImpl_completeMarketDataInitialisation(mamaSubscription subscription);

/**
 * This function will create a new subscription context for the specified symbol.
 *
 * @param[in] impl The subscription to create the context for.
 * @param[in] symbol The symbol to create the context for.
 * @paramm[out] context To return the context.
 *
 * @return mama_status value can be one of
 *              MAMA_STATUS_NOMEM
 *              MAMA_STATUS_OK
 */
mama_status mamaSubscriptionImpl_createSubjectContext(mamaSubscriptionImpl *impl, const char *symbol, SubjectContext **context);

/**
 * This function is used when the market data subscription is created, the creation occurs on the
 * the throttle and it is this function called when the throttle event is fired.
 *
 * @param[in] subscription Casted pointer to the subscription.
 * @param[in] closure Closure is not used by this function.
 */
void mamaSubscriptionImpl_createThrottleAction(void *subscription, void *closure);

/**
 * This function will perform the actual work of de-activating a subscription.
 *
 * @param[in] impl The subscription to deactivate.
 *
 * @return mama_status return code can be one of
 *          MAAM_STATUS_OK
 */
mama_status mamaSubscriptionImpl_deactivate(mamaSubscriptionImpl *impl);

/**
 * This function will perform final clean-up of the subscription including destroying the mutex and
 * deleting the impl. Note that this function will also set the state of the subscription to be
 * MAMA_SUBSCRIPTION_DEALLOCATED which will result in a finest level log message being written out.
 *
 * @param[in] impl The subscritpion impl to deallocate.
 */
void mamaSubscriptionImpl_deallocate(mamaSubscriptionImpl *impl);

/**
 * If symbol mapping is turned on in the transport this function will return the new symbol
 * from the map function, alternatively it will return a copy of the original symbol.
 *
 * @param[in] transport The transport.
 * @param[in] symbol The original symbol to map.
 *
 * @return the updated symbol.
 */
char *mamaSubscriptionImpl_determineMappedSymbol(mamaTransport transport, const char *symbol);

/**
 * This function is used during cleanup to free individual subscription contexts.
 *
 * @param[in] table The table that contains the contexts.
 * @param[in] data Casted pointer to the context.
 * @param[in] key The key is the symbol name.
 * @param[in] closure The closure is not used.
 */
void mamaSubscriptionImpl_freeSubjectContext(wtable_t table, void *data, const char* key, void *closure);

/**
 * This function will return the context for the symbol contained in a supplied message. If a context
 * does not exist then a new one will be created. Note that there will be only 1 context for a normal
 * market data subscription, multiple contexts are only used in group or wildcard subscriptions.
 *
 * @param[in] impl The subscription to get the context for.
 * @param[in] msg The message to get the symbol from.
 *
 * @return the context.
 */
SubjectContext *mamaSubscriptionImpl_getSubjectContext(mamaSubscriptionImpl *impl, const mamaMsg msg);

/**
 * This function will complete the initialisation of a market data subscription including creating
 * the bridge subscription.
 *
 * @param[in] impl The subscription impl.
 *
 */
void mamaSubscriptionImpl_initialize(mamaSubscriptionImpl *impl);

/**
 * This function will initially set the group size hint, note that the minimum allowed value is defined
 * in MAMA_SUBSCRIPTION_MINIMUM_GROUP_SIZE_HINT.
 *
 * @param[in] impl The subscription impl.
 */
void mamaSubscriptionImpl_initializeGroupSizeHint(mamaSubscriptionImpl *impl);

/**
 * This function will log the contents of the supplied mamaMsg and is used while processing
 * messages from the queue.
 *
 * @param[in] impl The subscription impl.
 * @param[in] message The message being processed.
 */
void mamaSubscriptionImpl_logProcessMessage(mamaSubscriptionImpl *impl, mamaMsg message);

/**
 * This function is used to destroy a subscription from the wrong thread. An event will be enqueued on the
 * subscription queue resulting in this callback being invoked. It will then go ahead and destroy the
 * subscription on the right thread.
 *
 * @param[in] queue The queue the subscription is being processed on.
 * @param[in] closure Casted mamaSubscription.
 */
void MAMACALLTYPE mamaSubscriptionImpl_onDestroyExEvent(mamaQueue queue, void *closure);

/**
 * This function is called whenever a refresh message has been successfully sent, it will simply destroy the
 * message as it is no longer needed.
 *
 * @param[in] publisher The publisher sending the message.
 * @param[in] message The refresh message.
 * @param[in] status Status code if anything went wrong.
 * @param[in] closure Closure is not used.
 */
void MAMACALLTYPE mamaSubscriptionImpl_onRefreshMessageSent(mamaPublisher publisher, mamaMsg message, mama_status status, void *closure);

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
 * This function sets the root, source and symbol information in the subscription. If these members are already valid then
 * they will be deleted and re-allocated. Note that the root will be determined by this function.
 *
 * @param[in] impl The subscription impl.
 * @param[in] source The new source.
 * @param[in] symbol The new symbol.
 *
 * @return mama_status value can be one of
 *          MAMA_STATUS_NOMEM
 *          MAMA_STATUS_OK
 */
mama_status mamaSubscriptionImpl_saveSubscriptionInfo(mamaSubscriptionImpl *impl, const char *source, const char *symbol);

/**
 * This function will atomically set the state of the subscription and write a corresponding finest level
 * log message.
 *
 * @param[in] impl The subscritpion impl to change the state of.
 * @param[in] state The new state.
 */
void mamaSubscriptionImpl_setState(mamaSubscriptionImpl *impl, mamaSubscriptionState state);

/**
 * This function will mark the subscription as being destroyed including writing the appropriate log messages
 * and invoking the user callback function. It is assumed that the lock is held when this function is called.
 *
 * @param[in] impl The subscritpion impl to change the state of.
 * @param[in] closure The closure will be passed back up to the onDestroy callback function.
 */
void mamaSubscriptionImpl_setSubscriptionDestroyed(mamaSubscriptionImpl *impl, void *closure);

/**
 * This function will check the service level and set the appropriate flags within the subscription.
 *
 * @param[in] impl The subscription impl.
 *
 * @return mama_status value can be one of
 *          MAMA_STATUS_INVALID_ARG
 *          MAMA_STATUS_OK
 */
mama_status mamaSubscriptionImpl_verifyServiceLevel(mamaSubscriptionImpl *impl);

/* *************************************************** */
/* Internal Functions. */
/* *************************************************** */

static int
isEntitledToSymbol (const char *source, const char*symbol, mamaSubscription subscription)
{
#ifdef WITH_ENTITLEMENTS
    int result = 0;
    char subject[WOMBAT_SUBJECT_MAX];
    mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

    snprintf (subject, WOMBAT_SUBJECT_MAX, "%s.%s", source, symbol);

    if (gEntitlementClient == 0) /* Not enforcing entitlements. */
    {
        return 1;
    }

    oeaSubscription_setSubject (impl->mSubjectContext.mOeaSubscription, subject);
    result = oeaSubscription_isAllowed (impl->mSubjectContext.mOeaSubscription);

    return result;
#else
    return 1;
#endif /* WITH_ENTITLEMENTS */
}


char* mamaSubscriptionImpl_copyString (const char*  str)
{
    /* Windows does not like strdup */
    size_t len = strlen (str) + 1;
    char* result = (char*)calloc (len, sizeof (char));
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

mama_status
setSubscInfo (
    mamaSubscription  subscription,
    mamaTransport     transport,
    const char*       root,
    const char*       source,
    const char*       symbol)
{
    mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;
    if (!impl) return MAMA_STATUS_NULL_ARG;

    if (root != NULL)
    {
        checkFree (&impl->mSubscRoot);
        impl->mSubscRoot = mamaSubscriptionImpl_copyString (root);
    }

    if (source != NULL)
    {
        checkFree (&impl->mSubscSource);
        impl->mSubscSource = mamaSubscriptionImpl_copyString (source);
    }

    /*Also check for empty string - tibrv subject is badly formed
     unless the symbol is NULL, better to check here than on every
     send.*/
    if ( (symbol != NULL) && (strcmp (symbol,"")!=0))
    {
        checkFree (&impl->mUserSymbol);
        checkFree (&impl->mSubscSymbol);
        impl->mUserSymbol  = mamaSubscriptionImpl_copyString (symbol);
        impl->mSubscSymbol = mamaSubscriptionImpl_determineMappedSymbol (transport, symbol);
    }
    return MAMA_STATUS_OK;
}

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

void mamaSubscription_checkSeqNum(mamaSubscription subscription, mamaMsg msg, int msgType, SubjectContext *ctx)
{
    /* Get the imp. */
    mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

    /* Only continue if we are checking sequence numbers. Note also that
     * a message may arrive before the subscription has been fully initialised
     * in which case the DQ strategy may still be NULL.
     */
    if((0 != impl->mRecoverGaps) && (NULL != impl->mDqStrategy))
    {
        /* Pass the call onto the DQ strategy. */
        dqStrategy_checkSeqNum(impl->mDqStrategy, msg, msgType, &ctx->mDqContext);
    }
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

void mamaSubscription_forwardMsg(mamaSubscription subscription, const mamaMsg msg)
{
    /* Cast the subscription to an impl. */
    mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;
    if(NULL != impl)
    {
        /* Invoke the message callback. */
        impl->mUserCallbacks.onMsg(
            subscription,
            msg,
            impl->mClosure,
            impl->mCurSubjectContext == NULL ? NULL : impl->mCurSubjectContext->mClosure);
    }
}

int mamaSubscription_getAcceptMultipleInitials(mamaSubscription subscription)
{
    /* Get the impl. */
    mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

    /* Return the value. */
    return impl->mAcceptMultipleInitials;
}

void mamaSubscription_getAdvisoryCauseAndPlatformInfo(mamaSubscription subscription, short *cause, const  void **platformInfo)
{
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Simply pass the call onto the transport. */
        mamaTransportImpl_getAdvisoryCauseAndPlatformInfo(impl->mTransport, cause, platformInfo);
    }
}

mamaBridgeImpl *mamaSubscription_getBridgeImpl(mamaSubscription subscription)
{
    /* Returns. */
    mamaBridgeImpl *ret = NULL;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Return the bridge. */
        ret = impl->mBridgeImpl;
    }

    return ret;
}

int mamaSubscription_getInitialCount(mamaSubscription subscription)
{
    /* Get the impl. */
    mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

    /* Return the value. */
    return impl->mInitialCount;
}

mama_status mamaSubscription_getQuality(mamaSubscription subscription, mamaQuality *quality)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if((NULL != subscription) && (NULL != quality))
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Get the data quality state from the context. */
        dqState state = DQ_STATE_NOT_ESTABLISHED;
        ret = dqStrategy_getDqState(impl->mSubjectContext.mDqContext, &state);
        if(MAMA_STATUS_OK == ret)
        {
            /* Convert the state into a quality value. */
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
                    *quality = MAMA_QUALITY_UNKNOWN;
                    break;
            }
        }
    }

	return ret;
}

const char *mamaSubscription_getSubscRoot(mamaSubscription subscription)
{
    /* Returns. */
    const char *ret = NULL;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Return a pointer to the root string. */
        ret = impl->mSubscRoot;
    }

    return ret;
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


wildCardType mamaSubscription_getWildCardType(mamaSubscription subscription)
{
    /* Get the impl. */
    mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

    /* Return the wildcard type. */
    return impl->mWildCardType;
}

int mamaSubscription_hasWildcards(mamaSubscription subscription)
{
    /* Returns. */
    int ret = 0;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Only continue if the bridge is valid. */
        if((NULL != impl->mBridgeImpl) && (NULL != impl->mSubscBridge))
        {
            /* Pass the call to the bridge. */
            ret = impl->mBridgeImpl->bridgeMamaSubscriptionHasWildcards(impl->mSubscBridge);
        }
    }

    return ret;
}

void mamaSubscription_incrementInitialCount(mamaSubscription subscription)
{
    /* Get the impl. */
    mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

    /* Increment the initial count. */
    impl->mInitialCount++;
}

int mamaSubscription_isExpectingUpdates (mamaSubscription subscription)
{
    /* Returns. */
    int ret = 0;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* The service level will indicate whether or not updates are expected. */
        switch(impl->mServiceLevel)
        {
                /* All these types expect updates. */
            case MAMA_SERVICE_LEVEL_REAL_TIME:
            case MAMA_SERVICE_LEVEL_CONFLATED:
            case MAMA_SERVICE_LEVEL_REPEATING_SNAPSHOT:
            case MAMA_SERVICE_LEVEL_UNKNOWN:
                ret = 1;
                break;

	    default:
		ret = 0;
		break;
        }
    }

    return ret;
}

int mamaSubscription_isTportDisconnected(mamaSubscription subscription)
{
    /* Returns. */
    int ret = 0;

    /* Get the impl. */
    mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;
    if((NULL != impl) && (NULL != impl->mBridgeImpl) && (NULL != impl->mSubscBridge))
    {
        /* Pass the call to the bridge. */
        ret = impl->mBridgeImpl->bridgeMamaSubscriptionIsTportDisconnected(impl->mSubscBridge);
    }

    return ret;
}

mama_status mamaSubscription_processErr(mamaSubscription subscription, int deactivate)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_OK;

    /* Get the impl. */
    mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

    /* If specified deactivate the subscription at this point. */
    if(deactivate)
    {
        /* First prevent any more updates coming in. */
        ret = mamaSubscription_cancel(subscription);
        if(MAMA_STATUS_OK == ret)
        {
            /* The deactivate the subscription. */
            ret = mamaSubscriptionImpl_deactivate(impl);
        }
    }

    /* Write a log at finer level. */
    if(gMamaLogLevel >= MAMA_LOG_LEVEL_FINER )
    {
        mama_log (MAMA_LOG_LEVEL_FINER, "mamaSubscription_processErr(): Symbol %s. Subscription deactivated %d", impl->mUserSymbol, deactivate);
    }

    /* Invoke the error callback, note that the error cause should have been set
     * by the calling function.
     */
    impl->mUserCallbacks.onError(
        subscription,
        MAMA_STATUS_TIMEOUT,
        MAMA_MSG_STATUS_OK,
        impl->mUserSymbol,
        impl->mClosure);

    /* Do not access the subscription here as it may have been deleted or destroyed in the callback. */

    return ret;
}

mama_status mamaSubscription_processMsg(mamaSubscription subscription, mamaMsg message)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_OK;

    /* Get the impl. */
    mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

    /* Get the subscription context for the supplied message, note that there may be multiple
     * symbols in use if this is a group or symbol list subscription.
     * This will be saved inside the subscription so that functions such as getItemClosure can
     * be called during the message callback.
     */
    impl->mCurSubjectContext = mamaSubscriptionImpl_getSubjectContext(impl, message);

    /* Log this event if appropriate, note that this if statement is inlined into the function
     * to increase speed as this is being executed on the message path.
     */
    if((gMamaLogLevel >= MAMA_LOG_LEVEL_FINEST) || (impl->mDebugLevel >= MAMA_LOG_LEVEL_FINEST))
    {
        mamaSubscriptionImpl_logProcessMessage(impl, message);
    }

    /* This will be a market data subscription if the listener message callback is valid. */
    if(NULL != impl->mCallback)
    {
        /* Check to see if we are discarding something. */
        if(0 != impl->mMsgQualFilter)
        {
            /* Get the qualifier from the message. */
            mama_u16_t filter = 0;
            mama_status mmg = mamaMsg_getU16(message, MamaFieldMsgQual.mName, MamaFieldMsgQual.mFid, &filter);
            if(MAMA_STATUS_OK == mmg)
            {
                if(impl->mMsgQualFilter & filter)
                {
                    /* Quit out here, alternatively the message will be processed as normal via the
                     * listenerMsgCallback below.
                     */
                    return ret;
                }
            }
        }

        /* Process the message with the listener. */
        listenerMsgCallback_processMsg(impl->mCallback, message, impl->mCurSubjectContext);
    }

    /* Otherwise simply forward the message. */
    else
    {
    	int32_t entitleCode = 0;

#ifdef WITH_ENTITLEMENTS
        mamaMsg_getEntitleCode (message, &entitleCode);
#endif
        if (entitleCode == 0)
        {
        	mamaSubscription_forwardMsg(impl, message);
        }
        else
        {
        	mama_log (MAMA_LOG_LEVEL_FINER, "mamaSubscription_processMsg(): "
                      "Not Entitled code: %d subsc (%p) %s",
                      entitleCode, subscription, impl->mUserSymbol);

            mama_setLastError (MAMA_ERROR_NOT_ENTITLED);

            impl->mUserCallbacks.onError (impl,
                    MAMA_STATUS_NOT_ENTITLED,
                    NULL,
                    impl->mUserSymbol,
                    impl->mClosure);
        }
    }

    /* Do not access the subscription here as it may have been deleted or destroyed in the callback. */

    return ret;
}

mama_status mamaSubscription_processTportMsg(mamaSubscription subscription, mamaMsg msg, void *topicClosure)
{
    int32_t entitleCode = 0;
    /* Get the impl. */
    mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

    /* Log this event if appropriate, note that this if statement is inlined into the function
     * to increase speed as this is being executed on the message path.
     */
    if((gMamaLogLevel >= MAMA_LOG_LEVEL_FINEST) || (impl->mDebugLevel >= MAMA_LOG_LEVEL_FINEST))
    {
        mamaSubscriptionImpl_logProcessMessage(impl, msg);
    }

#ifdef WITH_ENTITLEMENTS
    mamaMsg_getEntitleCode (msg, &entitleCode);
#endif
    if (entitleCode == 0)
    {
        impl->mWcCallbacks.onMsg (
                subscription,
                msg,
                NULL,
                impl->mClosure,
                topicClosure);
    }
    else
    {
        mama_log (MAMA_LOG_LEVEL_FINER, "mamaSubscription_processMsg(): "
                "Not Entitled code: %d subsc (%p) %s",
                entitleCode, subscription, impl->mUserSymbol);

        mama_setLastError (MAMA_ERROR_NOT_ENTITLED);

        impl->mWcCallbacks.onError (impl,
                MAMA_STATUS_NOT_ENTITLED,
                NULL,
                impl->mUserSymbol,
                impl->mClosure);
    }

    /*Do not access subscription here as it mey have been deleted/destroyed*/
    return MAMA_STATUS_OK;
}

mama_status mamaSubscription_processWildCardMsg(mamaSubscription subscription, mamaMsg msg, const char *topic, void *topicClosure)
{
	int32_t entitleCode = 0;
    /* Get the impl. */
    mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

    /* Log this event if appropriate, note that this if statement is inlined into the function
     * to increase speed as this is being executed on the message path.
     */
    if((gMamaLogLevel >= MAMA_LOG_LEVEL_FINEST) || (impl->mDebugLevel >= MAMA_LOG_LEVEL_FINEST))
    {
        mamaSubscriptionImpl_logProcessMessage(impl, msg);
    }

#ifdef WITH_ENTITLEMENTS
    mamaMsg_getEntitleCode (msg, &entitleCode);
#endif
    if (entitleCode == 0)
    {
        impl->mWcCallbacks.onMsg (
                subscription,
                msg,
                topic,
                impl->mClosure,
                topicClosure);
    }
    else
    {
        mama_log (MAMA_LOG_LEVEL_FINER, "mamaSubscription_processMsg(): "
                "Not Entitled code: %d subsc (%p) %s",
                entitleCode, subscription, impl->mUserSymbol);

        mama_setLastError (MAMA_ERROR_NOT_ENTITLED);

        impl->mWcCallbacks.onError (impl,
                MAMA_STATUS_NOT_ENTITLED,
                NULL,
                impl->mUserSymbol,
                impl->mClosure);
    }

    /*Do not access subscription here as it mey have been deleted/destroyed*/
    return MAMA_STATUS_OK;
}

mama_status mamaSubscription_requestRecap(mamaSubscription subscription)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NOT_IMPLEMENTED;

    /* Get the impl. */
    mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

    /* This function will not work for group subscriptions, symbols must be recapped individually. */
    if(MAMA_SUBSC_TYPE_GROUP != impl->mType)
    {
        /* Send the recap request. */
        ret = dqStrategy_sendRecapRequest(impl->mDqStrategy, NULL, &impl->mSubjectContext.mDqContext);
    }

    return ret;
}

int mamaSubscription_requiresSubscribe(mamaSubscription subscription)
{
    /* Returns. */
    int ret = 0;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Only continue if the bridge is valid. */
        if((NULL != impl->mBridgeImpl) && (NULL != impl->mSubscBridge))
        {
            /* Delegate to the bridge. */
            if((impl->mBridgeImpl->bridgeMamaSubscriptionIsValid(impl->mSubscBridge))
                &&
              (!impl->mBridgeImpl->bridgeMamaSubscriptionHasWildcards(impl->mSubscBridge)))
            {
                ret = 1;
            }
        }
    }

    return ret;
}

void mamaSubscription_resetInitialCount(mamaSubscription subscription)
{
    /* Get the impl. */
    mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

    /* Rest the initial count. */
    impl->mInitialCount = 0;
}

void mamaSubscription_respondToRefreshMessage(mamaSubscriptionImpl *impl)
{
    /* We may be ignoring refresh messages if we are the one's sending it. */
    if(impl->mRespondToNextRefresh == 1)
    {
        mamaTransportImpl_resetRefreshForListener(impl->mTransport, impl->mSubscHandle);
    }

    else
    {
        /* Set the flag back to 1 so that the next refresh is picked up. */
        impl->mRespondToNextRefresh = 1;
    }
}

void mamaSubscription_sendRefresh(mamaSubscription subscription, const mamaMsg message)
{
    /* Get the impl. */
    mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

    /* Only continue if the bridge if valid. */
    if((NULL != impl) && (NULL != impl->mBridgeImpl))
    {
        /* Don't send a refresh message for wildcard subscriptions. */
        if(mamaSubscription_hasWildcards(subscription) == 0)
        {
            /* Write a log message. */
            mama_log(
                MAMA_LOG_LEVEL_FINEST,
                "mamaSubscription_sendRefresh(): Sending refresh to: %s (%s)",
                (impl->mSubscSource == NULL) ? "" : impl->mSubscSource,
                (impl->mSubscSymbol == NULL) ? "" : impl->mSubscSymbol);

            /* As we're sending the refresh message we don't want to respond to it, therefore
             * ignore the next refresh that is processed.
             */
            impl->mRespondToNextRefresh = 0;

            /* Publish the refresh message, note that the message will be destroy in the
             * callback.
             */
            mamaPublisher_sendWithThrottle(impl->mSubscPublisher, message, mamaSubscriptionImpl_onRefreshMessageSent, NULL);
        }
    }

    /* Alternatively write a log message. */
    else
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaSubscription_sendRefresh(): Could not send refresh. NULL sub or bridge.");
    }
}

mama_status mamaSubscription_setMsgQualifierFilter(mamaSubscription subscription, int ignoreDefinitelyDuplicate, int ignorePossiblyDuplicate, int ignoreDefinitelyDelyaed, int ignorePossiblyDelayed, int ignoreOutOfSequence)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Set the filter based on the flags passed in. */
        if (ignoreDefinitelyDuplicate)
        {
            impl->mMsgQualFilter |= MAMA_MSG_QUAL_DEFINITELY_DUPLICATE;
        }
        else
        {
            impl->mMsgQualFilter &= ~MAMA_MSG_QUAL_DEFINITELY_DUPLICATE;
        }

        if (ignorePossiblyDuplicate)
        {
            impl->mMsgQualFilter |= MAMA_MSG_QUAL_POSSIBLY_DUPLICATE;
        }
        else
        {
            impl->mMsgQualFilter &= ~MAMA_MSG_QUAL_POSSIBLY_DUPLICATE;
        }

        if (ignoreDefinitelyDelyaed)
        {
            impl->mMsgQualFilter |= MAMA_MSG_QUAL_DEFINITELY_DELAYED;
        }
        else
        {
            impl->mMsgQualFilter &= ~MAMA_MSG_QUAL_DEFINITELY_DELAYED;
        }

        if (ignorePossiblyDelayed)
        {
            impl->mMsgQualFilter |= MAMA_MSG_QUAL_POSSIBLY_DELAYED;
        }
        else
        {
            impl->mMsgQualFilter &= ~MAMA_MSG_QUAL_POSSIBLY_DELAYED;
        }

        if (ignoreOutOfSequence)
        {
            impl->mMsgQualFilter |= MAMA_MSG_QUAL_OUT_OF_SEQUENCE;
        }
        else
        {
            impl->mMsgQualFilter &= ~MAMA_MSG_QUAL_OUT_OF_SEQUENCE;
        }

        /* Function succeeded. */
        ret = MAMA_STATUS_OK;
    }

    return ret;
}

mama_status mamaSubscription_setPossiblyStale(mamaSubscription subscription)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Get the current data quality state. */
        dqState state = DQ_STATE_OK;
        ret = dqStrategy_getDqState(impl->mSubjectContext.mDqContext, &state);
        if((MAMA_STATUS_OK == ret) && (DQ_STATE_OK == state))
        {
            /* Invoke the quality callback as the state is now changing to possibly stale. */
            if(NULL != impl->mUserCallbacks.onQuality)
            {
                /* Get information from the transport on what caused this. */
                short cause = 0;
                const void *platformInfo = NULL;
                mamaTransportImpl_getAdvisoryCauseAndPlatformInfo(impl->mTransport, &cause, &platformInfo);

                /* Invoke the callback. */
                impl->mUserCallbacks.onQuality(subscription, MAMA_QUALITY_MAYBE_STALE, impl->mSubscSymbol, cause, platformInfo, impl->mClosure);
            }

            /* Change the quality in the DQ strategy. */
            ret = dqStrategy_setPossiblyStale(&impl->mSubjectContext.mDqContext);
        }
    }

    return ret;
}

void mamaSubscription_stopWaitForResponse(mamaSubscription subscription, SubjectContext *ctx)
{
    /* Get the impl. */
    mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

    /* Stop waiting for initial and recap requests. */
    imageRequest_stopWaitForResponse(impl->mInitialRequest);
    imageRequest_stopWaitForResponse(ctx->mDqContext.mRecapRequest);
}

void mamaSubscription_unsetAllPossiblyStale(mamaSubscription subscription)
{
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Pass the call onto the transport. */
        mamaTransportImpl_unsetAllPossiblyStale(impl->mTransport);
    }
}

/* *************************************************** */
/* Private Functions. */
/* *************************************************** */

mama_status mamaSubscriptionImpl_activate(mamaSubscriptionImpl *impl)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_INVALID_ARG;
    if((NULL != impl) && (NULL != impl->mTransport))
    {
        /* We need to reset the context for each reuse of a subscription. */
        ret = dqContext_initializeContext(&impl->mSubjectContext.mDqContext, impl->mPreInitialCacheSize, impl->mRecapRequest);
        if(MAMA_STATUS_OK == ret)
        {
            /* The subscription will be created on the throttle, get it from the transport. */
            wombatThrottle throttle = mamaTransportImpl_getThrottle(impl->mTransport, MAMA_THROTTLE_DEFAULT);

            /* If the rate is 0 there is no throttle and the create action will be called directly, note that
             * in this case the member throttle action will not be set and the whole operation will be carried
             * out under the subscription lock.
             */
            double throttleRate = wombatThrottle_getRate(throttle);
            if(0.0 == throttleRate)
            {
                ret = mamaSubscriptionImpl_completeMarketDataInitialisation(impl);
            }

            /* Otherwise schedule a throttle action, the lock will be released by this function and will
             * be obtained again in the action callback.
             */
            else
            {
                /* The subscription actually gets created on the throttle queue, schedule a creation event. */
                ret = mamaTransport_throttleAction (
                        impl->mTransport,
                        MAMA_THROTTLE_DEFAULT,
                        mamaSubscriptionImpl_createThrottleAction,
                        impl,
                        impl,
                        NULL,
                        0,
                        &impl->mAction);
                if(MAMA_STATUS_OK != ret)
                {
                    /* Write a log message. */
                    mama_log (MAMA_LOG_LEVEL_ERROR, "mamaSubscription_activate(): Failed to throttle activate. [%s]", mamaStatus_stringForStatus(ret));
                }
            }
        }

        else
        {
            /* Write a log message. */
            mama_log(MAMA_LOG_LEVEL_ERROR, "mamaSubscription_activate(): Failed to initialize DQ context for subscription [%s]", mamaStatus_stringForStatus(ret));
        }
    }

    return ret;
}

char *mamaSubscriptionImpl_checkSymbol(mamaSubscriptionImpl *impl, const char *symbol)
{
    /* Returns. */
    char *ret = NULL;

    /* Check the type. */
    switch(impl->mType)
    {
        case MAMA_SUBSC_TYPE_SYMBOL_LIST:
        case MAMA_SUBSC_TYPE_SYMBOL_LIST_NORMAL:
            ret = strdup("SYMBOL_LIST_NORMAL");
            break;

        case MAMA_SUBSC_TYPE_SYMBOL_LIST_GROUP:
            ret = strdup("SYMBOL_LIST_GROUP");
            break;

        case MAMA_SUBSC_TYPE_SYMBOL_LIST_BOOK:
            ret = strdup("SYMBOL_LIST_BOOK");
            break;

            /* Otherwise make a copy of the original symbol. */
        default:
            ret = strdup(symbol);
            break;
    }

    return ret;

}

void mamaSubscriptionImpl_cleanup(mamaSubscriptionImpl *impl)
{
    /* This function will attempt to destroy as much as possible and will ignore any error returns. */

    /* Clear the transport pointer before doing any clean-up. This is to ensure that the destroy
     * functions for the image request and the strategy won't try and remove pending requests from
     * the transport. This has already been done during de-activation.
     */
    impl->mTransport = NULL;

    /* Destroy the data quality strategy. */
    if(NULL != impl->mDqStrategy)
    {
        dqStrategy_destroy(impl->mDqStrategy);
        impl->mDqStrategy = NULL;
    }

    /* Destroy the publisher. */
    if(NULL != impl->mSubscPublisher)
    {
        mamaPublisher_destroy(impl->mSubscPublisher);
        impl->mSubscPublisher = NULL;
    }

    /* Destroy the initial request inbox. */
    if(NULL != impl->mInitialRequest)
    {
        imageRequest_destroy(impl->mInitialRequest);
        impl->mInitialRequest = NULL;
    }

    /* The listener message callback. */
    if(NULL != impl->mCallback)
    {
        listenerMsgCallback_destroy(impl->mCallback);
        impl->mCallback = NULL;
    }

    /* The table of subjects. */
    if(NULL != impl->mSubjects)
    {
        wtable_clear_for_each(impl->mSubjects, mamaSubscriptionImpl_freeSubjectContext, NULL);
        wtable_destroy(impl->mSubjects);
        impl->mSubjects = NULL;
    }
#ifdef WITH_ENTITLEMENTS
	else
	{
		if (impl->mSubjectContext.mOeaSubscription != NULL)
		{
			/* Destroy will also close a subscription if it is open */
			oeaSubscription_destroy (impl->mSubjectContext.mOeaSubscription);
			impl->mSubjectContext.mOeaSubscription = NULL;
		}
	}
#endif

    /* Destroy the dq context, this will also destroy the recap request inbox. */
    dqContext_cleanup(&impl->mSubjectContext.mDqContext);
    impl->mRecapRequest = NULL;

    /* Free the symbol in the subject context. */
    if(NULL != impl->mSubjectContext.mSymbol)
    {
        free(impl->mSubjectContext.mSymbol);
        impl->mSubjectContext.mSymbol = NULL;
    }

    /* Clean-up the subscription info strings. */
    /* The root. */
    if(NULL != impl->mSubscRoot)
    {
        free(impl->mSubscRoot);
        impl->mSubscRoot = NULL;
    }

    /* The source. */
    if(NULL != impl->mSubscSource)
    {
        free(impl->mSubscSource);
        impl->mSubscSource = NULL;
    }

    /* The user symbol. */
    if(NULL != impl->mUserSymbol)
    {
        free(impl->mUserSymbol);
        impl->mUserSymbol = NULL;
    }

    /* The subscription symbol. */
    if(NULL != impl->mSubscSymbol)
    {
        free(impl->mSubscSymbol);
        impl->mSubscSymbol = NULL;
    }
}


mama_status mamaSubscriptionImpl_completeBasicInitialisation(mamaSubscription subscription)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;

    /* Ensure the subscription is valid. */
    if(NULL != subscription)
    {
        /* Obtain the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

 	/* Write the log message, this must be done before anything else to ensure backward compatibility. */
    	if(NULL != impl->mUserSymbol)
    	{
            mama_log(MAMA_LOG_LEVEL_FINE, "setupBasic(): %s: subscription (%p)", impl->mUserSymbol, subscription);
    	}
    	else
    	{
            mama_log(MAMA_LOG_LEVEL_FINE, "setupBasic(): subscription (%p)", subscription);
    	}

        /* It is possible that the subscription has been destroyed while the throttle thread is waiting at the
         * mutex lock statement above. Therefore an additional check on the status must now be made, execution
         * should only continue if the status is activating.
         */
        ret = MAMA_STATUS_OK;
        if(MAMA_SUBSCRIPTION_ACTIVATING == wInterlocked_read(&impl->mState))
        {
            /* Fill in the callback structure, note that the destroy callback is a private function as clean-up
             * must be performed internally before the user callback can be invoked.
             */
            mamaMsgCallbacks cb;
            memset(&cb, 0, sizeof(cb));

            /* Acquire the mutex. */
            wlock_lock(impl->mCreateDestroyLock);

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

            /* Unlock the mutex before the create callback is fired. */
            wlock_unlock(impl->mCreateDestroyLock);

            if(MAMA_STATUS_OK == ret)
            {
                /* The subscription is now active, set this before the onCreate callback. */
                mamaSubscriptionImpl_setState(impl, MAMA_SUBSCRIPTION_ACTIVATED);

                /* Invoke the create callback. */
                impl->mUserCallbacks.onCreate(impl, impl->mClosure);
            }
        }
    }

    return ret;
}

mama_status mamaSubscriptionImpl_completeMarketDataInitialisation(mamaSubscription subscription)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;

    /* Ensure the subscription is valid. */
    if(NULL != subscription)
    {
        /* Obtain the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

	/* Write the log message, this must be done before anything else to ensure backward compatibility. */
    	if(NULL != impl->mUserSymbol)
    	{
            mama_log(MAMA_LOG_LEVEL_FINE, "mamaSubscription::setup() %s: setup Marketdata subscription (%p)", impl->mUserSymbol, subscription);
    	}
    	else
    	{
            mama_log(MAMA_LOG_LEVEL_FINE, "mamaSubscription::setup() setup Marketdata subscription (%p)", subscription);
    	}

        /* It is possible that the subscription has been destroyed while the throttle thread is waiting at the
         * mutex lock statement above. Therefore an additional check on the status must now be made, execution
         * should only continue if the status is activating.
         */
        ret = MAMA_STATUS_OK;
        if(MAMA_SUBSCRIPTION_ACTIVATING == wInterlocked_read(&impl->mState))
        {
            /* Acquire the mutex. */
            wlock_lock(impl->mCreateDestroyLock);

            /* Add the subscription to the list of active subscriptions on the transport. This is only done if the
             * subscription is receiving regular updates. The transport uses its list for actions like refreshing.
             */
            if (impl->mSubscMsgType != MAMA_SUBSC_DDICT_SNAPSHOT &&
                impl->mSubscMsgType != MAMA_SUBSC_SNAPSHOT)
            {
                ret = mamaTransport_addSubscription(impl->mTransport, impl, &impl->mSubscHandle);
            }

            if(MAMA_STATUS_OK == ret)
            {
                /* Complete the initialisation of the subscription. */
                mamaSubscriptionImpl_initialize(impl);

                /* The subscription is now active. */
                mamaSubscriptionImpl_setState(impl, MAMA_SUBSCRIPTION_ACTIVATED);
            }

            /* Unlock the mutex. */
            wlock_unlock(impl->mCreateDestroyLock);
        }
    }

    return ret;
}

mama_status mamaSubscriptionImpl_createSubjectContext(mamaSubscriptionImpl *impl, const char *symbol, SubjectContext **context)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NOMEM;

    /* Allocate the context. */
    SubjectContext *localContext = (SubjectContext *)calloc(1, sizeof(SubjectContext));
    if(NULL != localContext)
    {
        /* Save a copy of the symbol in the context. */
        localContext->mSymbol = strdup(symbol);
        if(NULL != localContext->mSymbol)
        {
            /* Get the recap throttle to use with the recap request object. */
            wombatThrottle recapThrottle = mamaTransportImpl_getThrottle(impl->mTransport, MAMA_THROTTLE_RECAP);

            /* Create the recap request for the context. */
            imageRequest recapRequest = NULL;
            ret = imageRequest_create(&recapRequest, impl, localContext, impl->mSubscPublisher, recapThrottle);
            if(MAMA_STATUS_OK == ret)
            {
                /* Initialise the context. */
                dqContext_initializeContext(&localContext->mDqContext, impl->mPreInitialCacheSize, recapRequest);
            }
        }

        /* Clean up if something went wrong. */
        if(MAMA_STATUS_OK != ret)
        {
            /* Log an error. */
            mama_log (MAMA_LOG_LEVEL_ERROR, "Could not create recap image reqest. [%s]", mamaMsgStatus_stringForStatus(ret));

            /* Free the symbol. */
            if(NULL != localContext->mSymbol)
            {
                free(localContext->mSymbol);
            }

            /* Free the context. */
            free(localContext);
            localContext = NULL;
        }

        /* Return the context. */
        *context = localContext;
    }

    return ret;
}

void mamaSubscriptionImpl_createThrottleAction(void *subscription, void *closure)
{
    /* Get the impl. */
    mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

    /* Lock the mutex. */
    wlock_lock(impl->mCreateDestroyLock);

    /* When the subscription is deactivated it will remove any pending create action from the throttle,
     * clear the action to indicate that the action has been removed from the
     * throttle. Note that this is done under the lock.
     */
    impl->mAction = NULL;

    /* Unlock the mutex. */
    wlock_unlock(impl->mCreateDestroyLock);

    /* Complete the subscription creation. */
    mamaSubscriptionImpl_completeMarketDataInitialisation(impl);
}

mama_status mamaSubscriptionImpl_deactivate(mamaSubscriptionImpl *impl)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_OK;

    /* Lock the mutex. */
    wlock_lock(impl->mCreateDestroyLock);
    {
        /* mamaTransport_throttleRemoveForOwner() will lock the throttle, so we
         * need to lock it here before the mCreateDestroyLock to avoid a race condition
         * when destroying a sub which still has the create action on the throttle.
         * We also need to lock the refresh listeners first as we need it to remove
         * the sub listeners and it needs the throttle when sending refreshes.
         */
        /* Get the throttle. */
        wombatThrottle throttle = NULL;
        if(NULL != impl->mTransport)
        {
            throttle = mamaTransportImpl_getThrottle(impl->mTransport, MAMA_THROTTLE_DEFAULT);
        }

        if(NULL != throttle)
        {
            /* Lock the refresh transport before the throttle. */
            wombatThrottle_lock(throttle);
        }

        /* Write a log message. */
        if(NULL == impl->mUserSymbol)
        {
            mama_log(MAMA_LOG_LEVEL_FINE, "mamaSubscription_deactivate(): %Initiating deactivation of subscription (%p)", impl);
        }

        else
        {
            mama_log(MAMA_LOG_LEVEL_FINE, "mamaSubscription_deactivate(): %s: Initiating deactivation of subscription (%p)", impl->mUserSymbol, impl);
        }

        /* If any initial or recap request is pending it must be canceled. */
        imageRequest_cancelRequest(impl->mInitialRequest);
        imageRequest_cancelRequest(impl->mRecapRequest);

        /* Mute the subscription to prevent any more updates coming in, note that the subscription bridge will be NULL
         * for a snapshot or a dictionary subscription.
         */
        if(NULL != impl->mSubscBridge)
        {
            impl->mBridgeImpl->bridgeMamaSubscriptionMute(impl->mSubscBridge);
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

                /* If there is a create action on the throttle it must be removed. */
                if((NULL != throttle) && (NULL != impl->mAction))
                {
                    wombatThrottle_removeAction(throttle, impl->mAction);
                }
            }
        }

        /* Unlock the throttle and the refresh transport if they were locked above. */
        if(NULL != throttle)
        {
            wombatThrottle_unlock (throttle);
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
            /* Unlock the mutex in case the subscription is deleted during the callback. */
            wlock_unlock(impl->mCreateDestroyLock);

            /* Destroy the bridge subscription. */
            ret = impl->mBridgeImpl->bridgeMamaSubscriptionDestroy(impl->mSubscBridge);

            /* Write a log message to maintain backward compatibility, note that the form of the this message is such as to maintain
             * backward compatibility.
             */
            if(ret != MAMA_STATUS_OK)
            {
                mama_log(MAMA_LOG_LEVEL_ERROR, "deactivateAction(): Could not destroy subscription bridge [%s]", mamaStatus_stringForStatus(ret));
            }
        }

        /* Otherwise perform cleanup by invoking the callback directly. */
        else
        {
            /* Unlock the mutex in case the subscription is deleted during the callback. */
            wlock_unlock(impl->mCreateDestroyLock);

            /* Invoke the callback. */
            mamaSubscriptionImpl_onSubscriptionDestroyed(impl, impl->mClosure);
        }
    }

    return MAMA_STATUS_OK;
}

void mamaSubscriptionImpl_deallocate(mamaSubscriptionImpl *impl)
{
    /* Set the state to be de-allocated to at least show in the log that it has been completely removed. */
    mamaSubscriptionImpl_setState(impl, MAMA_SUBSCRIPTION_DEALLOCATED);

    /* Destroy the mutex. */
    wlock_destroy(impl->mCreateDestroyLock);

    /* Destroy the state. */
       wInterlocked_destroy(&impl->mState);

    /* Free the subscription impl. */
    free(impl);
}

char *mamaSubscriptionImpl_determineMappedSymbol(mamaTransport transport, const char *symbol)
{
    /* Returns. */
    char *ret = NULL;

    /* Get the symbol mapping function from the transport. */
    mamaSymbolMapFunc mapFunc = mamaTransport_getSymbolMapFunc(transport);

    /* If there is no mapping function then just return a copy of the symbol. */
    if(NULL == mapFunc)
    {
        ret = strdup(symbol);
    }
    else
    {
        /* Get the closure from the transport as well. */
        void *closure = mamaTransport_getSymbolMapFuncClosure (transport);

        /* Invoke the function to get the symbol. */
        /* We'd rather not create this temporary buffer, but
         * unfortunately, we currently need to create the mapped
         * symbol on the heap.  Perhaps mSubscSymbol and mUserSymbol
         * could be allocated as array members in the struct?
         */
        char tempSymbol[256] = "";
        (*mapFunc)(closure, tempSymbol, symbol, 256);

        /* Return a copy of the symbol. */
        ret = strdup(tempSymbol);
    }

    return ret;
}

void mamaSubscriptionImpl_freeSubjectContext(wtable_t table, void *data, const char* key, void *closure)
{
    /* The data is the subscription context to be destroyed. */
    SubjectContext *context = (SubjectContext *)data;
    if(NULL != context)
    {
        /* Destroy the DQ context. */
        dqContext_cleanup(&context->mDqContext);

        /* Free the symbol. */
        if(NULL != context->mSymbol)
        {
            free(context->mSymbol);
        }

        /* free the context itself. */
        free(context);
    }
}

SubjectContext *mamaSubscriptionImpl_getSubjectContext(mamaSubscriptionImpl *impl, const mamaMsg msg)
{
    /* Subject contexts are only used if there are multiple symbols at play for example with a
     * group or wildcard subscription.
     * If there is only one symbol return it, otherwise the table of subjects will be valid.
     */
#ifdef WITH_ENTITLEMENTS
	oeaStatus        entitlementStatus  = OEA_STATUS_OK;
#endif
    SubjectContext *ret = &impl->mSubjectContext;
    if(NULL != impl->mSubjects)
    {
        /* Get the symbol from the message. */
        const char *messageSymbol = NULL;
        mama_status status = msgUtils_getIssueSymbol(msg, &messageSymbol);
        ret = NULL;
        if((MAMA_STATUS_OK == status) && (NULL != messageSymbol))
        {
            /* Look up the subject context in the table, it is indexed by the symbol. */
            ret = (SubjectContext *)wtable_lookup(impl->mSubjects, (char *)messageSymbol);

            /* If there is no context for this symbol then create a new one. */
            if(NULL == ret)
            {
                status = mamaSubscriptionImpl_createSubjectContext(impl, messageSymbol, &ret);
                if(MAMA_STATUS_OK == status)
                {
#ifdef WITH_ENTITLEMENTS
                	ret->mOeaSubscription = oeaClient_newSubscription (&entitlementStatus, gEntitlementClient);
#endif
                    /* Save the new context in the table. */
                    wtable_insert(impl->mSubjects, (char *)messageSymbol, (void *)ret);
                }
            }
        }

        /* Else log an error if something went wrong. */
        else
        {
            /* Get a string representation of the message. */
            const char* messageString = mamaMsg_toString(msg);

            /* Write an error level log. */
            mama_log(
                MAMA_LOG_LEVEL_ERROR,
                "mamaSubscription_getSubjectContext(): Could not get issue symbol from group message [%s] [%s]. Cannot create/find context. Message: [%s]",
                mamaStatus_stringForStatus(status),
                impl->mSubscSymbol == NULL ? "" : impl->mSubscSymbol,
                messageString == NULL ? "" : messageString);
        }
    }

    return ret;
}

void mamaSubscriptionImpl_initialize(mamaSubscriptionImpl *impl)
{
    /* Write a log message. */
    mama_log(
        MAMA_LOG_LEVEL_FINE,
        "mamaSubscriptionImpl_initialize(): Initializing subscription (%p) for source=%s, symbol=%s (%s)",
        impl,
        impl->mSubscSource != NULL ? impl->mSubscSource : "",
        impl->mSubscSymbol != NULL ? impl->mSubscSymbol : "",
        impl->mUserSymbol  != NULL ? impl->mUserSymbol  : "");

    /* Snapshot subscriptions do not require a bridge. */
    if((impl->mSubscMsgType != MAMA_SUBSC_DDICT_SNAPSHOT) && (impl->mSubscMsgType != MAMA_SUBSC_SNAPSHOT))
    {
        /* Initialise a callback structure. */
        mamaMsgCallbacks callbacks;
        memset(&callbacks, 0, sizeof(callbacks));
        callbacks.onCreate          = impl->mUserCallbacks.onCreate;
        callbacks.onError           = impl->mUserCallbacks.onError;
        callbacks.onMsg             = impl->mUserCallbacks.onMsg;
        callbacks.onQuality         = impl->mUserCallbacks.onQuality;
        callbacks.onGap             = impl->mUserCallbacks.onGap;
        callbacks.onRecapRequest    = impl->mUserCallbacks.onRecapRequest;
        callbacks.onDestroy         = mamaSubscriptionImpl_onSubscriptionDestroyed;

        /* Create the bridge subscription. */
        impl->mBridgeImpl->bridgeMamaSubscriptionCreate(
            &impl->mSubscBridge,
            impl->mSubscSource,
            impl->mSubscSymbol,
            impl->mTransport,
            impl->mQueue,
            callbacks,
            impl,
            impl->mClosure);
    }

    /* If an initial is required then an image request must be sent. */
    if(impl->mRequiresInitial != 0)
    {
        /* Create an initial message for this subscription. */
        mamaMsg initialMessage = NULL;
        mama_status csm = msgUtils_createSubscriptionMessage(impl, impl->mSubscMsgType, &initialMessage, NULL);
        if(MAMA_STATUS_OK == csm)
        {
            /* Set the flags to indicate that an initial is expected. */
            impl->mExpectingInitial                 = 1;
            impl->mSubjectContext.mInitialArrived   = 0;

            /* Send the request without the throttle, the message will be destroyed by the image request. */
            imageRequest_sendRequest(impl->mInitialRequest, initialMessage, impl->mTimeout, impl->mRetries, 0, 0);
        }
    }

    /* Alternatively if a subscription is required then publish the message directly. */
    else if(mamaSubscription_requiresSubscribe((mamaSubscription)impl))
    {
        /* Create an initial message for this subscription. */
        mamaMsg initialMessage = NULL;
        mama_status csm = msgUtils_createSubscriptionMessage(impl, impl->mSubscMsgType, &initialMessage, NULL);
        if(MAMA_STATUS_OK == csm)
        {
            /* Send using the publisher. */
            mamaPublisher_send(impl->mSubscPublisher, initialMessage);

            /* Destroy the message. */
            mamaMsg_destroy(initialMessage);
        }
    }

    /* If this is a wildcard subscription then create the table of subjects. */
    if((NULL != impl->mBridgeImpl) && (impl->mBridgeImpl->bridgeMamaSubscriptionHasWildcards(impl->mSubscBridge) || (impl->mType == MAMA_SUBSC_TYPE_GROUP)))
    {
        impl->mSubjects = wtable_create("subjects", (impl->mGroupSizeHint / 10));
    }

    /* Invoke the onCreate callback now that the subscription has been setup. */
    impl->mUserCallbacks.onCreate(impl, impl->mClosure);

    /* Increment the appropriate stats. */
    if(gGenerateQueueStats)
    {
        mamaStatsCollector *queueStatsCollector = mamaQueueImpl_getStatsCollector(impl->mQueue);
        mamaStatsCollector_incrementStat (*queueStatsCollector, MamaStatNumSubscriptions.mFid);
    }

    if(gGenerateTransportStats)
    {
        mamaStatsCollector *transportStatsCollector = mamaTransport_getStatsCollector(impl->mTransport);
        mamaStatsCollector_incrementStat(*transportStatsCollector, MamaStatNumSubscriptions.mFid);
    }

    if(mamaInternal_getGlobalStatsCollector() != NULL)
    {
        mamaStatsCollector_incrementStat(*(mamaInternal_getGlobalStatsCollector()), MamaStatNumSubscriptions.mFid);
    }
}

void mamaSubscriptionImpl_initializeGroupSizeHint(mamaSubscriptionImpl *impl)
{
    /* If the group size hint is not set then get the value from the transport. */
    if(0 == impl->mGroupSizeHint)
    {
        impl->mGroupSizeHint = mamaTransportImpl_getGroupSizeHint(impl->mTransport);
    }

    /* Apply the minimum size. */
    if(impl->mGroupSizeHint < MAMA_SUBSCRIPTION_MINIMUM_GROUP_SIZE_HINT)
    {
        impl->mGroupSizeHint = MAMA_SUBSCRIPTION_MINIMUM_GROUP_SIZE_HINT;
    }
}

void mamaSubscriptionImpl_logProcessMessage(mamaSubscriptionImpl *impl, mamaMsg message)
{
    /* Get a string representation of the message. */
    const char *text = mamaMsg_toString(message);
    if(NULL != text)
    {
        /* The log message will depend on whether the symbol is valid. */
        if(NULL != impl->mUserSymbol)
        {
            /* Log at finest level. */
            mama_forceLog (
                MAMA_LOG_LEVEL_FINEST,
                "mamaSubscription_processMsg(): %s: msg = %s subsc (%p)",
                impl->mUserSymbol,
                text,
                impl);
        }

        else
        {
            mama_forceLog (
                MAMA_LOG_LEVEL_FINEST,
                "mamaSubscription_processMsg(): msg = %s subsc (%p)",
                text,
                impl);
        }

        /* Free the message text. */
        mamaMsg_freeString(message, text);
    }
}

void MAMACALLTYPE mamaSubscriptionImpl_onDestroyExEvent(mamaQueue queue, void *closure)
{
    /* Destroy the subscription now that we are on the correct thread. */
    mama_status msd = mamaSubscription_destroy((mamaSubscription)closure);

    /* Log an error if something went wrong. */
    if(MAMA_STATUS_OK != msd)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)closure;

        /* Log the message. */
	mama_log(MAMA_LOG_LEVEL_ERROR, "mamaSubscription_DestroyThroughQueueCB::Failed to destroy a subscription on %s.", impl->mSubscSymbol);
    }
}

void MAMACALLTYPE mamaSubscriptionImpl_onRefreshMessageSent(mamaPublisher publisher, mamaMsg message, mama_status status, void *closure)
{
    if(NULL != message)
    {
        /* Destroy the refresh message now that it has been successfully sent. */
        mama_status mmd = mamaMsg_destroy(message);
        if(MAMA_STATUS_OK != mmd)
        {
            mama_log(MAMA_LOG_LEVEL_WARN, "mamaSubscription::sendCompleteCb(): Could not destroy refresh message.");
        }
    }
}

void MAMACALLTYPE mamaSubscriptionImpl_onSubscriptionDestroyed(mamaSubscription subscription, void *closure)
{
    /* Obtain the impl from the subscription object. */
    mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;
    if(NULL != impl)
    {
        /* Lock the mutex. */
        wlock_lock(impl->mCreateDestroyLock);

        /* The next action will depend on the current state of the subscription. */
        switch(wInterlocked_read(&impl->mState))
        {
                /* The subscription is being deactivated. */
            case MAMA_SUBSCRIPTION_DEACTIVATING:

                /* Change the state. */
                mamaSubscriptionImpl_setState(impl, MAMA_SUBSCRIPTION_DEACTIVATED);

                /* Mark the bridge subscription as being destroyed, this function will release the lock. */
                mamaSubscriptionImpl_setSubscriptionDestroyed(impl, closure);

                break;

                /* The subscription is being deallocated, i.e. mamaSubscription_deallocate has been called
                 * before the destroy callback has come in from the bridge.
                 */
            case MAMA_SUBSCRIPTION_DEALLOCATING :

                /* Decrement the open object count on the queue, note that the queue can be NULL if setup
                 * has not yet been called.
                 */
                if(NULL != impl->mQueue)
                {
                    mamaQueue_decrementObjectCount(&impl->mLockHandle, impl->mQueue);
                }

                /* Perform clean up. */
                mamaSubscriptionImpl_cleanup(impl);

                /* Mark the bridge subscription as being destroyed, this function will release the lock. */
                mamaSubscriptionImpl_setSubscriptionDestroyed(impl, closure);

                /* Delete the subscription. */
                mamaSubscriptionImpl_deallocate(impl);

                /* At this point the function must return to prevent as the mutex has been destroyed. */
                return;
                break;

                /* The subscription is being destroyed. */
            case MAMA_SUBSCRIPTION_DESTROYING :

                /* Change the state. */
                mamaSubscriptionImpl_setState(impl, MAMA_SUBSCRIPTION_DESTROYED);

                /* Decrement the open object count on the queue, note that the queue can be NULL if setup
                 * has not yet been called.
                 */
                if(NULL != impl->mQueue)
                {
                    mamaQueue_decrementObjectCount(&impl->mLockHandle, impl->mQueue);
                }

                /* Perform cleanup, note that this will not de-allocate the subscription object, this must be done
                 * separately using the mamaSubscription_deallocate function.
                 */
                mamaSubscriptionImpl_cleanup(impl);

                /* Mark the bridge subscription as being destroyed, this function will release the lock. */
                mamaSubscriptionImpl_setSubscriptionDestroyed(impl, closure);

                break;

                /* The subscription must be de-activated then re-activated. */
            case MAMA_SUBSCRIPTION_ACTIVATING:

                /* Change the state. */
                mamaSubscriptionImpl_setState(impl, MAMA_SUBSCRIPTION_DEACTIVATED);

                /* Mark the bridge subscription as being destroyed, this function will release the lock. */
                mamaSubscriptionImpl_setSubscriptionDestroyed(impl, closure);

                /* Re-activate the subscription. */
                mamaSubscription_activate(subscription);

                break;

                /* Otherwise the subscription is in an invalid state. */
            default:
                {
                    /* Log a message. */
                    int state = wInterlocked_read(&impl->mState);
                    mama_log(MAMA_LOG_LEVEL_ERROR, "Subscription 0x%X is at the invalid state %s, (0x%X).", impl, mamaSubscription_stringForState(state), state);

                    /* Unlock the mutex before the callback is invoked. */
                    wlock_unlock(impl->mCreateDestroyLock);

                    /* Invoke the error callback. */
                    if(NULL != impl->mUserCallbacks.onError)
                    {
                        (*impl->mUserCallbacks.onError)(subscription, MAMA_STATUS_SUBSCRIPTION_INVALID_STATE, NULL, impl->mSubscSymbol, closure);
                    }
                }

                break;
        }
    }
}

mama_status mamaSubscriptionImpl_removeFromThrottle(mamaSubscriptionImpl *impl)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_SUBSCRIPTION_INVALID_STATE;

    /* Acquire the lock before anything else is done. */
    wlock_lock(impl->mCreateDestroyLock);

    /* Only continue if the subscrition has actually been placed on the throttle. */
    if((NULL != impl->mTransport) && (NULL != impl->mAction))
    {
        /* Get the throttle from the transport. */
        wombatThrottle throttle = mamaTransportImpl_getThrottle(impl->mTransport, MAMA_THROTTLE_DEFAULT);
        if(NULL != throttle)
        {
            /* Release the subscription lock, this is done to avoid a race condition where the throttle invokes
             * the mamaSubscriptionImpl_completeMarketDataInitialisation function under the throttle lock. The first
             * thing that this function does is to acquire the subscription lock causing a freeze.
             */
            wlock_unlock(impl->mCreateDestroyLock);

            /* Both the throttle and the refresh transport must be locked in order to perform this function. */
            wombatThrottle_lock(throttle);

            /* Re-acquire the subscription lock. */
            wlock_lock(impl->mCreateDestroyLock);

            /* At this point we have acquire the throttle lock and the subscription lock, as mentioned above it is possible
             * that the subscription has already been processed by the throttle. Therefore check the state and only continue
             * if the subscription is still in the activating state.
             */
            ret = MAMA_STATUS_SUBSCRIPTION_INVALID_STATE;
            if(MAMA_SUBSCRIPTION_ACTIVATING == wInterlocked_read(&impl->mState) &&
               NULL != impl->mAction)
            {
                /* Remove the subscription from the throttle. */
                wombatThrottle_removeAction(throttle, impl->mAction);
                ret = MAMA_STATUS_OK;
            }

            /* Unlock. */
            wombatThrottle_unlock(throttle);
        }
    }

    /* Release the lock. */
    wlock_unlock(impl->mCreateDestroyLock);

    return ret;
}

mama_status mamaSubscriptionImpl_saveSubscriptionInfo(mamaSubscriptionImpl *impl, const char *source, const char *symbol)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NOMEM;

    /* Set the root. */
    if(NULL != impl->mSubscRoot)
    {
        free(impl->mSubscRoot);
    }

    /* The root depends on whether this is a dictionary subscription. */
    if(MAMA_SUBSC_TYPE_DICTIONARY == impl->mType)
    {
        impl->mSubscRoot = strdup(MAMA_SUBSCRIPTION_ROOT_DICT);
    }

    else
    {
        impl->mSubscRoot = strdup(MAMA_SUBSCRIPTION_ROOT_NORMAL);
    }

    if(NULL != impl->mSubscRoot)
    {
        /* Save the source if it is valid. */
        ret = MAMA_STATUS_OK;
        if(NULL != source)
        {
            /* Free the existing source. */
            if(NULL != impl->mSubscSource)
            {
                free(impl->mSubscSource);
            }

            /* Make a copy of the new source. */
            impl->mSubscSource = strdup(source);
            ret = MAMA_STATUS_NOMEM;
            if(NULL != impl->mSubscSource)
            {
                ret = MAMA_STATUS_OK;
            }
        }

        if(MAMA_STATUS_OK == ret)
        {
            /* Save the symbol if it is valid. */
            if((NULL != symbol) && (strcmp (symbol,"") != 0))
            {
                /* Free the existing symbol and the mapped symbol. */
                if(NULL != impl->mUserSymbol)
                {
                    free(impl->mUserSymbol);
                    impl->mUserSymbol = NULL;
                }
                if(NULL != impl->mSubscSymbol)
                {
                    free(impl->mSubscSymbol);
                    impl->mSubscSymbol = NULL;
                }

                /* Make a copy of the user symbol. */
                ret = MAMA_STATUS_NOMEM;
                impl->mUserSymbol = strdup(symbol);
                if(NULL != impl->mUserSymbol)
                {
                    /* If symbol mapping is turned on then we need to translate the user symbol. */
                    impl->mSubscSymbol = mamaSubscriptionImpl_determineMappedSymbol(impl->mTransport, symbol);
                    if(NULL != impl->mSubscSymbol)
                    {
                        /* Function succeeded. */
                        ret = MAMA_STATUS_OK;
                    }
                }
            }
        }
    }

    return ret;
}

void mamaSubscriptionImpl_setState(mamaSubscriptionImpl *impl, mamaSubscriptionState state)
{
    /* Set the state using an atomic operation so it will be thread safe. */
    wInterlocked_set(state, &impl->mState);

    /* Write a log message at finest level to indicate the new state. */
    mama_log(MAMA_LOG_LEVEL_FINEST, "Subscription 0x%p is now at state %s.", impl, mamaSubscription_stringForState(state));
}

void mamaSubscriptionImpl_setSubscriptionDestroyed(mamaSubscriptionImpl *impl, void *closure)
{
    /* Clear the impl member variable. */
    impl->mSubscBridge = NULL;

    /* Unlock the mutex before the callback is invoked. */
    wlock_unlock(impl->mCreateDestroyLock);

    /* Invoke the user's callback if it has been supplied.
     * Note that there is a problem where the callback structure may not have been cleared
     * after allocation and the destroy pointer may be set to an invalid memory location.
     * If the structure has been cleared then the resevered field will be NULL. Therefore
     * only invoke the callback if reserved is NULL AND the callback is set.
     */
    if(NULL != impl->mUserCallbacks.onDestroy)
    {
        (*impl->mUserCallbacks.onDestroy)((mamaSubscription)impl, closure);
    }
}

mama_status mamaSubscriptionImpl_verifyServiceLevel(mamaSubscriptionImpl *impl)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_INVALID_ARG;

    /* Set the subscription type and flags based on the service level, note
     * that an invalid service level will case an error to be returned.
     */
    switch(impl->mServiceLevel)
    {
            /* Normal real time data. */
        case MAMA_SERVICE_LEVEL_REAL_TIME:
            ret = MAMA_STATUS_OK;
            break;

            /* Snapshot subscription, create an initial then de-activate. */
        case MAMA_SERVICE_LEVEL_SNAPSHOT:

            /* Requires initial must be on. */
            if(impl->mRequiresInitial)
            {
                /* Set the sub message type. */
                impl->mSubscMsgType = MAMA_SUBSC_SNAPSHOT;
                ret = MAMA_STATUS_OK;
            }
#ifdef WITH_ENTITLEMENTS
            oeaSubscription_setIsSnapshot (impl->mSubjectContext.mOeaSubscription, 1);
#endif
            break;

        /* All other cases are not supported. */
	default:
	    break;
    }

    if(MAMA_STATUS_OK == ret)
    {
        /* Also check the type. */
        switch(impl->mType)
        {
                /* Normal subscription. */
            case MAMA_SUBSC_TYPE_NORMAL:
                break;

                /* Group subscription has initials for each symbol. */
            case MAMA_SUBSC_TYPE_GROUP:
                impl->mAcceptMultipleInitials   = 1;
                impl->mRecoverGaps              = 1;
                break;

                /* Book subscription has only 1 initial. */
            case MAMA_SUBSC_TYPE_BOOK:
                impl->mAcceptMultipleInitials   = 0;
                impl->mRecoverGaps              = 1;
                break;

                /* Dictionary subscription. */
            case MAMA_SUBSC_TYPE_DICTIONARY:
                impl->mSubscMsgType = MAMA_SUBSC_DDICT_SNAPSHOT;
                break;

                /* Symbol list has multiple initials. */
            case MAMA_SUBSC_TYPE_SYMBOL_LIST:
            case MAMA_SUBSC_TYPE_SYMBOL_LIST_NORMAL:
            case MAMA_SUBSC_TYPE_SYMBOL_LIST_GROUP:
            case MAMA_SUBSC_TYPE_SYMBOL_LIST_BOOK:
                impl->mAcceptMultipleInitials   = 1;
                impl->mRecoverGaps              = 0;
                break;

                /* All other types are unrecognised or unsupported. */
            default:
                ret = MAMA_STATUS_INVALID_ARG;
                break;
        }
    }

    return ret;
}

/* *************************************************** */
/* Public Functions. */
/* *************************************************** */

mama_status mamaSubscription_allocate(mamaSubscription *subscription)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != subscription)
    {
        /* Allocate a new impl. */
        mamaSubscriptionImpl * impl = (mamaSubscriptionImpl *)calloc (1, sizeof (mamaSubscriptionImpl));
        ret = MAMA_STATUS_NOMEM;
        if(NULL != impl)
        {
            /* Read the pre-initial cache size from the properties. */
            const char *preInitialCacheSize = mama_getProperty(MAMA_SUBSCRIPTION_PROPNAME_PREINITIALCACHESIZE);
            if(NULL != preInitialCacheSize)
            {
                /* Convert the value from a string. */
                impl->mPreInitialCacheSize = atoi(preInitialCacheSize);

                /* Write the value out in a log message. */
                mama_log (MAMA_LOG_LEVEL_FINE, "PreInitialCacheSize set to %d", impl->mPreInitialCacheSize);
            }

            /* Create the mutex, this is used to protect access during create and
             * destroy operations.
             */
            impl->mCreateDestroyLock = wlock_create();

            /* Initialise all other members. */
            impl->mType                   = MAMA_SUBSC_TYPE_NORMAL;
            impl->mServiceLevel           = MAMA_SERVICE_LEVEL_REAL_TIME;
            impl->mRespondToNextRefresh   = 1;
            impl->mTimeout                = MAMA_SUBSCRIPTION_DEFAULT_TIMEOUT;
            impl->mRetries                = MAMA_SUBSCRIPTION_DEFAULT_RETRIES;
            impl->mRecoverGaps            = 1;
            impl->mRequiresInitial        = 1;
            impl->mSubscMsgType           = MAMA_SUBSC_SUBSCRIBE;
            impl->mAppDataType            = MAMA_MD_DATA_TYPE_STANDARD;
            impl->mDebugLevel             = MAMA_LOG_LEVEL_WARN;
            impl->mPreInitialCacheSize    = MAMA_SUBSCRIPTION_DEFAULT_PREINITIALCACHESIZE;

            /* Set the initial state of the subscription now that the memory has been allocated. */
            wInterlocked_initialize(&impl->mState);
            mamaSubscriptionImpl_setState(impl, MAMA_SUBSCRIPTION_ALLOCATED);

            /* The function has succeeded. */
            ret = MAMA_STATUS_OK;
        }

        /* Return the impl. */
        *subscription = impl;
    }

    return ret;
}

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
                    ret = mamaSubscriptionImpl_activate(impl);
                    break;

                    /* In the following states the subscription is already active so don't need to do anything. */
                case MAMA_SUBSCRIPTION_ACTIVATED:

                    /* The subscription is currently activating, don't do anything. */
                case MAMA_SUBSCRIPTION_ACTIVATING:

                    ret = MAMA_STATUS_OK;
                    break;

                    /* We are currently de-activating, re-activate as soon as this process is complete. */
                case MAMA_SUBSCRIPTION_DEACTIVATING:

                    /* Set the state to indicate that the subscription will be reactivated. */
                    mamaSubscriptionImpl_setState(impl, MAMA_SUBSCRIPTION_ACTIVATING);

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

int mamaSubscription_checkDebugLevel(mamaSubscription subscription, MamaLogLevel level)
{
    /* Returns. */
    int ret = 0;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Compare the levels. */
        if(impl->mDebugLevel >= level)
        {
            ret = 1;
        }
    }

    return ret;
}

mama_status mamaSubscription_create(mamaSubscription subscription, mamaQueue queue, const mamaMsgCallbacks *callbacks, mamaSource source, const char *symbol, void *closure)
{
    /* This function is equivalent to calling setup followed by activate. */
    mama_status ret = mamaSubscription_setup(subscription, queue, callbacks, source, symbol, closure);
    if(MAMA_STATUS_OK == ret)
    {
        ret = mamaSubscription_activate(subscription);
    }

    return ret;
}

mama_status mamaSubscription_createBasic(mamaSubscription subscription, mamaTransport transport, mamaQueue queue, const mamaMsgCallbacks *callbacks, const char *topic, void *closure)
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

            /* Allocated and destroyed are the only 2 valid states for this function call. */
            ret = MAMA_STATUS_SUBSCRIPTION_INVALID_STATE;
            if((MAMA_SUBSCRIPTION_ALLOCATED == wInterlocked_read(&impl->mState)) || (MAMA_SUBSCRIPTION_DESTROYED == wInterlocked_read(&impl->mState)))
            {
                /* Lock the mutex. */
                wlock_lock(impl->mCreateDestroyLock);

                /* Get the bridge impl from the transport. */
                ret = MAMA_STATUS_NO_BRIDGE_IMPL;
                impl->mBridgeImpl = mamaTransportImpl_getBridgeImpl(transport);

                /* If the bridge isn't valid then write a log message. */
                if(NULL == impl->mBridgeImpl)
                {
                    mama_log (MAMA_LOG_LEVEL_ERROR,"mamaSubscription_setupBasic(): Could not get bridge impl from transport.");

                    /* Unlock the mutex. */
                    wlock_unlock(impl->mCreateDestroyLock);
                }

                else
                {
                    /* Initialise the impl. */
                    impl->mClosure              = closure;
                    impl->mQueue                = queue;
                    impl->mRequiresInitial      = 0;
                    impl->mSubscSymbol          = mamaSubscriptionImpl_determineMappedSymbol(transport, topic);
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

                    /* Increment the object lock count on the queue. */
                    impl->mLockHandle = mamaQueue_incrementObjectCount(impl->mQueue, subscription);

                    /* Unlock the mutex, the lock will be taken again in the setup basic function, it must be released
                     * however before the onCreate callback can be fired in case the subscription is deactivated inside
                     * the callback
                     . */
                    wlock_unlock(impl->mCreateDestroyLock);

                    /* The basic subscription is no longer throttled by default, use setup then activate to do this.
                     * Instead invoke the final part of setup directly.
                     */
                    ret = mamaSubscriptionImpl_completeBasicInitialisation(impl);
                }
            }
        }
    }

    return ret;
}

mama_status mamaSubscription_createBasicWildCard(mamaSubscription subscription, mamaTransport transport, mamaQueue queue, const mamaWildCardMsgCallbacks *callbacks, const char *source, const char *symbol, void *closure)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != subscription)
    {
        /* Obtain the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Format the combined symbol and source name. */
        char buffer[(MAMA_MAX_SYMBOL_LEN + MAMA_MAX_SOURCE_LEN + 2)] = "";

        /* If the symbol is valid then combine it with the source name. */
        if(NULL != symbol)
        {
            if(NULL != source)
            {
                snprintf(buffer, (MAMA_MAX_SYMBOL_LEN + MAMA_MAX_SOURCE_LEN + 2), "%s.%s", source, symbol);
            }
            else
            {
                snprintf(buffer, MAMA_MAX_SYMBOL_LEN + 1, "%s", symbol);
            }

            /* With a valid symbol is this a wildcard type. */
            impl->mWildCardType = wc_wildcard;
        }
        else
        {
            /* Just use the source name. */
            snprintf(buffer, (MAMA_MAX_SOURCE_LEN + 1), "%s", source);

            /* Without a valid symbol is this a transport type. */
            impl->mWildCardType = wc_transport;
        }

        /* Copy the callback function pointers over. */
        memcpy(&impl->mWcCallbacks, callbacks, sizeof(impl->mWcCallbacks));

        /* Set the callbacks here as we will pass NULL to the setupBasic function below. */
        impl->mUserCallbacks.onCreate   = callbacks->onCreate;
        impl->mUserCallbacks.onDestroy  = callbacks->onDestroy;
        impl->mUserCallbacks.onError    = callbacks->onError;

        /* Delegate to the normal create function. */
        ret = mamaSubscription_createBasic (
                subscription,
                transport,
                queue,
                NULL,
                buffer,
                closure);
    }

    return ret;
}

mama_status mama_createDictionary(mamaDictionary *dictionary, mamaQueue queue, mamaDictionaryCallbackSet dictionaryCallback, mamaSource source, double timeout, int retries, void *closure)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != dictionary)
    {
        /* This variable will contain the returned dictionary. */
        mamaDictionary localDictionary = NULL;

        /* Check specifically for the queue. */
        ret = MAMA_STATUS_INVALID_QUEUE;
        if(NULL != queue)
        {
            /* Create the dictionary. */
            ret = mamaDictionary_create(&localDictionary);
            if(MAMA_STATUS_OK == ret)
            {
                /* Save the closure. */
                ret = mamaDictionary_setClosure(localDictionary, closure);
                if(MAMA_STATUS_OK == ret)
                {
                    /* Allocate the dictionary subscription. */
                    mamaSubscription subscription = NULL;
                    ret = mamaSubscription_allocate(&subscription);
                    if(MAMA_STATUS_OK == ret)
                    {
                        /* Cast the subscription to an impl to set member variables rather than calling the
                         * accessor functions.
                         */
                        mamaSubscriptionImpl *subscriptionImpl = (mamaSubscriptionImpl *)subscription;

                        /* Set the properties required for a dictionary subscription. */
                        subscriptionImpl->mType             = MAMA_SUBSC_TYPE_DICTIONARY;
                        subscriptionImpl->mServiceLevel     = MAMA_SERVICE_LEVEL_SNAPSHOT;
                        subscriptionImpl->mServiceLevelOpt  = 0;
                        subscriptionImpl->mRequiresInitial  = 1;
                        subscriptionImpl->mAppDataType      = MAMA_MD_DATA_TYPE_STANDARD;
                        subscriptionImpl->mTimeout          = timeout;
                        subscriptionImpl->mRetries          = retries;

                        /* Save the subscription inside the dictionary object. */
                        ret = mamaDictionary_setSubscPtr(localDictionary, subscription);
                        if(MAMA_STATUS_OK == ret)
                        {
                            /* Install the callbacks, this variable will point to the complete callback
                             * structure whenever this has been done.
                             */
                            const mamaMsgCallbacks *callbacks = NULL;

                            /* Complete callback sent whenever the dictionary has been fully downloaded. */
                            if(NULL != dictionaryCallback.onComplete)
                            {
                                mamaDictionary_setCompleteCallback(localDictionary, dictionaryCallback.onComplete);
                            }

                            /* Error callback if anything goes wrong. */
                            if(NULL != dictionaryCallback.onError)
                            {
                                mamaDictionary_setErrorCallback(localDictionary, dictionaryCallback.onError);
                            }

                            /* Timeout callback if the timeout elapses. */
                            if(NULL != dictionaryCallback.onTimeout)
                            {
                                mamaDictionary_setTimeoutCallback(localDictionary, dictionaryCallback.onTimeout);
                            }

                            /* Now that the callbacks have been installed get the completed callback structure from
                             * the dictionary object.
                             */
                            callbacks = mamaDictionary_getSubscCb(localDictionary);

                            /* Finally create the subscription, the dictionary object will then handle the various
                             * callbacks coming in from the subscription.
                             * Note that the dictionary is passed as a closure.
                             */
                            ret = mamaSubscription_create(subscription, queue, callbacks, source, MAMA_SUBSCRIPTION_DICTIONARY_SYMBOL, (void *)localDictionary);
                        }

                        /* If this failed then the subscription must be de-allocated, note that otherwise the subscription
                         * will be cleaned up by the dictionary.
                         */
                        else
                        {
                            mamaSubscription_deallocate(subscription);
                        }
                    }
                }

                /* If anything went wrong destroy the dictionary. */
                if(MAMA_STATUS_OK != ret)
                {
                    mamaDictionary_destroy(localDictionary);
                    localDictionary = NULL;
                }
            }
        }

        /* Return the dictionary object. */
        *dictionary = localDictionary;
    }

    return ret;
}

mama_status mamaSubscription_createSnapshot(mamaSubscription subscription, mamaQueue queue, const mamaMsgCallbacks *callbacks, mamaSource source, const char *symbol, void *closure)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != subscription)
    {
        /* Obtain the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Set the properties required for a snapshot subscription. */
        impl->mType             = MAMA_SUBSC_TYPE_NORMAL;
        impl->mServiceLevel     = MAMA_SERVICE_LEVEL_SNAPSHOT;
        impl->mServiceLevelOpt  = 0;
        impl->mRequiresInitial  = 1;
        impl->mAppDataType      = MAMA_MD_DATA_TYPE_STANDARD;

        /* Delegate to the standard create function. */
        ret = mamaSubscription_create(subscription, queue, callbacks, source, symbol, closure);
    }

    return ret;
}

mama_status mamaSubscription_deactivate(mamaSubscription subscription)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Deactivate is not supported for a basic subscription. */
        ret = MAMA_STATUS_SUBSCRIPTION_INVALID_TYPE;
        if(MAMA_SUBSC_TYPE_BASIC != impl->mType)
        {
            /* The next action will depend on the current state of the subscription. */
            switch(wInterlocked_read(&impl->mState))
            {
                    /* The subscription is waiting on the throttle. */
                case MAMA_SUBSCRIPTION_ACTIVATING:

                    /* Remove the subscription from the throttle queue. */
                    ret = mamaSubscriptionImpl_removeFromThrottle(impl);

                    /* If invalid state is returned by this function then the subscription has become active
                     * while waiting for it to be removed from the throttle.
                     */
					if(MAMA_STATUS_SUBSCRIPTION_INVALID_STATE == ret &&
                       MAMA_SUBSCRIPTION_ACTIVATED == wInterlocked_read(&impl->mState))
                    {
                        /* Invoke this function recursively to process the correct state. */
                        ret = mamaSubscription_deactivate(subscription);
                    }

                    /* Otherwise the subscription has been correctly removed from the throttle. */
                    else
                    {
                        /* Set the state to deactivating. */
                        mamaSubscriptionImpl_setState(impl, MAMA_SUBSCRIPTION_DEACTIVATING);

                        /* Perform any clean-up by invoking the destroy callback directly. */
                        mamaSubscriptionImpl_onSubscriptionDestroyed(subscription, impl->mClosure);
                        ret = MAMA_STATUS_OK;
                    }

                    break;

                    /* Deactivate the subscription. */
                case MAMA_SUBSCRIPTION_ACTIVATED:

                    /* Set the state to indicate that the subscription is in the process of being deactivated. */
                    mamaSubscriptionImpl_setState(impl, MAMA_SUBSCRIPTION_DEACTIVATING);

                    /* Deactivate the subscription, clean-up will be performed on the callback. */
                    ret = mamaSubscriptionImpl_deactivate(impl);
                    break;

                    /* The subscription is currently de-activating, don't do anything. */
                case MAMA_SUBSCRIPTION_DEACTIVATING:

                    /* In the following states the subscription is not active so don't need to do anything. */
                case MAMA_SUBSCRIPTION_ALLOCATED:
                case MAMA_SUBSCRIPTION_SETUP:
                case MAMA_SUBSCRIPTION_DEACTIVATED:
                case MAMA_SUBSCRIPTION_DESTROYED:

                    ret = MAMA_STATUS_OK;
                    break;

                    /* All other states are invalid. */
                case MAMA_SUBSCRIPTION_DESTROYING:
                default:
                    ret = MAMA_STATUS_SUBSCRIPTION_INVALID_STATE;
                    break;
            }
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

                /* Perform clean up. */
                mamaSubscriptionImpl_cleanup(impl);

                /* Delete the subscription. */
                mamaSubscriptionImpl_deallocate(impl);

                /* Return from here to prevent the mutex being unlocked. */
                return MAMA_STATUS_OK;
                break;

                /* All other states are invalid. */
            default:

                /* Write a log message as this has been done previously and we don't want to break backward compatibility. */
                mama_log (MAMA_LOG_LEVEL_WARN,"Could not deallocate mamaSubscription as it has not been destroyed.");

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

        /* The next action will depend on the current state of the subscription. */
        switch(wInterlocked_read(&impl->mState))
        {
                /* The subscription is currently activating, (i.e. it is awaiting processing by the throttle. */
            case MAMA_SUBSCRIPTION_ACTIVATING:

                /* Remove the subscription from the throttle queue. */
                ret = mamaSubscriptionImpl_removeFromThrottle(impl);

                /* If invalid state is returned by this function then the subscription has become active
                 * while waiting for it to be removed from the throttle.
                 */
                if(MAMA_STATUS_SUBSCRIPTION_INVALID_STATE == ret &&
				   MAMA_SUBSCRIPTION_ACTIVATED == wInterlocked_read(&impl->mState))
                {
                    /* Invoke this function recursively to process the correct state. */
                    ret = mamaSubscription_destroy(subscription);
                    break;
                }

                /* Otherwise the subscription has been correctly removed from the throttle. */
                /* Fall through to perform the remaining clean-up. */

                /* For the following states the subscription is not active, simply perform clean-up. */
            case MAMA_SUBSCRIPTION_ALLOCATED:
            case MAMA_SUBSCRIPTION_SETUP:
            case MAMA_SUBSCRIPTION_DEACTIVATED:

                /* Set the state to destroying. */
                mamaSubscriptionImpl_setState(impl, MAMA_SUBSCRIPTION_DESTROYING);

                /* Perform clean-up by invoking the destroy callback directly. */
                mamaSubscriptionImpl_onSubscriptionDestroyed(subscription, impl->mClosure);

                ret = MAMA_STATUS_OK;
                break;

                /* Destroy the subscription. */
            case MAMA_SUBSCRIPTION_ACTIVATED:

                /* Set the state to indicate that the subscription is in the process of being destroyed. */
                mamaSubscriptionImpl_setState(impl, MAMA_SUBSCRIPTION_DESTROYING);

                /* Deactivate the subscription, clean-up will be performed on the callback. */
                ret = mamaSubscriptionImpl_deactivate(impl);
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

                /* The subscription has already been destroyed, do nothing. */
            case MAMA_SUBSCRIPTION_DESTROYED:
                ret = MAMA_STATUS_OK;
                break;

                /* All other states are invalid. */
            default:
                ret = MAMA_STATUS_SUBSCRIPTION_INVALID_STATE;
                break;
        }
    }

    return ret;
}

mama_status mamaSubscription_destroyEx(mamaSubscription subscription)
{
    /* Returns. */
	mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Destroy the subscription on the correct thread by enqueuing an event. */
        ret = mamaQueue_enqueueEvent(impl->mQueue, mamaSubscriptionImpl_onDestroyExEvent, (void *)impl);

        /* Write a log if something went wrong. */
        if(MAMA_STATUS_OK != ret)
	    {
		    mama_log(MAMA_LOG_LEVEL_ERROR, "mamaSubscription_destroyEx::Failed to enqueue the destruction of a subscription on %s.", impl->mSubscSymbol);
	    }
    }

    return ret;
}

mama_status mamaSubscription_getAppDataType(mamaSubscription subscription, uint8_t *val)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Return the data type. */
        *val = impl->mAppDataType;

        /* The function has succeeded. */
        ret = MAMA_STATUS_OK;
    }

    return ret;
}

mama_status mamaSubscription_getExpectingInitial(mamaSubscription subscription, int *expectingInitial)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Return the flag. */
        *expectingInitial = impl->mExpectingInitial;

        /* The function has succeeded. */
        ret = MAMA_STATUS_OK;
    }

    return ret;
}

mama_status mamaSubscription_getClosure(mamaSubscription subscription, void **closure)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Return the closure. */
        *closure = impl->mClosure;

        /* The function has succeeded. */
        ret = MAMA_STATUS_OK;
    }

    return ret;
}

MamaLogLevel mamaSubscription_getDebugLevel(mamaSubscription subscription)
{
    /* Returns, the default level is warn. */
    MamaLogLevel ret = MAMA_LOG_LEVEL_WARN;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Get the new debug level. */
        ret = impl->mDebugLevel;
    }

    return ret;
}

mama_status mamaSubscription_getItemClosure(mamaSubscription subscription, void **itemClosure)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Verify that the context is valid. */
        ret = MAMA_STATUS_INVALID_ARG;
        if(NULL != impl->mCurSubjectContext)
        {
            /* Return the item closure. */
            *itemClosure = impl->mCurSubjectContext->mClosure;

            /* The function has succeeded. */
            ret = MAMA_STATUS_OK;
        }
    }

    return ret;
}

mama_status mamaSubscription_getMsgQualifierFilter(mamaSubscription subscription, int *ignoreDefinitelyDuplicate, int *ignorePossiblyDuplicate, int *ignoreDefinitelyDelayed, int *ignorePossiblyDelayed, int *ignoreOutOfSequence)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if((NULL != subscription) && (NULL != ignoreDefinitelyDuplicate) && (NULL != ignorePossiblyDuplicate) && (NULL != ignoreDefinitelyDelayed) && (NULL != ignorePossiblyDelayed) && (NULL != ignoreOutOfSequence))
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Set the values of the flags based on the options saved in the subscription. */
        *ignoreDefinitelyDuplicate 	= (impl->mMsgQualFilter & MAMA_MSG_QUAL_DEFINITELY_DUPLICATE);
        *ignorePossiblyDuplicate 	= (impl->mMsgQualFilter & MAMA_MSG_QUAL_POSSIBLY_DUPLICATE);
        *ignoreDefinitelyDelayed 	= (impl->mMsgQualFilter & MAMA_MSG_QUAL_DEFINITELY_DELAYED);
        *ignorePossiblyDelayed 		= (impl->mMsgQualFilter & MAMA_MSG_QUAL_POSSIBLY_DELAYED);
        *ignoreOutOfSequence 		= (impl->mMsgQualFilter & MAMA_MSG_QUAL_OUT_OF_SEQUENCE);

        /* Function succeeded. */
        ret = MAMA_STATUS_OK;
    }

    return ret;
}

mama_status mamaSubscription_getPlatformError(mamaSubscription subscription, void **error)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Verify that the bridge is valid. */
        ret = MAMA_STATUS_NO_BRIDGE_IMPL;
        if(NULL != impl->mBridgeImpl)
        {
            /* Get the error directly from the bridge. */
            ret = impl->mBridgeImpl->bridgeMamaSubscriptionGetPlatformError(impl->mSubscBridge, error);
        }
    }

    return ret;
}

mama_status mamaSubscription_getPreIntitialCacheSize(mamaSubscription subscription, int *result)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Return the cache size. */
        *result = impl->mPreInitialCacheSize;

        /* Function has succeeded. */
        ret = MAMA_STATUS_OK;
    }

    return ret;
}

mama_status mamaSubscription_getQueue(mamaSubscription subscription, mamaQueue *queue)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Return the transport. */
        *queue = impl->mQueue;

        /* The function has succeeded. */
        ret = MAMA_STATUS_OK;
    }

    return ret;
}

mama_status mamaSubscription_getReceivedInitial(mamaSubscription subscription, int *receivedInitial)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Check to see if an initial has arrived. */
        *receivedInitial = 0;
        if(1 == impl->mSubjectContext.mInitialArrived)
        {
            *receivedInitial = 1;
        }

        /* The function has succeeded. */
        ret = MAMA_STATUS_OK;
    }

    return ret;
}

mama_status mamaSubscription_getRecoverGaps(mamaSubscription subscription, int *result)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Set the type. */
        *result = impl->mRecoverGaps;

        /* Function succeeded. */
        ret = MAMA_STATUS_OK;
    }

    return ret;
}

mama_status mamaSubscription_getRequiresInitial(mamaSubscription subscription, int *requiresInitial)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Return the flag. */
        *requiresInitial = impl->mRequiresInitial;

        /* The function has succeeded. */
        ret = MAMA_STATUS_OK;
    }

    return ret;
}

mama_status mamaSubscription_getRetries(mamaSubscription subscription, int *val)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Verify that this isn't a basic subscription. */
        ret = MAMA_STATUS_INVALID_ARG;
        if(MAMA_SUBSC_TYPE_BASIC != impl->mType)
        {
            /* Return the number of retries. */
            *val = impl->mRetries;

            /* The function has succeeded. */
            ret = MAMA_STATUS_OK;
        }
    }

    return ret;
}

mama_status mamaSubscription_getServiceLevel(mamaSubscription subscription, mamaServiceLevel *serviceLevel)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Return the service level. */
        *serviceLevel = impl->mServiceLevel;

        /* The function has succeeded. */
        ret = MAMA_STATUS_OK;
    }

    return ret;
}

mama_status mamaSubscription_getServiceLevelOpt(mamaSubscription subscription, long *serviceLevelOpt)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Return the options. */
        *serviceLevelOpt = impl->mServiceLevelOpt;

        /* The function has succeeded. */
        ret = MAMA_STATUS_OK;
    }

    return ret;
}

mama_status mamaSubscription_getSource(mamaSubscription subscription, const char **source)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Verify that this isn't a basic subscription. */
        ret = MAMA_STATUS_INVALID_ARG;
        if(MAMA_SUBSC_TYPE_BASIC != impl->mType)
        {
            /* Return the source. */
            *source = impl->mSubscSource;

            /* The function has succeeded. */
            ret = MAMA_STATUS_OK;
        }
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

mama_status mamaSubscription_getSubscriptionType(mamaSubscription subscription, mamaSubscriptionType *type)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Return the type. */
        *type = impl->mType;

        /* The function has succeeded. */
        ret = MAMA_STATUS_OK;
    }

    return ret;
}

mama_status mamaSubscription_getSubscSymbol(mamaSubscription subscription, const char **symbol)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Return the symbol. */
        *symbol = impl->mSubscSymbol;

        /* The function has succeeded. */
        ret = MAMA_STATUS_OK;
    }

    return ret;
}

mama_status mamaSubscription_getSymbol(mamaSubscription subscription, const char **symbol)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Return the symbol. */
        *symbol = impl->mUserSymbol;

        /* The function has succeeded. */
        ret = MAMA_STATUS_OK;
    }

    return ret;
}

mama_status mamaSubscription_getTimeout(mamaSubscription subscription, double *val)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Verify that this isn't a basic subscription. */
        ret = MAMA_STATUS_INVALID_ARG;
        if(MAMA_SUBSC_TYPE_BASIC != impl->mType)
        {
            /* Return the timeout. */
            *val = impl->mTimeout;

            /* The function has succeeded. */
            ret = MAMA_STATUS_OK;
        }
    }

    return ret;
}

mama_status mamaSubscription_getTransport(mamaSubscription subscription, mamaTransport *transport)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Return the transport. */
        *transport = impl->mTransport;

        /* The function has succeeded. */
        ret = MAMA_STATUS_OK;
    }

    return ret;
}

mama_status mamaSubscription_getTransportIndex(mamaSubscription subscription, int *tportIndex)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Return the transport. */
        *tportIndex = impl->mTransportIndex;

        /* The function has succeeded. */
        ret = MAMA_STATUS_OK;
    }

    return ret;
}

mamaMsgCallbacks *mamaSubscription_getUserCallbacks(mamaSubscription subscription)
{
    /* Returns. */
    mamaMsgCallbacks *ret = NULL;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Return the message callbacks. */
        ret = &impl->mUserCallbacks;
    }

    return ret;
}

int mamaSubscription_isActive(mamaSubscription subscription)
{
    /* IsValid and IsActive are the same thing. */
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

mama_status mamaSubscription_muteCurrentTopic (mamaSubscription subscription)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Only continue if the bride is valid. */
        ret = MAMA_STATUS_NO_BRIDGE_IMPL;
        if((NULL != impl->mBridgeImpl) && (NULL != impl->mSubscBridge) && (wc_transport == impl->mWildCardType))
        {
            /* Pass the call onto the bridge. */
            ret = impl->mBridgeImpl->bridgeMamaSubscriptionMuteCurrentTopic(impl->mSubscBridge);
        }
    }

    return ret;
}

mama_status mamaSubscription_setAppDataType(mamaSubscription subscription, uint8_t appDataType)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Set the data type. */
        impl->mAppDataType = appDataType;

        /* Function succeeded. */
        ret = MAMA_STATUS_OK;
    }

    return ret;
}

mama_status mamaSubscription_setDebugLevel(mamaSubscription subscription, MamaLogLevel level)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Set the new debug level. */
        impl->mDebugLevel = level;

        /* Function succeeded. */
        ret = MAMA_STATUS_OK;
    }

    return ret;
}

mama_status mamaSubscription_setExpectingInitial(mamaSubscription subscription, int expectingInitial)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Set the flag. */
        impl->mExpectingInitial = expectingInitial;

        /* Function succeeded. */
        ret = MAMA_STATUS_OK;
    }

    return ret;
}

mama_status mamaSubscription_setGroupSizeHint(mamaSubscription subscription, int groupSizeHint)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Set the type. */
        impl->mGroupSizeHint = groupSizeHint;

        /* Function succeeded. */
        ret = MAMA_STATUS_OK;
    }

    return ret;
}

mama_status mamaSubscription_setItemClosure(mamaSubscription subscription, void *closure)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* If this is a wildcard subscription then pass this to the bridge. */
        if(impl->mWildCardType != wc_none)
        {
            if((NULL != impl->mBridgeImpl) && (NULL != impl->mSubscBridge))
            {
                ret = impl->mBridgeImpl->bridgeMamaSubscriptionSetTopicClosure(impl->mSubscBridge, closure);
            }
        }

        else
        {
            /* This is an invalid operation for a group subscription, (the current context will be NULL). */
            ret = MAMA_STATUS_INVALID_ARG;
            if(NULL != impl->mCurSubjectContext)
            {
                /* Save the closure in the current context. */
                impl->mCurSubjectContext->mClosure = closure;

                /* The function has succeeded. */
                ret = MAMA_STATUS_OK;
            }
        }
    }

    return ret;
}

mama_status mamaSubscription_setPreIntitialCacheSize(mamaSubscription subscription, int cacheSize)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Set the cache size. */
        impl->mPreInitialCacheSize = cacheSize;

        /* Ensure that it isn't below 0. */
        if(impl->mPreInitialCacheSize < 0)
        {
            impl->mPreInitialCacheSize = 0;
        }

        /* Function has succeeded. */
        ret = MAMA_STATUS_OK;
    }

    return ret;
}

mama_status mamaSubscription_setRecoverGaps(mamaSubscription subscription, int doesRecover)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Set the type. */
        impl->mRecoverGaps = doesRecover;

        /* Function succeeded. */
        ret = MAMA_STATUS_OK;
    }

    return ret;
}

mama_status mamaSubscription_setRequiresInitial(mamaSubscription subscription, int requiresInitial)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Set the flag. */
        impl->mRequiresInitial = requiresInitial;

        /* Function succeeded. */
        ret = MAMA_STATUS_OK;
    }

    return ret;
}

mama_status mamaSubscription_setRetries(mamaSubscription subscription, int retries)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Verify that this is not a basic subscription. */
        ret = MAMA_STATUS_INVALID_ARG;
        if(MAMA_SUBSC_TYPE_BASIC != impl->mType)
        {
            /* Set the retries. */
            impl->mRetries = retries;

            /* Function succeeded. */
            ret = MAMA_STATUS_OK;
        }
    }

    return ret;
}

mama_status mamaSubscription_setServiceLevel(mamaSubscription subscription, mamaServiceLevel serviceLevel, long serviceLevelOpt)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Set the service level values. */
        impl->mServiceLevel     = serviceLevel;
        impl->mServiceLevelOpt  = serviceLevelOpt;

        /* Function succeeded. */
        ret = MAMA_STATUS_OK;
    }

    return ret;
}

mama_status mamaSubscription_setSubscriptionType(mamaSubscription subscription, mamaSubscriptionType type)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Set the type. */
        impl->mType = type;

        /* Function succeeded. */
        ret = MAMA_STATUS_OK;
    }

    return ret;
}

mama_status mamaSubscription_setSymbol(mamaSubscription subscription, const char *symbol)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Free the current symbol. */
        if(NULL != impl->mUserSymbol)
        {
            free(impl->mUserSymbol);
        }

        /* Make a copy of the new symbol. */
        ret = MAMA_STATUS_NOMEM;
        impl->mUserSymbol = strdup(symbol);
        if(NULL != impl->mUserSymbol)
        {
            /* Function succeeded. */
            ret = MAMA_STATUS_OK;
        }
    }

    return ret;
}

mama_status mamaSubscription_setTimeout(mamaSubscription subscription, double timeout)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Verify that this is not a basic subscription. */
        ret = MAMA_STATUS_INVALID_ARG;
        if(MAMA_SUBSC_TYPE_BASIC != impl->mType)
        {
            /* Set the timeout. */
            impl->mTimeout = timeout;

            /* Function succeeded. */
            ret = MAMA_STATUS_OK;
        }
    }

    return ret;
}

mama_status mamaSubscription_setTransportIndex(mamaSubscription subscription, int tportIndex)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != subscription)
    {
        /* Get the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Return the transport. */
        impl->mTransportIndex = tportIndex;

        /* The function has succeeded. */
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

mama_status mamaSubscription_setupBasic(mamaSubscription subscription, mamaTransport transport, mamaQueue queue, const mamaMsgCallbacks *callbacks, const char *source, const char *symbol, void *closure)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    const char*       root                      = MAMA_SUBSCRIPTION_ROOT_NORMAL;
#ifdef WITH_ENTITLEMENTS
	oeaStatus        entitlementStatus  = OEA_STATUS_OK;
#endif

    if(NULL != subscription)
    {
        /* If the transport is NULL we will return invalid arg. */
        ret = MAMA_STATUS_INVALID_ARG;
        if(NULL != transport)
        {
            /* If the queue is NULL then invalid queue will be returned. */
            ret = MAMA_STATUS_INVALID_QUEUE;
            if(NULL == queue)
            {
                /* Write a log message. */
                mama_log(MAMA_LOG_LEVEL_ERROR, "mamaSubscription_setupBasic(): NULL Queue");
            }

            else
            {
                /* Get the impl. */
                mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

                /* Obtain the bridge from the transport. */
                ret = MAMA_STATUS_NO_BRIDGE_IMPL;
                impl->mBridgeImpl = mamaTransportImpl_getBridgeImpl(transport);
                if(NULL == impl->mBridgeImpl)
                {
                    /* Write a log message. */
                    mama_log(MAMA_LOG_LEVEL_ERROR,"mamaSubscription_setupBasic(): Could not get bridge impl from transport.");
                }

                else
                {
#ifdef WITH_ENTITLEMENTS
					impl->mSubjectContext.mOeaSubscription = oeaClient_newSubscription (&entitlementStatus, gEntitlementClient);
#endif

					/*Up from entitlement check based on string compare on symbol*/
					if (!isEntitledToSymbol (source, symbol, impl))
					{
						setSubscInfo (impl, transport, root, source, symbol);
						return MAMA_STATUS_NOT_ENTITLED;
					}
                    /* Ensure that the service level and type fields have been correctly set. */
                    ret = mamaSubscriptionImpl_verifyServiceLevel(impl);
                    if(MAMA_STATUS_OK == ret)
                    {
                        /* Make a copy of the symbol for the main subscription context. */
                        ret = MAMA_STATUS_NOMEM;
                        impl->mSubjectContext.mSymbol = strdup(symbol);
                        if(NULL != impl->mSubjectContext.mSymbol)
                        {
                            /* Particular types of subscription has predefined symbol names, determine this now. */
                            char *updatedSymbol = mamaSubscriptionImpl_checkSymbol(impl, symbol);
                            if(NULL != updatedSymbol)
                            {
                                /* Update the subscription structure. */
                                impl->mClosure      = closure;
                                impl->mQueue        = queue;
                                impl->mTransport    = transport;

                                /* Save the callback pointers, note that the callback object will be NULL for
                                 * wildcard subscritions.
                                 */
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

                                /* Get the transport index that wil lbe used for this subscription. */
                                mamaTransportImpl_getTransportIndex(transport, &impl->mTransportIndex);
                                mamaTransportImpl_nextTransportIndex(transport, source, updatedSymbol);

                                /* Initialise the group size hint. */
                                mamaSubscriptionImpl_initializeGroupSizeHint(impl);

                                /* Save the subscription symbol names. */
                                ret = mamaSubscriptionImpl_saveSubscriptionInfo(impl, source, updatedSymbol);
                                if(MAMA_STATUS_OK == ret)
                                {
                                    /* Create the publisher. */
                                    ret = mamaPublisher_createByIndex(&impl->mSubscPublisher, impl->mTransport, impl->mTransportIndex, impl->mSubscSymbol, impl->mSubscSource, impl->mSubscRoot);
                                    if(MAMA_STATUS_OK == ret)
                                    {
                                        /* Get the initial throttle. */
                                        wombatThrottle throttle = mamaTransportImpl_getThrottle(impl->mTransport, MAMA_THROTTLE_DEFAULT);

                                        /* Create the image request objects. */
                                        /* Initial image request. */
                                        ret = imageRequest_create(&impl->mInitialRequest, subscription, &impl->mSubjectContext, impl->mSubscPublisher, throttle);
                                        if(MAMA_STATUS_OK == ret)
                                        {
                                            /* Get the recap throttle. */
                                            throttle = mamaTransportImpl_getThrottle(impl->mTransport, MAMA_THROTTLE_RECAP);

                                            /* Create the recap request. */
                                            ret = imageRequest_create(&impl->mRecapRequest, subscription, &impl->mSubjectContext, impl->mSubscPublisher, throttle);
                                            if(MAMA_STATUS_OK == ret)
                                            {
                                                /* Create the listener message callback. */
                                                ret = listenerMsgCallback_create(&impl->mCallback, subscription);
                                                if(MAMA_STATUS_OK == ret)
                                                {
                                                    /* Create the DQ strategy - sequence number checking etc. */
                                                    ret = dqStrategy_create(&impl->mDqStrategy, subscription);
                                                    if(MAMA_STATUS_OK == ret)
                                                    {
                                                        /* Increment the object lock count on the queue. */
                                                        impl->mLockHandle = mamaQueue_incrementObjectCount(impl->mQueue, subscription);

                                                        /* The subscription has now been fully set-up. */
                                                        mamaSubscriptionImpl_setState(impl, MAMA_SUBSCRIPTION_SETUP);
                                                    }

                                                    else
                                                    {
                                                        mama_log(MAMA_LOG_LEVEL_ERROR, "Could not create DQ strategy. [%s]", mamaMsgStatus_stringForStatus(ret));
                                                    }
                                                }

                                                else
                                                {
                                                    mama_log(MAMA_LOG_LEVEL_ERROR, "Could not create msg listener. [%s]", mamaMsgStatus_stringForStatus(ret));
                                                }
                                            }

                                            else
                                            {
                                                mama_log(MAMA_LOG_LEVEL_ERROR, "Could not create recap image reqest. [%s]", mamaMsgStatus_stringForStatus(ret));
                                            }
                                        }

                                        else
                                        {
                                            mama_log (MAMA_LOG_LEVEL_ERROR, "Could not create image reqest. [%s]", mamaMsgStatus_stringForStatus(ret));
                                        }
                                    }

                                    else
                                    {
                                        /* Log an error. */
                                        mama_log (MAMA_LOG_LEVEL_ERROR,  "Could not create publisher bridge. [%s]", mamaMsgStatus_stringForStatus(ret));
                                    }
                                }

                                /* Delete the updated symbol. */
                                free(updatedSymbol);
                            }
                        }
                    }
                }
            }
        }
    }

    return ret;
}

mama_status mamaSubscription_setupBasicWildCard(mamaSubscription subscription, mamaTransport transport, mamaQueue queue, const mamaWildCardMsgCallbacks *callbacks, const char *source, const char *symbol, void *closure)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(NULL != subscription)
    {
        /* Obtain the impl. */
        mamaSubscriptionImpl *impl = (mamaSubscriptionImpl *)subscription;

        /* Set the wildcard type. */
        if(NULL == symbol)
        {
            impl->mWildCardType = wc_transport;
        }

        else
        {
            impl->mWildCardType = wc_wildcard;
        }

        /* Copy the callback function pointers over. */
        memcpy(&impl->mWcCallbacks, callbacks, sizeof(impl->mWcCallbacks));

        /* Set the callbacks here as we will pass NULL to the setupBasic function below. */
        impl->mUserCallbacks.onCreate   = callbacks->onCreate;
        impl->mUserCallbacks.onDestroy  = callbacks->onDestroy;
        impl->mUserCallbacks.onError    = callbacks->onError;

        /* Delegate to the normal setup function. */
        ret = mamaSubscription_setupBasic(subscription, transport, queue, NULL, source, symbol, closure);
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
        case MAMA_SUBSCRIPTION_DESTROYING: return "MAMA_SUBSCRIPTION_DESTROYING";
        case MAMA_SUBSCRIPTION_DESTROYED: return "MAMA_SUBSCRIPTION_DESTROYED";
        case MAMA_SUBSCRIPTION_DEALLOCATING: return "MAMA_SUBSCRIPTION_DEALLOCATING";
        case MAMA_SUBSCRIPTION_DEALLOCATED: return "MAMA_SUBSCRIPTION_DEALLOCATED";
    }

    return "State not recognised";
}

