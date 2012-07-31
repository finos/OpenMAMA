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

/**
 * The message callback interface. Callers provide an object implementing this
 * interface on creating a <code>MamaSubscription</code>.
 *
 * @see MamaSubscription
 */
public interface MamaSubscriptionCallback
{
    /**
     * Method invoked when subscription creation is complete.
     * Since subscriptions are created asynchronous by throttle, this callback
     * provides the subscription instance after the throttle processes the 
     * creation request.
     * In the case where a subscription is created on a queue other than the default
     * it is possible for <code>onMsg</code> calls to be processed to be called before 
     * the <code>onCreate</code> callback is processed.
     *
     * @param subscription The subscription.
     */
    void onCreate (MamaSubscription subscription);

    /**
     * Invoked if an error occurs during subscription creation or if the      
     * subscription receives a message for an unentitled subject.
     * <p>
     * If the status
     * is <code>MamaStatus.NOT_ENTITTLED</code> the subject parameter is the
     * specific unentitled subject. If the subscription subject contains         
     * wildcards, the subscription may still receive messages for other
     * entitled subjects.
     *
     * @param subscription The subscription.
     * @param wombatStatus The wombat error code.
     * @param platformError Third party, platform specific messaging error.
     * @param subject The subject for NOT_ENTITLED
     * @param e                                                                 
     */
    void onError(MamaSubscription subscription,
                 short wombatStatus,
                 int platformError,
                 String subject, Exception e); 

    /**
     * Invoked when a message arrives.
     *
     * @param subscription the <code>MamaSubscription</code>.
     * @param msg The MamaMsg.
     */
    void onMsg (MamaSubscription subscription, MamaMsg msg);

    /**
     * Invoked when a the quality of this subscription changes.
     *
     * @param subscription the <code>MamaSubscription</code>.
     * @param quality The new quality: one of the values in the MamaQuality class.
     * @param cause The cause of the data quality event.
     * @param platformInfo Info associated with the data quality event.
     *
     * The cause and platformInfo are supplied only by some middlewares.
     * The information provided by platformInfo is middleware specific.
     * The following middlewares are supported:    
     *
     * tibrv: provides the String object version of the tibrv advisory message
     */
    void onQuality (MamaSubscription subscription, 
                    short            quality,
                    short            cause,
                    final Object     platformInfo );
    
    /**
     * Method invoked when a recap is requested upon detecting a
     * sequence number gap.
     *
     * @param subscription the <code>MamaSubscription</code>.
     */               
    void onRecapRequest (MamaSubscription subscription);
     
     /**
     * Method invoked when a sequence number gap is detected. At this
     * point the topic is considered stale and the subscription will not
     * receive further messages until the feed handler satisfies a
     * recap request.
     *
     * @param subscription the <code>MamaSubscription</code>.
     */
    void onGap (MamaSubscription subscription);

     /**
     * This method is invoked when a subscription has been completely destroyed or deactivated,
     * the client can have confidence that no further messages will be placed on the queue
     * for this subscription.
     * 
     * @param subscription The MamaSubscription.
     */
    void onDestroy(MamaSubscription subscription);
}
