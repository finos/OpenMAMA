/* $Id: queue.h,v 1.22.4.2.8.4 2011/09/21 13:47:46 ianbell Exp $
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

#ifndef MamaQueueH__
#define MamaQueueH__

#include <mama/mama.h>
#include <mama/types.h>
#include <mama/status.h>

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * Callback invoked if an upper size limit has been specified for a queue and
 * that limit has been exceeded.
 *
 * @param queue The mamaQueue for which the size limit has been exceeded. NULL
 * if the queue is the default internal MAMA queue.
 * @param size  The current number of events outstanding on the queue (if
 * supported on the underlying middleware)
 * @param closure User supplied data set when the callback was registered.
 * NULL in the case of the default MAMA queue as no closure can be specified
 * when registering the data quality callbacks.
 */
typedef void (MAMACALLTYPE *mamaQueueHighWatermarkExceededCb) (mamaQueue queue,
                                                               size_t    size,
                                                               void*     closure);

/**
 * Callback invoked when the queue size returns to the lower limit specified.
 * Only Wombat TCP middleware supports low water mark callbacks.
 */
typedef void (MAMACALLTYPE *mamaQueueLowWatermarkCb) (mamaQueue queue,
                                                      size_t    size,
                                                      void*     closure);

/**
 * callbacks which may be invoked in response to certain conditions on
 * the specified queue being met.
 */
typedef struct mamaQueueMonitorCallbacks_
{
    mamaQueueHighWatermarkExceededCb onQueueHighWatermarkExceeded;
    mamaQueueLowWatermarkCb          onQueueLowWatermark;
} mamaQueueMonitorCallbacks;

/**
 * Function invoked when an event is enqueued on the queue for which
 * this function was registered.
 *
 * LBM Bridge: NB! Users may not dispatch events from this function when using
 * with mamaQueue_setEnqueueCallback()  The function is invoked from an LBM
 * internal thread. Attempts to dispatch from here will result in a deadlock.
 *
 * @param queue The mamaQueue on which the function was registered.
 * @param closure The user data supplied in the call to setEnqueueCallback.
 */
typedef void (MAMACALLTYPE *mamaQueueEnqueueCB)(mamaQueue queue, void* closure);

/** Function invoked when a user added event fires. Events are added
 * to a queue using the <code>mamaQuque_enqueueEvent()</code>.
 * @param queue The MamaQueue on which the event was enqueued.
 * @param closure  The user specified data associated with this event.
 */
typedef void (MAMACALLTYPE *mamaQueueEventCB)(mamaQueue queue, void* closure);

/**
 * Create a queue. Queues allow applications to dispatch events in order with
 * multiple threads using a single mamaDispatcher for each queue. A queue must
 * be associated with a particular middleware.
 *
 * @param queue A pointer to the resulting queue.
 * @param bridgeImpl A valid bridge implementation for which this queue is
 * being created.
 * @return MAMA_STATUS_OK if the call succeeds.
 * @return MAMA_STATUS_NO_BRIDGE_IMPL if the bridgeImpl parameter is not
 * valid.
 */
MAMAExpDLL
extern mama_status
mamaQueue_create (mamaQueue* queue, mamaBridge bridgeImpl);


MAMAExpDLL
extern mama_status
mamaQueue_create_usingNative (mamaQueue* queue, mamaBridge bridgeImpl, void* nativeQueue);

/**
 * Check to see if a queue can be destroyed. The queue cannot be destroyed if there are
 * currently open event objects on it.
 *
 * @param queue The queue.
 * @return MAMA_STATUS_OK if the queue can be destroyed.
 *         MAMA_STATUS_QUEUE_OPEN_OBJECTS if there are still objects open against the queue.
 *         MAMA_STATUS_NULL_ARG
 */
MAMAExpDLL
extern mama_status
mamaQueue_canDestroy(mamaQueue queue);

