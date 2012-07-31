/* $Id: mamaproducerc_v2.c,v 1.1.2.4 2011/09/29 17:56:55 emmapollock Exp $
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

#include "mama/mama.h"
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>   /* Needed for mlockall() */
#include <malloc.h>
#include <limits.h>
#include <sys/time.h>   /* needed for getrusage */
#include <sys/resource.h>    /* needed for getrusage */

#define TV2NANO(x) (1000 *((x)->tv_sec * 1000000 + (x)->tv_usec))
#define TV2USEC(x) ((x)->tv_sec * 1000000 + (x)->tv_usec)
#define TS2NANO(x) ((x)->tv_sec * 1000000000 + (x)->tv_nsec)

/* the number of points in the moving average model used for reaching a target */
#define AVG_POINTS 9
#define MED_SORT(a,b) { if ((a)>(b)) MED_SWAP((a),(b)); }
#define MED_SWAP(a,b) { uint64_t temp=(a);(a)=(b);(b)=temp; }

#define SEND_TIME_FID 16
#define PADDING_FID 10004
#define SEQ_NUM_FID 10
#define DEFAULT_REFRESH_RATE 1000

#define STEP_PERCENT 10006
#define STEP_TARGET 10007

#define PERROR(...) do {                                    \
    fprintf (stderr, "%s:%d (%s) - ",                       \
             __FILE__,                                      \
             __LINE__,                                      \
             __FUNCTION__);                                 \
    fprintf (stderr, ## __VA_ARGS__);                       \
    fprintf (stderr, ": %s\n", strerror (errno));           \
} while (0)

#define PRINT_ERROR(...) do {               \
    fprintf (stderr, "%s (%d): ",           \
             __FUNCTION__,                  \
             __LINE__);                     \
    fprintf (stderr, ## __VA_ARGS__);       \
    fprintf (stderr, "\n");                 \
} while (0)

/* MAMA function checking macro */
#define MAMA_CHECK(in) do {                                 \
    mama_status status = in;                                \
    if (MAMA_STATUS_OK != status) {                         \
        printf ("%s:%d (%s) - Function error : %d (%s)\n",  \
                __FILE__,                                   \
                __LINE__,                                   \
                __FUNCTION__,                               \
                status,                                     \
                mamaStatus_stringForStatus (status));       \
        exit(status);                                       \
    }                                                       \
} while (0)

#define MP_MIN_MSG_SIZE 35 /* TLV for sequence number and mama date time */
#define PRODUCER_LOW_RATE 500000

#define MAX_NUM_STEPS   100

/* FIX for SOLARIS issue */
#ifndef CLOCK_PROCESS_CPUTIME_ID
#define CLOCK_PROCESS_CPUTIME_ID CLOCK_HIGHRES
#endif

typedef struct {
    uint32_t      mMsgNum;
    mamaPublisher mMamaPublisher;
    char          mTopic[MAMA_MAX_SYMBOL_LEN];
    mamaMsg*      mMamaMsgs;
    uint32_t      mNumMamaMsgs;
} publisher;

static publisher*           gPublisherList      = NULL;
static mamaPublisher        gStepPublisher      = NULL;
static mamaSubscription     gStepSubscriber    = NULL;
static mamaTransport*       gTransportArray     = NULL;
static uint64_t             gThrottle           = 0;
static mamaMsg              gStepMsg            = NULL;
static mamaQueue            gDefaultQueue       = NULL;
static mamaTimer            gRandomTimer        = NULL;
static mamaTimer            gBurstTimer         = NULL;
static mamaTimer            gStatsTimer         = NULL;
static mamaBridge           gMamaBridge         = NULL;
static mamaDateTime         gLastDisplayTime    = NULL;
static uint64_t             gLastDisplayTsc     = 0;
static const char*          gAppName            = "mamaproducerc";
static char**               gSymbolList         = NULL;
static int                  gMaxNumSymbols      = 100;
static int                  gNumSymbols         = 0;
static int                  gNumTopics          = 1;

static double               gStatsInterval      = 1.0;
static uint64_t             gTargetRate         = 0;
static uint64_t             gTarget             = 0;
static uint64_t             gBaseRate           = 0;
static uint64_t             gAsyncRate          = 0;
static uint64_t             gRefreshRate        = DEFAULT_REFRESH_RATE;

static int                  gShutdown           = 0;
static int                  gSleep              = 0;
static int                  gNumTrans           = 1;
static int                  gDupTopics          = 0;
static int                  gNumPubs            = 1;
static int                  gRdtsc              = 0;
static int                  gStatsOnTimers      = 0;
static int                  gTransportCallbacks = 0;
static char*                gSymbolNamespace    = NULL;

static uint64_t             gUpperSleep         = 0;
static uint64_t             gLowerSleep         = 0;
static int                  gRandom             = 0;
static uint32_t             gRandomHigh         = 500000;
static uint32_t             gRandomLow          = 100000;
static double               gRandomTimeout      = 5;

static double               gStep               = 0.0;
static uint32_t*            gSteps              = NULL;
static uint32_t             gNoSteps            = 0;

static double               gBurstTimeout       = 0;
static uint32_t             gBurst              = 0;
static uint32_t             gBursting           = 0;
static double               gBurstHigh          = 10.0;
static double               gBurstLow           = 5.0;

static double               gCpuFreq;
static mamaDateTime         gNowTime            = NULL;
static mamaTimeZone         gTimeZone           = NULL;

static uint32_t             gRun                = 1;

/* DJD condition variable for shutdown */
pthread_cond_t              pendingShutdown     = PTHREAD_COND_INITIALIZER;
pthread_mutex_t             pendingShutdownLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t             gMutex              = PTHREAD_MUTEX_INITIALIZER;

static const char *         gUsageString[]      =
{
"mamaproducerc",
"Usage: mamaproducerc [OPTIONS]",
"Produces messages with a timestamp and sequence number fo calculation of transport latency",
" when used in conjunction with mamaproducerc.",
"",
"[OPTIONS]",
"      [-app appName]       The application name as it will appear at the mama level; default is mamaproducerc.",
"      [-burst]             Simulate random micro bursts in rate between burstLow and burstHigh times the current rate.",
"                           Can work in a timer mode where bursts happen every X seconds or in random mode.",
"      [-burstHigh X]       The maximum multiple of the current rate which the rate will burst to; default is 10.",
"      [-burstInterval X]   Interval (secs) between micro bursts; default is to randomly create microspikes.",
"                           With this option the microspike will occur at a constant interval.",
"      [-burstLow X]        The minimum multiple of the current rate which the rate will burst to. Default is 5.",
"      [-d msgDelta]        The number of bytes to change message by i.e. msg will be -l +- -d; default delta is 0.",
"      [-dupTopics]         Duplicate topics across available transports.",
"      [-fifo]              Set the scheduler as first in first out.",
"      [-l bytes]           The published message size in bytes; default is 200B.",
"      [-lock]              Configure malloc to lock all memory in RAM.",
"      [-m middleware]      The middleware to use [wmw/lbm/tibrv]; default is wmw.",
"      [-numTopics topics]  The number of topics to create in the form MAMA_TOPIC00 MAMA_TOPIC01 etc",
"                           uses [-s topic] as root.",
"      [-numTrans trans]    The number of transports to create.",
"      [-priority X ]       Set the process's scheduler priority to X.",
"      [-random]            Randomise rate between randomLow and randomHigh values at randomInterval second intervals.",
"      [-randomHigh X]      Upper rate for random range. Default is 500000 msgs/sec.",
"      [-randomInterval X]  Interval (secs) in which to randomize rate. Default is 5 sec.",
"      [-randomLow X]       Lower rate for random range. Default is 100000 msgs/sec.",
"      [-rdtsc]             Use the CPU Time Stamp Counter for calculation of transport latency.",
"      [-rr]                Set scheduler as round robin.",
"      [-s topic]           The topic on which to send messages. Default is \"MAMA_TOPIC\".",
"      [-S namespace]       Symbol name space for the data.",
"      [-sleep nanosecs]    Nanoseconds to sleep between publishing messages to throttle message rate.",
"                           Default is 0 == maximum publishing rate possible.",
"      [-stats-timers]      Use mama timers for stats display; default is to use message loop.",
"      [-stepInterval int]  Step rate to -target at int second intervals with steps as per -steps",
"      [-steps X,Y,Z]       The steps to target rate as percentages; comma seperated list.",
"      [-t X]               Display Rate stats every X seconds; default is 1 sec.",
"      [-target rate]       Target rate to publish msgs/sec.",
"      [-tport name]        The transport parameters to be used from mama.properties; default is pub.",
"      [-v]                 Increase verbosity. Can be passed multiple times.",
NULL
};

