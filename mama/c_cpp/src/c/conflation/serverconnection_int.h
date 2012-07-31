/* $Id: serverconnection_int.h,v 1.3.32.2 2011/09/01 16:34:38 emmapollock Exp $
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

#ifndef MAMA_SERVER_CONNECTION_INT_H__
#define MAMA_SERVER_CONNECTION_INT_H__

#include "wombat/port.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct mamaServerConnection_
{
    char        mIpAddress[INET_ADDRSTRLEN];
    uint16_t    mPort;
    int         mConnectionStatus;
} mamaServerConnectionImpl;

MAMAExpDLL 
extern mama_status
mamaServerConnection_allocate (mamaServerConnection* result,
                               const char*           ipAddress,
                               uint16_t              port,
                               int                   connectionStatus);

#if defined (__cplusplus)
}
#endif

#endif /* MAMA_SERVER_CONNECTION_INT_H */
