/* $Id: queue.c,v 1.35.4.5.2.1.4.11 2011/09/27 12:38:50 ianbell Exp $
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
#include <mama/mama.h>
#include <mama/queue.h>
#include "bridge.h"
#include "queueimpl.h"
#include "msgimpl.h"
#include <mama/stat.h>
#include <mama/statscollector.h>
#include <mama/statfields.h>
#include "statsgeneratorinternal.h"
#include "statinternal.h"
#include "wlock.h"
#include "wombat/wInterlocked.h"
#include <wombat/strutils.h>

extern int gGenerateQueueStats;
extern int gLogQueueStats;
extern int gPublishQueueStats;

int MAMACALLTYPE mamaQueue_pollQueueSizeCb (void* closure);

/* *************************************************** */
/* Definitions. */
/* *************************************************** */

/* property to turn on object locking tracking. */
#define MAMAQUEUE_PROPERTY_OBJECT_LOCK_TRACKING "mama.queue.object_lock_tracking"

/* *************************************************** */
/* Structures. */
/* *************************************************** */

/* This structure is used to track queue lock counts when this feature is
 * turned on.
 */
typedef struct mamaQueueLock
{
    /* The owner object. */
    void *m_owner;

} mamaQueueLock, * pMamaQueueLock;

/*Main structure for the mamaQueue*/
typedef struct mamaQueueImpl_
{
    /*Reuseable message for all received on this queue*/
    mamaMsg                     mMsg;
    int                         mIsDispatching;
    /*Hold onto the bridge impl for later use*/
    mamaBridgeImpl*             mBridgeImpl;
    /*The bridge specific queue implementation*/
    queueBridge                 mMamaQueueBridgeImpl;
    /*The dispatcher dispatching on this queue - there can be only one!*/
    mamaDispatcher              mDispatcher;
    mamaQueueMonitorCallbacks   mQueueMonitorCallbacks;
    void*                       mQueueMonitorClosure;
    const char*                 mQueueName;
    size_t                      mLowWatermark;
    size_t                      mHighWatermark;
    int                         mIsDefaultQueue;
    mamaStatsCollector*         mStatsCollector;
    mamaStat                    mInitialStat;
    mamaStat                    mRecapStat;
    mamaStat                    mUnknownMsgStat;
    mamaStat                    mMessageStat;
    mamaStat                    mQueueSizeStat;
    mamaStat                    mSubscriptionStat;
    mamaStat                    mTimeoutStat;
    mamaStat                    mWombatMsgsStat;
    mamaStat                    mFastMsgsStat;
    mamaStat                    mRvMsgsStat;

    /* The queue cannot be destroyed until all of the objects, (timer, subscription etc),
     * are destroyed. This counter keeps track of the number of open events in a thread
     * safe manner.
     */
    wInterlockedInt             mNumberOpenObjects;

    /* This flag indicates whether object locking and unlocking will be tracked by the queue. */
    int                         mTrackObjectLocks;
    void*                       mClosure;
} mamaQueueImpl;

/*Main structure for the mamaDispatcher*/
typedef struct mamaDisptacherImpl_
{
    /*The queue on which this dispatcher is dispatching*/
    mamaQueue      mQueue;
    /*The thread on which this dispatcher is dispathcing.*/
    wthread_t      mThread;
    /*Whether the dispatcher is dispatching*/
    int            mIsDispatching;
    /*Destroy has been called*/
    int            mDestroy;
} mamaDispatcherImpl;


mama_status
mamaQueue_setClosure ( mamaQueue queue, void* closure)
{
    mamaQueueImpl* impl = (mamaQueueImpl*)queue;
    if (!impl) return MAMA_STATUS_NULL_ARG;

    impl->mClosure = closure;

    return MAMA_STATUS_OK;
}

mama_status
mamaQueue_getClosure ( mamaQueue queue, void** closure)
{
    mamaQueueImpl* impl = (mamaQueueImpl*)queue;
    if (!impl) return MAMA_STATUS_NULL_ARG;
    if (!closure) return MAMA_STATUS_INVALID_ARG;

    *closure = impl->mClosure;

    return MAMA_STATUS_OK;
}


mama_status
mamaQueue_createReuseableMsg (mamaQueueImpl*  impl)
{
	 mama_status     status      =   MAMA_STATUS_OK;
	/*Create the reuseable cached mamaMsg for this queue*/
	if (MAMA_STATUS_OK != (status = mamaMsgImpl_createForPayload (&(impl->mMsg), NULL,NULL,0)))
	{
		mama_log (MAMA_LOG_LEVEL_ERROR, "mamaQueue_create(): "
				  "Could not create message for queue.");
		mamaQueue_destroy ((mamaQueue)impl);
		return status;
	}

	/*Set the bridge impl structure on the new message.
	  It will be needed for any bridge specific operations.*/
	if (MAMA_STATUS_OK!=(status=mamaMsgImpl_setBridgeImpl (impl->mMsg,
							impl->mBridgeImpl)))
	{
		mama_log (MAMA_LOG_LEVEL_ERROR, "mamaQueue_create(): "
				  "Could not set bridge impl on new message.");
		mamaQueue_destroy ((mamaQueue)impl);
		return status;
	}

	/*We don't want to destroy the bridge message when the mamamsg is
	 * destroyed as we won't own it*/
	if (MAMA_STATUS_OK!=(status=mamaMsgImpl_setMessageOwner (impl->mMsg, 0)))
	{
		mama_log (MAMA_LOG_LEVEL_ERROR, "mamaQueue_create():"
				  " Could not set owner on new message.");
		return status;
	}

	/*Associate the queue with the message*/
	if (MAMA_STATUS_OK!=(status=mamaMsgImpl_setQueue (impl->mMsg,
					(mamaQueue)impl)))
	{
		mama_log (MAMA_LOG_LEVEL_ERROR, "mamaQueue_create(): "
				  "Could not set queue on new message.");
		mamaQueue_destroy ((mamaQueue)impl);
		return status;
	}
    return status;
}

