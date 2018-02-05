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

#ifndef MAMA_BRIDGE_QPID_PUBLISHER_H__
#define MAMA_BRIDGE_QPID_PUBLISHER_H__


/*=========================================================================
  =                             Includes                                  =
  =========================================================================*/

#include "../../bridge.h"


#if defined(__cplusplus)
extern "C" {
#endif

/*=========================================================================
  =                  Public implementation functions                      =
  =========================================================================*/

/**
 * For publishers that do not use the qpid bridge message, this allows them to
 * set the type meta data in the proton message to the one supplied. The caller
 * can then send a proton message directly over proton.
 *
 * @param message  The proton message to set the type for.
 * @param type     The type to set the proton message supplied to be.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
void
qpidBridgePublisherImpl_setMessageType (pn_message_t*   message,
                                        qpidMsgType     type);


#if defined(__cplusplus)
}
#endif

#endif /* MAMA_BRIDGE_QPID_PUBLISHER_H__ */
