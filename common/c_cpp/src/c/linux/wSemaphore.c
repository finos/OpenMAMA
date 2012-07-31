/* $Id: wSemaphore.c,v 1.3.16.4 2011/09/07 09:45:08 emmapollock Exp $
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

#include "wombat/wSemaphore.h"

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