mama_status
mamaQueue_create (mamaQueue* queue,
                  mamaBridge bridgeImpl)
{
    mama_status     status      =   MAMA_STATUS_OK;
    mamaBridgeImpl* bImpl       =   (mamaBridgeImpl*)bridgeImpl;
    mamaQueueImpl*  impl        =   NULL;

    if (!bridgeImpl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaQueue_create(): NULL bridge.");
        return MAMA_STATUS_NO_BRIDGE_IMPL;
    }
    if (!queue)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaQueue_create(): NULL queue"
                                         " address.");
        return MAMA_STATUS_NULL_ARG;
    }

    /*Create the queue structure*/
    impl = (mamaQueueImpl*)calloc (1, sizeof (mamaQueueImpl));

    if (!impl)
        return MAMA_STATUS_NOMEM;

    impl->mBridgeImpl           =   bImpl;
    impl->mMamaQueueBridgeImpl  =   NULL;
    impl->mDispatcher           =   NULL;
    impl->mQueueName            =   strdup ("NO_NAME");
    impl->mHighWatermark        =   0;
    impl->mLowWatermark         =   1;
    impl->mQueueMonitorCallbacks.onQueueHighWatermarkExceeded =   NULL;
    impl->mQueueMonitorCallbacks.onQueueLowWatermark          =   NULL;

    impl->mQueueMonitorClosure  =   NULL;

    /* Create the counter lock. */
    wInterlocked_initialize(&impl->mNumberOpenObjects);
    wInterlocked_set(0, &impl->mNumberOpenObjects);



	/* Call the bridge impl specific queue create function*/
	if (MAMA_STATUS_OK!=(status=impl->mBridgeImpl->bridgeMamaQueueCreate
				(&(impl->mMamaQueueBridgeImpl),
				 impl)))
	{
		mama_log (MAMA_LOG_LEVEL_ERROR, "Could not create queue bridge.");
		mamaQueue_destroy ((mamaQueue)impl);
		return status;
	}

    /* Determine if object lock tracking is enabled. */
    {
        const char *propValue = mama_getProperty(MAMAQUEUE_PROPERTY_OBJECT_LOCK_TRACKING);

        /* Only do this if it's set. */
        if(NULL != propValue)
        {
            impl->mTrackObjectLocks = strtobool(propValue);
        }
    }

    *queue = (mamaQueue)impl;

    mamaQueue_enableStats((mamaQueue)impl);
    return MAMA_STATUS_OK;
}

