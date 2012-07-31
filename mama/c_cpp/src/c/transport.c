/* $Id: transport.c,v 1.79.4.6.2.2.2.8 2011/10/02 19:02:17 ianbell Exp $
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

#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "mama/mama.h"
#include "throttle.h"
#include "list.h"
#include "transportimpl.h"
#include "bridge.h"
#include "mama/timer.h"
#include "mama/publisher.h"
#include "msgutils.h"
#include "subscriptionimpl.h"
#include "clientmanageresponder.h"
#include "mamainternal.h"
#include "wombat/wincompat.h"
#include "refreshtransport.h"
#include "publisherimpl.h"
#include "platform.h"
#include "mama/stat.h"
#include "mama/statfields.h"
#include "statsgeneratorinternal.h"
#include "mama/statscollector.h"

extern int gGenerateTransportStats;
extern int gGenerateLbmStats;
extern int gLogTransportStats;
extern int gPublishTransportStats;

#define self                                ((transportImpl*)(transport))
#define MAX_TPORT_NAME_LEN                  (256)
#define MAX_PROP_STRING                     1000
#define DEFAULT_WANT_AUTO_CM_CREATE         0
#define PROP_NAME_WANT_AUTO_CM_CREATE       "cm_responder_enabled"
#define PROP_NAME_DISABLE_REFRESH           "disable_refresh"
#define PROP_NAME_DISABLE_DISCONNECT_CB     "disable_disconnect_callbacks"
/* A value of 10 will be passed to wtable_create in subscription code */
#define DEFAULT_GROUP_SIZE_HINT             100

static void
roundRobin (int         curTransportIndex,
            int         numTransports,
            const char* source,
            const char* symbol,
            int*        nextTransportIndex);

static void
randomInitial (int  numTransports,
               int* transportIndex);

typedef struct transportImpl_
{
    /* Transport has single bridge or multiple bridges for load balancing.
     * mCurTransportIndex is the bridge to use for the next subscription;
     * this is incremented by round robin or mLoadBalanceCb */
    transportBridge*         mTransports;
    int                      mNumTransports;
    int                      mCurTransportIndex;
    LIB_HANDLE               mLoadBalanceHandle;
    mamaTransportLbCB        mLoadBalanceCb;
    mamaTransportLbInitialCB mLoadBalanceInitialCb;
    mamaTransportCB          mTportCb;
    void*                    mTportClosure;
    mamaTransportTopicCB     mTportTopicCb;
    void*                    mTportTopicClosure;
    wombatThrottle           mThrottle;
    wombatThrottle           mRecapThrottle;
    mamaSymbolMapFunc        mMapFunc;
    void*                    mMapFuncClosure;

    uint32_t                 mWriteQueueHighWatermark;
    uint32_t                 mWriteQueueLowWatermark;
    /* These members are only needed for the market data transport */
    wList                    mListeners;

    /* This list contains all of the publishers using the transport. */
    wList                    mPublishers;

    /* mSetPossiblyStaleForAll: If true then every listener will be set to
     * possibly stale whenever an advisory occurs.  mQuality: Set to possibly
     * stale when an advisory arrives. Set to OK when a valid message arrives
     * subsequently.  mCause: Reason for the advisory.  mPlatform: Middleware
     * specific data associated with the advisory. */
    /* The platform info and the cause are stored to facilitate iteration of
     * listeners and are only valid for the duration of the function into which
     * they are passed, (for example mamaTransportImpl_processAdvisory). Most
     * importantly transport.c does not manage and memory on behalf of the
     * platform info.
     */
    int                      mSetPossiblyStaleForAll;
    mamaQuality              mQuality;
    short                    mCause;
    void*                    mPlatformInfo;

    /*Identifier for the middleware being used. Specified upon creation*/
    mamaBridgeImpl*          mBridgeImpl;

    refreshTransport         mRefreshTransport;
    char                     mName[MAX_TPORT_NAME_LEN];
    mamaCmResponder          mCmResponder;
    char*                    mDescription;

    int                     mDeactivateSubscriptionOnError;
    mamaStatsCollector*     mStatsCollector;
    mamaStat                mRecapStat;
    mamaStat                mUnknownMsgStat;
    mamaStat                mMessageStat;
    mamaStat                mFtTakeoverStat;
    mamaStat                mSubscriptionStat;
    mamaStat                mTimeoutStat;
    mamaStat                mInitialStat;
    mamaStat                mWombatMsgsStat;
    mamaStat                mFastMsgsStat;
    mamaStat                mRvMsgsStat;
    /* LBM stats.  */
    /*! Number of sent LBT-RM NAK packets */
    mamaStat                mNakPacketsSent;
    /*! Number of LBT-RM individul NAKs sent */
    mamaStat                mNaksSent;
    /*! Number of lost LBT-RM messages detected */
    mamaStat                mMsgsLost;
    /*! Number of NCFs received with reason set to ignore */
    mamaStat                mNcfsIgnore;
    /*! Number of NCFs received with reason set to shed */
    mamaStat                mNcfsShed;
    /*! Number of NCFs received with reason set to retransmit delay */
    mamaStat                mNcfsRxDelay;
    /*! Number of NCFs received with reason unknown */
    mamaStat                mNcfsUnknown;
    /*! Duplicate LBT-RM data messages received */
    mamaStat                mDuplicateDataMsgs;
    /*! Number of LBT-RM messages unrecoverable due to window advance */
    mamaStat                mUnrecoverableWindowAdvance;
    /*! Number of LBT-RM messages unrecoverable due to NAK generation expiration
     */
    mamaStat                mUnrecoverableTimeout;
    /*! Number of LBM messages received by LBT-RM transport with no topic associated */
    mamaStat                mLbmMsgsReceivedNoTopic;
    /*! Number of LBM requests received by LBT-RM transport */
    mamaStat                mLbmRequestsReceived;

    int                     mGroupSizeHint;

    uint8_t                 mDisableRefresh;
    dqStartegyScheme        mDQStratScheme;
    dqftStrategyScheme      mFTStratScheme;

    uint8_t                 mInternal;
    uint8_t                 mDisableDisconnectCb;
    preInitialScheme         mPreInitialScheme;
} transportImpl;

static mama_status
init (transportImpl* transport, int createResponder)
{
    mama_status rval = MAMA_STATUS_OK;

    self->mListeners     = list_create (sizeof (SubscriptionInfo));
    self->mPublishers    = list_create (sizeof (mamaPublisher));
    self->mSetPossiblyStaleForAll = 1;
    self->mQuality       = MAMA_QUALITY_OK;
    self->mCause         = 0;
    self->mPlatformInfo  = NULL;
    self->mPreInitialScheme = PRE_INITIAL_SCHEME_ON_GAP;
    self->mDQStratScheme    = DQ_SCHEME_DELIVER_ALL;
    self->mFTStratScheme    = DQ_FT_DO_NOT_WAIT_FOR_RECAP;


    mama_log (MAMA_LOG_LEVEL_FINEST,
             "%screating CmResponder for transport [%s]",
              createResponder ? "" : "Not ", self->mName ? self->mName : "");

    if (createResponder)
    {
        rval = mamaCmResponder_create (&transport->mCmResponder,
                                     (mamaTransport)transport,
                                     self->mNumTransports);
          if (rval != MAMA_STATUS_OK) return rval;
    }

    return MAMA_STATUS_OK;
}

/* *************************************************** */
/* Private Function Prototypes. */
/* *************************************************** */

/**
 * This function is used with mamaTransportImpl_clearTransportWithListeners and
 * will be invoked for each subscription held by the transport, (note that this
 * will be market data subscriptions only.
 * @param (in) list The list of subscriptions being enumerate.
 * @param (in) element This is actually a pointer to a SubscriptionInfo structure.
 * @param (in) closure Not used.
 *
 */
void mamaTransportImpl_clearTransportCallback (wList list, void *element, void *closure);
void mamaTransportImpl_clearTransportPublisherCallback (wList list, void *element, void *closure);
void mamaTransportImpl_clearTransportWithPublishers (transportImpl *impl);

/**
 * This function will inform all subscriptions listening to the transport that
 * the transport is about to be destroyed and that they should clear and
 * references or pointers they have. This is important as the subscription may
 * still not be completely destroyed as it will be waiting for a callback from
 * the middleware.
 *
 * @param (in) impl The transport impl.
 *
 */
void mamaTransportImpl_clearTransportWithListeners (transportImpl *impl);

static void
setPossiblyStaleForListeners (transportImpl* transport);

mama_status
mamaTransport_allocate (mamaTransport* result)
{
    transportImpl*  transport    =   NULL;
    mama_status     status       =   MAMA_STATUS_OK;


    transport = (transportImpl*)calloc (1, sizeof (transportImpl ) );
    if (transport == NULL)  return MAMA_STATUS_NOMEM;

    /*We need to create the throttle here as properties may be set
     before the transport is actually created.*/
    if (MAMA_STATUS_OK!=(status=wombatThrottle_allocate (&self->mThrottle)))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaTransport_allocate (): Could not"
                  " create throttle.");
        return status;
    }

    wombatThrottle_setRate (self->mThrottle,
                            MAMA_DEFAULT_THROTTLE_RATE);

    if (MAMA_STATUS_OK !=
       (status = wombatThrottle_allocate (&self->mRecapThrottle)))
    {
        return status;
    }

    wombatThrottle_setRate (self->mRecapThrottle,
                            MAMA_DEFAULT_RECAP_THROTTLE_RATE);

    self->mDescription          = NULL;
    self->mLoadBalanceCb        = NULL;
    self->mLoadBalanceInitialCb = NULL;
    self->mLoadBalanceHandle    = NULL;
    self->mCurTransportIndex    = 0;
    self->mDeactivateSubscriptionOnError = 1;
    self->mGroupSizeHint        = DEFAULT_GROUP_SIZE_HINT;
    *result = (mamaTransport)transport;

    self->mName[0] = '\0';

    return MAMA_STATUS_OK;
}

