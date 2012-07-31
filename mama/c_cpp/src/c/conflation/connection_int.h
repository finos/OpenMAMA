/* $Id: connection_int.h,v 1.9.22.2 2011/09/01 16:34:38 emmapollock Exp $
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

#ifndef MAMA_CONNECTION_INT_H__
#define MAMA_CONNECTION_INT_H__

#include "wombat/port.h"

#include "mama/conflation/connection.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define MAX_STR_LEN 67
#define MAX_USER_STR_LEN 256

#define MAMACONNECTION_MAX_IP_ADDRESS_LEN INET_ADDRSTRLEN    

typedef struct mamaConnection_
{
    mamaTransport mTransport;
    char          mIpAddress[MAMACONNECTION_MAX_IP_ADDRESS_LEN];
    uint16_t      mPort;
    uint32_t      mMaxQueueSize;
    uint32_t      mCurQueueSize;
    uint32_t      mMsgCount;
    uint32_t      mBytesSent;
    void*         mHandle;
    char          mStrVal[MAX_STR_LEN];
    char          mUserName[MAX_USER_STR_LEN];
    char          mAppName[MAX_USER_STR_LEN];
} mamaConnectionImpl;


/**
 * Allocate and initialize a mamaConnection. Mama calls this internally.
 * Currently clients should not need to call this method.
 *
 */
MAMAExpDLL 
extern mama_status
mamaConnection_allocate (mamaConnection* result,
                         mamaTransport   tport,
                         const char*     ipAddress,
                         uint16_t        port,
                         uint32_t        maxQueueSize,
                         uint32_t        curQueueSize,
                         uint32_t        msgCount,
                         uint32_t        bytesCount,
                         const char*     userName,
                         const char*     appName,
                         void*           handle);

MAMAExpDLL 
extern void*
mamaConnectionImpl_getHandle (mamaConnection connection);

#if defined(__cplusplus)
}
#endif
#endif /* MAMA_CONNECTION_INT_H__ */
