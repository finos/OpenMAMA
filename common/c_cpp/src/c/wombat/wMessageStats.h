/* $Id: wMessageStats.h,v 1.7.16.3.8.1 2012/04/17 10:52:12 emmapollock Exp $
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

#ifndef _PERF_DATA_H__
#define _PERF_DATA_H__

#if defined(__cplusplus)
    extern "C" {
#endif /* __cplusplus */

#include "machine.h"
#include <stdio.h>
#ifndef WIN32
#include <sys/time.h>
#endif

#ifdef WIN32

#include <winsock2.h>

#endif

/*******************************************************************************
*******************************************************************************/


struct perfData_t;
typedef struct perfData_t perfData;

struct statsCache_t;
typedef struct statsCache_t statsCache;

struct processInfo_t;
typedef struct processInfo_t processInfo;
 
typedef struct latencyVals
{
    double pubTimeMilliSecs;     /*time msg was published in milliseconds*/
    double recieveTimeMilliSecs; /*time msg was recieved in milliseconds*/
    double latencyMilliSecs;     /*latency between two previous vals*/
}latencyVals;

typedef struct performanceData
{
    const char* mSymbol;
    double mTotalTime;
    long   mMsgCountP;
    double mMsgPerSecP;
    long   mByteCountP;
    double mBytePerSecP;
    double mMinLatencyP;
    double mMaxLatencyP;
    double mAveLatencyP;
    double mStdDeviationP;
}performanceData;

enum
{
    STATS_OK                =   0,
    STATS_FAILURE_GENERAL   =  -1,
    STATS_NULL_PARAM        =   1,
    STATS_NO_MEMORY         =   2
};

/* used to create instance of struct to hold stats data
** @param statsCache        -- caches relevant statistics data
**
** @param numMsgCategories  -- 0 if stats for all symbols needed or number of
**                             symbols if stats are needed on per symbol basis
**
** @param outfile           -- where stats are to be displayed
**
** @param header            -- 1 to display header 0 for never
**
** returns STATS_NO_MEMORY or STATS_OK
*/
COMMONExpDLL
int createStatisticsCache(statsCache** sCache,
                            int numMsgCategories, 
                            FILE* outfile,
                            int header); 



/* used to gather stats at user specified intervals */
/*@param myPerformanceData  -- if user wants results in performanceData 
**                             structure pass in pointer to it, pass in null if
**                             not, wMessageStats.c will then print results to 
**                             FILE* outfile that was passed in on creation of 
**                             statsCache.
*/
COMMONExpDLL
void statisticsCacheToFile(statsCache* sCache,
                           performanceData* myPerformanceData);



/* stores stats for individual symbols and counts 
** messages and bytes between intervals
** @param msgCategoryName   -- name by which message or symbol
**                             is to be identified
** @param numBytesRecieved  -- size of message in Bytes
** @param timeSecs          -- time in secs from epoch until now
** @param timeMicroSecs     -- time in micro seconds from last second
** @param cLatency          -- if latency is already known pass it in,
**                             set timeSecs + timeMicroSecs = 0
** @param tv                -- time on which latency will be calculated
**                             i.e time message was recieved.
*/
COMMONExpDLL
int updateStatisticsCache(statsCache* sCache,
                          const char* msgCategoryName,
                                long  numBytesRecieved,
                                long  timeSecs,
                                long  timeMicroSecs,
                              double  cLatency,
                              struct  timeval tv);

COMMONExpDLL
void destroyStatsCache(statsCache*);

/* prints end of test report to file */
COMMONExpDLL
int printPerfReport(statsCache* sCache, FILE*);



/* function calculates latency between timeSecs,timeMicroSecs 
** and time of calling function. returns latencyVals structure.
** @param timeSecs      -- time in seconds since the epoch 
** @param timeMicroSecs -- time past the last second in micro seconds
** @param latency       -- puts latency value in double* latencyi
** @param tv            -- time on which latency will be calculated
**                         i.e time message was recieved.
*/
COMMONExpDLL
latencyVals calcLatency(long   timeSecs,
                        long   timeMicroSecs,
                     double*   latency,
              struct timeval   tv);


/* function calculates latency on timestamp and puts result in double* latency
** @param timeStamp     -- timestamp string
** @param timeFormat    -- format of timeStamp string ie "%d:%m:%Y:%H:%M:%S"
** @param latency       -- poiner to double where latency value
**                         is to be stored
** @param tv            -- time on which latency will be calculated
**                         i.e time message was recieved.
*/
COMMONExpDLL
latencyVals calcLatency1TimeStamp(const char* timeStamp,
                                  const char* timeFormat,
                                      double* latency,
                               struct timeval tv);

/* Function to print statsLevel 1 values 
** @param timeSecs      -- time in seconds since the epoch
** @param timeMicroSecs -- time past the last second in micro seconds
** @param numMsg        -- when using statsLevel 1, numMsg defines how often to
**                         calculate results.
** @graphData           -- where you want data to be displayed ie
**                         stdout or specify a directory /home/username/
*/


COMMONExpDLL
void printStatsLevel1(int numMsg, FILE* graphData);
/* function prints out to graphData:
** if out to screen
** |Global Count|Total Time|CPU utime|CPU stime|CPU%|Ave CPU%|Memory|Memory%|
** 
** if out to file Ave CPU% is left out
*/

/* function starts timer for cpu ticks which is used by getCpuTimeVals
** startCpuTime should be started before using getCpuTimeVals()
**
*/
COMMONExpDLL
void startCpuTimer(void);
COMMONExpDLL
void getCpuTimeVals(cpuVals *cpuV, int isUpdate);
COMMONExpDLL
void getCpuTimeValDiff(cpuVals cpuV1,cpuVals cpuV2,cpuVals* cpuV); 
/* function returns memory statistics */
/*void getMemVals( int pid,memVals *memV);*/

#if defined(__cplusplus)
}
#endif /* __cplusplus */
#endif /*_PERF_DATA_H__*/
