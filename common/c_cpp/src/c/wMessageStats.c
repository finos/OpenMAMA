/* $Id: wMessageStats.c,v 1.76.18.3.8.1 2012/04/17 10:52:12 emmapollock Exp $
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

#include "wombat/machine.h"
#include "wombat/wMessageStats.h"
#include "wombat/wtable.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>


#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <sys/stat.h>
#include <time.h>

#ifndef WIN32
#include <dirent.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/time.h>
#ifndef VMS
#ifndef __APPLE__
#include <sys/vfs.h>
#endif
#endif
#include <sys/times.h>
#endif

#ifdef WIN32
/*#include "wombat/strptime.h"*/
typedef struct tms {
	long	tms_utime;
	long	tms_stime;
	long	tms_cutime;
	long	tms_cstime;
}tms;

#endif


#define MAX_SYMBOL_LENGTH 256
#define MILLISECONDS_IN_HOUR (60*60*1000)
#define SECONDSINHOUR 60

const char* opHeaderPeriod = \
        "  Symbol Name ," \
        "Elapsed Time ," \
        "Msg Count," \
        "Msg / sec," \
        "KiloBytes," \
        "KB / sec," \
        "Min Lat ms," \
        "Max Lat ms,"    \
        "Ave Lat ms," \
        "Std Deviation,\n";

const char* opHeaderAcrossAll = \
        "Msg Count," \
        "Min Msg/sec," \
        "Max Msg/sec," \
        "Ave Msg/sec," \
        "KiloBytes," \
        "KB / sec," \
        "Min Lat ms," \
        "Max Lat ms,"    \
        "Ave Lat ms, " \
        "Std Dev,\n";

const char* gStatsLevel1Header=\
        " Global Count,Total Time,Interval,CPU utime,CPU stime,  CPU%  , Ave CPU% ," \
        " Memory , Memory% ,\n";

typedef struct statsCache_t
{
    perfData*   mPData;
    wtable_t    mSymbolTable;
    int         mNumMsg;
    int         mHeader;
    FILE*       mOutfile;
    long        mNumMessages;
    long        mNumBytes;
    double      mLastUpdateTime;/* time in seconds of last update*/
}statsCache_t;

typedef struct perfData_t
{
    double  mMinLatencyP;    /* min latency for this capture PERIOD*/
    double  mMaxLatencyP;    /* max latency for this capture PERIOD */
    double  mAveLatencyP;    /* average latency for this capture PERIOD*/
    double  mMinLatencyA;    /* min latency across ALL capture periods*/
    double  mMaxLatencyA;    /* max latency across ALL capture periods*/
    double  mAveLatencyA;    /* average latency across ALL capture periods*/

    /* Separate message counter for latency in case
    some message don't have latency timestamps
    or cannto be parsed */
    
    double mLatAccumP;       /* latency accumlation for PERIOD*/
    long   mLatMsgCountP;    /* latency message count for PERIOD*/
    double mLatAccumA;       /* latency accumlation ALL capture periods*/
    long   mLatMsgCountA;    /* latency message across ALL capture periods*/

    long  mMsgCountP;      /* rolling message count for this capture PERIOD*/
    double mMsgPerSecP;     /* average message rate for this capture PERIOD*/
    long  mMsgCountA;      /* total message count ALL capture periods*/
    double mMaxMsgPerSecA;  /* max average message rate ALL capture periods*/
    double mMinMsgPerSecA;  /* min average message rate ALL capture periods*/
    double mAveMsgPerSecA;  /* average message rate ALL capture periods*/

    double mByteCountP;     /* rolling byte count for this capture PERIOD*/
    double  mBytePerSecP;    /* average byte rate for this capture PERIOD*/
    double  mByteCountA;     /* total byte count across ALL capture periods*/
    double  mMaxBytePerSecA; /* max ave message byte rate ALL capture periods*/
    double  mAveBytePerSecA; /* average message byte rate ALL capture periods*/

    double mLastTime;       /* this capture period*/
    double mTotalTime;      /* period since creation*/

    /* Counters are used to collect stats to calculate standard deviation */
    double mLatSquareP;    /* latency squared PERIOD */
    double mLatSquareA;    /* latency squared ALL */
    double mStdDeviationP; /* standard deviation values for capture PERIOD*/
    double mStdDeviationA; /* standard deviation values for ALL periods */
    
    FILE* mOutfile;
    char  mSymbol[MAX_SYMBOL_LENGTH];
    
}perfData_t;

