package com.wombat.mama;

import java.net.MalformedURLException;
import java.net.URI;
import java.net.URISyntaxException;
import java.util.*;
import java.util.logging.Level;

public class MamaResourcePool
{
    private static final int MAMA_RESOURCE_POOL_DEFAULT_QUEUE_COUNT = 4;

    // Number of queues in use for the pool
    int mNumQueues = MAMA_RESOURCE_POOL_DEFAULT_QUEUE_COUNT;

    // Name of the pool
    String mName;

    // Implementation structures for the pool managed entities
    private final Map<String, MamaResourcePoolBridgeImpl> mPoolBridgeImpls = new HashMap<>();    // Indexed by bridge name, e.g. "qpid"
    private final Map<String, MamaResourcePoolTransportImpl> mPoolTransportImpls = new HashMap<>(); // Indexed by transport name, e.g. "sub"
    private final Map<String, MamaResourcePoolSourceImpl> mPoolSourceImpls = new HashMap<>();    // Indexed by source name, e.g. "OM"

    // Default implementations to use
    String mDefaultTransportName;
    String mDefaultSourceName;

    // List of all created subscriptions for cleanup
    private final Set<MamaResourcePoolSubscriptionImpl> mSubscriptions = new HashSet<>();

    // Cache a list of bridges which this resource pool knows about
    private final List<String> mBridgeNames;

    // Regex queue / channel filter
    private final MamaRegexChannelFilter mRegexChannelFilter = new MamaRegexChannelFilter();
    private boolean mIsMamaOpened;

    // Subscription cleanup callback instance
    private final DestroySubscriptionHandler mDestroySubscriptionHandler = new DestroySubscriptionHandler();

    private final MamaResourcePoolOptionsImpl mDefaultOptions = new MamaResourcePoolOptionsImpl();

    private class MamaResourcePoolTransportImpl {
        MamaResourcePoolBridgeImpl mMrpBridge;
        MamaTransport mTransport;
    };

    private class MamaResourcePoolSourceImpl {
        MamaSource mSource;
    };

    private class MamaResourcePoolSubscriptionImpl {
        MamaSubscription mSubscription;
        MamaResourcePoolSourceImpl mMrpSource;
    };

    private class DestroySubscriptionHandler implements MamaQueueEventCallback {
        @Override
        public void onEvent (MamaQueue queue, Object closure) {
            MamaSubscription subscription = (MamaSubscription) closure;
            subscription.destroy();
        }
    }

    private class MamaResourcePoolOptionsImpl {
        double mTimeout;
        int mRetries;
        MamaSubscriptionType mSubscriptionType;
        boolean mRequiresInitial;
        MamaLogLevel mDebugLevel;
    }

    private class MamaResourcePoolBridgeImpl {
        private final String mPoolName;
        private final MamaQueueGroup mQueueGroup;
        private final MamaBridge mBridge;

        MamaResourcePoolBridgeImpl(MamaBridge bridge, int numQueues, String poolName) {
            mPoolName = poolName;
            mBridge = bridge;
            mQueueGroup = new MamaQueueGroup(numQueues, mBridge);
            String queuePrefix = Mama.getProperty(String.format("mama.resource_pool.%s.thread_name_prefix", mPoolName));
            if (null != queuePrefix) {
                // Do the rounds with each queue in the queue group and name each thread
                for (int i = 0; i < numQueues; i++) {
                    MamaQueue queue = mQueueGroup.getNextQueue();
                    String queueName = String.format("%s_%d", queuePrefix, i);
                    queue.setQueueName(queueName);
                }
            }
        }
    }

