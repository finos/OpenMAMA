/* $Id: dqpublishermanager.c,v 1.2.22.4 2011/08/10 14:53:25 nicholasmarriott Exp $
 *
 * OpenMAMA: The open middleware agnostic messaging API
 * Copyright (C) 2011 NYSE Inc.
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

#include "mama/mama.h"
#include "subscriptionimpl.h"

#include "mama/dqpublishermanager.h"
#include "mama/dqpublisher.h"
#include "mama/publisher.h"

#include "mama/subscmsgtype.h"
#include "mama/clientmanage.h"

#include "wombat/wtable.h"
#include <string.h>

#define NUM_BUCKETS 11


/*Main mamaPublisher structure for the API*/
typedef struct mamaDQPublisherManagerImpl_
{
    wtable_t                               mPublisherMap;

    mamaMsg                             mRefreshResponseMsg;
    mamaMsg                             mNoSubscribersMsg;
    mamaMsg                             mSyncRequestMsg;

    mamaSubscription                    mSubscription;
    mamaPublisher                       mPublisher;
    mamaInbox                           mInbox;

    mamaMsgStatus                       mStatus;
    uint64_t                            mSenderId;
    mama_seqnum_t                       mSeqNum;

    mamaDQPublisherManagerCallbacks        mUserCallbacks;

    mamaTransport                          mTransport;
    mamaQueue                           mQueue;
    char *                              mNameSpace;
    void*                               mClosure;

} mamaDQPublisherManagerImpl;


static void MAMACALLTYPE
inboxMsgCb (mamaMsg msg, void *closure)
{
    const char**        topics         = NULL;
    const mama_i32_t*    types         = NULL;
    size_t                 resultLen     = 0;
    mamaPublishTopic*     info        = NULL;
    int                 i            = 0;
    mama_status          status         = MAMA_STATUS_NOT_FOUND;
    mamaDQPublisherManagerImpl* impl = (mamaDQPublisherManagerImpl*) (closure);

    if ((status = mamaMsg_getVectorString (msg,NULL, MAMA_SYNC_TOPICS_ID, 
                    &topics, &resultLen)) != MAMA_STATUS_OK)
    {
        impl->mUserCallbacks.onError ((mamaDQPublisherManager)impl, status,
                "Unknown Msg", msg);
        return;
    }

    if ((status = mamaMsg_getVectorI32 (msg, NULL, MAMA_SYNC_TYPES_ID, &types, 
                    &resultLen)) != MAMA_STATUS_OK)
    {
        impl->mUserCallbacks.onError ((mamaDQPublisherManager)impl,
                status, "Unknown Msg", msg);
        return;
    }

    for (i=0;i<resultLen;i++)
    {
        if ((info  = wtable_lookup (impl->mPublisherMap , ( topics[i]))))
            impl->mUserCallbacks.onRequest ((mamaDQPublisherManager)impl,
                    info, types[i],MAMA_SUBSC_RESUBSCRIBE, msg);
        else
            impl->mUserCallbacks.onNewRequest ((mamaDQPublisherManager)impl,
                    topics[i], types[i], MAMA_SUBSC_RESUBSCRIBE,  msg);
    }
}

static void MAMACALLTYPE
inboxErrorCb (mama_status status, void *closure)
{
    mamaDQPublisherManagerImpl* impl = (mamaDQPublisherManagerImpl*) (closure);

    impl->mUserCallbacks.onError ((mamaDQPublisherManager)impl, status,
            "Send sync failure", NULL);
}

void MAMACALLTYPE
dqPublisherImplCreateCb (mamaSubscription subsc, void* closure)
{
    mamaDQPublisherManagerImpl* impl = (mamaDQPublisherManagerImpl*) (closure);

    impl->mUserCallbacks.onCreate ((mamaDQPublisherManager)impl);
}


void MAMACALLTYPE
dqPublisherImplErrorCb (mamaSubscription subsc,
                        mama_status      status,
                        void*            platformError,
                        const char*      subject,
                        void*            closure)
{
    mamaDQPublisherManagerImpl* impl = NULL;

    if (closure)
        impl = (mamaDQPublisherManagerImpl*) (closure);

    impl->mUserCallbacks.onError ((mamaDQPublisherManager)impl, status, subject, NULL);
}

