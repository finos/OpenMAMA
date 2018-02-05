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
#include "mamainternal.h"
#include <wombat/strutils.h>
#include "bridge.h"

void
mamaBridgeImpl_getNormalizedPropertyName (mamaBridgeImpl*   impl,
                                          char*             propertyName,
                                          size_t            propertyLen,
                                          const char*       property);

mama_status
mamaBridgeImpl_setMetaProperty (mamaBridge      bridgeImpl,
                                const char*     property,
                                const char*     value);

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

/* All properties are write-once */
mama_status
mamaBridgeImpl_setReadOnlyProperty (mamaBridge bridgeImpl, const char* property, const char* value)
{
    return mamaBridgeImpl_setMetaProperty(bridgeImpl, property, value);
}

/*
 * Expects a bridge implementation, a pointer to a char buffer to populate with
 * the result, a size of this buffer and the property to normalize.
 */
void
mamaBridgeImpl_getNormalizedPropertyName (mamaBridgeImpl*   impl,
                                          char*             propertyName,
                                          size_t            propertyLen,
                                          const char*       property)
{
    char prefix[MAX_INTERNAL_PROP_LEN];

    /*
     * For bridge properties, accept both 'mama.bridgename.propname' and simply
     * 'propname'
     */
    snprintf (prefix, sizeof(prefix), "mama.%s.", impl->bridgeGetName());

    /* If the prefix is equal to the first characters of value */
    if (strlenEx(property) > strlenEx(prefix) &&
            0 == strncmp (prefix, property, strlen(prefix)))
    {
        /* Move past prefix */
        property += strlenEx(prefix);
    }

    /* Build full property string for bridge specific internal property */
    snprintf (propertyName,
              propertyLen,
              "mama.%s.%s",
              impl->bridgeGetName(),
              property);

    return;
}

mama_status
mamaBridgeImpl_setMetaProperty (mamaBridge      bridgeImpl,
                            const char*     property,
                            const char*     value)
{
    mamaBridgeImpl* impl = (mamaBridgeImpl*)bridgeImpl;
    char            propString[MAX_INTERNAL_PROP_LEN];

    mamaBridgeImpl_getNormalizedPropertyName (impl,
                                              propString,
                                              sizeof(propString),
                                              property);

    return mamaInternal_setMetaProperty (propString, value);
}

const char*
mamaBridgeImpl_getMetaProperty (mamaBridge bridgeImpl, const char* property)
{
    mamaBridgeImpl* impl = (mamaBridgeImpl*)bridgeImpl;
    char            propString[MAX_INTERNAL_PROP_LEN];

    mamaBridgeImpl_getNormalizedPropertyName (impl,
                                              propString,
                                              sizeof(propString),
                                              property);

    return mamaInternal_getMetaProperty (propString);
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

/* This must be called immediately after bridge init function called */
void
mamaBridgeImpl_populateBridgeMetaData (mamaBridge bridgeImpl)
{
    mamaBridgeImpl* impl = (mamaBridgeImpl*)bridgeImpl;

    /* Populate bridge member for fast access later */
    impl->mEntitleDeferred = strtobool (
            mamaBridgeImpl_getMetaProperty (bridgeImpl, "entitlements.deferred"));

}