/**
 * Check property which determines whether to create a cmresponder object
 * "under the hood" when the transport is created.
 *
 * Return non-zero if object should be created, otherwise return zero
 */
static int mamaTransportInternal_cmResponderEnabled (transportImpl *impl,
                                                     const char* transportName,
                                                     const char* middleware)
{
  const char* propValue;
  char propString[MAX_PROP_STRING];
  char propStringMw[MAX_PROP_STRING];
  int retVal;
  /* Returns. */
  int ret = 0;

  /* The CM responder will not be created for an internal transport. */
  if(impl->mInternal == 0)
  {
      /* Check for mama.middleware.transport.transportname first */
      retVal=snprintf (propStringMw, MAX_PROP_STRING,
              "mama.%s.transport.%s.%s", middleware,
              transportName ? transportName : "", PROP_NAME_WANT_AUTO_CM_CREATE);

      if ((retVal<0) || (retVal>=MAX_PROP_STRING))
      {
          mama_log (MAMA_LOG_LEVEL_ERROR,
                  "Error reading %s from properties file", PROP_NAME_WANT_AUTO_CM_CREATE);
          return DEFAULT_WANT_AUTO_CM_CREATE;
      }

      propValue = properties_Get (mamaInternal_getProperties (), propStringMw);

      if (NULL==propValue)
      {
          /* We might have specified mama.transport.transportname -
             only look for this after we've tried with middleware */
          retVal = snprintf (propString, MAX_PROP_STRING,
                  "mama.transport.%s.%s",
                  transportName ? transportName : "", PROP_NAME_WANT_AUTO_CM_CREATE);

          if ((retVal<0) || (retVal>=MAX_PROP_STRING))
          {
              mama_log (MAMA_LOG_LEVEL_ERROR,
                      "Error reading %s from properties file", PROP_NAME_WANT_AUTO_CM_CREATE);
              return DEFAULT_WANT_AUTO_CM_CREATE;
          }

          propValue = properties_Get (mamaInternal_getProperties (), propString);
      }

      /* Return default if we have specified neither mama.middleware.transport...nor
         mama.transport... */
      if (NULL==propValue)
      {
          return DEFAULT_WANT_AUTO_CM_CREATE;
      }
      else if (properties_GetPropertyValueAsBoolean (propValue))
      {
          return 1;
      }
      else
      {
          return 0;
      }
  }
  return ret;
}

static void setPreInitialStrategy (mamaTransport transport)
{
    const char* propValue     = NULL;
    char propNameBuf[256];

    if (!self) return;

    snprintf (propNameBuf, 256, "mama.transport.%s.preinitialstrategy", self->mName);

    propValue = properties_Get (mamaInternal_getProperties (),
                               propNameBuf);

    if (NULL!=propValue)
    {
        mama_log (MAMA_LOG_LEVEL_NORMAL, "Setting %s=%s",
                  propNameBuf, propValue);

        if (0==strcmp (propValue, "initial"))
        {
            self->mPreInitialScheme = PRE_INITIAL_SCHEME_ON_INITIAL;
        }
        else
        {
            self->mPreInitialScheme = PRE_INITIAL_SCHEME_ON_GAP;
        }
    }
    else
    {
        mama_log (MAMA_LOG_LEVEL_NORMAL,
                  "%s: Using default preinitial strategy: ON_GAP", self->mName);
    }
}
static void setDQStrategy (mamaTransport transport)
{
    const char* propValue     = NULL;
    char propNameBuf[256];

    if (!self) return;

    snprintf (propNameBuf, 256, "mama.transport.%s.dqstrategy", self->mName);

    propValue = properties_Get(mamaInternal_getProperties(),
                               propNameBuf);

    if (NULL!=propValue)
    {
        mama_log (MAMA_LOG_LEVEL_NORMAL, "Setting %s=%s",
                  propNameBuf, propValue);

        if (0==strcmp (propValue, "ignoredups"))
        {
            self->mDQStratScheme = DQ_SCHEME_INGORE_DUPS;
        }
        else
        {
            self->mDQStratScheme = DQ_SCHEME_DELIVER_ALL;
        }
    }
    else
    {
        mama_log (MAMA_LOG_LEVEL_NORMAL,
                  "%s: Using default dq strategy: DQ_SCHEME_DELIVER_ALL", self->mName);
    }
}

static void setFtStrategy (mamaTransport transport)
{
    const char* propValue     = NULL;
    char propNameBuf[256];

    if (!self) return;

    snprintf (propNameBuf, 256, "mama.transport.%s.ftstrategy", self->mName);

    propValue = properties_Get(mamaInternal_getProperties(),
                               propNameBuf);

    if (NULL!=propValue)
    {
        mama_log (MAMA_LOG_LEVEL_NORMAL, "Setting %s=%s",
                  propNameBuf, propValue);

        if (0==strcmp (propValue, "waitforrecap"))
        {
            self->mFTStratScheme = DQ_FT_WAIT_FOR_RECAP;
        }
        else
        {
            self->mFTStratScheme = DQ_FT_DO_NOT_WAIT_FOR_RECAP;
        }
    }
    else
    {
        mama_log (MAMA_LOG_LEVEL_NORMAL,
                  "%s: Using default ft strategy: DQ_FT_DO_NOT_WAIT_FOR_RECAP", self->mName);
    }
}

void mamaTransport_disableRefresh(mamaTransport transport, uint8_t disable)
{
    self->mDisableRefresh=disable;
}
/**
 * Check property to disable refresh messages. Undocumented.
 *
 * Return non-zero to disable refresh messages.
 */

static int mamaTransportInternal_disableRefreshes(const char* transportName)
{
  const char* propValue;
  char propString[MAX_PROP_STRING];
  int retVal;

  retVal=snprintf (propString, MAX_PROP_STRING,
    "mama.transport.%s.%s",
    transportName ? transportName : "", PROP_NAME_DISABLE_REFRESH);

  if ((retVal<0) || (retVal>=MAX_PROP_STRING))
  {
    /* snprintf failed (somehow) */
    mama_log (MAMA_LOG_LEVEL_ERROR,
      "Error reading %s from properties file", PROP_NAME_DISABLE_REFRESH);
    return 0;
  }

  propValue = properties_Get (mamaInternal_getProperties (), propString);

  if (NULL==propValue)
    return 0;
  else if (properties_GetPropertyValueAsBoolean (propValue))
    return 1;
  else
    return 0;
}

static int mamaTransportImpl_disableDisconnectCb (const char* transportName)
{
  const char* propValue;
  char propString[MAX_PROP_STRING];
  int retVal;

  retVal=snprintf (propString, MAX_PROP_STRING,
    "mama.transport.%s.%s",
    transportName ? transportName : "", PROP_NAME_DISABLE_DISCONNECT_CB);

  if ((retVal<0) || (retVal>=MAX_PROP_STRING))
  {
    /* snprintf failed (somehow) */
    mama_log (MAMA_LOG_LEVEL_ERROR,
      "Error reading %s from properties file", PROP_NAME_DISABLE_DISCONNECT_CB);
    return 0;
  }

  propValue = properties_Get (mamaInternal_getProperties (), propString);

  if (NULL==propValue)
    return 0;
  else if (properties_GetPropertyValueAsBoolean (propValue))
    return 1;
  else
    return 0;
}

static void setDeactivateOnError (mamaTransport transport)
{
    const char* propValue     = NULL;
    char propNameBuf[256];

    if (!self) return;

    snprintf (propNameBuf, 256, "mama.transport.%s.deactivateonerror", self->mName);
    /*Check for a user specified activity timer value*/
    propValue = properties_Get (mamaInternal_getProperties (),
                               propNameBuf);
    if (NULL!=propValue)
    {
        mama_log (MAMA_LOG_LEVEL_NORMAL, "Setting %s=%s",
                  propNameBuf, propValue);
        self->mDeactivateSubscriptionOnError = strtod (propValue, NULL);
    }
    else
        self->mDeactivateSubscriptionOnError = 1;
}

static void setGroupSizeHint (mamaTransport transport, const char* middleware)
{
    const char* propValue   = NULL;
    char propNameBuf[256];
    char propNameBufMw[256];

    if (!self) return;

    /* Check for mama.middleware.transport[...] first */
    snprintf (propNameBufMw, 256, "mama.%s.transport.%s.groupsizehint",
                middleware, self->mName);
    propValue = properties_Get (mamaInternal_getProperties (),
                               propNameBufMw);

    /* Only use mama.transport[...] if not found with middleware */
    if (NULL == propValue)
    {
        snprintf (propNameBuf, 256, "mama.transport.%s.groupsizehint", self->mName);
        propValue = properties_Get (mamaInternal_getProperties (),
                                   propNameBuf);
    }
    else
    {
        mama_log (MAMA_LOG_LEVEL_NORMAL, "Setting  %s=%s",
                  propNameBufMw, propValue);
    }

    /* Only use default if null after both tries */
    if (NULL!=propValue)
    {
        mama_log (MAMA_LOG_LEVEL_NORMAL, "Setting %s=%s",
                  propNameBuf, propValue);
        self->mGroupSizeHint = strtod (propValue, NULL);
    }
    else
        self->mGroupSizeHint = DEFAULT_GROUP_SIZE_HINT;
}

/**
 * Construct an TransportImpl.
 *
 * @param transport the transport.
 */
 /* TODO break up this way too big function !!!!
    The load balancing code should be separate GMc*/