void MAMACALLTYPE
dqPublisherImplMsgCb (mamaSubscription subsc,
                      mamaMsg          msg,
                      void*            closure,
                      void*            itemClosure)
{
    mama_i32_t                 msgType = 0;
    mama_i32_t                subType = 0;
    const char*                symbol    = NULL;
    mama_status             status  = MAMA_STATUS_NOT_FOUND;
    mamaPublishTopic*         info    = NULL;
    mamaDQPublisherManagerImpl* impl = (mamaDQPublisherManagerImpl*) (closure);


    if (mamaMsg_getI32 (msg, MamaFieldSubscriptionType.mName, 
                MamaFieldSubscriptionType.mFid, &subType) == MAMA_STATUS_OK)
    {
        if (mamaMsg_getString (msg, MamaFieldSubscSymbol.mName, 
                    MamaFieldSubscSymbol.mFid, &symbol) != MAMA_STATUS_OK)
        {
            if (mamaMsg_getSendSubject (msg, &symbol) != MAMA_STATUS_OK)
            {
                impl->mUserCallbacks.onError ((mamaDQPublisherManager)impl, 
                        status, "No symbol", msg);
                return;
            }
        }

        if (mamaMsg_getI32 (msg, MamaFieldSubscMsgType.mName, 
                    MamaFieldSubscMsgType.mFid, &msgType) != MAMA_STATUS_OK)
        {
            impl->mUserCallbacks.onError ((mamaDQPublisherManager)impl, status, 
                    "NO msg type", msg);
            return;
        }

        if ((info  = wtable_lookup (impl->mPublisherMap, (symbol))))
        {
            switch (msgType)
            {
                case MAMA_SUBSC_REFRESH:
                    if (!impl->mRefreshResponseMsg)
                    {
                        mamaMsg_create(&impl->mRefreshResponseMsg);
                        mamaMsg_addU8(impl->mRefreshResponseMsg, NULL,
                                MamaFieldMsgStatus.mFid, MAMA_MSG_STATUS_MISC);
                        mamaMsg_addU8(impl->mRefreshResponseMsg, NULL,
                                MamaFieldMsgType.mFid, MAMA_MSG_TYPE_REFRESH);

                    }
                    mamaDQPublisher_send(info->pub, impl->mRefreshResponseMsg);
                    impl->mUserCallbacks.onRefresh((mamaDQPublisherManager)impl,
                                                    info, subType, msgType, msg );
                    break;

                default:
                    impl->mUserCallbacks.onRequest ((mamaDQPublisherManager)impl,
                                                    info, subType, msgType, msg);
                    break;
            }
        }
        else
        {
            impl->mUserCallbacks.onNewRequest ((mamaDQPublisherManager)impl,
                                                symbol,
                                                subType,
                                                msgType,
                                                msg);
        }

    }
}


mama_status mamaDQPublisherManager_allocate(mamaDQPublisherManager* result )
{
    mamaDQPublisherManagerImpl* impl  = NULL;

    impl = (mamaDQPublisherManagerImpl*)
        calloc (1, sizeof (mamaDQPublisherManagerImpl));

    if (!impl) return MAMA_STATUS_NOMEM;
    impl->mSenderId = mamaSenderId_getSelf ();
    impl->mStatus = MAMA_MSG_STATUS_OK;
    impl->mSeqNum = 1;

    *result = impl;

    return MAMA_STATUS_OK;
}

void* mamaDQPublisherManager_getClosure (mamaDQPublisherManager manager)
{
    mamaDQPublisherManagerImpl* impl  = (mamaDQPublisherManagerImpl*) manager;

    if(NULL != impl)
        return impl->mClosure;
    else
        return NULL;
}