static __inline__ uint64_t
rdtsc(void) {
  uint32_t lo, hi;
  __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
  return (uint64_t)hi << 32 | lo;
}

/**********************************************************
 *                  FUNCTION PROTOTYPES                   *
 **********************************************************/

static void signalCatcher
(
    int             sig
);

static void usage
(
    int             exitStatus
);

static void startCompleteCallback
(
    mama_status     status);

static void nanoSleep
(
    uint64_t        nsec
);

static void randomizeRate
(   mamaTimer       timer,
    void*           closure
);

static inline void stepRate
(
    uint32_t        percent,
    uint64_t        target
);

static inline void incrementRate(void);

static void burstCallback
(
    mamaTimer       timer,
    void*           closure
);

static void burstRate (void);

static void initializeMama
(
    const char*     middleware,
    const char*     transportName
);

static void initializePublishers
(
    uint32_t        msgSize,
    uint32_t        msgVar
);

static void initializeMessages
(
    publisher*      pub,
    uint32_t        msgSize,
    uint32_t        msgVar
);

static void publishMessage
(
    uint32_t        pubIndex,
    uint32_t        msgSample,
    mamaDateTime*   mTime
);

static void publishMessageRdtsc
(
    uint32_t        pubIndex,
    uint32_t        msgSample,
    uint64_t*       nowTsc
);

static void parseCommandLine
(
    int             argc,
    const char**    argv,
    double*         pMsgInterval,
    const char**    transportName,
    const char**    middleware,
    uint32_t*       pMsgSize,
    uint32_t*       pMsgVar
);

static double getClock (void);

static void displayCallback
(
    mamaTimer       timer,
    void*           closure
);

static void producerShutdown
(
    uint32_t        msgVar
);

static inline uint8_t checkRateReached
(
    uint64_t numMsgs,
    uint64_t target,
    uint64_t start,
    uint64_t end
)
{
    static uint64_t lastNumMsgs = 0;
    uint64_t diffNumMsgs;
    uint64_t diffNsec;
    uint64_t rate;
    uint64_t error;
    uint64_t percentageDiff = 0;

    if(target != 0)
    {
        if (gRdtsc)
            diffNsec = (uint64_t)((double)(1000 * (end-start))/(double)gCpuFreq);
        else
            diffNsec = end - start;

        diffNumMsgs = numMsgs - lastNumMsgs;
        lastNumMsgs = numMsgs;
        rate = (uint64_t) ((double) (1000000000.0 * diffNumMsgs) / (double) diffNsec);
        /*Work out the errors*/
        error = abs(rate-target);

        percentageDiff = ((error*100) / target);

        /*
         * If the percentage error is greater than or equal to 1% we must adjust gTickCount
         * Must account for when target is low by just using error
         */
        if (percentageDiff >= 1 && error > 1)
        {
            return 0;
        }
        /* If the error is within 1% we say that the target has been reached */
        else
        {
            if (gRandom && !gUpperSleep)
                printf ("Upper Rate reached %" PRIu64 "\n", rate);
            else if (gRandom && !gLowerSleep)
                printf ("Lower Rate reached %" PRIu64 "\n", rate);

            return 1;
        }
    }
    else
        return 1;
}

static inline uint64_t calculateNsecSleep
(
    uint64_t pubNsec,
    uint64_t target
)
{
    return target == 0 ? 1000000000 :
        pubNsec > 1000000000/target ? 0 :
            1000000000/target - pubNsec;
}

static inline uint64_t calculatePubTicks
(
    uint64_t numMsgs,
    uint64_t start,
    uint64_t end,
    uint64_t sleep
)
{
    static uint64_t lastNumMsgs = 0;
    uint64_t        diffNsec;
    uint64_t        diffNumMsgs;
    uint64_t        pubNsec;

    if (gRdtsc)
        diffNsec = (uint64_t)((double)(1000 * (end-start))/(double)gCpuFreq);
    else
        diffNsec = end - start;

    diffNumMsgs = numMsgs - lastNumMsgs;
    lastNumMsgs = numMsgs;

    /*
     * Estimate how long a publish takes in nsec. This will be
     * There is a sleep of nsec between messages
     */
    pubNsec = diffNsec/diffNumMsgs - sleep;

    return pubNsec;
}


/**********************************************************
 *              REAL TIME FUNCTIONS                       *
 **********************************************************/
#define PRE_ALLOCATION_SIZE (512*1024*1024) /* 0.5GB pagefault free buffer */
static  int     gSched      =   0;
static  int     gLock       =   0;
static  int     gPriority   =   40;

static void setprio(int prio, int sched)
{
    struct sched_param param;
    param.sched_priority = prio;
    if (sched_setscheduler(0, sched, &param) < 0)
        perror("sched_setscheduler");
}

static void
show_new_pagefault_count(const char* logtext,
                  const char* allowed_maj,
                  const char* allowed_min)
{
    static int last_majflt = 0, last_minflt = 0;
    struct rusage usage;

    getrusage(RUSAGE_SELF, &usage);

    printf("%-30.30s: Pagefaults, Major:%ld (Allowed %s), " \
           "Minor:%ld (Allowed %s)\n", logtext,
           usage.ru_majflt - last_majflt, allowed_maj,
           usage.ru_minflt - last_minflt, allowed_min);

    last_majflt = usage.ru_majflt;
    last_minflt = usage.ru_minflt;
}

static void configure_malloc_behavior(void)
{
    if (mlockall(MCL_CURRENT | MCL_FUTURE))
        perror("mlockall failed:");

    mallopt(M_TRIM_THRESHOLD, -1);

    mallopt(M_MMAP_MAX, 0);
}

