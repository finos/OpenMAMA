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

package com.wombat.mama;

public interface MamaTransportListener
{
    /**
     * Invoked when transport disconnects.
     *
     * @param cause The cause of the transport disconnection
     * @param platformInfo Info associated with the transport disconnection
     *
     * The cause and platformInfo are supplied only by some middlewares.
     * The information provided by platformInfo is middleware specific.
     * The following middlewares are supported:
     *
     * tibrv: provides the String object version of the tibrv advisory message
     * Wombat Middleware (JNI only)
     */
    public void onDisconnect(short cause, final Object platformInfo);

    /**
     * Invoked when transport reconnects.
     *
     * @param cause The cause of the transport reconnection
     * @param platformInfo Info associated with the transport reconnection
     *
     * The cause and platformInfo are supplied only by some middlewares.
     * The information provided by platformInfo is middleware specific.
     * The following middlewares are supported:
     *
     * tibrv: provides the String object version of the tibrv advisory message
     * Wombat Middleware
     */
    public void onReconnect(short cause, final Object platformInfo);

    /**
     * Invoked on data quality event.
     *
     * @param cause The cause of the data quality event
     * @param platformInfo Info associated with the data quality event
     *
     * The cause and platformInfo are supplied only by some middlewares.
     * The information provided by platformInfo is middleware specific.
     * The following middlewares are supported:
     *
     * tibrv: provides the String object version of the tibrv advisory message
     * Wombat Middleware
     */
    public void onQuality (short cause, final Object platformInfo);

   /** 
    * Invoked on the subscriber when the transport connects.
    * 
    * @param cause The cause of the transport connection
    * @param platformInfo Info associated with the transport connection
    *
    * The following middlewares are supported:
    *
    * Wombat Middleware
    */
    public void onConnect (short cause, final Object platformInfo);

    /**
     * Invoked on the publisher when the transport accepts a connection.
     *
     * @param cause The cause of the transport connection
     * @param platformInfo Info associated with the transport connection
     *
     * The following middlewares are supported:
     *
     * Wombat Middleware
     */
    public void onAccept (short cause, final Object platformInfo);

    /**
     * Invoked on the publisher when the transport accepts a reconnection.
     *
     * @param cause The cause of the reconnection
     * @param platformInfo Info associated with the reconnection
     *
     * The following middlewares are supported:
     *
     * Wombat Middleware
     */
    public void onAcceptReconnect (short cause, final Object platformInfo);

    /**
     * Invoked on the subscriber when the transport disconnects from the publisher.
     * 
     * @param cause The cause of the disconnection
     * @param platformInfo Info associated with the disconnection
     *
     * The following middlewares are supported:
     *
     * Wombat Middleware
     */
    public void onPublisherDisconnect (short cause, final Object platformInfo);

    /**
     * Invoked on the subscriber on connection to the naming service.
     * 
     * @param cause The cause of the connection
     * @param platformInfo Info associated with the connection
     *
     * The following middlewares are supported:
     *
     * Wombat Middleware
     */
    public void onNamingServiceConnect (short cause, final Object platformInfo);

    /**
     * Invoked on the subscriber on disconnection to the naming service.
     * 
     * @param cause The cause of the disconnection
     * @param platformInfo Info associated with the disconnection
     *
     * The following middlewares are supported:
     *
     * Wombat Middleware
     */
    public void onNamingServiceDisconnect (short cause, final Object platformInfo);
}

