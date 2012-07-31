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

#include <map>
#include <string.h>

#include "mamainternal.h"
#include "InboxImpl.h"
#include "mamacppinternal.h"

#include "mama/mamacpp.h"
#include "mama/subscription.h"
#include "mama/MamaDQPublisherManager.h"
#include "mama/MamaDQPublisher.h"
#include "MamaDQPublisherImpl.h"
#include "mama/dqpublisher.h"
#include "mama/subscmsgtype.h"
#include "mama/clientmanage.h"

using std::map;
using std::string;

namespace Wombat
{

typedef  map<string,MamaPublishTopic*>  PublisherMap;
typedef  map<string,MamaPublishTopic*>::iterator  PublisherMapIterator;

void MamaPublishTopic::set(mamaPublishTopic* pubInfo)
{
	mSymbol = pubInfo->symbol;
	mCache=pubInfo->cache;
	mPub=(MamaDQPublisher*)mamaDQPublisher_getClosure(pubInfo->pub);
}

class DQPublisherManagerTestCallback : public MamaDQPublisherManagerCallback
{
public:
	DQPublisherManagerTestCallback(MamaDQPublisherManagerCallback* usercallback) {mCallback = usercallback;};


	virtual ~DQPublisherManagerTestCallback() {};

	virtual void onCreate (
		MamaDQPublisherManager*  publisherManager);

	virtual void onNewRequest (
		MamaDQPublisherManager*  	publisherManager,
		const char*        			symbol,
		short  						subType,
		short    					msgType,
		MamaMsg&     				msg);


	virtual void onRequest (
		MamaDQPublisherManager*  	publisherManager,
		const MamaPublishTopic&  	publishTopicInfo,
		short  						subType,
		short    					msgType,
		MamaMsg&     				msg);

	virtual void onRefresh (
		MamaDQPublisherManager*  	publisherManager,
		const MamaPublishTopic& 	 publishTopicInfo,
		short  						subType,
		short    					msgType,
		MamaMsg&     				msg);

	virtual void onError (
		MamaDQPublisherManager*  	publisher,
		const MamaStatus&  			status,
		const char*        			errortxt,
		MamaMsg*     				msg);



private:
	MamaDQPublisherManagerCallback* mCallback;
};

void DQPublisherManagerTestCallback::onCreate (MamaDQPublisherManager*  publisherManager)
{
	try {
		mCallback->onCreate(publisherManager);
	}
	catch (...)
	{
		fprintf (stderr, "MamaDQPublisherCallback onCreate EXCEPTION CAUGHT\n");
	}
}

void DQPublisherManagerTestCallback::onNewRequest (MamaDQPublisherManager*  publisherManager, const char* symbol, short subType, short msgType, MamaMsg&     msg)
{
	try {
		mCallback->onNewRequest(publisherManager, symbol, subType, msgType, msg);
	}
	catch (...)
	{
		fprintf (stderr, "MamaDQPublisherCallback onNewRequest EXCEPTION CAUGHT\n");
	}
}


void DQPublisherManagerTestCallback::onRequest (MamaDQPublisherManager* publisherManager, const MamaPublishTopic& publishTopicInfo, short subType, short msgType, MamaMsg& msg)
{
	try {
		mCallback->onRequest(publisherManager, publishTopicInfo, subType, msgType, msg);
	}
	catch (...)
	{
		fprintf (stderr, "MamaDQPublisherCallback onRequest EXCEPTION CAUGHT\n");
	}
}

void DQPublisherManagerTestCallback::onRefresh (MamaDQPublisherManager* publisherManager, const MamaPublishTopic& publishTopicInfo, short subType, short msgType, MamaMsg& msg)
{
	try {
		mCallback->onRefresh(publisherManager, publishTopicInfo, subType, msgType, msg);
	}
	catch (...)
	{
		fprintf (stderr, "MamaDQPublisherCallback onRefresh EXCEPTION CAUGHT\n");
	}
}

void DQPublisherManagerTestCallback::onError (MamaDQPublisherManager* publisher, const MamaStatus& status, const char* errortxt, MamaMsg* msg)
{
	try {
		mCallback->onError(publisher, status, errortxt, msg);
	}
	catch (...)
	{
		fprintf (stderr, "MamaDQPublisherCallback onError EXCEPTION CAUGHT\n");
	}
}


struct MamaDQPublisherManagerImpl
{
    MamaDQPublisherManagerImpl (MamaDQPublisherManager* publisher): 
		mCallback				(NULL),
        mParent                 (publisher)
    {
    	mamaDQPublisherManager_allocate(&mDQPublisherManager);
    	mReuseableMsg.create();
    };
    
