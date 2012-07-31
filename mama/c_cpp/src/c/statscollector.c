/* $Id: statscollector.c,v 1.9.4.1.16.2 2011/08/10 14:53:26 nicholasmarriott Exp $
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

#include <string.h>
#include "list.h"
#include "mama/mama.h"
#include "mama/types.h"
#include "mama/stat.h"
#include "statinternal.h"
#include "statscollectorinternal.h"
#include "mama/statscollector.h"
#include "mama/statfields.h"

#define MAMA_STAT_ARRAY_OFFSET 		105
#define MAMA_STAT_MAX_STATS    		30

typedef struct mamaStatsCollectorImpl__
{
    mamaStatsCollectorType 	mType;
    const char* 			mName;
    mamaStat    			mMamaStats[MAMA_STAT_MAX_STATS];
    const char* 			mMiddleware;
    collectorPollStatCb 	mPollCb;
    void*       			mPollClosure;
    int         			mPublishStats;
    int         			mLogStats;
} mamaStatsCollectorImpl;

mama_status
mamaStatsCollector_create (mamaStatsCollector* statsCollector, mamaStatsCollectorType type, const char* name, const char* middleware)
{
    mamaStatsCollectorImpl* impl = (mamaStatsCollectorImpl*)malloc (sizeof(mamaStatsCollectorImpl));
    if (impl == NULL) return MAMA_STATUS_NOMEM;

    impl->mType         = type;
    impl->mName         = strdup(name);
    impl->mMiddleware   = strdup(middleware);
    impl->mPollCb       = NULL;
    impl->mPollClosure  = NULL;
    /* Default behaviour is to log, but not publish, stats */
    impl->mPublishStats = 0;
    impl->mLogStats     = 1;

    memset (impl->mMamaStats, 0, MAMA_STAT_MAX_STATS*sizeof(mamaStat));

    *statsCollector = (mamaStatsCollector)impl;
    return MAMA_STATUS_OK;
}

mama_status
mamaStatsCollector_destroy (mamaStatsCollector statsCollector)
{
    mamaStatsCollectorImpl* impl = (mamaStatsCollectorImpl*)statsCollector;
    if (!impl) return MAMA_STATUS_NULL_ARG;

    free ((char*)impl->mName);
    impl->mName         = NULL;
    free ((char*)impl->mMiddleware);
    impl->mMiddleware   = NULL;
    impl->mPollCb       = NULL;
    impl->mPollClosure  = NULL;

    free (impl);
    return MAMA_STATUS_OK;
}

mama_status
mamaStatsCollector_addStat (mamaStatsCollector statsCollector, mamaStat stat)
{
    int index = 0;
    mamaStatsCollectorImpl* impl = (mamaStatsCollectorImpl*)statsCollector;
    if (impl == NULL) return MAMA_STATUS_NULL_ARG;

    index = mamaStat_getFid (stat) - MAMA_STAT_ARRAY_OFFSET;
    impl->mMamaStats[index] = stat;

    return MAMA_STATUS_OK;
}

mama_status
mamaStatsCollector_incrementStat (mamaStatsCollector statsCollector, mama_fid_t identifier)
{
    int index = 0;
    mamaStatsCollectorImpl* impl = (mamaStatsCollectorImpl*)statsCollector;
    if (impl == NULL) return MAMA_STATUS_NULL_ARG;

    index = identifier - MAMA_STAT_ARRAY_OFFSET;
    mamaStat_increment (impl->mMamaStats[index]);

    return MAMA_STATUS_OK;
}

mama_status
mamaStatsCollector_setStatIntervalValueFromTotal (mamaStatsCollector statsCollector, mama_fid_t identifer, mama_u32_t value)
{
    int index = 0;
    mamaStatsCollectorImpl* impl = (mamaStatsCollectorImpl*)statsCollector;
    if (impl == NULL) return MAMA_STATUS_NULL_ARG;

    index = identifer - MAMA_STAT_ARRAY_OFFSET;
    mamaStat_setIntervalValueFromTotal (impl->mMamaStats[index], value);

    return MAMA_STATUS_OK;
}

