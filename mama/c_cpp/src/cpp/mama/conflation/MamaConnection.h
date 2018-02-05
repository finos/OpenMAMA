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

#ifndef MAMA_CONNECTION_CPP_H__
#define MAMA_CONNECTION_CPP_H__

#include "mama/mamacpp.h"
#include "mama/MamaTransport.h"
#include "mama/conflation/connection.h"

namespace Wombat
{

class MAMACPPExpDLL MamaConnection
{
public:
    MamaConnection (MamaTransport* tport, mamaConnection cnxn) 
        : mConnnection (cnxn)
        , mTransport   (tport) {}

    virtual ~MamaConnection (void);
    
    /* Convenience method to get the transport to which this connection belongs.     */
    MamaTransport* getTransport (void) const;

    /* Return the remote IP address.
     */
    const char* getIpAddress (void) const;

    /* return the remote port. Note for outbound connections this will be the
     * "ephemeral" port assigned by connect() while the IP address will be the 
     * local address. For inbound connection it will be the "ephemeral" port 
     * on the remote client and the IP address of the remote client.
     *
     * The combination of IP address and port uniquely identifies the 
     * connection/client.
     */
    uint16_t getPort (void) const;

    /* Return the username related to this event. */
    const char* getUserName (void) const;

    /* Return the application name related to this event. */
    const char* getAppName (void) const;

    /* Return the current queue size which triggered the event. */
    uint32_t getQueueSize (void) const;

    /* Return the maximum queue size. */
    uint32_t getMaxQueueSize (void) const;

    /* Return the number of messages sent on the connection. */
    uint32_t getMsgCount (void) const;

    /* Return the number of bytes sent on the connection. */
    uint32_t getBytesSent (void) const;

    /* Return the string version of the connection. */
    const char* toString (void) const;

    /* Return true if this connection is intercepted. */
    bool isIntercepted (void) const;

    // Access to C types for implementation of related classes.
    mamaConnection        getCValue    ();
    const mamaConnection  getCValue    () const;
private:
    mamaConnection 			mConnnection; 
    MamaTransport* 			mTransport;

    MamaConnection (void) {}
    MamaConnection (const MamaConnection&) {}

};

}
#endif
