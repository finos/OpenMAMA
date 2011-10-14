/* $Id: basicpub.c,v 1.1.2.2 2011/09/26 10:10:44 ianbell Exp $
 *
 * OpenMAMA: The open middleware agnostic messaging API
 * Copyright (C) 2011 NYSE Inc.
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
 * This sample application demonstrates how to publish mama messages, and
 * respond to requests from a clients inbox.
 *
 * It accepts the following command line arguments:
 *      [-s topic]         The topic on which to send messages. Default value
 *                         is "MAMA_TOPIC".
 *      [-l topic]         The topic on which to listen for inbound requests.
 *                         Default is "MAMA_INBOUND_TOPIC".
 *      [-i interval]      The interval between messages .Default, 0.5.
 *      [-tport name]      The transport parameters to be used from
 *                         mama.properties.
 *      [-q]               Quiet mode. Suppress output.
 *---------------------------------------------------------------------------*/

#include <unistd.h>

#include "mama/mama.h"
#include "string.h"
#include <time.h>

static mamaTransport    gTransport      = NULL;
static mamaTimer        gTimer          = NULL;
static mamaSubscription gSubscription   = NULL;
static mamaPublisher    gPublisher      = NULL;
static mamaMsgCallbacks gInboundCb;

static mamaBridge       gMamaBridge         = NULL;
static mamaQueue        gMamaDefaultQueue   = NULL;
static const char*      gMiddleware         = "avis";

static const char *     gOutBoundTopic  = "MAMA_TOPIC";
static const char *     gInBoundTopic   = "MAMA_INBOUND_TOPIC";
static const char *     gTransportName  = "pub";
static double           gInterval       = 0.5;
static int              gQuietLevel     = 0;
static const char *     gUsageString[]  =
{
" This sample application demonstrates how to publish mama messages, and",
" respond to requests from a clients inbox.",
"",
" It accepts the following command line arguments:",
"      [-s topic]         The topic on which to send messages. Default is",
"                         is \"MAMA_TOPIC\".",
"      [-l topic]         The topic on which to listen for inbound requests.",
"                         Default is \"MAMA_INBOUND_TOPIC\".",
"      [-i interval]      The interval between messages .Default in  0.5.",
"      [-tport name]      The transport parameters to be used from",
"                         mama.properties. Default is pub",
"      [-m middleware]    The middleware to use [wmw/lbm/tibrv].",
"                         Default is wmw.",
"      [-q]               Quiet mode. Suppress output.",
"      [-v]               Increase verbosity. Can be passed multiple times",
NULL
};

static void parseCommandLine    (int argc, const char **argv);
static void initializeMama      (void);
static void createIntervalTimer (void);
#ifdef WIN32
static void __stdcall
#else
static void
#endif
timerCallback       (mamaTimer timer, void *closure);
static void createInboundSubscription (void);

#ifdef WIN32
static void __stdcall
#else
static void
#endif
inboundCreateCb     (mamaSubscription subscription, void *closure);

#ifdef WIN32
static void __stdcall
#else
static void
#endif
inboundErrorCb      (mamaSubscription subscription,
                     mama_status status,
                     void* platformError,
                     const char *subject,
                     void *closure);

#ifdef WIN32
static void __stdcall
#else
static void
#endif
inboundMsgCb        (mamaSubscription subscription,
                     mamaMsg msg,
                     void *closure,
                     void *itemClosure);

static void publishMessage      (mamaMsg request);
static void createPublisher     (void);
static void usage               (int exitStatus);


int main (int argc, const char **argv)
{
    parseCommandLine (argc, argv);
    initializeMama ();
    createInboundSubscription ();
    createPublisher ();
    sleep(10);
    createIntervalTimer ();
    mama_start(gMamaBridge);
    return 0;
}

