/* $Id: queue.h,v 1.1.28.4 2011/08/31 16:33:53 mikeschonberg Exp $
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

#ifndef WOMBAT_QUEUE_H__
#define WOMBAT_QUEUE_H__

/*
 * Fully thread safe queue implementation.
 */
#ifdef SEM_VALUE_MAX
#define WOMBAT_QUEUE_MAX_SIZE SEM_VALUE_MAX /* 2_147_483_647 on Linux */
#else
#define WOMBAT_QUEUE_MAX_SIZE 2147483647 /* max possible size */
#endif
#define WOMBAT_QUEUE_CHUNK_SIZE 64  /* default chunk size */

#if defined (__cplusplus)
extern "C"
{
#endif


/* Callback for dispatching events from a queue. */
typedef void (MAMACALLTYPE *wombatQueueCb)(void* data, void* closure);

typedef void* wombatQueue;

typedef enum
{
    WOMBAT_QUEUE_OK          = 1,
    WOMBAT_QUEUE_FULL        = 2,
    WOMBAT_QUEUE_NOMEM       = 3,
    WOMBAT_QUEUE_SEM_ERR     = 4,
    WOMBAT_QUEUE_MTX_ERR     = 5,
    WOMBAT_QUEUE_INVALID_ARG = 6,
    WOMBAT_QUEUE_WOULD_BLOCK = 7,
    WOMBAT_QUEUE_END         = 8,
    WOMBAT_QUEUE_TIMEOUT     = 9
} wombatQueueStatus;

/**
 * Allocate a queue. The queue will not be usable until wombatQueue_create()
 * is called.
 *
 * Size and other parameters may be set prior to calling wombatQueue_create().
 */
COMMONExpDLL wombatQueueStatus
wombatQueue_allocate (wombatQueue *result);

/**
 * Initialize the Queue. The growBySize indicates how many 
 * elements to allocate at a time.
 *
 * The max size bounds the queue to the specified size. The default is
 * unbounded up to WOMBAT_QUEUE_MAX_SIZE.
 *
 */
COMMONExpDLL wombatQueueStatus
wombatQueue_create (wombatQueue queue, uint32_t maxSize, uint32_t initialSize,
                    uint32_t growBySize);

/**
 * Destroy the Queue. 
 */
COMMONExpDLL wombatQueueStatus
wombatQueue_destroy (wombatQueue queue);

/** 
 * Set the maximum size of the queue. WOMBAT_QUEUE_MAX_SIZE is the maximum
 * queue size permitted and the default value. This value should be a multiple
 * of the chunk size.
 *
 * If it is not the actual max size will be rounded up to a multiple of the
 * chunk size.
 */
COMMONExpDLL wombatQueueStatus
wombatQueue_setMaxSize (wombatQueue queue, unsigned int value);

/**
 * Get the maximum size of the queue. WOMBAT_QUEUE_MAX_SIZE is the maximum
 * queue size permitted and the default value.
 */
COMMONExpDLL wombatQueueStatus
wombatQueue_getMaxSize (wombatQueue queue, unsigned int *value);

/**
 * Get the number of items currently in the queue.
 */
COMMONExpDLL wombatQueueStatus
wombatQueue_getSize (wombatQueue queue, int* size);

/**
 * Enqueue an event. 
 */
COMMONExpDLL wombatQueueStatus
wombatQueue_enqueue (wombatQueue queue, 
                     wombatQueueCb cb, 
                     void* data, 
                     void* closure);


/**
 * Dequeue an item. If a non-NULL callback was specified when the item was
 * enqueued the callback will be invoked. If the data argument is not NULL,
 * the dispatched item will be assigned as well.
 *
 * This call blocks until an item is enqueued if the queue is empty.
 *
 */
COMMONExpDLL wombatQueueStatus
wombatQueue_dispatch (wombatQueue queue, void** data, void** closure);

/**
 * Dequeue an item. If a non-NULL callback was specified when the item was
 * enqueued the callback will be invoked. If the data argument is not NULL,
 * the dispatched item will be assigned as well.
 *
 * If the queue is empty wait timout milliseconds before timing out and
 * returning WOMBAT_QUEUE_TIMEOUT
 *
 */
COMMONExpDLL wombatQueueStatus
wombatQueue_timedDispatch (wombatQueue queue, void** data, void** closure, 
        uint64_t timeout);

/**
 * Poll. This function deques and item if the queue is not empty otherwise it
 * returns immediately with WOMBAT__WOULD_BLOCK.
 */
COMMONExpDLL wombatQueueStatus
wombatQueue_poll (wombatQueue queue, void** data, void** closure);


/**
 * Cause a waiting thread to unblock without dequing an item. This is useful
 * when cleaning up.
 */
COMMONExpDLL wombatQueueStatus
wombatQueue_unblock (wombatQueue queue);

/**
 * Remove each item from the queue in order and invoke the supplied callback.
 * This is an atomic operation.,
 *
 * The itemClosure is the closure, if any, specified when the item was
 * enqueued. The "closure" is the closure passed to wombatQueue_flush().
 *
 */
typedef void (MAMACALLTYPE *wombatQueueFlushCb)(wombatQueue queue, 
                                                void* data, 
                                                void* itemClosure,
                                                void* closure);

COMMONExpDLL wombatQueueStatus
wombatQueue_flush (wombatQueue queue, wombatQueueFlushCb cb, void* closure);

/**
 * The queue contains a single iterator. This returns the next item. If the
 * iterator is at the beginning of the queue, it returns the first item.
 * It returns WOMBAT_QUEUE_END and sets the data and closure to NULL if the
 * iterator is at the end of the queue. In this case the iterator does not
 * move.
 */
COMMONExpDLL wombatQueueStatus
wombatQueue_next (wombatQueue queue, void** data, void** closure);

/**
 * The queue contains a single iterator. This returns the previous item. If the
 * iterator is at the end of the queue, it returns the last item.
 * It returns WOMBAT_QUEUE_END and sets the data and closure to NULL if the
 * iterator is at the beginning of the queue.
 */
COMMONExpDLL wombatQueueStatus
wombatQueue_prev (wombatQueue queue, void** data, void** closure);

/**
 * The queue contains a single iterator. This returns the current item. If the
 * the queue is empty or the iterator is not initialized (next() not called), 
 * it returns WOMBAT_QUEUE_END, and does sets the data and closure to NULL.
 */
COMMONExpDLL wombatQueueStatus
wombatQueue_cur (wombatQueue queue, void** data, void** closure);

/**
 * Remove the current item. If the queue is empty this method returns
 * WOMBAT_QUEUE_END.
 */
COMMONExpDLL wombatQueueStatus
wombatQueue_remove (wombatQueue queue, void** data, void** closure);

/**
 * Insert a new item after the current item. The iterator is not moved. If the
 * queue is empty the iterator is positioned so that next() returns the newly
 * inserted item.
 */
COMMONExpDLL wombatQueueStatus
wombatQueue_insertAfter (wombatQueue   queue, 
                         wombatQueueCb cb,
                         void*         data, 
                         void*         closure);

/**
 * Insert a new item before the current item. The iterator is not moved. If the
 * queue is empty the iterator is positioned so that prev() returns the newly
 * inserted item.
 */
COMMONExpDLL wombatQueueStatus
wombatQueue_insertBefore (wombatQueue   queue, 
                          wombatQueueCb cb,
                          void*         data, 
                          void*         closure);

/**
 * Replace the data and closure for the current node. If the iterator is not
 * positioned because the queue is empty or next() has not been called, 
 * it will return WOMBAT_QUEUE_END. 
 */
COMMONExpDLL wombatQueueStatus
wombatQueue_replace (wombatQueue   queue, 
                     wombatQueueCb cb, 
                     void*         data, 
                     void*         closure);

/**
 * Position the iterator so next() will return the first element.
 */
COMMONExpDLL wombatQueueStatus
wombatQueue_begin (wombatQueue queue);

/**
 * Position the iterator so prev() will return the last element.
 */
COMMONExpDLL wombatQueueStatus
wombatQueue_end (wombatQueue queue);


#if defined (__cplusplus)
}
#endif

#endif /* WOMBAT_QUEUE_H__ */
