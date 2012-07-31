/* $Id: MRSWLock.c,v 1.1.4.4.8.3 2011/08/10 14:53:23 nicholasmarriott Exp $
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

/* ********************************************************** */
/* Includes. */
/* ********************************************************** */
#include "wombat/port.h"
#include "wombat/MRSWLock.h"
#include <stdlib.h>

/* ********************************************************** */
/* Functions. */
/* ********************************************************** */

MRSW_RESULT MRSWLock_acquire(PMRSWLock lock, int read)
{
	/* Returns. */
	MRSW_RESULT ret = MRSW_E_INVALIDARG;
	if(lock != NULL)
	{
		/* If a writer lock is required. */
		if(read == 0)
		{
			/* If the current thread already holds the write lock then there is no need to acquire it again.
			 * Note that waiting on the semaphore twice in the same thread causes a problem.
			 */
			long currentThreadId = wGetCurrentThreadId();
			long writeThreadId = MRSWLock_getWriteThreadId(lock);
			if(currentThreadId == writeThreadId)
			{
				/* Increment the number of times the current thread has the write lock. */
				/* Acquire the lock. */
				wlock_lock(lock->m_mutex);

				/* Increment the count. */
				lock->m_numberWriters ++;

				/* Release the lock. */
				wlock_unlock(lock->m_mutex);
			}

			/* Otherwise acquire the write lock as normal. */
			else
			{
				/* It is possible that the semaphore can be signaled, this thread released but another
				 * thread acquire a read lock before this thread can acquire the mutex below. Therefore
				 * we must keep trying to acquire the lock until there are definitely no readers.
				 * Cannot acquire the mutex before checking the semaphore as this will stop all active
				 * readers releasing the lock.
				 */
				int validWriteLock = 0;
				while(0 == validWriteLock)
				{
				/* Wait until the synchronization event is signaled to indicate that there are no locks. */
				wsem_wait(&lock->m_noLocksEvent);

				/* Acquire the mutex, this will now prevent any readers from coming in. */
				wlock_lock(lock->m_mutex);

					/* The write lock is valid only if there are currently no readers. */
					if(0 == lock->m_numberReaders)
					{
						/* This is a valid write lock. */
						validWriteLock = 1;
					}

					/* Otherwise release the mutex and try again. */
					else
					{
						wlock_unlock(lock->m_mutex);
					}
				}

				/* Save the Id of this thread as it now holds the write lock. */
				lock->m_writeThreadId = currentThreadId;
			}
		}

		/* Else it is a reader lock. */
		else
		{
			/* Acquire the mutex. */
			wlock_lock(lock->m_mutex);

			/* If the reader count is 0 then reset the event to ensure that no writer threads can be released. */
			if(lock->m_numberReaders == 0)
			{
				/* Calling timed wait with 0 will effectively reset the semaphore. */
				wsem_timedwait(&lock->m_noLocksEvent, 0);
			}

			/* Increment the count of readers. */
			lock->m_numberReaders ++;			

			/* Release the mutex. */
			wlock_unlock(lock->m_mutex);
		}

		/* The function has succeeded. */
		ret = MRSW_S_OK;
	}

	return ret;
}

MRSW_RESULT MRSWLock_create(PMRSWLock *lock)
{
	/* Returns. */
	MRSW_RESULT ret = MRSW_E_INVALIDARG;
	if(lock != NULL)
	{
		/* Allocate a new structure. */
		PMRSWLock rLock = (PMRSWLock)calloc(1, sizeof(MRSWLock));
		ret = MRSW_E_OUTOFMEMORY;
		if(rLock != NULL)
		{
			/* Create the synchronization event. */
			int wmi = wsem_init(&rLock->m_noLocksEvent, 0, 0);
			if(wmi == 0)
			{
				/* Create the mutex. */
				rLock->m_mutex = wlock_create();
				if(rLock->m_mutex != NULL)
				{
					/* The event must be initially signaled as there are no readers. */
					wsem_post(&rLock->m_noLocksEvent);

					/* The function has now succeeded. */
					ret = MRSW_S_OK;
				}
			}
		}

		/* If something went wrong then free the structure. */
		if(MRSW_S_OK != ret)
		{
			MRSWLock_free(rLock);
			rLock = NULL;
		}

		/* Write the pointer back. */
		*lock = rLock;
	}

	return ret;
}

