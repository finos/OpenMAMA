/* $Id: connection.c,v 1.12.22.3 2011/09/01 16:34:38 emmapollock Exp $
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
#include <string.h>
#include "wombat/port.h"

#include "mama/conflation/connection.h"
#include "conflation/connection_int.h"
#include "bridge.h"
#include "transportimpl.h"

mama_status
mamaConnection_allocate (mamaConnection* result,
                         mamaTransport   tport,
                         const char*     ipAddress,
                         uint16_t        port,
                         uint32_t        maxQueueSize,
                         uint32_t        curQueueSize,
                         uint32_t        msgCount,
                         uint32_t        bytesSent,
                         const char*     userName,
                         const char*     appName,
                         void*           handle)
{
    mamaConnectionImpl* impl = 
        (mamaConnectionImpl*)calloc (1, sizeof (mamaConnectionImpl));

    if (impl == NULL)
    {
        return MAMA_STATUS_NOMEM;
    }

    impl->mTransport    = tport;
    impl->mPort         = port;
    impl->mMaxQueueSize = maxQueueSize;
    impl->mCurQueueSize = curQueueSize;
    impl->mMsgCount     = msgCount;
    impl->mBytesSent    = bytesSent;
    impl->mHandle       = handle;
    strncpy (impl->mIpAddress, ipAddress, (MAMACONNECTION_MAX_IP_ADDRESS_LEN - 1));
    strncpy (impl->mUserName, userName, (MAX_USER_STR_LEN - 1));
    strncpy (impl->mAppName, appName, (MAX_USER_STR_LEN - 1));

    *result = (mamaConnection)impl;

    return MAMA_STATUS_OK;
}

mama_status
mamaConnection_free (mamaConnection connection)
{
    mamaConnectionImpl* impl = (mamaConnectionImpl*)connection;
    if (impl != NULL)
        free (impl);
    return MAMA_STATUS_OK;
}

mama_status
mamaConnection_getTransport (mamaConnection connection, mamaTransport *result)
{
    mamaConnectionImpl* impl = (mamaConnectionImpl*)connection;
    if (impl == NULL )
        return MAMA_STATUS_NULL_ARG;

    *result = impl->mTransport;
    return MAMA_STATUS_OK;
}

mama_status
mamaConnection_getIpAddress (mamaConnection connection, const char** address)
{
    mamaConnectionImpl* impl = (mamaConnectionImpl*)connection;
    if (impl == NULL )
        return MAMA_STATUS_NULL_ARG;

    *address = impl->mIpAddress;
    return MAMA_STATUS_OK;
}

mama_status
mamaConnection_getPort (mamaConnection connection, uint16_t* port)
{
    mamaConnectionImpl* impl = (mamaConnectionImpl*)connection;
    if (impl == NULL )
        return MAMA_STATUS_NULL_ARG;

    *port = impl->mPort;
    return MAMA_STATUS_OK;
}
 
mama_status
mamaConnection_getUserName (mamaConnection connection, const char** userName)
{
    mamaConnectionImpl* impl = (mamaConnectionImpl*)connection;
    if (impl == NULL )
        return MAMA_STATUS_NULL_ARG;

    *userName = impl->mUserName;
    return MAMA_STATUS_OK;
}

mama_status
mamaConnection_getAppName (mamaConnection connection, const char** appName)
{
    mamaConnectionImpl* impl = (mamaConnectionImpl*)connection;
    if (impl == NULL )
        return MAMA_STATUS_NULL_ARG;

    *appName = impl->mAppName;
    return MAMA_STATUS_OK;
}

mama_status
mamaConnection_getMaxQueueSize(mamaConnection connection, uint32_t *queueSize)
{
    mamaConnectionImpl* impl = (mamaConnectionImpl*)connection;
    if (impl == NULL )
        return MAMA_STATUS_NULL_ARG;

    *queueSize = impl->mMaxQueueSize;
    return MAMA_STATUS_OK;
}

mama_status
mamaConnection_getQueueSize (mamaConnection connection, uint32_t *numItems)
{
    mamaConnectionImpl* impl = (mamaConnectionImpl*)connection;
    if (impl == NULL )
        return MAMA_STATUS_NULL_ARG;

    *numItems = impl->mCurQueueSize;
    return MAMA_STATUS_OK;
}

mama_status
mamaConnection_getMsgCount (mamaConnection connection, uint32_t *msgCount)
{
    mamaConnectionImpl* impl = (mamaConnectionImpl*)connection;
    if (impl == NULL )
        return MAMA_STATUS_NULL_ARG;

    *msgCount = impl->mMsgCount;
    return MAMA_STATUS_OK;
}

mama_status
mamaConnection_getBytesSent (mamaConnection connection, uint32_t *bytesSent)
{
    mamaConnectionImpl* impl = (mamaConnectionImpl*)connection;
    if (impl == NULL )
        return MAMA_STATUS_NULL_ARG;

    *bytesSent = impl->mBytesSent;
    return MAMA_STATUS_OK;
}

const char*
mamaConnection_toString (mamaConnection connection)
{
    /* BML - TODO - add appname and login - need to MAX_STR_LEN needs to grow */
    mamaConnectionImpl* impl = (mamaConnectionImpl*)connection;

    /* Clear the buffer first */
    memset(impl->mStrVal, 0, MAX_STR_LEN);

    /* Format the string and print it into the mStrVal buffer */
    /* uint16_t = max 5 digits, uint32_t = max 10 digits */
    snprintf (impl->mStrVal, (MAX_STR_LEN - 1), "%s %d %s %s %u %u %u %u",
              impl->mIpAddress,
              impl->mPort,
              impl->mAppName,
              impl->mUserName,
              impl->mMaxQueueSize,
              impl->mCurQueueSize,
              impl->mMsgCount,
              impl->mBytesSent);
    return impl->mStrVal;
}

void*
mamaConnectionImpl_getHandle (mamaConnection connection)
{
    mamaConnectionImpl* impl = (mamaConnectionImpl*)connection;

    if (impl == NULL )
        return NULL;

    return impl->mHandle;

}

mama_status
mamaConnection_isIntercepted (mamaConnection connection, uint8_t* result)
{
    mamaConnectionImpl* impl = (mamaConnectionImpl*)connection;

    if (impl == NULL)
        return MAMA_STATUS_NULL_ARG;

    return mamaTransportImpl_isConnectionIntercepted (impl->mTransport,
                                                      connection, 
                                                      result);
}
