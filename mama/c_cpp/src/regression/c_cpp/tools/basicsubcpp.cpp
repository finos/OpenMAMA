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
 *      [-s topic]         The topic to which to subscribe. Default value
 *                         is "MAMA_INBOUND_TOPIC".
 *      [-tport name]      The transport parameters to be used from
 *                         mama.properties. 
 *      [-q]               Quiet mode. Suppress output.
 *
 *
 *---------------------------------------------------------------------------*/

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

#include <iostream>

#include "mama/mama.h"
#include "mama/mamacpp.h"
#include "mama/MamaBasicSubscription.h"

using namespace Wombat;

using std::cerr;
using std::cout;
using std::endl;


static MamaTransport*            gTransport    = NULL;
static MamaBasicSubscription*    gSubscription = NULL;

static int           gWildcard                 = 0;
static int           gTport                    = 0;
static const char*   gMuteTopic                = NULL;

static const char *  gTopic                    = "MAMA_TOPIC";
static const char *  gTransportName            = "sub";
static int           gQuietLevel               = 0;
static MamaLogLevel  gSubscLogLevel            = MAMA_LOG_LEVEL_NORMAL;
static const char *  gMiddleware               = "wmw";
static mamaBridge    gBridge                   = NULL;
static MamaQueue*    gDefaultQueue             = NULL;
static const char *  gUsageString[]            =
{
    "This sample application demonstrates how to subscribe and process",
    "mamaMsg's from a basic subscription.",    
    "",
    " It accepts the following command line arguments:",
    "      [-s topic]         The topic to which to subscribe. Default value",
    "                         is \"MAMA_INBOUND_TOPIC\".",
    "      [-tport name]      The transport parameters to be used from",
    "                         mama.properties. Default is sub",
    "      [-m middleware]    The middleware to use [wmw/lbm/tibrv]. Default", 
    "                         is wmw.", 
    "      [-q]               Quiet mode. Suppress output.",
    NULL
};

static int	msgNumber	= 0;

static void parseCommandLine            (int argc, const char **argv);
static void initializeMama              (void);
static void createSubscriber            (void);
static void createWildCardSubscriber    (void);
static void usage                       (int exitStatus);

