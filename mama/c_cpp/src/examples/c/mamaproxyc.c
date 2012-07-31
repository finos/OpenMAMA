/* $Id: mamaproxyc.c,v 1.1.2.2 2011/09/27 16:54:39 emmapollock Exp $
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

#include "wombat/port.h"
#include "mama/mama.h"
#include "mama/log.h"
#include "mama/msgtype.h"
#include "mama/subscmsgtype.h"
#include "mama/subscription.h"
#include "mama/dqpublisher.h"
#include "mama/dqpublishermanager.h"
#include "string.h"

static const char *  gUsageString[] =
{
" This sample application demonstrates how to publish mama messages, and",
" respond to requests from a clients inbox.",
"",
" It accepts the following command line arguments:",
"      [-s topic]         The topic on which to send messages. Default value",
"                         is \"MAMA_TOPIC\".",
"      [-l topic]         The topic on which to listen for inbound requests.",
"                         Default is \"MAMA_INBOUND_TOPIC\".",
"      [-SP source]       The source name to use for publisher Default is WOMBAT.",
"      [-SS source]       The source name to use for subscriber Default is WOMBAT.",
"      [-i interval]      The interval between messages .Default, 0.5.",
"      [-mp middleware]   The middleware to use for publisher [wmw/lbm/tibrv]. Default is wmw.",
"      [-ms middleware]   The middleware to use for subscriber [wmw/lbm/tibrv]. Default is wmw.",
"      [-tport name]      The transport parameters to be used from",
"                         mama.properties. Default is pub",
"      [-q]               Quiet mode. Suppress output.",
NULL
};

#define MAX_SUBSCRIPTIONS 250000

typedef struct pubCache_
{
    const char * symbol;
    mamaDQPublisher  pub;
    mamaMsg          cachedMsg;
    mamaSubscription sub;
}pubCache;


typedef struct recapInfo_
{
    const char * symbol;
    int index;
    mamaMsg   msg;
}recapInfo;

static mamaBridge               gPubBridge   = NULL;
static mamaBridge               gSubBridge   = NULL;
static const char*              gPubMiddleware   = "wmw";
static const char*              gSubMiddleware   = "wmw";
static mamaQueue               gPubDefaultQueue   = NULL;
static mamaQueue               gSubDefaultQueue   = NULL;
static const char*              gPubTransportName   = NULL;
static const char*              gSubTransportName   = NULL;
static int                      gQuietness   = 0;
static const char*              gFilename   = NULL;
static const char*              gPubSource   = "WOMBAT";
static const char*              gSubSource   = "WOMBAT";
static mama_bool_t              gSendSync   = 0;
static const char**             gSymbolList         = NULL;
static mamaTransport           gPubTransport   = NULL;
static mamaTransport           gSubTransport   = NULL;
static mamaDQPublisherManager gDQPubManager   = NULL;
static pubCache* gSubscriptionList   = NULL;

static mamaSource        gSubscriptionSource = NULL;

    static int               gNumSymbols         = 0;

static MamaLogLevel            gSubscLogLevel;
static mamaTimer               gSyncTimer;
static void parseCommandLine (int argc, const char** argv);

static void MAMACALLTYPE
subscriptionOnMsg   (mamaSubscription    subscription,
                     mamaMsg             msg,
                     void*               closure,
                     void*               itemClosure);
static void MAMACALLTYPE
subscriptionOnError (mamaSubscription    subscription,
                     mama_status         status,
                     void*               platformError,
                     const char*         subject,
                     void*               closure);
static void MAMACALLTYPE
subscriptionOnCreate (mamaSubscription    subscription,
                      void*               closure);

static void MAMACALLTYPE
syncCallback       (mamaTimer           timer,
                    void *              closure);

static void MAMACALLTYPE
subscriptionHandlerOnCreateCb (mamaDQPublisherManager manager);

static void MAMACALLTYPE
subscriptionHandlerOnNewRequestCb(mamaDQPublisherManager manager,
                                  const char*        symbol,
                                  short  subType,
                                  short    msgType,
                                  mamaMsg     msg);

static void MAMACALLTYPE
subscriptionHandlerOnRequestCb(mamaDQPublisherManager manager,
                               mamaPublishTopic* info,
                               short  subType,
                               short    msgType,
                               mamaMsg     msg);

static void MAMACALLTYPE
subscriptionHandlerOnRefreshCb(mamaDQPublisherManager manager,
                               mamaPublishTopic* info,
                               short  subType,
                               short    msgType,
                               mamaMsg     msg);

static void MAMACALLTYPE
subscriptionHandlerOnErrorCb(mamaDQPublisherManager manager,
                             mama_status  status,
                             const char*        errortxt,
                             mamaMsg     msg);

static void MAMACALLTYPE
sendRecap     (mamaQueue queue, void* closure);

static void MAMACALLTYPE
startCB ( mama_status status );

static void createPublisher (void);

static void start (void);

static void subscribeToSymbols (void);

static void initializeMama (void);

void usage (int exitStatus);

static void createPublisher ()
{
    mamaDQPublisherManagerCallbacks managerCallback;

    memset(&managerCallback, 0, sizeof(managerCallback));
    managerCallback.onCreate =   subscriptionHandlerOnCreateCb;
    managerCallback.onError =subscriptionHandlerOnErrorCb;
    managerCallback.onNewRequest =subscriptionHandlerOnNewRequestCb;
    managerCallback.onRequest =subscriptionHandlerOnRequestCb;
    managerCallback.onRefresh =subscriptionHandlerOnRefreshCb;

    srand ( time(NULL) );

    mamaDQPublisherManager_allocate (&gDQPubManager);

    mamaDQPublisherManager_create (gDQPubManager, gPubTransport,
                                   gPubDefaultQueue,
                                   &managerCallback,
                                   gPubSource,
                                   "_MD",
                                   NULL);

	// Turn off updateing of the messages as incoming messages 
	// from FH already have seqnum and senderid
	mamaDQPublisherManager_setSeqNum(gDQPubManager, 0);
	mamaDQPublisherManager_setSenderId(gDQPubManager, 0);
    if (gSendSync)
    {
        mamaTimer_create (&gSyncTimer, gPubDefaultQueue, syncCallback, 15, gDQPubManager);
    }
}

void MAMACALLTYPE startCB ( mama_status status )
{}

static void start ()
{
    if (gPubBridge != gSubBridge)
    {
        mama_startBackground (gPubBridge, startCB);
    }

    mama_start (gSubBridge);
}

static void subscribeToSymbols ()
{
    int i       = 0;

    mamaMsgCallbacks callbacks;
    memset(&callbacks, 0, sizeof(callbacks));
    callbacks.onCreate  = subscriptionOnCreate;
    callbacks.onError   = subscriptionOnError;
    callbacks.onMsg     = subscriptionOnMsg;
    callbacks.onQuality = NULL;
    callbacks.onGap     = NULL;
    callbacks.onRecapRequest = NULL;

    gSubscriptionList = (pubCache*)calloc (gNumSymbols,
                                                   sizeof (pubCache));


    for (i = 0; i < gNumSymbols; i++)
    {
        mamaSubscription_allocate (&gSubscriptionList[i].sub);
        mamaSubscription_create (gSubscriptionList[i].sub,
                                 gSubDefaultQueue,
                                 &callbacks,
                                 gSubscriptionSource,
                                 gSymbolList[i],
                                 (void*)i);

		gSubscriptionList[i].symbol = gSymbolList[i];
		mamaMsg_create(&gSubscriptionList[i].cachedMsg);
    }
}

static void initializeMama ()
{
    mama_status status;

    status = mama_loadBridge (&gPubBridge, gPubMiddleware);
    status = mama_loadBridge (&gSubBridge, gSubMiddleware);

    mama_open();

    mama_getDefaultEventQueue (gPubBridge, &gPubDefaultQueue);
    mama_getDefaultEventQueue (gSubBridge, &gSubDefaultQueue);

    mamaTransport_allocate (&gSubTransport);
    mamaTransport_create (gSubTransport,
                          gSubTransportName,
                          gSubBridge);

    mamaTransport_allocate (&gPubTransport);
    mamaTransport_create (gPubTransport,
                          gPubTransportName,
                          gPubBridge);

    /*The mamaSource used for all subscription creation*/
    if (MAMA_STATUS_OK!=(status=mamaSource_create (&gSubscriptionSource)))
    {
        fprintf (stderr,
                 "Failed to create subscription mamaSource STATUS: %d %s\n",
                 status, mamaStatus_stringForStatus (status));
        exit(1);
    }

    mamaSource_setId (gSubscriptionSource, "Subscription_Source");
    mamaSource_setTransport (gSubscriptionSource, gSubTransport);
    mamaSource_setSymbolNamespace (gSubscriptionSource, gSubSource);


}

