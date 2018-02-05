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

#ifndef MAMA_BRIDGE_QPID_INBOX_H__
#define MAMA_BRIDGE_QPID_INBOX_H__


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
 * This function will return the topic on which to reply in order to reach the
 * supplied inbox.
 *
 * @param inboxBridge The inbox implementation to extract the reply subject from.
 *
 * @return const char* containing the subject on which to reply.
 */
const char*
qpidBridgeMamaInboxImpl_getReplySubject (inboxBridge inbox);


#if defined(__cplusplus)
}
#endif

#endif /* MAMA_BRIDGE_QPID_INBOX_H__ */
