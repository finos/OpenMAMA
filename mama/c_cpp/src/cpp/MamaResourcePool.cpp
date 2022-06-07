#include <unordered_map>
#include <memory>
#include <mutex>
#include <set>
#include <vector>
#include <sstream>
#include <apr_pools.h>
#include <apr_uri.h>
#include <apr_general.h>

#include "mama/mamacpp.h"
#include "mama/MamaResourcePool.h"
#include "mamacppinternal.h"
#include "mama/MamaRegexChannelFilter.h"
#include "property.h"
#include "mama/integration/mama.h"
#include "wombat/CommonStrUtils.h"
#include "../c/resourcepool.h"
#include "mama/MamaQueueGroup.h"

#define MAMA_RESOURCE_POOL_DEFAULT_QUEUE_COUNT 4

using namespace Wombat;

namespace Wombat {

class MamaResourcePoolBridgeImpl {
public:
    MamaResourcePoolBridgeImpl(
        mamaBridge bridge,
        int numQueues,
        std::string& poolName) : mPoolName(poolName),
                                 mBridge(bridge),
                                 mQueueGroup (numQueues, bridge) {
        // Get the queue prefix from configuration
        const char* queuePrefix = mamaImpl_getParameter (
            "mrp", "mama.resource_pool.%s.thread_name_prefix",
            mPoolName.c_str());

        if (nullptr != queuePrefix)
        {
            // Do the rounds with each queue in the queue group and name each thread
            for (int i = 0; i < numQueues; i++)
            {
                auto queue = mQueueGroup.getNextQueue ();
                char queueName[PROPERTY_NAME_MAX_LENGTH];
                snprintf (
                    queueName, sizeof (queueName), "%s_%d", queuePrefix, i);
                queue->setQueueName (queueName);
            }
        }
    }
    std::string    mPoolName;
    MamaQueueGroup mQueueGroup;
    mamaBridge     mBridge;
};

class MamaResourcePoolTransportImpl {
public:
    std::shared_ptr<MamaResourcePoolBridgeImpl> mMrpBridge;
    MamaTransport* mTransport;
};

class MamaResourcePoolSourceImpl {
public:
    MamaSource* mSource;
};

class MamaResourcePoolSubscriptionImpl {
public:
    MamaSubscription* mSubscription;
    std::shared_ptr<MamaResourcePoolSourceImpl> mMrpSource;
};

class MamaResourcePoolImpl {
public:

    class DestroySubscriptionHandler : public MamaQueueEventCallback
    {
    public:
      void onEvent (MamaQueue& queue, void* closure) override {
          auto subscription = (MamaSubscription*) closure;
          subscription->destroy();
          delete subscription;
      }
    };

