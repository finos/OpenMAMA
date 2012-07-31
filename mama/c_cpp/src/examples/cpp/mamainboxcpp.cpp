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
 * This sample application demonstrates how to send mamaMsg's from an inbox,
 * and receive the reply.
 *
 * It accepts the following command line arguments: 
 *      [-s topic]         The topic on which to send the request. Default value
 *                         is "MAMA_INBOUND_TOPIC".
 *      [-tport name]      The transport parameters to be used from
 *                         mama.properties. 
 *      [-q]               Quiet mode. Suppress output.
 *
 *
 *---------------------------------------------------------------------------*/

#include <iostream>
using std::cerr;
using std::cout;
using std::endl;

#include "mama/mama.h"
#include "mama/mamacpp.h"

using namespace Wombat;

static MamaTransport*       gTransport      = NULL;
static MamaInbox*           gInbox          = NULL;
static MamaPublisher*       gPublisher      = NULL;
static mamaBridge           gBridge         = NULL;
static MamaQueue*           gDefaultQueue   = NULL;

static const char *         gTopic          = "MAMA_INBOUND_TOPIC";
static const char *         gTransportName  = "sub";
static const char *         gMiddleware     = "wmw";
static int                  gQuietLevel     = 0;

static void parseCommandLine    (int argc, const char **argv);
static void initializeMama      (void);
static void createInbox         (void);
static void createPublisher     (void);
static void sendRequest         (void);

int main (int argc, const char **argv)
{
    int i = 0; 
    parseCommandLine (argc, argv);

    initializeMama ();
    createPublisher ();
    createInbox ();

    for (i=0; i<1000; i++)
    {
        sendRequest ();
    }

    Mama::start (gBridge);
}

void initializeMama (void)
{
    try
    {
        gBridge = Mama::loadBridge (gMiddleware);
        
        Mama::open ();

        gDefaultQueue = Mama::getDefaultEventQueue (gBridge);

        gTransport = new MamaTransport;
        gTransport->create (gTransportName, gBridge);
    }
    catch (MamaStatus &status)
    {
        cerr << "Error initializing mama or creating transport: " 
             << status.toString () << endl;
        exit (1);
    }
}

class MySendCompleteCb : public MamaSendCompleteCallback
{
    void onSendComplete (MamaPublisher& publisher,
                         MamaMsg*       msg,
                         MamaStatus&    status,
                         void*          closure)
    {
        delete msg;
        return;
    }
};

/*Single global instance for all send callbacks.*/
MySendCompleteCb sendCb;

static void sendRequest (void)
{
    try
    {
        MamaMsg* msg = new MamaMsg;
        msg->create ();

        /* WombatMsg does not like empty messages */
        msg->addI32 ("field", 1, 32);

        gPublisher->sendFromInboxWithThrottle (gInbox, msg, &sendCb, NULL);
    }
    catch (MamaStatus &status)
    {
        cerr << "Error sending request: " 
             << status.toString () << endl;
        exit (1);
    }
}

class Cb : public MamaInboxCallback
{
public:
    virtual ~Cb (void) {};

    virtual void onMsg (MamaInbox *inbox, MamaMsg& msg)
    {
        printf ("Received reply\n" );
    }

    virtual void onError (MamaInbox *inbox, const MamaStatus &status)
    {
        printf ("Error creating inbox: %s\n", 
                status.toString ());
        exit (1);
    }

    virtual void onDestroy (MamaInbox *inbox, void * closure)
    {
        printf ("Destroying Inbox\n");
    }
};

static void createInbox (void)
{
    try
    {
        gInbox = new MamaInbox;
        gInbox->create  (gTransport,
                         gDefaultQueue,
                         new Cb ());
    }
    catch (MamaStatus &status)
    {
        cerr << "Error creating inbox: " 
             << status.toString () << endl;
        exit (1);
    }
}


static void createPublisher (void)
{
    try
    {
        gPublisher = new MamaPublisher;
        gPublisher->create (gTransport, gTopic); 
    }
    catch (MamaStatus &status)
    {
        cerr << "Error creating publisher: " 
             << status.toString () << endl;
        exit (1);
    }
}

void parseCommandLine (int argc, const char **argv)
{
    int i = 0;
    for (i = 1; i < argc; )
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
        else if (strcmp ("-m", argv[i]) == 0)
        {
            gMiddleware = argv[i+1];
            i += 2;               
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

    if (gQuietLevel < 2)
    {
        printf ("Starting Publisher with:\n"
                "   topic:              %s\n"
                "   transport:          %s\n",
                gTopic == NULL ? "(null)":gTopic, gTransportName == NULL ? "(null)":gTransportName);
    }
}
