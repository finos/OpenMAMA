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
#include "mama/dqpublisher.h"
#include "mama/dqpublishermanager.h"
#include "mama/log.h"
#include "mama/msg.h"
#include "mama/msgtype.h"
#include "mama/subscmsgtype.h"
#include "mama/subscription.h"
#include "playback/playbackFileParser.h"
#include "string.h"
#include "wombat/port.h"
#include "mama/mamautils.h"

static const char *gUsageString[] = {
    " capturereplay - Sample application demonstrates how to publish mama ",
    " messages, and respond to requests from a client inbox.",
    "",
    " It accepts the following command line arguments:",
    "      [-S source]       The source name to use for publisher Default is ",
    "                        WOMBAT.",
    "      [-s symbol file]  A file listing the symbols which capturereplay",
    "                        expects to publish. If not specified, capturereplay",
    "                        will parse a symbol list from the capture file.",
    "      [-i interval]     The interval between messages .Default in  0.5.",
    "      [-f filename]     The capture filename",
    "      [-m middleware]   The middleware to use for publisher",
    "                        Default is wmw.",
    "      [-tport name]     The transport parameters to be used from",
    "                        mama.properties. Default is pub",
    "      [-dictionary]     The dictionary file which is sent in response to ",
    "                        client requests. Required.",
    "      [-q]              Quiet mode. Suppress output.",
    "      [-rewind|-r]      Rewind symbols when they reach the end of the ",
    "                        file.",
    "      [-v]              Increase logging verbosity.",
    NULL};

#define MAX_SUBSCRIPTIONS 250000

typedef struct pubCache_ {
    char *                 symbol;
    mamaDQPublisher        pub;
    mamaMsg                cachedMsg;
    mamaPlaybackFileParser fileParser;
    int                    index;
} pubCache;

static mamaBridge             gPubBridge              = NULL;
static const char *           gPubMiddleware          = "wmw";
static mamaQueue              gPubDefaultQueue        = NULL;
static mamaSubscription       gDictionarySubscription = NULL;
static mamaDictionary         gDictionary             = NULL;
static const char *           gDictionaryFile         = NULL;
static mamaMsg                gDictionaryMessage      = NULL;
static mamaPublisher          gDictionaryPublisher    = NULL;
static const char *           gPubTransportName       = "pub";
static int                    gQuietness              = 0;
static const char *           gFilename               = NULL;
static const char *           gPubSource              = "WOMBAT";
static const char **          gSymbolList             = NULL;
static const char *           gSymbolListFile         = NULL;
static mamaTransport          gPubTransport           = NULL;
static mamaDQPublisherManager gDQPubManager           = NULL;
static pubCache *             gSubscriptionList       = NULL;
static int                    gNumSymbols             = 0;
static MamaLogLevel           gSubscLogLevel          = MAMA_LOG_LEVEL_NORMAL;
static mamaTimer              gPubTimer               = NULL;
static double                 gTimeInterval           = 0.5;
static int                    gRewind                 = 0;

/* Set the header delimiter */
#define DELIM ':'

static void parseCommandLine (int argc, const char **argv);

static void MAMACALLTYPE
subscriptionHandlerOnCreateCb (mamaDQPublisherManager manager);
static void MAMACALLTYPE
subscriptionHandlerOnNewRequestCb (mamaDQPublisherManager manager,
                                   const char *           symbol,
                                   short                  subType,
                                   short                  msgType,
                                   mamaMsg                msg);
static void MAMACALLTYPE
subscriptionHandlerOnRequestCb (mamaDQPublisherManager manager,
                                mamaPublishTopic *     info,
                                short                  subType,
                                short                  msgType,
                                mamaMsg                msg);
static void MAMACALLTYPE
subscriptionHandlerOnRefreshCb (mamaDQPublisherManager manager,
                                mamaPublishTopic *     info,
                                short                  subType,
                                short                  msgType,
                                mamaMsg                msg);
static void MAMACALLTYPE
subscriptionHandlerOnErrorCb (mamaDQPublisherManager manager,
                              mama_status            status,
                              const char *           errortxt,
                              mamaMsg                msg);

static void createDQPublisherManager (void);
static void initializeMama (void);
static void usage (int exitStatus);
static void prepareSymbolList (void);
static void readSymbolsFromCaptureFile (void);
static void readSymbolsFromSymbolFile (void);

/* Methods for managing dictionary requests: */
static void prepareDictionaryListener (void);

