/* $Id: capturereplayc.c,v 1.1.2.4 2011/10/02 19:04:28 ianbell Exp $
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
#include "mama/log.h"
#include "mama/msg.h"
#include "mama/msgtype.h"
#include "mama/subscmsgtype.h"
#include "mama/subscription.h"
#include "mama/dqpublisher.h"
#include "mama/dqpublishermanager.h"
#include "playback/playbackFileParser.h"
#include "string.h"
#include "unistd.h"

static const char *  gUsageString[] =
{
" This sample application demonstrates how to publish mama messages, and",
" respond to requests from a clients inbox.",
"",
" It accepts the following command line arguments:",
"      [-S source]       The source name to use for publisher Default is WOMBAT.",
"      [-i interval]     The interval between messages .Default in  0.5.",
"      [-f filename]     The capture filename",
"      [-m middleware]   The middleware to use for publisher [wmw/lbm/tibrv]. Default is wmw.",
"      [-tport name]     The transport parameters to be used from",
"                        mama.properties. Default is pub",
"      [-q]              Quiet mode. Suppress output.",
NULL
};

#define MAX_SUBSCRIPTIONS 250000

typedef struct pubCache_
{
    char*                   symbol;
    mamaDQPublisher         pub;
    mamaMsg                 cachedMsg;
    mamaPlaybackFileParser  fileParser;
}pubCache;



static mamaBridge               gPubBridge          = NULL;
static const char*              gPubMiddleware      = "wmw";
static mamaQueue                gPubDefaultQueue    = NULL;
static const char*              gPubTransportName   = "pub";
static int                      gQuietness          = 0;
static const char*              gFilename           = NULL;
static const char*              gPubSource          = "WOMBAT";
static const char**             gSymbolList         = NULL;
static mamaTransport            gPubTransport       = NULL;
static mamaDQPublisherManager   gDQPubManager       = NULL;
static pubCache*                gSubscriptionList   = NULL;
static int                      gNumSymbols         = 0;
static MamaLogLevel             gSubscLogLevel      = MAMA_LOG_LEVEL_WARN;
static mamaTimer                gPubTimer           = NULL;
static double                   gTimeInterval       = 0.5;
#define DELIM                   ':'

static void parseCommandLine (int argc, const char** argv);

static void
subscriptionHandlerOnCreateCb (mamaDQPublisherManager manager);

static void
subscriptionHandlerOnNewRequestCb(mamaDQPublisherManager manager,
                                  const char*            symbol,
                                  short                  subType,
                                  short                  msgType,
                                  mamaMsg                msg);
static void
subscriptionHandlerOnRequestCb(mamaDQPublisherManager manager,
                               mamaPublishTopic*      info,
                               short                  subType,
                               short                  msgType,
                               mamaMsg                msg);
static void
subscriptionHandlerOnRefreshCb(mamaDQPublisherManager manager,
                               mamaPublishTopic*      info,
                               short                  subType,
                               short                  msgType,
                               mamaMsg                msg);
static void
subscriptionHandlerOnErrorCb(mamaDQPublisherManager manager,
                             mama_status            status,
                             const char*            errortxt,
                             mamaMsg                msg);

static void     createPublisher (void);
static void     initializeMama (void);
void            usage (int exitStatus);
static void readSymbolsFromFile (void);


static void
pubCallback (mamaTimer  timer, void*   closure)
{
    int index =0;
    char*temp=NULL;
    char*source=NULL;
    char*           headerString = NULL;
    mamaMsg        newMessage;

    for (index=0; index < gNumSymbols; index++)
    {
        if (gSubscriptionList[index].fileParser)
        {
            while (mamaPlaybackFileParser_getNextHeader(gSubscriptionList[index].fileParser, &headerString))
            {
                if (strlen (headerString) == 0) continue;
                /*skip source and transport name*/
                temp = strchr (headerString,DELIM);
                temp++;
                source = strchr (temp,DELIM);
                source++; /*skip :*/

                temp = strchr (source,DELIM);


				if (mamaPlaybackFileParser_getNextMsg (gSubscriptionList[index].fileParser,
														&newMessage))
				{

					if ((strncmp (gSubscriptionList[index].symbol, source, temp-source) == 0)  && (strlen(gSubscriptionList[index].symbol) == temp-source))
					{
						mamaMsg_applyMsg (gSubscriptionList[index].cachedMsg, newMessage);
						mamaDQPublisher_send(gSubscriptionList[index].pub, newMessage);
						break;
					}
				}
            }
        }
    }
}


