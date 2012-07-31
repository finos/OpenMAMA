/* $Id: queue.c,v 1.1.28.4 2011/08/31 16:33:53 mikeschonberg Exp $
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

#include "wombat/port.h"
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include "wombat/queue.h"
#include "wombat/wSemaphore.h"

#define WQ_REMOVE(impl, ele)                  \
    (ele)->mPrev->mNext = (ele)->mNext;       \
    (ele)->mNext->mPrev = (ele)->mPrev;       \
    (ele)->mNext = impl->mFirstFree.mNext;    \
    (impl)->mFirstFree.mNext = (ele);

/*
 * Items that get queued 
 */
typedef struct wombatQueueItem_
{
    wombatQueueCb              mCb;
    void*                      mData;
    void*                      mClosure;
    struct wombatQueueItem_*   mNext;
    struct wombatQueueItem_*   mPrev;
    struct wombatQueueItem_*   mChunkNext;
} wombatQueueItem;


typedef struct 
{
    wsem_t                mSem;
    wthread_mutex_t      mLock; /* for multiple readers */
    
    uint8_t              mUnblocking;

    uint32_t             mMaxSize;
    uint32_t             mChunkSize;

    /* Dummy nodes for free, head and tail */
    wombatQueueItem*  mIterator;
    wombatQueueItem   mHead;
    wombatQueueItem   mTail;
    wombatQueueItem   mFirstFree;
    wombatQueueItem*  mChunks;
} wombatQueueImpl;

static void
wombatQueueImpl_allocChunk ( wombatQueueImpl* impl, unsigned int items);

wombatQueueStatus
wombatQueue_allocate (wombatQueue *result)
{
    wombatQueueImpl *impl = NULL;
    *result = NULL;

    impl = (wombatQueueImpl*)calloc (1, sizeof(wombatQueueImpl));
    if (impl == NULL)
    {
        return WOMBAT_QUEUE_NOMEM;
    }
    *result = impl;

    /* Set defaults */
    impl->mMaxSize      = WOMBAT_QUEUE_MAX_SIZE;
    impl->mChunkSize    = WOMBAT_QUEUE_CHUNK_SIZE;
    impl->mIterator     = &impl->mHead;

    impl->mHead.mNext = &impl->mTail;
    impl->mHead.mPrev = &impl->mHead; /* for iteration */
    impl->mTail.mPrev = &impl->mHead;
    impl->mTail.mNext = &impl->mTail; /* for iteration */

    return WOMBAT_QUEUE_OK;
}

wombatQueueStatus
wombatQueue_create (wombatQueue queue, uint32_t maxSize, uint32_t initialSize,
                    uint32_t growBySize)
{
    wombatQueueImpl *impl = (wombatQueueImpl*)queue;
   
    if (maxSize)
        impl->mMaxSize      = maxSize;

    if (growBySize)
        impl->mChunkSize    = growBySize;
    
    if (wsem_init (&impl->mSem, 0, 0) != 0)
    {
        return WOMBAT_QUEUE_SEM_ERR;
    }

    wthread_mutex_init( &impl->mLock, NULL); 

    initialSize = initialSize == 0 ? WOMBAT_QUEUE_CHUNK_SIZE : initialSize;
    wombatQueueImpl_allocChunk (impl, initialSize);
    if (impl->mFirstFree.mNext == NULL) /* alloc failed */
    {
        return WOMBAT_QUEUE_NOMEM;
    }
    return WOMBAT_QUEUE_OK;
}

