/* $Id: mamasymbollistsubscriberc.c,v 1.1.2.2 2011/09/27 16:54:39 emmapollock Exp $
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
#include <string.h>
#include <mama/mama.h>
#include <mama/symbollist.h>
#include <mama/subscription.h>
#define MAX_SUBSCRIPTIONS 250000

static const char* gUsageString[] =
{
"This sample application subscribes to a symbol list and then makes individual",
"subscriptions to the symbols returned.",
"",
"Usage: mamasymbollistsubscriberc [options]",
"",
"Options:",
"",
"[-S, -source]      The symbol name space for the data.",
"[-m middleware]    The middleware to use [wmw/lbm/tibrv].",
"[-tport name]      The transport parameters to be used from",
"                   mama.properties.",
"[-d name]          The symbol name space for the data dictionary. Default:WOMBAT",
"[-dict_tport] name The name of the transport to use for obtaining the data",
"                   dictionary.",
"[-q]               Quiet mode. Suppress output.",
"[-v]               Increase the level of logging verbosity. Pass up to 4",
"                   times.",
"[-threads] n       The number of threads/queues from which to dispatch data.",
"                   The subscriptions will be created accross all queues in a",
"                   round robin fashion.",
"",
NULL
};

static mamaTransport    gTransport              = 0;
static const char*      gTransportName          = NULL;
static mamaTransport    gDictTransport          = 0;
static const char*      gDictTransportName      = NULL;

static mamaDictionary   gDictionary;
static const char*      gDictSymbolNamespace    = "WOMBAT";
static mamaSource       gDictSource             = NULL;
static int              gDictionaryComplete     = 0;


static const char*      gSymbolNamespace        = NULL;
static mamaSource       gSubscriptionSource     = NULL;

static const char*      gMiddleware             = "wmw";
static mamaBridge       gMamaBridge             = NULL;
static mamaQueue        gMamaDefaultQueue       = NULL;

static int              gQuietness              = 0;
static const char**     gSymbolList             = NULL;
static int              gSymbolListIndex        = 0;
static int              gThreads                = 0;
static mamaQueue*       gQueues                 = NULL;
static mamaDispatcher*  gDispatchers            = NULL;
static mamaSubscription* gSubscriptionList      = NULL;

static void parseCommandLine    (int argc, const char* argv[]);
static void initializeMama      (void);
static void buildDataDictionary (void);

static void MAMACALLTYPE
subscriptionOnMsg   (mamaSubscription    subscription,
                     mamaMsg             msg,
                     void*               closure,
                     void*               itemClosure);

static void MAMACALLTYPE
subscriptionOnQuality (mamaSubscription subsc,
                       mamaQuality      quality,
                       const char*      symbol,
                       short            cause,
                       const void*      platformInfo,
                       void*            closure);

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
symbolListSubscriptionOnMsg   (mamaSubscription    subscription,
                               mamaMsg             msg,
                               void*               closure,
                               void*               itemClosure);

static void MAMACALLTYPE
symbolListSubscriptionOnQuality (mamaSubscription subsc,
                       mamaQuality      quality,
                       const char*      symbol,
                       short            cause,
                       const void*      platformInfo,
                       void*            closure);

static void MAMACALLTYPE
symbolListSubscriptionOnError (mamaSubscription    subscription,
                     mama_status         status,
                     void*               platformError,
                     const char*         subject,
                     void*               closure);

static void MAMACALLTYPE
symbolListSubscriptionOnCreate (mamaSubscription    subscription,
                      void*               closure);

static void MAMACALLTYPE
timeoutCb                  (mamaDictionary      dict,
                            void *              closure);
static void MAMACALLTYPE
errorCb                    (mamaDictionary      dict,
                            const char *        errMsg,
                            void *              closure);

static void MAMACALLTYPE
completeCb                 (mamaDictionary      dict,
                            void *              closure);


void usage (int exitStatus);

int main (int argc, const char **argv)
{
    mama_status         status;
    mamaMsgCallbacks    symbolListCallbacks;
    mamaSubscription    symbolListSubscription;
    memset(&symbolListCallbacks, 0, sizeof(symbolListCallbacks));

    gSymbolList = (const char**) calloc (MAX_SUBSCRIPTIONS, sizeof (char*));
    gSubscriptionList = (mamaSubscription*)calloc (MAX_SUBSCRIPTIONS,
                                                   sizeof (mamaSubscription));


    memset(&symbolListCallbacks, 0, sizeof(symbolListCallbacks));
    symbolListCallbacks.onMsg          = symbolListSubscriptionOnMsg;
    symbolListCallbacks.onCreate       = symbolListSubscriptionOnCreate;
    symbolListCallbacks.onError        = symbolListSubscriptionOnError;
    symbolListCallbacks.onQuality      = symbolListSubscriptionOnQuality;
    symbolListCallbacks.onGap          = NULL;
    symbolListCallbacks.onRecapRequest = NULL;

    parseCommandLine (argc, argv);
    initializeMama ();
    buildDataDictionary ();

    /* Make the symbol list subscription */
    if (MAMA_STATUS_OK!=(status=mamaSubscription_allocate (&symbolListSubscription)))
    {
        printf ("Error allocating subscription: %s\n",
                mamaStatus_stringForStatus (status));
        exit (status);
    }

    mamaSubscription_setSubscriptionType (symbolListSubscription, MAMA_SUBSC_TYPE_SYMBOL_LIST);
    status = mamaSubscription_create (symbolListSubscription,
                                      gMamaDefaultQueue,
                                      &symbolListCallbacks,
                                      gSubscriptionSource,
                                      "Symbol List",
                                      NULL);

    if (status != MAMA_STATUS_OK)
    {
        fprintf (stdout,
                 "Error creating symbol list subscription.  STATUS: %s\n",
                 mamaStatus_stringForStatus (status));
    }
    else
    {
        printf ("Subscribed to symbol list.\n");
    }

    mama_start (gMamaBridge);

    return 0;
}

