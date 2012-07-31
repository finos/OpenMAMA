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

#include "mama/mamacpp.h"
#include "mamacppinternal.h"
#include "mamainternal.h"

namespace Wombat 
{

    class QueueEnqueueTestCallback : public MamaQueueEnqueueCallback
    {
    public:
	    QueueEnqueueTestCallback (MamaQueueEnqueueCallback* usercallback) 
        {
            mCallback = usercallback;
        }

	    virtual ~QueueEnqueueTestCallback () 
        {
        };

        virtual void onEventEnqueue (void* closure);

    private:
	    MamaQueueEnqueueCallback* mCallback;
    };

    void QueueEnqueueTestCallback::onEventEnqueue (void* closure)
    {
	    try 
        {
		    mCallback->onEventEnqueue (closure);
	    }
	    catch (...)
	    {
		    fprintf (stderr, "MamaQueueEnqueueCallback onEventQueue EXCEPTION CAUGHT\n");
	    }
    }

    class QueueEventTestCallback : public MamaQueueEventCallback
    {
    public:
	    QueueEventTestCallback (MamaQueueEventCallback* usercallback) 
        {
            mCallback = usercallback;
        }

        virtual ~QueueEventTestCallback () 
        {
        };

        virtual void onEvent (MamaQueue&  queue, 
                              void*       closure);

    private:
	    MamaQueueEventCallback* mCallback;
    };

    void QueueEventTestCallback::onEvent (MamaQueue&  queue, 
                                          void*       closure)
    {
	    try 
        {
		    mCallback->onEvent (queue, closure);
	    }
	    catch (...)
	    {
		    fprintf (stderr, "MamaQueueEventCallback onEvent EXCEPTION CAUGHT\n");
	    }
    }

    class QueueMonitorTestCallback : public MamaQueueMonitorCallback
    {
    public:
	    QueueMonitorTestCallback (MamaQueueMonitorCallback* usercallback) 
        {
            mCallback = usercallback;
        }

        virtual ~QueueMonitorTestCallback () 
        {
        }

        virtual void onHighWatermarkExceeded (MamaQueue*  queue, 
                                              size_t      size, 
                                              void*       closure);

        virtual void onLowWatermark (MamaQueue*  queue, 
                                     size_t      size, 
                                     void        *closure);

    private:
	    MamaQueueMonitorCallback* mCallback;
    };

    void QueueMonitorTestCallback::onHighWatermarkExceeded (MamaQueue*  queue, 
                                                            size_t      size, 
                                                            void*       closure)
    {
	    try 
        {
		    mCallback->onHighWatermarkExceeded (queue, size, closure);
	    }
	    catch (...)
	    {
		    fprintf (stderr, "MamaQueueMonitorCallback onHighWaterMarkExceeded EXCEPTION CAUGHT\n");
	    }
    }

    void QueueMonitorTestCallback::onLowWatermark (MamaQueue*  queue, 
                                                   size_t      size, 
                                                   void        *closure)
    {
	    try 
        {
		    mCallback->onLowWatermark (queue, size, closure);
	    }
	    catch (...)
	    {
		    fprintf (stderr, "MamaQueueMonitorCallback onLowWatermark EXCEPTION CAUGHT\n");
	    }
    }


    /*Used to store context data for the sendWithThrotle methods*/
    struct MamaQueueEnqueueEventClosure
    {
        MamaQueueEventCallback* mClientCallback;
        MamaQueue*              mQueue;
        void*                   mClientClosure;
    };

    struct MamaQueue::MamaQueueImpl 
    {
    public:
        MamaQueueImpl           (void)
            : mClosure          (NULL)
            , mEnqueueCallback  (NULL)
        	, mUserClosure      (NULL)
        {
        }

        void*                     mClosure;
        MamaQueueEnqueueCallback* mEnqueueCallback;
        MamaQueueMonitorCallback* mMonitorCallback;
        void*                     mMonitorCallbackClosure;
        void*                     mUserClosure;
    };



    MamaQueue::MamaQueue (void)
        : mPimpl (new MamaQueueImpl)
        , mQueue (NULL)
    {
    }

    MamaQueue::~MamaQueue (void)
    {
        destroy ();
        delete mPimpl;
    }

    void MamaQueue::create (mamaBridge bridgeImpl)
    {
        mamaTry (mamaQueue_create (&mQueue, bridgeImpl));
    }

    void MamaQueue::create (mamaBridge  bridgeImpl, 
                            void*       nativeQueue)
    {
        mamaTry (mamaQueue_create_usingNative (&mQueue, 
                                               bridgeImpl, 
                                               nativeQueue));
    }