mama_status
mamaQueue_enableStats(mamaQueue queue)
{
    mamaQueueImpl*  impl        =   (mamaQueueImpl *)queue;
    const char*     middleware  =   NULL;
    mama_status     status      =   MAMA_STATUS_OK;

	if (gGenerateQueueStats)
    {
        middleware = impl->mBridgeImpl->bridgeGetName();

        impl->mStatsCollector = (mamaStatsCollector*)mamaStatsGenerator_allocateStatsCollector (mamaInternal_getStatsGenerator());
        if (MAMA_STATUS_OK != (status=mamaStatsCollector_create (
                                    impl->mStatsCollector,
                                    MAMA_STATS_COLLECTOR_TYPE_QUEUE,
                                    impl->mQueueName, middleware)))
        {
            mama_log (MAMA_LOG_LEVEL_ERROR, "mamaQueue_create(): "
                      "Could not create stats collector.");
            mamaQueue_destroy ((mamaQueue)impl);
            return status;
        }

        /* Stats are logged by default - requires explicit property not
           to log */
        if (!gLogQueueStats)
        {
            if (MAMA_STATUS_OK != (status=mamaStatsCollector_setLog (
                                   *impl->mStatsCollector, 0)))
            {
                return status;
            }
        }

        /* Stats are not published by default - requires explicit property
           to publish */
        if (gPublishQueueStats)
        {
            if (MAMA_STATUS_OK != (status=mamaStatsCollector_setPublish (
                                   *impl->mStatsCollector, 1)))
            {
                return status;
            }

            mama_log (MAMA_LOG_LEVEL_NORMAL, "Stats publishing enabled for queue %s",
                      impl->mQueueName);
        }

        status = mamaStat_create (&impl->mInitialStat,
                                  impl->mStatsCollector,
                                  MAMA_STAT_NOT_LOCKABLE,
                                  MamaStatInitials.mName,
                                  MamaStatInitials.mFid);
        if (status != MAMA_STATUS_OK) return status;

        status = mamaStat_create (&impl->mRecapStat,
                                  impl->mStatsCollector,
                                  MAMA_STAT_NOT_LOCKABLE,
                                  MamaStatRecaps.mName,
                                  MamaStatRecaps.mFid);
        if (status != MAMA_STATUS_OK) return status;

        status = mamaStat_create (&impl->mUnknownMsgStat,
                                  impl->mStatsCollector,
                                  MAMA_STAT_NOT_LOCKABLE,
                                  MamaStatUnknownMsgs.mName,
                                  MamaStatUnknownMsgs.mFid);
        if (status != MAMA_STATUS_OK) return status;

        status = mamaStat_create (&impl->mMessageStat,
                                  impl->mStatsCollector,
                                  MAMA_STAT_NOT_LOCKABLE,
                                  MamaStatNumMessages.mName,
                                  MamaStatNumMessages.mFid);
        if (status != MAMA_STATUS_OK) return status;

        status = mamaStat_create (&impl->mQueueSizeStat,
                                  impl->mStatsCollector,
                                  MAMA_STAT_NOT_LOCKABLE,
                                  MamaStatQueueSize.mName,
                                  MamaStatQueueSize.mFid);
        if (status != MAMA_STATUS_OK) return status;

        status = mamaStat_create (&impl->mSubscriptionStat,
                                  impl->mStatsCollector,
                                  MAMA_STAT_NOT_LOCKABLE,
                                  MamaStatNumSubscriptions.mName,
                                  MamaStatNumSubscriptions.mFid);
        if (status != MAMA_STATUS_OK) return status;

        status = mamaStat_create (&impl->mTimeoutStat,
                                  impl->mStatsCollector,
                                  MAMA_STAT_NOT_LOCKABLE,
                                  MamaStatTimeouts.mName,
                                  MamaStatTimeouts.mFid);
        if (status != MAMA_STATUS_OK) return status;

        status = mamaStat_create (&impl->mWombatMsgsStat,
                                  impl->mStatsCollector,
                                  MAMA_STAT_NOT_LOCKABLE,
                                  MamaStatWombatMsgs.mName,
                                  MamaStatWombatMsgs.mFid);
        if (status != MAMA_STATUS_OK) return status;

        status = mamaStat_create (&impl->mFastMsgsStat,
                                  impl->mStatsCollector,
                                  MAMA_STAT_NOT_LOCKABLE,
                                  MamaStatFastMsgs.mName,
                                  MamaStatFastMsgs.mFid);
        if (status != MAMA_STATUS_OK) return status;

        status = mamaStat_create (&impl->mRvMsgsStat,
                                  impl->mStatsCollector,
                                  MAMA_STAT_NOT_LOCKABLE,
                                  MamaStatRvMsgs.mName,
                                  MamaStatRvMsgs.mFid);
        if (status != MAMA_STATUS_OK) return status;

        if (mamaInternal_getStatsGenerator())
        {
            if (MAMA_STATUS_OK != (
                    status=mamaStatsGenerator_addStatsCollector (
                        mamaInternal_getStatsGenerator(),
                        impl->mStatsCollector)))
            {
                mama_log(MAMA_LOG_LEVEL_ERROR, "mamaQueue_create(): "
                         "Could not add queue stats collector.");
                return status;
            }
        }

        mamaStat_setPollCallback (impl->mQueueSizeStat, mamaQueue_pollQueueSizeCb, queue);
    }

    return status;
}
mama_status
mamaQueue_create_usingNative (mamaQueue* queue,
                              mamaBridge bridgeImpl,
                              void*      nativeQueue)
{
    mama_status     status  =   MAMA_STATUS_OK;
    mamaBridgeImpl* bImpl   =   (mamaBridgeImpl*)bridgeImpl;
    mamaQueueImpl*  impl    =   NULL;

    if (!bridgeImpl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaQueue_create(): NULL bridge.");
        return MAMA_STATUS_NO_BRIDGE_IMPL;
    }
    if (!queue)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaQueue_create(): NULL queue"
                                         " address.");
        return MAMA_STATUS_NULL_ARG;
    }

    /*Create the queue structure*/
    impl = (mamaQueueImpl*)calloc (1, sizeof (mamaQueueImpl));

    if (!impl)
        return MAMA_STATUS_NOMEM;


    impl->mBridgeImpl           =   bImpl;
    impl->mMamaQueueBridgeImpl  =   NULL;
    impl->mDispatcher           =   NULL;
    impl->mQueueName            =   strdup ("NO_NAME");
    impl->mHighWatermark        =   0;
    impl->mLowWatermark         =   1;
    impl->mQueueMonitorCallbacks.onQueueHighWatermarkExceeded =   NULL;
    impl->mQueueMonitorCallbacks.onQueueLowWatermark          =   NULL;

    impl->mQueueMonitorClosure  =   NULL;

    /* Create the counter lock. */
    wInterlocked_set(0, &impl->mNumberOpenObjects);

    mamaQueue_createReuseableMsg(impl);

    /* Call the bridge impl specific queue create function*/
    if (MAMA_STATUS_OK!=(status=impl->mBridgeImpl->bridgeMamaQueueCreateUsingNative
                (&(impl->mMamaQueueBridgeImpl),
                 impl,
                 nativeQueue)))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "Could not create queue bridge.");
        mamaQueue_destroy ((mamaQueue)impl);
        return status;
    }

    *queue = (mamaQueue)impl;

    return MAMA_STATUS_OK;
}

