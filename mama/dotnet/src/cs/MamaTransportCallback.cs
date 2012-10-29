/* $Id: MamaTransportCallback.cs,v 1.5.34.7 2012/08/24 16:12:01 clintonmcdowell Exp $
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

using System;

namespace Wombat
{
    using System;

    /// <summary>
	/// Callbacks associated with MamaTransports
    /// </summary>
    /// <remarks>
    /// Events that can cause one of the callbacks to be invoked vary according
    /// to the underlying middleware. The details are as follows:
    /// <para>
    /// All Middleware: MAMA_TRANSPORT_QUALITY is passed when an the underlying
    /// transport detects an event that may affect data quality. Depending on the
    /// event, MAMA may also mark subscriptions as STALE or MAYBE_STALE.
    /// </para><para>
    /// TIBRV: TIBRV invokes the callback with MAMA_TRANSPORT_DISCONNECT when the
    /// transport disconnects from the RV daemon and MAMA_TRANSPORT_RECONNECT when
    /// it reestablishes a connection to the daemon.
    /// </para><para>
    /// Wombat Middleware: For Wombat Middleware MAMA distinguishes between
    /// connections established from a transport to another transport and
    /// connections established by the transport from another transport. In general
    /// MAMA applications establish connection from a MAMA transport to a feed
    /// handler (publisher); however, they may also accept connections from other 
    /// MAMA clients. Applications subscribe to topics and publisher messages over 
    /// all connections. When a transport successfully connects to another transport
    /// (ie. a feed handler) the mamaTranpsortCallback receives
    /// MAMA_TRANSPORT_CONNECT if the connection succeeds immediately otherwise it
    /// receives a MAMA_TRANSPORT_RECONNECT if it subsequently succeeds. Wombat
    /// Middlware transports may be configured to retry failed connections. When a
    /// conection that a transport establishes to another transport (the publisher
    /// by convention) and the connection is lost the mamaTranpsortCallback
    /// receives the MAMA_TRANSPORT_PUBLISHER_DISCONNECT event. When a transport
    /// accepts a connection from a remote transport (the subscriber by convention)
    /// it receives either a MAMA_TRANSPORT_ACCEPT or
    /// MAMA_TRANSPORT_ACCEPT_RECONNECT event indicating that the remote client is
    /// connecting or reconnecting. When the client disconnects MAMA passes the
    /// MAMA_TRANSPORT_DISCONNECT value.</para>
    /// </remarks>    
    public interface MamaTransportCallback
    {
        /// <summary>
        /// See interface remarks for details
        /// </summary>
        void onDisconnect(MamaTransport transport);

        /// <summary>
        /// See interface remarks for details
        /// </summary>
        void onReconnect(MamaTransport transport);

        /// <summary>
        /// Called when an event occurs which affects data quality
        /// </summary>
        void onQuality(MamaTransport transport);

        /// <summary>
        /// Called when a subscriber connects to a publisher (Wombat Middleware) or when 
        /// reconnection to an RV daemon occurs (TIBRV).
        /// </summary>
        /// <param name="transport"></param>
        void onConnect(MamaTransport transport);

        /// <summary>
        /// Called when a publisher accepts a connection from a subscriber (Wombat Middleware).
        /// </summary>
        /// <param name="transport"></param>
        void onAcceptConnect(MamaTransport transport);

        /// <summary>
        /// Called when a publisher accepts a reconnection from a subscriber (Wombat Middleware).
        /// </summary>
        /// <param name="transport"></param>
        void onAcceptReconnect(MamaTransport transport);

        /// <summary>
        /// Called when a subscriber loses connection to a  publisher (Wombat Middleware).
        /// </summary>
        /// <param name="transport"></param>
        void onPublisherDisconnect(MamaTransport transport);

        /// <summary>
        /// Called on connection to a NSD (Wombat Middleware).
        /// </summary>
        /// <param name="transport"></param>
        void onNamingServiceConnect(MamaTransport transport);

        /// <summary>
        /// Called on disconnection to a NSD (Wombat Middleware).
        /// </summary>
        /// <param name="transport"></param>
        void onNamingServiceDisconnect(MamaTransport transport);
    }
}
