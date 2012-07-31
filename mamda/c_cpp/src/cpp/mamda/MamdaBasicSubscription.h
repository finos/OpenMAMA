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

#ifndef MamdaBasicSubscriptionH
#define MamdaBasicSubscriptionH

#include <mamda/MamdaConfig.h>
#include <mama/subscriptiontype.h>
#include <mama/mamacpp.h>

namespace Wombat
{

    class MamaTransport;
    class MamaQueue;

    class MamdaBasicMsgListener;
    class MamdaBasicQualityListener;
    class MamdaBasicErrorListener;


    /**
     * A MamdaBasicSubscription is used to register interest in a particular
     * symbol. A MamaTransport is required to actually
     * activate the subscription.
     *
     * Multiple listeners can be added to the MamdaBasicSubscription.  In this
     * way, an application can make use of more than one of the
     * specialized value added MAMDA listeners, such as MamdaCalcListener
     */

    class MAMDAExpDLL MamdaBasicSubscription
    {
    public:

        MamdaBasicSubscription ();

        ~MamdaBasicSubscription ();

        /**
        * Create and activate the subscription. You can alternatively set all
        * objects and properties and then call activate is you wish.
        */
        void create (
            MamaTransport*   transport,
            MamaQueue*       queue,
            const char*      symbol,
            void*            closure = NULL);

        /**
         * Set the subscription timeout (in seconds).  Do this before
         * calling activate().
         */
        void setTimeout (double  timeout);

        /**
         * Add a listener for regular messages.  
         */
        void addMsgListener (MamdaBasicMsgListener*  listener);

        /**
         * Add a listener for changes in quality status.  
         */
        void addQualityListener (MamdaBasicQualityListener*  listener);

        /**
         * Add a listener for error events.  
         */
        void addErrorListener (MamdaBasicErrorListener*  listener);

        /**
         * Activate the subscription.  Until this method is invoked, no
         * updates will be received. Is called by create().
         */
        void activate ();

        /**
         * Deactivate the subscription.  No more updates will be received
         * for this subscription (unless activate() is invoked again).
         */
        void deactivate ();
        
        /**
         * Set the symbol for the subscription. Do this before calling activate().
         */
        void setSymbol (const char* symbol);
        
        /**
         * Get the symbol for the subscription.
         */
        const char* getSymbol () const;

        /**
         * Set the closure for the subscription. Do this before calling
         * activate().
         */
        void setClosure (void* closure);
        
        /**
         * Get the additional object passed as the closure to the create()
         * method.
         */
        void* getClosure() const;

        /**
         * Set the MAMA transport. Do this before calling activate().
         */
        void setTransport (MamaTransport* transport);

        /**
         * Get the MamaTransport for this subscription
         */
        MamaTransport* getTransport() const;

        /**
         * Set the MAMA queue. Do this before calling activate().
         */
        void setQueue (MamaQueue* queue);
        
        /**
         * Get the MAMA queue for this subscription.
         */
        MamaQueue* getQueue() const;

    private:
        struct MamdaBasicSubscriptionImpl;
        MamdaBasicSubscriptionImpl& mImpl;
    };

} // namespace

#endif // MamdaBasicSubscriptionH
