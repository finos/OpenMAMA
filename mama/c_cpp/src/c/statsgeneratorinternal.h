/* $Id: statsgeneratorinternal.h,v 1.3.20.4 2011/08/10 14:53:26 nicholasmarriott Exp $
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

#ifndef MamaStatsGeneratorInternalH__
#define MamaStatsGeneratorInternalH__

#include "mama/status.h"
#include "mama/types.h"

#if defined (__cplusplus)
extern "C" {
#endif

MAMAExpDLL
extern mama_status
mamaStatsGenerator_create (mamaStatsGenerator* statsGenerator, mama_f64_t reportInterval);

/**
 * This function should be called to stop the stats report timer before the internal event
 * queue has been destroyed.
 *
 * @param[in] statsGenerator The stats generator.
 * @returns mama_status can be one of
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaStatsGenerator_stopReportTimer(mamaStatsGenerator statsGenerator);
MAMAExpDLL
extern mama_status
mamaStatsGenerator_destroy (mamaStatsGenerator statsGenerator);

MAMAExpDLL
extern mama_status
mamaStatsGenerator_removeStatsCollector (mamaStatsGenerator statsGenerator, mamaStatsCollector* statsCollector);

MAMAExpDLL
extern mama_status
mamaStatsGenerator_setLogStats (mamaStatsGenerator statsGenerator, int logStats);

MAMAExpDLL
extern mama_status
mamaStatsGenerator_setStatsLogger (mamaStatsGenerator statsGenerator, mamaStatsLogger* statsLogger);

MAMAExpDLL
extern void
mamaStatsGenerator_generateStats (mamaStatsGenerator statsgenerator);

MAMAExpDLL
extern mama_status
mamaStatsGenerator_setQueue (mamaStatsGenerator statsgenerator, mamaQueue queue);

MAMAExpDLL
extern void*
mamaStatsGenerator_allocateStatsCollector (mamaStatsGenerator statsGenerator);

MAMAExpDLL
extern mama_status
mamaStatsGenerator_addStatsCollector (mamaStatsGenerator statsGenerator, mamaStatsCollector* statsCollector);

#if defined (__cplusplus)
}
#endif

#endif /* MamaStatsGeneratorInternalH__ */
