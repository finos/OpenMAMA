#include "resourcepool.h"
#include "list.h"
#include "mama/integration/mama.h"
#include "mama/mama.h"
#include "mama/queuegroup.h"
#include "mama/regexchannelfilter.h"
#include "strutils.h"
#include <apr_pools.h>
#include <apr_uri.h>
#include <mama/resourcepool.h>
#include <wombat/wtable.h>

/*=========================================================================
 =                Typedefs, structs, enums and globals                   =
 =========================================================================*/

#define MAMA_RESOURCE_POOL_DEFAULT_QUEUE_COUNT 4

// Since it's safest having one MAMA Resource Pool per middleware
typedef struct mamaResourcePoolBridgeImpl_
{
    mamaQueueGroup mQueueGroup;
    mamaBridge     mBridge;
} mamaResourcePoolBridgeImpl;

typedef struct mamaResourcePoolTransportImpl_
{
    mamaResourcePoolBridgeImpl* mMrpBridge;
    mamaTransport mTransport;
} mamaResourcePoolTransportImpl;

typedef struct mamaResourcePoolSourceImpl_
{
    mamaSource mSource;
} mamaResourcePoolSourceImpl;

typedef struct mamaResourcePoolSubscriptionImpl_
{
    mamaSubscription mSubscription;
} mamaResourcePoolSubscriptionImpl;

typedef struct mamaResourcePoolSubscriptionListMatcherImpl_
{
    mamaResourcePoolSubscriptionImpl* mMrpSubscriptionImpl;
    mamaSubscription mTarget;
} mamaResourcePoolSubscriptionListMatcherImpl;

typedef struct mamaResourcePoolImpl_
{
    char mName[MAMA_MAX_RESOURCE_POOL_LEN];
    // Configuration options for the pool
    int mNumQueues;

    // Implementation structures for the pool managed entities
    wtable_t mPoolBridgeImpls;    // Indexed by bridge name, e.g. "qpid"
    wtable_t mPoolTransportImpls; // Indexed by transport name, e.g. "sub"
    wtable_t mPoolSourceImpls;    // Indexed by source name, e.g. "OM"

    // Default implementations to use
    const char* mDefaultTransportName;
    const char* mDefaultSourceName;

    // Lock for accessing internal pool structures
    wthread_mutex_t mLock;

    // APR memory pool for cleanup
    apr_pool_t* mAprPool;

    // Default options for use when creating entities within this pool
    mamaResourcePoolOptionsImpl mDefaultOptions;

    // List of all created subscriptions for cleanup
    wList mSubscriptions;

    // Cache a list of bridges which this resource pool knows about
    char mBridgeNames[MAMA_MAX_MIDDLEWARES][MAMA_MAX_BRIDGE_NAME_LEN];
    size_t mBridgeCount;

    // Regex queue / channel filter
    mamaRegexChannelFilter mRegexChannelFilter;
    mama_bool_t mIsMamaOpened;
} mamaResourcePoolImpl;

/*=========================================================================
 =        Private implementation prototypes and static functions          =
 =========================================================================*/

void MAMACALLTYPE
mamaResourcePoolImpl_destroyMamaSubscriptionCb (mamaQueue queue, void* closure);

mama_status
mamaResourcePoolImpl_createSubscriptionFromComponents (
    mamaResourcePoolImpl* impl,
    mamaResourcePoolBridgeImpl* mrpBridgeImpl,
    mamaResourcePoolTransportImpl* mrpTransportImpl,
    const char* sourceName,
    const char* topicName,
    mamaMsgCallbacks callbacks,
    void* closure,
    mamaResourcePoolOptionsImpl* mrpOptionsImpl,
    mamaResourcePoolSourceImpl** mrpSourceImpl,
    mamaResourcePoolSubscriptionImpl** mrpSubscriptionImpl
);

mama_status
mamaResourcePoolImpl_initializeBridgeImpl (
    mamaResourcePoolImpl* impl,
    mamaResourcePoolBridgeImpl* bridgeImpl,
    mamaBridge bridge
);

mama_status
mamaResourcePoolImpl_findBridgeNameByTransportName (
    mamaResourcePoolImpl* impl,
    const char* transportName,
    char *bridgeName);

mama_status
mamaResourcePoolImpl_findOrCreateTransport (
    mamaResourcePoolImpl* impl,
    const char* name,
    mamaResourcePoolBridgeImpl** mrpBridgeImpl,
    mamaResourcePoolTransportImpl** mrpTransportImpl
);

mama_status
mamaResourcePoolImpl_findOrCreateSource (
    mamaResourcePoolImpl* impl,
    const char* name,
    mamaTransport transport,
    mamaResourcePoolSourceImpl** sourceImpl
);

void
mamaResourcePoolImpl_applyOption (
    mamaResourcePoolOptionsImpl* options,
    const char* key,
    const char* value
) {
    mama_log (MAMA_LOG_LEVEL_FINEST, "Found pool option [%s] = '%s'", key, value);
    if (strcmp(key, "timeout") == 0) {
        options->mTimeout = atof (value);
    } else if (strcmp(key, "retries") == 0) {
        options->mRetries = atoi (value);
    } else if (strcmp (key, "subscription_type") == 0) {
        if (strcmp(value, "NORMAL") == 0) {
            options->mSubscriptionType = MAMA_SUBSC_TYPE_NORMAL;
        } else if (strcmp(value, "GROUP") == 0) {
            options->mSubscriptionType = MAMA_SUBSC_TYPE_GROUP;
        } else if (strcmp(value, "BOOK") == 0) {
            options->mSubscriptionType = MAMA_SUBSC_TYPE_BOOK;
        } else if (strcmp(value, "BASIC") == 0) {
            options->mSubscriptionType = MAMA_SUBSC_TYPE_BASIC;
        } else if (strcmp(value, "DICTIONARY") == 0) {
            options->mSubscriptionType = MAMA_SUBSC_TYPE_DICTIONARY;
        } else if (strcmp(value, "SYMBOL_LIST") == 0) {
            options->mSubscriptionType = MAMA_SUBSC_TYPE_SYMBOL_LIST;
        } else if (strcmp(value, "SYMBOL_LIST_NORMAL") == 0) {
            options->mSubscriptionType = MAMA_SUBSC_TYPE_SYMBOL_LIST_NORMAL;
        } else if (strcmp(value, "SYMBOL_LIST_GROUP") == 0) {
            options->mSubscriptionType = MAMA_SUBSC_TYPE_SYMBOL_LIST_GROUP;
        } else if (strcmp(value, "SYMBOL_LIST_BOOK") == 0) {
            options->mSubscriptionType = MAMA_SUBSC_TYPE_SYMBOL_LIST_BOOK;
        } else {
            mama_log (MAMA_LOG_LEVEL_WARN,
                      "Found unknown subscription_type: %s (must be NORMAL, "
                      "GROUP, BOOK, BASIC, DICTIONARY, SYMBOL_LIST, "
                      "SYMBOL_LIST_NORMAL, SYMBOL_LIST_GROUP or "
                      "SYMBOL_LIST_BOOK",
                      value);
        }
    } else if (strcmp (key, "requires_initial") == 0) {
        options->mRequiresInitial = (mama_bool_t) strtobool (value);
    } else if (strcmp (key, "debug_level") == 0) {
        mama_tryStringToLogLevel (value, &options->mDebugLevel);
    } else {
        mama_log (MAMA_LOG_LEVEL_WARN,
                  "Found unrecognized mamaResourcePool option '%s'",
                  key);
    }
}

