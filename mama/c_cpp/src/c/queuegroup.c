#include <mama/config.h>
#include <mama/queuegroup.h>

typedef struct mamaQueueGroupImpl_
{
    int                  mQueueCount;
    unsigned int         mCallCount;
    mamaQueue*           mQueues;
    mamaDispatcher*      mDispatchers;
    mamaQueue            mDefaultQueue;
} mamaQueueGroupImpl;


MAMAExpDLL
extern mama_status
mamaQueueGroup_create (
        mamaQueueGroup* queueGroup,
        int numberOfQueues,
        mamaBridge bridgeImpl)
{
    mamaQueueGroupImpl* impl = NULL;
    int i = 0;
    if (queueGroup == NULL) return MAMA_STATUS_NULL_ARG;
    if (bridgeImpl == NULL) return MAMA_STATUS_NO_BRIDGE_IMPL;
    impl = (mamaQueueGroupImpl*) calloc (1, sizeof(mamaQueueGroupImpl));
    if (NULL == impl)
    {
        return MAMA_STATUS_NOMEM;
    }
    impl->mQueueCount = numberOfQueues;

    if (impl->mQueueCount > 0)
    {
        impl->mQueues = (mamaQueue*) calloc(impl->mQueueCount,
                                            sizeof(mamaQueue));
        impl->mDispatchers = (mamaDispatcher*) calloc(impl->mQueueCount,
                                                      sizeof(mamaDispatcher));

        for (i = 0; i < impl->mQueueCount; i++)
        {
            mamaQueue_create (&impl->mQueues[i], bridgeImpl);
            mamaDispatcher_create (&impl->mDispatchers[i], impl->mQueues[i]);
        }
    }

    mama_getDefaultEventQueue (bridgeImpl, &impl->mDefaultQueue);

    *queueGroup = impl;
    return MAMA_STATUS_OK;
}

MAMAExpDLL
extern mama_status
mamaQueueGroup_destroy (mamaQueueGroup queueGroup)
{
    mamaQueueGroupImpl* impl = (mamaQueueGroupImpl*)queueGroup;
    int i;
    if (impl->mQueues != NULL)
    {
        for (i = 0; i < impl->mQueueCount; i++)
        {
            if (impl->mDispatchers[i])
            {
                mamaDispatcher_destroy (impl->mDispatchers[i]);
                impl->mDispatchers[i] = NULL;
            }
            mamaQueue_destroy (impl->mQueues[i]);
            impl->mQueues[i] = NULL;
       }
   }

   free (impl->mQueues);
   free (impl->mDispatchers);
   free ((void*)impl);
   return MAMA_STATUS_OK;
}

MAMAExpDLL
extern mama_status
mamaQueueGroup_getNextQueue (mamaQueueGroup queueGroup, mamaQueue* queue) {
    mamaQueueGroupImpl* impl = (mamaQueueGroupImpl*)queueGroup;
    if (impl == NULL || queue == NULL) return MAMA_STATUS_NULL_ARG;
    if (impl->mQueues == NULL)
    {
        *queue = impl->mDefaultQueue;
    }
    else
    {
        *queue = impl->mQueues[impl->mCallCount++ % impl->mQueueCount];
    }
    return MAMA_STATUS_OK;
}

MAMAExpDLL
extern mama_status
mamaQueueGroup_getQueueByIndex (mamaQueueGroup queueGroup, mamaQueue* queue, int index) {
    mamaQueueGroupImpl* impl = (mamaQueueGroupImpl*)queueGroup;
    if (impl == NULL || queue == NULL) return MAMA_STATUS_NULL_ARG;
    if (impl->mQueues == NULL || index >= impl->mQueueCount)
    {
        return MAMA_STATUS_NOT_FOUND;
    }
    else
    {
        *queue = impl->mQueues[index];
        return MAMA_STATUS_OK;
    }
}

MAMAExpDLL
extern mama_status
mamaQueueGroup_getNumberOfQueues (mamaQueueGroup queueGroup, int* count)
{
    mamaQueueGroupImpl* impl = (mamaQueueGroupImpl*)queueGroup;
    if (impl == NULL || count == NULL) return MAMA_STATUS_NULL_ARG;
    *count = impl->mQueueCount;
    return MAMA_STATUS_OK;
}

MAMAExpDLL
extern mama_status
mamaQueueGroup_stopDispatch (mamaQueueGroup queueGroup)
{
    mamaQueueGroupImpl* impl = (mamaQueueGroupImpl*)queueGroup;
    mama_status status = MAMA_STATUS_OK;
    int i = 0;
    if (impl == NULL) return MAMA_STATUS_NULL_ARG;
    for (i = 0; i < impl->mQueueCount; i++)
    {
        if (impl->mDispatchers[i])
        {
            mama_status iterStatus =
                mamaDispatcher_destroy (impl->mDispatchers[i]);
            if (MAMA_STATUS_OK != iterStatus) {
                // Take latest failure as the return code
                status = iterStatus;
            }
            impl->mDispatchers[i] = NULL;
        }
    }
    return status;
}

MAMAExpDLL
extern mama_status
mamaQueueGroup_startDispatch (mamaQueueGroup queueGroup)
{
    mamaQueueGroupImpl* impl = (mamaQueueGroupImpl*)queueGroup;
    int i = 0;
    if (impl == NULL) return MAMA_STATUS_NULL_ARG;
    for (i = 0; i < impl->mQueueCount; i++)
    {
        if (!impl->mDispatchers[i])
        {
            mamaDispatcher_create(&impl->mDispatchers[i], impl->mQueues[i]);
        }
   }
    return MAMA_STATUS_OK;
}

MAMAExpDLL
extern mama_status
mamaQueueGroup_destroyWait(mamaQueueGroup queueGroup)
{
    mamaQueueGroupImpl* impl = (mamaQueueGroupImpl*)queueGroup;
    int index = 0;
    if (impl == NULL) return MAMA_STATUS_NULL_ARG;
    // First of all stop dispatching to ensure that no more messages are being processed.
    mamaQueueGroup_stopDispatch(queueGroup);

    // The queues may already have been destroyed
    if (impl->mQueues != NULL)
    {
        // Destroy all the queues in turn
        for (index = 0; index < impl->mQueueCount; index++)
        {
            if (NULL != impl->mQueues[index])
            {
                // This will block until the queue destroy has completed
                mamaQueue_destroyWait (impl->mQueues[index]);
                impl->mQueues[index] = NULL;
            }
        }

        // Delete the array of queues
        free (impl->mQueues);
        impl->mQueues = NULL;
    }
    return MAMA_STATUS_OK;
}
