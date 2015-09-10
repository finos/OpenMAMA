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

#include <wombat/memnode.h>


memoryNode*
memoryNode_create        (size_t              size)
{
    memoryNode* newNode = NULL;
    newNode = (memoryNode*) calloc (1, sizeof(memoryNode));
    if (NULL == newNode)
    {
        errno = ENOMEM;
        return NULL;
    }
    if (size > 0)
    {
        if (0 != allocateBufferMemory ((void**)&newNode->mNodeBuffer,
                                       &newNode->mNodeCapacity,
                                       size))
        {
            errno = ENOMEM;
            free (newNode);
            return NULL;
        }
    }

    return newNode;
}

int
memoryNode_stretch       (memoryNode*         node,
                          size_t              size)
{
    return allocateBufferMemory ((void**)&node->mNodeBuffer,
                                 &node->mNodeCapacity,
                                 size);
}

void
memoryNode_empty         (memoryNode*          node)
{
    if (NULL != node->mNodeBuffer)
    {
        free (node->mNodeBuffer);
        node->mNodeBuffer = NULL;
        node->mNodeCapacity = 0;
        node->mNodeSize = 0;
    }
}

void
memoryNode_destroy       (memoryNode*         node)
{
    memoryNode_empty (node);
    free (node);
}

int
allocateBufferMemory     (void**       buffer,
                          size_t*      size,
                          size_t       newSize)
{
    void* newbuf = NULL;

    if (0 == *size || NULL == *buffer)
    {
        newbuf = calloc (newSize, 1);

        if (newbuf == NULL)
        {
            errno = ENOMEM;
            return -1;
        }
        else
        {
            *buffer = newbuf;
            *size   = newSize;
            return 0;
        }
    }
    else if (newSize > *size)
    {
        newbuf = realloc (*buffer, newSize);

        if (newbuf == NULL)
        {
            errno = ENOMEM;
            return -1;
        }
        else
        {
            /* set newly added bytes to 0 */
            memset ((uint8_t*) newbuf + *size, 0, newSize - *size);

            *buffer = newbuf;
            *size   = newSize;
            return 0;
        }
    }
    else
    {
        /* No allocation / reallocation required */
        return 0;
    }
}
