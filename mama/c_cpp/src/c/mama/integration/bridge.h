/**
 * This file contains methods which are intended for use only by OpenMAMA core,
 * bridges and plugins. These methods typically only make sense to developers
 * who have intimate knowledge of the inner workings of OpenMAMA. Its use in
 * applications is heavily discouraged and entirely unsupported.
 *
 * Note that you can only use these methods if you have defined the
 * OPENMAMA_INTEGRATION macro. If you think a case is to be made for accessing
 * one of these methods, please raise to the mailing list and we'll investigate
 * opening up a more stable formal interface for the standard public API.
 */

#if defined (OPENMAMA_INTEGRATION) && ! defined (OPENMAMA_INTEGRATION_BRIDGE_H__)
#define OPENMAMA_INTEGRATION_BRIDGE_H__

#include <mama/mama.h>

#if defined (__cplusplus)
extern "C"
{
#endif

/**
 * This function will return the timeout value to be used when destroying the default queue.
 * The value is read from the properties file, (MAMA_BRIDGE_DEFAULT_QUEUE_TIMEOUT_PROPERTY),
 * but will default to MAMA_BRIDGE_DEFAULT_QUEUE_DEFAULT_TIMEOUT if the property is missing.
 *
 * @return The timeout value.
 */
MAMAExpDLL
extern int
mamaBridgeImpl_getDefaultQueueTimeout(void);

MAMAExpDLL
extern mama_status
mamaBridgeImpl_setClosure (mamaBridge bridgeImpl, void* closure);

MAMAExpDLL
extern mama_status
mamaBridgeImpl_setCppCallback (mamaBridge bridgeImpl, void* cppCallback);

MAMAExpDLL
extern mama_status
mamaBridgeImpl_getClosure (mamaBridge bridgeImpl, void** closure);

MAMAExpDLL
mama_status
mamaBridgeImpl_getInternalEventQueue (mamaBridge bridgeImpl, mamaQueue* internalQueue);

MAMAExpDLL
mama_status
mamaBridgeImpl_stopInternalEventQueue (mamaBridge bridgeImpl);

MAMAExpDLL
extern mama_status
mamaBridgeImpl_setReadOnlyProperty (mamaBridge bridgeImpl, const char* property, const char* value);

MAMAExpDLL
extern mama_bool_t
mamaBridgeImpl_areEntitlementsDeferred (mamaBridge bridgeImpl);

MAMAExpDLL
const char*
mamaBridgeImpl_getMetaProperty (mamaBridge bridgeImpl, const char* property);

MAMAExpDLL
extern void
mamaBridgeImpl_populateBridgeMetaData (mamaBridge bridgeImpl);

#if defined (__cplusplus)
}
#endif

#endif /* OPENMAMA_INTEGRATION_BRIDGE_H__ */
