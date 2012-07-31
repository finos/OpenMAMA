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

#ifndef MAMA_SUBSCRIPTION_CPP_H__
#define MAMA_SUBSCRIPTION_CPP_H__

#include "mama/MamaBasicSubscription.h"
#include "mama/subscription.h"

namespace Wombat 
{
    class MamaSubscriptionCallback;
    class MamaSource;
    class MamaSourceDerivative;

    /**
     * The <code>MamaSubscription</code> interface represents a
     * subscription to a topic. It provides transparent market data
     * semantics and functionality including initial value requests, recap
     * requests, subscription management and data quality.
     *
     */

    class MAMACPPExpDLL MamaSubscription : public MamaBasicSubscription
    {
    public:
        virtual ~MamaSubscription ();

        /* Construct a MamaSubscription. MamaSubscription::createXXX() must be
         * called to activate the subscription.
         */
        MamaSubscription (void);

        virtual void              setCSubscription (mamaSubscription subscription);
        virtual mamaSubscription  getCSubscription ();
            
        /**
         * Set up a subscription.
         *
         * @param callback  The callback.
         * @param transport  The transport.
         * @param queue  The mama queue.
         * @param source  The data source name for the listener.
         * @param symbol  The symbol for the listener.
         * @param closure  The caller supplied closure.
         */
         virtual void setup (MamaTransport*              transport,
                             MamaQueue*                  queue,
                             MamaSubscriptionCallback*   callback,
                             const char*                 source,
                             const char*                 symbol,
                             void*                       closure     = NULL);
        
        /**
         * Set up a subscription.
         *
         * @param queue The mama queue. 
         * @param callback  The callback.
         * @param source  The MamaSource identifying the publisher for this
         * symbol.
         * @param symbol  The symbol for the listener.
         * @param closure  The caller supplied closure.
         */
         virtual void setup (MamaQueue*                  queue,
                             MamaSubscriptionCallback*   callback,
                             MamaSource*                 source,
                             const char*                 symbol,
                             void*                       closure     = NULL);

        /**
         * Activate a subscriber using the throttle queue. This method
         * places a request to create a subscriber on the throttle queue
         * which dispatches tasks that produce messages at a controlled
         * rate. The rate is determined by the outbound throttle rate of
         * the underlying <code>MamaTransport</code>.
         * 
         * In the event that listener creation fails as the result of an
         * messaging related error the callback is invoked with
         * information regarding the error.
         * 
         * If entitlements are enabled, and the caller is not entitled to
         * the requested symbol, the first invocation of the callback will
         * invoked with status "MamaMsgStatus.NOT_ENTITLED".
         * 
         * As an added convenience, callers may implement the
         * <code>onComplete</code> and <code>onError</code> members of
         * <code>MamaSubscriptionCallback</code>.  <code>onComplete</code>
         * is invoked prior to the arrival of any initial message
         * signalling the successful creation of the listener.
         * <code>onError</code> is invoked if a TIBRV or entitlement error
         * occurs prior to listener creation.
         * 
         * It is also possible for an entitlement error to occur after a
         * listener is created. This occurs when the entitlement
         * information is included in the initial message sent by the feed
         * handler as is often the case.
         * 
         * If an error occurs during listener
         * creation. <code>destroy</code> is called automatically.
         */
         virtual void activate ();

        /**
         * Deactivate a subscriber.  The subscription can be reactivated
         * using activate().
         */
         virtual void deactivate ();   
        

        /**
         * Set up and activate a subscriber using the throttle queue. This
         * method is equivalent to calling setup() followed by activate().
         *
         * @param transport  The transport.
         * @param queue The mama queue.
         * @param callback  The callback.
         * @param source  The data source name for the listener.
         * @param symbol  The symbol for the listener.
         * @param closure The caller supplied closure.
         */
         virtual void create (MamaTransport*              transport,
                              MamaQueue*                  queue,
                              MamaSubscriptionCallback*   callback,
                              const char*                 source,
                              const char*                 symbol,
                              void*                       closure     = NULL);
        
        /**
         * Set up and activate a subscriber using the throttle queue. This
         * method is equivalent to calling setup() followed by activate().
         *
         * @param queue The mama queue.
         * @param callback  The callback.
         * @param source  The MamaSource identifying the publisher for this
         * symbol.
         * @param symbol  The symbol for the listener.
         * @param closure The caller supplied closure.
         */
         virtual void create (MamaQueue*                  queue,
                              MamaSubscriptionCallback*   callback,
                              MamaSource*                 source,
                              const char*                 symbol,
                              void*                       closure     = NULL);

        /**
         * Set up and activate a snapshot subscriber using the throttle
         * queue. This method is equivalent to calling setup() followed by
         * setServiceLevel(MAMA_SERVICE_LEVEL_SNAPSHOT,0) followed by
         * activate().
         *
         * @param transport  The transport.
         * @param queue The mama queue.
         * @param callback  The callback.
         * @param source  The data source name for the listener.
         * @param symbol  The symbol for the listener.
         * @param closure The caller supplied closure.
         */
        virtual void createSnapshot (MamaTransport*               transport,
                                     MamaQueue*                   queue,
                                     MamaSubscriptionCallback*    callback,
                                     const char*                  source,
                                     const char*                  symbol,
                                     void*                        closure = NULL);

