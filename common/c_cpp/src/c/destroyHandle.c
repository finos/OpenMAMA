/* $Id: destroyHandle.c,v 1.1.2.3.4.2 2011/08/10 14:53:23 nicholasmarriott Exp $
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

/* ************************************************************** */
/* Includes. */
/* ************************************************************** */
#include "wombat/port.h"
#include "destroyHandle.h"
#include <stdio.h>

/* ************************************************************** */
/* Private Function Prototypes. */
/* ************************************************************** */

/* ************************************************************** */
/* Private Functions. */
/* ************************************************************** */
/**
 * Will free memory associated with the destroy handle.
 *
 * @param[in] handle The destroy handle.
 */
void destroyHandle_freeHandle(pDestroyHandle handle)
{
	if(NULL != handle)
	{
		/* Destroy the lock. */
		wlock_destroy(handle->m_lock);

		/* Free the structure. */
		free(handle);
	}
}

/* ************************************************************** */
/* Public Functions. */
/* ************************************************************** */

pDestroyHandle destroyHandle_create(void *owner)
{
    /* Returns. */
    pDestroyHandle ret = (pDestroyHandle)calloc(1, sizeof(wDestroyHandle));
    if(NULL != ret)
    {
        /* Create the lock. */
        ret->m_lock = wlock_create();

        /* Save the owner object. */
        ret->m_owner = owner;
    }

    return ret;
}

void destroyHandle_destroyOwner(pDestroyHandle handle)
{
	if(NULL != handle)
	{
		/* Acquire the lock on. */
		wlock_lock(handle->m_lock);

		/* If there are no references held then the object can be deleted. */
		if(0 == handle->m_numberReferences)
		{
			destroyHandle_freeHandle(handle);
		}

		else
		{
			/* Flag that the handle is being destroyed, the memory will then be
			 * deleted the final time that removeReference is called.
			 */
			handle->m_destroyed = 1;

			/* Release the lock. */
			wlock_unlock(handle->m_lock);
		}
	}
}

void destroyHandle_incrementRefCount(pDestroyHandle handle)
{
	if(NULL != handle)
	{
		/* Acquire the lock on the response object. */
		wlock_lock(handle->m_lock);

		/* Increment the number of references. */
		handle->m_numberReferences ++;

		/* Unlock. */
		wlock_unlock( handle->m_lock);
	}
}

void *destroyHandle_removeReference(pDestroyHandle handle)
{
	/* Returns. */
	void *ret = NULL;
	if(NULL != handle)
	{
		/* Acquire the lock. */
		wlock_lock(handle->m_lock);

		/* Decrement the number of outstanding references. */
		handle->m_numberReferences --;

        /* A refernce to the objcet will only be returned if it has not been destroyed. */
        if(0 == handle->m_destroyed)
		{

			/* Alternatively return a reference to the object. */
			ret = handle->m_owner;

			/* Release the lock. */
			wlock_unlock(handle->m_lock);
		}
        /* If there are no references AND the owner has been destroyed then the destroy handle must be freed, else do nothing. */
        else if(0 == handle->m_numberReferences)
		{
			destroyHandle_freeHandle(handle);
		}
	}

	return ret;
}
