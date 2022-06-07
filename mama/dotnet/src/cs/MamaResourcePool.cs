using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace Wombat
{
	/// <summary>
    /// A MamaResourcePool is a convenience wrapper to allow easy subscription and management
    /// of OpenMAMA related resources
	/// </summary>
	public class MamaResourcePool
	{
		private static int MAMA_RESOURCE_POOL_DEFAULT_QUEUE_COUNT = 4;

		// Number of queues in use for the pool
		int mNumQueues = MAMA_RESOURCE_POOL_DEFAULT_QUEUE_COUNT;

		// Name of the pool
		string mName;

		// Implementation structures for the pool managed entities
		private Dictionary<string, MamaResourcePoolBridgeImpl> mPoolBridgeImpls = new Dictionary<string, MamaResourcePoolBridgeImpl>();    // Indexed by bridge name, e.g. "qpid"
		private Dictionary<string, MamaResourcePoolTransportImpl> mPoolTransportImpls = new Dictionary<string, MamaResourcePoolTransportImpl>(); // Indexed by transport name, e.g. "sub"
		private Dictionary<string, MamaResourcePoolSourceImpl> mPoolSourceImpls = new Dictionary<string, MamaResourcePoolSourceImpl>();    // Indexed by source name, e.g. "OM"

		// Default implementations to use
		string mDefaultTransportName;
		string mDefaultSourceName;

		// List of all created subscriptions for cleanup
		private HashSet<MamaResourcePoolSubscriptionImpl> mSubscriptions = new HashSet<MamaResourcePoolSubscriptionImpl>();

		// Cache a list of bridges which this resource pool knows about
		private List<String> mBridgeNames;

		// Regex queue / channel filter
		private MamaRegexChannelFilter mRegexChannelFilter = new MamaRegexChannelFilter();
		private bool mIsMamaOpened;

		// Subscription cleanup callback instance
		private DestroySubscriptionHandler mDestroySubscriptionHandler = new DestroySubscriptionHandler();

		private MamaResourcePoolOptionsImpl mDefaultOptions = new MamaResourcePoolOptionsImpl();

		private class MamaResourcePoolTransportImpl {
			public MamaResourcePoolBridgeImpl mMrpBridge;
			public MamaTransport mTransport;
		};

		private class MamaResourcePoolSourceImpl {
			public MamaSource mSource;
		};

		private class MamaResourcePoolSubscriptionImpl {
			public MamaSubscription mSubscription;
			public MamaResourcePoolSourceImpl mMrpSource;
		};

		private class DestroySubscriptionHandler : MamaQueueEventCallback {
			public void onEvent (MamaQueue queue, object closure) {
				MamaSubscription subscription = (MamaSubscription) closure;
				subscription.destroy();
			}
		}

		private class MamaResourcePoolOptionsImpl {
			public double mTimeout;
			public int mRetries;
			public mamaSubscriptionType mSubscriptionType;
			public bool mRequiresInitial;
			public MamaLogLevel mDebugLevel;
		}

		private class MamaResourcePoolBridgeImpl {
			public string mPoolName;
			public MamaQueueGroup mQueueGroup;
			public MamaBridge mBridge;

			public MamaResourcePoolBridgeImpl(MamaBridge bridge, int numQueues, String poolName) {
				mPoolName = poolName;
				mBridge = bridge;
				mQueueGroup = new MamaQueueGroup(mBridge, numQueues);
				string queuePrefix = Mama.getProperty(string.Format("mama.resource_pool.{0}.thread_name_prefix", mPoolName));
				if (null != queuePrefix) {
					// Do the rounds with each queue in the queue group and name each thread
					for (int i = 0; i < numQueues; i++) {
						MamaQueue queue = mQueueGroup.getNextQueue();
						String queueName = string.Format("{0}_{1}", queuePrefix, i);
						queue.setQueueName(queueName);
					}
				}
			}
		}
		
		private void applyOption(MamaResourcePoolOptionsImpl options, string key, string value) {
	        Mama.log(MamaLogLevel.MAMA_LOG_LEVEL_FINEST, string.Format("Found pool option [{0}] = '{1}'", key, value));
	        switch (key) {
	            case "timeout":
	                options.mTimeout = int.Parse(value);
	                break;
	            case "retries":
	                options.mRetries = int.Parse(value);
	                break;
	            case "subscription_type":
	                switch (value) {
	                    case "NORMAL":
	                        options.mSubscriptionType = mamaSubscriptionType.MAMA_SUBSC_TYPE_NORMAL;
	                        break;
	                    case "GROUP":
	                        options.mSubscriptionType = mamaSubscriptionType.MAMA_SUBSC_TYPE_GROUP;
	                        break;
	                    case "BOOK":
	                        options.mSubscriptionType = mamaSubscriptionType.MAMA_SUBSC_TYPE_BOOK;
	                        break;
	                    case "BASIC":
	                        options.mSubscriptionType = mamaSubscriptionType.MAMA_SUBSC_TYPE_BASIC;
	                        break;
	                    case "DICTIONARY":
	                        options.mSubscriptionType = mamaSubscriptionType.MAMA_SUBSC_TYPE_DICTIONARY;
	                        break;
	                    case "SYMBOL_LIST":
	                        options.mSubscriptionType = mamaSubscriptionType.MAMA_SUBSC_TYPE_SYMBOL_LIST;
	                        break;
	                    default:
	                        Mama.log(MamaLogLevel.MAMA_LOG_LEVEL_WARN, string.Format(
	                                "Found unknown subscription_type: %s (must be NORMAL, GROUP, BOOK, BASIC, "
	                                        + "DICTIONARY, SYMBOL_LIST, SYMBOL_LIST_NORMAL, SYMBOL_LIST_GROUP or SYMBOL_LIST_BOOK",
	                                value));
	                        break;
	                }
	                break;
	            case "requires_initial":
	                options.mRequiresInitial = bool.Parse(value);
	                break;
	            case "debug_level":
	                options.mDebugLevel = Mama.GetLogLevelForString(value);
	                break;
	            default:
	                Mama.log(MamaLogLevel.MAMA_LOG_LEVEL_WARN, string.Format(
	                        "Found unrecognized mamaResourcePool option '{0}'", key));
	                break;
	        }
	    }

	    private void applyOptionsFromConfiguration(MamaResourcePoolOptionsImpl options) {
	        // Use mName
	        string prefix = string.Format("mama.resource_pool.{0}.options.", mName);
	        foreach (KeyValuePair<string, string> kv in Mama.getProperties()) {
	            if (kv.Key.StartsWith(prefix)) {
	                applyOption(options, kv.Key.Substring(prefix.Length), kv.Value);
	            }
	        }
	    }

	    private void applyOptionsFromQueryString(String query, MamaResourcePoolOptionsImpl options) {
	        if (null == query) {
	            return;
	        }

	        // Prune opening question mark for this URI if it exists
	        if (query.StartsWith("?"))
	        {
		        query = query.Substring(1);
	        }

	        string[] queryKeyValPairs = query.Split('&');
	        foreach (string pair in queryKeyValPairs) {
	            int index = pair.IndexOf('=');
	            if (index > 0 && query.Length > index + 1) {
	                string key = pair.Substring(0, index);
	                string value = pair.Substring(index + 1);
	                applyOption(options, key, value);
	            }
	        }
	    }

	    private MamaResourcePoolSourceImpl findOrCreateSource (string name, MamaResourcePoolTransportImpl mrpTransportImpl) {
		    MamaResourcePoolSourceImpl mrpSourceImpl;
		    if (!mPoolSourceImpls.TryGetValue(name, out mrpSourceImpl)) {
			    // There is no transport here - we need to create it
			    MamaSource source = new MamaSource();
			    source.transport = mrpTransportImpl.mTransport;
			    source.symbolNamespace = name;

			    mrpSourceImpl = new MamaResourcePoolSourceImpl();
			    mrpSourceImpl.mSource = source;

			    // Add it to the cache
			    mPoolSourceImpls.Add(name, mrpSourceImpl);
		    }
		    return mrpSourceImpl;
	    }

	    private MamaResourcePoolTransportImpl findOrCreateTransport(string name) {
		    MamaResourcePoolTransportImpl mrpTransportImpl;
			if (!mPoolTransportImpls.TryGetValue(name, out mrpTransportImpl)) {
			    string bridgeName = findBridgeNameByTransportName(name);
			    MamaResourcePoolBridgeImpl mrpBridgeImpl;
			    if (!mPoolBridgeImpls.TryGetValue(bridgeName, out mrpBridgeImpl)) {
				    Mama.log(MamaLogLevel.MAMA_LOG_LEVEL_WARN,
					    string.Format("Could not find bridge {0}", bridgeName));
				    throw new MamaException(MamaStatus.mamaStatus.MAMA_STATUS_NOT_FOUND, "Could not find bridge");
			    }

			    // There is no transport here - we need to create it
			    MamaTransport transport = new MamaTransport();
			    transport.create(name, mrpBridgeImpl.mBridge);

			    mrpTransportImpl = new MamaResourcePoolTransportImpl();
			    mrpTransportImpl.mTransport = transport;
			    mrpTransportImpl.mMrpBridge = mrpBridgeImpl;

			    mPoolTransportImpls.Add(name, mrpTransportImpl);
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

	        string fqTopic;
	        if (null == sourceName) {
	            fqTopic = topicName;
	        } else {
	            fqTopic = string.Format("{0}/{1}", sourceName, topicName);
	        }

	        MamaQueue queue;
	        int queueIndex = mRegexChannelFilter.getChannel(fqTopic);
	        if (queueIndex == int.MaxValue) {
	            queue = mrpTransportImpl.mMrpBridge.mQueueGroup.getNextQueue();
	        } else {
	            queue = mrpTransportImpl.mMrpBridge.mQueueGroup.getQueueByIndex(queueIndex);
	        }

	        Mama.log(MamaLogLevel.MAMA_LOG_LEVEL_FINE, string.Format("MRP Creating subscription for {0}/{1}", sourceName, topicName));
	        MamaResourcePoolSubscriptionImpl mrpSubscriptionImpl = new MamaResourcePoolSubscriptionImpl();
	        MamaResourcePoolSourceImpl mrpSourceImpl = findOrCreateSource(sourceName, mrpTransportImpl);
	        mrpSubscriptionImpl.mMrpSource = mrpSourceImpl;
	        newSubscription.setup(
	                queue,
	                callbacks,
	                mrpSourceImpl.mSource,
	                topicName,
	                closure
	        );
	        mrpSubscriptionImpl.mSubscription = newSubscription;
	        mSubscriptions.Add(mrpSubscriptionImpl);
	        return mrpSubscriptionImpl;
	    }

        /**
         * Destroys a MAMA resource pool and all subscriptions, transports etc that
         * were created using it.
         */
	    [MethodImpl(MethodImplOptions.Synchronized)]
	    public void destroy()
        {
            // Stop dispatch on all the queues to prevent further callbacks
            foreach (KeyValuePair<string, MamaResourcePoolBridgeImpl> kv in mPoolBridgeImpls) {
	            kv.Value.mQueueGroup.stop();
            }
    
	        foreach (MamaResourcePoolSubscriptionImpl mrpSubscriptionImpl in mSubscriptions) {
                if (mrpSubscriptionImpl.mSubscription != null) {
                    mrpSubscriptionImpl.mSubscription.destroy();
                }
            }
            mSubscriptions.Clear();
    
            // Destroy all sources
            foreach (KeyValuePair<string, MamaResourcePoolSourceImpl> kv in mPoolSourceImpls) {
                kv.Value.mSource.destroy();
            }
            mPoolSourceImpls.Clear();
    
            // Destroy all queues
            foreach (KeyValuePair<string, MamaResourcePoolBridgeImpl> kv in mPoolBridgeImpls) {
	            kv.Value.mQueueGroup.destroyWait();
            }
    
            // Destroy all transports
	        foreach (KeyValuePair<string, MamaResourcePoolTransportImpl> kv in mPoolTransportImpls) {
	            kv.Value.mTransport.destroy();
            }
            mPoolTransportImpls.Clear();
    
            // Clear down bridge list too
            mPoolBridgeImpls.Clear();
    
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
	    [MethodImpl(MethodImplOptions.Synchronized)]
	    public MamaSubscription createSubscriptionFromUri (
	            string uri,
	            MamaSubscriptionCallback callbacks,
	            object closure) {
	        if (null == uri || null == callbacks)
	        {
	            throw new MamaException(MamaStatus.mamaStatus.MAMA_STATUS_NULL_ARG, "Null arguments passed to createSubscriptionFromUri");
	        }
	        if (uri.Length == 0) {
	            throw new MamaException(MamaStatus.mamaStatus.MAMA_STATUS_INVALID_ARG, "Zero length uri passed to createSubscriptionFromUri");
	        }
	        Uri parsedUri = new Uri(uri);
	        string bridgeName = parsedUri.Scheme;
	        if (null == bridgeName) {
	            throw new MamaException(MamaStatus.mamaStatus.MAMA_STATUS_NOT_FOUND, "Could not find valid bridge / scheme in createSubscriptionFromUri");
	        }

	        string transportName = parsedUri.Host;
	        if (null == transportName) {
	            throw new MamaException(MamaStatus.mamaStatus.MAMA_STATUS_NOT_FOUND, "Could not find valid transport / host in createSubscriptionFromUri");
	        }

	        MamaResourcePoolBridgeImpl mrpBridgeImpl;
	        if (!mPoolBridgeImpls.TryGetValue(bridgeName, out mrpBridgeImpl)) {
	            throw new MamaException(MamaStatus.mamaStatus.MAMA_STATUS_NOT_FOUND, "Could not find bridge loaded / configured for pool in createSubscriptionFromUri");
	        }

	        string parsedUriPath = parsedUri.AbsolutePath;
	        string[] pathComponents = parsedUriPath.Split('/');
	        if (pathComponents.Length < 2) {
	            throw new MamaException(MamaStatus.mamaStatus.MAMA_STATUS_INVALID_ARG, "Could not find valid path in createSubscriptionFromUri");
	        }

	        // Find or create the transport in question
	        MamaResourcePoolTransportImpl mrpTransportImpl = findOrCreateTransport(transportName);
	        mrpTransportImpl.mMrpBridge = mrpBridgeImpl;

	        // Take a copy of the default options for the pool
	        MamaResourcePoolOptionsImpl subscriptionOptions = mDefaultOptions;

	        // Apply the overrides from the query string
	        applyOptionsFromQueryString(parsedUri.Query, subscriptionOptions);

	        string topicName;
	        string sourceName;
	        if (pathComponents.Length == 2) {
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
	    [MethodImpl(MethodImplOptions.Synchronized)]
	    public MamaSubscription createSubscriptionFromComponents (
	            string transportName,
	            string sourceName,
	            string topicName,
	            MamaSubscriptionCallback callbacks,
	            object closure)
	    {
	        if (null == transportName || null == sourceName ||
	                null == topicName || null == callbacks)
	        {
	            throw new MamaException(MamaStatus.mamaStatus.MAMA_STATUS_NULL_ARG, "Null arguments passed to createSubscriptionFromComponents");
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
	    [MethodImpl(MethodImplOptions.Synchronized)]
	    public MamaSubscription createSubscriptionFromTopicWithSource (
	            string sourceName,
	            string topicName,
	            MamaSubscriptionCallback callbacks,
	            object closure)
	    {
	        if (null == sourceName || null == topicName || null == callbacks)
	        {
	            throw new MamaException(MamaStatus.mamaStatus.MAMA_STATUS_NULL_ARG, "Null arguments passed to createSubscriptionFromTopicWithSource");
	        }

	        String transportName = Mama.getProperty(string.Format("mama.resource_pool.{0}.default_transport_sub", mName));
	        if (null == transportName) {
	            throw new MamaException(MamaStatus.mamaStatus.MAMA_STATUS_NULL_ARG,
		            string.Format(
	                    "Could not create subscription for {0}.{1} - no default_transport_sub " +
	                        "transport defined for resource pool {2}",
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
	    [MethodImpl(MethodImplOptions.Synchronized)]
	    public MamaSubscription createSubscriptionFromTopic (
	            string topicName,
	            MamaSubscriptionCallback callbacks,
	            object closure)
	    {
	        if (null == topicName || null == callbacks)
	        {
	            throw new MamaException(MamaStatus.mamaStatus.MAMA_STATUS_NULL_ARG, "Null arguments passed to createSubscriptionFromTopic");
	        }
	        String sourceName = Mama.getProperty(string.Format("mama.resource_pool.{0}.default_source_sub", mName));
	        if (null == sourceName) {
	            throw new MamaException(
		            MamaStatus.mamaStatus.MAMA_STATUS_NULL_ARG,
		            string.Format(
	                    "Could not create subscription for {0} - no default_source_sub " +
	                        "sub source defined for resource pool {1}",
	                    topicName, mName));
	        }

	        string poolDefaultTransportName = Mama.getProperty(string.Format("mama.resource_pool.{0}.default_transport_sub", mName));
	        string sourceDefaultTransportName = Mama.getProperty(string.Format("mama.source.{0}.transport_sub", sourceName));

	        if (null == poolDefaultTransportName && null == sourceDefaultTransportName) {
	            throw new MamaException(
		            MamaStatus.mamaStatus.MAMA_STATUS_NULL_ARG, 
		            string.Format(
	                    "Could not create subscription for {0} - no default " +
	                            "source or resource pool default transport defined for source " +
	                            "{1} in resource pool {3]",
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
	    [MethodImpl(MethodImplOptions.Synchronized)]
	    public MamaTransport createTransportFromName (
		    string transportName)
	    {
		    if (null == transportName)
		    {
			    throw new MamaException(MamaStatus.mamaStatus.MAMA_STATUS_NULL_ARG,
				    "Null arguments passed to createTransportFromName");
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
	    [MethodImpl(MethodImplOptions.Synchronized)]
	    public void destroySubscription (MamaSubscription subscription)
	    {
		    if (null == subscription)
		    {
			    throw new MamaException(MamaStatus.mamaStatus.MAMA_STATUS_NULL_ARG,
				    "Null arguments passed to destroySubscription");
		    }

		    MamaResourcePoolSubscriptionImpl existingMrpSubscription = null;
		    foreach (MamaResourcePoolSubscriptionImpl mrpSubscription in mSubscriptions) {
			    if (mrpSubscription.mSubscription == subscription) {
				    existingMrpSubscription = mrpSubscription;
				    break;
			    }
		    }
		    if (null == existingMrpSubscription) {
			    throw new MamaException(MamaStatus.mamaStatus.MAMA_STATUS_NOT_FOUND,
				    "MAMA subscription cannot be destroyed since it could not be found in pool");
		    }

		    MamaQueue queue = subscription.subscQueue;
		    queue.enqueueEvent(mDestroySubscriptionHandler, subscription);

		    mSubscriptions.Remove(existingMrpSubscription);
	    }

	    private string findBridgeNameByTransportName (string name) {
		    foreach (string bridgeName in mBridgeNames) {
			    string prefix = string.Format("mama.{0}.transport.{1}.", bridgeName, name);
			    foreach (KeyValuePair<String, String> entry in Mama.getProperties()) {
				    if (entry.Key.StartsWith(prefix)) {
					    return bridgeName;
				    }
			    }
			    throw new MamaException(MamaStatus.mamaStatus.MAMA_STATUS_NOT_FOUND,
				    string.Format("Could not find a bridge for transport {0}", name));
		    }
		    return null;
	    }

        /**
         * Creates a MAMA resource pool to manage subscription lifecycles on behalf of
         * the application calling it.
         *
         * @param name         The name of the MAMA resource pool in configuration
         */
	    public MamaResourcePool (string name)
	    {
	        mIsMamaOpened = false;
	        if (null == name)
	        {
	            throw new MamaException(MamaStatus.mamaStatus.MAMA_STATUS_NULL_ARG,
		            "Null string passed to MamaResourcePool");
	        }
	        mName = name;
	        String bridgesProperty = Mama.getProperty (string.Format("mama.resource_pool.{0}.bridges", mName), "qpid");
	        mBridgeNames = new List<string>(bridgesProperty.Split(','));
	        if (mBridgeNames.Count == 0) {
	            throw new MamaException(
		            MamaStatus.mamaStatus.MAMA_STATUS_NOT_FOUND,
		            "No bridges defined for this resource pool - cannot create");
	        }
	        mRegexChannelFilter.setDefaultChannel(int.MaxValue);
	        mDefaultTransportName = Mama.getProperty (string.Format("mama.resource_pool.{0}.default_transport", mName));
	        mDefaultSourceName = Mama.getProperty (string.Format("mama.resource_pool.{0}.default_source", mName));
	        foreach (string bridgeName in mBridgeNames) {
	            Mama.log(MamaLogLevel.MAMA_LOG_LEVEL_FINE, string.Format("MAMA Resource Pool found bridge {0} - will attempt to load", bridgeName));
	            MamaBridge bridge = null;
	            try {
	                bridge = Mama.loadBridge(bridgeName);
	            } catch (MamaException e) {
	                Mama.log(MamaLogLevel.MAMA_LOG_LEVEL_ERROR,
	                        string.Format("MAMA Resource Pool couldn't load bridge {0}: {1}", bridgeName, e.Message));
	            }
	            if (null != bridge) {
	                string numQueuesProperty = Mama.getProperty(string.Format("mama.resource_pool.%s.queues", mName));
	                if (null != numQueuesProperty) {
	                    mNumQueues = int.Parse(numQueuesProperty);
	                } else {
	                    mNumQueues = MAMA_RESOURCE_POOL_DEFAULT_QUEUE_COUNT;
	                }

	                mPoolBridgeImpls.Add(bridgeName, new MamaResourcePoolBridgeImpl(bridge, mNumQueues, mName));
	                for (int queueIdx = 0; queueIdx < mNumQueues; ++queueIdx) {
	                    String regexStr = Mama.getProperty(string.Format("mama.resource_pool.%s.queue_%d.regex", mName, queueIdx));
	                    if (null != regexStr) {
	                        mRegexChannelFilter.addRegex(regexStr, queueIdx);
	                    }
	                }
	            } else {
	                Mama.log(MamaLogLevel.MAMA_LOG_LEVEL_WARN,
	                        string.Format("MAMA Resource Pool bridge {0} failed to load - will not be available for use in this instance", bridgeName));
	            }
	        }
	        Mama.open();
	        mIsMamaOpened = true;

	        mDefaultOptions.mSubscriptionType = mamaSubscriptionType.MAMA_SUBSC_TYPE_NORMAL;
	        mDefaultOptions.mDebugLevel = MamaLogLevel.MAMA_LOG_LEVEL_OFF;
	        mDefaultOptions.mRetries = 2;
	        mDefaultOptions.mTimeout = 30;
	        mDefaultOptions.mRequiresInitial = true;

	        applyOptionsFromConfiguration(mDefaultOptions);
	    }
	}
}
