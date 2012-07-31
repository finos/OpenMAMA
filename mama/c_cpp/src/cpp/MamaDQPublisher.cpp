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

#include <mama/mamacpp.h>
#include "InboxImpl.h"
#include "mamacppinternal.h"
#include "mama/senderId.h"

#include "mama/MamaDQPublisher.h"
#include "MamaDQPublisherImpl.h"
#include "mama/dqpublisher.h"

namespace Wombat
{

 





MamaDQPublisher::~MamaDQPublisher (void) 
{
    if (mImpl)
    {
        delete mImpl;
        mImpl = NULL;
    }
}

MamaDQPublisher::MamaDQPublisher (void) 
    : mImpl (new MamaDQPublisherImpl (this))
{
	
    
}

void MamaDQPublisher::create (MamaTransport *transport, 
                 const char*  topic)
{
    mImpl->create (transport, topic);
}

void MamaDQPublisher::send (MamaMsg* msg)
{
    

	mamaDQPublisher_send(mImpl->mDQPublisher, msg->getUnderlyingMsg());
}


void MamaDQPublisher::sendReply (const MamaMsg& request, MamaMsg* reply) const
{
    mamaDQPublisher_sendReply(mImpl->mDQPublisher, request.getUnderlyingMsg(), reply->getUnderlyingMsg());
}

void MamaDQPublisher::sendReply (mamaMsgReply replyHandle, MamaMsg* reply) const
{
	mamaDQPublisher_sendReplyWithHandle(mImpl->mDQPublisher, replyHandle, reply->getUnderlyingMsg());
}

void MamaDQPublisher::destroy (void)
{
    mImpl->destroy ();
}



void  MamaDQPublisher::setStatus (mamaMsgStatus  status)
{
	mamaDQPublisher_setStatus(mImpl->mDQPublisher, status);
}
    
void  MamaDQPublisher::setSenderId (uint64_t  id)
{
	mamaDQPublisher_setSenderId(mImpl->mDQPublisher, id);
}
   
void  MamaDQPublisher::setSeqNum (mama_seqnum_t num)
{
	mamaDQPublisher_setSeqNum(mImpl->mDQPublisher, num);
}

void* MamaDQPublisher::getCache (void)
{
	return mamaDQPublisher_getCache(mImpl->mDQPublisher);
}

void MamaDQPublisher::setCache (void* cache)
{
	mamaDQPublisher_setCache(mImpl->mDQPublisher, cache);
}


MamaDQPublisherImpl::MamaDQPublisherImpl(MamaDQPublisher* publisher): 
        mParent (publisher),
        mDQPublisher (NULL)

{

}

void MamaDQPublisherImpl::create (MamaTransport *transport, 
                 const char* topic)
{
	mamaDQPublisher_allocate(&mDQPublisher);
    mamaTry (mamaDQPublisher_create (mDQPublisher,
                           transport->getCValue(), 
                           topic));
    mamaDQPublisher_setClosure(mDQPublisher, (void*)this);
}

void MamaDQPublisherImpl::destroy (void)
{
    if (mDQPublisher)
    {
        mamaDQPublisher_destroy (mDQPublisher);
        mDQPublisher = NULL;
    }
}

} // namespace Wombat