static void MAMACALLTYPE
syncCallback (mamaTimer timer, void* closure)
{
    mamaTimer_destroy (timer);

    mamaDQPublisherManager_sendSyncRequest ((mamaDQPublisherManager) closure,50, 5, 30);
    printf ("Sent sync request.\n");
    fflush(stdout);
}

int main (int argc, const char **argv)
{

    gSymbolList = (const char**)calloc (MAX_SUBSCRIPTIONS, sizeof (char*));

    parseCommandLine (argc, argv);

    if (!gNumSymbols)
        gSymbolList[gNumSymbols++] = "MAMA_TOPIC";

    initializeMama ();
    createPublisher ();
    subscribeToSymbols ();
    start();

    return 0;
}

static void MAMACALLTYPE
subscriptionHandlerOnCreateCb (mamaDQPublisherManager manager)
{
    printf ("Created publisher subscription.\n");
}

static void MAMACALLTYPE
subscriptionHandlerOnErrorCb (mamaDQPublisherManager manager,
                              mama_status            status,
                              const char*            errortxt,
                              mamaMsg                msg)
{
    if (msg)
    {
        printf ("Unhandled Msg: %s (%s) %s\n",
                mamaStatus_stringForStatus (status),
                mamaMsg_toString (msg),
                errortxt);
    }
    else
    {
        printf("Unhandled Msg: %s %s\n",
               mamaStatus_stringForStatus (status),
               errortxt);
    }
}

