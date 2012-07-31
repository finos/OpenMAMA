/* $Id: mamapingpongc.c,v 1.1.2.3 2011/09/29 17:56:53 emmapollock Exp $
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

#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>
#include <pthread.h>

#include "mama/mama.h"
#include "mama/queue.h"

/* MAMA function checking macro */
#define MAMA_CHECK(status)                                                   \
{                                                                            \
 mama_status __status = status;                                              \
 if (MAMA_STATUS_OK != __status) {                                              \
   printf("Function error - function returned %s - error code %d at %s:%d\n", \
     mamaStatus_stringForStatus(__status), __status, __FILE__, __LINE__);         \
   exit(__status);                                                              \
 }                                                                            \
}


#define TS2NANO(x) ((x)->tv_sec * 1000000000 + (x)->tv_nsec)
#define MIN_SAMPLE_RATE         1
#define MIN_INTERVAL_RATE       10
#define PING_PONG_FID           16
#define PADDING_FID             10004
#define SEQ_NUM_FID             17
#define MIN_POW_SIZE            4
#define MAX_POW_SIZE            16
#define SOURCE_SIZE             50
#define MAMA_MSG_MAX_FIELD_SIZE UINT16_MAX
#define DEFAULT_MAX_MSG_SIZE    1048576 /* 1MB */
#define DEFAULT_MIN_SIZE        (2*sizeof(uint64_t))  /* 1MB */

enum syncState {
    SYNC_INIT   = 0,
    SYNC_SENT,
    SYNC_RESPONDED,
    SYNC_REACHED,
    SYNC_COMPLETE
};

typedef struct {
    mamaMsg     mMamaMsg;
    size_t      mMsgSize;
    int64_t     mMinLatency;
    int64_t     mMaxLatency;
    int64_t     mRunningLatency;
    uint64_t    mRunningSqLatency;
    uint64_t    mNumMsgs;
} message;

typedef struct {
    mamaTimer           mTimer;
    int                 mShuttingDown;
    enum syncState      mSyncState;
    uint8_t             mServer;
    uint8_t             mRdtsc;
    uint32_t            mStopCount;
    message*            mMsgs;
    uint32_t            mNumMsgs;
    uint64_t            mSyncSeqNum;
    mamaPublisher       mPublisher;
} pingPongEventCtx;

typedef struct {
    mamaSubscription    mSubscription;
    mamaBridge          mBridge;
    mamaQueue           mQueue;
    mamaDispatcher      mDispatcher;
    mamaTransport       mTransport;
} pingPongCtx;

static __inline__ uint64_t
rdtsc(void) {
  uint32_t lo, hi;
  __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
  return (uint64_t)hi << 32 | lo;
}

static const char*          gAppName            = "mamapingpong";
static int                  gSD                 = 0;
static int                  gIntervalRate       = 0;
pthread_mutex_t             gPendingDestroyLock;
pthread_cond_t              gPendingDestroyCond;
static int                  gSubscDestroyed     = 0;

static double               gCpuFreq;

static const char *         gUsageString[] =
{
"mamapingpong",
"Usage: mamapingpong [OPTIONS]",
"Provides bidirectional transport latency. Only one outstanding message at any time.",
"",
"EXAMPLE COMMANDLINES:",
"Requester: mamapingpong -tport tcp_pub -a -rdtsc",
"Responder: mamapingpong -tport tcp_pub -r",
"",
"OPTIONS",
"      [-a]            Run all message sizes from minimum to maximum.",
"      [-app appName]  The application name; default is mamapingpong.",
"      [-h|-?|--help]  Show this help messgae.",
"      [-i interval]   Output latency statistics every interval messages per message size.",
"      [-l length]     Minimum message size in bytes; default is 8.",
"      [-m middleware] The middleware to use [wmw/lbm/tibrv]; default is wmw.",
"      [-n msgCount]   Number of iterations per message size; default is 1000.",
"      [-rdtsc]        Use CPU Time Stamp Counter for latency calculation.",
"      [-S namespace]  Symbol name space for the data",
"      [-sd]           Calculates standard deviation per message size.",
"      [-r]            Mark instance as responder; default is requester.",
"      [-tport name]   The transport to be used from mama.properties.",
"      [-x length]     Maximum message size in bytes; default is 1048576 (1MB).",
"      [-v]            Increase verbosity. Can be passed multiple times.",
NULL
};

