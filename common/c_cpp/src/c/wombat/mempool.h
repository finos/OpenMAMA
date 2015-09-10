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

#include <wombat/wSemaphore.h>

#ifndef WOMBAT_MEMPOOL_H__
#define WOMBAT_MEMPOOL_H__

#include <wombat/memdefs.h>

/*=========================================================================
  =                             Includes                                  =
  =========================================================================*/


#if defined(__cplusplus)
extern "C" {
#endif

typedef void (*memoryPoolIteratorCb)(memoryPool* pool, memoryNode* node);

/**
 * This function will create a new memory pool containing a number of nodes. The
 * number of nodes is growable so new nodes will be allocated and added to the
 * pool even after the initial pool size has been filled. This implementation is
 * thread safe.
 *
 * @param poolSize      This is the initial number of nodes to be created in the
 *                      pool at create time.
 * @param nodeSize      This is the size of each node which is to be created at
 *                      create time.
 *
 * @return Newly created memory pool.
 */
memoryPool* memoryPool_create       (size_t poolSize, size_t nodeSize);

/**
 * This function will add a single node of the provided size to the memory pool
 *
 * @param pool          The pool to add a new node to.
 * @param nodeSize      The size of the new node to be created before adding to
 *                      the pool.
 *
 * @return Returns newly created node on success and NULL on failure.
 */
memoryNode* memoryPool_addNode      (memoryPool* pool, size_t nodeSize);

/**
 * This function will return a node from the provided memory pool and will
 * ensure that it will be the requested size before it's handed to the caller.
 *
 * @param pool          The pool to get the node from.
 * @param nodeSize      The size of the new node to be pulled out of the pool.
 *
 * @return Next available memory node of the requested size.
 */
memoryNode* memoryPool_getNode      (memoryPool* pool, size_t nodeSize);

/**
 * This function will return the provided node back into the pool provided.
 *
 * @param pool          The pool to return the node to.
 * @param node          The node to be returned.
 */
void        memoryPool_returnNode   (memoryPool* pool, memoryNode* node);

/**
 * This function will call the iterator callback for every allocated node.
 *
 * @param pool          The pool to return the node to.
 * @param node          The callback to be triggered
 */
void        memoryPool_iterate      (memoryPool*            pool,
                                     memoryPoolIteratorCb   callback);
/**
 * This function will destroy the pool and all members within it in a non-thread
 * safe manner as it assumes all nodes have been returned before this function
 * is called.
 *
 * @param pool          The pool to return the node to.
 * @param callback      This callback will be triggered before each node is
 *                      destroyed to let the caller do any cleanup required.
 */
void        memoryPool_destroy      (memoryPool*            pool,
                                     memoryPoolIteratorCb   callback);

#if defined(__cplusplus)
}
#endif

#endif /* WOMBAT_MEMPOOL_H__ */