static void
mamaResourcePoolImpl_applyOptionFromPropertyWalker (
    const char* name,
    const char* value,
    void* closure
) {
    mamaResourcePoolOptionsImpl* options = (mamaResourcePoolOptionsImpl*) closure;
    mamaResourcePoolImpl_applyOption (options, name, value);
}

void mamaResourcePoolImpl_applyOptionsFromConfiguration (
    const char* poolName,
    mamaResourcePoolOptionsImpl* options
) {
    char propertyPrefix[PROPERTY_NAME_MAX_LENGTH];
    snprintf (propertyPrefix,
              sizeof(propertyPrefix),
              "mama.resource_pool.%s.options.",
              poolName);
    propertyPrefix[sizeof(propertyPrefix) - 1] = '\0';

    // Cache to avoid repeated strlen calls in callback
    properties_ForEachWithPrefix (mamaInternal_getProperties(),
                                  propertyPrefix,
                                  mamaResourcePoolImpl_applyOptionFromPropertyWalker,
                                  options);
}

void mamaResourcePoolImpl_applyOptionsFromQueryString (
    const char* query,
    mamaResourcePoolOptionsImpl* options
) {
    char *mutableQuery = NULL;
    char *curPos = NULL;
    char *tokens = mutableQuery;
    if (NULL == query) {
        return;
    }
    mutableQuery = strdup(query);
    while ((curPos = wstrsep (&tokens, "&\n"))) {
        char *var = strtok (curPos, "=");
        char *val = NULL;
        if (var != NULL && (val = strtok (NULL, "="))) {
            mamaResourcePoolImpl_applyOption(options, var, val);
        }
    }
    free (mutableQuery);
}

static void
mamaResourcePoolImpl_destroySubscriptionListCb (wList list, void* element, void* closure) {
    mamaResourcePoolSubscriptionListMatcherImpl* matcher = (mamaResourcePoolSubscriptionListMatcherImpl*) closure;
    mamaResourcePoolSubscriptionImpl* mrpSubscriptionImpl = (mamaResourcePoolSubscriptionImpl*) element;
    if (mrpSubscriptionImpl->mSubscription == matcher->mTarget) {
        matcher->mMrpSubscriptionImpl = mrpSubscriptionImpl;
    }
}

void MAMACALLTYPE
mamaResourcePoolImpl_destroyMamaSubscriptionCb (mamaQueue queue, void* closure) {
    mamaSubscription subscription = (mamaSubscription) closure;
    mamaSubscription_destroy (subscription);
    mamaSubscription_deallocate (subscription);
}

static void
mamaResourcePoolImpl_stopDispatchPoolBridgeCb(wtable_t from, void* data, const char* key, void* closure) {
    mamaResourcePoolBridgeImpl* mrpBridgeImpl = (mamaResourcePoolBridgeImpl*) data;
    mama_status status = mamaQueueGroup_stopDispatch(mrpBridgeImpl->mQueueGroup);
    if (MAMA_STATUS_OK != status) {
        mama_log(MAMA_LOG_LEVEL_WARN,
                  "Failed to stop dispatch in MAMA resource pool for %s: %s",
                  key,
                  mamaStatus_stringForStatus (status));
    }
}

static void
mamaResourcePoolImpl_destroyQueuesPoolBridgeCb(wtable_t from, void* data, const char* key, void* closure) {
    mamaResourcePoolBridgeImpl* mrpBridgeImpl = (mamaResourcePoolBridgeImpl*) data;
    mama_status status = mamaQueueGroup_destroyWait (mrpBridgeImpl->mQueueGroup);
    if (MAMA_STATUS_OK != status) {
        mama_log(MAMA_LOG_LEVEL_WARN,
                  "Failed to destroy (wait) queue group in MAMA resource pool for %s: %s",
                  key,
                  mamaStatus_stringForStatus (status));
    }
    status = mamaQueueGroup_destroy (mrpBridgeImpl->mQueueGroup);
    if (MAMA_STATUS_OK != status) {
        mama_log(MAMA_LOG_LEVEL_WARN,
                  "Failed to destroy queue group in MAMA resource pool for %s: %s",
                  key,
                  mamaStatus_stringForStatus (status));
    }
}

static void
mamaResourcePoolImpl_destroyTransportPoolTransportCb(wtable_t from, void* data, const char* key, void* closure) {
    mamaResourcePoolTransportImpl * mrpTransportImpl = (mamaResourcePoolTransportImpl*) data;
    mama_status status = mamaTransport_destroy (mrpTransportImpl->mTransport);
    if (MAMA_STATUS_OK != status) {
        mama_log(MAMA_LOG_LEVEL_WARN,
                  "Failed to destroy transport in MAMA resource pool for %s: %s",
                  key,
                  mamaStatus_stringForStatus (status));
    }
}

static void
mamaResourcePoolImpl_destroySourcePoolSourceCb(wtable_t from, void* data, const char* key, void* closure) {
    mamaResourcePoolSourceImpl * mrpSourceImpl = (mamaResourcePoolSourceImpl*) data;
    mama_status status = mamaSource_destroy (mrpSourceImpl->mSource);
    if (MAMA_STATUS_OK != status) {
        mama_log(MAMA_LOG_LEVEL_WARN,
                  "Failed to destroy source in MAMA resource pool for %s: %s",
                  key,
                  mamaStatus_stringForStatus (status));
    }
}