void
mamaQueue_decrementObjectCount(mamaQueueLockHandle *handle,
                               mamaQueue           queue)
{
    /* Cast the queue to an impl. */
    mamaQueueImpl *impl = (mamaQueueImpl *)queue;

    /* Decrement the count. */
    int newCount = wInterlocked_decrement(&impl->mNumberOpenObjects);

    /* Write a log if something has gone wrong. */
    if(newCount < 0)
    {
        mama_log(MAMA_LOG_LEVEL_ERROR, "Queue 0x%p has been dereferenced too many times.", queue);
    }

    /* Check if lock tracking is turned on. */
    if(1 == impl->mTrackObjectLocks)
    {
        /* Cast the handle to the lock structure. */
        pMamaQueueLock lockStruct = (pMamaQueueLock)*handle;

        /* The handle will be valid when lock tracking is turned on, however if it is NULL then it has
         * already been freed.
         */
        if(NULL == *handle)
        {
            mama_log(MAMA_LOG_LEVEL_NORMAL, "The lock count for queue 0x%p has already been decremented.", queue);
        }
        else
        {
            /* Write a log message at normal level. */
            mama_log(MAMA_LOG_LEVEL_NORMAL, "mamaQueue_decrementObjectCount(): queue 0x%p, owner 0x%p, new count %d.", queue, lockStruct->m_owner, newCount);

            /* Free the structure. */
            free(lockStruct);

            /* Reset the variable to protect against multiple calls to this function. */
            *handle = NULL;
        }
    }
}

mamaQueueLockHandle
mamaQueue_incrementObjectCount(mamaQueue queue,
                               void      *owner)
{
    /* Returns. */
    mamaQueueLockHandle ret = NULL;

    /* Cast the queue to an impl. */
    mamaQueueImpl *impl = (mamaQueueImpl *)queue;

    /* Increment the count. */
    int newCount = wInterlocked_increment(&impl->mNumberOpenObjects);

    /* Check if lock tracking is turned on. */
    if(1 == impl->mTrackObjectLocks)
    {
        /* Allocate a lock structure, this means that if the lock count isn't decremented there will
         * be a memory leak the stack for which will be picked up by any leak diagnostic tool.
         */
        pMamaQueueLock lockStruct = (pMamaQueueLock)calloc(1, sizeof(mamaQueueLock));
        if(NULL != lockStruct)
        {
            /* Write a log message at normal level. */
            mama_log(MAMA_LOG_LEVEL_NORMAL, "mamaQueue_incrementObjectCount(): queue 0x%p, owner 0x%p, new count %d.", queue, owner, newCount);

            /* Save the owner pointer. */
            lockStruct->m_owner = owner;

            /* This will be the return handle. */
            ret = (mamaQueueLockHandle)lockStruct;
        }
    }

    return ret;
}

mama_status
mamaQueue_destroyWait(mamaQueue queue)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;

    if(queue == NULL)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaQueue_destroy(): NULL queue.");
    }
    else
    {
        /* Attempt to destroy the queue. */
        ret = mamaQueue_destroy(queue);

        /* If there are still open objects then keep trying. */
        while(MAMA_STATUS_QUEUE_OPEN_OBJECTS == ret)
        {
            /* Dispatch messages for 10 ms. */
            ret = mamaQueue_timedDispatch(queue, 10);
            if(MAMA_STATUS_OK == ret)
            {
                /* Attempt to destroy the queue again. */
                ret = mamaQueue_destroy(queue);
            }
        }
    }

    return ret;
}

mama_status
mamaQueue_destroyTimedWait(mamaQueue queue,
                           long      timeout)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;

    if(queue == NULL)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaQueue_destroy(): NULL queue.");
    }
    else
    {
        /* Timeout must be greater than 0. */
        if(timeout > 0)
        {
            /* Initialise a time counter. */
            long currentTime = 0;

            /* Attempt to destroy the queue. */
            ret = mamaQueue_destroy(queue);

            /* If there are still open objects then keep trying until the time runs out. */
            while((MAMA_STATUS_QUEUE_OPEN_OBJECTS == ret) && (currentTime < timeout))
            {
                /* Dispatch messages for 10 ms. */
                ret = mamaQueue_timedDispatch(queue, 10);
                if(MAMA_STATUS_OK == ret)
                {
                    /* Add 10ms to the count. */
                    currentTime += 10;

                    /* Attempt to destroy the queue again. */
                    ret = mamaQueue_destroy(queue);
                }
            }

            /* If the time has run out then return a specific error code. */
            if((MAMA_STATUS_OK != ret) && (currentTime >= timeout))
            {
                ret = MAMA_STATUS_TIMEOUT;
            }
        }
    }

    return ret;
}

mama_status
mamaQueue_canDestroy(mamaQueue queue)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if(queue != NULL)
    {
        /* Cast the queue to an impl. */
        mamaQueueImpl *impl = (mamaQueueImpl *)queue;

        /* If the number of objects is 0 then the queue can be destroyed. */
        ret = MAMA_STATUS_QUEUE_OPEN_OBJECTS;
        if(0 == wInterlocked_read(&impl->mNumberOpenObjects))
        {
            ret = MAMA_STATUS_OK;
        }
    }

    return ret;
}