/**
 * Destroy a queue. Note that the queue can only be destroyed if all of the objects created
 * on it, (timers, subscriptions etc), have been destroyed.
 *
 * @param queue The queue.
 * @return MAMA_STATUS_OK if the call is successful.
 *         MAMA_STATUS_QUEUE_OPEN_OBJECTS if there are still objects open against the queue.
 */
MAMAExpDLL
extern mama_status
mamaQueue_destroy (mamaQueue queue);

/**
 * Destroy a queue. Note that the queue can only be destroyed if all of the objects created
 * on it, (timers, subscriptions etc), have been destroyed. This function will block until
 * all of the objects have been destroyed and will then destroy the queue.
 *
 * @param queue The queue.
 * @return MAMA_STATUS_OK if the call is successful.
 */
MAMAExpDLL
extern mama_status
mamaQueue_destroyWait(mamaQueue queue);

/**
 * Destroy a queue. Note that the queue can only be destroyed if all of the objects created
 * on it, (timers, subscriptions etc), have been destroyed. This function will block for the
 * specified time or until all of the objects have been destroyed and will then destroy the queue.
 *
 * @param queue The queue.
 * @param timeout The time to block for in ms.
 * @return MAMA_STATUS_OK if the call is successful.
 *         MAMA_STATUS_TIMEOUT if the time elapsed.
 */
MAMAExpDLL
extern mama_status
mamaQueue_destroyTimedWait(mamaQueue queue, long timeout);

/**
 * Specify a high watermark for events on the queue.
 *
 * The behaviour for setting this value varies depending on the
 * underlying middleware.
 *
 * LBM: LBM uses an unbounded event queue. Setting this values allows users
 * of the API to receive a callback if the value is exceeded. (See
 * mamaQueue_setQueueMonitorCallback() for setting queue related callbacks)
 * The default behaviour is for the queue to grow unbounded without
 * notifications.
 * The high watermark for LBM can be set for all queues at once by setting the
 * mama.lbm.eventqueuemonitor.queue_size_warning property for the API. Calls
 * to this function will override the value specified in mama.properties at
 * runtime. Callbacks can be disabled by setting this value to 0,
 * effectively disabling high watermark checking.
 *
 * RV: This will set a queue limit policy of TIBRVQUEUE_DISCARD_FIRST whereby
 * the oldest events in the queue are discarded first. The discard amount will
 * be set with a value of 1, i.e. events will be dropped from the queue one at
 * a time. The default behaviour is an unlimited queue which does not discard
 * events.
 *
 * @param queue The mamaQueue for which the high watermark is being set.
 * @param highWatermark The size of the queue, beyond which, results in
 * notification of activity.
 * @return MAMA_STATUS_OK if the function returns successfully.
 */
MAMAExpDLL
extern mama_status
mamaQueue_setHighWatermark (mamaQueue queue,
                            size_t highWatermark);

/**
* Get the value of the high water mark for the specified queue. A value of 0
* will be returned if no high water mark was previously specified.
*
* @param queue The mamaQueue for which the high water mark is being retrieved
* @param highWatermark Address to which the high water mark will be written.
*/
MAMAExpDLL
extern mama_status
mamaQueue_getHighWatermark (mamaQueue queue,
                            size_t*   highWatermark);

/**
 * Set the low water mark for the queue. Only supported by Wombat TCP
 * middleware.
 *
 * The low watermark must be >1 and < highWaterMark otherwise this method
 * returns MAMA_STATUS_INVALID_ARG. For this reason the high water mark must
 * be set before invoking this method.
 *
 * @param queue The queue.
 * @param lowWatermark the low watermark.
 */
MAMAExpDLL
extern mama_status
mamaQueue_setLowWatermark (mamaQueue queue,
                           size_t lowWatermark);

/**
* Get the value of the low water mark for the specified queue. A value of 1
* will be returned if no low water mark was previously specified.
*
* @param queue The mamaQueue for which the low water mark is being retrieved.
* @param lowWatermark Address to which the low water mark will be written.
*/
MAMAExpDLL
extern mama_status
mamaQueue_getLowWatermark (mamaQueue queue,
                           size_t*   lowWatermark);