int main (int argc, const char**  argv)
{
    parseCommandLine (argc, argv);
    
    initializeMama ();
    if (gWildcard || gTport)
    {
        createWildCardSubscriber ();
    }
    else
    {
        createSubscriber ();
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

class  DisplayIterator : public MamaMsgFieldIterator
{
public:
    virtual ~DisplayIterator (void) {}

    virtual void onField (const MamaMsg&       msg,
                          const MamaMsgField&  field,
                          void*                closure) 
    {

        char        value[256]= "";

        const char *typeName  = "";
        const char *fieldName = "";
        int        fid        = 0;

        try
        {
            typeName  = field.getTypeName ();
            fieldName = field.getName ();
            fid       = field.getFid ();

            if (fieldName == NULL)
            {
                fieldName = "(null)";
            }
            if (typeName == NULL)
            {
                typeName = "(null)";
            }

            field.getAsString (value, 256);

            printf ("    %20s %5d %20s %20s\n", fieldName, fid, typeName, value);
            fflush(stdout);
            if(strcmp(value,"EXIT")==0)
            {
                exit(0);
            }
        }
        catch (MamaStatus &status)
        {
            printf ("Failed to get field: %d:%s\n", fid, fieldName);
            return;
        }

    }
};

class WildCardSubscriptionCallback : public MamaBasicWildCardSubscriptionCallback
{
public:
    virtual ~WildCardSubscriptionCallback () {}

    virtual void onCreate (MamaBasicWildCardSubscription*  subscription)
    {
        if (gQuietLevel < 2)
        {
            printf ("Created inbound subscription.\n");
        }
    }

     virtual void onDestroy (MamaBasicWildCardSubscription*  subscription, void * closure)
    {
        if (gQuietLevel < 2)
        {
            printf ("Destroyed subscription.\n");
        }
    }
   

    virtual void onError (MamaBasicWildCardSubscription*  subscription,
                          const MamaStatus&               status,
                          const char*                     subject) 
    {
        printf ("Error creating subscription: %s\n", 
                status.toString ());
        exit (1);
    }

    virtual void onMsg (MamaBasicWildCardSubscription* subscription, 
                        MamaMsg&                       msg, 
                        const char*                    topic)
    {
        static int i = 0;
        void* closure;

        if (gMuteTopic)
        {
            const char* topic = msg.getString ("PublisherTopic", 10002);
            if (0 == strncmp (topic, gMuteTopic, strlen(topic)))
            {
                gMuteTopic = NULL;
                printf ("Mutting %s\n", topic);
                subscription->muteCurrentTopic ();
                return;
            }
        }

        if ((closure = subscription->getTopicClosure()) == NULL)
        {
            subscription->setTopicClosure ((void*)(++i));
        }

        if (gQuietLevel < 2)
        {
            printf ("Received WildCard msg on %s with closure %p  msg.\n",
                topic, closure);
        }

        DisplayIterator it;
        msg.iterateFields (it, NULL, NULL);
    }
};

class SubscriptionCallback : public MamaBasicSubscriptionCallback
{
public:
    virtual ~SubscriptionCallback () {}

    virtual void onCreate (MamaBasicSubscription*  subscription)
    {
        if (gQuietLevel < 2)
        {
            printf ("Created inbound subscription.\n");
        }
    }

    virtual void onError (MamaBasicSubscription*  subscription,
                          const MamaStatus&       status,
                          const char*             subject) 
    {
        printf ("Error creating subscription: %s\n", 
                status.toString ());
        exit (1);
    }

    virtual void onMsg (MamaBasicSubscription* subscription, 
                        MamaMsg&               msg)
    {
        if (gQuietLevel < 2)
        {
            printf ("Received msg.\n");
        }

        DisplayIterator it;
        msg.iterateFields (it, NULL, NULL);
    }

    //JG: need to add in onDestroy as this is abstract and must be implemented
    virtual void onDestroy (MamaBasicSubscription*  subscription, void * closure)
    {
        if (gQuietLevel < 2)
        {
            printf ("Destroyed subscription.\n");
        }
    }

};

static void createSubscriber (void)
{
    try
    {
        gSubscription = new MamaBasicSubscription;
        gSubscription->createBasic (gTransport,
                                    gDefaultQueue,
                                    new SubscriptionCallback (),
                                    gTopic);
        gSubscription->setDebugLevel (gSubscLogLevel);
    }
    catch (MamaStatus &status)
    {
        cerr << "Error creating subscription: " 
             << status.toString () << endl;
        exit (1);
    }
}

static void createWildCardSubscriber (void)
{
    const char* source = gTport    ? gTopic : NULL;
    const char* symbol = gWildcard ? gTopic : NULL;

    try
    {
        gSubscription = new MamaBasicWildCardSubscription;
        ((MamaBasicWildCardSubscription*)(gSubscription))->create (
                            gTransport,
                            gDefaultQueue,
                            new WildCardSubscriptionCallback (),
                            source,
                            symbol);
        gSubscription->setDebugLevel (gSubscLogLevel);
    }
    catch (MamaStatus &status)
    {
        cerr << "Error creating subscription: " 
             << status.toString () << endl;
        exit (1);
    }
}




void parseCommandLine (int argc, const char**  argv)
{
    int i = 0;
    for (i = 1; i < argc;)
    {
        if (strcmp ("-s", argv[i]) == 0)
        {
            gTopic = argv[i+1];
            i += 2;
        }
        else if ((strcmp(argv[i], "-h") == 0)||
                 (strcmp(argv[i], "-?") == 0))
        {
            usage(0);
        }
        else if (strcmp("-tport", argv[i]) == 0)
        {
            gTransportName = argv[i+1];
            i += 2;
        }
        else if (strcmp ("-q", argv[i]) == 0)
        {
            gQuietLevel++;
            i++;
        }
        else if (strcmp(argv[i], "-v") == 0)
        {
            if ( Mama::getLogLevel () == MAMA_LOG_LEVEL_WARN)
            {
            	Mama::enableLogging (MAMA_LOG_LEVEL_NORMAL, stderr);
            }
            else if ( Mama::getLogLevel () == MAMA_LOG_LEVEL_NORMAL)
            {
            	Mama::enableLogging (MAMA_LOG_LEVEL_FINE, stderr);
            }
            else if(  Mama::getLogLevel () == MAMA_LOG_LEVEL_FINE )
            {
                Mama::enableLogging (MAMA_LOG_LEVEL_FINER, stderr);
            }
            else
            {
                Mama::enableLogging (MAMA_LOG_LEVEL_FINEST, stderr);
            }
            i++;
        }
        else if (strcmp ("-m", argv[i]) == 0)
        {
            gMiddleware = argv[i+1];
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
        else if (strcmp (argv[i], "-w") == 0)
        {
            if (gTport) 
            {
                printf ("A subscription can only be wildcard or "
                        "tport not both\n");
                exit(1);
            }
            gWildcard = 1;
            i++;
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
        else if (strcmp ("-mute", argv[i]) == 0)
        {
            gMuteTopic = argv[i+1];
            i += 2;               
        }
    }

    if (gQuietLevel < 2)
    {
        printf ("Starting Publisher with:\n"
                "   topic:              %s\n"
                "   transport:          %s\n",
                gTopic, gTransportName);
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

