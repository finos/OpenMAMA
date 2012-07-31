/* $Id: statslogger.c,v 1.1.2.6 2011/10/10 16:03:18 emmapollock Exp $
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

/************************************************************************
* Includes
*************************************************************************/
#include <string.h>
#include <mama/mama.h>
#include <mama/statslogger.h>
#include <mama/statsloggerfields.h>
#include "mama/msgqualifier.h"
#include <mama/statfields.h>
#include <statsgeneratorinternal.h>
#include <subscriptionimpl.h>
#include <stdio.h>
#include <wlock.h>
#include <mama/msg.h>
#include <wombat/wincompat.h>

/************************************************************************
* Constants
*************************************************************************/
#define SL_EVENT_ARRAY_MAX_SIZE 1
#define SL_INTERFACE_VERSION 1

/************************************************************************
* Typedefs
*************************************************************************/
typedef struct mamaStatsLoggerImpl_
{
    mamaQueue          mQueue;
    mamaDispatcher     mDispatcher;
    mamaTransport      mTport;
    mamaPublisher      mPublisher;

    mama_u8_t          mVersion;
    const char*        mUserName;
    const char*        mHostName;
    const char*        mIpAddress;
    const char*        mAppName;
    const char*        mAppClass;
    mama_bool_t        mIsDestroyed;
    mama_bool_t        mIsConnected;       /* Are we connected to the server */
    mamaDateTime       mTimeStamp;         /* Reuseable date time object */
    mamaDateTime       mStartTime;

    mama_size_t        mEventArraySize;    /* The current size of event array*/
    mama_size_t        mEventArrayMaxSize; /* The max size of the event array*/
    mamaMsg*           mEventArray;
    wLock              mLock;

    mamaMsg            mReportMsg;         /* Reuseable msg to send reports */
    mamaMsg            mStatMsg;           /* Reusable msg for stats */
    mama_size_t        mReportSize;        /* The preferred max report size */
    mama_f64_t         mReportInterval;    /* Interval for the report to cover */
    mamaDateTime       mReportStartTime;
    mamaTimer          mReportTimer;

    mama_bool_t        mLogMsgStats;

    mama_u64_t         mTotalMsgCount;
    mama_u64_t         mPreviousMsgCount;

    const char*        mEventFieldName;     /* These can either use the MamaSlEvents */
    mama_fid_t         mEventFieldFid;      /* or MamaStatEvents field */
} mamaStatsLoggerImpl;

/************************************************************************
* Local declarations
*************************************************************************/
static
mama_status mamaStatsLoggerImpl_setString (
                const mamaStatsLoggerImpl* impl,
                const char**               attribute,
                const char*                newValue);

static
mama_status mamaStatsLoggerImpl_newEventMsg (
                const mamaStatsLoggerImpl*  impl,
                mamaSlEvent                 eventType,
                mamaMsg*                    eventMsg);

static
mama_status mamaStatsLoggerImpl_addSubDetails (
                const mamaStatsLoggerImpl*  impl,
                mamaSubscription            subscription,
                mamaMsg                     eventMsg,
                int32_t                     entitleCode);

static
mama_status mamaStatsLoggerImpl_addEvent (
                mamaStatsLoggerImpl*  impl,
                mamaMsg               eventMsg);

static
mama_status mamaStatsLoggerImpl_resetReport (
                mamaStatsLoggerImpl*  impl);

static
mama_status mamaStatsLoggerImpl_sendReport (
                mamaStatsLoggerImpl*  impl);

static
void mamaStatsLoggerImpl_clearEventArray (
                mamaStatsLoggerImpl* impl);


static void MAMACALLTYPE
mamaStatsLoggerImpl_onReportTimer (
                mamaTimer timer,
                void* closure);

/************************************************************************
* 'static' functions
*************************************************************************/

const char*
mamaStatsLogger_eventToString (mamaSlEvent event)
{
    switch (event)
    {
        case MAMA_SL_EVENT_LOGIN:
            return "LOGIN";
        case MAMA_SL_EVENT_LOGOUT:
            return "LOGOUT";
        case MAMA_SL_EVENT_SUB_CREATE:
            return "SUB_CREATE";
        case MAMA_SL_EVENT_SUB_FAIL:
            return "SUB_FAIL";
        case MAMA_SL_EVENT_SUB_DESTROY:
            return "SUB_DESTROY";
        case MAMA_SL_EVENT_SUB_ENT_PASS:
            return "SUB_ENTITLE_PASS";
        default:
            return "UNKNOWN";
    }
}