    /**
     * Creates a MAMA resource pool to manage subscription lifecycles on behalf of
     * the application calling it.
     *
     * @param name         The name of the MAMA resource pool in configuration
     */
    public MamaResourcePool (final String name) throws MamaException
    {
        mIsMamaOpened = false;
        if (null == name)
        {
            throw new MamaException("Null string passed to MamaResourcePool");
        }
        mName = name;
        String bridgesProperty = Mama.getProperty (String.format("mama.resource_pool.%s.bridges", mName), "qpid");
        mBridgeNames = Arrays.asList(bridgesProperty.split(","));
        if (mBridgeNames.isEmpty()) {
            throw new MamaException("No bridges defined for this resource pool - cannot create");
        }
        mRegexChannelFilter.setDefaultChannel(Integer.MAX_VALUE);
        mDefaultTransportName = Mama.getProperty (String.format("mama.resource_pool.%s.default_transport", mName));
        mDefaultSourceName = Mama.getProperty (String.format("mama.resource_pool.%s.default_source", mName));
        for (String bridgeName : mBridgeNames) {
            Mama.log(MamaLogLevel.FINE, String.format("MAMA Resource Pool found bridge %s - will attempt to load", bridgeName));
            MamaBridge bridge = null;
            try {
                bridge = Mama.loadBridge(bridgeName);
            } catch (MamaException e) {
                Mama.log(MamaLogLevel.ERROR,
                        String.format("MAMA Resource Pool couldn't load bridge %s: %s", bridgeName, e.getMessage()));
            }
            if (null != bridge) {
                String numQueuesProperty = Mama.getProperty(String.format("mama.resource_pool.%s.queues", mName));
                if (null != numQueuesProperty) {
                    mNumQueues = Integer.parseInt(numQueuesProperty);
                } else {
                    mNumQueues = MAMA_RESOURCE_POOL_DEFAULT_QUEUE_COUNT;
                }

                mPoolBridgeImpls.put(bridgeName, new MamaResourcePoolBridgeImpl(bridge, mNumQueues, mName));
                for (int queueIdx = 0; queueIdx < mNumQueues; ++queueIdx) {
                    String regexStr = Mama.getProperty(String.format("mama.resource_pool.%s.queue_%d.regex", mName, queueIdx));
                    if (null != regexStr) {
                        mRegexChannelFilter.addRegex(regexStr, queueIdx);
                    }
                }
            } else {
                Mama.log(MamaLogLevel.WARN,
                        String.format("MAMA Resource Pool bridge %s failed to load - will not be available for use in this instance", bridgeName));
            }
        }
        Mama.open();
        mIsMamaOpened = true;

        mDefaultOptions.mSubscriptionType = MamaSubscriptionType.NORMAL;
        mDefaultOptions.mDebugLevel = MamaLogLevel.OFF;
        mDefaultOptions.mRetries = 2;
        mDefaultOptions.mTimeout = 30;
        mDefaultOptions.mRequiresInitial = true;

        applyOptionsFromConfiguration(mDefaultOptions);
    }

    private void applyOption(MamaResourcePoolOptionsImpl options, String key, String value) {
        Mama.log(MamaLogLevel.FINEST, String.format("Found pool option [%s] = '%s'", key, value));
        switch (key) {
            case "timeout":
                options.mTimeout = Integer.parseInt(value);
                break;
            case "retries":
                options.mRetries = Integer.parseInt(value);
                break;
            case "subscription_type":
                switch (value) {
                    case "NORMAL":
                        options.mSubscriptionType = MamaSubscriptionType.NORMAL;
                        break;
                    case "GROUP":
                        options.mSubscriptionType = MamaSubscriptionType.GROUP;
                        break;
                    case "BOOK":
                        options.mSubscriptionType = MamaSubscriptionType.BOOK;
                        break;
                    case "BASIC":
                        options.mSubscriptionType = MamaSubscriptionType.BASIC;
                        break;
                    case "DICTIONARY":
                        options.mSubscriptionType = MamaSubscriptionType.DICTIONARY;
                        break;
                    case "SYMBOL_LIST":
                        options.mSubscriptionType = MamaSubscriptionType.SYMBOL_LIST;
                        break;
                    case "SYMBOL_LIST_NORMAL":
                        options.mSubscriptionType = MamaSubscriptionType.SYMBOL_LIST_NORMAL;
                        break;
                    case "SYMBOL_LIST_GROUP":
                        options.mSubscriptionType = MamaSubscriptionType.SYMBOL_LIST_GROUP;
                        break;
                    case "SYMBOL_LIST_BOOK":
                        options.mSubscriptionType = MamaSubscriptionType.SYMBOL_LIST_BOOK;
                        break;
                    default:
                        Mama.log(MamaLogLevel.WARN, String.format(
                                "Found unknown subscription_type: %s (must be NORMAL, GROUP, BOOK, BASIC, "
                                        + "DICTIONARY, SYMBOL_LIST, SYMBOL_LIST_NORMAL, SYMBOL_LIST_GROUP or SYMBOL_LIST_BOOK",
                                value));
                        break;
                }
                break;
            case "requires_initial":
                options.mRequiresInitial = Boolean.parseBoolean(value);
                break;
            case "debug_level":
                options.mDebugLevel = MamaLogLevel.getMamaLogLevel(Level.parse(value));
                break;
            default:
                Mama.log(MamaLogLevel.WARN, String.format(
                        "Found unrecognized mamaResourcePool option '%s'",
                        key));
                break;
        }
    }

