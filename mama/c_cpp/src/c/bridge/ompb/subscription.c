/*=========================================================================
  =                             Includes                                  =
  =========================================================================*/

#include <stdint.h>
#include "bridgefunctions.h"
#include "transport.h"
#include <mama/mama.h>
#include <property.h>
#include <stdlib.h>
#include <string.h>

typedef struct implSubscriptionBridge_
{
    mamaMsgCallbacks    mMamaCallback;
    mamaSubscription    mMamaSubscription;
    mamaQueue           mMamaQueue;
    transportBridge     mTransport;
    const char*         mSource;
    const char*         mSymbol;
    void*               mClosure;
} implSubscriptionBridge;

/*=========================================================================
  =               Public interface implementation functions               =
  =========================================================================*/

mama_status
ompbBridgeMamaSubscription_create (subscriptionBridge* subscriber,
                                   const char*         source,
                                   const char*         symbol,
                                   mamaTransport       tport,
                                   mamaQueue           queue,
                                   mamaMsgCallbacks    callback,
                                   mamaSubscription    subscription,
                                   void*               closure)
{
    implSubscriptionBridge* impl = (implSubscriptionBridge*) calloc (1, sizeof(implSubscriptionBridge));

    if (NULL == impl)
    {
        return MAMA_STATUS_NOMEM;
    }

    mamaTransport_getBridgeTransport (tport, &impl->mTransport);

    if (NULL != source)
    {
        impl->mSource = strdup(source);
    }

    if (NULL != symbol)
    {
        impl->mSymbol = strdup(symbol);
    }

    impl->mMamaCallback        = callback;
    impl->mMamaSubscription    = subscription;
    impl->mMamaQueue           = queue;

    if (source != NULL && symbol != NULL)
    {
        char identifier[PROPERTY_NAME_MAX_LENGTH];
        snprintf(identifier, sizeof(identifier), "%s.%s", source, symbol);
        ompbBridgeMamaTransportImpl_registerSubscription (
            impl->mTransport, identifier, subscription);
    } else if (source != NULL) {
        ompbBridgeMamaTransportImpl_registerSubscription (
            impl->mTransport, source, subscription);
    } else if (symbol != NULL) {
        ompbBridgeMamaTransportImpl_registerSubscription (
            impl->mTransport, symbol, subscription);
    }

    *subscriber = (subscriptionBridge)impl;

    return MAMA_STATUS_OK;
}

mama_status
ompbBridgeMamaSubscription_mute (subscriptionBridge subscriber)
{
    return MAMA_STATUS_OK;
}

mama_status
ompbBridgeMamaSubscription_destroy (subscriptionBridge subscriber)
{
    implSubscriptionBridge* impl = (implSubscriptionBridge*) subscriber;
    free ((void*)impl->mSource);
    free ((void*)impl->mSymbol);
    free (subscriber);
    return MAMA_STATUS_OK;
}

int
ompbBridgeMamaSubscription_isValid (subscriptionBridge subscriber)
{
    return 1;
}

int
ompbBridgeMamaSubscription_hasWildcards (subscriptionBridge subscriber)
{
    return 0;
}

int
ompbBridgeMamaSubscription_isTportDisconnected (subscriptionBridge subscriber)
{
    return MAMA_STATUS_OK;
}

mama_status
ompbBridgeMamaSubscription_muteCurrentTopic (subscriptionBridge subscriber)
{
    return ompbBridgeMamaSubscription_mute (subscriber);
}