mama_status mamaDQPublisherManager_create (
        mamaDQPublisherManager manager,
        mamaTransport transport,
        mamaQueue  queue,
        const mamaDQPublisherManagerCallbacks*   callback,
        const char* sourcename,
        const char* root,
        void * closure)
{
    char topic[80];
    mamaDQPublisherManagerImpl* impl  = (mamaDQPublisherManagerImpl*) manager;
    static mamaMsgCallbacks basicSubscCbs =
    {
        dqPublisherImplCreateCb,
        dqPublisherImplErrorCb,
        dqPublisherImplMsgCb,
        NULL
    };

    impl->mUserCallbacks = *callback;
    impl->mTransport = transport;
    impl->mQueue = queue;
    impl->mClosure = closure;

    strcpy(topic, root);
    impl->mNameSpace =  strdup(sourcename);
    strcat(topic, ".");
    strcat(topic, sourcename);

    mamaSubscription_allocate (&impl->mSubscription);    
    mamaSubscription_createBasic (impl->mSubscription,
                                   transport,
                                   queue,
                                   &basicSubscCbs,
                                   topic,
                                   impl);

    mamaPublisher_create (&impl->mPublisher,
                   transport,
                   MAMA_CM_TOPIC,
                   NULL,
                   NULL);

    impl->mPublisherMap =  wtable_create  (topic, NUM_BUCKETS);

    return MAMA_STATUS_OK;
}

void mamaDQPublisherManager_destroy (mamaDQPublisherManager manager)
{
    mamaDQPublisherManagerImpl* impl  = (mamaDQPublisherManagerImpl*) manager;

    wtable_destroy ( impl->mPublisherMap );
}

mama_status mamaDQPublisherManager_addPublisher (
        mamaDQPublisherManager manager, 
        const char *symbol, 
        mamaDQPublisher pub, 
        void * cache)
{
    mamaPublishTopic* newTopic = NULL;
    mamaDQPublisherManagerImpl* impl  = (mamaDQPublisherManagerImpl*) manager;

    if (wtable_lookup (impl->mPublisherMap , ( char* )symbol))
        return (MAMA_STATUS_INVALID_ARG);


    newTopic =  (mamaPublishTopic*) wtable_lookup  (impl->mPublisherMap, (char*)symbol);


    if (newTopic)
    {
        newTopic =  (mamaPublishTopic*) calloc (1, sizeof (mamaPublishTopic));

        newTopic->pub = pub;
        newTopic->cache = cache;
        newTopic->symbol = strdup(symbol);

        if (wtable_insert  (impl->mPublisherMap, (char*)symbol, newTopic) != 1)
            return MAMA_STATUS_INVALID_ARG;
    }
    else
        return MAMA_STATUS_INVALID_ARG;

    return MAMA_STATUS_OK;
}

mama_status mamaDQPublisherManager_removePublisher (
        mamaDQPublisherManager manager, 
        const char *symbol, 
        mamaDQPublisher* pub)
{
    mamaDQPublisherManagerImpl* impl  = (mamaDQPublisherManagerImpl*) manager;
    mamaPublishTopic* newTopic = NULL;


    newTopic = (mamaPublishTopic*)wtable_lookup (impl->mPublisherMap, 
            (char*)symbol);

    if (newTopic)
    {
        wtable_remove  (impl->mPublisherMap, (char*)symbol);

        *pub = newTopic->pub;
        free  ((void*)newTopic->symbol);
        free  ((void*)newTopic);
        return MAMA_STATUS_OK;
    }

    return (MAMA_STATUS_INVALID_ARG);
}

mama_status mamaDQPublisherManager_createPublisher (
        mamaDQPublisherManager manager, 
        const char *symbol, 
        void * cache, 
        mamaDQPublisher *newPublisher)
{
    mamaDQPublisherManagerImpl* impl  = (mamaDQPublisherManagerImpl*) manager;
    mamaPublishTopic* newTopic = NULL;
    mama_status status = MAMA_STATUS_OK;
    char topic[80];

    newTopic =  (mamaPublishTopic*)wtable_lookup (impl->mPublisherMap, (char*)symbol);

    if (!newTopic)
    {
        if ((status = mamaDQPublisher_allocate(newPublisher)) == MAMA_STATUS_OK)
        {
            newTopic =  (mamaPublishTopic*) calloc (1, sizeof (mamaPublishTopic));

            newTopic->pub = *newPublisher;
            newTopic->cache = cache;
            newTopic->symbol = strdup(symbol);

            strcpy (topic, impl->mNameSpace);
            strcat (topic, ".");
            strcat (topic, symbol);

            if ((status = mamaDQPublisher_create(*newPublisher, 
                            impl->mTransport, topic)) != MAMA_STATUS_OK)
            {
                return status;
            }

            if (wtable_insert  (impl->mPublisherMap, (char*)symbol, newTopic) != 1)
            {
                mamaDQPublisher_destroy(*newPublisher);
                free  ((void*)newTopic->symbol);
                free  ((void*)newTopic);
                return status;
            }
            return MAMA_STATUS_OK;
        }
        return status;
    }

    return (MAMA_STATUS_INVALID_ARG);
}

