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

 
using System;
using System.Runtime.InteropServices;

namespace Wombat
{
    /* ************************************************************** */
    #region MamaBasicSubscription Class

    /// <summary>
    /// The basic subscription supports publish / subscribe.
    /// This class can be disposed or the deallocate function called to reduce time during
    /// finalization.
    /// Note that the deallocate function will attempt to destroy the subscription if this
    /// has not already been done whereas dispose will only de-allocate the memory.
    /// </summary>
    public class MamaBasicSubscription : MamaWrapper
    {
        /* ************************************************************** */
        #region Class Member Definition

        /* ************************************************************** */
        #region Native Method Prototypes

        protected struct NativeMethods
        {
            [StructLayout(LayoutKind.Sequential)]
            public struct SubscriptionCallbacks
            {
                public OnSubscriptionCreateDelegate mCreate;
                public OnSubscriptionErrorDelegate mError;
                public OnSubscriptionMessageDelegate mMessage;
                public OnSubscriptionQualityDelegate mQuality;
                public OnSubscriptionGapDelegate mGap;
                public OnSubscriptionRecapRequestDelegate mRecapRequest;
                public OnSubscriptionDestroyDelegate mDestroy;
                public IntPtr mReserved;
            }

            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern int mamaSubscription_allocate(ref IntPtr result);

            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern int mamaSubscription_createBasic(
                IntPtr nativeHandle,
                IntPtr transport,
                IntPtr queue,
                ref SubscriptionCallbacks callbacks,
                [MarshalAs(UnmanagedType.LPStr)] string symbol,
                IntPtr closure);

            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern int mamaSubscription_deallocate(IntPtr nativeHandle);

            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern int mamaSubscription_destroy(IntPtr nativeHandle);

            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern int mamaSubscription_destroyEx(IntPtr nativeHandle);

            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern int mamaSubscription_getState(IntPtr nativeHandle, ref int state);

            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern int mamaSubscription_getSubscSymbol(IntPtr subscription, ref IntPtr symbol);
        }

        #endregion

        /* ************************************************************** */
        #region Nested Classes

        /// <summary>
        /// This impl will perform the work of invoking user supplied callbacks, it is
        /// held to ensure that the user can destroy and recreate the parent subscription during
        /// the callbacks.
        /// </summary>
        protected class MamaBasicSubscriptionImpl
        {
            /* ************************************************************** */
            #region Private Member Variables

            /// <summary>
            /// The callback interface.
            /// </summary>
            private MamaBasicSubscriptionCallback mCallback;

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
            private MamaBasicSubscription mSubscription;

            #endregion

            /* ************************************************************** */
            #region Construction and Finalization

            /// <summary>
            /// Default constructor for internal use.
            /// </summary>
            protected MamaBasicSubscriptionImpl()
            {
            }

            /// <summary>
            /// Constructor initialises all member variables.
            /// </summary>
            /// <param name="callback">
            /// The user callback implementation.
            /// </param>
            /// <param name="closure">
            /// The closure supplied to the MamaBasicSubscription.createBasic function.
            /// </param>
            /// <param name="subscription">
            /// The actual C# timer object.
            /// </param>
            internal MamaBasicSubscriptionImpl(MamaBasicSubscriptionCallback callback, object closure, MamaBasicSubscription subscription)
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
            /// The user callback implementation
            /// </param>
            /// <param name="closure">
            /// The closure supplied to the MamaBasicSubscription.createBasic function.
            /// </param>
            /// <param name="subscription">
            /// The actual C# subscription object.
            /// </param>
            /// <returns>
            /// The IntPtr that can then be used for the closure.
            /// </returns>
            internal static IntPtr Create(MamaBasicSubscriptionCallback callback, object closure, MamaBasicSubscription subscription)
            {
                // Allocate a new impl
                MamaBasicSubscriptionImpl impl = new MamaBasicSubscriptionImpl(callback, closure, subscription);

                // Create a GC handle
                GCHandle handle = GCHandle.Alloc(impl);

                // Return the native pointer
                return (IntPtr)handle;
            }

