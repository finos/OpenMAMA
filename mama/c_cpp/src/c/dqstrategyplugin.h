/*
 * mamaPluginTemplate.h
 *
 *  Created on: 23 Mar 2015
 */

#ifndef MAMAPLUGINHOOK_H_
#define MAMAPLUGINHOOK_H_

#include "mama/mama.h"
#include "mama/status.h"
#include "mama/types.h"
#include "dqstrategy.h"

#if defined( WIN32 )
#   define PLUGINExpDLL __declspec( dllexport )
#else
#   define PLUGINExpDLL
#endif
#if defined(__cplusplus)
extern "C" {
#endif

PLUGINExpDLL
mama_status
dqstrategyMamaPlugin_initHook (mamaPluginInfo* pluginInfo);

PLUGINExpDLL
mama_status
dqstrategyMamaPlugin_transportPostCreateHook (mamaPluginInfo pluginInfo, mamaTransport transport);

PLUGINExpDLL
mama_status
dqstrategyMamaPlugin_publisherPreSendHook (mamaPluginInfo pluginInfo, mamaPublisher publisher, mamaMsg message);

PLUGINExpDLL
mama_status
dqstrategyMamaPlugin_shutdownHook (mamaPluginInfo pluginInfo);

PLUGINExpDLL
mama_status
dqstrategyMamaPlugin_transportEventHook (mamaPluginInfo pluginInfo, mamaTransport transport, int setStale, mamaTransportEvent tportEvent);

PLUGINExpDLL
mama_status
dqstrategyMamaPlugin_subscriptionPostCreateHook (mamaPluginInfo pluginInfo, mamaSubscription subscription);

PLUGINExpDLL
mama_status
dqstrategyMamaPlugin_subscriptionPreMsgHook (mamaPluginInfo pluginInfo, mamaSubscription subscription, int msgType, mamaMsg msg);

PLUGINExpDLL
mama_status
dqstrategyMamaPlugin_shutdownHook (mamaPluginInfo pluginInfo);

#if defined(__cplusplus)
}
#endif

#endif /* MAMAPLUGINHOOK_H_ */

