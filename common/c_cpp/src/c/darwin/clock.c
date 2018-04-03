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

#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_12
/* Doing nothing, since OSX now supports these */
static void nothing(void) { }
#else

#include "wombat/port.h"

#include <mach/mach.h>
#include <mach/mach_time.h>
#include <mach/clock.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/errno.h>

#define CHECK_MACH_INITIALISED()                        \
do {                                                    \
    if (!clock_gettime_initialised)                     \
    {                                                   \
        if(GETTIME_SUCCESS != clock_gettime_init ())    \
            return GETTIME_FAIL;                        \
    }                                                   \
}                                                       \
while (0)                                               \

#define NANOS_IN_SEC     1000000000L

// Private function declarations
static int clock_gettime_init ();
static int clock_gettime_internal ();
static int clock_gettime_process_cputime_id (struct timespec * ts);

// Timer variables
static mach_timebase_info_data_t    timebase_info;
static uint64_t                     init_clock;         // ticks boot   --> initialised
static uint8_t                      clock_gettime_initialised = 0;

int clock_gettime (int type, struct timespec * ts)
{
    switch (type)
    {
        case CLOCK_MONOTONIC:
            return clock_gettime_internal(ts, SYSTEM_CLOCK);
            break;
        case CLOCK_REALTIME:
            return clock_gettime_internal(ts, CALENDAR_CLOCK);
            break;
        case CLOCK_PROCESS_CPUTIME_ID:
            return clock_gettime_process_cputime_id (ts);
            break;
        default:
            errno = EINVAL;
            return GETTIME_FAIL;
    }
}

int clock_gettime_init()
{

    // Initialise the timebase
    if (0 != mach_timebase_info (&timebase_info))
    {
        return GETTIME_FAIL;
    }

    // Ticks since boot
    init_clock = mach_absolute_time ();

    // Only do once
    clock_gettime_initialised = 1;

    return GETTIME_SUCCESS;
}

int clock_gettime_internal(struct timespec * ts, int type)
{
    clock_serv_t cclock;
    mach_timespec_t mts;
    kern_return_t retval;

    // Get the time
    host_get_clock_service(mach_host_self(), type, &cclock);
    retval = clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);

    ts->tv_sec = mts.tv_sec;
    ts->tv_nsec = mts.tv_nsec;

    return (retval == KERN_SUCCESS) ? GETTIME_SUCCESS : GETTIME_FAIL;
}

int clock_gettime_process_cputime_id (struct timespec * ts)
{
    CHECK_MACH_INITIALISED ();

    // Get the time
    uint64_t time;      // ticks since mach initialised --> now
    time = mach_absolute_time () - init_clock;

    // Convert to timespec
    uint64_t nanos;     // nanos since mach initialised --> now
    nanos = time * (uint64_t)timebase_info.numer / (uint64_t)timebase_info.denom;

    ts->tv_sec = nanos / NANOS_IN_SEC;
    ts->tv_nsec = nanos % NANOS_IN_SEC;

    return GETTIME_SUCCESS;
}

#endif /* MAC_OS_X_VERSION_MIN_REQUIRED */

