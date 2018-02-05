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

#ifndef MAMA_BRIDGE_QPID_MSG_H__
#define MAMA_BRIDGE_QPID_MSG_H__


/*=========================================================================
  =                             Includes                                  =
  =========================================================================*/

#include "../../bridge.h"
#include <proton/message.h>
#include "qpiddefs.h"


#if defined(__cplusplus)
extern "C" {
#endif


/*=========================================================================
  =                  Public implementation functions                      =
  =========================================================================*/

/**
 * This will return true if the bridge message supplied is not null and has
 * been created in the past
 *
 * @param msg    The bridge message to examine.
 * @param result Pointer to an unsigned int to populate with 1 if valid and 0
 *               if invalid or null.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidBridgeMamaMsgImpl_isValid                 (msgBridge    msg,
                                               uint8_t*     result);
/**
 * This will set the bridge's internal message type according to the value
 * provided.
 *
 * @param msg    The bridge message to update.
 * @param type   The new bridge message type (e.g. QPID_MSG_PUB_SUB).
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidBridgeMamaMsgImpl_setMsgType              (msgBridge    msg,
                                               qpidMsgType  type);

/**
 * This will get the bridge's internal message type.
 *
 * @param msg    The bridge message to examine.
 * @param type   Pointer to populate with the value of the bridge message type
 *               (e.g. QPID_MSG_PUB_SUB).
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidBridgeMamaMsgImpl_getMsgType              (msgBridge    msg,
                                               qpidMsgType* type);

/**
 * This will set the bridge's internal message inbox name according to the value
 * provided.
 *
 * @param msg    The bridge message to update.
 * @param type   The new inbox name.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidBridgeMamaMsgImpl_setInboxName            (msgBridge    msg,
                                               const char*  value);

/**
 * This will get the bridge's inbox name.
 *
 * @param msg    The bridge message to examine.
 * @param type   Pointer to populate with the value of the inbox name.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidBridgeMamaMsgImpl_getInboxName            (msgBridge    msg,
                                               char**       value);

/**
 * This will set the bridge's internal message replyTo according to the value
 * provided. Note the replyTo field is used in request reply to advise which
 * url to send replies to.
 *
 * @param msg    The bridge message to update.
 * @param type   The new reply to URL.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidBridgeMamaMsgImpl_setReplyTo              (msgBridge    msg,
                                               const char*  value);

/**
 * This will get the bridge's internal message replyTo string. Note the replyTo
 * field is used in request reply to advise which url to send replies to.
 *
 * @param msg    The bridge message to examine.
 * @param type   Pointer to populate with the value of the reply to URL.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidBridgeMamaMsgImpl_getReplyTo              (msgBridge    msg,
                                               char**       value);

/**
 * This will set the bridge's target subject according to the value provided.
 * Note that the target subject is used in initial message responses to allow
 * the listener to determine which symbol this initial is in reference to.
 *
 * @param msg    The bridge message to update.
 * @param type   The new target subject.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidBridgeMamaMsgImpl_setTargetSubject        (msgBridge    msg,
                                               const char*  value);

/**
 * This will get the bridge's target subject. Note that the target subject is
 * used in initial message responses to allow the listener to determine which
 * symbol this initial is in reference to.
 *
 * @param msg    The bridge message to examine.
 * @param type   Pointer to populate with the value of the target subject.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidBridgeMamaMsgImpl_getTargetSubject        (msgBridge    msg,
                                               char**       value);

/**
 * This will set the bridge's destination according to the value provided.
 * This is the URL to send the message to.
 *
 * @param msg    The bridge message to update.
 * @param type   The new destination url (e.g. amqp://x.x.x.x/LISTENER).
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidBridgeMamaMsgImpl_setDestination          (msgBridge    msg,
                                               const char*  value);

/**
 * This will get the bridge's destination. This is the URL to send the message
 * to.
 *
 * @param msg    The bridge message to examine.
 * @param type   Pointer to populate with the value of the destination URL.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidBridgeMamaMsgImpl_getDestination          (msgBridge    msg,
                                               char**       value);

/**
 * This will get the bridge's send subject which is currently the topic name.
 * Note the setter for this is an interface bridge function.
 *
 * @param msg    The bridge message to examine.
 * @param type   Pointer to populate with the value of the send subject.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidBridgeMamaMsgImpl_getSendSubject          (msgBridge    msg,
                                               char**       value);

/**
 * This will get the inbox name from the reply handle specified.
 *
 * @param msg    The bridge message to examine.
 * @param type   Pointer to populate with reply handle's inbox name.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidBridgeMamaMsgReplyHandleImpl_getInboxName (void*       replyHandle,
                                               char**      value);

/**
 * This will set the inbox name for the reply handle specified.
 *
 * @param msg    The bridge message to update.
 * @param type   The inbox name to update the reply handle with.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidBridgeMamaMsgReplyHandleImpl_setInboxName (void*       replyHandle,
                                               const char* value);

/**
 * This will get the reply to URL from the reply handle specified.
 *
 * @param msg    The bridge message to examine.
 * @param type   Pointer to populate with reply handle's reply to URL
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidBridgeMamaMsgReplyHandleImpl_getReplyTo   (void*       replyHandle,
                                               char**      value);

/**
 * This will set the reply to URL for the reply handle specified.
 *
 * @param msg    The bridge message to update.
 * @param type   The reply to url to update the reply handle with.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidBridgeMamaMsgReplyHandleImpl_setReplyTo   (void*       replyHandle,
                                               const char* value);

/**
 * This will create a bridge message but will not create a parent with it.
 *
 * @param msg    The bridge message to create.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidBridgeMamaMsgImpl_createMsgOnly 		  (msgBridge*  msg);

#if defined(__cplusplus)
}
#endif

#endif /* MAMA_BRIDGE_QPID_MSG_H__ */
