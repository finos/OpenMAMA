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

/*----------------------------------------------------------------------------
 *
 * This application demonstrates how to measure the performence by a pingpong
 * test.
 *
 * mamatest_requestercpp  accepts the following command line arguments:
 *      [-n name]          The client name
 *      [-pp number]       The number of how many pingpongs to be sent.
 *                         Default 100000.
 *      [-stats number]    Intervals for stats. Default 100
 *      [-tport name]      The transport parameters to be used from
 *                         mama.properties.
 *      [-m middleware]    The middleware to use [wmw/lbm/tibrv]
 *                         Default is wmw.
 *
 * mamatest_replycpp  accepts the following command line arguments:
 *      [-tport name]      The transport parameters to be used from
 *                         mama.properties.
 *      [-m middleware]    The middleware to use [wmw/lbm/tibrv]
 *                         Default is wmw.
 *
 *---------------------------------------------------------------------------*/

#include <iostream>

#include <sys/time.h>
#include <sys/times.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#include "wombat/wincompat.h"
#include "wombat/wMessageStats.h"

#include "mama/mama.h"
#include "mama/mamacpp.h"
#include "mama/MamaBasicSubscription.h"

using namespace Wombat;

using std::cerr;
using std::cout;
using std::endl;

static MamaTransport *        gTransport     = NULL;
static MamaTransport *        gTransportPub  = NULL;
static MamaTransport *        gTransportSub  = NULL;
static MamaTimer *            gTimer         = NULL;
static MamaPublisher *        gPublisher     = NULL;
static const char *           gOutBoundTopic = "MAMA_REQUEST";

static MamaQueue*             gDefaultQueue  = NULL;
static mamaBridge             gBridge        = NULL;
static const char *           gMiddleware    = "wmw";

// Listens on 5 inbound topics
static MamaBasicSubscription*       gSubscriptionCalc       = NULL;
static MamaBasicSubscription*       gSubscriptionRisk       = NULL;
static MamaBasicSubscription*       gSubscriptionPosition   = NULL;
static MamaBasicSubscription*       gSubscriptionSignal     = NULL;
static MamaBasicSubscription*       gSubscriptionHistorical = NULL;
static const char *                 gInBoundTopicCalc       = "MAMA_REPLY_CALC";
static const char *                 gInBoundTopicRisk       = "MAMA_REPLY_RISK";
static const char *                 gInBoundTopicPosition   = "MAMA_REPLY_POSITION";
static const char *                 gInBoundTopicSignal     = "MAMA_REPLY_SIGNAL";
static const char *                 gInBoundTopicHistorical = "MAMA_REPLY_HISTORICAL";
static const char *                 gMyName                 = "pingpong";
static const char *                 gTransportName          = "ping";
static const char *                 gTransportNamePub       = "pingpub";
static const char *                 gTransportNameSub       = "pingsub";
static int                          gUseSeparateTransports  = 0;
static int                          gSleep                  = 0;
static double                       gInterval               = 2;
static int                          gQuietLevel             = 3;
static int                          gPingPong               = 1;
static int                          gPingNumber             = 100000;
static int                          gBlast                  = 0;
static int                          gBlastNumber            = 100000;
static int                          gBurst                  = 0;
static int                          gBurstNumber            = 1000;
static int                          gTotalBursts            = 100;
static int                          gNumBursts              = 1;
static int                          gNumPublished           = 0;
static int                          gMsgInterval            = 1000;
static int                          gSampled                = 0;
static double                       gTotalLatency           = 0.0;
static int                          gNumRecieved            = 0;
static int                          gBurstCompleted         = 0;
static int                          gBurstFirstSec          = 0;
static int                          gBurstFirstMicroSec     = 0;
static int                          gBurstLastSec           = 0;
static int                          gBurstLastMicroSec      = 0;
static int                          gBurstFirstRcvSec       = 0;
static int                          gBurstFirstRcvMicroSec  = 0;
static int                          gT1Sec                  = 0;
static int                          gT1MicroSec             = 0;
static int                          gT2Sec                  = 0;
static int                          gT2MicroSec             = 0;
static cpuVals                      gCpuV1;
static cpuVals                      gCpuV2;
static long                         gMaxMem                 = 0;
static const char *                 gUsageString[]          =
{
    " This application demonstrates how to measure the performence by a pingpong",
    " test.",
    "",
    " mamatest_requestercpp  accepts the following command line arguments:",
    "      [-n name]          The client name",
    "      [-pp number]       The number of how many pingpongs to be sent.",
    "                         Default 100000.",
    "      [-stats number]    Intervals for stats. Default 100",
    "      [-tport name]      The transport parameters to be used from",
    "                         mama.properties. Default is ping",
    "      [-m middleware]    The middleware to use [wmw/lbm/tibrv].",
    "                         Default is wmw.", 
    "",
    " mamatest_replycpp  accepts the following command line arguments:",
    "      [-tport name]      The transport parameters to be used from",
    "                         mama.properties.",
    NULL
};

