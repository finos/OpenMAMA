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

#include "mama/mama.h"
#include "string.h"

static mamaTransport    gTransport     = NULL;
static mamaFtMember     gFtMember      = NULL;
static mamaFtType       gFtType        = MAMA_FT_TYPE_MULTICAST;
static const char*      gFtGroup       = NULL;
static mama_u32_t       gFtWeight      = 50;
static const char *     gTransportName = "ft";
static mamaBridge       gMamaBridge    = NULL;
static const char *     gMiddleware    = "wmw";
static mamaQueue        gMamaDefaultQueue   = NULL;
static const char *     gUsageString[] =
{
"This sample application demonstrates fault tolerance",
"",
" It accepts the following command line arguments:",
"      [-tport name]      The transport parameters to be used from",
"                         mama.properties. Default is sub",
"      [-m middleware]    The middleware to use [wmw/lbm/tibrv]. Default", 
"                         is wmw.", 
"      [-g]               Fault tolerant group name.",
"      [-w]               Fault tolerant weighting.",
"      [-b]               Use Bridged FT (default is native).",
"      [-v]               Increase verbosity. Can be passed multiple times",
NULL
};

static void parseCommandLine            (int argc, const char **argv);
static void initializeMama              (void);
static void createFtMember              (void);

static void MAMACALLTYPE onFtStateChangeCb           (mamaFtMember  ftMember,
                                         const char*   groupName,
                                         mamaFtState   state,
                                         void*         closure);

static void usage (int  exitStatus);

static void MAMACALLTYPE onFtStateChangeCb (mamaFtMember  ftMember,
                   const char*   groupName,
                   mamaFtState   state,
                   void*         closure)
{
    printf("FT Notification: Current state is %s\n",mamaFtStateToString(state));
    fflush(stdout);
}


int main (int argc, const char** argv)
{
    parseCommandLine (argc, argv);

    initializeMama ();
   
    createFtMember ();

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

static void createFtMember (void)
{
    mama_status status;

    if (MAMA_STATUS_OK != (status= mamaFtMember_create(&gFtMember)))
    {
        printf ("Error creating FT Member: %s\n", 
                mamaStatus_stringForStatus (status));
        exit (status);
    }

    if (MAMA_STATUS_OK != (status = mamaFtMember_setup(gFtMember,
                       gFtType,
                       gMamaDefaultQueue,
                       onFtStateChangeCb,
                       gTransport,
                       gFtGroup,
                       gFtWeight,
                       1.0,
                       6.0,
                       NULL)))
    {
        printf ("Error setting up FT Member: %s\n",            
                mamaStatus_stringForStatus (status));
        exit (status);

    }

    if (MAMA_STATUS_OK != (status= mamaFtMember_activate(gFtMember)))
    {
        printf ("Error activating FT Member: %s\n",            
                mamaStatus_stringForStatus (status));
        exit (status);
    }
}

void parseCommandLine (int argc, const char** argv)
{
    int i = 0;
    for (i = 1; i < argc;)
    {
        if ((strcmp(argv[i], "-h") == 0) ||
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
        else if ((strcmp (argv[i], "-g") == 0) ||
                 (strcmp (argv[i], "-group") == 0))
        {
            gFtGroup = argv[i+1];
            i += 2;
        }
        else if ((strcmp (argv[i], "-w") == 0) ||
                 (strcmp (argv[i], "-weight") == 0))
        {
            gFtWeight = strtoul (argv[i+1], NULL, 10);
            i += 2;
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
        else if (strcmp (argv[i], "-b") == 0)
        {
            gFtType = MAMA_FT_TYPE_BRIDGE;
            ++i;
        }

        else
        {
        	i++;
        }
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

