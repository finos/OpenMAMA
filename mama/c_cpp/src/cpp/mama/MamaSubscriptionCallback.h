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

#ifndef MAMA_SUBSCRIPTION_CALLBACK_CPP_H__
#define MAMA_SUBSCRIPTION_CALLBACK_CPP_H__

#include "mama/mamacpp.h"

namespace Wombat 
{
    class MamaSubscription;
    /**
     * The message callback interface. Callers provide an object implementing this 
     * interface on creating a <code>MamaSubscription</code>.
     *
     *
     * @see MamaSubscription
     * @author mls
     */

    class MAMACPPExpDLL MamaSubscriptionCallback
    {
    public:
        virtual ~MamaSubscriptionCallback () 
        {};

        /**
         * Method invoked when subscription creation is complete, and
         * before any calls to <code>onMsg</code>. Since subscriptions are
         * created asynchronous by throttle, this callback provides the
         * subscription instance after the throttle processes the creation
         * request.
         *
         * @param subscription The subscription.
         */
        virtual void onCreate (MamaSubscription*  subscription) = 0;

        /**
         * Invoked if an error occurs during prior to subscription
         * creation or if the subscription receives a message for an
         * unentitled symbol.
         * 
         * If the status is <code>MamaMsgStatus.NOT_ENTITTLED</code> the
         * symbol parameter is the specific unentitled symbol. If the
         * subscription symbol contains wildcards, the subscription may
         * still receive messages for other entitled symbol.
         *
         * @param subscription The subscription.
         * @param status       The wombat error code.
         * @param symbol       The symbol for NOT_ENTITLED
         */
        virtual void onError (MamaSubscription*  subscription,
                              const MamaStatus&  status,
                              const char*        symbol) = 0;

        /**
         * Method invoked when a sequence number gap is detected. At this
         * point the topic is considered stale and the subscription will not
         * receive further messages until the feed handler satisfies a
         * recap request.
         *
         * @param subscription The subscription.
         */
        virtual void onGap (MamaSubscription*  subscription) 
        {};

        /**
         * Method invoked when a subscription has been destroyed through
	     * destroyEx.  
         *
         * @param subscription The subscription.
         */
        virtual void onDestroy (MamaSubscription*  subscription) 
        {};

        /**
         * Method invoked when a recap is requested upon detecting a
         * sequence number gap.
         *
         * @param subscription The subscription.
         */
        virtual void onRecapRequest (MamaSubscription*  subscription) 
        {};    
            
            
        /**
         * Invoked when a message arrives.
         *
         * @param subscription the <code>MamaSubscription</code>.
         * @param msg The MamaMsg which resulted in this callback being invoked.
         */
        virtual void onMsg (MamaSubscription*  subscription, 
                            MamaMsg&           msg) = 0;

        /**
         * Invoked when the quality of this subscription changes.
         *
         * @param subscription The subscription.
         * @param quality The new quality: one of the values in the
         * MamaQuality class.
         * @param symbol The symbol for this subscription.
         * @param cause The cause of the quality event
         * @param platformInfo Info associated with the quality event
         *
         * The cause and platformInfo are supplied only by some middlewares.
         * The information provided by platformInfo is middleware specific.
         * The following middlewares are supported:
         *
         * tibrv: provides the char* version of the tibrv advisory message.
         */
        virtual void onQuality (MamaSubscription*  subscription,
                                mamaQuality        quality,
                                const char*        symbol,
                                short              cause,
                                const void*        platformInfo) = 0;
       
        /* By default forward to MamaSubscription callback */
        virtual void onCreate (MamaBasicSubscription*  subscription)
        {
            onCreate ((MamaSubscription*)subscription);
        }

        virtual void onError (MamaBasicSubscription*  subscription,
                              const MamaStatus&       status,
                              const char*             symbol) 
        {
            onError ((MamaSubscription*)subscription, status, symbol);
        }

        /**
         * Invoked when a message arrives.
         *
         * @param subscription the <code>MamaSubscription</code>.
         * @param msg The TibrvMsg.
         */
        virtual void onMsg (MamaBasicSubscription*  subscription, 
                            MamaMsg&                msg) 
        {
            onMsg ((MamaSubscription*)subscription, msg);
        }
    };

} // namespace Wombat
#endif // MAMA_SUBSCRIPTION_CALLBACK_CPP_H__
