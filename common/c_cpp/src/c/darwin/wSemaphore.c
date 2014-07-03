/*
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

#include "wombat/wSemaphore.h"
#include <sys/errno.h>
#include <libkern/OSAtomic.h>


#define WSEM_CHECK_NULL(wsem_t)     \
    if (!wsem_t) {                  \
        errno = EFAULT;             \
        return WSEM_FAILED;         \
    }                               \


int wsem_init (wsem_t * sem, int shared, unsigned int value)
{
    WSEM_CHECK_NULL (sem);

    sem->dsema = dispatch_semaphore_create (value);
    if (!sem->dsema)
    {
        return WSEM_FAILED;
    }
    sem->count = value;

    return WSEM_SUCCEED;
}

int wsem_destroy (wsem_t * sem)
{
    WSEM_CHECK_NULL (sem);

    dispatch_release (sem->dsema);

    return WSEM_SUCCEED;
}

int wsem_getvalue (wsem_t * sem, int * i)
{
    WSEM_CHECK_NULL (sem);

    (*i) = sem->count;

    return WSEM_SUCCEED;
}

int wsem_post (wsem_t * sem)
{
    WSEM_CHECK_NULL (sem);

    int result;
    if (dispatch_semaphore_signal (sem->dsema) == 0)
    {
        OSAtomicIncrement32Barrier (&sem->count);
        result =  WSEM_SUCCEED;
    }
    else
    {
        result = WSEM_FAILED;
    }

    return result;
}

int wsem_wait (wsem_t * sem)
{
    WSEM_CHECK_NULL (sem);

    int result;
    if (dispatch_semaphore_wait (sem->dsema, DISPATCH_TIME_FOREVER) == 0)
    {
        if (sem->count)
        {
            OSAtomicDecrement32Barrier (&sem->count);
        }
        result = WSEM_SUCCEED;
    }
    else
    {
        result = WSEM_FAILED;
    }

    return result;
}

int wsem_trywait (wsem_t * sem)
{
    WSEM_CHECK_NULL (sem);

    int result;
    if (dispatch_semaphore_wait (sem->dsema, DISPATCH_TIME_NOW) == 0)
    {
        if (sem->count)
        {
            OSAtomicDecrement32Barrier (&sem->count);
        }
        result = WSEM_SUCCEED;
    }
    else
    {
        result = WSEM_FAILED;
    }

    return result;
}

int wsem_timedwait (wsem_t* sem, unsigned int ts)
{
    WSEM_CHECK_NULL (sem);

    int result;
    if (dispatch_semaphore_wait (sem->dsema,
                                dispatch_time (DISPATCH_TIME_NOW, ts * 1000)) == 0)
    {
        if (sem->count)
        {
            OSAtomicDecrement32Barrier (&sem->count);
        }
        result = WSEM_SUCCEED;
    }
    else
    {
        result = WSEM_FAILED;
    }

    return result;
}