wombatQueueStatus
wombatQueue_destroy (wombatQueue queue)
{
    wombatQueueImpl *impl      = (wombatQueueImpl*)queue;
    wombatQueueStatus result   = WOMBAT_QUEUE_OK;
    wombatQueueItem *curItem; 

    wthread_mutex_lock (&impl->mLock);
    /* Free the datas */
    curItem = impl->mChunks;
    while (curItem)
    {
        wombatQueueItem* tmp = curItem->mChunkNext;
        free (curItem);
        curItem = tmp;
    }

    wthread_mutex_unlock (&impl->mLock);
    
    /* Thee wsem_destroy and wthread_mutex_destroy methods simply makes
     * sure that no threads are waiting since the synchronization objects can
     * not be destroyed if threads are waiting. We could devise a mechanism
     * to ensure that dispatchers are not waiting or dispatching. We can
     * certainly stop enqueueing events. Dequing we can only control if we
     * create our own dispatchers.
     *
     * Clients should make sure that all dispatchers are stopped before the
     * destroying a queue. 
     *
     */
    if (wsem_destroy (&impl->mSem) != 0)
    {
        result = WOMBAT_QUEUE_SEM_ERR;
    }
    
    wthread_mutex_destroy( &impl->mLock);
    free (impl);
    return WOMBAT_QUEUE_OK;
}


wombatQueueStatus
wombatQueue_setMaxSize (wombatQueue queue, unsigned int value)
{
    wombatQueueImpl *impl = (wombatQueueImpl*)queue;
    if (value < 0 || value > WOMBAT_QUEUE_MAX_SIZE)
    {
        return WOMBAT_QUEUE_INVALID_ARG;
    }
    impl->mMaxSize = value;
    return WOMBAT_QUEUE_OK;
}

wombatQueueStatus
wombatQueue_getMaxSize (wombatQueue queue, unsigned int *value)
{
    wombatQueueImpl *impl = (wombatQueueImpl*)queue;
    *value = impl->mMaxSize;
    return WOMBAT_QUEUE_OK;
}

wombatQueueStatus
wombatQueue_enqueue (wombatQueue queue, 
                     wombatQueueCb cb, 
                     void* data,
                     void* closure)
{
    wombatQueueImpl* impl = (wombatQueueImpl*)queue;
    wombatQueueItem* item = NULL;

    wthread_mutex_lock (&impl->mLock);

    /* If there are no items in the free list, allocate some. It will set the
     * next free node to NULL if the queue is too big or there is no memory.
     */
    if (impl->mFirstFree.mNext == NULL)
        wombatQueueImpl_allocChunk (impl, impl->mChunkSize);
    item = impl->mFirstFree.mNext;

    if (item == NULL)
        return WOMBAT_QUEUE_FULL;

    impl->mFirstFree.mNext = item->mNext;
    /* Initialize the item. */
    item->mCb      = cb;
    item->mData    = data;
    item->mClosure = closure;

    /* Put on queue (insert before dummy tail node */
    item->mNext              = &impl->mTail;
    item->mPrev              = impl->mTail.mPrev;
    item->mPrev->mNext       = item;
    impl->mTail.mPrev        = item;

    /* Notify next available thread that an item is ready */
    wsem_post (&impl->mSem);
    wthread_mutex_unlock (&impl->mLock);

    return WOMBAT_QUEUE_OK;
}

wombatQueueStatus
wombatQueue_getSize (wombatQueue queue, int* size)
{
    wombatQueueImpl* impl    = (wombatQueueImpl*)queue;
    wsem_getvalue (&impl->mSem, size);
    
    return WOMBAT_QUEUE_OK;

}

static wombatQueueStatus
wombatQueue_dispatchInt (wombatQueue queue, void** data, void** closure, 
        uint8_t isTimed, uint64_t timout)
{
    wombatQueueImpl* impl     = (wombatQueueImpl*)queue;
    wombatQueueItem* head     = NULL;
    wombatQueueCb    cb       = NULL;
    void*            closure_ = NULL;
    void*            data_    = NULL;

    if (isTimed)
    {
        if (wsem_timedwait (&impl->mSem, (unsigned int)timout) !=0)
            return WOMBAT_QUEUE_TIMEOUT;
    }
    else
    {
        while (-1 == wsem_wait (&impl->mSem))
        {
            if (errno != EINTR)
                return WOMBAT_QUEUE_SEM_ERR;
        }
        if (impl->mUnblocking)
        {
            impl->mUnblocking = 0;
            return WOMBAT_QUEUE_OK;
        }
    }

    wthread_mutex_lock (&impl->mLock); /* May be multiple readers */
 
    /* remove the item */
    head                   = impl->mHead.mNext;
    WQ_REMOVE (impl, head);

    if (data)
        *data = head->mData;
    
    if (closure)
        *closure = head->mClosure;

    /* so we can unlock (allows cb to dequeue) */
    cb = head->mCb;
    closure_ = head->mClosure;
    data_    = head->mData;
    
    wthread_mutex_unlock (&impl->mLock);

    if (cb)
    {
        cb (data_, closure_);
    }

    return WOMBAT_QUEUE_OK;
}

