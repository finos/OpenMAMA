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

#ifndef MamdaSubscriptionH
#define MamdaSubscriptionH

#include <mamda/MamdaConfig.h>
#include <mama/mamacpp.h>
#include <mama/marketdata.h>
#include <mamda/MamdaFieldState.h>

namespace Wombat
{

    class MamaSource;
    class MamaTransport;
    class MamaQueue;

    class MamdaMsgListener;
    class MamdaQualityListener;
    class MamdaErrorListener;

    /**
     * A MamdaSubscription is used to register interest in a particular
     * symbol and source.  A MamaSource object is required to actually
     * activate the subscription.
     *
     * Multiple listeners can be added to the MamdaSubscription.  In this
     * way, an application can make use of more than one of the
     * specialized value added MAMDA listeners, such as MamdaTradeListener
     * and MamdaQuoteListener.
     *
     * The queue argument may be null to use Mama's internal queue. 
     */

    class MAMDAExpDLL MamdaSubscription
    {
    public:
        /**
         * Default constructor.  Use the create() method to create and
         * activate the subscription.
         */
        MamdaSubscription ();

        /**
         * Destructor.
         */
        virtual ~MamdaSubscription ();

        /**
         * Clone this MamdaSubscription.  Allocate an inactive
         * MamdaSubscription object that is initialized as a duplicate of
         * this one.  The purpose of this facility is to enable a
         * "template" of a MamdaSubscription for use with many
         * subscriptions.  The following subscription attributes are
         * copied: source, symbol, queue, subscription type,
         * service level, requires initial and timeout.
         */
        virtual MamdaSubscription* clone() const;

        /**
         * Create and activate a subscription. Set any subscription properties
         * prior to calling this method.
         */
        void create (
            MamaQueue*            queue,
            MamaSource*           source,
            const char*           symbol,
            void*                 closure = NULL);
            
        /**
         * Destroy a subscription.
         */
        void destroy ();

        /**
         * Return whether subscription is active.
         */
        bool isActive () const;

        /**
         * Set the data source name.  Do this before calling activate().
         */
        void setSource (MamaSource*  source);

        /**
         * Set the symbol.  Do this before calling activate().
         */
        void setSymbol (const char*  symbol);

        /**
         * Set the MAMA queue.  Do this before calling activate().
         */
        void setQueue (MamaQueue*  queue);

        /**
         * Set the market data type.  Do this before calling activate().
         */
        void setMdDataType (mamaMdDataType  mdDataType);

        /**
         * Set the subscription type.  Do this before calling activate().
         */
        void setType (mamaSubscriptionType  type);

        /**
         * Set the MAMA service level.  
         */
        void setServiceLevel (mamaServiceLevel  serviceLevel,
                              long              serviceLevelOpt = 0);

        /**
         * Set whether an initial value is required.  Do this before
         * calling activate().
         */
        void setRequireInitial (bool  require);

        /**
         * Set the subscription timeout (in seconds).  Do this before
         * calling activate().
         */
        void setTimeout (double  timeout);
        
        /**
         * Set the subscription retries.  Do this before
         * calling activate().
         */
        void setRetries (int  retries);

        /**
         * Set the closure.  Do this before calling activate().
         */
        void setClosure (void*  closure);

        /**
         * Set the group size hint.  Do this before calling activate().
         */
        void setGroupSizeHint (int groupSizeHint); 

        /**
         * Set the MamaSubscription.  This is normally done automatically.
         */
        void setMamaSubscription (MamaSubscription*  subscription);

        /**
         * Add a listener for regular messages.  
         */
        void addMsgListener (MamdaMsgListener*  listener);

        /**
         * Add a listener for changes in quality status.  
         */
        void addQualityListener (MamdaQualityListener*  listener);

        /**
         * Add a listener for error events.  
         */
        void addErrorListener (MamdaErrorListener*  listener);
        
        /**
         * Return the vector of message listeners.
         *
         * @return Vector of message listeners registered with the object.
         */ 
        std::vector<MamdaMsgListener*>& getMsgListeners ();
        
        /**
         * Activate the subscription.  Until this method is invoked, no
         * updates will be received.  The parameters for the subscription
         * should have been specified using the "set" methods.
         */
        void activate ();

        /**
         * Deactivate the subscription.  No more updates will be received
         * for this subscription (unless activate() is invoked again).
         *
         * This function must be called from the same thread dispatching on the
         * associated event queue unless both the default queue and dispatch queue are
         * not actively dispatching.
         */
        void deactivate ();

        /**
         * Deprecated.
         * This method is now a no-op.
         */
        void requestRecap ();

        /**
         * Return the source. Note: When using managed subscriptions this currently
         * returns NULL
         */
        MamaSource* getSource() const;

        /**
         * Return the publisher source name.
         */
        const char* getSourceName() const;
        
        /**
         * Return the symbol.
         */
        const char* getSymbol() const;
        
        /**
         * Return the exchange.
         */
        const char* getExchange() const;

        /**
         * Return the transport.
         */
        MamaTransport* getTransport() const;

        /**
         * Return the queue.
         */
        MamaQueue* getQueue() const;

        /**
         * Return the MamaSubscription object.
         */
        MamaSubscription* getMamaSubscription();

        /**
         * Return the subscription type.
         */
        mamaSubscriptionType getType() const;

        /**
         * Return the service level.
         */
        mamaServiceLevel getServiceLevel() const;

        /**
         * Return the service level option.
         */
        long getServiceLevelOpt() const;

        /**
         * Return whether an initial is required.
         */
        bool getRequireInitial() const;

        /**
         * Return the timeout (seconds).
         */
        double getTimeout() const;

        /**
         * Return the retries.
         */
        int getRetries() const;
        
        /**
         * Get the additional object passed as the closure to the create()
         * method.
         */
        void* getClosure() const;

        /**
         * Get the message-level sequence number.  This number is normally
         * sequential although there are some exceptions.  Erroneous
         * exceptions are reported via the "quality listener" interface.
         */
        uint32_t getSeqNum() const;
        
        /**
         * Set the item closure for group subscriptions.
         *
         * Setting the item closure for a non-group subscription provides
         * a second closure.
         */
        void setItemClosure (void* closure);

        /**
         * Get the item closure for group subscriptions.
         */
        void* getItemClosure (void);
        
        /**
         * Return whether the debug level for this subscription equals or
         * exceeds some level.
         *
         * @param level        The debug level to check.
         * @return whether the level equals or exceeds the set level for this
         * subscription.
         */
        bool checkDebugLevel (MamaLogLevel  level) const;
           
    private:
        struct MamdaSubscriptionImpl;
        MamdaSubscriptionImpl& mImpl;
    };

} // namespace

#endif // MamdaSubscriptionH