    private void applyOptionsFromConfiguration(MamaResourcePoolOptionsImpl options) {
        // Use mName
        String prefix = String.format("mama.resource_pool.%s.options.", mName);
        for (Map.Entry<String, String> entry : Mama.getProperties().entrySet()) {
            if (entry.getKey().startsWith(prefix)) {
                applyOption(options, entry.getKey().substring(prefix.length()), entry.getValue());
            }
        }
    }

    private void applyOptionsFromQueryString(String query, MamaResourcePoolOptionsImpl options) {
        if (null == query) {
            return;
        }

        // Prune opening question mark for this URI if it exists
        if (query.startsWith("?"))
        {
            query = query.substring(1);
        }

        for (String pair : query.split("&")) {
            int index = pair.indexOf("=");
            if (index > 0 && query.length() > index + 1) {
                String key = pair.substring(0, index);
                String value = pair.substring(index + 1);
                applyOption(options, key, value);
            }
        }
    }

    private MamaResourcePoolSourceImpl findOrCreateSource (String name, MamaResourcePoolTransportImpl mrpTransportImpl) {
        MamaResourcePoolSourceImpl mrpSourceImpl = mPoolSourceImpls.get(name);
        if (null == mrpSourceImpl) {
            // There is no transport here - we need to create it
            MamaSource source = new MamaSource();
            source.setTransport(mrpTransportImpl.mTransport);
            source.setSymbolNamespace(name);

            mrpSourceImpl = new MamaResourcePoolSourceImpl();
            mrpSourceImpl.mSource = source;

            // Add it to the cache
            mPoolSourceImpls.put(name, mrpSourceImpl);
        }
        return mrpSourceImpl;
    }

    private MamaResourcePoolTransportImpl findOrCreateTransport(String name) {
        MamaResourcePoolTransportImpl mrpTransportImpl = mPoolTransportImpls.get(name);
        if (null == mrpTransportImpl) {
            String bridgeName = findBridgeNameByTransportName(name);
            MamaResourcePoolBridgeImpl mrpBridgeImpl = mPoolBridgeImpls.get(bridgeName);
            if (mrpBridgeImpl == null) {
                Mama.log(MamaLogLevel.WARN,
                        String.format("Could not find bridge %s", bridgeName));
                throw new MamaException("Could not find bridge");
            }

            // There is no transport here - we need to create it
            MamaTransport transport = new MamaTransport();
            transport.create(name, mrpBridgeImpl.mBridge);

            mrpTransportImpl = new MamaResourcePoolTransportImpl();
            mrpTransportImpl.mTransport = transport;
            mrpTransportImpl.mMrpBridge = mrpBridgeImpl;

            mPoolTransportImpls.put(name, mrpTransportImpl);
        }
        return mrpTransportImpl;
    }

    private MamaResourcePoolSubscriptionImpl createSubscriptionFromComponents(
            MamaResourcePoolTransportImpl mrpTransportImpl,
            String sourceName,
            String topicName,
            MamaSubscriptionCallback callbacks,
            Object closure,
            MamaResourcePoolOptionsImpl mrpOptionsImpl) {
        MamaResourcePoolOptionsImpl subscriptionOptions;
        if (null == mrpOptionsImpl) {
            subscriptionOptions = mDefaultOptions;
        } else {
            subscriptionOptions = mrpOptionsImpl;
        }
        MamaSubscription newSubscription = new MamaSubscription();
        newSubscription.setTimeout(subscriptionOptions.mTimeout);
        newSubscription.setRetries (subscriptionOptions.mRetries);
        newSubscription.setSubscriptionType (subscriptionOptions.mSubscriptionType);
        newSubscription.setRequiresInitial (subscriptionOptions.mRequiresInitial);

        String fqTopic;
        if (null == sourceName) {
            fqTopic = topicName;
        } else {
            fqTopic = String.format("%s/%s", sourceName, topicName);
        }

        MamaQueue queue;
        int queueIndex = mRegexChannelFilter.getChannel(fqTopic);
        if (queueIndex == Integer.MAX_VALUE) {
            queue = mrpTransportImpl.mMrpBridge.mQueueGroup.getNextQueue();
        } else {
            queue = mrpTransportImpl.mMrpBridge.mQueueGroup.getQueueByIndex(queueIndex);
        }

        Mama.log(MamaLogLevel.FINE, String.format("MRP Creating subscription for %s/%s", sourceName, topicName));
        MamaResourcePoolSubscriptionImpl mrpSubscriptionImpl = new MamaResourcePoolSubscriptionImpl();
        MamaResourcePoolSourceImpl mrpSourceImpl = findOrCreateSource(sourceName, mrpTransportImpl);
        mrpSubscriptionImpl.mMrpSource = mrpSourceImpl;
        newSubscription.setupSubscription(
                callbacks,
                queue,
                mrpSourceImpl.mSource,
                topicName,
                closure
        );
        mrpSubscriptionImpl.mSubscription = newSubscription;
        mSubscriptions.add(mrpSubscriptionImpl);
        return mrpSubscriptionImpl;
    }

