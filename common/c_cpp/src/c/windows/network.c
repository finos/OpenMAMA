/*
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

#include "wombat/port.h"

struct in_addr wresolve_ip (const char * arg)
{
    struct in_addr resolved; 

    if (0 == strcmp(arg, ""))
        resolved.s_addr = INADDR_ANY;
    else
        resolved.s_addr = inet_addr (arg);
    return (resolved);
}

int 
wsetnonblock (int s)
{
    unsigned long arg = 1;
    if (0 != ioctlsocket (s, FIONBIO, &arg))
        return -1;
    return 0;
}

static char gIPAddress[16];
static const char* gHostName = NULL;

static void lookupIPAddress (void)
{
    struct hostent *host = NULL;
    char           *addrStr = "not determined";

    char hostname[256];
    if( 0 != gethostname( hostname, 256 ) )
    {
        snprintf( hostname, strlen( "localhost" ), "localhost" );
    }
    gHostName = strdup (hostname);

    host = gethostbyname( gHostName );

    if( gHostName == NULL ||
        host == NULL      ||
        host->h_addr_list[0] == NULL )
    {
       strncpy( (char *)gIPAddress, "not determined", sizeof( gIPAddress ) );
/*       return MAMA_STATUS_IP_NOT_FOUND;*/
    }
    else
    {
        addrStr = inet_ntoa( *((struct in_addr *)( host->h_addr_list[0] )));
    }

    strncpy ((char*)gIPAddress, addrStr, sizeof (gIPAddress));
}

const char* getIpAddress(void)
{
    if (NULL == gHostName)
        lookupIPAddress();
    return gIPAddress;
}

const char* getHostName(void)
{
    if (NULL == gHostName)
        lookupIPAddress();
    return gHostName;
}
