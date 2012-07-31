/* $Id: dqpublisher.h,v 1.2.22.4 2011/08/10 14:53:28 nicholasmarriott Exp $
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

#ifndef MAMA_DQPUBLISHER_H__
#define MAMA_DQPUBLISHER_H__

#include "mama/types.h"
#include "wombat/wConfig.h"

#if defined( __cplusplus )
extern "C"
{
#endif /* defined( __cplusplus ) */

/**
 * Responsible for publishing mama marketdata messages.
 */


/**
 * Allocate a mamaDQPublisher.
 *
 * @param result A pointer to hold the mamaDQPublisher.
 */
MAMAExpDLL
extern mama_status
mamaDQPublisher_allocate (mamaDQPublisher* result );

/**
 * Create a mamaDQPublisher for the corresponding transport.
 *
 * @param pub A pointer to the mamaDQPublisher.
 * @param transport The transport.
 * @param topic Symbol on which to publish.
 */
MAMAExpDLL
extern mama_status
mamaDQPublisher_create (mamaDQPublisher pub, mamaTransport transport,
                 	 	 const char* topic);

/**
 * Send a message.
 *
 * @param publisher The publish from which to send the message.
 * @param msg The mamaMsg to send.
 */
MAMAExpDLL
extern mama_status
mamaDQPublisher_send (mamaDQPublisher pub, mamaMsg msg);


/*
 * Send a p2p message reply.
 *
 * @param publisher The mamaPublisher from which to send the message.
 * @param request The mamaMsg being responded to.
 * @param reply The mamaMsg to be send
 */
MAMAExpDLL
extern mama_status
mamaDQPublisher_sendReply (mamaDQPublisher pub, mamaMsg request,
                                     mamaMsg reply) ;


MAMAExpDLL
extern mama_status
mamaDQPublisher_sendReplyWithHandle (mamaDQPublisher pub,
                                     mamaMsgReply  replyAddress,
                                     mamaMsg reply);

MAMAExpDLL
extern void
mamaDQPublisher_destroy (mamaDQPublisher pub);



MAMAExpDLL
extern void
mamaDQPublisher_setStatus (mamaDQPublisher pub, mamaMsgStatus  status);

MAMAExpDLL
extern void
mamaDQPublisher_setSenderId (mamaDQPublisher pub, uint64_t  senderid);

MAMAExpDLL
extern void
mamaDQPublisher_setSeqNum (mamaDQPublisher pub, mama_seqnum_t num);

MAMAExpDLL
extern void
mamaDQPublisher_setClosure (mamaDQPublisher pub, void*  closure);

MAMAExpDLL
extern void*
mamaDQPublisher_getClosure (mamaDQPublisher pub);

MAMAExpDLL
extern void
mamaDQPublisher_setCache (mamaDQPublisher pub, void*  cache);

MAMAExpDLL
extern void*
mamaDQPublisher_getCache (mamaDQPublisher pub);

#if defined( __cplusplus )
}
#endif /* defined( __cplusplus ) */

#endif /* MAMA_PUBLISHER_H_*/