    explicit MamaResourcePoolImpl(std::string& name) :
          mNumQueues(MAMA_RESOURCE_POOL_DEFAULT_QUEUE_COUNT),
          mName(name),
          mAprPool(nullptr),
          mIsMamaOpened(false)
    {
        apr_status_t aprPoolStatus = apr_initialize();
        if (APR_SUCCESS != aprPoolStatus) {
            throw MamaStatus (MAMA_STATUS_NOMEM);
        }

        aprPoolStatus = apr_pool_create_unmanaged (&mAprPool);
        if (APR_SUCCESS != aprPoolStatus) {
            throw MamaStatus (MAMA_STATUS_NOMEM);
        }

        const char* bridgesProperty = mamaImpl_getParameter (
            "qpid", "mama.resource_pool.%s.bridges", mName.c_str());
        if (nullptr != bridgesProperty) {
            mBridgeNames = CommonStrUtils::split(bridgesProperty, ",");
        }
        if (mBridgeNames.empty()) {
            mama_log (MAMA_LOG_LEVEL_ERROR,
                      "No bridges defined for this resource pool - cannot create");
            throw MamaStatus (MAMA_STATUS_INVALID_ARG);
        }

        // Default channel should be INT_MAX so we can easily detect it
        mRegexChannelFilter.setDefaultChannel (INT_MAX);

        // Gather in default values from properties
        const char* defaultTransport = properties_GetPropertyValueUsingFormatString (
            mamaInternal_getProperties(),
            nullptr,
            "mama.resource_pool.%s.default_transport",
            mName.c_str());
        if (defaultTransport)
        {
            mDefaultTransportName = defaultTransport;
        }
        const char* defaultSource = properties_GetPropertyValueUsingFormatString (
            mamaInternal_getProperties(),
            nullptr,
            "mama.resource_pool.%s.default_source",
            mName.c_str());
        if (defaultSource) {
            mDefaultSourceName = defaultSource;
        }

        for (const auto& bridgeName : mBridgeNames) {
            // Separate status since bridge load failing is non-fatal (other bridges
            // may succeed)
            mama_log (MAMA_LOG_LEVEL_FINE,
                      "MAMA Resource Pool found bridge %s - will attempt to load",
                      bridgeName.c_str());

            auto bridge = Mama::loadBridge (bridgeName.c_str());
            if (nullptr != bridge) {
                mNumQueues = (int) mamaImpl_getParameterAsLong (
                    MAMA_RESOURCE_POOL_DEFAULT_QUEUE_COUNT,
                    1,
                    UINT8_MAX,
                    "mama.resource_pool.%s.queues",
                    mName.c_str());

                mPoolBridgeImpls.emplace(
                    bridgeName,
                    std::make_shared<MamaResourcePoolBridgeImpl>(
                        bridge,
                        mNumQueues,
                        mName));

                for (int queueIdx = 0; queueIdx < mNumQueues; ++queueIdx) {
                    const char* regexStr = properties_GetPropertyValueUsingFormatString (
                        mamaInternal_getProperties(),
                        nullptr,
                        "mama.resource_pool.%s.queue_%d.regex",
                        mName.c_str(),
                        queueIdx);
                    if (nullptr != regexStr) {
                        mRegexChannelFilter.addRegex (regexStr, queueIdx);
                    }
                }
            } else {
                mama_log(
                    MAMA_LOG_LEVEL_WARN,
                    "MAMA Resource Pool bridge %s failed to load - will not be "
                    "available for use in this instance",
                    bridgeName.c_str());
            }
        }

        Mama::open();
        mIsMamaOpened = true;

        // Set default values for options
        mDefaultOptions.mSubscriptionType = MAMA_SUBSC_TYPE_NORMAL;
        mDefaultOptions.mDebugLevel = MAMA_LOG_LEVEL_OFF;
        mDefaultOptions.mRetries = MAMA_DEFAULT_RETRIES;
        mDefaultOptions.mTimeout = MAMA_DEFAULT_TIMEOUT;
        mDefaultOptions.mRequiresInitial = MAMA_BOOL_TRUE;

        // Apply any default options from config
        mamaResourcePoolImpl_applyOptionsFromConfiguration (mName.c_str(),
                                                            &mDefaultOptions);
    }

    ~MamaResourcePoolImpl() {
        // Stop dispatch on all the queues to prevent further callbacks
        for (const auto& bridgeImpl : mPoolBridgeImpls) {
            bridgeImpl.second->mQueueGroup.stopDispatch();
        }

        // Destroy and deallocate all subscriptions
        for (auto &subscription : mSubscriptions)
        {
            if (subscription->mSubscription != nullptr) {
                subscription->mSubscription->destroy();
                delete subscription->mSubscription;
            }
        }
        mSubscriptions.clear();

        // Destroy all sources
        for (const auto& sourceImpl : mPoolSourceImpls) {
            delete sourceImpl.second->mSource;
        }
        mPoolSourceImpls.clear();

        // Destroy all queues
        for (const auto& bridgeImpl : mPoolBridgeImpls) {
            bridgeImpl.second->mQueueGroup.destroyWait();
        }

        // Destroy all transports
        for (const auto& transportImpl : mPoolTransportImpls) {
            delete transportImpl.second->mTransport;
        }

        mPoolTransportImpls.clear();

        // Clear pool bridge impl calling destructors for inline members
        mPoolBridgeImpls.clear();

        // Free up MAMA Resource Pool resources
        if (nullptr != mAprPool)
        {
            apr_pool_destroy (mAprPool);
            apr_terminate ();
        }

        // Close out MAMA if we ever opened it
        if (mIsMamaOpened) {
            Mama::close();
        }
    }

    std::string findBridgeNameByTransportName (const char* name) {
        for (const auto& bridgeName : mBridgeNames) {
            std::stringstream ss;
            ss << "mama." << bridgeName << ".transport." << name << ".";
            auto prefix = ss.str();
            for (const auto& entry : Mama::getProperties()) {
                if (entry.first.substr(0, prefix.length()) == prefix) {
                    return bridgeName;
                }
            }
        }
        mama_log (MAMA_LOG_LEVEL_WARN,
                  "Could not find a bridge for transport %s", name);
        throw MamaStatus(MAMA_STATUS_NOT_FOUND);
    }