static void reserve_process_memory(int size)
{
    int i;
    char *buffer;

    buffer = malloc(size);

    for (i = 0; i < size; i += sysconf(_SC_PAGESIZE)) {
        buffer[i] = 0;
    }

    free(buffer);
}

/**********************************************************
 *                EOF  FUNCTION PROTOTYPES                *
 **********************************************************/

static uint64_t
median(uint64_t* p)
{
#if AVG_POINTS==5
    MED_SORT(p[0],p[1]) ; MED_SORT(p[3],p[4]) ; MED_SORT(p[0],p[3]) ;
    MED_SORT(p[1],p[4]) ; MED_SORT(p[1],p[2]) ; MED_SORT(p[2],p[3]) ;
    MED_SORT(p[1],p[2]) ; return(p[2]) ;
#elif AVG_POINTS==7
    MED_SORT(p[0], p[5]) ; MED_SORT(p[0], p[3]) ; MED_SORT(p[1], p[6]) ;
    MED_SORT(p[2], p[4]) ; MED_SORT(p[0], p[1]) ; MED_SORT(p[3], p[5]) ;
    MED_SORT(p[2], p[6]) ; MED_SORT(p[2], p[3]) ; MED_SORT(p[3], p[6]) ;
    MED_SORT(p[4], p[5]) ; MED_SORT(p[1], p[4]) ; MED_SORT(p[1], p[3]) ;
    MED_SORT(p[3], p[4]) ; return (p[3]) ;
#elif AVG_POINTS==9
    MED_SORT(p[1], p[2]) ; MED_SORT(p[4], p[5]) ; MED_SORT(p[7], p[8]) ;
    MED_SORT(p[0], p[1]) ; MED_SORT(p[3], p[4]) ; MED_SORT(p[6], p[7]) ;
    MED_SORT(p[1], p[2]) ; MED_SORT(p[4], p[5]) ; MED_SORT(p[7], p[8]) ;
    MED_SORT(p[0], p[3]) ; MED_SORT(p[5], p[8]) ; MED_SORT(p[4], p[7]) ;
    MED_SORT(p[3], p[6]) ; MED_SORT(p[1], p[4]) ; MED_SORT(p[2], p[5]) ;
    MED_SORT(p[4], p[7]) ; MED_SORT(p[4], p[2]) ; MED_SORT(p[6], p[4]) ;
    MED_SORT(p[4], p[2]) ; return(p[4]) ;
#else
    fprintf(stderr,"ERROR: AVG_POINTS must be 5, 7 or 9. Exiting\n");
    exit(1);
#endif
}


/****************************************************
 *                      MAIN                        *
 ****************************************************/
