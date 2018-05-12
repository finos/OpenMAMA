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

#include <string.h>
#include <stdio.h>
#include <limits.h>

#include "wombat/port.h"
#include "wombat/environment.h"
#include "wombat/strutils.h"
#include "wombat/wInterlocked.h"
#include "wombat/thread.h"
#include "bridge.h"

#include <mama/mama.h>
#include <mama/error.h>
#include <mamainternal.h>
#include <mama/version.h>
#include <bridge.h>
#include <payloadbridge.h>
#include <property.h>
#include <platform.h>
#include <plugin.h>
#include <registerfunctions.h>

#include "fileutils.h"
#include "reservedfieldsimpl.h"
#include <mama/statslogger.h>
#include <mama/stat.h>
#include <mama/statfields.h>
#include <statsgeneratorinternal.h>
#include <mama/statscollector.h>
#include "transportimpl.h"

#ifndef OPENMAMA_INTEGRATION
  #define OPENMAMA_INTEGRATION
#endif

#include <mama/integration/mama.h>

#define PROPERTY_FILE "mama.properties"
#define WOMBAT_PATH_ENV "WOMBAT_PATH"
#define MAMA_PROPERTY_BRIDGE "mama.bridge.provider"
#define MAMA_PROPERTY_THREAD_AFFINITY "mama.thread_affinity."
#define MAMA_PROPERTY_PAYLOAD_AUTOLOAD "mama.payload.autoload."
#define MAMA_ENTITLEMENT_LIB_FILEPATTERN "mamaent%s"
#define DEFAULT_STATS_INTERVAL 60

#include "entitlementinternal.h"
#define MAX_USER_NAME_STR_LEN 64
#define MAX_HOST_NAME_STR_LEN 64

extern void initReservedFields (void);

mamaEntitlementCallbacks  gEntitlementCallbacks;
extern const char*        gEntitlementBridges[MAX_ENTITLEMENT_BRIDGES];

/* Sats Configuration*/
int gLogQueueStats          = 1;
int gLogTransportStats      = 1;
int gLogGlobalStats         = 1;
int gLogLbmStats            = 1;
int gLogUserStats           = 1;

int gGenerateQueueStats     = 0;
int gGenerateTransportStats = 0;
int gGenerateGlobalStats    = 0;
int gGenerateLbmStats       = 0;
int gGenerateUserStats      = 0;

int gPublishQueueStats      = 0;
int gPublishTransportStats  = 0;
int gPublishGlobalStats     = 0;
int gPublishLbmStats        = 0;
int gPublishUserStats       = 0;

static mamaStatsLogger  gStatsPublisher  = NULL;

mamaStatsGenerator      gStatsGenerator         = NULL;
mamaStatsCollector      gGlobalStatsCollector   = NULL;

/* Global Stats */
mamaStat                gInitialStat;
mamaStat                gRecapStat;
mamaStat                gUnknownMsgStat;
mamaStat                gMessageStat;
mamaStat                gFtTakeoverStat;
mamaStat                gSubscriptionStat;
mamaStat                gTimeoutStat;
mamaStat                gWombatMsgsStat;
mamaStat                gFastMsgsStat;
mamaStat                gRvMsgsStat;
mamaStat                gPublisherSend;
mamaStat                gPublisherInboxSend;
mamaStat                gPublisherReplySend;

mama_bool_t gAllowMsgModify = 1;

mama_status mama_statsInit (void);
mama_status mama_setupStatsGenerator (void);

int gCatchCallbackExceptions = 0;

wproperty_t             gProperties      = 0;

static mamaPayloadBridge    gDefaultPayload = NULL;

static wthread_key_t last_err_key;

/**
 * struct mamaApplicationGroup
 * Contains the name of the application and its class name.
 */
typedef struct mamaAppContext_
{
    const char* myApplicationName;
    const char* myApplicationClass;
} mamaApplicationContext;

/**
 * @brief Structure for storing combined mamaBridge and LIB_HANDLE data
 */
typedef struct mamaMiddlewareLib_
{
    mamaBridge  bridge;
    LIB_HANDLE  library;
} mamaMiddlewareLib;


/**
 * @brief Structure for managing the loaded middleware libraries
 */
typedef struct mamaMiddlewares_
{
    /* wtable of loaded middlewares, keyed by middleware name */
    wtable_t            table;

    /* Array of loaded middleware libraries, indexed by order of load */
    mamaMiddlewareLib*  byIndex[MAMA_MAX_MIDDLEWARES];

    /* Count of number of currently loaded middlewares */
    mama_i32_t          count;
} mamaMiddlewares;

/**
 * @brief Structure for managing loaded payload libraries
 */
typedef struct mamaPayloads_
{
    /* wtable of loaded payloads, keyed by payload name */
    wtable_t            table;

    /* Array indexed by char id, for rapid access when required. */
    mamaPayloadLib*     byChar[MAMA_PAYLOAD_MAX];

    /* Count of number of currently loaded payloads */
    mama_i32_t          count;
} mamaPayloads;

/**
 * @brief Structure for managing loaded entitlement libraries
 */
typedef struct mamaEntitlements_
{
    /* wtable of loaded entitlement libraries, keyed by name */
    wtable_t            table;

    /* Array of loaded entitlement libraries, indexed by order loaded */
    mamaEntitlementLib* byIndex[MAMA_MAX_ENTITLEMENTS];

    /* Count of number of currently loaded entitlement libraries */
    mama_i32_t          count;
} mamaEntitlements;

/**
 * This structure contains data needed to control starting and stopping of
 * mama.
 */
typedef struct mamaImpl_
{
    /* Struct containing loaded middleware bridges. */
    mamaMiddlewares        middlewares;

    /* Struct containing loaded payload bridges */
    mamaPayloads           payloads;

    /* Struct containing loaded entitlement bridges */
    mamaEntitlements       entitlements;

    unsigned int           myRefCount;

    /* wInterlockedInt indicating if the struct has been successfully initialised */
    wInterlockedInt        init;

    wthread_static_mutex_t myLock;

    /* Internal properties */
    wproperty_t            internalProperties;

    /* Version Information */
    versionInfo            version;
} mamaImpl;

static mamaApplicationContext  appContext;
static char mama_ver_string[256];

static mamaImpl gImpl = {
                            { NULL, {0}, 0 },         /* middlewares */
                            { NULL, {0}, 0 },         /* payloads */
                            { NULL, {0}, 0 },         /* entitlements */
                            0,                        /* myRefCount */
                            0,                        /* init */
                            WSTATIC_MUTEX_INITIALIZER,/* myLock */
                            NULL
                        };

/*
 * Function pointer type for calling getVersion in the wrapper
 */
typedef const char* (MAMACALLTYPE *fpWrapperGetVersion)(void);

static fpWrapperGetVersion wrapperGetVersion = NULL;

/* ************************************************************************* */
/* Private Function Prototypes. */
/* ************************************************************************* */

static mama_status
mama_loadBridgeWithPathInternal (mamaBridge* impl,
                                 const char* middlewareName,
                                 const char* path);

mama_status
mama_loadPayloadBridgeInternal  (mamaPayloadBridge* impl,
                                 const char*        payloadName);

mama_status
mama_loadEntitlementBridgeInternal  (const char* name);

static void
threadPropertiesCb (const char* name, const char* value, void* closure);

mama_bool_t
mama_areVersionsCompatibleInternal (versionInfo mamaVer, versionInfo bridgeVer);

void
mama_normalizeMamaBridgeInterfaceVersionInternal (versionInfo* version);

MAMAExpDLL
void
mama_setWrapperGetVersion(fpWrapperGetVersion value);

mama_status mamaInternal_autoloadPayloadBridges (void);

static void autoloadPayloadPropertiesCb (const char* name,
                                         const char* value,
                                         void*      closure);


/*  Description :   This function will free any memory associated with a
 *                  mamaApplicationContext object but will not free the
 *                  object itself.
 *  Arguments   :   context [I] The context object to free.
 */
static void
mama_freeAppContext(mamaApplicationContext *context)
{
    /* Only continue if the object is valid. */
    if(context != NULL)
    {
        /* Free all memory */
        if(context->myApplicationName != NULL)
        {
            free((void *)context->myApplicationName);
            context->myApplicationName = NULL;
        }

        if(context->myApplicationClass != NULL)
        {
            free((void *)context->myApplicationClass);
            context->myApplicationClass = NULL;
        }
    }
}

mama_status
mama_setApplicationName (const char* applicationName)
{
    if (appContext.myApplicationName)
    {
        free ((void*)appContext.myApplicationName);
        appContext.myApplicationName = NULL;
    }

    if (applicationName)
    {
        appContext.myApplicationName = strdup (applicationName);
    }
    return MAMA_STATUS_OK;
}

mama_status
mama_setApplicationClassName (const char* className)
{
    if (appContext.myApplicationClass)
    {
        free ((void*)appContext.myApplicationClass);
        appContext.myApplicationClass = NULL;
    }

    if (className)
    {
        appContext.myApplicationClass =  strdup (className);
    }
    return MAMA_STATUS_OK;
}

mama_status
mama_getApplicationName (const char**  applicationName)
{
    if (applicationName == NULL) return MAMA_STATUS_NULL_ARG;
    *applicationName = appContext.myApplicationName;
    return MAMA_STATUS_OK;
}

mama_status
mama_getApplicationClassName (const char** className)
{
    if (className == NULL) return MAMA_STATUS_NULL_ARG;
    *className = appContext.myApplicationClass;
    return MAMA_STATUS_OK;
}

static void
mamaInternal_loadProperties (const char *path,
                             const char *filename)
{
    wproperty_t fileProperties;
    int usingDefaults = 0;

    if (!path && !filename)
    {
        usingDefaults = 1;
    }

    if( gProperties == 0 )
    {
        gProperties = properties_Create ();
    }

    if( !path )
    {
        path = getenv (WOMBAT_PATH_ENV);
        mama_log (MAMA_LOG_LEVEL_NORMAL, "Using path specified in %s",
            WOMBAT_PATH_ENV);
    }

    if( !filename )
    {
       filename = PROPERTY_FILE;
       mama_log (MAMA_LOG_LEVEL_NORMAL,
                 "Using default properties file %s",
                 PROPERTY_FILE);
    }

    mama_log (MAMA_LOG_LEVEL_NORMAL,
              "Attempting to load MAMA properties from %s", path ? path : "");

    fileProperties = properties_Load (path, filename);

    /* Fail if loading properties failed and we have no properties so far or
     * non-defaults were specified */
    if( fileProperties == 0 && (!usingDefaults || 0 == properties_Count(gProperties)))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "Failed to open properties file.");
        return;
    }

    if (NULL != fileProperties)
    {
        /* We've got file properties, so we need to merge 'em into
         * anything we've already gotten */
        properties_Merge(fileProperties, gProperties);

        /* Free the file properties, note that FreeEx2 is called to ensure that the data
         * isn't freed as the pointers have been copied over to gProperties.
         */
        properties_FreeEx2(gProperties);

        gProperties = fileProperties;
    }
}

static int mamaInternal_statsPublishingEnabled (void)
{
    return (gPublishGlobalStats
         || gPublishTransportStats
         || gPublishQueueStats
         || gPublishLbmStats
         || gPublishUserStats);
}

static mama_status
mamaInternal_createStatsPublisher (void)
{
    mama_status         result                  = MAMA_STATUS_OK;
    mamaMiddlewareLib*  middlewareLib           = NULL;
    mamaBridge          bridge                  = NULL;
    mamaQueue           queue                   = NULL;
    mamaTransport       statsLogTport           = NULL;
    const char*         userName                = NULL;
    const char*         statsLogTportName       = NULL;
    const char*         statsLogMiddlewareName  = NULL;

    mama_log (MAMA_LOG_LEVEL_NORMAL, "Stats publishing enabled");

    statsLogTportName       = properties_Get (gProperties,
                                              "mama.statslogging.transport");
    statsLogMiddlewareName  = properties_Get (gProperties,
                                              "mama.statslogging.middleware");

    if (!statsLogMiddlewareName)
    {
        statsLogMiddlewareName = "wmw";
    }

    middlewareLib = (mamaMiddlewareLib*)wtable_lookup (
                                            gImpl.middlewares.table,
                                            statsLogMiddlewareName);

    if (NULL == middlewareLib || NULL == middlewareLib->bridge)
    {
        result = MAMA_STATUS_NO_BRIDGE_IMPL;
        mama_log (MAMA_LOG_LEVEL_WARN,
                  "mamaInternal_createStatsPublisher ():"
                  "Unable to access middleware bridge [%s]. Cannot create stats publisher.",
                  statsLogMiddlewareName);
        return result;
    }

    bridge = middlewareLib->bridge;

    if (MAMA_STATUS_OK != (result = mamaBridgeImpl_getInternalEventQueue (bridge,
                                                                          &queue)))
    {
        return result;
    }

    result = mamaStatsLogger_allocate (&gStatsPublisher);

    if( result != MAMA_STATUS_OK )
        return result;

    mama_getUserName (&userName);
    mamaStatsLogger_setReportSize       (gStatsPublisher, 100);
    mamaStatsLogger_setUserName         (gStatsPublisher, userName);
    mamaStatsLogger_setIpAddress        (gStatsPublisher, getIpAddress());
    mamaStatsLogger_setHostName         (gStatsPublisher, getHostName());
    mamaStatsLogger_setApplicationName  (gStatsPublisher,
                                         appContext.myApplicationName);
    mamaStatsLogger_setApplicationClass (gStatsPublisher,
                                         appContext.myApplicationClass);

    mamaStatsLogger_setLogMsgStats (gStatsPublisher, 0);

    if (!statsLogTportName)
    {
        statsLogTportName = "statslogger";
    }

    result = mamaTransportImpl_allocateInternalTransport (&statsLogTport);
    if( result != MAMA_STATUS_OK )
        return result;

    result = mamaTransport_create (statsLogTport,
                                   statsLogTportName,
                                   bridge);
    if (result != MAMA_STATUS_OK)
        return result;

    result = mamaStatsLogger_createForStats (gStatsPublisher,
                                             queue,
                                             statsLogTport,
                                             STATS_TOPIC);
    if (result != MAMA_STATUS_OK)
        return result;

    mama_log (MAMA_LOG_LEVEL_NORMAL, "Stats logging middleware [%s]",
                                    statsLogMiddlewareName ? statsLogMiddlewareName : "");
    mama_log (MAMA_LOG_LEVEL_NORMAL, "Stats logging transport [%s]",
                                    statsLogTportName ? statsLogTportName : "");

    return result;
}

