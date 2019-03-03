/*=========================================================================
  =                             Includes                                  =
  =========================================================================*/

#include <stdint.h>
#include <string.h>
#include <mama/mama.h>
#include <mama/integration/types.h>
#include <mama/integration/subscription.h>
#include <mama/integration/bridge/base.h>

/*=========================================================================
  =               Public interface implementation functions               =
  =========================================================================*/

mama_status
baseBridgeMamaSubscription_create (subscriptionBridge* subscriber,
                                   const char*         source,
                                   const char*         symbol,
                                   mamaTransport       tport,
                                   mamaQueue           queue,
                                   mamaMsgCallbacks    callback,
                                   mamaSubscription    subscription,
                                   void*               closure)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
baseBridgeMamaSubscription_createWildCard (subscriptionBridge*     subscriber,
                                           const char*             source,
                                           const char*             symbol,
                                           mamaTransport           transport,
                                           mamaQueue               queue,
                                           mamaMsgCallbacks        callback,
                                           mamaSubscription        subscription,
                                           void*                   closure)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
baseBridgeMamaSubscription_mute (subscriptionBridge subscriber)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
baseBridgeMamaSubscription_destroy (subscriptionBridge subscriber)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

int
baseBridgeMamaSubscription_isValid (subscriptionBridge subscriber)
{
    return 0;
}

int
baseBridgeMamaSubscription_hasWildcards (subscriptionBridge subscriber)
{
    return 0;
}

mama_status
baseBridgeMamaSubscription_getPlatformError (subscriptionBridge subscriber,
                                             void** error)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

int
baseBridgeMamaSubscription_isTportDisconnected (subscriptionBridge subscriber)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
baseBridgeMamaSubscription_setTopicClosure (subscriptionBridge subscriber,
                                            void* closure)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
baseBridgeMamaSubscription_muteCurrentTopic (subscriptionBridge subscriber)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