/*Variables for testing latency stats*/
long    gCount              = 0;
double  gMilliSecAdded      = 0;
double  gStartTime          = 0;
double  gLastTime           = 0;
double  gTotalTime          = 0;
double  gTotalTimeCalc      = 0;
double  gTotalLatency       = 0;
double  gLatencyPerThousand = 0;
/*Variables for measuring cpu time*/
double gCpuClockTicksPerSecond;

struct cpuVals gLastCpuV={0.0,0.0,0.0,0.0};
double gStartRealTime = 0;
struct tms gStartProcTime;
/*struct tms gCpuStartTime;
struct tms gCpuEndTime;
double gCpuStartTimeSeconds = 0;
double gCpuEndTimeSeconds   = 0;
clock_t gProcStartTime= 0;
struct timeval gProcessStart={0,0};*/

static int gNumCpu=1;

/*struct tms myAppStartClock;
struct tms myAppEndClock;
clock_t myGlobalStartClock;
clock_t myGlobalEndClock;*/
double gTotalPercent=0;
double gRealTime = 0.0;

/*internal function prototypes */

/* Routines for calculating and printing performance data for each period */
static void processWTableData (wtable_t table, void* data, const char* key, void* closure);
static int printPerfData(perfData* pData,int headerFlag);
static void calcPerfData(perfData* mPData,double interval,
                         performanceData* myPerformanceData);
static void perfDataReportHeader(FILE* outfile);
extern void getMemVals(int pid, memVals *memV);

/*
** mode "all" will calculate std deviation across all capture periods
** mode "period" will calculate it across that period
*/
static double calcStdDeviation(perfData* mPData,const char* mode);
/*static long getTotalSystemMem(void);*/
/*reset period counters at end of time interval*/
static void resetPeriodCounters(perfData* mPData);

/* convert timeval structures to strings */
/*void timeval2string(struct timeval *ts, char buffer[], int len);*/
/*void ctime_r(long*, const char[],int);*/
 
/* Routines for printing global/average stats on exiting application */
static void wtableIteration(
    wtable_t table, void* data,const char* key, void* closure);
static void printPerfDataReport(perfData* pData,FILE* outfile);
static void initPerfData(perfData* pData, FILE* outfile, const char* symbol);



void startCpuTimer(void)
{
#ifndef WIN32
    gCpuClockTicksPerSecond = (double)sysconf(_SC_CLK_TCK);
    gStartRealTime = (double)times(&gStartProcTime)/(double)gCpuClockTicksPerSecond; 
    /*printf("initProcTable(getpid(),0)\n");*/
    initProcTable(getpid(),0);
    /*printf("getNumCpu()=%d\n",gNumCpu);*/
#endif
#ifdef WIN32
    gCpuClockTicksPerSecond = CLOCKS_PER_SEC;
    gStartRealTime = (double)clock()/(double)gCpuClockTicksPerSecond;
#endif
/*    gProcStartTime = times(&gCpuStartTime);*/
    gNumCpu=getNumCpu(); 
}


