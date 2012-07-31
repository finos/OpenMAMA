/* $Id: mamasubscriberc.c,v 1.1.2.2 2011/09/27 16:54:39 emmapollock Exp $
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

static mamaTransport    gTransport     = NULL;
static mamaSubscription gSubscription  = NULL;
static int              gWildcard      = 0;
static int              gTport      = 0;
static mamaMsgCallbacks gCb;
static mamaWildCardMsgCallbacks gWcCb;

static const char *     gTopic         = "MAMA_TOPIC";
static const char *     gMuteTopic     = NULL;
static const char *     gTransportName = "sub";
static int              gQuietLevel    = 0;
static MamaLogLevel     gSubscLogLevel = MAMA_LOG_LEVEL_NORMAL;
static mamaBridge       gMamaBridge    = NULL;


static const char *     gMiddleware    = "wmw";
static mamaQueue        gMamaDefaultQueue   = NULL;
static const char *     gUsageString[] =
{
"This sample application demonstrates how to subscribe and process",
"mamaMsg's from a basic subscription.",    
"",
" It accepts the following command line arguments:",
"      [-s topic]         The topic to which to subscribe. Default value",
"                         is \"MAMA_TOPIC\".",
"      [-tport name]      The transport parameters to be used from",
"                         mama.properties. Default is sub",
"      [-m middleware]    The middleware to use [wmw/lbm/tibrv]. Default", 
"                         is wmw.", 
"      [-q]               Quiet mode. Suppress output.",
"      [-v]               Increase verbosity. Can be passed multiple times",
NULL
};

static void parseCommandLine            (int argc, const char **argv);
static void initializeMama              (void);
static void createSubscriber            (void);
static void createWildCardSubscriber    (void);
static void displayMsg                  (mamaMsg msg);

static void MAMACALLTYPE
displayCb (const mamaMsg          msg,
           const mamaMsgField     field,
           void *                 closure);

static void MAMACALLTYPE
createCb (mamaSubscription       subscription,
          void *                 closure);

static void MAMACALLTYPE
errorCb (mamaSubscription       subscription,
         mama_status            status,
         void*                  platformError,
         const char*            subject,
         void*                  closure);

static void MAMACALLTYPE
msgCb (mamaSubscription       subscription, 
       mamaMsg                msg, 
       void *                 closure,
       void *                 itemClosure);

static void MAMACALLTYPE
wildCardMsgCb (mamaSubscription       subscription, 
               mamaMsg                msg, 
               const char*            topic,
               void *                 closure,
               void *                 itemClosure);

static void usage (int  exitStatus);

int main (int argc, const char** argv)
{
    parseCommandLine (argc, argv);

    initializeMama ();
   
    if (gWildcard || gTport)
        createWildCardSubscriber ();
    else
        createSubscriber ();

    mama_start (gMamaBridge);

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

static void createWildCardSubscriber (void)
{
    mama_status status;
    const char* source = gTport ? gTopic : NULL;
    const char* symbol = gWildcard ? gTopic : NULL;

    memset(&gWcCb, 0, sizeof(gWcCb));
    gWcCb.onCreate  = createCb;
    gWcCb.onError   = errorCb;
    gWcCb.onMsg     = wildCardMsgCb;

    if (MAMA_STATUS_OK!=(status=mamaSubscription_allocate (&gSubscription)))
    {
        printf ("Error allocating subscription: %s\n", 
                mamaStatus_stringForStatus (status));
        exit (status);
    }
    
    mamaSubscription_setDebugLevel (gSubscription, gSubscLogLevel);

    if (MAMA_STATUS_OK!=(status=mamaSubscription_createBasicWildCard (gSubscription,
                                           gTransport,
                                           gMamaDefaultQueue,
                                           &gWcCb,
                                           source,
                                           symbol,
                                           NULL)))
    {
        printf ("Error creating subscription: %s\n", 
                mamaStatus_stringForStatus (status));
        exit (status);
    }
}

static void createSubscriber (void)
{
    mama_status status;

    memset(&gCb, 0, sizeof(gCb));
    gCb.onCreate  = createCb;
    gCb.onError   = errorCb;
    gCb.onMsg     = msgCb;
    gCb.onQuality = NULL;       /* not used by basic subscriptions */
    gCb.onGap = NULL;           /* not used by basic subscriptions */
    gCb.onRecapRequest = NULL;  /* not used by basic subscriptions */

    if (MAMA_STATUS_OK!=(status=mamaSubscription_allocate (&gSubscription)))
    {
        printf ("Error allocating subscription: %s\n", 
                mamaStatus_stringForStatus (status));
        exit (status);
    }
    
    mamaSubscription_setDebugLevel (gSubscription, gSubscLogLevel);

    if (MAMA_STATUS_OK!=(status=mamaSubscription_createBasic (gSubscription,
                                           gTransport,
                                           gMamaDefaultQueue,
                                           &gCb,
                                           gTopic,
                                           NULL)))
    {
        printf ("Error creating subscription: %s\n", 
                mamaStatus_stringForStatus (status));
        exit (status);
    }
}

static void MAMACALLTYPE
createCb (mamaSubscription subscription, void *closure)
{
    if (gQuietLevel < 2)
    {
        printf ("mamasubscriberc: Created inbound subscription.\n");
    }
}

static void MAMACALLTYPE
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

static void MAMACALLTYPE
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

static void MAMACALLTYPE
wildCardMsgCb (mamaSubscription       subscription, 
               mamaMsg                msg, 
               const char*            topic,
               void *                 closure,
               void *                 itemClosure)
{
    static int i = 0;
    
    if (gMuteTopic)
    {
        const char* topic = NULL;
        mamaMsg_getString (msg, "PublisherTopic", 10002, &topic);
        if (0 == strncmp (topic, gMuteTopic, strlen(topic)))
        {
            gMuteTopic = NULL;
            printf ("Mutting %s\n", topic);
            mamaSubscription_muteCurrentTopic (subscription);
            return;
        }
    }
    if (itemClosure == NULL)
    {
        mamaSubscription_setItemClosure (subscription, (void*)(++i));
    }
    if (gQuietLevel < 2)
    {
        printf ("Received Wild Card Message with topic %s closure=%d\n", topic,
            (int)itemClosure);
    }
    displayMsg (msg);
}

void MAMACALLTYPE
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
            if (mama_getLogLevel() == MAMA_LOG_LEVEL_WARN)
            {
                mama_enableLogging(stderr, MAMA_LOG_LEVEL_NORMAL);
            }
            else if (mama_getLogLevel() == MAMA_LOG_LEVEL_NORMAL)
            {
                mama_enableLogging(stderr, MAMA_LOG_LEVEL_FINE);
            }
            else if( mama_getLogLevel() == MAMA_LOG_LEVEL_FINE)
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
            gMiddleware = (argv[i+1]);
            i += 2;               
        }
        else if (strcmp ("-w", argv[i]) == 0)
        {
            if (gTport) 
            {
                printf ("A subscription can only be wildcard or "
                        "tport not both\n");
                exit(1);
            }
            gWildcard = 1;
            i += 1;               
        }
        else if (strcmp ("-mute", argv[i]) == 0)
        {
            gMuteTopic = argv[i+1];
            i += 2;               
        }
        else if (strcmp ("-t", argv[i]) == 0)
        {
            if (gWildcard) 
            {
                printf ("A subscription can only be wildcard or "
                        "tport not both\n");
                exit(1);
            }
            gTport = 1;
            i += 1;               
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
        else
        {
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

