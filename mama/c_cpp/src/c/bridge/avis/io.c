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
#include <mama/io.h>
#include <wombat/port.h>
#include "avisbridgefunctions.h"
#include "io.h"
#include <event.h>

/*=========================================================================
  =                Typedefs, structs, enums and globals                   =
  =========================================================================*/

typedef struct avisIoImpl
{
    struct event_base*  mEventBase;
    wthread_t           mDispatchThread;
    uint8_t             mActive;
    uint8_t             mEventsRegistered;
    wsem_t              mResumeDispatching;
} avisIoImpl;

typedef struct avisIoEventImpl
{
    uint32_t            mDescriptor;
    mamaIoCb            mAction;
    mamaIoType          mIoType;
    mamaIo              mParent;
    void*               mClosure;
    struct event        mEvent;
} avisIoEventImpl;

/*
 * Global static container to hold instance-wide information not otherwise
 * available in this interface.
 */
static avisIoImpl       gAvisIoContainer;


/*=========================================================================
  =                  Private implementation prototypes                    =
  =========================================================================*/

void*
avisBridgeMamaIoImpl_dispatchThread (void* closure);

void
avisBridgeMamaIoImpl_libeventIoCallback (int fd, short type, void* closure);


/*=========================================================================
  =                   Public implementation functions                     =
  =========================================================================*/

/* Not implemented in the avis bridge */
mama_status
avisBridgeMamaIo_create         (ioBridge*   result,
                                 void*       nativeQueueHandle,
                                 uint32_t    descriptor,
                                 mamaIoCb    action,
                                 mamaIoType  ioType,
                                 mamaIo      parent,
                                 void*       closure)
{
    avisIoEventImpl*    impl    = NULL;
    short               evtType = 0;

    if (NULL == result)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    *result = 0;

    /* Check for supported types so we don't prematurely allocate */
    switch (ioType)
    {
    case MAMA_IO_READ:
        evtType = EV_READ;
        break;
    case MAMA_IO_WRITE:
        evtType = EV_WRITE;
        break;
    case MAMA_IO_ERROR:
        evtType = EV_READ | EV_WRITE;
        break;
    case MAMA_IO_CONNECT:
    case MAMA_IO_ACCEPT:
    case MAMA_IO_CLOSE:
    case MAMA_IO_EXCEPT:
    default:
        return MAMA_STATUS_UNSUPPORTED_IO_TYPE;
        break;
    }

    impl = (avisIoEventImpl*) calloc (1, sizeof (avisIoEventImpl));
    if (NULL == impl)
    {
        return MAMA_STATUS_NOMEM;
    }

    impl->mDescriptor           = descriptor;
    impl->mAction               = action;
    impl->mIoType               = ioType;
    impl->mParent               = parent;
    impl->mClosure              = closure;

    event_set (&impl->mEvent,
               impl->mDescriptor,
               evtType,
               avisBridgeMamaIoImpl_libeventIoCallback,
               impl);

    event_add (&impl->mEvent, NULL);

    event_base_set (gAvisIoContainer.mEventBase, &impl->mEvent);

    /* If this is the first event since base was emptied or created */
    if (0 == gAvisIoContainer.mEventsRegistered)
    {
        wsem_post (&gAvisIoContainer.mResumeDispatching);
    }
    gAvisIoContainer.mEventsRegistered++;

    *result = (ioBridge)impl;

    return MAMA_STATUS_OK;
}

/* Not implemented in the avis bridge */
mama_status
avisBridgeMamaIo_destroy        (ioBridge io)
{
    avisIoEventImpl* impl = (avisIoEventImpl*) io;
    if (NULL == io)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    event_del (&impl->mEvent);

    free (impl);
    gAvisIoContainer.mEventsRegistered--;

    return MAMA_STATUS_OK;
}

/* Not implemented in the avis bridge */
mama_status
avisBridgeMamaIo_getDescriptor  (ioBridge    io,
                                 uint32_t*   result)
{
    avisIoEventImpl* impl = (avisIoEventImpl*) io;
    if (NULL == io || NULL == result)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    *result = impl->mDescriptor;

    return MAMA_STATUS_OK;
}

/*=========================================================================
  =                  Public implementation prototypes                     =
  =========================================================================*/

mama_status
avisBridgeMamaIoImpl_start (void)
{
    int threadResult                        = 0;
    gAvisIoContainer.mEventsRegistered      = 0;
    gAvisIoContainer.mActive                = 1;
    gAvisIoContainer.mEventBase             = event_init ();

    wsem_init (&gAvisIoContainer.mResumeDispatching, 0, 0);
    threadResult = wthread_create (&gAvisIoContainer.mDispatchThread,
                                   NULL,
                                   avisBridgeMamaIoImpl_dispatchThread,
                                   gAvisIoContainer.mEventBase);
    if (0 != threadResult)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "avisBridgeMamaIoImpl_initialize(): "
                  "wthread_create returned %d", threadResult);
        return MAMA_STATUS_PLATFORM;
    }
    return MAMA_STATUS_OK;
}

mama_status
avisBridgeMamaIoImpl_stop (void)
{
    gAvisIoContainer.mActive = 0;

    /* Alert the semaphore so the dispatch loop can exit */
    wsem_post (&gAvisIoContainer.mResumeDispatching);

    /* Tell the event loop to exit */
    event_base_loopexit (gAvisIoContainer.mEventBase, NULL);

    /* Join with the dispatch thread - it should exit shortly */
    wthread_join (gAvisIoContainer.mDispatchThread, NULL);
    wsem_destroy (&gAvisIoContainer.mResumeDispatching);

    /* Free the main event base */
    event_base_free (gAvisIoContainer.mEventBase);

    return MAMA_STATUS_OK;
}



/*=========================================================================
  =                  Private implementation prototypes                    =
  =========================================================================*/

void*
avisBridgeMamaIoImpl_dispatchThread (void* closure)
{
    int             dispatchResult = 0;

    /* Wait on the first event to register before starting dispatching */
    wsem_wait (&gAvisIoContainer.mResumeDispatching);

    while (0 != gAvisIoContainer.mActive)
    {
        dispatchResult = event_base_loop (gAvisIoContainer.mEventBase,
                                          EVLOOP_NONBLOCK | EVLOOP_ONCE);

        /* If no events are currently registered */
        if (1 == dispatchResult)
        {
            /* Wait until they are */
            gAvisIoContainer.mEventsRegistered = 0;
            wsem_wait (&gAvisIoContainer.mResumeDispatching);
        }
    }
    return NULL;
}

void
avisBridgeMamaIoImpl_libeventIoCallback (int fd, short type, void* closure)
{
    avisIoEventImpl* impl = (avisIoEventImpl*) closure;

    /* Timeout is the only error detectable with libevent */
    if (EV_TIMEOUT == type)
    {
        /* If this is an error IO type, fire the callback */
        if (impl->mIoType == MAMA_IO_ERROR && NULL != impl->mAction)
        {
            (impl->mAction)(impl->mParent, impl->mIoType, impl->mClosure);
        }
        /* If this is not an error IO type, do nothing */
        else
        {
            return;
        }
    }

    /* Call the action callback if defined */
    if (NULL != impl->mAction)
    {
        (impl->mAction)(impl->mParent, impl->mIoType, impl->mClosure);
    }

    /* Enqueue for the next time */
    event_add (&impl->mEvent, NULL);
}
