/**
* This file contains methods which are intended for use only by OpenMAMA core,
* bridges and plugins. These methods typically only make sense to developers
* who have intimate knowledge of the inner workings of OpenMAMA. Its use in
* applications is heavily discouraged and entirely unsupported.
*
* Note that you can only use these methods if you have defined the
* OPENMAMA_INTEGRATION macro. If you think a case is to be made for accessing
* one of these methods, please raise to the mailing list and we'll investigate
* opening up a more stable formal interface for the standard public API.
*/

#if defined (OPENMAMA_INTEGRATION) && ! defined (OPENMAMA_INTEGRATION_QUEUE_H__)
#define OPENMAMA_INTEGRATION_QUEUE_H__

#include <mama/mama.h>

#if defined (__cplusplus)
extern "C"
{
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
extern mamaStatsCollector
mamaQueueImpl_getStatsCollector (mamaQueue queue);

#if defined (__cplusplus)
}
#endif

#endif /* OPENMAMA_INTEGRATION_QUEUE_H__ */