mama_status
mamaTransport_create (mamaTransport transport,
                      const char*   name,
                      mamaBridge    bridgeImpl)
{
    mama_status rval = MAMA_STATUS_OK;
    int           i;
    int           numTransports;
    int           isLoadBalanced;
    char          loadBalanceName[MAX_TPORT_NAME_LEN];
    const char*   bridgeName = NULL;
    const char*   sharedObjectName = NULL;
    mamaQueue     defaultQueue = NULL;
    tportLbScheme scheme;
    mama_status   status;
    const char*   middleware = NULL;
     const char*     throttleInt  =   NULL;
    if (!transport) return MAMA_STATUS_NULL_ARG;
    if (!bridgeImpl) return MAMA_STATUS_NO_BRIDGE_IMPL;
    mama_log(MAMA_LOG_LEVEL_FINER, "Entering mamaTransport_create for transport (%p) with name %s", transport, name);

    self->mBridgeImpl = (mamaBridgeImpl*)bridgeImpl;

    /*So that we can configure the default transport!*/
    if (!name) name = "default";

    if (name)
    {
        snprintf (self->mName, MAX_TPORT_NAME_LEN, "%s", name);
        self->mName[MAX_TPORT_NAME_LEN-1] = '\0';
    }

    setDeactivateOnError (transport);

    mama_log (MAMA_LOG_LEVEL_FINE, "Creating transport: [%s]",
                                    name ? name : "");

    if (MAMA_STATUS_OK!=(status=mama_getDefaultEventQueue (
                            (mamaBridge)(self->mBridgeImpl), &defaultQueue)))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaTransport_create ():"
                  " Could not get default queue.");
        return status;
    }

    if (gGenerateTransportStats || gGenerateLbmStats)
    {
        if (MAMA_STATUS_OK != (status =
                mamaTransport_initStats (transport)))
        {
            return status;
        }
    }

    throttleInt = properties_Get (mamaInternal_getProperties (),
            "mama.throttle.interval");
    if (throttleInt != NULL)
    {
        double interval = strtod (throttleInt, NULL);
        if (interval != 0)
        {
            mama_log (MAMA_LOG_LEVEL_FINER,
                    "Setting mama.throttle.interval to %f\n", interval);
            wombatThrottle_setInterval (self->mThrottle, interval);
        }
    }

    wombatThrottle_create (self->mRecapThrottle, defaultQueue);
    wombatThrottle_create (self->mThrottle,      defaultQueue);

    rval = self->mBridgeImpl->bridgeMamaTransportGetNumLoadBalanceAttributes (
                                                name, &numTransports);
    if (rval != MAMA_STATUS_OK)
        return rval;



    mama_log (MAMA_LOG_LEVEL_FINER,
              "Number of load balanced sub transports defined: [%d]",
              numTransports);

    if (numTransports == 0)
    {
        /* Load balance transports are not to be used, so just a single
           transport is required. */
        isLoadBalanced = 0;
        self->mNumTransports = 1;
        mama_log (MAMA_LOG_LEVEL_FINER,
                  "Not using load balancing");
    }
    /* TODO break this out into another function */
    else
    {
        isLoadBalanced = 1;
        self->mNumTransports = numTransports;
        rval = self->mBridgeImpl->bridgeMamaTransportGetLoadBalanceScheme (
                                                name, &scheme);

        if (rval != MAMA_STATUS_OK)
            return rval;

        switch (scheme)
        {
        case TPORT_LB_SCHEME_ROUND_ROBIN:
        {
            self->mLoadBalanceCb = roundRobin;
            mama_log (MAMA_LOG_LEVEL_FINER,
                      "Using round robin load balancing");
            break;
        }
        case TPORT_LB_SCHEME_API:
        {
            if (!self->mLoadBalanceCb)
            {
                mama_log (MAMA_LOG_LEVEL_FINER,
                          "No user defined load balancing function set");
            }
            else
            {
                mama_log (MAMA_LOG_LEVEL_FINER,
                        "Using API defined load balancing");
            }
            break;
        }
        case TPORT_LB_SCHEME_LIBRARY:
        {
            rval = self->mBridgeImpl->
                    bridgeMamaTransportGetLoadBalanceSharedObjectName (
                            name,
                            &sharedObjectName);
            if (rval != MAMA_STATUS_OK)
                return rval;

            if (sharedObjectName)
            {
                self->mLoadBalanceHandle = openSharedLib (sharedObjectName, NULL);
                if (self->mLoadBalanceHandle)
                {
                    mama_log (MAMA_LOG_LEVEL_FINER,
                              "Using Library defined load balancing");
                    self->mLoadBalanceCb =  (mamaTransportLbCB)
                        loadLibFunc ((LIB_HANDLE)self->mLoadBalanceHandle,
                                "loadBalance");
                    self->mLoadBalanceInitialCb = (mamaTransportLbInitialCB)
                        loadLibFunc ((LIB_HANDLE)self->mLoadBalanceHandle,
                                "loadBalanceInitial");
                }
                else
                {
                    mama_log (MAMA_LOG_LEVEL_FINE,
                        "Can not open library: [%s]",
                        sharedObjectName);
                }
            }
            else
            {
                mama_log (MAMA_LOG_LEVEL_FINE,
                         "No load balancing shared object specified.");
            }

            break;
        }
        case TPORT_LB_SCHEME_STATIC:
        default:
        {
            self->mLoadBalanceCb = NULL;
            mama_log (MAMA_LOG_LEVEL_FINER,
                    "Using static load balancing");
            break;
        }
        }
        /* Get the initial load balancing sub transport to use */
        if (!self->mLoadBalanceInitialCb)
        {
            self->mLoadBalanceInitialCb = randomInitial;
        }
        self->mLoadBalanceInitialCb (self->mNumTransports,
                                     &self->mCurTransportIndex);

        if ((self->mCurTransportIndex) < 0  ||
            (self->mCurTransportIndex) > self->mNumTransports)
        {
            mama_log (MAMA_LOG_LEVEL_FINE,
                        "Initial load balancing sub transport [%d] "
                        "is not valid.",
                        self->mCurTransportIndex);
            return MAMA_STATUS_INVALID_ARG;
        }
        else
        {
                mama_log (MAMA_LOG_LEVEL_FINEST,
                        "Using initial load balance sub transport [%d] ",
                        self->mCurTransportIndex);
        }
    }

    /* if we have a load balancing function defined then create
       all of them as we don't know which one it will select
    */
    if (self->mLoadBalanceCb)
    {
        self->mTransports = calloc (self->mNumTransports,
                                    sizeof (transportBridge*));
        if (self->mTransports == NULL)
            return MAMA_STATUS_NOMEM;

        for (i = 0; i < self->mNumTransports; ++i)
        {
            snprintf (loadBalanceName, MAX_TPORT_NAME_LEN,
                    "%s.lb%d", self->mName, i);
            loadBalanceName[MAX_TPORT_NAME_LEN-1] = '\0';
            bridgeName = loadBalanceName;

            rval = self->mBridgeImpl->bridgeMamaTransportCreate
                                            (&self->mTransports[i],
                                             bridgeName,
                                             transport);

            if (rval != MAMA_STATUS_OK) return rval;

        }
    }
    /* Only create one at index 0, either because we are not using
       load balancing or because there is no per sub load balancing
       so only one sub tport will ever be used
    */
    else
    {
        self->mTransports = calloc (1, sizeof (transportBridge*));
        if (self->mTransports == NULL)
            return MAMA_STATUS_NOMEM;

        if (isLoadBalanced)
        {
            snprintf (loadBalanceName, MAX_TPORT_NAME_LEN,
                      "%s.lb%d", self->mName, self->mCurTransportIndex);
            loadBalanceName[MAX_TPORT_NAME_LEN-1] = '\0';
            bridgeName = loadBalanceName;
        }
        else
        {
            bridgeName = name;
        }
        self->mCurTransportIndex = 0;
        rval = self->mBridgeImpl->bridgeMamaTransportCreate (
                                        &self->mTransports[0],
                                        bridgeName,
                                        transport);
        if (rval != MAMA_STATUS_OK) return rval;

    }

    middleware = self->mBridgeImpl->bridgeGetName ();

    setGroupSizeHint (transport, middleware);

    rval = init (self, mamaTransportInternal_cmResponderEnabled (self, name, middleware));
    if (rval != MAMA_STATUS_OK) return rval;


    setPreInitialStrategy ((mamaTransport)self);
    setDQStrategy (self);
    setFtStrategy (self);

    if (mamaTransportImpl_disableDisconnectCb (name))
    {
        self->mDisableDisconnectCb = 1;
        mama_log (MAMA_LOG_LEVEL_FINE, "Disabled disconnect callbacks for %s\n",
                name);
    }

    if ((!self->mDisableRefresh) && (!mamaTransportInternal_disableRefreshes(name)))
    {
        return refreshTransport_create (&self->mRefreshTransport,
                                    (mamaTransport)self,
                                    self->mListeners,
                                    self->mBridgeImpl);
    }

    return MAMA_STATUS_OK;
}