static void MAMACALLTYPE dictionarySubOnCreate (mamaSubscription subsc,
                                                void *           closure);
static void MAMACALLTYPE dictionarySubOnDestroy (mamaSubscription subsc,
                                                 void *           closure);
static void MAMACALLTYPE dictionarySubOnError (mamaSubscription subsc,
                                               mama_status      status,
                                               void *           platformError,
                                               const char *     subject,
                                               void *           closure);
static void MAMACALLTYPE dictionarySubOnMsg (mamaSubscription subsc,
                                             mamaMsg          msg,
                                             void *           closure,
                                             void *           itemClosure);

static void MAMACALLTYPE pubCallback (mamaTimer timer, void *closure)
{
    int         index        = 0;
    char *      temp         = NULL;
    char *      source       = NULL;
    char *      headerString = NULL;
    mamaMsg     newMessage;
    mama_status status = MAMA_STATUS_OK;

    /* Iterate each instrument in the instrument list (loaded from the playback
     * file), check if it has an open playback handle, then walk through the 
     * file until you find a message. Cache it (apply), then publish the 
     * message.
     */
    for (index = 0; index < gNumSymbols; index++) {
        if (gSubscriptionList[index].fileParser) {
            int header = 1;

            /* Continue while there is another header in the playback file, and
             * we haven't already published one.
             */
            while ((header = mamaPlaybackFileParser_getNextHeader (
                        gSubscriptionList[index].fileParser, &headerString))) {
                if (strlen (headerString) == 0)
                    continue;
                /* Each message recorded in the playback file has a header, with the 
                 * following format: "SOURCE:TPORT:SYMBOL:MSGLEN."
                 * In this case, skip the source and the transport, storing the start
                 * of the instrument in 'source' and the end in 'temp'.
                 */
                temp = strchr (headerString, DELIM);
                temp++;
                source = strchr (temp, DELIM);
                source++;
                temp = strchr (source, DELIM);

                if (mamaPlaybackFileParser_getNextMsg (
                        gSubscriptionList[index].fileParser, &newMessage)) {
                    
                    /* Store the length of the symbol from the header. */
                    int symLength = temp - source;

                    /* Check if the symbol matches the symbol in the list, and
                     * that it isn't a sub-string. 
                     */
                    if (0 == strncmp (gSubscriptionList[index].symbol,
                                      source,
                                      symLength) &&
                        symLength ==
                            (strlen (gSubscriptionList[index].symbol))) {

                        /* Apply the message to the cache (really simple
                         * version) 
                         */
                        status = mamaMsg_applyMsg (
                            gSubscriptionList[index].cachedMsg, newMessage);

                        if (MAMA_STATUS_OK != status) {
                            mama_log (MAMA_LOG_LEVEL_WARN,
                                      "pubCallback (): Failed to apply message "
                                      "to cache for [%s]. Continuing.",
                                      gSubscriptionList[index].symbol);
                        }

                        mama_log (MAMA_LOG_LEVEL_FINEST,
                                  "pubCallback (): Publishing message: %s",
                                  mamaMsg_toString (newMessage));

                        /* Send the message via the publisher. */
                        status = mamaDQPublisher_send (
                            gSubscriptionList[index].pub, newMessage);
                        if (MAMA_STATUS_OK != status) {
                            mama_log (MAMA_LOG_LEVEL_WARN,
                                      "pubCallback (): Failed to publish "
                                      "message for [%s]. Continuing.",
                                      gSubscriptionList[index].symbol);
                        }
                        break;
                    }
                }
            }

            /* If the rewind option has been set, and there is no new header to
             * to access, log the end of the file, and begin rewinding. Otherwise
             * log the end of the file for this instrument.
             */
            if (gRewind && !header) {
                mama_log (MAMA_LOG_LEVEL_FINE,
                          "pubCallback (): End of file reached for symbol %s - Rewinding.",
                          gSubscriptionList[index].symbol);
                status = mamaPlaybackFileParser_rewindFile (
                    gSubscriptionList[index].fileParser);

                if (MAMA_STATUS_OK != status) {
                    mama_log (MAMA_LOG_LEVEL_FINE,
                              "pubCallback (): Failed to rewind file %s",
                              mamaStatus_stringForStatus (status));
                }
            } else if (!header) {
                mama_log (MAMA_LOG_LEVEL_FINE,
                          "pubCalback (): End of file reached for symbol %s.",
                          gSubscriptionList[index].symbol);
            }
        }
    }
}

