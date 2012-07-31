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

#ifndef MAMA_QUEUE_CPP_H__
#define MAMA_QUEUE_CPP_H__

#include <mama/mamacpp.h>

namespace Wombat 
{
 
    class MamaQueueEnqueueCallback;
    class MamaQueueMonitorCallback;
    class MamaQueueEventCallback;

    /**
     * Queue allows applications to dispatch events in order with
     * multiple threads using a single MamaDispatcher for each queue.
     *
     */
    class MAMACPPExpDLL MamaQueue
    {   

    public:
        MamaQueue          (void);
        virtual ~MamaQueue (void);

        /**
         * Create a queue. Queues allow applications to dispatch events in
         * order with multiple threads using a single mamaDispatcher for
         * each queue.
         *
         * Callers should call delete queue when done.
         *
         * @return a pointer the queue.
         */
        virtual void create (
            mamaBridge  bridgeImpl);

        virtual void create (
            mamaBridge  bridgeImpl,
            void*       nativeQueue);

        /**
         * Dispatch message. Blocks and dispatches messages until unblock
         * is called.
         */
        virtual void dispatch ();

        /**
         * Dispatch messages until timeout (see release notes for details)
         */
        virtual void timedDispatch (
            uint64_t  timeout);

        /**
         * Dispatch a single event from the specified queue. If there is no event on
         * the queue simply return and do nothing
         */
        virtual void dispatchEvent ();


        /** Add a user event to a queue.
         *
         * @param callback Instance of the MamaQueueEventCallback interface.
         * MamaQueueEventCallback.onEvent() will be invoked when the event fires.
         * @param closure Optional user supplied arbitrary closure data which will be
         * passed back in the MamaQueueEventCallback.onEvent) callback
         * @throws MamaException Not currently implemented for pure Java API.
         */
        virtual void enqueueEvent (
            MamaQueueEventCallback*  callback, 
            void*                    closure);
        
        virtual void enqueueEvent (
            MamaQueueEventCallback&  callback,
            void*                    closure);
        
        /**
         * stopDispatch the queue.
         */
        virtual void stopDispatch ();

        /**
         * Returns the number of events currently on the queue.
         *
         * @return size_t The number of the events on the queue.
         */
        virtual size_t getEventCount ();

        /**
         * Set a callback which will be invoked as each event is added to the
         * underlying event queue.
         *
         * @param cb Pointer to an instance of MamaQueueEnqueueCallback
         * @param closure Arbitrary user supplied data. Passed back to
         * onEventEnqueue() for each event enqueued.
         */
        virtual void setEnqueueCallback (
            MamaQueueEnqueueCallback*  cb,
            void*                      closure);

        /**
         * Register an object to receive callbacks for monitoring the behaviour of
         * the MamaQueue.
         *
         * @param cb Reference to the object which will receive callbacks.
         * @param closure User supplied data which will be returned as the
         * callbacks are invoked.
         */
        virtual void setQueueMonitorCallback (
            MamaQueueMonitorCallback*  cb,
            void*                      closure);
        
        /**
         * Specify a high watermark for events on the queue.
         *
         * The behaviour for setting this value varies depending on the
         * underlying middleware.
         *
         * LBM: LBM uses an unbounded event queue. Setting this values allows users
         * of the API to receive a callback if the value is exceeded. (See
         * mamaQueue_setQueueMonitorCallback() for setting queue related callbacks)
         * the default behaviour is for the queue to grow unbounded without
         * notifications.
         * The high watermark for LBM can be set for all queues at once by setting the
         * mama.lbm.eventqueuemonitor.queue_size_warning property for the API. Calls
         * to this function will override the value specified in mama.properties.
         *
         * RV: This will set a queue limit policy of TIBRVQUEUE_DISCARD_FIRST whereby
         * the oldest events in the queue are discarded first. The discard amount will
         * be set with a value of 1. i.e. events will be dropped from the queue one at
         * a time. The default behaviour is an unlimited queue which does not discard 
         * events.
         */
        virtual void setHighWatermark (
            size_t  highWatermark);

        /**
         * Return the high water mark as set via <code>setHighWaterMark()</code>
         */
        virtual size_t getHighWatermark (void) const;
       
        /**
         * Set the low watermark.
         * Only supported for Wombat TCP middleware.
         * @param lowWatermark The low water mark.
         */
        virtual void setLowWatermark (
            size_t  lowWatermark);

        /**
         * Return the low water mark as set via <code>setLowWaterMark()</code>
         */
        virtual size_t getLowWatermark (void) const;
        
        /**
        * Associate a name identifier with the event queue. This will be used in
        * queue related logging statements.
        * The string is copied by the API.
        *
        * @param name The string name identifier for the queue. 
        */
        virtual void setQueueName (
            const char*  name);

        /**
         * Retrieve the string name identifier for the queue as specified from a
         * call to <code>setQueueName()</code>.
         * If a name has not been specified via a call to<code>setQueueName()</code>
         * the queue will assume a default name of "NO_NAME"
         *
         * @return The name identifier for the MamaQueue.
         */
        virtual const char* getQueueName () const;

        /**
         * Retrieve the string name identifier for the queue's bridge.
         *
         * @return The name identifier for the bridge: "wmw", "lbm", or "tibrv".
         */
        virtual const char* getQueueBridgeName () const;

        /**
         * Destroy a queue. Note that the queue can only be destroyed if all of the objects created
         * on it, (timers, subscriptions etc), have been destroyed.
         *
         * @param queue The queue.
         * @exception MamaStatus with a code of MAMA_STATUS_QUEUE_OPEN_OBJECTS if there are still 
         *            objects open against the queue.
         */
        virtual void destroy ();


        virtual void setClosure (
            void*  closure);

        virtual void* getClosure ();

        /**
         * Destroy a queue. Note that the queue can only be destroyed if all of the objects created
         * on it, (timers, subscriptions etc), have been destroyed. This function will block for the
         * specified time or until all of the objects have been destroyed and will then destroy the queue.
         *
         * @param timeout The time to block for in ms.
         * @exception MamaStatus with a code of MAMA_STATUS_TIMEOUT if the time elapsed.
         */
        virtual void destroyTimedWait (
            long  timeout);

        /**
         * Destroy a queue. Note that the queue can only be destroyed if all of the objects created
         * on it, (timers, subscriptions etc), have been destroyed. This function will block until
         * all of the objects have been destroyed and will then destroy the queue. 
         *
         */
        virtual void destroyWait ();


        /**
         * Access to C types for implementation of related classes.
         */
        mamaQueue        getCValue ();
        const mamaQueue  getCValue () const;

        /**
         * This can only be set once and only 
         * if the c value is not already set  - E.g. from calling create()
         */
        void setCValue (
            mamaQueue  cQueue);

        struct          MamaQueueImpl;
        MamaQueueImpl*  mPimpl;
    private:
        mamaQueue       mQueue;

        void destroyInternal (
            mama_status  status);
    };

} // namespace Wombat
#endif // MAMA_QUEUE_CPP_H__