int createStatisticsCache(statsCache** sCache,int numMsgCategories,
                                 FILE* outfile,int header)
{
    int mysCacheStatus;
    struct timeval sTime;
    /*create statsCache struct*/
    statsCache* mysCache = (statsCache*)calloc(1,sizeof(statsCache));
    if(!mysCache)
    {
        mysCacheStatus = STATS_NO_MEMORY;
    } 
    mysCache->mNumMsg = numMsgCategories;
    mysCache->mHeader = header;
    mysCache->mOutfile = outfile;
    mysCache->mNumMessages = 0;
    mysCache->mNumBytes = 0;

    (void)gettimeofday(&sTime,NULL);
    mysCache->mLastUpdateTime = ( (double) sTime.tv_sec ) +
                                ( (double)sTime.tv_usec / 1000000 );

   /* mysCache->mNumMsg flags wether calculating stats on per symbol 
    * basis or general stats on all symbols, if > 0 then a table is needed
    * to store data on all symbols
    */
    
    if(mysCache->mNumMsg>0)
    {
        /*Per symbol/category*/
        mysCache->mSymbolTable = 
                  wtable_create("mSymbolTable",mysCache->mNumMsg);

        if (!mysCache->mSymbolTable)
        {
            printf ("Error: wtable_create() failed.\n");
        }
        else
        {

            if(mysCache->mHeader == 1 && mysCache->mOutfile == stdout)
            {
                fprintf(mysCache->mOutfile,opHeaderPeriod);
                mysCache->mHeader=0;
            }

        *sCache = mysCache;            
        }
    }
    else
    {
        /* Global statistics */
        const char* dummySymbol = "Global"; 
        mysCache->mPData =(perfData*)calloc(1,sizeof(perfData));
        if(!mysCache)
        {
            return STATS_NO_MEMORY;
        }
    initPerfData(mysCache->mPData, outfile, dummySymbol);

        if(mysCache->mPData)
        {
            if(mysCache->mHeader == 1 && mysCache->mOutfile == stdout)
            {
                fprintf(mysCache->mOutfile,opHeaderPeriod);
                mysCache->mHeader=0;
            }

            *sCache = mysCache;
        }
    }
    return STATS_OK;
}

 
void initPerfData(perfData* pData, FILE* outfile, const char* symbol)
{
    /*Initialise Perf Data Structure*/
    pData->mMinLatencyP=999999999;
    pData->mMaxLatencyP=0;
    pData->mAveLatencyP=0.00;
    pData->mMinLatencyA=999999999;
    pData->mMaxLatencyA=0;
    pData->mAveLatencyA=0.00;

    pData->mLatAccumP=0;
    pData->mLatMsgCountP=0;
    pData->mLatAccumA=0;
    pData->mLatMsgCountA=0;

    pData->mMsgCountP=0;
    pData->mMsgPerSecP=0.00;
    pData->mMsgCountA=0;
    pData->mMaxMsgPerSecA=0.00;
    pData->mMinMsgPerSecA=999999999;
    pData->mAveMsgPerSecA=0.00;

    pData->mByteCountP=0;
    pData->mBytePerSecP=0.00;
    pData->mByteCountA=0;
    pData->mMaxBytePerSecA=0.00;  /* not used at present*/
    pData->mAveBytePerSecA=0.00;  /* not used at present*/

    pData->mLastTime = 0;
    pData->mTotalTime = 0;
    pData->mOutfile=outfile;
    snprintf(pData->mSymbol,MAX_SYMBOL_LENGTH,"%s",symbol);
}

void resetPeriodCounters(perfData* mPData)
{
        mPData->mMsgCountP  = 0;
        mPData->mByteCountP = 0;
        mPData->mLatMsgCountP = 0;
        mPData->mLatAccumP = 0;
        mPData->mMinLatencyP = 999999999;
        mPData->mMaxLatencyP = 0;
        mPData->mAveLatencyP = 0;
        mPData->mStdDeviationP = 0;
        mPData->mLatSquareP = 0;
}


void statisticsCacheToFile(statsCache* sCache,
                           performanceData* myPerformanceData)
{
    double timeIntervalInSeconds;
    struct timeval sTime;
    double timeNow;
    (void)gettimeofday(&sTime,NULL);
    timeNow= ( (double) sTime.tv_sec ) +
             ( (double)sTime.tv_usec/1000000 );                            
    if(sCache)
    {
        timeIntervalInSeconds = timeNow - sCache->mLastUpdateTime;

#ifdef WIN32
        if(timeIntervalInSeconds<0)
            timeIntervalInSeconds+=60;
#endif
        sCache->mLastUpdateTime = timeNow;
        if(sCache->mNumMsg>0)
        {
            double paramArray[2];
            paramArray[0]=  timeIntervalInSeconds;
            paramArray[1]= (double) sCache->mHeader;

            /*iterate round wTable and update stats for each symbol*/
            wtable_for_each (sCache->mSymbolTable,
                             processWTableData, paramArray);
        }
        else
        {    
            /*calculate overall stats for subsCacheriptions*/
            if(sCache->mPData)
            {
                calcPerfData(sCache->mPData,timeIntervalInSeconds,myPerformanceData);
                if(!myPerformanceData)
                {  
                    /* Print out stats */
                    printPerfData (sCache->mPData,sCache->mHeader);	
                }
                resetPeriodCounters(sCache->mPData);
            }
        }
        sCache->mNumMessages=0;
        sCache->mNumBytes=0;
    }
}