void
mamaStatsCollector_populateMsg /*And string logging */ (mamaStatsCollector* statsCollector, mamaMsg msg, int* wasLogged)
{
    mamaStatsCollectorImpl* impl = (mamaStatsCollectorImpl*)*statsCollector;
    mama_fid_t  fid;
    const char* name = NULL;
    const char* type = NULL;
    mama_u32_t  intervalValue = 0;
    mama_u32_t  maxValue      = 0;
    mama_u32_t  totalValue    = 0;
    int i;
    int logHeader   = 1;

    if (!impl->mLogStats && !impl->mPublishStats)
    {
        *wasLogged = 0;
         return;
    }

    mamaMsg_clear (msg);

    type = mamaStatsCollectorType_stringForType (impl->mType);

    mamaMsg_addString (msg, MamaStatName.mName, MamaStatName.mFid, impl->mName);
    mamaMsg_addString (msg, MamaStatType.mName, MamaStatType.mFid, type);
    mamaMsg_addString (msg, MamaStatMiddleware.mName, MamaStatMiddleware.mFid, impl->mMiddleware);

    if (impl->mPollCb != NULL)
    {
        impl->mPollCb (*statsCollector, impl->mPollClosure);
    }

    for (i=0;i<MAMA_STAT_MAX_STATS;i++)
    {
        if (impl->mMamaStats[i] != NULL) /*Not all stats collectors will have all stats - eg transports will not have queue size*/
        {
            if (mamaStat_getPublish(impl->mMamaStats[i]))
            {
                fid   = mamaStat_getFid (impl->mMamaStats[i]);
                name  = mamaStat_getName (impl->mMamaStats[i]);

                mamaStat_getStats (impl->mMamaStats[i], &intervalValue, &maxValue, &totalValue);
                mamaMsg_addU32 (msg, name, fid, intervalValue);
            }

            if (impl->mLogStats && mamaStat_getLog (impl->mMamaStats[i]))
            {
                if (intervalValue > 0 || maxValue > 0 || totalValue > 0)
                {
                    /* Stats are logged at WARN so that users don't have to enable NORMAL (or higher)
                       logging, even though they aren't actually warnings...*/
                       mama_log (MAMA_LOG_LEVEL_WARN, "%24.24s | %9.9s | %10.10s | %15.15s | %10u | %10u | %10u |",
                                                      logHeader ? impl->mName : "",
                                                      logHeader ? type : "",
                                                      logHeader ? impl->mMiddleware : "",
                                                      name,
                                                      intervalValue,
                                                      maxValue,
                                                      totalValue);

                    if (logHeader) logHeader = 0;
                    if (!(*wasLogged)) *wasLogged = 1;
                }
            }

            mamaStat_reset (impl->mMamaStats[i]);
        }
    }
}

mama_status
mamaStatsCollector_setName (mamaStatsCollector statsCollector, const char* name)
{
    mamaStatsCollectorImpl* impl = (mamaStatsCollectorImpl*)statsCollector;
    if (impl == NULL) return MAMA_STATUS_NULL_ARG;

    if (impl->mName)
    {
        free ((char*)(impl->mName));
    }

    impl->mName = strdup(name);

    return MAMA_STATUS_OK;
}

mama_status
mamaStatsCollector_setPollCallback (mamaStatsCollector statsCollector, collectorPollStatCb cb, void* closure)
{
    mamaStatsCollectorImpl* impl = (mamaStatsCollectorImpl*)statsCollector;
    if (impl == NULL) return MAMA_STATUS_NULL_ARG;

    impl->mPollCb = cb;
    impl->mPollClosure = closure;

    return MAMA_STATUS_OK;
}

mama_status
mamaStatsCollector_setPublish (mamaStatsCollector statsCollector, int publish)
{
    mamaStatsCollectorImpl* impl = (mamaStatsCollectorImpl*)statsCollector;
    if (impl == NULL) return MAMA_STATUS_NULL_ARG;

    impl->mPublishStats = publish;

    return MAMA_STATUS_OK;
}

int
mamaStatsCollector_getPublish (mamaStatsCollector statsCollector)
{
    mamaStatsCollectorImpl* impl = (mamaStatsCollectorImpl*)statsCollector;
    if (impl == NULL) return MAMA_STATUS_NULL_ARG;

    return impl->mPublishStats;
}

mama_status
mamaStatsCollector_setLog (mamaStatsCollector statsCollector, int log)
{
    mamaStatsCollectorImpl* impl = (mamaStatsCollectorImpl*)statsCollector;
    if (impl == NULL) return MAMA_STATUS_NULL_ARG;

    impl->mLogStats = log;

    return MAMA_STATUS_OK;
}

int
mamaStatsCollector_getLog (mamaStatsCollector statsCollector)
{
    mamaStatsCollectorImpl* impl = (mamaStatsCollectorImpl*)statsCollector;
    if (impl == NULL) return MAMA_STATUS_NULL_ARG;

    return impl->mLogStats;
}

const char*
mamaStatsCollectorType_stringForType (mamaStatsCollectorType type)
{
    switch (type)
    {
        case MAMA_STATS_COLLECTOR_TYPE_QUEUE     : return "Queue";
        case MAMA_STATS_COLLECTOR_TYPE_TRANSPORT : return "Transport";
        case MAMA_STATS_COLLECTOR_TYPE_GLOBAL    : return "Global";
        default : return "Unknown";
    }
}
