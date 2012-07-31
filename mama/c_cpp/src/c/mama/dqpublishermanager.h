/* $Id: dqpublishermanager.h,v 1.2.22.3 2011/08/10 14:53:28 nicholasmarriott Exp $
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

#ifndef MAMA_DQPUBLISHERMANAGER_H__
#define MAMA_DQPUBLISHERMANAGER_H__

#include "mama/types.h"
#include "wombat/wConfig.h"

#if defined( __cplusplus )
extern "C"
{
#endif /* defined( __cplusplus ) */



typedef struct mamaPublishTopic_
{
    const char*     symbol;
    mamaDQPublisher pub;
    void*           cache;
}mamaPublishTopic;

typedef void (MAMACALLTYPE *mamaDQPublisherCreateCB)(
        mamaDQPublisherManager manager);

typedef void (MAMACALLTYPE *mamaDQPublisherNewRequestCB)(
        mamaDQPublisherManager manager,
        const char*        symbol,
        short  subType,
        short    msgType,
        mamaMsg     msg);

typedef void (MAMACALLTYPE *mamaDQPublisherRequestCB)(
        mamaDQPublisherManager manager,
        mamaPublishTopic* info,
        short  subType,
        short    msgType,
        mamaMsg     msg);

typedef void (MAMACALLTYPE *mamaDQPublisherRefreshCB)(
        mamaDQPublisherManager manager,
        mamaPublishTopic* info,
        short  subType,
        short    msgType,
        mamaMsg     msg);

typedef void (MAMACALLTYPE *mamaDQPublisherErrorCB)(
        mamaDQPublisherManager manager,
        mama_status  status,
        const char*        errortxt,
        mamaMsg     msg);


typedef struct mamaDQPublisherManagerCallbacks_
{
    mamaDQPublisherCreateCB         onCreate;
    mamaDQPublisherNewRequestCB     onNewRequest;
    mamaDQPublisherRequestCB        onRequest;
    mamaDQPublisherRefreshCB        onRefresh;
    mamaDQPublisherErrorCB          onError;
} mamaDQPublisherManagerCallbacks;



/**
 * Create a mama publisher for the corresponding transport. If the transport
 * is a marketdata transport, as opposed to a "basic" transport, the topic
 * corresponds to the symbol. For a basic transport, the source and root get
 * ignored.
 *
 * @param result A pointer to hold the resulting mamaPublisher.
 * @param tport The transport.
 * @param symbol Symbol on which to publish.
 * @param source The source for market data publishers. (e.g. source.symbol)
 * @param root The root for market data publishers. Used internally.
 */
MAMAExpDLL
extern mama_status
mamaDQPublisherManager_allocate (mamaDQPublisherManager* result);

/**
 * Send a message from the specified publisher.
 *
 * @param publisher The publisher from which to send a message.
 * @param msg The mamaMsg to send.
 */
MAMAExpDLL
extern mama_status
mamaDQPublisherManager_create (
        mamaDQPublisherManager dqPublisher,mamaTransport transport,
        mamaQueue  queue,
        const mamaDQPublisherManagerCallbacks*   callback,
        const char* sourcename,
        const char* root,
        void* closure);


MAMAExpDLL
extern void
mamaDQPublisherManager_destroy (mamaDQPublisherManager manager);

MAMAExpDLL
extern mama_status
mamaDQPublisherManager_addPublisher (
        mamaDQPublisherManager manager, 
        const char *symbol, 
        mamaDQPublisher pub, 
        void * cache);

MAMAExpDLL
extern mama_status
mamaDQPublisherManager_removePublisher (
        mamaDQPublisherManager manager, 
        const char *symbol, 
        mamaDQPublisher* pub);

MAMAExpDLL
extern mama_status
mamaDQPublisherManager_createPublisher (
        mamaDQPublisherManager manager, 
        const char *symbol, 
        void * cache, 
        mamaDQPublisher *newPublisher);

MAMAExpDLL
extern mama_status
mamaDQPublisherManager_destroyPublisher (
        mamaDQPublisherManager manager, 
        const char *symbol);


MAMAExpDLL
extern void
mamaDQPublisherManager_setStatus (
        mamaDQPublisherManager manager,
        mamaMsgStatus  status);

MAMAExpDLL
extern void*
mamaDQPublisherManager_getClosure (mamaDQPublisherManager manager);

MAMAExpDLL
extern void
mamaDQPublisherManager_setSenderId (
        mamaDQPublisherManager manager,
        uint64_t  senderid);

MAMAExpDLL
extern void
mamaDQPublisherManager_setSeqNum (
        mamaDQPublisherManager manager,
        mama_seqnum_t num);

MAMAExpDLL
extern mama_status
mamaDQPublisherManager_sendSyncRequest (
        mamaDQPublisherManager manager,
        mama_u16_t nummsg, 
        mama_f64_t delay, 
        mama_f64_t duration);


MAMAExpDLL
extern mama_status
mamaDQPublisherManager_sendNoSubscribers (
        mamaDQPublisherManager manager,
        const char *symbol);

#if defined( __cplusplus )
}
#endif /* defined( __cplusplus ) */

#endif /* MAMA_DQPUBLISHERMANAGER_H__*/
