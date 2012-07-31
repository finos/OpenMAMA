/* $Id: mamamultisubscriberc.c,v 1.1.2.6 2011/09/29 07:02:22 ianbell Exp $
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
#include "string.h"
#define MAX_BRIDGES 10

static const char *     gTopic              = "MAMA_TOPIC";
static const char *     gTransportName      = "sub";
static int              gQuietLevel         = 0;
static MamaLogLevel     gSubscLogLevel      = MAMA_LOG_LEVEL_NORMAL;
static const char**     gMamaMiddlewares    = NULL;
static mamaBridge*      gMamaBridges        = NULL;
static size_t           gNumBridges         = 0;
static const char *     gUsageString[] =
{
"This sample application demonstrates how to subscribe and process",
"mamaMsg's from a basic subscription using one or more middlewares at once.",
"If using more than one middleware then the same transport must be defined",
"for each.",
"",
" It accepts the following command line arguments:",
"      [-s topic]         The topic to which to subscribe. Default value",
"                         is \"MAMA_INBOUND_TOPIC\".",
"      [-tport name]      The transport parameters to be used from",
"                         mama.properties. Default is sub",
"      [-q]               Quiet mode. Suppress output.",
"      [-m  middleware]    The one or more middlewares to use [wmw/lbm/tibrv].",
"      [-v]               Increase verbosity. Can be passed multiple times",
NULL
};

static void parseCommandLine    (int argc, const char **argv);
static void initialize          (void);
static void createSubscriber    (mamaBridge bridge, mamaTransport transport);
static void MAMACALLTYPE startCB             ( mama_status status );
static void start               (void);
static void displayMsg          (mamaMsg msg);
static void usage               (int  exitStatus);

static void
MAMACALLTYPE
displayCb (const mamaMsg          msg,
           const mamaMsgField     field,
           void *                 closure);

static void
MAMACALLTYPE
createCb (mamaSubscription       subscription,
          void *                 closure);

static void
MAMACALLTYPE
errorCb (mamaSubscription       subscription,
         mama_status            status,
         void*                  platformError,
         const char*            subject,
         void*                  closure);

static void
MAMACALLTYPE
msgCb (mamaSubscription       subscription,
       mamaMsg                msg,
       void *                 closure,
       void *                 itemClosure);



int main (int argc, const char** argv)
{
    gMamaMiddlewares = (const char**)
                       calloc (MAX_BRIDGES, sizeof (char*));
    gMamaBridges     = (mamaBridge*)
                       calloc (MAX_BRIDGES, sizeof (mamaBridge));

    parseCommandLine (argc, argv);

    initialize ();

    start ();

    return 0;
}

void initialize (void)
{
   int              i        = 0;
   mama_status      status   = MAMA_STATUS_OK;

   for (i = 0; i != gNumBridges; ++i)
   {
        if (MAMA_STATUS_OK != (status =
            mama_loadBridge (&gMamaBridges[i], gMamaMiddlewares[i])))
        {
            printf ("Error loading bridge: %s\n",
                    mamaStatus_stringForStatus (status));
            exit (status);
        }
    }

    if (MAMA_STATUS_OK != (status = mama_open ()))
    {
        printf ("Error initializing mama: %s\n",
                mamaStatus_stringForStatus (status));
        exit (status);
    }

    for (i = 0; i != gNumBridges; ++i)
    {
        mamaTransport tport = NULL;
        if (MAMA_STATUS_OK != (status =
            mamaTransport_allocate (&tport)))
        {
            printf ("Error allocating transport: %s\n",
                    mamaStatus_stringForStatus (status));
            exit (status);
        }

        if (MAMA_STATUS_OK != (status =
            mamaTransport_create (tport, gTransportName, gMamaBridges[i])))
        {
            printf ("Error creating transport: %s\n",
                    mamaStatus_stringForStatus (status));
            exit (status);
        }

        createSubscriber (gMamaBridges[i], tport);
   }
}

static void createSubscriber (mamaBridge bridge, mamaTransport transport)
{
    mama_status         status;
    mamaSubscription    subscription;
    mamaMsgCallbacks    callbacks;
    mamaQueue           queue;

    callbacks.onCreate          = createCb;
    callbacks.onError           = errorCb;
    callbacks.onMsg             = msgCb;
    callbacks.onQuality         = NULL; /* not used by basic subscriptions */
    callbacks.onGap             = NULL; /* not used by basic subscriptions */
    callbacks.onRecapRequest    = NULL; /* not used by basic subscriptions */


    if (MAMA_STATUS_OK!=(status=mamaSubscription_allocate (&subscription)))
    {
        printf ("Error allocating subscription: %s\n",
                mamaStatus_stringForStatus (status));
        exit (status);
    }

    /* Create a dispatcher for each bridge to use */
    if (MAMA_STATUS_OK != (status =
        mama_getDefaultEventQueue (bridge, &queue)))
    {
        printf ("Error getting default queue: %s\n",
                mamaStatus_stringForStatus (status));
        exit (status);
    }

    mamaSubscription_setDebugLevel (subscription, gSubscLogLevel);

    if (MAMA_STATUS_OK!=(status=mamaSubscription_createBasic (subscription,
                                           transport,
                                           queue,
                                           &callbacks,
                                           gTopic,
                                           NULL)))
    {
        printf ("Error creating subscription: %s\n",
                mamaStatus_stringForStatus (status));
        exit (status);
    }
}