static void
mamaResourcePoolImpl_destroySubscriptionCb(wList list, void* element, void* closure) {
    mamaResourcePoolSubscriptionImpl* mrpSubImpl = (mamaResourcePoolSubscriptionImpl*) element;
    // Take a copy of symbol - let's not assume it's available after destruction
    mama_status status;
    const char* symbol;
    char symbolCopy[MAMA_MAX_SYMBOL_LEN];
    mamaSubscription_getSymbol (mrpSubImpl->mSubscription, &symbol);
    strncpy (symbolCopy, symbol, sizeof(symbolCopy));

    status = mamaSubscription_destroy (mrpSubImpl->mSubscription);
    if (MAMA_STATUS_OK != status) {
        mama_log (MAMA_LOG_LEVEL_WARN,
                  "Failed to destroy subscription in MAMA resource pool for %s: %s",
                  symbol,
                  mamaStatus_stringForStatus (status));
    }
    status = mamaSubscription_deallocate (mrpSubImpl->mSubscription);
    if (MAMA_STATUS_OK != status) {
        mama_log (MAMA_LOG_LEVEL_WARN,
                  "Failed to destroy subscription  in MAMA resource pool for %s: %s",
                  symbol,
                  mamaStatus_stringForStatus (status));
    }
}

/*=========================================================================
 =               Public interface implementation functions               =
 =========================================================================*/

MAMAExpDLL
extern mama_status
mamaResourcePool_create (
    mamaResourcePool* resourcePool,
    const char* name
) {
    mama_status status;
    apr_status_t aprPoolStatus;
    mamaResourcePoolImpl* impl = NULL;
    char originalBridgesPropertyValue[PROPERTY_NAME_MAX_LENGTH];
    const char* bridgesProperty = NULL;
    char* nextSeg = NULL;
    const char* defaultTransport;
    const char* defaultSource;
    size_t i;
    if (NULL == resourcePool || NULL == name) {
        return MAMA_STATUS_NULL_ARG;
    }
    if (strlen(name) == 0) {
        return MAMA_STATUS_INVALID_ARG;
    }

    impl = (mamaResourcePoolImpl*) calloc (1, sizeof(mamaResourcePoolImpl));
    if (NULL == impl) {
        status = MAMA_STATUS_NOMEM;
        goto cleanup;
    }

    wthread_mutex_init(&impl->mLock, NULL);
    if (strlen (name) < sizeof(impl->mName)) {
        strncpy (impl->mName, name, sizeof(impl->mName));
    } else {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "Cannot create resource pool - name '%s' too long",
                  impl->mName);
        goto cleanup;
    }

    impl->mPoolBridgeImpls = wtable_create ("mrp_bridges", 10);
    if (NULL == impl->mPoolBridgeImpls) {
        status = MAMA_STATUS_NOMEM;
        goto cleanup;
    }
    impl->mPoolTransportImpls = wtable_create ("mrp_transports", 10);
    if (NULL == impl->mPoolTransportImpls) {
        status = MAMA_STATUS_NOMEM;
        goto cleanup;
    }

    impl->mPoolSourceImpls = wtable_create ("mrp_sources", 10);
    if (NULL == impl->mPoolSourceImpls) {
        status = MAMA_STATUS_NOMEM;
        goto cleanup;
    }

    aprPoolStatus = apr_initialize();
    if (APR_SUCCESS != aprPoolStatus) {
        status = MAMA_STATUS_NOMEM;
        goto cleanup;
    }

    aprPoolStatus = apr_pool_create_unmanaged (&impl->mAprPool);
    if (APR_SUCCESS != aprPoolStatus) {
        status = MAMA_STATUS_NOMEM;
        goto cleanup;
    }

    impl->mSubscriptions = list_create (
        sizeof (mamaResourcePoolSubscriptionImpl));
    if (impl->mSubscriptions == INVALID_LIST) {
        status = MAMA_STATUS_NOMEM;
        goto cleanup;
    }

    bridgesProperty = mamaImpl_getParameter (
        "qpid", "mama.resource_pool.%s.bridges", name);
    strncpy (originalBridgesPropertyValue, bridgesProperty, sizeof(originalBridgesPropertyValue));

    nextSeg = strtok (originalBridgesPropertyValue, ",");
    while (nextSeg != NULL) {
        strcpy (impl->mBridgeNames[impl->mBridgeCount++], nextSeg);
        nextSeg = strtok (NULL, ",");
    }

    if (0 == impl->mBridgeCount) {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "No bridges defined for this resource pool - cannot create");
        status = MAMA_STATUS_INVALID_ARG;
        goto cleanup;
    }

    // Create the regex filter object
    status = mamaRegexChannelFilter_create(&impl->mRegexChannelFilter);
    if (MAMA_STATUS_OK != status) {
        goto cleanup;
    }

    // Default channel should be INT_MAX so we can easily detect it
    status = mamaRegexChannelFilter_setDefaultChannel (
        impl->mRegexChannelFilter,
        INT_MAX);
    if (MAMA_STATUS_OK != status) {
        goto cleanup;
    }

    // Gather in default values from properties
    defaultTransport = properties_GetPropertyValueUsingFormatString (
            mamaInternal_getProperties(),
            NULL,
            "mama.resource_pool.%s.default_transport",
            name);
    if (defaultTransport)
    {
        impl->mDefaultTransportName = strdup(defaultTransport);
    }
    defaultSource = properties_GetPropertyValueUsingFormatString (
            mamaInternal_getProperties(),
            NULL,
            "mama.resource_pool.%s.default_source",
            name);
    if (defaultSource) {
        impl->mDefaultSourceName = strdup(defaultSource);
    }

    for (i = 0; i < impl->mBridgeCount; i++) {
        mamaBridge bridge;
        // Separate status since bridge load failing is non-fatal (other bridges
        // may succeed)
        mama_status bridgeLoadStatus;
        mama_log (MAMA_LOG_LEVEL_FINE,
                  "MAMA Resource Pool found bridge %s - will attempt to load",
                  impl->mBridgeNames[i]);

        bridgeLoadStatus = mama_loadBridge (
            &bridge,
            impl->mBridgeNames[i]);
        if (bridgeLoadStatus == MAMA_STATUS_OK) {
            int wtableResult = 0;
            int queueIdx = 0;
            // Create a mama resource pool bridge implementation container
            mamaResourcePoolBridgeImpl* mrpBridgeImpl =
                (mamaResourcePoolBridgeImpl*) calloc (1, sizeof(mamaResourcePoolBridgeImpl));
            if (NULL == mrpBridgeImpl) {
                status = MAMA_STATUS_NOMEM;
                goto cleanup;
            }
            wtableResult = wtable_insert (impl->mPoolBridgeImpls,
                                           impl->mBridgeNames[i],
                                           mrpBridgeImpl);
            if (wtableResult < 0) {
                // If insertion failed, we also need to clean up the element
                free (mrpBridgeImpl);
                status = MAMA_STATUS_NOMEM;
                goto cleanup;
            }
            impl->mNumQueues = (int) mamaImpl_getParameterAsLong (
                MAMA_RESOURCE_POOL_DEFAULT_QUEUE_COUNT,
                1,
                UINT8_MAX,
                "mama.resource_pool.%s.queues", name);
            for (queueIdx = 0; queueIdx < impl->mNumQueues; queueIdx++) {
                const char* regexStr = properties_GetPropertyValueUsingFormatString (
                    mamaInternal_getProperties(),
                    NULL,
                    "mama.resource_pool.%s.queue_%d.regex",
                    name,
                    queueIdx);
                if (NULL != regexStr) {
                    bridgeLoadStatus = mamaRegexChannelFilter_addRegex (
                        impl->mRegexChannelFilter,
                        regexStr,
                        queueIdx);
                    if (MAMA_STATUS_OK != bridgeLoadStatus) {
                        mama_log (
                            MAMA_LOG_LEVEL_WARN,
                            "Failed to add regex %s to pool '%s': %s",
                            regexStr,
                            name,
                            mamaStatus_stringForStatus (bridgeLoadStatus));
                    }
                }
            }
            bridgeLoadStatus = mamaResourcePoolImpl_initializeBridgeImpl (
                impl,
                mrpBridgeImpl,
                bridge);
            if (bridgeLoadStatus != MAMA_STATUS_OK) {
                mama_log (MAMA_LOG_LEVEL_WARN,
                          "Failed to initialize bridge %s for pool '%s': %s",
                          impl->mBridgeNames[i],
                          name,
                          mamaStatus_stringForStatus (bridgeLoadStatus));
            } else {
                mama_log (MAMA_LOG_LEVEL_FINE,
                          "MAMA Resource Pool bridge %s now active",
                          impl->mBridgeNames[i]);
            }
        } else {
            mama_log(
                MAMA_LOG_LEVEL_WARN,
                "MAMA Resource Pool bridge %s failed to load - will not be "
                "available for use in this instance",
                impl->mBridgeNames[i]);
        }
    }

    // Do the open here. Can be called multiple times as long as the close count
    // matches.
    status = mama_open();
    if (MAMA_STATUS_OK != status) {
        goto cleanup;
    } else {
        impl->mIsMamaOpened = MAMA_BOOL_TRUE;
    }

    // Set default values for options
    impl->mDefaultOptions.mSubscriptionType = MAMA_SUBSC_TYPE_NORMAL;
    impl->mDefaultOptions.mDebugLevel = MAMA_LOG_LEVEL_OFF;
    impl->mDefaultOptions.mRetries = MAMA_DEFAULT_RETRIES;
    impl->mDefaultOptions.mTimeout = MAMA_DEFAULT_TIMEOUT;
    impl->mDefaultOptions.mRequiresInitial = MAMA_BOOL_TRUE;

    // Apply any default options from config
    mamaResourcePoolImpl_applyOptionsFromConfiguration (impl->mName,
                                                        &impl->mDefaultOptions);

    *resourcePool = impl;

    return status;