static mama_status
mamaInternal_enableStatsLogging (void)
{
    mama_status     result                  = MAMA_STATUS_OK;
    const char*     statsLogIntervalStr     = NULL;

    mama_log (MAMA_LOG_LEVEL_NORMAL, "Stats logging enabled");

    if (mamaInternal_statsPublishingEnabled())
    {
        if (MAMA_STATUS_OK != (result = mamaInternal_createStatsPublisher ()))
        {
            mama_log (MAMA_LOG_LEVEL_ERROR,
                      "mamaInternal_enableStatsLogging(): "
                      "Could not create stats publisher");
            return result;
        }
    }

    mama_log (MAMA_LOG_LEVEL_NORMAL, "Stats logging interval [%s]",
                                    statsLogIntervalStr ? statsLogIntervalStr : "");
    mama_log (MAMA_LOG_LEVEL_NORMAL, "Stats logging: global stats logging %s",
                                     gGenerateGlobalStats ? "enabled" : "disabled");
    mama_log (MAMA_LOG_LEVEL_NORMAL, "Stats logging: transport stats logging %s",
                                     gGenerateTransportStats ? "enabled" : "disabled");
    mama_log (MAMA_LOG_LEVEL_NORMAL, "Stats logging: queue stats logging %s",
                                     gGenerateQueueStats ? "enabled" : "disabled");


    if (gGenerateGlobalStats)
    {
        const char* appName;
        mama_getApplicationName (&appName);

        if (MAMA_STATUS_OK != (result =
            mamaStatsCollector_create (&gGlobalStatsCollector,
                                       MAMA_STATS_COLLECTOR_TYPE_GLOBAL,
                                       appName,
                                       "-----")))
        {
            return result;
        }

        if (!gLogGlobalStats)
        {
            if (MAMA_STATUS_OK != (result =
                mamaStatsCollector_setLog (gGlobalStatsCollector, 0)))
            {
                return MAMA_STATUS_OK;
            }
        }

        if (gPublishGlobalStats)
        {
            if (MAMA_STATUS_OK != (result =
                mamaStatsCollector_setPublish (gGlobalStatsCollector, 1)))
            {
                return MAMA_STATUS_OK;
            }

            mama_log (MAMA_LOG_LEVEL_NORMAL, "Stats publishing enabled for global stats");
        }

        result = mamaStat_create (&gInitialStat,
                                  gGlobalStatsCollector,
                                  MAMA_STAT_LOCKABLE,
                                  MamaStatInitials.mName,
                                  MamaStatInitials.mFid);
        if (result != MAMA_STATUS_OK) return result;

        result = mamaStat_create (&gRecapStat,
                                  gGlobalStatsCollector,
                                  MAMA_STAT_LOCKABLE,
                                  MamaStatRecaps.mName,
                                  MamaStatRecaps.mFid);
        if (result != MAMA_STATUS_OK) return result;

        result = mamaStat_create (&gUnknownMsgStat,
                                  gGlobalStatsCollector,
                                  MAMA_STAT_LOCKABLE,
                                  MamaStatUnknownMsgs.mName,
                                  MamaStatUnknownMsgs.mFid);
        if (result != MAMA_STATUS_OK) return result;

        result = mamaStat_create (&gMessageStat,
                                  gGlobalStatsCollector,
                                  MAMA_STAT_LOCKABLE,
                                  MamaStatNumMessages.mName,
                                  MamaStatNumMessages.mFid);
        if (result != MAMA_STATUS_OK) return result;

        result = mamaStat_create (&gFtTakeoverStat,
                                  gGlobalStatsCollector,
                                  MAMA_STAT_LOCKABLE,
                                  MamaStatFtTakeovers.mName,
                                  MamaStatFtTakeovers.mFid);
        if (result != MAMA_STATUS_OK) return result;

        result = mamaStat_create (&gSubscriptionStat,
                                  gGlobalStatsCollector,
                                  MAMA_STAT_LOCKABLE,
                                  MamaStatNumSubscriptions.mName,
                                  MamaStatNumSubscriptions.mFid);
        if (result != MAMA_STATUS_OK) return result;

        result = mamaStat_create (&gTimeoutStat,
                                  gGlobalStatsCollector,
                                  MAMA_STAT_LOCKABLE,
                                  MamaStatTimeouts.mName,
                                  MamaStatTimeouts.mFid);
        if (result != MAMA_STATUS_OK) return result;

        result = mamaStat_create (&gWombatMsgsStat,
                                  gGlobalStatsCollector,
                                  MAMA_STAT_LOCKABLE,
                                  MamaStatWombatMsgs.mName,
                                  MamaStatWombatMsgs.mFid);
        if (result != MAMA_STATUS_OK) return result;

        result = mamaStat_create (&gFastMsgsStat,
                                  gGlobalStatsCollector,
                                  MAMA_STAT_LOCKABLE,
                                  MamaStatFastMsgs.mName,
                                  MamaStatFastMsgs.mFid);
        if (result != MAMA_STATUS_OK) return result;

        result = mamaStat_create (&gRvMsgsStat,
                                  gGlobalStatsCollector,
                                  MAMA_STAT_LOCKABLE,
                                  MamaStatRvMsgs.mName,
                                  MamaStatRvMsgs.mFid);
        if (result != MAMA_STATUS_OK) return result;

        result = mamaStat_create (&gPublisherSend,
                                  gGlobalStatsCollector,
                                  MAMA_STAT_LOCKABLE,
                                  MamaStatPublisherSend.mName,
                                  MamaStatPublisherSend.mFid);
        if (result != MAMA_STATUS_OK) return result;

        result = mamaStat_create (&gPublisherInboxSend,
                                  gGlobalStatsCollector,
                                  MAMA_STAT_LOCKABLE,
                                  MamaStatPublisherInboxSend.mName,
                                  MamaStatPublisherInboxSend.mFid);
        if (result != MAMA_STATUS_OK) return result;

        result = mamaStat_create (&gPublisherReplySend,
                                  gGlobalStatsCollector,
                                  MAMA_STAT_LOCKABLE,
                                  MamaStatPublisherReplySend.mName,
                                  MamaStatPublisherReplySend.mFid);
        if (result != MAMA_STATUS_OK) return result;
        mamaStatsGenerator_addStatsCollector (gStatsGenerator, gGlobalStatsCollector);
    }

    if (gLogQueueStats || gLogTransportStats || gLogGlobalStats || gLogLbmStats || gLogUserStats)
    {
        mamaStatsGenerator_setLogStats (gStatsGenerator, 1);
    }
    else
    {
        mamaStatsGenerator_setLogStats (gStatsGenerator, 0);
    }

    if (gStatsPublisher != NULL)
    {
        if (MAMA_STATUS_OK != (result =
                        mamaStatsGenerator_setStatsLogger (gStatsGenerator, &gStatsPublisher)))
        {
            return result;
        }
    }

    return result;
}

mamaStatsGenerator
mamaInternal_getStatsGenerator()
{
    return gStatsGenerator;
}

mamaStatsCollector
mamaInternal_getGlobalStatsCollector()
{
    return gGlobalStatsCollector;
}

/**
 * Expose the property object
 */
wproperty_t
mamaInternal_getProperties()
{
  return gProperties;
}

/**
 * @brief Return the first available bridge.
 *
 * @return A mamaBridge representing the first available bridge.
 */
mamaBridge
mamaInternal_findBridge ()
{
    mamaMiddlewareLib* middlewareLib = gImpl.middlewares.byIndex[0];
    mamaBridge bridge = NULL;

    /* If it exists, take the first bridge from the middlewares index. */
    if (NULL != middlewareLib && NULL != middlewareLib->bridge)
    {
        bridge = middlewareLib->bridge;
    }

    return bridge;
}

mamaPayloadBridge
mamaInternal_findPayload (char id)
{
    mamaPayloadLib* payloadLib = NULL;

    if (MAMA_PAYLOAD_NULL == id)
        return NULL;

    payloadLib = gImpl.payloads.byChar[(uint8_t)id];

    if (NULL != payloadLib)
    {
        return payloadLib->bridge;
    }
    else
    {
        return NULL;
    }
}

mamaPayloadBridge
mamaInternal_getDefaultPayload (void)
{
    return gDefaultPayload;
}

mama_bool_t
mamaInternal_getAllowMsgModify (void)
{
    return gAllowMsgModify;
}