    void MamaQueue::dispatch ()
    {
        mamaTry (mamaQueue_dispatch (mQueue));
    }

    void MamaQueue::timedDispatch (uint64_t timeout)
    {
        mamaTry (mamaQueue_timedDispatch (mQueue, timeout));
    }

    void MamaQueue::dispatchEvent ()
    {
        mamaTry (mamaQueue_dispatchEvent (mQueue));
    }

    size_t MamaQueue::getEventCount ()
    {
        size_t eventCount = 0;
        mamaTry (mamaQueue_getEventCount (mQueue, &eventCount));
        return eventCount;
    }

    void MamaQueue::stopDispatch ()
    {
        mamaTry (mamaQueue_stopDispatch (mQueue));
    }

    extern "C"
    {
        /*The C level callback function for queue limit exceeded events*/
        static void MAMACALLTYPE queueLimitExceeded (mamaQueue  queueC,
                                        size_t     size,
                                        void*      closure)
        {
            MamaQueue* queue = static_cast<MamaQueue*> (closure);

            if (queue->mPimpl->mMonitorCallback)
            {
                queue->mPimpl->mMonitorCallback->onHighWatermarkExceeded (
                        queue, 
                        size,
                        queue->mPimpl->mMonitorCallbackClosure);
            }
        }


        static void MAMACALLTYPE queueLowWater (mamaQueue  queueC,
                                   size_t     size,
                                   void*      closure)
        {
            MamaQueue* queue = static_cast<MamaQueue*> (closure);

            if (queue->mPimpl->mMonitorCallback)
            {
                queue->mPimpl->mMonitorCallback->onLowWatermark (
                        queue,
                        size,
                        queue->mPimpl->mMonitorCallbackClosure);
            }
        }

        static void MAMACALLTYPE queueEventCallBack  (mamaQueue  queue,
                                         void*      closure)
        {
            MamaQueueEnqueueEventClosure* closureData 
                    = (MamaQueueEnqueueEventClosure*)closure;

            if (!closureData)
            {
                return;
            }

            closureData->mClientCallback->onEvent (*closureData->mQueue,
                                                   closureData->mClientClosure);

            delete (closureData);
        }
    }/* end extern C */

    void MamaQueue::setQueueMonitorCallback (MamaQueueMonitorCallback*  cb,
                                             void*                      closure)
    {
        /* The C sturcture for callbacks */
        mamaQueueMonitorCallbacks queueMonitorCallbacks;

        queueMonitorCallbacks.onQueueHighWatermarkExceeded = queueLimitExceeded;
        queueMonitorCallbacks.onQueueLowWatermark          = queueLowWater;

        /* We store this in the impl for subsequent callbacks */
	    if (mamaInternal_getCatchCallbackExceptions ())
	    {
		    mPimpl->mMonitorCallback = new QueueMonitorTestCallback (cb);
	    }
	    else
	    {
		    mPimpl->mMonitorCallback = cb;
	    }

        mPimpl->mMonitorCallbackClosure = closure;

        mamaTry (mamaQueue_setQueueMonitorCallbacks (mQueue, 
                                                     &queueMonitorCallbacks,
                                                     this));
    }

    void MamaQueue::setHighWatermark (size_t highWatermark)
    {
        mamaTry (mamaQueue_setHighWatermark (mQueue, highWatermark));
    }

    size_t MamaQueue::getHighWatermark (void) const
    {
        size_t highWaterMark = 0;
        mamaTry (mamaQueue_getHighWatermark (mQueue, &highWaterMark));

        return highWaterMark;
    }

    void MamaQueue::setLowWatermark (size_t lowWatermark)
    {
        mamaTry (mamaQueue_setLowWatermark (mQueue, lowWatermark));
    }

    size_t MamaQueue::getLowWatermark (void) const
    {
        size_t lowWaterMark = 0;
        mamaTry (mamaQueue_getLowWatermark (mQueue, &lowWaterMark));

        return lowWaterMark;
    }

    void MamaQueue::setQueueName (const char* name)
    {
        mamaTry (mamaQueue_setQueueName (mQueue, name));
    }

    const char* MamaQueue::getQueueName () const
    {
        const char* queueName = NULL;
        mamaTry (mamaQueue_getQueueName (mQueue, &queueName)); 

        return queueName;
    }

    const char* MamaQueue::getQueueBridgeName () const
    {
        const char* queueBridgeName = NULL;
        mamaTry (mamaQueue_getQueueBridgeName (mQueue, &queueBridgeName));

        return queueBridgeName;
    }

