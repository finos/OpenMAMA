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
#include "mama/MamaBasicSubscription.h"
#include "mama/MamaQueue.h"
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
class BasicSubscriptionTestCallback : public MamaBasicSubscriptionCallback
{
	MamaBasicSubscriptionCallback *mUserCallback;


public:
    
    BasicSubscriptionTestCallback(MamaBasicSubscriptionCallback *userCallback) 
    {
        mUserCallback = userCallback;
    }

    virtual ~BasicSubscriptionTestCallback(void)
    {
    }

	virtual void onDestroy(MamaBasicSubscription *subscription, void *closure)
	{
        try 
        {
            // Invoke the user callback
            MamaBasicSubscriptionCallback *callback = (MamaBasicSubscriptionCallback *)mUserCallback;
            callback->onDestroy(subscription, closure);
        }
        catch (...)
        {
            // Print a message on an error
            fprintf (stderr, "onDestroy EXCPETION CAUGHT\n");
        }
	}

    void onCreate(MamaBasicSubscription *subscription)
    {
	    try {
		    mUserCallback->onCreate(subscription);
	    }
	    catch (...)
	    {
		    fprintf (stderr, "onCreate EXCEPTION CAUGHT\n");
	    }
    }


    void onError(MamaBasicSubscription *subscription, const MamaStatus &status, const char *subject)
    {
	    try {
		    mUserCallback->onError(subscription, status, subject);
	    }
	    catch (...)
	    {
		    fprintf (stderr, "onError EXCEPTION CAUGHT\n");
	    }
    }
								   