static void parseCommandLine          (int argc, const char **argv);
static void initializeMama            (void);
static void createIntervalTimer       (void);
static void createInboundSubscription (void);

static void shutdown                  (void);
static void ctrl_c_shutdown           (void);

static void publishMessage            (const MamaMsg* request ,int tag);
static void createPublisher           (void);
void        usage                     (int exitStatus);


double calLatency2Points              (int startSec,
                                       int startMicroSec,
                                       int endSec,
                                       int endMicroSec);

static void signalCleanup             (int sig);

// Used to ensure correct data types are used when passing data to the database
// from DBAC
enum DBAC_TYPES
{
    DBAC_BOOL,
    DBAC_CHAR,
    DBAC_STRING,
    DBAC_INT_32,
    DBAC_INT_64,
    DBAC_FLOAT_64,
    DBAC_DATE,
    DBAC_TIME,
    DBAC_TIMESTAMP
};

// Message types - used to let the system know which of the calculation servers
// processes the message - MAMA_REPLY_CALC is routed to the calc server, etc.
enum MAMA_MSG_TYPES
{
    MAMA_REPLY_CALC,
    MAMA_REPLY_RISK,
    MAMA_REPLY_POSITION,
    MAMA_REPLY_SIGNAL,
    MAMA_REPLY_HISTORICAL,
    MAMA_REQUEST
};

enum TRANSMISSION_TYPES
{
    TRAN_BURST_NORMAL,
    TRAN_BURST_FIRST,
    TRAN_BURST_LAST,
    TRAN_PINGPONG,
    TRAN_BLAST
};

int main (int argc, const char **argv)
{
    signal (SIGTERM, signalCleanup);
    signal (SIGINT,  signalCleanup);
    
    startCpuTimer             ();
    parseCommandLine          (argc, argv);
    initializeMama            ();
    createInboundSubscription ();
    createPublisher           ();
    sleep                     (2);
    createIntervalTimer       ();
    Mama::start               (gBridge);
}

void shutdown (void)
{
    delete gSubscriptionCalc;
    delete gSubscriptionRisk;
    delete gSubscriptionPosition;
    delete gSubscriptionSignal;
    delete gSubscriptionHistorical;
    delete gTransport;
    delete gTransportPub;
    delete gTransportSub;
    Mama::close ();
    cpuVals cpuV;
    getCpuTimeVals (&cpuV,1);
    printf ("Average Latency(ms):%f, Total User Time:%f, Max Mem:%ld\n",
            gTotalLatency/gSampled, cpuV.userTime, gMaxMem);
    
    exit (0);
}

void ctrl_c_shutdown (void)
{
  delete gSubscriptionCalc;
  delete gSubscriptionRisk;
  delete gSubscriptionPosition;
  delete gSubscriptionSignal;
  delete gSubscriptionHistorical;
  delete gTransport;
  delete gTransportPub;
  delete gTransportSub;
  Mama::close ();
  printf ("\nProgram Stopped by user. Stats Interval = %d\n\n", gMsgInterval);
    
  exit (0);
}


void initializeMama (void)
{
    try
    {
        gBridge = Mama::loadBridge (gMiddleware);
        
        Mama::open ();

        gDefaultQueue = Mama::getDefaultEventQueue (gBridge);
        
        if (gUseSeparateTransports)
        {
            gTransportPub = new MamaTransport;
            gTransportPub->create (gTransportNamePub, gBridge);
            sleep (gSleep);
            gTransportSub = new MamaTransport;
            gTransportSub->create (gTransportNameSub, gBridge);
        }
        else
        {
            gTransport = new MamaTransport;
            gTransport->create (gTransportName, gBridge);
        }
    }
    catch (MamaStatus &status)
    {
        cerr << "Error initializing mama or creating transport: "
             << status.toString () << endl;
        exit (1);
    }
}

