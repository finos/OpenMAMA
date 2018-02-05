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

#if defined( WIN32 )
#   define PLUGINExpDLL __declspec( dllexport )
#else
#   define PLUGINExpDLL
#endif


PLUGINExpDLL
mama_status
templateMamaPlugin_initHook (mamaPluginInfo* pluginInfo);

PLUGINExpDLL
mama_status
templateMamaPlugin_transportPostCreateHook (mamaPluginInfo pluginInfo, mamaTransport transport);

PLUGINExpDLL
mama_status
templateMamaPlugin_publisherPreSendHook (mamaPluginInfo pluginInfo, mamaPublisher publisher, mamaMsg message);

PLUGINExpDLL
mama_status
templateMamaPlugin_shutdownHook (mamaPluginInfo pluginInfo);

#endif /* MAMAPLUGINHOOK_H_ */

