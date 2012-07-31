/* $Id: refreshtransport.c,v 1.27.4.1.10.3 2011/08/10 14:53:25 nicholasmarriott Exp $
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

/**
 * Adds refresh and sync logic to transport impl. Not all messaging middleware
 * requires refresh and sync. For example elvin takes care of this under the hood,
 * and the corresponding feed handler is simply a broadcast handler that does
 * not recognize sync or refresh requests.
 *
 * Note: We may want to replace this inheritance relationship with a delgate/strategy
 * pattern to acheive a plugin architecture. This will only be necessary if other
 * middleware specific transport features arise that need to be added separately
 * from Refresh semantics.
 */

#include <time.h>

#include "mama/mama.h"
#include "mama/timer.h"
#include "mama/msg.h"
#include "mama/subscmsgtype.h"
#include "msgutils.h"
#include "bridge.h"
#include "transportimpl.h"
#include "subscriptionimpl.h"
#include "list.h"
#include "refreshtransport.h"

#define SYNC_SUBJECTS_PER_MESSAGE  100

static time_t last = 0;

typedef struct refreshTransportImpl_
{
    mamaTransport      mMamaTransport;
    mamaTimer          mRefreshTimer;
    mamaTimer          mStaleRecapTimer;
    int                mStaleRecapTimeout;
    wList              mListeners;
    wList              mNewListeners;
    mamaBridgeImpl*    mBridgeImpl;
} refreshTransportImpl;


static
void refreshTransportImpl_doRefresh (refreshTransportImpl *impl);

static mama_status
init (refreshTransportImpl *impl);


extern mama_status
refreshTransport_create (refreshTransport*  result,
                        mamaTransport      transport,
                        wList              listeners,
                        mamaBridgeImpl*    bridgeImpl)
{
    const char* propstring = NULL;

    refreshTransportImpl *impl =
        (refreshTransportImpl*)calloc (1, sizeof (refreshTransportImpl));

    if (impl == NULL)
    {
        return MAMA_STATUS_NOMEM;
    }

    impl->mMamaTransport = transport;
    impl->mListeners     = listeners;
    impl->mNewListeners  = list_create (sizeof (SubscriptionInfo));
    impl->mBridgeImpl    = bridgeImpl;

    propstring = properties_Get (mamaInternal_getProperties (),
            "mama.maybestale.recap.timeout");
    if (propstring)
    {
        impl->mStaleRecapTimeout = atoi (propstring);
    }

    *result = impl;

    return init (impl);
}

SubscriptionInfo*
refreshTransport_allocateSubscInfo (refreshTransport transport)
{
    refreshTransportImpl* impl = (refreshTransportImpl*)transport;
    return (SubscriptionInfo*) list_allocate_element (impl->mNewListeners);
}


static void MAMACALLTYPE
refreshTransportImpl_timerCallback (mamaTimer timer, void *closure)
{
    refreshTransportImpl *impl = (refreshTransportImpl*)closure;
    refreshTransportImpl_doRefresh (impl);
}

static void
checkQualityIterator (wList list, void* element, void* closure)
{
	mamaQuality mQuality = MAMA_QUALITY_UNKNOWN;
    SubscriptionInfo* subsc = (SubscriptionInfo*)element;
	mamaSubscription_getQuality (subsc->mSubscription, &mQuality);
    if (mQuality == MAMA_QUALITY_MAYBE_STALE)
    {
        mamaSubscription_requestRecap (subsc->mSubscription);
    }
}

static void MAMACALLTYPE
refreshTransportImpl_staleRecapTimerCallback (mamaTimer timer, void *closure)
{
	refreshTransportImpl *impl = (refreshTransportImpl*)closure;
	wombatThrottle  throttle = NULL;
	if (impl->mMamaTransport)
        throttle = mamaTransportImpl_getThrottle (impl->mMamaTransport,
                                               	  MAMA_THROTTLE_RECAP);

    if (throttle)
		wombatThrottle_lock (throttle);

	refreshTransport_iterateListeners (impl,checkQualityIterator,NULL);

	if (throttle)
            wombatThrottle_unlock (throttle);

    mamaTimer_destroy (timer);
}

