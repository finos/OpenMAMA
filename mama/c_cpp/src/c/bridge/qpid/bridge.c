/* $Id$
 *
 * OpenMAMA: The open middleware agnostic messaging API
 * Copyright (C) 2011 NYSE Inc.
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


/*=========================================================================
  =                             Includes                                  =
  =========================================================================*/

#include <mama/mama.h>
#include <mama/version.h>
#include <timers.h>
#include <wombat/strutils.h>
#include "io.h"
#include "qpidbridgefunctions.h"


/*=========================================================================
  =                Typedefs, structs, enums and globals                   =
  =========================================================================*/

/* Global timer heap */
timerHeap           gQpidTimerHeap;

/* Default payload names and IDs to be loaded when this bridge is loaded */
static char*        PAYLOAD_NAMES[]         =   { "qpidmsg", NULL };
static char         PAYLOAD_IDS[]           =   { MAMA_PAYLOAD_QPID, '\0' };


/*=========================================================================
  =                              Macros                                   =
  =========================================================================*/

/* Version identifiers */
#define             QPID_BRIDGE_NAME            "qpid"
#define             QPID_BRIDGE_VERSION         "1.0"

/* Name to be given to the default queue. Should be bridge-specific. */
#define             QPID_DEFAULT_QUEUE_NAME     "QPID_DEFAULT_MAMA_QUEUE"

/* Timeout for dispatching queues on shutdown in milliseconds */
#define             QPID_SHUTDOWN_TIMEOUT       5000


/*=========================================================================
  =               Public interface implementation functions               =
  =========================================================================*/

mama_status qpidBridge_init (mamaBridge bridgeImpl)
{
    mama_status status         = MAMA_STATUS_OK;
    const char* runtimeVersion = NULL;

    /* Reusable buffer to populate with property values */
    char propString[MAX_INTERNAL_PROP_LEN];
    versionInfo rtVer;

    /* Will set the bridge's compile time MAMA version */
    MAMA_SET_BRIDGE_COMPILE_TIME_VERSION(QPID_BRIDGE_NAME);

    /* Ensure that the qpid bridge is defined as not deferring entitlements */
    status = mamaBridgeImpl_setReadOnlyProperty (bridgeImpl,
                                                 MAMA_PROP_BARE_ENT_DEFERRED,
                                                 "false");

    /* Get the runtime version of MAMA and parse into version struct */
    runtimeVersion = mamaInternal_getMetaProperty (MAMA_PROP_MAMA_RUNTIME_VER);
    strToVersionInfo (runtimeVersion, &rtVer);

    /* NB checks are runtime only - assume build system will prevent accidental
     * compilation against incompatible versions. This is a demonstration t
     * show how you could do runtime version checking. */
    if (1 == rtVer.mMajor)
    {
        mama_log (MAMA_LOG_LEVEL_SEVERE, "qpidBridge_init(): "
                  "This version of the bridge (%s) cannot be used with MAMA %s.",
                  QPID_BRIDGE_VERSION,
                  runtimeVersion);
        return MAMA_STATUS_NOT_IMPLEMENTED;
    }

    return status;
}

mama_status
qpidBridge_open (mamaBridge bridgeImpl)
{
    mama_status         status  = MAMA_STATUS_OK;
    mamaBridgeImpl*     bridge  = (mamaBridgeImpl*) bridgeImpl;

    wsocketstartup();

    if (NULL == bridgeImpl)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Create the default event queue */
    status = mamaQueue_create (&bridge->mDefaultEventQueue, bridgeImpl);
    if (MAMA_STATUS_OK != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "qpidBridge_open(): Failed to create QPID queue (%s).",
                  mamaStatus_stringForStatus (status));
        return status;
    }

    /* Set the queue name (used to identify this queue in MAMA stats) */
    mamaQueue_setQueueName (bridge->mDefaultEventQueue,
                            QPID_DEFAULT_QUEUE_NAME);

    /* Create the timer heap */
    if (0 != createTimerHeap (&gQpidTimerHeap))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "qpidBridge_open(): Failed to initialize timers.");
        return MAMA_STATUS_PLATFORM;
    }

    /* Start the dispatch timer heap which will create a new thread */
    if (0 != startDispatchTimerHeap (gQpidTimerHeap))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "qpidBridge_open(): Failed to start timer thread.");
        return MAMA_STATUS_PLATFORM;
    }

    /* Start the io thread */
    qpidBridgeMamaIoImpl_start ();

    return MAMA_STATUS_OK;
}

mama_status
qpidBridge_close (mamaBridge bridgeImpl)
{
    mama_status      status      = MAMA_STATUS_OK;
    mamaBridgeImpl*  bridge      = (mamaBridgeImpl*) bridgeImpl;
    wthread_t        timerThread;


    /* Remove the timer heap */
    if (NULL != gQpidTimerHeap)
    {
        /* The timer heap allows us to access it's thread ID for joining */
        timerThread = timerHeapGetTid (gQpidTimerHeap);
        if (0 != destroyHeap (gQpidTimerHeap))
        {
            mama_log (MAMA_LOG_LEVEL_ERROR,
                      "qpidBridge_close(): Failed to destroy QPID timer heap.");
            status = MAMA_STATUS_PLATFORM;
        }
        /* The timer thread expects us to be responsible for terminating it */
        wthread_join    (timerThread, NULL);
    }
    gQpidTimerHeap = NULL;

    /* Destroy once queue has been emptied */
    mamaQueue_destroyTimedWait (bridge->mDefaultEventQueue,
                                QPID_SHUTDOWN_TIMEOUT);

    /* Stop and destroy the io thread */
    qpidBridgeMamaIoImpl_stop ();

    return status;
}

mama_status
qpidBridge_start (mamaQueue defaultEventQueue)
{
    if (NULL == defaultEventQueue)
    {
      mama_log (MAMA_LOG_LEVEL_FINER,
                "qpidBridge_start(): defaultEventQueue is NULL");
      return MAMA_STATUS_NULL_ARG;
    }

    /* Start the default event queue */
    return mamaQueue_dispatch (defaultEventQueue);;
}

mama_status
qpidBridge_stop (mamaQueue defaultEventQueue)
{
    if (NULL == defaultEventQueue)
    {
      mama_log (MAMA_LOG_LEVEL_FINER,
                "qpidBridge_stop(): defaultEventQueue is NULL");
      return MAMA_STATUS_NULL_ARG;
    }

    return mamaQueue_stopDispatch (defaultEventQueue);;
}

const char*
qpidBridge_getVersion (void)
{
    return QPID_BRIDGE_VERSION;
}

const char*
qpidBridge_getName (void)
{
    return QPID_BRIDGE_NAME;
}

mama_status
qpidBridge_getDefaultPayloadId (char ***name, char **id)
{
    if (NULL == name || NULL == id)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    /*
     * Populate name with the value of all supported payload names, the first
     * being the default
     */
    *name   = PAYLOAD_NAMES;

    /*
     * Populate id with the char keys for all supported payload names, the first
     * being the default
     */
    *id     = PAYLOAD_IDS;

     return MAMA_STATUS_OK;
}