    /**
     * Destroys a MAMA resource pool and all subscriptions, transports etc that
     * were created using it.
     */
    public synchronized void destroy()
    {
        // Stop dispatch on all the queues to prevent further callbacks
        for (Map.Entry<String, MamaResourcePoolBridgeImpl> entry: mPoolBridgeImpls.entrySet()) {
            entry.getValue().mQueueGroup.stopDispatch();
        }

        for (MamaResourcePoolSubscriptionImpl mrpSubscriptionImpl : mSubscriptions) {
            if (mrpSubscriptionImpl.mSubscription != null) {
                mrpSubscriptionImpl.mSubscription.destroy();
            }
        }
        mSubscriptions.clear();

        // Destroy all sources
        for (Map.Entry<String, MamaResourcePoolSourceImpl> entry: mPoolSourceImpls.entrySet()) {
            entry.getValue().mSource.destroy();
        }
        mPoolSourceImpls.clear();

        // Destroy all queues
        for (Map.Entry<String, MamaResourcePoolBridgeImpl> entry: mPoolBridgeImpls.entrySet()) {
            entry.getValue().mQueueGroup.destroyWait();
        }

        // Destroy all transports
        for (Map.Entry<String, MamaResourcePoolTransportImpl> entry: mPoolTransportImpls.entrySet()) {
            entry.getValue().mTransport.destroy();
        }
        mPoolTransportImpls.clear();

        // Clear down bridge list too
        mPoolBridgeImpls.clear();

        // Close out MAMA if we ever got as far as opening it
        if (mIsMamaOpened) {
            Mama.close();
        }
    }

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
    public synchronized MamaSubscription createSubscriptionFromUri (
            final String uri,
            final MamaSubscriptionCallback callbacks,
            final Object closure) {
        if (null == uri || null == callbacks)
        {
            throw new MamaException("Null arguments passed to createSubscriptionFromUri");
        }
        if (uri.length() == 0) {
            throw new MamaException("Zero length uri passed to createSubscriptionFromUri");
        }
        URI parsedUri = URI.create(uri);
        String bridgeName = parsedUri.getScheme();
        if (null == bridgeName) {
            throw new MamaException("Could not find valid bridge / scheme in createSubscriptionFromUri");
        }

        String transportName = parsedUri.getHost();
        if (null == transportName) {
            throw new MamaException("Could not find valid transport / host in createSubscriptionFromUri");
        }

        MamaResourcePoolBridgeImpl mrpBridgeImpl = mPoolBridgeImpls.get(bridgeName);
        if (null == mrpBridgeImpl) {
            throw new MamaException("Could not find bridge loaded / configured for pool in createSubscriptionFromUri");
        }

        String parsedUriPath = parsedUri.getPath();
        String[] pathComponents = parsedUriPath.split("/");
        if (pathComponents.length < 2) {
            throw new MamaException("Could not find valid path in createSubscriptionFromUri");
        }


        // Find or create the transport in question
        MamaResourcePoolTransportImpl mrpTransportImpl = findOrCreateTransport(transportName);
        mrpTransportImpl.mMrpBridge = mrpBridgeImpl;

        // Take a copy of the default options for the pool
        MamaResourcePoolOptionsImpl subscriptionOptions = mDefaultOptions;

        // Apply the overrides from the query string
        applyOptionsFromQueryString(parsedUri.getRawQuery(), subscriptionOptions);

        String topicName;
        String sourceName;
        if (pathComponents.length == 2) {
            topicName     = pathComponents[1];
            sourceName    = null;
        } else {
            sourceName = pathComponents[1];
            topicName = pathComponents[2];
        }

        MamaResourcePoolSubscriptionImpl mrpSubscriptionImpl = createSubscriptionFromComponents (
                mrpTransportImpl,
                sourceName,
                topicName,
                callbacks,
                closure,
                subscriptionOptions
        );

        return mrpSubscriptionImpl.mSubscription;
    }

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
    public synchronized MamaSubscription createSubscriptionFromComponents (
            final String transportName,
            final String sourceName,
            final String topicName,
            final MamaSubscriptionCallback callbacks,
            final Object closure)
    {
        if (null == transportName || null == sourceName ||
                null == topicName || null == callbacks)
        {
            throw new MamaException("Null arguments passed to createSubscriptionFromComponents");
        }
        MamaResourcePoolTransportImpl mrpTransportImpl = findOrCreateTransport(transportName);
        MamaResourcePoolSubscriptionImpl mrpSubscriptionImpl = createSubscriptionFromComponents(
                mrpTransportImpl,
                sourceName,
                topicName,
                callbacks,
                closure,
                null);
        return mrpSubscriptionImpl.mSubscription;
    }

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
    public synchronized MamaSubscription createSubscriptionFromTopicWithSource (
            final String sourceName,
            final String topicName,
            final MamaSubscriptionCallback callbacks,
            final Object closure)
    {
        if (null == sourceName || null == topicName || null == callbacks)
        {
            throw new MamaException("Null arguments passed to createSubscriptionFromTopicWithSource");
        }

        String transportName = Mama.getProperty(String.format("mama.resource_pool.%s.default_transport_sub", mName));
        if (null == transportName) {
            throw new MamaException(String.format(
                    "Could not create subscription for %s.%s - no default_transport_sub " +
                        "transport defined for resource pool %s",
                    sourceName,
                    topicName,
                    mName
            ));
        }

        return createSubscriptionFromComponents (
                transportName,
                sourceName,
                topicName,
                callbacks,
                closure
        );
    }

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
    public synchronized MamaSubscription createSubscriptionFromTopic (
            final String topicName,
            final MamaSubscriptionCallback callbacks,
            final Object closure)
    {
        if (null == topicName || null == callbacks)
        {
            throw new MamaException("Null arguments passed to createSubscriptionFromTopic");
        }
        String sourceName = Mama.getProperty(String.format("mama.resource_pool.%s.default_source_sub", mName));
        if (null == sourceName) {
            throw new MamaException(String.format(
                    "Could not create subscription for %s - no default_source_sub " +
                        "sub source defined for resource pool %s",
                    topicName, mName));
        }

        String poolDefaultTransportName = Mama.getProperty(String.format("mama.resource_pool.%s.default_transport_sub", mName));
        String sourceDefaultTransportName = Mama.getProperty(String.format("mama.source.%s.transport_sub", sourceName));

        if (null == poolDefaultTransportName && null == sourceDefaultTransportName) {
            throw new MamaException(String.format(
                    "Could not create subscription for %s - no default " +
                            "source or resource pool default transport defined for source " +
                            "%s in resource pool %s",
                    topicName, sourceName, mName));
        }
        return createSubscriptionFromComponents(
                sourceDefaultTransportName != null ? sourceDefaultTransportName : poolDefaultTransportName,
                sourceName,
                topicName,
                callbacks,
                closure
        );
    }

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
    public synchronized MamaTransport createTransportFromName (
            final String transportName)
    {
        if (null == transportName)
        {
            throw new MamaException("Null arguments passed to createTransportFromName");
        }
        return findOrCreateTransport(transportName).mTransport;
    }