    std::shared_ptr<MamaResourcePoolSourceImpl>
    findOrCreateSource (
        const char* name,
        const std::shared_ptr<MamaResourcePoolTransportImpl>& mrpTransportImpl)
    {
        auto mrpSourceImplIt = mPoolSourceImpls.find(name);
        if (mrpSourceImplIt != mPoolSourceImpls.end()) {
            return mrpSourceImplIt->second;
        }

        // There is no transport here - we need to create it
        auto* source = new MamaSource();
        source->setTransport (mrpTransportImpl->mTransport);
        source->setPublisherSourceName (name);

        auto mrpSourceImpl = std::make_shared<MamaResourcePoolSourceImpl>();
        mrpSourceImpl->mSource = source;

        // Add it to the cache
        mPoolSourceImpls.insert(std::make_pair<>(name, mrpSourceImpl));

        return mrpSourceImpl;
    }

    std::shared_ptr<MamaResourcePoolTransportImpl>
    findOrCreateTransport(const char* name)
    {
        // Firstly, look up the transport by name, it could already be defined
        auto existing = mPoolTransportImpls.find(name);
        if (existing != mPoolTransportImpls.end()) {
            return existing->second;
        }

        auto bridgeName = findBridgeNameByTransportName (name);
        auto mrpBridgeImpl = mPoolBridgeImpls.find (bridgeName);
        if (mrpBridgeImpl == mPoolBridgeImpls.end()) {
            mama_log (MAMA_LOG_LEVEL_WARN,
                      "Could not find bridge %s", bridgeName.c_str());
            throw MamaStatus(MAMA_STATUS_NOT_FOUND);
        }

        // There is no transport here - we need to create it
        auto transport = new MamaTransport();
        transport->create (name, mrpBridgeImpl->second->mBridge);

        auto mrpTransportImpl = std::make_shared<MamaResourcePoolTransportImpl>();
        mrpTransportImpl->mTransport = transport;
        mrpTransportImpl->mMrpBridge = mrpBridgeImpl->second;

        mPoolTransportImpls.insert(std::make_pair<>(name, mrpTransportImpl));

        return mrpTransportImpl;
    }

    MamaSubscription* createSubscriptionFromComponents (
        const char*                     transportName,
        const char*                     sourceName,
        const char*                     topicName,
        MamaSubscriptionCallback*       callbacks,
        void*                           closure) {
        std::lock_guard<std::recursive_mutex> lock(mMutex);
        auto mrpTransportImpl = findOrCreateTransport (transportName);
        auto mrpSubscriptionImpl = createSubscriptionFromComponents (
            mrpTransportImpl,
            sourceName,
            topicName,
            callbacks,
            closure,
            nullptr);
        return mrpSubscriptionImpl->mSubscription;
    }

    MamaSubscription* createSubscriptionFromTopicWithSource (
        const char*                     sourceName,
        const char*                     topicName,
        MamaSubscriptionCallback*       callbacks,
        void*                           closure) {
        // Get the default subscribing transport name from the mama.properties for
        // this resource pool
        const char* transportName = properties_GetPropertyValueUsingFormatString (
            mamaInternal_getProperties(),
            nullptr,
            "mama.resource_pool.%s.default_transport_sub",
            mName.c_str());
        if (nullptr == transportName) {
            mama_log (MAMA_LOG_LEVEL_WARN,
                      "Could not create subscription for %s.%s - no default_transport_sub "
                      "transport defined for resource pool %s",
                      sourceName == nullptr ? "" : sourceName,
                      topicName,
                      mName.c_str());
            throw MamaStatus(MAMA_STATUS_INVALID_ARG);
        }
        std::lock_guard<std::recursive_mutex> lock(mMutex);
        return createSubscriptionFromComponents (
            transportName,
            sourceName,
            topicName,
            callbacks,
            closure
        );
    }