static void MAMACALLTYPE
subscriptionHandlerOnNewRequestCb (mamaDQPublisherManager manager,
                                   const char*            symbol,
                                   short                  subType,
                                   short                  msgType,
                                   mamaMsg                msg)
{
    int i = 0;
    mamaQueue queue;
    recapInfo*  info = NULL;

    for (i=0; i < gNumSymbols; i++)
    {
       if (strcmp (gSubscriptionList[i].symbol, symbol) == 0)
            break;
    }

    if (i == gNumSymbols)
    {
               printf ("Received request for unknown symbol: %s\n", symbol);

        return;
    }

    mamaDQPublisherManager_createPublisher (manager, symbol, (void*)i, &gSubscriptionList[i].pub);

    printf ("Received new request: %s\n", symbol);


    info = (recapInfo*) calloc (1, sizeof(recapInfo));
    info->index =i;
    mamaSubscription_getQueue(gSubscriptionList[i].sub, &queue);
    switch (msgType)
    {
        case MAMA_SUBSC_SUBSCRIBE:
        case MAMA_SUBSC_SNAPSHOT:
            mamaMsg_detach (msg);

            info->msg = msg;
            mamaQueue_enqueueEvent  (queue,
                                     sendRecap,
                                     info);
        break;
        default:
           mamaQueue_enqueueEvent  (queue,
                                    sendRecap,
                                    info);
        break;
    }

}

static void MAMACALLTYPE
sendRecap     (mamaQueue queue, void* closure)
{
    recapInfo* info = (recapInfo*) closure;


       mamaMsg_updateU8(gSubscriptionList[info->index].cachedMsg, NULL, MamaFieldMsgType.mFid, MAMA_MSG_TYPE_INITIAL);

       if (info->msg)
       {
            mamaDQPublisher_sendReply(gSubscriptionList[info->index].pub, info->msg, gSubscriptionList[info->index].cachedMsg);
            mamaMsg_destroy( info->msg);

        }
        else
          mamaDQPublisher_send(gSubscriptionList[info->index].pub, gSubscriptionList[info->index].cachedMsg);


    free (info);
}

