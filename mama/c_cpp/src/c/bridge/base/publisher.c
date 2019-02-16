/*=========================================================================
 =                             Includes                                  =
 =========================================================================*/

#include <stdint.h>
#include <string.h>

#include <mama/mama.h>
#include <mama/inbox.h>
#include <mama/publisher.h>
#include <mama/integration/bridge.h>
#include <mama/integration/bridge/base.h>
#include <mama/integration/publisher.h>


/*=========================================================================
 =               Public interface implementation functions               =
 =========================================================================*/

mama_status
baseBridgeMamaPublisher_createByIndex (publisherBridge*     result,
                                       mamaTransport        tport,
                                       int                  tportIndex,
                                       const char*          topic,
                                       const char*          source,
                                       const char*          root,
                                       mamaPublisher        parent)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
baseBridgeMamaPublisher_destroy (publisherBridge publisher)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
baseBridgeMamaPublisher_send (publisherBridge publisher, mamaMsg msg)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

/* Send reply to inbox. */
mama_status
baseBridgeMamaPublisher_sendReplyToInbox (publisherBridge   publisher,
                                          mamaMsg           request,
                                          mamaMsg           reply)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
baseBridgeMamaPublisher_sendReplyToInboxHandle (publisherBridge     publisher,
                                                void*               inbox,
                                                mamaMsg             reply)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

/* Send a message from the specified inbox using the throttle. */
mama_status
baseBridgeMamaPublisher_sendFromInboxByIndex (publisherBridge   publisher,
                                              int               tportIndex,
                                              mamaInbox         inbox,
                                              mamaMsg           msg)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
baseBridgeMamaPublisher_sendFromInbox (publisherBridge  publisher,
                                       mamaInbox        inbox,
                                       mamaMsg          msg)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
baseBridgeMamaPublisher_setUserCallbacks (publisherBridge         publisher,
                                          mamaQueue               queue,
                                          mamaPublisherCallbacks* cb,
                                          void*                   closure)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}