    MamaSubscription* createSubscriptionFromTopic (
        const char*                     topicName,
        MamaSubscriptionCallback*       callbacks,
        void*                           closure)
    {
        // Get the default subscribing transport name from the mama.properties for
        // this resource pool
        const char* sourceName = properties_GetPropertyValueUsingFormatString (
            mamaInternal_getProperties(),
            nullptr,
            "mama.resource_pool.%s.default_source_sub",
            mName.c_str());
        if (nullptr == sourceName) {
            mama_log (MAMA_LOG_LEVEL_WARN,
                      "Could not create subscription for %s - no default_source_sub "
                      "sub source defined for resource pool %s",
                      topicName == nullptr ? "" : topicName,
                      mName.c_str());
            throw MamaStatus(MAMA_STATUS_INVALID_ARG);
        }

        // Get the default subscribing transport name from the mama.properties for
        // this resource pool
        const char* poolDefaultTransportName = properties_GetPropertyValueUsingFormatString (
            mamaInternal_getProperties(),
            nullptr,
            "mama.resource_pool.%s.default_transport_sub",
            mName.c_str());

        // Get the default subscribing transport name from the mama.properties for
        // this source
        const char* sourceDefaultTransportName = properties_GetPropertyValueUsingFormatString (
            mamaInternal_getProperties(),
            nullptr,
            "mama.source.%s.transport_sub",
            sourceName);
        if (nullptr == poolDefaultTransportName && nullptr == sourceDefaultTransportName) {
            mama_log (MAMA_LOG_LEVEL_WARN,
                      "Could not create subscription for %s - no default "
                      "source or resource pool default transport defined for source "
                      "%s in resource pool %s",
                      topicName,
                      sourceName,
                      mName.c_str());
            throw MamaStatus(MAMA_STATUS_INVALID_ARG);
        }

        std::lock_guard<std::recursive_mutex> lock(mMutex);
        return createSubscriptionFromComponents (
            // Prefer source default if available, then use the pool's
            sourceDefaultTransportName != nullptr ? sourceDefaultTransportName : poolDefaultTransportName,
            sourceName,
            topicName,
            callbacks,
            closure
        );
    }

    MamaTransport* createTransportFromName (const char* name) {
        std::lock_guard<std::recursive_mutex> lock(mMutex);
        return findOrCreateTransport (name)->mTransport;
    }

    void destroySubscription (MamaSubscription* subscription) {
        std::lock_guard<std::recursive_mutex> lock(mMutex);
        auto it = mSubscriptions.begin();
        for (; it != mSubscriptions.end(); ++it) {
            if ((*it)->mSubscription == subscription) {
                break;
            }
        }
        if (it == mSubscriptions.end()) {
            throw MamaStatus(MAMA_STATUS_NOT_FOUND);
        }

        auto queue = (*it)->mSubscription->getQueue();
        queue->enqueueEvent (mDestroySubscriptionHandler, (*it)->mSubscription);

        // Remove our reference to it either way
        mSubscriptions.erase (it);
    }

    std::shared_ptr<MamaResourcePoolSubscriptionImpl>
    createSubscriptionFromComponents (
        const std::shared_ptr<MamaResourcePoolTransportImpl>& mrpTransportImpl,
        const char* sourceName,
        const char* topicName,
        MamaSubscriptionCallback* callbacks,
        void* closure,
        mamaResourcePoolOptionsImpl* mrpOptionsImpl
    ) {
        mamaResourcePoolOptionsImpl subscriptionOptions;
        if (nullptr == mrpOptionsImpl) {
            subscriptionOptions = mDefaultOptions;
        } else {
            subscriptionOptions = *mrpOptionsImpl;
        }
        auto* newSubscription = new MamaSubscription();
        newSubscription->setTimeout (subscriptionOptions.mTimeout);
        newSubscription->setRetries (subscriptionOptions.mRetries);
        newSubscription->setSubscriptionType (subscriptionOptions.mSubscriptionType);
        newSubscription->setRequiresInitial (subscriptionOptions.mRequiresInitial);
        newSubscription->setDebugLevel (subscriptionOptions.mDebugLevel);

        char fqTopic[MAMA_MAX_SYMBOL_LEN];
        if (nullptr == sourceName) {
            strcpy (fqTopic, topicName);
        } else {
            snprintf (fqTopic, sizeof(fqTopic), "%s/%s", sourceName, topicName);
        }

        MamaQueue* queue;
        int queueIndex = mRegexChannelFilter.getChannel (fqTopic);
        if (queueIndex == INT_MAX) {
            // If no regex match defined / found, round robin
            queue = mrpTransportImpl->mMrpBridge->mQueueGroup.getNextQueue();
        } else {
            queue = mrpTransportImpl->mMrpBridge->mQueueGroup.getQueueByIndex (queueIndex);
        }

        auto mrpSubscriptionImpl = std::make_shared<MamaResourcePoolSubscriptionImpl>();
        if (sourceName && topicName) {
            // Find or create the source in question
            auto mrpSourceImpl = findOrCreateSource (sourceName,
                                                     mrpTransportImpl);
            mrpSubscriptionImpl->mMrpSource = mrpSourceImpl;
            newSubscription->setup (
                queue,
                callbacks,
                mrpSourceImpl->mSource,
                topicName,
                closure
            );
        } else {
            mrpSubscriptionImpl->mMrpSource = nullptr;
            newSubscription->createBasic (
                mrpTransportImpl->mTransport,
                queue,
                callbacks,
                topicName,
                closure
            );
        }

        mrpSubscriptionImpl->mSubscription = newSubscription;

        mSubscriptions.insert (mrpSubscriptionImpl);

        return mrpSubscriptionImpl;
    }

