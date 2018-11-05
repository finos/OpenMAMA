/*=========================================================================
  =                             Includes                                  =
  =========================================================================*/

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <mama/mama.h>
#include "bridgefunctions.h"


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
noopBridgeMamaSubscription_create (subscriptionBridge* subscriber,
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

    *subscriber = (subscriptionBridge)impl;

    return MAMA_STATUS_OK;
}

mama_status
noopBridgeMamaSubscription_mute (subscriptionBridge subscriber)
{
    return MAMA_STATUS_OK;
}

mama_status
noopBridgeMamaSubscription_destroy (subscriptionBridge subscriber)
{
    implSubscriptionBridge* impl = (implSubscriptionBridge*) subscriber;
    free ((void*)impl->mSource);
    free ((void*)impl->mSymbol);
    free (subscriber);
    return MAMA_STATUS_OK;
}

int
noopBridgeMamaSubscription_isValid (subscriptionBridge subscriber)
{
    return 1;
}

int
noopBridgeMamaSubscription_hasWildcards (subscriptionBridge subscriber)
{
    return 0;
}

int
noopBridgeMamaSubscription_isTportDisconnected (subscriptionBridge subscriber)
{
    return MAMA_STATUS_OK;
}

mama_status
noopBridgeMamaSubscription_muteCurrentTopic (subscriptionBridge subscriber)
{
    return noopBridgeMamaSubscription_mute (subscriber);
}

