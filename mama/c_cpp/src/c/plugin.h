/* $Id$
 *
 * OpenMAMA: The open middleware agnostic messaging API
 * Copyright (C) 2011 NYSE Technologies, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#ifndef PluginH__
#define PluginH__
#include "dqstrategyplugin.h"

/**
 * @brief Containers used within the mama plugin
 */
typedef mama_status (*mamaPlugin_publisherPreSendHook) (mamaPluginInfo pluginInfo, mamaPublisher publisher, mamaMsg message);
typedef mama_status (*mamaPlugin_transportPostCreateHook) (mamaPluginInfo pluginInfo, mamaTransport transport);
typedef mama_status (*mamaPlugin_shutdownHook) (mamaPluginInfo pluginInfo);
typedef mama_status (*mamaPlugin_initHook) (mamaPluginInfo* pluginInfo);
typedef mama_status (*mamaPlugin_subscriptionPostCreateHook) (mamaPluginInfo pluginInfo, mamaSubscription subscription);
typedef mama_status (*mamaPlugin_subscriptionPreMsgHook) (mamaPluginInfo pluginInfo, mamaSubscription subscription, int msgType, mamaMsg msg);
typedef mama_status (*mamaPlugin_transportEventHook) (mamaPluginInfo pluginInfo, mamaTransport transport, int setStale, mamaTransportEvent tportEvent);

typedef struct mamaPluginImpl_
{
    LIB_HANDLE          mPluginHandle;
    char*               mPluginName;
    mamaPluginInfo      mPluginInfo;

    mamaPlugin_publisherPreSendHook                 mamaPluginPublisherPreSendHook;
    mamaPlugin_transportPostCreateHook              mamaPluginTransportPostCreateHook;
    mamaPlugin_shutdownHook                         mamaPluginShutdownHook;
    mamaPlugin_initHook                             mamaPluginInitHook;
    mamaPlugin_subscriptionPostCreateHook           mamaPluginSubscriptionPostCreateHook;
    mamaPlugin_subscriptionPreMsgHook               mamaPluginSubscriptionPreMsgHook;
    mamaPlugin_transportEventHook                   mamaPluginTransportEventHook;

} mamaPluginImpl;

 /**
 * @brief Used find a plugin using the library name
 *
 * param[in] pluginName
 *
 * @return mama_status return code can be one of:
 *          MAMA_STATUS_OK
 */
mama_status
mama_loadPlugin (const char* pluginName);

/**
 * @brief Used find a plugin using the library name
 *
 * param[in] name
 *
 * @return a valid mamaPluginImpl if found
 */
mamaPluginImpl*
mamaPlugin_findPlugin (const char* name);

/**
 * @brief Used to load register all possible plugin functions to be
 *        used within Mama.
 *
 * param[in] pluginLib
 * param[in] name
 * param[in] pluginImpl The plugin impl to be used
 *
 * @return mama_status return code can be one of:
 *          MAMA_STATUS_OK
 */
mama_status
mamaPlugin_registerFunctions   (LIB_HANDLE      pluginLib,
                                const char*     name,
                                mamaPluginInfo  pluginInfo,
                                mamaPluginImpl* pluginImpl);
int 
mamaPlugin_isUsingDq(void);

/**
 * Initialize the internal plugin interface
 *
 * @return mama status code
 */
extern mama_status
mama_initPlugins (void);

/**
 * Shutdown the internal plugin interface
 *
 * @return mama status code
 */
extern mama_status
mama_shutdownPlugins (void);

extern mama_status
mamaPlugin_firePublisherPreSendHook (mamaPublisher publisher, mamaMsg message);

extern mama_status
mamaPlugin_fireTransportPostCreateHook (mamaTransport transport);

extern mama_status
mamaPlugin_fireShutdownHook (void);

extern mama_status
mamaPlugin_fireSubscriptionPostCreateHook (mamaSubscription subscription);

extern mama_status
mamaPlugin_fireSubscriptionPreMsgHook (mamaSubscription subscription, int msgType, mamaMsg msg);

extern mama_status
mamaPlugin_fireTransportEventHook (mamaTransport transport, int setStale, mamaTransportEvent tportEvent);

#endif /* PluginH__ */
