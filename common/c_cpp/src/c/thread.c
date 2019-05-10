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

#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "wombat/thread.h"
#include "wombat/wtable.h"

/* *************************************************** */
/* Definitions. */
/* *************************************************** */

/* Default wtable_t size for thread name => wombatThreadImpl map. */
#define DEFAULT_WTABLE_SIZE 8

/* *************************************************** */
/* Structures. */
/* *************************************************** */

typedef struct wombatThread_
{
    wthread_t               mThread;        /* Thread object (may not be defined yet). */
    char*                   mThreadName;    /* Thread name. */
    int                     mIsCpuSet;      /* Has affinity been set? */
    CPU_AFFINITY_SET        mCpuSet;        /* Affinity as defined (not necessarily as exists) */
} wombatThreadImpl;

/* *************************************************** */
/* Global Variables. */
/* *************************************************** */

static wtable_t               gWombatThreadDict;      /* Named threads. */
static wthread_static_mutex_t gWombatThreadsMutex = WSTATIC_MUTEX_INITIALIZER; /* Protects gWombatThreadDict. */

/* *************************************************** */
/* Private Function Prototypes. */
/* *************************************************** */
static wombatThreadStatus allocateWombatThreadDict (void);
static wombatThreadStatus getThreadImpl     (const char* threadName, wombatThreadImpl** result);

/* *************************************************** */
/* Public Functions. */
/* *************************************************** */

wombatThreadStatus
wombatThread_create(
    const char*             threadName,
    wombatThread*           wombatThread,
    const wthread_attr_t*   attr,
    void*                   (*startRoutine)(void*),
    void*                   closure)
{
    wombatThreadImpl*       result;
    wombatThreadStatus      status;

    if (!threadName || !wombatThread) return WOMBAT_THREAD_INVALID_ARG;
   
    status = allocateWombatThreadDict();
    if (status != WOMBAT_THREAD_OK) return status;

    wthread_static_mutex_lock(&gWombatThreadsMutex);

    status = getThreadImpl(threadName, &result);
    if (status != WOMBAT_THREAD_OK)
    {
        wthread_static_mutex_unlock(&gWombatThreadsMutex);
        return status;
    }

    *wombatThread = result;

    wthread_static_mutex_unlock(&gWombatThreadsMutex);

    if (wthread_create(&result->mThread, attr, startRoutine, closure))
    {
        return WOMBAT_THREAD_CANNOT_CREATE;
    }

    if (result->mIsCpuSet)
    {
        if (wthread_set_affinity_mask (result->mThread, sizeof(CPU_AFFINITY_SET), &result->mCpuSet))
        {
            return WOMBAT_THREAD_PROPERTY;
        }
    }

    return WOMBAT_THREAD_OK;
}

wombatThreadStatus
wombatThread_destroy (const char* name)
{
    wombatThreadImpl* impl = NULL;

    if (!name) return WOMBAT_THREAD_INVALID_ARG;

    wthread_static_mutex_lock(&gWombatThreadsMutex);
    
    impl = wtable_lookup (gWombatThreadDict, name);

    /* Remove from Thread Dictionary */
    wtable_remove (gWombatThreadDict, name);

    if (wtable_get_count(gWombatThreadDict) == 0)
    {
        wtable_destroy(gWombatThreadDict);
        gWombatThreadDict = NULL;
    }

    wthread_static_mutex_unlock(&gWombatThreadsMutex);

    // Attempt to join the thread to ensure it has terminated
    wthread_join (impl->mThread, NULL);

    if (impl)
    {
        free (impl->mThreadName);
    }
    free(impl);

    return WOMBAT_THREAD_OK;
}

wthread_t
wombatThread_getOsThread(wombatThread thread)
{
    wombatThreadImpl*   impl = (wombatThreadImpl*) thread;

    if (!thread) return 0;
    return impl->mThread;
}

const char*
wombatThread_getThreadName(wombatThread thread)
{
    wombatThreadImpl*   impl = (wombatThreadImpl*) thread;

    if (!thread) return NULL;
    return impl->mThreadName;
}