mama_status
mamaTransport_initStats (mamaTransport transport)
{
    mama_status status = MAMA_STATUS_OK;
    const char* middleware = NULL;

    if (!self) return MAMA_STATUS_NULL_ARG;

    middleware = self->mBridgeImpl->bridgeGetName ();

    self->mStatsCollector = (mamaStatsCollector*) mamaStatsGenerator_allocateStatsCollector
                                                            (mamaInternal_getStatsGenerator ());

    if (MAMA_STATUS_OK != (status=mamaStatsCollector_create (
                               self->mStatsCollector,
                                MAMA_STATS_COLLECTOR_TYPE_TRANSPORT,
                                self->mName, middleware)))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaTransport_create (): "
                  "Could not create stats collector.");
        return status;
    }

    /* Stats are logged by default - requires explicit property not
           to log */
    if (!gLogTransportStats)
    {
        if (MAMA_STATUS_OK != (status=mamaStatsCollector_setLog (
                                    *self->mStatsCollector, 0)))
        {
            return status;
        }

        mama_log (MAMA_LOG_LEVEL_NORMAL, "Stats logging disabled for transport %s",
                  self->mName);
    }

    /* Stats are not published by default - requires explicit property
       to publish */
    if (gPublishTransportStats)
    {
        if (MAMA_STATUS_OK != (status=mamaStatsCollector_setPublish (
                                    *self->mStatsCollector, 1)))
        {
            return status;
        }

        mama_log (MAMA_LOG_LEVEL_NORMAL, "Stats publishing enabled for transport %s",
                  self->mName);
    }

    if (gGenerateTransportStats)
    {
        status = mamaStat_create (&self->mInitialStat,
                                  self->mStatsCollector,
                                  MAMA_STAT_LOCKABLE,
                                  MamaStatInitials.mName,
                                  MamaStatInitials.mFid);
        if (status != MAMA_STATUS_OK) return status;

        status = mamaStat_create (&self->mRecapStat,
                                  self->mStatsCollector,
                                  MAMA_STAT_LOCKABLE,
                                  MamaStatRecaps.mName,
                                  MamaStatRecaps.mFid);
        if (status != MAMA_STATUS_OK) return status;

        status = mamaStat_create (&self->mUnknownMsgStat,
                                  self->mStatsCollector,
                                  MAMA_STAT_LOCKABLE,
                                  MamaStatUnknownMsgs.mName,
                                  MamaStatUnknownMsgs.mFid);
        if (status != MAMA_STATUS_OK) return status;

        status = mamaStat_create (&self->mMessageStat,
                                  self->mStatsCollector,
                                  MAMA_STAT_LOCKABLE,
                                  MamaStatNumMessages.mName,
                                  MamaStatNumMessages.mFid);
        if (status != MAMA_STATUS_OK) return status;

        status = mamaStat_create (&self->mFtTakeoverStat,
                                  self->mStatsCollector,
                                  MAMA_STAT_LOCKABLE,
                                  MamaStatFtTakeovers.mName,
                                  MamaStatFtTakeovers.mFid);
        if (status != MAMA_STATUS_OK) return status;

        status = mamaStat_create (&self->mSubscriptionStat,
                                  self->mStatsCollector,
                                  MAMA_STAT_LOCKABLE,
                                  MamaStatNumSubscriptions.mName,
                                  MamaStatNumSubscriptions.mFid);
        if (status != MAMA_STATUS_OK) return status;

        status = mamaStat_create (&self->mTimeoutStat,
                                  self->mStatsCollector,
                                  MAMA_STAT_LOCKABLE,
                                  MamaStatTimeouts.mName,
                                  MamaStatTimeouts.mFid);
        if (status != MAMA_STATUS_OK) return status;

        status = mamaStat_create (&self->mWombatMsgsStat,
                                  self->mStatsCollector,
                                  MAMA_STAT_NOT_LOCKABLE,
                                  MamaStatWombatMsgs.mName,
                                  MamaStatWombatMsgs.mFid);
        if (status != MAMA_STATUS_OK) return status;

        status = mamaStat_create (&self->mFastMsgsStat,
                                  self->mStatsCollector,
                                  MAMA_STAT_NOT_LOCKABLE,
                                  MamaStatFastMsgs.mName,
                                  MamaStatFastMsgs.mFid);
        if (status != MAMA_STATUS_OK) return status;

        status = mamaStat_create (&self->mRvMsgsStat,
                                  self->mStatsCollector,
                                  MAMA_STAT_NOT_LOCKABLE,
                                  MamaStatRvMsgs.mName,
                                  MamaStatRvMsgs.mFid);
        if (status != MAMA_STATUS_OK) return status;
    }

    if (gGenerateLbmStats)
    {
        status = mamaStat_create (&self->mNakPacketsSent,
                                  self->mStatsCollector,
                                  MAMA_STAT_NOT_LOCKABLE,
                                  MamaStatNakPacketsSent.mName,
                                  MamaStatNakPacketsSent.mFid);
        if (status != MAMA_STATUS_OK) return status;

        status = mamaStat_create (&self->mNaksSent,
                                  self->mStatsCollector,
                                  MAMA_STAT_NOT_LOCKABLE,
                                  MamaStatNaksSent.mName,
                                  MamaStatNaksSent.mFid);
        if (status != MAMA_STATUS_OK) return status;

        status = mamaStat_create (&self->mMsgsLost,
                                  self->mStatsCollector,
                                  MAMA_STAT_NOT_LOCKABLE,
                                  MamaStatMsgsLost.mName,
                                  MamaStatMsgsLost.mFid);
        if (status != MAMA_STATUS_OK) return status;

        status = mamaStat_create (&self->mNcfsIgnore,
                                  self->mStatsCollector,
                                  MAMA_STAT_NOT_LOCKABLE,
                                  MamaStatNcfsIgnore.mName,
                                  MamaStatNcfsIgnore.mFid);
        if (status != MAMA_STATUS_OK) return status;

        status = mamaStat_create (&self->mNcfsShed,
                                  self->mStatsCollector,
                                  MAMA_STAT_NOT_LOCKABLE,
                                  MamaStatNcfsShed.mName,
                                  MamaStatNcfsShed.mFid);
        if (status != MAMA_STATUS_OK) return status;

        status = mamaStat_create (&self->mNcfsRxDelay,
                                  self->mStatsCollector,
                                  MAMA_STAT_NOT_LOCKABLE,
                                  MamaStatNcfsRxDelay.mName,
                                  MamaStatNcfsRxDelay.mFid);
        if (status != MAMA_STATUS_OK) return status;

        status = mamaStat_create (&self->mNcfsUnknown,
                                  self->mStatsCollector,
                                  MAMA_STAT_NOT_LOCKABLE,
                                  MamaStatNcfsUnknown.mName,
                                  MamaStatNcfsUnknown.mFid);
        if (status != MAMA_STATUS_OK) return status;

        status = mamaStat_create (&self->mDuplicateDataMsgs,
                                  self->mStatsCollector,
                                  MAMA_STAT_NOT_LOCKABLE,
                                  MamaStatDuplicateDataMsgs.mName,
                                  MamaStatDuplicateDataMsgs.mFid);
        if (status != MAMA_STATUS_OK) return status;

        status = mamaStat_create (&self->mUnrecoverableWindowAdvance,
                                  self->mStatsCollector,
                                  MAMA_STAT_NOT_LOCKABLE,
                                  MamaStatUnrecoverableWindowAdvance.mName,
                                  MamaStatUnrecoverableWindowAdvance.mFid);
        if (status != MAMA_STATUS_OK) return status;

        status = mamaStat_create (&self->mUnrecoverableTimeout,
                                  self->mStatsCollector,
                                  MAMA_STAT_NOT_LOCKABLE,
                                  MamaStatUnrecoverableTimeout.mName,
                                  MamaStatUnrecoverableTimeout.mFid);
        if (status != MAMA_STATUS_OK) return status;

        status = mamaStat_create (&self->mLbmMsgsReceivedNoTopic,
                                  self->mStatsCollector,
                                  MAMA_STAT_NOT_LOCKABLE,
                                  MamaStatLbmMsgsReceivedNoTopic.mName,
                                  MamaStatLbmMsgsReceivedNoTopic.mFid);
        if (status != MAMA_STATUS_OK) return status;

        status = mamaStat_create (&self->mLbmRequestsReceived,
                                  self->mStatsCollector,
                                  MAMA_STAT_NOT_LOCKABLE,
                                  MamaStatLbmRequestsReceived.mName,
                                  MamaStatLbmRequestsReceived.mFid);
        if (status != MAMA_STATUS_OK) return status;
    }

    if (mamaInternal_getStatsGenerator ())
    {
        if (MAMA_STATUS_OK != (
                status = mamaStatsGenerator_addStatsCollector (
                            mamaInternal_getStatsGenerator (),
                            self->mStatsCollector)))
        {
            mama_log (MAMA_LOG_LEVEL_ERROR, "mamaTransport_create (): "
                     "Could not add transport stats collector.");
            return status;
        }
    }

    return status;
}

/**
 * Destroy this TransportImpl.
 */
