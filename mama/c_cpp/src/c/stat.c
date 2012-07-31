/* $Id: stat.c,v 1.8.22.5 2011/09/21 13:47:45 ianbell Exp $
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
#include <string.h>
#include <stddef.h>
#include "wombat/wincompat.h"
#include "mama/types.h"
#include "mama/mama.h"
#include "mama/statscollector.h"
#include "mama/stat.h"
#include "statinternal.h"

typedef struct mamaStatImpl__
{
    const char*         mName;
    mama_fid_t          mFid;
    mamaStatsCollector* mStatsCollector;
    mama_u32_t          mIntervalValue;
    mama_u32_t          mMaxValue;
    mama_u32_t          mTotalValue;
    int                 mLockable;
    void*               mPollStatClosureData;
    pollStatCb          mPollStatCallback;
    int                 mPublish;
    int                 mLog;
    wthread_mutex_t     mUpdateMutex;
} mamaStatImpl;

mama_status
mamaStat_create (mamaStat* stat, mamaStatsCollector* statsCollector, int lockable, const char* name, mama_fid_t fid)
{
    mamaStatImpl* impl = (mamaStatImpl*) malloc (sizeof(mamaStatImpl));
    if (impl == NULL) return MAMA_STATUS_NOMEM;

    impl->mName            = strdup(name);
    impl->mFid             = fid;
    impl->mStatsCollector  = statsCollector;
    impl->mIntervalValue   = 0;
    impl->mMaxValue        = 0;
    impl->mTotalValue      = 0;
    impl->mPollStatCallback = NULL;
    impl->mPublish         = 1;
    impl->mLog             = 1;
    impl->mLockable = lockable;

    if (lockable)
        wthread_mutex_init (&impl->mUpdateMutex, NULL);

    *stat = (mamaStat)impl;
    mamaStatsCollector_addStat (*impl->mStatsCollector, *stat);
    return MAMA_STATUS_OK;
}

mama_status
mamaStat_destroy (mamaStat stat)
{
    mamaStatImpl* impl = (mamaStatImpl*)stat;
    if (impl == NULL) return MAMA_STATUS_NULL_ARG;

    if (impl->mName)
    {
        free ((char*)impl->mName);
        impl->mName = NULL;
    }

    if (impl->mLockable)
        wthread_mutex_destroy (&impl->mUpdateMutex);

    impl->mStatsCollector  = NULL;
    free (impl);

    return MAMA_STATUS_OK;
}

mama_status
mamaStat_increment (mamaStat stat)
{
    mamaStatImpl* impl = (mamaStatImpl*)stat;
    if (!impl) return MAMA_STATUS_NULL_ARG;

    if (impl->mLockable)
        wthread_mutex_lock (&impl->mUpdateMutex);

    impl->mIntervalValue++;
    if (impl->mIntervalValue > impl->mMaxValue)
    {
        impl->mMaxValue = impl->mIntervalValue;
    }
    impl->mTotalValue++;

    if (impl->mLockable)
        wthread_mutex_unlock (&impl->mUpdateMutex);

    return MAMA_STATUS_OK;
}

mama_status
mamaStat_decrement (mamaStat stat)
{
    mamaStatImpl* impl = (mamaStatImpl*)stat;
    if (!impl) return MAMA_STATUS_NULL_ARG;

    if (impl->mLockable)
    {
        wthread_mutex_lock (&impl->mUpdateMutex);
    }

    impl->mIntervalValue--;

    if (impl->mLockable)
    {
        wthread_mutex_unlock (&impl->mUpdateMutex);
    }

    return MAMA_STATUS_OK;
}

mama_status
mamaStat_reset (mamaStat stat)
{
    mamaStatImpl* impl = (mamaStatImpl*)stat;
    if (!impl) return MAMA_STATUS_NULL_ARG;

    if (impl->mLockable)
    {
        wthread_mutex_lock (&impl->mUpdateMutex);
    }

    impl->mIntervalValue = 0;

    if (impl->mLockable)
    {
        wthread_mutex_unlock (&impl->mUpdateMutex);
    }
    return MAMA_STATUS_OK;
}

mama_fid_t
mamaStat_getFid (mamaStat stat)
{
    mamaStatImpl* impl = (mamaStatImpl*)stat;

    return impl->mFid;
}

const char*
mamaStat_getName (mamaStat stat)
{
    mamaStatImpl* impl = (mamaStatImpl*)stat;

    return impl->mName;
}

mama_status
mamaStat_setIntervalValueFromTotal (mamaStat stat, mama_u32_t value)
{
    mamaStatImpl* impl = (mamaStatImpl*)stat;
    if (impl == NULL) return MAMA_STATUS_NULL_ARG;

    /* Haven't updated mTotalValue - so (newTotal - currentTotal)
       will give value for interval */
    impl->mIntervalValue = (value - impl->mTotalValue);
    impl->mTotalValue = value;

    if (impl->mIntervalValue > impl->mMaxValue)
    {
        impl->mMaxValue = impl->mIntervalValue;
    }

    return MAMA_STATUS_OK;
}