cleanup:
    mamaResourcePool_destroy (impl);
    return status;
}

MAMAExpDLL
extern mama_status
mamaResourcePool_destroy (
    mamaResourcePool resourcePool
) {
    mamaResourcePoolImpl* impl = (mamaResourcePoolImpl*) resourcePool;
    mama_status status = MAMA_STATUS_OK;
    if (NULL == resourcePool) {
        return MAMA_STATUS_INVALID_ARG;
    }

    wthread_mutex_lock(&impl->mLock);

    // Stop dispatch on all the queues to prevent further callbacks
    if (NULL != impl->mPoolBridgeImpls)
    {
        wtable_for_each (impl->mPoolBridgeImpls,
                         mamaResourcePoolImpl_stopDispatchPoolBridgeCb,
                         NULL);
    }

    // Destroy and deallocate all subscriptions
    if (NULL != impl->mSubscriptions)
    {
        list_destroy (impl->mSubscriptions,
                      mamaResourcePoolImpl_destroySubscriptionCb,
                      NULL);
    }

    // Destroy all sources
    if (NULL != impl->mPoolSourceImpls)
    {
        wtable_for_each (impl->mPoolSourceImpls,
                         mamaResourcePoolImpl_destroySourcePoolSourceCb,
                         NULL);
        wtable_free_all (impl->mPoolSourceImpls);
        wtable_destroy (impl->mPoolSourceImpls);
    }

    // Destroy all queues
    if (NULL != impl->mPoolBridgeImpls)
    {
        wtable_for_each (impl->mPoolBridgeImpls,
                         mamaResourcePoolImpl_destroyQueuesPoolBridgeCb,
                         NULL);
    }

    // Destroy all transports
    if (NULL != impl->mPoolTransportImpls)
    {
        wtable_for_each (impl->mPoolTransportImpls,
                         mamaResourcePoolImpl_destroyTransportPoolTransportCb,
                         NULL);
        wtable_free_all (impl->mPoolTransportImpls);
        wtable_destroy (impl->mPoolTransportImpls);
    }

    // Destroy all bridge trackers for the resource pool
    if (NULL != impl->mPoolBridgeImpls)
    {
        wtable_free_all (impl->mPoolBridgeImpls);
        wtable_destroy (impl->mPoolBridgeImpls);
    }

    if (NULL != impl->mRegexChannelFilter)
    {
        mamaRegexChannelFilter_destroy (impl->mRegexChannelFilter);
    }

    // These values are strdup'd so clean up
    free ((void*)impl->mDefaultTransportName);
    free ((void*)impl->mDefaultSourceName);

    // Free up MAMA Resource Pool resources
    if (NULL != impl->mAprPool)
    {
        apr_pool_destroy (impl->mAprPool);
        apr_terminate ();
    }

    // Close out MAMA if we ever opened it
    if (impl->mIsMamaOpened) {
        status = mama_close ();
    }

    // Destroy the lock
    wthread_mutex_unlock (&impl->mLock);
    wthread_mutex_destroy (&impl->mLock);

    // Free up the main implementation
    free (impl);

    return status;
}

