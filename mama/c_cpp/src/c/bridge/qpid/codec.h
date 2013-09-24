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

#ifndef MAMA_BRIDGE_QPID_CODEC_H__
#define MAMA_BRIDGE_QPID_CODEC_H__


/*=========================================================================
  =                             Includes                                  =
  =========================================================================*/

#include "../../bridge.h"
#include <proton/message.h>


#if defined(__cplusplus)
extern "C" {
#endif


/*=========================================================================
  =                         Public functions                              =
  =========================================================================*/

/**
 * This function is responsible for preparing a pn_message_t for publishing by
 * accessing all meta data collected by the bridge message to this point and
 * inserting it into the pn_message_t supplied. This is one of the last steps
 * taken with a qpidBridgeMessage prior to publishing.
 *
 * @param bridgeMessage This is the qpid bridge message containing the required
 *                      meta data for publishing.
 * @param target        This is the original mamaMsg containing the payload
 *                      which is to be published
 * @param protonMessage This is the proton message supplied by the publisher.
 *                      Note that if the mamaMsg is a qpid payload message, the
 *                      provided pn_message_t will be replaced by the one used
 *                      in the mamaMsg payload. If the mamaMsg is not a qpid
 *                      payload message, then the provided pn_message_t will be
 *                      used and returned.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidBridgeMsgCodec_pack     (msgBridge       bridgeMessage,
                             mamaMsg         target,
                             pn_message_t**  protonMessage);

/**
 * This function is responsible for reading a pn_message_t which has just been
 * received and populating the bridge message provided with all the meta data
 * extracted from it. It is also responsible for populating the mamaMsg
 * provided with the extracted payload.
 *
 * @param bridgeMessage This is the qpid bridge message to write the
 *                      pn_message_t meta data to.
 * @param target        This is the mamaMsg to be updated with the payload which
 *                      has been extracted from the pn_message_t provided.
 * @param protonMessage This is the proton message which has just been received
 *                      and is now being presented for processing.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidBridgeMsgCodec_unpack   (msgBridge       bridgeMessage,
                             mamaMsg         target,
                             pn_message_t*   protonMessage);

#if defined(__cplusplus)
}
#endif

#endif /* MAMA_BRIDGE_QPID_CODEC_H__ */