wombatQueueStatus
wombatQueue_dispatch (wombatQueue queue, void** data, void** closure)
{
    return wombatQueue_dispatchInt (queue, data, closure, 0, 0);
}

wombatQueueStatus
wombatQueue_timedDispatch (wombatQueue queue, void** data, void** closure,
        uint64_t timeout)
{
    return wombatQueue_dispatchInt (queue, data, closure, 1, timeout);
}


wombatQueueStatus
wombatQueue_poll (wombatQueue queue, void** data, void** closure)
{
    wombatQueueImpl* impl     = (wombatQueueImpl*)queue;
    wombatQueueItem* head     = NULL;
    wombatQueueCb    cb       = NULL;
    void*            closure_ = NULL;
    void*            data_    = NULL;

    if (wsem_trywait (&impl->mSem) != 0)
    {
        return WOMBAT_QUEUE_WOULD_BLOCK;
    }

    wthread_mutex_lock (&impl->mLock); 
   
    head             = impl->mHead.mNext;
    WQ_REMOVE (impl, head);

    /* so we can unlock (allows cb to dequeue) */
    if (data)
        *data = head->mData;

    if (closure)
        *closure = head->mClosure;

    cb = head->mCb;
    closure_ = head->mClosure;
    data_    = head->mData;

    wthread_mutex_unlock (&impl->mLock);

    if (cb)
    {
        cb (data_, closure_);
    }

    return WOMBAT_QUEUE_OK;
}


/* Static/Private functions */
static void
wombatQueueImpl_allocChunk ( wombatQueueImpl* impl, unsigned int items)
{
    size_t sizeToAlloc =  items * sizeof(wombatQueueItem);
    wombatQueueItem* result;
    int size;

    wsem_getvalue (&impl->mSem, &size);
    if (size + items > impl->mMaxSize)
    { 
        /* impl->mFirstFree.mNext is already NULL */
        return;
    }

    result = (wombatQueueItem*)calloc( 1, sizeToAlloc);
    result->mChunkNext = impl->mChunks;
    impl->mChunks   = result;
    impl->mFirstFree.mNext = result;

    /* Create the links */
    {
        unsigned int i;
        for (i = 0; i < items - 1; i++) /* -1 so last is NULL */
        {
            result[i].mNext   = &result[i+1];
            result[i+1].mPrev = &result[i];
        }
    }
}

wombatQueueStatus
wombatQueue_unblock (wombatQueue queue)
{
    wombatQueueImpl* impl    = (wombatQueueImpl*)queue;

    impl->mUnblocking = 1;
    wsem_post (&impl->mSem);
    return WOMBAT_QUEUE_OK;
}

wombatQueueStatus
wombatQueue_flush (wombatQueue queue, wombatQueueFlushCb cb, void* closure)
{
    wombatQueueImpl* impl    = (wombatQueueImpl*)queue;
    wombatQueueItem* head    = NULL;

    for (;;) /* until the queue is empty */
    {
        if (wsem_trywait (&impl->mSem) != 0)
        {
            /* Queue is empty */
            return WOMBAT_QUEUE_OK;
        }

        wthread_mutex_lock (&impl->mLock); 

        head             = impl->mHead.mNext;
        WQ_REMOVE (impl, head);

        cb (queue, head->mData, head->mClosure, closure);

        wthread_mutex_unlock (&impl->mLock);

        return WOMBAT_QUEUE_OK;
    }
}