MAMAExpDLL
extern mama_status
mamaResourcePool_createSubscriptionFromUri (
    mamaResourcePool resourcePool,
    mamaSubscription* subscription,
    const char* uri,
    mamaMsgCallbacks callbacks,
    void* closure
) {
    // This is the *only* function which won't already have a default bridge
    // configured and therefore the only function which needs to do a hash table
    // lookup to find the bridge for the given subscription request. Even this
    // hash table though should only return a bridge index in the mama resource
    // pool table. A mama resource pool's bridges should be known at
    // construction time and from that point should be immutable.
    mamaResourcePoolImpl* impl = (mamaResourcePoolImpl*) resourcePool;
    apr_uri_t parsedUri;
    apr_status_t aprStatus;
    mama_status status;
    const char* bridgeName = NULL;
    const char* transportName = NULL;
    mamaResourcePoolTransportImpl* mrpTransportImpl = NULL;
    mamaResourcePoolOptionsImpl subscriptionOptions;
    mamaResourcePoolBridgeImpl* mrpBridgeImpl = NULL;
    size_t originalPathLength = 0;
    char*  sourceName         = NULL;
    char*  topicName          = NULL;
    mamaResourcePoolSubscriptionImpl* mrpSubscriptionImpl = NULL;
    mamaResourcePoolSourceImpl* mrpSourceImpl = NULL;
    if (NULL == impl || NULL == subscription || NULL == uri) {
        return MAMA_STATUS_NULL_ARG;
    }

    if (NULL == callbacks.onMsg || 0 == strlen (uri)) {
        return MAMA_STATUS_INVALID_ARG;
    }

    wthread_mutex_lock (&impl->mLock);
    aprStatus = apr_uri_parse (impl->mAprPool, uri, &parsedUri);
    if (APR_SUCCESS != aprStatus) {
        mama_log (MAMA_LOG_LEVEL_WARN,
                  "Could not parse url %s - is it correctly formed?",
                  uri);
        wthread_mutex_unlock (&impl->mLock);
        return MAMA_STATUS_INVALID_ARG;
    }

    // The bridge is the scheme
    bridgeName = parsedUri.scheme;
    if (NULL == bridgeName) {
        mama_log (MAMA_LOG_LEVEL_WARN,
                  "Could not parse url %s - could not find a valid bridge.",
                  uri);
        wthread_mutex_unlock (&impl->mLock);
        return MAMA_STATUS_INVALID_ARG;
    }

    // The transport is the authority / hostname
    transportName = parsedUri.hostname;
    if (NULL == transportName) {
        mama_log (MAMA_LOG_LEVEL_WARN,
                  "Could not parse url %s - could not find a valid transport.",
                  uri);
        wthread_mutex_unlock (&impl->mLock);
        return MAMA_STATUS_INVALID_ARG;
    }

    // Find the bridge for this
    mrpBridgeImpl = (mamaResourcePoolBridgeImpl*) wtable_lookup (
        impl->mPoolBridgeImpls, bridgeName);
    if (NULL == mrpBridgeImpl) {
        mama_log (MAMA_LOG_LEVEL_WARN,
                  "Could not find bridge using scheme '%s'.",
                  bridgeName);
        wthread_mutex_unlock (&impl->mLock);
        return MAMA_STATUS_NOT_FOUND;
    }

    // Find or create the transport in question
    status = mamaResourcePoolImpl_findOrCreateTransport (
        impl, transportName, &mrpBridgeImpl, &mrpTransportImpl);
    if (status != MAMA_STATUS_OK) {
        mama_log (MAMA_LOG_LEVEL_WARN,
                  "Could not create transport %s - %s.",
                  transportName,
                  mamaStatus_stringForStatus (status));
        wthread_mutex_unlock (&impl->mLock);
        return MAMA_STATUS_INVALID_ARG;
    }

    // Take a copy of the default options for the pool
    subscriptionOptions = impl->mDefaultOptions;

    // Apply the overrides from the query string
    mamaResourcePoolImpl_applyOptionsFromQueryString (parsedUri.query,
                                                      &subscriptionOptions);

    // All the paths begin with a "/" so this will simply skip past that
    originalPathLength = strlen (parsedUri.path);
    sourceName         = strtok (parsedUri.path, "/");
    topicName          = NULL;
    // Fully qualified name is used for regex matching so source is considered
    if (NULL == sourceName) {
        // There is no source present - this is a topic
        topicName     = sourceName;
        sourceName    = NULL;

    } else if (originalPathLength > strlen (sourceName) + 1) {
        // If there was more after the trailing slash, read in the topic
        topicName = sourceName + strlen(sourceName) + 1;
    }

    status = mamaResourcePoolImpl_createSubscriptionFromComponents (
        impl,
        mrpBridgeImpl,
        mrpTransportImpl,
        sourceName,
        topicName,
        callbacks,
        closure,
        &subscriptionOptions,
        &mrpSourceImpl,
        &mrpSubscriptionImpl
    );

    *subscription = mrpSubscriptionImpl->mSubscription;

    wthread_mutex_unlock(&impl->mLock);
    return status;
}

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
) {
    // Find or create the transport in question
    mamaResourcePoolImpl* impl = (mamaResourcePoolImpl*) resourcePool;
    mamaResourcePoolTransportImpl* mrpTransportImpl = NULL;
    mamaResourcePoolBridgeImpl* mrpBridgeImpl = NULL;
    mamaResourcePoolSourceImpl* mrpSourceImpl = NULL;
    mamaResourcePoolSubscriptionImpl* mrpSubscriptionImpl = NULL;
    mama_status status;

    if (NULL == impl || NULL == subscription || NULL == transportName
        || NULL == topicName) {
        return MAMA_STATUS_NULL_ARG;
    }

    if (callbacks.onMsg == NULL) {
        return MAMA_STATUS_INVALID_ARG;
    }

    wthread_mutex_lock(&impl->mLock);

    status = mamaResourcePoolImpl_findOrCreateTransport (
        impl, transportName, &mrpBridgeImpl, &mrpTransportImpl);
    if (status != MAMA_STATUS_OK) {
        mama_log (MAMA_LOG_LEVEL_WARN,
                  "Could not create transport %s - %s.",
                  transportName,
                  mamaStatus_stringForStatus (status));
        wthread_mutex_unlock (&impl->mLock);
        return MAMA_STATUS_INVALID_ARG;
    }

    status = mamaResourcePoolImpl_createSubscriptionFromComponents (
        impl,
        mrpBridgeImpl,
        mrpTransportImpl,
        sourceName,
        topicName,
        callbacks,
        closure,
        NULL,
        &mrpSourceImpl,
        &mrpSubscriptionImpl
    );
    *subscription = mrpSubscriptionImpl->mSubscription;
    wthread_mutex_unlock(&impl->mLock);
    return status;
}


