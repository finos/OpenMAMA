#include <mama/mama.h>
#include <mama/integration/bridge.h>
#include <mama/integration/publisher.h>
#include <mama/integration/subscription.h>
#include <mama/integration/transport.h>

mama_status
noopBridge_init (mamaBridge bridgeImpl);

const char*
noopBridge_getVersion (void);

const char*
noopBridge_getName (void);

mama_status
noopBridge_getDefaultPayloadId (char ***name, char **id);

mama_status
noopBridgeMamaPublisher_createByIndex (publisherBridge*     result,
                                           mamaTransport        tport,
                                           int                  tportIndex,
                                           const char*          topic,
                                           const char*          source,
                                           const char*          root,
                                           mamaPublisher        parent);

mama_status
noopBridgeMamaPublisher_destroy (publisherBridge publisher);

mama_status
noopBridgeMamaPublisher_send (publisherBridge publisher, mamaMsg msg);

mama_status
noopBridgeMamaPublisher_sendReplyToInbox (publisherBridge   publisher,
                                              mamaMsg           request,
                                              mamaMsg           reply);

mama_status
noopBridgeMamaPublisher_sendReplyToInboxHandle (publisherBridge     publisher,
                                                    void*               inbox,
                                                    mamaMsg             reply);

mama_status
noopBridgeMamaPublisher_sendFromInboxByIndex (publisherBridge   publisher,
                                                  int               tportIndex,
                                                  mamaInbox         inbox,
                                                  mamaMsg           msg);

mama_status
noopBridgeMamaPublisher_sendFromInbox (publisherBridge  publisher,
                                           mamaInbox        inbox,
                                           mamaMsg          msg);

mama_status
noopBridgeMamaPublisher_setUserCallbacks (publisherBridge         publisher,
                                              mamaQueue               queue,
                                              mamaPublisherCallbacks* cb,
                                              void*                   closure);

mama_status
noopBridgeMamaSubscription_create (subscriptionBridge* subscriber,
                                       const char*         source,
                                       const char*         symbol,
                                       mamaTransport       tport,
                                       mamaQueue           queue,
                                       mamaMsgCallbacks    callback,
                                       mamaSubscription    subscription,
                                       void*               closure);

mama_status
noopBridgeMamaSubscription_mute (subscriptionBridge subscriber);

mama_status
noopBridgeMamaSubscription_destroy (subscriptionBridge subscriber);

int
noopBridgeMamaSubscription_isValid (subscriptionBridge subscriber);

int
noopBridgeMamaSubscription_hasWildcards (subscriptionBridge subscriber);

int
noopBridgeMamaSubscription_isTportDisconnected (subscriptionBridge subscriber);

mama_status
noopBridgeMamaSubscription_muteCurrentTopic (subscriptionBridge subscriber);

int
noopBridgeMamaTransport_isValid (transportBridge transport);
mama_status
noopBridgeMamaTransport_destroy (transportBridge transport);

mama_status
noopBridgeMamaTransport_create (transportBridge*    result,
                                    const char*         name,
                                    mamaTransport       parent);