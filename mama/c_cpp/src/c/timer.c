/* $Id: timer.c,v 1.11.4.2.2.1.4.3 2011/08/10 14:53:26 nicholasmarriott Exp $
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

#include <mama/mama.h>
#include <mama/timer.h>
#include "bridge.h"
#include "queueimpl.h"
#include <assert.h>                                                                                                                     

/* Function Prototypes. */
void MAMACALLTYPE mamaTimer_onTimerDestroyed(mamaTimer timer, void *closure);

/*Implementation of the mamaTimer structure*/
typedef struct mamaTimerImpl_
{
    mamaBridgeImpl* mBridgeImpl;
    mamaQueue       mQueue;
    timerBridge     mMamaTimerBridgeImpl;

    /* This callback will be invoked whenever the timer has been completely destroyed. */
    mamaTimerCb     mOnTimerDestroyed;

    /* The queue lock handle. */
    mamaQueueLockHandle mLockHandle;

} mamaTimerImpl;

mama_status
mamaTimer_create(mamaTimer*   result,
                 mamaQueue    queue,
                 mamaTimerCb  action, 
                 mama_f64_t   interval,
                 void*        closure)
{
   return mamaTimer_create2(result, queue, action, NULL, interval, closure);
}

mama_status
mamaTimer_create2(mamaTimer*   result,
                  mamaQueue    queue,
                  mamaTimerCb  action, 
                  mamaTimerCb  onTimerDestroyed, 
                  mama_f64_t   interval,
                  void*        closure)
{
    mama_status     status              =   MAMA_STATUS_OK;
    if (MAMA_STATUS_OK!=(status=mamaTimer_allocate2 (result, queue, onTimerDestroyed)))
        return status;
        
    if (MAMA_STATUS_OK!=(status=mamaTimer_start(*result,action, interval,closure)))
        return status;
        
    return status;
}

mama_status
mamaTimer_allocate(mamaTimer*   result,
                   mamaQueue    queue)
{
    /* Call the overload with NULL for the timer destroyed callback. */
    return mamaTimer_allocate2(result, queue, NULL);
}

mama_status
mamaTimer_allocate2(mamaTimer*   result,
                    mamaQueue    queue,
                    mamaTimerCb  onTimerDestroyed)
{
    mamaTimerImpl*  impl                =   NULL;
    mamaBridgeImpl* bridgeImpl          =   NULL;
    
    if (!result) 
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaTimer_allocate(): NULL timer"
                  " address.");
        return MAMA_STATUS_NULL_ARG;
    }
    
    *result = NULL;

    if (!queue) 
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaTimer_allocate(): NULL Queue.");
        assert (queue);
        return MAMA_STATUS_INVALID_QUEUE;
    }
    
    /*Get the bridge impl from the queue - mandatory*/
    bridgeImpl = mamaQueueImpl_getBridgeImpl (queue);

    if (!bridgeImpl)
        return MAMA_STATUS_NO_BRIDGE_IMPL;


    impl = (mamaTimerImpl*)calloc (1, sizeof (mamaTimerImpl));

    if (!impl)
        return MAMA_STATUS_NOMEM;
    
    impl->mBridgeImpl           = bridgeImpl;
    impl->mQueue                = queue;
    impl->mOnTimerDestroyed     = onTimerDestroyed;

    /* Increment the object lock count on the queue. */
    impl->mLockHandle = mamaQueue_incrementObjectCount(impl->mQueue, impl);
    
    *result = (mamaTimer)impl;
        
    return MAMA_STATUS_OK;
}

mama_status
mamaTimer_start(mamaTimer   result,
                mamaTimerCb action, 
                mama_f64_t  interval,
                void*       closure)
{
    mama_status     status              =  MAMA_STATUS_OK;
    mamaTimerImpl*  impl                =  NULL;
    mamaBridgeImpl* bridgeImpl          =  NULL;
    mamaQueue       queue               =  NULL;
    void*           nativeQueueHandle   =  NULL;
    
    if (!result)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaTimer_start(): NULL timer");
        return MAMA_STATUS_NULL_ARG;
    }
    
    impl          = (mamaTimerImpl*) result;
    bridgeImpl    = impl->mBridgeImpl;
    queue         = impl->mQueue;
    
    if (impl->mMamaTimerBridgeImpl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaTimer_start(): Already Started.");
        return MAMA_STATUS_NULL_ARG;
    }
     
    /*Get the native queue handle*/
    mamaQueue_getNativeHandle (queue, &nativeQueueHandle);
    if (!nativeQueueHandle) 
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, 
                  "mamaTimer_start(): Could not get native queue handle.");
        return MAMA_STATUS_INVALID_QUEUE;
    }
    
    
    /*Create the corresponding bridge specific structure*/
    if (MAMA_STATUS_OK!=(status=bridgeImpl->bridgeMamaTimerCreate (
                                &impl->mMamaTimerBridgeImpl,
                                nativeQueueHandle,
                                action,
                                mamaTimer_onTimerDestroyed,
                                interval,
                                (mamaTimer)impl,
                                closure)))
    {
        free (impl);
        return status;
    }
    
    return status;
}
 
