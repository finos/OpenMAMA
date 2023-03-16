#ifndef MamaQueueGroupH__
#define MamaQueueGroupH__

#if defined(__cplusplus)
extern "C"
{
#endif

#include <mama/config.h>
#include <mama/status.h>
#include <mama/types.h>
#include <mama/queue.h>


/**
 * Creates a MAMA Queue Group. A MAMA Queue Group provides a convenient way
 * to define a series of queues which can then be used for strategic load
 * balancing and locality of context.
 * @param queueGroup        A pointer which will be populated with a handle
 *                          to this queue group on successful invocation.
 * @param numberOfQueues    The number of queues which this queue group will
 *                          encapsulate.
 * @param bridgeImpl        Handle to the underlying MAMA bridge whose queue
 *                          implementation this queue group will be built using.
 * @result A mama_status value to reflect if this operation was successful.
 */
MAMAExpDLL
extern mama_status
mamaQueueGroup_create (
        mamaQueueGroup* queueGroup,
        int numberOfQueues,
        mamaBridge bridgeImpl);

/**
 * Destroys a given queue group along with underlying queues and dispatchers.
 * @param queueGroup A handle to the queue group to be destroyed
 * @result A mama_status value to reflect if this operation was successful.
 */
MAMAExpDLL
extern mama_status
mamaQueueGroup_destroy (
        mamaQueueGroup queueGroup);

/**
 * Returns the next queue to be selected when doing round robin dispatching.
 * @param queueGroup A handle to the queue group to be interrogated
 * @param queue      A queue pointer to populate with the result on successful
 *                   execution
 * @result A mama_status value to reflect if this operation was successful.
 */
MAMAExpDLL
extern mama_status
mamaQueueGroup_getNextQueue (
        mamaQueueGroup queueGroup,
        mamaQueue* queue);

/**
 * Returns the specifically requested queue based on its index.
 * @param queueGroup A handle to the queue group to be interrogated
 * @param queue      A queue pointer to populate with the result on successful
 *                   execution
 * @param index      The queue index to request
 * @result A mama_status value to reflect if this operation was successful.
 */
MAMAExpDLL
extern mama_status
mamaQueueGroup_getQueueByIndex (
        mamaQueueGroup queueGroup,
        mamaQueue* queue,
        int index);

/**
 * Gather up the number of queues contained within this queue group.
 * @param queueGroup A handle to the queue group to be interrogated
 * @param count      A pointer to the result to populate with the number of
 *                   queues on successful execution
 * @result A mama_status value to reflect if this operation was successful.
 */
MAMAExpDLL
extern mama_status
mamaQueueGroup_getNumberOfQueues (
        mamaQueueGroup queueGroup,
        int* count);

/**
 * This will stop dispatching for all queues in the given queue group
 * @param queueGroup The queue group to stop dispatching on
 * @result A mama_status value to reflect if this operation was successful.
 */
MAMAExpDLL
extern mama_status
mamaQueueGroup_stopDispatch (
        mamaQueueGroup queueGroup);

/**
 * This will start dispatching for all queues in the given queue group
 * @param queueGroup The queue group to start dispatching on
 * @result A mama_status value to reflect if this operation was successful.
 */
MAMAExpDLL
extern mama_status
mamaQueueGroup_startDispatch (
        mamaQueueGroup queueGroup);

/**
 * This will destroy the given queue group by stopping dispatch, and waiting
 * until queue is empty before returning. Note that mamaQueueGroup_destroy will
 * still need to be called after this to destroy the underlying mamaQueueGroup
 * handle.
 * @param queueGroup The queue group to destroy.
 * @result A mama_status value to reflect if this operation was successful.
 */
MAMAExpDLL
extern mama_status
mamaQueueGroup_destroyWait(
        mamaQueueGroup queueGroup);


#if defined(__cplusplus)
}
#endif

#endif /* MamaQueueGroupH__ */