void initializeMama (void)
{
    mama_status status;

    status = mama_loadBridge (&gMamaBridge, gMiddleware);
    if (status != MAMA_STATUS_OK)
    {
        printf ("Error loading bridge: %s\n",
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

    /*Use the default internal event queue for all subscriptions and timers*/
    mama_getDefaultEventQueue (gMamaBridge, &gMamaDefaultQueue);

    status = mamaTransport_allocate (&gTransport);

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

static void createPublisher (void)
{
    mama_status status;

    status = mamaPublisher_create (&gPublisher,
                                   gTransport,
                                   gOutBoundTopic,
                                   NULL,   /* Not needed for basic publishers */
                                   NULL); /* Not needed for basic publishers */

    if (status != MAMA_STATUS_OK)
    {
        printf ("Error creating publisher: %s\n",
                mamaStatus_stringForStatus (status));
        exit (status);
    }
}

static void publishMessage (mamaMsg request)
{
    static int msgNumber = 0;
    static mamaMsg msg = NULL;
    mama_status status;

	if (msg == NULL)
	{
		status = mamaMsg_create(&msg);
	}
	else
	{
		status = mamaMsg_clear(msg);
	}

	if (status != MAMA_STATUS_OK)
	{
		printf ("Error creating/clearing msg: %s\n",
				mamaStatus_stringForStatus (status));
		exit (status);
	}


    /* Add some fields. This is not required, but illustrates how to
     * send data.
     */
	status = mamaMsg_addI32 (msg, MamaFieldMsgType.mName, MamaFieldMsgType.mFid, MAMA_MSG_STATUS_OK);
    if (status != MAMA_STATUS_OK)
    {
        printf ("Error adding int to msg: %s\n",
                mamaStatus_stringForStatus (status));
        exit (status);
    }

/*   if (msgNumber==0)
#   {
#      
#	 status = mamaMsg_addString (msg, "PublisherTopic", 10002, gOutBoundTopic);
   } */
 
   switch(msgNumber) 
   {
    case 0 :
        status = mamaMsg_addString (msg, "PublisherTopic", 10002, gOutBoundTopic);
        break;
    case 1 :
	status = mamaMsg_addF32(msg, "f32", 1001, 1.23456);	
        break;
    case 2 :
        status = mamaMsg_addF64(msg, "f64", 1002, 0.98765);
        break;
    case 3 :
        status = mamaMsg_addI8(msg, "i8", 1003, 244);
        break;

    case 4 :
        status = mamaMsg_addI16(msg, "i16", 1004, 63000);
        break;

    case 5 :
        status = mamaMsg_addI32(msg, "i32", 1005, 46810);
        break;

    case 6 :
        status = mamaMsg_addI64(msg, "i64", 1006, 38765);
        break;

    case 7 :
        status = mamaMsg_addU8(msg, "u8", 1007, 254);
        break;

    case 8 :
        status = mamaMsg_addU16(msg, "u16", 1008, 3.57911);
        break;

    case 9 :
        status = mamaMsg_addU32(msg, "u32", 1009, 97531);
        break;

    case 10 :
        status = mamaMsg_addU64(msg, "u64", 1010, 9.7531);
        break;

   }


  

    if (status != MAMA_STATUS_OK)
    {
        printf ("Error adding data to msg: %s\n",
                mamaStatus_stringForStatus (status));
        exit (status);
    }

    if (request)
    {
        if (gQuietLevel < 1)
        {
            printf ("\nPublishing message %d to inbox .", msgNumber);
        }
		status = mamaMsg_addU8 (msg, MamaFieldMsgType.mName, MamaFieldMsgType.mFid, MAMA_MSG_TYPE_RECAP);
        status = mamaPublisher_sendReplyToInbox (gPublisher, request, msg);

    }
    else
    {
        if (gQuietLevel < 1)
        {
            printf ("\nPublishing message %d to %s.", msgNumber, gOutBoundTopic);
			fflush(stdout);
        }
        status = mamaPublisher_send (gPublisher, msg);
        if (gQuietLevel < 1)
        {
           const char* subject=NULL;
           mamaMsg_getSendSubject(msg, &subject);
           if (subject)
              printf ("\tsubject=%s", subject);
           }
           printf ("\tmsg=%s", mamaMsg_toString (msg));
    }

    if (status != MAMA_STATUS_OK)
    {

        printf ("Error sending msg: %s\n",
                mamaStatus_stringForStatus (status));
        exit (status);
    }

        msgNumber++;	
	if(msgNumber>11)
	{
		fflush(stdout);
		sleep(1);
		
		exit(0);
	}

}

static void createInboundSubscription (void)
{
    mama_status status;

    memset(&gInboundCb, 0, sizeof(gInboundCb));
    gInboundCb.onCreate         = inboundCreateCb;
    gInboundCb.onError          = inboundErrorCb;
    gInboundCb.onMsg            = inboundMsgCb;
    gInboundCb.onQuality        = NULL; /* not used by basic subscriptions */
    gInboundCb.onGap            = NULL; /* not used by basic subscriptions */
    gInboundCb.onRecapRequest   = NULL; /* not used by basic subscriptions */

    status = mamaSubscription_allocate (&gSubscription);

    status = mamaSubscription_createBasic (gSubscription,
                                           gTransport,
                                           gMamaDefaultQueue,
                                           &gInboundCb,
                                           gInBoundTopic,
                                           NULL);
    if (status != MAMA_STATUS_OK)
    {
        printf ("Error creating subscription: %s\n",
                mamaStatus_stringForStatus (status));
        exit (status);
    }
}

#ifdef WIN32
static void __stdcall
#else
static void
#endif
inboundCreateCb (mamaSubscription subscription, void *closure)
{
    if (gQuietLevel < 2)
    {
        printf ("Created inbound subscription.\n");
    }
}

#ifdef WIN32
static void __stdcall
#else
static void
#endif
inboundErrorCb (mamaSubscription subscription,
                mama_status      status,
                void*            platformError,
                const char*      subject,
                void*            closure)
{
    printf ("Error creating subscription: %s\n",
            mamaStatus_stringForStatus (status));
    exit (status);
}

#ifdef WIN32
static void __stdcall
#else
static void
#endif
inboundMsgCb (mamaSubscription subscription,
              mamaMsg          msg,
              void*            closure,
              void*            itemClosure)
{
    mamaMsgReply reply1, reply2;
    mamaMsg newmsg;

    if (gQuietLevel < 2)
    {
        printf ("Recieved inbound msg. Sending response\n");
    }

    if (!mamaMsg_isFromInbox (msg))
    {
        printf ("Inbound msg not from inbox. No reply sent.\n");
        return;
    }

    publishMessage (msg);
}

static void createIntervalTimer (void)
{
    mama_status status;

    status = mamaTimer_create (&gTimer,
                               gMamaDefaultQueue,
                               timerCallback,
                               gInterval,
                               NULL);

    if (status != MAMA_STATUS_OK)
    {
        printf ("Error creating timer: %s\n",
                mamaStatus_stringForStatus (status));
        exit (status);
    }
}


#ifdef WIN32
static void __stdcall
#else
static void
#endif
timerCallback (mamaTimer timer, void *closure)
{
    publishMessage (NULL);

}

void parseCommandLine (int argc, const char **argv)
{
    int i = 0;
    for (i = 1; i < argc; )
    {
        if (strcmp ("-s", argv[i]) == 0)
        {
            gOutBoundTopic = argv[i+1];
            i += 2;
        }
        else if (strcmp ("-l", argv[i]) == 0)
        {
            gInBoundTopic = argv[i+1];
            i += 2;
        }
        else if ((strcmp (argv[i], "-h") == 0)||
                 (strcmp (argv[i], "--help") == 0)||
                 (strcmp (argv[i], "-?") == 0))
        {
            usage(0);
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
        else if (strcmp ("-q", argv[i]) == 0)
        {
            gQuietLevel++;
            i++;
        }
        else if (strcmp ("-m", argv[i]) == 0)
        {
            gMiddleware = argv[i+1];
            i += 2;
        }
        else if (strcmp (argv[i], "-v") == 0)
        {
            if ( mama_getLogLevel () == MAMA_LOG_LEVEL_WARN )
            {
                mama_enableLogging( stderr, MAMA_LOG_LEVEL_NORMAL );
            }
            else if ( mama_getLogLevel () == MAMA_LOG_LEVEL_NORMAL )
            {
                mama_enableLogging( stderr, MAMA_LOG_LEVEL_FINE );
            }
            else if( mama_getLogLevel () == MAMA_LOG_LEVEL_FINE )
            {
                mama_enableLogging( stderr, MAMA_LOG_LEVEL_FINER );
            }
            else
            {
                mama_enableLogging( stderr, MAMA_LOG_LEVEL_FINEST );
            }
            i++;
        }
		else
		{
			printf ("Unknown arg: %s\n",	argv[i]);
			i++;
		}
    }

    if (gQuietLevel < 2)
    {
        printf ("Starting Publisher with:\n"
                "   topic:              %s\n"
                "   inbound topic:      %s\n"
                "   interval            %f\n"
                "   transport:          %s\n",
                gOutBoundTopic, gInBoundTopic, gInterval, gTransportName);
    }
}

void usage (int exitStatus)
{
    int i=0;
    while (gUsageString[i]!=NULL)
    {
        printf ("%s\n", gUsageString[i++]);
    }
    exit (exitStatus);
}