mama_status
mamaQueue_destroy (mamaQueue queue)
{
    mamaQueueImpl* impl = (mamaQueueImpl*)queue;
    mama_status    status = MAMA_STATUS_OK;

    mama_log (MAMA_LOG_LEVEL_FINEST, "Entering mamaQueue_destroy for queue 0x%X.", queue);

    impl->mIsDispatching = 0;
    if (!queue)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaQueue_destroy(): NULL queue.");
        return MAMA_STATUS_NULL_ARG;
    }

    /* The queue can only be destroyed if there are no open event objects. */
    status = MAMA_STATUS_QUEUE_OPEN_OBJECTS;

    /* Only continue if the object count is 0. */
    if(0 == wInterlocked_read(&impl->mNumberOpenObjects))
    {
        if (impl->mMamaQueueBridgeImpl)
        {
            if (MAMA_STATUS_OK!=(status=impl->mBridgeImpl->bridgeMamaQueueDestroy (
                        impl->mMamaQueueBridgeImpl)))
            {
                mama_log (MAMA_LOG_LEVEL_ERROR,
                      "mamaQueue_destroy(): Could not destroy queue bridge.");
                     /*We should continue and free up the rest of the structure.*/
            }
        }

        if (impl->mDispatcher)
        {
            /* We don't want the dispatcher to access a destroyed queue */
            ((mamaDispatcherImpl*)(impl->mDispatcher))->mIsDispatching = 0;
            ((mamaDispatcherImpl*)(impl->mDispatcher))->mQueue = NULL;
        }

        /*Destroy the cached mamaMsg - no longer needed*/
        if (impl->mMsg) mamaMsg_destroy (impl->mMsg);

        if (impl->mInitialStat)
        {
            mamaStat_destroy (impl->mInitialStat);
            impl->mInitialStat = NULL;
        }

        if (impl->mRecapStat)
        {
            mamaStat_destroy (impl->mRecapStat);
            impl->mRecapStat = NULL;
        }

        if (impl->mUnknownMsgStat)
        {
            mamaStat_destroy (impl->mUnknownMsgStat);
            impl->mUnknownMsgStat = NULL;
        }

        if (impl->mMessageStat)
        {
            mamaStat_destroy (impl->mMessageStat);
            impl->mMessageStat = NULL;
        }

        if (impl->mQueueSizeStat)
        {
            mamaStat_destroy (impl->mQueueSizeStat);
            impl->mQueueSizeStat = NULL;
        }

        if (impl->mSubscriptionStat)
        {
            mamaStat_destroy (impl->mSubscriptionStat);
            impl->mSubscriptionStat = NULL;
        }

        if (impl->mTimeoutStat)
        {
            mamaStat_destroy (impl->mTimeoutStat);
            impl->mTimeoutStat = NULL;
        }

        if (impl->mWombatMsgsStat)
        {
            mamaStat_destroy (impl->mWombatMsgsStat);
            impl->mWombatMsgsStat = NULL;
        }

        if (impl->mFastMsgsStat)
        {
            mamaStat_destroy (impl->mFastMsgsStat);
            impl->mFastMsgsStat = NULL;
        }

        if (impl->mRvMsgsStat)
        {
            mamaStat_destroy (impl->mRvMsgsStat);
            impl->mRvMsgsStat = NULL;
        }

        if (impl->mStatsCollector)
        {
            mamaStatsGenerator_removeStatsCollector  (mamaInternal_getStatsGenerator(), impl->mStatsCollector);
            mamaStatsCollector_destroy (*impl->mStatsCollector);
            impl->mStatsCollector = NULL;
        }
         if (impl->mQueueName)
            free ((void*)impl->mQueueName);

        impl->mBridgeImpl          = NULL;
        impl->mMamaQueueBridgeImpl = NULL;
        impl->mMsg                 = NULL;

        /* Destroy the counter lock */
        wInterlocked_destroy(&impl->mNumberOpenObjects);

        free (impl);

        mama_log (MAMA_LOG_LEVEL_FINEST, "Leaving mamaQueue_destroy for queue 0x%X.", queue);
        status = MAMA_STATUS_OK;
    }

    return status;
}

mama_status
mamaQueue_getEventCount (mamaQueue queue,
                         size_t*   count)
{
    mamaQueueImpl* impl = (mamaQueueImpl*)queue;

    if (!impl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mamaQueue_getEventCount(): NULL queue.");
        return MAMA_STATUS_NULL_ARG;
    }

    if (!count)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mamaQueue_getEventCount(): Invalid count address.");
        return MAMA_STATUS_INVALID_ARG;
    }

    return impl->mBridgeImpl->bridgeMamaQueueGetEventCount (
                    impl->mMamaQueueBridgeImpl,
                    count);
}

mama_status
mamaQueue_dispatch (mamaQueue queue)
{
    mamaQueueImpl* impl = (mamaQueueImpl*)queue;
    if (!impl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mamaQueue_dispatch(): NULL queue.");
        return MAMA_STATUS_NULL_ARG;
    }
    impl->mIsDispatching = 1;

    return impl->mBridgeImpl->bridgeMamaQueueDispatch
                                (impl->mMamaQueueBridgeImpl);
}

mama_status
mamaQueue_timedDispatch (mamaQueue queue,
                         uint64_t  timeout)
{
    mamaQueueImpl* impl = (mamaQueueImpl*)queue;

    if (!impl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mamaQueue_timedDispatch(): NULL queue.");
        return MAMA_STATUS_NULL_ARG;
    }

    return impl->mBridgeImpl->bridgeMamaQueueTimedDispatch
                            (impl->mMamaQueueBridgeImpl, timeout);
}

