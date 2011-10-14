/* $Id: wSemaphore.h,v 1.3.16.3 2011/08/10 14:53:24 nicholasmarriott Exp $
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

#ifndef WSEMAPHORE_H__
#define WSEMAPHORE_H__

#ifndef WIN32

#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define wsem_t          sem_t          
#define wsem_init       sem_init       
#define wsem_post       sem_post       
#define wsem_wait       sem_wait       
#define wsem_trywait    sem_trywait    
#define wsem_destroy    sem_destroy    
#define wsem_getvalue   sem_getvalue   

int wsem_timedwait (wsem_t* sem, unsigned int ts);

#else
#include "wombat/wincompat.h"

/**
 * Waiting on Win32 semaphore involves a system call and is very inefficient.
 * When we test to determine if the queue is empty with a Win32 semaphore
 * WaitForSingleObject consumes all the CPU. We can be much more efficient
 * using a critical section and not call WaitForSingleObject when there is
 * nothing on the Queue.
 */
typedef void* wsem_t;

COMMONExpDLL
int wsem_init (wsem_t* sem, int dummy, int count);

COMMONExpDLL
int wsem_destroy (wsem_t* sem);

COMMONExpDLL
int wsem_post (wsem_t* sem);

COMMONExpDLL
int wsem_wait (wsem_t* sem);

COMMONExpDLL
int wsem_timedwait (wsem_t* sem, unsigned int ts);

COMMONExpDLL
int wsem_trywait (wsem_t* sem);

COMMONExpDLL
int wsem_getvalue (wsem_t*, int* items);

#endif

#endif /* WSEMAPHORE_H__ */
