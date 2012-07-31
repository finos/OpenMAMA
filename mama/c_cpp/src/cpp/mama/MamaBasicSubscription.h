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

#ifndef MAMA_BASIC__SUBSCRIPTION_CPP_H__
#define MAMA_BASIC__SUBSCRIPTION_CPP_H__

#include "mama/mama.h"

namespace Wombat 
{
class MamaStatus;
class MamaMsg;
class MamaQueue;
class MamaTransport;
class MamaBasicSubscriptionCallback;

/**
 * The <code>MamaBasicSubscription</code> interface represents a
 * subscription to a topic with no market data semantics.
 *
 * @see Mama
 * @author Michael Schonberg
 * copyright 2003 Wombat Consulting Inc.
 */
class MAMACPPExpDLL MamaBasicSubscription
{
public:

    /**
     * The destructor will call <code>destroy()</code> if the 
     * subscription has not already been destroyed.
     */
    virtual ~MamaBasicSubscription ();
   
    /**
     * Constructor. Call createBasic() to activate the subscription.
     */
    MamaBasicSubscription (void);
    
    /**
     * Create a basic subscription without market data semantics.
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
    virtual void createBasic (
        MamaTransport*                 transport,
        MamaQueue*                     queue,
        MamaBasicSubscriptionCallback* callback,
        const char*                    topic,
        void*                          closure = NULL);

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

    /**
     * Return whether the subscription is active.
     *
     * @return whether the subscription is active.
     */
    virtual bool isActive (void) const;

    /**
     * Return the symbol for this subscription.
     *
     * @return The topic.
     */
    virtual const char*  getTopic (void) const;

    /**
     * Return the <code>MamaTransport</code> for this subscription.
     * @return the transport.
     */
    virtual MamaTransport* getTransport (void) const;

    /**
     * Return the <code>MamaSubscriptionCallback</code> for this
     * subscription.
     *
     * @return the callback.
     */
    virtual MamaBasicSubscriptionCallback* getBasicCallback (void) const;

    /**
     * Return the <code>MamaQueue</code> for the subscription.
     *
     * @return the queue.
     */
    virtual MamaQueue* getQueue (void) const;

    /**
     * Set the closure for the subscription.
     *
     */
    virtual void setClosure (void*  closure);
    
    /**
     * Return the closure for the subscription.
     *
     * @return the closure.
     */
    virtual void* getClosure (void) const;
    
    /**
     * Set the debug level for this subscription.
     *
     * @param level        The new debug level.
     */
    virtual void setDebugLevel (MamaLogLevel  level);

    /**
     * Return the debug level for this subscription.
     *
     * @return the debug level for this subscription.
     */
    virtual MamaLogLevel getDebugLevel () const;

    /**
     * Return whether the debug level for this subscription equals or
     * exceeds some level.
     *
     * @param level        The debug level to check.
     * @return whether the level equals or exceeds the set level for this
     * subscription.
     */
    virtual bool checkDebugLevel (MamaLogLevel  level) const;

    /**
     * This function will return the current state of the subscription, this function
     * should be used in preference to the mamaSubscription_isActive or mamaSubscription_isValid
     * functions.
     * This function is thread-safe.
     *
     * @return mama_status this can be one of the mamaSubscriptionState enumeration values.
     */
    virtual mamaSubscriptionState getState(void);

private:

    /* Private functions. */    
    static void MAMACALLTYPE onSubscriptionCreate(mamaSubscription subsc, void *closure);    
    static void MAMACALLTYPE onSubscriptionDestroy(mamaSubscription subscription, void *closure);
    static void MAMACALLTYPE onSubscriptionError(mamaSubscription subsc, mama_status status, void *platformError, const char *subject, void *closure);
    static void MAMACALLTYPE onSubscriptionMessage(mamaSubscription subsc, mamaMsg msg, void *closure, void *itemClosure);

    // The callback class passed to the create function
    MamaBasicSubscriptionCallback *mCallback;

protected:

    // The closure passed to the create function
    void *mClosure;

    // The queue passed to the create function
    MamaQueue *mQueue;

    // The underlying C subscription
    mamaSubscription mSubscription;

    // The transport passed to the create function
    MamaTransport *mTransport;
};

} // namespace Wombat
#endif // MAMA_BASIC__SUBSCRIPTION_CPP_H__