static void createPublisher ()
{
    mamaDQPublisherManagerCallbacks managerCallback;

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
}

static void start_timed_publish (void)
{


    mamaTimer_create (&gPubTimer,
                      gPubDefaultQueue,
                      pubCallback,
                      gTimeInterval,
                      NULL);
}


static void initializeMama ()
{
    mama_status status;
    status = mama_loadBridge (&gPubBridge, gPubMiddleware);

    if (status != MAMA_STATUS_OK)
        exit(1);

    status = mama_open();

    if (status != MAMA_STATUS_OK)
        exit(1);

    mama_getDefaultEventQueue (gPubBridge, &gPubDefaultQueue);

    mamaTransport_allocate (&gPubTransport);
    mamaTransport_create (gPubTransport,
                          gPubTransportName,
                          gPubBridge);

}

int main (int argc, const char **argv)
{

    gSymbolList = (const char**)calloc (MAX_SUBSCRIPTIONS, sizeof (char*));

    parseCommandLine (argc, argv);

    printf ("Reading symbols from file, please standby.....\n");

    fflush(stdout);
    initializeMama ();

    readSymbolsFromFile();


    createPublisher ();

    start_timed_publish();

    printf ("Ready for Subscriptions.\n");
    fflush(stdout);

    mama_start (gPubBridge);

    return 0;
}

static void
subscriptionHandlerOnCreateCb (mamaDQPublisherManager manager)
{
    printf ("Created publisher subscription.\n");
}

static void
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

static void
subscriptionHandlerOnNewRequestCb (mamaDQPublisherManager manager,
                                   const char*            symbol,
                                   short                  subType,
                                   short                  msgType,
                                   mamaMsg                msg)
{
    int index = 0;
    char* headerString;
    char *temp;
    char * source;
    mamaMsg newMessage;

    for (index=0; index < gNumSymbols; index++)
    {
       if (strcmp (gSubscriptionList[index].symbol, symbol) == 0)
            break;
    }

    if (index == gNumSymbols)
    {
        printf ("Received request for unknown symbol: %s\n", symbol);
        return;
    }

    printf ("Received new request: %s\n", symbol);

    mamaDQPublisherManager_createPublisher (manager, symbol, (void*)index, &gSubscriptionList[index].pub);
    mamaPlaybackFileParser_allocate (&gSubscriptionList[index].fileParser);
    mamaPlaybackFileParser_openFile(gSubscriptionList[index].fileParser, (char*)gFilename);
    mamaMsg_create(&gSubscriptionList[index].cachedMsg);

    while (mamaPlaybackFileParser_getNextHeader(gSubscriptionList[index].fileParser, &headerString))
    {
        /* skip source and transport name */
        temp = strchr (headerString,DELIM);
        temp++;
        source = strchr (temp,DELIM);
        source++; /* skip : */

        temp = strchr (source,DELIM);
        if ((strncmp (gSubscriptionList[index].symbol, source, temp-source) == 0) && (strlen(gSubscriptionList[index].symbol) == temp-source))
                break;
        mamaPlaybackFileParser_getNextMsg (gSubscriptionList[index].fileParser,
                                                    &newMessage);
    }

    if (mamaPlaybackFileParser_getNextMsg (gSubscriptionList[index].fileParser,
                                            &newMessage))
    {
        mamaMsg_applyMsg (gSubscriptionList[index].cachedMsg, newMessage);

        switch (msgType)
        {
            case MAMA_SUBSC_SUBSCRIBE:
            case MAMA_SUBSC_SNAPSHOT:
                mamaMsg_updateU8(gSubscriptionList[index].cachedMsg, NULL, MamaFieldMsgType.mFid, MAMA_MSG_TYPE_INITIAL);
                mamaDQPublisher_sendReply(gSubscriptionList[index].pub, msg, gSubscriptionList[index].cachedMsg);
                break;
            default:
                mamaMsg_updateU8(gSubscriptionList[index].cachedMsg, NULL, MamaFieldMsgType.mFid, MAMA_MSG_TYPE_RECAP);
                mamaDQPublisher_send(gSubscriptionList[index].pub, gSubscriptionList[index].cachedMsg);
                break;
        }
    }

}