mama_status
mamaTransport_destroy (mamaTransport transport)
{
    int i;
    int allTransportsValid;
    mama_log(MAMA_LOG_LEVEL_FINER, "Entering mamaTransport_destroy for transport (%p)", transport);

    if (!self) return MAMA_STATUS_NULL_ARG;
    if (!self->mBridgeImpl) return MAMA_STATUS_NO_BRIDGE_IMPL;

    allTransportsValid = 1;
    for (i = 0; i < self->mNumTransports; i++)
    {
        if (!self->mBridgeImpl->bridgeMamaTransportIsValid (
                                                        self->mTransports[i]))
        {
            allTransportsValid = 0;
            break;
        }
    }
    if (allTransportsValid)
    {
        if(NULL != self->mCmResponder)
        {
            mamaCmResponder_destroy (self->mCmResponder);
        }
        /* Inform all listeners that the transport is about to be destroyed. */
        mamaTransportImpl_clearTransportWithListeners (self);

        /* Destroy the array of listeners. */
        list_destroy (self->mListeners, NULL, self);

        /* Inform all publishers. */
        mamaTransportImpl_clearTransportWithPublishers (self);

        /* Destroy the publisher list. */
        list_destroy (self->mPublishers, NULL, self);

        if (self->mRefreshTransport)
        {
            refreshTransport_destroy (self->mRefreshTransport);
            self->mRefreshTransport = NULL;
        }

        if (self->mThrottle)
        {
            wombatThrottle_destroy (self->mThrottle);
            self->mThrottle = 0;
        }

        if (self->mRecapThrottle)
        {
            wombatThrottle_destroy (self->mRecapThrottle);
            self->mRecapThrottle = 0;
        }

        if (self->mCmResponder)
        {
            mamaCmResponder_destroy (self->mCmResponder);

        }

        if (self->mLoadBalanceHandle)
        {
            closeSharedLib (self->mLoadBalanceHandle);
        }
    }

    for (i = 0; i < self->mNumTransports; i++)
    {
        self->mBridgeImpl->bridgeMamaTransportDestroy (self->mTransports[i]);
    }

    free (self->mTransports);

    if (self->mDescription) free ((void*)(self->mDescription));

    if (self->mInitialStat)
    {
        mamaStat_destroy (self->mInitialStat);
        self->mInitialStat = NULL;
    }

    if (self->mRecapStat)
    {
        mamaStat_destroy (self->mRecapStat);
        self->mRecapStat = NULL;
    }

     if (self->mUnknownMsgStat)
    {
        mamaStat_destroy (self->mUnknownMsgStat);
        self->mUnknownMsgStat = NULL;
    }

    if (self->mMessageStat)
    {
        mamaStat_destroy (self->mMessageStat);
        self->mMessageStat = NULL;
    }

    if (self->mFtTakeoverStat)
    {
        mamaStat_destroy (self->mFtTakeoverStat);
        self->mFtTakeoverStat = NULL;
    }

    if (self->mSubscriptionStat)
    {
        mamaStat_destroy (self->mSubscriptionStat);
        self->mSubscriptionStat = NULL;
    }

    if (self->mTimeoutStat)
    {
        mamaStat_destroy (self->mTimeoutStat);
        self->mSubscriptionStat = NULL;
    }

    if (self->mWombatMsgsStat)
    {
        mamaStat_destroy (self->mWombatMsgsStat);
        self->mWombatMsgsStat = NULL;
    }

    if (self->mFastMsgsStat)
    {
        mamaStat_destroy (self->mFastMsgsStat);
        self->mFastMsgsStat = NULL;
    }

    if (self->mRvMsgsStat)
    {
        mamaStat_destroy (self->mRvMsgsStat);
        self->mRvMsgsStat = NULL;
    }

    if (self->mNakPacketsSent)
    {
        mamaStat_destroy (self->mNakPacketsSent);
        self->mNakPacketsSent = NULL;
    }

    if (self->mNaksSent)
    {
        mamaStat_destroy (self->mNaksSent);
        self->mNaksSent = NULL;
    }

    if (self->mMsgsLost)
    {
        mamaStat_destroy (self->mMsgsLost);
        self->mMsgsLost = NULL;
    }

    if (self->mNcfsIgnore)
    {
        mamaStat_destroy (self->mNcfsIgnore);
        self->mNcfsIgnore = NULL;
    }

    if (self->mNcfsShed)
    {
        mamaStat_destroy (self->mNcfsShed);
        self->mNcfsShed = NULL;
    }

    if (self->mNcfsRxDelay)
    {
        mamaStat_destroy (self->mNcfsRxDelay);
        self->mNcfsShed = NULL;
    }

    if (self->mNcfsUnknown)
    {
        mamaStat_destroy (self->mNcfsUnknown);
        self->mNcfsUnknown = NULL;
    }

    if (self->mDuplicateDataMsgs)
    {
        mamaStat_destroy (self->mDuplicateDataMsgs);
        self->mDuplicateDataMsgs = NULL;
    }

    if (self->mUnrecoverableWindowAdvance)
    {
        mamaStat_destroy (self->mUnrecoverableWindowAdvance);
        self->mUnrecoverableWindowAdvance = NULL;
    }

    if (self->mUnrecoverableTimeout)
    {
        mamaStat_destroy (self->mUnrecoverableTimeout);
        self->mUnrecoverableTimeout = NULL;
    }

    if (self->mLbmMsgsReceivedNoTopic)
    {
        mamaStat_destroy (self->mLbmMsgsReceivedNoTopic);
        self->mLbmMsgsReceivedNoTopic = NULL;
    }

    if (self->mLbmRequestsReceived)
    {
        mamaStat_destroy (self->mLbmRequestsReceived);
        self->mLbmRequestsReceived = NULL;
    }

    if (self->mStatsCollector)
    {
        mamaStatsGenerator_removeStatsCollector  (mamaInternal_getStatsGenerator (), self->mStatsCollector);
        mamaStatsCollector_destroy (*self->mStatsCollector);
        self->mStatsCollector = NULL;
    }

    free (self);

    return MAMA_STATUS_OK;
}

mama_status
mamaTransport_setName (mamaTransport transport,
                       const char*   name)
{
    if (!self) return MAMA_STATUS_NULL_ARG;

    if (name)
    {
        snprintf (self->mName, MAX_TPORT_NAME_LEN, "%s", name);
        self->mName[MAX_TPORT_NAME_LEN-1] = '\0';
    }
    else
    {
        return MAMA_STATUS_NULL_ARG;
    }

    if (self->mStatsCollector)
    {
        mamaStatsCollector_setName (*self->mStatsCollector, self->mName);
    }

    return MAMA_STATUS_OK;
}

mama_status
mamaTransport_getName (mamaTransport transport,
                       const char**  result)
{
    if  (!self)
    {
        *result = NULL;
        return MAMA_STATUS_NULL_ARG;
    }
    *result = self->mName;
    return MAMA_STATUS_OK;
}

const char *
mamaTransport_getMiddleware (mamaTransport transport)
{
    if  (!self)
    {
        return NULL;
    }

    return (self->mBridgeImpl->bridgeGetName ());
}

/**
 * Get the outbound throttle in messages per second.
 *
 * @return The outbound throttle rate.
 */
mama_status
mamaTransport_getOutboundThrottle (mamaTransport transport,
                                   mamaThrottleInstance instance,
                                   double*       result)
{
    wombatThrottle throttle =
        mamaTransportImpl_getThrottle (transport, instance);
    *result = wombatThrottle_getRate (throttle);
    return MAMA_STATUS_OK;
}


mama_status
mamaTransport_getBridgeTransport (mamaTransport     transport,
                                  transportBridge*  result)
{
    *result = self->mTransports[0];
    return MAMA_STATUS_OK;
}

mama_status
mamaTransport_getBridgeTransportByIndex (mamaTransport    transport,
                                         int              index,
                                         transportBridge* result)
{
    *result = self->mTransports[index];
    return MAMA_STATUS_OK;
}

mama_status
mamaTransport_throttleAction (mamaTransport         transport,
                              mamaThrottleInstance  instance,
                              throttleCb            throttleAction,
                              void*                 owner,
                              void*                 closure1,
                              void*                 closure2,
                              int                   immediate,
                              wombatThrottleAction* action)
{
    wombatThrottle throttle =
        mamaTransportImpl_getThrottle (transport, instance);

    if (!self->mBridgeImpl) return MAMA_STATUS_NO_BRIDGE_IMPL;

    /* We use the closure as the owner so the ownder has a handle to remove
     * the action if necessary.
     */

    return wombatThrottle_dispatch (throttle,
                                    owner,
                                    throttleAction,
                                    closure1,
                                    closure2,
                                    immediate,
                                    action);
}

/**
 * Set the outbound throttle in messages per second.
 *
 * @param outboundThrottle
 */
mama_status
mamaTransport_setOutboundThrottle (mamaTransport transport,
                                   mamaThrottleInstance instance,
                                   double outboundThrottle)
{
    wombatThrottle throttle =
        mamaTransportImpl_getThrottle (transport, instance);
    wombatThrottle_setRate (throttle, outboundThrottle);
    return MAMA_STATUS_OK;
}


void
mamaTransport_setSymbolMapFunc (mamaTransport      transport,
                                mamaSymbolMapFunc  mapFunc,
                                void*              closure)
{
    self->mMapFunc = mapFunc;
    self->mMapFuncClosure = closure;
}

mamaSymbolMapFunc
mamaTransport_getSymbolMapFunc (mamaTransport  transport)
{
    return self->mMapFunc;
}

void*
mamaTransport_getSymbolMapFuncClosure (mamaTransport  transport)
{
    if (self)
    {
        return self->mMapFuncClosure;
    }
    else
    {
        return NULL;
    }
}

mama_status
mamaTransport_setDescription (mamaTransport transport,
                              const char* description)
{
    mama_status rval;

    if (!self) return MAMA_STATUS_NULL_ARG;

    if (self->mDescription)
    {
        free ((void*)(self->mDescription));
    }

    self->mDescription = strdup (description);

    rval = MAMA_STATUS_OK;
    return rval;
}

mama_status
mamaTransport_getDescription (mamaTransport transport,
                              const char** description)
{
    if (!self) return MAMA_STATUS_NULL_ARG;
    *description = self->mDescription;
    return MAMA_STATUS_OK;
}

mama_status
mamaTransport_setInvokeQualityForAllSubscs (
                                      mamaTransport transport,
                                      int           invokeQualityForAllSubscs)
{
    if (!self)
        return MAMA_STATUS_NULL_ARG;

    self->mSetPossiblyStaleForAll = invokeQualityForAllSubscs;

    return MAMA_STATUS_OK;
}

mama_status
mamaTransport_getInvokeQualityForAllSubscs (
                                 const mamaTransport transport,
                                 int*                invokeQualityForAllSubscs)
{
    if (!self)
        return MAMA_STATUS_NULL_ARG;
    if (!invokeQualityForAllSubscs)
        return MAMA_STATUS_INVALID_ARG;

    *invokeQualityForAllSubscs = self->mSetPossiblyStaleForAll;

    return MAMA_STATUS_OK;
}