mama_status
mamaQueue_dispatchEvent (mamaQueue queue)
{
    mamaQueueImpl* impl = (mamaQueueImpl*)queue;

    if (!impl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mamaQueue_dispatchEvent(): NULL queue.");
        return MAMA_STATUS_NULL_ARG;
    }

    return impl->mBridgeImpl->bridgeMamaQueueDispatchEvent
                        (impl->mMamaQueueBridgeImpl);
}

mama_status
mamaQueue_stopDispatch (mamaQueue queue)
{
    mamaQueueImpl* impl = (mamaQueueImpl*)queue;

    if (!impl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mamaQueue_stopDispatch(): NULL queue.");
        return MAMA_STATUS_NULL_ARG;
    }
    impl->mIsDispatching = 0;

    return impl->mBridgeImpl->bridgeMamaQueueStopDispatch
                            (impl->mMamaQueueBridgeImpl);
}

mama_status
mamaQueue_setEnqueueCallback (mamaQueue          queue,
                              mamaQueueEnqueueCB callback,
                              void*              closure)
{
    mamaQueueImpl* impl = (mamaQueueImpl*)queue;

    if (!impl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mamaQueue_setEnqueueCallback(): NULL queue.");
        return MAMA_STATUS_NULL_ARG;
    }

    return impl->mBridgeImpl->bridgeMamaQueueSetEnqueueCallback
                        (impl->mMamaQueueBridgeImpl, callback, closure);
}

mama_status
mamaQueue_removeEnqueueCallback (mamaQueue queue)
{
    mamaQueueImpl* impl = (mamaQueueImpl*)queue;

    if (!impl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mamaQueue_removeEnqueueCallback(): NULL queue.");
        return MAMA_STATUS_NULL_ARG;
    }

    return impl->mBridgeImpl->bridgeMamaQueueRemoveEnqueueCallback
                    (impl->mMamaQueueBridgeImpl);
}

mama_status
mamaQueue_enqueueEvent  (mamaQueue        queue,
                         mamaQueueEventCB callback,
                         void*            closure)
{
    mamaQueueImpl* impl = (mamaQueueImpl*)queue;

    if (!impl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mamaQueue_enqueueEvent(): NULL queue.");
        return MAMA_STATUS_NULL_ARG;
    }

    return impl->mBridgeImpl->bridgeMamaQueueEnqueueEvent
                        (impl->mMamaQueueBridgeImpl, callback, closure);
}

mama_status
mamaQueue_getNativeHandle (mamaQueue queue,
                           void**    nativeHandle)
{
    mamaQueueImpl* impl = (mamaQueueImpl*)queue;

    if (!impl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mamaQueue_getNativeHandle(): NULL queue.");
        return MAMA_STATUS_NULL_ARG;
    }

    return impl->mBridgeImpl->bridgeMamaQueueGetNativeHandle
             (impl->mMamaQueueBridgeImpl, nativeHandle);
}

mama_status
mamaQueue_setQueueMonitorCallbacks (mamaQueue                  queue,
                                    mamaQueueMonitorCallbacks* queueMonitorCallbacks,
                                    void*                      closure)
{
    mamaQueueImpl* impl = (mamaQueueImpl*)queue;
    if (!impl) return MAMA_STATUS_NULL_ARG;

    impl->mQueueMonitorCallbacks = *queueMonitorCallbacks; /*Copy*/
    impl->mQueueMonitorClosure   = closure;
    return MAMA_STATUS_OK;
}

extern mama_status
mamaQueue_setHighWatermark (mamaQueue queue,
                            size_t    highWatermark)
{
    mamaQueueImpl* impl = (mamaQueueImpl*)queue;

    if (!impl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                    "mamaQueue_setHighWatermark(): NULL queue.");
        return MAMA_STATUS_NULL_ARG;
    }

    if (highWatermark < impl->mLowWatermark)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                "mamaQueue_setHighWatermark(): "
                "high watermark [%d] is less than low watermark [%d].",
                highWatermark,
                impl->mLowWatermark);
        return MAMA_STATUS_INVALID_ARG;
    }

    impl->mHighWatermark = highWatermark;
    return impl->mBridgeImpl->bridgeMamaQueueSetHighWatermark
            (impl->mMamaQueueBridgeImpl, highWatermark);
}

mama_status
mamaQueue_setLowWatermark (mamaQueue   queue,
                           size_t      lowWatermark)
{
    mamaQueueImpl* impl = (mamaQueueImpl*)queue;

    if (!impl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mamaQueue_setLowWatermark(): NULL queue.");
        return MAMA_STATUS_NULL_ARG;
    }
    /* WMW does not like 0 as low water mark. */
    if (lowWatermark == 0) lowWatermark = 1;

    if (impl->mHighWatermark < lowWatermark)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mamaQueue_setLowWatermark(): "
                  "low watermark [%d] is greater than high watermark [%d].",
                  lowWatermark,
                  impl->mHighWatermark);
        return MAMA_STATUS_INVALID_ARG;
    }

    impl->mLowWatermark = lowWatermark;

    return impl->mBridgeImpl->bridgeMamaQueueSetLowWatermark
             (impl->mMamaQueueBridgeImpl, lowWatermark);
}