    virtual ~MamaDQPublisherManagerImpl (void) { destroy (); }

    void create (MamaTransport *transport, MamaQueue* queue,
             MamaDQPublisherManagerCallback*   	callback,
             const char* sourcename, const char* root);
    void destroy (void);

     
    void addPublisher (const char *symbol, MamaDQPublisher* pub, void * cache);
    MamaDQPublisher* removePublisher (const char *symbol);
    
    MamaDQPublisher* createPublisher (const char *symbol, void * cache);
    void destroyPublisher (const char *symbol);
    
      
    void setInfo(mamaPublishTopic* info){mReuseableInfo.set(info);}

    MamaMsg 				mReuseableMsg;


    MamaDQPublisherManager* mParent;
    mamaDQPublisherManager 		 	mDQPublisherManager;
    MamaDQPublisherManagerCallback*  mCallback;
    
    MamaPublishTopic				mReuseableInfo;
};


void MAMACALLTYPE dqPublisherManagerImplCreateCb (
        mamaDQPublisherManager manager)
{
	MamaDQPublisherManagerImpl* mImpl = (MamaDQPublisherManagerImpl*)mamaDQPublisherManager_getClosure(manager);
	mImpl->mCallback->onCreate(mImpl->mParent);
    }
    
void MAMACALLTYPE dqPublisherManagerImplNewRequestCb(
        mamaDQPublisherManager manager,
        const char*        symbol,
        short  subType,
        short    msgType,
        mamaMsg     msg)
    {
	MamaDQPublisherManagerImpl* mImpl = (MamaDQPublisherManagerImpl*)mamaDQPublisherManager_getClosure(manager);
	mImpl->mReuseableMsg.setMsg(msg);
	mImpl->mCallback->onNewRequest(mImpl->mParent, symbol, subType,msgType, mImpl->mReuseableMsg);
    }

void MAMACALLTYPE dqPublisherManagerImplRequestCb(
        mamaDQPublisherManager manager,
        mamaPublishTopic* info,
        short  subType,
        short    msgType,
        mamaMsg     msg)
    {
	MamaDQPublisherManagerImpl* mImpl = (MamaDQPublisherManagerImpl*)mamaDQPublisherManager_getClosure(manager);
	mImpl->mReuseableMsg.setMsg(msg);
	mImpl->setInfo(info);
	mImpl->mCallback->onRequest(mImpl->mParent, mImpl->mReuseableInfo, subType, msgType, mImpl->mReuseableMsg);
}

void MAMACALLTYPE dqPublisherManagerImplRefreshCb(
        mamaDQPublisherManager manager,
        mamaPublishTopic* info,
        short  subType,
        short    msgType,
        mamaMsg     msg)
{
	MamaDQPublisherManagerImpl* mImpl = (MamaDQPublisherManagerImpl*)mamaDQPublisherManager_getClosure(manager);
	mImpl->mReuseableMsg.setMsg(msg);
	mImpl->setInfo(info);
	mImpl->mCallback->onRefresh(mImpl->mParent, mImpl->mReuseableInfo, subType, msgType, mImpl->mReuseableMsg);
}

void MAMACALLTYPE dqPublisherManagerImplErrorCb(
        mamaDQPublisherManager manager,
                              mama_status      status,
        const char*        errortxt,
        mamaMsg     msg)
{
	MamaDQPublisherManagerImpl* mImpl = (MamaDQPublisherManagerImpl*)mamaDQPublisherManager_getClosure(manager);
	mImpl->mReuseableMsg.setMsg(msg);
	mImpl->mCallback->onError(mImpl->mParent, MamaStatus(status), errortxt, &mImpl->mReuseableMsg);
}





MamaDQPublisherManager::~MamaDQPublisherManager (void) 
{
    if (mImpl)
    {
        delete mImpl;
        mImpl = NULL;
    }
}

MamaDQPublisherManager::MamaDQPublisherManager (void) 
    : mImpl (new MamaDQPublisherManagerImpl (this))
{
}

void MamaDQPublisherManager::create (MamaTransport *transport, 
             MamaQueue*  queue,
             MamaDQPublisherManagerCallback*   callback,
             const char* sourcename,
             const char* root)
{
    mImpl->create (transport, queue, callback, sourcename, root);
}

void MamaDQPublisherManager::addPublisher (const char *symbol, MamaDQPublisher* pub, void * cache)
{
    mImpl->addPublisher (symbol, pub, cache);
}

MamaDQPublisher* MamaDQPublisherManager::removePublisher (const char *symbol)
{
	return mImpl->removePublisher (symbol);
}

MamaDQPublisher* MamaDQPublisherManager::createPublisher (const char *symbol, void * cache)
{
    return mImpl->createPublisher (symbol, cache);
}

void MamaDQPublisherManager::destroyPublisher (const char *symbol)
{
	mImpl->destroyPublisher (symbol);
}

void MamaDQPublisherManager::destroy (void)
{
    mImpl->destroy ();
    delete mImpl;
}

void MamaDQPublisherManager::setStatus (mamaMsgStatus  status)
{
	mamaDQPublisherManager_setStatus(mImpl->mDQPublisherManager, status);
}
    
void MamaDQPublisherManager::setSenderId (uint64_t  id)
{
	mamaDQPublisherManager_setSenderId(mImpl->mDQPublisherManager, id);
}
   
void MamaDQPublisherManager::setSeqNum (mama_seqnum_t num)
{
	mamaDQPublisherManager_setSeqNum(mImpl->mDQPublisherManager, num);
}

void MamaDQPublisherManager::sendSyncRequest (mama_u16_t nummsg, mama_f64_t delay, mama_f64_t duration)
{
	mamaDQPublisherManager_sendSyncRequest(mImpl->mDQPublisherManager, nummsg, delay, duration);
}

void MamaDQPublisherManager::sendNoSubscribers (const char *symbol)
{
	mamaDQPublisherManager_sendNoSubscribers(mImpl->mDQPublisherManager, symbol);
}




/********************************************************
 *  IMPL
 */

void MamaDQPublisherManagerImpl::addPublisher (const char *symbol, MamaDQPublisher* pub, void * cache)
{
	mamaDQPublisherManager_addPublisher(mDQPublisherManager, symbol, pub->mImpl->mDQPublisher, cache);
}

MamaDQPublisher* MamaDQPublisherManagerImpl::removePublisher (const char *symbol)
{
	mamaDQPublisher aDQPublisher;
	mamaDQPublisherManager_removePublisher(mDQPublisherManager, symbol, &aDQPublisher);
	return (MamaDQPublisher*)mamaDQPublisher_getClosure(aDQPublisher);
}
 
MamaDQPublisher* MamaDQPublisherManagerImpl::createPublisher (const char *symbol, void * cache)
{
	MamaDQPublisher* aDQPublisher = new MamaDQPublisher;
	mamaDQPublisherManager_createPublisher(mDQPublisherManager, symbol, cache, &(aDQPublisher->mImpl->mDQPublisher));
	mamaDQPublisher_setClosure(aDQPublisher->mImpl->mDQPublisher, (void*)aDQPublisher);
	return (aDQPublisher);
}

void MamaDQPublisherManagerImpl::destroyPublisher (const char *symbol)
{	
	MamaDQPublisher* aDQPublisher = removePublisher(symbol);
	aDQPublisher->destroy();
}

void MamaDQPublisherManagerImpl::create (MamaTransport *transport, 
             MamaQueue*  queue,
             MamaDQPublisherManagerCallback*   callback,
             const char* sourcename,
             const char* root)
{
    // This static structure contains all of the callback function pointers
    static mamaDQPublisherManagerCallbacks aDQPublisherManagerCb =
    {
    		dqPublisherManagerImplCreateCb,
    		dqPublisherManagerImplNewRequestCb,
    		dqPublisherManagerImplRequestCb,
    		dqPublisherManagerImplRefreshCb,
    		dqPublisherManagerImplErrorCb
    };
	
    mCallback = callback;
    mamaDQPublisherManager_create(mDQPublisherManager, transport->getCValue(), queue->getCValue(),
			&aDQPublisherManagerCb,
			sourcename, root, (void*)this);
    }

void MamaDQPublisherManagerImpl::destroy (void)
	{

        
	mamaDQPublisherManager_destroy(mDQPublisherManager);


	if (mamaInternal_getCatchCallbackExceptions())
	{
		delete mCallback;
		mCallback = 0;
	}
	return;
}


} // namespace Wombat