static void createDQPublisherManager ( )
{
    mama_status                     status = MAMA_STATUS_OK;
    mamaDQPublisherManagerCallbacks managerCallback;

    managerCallback.onCreate     = subscriptionHandlerOnCreateCb;
    managerCallback.onError      = subscriptionHandlerOnErrorCb;
    managerCallback.onNewRequest = subscriptionHandlerOnNewRequestCb;
    managerCallback.onRequest    = subscriptionHandlerOnRequestCb;
    managerCallback.onRefresh    = subscriptionHandlerOnRefreshCb;

    /* Seed the random number generator with the current time offset. */
    srand (time (NULL));

    /* Allocate a DQ Publisher Manager */
    status = mamaDQPublisherManager_allocate (&gDQPubManager);
    if (MAMA_STATUS_OK != status) {
        mama_log (MAMA_LOG_LEVEL_ERROR, "createDQPublisherManager: Failed to allocate "
                                        "the DQ Publisher Manager. Exiting.");
        exit (1);
    }

    /* Create the DQ Publisher Manager */
    status = mamaDQPublisherManager_create (gDQPubManager,
                                            gPubTransport,
                                            gPubDefaultQueue,
                                            &managerCallback,
                                            gPubSource,
                                            "_MD",
                                            NULL);
    if (MAMA_STATUS_OK != status) {
        /* Note: At present the mamaDQPublisherManager_create method *only*
         * returns MAMA_STATUS_OK, even if it may have failed. However,
         * we're adding the checks here so if the publisher code gets tidied
         * up we're ready for it.
         */
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "Failed to create the DQ Publisher Manager. Exiting.");
        exit (1);
    }
}

/* Start a timed publish of messages. At each point the timer is triggered, the
 * publisher callback is fired. 
 */
static void start_timed_publish (void)
{
    mamaTimer_create (
        &gPubTimer, gPubDefaultQueue, pubCallback, gTimeInterval, NULL);
}

/* Load the MAMA Data Dictionary, create the dictionary topic subscriber,
 * prepare the data dictionary message, and create a dictionary publisher.
 * 
 * TODO: Determine whether these functions are generic enough that they
 * could make useful standalone components. 
 */
static void prepareDictionaryListener (void)
{
    mama_status      status = MAMA_STATUS_OK;
    mamaMsgCallbacks dictionarySubCallbacks;

    dictionarySubCallbacks.onCreate  = dictionarySubOnCreate;
    dictionarySubCallbacks.onDestroy = dictionarySubOnDestroy;
    dictionarySubCallbacks.onError   = dictionarySubOnError;
    dictionarySubCallbacks.onMsg     = dictionarySubOnMsg;

    /* Basic subscription so these are set to NULL */
    dictionarySubCallbacks.onQuality      = NULL;
    dictionarySubCallbacks.onGap          = NULL;
    dictionarySubCallbacks.onRecapRequest = NULL;

    status = mamaSubscription_allocate (&gDictionarySubscription);
    if (MAMA_STATUS_OK != status) {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "Could not allocate dictionary subscription.");
        exit (1);
    }

    /* MAMA dictionary responders are effectively listeners to a
     * well know topic, "_MDDD.WOMBAT.DATA_DICT". Therefore we create
     * a new subscription on that topic, listening for new requests.
     */
    status = mamaSubscription_createBasic (gDictionarySubscription,
                                           gPubTransport,
                                           gPubDefaultQueue,
                                           &dictionarySubCallbacks,
                                           "_MDDD.WOMBAT.DATA_DICT",
                                           NULL);
    if (MAMA_STATUS_OK != status) {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "Could not create dictionary subscription");
        exit (1);
    }

    /* Create the Dictionary Object */
    status = mamaDictionary_create (&gDictionary);
    if (MAMA_STATUS_OK != status) {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "Failed to create the MAMA Dictionary. Exiting.");
        exit (1);
    }

    /* And populate it from a file. */
    status = mamaDictionary_populateFromFile (gDictionary, gDictionaryFile);
    if (MAMA_STATUS_OK != status) {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "Failed to populate the MAMA dictionary from"
                  " [%s]. Exiting.",
                  gDictionaryFile);
        exit (1);
    }

    /* Create Dictionary Response Message */
    status =
        mamaDictionary_getDictionaryMessage (gDictionary, &gDictionaryMessage);
    if (MAMA_STATUS_OK != status) {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "Failed to allocate dictionary message.");
        exit (1);
    }

    /* Build dictionary publisher */
    status = mamaPublisher_create (
        &gDictionaryPublisher, gPubTransport, "WOMBAT.DATA_DICT", NULL, NULL);

    if (MAMA_STATUS_OK != status) {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "Failed to create dictionary publisher.");
        exit (1);
    }

    /* Update the message type to 'INITIAL' */
    status = mamaMsg_updateU8 (gDictionaryMessage,
                               MamaFieldMsgType.mName,
                               MamaFieldMsgType.mFid,
                               MAMA_MSG_TYPE_INITIAL);

    if (MAMA_STATUS_OK != status) {
        mama_log (MAMA_LOG_LEVEL_ERROR, "Failed to update dictionary message.");
        exit (1);
    }
}