wombatThreadStatus
wombatThread_getDefinedThreadAffinity(wombatThread thread, CPU_AFFINITY_SET* affinity)
{
    int                 i;
    wombatThreadImpl*   impl = (wombatThreadImpl*) thread;

    if (!thread || !affinity) return WOMBAT_THREAD_INVALID_ARG;

    for (i = 0; i < sizeof(CPU_AFFINITY_SET); i++)
    {
        ((char*) affinity)[i] = ((char*) &(impl->mCpuSet))[i];
    }
    return WOMBAT_THREAD_OK;
}

wombatThreadStatus
wombatThread_setAffinity(const char* threadName, CPU_AFFINITY_SET* affinity)
{
    int                 i;
    wombatThreadImpl*   thread;
    wombatThreadStatus  status;

    if (!threadName || !affinity) return WOMBAT_THREAD_INVALID_ARG;

    status = allocateWombatThreadDict();
    if (status != WOMBAT_THREAD_OK) return status;

    status = getThreadImpl(threadName, &thread);
    if ((status != WOMBAT_THREAD_OK) &&
        (status != WOMBAT_THREAD_IN_USE)) return status;

    for (i = 0; i < sizeof(CPU_AFFINITY_SET); i++)
    {   /* Copy the affinity to the table entry. */
        ((char*) &(thread->mCpuSet))[i] = ((char*) affinity)[i];
    }

    if (thread->mThread)
    {
        if (wthread_set_affinity_mask (thread->mThread, sizeof (CPU_AFFINITY_SET), &thread->mCpuSet))
        {
            return WOMBAT_THREAD_PROPERTY;
        }
    }

    thread->mIsCpuSet = 1;
    return WOMBAT_THREAD_OK;
}

/* *************************************************** */
/* Private Functions. */
/* *************************************************** */

/* Create initial state. */
static wombatThreadStatus
allocateWombatThreadDict (void)
{
    wthread_static_mutex_lock(&gWombatThreadsMutex);

    if (!gWombatThreadDict)
    {
        gWombatThreadDict = wtable_create("gWombatThreadDict", (DEFAULT_WTABLE_SIZE));
        if (!gWombatThreadDict)
        {
            wthread_static_mutex_unlock(&gWombatThreadsMutex);
            return WOMBAT_THREAD_NOMEM;
        }
    }
    wthread_static_mutex_unlock(&gWombatThreadsMutex);

    return WOMBAT_THREAD_OK;
}

/* Find/create named wombat thread object. */
static
wombatThreadStatus
getThreadImpl (const char* threadName, wombatThreadImpl** result)
{
    wthread_static_mutex_lock(&gWombatThreadsMutex);

    *result = wtable_lookup(gWombatThreadDict, threadName);

    if (! *result)
    {
        /* There is no pre-allocated table entry for this named thread. */
        int i;
        *result = calloc(sizeof(wombatThreadImpl), 1);
        if (! *result)
        {
            wthread_static_mutex_unlock(&gWombatThreadsMutex);
            return WOMBAT_THREAD_NOMEM;
        }
        (*result)->mThreadName = strdup(threadName);
        if (! (*result)->mThreadName)
        {
            free(*result);
            wthread_static_mutex_unlock(&gWombatThreadsMutex);
            return WOMBAT_THREAD_NOMEM;
        }
        wtable_insert(gWombatThreadDict, threadName, *result);
        for (i = 0; i < sizeof(CPU_AFFINITY_SET); i++)
        {
            /* Allow all CPUs by default. */
            ((char*) &(*result)->mCpuSet)[i] = (char) 0xFF;
        }
    }
    else
    {
        /* The table entry has been preallocated for this named thread. */
        if ((*result)->mThread)
        {
            /* The named thread has been previously created. */
            wthread_static_mutex_unlock(&gWombatThreadsMutex);
            return WOMBAT_THREAD_IN_USE;
        }
    }
    wthread_static_mutex_unlock(&gWombatThreadsMutex);

    return WOMBAT_THREAD_OK;
}
