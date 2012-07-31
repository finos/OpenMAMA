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
 *
 *---------------------------------------------------------------------------*/

#include <iostream>
#include "wombat/wincompat.h"

using std::cerr;
using std::cout;
using std::endl;

#include "mama/mama.h"
#include "mama/mamacpp.h"
#include "mama/MamaBasicSubscription.h"

using namespace Wombat;

#include "wombat/wMessageStats.h"

static MamaTransport*           gTransport          = NULL;
static MamaTransport*           gTransportPub       = NULL;
static MamaTransport*           gTransportSub       = NULL;
static MamaBasicSubscription*   gSubscription       = NULL;

static mamaBridge               gBridge             = NULL;
static const char *             gMiddleware         = "wmw";
static MamaQueue*               gDefaultQueue       = NULL;

// the reply simulater has 5 outbound topics and one inbound. 
static const char *           gOutBoundTopicCalc        = "MAMA_REPLY_CALC";
static const char *           gOutBoundTopicRisk        = "MAMA_REPLY_RISK";
static const char *           gOutBoundTopicPosition    = "MAMA_REPLY_POSITION";
static const char *           gOutBoundTopicSignal      = "MAMA_REPLY_SIGNAL";
static const char *           gOutBoundTopicHistorical  = "MAMA_REPLY_HISTORICAL";

static MamaPublisher*         gPublisherCalc            = NULL;
static MamaPublisher*         gPublisherRisk            = NULL;
static MamaPublisher*         gPublisherPosition        = NULL;
static MamaPublisher*         gPublisherSignal          = NULL;
static MamaPublisher*         gPublisherHistorical      = NULL;

static const char *           gInBoundTopic             = "MAMA_REQUEST";
static const char *           gTransportName            = "pong";
static const char *           gTransportNamePub         = "pongpub";
static const char *           gTransportNameSub         = "pongsub";
static int                    gUseSeparateTransports    = 0;
static int                    gSleep                    = 0;
static double                 gInterval                 = 0.5;
static int                    gQuietLevel               = 3;
static int                    gNumPublished             = 0;
static int                    gPrintInterval            = 10000;

static cpuVals                gCpuV1;
static cpuVals                gCpuV2;
static const char *           gUsageString[]            =
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
    "                         mama.properties. Default is pong",
    "      [-m middleware]    The middleware to use [wmw/lbm/tibrv].",
    "                         Default is wmw.", 
    "",
    " mamatest_replycpp  accepts the following command line arguments:",
    "      [-tport name]      The transport parameters to be used from",
    "                         mama.properties. Default is pong",
    NULL
};

static void parseCommandLine          (int argc, const char **argv);
static void initializeMama            (void);
static void createInboundSubscription (void);
static void createPublisher           (void);
static void usage                     (int exitStatus);

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

enum MAMA_MSG_TYPES
{
    MAMA_REPLY_CALC,
    MAMA_REPLY_RISK,
    MAMA_REPLY_POSITION,
    MAMA_REPLY_SIGNAL,
    MAMA_REPLY_HISTORICAL,
    MAMA_REQUEST
};

