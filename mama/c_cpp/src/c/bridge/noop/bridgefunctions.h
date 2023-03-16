#include <mama/mama.h>
#include <mama/integration/bridge.h>
#include <mama/integration/publisher.h>
#include <mama/integration/subscription.h>
#include <mama/integration/transport.h>

MAMAExpBridgeDLL
mama_status
noopBridge_init (mamaBridge bridgeImpl);

MAMAExpBridgeDLL
const char*
noopBridge_getVersion (void);

MAMAExpBridgeDLL
const char*
noopBridge_getName (void);

MAMAExpBridgeDLL
mama_status
noopBridge_getDefaultPayloadId (char ***name, char **id);

MAMAExpBridgeDLL
mama_status
noopBridgeMamaPublisher_createByIndex (publisherBridge*     result,
                                           mamaTransport        tport,
                                           int                  tportIndex,
                                           const char*          topic,
                                           const char*          source,
                                           const char*          root,
                                           mamaPublisher        parent);

MAMAExpBridgeDLL
mama_status
noopBridgeMamaPublisher_destroy (publisherBridge publisher);

MAMAExpBridgeDLL
mama_status
noopBridgeMamaPublisher_send (publisherBridge publisher, mamaMsg msg);

MAMAExpBridgeDLL
mama_status
noopBridgeMamaPublisher_sendReplyToInbox (publisherBridge   publisher,
                                              mamaMsg           request,
                                              mamaMsg           reply);

MAMAExpBridgeDLL
mama_status
noopBridgeMamaPublisher_sendReplyToInboxHandle (publisherBridge     publisher,
                                                    void*               inbox,
                                                    mamaMsg             reply);

MAMAExpBridgeDLL
mama_status
noopBridgeMamaPublisher_sendFromInboxByIndex (publisherBridge   publisher,
                                                  int               tportIndex,
                                                  mamaInbox         inbox,
                                                  mamaMsg           msg);

MAMAExpBridgeDLL
mama_status
noopBridgeMamaPublisher_sendFromInbox (publisherBridge  publisher,
                                           mamaInbox        inbox,
                                           mamaMsg          msg);

MAMAExpBridgeDLL
mama_status
noopBridgeMamaPublisher_setUserCallbacks (publisherBridge         publisher,
                                              mamaQueue               queue,
                                              mamaPublisherCallbacks* cb,
                                              void*                   closure);

MAMAExpBridgeDLL
mama_status
noopBridgeMamaSubscription_create (subscriptionBridge* subscriber,
                                       const char*         source,
                                       const char*         symbol,
                                       mamaTransport       tport,
                                       mamaQueue           queue,
                                       mamaMsgCallbacks    callback,
                                       mamaSubscription    subscription,
                                       void*               closure);

MAMAExpBridgeDLL
mama_status
noopBridgeMamaSubscription_mute (subscriptionBridge subscriber);

MAMAExpBridgeDLL
mama_status
noopBridgeMamaSubscription_destroy (subscriptionBridge subscriber);

MAMAExpBridgeDLL
int
noopBridgeMamaSubscription_isValid (subscriptionBridge subscriber);

MAMAExpBridgeDLL
int
noopBridgeMamaSubscription_hasWildcards (subscriptionBridge subscriber);

MAMAExpBridgeDLL
int
noopBridgeMamaSubscription_isTportDisconnected (subscriptionBridge subscriber);

MAMAExpBridgeDLL
mama_status
noopBridgeMamaSubscription_muteCurrentTopic (subscriptionBridge subscriber);

MAMAExpBridgeDLL
int
noopBridgeMamaTransport_isValid (transportBridge transport);

MAMAExpBridgeDLL
mama_status
noopBridgeMamaTransport_destroy (transportBridge transport);

MAMAExpBridgeDLL
mama_status
noopBridgeMamaTransport_create (transportBridge*    result,
                                    const char*         name,
                                    mamaTransport       parent);
