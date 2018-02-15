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

#ifndef TransportImplH__
#define TransportImplH__

#include <time.h>

#include "mama/transport.h"
#include "throttle.h"
#include "bridge.h"
#include "throttle.h"
#include "list.h"
#include "mama/entitlement.h"
#include "refreshtransport.h"
#include "plugin.h"

#ifndef OPENMAMA_INTEGRATION
  #define OPENMAMA_INTEGRATION
#endif

#include <mama/integration/transport.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct PublisherInfo_
{
    mamaPublisher mPublisher;
} PublisherInfo;

/*
 * Methods used internally by other classes but not exposed in MAMA API.
 */

mama_status mamaTransport_addPublisher(mamaTransport transport, mamaPublisher publisher, void **result);
mama_status mamaTransport_removePublisher(mamaTransport transport, void *handle);

MAMAExpDLL
preInitialScheme mamaTransportImpl_getPreInitialScheme (mamaTransport transport);

MAMAExpDLL
dqStartegyScheme
mamaTransportImpl_getDqStrategyScheme (mamaTransport transport);

MAMAExpDLL
dqftStrategyScheme
mamaTransportImpl_getFtStrategyScheme (mamaTransport transport);

MAMAExpDLL
extern mama_bool_t
mamaTransportImpl_preRecapCacheEnabled (mamaTransport transport);

mama_status
mamaTransportImpl_getEntitlementBridge(mamaTransport tport, mamaEntitlementBridge* entBridge);

MAMAExpDLL
mama_status
getPossiblyStaleForAll(mamaTransport transport, int* possiblyStaleForAll);

MAMAExpDLL
mama_status
mamaTransportImpl_setQuality(mamaTransport transport, mamaQuality quality);

MAMAExpDLL
void
mamaTransportImpl_setPossiblyStaleForListeners(mamaTransport transport);

MAMAExpDLL
mama_status
mamaTransportImpl_getPossiblyStaleForAll(mamaTransport transport, int* possiblyStale);

MAMAExpDLL
mama_status
mamaTransportImpl_getRefreshTransport(mamaTransport transport, refreshTransport *rTransport);

MAMAExpDLL
mama_status
mamaTransportImpl_setDqPlugin (mamaTransport transport, mamaPluginImpl* dqPlugin);

MAMAExpDLL
mama_status
mamaTransportImpl_getDqPlugin (mamaTransport transport, mamaPluginImpl** dqPlugin);

MAMAExpDLL
mama_status
mamaTransportImpl_getDqEnabled(mamaTransport transport, int* result);

MAMAExpDLL
mama_status
mamaTransportImpl_setDqPluginInfo (mamaTransport transport, mamaPluginInfo pluginInfo);

MAMAExpDLL
mama_status
mamaTransportImpl_getDqPluginInfo (mamaTransport transport, mamaPluginInfo* pluginInfo);

#if defined(__cplusplus)
}
#endif

#endif /* TransportImplH__ */
