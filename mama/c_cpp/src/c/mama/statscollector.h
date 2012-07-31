/* $Id: statscollector.h,v 1.5.22.3 2011/08/10 14:53:28 nicholasmarriott Exp $
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

#ifndef MamaStatsCollectorH__
#define MamaStatsCollectorH__

#include "mama/status.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef enum mamaStatsCollectorType
{
    MAMA_STATS_COLLECTOR_TYPE_QUEUE     = 0,
    MAMA_STATS_COLLECTOR_TYPE_TRANSPORT = 1,
    MAMA_STATS_COLLECTOR_TYPE_GLOBAL    = 2
} mamaStatsCollectorType;

/**
 * Create a mamaStatsCollector object
 *
 * @param statsCollector  The stats collector object to create
 */
MAMAExpDLL
extern mama_status
mamaStatsCollector_create (mamaStatsCollector* statsCollector, mamaStatsCollectorType type, const char* name, const char* middleware);

/**
 * Destroy a mamaStatsCollector object
 *
 * @param statsCollector  The stats collector object to destroy
 */
MAMAExpDLL
extern mama_status
mamaStatsCollector_destroy (mamaStatsCollector statsCollector);

/**
 * Register a stats object with the collector
 *
 * @param statscollector  The stats collector object to register with
 * @param stat            The stat object to register
 */
MAMAExpDLL
extern mama_status
mamaStatsCollector_addStat (mamaStatsCollector statsCollector, mamaStat stat);

/**
 * Increment the current interval value of the stat represented
 * by FID identifier
 *
 * @param statscollector  The stats collector object for which to increment the stat
 * @param identifier      FID of the stat to increment
 */
MAMAExpDLL
extern mama_status
mamaStatsCollector_incrementStat (mamaStatsCollector statsCollector, mama_fid_t identifier);

/**
 * Set the name of the stats collector object.  By default, this will be
 * the same as the associated queue, transport, or MAMA Application.
 *
 * @param statscollector  The stats collector object for which to set the name
 * @param name            The name to set for the stats collector
 */
MAMAExpDLL
extern mama_status
mamaStatsCollector_setName (mamaStatsCollector statsCollector, const char* name);

/**
 * Set whether or not stats for this stats collector object should be
 * published in stats messages.
 *
 * @param statscollector  The stats collector object to set publishing for
 * @param publish         Whether or not to publish stats for this stats collector
 */
MAMAExpDLL
extern mama_status
mamaStatsCollector_setPublish (mamaStatsCollector statsCollector, int publish);

/**
 * Get whether or not stats are being published for this stats collector.
 *
 * @param statscollector  The stats collector object to get publishing for
 */
MAMAExpDLL
extern int
mamaStatsCollector_getPublish (mamaStatsCollector statsCollector);

/**
 * Set whether or not stats for this stats collector object should be
 * logged to the MAMA log.
 *
 * @param statscollector  The stats collector object to set logging for
 * @param log             Whether or not to log stats for this stats collector
 */
MAMAExpDLL
extern mama_status
mamaStatsCollector_setLog (mamaStatsCollector statsCollector, int log);

/**
 * Get whether or not stats are being logged for this stats collector.
 *
 * @param statscollector  The stats collector object to get logging for.
 */
MAMAExpDLL
extern int
mamaStatsCollector_getLog (mamaStatsCollector statsCollector);

/**
 * Return a string representation of a mamaStatsCollectorType
 *
 * @param type  The stats collector type to return as a string
 */
MAMAExpDLL
extern const char*
mamaStatsCollectorType_stringForType (mamaStatsCollectorType type);

#if defined(__cplusplus)
}
#endif

#endif /* MamaStatsCollectorH__ */
