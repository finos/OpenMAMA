/* $Id: queueimpl.h,v 1.7.6.2.2.1.4.3 2011/08/10 14:53:25 nicholasmarriott Exp $
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

#ifndef MamaQueueImplH__
#define MamaQueueImplH__

#include "bridge.h"

#if defined(__cplusplus)
extern "C" {
#endif

/* This typedef is used when locking and unlocking the queue. */
typedef void * mamaQueueLockHandle;

/*
   Decrements the count of open objects against the queue. Note that the
   queue cannot be destroyed until this count is 0.
   
   @param handle The lock handle returned by mamaQueue_incrementObjectCount.
   @param queue The mamaQueue.   
*/
MAMAExpDLL
extern void 
mamaQueue_decrementObjectCount(mamaQueueLockHandle *handle, mamaQueue queue);

/**
 * Increments the count of open objects against the queue. Note that the
 * queue cannot be destroyed until this count is 0.
 *  
 * @param queue The mamaQueue.   
 * @param owner The owner object, this is used for tracking and its value will be written out whenever
 *              the count is incremented and decremented.
 * @return The lock handle, this must be passed to the mamaQueue_decrementObjectCount function and will
 *         be used to track locks whenever the appropriate flag is enabled.
*/
MAMAExpDLL
extern mamaQueueLockHandle
mamaQueue_incrementObjectCount(mamaQueue queue, void *owner);

/*
   Get the bridge impl associated with the specified queue.
   This will be how other objects gain access to the bridge.

   @param queue A valid mamaQueue object.
   @return A pointer to a valid mamaBridgeImpl object.
*/
MAMAExpDLL
extern mamaBridgeImpl*
mamaQueueImpl_getBridgeImpl (mamaQueue queue);

/*
   Get the cached mamaMsg which is reused for all incoming messages.
   
   @param queue The mamaQueue which has cached the message.

   @return The cached mamaMsg.
*/
MAMAExpDLL
extern mamaMsg
mamaQueueImpl_getMsg (mamaQueue queue);

/*
    When detaching a message it must be disassociated from the queue onto
    which is it attached.
*/
MAMAExpDLL
extern mama_status
mamaQueueImpl_detachMsg (mamaQueue queue, mamaMsg msg);

/*
    Will return true if the queue is currently dispatching
*/
MAMAExpDLL
extern int
mamaQueueImpl_isDispatching (mamaQueue queue);

/* Called by the middleware bridge layer when high watermark is exceeded */
MAMAExpDLL
extern mama_status
mamaQueueImpl_highWatermarkExceeded (mamaQueue queue, size_t size);

/* Called by the middleware bridge layer when low watermark is exceeded */
MAMAExpDLL
extern mama_status
mamaQueueImpl_lowWatermarkExceeded (mamaQueue queue, size_t size);

MAMAExpDLL
extern mamaStatsCollector* 
mamaQueueImpl_getStatsCollector (mamaQueue queue);

#if defined(__cplusplus)
}
#endif

#endif /* MamaQueueImplH__ */