    MamaSubscription* createSubscriptionFromUri (
            const char* uri,
            MamaSubscriptionCallback* callbacks,
            void* closure) {
        if (nullptr == uri) {
            throw MamaStatus(MAMA_STATUS_NULL_ARG);
        }

        if (0 == strlen (uri)) {
            throw MamaStatus(MAMA_STATUS_INVALID_ARG);
        }

        apr_uri_t parsedUri;
        apr_status_t aprStatus;
        aprStatus = apr_uri_parse (mAprPool, uri, &parsedUri);
        if (APR_SUCCESS != aprStatus) {
            mama_log (MAMA_LOG_LEVEL_WARN,
                      "Could not parse url %s - is it correctly formed?",
                      uri);
            throw MamaStatus(MAMA_STATUS_INVALID_ARG);
        }

        // The bridge is the scheme
        const char* bridgeName = parsedUri.scheme;
        if (nullptr == bridgeName) {
            mama_log (MAMA_LOG_LEVEL_WARN,
                      "Could not parse url %s - could not find a valid bridge.",
                      uri);
            throw MamaStatus(MAMA_STATUS_INVALID_ARG);
        }

        // The transport is the authority / hostname
        const char* transportName = parsedUri.hostname;
        if (nullptr == transportName) {
            mama_log (MAMA_LOG_LEVEL_WARN,
                      "Could not parse url %s - could not find a valid transport.",
                      uri);
            throw MamaStatus(MAMA_STATUS_INVALID_ARG);
        }

        // Find the bridge for this
        std::lock_guard<std::recursive_mutex> lock(mMutex);
        auto mrpBridgeImplIt = mPoolBridgeImpls.find (bridgeName);
        if (mrpBridgeImplIt == mPoolBridgeImpls.end()) {
            mama_log (MAMA_LOG_LEVEL_WARN,
                      "Could not find bridge using scheme '%s'.",
                      bridgeName);
            throw MamaStatus(MAMA_STATUS_NOT_FOUND);
        }
        auto mrpBridgeImpl = mrpBridgeImplIt->second;

        // Find or create the transport in question
        auto mrpTransportImpl = findOrCreateTransport (transportName);
        mrpTransportImpl->mMrpBridge = mrpBridgeImpl;

        // Take a copy of the default options for the pool
        mamaResourcePoolOptionsImpl subscriptionOptions = mDefaultOptions;

        // Apply the overrides from the query string
        mamaResourcePoolImpl_applyOptionsFromQueryString (parsedUri.query,
                                                          &subscriptionOptions);

        // All the paths begin with a "/" so this will simply skip past that
        size_t originalPathLength = strlen (parsedUri.path);
        char*  sourceName         = strtok (parsedUri.path, "/");
        char*  topicName          = nullptr;
        // Fully qualified name is used for regex matching so source is considered
        if (nullptr == sourceName) {
            // There is no source present - this is a topic
            topicName     = sourceName;
            sourceName    = nullptr;

        } else if (originalPathLength > strlen (sourceName) + 1) {
            // If there was more after the trailing slash, read in the topic
            topicName = sourceName + strlen(sourceName) + 1;
        }
        auto mrpSubscriptionImpl = createSubscriptionFromComponents (
            mrpTransportImpl,
            sourceName,
            topicName,
            callbacks,
            closure,
            &subscriptionOptions
        );

        return mrpSubscriptionImpl->mSubscription;
    }

    // Name of the pool
    std::string mName;

    // Number of queues in use for the pool
    int mNumQueues;

