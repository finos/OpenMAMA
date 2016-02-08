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
#include <stdio.h>

#include "mama/mama.h"
#include "mama/mamacpp.h"

using namespace Wombat;

using std::cerr;
using std::cout;
using std::endl;

static int           gCount         = 0;
static const char *  gSource        = "";
static const char *  gOutBoundTopic = "MAMA_TOPIC";
static const char *  gInBoundTopic  = "MAMA_INBOUND_TOPIC";
static const char *  gTransportName = "pub";
static const char *  gMiddleware    = "wmw";
static double        gInterval      = 0.5;
static int           gQuietLevel    = 0;
static int           gPubCb         = 1;
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
    "      [-pubCb]           Listen for publisher callbacks",
    NULL
};

static void parseCommandLine (int argc, const char **argv);
static void usage            (int exitStatus);


class MamaPublisherSample : public MamaBasicSubscriptionCallback
                , public MamaTimerCallback
                , public MamaPublisherCallback
{
public:
    MamaPublisherSample();
    virtual ~MamaPublisherSample() {}

    virtual void run();

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

    // Publisher Callbacks
    void onCreate (
        MamaPublisher* publisher,
        void* closure);

    void onDestroy (
        MamaPublisher* publisher,
        void* closure);

    void onError (
        MamaPublisher* publisher,
        const MamaStatus& status,
        const char* info,
        void* closure);

private:
    int msgNumber;

    mamaBridge      mBridge;
    MamaQueue*      mDefaultQueue;
    MamaTransport*  mTransport;
    MamaTimer*      mTimer;
    MamaBasicSubscription*  mSubscription;
    MamaQueueGroup* mQueueGroup;
    MamaMsg*        mMsg;
    MamaPublisher*  mPublisher;
};

MamaPublisherSample::MamaPublisherSample() :
    msgNumber(1),
    mBridge(NULL),
    mDefaultQueue(NULL),
    mTransport(NULL),
    mTimer(NULL),
    mSubscription(NULL),
    mQueueGroup(NULL),
    mMsg(NULL),
    mPublisher(NULL)
{
}

void MamaPublisherSample::run()
{
    try
    {
        mBridge = Mama::loadBridge (gMiddleware);

        Mama::open ();

        mDefaultQueue = Mama::getDefaultEventQueue (mBridge);

        mMsg = new MamaMsg();
        mMsg->create();

        mTransport = new MamaTransport();
        mTransport->create (gTransportName, mBridge);

        mTimer = new MamaTimer();
        mTimer->create (mDefaultQueue, this, gInterval);

        mQueueGroup = new MamaQueueGroup(1, mBridge);

        mSubscription = new MamaBasicSubscription();
        mSubscription->createBasic (mTransport,
                                    mDefaultQueue,
                                    this,
                                    gInBoundTopic);

        mPublisher = new MamaPublisher();
        if (gPubCb)
        {
            mPublisher->createWithCallbacks (mTransport,
                                             mQueueGroup->getNextQueue(),
                                             this,
                                             NULL,
                                             gOutBoundTopic,
                                             gSource,
                                             NULL);
        }
        else
        {
            mPublisher->create (mTransport, gOutBoundTopic);
        }
    }
    catch (MamaStatus &status)
    {
        cerr << "Caught MAMA exception: " << status.toString () << endl;
        exit (1);
    }

    Mama::start (mBridge);

    Mama::close ();
}

void MamaPublisherSample::onCreate (MamaBasicSubscription*  subscription)
{
    if (gQuietLevel < 2)
    {
        printf ("Created inbound subscription.\n");
    }
}

void MamaPublisherSample::onDestroy (MamaBasicSubscription*  subscription, void * closure)
{
    if (gQuietLevel < 2)
    {
        printf ("Destroyed subscription.\n");
    }
}

void MamaPublisherSample::onError (MamaBasicSubscription*  subscription,
                         const MamaStatus&       status,
                         const char*             symbol)
{
    printf ("Error creating subscription: %s\n",
            status.toString ());
    exit (1);
}

void MamaPublisherSample::onMsg (MamaBasicSubscription* subscription,
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

void MamaPublisherSample::onTimer (MamaTimer *timer)
{
    publishMessage (NULL);
    if (gCount > 0 && --gCount == 0)
    {
        /* All done, destroy the publisher and wait for destroy callback */
        mPublisher->destroy();
        sleep (1);    /* to see all queued events */
        Mama::stop (mBridge);
    }
}

void MamaPublisherSample::onDestroy (MamaTimer *timer, void* closure)
{
    printf ("Timer destroyed\n");
}

void MamaPublisherSample::publishMessage (const MamaMsg*  request)
{
    try
    {
        mMsg->clear();

        /* Add some fields. This is not required, but illustrates how to
         * send data.
         */
        mamaMsgType msgType;
        if (msgNumber == 1) msgType = MAMA_MSG_TYPE_INITIAL;
        else msgType = MAMA_MSG_TYPE_UPDATE;

        mMsg->addI32 (MamaFieldMsgType.mName, MamaFieldMsgType.mFid, msgType);
        mMsg->addI32 (MamaFieldMsgStatus.mName, MamaFieldMsgStatus.mFid, MAMA_MSG_STATUS_OK);
        mMsg->addI32 (MamaFieldSeqNum.mName, MamaFieldSeqNum.mFid, msgNumber);

        mMsg->addString ("MdFeedHost", 12, gOutBoundTopic);

        if (request)
        {
            if (gQuietLevel < 1)
            {
                mama_log (MAMA_LOG_LEVEL_NORMAL, "Publishing message to inbox");
            }
            mPublisher->sendReplyToInbox (*request, mMsg);
        }
        else
        {
            if (gQuietLevel < 1)
            {
                mama_log (MAMA_LOG_LEVEL_NORMAL, "Publishing message %d to %s", msgNumber, gOutBoundTopic);
            }
            mPublisher->send (mMsg);
        }
        msgNumber++;
    }
    catch (MamaStatus &status)
    {
        cerr << "Error publishing message: " << gOutBoundTopic << " "
             << status.toString () << endl;
        exit (1);
    }
}

void MamaPublisherSample::onCreate (
    MamaPublisher* publisher,
    void* closure)
{
    if (gQuietLevel < 1)
    {
        mama_log(MAMA_LOG_LEVEL_NORMAL, "onPublishCreate: %s.%s",
            publisher->getSource(), publisher->getSymbol());
    }
}

void MamaPublisherSample::onDestroy (
    MamaPublisher* publisher,
       void* closure)
{
    if (gQuietLevel < 1)
    {
        mama_log(MAMA_LOG_LEVEL_NORMAL, "onPublishDestroy: %s.%s",
            publisher->getSource(), publisher->getSymbol());
    }
}

void MamaPublisherSample::onError (
    MamaPublisher* publisher,
    const MamaStatus& status,
    const char* info,
    void* closure)
{
    mama_log(MAMA_LOG_LEVEL_ERROR, "onPublishError: %s.%s %s %s",
        publisher->getSource(), publisher->getSymbol(), status.toString(), info);
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
        else if (strcmp ("-S", argv[i]) == 0)
        {
            gSource = argv[i+1];
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
        else if (strcmp ("-pubCb", argv[i]) == 0)
        {
            gPubCb = 1;
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


int main (int argc, const char **argv)
{
    setbuf (stdout, NULL);
    parseCommandLine (argc, argv);
    MamaPublisherSample* h = new MamaPublisherSample();
    h->run();
    delete h;
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