const char*
mamaStatsLogger_subFailReasonToString (mamaSlSubFailReason subFailReason)
{
    switch (subFailReason)
    {
        case MAMA_SL_SUB_FAIL_REGEX:
            return "REGEX";
        case MAMA_SL_SUB_FAIL_CODE:
            return "CODE";
        case MAMA_SL_SUB_FAIL_TIMEOUT:
            return "TIMEOUT";
        case MAMA_SL_SUB_FAIL_NOT_FOUND:
            return "NOT_FOUND";
        default:
            return "UNKNOWN";
    }
}

/************************************************************************
* Public methods
*************************************************************************/
mama_status
mamaStatsLogger_allocate (mamaStatsLogger*  logger)
{
    mamaStatsLoggerImpl* impl  =
                (mamaStatsLoggerImpl*) calloc (1, sizeof(mamaStatsLoggerImpl));
    if (impl == NULL) return MAMA_STATUS_NOMEM;

    *logger = (mamaStatsLogger)impl;

    impl->mReportSize           = 100;
    impl->mReportInterval       = 60.0;
    impl->mEventArraySize       = 0;
    impl->mEventArrayMaxSize    = SL_EVENT_ARRAY_MAX_SIZE;
    impl->mIsConnected          = 1;
    impl->mLock                 = wlock_create ();
    impl->mIsDestroyed          = 0;
    impl->mVersion              = SL_INTERFACE_VERSION;
    impl->mUserName             = NULL;
    impl->mHostName             = NULL;
    impl->mIpAddress            = NULL;
    impl->mAppName              = NULL;
    impl->mAppClass             = NULL;
    impl->mPreviousMsgCount     = 0;
    impl->mTotalMsgCount        = 0;
    impl->mLogMsgStats          = 0;

    return MAMA_STATUS_OK;
}

mama_status
mamaStatsLogger_setReportInterval (mamaStatsLogger logger,
                                   mama_f64_t      interval)
{
    mamaStatsLoggerImpl* impl = (mamaStatsLoggerImpl*)logger;
    if (!impl) return MAMA_STATUS_NULL_ARG;

    if (0 >= interval)
        return MAMA_STATUS_INVALID_ARG;

    impl->mReportInterval = interval;
    return MAMA_STATUS_OK;
}

mama_status
mamaStatsLogger_setReportSize (mamaStatsLogger  logger,
                               mama_size_t      numEvents)
{
    mamaStatsLoggerImpl* impl = (mamaStatsLoggerImpl*)logger;
    if (!impl) return MAMA_STATUS_NULL_ARG;

    if (0 <= numEvents)
        return MAMA_STATUS_INVALID_ARG;

    impl->mReportSize = numEvents;

    return MAMA_STATUS_OK;
}

mama_status
mamaStatsLogger_create (mamaStatsLogger  logger,
                        mamaQueue        queue,
                        mamaTransport    tport)
{
    mama_status status = MAMA_STATUS_OK;
    mamaStatsLoggerImpl* impl = (mamaStatsLoggerImpl*)logger;
    if (!impl) return MAMA_STATUS_NULL_ARG;

    if (!(impl->mQueue = queue))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mamaStatsLogger_create(): "
                  "No queue specified");
        return MAMA_STATUS_NULL_ARG;
    }

    if (!(impl->mTport = tport))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mamaStatsLogger_create(): "
                  "NULL transport");
        return MAMA_STATUS_NULL_ARG;
    }

    status = mamaDateTime_create   (&impl->mTimeStamp);
    if (MAMA_STATUS_OK != status) return status;

    status = mamaDateTime_create   (&impl->mStartTime);
    if (MAMA_STATUS_OK != status) return status;

    status = mamaDateTime_create   (&impl->mReportStartTime);
    if (MAMA_STATUS_OK != status) return status;

    status = mamaDateTime_setToNow (impl->mStartTime);
    if (MAMA_STATUS_OK != status) return status;

    mamaDateTime_copy (impl->mReportStartTime, impl->mStartTime);

    status = mamaMsg_create(&impl->mReportMsg);
    if (MAMA_STATUS_OK != status) return status;

    status = mamaMsg_create (&impl->mStatMsg);
    if (MAMA_STATUS_OK != status) return status;

    status = mamaPublisher_create (&impl->mPublisher,
                                   impl->mTport,
                                   SL_TOPIC_USAGE_LOG,
                                   NULL,  /*Source */
                                   NULL); /* Root */
    if (MAMA_STATUS_OK != status)
    {
        mama_log (MAMA_LOG_LEVEL_FINE,
                  "mamaStatsLogger_create(): "
                  "Failed to create stats logger publisher [%s]",
                  SL_TOPIC_USAGE_LOG);
        return status;
    }

    status = mamaTimer_create (&impl->mReportTimer,
                               impl->mQueue,
                               mamaStatsLoggerImpl_onReportTimer,
                               impl->mReportInterval,
                               impl);

    impl->mEventFieldName = MamaSlEvents.mName;
    impl->mEventFieldFid = MamaSlEvents.mFid;

    impl->mEventArray = (mamaMsg*)
                         calloc (impl->mEventArrayMaxSize, (sizeof (mamaMsg)));

    if  (!impl->mEventArray)
        return MAMA_STATUS_NOMEM;

    status = mamaStatsLoggerImpl_resetReport (impl);
    if (MAMA_STATUS_OK != status) return status;

    return MAMA_STATUS_OK;
}

