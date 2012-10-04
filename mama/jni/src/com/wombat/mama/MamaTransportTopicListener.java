/* $Id:
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

public class MamaTransportTopicListener
{
    /**
     * Invoked when a topic is subscribed to
     *
     * @param cause The cause of the topic subscription
     * @param platformInfo Info associated with the topic subscription
     *
     * The cause and platformInfo are supplied only by some middlewares.
     * The information provided by platformInfo is middleware specific.
     * The following middlewares are supported:
     *
     * tibrv: provides the String object version of the tibrv advisory message
     * Wombat Middleware
     */
    public void onTopicSubscribe(short cause, final Object platformInfo)
    {
        return;
    }

    /**
     * Invoked when a topic is unsubscribed to
     *
     * @param cause The cause of the topic unsubscription
     * @param platformInfo Info associated with the topic unsubscription
     *
     * The cause and platformInfo are supplied only by some middlewares.
     * The information provided by platformInfo is middleware specific.
     * The following middlewares are supported:
     *
     * tibrv: provides the String object version of the tibrv advisory message
     * Wombat Middleware
     */
    public void onTopicUnsubscribe(short cause, final Object platformInfo)
    {
        return;
    }
}

