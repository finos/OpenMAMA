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

/* *************************************************** */
/* Includes. */
/* *************************************************** */
#include "mama/mama.h"
#include "mama/MamaBasicWildCardSubscription.h"
#include "mama/MamaBasicWildCardSubscriptionCallback.h"
#include "mamacppinternal.h"
#include "mamainternal.h"

namespace Wombat
{

/* *************************************************** */
/* Private Classes. */
/* *************************************************** */

/* This class is used whenever MAMA must catch any exceptions thrown as the result
 * of the user callback doing something wrong.
 * It will translate the exceptions into error messages written to stderr.
 */
class BasicWildCardSubscriptionTestCallback : public MamaBasicWildCardSubscriptionCallback
{
    // The user callback
	MamaBasicWildCardSubscriptionCallback *mUserCallback;

public:
    
    BasicWildCardSubscriptionTestCallback(MamaBasicWildCardSubscriptionCallback *userCallback) 
    {
        // Save arguments in member variables
        mUserCallback = userCallback;
    }

    virtual ~BasicWildCardSubscriptionTestCallback(void)
    {
    }

	virtual void onDestroy(MamaBasicWildCardSubscription *subscription, void *closure)
	{
        /* Only invoke the user callback if it is of the appropriate type
         */
        try 
        {
            // Invoke the user callback
            mUserCallback->onDestroy(subscription, closure);
        }
        catch (...)
        {
            // Print a message on an error
            fprintf (stderr, "onDestroy EXCPETION CAUGHT\n");
        }
	}

    void onCreate(MamaBasicWildCardSubscription *subscription)
    {
	    try {
		    mUserCallback->onCreate(subscription);
	    }
	    catch (...)
	    {
		    fprintf (stderr, "onCreate EXCEPTION CAUGHT\n");
	    }
    }


    void onError(MamaBasicWildCardSubscription *subscription, const MamaStatus &status, const char *subject)
    {
	    try {
		    mUserCallback->onError(subscription, status, subject);
	    }
	    catch (...)
	    {
		    fprintf (stderr, "onError EXCEPTION CAUGHT\n");
	    }
    }
								   
    void onMsg(MamaBasicWildCardSubscription *subscription, MamaMsg &msg, const char *topic)
    {
	    try {
		    mUserCallback->onMsg(subscription, msg, topic);
	    }
	    catch (...)
	    {
		    fprintf (stderr, "onMsg EXCEPTION CAUGHT\n");
	    }
    }	
};
    
/* An instance of this class is allocated each time the subscription is created and is passed
 * down into the C layer as the closure.
 * It will be member variables from this class used to invoke callbacks during event processing
 * rather than those associated with the main MamaBasicSubscription class. This is done to ensure that
 * the client will have full flexibility in destroying and recreating the subscription during the
 * callbacks.
 */
class MamaBasicWildCardSubscriptionImpl
{
    // The C++ subscription class
    MamaBasicWildCardSubscription *mBasicWildCardSubscription;

    // The user callback
    MamaBasicWildCardSubscriptionCallback *mCallback;

    // The user supplied closure
    void *mClosure;

    // The re-usable message
    MamaMsg mResuableMsg;

public:

    MamaBasicWildCardSubscriptionImpl(MamaBasicWildCardSubscriptionCallback *callback, void *closure, MamaBasicWildCardSubscription *basicWildCardSubscription)
    {
        // Save arguments in member variables
        mCallback                  = callback;
        mClosure                   = closure;
        mBasicWildCardSubscription = basicWildCardSubscription;

        // If callback exceptions are being caught then install this now
        if(mamaInternal_getCatchCallbackExceptions())
	    {
		    mCallback = new BasicWildCardSubscriptionTestCallback(callback);
	    }
    }

    virtual ~MamaBasicWildCardSubscriptionImpl(void)
    {
        // Delete the test callback if it was installed
        if((NULL != mCallback) && (mamaInternal_getCatchCallbackExceptions()))
	    {
		    delete mCallback;
	    }
    }

    void InvokeCreate(void)
    {
        // Only invoke the callback if it is supplied
        if(NULL != mCallback)
        {
            // Invoke the callback funtcion
            mCallback->onCreate(mBasicWildCardSubscription);
        }
    }

    void InvokeDestroy(void)
    {
        if(NULL != mCallback)
        {
            // Invoke the onDestroy
            mCallback->onDestroy(mBasicWildCardSubscription, mClosure);
        }
    }

    void InvokeError(mama_status status, const char *topic)
    {
        // Only invoke the callback if it is supplied
        if(NULL != mCallback)
        {
            // Convert to a C++ status
            MamaStatus cppStatus(status);

            // Invoke the callback funtcion
            mCallback->onError(mBasicWildCardSubscription, status, topic);
        }
    }