int
mamaStat_getIntervalValue (mamaStat stat)
{
    mamaStatImpl* impl = (mamaStatImpl*)stat;

    if (impl->mPollStatCallback != NULL)
    {
        impl->mIntervalValue = impl->mPollStatCallback (impl->mPollStatClosureData);
        if (impl->mIntervalValue > impl->mMaxValue)
        {
            impl->mMaxValue = impl->mIntervalValue;
        }
    }

    return impl->mIntervalValue;
}

int
mamaStat_getMaxValue (mamaStat stat)
{
    mamaStatImpl* impl = (mamaStatImpl*)stat;

    int current = 0;
    if (impl->mPollStatCallback != NULL)
    {
        current = impl->mPollStatCallback (impl->mPollStatClosureData);
        if (current > impl->mMaxValue)
        {
            impl->mMaxValue = current;
        }
    }

    return impl->mMaxValue;
}

int
mamaStat_getTotalValue (mamaStat stat)
{
    mamaStatImpl* impl = (mamaStatImpl*)stat;

    return impl->mTotalValue;
}

void
mamaStat_getStats (mamaStat stat, mama_u32_t* intervalValue, mama_u32_t* maxValue, mama_u32_t* totalValue)
{
    mamaStatImpl* impl = (mamaStatImpl*)stat;

    if (impl->mPollStatCallback != NULL)
    {
        impl->mIntervalValue = impl->mPollStatCallback (impl->mPollStatClosureData);
    }
    *intervalValue  = impl->mIntervalValue;

    /* maxValue is normally incremented in mamaStat_increment - this will not be the
       case in 'polled' stats (eg queue size), so need to check here */
    if (impl->mPollStatCallback != NULL)
    {
        if (impl->mIntervalValue > impl->mMaxValue)
        {
            impl->mMaxValue = impl->mIntervalValue;
        }
    }

    *maxValue       = impl->mMaxValue;
    *totalValue     = impl->mTotalValue;
}

mama_status
mamaStat_setPollCallback (mamaStat stat, pollStatCb cb, void* closure)
{
    mamaStatImpl* impl = (mamaStatImpl*)stat;
    if (!impl) return MAMA_STATUS_NULL_ARG;

    impl->mPollStatClosureData = closure;
    impl->mPollStatCallback = cb;

    return MAMA_STATUS_OK;
}

mama_status
mamaStat_setPublish (mamaStat stat, int publish)
{
    mamaStatImpl* impl = (mamaStatImpl*)stat;
    if (!impl) return MAMA_STATUS_NULL_ARG;

    impl->mPublish = publish;

    return MAMA_STATUS_OK;
}

int
mamaStat_getPublish (mamaStat stat)
{
    mamaStatImpl* impl = (mamaStatImpl*)stat;

    return impl->mPublish;
}

mama_status
mamaStat_setLog (mamaStat stat, int log)
{
    mamaStatImpl* impl = (mamaStatImpl*)stat;
    if (!impl) return MAMA_STATUS_NULL_ARG;

    impl->mLog = log;

    return MAMA_STATUS_OK;
}

int
mamaStat_getLog (mamaStat stat)
{
    mamaStatImpl* impl = (mamaStatImpl*)stat;

    return impl->mLog;
}