mama_status
mama_openWithPropertiesCount (const char* path,
                              const char* filename,
                              unsigned int* count)
{
    mama_status         result                  = MAMA_STATUS_OK;
    mama_size_t         numBridges              = 0;
    mamaMiddleware      middleware              = 0;
    const char*         appString               = NULL;
    const char*         prop                    = NULL;
    int                 bridgeIdx               = 0;
    mamaPayloadBridge   payloadImpl             = NULL;
    char**              payloadName;
    char*               payloadId;
    mamaPayloadLib*     payloadLib;

    wthread_static_mutex_lock (&gImpl.myLock);

    if (wthread_key_create(&last_err_key, NULL) != 0)
    {
        mama_log (MAMA_LOG_LEVEL_NORMAL, "WARNING!!! - CANNOT ALLOCATE KEY FOR ERRORS");
    }

    /* If appName is not specified, set a default */
    appString = NULL;
    mama_getApplicationName(&appString);
    if (NULL==appString)
      mama_setApplicationName("MamaApplication");

    /* If appClass is not specified, set a default */
    appString = NULL;
    mama_getApplicationClassName(&appString);
    if (NULL==appString)
      mama_setApplicationClassName("MamaApplications");

#ifdef WITH_CALLBACK_RETURN
    mama_log (MAMA_LOG_LEVEL_WARN,
            "********************************************************");
    mama_log (MAMA_LOG_LEVEL_WARN, "WARNING!!! - In callback return mode."
                                      " Do not release!!!");
    mama_log (MAMA_LOG_LEVEL_WARN,
            "********************************************************");
#endif

#ifdef WITH_UNITTESTS
    mama_log (MAMA_LOG_LEVEL_WARN,
            "********************************************************");
    mama_log (MAMA_LOG_LEVEL_WARN, "WARNING!!! - Built for unittesting."
                                      " Do not release!!!");
    mama_log (MAMA_LOG_LEVEL_WARN,
            "********************************************************");
#endif

    if (0 != gImpl.myRefCount)
    {
        gImpl.myRefCount++;
        
        if (count)
            *count = gImpl.myRefCount;

        wthread_static_mutex_unlock (&gImpl.myLock);
        return result;
    }
    /* Code after this point is one-time initialization */

#ifdef DEV_RELEASE
    mama_log (MAMA_LOG_LEVEL_WARN,
                "\n********************************************************************************\n"
                "Warning: This is a developer release and has only undergone basic sanity checks.\n"
                "It is for testing only and should not be used in a production environment\n"
                "**********************************************************************************");
#endif /* BAR_RELEASE */

    mamaInternal_loadProperties (path, filename);

    initReservedFields();
    mama_loginit();

    /* Gather global named thread properties. */
    properties_ForEach (mamaInternal_getProperties(), threadPropertiesCb, NULL);

    /* Load all specified Entitlements Bridges. This is done before payloads/middlewares
     * as an entitlementsBridge is necessary for transport_create().
     */
    while (NULL != gEntitlementBridges[bridgeIdx])
    {
        mama_log(MAMA_LOG_LEVEL_FINE,
                 "Trying to load %s entitlement bridge.",
                 gEntitlementBridges[bridgeIdx]);

        result = mama_loadEntitlementBridgeInternal(gEntitlementBridges[bridgeIdx]);

        if (MAMA_STATUS_OK != result)
        {
            mama_log(MAMA_LOG_LEVEL_SEVERE,
                     "mama_openWithProperties(): "
                     "Could not load %s entitlements library.",
                     gEntitlementBridges[bridgeIdx]);

            wthread_static_mutex_unlock (&gImpl.myLock);
            mama_close();

            if (count)
                *count = gImpl.myRefCount;

            return result;
        }
        bridgeIdx++;
    }


    /* Check mama.properteis for default payload bridge to use when calling mamaMsg_create. 
     * The payload bridge will be loaded before any middleware or payload bridges.
     */
    if (!gDefaultPayload)
    {
        prop = properties_Get (gProperties, "mama.payload.default");
        if (prop != NULL)
        {
            mama_log(MAMA_LOG_LEVEL_FINE, 
                     "mama.payload.default=%s detected, setting default payload.",
                     prop);

            result = mama_loadPayloadBridge (&payloadImpl, prop);
            if (MAMA_STATUS_OK != result)
            {
                mama_log(MAMA_LOG_LEVEL_ERROR, 
                         "Could not load defauult payload [%s].",
                         prop);
            }

            payloadLib = (mamaPayloadLib*)wtable_lookup (gImpl.payloads.table,
                                                         prop);
            if (NULL != payloadLib)
            {
                mama_setDefaultPayload(payloadLib->id);
            }
        }
    }

    /* Read any payload bridge names from the properties file, and load 
     * them before any of the other middleware bridges.
     */
    result = mamaInternal_autoloadPayloadBridges ( );

    /* Iterate the loaded middleware bridges, check for their default payloads,
     * and make sure they have been loaded.
     *
     * Note: At present we ignore the return status of the loadPayloadBridge
     * call within the iteration, though we may wish to resolve that.
     */
    for (middleware = 0; middleware != gImpl.middlewares.count; ++middleware)
    {
        mamaMiddlewareLib*  middlewareLib   = gImpl.middlewares.byIndex[middleware];

        if (middlewareLib && middlewareLib->bridge)
        {
            if (middlewareLib->bridge
                             ->bridgeGetDefaultPayloadId (&payloadName, &payloadId) == MAMA_STATUS_OK)
            {
                uint8_t i = 0;
                while (payloadId[i] != MAMA_PAYLOAD_NULL)
                {
                    if (!gImpl.payloads.byChar[payloadId[i]])
                    {
                        mamaPayloadBridge payloadImpl = NULL;
                        mama_loadPayloadBridge (&payloadImpl, payloadName[i]);
                    }
                    i++;
                }
            }
        }
    }

    /* This will initialise all plugins */
    mama_initPlugins();

    prop = properties_Get (gProperties, "mama.catchcallbackexceptions.enable");
    if (prop != NULL && strtobool(prop))
    {
        gCatchCallbackExceptions = 1;
    }

    prop = properties_Get (gProperties, "mama.message.allowmodify");
    if (prop && (!strtobool (prop)))
    {
        gAllowMsgModify = 0;
        mama_log (MAMA_LOG_LEVEL_FINE, "mama.message.allowmodify: false");
    }

    if (strlen( (char*) gEntitlementBridges))
    {
        mama_log (MAMA_LOG_LEVEL_WARN, "%s (entitled)",mama_version);
    }
    else
    {
        mama_log (MAMA_LOG_LEVEL_WARN, "%s (non entitled)",mama_version);
    }
    
    /* Iterate the currently loaded middleware bridges, log their version, and
     * increment the count of open bridges.
     */
    for (middleware = 0; middleware != gImpl.middlewares.count; ++middleware)
    {
        mamaMiddlewareLib* middlewareLib 
            = (mamaMiddlewareLib*)gImpl.middlewares.byIndex[middleware];

        if (middlewareLib)
        {
            mama_log (MAMA_LOG_LEVEL_WARN,
                    middlewareLib->bridge->bridgeGetVersion());

            /* Increment the reference count for each bridge loaded */
            ++numBridges;
        }
    }

    if (0 == numBridges)
    {
        cleanupReservedFields();

        mama_log (MAMA_LOG_LEVEL_SEVERE,
                  "mama_openWithProperties(): "
                  "At least one bridge must be specified");
        if (count)
            *count = gImpl.myRefCount;
        
        wthread_static_mutex_unlock (&gImpl.myLock);
        return MAMA_STATUS_NO_BRIDGE_IMPL;
    }

    if (!gDefaultPayload)
    {
        cleanupReservedFields();

        mama_log (MAMA_LOG_LEVEL_SEVERE,
                  "mama_openWithProperties(): "
                  "At least one payload must be specified");
        if (count)
            *count = gImpl.myRefCount;
        
        wthread_static_mutex_unlock (&gImpl.myLock);
        return MAMA_STATUS_NO_BRIDGE_IMPL;
    }


    mama_statsInit();

    gImpl.myRefCount++;
    if (count)
        *count = gImpl.myRefCount;
    wthread_static_mutex_unlock (&gImpl.myLock);
    return result;
}

static void
threadPropertiesCb (const char* name, const char* value, void* closure)
{
    wombatThreadStatus status;

    if (strncmp (name, MAMA_PROPERTY_THREAD_AFFINITY,
                 strlen (MAMA_PROPERTY_THREAD_AFFINITY)) == 0)
    {
        const char*      threadName = name + strlen(MAMA_PROPERTY_THREAD_AFFINITY);
        CPU_AFFINITY_SET affinity;
        int              i;
        char*            delimiter;
        char*            str = (char*) value;

        for (i = 0; i < sizeof(CPU_AFFINITY_SET); i++)
        {
            /* Clear the affinity set. */
            ((char*) (&affinity))[i] = 0x00;
        }

        for(;;)
        {
            i = strtoul(str, &delimiter, 10);

            if (i < 8 * sizeof(CPU_AFFINITY_SET))
            {
                CPU_SET(i, &affinity);
            }
            if (*delimiter != ',')
            {
                if (*delimiter != '\0') return;
                break;
            }
            str = ++delimiter;
        }

        status = wombatThread_setAffinity (threadName, &affinity);

        if (status != WOMBAT_THREAD_OK)
        {
            mama_log(MAMA_LOG_LEVEL_NORMAL,
                "threadPropertiesCb: "
                " Unable to set affinity for thread '%s' to value '%s'",
                threadName,
                value);
        }
        else
        {
            mama_log(MAMA_LOG_LEVEL_NORMAL,
                "threadPropertiesCb: "
                "Set affinity for thread '%s' to value '%s'",
                threadName,
                value);
        }
    }
}

mama_status
mama_statsInit (void)
    {
    mamaMiddleware middleware = 0;
    char* statsLogging = (char*) properties_Get (gProperties, "mama.statslogging.enable");

    if ( (statsLogging != NULL) && strtobool (statsLogging))
    {
        const char*     propVal           = NULL;

        /* If logging has been explicitly set false, and publishing is also set false, then don't
           generate stats (and neither log nor publish). */

        propVal           = properties_Get (gProperties, "mama.statslogging.global.logging");
        if ( propVal != NULL)
            gLogGlobalStats = strtobool(propVal);

        propVal           = properties_Get (gProperties, "mama.statslogging.global.publishing");
        if ( propVal != NULL)
            gPublishGlobalStats = strtobool(propVal);

        propVal           = properties_Get (gProperties, "mama.statslogging.transport.logging");
        if ( propVal != NULL)
            gLogTransportStats = strtobool(propVal);

        propVal           = properties_Get (gProperties, "mama.statslogging.transport.publishing");
        if ( propVal != NULL)
            gPublishTransportStats = strtobool(propVal);

        propVal           = properties_Get (gProperties, "mama.statslogging.queue.logging");
        if ( propVal != NULL)
            gLogQueueStats = strtobool(propVal);

        propVal           = properties_Get (gProperties, "mama.statslogging.queue.publishing");
        if ( propVal != NULL)
            gPublishQueueStats = strtobool(propVal);

        propVal           = properties_Get (gProperties, "mama.statslogging.lbm.logging");
        if ( propVal != NULL)
            gLogLbmStats = strtobool(propVal);

        propVal           = properties_Get (gProperties, "mama.statslogging.lbm.publishing");
        if ( propVal != NULL)
            gPublishLbmStats = strtobool(propVal);

        propVal           = properties_Get (gProperties, "mama.statslogging.user.logging");
        if ( propVal != NULL)
            gLogUserStats = strtobool(propVal);

        propVal           = properties_Get (gProperties, "mama.statslogging.user.publishing");
        if ( propVal != NULL)
            gPublishUserStats = strtobool(propVal);
        
        if (gLogGlobalStats || gPublishGlobalStats) gGenerateGlobalStats=1;
        if (gLogTransportStats || gPublishTransportStats) gGenerateTransportStats=1;
        if (gLogQueueStats || gPublishQueueStats) gGenerateQueueStats=1;
        if (gLogLbmStats || gPublishLbmStats) gGenerateLbmStats=1;
        if (gLogUserStats || gPublishUserStats) gGenerateUserStats=1;
          
        mama_setupStatsGenerator();

        mamaInternal_enableStatsLogging();

        /* Iterate each bridge, and call the enableStats method on the default
         * queue for each.
         */
        for (middleware = 0; middleware != gImpl.middlewares.count; ++middleware)
        {
            mamaMiddlewareLib* middlewareLib = 
                (mamaMiddlewareLib*)gImpl.middlewares.byIndex[middleware];

            if (middlewareLib && middlewareLib->bridge)
            {
                mamaQueue_enableStats (middlewareLib->bridge->mDefaultEventQueue);
            }
        }
    }

    return MAMA_STATUS_OK;
}

mama_status
mama_setupStatsGenerator (void)
{
    mama_status result = MAMA_STATUS_OK;

    const char*     statsIntervalStr    = NULL;
    mamaQueue       statsGenQueue       = NULL;

    statsIntervalStr = properties_Get (gProperties, "mama.statslogging.interval");

    if (MAMA_STATUS_OK != (result = mamaStatsGenerator_create(
                                    &gStatsGenerator,
                                    statsIntervalStr ? atof (statsIntervalStr) : DEFAULT_STATS_INTERVAL)))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mama_openWithProperties(): "
                  "Could not create stats generator.");

        return result;
    }
    
    /* No publishing, therefore no middleware needs to be specified
       in mama.properties.  Instead, check through loaded bridges */
    if (!mamaInternal_statsPublishingEnabled())
    {
        mamaBridgeImpl* impl = (mamaBridgeImpl*) mamaInternal_findBridge ();

        if (impl != NULL)
        {
            statsGenQueue = impl->mDefaultEventQueue;
        }
    }
    else
    {
        /* Stats publishing enabled, therefore use the mama.statslogging.middleware
           property */
        mamaBridge bridge;

        const char* statsMiddleware = NULL;
        statsMiddleware = properties_Get (gProperties, "mama.statslogging.middleware");

        if (!statsMiddleware)
        {
            statsMiddleware = "wmw";
        }

        mama_loadBridge(&bridge, statsMiddleware);

        if (MAMA_STATUS_OK != (result = mamaBridgeImpl_getInternalEventQueue (bridge,
                                                           &statsGenQueue)))
        {
            return result;
        }
    }

    if (MAMA_STATUS_OK != (result = mamaStatsGenerator_setQueue (gStatsGenerator, statsGenQueue)))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mama_openWithProperties(): "
                  "Could not set queue for stats generator.");
            return result;
    }

    mamaStatsGenerator_setLogStats (gStatsGenerator, 1);

    return result;
}

mama_status
mama_open ()
{
    /*Passing NULL as path and filename will result in the
     default behaviour - mama.properties on $WOMBAT_PATH*/
    return mama_openWithPropertiesCount (NULL, NULL, NULL);
}

mama_status
mama_openCount (unsigned int* count)
{
    /*Passing NULL as path and filename will result in the
     default behaviour - mama.properties on $WOMBAT_PATH*/
    return mama_openWithPropertiesCount (NULL, NULL, count);
}

mama_status
mama_openWithProperties (const char* path,
                         const char* filename)
{
    return mama_openWithPropertiesCount (path, filename, NULL);
}

mama_status
mama_setProperty (const char* name,
                  const char* value)
{
    if (!gProperties)
    {
       mamaInternal_loadProperties( NULL, NULL );
    }

    if (!name||!value)
        return MAMA_STATUS_NULL_ARG;

    mama_log (MAMA_LOG_LEVEL_NORMAL,"Setting property: %s with value: %s",
                                   name, value);

    if( 0 == properties_setProperty (gProperties,
                                     name,
                                     value))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
            "mama_setProperty(): Failed to set property");
        return MAMA_STATUS_PLATFORM;
    }
    return MAMA_STATUS_OK;
}

mama_status
mama_setPropertiesFromFile (const char *path,
                            const char *filename)
{
    if (!path || !filename)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    mamaInternal_loadProperties (path, filename);

    return MAMA_STATUS_OK;
}

const char *
mama_getProperty (const char *name)
{
    if( !gProperties )
    {
        mamaInternal_loadProperties( NULL, NULL );
    }

    if( !name )
        return NULL;

    return properties_Get( gProperties, name );
}