double calcStdDeviation(perfData* mPData,const char* mode)
{
    /*
    if the POPULATION formula is needed change the final denominator
    from ( mPData->mLatMsgCountP -1 ) to  ( mPData->mLatMsgCountP )
    and  ( mPData->mLatMsgCountA -1 ) to  ( mPData->mLatMsgCountA )
    */

    double stdDeviation=0;
    if(strcmp(mode,"period")==0)
    {
        if( mPData->mLatMsgCountP > 1 )
        {
            stdDeviation = sqrt ((mPData->mLatSquareP-
                                 (mPData->mLatAccumP*mPData->mLatAccumP)/
                                  mPData->mLatMsgCountP) / 
                                 (mPData->mLatMsgCountP - 1));

        }
    }
    else if (strcmp(mode,"all")==0)
    {
        if( mPData->mLatMsgCountA > 1 )
        {                                                                    
            stdDeviation =sqrt ((mPData->mLatSquareA-
                                (mPData->mLatAccumA*mPData->mLatAccumA)/
                                 mPData->mLatMsgCountA) /
                                (mPData->mLatMsgCountA - 1));
    }
    }
    return stdDeviation;
}

/*
** this function is called each time 
** a new data item is retrieved from wtable
*/
void processWTableData (
    wtable_t table, void* data, const char* key, void* closure)
{
    double* cpyParamArray = (double*) closure;
    double interval = cpyParamArray[0];
    double header = cpyParamArray[1];

    perfData*       mPData = NULL;
    mPData = (perfData*) data;

    if (mPData)
    {
        /*calculate overall stats for subscriptions*/
        calcPerfData(mPData,interval,NULL);
        /* Print out stats */
        printPerfData (mPData,(int)header);            
        resetPeriodCounters(mPData);
    }                                                                                                                        
}


int updateStatisticsCache(statsCache* sCache ,
                          const char* msgCategoryName ,
                          long         numBytesRecieved,
                          long        timeSecs,
                          long        timeMicroSecs,
                          double      cLatency,
                          struct timeval tv)
{
    int ret;
    perfData *mPData = NULL;
    double latency = 0;

    if(timeSecs != 0 || timeMicroSecs != 0)
       calcLatency(timeSecs,timeMicroSecs,&latency,tv);
    else
       latency=cLatency;
                                                                                
    if(sCache->mNumMsg>0 && sCache->mSymbolTable)
    {
        /* More than one category*/
        mPData = 
            (perfData*) wtable_lookup (sCache->mSymbolTable,msgCategoryName);
        /* If category doesn't already exist*/
        if (!mPData)
        {
            /* Then create & init*/
            mPData = (perfData*)calloc(1,sizeof(perfData));    
            initPerfData(mPData,sCache->mOutfile,msgCategoryName);
        }        
    }
    else
    {
        /* No categories */
        mPData = sCache->mPData;
    }

    /* Update statistics for period    */
    mPData->mMsgCountP++;
    mPData->mByteCountP += (double)numBytesRecieved;
    
    /* change to get rid of spurious results*/
    /*    if (latency > 0) */
    if (/*latency >0  &&*/ latency<999999999)
    {
        mPData->mLatMsgCountP++;
        mPData->mLatAccumP += latency;
        if (latency < mPData->mMinLatencyP)
        {
            mPData->mMinLatencyP = latency;
        }
        if (latency > mPData->mMaxLatencyP)
        {
            mPData->mMaxLatencyP = latency;
        }
        mPData->mLatSquareP += (latency*latency);
    }     

    /* Update table in structure if more than one category*/
    if(sCache->mNumMsg>0)
    {
        ret=
        wtable_insert(sCache->mSymbolTable,msgCategoryName, (void*) mPData);
        if(ret>0)
        {
            /*error in insert*/    
        }  
             
    }

    sCache->mNumMessages++; 
    sCache->mNumBytes+=numBytesRecieved;
    return STATS_OK;
}

 
void destroyStatsCache(statsCache* sCache)
{
    if (sCache)
    {
        if (sCache->mPData)
        {
            free (sCache->mPData);
        }
        if (sCache->mSymbolTable)
        {
            wtable_destroy (sCache->mSymbolTable);
        }
        free (sCache);
    }
}

latencyVals calcLatency(long timeSecs,
                 long timeMicroSecs,
                 double* latency,
                 struct timeval tv)
{
    static long cachedMaxLatencySecs = (LONG_MAX/1000000);
    static latencyVals noLatVals = {0.0,0.0,0.0};
    double latencySecs = 0;
    double latencyUSecs = 0;
    struct latencyVals latVals = {0.0,0.0,0.0};

    /* Calculate now time */
    latencySecs = ((double)tv.tv_sec - (double)timeSecs);