void MAMACALLTYPE
timeoutCb (mamaDictionary dict, void *closure)
{
    printf ("Timed out waiting for dictionary\n" );
    mama_stop(gMamaBridge);
}

void MAMACALLTYPE
errorCb (mamaDictionary dict, const char *errMsg, void *closure)
{
    fprintf (stderr, "Error getting dictionary: %s\n", errMsg );
    mama_stop(gMamaBridge);
}

void MAMACALLTYPE
completeCb (mamaDictionary dict, void *closure)
{
    gDictionaryComplete = 1;
    /*
     * Stop processing events until all subscriptions have been created.
     */
    mama_stop (gMamaBridge);
}

static void buildDataDictionary (void)
{
    mamaDictionaryCallbackSet dictionaryCallback;
    mama_status result  =   MAMA_STATUS_OK;

    memset(&dictionaryCallback, 0, sizeof(dictionaryCallback));
    dictionaryCallback.onComplete = completeCb;
    dictionaryCallback.onTimeout  = timeoutCb;
    dictionaryCallback.onError    = errorCb;

    /*
        The dictionary is created asychronously. The dictionary has been
        successfully retreived once the onComplete callback has been
        invoked.
     */
     result = mama_createDictionary (&gDictionary,
                                     gMamaDefaultQueue,/*Use the default event queue*/
                                     dictionaryCallback,
                                     gDictSource,
                                     10.0,
                                     3,
                                     NULL);/*Don't specify a closure*/

     if (result != MAMA_STATUS_OK)
     {
        fprintf (stderr, "Exception creating "
                         "dictionary: MamaStatus: %s\n",
                         mamaStatus_stringForStatus (result));
        exit (1);
     }

    /*
        Start dispatching on the default event queue. Dispatching on the
        queue is unblocked once one of the dictionary callbacks is invoked.
    */
    mama_start (gMamaBridge);

    /*
        True only if onComplete resulted in the unblocking of the queue
        dispatching
    */
    if (!gDictionaryComplete)
    {
        fprintf (stderr, "Could not create dictionary.\n" );
        exit(1);
    }
}

