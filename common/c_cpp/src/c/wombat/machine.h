/* $Id: machine.h,v 1.14.16.7 2011/08/10 14:53:24 nicholasmarriott Exp $
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

#ifndef _WOMBAT_MACHINE_H
#define _WOMBAT_MACHINE_H

#include <time.h>

#include "wConfig.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct cpuVals
{
    double userTime;    /* user time allocated to cpu */
    double sysTime;     /* system time allocated to cpu */
    double realTime;    /* time elapsed since process started */
    double pCpu;        /* time spent on cpu as a percentage */
} cpuVals;

typedef struct memVals
{
    long vsize;
    long rss;
    double memPercent;
} memVals;

typedef struct systemMemVals
{
    long totalMem;      /* total system memory in kb */
    long freeMem;       /* free system memory in kb */
    long totalSwap;     /* total swap space in kb */
    long freeSwap;      /* free swap space in kb */
} systemMemVals;

COMMONExpDLL void getMemVals (int pid, memVals* memV);
COMMONExpDLL void initProcTable (int pid, int debuginfo);
COMMONExpDLL int getNumCpu (void);
COMMONExpDLL int getSystemMemory  (systemMemVals* mem);
COMMONExpDLL long getTotalSystemMem (void);
COMMONExpDLL time_t getTimeSinceEpoch (void);
COMMONExpDLL int getProcessInfo (int pid,
                                 memVals* memV,
                                 cpuVals* cpuV,
                                 int childFlag);
COMMONExpDLL void getProcAndChildCpu (int pid, cpuVals* cpv);
COMMONExpDLL void getSystemTime (double* upTime, double* idleTime);
COMMONExpDLL void getProcessorTime (long* usageTime,
                                    long* idleTime,
                                    int processor);
COMMONExpDLL void enableLWP (void);
COMMONExpDLL void disableLWP (void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _WOMBAT_MACHINE_H */