mama_status
mamaStatsLogger_createForStats (mamaStatsLogger  logger,
                                mamaQueue        queue,
                                mamaTransport    tport,
                                const char*      topic)
{
    mama_status status = MAMA_STATUS_OK;
    mamaStatsLoggerImpl* impl = (mamaStatsLoggerImpl*)logger;

    if (!impl) return MAMA_STATUS_NULL_ARG;

    if (!(impl->mQueue = queue))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mamaStatsLogger_create(): "
                  "No queue specified");
        return MAMA_STATUS_NULL_ARG;
    }

    if (!(impl->mTport = tport))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mamaStatsLogger_create(): "
                  "NULL transport");
        return MAMA_STATUS_NULL_ARG;
    }

    status = mamaDateTime_create   (&impl->mTimeStamp);
    if (MAMA_STATUS_OK != status) return status;

    status = mamaDateTime_create   (&impl->mStartTime);
    if (MAMA_STATUS_OK != status) return status;

    status = mamaDateTime_create   (&impl->mReportStartTime);
    if (MAMA_STATUS_OK != status) return status;

    status = mamaDateTime_setToNow (impl->mStartTime);
    if (MAMA_STATUS_OK != status) return status;

    mamaDateTime_copy (impl->mReportStartTime, impl->mStartTime);

    status = mamaMsg_create (&impl->mReportMsg);
    if (MAMA_STATUS_OK != status) return status;

    status = mamaMsg_create (&impl->mStatMsg);
    if (MAMA_STATUS_OK != status) return status;

    status = mamaPublisher_create (&impl->mPublisher,
                                   impl->mTport,
                                   topic,
                                   NULL,  /*Source */
                                   NULL); /* Root */
    if (status != MAMA_STATUS_OK)
    {
        mama_log (MAMA_LOG_LEVEL_FINE,
                  "mamaStatsLogger_create(): "
                  "Failed to create stats logger publisher [%s]",
                  SL_TOPIC_USAGE_LOG);
        return status;
    }

    impl->mEventFieldName = MamaStatEvents.mName;
    impl->mEventFieldFid = MamaStatEvents.mFid;

    impl->mEventArray = (mamaMsg*)
                         calloc (impl->mEventArrayMaxSize, (sizeof (mamaMsg)));

    if  (!impl->mEventArray)
        return MAMA_STATUS_NOMEM;

    status = mamaStatsLoggerImpl_resetReport (impl);
    if (MAMA_STATUS_OK != status) return status;

    return MAMA_STATUS_OK;
}