int main (int argc, const char **argv)
{
    const char*     transportName   = "pub";
    const char*     middleware      = "wmw";
    char            nowString [40]  = {0x0};
    double          msgInterval     = 0.5;
    int32_t 	    randValCounts   = 0;
    uint32_t        msgSize         = 200;
    uint32_t        msgVar          = 0;
    uint64_t        lastStep        = 0;
    uint32_t        randVal         = 0;
    uint64_t        randNum         = 0;
    uint64_t        nsec            = 0;
    uint64_t        lastDisplay     = 0;
    uint64_t        numPoints       = 0;
    uint32_t        i               = 0;
    uint32_t        pubIndex        = 0;
    uint64_t        start, end;
    uint64_t        nowTsc;
    struct          timespec ts;
    uint64_t*       pubTickArray    = NULL;
    uint64_t        numMsgs = 0;
    int64_t         diffUsec        = 0;
    mamaDateTime    nowTime         = NULL;
    uint64_t        targetRate      = 0;
    uint64_t        usecStatInterval = 0;
    uint64_t        nsecStatInterval = 0;
    uint64_t        tscStatInterval = 0;
    uint64_t        pubNsec         = 0;
    uint8_t         reached         = 0;
    uint64_t        tscStepInterval = 0;
    uint64_t        nsecStepInterval = 0;
    uint64_t        startStep = 0;

    if(setvbuf(stdout, NULL, _IONBF, 0))
    {
        printf("Failed to change the buffer of stdout\n");
        return (1);
    }

    pubTickArray = (uint64_t*)calloc (AVG_POINTS, sizeof (uint64_t));
    gSymbolList = calloc (gMaxNumSymbols,
                          sizeof (*gSymbolList) * gMaxNumSymbols);

    /* Get clock frequency for use with rdtsc */
    gCpuFreq = getClock ();

    gTimeZone=mamaTimeZone_local();

    /* parse command line arguments */
    parseCommandLine (argc,
                      argv,
                      &msgInterval,
                      &transportName,
                      &middleware,
                      &msgSize,
                      &msgVar);

    /*
     * Real time priorities
     */
    if (gLock)
    {
        configure_malloc_behavior();
        reserve_process_memory(PRE_ALLOCATION_SIZE);
    }

    if(gSched)
    {
        if(gPriority)
        {
            if(gPriority>sched_get_priority_max(gSched))
            {
                fprintf(stderr,"ERROR: Piority %d is above max priority available of %d; correcting.\n",
                        gPriority,
                        sched_get_priority_max(gSched));
                gPriority=sched_get_priority_max(gSched);
            }
            if(gPriority<sched_get_priority_min(gSched))
            {
                fprintf(stderr,"ERROR: Piority %d is below min priority available of %d; correcting.\n",
                        gPriority,
                        sched_get_priority_min(gSched));
                gPriority=sched_get_priority_min(gSched);
            }
        }
        setprio(gPriority,gSched);
    }

    initializeMama (middleware,
                    transportName);

    initializePublishers (msgSize,
                          msgVar);

    mama_startBackground (gMamaBridge,
                          startCompleteCallback);

    signal (SIGINT, signalCatcher);

    MAMA_CHECK (mamaDateTime_create (&gNowTime));
    mamaDateTime_setToNow(gNowTime);
    MAMA_CHECK (mamaDateTime_create (&nowTime));
    mamaDateTime_setToNow(nowTime);

    MAMA_CHECK (mamaDateTime_create (&gLastDisplayTime));
    mamaDateTime_setToNow(gLastDisplayTime);
    gLastDisplayTsc = rdtsc();

    randValCounts = (2*msgVar)+1;


    mamaDateTime_setToNow (nowTime);
    mamaDateTime_getAsFormattedStringWithTz(nowTime,nowString,40,"%Y/%m/%d - %H:%M:%S",gTimeZone);

    printf ("%21s Starting to publish\n", nowString);

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts);
    srand(ts.tv_nsec);

    if(gRdtsc)
        end = rdtsc();
    else
    {
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts);
        end = TS2NANO(&ts);
    }

    if(gStatsOnTimers)
    {
        mamaTimer_create(&gStatsTimer, gDefaultQueue, displayCallback, gStatsInterval, (void*) &numMsgs);
    }
    else
    {
        lastDisplay = end;
        usecStatInterval=(uint64_t)(gStatsInterval*1000000);
        nsecStatInterval=(uint64_t)(gStatsInterval*1000000000);
        tscStatInterval=(uint64_t)(gStatsInterval*gCpuFreq*1000000);
    }

    /* do not start stepping for at least 10 seconds */
    if (gRdtsc)
        startStep = end + (10 * gCpuFreq * 1000000);
    else
        startStep = end + (10 * (uint64_t) 1000000000);

    /*Sleep mode where the sleep has been specified */
    if (gSleep)
    {
        while (gRun)
        {
            if(gRdtsc)
                randNum = rdtsc();
            else
            {
                clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts);
                randNum = TS2NANO(&ts);
            }
            randVal = randNum % randValCounts;
            for (i=0; i<gRefreshRate;i++)
            {
                if (!gRun)  break;

                nanoSleep (gSleep);

                if(gRdtsc)
                    publishMessageRdtsc (pubIndex, randVal, &nowTsc);
                else
                    publishMessage (pubIndex, randVal, &nowTime);

                numMsgs++;

                pubIndex++;
                if (pubIndex >= gNumPubs)
                    pubIndex = 0;

                if(gRdtsc)
                {
                    if(gStatsInterval && !gStatsOnTimers)
                    {
                        if(nowTsc- lastDisplay >= tscStatInterval)
                        {
                            displayCallback (NULL, &numMsgs);
                            lastDisplay = nowTsc;
                        }
                    }
                }
                else
                {
                    if(gStatsInterval && !gStatsOnTimers)
                    {
                        mamaDateTime_diffMicroseconds(nowTime, gLastDisplayTime, &diffUsec);
                        if(diffUsec >= usecStatInterval)
                        {
                            displayCallback (NULL, &numMsgs);
                        }
                    }
                }
            }
        }
    }
    /* random mode is an add on to target mode so there behaviour is almost the same */
    else if (gTarget || gRandom || gNoSteps)
    {
        if (gRandom)
            targetRate = gAsyncRate = gBaseRate = gRandomHigh;
        else if (gNoSteps)
            targetRate = gAsyncRate = gBaseRate = (gTargetRate*gSteps[0])/100;
        else
            targetRate = gAsyncRate = gBaseRate = gTargetRate;

        /*Must initializze nsec to something sensible and the model*/
        if(targetRate) gRefreshRate=targetRate/8+1;
        else           gRefreshRate=DEFAULT_REFRESH_RATE;
        nsec = calculateNsecSleep (0, targetRate);

        while (gRun)
        {
            if(gRdtsc)
                randNum = start = rdtsc();
            else
            {
                clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts);
                randNum = start = TS2NANO(&ts);
            }
            randVal = randNum % randValCounts;

            /*Need to set the refresh rate to something sensible*/
            if(targetRate) gRefreshRate=targetRate/8 + 1;
            else           gRefreshRate=DEFAULT_REFRESH_RATE;

            for (i=0; i<gRefreshRate; i++)
            {
                nanoSleep (nsec);

                if (targetRate)
                {
                    if(gRdtsc)  publishMessageRdtsc (pubIndex, randVal, &nowTsc);
                    else        publishMessage (pubIndex, randVal, &nowTime);
                    numMsgs++;
                }

                pubIndex++;
                if (pubIndex >= gNumPubs)
                    pubIndex = 0;
            }

            if(gRdtsc)
                end = rdtsc();
            else
            {
                clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts);
                end = TS2NANO(&ts);
            }

            pubTickArray [numPoints++%AVG_POINTS] = calculatePubTicks (numMsgs, start, end, nsec);
            pubNsec = median (pubTickArray);

            reached = checkRateReached (numMsgs, targetRate, start, end);

            if(gRdtsc)
            {
                if(tscStepInterval)
                {
                    if(end - lastStep > tscStepInterval)
                    {
                        incrementRate();
                        lastStep = end;
                    }
                }
                if(gStatsInterval && !gStatsOnTimers)
                {
                    if(end - lastDisplay >= tscStatInterval)
                    {
                        displayCallback(NULL,&numMsgs);
                        lastDisplay = end;
                    }
                }
            }
            else
            {
                if(nsecStepInterval)
                {
                    if(end - lastStep >= nsecStepInterval)
                    {
                        incrementRate();
                        lastStep = end;
                    }
                }
                if(gStatsInterval && !gStatsOnTimers)
                {
                    if(end - lastDisplay >= nsecStatInterval)
                    {
                        displayCallback(NULL,&numMsgs);
                        lastDisplay = end;
                    }
                }
            }

            /* if in random mode, and a limit has been reached, and the upper or lower limits are not known*/
            if(gRandom && reached && (!gLowerSleep || !gUpperSleep))
            {
                /* Upper is the first limit to be reached */
                if(!gUpperSleep)
                {
                    gUpperSleep = nsec;

                    /* Reset to find the low limit*/
                    gAsyncRate = gBaseRate = gRandomLow;
                    reached=0;
                }
                else
                {
                    /* The upper and lower sleeps are now known so randomizing can begin */
                    gLowerSleep = nsec;
                    mamaTimer_create(&gRandomTimer, gDefaultQueue, randomizeRate, gRandomTimeout, NULL);
                }
            }

            if(!nsecStepInterval && !tscStepInterval && gNoSteps && reached && end >= startStep)
            {
        		if(gRdtsc)
					tscStepInterval = (uint64_t)(gStep * gCpuFreq * 1000000);
				else
					nsecStepInterval = (uint64_t)(gStep * 1000000000);
                incrementRate ();
                lastStep = end;
            }

            /*if in burst mode*/
            if(gBurst)
            {
                /*if burst has been signalled*/
                if (gBursting)
                {
                    /*if burst is complete*/
                    if(targetRate==gAsyncRate)
                    {
                        gBursting=0;
                        gAsyncRate=gBaseRate;
                    }
                }
                else
                {
                    /*no burst signalled so can set AsyncRate to the base rate*/
                    gAsyncRate = gBaseRate;
                    /*if targets have been reached*/
                    if(reached)
                    {
                        /*if in timed burst mode*/
                        if(gBurstTimeout)
                        {
                            /*If timer has not been created and ready to create then create*/
                            if(!gBurstTimer)
                            {
                                mamaTimer_create(&gBurstTimer,
                                        gDefaultQueue,
                                        burstCallback,
                                        gBurstTimeout,
                                        NULL);
                            }
                        }
                        /*else in random burst mode*/
                        else
                        {
                            burstRate();
                        }
                    }
                }
            }
            else
            {
                gAsyncRate=gBaseRate;
            }

            targetRate=gAsyncRate;
            nsec = calculateNsecSleep (pubNsec, targetRate);
        }
    }
    else
    {
        while (gRun)
        {
            if(gRdtsc)
                randNum = rdtsc();
            else
            {
                clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts);
                randNum = TS2NANO(&ts);
            }
            randVal = randNum % randValCounts;
            for (i=0; i<gRefreshRate;i++)
            {
                if (!gRun)  continue;

                if(gRdtsc)  publishMessageRdtsc (pubIndex, randVal, &nowTsc);
                else        publishMessage (pubIndex, randVal, &nowTime);

                numMsgs++;

                pubIndex++;
                if (pubIndex >= gNumPubs)
                    pubIndex = 0;

                if(gRdtsc)
                {
                    if(gStatsInterval && !gStatsOnTimers)
                    {
                        if(nowTsc-lastDisplay >= tscStatInterval)
                        {
                            displayCallback(NULL, &numMsgs);
                            lastDisplay = nowTsc;
                        }
                    }
                }
                else
                {
                    if(gStatsInterval && !gStatsOnTimers)
                    {
                        mamaDateTime_diffMicroseconds(nowTime, gLastDisplayTime, &diffUsec);
                        if(diffUsec >= usecStatInterval)
                        {
                            displayCallback(NULL, &numMsgs);
                        }
                    }
                }
            }
        }
    }

    mama_stop (gMamaBridge);

    /* Wait for mama_stop to complete */
    pthread_mutex_lock (&pendingShutdownLock);
    while (!gShutdown)
    {
        pthread_cond_wait (&pendingShutdown, &pendingShutdownLock);
    }
    pthread_mutex_unlock (&pendingShutdownLock);

    producerShutdown (msgVar);

    MAMA_CHECK (mamaDateTime_destroy (gLastDisplayTime));
    MAMA_CHECK (mamaDateTime_destroy (gNowTime));
    MAMA_CHECK (mamaDateTime_destroy (nowTime));

    mama_close ();
    return 0;
}