/* Initialise the MAMA Libraries, loading the required publisher
 * middleware bridge, opening MAMA, getting the default event queue,
 * and setting up the publisher transport to be used from now on. 
 */
static void initializeMama ( )
{
    mama_status status = MAMA_STATUS_OK;

    status = mama_loadBridge (&gPubBridge, gPubMiddleware);
    if (MAMA_STATUS_OK != status) {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "Could not load middleware bridge. Exiting.");
        exit (1);
    }

    status = mama_open ( );
    if (MAMA_STATUS_OK != status) {
        mama_log (MAMA_LOG_LEVEL_ERROR, "Failed to Open MAMA. Exiting");
        exit (1);
    }

    /* Current architecture uses a single event queue for all publishing.
     * Depending on performance considerations we may want to modify
     * this in the future.
     */
    status = mama_getDefaultEventQueue (gPubBridge, &gPubDefaultQueue);
    if (MAMA_STATUS_OK != status) {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "Failed to get the default event queue. Exiting.");
        exit (1);
    }

    /* Allocate a single publisher transport */
    status = mamaTransport_allocate (&gPubTransport);
    if (MAMA_STATUS_OK != status) {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "Failed to allocate the replay transport. Exiting.");
        exit (1);
    }

    /* Create the transport with the configured name (default: pub) */
    status =
        mamaTransport_create (gPubTransport, gPubTransportName, gPubBridge);
    if (MAMA_STATUS_OK != status) {
        mama_log (MAMA_LOG_LEVEL_ERROR, "Failed to create transport. Exiting");
        exit (1);
    }
}

int main (int argc, const char **argv)
{
    /* Enabling 'Normal' MAMA Logging, to provide feedback to users regarding
     * processing.
     */
    mama_enableLogging (stderr, MAMA_LOG_LEVEL_NORMAL);
    parseCommandLine (argc, argv);

    mama_log (MAMA_LOG_LEVEL_NORMAL,
              "Reading symbols from file, please standby.");

    /* Setup standard MAMA stuff. */
    initializeMama ( );

    /* Create the Dictionary listener. */
    prepareDictionaryListener ( );

    /* Populate the symbol list */
    prepareSymbolList ( );

    /* Create the publisher manager. */
    createDQPublisherManager ( );

    /* Begin publishing, every 'gTimeInterval'' */
    start_timed_publish ( );

    mama_log (MAMA_LOG_LEVEL_NORMAL, "Ready for Subscriptions.");

    /* Start MAMA in the foreground */
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
                              const char *           errortxt,
                              mamaMsg                msg)
{
    if (msg) {
        mama_log (MAMA_LOG_LEVEL_WARN,
                  "Unhandled Msg: %s (%s) %s\n",
                  mamaStatus_stringForStatus (status),
                  mamaMsg_toString (msg),
                  errortxt);
    } else {
        mama_log (MAMA_LOG_LEVEL_WARN,
                  "Unhandled Msg: %s %s\n",
                  mamaStatus_stringForStatus (status),
                  errortxt);
    }
}