    void InvokeMsg(mamaMsg msg, const char *topic)
    {
        // Only invoke the callback if it is supplied
        if(NULL != mCallback)
        {
            // Convert the C message to a C++ message            
            mResuableMsg.createFromMsg(msg);

            // Invoke the callback
            mCallback->onMsg(mBasicWildCardSubscription, mResuableMsg, topic);
        }
    }
};


/* *************************************************** */
/* MamaBasicWildCardSubscription Implementation. */
/* *************************************************** */

/* *************************************************** */
/* Constructor and Destructor. */
/* *************************************************** */

MamaBasicWildCardSubscription::MamaBasicWildCardSubscription(void) 
    : MamaBasicSubscription()
    , mCallback(NULL)
{
}

MamaBasicWildCardSubscription::~MamaBasicWildCardSubscription(void) 
{
}

/* *************************************************** */
/* Private Function Implementations. */
/* *************************************************** */

void MAMACALLTYPE MamaBasicWildCardSubscription::onSubscriptionCreate(mamaSubscription subsc, void *closure)
{
    // Extract the impl from the closure.
    MamaBasicWildCardSubscriptionImpl *impl = (MamaBasicWildCardSubscriptionImpl *)closure;
    if(NULL != impl)
    {
        // Invoke the callback
        impl->InvokeCreate();
    }
}

void MAMACALLTYPE MamaBasicWildCardSubscription::onSubscriptionDestroy(mamaSubscription subscription, void *closure)
{
    // Extract the impl from the closure.
    MamaBasicWildCardSubscriptionImpl *impl = (MamaBasicWildCardSubscriptionImpl *)closure;
    if(NULL != impl)
    {
        // Invoke the callback
        impl->InvokeDestroy();

        // Destroy the impl as it is no longer needed
        delete impl;
    }
}

void MAMACALLTYPE MamaBasicWildCardSubscription::onSubscriptionError(mamaSubscription subsc, mama_status status, void *platformError, const char *subject, void *closure)
{
    // Extract the impl from the closure.
    MamaBasicWildCardSubscriptionImpl *impl = (MamaBasicWildCardSubscriptionImpl *)closure;
    if(NULL != impl)
    {
        // Invoke the callback
        impl->InvokeError(status, subject);
    }
}

void MAMACALLTYPE MamaBasicWildCardSubscription::onSubscriptionMessage(mamaSubscription subsc, mamaMsg msg, const char *topic, void *closure, void *topicClosure)
{
    // Extract the impl from the closure.
    MamaBasicWildCardSubscriptionImpl *impl = (MamaBasicWildCardSubscriptionImpl *)closure;
    if(NULL != impl)
    {
        // Invoke the callback
        impl->InvokeMsg(msg, topic);
    }
}

/* *************************************************** */
/* Public Function Implementations. */
/* *************************************************** */

void MamaBasicWildCardSubscription::create (
    MamaTransport*              transport,
    MamaQueue*                  queue,
    MamaBasicWildCardSubscriptionCallback*   callback,
    const char*                 source,
    const char*                 symbol,
    void*                       closure)      // = NULL)
{
    // Save arguments in member variables
    mCallback  = callback;
    mClosure   = closure;
    mQueue     = queue;
    mTransport = transport;

    // This static structure contains all of the callback function pointers
    static mamaWildCardMsgCallbacks basicWildCardSubscriptionCallbacks =
    {
        onSubscriptionCreate,
        onSubscriptionError,
        onSubscriptionMessage,
        onSubscriptionDestroy
    };
        
    // Create a new impl
    MamaBasicWildCardSubscriptionImpl *impl = new MamaBasicWildCardSubscriptionImpl(callback, closure, this);
    
    // Create the basic subscription, note that we are registering for the destroy callbacks regardless
    mama_status status = mamaSubscription_createBasicWildCard(
                                mSubscription,
                                transport->getCValue(),
                                queue->getCValue(),
                                &basicWildCardSubscriptionCallbacks,
                                source,
                                symbol,
                                impl);

    // If something went wrong then delete the impl
    if(MAMA_STATUS_OK != status)
    {
        delete impl;
    }

    // Convert the status into an exception
    mamaTry(status);
}

void MamaBasicWildCardSubscription::setTopicClosure (void* closure) 
{
    mamaTry (mamaSubscription_setItemClosure(mSubscription, closure));
}

void* MamaBasicWildCardSubscription::getTopicClosure (void) const
{
    void *itemClosure = NULL;
    mamaTry (mamaSubscription_getItemClosure(mSubscription, &itemClosure));
    return itemClosure;
}

void MamaBasicWildCardSubscription::muteCurrentTopic (void)
{
    mamaTry (mamaSubscription_muteCurrentTopic(mSubscription));
}

const char* MamaBasicWildCardSubscription::getSymbol (void) const
{
    return MamaBasicSubscription::getTopic();
}

const char* MamaBasicWildCardSubscription::getSource (void) const
{
    const char *source  = NULL;
    mamaTry(mamaSubscription_getSource(mSubscription,&source));
    return source;
}

MamaBasicWildCardSubscriptionCallback *MamaBasicWildCardSubscription::getBasicWildCardCallback (void) const
{
    return mCallback;
}

} // namespace wombat
