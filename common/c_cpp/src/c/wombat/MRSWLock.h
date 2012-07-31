/* $Id: MRSWLock.h,v 1.1.2.2 2011/08/10 14:53:24 nicholasmarriott Exp $
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

#ifndef MRSWLOCK_H
#define MRSWLOCK_H

/* ********************************************************** */
/* Includes. */
/* ********************************************************** */
#include "wlock.h"
#include "wombat/wSemaphore.h"

/* ********************************************************** */
/* Type Defines. */
/* ********************************************************** */

/* Defines the return code used in this file. */
typedef int MRSW_RESULT;
#define MRSW_S_OK 0					/* Function succeeded. */
#define MRSW_E_INVALIDARG 1			/* Invalid arguments. */
#define MRSW_E_OUTOFMEMORY 2		/* No more memory. */
#define MRSW_E_SYSTEM 3				/* System error. */

/* ********************************************************** */
/* Structures. */
/* ********************************************************** */

/* This structure contains all the variables needed to implement the lock. */
typedef struct MRSWLock
{
	/* The number of reader locks currently being held. */
	long m_numberReaders;

	/* The number of times the current thread has re-acquire the write lock. */
	long m_numberWriters;

	/* This event is signaled when there are no locks. Note that a semaphore
	 * with a count of 0 is used as we don't have a cross platform event.
	 */
	wsem_t m_noLocksEvent;

	/* Mutex protects access to this structure. */
	wLock m_mutex;	 

	/* The Id of the thread that holds the write lock. */
	long m_writeThreadId;
   
} MRSWLock, * PMRSWLock;

/* ********************************************************** */
/* Function Prototypes. */
/* ********************************************************** */

/**
 * This function will acquire the lock. This function must be accompanied by a call to
 * MRSWLock_release.
 * @param lock (in) The lock created by a called to MRSWLock_create.
 * @param read (in) Non-zero to acquire a read lock, 0 to acquire a write lock.
 * @return one of the MRSW_RESULT codes.
 */
COMMONExpDLL
extern MRSW_RESULT 
MRSWLock_acquire(
	PMRSWLock lock, 
	int read);

/**
 * This function will create a new multiple reader / single writer lock.
 * @param lock (out) To return the lock object, free this by calling MRSW_release.
 * @return one of the MRSW_RESULT codes.
 */
COMMONExpDLL
extern MRSW_RESULT 
MRSWLock_create(
	PMRSWLock *lock);

/**
 * This function will downgrade a writer lock to a reader lock.
 * @param lock (in) The lock created by a called to MRSWLock_create.
 * @return one of the MRSW_RESULT codes.
 */
COMMONExpDLL
extern MRSW_RESULT 
MRSWLock_downgrade(
	PMRSWLock lock);

/**
 * This function will free the lock object.
 * @param lock (in) The lock created by a called to MRSWLock_create.
 * @return one of the MRSW_RESULT codes.
 */
COMMONExpDLL
extern MRSW_RESULT 
MRSWLock_free(
	PMRSWLock lock);

/**
 * This function will return the Id of the thread that currently holds the
 * write lock.
 * @param lock (in) The lock created by a called to MRSWLock_create.
 * @return The write thread Id or 0 if no thread holds the write lock.
 */
COMMONExpDLL
extern long 
MRSWLock_getWriteThreadId(
	PMRSWLock lock);

/**
 * This function will releae the lock.
 * @param lock (in) The lock created by a called to MRSWLock_create.
 * @param read (in) Non-zero to release a read lock, 0 to release a write lock.
 * @return one of the MRSW_RESULT codes.
 */
COMMONExpDLL
extern MRSW_RESULT 
MRSWLock_release(
	PMRSWLock lock, 
	int read);

/**
 * This function will upgrade a reader lock to a writer lock. Note that this function
 * will only work if the current thread only holds 1 instance of the reader lock.
 * @param lock (in) The lock created by a called to MRSWLock_create.
 * @return one of the MRSW_RESULT codes.
 */
COMMONExpDLL
extern MRSW_RESULT 
MRSWLock_upgrade(
	PMRSWLock lock);

#endif
