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

#ifndef MAMA_BASIC_WILD_CARD__SUBSCRIPTION_CPP_H__
#define MAMA_BASIC_WILD_CARD__SUBSCRIPTION_CPP_H__

#include "mama/mama.h"
#include "mama/MamaBasicSubscription.h"

namespace Wombat 
{
class MamaStatus;
class MamaMsg;
class MamaQueue;
class MamaTransport;
class MamaBasicWildCardSubscriptionCallback;

/**
 * The <code>MamaBasicWildCardSubscription</code> interface represents a
 * subscription to a topic with no market data semantics.
 *
 * @see Mama
 * @author Michael Schonberg
 * copyright 2003 Wombat Consulting Inc.
 */
class MAMACPPExpDLL MamaBasicWildCardSubscription : public MamaBasicSubscription
{
public:

    /**
     * The destructor will call <code>destroy()</code> if the 
     * subscription has not already been destroyed.
     */
    virtual ~MamaBasicWildCardSubscription ();
   
    /**
     * Constructor. Call createBasicWildCard() to activate the subscription.
     */
    MamaBasicWildCardSubscription (void);
    
    /**
     * Create a basic wild card subscription without market data semantics.
     *
     * If the topic is NULL for WMW, this method creates a "transport"
     * subscription that subscribes to all the topics from publishers with the
     * the "publish_name" property equal to the source value. 
     *
     * @param transport The transport to use. Must be a basic transport.
     * @param queue The queue.
     * @param callback The mamaMsgCallbacks structure containing the three
     * callback methods.
     *
     * @param topic The topic.
     * @param closure The caller supplied closure.
     *
     */
    virtual void create (
        MamaTransport*                 transport,
        MamaQueue*                     queue,
        MamaBasicWildCardSubscriptionCallback* callback,
        const char*                    source,
        const char*                    topic,
        void*                          closure = NULL);

    /**
     * Return the symbol for this subscription.
     *
     * @return The topic.
     */
    virtual const char*  getSymbol (void) const;
    
    /**
     * Return the source  for this subscription.
     *
     * @return The topic.
     */
    virtual const char*  getSource (void) const;

    /**
     * Get the closure for the specific wildcard topic.
     *
     * This method may only be called from the onMsg callback.
     *
     * @ return The closure specified by ::setTopicClosure() or NULL if no
     * topic closure set.
     */
    virtual void* getTopicClosure() const;

    /**
     *  Set the topic closure for the current message's topic. 
     * 
     * This method can only be invoked from the onMsg() callback.
     *
     * @prarm closure The closure.
     */
    virtual void setTopicClosure (void *closure);

    /**
     * For "transport" subscriptions (WMW only) stop processing messages for
     * the current topic.
     */
    virtual void muteCurrentTopic (void);

    /**
     * Return the <code>MamaSubscriptionCallback</code> for this
     * subscription.
     *
     * @return the callback.
     */
    virtual MamaBasicWildCardSubscriptionCallback* getBasicWildCardCallback (void) const;

private:

    /* Private functions. */    
    static void MAMACALLTYPE onSubscriptionCreate(mamaSubscription subsc, void *closure);    
    static void MAMACALLTYPE onSubscriptionDestroy(mamaSubscription subscription, void *closure);
    static void MAMACALLTYPE onSubscriptionError(mamaSubscription subsc, mama_status status, void *platformError, const char *subject, void *closure);
    static void MAMACALLTYPE onSubscriptionMessage(mamaSubscription subsc, mamaMsg msg, const char *topic, void *closure, void *topicClosure);

    // The callback class passed to the create function
    MamaBasicWildCardSubscriptionCallback *mCallback;
};

} // namespace Wombat
#endif // MAMA_BASIC_WILD_CARD__SUBSCRIPTION_CPP_H__