int main (int argc, const char **argv)
{
    parseCommandLine (argc, argv);

    initializeMama ();

    createInboundSubscription ();
    createPublisher ();
    sleep (2);
    startCpuTimer ();

    printf ("%10s,%10s,%10s,%10s,%10s,%10s\n",
            "#Pingpong",
            "user time",
            "sys time",
            "real time",
            "Percentage CPU",
            "Mem");
    Mama::start (gBridge);
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

        gPublisherCalc = new MamaPublisher;
        gPublisherCalc->create (transport, 
                                gOutBoundTopicCalc); 

        gPublisherRisk = new MamaPublisher;
        gPublisherRisk->create (transport, 
                                gOutBoundTopicRisk); 

        gPublisherPosition = new MamaPublisher;
        gPublisherPosition->create (transport, 
                                    gOutBoundTopicPosition); 

        gPublisherSignal = new MamaPublisher;
        gPublisherSignal->create (transport, 
                                  gOutBoundTopicSignal); 

        gPublisherHistorical = new MamaPublisher;
        gPublisherHistorical->create (transport, 
                                      gOutBoundTopicHistorical); 
    }
    catch (MamaStatus &status)
    {
        cerr << "Error creating publisher: " 
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
        int timeSec      = 0;
        int timeMicroSec = 0;
        int msgTag       = 0;

        if (gQuietLevel < 2)
        {
            printf ("\nRecieved inbound msg.\n");
        }

        // identify which server the request is intended for 
        int calc_server; 
        msg.tryI32 ("wCalcServer", 2, calc_server);
        
        // identify which server the request is intended for 
        int requestSeqNum; 
        msg.tryI32 ("wRequestSeqNum", 5, requestSeqNum);
        
        // find the machine name of the machine sending the request
        const char* id = NULL;
        msg.tryString ("wRequesterId", 4, id);
        char* requesterId = const_cast<char*> (id);

        // retrieve the request function 
        const char* func = NULL;
        msg.tryString ("wRequestFunction", 7, func);
        char* requestFunction = const_cast<char*> (func);

        if (gNumPublished == 0)
        {
            startCpuTimer ();
            getCpuTimeVals (&gCpuV1, 0);
        }

        if((gNumPublished+1)%gPrintInterval == 0)
        {
            struct memVals memV;
            struct cpuVals cpuV;
            getCpuTimeVals    (&gCpuV2, 0);
            getCpuTimeValDiff (gCpuV1, gCpuV2, &cpuV);

            getMemVals(getpid (), &memV);
            printf ("%10d,%10f,%10f,%10f,%10f,%10lu\n",
                    (gNumPublished+1),
                    gCpuV2.userTime,
                    gCpuV2.sysTime,
                    gCpuV2.realTime,
                    cpuV.pCpu,
                    memV.rss);
        }

        msg.tryI32 ("wMsgTag",       13, msgTag);
        msg.tryI32 ("wTimeSec",      14, timeSec);
        msg.tryI32 ("wTimeMicroSec", 15, timeMicroSec);

        if (gQuietLevel < 2)
        {
            printf ("calc server\t %i\n",      calc_server);
            printf ("requestFunction\t  %s\n", requestFunction);
            printf ("requester ID\t %s\n",     requesterId);
            printf ("request SeqNum\t %i\n",   requestSeqNum); 
        } 

        // decides which one of the calc servers processes the request
        switch (calc_server)
        {
            case MAMA_REPLY_CALC:
            {
                if (gQuietLevel < 2)
                {
                    printf ("Publishing Calc Reply\n");
                }

                MamaMsg *msg = new MamaMsg;
                msg->create ();

                msg->addI32    ("wMsgType",         1,  MAMA_MSG_TYPE_WOMBAT_REQUEST);
                msg->addI32    ("wCalcServer",      2,  MAMA_REPLY_CALC);                
                msg->addI32    ("wMsgStatus",       3,  MAMA_MSG_STATUS_OK);
                msg->addString ("wRequesterId",     4,  requesterId);                  
                msg->addI32    ("wRequestSeqNum",   5,  requestSeqNum);
                msg->addString ("wActivityTime",    6,  "12:00:00");
                msg->addString ("wRequestFunction", 7,  "getTestResult");
                msg->addI32    ("wMsgTag",          8,  msgTag);
                msg->addI32    ("wTimeSec",         9,  timeSec);
                msg->addI32    ("wTimeMicroSec",    10, timeMicroSec);

                gPublisherCalc->send (msg); 
                gNumPublished += 1; 
                delete msg;
                break;
            }
            case MAMA_REPLY_RISK:
            {
                if (gQuietLevel < 2)
                {
                    printf ("Publishing Risk Reply \n");
                }

                MamaMsg *msg = new MamaMsg;
                msg->create ();

                msg->addI32    ("wMsgType",         1,  MAMA_MSG_TYPE_WOMBAT_REQUEST);
                msg->addI32    ("wCalcServer",      2,  MAMA_REPLY_RISK);                
                msg->addI32    ("wMsgStatus",       3,  MAMA_MSG_STATUS_OK);
                msg->addString ("wRequesterId",     4,  requesterId);                  
                msg->addI32    ("wRequestSeqNum",   5,  requestSeqNum);
                msg->addString ("wActivityTime",    6,  "12:00:00");
                msg->addString ("wRequestFunction", 7,  "getTestResult");
                msg->addI32    ("wMsgTag",          8,  msgTag);
                msg->addI32    ("wTimeSec",         9,  timeSec);
                msg->addI32    ("wTimeMicroSec",    10, timeMicroSec);

                gPublisherRisk->send (msg);
                gNumPublished += 1; 

                if (gNumPublished%gPrintInterval == 0)
                {
                    printf ("%i\n", gNumPublished); 
                }  

                delete msg;
                break;
            }
            case MAMA_REPLY_POSITION:
            {
                if (gQuietLevel < 2)
                {
                   printf ("Publishing Position Reply \n");
                }

                MamaMsg *msg = new MamaMsg;
                msg->create ();

                msg->addI32    ("wMsgType",         1,  MAMA_MSG_TYPE_WOMBAT_REQUEST);
                msg->addI32    ("wCalcServer",      2,  MAMA_REPLY_POSITION);                
                msg->addI32    ("wMsgStatus",       3,  MAMA_MSG_STATUS_OK);
                msg->addString ("wRequesterId",     4,  requesterId);                  
                msg->addI32    ("wRequestSeqNum",   5,  requestSeqNum);
                msg->addString ("wActivityTime",    6,  "12:00:00");
                msg->addString ("wRequestFunction", 7,  "getTestResult");
                msg->addI32    ("wMsgTag",          8,  msgTag);
                msg->addI32    ("wTimeSec",         9,  timeSec);
                msg->addI32    ("wTimeMicroSec",    10, timeMicroSec);

                gPublisherPosition->send (msg); 
                gNumPublished += 1;

                if (gNumPublished%gPrintInterval == 0)
                {
                    printf ("%i\n", gNumPublished); 
                }  

                delete msg;
                break;
            }
            case MAMA_REPLY_SIGNAL:
            {
                if (gQuietLevel < 2)
                {
                    printf ("Publishing Signal Reply \n");
                }
                
                MamaMsg *msg = new MamaMsg;
                msg->create ();

                msg->addI32    ("wMsgType",         1,  MAMA_MSG_TYPE_WOMBAT_REQUEST);
                msg->addI32    ("wCalcServer",      2,  MAMA_REPLY_SIGNAL);                
                msg->addI32    ("wMsgStatus",       3,  MAMA_MSG_STATUS_OK);
                msg->addString ("wRequesterId",     4,  requesterId);                  
                msg->addI32    ("wRequestSeqNum",   5,  requestSeqNum);
                msg->addString ("wActivityTime",    6,  "12:00:00");
                msg->addString ("wRequestFunction", 7,  "getTestResult");
                msg->addI32    ("wMsgTag",          8,  msgTag);
                msg->addI32    ("wTimeSec",         9,  timeSec);
                msg->addI32    ("wTimeMicroSec",    10, timeMicroSec);

                gPublisherSignal->send (msg); 
                gNumPublished += 1; 

                if (gNumPublished%gPrintInterval == 0)
                {
                    printf ("%i\n", gNumPublished); 
                }  

                delete msg;
                break;
            }
            case MAMA_REPLY_HISTORICAL:
            {
                if (gQuietLevel < 2)
                {
                    printf ("Publishing Historical Reply \n");
                }
                
                MamaMsg *msg = new MamaMsg;
                msg->create ();

                msg->addI32    ("wMsgType",         1,  MAMA_MSG_TYPE_WOMBAT_REQUEST);
                msg->addI32    ("wCalcServer",      2,  MAMA_REPLY_HISTORICAL);                
                msg->addI32    ("wMsgStatus",       3,  MAMA_MSG_STATUS_OK);
                msg->addString ("wRequesterId",     4,  requesterId);                  
                msg->addI32    ("wRequestSeqNum",   5,  requestSeqNum);
                msg->addString ("wActivityTime",    6,  "12:00:00");
                msg->addString ("wRequestFunction", 7,  "getTestResult");
                msg->addI32    ("wMsgTag",          8,  msgTag);
                msg->addI32    ("wTimeSec",         9,  timeSec);
                msg->addI32    ("wTimeMicroSec",    10, timeMicroSec);

                gPublisherHistorical->send (msg); 
                gNumPublished += 1; 

                delete msg;
                break;
            }
            case MAMA_REQUEST:
            {
                // will always be a request in the reply server 
                break;
            }
            default:
            {
            }
        }
    }
};