void MAMACALLTYPE 
mamaTimer_onTimerDestroyed(mamaTimer timer, void *closure)
{
    /* Obtain the impl from the timer object. */
    mamaTimerImpl *impl = (mamaTimerImpl *)timer;

    /* Invoke the user's callback if it has been supplied. */
    if (impl->mOnTimerDestroyed != NULL)
    {
        (*impl->mOnTimerDestroyed)(timer, closure);
    }

    /* Decrement the open object count on the queue. */
    mamaQueue_decrementObjectCount(&impl->mLockHandle, impl->mQueue);

    /* Free the timer structure itself. */
    free(impl);
}

mama_status
mamaTimer_destroy(mamaTimer timer)
{
    mamaTimerImpl* impl = (mamaTimerImpl*)timer;
    mama_status    status   =   MAMA_STATUS_OK;

    if (!impl) 
        return MAMA_STATUS_NULL_ARG;

    if (!impl->mMamaTimerBridgeImpl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaTimer_destroy(): "
                  "No timer bridge impl. Cannot destroy.");
        status = MAMA_STATUS_INVALID_ARG;
    }
    
    if (!impl->mBridgeImpl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaTimer_destroy() "
                  "No bridge impl. Cannot destroy.");
        status = MAMA_STATUS_NO_BRIDGE_IMPL;
    }

    if (impl->mMamaTimerBridgeImpl && impl->mBridgeImpl)
    {
        if (MAMA_STATUS_OK!=(status=impl->mBridgeImpl->bridgeMamaTimerDestroy
                            (impl->mMamaTimerBridgeImpl)))
        {
            mama_log (MAMA_LOG_LEVEL_ERROR, "mamaTimer_destroy(): "
                  "Could not destroy timer bridge.");
        }
    }
    
    return status;
}

mama_status
mamaTimer_reset(mamaTimer timer)
{
    mamaTimerImpl* impl = (mamaTimerImpl*)timer;

    if (!impl)
        return MAMA_STATUS_NULL_ARG;

    if (!impl->mMamaTimerBridgeImpl)
        return MAMA_STATUS_INVALID_ARG;

    if (!impl->mBridgeImpl)
        return MAMA_STATUS_NO_BRIDGE_IMPL;
    
    return impl->mBridgeImpl->bridgeMamaTimerReset (impl->mMamaTimerBridgeImpl);
}

mama_status
mamaTimer_setInterval(mamaTimer   timer,
                      mama_f64_t  interval)
{
    mamaTimerImpl* impl = (mamaTimerImpl*)timer;

    if (!impl)
        return MAMA_STATUS_NULL_ARG;

    if (!impl->mMamaTimerBridgeImpl)
        return MAMA_STATUS_INVALID_ARG;

    if (!impl->mBridgeImpl)
        return MAMA_STATUS_NO_BRIDGE_IMPL;
    
    return impl->mBridgeImpl->bridgeMamaTimerSetInterval 
                    (impl->mMamaTimerBridgeImpl,
                     interval);
}

mama_status
mamaTimer_getInterval(const mamaTimer   timer,
                      mama_f64_t*       interval)
{
    mamaTimerImpl* impl = (mamaTimerImpl*)timer;

    if (!impl)
        return MAMA_STATUS_NULL_ARG;

    if (!impl->mMamaTimerBridgeImpl)
        return MAMA_STATUS_INVALID_ARG;

    if (!impl->mBridgeImpl)
        return MAMA_STATUS_NO_BRIDGE_IMPL;
    
    return impl->mBridgeImpl->bridgeMamaTimerGetInterval 
                    (impl->mMamaTimerBridgeImpl,
                     interval);
}

mama_status
mamaTimer_getQueue (const mamaTimer   timer,
                    mamaQueue*        result)
{
    mamaTimerImpl* impl = (mamaTimerImpl*)timer;

    if (!impl)
        return MAMA_STATUS_NULL_ARG;

    *result = impl->mQueue;
    
    return MAMA_STATUS_OK;
}
