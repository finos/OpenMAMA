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
#include "mama/mamacpp.h"
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
    class InboxTestCallback : public MamaInboxCallback
    {
        // The user callback
	    MamaInboxCallback *myUserCallback;

    public:

        InboxTestCallback (MamaInboxCallback *userCallback) 
        {
            // Save arguments in member variables
            myUserCallback = userCallback;
        }

        virtual ~InboxTestCallback(void)
        {
        }

        virtual void onDestroy (MamaInbox *inbox, void *closure)
	    {
            try 
            {
                // Invoke the user callback
                myUserCallback->onDestroy (inbox, closure);
            }
            catch (...)
            {
                // Print a message on an error
                fprintf (stderr, "MamaInboxCallback onDestroy EXCPETION CAUGHT\n");
            }
	    }

        virtual void onError (MamaInbox* inbox, const MamaStatus& status)
        {
	        try {
		        myUserCallback->onError(inbox, status);
	        }
	        catch (...)
	        {
		        fprintf (stderr, "MamaInboxCallback onError EXCEPTION CAUGHT\n");
	        }
        }
        
        virtual void onMsg (MamaInbox* inbox, MamaMsg& msg)
        {
	        try {
		        myUserCallback->onMsg(inbox, msg);
	        }
	        catch (...)
	        {
		        fprintf (stderr, "MamaInboxCallback onMsg EXCEPTION CAUGHT\n");
	        }
        }
    };

    /* An instance of this class is allocated each time the inbox is created and is passed
     * down into the C layer as the closure.
     * It will be member variables from this class used to invoke callbacks during event processing
     * rather than those associated with the main MamaInbox class. This is done to ensure that
     * the client will have full flexibility in destroying and recreating the inbox during the
     * callbacks.
     */
    class MamaInboxImpl
    {
        // The user callback
        MamaInboxCallback *m_callback;

        // The user supplied closure
        void *m_closure;

        // The C++ inbox class
        MamaInbox *m_inbox;

    public:

        MamaInboxImpl(MamaInboxCallback *callback, void *closure, MamaInbox *inbox)
        {
            // Save arguments in member variables
            m_callback  = callback;
            m_closure   = closure;
            m_inbox     = inbox;

            // If callback exceptions are being caught then install this now
            if (mamaInternal_getCatchCallbackExceptions ())
	        {
		        m_callback = new InboxTestCallback (callback);
	        }
        }

        virtual ~MamaInboxImpl(void)
        {
            // Delete the test callback if it was installed
            if ((NULL != m_callback) && (mamaInternal_getCatchCallbackExceptions ()))
	        {
		        delete m_callback;
	        }
        }

        void InvokeDestroy(void)
        {
            if (NULL != m_callback)
            {
                // Invoke the onDestroy
                m_callback->onDestroy (m_inbox, m_closure);
            }
        }

        void InvokeError (mama_status status)
        {
            // Only invoke the callback if it is supplied
            if (NULL != m_callback)
            {
                // Convert to a C++ status
                MamaStatus cppStatus (status);

                // Invoke the callback funtcion
                m_callback->onError (m_inbox, status);
            }
        }

        void InvokeMsg (mamaMsg msg)
        {
            // Only invoke the callback if it is supplied
            if (NULL != m_callback)
            {
                // Convert the C message to a C++ message
                MamaMsg cppMsg;
                cppMsg.createFromMsg (msg);

                // Invoke the callback
                m_callback->onMsg (m_inbox, cppMsg);
            }
        }
    };

    /* *************************************************** */
    /* MamaInbox Implementation. */
    /* *************************************************** */

    /* *************************************************** */
    /* Constructor and Destructor. */
    /* *************************************************** */

    MamaInbox::MamaInbox (void)
        : mInbox         (NULL)
        , mCallback      (NULL)
        , mClosure       (NULL)      
    {
    }

    MamaInbox::~MamaInbox (void)
    {
        // Destroy the inbox if this has not already been done
        destroy ();
    }

    /* *************************************************** */
    /* Private Function Implementations. */
    /* *************************************************** */

    /**
     * This handler function is called whenever the inbox is completely destroyed
     * and will cause the user callback to be invoked.
     *
     * @param[in] inbox The C inbox object.
     * @param[in] closure The closue will be a pointer to the MamaInbox class.
     */
    void MAMACALLTYPE MamaInbox::onInboxDestroy (mamaInbox inbox, void *closure)
    {
        // Extract the impl from the closure.
        MamaInboxImpl *impl = (MamaInboxImpl  *)closure;
        if (NULL != impl)
        {
            // Invoke the callback
            impl->InvokeDestroy ();

            // Destroy the impl as it is no longer needed
            delete impl;
        }
    }

    void MAMACALLTYPE MamaInbox::onInboxError (mama_status status, void *closure)
    {
        // Extract the impl from the closure.
        MamaInboxImpl *impl = (MamaInboxImpl *)closure;
        if (NULL != impl)
        {
            // Invoke the callback
            impl->InvokeError (status);
        }
    }

    void MAMACALLTYPE MamaInbox::onInboxMsg (mamaMsg msg, void *closure)
    {
        // Extract the impl from the closure.
        MamaInboxImpl *impl = (MamaInboxImpl *)closure;
        if (NULL != impl)
        {
            // Invoke the callback
            impl->InvokeMsg (msg);
        }
    }

    /* *************************************************** */
    /* Public Function Implementations. */
    /* *************************************************** */

    void MamaInbox::create (
        MamaTransport       *tport, 
        MamaQueue           *queue, 
        MamaInboxCallback   *callback, 
        void                *closure)
    {
        // Save arguments in member variables
        mClosure   = closure;
        mCallback  = callback;

        // Create a new impl
        MamaInboxImpl *impl = new MamaInboxImpl (callback, closure, this);

        // Create the timer and register for the destroy callback, the impl will be passed as the closure
        mama_status status = mamaInbox_create2 (&mInbox, 
                                                tport->getCValue (), 
                                                queue->getCValue (),
                                                onInboxMsg, 
                                                onInboxError,
                                                onInboxDestroy,
                                                impl);

        // If something went wrong then delete the impl
        if (MAMA_STATUS_OK != status)
        {
            delete impl;
        }

        // Convert the status into an exception
        mamaTry (status);
    }    

    void MamaInbox::destroy (void)
    {
        // Only continue if the inbox is valid
        if (NULL != mInbox)
        {
            // Destroy the underlying inbox
            mamaTry (mamaInbox_destroy (mInbox));

            /* Reset all member variables, this must be done now in case the user recreates the inbox
             * during any of the callbacks.
             */
            mCallback   = NULL;
            mClosure    = NULL;
            mInbox      = NULL;
        }
    }

    void* MamaInbox::getClosure (void) const
    {
       return mClosure;
    }

    mamaInbox MamaInbox::getCValue ()
    {
        return mInbox;
    }

    const mamaInbox MamaInbox::getCValue () const
    {
        return mInbox;
    }

}