const char*
mama_getVersion (mamaBridge bridgeImpl)
{
    mamaBridgeImpl* impl =  (mamaBridgeImpl*)bridgeImpl;

    if (!impl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mama_getVersion(): No bridge implementation specified");
        return NULL;
    }

    /*Delegate the call to the bridge specific implementation*/
    if (strlen( (char*) gEntitlementBridges))
    {
        snprintf(mama_ver_string,sizeof(mama_ver_string),"%s (%s) (entitled)",
                mama_version, impl->bridgeGetVersion ());
    }
    else
    {
        snprintf(mama_ver_string,sizeof(mama_ver_string),"%s (%s) (non entitled)",
                mama_version, impl->bridgeGetVersion ());
    }

    return mama_ver_string;
}

mama_status
mama_closeCount (unsigned int* count)
{
    mama_status    result     = MAMA_STATUS_OK;
    mamaMiddleware middleware = 0;
    int payload = 0;
    int bridgeCount = 0;

    wthread_static_mutex_lock (&gImpl.myLock);
    if (gImpl.myRefCount == 0)
    {
        if (count)
            *count = gImpl.myRefCount;
        wthread_static_mutex_unlock (&gImpl.myLock);
        return MAMA_STATUS_OK;
    }

    if (!--gImpl.myRefCount)
    {
        for (bridgeCount=0 ; bridgeCount != gImpl.entitlements.count ; bridgeCount++)
        {
            mamaEntitlementLib* entLib = gImpl.entitlements.byIndex[bridgeCount];
            if (entLib)
            {
                /* Remove from indexed list of entitlements libraries. */
                gImpl.entitlements.byIndex[bridgeCount] = NULL;

                /* Destroy the MAMA-level bridge and underlying implementation struct. */    
                mamaEntitlementBridge_destroy(entLib->bridge);
                entLib->bridge = NULL;

                /* Destroy library struct. */
                if (entLib->library)
                {
                    closeSharedLib (entLib->library);
                    entLib->library = NULL;
                }
                free (entLib);
            }
        }

        /* Once the libraries have been unloaded, clear down the wtable. */
        wtable_clear (gImpl.entitlements.table);

        /* Reset the count of loaded entitlements libraries */
        gImpl.entitlements.count = 0;

        wthread_key_delete(last_err_key);

        if (gStatsGenerator)
        {
            mamaStatsGenerator_stopReportTimer(gStatsGenerator);
        }

        /* Iterate the loaded bridges, and stop the internal event queue for
         * each.
         */
        for (middleware = 0; middleware != gImpl.middlewares.count; ++middleware)
        {
            char threadname[256];
            mamaMiddlewareLib* middlewareLib =
                        (mamaMiddlewareLib*)gImpl.middlewares.byIndex[middleware];

            if (middlewareLib && middlewareLib->bridge)
            {
                mamaBridgeImpl_stopInternalEventQueue (middlewareLib->bridge);
            }
        }

        if (gInitialStat)
        {
            mamaStat_destroy (gInitialStat);
            gInitialStat = NULL;
        }

        if (gRecapStat)
        {
            mamaStat_destroy (gRecapStat);
            gRecapStat = NULL;
        }

        if (gUnknownMsgStat)
        {
            mamaStat_destroy (gUnknownMsgStat);
            gUnknownMsgStat = NULL;
        }

        if (gMessageStat)
        {
            mamaStat_destroy (gMessageStat);
            gMessageStat = NULL;
        }

        if (gFtTakeoverStat)
        {
            mamaStat_destroy (gFtTakeoverStat);
            gFtTakeoverStat = NULL;
        }

        if (gSubscriptionStat)
        {
            mamaStat_destroy (gSubscriptionStat);
            gSubscriptionStat = NULL;
        }

        if (gTimeoutStat)
        {
            mamaStat_destroy (gTimeoutStat);
            gTimeoutStat = NULL;
        }

        if (gWombatMsgsStat)
        {
            mamaStat_destroy (gWombatMsgsStat);
            gWombatMsgsStat = NULL;
        }

        if (gFastMsgsStat)
        {
            mamaStat_destroy (gFastMsgsStat);
            gFastMsgsStat = NULL;
        }

        if (gRvMsgsStat)
        {
            mamaStat_destroy (gRvMsgsStat);
            gRvMsgsStat = NULL;
        }

        if (gPublisherSend)
        {
            mamaStat_destroy (gPublisherSend);
            gPublisherSend = NULL;
        }

        if (gPublisherInboxSend)
        {
            mamaStat_destroy (gPublisherInboxSend);
            gPublisherInboxSend = NULL;
        }

        if (gPublisherReplySend)
        {
            mamaStat_destroy (gPublisherReplySend);
            gPublisherReplySend = NULL;
        }

        if (gGlobalStatsCollector)
        {
            if (gStatsGenerator)
            {
                mamaStatsGenerator_removeStatsCollector (gStatsGenerator, gGlobalStatsCollector);
            }
            mamaStatsCollector_destroy (gGlobalStatsCollector);
            gGlobalStatsCollector = NULL;
        }

        if (gStatsPublisher)
        {
            mamaStatsLogger_destroy (gStatsPublisher);
            gStatsPublisher = NULL;
        }

                /* Destroy the stats generator after the bridge is closed so we will
           have removed the default queue stats collector */
        if (gStatsGenerator)
        {
            mamaStatsGenerator_destroy (gStatsGenerator);
            gStatsGenerator = NULL;
        }

        cleanupReservedFields();

        /* Iterate over the payloads which have been loaded, and unload each
         * library in turn.
         */
        for (payload = 0; payload != MAMA_PAYLOAD_MAX; ++payload)
        {
            mamaPayloadLib* payloadLib =
                    (mamaPayloadLib *)gImpl.payloads.byChar[payload];

            if (payloadLib)
            {
                /* Remove the pointer from the payload cache */
                gImpl.payloads.byChar[payload] = NULL;

                /* The memory for the payloadBridge struct is presently allocated
                 * by the bridge itself. However, there is no mechanism via which
                 * it can be freed during close. Therefore we free it here.
                 *
                 * Note: There is a risk here if both the bridge and MAMA are
                 * built with different version, we may trigger some undefined
                 * behaviour via this free,
                 */
                free (payloadLib->bridge);
                payloadLib->bridge  = NULL;
                payloadLib->id      = MAMA_PAYLOAD_NULL;

                if(payloadLib->library)
                {
                    closeSharedLib (payloadLib->library);
                    payloadLib->library = NULL;
                }

                /* Free the payloadLib structure */
                free (payloadLib);
            }

        }

        /* Once the libraries have been unloaded, clear down the wtable. */
        wtable_clear (gImpl.payloads.table);

        /* This will shutdown all plugins */
        mama_shutdownPlugins();

        /* Reset the count of loaded payloads */
        gImpl.payloads.count = 0;

        gDefaultPayload = NULL;

        /* Iterate over the loaded middleware bridges, close each bridge, and
         * unload each library in turn.
         */
        for (middleware = 0; middleware != gImpl.middlewares.count; ++middleware)
        {
            mamaMiddlewareLib* middlewareLib =
                        (mamaMiddlewareLib*)gImpl.middlewares.byIndex[middleware];

            if (middlewareLib)
            {
                if (middlewareLib->bridge)
                {
                    if (MAMA_STATUS_OK != middlewareLib->bridge->
                                            bridgeClose (middlewareLib->bridge))
                    {
                        mama_log (MAMA_LOG_LEVEL_ERROR,
                                  "mama_close(): Error closing %s bridge.",
                                  middlewareLib->bridge->bridgeGetName ());
                    }

                    if (middlewareLib->bridge->mLock)
                    {
                        wlock_destroy(middlewareLib->bridge->mLock);
                    }

                    /* If there was a background thread, clean it up */
                    if (middlewareLib->bridge->mStartBackgroundThread)
                    {
                        // Get name of start background thread and destroy it
                        const char* threadName = wombatThread_getThreadName(middlewareLib->bridge->mStartBackgroundThread);
                        wombatThread_destroy (threadName);
                    }

                    free (middlewareLib->bridge);
                    middlewareLib->bridge = NULL;
                }

                if (middlewareLib->library)
                {
                    closeSharedLib (middlewareLib->library);
                    middlewareLib->library = NULL;
                }

                /* Free the middlewareLib struct */
                free (middlewareLib);

                gImpl.middlewares.byIndex[middleware] = NULL;
            }
        }

        /* Once the libraries have been unloaded, clear the middlewareTable. */
        wtable_clear (gImpl.middlewares.table);

        /* Reset the count of loaded middlewares */
        gImpl.middlewares.count = 0;

        /* Destroy any internal properties */
        if (gImpl.internalProperties)
        {
            properties_Free (gImpl.internalProperties);
            gImpl.internalProperties = NULL;
        }

        /* The properties must not be closed down until after the bridges have been destroyed. */
        if (gProperties != 0)
        {
            properties_Free (gProperties);
            gProperties = 0;
        }

        /* Destroy logging */
        mama_logDestroy();

        /* Clean up any timezone related threads which may have been started */
        mamaTimeZone_cleanUp ();

        /* Free application context details. */
        mama_freeAppContext(&appContext);

    }
    if (count)
        *count = gImpl.myRefCount;
    wthread_static_mutex_unlock (&gImpl.myLock);
    return result;
}

mama_status
mama_close (void)
{
    return mama_closeCount (NULL);
}

/**
 * Start processing messages.
 */
mama_status
mama_start (mamaBridge bridgeImpl)
{
    mamaBridgeImpl* impl =  (mamaBridgeImpl*)bridgeImpl;
    mama_status rval = MAMA_STATUS_OK;
    unsigned int prevRefCnt = 0;

    if (!impl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mama_start(): No bridge implementation specified");
        return MAMA_STATUS_NO_BRIDGE_IMPL;
    }

    if (!impl->mDefaultEventQueue)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mama_start(): NULL default queue. "
                  "Has mama_open() been called?");
        return MAMA_STATUS_INVALID_QUEUE;
    }

    wthread_static_mutex_lock(&gImpl.myLock);
    prevRefCnt = impl->mRefCount++;
    wthread_static_mutex_unlock(&gImpl.myLock);

    if (prevRefCnt > 0)
        return MAMA_STATUS_OK;

    /* Delegate to the bridge specific implementation */
    /* Can't hold lock because bridgeStart blocks */
    rval =  impl->bridgeStart (impl->mDefaultEventQueue);

    if (rval != MAMA_STATUS_OK)
    {
        wthread_static_mutex_lock(&gImpl.myLock);
        impl->mRefCount--;
        wthread_static_mutex_unlock(&gImpl.myLock);
    }

    return rval;
}

struct startBackgroundClosure
{
    mamaStopCB   mStopCallback;
    mamaStopCBEx mStopCallbackEx;
    mamaBridge   mBridgeImpl;
    void*        mClosure;
};

static void* mamaStartThread (void* closure)
{
    struct startBackgroundClosure* cb =
                (struct startBackgroundClosure*)closure;
    mama_status rval = MAMA_STATUS_OK;

    if (!cb) return NULL;

    rval = mama_start (cb->mBridgeImpl);

    if (cb->mStopCallback)
        cb->mStopCallback (rval);

    if (cb->mStopCallbackEx)
        cb->mStopCallbackEx (rval, cb->mBridgeImpl, cb->mClosure);

    /* Free the closure object */
    free(cb);

    return NULL;
}

static mama_status
mama_startBackgroundHelper (mamaBridge   bridgeImpl,
                            mamaStopCB   callback,
                            mamaStopCBEx exCallback,
                            void*        closure)
{
    struct startBackgroundClosure*  closureData;
    mamaBridgeImpl*     impl         = (mamaBridgeImpl*)bridgeImpl;
    wombatThreadStatus  threadStatus = WOMBAT_THREAD_OK;
    wombatThread        thread;
    char                threadname[256];

    if (!bridgeImpl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mama_startBackgroundHelper(): NULL bridge "
                  " impl.");
        return MAMA_STATUS_NO_BRIDGE_IMPL;
    }

    if (!callback && !exCallback)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mama_startBackgroundHelper(): No "
                  "stop callback or extended stop callback specified.");
        return MAMA_STATUS_INVALID_ARG;
    }

    if (callback && exCallback)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mama_startBackgroundHelper(): Both "
                "stop callback and extended stop callback specified.");
        return MAMA_STATUS_INVALID_ARG;
    }

    closureData = calloc (1, (sizeof (struct startBackgroundClosure)));

    if (!closureData)
        return MAMA_STATUS_NOMEM;

    closureData->mStopCallback   = callback;
    closureData->mStopCallbackEx = exCallback;
    closureData->mBridgeImpl     = bridgeImpl;
    closureData->mClosure        = closure;

    snprintf (threadname, 256, "mama_%s_default", bridgeImpl->bridgeGetName());

    mama_log (MAMA_LOG_LEVEL_FINER, "mama_startBackgroundHelper (): "
              "Creating new background thread (name=%s).", threadname);

    threadStatus = wombatThread_create(threadname,
                            &impl->mStartBackgroundThread,
                            NULL,
                            mamaStartThread,
                            (void*) closureData);

    if (threadStatus == WOMBAT_THREAD_PROPERTY)
    {
        /* Failed to set the thread affinity, but the thread has been created
         * so log an error but carry on. */
        mama_log (MAMA_LOG_LEVEL_ERROR, "mama_startBackgroundHelper(): Could not "
                  "apply thread affinity to "
                  "background MAMA thread.");
    }
    else if (threadStatus != WOMBAT_THREAD_OK)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mama_startBackgroundHelper(): Could not "
                  "start background MAMA thread.");
        return MAMA_STATUS_SYSTEM_ERROR;
    }

    return MAMA_STATUS_OK;
}

