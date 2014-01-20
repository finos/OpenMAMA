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
 * @param transport  The mamaTransport to extract the bridge transport from.
 *
 * @return qpidTransportBridge* associated with the mamaTransport.
 */
qpidTransportBridge*
qpidBridgeMamaTransportImpl_getTransportBridge (mamaTransport transport);

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
