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


/**
 * @brief Containers used within the mama plugin
 */
typedef mama_status (*mamaPlugin_publisherPreSendHook) (mamaPluginInfo pluginInfo, mamaPublisher publisher, mamaMsg message);
typedef mama_status (*mamaPlugin_transportPostCreateHook) (mamaPluginInfo pluginInfo, mamaTransport transport);
typedef mama_status (*mamaPlugin_shutdownHook) (mamaPluginInfo pluginInfo);
typedef mama_status (*mamaPlugin_initHook) (mamaPluginInfo* pluginInfo);

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

#endif /* PluginH__ */
