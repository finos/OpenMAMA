
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <net/if.h>
#include "wombat/wCommon.h"


struct in_addr wresolve_ip (const char * arg)
{

    struct in_addr resolved; 
    struct in_addr addr;
    unsigned long  netip    = 0;
    const char * ifname     = arg;
    int sock                = -1;
    int i=0;
    unsigned      numreqs=0;
    struct ifconf ifc;
    struct ifreq *ifr;
    resolved = inet_makeaddr(0,0);   
    addr = inet_makeaddr(0,0);   
    

    if (!arg || (strlen(arg)==0))
    {
        resolved.s_addr = INADDR_ANY;
        return resolved;
    }
    /* 1. try interpreting as network or host IP address */
    {
        netip   = inet_network (arg);
        if (netip != (unsigned long)(-1))
        {
            addr = inet_makeaddr(netip, 0);
        }
    }
    /* 2. try interpreting as network name */
    if (! addr.s_addr)
    {
        struct netent * net = getnetbyname (arg);
        if ((net != NULL) && (net->n_addrtype == AF_INET))
        {
            addr = inet_makeaddr (net->n_net, 0);
        }
    }
    /* 3. try interpreting as host name */
    if (! addr.s_addr)
    {
        struct hostent * host = gethostbyname (arg);
        if ((host != NULL) && (host->h_addrtype == AF_INET))
        {
            addr = *(struct in_addr *)(host->h_addr_list[0]);
        }
    }
    
    /* 4. try interpreting as a NIC interface name */
    numreqs = 30;
    sock = socket(AF_INET, SOCK_DGRAM, 0);

    ifc.ifc_buf = NULL;
    for (;;)
    {
        ifc.ifc_len = sizeof(struct ifreq) * numreqs;
        ifc.ifc_buf = (char *)malloc(ifc.ifc_len);
        if (ioctl(sock, SIOCGIFCONF, &ifc) < 0)
        {
            perror ("SIOCGIFCONF");
            resolved.s_addr = (unsigned long) -1;
            return resolved;
        }
        if (ifc.ifc_len == (sizeof(struct ifreq) * numreqs))
        {
            numreqs *= 2;
            continue;
        }
        break;
    }

    ifr = ifc.ifc_req;
    for (i = 0; i < ifc.ifc_len; i += sizeof(struct ifreq))
    {
        struct sockaddr_in    hostaddr; 
        struct sockaddr_in    netmask;  
        struct in_addr network  = inet_makeaddr(0,0);

        hostaddr.sin_addr = inet_makeaddr(0,0);
        netmask.sin_addr  = inet_makeaddr(0,0);
        
        if (ioctl(sock, SIOCGIFADDR, ifr) < 0)
        {
            perror ("SIOCGIFADDR");
            resolved.s_addr = (unsigned long)-1;
            return resolved;
        }
        memcpy (&hostaddr, &(ifr->ifr_addr), sizeof(struct sockaddr_in));

        if (ioctl(sock, SIOCGIFNETMASK, ifr) < 0)
        {
            perror ("SIOCGIFNETMASK");
            resolved.s_addr = (unsigned long)-1;
            return resolved;
        }
        memcpy (&netmask, &(ifr->ifr_addr), sizeof (struct sockaddr_in));
        network.s_addr = hostaddr.sin_addr.s_addr & netmask.sin_addr.s_addr;
        if (addr.s_addr)
        {
            if (addr.s_addr == hostaddr.sin_addr.s_addr)
            {
                resolved = hostaddr.sin_addr;
                break;
            }
            else if (addr.s_addr == network.s_addr)
            {
                resolved = hostaddr.sin_addr;
                break;
            }
        }

        if (strcmp(ifr->ifr_name, ifname) == 0)
        {
            resolved = hostaddr.sin_addr;
            break;
        }
        ifr++;
    }

    if (!resolved.s_addr)
    {
        resolved.s_addr = (unsigned long)-1;
    } 
    return (resolved);
}

static const char gIPAddress[16];
static const char* gHostName = NULL;

static void lookupIPAddress (void)
{
    struct hostent *host = NULL;
    char           *addrStr = "not determined";

    struct         utsname uts;
    memset( gIPAddress, 0, 16 );
    uname (&uts);
    gHostName = strdup (uts.nodename);

    host = gethostbyname( gHostName );

    if( gHostName == NULL ||
        host == NULL      ||
        host->h_addr_list[0] == NULL )
    {
       strncpy( (char *)gIPAddress, "not determined", sizeof( gIPAddress ) );
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