/**
 * Specify a set of callbacks which may be invoked in response to certain
 * conditions arising on the queue.
 *
 * The behaviour here is middleware specific as not all will support all
 * callbacks.
 *
 * LBM: When the high watermark is exceeded the
 * mamaQueueHighWatermarkExceededCb callback will invoked each time an event
 * on the queue is dispatched until such time as the number of events on the
 * queue falls below the high watermark.
 *
 */
MAMAExpDLL
extern mama_status
mamaQueue_setQueueMonitorCallbacks (
                        mamaQueue                   queue,
                        mamaQueueMonitorCallbacks*  queueMonitorCallbacks,
                        void*                       closure);

/**
 * Writes the number of events currently on the specified queue to
 * the address specified by count.
 *
 * @param queue The queue.
 * @param count Address to where the number of events on the queue will be
 *              written
 * @return MAMA_STATUS_OK if the call is successful.
 */
MAMAExpDLL
extern mama_status
mamaQueue_getEventCount (mamaQueue queue, size_t* count);

/**
* Associate a name identifier with the event queue. This will be used in
* queue related logging statements.
* The string is copied by the API.
*
* @param queue The event queue for which the name is being specified.
* @param name The string identifier for the queue.
* @return MAMA_STATUS_OK The function call succeeded.
* @return MAMA_STATUS_NULL_ARG The queue parameter is NULL
* @return MAMA_STATUS_INVALID_ARG The name parameter is NULL
* @return MAMA_STATUS_NO_MEM  The name could not be copied.
*/
MAMAExpDLL
extern mama_status
mamaQueue_setQueueName (mamaQueue queue, const char* name);

/**
* Get the string name identifier for the specified event queue.
*
* @param queue The event queue for which the name is being sought.
* @param name Address to which the name will be written.
* @return MAMA_STATUS_OK The function call succeeded.
* @return MAMA_STATUS_NULL_ARG The queue parameter was NULL
* @return MAMA_STATUS_INVALID_ARG The name parameter was NULL
*/
MAMAExpDLL
extern mama_status
mamaQueue_getQueueName (mamaQueue queue, const char** name);

/**
* Get the string name identifier of the bridge for the specified
* event queue.  Name will be either "wmw", "tibrv", or "lbm".
*
* @param queue The event queue for which the bridge name is
*              being sought.
* @param name  Address to which the name will be written.
* @return MAMA_STATUS_OK The function call succeeded.
* @return MAMA_STATUS_NULL_ARG The queue parameter was NULL
*/
MAMAExpDLL
extern mama_status
mamaQueue_getQueueBridgeName (mamaQueue queue, const char** name);


/**
 * Dispatch messages from the queue. This call blocks and dispatches
 * until mamaQueue_stopDispatch() is called.
 *
 * @param queue The queue.
 * @return MAMA_STATUS_OK if the call is successful.
 */
MAMAExpDLL
extern mama_status
mamaQueue_dispatch (mamaQueue queue);

/**
 * Dispatch messages from the queue. This call blocks and dispatches
 * until timeout has elapsed.
 *
 * @param queue     The queue.
 * @param timeout   The number of milliseconds to block for before the function returns.
 * @return MAMA_STATUS_OK if the call is successful.
 */
MAMAExpDLL
extern mama_status
mamaQueue_timedDispatch (mamaQueue queue, uint64_t timeout);

/**
 * Dispatch a single event from the specified queue. If there is no event on
 * the queue simply return and do nothing.
 *
 * @param queue The queue from which to dispatch the event.
 * @return MAMA_STATUS_OK if the function succeeds.
 */
MAMAExpDLL
extern mama_status
mamaQueue_dispatchEvent (mamaQueue queue);


/** Add an user event to a queue. Currently only supported using Wombat
 * Middleware.
 *
 * @param queue The queue to which the event is to be added
 * @param callback The function to be invoked when the event fires.
 * @param closure Optional arbitrary user supplied data. Passed back to
 * callback function.
 * @return MAMA_STATUS_OK if the function succeeds.
 */