mama_status
mamaQueue_setQueueName (mamaQueue   queue,
                        const char* name)
{
    mamaQueueImpl*    impl  = (mamaQueueImpl*)queue;

    if (!impl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mamaQueue_setQueueName(): NULL queue.");
        return MAMA_STATUS_NULL_ARG;
    }
    if (!name)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mamaQueue_setQueueName(): NULL name.");
        return MAMA_STATUS_INVALID_ARG;
    }

    if (impl->mQueueName)
        free ((void*)impl->mQueueName);

    impl->mQueueName = NULL;

    impl->mQueueName = strdup (name);

    if (strcmp (impl->mQueueName, "DEFAULT_MAMA_QUEUE")==0)
    {
        impl->mIsDefaultQueue = 1;
    }

    if (!impl->mQueueName)
        return MAMA_STATUS_NOMEM;

    if (impl->mStatsCollector)
    {
        mamaStatsCollector_setName (*impl->mStatsCollector, impl->mQueueName);
    }

    return MAMA_STATUS_OK;
}

mama_status
mamaQueue_getHighWatermark (mamaQueue queue,
                            size_t*   highWatermark)
{
    mamaQueueImpl*    impl  = (mamaQueueImpl*)queue;

    if (!impl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mamaQueue_getHighWatermark(): NULL queue.");
        return MAMA_STATUS_NULL_ARG;
    }
    if (!highWatermark)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mamaQueue_getHighWatermark(): NULL highWatermark.");
        return MAMA_STATUS_INVALID_ARG;
    }

    *highWatermark = impl->mHighWatermark;

    return MAMA_STATUS_OK;
}

mama_status
mamaQueue_getLowWatermark (mamaQueue queue,
                           size_t*   lowWatermark)
{
    mamaQueueImpl*    impl  = (mamaQueueImpl*)queue;

    if (!impl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mamaQueue_getLowWatermark(): NULL queue.");
        return MAMA_STATUS_NULL_ARG;
    }
    if (!lowWatermark)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mamaQueue_getLowWatermark(): NULL lowWatermark.");
        return MAMA_STATUS_INVALID_ARG;
    }

    *lowWatermark = impl->mLowWatermark;

    return MAMA_STATUS_OK;
}

mama_status
mamaQueue_getQueueName (mamaQueue    queue,
                        const char** name)
{
    mamaQueueImpl* impl = (mamaQueueImpl*)queue;
    if (!impl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mamaQueue_getQueueName(): NULL queue.");
        return MAMA_STATUS_NULL_ARG;
    }
    if (!name)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mamaQueue_getQueueName(): NULL name.");
        return MAMA_STATUS_INVALID_ARG;
    }

    *name  = impl->mQueueName;

    return MAMA_STATUS_OK;
}

mama_status
mamaQueue_getQueueBridgeName (mamaQueue    queue,
                              const char** name)
{
    mamaQueueImpl* impl = (mamaQueueImpl*)queue;
    if (!impl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mamaQueue_getQueueBridgeName(): NULL queue.");
        return MAMA_STATUS_NULL_ARG;
    }

    *name = impl->mBridgeImpl->bridgeGetName();

    return MAMA_STATUS_OK;
}

mamaStatsCollector*
mamaQueueImpl_getStatsCollector (mamaQueue queue)
{
    mamaQueueImpl* impl = (mamaQueueImpl*)queue;
    if (!impl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mamaQueue_getStatsCollector(): NULL queue.");
        return NULL;
    }

    return impl->mStatsCollector;
}

mamaBridgeImpl*
mamaQueueImpl_getBridgeImpl (mamaQueue queue)
{
    mamaQueueImpl* impl = (mamaQueueImpl*)queue;
    if (!impl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mamaQueueImpl_getBridgeImpl(): NULL queue.");
        return NULL;
    }
    return impl->mBridgeImpl;
}

mamaMsg
mamaQueueImpl_getMsg (mamaQueue queue)
{
    mamaQueueImpl* impl = (mamaQueueImpl*)queue;
    if (!impl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mamaQueueImpl_getMsg(): NULL queue.");
        return NULL;
    }
    if (!impl->mMsg)
    	mamaQueue_createReuseableMsg(impl);
    return impl->mMsg;
}

mama_status
mamaQueueImpl_detachMsg (mamaQueue queue,
                         mamaMsg   msg)
{
    mamaQueueImpl* impl = (mamaQueueImpl*)queue;
    mama_status    status   =   MAMA_STATUS_OK;

    if (!impl)
        return MAMA_STATUS_NULL_ARG;

    if (msg == impl->mMsg)
    {
        if (MAMA_STATUS_OK!=(status=mamaMsg_create (&(impl->mMsg))))
        {
            mama_log (MAMA_LOG_LEVEL_ERROR, "mamaQueueImpl_detachMsg():"
                      " Could not create new message.");
            impl->mMsg = msg;
            return status;
        }

        /*We now have to set the bridge and queue on this new message*/
        if (MAMA_STATUS_OK!=(status=mamaMsgImpl_setBridgeImpl (impl->mMsg,
                        impl->mBridgeImpl)))
        {
            mama_log (MAMA_LOG_LEVEL_ERROR, "mamaQueueImpl_detachMsg():"
                      " Could not set bridge on new message.");
            mamaMsg_destroy (impl->mMsg);
            impl->mMsg = msg;
            return status;
        }

        /*We don't want to destroy the bridge message when the mamamsg is
         * destroyed as we won't own it*/
        if (MAMA_STATUS_OK!=(status=mamaMsgImpl_setMessageOwner (impl->mMsg, 0)))
        {
            mama_log (MAMA_LOG_LEVEL_ERROR, "mamaQueueImpl_detachMsg():"
                      " Could not set owner on new message.");
            return status;
        }

        if (MAMA_STATUS_OK!=(status=mamaMsgImpl_setQueue (impl->mMsg, queue)))
        {
            mama_log (MAMA_LOG_LEVEL_ERROR, "mamaQueueImpl_detachMsg():"
                      " Could not set queue on new message.");
            mamaMsg_destroy (impl->mMsg);
            impl->mMsg = msg;
            return status;
        }
    }
    return MAMA_STATUS_OK;
}

