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

#include <iostream>

using std::cerr;
using std::cout;
using std::endl;


#include "mama/mama.h"
#include "mama/mamacpp.h"

using namespace Wombat;
#define MAX_NUM_MSGS 1024

static int           gCount         = 0;
static const char *  gOutBoundTopic = "MAMA_TOPIC";
static const char *  gSymbol        = NULL;
static const char *  gInBoundTopic  = "MAMA_INBOUND_TOPIC";
static const char *  gTransportName = "pub";
static const char *  gMiddleware    = "wmw";
static MamaQueue*    gDefaultQueue  = NULL;
static mamaBridge    gBridge        = NULL;
static double        gInterval      = 0.5;
static int           gQuietLevel    = 0;
static MamaMsg    *  gMsg[MAX_NUM_MSGS];
static int           gNumMsg        = 0;
static const char *  gUsageString[] =
{
    "This sample application demonstrates how to publish customized messages",
    "from command line. MdSeqNum is calculated and filled into messages.",
    "This program is based on mamapublishercpp",
    "",
    "It accepts the following command line arguments:",
    "./mamacoolpublishercpp -S MAMA_TOPIC -i 1 -tport pub \\",
    "    -s CTA.MSFT -m wField:1:I32:1 wField:2:I32:2 \\",
    "    -s IBM -m \"wField:1:String:This is IBM \"",
    "",
    "Reports from \"./mamasubscribercpp -tport sub\":",
    "Starting Publisher with:",
    " topic:              MAMA_TOPIC",
    " transport:          sub",
    "Created inbound subscription.",
    "Recieved msg.",
    "              MdSeqNum    10              INTEGER                    1",
    "               wSymbol   470               STRING             CTA.MSFT",
    "             MdMsgType     1              INTEGER                   14",
    "           MdMsgStatus     2              INTEGER                    0",
    "         MdSubscSymbol    71               STRING           MAMA_TOPIC",
    "        MdSubscSubject     6               STRING           MAMA_TOPIC",
    "Recieved msg.",
    "              MdSeqNum    10              INTEGER                    1",
    "               wSymbol   470               STRING                  IBM",
    "                wField     1               STRING          This is IBM",
    "         MdSubscSymbol    71               STRING           MAMA_TOPIC",
    "        MdSubscSubject     6               STRING           MAMA_TOPIC",
    "",
    "Usage:",
    "      [-S topic]         The topic on which to send messages. Default value",
    "                         is \"MAMA_TOPIC\".",
    "      [-s symbol]        The symbol name to be published for wSymbol",
    "                         Default value is TEST.SYMBOL",
    "      [-d message]       The contents to be published.",
    "                         Format: FieldName:FieldId:FieldType:Contents",
    "                         Supported types: I32,I64,U32,U64,F32,F64,String.",
    "                         Default message is one with all supported fields",
    "      [-m middleware]    The middleware to use [wmw/lbm/tibrv].",
    "                         Default is wmw.", 
    "      [-i interval]      Publishing interval.",
    "                         Default value is 0.5",
    "      [-tport name]      The transport parameters to be used from",
    "                         mama.properties. Default is pub",
    NULL
};

static void parseCommandLine (int argc, const char **argv);
static void usage            (int exitStatus);
static void parseMessage     (char * msgString, MamaMsg* msg);


class MyHandler : public MamaBasicSubscriptionCallback
                , public MamaTimerCallback
                , public MamaPublisher
{
    // Note: MyHandler does not have to inherit from MamaPublisher.
    // We did this to illustrate that such inheritance is possible and
    // sometimes convenient.
public:
    MyHandler() {}
    virtual ~MyHandler() {}
    virtual void onCreate (MamaBasicSubscription*  subscription);
    virtual void onError  (MamaBasicSubscription*  subscription,
                           const MamaStatus&       status,
                           const char*             symbol);
    virtual void onMsg    (MamaBasicSubscription*  subscription, 
                           MamaMsg&          msg);
    virtual void onTimer  (MamaTimer*  timer);
    void publishMessage   (const MamaMsg*  request);
    void createMessage    ();
};