mama_status
mamaTransport_getQuality (const mamaTransport transport,
                          mamaQuality*        quality)
{
    if (!self)
        return MAMA_STATUS_NULL_ARG;
    if (!quality)
        return MAMA_STATUS_INVALID_ARG;

    *quality = self->mQuality;

    return MAMA_STATUS_OK;
}

/*
 *  Protected method called internally by subscription.
 */
mama_status
mamaTransport_addSubscription (mamaTransport    transport,
                               mamaSubscription subscription,
                               void**           result)
{
    SubscriptionInfo*   handle = NULL;

    if (self->mRefreshTransport)
        handle = refreshTransport_allocateSubscInfo (self->mRefreshTransport);

    if (handle == NULL) return MAMA_STATUS_NOMEM;

    handle->mSubscription = subscription;

    *result = handle;

    if (self->mRefreshTransport)
        refreshTransport_addSubscription (self->mRefreshTransport, handle);

    return MAMA_STATUS_OK;
}


mama_status
mamaTransport_removeListener (mamaTransport transport,  void* handle)
{
    if (handle == 0) return MAMA_STATUS_OK;

    if (self->mRefreshTransport)
    {
        refreshTransport_removeListener (self->mRefreshTransport, handle, 1);
    }

    return MAMA_STATUS_OK;
}

mama_status
mamaTransport_throttleRemoveForOwner (mamaTransport transport,
                                      mamaThrottleInstance instance,
                                      void* owner)
{
    wombatThrottle throttle =
        mamaTransportImpl_getThrottle (transport, instance);
    wombatThrottle_removeMessagesForOwner (throttle, owner);

    return MAMA_STATUS_OK;
}

mama_status
mamaTransport_throttleRemoveFromList (mamaTransport transport,
                                      mamaThrottleInstance instance,
                                      wList list)
{
    wombatThrottle throttle =
        mamaTransportImpl_getThrottle (transport, instance);
    return wombatThrottle_removeMessagesFromList ( throttle, list);
}

const char*
mamaTransportEvent_toString (mamaTransportEvent event)
{
    switch ( event )
    {
        case MAMA_TRANSPORT_CONNECT:                return "CONNECT";
        case MAMA_TRANSPORT_CONNECT_FAILED:         return "CONNECT_FAILED";
        case MAMA_TRANSPORT_RECONNECT:              return "RECONNECT";
        case MAMA_TRANSPORT_DISCONNECT:             return "DISCONNECT";
        case MAMA_TRANSPORT_ACCEPT:                 return "ACCEPT";
        case MAMA_TRANSPORT_ACCEPT_RECONNECT:       return "ACCEPT_RECONNECT";
        case MAMA_TRANSPORT_PUBLISHER_DISCONNECT:   return "PUBLISHER_DISCONNECT";
        case MAMA_TRANSPORT_QUALITY:                return "QUALITY";
        case MAMA_TRANSPORT_NAMING_SERVICE_CONNECT: return "NAMING_SERVICE_CONNECT";
        case MAMA_TRANSPORT_NAMING_SERVICE_DISCONNECT: return "NAMING_SERVICE_DISCONNECT";
        case MAMA_TRANSPORT_WRITE_QUEUE_HIGH_WATER_MARK: return "MAMA_TRANSPORT_WRITE_QUEUE_HIGH_WATER_MARK";
        case MAMA_TRANSPORT_WRITE_QUEUE_LOW_WATER_MARK: return "MAMA_TRANSPORT_WRITE_QUEUE_LOW_WATER_MARK";
        default: return "UNKNOWN";
    }
}


mama_status
mamaTransport_setTransportCallback (mamaTransport transport,
                                    mamaTransportCB callback,
                                    void* closure)
{
    self->mTportCb      = callback;
    self->mTportClosure = closure;
    return MAMA_STATUS_OK;
}

mama_status
mamaTransport_setWriteQueueWatermarks (mamaTransport transport,
                                       uint32_t high,
                                       uint32_t low)
{
    if (!self)
        return MAMA_STATUS_NULL_ARG;
    if (high < low || low == 0)
        return MAMA_STATUS_INVALID_ARG;

    self->mWriteQueueHighWatermark = high;
    self->mWriteQueueLowWatermark = low;
    return MAMA_STATUS_OK;
}

void
mamaTransportImpl_getWriteQueueWatermarks (mamaTransport transport,
                                           uint32_t* high,
                                           uint32_t* low)
{
    *high = self->mWriteQueueHighWatermark;
    *low = self->mWriteQueueLowWatermark;
}

mama_status
mamaTransport_setTransportTopicCallback (mamaTransport transport,
                                         mamaTransportTopicCB callback,
                                         void* closure)
{
    self->mTportTopicCb      = callback;
    self->mTportTopicClosure = closure;
    return MAMA_STATUS_OK;
}

void
mamaTransportImpl_getTransportTopicCallback (mamaTransport transport,
                                             mamaTransportTopicCB* callback,
                                             void** closure)
{
    *closure = self->mTportTopicClosure;
    *callback = self->mTportTopicCb;
}

static void
setStaleListenerIterator (wList list, void* element, void* closure)
{
    SubscriptionInfo* subsc = (SubscriptionInfo*)element;
    if (mamaSubscription_isTportDisconnected (subsc->mSubscription))
            mamaSubscription_setPossiblyStale (subsc->mSubscription);
}

static void
setPossiblyStaleForListeners (transportImpl* transport)
{
    if (self->mRefreshTransport)
    {
        refreshTransport_iterateListeners (self->mRefreshTransport,
                                      setStaleListenerIterator, NULL);
    }
}

preInitialScheme
mamaTransportImpl_getPreInitialScheme (mamaTransport transport)
{
    if (self)
    {
        return self->mPreInitialScheme;
    }
    return PRE_INITIAL_SCHEME_ON_GAP;
}

dqStartegyScheme
mamaTransportImpl_getDqStrategyScheme (mamaTransport transport)
{
    if (self)
    {
        return self->mDQStratScheme;
    }
    return DQ_SCHEME_DELIVER_ALL;
}

dqftStrategyScheme
mamaTransportImpl_getFtStrategyScheme (mamaTransport transport)
{
    if (self)
    {
        return self->mFTStratScheme;
    }
    return DQ_FT_DO_NOT_WAIT_FOR_RECAP;
}
/* Process an advisory message and invokes callbacks
 *                    on all listeners.
 * @param transport The transport.
 * @param cause Cause for the advisory.
 * @param platformInfo Additional bridge specific information that will
 *                     be passed to all listeners.
 */
void
mamaTransportImpl_processAdvisory (mamaTransport transport,
                                   short         cause,
                                   const void*   platformInfo)
{
    if (!self)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                "mamaTransportImpl_processAdvisory (): Could not process.");
        return;
    }

    /* Save the platforminfo and the cause in member variables, this avoids
     * having to pass them around iterators when invoking callback functions
     * on the listeners.
     */
    self->mCause        = cause;
    self->mPlatformInfo = (void*)platformInfo;

    if (self->mSetPossiblyStaleForAll)
    {
        setPossiblyStaleForListeners (self);
        if ( self->mRefreshTransport )
            refreshTransport_startStaleRecapTimer (self->mRefreshTransport );
    }

    self->mQuality = MAMA_QUALITY_MAYBE_STALE;

    if (self->mTportCb != NULL)
    {
        self->mTportCb (transport, MAMA_TRANSPORT_QUALITY,
                        self->mCause, self->mPlatformInfo,
                        self->mTportClosure);
    }

    /* Clear the platforminfo and cause, these should not be used after this
     * point. */
    self->mCause        = 0;
    self->mPlatformInfo = NULL;
}

struct topicsForSourceClosure
{
    int curIdx;
    const char* source;
    const char** topics;
    mama_i32_t*  types;

    /* This is index of the sub transport bridge that the subscription is
     * listening on. */
    int transportIndex;
};

static void
topicsForSourceIterator (wList list, void* element, void* c)
{
    const char *subscSource = NULL;
    struct topicsForSourceClosure *closure = (struct topicsForSourceClosure*)c;
    SubscriptionInfo* subsc = (SubscriptionInfo*)element;

    /* Get the bridge transport index for this subscription. */
    int transportIndex = 0;
    mamaSubscription_getTransportIndex (subsc->mSubscription, &transportIndex);

    /* Only continue if the subscription info is using the same sub transport bridge. */
    if (transportIndex == closure->transportIndex)
    {
        /* Get the source for this subscription. */
        mamaSubscription_getSource (subsc->mSubscription, &subscSource);

        if ((closure->source == NULL && subscSource != NULL) ||
            strcmp (closure->source, subscSource) == 0       ||
            (closure->source == NULL && subscSource == NULL))
        {
            const char *topic = NULL;
            mamaSubscription_getSymbol (subsc->mSubscription, &topic);

            if (topic != NULL)
            {
                mamaSubscriptionType subscType = MAMA_SUBSC_TYPE_NORMAL;
                mamaSubscription_getSubscriptionType (subsc->mSubscription, &subscType);
                closure->types[closure->curIdx] = subscType;
                closure->topics[closure->curIdx++] = topic;
            }
        }
    }
}

