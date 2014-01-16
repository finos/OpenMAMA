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
#include "wombat/port.h"

static const char * gUsageString[] =
{
    " capturereplay - Sample application demonstrates how to publish mama messages, and",
    " respond to requests from a client inbox.",
    "",
    " It accepts the following command line arguments:",
    "      [-S source]       The source name to use for publisher Default is WOMBAT.",
    "      [-i interval]     The interval between messages .Default in  0.5.",
    "      [-f filename]     The capture filename",
    "      [-m middleware]   The middleware to use for publisher [wmw/lbm/tibrv]. Default is wmw.",
    "      [-tport name]     The transport parameters to be used from",
    "                        mama.properties. Default is pub",
    "      [-dictionary]     The dictionary file which is sent in response to client requests. Required.",
    "      [-q]              Quiet mode. Suppress output.", 
    "      [-rewind|-r]      Rewind symbols when they reach the end of the file.",
    "      [-v]              Increase logging verbosity.",
    NULL
};

#define MAX_SUBSCRIPTIONS 250000

typedef struct pubCache_
{
    char*                   symbol;
    mamaDQPublisher         pub;
    mamaMsg                 cachedMsg;
    mamaPlaybackFileParser  fileParser;
} pubCache;

static mamaBridge               gPubBridge              = NULL;
static const char*              gPubMiddleware          = "wmw";
static mamaQueue                gPubDefaultQueue        = NULL;
static mamaSubscription         gDictionarySubscription = NULL;
static mamaDictionary           gDictionary             = NULL;
static const char*              gDictionaryFile         = NULL;
static mamaMsg                  gDictionaryMessage      = NULL;
static mamaPublisher            gDictionaryPublisher    = NULL;
static const char*              gPubTransportName       = "pub";
static int                      gQuietness              = 0;
static const char*              gFilename               = NULL;
static const char*              gPubSource              = "WOMBAT";
static const char**             gSymbolList             = NULL;
static mamaTransport            gPubTransport           = NULL;
static mamaDQPublisherManager   gDQPubManager           = NULL;
static pubCache*                gSubscriptionList       = NULL;
static int                      gNumSymbols             = 0;
static MamaLogLevel             gSubscLogLevel          = MAMA_LOG_LEVEL_NORMAL;
static mamaTimer                gPubTimer               = NULL;
static double                   gTimeInterval           = 0.5;
static int                      gRewind                 = 0;
#define DELIM                   ':'

static void parseCommandLine (int argc, const char** argv);

static void MAMACALLTYPE 
subscriptionHandlerOnCreateCb (mamaDQPublisherManager manager);

static void MAMACALLTYPE 
subscriptionHandlerOnNewRequestCb(mamaDQPublisherManager manager,
                                  const char*            symbol,
                                  short                  subType,
                                  short                  msgType,
                                  mamaMsg                msg);
static void MAMACALLTYPE 
subscriptionHandlerOnRequestCb(mamaDQPublisherManager manager,
                               mamaPublishTopic*      info,
                               short                  subType,
                               short                  msgType,
                               mamaMsg                msg);
static void MAMACALLTYPE 
subscriptionHandlerOnRefreshCb(mamaDQPublisherManager manager,
                               mamaPublishTopic*      info,
                               short                  subType,
                               short                  msgType,
                               mamaMsg                msg);
static void MAMACALLTYPE 
subscriptionHandlerOnErrorCb(mamaDQPublisherManager manager,
                             mama_status            status,
                             const char*            errortxt,
                             mamaMsg                msg);

static void     createPublisher (void);
static void     initializeMama (void);
static void usage (int exitStatus);
static void readSymbolsFromFile (void);

/* Methods for managing dictionary requests: */
static void prepareDictionaryListener (void);

static void MAMACALLTYPE  dictionarySubOnCreate  (mamaSubscription subsc, void* closure);
static void MAMACALLTYPE  dictionarySubOnDestroy (mamaSubscription subsc, void* closure);

static void MAMACALLTYPE  dictionarySubOnError   (mamaSubscription subsc, 
                                    mama_status      status,
                                    void*            platformError, 
                                    const char*      subject,
                                    void*            closure);

static void MAMACALLTYPE dictionarySubOnMsg     (mamaSubscription subsc, 
                                    mamaMsg          msg,
                                    void*            closure, 
                                    void*            itemClosure);

