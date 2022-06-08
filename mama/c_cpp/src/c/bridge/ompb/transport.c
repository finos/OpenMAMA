/*=========================================================================
  =                             Includes                                  =
  =========================================================================*/

#include "transport.h"
#include "bridgefunctions.h"
#include <mama/integration/mama.h>
#include <mama/integration/transport.h>
#include <mama/mama.h>
#include <playback/playbackFileParser.h>
#include <signal.h>
#include <wombat/mempool.h>

#define DEFAULT_PLAYBACK_RATE 1000

typedef struct implTransportBridge_
{
    mama_bool_t             mIsValid;
    mama_bool_t             mRewindEnabled;
    mama_bool_t             mRaiseEnabled;
    mamaTransport           mTransport;
    const char*             mName;
    mamaQueue               mDispatcherQueue;
    mamaDispatcher          mDispatcher;
    mamaTimer               mDispatcherTimer;
    const char*             mFileName;
    mama_f64_t              mPlaybackInterval;
    mamaPlaybackFileParser  mFileParser;
    wtable_t                mSubscriptions;
    wthread_mutex_t         mSubscriptionsLock;
} implTransportBridge;

static void MAMACALLTYPE ompbBridgeMamaTransportImpl_dispatchThread (mamaTimer timer, void* closure);

/*=========================================================================
  =               Public interface implementation functions               =
  =========================================================================*/

int
ompbBridgeMamaTransport_isValid (transportBridge transport)
{
    return ((implTransportBridge*)transport)->mIsValid;
}

mama_status
ompbBridgeMamaTransport_destroy (transportBridge transport)
{
    printf("DESTROYING THE TRANSPORT IF THIS WORKS...\n");
    free (transport);
    return MAMA_STATUS_OK;
}

void ompbBridgeMamaTransportImpl_registerSubscription(transportBridge transport,
                                                      const char* identifier,
                                                      mamaSubscription sub)
{
    mama_log(MAMA_LOG_LEVEL_NORMAL, "Registering subscription for [%s]", identifier);
    implTransportBridge* impl = (implTransportBridge*) transport;
    wthread_mutex_lock (&impl->mSubscriptionsLock);
    if (WTABLE_INSERT_SUCCESS != wtable_insert (
             impl->mSubscriptions, identifier, sub)) {
        mama_log(MAMA_LOG_LEVEL_ERROR,
                  "Failed to register subscription %s",
                  identifier);
    }
    wthread_mutex_unlock (&impl->mSubscriptionsLock);
}

#define HEADER_DELIM ':'

void ompbBridgeMamaTransportImpl_dispatchThread (mamaTimer timer, void* closure)
{
    implTransportBridge* impl = (implTransportBridge*) closure;

    // Read on if further header is available
    char* headerString;
    mamaMsg msg;

    int         index        = 0;
    char*       sourceStart  = NULL;
    char*       tportStart   = NULL;
    char*       symStart     = NULL;
    char*       lenStart     = NULL;
    char*       next       = NULL;

    if (mamaPlaybackFileParser_getNextHeader (impl->mFileParser, &headerString))
    {
        // Read on if further message is available
        sourceStart = headerString;
        tportStart = strchr (sourceStart, HEADER_DELIM);
        *tportStart = '\0';
        symStart = strchr (++tportStart, HEADER_DELIM);
        *symStart = '\0';
        lenStart = strchr (++symStart, HEADER_DELIM);
        *lenStart = '\0';

        char identifier[PROPERTY_NAME_MAX_LENGTH];
        snprintf(identifier, sizeof(identifier), "%s.%s", sourceStart, symStart);

        if (mamaPlaybackFileParser_getNextMsg (impl->mFileParser, &msg)) {
            /*
             * Should have a message here which can be relayed onto whatever
             * subscriptions are hanging off this transport
             */
            wthread_mutex_lock (&impl->mSubscriptionsLock);
            void* element = wtable_lookup (impl->mSubscriptions, identifier);
            wthread_mutex_unlock (&impl->mSubscriptionsLock);
            if (element != NULL) {
                mamaSubscription subscription = (mamaSubscription)element;
                // Process inline since the msg is transient
                mamaSubscription_processMsg (subscription, msg);
            }
        } else {
            mama_log(MAMA_LOG_LEVEL_WARN,
                      "Found a header '%s' but no body in playback file.",
                      headerString);
        }
    }
    else if (impl->mRewindEnabled)
    {
        mama_log(MAMA_LOG_LEVEL_NORMAL,
                  "Could not get next header - attempting rewind.");
        mama_status status = mamaPlaybackFileParser_rewindFile (impl->mFileParser);
        if (status != MAMA_STATUS_OK) {
            mama_log(MAMA_LOG_LEVEL_ERROR,
                      "Failed to rewind playback file");
        }
    }
    else if (impl->mRaiseEnabled) {
        raise(2);
    }
}