/* Steps required for initializing the API */
void initializeMama (void)
{
    mama_status status = MAMA_STATUS_OK;

    status = mama_loadBridge (&gMamaBridge, gMiddleware);

    if (MAMA_STATUS_OK!=status)
    {
        fprintf (stderr,"Could not open MAMA %s bridge.\n",
                                         gMiddleware);
        exit (1);
    }

    /*
        mama_open() should be the first MAMA API call made in an
        application, with the exception of mama_loadBridge,
        mama_setApplicationName(), mama_setApplicationClassName()
        and mama_setProperty().
        It is reference counted. mama_close() must be called a
        corresponding number of times.
    */
    if (MAMA_STATUS_OK!=(status=mama_open ()))
    {
        fprintf (stderr,
                 "Failed to initialize MAMA STATUS: %d (%s)\n", status,
                 mamaStatus_stringForStatus (status));
        exit(1);
    }

    /*Get the default event queue*/
    mama_getDefaultEventQueue (gMamaBridge, &gMamaDefaultQueue);

    /*Using a single transport for all subscriptions*/
    status = mamaTransport_allocate (&gTransport);

    if (status != MAMA_STATUS_OK)
    {
        fprintf (stderr,
                 "Failed to allocate transport STATUS: %d %s\n", status,
                 mamaStatus_stringForStatus (status));
        exit(1);
    }

    /* Create the transport after any properties have been set. */
    if (MAMA_STATUS_OK!=
       (status=mamaTransport_create (gTransport, gTransportName, gMamaBridge)))
    {
        fprintf (stderr,
                 "Failed to create transport STATUS: %d %s\n", status,
                 mamaStatus_stringForStatus (status));
        exit(1);
    }

    /*
        The mamaSource replaces the individual specifying of the
        symbolNameSpace and the transport. The mamaSource acts as a unique
        identifier for a source of data. Multiple subscriptions will typically
        be created using a single mamaSource instance.
    */

    /*The mamaSource used for all subscription creation*/
    if (MAMA_STATUS_OK!=(status=mamaSource_create (&gSubscriptionSource)))
    {
        fprintf (stderr,
                 "Failed to create subscription mamaSource STATUS: %d %s\n",
                 status, mamaStatus_stringForStatus (status));
        exit(1);
    }

    mamaSource_setId (gSubscriptionSource, "Subscription_Source");
    mamaSource_setTransport (gSubscriptionSource, gTransport);
    mamaSource_setSymbolNamespace (gSubscriptionSource, gSymbolNamespace);

    /*Specify a separate transport on which to retreive the dictioanry*/
    if (gDictTransportName != NULL)
    {
        fprintf (stderr,
                 "Creating data dictionary transport using name: %s\n",
                 gDictTransportName);
        if (strlen(gDictTransportName)==0) gDictTransportName = NULL;

        status = mamaTransport_allocate (&gDictTransport);
        status = mamaTransport_create (gDictTransport, gDictTransportName, gMamaBridge);
        if (status != MAMA_STATUS_OK)
        {
            fprintf (stderr,
                     "Failed to create data dictionary transport STATUS: %d %s\n",
                     status, mamaStatus_stringForStatus (status));
            exit(1);
        }
    }
    else
    {
        gDictTransport = gTransport;
    }

    /*The mamaSource used for creating the dictionary*/
    if (MAMA_STATUS_OK!=(status=mamaSource_create (&gDictSource)))
    {
        fprintf (stderr,
                 "Failed to create dictionary mamaSource STATUS: %d %s\n",
                 status, mamaStatus_stringForStatus (status));
        exit(1);
    }

    mamaSource_setId (gDictSource, "Dictionary_Source");
    mamaSource_setTransport (gDictSource, gDictTransport);
    mamaSource_setSymbolNamespace (gDictSource, gDictSymbolNamespace);
}