mama_status
mama_startBackground (mamaBridge bridgeImpl, mamaStartCB callback)
{
    /* Passing these NULLs tells mama_startBackgroundHelper to use old functionality */
    return mama_startBackgroundHelper (bridgeImpl, (mamaStopCB)callback, NULL, NULL);
}

mama_status
mama_startBackgroundEx (mamaBridge bridgeImpl, mamaStopCBEx exCallback, void* closure)
{
    /* Passing this NULL tells mama_StartBackgroundHelper to use new functionality */
    return mama_startBackgroundHelper (bridgeImpl, NULL, exCallback, closure);
}

/**
 * Stop processing messages
 */
mama_status
mama_stop (mamaBridge bridgeImpl)
{
    mamaBridgeImpl* impl =  (mamaBridgeImpl*)bridgeImpl;
    mama_status rval = MAMA_STATUS_OK;

    if (!impl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mama_stop(): No bridge implementation specified");
        return MAMA_STATUS_NO_BRIDGE_IMPL;
    }

    if (!impl->mDefaultEventQueue)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mama_stop(): NULL default queue. Has mama_open() been "
                  "called?");
        return MAMA_STATUS_INVALID_QUEUE;
    }

    /*Delegate to the bridge specific implementation*/
    wthread_static_mutex_lock(&gImpl.myLock);
    if (impl->mRefCount > 0)
    {
        impl->mRefCount--;
        if(impl->mRefCount == 0)
        {
            rval = impl->bridgeStop (impl->mDefaultEventQueue);
            if (MAMA_STATUS_OK != rval)
                impl->mRefCount++;
        }
    }
    wthread_static_mutex_unlock(&gImpl.myLock);
    return rval;
}

/**
 * Stops all the bridges.
 */
mama_status
mama_stopAll (void)
{
    mama_status  result      = MAMA_STATUS_OK;
    mamaMiddleware middleware = 0;

    /* Iterate the loaded bridges and call mama_stop for each. */
    for (middleware = 0; middleware != gImpl.middlewares.count; ++middleware)
    {
        mamaMiddlewareLib* middlewareLib =
                    (mamaMiddlewareLib*)gImpl.middlewares.byIndex[middleware];

        if (middlewareLib && middlewareLib->bridge)
        {
            result = mama_stop (middlewareLib->bridge);
            if (MAMA_STATUS_OK != result)
            {
                mama_log (MAMA_LOG_LEVEL_ERROR,
                          "mama_stopAll(): Error stopping [%s] bridge.",
                          middlewareLib->bridge->bridgeGetName ());

            }
        }
    }

    /* NOTE: We're doing this to maintain consistency with the previous
     * implementation, though there are issues - multiple failures
     * will still only pass back the error status of the last one.
     */
    return result;
}

mama_status
mama_getDefaultEventQueue (mamaBridge bridgeImpl,
                           mamaQueue* defaultQueue)
{
    mamaBridgeImpl* impl =  (mamaBridgeImpl*)bridgeImpl;

    if (!impl)
    {
        mama_log (MAMA_LOG_LEVEL_WARN, "mama_getDefaultEventQueue(): "
                  "No bridge implementation specified");
        return MAMA_STATUS_NO_BRIDGE_IMPL;
    }

    if (!impl->mDefaultEventQueue)
    {
        mama_log (MAMA_LOG_LEVEL_WARN, "mama_getDefaultEventQueue (): "
                  "NULL default queue for bridge impl. Has mama_open() been "
                  "called?");
        return MAMA_STATUS_INVALID_QUEUE;
    }

    *defaultQueue = impl->mDefaultEventQueue;
    return MAMA_STATUS_OK;
}

mama_status
mamaImpl_setDefaultEventQueue (mamaBridge bridgeImpl,
                               mamaQueue defaultQueue)
{
    mamaBridgeImpl* impl =  (mamaBridgeImpl*)bridgeImpl;

    if (NULL == impl)
    {
        mama_log (MAMA_LOG_LEVEL_WARN, "mama_setDefaultEventQueue(): "
                  "No bridge implementation specified");
        return MAMA_STATUS_NO_BRIDGE_IMPL;
    }

    if (NULL == defaultQueue)
    {
        mama_log (MAMA_LOG_LEVEL_WARN, "mama_setDefaultEventQueue (): "
                  "NULL default queue for bridge impl provided.");
        return MAMA_STATUS_NULL_ARG;
    }

    impl->mDefaultEventQueue = defaultQueue;
    return MAMA_STATUS_OK;
}

mama_status
mama_getUserName (const char** userName)
{
    if (userName == NULL)
        return MAMA_STATUS_NULL_ARG;

    *userName = getlogin();
    return MAMA_STATUS_OK;
}

mama_status
mama_getHostName (const char** hostName)
{
    if (hostName == NULL) return MAMA_STATUS_NULL_ARG;
    *hostName = getHostName();
    return MAMA_STATUS_OK;
}

mama_status
mama_getIpAddress (const char** ipAddress)
{
    if (ipAddress == NULL) return MAMA_STATUS_NULL_ARG;
    *ipAddress = getIpAddress();
    return MAMA_STATUS_OK;
}

mama_status
mama_registerEntitlementCallbacks (const mamaEntitlementCallbacks* entitlementCallbacks)
{
    if (entitlementCallbacks == NULL) return MAMA_STATUS_NULL_ARG;
    gEntitlementCallbacks = *entitlementCallbacks;
    return MAMA_STATUS_OK;
}
 
void MAMACALLTYPE mamaImpl_entitlementDisconnectCallback (
                            const sessionDisconnectReason  reason,
                            const char * const             userId,
                            const char * const             host,
                            const char * const             appName)
{
    if (gEntitlementCallbacks.onSessionDisconnect != NULL)
    {
        gEntitlementCallbacks.onSessionDisconnect (reason, userId, host, appName);
    }
}

void MAMACALLTYPE mamaImpl_entitlementUpdatedCallback ()
{
    if (gEntitlementCallbacks.onEntitlementUpdate != NULL)
    {
        gEntitlementCallbacks.onEntitlementUpdate();
    }
}

void MAMACALLTYPE mamaImpl_entitlementCheckingSwitchCallback (
                            int isEntitlementsCheckingDisabled)
{
    if (gEntitlementCallbacks.onEntitlementCheckingSwitch != NULL)
    {
        gEntitlementCallbacks.onEntitlementCheckingSwitch(isEntitlementsCheckingDisabled);
    }
}


MAMADeprecated("mamaInternal_registerBridge has been deprecated, use dynamic loading instead!")
void
mamaInternal_registerBridge (mamaBridge     bridge,
                             const char*    middlewareName)
{
    /* NO-OP */
}

mama_status
mama_setDefaultPayload (char id)
{
    if (MAMA_PAYLOAD_NULL == id || gImpl.payloads.byChar[(uint8_t)id] == NULL)
        return MAMA_STATUS_NULL_ARG;

    gDefaultPayload = gImpl.payloads.byChar[(uint8_t)id]->bridge;

    return MAMA_STATUS_OK;
}

mama_status
mama_loadPayloadBridge (mamaPayloadBridge* impl,
                         const char*        payloadName)
{
    return mama_loadPayloadBridgeInternal (impl, payloadName);
}


mama_status
mama_loadPayloadBridgeInternal  (mamaPayloadBridge* impl,
                                 const char*        payloadName)
{
    mama_status             status          = MAMA_STATUS_OK;

    mamaPayloadLib*         payloadLib       = NULL;
    LIB_HANDLE              payloadLibHandle = NULL;

    char                    payloadChar     = MAMA_PAYLOAD_NULL;
    versionInfo             bridgeMamaVersion;
    char                    payloadImplName [256];
    char                    initFuncName    [256];
    char                    payloadVersion  [MAX_INTERNAL_PROP_LEN];
    const char*             suppressLoadFailLogging;
    msgPayload_init         initFunc        = NULL;
    void*                   vp              = NULL;

    /* Indicates if the payload bridge struct was allocated by MAMA or
     * by the bridge 
     */
    if (!impl || !payloadName)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Ensure we have initialised the appropriate structures before we attempt
     * to load a new payload.
     */
    status = mamaInternal_init ();

    if (MAMA_STATUS_OK != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mama_loadPayloadBridgeInternal (): "
                  "Error initialising internal MAMA state. Cannot load payload"
                  "[%s]", payloadName);
        return status;
    }

    /* Lock here as we don't want anything else being added to the table
     * until either we've returned the appropriate bridge, or loaded and added
     * the new one.
     */
    wthread_static_mutex_lock (&gImpl.myLock);

    payloadLib = (mamaPayloadLib*)wtable_lookup (gImpl.payloads.table,
                                                 payloadName);

    if (payloadLib && payloadLib->bridge)
    {
        status = MAMA_STATUS_OK;
        mama_log (MAMA_LOG_LEVEL_NORMAL,
                  "mama_loadPayloadBridgeInternal (): "
                  "Payload bridge [%s] already loaded. Returning previously loaded bridge.",
                  payloadName);

        /* Return the existing payload bridge implementation */
        *impl = payloadLib->bridge;
        goto error_handling_unlock;
    }

    /* Once we have checked if the payload has already been loaded, check if
     * we've already loaded the maximum number of bridges allowed,
     * MAMA_PAYLOAD_MAX. If we have, report an error and return.
     */
    if (gImpl.payloads.count >= MAMA_PAYLOAD_MAX)
    {
        status = MAMA_STATUS_NO_BRIDGE_IMPL;
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mama_loadPayloadBridgeInternal (): "
                  "Maximum number of available payload bridges has been loaded. Cannot load [%s].",
                  payloadName);
        goto error_handling_unlock;
    }

    snprintf (payloadImplName, 256, "mama%simpl", payloadName);

    payloadLibHandle = openSharedLib (payloadImplName, NULL);

    if (!payloadLibHandle)
    {
        status = MAMA_STATUS_NO_BRIDGE_IMPL;

        /* In some instances middleware's attempt to load payloads that may
         * not be available to all clients. As this results in an 'ERROR' logged
         * we want to be able to suppress this where it's an understood problem.
         *
         * Note: We don't want to suppress the other logs, as they would
         * indicate real problems that need fixed.
         */
        suppressLoadFailLogging = mama_getProperty ("mama.payload.suppress_load_failure_logging");

        if (NULL == suppressLoadFailLogging ||
            (!strtobool (suppressLoadFailLogging)))
        {
            mama_log (MAMA_LOG_LEVEL_ERROR,
                     "mama_loadPayloadBridge(): "
                      "Could not open payload bridge library [%s] [%s]",
                      payloadImplName,
                      getLibError());
        }
        goto error_handling_unlock;
    }

    /* Begin by searching for the *Payload_init function */
    snprintf (initFuncName, 256, "%sPayload_init",  payloadName);

    vp       = loadLibFunc (payloadLibHandle, initFuncName);
    initFunc = *(msgPayload_init*) &vp;

    if (NULL == initFunc)
    {
        status = MAMA_STATUS_INVALID_ARG;
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mama_loadPayloadBridgeInternal (): "
                  "Failed to initialise payload bridge [%s]. "
                  "Cannot find function %s in implementation library.",
                  payloadName,
                  initFuncName);
        goto error_handling_impl;
    }

    /* Allocate and initialise the bridge, using function search to populate. */
    *impl = calloc (1, sizeof (mamaPayloadBridgeImpl));

    if (NULL == *impl)
    {
        status = MAMA_STATUS_NOMEM;
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mama_loadPayloadBridgeInternal (): "
                  "Failed to allocate memory for payload bridge [%s]. Cannot load payload.",
                  payloadName);
        goto error_handling_impl;
    }

    /* Once the payload has been successfully allocated and initialised,
     * we can use the function search to register various payload functions
     */
    status = mamaInternal_registerPayloadFunctions (payloadLibHandle,
                                                    impl,
                                                    payloadName);

    if (MAMA_STATUS_OK != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mama_loadPayloadBridgeInternal (): "
                  "Failed to register payload functions for [%s] payload bridge from [%s] library.",
                  payloadName,
                  payloadImplName);
        goto error_handling_impl_allocated;
    }

    status = initFunc (*impl, &payloadChar);

    if (MAMA_STATUS_OK != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mama_loadPayloadBridgeInternal (): "
                  "Failed to initialise payload bridge [%s]. Cannot load payload.",
                  payloadName);
        goto error_handling_impl_allocated;
    }

    /* Build payload bridge compile time version property string */
    snprintf (payloadVersion,
              sizeof(payloadVersion),
              MAMA_PROP_BRIDGE_COMPILE_TIME_VER,
              payloadName);

    /* Verify interface compatibility */
    if(!strToVersionInfo(
            mamaInternal_getMetaProperty (payloadVersion),
            &bridgeMamaVersion))
    {
        status = MAMA_STATUS_INVALID_ARG;
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mama_loadBridge (): "
                  "Failed to initialise middleware bridge [%s]. "
                  "Cannot determine bridge's MAMA version.",
                  payloadName);
        goto error_handling_impl_allocated;
    }

    /* Fail to load if bridge and API versions are incompatible */
    if (! mama_areVersionsCompatibleInternal (gImpl.version, bridgeMamaVersion))
    {
        status = MAMA_STATUS_INVALID_ARG;
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mama_loadPayloadBridgeInternal (): "
                  "Failed to initialise payload bridge [%s]. "
                  "MAMA Runtime Version v%d.%d.%d not compatible with bridge's compile version v%d.%d.%d",
                  payloadName,
                  gImpl.version.mMajor,
                  gImpl.version.mMinor,
                  gImpl.version.mRelease,
                  bridgeMamaVersion.mMajor,
                  bridgeMamaVersion.mMinor,
                  bridgeMamaVersion.mRelease);
        goto error_handling_impl_allocated;
    }

    mama_log (MAMA_LOG_LEVEL_FINE,
              "mama_loadPayloadBridgeInternal(): "
              "Loaded payload bridge [%s] compiled against MAMA v%d.%d.%d %s",
              payloadName,
              bridgeMamaVersion.mMajor,
              bridgeMamaVersion.mMinor,
              bridgeMamaVersion.mRelease,
              bridgeMamaVersion.mExtra ? bridgeMamaVersion.mExtra : "");


    /* Return the bridge payload ID, either from properties, or from the bridge
     * itself.
     */
    status = mamaInternal_getPayloadId (payloadName,
                                        *impl,
                                        &payloadChar);

    if (MAMA_STATUS_OK != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mama_loadPayloadBridgeInternal (): "
                  "Cannot determine appropriate payload character. Cannot load [%s] payload bridge.",
                  payloadName);
        goto error_handling_impl_allocated;
    }

    /* Allocate the payloadLib struct, and populate with the payloadBridgeImpl
     * and library handle.
     */
    payloadLib = malloc (sizeof (mamaPayloadLib));

    if (NULL == payloadLib)
    {
        status = MAMA_STATUS_NOMEM;
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mama_loadPayloadBridgeInternal (): "
                  "Failed to allocate memory for [%s] payload library.",
                  payloadName);
        goto error_handling_impl_allocated;
    }

    payloadLib->bridge        = *(mamaPayloadBridge*)impl;
    payloadLib->library       = payloadLibHandle;
    payloadLib->id            = payloadChar;

    status = wtable_insert (gImpl.payloads.table,
                            payloadName,
                            (void*)payloadLib);

    // Update the payload bridge to back reference to this library
    (*impl)->payloadLib = payloadLib;

    /* wtable_insert may return 1 for success, 0 for inserting a duplicate key,
     * -1 for failure of allocation. Unfortunately it also returns 0 for no table,
     * which means we have an abiguity. However, we shouldn't be able to reach
     * this point with a duplicate, so we should generally happy to proceed.
     */
    if (WTABLE_INSERT_SUCCESS != status)
    {
        status = MAMA_STATUS_NO_BRIDGE_IMPL;
        mama_log (MAMA_LOG_LEVEL_ERROR, 
                  "mama_loadPayloadBridgeInternal (): "
                  "Failed to register payload [%s] with MAMA.",
                  payloadName);
        goto error_handling_payloadlib;
    }

    if (gImpl.payloads.byChar[payloadChar])
    {
        status = MAMA_STATUS_SYSTEM_ERROR;
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mama_loadPayloadBridgeInternal (): "
                  "Payload bridge using this character [%c] has already been registered. Cannot load payload bridge [%s]",
                  payloadChar,
                  payloadName);
        goto error_handling_payloadlib;
    }

    /* Add a pointer to the byChar array for rapid access later. */
    gImpl.payloads.byChar[payloadChar] = payloadLib;

    /* Increment the count of loaded payloads. */
    /* Note: Due to the check for the PAYLOAD_MAX limit at the start of the
     * function, we don't need to worry about overflowing here.
     */
    gImpl.payloads.count++;

    if (!gDefaultPayload)
    {
        gDefaultPayload = *impl;
    }

    mama_log (MAMA_LOG_LEVEL_FINE,
              "mama_loadPayloadBridge(): "
              "Sucessfully loaded %s payload bridge from library [%s]",
              payloadName,
              payloadImplName);

    wthread_static_mutex_unlock (&gImpl.myLock);

    status = MAMA_STATUS_OK;
    return status;