    if(cachedMaxLatencySecs > latencySecs)
    {
        latencyUSecs =((double)tv.tv_usec - (double)timeMicroSecs);
        *latency = ((latencySecs *1000000) + latencyUSecs)/1000.0;

        latVals.pubTimeMilliSecs = latencySecs*1000.0;
        latVals.recieveTimeMilliSecs = latencyUSecs/1000.0;
        latVals.latencyMilliSecs = *latency;
        return latVals;
    }
    return noLatVals;
}

latencyVals calcLatency1TimeStamp(const char* timeStamp,
                  const char* timeFormat,
                  double*     latency,
                  struct timeval   tv)
{
    static latencyVals noLatVals = {0.0,0.0,0.0};
    time_t rawTime;
    struct tm* currentTime;
    struct tm myTm;
    struct latencyVals latVals={0.0,0.0,0.0};
    long millis = 0;
    const char* pos1=NULL;
    int lenTimeFormat = 0;
    int lenTimeStamp = 0;
    double latencySecs=0;
    double latencyUSec=0;   
    int nextNumCharBack=3;
#ifdef WIN32
    int secs=0;
    int mins=0;
#endif

    lenTimeFormat = strlen(timeFormat);
    lenTimeStamp = strlen(timeStamp);
    if ((timeStamp == NULL) || (timeFormat== NULL)
        || (lenTimeFormat == 0) || (lenTimeStamp == 0))
    {
      fprintf(stderr,"Error - calcLatency\n");
      return noLatVals;
    }
    /*printf("TimeStampStr[%s]",timeStamp);*/
    /* Calculate now time */
    time ( &rawTime );
    currentTime = localtime(&rawTime);

/*#ifndef WIN32*/

    pos1 = timeStamp+(lenTimeStamp-nextNumCharBack);
    millis = strtol(pos1,NULL,10);
    strptime(timeStamp,timeFormat,&myTm);
    latencySecs = (((currentTime->tm_hour*60.0+currentTime->tm_min)*60.0)+currentTime->tm_sec) -
                  (((myTm.tm_hour*60.0+myTm.tm_min)*60.0)+myTm.tm_sec);
    /*printf("%d:%d:%d-%d:%d:%d=%f\n",currentTime->tm_hour,currentTime->tm_min,currentTime->tm_sec,myTm.tm_hour,myTm.tm_min,myTm.tm_sec,latencySecs);*/

/*#else


    if(lenTimeStamp>10)
    {
        millis=0;
        pos1 = timeStamp+(lenTimeStamp-nextNumCharBack);
        if(pos1[0]=='.' || pos1[0] ==':')
        {
            nextNumCharBack-=1;
            pos1 = timeStamp+(lenTimeStamp-nextNumCharBack);
            nextNumCharBack+=3;
        }
        else
            nextNumCharBack+=3;       

        millis = strtol(pos1,NULL,10);
    }
    else
    {
        nextNumCharBack=2;
    }
    pos1 = timeStamp+(lenTimeStamp-nextNumCharBack);
    nextNumCharBack+=3;
    secs = (int)strtol(pos1,NULL,10);
    pos1 = timeStamp+(lenTimeStamp-nextNumCharBack);
    mins = (int)strtol(pos1,NULL,10);
    latencySecs = ((currentTime->tm_min*60) + currentTime->tm_sec) -
                  ((mins*60) + secs);
#endif*/

    latencyUSec = ((double)tv.tv_usec-(millis*1000.0));
    /*printf("-------%f - %f=%f\n",(double)tv.tv_usec,millis*1000.0,latencyUSec);*/

    /*if(latencySecs<0)
    {
       latencySecs+= SECONDSINHOUR;
    }*/

    *latency = ((latencySecs *1000000) + latencyUSec)/1000.0;                                                                                         
    latVals.pubTimeMilliSecs = latencySecs*1000.0;
    latVals.recieveTimeMilliSecs = latencyUSec/1000.0;
    latVals.latencyMilliSecs = *latency;
    
    return latVals;
}

