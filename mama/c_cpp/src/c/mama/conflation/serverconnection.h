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
 * @brief Free the server connection.
 *
 * @param[in] connection The mama server connection
 *
 * @return mama_status return code MAMA_STATUS_OK
 */
MAMAExpDLL
mama_status
mamaServerConnection_free (mamaServerConnection connection);

/**
 * @brief Get the IP address of the connected server.
 *
 * @param[in]  connection The mama server connection
 * @param[out] address    Returning the associated IP address
 *
 * @return mama_status return code MAMA_STATUS_OK
 */
MAMAExpDLL
mama_status
mamaServerConnection_getIpAddress (mamaServerConnection connection,
                                   const char**         address);

/**
 * @brief Get the port of the connected server.
 *
 * @param[in]  connection The mama server connection
 * @param[out] port       Returning the associated port
 *
 * @return mama_status return code MAMA_STATUS_OK
 */
MAMAExpDLL
mama_status
mamaServerConnection_getPort (mamaServerConnection connection, uint16_t* port);

/**
 * @brief Get the status of the server connection.
 *
 * @param[in]  connection The mama server connection
 * @param[out] connectionStatus Returning the connection status
 *
 * @return mama_status return code MAMA_STATUS_OK
 */
MAMAExpDLL
mama_status
mamaServerConnection_getConnectionStatus (mamaServerConnection connection,
                                          int*                 connectionStatus);

#if defined(__cplusplus)
}
#endif

#endif /* MAMA_SERVER_CONNECTION_CPP_H__ */
