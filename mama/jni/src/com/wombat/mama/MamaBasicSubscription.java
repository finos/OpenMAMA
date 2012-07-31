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

/*
* Wrapper class for the native C basic subscription structure
*/
public class MamaBasicSubscription 
{
    /* ************************************************** */
    /* Static Initialisation. */
    /* ************************************************** */
    static
    {
        // Setup all the native function pointers
        initIDs();
    }

    /* ************************************************** */
    /* Private Member Variables. */
    /* ************************************************** */

    // The closure must be held in java as it may be a java object
    private Object myClosure;

    /* A long value containing a pointer to the underlying C subscription structure */
    private long basicSubscriptionPointer_i = 0;

    /* ************************************************** */
    /* Construction and Finalization. */
    /* ************************************************** */

    /**
     * The constructor allocates the underlying C subscription.
     */
    public MamaBasicSubscription()
    {
        // Allocate the underlying C subscription
        allocateSubscription();
    }

    /**
     * The finalizer deallocates the underlying C subscritpion.
     * @throws Throwable A MamaException will be thrown if the underlying
     *                   subscription could not be de-allocated.
     */
    protected void finalize() throws Throwable
    {
        try
        {
            // Only deallocate the subscription if the native pointer is valid
            if(basicSubscriptionPointer_i != 0)
            {
                deallocate();
            }
        }

        finally
        {
            // Call the base class finalizer
            super.finalize();
        }
    }

    /* ************************************************** */
    /* Public Functions. */
    /* ************************************************** */

    /**
     * Create a basic subscription without marketdata semantics.
     *
     * @param callback      The object implementing the callback functions.
     * @param transport     The MamaTransport.
     * @param queue         The MamaQueue.     
     * @param topic         The symbol name.
     * @param closure       The closure will be passed to subsequent callback invocations
     *                      for this subscription.
     */
    public void createBasicSubscription(
        final MamaBasicSubscriptionCallback  callback,
        final MamaTransport                  transport,
        final MamaQueue                      queue,
        final String                         topic,
        final Object                         closure)
    {
        // Save the closure as well
        myClosure = closure;

        // Create the native subscription
        createNativeSubscription(
            callback,
            transport,
            queue,
            topic,
            null);
    }

    /**
     * This function returns the closure supplied to the createSubscription
     * function.
     *
     * @return The closure object.
     */
    public Object getClosure()
    {
        return myClosure;
    }
    
    /**
     * This function returns the pointer to the underlying C subscription.
     * @return The pointer.
     */
    public long getPointerVal()
    {
        return basicSubscriptionPointer_i;
    }

    /**
     * This function returns the current state of the subscription as one
     * of the MamaSubscriptionState instances, see this class for more
     * information.
     *
     * @return The MamaSubscriptionState that represents the current state
     *         of the subscription.
     */
    public MamaSubscriptionState getState()
    {
        // Get the native value
        int state = getSubscriptionState();

        // Convert to a state object
        return MamaSubscriptionState.getSubscriptionState(state);
    }

    /* ************************************************** */
    /* Public Native Functions, all entries here must be fully
     * documented. */
    /* ************************************************** */

    /**
     * De-allocates the underlying C subscription. This can be used to
     * reduce time during finalization.
     */
    public native void deallocate();

    /**
     * This function will destroy the subscription, it must be called from
     * the same thread that is processing the queue for the subscription.
     * Use destroyEx to destroy the subscription from any thread.
     * Note that the subscription will not be fully destroyed until the
     * onDestroy callback is received.
     */
    public native void destroy();

    /**
     * This function will destroy the subscription and can be called from
     * any thread.
     * Note that the subscription will not be fully destroyed until the
     * onDestroy callback is received.
     * To destroy from the dispatching thread the destroy function should
     * be used in preference.
     */
    public native void destroyEx();

    /**
     * This function returns the symbol subscribed to.
     *
     * @return The symbol.
     */
    public native String getSymbol();

    /* ************************************************** */
    /* Private Native Functions. */
    /* ************************************************** */
    private native void allocateSubscription();

    /**
     * Create a basic subscription without marketdata semantics.
     *
     * @param callback      The object implementing the callback functions.
     * @param transport     The MamaTransport.
     * @param queue         The MamaQueue.
     * @param topic         The symbol name.
     * @param closure       The closure will be passed to subsequent callback invocations
     *                      for this subscription.
     */
    private native void createNativeSubscription(
        final MamaBasicSubscriptionCallback  callback,
        final MamaTransport                  transport,
        final MamaQueue                      queue,
        final String                         topic,
        final Object                         closure);

    private native int getSubscriptionState();
    private static native void initIDs();

}/*End class*/