            /// <summary>
            /// This function will invoke the create callback on the user supplied callback implementation.
            /// </summary>
            internal virtual void InvokeCreate()
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
            internal virtual void InvokeDestroy()
            {
                // Only the MamaBasicSubscriptionCallback class has the onDestroy function
                MamaBasicSubscriptionCallback callback = mCallback as MamaBasicSubscriptionCallback;
                if(null != callback)
                {
                    // Invoke the onDestroy
                    callback.onDestroy(mSubscription, mClosure);
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
            internal virtual void InvokeError(int nativeStatus, string subject)
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
            /// This function will invoke the message callback on the user supplied callback implementation.
            /// </summary>
            /// <param name="nativeMsg">
            /// The native MAMA message.
            /// </param>
            internal virtual void InvokeMessage(IntPtr nativeMsg)
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

            #endregion
        }

        #endregion

        /* ************************************************************** */
        #region Protected Delegates

        /// <summary>
        /// This delegate describes that native callback function that is invoked whenever the subscription
        /// has been created.
        /// </summary>
        /// <param name="nativeHandle">
        /// The native subscription.
        /// </param>
        /// <param name="closure">
        /// The native closure passed to the create function.
        /// </param>
        public delegate void OnSubscriptionCreateDelegate(IntPtr nativeHandle, IntPtr closure);

        /// <summary>
        /// This delegate describes the native function invoked whenever the subscription has been either
        /// destroyed or deactivated.
        /// </summary>
        /// <param name="nativeHandle">
        /// The native subscription.
        /// </param>
        /// <param name="closure">
        /// The native closure passed to the create function.
        /// </param>
        public delegate void OnSubscriptionDestroyDelegate(IntPtr nativeHandle, IntPtr closure);

        /// <summary>
        /// This delegate describes the native function invoked if an error occurs during prior to subscription
        /// creation or if the subscription receives a message for an un-entitled subject.
        /// If the status is MAMA_MSG_STATUS_NOT_ENTITTLED the subject parameter is the specific unentitled subject.
        /// If the subscription subject contains wildcards, the subscription may still receive messages for other
        /// entitled subjects. Note wildcard subscriptions are not supported on all platforms.
        /// </summary>
        /// <param name="nativeHandle">
        /// The native subscription.
        /// </param>
        /// <param name="status">
        /// The associated error code.
        /// </param>
        /// <param name="platformError">
        /// Platform specific error information.
        /// </param>
        /// <param name="subject">
        /// The subject for NOT_ENTITLED errors.
        /// </param>
        /// <param name="closure">
        /// The native closure passed to the create function.
        /// </param>
        public delegate void OnSubscriptionErrorDelegate(IntPtr nativeHandle, int status, IntPtr platformError, string subject, IntPtr closure);

        /// <summary>
        /// This delegate describes the native function invoked when a sequence number gap is detected. At this
        /// point the topic is considered stale and the subscription will not receive further messages until the
        /// feed handler satisfies a recap request.
        /// </summary>
        /// <param name="nativeHandle">
        /// The native subscription.
        /// </param>
        /// <param name="closure">
        /// The native closure passed to the create function.
        /// </param>
        public delegate void OnSubscriptionGapDelegate(IntPtr nativeHandle, IntPtr closure);

        /// <summary>
        /// This delegate describes the native function that is invoked whenever a message arrives.
        /// </summary>
        /// <param name="nativeHandle">
        /// The native subscription.
        /// </param>
        /// <param name="msg">
        /// The native mama message.
        /// </param>
        /// <param name="closure">
        /// The native closure passed to the create function.
        /// </param>
        /// <param name="itemClosure">
        /// The item closure for the subscription can be set with setItemClosure, note that setItemClosure is not provided
        /// in the C# implementation.
        /// </param>
        public delegate void OnSubscriptionMessageDelegate(IntPtr nativeHandle, IntPtr msg, IntPtr closure, IntPtr itemClosure);

        /// <summary>
        /// This delegate describes the native function invoked to indicate a data quality event.
        /// </summary>
        /// <param name="nativeHandle">
        /// The native subscription.
        /// </param>
        /// <param name="quality">
        /// The new quality.
        /// </param>
        /// <param name="symbol">
        /// The symbol.
        /// </param>
        /// <param name="cause">
        /// The cause of the data quality event.
        /// </param>
        /// <param name="platforminfo">
        /// Info associated with the data quality event.
        /// </param>
        /// <param name="closure">
        /// The native closure passed to the create function.
        /// </param>
        public delegate void OnSubscriptionQualityDelegate(IntPtr nativeHandle, int quality, string symbol, short cause, string platforminfo, IntPtr closure);

        /// <summary>
        /// This delegate describes the native function invoked when a recap is requested upon detecting a
        /// sequence number gap.
        /// </summary>
        /// <param name="nativeHandle">
        /// The native subscription.
        /// </param>
        /// <param name="closure">
        /// The native closure passed to the create function.
        /// </param>
        public delegate void OnSubscriptionRecapRequestDelegate(IntPtr nativeHandle, IntPtr closure);

        #endregion

        /* ************************************************************** */
        #region Protected Member Variables

        /// <summary>
        /// The closure passed to the create function. This member is declared as protected
        /// as there is a public read only property to access it.
        /// </summary>
        protected object mClosure;

        /// <summary>
        /// The queue passed to the create function. This member is declared as protected
        /// as there is a public read only property to access it.
        /// </summary>
        protected MamaQueue mQueue;

        /// <summary>
        /// The transport passed to the create function. This member is declared as protected
        /// as there is a public read only property to access it.
        /// </summary>
        protected MamaTransport mTransport;

        #endregion

        /* ************************************************************** */
        #region Protected Static Member Variables

        /// <summary>
        /// This structure holds all of the callback delegates, it is initialised in the static constructor.
        /// </summary>
        protected static NativeMethods.SubscriptionCallbacks mCallbackDelegates;

        #endregion

        #endregion

        /* ************************************************************** */
        #region Construction and Finalization

        /// <summary>
        /// Static constructor initialises all of the delegates.
        /// </summary>
        static MamaBasicSubscription()
        {
            // Create the callback delegates structure
            mCallbackDelegates = new NativeMethods.SubscriptionCallbacks();

            // Create the individual delegates
            mCallbackDelegates.mCreate    = new OnSubscriptionCreateDelegate(MamaBasicSubscription.onCreate);
            mCallbackDelegates.mDestroy   = new OnSubscriptionDestroyDelegate(MamaBasicSubscription.onDestroy);
            mCallbackDelegates.mError     = new OnSubscriptionErrorDelegate(MamaBasicSubscription.OnError);
            mCallbackDelegates.mMessage   = new OnSubscriptionMessageDelegate(MamaBasicSubscription.OnMessage);
        }

        /// <summary>
        /// Constructor constructs a new basic subscription including creating the underlying native
        /// subscription.
        /// </summary>
        public MamaBasicSubscription() : base()
        {
            // Allocate the native subscription
            IntPtr nativeSubscription = IntPtr.Zero;
            CheckResultCode(NativeMethods.mamaSubscription_allocate(ref nativeSubscription));

            // Save the result in the member variable
            NativeHandle = nativeSubscription;
        }

        /// <summary>
        /// Constructor initialises the subscription using an existing native subscription.
        /// </summary>
        /// <param name="nativeHandle">
        /// The native handle of the existing basic subscritpion.
        /// </param>
        internal MamaBasicSubscription(IntPtr nativeHandle) : base(nativeHandle)
        {
        }

        #endregion

        /* ************************************************************** */
        #region Private Static Functions

        /// <summary>
        /// This event handler is called by the native layer whenever the subscription has been
        /// created.
        /// </summary>
        /// <param name="nativeHandle">
        /// The native handle for the subscription.
        /// </param>
        /// <param name="closure">
        /// Native reference to the closure.
        /// </param>
        private static void onCreate(IntPtr nativeHandle, IntPtr closure)
        {
            // Obtain the handle from the closure
            GCHandle handle = (GCHandle)closure;

            // Extract the impl from the handle
            MamaBasicSubscriptionImpl impl = (MamaBasicSubscriptionImpl)handle.Target;

            // Use the impl to invoke the error callback
            impl.InvokeCreate();
        }

        /// <summary>
        /// This event handler is called by the native layer whenever the subscription has been fully destroyed.
        /// It will perform all clean-up and then invoke the onDestroy callback function if this has
        /// been supplied.
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

            // Extract the impl from the handle
            MamaBasicSubscriptionImpl impl = (MamaBasicSubscriptionImpl)handle.Target;

            // Use the impl to invoke the destroy callback, (if this has been supplied)
            impl.InvokeDestroy();

            /* The timer has now been destroyed and the impl is no longer required, free the handle to
             * allow the garbage collector to clean it up.
             */
            handle.Free();
        }

