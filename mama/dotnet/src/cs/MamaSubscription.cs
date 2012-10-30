/* $Id: MamaSubscription.cs,v 1.20.8.7 2012/08/24 16:12:01 clintonmcdowell Exp $
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

using System;
using System.Runtime.InteropServices;

namespace Wombat
{
    /* ************************************************************** */
    #region mamaServiceLevel Enumeration

    /// <summary>
    /// An enumeration that represents the service
    /// level (a) requested by a subscribing application, or (b) provided
    /// by the publisher.  
    /// </summary>
    /// <remarks>
    /// In the future, the service level provided to the
    /// application might change dynamically, depending upon infrastructure
    /// capacity.  So, even though the requested service level is "real
    /// time", if service deteriorates for any reason, the subscription may
    /// automatically switch to a conflated level (after notifying the
    /// application of the switch).
    /// </remarks>
    public enum mamaServiceLevel
    {
        /// <summary>
        /// Real time updates
        /// </summary>
        MAMA_SERVICE_LEVEL_REAL_TIME = 0,
        
        /// <summary>
        /// Single snapshot image only
        /// </summary>
        MAMA_SERVICE_LEVEL_SNAPSHOT = 1,
        
        /// <summary>
        /// Repeating snapshot image. When creating subscriptions, the
        /// serviceLevelOpt parameter should be set to the number of
        /// milliseconds representing the interval between snapshots.  The
        /// first snapshot will be obtained within some random period
        /// between zero and the interval.  Only one snapshot request will
        /// be allowed to be outstanding at a time.
        /// </summary>
        MAMA_SERVICE_LEVEL_REPEATING_SNAPSHOT = 2,
        
        /// <summary>
        /// Conflated updates (future)
        /// </summary>
        MAMA_SERVICE_LEVEL_CONFLATED = 5,

        /// <summary>
        /// Unknown level
        /// </summary>
        MAMA_SERVICE_LEVEL_UNKNOWN = 99
    }

    #endregion

    /* ************************************************************** */
    #region mamaSubscriptionType Enumerations

    /// <summary>
    /// mamaSubscriptionType
    /// </summary>
    public enum mamaSubscriptionType
    {
        MAMA_SUBSC_TYPE_NORMAL       = 0,
        MAMA_SUBSC_TYPE_GROUP        = 1,
        MAMA_SUBSC_TYPE_BOOK         = 2,
        /// <summary>
        /// No Market data
        /// </summary>
        MAMA_SUBSC_TYPE_BASIC        = 3,
        MAMA_SUBSC_TYPE_DICTIONARY   = 4,
        MAMA_SUBSC_TYPE_SYMBOL_LIST  = 5
    }

    #endregion

    /* ************************************************************** */
    #region MamaSubscription Class

    /// <summary>
	/// Subscription class, derives from a basic subscription.
	/// </summary>
	public class MamaSubscription : MamaBasicSubscription
    {
        /* ************************************************************** */
        #region Class Member Definition

        /* ************************************************************** */
        #region Native Methods

        private struct SubscriptionNativeMethods
        {
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern int mamaSubscription_activate(IntPtr subscription);

            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern int mamaSubscription_deactivate(IntPtr subscription);

            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern int mamaSubscription_getPreIntitialCacheSize(IntPtr subscription, ref int cacheSize);

            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern int mamaSubscription_getReceivedInitial(IntPtr nativeHandle, ref int receivedInitial);

            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern int mamaSubscription_getSource(IntPtr subscription, ref IntPtr symbol);

            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern int mamaSubscription_setPreIntitialCacheSize(IntPtr subscription, int cacheSize);

            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern int mamaSubscription_setRecoverGaps(IntPtr nativeHandle, int recoverGaps);

            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern int mamaSubscription_setRequiresInitial(IntPtr nativeHandle, int requiresInitial);

            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern int mamaSubscription_setRetries(IntPtr nativeHandle, int retries);

            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern int mamaSubscription_setServiceLevel(IntPtr nativeHandle, int serviceLevel, int serviceLevelOpt);

            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern int mamaSubscription_setSubscriptionType(IntPtr nativeHandle, int type);

            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern int mamaSubscription_setTimeout(IntPtr nativeHandle, double timeout);

            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern int mamaSubscription_setup(IntPtr subscription, IntPtr queue, ref MamaBasicSubscription.NativeMethods.SubscriptionCallbacks callbacks, IntPtr source, [MarshalAs(UnmanagedType.LPStr)]string symbol, IntPtr closure);

            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern int mamaSubscription_setup2(IntPtr subscription, IntPtr transport, IntPtr queue, ref MamaBasicSubscription.NativeMethods.SubscriptionCallbacks callbacks, [MarshalAs(UnmanagedType.LPStr)]string sourceName, [MarshalAs(UnmanagedType.LPStr)]string symbol, IntPtr closure);
        }

        #endregion

        /* ************************************************************** */
        #region Private Classes

        /* ************************************************************** */
        #region MamaBasicCallbackAdapter Class

        /// <summary>
        /// This adapter provides translation between the MamaBasicSubscriptionCallback
        /// and MamaSubscriptionCallback types.
        /// </summary>
        private class MamaBasicCallbackAdapter : MamaBasicSubscriptionCallback
        {
            /* ************************************************************** */
            #region Private Member Variables

            /// <summary>
            /// The subscription callback.
            /// </summary>
            private MamaSubscriptionCallback mCallback;

            #endregion

            /* ************************************************************** */
            #region Construction and Finalization

            /// <summary>
            /// Constructor initialises all member variables.
            /// </summary>
            /// <param name="callback">
            /// The subscription callback.
            /// </param>
            internal MamaBasicCallbackAdapter(MamaSubscriptionCallback callback)
            {
                mCallback = callback;
            }

            #endregion

            /* ************************************************************** */
            #region MamaBasicSubscriptionCallback Implementation

            /// <summary>
            /// Called when subscription creation is complete, and before
            /// the onMsg() callback is invoked
            /// </summary>
            /// <param name="subscription">
            /// The subscription that has been created.
            /// </param>
            void MamaBasicSubscriptionCallback.onCreate(MamaBasicSubscription subscription)
            {
                if (mCallback != null)
                {
                    mCallback.onCreate((MamaSubscription)subscription);
                }
            }

            /// <summary>
            /// Invoked whenever the basic subscription has been destroyed. The client can have confidence that 
            /// no further messages will be placed on the queue for this subscription.
            /// </summary>
            /// <param name="subscription">
            /// The basic subscription.
            /// </param>
            /// <param name="closure">
            /// The closure provided to the createBasic function.
            /// </param>
            void MamaBasicSubscriptionCallback.onDestroy(MamaBasicSubscription subscription, object closure)
            {
                if (mCallback != null)
                {
                    mCallback.onDestroy((MamaSubscription)subscription);
                }
            }

            /// <summary>
            /// Called when an error has occurred with the subscription
            /// </summary>
            /// <param name="subscription">
            /// The subscription where the error has been detected.
            /// </param>
            /// <param name="status">
            /// The status code associated with the error.
            /// </param>
            /// <param name="subject">
            /// The subject for entitlement errors.
            /// </param>
            void MamaBasicSubscriptionCallback.onError(MamaBasicSubscription subscription, MamaStatus.mamaStatus status, string subject)
            {
                if (mCallback != null)
                {
                    mCallback.onError((MamaSubscription)subscription, status, subject);
                }
            }

            /// <summary>
            /// Invoked when a message arrives
            /// </summary>
            /// <param name="subscription">
            /// The subscription where the message has arrived.
            /// </param>
            /// <param name="message">
            /// The message.
            /// </param>
            void MamaBasicSubscriptionCallback.onMsg(MamaBasicSubscription subscription, MamaMsg message)
            {
                if (mCallback != null)
                {
                    mCallback.onMsg((MamaSubscription)subscription, message);
                }
            }

            #endregion
        }

        #endregion

        /* ************************************************************** */
        #region MamaSubscriptionImpl Class

        /// <summary>
        /// This impl will perform the work of invoking user supplied callbacks, it is
        /// held to ensure that the user can destroy and recreate the parent subscription during
        /// the callbacks.
        /// Note that this class derives from the MamaBasicSubscriptionImpl so that the static
        /// callback functions in the MamaBasicSubscription class can be re-used. However as
        /// the MamaSubscriptionCallback interface is a separate type, (it cannot be derived from
        /// MamaBasicSubscriptionCallback as the type of the subscription passed to the callback
        /// functions would be incorrect), overloads must be provided for all the Invoke methods.
        /// </summary>
        private class MamaSubscriptionImpl : MamaBasicSubscriptionImpl
        {
            /* ************************************************************** */
            #region Private Member Variables

            /// <summary>
            /// The callback interface.
            /// </summary>
            private MamaSubscriptionCallback mCallback;

            /// <summary>
            /// The closure provided by the user.
            /// </summary>
            private object mClosure;

            /// <summary>
            /// Used to prevent a new message being created each time the onMsg
            /// is received.
            /// </summary>
            private MamaMsg mReusableMsg;

            /// <summary>
            /// The actual subscription object that will be provided in the callbacks.
            /// </summary>
            private MamaSubscription mSubscription;

            #endregion

            /* ************************************************************** */
            #region Construction and Finalization

            /// <summary>
            /// Constructor initialises all member variables.
            /// Note that the base class default constructor is used as we don't want to use any
            /// of the base class implementation.
            /// </summary>
            /// <param name="callback">
            /// The user callback implementation, can be an instance of the MamaSubscriptionCallbackEx
            /// interface to receive destroy notifications.
            /// </param>
            /// <param name="closure">
            /// The closure supplied to the MamaSubscription.create function.
            /// </param>
            /// <param name="subscription">
            /// The actual C# subscription object.
            /// </param>
            internal MamaSubscriptionImpl(MamaSubscriptionCallback callback, object closure, MamaSubscription subscription)                                    
                : base()
            {
                // Save arguments in member variables
                mCallback      = callback;
                mClosure       = closure;
                mSubscription  = subscription;
            }

            #endregion

            /* ************************************************************** */
            #region Internal Operations
           
            /// <summary>
            /// This function creates a new impl and returns an IntPtr that can then be passed to
            /// the native layer.
            /// </summary>
            /// <param name="callback">
            /// The user callback implementation, can be an instance of the MamaSubscriptionCallbackEx
            /// interface to receive destroy notifications.
            /// </param>
            /// <param name="closure">
            /// The closure supplied to the MamaSubscription.create function.
            /// </param>
            /// <param name="subscription">
            /// The actual C# subscription object.
            /// </param>
            /// <returns>
            /// The IntPtr that can then be used for the closure.
            /// </returns>
            internal static IntPtr Create(MamaSubscriptionCallback callback, object closure, MamaSubscription subscription)
            {
                // Allocate a new impl
                MamaSubscriptionImpl impl = new MamaSubscriptionImpl(callback, closure, subscription);

                // Create a GC handle
                GCHandle handle = GCHandle.Alloc(impl);

                // Return the native pointer
                return (IntPtr)handle;
            }

            /// <summary>
            /// This function will invoke the create callback on the user supplied callback implementation.
            /// </summary>
            internal override void InvokeCreate()
            {
                if (null != mCallback)
                {
                    // Invoke the callback
                    mCallback.onCreate(mSubscription);
                }
            }

            /// <summary>
            /// This function will invoke the destroy callback on the user supplied callback implementation.            
            /// </summary>
            internal override void InvokeDestroy()
            {
                if (null != mCallback)
                {
                    // Invoke the onDestroy
                    mCallback.onDestroy(mSubscription);
                }
            }

            /// <summary>
            /// This function will invoke the error callback on the user supplied callback implementation.
            /// </summary>
            /// <param name="nativeStatus">
            /// The status code for the error that has occured.
            /// </param>
            /// <param name="subject">
            /// The subject for entitlement errors.
            /// </param>
            internal override void InvokeError(int nativeStatus, string subject)
            {
                // Only invoke the callback if it is supplied
                if (null != mCallback)
                {
                    // Create a managed status value fo the native value passed in
                    MamaStatus.mamaStatus status = (MamaStatus.mamaStatus)nativeStatus;

                    // Invoke the callback
                    mCallback.onError(mSubscription, status, subject);
                }
            }

            /// <summary>
            /// This function will invoke the gap callback on the user supplied callback implementation.
            /// </summary>
            internal void InvokeGap()
            {
                if (null != mCallback)
                {
                    // Invoke the callback
                    mCallback.onGap(mSubscription);
                }
            }

            /// <summary>
            /// This function will invoke the message callback on the user supplied callback implementation.
            /// </summary>
            /// <param name="nativeMsg">
            /// The native MAMA message.
            /// </param>
            internal override void InvokeMessage(IntPtr nativeMsg)
            {
                if (null != mCallback)
                {
                    // If the re-usable message hasn't been created yet then do so now
                    if (mReusableMsg == null)
                    {
                        mReusableMsg = new MamaMsg(nativeMsg);
                    }

                    else
                    {
                        mReusableMsg.setNativeHandle(nativeMsg);
                    }
                    
                    // Invoke the callback
                    mCallback.onMsg(mSubscription, mReusableMsg);
                }
            }

            /// <summary>
            /// This function will invoke the quality callback on the user supplied callback implementation.
            /// </summary>
            internal void InvokeQuality(int nativeQuality, string symbol)
            {
                if (null != mCallback)
                {
                    // Invoke the callback
                    mCallback.onQuality(Subscription, (mamaQuality)nativeQuality, symbol);
                }
            }

            /// <summary>
            /// This function will invoke the recap request callback on the user supplied callback implementation.
            /// </summary>
            internal void InvokeRecapRequest()
            {
                if (null != mCallback)
                {
                    // Invoke the callback
                    mCallback.onRecapRequest(Subscription);
                }
            }

            /// <summary>
            /// Read only property returns the parent subscription.
            /// </summary>
            internal MamaSubscription Subscription
            {
                get
                {
                    return mSubscription;
                }
            }

            #endregion
        }

        #endregion

        #endregion

        #endregion

        /* ************************************************************** */
        #region Construction and Finalization

        /// <summary>
        /// Static constructor initialises all of the delegates.
        /// </summary>
        static MamaSubscription()
        {
            /* Complete the additional delegates in the base class structure, note that the market data subscription
             * has different functionality in the destroy callback than the basic subscription.
             */
            MamaBasicSubscription.mCallbackDelegates.mDestroy         = new MamaBasicSubscription.OnSubscriptionDestroyDelegate(MamaSubscription.onDestroy);
            MamaBasicSubscription.mCallbackDelegates.mGap             = new MamaBasicSubscription.OnSubscriptionGapDelegate(MamaSubscription.onGap);
            MamaBasicSubscription.mCallbackDelegates.mQuality         = new MamaBasicSubscription.OnSubscriptionQualityDelegate(MamaSubscription.OnQuality);
            MamaBasicSubscription.mCallbackDelegates.mRecapRequest    = new MamaBasicSubscription.OnSubscriptionRecapRequestDelegate(MamaSubscription.onRecapRequest);            
        }

        /// <summary>
		/// Allocate memory for a new subscription. The subscription is not actually created 
		/// until a call to one of the create() functions is made.
		/// Memory must be freed using the deallocate()
		/// </summary>
		public MamaSubscription() : base()
		{			
		}

		internal MamaSubscription(IntPtr nativeHandle) : base(nativeHandle)
		{
		}

        #endregion

        /* ************************************************************** */
        #region Private Static Functions

        /// <summary>
        /// This event handler is called by the native layer whenever the subscription has been fully destroyed.
        /// Note that clean up will only be performed if the subscription is about to be destroyed or deallocated,
        /// this can be determined by using the getState function.
        /// </summary>
        /// <param name="subscription">
        /// The native subscription.
        /// </param>
        /// <param name="closure">
        /// The closure passed down to the native layer.
        /// </param>
        private static void onDestroy(IntPtr subscription, IntPtr closure)
        {
            // Obtain the handle from the closure
            GCHandle handle = (GCHandle)closure;

            /* If the target is a basic impl then createBasic has been invoked on this class instead of the base class,
             * however as the MamaSubscriptionImpl is a derived class from MamaBasicSubscriptionImpl we must check for
             * the derived class first.
             */
            if (handle.Target is MamaSubscriptionImpl)
            {
                // Extract the impl from the handle
                MamaSubscriptionImpl impl = (MamaSubscriptionImpl)handle.Target;

                /* Get the state before the destroy is called, (in case the user recreates the subscription on
                 * the callback).
                 */
                mamaSubscriptionState state = impl.Subscription.State;

                // Use the impl to invoke the destroy callback, (if this has been supplied)
                impl.InvokeDestroy();

                // If we are destroying rather than deactivating then delete the impl
                if ((mamaSubscriptionState.MAMA_SUBSCRIPTION_DESTROYED == state) || (mamaSubscriptionState.MAMA_SUBSCRIPTION_DEALLOCATING == state))
                {
                    /* The subscription has now been destroyed or deleted and the impl is no longer required, free the handle to 
                     * allow the garbage collector to clean it up.
                     */
                    handle.Free();
                }
            }

            else if (handle.Target is MamaBasicSubscriptionImpl)
            {
                // Extract the impl from the handle
                MamaBasicSubscriptionImpl impl = (MamaBasicSubscriptionImpl)handle.Target;

                // Use the impl to invoke the destroy callback, (if this has been supplied)
                impl.InvokeDestroy();

                /* The timer has now been destroyed and the impl is no longer required, free the handle to 
                 * allow the garbage collector to clean it up.
                 */
                handle.Free();
            }

            else
            {
                // Otherwise something has gone wrong
                throw new InvalidOperationException();
            }
        }

        /// <summary>
        /// This event handler is called by the native layer whenever the a gap occurs in the message
        /// sequence.
        /// </summary>
        /// <param name="nativeHandle">
        /// The native handle for the subscription.
        /// </param>
        /// <param name="closure">
        /// Native reference to the closure.
        /// </param>
        private static void onGap(IntPtr nativeHandle, IntPtr closure)
        {
            // Obtain the handle from the closure
            GCHandle handle = (GCHandle)closure;

            // Extract the impl from the handle
            MamaSubscriptionImpl impl = (MamaSubscriptionImpl)handle.Target;

            // Use the impl to invoke the callback
            impl.InvokeGap();
        }

        /// <summary>
        /// This event handler is called by the native layer whenever the quality of the data in the subscription
        /// changes.
        /// </summary>
        /// <param name="nativeHandle">
        /// The native handle to the subscription.
        /// </param>
        /// <param name="quality">
        /// The current quality.
        /// </param>
        /// <param name="symbol">
        /// The symbol being subscribed to.
        /// </param>
        /// <param name="cause">
        /// The cause of the quality change.
        /// </param>
        /// <param name="platformInfo">
        /// Platform specific information.
        /// </param>
        /// <param name="closure">
        /// Native reference to the closure.
        /// </param>
        private static void OnQuality(IntPtr nativeHandle, int quality, string symbol, short cause, string platformInfo, IntPtr closure)
        {
            // Obtain the handle from the closure
            GCHandle handle = (GCHandle)closure;

            // Extract the impl from the handle
            MamaSubscriptionImpl impl = (MamaSubscriptionImpl)handle.Target;

            // Use the impl to invoke the callback
            impl.InvokeQuality(quality, symbol);
        }

        /// <summary>
        /// This event handler is called by the native layer whenever the a recap is requested by the
        /// native layer.
        /// </summary>
        /// <param name="nativeHandle">
        /// The native handle for the subscription.
        /// </param>
        /// <param name="closure">
        /// Native reference to the closure.
        /// </param>
        private static void onRecapRequest(IntPtr nativeHandle, IntPtr closure)
        {
            // Obtain the handle from the closure
            GCHandle handle = (GCHandle)closure;

            // Extract the impl from the handle
            MamaSubscriptionImpl impl = (MamaSubscriptionImpl)handle.Target;

            // Use the impl to invoke the callback
            impl.InvokeRecapRequest();
        }

        #endregion

        /* ************************************************************** */
        #region Public Functions


        /// <summary>
        /// Activate a subscription that has been set up by calling the setup function.
        /// Subscription creation actually occurs on the throttle queue. An individual
        /// subscription cannot be assumed to be fully created until its onCreate callback
        /// has been successfully invoked. The subscription rate can be governed via the
        /// MamaTransport.setOutboundThrottle function.
        /// Any subscription properties should be set prior to calling this function.
        /// </summary>        
        public void activate()
        {
            // Ensure that the subscription has been created
            EnsurePeerCreated();

            // Call the native layer
            CheckResultCode(SubscriptionNativeMethods.mamaSubscription_activate(NativeHandle));        
        }

        /// <summary>
        /// Set up and activate a subscriber using the throttle queue. This
        /// method is equivalent to calling setup() followed by activate().
        /// </summary>
        /// <param name="queue">
        /// The mama queue.
        /// </param>
        /// <param name="callback">
        /// The callback.
        /// </param>
        /// <param name="source">
        /// The MamaSource identifying the publisher for this symbol.
        /// </param>
        /// <param name="symbol">
        /// The symbol for the listener.
        /// </param>
        public void create(MamaQueue queue, MamaSubscriptionCallback callback, MamaSource source, string symbol)
        {
            // Call the overload with a null closure
            create(queue, callback, source, symbol, null);
        }

        /// <summary>
		/// Set up and activate a subscriber using the throttle queue. This
		/// method is equivalent to calling setup() followed by activate().
		/// </summary>
		/// <param name="queue">
        /// The mama queue.
        /// </param>
		/// <param name="callback">
        /// The callback.
        /// </param>
		/// <param name="source">
        /// The MamaSource identifying the publisher for this symbol.
        /// </param>
		/// <param name="symbol">
        /// The symbol for the listener.
        /// </param>
		/// <param name="closure">
        /// The caller supplied closure.
        /// </param>
		public void create(MamaQueue queue, MamaSubscriptionCallback callback, MamaSource source, string symbol, object closure)
		{
			// This is equivalent to calling setup then activate
            setup(queue, callback, source, symbol, closure);
            activate();
		}

        /// <summary>
        /// This function will create the basic subscription without marketdata semantics.
        /// To pass a closure use the overload.
        /// This overload provides access to the basic subscription with the MamaSubscriptionCallback rather than the 
        /// MamaBasicSubscriptionCallback interface.
        /// Instantiating the base class directly will provide better performance.
        /// </summary>
        /// <param name="transport">
        /// The MamaTransport.
        /// </param>
        /// <param name="queue">
        /// The MamaQueue.
        /// </param>
        /// <param name="callback">
        /// Provide callback function implementations to be informed of events on the subscription.
        /// </param>
        /// <param name="symbol">
        /// The symbol to subscribe to.
        /// </param>
        public void createBasic(MamaTransport transport, MamaQueue queue, MamaSubscriptionCallback callback, string symbol)
        {
            // Call the overload
            createBasic(transport, queue, callback, symbol, null);
        }

        /// <summary>
        /// This function will create the basic subscription without marketdata semantics.
        /// This overload provides access to the basic subscription with the MamaSubscriptionCallback rather than the 
        /// MamaBasicSubscriptionCallback interface.
        /// Instantiating the base class directly will provide better performance.
        /// </summary>
        /// <param name="transport">
        /// The MamaTransport.
        /// </param>
        /// <param name="queue">
        /// The MamaQueue.
        /// </param>
        /// <param name="callback">
        /// Provide callback function implementations to be informed of events on the subscription.
        /// </param>
        /// <param name="symbol">
        /// The symbol to subscribe to.
        /// </param>
        /// <param name="closure">
        /// The closure that will be passed back with the callback functions.
        /// </param>
        public void createBasic(MamaTransport transport, MamaQueue queue, MamaSubscriptionCallback callback, string symbol, object closure)
        {
            // Call the base class using the adapter to convert between the callback types
            base.createBasic(transport, queue, new MamaBasicCallbackAdapter(callback), symbol, closure);
        }

        /// <summary>
        /// Deactivate an active subscription.
        /// The onDestroy callback will be invoked whenever the subscription has been fully deactivated.
        /// </summary>        
        public void deactivate()
        {
            // Verify that the native subscription has been allocated
            EnsurePeerCreated();

            // Call the native layer
            CheckResultCode(SubscriptionNativeMethods.mamaSubscription_deactivate(NativeHandle));
        }

        /// <summary>
        /// Return the initial value cache size.
        /// </summary>
        /// <returns>
        /// The cache size.
        /// </returns>
        public int getPreInitialCacheSize()
        {
            // Verify that the native subscription has been allocated
            EnsurePeerCreated();
            int cacheSize = 0;

            // Call the native layer
            CheckResultCode(SubscriptionNativeMethods.mamaSubscription_getPreIntitialCacheSize(NativeHandle, ref cacheSize));
            
            return cacheSize;
        }

        /// <summary>
        /// Whether an Initial has been received for the specified subscription.
        /// Returns true if initial has been received.
        /// </summary>
        /// <returns>
        /// True if an initial has been received for this subscription.
        /// </returns>
        public bool getReceivedInitial()
        {
            // Returns
            bool ret = false;

            // Verify that the native subscription has been allocated
            EnsurePeerCreated();

            // Call the native layer
            int receivedInitial = 0;
            CheckResultCode(SubscriptionNativeMethods.mamaSubscription_getReceivedInitial(NativeHandle, ref receivedInitial));
            
            // Convert the integer returned from the native call into a boolean            
            if (receivedInitial == 1)
            {
                ret = true;
            }

            return ret;
        }

        /// <summary>
        /// Set the number of messages to cache for each symbol before the initial
        /// value arrives. This allows the subscription to recover when the initial
        /// value arrives late (after a subsequent trade or quote already arrived).     
        /// For group subscription, a separate cache is used for each group member.
        /// The default is 10.
        /// </summary>
        /// <param name="cacheSize">
        /// The cache size to set.
        /// </param>
        public void setPreInitialCacheSize(int cacheSize)
        {
            // Verify that the native subscription has been allocated
            EnsurePeerCreated();

            // Call the native layer to set the cache size
            CheckResultCode(SubscriptionNativeMethods.mamaSubscription_setPreIntitialCacheSize(NativeHandle, cacheSize));            
        }

        /// <summary>
        /// If this flag is set then the subscription will attempt to recover from a
        /// sequence number gap.
        /// </summary>
        /// <param name="recover">
        /// True to attempt recovery.
        /// </param>
        public void setRecoverGaps(bool recover)
        {
            // Verify that the native subscription has been allocated
            EnsurePeerCreated();

            // Convert the boolean argument into an integer for the native call
            int recoverGaps = recover ? 1 : 0;

            // Call the native layer to set the flag
            CheckResultCode(SubscriptionNativeMethods.mamaSubscription_setRecoverGaps(NativeHandle, recoverGaps));            
        }

        /// <summary>
        /// Whether an initial value is required for the specified subscription.
        /// This only applies to market data subscriptions and not to basic subscriptions. 
        /// Default value of true indicating that initial values are required.
        /// </summary>
        /// <param name="requiresInitial">
        /// True if the subscription requires an initial.
        /// </param>
        public void setRequiresInitial(bool requiresInitial)
        {
            // Verify that the native subscription has been allocated
            EnsurePeerCreated();

            // Convert the boolean to an integer value for the native call
            int requiresInit = requiresInitial ? 1 : 0;

            // Call the native layer
            CheckResultCode(SubscriptionNativeMethods.mamaSubscription_setRequiresInitial(NativeHandle, requiresInit));            
        }

        /// <summary>
        /// Set the number of retries when requesting recaps.
        /// </summary>
        /// <param name="retries">
        /// The number of retries.
        /// </param>
        public void setRetries(int retries)
        {
            // Verify that the native subscription has been allocated
            EnsurePeerCreated();

            // Call the native layer to set the number of retries
            CheckResultCode(SubscriptionNativeMethods.mamaSubscription_setRetries(NativeHandle, retries));            
        }

        /// <summary>
        /// Set the service level without any other options.
        /// This method must be invoked before create().
        /// </summary>
        /// <param name="svcLevel">
        /// The service level to set.
        /// </param>
        public void setServiceLevel(mamaServiceLevel svcLevel)
        {
            // Call the overload
            setServiceLevel(svcLevel, 0);
        }

        /// <summary>
        /// Set the service level and an passes an optional service level parameter. This method must be invoked before create().
        /// </summary>
        /// <param name="svcLevel">
        /// The service level to set.
        /// </param>
        /// <param name="serviceLevelOpt">
        /// The service level options.
        /// </param>
        public void setServiceLevel(mamaServiceLevel svcLevel, int serviceLevelOpt)
        {
            // Verify that the native subscription has been allocated
            EnsurePeerCreated();

            // Call the native layer to set the service level
            CheckResultCode(SubscriptionNativeMethods.mamaSubscription_setServiceLevel(NativeHandle, (int)svcLevel, serviceLevelOpt));
        }

        /// <summary>
        /// Set the subscription type for the subscription being created.
        /// If not called the subscription type defaults to MAMA_SUBSC_TYPE_NORMAL.
        /// See mamaSubscriptionType enum for valid values.
        /// </summary>
        /// <param name="type">
        /// The type to set.
        /// </param>
        public void setSubscriptionType(mamaSubscriptionType type)
        {
            // Verify that the native subscription has been allocated
            EnsurePeerCreated();

            // Call the native layer to set the subscription type
            CheckResultCode(SubscriptionNativeMethods.mamaSubscription_setSubscriptionType(NativeHandle, (int)type));            
        }

        /// <summary>
        /// Set the timeout for this subscription. The timeout is used for requesting recaps.
        /// </summary>
        /// <param name="timeout">
        /// The timeout value in seconds.
        /// </param>
        public void setTimeout(double timeout)
        {
            // Verify that the subscription has been allocated
            EnsurePeerCreated();

            // Call the native layer to set the timeout
            CheckResultCode(SubscriptionNativeMethods.mamaSubscription_setTimeout(NativeHandle, timeout));            
        }

        /// <summary>
        /// Set the parameters for a subscription that may be actually activated later.  
        /// Activate the subscription using MamaSubscription.activate().
        /// </summary>
        /// <param name="transport">
        /// The MAMA transport.
        /// </param>
        /// <param name="queue">
        /// The MAMA queue.
        /// </param>
        /// <param name="callback">
        /// The callback interface.
        /// </param>
        /// <param name="sourceName">
        /// The name of the source identifying the publisher for this symbol.
        /// </param>
        /// <param name="symbol">
        /// The symbol for the listener.
        /// </param>
        /// <param name="closure">
        /// The caller supplied closure.
        /// </param>
        public void setup(MamaQueue queue, MamaTransport transport, MamaSubscriptionCallback callback, string sourceName, string symbol, object closure)
        {
            // Verify that the subscription has been created
            EnsurePeerCreated();

            // Save arguments in member variables
            base.mClosure      = closure;
            base.mQueue        = queue;
            base.mTransport    = transport;

            // Create the impl
            IntPtr impl = MamaSubscriptionImpl.Create(callback, closure, this);

            // Call into the native layer to setup the subscription
            CheckResultCode(SubscriptionNativeMethods.mamaSubscription_setup2(
                NativeHandle,
                transport.NativeHandle,
                queue.NativeHandle,
                ref MamaBasicSubscription.mCallbackDelegates,
                sourceName,
                symbol,
                impl));            
        }
        
		/// <summary>
        /// Set the parameters for a subscription that may be actually activated later.  
        /// Activate the subscription using MamaSubscription.activate().
        /// </summary>
        /// <param name="queue">
        /// The mama queue.
        /// </param>
        /// <param name="callback">
        /// The callback.
        /// </param>
        /// <param name="source">
        /// The MamaSource identifying the publisher for this symbol.
        /// </param>
        /// <param name="symbol">
        /// The symbol for the listener.
        /// </param>
        /// <param name="closure">
        /// The caller supplied closure.
        /// </param>
        public void setup(MamaQueue queue, MamaSubscriptionCallback callback, MamaSource source, string symbol, object closure)
        {
            // Verify that the subscription has been created
            EnsurePeerCreated();

            // Save arguments in member variables
            base.mClosure      = closure;
            base.mQueue        = queue;
            base.mTransport    = source.transport;

            // Create the impl
            IntPtr impl = MamaSubscriptionImpl.Create(callback, closure, this);

            // Call into the native layer to setup the subscription
            CheckResultCode(SubscriptionNativeMethods.mamaSubscription_setup(
                NativeHandle,
                queue.NativeHandle,
                ref MamaBasicSubscription.mCallbackDelegates,
                source.NativeHandle,
                symbol,
                impl));
        }

        /// <summary>
        /// Set the parameters for a subscription that may be actually activated later.  
        /// Activate the subscription using MamaSubscription.activate().
        /// This overload passes a null value for the closure.
        /// </summary>
        /// <param name="queue">
        /// The mama queue.
        /// </param>
        /// <param name="callback">
        /// The callback.
        /// </param>
        /// <param name="source">
        /// The MamaSource identifying the publisher for this symbol.</param>
        /// <param name="symbol">
        /// The symbol for the listener.
        /// </param>        
        public void setup(MamaQueue queue, MamaSubscriptionCallback callback, MamaSource source, string symbol)
        {
            // Call the overload passing null for the closure
            setup(queue, callback, source, symbol, null);
        }

        /// <summary>
        /// Read only property returns the subscription source.
        /// </summary>
        public string subscSource
        {
            get
            {
                // Get the source from the native layer
                IntPtr ret = IntPtr.Zero;
                CheckResultCode(SubscriptionNativeMethods.mamaSubscription_getSource(NativeHandle, ref ret));

                // Convert to an ansi string                
                return Marshal.PtrToStringAnsi(ret);
            }
        }
        #endregion
    }

    #endregion
}