int main (int argc, const char **argv)
{
    parseCommandLine (argc, argv);
    gBridge = Mama::loadBridge (gMiddleware);
    Mama::open ();
    gDefaultQueue = Mama::getDefaultEventQueue (gBridge);
    MamaTransport          transport;
    MamaTimer              timer;
    MamaBasicSubscription  subscription;
    MyHandler              publisher;

    try
    {
        transport.create (gTransportName, gBridge);
        timer.create     (gDefaultQueue, &publisher, gInterval);
        subscription.createBasic (&transport,
                                  gDefaultQueue,
                                  &publisher,
                                  gInBoundTopic);
        publisher.create (&transport, gOutBoundTopic); 
    }
    catch (MamaStatus &status)
    {
        cerr << "Caught MAMA exception: " << status.toString () << endl;
        exit (1);
    }

    Mama::start (gBridge);
    Mama::close ();
}

void MyHandler::onCreate (
    MamaBasicSubscription*  subscription)
{
    if (gQuietLevel < 2)
    {
        printf ("Created inbound subscription.\n");
    }
}

void MyHandler::onError (MamaBasicSubscription*  subscription,
                         const MamaStatus&       status,
                         const char*             symbol) 
{
    printf ("Error creating subscription: %s\n", 
            status.toString ());
    exit (1);
}

void MyHandler::onMsg (MamaBasicSubscription* subscription, 
                       MamaMsg&               msg)
{
    if (gQuietLevel < 2)
    {
        printf ("Recieved inbound msg. Sending response\n");
    }

    if (!msg.isFromInbox())
    {
        printf ("Inbound msg not from inbox. No reply sent.\n");
        return;
    }

    publishMessage (&msg);
}

void MyHandler::onTimer (MamaTimer *timer)
{
    publishMessage (NULL);
    if (gCount > 0 && --gCount == 0)
    {
        Mama::stop (gBridge);
        return;
    }
}

void MyHandler::publishMessage (const MamaMsg*  request)
{
    static int msgNumber = 0;
    try
    {
        if (request)
        {
            if (gQuietLevel < 1)
            {
                printf ("Publishing message to inbox.\n");
            }
            sendReplyToInbox (*request, gMsg[msgNumber%gNumMsg]);
        }
        else
        {
            if (gQuietLevel < 1)
            {
                printf ("Publishing message to %s.\n", gOutBoundTopic);
            }
            send (gMsg[msgNumber%gNumMsg]);
        }
        gMsg[msgNumber%gNumMsg]->updateI32 (
            "MdSeqNum",
            10,
            gMsg[msgNumber%gNumMsg]->getI32(NULL , 10) + 1);
    }
    catch (MamaStatus &status)
    {
        cerr << "Error publishing message: " 
             << status.toString () << endl;
        exit (1);
    }

    msgNumber ++;
}

void parseMessage (char * msgString, MamaMsg* msg)
{
    char*           fieldName     = NULL;
    char*           fieldIdString = NULL;
    unsigned int    fieldId       = 0;
    char*           fieldType     = NULL;
    char*           value         = NULL;

    fieldName       = strtok (msgString, ":");
    fieldIdString   = strtok (NULL, ":");
    fieldType       = strtok (NULL, ":");
    value           = strtok (NULL, ":");
    fieldId         = atoi   (fieldIdString);

    if (fieldType == NULL)
    {
        return;
    }
    else if (strcmp(fieldType, "I32") == 0)
    {
        int32_t     realValue;
        sscanf      (value, "%d", &realValue);
        msg->addI32 (fieldName, fieldId, realValue); 
    }
    else if (strcmp(fieldType, "I64") == 0)
    {
        int64_t     realValue;
        sscanf      (value, "%lld", &realValue);
        msg->addI64 (fieldName, fieldId, realValue);
    }
    else if (strcmp(fieldType, "U32") == 0)
    {
        uint32_t    realValue;
        sscanf      (value, "%u", &realValue);
        msg->addU32 (fieldName, fieldId, realValue);
    }
    else if (strcmp(fieldType, "U64") == 0)
    {
        uint64_t    realValue;
        sscanf      (value, "%llu", &realValue);
        msg->addU64 (fieldName, fieldId, realValue);
    }
    else if (strcmp(fieldType, "F32") == 0)
    {
        mama_f32_t  realValue;
        sscanf      (value, "%f", &realValue);
        msg->addF32 (fieldName, fieldId, realValue);
    }
    else if (strcmp(fieldType, "F64") == 0)
    {
        mama_f64_t  realValue;
        sscanf      (value, "%lf", &realValue);
        msg->addF64 (fieldName, fieldId, realValue);
    }
    else if (strcmp(fieldType, "String") == 0)
    {
        msg->addString (fieldName, fieldId, value);
    }
    else
    {
        printf("not supported field type %s\n", fieldType);
    }
}

