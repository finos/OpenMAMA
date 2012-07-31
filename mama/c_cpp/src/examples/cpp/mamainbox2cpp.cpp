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

/**
 * This example program shows how the MAMA inbox and timer operate. It will
 * create an inbox and send a request. The example program will terminate whenever
 * a reply is received or a timeout elapses.
 *
 * Some important things to note:
 *  1. All of the objects are created on the MAMA default queue, this means that
 *     once mama_start is called all access to member variables will be from only
 *     one thread which will be the MAMA thread that dispatches from the default
 *     queue. In more advanced programs locking around member variables may be
 *     required.
 *  2. This example makes use of the overloaded create functions for the timer
 *     and the inbox which notify the caller whenever those objects are destroyed.
 *     The application will not shutdown until all these objects have been cleaned
 *     up. Note that if the mama_close or mamaQueue_destroy functions are called
 *     whenever there are open objects then the thread will block and dispatch
 *     messages from the queue until all objects have been fully destroyed.
 *  3. To obtain a list of the parameters supported by this program run it with
 *     no command line arguments or using --help.
 */

/* *************************************************** */
/* Includes. */
/* *************************************************** */
#include <iostream>
#include "mama/mama.h"
#include "mama/mamacpp.h"

/* *************************************************** */
/* Namespaces. */
/* *************************************************** */
using std::cerr;
using std::cout;
using std::endl;
using namespace Wombat;

/* *************************************************** */
/* Structures and classes. */
/* *************************************************** */

/**
 * This structure parses the command line options and exposes them as a series
 * of public member variables.
 */
struct CommandLineParser
{
    /* *************************************************** */
    /* Public Member Variables. */
    /* *************************************************** */
    
    // The topic to subscribe to
    const char *topic;

    // The name of the transport from mama.properties
    const char *transportName;

    // The name of the middleware
    const char *middleware;

    // The quietness leve
    int quietLevel;

    // The timeout value
    mama_f64_t timeout;

    /* *************************************************** */
    /* Construction. */
    /* *************************************************** */

    CommandLineParser(void)
    {
        // Set up the default parameter values
        topic           = "MAMA_INBOUND_TOPIC";
        transportName   = "sub";
        middleware      = "wmw";
        quietLevel      = 0;
        timeout         = 10;
    }

    /* *************************************************** */
    /* Public Functions. */
    /* *************************************************** */
        
    bool parse(int argc, const char **argv)
    {
        // Returns
        bool ret = true;

        // Check each command line parameter in turn        
        for (int i = 1; i < argc;)
        {
            if (strcmp ("-s", argv[i]) == 0)
            {
                topic = argv[i+1];
                i += 2;
            }
            else if (strcmp ("-tport", argv[i]) == 0)
            {
                transportName = argv[i+1];
                i += 2;
            }
            else if (strcmp ("-q", argv[i]) == 0)
            {
                quietLevel++;
                i++;
            }
            else if (strcmp ("-t", argv[i]) == 0)
            {
                int timeoutMs = atoi(argv[i+1]);
                timeout = (mama_f64_t)((mama_f64_t)timeoutMs / 1000);
                i += 2;
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
                middleware = argv[i+1];
                i += 2;               
            }
            else if ((strcmp(argv[i], "-?") == 0) ||
                     ((strcmp(argv[i], "--help") == 0)))
            {
                /* Print the usage string. */
                usage();
                ret = false;
                break;
            }
            else 
            {
                cout << "Unrecognized parameter: <" << argv[i] << ">" << endl;
                i++;
            }
        }

        if (quietLevel < 2)
        {
            printf("Starting Publisher with:\n\ttopic:\t\t%s\n\ttransport:\t%s\n", topic, transportName);
        }

        return ret;
    }

    void usage(void)
    {
        /* The usage string is displayed if no command line parameters are specified. */
        static const char *usageString[]  =
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
            "      [-t timeout]       The timeout after which the program will terminate, (ms).",
            "      [-?, --help]       Print this usage information and exit.",
            "",
            NULL
        };

        // Print the entire usage string line by line.
        int nextLine = 0;
        while(usageString[nextLine]!=NULL) 
        {
            cout << usageString[nextLine++] << endl;
        }    
    }
};

/**
 * This class encapsulates an inbox request and incorporates both an inbox and a timeout
 * timer.
 */
class InboxRequest : public MamaInboxCallback, public MamaTimerCallback
{
    /* *************************************************** */
    /* Private Member Variables. */
    /* *************************************************** */
private:
    // The mama bridge
    mamaBridge mBridge;

    // The inbox
    MamaInbox mInbox;

