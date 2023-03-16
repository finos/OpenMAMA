#ifndef MAMA_RESOURCE_POOL_CPP_H__
#define MAMA_RESOURCE_POOL_CPP_H__

#include <mama/config.h>
#include <mama/status.h>
#include <mama/MamaSubscription.h>
#include <mama/MamaTransport.h>
#include <string>

namespace Wombat
{
    class MamaSubscription;
    class MamaTransport;
    class MamaResourcePoolImpl;

    class MAMACPPExpDLL MamaResourcePool
    {
    public:
        /**
         * Creates a MAMA resource pool to manage subscription lifecycles on behalf of
         * the application calling it.
         *
         * @param name         The name of the MAMA resource pool in configuration
         */
        explicit MamaResourcePool (const char* name);

        /**
         * Destroys a MAMA resource pool and all subscriptions, transports etc that
         * were created using it.
         */
        ~MamaResourcePool ();

        /**
         * Creates a MAMA Subscription using the given uri which is expected to be a
         * null terminated string in "bridge://transport/source/topic?param=1" format.
         *
         * @param uri           A url string in "bridge://transport/source/topic?param=1"
         *                      format
         * @param callbacks     A list of mama message callbacks to invoke during the
         *                      subscription's life cycle
         * @param closure       An opaque object to point to your callbacks
         * @result A pointer to a mama subscription object, or NULL on failure
         */
        MamaSubscription* createSubscriptionFromUri (
            const char* uri,
            MamaSubscriptionCallback* callbacks,
            void* closure = nullptr
        );

        /**
         * Creates a MAMA Subscription using the given components which are referenced
         * by name.
         *
         * @param transportName The name of the transport to use. If it has not yet been
         *                      used, it will be created in the process here.
         * @param sourceName    The name of the source to use. If it has not yet been
         *                      used, it will be created in the process here.
         * @param topicName     The name of the source to use. If it has not yet been
         *                      used, it will be created in the process here.
         * @param callbacks     A list of mama message callbacks to invoke during the
         *                      subscription's life cycle
         * @param closure       An opaque object to point to your callbacks
         * @result A pointer to a mama subscription object, or NULL on failure
         */
        MamaSubscription* createSubscriptionFromComponents (
            const char* transportName,
            const char* sourceName,
            const char* topicName,
            MamaSubscriptionCallback* callbacks,
            void* closure = nullptr
        );

        /**
         * Creates a MAMA Subscription using the given topic and source and the default
         * resource pool's subscribing transport (default_transport_sub).
         *
         * @param sourceName    The name of the source to use. If it has not yet been
         *                      used, it will be created in the process here.
         * @param topicName     The name of the source to use. If it has not yet been
         *                      used, it will be created in the process here.
         * @param callbacks     A list of mama message callbacks to invoke during the
         *                      subscription's life cycle
         * @param closure       An opaque object to point to your callbacks
         * @result A pointer to a mama subscription object, or NULL on failure
         */
        MamaSubscription* createSubscriptionFromTopicWithSource (
            const char* sourceName,
            const char* topicName,
            MamaSubscriptionCallback* callbacks,
            void* closure = nullptr
        );

        /**
         * Creates a MAMA Subscription using the given topic the default
         * resource pool's subscribing source (default_source_sub).
         *
         * @param topicName     The name of the source to use. If it has not yet been
         *                      used, it will be created in the process here.
         * @param callbacks     A list of mama message callbacks to invoke during the
         *                      subscription's life cycle
         * @param closure       An opaque object to point to your callbacks
         * @result A pointer to a mama subscription object, or NULL on failure
         */
        MamaSubscription* createSubscriptionFromTopic (
            const char* topicName,
            MamaSubscriptionCallback* callbacks,
            void* closure = nullptr
        );

        /**
         * Creates a MAMA Subscription using the given topic the default
         * resource pool's subscribing source (default_source_sub).
         *
         * @param topicName     The name of the source to use. If it has not yet been
         *                      used, it will be created in the process here.
         * @param callbacks     A list of mama message callbacks to invoke during the
         *                      subscription's life cycle
         * @param closure       An opaque object to point to your callbacks
         * @result A pointer to a mama subscription object, or NULL on failure
         */
        MamaTransport* createTransportFromName (
            const char* transportName
        );

        /**
         * Destroys the given subscription and removes it from the resource pool
         * asynchronously. There may be further callbacks after calling this function
         * since this event is enqueued on the subscription's thread, so for any closure
         * related shutdown code, cleanup should be performed in the subscription's
         * onDestroy callback.
         *
         * @param subscription  The subscription to destroy
         */
        void destroySubscription (
            MamaSubscription* subscription
        );
    private:
        // No non-explicit constructors enabled by default
        MamaResourcePool(const MamaResourcePool& other);
        MamaResourcePool(MamaResourcePool&& other);
        MamaResourcePool& operator=(const MamaResourcePool& other);
        MamaResourcePool& operator=(MamaResourcePool&& other);
        MamaResourcePoolImpl* mPimpl;
    };

} // namespace Wombat


#endif // MAMA_RESOURCE_POOL_CPP_H__
