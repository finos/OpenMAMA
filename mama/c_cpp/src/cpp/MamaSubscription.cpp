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
#include "mama/MamaSource.h"
#include "mama/MamaSourceDerivative.h"
#include "mama/MamaSubscriptionCallback.h"
#include "mamainternal.h"
#include "mamacppinternal.h"
#include "../c/mama/subscription.h"

namespace Wombat
{

    /* *************************************************** */
    /* Private Classes. */
    /* *************************************************** */

    /* This class is used whenever MAMA must catch any exceptions thrown as the result
     * of the user callback doing something wrong.
     * It will translate the exceptions into error messages written to stderr.
     */

    class SubscriptionTestCallback : public MamaSubscriptionCallback
    {
        // The user callback
	    MamaSubscriptionCallback *myUserCallback;

    public:
        
        SubscriptionTestCallback (MamaSubscriptionCallback *userCallback) 
        {
            // Save arguments in member variables        
            myUserCallback = userCallback;
        }

        virtual ~SubscriptionTestCallback (void)
        {
        }
	
        virtual void onCreate (MamaSubscription *subscription)
        {
	        try 
            {
		        myUserCallback->onCreate (subscription);
	        }
	        catch (...)
	        {
		        fprintf (stderr, "TestCallback::onCreate EXCEPTION CAUGHT\n");
	        }
        }

        virtual void onDestroy (MamaSubscription *subscription)
        {
	        try 
            {
		        myUserCallback->onDestroy (subscription);
	        }
	        catch (...)
	        {
		        fprintf (stderr, "TestCallback::onDestroy EXCEPTION CAUGHT\n");
	        }
        }

        virtual void onError (MamaSubscription  *subscription, 
                              const MamaStatus  &status, 
                              const char        *subject)
        {
	        try 
            {
		        myUserCallback->onError(subscription, 
                                        status, 
                                        subject);
	        }
	        catch (...)
	        {
		        fprintf (stderr, "TestCallback::onError EXCEPTION CAUGHT\n");
	        }
        }

        virtual void onGap (MamaSubscription *subscription)
        {
	        try 
            {
		        myUserCallback->onGap (subscription);
	        }
	        catch (...)
	        {
		        fprintf (stderr, "TestCallback::onGap EXCEPTION CAUGHT\n");
	        }
        }
							       
        virtual void onMsg (MamaSubscription  *subscription, 
                            MamaMsg           &msg)
        {
	        try 
            {
		        myUserCallback->onMsg (subscription, msg);
	        }
	        catch (...)
	        {
		        fprintf (stderr, "TestCallback::onMsg EXCEPTION CAUGHT\n");
	        }
        }

        virtual void onQuality (MamaSubscription  *subscription,
                                mamaQuality       quality,
                                const char*       symbol,
                                short             cause,
                                const void*       platformInfo)
        {
	        try 
            {
		        myUserCallback->onQuality (subscription, 
                                           quality, 
                                           symbol, 
                                           cause, 
                                           platformInfo);
	        }
	        catch (...)
	        {
		        fprintf (stderr, "TestCallback::onquality EXCEPTION CAUGHT\n");
	        }
        }

