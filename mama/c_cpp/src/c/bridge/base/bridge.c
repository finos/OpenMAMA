/*=========================================================================
  =                             Includes                                  =
  =========================================================================*/

#include <mama/mama.h>
#include <mama/version.h>
#include <timers.h>
#include <wombat/strutils.h>
#include "io.h"
#include "basebridgefunctions.h"
#include "basedefs.h"


/*=========================================================================
  =                Typedefs, structs, enums and globals                   =
  =========================================================================*/


/*=========================================================================
  =                              Macros                                   =
  =========================================================================*/

/* Name to be given to the default queue. Should be bridge-specific. */
#define             DEFAULT_QUEUE_NAME          "BASE_DEFAULT_MAMA_QUEUE"

/* Timeout for dispatching queues on shutdown in milliseconds */
#define             QUEUE_SHUTDOWN_TIMEOUT       5000


/*=========================================================================
  =               Public interface implementation functions               =
  =========================================================================*/

/* Must be immplemented in overriding bridge */
mama_status
baseBridge_init (mamaBridge bridgeImpl)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

/*
 * Note that if this method is not overridden, the implementation no longer owns the bridge
 * closure
 */
mama_status
baseBridge_open (mamaBridge bridgeImpl)
{
    mama_status         status  = MAMA_STATUS_OK;
    baseBridgeClosure*  closure = NULL;
    mamaBridgeImpl*     bridge  = (mamaBridgeImpl*) bridgeImpl;

    wsocketstartup();

    if (NULL == bridgeImpl)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Create the bridge impl container */
    closure = (baseBridgeClosure*) calloc(1, sizeof(baseBridgeClosure));
    if (NULL == closure)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "baseBridge_open(): Could not allocate bridge structure.");
        return MAMA_STATUS_NOMEM;
    }
    mamaBridgeImpl_setClosure(bridgeImpl, closure);

    /* Create the default event queue */
    status = mamaQueue_create (&bridge->mDefaultEventQueue, bridgeImpl);
    if (MAMA_STATUS_OK != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "baseBridge_open(): Failed to create QPID queue (%s).",
                  mamaStatus_stringForStatus (status));
        return status;
    }

    /* Set the queue name (used to identify this queue in MAMA stats) */
    mamaQueue_setQueueName (bridge->mDefaultEventQueue,
                            DEFAULT_QUEUE_NAME);

    /* Create the timer heap */
    if (0 != createTimerHeap (&closure->mTimerHeap))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "baseBridge_open(): Failed to initialize timers.");
        return MAMA_STATUS_PLATFORM;
    }

    /* Start the dispatch timer heap which will create a new thread */
    if (0 != startDispatchTimerHeap (closure->mTimerHeap))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "baseBridge_open(): Failed to start timer thread.");
        return MAMA_STATUS_PLATFORM;
    }

    /* Start the io thread */
    baseBridgeMamaIoImpl_start ((void*)closure);

    return MAMA_STATUS_OK;
}

mama_status
baseBridge_close (mamaBridge bridgeImpl)
{
    mama_status         status      = MAMA_STATUS_OK;
    mamaBridgeImpl*     bridge      = (mamaBridgeImpl*) bridgeImpl;
    baseBridgeClosure*  closure     = NULL;
    wthread_t           timerThread;

    mamaBridgeImpl_getClosure(bridgeImpl, (void**)&closure);

    /* Remove the timer heap */
    if (NULL != closure->mTimerHeap)
    {
        /* The timer heap allows us to access it's thread ID for joining */
        timerThread = timerHeapGetTid (closure->mTimerHeap);
        if (0 != destroyHeap (closure->mTimerHeap))
        {
            mama_log (MAMA_LOG_LEVEL_ERROR,
                      "baseBridge_close(): Failed to destroy QPID timer heap.");
            status = MAMA_STATUS_PLATFORM;
        }
        /* The timer thread expects us to be responsible for terminating it */
        wthread_join    (timerThread, NULL);
    }
    mamaBridgeImpl_setClosure(bridgeImpl, NULL);

    /* Destroy once queue has been emptied */
    mamaQueue_destroyTimedWait (bridge->mDefaultEventQueue,
                                QUEUE_SHUTDOWN_TIMEOUT);

    /* Stop and destroy the io thread */
    baseBridgeMamaIoImpl_stop ((void*)closure);

    return status;
}

mama_status
baseBridge_start (mamaQueue defaultEventQueue)
{
    if (NULL == defaultEventQueue)
    {
      mama_log (MAMA_LOG_LEVEL_FINER,
                "baseBridge_start(): defaultEventQueue is NULL");
      return MAMA_STATUS_NULL_ARG;
    }

    /* Start the default event queue */
    return mamaQueue_dispatch (defaultEventQueue);;
}

mama_status
baseBridge_stop (mamaQueue defaultEventQueue)
{
    if (NULL == defaultEventQueue)
    {
      mama_log (MAMA_LOG_LEVEL_FINER,
                "baseBridge_stop(): defaultEventQueue is NULL");
      return MAMA_STATUS_NULL_ARG;
    }

    return mamaQueue_stopDispatch (defaultEventQueue);;
}

/* Must be immplemented in overriding bridge */
const char*
baseBridge_getVersion (void)
{
    return "UNDEFINED (BASE BRIDGE)";
}

/* Must be immplemented in overriding bridge */
const char*
baseBridge_getName (void)
{
    return "UNDEFINED (BASE BRIDGE)";
}

/* Must be immplemented in overriding bridge */
mama_status
baseBridge_getDefaultPayloadId (char ***name, char **id)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}
