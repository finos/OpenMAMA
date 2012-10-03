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

#ifndef MamaStatsCollectorInternalH__
#define MamaStatsCollectorInternalH__

#include "mama/statscollector.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef void (
*collectorPollStatCb) (mamaStatsCollector statsCollector, void* closure);

#define MAMA_STAT_ARRAY_OFFSET 		105
#define MAMA_STAT_MAX_STATS    		30

typedef struct mamaStatsCollectorImpl__
{
    mamaStatsCollectorType 	mType;
    char* 			        mName;
    mamaStat    			mMamaStats[MAMA_STAT_MAX_STATS];
    char* 			        mMiddleware;
    collectorPollStatCb 	mPollCb;
    void*       			mPollClosure;
    int         			mPublishStats;
    int         			mLogStats;
    void*          			mHandle;
} mamaStatsCollectorImpl;

MAMAExpDLL
extern mama_status
mamaStatsCollector_setPollCallback (mamaStatsCollector statsCollector, collectorPollStatCb cb, void* closure);

/**
 * Populate a reusable message to be published via the stats logger,
 * and a string to be logged via MAMA logging
 *
 * @param statscollector  The stats collector object to gather stats from
 * @param statsStr        A reusable string for MAMA logging
 * @param msg             A reusable message to be published via the Stats Logger
 * @param wasLogged       Whether or not any stats were logged
 */
MAMAExpDLL
extern void
mamaStatsCollector_populateMsg (mamaStatsCollector statsCollector, mamaMsg msg, int* wasLogged);

MAMAExpDLL
extern mama_status
mamaStatsCollector_setStatIntervalValueFromTotal (mamaStatsCollector statsCollector, mama_fid_t identifier, mama_u32_t value);

#if defined(__cplusplus)
}
#endif

#endif /* MamaStatsCollectorInternalH__ */