    extern "C"
    {
        static void MAMACALLTYPE enqueueCallback (mamaQueue cQueue, void* closure)
        {
            MamaQueue* queue = static_cast<MamaQueue*> (closure);
            if (queue && queue->mPimpl->mEnqueueCallback)
            {
                queue->mPimpl->mEnqueueCallback->onEventEnqueue(
                    queue->mPimpl->mClosure);
            }
        }
    }

    void MamaQueue::setEnqueueCallback (MamaQueueEnqueueCallback* cb,
                                        void*                     closure)
    {
        if (mQueue)
        {
            /* Store these in the impl for use in the C callback function */
            mPimpl->mClosure = closure; 

		    if (mamaInternal_getCatchCallbackExceptions ())
		    {
			    mPimpl->mEnqueueCallback = new QueueEnqueueTestCallback (cb);
		    }
		    else
		    {
			    mPimpl->mEnqueueCallback = cb;
		    }

            /* We are using the impl instance as the closure to the call */
            mamaTry (mamaQueue_setEnqueueCallback (mQueue, 
                                                   enqueueCallback, 
                                                   this));
        }
    }

    void MamaQueue::enqueueEvent  (MamaQueueEventCallback*  cb,
                                   void*                    closure)
    {
        if (mQueue)
        {
            mama_status status = MAMA_STATUS_OK;

            MamaQueueEnqueueEventClosure* closureData =
                        new MamaQueueEnqueueEventClosure;

            /* Store these in the closure for use in the C callback function */
            closureData->mQueue          = this;
            closureData->mClientCallback = cb;
            closureData->mClientClosure  = closure;

            /* Checking the status here rather that using mamaTry so we can
               clean up */
            status = mamaQueue_enqueueEvent (mQueue,
                                             &queueEventCallBack,
                                             closureData);
            if (MAMA_STATUS_OK != status)
            {
                delete (closureData);
                throw MamaStatus (status);
            }
        }   
    }

    void MamaQueue::enqueueEvent  (MamaQueueEventCallback&  cb,
                                   void*                    closure)
    {
        if (mQueue)
        {
            mama_status status = MAMA_STATUS_OK;

            MamaQueueEnqueueEventClosure* closureData =
                        new MamaQueueEnqueueEventClosure;
            /* Store these in the closure for use in the C callback function */
           
            closureData->mQueue          = this;
            closureData->mClientCallback = &cb;
            closureData->mClientClosure  = closure;

            /* Checking the status here rather that using mamaTry so we can
               clean up */
            status = mamaQueue_enqueueEvent (mQueue,
                                             &queueEventCallBack,
                                             closureData);
           if (MAMA_STATUS_OK != status)
            {
                delete (closureData);
                throw MamaStatus (status);
            }
        }   
    }

    void MamaQueue::destroyInternal(mama_status status)
    {
        // If there is an error then throw an exception
        if(MAMA_STATUS_OK != status)
        {
            throw MamaStatus(status);
        }

        // Reset the queue pointer
        mQueue = NULL;
        
        // Delete the callback monitor
        if (mamaInternal_getCatchCallbackExceptions())
	    {
		    if (mPimpl->mMonitorCallback != NULL)
		    {
			    delete mPimpl->mMonitorCallback;
			    mPimpl->mMonitorCallback = 0;
		    }

		    if (mPimpl->mEnqueueCallback != NULL)
		    {
			    delete mPimpl->mEnqueueCallback;
			    mPimpl->mEnqueueCallback = 0;
		    }
	    }
    }

    void MamaQueue::destroy ()
    {	
        mama_status status = MAMA_STATUS_OK;

        if (mQueue)
        {
            status = mamaQueue_destroy (mQueue);        
        }

        destroyInternal(status);
    }

    void MamaQueue::destroyTimedWait (long timeout)
    {
        mama_status status = MAMA_STATUS_OK;

        if (mQueue)
        {
            status = mamaQueue_destroyTimedWait (mQueue, timeout);        
        }

        destroyInternal (status);
    }

    void MamaQueue::destroyWait ()
    {
        mama_status status = MAMA_STATUS_OK;

        if (mQueue)
        {
            status = mamaQueue_destroyWait (mQueue);        
        }

        destroyInternal (status);
    }

    mamaQueue MamaQueue::getCValue ()
    {
        return mQueue;
    }

    const mamaQueue MamaQueue::getCValue () const
    {
        return mQueue;
    }

    void MamaQueue::setCValue (mamaQueue cQueue)
    {
        if (!mQueue) 
        {
            mQueue = cQueue;
        }
    }

    void MamaQueue::setClosure (void* closure)
    {
	    mPimpl->mUserClosure = closure;
    }

    void* MamaQueue::getClosure ()
    {
	    return mPimpl->mUserClosure;
    }

} // namespace Wombat