    // Implementation structures for the pool managed entities
    std::unordered_map<std::string, std::shared_ptr<MamaResourcePoolBridgeImpl>> mPoolBridgeImpls;    // Indexed by bridge name, e.g. "qpid"
    std::unordered_map<std::string, std::shared_ptr<MamaResourcePoolTransportImpl>> mPoolTransportImpls; // Indexed by transport name, e.g. "sub"
    std::unordered_map<std::string, std::shared_ptr<MamaResourcePoolSourceImpl>> mPoolSourceImpls;    // Indexed by source name, e.g. "OM"

    // Default implementations to use
    std::string mDefaultTransportName;
    std::string mDefaultSourceName;

    // Lock for accessing internal pool structures
    std::recursive_mutex mMutex;

    // APR memory pool for cleanup
    apr_pool_t* mAprPool;

    // Default options for use when creating entities within this pool
    mamaResourcePoolOptionsImpl mDefaultOptions;

    // List of all created subscriptions for cleanup
    std::set<std::shared_ptr<MamaResourcePoolSubscriptionImpl>> mSubscriptions;

    // Cache a list of bridges which this resource pool knows about
    std::vector<std::string> mBridgeNames;

    // Regex queue / channel filter
    MamaRegexChannelFilter mRegexChannelFilter;
    bool mIsMamaOpened;

    // Subscription cleanup callback instance
    DestroySubscriptionHandler mDestroySubscriptionHandler;
};

}

typedef struct MamaSourceSubscriptionIteratorClosure_
{
    MamaSource*                               mSource;
    void*                                     mData;
} MamaSourceSubscriptionIteratorClosure;


MamaResourcePool::MamaResourcePool (const char* name)
{
    if (nullptr == name)
    {
        throw MamaStatus(MAMA_STATUS_NULL_ARG);
    }

    std::string poolName = name;
    mPimpl = new MamaResourcePoolImpl(poolName);
}

MamaResourcePool::~MamaResourcePool ( ) {
    delete mPimpl;
}

MamaSubscription* MamaResourcePool::createSubscriptionFromUri (
    const char*                     uri,
    MamaSubscriptionCallback*       callbacks,
    void*                           closure)
{
    if (nullptr == uri || nullptr == callbacks)
    {
        throw MamaStatus(MAMA_STATUS_NULL_ARG);
    }
    return mPimpl->createSubscriptionFromUri (uri, callbacks, closure);
}

MamaSubscription* MamaResourcePool::createSubscriptionFromComponents (
    const char*                     transportName,
    const char*                     sourceName,
    const char*                     topicName,
    MamaSubscriptionCallback*       callbacks,
    void*                           closure)
{
    if (nullptr == transportName || nullptr == sourceName ||
        nullptr == topicName || nullptr == callbacks)
    {
        throw MamaStatus(MAMA_STATUS_NULL_ARG);
    }
    return mPimpl->createSubscriptionFromComponents (transportName,
                                                     sourceName,
                                                     topicName,
                                                     callbacks,
                                                     closure);
}

MamaSubscription* MamaResourcePool::createSubscriptionFromTopicWithSource (
    const char*                     sourceName,
    const char*                     topicName,
    MamaSubscriptionCallback*       callbacks,
    void*                           closure)
{
    if (nullptr == sourceName || nullptr == topicName || nullptr == callbacks)
    {
        throw MamaStatus(MAMA_STATUS_NULL_ARG);
    }
    return mPimpl->createSubscriptionFromTopicWithSource (sourceName,
                                                          topicName,
                                                          callbacks,
                                                          closure);
}

MamaSubscription* MamaResourcePool::createSubscriptionFromTopic (
    const char*                     topicName,
    MamaSubscriptionCallback*       callbacks,
    void*                           closure)
{
    if (nullptr == topicName || nullptr == callbacks)
    {
        throw MamaStatus(MAMA_STATUS_NULL_ARG);
    }
    return mPimpl->createSubscriptionFromTopic (topicName,
                                                callbacks,
                                                closure);
}

MamaTransport* MamaResourcePool::createTransportFromName (
    const char*    transportName)
{
    if (nullptr == transportName)
    {
        throw MamaStatus(MAMA_STATUS_NULL_ARG);
    }
    return mPimpl->createTransportFromName (transportName);
}

void MamaResourcePool::destroySubscription (
    MamaSubscription* subscription)
{
    if (nullptr == subscription)
    {
        throw MamaStatus(MAMA_STATUS_NULL_ARG);
    }
    return mPimpl->destroySubscription (subscription);
}