mama_status mamaDQPublisherManager_destroyPublisher (
        mamaDQPublisherManager
        manager, 
        const char *symbol)
{
    mamaDQPublisherManagerImpl* impl  = (mamaDQPublisherManagerImpl*) manager;
    mamaPublishTopic* newTopic = NULL;

    if ((newTopic =  wtable_lookup (impl->mPublisherMap , ( char* )symbol)))
        return (MAMA_STATUS_INVALID_ARG);

    mamaDQPublisher_destroy(newTopic->pub);

    free  ((void*)newTopic->symbol);
    free  ((void*)newTopic);
    return MAMA_STATUS_OK;
}




void mamaDQPublisherManager_setStatus (
        mamaDQPublisherManager manager, 
        mamaMsgStatus  status)
{
    mamaDQPublisherManagerImpl* impl  = (mamaDQPublisherManagerImpl*) manager;
    impl->mStatus = status;
}

void mamaDQPublisherManager_setSenderId (
        mamaDQPublisherManager manager, 
        uint64_t  senderid)
{
    mamaDQPublisherManagerImpl* impl  = (mamaDQPublisherManagerImpl*) manager;
    impl->mSenderId = senderid;
}

void mamaDQPublisherManager_setSeqNum (
        mamaDQPublisherManager manager, 
        mama_seqnum_t num)
{
    mamaDQPublisherManagerImpl* impl  = (mamaDQPublisherManagerImpl*) manager;
    impl->mSeqNum=num;
}

mama_status mamaDQPublisherManager_sendSyncRequest (
        mamaDQPublisherManager manager, 
        mama_u16_t nummsg, 
        mama_f64_t delay, 
        mama_f64_t duration)
{
    mamaDQPublisherManagerImpl* impl  = (mamaDQPublisherManagerImpl*) manager;
    if (!impl->mSyncRequestMsg)
    {
        mamaInbox_create (&impl->mInbox,
                                    impl->mTransport,
                                    impl->mQueue,
                               inboxMsgCb,
                               inboxErrorCb,
                               impl);
        mamaMsg_create(&impl->mSyncRequestMsg  );

        mamaMsg_addU16(impl->mSyncRequestMsg, NULL, MAMA_CM_COMMAND_ID, 1);
        mamaMsg_addU16(impl->mSyncRequestMsg, NULL, MAMA_SYNC_TOPICS_PER_MSG_ID, nummsg);
        mamaMsg_addF64(impl->mSyncRequestMsg, NULL, MAMA_SYNC_RESPONSE_DELAY_ID, delay);
        mamaMsg_addF64(impl->mSyncRequestMsg, NULL, MAMA_SYNC_RESPONSE_DURATION_ID, duration);
        mamaMsg_addString(impl->mSyncRequestMsg, NULL, MAMA_SYNC_SOURCE_ID, impl->mNameSpace);
    }

    return mamaPublisher_sendFromInbox (impl->mPublisher,
                                        impl->mInbox,
                                        impl->mSyncRequestMsg);
}

mama_status mamaDQPublisherManager_sendNoSubscribers (
        mamaDQPublisherManager manager, 
        const char *symbol)
{
    mamaDQPublisherManagerImpl* impl  = (mamaDQPublisherManagerImpl*) manager;
    if (!impl->mNoSubscribersMsg)
    {
        mamaMsg_create(&impl->mNoSubscribersMsg  );
    }

    mamaPublisher_send (impl->mPublisher, impl->mNoSubscribersMsg);

    return MAMA_STATUS_OK;
}