extern void
refreshTransport_startRefreshTimer (struct refreshTransportImpl_ *impl)
{
    mama_status status = MAMA_STATUS_OK;

    if (!impl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "refreshTransport_startRefreshTimer (): NULL impl. Cannot"
                  " create refresh timer.");
        return;
    }

    if (impl->mRefreshTimer != NULL) return;

    if (MAMA_STATUS_OK!=(status=mamaTimer_create (
                &impl->mRefreshTimer,
                impl->mBridgeImpl->mDefaultEventQueue,
                refreshTransportImpl_timerCallback,
                REFRESH_GRANULARITY,
                impl)))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "refreshTransport_startRefreshTimer (): Failed to create"
                  " timer. [%s]", mamaStatus_stringForStatus (status));
    }

    return;
}

extern void
refreshTransport_startStaleRecapTimer (struct refreshTransportImpl_ *impl)
{
    mama_status status = MAMA_STATUS_OK;

    if (!impl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "refreshTransport_startRefreshTimer (): NULL impl. Cannot"
                  " create refresh timer.");
        return;
    }

    if (impl->mStaleRecapTimer != NULL) return;
    if (impl->mStaleRecapTimeout == 0) return;


    if (MAMA_STATUS_OK!=(status=mamaTimer_create (
                &impl->mStaleRecapTimer,
                impl->mBridgeImpl->mDefaultEventQueue,
                refreshTransportImpl_staleRecapTimerCallback,
                impl->mStaleRecapTimeout,
                impl)))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "refreshTransport_startStaleRecapTimer (): Failed to create"
                  " timer. [%s]", mamaStatus_stringForStatus (status));
    }

    return;
}



static mama_status
init (refreshTransportImpl *impl)
{
    srand (time (NULL));
    refreshTransport_startRefreshTimer (impl);


    return MAMA_STATUS_OK;

}

extern mama_status
refreshTransport_destroy (refreshTransport transport)
{
    refreshTransportImpl *impl = (refreshTransportImpl*)transport;

    list_destroy (impl->mNewListeners, NULL, NULL);

    if (impl->mRefreshTimer != NULL)
    {
        mamaTimer_destroy (impl->mRefreshTimer);
        impl->mRefreshTimer = NULL;
    }

    free (impl);

    return MAMA_STATUS_OK;
}

extern void
refreshTransport_addSubscription (refreshTransport transport,
                                  SubscriptionInfo *info)
{
    refreshTransportImpl *impl = (refreshTransportImpl*)transport;

    info->mNextRefreshTime = time (NULL) + MAMA_REFRESHINTERVALRAND * 60;
    list_push_back (impl->mNewListeners, info);

    return;
}

extern mama_status
refreshTransport_stopRefreshTimer (refreshTransportImpl *impl)
{
    if (impl->mRefreshTimer != NULL)
    {
        mamaTimer_destroy (impl->mRefreshTimer);
        impl->mRefreshTimer = NULL;
    }

    return MAMA_STATUS_OK;
}



void checkOrder (wList list, void *element, void *closure)
{
    SubscriptionInfo *info = (SubscriptionInfo*)element;

    if (info->mNextRefreshTime < last)
    {
        fprintf (stderr, "Bad list order!!!!\n");
        exit (0);
    }

    last = info->mNextRefreshTime;
}