void printStatsLevel1(int numMsg,FILE* graphData)
{
    struct cpuVals cpuV={0.0,0.0,0.0,0.0};
    struct memVals memV={0,0,0.0};
    gCount+=1;
                                                                              
    if(gCount==1 && graphData == stdout)
        printf("\n\n%s",gStatsLevel1Header);
                                                                               
    if( (gCount % numMsg) ==0)
    {
        if(gCount%(numMsg*30)==0 && graphData == stdout)
        {
           printf("\n\n%s",gStatsLevel1Header);
        }
        getCpuTimeVals(&cpuV,1);
        gRealTime+=cpuV.realTime;
        gTotalPercent+=cpuV.pCpu;
        getMemVals(getpid(),&memV);
        if(graphData == stdout)
        {
          fprintf(graphData,
          ",%-12.0ld,%-10.4f,%-9.3f,%-9.4f,%-9.4f,%-7.2f%%,%-9.2f%%,%-8ld,%-8.2f%%,\n",
          gCount,gRealTime,cpuV.realTime,cpuV.userTime,cpuV.sysTime,
          cpuV.pCpu,(gTotalPercent/(gCount/numMsg)),
          memV.rss,memV.memPercent);
        }
        else if(graphData != NULL)
        {
          /* this file version does not include ave cpu percentage */
          fprintf(graphData,
          "%-12.0ld,%-10.4f,%-9.3f,%-9.4f,%-9.4f,%-7.2f,%-8ld,%-8.2f\n",
          gCount,gRealTime,cpuV.realTime,cpuV.userTime,
          cpuV.sysTime,cpuV.pCpu,memV.rss,memV.memPercent); 
        }
    }
}

/*clock_t utime=0;
double total=0;
struct cpuVals lastCpuV;*/
void getCpuTimeVals(cpuVals* cpuV, int isUpdate)
{
   /**
    * Same value as running top with Interactive command "I"(<shift> i)
    *
    * Extract from top man page for INTERACTIVE COMMAND "I":
    * Toggle between Solaris (CPU percentage divided by total number  of
    * CPUs)  and  Irix  (CPU  percentage  calculated solely by amount of
    * time) views.  This is a toggle switch that affects only  SMP  sys-
    * tems.
    */
    double diffRealTime=0.0;
    double diff = 1234.0;
    struct tms CurrentProcTime;
    double CurrentRealTime = 0.0;

#ifndef WIN32
    CurrentRealTime = (double)times(&CurrentProcTime)/gCpuClockTicksPerSecond;
#endif
#ifdef WIN32
    CurrentRealTime = clock()/(double)gCpuClockTicksPerSecond;
#endif
    diffRealTime=CurrentRealTime-gStartRealTime;
    getProcAndChildCpu((int)getpid(),cpuV);
    cpuV->realTime =diffRealTime;

/* 
    although solaris cpu% is calculated(moving decaying?) by kernel, 
    it is not accurate in our scheme.
*/

    diff = (cpuV->userTime+cpuV->sysTime)- 
           (gLastCpuV.userTime+gLastCpuV.sysTime);
    if(cpuV->realTime > 1e-6)
    {
        cpuV->pCpu = diff/diffRealTime*100.0/gNumCpu;
        /*printf("New : (utime + stime ) (%9f + %9f) = %9f |rtime = %9f\n",cpuV->userTime,
                                                                             cpuV->sysTime,
                                                                             cpuV->userTime+cpuV->sysTime,
                                                                             CurrentRealTime);
        printf("Last: (utime + stime ) (%9f + %9f) = %9f |rtime = %9f\n",gLastCpuV.userTime,
                                                                             gLastCpuV.sysTime,
                                                                             gLastCpuV.userTime+gLastCpuV.sysTime,
                                                                             gStartRealTime);
        printf("Diff: (utime + stime ) (%9f + %9f) = %9f |rtime = %9f\n",cpuV->userTime - gLastCpuV.userTime,
                                                                            cpuV->sysTime  - gLastCpuV.sysTime,
                                                                            diff,
                                                                            diffRealTime);
        printf("getCpuTimeval    : CPU %9f = %9f / (%9f  *  %d) \n", cpuV->pCpu, diff, diffRealTime, gNumCpu);*/
    }
    else
    {
        cpuV->pCpu = -1;
    }

    if(isUpdate)
    {
        gStartRealTime=CurrentRealTime;
        gStartProcTime=CurrentProcTime;
        gLastCpuV=*(cpuV);
    }
}