        /**
         * Determines whether the subscription requires an initial value. Must be
         * set before calling createXXX(). Default is true. Not applicable for
         * snapshot subscriptions as they simply request an initial value.
         *
         * @param requiresInitial True if an initial value is required
         */
        virtual void setRequiresInitial (bool requiresInitial);

        /**
         * Return true if the subscription requires an initial value.
         */
        virtual bool getRequiresInitial (void);

        /**
         * Return true if the subscription has received an initial.
         */
        virtual bool getReceivedInitial (void);

        /**
         * Set the number of retries for initial value requests and recap
         * requests. This must called before createXXX() to affect the initial
         * value requests. Calling it after createXXX() only affects recap
         * requests. The default is MAMA_DEFAULT_RETRIES.
         *
         * @param retries The number of time to retry the initial value request.    
         */
        virtual void setRetries (int retries);

        /**
         * Return the retries.
         */
        virtual int getRetries (void);

        /**
         * Set the subscription type. The default is normal.
         *
         * @param type The type of subscription (normal, group, order book, etc.).
         */
        virtual void setSubscriptionType (mamaSubscriptionType type);

        /**
         * Return the subscription type.
         */
        virtual mamaSubscriptionType getSubscriptionType (void);

        /**
         * Return the service level option.
         */
        virtual long getServiceLevelOpt (void);

        /**
         * Set the service level. This method must be invoked before createXXX().
         *
         * @param svcLevel The service level of the subscription (real
         * time, snapshot, etc.). The default is real time.
         * @param svcLevelOpt An optional argument for certain service levels.
         */
        virtual void setServiceLevel (mamaServiceLevel svcLevel, 
                                      long             svcLevelOpt);

        /**
         * Return the service level.
         */
        virtual mamaServiceLevel getServiceLevel (void);

        /**
         * Return the symbol for this subscription.
         *
         * @return The topic.
         */
        virtual const char*  getSymbol (void) const;

        virtual MamaSubscriptionCallback*   getCallback (void) const;
        
        /**
         * Sets the symbol for this subscription.
         * Should generally only be used for updating symbology mappings.
         */
        virtual void setSymbol (const char* symbol);

        /**
         * Return the (subscription-specific) MAMA source derivative for
         * this subscription.
         *
         * @return The source derivative.
         */
        virtual const MamaSourceDerivative*  getSourceDerivative (void) const;

        /**
         * Return the (subscription-specific) MAMA source derivative for
         * this subscription.
         *
         * @return The source derivative.
         */
        virtual MamaSourceDerivative*  getSourceDerivative (void);

        /**
         * Return the MAMA source for this subscription.
         *
         * @return The source.
         */
        virtual const MamaSource*  getSource (void) const;

        /**
         * Return the source for this subscription.
         *
         * @return The source.
         */
        virtual const char*  getSubscSource (void) const;

        /**
         * Set the timeout for this subscription. The timeout is used for
         * requesting initial values, and recaps.
         *
         * @param timeout The timeout in seconds.
         */
        virtual void setTimeout (double timeout);

        /**
         * Return the timeout.
         */
        virtual double getTimeout (void);

        /**
         * Attempt to recover from sequence number gaps by requesting a
         * recap.
         *
         * @param recover true enables recovery attempts.
         */
        virtual void setRecoverGaps (bool recover);

        /**
         * Returns true if listener is configure to recover from sequence
         * number gaps by requesting a recap.
         *
         * @return true if gap recover is enabled.
         */
        virtual bool getRecoverGaps (void) const;

        /**
         * Set the application data type. The default is 0.
         *
         * @param dataType The application-specific data type (e.g., market data).
         */
        virtual void setAppDataType (uint8_t dataType);

        /**
         * Get the application data type. The default is 0.
         *
         * @return The application-specific data type (e.g., market data).
         */
        virtual uint8_t getAppDataType () const;

        /**
         * Set a hint to the size of groups when making group subscriptions.
         *
         * @param groupSizeHint Approximate expected group size
         */
        virtual void setGroupSizeHint (int groupSizeHint); 

               /**
         * Set the item closure for group subscriptions.
         *
         * Group subscriptions receive updates for multiple symbols. This
         * method allows calls to set a per-symbol closure which will be
         * passed as the fourth argument to subsequent calls to the onMsg
         * callback. This method may only be called during the onMsg
         * callback.
         *
         * Setting the item closure for a non-group subscription provides
         * a second closure.
         */
        virtual void setItemClosure (void*  closure);

        /**
         * Get the item closure for group subscriptions. See
         * setItemClosure. When invoked during an onMsg callback this
         * method returns the closure for the current item in a group
         * subscription. When invoked outside an onMsg callback, it
         * returns the closure from the most recent callback.
         */
        virtual void* getItemClosure (void);
         