static void MAMACALLTYPE
subscriptionHandlerOnRequestCb (mamaDQPublisherManager manager,
                                mamaPublishTopic*      publishTopicInfo,
                                short                  subType,
                                short                  msgType,
                                mamaMsg                msg)
{

    recapInfo *info = NULL;
    mamaQueue queue;

    printf ("Received request: %s\n", publishTopicInfo->symbol);

    switch (msgType)
    {
        case MAMA_SUBSC_SUBSCRIBE:
        case MAMA_SUBSC_SNAPSHOT:
            mamaMsg_detach (msg);
            info = (recapInfo*) calloc (1, sizeof(recapInfo));
            info->index =(int)publishTopicInfo->cache;
            info->msg = msg;
            mamaSubscription_getQueue(gSubscriptionList[info->index].sub, &queue);
            mamaQueue_enqueueEvent  (queue,
                             sendRecap,
                             info);
            break;
        case MAMA_SUBSC_DQ_SUBSCRIBER :
        case MAMA_SUBSC_DQ_PUBLISHER:
        case MAMA_SUBSC_DQ_NETWORK:
        case MAMA_SUBSC_DQ_UNKNOWN:
        case MAMA_SUBSC_DQ_GROUP_SUBSCRIBER:
            info = (recapInfo*) calloc (1, sizeof(recapInfo));
            info->index =(int)publishTopicInfo->cache;
            mamaSubscription_getQueue(gSubscriptionList[info->index].sub, &queue);
            mamaQueue_enqueueEvent  (queue,
                             sendRecap,
                             info);
            break;

        case MAMA_SUBSC_UNSUBSCRIBE:
        case MAMA_SUBSC_RESUBSCRIBE:
        case MAMA_SUBSC_REFRESH:
        default:
            break;
    }
}

static void MAMACALLTYPE
subscriptionHandlerOnRefreshCb (mamaDQPublisherManager  publisherManager,
                                     mamaPublishTopic*         publishTopicInfo,
                                     short                    subType,
                                     short                    msgType,
                                     mamaMsg                 msg)
{
    printf ("Received Refresh: %s\n",  publishTopicInfo->symbol);
}


static void readSymbolsFromFile (void)
{
    /* get subjects from file or interactively */
    FILE* fp = NULL;
    char charbuf[1024];

    if (gFilename)
    {
        if ((fp = fopen (gFilename, "r")) == (FILE *)NULL)
        {
            perror (gFilename);
            exit (1);
        }
    }
    else
    {
        fp = stdin;
    }
    if (isatty(fileno (fp)))
    {
        printf ("Enter one symbol per line and terminate with a .\n");
        printf ("Symbol> ");
    }
    while (fgets (charbuf, 1023, fp))
    {
        char *c = charbuf;

        /* Input terminate case */
        if (*c == '.')
            break;

        /* replace newlines with NULLs */
        while ((*c != '\0') && (*c != '\n'))
            c++;
        *c = '\0';

        gSymbolList[gNumSymbols++] = strdup (charbuf);
        if (isatty(fileno (fp)))
            printf ("Symbol> ");
    }
}