void parseCommandLine (int argc, const char **argv)
{
    int i = 0;
    bool ifUseDefaultMsg = true;
    bool ifUseDefaultSym = true;

    if (argc == 1)
    {
        usage(0);
    }
    for (i = 1; i < argc;)
    {
        if (strcmp ("-S", argv[i]) == 0)
        {
            gOutBoundTopic = argv[i+1];
            i += 2;
        }
        else if (strcmp ("-s", argv[i]) == 0)
        {
            ifUseDefaultSym = false;
            gSymbol = argv[i+1];
            i += 2;
            gNumMsg++;
            gMsg[gNumMsg - 1] = new MamaMsg;
            gMsg[gNumMsg - 1]->create ();
            gMsg[gNumMsg - 1]->addI32 ("MdSeqNum", 10, 1);
        }
        else if (strcmp ("-l", argv[i]) == 0)
        {
            gInBoundTopic = argv[i+1];
            i += 2;
        }
        else if (strcmp ("-c", argv[i]) == 0)
        {
            gCount = atoi (argv[i+1]);
            i += 2;
        }
        else if (strcmp ("-i", argv[i]) == 0)
        {
            gInterval = strtod (argv[i+1], NULL);
            i += 2;
        }
        else if ((strcmp(argv[i], "-h") == 0) ||
                 (strcmp(argv[i], "-?") == 0))
        {
            usage(0);
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
        else if (strcmp ("-d", argv[i]) == 0)
        {
            char temp[1024] = "";
            int k = 0;
            i++;
            ifUseDefaultMsg = false;
            if (gSymbol != NULL)
            {
                gMsg[gNumMsg - 1]->addString ("wSymbol", 470, gSymbol);
            }
            for ( k = 0; k + i < argc && * (argv[i+k]) != '-'; k++)
            {
                if (k >= MAX_NUM_MSGS)
                {
                    printf("Too many msgs! Max is %d\n", MAX_NUM_MSGS);
                }
                strcpy(temp, argv[i+k]);
                parseMessage(temp, gMsg[gNumMsg - 1]);
            }
            i += k;
        }
        else if ( strcmp( argv[i], "-v" ) == 0 )
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
    }
    
    if (ifUseDefaultSym)
    {
        gSymbol = "TEST.SYMBOL";
        gNumMsg++;
        gMsg[gNumMsg - 1] = new MamaMsg;
        gMsg[gNumMsg - 1]->create ();
        gMsg[gNumMsg - 1]->addI32 ("MdSeqNum", 10, 1);
    }
    if (ifUseDefaultMsg)
    {
        gMsg[gNumMsg - 1]->addI64 ("wFieldI64", 1, 1);
        gMsg[gNumMsg - 1]->addU32 ("wFieldU32", 2, 1);
        gMsg[gNumMsg - 1]->addU64 ("wFieldU64", 3, 1);
        gMsg[gNumMsg - 1]->addF32 ("wFieldF32", 4, 1234.5678);
        gMsg[gNumMsg - 1]->addF64 ("wFieldF64", 5, 1234.5678);
        gMsg[gNumMsg - 1]->addString ("wSymbol", 470, gSymbol);
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
    int i = 0;
    while(gUsageString[i] != NULL)
    {
        printf("%s\n", gUsageString[i++]);
    }
    exit(exitStatus);
}
