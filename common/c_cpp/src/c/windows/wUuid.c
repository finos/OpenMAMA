/* $Id: wUuid.c,v 1.1.2.3 2012/04/05 09:33:15 evinkeating Exp $
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


#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "wombat/wUuid.h"

static uint64_t gRoot = 0;
static uint16_t gGotroot = 0;

#ifndef HOST_NAME_MAX
# define HOST_NAME_MAX 255
#endif

void wUuid_generate_time (wUuid myUuid)
{
}

void wUuid_unparse (wUuid myUuid, char* out)
{
	if (!gGotroot)
	{
		char hostname[HOST_NAME_MAX+1];
		if (gethostname (hostname, HOST_NAME_MAX) == 0)
		{
		    struct in_addr addr;
		    struct hostent* host = gethostbyname (hostname);
		    if ((host != NULL) && (host->h_addrtype == AF_INET))
		    {
		        /* Found host by name - we just look at the first entry.
		         * Maybe we should look at all of the entries (these are not
		         * aliases though). */
		        addr = *(struct in_addr*)(host->h_addr_list[0]);
		    }
		    else
		    	addr.s_addr=INADDR_NONE;

		    gRoot	= (((uint64_t)addr.s_addr) << 32) + getpid();
		}
		else
		{
			gRoot	= getpid();
		}
		gGotroot	= 1;
	}

    srand (time (NULL));
    snprintf (out, 49, "%d%d", gRoot, rand() % 10000);
}
