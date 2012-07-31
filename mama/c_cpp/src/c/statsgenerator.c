/* $Id: statsgenerator.c,v 1.8.20.5 2011/08/18 10:54:08 ianbell Exp $
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

#include "list.h"
#include "mama/log.h"
#include "mama/types.h"
#include "mama/mama.h"
#include "mama/statslogger.h"
#include "mama/statscollector.h"
#include "statscollectorinternal.h"
#include "statsgeneratorinternal.h"
#include "mama/statfields.h"

#define LOG_HEADER      "                    Name |      Type | Middleware |       Statistic |       Rate |        Max |      Total |"
#define LOG_SEPARATOR   "-------------------------+-----------+------------+-----------------+------------+------------+------------+"

typedef struct mamaStatsGeneratorImpl__
{
    wList               mStatsCollectors;
    mamaStatsLogger*    mStatsLogger;
    int                 mLogStats;
    mamaTimer           mReportTimer;
    mama_f64_t          mReportInterval;
    mamaMsg             mStatMsg;
    mamaQueue           mQueue;
} mamaStatsGeneratorImpl;


static void MAMACALLTYPE
mamaStatsGenerator_onReportTimer (mamaTimer timer, void* closure);

mama_status
mamaStatsGenerator_create (mamaStatsGenerator* statsGenerator, mama_f64_t interval)
{
    mama_status status = MAMA_STATUS_OK;
    mamaStatsGeneratorImpl* impl = (mamaStatsGeneratorImpl*) malloc (sizeof(mamaStatsGeneratorImpl));
    if (impl==NULL) return MAMA_STATUS_NOMEM;

    impl->mStatsLogger      = NULL;
    impl->mLogStats         = 0;
    impl->mReportInterval   = interval;
    impl->mQueue            = NULL;
    impl->mReportTimer      = NULL;

    impl->mStatsCollectors  = list_create (sizeof(mamaStatsCollector));

    status = mamaMsg_create (&impl->mStatMsg);
    if (MAMA_STATUS_OK != status) return status;

    *statsGenerator = (mamaStatsGenerator)impl;
    return MAMA_STATUS_OK;
}

mama_status
mamaStatsGenerator_destroy (mamaStatsGenerator statsGenerator)
{
    mamaStatsGeneratorImpl* impl = (mamaStatsGeneratorImpl*)statsGenerator;

    if(impl)
    {
    impl->mStatsLogger = NULL;

    if (impl->mStatMsg != NULL)
    {
        mamaMsg_destroy (impl->mStatMsg);
        impl->mStatMsg = NULL;
    }

    if (impl->mStatsCollectors != NULL)
    {
        list_destroy (impl->mStatsCollectors, NULL, NULL);
    }

    free (impl);

    }
    return MAMA_STATUS_OK;
}

mama_status
mamaStatsGenerator_setQueue (mamaStatsGenerator statsGenerator, mamaQueue queue)
{
    mama_status status = MAMA_STATUS_OK;
    mamaStatsGeneratorImpl* impl = (mamaStatsGeneratorImpl*)statsGenerator;
    if (impl == NULL) return MAMA_STATUS_NULL_ARG;

    if (!(impl->mQueue = queue))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "mamaStatsGenerator_setQueue(): "
                  "No queue specified");
        return MAMA_STATUS_NULL_ARG;
    }

    if (MAMA_STATUS_OK != (status = mamaTimer_create (&impl->mReportTimer,
                                    impl->mQueue,
                                    mamaStatsGenerator_onReportTimer,
                                    impl->mReportInterval,
                                    (mamaStatsGenerator)impl)))
    {
        return status;
    }

    return status;
}

mama_status
mamaStatsGenerator_addStatsCollector (mamaStatsGenerator statsGenerator, mamaStatsCollector* statsCollector)
{
    mamaStatsGeneratorImpl* impl = (mamaStatsGeneratorImpl*)statsGenerator;

    if (impl==NULL) return MAMA_STATUS_NULL_ARG;
    if (statsCollector == NULL) return MAMA_STATUS_NULL_ARG;

    list_push_back (impl->mStatsCollectors, statsCollector);

    return MAMA_STATUS_OK;
}

mama_status
mamaStatsGenerator_removeStatsCollector (mamaStatsGenerator statsGenerator, mamaStatsCollector* statsCollector)
{
    mamaStatsGeneratorImpl* impl = (mamaStatsGeneratorImpl*)statsGenerator;

    if (impl==NULL) return MAMA_STATUS_NULL_ARG;
    if (statsCollector==NULL) return MAMA_STATUS_NULL_ARG;

    list_remove_element (impl->mStatsCollectors, (void*)statsCollector);
    list_free_element (impl->mStatsCollectors, (void*)statsCollector);

    return MAMA_STATUS_OK;
}

void
mamaStatsGenerator_generateStats (mamaStatsGenerator statsGenerator)
{
    mamaStatsGeneratorImpl* impl = (mamaStatsGeneratorImpl*)statsGenerator;
    mamaStatsCollector* current;
    int wasLogged   = 0;
    int logLast     = 0;

    current = (mamaStatsCollector*)list_get_head (impl->mStatsCollectors);

    /* Stats are logged at WARN so that users don't have to enable NORMAL (or higher)
       logging, even though they aren't actually warnings...*/
    if (impl->mLogStats)
    {
        mama_log (MAMA_LOG_LEVEL_WARN, LOG_SEPARATOR);
        mama_log (MAMA_LOG_LEVEL_WARN, LOG_HEADER);
        mama_log (MAMA_LOG_LEVEL_WARN, LOG_SEPARATOR);
    }

   while (current != NULL)
   {
        wasLogged = 0;

        mamaStatsCollector_populateMsg (current, impl->mStatMsg, &wasLogged);

        if (impl->mStatsLogger && mamaStatsCollector_getPublish(*current))
        {
            mamaStatsLogger_addStatMsg (impl->mStatsLogger, impl->mStatMsg);
        }

        if (wasLogged)
        {
            if (impl->mLogStats && mamaStatsCollector_getLog(*current))
            {
                mama_log (MAMA_LOG_LEVEL_WARN, LOG_SEPARATOR);
            }
        }

        logLast = mamaStatsCollector_getLog(*current);
        current = (mamaStatsCollector*)list_get_next (impl->mStatsCollectors, current);
    }

    if (impl->mStatsLogger)
    {
        mamaStatsLogger_sendReport (impl->mStatsLogger);
    }

    /* If the last collector didn't give us any stats, still log a separator */
    if (!wasLogged && impl->mLogStats && logLast)
    {
        mama_log (MAMA_LOG_LEVEL_WARN, LOG_SEPARATOR);
    }
}

