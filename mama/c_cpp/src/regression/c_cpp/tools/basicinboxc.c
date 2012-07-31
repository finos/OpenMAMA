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

#include "wombat/port.h"
#include "mama/mama.h"
#include "string.h"

static mamaTransport    gTransport    = NULL;
static mamaInbox        gInbox        = NULL;
static mamaPublisher    gPublisher    = NULL;
static mamaTimer        gTimer          = NULL;
static mamaBridge       gMamaBridge         = NULL;
static mamaQueue        gMamaDefaultQueue   = NULL;

static const char *     gTopic = "MAMA_INBOUND_TOPIC";
static const char *     gTransportName = "sub";
static int              gQuietLevel = 0;
static const char*      gMiddleware = "wmw";
static const char *     gUsageString[]  =
{
" This sample application demonstrates how to send mamaMsgs from an inbox,",
" and receive the reply.",
"",
" It accepts the following command line arguments:",
"",
"      [-s topic]         The topic on which to send the message. Default value",
"                         is \"MAMA_INBOUND_TOPIC\"",
"      [-tport name]      The transport parameters to be used from",
"                         mama.properties. Default is \"sub\"",
"      [-q]               Quiet mode. Suppress output.",
"      [-m middleware]    The middleware to use [wmw/lbm/tibrv]. Default is wmw",
"      [-?, --help]       Print this usage information and exit.",
"",
NULL
};

static void parseCommandLine    (int argc, const char** argv);
static void initializeMama      (void);
static void createInbox         (void);
static void createPublisher     (void);
static void sendRequest         (void);

static void MAMACALLTYPE
msgCB (mamaMsg msg, void* closure);

static void MAMACALLTYPE
errorCB (mama_status status, void* closure);


static void MAMACALLTYPE
timerCallback (mamaTimer timer, void *closure)
{
	sendRequest ();

}


static void createIntervalTimer (void)
{
    mama_status status;

    status = mamaTimer_create (&gTimer,
                               gMamaDefaultQueue,
                               timerCallback,
                               1,
                               NULL);

    if (status != MAMA_STATUS_OK)
    {
        printf ("Error creating timer: %s\n",
                mamaStatus_stringForStatus (status));
        exit (status);
    }
}


int main (int argc, const char** argv)
{
    parseCommandLine(argc, argv);

    initializeMama ();

    createPublisher ();

    createInbox ();

    createIntervalTimer ();

    mama_start (gMamaBridge);

    return 0;
}


void initializeMama (void)
{
    mama_status status;

    status = mama_loadBridge (&gMamaBridge, gMiddleware);

    if (status != MAMA_STATUS_OK)
    {
        printf ("Error loading %s bridge: %s\n",
                gMiddleware,
                mamaStatus_stringForStatus (status));
        exit (status);
    }

    status = mama_open ();

    if (status != MAMA_STATUS_OK)
    {
        printf ("Error initializing mama: %s\n",
                mamaStatus_stringForStatus (status));
        exit (status);
    }

    mama_getDefaultEventQueue (gMamaBridge, &gMamaDefaultQueue);

    status = mamaTransport_allocate(&gTransport);

    if (status != MAMA_STATUS_OK)
    {
        printf ("Error allocating transport: %s\n",
                mamaStatus_stringForStatus (status));
        exit (status);
    }

    status = mamaTransport_create (gTransport, gTransportName, gMamaBridge);

    if (status != MAMA_STATUS_OK)
    {
        printf ("Error creating transport: %s\n",
                mamaStatus_stringForStatus (status));
        exit (status);
    }
}

/* Used to destroy messages sent on the throttle */
static void MAMACALLTYPE
sendCompleteCb (mamaPublisher publisher,
                mamaMsg       msg,
                mama_status   status,
                void*         closure)
{
    mama_status delStatus = MAMA_STATUS_OK;
    if (msg)
    {
        if (MAMA_STATUS_OK!=(delStatus=mamaMsg_destroy (msg)))
        {
            fprintf (stderr,
                     "Error destroying message from throttle: %s\n",
                     mamaStatus_stringForStatus (delStatus));
        }
    }

    printf ("Message sent!\n");
}