MAMAExpDLL
extern mama_status
mamaResourcePool_createSubscriptionFromTopicWithSource (
    mamaResourcePool resourcePool,
    mamaSubscription* subscription,
    const char* sourceName,
    const char* topicName,
    mamaMsgCallbacks callbacks,
    void* closure
) {
    mamaResourcePoolImpl* impl = (mamaResourcePoolImpl*) resourcePool;
    const char* transportName = NULL;
    if (NULL == impl || NULL == subscription || NULL == topicName) {
        return MAMA_STATUS_NULL_ARG;
    }

    if (callbacks.onMsg == NULL) {
        return MAMA_STATUS_INVALID_ARG;
    }

    // Get the default subscribing transport name from the mama.properties for
    // this resource pool
    transportName = properties_GetPropertyValueUsingFormatString (
        mamaInternal_getProperties(),
        NULL,
        "mama.resource_pool.%s.default_transport_sub",
        impl->mName);
    if (NULL == transportName) {
        mama_log (MAMA_LOG_LEVEL_WARN,
                  "Could not create subscription for %s.%s - no default_transport_sub "
                  "transport defined for resource pool %s",
                  sourceName == NULL ? "" : sourceName,
                  topicName,
                  impl->mName);
        return MAMA_STATUS_INVALID_ARG;
    }
    return mamaResourcePool_createSubscriptionFromComponents (
            resourcePool,
            subscription,
            transportName,
            sourceName,
            topicName,
            callbacks,
            closure
    );
}

MAMAExpDLL
extern mama_status
mamaResourcePool_createSubscriptionFromTopic(
    mamaResourcePool resourcePool,
    mamaSubscription* subscription,
    const char* topicName,
    mamaMsgCallbacks callbacks,
    void* closure
) {
    mamaResourcePoolImpl* impl = (mamaResourcePoolImpl*) resourcePool;
    const char* sourceName = NULL;
    const char* poolDefaultTransportName = NULL;
    const char* sourceDefaultTransportName = NULL;
    if (NULL == impl || NULL == subscription || NULL == topicName) {
        return MAMA_STATUS_NULL_ARG;
    }

    if (callbacks.onMsg == NULL) {
        return MAMA_STATUS_INVALID_ARG;
    }
    // Get the default subscribing transport name from the mama.properties for
    // this resource pool
    sourceName = properties_GetPropertyValueUsingFormatString (
        mamaInternal_getProperties(),
        NULL,
        "mama.resource_pool.%s.default_source_sub",
        impl->mName);
    if (NULL == sourceName) {
        mama_log (MAMA_LOG_LEVEL_WARN,
                  "Could not create subscription for %s - no default_source_sub "
                  "sub source defined for resource pool %s",
                  topicName == NULL ? "" : topicName,
                  impl->mName);
        return MAMA_STATUS_INVALID_ARG;
    }

    // Get the default subscribing transport name from the mama.properties for
    // this resource pool
    poolDefaultTransportName = properties_GetPropertyValueUsingFormatString (
        mamaInternal_getProperties(),
        NULL,
        "mama.resource_pool.%s.default_transport_sub",
        impl->mName);

    // Get the default subscribing transport name from the mama.properties for
    // this source
    sourceDefaultTransportName = properties_GetPropertyValueUsingFormatString (
        mamaInternal_getProperties(),
        NULL,
        "mama.source.%s.transport_sub",
        sourceName);
    if (NULL == poolDefaultTransportName && NULL == sourceDefaultTransportName) {
        mama_log (MAMA_LOG_LEVEL_WARN,
                  "Could not create subscription for %s - no default "
                  "source or resource pool default transport defined for source "
                  "%s in resource pool %s",
                  topicName,
                  sourceName,
                  impl->mName);
        return MAMA_STATUS_INVALID_ARG;
    }

    return mamaResourcePool_createSubscriptionFromComponents (
        resourcePool,
        subscription,
        // Prefer source default if available, then use the pool's
        sourceDefaultTransportName != NULL ? sourceDefaultTransportName : poolDefaultTransportName,
        sourceName,
        topicName,
        callbacks,
        closure
    );
}

MAMAExpDLL
extern mama_status
mamaResourcePool_createTransportFromName (
    mamaResourcePool resourcePool,
    mamaTransport* transport,
    const char* transportName)
{
    mamaResourcePoolImpl* impl = (mamaResourcePoolImpl*) resourcePool;
    mamaResourcePoolBridgeImpl* mrpBridgeImpl = NULL;
    mamaResourcePoolTransportImpl * mrpTransportImpl = NULL;
    mama_status status;
    if (NULL == impl || NULL == transport || NULL == transportName) {
        return MAMA_STATUS_NULL_ARG;
    }

    if (0 == strlen (transportName)) {
        return MAMA_STATUS_INVALID_ARG;
    }
    // Find or create the transport in question
    status = mamaResourcePoolImpl_findOrCreateTransport (
        impl, transportName, &mrpBridgeImpl, &mrpTransportImpl);
    if (status == MAMA_STATUS_OK) {
        *transport = mrpTransportImpl->mTransport;
    }

    return status;
}

MAMAExpDLL
extern mama_status
mamaResourcePool_destroySubscription (
    mamaResourcePool resourcePool,
    mamaSubscription subscription
)
{
    mamaResourcePoolImpl* impl = (mamaResourcePoolImpl*) resourcePool;
    mamaResourcePoolSubscriptionListMatcherImpl matcher;
    mamaQueue queue = NULL;
    mama_status status;
    if (NULL == impl || NULL == subscription) {
        return MAMA_STATUS_NULL_ARG;
    }
    memset (&matcher, 0, sizeof(matcher));
    matcher.mTarget = subscription;
    list_for_each (impl->mSubscriptions,
                   mamaResourcePoolImpl_destroySubscriptionListCb,
                   &matcher);
    if (matcher.mMrpSubscriptionImpl == NULL) {
        return MAMA_STATUS_NOT_FOUND;
    }

    // Schedule this subscription's removal (most happen on subscriber's queue
    status = mamaSubscription_getQueue (
        matcher.mMrpSubscriptionImpl->mSubscription,
        &queue);
    if (status != MAMA_STATUS_OK) {
        return status;
    }
    status = mamaQueue_enqueueEvent (
        queue,
        mamaResourcePoolImpl_destroyMamaSubscriptionCb,
        matcher.mMrpSubscriptionImpl->mSubscription);
    if (status != MAMA_STATUS_OK) {
        return status;
    }

    // Remove our passive reference to this subscription
    list_remove_element (impl->mSubscriptions, matcher.mMrpSubscriptionImpl);
    list_free_element (impl->mSubscriptions, matcher.mMrpSubscriptionImpl);

    return MAMA_STATUS_OK;
}

