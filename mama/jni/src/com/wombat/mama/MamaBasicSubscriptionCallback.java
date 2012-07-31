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

public interface MamaBasicSubscriptionCallback
{
    /**
     * Method invoked when subscription creation is complete, and before any calls
     * to <code>onMsg</code>. Since subscriptions are created asynchronous by
     * throttle, this callback provides the subscription instance after the
     * throttle processes the creation request.
     *
     * @param subscription The subscription.
     */
    void onCreate (MamaBasicSubscription subscription);

    /**
     * Invoked if an error occurs during prior to subscriptionBridge creation or if the
     * subscriptionBridge receives a message for an unentitled subject.
     * <p>
     * If the status
     * is <code>MamaStatus.NOT_ENTITTLED</code> the subject parameter is the
     * specific unentitled subject. If the subscriptionBridge subject contains
     * wildcards, the subscriptionBridge may still receive messages for other
     * entitled subjects.
     *
     * @param subscription The subscriptionBridge.
     * @param wombatStatus The wombat error code.
     * @param platformError Third party, platform specific messaging error.
     * @param subject The subject for NOT_ENTITLED
     */
    void onError (MamaBasicSubscription subscription,
                  short wombatStatus,
                  int platformError,
                  String subject);

    /**
     * Invoked when a message arrives.
     *
     * @param subscription the <code>MamaSubscription</code>.
     * @param msg The MamaMsg.
     */
    void onMsg (MamaBasicSubscription subscription, MamaMsg msg);

    /**
     * This method is invoked when a subscription has been completely destroyed or deactivated,
     * the client can have confidence that no further messages will be placed on the queue
     * for this subscription.
     * 
     * @param subscription The MamaBasicSubscription.
     */
    void onDestroy(MamaBasicSubscription subscription);
}