static void
subscriptionHandlerOnRequestCb (mamaDQPublisherManager manager,
                                mamaPublishTopic*      publishTopicInfo,
                                short                  subType,
                                short                  msgType,
                                mamaMsg                msg)
{
    int index =0;

    printf ("Received request: %s\n", publishTopicInfo->symbol);

    switch (msgType)
    {
        case MAMA_SUBSC_SUBSCRIBE:
        case MAMA_SUBSC_SNAPSHOT:
            index =(int)publishTopicInfo->cache;
            mamaMsg_updateU8(gSubscriptionList[index].cachedMsg, NULL, MamaFieldMsgType.mFid, MAMA_MSG_TYPE_INITIAL);
            mamaDQPublisher_sendReply(gSubscriptionList[index].pub, msg, gSubscriptionList[index].cachedMsg);
            break;
        case MAMA_SUBSC_DQ_SUBSCRIBER :
        case MAMA_SUBSC_DQ_PUBLISHER:
        case MAMA_SUBSC_DQ_NETWORK:
        case MAMA_SUBSC_DQ_UNKNOWN:
        case MAMA_SUBSC_DQ_GROUP_SUBSCRIBER:
            index =(int)publishTopicInfo->cache;
            mamaMsg_updateU8(gSubscriptionList[index].cachedMsg, NULL, MamaFieldMsgType.mFid, MAMA_MSG_TYPE_RECAP);
            mamaDQPublisher_send(gSubscriptionList[index].pub, gSubscriptionList[index].cachedMsg);
            break;
        case MAMA_SUBSC_UNSUBSCRIBE:
        case MAMA_SUBSC_RESUBSCRIBE:
        case MAMA_SUBSC_REFRESH:
        default:
            break;
    }
}

static void
subscriptionHandlerOnRefreshCb (mamaDQPublisherManager  publisherManager,
                                mamaPublishTopic*       publishTopicInfo,
                                short                   subType,
                                short                   msgType,
                                mamaMsg                 msg)
{
    printf ("Received Refresh: %s\n",  publishTopicInfo->symbol);
}

static void readSymbolsFromFile (void)
{
    mamaPlaybackFileParser  fileParser;
    char*           headerString = NULL;
    char*           temp = NULL;
    char * source = NULL;
    int             symbolIndex = 0, i=0;
    mamaMsg        newMessage;

    gSubscriptionList = (pubCache*)calloc (MAX_SUBSCRIPTIONS,
                                                   sizeof (pubCache));

    mamaPlaybackFileParser_allocate (&fileParser);
    mamaPlaybackFileParser_openFile(fileParser, (char*)gFilename);

    while (mamaPlaybackFileParser_getNextHeader(fileParser, &headerString))
    {
        if (mamaPlaybackFileParser_getNextMsg (fileParser,
                                                &newMessage))
        {
            temp = strchr (headerString,DELIM);
            temp++;
            source = strchr (temp,DELIM);
            source++;

            temp = strchr (source,DELIM);

            for (i=0;i<symbolIndex;i++)
            {
                int res = strncmp (gSubscriptionList[i].symbol, source, temp-source);

                 if (( res == 0)  && (strlen(gSubscriptionList[i].symbol) == temp-source))
                   break;
            }
            if (i==symbolIndex)
            {
                gSubscriptionList[symbolIndex].symbol = (char*)calloc (temp-source+1,
                                                       sizeof (char));
                strncpy (gSubscriptionList[symbolIndex].symbol, source, temp-source);
                symbolIndex++;
            }
        }
    }
    gNumSymbols = symbolIndex;

    mamaPlaybackFileParser_closeFile(fileParser);
    mamaPlaybackFileParser_deallocate (fileParser);
}

static void parseCommandLine (int argc, const char** argv)
{
    int i = 0;
    for (i = 1; i < argc;)
    {
        if (strcmp (argv[i], "-S") == 0)
        {
            gPubSource = argv[i + 1];
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
        else if (strcmp ("-tport", argv[i]) == 0)
        {
            gPubTransportName = argv[i+1];
            i += 2;
        }
        else if (strcmp ("-m", argv[i]) == 0)
        {
            gPubMiddleware = argv[i+1];
            i += 2;
        }
        else if (strcmp ("-q", argv[i]) == 0)
        {
            gQuietness++;
            i++;
        }
        else if (strcmp (argv[i], "-i") == 0)
        {
            gTimeInterval = strtod (argv[i+1], NULL);
            i += 2;
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