MRSW_RESULT MRSWLock_downgrade(PMRSWLock lock)
{
	/* Returns. */
	MRSW_RESULT ret = MRSW_E_INVALIDARG;
	if(lock != NULL)
	{
		/* Release the writer lock currently held. */
		ret = MRSWLock_release(lock, 0);
		if(MRSW_S_OK == ret)
		{
			/* Acquire a new reader lock. */
			ret = MRSWLock_acquire(lock, 1);
		}
	}

	return ret;
}

MRSW_RESULT MRSWLock_free(PMRSWLock lock)
{
	/* Returns. */
	MRSW_RESULT ret = MRSW_E_INVALIDARG;
	if(lock != NULL)
	{
		/* Set the return code, note that errors will be preserved. */
		ret = MRSW_S_OK;

		/* Destroy the mutex. */		
		if(lock->m_mutex != NULL)
		{
			wlock_destroy(lock->m_mutex);
		}

		/* Destroy the semaphore. */
		if(&lock->m_noLocksEvent != NULL)
		{
			int wmd = wsem_destroy(&lock->m_noLocksEvent);
			if((wmd != 0) && (ret == MRSW_S_OK))
			{
				ret = MRSW_E_SYSTEM;
			}
		}

		/* Free the structure. */
		free(lock);
	}

	return ret;
}

long MRSWLock_getWriteThreadId(PMRSWLock lock)
{
	/* Returns. */
	long ret = 0;

	/* Acquire the lock. */
	wlock_lock(lock->m_mutex);

	/* Read the data from the lock structure. */
	ret = lock->m_writeThreadId;

	/* Release the lock. */
	wlock_unlock(lock->m_mutex);

	return ret;
}

MRSW_RESULT MRSWLock_release(PMRSWLock lock, int read)
{
	/* Returns. */
	MRSW_RESULT ret = MRSW_E_INVALIDARG;
	if(lock != NULL)
	{
		/* If a writer lock is required. */
		if(read == 0)
		{
			/* Acquire the mutex. */
			wlock_lock(lock->m_mutex);

			/* If the current thread has more than 1 writer lock then just decrement the count. */
			if(lock->m_numberWriters > 0)
			{
				lock->m_numberWriters --;
			}

			else
			{
				/* Otherwise release the write lock obtained in the acquire function. */
				wlock_unlock(lock->m_mutex);

				/* Reset the Id of the thread currently holding the lock. */
				lock->m_writeThreadId = 0;

				/* Signal the semaphore to allow a new write lock to be acquired. */
				wsem_post(&lock->m_noLocksEvent);
			}
			
			/* Release the mutex. */
			wlock_unlock(lock->m_mutex);
		}

		/* Else it is a reader lock. */
		else
		{
			/* Acquire the mutex. */
			wlock_lock(lock->m_mutex);

			/* Decrement the number of readers. */
			lock->m_numberReaders --;

			/* If the reader count is 0 then set the event to allow a writer lock. */
			if(lock->m_numberReaders == 0)
			{
				/* Signal the semaphore to allow a new write lock to be acquired. */
				wsem_post(&lock->m_noLocksEvent);
			}

			/* Release the mutex. */
			wlock_unlock(lock->m_mutex);
		}

		/* The function has succeeded. */
		ret = MRSW_S_OK;
	}

	return ret;
}

MRSW_RESULT MRSWLock_upgrade(PMRSWLock lock)
{
	/* Returns. */
	MRSW_RESULT ret = MRSW_E_INVALIDARG;
	if(lock != NULL)
	{
		/* Release the reader lock currently held. */
		ret = MRSWLock_release(lock, 1);
		if(MRSW_S_OK == ret)
		{
			/* Acquire a new writer lock. */
			ret = MRSWLock_acquire(lock, 0);
		}
	}

	return ret;
}