static void producerShutdown
(
    uint32_t        msgVar
)
{
    int i, j;
    uint64_t nsec = gThrottle ? 1000000000/gThrottle : 0;

    if(gLock)
        munlockall();

    printf("Shutdown: Destroying Timers\n");
    if (gRandomTimer != NULL)
    {
        MAMA_CHECK (mamaTimer_destroy(gRandomTimer));
    }

    if (gBurstTimer != NULL)
    {
        MAMA_CHECK (mamaTimer_destroy(gBurstTimer));
    }

    if (gStatsTimer != NULL)
    {
        MAMA_CHECK (mamaTimer_destroy(gStatsTimer));
    }

    /*Publishers*/
    printf("Shutdown: Destroying Publishers\n");
    if (gStepPublisher != NULL)
        MAMA_CHECK (mamaPublisher_destroy (gStepPublisher));

    if (gStepSubscriber != NULL)
    {
        MAMA_CHECK (mamaSubscription_destroy (gStepSubscriber));
        MAMA_CHECK (mamaSubscription_deallocate (gStepSubscriber));
    }

    if(gPublisherList != NULL)
    {
        for (i = 0; i < gNumPubs; i++)
        {
            MAMA_CHECK (mamaPublisher_destroy (
                            gPublisherList[i].mMamaPublisher));
            for (j = 0; j < gPublisherList[i].mNumMamaMsgs; j++)
            {
                MAMA_CHECK (mamaMsg_destroy (gPublisherList[i].mMamaMsgs[j]));
            }
            if (nsec) nanoSleep (nsec);
        }
        free (gPublisherList);
    }

    MAMA_CHECK (mamaMsg_destroy (gStepMsg));

    /*Transport*/
    printf("Shutdown: Destroying Transport\n");
    if(gTransportArray != NULL)
    {
        for (i=0; i<gNumTrans; i++)
        {
            MAMA_CHECK (mamaTransport_destroy (gTransportArray[i]));
        }
        free(gTransportArray);
    }

    if(gSteps != NULL)
    {
        printf("Shutdown: Destroying Steplist\n");
        free (gSteps);
    }

    if (gSymbolList)
    {
        char** symbol = gSymbolList;

        while (*symbol)
        {
            free (*symbol);
            symbol++;
        }
        free (gSymbolList);
    }

    free (gSymbolNamespace);
}


static double getClock()
{
    FILE *cpuInfo;
    if ((cpuInfo = fopen ("/proc/cpuinfo", "rb"))!=NULL)
    {
        char* tmp;
        char buff[16384];
        char freq[128];
        char searchStr[128];

        while (!feof (cpuInfo))
        {
            fread (buff, sizeof (buff), 1, cpuInfo);
        }
        strcpy (searchStr, "cpu MHz\t\t: ");
        tmp = strstr (buff,searchStr);
        tmp += strlen (searchStr);
        strncpy (freq, tmp, 8);

        fclose(cpuInfo);
        return atof (freq);
    }
    PRINT_ERROR("Could not get CPU Frequency");
    exit (1);
}

static void transportCb
(
    mamaTransport      tport,
    mamaTransportEvent ev,
    short              cause,
    const void*        platformInfo,
    void*              closure
)
{
    printf ("Transport event %d: %s\n",
            ev,
            mamaTransportEvent_toString (ev));
}

static void onCreateStep
(
     mamaSubscription   subscription,
     void*              closure
)
{}

static void onErrorStep
(
    mamaSubscription    subscription,
    mama_status         status,
    void*               platformError,
    const char*         subject,
    void*               closure
)
{}

static void onMsgStep
(
    mamaSubscription        subscription,
    mamaMsg                 msg,
    void*                   closure,
    void*                   itemClosure
)
{
    static uint32_t percent = 0;
    static uint64_t target  = 0;

    mamaMsg_getU32 (msg, NULL, STEP_PERCENT, &percent);
    mamaMsg_getU64 (msg, NULL, STEP_TARGET, &target);

    stepRate (percent, target);
}

static void initializeMama
(
    const char*     middleware,
    const char*     transportName
)
{
    int i = 0;

    if (gAppName)
    {
        mama_setApplicationName (gAppName);
    }

    MAMA_CHECK(mama_loadBridge (&gMamaBridge, middleware) );
    MAMA_CHECK(mama_open ());
    mama_getDefaultEventQueue (gMamaBridge, &gDefaultQueue);

    gTransportArray = (mamaTransport*) calloc (gNumTrans,
                                               sizeof *gTransportArray);

    for (i=0; i<gNumTrans; i++)
    {
        MAMA_CHECK( mamaTransport_allocate (&gTransportArray[i]));
        if (gTransportCallbacks)
        {
            mamaTransport_setTransportCallback (gTransportArray[i],
                                                transportCb,
                                                NULL);
        }
        MAMA_CHECK( mamaTransport_create (gTransportArray[i],
                                          transportName,
                                          gMamaBridge));
    }
}
static void initializePublishers
(
    uint32_t    msgSize,
    uint32_t    msgVar
)
{
    int i = 0, j = 0, count = 0;
    publisher* pub;
    uint64_t   nsec = gThrottle ? 1000000000/gThrottle : 0;
    char** symbol = gSymbolList;

    gNumPubs = gNumTopics * gNumSymbols;

    if (gDupTopics)
        gNumPubs *= gNumTrans;

    gPublisherList = (publisher*)calloc (gNumPubs, sizeof (publisher));

    pub = gPublisherList;

    while (*symbol)
    {
        /* For each generated topic */
        for (i = 0; i < gNumTopics; i++)
        {
            /* For each transport if duplicating */
            for (;;)
            {
                if (gNumTopics > 1)
                {
                    snprintf (pub->mTopic,
                              MAMA_MAX_SYMBOL_LEN,
                              "%s%s%.2d",
                              gSymbolNamespace ? gSymbolNamespace : "",
                              *symbol,
                              i);
                }
                else
                {
                    snprintf (pub->mTopic,
                              MAMA_MAX_SYMBOL_LEN,
                              "%s%s",
                              gSymbolNamespace ? gSymbolNamespace : "",
                              *symbol);
                }

                MAMA_CHECK (mamaPublisher_create (&pub->mMamaPublisher,
                                                  gTransportArray[j],
                                                  pub->mTopic,
                                                  NULL,
                                                  NULL));

                initializeMessages (pub, msgSize, msgVar);
                pub++;
                j++;
                count++;

                if (count > 0 && count % 1000 == 0)
                    printf ("Created %i publishers.\n", count);

                /* Enforce throttle */
                if (nsec) nanoSleep (nsec);

                /* If created on all tports break */
                if (j == gNumTrans)
                {
                    j = 0;
                    break;
                }

                /* If not duplicating, break to round robin across transports */
                if (!gDupTopics)
                    break;
            }
        }
        symbol++;
    }

    if (gSteps != NULL)
    {
        if (gNoSteps != 0)
        {
            MAMA_CHECK(mamaPublisher_create (&gStepPublisher,
                                             gTransportArray[0],
                                             "MAMA_STEP",
                                             NULL,
                                             NULL));
        }
        else
        {
            mamaMsgCallbacks callbacks;
            memset (&callbacks, 0, sizeof callbacks);
            callbacks.onMsg = onMsgStep;
            callbacks.onCreate = onCreateStep;
            callbacks.onError = onErrorStep;

            MAMA_CHECK (mamaSubscription_allocate (&gStepSubscriber));
            MAMA_CHECK (mamaSubscription_createBasic (gStepSubscriber,
                                                      gTransportArray[0],
                                                      gDefaultQueue,
                                                      &callbacks,
                                                      "MAMA_STEP",
                                                      NULL));
        }
    }

    MAMA_CHECK(mamaMsg_create(&gStepMsg));
    MAMA_CHECK(mamaMsg_addU64(gStepMsg, NULL, STEP_TARGET, 0));
    MAMA_CHECK(mamaMsg_addU32(gStepMsg, NULL, STEP_PERCENT, 0));
}