wombatQueueStatus
wombatQueue_next (wombatQueue queue, void** data, void** closure)
{
    wombatQueueImpl* impl     = (wombatQueueImpl*)queue;
    wthread_mutex_lock (&impl->mLock);
    impl->mIterator = impl->mIterator->mNext;
    
    if (impl->mIterator == &impl->mTail) /* at end */
    { 
        if (data)
            *data = NULL;
        if (closure)
            *closure = NULL;

        /* Position it on the last element so insertAfter() and cur() work as
         * expected.
         */
        impl->mIterator = impl->mIterator->mPrev; 
        wthread_mutex_unlock (&impl->mLock);
        return WOMBAT_QUEUE_END;
    }

    if (data)
        *data = impl->mIterator->mData;
    if (closure)
        *closure = impl->mIterator->mClosure;

    wthread_mutex_unlock (&impl->mLock);
    return WOMBAT_QUEUE_OK;
}

wombatQueueStatus
wombatQueue_prev (wombatQueue queue, void** data, void** closure)
{
    wombatQueueImpl* impl     = (wombatQueueImpl*)queue;
    wthread_mutex_lock (&impl->mLock);
    impl->mIterator = impl->mIterator->mPrev;
    
    if (impl->mIterator == &impl->mHead) /* at beginning */
    {
        if (data)
            *data = NULL;
        if (closure)
            *closure = NULL;

        /* Position it on the first element so insertBefore() and cur() work as
         * expected.
         */
        impl->mIterator = impl->mIterator->mNext; 
        wthread_mutex_unlock (&impl->mLock);
        return WOMBAT_QUEUE_END;
    }

    if (data)
        *data = impl->mIterator->mData;
    if (closure)
        *closure = impl->mIterator->mClosure;

    wthread_mutex_unlock (&impl->mLock);
    return WOMBAT_QUEUE_OK;
}

wombatQueueStatus
wombatQueue_cur (wombatQueue queue, void** data, void** closure)
{
    wombatQueueImpl* impl     = (wombatQueueImpl*)queue;
    wthread_mutex_lock (&impl->mLock);
    
    if (impl->mIterator == &impl->mHead || impl->mIterator == &impl->mTail) 
    {
        if (data)
            *data = NULL;
        if (closure)
            *closure = NULL;

        wthread_mutex_unlock (&impl->mLock);
        return WOMBAT_QUEUE_END;
    }

    if (data)
        *data = impl->mIterator->mData;
    if (closure)
        *closure = impl->mIterator->mClosure;

    wthread_mutex_unlock (&impl->mLock);
    return WOMBAT_QUEUE_OK;
}

wombatQueueStatus
wombatQueue_remove (wombatQueue queue, void** data, void** closure)
{
    wombatQueueImpl* impl     = (wombatQueueImpl*)queue;
    wombatQueueItem* tmp;
    wthread_mutex_lock (&impl->mLock);
    
    if (impl->mIterator == &impl->mHead || impl->mIterator == &impl->mTail) 
    {
        wthread_mutex_unlock (&impl->mLock);
        return WOMBAT_QUEUE_END;
    }

    if (data)
        *data = impl->mIterator->mData;
    if (closure)
        *closure = impl->mIterator->mClosure;

    /* remove the item */
    tmp = impl->mIterator;
    impl->mIterator = impl->mIterator->mNext;
    tmp->mPrev->mNext = tmp->mNext;
    tmp->mNext->mPrev = tmp->mPrev;
   
    /* put it on the free list. */
    tmp->mNext = impl->mFirstFree.mNext;
    impl->mFirstFree.mNext = tmp;

    wthread_mutex_unlock (&impl->mLock);
    return WOMBAT_QUEUE_OK;
}

