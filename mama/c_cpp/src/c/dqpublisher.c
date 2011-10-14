/* $Id: dqpublisher.c,v 1.2.22.3 2011/08/10 14:53:25 nicholasmarriott Exp $
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

#include "mama/dqpublisher.h"
#include "mama/publisher.h"



typedef struct mamaDQPublisherImpl_
{
    mamaPublisher 	mPublisher;
    mamaMsgStatus  	mStatus;
    uint64_t  		mSenderId;
    mama_seqnum_t   mSeqNum;
} mamaDQPublisherImpl;


mama_status mamaDQPublisher_allocate (mamaDQPublisher* result)
{
    mamaDQPublisherImpl* impl  = NULL;

    impl = (mamaDQPublisherImpl*)calloc (1, sizeof (mamaDQPublisherImpl));
    if (!impl) return MAMA_STATUS_NOMEM;
    
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

	if (status == MAMA_STATUS_OK)
    {
        impl->mSenderId = mamaSenderId_getSelf ();
        impl->mStatus = MAMA_MSG_STATUS_OK;
        impl->mSeqNum = 1;
    }
    
    return status;
}


mama_status mamaDQPublisher_send (mamaDQPublisher pub, mamaMsg msg)
{     
    mamaDQPublisherImpl* impl = (mamaDQPublisherImpl*) (pub);

	if (impl->mSeqNum != 0)
    {
        switch (mamaMsgType_typeForMsg (msg))
        {
			case MAMA_MSG_TYPE_REFRESH :
			case MAMA_MSG_TYPE_SYNC_REQUEST :
			case MAMA_MSG_TYPE_MISC :
			case MAMA_MSG_TYPE_NOT_PERMISSIONED :
			case MAMA_MSG_TYPE_NOT_FOUND : 
				break;
				
            case MAMA_MSG_TYPE_INITIAL      :
            case MAMA_MSG_TYPE_BOOK_INITIAL :
            case MAMA_MSG_TYPE_RECAP        :
            case MAMA_MSG_TYPE_BOOK_RECAP   :
				mamaMsg_updateU8(msg,NULL, MamaFieldMsgStatus.mFid, impl->mStatus);
                break;

            default:
				mamaMsg_updateU8(msg,NULL, MamaFieldMsgStatus.mFid, impl->mStatus);
                impl->mSeqNum++;
                break;
        }
		mamaMsg_updateU32(msg, NULL, MamaFieldSeqNum.mFid, impl->mSeqNum);  
    }
    
    if (impl->mSenderId != 0)
        mamaMsg_updateU64(msg, NULL, MamaFieldSenderId.mFid, impl->mSenderId);

	return (mamaPublisher_send (impl->mPublisher, msg));
} 

mama_status mamaDQPublisher_sendReply (mamaDQPublisher pub,
                                       mamaMsg request,
                                       mamaMsg reply)
{
    mamaDQPublisherImpl* impl = (mamaDQPublisherImpl*) (pub);

    mamaMsg_updateU8(reply, NULL, MamaFieldMsgStatus.mFid, impl->mStatus);
    
    if (impl->mSenderId != 0)
        mamaMsg_updateU64(reply, NULL, MamaFieldSenderId.mFid, impl->mSenderId);
        
    if (impl->mSeqNum != 0)
        mamaMsg_updateU32(reply, NULL, MamaFieldSeqNum.mFid, impl->mSeqNum);  

    return (mamaPublisher_sendReplyToInbox (impl->mPublisher, request, reply));
}


void mamaDQPublisher_destroy (mamaDQPublisher pub)
{
    mamaDQPublisherImpl* impl = (mamaDQPublisherImpl*) (pub);

    if (impl->mPublisher)
    {
        mamaPublisher_destroy (impl->mPublisher);
        impl->mPublisher = NULL;
    }
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