        /// <summary>
        /// This event handler is called by the native layer whenever an error occurs with the subscription.
        /// </summary>
        /// <param name="nativeHandle">
        /// The native handle for the subscription.
        /// </param>
        /// <param name="nativeStatus">
        /// The native status code for this error.
        /// </param>
        /// <param name="platformError">
        /// Platform specific error information.
        /// </param>
        /// <param name="subject">
        /// Subject for entitlement errors.
        /// </param>
        /// <param name="closure">
        /// Native reference to the closure.
        /// </param>
        private static void OnError(IntPtr nativeHandle, int nativeStatus, IntPtr platformError, string subject, IntPtr closure)
        {
            // Obtain the handle from the closure
            GCHandle handle = (GCHandle)closure;

            // Extract the impl from the handle
            MamaBasicSubscriptionImpl impl = (MamaBasicSubscriptionImpl)handle.Target;

            // Use the impl to invoke the error callback
            impl.InvokeError(nativeStatus, subject);
        }

        /// <summary>
        /// Invoked whenever a message is sent to the subscription.
        /// </summary>
        /// <param name="nativeHandle">
        /// The native handle for the subscription.
        /// </param>
        /// <param name="nativeMsg">
        /// Native handle for the message.
        /// </param>
        /// <param name="closure">
        /// Native reference to the closure.
        /// </param>
        /// <param name="itemClosure">
        /// Item closure for this subject.
        /// </param>
        private static void OnMessage(IntPtr nativeHandle, IntPtr nativeMsg, IntPtr closure, IntPtr itemClosure)
        {
            // Obtain the handle from the closure
            GCHandle handle = (GCHandle)closure;

            // Extract the impl from the handle
            MamaBasicSubscriptionImpl impl = (MamaBasicSubscriptionImpl)handle.Target;

            // Use the impl to invoke the callback
            impl.InvokeMessage(nativeMsg);
        }

