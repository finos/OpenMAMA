/* $Id: wSemaphore.c,v 1.3.16.4 2011/09/07 09:45:08 emmapollock Exp $
 *
 * OpenMAMA: The open middleware agnostic messaging API
 * Copyright (C) 2011 NYSE Inc.
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

#include "wombat/wincompat.h"
#include "wombat/wSemaphore.h"

#ifndef WIN32
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <sys/time.h>


int wsem_timedwait (wsem_t* sem, unsigned int timeoutval) 
{
    struct timespec    ts;
    struct timeval tv;

    /*NB! - sem_timedwait() uses an absolute timeout on Linux!!*/
    if (0!=gettimeofday (&tv, NULL))
    {
        return 0;
    }

    /*Convert the timeval to timespec*/
    ts.tv_sec = tv.tv_sec;             
    ts.tv_nsec = tv.tv_usec * 1000;

    /* Now add our relative timeout to the current time
       Integer division truncation will give us the seconds.
       The timeout passed in is in milliseconds*/
    ts.tv_sec += (timeoutval/1000);
    /* Now calculate the relative nsec component*/
    ts.tv_nsec += ((timeoutval%1000) * 1000000);

    /* The nsec component may now be greater than the maximum possible value.
       If so adjust the members accordingly. */
    if (ts.tv_nsec>=1000000000)
    {
        ts.tv_nsec -= 1000000000;
        ts.tv_sec += 1;
    }

    return sem_timedwait (sem, &ts); 
}


#else


/**
 * Waiting on Win32 semaphore involves a system call and is very inefficient. When
 * we test to determine if the queue is empty with a Win32 semaphore
 * WaitForSingleObject consumes all the CPU!!!!. We can be much more efficient using
 * a critical section and not call WaitForSingleObject when there is nothing on the Queue.
 */

typedef struct
{
    unsigned int     mCount;
    HANDLE           mEvent;
    CRITICAL_SECTION mCs;
} sem_t;

int wsem_init (wsem_t* result, int dummy, int count)
{
    sem_t* sem = (sem_t*)malloc (sizeof (sem_t));
    if (sem == NULL)
    {
        return 1;
    }
   
    *result              = sem; 
    sem->mCount          = count;
    sem->mEvent          = CreateEvent (NULL, TRUE, FALSE, NULL);
    if (sem->mEvent == INVALID_HANDLE_VALUE)
    {
        return 1;
    }
    InitializeCriticalSection (&sem->mCs);

    return 0;
}
    
int wsem_destroy (wsem_t* sem)
{
    sem_t* impl = (sem_t*)(*sem);
    CloseHandle (impl->mEvent);
    DeleteCriticalSection (&impl->mCs);
    free (impl);
    return 0;
}

int wsem_post (wsem_t* sem)
{
    sem_t* impl = (sem_t*)(*sem);
    EnterCriticalSection (&impl->mCs);
    impl->mCount++;
    
    /* MLS: we might be able to optimize here and only call SetEvent when the 
     * count is 1; however, this would require extensive testing as a race
     * condition would stop dispatching altogether. I don't see any obvious
     * race conditions but I am a little concerned that there might be undefined
     * behavior when SetEvent and WaitForSingleObject race. MS does not 
     * indicate this directly but the documentation for PulseEvent() mentions
     * some potential problems. We don't use pulse event for this reason.
     */
    SetEvent (impl->mEvent);
    LeaveCriticalSection (&impl->mCs);
    return 0;
}

int wsem_wait (wsem_t* sem)
{
    unsigned int t = INFINITE;
    return wsem_timedwait (sem, t);
     
}

int wsem_trywait (wsem_t* sem)
{
    sem_t* impl = (sem_t*)(*sem);
    int wouldWait = 0;

    EnterCriticalSection (&impl->mCs);
    if (impl->mCount)
    {
        impl->mCount--;
    }
    else
    {
        wouldWait = 1;
    }
    LeaveCriticalSection (&impl->mCs);

    return wouldWait;
}

int wsem_getvalue (wsem_t* sem, int* items)
{
    sem_t* impl = (sem_t*)(*sem);
    EnterCriticalSection (&impl->mCs);
    *items = impl->mCount;
    LeaveCriticalSection (&impl->mCs);

    return 0;
}



int wsem_timedwait (wsem_t* sem, unsigned int timeoutval) 
{
    DWORD ts = (DWORD) timeoutval;
    sem_t* impl = (sem_t*)(*sem);
    int wait = 0;

    do
    {
        EnterCriticalSection (&impl->mCs);
        if (impl->mCount <= 1) ResetEvent (impl->mEvent);
        if (impl->mCount)
        {
            impl->mCount--;
            LeaveCriticalSection (&impl->mCs);
            return 0;
        }
        LeaveCriticalSection (&impl->mCs);
    } while (WAIT_OBJECT_0 == WaitForSingleObject (impl->mEvent, ts));

    /* Time out or worse */
    return -1;
}

#endif