error_handling_payloadlib:
    free (payloadLib);
error_handling_impl_allocated:
    free (*impl);
error_handling_impl:
    *impl = NULL;
    closeSharedLib (payloadLibHandle);
error_handling_unlock:
    wthread_static_mutex_unlock (&gImpl.myLock);
    return status;
}

mama_status
mama_loadEntitlementBridgeInternal(const char* name)
{
    mama_status         status                  = MAMA_STATUS_NOT_FOUND;
    mamaEntitlementLib* entitlementLib          = NULL;
    LIB_HANDLE          entitlementLibHandle    = NULL;
    char                initFuncName[256];
    char                entImplName[256];
    void*               vp                      = NULL;
    entitlementBridge_init initFunc             = NULL;
    mamaEntitlementBridge  entBridge            = 0;
    int                    insertCheck          = 0;

    if (!name)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    status = mamaInternal_init ();

    if (MAMA_STATUS_OK != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mama_loadEntitlementBridgeInternal (): "
                  "Error initialising internal MAMA state. Cannot load %s entitlements library"
                  "[%s]", name, mamaStatus_stringForStatus(status));
        return status;
    }

    /* Lock here as we don't want anything else being added to the table
     * until either we've returned the appropriate bridge, or loaded and added
     * the new one.
     */
    wthread_static_mutex_lock (&gImpl.myLock);

    entitlementLib = (mamaEntitlementLib*) wtable_lookup (gImpl.entitlements.table,
                                                         name);

    if (entitlementLib && entitlementLib->bridge)
    {
        status = MAMA_STATUS_OK;
        mama_log (MAMA_LOG_LEVEL_FINE,
                  "mama_loadEntitlementBridgeInternal (): "
                  "Entitlement bridge [%s] already loaded. Returning previously loaded bridge.",
                  name);

        /* Return the existing payload bridge implementation */
        entBridge = entitlementLib->bridge;
        goto error_handling_unlock;
    }

    /* Once we have checked if the bridge has already been loaded, check if
     * we've already loaded the maximum number of bridges allowed,
     * MAMA_MAX_ENTITLEMENTS. If we have, report an error and return.
     */
    if (gImpl.entitlements.count >= MAMA_MAX_ENTITLEMENTS)
    {
        status = MAMA_STATUS_NO_BRIDGE_IMPL;
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mama_loadEntitlementBridgeInternal (): "
                  "Maximum number of available bridges has been loaded. Cannot load [%s].",
                  name);
        goto error_handling_unlock;
    }

    snprintf (entImplName, 256, MAMA_ENTITLEMENT_LIB_FILEPATTERN, name);

    entitlementLibHandle = openSharedLib (entImplName, NULL);

    if (!entitlementLibHandle)
    {
        status = MAMA_STATUS_NO_BRIDGE_IMPL;
        mama_log (MAMA_LOG_LEVEL_ERROR,
                "mama_loadPayloadBridge(): "
                "Could not open entitlement bridge library [%s] [%s]",
                 name,
                 getLibError());
        goto error_handling_unlock;
    }

    snprintf (initFuncName, 256, "%sEntitlementBridge_init",  name);

    /* Begin by searching for the *_init function */
    vp          = loadLibFunc (entitlementLibHandle, initFuncName);
    initFunc    = *(entitlementBridge_init*) &vp;

    if (!initFunc)
    {
        status = MAMA_STATUS_NOT_IMPLEMENTED;
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mama_loadEntitlementBridgeInternal (): "
                  "Could not find function [%s] in library [%s]",
                  initFuncName,
                  name);
        goto error_handling_close_and_unlock;
    }

    entBridge   = (mamaEntitlementBridge) calloc (1, sizeof(mamaEntitlementBridge_));

    if (NULL == entBridge)
    {
        status = MAMA_STATUS_NOMEM;
        mama_log(MAMA_LOG_LEVEL_ERROR, 
                "mama_loadEntitlementBridgeInternal (): "
                 "Could not allocate memory for %s entitlement bridge.",
                 name);
        goto error_handling_bridge_allocated;
    }


    status = initFunc(&entBridge->mImpl);
    if (MAMA_STATUS_OK != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mama_loadEntitlementBridgeInternal (): "
                  "Failed to initialise entitlement bridge [%s]. Cannot load.",
                  name);
        goto error_handling_bridge_allocated;
    }

    status = mamaInternal_registerEntitlementFunctions (entitlementLibHandle,
                                                        &entBridge,
                                                        name);

    if (MAMA_STATUS_OK != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mama_loadEntitlementBridgeInternal (): "
                  "Failed to register functions for [%s] entitlement bridge.",
                  name);
        goto error_handling_close_and_unlock;
    }

    entitlementLib = calloc(1, sizeof(mamaEntitlementLib));
    if (NULL == entitlementLib)
    {
        status = MAMA_STATUS_NOMEM;
        mama_log(MAMA_LOG_LEVEL_ERROR, 
                 "mama_loadEntitlementBridgeInternal (): "
                 "Could not allocate entitlementLib %s.",
                 name);
        goto error_handling_unlock;
    }

    entitlementLib->bridge        = entBridge; 
    entitlementLib->library       = entitlementLibHandle;

    insertCheck = wtable_insert (gImpl.entitlements.table,
                                 name,
                                 (void*)entitlementLib);
    if (WTABLE_INSERT_SUCCESS != insertCheck)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mama_loadBridge (): "
                  "Could not insert %s into entitlements table [%d].",
                  name,
                  insertCheck);
    }
    else
    {

        gImpl.entitlements.count++;

        gImpl.entitlements.byIndex[gImpl.entitlements.count - 1] = entitlementLib;

        mama_log (MAMA_LOG_LEVEL_FINE,
                  "mama_loadEntitlementBridgeInternal (): "
                  "Successfully loaded %s entitlement bridge from library [%s]",
                  name,
                  entImplName);
    }

    wthread_static_mutex_unlock (&gImpl.myLock);
    return status;

error_handling_bridge_allocated:
    free(entBridge);
error_handling_close_and_unlock:
    closeSharedLib (entitlementLibHandle);
error_handling_unlock:
    wthread_static_mutex_unlock (&gImpl.myLock);
    return status;

}


int
mamaInternal_generateLbmStats ()
{
    return gGenerateLbmStats;
}

mama_status
mama_loadBridge (mamaBridge* impl,
                 const char* middlewareName)
{
    /* Otherwise this is a dynamic build, load the bridge normally. */
    return mama_loadBridgeWithPath (impl, middlewareName, NULL);
}

