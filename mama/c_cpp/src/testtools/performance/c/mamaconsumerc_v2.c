/* $Id: mamaconsumerc_v2.c,v 1.1.2.2 2011/09/28 15:21:09 emmapollock Exp $
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
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <math.h>
#include <sys/mman.h>   /* Needed for mlockall() */
#include <malloc.h>
#include <limits.h>
#include <sys/time.h>   /* needed for getrusage */
#include <sys/resource.h>    /* needed for getrusage */

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

#define TS2NANO(x) ((x)->tv_sec * 1000000000 + (x)->tv_nsec)
#define MIN_SAMPLE_RATE             1
#define MIN_INTERVAL_RATE           10
#define SOURCE_SIZE                    50

#define SEND_TIME_FID   16
#define LINE_TIME_FID   1174
#define EX_TIME_FID     465

#define MAX(a, b)   (a > b ? a : b)

#define MAX_MD_FID MAX(SEND_TIME_FID, MAX(LINE_TIME_FID, EX_TIME_FID))

#define STEP_PERCENT    10006
#define STEP_TARGET     10007

#define SEQ_NUM_FID 10

typedef struct {
    char*       mTopic;
    const char* mSource;
    const char* mName;
    int         mSourceIndex;
    uint32_t    mSeqNum;
} symbol;

struct {
    uint32_t    mPercent;
    uint64_t    mTarget;
} gStepClosure;

typedef struct {
    uint64_t    mNumMsgs;
    int64_t     mMin;
    int64_t     mTotal;
    int64_t     mMax;
    int64_t     mSqTotal;
    uint64_t*   mBuckets;
} stats;

typedef struct {
    stats       mTrans;
    stats       mPlat;
    stats       mInt;
    stats       mEx;
    stats       mTot;
    stats       mCallback;
    uint64_t    mNumRateMsgs;
    /* Transport Gaps from onError CB with GAP status */
    uint64_t    mTransportGaps;
    /*
     * For Basic subs gaps detected from mamaproducer seq num
     * For MD subs gaps from subscriptionOnGap callback
     */
    uint64_t    mGaps;
    uint64_t    mMissingFids;
    uint64_t    mMsgSize;
    uint64_t    mFirstTsc;
    uint64_t    mLastTsc;
    uint64_t    mMsgTsc;

    mamaMsgIterator mItr;

    mamaDateTime mFirstTime;
    mamaDateTime mLastTime;
    mamaDateTime mMsgTime;
    mamaDateTime mLineTime;
    mamaDateTime mExTime;

    int64_t     mTotLat;
    int64_t     mPlatLat;
    int64_t     mExLat;
    int64_t     mIntLat;
    int64_t     mTransLat;
} statsGroup;

static inline uint64_t
rdtsc(void) {
  uint32_t lo, hi;
  __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
  return (uint64_t)hi << 32 | lo;
}

typedef int(*mcOnFieldCb) (mamaMsgField field,
                           symbol*      sym,
                           statsGroup*  pStatsGroup);

typedef struct {
    mcOnFieldCb mCb;
    uint16_t    mFid;
} mcOnField;

static statsGroup*          gStatsGroups        = NULL;
static statsGroup           gStatsGroup;
static uint64_t             gStatsGroupIndex    = 0;

#define NUM_STATS_GROUPS 10
#define MAX_NUM_PERCENTILES 100

static uint32_t             gMaxSubscriptions   = 1300000;
static uint32_t             gMaxSources         = 100;

/* FIDs are uint16_t */
static mcOnField            gFields[10];
static int                  gNumFields          = 0;
static mcOnField*           gFieldsByFid[UINT16_MAX];
static mamaTimeZone         gTimeZone           = NULL;
static mamaQueue            gDefaultQueue       = NULL;
static mamaQueue            gDisplayQueue       = NULL;
static mamaQueue*           gQueues             = NULL;
static mamaTransport        gTransport          = NULL;
static mamaDispatcher*      gDispatchers        = NULL;
static mamaDispatcher       gDisplayDispatcher  = NULL;
static mamaSubscription*    gSubscriptionList   = NULL;
static mamaSource*          gSubscriptionSourceList = NULL;
static mamaBridge           gMamaBridge         = NULL;
static mamaTimer            gDisplayTimer        = NULL;

static double*              gPercentiles        = NULL;
static uint64_t             gNoPercentiles      = 0;

static const char*          gSymFileName        = NULL;
static const char*          gSymRateFileName    = NULL;
static const char*          gSymbolNamespace    = NULL;
static const char*          gSymbolRoot         = "MAMA_TOPIC";
static const char*          gStatsFile          = NULL;
static char                 gFile[PATH_MAX]     = "";
static const char*          gTransportName      = "sub";
static const char*          gAppName            = "mamaconsumerc";
static char                 gSources[SOURCE_SIZE][SOURCE_SIZE]   = {{0}};

static FILE*                gStatsFilep         = NULL;
static FILE*                gFilep              = NULL;

static symbol*              gSymbolList         = NULL;

static int                  gTransportCallbacks = 0;
static int                  gThreads            = 1;
static int                  gBasicSub           = 1;
static int                  gRequiresInitial    = 1;
static int                  gStatsOnTimers      = 0;
static int                  gPerfMode           = 0;
static int                  gRetries            = 2;
static int                  gIntLat             = 0;
static int                  gTransLat           = 1;
static int                  gPlatLat            = 0;
static int                  gTotLat             = 0;
static int                  gDataStats          = 0;
static int                  gExLat              = 0;
static int                  gSD                 = 0;
static int                  gCB                 = 0;
static int                  gHighWaterMark      = 0;
static int                  gLowWaterMark       = 0;

static int                  gBucketCount        = 1000;
static int                  gMaxBucketMin       = 0;
static int                  gBucketWidth        = 20;
static int                  gCollectStats       = 0;

static int                  gGenerateSymbols    = 0;
static int                  gStartLogging       = 5;
static int                  gCountInitials      = 0;
static int                  gCountTimeout       = 0;
static int                  gNumSources         = 0;
static int                  gLogResults         = 0;
static uint64_t             gStatsCount         = 0;
static int                  gIntervalStats      = 0;

static double               gCpuFreq;

static double               gTimeout            = 10.0;

static uint8_t              gRdtsc              = 0;
static uint8_t              gEventsInline       = 0;
static uint32_t             gNumSymbols         = 0;
static uint32_t             gPendingDestroyCount = 0;
static double               gTimeInterval       = 1.0;
static uint64_t             gTscStatInterval    = 0;
static uint64_t             gUsecStatInterval   = 0;
static uint32_t             gIntervalRate       = 1000 * MIN_INTERVAL_RATE;
static uint32_t             gSampleRate         = MIN_SAMPLE_RATE;

static int64_t              gThrottle           = -1;

static uint64_t             gTargetMsgCount     = 0;
static uint64_t             gRateSymMin         = 0;
static uint64_t             gRateSymMax         = 0;

pthread_mutex_t             gMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t             gPendingDestroyLock;
pthread_cond_t              gPendingDestroyCond;

static const char *     gUsageString[] =
{
"mamaconsumerc",
"Usage: mamaconsumerc [OPTIONS]",
"Consumes messages and calculates statistics as specified.",
"",
"Output: All minimum, average and maximum latencies are in usec",
"        With -rdtsc option, transport latency values are given in nsec",
"        Callback Average (CB AVG) value is given in nsec",
"[OPTIONS]",
"    [-app appName]         The application name as it will appear at the mama level. Default is mamaconsumerc.",
"    [-bucketCount X]       Number of buckets used for stats collection; default is 1000.",
"    [-bucketWidth X]       Width of the bucket for stats collection; default is 20.",
"    [-cb]                  Calculates message callback latency and display average per output stat.",
"    [-csv FILE]            Outputs all displayed stats to a comma delimited file.",
"    [-file FILE]           Outputs all displayed stats to $WOMBAT_LOGS/FILE.",
"    [-dataStats]           Print stats such as KB/sec and Average Message Size.",
"    [-ex]                  Calculates the min, average and max exchange latency.",
"    [-f symbolFile]        The symbol file to subscribe to.",
"    [-fifo]                Set the scheduler as first in first out.",
"    [-F rateSymFile]       The symbol file to subscribe to that will only affect rate stats. Used to deal with clock sync issues.",
"    [-fh]                  Calculates the min, average and max internal feed handler latency.",
"    [-genSyms X]           Generate symbols from 00 - X in form of MAMA_TOPIC00, MAMA_TOPIC01 etc. Uses -symRoot as root.",
"    [-h|-?|--help]         Show this help messgae.",
"    [-high X]              High Water mark - close the application when any queue grows to X events or greater.",
"    [-I]                   Disables initial requests.",
"    [-inline]              IO Events inline rather than on seperate queue.",
"    [-lock]                Configure malloc to lock all memory in RAM.",
"    [-low X]               Low Water mark - set to alarm of X events left in queue.",
"    [-m middleware]        The middleware to use [wmw/lbm/tibrv]. Default is wmw.",
"    [-mdsub]               Enables market data subscriptions.",
"    [-no-trans]            Do not calculates the min, average and max transport latency; default is to calculate.",
"    [-percentiles X,Y,Z]   The percentiles to print per output latency statistic; comma seperated list.",
"    [-plat]                Calculates the min, average and max platform latency.",
"    [-priority X]          Set the process's scheduler priority to X.",
"    [-rdtsc]               Read the CPU Time Stamp Counter for calculation of transport latency.",
"    [-rr]                  Set scheduler as round robin.",
"    [-s symbol]            The symbol to subscribe to.",
"    [-S namespace]         Symbol namespace for the data",
"    [-sd]                  Calculates standard deviation per output latency statistic.",
"    [-start-stats X]       Enables stats collection after X intervals, as per -t or 1 if -t is 0; default is 5 intervals.",
"    [-stats]               Collects latency statistics in buckets and displays on shutdown; by default this is disabled.",
"    [-stats-timers]        Uses mama timers for stats display; default is to use message loop.",
"    [-steps]               Expect stepping mamaproducerc.",
"    [-symRoot symbol]      Root used when generating symbols; default is MAMA_TOPIC.",
"    [-t X]                 Output interval for stats; default is every 1 sec.",
"    [-tc]                  Register transport callbacks.",
"    [-threads X]           Number of event queue threads to use in addition to the default queue; default is 1 additional queue.",
"    [-throttle X]          Throttles subscriptions at a rate of X per second.",
"    [-total]               Calculates the min, average and max total latency.",
"    [-tport name]          The transport parameters to be used from mama.properties.",
"    [-v]                   Increase verbosity. Can be passed multiple times.",
NULL
};

/**********************************************************
 *                  FUNCTION PROTOTYPES                   *
 **********************************************************/

static void parseCommandLine
(
    int                     argc,
    const char**            argv,
    const char**            topic,
    const char**            middleware
);

static void initializeMama
(
    const char*             middleware
);

static void parseSymbols (void);

static void subscribeToSymbols (void);

static void subscriptionOnCreate
(   mamaSubscription        subscription,
    void*                   closure
);

static void subscriptionOnGap
(
    mamaSubscription        subscription,
    void*                   closure
);

static void subscriptionOnQuality
(   mamaSubscription        subscription,
    mamaQuality             quality,
    const char*             symbol,
    short                   cause,
    const void*             opaque,
    void*                   closure
);

static void subscriptionOnError
(   mamaSubscription        subscription,
    mama_status             status,
    void*                   platformError,
    const char*             subject,
    void*                   closure
);

static void subscriptionOnMsg
(   mamaSubscription        subscription,
    mamaMsg                 msg,
    void*                   closure,
    void*                   itemClosure
);

static void subscriptionOnMsgStep
(
    mamaSubscription            subscription,
    mamaMsg                     msg,
    void*                       closure,
    void*                       itemClosure
);

