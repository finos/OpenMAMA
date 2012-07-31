/* $Id: timezone.c,v 1.10.4.1.16.5 2011/10/02 19:02:17 ianbell Exp $
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
#include "wombat/environment.h"
#include <mama/timezone.h>
#include <list.h>
#include <string.h>
#include <stdio.h>
#include <wombat/wincompat.h>
#include <time.h>
#include <assert.h>


#define MAX_TZ_LEN  64

typedef struct mamaTimeZoneImpl_
{
    char         mTz[MAX_TZ_LEN];
    mama_i32_t   mOffset;      /* seconds */
    long         mInstanceId;  /* unique identifier for object */
} mamaTimeZoneImpl;


static mamaTimeZone  sTzLocal   = NULL;
static mamaTimeZone  sTzUtc     = NULL;
static mamaTimeZone  sTzEastern = NULL;

mamaTimeZone mamaTimeZone_local()
{
    if (!sTzLocal)
    {
        mamaTimeZone_createFromTz (&sTzLocal, NULL);
    }
    return sTzLocal;
}

mamaTimeZone mamaTimeZone_utc()
{
    if (!sTzUtc)
    {
        mamaTimeZone_createFromTz (&sTzUtc, "UTC");
    }
    return sTzUtc;
}

mamaTimeZone mamaTimeZone_usEastern()
{
    if (!sTzEastern)
    {
        mamaTimeZone_createFromTz (&sTzEastern, "US/Eastern");
    }
    return sTzEastern;
}

static mama_f64_t sScanningThreadInterval = 1800.37;  /* approx 30 minutes */
static long       sInstanceId = 0;
static long       getNextId(void) { return ++sInstanceId; }


/* Used to check whether the instance scanning thread has been started. */
static int sThreadStarted = 0;

/* Mutex used in the check() method. */
static wthread_static_mutex_t sCheck_mutex  = WSTATIC_MUTEX_INITIALIZER;



/* Mutex used when accessing the vector. */
static wthread_static_mutex_t sVector_mutex = WSTATIC_MUTEX_INITIALIZER;


/*
* Global function which maintains all the instances
* of the MamaTimeZone class in its internal static vector
*
* Has to be global because when instances of the C++ MamaTimeZone
* class are declared globally, compilation order of the files (on GCC
* 3.2) determined whether the static vector was actually instantiated
* before the global MamaTimeZone instance was created, resulting in
* none of the global instances ending up in the vector at runtime!
*
* Maintains pointers to all mamaTimeZone instances ever created.
*/
static wList getTimeZones(void)
{
    static wList sTimeZones = NULL;
    if (!sTimeZones)
    {
        sTimeZones = list_create (sizeof(mamaTimeZoneImpl));
        assert (sTimeZones != INVALID_LIST);
    }
    return sTimeZones;
}

static void  startThread(void);
static void* updateTimeZones (void* ptr);


mama_status
mamaTimeZone_create (mamaTimeZone*  timeZone)
{
    mamaTimeZoneImpl* impl;
    wList  tzList;

    wthread_static_mutex_lock (&sVector_mutex);

    tzList = getTimeZones();
    impl = list_allocate_element (tzList);
    if (!impl)
    {
        wthread_static_mutex_unlock (&sVector_mutex);
        return MAMA_STATUS_NOMEM;
    }
    else
    {
        impl->mInstanceId = getNextId();
        list_push_back (tzList, impl);
        if (!sThreadStarted)
            startThread();

        wthread_static_mutex_unlock (&sVector_mutex);

        *timeZone = (mamaTimeZone)impl;
        return MAMA_STATUS_OK;
    }
}

mama_status
mamaTimeZone_createFromTz (mamaTimeZone*  timeZone,
                           const char*    tzId)
{
    mama_status status = mamaTimeZone_create (timeZone);
    if (MAMA_STATUS_OK == status)
    {
        status = mamaTimeZone_set (*timeZone, tzId);
    }
    return status;
}

mama_status
mamaTimeZone_createCopy (mamaTimeZone*       timeZone,
                         const mamaTimeZone  timeZoneCopy)
{
    if (!timeZoneCopy)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    else
    {
        mama_status status = mamaTimeZone_create (timeZone);
        if (MAMA_STATUS_OK == status)
        {
            status = mamaTimeZone_copy (*timeZone, timeZoneCopy);
        }
        return status;
    }
}

mama_status
mamaTimeZone_destroy (mamaTimeZone  timeZone)
{
    if (!timeZone)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    else
    {
        /* Remove from the list and free the memory. */
        mamaTimeZoneImpl* impl = (mamaTimeZoneImpl*)timeZone;
        wList tzList;
        wthread_static_mutex_lock (&sVector_mutex);
        tzList = getTimeZones();
        list_remove_element (tzList, impl);
        list_free_element (tzList, impl);
        wthread_static_mutex_unlock (&sVector_mutex);
        return MAMA_STATUS_OK;
    }
}

mama_status
mamaTimeZone_copy (mamaTimeZone        timeZone,
                   const mamaTimeZone  timeZoneCopy)
{
    if (!timeZone || !timeZoneCopy)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    else
    {
        mamaTimeZoneImpl* impl     = (mamaTimeZoneImpl*) timeZone;
        mamaTimeZoneImpl* implCopy = (mamaTimeZoneImpl*) timeZoneCopy;
        strcpy (impl->mTz, implCopy->mTz);
        impl->mOffset = implCopy->mOffset;
        return MAMA_STATUS_OK;
    }
}