static void createPublisher (void)
{
    try
    {
        MamaTransport* transport = gUseSeparateTransports ?
                                   gTransportPub : gTransport;

        gPublisher = new MamaPublisher;
        gPublisher->create (transport,
                            gOutBoundTopic);
    }
    catch (MamaStatus &status)
    {
        cerr << "Error creating publisher: "
             << status.toString () << endl;
        exit (1);
    }
}

static void publishMessage (const MamaMsg *request, int tag)
{
    static int msgNumber = 0;
    static int count     = 0;       // Round robin for testing

    MamaMsg *msg;

    try
    {
        msg = new MamaMsg;
        msg->create ();

        msg->addI32    ("wMsgType",           1,  MAMA_MSG_TYPE_WOMBAT_REQUEST);
        msg->addI32    ("wCalcServer",        2,  count);
        msg->addI32    ("wMsgStatus",         3,  MAMA_MSG_STATUS_OK);
        msg->addString ("wRequesterId",       4,  gMyName);
        msg->addI32    ("wRequestSeqNum",     5,  msgNumber++);
        msg->addString ("wActivityTime",      6,  "12:00:00");
        msg->addString ("wRequestFunction",   7,  "getTestResult");
        msg->addI32    ("wRequestNumArgs",    8,  2);
        msg->addI32    ("wRequestParamType1", 9,  DBAC_STRING);
        msg->addString ("wRequestParamVal1",  10, "5");
        msg->addI32    ("wRequestParamType2", 11, DBAC_INT_32);
        msg->addI32    ("wRequestParamVal2",  12, msgNumber);

        if((tag==TRAN_BURST_FIRST) || (tag==TRAN_BURST_LAST))
        {
            struct timeval tv;
            (void)gettimeofday (&tv, NULL);
            msg->addI32 ("wMsgTag",       13, tag);
            msg->addI32 ("wTimeSec",      14, tv.tv_sec);
            msg->addI32 ("wTimeMicroSec", 15, tv.tv_usec);
        }
        else
        {
            msg->addI32 ("wMsgTag",       13, tag);
            msg->addI32 ("wTimeSec",      14, 0);
            msg->addI32 ("wTimeMicroSec", 15, 0);
        }

        if (request)
        {
            if (gQuietLevel < 1)
            {
                printf ("Publishing message to inbox.\n");
            }
            gPublisher->sendReplyToInbox (*request, msg);
        }
        else
        {
            if (gQuietLevel < 1)
            {
                printf ("Publishing message to %s.\n", gOutBoundTopic);
            }
            gPublisher->send (msg);
        }

        // Calc server round robin for testing
        if (count ==4)
        {
            count = 0;
        }
        else
        {
            count+=1;
        }

        gNumPublished ++;

        delete msg;
    }
    catch (MamaStatus &status)
    {
        cerr << "Error publishing message: "
             << status.toString () << endl;
        exit (1);
    }
}

class SubscriptionCallback : public MamaBasicSubscriptionCallback
{
public:
    virtual ~SubscriptionCallback () {}

    virtual void onCreate (MamaBasicSubscription*  subscription)
    {
        if (gQuietLevel < 2)
        {
            printf ("Created inbound subscription.\n");
        }
    }

    virtual void onError (MamaBasicSubscription*  subscription,
                          const MamaStatus&       status,
                          const char*             subject)
    {
        printf ("Error creating subscription: %s\n",
                status.toString ());
        exit (1);
    }

