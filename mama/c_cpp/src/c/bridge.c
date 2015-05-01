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

#include <mama/mama.h>
#include <wombat/strutils.h>
#include "bridge.h"

#define MAX_PROP_STRING                   1000
#define PROP_NAME_ENTITLEMENTS_DEFERRED   "entitlements.deferred"

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

	wlock_lock (impl->mLock);
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
    wlock_unlock (impl->mLock);
    
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

mama_status
mamaBridgeImpl_setReadOnlyProperty (mamaBridge bridgeImpl, const char* property, const char* value)
{
    mamaBridgeImpl_setProperty(bridgeImpl, property, value);
    bridgeImpl->mEntitleReadOnly = 1;
    return MAMA_STATUS_OK;
}

mama_status
mamaBridgeImpl_setProperty (mamaBridge bridgeImpl, const char* property, const char* value)
{
    char propString[MAX_PROP_STRING];

    mamaBridgeImpl* impl = (mamaBridgeImpl*)bridgeImpl;

    /* Check for mama.middleware.entitlements_deferred first */
    snprintf(propString, MAX_PROP_STRING,
        "mama.%s.%s",
        impl->bridgeGetName(),
        PROP_NAME_ENTITLEMENTS_DEFERRED);

    if(0 == strcmp(property, propString))
    {
        if (1 == bridgeImpl->mEntitleReadOnly)
        {
            mama_log (MAMA_LOG_LEVEL_WARN, "mamaBridgeImpl_setProperty(): "
                      "Bridge is read only, property can not be set.");
            return MAMA_STATUS_INVALID_ARG;
        }
        else
        {
            if (strtobool(value))
                bridgeImpl->mEntitleDeferred = 1;
            else
                bridgeImpl->mEntitleDeferred = 0;
        }
    }
    else
    {
        mama_log (MAMA_LOG_LEVEL_WARN, "mamaBridgeImpl_setProperty(): "
            "Unknown property string [%s] entered.", property);
        return MAMA_STATUS_INVALID_ARG;
    }
    return MAMA_STATUS_OK;
}

const char*
mamaBridgeImpl_getProperty (mamaBridge bridgeImpl, const char* property)
{
    return NULL;
}

mama_bool_t
mamaBridgeImpl_areEntitlementsDeferred (mamaBridge bridgeImpl)
{
    if (bridgeImpl)
    {
        return bridgeImpl->mEntitleDeferred;
    }
    return 0;
}
