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

#include "mama/mama.h"
#include "subscriptionimpl.h"

#include "mama/dqpublisher.h"
#include "mama/publisher.h"
#include "mama/msg.h"
#include "mama/msgfield.h"


#define MAX_DATE_STR    50

typedef struct mamaDQPublisherImpl_
{
    mamaPublisher   mPublisher;
    mamaMsgStatus   mStatus;
    uint64_t        mSenderId;
    mama_seqnum_t   mSeqNum;
    void*           mClosure;
    void*           mCache;
    mamaDateTime    mSendTime;
    char*           mSendTimeFormat;
} mamaDQPublisherImpl;


mama_status updateSendTime (mamaDQPublisher pub, mamaMsg msg);
mama_status updateSenderId (mamaDQPublisher pub, mamaMsg msg);

mama_status mamaDQPublisher_allocate (mamaDQPublisher* result)
{
    mamaDQPublisherImpl* impl  = NULL;

    impl = (mamaDQPublisherImpl*)calloc (1, sizeof (mamaDQPublisherImpl));
    if (!impl) return MAMA_STATUS_NOMEM;
        
    impl->mSenderId = mamaSenderId_getSelf ();
    impl->mStatus = MAMA_MSG_STATUS_OK;
    impl->mSeqNum = 1;

    impl->mSendTimeFormat = strdup("%T%;");

    *result = impl;

    return MAMA_STATUS_OK;
}

mama_status mamaDQPublisher_create (mamaDQPublisher pub, mamaTransport transport, 
                                         const char* topic)
{
    mamaDQPublisherImpl* impl = (mamaDQPublisherImpl*) (pub);
    mama_status status = MAMA_STATUS_OK;


    status = mamaPublisher_create (&impl->mPublisher,
                           transport, 
                           topic, 
                           NULL,
                           NULL);
    
    return status;
}


mama_status mamaDQPublisher_send (mamaDQPublisher pub, mamaMsg msg)
{     
    mamaDQPublisherImpl* impl = (mamaDQPublisherImpl*) (pub);
    mamaMsg modifableMsg = NULL;

    if (impl->mSeqNum != 0)
    {
        mamaMsg_getTempCopy (msg, &modifableMsg);
        switch (mamaMsgType_typeForMsg (modifableMsg))
        {
            case MAMA_MSG_TYPE_REFRESH :
            case MAMA_MSG_TYPE_SYNC_REQUEST :
            case MAMA_MSG_TYPE_MISC :
            case MAMA_MSG_TYPE_NOT_PERMISSIONED :
            case MAMA_MSG_TYPE_NOT_FOUND : 
                break;
            default:
                if(MAMA_STATUS_OK !=
                        mamaMsg_updateU8(modifableMsg,MamaFieldMsgStatus.mName,
                            MamaFieldMsgStatus.mFid, impl->mStatus))
                {
                   mamaMsg_updateI16(modifableMsg,MamaFieldMsgStatus.mName,
                           MamaFieldMsgStatus.mFid, impl->mStatus);
                }
                break;
        }
        mamaMsg_updateU32(modifableMsg, MamaFieldSeqNum.mName, MamaFieldSeqNum.mFid,
                impl->mSeqNum);
        impl->mSeqNum++;
    }
    
    if (impl->mSenderId != 0)
    {
        mamaMsg_getTempCopy (msg, &modifableMsg);
        updateSenderId(impl, modifableMsg);
    }

    if (impl->mSendTime)
        {
        mamaMsg_getTempCopy (msg, &modifableMsg);
        updateSendTime(impl, modifableMsg);
    }

    if (modifableMsg)
        return (mamaPublisher_send (impl->mPublisher, modifableMsg));
    else
        return (mamaPublisher_send (impl->mPublisher, msg));
} 

