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

#include <avis/elvin.h>

#include "wombat/port.h"

#include <mama/mama.h>
#include <timers.h>
#include <queueimpl.h>
#include "avisbridgefunctions.h"
#include "avisdefs.h"
#include "transportbridge.h"
#include "io.h"

timerHeap gAvisTimerHeap;

mama_status avisBridge_init (void)
{
    mama_status status = MAMA_STATUS_OK;
    /* The bridge actually doesn't need to begin any initialisation at this
     * stage, thus can ignore this function.
     */

    return status;
}

const char*
avisBridge_getVersion (void)
{
    return (const char*) "Unable to get version number";
}

const char*
avisBridge_getName (void)
{
    return "avis";
}

static const char* PAYLOAD_NAMES[] = {"avismsg",NULL};
static const char PAYLOAD_IDS[] = {MAMA_PAYLOAD_AVIS,NULL};

mama_status
avisBridge_getDefaultPayloadId (char***name, char** id)
{
    if (!name) return MAMA_STATUS_NULL_ARG;
    if (!id) return MAMA_STATUS_NULL_ARG;
    *name = (char**)PAYLOAD_NAMES;
    *id = (char*)PAYLOAD_IDS;

    return MAMA_STATUS_OK;
}


mama_status
avisBridge_open (mamaBridge bridgeImpl)
{
    mama_status status = MAMA_STATUS_OK;
    mamaBridgeImpl* impl =  (mamaBridgeImpl*)bridgeImpl;

    wsocketstartup();
    
    mama_log (MAMA_LOG_LEVEL_FINEST, "avisBridge_open(): Entering.");

    if (MAMA_STATUS_OK !=
       (status =  mamaQueue_create (&impl->mDefaultEventQueue, bridgeImpl)))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "avisBridge_open():"
            "Failed to create Avis queue.");
        return status;
    }

    mamaQueue_setQueueName (impl->mDefaultEventQueue,
                            "AVIS_DEFAULT_MAMA_QUEUE");

    mama_log (MAMA_LOG_LEVEL_NORMAL,
              "avisBridge_open(): Successfully created Avis queue");

    if (0 != createTimerHeap (&gAvisTimerHeap))
    {
        mama_log (MAMA_LOG_LEVEL_NORMAL,
                "avisBridge_open(): Failed to initialize timers.");
        return MAMA_STATUS_PLATFORM;
    }

    if (0 != startDispatchTimerHeap (gAvisTimerHeap))
    {
        mama_log (MAMA_LOG_LEVEL_NORMAL,
                "avisBridge_open(): Failed to start timer thread.");
        return MAMA_STATUS_PLATFORM;
    }

    /* Start the io thread */
    avisBridgeMamaIoImpl_start ();

    return MAMA_STATUS_OK;
}

mama_status
avisBridge_close (mamaBridge bridgeImpl)
{
    mama_status     status = MAMA_STATUS_OK;
    mamaBridgeImpl* impl   = NULL;
    
    mama_log (MAMA_LOG_LEVEL_FINEST, "avisBridge_close(): Entering.");

    impl =  (mamaBridgeImpl*)bridgeImpl;


    if (0 != destroyHeap (gAvisTimerHeap))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "avisBridge_close():"
                "Failed to destroy Avis timer heap.");
        status = MAMA_STATUS_PLATFORM;
    }

    wlock_destroy (impl->mLock);

    mamaQueue_destroyWait(impl->mDefaultEventQueue);

    free (impl);
    
    wsocketcleanup();

    /* Stop and destroy the io thread */
	avisBridgeMamaIoImpl_stop ();

    return status;
}


mama_status
avisBridge_start(mamaQueue defaultEventQueue)
{
    mama_status     status     = MAMA_STATUS_OK;

    mama_log (MAMA_LOG_LEVEL_FINER, "avisBridge_start(): Start dispatching on default event queue.");

    // start Mama event loop
    return mamaQueue_dispatch(defaultEventQueue);
}

mama_status
avisBridge_stop(mamaQueue defaultEventQueue)
{
    mama_status     status     = MAMA_STATUS_OK;

    mama_log (MAMA_LOG_LEVEL_FINER, "avisBridge_stop(): Stopping bridge.");

    // stop Mama event loop
    status = mamaQueue_stopDispatch (defaultEventQueue);
    if (status != MAMA_STATUS_OK)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "avisBridge_stop(): Failed to unblock  queue.");
        return status;
    }

    return MAMA_STATUS_OK;
}
