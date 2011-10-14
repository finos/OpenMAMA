/* $Id: wincompat.h,v 1.28.2.1.8.3 2011/08/10 14:53:24 nicholasmarriott Exp $
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

#ifndef _WOMBAT_WINCOMPAT_H
#define _WOMBAT_WINCOMPAT_H

#include "wConfig.h"

#define PATHSEP  "/"
#define CPU_AFFINITY_SET 				cpu_set_t

#define wthread_mutex_t         		pthread_mutex_t
#define wthread_mutex_init      		pthread_mutex_init
#define wthread_mutex_unlock    		pthread_mutex_unlock
#define wthread_mutex_lock      		pthread_mutex_lock
#define wthread_mutex_destroy   		pthread_mutex_destroy

#define wthread_t               		pthread_t
#define wthread_detach          		pthread_detach
#define wthread_self            		pthread_self
#define wthread_equal           		pthread_equal
#define wthread_cleanup_push    		pthread_cleanup_push
#define wthread_cleanup_pop     		pthread_cleanup_pop
#define wthread_join					pthread_join
#define wthread_create					pthread_create
#define wthread_exit            		pthread_exit

#define wthread_cond_t	        		pthread_cond_t
#define wthread_cond_init       		pthread_cond_init
#define wthread_cond_signal     		pthread_cond_signal
#define wthread_cond_destroy    		pthread_cond_destroy
#define wthread_cond_wait				pthread_cond_wait

#define wthread_spinlock_t    			pthread_spinlock_t
#define wthread_spin_init      			pthread_spin_init
#define wthread_spin_unlock    			pthread_spin_unlock
#define wthread_spin_lock      			pthread_spin_lock
#define wthread_spin_destroy   			pthread_spin_destroy

#define wdlopen                 		dlopen
#define wdlsym							dlsym
#define wdlclose						dlclose
#define wdlerror						dlerror

#define wthread_attr_t   				pthread_attr_t
#define wthread_attr_init 				pthread_attr_init
#define wthread_attr_setdetachstate 	pthread_attr_setdetachstate

#define wthread_mutexattr_t 			pthread_mutexattr_t
#define wthread_mutexattr_init 			pthread_mutexattr_init
#define wthread_mutexattr_settype 		pthread_mutexattr_settype

#define wGetCurrentThreadId     		pthread_self

#endif /* _WOMBAT_WINCOMPAT_H */