static void createInboundSubscription (void)
{
    try
    {
        MamaTransport* transport = gUseSeparateTransports ?
                                   gTransportSub : gTransport;

        gSubscription = new MamaBasicSubscription;

        cout << "Creating basic subscription. " << gDefaultQueue << endl;
        gSubscription->createBasic (
                            transport,
                            gDefaultQueue,
                            new SubscriptionCallback (),
                            gInBoundTopic);
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
    for (i = 1; i < argc;)
    {
        if (strcmp ("-s", argv[i]) == 0)
        {
            //gOutBoundTopic = argv[i+1];
            i += 2;
        }
        else if (strcmp ("-l", argv[i]) == 0)
        {
            gInBoundTopic = argv[i+1];
            i += 2;
        }
        else if (strcmp ("-i", argv[i]) == 0)
        {
            gInterval = strtod (argv[i+1], NULL);
            i += 2;
        }
        else if ((strcmp (argv[i], "-h") == 0) ||
                 (strcmp (argv[i], "-?") == 0))
        {
            usage (0);
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
        else if (strcmp ("-stats", argv[i]) == 0)
        {
            gPrintInterval = atoi (argv[i+1]);
            i += 2;
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
        else if (strcmp ("-Q", argv[i]) == 0)
        {
            gQuietLevel--;
            i++;
        }
        else if (strcmp ("-Q", argv[i]) == 0)
        {
            gQuietLevel--;
            i++;
        }
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