mama_status
mamaStatsGenerator_setStatsLogger (mamaStatsGenerator statsGenerator, mamaStatsLogger* statsLogger)
{
    mamaStatsGeneratorImpl* impl = (mamaStatsGeneratorImpl*)statsGenerator;
    if (impl == NULL) return MAMA_STATUS_NULL_ARG;

    impl->mStatsLogger = statsLogger;

    return MAMA_STATUS_OK;
}

mama_status
mamaStatsGenerator_setLogStats (mamaStatsGenerator statsGenerator, int logStats)
{
    mamaStatsGeneratorImpl* impl = (mamaStatsGeneratorImpl*)statsGenerator;
    if (impl == NULL) return MAMA_STATUS_NULL_ARG;

    impl->mLogStats = logStats;

    return MAMA_STATUS_OK;
}

static void MAMACALLTYPE
mamaStatsGenerator_onReportTimer (mamaTimer timer, void* closure)
{
    mamaStatsGenerator statsgenerator = (mamaStatsGenerator)closure;

    mamaStatsGenerator_generateStats (statsgenerator);
}

void*
mamaStatsGenerator_allocateStatsCollector (mamaStatsGenerator statsGenerator)
{
    mamaStatsGeneratorImpl* impl = (mamaStatsGeneratorImpl*)statsGenerator;

    return list_allocate_element (impl->mStatsCollectors);
}

mama_status mamaStatsGenerator_stopReportTimer(mamaStatsGenerator statsGenerator)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;

    /* Get the impl. */
    mamaStatsGeneratorImpl *impl = (mamaStatsGeneratorImpl*)statsGenerator;
    if(NULL != impl)
    {
        /* Destroy the timer. */
        ret = MAMA_STATUS_OK;
        if(NULL != impl->mReportTimer)
        {
            ret = mamaTimer_destroy(impl->mReportTimer);
            impl->mReportTimer = NULL;
        }
    }

    return ret;
}