    void onMsg(MamaBasicSubscription *subscription, MamaMsg &msg)
    {
	    try {
		    mUserCallback->onMsg(subscription, msg);
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
class MamaBasicSubscriptionImpl
{
    // The C++ subscription class
    MamaBasicSubscription *mBasicSubscription;

    // The user callback
    MamaBasicSubscriptionCallback *mCallback;

    // The user supplied closure
    void *mClosure;

    // The re-usable message
    MamaMsg mResuableMsg;

public:

    MamaBasicSubscriptionImpl(MamaBasicSubscriptionCallback *callback, void *closure, MamaBasicSubscription *basicSubscription)
    {
        // Save arguments in member variables
        mCallback          = callback;
        mClosure           = closure;
        mBasicSubscription = basicSubscription;

        // If callback exceptions are being caught then install this now
        if(mamaInternal_getCatchCallbackExceptions())
	    {
		    mCallback = new BasicSubscriptionTestCallback(callback);
	    }
    }

    virtual ~MamaBasicSubscriptionImpl(void)
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
            mCallback->onCreate(mBasicSubscription);
        }
    }

    void InvokeDestroy(void)
    {
        if(NULL != mCallback)
        {
            // Invoke the onDestroy
            mCallback->onDestroy(mBasicSubscription, mClosure);
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
            mCallback->onError(mBasicSubscription, status, topic);
        }
    }

    void InvokeMsg(mamaMsg msg)
    {
        // Only invoke the callback if it is supplied
        if(NULL != mCallback)
        {
            // Convert the C message to a C++ message            
            mResuableMsg.createFromMsg(msg);

            // Invoke the callback
            mCallback->onMsg(mBasicSubscription, mResuableMsg);
        }
    }
};

/* *************************************************** */
/* MamaBasicSubscription Implementation. */
/* *************************************************** */

/* *************************************************** */
/* Constructor and Destructor. */
/* *************************************************** */

MamaBasicSubscription::MamaBasicSubscription(void)
    : mCallback(NULL),
      mClosure(NULL),
      mQueue(NULL),
      mSubscription(NULL),
      mTransport(NULL)
{
    // Allocate the subscription
    mamaTry(mamaSubscription_allocate(&mSubscription));
}

MamaBasicSubscription::~MamaBasicSubscription(void)
{
    // Destroy the subscription if this has not already been done
    destroy();

    /* Deallocate the subscription, this can't be done in the destroy function as the subscription
     * is allocated in the constructor and the user must be able to recreate from the destroy
     * callback.
     */
    if(NULL != mSubscription)
    {
        mamaSubscription_deallocate(mSubscription);        
    }
}

/* *************************************************** */
/* Private Function Implementations. */
/* *************************************************** */

void MAMACALLTYPE MamaBasicSubscription::onSubscriptionCreate(mamaSubscription subsc, void *closure)
{
    // Extract the impl from the closure.
    MamaBasicSubscriptionImpl *impl = (MamaBasicSubscriptionImpl *)closure;
    if(NULL != impl)
    {
        // Invoke the callback
        impl->InvokeCreate();
    }
}

void MAMACALLTYPE MamaBasicSubscription::onSubscriptionDestroy(mamaSubscription subscription, void *closure)
{
    // Extract the impl from the closure.
    MamaBasicSubscriptionImpl *impl = (MamaBasicSubscriptionImpl *)closure;
    if(NULL != impl)
    {
        // Invoke the callback
        impl->InvokeDestroy();

        // Destroy the impl as it is no longer needed
        delete impl;
    }
}

void MAMACALLTYPE MamaBasicSubscription::onSubscriptionError(mamaSubscription subsc, mama_status status, void *platformError, const char *subject, void *closure)
{
    // Extract the impl from the closure.
    MamaBasicSubscriptionImpl *impl = (MamaBasicSubscriptionImpl *)closure;
    if(NULL != impl)
    {
        // Invoke the callback
        impl->InvokeError(status, subject);
    }
}

void MAMACALLTYPE MamaBasicSubscription::onSubscriptionMessage(mamaSubscription subsc, mamaMsg msg, void *closure, void *itemClosure)
{
    // Extract the impl from the closure.
    MamaBasicSubscriptionImpl *impl = (MamaBasicSubscriptionImpl *)closure;
    if(NULL != impl)
    {
        // Invoke the callback
        impl->InvokeMsg(msg);
    }
}

/* *************************************************** */
/* Public Function Implementations. */
/* *************************************************** */

bool MamaBasicSubscription::checkDebugLevel(MamaLogLevel level) const
{
    return mamaSubscription_checkDebugLevel(mSubscription, level);
}

void MamaBasicSubscription::createBasic(
        MamaTransport*                 transport,
        MamaQueue*                     queue,
        MamaBasicSubscriptionCallback* callback,
        const char*                    topic,
        void*                          closure) // = NULL)
{
    // Save arguments in member variables
    mCallback  = callback;
    mClosure   = closure;
    mQueue     = queue;
    mTransport = transport;

    // This static structure contains all of the callback function pointers
    static mamaMsgCallbacks basicSubscriptionCallbacks =
    {
        onSubscriptionCreate,
        onSubscriptionError,
        onSubscriptionMessage,
        NULL,                           // OnQuality
        NULL,                           // OnGap
        NULL,                           // OnRecapRequest
        onSubscriptionDestroy
    };
        
    // Create a new impl
    MamaBasicSubscriptionImpl *impl = new MamaBasicSubscriptionImpl(callback, closure, this);

    // Create the basic subscription, note that we are registering for the destroy callbacks regardless
    mama_status status = mamaSubscription_createBasic(
                                mSubscription,
                                transport->getCValue(),
                                queue->getCValue(),
                                &basicSubscriptionCallbacks,
                                topic,
                                impl);

    // If something went wrong then delete the impl
    if(MAMA_STATUS_OK != status)
    {
        delete impl;
    }

    // Convert the status into an exception
    mamaTry(status);
}

void MamaBasicSubscription::destroy(void)
{
    // Only continue if the inbox is valid
    if(NULL != mSubscription)
    {
        // Destroy the underlying subscription
        mamaTry(mamaSubscription_destroy(mSubscription));

        /* Reset all member variables, this must be done now in case the user recreates the subscription
         * during any of the callbacks.
         */
        mCallback  = NULL;
        mClosure   = NULL;
        mQueue     = NULL;
        mTransport = NULL;
    }
}

void MamaBasicSubscription::destroyEx(void)
{
    // Only continue if the inbox is valid
    if(NULL != mSubscription)
    {
        // Destroy the underlying subscription
        mamaTry(mamaSubscription_destroyEx(mSubscription));

        /* Reset all member variables, this must be done now in case the user recreates the subscription
         * during any of the callbacks.
         */
        mCallback  = NULL;
        mClosure   = NULL;
        mQueue    = NULL;
        mTransport = NULL;
    }
}

MamaBasicSubscriptionCallback* MamaBasicSubscription::getBasicCallback (void) const
{
    return mCallback;
}

void* MamaBasicSubscription::getClosure(void) const
{
    return mClosure;
}

MamaLogLevel MamaBasicSubscription::getDebugLevel(void) const
{
    return mamaSubscription_getDebugLevel(mSubscription);
}

MamaQueue* MamaBasicSubscription::getQueue(void) const
{
    return mQueue;
}

mamaSubscriptionState MamaBasicSubscription::getState(void)
{
    mamaSubscriptionState ret = MAMA_SUBSCRIPTION_UNKNOWN;
    mamaTry (mamaSubscription_getState(mSubscription, &ret));
    return ret;
}

const char* MamaBasicSubscription::getTopic(void) const 
{
    const char* topic  = NULL;
    mamaTry(mamaSubscription_getSubscSymbol(mSubscription, &topic));
    return topic; 
}

MamaTransport* MamaBasicSubscription::getTransport(void) const
{
    return mTransport;
}

bool MamaBasicSubscription::isActive(void) const
{
    return mamaSubscription_isActive(mSubscription);
}

void MamaBasicSubscription::setClosure(void *closure)
{
    mClosure = closure;
}

void MamaBasicSubscription::setDebugLevel(MamaLogLevel  level)
{
    mamaSubscription_setDebugLevel(mSubscription, level);
}

} // namespace Wombat