mama_status
mama_loadBridgeWithPathInternal (mamaBridge* impl,
                                 const char* middlewareName,
                                 const char* path)
{
    mama_status         status              = MAMA_STATUS_OK;
    int                 i                   = 0;
    mamaMiddlewareLib*  middlewareLib       = NULL;
    LIB_HANDLE          middlewareLibHandle = NULL;

    char                middlewareImplName  [256];
    char                initFuncName        [256];
    versionInfo         bridgeMamaVersion;
    bridge_init         initFunc        = NULL;
    void*               vp              = NULL;

    if (!impl || !middlewareName)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Ensure we have initialised the appropriate structures before we attempt
     * to load and new bridge.
     */
    status = mamaInternal_init ();

    if (MAMA_STATUS_OK != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mama_loadBridgeWithPathInternal (): "
                  "Error initialising internal MAMA state. Cannot load bridge "
                  "[%s]", middlewareName);
        return status;
    }

    /* Lock here as we don't want anything else being added to the table
     * until either we've returned the appropriate bridge, or loaded and added
     * the new one.
     */
    wthread_static_mutex_lock (&gImpl.myLock);

    /* Check if a bridge has already been initialized for the middleware */
    middlewareLib = (mamaMiddlewareLib*)wtable_lookup (
                                     gImpl.middlewares.table,
                                     middlewareName);

    if (middlewareLib && middlewareLib->bridge)
    {
        status = MAMA_STATUS_OK;
        mama_log (MAMA_LOG_LEVEL_NORMAL,
                  "mama_loadBridgeWithPathInternal (): "
                  "Middleware bridge [%s] already loaded. Returning previously loaded bridge.",
                  middlewareName);

        /* Return the existing middleware implementation. */
        *impl = middlewareLib->bridge;

        goto error_handling_unlock;
    }

    /* Once we have checked if the bridge has already been loaded, we want to
     * check if we've already loaded the maximum number of bridges allowed,
     * MAMA_MAX_MIDDLEWARES. If we have, report an error and return
     */
    if (gImpl.middlewares.count >= MAMA_MAX_MIDDLEWARES)
    {
        status = MAMA_STATUS_NO_BRIDGE_IMPL;
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mama_loadBridgeWithPathInternal (): "
                  "Maximum number of available middleware bridges has been loaded. Cannot load [%s].",
                  middlewareName);

        goto error_handling_unlock;
    }

    snprintf (middlewareImplName, 256, "mama%simpl", middlewareName);

    middlewareLibHandle = openSharedLib (middlewareImplName, path);

    if (!middlewareLibHandle)
    {
        status = MAMA_STATUS_NO_BRIDGE_IMPL;
        if (path)
        {
                mama_log (MAMA_LOG_LEVEL_ERROR,
                "mama_loadmamaPayload(): "
                "Could not open middleware bridge library [%s] [%s] [%s]",
                path,
                middlewareImplName,
                getLibError());
        }
        else
        {
                mama_log (MAMA_LOG_LEVEL_ERROR,
                "mama_loadmamaPayload(): "
                "Could not open middleware bridge library [%s] [%s]",
                middlewareImplName,
                getLibError());
        }
        goto error_handling_unlock;
    }

    /* Begin by searching for the *Bridge_init function */
    snprintf (initFuncName, 256, "%sBridge_init", middlewareName);

    vp = loadLibFunc (middlewareLibHandle, initFuncName);
    initFunc = *(bridge_init*)&vp;

    if (NULL == initFunc)
    {
        status = MAMA_STATUS_INVALID_ARG;
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mama_loadBridge (): "
                  "Failed to initialise middleware bridge [%s]. "
                  "Cannot find function %s in implementation library.",
                  middlewareName,
                  initFuncName);
        goto error_handling_impl;
    }

    /* Construct the bridge in MAMA */
    mama_log (MAMA_LOG_LEVEL_FINE,
              "mama_loadBridge (): "
              "Found bridge_init function, loading bridge [%s] with function search.",
              middlewareName);

    /* Allocate the bridge structure */
    *impl = calloc (1, sizeof (mamaBridgeImpl));

    if (NULL == *impl)
    {
        status = MAMA_STATUS_NOMEM;
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mama_loadBridge (): "
                  "Failed to allocate memory for middleware bridge [%s]. Cannot load bridge.",
                  middlewareName);
        goto error_handling_impl;
    }

    /* Once the middleware has been successfully allocate and initialised,
     * we can use the function search to register various middleware functions.
     */
    status = mamaInternal_registerMiddlewareFunctions (middlewareLibHandle,
                                                       impl,
                                                       middlewareName);

    if (MAMA_STATUS_OK != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mama_loadBridge(): "
                  "Failed to register middleware functions for [%s] from [%s] library:[%s]",
                  middlewareName,
                  middlewareImplName,
                  mamaStatus_stringForStatus (status));

        goto error_handling_impl_allocated;
    }

    status = initFunc (*impl);

    if (MAMA_STATUS_OK != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mama_loadBridge (): "
                  "Failed to initialise middleware bridge [%s]. Cannot load bridge.",
                  middlewareName);
        goto error_handling_impl_allocated;
    }

    /* Create the bridge lock */
    (*impl)->mLock = wlock_create();

    /* Populate bridge meta data based on bridge's init properties */
    mamaBridgeImpl_populateBridgeMetaData (*impl);

    /* Verify interface compatibility */
    if(!strToVersionInfo(
            mamaBridgeImpl_getMetaProperty (*impl, MAMA_PROP_BARE_COMPILE_TIME_VER),
            &bridgeMamaVersion))
    {
        status = MAMA_STATUS_INVALID_ARG;
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mama_loadBridge (): "
                  "Failed to initialise middleware bridge [%s]. "
                  "Cannot determine bridge's MAMA version.",
                  middlewareName);
        goto error_handling_impl_allocated;
    }

    /* Fail to load if bridge and API versions are incompatible */
    if (! mama_areVersionsCompatibleInternal (gImpl.version, bridgeMamaVersion))
    {
        status = MAMA_STATUS_INVALID_ARG;
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mama_loadBridge (): "
                  "Failed to initialise middleware bridge [%s]. "
                  "MAMA Runtime Version v%d.%d.%d not compatible with bridge's compile version v%d.%d.%d",
                  middlewareName,
                  gImpl.version.mMajor,
                  gImpl.version.mMinor,
                  gImpl.version.mRelease,
                  bridgeMamaVersion.mMajor,
                  bridgeMamaVersion.mMinor,
                  bridgeMamaVersion.mRelease);
        goto error_handling_impl_allocated;
    }

    mama_log (MAMA_LOG_LEVEL_FINE,
              "mama_loadBridge(): "
              "Loaded middleware bridge [%s] compiled against MAMA v%d.%d.%d %s",
              middlewareName,
              bridgeMamaVersion.mMajor,
              bridgeMamaVersion.mMinor,
              bridgeMamaVersion.mRelease,
              bridgeMamaVersion.mExtra ? bridgeMamaVersion.mExtra : "");

    mama_log (MAMA_LOG_LEVEL_NORMAL,
             "mama_loadBridge(): "
             "Sucessfully loaded %s middleware from library [%s]",
             middlewareName, middlewareImplName);

    status = ((mamaBridgeImpl*)(*impl))->bridgeOpen (*impl);

    /* Middleware bridge failed to open successfully. Log the issue, close
     * the shared library, and ensure the impl struct points to NULL.
     */
    if (MAMA_STATUS_OK != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mama_loadBridge(): "
                  "Error when attempting to open middleware [%s] - [%s]",
                  middlewareName,
                  mamaStatus_stringForStatus (status));

        /* Since open has failed, we can't safely call close. It is possible the
         * underlying bridge has tidied up after itself, but if not we'll try
         * and free the impl.
         */
        if (*impl)
        {
            free (*impl);
        }

        goto error_handling_impl;
    }

    /* Allocate the bridgeLib struct, and populate with the bridgeImpl and
     * library handle.
     */
    middlewareLib = malloc (sizeof (mamaMiddlewareLib));

    if (!middlewareLib)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mama_loadBridgeWithPathInternal (): "
                  "Failed to allocate memory for [%s] bridge library.",
                  middlewareName);

        /* Since we can't finish the load, we must cleanup after ourselves... */
        status = (*impl)->bridgeClose (*impl);
        if (MAMA_STATUS_OK != status)
        {
            mama_log (MAMA_LOG_LEVEL_ERROR,
                      "mama_loadBridgeWithPathInternal (): "
                      "Failed to close middleware bridge [%s] during cleanup.",
                      middlewareName);
        }

        status = MAMA_STATUS_NOMEM;
        goto error_handling_impl;
    }

    middlewareLib->bridge        = *(mamaBridge*)impl;
    middlewareLib->library       = middlewareLibHandle;

    status = wtable_insert (gImpl.middlewares.table,
                            middlewareName,
                            (void*)middlewareLib);

    /* wtable_insert may return 1 for success, 0 for inserting a duplicate key,
     * -1 for failure of allocation. Unfortunately it also returns 0 for no table,
     * which means we have an abiguity. However, we shouldn't be able to reach
     * this point with a duplicate, so we should generally happy to proceed.
     */
    if (WTABLE_INSERT_SUCCESS != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mama_loadBridgeWithPathInternal (): "
                  "Failed to register middleware [%s] with MAMA.",
                  middlewareName);

        /* Since we can't finish the load, we must cleanup after ourselves... */
        status = (*impl)->bridgeClose (*impl);
        if (MAMA_STATUS_OK != status)
        {
            mama_log (MAMA_LOG_LEVEL_ERROR,
                      "mama_loadBridgeWithPathInternal (): "
                      "Failed to close middleware bridge [%s] during cleanup.",
                      middlewareName);
        }

        goto error_handling_middlewarelib;
    }

    /* Increment the count of loaded middlewares */
    /* NOTE: The check at the start of the function ensures that we can never
     * go beyond MAMA_MAX_MIDDLEWARES, so a check is redundant here.
     */
    gImpl.middlewares.count++;

    /* Add a pointer to the middlewareLib at the middlewareCount'th space.
     * Note: We can get away with this now, because there's no mechanism for out
     * of order unloading of middlewares.
     */
    gImpl.middlewares.byIndex[gImpl.middlewares.count - 1] = middlewareLib;

    mama_log (MAMA_LOG_LEVEL_FINE,
              "mama_loadBridge (): "
              "Successfully loaded %s middleware bridge from library [%s]",
              middlewareName,
              middlewareImplName);

    wthread_static_mutex_unlock (&gImpl.myLock);

    status = MAMA_STATUS_OK;
    return status;

error_handling_middlewarelib:
    free (middlewareLib);
error_handling_impl_allocated:
    free (*impl);
error_handling_impl:
    *impl = NULL;
    closeSharedLib (middlewareLibHandle);
error_handling_unlock:
    wthread_static_mutex_unlock (&gImpl.myLock);
    return status;
}

mama_status
mama_loadBridgeWithPath (mamaBridge* impl,
                         const char* middlewareName,
                         const char* path)
{
    return mama_loadBridgeWithPathInternal(impl, middlewareName, path);
}

MAMAExpDLL
void
mama_setWrapperGetVersion(fpWrapperGetVersion value)
{
    wrapperGetVersion = value;
}

/**
* Exposes the property for whether or not the test classes need to be used for catching callback exceptions
*/
int
mamaInternal_getCatchCallbackExceptions (void)
{
    return gCatchCallbackExceptions;
}


/* Do not expose in the public headers */
const char*
mama_wrapperGetVersion(mamaBridge bridge)
{
  if (wrapperGetVersion)
      /* use getVersion from wrapper */
      return wrapperGetVersion();
  else
      /* Use native getVersion */
      return mama_getVersion(bridge);
}

void
mama_setLastError (mamaError error)
{
     wthread_setspecific(last_err_key, (void*)error);
}

mamaError
mama_getLastErrorCode (void)
{
    return (mamaError)wthread_getspecific(last_err_key);
}

const char*
mama_getLastErrorText (void)
{
    return mamaError_convertToString((mamaError)wthread_getspecific(last_err_key));
}


mama_status
mama_setBridgeInfoCallback (mamaBridge bridgeImpl, bridgeInfoCallback callback)
{
    mamaBridgeImpl* impl = (mamaBridgeImpl*)bridgeImpl;

    switch (mamaMiddleware_convertFromString (impl->bridgeGetName()))
    {
        case MAMA_MIDDLEWARE_LBM    :   impl->mBridgeInfoCallback = callback;
                                        return MAMA_STATUS_OK;
        case MAMA_MIDDLEWARE_TIBRV  :
        case MAMA_MIDDLEWARE_WMW    :
        default                     :   return MAMA_STATUS_NOT_IMPLEMENTED;
    }

    return MAMA_STATUS_OK;
}

mama_status
mama_addStatsCollector (mamaStatsCollector statsCollector)
{
    mama_status status = MAMA_STATUS_NOT_FOUND;

    if (!gStatsGenerator)
        mama_statsInit();

    if (MAMA_STATUS_OK != (
            status = mamaStatsGenerator_addStatsCollector (
                        gStatsGenerator,
                        statsCollector)))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mama_addStatsCollector (): "
                    "Could not add User stats collector.");
        return status;
    }

    return status;
}

mama_status
mama_removeStatsCollector (mamaStatsCollector statsCollector)
{
    mama_status status = MAMA_STATUS_NOT_FOUND;

    if (gStatsGenerator)
    {
        if (MAMA_STATUS_OK != (
                status = mamaStatsGenerator_removeStatsCollector (
                            mamaInternal_getStatsGenerator(),
                            statsCollector)))
        {
            mama_log (MAMA_LOG_LEVEL_ERROR, "mama_removeStatsCollector (): "
                     "Could not remove User stats collector.");
            return status;
        }
    }
    else
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaInternal_getStatsGenerator (): "
                  "Could not find stats generator.");
    }
    return status;
}

mama_status
mamaInternal_initialiseTable (wtable_t* table,
                              const char* name,
                              mama_size_t size)
{
    mama_status status = MAMA_STATUS_OK;
    if (NULL == table)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    if (!(*table))
    {
       *table = wtable_create (name, (unsigned long) size);

       if (!(*table))
       {
            mama_log (MAMA_LOG_LEVEL_ERROR,
                      "mamaInternal_init (): "
                      "Error initialising the %s table.",
                      name);
            status = MAMA_STATUS_PLATFORM;
            return status;
        }
    }
    return status;
}