mama_status
mamaStatsLogger_destroy (mamaStatsLogger logger)
{
    mamaStatsLoggerImpl* impl = (mamaStatsLoggerImpl*)logger;
    if (!impl) return MAMA_STATUS_NULL_ARG;

    /* Don't complain if called more than once */
    if (impl->mIsDestroyed)
        return MAMA_STATUS_OK;
    /* publish any waiting events */
    wlock_lock (impl->mLock);
    mamaStatsLoggerImpl_sendReport (impl);
    wlock_unlock (impl->mLock);

    if (impl->mPublisher)
    {
        mamaPublisher_destroy (impl->mPublisher);
        impl->mPublisher = NULL;
    }

    if (impl->mStartTime)
    {
        mamaDateTime_destroy (impl->mStartTime);
        impl->mStartTime = NULL;
    }

    if (impl->mTimeStamp)
    {
        mamaDateTime_destroy (impl->mTimeStamp);
        impl->mTimeStamp = NULL;
    }

    if (impl->mReportStartTime)
    {
        mamaDateTime_destroy (impl->mReportStartTime);
        impl->mReportStartTime = NULL;
    }

    if (impl->mLock)
    {
        wlock_destroy (impl->mLock);
        impl->mLock = NULL;
    }

    if (impl->mDispatcher)
    {
        mamaDispatcher_destroy (impl->mDispatcher);
        impl->mDispatcher = NULL;
    }

    if (impl->mReportMsg)
    {
        mamaMsg_destroy (impl->mReportMsg);
        impl->mReportMsg = NULL;
    }

    if (impl->mStatMsg)
    {
        mamaMsg_destroy (impl->mStatMsg);
        impl->mStatMsg = NULL;
    }

    free ((char*)impl->mUserName);  impl->mUserName = NULL;
    free ((char*)impl->mHostName);  impl->mHostName = NULL;
    free ((char*)impl->mIpAddress); impl->mIpAddress = NULL;
    free ((char*)impl->mAppName);   impl->mAppName = NULL;
    free ((void*)impl->mAppClass);  impl->mAppClass = NULL;

    mamaStatsLoggerImpl_clearEventArray (impl);

    impl->mIsDestroyed = 1;
    free ((void*)impl);
    return MAMA_STATUS_OK;
}

mama_status
mamaStatsLogger_setUserName (mamaStatsLogger logger,
                             const char*     userName)
{
    mamaStatsLoggerImpl* impl = (mamaStatsLoggerImpl*)logger;
    if (!impl)
        return MAMA_STATUS_INVALID_ARG;
    return mamaStatsLoggerImpl_setString (impl, &impl->mUserName, userName);
}

mama_status
mamaStatsLogger_getUserName  (const mamaStatsLogger   logger,
                              const char**            userName)
{
    mamaStatsLoggerImpl* impl = (mamaStatsLoggerImpl*)logger;
    if (!impl)
        return MAMA_STATUS_INVALID_ARG;
    *userName = impl->mUserName;
    return MAMA_STATUS_OK;
}

mama_status
mamaStatsLogger_setHostName (mamaStatsLogger logger,
                             const char*     hostName)
{
    mamaStatsLoggerImpl* impl = (mamaStatsLoggerImpl*)logger;
    if (!impl)
        return MAMA_STATUS_INVALID_ARG;
    return mamaStatsLoggerImpl_setString (impl, &impl->mHostName, hostName);
}

mama_status
mamaStatsLogger_getHostName  (const mamaStatsLogger logger,
                              const char**          hostName)
{
    mamaStatsLoggerImpl* impl = (mamaStatsLoggerImpl*)logger;
    if (!impl)
        return MAMA_STATUS_INVALID_ARG;
    *hostName = impl->mHostName;
    return MAMA_STATUS_OK;
}

mama_status
mamaStatsLogger_setApplicationName (mamaStatsLogger logger,
                                    const char*     applicationName)
{
    mamaStatsLoggerImpl* impl = (mamaStatsLoggerImpl*)logger;
    if (!impl)
        return MAMA_STATUS_INVALID_ARG;

    return mamaStatsLoggerImpl_setString (impl,
                                          &impl->mAppName,
                                          applicationName);
}

mama_status
mamaStatsLogger_getApplicationName  (const mamaStatsLogger   logger,
                                     const char**            applicationName)
{
    mamaStatsLoggerImpl* impl = (mamaStatsLoggerImpl*)logger;
    if (!impl)
        return MAMA_STATUS_INVALID_ARG;
    *applicationName = impl->mAppName;
    return MAMA_STATUS_OK;
}

mama_status
mamaStatsLogger_setApplicationClass (mamaStatsLogger logger,
                                     const char*     applicationClass)
{
    mamaStatsLoggerImpl* impl = (mamaStatsLoggerImpl*)logger;
    if (!impl)
        return MAMA_STATUS_INVALID_ARG;

    return mamaStatsLoggerImpl_setString (impl,
                                          &impl->mAppClass,
                                          applicationClass);
}

mama_status
mamaStatsLogger_getApplicationClass  (const mamaStatsLogger logger,
                                      const char**          applicationClass)
{
    mamaStatsLoggerImpl* impl = (mamaStatsLoggerImpl*)logger;
    if (!impl)
        return MAMA_STATUS_INVALID_ARG;
    *applicationClass = impl->mAppClass;
    return MAMA_STATUS_OK;
}

