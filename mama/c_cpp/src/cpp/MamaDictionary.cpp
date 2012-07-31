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

#include "mama/MamaDictionary.h"
#include "mama/MamaSource.h"
#include "FieldDescriptorImpl.h"
#include "mamacppinternal.h"
#include "mamainternal.h"

namespace Wombat
{
/******************************************************************************
 * MamaDictionary Implementation
 */
class DictionaryImpl 
{
public:

    DictionaryImpl() :
        mDictionary (NULL), 
        mCallback (NULL), 
        mClosure (NULL){}

    virtual void create (
        MamaQueue*              queue,
        MamaDictionaryCallback* callback,
        MamaSource*             source,
        double                  timeout,
        int                     retries,
        void*                   closure); 

     virtual ~DictionaryImpl (void);

    virtual const char* getFeedName();
    virtual const char* getFeedHost();
    virtual MamaFieldDescriptor* getFieldByFid  (mama_fid_t fid);
    virtual MamaFieldDescriptor* getFieldByIndex (size_t index);
    virtual MamaFieldDescriptor* getFieldByName (const char* name);

    virtual mama_fid_t                 getMaxFid      (void) const;
    virtual size_t                     getSize        (void) const;
    virtual bool                       hasDuplicates  (void) const;
    
    virtual MamaDictionaryCallback* getCallback (void) const;
    virtual void setCallback (MamaDictionaryCallback* callback);
    virtual mamaDictionary getDictC ();
    virtual void* getClosure (void) const;
    virtual MamaMsg* getDictionaryMessage() const;
    virtual void buildDictionaryFromMessage(MamaMsg& msg);
    virtual MamaFieldDescriptor* createFieldDescriptor (
                            mama_fid_t             fid,
                            const char*            name,
                            mamaFieldType          type);
    virtual void setMaxFid (
                        mama_size_t          maxFid);
    virtual void writeToFile (const char* fileName);
    virtual void populateFromFile (const char* fileName);

