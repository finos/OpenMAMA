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
 * This sample application demonstrates how to use mamaIoHandlers. It creates a
 * socket an waits for a connection. Upon accepting a connection it creates a
 * mamaIoHandler to read from the socket and connects back to the client on port
 * 107 (telnet). It echos the users input. Type "quit" to exit.
 *
 * It accepts the following command line arguments: 
 *      [-port number]     The TCP/IP port on which to listen. Defaults to 9998
 *      [-q]               Quiet mode. Suppress output.
 *
 *
 *---------------------------------------------------------------------------*/


#include "wombat/port.h"
#include <iostream>
#include <errno.h>

using std::cerr;
using std::cout;
using std::endl;

#include "mama/mamacpp.h"
#include "mama/io.h"

using namespace Wombat;

static MamaIo *         gReadHandler   = NULL;
static MamaIo *         gWriteHandler  = NULL;
static MamaIo *         gExceptHandler = NULL;

static int              gPort           = 9998;
static int              gQuietLevel     = 0;
static int              gFD             = 0;
static const char *     gMiddleware     = "wmw";
static mamaBridge       gBridge         = NULL;
static MamaQueue*       gDefaultQueue   = NULL;

static void parseCommandLine    (int argc, const char **argv);
static void initializeMama      (void);
static void waitForConection    (void);
static void createIOHandlers    (void);

int main (int argc, const char **argv)
{
    parseCommandLine (argc, argv);

    initializeMama ();

    waitForConection ();

    createIOHandlers ();

    Mama::start (gBridge);

    if (gQuietLevel < 2)
    {
        printf ("Cleainging up FD = %d\n", gReadHandler->getDescriptor ());
    }

    gReadHandler->destroy ();
    delete gReadHandler;

    gWriteHandler->destroy ();
    delete gWriteHandler;

    gExceptHandler->destroy ();
    delete gExceptHandler;

    Mama::close ();

    shutdown (gFD, SHUT_RDWR);
}

static void  waitForConection (void)
{
    struct sockaddr_in myAddr; 
    struct sockaddr_in theirAddr;
    int s;

    socklen_t   len = sizeof (theirAddr);

    memset (&myAddr, 0, sizeof (myAddr));

    myAddr.sin_family        = AF_INET;
    myAddr.sin_port          = htons (gPort);
    myAddr.sin_addr.s_addr   = htonl (INADDR_ANY);


    if (gQuietLevel < 2)
    {
        printf ("Creating Socket on port %d\n", gPort);
    }
    
    s = socket (PF_INET, SOCK_STREAM, 0);

    if (s < 0)
    {
        printf ("Error creating socket: %s\n", strerror (errno));
        exit (1);
    }


    if (::bind (s, (struct sockaddr *)&myAddr, sizeof (myAddr)) < 0)
    {
        printf ("Error binding socket: %s\n", strerror (errno));
        exit (1);
    }

    printf ("Waiting for a connection on port %d\n", gPort);

    if (listen (s, 1) < 0)
    {
        printf ("Error listening on socket: %s\n", strerror (errno));
        exit (1);
    }


    gFD = accept (s, (struct sockaddr *)&theirAddr, &len);

    if (gFD < 0)
    {
        printf ("Error accepting connection: %s\n", strerror (errno));
        exit (1);
    }

    if (gQuietLevel < 2)
    {
        printf ("Accepting connection from %s\n", 
                inet_ntoa (theirAddr.sin_addr));
    }

    send (gFD, 
          "Type \"quit\" to stop server.\n",
          strlen ("Type \"quit\" to stop server.\n"), 0);
}

class IOCallback : public MamaIoCallback
{
    virtual void onIo (MamaIo *io, mamaIoType ioType)
    {
        char buffer[1024];
        int len;

        if (io == gReadHandler)
        {
            len = read (gFD, buffer, 1023);
            buffer[len] = '\0';

            if (gQuietLevel < 2)
            {
                printf ("READ: %s", buffer);
            }

            if (strstr (buffer, "quit"))
            {
                printf ("QUITING\n");
                Mama::stop (gBridge);
                return;
            }

            send (gFD, buffer, len, 0);
        }
        else if (io == gWriteHandler)
        {
            if (gQuietLevel < 1)
            {
                printf ("WRITE\n");
            }
        }
        else if (io == gExceptHandler)
        {
            if (gQuietLevel < 2)
            {
                printf ("EXCEPT\n");
            }
        }
    }
};

static void createIOHandlers (void)
{
    try
    {
        if (gQuietLevel < 2)
        {
            printf ("Creating IO Handler\n");
        }

        gReadHandler = new MamaIo;
        gReadHandler->create (gDefaultQueue,
                              new IOCallback (), 
                              gFD, 
                              MAMA_IO_READ);

        gWriteHandler = new MamaIo;
        gWriteHandler->create (gDefaultQueue, 
                               new IOCallback (), 
                               gFD, 
                               MAMA_IO_WRITE);

        /* Handles exceptional events like out of band data */
        gExceptHandler = new MamaIo;
        gExceptHandler->create (gDefaultQueue, 
                                new IOCallback (), 
                                gFD, 
                                MAMA_IO_EXCEPT);
    }
    catch (MamaStatus &status)
    {
        cerr << "Error creating IO Handler: " << status.toString () << endl;
        cerr.flush ();
        exit (1);
    }
}

void initializeMama (void)
{
    try
    {
        gBridge = Mama::loadBridge (gMiddleware);
        Mama::open ();
        gDefaultQueue = Mama::getDefaultEventQueue (gBridge);
    }
    catch (MamaStatus &status)
    {
        cerr << "Error initializing mama: " << status.toString () << endl;
        exit (1);
    }
}

void parseCommandLine (int argc, const char **argv)
{
    int i = 0;
    for (i = 1; i < argc;)
    {
        if (strcmp ("-p", argv[i]) == 0)
        {
            gPort = atoi (argv[i+1]);
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
    }

    if (gQuietLevel < 2)
    {
        printf ("Starting IO with:\n"
                "   port:               %d\n", gPort);
    }
}