static void MAMACALLTYPE pubCallback (mamaTimer timer, void* closure)
{
    int index =0;
    char*temp=NULL;
    char*source=NULL;
    char*           headerString = NULL;
    mamaMsg        newMessage;
    mama_status status = MAMA_STATUS_OK;

    for (index=0; index < gNumSymbols; index++)
    {
        if (gSubscriptionList[index].fileParser)
        {
            int header = 1;
            while ((header = mamaPlaybackFileParser_getNextHeader (
                    gSubscriptionList[index].fileParser, &headerString)))
            {
                if (strlen (headerString) == 0) continue;
                /*skip source and transport name*/
                temp = strchr (headerString,DELIM);
                temp++;
                source = strchr (temp,DELIM);
                source++; /*skip :*/

                temp = strchr (source, DELIM);

                if (mamaPlaybackFileParser_getNextMsg (
                        gSubscriptionList[index].fileParser, &newMessage))
                {
                    int symLength = temp - source;
                    if (0 == strncmp (gSubscriptionList[index].symbol,
                                        source,
                                        symLength)
                       &&
                        symLength == (strlen (gSubscriptionList[index].symbol))
                       )
                    {
                        mamaMsg_applyMsg (gSubscriptionList[index].cachedMsg,
                                          newMessage);

                        mama_log (MAMA_LOG_LEVEL_FINEST,
                                  "Publishing message: %s",
                                  mamaMsg_toString(newMessage));

                        mamaDQPublisher_send (gSubscriptionList[index].pub,
                                              newMessage);
                        break;
                    }
                }
            }
            if (gRewind && !header) 
            {
                mama_log (MAMA_LOG_LEVEL_FINE, 
                            "End of file reached for symbol %s - Rewinding.",
                            gSubscriptionList[index].symbol);
                status = mamaPlaybackFileParser_rewindFile(
                            gSubscriptionList[index].fileParser);

                if (MAMA_STATUS_OK != status)
                {
                  mama_log(MAMA_LOG_LEVEL_FINE, "Failed to rewind file %s",
                           mamaStatus_stringForStatus(status));
                }
            } else if (!header) {
                mama_log (MAMA_LOG_LEVEL_FINE, 
                            "End of file reached for symbol %s.", 
                            gSubscriptionList[index].symbol);
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

static void prepareDictionaryListener (void)
{
    mama_status      status                     = MAMA_STATUS_OK;
    mamaMsgCallbacks dictionarySubCallbacks;

    dictionarySubCallbacks.onCreate       = dictionarySubOnCreate;
    dictionarySubCallbacks.onDestroy      = dictionarySubOnDestroy;
    dictionarySubCallbacks.onError        = dictionarySubOnError;
    dictionarySubCallbacks.onMsg          = dictionarySubOnMsg;

    /* Basic subscription so these are set to NULL */
    dictionarySubCallbacks.onQuality      = NULL;
    dictionarySubCallbacks.onGap          = NULL;
    dictionarySubCallbacks.onRecapRequest = NULL;

    status = mamaSubscription_allocate (&gDictionarySubscription);
    if (MAMA_STATUS_OK != status) 
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "Could not allocate dictionary subscription.");
        exit (1);
    }

    status = mamaSubscription_createBasic (gDictionarySubscription, 
                                           gPubTransport,
                                           gPubDefaultQueue, 
                                           &dictionarySubCallbacks,
                                           "_MDDD.WOMBAT.DATA_DICT", 
                                           NULL);
    if (status != MAMA_STATUS_OK) 
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, 
                  "Could not create dictionary subscription");
        exit (1);
    }

    /* Create the Dictionary Object */
    mamaDictionary_create           (&gDictionary);
    mamaDictionary_populateFromFile (gDictionary, gDictionaryFile);

    /* Create Dictionary Response Message */
    status = mamaDictionary_getDictionaryMessage (gDictionary,
                                                  &gDictionaryMessage);
    if (MAMA_STATUS_OK != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "Failed to allocate dictionary message.");
        exit (1);
    }

    /* Build dictionary publisher */
    status = mamaPublisher_create (&gDictionaryPublisher,
                                   gPubTransport,
                                   "WOMBAT.DATA_DICT",
                                   NULL, 
                                   NULL);

    if (MAMA_STATUS_OK != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "Failed to create dictionary publisher.");
        exit (1);
    }

    status = mamaMsg_updateU8 (gDictionaryMessage,
                               MamaFieldMsgType.mName,
                               MamaFieldMsgType.mFid,
                               MAMA_MSG_TYPE_INITIAL);

    if (MAMA_STATUS_OK != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "Failed to update dictionary message.");
        exit (1);
    }
}

static void initializeMama ()
{
    mama_status status = MAMA_STATUS_OK;

    status = mama_loadBridge (&gPubBridge, gPubMiddleware);
    if (MAMA_STATUS_OK != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, 
                  "Could not load middleware bridge. Exiting.");
        exit (1);
    }

    status = mama_open ();
    if (MAMA_STATUS_OK != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "Failed to Open MAMA. Exiting");
        exit (1);
    }

    mama_getDefaultEventQueue (gPubBridge, &gPubDefaultQueue);

    mamaTransport_allocate (&gPubTransport);
    mamaTransport_create (gPubTransport,
                          gPubTransportName,
                          gPubBridge);

}