    static void	MAMACALLTYPE completeCallback (mamaDictionary, void *);
    static void	MAMACALLTYPE timeoutCallabck (mamaDictionary, void *);
    static void MAMACALLTYPE errorCallback   (mamaDictionary, const char*, void *);

private:
    MamaFieldDescriptor*    getCppFieldDescriptor (mamaFieldDescriptor fd);
    mamaDictionary          mDictionary;
    MamaDictionaryCallback* mCallback;
    void*                   mClosure;
};

class DictionaryTestCallback : public MamaDictionaryCallback
{
public:
	DictionaryTestCallback(MamaDictionaryCallback* usercallback);
	virtual ~DictionaryTestCallback(){};
	virtual void onComplete();
	virtual void onTimeout();
	virtual void onError(const char* message);
	MamaDictionaryCallback* getCallback() {return mCallback;}
	void setCallback(MamaDictionaryCallback* usercallback) {mCallback = usercallback;}
private:
	MamaDictionaryCallback* 	mCallback;
};

DictionaryTestCallback::DictionaryTestCallback(MamaDictionaryCallback* usercallback)
{
	mCallback = usercallback;
}

void DictionaryTestCallback::onComplete()
{
	try {
		mCallback->onComplete();
	}
	catch (...)
	{
		fprintf (stderr, "onComplete EXCEPTION CAUGHT\n");
	}
	return;
}

void DictionaryTestCallback::onTimeout()
{
	try {
		mCallback->onTimeout();
	}
	catch (...)
	{
		fprintf (stderr, "onTimeout EXCEPTION CAUGHT\n");
	}

	return;
}

void DictionaryTestCallback::onError(const char* message)
{
	try {
		mCallback->onError(message);
	}
	catch (...)
	{
		fprintf (stderr, "onError EXCEPTION CAUGHT\n");
	}

	return;
}
   

MamaDictionary::~MamaDictionary ()
{
    if (mPimpl)
    {
		if (mamaInternal_getCatchCallbackExceptions())
		{
			DictionaryTestCallback* lcallback = (DictionaryTestCallback*)mPimpl->getCallback();
			delete lcallback;
		}
        delete mPimpl;
        mPimpl = NULL;
    }
}

MamaDictionary::MamaDictionary (void)
    : mPimpl (new DictionaryImpl) {}

void MamaDictionary::create (
    MamaQueue*              queue,
    MamaDictionaryCallback* callback,
    MamaSource*             source,
    int                     retries, // = MAMA_DEFAULT_RETRIES,
    double                  timeout, // = MAMA_DEFAULT_TIMEOUT,
    void*                   closure) // = NULL)
{
	if (mamaInternal_getCatchCallbackExceptions())
	{
		DictionaryTestCallback* lcallback = new DictionaryTestCallback(callback);
	    mPimpl->create (queue, lcallback, source, timeout, retries, closure);
	}
	else
	{
		mPimpl->create (queue, callback, source, timeout, retries, closure);
	}
}

const char* MamaDictionary::getFeedName()
{
    return mPimpl->getFeedName();
}

const char* MamaDictionary::getFeedHost()
{
    return mPimpl->getFeedHost();
}

MamaFieldDescriptor* MamaDictionary::getFieldByFid (mama_fid_t fid)
{
    return mPimpl->getFieldByFid (fid);
}

const MamaFieldDescriptor* MamaDictionary::getFieldByFid (mama_fid_t fid) const
{
    return const_cast<const MamaFieldDescriptor*>(
        mPimpl->getFieldByFid (fid));
}

MamaFieldDescriptor* MamaDictionary::getFieldByIndex (size_t index)
{
    return mPimpl->getFieldByIndex (index);
}

const MamaFieldDescriptor* MamaDictionary::getFieldByIndex (size_t index) const
{
    return const_cast<const MamaFieldDescriptor*>(
        mPimpl->getFieldByIndex (index));
}

MamaFieldDescriptor* MamaDictionary::getFieldByName (const char* name)
{
    return mPimpl->getFieldByName (name);
}

const MamaFieldDescriptor* MamaDictionary::getFieldByName (const char* name) const
{
    return const_cast<const MamaFieldDescriptor*>(
        mPimpl->getFieldByName (name));
}

mama_fid_t MamaDictionary::getMaxFid (void) const
{
    return mPimpl->getMaxFid ();
}

size_t MamaDictionary::getSize (void) const
{
    return mPimpl->getSize ();
}

bool MamaDictionary::hasDuplicates (void) const
{
    return mPimpl->hasDuplicates ();
}

MamaDictionaryCallback* MamaDictionary::getCallback (void) const
{
	if (mamaInternal_getCatchCallbackExceptions())
	{
		DictionaryTestCallback* lcallback;
		lcallback = (DictionaryTestCallback*)mPimpl->getCallback();
		return lcallback->getCallback();
	}
	else 
	{
		return mPimpl->getCallback();
	}
}

void MamaDictionary::setCallback (MamaDictionaryCallback* callback)
{
	if (mamaInternal_getCatchCallbackExceptions())
	{
		DictionaryTestCallback* lcallback = (DictionaryTestCallback*)mPimpl->getCallback();
		lcallback->setCallback(callback);
	}
	else
	{
		mPimpl->setCallback (callback);
	}
	return;
}

mamaDictionary MamaDictionary::getDictC ()
{
    return mPimpl->getDictC ();
}

const mamaDictionary MamaDictionary::getDictC () const
{
    return const_cast<const mamaDictionary>(mPimpl->getDictC());
}

void* MamaDictionary::getClosure (void) const
{
    return mPimpl->getClosure ();
}

MamaMsg* MamaDictionary::getDictionaryMessage() const
{
    return mPimpl->getDictionaryMessage();
}

void MamaDictionary::buildDictionaryFromMessage(MamaMsg& msg)
{
    mPimpl->buildDictionaryFromMessage(msg);
}

MamaFieldDescriptor* MamaDictionary::createFieldDescriptor (
    mama_fid_t             fid,
    const char*            name,
    mamaFieldType          type)
{
    return mPimpl->createFieldDescriptor (fid, name, type);
}

void MamaDictionary::setMaxFid (
    mama_size_t          maxFid)
{
    mPimpl->setMaxFid (maxFid);
}

void MamaDictionary::writeToFile (const char* fileName)
{
    mPimpl->writeToFile (fileName);
}

void MamaDictionary::populateFromFile (const char* fileName)
{
    mPimpl->populateFromFile (fileName);
}

extern "C"
{

void
DictionaryImpl::completeCallback (
    mamaDictionary dictionary, void* closure)
{
    DictionaryImpl* impl = (DictionaryImpl*)closure;
    impl->mCallback->onComplete ();
}

void
DictionaryImpl::timeoutCallabck (mamaDictionary dictionary, void *closure)
{
    DictionaryImpl *impl = (DictionaryImpl*)closure;
    impl->mCallback->onTimeout ();
}

void
DictionaryImpl::errorCallback (mamaDictionary dictionary, 
                                    const char* errorText, 
                                    void *closure)
{
    DictionaryImpl *impl = (DictionaryImpl*)closure;
    impl->mCallback->onError (errorText);
}

} // extern "C"

void DictionaryImpl::create (
    MamaQueue*              queue,
    MamaDictionaryCallback* callback,
    MamaSource*             source,
    double                  timeout,
    int                     retries,
    void*                   closure)
{
    mCallback = callback;
    mClosure  = closure;

    mamaDictionaryCallbackSet cbSet = { completeCallback,
                                        timeoutCallabck,
                                        errorCallback };

    mamaQueue cQueue = (queue == NULL) ? NULL : queue->getCValue();

    mamaTry (mama_createDictionary (
                &mDictionary,
                cQueue,
                cbSet,
                source->getCValue(),
                timeout,
                retries,
                this));
}

DictionaryImpl::~DictionaryImpl (void)
{
    /* Return if dictionary is null */ 
    if (mDictionary == NULL)
    {
        return;
    }
    
    /* We need to delete all of the MamaFieldDescriptor objects */
    if (getSize() == 0)
    {
        //Return when the size of the Dictionary is zero
        return;
    }
    for (int i = 0; i <= getMaxFid(); i++)
    {
        mamaFieldDescriptor fd = 0;
        mamaTryIgnoreNotFound (mamaDictionary_getFieldDescriptorByFid (
                mDictionary,
                &fd,
                i));
        if (fd)
        {
            void* closureData;
            mamaTry (mamaFieldDescriptor_getClosure (fd, &closureData));
            MamaFieldDescriptor* cppFd = (MamaFieldDescriptor*)closureData;
            delete cppFd;
        }
    }

    mamaTry (mamaDictionary_destroy (mDictionary));
    mDictionary = 0;
}

const char* DictionaryImpl::getFeedName()
{
    const char* result = NULL;
    mamaTryIgnoreNotFound ( mamaDictionary_getFeedName (
                            mDictionary,
                            &result));
    return result; 
}

const char* DictionaryImpl::getFeedHost()
{
    const char* result = NULL;
    mamaTryIgnoreNotFound ( mamaDictionary_getFeedHost (
                            mDictionary,
                            &result));
    return result; 
}

MamaFieldDescriptor* DictionaryImpl::getFieldByFid (mama_fid_t fid)
{
    mamaFieldDescriptor result = 0;

    mamaTry (mamaDictionary_getFieldDescriptorByFid (
                mDictionary,
                &result,
                fid));
    
   return getCppFieldDescriptor (result); 
}

MamaFieldDescriptor* DictionaryImpl::getFieldByIndex (size_t index)
{
    mamaFieldDescriptor result = 0;

    mamaTryIgnoreNotFound (mamaDictionary_getFieldDescriptorByIndex (
                mDictionary,
                &result,
                index));
    return getCppFieldDescriptor (result);
}

MamaFieldDescriptor* 
DictionaryImpl::getFieldByName (const char* name)
{
    mamaFieldDescriptor result;

    mamaTryIgnoreNotFound (mamaDictionary_getFieldDescriptorByName (
                mDictionary,
                &result,
                name));
    return getCppFieldDescriptor (result);
}


mama_fid_t DictionaryImpl::getMaxFid (void) const
{
    mama_fid_t result;
    mamaTry (mamaDictionary_getMaxFid (mDictionary, &result));
    return result;
}

size_t DictionaryImpl::getSize (void) const
{
    size_t result;
    mamaTry (mamaDictionary_getSize (mDictionary, &result));
    return result;
}


bool DictionaryImpl::hasDuplicates (void) const
{
    int result;
    mamaTry (mamaDictionary_hasDuplicates (mDictionary, &result));
    return result != 0;
}


MamaDictionaryCallback* DictionaryImpl::getCallback (void) const
{
    return mCallback;
}

void DictionaryImpl::setCallback (MamaDictionaryCallback* callback)
{
    mCallback = callback;
}

mamaDictionary DictionaryImpl::getDictC (void)
{
    return mDictionary;
}

void* DictionaryImpl::getClosure (void) const
{
    return mClosure;
}

MamaMsg* DictionaryImpl::getDictionaryMessage() const
{
    mamaMsg  dictMsgC = NULL;
    MamaMsg* dictMsgCpp = NULL;
    
    /*First get the mamaMsg from the C layer*/
    mamaTry(mamaDictionary_getDictionaryMessage(mDictionary,&dictMsgC));
 
    /*We need a C++ wrapper to return*/
    dictMsgCpp = new MamaMsg();

    /*The wrapper must destroy the C msg when it is deleted.*/
    dictMsgCpp->createFromMsg(dictMsgC, true);
    
    return dictMsgCpp;
}

void DictionaryImpl::buildDictionaryFromMessage(MamaMsg& msg)
{
    if (!mDictionary)
    {
        mamaTry(mamaDictionary_create(&mDictionary));
        
        mamaTry(mamaDictionary_buildDictionaryFromMessage(
             mDictionary,msg.getUnderlyingMsg()));
    }
    return;
}

MamaFieldDescriptor* DictionaryImpl::createFieldDescriptor (
        mama_fid_t             fid,
        const char*            name,
        mamaFieldType          type)
{
    mamaFieldDescriptor newDescriptor = NULL;

    /*Create the underlying dictionary if required.*/
    if (!mDictionary)
        mamaTry(mamaDictionary_create(&mDictionary));

    mamaTry(mamaDictionary_createFieldDescriptor (mDictionary,
                                       fid,
                                       name,
                                       type,
                                       &newDescriptor));
    
    return getCppFieldDescriptor (newDescriptor);
}

void DictionaryImpl::setMaxFid (
        mama_size_t          maxFid)
{
    /*Create the underlying dictionary if required.*/
    if (!mDictionary)
        mamaTry(mamaDictionary_create(&mDictionary));

    mamaTry (mamaDictionary_setMaxFid (mDictionary, maxFid));
}


void DictionaryImpl::writeToFile (const char* fileName)
{
    mamaTry (mamaDictionary_writeToFile (mDictionary, fileName));
}

void DictionaryImpl::populateFromFile (const char* fileName)
{
    if (!mDictionary)
    {
        mamaTry(mamaDictionary_create(&mDictionary));
    }
    
    mamaTry (mamaDictionary_populateFromFile (mDictionary, fileName));
}

MamaFieldDescriptor* DictionaryImpl::getCppFieldDescriptor (
                                mamaFieldDescriptor fd)
{
    MamaFieldDescriptor* cppDescriptor = NULL;
    void*                closureData   = NULL;
   
    /* Return NULL for field not found otherwise MAMDA breaks. (MLS) */
    if (fd == 0)
    {
        return NULL;
    }

    mamaTry (mamaFieldDescriptor_getClosure (fd, &closureData));
    
    if (closureData)
    {
        cppDescriptor = (MamaFieldDescriptor*)closureData;
    }
    else
    {
        cppDescriptor = new MamaFieldDescriptor (fd);
        mamaFieldDescriptor_setClosure (fd,(void*)cppDescriptor);
    }

    return cppDescriptor;
}

} // namespace Wombat