static void initializeMessages
(
    publisher*      pub,
    uint32_t        msgSize,
    uint32_t        msgVar
)
{
    mamaDateTime dateTime = NULL;
    uint32_t    i           =  0;
    char *      padding     =  NULL;
    int32_t     paddingSize =  gRdtsc == 0 ?
		msgSize - sizeof(mamaDateTime) - sizeof(uint32_t) - sizeof(uint8_t) :
		msgSize - sizeof(uint64_t) - sizeof(uint32_t) - sizeof(uint8_t);

    if (paddingSize - msgVar <= 0)
    {
        PRINT_ERROR ("msgSize too small - must be a minimum of %d",
                     MP_MIN_MSG_SIZE + msgVar);
        exit (1);
    }

    MAMA_CHECK (mamaDateTime_create (&dateTime));

    paddingSize += msgVar;
    padding = (char*)malloc(paddingSize);
    memset(padding, 'A', paddingSize);

    pub->mMamaMsgs = (mamaMsg*) calloc (((2*msgVar)+1), sizeof *pub->mMamaMsgs);
    if (!pub->mMamaMsgs)
    {
        PRINT_ERROR ("Failed to allocate memory for mama messages");
        exit (1);
    }
    pub->mNumMamaMsgs = 2*msgVar+1;

    for (i=0; i < pub->mNumMamaMsgs; i++)
    {
        MAMA_CHECK (mamaMsg_create (&pub->mMamaMsgs[i]));
        MAMA_CHECK (mamaMsg_addU32 (pub->mMamaMsgs[i], NULL, SEQ_NUM_FID, 0));
        if (gRdtsc)
        {
            MAMA_CHECK(mamaMsg_addU64 (pub->mMamaMsgs[i],
                                       NULL,
                                       SEND_TIME_FID,
                                       0));
        }
        else
        {
            MAMA_CHECK (mamaMsg_addDateTime(pub->mMamaMsgs[i],
                                            NULL,
                                            SEND_TIME_FID,
                                            dateTime));
        }

        padding[paddingSize-i-1] = '\0';
        MAMA_CHECK(mamaMsg_addString(pub->mMamaMsgs[i],
                                     NULL,
                                     PADDING_FID,
                                     padding));
    }

    free(padding);
    MAMA_CHECK (mamaDateTime_destroy (dateTime));
}

static void publishMessage
(
    uint32_t        pubIndex,
    uint32_t        msgSample,
    mamaDateTime*   mTime
)
{
    publisher* pub = &gPublisherList[pubIndex];
	/*Update Seq Number*/
    MAMA_CHECK(mamaMsg_updateU32 (pub->mMamaMsgs[msgSample],
                                  "NULL",
                                  SEQ_NUM_FID,
                                  pub->mMsgNum++));

    pthread_mutex_lock(&gMutex);

	/*Get and update the timestamp*/
    mamaDateTime_setToNow(*mTime);
    MAMA_CHECK (mamaMsg_updateDateTime (pub->mMamaMsgs[msgSample],
                                        NULL,
                                        SEND_TIME_FID,
                                        *mTime));

	/*Publish the message*/
    MAMA_CHECK (mamaPublisher_send (pub->mMamaPublisher, pub->mMamaMsgs[msgSample]));
    pthread_mutex_unlock(&gMutex);
}

static void publishMessageRdtsc
(
    uint32_t        pubIndex,
    uint32_t        msgSample,
    uint64_t*       nowTsc
)
{
    publisher* pub = &gPublisherList[pubIndex];
    MAMA_CHECK(mamaMsg_updateU32 (pub->mMamaMsgs[msgSample],
                                  "NULL",
                                  SEQ_NUM_FID,
                                  pub->mMsgNum++));

    pthread_mutex_lock(&gMutex);
    *nowTsc = rdtsc();
    MAMA_CHECK (mamaMsg_updateU64 (pub->mMamaMsgs[msgSample],
                                   NULL,
                                   SEND_TIME_FID,
                                   *nowTsc));

    MAMA_CHECK (mamaPublisher_send (pub->mMamaPublisher, pub->mMamaMsgs[msgSample]));
    pthread_mutex_unlock(&gMutex);
}