    virtual void onMsg (MamaBasicSubscription* subscription,
                        MamaMsg&               msg)
    {
        gNumRecieved++;

        int timeSec      = 0;
        int timeMicroSec = 0;
        int msgTag       = 0;

        struct timeval tv;

        if (gQuietLevel < 2)
        {
            printf ("Recieved inbound msg. Sending response\n");
        }

        msg.tryI32 ("wMsgTag", 8, msgTag);
        if (gBlast)       // Blast test
        {

        }
        else if (gBurst)  // Burst test
        {
            switch (msgTag)
            {
                case TRAN_BURST_NORMAL:
                {
                    break;
                }
                case TRAN_BURST_FIRST:
                {
                    gettimeofday (&tv,NULL);

                    timeSec      = tv.tv_sec;
                    timeMicroSec = tv.tv_usec;

                    msg.tryI32 ("wTimeSec",      9,  gBurstFirstSec);
                    msg.tryI32 ("wTimeMicroSec", 10, gBurstFirstMicroSec);

                    gBurstFirstRcvSec      = timeSec;
                    gBurstFirstRcvMicroSec = timeMicroSec;

                    printf ("first arrived\n");
                    break;
                }
                case TRAN_BURST_LAST:
                {
                    gettimeofday (&tv, NULL);

                    timeSec      = tv.tv_sec;
                    timeMicroSec = tv.tv_usec;

                    printf ("last arrived\n");

                    msg.tryI32 ("wTimeSec",      9,  gBurstLastSec);
                    msg.tryI32 ("wTimeMicroSec", 10, gBurstLastMicroSec);

                    double allTime      = 0,
                           lastLatency  = 0,
                           firstLatency = 0,
                           burstLength  = 0;
                    
                    allTime = calLatency2Points    (gBurstFirstSec,
                                                    gBurstFirstMicroSec,
                                                    timeSec,
                                                    timeMicroSec);

                    lastLatency=calLatency2Points  (gBurstLastSec, 
                                                    gBurstLastMicroSec,
                                                    timeSec,
                                                    timeMicroSec);

                    firstLatency=calLatency2Points (gBurstFirstSec, 
                                                    gBurstFirstMicroSec,
                                                    gBurstFirstRcvSec,
                                                    gBurstFirstRcvMicroSec);

                    burstLength=calLatency2Points  (gBurstFirstSec,
                                                    gBurstFirstMicroSec,
                                                    gBurstLastSec,
                                                    gBurstLastMicroSec);

                    printf ("allTime(ms):%f firstlatency(ms):%f lastLatency(ms):%f burstLength(ms):%f\n",
                             allTime,
                             firstLatency,
                             lastLatency,
                             burstLength);

                    struct memVals memV;
                    struct cpuVals cpuV;

                    getCpuTimeVals (&cpuV, 0);
                    getMemVals(getpid (), &memV);

                    printf ("user time:%f, sys time:%f, real time:%f, percent cpu:%f ,mem:%ldK\n",
                             cpuV.userTime,
                             cpuV.sysTime,
                             cpuV.realTime,
                             cpuV.pCpu,
                             memV.rss);
                    break;
                }
                default:
                {
                    printf ("unexpected\n");
                    break;
                }
            }
        }
        else if (gPingPong)
        {
            if (gNumRecieved % gMsgInterval == 0)
            {
               
                if (gNumRecieved>1)
                {
                    gSampled++;
                    gettimeofday (&tv, NULL);

                    gT2Sec      = tv.tv_sec;
                    gT2MicroSec = tv.tv_usec;

                    double latency = 0;
                    latency = calLatency2Points (gT1Sec,
                                                 gT1MicroSec,
                                                 gT2Sec,
                                                 gT2MicroSec)/gMsgInterval;
                    gT1Sec      = gT2Sec;
                    gT1MicroSec = gT2MicroSec;

                    gTotalLatency += latency;

                    struct memVals memV;
                    struct cpuVals cpuV;

                    getCpuTimeVals (&gCpuV2, 0);
                    getCpuTimeValDiff (gCpuV1, gCpuV2, &cpuV);

                    getMemVals(getpid (), &memV);

                    printf ("%10d,%10f,%10f,%10f,%10f,%10ld\n",
                            gNumRecieved,
                            latency,
                            gCpuV2.userTime,
                            cpuV.userTime,
                            cpuV.pCpu,
                            memV.rss);

                    gCpuV1.userTime = gCpuV2.userTime;

                    if (memV.rss > gMaxMem)
                    {
                        gMaxMem = memV.rss;
                    }
                }
            }
        };

        // Identify which server the reply is from
        int calc_server;
        msg.tryI32 ("wCalcServer", 2, calc_server);

        // Identify which client the reply was intended for
        int requestSeqNum;
        msg.tryI32 ("wCalcServer", 5, requestSeqNum);

        // Find the name of the machine sending the request
        const char* id = NULL;
        msg.tryString ("wRequesterId", 4, id);
        char* requesterId = const_cast<char*> (id);

        if (gQuietLevel < 2)
        {
            printf ("\ncalc server %i\n",   calc_server);
            printf ("request seq num %i\n", requestSeqNum);
            printf ("requester ID %s\n",    requesterId);
        }

        // Only proceed if this response is for me
        // in some cases we'll just want to listen
        if(strcmp (requesterId, gMyName) == 0)
        {
            // Retrieve the request function
            const char* func = NULL;
            msg.tryString ("wRequestFunction", 7, func);
            char* requestFunction = const_cast<char*> (func);

            if (gQuietLevel < 2)
            {
                printf ("requestFunction  %s\n", requestFunction);
            }

            switch (calc_server)
            {
                case MAMA_REPLY_CALC:
                {
                    if (gQuietLevel < 2)
                    {
                        printf ("Processing calc reply \n");
                    }
                    break;
                }
                case MAMA_REPLY_RISK:
                {
                    if (gQuietLevel < 2)
                    {
                        printf ("Processing risk reply \n");
                    }
                    break;
                }
                case MAMA_REPLY_POSITION:
                {
                    if (gQuietLevel < 2)
                    {
                        printf ("Processsing position reply \n");
                    }
                    break;
                }
                case MAMA_REPLY_SIGNAL:
                {
                    if (gQuietLevel < 2)
                    {
                        printf ("Processing signal reply \n");
                    }
                    break;
                }
                case MAMA_REPLY_HISTORICAL:
                {
                    if (gQuietLevel < 2)
                    {
                        printf ("Recieved historical reply \n");
                    }
                    break;
                }
                case MAMA_REQUEST:
                {
                    break;
                }
                default:
                {
                }

            }
            if (gPingPong == 1 && gNumPublished < gPingNumber)
            {
                publishMessage (NULL,  TRAN_PINGPONG);
            }
        }
        if (gNumRecieved == gNumPublished)
        {
            shutdown ();
        }
    }
};

