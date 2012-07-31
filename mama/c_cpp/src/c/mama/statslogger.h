/* $Id: statslogger.h,v 1.1.2.4 2011/10/02 19:02:18 ianbell Exp $
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

#ifndef MamaStatsLoggerH__
#define MamaStatsLoggerH__

#include <mama/status.h>
#include <mama/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

/************************************************************************
* Constants
*************************************************************************/
#define SL_TOPIC_USAGE_LOG "SL_TOPIC_USAGE_LOG"
#define STATS_TOPIC        "STATS_TOPIC"

/************************************************************************
* Typedefs
*************************************************************************/
/** The event types handled by the stats logger */
typedef enum mamaSlEvent_
{
    MAMA_SL_EVENT_LOGIN         = 0,
    MAMA_SL_EVENT_LOGOUT        = 1,
    MAMA_SL_EVENT_SUB_CREATE    = 2,
    MAMA_SL_EVENT_SUB_FAIL      = 3,
    MAMA_SL_EVENT_SUB_DESTROY   = 4,
    MAMA_SL_EVENT_SUB_ENT_PASS  = 5
} mamaSlEvent;

/** The reasons for a subscription to fail*/
typedef enum mamaSlSubFailReason_
{
    MAMA_SL_SUB_FAIL_REGEX      = 0,
    MAMA_SL_SUB_FAIL_CODE       = 1,
    MAMA_SL_SUB_FAIL_TIMEOUT    = 2,
    MAMA_SL_SUB_FAIL_NOT_FOUND  = 3
} mamaSlSubFailReason;

/************************************************************************
* Functions
*************************************************************************/

/**
 * Convert a mamaSlEvent value to a string.  Do no attempt to free the
 * string result.
 *
 * @param event  The mamaSlEvent to convert.
 */
MAMAExpDLL
extern const char*
mamaStatsLogger_eventToString (mamaSlEvent  event);
/**
 * Convert a mamaSlSubFailReason value to a string.  Do no attempt to free
 * the string result.
 *
 * @param subFailReason  The mamaSlSubFailReason to convert.
 */
MAMAExpDLL
extern const char*
mamaStatsLogger_subFailReasonToString (mamaSlSubFailReason  subFailReason);

/**
 * Allocate memory for a new stats logger. The logger is not
 * actually created until a call the create() functions is made.
 * Memory must be freed using the <code>mamaStatsLogger_Deallocate()</code>
 * function.
 *
 * @param logger Where the address of the new logger will be written
 */
MAMAExpDLL
extern mama_status
mamaStatsLogger_allocate (mamaStatsLogger* logger);

/**
 * Create and activate mamaStatsLogger object.
 *
 * @param logger The location of a mamaStatsLogger
 * @param queue The queue for the Stats Logger to use for publishing
 * @param tport the transport
 */
MAMAExpDLL
extern mama_status
mamaStatsLogger_create (mamaStatsLogger  logger,
                        mamaQueue        queue,
                        mamaTransport    tport);

MAMAExpDLL
extern mama_status
mamaStatsLogger_createForStats (mamaStatsLogger  logger,
                                mamaQueue        queue,
                                mamaTransport    tport,
                                const char*      topic);


/**
 * Destroy a mamaStatsLogger object, freeing any memory
 *
 * @param logger The location of a mamaStatsLogger
 */
MAMAExpDLL
extern mama_status
mamaStatsLogger_destroy (mamaStatsLogger  logger);

/**
 * Set the reporting interval
 *
 * @param logger The location of a mamaStatsLogger
 * @param interval The report interval in seconds
 */
MAMAExpDLL
extern mama_status
mamaStatsLogger_setReportInterval (mamaStatsLogger logger,
                                   mama_f64_t      interval);

/**
 * Set the maximum number of events to cache before reporting
 *
 * @param logger The location of a mamaStatsLogger
 * @param numEvents The number of events to report after
 */
MAMAExpDLL
extern mama_status
mamaStatsLogger_setReportSize(mamaStatsLogger logger,
                              mama_size_t     numEvents);

/**
 * Set the userName
 *
 * @param logger   The logger object to update.
 * @param userName  The user name for the logger
  */
MAMAExpDLL
extern mama_status
mamaStatsLogger_setUserName (mamaStatsLogger    logger,
                            const char*         userName);

/**
 * Get the userName
 *
 * @param logger   The logger object to check.
 * @param userName  Location of the result for the description of the logger.
 */
MAMAExpDLL
extern mama_status
mamaStatsLogger_getUserName (const mamaStatsLogger  logger,
                             const char**           userName);