mama_status
mamaInternal_init (void)
{
    mama_status status             = MAMA_STATUS_OK;
    const char* mamaRuntimeVersion = mama_version + 5;

    enum mamaInternalTableSize {
        MIDDLEWARE_TABLE_SIZE  = 5,
        PAYLOAD_TABLE_SIZE     = 15,
        ENTITLEMENT_TABLE_SIZE = 5,
    };

    /* Lock the gImpl to ensure we don't clobber something else trying to
     * access at the same time. 
     */
    wthread_static_mutex_lock (&gImpl.myLock);

    /* Create meta properties and populate MAMA's own meta properties.
     * Note that mama_version is always prefixed by 'mama ', hence the
     * '5' character offset. Other meta properties may also be set here. */
    if (NULL == gImpl.internalProperties)
    {
        gImpl.internalProperties = properties_Create ();
        status = mamaInternal_setMetaProperty(MAMA_PROP_MAMA_RUNTIME_VER,
                mamaRuntimeVersion);
        if (MAMA_STATUS_OK != status)
        {
            wthread_static_mutex_unlock (&gImpl.myLock);
            return status;
        }
    }


    /* Check if we've initialised before - 1 means yes, 0 means no. */
    if (0 == wInterlocked_read (&gImpl.init))
    {
        /* Initialize the middlewares wtable */
        status = mamaInternal_initialiseTable (&gImpl.middlewares.table,
                                               "Middlewares",
                                               MIDDLEWARE_TABLE_SIZE);

        if (MAMA_STATUS_OK != status) 
        {
            wthread_static_mutex_unlock (&gImpl.myLock);
            return status;
        }

        /* Initialize the payloads wtable */
        status = mamaInternal_initialiseTable (&gImpl.payloads.table,
                                               "Payloads",
                                               PAYLOAD_TABLE_SIZE);

        if (MAMA_STATUS_OK != status)
        {
            wthread_static_mutex_unlock (&gImpl.myLock);
            return status;
        }

        /* Initialize the entitlements wtable */
        status = mamaInternal_initialiseTable (&gImpl.entitlements.table,
                                               "Entitlements",
                                               ENTITLEMENT_TABLE_SIZE);

        if (MAMA_STATUS_OK != status)
        {
            wthread_static_mutex_unlock (&gImpl.myLock);
            return status;
        }

        /* Update MAMA version information structure once here */
        strToVersionInfo(mamaRuntimeVersion, &gImpl.version);

        /* Increment the init variable to indicate that initialization has been
         * successfully performed
         */
        wInterlocked_increment (&gImpl.init);
    }

    wthread_static_mutex_unlock (&gImpl.myLock);
    return status;
}

mama_status
mamaInternal_getPayloadId (const char* payloadName,
                           mamaPayloadBridge payload,
                           char* payloadChar)
{
    mama_status status = MAMA_STATUS_OK;
    char propNameBuf[256];
    const char* propertyReturn = NULL;

    if (NULL == payloadName || NULL == payloadChar || NULL == payload)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Build properties string, and query the global properties object for
     * matching characters.
     */
    snprintf (propNameBuf, 256, "mama.payload.%s.payloadId", payloadName);
    propertyReturn = mama_getProperty (propNameBuf);

    /* Found a property. */
    if (NULL != propertyReturn)
    {
        mama_log (MAMA_LOG_LEVEL_FINE,
                  "mamaInternal_getPayloadId (): "
                  "Found payload ID char for [%s] in properties - [%c]",
                  payloadName,
                  propertyReturn[0]);

        *payloadChar = propertyReturn[0];
    }
    else
    {
        /* Check the payload bridge itself for it's self identifier. */
        *payloadChar = (char)payload->msgPayloadGetType ();
    }

    return status;
}

mama_i32_t
mamaInternal_getEntitlementBridgeCount ()
{
    return gImpl.entitlements.count;
}

mama_status
mamaInternal_getEntitlementBridgeByName(mamaEntitlementBridge* entBridge, const char* name)
{
    mamaEntitlementLib* entitlementLib;
    entitlementLib = (mamaEntitlementLib*) wtable_lookup (gImpl.entitlements.table,
                                                         name);

    if (entitlementLib && entitlementLib->bridge)
    {
        /* Return the existing entitlement bridge implementation */
        *entBridge = entitlementLib->bridge;
        return MAMA_STATUS_OK;
    }
    else
    {
        mama_log (MAMA_LOG_LEVEL_SEVERE,
                  "mamaInternal_getEntitlementBridgeByName (): "
                  "Could not find loaded entitlement library [%s].",
                  name);
        return MAMA_STATUS_NOT_FOUND;
    }
}

const char*
mamaInternal_getMetaProperty (const char* name)
{
    return properties_Get (gImpl.internalProperties, name);
}

mama_status
mamaInternal_setMetaProperty (const char* name, const char* value)
{
    const char* existingValue = NULL;
    /* If this internal property has been set, don't let it be overwritten */
    if (NULL != (existingValue = mamaInternal_getMetaProperty (name)))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mamaInternal_setMetaProperty (): "
                  "Can't set meta property %s=%s - it is already set to [%s].",
                  name, value, existingValue);
        return MAMA_STATUS_INVALID_ARG;
    }
    if (properties_setProperty(gImpl.internalProperties, name, value))
    {
        mama_log (MAMA_LOG_LEVEL_FINER,
                  "mamaInternal_setMetaProperty (): "
                  "Successfully set meta property %s=%s.", name, value);
        return MAMA_STATUS_OK;
    }
    else
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mamaInternal_setMetaProperty (): "
                  "Can't set meta property %p %s=%s - out of memory?",
                  gImpl.internalProperties, name, value, existingValue);
        return MAMA_STATUS_NOMEM;
    }
}

/* NB copy rather than reference - this is intentional because we are going to
 * be modifying these values to normalized versions */
mama_bool_t mama_areVersionsCompatibleInternal (versionInfo mamaVer, versionInfo bridgeVer)
{
    /* Normalize bridge versions with standard major / minor versions */
    mama_normalizeMamaBridgeInterfaceVersionInternal (&mamaVer);
    mama_normalizeMamaBridgeInterfaceVersionInternal (&bridgeVer);

    /* If major and minor versions match, they are compatible */
    if (mamaVer.mMajor == bridgeVer.mMajor && mamaVer.mMinor == bridgeVer.mMinor)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/* This function is destructive and will modify the versionInfo provided to
 * return a bridge compatibility version (i.e. 6.0, 6.1 etc)
   +-----------------+------------------------------+
   | (O)MAMA Version | Normalized Interface Version |
   +-----------------+------------------------------+
   | 2.2.x           | 6.0.0                        |
   | 2.3.x           | 6.0.0                        |
   | 6.0.x (x <  7)  | 6.0.0                        |
   | 2.4.x           | 6.1.0                        |
   | 6.0.x (x >= 7)  | 6.1.0                        |
   | 6.1.x           | 6.1.0                        |
   | 6.2.x (future)  | 6.2.0                        |
   +-----------------+------------------------------+
 */
void mama_normalizeMamaBridgeInterfaceVersionInternal (versionInfo* version)
{
    /* 2.3.x is equivalent to all releases prior to MAMA 6.0.7 */
    if (version->mMajor == 2 && version->mMinor < 4)
    {
        version->mMajor = 6;
        version->mMinor = 0;
    }

    /* 6.0.7+ is equivalent in bridge compatibility to 6.1.x */
    if (version->mMajor == 6 && version->mMinor == 0 && version->mRelease >= 7)
    {
        version->mMajor = 6;
        version->mMinor = 1;
    }

    /* 2.4.x is equivalent to 6.1 */
    if (version->mMajor == 2 && version->mMinor == 4)
    {
        version->mMajor = 6;
        version->mMinor = 1;
    }

    /* This is irrelevant at this point so blank for clarity */
    version->mRelease = 0;
}

int mama_getGenerateTransportStats()
{
    return gGenerateTransportStats;
}
/** 
 * Get Middleware Bridge by middleware name
 *
 * @param bridge Pointer to a mamaBridge object.
 * @param middlewareName String denoting the middleware to return
 * @return mama_status MAMA_STATUS_OK if successful. 
 *                     MAMA_STATUS_NOT_FOUND if no bridge available.
 */
MAMAExpDLL
extern mama_status
mama_getMiddlewareBridge (mamaBridge *bridge, const char *middlewareName)
{
    mama_status        status         = MAMA_STATUS_OK;
    mamaMiddlewareLib *middlewareLib  = NULL;


    if (NULL == bridge || NULL == middlewareName)
    {
        status = MAMA_STATUS_NULL_ARG;
        return status;
    }

    /* Locking here so we don't accidentally try and grab a bridge while we're
     * in the middle of loading another.
     */
    wthread_static_mutex_lock (&gImpl.myLock);

    mama_log (MAMA_LOG_LEVEL_FINEST,
             "mama_getMiddlewareBridge (): "
             "Searching for existing bridge [%s]",
             middlewareName);
    
    middlewareLib = (mamaMiddlewareLib*)wtable_lookup (
                                     gImpl.middlewares.table,
                                     middlewareName);
    
    if (middlewareLib && middlewareLib->bridge)
    {
        status = MAMA_STATUS_OK;
        mama_log (MAMA_LOG_LEVEL_FINER,
                  "mama_getMiddlewareBridge (): "
                  "Middleware bridge [%s] found.",
                  middlewareName);

        /* Return the existing middleware implementation. */
        *bridge = middlewareLib->bridge;
    } else {
        status = MAMA_STATUS_NOT_FOUND;
        mama_log (MAMA_LOG_LEVEL_FINER,
                  "mama_getMiddlewareBridge (): "
                  "Middleware bridge [%s] not found.",
                  middlewareName);

        /* Set the point to NULL incase clients use it to check. Or don't */
        *bridge = NULL;
    }

    wthread_static_mutex_unlock (&gImpl.myLock);
    return status;
}

/**
 * Get Payload bridge by payload name
 * 
 * @param payloadBridge Pointer to the mamaPayloadBridge object.
 * @param payloadName String denoting the payload to return
 * @return mama_status MAMA_STATUS_OK if successful
 *                     MAMA_STATUS_NOT_FOUND if no payload available.
 */
MAMAExpDLL
extern mama_status
mama_getPayloadBridge (mamaPayloadBridge *payloadBridge, 
                       const char        *payloadName)
{
    mama_status status         = MAMA_STATUS_OK;
    mamaPayloadLib *payloadLib = NULL;

    if (NULL == payloadBridge || NULL == payloadName)
    {
        status = MAMA_STATUS_NULL_ARG;
        return status;
    }

    /* Lock here to avoid accidentally grabbing a payload mid load. */
    wthread_static_mutex_lock (&gImpl.myLock);

    mama_log (MAMA_LOG_LEVEL_FINEST, 
              "mama_getPayloadBridge ():"
              "Searching for existing payload [%s]",
              payloadName);

    payloadLib = (mamaPayloadLib*)wtable_lookup (gImpl.payloads.table,
                                                 payloadName);

    if (payloadLib && payloadLib->bridge)
    {
        status = MAMA_STATUS_OK;
        mama_log (MAMA_LOG_LEVEL_FINER,
                  "mama_getPayloadBridge (): "
                  "Payload bridge [%s] found.",
                  payloadName);

        /* Return the existing payload bridge implementation */
        *payloadBridge = payloadLib->bridge;
    } else {
        status = MAMA_STATUS_NOT_FOUND;
        mama_log (MAMA_LOG_LEVEL_FINER,
                  "mama_getPayloadBridge ():"
                  "Payload bridge [%s] not found.",
                  payloadName);

        /* Set the point to NULL incase clients use it to check. Or don't */
        *payloadBridge = NULL;
    }

    wthread_static_mutex_unlock (&gImpl.myLock);
    return status;
}

/**
 * Automatically load any payload bridges specified in the configuration
 * files using the "mama.payload.autoload.*" property.
 *
 * @return mama_status Always returns MAMA_STATUS_OK. Logs indicate failure. 
 */
mama_status mamaInternal_autoloadPayloadBridges (void)
{
    mama_status status = MAMA_STATUS_OK;

    mama_log (MAMA_LOG_LEVEL_FINE,
                "mamaInternal_autoloadPayloadBridges (): "
                "Attempting to autoload payload bridges.");

    /* Ensure that we have loaded the properties file before we start
     * loading values. 
     */
    if (!gProperties) {
        mamaInternal_loadProperties (NULL, NULL);
    }

    /* Gather global named thread properties. */
    properties_ForEach (
        mamaInternal_getProperties ( ), autoloadPayloadPropertiesCb, NULL);

    return status;
}

static void
autoloadPayloadPropertiesCb (const char* name, const char* value, void* closure)
{
    mama_status status = MAMA_STATUS_OK;

    if (strncmp (name,
                 MAMA_PROPERTY_PAYLOAD_AUTOLOAD,
                 strlen (MAMA_PROPERTY_PAYLOAD_AUTOLOAD)) == 0) {
        const char *payloadBridgeName =
            name + strlen (MAMA_PROPERTY_PAYLOAD_AUTOLOAD);
        int payloadAutoload = properties_GetPropertyValueAsBoolean (value);

        /* We don't use this here, simply allocate on the stack to complete the
         * loading process.
         */
        mamaPayloadBridge loadedBridge = NULL;

        if ((0 != strcmp (payloadBridgeName, "")) && payloadAutoload) {
            mama_log (MAMA_LOG_LEVEL_FINE,
                      "autoloadPayloadPropertiesCb (): "
                      "Attempting to autoload payload bridge %s (autoload=%s).",
                      payloadBridgeName,
                      value);
            status = mama_loadPayloadBridge (&loadedBridge, payloadBridgeName);

            if (MAMA_STATUS_OK != status) {
                mama_log (MAMA_LOG_LEVEL_FINE,
                          "autoloadPayloadPropertiesCb (): "
                          "Failed to autoload payload bridge %s (autoload=%s).",
                          payloadBridgeName,
                          value);
            } else {
                mama_log (
                    MAMA_LOG_LEVEL_FINE,
                    "autoloadPayloadPropertiesCb (): "
                    "Successful automatic loading of the payload bridge %s "
                    "(autoload=%s).",
                    payloadBridgeName,
                    value);
            }
        }
    }
}