mama_status
mamaTransportImpl_getTopicsAndTypesForSource (mamaTransport transport,
                                              int transportIndex,
                                              const char*   source,
                                              const char*** topics,
                                              mama_i32_t**  types,
                                              int *len)
{
    struct topicsForSourceClosure closure;
    int size = 0;

    /* This is the most time efficient, but least space efficient way to do
     * this. But the memory should not be allocated for very long.
     */
    if (self->mRefreshTransport)
        size = refreshTransport_numListeners (self->mRefreshTransport);

    closure.topics =
        (const char**) calloc (sizeof (char*), size);

    closure.types =
        (mama_i32_t*) calloc (sizeof (mama_i32_t), size);

    closure.curIdx = 0;
    closure.source = source;
    closure.transportIndex = transportIndex;

    if (self->mRefreshTransport)
    {
        refreshTransport_iterateListeners (self->mRefreshTransport,
                                      topicsForSourceIterator, &closure);
    }

    *topics = closure.topics;
    *types  = closure.types;
    *len = closure.curIdx;

    return MAMA_STATUS_OK;
}

void mamaTransportImpl_disconnectNoStale (mamaTransport      transport,
                                          mamaTransportEvent event,
                                          const void*        platformInfo,
                                          const void*        connectionInfo)
{
    if (!self)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaTransportImpl_disconnectNoStale (): "
                                 "Could not disconnect. NULL transport.");
        return;
    }

    self->mQuality = MAMA_QUALITY_MAYBE_STALE;

    if (!self->mDisableDisconnectCb && self->mTportCb != NULL )
    {
        self->mTportCb (transport, event,
                        self->mCause, connectionInfo,
                        self->mTportClosure);
    }
}

void mamaTransportImpl_disconnect (mamaTransport      transport,
                                   mamaTransportEvent event,
                                   const void*        platformInfo,
                                   const void*        connectionInfo)
{
    if (!self)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaTransportImpl_disconnect (): "
                                 "Could not disconnect. NULL transport.");
        return;
    }

    /* Save the platforminfo and the cause in member variables, this avoids
     * having to pass them around iterators when invoking callback functions
     * on the listeners.
     */
    self->mCause        = DQ_DISCONNECT;
    self->mPlatformInfo = (void*)platformInfo;


    if (self->mSetPossiblyStaleForAll)
    {
        setPossiblyStaleForListeners (self);

    }

    self->mQuality = MAMA_QUALITY_MAYBE_STALE;

    if (!self->mDisableDisconnectCb && self->mTportCb != NULL )
    {
        self->mTportCb (transport, event,
                        self->mCause, connectionInfo,
                        self->mTportClosure);
    }

    /* Clear the platforminfo and cause, these should not be used after this point. */
    self->mCause        = 0;
    self->mPlatformInfo = NULL;
}

void mamaTransportImpl_reconnect (mamaTransport      transport,
                                  mamaTransportEvent event,
                                  const void*        platformInfo,
                                  const void*        connectionInfo)
{
    if (self->mTportCb != NULL)
    {
        self->mTportCb (transport, event,
                            self->mCause, connectionInfo,
                            self->mTportClosure);
    }
}

void
mamaTransportImpl_resetRefreshForListener (mamaTransport transport,
                                           void*         handle)
{
    if (self->mRefreshTransport)
    {
        refreshTransport_resetRefreshForListener (self->mRefreshTransport,
                                                  handle);
    }
}

mama_status
mamaTransport_findConnection (mamaTransport   transport,
                              mamaConnection* result,
                              const char*     ipAddress,
                              uint16_t        port)
{
    if (!self) return MAMA_STATUS_NULL_ARG;
    if (!result)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaTransport_findConnection (): "
                                         "Invalid result address.");
        return MAMA_STATUS_INVALID_ARG;
    }

    return self->mBridgeImpl->bridgeMamaTransportFindConnection (
                                    self->mTransports,
                                    self->mNumTransports,
                                    result,
                                    ipAddress,
                                    port);
}

mama_status
mamaTransportImpl_forceClientDisconnect (mamaTransport transport,
                                         const char*   ipAddress,
                                         uint16_t      port)
{
    if (!self || ipAddress == NULL) return MAMA_STATUS_NULL_ARG;

    return self->mBridgeImpl->bridgeMamaTransportForceClientDisconnect (
                                              self->mTransports,
                                              self->mNumTransports,
                                              ipAddress,
                                              port);
}

mama_status
mamaTransport_getAllConnections (mamaTransport    transport,
                                 mamaConnection** result,
                                 uint32_t*        len)
{
    if (!self) return MAMA_STATUS_NULL_ARG;
    if (!result)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaTransport_getAllConnections (): "
                                         "Invalid connections address.");
        return MAMA_STATUS_INVALID_ARG;
    }

    return self->mBridgeImpl->bridgeMamaTransportGetAllConnections (
                                              self->mTransports,
                                              self->mNumTransports,
                                              result,
                                              len);
}

mama_status
mamaTransport_getAllConnectionsForTopic (mamaTransport    transport,
                                         const char*      topic,
                                         mamaConnection** result,
                                         uint32_t*        len)
{
    if (!self) return MAMA_STATUS_NULL_ARG;
    if (!result)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaTransport_getAllConnectionsForTopic (): "
                                         "Invalid connections address.");
        return MAMA_STATUS_INVALID_ARG;
    }

    return self->mBridgeImpl->bridgeMamaTransportGetAllConnectionsForTopic (
                                              self->mTransports,
                                              self->mNumTransports,
                                              topic,
                                              result,
                                              len);
}

extern mama_status
mamaTransport_freeAllConnections (mamaTransport   transport,
                                  mamaConnection* connections,
                                  uint32_t        len)
{
    if (!self) return MAMA_STATUS_NULL_ARG;
    if (!connections)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaTransport_freeAllConnections (): "
                                         "Invalid connections address.");
        return MAMA_STATUS_INVALID_ARG;
    }

    return self->mBridgeImpl->bridgeMamaTransportFreeAllConnections (
                                              self->mTransports,
                                              self->mNumTransports,
                                              connections,
                                              len);
}

mama_status
mamaTransport_getAllServerConnections (mamaTransport          transport,
                                       mamaServerConnection** result,
                                       uint32_t*              len)
{
    if (!self) return MAMA_STATUS_NULL_ARG;
    if (!result)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaTransport_getAllConnections (): "
                                         "Invalid result address.");
        return MAMA_STATUS_INVALID_ARG;
    }

    return self->mBridgeImpl->bridgeMamaTransportGetAllServerConnections (
                                        self->mTransports,
                                        self->mNumTransports,
                                        result,
                                        len);
}

mama_status
mamaTransport_freeAllServerConnections (mamaTransport         transport,
                                        mamaServerConnection* connections,
                                        uint32_t              len)
{
    if (!self) return MAMA_STATUS_NULL_ARG;
    if (!connections)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaTransport_getAllConnections (): "
                                         "Invalid connections address.");
        return MAMA_STATUS_INVALID_ARG;
    }

    return self->mBridgeImpl->bridgeMamaTransportFreeAllServerConnections (
                                        self->mTransports,
                                        self->mNumTransports,
                                        connections,
                                        len);
}

int
mamaTransportImpl_isPossiblyStale (mamaTransport transport)
{
    return self->mQuality == MAMA_QUALITY_MAYBE_STALE;
}


void
mamaTransportImpl_unsetAllPossiblyStale (mamaTransport transport)
{
    if (!self)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                "mamaTransportImpl_unsetAllPossiblyStale (): "
                "Invalid transport: NULL");
        return;
    }

    if (self->mQuality != MAMA_QUALITY_OK)
    {
        self->mQuality = MAMA_QUALITY_OK;

        if (self->mTportCb != NULL)
        {
            self->mTportCb (transport, MAMA_TRANSPORT_QUALITY,
                            self->mCause, self->mPlatformInfo,
                            self->mTportClosure);
        }
    }
}

/**
 * Return the cause and platform info for the last message processed on the
 * transport.
 *
 * @param transport       The transport.
 * @param cause           To return the cause.
 * @param platformInfo    To return the bridge specific info, under no circumstances
 *                                        should the returned object be deleted.
 */
void
mamaTransportImpl_getAdvisoryCauseAndPlatformInfo (mamaTransport transport,
                                                   short*        cause,
                                                   const void**  platformInfo)
{
    if (!self)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                "mamaTransportImpl_getAdvisoryCauseAndPlatformInfo (): "
                "NULL transport.");
        return;
    }

    *cause  = self->mCause;
    *platformInfo = self->mPlatformInfo;
}

void mamaTransportImpl_setAdvisoryCauseAndPlatformInfo (mamaTransport transport, short cause, const void *platformInfo)
{
    /* Get the impl. */
    transportImpl *impl = (transportImpl *)transport;
    if(NULL != impl)
    {
        /* Set the cause. */
        impl->mCause        = cause;
        impl->mPlatformInfo = platformInfo;
    }

    /* Otherwise write an error log. */
    else
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaTransportImpl_setAdvisoryCauseAndPlatformInfo(): NULL transport.");
    }
}
void
mamaTransportImpl_getTransportIndex (mamaTransport transport,
                                     int*          transportIndex)
{
    *transportIndex = self->mCurTransportIndex;
}

mama_status
mamaTransport_setLbInitialCallback (mamaTransport            transport,
                                    mamaTransportLbInitialCB callback)
{
    self->mLoadBalanceInitialCb = callback;
    return MAMA_STATUS_OK;
}

mama_status
mamaTransport_setLbCallback (mamaTransport     transport,
                             mamaTransportLbCB callback)
{
    self->mLoadBalanceCb = callback;
    return MAMA_STATUS_OK;
}

mama_status
mamaTransport_getNativeTransport (mamaTransport transport,
                                  int index,
                                  void** nativeTport)
{
    if (!self) return MAMA_STATUS_NULL_ARG;
    if (!nativeTport)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaTransport_getNativeTransport (): "
                                         "Invalid result address.");
        return MAMA_STATUS_INVALID_ARG;
    }
    if (!(index < self->mNumTransports))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaTransport_getNativeTransport (): "
                                         "Invalid index.");
        return MAMA_STATUS_INVALID_ARG;
    }

    return self->mBridgeImpl->bridgeMamaTransportGetNativeTransport (
                                    self->mTransports[index],
                                    nativeTport);
}