static void subscriptionOnMsgRate
(
    mamaSubscription        subscription,
    mamaMsg                 msg,
    void*                   closure,
    void*                   itemClosure
);

static void subscriptionOnMsgPerf
(
    mamaSubscription        subscription,
    mamaMsg                 msg,
    void*                   closure,
    void*                   itemClosure
);

static void subscriptionOnMsgInterval
(
    mamaSubscription        subscription,
    mamaMsg                 msg,
    void*                   closure,
    void*                   itemClosure
);

static void subscriptionOnDestroy
(
   mamaSubscription             subsc,
   void*                        closure
);

static void setQueueMonitors (mamaQueue* localQueue);

static void highWaterMarkCallback
(
    mamaQueue               queue,
    size_t                  size,
    void*                   closure
);

static void lowWaterMarkCallback
(   mamaQueue               queue,
    size_t                  size,
    void*                   closure
);


static void displayCallback
(   mamaTimer               timer,
    void*                   closure
);

static void findSourceInList
(
    const char*             source,
    int*                    sourceIndex
);

static void usage
(
    int                     exitStatus
);

static void readSymbolsFromFile
(
    char*                   file
);

static void consumerNanoSleep
(
    unsigned                nsec
);

static void generateSymbols (void);

static double getClock (void);

static void signalCatcher
(
    int                     sig
);

static void consumerShutdown (void);

static void printHeader
(
    char* pString
);

static void displayStatsGroupBuckets
(
    statsGroup*     pStatsGroup
);

static void displayStatsBuckets
(
    stats*          pStats
);

static void displayStatsGroupCallback
(
    mamaQueue queue,
    void* closure
);

static void countInitialCallback
(
    mamaQueue queue,
    void* closure
);

static void startLoggingToBuckets
(
    mamaQueue queue,
    void* closure
);

static inline statsGroup* moveStatsGroup
(void)
{
    statsGroup *old, *new;
    old = &(gStatsGroups[gStatsGroupIndex%NUM_STATS_GROUPS]);
    ++gStatsGroupIndex;
    new = &(gStatsGroups[gStatsGroupIndex%NUM_STATS_GROUPS]);

    if (gRdtsc)
        new->mFirstTsc = old->mLastTsc;
    else
        mamaDateTime_copy(new->mFirstTime, old->mLastTime);

    return new;
}

static inline void mc_enqueueEvent
(
    mamaQueueEventCB    callback,
    void*               closure
)
{
    if (gEventsInline)
        callback (NULL, closure);
    else
        mamaQueue_enqueueEvent (gDisplayQueue, callback, closure);
}

static inline statsGroup* displayAndMoveStatsGroup
(void)
{
    statsGroup *old = &(gStatsGroups[gStatsGroupIndex%NUM_STATS_GROUPS]);
    statsGroup *new = moveStatsGroup ();

    mc_enqueueEvent(displayStatsGroupCallback, old);

    return new;
}

static inline void updateStats
(
    stats*      pStats,
    int64_t     lat
)
{
    ++pStats->mNumMsgs;
    pStats->mTotal += lat;
    if (lat < pStats->mMin)  pStats->mMin = lat;
    if (lat > pStats->mMax)  pStats->mMax = lat;
    if (gSD) pStats->mSqTotal += lat*lat;

    if (gCollectStats)
    {
        if (lat < gBucketWidth)        ++pStats->mBuckets[0];
        else if (lat >= gMaxBucketMin) ++pStats->mBuckets[gBucketCount-1];
        else                           ++pStats->mBuckets[lat/gBucketWidth];
    }
}

static void displayStats
(
    stats*      aStats
);

static void displayStatsGroup
(
    statsGroup*     pStatsGroup
);

static void createStatsGroup
(
    statsGroup* pStatsGroup
);

static void createStats
(
    stats* pStats
);

static void destroyStatsGroup
(
    statsGroup* pStatsGroup
);

static void destroyStats
(
    stats* pStats
);

static void initialiseStatsGroup
(
    statsGroup* pStatsGroup
);

static void initialiseStats
(
    stats* pStats
);

static void addStatsGroups
(
    statsGroup*     pResStatsGroup,
    statsGroup*     pStatsGroup
);

static void addStats
(
    stats*     pResStats,
    stats*     pStats
);

static void displayStatsGroupBucketsCallback
(
    mamaQueue queue,
    void* closure
);

static int exTimeOnField
(
    mamaMsgField   field,
    symbol*     sym,
    statsGroup* pStatsGroup
)
{
    if (MAMA_STATUS_OK == mamaMsgField_getDateTime (field,
                                                    pStatsGroup->mExTime))
        return 0;
    else
        return 1;
}

static int lineTimeOnField
(
    mamaMsgField   field,
    symbol*     sym,
    statsGroup* pStatsGroup
)
{
    if (MAMA_STATUS_OK == mamaMsgField_getDateTime (field,
                                                    pStatsGroup->mLineTime))
        return 0;
    else
        return 1;
}

static int sendTscOnField
(
    mamaMsgField   field,
    symbol*     sym,
    statsGroup* pStatsGroup
)
{
    if (MAMA_STATUS_OK == mamaMsgField_getU64 (field,
                                               &pStatsGroup->mMsgTsc))
        return 0;
    else
        return 1;
}

static int sendTimeOnField
(
    mamaMsgField   field,
    symbol*     sym,
    statsGroup* pStatsGroup
)
{
    if (MAMA_STATUS_OK == mamaMsgField_getDateTime (field,
                                                    pStatsGroup->mMsgTime))
        return 0;
    else
        return 1;
}

static int seqNumOnField
(
    mamaMsgField   field,
    symbol*     sym,
    statsGroup* pStatsGroup
)
{
    uint32_t seqNum;
    if (MAMA_STATUS_OK == mamaMsgField_getU32 (field,
                                               &seqNum))
    {
        /* Do not count as gap if mSeqNum is 0 */
        if (sym->mSeqNum && seqNum != sym->mSeqNum)
            ++pStatsGroup->mGaps;
        sym->mSeqNum = seqNum + 1;
        return 0;
    }
    else
        return 1;
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
        PERROR("sched_setscheduler");
}

static void configure_malloc_behavior(void)
{
    if (mlockall(MCL_CURRENT | MCL_FUTURE))
        PERROR("mlockall failed");

    mallopt(M_TRIM_THRESHOLD, -1);

    mallopt(M_MMAP_MAX, 0);
}

