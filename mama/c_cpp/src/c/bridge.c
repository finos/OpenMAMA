/* $Id: bridge.c,v 1.8.12.1.4.4 2011/09/07 11:01:05 ianbell Exp $
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
#include "bridge.h"

int mamaBridgeImpl_getDefaultQueueTimeout(void)
{
    /* Returns. */
    int ret = MAMA_BRIDGE_DEFAULT_QUEUE_DEFAULT_TIMEOUT;

    /* Read the property. */
    const char *propertyString = properties_Get(mamaInternal_getProperties(), MAMA_BRIDGE_DEFAULT_QUEUE_TIMEOUT_PROPERTY);
    if(NULL != propertyString)
    {
        ret = atoi(propertyString);
    }

    return ret;
}

mama_status
mamaBridgeImpl_setClosure (mamaBridge bridgeImpl, void* closure)
{
    mamaBridgeImpl* impl = (mamaBridgeImpl*)bridgeImpl;
    if (!impl) return MAMA_STATUS_NULL_ARG;

    impl->mClosure = closure;

    return MAMA_STATUS_OK;
}

mama_status
mamaBridgeImpl_setCppCallback (mamaBridge bridgeImpl, void* cppCallback)
{
    mamaBridgeImpl* impl = (mamaBridgeImpl*)bridgeImpl;
    if (!impl) return MAMA_STATUS_NULL_ARG;

    impl->mCppCallback = cppCallback;

    return MAMA_STATUS_OK;
}
                                                                                                                     
mama_status
mamaBridgeImpl_getClosure (mamaBridge bridgeImpl, void** closure)
{
    mamaBridgeImpl* impl = (mamaBridgeImpl*)bridgeImpl;
    if (!impl) return MAMA_STATUS_NULL_ARG;
    if (!closure) return MAMA_STATUS_INVALID_ARG;

    *closure = impl->mClosure;

    return MAMA_STATUS_OK;
}
       
mama_status
mamaBridgeImpl_getInternalEventQueue (mamaBridge bridgeImpl, mamaQueue* internalQueue)
{
    mamaBridgeImpl* impl =  (mamaBridgeImpl*)bridgeImpl;

    if (!impl)
    {
        mama_log (MAMA_LOG_LEVEL_WARN, "mamaBridgeImpl_getInternalEventQueue(): "
                  "No bridge implementation specified");
        return MAMA_STATUS_NO_BRIDGE_IMPL;
    }

    if (!impl->mInternalEventQueue)
    {
        if (MAMA_STATUS_OK != mamaQueue_create (&impl->mInternalEventQueue,
                                            bridgeImpl))
        {
            mama_log (MAMA_LOG_LEVEL_WARN, "mamaBridgeImpl_getInternalEventQueue(): "
                          "Could not create internal queue");
            return MAMA_STATUS_NO_BRIDGE_IMPL;
        }
            
        if (MAMA_STATUS_OK != mamaDispatcher_create (&impl->mInternalDispatcher , 
                    impl->mInternalEventQueue))
        {
            mama_log (MAMA_LOG_LEVEL_WARN, "mamaBridgeImpl_getInternalEventQueue(): "
                          "Could not create internal dispatcher");
            return MAMA_STATUS_NO_BRIDGE_IMPL;
        }
    }
    
    *internalQueue = impl->mInternalEventQueue;
    return MAMA_STATUS_OK;
}

mama_status
mamaBridgeImpl_stopInternalEventQueue (mamaBridge bridgeImpl)
{
    mamaBridgeImpl* impl =  (mamaBridgeImpl*)bridgeImpl;

    if (!impl)
    {
        mama_log (MAMA_LOG_LEVEL_WARN, "mamaBridgeImpl_stopInternalEventQueue(): "
                  "No bridge implementation specified");
        return MAMA_STATUS_NO_BRIDGE_IMPL;
    }

    if (impl->mInternalEventQueue)
    {
           
    	/* Get the queue timeout value. */
    	int defaultQueueTimeout = mamaBridgeImpl_getDefaultQueueTimeout();
        if (MAMA_STATUS_OK != mamaDispatcher_destroy (impl->mInternalDispatcher))
        {
            mama_log (MAMA_LOG_LEVEL_WARN, "mamaBridgeImpl_stopInternalEventQueue(): "
                          "Could not destroy internal dispatcher");
            return MAMA_STATUS_NO_BRIDGE_IMPL;
        }
        
        /* Destroy the queue waiting for the appropriate time value. */
        if (MAMA_STATUS_OK != mamaQueue_destroyTimedWait (impl->mInternalEventQueue, defaultQueueTimeout))
        {
            mama_log (MAMA_LOG_LEVEL_WARN, "mamaBridgeImpl_stopInternalEventQueue(): "
                          "Could not destroy internal queue");
            return MAMA_STATUS_NO_BRIDGE_IMPL;
        }
    }
    
    return MAMA_STATUS_OK;
}