/**********************************************************
 *                  FUNCTION PROTOTYPES                   *
 **********************************************************/
static void parseCommandLine
(   int                     argc,
    const char**            argv,
    const char**            middleware,
    const char**            tport,
    const char**            namespace,
    uint8_t*                readTsc,
    uint8_t*                server,
    uint8_t*                allMsgs,
    size_t*                 size,
    size_t*                 maxSize,
    uint32_t*               stopCount
);

static void initializeMama
(
    const char*     middleware,
    const char*     pubName,
    mamaBridge*     bridge,
    mamaQueue*      queue,
    mamaDispatcher* dispatcher,
    mamaTransport*  pubTransport,
    void*           closure
);

static void syncCb
(
    mamaTimer               timer,
    void*                   closure
);

static void syncCompleteCb
(
    mamaTimer               timer,
    void*                   closure
);

static void subscribeToSymbol
(
    const char*       symbol,
    const char*       namespace,
    mamaSubscription* subscription,
    mamaTransport     transport,
    mamaQueue         queue,
    pingPongEventCtx* eventCtx
);

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

static void subscriptionOnMsgPing
(   mamaSubscription        subscription,
    mamaMsg                 msg,
    void*                   closure,
    void*                   itemClosure
);

static void subscriptionOnMsgPong
(
    mamaSubscription        subscription,
    mamaMsg                 msg,
    void*                   closure,
    void*                   itemClosure
);

static void usage
(
    int                     exitStatus
);

static double getClock (void);

static void signalCatcher
(
    int                     sig
);

static void pingpongShutdown (pingPongCtx* ppCtx, pingPongEventCtx* eventCtx);

static void
initMessages(message* pMsgs,
             uint32_t numMsgs,
             uint32_t size,
             uint32_t maxSize);

static void createPublisher
(
    mamaPublisher*      pPublisher,
    mamaTransport       transport,
    const char*         outboundTopic,
    const char*         namespace
);

static void displayResults(pingPongEventCtx* eventCtx);

static void displayMessageStats
(
    message* pMsg
);
/**********************************************************
 *                EOF  FUNCTION PROTOTYPES                *
 **********************************************************/

static    pingPongCtx         gPingPongCtx;

/**********************************************************
 *                        MAIN                            *
 **********************************************************/
int main (int argc, const char** argv)
{
    const char*         transportName       = "sub";
    const char*         middleware          = "wmw";
    const char*         publishSymbol       = "MAMA_PING";
    const char*         subscribeSymbol     = "MAMA_PONG";
    const char*         symbolNamespace     = "MAMA";
    size_t              size                = DEFAULT_MIN_SIZE;
    size_t              maxSize             = DEFAULT_MAX_MSG_SIZE;
    uint8_t             allMsgs             = 0;
    pingPongEventCtx    eventCtx;

    memset (&gPingPongCtx, 0, sizeof gPingPongCtx);
    memset (&eventCtx, 0, sizeof eventCtx);

    /* Unbuffered output to stdout */
    if (setvbuf (stdout, NULL, _IOLBF, 0))
    {
        printf ("Failed to change the buffer of stdout\n");
        return (1);
    }

    /* Get clock requency for use with rdtsc */
    gCpuFreq = getClock ();
    eventCtx.mStopCount = 1000;

    parseCommandLine (argc,
                      argv,
                      &middleware,
                      &transportName,
                      &symbolNamespace,
                      &eventCtx.mRdtsc,
                      &eventCtx.mServer,
                      &allMsgs,
                      &size,
                      &maxSize,
                      &eventCtx.mStopCount);
	if(eventCtx.mServer)
	{
        publishSymbol   = "MAMA_PONG";
        subscribeSymbol = "MAMA_PING";
	}

    initializeMama (middleware,
                    transportName,
                    &gPingPongCtx.mBridge,
                    &gPingPongCtx.mQueue,
                    &gPingPongCtx.mDispatcher,
                    &gPingPongCtx.mTransport,
                    &eventCtx);
                    
	createPublisher (&eventCtx.mPublisher,
                     gPingPongCtx.mTransport,
                     publishSymbol,
                     symbolNamespace);

	if(!eventCtx.mServer)
    {
        if(allMsgs)
        {
            eventCtx.mNumMsgs = (uint32_t)
                ceil (log (maxSize)/log (2)) - ilogb (size) + 1;
        }
        else
            eventCtx.mNumMsgs = 1;

        eventCtx.mMsgs = (message*) calloc (eventCtx.mNumMsgs, sizeof *eventCtx.mMsgs);

        if (NULL == eventCtx.mMsgs)
            exit (EXIT_FAILURE);

        initMessages (eventCtx.mMsgs, eventCtx.mNumMsgs, size, maxSize);
    }

    subscribeToSymbol (subscribeSymbol,
                       symbolNamespace,
                       &gPingPongCtx.mSubscription,
                       gPingPongCtx.mTransport,
                       gPingPongCtx.mQueue,
                       &eventCtx);

    /* Used signal catcher to clean up */
    signal(SIGINT, signalCatcher);

    if(!eventCtx.mServer)
    {
        mamaTimer_create2 (&eventCtx.mTimer,
                           gPingPongCtx.mQueue,
                           syncCb,
                           syncCompleteCb,
                           1.0,
                           &eventCtx);
    }
    else
    {
        printf ("Responder waiting for connections\n");
    }

    /* start the mama bridge, blocks here */
    mama_start (gPingPongCtx.mBridge);

    pingpongShutdown (&gPingPongCtx, &eventCtx);
    mama_close ();

    if(!eventCtx.mServer)
    {
        displayResults(&eventCtx);
        free(eventCtx.mMsgs);
    }

    return (EXIT_SUCCESS);
}