static void
refreshTransportImpl_processNewSubscriptions (refreshTransportImpl *impl)
{
    int sent = 0;
    mamaMsg refreshMsg;
    SubscriptionInfo *cur;
    SubscriptionInfo *last;
    int intervals = 0;
    int newListenerCount = 0;
    int messagesPerInterval = 0;
    int maxMessages = 0;
    time_t curTime = time (NULL);
    wombatThrottle  throttle = NULL;

    /* We only send message for new subscription after MAMA_REFRESHINTERVALMIN.
     * The times in the info structures are initialized to
     * MAMA_REFRESHINTERVALRAND.
     */
    time_t dontRefreshBefore =
        curTime + (MAMA_REFRESHINTERVALRAND - MAMA_REFRESHINTERVALMIN)*60;

    if (impl->mMamaTransport)
        throttle = mamaTransportImpl_getThrottle (impl->mMamaTransport,
                                               MAMA_THROTTLE_DEFAULT);
    if (throttle) wombatThrottle_lock (throttle);

    list_lock (impl->mNewListeners);/* Always lock new listeners first */
     cur  = (SubscriptionInfo*)list_get_head (impl->mNewListeners);
     last = (SubscriptionInfo*)list_get_tail (impl->mNewListeners);

    newListenerCount = list_size (impl->mNewListeners);

    /* Make sure that we need to process new subscriptions */
    if (list_size (impl->mNewListeners) == 0 ||
        cur->mNextRefreshTime > dontRefreshBefore)
    {
        mama_log (MAMA_LOG_LEVEL_FINER, "No refreshes to send");
        list_unlock (impl->mNewListeners);
        if (throttle)
            wombatThrottle_unlock (throttle);
        return;
    }

    /* We also need to introduce a random component. We compute the number of
     * intervals between now and the time that the last message must go out. We
     * use this value to compute the number of messages required/interval, and
     * then choose a random value.
     */
    intervals = (last->mNextRefreshTime - curTime)/REFRESH_GRANULARITY;

    /*Just in case the interval is 0 due to truncation when
     last->mNextRefreshTime - curTime < REFRESH_GRANULARITY*/
    if (intervals == 0)
    {
        mama_log (MAMA_LOG_LEVEL_FINER,"processNewSubscriptions () interval was 0");
        mama_log (MAMA_LOG_LEVEL_FINER,"last->mNextRefreshTime [%d] curTime [%d]",
                                        last->mNextRefreshTime, curTime);

        intervals = 1;
    }

    messagesPerInterval = newListenerCount/intervals;

     /*
     * Multiply by 2 so on average we send the correct number of messages.
     */
    maxMessages = 2*( 1 + ((double)rand ()/(double)RAND_MAX) *
                      messagesPerInterval);

    mama_log (MAMA_LOG_LEVEL_FINER, "Sending some refreshes");

    /* We handle new subscriptions separately to introduce a random componenent.
     */
    list_lock (impl->mListeners);

    while (NULL != (cur = (SubscriptionInfo*)list_get_head (impl->mNewListeners)))
    {
        /*
         * All immediate refreshes sent. and random number of refreshes
         * that are > than MAMA_REFRESHINTERVALMIN old.
         */

        if ((cur->mNextRefreshTime > curTime + REFRESH_GRANULARITY) &&
            (cur->mNextRefreshTime > dontRefreshBefore ||  sent > maxMessages))
        {
            break;
        }

        ++sent;


        /* refreshMsg destroyed when sent with throttle. */
        msgUtils_createRefreshMsg (cur->mSubscription, &refreshMsg);
        mamaSubscription_sendRefresh (cur->mSubscription, refreshMsg);

        /* Take it off the new list and put it on the main list */
        list_pop_front (impl->mNewListeners);
        cur->mNextRefreshTime = time (NULL) + MAMA_REFRESHINTERVALRAND * 60;
        cur->mIsInMainList = 1;
        list_push_back (impl->mListeners, cur);
    }

    list_unlock (impl->mListeners);
    list_unlock (impl->mNewListeners);

    if (throttle)
            wombatThrottle_unlock (throttle);

}