static void parseCommandLine
(
    int             argc,
    const char**    argv,
    double*         pMsgInterval,
    const char**    transportName,
    const char**    middleware,
    uint32_t*       pMsgSize,
    uint32_t*       pMsgVar)
{
    int i = 0;

    for (i = 1; i < argc; )
    {
        /*
         * Flag command line options. i.e. no trailing value
         */
        if (strcmp ("-dupTopics", argv[i]) == 0)
        {
            gDupTopics=1;
            i ++;
        }
        else if (strcmp ("-random", argv[i]) == 0)
        {
            gRandom=1;
            i ++;
        }
        else if ((strcmp (argv[i], "-h")     == 0) ||
                 (strcmp (argv[i], "--help") == 0) ||
                 (strcmp (argv[i], "-?")     == 0))
        {
            usage(0);
        }
        else if (strcmp ("-rdtsc", argv[i]) == 0)
        {
            gRdtsc=1;
            i++;
        }
        else if (strcmp ("-burst", argv[i]) == 0)
        {
            gBurst= 1;
            i++;
        }
        else if (strcmp (argv[i], "-lock") == 0)
        {
            gLock = 1;
            i++;
        }
        else if (strcmp (argv[i], "-fifo") == 0)
        {
            gSched = SCHED_FIFO;
            i++;
        }
        else if (strcmp (argv[i], "-rr") == 0)
        {
            gSched = SCHED_RR;
            i++;
        }
        else if (strcmp (argv[i], "-tc") == 0)
        {
            gTransportCallbacks = 1;
            i++;
        }
        else if (strcmp (argv[i], "-v") == 0)
        {
            if ( mama_getLogLevel () == MAMA_LOG_LEVEL_WARN )
            {
                mama_enableLogging( stderr, MAMA_LOG_LEVEL_NORMAL );
            }
            else if ( mama_getLogLevel () == MAMA_LOG_LEVEL_NORMAL )
            {
                mama_enableLogging( stderr, MAMA_LOG_LEVEL_FINE );
            }
            else if( mama_getLogLevel () == MAMA_LOG_LEVEL_FINE )
            {
                mama_enableLogging( stderr, MAMA_LOG_LEVEL_FINER );
            }
            else
            {
                mama_enableLogging( stderr, MAMA_LOG_LEVEL_FINEST );
            }
            i++;
        }
        else if (strcmp ("-steps", argv[i]) == 0)
        {
            int j = 0;
            gSteps = (uint32_t*)calloc (MAX_NUM_STEPS, sizeof (uint32_t));

            if (i+1 != argc && argv[i+1][0] != '-')
            {
                char* c = strdup (argv[i+1]);
                char* res = strtok (c , ",");

                while (res != NULL)
                {
                    if(j<MAX_NUM_STEPS)
                    {
                        gSteps[j++] = (uint32_t) atol(res);
                    }
                    else
                    {
                        fprintf(stderr,
                                "ERROR:Only %d steps being used as"
                                "this is max\n",
                                MAX_NUM_STEPS);
                        break;
                    }
                    res = strtok (NULL , ",");
                }

                if (gStep == 0.0)
                    gStep=30.0;
                i++;
                free (c);
            }
            gNoSteps=j;
            i++;
        }
        else
        {
            /*
             * Command line options with trailing value
             */
            if (i == argc-1)
            {
                fprintf(stderr,"ERROR: Last Option %s is not a valid commandline flag. Ignoring\n",argv[i]);
                i++;
            }
            else if (strcmp (argv[i], "-stats-timers") == 0)
            {
                gStatsOnTimers=1;
                i++;
            }
            else if (strcmp ("-priority", argv[i]) == 0)
            {
                gPriority = atoi (argv[i + 1]);
                i += 2;
            }
            else if (strcmp ("-s", argv[i]) == 0)
            {
                if (gNumSymbols == gMaxNumSymbols)
                {
                    void* vp = NULL;
                    vp = realloc (gSymbolList,
                                  2 * gMaxNumSymbols * sizeof (*gSymbolList));
                    if (vp)
                    {
                        gSymbolList = vp;
                        memset (&gSymbolList[gMaxNumSymbols],
                                0,
                                sizeof (*gSymbolList) * gMaxNumSymbols);
                        gMaxNumSymbols *= 2;
                    }
                    else
                    {
                        PRINT_ERROR ("Failed to realloc symbol list");
                        exit (1);
                    }
                }

                gSymbolList[gNumSymbols] = strdup (argv[i+1]);
                gNumSymbols++;
                i += 2;
            }
            else if (strcmp ("-S", argv[i]) == 0)
            {
                gSymbolNamespace = calloc (MAMA_MAX_SYMBOL_LEN,
                                           sizeof *gSymbolNamespace);
                snprintf (gSymbolNamespace,
                          MAMA_MAX_SYMBOL_LEN,
                          "%s.",
                          argv[i+1]);
                i += 2;
            }
            else if (strcmp ("-l", argv[i]) == 0)
            {
                *pMsgSize = (uint32_t)atol(argv[i+1]);
                i += 2;
            }
            else if (strcmp ("-throttle", argv[i]) == 0)
            {
                gThrottle = (uint64_t)atol(argv[i+1]);
                i += 2;
            }
            else if (strcmp ("-sleep", argv[i]) == 0)
            {
                gSleep = (uint32_t) atol (argv[i+1]);
                i += 2;
            }
            else if (strcmp ("-numTopics", argv[i]) == 0)
            {
                gNumTopics = (uint32_t) atol (argv[i+1]);
                i += 2;
            }
            else if (strcmp ("-numTrans", argv[i]) == 0)
            {
                gNumTrans = (uint32_t) atol (argv[i+1]);
                i += 2;
            }
            else if (strcmp ("-target", argv[i]) == 0)
            {
                gTarget = 1;
                gTargetRate = (uint32_t) atol (argv[i+1]);
                i += 2;
            }
            else if (strcmp ("-t", argv[i]) == 0)
            {
                gStatsInterval = strtod(argv[i+1], NULL);
                i += 2;
            }
            else if (strcmp ("-burstInterval", argv[i]) == 0)
            {
                gBurst=1;
                gBurstTimeout= strtod(argv[i+1], NULL);
                i += 2;
            }
            else if (strcmp ("-burstLow", argv[i]) == 0)
            {
                gBurst = 1;
                gBurstLow = strtod(argv[i+1],NULL);
                i += 2;
            }
            else if (strcmp ("-burstHigh", argv[i]) == 0)
            {
                gBurstHigh = strtod(argv[i+1],NULL);
                i += 2;
            }
            else if (strcmp ("-randomInterval", argv[i]) == 0)
            {
                gRandomTimeout= strtod(argv[i+1], NULL);
                i += 2;
            }
            else if (strcmp ("-stepInterval", argv[i]) == 0)
            {
                gStep = strtod(argv[i+1], NULL);
                i += 2;
            }

            else if (strcmp ("-randomHigh", argv[i]) == 0)
            {
                gRandomHigh = (uint32_t) atol(argv[i+1]);
                i += 2;
            }
            else if (strcmp ("-randomLow", argv[i]) == 0)
            {
                gRandomLow = (uint32_t) atol(argv[i+1]);
                gRandom=1;
                i += 2;
            }
            else if (strcmp ("-d", argv[i]) == 0)
            {
                *pMsgVar = (uint32_t) atol(argv[i+1]);
                i += 2;
            }
            else if (strcmp ("-tport", argv[i]) == 0)
            {
                *transportName = argv[i+1];
                i += 2;
            }
            else if (strcmp ("-m", argv[i]) == 0)
            {
                *middleware = argv[i+1];
                i += 2;
            }
            else
            {
                fprintf(stderr,"ERROR: Commandline option %s not recognised. Skipping\n",argv[i]);
                i++;
            }
        }
    }

    if (gNumSymbols == 0)
    {
        gSymbolList[gNumSymbols] = strdup ("MAMA_TOPIC");
        gNumSymbols++;
    }

    if (gSleep && (gRandom || gTarget || gBurst || gNoSteps))
    {
        fprintf (stderr,"ERROR: Cannot use -sleep and with random mode, target mode, step mode or burst mode\n\n");
        usage(0);
    }

    if (gNoSteps && (gRandom || gBurst))
    {
        fprintf (stderr,"ERROR: Cannot use -step and with random mode or burst mode\n\n");
        usage(0);
    }

    if (gRandom)
    {
        if (gRandomHigh <= gRandomLow)
        {
            fprintf (stderr,"ERROR: -randomHigh < -randomLow\n\n");
            usage(0);
        }
        else
        {
            printf ("INFO: Using random rate between %d msgs/sec and %d msgs/sec at %.1f second intervals. ", gRandomLow, gRandomHigh, gRandomTimeout);
        }
    }

    if (gBurst)
    {
        if(gBurstHigh<=gBurstLow)
        {
            fprintf(stderr, "ERROR: -burstHigh:%.2f must be greater than -burstLow:%.2f\n",gBurstHigh, gBurstLow);
            usage(0);
        }
        else if (gBurstLow==0.0)
        {
            fprintf(stderr, "ERROR: -burstLow:%.2f must be greater than 0.00\n",gBurstLow);
            usage(0);
        }
        else
        {
            if(gBurstTimeout)
            {
                if(gBurstTimeout<2.0)
                {
                    fprintf(stderr, "ERROR: -burstInterval:%.2f is below minimum of 2.0\n",gBurstTimeout);
                    usage(0);
                }
                else
                {
                    printf("INFO: Will simulate microspikes between %.2f and %.2f times current rate at %.1f sec intervals\n",
                            gBurstLow,
                            gBurstHigh,
                            gBurstTimeout);
                }
            }
            else
            {
                printf("INFO: Will randomly simulate microspikes between %.2f and %.2f times current rate\n",
                        gBurstLow,
                        gBurstHigh);
            }
        }
    }

    if (gNoSteps)
    {
        printf("INFO: Using stepping mode up to %lu msgs/sec at %.1f second intervals\n",gTargetRate,gStep);

        printf("INFO: Steps as %d%%",gSteps[0]);

        for (i=1;i<gNoSteps;i++)
        {
            printf(", %d%%",gSteps[i]);
        }

        printf("\n");
    }

    printf("\n");

    printf ( "Starting Publisher with:\n");


    for (i = 0; i < gNumSymbols; i++)
    {
        printf ("   topic%d:             %s\n",
                i,
                gSymbolList[i]);
    }
    printf ( "   interval            %f\n"
             "   transport:          %s\n"
             "   msg size            %d\n",
             *pMsgInterval, *transportName, *pMsgSize);

    if(gSymbolNamespace)
    {
        printf("   namespace:          %s\n", gSymbolNamespace);
    }

}