/*=========================================================================
 =                  Private implementation functions                     =
 =========================================================================*/

mama_status
mamaResourcePoolImpl_createSubscriptionFromComponents (
    mamaResourcePoolImpl* impl,
    mamaResourcePoolBridgeImpl* mrpBridgeImpl,
    mamaResourcePoolTransportImpl* mrpTransportImpl,
    const char* sourceName,
    const char* topicName,
    mamaMsgCallbacks callbacks,
    void* closure,
    mamaResourcePoolOptionsImpl* mrpOptionsImpl,
    mamaResourcePoolSourceImpl** mrpSourceImpl,
    mamaResourcePoolSubscriptionImpl** mrpSubscriptionImpl
) {
    mamaResourcePoolOptionsImpl subscriptionOptions;
    mama_status status;
    mamaSubscription newSubscription = NULL;
    char fqTopic[MAMA_MAX_SYMBOL_LEN];
    int queueIndex = 0;
    mamaQueue queue = NULL;
    mamaResourcePoolSubscriptionImpl* mrpSubImpl = NULL;
    if (NULL == mrpOptionsImpl) {
        subscriptionOptions = impl->mDefaultOptions;
    } else {
        subscriptionOptions = *mrpOptionsImpl;
    }
    status = mamaSubscription_allocate (&newSubscription);
    if (status != MAMA_STATUS_OK) goto cleanup;
    status = mamaSubscription_setTimeout (newSubscription, subscriptionOptions.mTimeout);
    if (status != MAMA_STATUS_OK) goto cleanup;
    status = mamaSubscription_setRetries (newSubscription, subscriptionOptions.mRetries);
    if (status != MAMA_STATUS_OK) goto cleanup;
    status = mamaSubscription_setSubscriptionType (
        newSubscription, subscriptionOptions.mSubscriptionType);
    if (status != MAMA_STATUS_OK) goto cleanup;
    status = mamaSubscription_setRequiresInitial (
        newSubscription, subscriptionOptions.mRequiresInitial);
    if (status != MAMA_STATUS_OK) goto cleanup;
    status = mamaSubscription_setDebugLevel (newSubscription,
                                             subscriptionOptions.mDebugLevel);
    if (status != MAMA_STATUS_OK) goto cleanup;

    if (NULL == sourceName) {
        strcpy (fqTopic, topicName);
    } else {
        snprintf (fqTopic, sizeof(fqTopic), "%s/%s", sourceName, topicName);
    }

    status = mamaRegexChannelFilter_getChannel (
        impl->mRegexChannelFilter,
        fqTopic,
        &queueIndex);
    if (queueIndex == INT_MAX || status != MAMA_STATUS_OK) {
        // If no regex match defined / found, round robin
        mamaQueueGroup_getNextQueue (mrpBridgeImpl->mQueueGroup, &queue);
    } else {
        mamaQueueGroup_getQueueByIndex (mrpBridgeImpl->mQueueGroup, &queue, queueIndex);
    }

    if (sourceName && topicName) {
        // Find or create the source in question
        status = mamaResourcePoolImpl_findOrCreateSource (
            impl, sourceName, mrpTransportImpl->mTransport, mrpSourceImpl);
        if (status != MAMA_STATUS_OK) {
            mama_log (MAMA_LOG_LEVEL_WARN,
                      "Could not create source %s - %s.",
                      sourceName,
                      mamaStatus_stringForStatus (status));
            status = MAMA_STATUS_INVALID_ARG;
            goto cleanup;
        }
        status = mamaSubscription_setup (newSubscription,
                                         queue,
                                         &callbacks,
                                         (*mrpSourceImpl)->mSource,
                                         topicName,
                                         closure);
    } else {
        status = mamaSubscription_createBasic (newSubscription,
                                               mrpTransportImpl->mTransport,
                                               queue,
                                               &callbacks,
                                               topicName,
                                               closure);
    }
    if (status != MAMA_STATUS_OK) goto cleanup;

    mrpSubImpl = (mamaResourcePoolSubscriptionImpl*) list_allocate_element (impl->mSubscriptions);
    if (NULL == mrpSubImpl) {
        status = MAMA_STATUS_NOMEM;
        goto cleanup;
    }
    mrpSubImpl->mSubscription = newSubscription;
    list_push_back (impl->mSubscriptions, mrpSubImpl);

    *mrpSubscriptionImpl = mrpSubImpl;

    return MAMA_STATUS_OK;

cleanup:
    if (newSubscription != NULL) {
        mamaSubscription_destroy (newSubscription);
    }
    return status;
}

mama_status
mamaResourcePoolImpl_initializeBridgeImpl(
    mamaResourcePoolImpl* impl,
    mamaResourcePoolBridgeImpl* bridgeImpl,
    mamaBridge bridge)
{
    mama_status status;
    const char* queuePrefix = NULL;
    int i;
    bridgeImpl->mBridge = bridge;
    status = mamaQueueGroup_create (
        &bridgeImpl->mQueueGroup, impl->mNumQueues, bridge);
    if (status != MAMA_STATUS_OK) {
        goto cleanup;
    }

    // Get the queue prefix from configuration
    queuePrefix = mamaImpl_getParameter (
        "mrp", "mama.resource_pool.%s.thread_name_prefix", impl->mName);
    if (NULL == queuePrefix) {
        return status;
    }

    // Do the rounds with each queue in the queue group and name each thread
    for (i = 0; i < impl->mNumQueues; i++) {
        char queueName[PROPERTY_NAME_MAX_LENGTH];
        mamaQueue queue;
        status = mamaQueueGroup_getNextQueue (bridgeImpl->mQueueGroup, &queue);
        if (status != MAMA_STATUS_OK) {
            goto cleanup;
        }
        snprintf (queueName, sizeof(queueName), "%s_%d", queuePrefix, i);
        status = mamaQueue_setQueueName (queue, queueName);
        if (status != MAMA_STATUS_OK) {
            mama_log (MAMA_LOG_LEVEL_WARN,
                      "Failed to name queue #%d in resource pool %s to: '%s'",
                      i,
                      impl->mName,
                      queueName);
        }
    }
    return status;

cleanup:
    return status;
}