int main (int argc, const char **argv)
{

    gSymbolList = (const char**)calloc (MAX_SUBSCRIPTIONS, sizeof (char*));

    /* Enabling Normal MAMA Logging, to provide feedback to users regarding
     * processing.
     */
    mama_enableLogging (stderr, MAMA_LOG_LEVEL_NORMAL);
    parseCommandLine (argc, argv);

    mama_log (MAMA_LOG_LEVEL_NORMAL, 
              "Reading symbols from file, please standby.");

    initializeMama ();
    prepareDictionaryListener();

    readSymbolsFromFile();


    createPublisher ();

    start_timed_publish();

    mama_log (MAMA_LOG_LEVEL_NORMAL, "Ready for Subscriptions.");

    mama_start (gPubBridge);

    return 0;
}

static void MAMACALLTYPE 
subscriptionHandlerOnCreateCb (mamaDQPublisherManager manager)
{
    mama_log (MAMA_LOG_LEVEL_NORMAL, "Created publisher subscription.");
}

static void MAMACALLTYPE 
subscriptionHandlerOnErrorCb (mamaDQPublisherManager manager,
                              mama_status            status,
                              const char*            errortxt,
                              mamaMsg                msg)
{
    if (msg)
    {
        mama_log (MAMA_LOG_LEVEL_WARN, 
                  "Unhandled Msg: %s (%s) %s\n",
                  mamaStatus_stringForStatus (status), 
                  mamaMsg_toString (msg),
                  errortxt);
    }
    else
    {
        mama_log (MAMA_LOG_LEVEL_WARN, 
                  "Unhandled Msg: %s %s\n", 
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
       mama_log  (MAMA_LOG_LEVEL_WARN, 
                  "Received request for unknown symbol: %s", 
                  symbol);
        return;
    }

    mama_log (MAMA_LOG_LEVEL_NORMAL, 
              "Received new request: %s", 
              symbol);

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
            if (subType == MAMA_SUBSC_TYPE_BOOK)
            {
                mamaMsg_updateU8 (gSubscriptionList[index].cachedMsg, 
                                  NULL,
                                  MamaFieldMsgType.mFid, 
                                  MAMA_MSG_TYPE_BOOK_INITIAL);
            } else {
                mamaMsg_updateU8 (gSubscriptionList[index].cachedMsg, 
                                  NULL,
                                  MamaFieldMsgType.mFid, 
                                  MAMA_MSG_TYPE_INITIAL);
            }
            mamaDQPublisher_sendReply (gSubscriptionList[index].pub, msg,
                                       gSubscriptionList[index].cachedMsg);
            break;
        default:
            mama_log (MAMA_LOG_LEVEL_NORMAL, "Publishing MAMA_MSG_TYPE_RECAP");
            mamaMsg_updateU8 (gSubscriptionList[index].cachedMsg, 
                              NULL,
                              MamaFieldMsgType.mFid, 
                              MAMA_MSG_TYPE_RECAP);

            mamaDQPublisher_send (gSubscriptionList[index].pub,
                                  gSubscriptionList[index].cachedMsg);
            break;
        }
    }

}