wombatQueueStatus
wombatQueue_insertAfter (wombatQueue   queue, 
                         wombatQueueCb cb,
                         void*         data, 
                         void*         closure)
{
    wombatQueueImpl* impl = (wombatQueueImpl*)queue;
    wombatQueueItem* item = NULL;

    wthread_mutex_lock (&impl->mLock);

    /* If we are empty and positioned on the tail move to the head. */
    if (impl->mIterator == &impl->mTail)
        impl->mIterator = &impl->mHead;

    /* If there are no items in the free list, allocate some. It will set the
     * Next free node to NULL if the queue is too big or there is no memeory.
     */
    if (impl->mFirstFree.mNext == NULL)
        wombatQueueImpl_allocChunk (impl, impl->mChunkSize);
    item = impl->mFirstFree.mNext;

    if (item == NULL)
        return WOMBAT_QUEUE_FULL;

    impl->mFirstFree.mNext = item->mNext;
    /* Initialize the item. */
    item->mCb      = cb;
    item->mData    = data;
    item->mClosure = closure;

    /* Put on queue */
    item->mNext                   = impl->mIterator->mNext;
    item->mPrev                   = impl->mIterator;
    impl->mIterator->mNext->mPrev = item;
    impl->mIterator->mNext        = item;

    /* Notify next available thread that an item is ready */
    wsem_post (&impl->mSem);
    wthread_mutex_unlock (&impl->mLock);

    return WOMBAT_QUEUE_OK;
}

wombatQueueStatus
wombatQueue_insertBefore (wombatQueue   queue, 
                          wombatQueueCb cb,
                          void*         data, 
                          void*         closure)
{
    wombatQueueImpl* impl = (wombatQueueImpl*)queue;
    wombatQueueItem* item = NULL;

    wthread_mutex_lock (&impl->mLock);

    /* If we are empty and positioned on the head move to the tail. */
    if (impl->mIterator == &impl->mHead)
        impl->mIterator = &impl->mTail;

    /* If there are no items in the free list, allocate some. It will set the
     * Next free node to NULL if the queue is too big or there is no memeory.
     */
    if (impl->mFirstFree.mNext == NULL)
        wombatQueueImpl_allocChunk (impl, impl->mChunkSize);
    item = impl->mFirstFree.mNext;

    if (item == NULL)
        return WOMBAT_QUEUE_FULL;

    impl->mFirstFree.mNext = item->mNext;
    /* Initialize the item. */
    item->mCb      = cb;
    item->mData    = data;
    item->mClosure = closure;

    /* Put on queue */
    item->mNext                   = impl->mIterator;
    item->mPrev                   = impl->mIterator->mPrev;
    impl->mIterator->mPrev->mNext = item;
    impl->mIterator->mPrev        = item;

    /* Notify next available thread that an item is ready */
    wsem_post (&impl->mSem);
    wthread_mutex_unlock (&impl->mLock);

    return WOMBAT_QUEUE_OK;
}

wombatQueueStatus
wombatQueue_replace (wombatQueue   queue, 
                     wombatQueueCb cb,
                     void*         data, 
                     void*         closure)
{
    wombatQueueImpl* impl     = (wombatQueueImpl*)queue;
    wthread_mutex_lock (&impl->mLock);
    
    if (impl->mIterator == &impl->mHead || impl->mIterator == &impl->mTail) 
    {
        wthread_mutex_unlock (&impl->mLock);
        return WOMBAT_QUEUE_END;
    }

    impl->mIterator->mData    = data;
    impl->mIterator->mClosure = closure;

    wthread_mutex_unlock (&impl->mLock);
    return WOMBAT_QUEUE_OK;
}

wombatQueueStatus
wombatQueue_begin (wombatQueue queue)
{
    wombatQueueImpl* impl     = (wombatQueueImpl*)queue;

    wthread_mutex_lock (&impl->mLock);
    impl->mIterator = &impl->mHead;
    wthread_mutex_unlock (&impl->mLock);

    return WOMBAT_QUEUE_OK;
}

wombatQueueStatus
wombatQueue_end (wombatQueue queue)
{
    wombatQueueImpl* impl     = (wombatQueueImpl*)queue;

    wthread_mutex_lock (&impl->mLock);
    impl->mIterator = &impl->mTail;
    wthread_mutex_unlock (&impl->mLock);

    return WOMBAT_QUEUE_OK;
}
