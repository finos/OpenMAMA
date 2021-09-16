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

#ifndef MAMA_BRIDGE_OMPB_TRANSPORT_H__
#define MAMA_BRIDGE_OMPB_TRANSPORT_H__


/*=========================================================================
  =                             Includes                                  =
  =========================================================================*/

#include <mama/mama.h>
#include <bridge.h>

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * Register a subscription on this transport for the given source.topic
 * identifier as a string.
 *
 * @param transport     The transport bridge to use
 * @param identifier    The subscription string in source.topic format
 * @param sub           The mama subscription to register
 */
void
ompbBridgeMamaTransportImpl_registerSubscription(transportBridge transport,
                                                 const char* identifier,
                                                 mamaSubscription sub);

#if defined(__cplusplus)
}
#endif

#endif /* MAMA_BRIDGE_OMPB_TRANSPORT_H__*/