MAMAExpDLL
extern mama_status
mamaQueue_enqueueEvent  (mamaQueue          queue,
                         mamaQueueEventCB   callback,
                         void*              closure);

/**
 * Unblock the queue as soon as possible. This will cause mamaDispatchers to
 * exit. Creating a new dispatcher will resume dispatching events.
 *
 * @param queue The queue.
 * @return MAMA_STATUS_OK if the call is successful.
 */
MAMAExpDLL
extern mama_status
mamaQueue_stopDispatch (mamaQueue queue);

/**
 * Register the specified callback function to receive a callback
 * each time an event is enqueued on the specified mamaQueue
 *
 * @param queue The mamaQueue on which the callback should be registered.
 * @param callback The function which should be invoked for each enqueue
 * operation
 * @param closure Optional arbitrary user supplied data. Passed back to
 * callback function.
 *
 * @return MAMA_STATUS_OK if the call is successful.
 */
MAMAExpDLL
extern mama_status
mamaQueue_setEnqueueCallback (mamaQueue          queue,
                              mamaQueueEnqueueCB callback,
                              void*              closure);

/**
 * If the specified queue has a registered enqueue callback it is unregistered
 * and the previously supplied callback function will no longer receive
 * callbacks for enqueue events.
 *
 * @param queue The mamaQueue for which the callback function should be
 * removed.
 * @return MAMA_STATUS_OK if the call is successful.
 */
MAMAExpDLL
extern mama_status
mamaQueue_removeEnqueueCallback (mamaQueue queue);

/**
 * Get the native middleware implementation queue handle (if
 * applicable for the implementation).  This function is for internal
 * use only.
 *
 * @param queue The mamaQueue for which the native handle is requested.
 * @param nativeHandle The resulting native handle.
 * @return MAMA_STATUS_OK if the call is successful.
 */
MAMAExpDLL
extern mama_status
mamaQueue_getNativeHandle (mamaQueue queue,
                           void**    nativeHandle);

/**
 * Create a mamaDispatcher. The dispatcher spawns a thread to dispatch events
 * from a queue. It will continue to dispatch events until it is destroyed or
 * mamaQueue_stopDispatch is called.
 *
 * Only a single dispatcher can be created for a given queue. Attempting to
 * create multiple dispatchers for a queue will result in and error. Dispatching
 * message from a single queue with multiple threads results in messages
 * arriving out of order and sequence number gaps for market data subscriptions.
 *
 * @param result A pointer to the resulting mamaDispatcher.
 * @param queue The queue.
 * @return MAMA_STATUS_OK if the call is successful.
 */
MAMAExpDLL
extern mama_status
mamaDispatcher_create (mamaDispatcher* result, mamaQueue queue);

/**
 * Return the queue associated with the dispatcher.
 *
 * @param dispatcher The dispatcher.
 * @param result The queue.
 * @return MAMA_STATUS_OK if the call is successful.
 */
MAMAExpDLL
extern mama_status
mamaDispatcher_getQueue (mamaDispatcher dispatcher, mamaQueue* result);

/**
 * Enable stats logging on queue
 *
 * @param queue The queue.
 * @return MAMA_STATUS_OK if the call is successful.
 */
MAMAExpDLL
extern mama_status
mamaQueue_enableStats(mamaQueue queue);


/**
 * Destroy the dispatcher and stop dispatching events. If
 * mamaDispatcher_createQueue() was used then the underlying queue will be
 * destroyed as well.
 *
 * @param dispatcher The dispatcher.
 * @return MAMA_STATUS_OK if the call is successful.
 */
MAMAExpDLL
extern mama_status
mamaDispatcher_destroy (mamaDispatcher dispatcher);

MAMAExpDLL
extern mama_status
mamaQueue_getClosure (mamaQueue queue, void** closure);

MAMAExpDLL
extern mama_status
mamaQueue_setClosure (mamaQueue queue, void* closure);
#if defined(__cplusplus)
}
#endif

#endif /*MamaQueueH__*/
