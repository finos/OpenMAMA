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

#ifndef MAMA_BRIDGE_QPID_TRANSPORT_H__
#define MAMA_BRIDGE_QPID_TRANSPORT_H__


/*=========================================================================
  =                             Includes                                  =
  =========================================================================*/

#include <stdlib.h>
#include <string.h>

#include <mama/mama.h>
#include <bridge.h>
#include "qpiddefs.h"

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * This is a simple convenience function to return a qpidTransportBridge
 * pointer based on the provided mamaTransport.
 *
 * @param transport     The mamaTransport to extract the bridge transport from.
 *
 * @return qpidTransportBridge* associated with the mamaTransport.
 */
qpidTransportBridge*
qpidBridgeMamaTransportImpl_getTransportBridge (mamaTransport transport);

/**
 * This is a simple convenience function to return the transport's distinct
 * uuid.
 *
 * @param transport     The mamaTransport to extract the uuid from.
 *
 * @return const char*  String representation of the UUID (memory still owned
 *                      by the transport bridge.
 */
const char*
qpidBridgeMamaTransportImpl_getUuid (transportBridge transport);

/**
 * This is a simple convenience function to return the transport's qpid bridge
 * type,
 *
 * @param transport     The transportBridge to extract the uuid from.
 *
 * @return qpidTransportType releating to the type of the qpid transport bridge.
 */
qpidTransportType
qpidBridgeMamaTransportImpl_getType (transportBridge transport);

/**
 * This is a simple convenience function to return the transport's outgoing
 * address (not format string expanded).
 *
 * @param transport     The transportBridge to extract the outgoing address from.
 *
 * @return const char*  String representation of the address (memory still owned
 *                      by the transport bridge.
 */
const char*
qpidBridgeMamaTransportImpl_getOutgoingAddress (transportBridge transport);

/**
 * This is a simple convenience function to return the transport's incoming
 * address (not format string expanded).
 *
 * @param transport     The transportBridge to extract the incoming address from.
 *
 * @return const char*  String representation of the address (memory still owned
 *                      by the transport bridge.
 */
const char*
qpidBridgeMamaTransportImpl_getIncomingAddress (transportBridge transport);

/**
 * This is a simple convenience function to return the transport's reply
 * address (not format string expanded).
 *
 * @param transport     The transportBridge to extract the reply address from.
 *
 * @return const char*  String representation of the address (memory still owned
 *                      by the transport bridge.
 */
const char*
qpidBridgeMamaTransportImpl_getReplyAddress (transportBridge transport);

/**
 * This advises the transport bridge about a known MAMA symbol namespace. This
 * allows flattened fully qualified topics (e.g. SOURCENAME.TOPIC) to be
 * detected and re-expanded where necessary (i.e. so we can tell the difference
 * between a MAMA basic subscription topic and a market data subscription for %S
 * and %s expansion in the URI).
 *
 * @param transport         The transportBridge to add a known MAMA symbol
 *                          namespace to.
 * @param symbolNamespace   The known MAMA symbol namespace to add.
 */
void
qpidBridgeMamaTransportImpl_addKnownMamaSymbolNamespace (transportBridge transport,
        const char* symbolNamespace);

/**
 * This is a simple convenience function to return a qpidTransportBridge
 * pointer based on the provided mamaTransport.
 *
 * @param transport         The transportBridge to check for the provided MAMA
 *                          symbol namespace.
 * @param symbolNamespace   The MAMA symbol namespace to check.
 *
 * @return mama_bool_t returning true if the provided symbolNamespace is a
 *                     confirmed MAMA symbol namespace as added to the
 *                     transport bridge using
 *                     qpidBridgeMamaTransportImpl_addKnownMamaSymbolNamespace.
 */
mama_bool_t
qpidBridgeMamaTransportImpl_isKnownMamaSymbolNamespace (transportBridge transport,
        const char* symbolNamespace);

/**
 * This is purely a debug function to dump to screen a snapshot of the status
 * of the transport's message pool.
 *
 * @param impl       The qpid transport bridge referring to a message pool.
 */
void
qpidBridgeMamaTransportImpl_dumpMessagePool (qpidTransportBridge* impl);

#if defined(__cplusplus)
}
#endif

#endif /* MAMA_BRIDGE_QPID_TRANSPORT_H__*/
