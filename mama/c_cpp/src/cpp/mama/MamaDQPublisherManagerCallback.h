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

#ifndef MAMA_DQPUBLISHERMANAGER_CALLBACK_CPP_H__
#define MAMA_DQPUBLISHERMANAGER_CALLBACK_CPP_H__

#include "mama/mamacpp.h"
#include "mama/msgtype.h"


namespace Wombat 
{


class MamaDQPublisherManager;
class MamaDQPublisher;
class MamaPublishTopic;

class MAMACPPExpDLL MamaDQPublisherManagerCallback
{
public:
    virtual ~MamaDQPublisherManagerCallback() {};

    virtual void onCreate (
        MamaDQPublisherManager*  publisherManager) = 0;
     
    virtual void onNewRequest (
        MamaDQPublisherManager*  publisherManager,
        const char*        symbol,
        short  subType,
        short    msgType,
        MamaMsg&     msg) = 0;
        
        
    virtual void onRequest (
        MamaDQPublisherManager*  publisherManager,
        const MamaPublishTopic&  publishTopicInfo,
        short  subType,
        short    msgType,
        MamaMsg&     msg) = 0;

    virtual void onRefresh (
        MamaDQPublisherManager*  publisherManager,
        const MamaPublishTopic&  publishTopicInfo,
        short  subType,
        short    msgType,
        MamaMsg&     msg) = 0;
        
    virtual void onError (
        MamaDQPublisherManager*  publisher,
        const MamaStatus&  status,
        const char*        errortxt,
        MamaMsg*     msg) = 0;

	virtual void onMsg(
			MamaDQPublisherManager*  publisher,
			MamaMsg&     msg) {};

};

} // namespace Wombat
#endif // MAMA_DQPUBLISHER_CALLBACK_CPP_H__