static void parseCommandLine (int argc, const char** argv)
{
    int i = 0;
    for (i = 1; i < argc;)
    {
        if (strcmp (argv[i], "-SS") == 0)
        {
            gSubSource = argv[i + 1];
            i += 2;
        }
        else if (strcmp (argv[i], "-SP") == 0)
        {
            gPubSource = argv[i + 1];
            i += 2;
        }
        else if (strcmp (argv[i], "-s") == 0)
        {
            gSymbolList[gNumSymbols++]=strdup (argv[i + 1]);
                i += 2;
        }
        else if (strcmp (argv[i], "-f") == 0)
        {
            gFilename = argv[i + 1];
            i += 2;
        }
        else if ((strcmp (argv[i], "-h") == 0) ||
                 (strcmp (argv[i], "-?") == 0))
        {
            usage (0);
            i++;
        }
        else if (strcmp ("-tports", argv[i]) == 0)
        {
            gSubTransportName = argv[i+1];
            i += 2;
        }
        else if (strcmp ("-tportp", argv[i]) == 0)
        {
            gPubTransportName = argv[i+1];
            i += 2;
        }
        else if (strcmp ("-ms", argv[i]) == 0)
        {
            gSubMiddleware = argv[i+1];
            i += 2;
        }
        else if (strcmp ("-mp", argv[i]) == 0)
        {
            gPubMiddleware = argv[i+1];
            i += 2;
        }
        else if (strcmp ("-q", argv[i]) == 0)
        {
            gQuietness++;
            i++;
        }
        else if (strcmp ("-sync", argv[i]) == 0)
        {
            gSendSync=1;
            i++;
        }
        else if (strcmp( argv[i], "-v") == 0 )
        {
            if (gSubscLogLevel == MAMA_LOG_LEVEL_WARN)
            {
                gSubscLogLevel = MAMA_LOG_LEVEL_NORMAL;
                mama_enableLogging (stderr, MAMA_LOG_LEVEL_NORMAL);
            }
            else if (gSubscLogLevel == MAMA_LOG_LEVEL_NORMAL)
            {
                gSubscLogLevel = MAMA_LOG_LEVEL_FINE;
                mama_enableLogging (stderr, MAMA_LOG_LEVEL_FINE);
            }
            else if (gSubscLogLevel == MAMA_LOG_LEVEL_FINE)
            {
                gSubscLogLevel = MAMA_LOG_LEVEL_FINER;
                mama_enableLogging (stderr, MAMA_LOG_LEVEL_FINER);
            }
            else
            {
                gSubscLogLevel = MAMA_LOG_LEVEL_FINEST;
                mama_enableLogging (stderr, MAMA_LOG_LEVEL_FINEST);
            }
            i++;
        }
        else
        {
            printf ("Unknown arg %s\n", argv[i]);
            usage (0);
        }
    }

    if (gNumSymbols==0)
        readSymbolsFromFile ();
}

void
usage (int exitStatus)
{
   int i = 0;
   while (gUsageString[i] != NULL)
   {
       printf ("%s\n", gUsageString[i++]);
   }
   exit(exitStatus);
}


void MAMACALLTYPE
subscriptionOnCreate (mamaSubscription subscription, void* closure)
{

}

static void MAMACALLTYPE
subscriptionOnError (mamaSubscription subscription,
                     mama_status      status,
                     void*            platformError,
                     const char*      subject,
                     void*            closure)
{
    fprintf (stdout,
             "An error occurred creating subscription for %s: %s\n",
             subject ? subject : "(null)",
             mamaStatus_stringForStatus (status));
}

void MAMACALLTYPE
subscriptionOnMsg  (mamaSubscription subscription,
                    mamaMsg msg,
                    void *closure,
                    void *itemClosure)
{
    switch (mamaMsgType_typeForMsg (msg))
    {
    case MAMA_MSG_TYPE_DELETE:
    case MAMA_MSG_TYPE_EXPIRE:
        mamaSubscription_destroy (subscription);
        mamaSubscription_deallocate (subscription);
        if (gSubscriptionList[(size_t)closure].pub)
            mamaDQPublisher_send(gSubscriptionList[(size_t)closure].pub, msg);
        gSubscriptionList[(size_t)closure].sub = NULL;
        return;
    default:
        break;
    }

    switch (mamaMsgStatus_statusForMsg (msg))
    {
    case MAMA_MSG_STATUS_BAD_SYMBOL:
    case MAMA_MSG_STATUS_EXPIRED:
    case MAMA_MSG_STATUS_TIMEOUT:
        mamaSubscription_destroy (subscription);
        mamaSubscription_deallocate (subscription);
        if (gSubscriptionList[(size_t)closure].pub)
            mamaDQPublisher_send(gSubscriptionList[(size_t)closure].pub, msg);
        gSubscriptionList[(size_t)closure].sub = NULL;
        return;
    default:
        break;
    }

    mamaMsg_applyMsg(gSubscriptionList[(size_t)closure].cachedMsg, msg);

    if (gSubscriptionList[(size_t)closure].pub)
    {
        mamaDQPublisher_setStatus(gSubscriptionList[(size_t)closure].pub, mamaMsgStatus_statusForMsg (msg));
         mamaDQPublisher_send(gSubscriptionList[(size_t)closure].pub, msg);
    }

    fflush(stdout);
}