static void 
show_new_pagefault_count(const char* logtext,
                  const char* allowed_maj,
                  const char* allowed_min)
{
    int last_majflt = 0, last_minflt = 0;
    struct rusage usage;

    getrusage(RUSAGE_SELF, &usage);

    fprintf (gFilep, "%-30.30s: Pagefaults, Major:%ld (Allowed %s), "
             "Minor:%ld (Allowed %s)\n",
             logtext,
             usage.ru_majflt - last_majflt,
             allowed_maj,
             usage.ru_minflt - last_minflt,
             allowed_min);

    last_majflt = usage.ru_majflt;
    last_minflt = usage.ru_minflt;
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
static void transportCb
(
    mamaTransport      tport,
    mamaTransportEvent ev,
    short              cause,
    const void*        platformInfo,
    void*              closure
)
{
    switch (ev)
    {
        case MAMA_TRANSPORT_QUALITY:
            fprintf (gFilep, "Transport [%p] gaps %d\n",
                    (void*) tport,
                    cause);
            break;
        default:
            fprintf (gFilep, "Transport [%p] event %d: %s\n",
                    (void*) tport,
                    ev,
                    mamaTransportEvent_toString (ev));
            break;
    }
}

static int setNonBlocking (FILE* file)
{
    int fd = -1;
    int flags = 0;

    if (-1 == (fd = fileno (file)))
        return -1;

    if (-1 == (flags = fcntl(fd, F_GETFL, 0)))
        flags = 0;

    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

/**********************************************************
 *                        MAIN                            *
 **********************************************************/
int main (int argc, const char** argv)
{
    const char*         topic               = "MAMA_TOPIC";
    const char*         middleware          = "wmw";
    int                 i                   = 0;
    int                 read                = 0;

    gFilep = stdout;

    /*Set standard out to be line buffered and non-blocking */
    if (setvbuf (stdout, NULL, _IOLBF, 0))
    {
        fprintf (gFilep, "Failed to change the buffer of stdout\n");
        return (1);
    }

    if (-1 == setNonBlocking (stdout))
    {
        fprintf (stderr, "Failed to set std out non-blocking\n");
        return 1;
    }


    /* Get clock frequency for use with rdtsc */
    gCpuFreq = getClock ();

    gSymbolList = (symbol*) calloc (gMaxSubscriptions, sizeof (*gSymbolList));

    gSubscriptionSourceList = (mamaSource*) calloc (gMaxSources,
                                                    sizeof (mamaSource));
    gSubscriptionList = (mamaSubscription*)calloc (gMaxSubscriptions,
                                                   sizeof (mamaSubscription));

    gTimeZone=mamaTimeZone_local();

    parseCommandLine (argc,
                      argv,
                      &topic,
                      &middleware);

    memset (&gFields, 0, sizeof gFields);
    memset (&gFieldsByFid, 0, sizeof gFieldsByFid);
    if (gTransLat || gIntLat)
    {
        if (gRdtsc)
            gFields[gNumFields].mCb = sendTscOnField;
        else
            gFields[gNumFields].mCb = sendTimeOnField;
        gFields[gNumFields].mFid = SEND_TIME_FID;
        gFieldsByFid[SEND_TIME_FID] = &gFields[gNumFields];
        ++gNumFields;
    }
    if (gPlatLat || gIntLat || gExLat)
    {
        gFields[gNumFields].mCb = lineTimeOnField;
        gFields[gNumFields].mFid = LINE_TIME_FID;
        gFieldsByFid[LINE_TIME_FID] = &gFields[gNumFields];
        ++gNumFields;
    }
    if (gTotLat || gExLat)
    {
        gFields[gNumFields].mCb = exTimeOnField;
        gFields[gNumFields].mFid = EX_TIME_FID;
        gFieldsByFid[EX_TIME_FID] = &gFields[gNumFields];
        ++gNumFields;
    }
    if (gBasicSub)
    {
        gFields[gNumFields].mCb = seqNumOnField;
        gFields[gNumFields].mFid = SEQ_NUM_FID;
        gFieldsByFid[SEQ_NUM_FID] = &gFields[gNumFields];
        ++gNumFields;
    }

    gStatsGroups = (statsGroup*) calloc (NUM_STATS_GROUPS, sizeof (statsGroup));
    for (i=0; i<NUM_STATS_GROUPS; ++i)
    {
        createStatsGroup(&(gStatsGroups[i]));
    }
    createStatsGroup(&gStatsGroup);

    if (gRdtsc) gStatsGroups[0].mFirstTsc = rdtsc();
    else        mamaDateTime_setToNow (gStatsGroups[0].mFirstTime);

    if (gCollectStats)   gMaxBucketMin=(gBucketCount-1)*gBucketWidth;

    if (gTimeInterval)
    {
        gTscStatInterval = (uint64_t)(gTimeInterval * gCpuFreq * 1000000);
        gUsecStatInterval = (uint64_t)(gTimeInterval * 1000000);
    }
    else
    {
        gTscStatInterval = (uint64_t)(1.0 * gCpuFreq * 1000000);
        gUsecStatInterval = (uint64_t)(1.0 * 1000000);
    }

    /*
     * Real time priorities
     */
    if (gLock)
    {
        configure_malloc_behavior();
        reserve_process_memory(PRE_ALLOCATION_SIZE);
    }

    if (gSched)
    {
        if (gPriority)
        {
            if (gPriority>sched_get_priority_max(gSched))
            {
                fprintf(stderr,
                        "ERROR: Piority %d above max of %d; correcting.\n",
                        gPriority,
                        sched_get_priority_max(gSched));
                gPriority=sched_get_priority_max(gSched);
            }
            if (gPriority<sched_get_priority_min(gSched))
            {
                fprintf(stderr,
                        "ERROR: Piority %d below min of %d; correcting.\n",
                        gPriority,
                        sched_get_priority_min(gSched));
                gPriority=sched_get_priority_min(gSched);
            }
        }
        setprio(gPriority,gSched);
    }

    if (gStatsFile || gCollectStats || gDataStats || gIntervalStats || gCountInitials || gSD || gCB)
    {
        gPerfMode=0;
        if (gIntervalStats)
        {
            fprintf (gFilep, "INFO: Running in interval stats mode\n");
            if (gStatsFile || gCollectStats || gDataStats || gCountInitials ||
                    gSD || gCB)
            {
                gStatsFile = NULL;
                gCollectStats = gDataStats = gCountInitials = gSD = gCB = 0;
                fprintf(stderr,
                        "ERROR: All other functional command line options being"
                        " ignored in interval stats mode.\n");
            }

            if (gIntLat || gPlatLat || gExLat || gTotLat)
            {
                gIntLat = gPlatLat = gExLat = gTotLat = 0;
                fprintf (stderr,
                        "ERROR: Only transport latency stats available in "
                        "interval stats mode.\n");
            }
        }
        else
            fprintf (gFilep, "INFO: Running in timed stats mode\n");
    }
    else
    {
        gPerfMode=1;
        fprintf (gFilep, "INFO: Running in performance timed stats mode\n");
    }
    if (gStatsFile)
    {
        if ((gStatsFilep = fopen (gStatsFile, "w")) == (FILE *)NULL)
        {
            PERROR ("ERROR: main()-failed to open stats file");
            return (1);
        }
        read = setvbuf (gStatsFilep, 0, _IOLBF, 0);
        if (read != 0)
        {
            PERROR ("ERROR: main()-failed to set line buffering on stats file\n");
            return (1);
        }
        if (-1 == setNonBlocking (gStatsFilep))
        {
            fprintf (stderr, "Failed to set csv non-blocking\n");
            return 1;
        }
    }
    if (strcmp (gFile, "") != 0)
    {
        if ((gFilep = fopen (gFile, "w")) == (FILE *)NULL)
        {
            PERROR ("ERROR: failed to open stats file");
            return (1);
        }
        read = setvbuf (gFilep, 0, _IOLBF, 0);
        if (read != 0)
        {
            PERROR ("ERROR: failed to set line buffering on output file\n");
            return (1);
        }
        if (-1 == setNonBlocking (gFilep))
        {
            fprintf (stderr, "Failed to set stats file non-blocking\n");
            return 1;
        }
    }
    else
        gFilep = stdout;

    /* Read symbol files if specified*/
    if (gSymFileName)   readSymbolsFromFile((char*)gSymFileName);
    if (gSymRateFileName)
    {
        gRateSymMin=gNumSymbols;
        readSymbolsFromFile((char*)gSymRateFileName);
        gRateSymMax=gNumSymbols;
    }

    /*Generate symbols if requested*/
    if (gBasicSub && gGenerateSymbols)
    {
        fprintf (gFilep, "Generate symbols\n");
        generateSymbols();
    }
    /* If no symbols now then read symbols from file (-f) */
    if (!gNumSymbols)
    {
        /*No symbols then read from standard in*/
        readSymbolsFromFile (NULL);
    }

    /*If no symbols then invalid*/
    if (!gNumSymbols)
    {
        fprintf (stderr, "ERROR: Invalid command line. No symbols specified\n");
        exit(1);
    }

    parseSymbols ();

    initializeMama (middleware);

    subscribeToSymbols ();

    /*Print the output Header*/
    fprintf (gFilep, "\n%21s %8s ","TIME","RATE");
    if (gStatsFilep) fprintf (gStatsFilep, "TIME,RATE");

    if (!gPerfMode && !gIntervalStats)
    {
        if (gCB)
        {
            printHeader ("CB");
        }
        if (gDataStats)
        {
            fprintf (gFilep, "%10s ", "KB/Sec");
            if (gStatsFilep) fprintf (gStatsFilep, ",KB/Sec");

            fprintf (gFilep, "%12s ", "Av Msg Bytes");
            if (gStatsFilep) fprintf (gStatsFilep, ",Av Msg Bytes");
        }
    }
    if (gIntLat)
    {
        printHeader("FH");
    }
    if (gTransLat)
    {
        printHeader("TRANS");
    }
    if (gPlatLat)
    {
        printHeader("PLAT");
    }
    if (gExLat)
    {
        printHeader("EX");
    }
    if (gTotLat)
    {
        printHeader("TOT");
    }
    fprintf (gFilep, "%8s ", "GAPS");
    if (gStatsFilep) fprintf (gStatsFilep, ",GAPS");

    if (gBasicSub)
    {
        fprintf (gFilep, "%8s ", "MC GAPS");
        if (gStatsFilep) fprintf (gStatsFilep, ",MC GAPS");
    }
    else
    {
        fprintf (gFilep, "%8s ", "MD GAPS");
        if (gStatsFilep) fprintf (gStatsFilep, ",MD GAPS");
    }

    if (!gIntervalStats)
    {
        fprintf (gFilep, "%12s ", "MISSING FIDS");
        if (gStatsFilep) fprintf (gStatsFilep, ",MISSING FIDS");

        if (gStatsOnTimers  && gTimeInterval)
        {
            /* Timer used to print out stats every -t interval */
            mamaTimer_create (&gDisplayTimer,
                              gDefaultQueue,
                              displayCallback,
                              gTimeInterval,
                              NULL);
        }
    }
    fprintf (gFilep, "\n");
    if (gStatsFilep) fprintf (gStatsFilep, "\n");

    /* Used signal catcher to clean up */
    signal(SIGINT, signalCatcher);

    /* start the mama bridge, blocks here */
    mama_start (gMamaBridge);

    if (gCollectStats)
    {
        /*Display the current stats group and increment global stats group if required*/
        if (gStatsGroups[gStatsGroupIndex%NUM_STATS_GROUPS].mNumRateMsgs>0)
        {
            displayStatsGroup(
                    &(gStatsGroups[gStatsGroupIndex%NUM_STATS_GROUPS]));
            if (gLogResults)
                addStatsGroups(&gStatsGroup,
                        &(gStatsGroups[gStatsGroupIndex%NUM_STATS_GROUPS]));
        }

        /*Print the buckets of the global stats group*/
        displayStatsGroupBuckets(&gStatsGroup);
    }

    consumerShutdown();

    for(i=0; i<NUM_STATS_GROUPS; ++i)
        destroyStatsGroup(&(gStatsGroups[i]));
    destroyStatsGroup(&gStatsGroup);

    mama_close ();

    return 0;
}

static void printHeader
(
    char* pString
)
{
    char tmpString[40] = {0x0};
    uint64_t i = 0;
    double tmpShifted,iptr = 0.0;
    int numPlaces=0;

    strcpy(tmpString,pString);
    strcat(tmpString," LOW");
    fprintf (gFilep, "%10s ",tmpString);
    if (gStatsFilep) fprintf (gStatsFilep, ",%s",tmpString);

    strcpy(tmpString,pString);
    strcat(tmpString," AVG");
    fprintf (gFilep, "%10s ",tmpString);
    if (gStatsFilep) fprintf (gStatsFilep, ",%s",tmpString);

    if (gNoPercentiles > 0)
    {
        for(i=0; i<gNoPercentiles; ++i)
        {
            tmpShifted=gPercentiles[i];
            numPlaces=0;
            while(modf(tmpShifted,&iptr)>0.0)
            {
                ++numPlaces;
                tmpShifted=tmpShifted*10.0;
            }

            fprintf (gFilep, "%9.*f%% ", numPlaces, gPercentiles[i]);
            if (gStatsFilep)
            {
                fprintf (gStatsFilep,
                         ",%.*f%%",
                         numPlaces,
                         gPercentiles[i]);
            }
        }
    }

    strcpy(tmpString,pString);
    strcat(tmpString," HIGH");
    fprintf (gFilep, "%10s ",tmpString);
    if (gStatsFilep) fprintf (gStatsFilep, ",%s",tmpString);

    if (gSD)
    {
        strcpy(tmpString,pString);
        strcat(tmpString," SD");
        fprintf (gFilep, "%10s ",tmpString);
        if (gStatsFilep) fprintf (gStatsFilep, ",%s",tmpString);
    }
}


static void consumerShutdown (void)
{
    int i;
    unsigned nsecSleep = gThrottle != -1 ? 1000000000 / gThrottle : 0;
    pthread_mutexattr_t    attr;

    pthread_mutexattr_init (&attr);
    pthread_mutexattr_settype (&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init (&gPendingDestroyLock, &attr);
    pthread_cond_init  (&gPendingDestroyCond, NULL);

    if (gLock)
        munlockall();

    /*
     * To avoid QBYPASS problems, register onDestroy CB and on wait until all
     * subscriptions are destroyed before any deallocation.
     */

    fprintf (gFilep, "Shutdown: Destroying Subscriptions\n");

    gPendingDestroyCount = gNumSymbols;
    for (i = 0; i < gNumSymbols; ++i)
    {
        if (gSubscriptionList[i])
            mamaSubscription_destroy (gSubscriptionList[i]);
        if (gThrottle != -1)
            consumerNanoSleep (nsecSleep);
    }

    pthread_mutex_lock (&gPendingDestroyLock);
    while (gPendingDestroyCount)
    {
        pthread_cond_wait (&gPendingDestroyCond,
                           &gPendingDestroyLock);
    }
    pthread_mutex_unlock (&gPendingDestroyLock);

    /* Dispatchers */
    fprintf (gFilep, "Shutdown: Destroying Dispatchers\n");
    if (gDispatchers != NULL)
    {
        for (i = 0; i < gThreads; ++i)
        {
            MAMA_CHECK (mamaDispatcher_destroy (gDispatchers[i]));
        }
        free (gDispatchers);
    }
    if (gDisplayDispatcher != NULL)
    {
        MAMA_CHECK (mamaDispatcher_destroy (gDisplayDispatcher));
    }

    /*Subscriptions*/
    fprintf (gFilep, "Shutdown: Deallocating Subscriptions\n");
    for (i = 0; i < gNumSymbols; ++i)
    {
        if (gSubscriptionList[i])
            mamaSubscription_deallocate (gSubscriptionList[i]);
        free (gSymbolList[i].mTopic);
        gSymbolList[i].mSource = NULL;
        gSymbolList[i].mName = NULL;
    }
    free (gSymbolList);
    gSymbolList = NULL;
    free (gSubscriptionList);

    /*Sources*/
    fprintf (gFilep, "Shutdown: Destroying Sources\n");
    for (i=0; i < gNumSources; ++i)
    {
        MAMA_CHECK (mamaSource_destroy (gSubscriptionSourceList[i]));
    }
    free(gSubscriptionSourceList);

    /*Timers*/
    fprintf (gFilep, "Shutdown: Destroying Timers\n");
    if (!gIntervalStats && gStatsOnTimers)
    {
        if (gDisplayTimer != NULL)
        {
            MAMA_CHECK (mamaTimer_destroy (gDisplayTimer));
        }
    }

    /*Queues*/
    fprintf (gFilep, "Shutdown: Destroying Queues\n");
    if (gQueues != NULL)
    {
        for (i = 0; i < gThreads; ++i)
        {
            MAMA_CHECK (mamaQueue_destroyWait (gQueues[i]));
        }
        free (gQueues);
    }
    if (gDisplayQueue != NULL)
    {
        MAMA_CHECK (mamaQueue_destroyWait (gDisplayQueue));
    }

    /*Transport*/
    fprintf (gFilep, "Shutdown: Destroying Transport\n");
    if (gTransport != NULL)
    {
        MAMA_CHECK (mamaTransport_destroy (gTransport));
    }

    if (gStatsFile)
    {
        if (fclose(gStatsFilep) == EOF)
            PERROR("ERROR: shutdown()-Could not close stats file");
    }

    if (gFilep != stdout)
        fclose (gFilep);

    gFilep = stdout;

    if (gPercentiles != NULL) free(gPercentiles);
}

static void countInitialCallback
(
    mamaQueue queue,
    void* closure
)
{
    static uint64_t countInitials=0;
    ++countInitials;
    fprintf (gFilep,
            "Received initial [%" PRIu64 "]/[%d]-[%" PRIu64 "] remaining.\n",
            countInitials,
            gNumSymbols,
            gNumSymbols-countInitials);
}


static void displayStatsBuckets
(
    stats*      pStats
)
{
    double   cumPercent     = 0.0;
    double   percentile     = 0.0;
    int64_t  bucketLower    = 0;
    int64_t  bucketUpper    = 0;
    int64_t  maxUpper       = gBucketCount*gBucketWidth;
    int      i              = 0;
    uint64_t totalNumMsgs   = 0;
    uint64_t runningNumMsgs = 0;

    if (maxUpper<pStats->mMax)   maxUpper=pStats->mMax;

    for(i=0; i<gBucketCount; ++i) totalNumMsgs+=pStats->mBuckets[i];

    fprintf (gFilep,
             "\n%10s %10s %10s %10s %10s\n",
             "Lower",
             "Upper",
             "# Msgs",
             "Percentile",
             "Cumul. %");
    if (gStatsFilep)
        fprintf (gStatsFilep,
                "\n%s,%s,%s,%s,%s\n",
                "Lower",
                "Upper",
                "# Msgs",
                "Percentile",
                "Cumul. %");

    for(i=0; i<gBucketCount; ++i)
    {
        percentile = ((double)pStats->mBuckets[i]*100)/(double)totalNumMsgs;
        runningNumMsgs += pStats->mBuckets[i];
        cumPercent = ((double)runningNumMsgs*100)/(double)totalNumMsgs;
        bucketLower = i * gBucketWidth;
        if ( i == gBucketCount-1)
        {
            fprintf (gFilep,
                    "%10" PRIi64 " %10" PRIi64 " %10" PRIu64 " %10.5f %10.5f\n",
                    bucketLower,
                    maxUpper,
                    pStats->mBuckets[i],
                    percentile,
                    cumPercent);
            if (gStatsFilep)
                fprintf (gStatsFilep,
                         "%" PRIi64 ",%" PRIi64 ",%" PRIu64 ",%f,%f\n",
                         bucketLower,
                         maxUpper,
                         pStats->mBuckets[i],
                         percentile,
                         cumPercent);
        }
        else
        {
            bucketUpper = bucketLower + gBucketWidth;
            fprintf(gFilep,
                   "%10" PRIi64 " %10" PRIi64 " %10" PRIu64 " %10.5f %10.5f\n",
                   bucketLower,
                   bucketUpper,
                   pStats->mBuckets[i],
                   percentile,
                   cumPercent);
            if (gStatsFilep)
                fprintf (gStatsFilep,
                         "%" PRIi64 ",%" PRIi64 ",%" PRIu64 ",%f,%f\n",
                         bucketLower,
                         bucketUpper,
                         pStats->mBuckets[i],
                         percentile,
                         cumPercent);
        }

        if (runningNumMsgs==totalNumMsgs)   return;
    }
}

static void displayStatsGroupBuckets
(
    statsGroup*     pStatsGroup
)
{
    /* Print out the stats from each bucket */
    if (gIntLat)
    {
        fprintf (gFilep, "\n\t=== FH Internal Latency Count ====\n");
        if (gStatsFilep)
            fprintf (gStatsFilep,"\n\t=== FH Internal Latency Count ====\n");
        displayStatsBuckets(&(pStatsGroup->mInt));
    }
    if (gTransLat)
    {
        fprintf (gFilep, "\n\t=== Transport Latency Count ====\n");
        if (gStatsFilep)
            fprintf (gStatsFilep,"\n\t=== Transport Latency Count ====\n");
        displayStatsBuckets(&(pStatsGroup->mTrans));
    }

    if (gPlatLat)
    {
        fprintf (gFilep, "\n\t=== Platform Latency Count ====\n");
        if (gStatsFilep)
            fprintf (gStatsFilep,"\n\t=== Platform Latency Count ====\n");
        displayStatsBuckets(&(pStatsGroup->mPlat));
    }

    if (gExLat)
    {
        fprintf (gFilep, "\n\t=== Exchange Latency Count ====\n");
        if (gStatsFilep)
            fprintf (gStatsFilep,"\n\t=== Exchange Latency Count ====\n");
        displayStatsBuckets(&(pStatsGroup->mEx));
    }

    if (gTotLat)
    {
        fprintf (gFilep, "\n\t=== Total Latency Count ====\n");
        if (gStatsFilep)
            fprintf (gStatsFilep,"\n\t=== Total Latency Count ====\n");
        displayStatsBuckets(&(pStatsGroup->mTot));
    }

}

static void initializeMama (const char*     middleware)
{
    int i = 0;
    char queueNameBuff[24] = {0x0};

    if (gAppName)
    {
        mama_setApplicationName (gAppName);
    }

    MAMA_CHECK (mama_loadBridge (&gMamaBridge, middleware));
    MAMA_CHECK (mama_open ());
    mama_getDefaultEventQueue (gMamaBridge, &gDefaultQueue);
    MAMA_CHECK (mamaTransport_allocate (&gTransport));

    if (gTransportCallbacks)
    {
        /*Register interest in transport related events*/
        mamaTransport_setTransportCallback(gTransport, transportCb, NULL);
    }

    MAMA_CHECK (mamaTransport_setOutboundThrottle (gTransport,
                                                   MAMA_THROTTLE_DEFAULT,
                                                   -1));
    MAMA_CHECK (mamaTransport_create (gTransport, gTransportName, gMamaBridge));

    setQueueMonitors (&gDefaultQueue);
    /*
     * Create and alloc threads if we're using threads, if not,
     * just use the default queue
     */
    if (gThreads > 0)
    {
        gQueues      = (mamaQueue*) calloc (gThreads, sizeof (mamaQueue));
        gDispatchers = (mamaDispatcher*) calloc (gThreads,
                                                 sizeof (mamaDispatcher));

        for (i = 0; i < gThreads; ++i)
        {
            MAMA_CHECK (mamaQueue_create (&gQueues[i], gMamaBridge));

            sprintf (queueNameBuff, "Queue %d", i);
            mamaQueue_setQueueName (gQueues[i], queueNameBuff);
            setQueueMonitors (&gQueues[i]);

            MAMA_CHECK (mamaDispatcher_create (&gDispatchers[i] , gQueues[i]));
        }
    }
    if (!gEventsInline)
    {
        /* Unset qbypass temporarily as display queue should not be bypassed */
        char* value = NULL;
        char* bypass = getenv ("QBYPASS");

        if (bypass != NULL)
        {
            value = strdup (bypass);
            if (NULL != value)
            {
                if (0 != unsetenv ("QBYPASS"))
                {
                    fprintf (stderr, "ERROR: unsetenv failed - %s\n",
                             strerror (errno));
                }
            }
            else
            {
                fprintf (stderr,
                         "ERROR: strdup failed - Insufficient memory\n");
            }
        }

        MAMA_CHECK (mamaQueue_create (&gDisplayQueue, gMamaBridge));
        mamaQueue_setQueueName (gDisplayQueue, "Display Queue");
        setQueueMonitors (&gDisplayQueue);
        MAMA_CHECK (mamaDispatcher_create (&gDisplayDispatcher, gDisplayQueue));

        if (NULL != value)
        {
            if (0 != setenv ("QBYPASS", value, 1))
                fprintf (stderr, "ERROR: setenv failed\n");

            free (value);
        }
    }
}

static void findSourceInList
(
    const char*         source,
    int*                sourceIndex
)
{

    int i=0;
    int found = 0;
    for (i=0; i<gNumSources; ++i)
    {
        if (0==strncmp (source, gSources[i], 50))
        {
            /*
             * The source has already been registered - set the source index
             * for the symbol to enable subscription registration
             */
            *sourceIndex = i;
            found = 1;
        }
    }

    if (!found)
    {
        strncpy (gSources[gNumSources], source, 50);
        *sourceIndex = gNumSources;
        ++gNumSources;
    }
}

static void subscribeToSymbols (void)
{
    unsigned nsecSleep = gThrottle != -1 ? 1000000000 / gThrottle : 0;
    size_t i = 0;
    mamaMsgCallbacks callbacks;
    wombat_subscriptionOnMsgCB onMsg;

    memset (&callbacks, 0, sizeof callbacks);

    callbacks.onCreate          = subscriptionOnCreate;
    callbacks.onError           = subscriptionOnError;
    callbacks.onQuality         = subscriptionOnQuality;
    callbacks.onGap             = subscriptionOnGap;
    callbacks.onDestroy         = subscriptionOnDestroy;
    if (gPerfMode)
        onMsg = subscriptionOnMsgPerf;
    else if (gIntervalStats)
        onMsg = subscriptionOnMsgInterval;
    else
        onMsg = subscriptionOnMsg;

        for (i=0; i < gNumSources; ++i)
    {
        /* create that source and have it ready for use by init below */
        MAMA_CHECK (mamaSource_create (&gSubscriptionSourceList[i]));
        mamaSource_setId (gSubscriptionSourceList[i], gSources[i]);
        mamaSource_setTransport (gSubscriptionSourceList[i], gTransport);
        mamaSource_setSymbolNamespace (gSubscriptionSourceList[i], gSources[i]);
    }

    for (i=0; i < gNumSymbols; ++i)
    {
        char tmpSym[100];
        mamaQueue localQueue;

        /* Correct the onMsg callback if this is a rate only symbol */
        if (i>=gRateSymMin && i<gRateSymMax)
            callbacks.onMsg         = subscriptionOnMsgRate;
        else if (strcmp ("MAMA_STEP" ,gSymbolList[i].mTopic) == 0)
            callbacks.onMsg         = subscriptionOnMsgStep;
        else
            callbacks.onMsg         = onMsg;

        /* If there is more than one queue round robin accross them */
        localQueue = gQueues ==
            NULL ? gDefaultQueue : gQueues[i % gThreads];

        if (i == gMaxSubscriptions-1)
        {
            fprintf (gFilep,
                     "ERROR - reached maximum subscription count - try "
                     "rerunning specifying -subCount\n"
                     "at the command line to increase max subscription count "
                     "from %" PRIu32 "\n",
                     gMaxSubscriptions);
            exit (1);
        }

        MAMA_CHECK (mamaSubscription_allocate (&gSubscriptionList[i]));

        mamaSubscription_setTimeout (gSubscriptionList[i],
                                     gTimeout);

        mamaSubscription_setRetries (gSubscriptionList[i],
                                     gRetries);
        if (gBasicSub)
        {
            memset(tmpSym,0,100);
            /*Implement the namespace use*/
            if (gSymbolList[i].mSource)
            {
                sprintf (tmpSym,
                         "%s.%s",
                         gSymbolList[i].mSource,
                         gSymbolList[i].mName);
            }
            else
                strcpy (tmpSym,gSymbolList[i].mName);

            MAMA_CHECK (mamaSubscription_createBasic (gSubscriptionList[i],
                                                      gTransport,
                                                      localQueue,
                                                      &callbacks,
                                                      tmpSym,
                                                      (void*)&gSymbolList[i]));
        }
        else
        {
            if (!gSymbolList[i].mSource)
            {
                PRINT_ERROR ("No source found for %s",
                             gSymbolList[i].mTopic);
                exit (1);
            }
            MAMA_CHECK (mamaSubscription_setSubscriptionType (
                            gSubscriptionList[i],
                            MAMA_SUBSC_TYPE_NORMAL));
            /* Create a regular market data subscription.  */
            mamaSubscription_setRequiresInitial (gSubscriptionList[i],
                                                 gRequiresInitial);

            MAMA_CHECK (mamaSubscription_create (
                           gSubscriptionList[i],
                           localQueue,
                           &callbacks,
                           gSubscriptionSourceList[gSymbolList[i].mSourceIndex],
                           gSymbolList[i].mName,
                           (void*)&gSymbolList[i]));
        }

        if (gThrottle != -1)
            consumerNanoSleep (nsecSleep);

        if (i > 1000 && i % 1000 == 0)
        {
            fprintf (gFilep, "Subscribed to %lu symbols.\n", i);
        }
    }

}

static void subscriptionOnDestroy
(
   mamaSubscription             subsc,
   void*                        closure
)
{
    pthread_mutex_lock (&gPendingDestroyLock);
    gPendingDestroyCount--;
    if (!gPendingDestroyCount)
        pthread_cond_signal (&gPendingDestroyCond);
    pthread_mutex_unlock (&gPendingDestroyLock);
}

static void subscriptionOnCreate
(
    mamaSubscription            subscription,
    void*                       closure
)
{
}

static void subscriptionOnGap
(   mamaSubscription   subscription,
    void*              closure
)
{
    statsGroup* lats;
    if (gStatsOnTimers || gThreads > 1)
        pthread_mutex_lock(&gMutex);

    lats = &(gStatsGroups[gStatsGroupIndex%NUM_STATS_GROUPS]);
    ++lats->mGaps;

    if (gStatsOnTimers || gThreads > 1)
        pthread_mutex_unlock(&gMutex);
}

static void subscriptionOnQuality
(   mamaSubscription            subscription,
    mamaQuality                 quality,
    const char*                 symbol,
    short                       cause,
    const void*                 opaque,
    void*                       closure
)
{
    if (mama_getLogLevel () > MAMA_LOG_LEVEL_WARN)
    {
        switch (quality)
        {
            case MAMA_QUALITY_OK:
            case MAMA_QUALITY_MAYBE_STALE:
            case MAMA_QUALITY_STALE:
            case MAMA_QUALITY_PARTIAL_STALE:
            case MAMA_QUALITY_FORCED_STALE:
            case MAMA_QUALITY_UNKNOWN:
            {
                fprintf (gFilep, "Quality changed to %s for %s\n",
                        mamaQuality_convertToString (quality),
                        symbol);
                break;
            }
            default:
            {
                fprintf (gFilep, "WARNING %s Illegal quality value.\n", symbol);
                break;
            }
        }
    }
}

static void subscriptionOnError
(
    mamaSubscription            subscription,
    mama_status                 status,
    void*                       platformError,
    const char*                 subject,
    void*                       closure
)
{
    switch (status)
    {
        case MAMA_STATUS_TIMEOUT:
            if (mama_getLogLevel () > MAMA_LOG_LEVEL_WARN)
            {
                if (gCountTimeout)
                {
                    fprintf (gFilep, "[%d]/[%d] Subscriptions Timed out\n",
                             gCountTimeout,
                             gNumSymbols);
                    ++gCountTimeout;
                }
            }
            break;
        default:
            if (mama_getLogLevel () > MAMA_LOG_LEVEL_WARN)
            {
                fprintf (gFilep,
                         "Error [%s] occurred on subscription for %s: %s\n",
                         mama_getLastErrorText(),
                         subject,
                         mamaStatus_stringForStatus (status));
            }
    }
}

static void displayCallback
(
    mamaTimer               timer,
    void*                   closure
)
{
    statsGroup* lats;
    pthread_mutex_lock(&gMutex);
    ++gStatsCount;
    lats = &(gStatsGroups[gStatsGroupIndex%NUM_STATS_GROUPS]);

    if (gRdtsc) lats->mLastTsc = rdtsc();
    else        mamaDateTime_setToNow(lats->mLastTime);

    displayAndMoveStatsGroup ();

    pthread_mutex_unlock(&gMutex);

    if ((gStatsCount == gStartLogging) && (gCollectStats))
        mc_enqueueEvent(startLoggingToBuckets, NULL);
}

static void startLoggingToBuckets
(
    mamaQueue queue,
    void* closure
)
{
    fprintf (gFilep, "Begin Logging to Buckets\n");
    gLogResults=1;
}

static void rateStepCallback
(
    mamaQueue queue,
    void* closure
)
{
    fprintf (gFilep, "Rate Step to %u%% - %" PRIu64 " msgs/sec\n",
            gStepClosure.mPercent,
            gStepClosure.mTarget);
}

static void stopLoggingToBuckets
(
    mamaQueue queue,
    void* closure
)
{
    gLogResults=0;
}

/****************************************************************
 * STATS FUNCTIONS
 ****************************************************************/

static void createStatsGroup
(
    statsGroup* pStatsGroup
)
{
    mamaDateTime_create(&(pStatsGroup->mLastTime));
    mamaDateTime_create(&(pStatsGroup->mFirstTime));
    mamaDateTime_create(&pStatsGroup->mMsgTime);
    mamaDateTime_create(&pStatsGroup->mLineTime);
    mamaDateTime_create(&pStatsGroup->mExTime);
    mamaMsgIterator_create (&pStatsGroup->mItr, NULL);

    createStats(&(pStatsGroup->mInt));
    createStats(&(pStatsGroup->mTrans));
    createStats(&(pStatsGroup->mPlat));
    createStats(&(pStatsGroup->mEx));
    createStats(&(pStatsGroup->mTot));
    createStats(&(pStatsGroup->mCallback));

    initialiseStatsGroup(pStatsGroup);
}

static void createStats
(
    stats* pStats
)
{
    if (gCollectStats)
    {
        pStats->mBuckets= (uint64_t*) malloc (gBucketCount * sizeof (uint64_t));
        if (pStats->mBuckets == NULL)
        {
            PERROR ("ERROR: createStats()-unable to alloc memory for buckets-");
            exit(1);
        }
    }
    initialiseStats(pStats);
}

static void destroyStatsGroup
(
    statsGroup* pStatsGroup
)
{
    mamaDateTime_destroy(pStatsGroup->mFirstTime);
    mamaDateTime_destroy(pStatsGroup->mLastTime);
    mamaDateTime_destroy(pStatsGroup->mMsgTime);
    mamaDateTime_destroy(pStatsGroup->mLineTime);
    mamaDateTime_destroy(pStatsGroup->mExTime);
    mamaMsgIterator_destroy (pStatsGroup->mItr);
    destroyStats(&(pStatsGroup->mInt));
    destroyStats(&(pStatsGroup->mTrans));
    destroyStats(&(pStatsGroup->mPlat));
    destroyStats(&(pStatsGroup->mEx));
    destroyStats(&(pStatsGroup->mTot));
}

static void destroyStats
(
    stats* pStats
)
{
    if (gCollectStats) free(pStats->mBuckets);
}


static void initialiseStatsGroup
(
    statsGroup* pStatsGroup
)
{
    initialiseStats(&(pStatsGroup->mTrans));
    initialiseStats(&(pStatsGroup->mPlat));
    initialiseStats(&(pStatsGroup->mInt));
    initialiseStats(&(pStatsGroup->mEx));
    initialiseStats(&(pStatsGroup->mTot));
    initialiseStats(&(pStatsGroup->mCallback));

    pStatsGroup->mNumRateMsgs=0;
    pStatsGroup->mTransportGaps=0;
    pStatsGroup->mGaps=0;
    pStatsGroup->mMissingFids=0;
    pStatsGroup->mMsgSize=0;
    pStatsGroup->mFirstTsc=0;
    pStatsGroup->mLastTsc=0;
}

static void initialiseStats
(
    stats* pStats
)
{
    pStats->mMin=INT64_MAX;
    pStats->mTotal=0;
    pStats->mMax=INT64_MIN;
    pStats->mSqTotal=0;
    pStats->mNumMsgs=0;

    if (gCollectStats)
        memset (pStats->mBuckets, 0, gBucketCount * sizeof (uint64_t));
}

static void displayStatsGroupBucketsCallback
(
    mamaQueue queue,
    void* closure
)
{
    statsGroup* lats = (statsGroup*) closure;
    displayStatsGroupBuckets (lats);
    initialiseStatsGroup (lats);
}

static void displayStatsGroupCallback
(
    mamaQueue queue,
    void* closure
)
{
    statsGroup* lats = (statsGroup*) closure;

    displayStatsGroup (lats);
    if (gLogResults) addStatsGroups (&gStatsGroup,lats);
    initialiseStatsGroup (lats);
}

static void displayStatsGroup
(
    statsGroup*     pStatsGroup
)
{
    uint64_t nowTsc=0;
    char timeString[40] = {0x0};
    int64_t diffUsec = 0;

    /*Print Date Time*/
    if (gRdtsc)
    {
        nowTsc=rdtsc();
        mamaDateTime_setToNow (pStatsGroup->mLastTime);

        if (((double)(nowTsc - pStatsGroup->mLastTsc)
                    / (double)gCpuFreq) >= 1000000)
        {
            mamaDateTime_addSeconds(
                    pStatsGroup->mLastTime,
                    -1*(int64_t)(((double)(nowTsc - pStatsGroup->mLastTsc))
                        / (double)(gCpuFreq*1000000)));
        }
        diffUsec= (int64_t) (
                (double)(pStatsGroup->mLastTsc - pStatsGroup->mFirstTsc)
                    / (double)gCpuFreq);
    }
    else
    {
        mamaDateTime_diffMicroseconds(pStatsGroup->mLastTime,
                                      pStatsGroup->mFirstTime,
                                      &diffUsec);
    }

    if (diffUsec == 0) return;
    mamaDateTime_getAsFormattedStringWithTz(
            pStatsGroup->mLastTime,
            timeString,
            40,
            "%Y/%m/%d - %H:%M:%S",
            gTimeZone);

    fprintf (gFilep, "%21s",timeString);
    if (gStatsFilep) fprintf (gStatsFilep, "%s", timeString);

    /*Print Rate*/
    switch(pStatsGroup->mNumRateMsgs)
    {
        case 0:
        {
            fprintf (gFilep, " %8d",0);
            if (gStatsFilep) fprintf (gStatsFilep, ",0");
            break;
        }
        case 1:
        {
            fprintf (gFilep, " %8d",1);
            if (gStatsFilep) fprintf (gStatsFilep, ",1");
            break;
        }
        default:
        {
            fprintf (gFilep, " %8lu",
                     (pStatsGroup->mNumRateMsgs*1000000)/diffUsec);
            if (gStatsFilep)
                fprintf (gStatsFilep, ",%lu",
                         (pStatsGroup->mNumRateMsgs*1000000)/diffUsec);
            break;
        }
    }

    if (gCB)    displayStats(&pStatsGroup->mCallback);

    if (gDataStats)
    {
        if (pStatsGroup->mNumRateMsgs>0)
        {
            fprintf (gFilep, " %10lu",
                     pStatsGroup->mMsgSize/pStatsGroup->mNumRateMsgs);
            if (gStatsFilep)
                fprintf (gStatsFilep, ",%lu",
                         pStatsGroup->mMsgSize/pStatsGroup->mNumRateMsgs);
        }
        else
        {
            fprintf (gFilep, " %10d",0);
            if (gStatsFilep) fprintf (gStatsFilep, ",0");
        }
    }

    /*Latency stats*/
    if (gIntLat)    displayStats(&pStatsGroup->mInt);
    if (gTransLat)  displayStats(&pStatsGroup->mTrans);
    if (gPlatLat)   displayStats(&pStatsGroup->mPlat);
    if (gExLat)     displayStats(&pStatsGroup->mEx);
    if (gTotLat)    displayStats(&pStatsGroup->mTot);

    fprintf (gFilep, " %8" PRIu64,pStatsGroup->mTransportGaps);
    if (gStatsFilep) fprintf (gStatsFilep, ",%" PRIu64, pStatsGroup->mTransportGaps);

    fprintf (gFilep, " %8" PRIu64,pStatsGroup->mGaps);
    if (gStatsFilep) fprintf (gStatsFilep, ",%" PRIu64, pStatsGroup->mGaps);

    if (!gIntervalStats)
    {
        fprintf (gFilep, " %12" PRIu64, pStatsGroup->mMissingFids);
        if (gStatsFilep) fprintf (gStatsFilep, ",%" PRIu64,
                                  pStatsGroup->mMissingFids);
    }

    fprintf (gFilep, "\n");
    if (gStatsFilep) fprintf (gStatsFilep, "\n");
}

static void addStatsGroups
(
    statsGroup*     pResStatsGroup,
    statsGroup*     pStatsGroup
)
{
    addStats(&(pResStatsGroup->mTrans),&(pStatsGroup->mTrans));
    addStats(&(pResStatsGroup->mPlat),&(pStatsGroup->mPlat));
    addStats(&(pResStatsGroup->mEx),&(pStatsGroup->mEx));
    addStats(&(pResStatsGroup->mInt),&(pStatsGroup->mInt));
    addStats(&(pResStatsGroup->mTot),&(pStatsGroup->mTot));
    addStats(&(pResStatsGroup->mCallback),&(pStatsGroup->mCallback));
    pResStatsGroup->mNumRateMsgs += pStatsGroup->mNumRateMsgs;
    pResStatsGroup->mTransportGaps += pStatsGroup->mTransportGaps;
    pResStatsGroup->mGaps += pStatsGroup->mGaps;
    pResStatsGroup->mMissingFids += pStatsGroup->mMissingFids;
    pResStatsGroup->mMsgSize += pStatsGroup->mMsgSize;

    /*
     * Only set the times if the stat group being added has messages,
     * Otherwise the times are not valid
     */
    if (pStatsGroup->mNumRateMsgs>0)
    {
        /* Set first time to earliest */
        if (pResStatsGroup->mFirstTsc > pStatsGroup->mFirstTsc)
            pResStatsGroup->mFirstTsc = pStatsGroup->mFirstTsc;
        /* Set lat time to latest */
        if (pResStatsGroup->mLastTsc < pStatsGroup->mLastTsc)
            pResStatsGroup->mLastTsc = pStatsGroup->mLastTsc;

        /*
         * Compare two date/time objects.  The return value is negative if lhs
         * is earlier than rhs, positive if lhs is greater than rhs and zero
         * if the two are equal.
         */

        /* Set first time to earliest */
        if (mamaDateTime_compare(pResStatsGroup->mFirstTime,
                                 pStatsGroup->mFirstTime) > 0)
        {
            mamaDateTime_copy(pResStatsGroup->mFirstTime,
                              pStatsGroup->mFirstTime);
        }
        /* Set last time to latest */
        if (mamaDateTime_compare(pResStatsGroup->mLastTime,
                                 pStatsGroup->mLastTime) < 0)
        {
            mamaDateTime_copy(pResStatsGroup->mLastTime,
                              pStatsGroup->mLastTime);
        }
    }
}

static void addStats
(
    stats*     pResStats,
    stats*     pStats
)
{
    int i =0;

    if (pResStats->mMin > pStats->mMin) pResStats->mMin = pStats->mMin;
    if (pResStats->mMax < pStats->mMax) pResStats->mMax = pStats->mMax;

    pResStats->mNumMsgs += pStats->mNumMsgs;
    pResStats->mTotal += pStats->mTotal;
    pResStats->mSqTotal += pStats->mSqTotal;

    if (gCollectStats)
    {
        for(i=0; i<gBucketCount; ++i)
        {
            pResStats->mBuckets[i]+=pStats->mBuckets[i];
        }
    }
}

static void displayStats
(
    stats*      pStats
)
{
    uint64_t i = 0;
    uint64_t percentileIndex = 0;
    uint64_t totalNumMsgs, targetNumMsgs, numMsgs = 0;
    int64_t  limit = 0;

    if (pStats->mNumMsgs>0)
    {
        fprintf (gFilep, " %10" PRIi64 " %10" PRIi64,
                pStats->mMin,
                pStats->mTotal/(int64_t)pStats->mNumMsgs);
        if (gStatsFilep) fprintf (gStatsFilep, ",%10" PRIi64 ",%10" PRIi64,
                pStats->mMin,
                pStats->mTotal/(int64_t)pStats->mNumMsgs);
    }
    else
    {
        fprintf (gFilep, " %10d %10d",0,0);
        if (gStatsFilep) fprintf(gStatsFilep, ",0,0");
    }

    if (gNoPercentiles > 0)
    {
        totalNumMsgs=0;
        for(i=0; i<gBucketCount; ++i) totalNumMsgs+=pStats->mBuckets[i];

        percentileIndex=0;

        targetNumMsgs = ceil ((totalNumMsgs*gPercentiles[percentileIndex])/100);

        numMsgs=0;
        i = 0;

        while (percentileIndex < gNoPercentiles)
        {
            if (i < gBucketCount)
                numMsgs += pStats->mBuckets[i];
            ++i;

            while (numMsgs >= targetNumMsgs)
            {
                if (i != gBucketCount -1 )  limit = i * gBucketWidth;
                else                        limit = pStats->mMax;

                if (limit > pStats->mMax)
                    limit = pStats->mMax;

                fprintf (gFilep, " %10" PRIi64, limit);
                if (gStatsFilep) fprintf(gStatsFilep, ",%10" PRIi64, limit);

                ++percentileIndex;
                if (percentileIndex < gNoPercentiles)
                    targetNumMsgs = ceil((totalNumMsgs * gPercentiles[percentileIndex])/100);
                else
                    break;
            }
        }
    }

    if (pStats->mNumMsgs>0)
    {
        fprintf (gFilep, " %10" PRIi64, pStats->mMax);
        if (gStatsFilep) fprintf (gStatsFilep, ",%10" PRIi64, pStats->mMax);
    }
    else
    {
        fprintf (gFilep, " %10d",0);
        if (gStatsFilep) fprintf(gStatsFilep, ",0");
    }
    if (gSD)
    {
        if (pStats->mNumMsgs>0)
        {
            fprintf (gFilep, " %10.2f",
                    (double) sqrt (((double)pStats->mSqTotal -
                          ((double)pStats->mTotal * (double)pStats->mTotal)
                          /(double)pStats->mNumMsgs)/(double)pStats->mNumMsgs));
            if (gStatsFilep)
            {
                fprintf (gStatsFilep,",%f",
                    (double) sqrt (((double)pStats->mSqTotal -
                          ((double)pStats->mTotal*(double)pStats->mTotal)
                          /(double)pStats->mNumMsgs)/(double)pStats->mNumMsgs));
            }
        }
        else
        {
            fprintf (gFilep, " %10.2f",0.0);
            if (gStatsFilep) fprintf(gStatsFilep, ",0.0");
        }
    }
}

static inline void processMsg
(
    mamaMsg       msg,
    statsGroup*   pStatsGroup,
    symbol*       sym
)
{
    int i, numFields = 0;
    mamaPayloadType payloadType = MAMA_PAYLOAD_UNKNOWN;
    mamaMsgField field = NULL;
    uint16_t fid;
    uint64_t lastMissingFids = pStatsGroup->mMissingFids;

    ++pStatsGroup->mNumRateMsgs;
    if (!gBasicSub)
    {
        /* Only process latency for updates */
        switch (mamaMsgType_typeForMsg (msg))
        {
            case MAMA_MSG_TYPE_INITIAL:
            case MAMA_MSG_TYPE_RECAP:
            case MAMA_MSG_TYPE_SNAPSHOT:
            case MAMA_MSG_TYPE_BOOK_RECAP:
            case MAMA_MSG_TYPE_BOOK_SNAPSHOT:
            case MAMA_MSG_TYPE_BOOK_INITIAL:
            case MAMA_MSG_TYPE_END_OF_INITIALS:
            case MAMA_MSG_TYPE_DDICT_SNAPSHOT:
                return;
            default:
                break;
        }
    }

    mamaMsg_getPayloadType (msg, &payloadType);
    if (payloadType == MAMA_PAYLOAD_V5)
    {
        /* Use direct access for wirecache msgs */
        for (i = 0; i < gNumFields; ++i)
        {
            if (MAMA_STATUS_OK == mamaMsg_getField (msg,
                                                    NULL,
                                                    gFields[i].mFid,
                                                    &field))
            {
                if (gFields[i].mCb (field, sym, pStatsGroup))
                    ++pStatsGroup->mMissingFids;
            }
            else
                ++pStatsGroup->mMissingFids;
        }
    }
    else
    {
        /* Use iterator for all other types */
        mamaMsgIterator_associate (pStatsGroup->mItr, msg);

        while ((field = mamaMsgIterator_next (pStatsGroup->mItr)) != NULL )
        {
            if (MAMA_STATUS_OK == mamaMsgField_getFid (field, &fid))
            {
                if (gFieldsByFid[fid] != NULL)
                {
                    if (gFieldsByFid[fid]->mCb (field, sym, pStatsGroup))
                        ++pStatsGroup->mMissingFids;
                    ++numFields;
                    if (numFields == gNumFields)
                        break;
                }
            }
        }
        pStatsGroup->mMissingFids += gNumFields - numFields;
    }

    if (lastMissingFids == pStatsGroup->mMissingFids)
    {
        if (gTransLat)
        {
            if (gRdtsc)
            {
                pStatsGroup->mTransLat = (uint64_t)
                  ((double)((pStatsGroup->mLastTsc - pStatsGroup->mMsgTsc)*1000)
                                  / (double)gCpuFreq);
                updateStats (&pStatsGroup->mTrans, pStatsGroup->mTransLat);
            }
            else
            {
                mamaDateTime_diffMicroseconds (pStatsGroup->mLastTime,
                                               pStatsGroup->mMsgTime,
                                               &pStatsGroup->mTransLat);
                updateStats (&(pStatsGroup->mTrans), pStatsGroup->mTransLat);
            }
        }
        if (gIntLat)
        {
            mamaDateTime_diffMicroseconds (pStatsGroup->mMsgTime,
                                           pStatsGroup->mLineTime,
                                           &pStatsGroup->mIntLat);
            updateStats (&(pStatsGroup->mInt), pStatsGroup->mIntLat);
        }
        if (gPlatLat)
        {
            mamaDateTime_diffMicroseconds (pStatsGroup->mLastTime,
                                           pStatsGroup->mLineTime,
                                           &pStatsGroup->mPlatLat);
            updateStats (&(pStatsGroup->mPlat), pStatsGroup->mPlatLat);
        }
        if (gExLat)
        {
            mamaDateTime_diffMicroseconds (pStatsGroup->mLineTime,
                                           pStatsGroup->mExTime,
                                           &pStatsGroup->mExLat);
            updateStats (&pStatsGroup->mEx, pStatsGroup->mExLat);
        }
        if (gTotLat)
        {
            mamaDateTime_diffMicroseconds (pStatsGroup->mLastTime,
                                           pStatsGroup->mExTime,
                                           &pStatsGroup->mTotLat);
            updateStats (&pStatsGroup->mTot, pStatsGroup->mTotLat);
        }
    }
}


/****************************************************************
 * ON MESSAGE CALLBACKS
 ****************************************************************/

static void subscriptionOnMsg
(
    mamaSubscription            subscription,
    mamaMsg                     msg,
    void*                       closure,
    void*                       itemClosure
)
{
    int64_t gapSinceDisplay = 0;
    struct timespec ts1,ts2;
    int64_t diff = 0;
    uint64_t startTsc, endTsc =0;
    mama_size_t msgLen;
    symbol* sym = (symbol*)closure;
    statsGroup* lats;

    if (gCB)
    {
        if (gRdtsc) startTsc=rdtsc();
        else        clock_gettime(CLOCK_REALTIME, &ts1);
    }

    /* TODO: minimise lock to just the stats group */

    if (gStatsOnTimers || gThreads > 1)
        pthread_mutex_lock(&gMutex);

    lats = &(gStatsGroups[gStatsGroupIndex%NUM_STATS_GROUPS]);

    if (gRdtsc) lats->mLastTsc = rdtsc();
    else        mamaDateTime_setToNow(lats->mLastTime);

    processMsg (msg, lats, sym);

    if (gCountInitials)
    {
        if (mamaMsgType_typeForMsg (msg) == MAMA_MSG_TYPE_INITIAL)
        {
            mc_enqueueEvent(countInitialCallback, NULL);
        }
    }

    if (gDataStats)
    {
        mamaMsg_getByteSize (msg, &msgLen);
        lats->mMsgSize += msgLen;
    }

    /*If not using timer to display stats*/
    if (!gStatsOnTimers)
    {
        if ((gRdtsc && ((lats->mLastTsc - lats->mFirstTsc) > gTscStatInterval))
                || (!gRdtsc && MAMA_STATUS_OK ==
                        mamaDateTime_diffMicroseconds(lats->mLastTime,
                                                      lats->mFirstTime,
                                                      &gapSinceDisplay)
                    && (gapSinceDisplay >= gUsecStatInterval)))
        {
            ++gStatsCount;
            if (gTimeInterval)
                lats = displayAndMoveStatsGroup ();

            if (gStatsCount < gStartLogging)
            {
                lats = moveStatsGroup ();
            }
            else if (gCollectStats && (gStatsCount == gStartLogging))
            {
                /*Start logging to the global buckets now*/
                mc_enqueueEvent (startLoggingToBuckets, NULL);
                /*
                 * Move onto the next stats group
                 * Important if gTimeInterval is not set and only want latencies
                 * from this point on to be added to the global stats group
                 */
                lats = moveStatsGroup ();
            }
        }
    }

    if (gStatsOnTimers || gThreads > 1)
        pthread_mutex_unlock(&gMutex);

    if (gCB)
    {
        if (gRdtsc)
        {
            endTsc=rdtsc();
            diff = (uint64_t) ((double)((endTsc - startTsc)*1000)
                                    / (double)gCpuFreq);
        }
        else
        {
            clock_gettime(CLOCK_REALTIME, &ts2);
            diff = ts2.tv_nsec - ts1.tv_nsec;
            if (diff < 0)
                diff = ts2.tv_nsec + (1000000000 - ts1.tv_nsec);
        }
        if (diff != 0)
            updateStats (&lats->mCallback, diff);
    }
}

static void subscriptionOnMsgStep
(   mamaSubscription            subscription,
    mamaMsg                     msg,
    void*                       closure,
    void*                       itemClosure
)
{
    statsGroup* lats;

    if (gStatsOnTimers || gThreads > 1)
        pthread_mutex_lock(&gMutex);
    lats = &(gStatsGroups[gStatsGroupIndex%NUM_STATS_GROUPS]);

    if (gRdtsc) lats->mLastTsc = rdtsc();
    else        mamaDateTime_setToNow (lats->mLastTime);

    /* Display the current stats group */
    displayAndMoveStatsGroup ();
    if (gCollectStats)
    {
        /* Display the buckets of the global group */
        mc_enqueueEvent(displayStatsGroupBucketsCallback, &gStatsGroup);
        gStatsCount=0;
        mc_enqueueEvent(stopLoggingToBuckets, NULL);
    }
    mamaMsg_getU32 (msg, NULL, STEP_PERCENT, &gStepClosure.mPercent);
    mamaMsg_getU64 (msg, NULL, STEP_TARGET,  &gStepClosure.mTarget);

    mc_enqueueEvent(rateStepCallback, NULL);

    if (gStatsOnTimers || gThreads > 1)
        pthread_mutex_unlock(&gMutex);
}

static void subscriptionOnMsgRate
(   mamaSubscription            subscription,
    mamaMsg                     msg,
    void*                       closure,
    void*                       itemClosure
)
{
    uint32_t retSeqNum = 0;
    symbol* sym = (symbol*)closure;
    statsGroup* lats;

    if (gStatsOnTimers || gThreads > 1)
        pthread_mutex_lock(&gMutex);

    lats = &(gStatsGroups[gStatsGroupIndex%NUM_STATS_GROUPS]);

    if (gRdtsc) lats->mLastTsc=rdtsc();
    else        mamaDateTime_setToNow(lats->mLastTime);

    ++lats->mNumRateMsgs;

    if (gBasicSub)
    {
        mamaMsg_getU32 (msg, NULL, SEQ_NUM_FID, &retSeqNum);
        if (sym->mSeqNum && retSeqNum != ++sym->mSeqNum)
            ++lats->mGaps;
        sym->mSeqNum = retSeqNum;
    }

    if (gStatsOnTimers || gThreads > 1)
        pthread_mutex_unlock(&gMutex);
}

static void subscriptionOnMsgPerf
(   mamaSubscription            subscription,
    mamaMsg                     msg,
    void*                       closure,
    void*                       itemClosure
)
{
    int64_t gapSinceDisplay = 0;
    symbol* sym = (symbol*)closure;
    statsGroup* lats;

    if (gStatsOnTimers || gThreads > 1)
        pthread_mutex_lock(&gMutex);

    lats = &(gStatsGroups[gStatsGroupIndex%NUM_STATS_GROUPS]);
    if (gRdtsc) lats->mLastTsc = rdtsc();
    else        mamaDateTime_setToNow(lats->mLastTime);

    processMsg (msg, lats, sym);

    /*If not using timer to display stats*/
    if (!gStatsOnTimers)
    {
        mamaDateTime_diffMicroseconds(lats->mLastTime,
                                      lats->mFirstTime,
                                      &gapSinceDisplay);
        if ((gRdtsc && ((lats->mLastTsc - lats->mFirstTsc) > gTscStatInterval))
                || (!gRdtsc && gapSinceDisplay >= gUsecStatInterval))
        {
            if (gTimeInterval)
                lats = displayAndMoveStatsGroup ();
        }
    }

    if (gStatsOnTimers || gThreads > 1)
        pthread_mutex_unlock(&gMutex);
}

static void subscriptionOnMsgInterval
(   mamaSubscription            subscription,
    mamaMsg                     msg,
    void*                       closure,
    void*                       itemClosure
)
{
    static uint64_t msgCount=0;
    int64_t transLatency = 0;
    symbol* sym = (symbol*)closure;
    uint32_t retSeqNum = 0;
    uint64_t msgTime =0;
    statsGroup* lats;

    if (gThreads > 1)
        pthread_mutex_lock(&gMutex);

    lats = &(gStatsGroups[gStatsGroupIndex%NUM_STATS_GROUPS]);

    if (gRdtsc) lats->mLastTsc = rdtsc();
    else        mamaDateTime_setToNow(lats->mLastTime);

    if (gBasicSub)
    {
        mamaMsg_getU32 (msg, NULL, SEQ_NUM_FID, &retSeqNum);
        if (sym->mSeqNum && retSeqNum != ++sym->mSeqNum)
            ++lats->mGaps;
        sym->mSeqNum = retSeqNum;
    }

    if (msgCount++ < gTargetMsgCount)
        return;

    ++lats->mNumRateMsgs;
    if (gRdtsc)
    {
        mamaMsg_getU64 (msg, NULL, SEND_TIME_FID, &msgTime);
        transLatency = (uint64_t) ((double)((lats->mLastTsc - msgTime)*1000)
                                        / (double)gCpuFreq);
        updateStats (&lats->mTrans, transLatency);
    }
    else
    {
        if (MAMA_STATUS_OK != (mamaMsg_getDateTime (msg,
                                                    NULL,
                                                    SEND_TIME_FID,
                                                    lats->mMsgTime)))
        {
            ++lats->mMissingFids;
        }
        else
        {
            mamaDateTime_diffMicroseconds (lats->mLastTime,
                                           lats->mMsgTime,
                                           &transLatency);
            updateStats (&lats->mTrans, transLatency);
        }
    }

    if (lats->mNumRateMsgs >= gIntervalRate)
    {
        displayAndMoveStatsGroup ();
        gTargetMsgCount = msgCount+gSampleRate;
    }

    if (gThreads > 1)
        pthread_mutex_unlock(&gMutex);
}

static void parseCommandLine
(   int                     argc,
    const char**            argv,
    const char**            topic,
    const char**            middleware
)
{
    time_t nTime;
    struct tm * now;
    char nowTime [40] = {0x0};
    int i = 1;

    for (i = 1; i < argc;)
    {
        /*
         * Flag command line options. i.e. no trailing value
         */
        if (strcmp (argv[i], "-countInitial") == 0)
        {
            gCountInitials = 1;
            i++;
        }
        else if (strcmp (argv[i], "-inline") == 0)
        {
            gEventsInline = 1;
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
        else if (strcmp (argv[i], "-countTimeout") == 0)
        {
            gCountTimeout = 1;
            i++;
        }
        else if (strcmp (argv[i], "-stats-timers") == 0)
        {
            gStatsOnTimers=1;
            i++;
        }
        else if (strcmp (argv[i], "-fh") == 0)
        {
            gIntLat = 1;
            i++;
        }
        else if (strcmp (argv[i], "-tc") == 0)
        {
            gTransportCallbacks = 1;
            i++;
        }
        else if (strcmp (argv[i], "-no-trans") == 0)
        {
            gTransLat = 0;
            i++;
        }
        else if (strcmp (argv[i], "-plat") == 0)
        {
            gPlatLat = 1;
            i++;
        }
        else if (strcmp (argv[i], "-csv") == 0)
        {
            gStatsFile = argv[i+1];
            i += 2;
        }
        else if (strcmp (argv[i], "-file") == 0)
        {
            char* logDir = getenv ("WOMBAT_LOGS");

            if (NULL != logDir)
            {
                snprintf (gFile, PATH_MAX, "%s/%s", logDir, argv[i+1]);
                gFile[PATH_MAX-1] = '\0';
            }
            i += 2;
        }
        else if (strcmp (argv[i], "-total") == 0)
        {
            gTotLat = 1;
            i++;
        }
        else if (strcmp (argv[i], "-dataStats") == 0)
        {
            gDataStats = 1;
            i++;
        }
        else if (strcmp (argv[i], "-ex") == 0)
        {
            gExLat = 1;
            i++;
        }
        else if (strcmp (argv[i], "-mdsub") == 0)
        {
            gBasicSub = 0;
            i++;
        }
        else if (strcmp (argv[i], "-stats") == 0)
        {
            gCollectStats = 1;
            i++;
        }
        else if (strcmp (argv[i], "-cb") == 0)
        {
            gCB = 1;
            i++;
        }
        else if (strcmp (argv[i], "-sd") == 0)
        {
            gSD = 1;
            i++;
        }
        else if (strcmp (argv[i], "-I") == 0)
        {
            gRequiresInitial = 0;
            i++;
        }
        else if (strcmp (argv[i], "-rdtsc") == 0)
        {
            gRdtsc = 1;
            i ++;
        }
        else if (strcmp (argv[i], "-steps") == 0)
        {
            gSymbolList[gNumSymbols++].mTopic = strdup ("MAMA_STEP");
            i++;
        }
        else if (strcmp(argv[i], "-v") == 0)
        {
            if (mama_getLogLevel () == MAMA_LOG_LEVEL_WARN)
            {
                mama_enableLogging (stderr, MAMA_LOG_LEVEL_NORMAL);
            }
            else if (mama_getLogLevel () == MAMA_LOG_LEVEL_NORMAL)
            {
                mama_enableLogging (stderr, MAMA_LOG_LEVEL_FINE);
            }
            else if (mama_getLogLevel () == MAMA_LOG_LEVEL_FINE)
            {
                mama_enableLogging (stderr, MAMA_LOG_LEVEL_FINER);
            }
            else
            {
                mama_enableLogging (stderr, MAMA_LOG_LEVEL_FINEST);
            }
            i++;
        }
        else if ((strcmp(argv[i], "-h") == 0) ||
                (strcmp(argv[i], "--help") == 0) ||
                (strcmp(argv[i], "-?") == 0))
        {
            usage(0);
        }
        else
        {
            /*
            * Command line options with trailing value
            */
            if (i == argc-1)
            {
                fprintf (stderr,
                        "ERROR: Last Option %s is not a valid commandline flag."
                        " Ignoring\n",
                        argv[i]);
                i++;
            }
            else if (strcmp (argv[i], "-s") == 0)
            {
                gSymbolList[gNumSymbols++].mTopic = strdup (argv[i + 1]);
                i += 2;
            }
            else if ( strcmp( argv[i], "-app" ) == 0 )
            {
                gAppName = argv[i+1];
                i += 2;
            }
            else if (strcmp ("-priority", argv[i]) == 0)
            {
                gPriority = atoi (argv[i + 1]);
                i += 2;
            }
            else if (strcmp ("-genSyms", argv[i]) == 0)
            {
                gGenerateSymbols = atoi (argv[i + 1]);
                i += 2;
            }
            else if (strcmp ("-symRoot", argv[i]) == 0)
            {
                gSymbolRoot=(argv[i+1]);
                i+=2;
            }
            else if (strcmp (argv[i], "-threads") == 0)
            {
                gThreads = atoi (argv[i + 1]);
                i += 2;
            }
            else if (strcmp (argv[i], "-start-stats") == 0)
            {
                gStartLogging = atoi (argv[i + 1]);
                i += 2;
            }
            else if (strcmp ("-i", argv[i]) == 0)
            {
                gIntervalRate = (uint32_t)atoi(argv[i+1]);
                if (gIntervalRate < MIN_INTERVAL_RATE)
                {
                    fprintf (gFilep,
                            "Warning - interval rate must be a minimum of %d "
                            "messages\n",
                            MIN_SAMPLE_RATE);
                    gIntervalRate = MIN_INTERVAL_RATE;
                    fprintf (gFilep, "Modified interval rate to %d\n",
                             MIN_INTERVAL_RATE);
                }
                gIntervalStats=1;
                i += 2;
            }
            else if (strcmp (argv[i], "-bucketCount") == 0)
            {
                gBucketCount = strtol (argv[i+1], NULL, 10);
                gCollectStats = 1;
                i += 2;
            }
            else if (strcmp (argv[i], "-bucketWidth") == 0)
            {
                gBucketWidth = strtol (argv[i+1], NULL, 10);
                gCollectStats = 1;
                i += 2;
            }
            else if (strcmp (argv[i], "-high") == 0)
            {
                gHighWaterMark = strtol (argv[i+1], NULL, 10);
                i += 2;
            }
            else if (strcmp (argv[i], "-low") == 0)
            {
                gLowWaterMark = strtol (argv[i+1], NULL, 10);
                i += 2;
            }
            else if (strcmp (argv[i], "-throttle") == 0)
            {
                gThrottle = strtol (argv[i+1], NULL, 10);
                i += 2;
            }
            else if (strcmp ("-t", argv[i]) == 0)
            {
                gTimeInterval = strtod(argv[i+1], NULL);
                i += 2;
            }
            else if (strcmp (argv[i], "-timeout") == 0)
            {
                gTimeout = strtod (argv[i+1], NULL);
                i += 2;
            }
            else if (strcmp (argv[i], "-retries") == 0)
            {
                gRetries = strtod (argv[i+1], NULL);
                i += 2;
            }
            else if (strcmp (argv[i], "-f") == 0)
            {
                gSymFileName = argv[i + 1];
                i += 2;
            }
            else if (strcmp (argv[i], "-F") == 0)
            {
                gSymRateFileName = argv[i + 1];
                i += 2;
            }
            else if (strcmp("-tport", argv[i]) == 0)
            {
                gTransportName = argv[i+1];
                i += 2;
            }
            else if (strcmp ("-m", argv[i]) == 0)
            {
                *middleware = (argv[i+1]);
                i += 2;
            }
            else if (strcmp(argv[i], "-S") == 0)
            {
                gSymbolNamespace = argv[i+1];
                i += 2;
            }
            else if (strcmp ("-percentiles", argv[i]) == 0)
            {
                char* c = strdup(argv[i+1]);
                char* res = strtok (c , ",");
                gPercentiles = (double*)calloc (MAX_NUM_PERCENTILES,
                                                sizeof (double));

                while (res != NULL)
                {
                    if (gNoPercentiles<MAX_NUM_PERCENTILES)
                    {
                        gPercentiles[gNoPercentiles++] = strtod(res,NULL);
                    }
                    else
                    {
                        fprintf(stderr,
                              "ERROR:Only %i steps being used as this is max\n",
                              MAX_NUM_PERCENTILES);
                        break;
                    }
                    res = strtok (NULL , ",");
                }

                i += 2;
                gCollectStats=1;
            }
            else
            {
                fprintf(stderr,"ERROR: Commandline option %s not recognised. "
                        "Skipping\n",
                        argv[i]);
                i++;
            }
        }
    }
    fprintf (gFilep, "\n");

    time(&nTime);
    now = localtime(&nTime);
    strftime(nowTime, 100, "%Y/%m/%d - %H:%M:%S", now);

    fprintf (gFilep, "%21s Starting consumer\n",
            nowTime);
}

static void usage
(
    int                     exitStatus
)
{
    int i = 0;
    while (NULL != gUsageString[i])
    {
        fprintf (gFilep, "%s\n", gUsageString[i++]);
    }
    exit (exitStatus);
}

static void parseSymbols (void)
{
    char* dot;
    int i;

    for (i = 0; i < gNumSymbols; ++i)
    {
        /* If source name configured (-S) use this */
        if (gSymbolNamespace)
        {
            gSymbolList[i].mSource = gSymbolNamespace;
            gSymbolList[i].mName = gSymbolList[i].mTopic;
        }
        else
        {
            /* Source name is up until the first dot */
            dot = NULL;
            dot = strchr (gSymbolList[i].mTopic, '.');
            if (dot)
            {
                *dot = '\0';
                gSymbolList[i].mSource = gSymbolList[i].mTopic;
                gSymbolList[i].mName = dot + 1;
            }
            else
                gSymbolList[i].mName = gSymbolList[i].mTopic;
        }

        /*
         * check the source for this symbol - if it exists in list return the
         * index else add and return index
         */
        if (gSymbolList[i].mSource)
        {
            findSourceInList (gSymbolList[i].mSource,
                              &(gSymbolList[i].mSourceIndex));
        }
    }
}

static void readSymbolsFromFile
(
    char*                   file
)
{
    /* get subjects from file or interactively */
    FILE* fp = NULL;
    char charbuf[1024];

    if (file)
    {
        if ((fp = fopen (file, "r")) == (FILE *)NULL)
        {
            fprintf (gFilep, "Unable to open file %s\n", file);
            exit (1);
        }
    }
    else
    {
        fp = stdin;
    }
    if (!file)
    {
        fprintf (gFilep, "Enter one symbol per line and terminate with a .\n");
        fprintf (gFilep, "Symbol> ");
    }

    /* Just strdup here. Calculation of SOURCE happens later */
    while (fgets (charbuf, 1023, fp))
    {
        char *c = charbuf;

        /* Input terminate case */
        if (*c == '.')
            break;

        while ((*c != '\0') && (*c != '\n'))
            ++c;
        /* Ensure termination character is NULL */
        *c = '\0';

        gSymbolList[gNumSymbols].mTopic = strdup (charbuf);

        ++gNumSymbols;
        if (isatty(fileno (fp)))
            fprintf (gFilep, "Symbol> ");
    }

    if (fclose(fp)==EOF)
        PERROR("ERROR: readSymbolsFromFile()-failed to close file");
}

static void setQueueMonitors (mamaQueue* localQueue)
{
    if (gHighWaterMark>0 || gLowWaterMark>0)
    {
        mamaQueueMonitorCallbacks queueCallbacks;

        queueCallbacks.onQueueHighWatermarkExceeded = highWaterMarkCallback;
        queueCallbacks.onQueueLowWatermark  = lowWaterMarkCallback;

        mamaQueue_setQueueMonitorCallbacks (*localQueue,
                                            &queueCallbacks,
                                            NULL);
        if (gHighWaterMark > 0)
        {
            MAMA_CHECK (mamaQueue_setHighWatermark(*localQueue,
                                                   gHighWaterMark));
        }
        if (gLowWaterMark > 0)
        {
            MAMA_CHECK (mamaQueue_setLowWatermark(*localQueue, gLowWaterMark));
        }
    }
}

static void highWaterMarkCallback
(   mamaQueue                   queue,
    size_t                      size,
    void*                       closure
)
{
    const char* queueName   = NULL;

    if (queue)
    {
        MAMA_CHECK (mamaQueue_getQueueName (queue, &queueName));
    }
    else /*Default queue*/
    {
        queueName = "DEFAULT_QUEUE";
    }

    fprintf (gFilep,
            "%s: Queue high water mark exceeded: %lu. "
            "Shutting down to stop consuming all memory\n",
            queueName,
            size);

    signalCatcher(SIGUSR1);
}


static void lowWaterMarkCallback
(   mamaQueue                   queue,
    size_t                      size,
    void*                       closure
)
{
    const char* queueName   = NULL;

    if (queue)
    {
        MAMA_CHECK (mamaQueue_getQueueName (queue, &queueName));
    }
    else /*Default queue*/
    {
        queueName = "DEFAULT_QUEUE";
    }
    fprintf (gFilep, "%s: Queue below low water mark: %lu\n", queueName, size);
}

static void consumerNanoSleep
(
    unsigned                nsec
)
{
    struct timespec     st,et;
    uint64_t            start;
    int64_t             delta;

    clock_gettime (CLOCK_PROCESS_CPUTIME_ID, &st);

    start = TS2NANO (&st);
    while(1)  {

        clock_gettime (CLOCK_PROCESS_CPUTIME_ID, &et);
        delta = TS2NANO (&et) - start - nsec;
        if (delta >= 0)
        {
            return;
        }
    }
}

static void generateSymbols
(
    void
)
{

    int i=0;
    char charbuf[1024];
    char* tmp = strdup (charbuf);

    for (i = 0; i < gGenerateSymbols; ++i)
    {
        snprintf (tmp, 1024, "%s%.2d", gSymbolRoot, i);
        gSymbolList[i].mTopic = strdup(tmp);
    }
    gNumSymbols=i;
}

static void signalCatcher
(
    int                     sig
)
{
    static int appStopped = 0;

    if (sig == SIGINT)
        fprintf (gFilep, "SIGINT caught. Shutting down\n");
    else if (sig == SIGUSR1)
        fprintf (gFilep, "Internal signal caught. Shutting down\n");
    else
        fprintf (gFilep, "Unknown signal caught. Shutting down\n");

    if (!appStopped)
    {
        /* Stop MAMA */
        mama_stop (gMamaBridge);
        appStopped = 1;
    }
}

static double getClock(void)
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