/* Callback method used to handle requests for new instruments */
static void MAMACALLTYPE
subscriptionHandlerOnNewRequestCb (mamaDQPublisherManager manager,
                                   const char *           symbol,
                                   short                  subType,
                                   short                  msgType,
                                   mamaMsg                msg)
{
    mama_status status = MAMA_STATUS_OK;
    int         index  = 0;
    char *      headerString;
    char *      temp;
    char *      source;
    mamaMsg     newMessage;

    /* Check the existing symbol list for instruments matching the
     * new symbol name.
     */
    for (index = 0; index < gNumSymbols; index++) {
        if (strcmp (gSubscriptionList[index].symbol, symbol) == 0)
            break;
    }

    /* If the symbol does not match the symbol list, log a warning and return */
    if (index == gNumSymbols) {
        mama_log (MAMA_LOG_LEVEL_WARN,
                  "Received request for unknown symbol: %s",
                  symbol);
        return;
    }

    mama_log (MAMA_LOG_LEVEL_NORMAL, "Received new request: %s", symbol);

    /* Create a new DQ Publisher */
    status = mamaDQPublisherManager_createPublisher (
        manager,
        symbol,
        (void *)&gSubscriptionList[index],
        &gSubscriptionList[index].pub);
    if (MAMA_STATUS_OK != status) {
        mama_log (MAMA_LOG_LEVEL_WARN,
                  "subscriptionOnNewRequest (): Failed to create DQ Publisher "
                  "for [%s]",
                  symbol);
        return;
    }

    /* Allocate a new playback file parser for the new instrument. */
    status =
        mamaPlaybackFileParser_allocate (&gSubscriptionList[index].fileParser);
    if (MAMA_STATUS_OK != status) {
        mama_log (MAMA_LOG_LEVEL_WARN,
                  "subscriptionOnNewRequest (): Failed to allocate playback "
                  "file parser for [%s]",
                  symbol);
        return;
    }

    /* Open the playback file for the instrument */
    status = mamaPlaybackFileParser_openFile (
        gSubscriptionList[index].fileParser, (char *)gFilename);
    if (MAMA_STATUS_OK != status) {
        mama_log (MAMA_LOG_LEVEL_WARN,
                  "subscriptionOnNewRequest (): Failed to open playback file "
                  "[%s] for symbol [%s]",
                  gFilename,
                  symbol);
        return;
    }

    status = mamaMsg_create (&gSubscriptionList[index].cachedMsg);
    if (MAMA_STATUS_OK != status) {
        mama_log (MAMA_LOG_LEVEL_WARN,
                  "subscriptionOnNewRequest (): Failed to create cached MAMA "
                  "message for [%s]",
                  symbol);
        return;
    }

    /* Iterate messages in the playback file, looking for a message from this
     * instrument. 
     */
    while (mamaPlaybackFileParser_getNextHeader (
        gSubscriptionList[index].fileParser, &headerString)) {

        /* Each message recorded in the playback file has a header, with the 
         * following format: "SOURCE:TPORT:SYMBOL:MSGLEN."
         * In this case, skip the source and the transport, storing the start
         * of the instrument in 'source' and the end in 'temp'.
         */
        temp = strchr (headerString, DELIM);
        temp++;
        source = strchr (temp, DELIM);
        source++; 
        temp = strchr (source, DELIM);

        /* Check if the instrument in the header matches the symbol requested.
         * If so, break out of the loop. 
         */
        if ((strncmp (gSubscriptionList[index].symbol, source, temp - source) ==
             0) &&
            (strlen (gSubscriptionList[index].symbol) == temp - source))
            break;

        mamaPlaybackFileParser_getNextMsg (gSubscriptionList[index].fileParser,
                                           &newMessage);
    }

    /* Process the next message */
    if (mamaPlaybackFileParser_getNextMsg (gSubscriptionList[index].fileParser,
                                           &newMessage)) {

        /* Apply the most recent message to the cached message for the instrument */
        mamaMsg_applyMsg (gSubscriptionList[index].cachedMsg, newMessage);

        switch (msgType) {
        
        /* If the subscription type is a regular or snapshot subscription, send
         * a point-to-point response in the form of the cached message.
         */ 
        case MAMA_SUBSC_SUBSCRIBE:
        case MAMA_SUBSC_SNAPSHOT:
            if (subType == MAMA_SUBSC_TYPE_BOOK) {
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

        /* Otherwise we have a RECAP request, so we send a broadcast response
         * to all consuming applications.
         */
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

/* Callback to handle requests for instruments which the DQ Publisher manager
 * has already seen.
 */
static void MAMACALLTYPE
subscriptionHandlerOnRequestCb (mamaDQPublisherManager manager,
                                mamaPublishTopic *     publishTopicInfo,
                                short                  subType,
                                short                  msgType,
                                mamaMsg                msg)
{
    int index = 0;

    mama_log (MAMA_LOG_LEVEL_NORMAL,
              "Received request: %s",
              publishTopicInfo->symbol);

    switch (msgType) {
    /* For regular or snapshot subscriptions, send a point to point response */
    case MAMA_SUBSC_SUBSCRIBE:
    case MAMA_SUBSC_SNAPSHOT:
        index = ((pubCache *)publishTopicInfo->cache)->index;

        if (subType == MAMA_SUBSC_TYPE_BOOK) {
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
    
    /* For data quality events (where the subscriber has discovered an event) 
     * send a general recap to all consumers.
     */
    case MAMA_SUBSC_DQ_SUBSCRIBER:
    case MAMA_SUBSC_DQ_PUBLISHER:
    case MAMA_SUBSC_DQ_NETWORK:
    case MAMA_SUBSC_DQ_UNKNOWN:
    case MAMA_SUBSC_DQ_GROUP_SUBSCRIBER:
        index = (int)((pubCache *)publishTopicInfo->cache)->index;
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

/* Handle instrument refreshes. */
static void MAMACALLTYPE
subscriptionHandlerOnRefreshCb (mamaDQPublisherManager publisherManager,
                                mamaPublishTopic *     publishTopicInfo,
                                short                  subType,
                                short                  msgType,
                                mamaMsg                msg)
{
    mama_log (MAMA_LOG_LEVEL_NORMAL,
              "Received Refresh: %s",
              publishTopicInfo->symbol);
}

static void prepareSymbolList (void)
{
    /* Allocate a symbol list, up to a maximum size of 'MAX_SUBSCRIPTIONS' */
    gSymbolList = (const char **)calloc (MAX_SUBSCRIPTIONS, sizeof (char *));

    /* Populate the 'symbols' array with instruments which capturereplay
     * is going to allow subsription to.
     * This can be generated either from the capture file being loaded,
     * or from a MAMA symbol list file if one is supplied.
     */
    gSubscriptionList = (pubCache *)calloc (MAX_SUBSCRIPTIONS, sizeof (pubCache));
    if (gSymbolListFile) {
        readSymbolsFromSymbolFile ();
    }

    /* If the symbol list file is not supplied, or the symbol count as a
     * result is '0', load from the capture file itself.
     */
    if (!gSymbolListFile || 0 == gNumSymbols) {
        /* Read all the symbols available in the playback file, storing each
         * in the 'symbols' array.
         */
        readSymbolsFromCaptureFile ( );
    }
}

static void readSymbolsFromSymbolFile (void)
{
    mama_size_t l1SymbolCount = 0;
    mama_size_t l2SymbolCount = 0;
    size_t      symbolLen     = 0;
    size_t      i             = 0;
    mama_status status        = MAMA_STATUS_OK;

    /* Use the MAMA Utility function to read symbols from a file. 
     * We don't expect to use the L2 symbol functionality, so we
     * don't pass data into it.
     */
    status = mamaUtils_readSymbolFile (gSymbolListFile,
                                       gSymbolList,
                                       MAX_SUBSCRIPTIONS,
                                       &l1SymbolCount,
                                       NULL,
                                       0,
                                       NULL);
    
    /* If the number of symbols returned is 0, or if there's been a problem
     * drop out now so we can use the capture file. 
     */
    if (0 == l1SymbolCount || MAMA_STATUS_OK != status) {
        mama_log (MAMA_LOG_LEVEL_WARN,
                  "Failed to load symbols from symbol "
                  "file [%s], falling back to loading "
                  "from capture file.");
        
        /* Set num symbols to zero to force the capture load */
        gNumSymbols = 0;
    } else {
        /* Populate the pubcache list with the symbol list information */
        for (i = 0; i < l1SymbolCount; i++) {
            printf ("Count %d, symbol %s/n", i, gSymbolList[i]);
            symbolLen = strlen (gSymbolList[i]);

            /* Allocate memory for the symbol name, then copy across. */
            gSubscriptionList[i].symbol =
                (char *)calloc (symbolLen, sizeof (char));
            strncpy (gSubscriptionList[i].symbol, gSymbolList[i], symbolLen);
            gSubscriptionList[i].index = i;
        }

        /* Set the total number of instruments available */
        gNumSymbols = l1SymbolCount;

        mama_log (MAMA_LOG_LEVEL_NORMAL,
                  "Symbols read from symbol file. Total symbols:\t%d",
                  gNumSymbols);
    }
}

static void readSymbolsFromCaptureFile (void)
{
    mama_status            status       = MAMA_STATUS_OK;
    mamaPlaybackFileParser fileParser;
    char *                 headerString = NULL;
    char *                 temp         = NULL;
    char *                 source       = NULL;
    int                    symbolIndex  = 0;
    int                    i            = 0;
    int                    iterations   = 0;
    mamaMsg                newMessage;

    /* Allocate a Playback file parser, and open the file. */
    status = mamaPlaybackFileParser_allocate (&fileParser);
    if (MAMA_STATUS_OK != status) {
        mama_log (MAMA_LOG_LEVEL_ERROR, "Failed to allocate memory for MAMA "
                                        "playback file parser. Exiting.");
        exit (1);
    }

    status = mamaPlaybackFileParser_openFile (fileParser, (char *)gFilename);
    if (MAMA_STATUS_OK != status) {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "Failed to open MAMA Playback file [%s]. Exiting.",
                  gFilename);
        exit (1);
    }

    /* Begin iterating the playback file. 
     * 
     * Each message recorded in the playback file has a header, with the 
     * following format: "SOURCE:TPORT:SYMBOL:MSGLEN."
     */
    mama_log (MAMA_LOG_LEVEL_NORMAL, "Continuing.");
    while (mamaPlaybackFileParser_getNextHeader (fileParser, &headerString)) {
        if (mamaPlaybackFileParser_getNextMsg (fileParser, &newMessage)) {
            /* Skip the source, then the transport, storing the start of
             * the symbol in 'source' and the end in 'temp'.
             */
            temp = strchr (headerString, DELIM);
            temp++;
            source = strchr (temp, DELIM);
            source++;
            temp = strchr (source, DELIM);

            /* Iterate the symbols in the gSubscriptionList, comparing the
             * symbol name to the name in the header. If the name matches, 
             * break (we already have it stored).
             */
            for (i = 0; i < symbolIndex; i++) {
                int res = strncmp (
                    gSubscriptionList[i].symbol, source, temp - source);

                if ((res == 0) &&
                    (strlen (gSubscriptionList[i].symbol) == temp - source))
                    break;
            }

            /* If we've reached the end of the current symbol list
             * (at symbolIndex), copy the symbol across, set the index (why?)
             * increment the max index. 
             */
            if (i == symbolIndex) {
                gSubscriptionList[symbolIndex].symbol =
                    (char *)calloc (temp - source + 1, sizeof (char));
                gSubscriptionList[symbolIndex].index = symbolIndex;
                strncpy (gSubscriptionList[symbolIndex].symbol,
                         source,
                         temp - source);
                symbolIndex++;

                if (0 == (symbolIndex % 20)) {
                    mama_log (MAMA_LOG_LEVEL_NORMAL,
                              "Read %d symbols from playback file.",
                              symbolIndex);
                    mama_log (MAMA_LOG_LEVEL_NORMAL, "Continuing.");
                }
            }

            /*
             * Just some additional logging to help make it clear the
             * application hasn't frozen when processing large files.
             */
            iterations++;
            if (0 == (iterations % 5000)) {
                printf (".");
                fflush (stdout);

                if (0 == (iterations % 50000)) {
                    printf (".\n");
                    mama_log (MAMA_LOG_LEVEL_NORMAL, "Continuing.");
                }
            }
        }
    }

    /* Update the total number of symbols. */
    gNumSymbols = symbolIndex;

    /* End logging. */
    mama_log (MAMA_LOG_LEVEL_NORMAL,
              "Symbols read from playback file. Total symbols:\t%d",
              gNumSymbols);

    /* Close the playback file parser, and deallocate. */
    status = mamaPlaybackFileParser_closeFile (fileParser);
    if (MAMA_STATUS_OK != status) {
        mama_log (
            MAMA_LOG_LEVEL_ERROR,
            "readSymbolsFromCaptureFile (): Failed to close playback file. Exiting.");
        exit (1);
    }

    status = mamaPlaybackFileParser_deallocate (fileParser);
    if (MAMA_STATUS_OK != status) {
        mama_log (MAMA_LOG_LEVEL_ERROR, "readSymbolsFromCaptureFile (): Failed to "
                                        "deallocate playback file parser "
                                        "memory. Exiting.");
        exit (1);
    }
}

/* Parse the command line arguments used by capturereplay */
static void parseCommandLine (int argc, const char **argv)
{
    int i = 0;
    for (i = 1; i < argc;) {
        /* Publisher Source */
        if (strcmp (argv[i], "-S") == 0) {
            gPubSource = argv[i + 1];
            i += 2;

        /* Playback file to load */
        } else if (strcmp (argv[i], "-f") == 0) {
            gFilename = argv[i + 1];
            i += 2;

        /* Symbol list file to load. */
        } else if (strcmp (argv[i], "-s") == 0) {
            gSymbolListFile = argv[i+1];
            i += 2;

        /* Print help text and exit */
        } else if ((strcmp (argv[i], "-h") == 0) ||
                   (strcmp (argv[i], "-?") == 0)) {
            usage (0);
            i++;
        
        /* MAMA transport to use for publishing playback data */
        } else if (strcmp ("-tport", argv[i]) == 0) {
            gPubTransportName = argv[i + 1];
            i += 2;
        
        /* Middleware to use for publishing playback data. */
        } else if (strcmp ("-m", argv[i]) == 0) {
            gPubMiddleware = argv[i + 1];
            i += 2;
        
        /* Increase 'quietness' */
        } else if (strcmp ("-q", argv[i]) == 0) {
            gQuietness++;
            i++;
        
        /* Interval on which to publish playback data */
        } else if (strcmp (argv[i], "-i") == 0) {
            gTimeInterval = strtod (argv[i + 1], NULL);
            i += 2;
        
        /* Dictionary file to load, used to service dictionary requests. */
        } else if (strcmp (argv[i], "-dictionary") == 0) {
            gDictionaryFile = argv[i + 1];
            i += 2;
        
        /* Rewind the playback file when the playback completes */
        } else if (strcmp (argv[i], "-r") == 0 ||
                   strcmp (argv[i], "-rewind") == 0) {
            gRewind = 1;
            i++;
        
        /* Set the verbosity of the MAMA logging. */
        } else if (strcmp (argv[i], "-v") == 0) {
            if (gSubscLogLevel == MAMA_LOG_LEVEL_WARN) {
                gSubscLogLevel = MAMA_LOG_LEVEL_NORMAL;
                mama_enableLogging (stderr, MAMA_LOG_LEVEL_NORMAL);
            } else if (gSubscLogLevel == MAMA_LOG_LEVEL_NORMAL) {
                gSubscLogLevel = MAMA_LOG_LEVEL_FINE;
                mama_enableLogging (stderr, MAMA_LOG_LEVEL_FINE);
            } else if (gSubscLogLevel == MAMA_LOG_LEVEL_FINE) {
                gSubscLogLevel = MAMA_LOG_LEVEL_FINER;
                mama_enableLogging (stderr, MAMA_LOG_LEVEL_FINER);
            } else {
                gSubscLogLevel = MAMA_LOG_LEVEL_FINEST;
                mama_enableLogging (stderr, MAMA_LOG_LEVEL_FINEST);
            }
            i++;
        } else {
            printf ("Unknown arg %s\n", argv[i]);
            usage (0);
        }
    }

    /*
     * Dictionary is a required option. If not passed display a warning and
     * drop out.
     */
    if (NULL == gDictionaryFile) {
        printf (
            "\nWARNING: A dictionary file must be specified. Please pass a "
            "valid dictionary via the -dictionary command line option.\n\n");
        usage (1);
    }
}

static void usage (int exitStatus)
{
    int i = 0;
    while (gUsageString[i] != NULL) {
        printf ("%s\n", gUsageString[i++]);
    }
    exit (exitStatus);
}

static void MAMACALLTYPE dictionarySubOnCreate (mamaSubscription subsc,
                                                void *           closure)
{
    mama_log (MAMA_LOG_LEVEL_NORMAL, "Dictionary Subscription Created:");
}

static void MAMACALLTYPE dictionarySubOnDestroy (mamaSubscription subsc,
                                                 void *           closure)
{
    mama_log (MAMA_LOG_LEVEL_NORMAL, "Dictionary Subscription Destroyed:");
}

static void MAMACALLTYPE dictionarySubOnError (mamaSubscription subsc,
                                               mama_status      status,
                                               void *           platformError,
                                               const char *     subject,
                                               void *           closure)
{
    mama_log (MAMA_LOG_LEVEL_NORMAL, "Dictionary Subscription Error:");
}

/* Handle dictionary subscription requests */
static void MAMACALLTYPE dictionarySubOnMsg (mamaSubscription subsc,
                                             mamaMsg          msg,
                                             void *           closure,
                                             void *           itemClosure)
{
    mama_status status = MAMA_STATUS_OK;

    /* Take the existing dictionary publisher and dictionary message, and
     * send a point to point response.
     */
    status = mamaPublisher_sendReplyToInbox (
        gDictionaryPublisher, msg, gDictionaryMessage);

    if (MAMA_STATUS_OK != status) {
        mama_log (MAMA_LOG_LEVEL_ERROR, "Failed to send dictionary message.");
        exit (1);
    }
}