mama_status
mamaStatsLogger_setIpAddress (mamaStatsLogger logger,
                              const char*     ipAddress)
{
    mamaStatsLoggerImpl* impl = (mamaStatsLoggerImpl*)logger;
    if (!impl)
        return MAMA_STATUS_INVALID_ARG;

    return mamaStatsLoggerImpl_setString (impl, &impl->mIpAddress, ipAddress);

}
mama_status
mamaStatsLogger_getIpAddress  (const mamaStatsLogger   logger,
                               const char**            ipAddress)
{
    mamaStatsLoggerImpl* impl = (mamaStatsLoggerImpl*)logger;
    if (!impl)
        return MAMA_STATUS_INVALID_ARG;
    *ipAddress = impl->mIpAddress;
    return MAMA_STATUS_OK;
}

mama_status
mamaStatsLogger_logLogin (mamaStatsLogger logger)
{
    mama_status status = MAMA_STATUS_OK;
    mamaStatsLoggerImpl* impl = (mamaStatsLoggerImpl*)logger;

    mamaMsg eventMsg;
    if (!impl)
        return MAMA_STATUS_INVALID_ARG;

    status = mamaStatsLoggerImpl_newEventMsg (impl,
                                              MAMA_SL_EVENT_LOGIN,
                                              &eventMsg);
    if (MAMA_STATUS_OK != status) return status;

    status = mamaStatsLoggerImpl_addEvent (impl, eventMsg);
    if (MAMA_STATUS_OK != status) return status;

    return status;
}

mama_status
mamaStatsLogger_logLogout (mamaStatsLogger logger)
{
    mama_status status = MAMA_STATUS_OK;
    mamaStatsLoggerImpl* impl = (mamaStatsLoggerImpl*)logger;

    mamaMsg eventMsg;
    if (!impl)
        return MAMA_STATUS_INVALID_ARG;

    status = mamaStatsLoggerImpl_newEventMsg (impl,
                                              MAMA_SL_EVENT_LOGOUT,
                                              &eventMsg);
    if (MAMA_STATUS_OK != status) return status;

    status = mamaStatsLoggerImpl_addEvent (impl, eventMsg);
    if (MAMA_STATUS_OK != status) return status;

    return status;
}



mama_status
mamaStatsLogger_logSubscriptionEntitlePass (mamaStatsLogger logger,
                                            mamaSubscription subscription,
                                            int32_t entitleCode)
{
    mama_status status = MAMA_STATUS_OK;

    mamaStatsLoggerImpl* impl = (mamaStatsLoggerImpl*)logger;

    mamaMsg eventMsg;

    if (!impl)
        return MAMA_STATUS_INVALID_ARG;

    /* Some internal subs shouldn't be logged */
    if (!mamaSubscription_getLogStats(subscription))
        return status;
    status = mamaStatsLoggerImpl_newEventMsg (impl,
                                              MAMA_SL_EVENT_SUB_ENT_PASS,
                                              &eventMsg);
    if (MAMA_STATUS_OK != status) return status;

    status = mamaStatsLoggerImpl_addSubDetails (impl,
                                                subscription,
                                                eventMsg,
                                                entitleCode);
    if (MAMA_STATUS_OK != status) return status;

    status = mamaStatsLoggerImpl_addEvent (impl, eventMsg);

    return status;
}

mama_status
mamaStatsLogger_logSubscriptionFail (mamaStatsLogger      logger,
                                     mamaSubscription     subscription,
                                     mamaSlSubFailReason  reason)
{
    mama_status status = MAMA_STATUS_OK;

    mamaStatsLoggerImpl* impl = (mamaStatsLoggerImpl*)logger;

    mamaMsg eventMsg;
    if (!impl)
        return MAMA_STATUS_INVALID_ARG;
    /* Some internal subs shouldn't be logged */
    if (!mamaSubscription_getLogStats(subscription))
        return status;
    status = mamaStatsLoggerImpl_newEventMsg (impl,
                                              MAMA_SL_EVENT_SUB_FAIL,
                                              &eventMsg);
    if (MAMA_STATUS_OK != status) return status;

    status = mamaStatsLoggerImpl_addSubDetails (impl,
                                                subscription,
                                                eventMsg,
                                                0);

    status = mamaMsg_addU32 (eventMsg,
                             MamaSlEventReason.mName,
                             MamaSlEventReason.mFid,
                             reason);

    if (MAMA_STATUS_OK != status) return status;

    status = mamaStatsLoggerImpl_addEvent (impl, eventMsg);

    return status;
}
mama_status
mamaStatsLogger_logSubscriptionDestroy (mamaStatsLogger  logger,
                                        mamaSubscription subscription)
{
    mama_status status = MAMA_STATUS_OK;

    mamaStatsLoggerImpl* impl = (mamaStatsLoggerImpl*)logger;

    mamaMsg eventMsg;
    if (!impl)
        return MAMA_STATUS_INVALID_ARG;
    /* Some internal subs shouldn't be logged */
    if (!mamaSubscription_getLogStats(subscription))
        return status;
    status = mamaStatsLoggerImpl_newEventMsg (impl,
                                              MAMA_SL_EVENT_SUB_DESTROY,
                                              &eventMsg);
    if (MAMA_STATUS_OK != status) return status;

    status = mamaStatsLoggerImpl_addSubDetails (impl,
                                                subscription,
                                                eventMsg,
                                                0);
    if (MAMA_STATUS_OK != status) return status;

    status = mamaStatsLoggerImpl_addEvent (impl, eventMsg);
    return status;
}