static void createInboundSubscription (void)
{
    try
    {
        MamaTransport* transport = gUseSeparateTransports ?
                                   gTransportSub : gTransport;

        gSubscriptionCalc = new MamaBasicSubscription;
        gSubscriptionCalc->createBasic (
                            transport,
                            gDefaultQueue,
                            new SubscriptionCallback (),
                            gInBoundTopicCalc);

        gSubscriptionRisk = new MamaBasicSubscription;
        gSubscriptionRisk->createBasic (
                            transport,
                            gDefaultQueue,
                            new SubscriptionCallback (),
                            gInBoundTopicRisk);

        gSubscriptionPosition = new MamaBasicSubscription;
        gSubscriptionPosition->createBasic (
                            transport,
                            gDefaultQueue,
                            new SubscriptionCallback (),
                            gInBoundTopicPosition);

        gSubscriptionSignal = new MamaBasicSubscription;
        gSubscriptionSignal->createBasic (
                            transport,
                            gDefaultQueue,
                            new SubscriptionCallback (),
                            gInBoundTopicSignal);

        gSubscriptionHistorical = new MamaBasicSubscription;
        gSubscriptionHistorical->createBasic (
                            transport,
                            gDefaultQueue,
                            new SubscriptionCallback (),
                            gInBoundTopicHistorical);

    }
    catch (MamaStatus &status)
    {
        cerr << "Error creating subscription: "
             << status.toString () << endl;
        exit (1);
    }

}

void usage  (int exitStatus)
{
    int i = 0;
    while (gUsageString[i] != NULL)
    {
        printf ("%s\n", gUsageString[i++]);
    }
    exit (exitStatus);
}


class TimerCallback : public MamaTimerCallback
{
public:
    virtual ~TimerCallback (void) {}

    virtual void onTimer (MamaTimer *timer)
    {
        if (gBlast)  // Blast test
        {
            double start = clock ();
            printf ("starting blast :: %i\n", gBlastNumber);

            for (int i = 0; i < gBlastNumber; i++)
            {
                publishMessage (NULL, -1);
            }
            gBlast = 0;

            printf ("completed blast\n");

            double end = clock ();
            double rtime = (end-start) / 1000000;

            printf ("blast time :: %f\n", rtime);
        }
        else if (gBurst && (!gBurstCompleted))  // Burst test
        {
            printf          ("starting burst :: %i\n", gNumBursts);
            publishMessage  (NULL, TRAN_BURST_FIRST);
            printf          ("first sent\n");

            for (int i = 1; i < (gBurstNumber-1); i++)
            {
                publishMessage (NULL, TRAN_BURST_NORMAL);
            }
            publishMessage (NULL, TRAN_BURST_LAST);

            printf ("last sent\n");

            if (gNumBursts == gTotalBursts)
            {
                gBurstCompleted = 1;
                printf ("completed burst test\n");
            }
            gNumBursts++;
        }
        else if (gPingPong)
        {
            if (gNumPublished == 0)
            {
                printf ("#Pingpong, Two-way latency(ms), Total user time, Incremental user time, Percentage CPU, Mem\n");
                struct timeval tv;
                startCpuTimer ();

                getCpuTimeVals (&gCpuV1, 0);
                gettimeofday   (&tv, NULL);

                gT1Sec      = tv.tv_sec;
                gT1MicroSec = tv.tv_usec;

                publishMessage (NULL,  TRAN_PINGPONG);
            }
        }
    }
};

