/* $Id: inbox.h,v 1.8.14.1.2.1.4.4 2011/09/01 16:34:38 emmapollock Exp $
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

#ifndef MAMA_INBOX_H__
#define MAMA_INBOX_H__ 

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * Inbox structure for processing point to point messaging.
 */

/**
 * Invoked in response to a p2p message being received.
 *
 * @param msg The mamaMsg received in the p2p response.
 * @param closure The user supplied data passed to mamaInbox_create()
 */
typedef void (MAMACALLTYPE *mamaInboxMsgCallback) (mamaMsg msg, void *closure);

/**
 * NB. Not currently used.
 * Invoked when an error is encountered during p2p messaging.
 * 
 * @param status The mama_status describing the error condition.
 * @param closure The user supplied data passed to mamaInbox_create()
 */
typedef void (MAMACALLTYPE *mamaInboxErrorCallback) ( mama_status status, void *closure);

/**
 * Invoked whenever the inbox has been destroyed.
 * 
 * @param inbox The mama inbox.
 * @param closure The user supplied data passed to mamaInbox_create()
 */
typedef void (MAMACALLTYPE *mamaInboxDestroyCallback) ( mamaInbox inbox, void *closure);

/**
 * Creates an inbox and stores at the address specified by the calling client.
 *
 * @param inbox Pointer to the inbox which will be created
 * @param transport The mamaTransport being used.
 * @param queue The mamaQueue to use.
 * @param msgCB Invoked for any point to point responses.
 * @param errorCB For future use. Not currently used.
 * @param closure User supplied data to be passed back in callbacks.
 *
 * @return mama_status MAMA_STATUS_OK if the function is successful.
 */
MAMAExpDLL
extern mama_status
mamaInbox_create (mamaInbox*             inbox,
                  mamaTransport          transport,
                  mamaQueue              queue,
                  mamaInboxMsgCallback   msgCB,
                  mamaInboxErrorCallback errorCB,
                  void*                  closure);

/**
 * Creates an inbox and stores at the address specified by the calling client.
 *
 * @param inbox Pointer to the inbox which will be created
 * @param transport The mamaTransport being used.
 * @param queue The mamaQueue to use.
 * @param msgCB Invoked for any point to point responses.
 * @param errorCB For future use. Not currently used.
 * @param closure User supplied data to be passed back in callbacks.
 *
 * @return mama_status MAMA_STATUS_OK if the function is successful.
 */
MAMAExpDLL
extern mama_status
mamaInbox_create2 (mamaInbox*               inbox,
                   mamaTransport            transport,
                   mamaQueue                queue,
                   mamaInboxMsgCallback     msgCB,
                   mamaInboxErrorCallback   errorCB,
                   mamaInboxDestroyCallback onInboxDestroyed,
                   void*                    closure);

/**
 * Destroy the supplied inbox structure.
 * Note that this function is asynchronous and is only guaranteed to have finished
 * whenever the onInboxDestroyed function passed to the mamaInbox_create2 has been
 * called.
 *
 * @param inbox The mamaInbox to be destroyed.
 *
 * @return mama_Status MAMA_STATUS_OK if function call successful.
 */
MAMAExpDLL
extern mama_status 
mamaInbox_destroy (mamaInbox inbox);

#if defined(__cplusplus)
}
#endif


#endif /* MAMA_INBOX_H__ */
