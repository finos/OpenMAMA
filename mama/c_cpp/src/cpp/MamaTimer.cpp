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
    class TimerTestCallback : public MamaTimerCallback
    {
        // The user callback
        MamaTimerCallback *myUserCallback;

    public:
        
        TimerTestCallback (MamaTimerCallback *userCallback) 
        {
            // Save arguments in member variables
            myUserCallback = userCallback;
        }

        virtual ~TimerTestCallback (void)
        {
        }

        virtual void onDestroy (MamaTimer *timer, void *closure)
        {
            /* Only invoke the user callback if it is of the appropriate type. */
            try 
            {
                // Invoke the user callback
                myUserCallback->onDestroy (timer, closure);
            }
            catch (...)
            {
                // Print a message on an error
                fprintf (stderr, "onDestroy EXCPETION CAUGHT\n");
            }
    }

        virtual void onTimer (MamaTimer* timer)
        {
            try 
            {
                myUserCallback->onTimer(timer);
            }
            catch (...)
            {
                fprintf (stderr, "onTimer EXCPETION CAUGHT\n");
            }
        }	
    };

    /* An instance of this class is allocated each time the timer is created and is passed
     * down into the C layer as the closure.
     * It will be member variables from this class used to invoke callbacks during event processing
     * rather than those associated with the main MamaTimer class. This is done to ensure that
     * the client will have full flexibility in destroying and recreating the timer during the
     * callbacks.
     */
    class MamaTimerImpl
    {
        // The user callback
        MamaTimerCallback *m_callback;

        // The user supplied closure
        void *m_closure;

        // The C++ timer class
        MamaTimer *m_timer;

    public:

        /**
         * The constructor initialises all member variables and creates the TestCallback
         * it if needs to be installed.
         */
        MamaTimerImpl (MamaTimerCallback *callback, void *closure, MamaTimer *timer)
        {
            // Save arguments in member variables
            m_callback  = callback;
            m_closure   = closure;
            m_timer     = timer;

            // If callback exceptions are being caught then install this now
            if (mamaInternal_getCatchCallbackExceptions())
            {
                m_callback = new TimerTestCallback (callback);
            }
        }

        virtual ~MamaTimerImpl (void)
        {
            // Delete the test callback if it was installed
            if ((NULL != m_callback) && (mamaInternal_getCatchCallbackExceptions()))
            {
                delete m_callback;
            }
        }

        void InvokeDestroy (void)
        {
            if(NULL != m_callback)
            {
                // Invoke the onDestroy
                m_callback->onDestroy (m_timer, m_closure);
            }
        }

        void InvokeTick (void)
        {
            // Only invoke the callback if it is supplied
            if(NULL != m_callback)
            {
                m_callback->onTimer(m_timer);
            }
        }
    };

    /* *************************************************** */
    /* MamaTimer Implementation. */
    /* *************************************************** */

    /* *************************************************** */
    /* Constructor and Destructor. */
    /* *************************************************** */

    MamaTimer::MamaTimer (void)
        : myTimer    (NULL)
        , myCallback (NULL)
        , myClosure  (NULL)    
    {
    }

    MamaTimer::~MamaTimer (void) 
    {
        // Destroy the timer if it has not already been done.     
        destroy ();
    }

    /* *************************************************** */
    /* Private Function Implementations. */
    /* *************************************************** */

    /**
     * This handler function is called whenever the timer is completely destroyed
     * and will cause the user callback to be invoked. 
     *
     * @param[in] timer The C timer object.
     * @param[in] closure The closure will be a pointer to the MamaTimer class.
     */
    void MAMACALLTYPE MamaTimer::onTimerDestroy (mamaTimer timer, void *closure)
    {
        // Extract the impl from the closure.
        MamaTimerImpl *impl = (MamaTimerImpl *)closure;
        if(NULL != impl)
        {
            // Invoke the callback
            impl->InvokeDestroy();

            // Destroy the impl as it is no longer needed
            delete impl;
        }
    }

    void MAMACALLTYPE MamaTimer::onTimerTick (mamaTimer timer, void* closure)
    {
        // Extract the impl from the closure.
        MamaTimerImpl *impl = (MamaTimerImpl *)closure;
        if (NULL != impl)
        {
            // Invoke the callback
            impl->InvokeTick();
        }
    }

    /* *************************************************** */
    /* Public Function Implementations. */
    /* *************************************************** */

    void MamaTimer::create (MamaQueue *queue, MamaTimerCallback *callback, mama_f64_t interval, void *closure)
    {	
        // Save arguments in member variables
        myClosure   = closure;
        myCallback  = callback;

        // Create a new impl
        MamaTimerImpl *impl = new MamaTimerImpl (callback, closure, this);

        // Create the timer and register for the destroy callback, the impl will be passed as the closure
        mama_status status = mamaTimer_create2 (&myTimer, 
                                                queue->getCValue(),
                                                onTimerTick,
                                                onTimerDestroy,
                                                interval,
                                                impl);

        // If something went wrong then delete the impl
        if (MAMA_STATUS_OK != status)
        {
            delete impl;
        }

        // Convert the status into an exception
        mamaTry(status);
    }
       
    void MamaTimer::destroy()
    {
        // Only continue if the timer is valid
        if (NULL != myTimer)
        {
            // Destroy the underlying timer, the impl will be cleaned up by the onDestroy callback
            mamaTry (mamaTimer_destroy(myTimer));

            /* Reset all member variables, this must be done now in case the user recreates the timer
             * during any of the callbacks.
             */
            myCallback  = NULL;
            myClosure   = NULL;
            myTimer     = NULL;
        }    
    }

    bool MamaTimer::isActive () const
    {
        return (myTimer != NULL);
    }

    void MamaTimer::reset()
    {
        mamaTry (mamaTimer_reset (myTimer));
    }

    void MamaTimer::setInterval (mama_f64_t seconds)
    {
        mamaTry (mamaTimer_setInterval (myTimer, seconds));
    }

    mama_f64_t MamaTimer::getInterval () const
    {
        mama_f64_t  result = 0;
        mamaTry (mamaTimer_getInterval (myTimer, &result));
        return result;
    }

    void* MamaTimer::getClosure (void) const
    {
        return myClosure;
    }

    MamaTimerCallback* MamaTimer::getCallback (void) const
    {
        return myCallback;
    }

    mamaTimer MamaTimer::getCValue ()
    {
        return myTimer;
    }

    const mamaTimer MamaTimer::getCValue () const
    {
        return myTimer;
    }

} // namespace Wombat
