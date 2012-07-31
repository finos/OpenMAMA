/* $Id: senderId.c,v 1.4.22.4 2011/10/02 19:02:17 ianbell Exp $
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

#include "wombat/port.h"
#include "wombat/wincompat.h"
#include "mama/senderId.h"


#ifndef HOST_NAME_MAX
# define HOST_NAME_MAX 255
#endif

static uint64_t gSelf = 0;
static uint16_t gIsSetSelf = 0;

void mamaSenderId_getIpAddr (uint64_t  senderId,
                             char*     buffer,
                             size_t    maxLen)
{
    struct in_addr addr;
    const char*    ip = NULL;
    addr.s_addr = (uint32_t)(senderId >> 32);
    ip = inet_ntoa (addr);
    snprintf (buffer, maxLen, "%s", ip);
}

void mamaSenderId_getPid (uint64_t   senderId,
                          uint16_t*  result)
{
    /* Last 16 bytes */
    *result = (0x00FF & senderId);
}

void mamaSenderId_getPid64 (uint64_t   senderId,
                          uint32_t*  result)
{
    /* Last 32 bits */
    *result = (0x0000FFFF & senderId);
}

uint64_t mamaSenderId_getSelf ()
{
	if (!gIsSetSelf)
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

		    gSelf	= (((uint64_t)addr.s_addr) << 32) + getpid();
		}
		else
		{
			gSelf	= getpid();
		}
		gIsSetSelf	= 1;
	}

	return gSelf;
}

void mamaSenderId_setSelf(uint64_t self)
{
	gSelf	= self;
	gIsSetSelf	= 1;
}