        #endregion

        /* ************************************************************** */
        #region Protected Functions

        /// <summary>
        /// This will be called by the base class to de-allocate the native subscription.
        /// <seealso cref="MamaWrapper.DestroyNativePeer"/>
        /// </summary>
        /// <returns>
        /// MAMA Status code.
        /// </returns>
        protected override MamaStatus.mamaStatus DestroyNativePeer()
        {
            // Returns
            MamaStatus.mamaStatus ret = MamaStatus.mamaStatus.MAMA_STATUS_OK;

            // Only deallocate if this has not already been done
            if (IntPtr.Zero != NativeHandle)
            {
                ret = (MamaStatus.mamaStatus)NativeMethods.mamaSubscription_deallocate(NativeHandle);
            }

            return ret;
        }

        #endregion

        /* ************************************************************** */
        #region Public Functions

        /// <summary>
        /// This function will create the basic subscription without marketdata semantics.
        /// To pass a closure use the overload.
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
        public void createBasic(MamaTransport transport, MamaQueue queue, MamaBasicSubscriptionCallback callback, string symbol)
        {
            // Call the overloade
            createBasic(transport, queue, callback, symbol, null);
        }

        /// <summary>
        /// This function will create the basic subscription without marketdata semantics.
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
        public void createBasic(MamaTransport transport, MamaQueue queue, MamaBasicSubscriptionCallback callback, string symbol, object closure)
        {
            // Ensure that the native subscription has been allocated
            EnsurePeerCreated();

            // Save arguments in member variables
            mClosure   = closure;
            mQueue     = queue;
            mTransport = transport;

            // Create the impl
            IntPtr impl = MamaBasicSubscriptionImpl.Create(callback, closure, this);

            /* Create the subscription, register for the destroy callback regardless if the client wants it or not,
             * this is to allow clean-up to be done whenever the timer has been fully destroyed.
             */
            CheckResultCode(NativeMethods.mamaSubscription_createBasic(
                NativeHandle,
                transport.NativeHandle,
                queue.NativeHandle,
                ref mCallbackDelegates,
                symbol,
                impl));
        }

