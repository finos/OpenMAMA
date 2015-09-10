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

#ifndef WOMBAT_MEMNODE_H__
#define WOMBAT_MEMNODE_H__

#include <wombat/memdefs.h>

/*=========================================================================
  =                             Includes                                  =
  =========================================================================*/


#if defined(__cplusplus)
extern "C" {
#endif

/**
 * A memory node is a data structure which manages an underlying elastic buffer.
 * This method will attempt to allocate a new instance of a memory node.
 *
 * @param size        Size of the underlying buffer to allocate when creating
 *                    this memory node.
 *
 * @return Will return a pointer to the new node on success and NULL on failure,
 *         setting errno appropriately.
 */
memoryNode*
memoryNode_create        (size_t              size);

/**
 * This function will set the size of the underlying buffer to the requested
 * size. If this is smaller than the existing size, no action will be taken. If
 * this is larger, the buffer will be reallocated.
 *
 * @param node        The memory node to update.
 * @param size        Size of the underlying buffer to allocate when creating
 *                    this memory node.
 *
 * @return Will return 0 on success and -1 on failure, setting errno
 *         appropriately.
 */
int
memoryNode_stretch       (memoryNode*         node,
                          size_t              size);

/**
 * This will empty the underlying buffer and set its size to zero without
 * destroying the node itself.
 *
 * @param node        The memory node to empty.
 */
void
memoryNode_empty         (memoryNode*         node);

/**
 * This will destroy the memory node provided and empty the underlying buffer.
 * Note this will not remove the node from any parent memory pools - that needs
 * to be handled separately.
 *
 * @param node        The memory node to empty.
 */
void
memoryNode_destroy       (memoryNode*         node);

/**
 * This function exists to centralize the allocation of buffer memory. When
 * called for the first time with an empty buffer, it will calloc the requested
 * memory. When requesting a size smaller than the current size, it will do
 * nothing. When requesting a size larger than the current size, it will perform
 * a realloc. On success, 0 will be returned, and if necessary, the buffer
 * pointer will be written back to reflect the new value. The currentSize
 * pointer will also be modified to reflect the new size. On failure, -1
 * will be returned indicating an error, errno will be set and the buffer and
 * current size will remain unchanged.
 *
 * @param buffer      Pointer to the buffer to examine and resize if necessary.
 * @param size        Current size of the buffer. Will be updated with the newly
 *                    requested newSize on success.
 * @param newSize     The newly requested buffer size.
 *
 * @return Will return 0 on success and -1 on failure.
 */
int
allocateBufferMemory     (void**       buffer,
                          size_t*      size,
                          size_t       newSize);

#if defined(__cplusplus)
}
#endif

#endif /* WOMBAT_MEMNODE_H__ */