        virtual void onRecapRequest(MamaSubscription *subscription)
        {
	        try 
            {
		        myUserCallback->onRecapRequest(subscription);
	        }
	        catch (...)
	        {
		        fprintf (stderr, "TestCallback::onRecapRequest EXCEPTION CAUGHT\n");
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
    class MamaSubscriptionImpl
    {
        // The user callback
        MamaSubscriptionCallback *mCallback;

        // The user supplied closure
        void *mClosure;

        // The re-usable message
        MamaMsg mResuableMsg;

        // The C++ subscription class
        MamaSubscription *mSubscription;

    public:

        MamaSubscriptionImpl (MamaSubscriptionCallback *callback, 
                              void                     *closure, 
                              MamaSubscription         *subscription)
        {
            // Save arguments in member variables
            mCallback      = callback;
            mClosure       = closure;
            mSubscription  = subscription;

            // If callback exceptions are being caught then install this now
            if(mamaInternal_getCatchCallbackExceptions ())
	        {
		        mCallback = new SubscriptionTestCallback (callback);
	        }
        }

        virtual ~MamaSubscriptionImpl (void)
        {
            // Delete the test callback if it was installed
            if ((NULL != mCallback) && mamaInternal_getCatchCallbackExceptions ())
	        {
		        delete mCallback;
	        }
        }

        void InvokeCreate (void)
        {
            // Only invoke the callback if it is supplied
            if (NULL != mCallback)
            {
                // Invoke the callback funtcion
                mCallback->onCreate (mSubscription);
            }
        }

        void InvokeDestroy (void)
        {
            // Only invoke the callback if it is supplied
            if (NULL != mCallback)
            {
                // Invoke the onDestroy            
                mCallback->onDestroy (mSubscription);
            }
        }

        void InvokeError (mama_status  status, 
                          const char   *topic)
        {
            // Only invoke the callback if it is supplied
            if (NULL != mCallback)
            {
                // Convert to a C++ status
                MamaStatus cppStatus (status);

                // Invoke the callback funtcion
                mCallback->onError (mSubscription, 
                                     status, 
                                     topic);
            }
        }

        void InvokeGap (void)
        {
            // Only invoke the callback if it is supplied
            if (NULL != mCallback)
            {
                // Invoke the callback funtcion
                mCallback->onGap (mSubscription);
            }
        }

        void InvokeMsg (mamaMsg msg)
        {
            // Only invoke the callback if it is supplied
            if (NULL != mCallback)
            {
                // Convert the C message to a C++ message            
                mResuableMsg.createFromMsg (msg);

                // Invoke the callback
                mCallback->onMsg (mSubscription, mResuableMsg);
            }
        }

        void InvokeQuality (mamaQuality  quality, 
                            const char   *symbol, 
                            short        cause, 
                            const void   *platformInfo)
        {
            // Only invoke the callback if it is supplied
            if (NULL != mCallback)
            {
                // Invoke the callback
                mCallback->onQuality (mSubscription, 
                                       quality, 
                                       symbol, 
                                       cause, 
                                       platformInfo);
            }
        }

        void InvokeRecapRequest (void)
        {
            // Only invoke the callback if it is supplied
            if (NULL != mCallback)
            {
                // Invoke the callback
                mCallback->onRecapRequest (mSubscription);
            }
        }
    };

    /* *************************************************** */
    /* MamaSubscription Implementation. */
    /* *************************************************** */

    /* *************************************************** */
    /* Constructor and Destructor. */
    /* *************************************************** */

    MamaSubscription::MamaSubscription (void) 
        : MamaBasicSubscription ()
        , mCallback            (NULL)
        , mSourceDeriv         (NULL)
    {
    }

    MamaSubscription::~MamaSubscription (void) 
    {
        // Delete the source derivative
        if (NULL != mSourceDeriv)
        {
            delete mSourceDeriv;        
        }
    }

    /* *************************************************** */
    /* Private Function Implementations. */
    /* *************************************************** */

    void MAMACALLTYPE MamaSubscription::onSubscriptionCreate (mamaSubscription  subscription, 
                                                              void              *closure)
    {
        // Extract the impl from the closure.
        MamaSubscriptionImpl *impl = (MamaSubscriptionImpl *)closure;
        if (NULL != impl)
        {
            // Invoke the callback
            impl->InvokeCreate ();
        }
    }

    void MAMACALLTYPE MamaSubscription::onSubscriptionDestroy (mamaSubscription  subscription, 
                                                               void              *closure)
    {
        // Extract the impl from the closure.
        MamaSubscriptionImpl *impl = (MamaSubscriptionImpl *)closure;

        if (NULL != impl)
        {
            /* Get the state of the subscription before the callback is invoked, note that we must get the
             * state from the C subscription and not the C++ object in the impl as the C++ class could
             * have been delete by this stage whereas the C object will remain valid until at least after
             * this callback has completed.
             */
            mamaSubscriptionState state = MAMA_SUBSCRIPTION_UNKNOWN;
            mamaSubscription_getState (subscription, &state);

            /* If the C++ subscription has been deleted then the state will be MAMA_SUBSCRIPTION_DEALLOCATING,
             * in this case we do not invoke the destroy callback as the callback object will probably
             * have been deleted as well.
             */
            if (MAMA_SUBSCRIPTION_DEALLOCATING != state)
            {
                // Invoke the callback
                impl->InvokeDestroy ();
            }
            
            /* The impl must be deleted if we are destroying or deallocating the subscription, note that this
             * callback can also occur during de-activation.
             */
            if ((MAMA_SUBSCRIPTION_DESTROYED == state) || (MAMA_SUBSCRIPTION_DEALLOCATING == state))
            {
                // Destroy the impl as it is no longer needed
                delete impl;
            }
        }
    }

    void MAMACALLTYPE MamaSubscription::onSubscriptionError (mamaSubscription  subscription, 
                                                             mama_status       status, 
                                                             void              *platformError, 
                                                             const char        *subject, 
                                                             void              *closure)
    {
        // Extract the impl from the closure.
        MamaSubscriptionImpl *impl = (MamaSubscriptionImpl *)closure;

        if (NULL != impl)
        {
            // Invoke the callback
            impl->InvokeError (status, subject);
        }
    }

    void MAMACALLTYPE MamaSubscription::onSubscriptionGap (mamaSubscription  subscription,
                                                           void              *closure)
    {
        // Extract the impl from the closure.
        MamaSubscriptionImpl *impl = (MamaSubscriptionImpl *)closure;

        if (NULL != impl)
        {
            // Invoke the callback
            impl->InvokeGap ();
        }
    }

    void MAMACALLTYPE MamaSubscription::onSubscriptionMessage (mamaSubscription  subscription, 
                                                               mamaMsg           msg, 
                                                               void              *closure, 
                                                               void              *topicClosure)
    {
        // Extract the impl from the closure.
        MamaSubscriptionImpl *impl = (MamaSubscriptionImpl *)closure;

        if (NULL != impl)
        {
            // Invoke the callback
            impl->InvokeMsg (msg);
        }
    }

    void MAMACALLTYPE MamaSubscription::onSubscriptionQuality (mamaSubscription  subscription, 
                                                               mamaQuality       quality, 
                                                               const char        *symbol, 
                                                               short             cause, 
                                                               const void        *platformInfo, 
                                                               void              *closure)
    {
        // Extract the impl from the closure.
        MamaSubscriptionImpl *impl = (MamaSubscriptionImpl *)closure;
        if (NULL != impl)
        {
            // Invoke the callback
            impl->InvokeQuality (quality, symbol, cause, platformInfo);
        }
    }

    void MAMACALLTYPE MamaSubscription::onSubscriptionRecapRequest (mamaSubscription  subscription, 
                                                                    void              *closure)
    {
        // Extract the impl from the closure.
        MamaSubscriptionImpl *impl = (MamaSubscriptionImpl *)closure;
        if (NULL != impl)
        {
            // Invoke the callback
            impl->InvokeRecapRequest ();
        }
    }

    /* *************************************************** */
    /* Public Function Implementations. */
    /* *************************************************** */

    void MamaSubscription::create (MamaTransport*              transport,
                                   MamaQueue*                  queue,
                                   MamaSubscriptionCallback*   callback,
                                   const char*                 source,
                                   const char*                 symbol,
                                   void*                       closure)
    {
        setup (transport, queue, callback, source, symbol, closure);
        activate();
    }

    void MamaSubscription::create (MamaQueue*                  queue,
                                   MamaSubscriptionCallback*   callback,
                                   MamaSource*                 source,
                                   const char*                 symbol,
                                   void*                       closure)
    {
        setup (queue, callback, source, symbol, closure);
        activate();
    }

    void MamaSubscription::createSnapshot (MamaTransport*               transport,
                                           MamaQueue*                   queue,
                                           MamaSubscriptionCallback*    callback,
                                           const char*                  source,
                                           const char*                  symbol,
                                           void*                        closure)
    {
        setServiceLevel (MAMA_SERVICE_LEVEL_SNAPSHOT, 0);
        setup (transport, queue, callback, source, symbol, closure);
        activate();
    }

    void MamaSubscription::destroy (void)
    {
        // Call the base class to perform the work
        MamaBasicSubscription::destroy ();

        // Clear the callback in this class
        mCallback = NULL;
    }

    void MamaSubscription::destroyEx(void)
    {
        // Call the base class to perform the work
        MamaBasicSubscription::destroyEx();

        // Clear the callback in this class
        mCallback = NULL;
    }


    void MamaSubscription::setup (MamaTransport*              transport,
                                  MamaQueue*                  queue,
                                  MamaSubscriptionCallback*   callback,
                                  const char*                 source,
                                  const char*                 symbol,
                                  void*                       closure)
        
    {
        // This static structure contains all of the callback function pointers
        static mamaMsgCallbacks subscriptionCallbacks =
        {
            onSubscriptionCreate,
            onSubscriptionError,
            onSubscriptionMessage,
            onSubscriptionQuality,
            onSubscriptionGap,
            onSubscriptionRecapRequest,
		    onSubscriptionDestroy
        };

        // Save the callback in the member variable
        mCallback = callback;
        mClosure  = closure;

        // Create a new impl
        MamaSubscriptionImpl *impl = new MamaSubscriptionImpl (callback, closure, this);

        // Delete the source deriviative if it is valid
        if (NULL != mSourceDeriv)
        {
            delete mSourceDeriv;
            mSourceDeriv = NULL;
        }
	
        // Setup the MAMA subscription, it will be activated later
        mama_status status = mamaSubscription_setup2 (mSubscription,
                                                      transport->getCValue (),
                                                      queue->getCValue (),
                                                      &subscriptionCallbacks,
                                                      source,
                                                      symbol,
                                                      impl);

        // If something went wrong then delete the impl
        if(MAMA_STATUS_OK != status)
        {
            delete impl;
        }

        // Convert the status into an exception
        mamaTry (status);
    }

    void MamaSubscription::setup (MamaQueue*                  queue,
                                  MamaSubscriptionCallback*   callback,
                                  MamaSource*                 source,
                                  const char*                 symbol,
                                  void*                       closure)
        
    {
        // This static structure contains all of the callback function pointers
        static mamaMsgCallbacks subscriptionCallbacks =
        {
            onSubscriptionCreate,
            onSubscriptionError,
            onSubscriptionMessage,
            onSubscriptionQuality,
            onSubscriptionGap,
            onSubscriptionRecapRequest,
		    onSubscriptionDestroy
        };

        // Save the callback in the member variable
        mCallback = callback;
        mClosure  = closure;

        // Create the source derivative
        if (NULL == mSourceDeriv)
        {
            mSourceDeriv = new MamaSourceDerivative (source);
        }

        // Create a new impl
        MamaSubscriptionImpl *impl = new MamaSubscriptionImpl (callback, 
                                                               closure, 
                                                               this);

        // Setup the MAMA subscription, it will be activated later
        mama_status status = mamaSubscription_setup (mSubscription,
                                                     queue->getCValue (),
                                                     &subscriptionCallbacks,
                                                     source->getCValue (),
                                                     symbol,
                                                     impl);

        // If something went wrong then delete the impl
        if (MAMA_STATUS_OK != status)
        {
            delete impl;
        }

        // Convert the status into an exception
        mamaTry (status);
    }

    mamaSubscription MamaSubscription::getCSubscription (void)
    {
        return mSubscription;
    }

    void MamaSubscription::setCSubscription (mamaSubscription subscription)
    {
        mSubscription = subscription;
    }

    void MamaSubscription::activate (void)
    {
        mamaTry (mamaSubscription_activate (mSubscription));
    }

    void MamaSubscription::deactivate (void)
    {
        mamaTry (mamaSubscription_deactivate (mSubscription));
    }

    MamaSourceDerivative* MamaSubscription::getSourceDerivative ()
    {
        return mSourceDeriv;
    }

    const MamaSourceDerivative* MamaSubscription::getSourceDerivative (void) const
    {
        return mSourceDeriv;
    }

    const MamaSource* MamaSubscription::getSource (void) const
    {
        return mSourceDeriv ? mSourceDeriv->getBaseSource () : NULL;
    }

    void MamaSubscription::setRequiresInitial (bool requiresInitial)
    {
        mamaTry(mamaSubscription_setRequiresInitial (mSubscription, 
                                                     requiresInitial == true ? 1 : 0));
    }

    bool MamaSubscription::getRequiresInitial (void)
    {
        int  requiresInitial = 0;
        mamaTry (mamaSubscription_getRequiresInitial (mSubscription, 
                                                      &requiresInitial));
        return requiresInitial == 1 ? true : false;
    }

    bool MamaSubscription::getReceivedInitial (void)
    {
        int  receivedInitial = 0;
        mamaTry(mamaSubscription_getReceivedInitial (mSubscription,
                                                     &receivedInitial));
        return receivedInitial == 1 ? true : false;
    }
        
    void MamaSubscription::setRetries (int retries)
    {
        if (NULL != mSubscription)
        {
            mamaTry (mamaSubscription_setRetries (mSubscription, 
                                                  retries));
        }
    }

    int MamaSubscription::getRetries (void)
    {
        int retries = 0;
        mamaTry (mamaSubscription_getRetries (mSubscription, 
                                              &retries));
        return retries;
    }
        
    void MamaSubscription::setSubscriptionType (mamaSubscriptionType type)
    {
        mamaTry (mamaSubscription_setSubscriptionType (mSubscription,
                                                       type));
    }

    mamaSubscriptionType MamaSubscription::getSubscriptionType (void)
    {
        mamaSubscriptionType type;
        mamaTry (mamaSubscription_getSubscriptionType (mSubscription,
                                                       &type));
        return type;
    }

    void MamaSubscription::setAppDataType (uint8_t dataType)
    {
        mamaTry (mamaSubscription_setAppDataType (mSubscription, 
                                                  dataType));
    }

    uint8_t MamaSubscription::getAppDataType (void) const
    {
        uint8_t dataType = 0U;
        mamaTry (mamaSubscription_getAppDataType (mSubscription,
                                                  &dataType));
        return dataType;
    }

    void MamaSubscription::setItemClosure (void *closure) 
    {
        mamaTry (mamaSubscription_setItemClosure (mSubscription, 
                                                  closure));
    }

    void* MamaSubscription::getItemClosure (void) 
    {
        void *itemClosure = NULL;
        mamaTry (mamaSubscription_getItemClosure (mSubscription, 
                                                  &itemClosure));
        return itemClosure;
    }

    void MamaSubscription::setPreInitialCacheSize (int cacheSize) 
    {
        mamaTry (mamaSubscription_setPreIntitialCacheSize (mSubscription, 
                                                           cacheSize));
    }

    int MamaSubscription::getPreInitialCacheSize (void) 
    {
        int result = 0;
        mamaTry (mamaSubscription_getPreIntitialCacheSize (mSubscription, 
                                                           &result));
        return result;
    }
        
    void MamaSubscription::setMsgQualifierFilter (bool ignoreDefinitelyDuplicate,
                                                  bool ignorePossiblyDuplicate,
                                                  bool ignoreDefinitelyDelyaed,
                                                  bool ignorePossiblyDelayed,
                                                  bool ignoreOutOfSequence)
    {
        mamaTry (mamaSubscription_setMsgQualifierFilter (mSubscription,                                                       
                                                         ignoreDefinitelyDuplicate,
                                                         ignorePossiblyDuplicate,
                                                         ignoreDefinitelyDelyaed,
                                                         ignorePossiblyDelayed,
                                                         ignoreOutOfSequence));
    }

    void MamaSubscription::getMsgQualifierFilter (bool &ignoreDefinitelyDuplicate,
                                                  bool &ignorePossiblyDuplicate,
                                                  bool &ignoreDefinitelyDelyaed,
                                                  bool &ignorePossiblyDelayed,
                                                  bool &ignoreOutOfSequence) const
    {
        int iDDup = 0;
        int iPDup = 0;
        int iDDel = 0;
        int iPDel = 0;
        int iSeq  = 0;

        mamaTry (mamaSubscription_getMsgQualifierFilter (mSubscription,
                                                         &iDDup,
                                                         &iPDup,
                                                         &iDDel,
                                                         &iPDel,
                                                         &iSeq));

        ignoreDefinitelyDuplicate = iDDup != 0;
        ignorePossiblyDuplicate   = iPDup != 0;  
        ignoreDefinitelyDelyaed   = iDDel != 0;
        ignorePossiblyDelayed     = iPDel != 0;
        ignoreOutOfSequence       = iSeq  != 0;
    }
        
    void MamaSubscription::setSymbol (const char* symbol)
    {
        mamaSubscription_setSymbol (mSubscription, 
                                    symbol);
    }

    void MamaSubscription::setServiceLevel (mamaServiceLevel svcLevel, 
                                            long             svcLevelOpt)
    {
        mamaTry (mamaSubscription_setServiceLevel (mSubscription, 
                                                   svcLevel, 
                                                   svcLevelOpt));
    }

    mamaServiceLevel MamaSubscription::getServiceLevel (void)
    {
        mamaServiceLevel svcLevel;
        mamaTry (mamaSubscription_getServiceLevel (mSubscription, 
                                                   &svcLevel));
        return svcLevel;
    }
        
    long MamaSubscription::getServiceLevelOpt (void)
    {
        long svcLevelOpt = 0;
        mamaTry (mamaSubscription_getServiceLevelOpt (mSubscription,
                                                      &svcLevelOpt));
        return svcLevelOpt;
    }

    const char *MamaSubscription::getSymbol (void) const
    {
        return getTopic ();
    }

    MamaSubscriptionCallback *MamaSubscription::getCallback (void) const
    {
        return mCallback;
    }

    const char* MamaSubscription::getSubscSource (void) const
    {
        const char* source  = NULL;
        mamaTry (mamaSubscription_getSource (mSubscription, 
                                             &source));
        return source;
    }

    void MamaSubscription::setTimeout (double timeout)
    {
        if (NULL != mSubscription)
        {
            mamaTry (mamaSubscription_setTimeout (mSubscription, 
                                                  timeout));
        }
    }

    double MamaSubscription::getTimeout (void)
    {
        double timeout = 0.0;
        mamaTry (mamaSubscription_getTimeout (mSubscription, 
                                              &timeout));
        return timeout;
    }

    void MamaSubscription::setRecoverGaps (bool recover)
    {
        mamaTry (mamaSubscription_setRecoverGaps (mSubscription, 
                                                  recover ? 1 : 0));
    }

    bool MamaSubscription::getRecoverGaps (void) const
    {
        int result = 0;
        mamaTry (mamaSubscription_getRecoverGaps (mSubscription, 
                                                  &result));
        return result != 0;
    }

    void MamaSubscription::setGroupSizeHint (int groupSizeHint)
    {
        if (NULL != mSubscription)
        {
            mamaTry(mamaSubscription_setGroupSizeHint (mSubscription, 
                                                       groupSizeHint));
        }
    }

} // namespace wombat