mama_status
mamaStatsLogger_addStatMsg (mamaStatsLogger* logger,
                            mamaMsg          msg)
{
    mamaMsg msgCopy = NULL;
    mama_status status = MAMA_STATUS_OK;
    mamaStatsLoggerImpl* impl = (mamaStatsLoggerImpl*)*logger;
    if (!impl) return MAMA_STATUS_NULL_ARG;

    mamaMsg_copy (msg, &msgCopy);

    mamaDateTime_setToNow (impl->mTimeStamp);

    if (status != MAMA_STATUS_OK) return status;

    status = mamaMsg_addDateTime (msgCopy,
                                  MamaStatTime.mName,
                                  MamaStatTime.mFid,
                                  impl->mTimeStamp);
    if (status != MAMA_STATUS_OK) return status;

    status = mamaStatsLoggerImpl_addEvent (impl, msgCopy);
    return status;
}

mama_status
mamaStatsLogger_sendReport (mamaStatsLogger* logger)
{
    mama_status status = MAMA_STATUS_OK;
    mamaStatsLoggerImpl* impl = (mamaStatsLoggerImpl*)*logger;
    if (!impl) return MAMA_STATUS_NULL_ARG;

    mamaStatsLoggerImpl_sendReport (impl);

    return status;
}

mama_status
mamaStatsLogger_getLogMsgStats (mamaStatsLogger  logger,
                                mama_bool_t*     logMsgStats)
{
    mamaStatsLoggerImpl* impl = (mamaStatsLoggerImpl*)logger;
    if (!impl)
        return MAMA_STATUS_INVALID_ARG;
    *logMsgStats = impl->mLogMsgStats;
    return MAMA_STATUS_OK;
}

mama_status
mamaStatsLogger_setLogMsgStats (mamaStatsLogger  logger,
                                mama_bool_t      logMsgStats)
{
    mamaStatsLoggerImpl* impl = (mamaStatsLoggerImpl*)logger;
    if (!impl)
        return MAMA_STATUS_INVALID_ARG;
    impl->mLogMsgStats  = logMsgStats;
    return MAMA_STATUS_OK;
}

mama_status
mamaStatsLogger_incMsgCount (mamaStatsLogger logger)
{
    mamaStatsLoggerImpl* impl = (mamaStatsLoggerImpl*)logger;
    if (!impl)
        return MAMA_STATUS_INVALID_ARG;
    wlock_lock (impl->mLock);
    ++(impl->mTotalMsgCount);
    wlock_unlock (impl->mLock);
    return MAMA_STATUS_OK;
}

