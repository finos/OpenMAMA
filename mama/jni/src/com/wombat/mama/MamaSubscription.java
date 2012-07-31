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
import java.util.logging.*;

/*
 * Wrapper class for the native C subscription structure
 */
public class MamaSubscription
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
    /* Constants. */
    /* ************************************************** */
    // Default number of connection retries when obtaining the dictionary
    public static final int DEFAULT_RETRIES = 2;

    // The default timeout when creating the dictionary
    public static final double DEFAULT_TIMEOUT = 10.0;

    /* ************************************************** */
    /* Private Member Variables. */
    /* ************************************************** */

    /* A long value containing a pointer to the underlying C subscription structure */
    private long  subscriptionPointer_i = 0;

    /* The closure must be held in Java as it may be a Java object */
    private Object myClosure;

    /* We maintain this reference to facilitate getSource()  - easier than
     * returning it from the C code.
     */
    private MamaSource mySource = null;

    /* The type of the subscription. */
    private MamaSubscriptionType mySubType = MamaSubscriptionType.NORMAL;

    /* ************************************************** */
    /* Construction and Finalization. */
    /* ************************************************** */

    /**
     * The constructor allocates the underlying C subscription.
     */
    public MamaSubscription()
    {
        // Allocate the underlying C subscription
        allocateSubscription();
    }

    /**
     * The finalizer deallocates the underlying C subscription.
     * @throws Throwable A MamaException will be thrown if the underlying
     *                   subscription could not be de-allocated.
     */
    protected void finalize() throws Throwable
    {
        try 
        {
            // Only deallocate the subscription if the native pointer is valid
            if(subscriptionPointer_i != 0)
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
     * Create and activate subscription. This is effectively a pair of
     * calls to mamaSubscription_setup() and mamaSubscription_activate().
     *
     * @param callback      The object implementing the callback functions. 
     * @param queue         The MamaQueue.
     * @param source        The MamaSource identifying the publisher for this symbol.
     * @param symbol        The symbol name.
     * @param closure       The closure will be passed to subsequent callback invocations
     *                      for this subscription.
     */
    public void createSubscription(
        MamaSubscriptionCallback  callback,
        MamaQueue                 queue,
        MamaSource                source,
        String                    symbol,
        Object                    closure)
    {
        // Save the source in the Java layer to prevent a context switch to C
        mySource = source;

        // Save the closure as well
        myClosure = closure;

        // Create the native subscription
        createNativeSubscription(
            callback,
            queue,
            source,
            symbol,
            null);
    }

    /**
     * Create a dictionary subscription using default values for the timeout and
     * number of retries.
     *
     * @param callback      The object implementing the callback functions, this can
     *                      also be an instance of MamaSubscriptionCallbackEx to
     *                      be notified when the subscription has been destroyed.
     * @param queue         The MamaQueue.
     * @param source        The MamaSource identifying the publisher for this symbol.
     */
    public MamaDictionary createDictionarySubscription(
        MamaDictionaryCallback  callback,
        MamaQueue               queue,
        MamaSource              source)
    {
        // Simply call the overload
        return createDictionarySubscription (
                callback,
                queue,
                source,
                DEFAULT_TIMEOUT,
                DEFAULT_RETRIES);
    }

    /**
     * Create a dictionary subscription using default values for the timeout and
     * number of retries.
     *
     * @param callback      The object implementing the callback functions.
     * @param queue         The MamaQueue.
     * @param source        The MamaSource identifying the publisher for this symbol.
     * @param timeout       The timeout value in seconds.
     * @param retries       The number of retries.
     */
    public MamaDictionary createDictionarySubscription(
        MamaDictionaryCallback  callback,
        MamaQueue               queue,
        MamaSource              source,
        double                  timeout,
        int                     retries)
    {
        // Save the source to prevent additional context switches into C
        mySource = source;

        // Create the native dictionary subscription
        return createNativeDictionarySubscription(callback, queue, source, timeout, retries);
    }

    /**
     * Create a snapshot subscription for initial value only (no updates).
     * This function is equivalent to mamaSubscription_create () with
     * svcLevel set to MAMA_SERVICE_LEVEL_SNAPSHOT and default arguments
     * for type, svcLevelOpt, requiresInitial, retries, timeout.
     *
     * @param callback      The object implementing the callback functions.
     * @param queue         The MamaQueue.
     * @param source        The MamaSource identifying the publisher for this symbol.
     * @param symbol        The symbol name.
     * @param closure       The closure will be passed to subsequent callback invocations
     *                      for this subscription.
     */
    public void createSnapshotSubscription(
        MamaSubscriptionCallback  callback,
        MamaQueue                 queue,
        MamaSource                source,
        String                    symbol,
        Object                    closure)
    {
        // Set the type and service level before the subscription is created
        setSubscriptionType (MamaSubscriptionType.NORMAL);
        setServiceLevel     (MamaServiceLevel.SNAPSHOT,0);
        setRequiresInitial  (true);

        // Save the source in the Java layer to prevent a context switch to C
        mySource = source;

        // Save the closure
        myClosure = closure;

        // Create the native subscription
        createNativeSubscription(
            callback,
            queue,
            source,
            symbol,
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
        return subscriptionPointer_i;
    }

    /**
     * Return the source associated with the subscription.
     *
     * @return The MamaSource object.
     */
    public MamaSource getSource ()
    {
        return mySource;
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

    /**
     * Setup a basic subscription without marketdata semantics. Note that
     * activate must still be called.
     *
     * @param callback      The object implementing the callback functions.
     * @param queue         The MamaQueue.
     * @param source        The MamaSource identifying the publisher for this symbol.
     * @param symbol        The symbol name.
     * @param closure       The closure will be passed to subsequent callback invocations
     *                      for this subscription.
     */
    public void setupSubscription(
        MamaSubscriptionCallback  callback,
        MamaQueue                 queue,
        MamaSource                source,
        String                    symbol,
        Object                    closure)
    {
        // Save the source in the Java layer to prevent a context switch to C
        mySource = source;

        // Create the native subscription
        setupNativeSubscription(
            callback,
            queue,
            source,
            symbol,
            closure);
    }
    
    public void setAppDataType (MamaMdDataType type)
    {
        final String METHOD_NAME = "setAppDataType(): ";
        throw new MamaException(METHOD_NAME+"Not yet supported");
    }

    public MamaMdDataType getAppDataType ()
    {
        final String METHOD_NAME = "getAppDataType(): ";
        throw new MamaException(METHOD_NAME+"Not yet supported");
    }

    public void setDebugLevel (Level level)
    {
        final String METHOD_NAME = "setDebugLevel(): ";
        throw new MamaException(METHOD_NAME+"Not yet supported");
    }
    public void setMessageQualifierFilter (MamaMsgQualifier qualifier)
    {
        final String METHOD_NAME = "setMessageQualifierFilter(): ";
        throw new MamaException(METHOD_NAME+"Not yet supported");
    }

    public MamaMsgQualifier getMessageQualifierFilter ()
    {
        final String METHOD_NAME = "getMessageQualifierFilter(): ";
        throw new MamaException(METHOD_NAME+"Not yet supported");
    }
    
    public void setSubscriptionType (MamaSubscriptionType type)
    {
        mySubType = type;
        _setSubscriptionType (mySubType.getValue());
    }
    public MamaSubscriptionType getSubscriptionType ()
    {
        return mySubType;
    }

    /* Return the <code>MamaTransport</code> for this subscription. */
    public MamaTransport getTransport ()
    {
        return mySource.getTransport ();
    }

    /* ************************************************** */
    /* Public Native Functions, all entries here must be fully
     * documented. */
    /* ************************************************** */

    /**
     * Activate a subscription that has been set up by calling MamaSubscription.setup.
     * Subscription creation actually occurs on the throttle queue.
     * An individual subscription cannot be assumed to be fully created until
     * its onCreate() callback has been successfully invoked.
     * The subscription rate can be governed via the
     * MamaTransport.setOutboundThrottle () function.
     * Any subscription properties should be set prior to calling this function.
     */
    public native void activate();

    /**
     * Deactivate a subscription.  The subscription can be reactivated if
     * desired using MamaSubscription.activate().
     * Note that the subscription will not be fully deactivated until the
     * onDestroy callback is received.
     */
    public native void deactivate ();

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
     * Get whether we have received an Initial.
     *
     * @return True if an initial has been received.
     */
    public native boolean getReceivedInitial ();

    /**
     * Return the source for this subscription.
     *
     * @return The source string.
     */
    public native String getSubscSource();

    /**
     * Return the symbol for this subscription.
     *
     * @return The symbol string.
     */
    public native String getSymbol();

    /**
     * To return the subscription timeout.
     *
     * @return The timeout in seconds.
     */
    public native double getTimeout ();

    /**
     * Returns whether the subscription is valid, note that this function has
     * been deprecated, use mamaSubscription_getState instead.
     *
     * @return whether the subscription is valid.
     */
    public native boolean isActive();

    /**
     * Returns whether the subscription is valid, note that this function has
     * been deprecated, use mamaSubscription_getState instead.
     *
     * @return whether the subscription is valid.
     */
    public native boolean isValid();

    /**
     * Set whether subscriptoin requires initial value.
     *
     * @param value True if the subscription needs an initial.
     */
    public native void setRequiresInitial (boolean value);

    /**
     * Set the timeout for this subscription.
     *
     * @param timeout The timeout in seconds.
     */
    public native void setTimeout (double timeout);

    /* This are still to be commented. */
    public native Object getItemClosure();
    public native int getPreInitialCacheSize ();
    public native boolean getRecoverGaps();
    public native boolean getRequiresInitital();
    public native int getRetries ();
    public native short getServiceLevel ();
    public native long getServiceLevelOptions ();
    public native void setPreInitialCacheSize (int size);
    public native void setItemClosure(Object closure);
    public native void setRecoverGaps( boolean recover );

     /*Set the service level and options for the subscription*/
    public native void setServiceLevel (short level, long options);

    /* Set the number of retries when requesting initial values and recaps. */
    public native void setRetries (int retries);

    /* ************************************************** */
    /* Private Native Functions. */
    /* ************************************************** */       
    
    private native void allocateSubscription();
    private native MamaDictionary createNativeDictionarySubscription(
        MamaDictionaryCallback  callback,
        MamaQueue               queue,
        MamaSource              source,
        double                  timeout,
        int                     retries);
    private native void createNativeSubscription(
        final MamaSubscriptionCallback  callback,
        final MamaQueue                 queue,
        final MamaSource                source,
        final String                    symbol,
        final Object                    closure);
    private native int getSubscriptionState();
    private static native void initIDs();
    private native void _setSubscriptionType (short value);
    private native void setupNativeSubscription(
        final MamaSubscriptionCallback  callback,
        final MamaQueue                 queue,
        final MamaSource                source,
        final String                    symbol,
        final Object                    closure);

}/*End class*/
