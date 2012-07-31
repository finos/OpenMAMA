/* $Id: serverconnection.c,v 1.3.32.2 2011/09/01 16:34:38 emmapollock Exp $
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

#include <mama/mama.h>
#include <mama/conflation/serverconnection.h>

#include "serverconnection_int.h"

#include <string.h>

mama_status
mamaServerConnection_allocate (mamaServerConnection* result,
                               const char*           ipAddress,
                               uint16_t              port,
                               int                   connectionStatus)
{
    mamaServerConnectionImpl* impl = 
      (mamaServerConnectionImpl*)calloc (1, sizeof (mamaServerConnectionImpl));

    if (impl == NULL)
    {
        return MAMA_STATUS_NOMEM;
    }

    strncpy (impl->mIpAddress, ipAddress, sizeof (impl->mIpAddress));
    impl->mPort = port;
    impl->mConnectionStatus = connectionStatus;

    *result = (mamaServerConnection)impl;

    return MAMA_STATUS_OK;
}

mama_status
mamaServerConnection_free (mamaServerConnection connection)
{
    mamaServerConnectionImpl* impl = (mamaServerConnectionImpl*)connection;
    if (impl != NULL)
        free (impl);
    return MAMA_STATUS_OK;
}

mama_status
mamaServerConnection_getIpAddress (mamaServerConnection connection,
                                   const char**         address)
{
    mamaServerConnectionImpl* impl = (mamaServerConnectionImpl*)connection;
    *address = impl->mIpAddress;
    return MAMA_STATUS_OK;
}


mama_status
mamaServerConnection_getPort (mamaServerConnection connection, uint16_t* port)
{
    mamaServerConnectionImpl* impl = (mamaServerConnectionImpl*)connection;
    *port = impl->mPort;
    return MAMA_STATUS_OK;
}

mama_status
mamaServerConnection_getConnectionStatus (mamaServerConnection connection,
                                          int*                 connectionStatus)
{
    mamaServerConnectionImpl* impl = (mamaServerConnectionImpl*)connection;
    *connectionStatus = impl->mConnectionStatus;
    return MAMA_STATUS_OK;
}
