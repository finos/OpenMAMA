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
 * This class contains information regarding outbound connections for
 * per/connection conflation.
 */

#ifndef MAMA_CONNECTION_H__
#define MAMA_CONNECTION_H__

#include "mama/mama.h"
#include "mama/config.h"

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * @brief Free the connection.
 *
 * @param[in] connection The mama connection
 *
 * @return mama_status return code MAMA_STATUS_OK
 */
MAMAExpDLL
mama_status
mamaConnection_free (mamaConnection connection);

/**
 * @brief Convenience method for returnining the transport.
 *
 * @param[in]  connection The mama connection
 * @param[out] result     Returning the associated transport.
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK       
 */
MAMAExpDLL
mama_status
mamaConnection_getTransport (mamaConnection connection, mamaTransport *result);

/**
 * @brief Return the IP address of the connected client.
 *
 * @param[in]  connection The mama connection
 * @param[out] address    Returning the associated IP address
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
mama_status
mamaConnection_getIpAddress (mamaConnection connection, const char** address);

/**
 * @brief Return the port of the connected client. This will usually be an ephemeral
 * port selected by the client's ip stack.
 *
 * @param[in]  connection The mama connection
 * @param[out] port       Returning the associated port
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
mama_status
mamaConnection_getPort (mamaConnection connection, uint16_t* port);

/*
 * @brief Return the user name of the connected client.
 *
 * @param[in]  connection The mama connection
 * @param[out] userName   Returning the associated user name
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
mama_status
mamaConnection_getUserName (mamaConnection connection, const char** userName);

/**
 * @brief Return the application name of the connected client.
 *
 * @param[in]  connection The mama connection
 * @param[out] appName    Returning the associated application name
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
mama_status
mamaConnection_getAppName (mamaConnection connection, const char** appName);

/**
 * @brief Return the maximum allowable outbound queue size for the client.
 *
 * @param[in]  connection The mama connection
 * @param[out] queueSize  Returning the associated maximum queue size
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
mama_status
mamaConnection_getMaxQueueSize(mamaConnection connection, uint32_t *queueSize);

/**
 * @brief Get the number of items in the queue when structure was created.
 *
 * @param[in]  connection The mama connection
 * @param[out] numItems   Returning the associated number of items in the 
 *                        queue
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
mama_status
mamaConnection_getQueueSize (mamaConnection connection, uint32_t *numItems);

/**
 * @brief Get the number of messages sent on the connection.
 *
 * @param[in]  connection The mama connection
 * @param[out] msgCount   Returning the number of messages per connection
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK 
 */
MAMAExpDLL
mama_status
mamaConnection_getMsgCount (mamaConnection connection, uint32_t *msgCount);

/**
 * @brief Get the number of bytes sent on the connection.
 *
 * @param[in]  connection The mama connection
 * @param[out] bytesSent  Returning the number of bytes sent per
 *                        connection
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
mama_status
mamaConnection_getBytesSent (mamaConnection connection, uint32_t *bytesSent);

/**
 * @brief Convert a connection to a string.
 *
 * @param[in]  connection The mama connection
 *
 * @return String representation of the mama connection object
 */
MAMAExpDLL
const char*
mamaConnection_toString (mamaConnection connection);

/**
 * @brief Check if the conneciton is intercepted
 *
 * @param[in]  connection The mama connection
 * @param[out] result     Returns 0 if connection is not intercepted.
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_NOT_INSTALLED
 */
MAMAExpDLL
mama_status
mamaConnection_isIntercepted (mamaConnection connection, uint8_t* result);

#if defined(__cplusplus)
}
#endif
#endif /* MAMA_CONNECTION_H__ */