static void MAMACALLTYPE 
subscriptionHandlerOnRequestCb (mamaDQPublisherManager manager,
                                mamaPublishTopic*      publishTopicInfo,
                                short                  subType,
                                short                  msgType,
                                mamaMsg                msg)
{
    int index =0;

    mama_log (MAMA_LOG_LEVEL_NORMAL, 
              "Received request: %s", 
              publishTopicInfo->symbol);

    switch (msgType)
    {
    case MAMA_SUBSC_SUBSCRIBE:
    case MAMA_SUBSC_SNAPSHOT:
        index = (int) publishTopicInfo->cache;

        if (subType == MAMA_SUBSC_TYPE_BOOK) 
        {
            mamaMsg_updateU8 (gSubscriptionList[index].cachedMsg, 
                              NULL,
                              MamaFieldMsgType.mFid, 
                              MAMA_MSG_TYPE_BOOK_INITIAL);
        } else {
            mamaMsg_updateU8 (gSubscriptionList[index].cachedMsg, 
                              NULL,
                              MamaFieldMsgType.mFid, 
                              MAMA_MSG_TYPE_INITIAL);
        }
        mamaDQPublisher_sendReply (gSubscriptionList[index].pub, 
                                   msg,
                                   gSubscriptionList[index].cachedMsg);
        break;
    case MAMA_SUBSC_DQ_SUBSCRIBER:
    case MAMA_SUBSC_DQ_PUBLISHER:
    case MAMA_SUBSC_DQ_NETWORK:
    case MAMA_SUBSC_DQ_UNKNOWN:
    case MAMA_SUBSC_DQ_GROUP_SUBSCRIBER:
        index = (int) publishTopicInfo->cache;
        mamaMsg_updateU8 (gSubscriptionList[index].cachedMsg, 
                          NULL,
                          MamaFieldMsgType.mFid, 
                          MAMA_MSG_TYPE_RECAP);
        mamaDQPublisher_send (gSubscriptionList[index].pub,
                              gSubscriptionList[index].cachedMsg);
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
                                mamaPublishTopic*       publishTopicInfo,
                                short                   subType,
                                short                   msgType,
                                mamaMsg                 msg)
{
    mama_log (MAMA_LOG_LEVEL_NORMAL, 
              "Received Refresh: %s", 
              publishTopicInfo->symbol);
}

static void readSymbolsFromFile (void)
{
    mamaPlaybackFileParser  fileParser;
    char*                   headerString    = NULL;
    char*                   temp            = NULL;
    char*                   source          = NULL;
    int                     symbolIndex     = 0;
    int                     i               = 0;
    int                     iterations      = 0;
    mamaMsg                 newMessage;

    gSubscriptionList = (pubCache*)calloc (MAX_SUBSCRIPTIONS,
                                                   sizeof (pubCache));

    mamaPlaybackFileParser_allocate (&fileParser);
    mamaPlaybackFileParser_openFile(fileParser, (char*)gFilename);

    mama_log (MAMA_LOG_LEVEL_NORMAL, "Continuing.");
    while (mamaPlaybackFileParser_getNextHeader (fileParser, &headerString))
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

                if (0 == (symbolIndex % 20))
                {
                    mama_log (MAMA_LOG_LEVEL_NORMAL, 
                              "Read %d symbols from playback file.",
                              symbolIndex);
                    mama_log (MAMA_LOG_LEVEL_NORMAL,
                              "Continuing.");
                }
            }

            /*
             * Just some additional logging to help make it clear the application
             * hasn't frozen when processing large files.
             */
            iterations++;
            if (0 == (iterations % 5000))
            {
                printf (".");
                fflush (stdout);

                if (0 == (iterations % 50000))
                {
                    printf(".\n");
                    mama_log (MAMA_LOG_LEVEL_NORMAL, "Continuing.");
                }
            }
        }
    }
    gNumSymbols = symbolIndex;

    /* End logging. */
    mama_log (MAMA_LOG_LEVEL_NORMAL, 
              "Symbols read from playback file. Total symbols:\t%d", 
              gNumSymbols);

    mamaPlaybackFileParser_closeFile  (fileParser);
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
        else if (strcmp (argv[i], "-dictionary") == 0)
        {
            gDictionaryFile = argv[i + 1];
            i += 2;
        }
        else if (strcmp (argv[i], "-r") == 0 || strcmp (argv[i], "-rewind") == 0)
        {
            gRewind = 1;
            i++;
        }
        else if (strcmp (argv[i], "-v") == 0)
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

    /*
     * Dictionary is a required option. If not passed display a warning and
     * drop out.
     */
    if (NULL == gDictionaryFile)
    {
        printf ("\nWARNING: A dictionary file must be specified. Please pass a "
                "valid dictionary via the -dictionary command line option.\n\n");
        usage(1);
    }
}

static void usage (int exitStatus)
{
    int i = 0;
    while (gUsageString[i] != NULL )
    {
        printf ("%s\n", gUsageString[i++]);
    }
    exit (exitStatus);
}

static void MAMACALLTYPE dictionarySubOnCreate (mamaSubscription subsc, void* closure)
{
    mama_log (MAMA_LOG_LEVEL_NORMAL, "Dictionary Subscription Created:");
}

static void MAMACALLTYPE dictionarySubOnDestroy (mamaSubscription subsc, void* closure)
{
    mama_log (MAMA_LOG_LEVEL_NORMAL, "Dictionary Subscription Destroyed:");
}

static void MAMACALLTYPE dictionarySubOnError (mamaSubscription subsc, 
                                  mama_status      status,
                                  void*            platformError, 
                                  const char*      subject,
                                  void*            closure)
{
    mama_log (MAMA_LOG_LEVEL_NORMAL, "Dictionary Subscription Error:");
}

static void MAMACALLTYPE dictionarySubOnMsg (mamaSubscription subsc, 
                                mamaMsg          msg,
                                void*            closure, 
                                void*            itemClosure)
{
    mama_status status = MAMA_STATUS_OK;
    status = mamaPublisher_sendReplyToInbox (gDictionaryPublisher,
                                             msg,
                                             gDictionaryMessage);

    if (MAMA_STATUS_OK != status) 
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "Failed to send dictionary message.");
        exit (1);
    }
}