mamaStatType
mamaStatType_fromString (const char* statTypeString)
{
    if ((strcmp (statTypeString, "Initials")) == 0)
    {
        return MAMA_STAT_TYPE_INITIALS;
    }
    if((strcmp (statTypeString, "Recaps")) == 0)
    {
        return MAMA_STAT_TYPE_RECAPS;
    }
    if ((strcmp (statTypeString, "Messages")) == 0)
    {
        return MAMA_STAT_TYPE_NUM_MESSAGES;
    }
    if ((strcmp (statTypeString, "FT Takeovers")) == 0)
    {
        return MAMA_STAT_TYPE_FT_TAKEOVERS;
    }
    if ((strcmp (statTypeString, "Queue Size")) == 0)
    {
        return MAMA_STAT_TYPE_QUEUE_SIZE;
    }
    if ((strcmp (statTypeString, "Subscriptions")) == 0)
    {
        return MAMA_STAT_TYPE_SUBSCRIPTIONS;
    }
    if ((strcmp (statTypeString, "Timeouts")) == 0)
    {
        return MAMA_STAT_TYPE_TIMEOUTS;
    }
    if ((strcmp (statTypeString, "Message Index")) == 0)
    {
        return MAMA_STAT_TYPE_MSG_INDEX;
    }
    if ((strcmp (statTypeString, "Nak Packets Sent")) == 0)
    {
        return MAMA_STAT_TYPE_NAK_PACKETS_SENT;
    }
    if ((strcmp (statTypeString, "Naks Sent")) == 0)
    {
        return MAMA_STAT_TYPE_NAKS_SENT;
    }
    if ((strcmp (statTypeString, "Messages Lost")) == 0)
    {
        return MAMA_STAT_TYPE_MSGS_LOST;
    }
    if ((strcmp (statTypeString, "Ncfs Ignore")) == 0)
    {
        return MAMA_STAT_TYPE_NCFS_IGNORE;
    }
    if ((strcmp (statTypeString, "Ncfs Shed")) == 0)
    {
        return MAMA_STAT_TYPE_NCFS_SHED;
    }
    if ((strcmp (statTypeString, "Ncfs Retransmit Delay")) == 0)
    {
        return MAMA_STAT_TYPE_NCFS_RX_DELAY;
    }
    if ((strcmp (statTypeString, "Ncfs Unknown")) == 0)
    {
        return MAMA_STAT_TYPE_NCFS_UNKNOWN;
    }
    if ((strcmp (statTypeString, "Duplicate Messages")) == 0)
    {
        return MAMA_STAT_TYPE_DUPLICATE_MSGS;
    }
    if ((strcmp (statTypeString, "Window Advance Msgs Unrecoverable")) == 0)
    {
        return MAMA_STAT_TYPE_UNRECOVERABLE_WINDOW_ADVANCE;
    }
    if ((strcmp (statTypeString, "Timeout Msgs Unrecoverable")) == 0)
    {
        return MAMA_STAT_TYPE_UNRECOVERABLE_TIMEOUT;
    }
    if ((strcmp (statTypeString, "LBM Msgs Received With No Topic")) == 0)
    {
        return MAMA_STAT_TYPE_LBM_MSGS_RECEIVED_NO_TOPIC;
    }
    if ((strcmp (statTypeString, "LBM Requests Received")) == 0)
    {
        return MAMA_STAT_TYPE_LBM_REQUESTS_RECEIVED;
    }
    if ((strcmp (statTypeString, "Wombat Messages")) == 0)
    {
        return MAMA_STAT_TYPE_WOMBAT_MSGS;
    }
    if ((strcmp (statTypeString, "RV Messages")) == 0)
    {
        return MAMA_STAT_TYPE_RV_MSGS;
    }
    if ((strcmp (statTypeString, "FAST Messages")) == 0)
    {
        return MAMA_STAT_TYPE_FAST_MSGS;
    }

    return MAMA_STAT_TYPE_UNKNOWN;
}


const char*
mamaStatType_toString (mamaStatType statType)
{
    switch (statType)
    {
        case MAMA_STAT_TYPE_INITIALS                     : return "Initials";
                                                           break;
        case MAMA_STAT_TYPE_RECAPS                       : return "Recaps";
                                                           break;
        case MAMA_STAT_TYPE_NUM_MESSAGES                 : return "Messages";
                                                           break;
        case MAMA_STAT_TYPE_FT_TAKEOVERS                 : return "FT Takeovers";
                                                           break;
        case MAMA_STAT_TYPE_QUEUE_SIZE                   : return "Queue Size";
                                                           break;
        case MAMA_STAT_TYPE_SUBSCRIPTIONS                : return "Subscriptions";
                                                           break;
        case MAMA_STAT_TYPE_TIMEOUTS                     : return "Timeouts";
                                                           break;
        case MAMA_STAT_TYPE_MSG_INDEX                    : return "Message Index";
                                                           break;
        case MAMA_STAT_TYPE_NAK_PACKETS_SENT             : return "Nak Packets Sent";
                                                           break;
        case MAMA_STAT_TYPE_NAKS_SENT                    : return "Naks Sent";
                                                           break;
        case MAMA_STAT_TYPE_MSGS_LOST                    : return "Messages Lost";
                                                           break;
        case MAMA_STAT_TYPE_NCFS_IGNORE                  : return "Ncfs Ignore";
                                                           break;
        case MAMA_STAT_TYPE_NCFS_SHED                    : return "Ncfs Shed";
                                                           break;
        case MAMA_STAT_TYPE_NCFS_RX_DELAY                : return "Ncfs Retransmit Delay";
                                                           break;
        case MAMA_STAT_TYPE_NCFS_UNKNOWN                 : return "Ncfs Unknown";
                                                           break;
        case MAMA_STAT_TYPE_DUPLICATE_MSGS               : return "Duplicate Messages";
                                                           break;
        case MAMA_STAT_TYPE_UNRECOVERABLE_WINDOW_ADVANCE : return "Window Advance Msgs Unrecoverable";
                                                           break;
        case MAMA_STAT_TYPE_UNRECOVERABLE_TIMEOUT        : return "Timeout Msgs Unrecoverable";
                                                           break;
        case MAMA_STAT_TYPE_LBM_MSGS_RECEIVED_NO_TOPIC   : return "LBM Msgs Received With No Topic";
                                                           break;
        case MAMA_STAT_TYPE_LBM_REQUESTS_RECEIVED        : return "LBM Requests Received";
                                                           break;
        case MAMA_STAT_TYPE_WOMBAT_MSGS                  : return "Wombat Messages";
                                                           break;
        case MAMA_STAT_TYPE_RV_MSGS                      : return "RV Messages";
                                                           break;
        case MAMA_STAT_TYPE_FAST_MSGS                    : return "FAST Messages";
                                                           break;
        default                                          : return "Unknown";
                                                           break;
    }
}