mama_status mamaDQPublisher_sendReply (mamaDQPublisher pub,
                                       mamaMsg request,
                                       mamaMsg reply)
{
    mamaDQPublisherImpl* impl = (mamaDQPublisherImpl*) (pub);
   
    if (impl->mSenderId != 0)
        updateSenderId(impl, reply);
        
    if (impl->mSeqNum != 0)
    {
        mamaMsg_updateU32(reply, MamaFieldSeqNum.mName, MamaFieldSeqNum.mFid,
                impl->mSeqNum);

        if(MAMA_STATUS_OK != mamaMsg_updateU8(reply,MamaFieldMsgStatus.mName,
                MamaFieldMsgStatus.mFid, impl->mStatus))
        {
            mamaMsg_updateI16(reply,MamaFieldMsgStatus.mName,
                MamaFieldMsgStatus.mFid, impl->mStatus);
        }
    }

    if (impl->mSendTime)
    	updateSendTime(impl, reply);

    return (mamaPublisher_sendReplyToInbox (impl->mPublisher, request, reply));
}

mama_status mamaDQPublisher_sendReplyWithHandle (mamaDQPublisher pub,
                                                   mamaMsgReply  replyAddress,
                                                mamaMsg reply)
{
    mamaDQPublisherImpl* impl = (mamaDQPublisherImpl*) (pub);

    if (impl->mSenderId != 0)
        updateSenderId(impl, reply);

    if (impl->mSeqNum != 0)
    {
        mamaMsg_updateU32(reply, MamaFieldSeqNum.mName, MamaFieldSeqNum.mFid,
                impl->mSeqNum);

        if(MAMA_STATUS_OK != mamaMsg_updateU8(reply,MamaFieldMsgStatus.mName,
                    MamaFieldMsgStatus.mFid, impl->mStatus))
        {
            mamaMsg_updateI16(reply,MamaFieldMsgStatus.mName,
                    MamaFieldMsgStatus.mFid, impl->mStatus);
        }
    }

    if (impl->mSendTime)
    	updateSendTime(impl, reply);

    return (mamaPublisher_sendReplyToInboxHandle (impl->mPublisher,
                replyAddress, reply));
}

void mamaDQPublisher_destroy (mamaDQPublisher pub)
{
    mamaDQPublisherImpl* impl = (mamaDQPublisherImpl*) (pub);

    if (impl->mPublisher)
    {
        mamaPublisher_destroy (impl->mPublisher);
        impl->mPublisher = NULL;
    }

    free (impl->mSendTimeFormat);
    impl->mSendTimeFormat=NULL;

    if (impl->mSendTime)
    {
        mamaDateTime_destroy (impl->mSendTime);
        impl->mSendTime = NULL;
    }

    free(impl);
}



void mamaDQPublisher_setStatus (mamaDQPublisher pub, mamaMsgStatus  status)
{
    mamaDQPublisherImpl* impl = (mamaDQPublisherImpl*) (pub);
    impl->mStatus = status;
}
    
void mamaDQPublisher_setSenderId (mamaDQPublisher pub, uint64_t  senderid)
{
    mamaDQPublisherImpl* impl = (mamaDQPublisherImpl*) (pub);
    impl->mSenderId = senderid;
}
   
void mamaDQPublisher_setSeqNum (mamaDQPublisher pub, mama_seqnum_t num)
{
    mamaDQPublisherImpl* impl = (mamaDQPublisherImpl*) (pub);
    impl->mSeqNum=num;
}

void mamaDQPublisher_enableSendTime (mamaDQPublisher pub, mama_bool_t enable)
{
    mamaDQPublisherImpl* impl = (mamaDQPublisherImpl*) (pub);

    if (enable)
    {
        if (!impl->mSendTime)
            mamaDateTime_create(&impl->mSendTime);
    }
    else
    {
        if (impl->mSendTime)
        {
            mamaDateTime_destroy(impl->mSendTime);
            impl->mSendTime=NULL;
        }
    }
}

void mamaDQPublisher_setClosure (mamaDQPublisher pub, void*  closure)
{
    mamaDQPublisherImpl* impl = (mamaDQPublisherImpl*) (pub);
    if (impl)
        impl->mClosure = closure;
}


void* mamaDQPublisher_getClosure (mamaDQPublisher pub)
{
    mamaDQPublisherImpl* impl = (mamaDQPublisherImpl*) (pub);

    if (impl)
        return impl->mClosure;
    else
        return NULL;
}