static void
mamaResourcePoolImpl_findBridgeNameByTransportNamePropWalker(
    const char* name,
    const char* value,
    void* closure
) {
    // Increment the match count
    ++(*(size_t*)closure);
}

mama_status
mamaResourcePoolImpl_findBridgeNameByTransportName(
    mamaResourcePoolImpl* impl,
    const char* transportName,
    char* bridgeName)
{
    size_t i;
    // We can only look up according to bridges which we have configured within
    // the pool
    for (i = 0; i < impl->mBridgeCount; i++) {
        // Look for a property matching this prefix - if one is found, this is
        // presumed to be the correct bridge for the given transport
        char propertyPrefix[PROPERTY_NAME_MAX_LENGTH];
        size_t matches = 0;
        snprintf(propertyPrefix,
                  sizeof(propertyPrefix),
                  "mama.%s.transport.%s.",
                  impl->mBridgeNames[i],
                  transportName);
        propertyPrefix[sizeof(propertyPrefix) - 1] = '\0';
        // Cache to avoid repeated strlen calls in callback
        properties_ForEachWithPrefix (
            mamaInternal_getProperties(),
            propertyPrefix,
            mamaResourcePoolImpl_findBridgeNameByTransportNamePropWalker,
            &matches);
        if (matches > 0) {
            strcpy (bridgeName, impl->mBridgeNames[i]);
            return MAMA_STATUS_OK;
        }
    }
    return MAMA_STATUS_NOT_FOUND;
}

mama_status
mamaResourcePoolImpl_findOrCreateTransport(
    mamaResourcePoolImpl* impl,
    const char* name,
    mamaResourcePoolBridgeImpl** mrpBridgeImpl,
    mamaResourcePoolTransportImpl** mrpTransportImpl)
{
    mama_status status;
    mamaTransport transport = NULL;
    mamaResourcePoolTransportImpl* newMrpTransportImpl = NULL;
    mamaResourcePoolTransportImpl* existing = (mamaResourcePoolTransportImpl*) wtable_lookup (
        impl->mPoolTransportImpls, name);
    int wtable_result = 0;
    if (NULL != existing)
    {
        *mrpBridgeImpl = existing->mMrpBridge;
        *mrpTransportImpl = existing;
        return MAMA_STATUS_OK;
    }

    // If mrpBridgeImpl is NULL, and this is a transport that we don't know, we
    // need to go try and find out which bridge this transport translates to
    if (NULL == *mrpBridgeImpl)
    {
        char bridgeName[MAMA_MAX_BRIDGE_NAME_LEN];
        status = mamaResourcePoolImpl_findBridgeNameByTransportName (
            impl, name, bridgeName);
        if (MAMA_STATUS_OK != status)
        {
            mama_log (MAMA_LOG_LEVEL_WARN,
                      "Could not find a bridge for transport %s",
                      name);
            return status;
        }
        *mrpBridgeImpl = (mamaResourcePoolBridgeImpl*) wtable_lookup (impl->mPoolBridgeImpls, bridgeName);
        if (NULL == *mrpBridgeImpl) {
            return MAMA_STATUS_NOT_FOUND;
        }
    }

    // There is no transport here - we need to create it
    status = mamaTransport_allocate (&transport);
    if (MAMA_STATUS_OK != status) {
        return status;
    }

    status = mamaTransport_create (transport, name, (*mrpBridgeImpl)->mBridge);
    if (MAMA_STATUS_OK != status) {
        mamaTransport_destroy (transport);
        return status;
    }

    newMrpTransportImpl = (mamaResourcePoolTransportImpl*)calloc(1, sizeof(mamaResourcePoolTransportImpl));
    if (NULL == newMrpTransportImpl) {
        return MAMA_STATUS_NOMEM;
    }
    newMrpTransportImpl->mTransport = transport;
    newMrpTransportImpl->mMrpBridge = (*mrpBridgeImpl);

    wtable_result = wtable_insert (impl->mPoolTransportImpls, name, newMrpTransportImpl);
    if (wtable_result < 0) {
        free(newMrpTransportImpl);
        mamaTransport_destroy (transport);
        return MAMA_STATUS_NOMEM;
    } else {
        mama_log (MAMA_LOG_LEVEL_FINE,
                  "Created MAMA Resource Pool transport '%s'",
                  name);
        *mrpTransportImpl = newMrpTransportImpl;
        return MAMA_STATUS_OK;
    }
}

mama_status
mamaResourcePoolImpl_findOrCreateSource (
    mamaResourcePoolImpl* impl,
    const char* name,
    mamaTransport transport,
    mamaResourcePoolSourceImpl** sourceImpl)
{
    mama_status status;
    int wtable_result = 0;
    mamaSource source = NULL;
    mamaResourcePoolSourceImpl* mrpSourceImpl = (mamaResourcePoolSourceImpl*)wtable_lookup (
        impl->mPoolSourceImpls, name);
    if (NULL != mrpSourceImpl)
    {
        *sourceImpl = mrpSourceImpl;
        return MAMA_STATUS_OK;
    }

    // There is no transport here - we need to create it
    status = mamaSource_create (&source);
    if (MAMA_STATUS_OK != status) {
        return status;
    }

    status = mamaSource_setTransport (source, transport);
    if (MAMA_STATUS_OK != status) {
        mamaSource_destroy (source);
        return status;
    }

    status = mamaSource_setSymbolNamespace(source, name);
    if (MAMA_STATUS_OK != status) {
        mamaSource_destroy (source);
        return status;
    }

    mrpSourceImpl = calloc(1, sizeof(mamaResourcePoolSourceImpl));
    if (NULL == mrpSourceImpl) {
        return MAMA_STATUS_NOMEM;
    }
    mrpSourceImpl->mSource = source;

    wtable_result = wtable_insert (impl->mPoolSourceImpls, name, mrpSourceImpl);
    if (wtable_result < 0) {
        free(mrpSourceImpl);
        mamaSource_destroy (source);
        return MAMA_STATUS_NOMEM;
    } else {
        mama_log (MAMA_LOG_LEVEL_FINE,
                  "Created MAMA Resource Pool transport '%s'",
                  name);
        *sourceImpl = mrpSourceImpl;
        return MAMA_STATUS_OK;
    }
}
