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

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>

#include <wombat/mempool.h>
#include <wombat/memnode.h>

#define POOL_ALLOCATE_BLOCK_SIZE 256
#define POOL_ALLOCATE_BLOCK_MASK 0xff


memoryPool*
memoryPool_create (size_t poolSize, size_t nodeSize)
{
    memoryPool* newPool = NULL;
    memoryNode* newNode = NULL;
    size_t i = 0;

    if (poolSize < 2)
    {
        errno = EINVAL;
        return NULL;
    }

    newPool = (memoryPool*) calloc (1, sizeof(memoryPool));
    if (NULL == newPool)
    {
        errno = ENOMEM;
        return NULL;
    }

    newPool->mAllocateBlockSize = poolSize;

    wthread_mutex_init(&newPool->mLock, NULL);

    /* Extend the message pool */
    for (i = 0; i < poolSize; i++)
    {
        newNode = memoryPool_addNode (newPool, nodeSize);
        /* If a node allocation failed, abort whole operation */
        if (NULL == newNode)
        {
            /* Nodes don't get linked until allocation so this is safe */
            memoryPool_destroy (newPool, NULL);
            errno = ENOMEM;
            return NULL;
        }
    }
    return newPool;
}

memoryNode*
memoryPool_addNode (memoryPool* pool, size_t nodeSize)
{
    memoryNode* newNode = memoryNode_create (nodeSize);
    memoryNode** tracker = NULL;

    if (NULL == newNode)
    {
        return NULL;
    }

    /* Only allocate the tracker array every POOL_ALLOCATE_BLOCK_SIZE */
    if ((pool->mNumNodesTotal & POOL_ALLOCATE_BLOCK_MASK) == 0)
    {
        size_t newSize = sizeof(memoryNode*) *
                (POOL_ALLOCATE_BLOCK_SIZE + pool->mNumNodesTotal);
        /* If we can't track this node, put it back */
        if (0 != memoryNode_stretch (&pool->mAllocatedNodes, newSize))
        {
            memoryNode_destroy(newNode);
            return NULL;
        }
    }

    tracker = (memoryNode**)pool->mAllocatedNodes.mNodeBuffer;

    /* Update tracker array with newly created node */
    tracker[pool->mNumNodesTotal] = newNode;

    /*
     * For the first message in the pool, we point the freelist
     * value to it. We then assign the currentNode to this value.
     */
    if (pool->mNumNodesTotal != 0)
    {
        newNode->mNext = pool->mFreeList;
    }
    pool->mFreeList = newNode;

    newNode->mPool = pool;

    /* Increment the free count for the message pool */
    pool->mNumNodesTotal++;
    pool->mNumNodesFree++;

    return newNode;
}

memoryNode*
memoryPool_getNode (memoryPool* pool, size_t nodeSize)
{
    memoryNode*  newNode  = NULL;

    wthread_mutex_lock(&pool->mLock);

    /*
     * Check if any buffers remain in the freeList, and if not extend the buffer
     */
    if (1 == pool->mNumNodesFree)
    {
        memoryPool_addNode (pool, nodeSize);
    }

    /*
     * Remove the next available node from the pool free list: get the node
     * pointer then assign the pool free list pointer to the next available free
     * node. Reduce the pool free message count by one.
     */
    newNode = pool->mFreeList;
    pool->mFreeList = newNode->mNext;
    pool->mNumNodesFree--;

    wthread_mutex_unlock(&pool->mLock);

    memoryNode_stretch (newNode, nodeSize);

    return newNode;
}


void
memoryPool_returnNode (memoryPool* pool, memoryNode* node)
{
    wthread_mutex_lock(&pool->mLock);

    /* Move returned node to the front of the freelist */
    node->mNext     = pool->mFreeList;
    pool->mFreeList = node;
    pool->mNumNodesFree++;

    wthread_mutex_unlock(&pool->mLock);
    return;
}

void
memoryPool_iterate (memoryPool* pool, memoryPoolIteratorCb callback)
{
    size_t      i = 0;
    memoryNode* node = NULL;
    memoryNode** tracker = NULL;
    wthread_mutex_lock(&pool->mLock);
    tracker = (memoryNode**)pool->mAllocatedNodes.mNodeBuffer;
    for (i = 0; i < pool->mNumNodesTotal; i++)
    {
        node = tracker[pool->mNumNodesTotal];
        if (NULL != node)
        {
            if (NULL != callback)
            {
                callback (pool, node);
            }
        }
    }
    wthread_mutex_unlock(&pool->mLock);
}

void
memoryPool_destroy (memoryPool* pool, memoryPoolIteratorCb callback)
{
    size_t      i = 0;
    memoryNode* node = NULL;
    memoryNode** tracker = NULL;
    wthread_mutex_lock(&pool->mLock);
    tracker = (memoryNode**)pool->mAllocatedNodes.mNodeBuffer;
    for (i = 0; i < pool->mNumNodesTotal; i++)
    {
        node = tracker[pool->mNumNodesTotal];
        if (NULL != node)
        {
            if (NULL != callback)
            {
                callback (pool, node);
            }
            memoryNode_empty(node);
        }
    }
    wthread_mutex_unlock(&pool->mLock);
    wthread_mutex_destroy(&pool->mLock);
    free (pool);
}

