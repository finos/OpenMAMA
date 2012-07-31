/* $Id: destroyHandle.h,v 1.1.2.1.4.3 2011/08/10 14:53:23 nicholasmarriott Exp $
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

#ifndef DESTROYHANDLE_H
#define DESTROYHANDLE_H

/* The destroy handle is used where there are multiple threads using a single
 * resource which is needed until all threads have ended. The problem with this
 * scenario is knowing when to destroy the object. If the owner thread deletes
 * the object then other threads may in-advertantly access it despite the memory
 * being freed.
 * A destroy handle contains a reference to the object and a count of the number
 * of outstanding threads. The destroy handle will only be destroyed once the object
 * itself has been destroyed and all threads have finished using it. This means that
 * although the object may be gone the thread will have valid memory to access.
 */

#if defined(__cplusplus)
extern "C" {
#endif
/* ************************************************************** */
/* Includes. */
/* ************************************************************** */
#include <stdlib.h>
#include "wlock.h"

/* ************************************************************** */
/* Structures. */
/* ************************************************************** */

/* This structure is used to track the number of open references to the owner
 * object.
 */
typedef struct wDestroyHandle
{
    /* This flag indicates if the owner has been destroyed. */
    int m_destroyed;

    /* The lock protects access to this structure. */
    wLock m_lock;

    /* This is the number of references that are held to the owner. */
    long m_numberReferences;

    /* A reference to the owner object, this will point to invalid
	 * memory if m_destroyed is set.
	 */
    void *m_owner;

} wDestroyHandle, * pDestroyHandle;

/* ********************************************************** */
/* Function Prototypes. */
/* ********************************************************** */

/**
 * This function will create a new destroy handle and save a reference
 * to the owner.
 *
 * @param[in] owner The owner object.
 * @return The destroy handle or NULL on failure.
 */
COMMONExpDLL
extern
pDestroyHandle
destroyHandle_create(
	void *owner);

/**
 * This function must be called whenever the owner is destroyed. Note that the
 * memory for the destroy handle won't be released until the owner is destroyed
 * and the reference count reaches zero.
 *
 * @param[in] handle The destroy handle.
 */
COMMONExpDLL
extern
void
destroyHandle_destroyOwner(
	pDestroyHandle handle);

/**
 * This function must be called each time a reference to the destroy handle is passed
 * to another thread. This should be matched by a corresponding call to
 * destroyHandle_removeReference when the thread no longer needs the destroy handle.
 *
 * @param[in] handle The destroy handle.
 */
COMMONExpDLL
extern
void
destroyHandle_incrementRefCount(
	pDestroyHandle handle);

/**
 * This function must be called whenever a thread has finished using the destroy handle.
 * It will return a reference to the owner object and will decrement the reference count.
 * If the count has reached zero and the owner object has been destroyed then the destroy
 * handle will be deleted.
 *
 * @param[in] handle The destroy handle.
 * @return The owner object or NULL will be returned if the owner has been destroyed.
 */
COMMONExpDLL
extern
void *
destroyHandle_removeReference(
	pDestroyHandle handle);
#if defined(__cplusplus)
}
#endif

#endif