        /**
         * Set the number of messages to cache for each symbol before the initial
         * value arrives. This allows the subscription to recover when the initial
         * value arrives late (after a subsequent trade or quote already arrived).
         *
         * For group subscription, a separate cache is used for each group member.
         *
         * The default is 10.
         *
         * @param cacheSize The size of the cache.
         */
        virtual void setPreInitialCacheSize (int cacheSize);
        
        /**
         * Return the initial value cache size.
         *
         * @return The cache size.
         */
        virtual int getPreInitialCacheSize (void);

        /**
         * Set a filter to discard messages.
         *
         * @param ignoreDefinitelyDuplicate If true callbacks will not be invoked
         * for messages where MamaMsg::getIsDefinitelyDuplicate returns true.
         *
         * @param ignorePossiblyDuplicate If true callbacks will not be invoked
         * for messages where MamaMsg::getIsPossiblyDuplicate returns true.
         *
         * @param ignoreDefinitelyDelyaed If true callbacks will not be invoked
         * for messages where MamaMsg::getIsDefinitelyDelayed returns true.
         *
         * @param ignorePossiblyDelayed If true callbacks will not be invoked
         * for messages where MamaMsg::getIsPossiblyDelayed returns true.
         *
         * @param ignoreOutOfSequence If true callbacks will not be invoked for
         * messages where MamaMsg::getIsOutOfSequence returns true.
         */
        virtual void setMsgQualifierFilter (bool ignoreDefinitelyDuplicate,
                                            bool ignorePossiblyDuplicate,
                                            bool ignoreDefinitelyDelyaed,
                                            bool ignorePossiblyDelayed,
                                            bool ignoreOutOfSequence);

        /**
         * Get the filters that discard message according to the message
         * qualifier.
         *
         * @param ignoreDefinitelyDuplicate If true callbacks will not be invoked
         * for messages where MamaMsg::getIsDefinitelyDuplicate returns true.
         *
         * @param ignorePossiblyDuplicate If true callbacks will not be invoked
         * for messages where MamaMsg::getIsPossiblyDuplicate returns true.
         *
         * @param ignoreDefinitelyDelayed If true callbacks will not be invoked
         * for messages where MamaMsg::getIsDefinitelyDelayed returns true.
         *
         * @param ignorePossiblyDelayed If true callbacks will not be invoked
         * for messages where MamaMsg::getIsPossiblyDelayed returns true.
         *
         * @param ignoreOutOfSequence If true callbacks will not be invoked for
         * messages where MamaMsg::getIsOutOfSequence returns true.
         */
        virtual void getMsgQualifierFilter (bool &ignoreDefinitelyDuplicate,
                                            bool &ignorePossiblyDuplicate,
                                            bool &ignoreDefinitelyDelyaed,
                                            bool &ignorePossiblyDelayed,
                                            bool &ignoreOutOfSequence) const;

        /**
         * Destroy the subscription.
         * <p>
         * Destroys the underlying subscription. The subscription can be recreated
         * via a subsequent call to create()
         */
        virtual void destroy ();

        /**
         * This function will destroy the subscription and can be called from
         * any thread.
         * Note that the subscription will not be fully destroyed until the
         * onDestroy callback is received from the MamaBasicSubscriptionCallback
         * interface.
         * To destroy from the dispatching thread the destroy function should be 
         * used in preference.
         */
        virtual void destroyEx();

    private:

        /* Private functions. */    
        static void MAMACALLTYPE onSubscriptionCreate       (mamaSubscription subscription, 
                                                             void             *closure);

        static void MAMACALLTYPE onSubscriptionDestroy      (mamaSubscription subscription, 
                                                             void             *closure);

        static void MAMACALLTYPE onSubscriptionError        (mamaSubscription subscription, 
                                                             mama_status      status, 
                                                             void             *platformError, 
                                                             const char       *subject, 
                                                             void             *closure);

        static void MAMACALLTYPE onSubscriptionGap          (mamaSubscription subscription, 
                                                             void*            closure);

        static void MAMACALLTYPE onSubscriptionMessage      (mamaSubscription subscription, 
                                                             mamaMsg          msg, 
                                                             void             *closure, 
                                                             void             *topicClosure);

        static void MAMACALLTYPE onSubscriptionQuality      (mamaSubscription subscription, 
                                                             mamaQuality      quality, 
                                                             const char       *symbol, 
                                                             short            cause, 
                                                             const void       *platformInfo, 
                                                             void             *closure);

        static void MAMACALLTYPE onSubscriptionRecapRequest (mamaSubscription subscription, 
                                                             void*            closure);    

        // The callback class passed to the create function
        MamaSubscriptionCallback *mCallback;

        // The source derivative
        MamaSourceDerivative *mSourceDeriv;    
    };

} // namespace Wombat
#endif // MAMA_SUBSCRIPTION_CPP_H__