mama_status
ompbBridgeMamaTransport_create (transportBridge*    result,
                                const char*         name,
                                mamaTransport       parent)
{
    *result = (transportBridge) calloc (1, sizeof(implTransportBridge));
    if (NULL == *result || NULL == name)
    {
        return MAMA_STATUS_NOMEM;
    }
    implTransportBridge* impl = (implTransportBridge*) *result;
    mamaBridge bridge = mamaTransportImpl_getBridgeImpl (parent);
    if (NULL == bridge) {
        goto cleanup_error;
    }
    mama_status status = mamaQueue_create(&impl->mDispatcherQueue, bridge);
    if (status != MAMA_STATUS_OK) {
        mama_log(MAMA_LOG_LEVEL_ERROR,
                  "Failed to create transport queue for %s: %s",
                  name,
                  mamaStatus_stringForStatus (status));
        goto cleanup_error;
    }
    status = mamaDispatcher_create (&impl->mDispatcher, impl->mDispatcherQueue);
    if (status != MAMA_STATUS_OK) {
        mama_log(MAMA_LOG_LEVEL_ERROR,
                  "Failed to create transport dispatcher for %s: %s",
                  name,
                  mamaStatus_stringForStatus (status));
        goto cleanup_error;
    }

    char paramName[PROPERTY_NAME_MAX_LENGTH];
    const char* property = NULL;

    property = properties_GetPropertyValueUsingFormatString (
        mamaInternal_getProperties(),
        "1000",
        "mama.ompb.transport.%s.rate",
        name);
    uint32_t rate = (uint32_t) atol (property);
    impl->mPlaybackInterval = 1.0 / (mama_f64_t)rate;
    mama_log(MAMA_LOG_LEVEL_FINE,
              "Setting playback rate to %" PRIu32 " msg/s in transport %s",
              rate,
              name);

    property = properties_GetPropertyValueUsingFormatString (
        mamaInternal_getProperties(),
        NULL,
        "mama.ompb.transport.%s.file",
        name);
    if (NULL == property) {
        mama_log(MAMA_LOG_LEVEL_ERROR,
                  "Property configuration for %s not found - cannot playback",
                  paramName);
        goto cleanup_error;
    } else {
        mama_log(MAMA_LOG_LEVEL_FINE,
                  "Using playback file '%s' for transport %s",
                  property,
                  name);
        impl->mFileName = strdup(property);
    }

    property = properties_GetPropertyValueUsingFormatString (
        mamaInternal_getProperties(),
        "raise",
        "mama.ompb.transport.%s.on_completion",
        name);
    if (0 == strcasecmp(property, "raise")) {
        impl->mRewindEnabled = 0;
        impl->mRaiseEnabled = 1;
        mama_log(MAMA_LOG_LEVEL_FINE,
                  "Using enabling raise on completion for transport %s",
                  property,
                  name);
    }
    else if (0 == strcasecmp(property, "rewind")) {
        impl->mRewindEnabled = 1;
        impl->mRaiseEnabled = 0;
        mama_log(MAMA_LOG_LEVEL_FINE,
                  "Using rewind on completion for transport %s",
                  property,
                  name);
    } else {
        mama_log(MAMA_LOG_LEVEL_FINE,
                  "Invalid on_completion - defaulting to 'raise' "
                  "for transport %s",
                  property,
                  name);
        impl->mRewindEnabled = 0;
        impl->mRaiseEnabled = 1;
    }

    // Initialize lock and create subscriptions table inside
    wthread_mutex_init(&impl->mSubscriptionsLock, NULL);
    wthread_mutex_lock (&impl->mSubscriptionsLock);
    impl->mSubscriptions = wtable_create("ompbsubs", 1024);
    wthread_mutex_unlock (&impl->mSubscriptionsLock);

    /* Allocate a new playback file parser for the new instrument. */
    status = mamaPlaybackFileParser_allocate (&impl->mFileParser);
    if (MAMA_STATUS_OK != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "Could not allocate file parser for transport: %s",
                  name);
        goto cleanup_error;
    }

    /* Open the playback file for the instrument */
    status = mamaPlaybackFileParser_openFile (impl->mFileParser,
                                              (char*)impl->mFileName);
    if (MAMA_STATUS_OK != status)
    {
        mama_log (MAMA_LOG_LEVEL_WARN,
                  "Could not open playback file '%s' for transport: %s",
                  impl->mFileName,
                  name);
        goto cleanup_error;
    }

    status = mamaTimer_create (&impl->mDispatcherTimer,
                               impl->mDispatcherQueue,
                               ompbBridgeMamaTransportImpl_dispatchThread,
                               impl->mPlaybackInterval,
                               impl);
    if (status != MAMA_STATUS_OK) {
        mama_log(MAMA_LOG_LEVEL_ERROR,
                  "Failed to create transport timer for %s: %s",
                  name,
                  mamaStatus_stringForStatus (status));
        goto cleanup_error;
    }

    impl->mIsValid = 1;
    return MAMA_STATUS_OK;

cleanup_error:
    free (*result);
    if (NULL != impl->mDispatcherTimer) {
        mamaTimer_destroy(impl->mDispatcherTimer);
    }

    if (NULL != impl->mDispatcher) {
        mamaDispatcher_destroy (impl->mDispatcher);
    }

    if (NULL != impl->mDispatcherQueue) {
        mamaQueue_destroy(impl->mDispatcherQueue);
    }

    if (NULL != impl->mFileParser) {
        mamaPlaybackFileParser_closeFile (impl->mFileParser);
        mamaPlaybackFileParser_deallocate (impl->mFileParser);
    }
    return MAMA_STATUS_PLATFORM;
}
