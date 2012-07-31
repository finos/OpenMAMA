/* $Id: serverconnection.h,v 1.3.32.2 2011/09/01 16:34:38 emmapollock Exp $
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
 * This module contains information regarding inbound connections.
 */

#ifndef MAMA_SERVER_CONNECTION_CPP_H__
#define MAMA_SERVER_CONNECTION_CPP_H__

#include "mama/mama.h"
#include "mama/config.h"

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * Free the server connection.
 */
MAMAExpDLL
mama_status
mamaServerConnection_free (mamaServerConnection connection);

/**
 * Get the IP address of the connected server.
 */
MAMAExpDLL
mama_status
mamaServerConnection_getIpAddress (mamaServerConnection connection,
                                   const char**         address);

/**
 * Get the port of the connected server.
 */
MAMAExpDLL
mama_status
mamaServerConnection_getPort (mamaServerConnection connection, uint16_t* port);

/**
 * Get the status of the server connection.
 */
MAMAExpDLL
mama_status
mamaServerConnection_getConnectionStatus (mamaServerConnection connection,
                                          int*                 connectionStatus);

#if defined(__cplusplus)
}
#endif

#endif /* MAMA_SERVER_CONNECTION_CPP_H__ */
