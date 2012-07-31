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

/*----------------------------------------------------------------------------
 *
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
 *      [-c number]        How many messages to publish (default infinite).
 *
 *
 *---------------------------------------------------------------------------*/

#include <iostream>

#include "mama/mama.h"
#include "mama/mamacpp.h"

#ifdef _WIN32
    #include <windows.h>
    inline void my_sleep(unsigned int sec)
    {
        Sleep(sec);
    }

#else
    #include <unistd.h>
    inline void my_sleep(unsigned int sec)
    {
        sleep(sec);
    }
#endif

using namespace Wombat;

using std::cerr;
using std::cout;
using std::endl;


static int           gCount         = 0;
static const char *  gOutBoundTopic = "MAMA_TOPIC";
static const char *  gInBoundTopic  = "MAMA_INBOUND_TOPIC";
static const char *  gTransportName = "pub";
static const char *  gMiddleware    = "wmw";
static mamaBridge    gBridge        = NULL;
static MamaQueue*    gDefaultQueue  = NULL;
static double        gInterval      = 0.5;
static int           gQuietLevel    = 0;
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
    "      [-i interval]      The interval between messages .Default, 0.5.",
    "      [-m middleware]    The middleware to use [wmw/lbm/tibrv]. Default is wmw.",
    "      [-tport name]      The transport parameters to be used from",
    "                         mama.properties. Default is pub",
    "      [-q]               Quiet mode. Suppress output.",
    NULL
};

static int	msgNumber	= 0;

static void parseCommandLine (int argc, const char **argv);
static void usage            (int exitStatus);


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

    virtual void onDestroy(MamaBasicSubscription* subscription, void * closure);

    virtual void onDestroy(MamaTimer* timer, void* closure);

    void publishMessage   (const MamaMsg*  request);
};


int main (int argc, const char **argv)
{
    parseCommandLine (argc, argv);
    gBridge = Mama::loadBridge (gMiddleware);

    Mama::open ();

    MamaTransport          transport;
    MamaTimer              timer;
    MamaBasicSubscription  subscription;
    MyHandler              publisher;

    try
    {
        gDefaultQueue = Mama::getDefaultEventQueue (gBridge);
        transport.create (gTransportName, gBridge);
        timer.create (gDefaultQueue, &publisher, gInterval);
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

void MyHandler::onCreate (MamaBasicSubscription*  subscription)
{
    if (gQuietLevel < 2)
    {
        printf ("Created inbound subscription.\n");
    }
}

void MyHandler::onDestroy (MamaBasicSubscription*  subscription, void * closure)
{
    if (gQuietLevel < 2)
    {
        printf ("Destroyed subscription.\n");
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
        printf ("Received inbound msg. Sending response\n");
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
    if (msgNumber > 12 )
    {  
        timer->destroy();  
        my_sleep(1);
        Mama::stop (gBridge);
        return;
    }
}

void MyHandler::onDestroy (MamaTimer *timer, void* closure)
{
    printf ("Timer destroyed\n");
}

void MyHandler::publishMessage (const MamaMsg*  request)
{
    static MamaMsg* msg = NULL;

    try
    {
		if (msg == NULL)
		{
			msg = new MamaMsg();
			msg->create();
		}
		else
		{
        	msg->clear();
		}

        /* Add some fields. This is not required, but illustrates how to
         * send data.
         */
 
        switch(msgNumber)
        {
         case 0 :
             msg->addString ("PublisherTopic", msgNumber+1, gOutBoundTopic);
             break;
         case 1 :
             msg->addF32("f32", msgNumber+1, 1.23456);     
             break;
         case 2 :
             msg->addF64("f64", msgNumber+1 ,123.456789);
             break;
         case 3 :
             msg->addI8("i8", msgNumber+1, -126);
             break;
         case 4 :
            msg->addI16("i16", msgNumber+1, 32766);
            break;
         case 5 :
            msg->addI32("i32", msgNumber+1, 2147483647);
            break;
         case 6 :
            msg->addI64("i64", msgNumber+1, 4294967297);
            break;
         case 7 :
            msg->addU8("u8", msgNumber+1, 255);
            break;
         case 8 :
            msg->addU16("u16", msgNumber+1, 55000);
            break;
         case 9 :
            msg->addU32("u32", msgNumber+1, 4294967295);
            break;
         case 10 :
            msg->addU64("u64", msgNumber+1, 5294967297);
            break;
         case 11 :
            msg->addChar("char", msgNumber+1, 'c');
         break;
         case 12 :
            msg->addString("EXIT", msgNumber+1,"EXIT");
            break;
  
        }
        

        if (request)
        {
            if (gQuietLevel < 1)
            {
                printf ("Publishing message to inbox.\n");
            }
            sendReplyToInbox (*request, msg);
        }
        else
        {
            if (gQuietLevel < 1)
            {
                printf ("Publishing message to %s", gOutBoundTopic);
                printf ("\tmsg=%s\n", msg->toString());
                fflush(stdout);
            }
            send (msg);
        }
    msgNumber++;
    }
    catch (MamaStatus &status)
    {
        cerr << "Error publishing message: "
             << status.toString () << endl;
        exit (1);
    }
}


void parseCommandLine (int argc, const char **argv)
{
    int i = 0;
    for (i = 1; i < argc;)
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
        else if ((strcmp(argv[i], "-h") == 0)||
                 (strcmp(argv[i], "-?") == 0))
        {
            usage(0);
        }
        else if (strcmp ("-tport", argv[i]) == 0)
        {
            gTransportName = argv[i+1];
            i += 2;
        }
        else if (strcmp ("-m", argv[i]) == 0)
        {
            gMiddleware = argv[i+1];
            i += 2;
        }
        else if (strcmp ("-q", argv[i]) == 0)
        {
            gQuietLevel++;
            i++;
        }
        else if (strcmp( argv[i], "-v") == 0 )
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
        printf("%s\n",gUsageString[i++]);
    }
    exit(exitStatus);
}
