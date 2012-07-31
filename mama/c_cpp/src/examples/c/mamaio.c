/* $Id: mamaio.c,v 1.1.2.2 2011/09/27 16:54:39 emmapollock Exp $
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
 * This sample application demonstrates how to use mamaIoHandlers. It creates a
 * socket an waits for a connection. Upon accepting a connection it creates a
 * mamaIoHandler to read from the socket and connects back to the client on
 * port 107 (telnet). It echos the users input. Type "quit" to exit.
 *
 * It accepts the following command line arguments:
 *      [-port number]     The TCP/IP port on which to listen. Defaults to 9998
 *      [-q]               Quiet mode. Suppress output.
 *---------------------------------------------------------------------------*/

#include "wombat/port.h"
#include <sys/types.h>
#include <errno.h>
#include <string.h>

#include "mama/mama.h"
#include "mama/io.h"

static mamaIo           gReadHandler    = NULL;
static mamaIo           gWriteHandler   = NULL;

static int              gPort           = 9998;
static int              gQuietLevel     = 0;
static int              gFD             = 0;

static mamaBridge       gMamaBridge     = NULL;
static const char*      gMiddleware     = "wmw";
static void parseCommandLine    (int argc, const char **argv);
static void initializeMama      (void);
static void waitForConection    (void);
static void createIOHandlers    (void);

static void MAMACALLTYPE
ioCallback          (mamaIo io, mamaIoType ioType, void *closure);

int main (int argc, const char **argv)
{
    parseCommandLine (argc, argv);

    initializeMama ();

    waitForConection ();

    createIOHandlers ();

    mama_start (gMamaBridge);

    if (gQuietLevel < 2)
    {
        uint32_t d;
        mamaIo_getDescriptor (gReadHandler, &d);

        printf ("Cleaning up FD = %d\n", d);
    }

    mamaIo_destroy (gReadHandler);
    mamaIo_destroy (gWriteHandler);

    mama_close ();

    shutdown (gFD, SHUT_RDWR);

    return 0;
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


    if (bind (s, (struct sockaddr *)&myAddr, sizeof (myAddr)) < 0)
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
          strlen ("Type \"quit\" to stop server.\n"), 0 );
}

static void MAMACALLTYPE
ioCallback (mamaIo io, mamaIoType ioType, void *closure)
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
            mama_stop (gMamaBridge);
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
}

static void createIOHandlers (void)
{
    mama_status status          =   MAMA_STATUS_OK;
    mamaQueue   defaultQueue    =   NULL;

    if (gQuietLevel < 2)
    {
        printf ("Creating IO Handler\n");
    }

    /*Get the default event queue*/
    mama_getDefaultEventQueue (gMamaBridge, &defaultQueue);

    status = mamaIo_create (&gReadHandler,
                            defaultQueue,
                            gFD,
                            ioCallback,
                            MAMA_IO_READ,
                            NULL);

    if (status != MAMA_STATUS_OK)
    {
        printf ("Error creating mamaIoHandler for READ: %s\n",
                mamaStatus_stringForStatus (status));
        exit (1);
    }

    status = mamaIo_create (&gWriteHandler,
                            defaultQueue,
                            gFD,
                            ioCallback,
                            MAMA_IO_WRITE,
                            NULL);

    if (status != MAMA_STATUS_OK)
    {
        printf ("Error creating mamaIoHandler for WRITE: %s\n",
                mamaStatus_stringForStatus (status));
        exit (1);
    }
}

void initializeMama (void)
{
    mama_status status  =   MAMA_STATUS_OK;

    if (MAMA_STATUS_OK !=
       (status = mama_loadBridge (&gMamaBridge, gMiddleware)))
    {
        printf ("Error loading %s bridge : %s\n",
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
}

void parseCommandLine (int argc, const char **argv)
{
    int i = 0;
    for (i = 1; i < argc; )
    {
        if (strcmp ("-p", argv[i]) == 0  ||
            strcmp ("-port", argv[i]) == 0)
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
