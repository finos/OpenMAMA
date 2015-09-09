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

#ifndef WOMBAT_MEMDEFS_H__
#define WOMBAT_MEMDEFS_H__

/*=========================================================================
  =                             Includes                                  =
  =========================================================================*/


#if defined(__cplusplus)
extern "C" {
#endif

typedef struct memoryNode_ memoryNode;
typedef struct memoryPool_ memoryPool;

struct memoryNode_
{

    size_t          mNodeSize;          /* The size of data in buffer */
    size_t          mNodeCapacity;      /* Total bytes in buffer */
    uint8_t*        mNodeBuffer;        /* The data itself */
    memoryNode*     mNext;              /* If part of pool, this is next node */
    memoryPool*     mPool;              /* If part of pool, this is the pool */
};

struct memoryPool_
{
    memoryNode*     mFreeList;          /* Linked list of avail nodes */
    size_t          mAllocateBlockSize; /* mAllocatedNodes buffer will be
                                           extended in blocks of this size */
    memoryNode      mAllocatedNodes;    /* Node containing tracker array */
    size_t          mNumNodesTotal;     /* Number of nodes in the pool */
    size_t          mNumNodesFree;      /* Number of free nodes in the pool */
    wthread_mutex_t mLock;
};


#if defined(__cplusplus)
}
#endif

#endif /* WOMBAT_MEMDEFS_H__ */