/**
 * Set the hostName
 *
 * @param logger   The logger object to update.
 * @param hostName  The host name for the logger
  */
MAMAExpDLL
extern mama_status
mamaStatsLogger_setHostName (mamaStatsLogger logger,
                             const char*     hostName);

/**
 * Get the hostName
 *
 * @param logger   The logger object to check.
 * @param hostName  Location of the result for the description of the logger.
 */
MAMAExpDLL
extern mama_status
mamaStatsLogger_getHostName (const mamaStatsLogger logger,
                             const char**          hostName);

/**
 * Set the applicationName
 *
 * @param logger   The logger object to update.
 * @param appName  The applicationName for the logger
  */
MAMAExpDLL
extern mama_status
mamaStatsLogger_setApplicationName (mamaStatsLogger    logger,
                                    const char*        appName);

/**
 * Get the applicationName
 *
 * @param logger   The logger object to check.
 * @param appName  Location of the result for the applicationName
 */
MAMAExpDLL
extern mama_status
mamaStatsLogger_getApplicationName (const mamaStatsLogger  logger,
                                    const char**           appName);

/**
 * Set the applicationClass
 *
 * @param logger   The logger object to update.
 * @param applClass  The applicationName for the logger
  */
MAMAExpDLL
extern mama_status
mamaStatsLogger_setApplicationClass (mamaStatsLogger logger,
                                    const char*      applClass);

/**
 * Get the applicationClass
 *
 * @param logger   The logger object to check.
 * @param appClass  Location of the result for the applicationClass
 */
MAMAExpDLL
extern mama_status
mamaStatsLogger_getApplicationClass (const mamaStatsLogger  logger,
                                     const char**           appClass);

/**
 * Set the ipAddress
 *
 * @param logger   The logger object to update.
 * @param ipAddress  The ipAddress for the logger
  */
MAMAExpDLL
extern mama_status
mamaStatsLogger_setIpAddress (mamaStatsLogger logger,
                              const char*     ipAddress);

/**
 * Get the ipAddress
 *
 * @param logger   The logger object to check.
 * @param ipAddress  Location of the result for the ipAddress of the logger.
 */
MAMAExpDLL
extern mama_status
mamaStatsLogger_getIpAddress (const mamaStatsLogger  logger,
                              const char**           ipAddress);

/**
 * Log a login event
 *
 * @param logger   The logger object to use.
 */
MAMAExpDLL
extern mama_status
mamaStatsLogger_logLogin (mamaStatsLogger logger);

/**
 * Log a logout event
 *
 * @param logger   The logger object to use.
 */
MAMAExpDLL
extern mama_status
mamaStatsLogger_logLogout (mamaStatsLogger logger);

/**
 * Log a subscription entitlement check pass event
 *
 * @param logger   The logger object to use.
 * @param subscription the subscription
 * @param entitleCode the entitlement code
 */
MAMAExpDLL
extern mama_status
mamaStatsLogger_logSubscriptionEntitlePass (mamaStatsLogger logger,
                                            mamaSubscription subscription,
                                            int32_t          entitleCode);

/**
 * Log a subscription fail event
 *
 * @param logger   The logger object to use.
 * @param subscription the subscription
 * @param reason the reason
 */
MAMAExpDLL
extern mama_status
mamaStatsLogger_logSubscriptionFail (mamaStatsLogger     logger,
                                     mamaSubscription    subscription,
                                     mamaSlSubFailReason reason);
/**
 * Log a subscription destroy event
 *
 * @param logger   The logger object to use.
 * @param subscription the subscription
 */
MAMAExpDLL
extern mama_status
mamaStatsLogger_logSubscriptionDestroy (mamaStatsLogger  logger,
                                        mamaSubscription subscription);

MAMAExpDLL
extern mama_status
mamaStatsLogger_addStatMsg (mamaStatsLogger* logger,
                                mamaMsg          msg);

MAMAExpDLL
extern mama_status
mamaStatsLogger_sendReport (mamaStatsLogger* logger);

MAMAExpDLL
extern mama_status
mamaStatsLogger_getLogMsgStats (mamaStatsLogger  logger,
                                mama_bool_t*     logMsgStats);
MAMAExpDLL
extern mama_status
mamaStatsLogger_setLogMsgStats (mamaStatsLogger  logger,
                                mama_bool_t      logMsgStats);

MAMAExpDLL
extern mama_status
mamaStatsLogger_incMsgCount (mamaStatsLogger statsLogger);

#if defined(__cplusplus)
}
#endif

#endif
