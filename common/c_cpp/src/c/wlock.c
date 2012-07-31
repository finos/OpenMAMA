/* $Id: wlock.c,v 1.6.28.3 2011/08/10 14:53:24 nicholasmarriott Exp $
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

#include "wombat/port.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <wlock.h>

typedef struct
{
    wthread_mutex_t    mMutex;
} wLockImpl;


wLock
wlock_create( void )

{
    wLockImpl* rval = calloc(1, sizeof( wLockImpl ) );

    wthread_mutexattr_t attr;
    wthread_mutexattr_init (&attr);
    wthread_mutexattr_settype (&attr, PTHREAD_MUTEX_RECURSIVE_NP);
    wthread_mutex_init (&rval->mMutex, &attr);

    return( rval );
}

void
wlock_destroy( wLock lock )
{
    wLockImpl* impl = (wLockImpl*) lock;

    wthread_mutex_destroy( &impl->mMutex );
    free( lock );
}

void
wlock_lock( wLock lock )
{
    wLockImpl* impl = (wLockImpl*) lock;

    wthread_mutex_lock( &impl->mMutex );
}

void 
wlock_unlock( wLock lock )
{
    wLockImpl* impl = (wLockImpl*) lock;

    wthread_mutex_unlock( &impl->mMutex );   
}