mama_status
mamaTimeZone_set (mamaTimeZone   timeZone,
                  const char*    tzId)
{
    mama_status status = mamaTimeZone_clear(timeZone);
    if (MAMA_STATUS_OK == status)
    {
        if (tzId && tzId[0])
        {
            mamaTimeZoneImpl* impl = (mamaTimeZoneImpl*)timeZone;
            snprintf (impl->mTz, (MAX_TZ_LEN - 1), "%s", tzId);
        }
        status = mamaTimeZone_check(timeZone);
    }
    return status;
}

mama_status
mamaTimeZone_clear (mamaTimeZone   timeZone)
{
    if (!timeZone)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    else
    {
        mamaTimeZoneImpl* impl = (mamaTimeZoneImpl*)timeZone;
        memset(impl->mTz, 0, MAX_TZ_LEN);
        impl->mOffset = 0;
        return MAMA_STATUS_OK;
    }
}

mama_status
mamaTimeZone_getTz (const mamaTimeZone  timeZone,
                    const char**        result)
{
    if (!timeZone)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    else
    {
        mamaTimeZoneImpl* impl = (mamaTimeZoneImpl*)timeZone;

        if (!impl->mTz[0])
        {
            *result = *tzname;
        }
        else
        {
            *result = impl->mTz;
        }
        return MAMA_STATUS_OK;
    }
}

mama_status
mamaTimeZone_getOffset (const mamaTimeZone  timeZone,
                        mama_i32_t*         result)
{
    if (!timeZone)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    else
    {
        mamaTimeZoneImpl* impl = (mamaTimeZoneImpl*)timeZone;
        *result = impl->mOffset;
        return MAMA_STATUS_OK;
    }
}

mama_status
mamaTimeZone_check (mamaTimeZone  timeZone)
{
    if (!timeZone)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    else
    {
        mamaTimeZoneImpl* impl = (mamaTimeZoneImpl*)timeZone;
        const char* tzSaved = NULL;
        time_t      tzClock;
        time_t      gmClock;
        struct tm   tzTm;

        /* get the mutex for the accessing thread stops other threads
         * from calling this method at the same time as the TZ update
         * thread. */
        wthread_static_mutex_lock (&sCheck_mutex);

        /* Save the current value of TZ: */
        tzSaved = environment_getVariable("TZ");
        if (tzSaved && tzSaved[0])
        {
            tzSaved = strdup(tzSaved);
        }

        /* Switch TZ to the mTz zone: */
        if (impl->mTz[0])
        {
            environment_setVariable("TZ", impl->mTz);
        }
        else
        {
            environment_deleteVariable("TZ");
        }

        tzset();

        /* Get the mTz time: */
        time (&tzClock);
        localtime_r (&tzClock, &tzTm);

        /* Switch TZ to the UTC: */
        environment_setVariable("TZ", "UTC");
        tzset();

        /* Get the GMT time: */
        gmClock = mktime(&tzTm);

        /* Restore original TZ */
        if (tzSaved && tzSaved[0])
        {
            environment_setVariable("TZ", tzSaved);
            free ((void*)tzSaved);
        }
        else
        {
            environment_deleteVariable("TZ");
        }

        tzset();

        impl->mOffset = difftime (gmClock, tzClock);

        /* release the mutex on this method */
        wthread_static_mutex_unlock (&sCheck_mutex);

        return MAMA_STATUS_OK;
    }
}

void
mamaTimeZone_setScanningInterval (mama_f64_t  seconds)
{
    sScanningThreadInterval = seconds;
}

/* Utility file scoped method used to start the single instance
 * monitoring thread.
 *
 * NB! At the moment this is not thread safe.  However all instances
 * are currently being created on a single thread.  Revisit
 * implementation if this changes. */
static void startThread(void)
{
    /* start the thread scanning the vector of TimeZones. */
    int                 status = 0;
    static wthread_t    timeThread;

    /* Start the thread scanning the vector of TimeZones. */
     sThreadStarted = 1;

    if ((status = wthread_create(&timeThread, NULL, updateTimeZones, NULL) != 0))
    {
        printf ("TZ debug: failed to successfully create thread: "
                "%d\n", status);
	    sThreadStarted = 0;
    }
}

static void
checkTzIter (wList list, void* element, void* closure)
{
    mamaTimeZone tz = (mamaTimeZone)element;
    if (tz != NULL)
    {
        mamaTimeZone_check(tz);
    }
}

static void* updateTimeZones (void* ptr)
{

    struct wtimespec delay;
    struct wtimespec initialDelay;
    wList           timeZones;

    initialDelay.tv_sec = 10;
    initialDelay.tv_nsec = 0;
    wnanosleep (&initialDelay, NULL);


    /* It makes sense for this thread to continue running for the life
     * of the process. No need to check for stop condition. */
    while (1)
    {
        delay.tv_sec  = sScanningThreadInterval;
        delay.tv_nsec = 100;

        /* If the scanning thread interval is not set, then sleep for
         * a min and go again. This allows this to be turned off and
         * turned back on using a wadmin command. */
        if (!sScanningThreadInterval)
        {
            delay.tv_sec = 60;
            delay.tv_nsec = 100;
            wnanosleep(&delay,NULL);

            continue;
        }
        wnanosleep(&delay,NULL);

        /* Lock access to the list of timezones while we recheck. */
        wthread_static_mutex_lock (&sVector_mutex);

        timeZones = getTimeZones();

        list_for_each (timeZones, checkTzIter, NULL);

        wthread_static_mutex_unlock (&sVector_mutex);
    }

    /* The return value is not applicable. */
    return NULL;
}
