
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

#ifndef STATICLOCK_H__
#define STATICLOCK_H__

/* Make windows synchronization look like pthreads we use 'w' rather than 'p' to 
 * avoid conflicts with other pthreads for windows implementations
 */
typedef CRITICAL_SECTION wthread_mutex_t;
typedef HANDLE wthread_cond_t;
typedef void * wthread_t;

#define INVALID_THREAD NULL

#define PTHREAD_CREATE_JOINABLE     0
#define PTHREAD_MUTEX_RECURSIVE_NP  0
#define PTHREAD_MUTEX_RECURSIVE     0

typedef wthread_mutex_t wthread_spinlock_t;

#define wthread_spin_lock    wthread_mutex_lock
#define wthread_spin_unlock  wthread_mutex_unlock
#define wthread_spin_init    wthread_mutex_init
#define wthread_spin_destroy    wthread_mutex_destroy

#define wthread_mutex_init( h, zip ) (InitializeCriticalSection( (h) ))
#define wthread_mutex_unlock( h )    (LeaveCriticalSection( (h) ) )
#define wthread_mutex_lock( h )      (EnterCriticalSection( (h) ) )
#define wthread_mutex_destroy( h )   (DeleteCriticalSection( (h) ) )

/* Windows does not have static lock initializers */
/* the xxxUseLock variable is needed if 2 threads are started together, the
 * first thread checks the lexerlockInitialized through atomic action and then
 * starts to initialize the mutex which isn't atomic. The second thread checks
 * the variable lexerlockInitialized it has been incremented, this thread would
 * then try to grab a lock that is not initialized. On windows this causes the
 * application to hang therefore after the mutex is initialized the variable
 * xxxUseLock is set to 1. Just before a lock is acquired it will poll until
 * xxxUseLock is not equal to zero this will stop the race condition. A
 * condition variable can't be used because you get into the same problem of
 * waiting for an uninitialized condition variable 
 */
typedef struct                                     
{                                          
    wthread_mutex_t mLock;                 
    volatile long   mInitialized;      
    volatile long   mUseLock;          
} wthread_static_mutex_t;

#define WSTATIC_MUTEX_INITIALIZER {0,0,0}

#define wthread_static_mutex_lock(x)                                \
    if( InterlockedIncrement(&((x)->mInitialized)) == 1 ) \
    {                                                    \
        wthread_mutex_init(&((x)->mLock), NULL);          \
        (x)->mUseLock = 1;                                \
    }                                                    \
    while((x)->mUseLock == 0);                            \
    wthread_mutex_lock(&((x)->mLock))

#define wthread_static_mutex_unlock(x) wthread_mutex_unlock(&((x)->mLock))

#endif /* STATIClOCK_H__ */
