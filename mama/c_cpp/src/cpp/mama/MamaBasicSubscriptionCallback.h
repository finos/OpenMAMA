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

#ifndef MAMA_BASIC_SUBSCRIPTION_CALLBACK_CPP_H__
#define MAMA_BASIC_SUBSCRIPTION_CALLBACK_CPP_H__

#include "mama/mamacpp.h"

namespace Wombat 
{
/**
 * The message callback interface for basic subscriptions. 
 * Callers provide an object implementing this 
 * interface on creating a <code>MamaSubscription</code>.
 *
 * <p>Copyright 2003 Wombat Consulting<p>
 *
 * @see MamaSubscription
 * @author mls
 */
class MAMACPPExpDLL MamaBasicSubscriptionCallback
{
public:
    virtual ~MamaBasicSubscriptionCallback() {};

    /**
     * Method invoked when subscription creation is complete, and
     * before any calls to <code>onMsg</code>. Since subscriptions are
     * created asynchronous by throttle, this callback provides the
     * subscription instance after the throttle processes the creation
     * request.
     *
     * @param subscription The subscription.
     */
    virtual void onCreate (
        MamaBasicSubscription*  subscription) = 0;

    /**
     * Invoked if an error occurs during prior to subscription
     * creation or if the subscription receives a message for an
     * unentitled topic.
     * 
     * If the status is <code>MamaMsgStatus.NOT_ENTITTLED</code> the
     * topic parameter is the specific unentitled topic. If the
     * subscription topic contains wildcards, the subscription may
     * still receive messages for other entitled topics.
     *
     * @param subscription The subscription.
     * @param status       The wombat error code.
     * @param topic The topic for NOT_ENTITLED
     */
    virtual void onError (
        MamaBasicSubscription*  subscription,
        const MamaStatus&       status,
        const char*             topic) = 0;

    /**
     * Invoked when a message arrives.
     *
     * @param subscription the <code>MamaSubscription</code>.
     * @param msg The TibrvMsg.
     */
    virtual void onMsg (
        MamaBasicSubscription*  subscription, 
        MamaMsg&                msg) = 0;


    /**
	 * This method is invoked when a subscription has been completely destroyed,
	 * the client can have confidence that no further events will be placed 
	 * on the queue for this subscription.
	 * 
	 * @param[in] subscription The The Mama Basic Subscription.
     * @param[in] closure The closure passed to the create function.
	 */
    virtual void onDestroy (
        MamaBasicSubscription* subscription, 
        void*                  closure)
    {
    };

};


} // namespace Wombat
#endif // MAMA_BASIC_SUBSCRIPTION_CALLBACK_CPP_H__