void getCpuTimeValDiff(cpuVals cpuV1, cpuVals cpuV2, cpuVals* cpuV)
{
                                                                                
    cpuV->userTime =cpuV2.userTime - cpuV1.userTime;
                                                                                
    cpuV->sysTime  =cpuV2.sysTime  - cpuV1.sysTime;
                                                                                
    cpuV->realTime =cpuV2.realTime - cpuV1.realTime;
/*#ifdef __linux__*/                                                                                
    /*if (cpuV->realTime != 0)
        cpuV->pCpu =(((cpuV->userTime + cpuV->sysTime)*100.0)/(cpuV->realTime*gNumCpu));*/
    if (cpuV->realTime > 1e-6)
    {
        cpuV->pCpu =(((cpuV->userTime + cpuV->sysTime)*100.0)/(cpuV->realTime*gNumCpu));
        /*printf("getCpuTimeValdiff: CPU %9f = %9f / (%9f  *  %d) \n", cpuV->pCpu, 
                                                                     cpuV->userTime + cpuV->sysTime,
                                                                     cpuV->realTime,
                                                                     gNumCpu);*/
    }
    else
    {
        cpuV->pCpu = -1;
    }
    /*printf("getCpuDiff:cpu-num%d-%f\n",gNumCpu,cpuV->pCpu);*/
/*#endif*/

}

void calcPerfData(perfData* mPData, double interval,
                  performanceData* myPerformanceData)
{
    /*Calculate averages for the period*/
    if (mPData->mMsgCountP > 0)
    {
        mPData->mMsgPerSecP  = ( (double) mPData->mMsgCountP/ 
                                 (double) interval);
    }
    else
    {
        mPData->mMsgPerSecP = 0.00;
    }
    if (mPData->mByteCountP > 0)
    {
        mPData->mBytePerSecP = ( (double) mPData->mByteCountP / 
                                 (double) interval);
    }
    else
    {
        mPData->mBytePerSecP = 0.00;
    }
    if ( (mPData->mLatAccumP > 0) && (mPData->mLatMsgCountP > 0) )
    {
        mPData->mAveLatencyP = ( (double) mPData->mLatAccumP /
                                 (double)mPData->mLatMsgCountP );
    }
    /*Update global counters*/
    mPData->mMsgCountA += mPData->mMsgCountP;
    mPData->mByteCountA += mPData->mByteCountP;
    mPData->mLatAccumA += mPData->mLatAccumP;
    mPData->mLatSquareA += mPData->mLatSquareP;
    mPData->mLatMsgCountA += mPData->mLatMsgCountP;
    mPData->mTotalTime += interval;

/*    sCache->numMessages += mPData->mMsgCountP;
    sCache->mNumBytes += mPData->mByteCountP;
*/

    /*Update global Max & Mins*/
    if (mPData->mMaxMsgPerSecA < mPData->mMsgPerSecP)
    {
        mPData->mMaxMsgPerSecA = mPData->mMsgPerSecP;
    }
    if (mPData->mMinMsgPerSecA > mPData->mMsgPerSecP)
    {
        mPData->mMinMsgPerSecA =  mPData->mMsgPerSecP;
    }
    if (mPData->mMinLatencyP < mPData->mMinLatencyA)
    {
        mPData->mMinLatencyA = mPData->mMinLatencyP;
    }
    if (mPData->mMaxLatencyP >  mPData->mMaxLatencyA)
    {
        mPData->mMaxLatencyA = mPData->mMaxLatencyP;
    } 
    /* Update global averages*/
    if (mPData->mMsgCountA > 0)
    {
        mPData->mAveMsgPerSecA  = ( (double) mPData->mMsgCountA / 
                                    (double) mPData->mTotalTime);
    }
    else
    {
        mPData->mAveMsgPerSecA  = 0.00;
    }
    if (mPData->mByteCountA > 0)
    {
        mPData->mAveBytePerSecA = ( (double) mPData->mByteCountA /
                                    (double) mPData->mTotalTime);
    }
    else
    {
        mPData->mAveBytePerSecA = 0.00;
    }
    if ( (mPData->mLatAccumA > 0) && (mPData->mLatMsgCountA > 0) )
    {
        mPData->mAveLatencyA = ( (double) mPData->mLatAccumA /
                                 (double) mPData->mLatMsgCountA);
    }
    else
    {
        mPData->mAveLatencyA = 0.00;
    }
    
    /* call to get standard deviation for this period*/
    mPData->mStdDeviationP= calcStdDeviation(mPData,"period");
    if(myPerformanceData)
    {
        myPerformanceData->mSymbol = mPData->mSymbol;
        myPerformanceData->mTotalTime = mPData->mTotalTime;
        myPerformanceData->mMsgCountP = mPData->mMsgCountP;
        myPerformanceData->mMsgPerSecP = mPData->mMsgPerSecP;
        myPerformanceData->mByteCountP = mPData->mByteCountP/1000;
        myPerformanceData->mBytePerSecP = mPData->mBytePerSecP/1000;
        myPerformanceData->mMinLatencyP = mPData->mMinLatencyP;
        myPerformanceData->mMaxLatencyP = mPData->mMaxLatencyP;
        myPerformanceData->mAveLatencyP = mPData->mAveLatencyP;
        myPerformanceData->mStdDeviationP = mPData->mStdDeviationP;
    }
}