static void
initMessages(message* pMsgs,
             uint32_t numMsgs,
             uint32_t size,
             uint32_t maxSize)
{
    int        i;
	char*      padding;
    char*      fieldPadding;
	size_t     paddingSize;
	size_t     fieldSize;
    mama_fid_t paddingFid;

	paddingSize= size - DEFAULT_MIN_SIZE;
	if (paddingSize < 0)
	{
		printf("ERROR: msgSize is too small - must be a minimum of %lu\n",
				DEFAULT_MIN_SIZE);
		exit(1);
	}

    paddingSize += maxSize - size;
	padding = (char*)malloc(paddingSize);
    if (NULL == padding)
        exit (EXIT_FAILURE);

    for (i = 0; i < numMsgs; i++)
    {
        pMsgs->mMinLatency = INT64_MAX;
        pMsgs->mMaxLatency = INT64_MIN;

	    MAMA_CHECK (mamaMsg_create(&pMsgs->mMamaMsg));
        MAMA_CHECK (mamaMsg_addU64(pMsgs->mMamaMsg, NULL, PING_PONG_FID, 0));

        if(i==0)                        
            pMsgs->mMsgSize = size;
        else if (i == numMsgs-1) 
            pMsgs->mMsgSize = maxSize;
        else                            
            pMsgs->mMsgSize = 1 << (uint64_t)(logb(size)+i);

        /* 
         * Break padding up into seperate fields if it is too big for one
         * field.
         */
        fieldPadding = padding;
        paddingFid = PADDING_FID;
        paddingSize = pMsgs->mMsgSize - DEFAULT_MIN_SIZE;
        memset(padding, 'P', paddingSize);
        while (paddingSize)
        {
            if (paddingSize > MAMA_MSG_MAX_FIELD_SIZE)
                fieldSize = MAMA_MSG_MAX_FIELD_SIZE;
            else
                fieldSize = paddingSize;

            fieldPadding[fieldSize-1] = '\0';

            MAMA_CHECK(mamaMsg_addString (pMsgs->mMamaMsg,
                                          NULL,
                                          paddingFid,
                                          fieldPadding)); 

            paddingFid++;
            paddingSize -= fieldSize;
            fieldPadding += fieldSize;
        }
        pMsgs++;
    }

    free (padding);
}

static inline uint64_t getNanoSecs (uint8_t readTsc)
{
    static struct timespec ts;

    if (readTsc)
    {
        return (uint64_t) ((double)(1000 * rdtsc()) / gCpuFreq);
    }
    else
    {
        clock_gettime(CLOCK_REALTIME, &ts);
        return (uint64_t) (ts.tv_sec * 1000000000 + ts.tv_nsec);
    }
}