    // Flag indicates if the inbox has been destroyed
    bool mInboxDestroyed;

    // The mama queue that the objects will be created on
    MamaQueue *mQueue;
    
    // The timeout timer
    MamaTimer mTimeoutTimer;

    // Flag indicates if the timer has been destroyed
    bool mTimerDestroyed;

    // The mama transport
    MamaTransport *mTransport;

    /* *************************************************** */
    /* Private Functions. */
    /* *************************************************** */

    void checkDestroy(void)
    {
        /* Only quit out if both the timer and the inbox have been destroyed. */
        if(mTimerDestroyed && mInboxDestroyed)
        {
            /* Stop mama to unblock the main function. */
            Mama::stop(mBridge);
        }  
    }

public:

    /* *************************************************** */
    /* Construction and Destruction. */
    /* *************************************************** */

    InboxRequest(mamaBridge bridge, MamaQueue *queue, MamaTransport *transport)
    {
        // Save arguments in member variables
        mBridge    = bridge;
        mQueue     = queue;
        mTransport = transport;

        // Reset the flags
        mInboxDestroyed = false;
        mTimerDestroyed = false;
    }
   
    void create(mama_f64_t timeout)
    {
        // Create the inbox
        mInbox.create(mTransport, mQueue, this);

        // Create the timeout timer
        mTimeoutTimer.create(mQueue, this, timeout);                         
    }

    void sendRequest(const char *topic)
    {        
        // Create a publisher
        MamaPublisher publisher;
        publisher.create(mTransport, topic);
        
        // Create a mama message
        MamaMsg message;        
        message.create();
            
        // Add a field
        message.addI32("field", 1, 32);

        // Reset the timeout timer just before the request is sent
        mTimeoutTimer.reset();

        // Send the message
        publisher.sendFromInbox(&mInbox, &message);        
    }

    virtual void onDestroy(MamaInbox *inbox, void *closure)
    {
        // Flag that the inbox has been destroyed
        mInboxDestroyed = true;

        // Shutdown if all objects have been destroyed
        checkDestroy();
    }

    virtual void onDestroy(MamaTimer *timer, void *closure)
    {
        // Flag that the timer has been destroyed
        mTimerDestroyed = true;

        // Shutdown if all objects have been destroyed
        checkDestroy();
    }

    virtual void onMsg(MamaInbox *inbox, MamaMsg& msg)
    {
        // Write out an error message
        cout << "Received reply: " << msg.toString() << endl;
        
        // Now that the reply has been received destroy both the inbox and the timer.
        mInbox.destroy();
        mTimeoutTimer.destroy();
    }

    virtual void onError(MamaInbox *inbox, const MamaStatus &status)
    {
        // Write out an error message
        cout << "Error creating inbox: " << status.toString() << endl;

        // Destroy both the timer and the inbox
        mInbox.destroy();
        mTimeoutTimer.destroy();
    }

    virtual void onTimer(MamaTimer* timer)
    {
        /* Write out a message. */
        cout << "The request has timeout out." << endl;

        // A reply has not been received in time, both the inbox and timer must now be destroyed.
        mInbox.destroy();
        mTimeoutTimer.destroy();
    }
};

/**
 * The main function.
 */
int main (int argc, const char **argv)
{
    try
    {
        // Parse the command line arguments, this will simply save them into global variables
        CommandLineParser parser;
        bool argsValid = parser.parse(argc, argv);
        if(argsValid)
        {
            // Load the mama bridge
            mamaBridge bridge = Mama::loadBridge(parser.middleware);

            // Open mama
            Mama::open();
            try
            {
                // Create the transport
                MamaTransport transport;
                transport.create(parser.transportName, bridge);
                
                // The mama default queue will be used to create all objects
                MamaQueue *defaultQueue = Mama::getDefaultEventQueue(bridge);

                // Create the inbox class that will actually send the request
                InboxRequest request(bridge, defaultQueue, &transport);
                request.create(parser.timeout);

                // Send the inbox request
                request.sendRequest(parser.topic);

                /* Start MAMA, this function will block and not return until either a reply has been received or
                 * the timeout elapses. In either case both timer and inbox will be destroyed before this function
                 * returns.
                 */
                Mama::start(bridge);                
            }

            catch(...)
            {
                // Close mama
                Mama::close();

                // Rethrow the exception
                throw;
            }

            // Close mama
            Mama::close();
        }
    }

    catch(MamaStatus mamaException)
    {
        // Write out an error message
        cout << "An error has occurred running this example program, " << mamaException.toString() << endl;
    }

    return 0;
}