/************************************************************************
* Private functions
*************************************************************************/
static
mama_status mamaStatsLoggerImpl_resetReport (
                mamaStatsLoggerImpl*  impl)
{
    if (!impl)
        return MAMA_STATUS_INVALID_ARG;

    mamaMsg_clear (impl->mReportMsg);
    /* Add the standard header fields */
    mamaMsg_addU8       (impl->mReportMsg,
                        MamaFieldInterfaceVersion.mName,
                        MamaFieldInterfaceVersion.mFid,
                        impl->mVersion);
    mamaMsg_addString   (impl->mReportMsg,
                        MamaFieldSubscSourceUser.mName,
                        MamaFieldSubscSourceUser.mFid,
                        (impl->mUserName) ? impl->mUserName : "");
    mamaMsg_addString   (impl->mReportMsg,
                        MamaFieldSubscSourceHost.mName,
                        MamaFieldSubscSourceHost.mFid,
                        (impl->mHostName) ? impl->mHostName : "");
    mamaMsg_addString   (impl->mReportMsg,
                        MamaFieldSubscSourceApp.mName,
                        MamaFieldSubscSourceApp.mFid,
                        (impl->mAppName) ? impl->mAppName : "");
    mamaMsg_addString   (impl->mReportMsg,
                        MamaFieldSubscSourceAppClass.mName,
                        MamaFieldSubscSourceAppClass.mFid,
                        (impl->mAppClass) ? impl->mAppClass : "");
    mamaMsg_addString   (impl->mReportMsg,
                        MamaFieldSubscSourceIp.mName,
                        MamaFieldSubscSourceIp.mFid,
                        (impl->mIpAddress) ? impl->mIpAddress : "");
    mamaMsg_addDateTime (impl->mReportMsg,
                        MamaSlIntervalStartTime.mName,
                        MamaSlIntervalStartTime.mFid,
                        impl->mReportStartTime);

    return MAMA_STATUS_OK;
}

static
mama_status mamaStatsLoggerImpl_setString (
                const mamaStatsLoggerImpl* impl,
                const char**               attribute,
                const char*                newValue)
{
    if (!impl)
        return MAMA_STATUS_INVALID_ARG;

    if (*attribute)
    {
        free ((void*) *attribute);
        *attribute = NULL;
    }
    if (newValue)
    {
        *attribute = strdup (newValue);
        if (!*attribute)
        {
            return MAMA_STATUS_NOMEM;
        }
    }
    return MAMA_STATUS_OK;
}

static
mama_status mamaStatsLoggerImpl_newEventMsg (
                const mamaStatsLoggerImpl*  impl,
                mamaSlEvent                 eventType,
                mamaMsg*                    eventMsg)
{
    mama_status     status = MAMA_STATUS_OK;
    mamaMsg msg;
    if (!impl)
            return MAMA_STATUS_INVALID_ARG;

    mamaDateTime_setToNow (impl->mTimeStamp);

    status = mamaMsg_create (&msg);
    if (status != MAMA_STATUS_OK) return status;

    status = mamaMsg_addDateTime (msg,
                                  MamaSlEventTime.mName,
                                  MamaSlEventTime.mFid,
                                  impl->mTimeStamp);
    if (status != MAMA_STATUS_OK) return status;

    status = mamaMsg_addU32 (msg,
                            MamaSlEventType.mName,
                            MamaSlEventType.mFid,
                            (mama_u32_t) eventType);
    if (status != MAMA_STATUS_OK) return status;

    status = mamaMsg_addI64 (msg,
                            MamaFieldSeqNum.mName,
                            MamaFieldSeqNum.mFid,
                            impl->mEventArraySize);
    if (status != MAMA_STATUS_OK) return status;
    *eventMsg = msg;
    return MAMA_STATUS_OK;
}

#define ENTITLE_FIELD_NAME  "wEntitleCode"
#define ENTITLE_FIELD_ID    496

static
mama_status mamaStatsLoggerImpl_addSubDetails (
                const mamaStatsLoggerImpl*  impl,
                mamaSubscription            subscription,
                mamaMsg                     eventMsg,
                int32_t                     entitleCode)
{
    mama_status     status    = MAMA_STATUS_OK;
    const char*     tportName = NULL;
    const char*     symbol    = NULL;
    const char*     namespace = NULL;
    mamaTransport   tport;
    /*int32_t         entitleCode; */

    if (!impl)
            return MAMA_STATUS_INVALID_ARG;

    mamaSubscription_getSymbol      (subscription, &symbol);
    mamaSubscription_getTransport   (subscription, &tport);
    mamaTransport_getName           (tport, &tportName);
    mamaSubscription_getSource      (subscription, &namespace);

    status = mamaMsg_addI32 (eventMsg,
                             ENTITLE_FIELD_NAME,
                             ENTITLE_FIELD_ID,
                             entitleCode);
    status = mamaMsg_addString (eventMsg,
                                MamaFieldTportName.mName,
                                MamaFieldTportName.mFid,
                                tportName ? tportName : "");
    status = mamaMsg_addString (eventMsg,
                                MamaFieldSubscSymbol.mName,
                                MamaFieldSubscSymbol.mFid,
                                symbol ? symbol : "");
    status = mamaMsg_addString (eventMsg,
                                MamaFieldSubscNamespace.mName,
                                MamaFieldSubscNamespace.mFid,
                                namespace ? namespace : "");
    return MAMA_STATUS_OK;
}