mama_status
mamaTransport_getNativeTransportNamingCtx (mamaTransport transport,
                                           int index,
                                           void** nativeTportNamingCtx)
{
    if (!self) return MAMA_STATUS_NULL_ARG;
    if (!nativeTportNamingCtx)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaTransport_getNativeTransportNamingCtx (): "
                                         "Invalid result address.");
        return MAMA_STATUS_INVALID_ARG;
    }
    if (!(index < self->mNumTransports))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaTransport_getNativeTransportNamingCtx (): "
                                         "Invalid index.");
        return MAMA_STATUS_INVALID_ARG;
    }

    return self->mBridgeImpl->bridgeMamaTransportGetNativeTransportNamingCtx (
                                    self->mTransports[index],
                                    nativeTportNamingCtx);
}


mamaStatsCollector*
mamaTransport_getStatsCollector (mamaTransport transport)
{
    if (!self) return 0;
    return self->mStatsCollector;
}


static void
roundRobin (int         curTransportIndex,
            int         numTransports,
            const char* source,
            const char* symbol,
            int*        nextTransportIndex)
{
    *nextTransportIndex = curTransportIndex + 1;
    if (*nextTransportIndex == numTransports)
    {
        *nextTransportIndex = 0;
    }
}


static void
randomInitial (int  numTransports,
              int* transportIndex)

{
    struct timeval tod;
    gettimeofday (&tod, NULL);
    *transportIndex = (int)((tod.tv_usec/1000)  % numTransports);
}

void
mamaTransportImpl_nextTransportIndex (mamaTransport transport,
                                      const char*   source,
                                      const char*   symbol)
{
    /* if there is a callback registered then use that, otherwise
       do nothing, i.e. the next transport will be equal to the
       current transport
    */
    if (self->mLoadBalanceCb)
    {
        self->mLoadBalanceCb (self->mCurTransportIndex, self->mNumTransports,
                              source, symbol,
                              &self->mCurTransportIndex);
    }
}

wombatThrottle
mamaTransportImpl_getThrottle (mamaTransport transport,
                               mamaThrottleInstance instance)
{
    switch (instance)
    {
    case MAMA_THROTTLE_RECAP  :
        return self->mRecapThrottle;
    case MAMA_THROTTLE_DEFAULT:
    case MAMA_THROTTLE_INITIAL:
    default                   :
        return self->mThrottle;
    }
}

mamaBridgeImpl*
mamaTransportImpl_getBridgeImpl (mamaTransport transport)
{
    if (!self) return NULL;
    return self->mBridgeImpl;
}

int
mamaTransportImpl_getGroupSizeHint (mamaTransport transport)
{
    if (!self) return 0;
    return self->mGroupSizeHint;
}

mama_status
mamaTransportImpl_isConnectionIntercepted (mamaTransport  transport,
                                           mamaConnection connection,
                                           uint8_t*       result)
{
     if (!self) return MAMA_STATUS_NULL_ARG;
     /* TODO should this work for multiple LB tports ? */
     return self->mBridgeImpl->bridgeMamaTransportIsConnectionIntercepted (
                            connection,
                            result);

}

mama_status
mamaTransportImpl_installConnectConflateMgr (mamaTransport         transport,
                                             mamaConflationManager mgr,
                                             mamaConnection        connection,
                                             conflateProcessCb     processCb,
                                             conflateGetMsgCb      msgCb)
{
     if (!self) return MAMA_STATUS_NULL_ARG;
    /* TODO should this work for multiple LB tports ? */
     return self->mBridgeImpl->bridgeMamaTransportInstallConnectConflateMgr (
                            self->mTransports[0],
                            mgr,
                            connection,
                            processCb,
                            msgCb);
}

mama_status
mamaTransportImpl_uninstallConnectConflateMgr (
                                        mamaTransport         transport,
                                        mamaConflationManager mgr,
                                        mamaConnection        connection)
{
     if (!self) return MAMA_STATUS_NULL_ARG;
    /* TODO should this work for multiple LB tports ? */
     return self->mBridgeImpl->bridgeMamaTransportUninstallConnectConflateMgr (
                            self->mTransports[0],
                            mgr,
                            connection);
}

mama_status
mamaTransportImpl_startConnectionConflation (
                                        mamaTransport          transport,
                                        mamaConflationManager  mgr,
                                        mamaConnection         connection)
{
     if (!self) return MAMA_STATUS_NULL_ARG;
    /* TODO should this work for multiple LB tports ? */
     return self->mBridgeImpl->bridgeMamaTransportStartConnectionConflation (
                            self->mTransports[0],
                            mgr,
                            connection);
}

mama_status
mamaTransportImpl_sendMsgToConnection (mamaTransport   transport,
                                       mamaConnection  connection,
                                       mamaMsg         msg,
                                       const char*     topic)
{
     if (!self) return MAMA_STATUS_NULL_ARG;
    /* TODO should this work for multiple LB tports ? */
     return self->mBridgeImpl->bridgeMamaTransportSendMsgToConnection (
                            self->mTransports[0],
                            connection,
                            msg,
                            topic);
}

mama_status
mamaTransport_requestConflation (mamaTransport transport)
{
    return self->mBridgeImpl->bridgeMamaTransportRequestConflation (
                                            self->mTransports,
                                            self->mNumTransports);
}


mama_status
mamaTransport_requestEndConflation (mamaTransport transport)
{
    return self->mBridgeImpl->bridgeMamaTransportRequestEndConflation (
                                            self->mTransports,
                                            self->mNumTransports);
}


mama_status
mamaTransport_setDeactivateSubscriptionOnError (mamaTransport transport,
                                                    int deactivate)
{
    if (!self) return MAMA_STATUS_NULL_ARG;
    self->mDeactivateSubscriptionOnError = deactivate;
    return MAMA_STATUS_OK;
}

int
mamaTransport_getDeactivateSubscriptionOnError (mamaTransport transport)
{
    if (!self) return 0;
    return self->mDeactivateSubscriptionOnError;
}

/* *************************************************** */
/* Internal Functions. */
/* *************************************************** */

void mamaTransportImpl_invokeTransportCallback (mamaTransport transport,
                                                mamaTransportEvent event,
                                                short cause,
                                                const void *platformInfo)
{
    if (!self)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                "mamaTransportImpl_invokeTransportCallback (): "
                "Could not process.");
    }

    else
    {
        /* Only continue if the callback is valid. */
        if (self->mTportCb != NULL)
        {
            /* Save the platforminfo and the cause in member variables, this
             * avoids having to pass them around iterators when invoking
             * callback functions on the listeners.
             */
            self->mCause        = cause;
            self->mPlatformInfo = (void*)platformInfo;

            /* Invoke the callback. */
            self->mTportCb (transport, event, self->mCause, self->mPlatformInfo, self->mTportClosure);

            /* Clear the platforminfo and cause, these should not be used after
             * this point. */
            self->mCause        = 0;
            self->mPlatformInfo = NULL;
        }
    }
}

/* *************************************************** */
/* Private Functions. */
/* *************************************************** */

void mamaTransportImpl_clearTransportCallback (wList list,
                                               void *element,
                                               void *closure)
{
    /* The element passed in is the subscription information structure. */
    SubscriptionInfo *info = (SubscriptionInfo *)element;
    if (NULL != info)
    {
        /* Clear the transport pointers inside the subscription. */
        mamaSubscription_clearTransport (info->mSubscription);
    }
}

void mamaTransportImpl_clearTransportPublisherCallback (wList list,
                                                        void *element,
                                                        void *closure)
{
    /* The element passed in is the publisher info. */
    PublisherInfo *info = (PublisherInfo *)element;
    if (NULL != info)
    {
        /* Clear the transport pointers inside the subscription. */
        mamaPublisherImpl_clearTransport (info->mPublisher);
    }
}

void mamaTransportImpl_clearTransportWithListeners (transportImpl *impl)
{

    /* The refresh transport may still not be valid. */
    if (NULL != impl->mRefreshTransport)
    {
        refreshTransport_iterateListeners (impl->mRefreshTransport,
                mamaTransportImpl_clearTransportCallback, NULL);
    }
}

void mamaTransportImpl_clearTransportWithPublishers (transportImpl *impl)
{
    /* Iterate the list of publishers. */
    list_for_each (impl->mPublishers,
            mamaTransportImpl_clearTransportPublisherCallback, NULL);
}


mama_status mamaTransport_addPublisher (mamaTransport transport,
                                        mamaPublisher publisher,
                                        void **result)
{
    PublisherInfo*   handle = NULL;

    handle = (PublisherInfo*)list_allocate_element (self->mPublishers);


    if (handle == NULL) return MAMA_STATUS_NOMEM;

    handle->mPublisher = publisher;

    *result = handle;

    list_push_back (self->mPublishers, handle);

    return MAMA_STATUS_OK;
}

mama_status mamaTransport_removePublisher (mamaTransport transport, void *handle)
{
    if (NULL != handle)
    {
        list_remove_element (self->mPublishers, handle);
        list_free_element (self->mPublishers, handle);
    }

    return MAMA_STATUS_OK;
}

mama_status mamaTransportImpl_allocateInternalTransport(mamaTransport *transport)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;

    if(NULL != transport)
    {
        /* Allocate the transport as normal. */
        ret = mamaTransport_allocate(transport);
        if(MAMA_STATUS_OK == ret)
        {
            /* Get the impl. */
            transportImpl *impl = (transportImpl *)*transport;

            /* Set the internal flag. */
            impl->mInternal = 1;
        }
    }
    return ret;
}