static void publishMessage
(
    mamaPublisher*  pPublisher,
    mamaMsg*        pMamaMsg,
    uint8_t         readTsc
)
{
    static uint64_t seqNum = 0;
    uint64_t time = getNanoSecs (readTsc);

    MAMA_CHECK (mamaMsg_updateU64(*pMamaMsg, NULL, SEQ_NUM_FID, seqNum));
    MAMA_CHECK (mamaMsg_updateU64(*pMamaMsg, NULL, PING_PONG_FID, time));
    MAMA_CHECK(mamaPublisher_send (*pPublisher, *pMamaMsg));
    seqNum++;
}

static void pingpongShutdown (pingPongCtx* ppCtx, pingPongEventCtx* eventCtx)
{
    pthread_mutexattr_t    attr;

    if (ppCtx->mSubscription != NULL)
    {
        pthread_mutexattr_init (&attr);
        pthread_mutexattr_settype (&attr, PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init (&gPendingDestroyLock, &attr);
        pthread_cond_init  (&gPendingDestroyCond, NULL);

        gSubscDestroyed = 0;
    	mamaSubscription_destroy (ppCtx->mSubscription);

        pthread_mutex_lock (&gPendingDestroyLock);
        while (!gSubscDestroyed)
        {
            pthread_cond_wait (&gPendingDestroyCond,
                               &gPendingDestroyLock);

        }
        pthread_mutex_unlock (&gPendingDestroyLock);
    	mamaSubscription_deallocate (ppCtx->mSubscription);
        ppCtx->mSubscription = NULL;
    }

    if (ppCtx->mDispatcher != NULL)
    {
        MAMA_CHECK (mamaDispatcher_destroy (ppCtx->mDispatcher));
        ppCtx->mDispatcher = NULL;
    }

    if (eventCtx->mTimer != NULL)
    {
        eventCtx->mShuttingDown = 1;
        MAMA_CHECK (mamaTimer_destroy (eventCtx->mTimer));
        eventCtx->mTimer = NULL;
    }

    if (eventCtx->mPublisher != NULL)
    {
		MAMA_CHECK (mamaPublisher_destroy (eventCtx->mPublisher));
        eventCtx->mPublisher = NULL;
    }

    if (ppCtx->mQueue != NULL)
    {
		MAMA_CHECK (mamaQueue_destroyWait (ppCtx->mQueue));
        ppCtx->mQueue = NULL;
    }

    if (ppCtx->mTransport != NULL)
    {
        MAMA_CHECK (mamaTransport_destroy (ppCtx->mTransport));
        ppCtx->mTransport = NULL;
    }
}

static void
initializeMama (const char*     middleware,
                const char*     pubName,
                mamaBridge*     bridge,
                mamaQueue*      queue,
                mamaDispatcher* dispatcher,
                mamaTransport*  pubTransport,
                void*           closure)
{
    char queueNameBuff[24] = "Queue PingPong";

    if (gAppName)
    {
        MAMA_CHECK (mama_setApplicationName (gAppName));
    }

    MAMA_CHECK (mama_loadBridge (bridge, middleware));
    MAMA_CHECK (mama_open ());

	/*Allocate the transports*/
    MAMA_CHECK (mamaTransport_allocate (pubTransport));
    MAMA_CHECK (mamaTransport_create (*pubTransport,
                                      pubName,
                                      *bridge));

    MAMA_CHECK (mamaQueue_create (queue, *bridge));
    MAMA_CHECK (mamaQueue_setQueueName (*queue, queueNameBuff));
    MAMA_CHECK (mamaDispatcher_create (dispatcher , *queue));
}

static void subscriptionOnDestroy
(
   mamaSubscription             subsc,
   void*                        closure
)
{
    pthread_mutex_lock (&gPendingDestroyLock);
    gSubscDestroyed = 1;
    pthread_cond_signal (&gPendingDestroyCond);
    pthread_mutex_unlock (&gPendingDestroyLock);
}

static void
subscribeToSymbol (const char*       symbol,
                   const char*       namespace,
                   mamaSubscription* subscription,
                   mamaTransport     transport,
                   mamaQueue         queue,
                   pingPongEventCtx* eventCtx)
                           
{
    mamaMsgCallbacks callbacks;
    char             topic[1024];

    memset (&callbacks, 0, sizeof callbacks);

    callbacks.onCreate          = subscriptionOnCreate;
    callbacks.onError           = subscriptionOnError;

	if(eventCtx->mServer)
        callbacks.onMsg = subscriptionOnMsgPong;
	else
        callbacks.onMsg = subscriptionOnMsgPing;

    callbacks.onQuality         = subscriptionOnQuality;
    callbacks.onGap             = subscriptionOnGap;
    callbacks.onDestroy         = subscriptionOnDestroy;

    snprintf(topic,sizeof(topic),"%s.%s",namespace,symbol);

    MAMA_CHECK (mamaSubscription_allocate (subscription));
    MAMA_CHECK (mamaSubscription_setTimeout (*subscription, 10.0));
	MAMA_CHECK (mamaSubscription_createBasic (*subscription,
                                              transport,
                                              queue,
                                              &callbacks,
                                              topic,
                                              eventCtx));
}

static void createPublisher
(
    mamaPublisher*      pPublisher,
    mamaTransport       transport,
    const char*         topic,
    const char*         namespace
)
{
	MAMA_CHECK (mamaPublisher_create (pPublisher,
    	                              transport,
                                      topic,
                                      namespace,
                                      NULL));
}

static void subscriptionOnCreate
(
    mamaSubscription            subscription,
    void*                       closure
)
{}

static void subscriptionOnGap
(   mamaSubscription   subscription,
    void*              closure
)
{
	printf("Received a GAP\n");
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
    printf ("Quality changed to %s for %s\n",
            mamaQuality_convertToString (quality),
            symbol);
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
    printf ("subscriptionOnError for %s - %s (%d): %s\n",
            subject,
            mamaStatus_stringForStatus (status),
            status,
            mama_getLastErrorText());
}

static void printHeader(void)
{
	printf("%10s %11s %10s %10s %10s",
            "#bytes",
            "#iterations",
            "min[usec]",
            "mean[usec]",
            "max[usec]");

    if (gSD)
        printf(" %10s",
                "sd[usec]");

    printf("\n");
}

static void displayResults(pingPongEventCtx* eventCtx)
{
    int i = 0;
    message* pMsg = eventCtx->mMsgs;
	printHeader();

    for (i = 0; i < eventCtx->mNumMsgs; i++)
    {
        displayMessageStats(pMsg);
        pMsg++;
    }
}

static void displayMessageStats
(
    message*    pMsg
)
{
    double var; /*nsec*/
    double avg; /*nsec*/

    printf ("%10lu %11" PRIu64,
            pMsg->mMsgSize,
            pMsg->mNumMsgs);

    /*Deal with latencies*/
    if(!pMsg->mNumMsgs)
    {
        printf (" %10.2f %10.2f %10.2f", 0.0, 0.0, 0.0);
        if (gSD) printf (" %10.2f", 0.0);
    }
    else
    {
        avg = (double)pMsg->mRunningLatency/(double)pMsg->mNumMsgs;
        printf (" %10.2f %10.2f %10.2f",
                (double)pMsg->mMinLatency/1000.0,
                avg/1000.0,
                (double)pMsg->mMaxLatency/1000.0);

        if (gSD)
        {
            var = (pMsg->mRunningSqLatency/
                (double)(pMsg->mNumMsgs)) - avg*avg;
            printf (" %10.2f",
                    (double)sqrt(var)/1000.0);
        }
    }
    printf ("\n");
}

static
void syncCompleteCb
(
    mamaTimer               timer,
    void*                   closure
)
{
    pingPongEventCtx* eventCtx = (pingPongEventCtx*) closure;

    if (!eventCtx->mShuttingDown)
        publishMessage (&eventCtx->mPublisher, &eventCtx->mMsgs->mMamaMsg, eventCtx->mRdtsc);
}

static
void syncCb
(
    mamaTimer               timer,
    void*                   closure
)
{
    pingPongEventCtx* eventCtx = (pingPongEventCtx*) closure;

    /* 
     * Start in sync init, send messages on timer. When response is received, wait until
     * other outstanding responses are received then start.
     */
    switch (eventCtx->mSyncState)
    {
    case SYNC_INIT:
        printf ("Sending sync message\n");
        eventCtx->mSyncState = SYNC_SENT;
    case SYNC_SENT:
        publishMessage (&eventCtx->mPublisher, &eventCtx->mMsgs->mMamaMsg, eventCtx->mRdtsc);
        mamaMsg_getU64 (eventCtx->mMsgs->mMamaMsg, 
                        NULL,
                        SEQ_NUM_FID,
                        &eventCtx->mSyncSeqNum);
        break;
    case SYNC_REACHED:
        printf ("Sync complete\n");
        eventCtx->mSyncState = SYNC_COMPLETE;
        if (gIntervalRate)
            printHeader ();
        mamaTimer_destroy (eventCtx->mTimer);
        eventCtx->mTimer = NULL;
        break;
        break;
    default:
        break;
    }
}

static void subscriptionOnMsgPong
(
    mamaSubscription            subscription,
    mamaMsg                     msg,
    void*                       closure,
    void*                       itemClosure
)
{
    pingPongEventCtx*   eventCtx = (pingPongEventCtx*) closure;

    /* Just send the message back. */
    mamaPublisher_send (eventCtx->mPublisher, msg);
}

static void subscriptionOnMsgPing
(
    mamaSubscription            subscription,
    mamaMsg                     msg,
    void*                       closure,
    void*                       itemClosure
)
{
    static int64_t msgIndex = 0;
    static uint64_t seqNum = 0;
	static int64_t pingPongLatency =0;
	static uint64_t nowTime, pingTime;
    const char* padding = NULL;
    size_t paddingSize, fieldSize;
    mama_fid_t paddingFid;
    mama_status status;

    pingPongEventCtx*   eventCtx = (pingPongEventCtx*) closure;
    message*            ppMsg = &eventCtx->mMsgs[msgIndex];

    /* Timestamp as soon as possible. */
    nowTime = getNanoSecs (eventCtx->mRdtsc);

    if (SYNC_COMPLETE != eventCtx->mSyncState)
    {
        /* Publish cache can cause message before sync is sent. */
        if (SYNC_INIT == eventCtx->mSyncState)
        {
            return;
        }
        eventCtx->mSyncState = SYNC_RESPONDED;

        printf ("Sync responded\n");
        mamaMsg_getU64 (msg, NULL, SEQ_NUM_FID, &seqNum);
        if (seqNum == eventCtx->mSyncSeqNum)
        {
            eventCtx->mSyncState = SYNC_REACHED;
            printf ("Sync reached\n");
        }
        return;
    }

    /* Check that this is the expected message. */
    mamaMsg_getU64 (ppMsg->mMamaMsg, 
                    NULL,
                    SEQ_NUM_FID,
                    &eventCtx->mSyncSeqNum);
    mamaMsg_getU64 (msg, NULL, SEQ_NUM_FID, &seqNum);
    if (seqNum != eventCtx->mSyncSeqNum)
    {
        printf ("Unexpected sequence number %" PRIu64 ", expected %" PRIu64
                "\n",
                seqNum,
                eventCtx->mSyncSeqNum);
        return;
    }

    /* Check that the message size is as expected. */
    paddingFid = PADDING_FID;
    paddingSize = 0;
    fieldSize = 0;
    do
    {
        padding = NULL;
    	status = mamaMsg_getString (msg,
                                    NULL,
                                    paddingFid,
                                    &padding);

        if(NULL != padding)
            paddingSize += strlen(padding) + 1;

        paddingFid++;

    } while (status == MAMA_STATUS_OK);

    if (paddingSize + DEFAULT_MIN_SIZE != ppMsg->mMsgSize)
    {
        printf ("ERROR: Expected msg size %luB, received %luB (%luB/%luB)\n",
                ppMsg->mMsgSize,
                paddingSize + DEFAULT_MIN_SIZE,
                DEFAULT_MIN_SIZE,
                paddingSize);
        signalCatcher(SIGUSR1);
        return;
    }

    mamaMsg_getU64 (msg, NULL, PING_PONG_FID, &pingTime);
    
    pingPongLatency = (int64_t) (nowTime - pingTime);
    ppMsg->mRunningLatency += pingPongLatency;

    if(gSD)
    {
        ppMsg->mRunningSqLatency += pingPongLatency*pingPongLatency;
    }

    if (pingPongLatency < ppMsg->mMinLatency)  
        ppMsg->mMinLatency = pingPongLatency;
    if (pingPongLatency > ppMsg->mMaxLatency)  
        ppMsg->mMaxLatency = pingPongLatency;

    ppMsg->mNumMsgs++;

    if(gIntervalRate)
    {
        if((ppMsg->mNumMsgs+1)%gIntervalRate==0)
            displayMessageStats(ppMsg);
    }

    if(ppMsg->mNumMsgs >= eventCtx->mStopCount)
    {
        msgIndex++;
        ppMsg++;
    }

    if(msgIndex < eventCtx->mNumMsgs)
    {
        publishMessage (&eventCtx->mPublisher, &ppMsg->mMamaMsg, eventCtx->mRdtsc);
    }
    else
    {
        printf ("Complete\n");
        signalCatcher(SIGUSR1);
    }
}


static void parseCommandLine
(   int                     argc,
    const char**            argv,
    const char**            middleware,
    const char**            tport,
    const char**            namespace,
    uint8_t*                readTsc,
    uint8_t*                server,
    uint8_t*                allMsgs,
    size_t*                 size,
    size_t*                 maxSize,
    uint32_t*               stopCount
)
{
    int i = 1;

    for (i = 1; i < argc;)
    {
        if (strcmp (argv[i], "-rdtsc") == 0)
        {
            *readTsc = 1;
            i ++;
        }
        else if ((strcmp(argv[i], "-h") == 0) ||
                (strcmp(argv[i], "--help") == 0) ||
                (strcmp(argv[i], "-?") == 0))
        {
            usage (EXIT_SUCCESS);
        }
        else if(strcmp("-a", argv[i]) == 0)
        {
            *allMsgs = 1;
            i++;
        }
        else if(strcmp("-r", argv[i]) == 0)
        {
            *server = 1;
            i ++;
        }
        else if (strcmp (argv[i], "-sd") == 0)
        {
            gSD = 1;
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
        else
        {
            /*
             * Command line options with trailing values
             */
            if (i == argc-1)
            {
                fprintf(stderr,
                        "ERROR: Last Option %s is not a valid commandline flag. Ignoring\n",argv[i]);
                i++;
            }
            else if ( strcmp( argv[i], "-app" ) == 0 )
            {
                gAppName = argv[i+1];
                i += 2;
            }
            else if (strcmp (argv[i], "-l") == 0)
            {
                *size = atoi (argv[i + 1]);
                i += 2;
            }
            else if (strcmp (argv[i], "-i") == 0)
            {
                gIntervalRate = atoi (argv[i + 1]);
                i += 2;
            }
            else if (strcmp (argv[i], "-x") == 0)
            {
                *maxSize = atoi (argv[i + 1]);
                *allMsgs = 1;
                i += 2;
            }
            else if (strcmp (argv[i], "-n") == 0)
            {
                *stopCount = atoi (argv[i + 1]);
                i += 2;
            }
            else if(strcmp("-tport", argv[i]) == 0)
            {
                *tport = argv[i+1];
                i += 2;
            }
            else if (strcmp ("-m", argv[i]) == 0)
            {
                *middleware = (argv[i+1]);
                i += 2;                  }
            else if (strcmp(argv[i], "-S") == 0)
            {
                *namespace = argv[i+1];
                i += 2;
            }
            else
            {
                fprintf(stderr,"ERROR: Commandline option %s not recognised. Skipping\n",argv[i]);
                i++;
            }
        }
    }
}

static void usage
(
    int                     exitStatus
)
{
    int i = 0;
    while (NULL != gUsageString[i])
    {
        printf ("%s\n", gUsageString[i++]);
    }
    exit (exitStatus);
}

static void stopAppCallback
(
    mamaQueue queue,
    void* closure
)
{
    static int appStopped = 0;

    if (!appStopped)
    {
        printf ("Shutting down.\n");
        mama_stop (gPingPongCtx.mBridge);
    }
}

static void signalCatcher
(
    int                     sig
)
{
    static int appStopped = 0;

    if(!appStopped)
    {
        appStopped = 1;
        mamaQueue_enqueueEvent (gPingPongCtx.mQueue, stopAppCallback, NULL);
    }
}

static double getClock(void)
{
    FILE *cpuInfo;
    char buff[16384];
    char freq[128];
    char searchStr[128];
    char* tmp;

    if((cpuInfo = fopen ("/proc/cpuinfo", "rb"))!=NULL)
    {
        while (!feof (cpuInfo))
        {
            fread (buff, sizeof (buff), 1, cpuInfo);
        }
        strcpy (searchStr, "cpu MHz\t\t: ");
        tmp = strstr (buff,searchStr);
        tmp += strlen (searchStr);
        strncpy (freq, tmp, 8);
        return atof (freq);
    }
    else
    {
        perror("ERROR: Could not get CPU Frequency-");
        exit(1);
    }
}