static void usage
(
    int         exitStatus
)
{
    int i=0;
    while (NULL != gUsageString[i])
    {
        printf ("%s\n", gUsageString[i++]);
    }
    exit (exitStatus);
}

static void nanoSleep
(
    uint64_t    nsec
)
{
    struct      timespec st,et;
    uint64_t    start, end, tickDelta;

    if (gRdtsc)
    {
        /*
         * Convert nsec to CPU Ticks
         * gCpuFreq is in MHz (million Ticks per sec)
         * Divide by 1 x 10^9 to get ticks per nsec
         */
        tickDelta =  (uint64_t) ((double) (nsec * gCpuFreq)/1000.0);
        start = rdtsc();

        while (1)
        {
            end = rdtsc( );
            if (end - start >= tickDelta )
            {
                return;
            }
        }
    }
    else
    {
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &st);

        start = TS2NANO(&st);
        while (1)
        {
            clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &et);
            if (TS2NANO(&et) - start >= nsec)
            {
                return;
            }
        }
    }
}
static void startCompleteCallback
(
    mama_status status
)
{
    pthread_mutex_lock (&pendingShutdownLock);
    pthread_cond_signal  (&pendingShutdown);
    gShutdown=1;
    pthread_mutex_unlock (&pendingShutdownLock);
}

static void randomizeRate
(
    mamaTimer       timer,
    void*           closure
)
{
    printf("Randomizing rate\n");

    gBaseRate=gRandomLow+(gRandomHigh-gRandomLow)*(rand()/(RAND_MAX+1.0));
}

static inline void stepRate
(
    uint32_t    percent,
    uint64_t    target
)
{
    if(percent != 0)
    {
        gBaseRate=target;
        printf("Stepping rate to %" PRIu32"%% - %" PRIu64 " msgs/sec\n",
                percent,
                target);
    }
    else
    {
        printf("Stepping has completed.\n");
        signalCatcher (SIGUSR1);
    }
}

static inline void incrementRate (void)
{
    static int i = 0;
    uint32_t percent = i < gNoSteps ? gSteps[i] : 0;
    uint64_t target = i < gNoSteps ? (gTargetRate*gSteps[i])/100 : 0;

    stepRate (percent, target);

    MAMA_CHECK(mamaMsg_updateU64 (gStepMsg, NULL, STEP_TARGET, target));
    MAMA_CHECK(mamaMsg_updateU32 (gStepMsg, NULL, STEP_PERCENT, percent));
    pthread_mutex_lock(&gMutex);
    MAMA_CHECK(mamaPublisher_send (gStepPublisher, gStepMsg));
    pthread_mutex_unlock(&gMutex);
    i++;
}

static void burstCallback
(
    mamaTimer       timer,
    void*           closure
)
{
    burstRate();
}

static void burstRate(void)
{
    if(gBurstTimeout || (rand()%20>17))
    {
        gBursting=1;
        gAsyncRate=(uint32_t)gBaseRate*(gBurstLow+((gBurstHigh-gBurstLow)*(rand() / (RAND_MAX +1.0))));
        printf("Bursting Rate\n");
    }
}

static void displayCallback
(
    mamaTimer       timer,
    void*           closure
)
{
    static char         nowString [40]    = {0x0};
    static uint64_t     lastNumMsgs=0;
    uint64_t            rate;
    uint64_t            diffNumMsgs;
    uint64_t*           numMsgs = (uint64_t*)closure;
    int64_t             diffUsec=0;
    uint64_t            nowTsc;

    /* Check for overflow */
    if ( *numMsgs >= lastNumMsgs)
    {
        diffNumMsgs = *numMsgs - lastNumMsgs;
    }
    else
    {
        diffNumMsgs = UINT64_MAX - lastNumMsgs;
        diffNumMsgs += *numMsgs;
    }
    lastNumMsgs = *numMsgs;

    mamaDateTime_setToNow (gNowTime);
    if (gRdtsc)
    {
        nowTsc = rdtsc ();
        diffUsec = (uint64_t)((double)(nowTsc - gLastDisplayTsc)/(double)gCpuFreq);
        gLastDisplayTsc = nowTsc;
    }
    else
    {
        mamaDateTime_diffMicroseconds (gNowTime, gLastDisplayTime, &diffUsec);
        mamaDateTime_copy(gLastDisplayTime,gNowTime);
    }
    rate = (uint64_t) ((diffNumMsgs*1000000)/diffUsec);

    mamaDateTime_getAsFormattedStringWithTz(gNowTime,nowString,40,"%Y/%m/%d - %H:%M:%S",gTimeZone);

    if(diffNumMsgs > 0 && rate==0) rate=1;

    printf ("%s RATE = %" PRIu64 "\n", nowString,rate);
}

static void signalCatcher
(
    int             sig
)
{
    if(sig == SIGINT)
        printf("SIGINT caught. Shutting down\n");
    else if (sig == SIGUSR1)
        printf("Internal signal caught. Shutting down\n");
    else
        printf("Unknown signal caught. Shutting down\n");

    gRun = 0;
}