static
mama_status mamaStatsLoggerImpl_addEvent (
                mamaStatsLoggerImpl*  impl,
                mamaMsg               eventMsg)
{
    int i = 0;
    if (!impl)
            return MAMA_STATUS_INVALID_ARG;
    wlock_lock (impl->mLock);

    /* Grow the array if necessary */
    if  (impl->mEventArraySize == impl->mEventArrayMaxSize)
    {
        impl->mEventArrayMaxSize = (impl->mEventArrayMaxSize * 2);
        impl->mEventArray =
            (mamaMsg*) realloc (impl->mEventArray,
                                (impl->mEventArrayMaxSize * sizeof (mamaMsg)));

        for (i = impl->mEventArraySize; i < impl->mEventArrayMaxSize; i++)
        {
            impl->mEventArray[i] = NULL;
        }

        if (!impl->mEventArray)
        {
            wlock_unlock (impl->mLock);
            return MAMA_STATUS_NOMEM;
        }
    }

    impl->mEventArray [impl->mEventArraySize++] = eventMsg;

    /* figure out if we have enough messages to send a report */
    if ((impl->mIsConnected) &&
       ((impl->mEventArraySize) >= impl->mReportSize))

    {
        mamaStatsLoggerImpl_sendReport (impl);
    }
    wlock_unlock (impl->mLock);
    return MAMA_STATUS_OK;
}

static void MAMACALLTYPE
mamaStatsLoggerImpl_onReportTimer (mamaTimer timer, void* closure)
{
    mamaStatsLoggerImpl*  impl = (mamaStatsLoggerImpl*) closure;

    wlock_lock (impl->mLock);

    mamaStatsLoggerImpl_sendReport (impl);

    wlock_unlock (impl->mLock);

}

/* NB: mLock must be locked before calling this, and unlocked
       afterward
*/
static
mama_status mamaStatsLoggerImpl_sendReport (
                mamaStatsLoggerImpl*  impl)
{
    mama_status status      = MAMA_STATUS_OK;
    mamaDateTime_setToNow (impl->mTimeStamp);


    status = mamaMsg_addDateTime (impl->mReportMsg,
                                 MamaSlIntervalEndTime.mName,
                                 MamaSlIntervalEndTime.mFid,
                                 impl->mTimeStamp);
    if (status != MAMA_STATUS_OK) return status;

    if (impl->mLogMsgStats)
    {

        mama_u64_t msgCount = impl->mTotalMsgCount - impl->mPreviousMsgCount;
        impl->mPreviousMsgCount = impl->mTotalMsgCount;

        status = mamaMsg_addU64   (impl->mReportMsg,
                                  MamaSlTotalMsgCount.mName,
                                  MamaSlTotalMsgCount.mFid,
                                  impl->mTotalMsgCount);
        if (status != MAMA_STATUS_OK) return status;

        status = mamaMsg_addU64   (impl->mReportMsg,
                                   MamaSlIntervalMsgCount.mName,
                                   MamaSlIntervalMsgCount.mFid,
                                   msgCount);
        if (status != MAMA_STATUS_OK) return status;

    }

    if (impl->mEventArraySize)
    {
        status = mamaMsg_addVectorMsg (
                                 impl->mReportMsg,
                                 impl->mEventFieldName,
                                 impl->mEventFieldFid,
                                 impl->mEventArray,
                                 impl->mEventArraySize);
    }

    mamaPublisher_send (impl->mPublisher, impl->mReportMsg);
    mamaDateTime_copy (impl->mReportStartTime, impl->mTimeStamp);
    mamaStatsLoggerImpl_resetReport (impl);
    mamaStatsLoggerImpl_clearEventArray (impl);

    impl->mEventArray =
            (mamaMsg*)calloc (impl->mEventArrayMaxSize, (sizeof (mamaMsg)));

    return MAMA_STATUS_OK;
}

static void mamaStatsLoggerImpl_clearEventArray (mamaStatsLoggerImpl* impl)
{
    int i = 0;

    for (i = 0; i<impl->mEventArrayMaxSize; i++)
    {
        if (impl->mEventArray[i] != NULL)
        {
            mamaMsg_destroy (impl->mEventArray[i]);
            impl->mEventArray[i] = NULL;
        }
    }
    free ((void*)impl->mEventArray);
    impl->mEventArray = NULL;
    impl->mEventArrayMaxSize = SL_EVENT_ARRAY_MAX_SIZE;
    impl->mEventArraySize = 0;
}