static void parseCommandLine (int argc, const char** argv)
{
    int i = 1;


    for (i = 1; i < argc;)
    {
        if ((strcmp (argv[i], "-S") == 0) ||
            (strcmp (argv[i], "-source") == 0)||
            (strcmp (argv[i], "-SN") == 0)||
            (strcmp (argv[i], "-symbolnamespace") == 0))
        {
            gSymbolNamespace = argv[i + 1];
            i += 2;
        }
        else if (strcmp (argv[i], "-d") == 0)
        {
            gDictSymbolNamespace = argv[i + 1];
            i += 2;
        }
        else if (strcmp ("-m", argv[i]) == 0)
        {
            gMiddleware = argv[i+1];
            i += 2;
        }
        else if (strcmp (argv[i], "-q") == 0)
        {
            gQuietness++;
            i++;
        }
        else if (strcmp (argv[i], "-tport") == 0)
        {
            gTransportName = argv[i+1];
            i += 2;
        }
        else if (strcmp (argv[i], "-dict_tport") == 0)
        {
            gDictTransportName = argv[i+1];
            i += 2;
        }
        else if (strcmp (argv[i], "-threads") == 0)
        {
            gThreads = atoi (argv[i + 1]);
            i += 2;
        }
        else if (strcmp (argv[i], "-v") == 0)
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
        else if ((strcmp (argv[i], "-?") == 0)||
                 ((strcmp (argv[i], "--help") == 0)))
        {
            usage (0);
        }
    }
}

static void subscribeToSymbols (void)
{
    int              howMany = 0;
    mama_status      status  = MAMA_STATUS_OK;
    size_t           i;
    mamaMsgCallbacks callbacks;
    memset(&callbacks, 0, sizeof(callbacks));

    callbacks.onCreate       = subscriptionOnCreate;
    callbacks.onError        = subscriptionOnError;
    callbacks.onMsg          = subscriptionOnMsg;
    callbacks.onQuality      = subscriptionOnQuality;
    callbacks.onGap          = NULL;
    callbacks.onRecapRequest = NULL;
    /*
     * If use of multiple threads/queue was specified create a new mamaQueue
     * and mamaDispatcher for each one
     */
    if ((gThreads > 0))
    {
        gQueues      = (mamaQueue*)calloc
                            (gThreads, sizeof (mamaQueue));
        gDispatchers = (mamaDispatcher*)calloc
                            (gThreads, sizeof (mamaDispatcher));

        for (i = 0; i < gThreads; i++)
        {
            if (MAMA_STATUS_OK != (status = mamaQueue_create (&gQueues[i],
                            gMamaBridge)))
            {
                fprintf (stderr, "Error creating queue %s\n",
                         mamaStatus_stringForStatus( status ) );
                exit(1);
            }

            status = mamaDispatcher_create (&gDispatchers[i] , gQueues[i]);

            if (status != MAMA_STATUS_OK)
            {
                fprintf (stderr, "Error creating dispatcher %s\n",
                         mamaStatus_stringForStatus( status ) );
                exit(1);
            }
        }
    }

    for (i = 0; i < gSymbolListIndex; i++)
    {
        /*If there is more than one queue round robin accross them*/
        mamaQueue queue = gQueues ==
                NULL ? gMamaDefaultQueue : gQueues[i % gThreads];

        mamaSubscription_allocate (&gSubscriptionList[howMany]);

        /*
            The subscription is not created immediately here. Rather an action
            to create the subscription is placed on the throttle queue.
        */
        status = mamaSubscription_create (gSubscriptionList[howMany],
                                          queue,
                                          &callbacks,
                                          gSubscriptionSource,
                                          gSymbolList[i],
                                          (void*)i);
    }
    if (status != MAMA_STATUS_OK)
    {
        fprintf (stdout,
                 "Error creating subscription for %s. STATUS: %s\n",
                  gSymbolList[i], mamaStatus_stringForStatus(status));
    }

    if (++howMany % 1000 == 0)
    {
            printf ("Subscribed to %d subjects.\n", howMany);
    }
}