        /// <summary>
        /// Free the memory for a mamaSubscription which was allocated via a call to constructor.
        /// This function will call destroy() if the subscription has not already been destroyed.
        /// Calling this function will reduce time during finalization.
        /// </summary>
        public void deallocate()
        {
            // Verify that the native subscription has been created
            EnsurePeerCreated();

            // Dispose the subscription to de-allocate the memory
            Dispose();
        }

        /// <summary>
        /// This function will destroy the subscription, it must be called from
        /// the same thread that is processing the queue for the subscription.
        /// Use destroyEx to destroy the subscription from any thread.
        /// Note that the subscription will not be fully destroyed until the
        /// onDestroy callback is received.
        /// </summary>
        public void destroy()
        {
            // Verify that the native subscription has been created
            EnsurePeerCreated();

            // Clear the member variables
            mClosure   = null;
            mQueue     = null;
            mTransport = null;

            // Call the native function
            CheckResultCode(NativeMethods.mamaSubscription_destroy(NativeHandle));
        }

        /// <summary>
        /// Destroy the subscription.
        /// This function is another option to destroy the resources associated with the subscription
        /// It will schedule the destroy() of the subscription on the queue on which it dispatches.
        /// This function does not free the memory associated with the subscription.
        /// create() can be called again after this function has
        /// been called.
        /// After the Subscription is effectively destroyed, the OnDestroy callback will be triggered
        /// for it.
        ///
        /// This function can be called from any thread, as opposed to destroy().
        /// </summary>
        public void destroyEx()
        {
            // Verify that the native subscription has been created
            EnsurePeerCreated();

            // Clear the member variables
            mClosure   = null;
            mQueue     = null;
            mTransport = null;

            // Call the native function
            CheckResultCode(NativeMethods.mamaSubscription_destroyEx(NativeHandle));
        }

        /// <summary>
        /// This read only property returns the current state of the subscription, this function
        /// should be used in preference to the mamaSubscription_isActive or mamaSubscription_isValid
        /// functions.
        /// function is thread-safe.
        /// The state will be one of the mamaSubscriptionState enumeration values.
        /// </summary>
        public mamaSubscriptionState State
        {
            get
            {
                // Only continue if the subscription is valid
                EnsurePeerCreated();

                // Call the native function to get the state
                int state = 0;
                CheckResultCode(NativeMethods.mamaSubscription_getState(NativeHandle, ref state));

                // Convert the integer return into an enumerate value
                return (mamaSubscriptionState)state;
            }
        }

        /// <summary>
        /// Read only property returns the closure passed to the create function.
        /// </summary>
        public object subscClosure
        {
            get
            {
                return mClosure;
            }
        }

        /// <summary>
        /// Read only property returns the queue.
        /// </summary>
        public MamaQueue subscQueue
        {
            get
            {
                return mQueue;
            }
        }

        /// <summary>
        /// Read only property returns the symbol for this subscription.
        /// </summary>
        public string subscSymbol
        {
            get
            {
                // Get the symbol from the native layer
                IntPtr ret = IntPtr.Zero;
                CheckResultCode(NativeMethods.mamaSubscription_getSubscSymbol(NativeHandle, ref ret));

                // Convert to an ANSI string
                return Marshal.PtrToStringAnsi(ret);
            }
        }

        #endregion
    }

    #endregion
}