mama_status
mamaQueueImpl_highWatermarkExceeded (mamaQueue queue,
                                     size_t    size)
{
    mamaQueueImpl* impl = (mamaQueueImpl*)queue;

    if (!impl)
        return MAMA_STATUS_NULL_ARG;

    if (impl->mQueueMonitorCallbacks.onQueueHighWatermarkExceeded)
    {
        mama_log (MAMA_LOG_LEVEL_FINE,
                  "High water mark for queue [%s] exceeded : %d",
                  impl->mQueueName, size);
        impl->mQueueMonitorCallbacks.onQueueHighWatermarkExceeded (
                                queue,
                                size,
                                impl->mQueueMonitorClosure);
    }
    return MAMA_STATUS_OK;
}

mama_status
mamaQueueImpl_lowWatermarkExceeded (mamaQueue queue,
                                    size_t    size)
{
    mamaQueueImpl* impl = (mamaQueueImpl*)queue;

    if (!impl) return MAMA_STATUS_NULL_ARG;

    if (impl->mQueueMonitorCallbacks.onQueueLowWatermark)
    {
        mama_log (MAMA_LOG_LEVEL_FINE,
                  "Low water mark for queue [%s] exceeded : %d",
                  impl->mQueueName, size);
        impl->mQueueMonitorCallbacks.onQueueLowWatermark (
                                queue,
                                size,
                                impl->mQueueMonitorClosure);
    }
    return MAMA_STATUS_OK;
}

static void
*dispatchThreadProc( void *closure )
{
    mamaDispatcherImpl* impl = (mamaDispatcherImpl*)closure;

    impl->mIsDispatching = 1;

    while (impl->mIsDispatching && !impl->mDestroy &&
           MAMA_STATUS_OK == mamaQueue_dispatch (impl->mQueue))
        ;

    impl->mIsDispatching = 0;

    if (impl->mQueue)
        impl->mQueue->mDispatcher = NULL;

    return NULL;
}

mama_status
mamaDispatcher_create (mamaDispatcher *result,
                       mamaQueue      queue)
{
    mamaQueueImpl*      qImpl   = (mamaQueueImpl*)queue;
    mamaDispatcherImpl* impl    = NULL;

    if (!queue)
    {
        mama_log (MAMA_LOG_LEVEL_WARN, "mamaDispatcher_create(): NULL queue.");
        return MAMA_STATUS_INVALID_ARG;
    }

    if (!result)
    {
        mama_log (MAMA_LOG_LEVEL_WARN, "mamaDispatcher_create(): Invalid "
                                        "address");
        return MAMA_STATUS_NULL_ARG;
    }

    *result = NULL;

    /*A queue can only have a single dispatcher.*/
    if (qImpl->mDispatcher)
        return MAMA_STATUS_TOO_MANY_DISPATCHERS;

    impl = (mamaDispatcherImpl*)calloc( 1, sizeof(mamaDispatcherImpl) );
    if (impl == NULL)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaDispatcher_create(): Could not "
                                         "allocate dispatcher.");
        return MAMA_STATUS_NOMEM;
    }

    impl->mQueue = queue;
    impl->mDestroy = 0;
    if (wthread_create(&impl->mThread, NULL, dispatchThreadProc, impl))
    {
        free (impl);
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaDispatcher_create(): Could not "
                                        "create dispatch thread.");
        return MAMA_STATUS_SYSTEM_ERROR;
    }

    qImpl->mDispatcher = (mamaDispatcher)impl;
    *result = (mamaDispatcher)impl;

    return MAMA_STATUS_OK;
}

mama_status
mamaDispatcher_destroy (mamaDispatcher dispatcher)
{
    mamaDispatcherImpl* impl = (mamaDispatcherImpl*)dispatcher;

    if (!impl)
        return MAMA_STATUS_NULL_ARG;

    if( impl->mQueue && impl->mIsDispatching )
    {
        impl->mIsDispatching = 0;
        mamaQueue_stopDispatch (impl->mQueue);
    }

    impl->mDestroy = 1;

    /* Wait for the thread to return. */
    wthread_join (impl->mThread, NULL);

    impl->mQueue->mDispatcher = NULL;
    free (impl);
    return MAMA_STATUS_OK;
}

int
mamaQueueImpl_isDispatching (mamaQueue queue)
{
    return ((mamaQueueImpl*)queue)->mIsDispatching;
}

int MAMACALLTYPE
mamaQueue_pollQueueSizeCb (void* closure)
{
    size_t count = 0;
    mamaQueue queue = (mamaQueue)closure;

    mamaQueue_getEventCount (queue, &count);

    return count;
}
