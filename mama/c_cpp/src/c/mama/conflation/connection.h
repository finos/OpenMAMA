/* $Id: connection.h,v 1.6.24.2 2011/09/01 16:34:38 emmapollock Exp $
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
 * Free the connection.
 */
MAMAExpDLL
mama_status
mamaConnection_free (mamaConnection connection);

/**
 * Convenience method for returnining the transport.
 */
MAMAExpDLL
mama_status
mamaConnection_getTransport (mamaConnection connection, mamaTransport *result);

/**
 * Return the IP address of the connected client.
 */
MAMAExpDLL
mama_status
mamaConnection_getIpAddress (mamaConnection connection, const char** address);

/**
 * Return the port of the connected client. This will usually be an ephemeral
 * port selected by the client's ip stack.
 */
MAMAExpDLL
mama_status
mamaConnection_getPort (mamaConnection connection, uint16_t* port);

/*
 * Return the user name of the connected client.
 */
MAMAExpDLL
mama_status
mamaConnection_getUserName (mamaConnection connection, const char** userName);

/**
 * Return the application name of the connected client.
 */
MAMAExpDLL
mama_status
mamaConnection_getAppName (mamaConnection connection, const char** appName);

/**
 * Return the maximum allowable outbound queue size for the client.
 */
MAMAExpDLL
mama_status
mamaConnection_getMaxQueueSize(mamaConnection connection, uint32_t *queueSize);

/**
 * Get the number of items in the queue when structure was created.
 */
MAMAExpDLL
mama_status
mamaConnection_getQueueSize (mamaConnection connection, uint32_t *numItems);

/**
 * Get the number of messages sent on the connection.
 */
MAMAExpDLL
mama_status
mamaConnection_getMsgCount (mamaConnection connection, uint32_t *msgCount);

/**
 * Get the number of bytes sent on the connection.
 */
MAMAExpDLL
mama_status
mamaConnection_getBytesSent (mamaConnection connection, uint32_t *bytesSent);

/**
 * Convert a connection to a string.
 */
MAMAExpDLL
const char*
mamaConnection_toString (mamaConnection connection);

/* Result is 0 if connection is not intercepted */
MAMAExpDLL
mama_status
mamaConnection_isIntercepted (mamaConnection connection, uint8_t* result);

#if defined(__cplusplus)
}
#endif
#endif /* MAMA_CONNECTION_H__ */