void MAMACALLTYPE
startCB ( mama_status status )
{}

void start ()
{
    int              i           = 0;

    for (i = 1; i != gNumBridges; ++i)
    {
        mama_startBackground (gMamaBridges[i], startCB);
    }

    mama_start (gMamaBridges[0]);

}

static void
MAMACALLTYPE
createCb (mamaSubscription subscription, void *closure)
{
    if (gQuietLevel < 2)
    {
        printf ("mamasubscriberc: Created inbound subscription.\n");
    }
}

static void
MAMACALLTYPE
errorCb (mamaSubscription   subscription,
         mama_status        status,
         void*              platformError,
         const char*        subject,
         void*              closure)
{
    printf ("mamasubscriberc: Error creating subscription: %s\n",
            mamaStatus_stringForStatus (status));
    exit (status);
}

static void
MAMACALLTYPE
msgCb (mamaSubscription  subscription,
       mamaMsg           msg,
       void*             closure,
       void*             itemClosure)
{
    if (gQuietLevel < 2)
    {
        printf ("mamasubscriberc: Recieved msg.\n");
    }
    displayMsg (msg);
}

void
MAMACALLTYPE
displayCb  (const mamaMsg       msg,
            const mamaMsgField  field,
            void*               closure)
{
    char         value[256] = "";
    const char*  typeName   = "(null)";
    const char*  fieldName  = "(null)";
    uint16_t     fid        = 0;

    if  (MAMA_STATUS_OK != mamaMsgField_getTypeName  (field,&typeName))
    {
        printf ("Failed to get type name\n");
        return;
    }
    if  (MAMA_STATUS_OK != mamaMsgField_getName      (field,&fieldName))
    {
        printf ("Failed to get name\n");
        return;
    }
    if  (MAMA_STATUS_OK != mamaMsgField_getFid       (field,&fid))
    {
        printf ("Failed to get Fid\n");
        return;
    }

    if  (MAMA_STATUS_OK != mamaMsgField_getAsString  (field, value, 256))
    {
        printf ("Failed to get field: %d:%s\n", fid, fieldName);
        return;
    }

    printf ("    %20s %5u %20s %20s\n", fieldName, fid, typeName, value);

}

void displayMsg (mamaMsg msg)
{
    mamaMsg_iterateFields (msg, displayCb, NULL, msg);
}

void parseCommandLine (int argc, const char** argv)
{
    int i = 0;
    for (i = 1; i < argc;)
    {
        if (strcmp ("-s", argv[i]) == 0)
        {
            gTopic = argv[i+1];
            i += 2;
        }
        else if ((strcmp(argv[i], "-h") == 0) ||
                 (strcmp(argv[i], "--help") == 0) ||
                 (strcmp(argv[i], "-?") == 0))
        {
            usage(0);
        }
        else if(strcmp("-tport", argv[i]) == 0)
        {
            gTransportName = argv[i+1];
            i += 2;
        }
        else if (strcmp ("-q", argv[i]) == 0)
        {
            gQuietLevel++;
            i++;
        }
        else if (strcmp( argv[i], "-v") == 0)
        {
            if (mama_getLogLevel () == MAMA_LOG_LEVEL_WARN)
            {
                mama_enableLogging(stderr, MAMA_LOG_LEVEL_NORMAL);
            }
            else if (mama_getLogLevel () == MAMA_LOG_LEVEL_NORMAL)
            {
                mama_enableLogging(stderr, MAMA_LOG_LEVEL_FINE);
            }
            else if( mama_getLogLevel () == MAMA_LOG_LEVEL_FINE)
            {
                mama_enableLogging(stderr, MAMA_LOG_LEVEL_FINER);
            }
            else
            {
                mama_enableLogging(stderr, MAMA_LOG_LEVEL_FINEST);
            }
            i++;
        }
        else if (strcmp ("-m", argv[i]) == 0)
        {
            gMamaMiddlewares [gNumBridges++] = argv[i+1];
            i += 2;
        }
        else if (strcmp(argv[i], "-V") == 0)
        {
            if (gSubscLogLevel == MAMA_LOG_LEVEL_NORMAL)
            {
                gSubscLogLevel = MAMA_LOG_LEVEL_FINE;
            }
            else if (gSubscLogLevel == MAMA_LOG_LEVEL_FINE)
            {
                gSubscLogLevel = MAMA_LOG_LEVEL_FINER;
            }
            else
            {
                gSubscLogLevel = MAMA_LOG_LEVEL_FINEST;
            }
            i++;
        }
    }

    if (gQuietLevel < 2)
    {
        printf ("Starting Subscriber with:\n topic:              %s\n"
                "   transport:          %s\n",
                gTopic, gTransportName);
    }
}

void usage (int exitStatus)
{
    int i = 0;
    while (gUsageString[i] != NULL)
    {
        printf ("%s\n", gUsageString[i++]);
    }
    exit (exitStatus);
}
