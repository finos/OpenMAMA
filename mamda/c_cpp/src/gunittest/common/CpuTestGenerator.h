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

#ifndef CpuTestGeneratorH
#define CpuTestGeneratorH

#include <gtest/gtest.h>
#include <mama/mamacpp.h>
#include <wombat/machine.h>
#include <wombat/wMessageStats.h>
#include <valgrind/callgrind.h>
#include <stdio.h>
#include <stdlib.h>	
#include <math.h>
#include <string>
#include <string.h>

#define DEFAULT_PARAMETER_VALUE 0.0


/** 
 * Loop operation and record cpu used. A config parameter can be used to fail
 * test if over acceptable limit
 */
    
#define START_RECORDING_CPU(loopCount) \
    struct cpuVals cpuV; \
    long cpuLoopCount = loopCount; \
    double userTime, sysTime, allowable; \
    userTime = sysTime = allowable = DEFAULT_PARAMETER_VALUE; \
    string allowableCpuParameter = "Allowable"+string(::testing::UnitTest::GetInstance()->current_test_info()->name()); \
    const char* str_allowable; \
    str_allowable = Mama::getProperty((allowableCpuParameter.c_str())); \
    if (!str_allowable) { str_allowable = "0.0"; printf("\n Error reading allowableCpuParameter \n"); } \
    allowable = atof(str_allowable); \
    startCpuTimer(); \
    initProcTable(getpid(),false); \
    getCpuTimeVals(&cpuV, 0 ); \
    userTime = cpuV.userTime; \
    sysTime = cpuV.sysTime; \
    if (RUNNING_ON_VALGRIND != 0) \
    { \
        cpuLoopCount = 1; \
        printf("[          ] - !!!!! VALGRIND DETECTED - Loop count reset to 1\n"); \
        CALLGRIND_START_INSTRUMENTATION; \
    } \
    for (long i=0;i<cpuLoopCount;i++) \
    {

/**
 * Fail test if over acceptable limit (this is defined by a user & added to config file)
 */
#define STOP_RECORDING_CPU() \
    } \
    if (RUNNING_ON_VALGRIND != 0) \
    { \
        CALLGRIND_STOP_INSTRUMENTATION; \
        CALLGRIND_DUMP_STATS_AT (::testing::UnitTest::GetInstance()->current_test_info()->name()); \
    } \
    { \
    getCpuTimeVals(&cpuV, 0); \
    double usageTime = (cpuV.userTime + cpuV.sysTime) - (userTime + sysTime); \
    if (allowable == DEFAULT_PARAMETER_VALUE) \
    { \
        printf("Performing test with no acceptable value set\n"); \
        printf("Setup your max permitted time in config file, using parameter %s \n" \
               ,allowableCpuParameter.c_str()); \
    } \
    if (usageTime > 1) \
    { \
        double perSec = (((double)1.0 / usageTime) * (double)cpuLoopCount); \
        printf("[          ] - AllowableCpu %f UsedCpu %f (usr %f sys %f). %f updates per sec\n", \
            allowable, usageTime, (cpuV.userTime-userTime), (cpuV.sysTime-sysTime), perSec ); \
    } \
    else \
    { \
        printf("[          ] - AllowableCpu %f UsedCpu %f (usr %f sys %f).\n",allowable, usageTime, \
            (cpuV.userTime-userTime), (cpuV.sysTime-sysTime)); \
    } \
    RecordProperty ("AllowableCpu",allowable); \
    RecordProperty ("UsedCpu",usageTime); \
    EXPECT_GE (allowable, usageTime)<<"Cpu over acceptable limit"; \
    }

//#endif

#endif // CpuTestGeneratorH

