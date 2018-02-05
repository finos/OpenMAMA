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

#ifndef _WOMBAT_THREAD_H

#include "wombat/port.h"
#include "wombat/wConfig.h"

#define _WOMBAT_THREAD_H

#if defined(__cplusplus)
extern "C" {
#endif

typedef void* wombatThread;

/* Used for status results. WOMBAT_THREAD_OK indicates successful function call. */
typedef enum
{
    WOMBAT_THREAD_OK                = 0,    /* Success. */
    WOMBAT_THREAD_INVALID_ARG       = 1,    /* An argument is invalid. */
    WOMBAT_THREAD_NOMEM             = 2,    /* No memory available for allocation. */
    WOMBAT_THREAD_IN_USE            = 3,    /* Thread has already been assigned. */
    WOMBAT_THREAD_CANNOT_CREATE     = 4,    /* Unable to create an OS process. */
    WOMBAT_THREAD_PROPERTY          = 5     /* Unable to assign thread property. */
} wombatThreadStatus;

/* Create an OS thread and associate it with a wombat thread.
 *
 * PARAMS:
 *  @threadName     The name associated with the thread. There can only ever be
 *                  one thread created with with this name.
 *  @wombatThread   [OUT] On success, this is set to the corresponding wombat
 *                  thread object.  You must never destroy this object.
 *  @attr           See the thread create method for the underlying OS.
 *  @startRoutine   See the thread create method for the underlying OS.
 *  @closure        See the thread create method for the underlying OS.
 *  @return         WOMBAT_THREAD_OK on success.
 *
 * If the "threadName" already corresponds to an OS thread, a new OS thread will
 * _NOT_ be created.
 */
COMMONExpDLL
wombatThreadStatus
wombatThread_create(
    const char*             threadName,
    wombatThread*           wombatThread,
    const wthread_attr_t*   attr,
    void*                   (*startRoutine)(void*),
    void*                   closure);

/* Remove a Named Thread from the appropriate data structures
 *
 * PARAMS:
 *  @name   The name associated with the Thread
 *
 * If the "name" corresponds to a Named Thread then it will be removed from the Thread
 * Dicitonary. WOMBAT_THREAD_OK will be returned.
 *
 * If the "name" does not correspond to a Named Thread, nothing will be removed from any
 * data structures and WOMBAT_THREAD_INVALID_ARG
 *
 */
COMMONExpDLL
wombatThreadStatus
wombatThread_destroy(
    const char* name);

/* Return the underlying OS thread object corresponding to the wombatThread object.
 *
 * @thread      The wombat thread.
 * @return      The OS thread handle of the 'no-thread' value is 'thread' is NULL.
 */
COMMONExpDLL
wthread_t
wombatThread_getOsThread(wombatThread thread);


/* Return the defined name corresponding to the wombatThread object.
 *
 * @thread      The wombat thread.
 * @return      The thread name or NULL if 'thread' is NULL.
 */
COMMONExpDLL
const char*
wombatThread_getThreadName(wombatThread thread);


/* Return the defined CPU affinity corresponding to the wombat thread.
 *
 * @thread      The wombat thread.
 * @affinity    [OUT] The affinity set for the thread.
 *
 * The result set will be set to all zero bits if 'thread' is NULL.
 * The result set for a valid 'thread' is the CPU affinity bitset that was
 * defined for the thread via wombatThread_setAffinity. No attempt is made
 * to determine the actual CPU affinity that is associated with the real OS
 * thread object, if any.
 */
COMMONExpDLL
wombatThreadStatus
wombatThread_getDefinedThreadAffinity(wombatThread thread, CPU_AFFINITY_SET* affinity);

/* Associate a CPU affinithy with a wombat thread.
 * If no OS thread corresponding to the named wombat thread exists yet, the
 * affinity is captured to be applied when the wombat thread is created or
 * registered in the future. Otherwise, the affinity is applied immediately.
 * You can call this function repeatedly to reapply a CPU affinity to a
 * wombat thread.
 *
 * @threadName      The name associated with the thread.
 * @affinity        The native OS affinity set to be applied.
 * @return          WOMBAT_THREAD_OK on success.
 */
COMMONExpDLL
wombatThreadStatus
wombatThread_setAffinity(const char* threadName, CPU_AFFINITY_SET* affinity);

#if defined(__cplusplus)
}
#endif

#endif /* _WOMBAT_THREAD_H */