static void sendRequest (void)
{
    mama_status status;
    mamaMsg msg = NULL;

    status = mamaMsg_create (&msg);

    if (status != MAMA_STATUS_OK)
    {
        printf ("Error creating msg: %s\n",
                mamaStatus_stringForStatus (status));
        exit (status);
    }

    status = mamaMsg_addI32 (msg, "field", 1, 222);

    if (status != MAMA_STATUS_OK)
    {
        printf ("Error adding to msg: %s\n",
                mamaStatus_stringForStatus (status));
        exit (status);
    }

    mama_log (MAMA_LOG_LEVEL_ERROR,
                          "mamaPublisher_sendFromInboxWithThrottle(): "
                          " %s",
                          mamaMsg_toString(msg));

    /*The message can be destroyed once the sendCompleteCb has been invoked.*/
    status = mamaPublisher_sendFromInboxWithThrottle (gPublisher, gInbox, msg,
            sendCompleteCb, NULL);

    if (status != MAMA_STATUS_OK)
    {
        printf ("Error sending request: %s\n",
                mamaStatus_stringForStatus (status));
        exit (status);
    }
}

static void createInbox (void)
{
    mama_status status;

    status = mamaInbox_create (&gInbox,
                               gTransport,
                               gMamaDefaultQueue,
                               msgCB,
                               errorCB,
                               NULL);

    if (status != MAMA_STATUS_OK)
    {
        printf ("Error creating inbox: %s\n",
                mamaStatus_stringForStatus (status));
        exit (status);
    }
}

static void MAMACALLTYPE
msgCB (mamaMsg msg, void *closure)
{
    printf ("Received reply: %s\n", mamaMsg_toString (msg));
    mama_stop (gMamaBridge);
}

static void MAMACALLTYPE
errorCB (mama_status status, void *closure)
{
    printf ("Error creating inbox: %s\n",
            mamaStatus_stringForStatus (status));
    exit (status);
}



static void createPublisher (void)
{
    mama_status status;

    status = mamaPublisher_create (&gPublisher,
                                   gTransport,
                                   gTopic,
                                   NULL,   /* Not needed for basic publishers */
                                   NULL); /* Not needed for basic publishers */

    if (status != MAMA_STATUS_OK)
    {
        printf ("Error creating publisher: %s\n",
                mamaStatus_stringForStatus (status));
        exit (status);
    }
}

static void usage(int exitStatus)
{
    int i = 0;
    while (gUsageString[i]!=NULL)
    {
        printf ("%s\n", gUsageString[i++]);
    }
    exit(exitStatus);
}

void parseCommandLine (int argc, const char **argv)
{
    int i = 0;
    for (i = 1; i < argc;)
    {
        if (strcmp ("-s", argv[i]) == 0)
        {
            gTopic = argv[i+1];
            i += 2;
        }
        else if (strcmp ("-tport", argv[i]) == 0)
        {
            gTransportName = argv[i+1];
            i += 2;
        }
        else if (strcmp ("-q", argv[i]) == 0)
        {
            gQuietLevel++;
            i++;
        }
        else if (strcmp( argv[i], "-v") == 0 )
        {
            if (mama_getLogLevel () == MAMA_LOG_LEVEL_WARN )
            {
                mama_enableLogging( stderr, MAMA_LOG_LEVEL_NORMAL);
            }
            else if (mama_getLogLevel () == MAMA_LOG_LEVEL_NORMAL )
            {
                mama_enableLogging( stderr, MAMA_LOG_LEVEL_FINE );
            }
            else if(mama_getLogLevel () == MAMA_LOG_LEVEL_FINE )
            {
                mama_enableLogging( stderr, MAMA_LOG_LEVEL_FINER );
            }
            else
            {
                mama_enableLogging( stderr, MAMA_LOG_LEVEL_FINEST );
            }
            i++;
        }
        else if (strcmp ("-m", argv[i]) == 0)
        {
            gMiddleware = argv[i+1];
            i += 2;
        }
        else if ((strcmp(argv[i], "-?") == 0) ||
                 ((strcmp(argv[i], "--help") == 0)))
        {
            usage(EXIT_SUCCESS);
        }
        else
        {
            printf("Unrecognized parameter: <%s>\n", argv[i]);
            exit(EXIT_FAILURE);
        }
    }

    if (gQuietLevel < 2)
    {
        printf ("Starting Publisher with:\n"
                "   topic:              %s\n"
                "   transport:          %s\n",
                gTopic == NULL ? "(null)":gTopic,
                gTransportName == NULL ? "(null)":gTransportName);
    }
}
