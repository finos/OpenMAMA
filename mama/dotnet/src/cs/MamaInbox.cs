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
    #region MamaInbox Class

    /// <summary>
	/// A class for processing point-to-point messaging
	/// </summary>
	public class MamaInbox : MamaWrapper
    {
        /* ************************************************************** */
        #region Class Member Definition

        /* ************************************************************** */
        #region Native Method Prototypes

        private struct NativeMethods
        {
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern int mamaInbox_create2(
                ref IntPtr inbox,
                IntPtr transport,
                IntPtr queue,
                OnInboxMsgDelegate messageCallback,
                OnInboxErrorDelegate errorCallback,
                OnInboxDestroyDelegate destroyCallback,
                IntPtr closure);

            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern int mamaInbox_destroy(IntPtr inbox);
        }

        #endregion

        /* ************************************************************** */
        #region Private Classes

        /// <summary>
        /// This impl will perform the work of invoking user supplied callbacks, it is
        /// held to ensure that the user can destroy and recreate the parent inbox during
        /// the callbacks.
        /// </summary>
        private class MamaInboxImpl
        {
            /* ************************************************************** */
            #region Private Member Variables

            /// <summary>
            /// The callback interface.
            /// </summary>
            private MamaInboxCallback mCallback;

            /// <summary>
            /// The closure provided by the user.
            /// </summary>
            private object mClosure;

            /// <summary>
            /// The actual inbox object that will be provided in the callbacks.
            /// </summary>
            private MamaInbox mInbox;

            #endregion

            /* ************************************************************** */
            #region Construction and Finalization

            /// <summary>
            /// Constructor initialises all member variables.
            /// </summary>
            /// <param name="callback">
            /// The user callback implementation.
            /// </param>
            /// <param name="closure">
            /// The closure supplied to the MamaInbox.create function.
            /// </param>
            /// <param name="inbox">
            /// The actual C# inbox object.
            /// </param>
            internal MamaInboxImpl(MamaInboxCallback callback, object closure, MamaInbox inbox)
            {
                // Save arguments in member variables
                mCallback  = callback;
                mClosure   = closure;
                mInbox     = inbox;
            }

            #endregion

            /* ************************************************************** */
            #region Internal Operations

            /// <summary>
            /// This function creates a new impl and returns an IntPtr that can then be passed to
            /// the native layer.
            /// </summary>
            /// <param name="callback">
            /// The user callback implementation.
            /// </param>
            /// <param name="closure">
            /// The closure supplied to the MamaInbox.create function.
            /// </param>
            /// <param name="inbox">
            /// The actual C# inbox object.
            /// </param>
            /// <returns>
            /// The IntPtr that can then be used for the closure.
            /// </returns>
            internal static IntPtr Create(MamaInboxCallback callback, object closure, MamaInbox inbox)
            {
                // Allocate a new impl
                MamaInboxImpl impl = new MamaInboxImpl(callback, closure, inbox);

                // Create a GC handle
                GCHandle handle = GCHandle.Alloc(impl);

                // Return the native pointer
                return (IntPtr)handle;
            }

            /// <summary>
            /// This function will invoke the destroy callback.
            /// </summary>
            internal void InvokeDestroy()
            {
                if (null != mCallback)
                {
                    // Invoke the onDestroy
                    mCallback.onDestroy(mInbox, mClosure);
                }
            }

            /// <summary>
            /// This function will invoke the error callback on the user supplied callback implementation.
            /// </summary>
            /// <param name="nativeStatus">
            /// The status code for the error that has occured.
            /// </param>
            internal void InvokeError(int nativeStatus)
            {
                // Only invoke the callback if it is supplied
                if (null != mCallback)
                {
                    // Create a managed status value fo the native value passed in
                    MamaStatus.mamaStatus status = (MamaStatus.mamaStatus)nativeStatus;

                    // Invoke the callback
                    mCallback.onError(mInbox, status);
                }
            }

            /// <summary>
            /// This function will invoke the message callback on the user supplied callback implementation.
            /// </summary>
            /// <param name="nativeMsg">
            /// The native MAMA message.
            /// </param>
            internal void InvokeMessage(IntPtr nativeMsg)
            {
                if (null != mCallback)
                {
                    // Create a new MamaaMsg using the native handle passed in
                    MamaMsg message = new MamaMsg(nativeMsg);

                    // Invoke the callback
                    mCallback.onMsg(mInbox, message);
                }
            }

            #endregion
        }

        #endregion

        /* ************************************************************** */
        #region Private Delegates

        /// <summary>
        /// This delegate describes the native callback function for the destroy event.
        /// </summary>
        /// <param name="inbox">
        /// The native inbox.
        /// </param>
        /// <param name="closure">
        /// Closure passed to the create function.
        /// </param>
        private delegate void OnInboxDestroyDelegate(IntPtr inbox, IntPtr closure);

        /// <summary>
        /// Allows the native layer to invoke the error callback.
        /// </summary>
        /// <param name="nativeStatus">
        /// The native error status.
        /// </param>
        /// <param name="closure">
        /// Native reference to the closure.
        /// </param>
        private delegate void OnInboxErrorDelegate(int nativeStatus, IntPtr closure);

        /// <summary>
        /// Allows the native layer to invoke the message callback.
        /// </summary>
        /// <param name="msg">
        /// Native handle for the message.
        /// </param>
        /// <param name="closure">
        /// Native reference to the closure.
        /// </param>
        private delegate void OnInboxMsgDelegate(IntPtr msg, IntPtr closure);

        #endregion

        /* ************************************************************** */
        #region Private Member Variables

        /// <summary>
        /// The closure is also stored for ease of access.
        /// </summary>
        private object mClosure;

        #endregion

        /* ************************************************************** */
        #region Private Static Member Variables

        /// <summary>
        /// This delegate is used to invoke the destroy callback.
        /// </summary>
        private static OnInboxDestroyDelegate mDestroyDelegate = new OnInboxDestroyDelegate(MamaInbox.onDestroy);

        /// <summary>
        /// This delegate is used to invoke the error callback.
        /// </summary>
        private static OnInboxErrorDelegate mErrorDelegate = new OnInboxErrorDelegate(MamaInbox.onError);

        /// <summary>
        /// This delegate is used to invoke the message callback.
        /// </summary>
        private static OnInboxMsgDelegate mMsgDelegate = new OnInboxMsgDelegate(MamaInbox.onMessage);

        #endregion

        #endregion

        /* ************************************************************** */
        #region Construction and Finalization

        /// <summary>
		/// Construct an inbox.
		/// </summary>
		public MamaInbox() : base()
		{
		}

        /// <summary>
        /// Construct an inbox object using a precreated native inbox.
        /// </summary>
        /// <param name="nativeHandle">
        /// The native inbox handle.
        /// </param>
		internal MamaInbox(IntPtr nativeHandle) : base(nativeHandle)
		{
        }

        #endregion

        /* ************************************************************** */
        #region Private Static Functions

        /// <summary>
        /// This event handler is called by the native layer whenever the inbox has been fully destroyed.
        /// It will perform all clean-up and then invoke the onDestroy callback function if this has
        /// been supplied.
        /// </summary>
        /// <param name="inbox">
        /// The native inbox.
        /// </param>
        /// <param name="closure">
        /// The closure passed down to the native layer.
        /// </param>
        private static void onDestroy(IntPtr inbox, IntPtr closure)
        {
            // Obtain the handle from the closure
            GCHandle handle = (GCHandle)closure;

            // Extract the impl from the handle
            MamaInboxImpl impl = (MamaInboxImpl)handle.Target;

            // Use the impl to invoke the destroy callback, (if this has been supplied)
            impl.InvokeDestroy();

            /* The timer has now been destroyed and the impl is no longer required, free the handle to
             * allow the garbage collector to clean it up.
             */
            handle.Free();
        }

        /// <summary>
        /// Allows the native layer to invoke the error callback.
        /// </summary>
        /// <param name="nativeStatus">
        /// The native error status.
        /// </param>
        /// <param name="closure">
        /// Native reference to the closure.
        /// </param>
        private static void onError(int nativeStatus, IntPtr closure)
        {
            // Obtain the handle from the closure
            GCHandle handle = (GCHandle)closure;

            // Extract the impl from the handle
            MamaInboxImpl impl = (MamaInboxImpl)handle.Target;

            // Use the impl to invoke the error callback
            impl.InvokeError(nativeStatus);
        }

        /// <summary>
        /// Invoked whenever a message is sent to the inbox.
        /// </summary>
        /// <param name="nativeMsg">
        /// Native handle for the message.
        /// </param>
        /// <param name="closure">
        /// Native reference to the closure.
        /// </param>
        private static void onMessage(IntPtr nativeMsg, IntPtr closure)
        {
            // Obtain the handle from the closure
            GCHandle handle = (GCHandle)closure;

            // Extract the impl from the handle
            MamaInboxImpl impl = (MamaInboxImpl)handle.Target;

            // Use the impl to invoke the error callback
            impl.InvokeMessage(nativeMsg);
        }

        #endregion

        /* ************************************************************** */
        #region Protected Functions

        /// <summary>
		/// Implements the destruction of the underlying peer object.
		/// <seealso cref="MamaWrapper.DestroyNativePeer"/>
		/// </summary>
		protected override MamaStatus.mamaStatus DestroyNativePeer()
		{
            // Destroy the native inbox
            return (MamaStatus.mamaStatus)NativeMethods.mamaInbox_destroy(NativeHandle);
		}

        #endregion

        /* ************************************************************** */
        #region Public Functions

        /// <summary>
		/// Creates an inbox and stores at the address specified by the calling client.
		/// </summary>
		/// <param name="transport">
        /// The mamaTransport being used.
        /// </param>
		/// <param name="queue">
        /// Optional mamaQueue. Will use default queue if null.
        /// </param>
		/// <param name="callback">
        /// A callback interface invoked in response to a p2p message being received or when an error is
        /// encountered during p2p messaging.
        /// </param>
		/// <param name="closure">
        /// User supplied data
        /// </param>
		public void create(MamaTransport transport, MamaQueue queue, MamaInboxCallback callback, object closure)
		{
            // Check the arguments
			if (transport == null)
			{
				throw new ArgumentNullException("transport");
			}

            if(queue == null)
            {
                throw new ArgumentNullException("queue");
            }

			if (callback == null)
			{
				throw new ArgumentNullException("callback");
			}

            // Save the clousre in the member variable
            mClosure = closure;

            // Create the impl
            IntPtr impl = MamaInboxImpl.Create(callback, closure, this);

            /* Create the inbox, register for the destroy callback regardless if the client wants it or not,
             * this is to allow clean-up to be done whenever the inbox has been fully destroyed.
             */
            IntPtr nativeInbox = IntPtr.Zero;
            CheckResultCode(NativeMethods.mamaInbox_create2(
                ref nativeInbox,
                transport.NativeHandle,
                queue.NativeHandle,
                mMsgDelegate,
                mErrorDelegate,
                mDestroyDelegate,
                impl));

            // Save the native timer in the member variable
            NativeHandle = nativeInbox;
		}

		/// <summary>
		/// Creates an inbox and stores at the address specified by the calling client.
		/// </summary>
		/// <param name="transport">
        /// The mamaTransport being used.
        /// </param>
		/// <param name="queue">
        /// Optional mamaQueue. Will use default queue if null.
        /// </param>
		/// <param name="callback">
        /// A callback interface invoked in response to a p2p message being received or when an error is
        /// encountered during p2p messaging.
        /// </param>
		public void create(MamaTransport transport, MamaQueue queue, MamaInboxCallback callback)
		{
			create(transport, queue, callback, null);
		}

		/// <summary>
		/// Creates an inbox and stores at the address specified by the calling client.
		/// Uses a default queue.
        /// This functionality is no longer supported.
		/// </summary>
		/// <param name="transport">
        /// The mamaTransport being used.
        /// </param>
		/// <param name="callback">
        /// A callback interface invoked in response to a p2p message being received or hen an error is
        /// encountered during p2p messaging.
        /// </param>
        /// <param name="closure">
        /// User supplied data.
        /// </param>
		public void create(MamaTransport transport, MamaInboxCallback callback, object closure)
		{
            throw new NotSupportedException("This function is no longer supported, use an overload that takes a MamaQueue object.");
		}

		/// <summary>
		/// A synonym to the <see cref="Wombat.MamaWrapper.Dispose()">MamaWrapper.Dispose</see> method.
		/// </summary>
		public void destroy()
		{
            // Dispose, note that the cleanup will not be done until the destroy callback returns
			Dispose();
		}

		/// <summary>
        /// Returns the user supplied data passed to create.
		/// </summary>
		/// <returns>
        /// The user supplied data passed to create.
        /// </returns>
		public object getClosure()
		{
            // Return the closure from the first delegate
            return mClosure;
        }

        #endregion
    }

    #endregion
}
