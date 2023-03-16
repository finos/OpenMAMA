#include <mama/mama.h>
#include <mama/integration/bridge.h>
#include <mama/integration/publisher.h>
#include <mama/integration/subscription.h>
#include <mama/integration/transport.h>

MAMAExpBridgeDLL
mama_status
ompbBridge_init (mamaBridge bridgeImpl);

MAMAExpBridgeDLL
const char*
ompbBridge_getVersion (void);

MAMAExpBridgeDLL
const char*
ompbBridge_getName (void);

MAMAExpBridgeDLL
mama_status
ompbBridge_getDefaultPayloadId (char ***name, char **id);

MAMAExpBridgeDLL
mama_status
ompbBridgeMamaPublisher_createByIndex (publisherBridge*     result,
                                           mamaTransport        tport,
                                           int                  tportIndex,
                                           const char*          topic,
                                           const char*          source,
                                           const char*          root,
                                           mamaPublisher        parent);

MAMAExpBridgeDLL
mama_status
ompbBridgeMamaPublisher_destroy (publisherBridge publisher);

MAMAExpBridgeDLL
mama_status
ompbBridgeMamaPublisher_send (publisherBridge publisher, mamaMsg msg);

MAMAExpBridgeDLL
mama_status
ompbBridgeMamaPublisher_sendReplyToInbox (publisherBridge   publisher,
                                              mamaMsg           request,
                                              mamaMsg           reply);

MAMAExpBridgeDLL
mama_status
ompbBridgeMamaPublisher_sendReplyToInboxHandle (publisherBridge     publisher,
                                                    void*               inbox,
                                                    mamaMsg             reply);

MAMAExpBridgeDLL
mama_status
ompbBridgeMamaPublisher_sendFromInboxByIndex (publisherBridge   publisher,
                                                  int               tportIndex,
                                                  mamaInbox         inbox,
                                                  mamaMsg           msg);

MAMAExpBridgeDLL
mama_status
ompbBridgeMamaPublisher_sendFromInbox (publisherBridge  publisher,
                                           mamaInbox        inbox,
                                           mamaMsg          msg);

MAMAExpBridgeDLL
mama_status
ompbBridgeMamaPublisher_setUserCallbacks (publisherBridge         publisher,
                                              mamaQueue               queue,
                                              mamaPublisherCallbacks* cb,
                                              void*                   closure);

MAMAExpBridgeDLL
mama_status
ompbBridgeMamaSubscription_create (subscriptionBridge* subscriber,
                                       const char*         source,
                                       const char*         symbol,
                                       mamaTransport       tport,
                                       mamaQueue           queue,
                                       mamaMsgCallbacks    callback,
                                       mamaSubscription    subscription,
                                       void*               closure);

MAMAExpBridgeDLL
mama_status
ompbBridgeMamaSubscription_mute (subscriptionBridge subscriber);

MAMAExpBridgeDLL
mama_status
ompbBridgeMamaSubscription_destroy (subscriptionBridge subscriber);

MAMAExpBridgeDLL
int
ompbBridgeMamaSubscription_isValid (subscriptionBridge subscriber);

MAMAExpBridgeDLL
int
ompbBridgeMamaSubscription_hasWildcards (subscriptionBridge subscriber);

MAMAExpBridgeDLL
int
ompbBridgeMamaSubscription_isTportDisconnected (subscriptionBridge subscriber);

MAMAExpBridgeDLL
mama_status
ompbBridgeMamaSubscription_muteCurrentTopic (subscriptionBridge subscriber);

MAMAExpBridgeDLL
int
ompbBridgeMamaTransport_isValid (transportBridge transport);

MAMAExpBridgeDLL
mama_status
ompbBridgeMamaTransport_destroy (transportBridge transport);

MAMAExpBridgeDLL
mama_status
ompbBridgeMamaTransport_create (transportBridge*    result,
                                    const char*         name,
                                    mamaTransport       parent);