static
void refreshTransportImpl_doRefresh (refreshTransportImpl *impl)
{
    mamaMsg refreshMsg;
    SubscriptionInfo *cur;
    time_t curTime = time (NULL);
    wombatThrottle  throttle = NULL;
    /* We must send refreshes for all messages with mNextRefreshTime <=
     * this value.
     */
    time_t refreshUntil = curTime + REFRESH_GRANULARITY;

    mama_log (MAMA_LOG_LEVEL_FINER, "Starting refresh message mini-cycle");

    /* We handle new subscriptions separately to introduce a random componenent.
     */
    refreshTransportImpl_processNewSubscriptions (impl);

    if (impl->mMamaTransport)
        throttle = mamaTransportImpl_getThrottle (impl->mMamaTransport,
                                               MAMA_THROTTLE_DEFAULT);
    if (throttle) wombatThrottle_lock (throttle);

    list_lock (impl->mListeners);

    /*
     * Uncomment this to verify that we maintain the list in
     * the correct order.
     */
    /*last = 0;
    list_for_each (impl->mListeners, checkOrder, NULL); */

    while (NULL != (cur = (SubscriptionInfo*)list_get_head (impl->mListeners)))
    {
        /*
         * All immediate refreshes sent, and random number of refreshes
         * that are > than MI
         */
        if (cur->mNextRefreshTime > refreshUntil)
        {
                break;
        }

        /* refreshMsg destroyed when sent with throttle. */
        msgUtils_createRefreshMsg (cur->mSubscription, &refreshMsg);
        mamaSubscription_sendRefresh (cur->mSubscription, refreshMsg);


        /* Move to the end */
        list_pop_front (impl->mListeners);
        cur->mNextRefreshTime = time (NULL) + MAMA_REFRESHINTERVALRAND * 60;
        list_push_back (impl->mListeners, cur);
    }

    list_unlock (impl->mListeners);
    if (throttle)
            wombatThrottle_unlock (throttle);
}

/**
 * Put the SubscriptionInfo for this handle to the end of the list.
 *
 * @throws ClassCastException If the object is not a valid MdrvListEntry
 * @param handle
 */
extern void
refreshTransport_resetRefreshForListener (refreshTransportImpl* impl, void* handle)
{
    SubscriptionInfo* info = (SubscriptionInfo*)handle;

    refreshTransport_removeListener (impl, handle, 0);

    info->mIsInMainList = 1;
    list_lock (impl->mListeners);
    info->mNextRefreshTime =  time (NULL) + MAMA_REFRESHINTERVALRAND * 60 -1;
    list_push_back (impl->mListeners, handle);
    list_unlock (impl->mListeners);
}

extern void
refreshTransport_removeListener (refreshTransportImpl* impl,  void *handle,
    int freeElement)
{
    SubscriptionInfo* info = (SubscriptionInfo*)handle;

    /* Lock both lists. The remove will remove it from either list */
    list_lock (impl->mNewListeners); /* Always lock new listeners first */
    list_lock (impl->mListeners);

    if (info->mIsInMainList)
    {
        list_remove_element (impl->mListeners, handle);
        if (freeElement)
        {
            list_free_element  (impl->mListeners, handle);
        }
    }
    else
    {
        list_remove_element (impl->mNewListeners, handle);
        if (freeElement)
        {
            list_free_element (impl->mNewListeners, handle);
        }
    }


    list_unlock (impl->mListeners);
    list_unlock (impl->mNewListeners);
}

long refreshTransport_numListeners (refreshTransport transport)
{
    refreshTransportImpl *impl = (refreshTransportImpl*)transport;
    return list_size (impl->mNewListeners) + list_size (impl->mListeners);
}

void refreshTransport_iterateListeners (refreshTransport transport,
                                       wListCallback cb,
                                       void *closure)
{
    refreshTransportImpl *impl = (refreshTransportImpl*)transport;

    list_lock (impl->mNewListeners);
    list_lock (impl->mListeners);

    list_for_each (impl->mListeners, cb, closure);
    list_for_each (impl->mNewListeners, cb, closure);

    list_unlock (impl->mListeners);
    list_unlock (impl->mNewListeners);
}