void MAMACALLTYPE
symbolListSubscriptionOnMsg  (mamaSubscription subscription,
                              mamaMsg msg,
                              void *closure,
                              void *itemClosure)
{
    const char* newSymbols;
    const char* result     = NULL;

    switch (mamaMsgType_typeForMsg (msg))
    {
        case MAMA_MSG_TYPE_INITIAL:
            printf("Received initial for symbol list subscription.  Updating list of symbols to subscribe to.\n");
            mamaMsg_getString (msg, MamaFieldSymbolList.mName, MamaFieldSymbolList.mFid, &newSymbols);
            result = strtok ((char*)newSymbols, ",");

            while (result != NULL)
            {
                gSymbolList[gSymbolListIndex] = strdup (result);
                gSymbolListIndex++;
                result = strtok (NULL, ",");
            }

            break;
        case MAMA_MSG_TYPE_END_OF_INITIALS:
            printf("End of initials.  Subscribing to symbols.\n");
            subscribeToSymbols ();
            break;
        default:
            break;
    }
}

void MAMACALLTYPE
symbolListSubscriptionOnCreate (mamaSubscription subscription, void* closure)
{
    const char* source = NULL;

    mamaSubscription_getSource (subscription, &source);

    fprintf(stdout,
            "Created symbol list subscription: source=[%s]\n",
            source);
}

static void MAMACALLTYPE
symbolListSubscriptionOnError (mamaSubscription subscription,
                               mama_status      status,
                               void*            platformError,
                               const char*      subject,
                               void*            closure)
{
    fprintf (stdout,
             "An error occurred creating symbol list subscription: %s\n",
             mamaStatus_stringForStatus (status));
}

static void MAMACALLTYPE
symbolListSubscriptionOnQuality (mamaSubscription subsc,
                       mamaQuality      quality,
                       const char*      symbol,
                       short            cause,
                       const void*      platformInfo,
                       void*            closure)
{
    printf ("Quality changed to %s for symbol list subscription, cause %d, platformInfo: %s\n",
            mamaQuality_convertToString (quality), cause,
            platformInfo ? (char*)platformInfo: "");
}

void MAMACALLTYPE
subscriptionOnMsg  (mamaSubscription subscription,
                              mamaMsg msg,
                              void *closure,
                              void *itemClosure)
{
    const char* symbol = NULL;

    if (gQuietness < 1)
    {
        mamaSubscription_getSymbol (subscription, &symbol);
        printf ("Received message of type %s for %s.\n",
                mamaMsgType_stringForType (mamaMsgType_typeForMsg (msg)),
                symbol);
    }

}

void MAMACALLTYPE
subscriptionOnCreate (mamaSubscription subscription, void* closure)
{
    const char* source = NULL;
    const char* symbol = NULL;

    mamaSubscription_getSource (subscription, &source);
    mamaSubscription_getSymbol (subscription, &symbol);

    fprintf(stdout,
            "Created subscription: source=[%s], symbol=[%s]\n",
            source, symbol);
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

static void MAMACALLTYPE
subscriptionOnQuality (mamaSubscription subsc,
                       mamaQuality      quality,
                       const char*      symbol,
                       short            cause,
                       const void*      platformInfo,
                       void*            closure)
{
    printf ("Quality changed to %s for %s, cause %d, platformInfo: %s\n",
            mamaQuality_convertToString (quality), symbol, cause,
            platformInfo ? (char*)platformInfo: "");
}


void usage (int exitStatus)
{
    int i = 0;
    while (gUsageString[i]!=NULL)
    {
        printf ("%s\n", gUsageString[i++]);
    }
    exit (exitStatus);
}