void mamaDQPublisher_setCache (mamaDQPublisher pub, void*  cache)
{
    mamaDQPublisherImpl* impl = (mamaDQPublisherImpl*) (pub);
    if (impl)
        impl->mCache = cache;
}


void* mamaDQPublisher_getCache (mamaDQPublisher pub)
{
    mamaDQPublisherImpl* impl = (mamaDQPublisherImpl*) (pub);

    if (impl)
        return impl->mCache;
    else
        return NULL;
}

mama_status updateSendTime (mamaDQPublisher pub, mamaMsg msg)
{
    mamaMsgField 	sendTimeField = NULL;
    mama_u64_t	sendTime =0;
    char  			sendtimestr[MAX_DATE_STR];
    mamaDQPublisherImpl* impl = (mamaDQPublisherImpl*) (pub);

    mamaDateTime_setToNow(impl->mSendTime);

    if (MAMA_STATUS_OK == mamaMsg_getField(msg, MamaFieldSendTime.mName,
                MamaFieldSendTime.mFid, &sendTimeField))
   {
        mamaFieldType sendTimeType = MAMA_FIELD_TYPE_UNKNOWN;
        if (MAMA_STATUS_OK == mamaMsgField_getType(sendTimeField,
                    &sendTimeType))
        {
            switch(sendTimeType)
            {
                case MAMA_FIELD_TYPE_I64:
                    mamaDateTime_getEpochTimeMilliseconds(impl->mSendTime, &sendTime);
                    mamaMsgField_updateI64(sendTimeField,(mama_i64_t)sendTime);
                    break;

                case MAMA_FIELD_TYPE_STRING:
                    mamaDateTime_getAsFormattedString (impl->mSendTime,
                                                sendtimestr,
                                                MAX_DATE_STR,
                                                impl->mSendTimeFormat);
                    mamaMsg_updateString(msg, MamaFieldSendTime.mName,
                                    MamaFieldSendTime.mFid, sendtimestr);
                    break;

                case MAMA_FIELD_TYPE_TIME:
                    mamaMsgField_updateDateTime(sendTimeField, impl->mSendTime);
                    break;

                default:
                    mama_log(MAMA_LOG_LEVEL_WARN, "Unsupported sendtime field format");
                    break;
            }
    	}
    }
    else
        mamaMsg_addDateTime(msg, MamaFieldSendTime.mName,
                    MamaFieldSendTime.mFid, impl->mSendTime);

    return MAMA_STATUS_OK;
}

mama_status updateSenderId (mamaDQPublisher pub, mamaMsg msg)
{
    mamaMsgField senderIdField = NULL;
    mamaDQPublisherImpl* impl = (mamaDQPublisherImpl*) (pub);

    if (MAMA_STATUS_OK == mamaMsg_getField(msg,
        MamaFieldSenderId.mName, MamaFieldSenderId.mFid,  &senderIdField))
    {
        mamaFieldType senderIdType = MAMA_FIELD_TYPE_UNKNOWN;
        if (MAMA_STATUS_OK == mamaMsgField_getType(senderIdField,
                                                                                                            &senderIdType))
        {
            switch(senderIdType)
            {
                case MAMA_FIELD_TYPE_U16:
                mamaMsgField_updateU16(senderIdField,
                (mama_u16_t)impl->mSenderId);
                break;

                case MAMA_FIELD_TYPE_U32:
                mamaMsgField_updateU32(senderIdField,
                (mama_u32_t)impl->mSenderId);
                break;

                case MAMA_FIELD_TYPE_U64:
                    mamaMsgField_updateU64(senderIdField,
                                                                         impl->mSenderId);
                    break;

                default:
                    mama_log(MAMA_LOG_LEVEL_WARN, "Unsupported senderid field format");
                    break;
            }
        }
    }
    else
        mamaMsg_addU64(msg, MamaFieldSenderId.mName,
                                                MamaFieldSenderId.mFid, impl->mSenderId);

    return MAMA_STATUS_OK;
}