int printPerfData(perfData* pData,int headerFlag)
{
    if(pData->mOutfile != NULL)
    {
        /*only print out symbol details if they have changed*/
        if( pData->mMsgCountP>0 )
        {

            if(headerFlag==1)
            {
                   /* print stats header every time for all symbols */
                 fprintf(pData->mOutfile,opHeaderPeriod);
            }
            fprintf(pData->mOutfile," %-13s,",pData->mSymbol);
            fprintf(pData->mOutfile,"%-13.2f,",pData->mTotalTime);
            fprintf(pData->mOutfile,"%-9.0ld,",pData->mMsgCountP);
            fprintf(pData->mOutfile,"%-9.2f,",pData->mMsgPerSecP);        
            fprintf(pData->mOutfile,"%-9.2f,",(pData->mByteCountP/1000));
            fprintf(pData->mOutfile,"%-8.2f,",(pData->mBytePerSecP/1000));    
            fprintf(pData->mOutfile,"%-10.3f,",pData->mMinLatencyP);
            fprintf(pData->mOutfile,"%-10.3f,",pData->mMaxLatencyP);
            fprintf(pData->mOutfile,"%-10.3f,",pData->mAveLatencyP);    
            fprintf(pData->mOutfile,"%-13.4f,\n",pData->mStdDeviationP);
        } 
        return STATS_OK;
    }
    else
    {    
        return STATS_NULL_PARAM;
    }
}


int printPerfReport  (statsCache* sCache,FILE* outfile)
{
    /* update stats using final interval */
    statisticsCacheToFile(sCache,NULL);
    
    if(sCache && outfile != NULL)
    {
        if(sCache->mNumMsg>0)
        {
            /* call to iterate around wTable*/
            if(outfile==stdout)
            {
                perfDataReportHeader(outfile);
            }
            wtable_for_each (sCache->mSymbolTable,wtableIteration,outfile );
        }
        else
        {
            /* just print current perfData*/
            if(outfile==stdout)
            {
                perfDataReportHeader(outfile);
            }
            sCache->mPData->mStdDeviationA =  
                 calcStdDeviation(sCache->mPData,"all");
            printPerfDataReport(sCache->mPData,outfile);
        }
        return STATS_OK;
    }
    else
    {
        return STATS_NULL_PARAM;
    }
}

void wtableIteration (
    wtable_t table, void* data, const char* key, void* closure)
{
    /* 
    call back function from wtable_for_each() in printPerfReport  ()
    iterates around wtable returning new data item each time.
    */

    FILE* outfile = (FILE*) closure;
    perfData*       mPData = NULL;
    mPData = (perfData*) data;
    if (outfile != NULL)
    {
        mPData->mStdDeviationA = calcStdDeviation(mPData,"all"); 
        printPerfDataReport(mPData,outfile);
    }
}
void perfDataReportHeader(FILE* outfile)
{
    if(outfile!=NULL)
    {
        /* prints end of test report header*/
        fprintf(outfile,
        "\n\n\n\t\t\t\t\t *** Statistics Summary For Execution ***\n");
        fprintf(outfile, "  Symbol Name ,");
        fprintf(outfile,opHeaderAcrossAll);
    }
}
void printPerfDataReport(perfData* pData,FILE* outfile)
{
    if(outfile!=NULL)
    {
        fprintf(outfile," %-13s,",pData->mSymbol);
        fprintf(outfile,"%-9.0ld,",pData->mMsgCountA);
        fprintf(outfile,"%-11.2f,",pData->mMinMsgPerSecA);
        fprintf(outfile,"%-11.2f,",pData->mMaxMsgPerSecA);
        fprintf(outfile,"%-11.2f,",pData->mAveMsgPerSecA);
        fprintf(outfile,"%-9.2f,",(pData->mByteCountA/1000));
        fprintf(outfile,"%-8.2f,",(pData->mAveBytePerSecA/1000));
        fprintf(outfile,"%-10.3f,",pData->mMinLatencyA);
        fprintf(outfile,"%-10.3f,",pData->mMaxLatencyA);
        fprintf(outfile,"%-10.3f,",pData->mAveLatencyA); 
        fprintf(outfile,"%-8.2f,\n",pData->mStdDeviationA);
    }
}