static void createIntervalTimer (void)
{
    try
    {
        gTimer = new MamaTimer;
        gTimer->create (gDefaultQueue, new TimerCallback (), gInterval);
    }
    catch (MamaStatus &status)
    {
        cerr << "Error creating subscription: "
             << status.toString () << endl;
        exit (1);
    }
}


void parseCommandLine (int argc, const char **argv)
{
    int i = 0;
    bool check = true;
    for (i = 1; i < argc;)
    {
        if (strcmp ("-s", argv[i]) == 0)
        {
            gOutBoundTopic = argv[i+1];
            i += 2;
        }
        else if (strcmp ("-l", argv[i]) == 0)
        {
            //gInBoundTopic = argv[i+1];
            i += 2;
        }
        else if (strcmp ("-i", argv[i]) == 0)
        {
            gInterval = strtod (argv[i+1], NULL);
            i += 2;
        }
        else if (strcmp ("-tport", argv[i]) == 0)
        {
            gTransportName = argv[i+1];
            i += 2;
        }
        else if (strcmp ("-tportpub", argv[i]) == 0)
        {
            gTransportNamePub = argv[i+1];
            i += 2;
        }
        else if (strcmp ("-tportsub", argv[i]) == 0)
        {
            gTransportNameSub = argv[i+1];
            i += 2;
        }
        else if (strcmp ("-m", argv[i]) == 0)
        {
            gMiddleware = argv[i+1];
            i += 2;
        }
        else if (strcmp ("-v", argv[i]) == 0)
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
        else if (strcmp ("-st", argv[i]) == 0)
        {
            gUseSeparateTransports = 1;
            i++;
        }
        else if (strcmp ("-sleep", argv[i]) == 0)
        {
            gSleep = atoi (argv[i+1]);
            i += 2;
        }
        else if (strcmp ("-q", argv[i]) == 0)
        {
            gQuietLevel++;
            i++;
        }
        else if (strcmp ("-Q", argv[i]) == 0)
        {
            gQuietLevel--;
            i++;
        }
        else if (strcmp ("-bl",argv[i]) == 0)
        {
            gBlast = 1;
        gBlastNumber = atoi (argv[i+1]);
            i += 2;
        }
        else if (strcmp ("-bu",argv[i]) == 0)
        {
            gBurst = 1;
        gBurstNumber = atoi (argv[i+1]);
            i += 2;
        }
        else if (strcmp ("-bn",argv[i]) == 0)
        {
        gTotalBursts = atoi (argv[i+1]);
            i += 2;
        }
        else if (strcmp ("-n",argv[i]) == 0)
        {
            gMyName = argv[i+1];
            i += 2;
            check = false;
        }
        else if (strcmp ("-pp",argv[i]) == 0)
        {
            gPingPong = 1;
            gPingNumber = atoi (argv[i+1]);
            if (gPingNumber < 1000)
                gMsgInterval=gPingNumber;
            i += 2;
        }
        else if (strcmp ("-stats",argv[i]) == 0)
        {
            gMsgInterval = atoi (argv[i+1]);
            i += 2;
        }
        else if (strcmp ("-timer",argv[i]) == 0)
        {
            int timeInterval = atoi (argv[i+1]);
            gInterval= (double)timeInterval/1000.0;
            i += 2;
        }
    }
}


double calLatency2Points (int startSec, int startMicroSec, int endSec, int endMicroSec)
{
    double startTime = startSec * 1000000.0 + startMicroSec;
    double endTime   = endSec   * 1000000.0 + endMicroSec;
    double latency   = endTime - startTime;
    return latency / 1000.0;
}

static void
    signalCleanup(int sig)
{
    sigset_t set;

    sigemptyset (&set);
    sigaddset   (&set, SIGTERM);
    sigaddset   (&set, SIGINT);

    if( sigprocmask(SIG_BLOCK, &set, NULL) < 0)
    {
        fprintf(stderr,"Could not block signal\n");
    }

    ctrl_c_shutdown ();

    exit(1);
}
