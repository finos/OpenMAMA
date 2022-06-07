#ifndef MamaResourcePoolH__
#define MamaResourcePoolH__

#if defined(__cplusplus)
extern "C"
{
#endif

#include <mama/config.h>
#include <mama/status.h>
#include <mama/types.h>
#include <mama/subscription.h>


/**
 * Creates a MAMA resource pool to manage subscription lifecycles on behalf of
 * the application calling it.
 *
 * @param resourcePool A pointer to populate with the newly allocated pool.
 * @param name         The name of the MAMA resource pool in configuration
 * @result A mama_status value to reflect if this operation was successful.
 */
MAMAExpDLL
extern mama_status
mamaResourcePool_create (
    mamaResourcePool* resourcePool,
    const char* name
);


/**
 * Destroys a MAMA resource pool and all subscriptions, transports etc that
 * were created using it.
 *
 * @param resourcePool A reference to the mama resource pool to destroy
 * @result A mama_status value to reflect if this operation was successful.
 */
MAMAExpDLL
extern mama_status
mamaResourcePool_destroy (
    mamaResourcePool resourcePool
);


/**
 * Creates a MAMA Subscription using the given uri which is expected to be a
 * null terminated string in "bridge://transport/source/topic?param=1" format.
 *
 * @param resourcePool  A reference to the mama resource pool to use
 * @param subscription  A pointer which this function will populate with any
 *                      newly created subscriptions.
 * @param uri           A url string in "bridge://transport/source/topic?param=1"
 *                      format
 * @param callbacks     A list of mama message callbacks to invoke during the
 *                      subscription's life cycle
 * @param closure       An opaque object to point to your callbacks
 * @result A mama_status value to reflect if this operation was successful.
 */
MAMAExpDLL
extern mama_status
mamaResourcePool_createSubscriptionFromUri (
    mamaResourcePool resourcePool,
    mamaSubscription* subscription,
    const char* uri,
    mamaMsgCallbacks callbacks,
    void* closure
);


/**
 * Creates a MAMA Subscription using the given components which are referenced
 * by name.
 *
 * @param resourcePool  A reference to the mama resource pool to use
 * @param subscription  A pointer which this function will populate with any
 *                      newly created subscriptions.
 * @param transportName The name of the transport to use. If it has not yet been
 *                      used, it will be created in the process here.
 * @param sourceName    The name of the source to use. If it has not yet been
 *                      used, it will be created in the process here.
 * @param topicName     The name of the source to use. If it has not yet been
 *                      used, it will be created in the process here.
 * @param callbacks     A list of mama message callbacks to invoke during the
 *                      subscription's life cycle
 * @param closure       An opaque object to point to your callbacks
 * @result A mama_status value to reflect if this operation was successful.
 */
MAMAExpDLL
extern mama_status
mamaResourcePool_createSubscriptionFromComponents (
    mamaResourcePool resourcePool,
    mamaSubscription* subscription,
    const char* transportName,
    const char* sourceName,
    const char* topicName,
    mamaMsgCallbacks callbacks,
    void* closure
);


/**
 * Creates a MAMA Subscription using the given topic and source and the default
 * resource pool's subscribing transport (default_transport_sub).
 *
 * @param resourcePool  A reference to the mama resource pool to use
 * @param subscription  A pointer which this function will populate with any
 *                      newly created subscriptions.
 * @param sourceName    The name of the source to use. If it has not yet been
 *                      used, it will be created in the process here.
 * @param topicName     The name of the source to use. If it has not yet been
 *                      used, it will be created in the process here.
 * @param callbacks     A list of mama message callbacks to invoke during the
 *                      subscription's life cycle
 * @param closure       An opaque object to point to your callbacks
 * @result A mama_status value to reflect if this operation was successful.
 */
MAMAExpDLL
extern mama_status
mamaResourcePool_createSubscriptionFromTopicWithSource (
    mamaResourcePool resourcePool,
    mamaSubscription* subscription,
    const char* sourceName,
    const char* topicName,
    mamaMsgCallbacks callbacks,
    void* closure
);


/**
 * Creates a MAMA Subscription using the given topic the default
 * resource pool's subscribing source (default_source_sub).
 *
 * @param resourcePool  A reference to the mama resource pool to use
 * @param subscription  A pointer which this function will populate with any
 *                      newly created subscriptions.
 * @param topicName     The name of the source to use. If it has not yet been
 *                      used, it will be created in the process here.
 * @param callbacks     A list of mama message callbacks to invoke during the
 *                      subscription's life cycle
 * @param closure       An opaque object to point to your callbacks
 * @result A mama_status value to reflect if this operation was successful.
 */
MAMAExpDLL
extern mama_status
mamaResourcePool_createSubscriptionFromTopic (
    mamaResourcePool resourcePool,
    mamaSubscription* subscription,
    const char* topicName,
    mamaMsgCallbacks callbacks,
    void* closure
);


/**
 * Creates a MAMA Subscription using the given topic the default
 * resource pool's subscribing source (default_source_sub).
 *
 * @param resourcePool  A reference to the mama resource pool to use
 * @param subscription  A pointer which this function will populate with any
 *                      newly created subscriptions.
 * @param topicName     The name of the source to use. If it has not yet been
 *                      used, it will be created in the process here.
 * @param callbacks     A list of mama message callbacks to invoke during the
 *                      subscription's life cycle
 * @param closure       An opaque object to point to your callbacks
 * @result A mama_status value to reflect if this operation was successful.
 */
MAMAExpDLL
extern mama_status
mamaResourcePool_createTransportFromName (
    mamaResourcePool resourcePool,
    mamaTransport* transport,
    const char* transportName
);


/**
 * Destroys the given subscription and removes it from the resource pool
 * asynchronously. There may be further callbacks after calling this function
 * since this event is enqueued on the subscription's thread, so for any closure
 * related shutdown code, cleanup should be performed in the subscription's
 * onDestroy callback.
 *
 * @param resourcePool  A reference to the mama resource pool to use
 * @param subscription  The subscription to destroy
 * @result A mama_status value to reflect if this operation was successful.
 */
MAMAExpDLL
extern mama_status
mamaResourcePool_destroySubscription (
    mamaResourcePool resourcePool,
    mamaSubscription subscription
);

#if defined(__cplusplus)
}
#endif

#endif /* MamaResourcePoolH__ */