    /**
     * Destroys the given subscription and removes it from the resource pool
     * asynchronously. There may be further callbacks after calling this function
     * since this event is enqueued on the subscription's thread, so for any closure
     * related shutdown code, cleanup should be performed in the subscription's
     * onDestroy callback.
     *
     * @param subscription  The subscription to destroy
     */
    public synchronized void destroySubscription (
            final MamaSubscription subscription)
    {
        if (null == subscription)
        {
            throw new MamaException("Null arguments passed to destroySubscription");
        }

        MamaResourcePoolSubscriptionImpl existingMrpSubscription = null;
        for (MamaResourcePoolSubscriptionImpl mrpSubscription : mSubscriptions) {
            if (mrpSubscription.mSubscription == subscription) {
                existingMrpSubscription = mrpSubscription;
                break;
            }
        }
        if (null == existingMrpSubscription) {
            throw new MamaException("MAMA subscription cannot be destroyed since it could not be found in pool");
        }

        MamaQueue queue = subscription.getQueue();
        queue.enqueueEvent(mDestroySubscriptionHandler, subscription);

        mSubscriptions.remove(existingMrpSubscription);
    }

    private String findBridgeNameByTransportName (String name) {
        for (String bridgeName : mBridgeNames) {
           String prefix = String.format("mama.%s.transport.%s.", bridgeName, name);
            for (Map.Entry<String, String> entry : Mama.getProperties().entrySet()) {
                if (entry.getKey().startsWith(prefix)) {
                    return bridgeName;
                }
            }
            throw new MamaException(String.format("Could not find a bridge for transport %s", name));
        }
        return null;
    }
}
