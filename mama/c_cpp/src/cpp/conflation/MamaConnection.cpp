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

#include "mama/mamacpp.h"
#include "mamacppinternal.h"
#include "mama/conflation/MamaConnection.h"

namespace Wombat
{

MamaConnection::~MamaConnection (void)
{
    mamaConnection_free (mConnnection);
}
    
MamaTransport* 
MamaConnection::getTransport (void) const
{
    return mTransport;
}

const char* 
MamaConnection::getIpAddress (void) const
{ 
    const char* result = NULL;
    mamaTry (mamaConnection_getIpAddress (mConnnection, &result));
    return result;
}

uint16_t
MamaConnection::getPort (void) const
{ 
    uint16_t result = 0;
    mamaTry (mamaConnection_getPort (mConnnection, &result));
    return result;
}

const char*
MamaConnection::getUserName (void) const
{
    const char* result = NULL;
    mamaTry (mamaConnection_getUserName (mConnnection, &result));
    return result;
}

const char*
MamaConnection::getAppName (void) const
{
    const char* result = NULL;
    mamaTry (mamaConnection_getAppName (mConnnection, &result));
    return result;
}

uint32_t
MamaConnection::getQueueSize (void) const
{ 
    uint32_t result = 0;
    mamaTry (mamaConnection_getQueueSize (mConnnection, &result));
    return result;
}

uint32_t
MamaConnection::getMaxQueueSize (void) const
{ 
    uint32_t result = 0;
    mamaTry (mamaConnection_getMaxQueueSize (mConnnection, &result));
    return result;
}

uint32_t
MamaConnection::getMsgCount (void) const
{
    uint32_t result = 0;
    mamaTry (mamaConnection_getMsgCount (mConnnection, &result));
    return result;
}

uint32_t
MamaConnection::getBytesSent (void) const
{
    uint32_t result = 0;
    mamaTry (mamaConnection_getBytesSent (mConnnection, &result));
    return result;
}

const char*
MamaConnection::toString (void) const
{
    return mamaConnection_toString (mConnnection);
}

mamaConnection MamaConnection::getCValue ()
{
    return mConnnection;
}

const mamaConnection MamaConnection::getCValue () const
{
    return mConnnection;
}

bool MamaConnection::isIntercepted (void) const
{
    uint8_t result = 0;
    mamaTry (mamaConnection_isIntercepted (mConnnection, &result));
    return result == 0 ? false : true;
}
}
