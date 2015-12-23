/* $Id: MamaPublisher.cs,v 1.9.2.6 2012/08/24 16:12:01 clintonmcdowell Exp $
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
using System.Collections;
using System.Runtime.InteropServices;

namespace Wombat
{
	/// <summary>
    /// The MamaPublisher class publishes messages to basic or market data
    /// subscriptions depending on the underlying transport
	/// </summary>
	public class MamaPublisher : MamaWrapper
	{
        /* ************************************************************** */
        #region Private Member Variables

        /// <summary>
        /// This is used to store MamaCallback objects that are passed to the native layer
        /// when using the throttle.
        /// </summary>
        private MamaCallbackStore<MamaSendCompleteCallback, MamaThrottledSendCompleteDelegate> mCallbackStore;
        private MamaMsg mReusableMsg;
        private MamaTransport mReusableTransport;

        #endregion

        /* ************************************************************** */
        #region Construction and Finalization

		/// <summary>
		/// <see cref="M:Wombat.MamaWrapper.#ctor" />
		/// </summary>
		public MamaPublisher() : base()
		{
			// Create the callback store
            mCallbackStore = new MamaCallbackStore<MamaSendCompleteCallback, MamaThrottledSendCompleteDelegate>();
		}

		/// <summary>
		/// <see cref="M:Wombat.MamaWrapper.#ctor(System.IntPtr)" />
		/// </summary>
		internal MamaPublisher(IntPtr nativeHandle) : base(nativeHandle)
		{
			// Create the callback store
            mCallbackStore = new MamaCallbackStore<MamaSendCompleteCallback, MamaThrottledSendCompleteDelegate>();            
        }

        #endregion

        protected override void Dispose(bool disposing)
        {
            // Dispose managed resources
            if (disposing)
            {
                // Dispose the wrapper store
                if (mCallbackStore != null)
                {
                    IDisposable disposable = (IDisposable)mCallbackStore;
                    disposable.Dispose();
                    mCallbackStore = null;
                }
            }
		}

        /// <summary>
        /// Implements the destruction of the underlying peer object
        /// <seealso cref="MamaWrapper.DestroyNativePeer"/>
        /// </summary>
        /// <returns>MAMA Status code</returns>
        protected override MamaStatus.mamaStatus DestroyNativePeer()
		{
			return (MamaStatus.mamaStatus)NativeMethods.mamaPublisher_destroy(nativeHandle);
		}

		/// <summary>
		/// Create a mama publisher for the corresponding transport. If the transport
		/// is a marketdata transport, as opposed to a "basic" transport, the topic
		/// corresponds to the symbol. For a basic transport, the source and root get 
		/// ignored.
		/// </summary>
		/// <param name="transport">The transport.</param>
		/// <param name="topic">Symbol on which to publish.</param>
		/// <param name="source">The source for market data publishers. (e.g. source.symbol)</param>
		/// <param name="root">The root for market data publishers. Used internally.</param>
		public void create(
			MamaTransport transport,
			string topic,
			string source,
			string root)
		{
#if MAMA_WRAPPERS_CHECK_ARGUMENTS
			if (transport == null)
			{
				throw new ArgumentNullException("transport");
			}
			if (topic == null)
			{
				throw new ArgumentNullException("topic");
			}
			if (nativeHandle != IntPtr.Zero)
			{
				throw new InvalidOperationException("MamaPublisher already created");
			}
#endif // MAMA_WRAPPERS_CHECK_ARGUMENTS
			
			int code = NativeMethods.mamaPublisher_create(ref nativeHandle, transport.NativeHandle, topic, source, root);
			CheckResultCode(code);
			GC.KeepAlive(transport);
		}

        /// <summary>
        /// Create a mama publisher for the corresponding transport. If the transport
        /// is a marketdata transport, as opposed to a "basic" transport, the topic
        /// corresponds to the symbol. For a basic transport, the source and root get 
        /// ignored.
        /// </summary>
        /// <param name="transport">The transport.</param>
        /// <param name="topic">Symbol on which to publish.</param>
        /// <param name="source">The source for market data publishers. (e.g. source.symbol)</param>
        /// <param name="root">The root for market data publishers. Used internally.</param>
        public void createWithCallbacks(
            MamaTransport transport,
            MamaQueue queue,
            MamaPublisherCallback callback,
            Object closure,
            string topic,
            string source,
            string root)
        {
#if MAMA_WRAPPERS_CHECK_ARGUMENTS
			if (transport == null)
			{
				throw new ArgumentNullException("transport");
			}
			if (topic == null)
			{
				throw new ArgumentNullException("topic");
			}
			if (queue == null)
			{
				throw new ArgumentNullException("queue");
			}
			if (callback == null)
			{
				throw new ArgumentNullException("callback");
			}
			if (nativeHandle != IntPtr.Zero)
			{
				throw new InvalidOperationException("MamaPublisher already created");
			}
#endif // MAMA_WRAPPERS_CHECK_ARGUMENTS
            mCallback = callback;

            GCHandle handle = GCHandle.Alloc(this);

            int code = NativeMethods.mamaPublisher_createWithCallbacks(
                ref nativeHandle,
                transport.NativeHandle,
                queue.NativeHandle,
                ref mCallbackDelegates,
                (IntPtr) handle,
                topic, source, root);
            CheckResultCode(code);
            GC.KeepAlive(transport);
            GC.KeepAlive(queue);
            GC.KeepAlive(callback);
        }

        /// <summary>
		/// Create a mama publisher for the corresponding transport. If the transport
		/// is a marketdata transport, as opposed to a "basic" transport, the topic
		/// corresponds to the symbol. For a basic transport, the source and root get 
		/// ignored.
		/// </summary>
		/// <param name="transport">The transport.</param>
		/// <param name="topic">Symbol on which to publish.</param>
		/// <param name="source">The source for market data publishers. (e.g. source.symbol)</param>
		public void create(
			MamaTransport transport,
			string topic,
			string source)
		{
			create(transport, topic, source, null);
		}

		/// <summary>
		/// Create a mama publisher for the corresponding transport. If the transport
		/// is a marketdata transport, as opposed to a "basic" transport, the topic
		/// corresponds to the symbol. For a basic transport, the source and root get 
		/// ignored.
		/// </summary>
		/// <param name="transport">The transport.</param>
		/// <param name="topic">Symbol on which to publish.</param>
		public void create(
			MamaTransport transport,
			string topic)
		{
			create(transport, topic, null, null);
		}

		/// <summary>
		/// Send a message from the specified publisher.
		/// </summary>
		/// <param name="message">The mamaMsg to send.</param>
		public void send(MamaMsg message)
		{
#if MAMA_WRAPPERS_CHECK_ARGUMENTS
			if (message == null)
			{
				throw new ArgumentNullException("message");
			}
			EnsurePeerCreated();
#endif // MAMA_WRAPPERS_CHECK_ARGUMENTS
			
			int code = NativeMethods.mamaPublisher_send(nativeHandle, message.NativeHandle);
			CheckResultCode(code);
			GC.KeepAlive(message);
		}

		/// <summary>
		/// Send a message with the throttle.
		/// The lifecycle of the message sent is controlled by the user of the API. The
		/// callback indicates when the API is no longer using the message and can be
		/// destroyed/reused by the application.
		/// </summary>
		/// <param name="message">The mamaMsg to send.</param>
		/// <param name="callback">Callback invoked once the message has been send on
		/// the throttle</param>
		/// <param name="closure">User supplied data returned in the send complete callback.</param>
		public void sendWithThrottle(
			MamaMsg message, 
			MamaSendCompleteCallback callback,
			object closure)
		{
#if MAMA_WRAPPERS_CHECK_ARGUMENTS
			if (message == null)
			{
				throw new ArgumentNullException("message");
			}
			if (callback == null)
			{
				throw new ArgumentNullException("callback");
			}
			EnsurePeerCreated();
#endif // MAMA_WRAPPERS_CHECK_ARGUMENTS

            // Create a new callback wrapper
            MamaCallbackWrapper<MamaSendCompleteCallback, MamaThrottledSendCompleteDelegate> wrapper =
                new MamaCallbackWrapper<MamaSendCompleteCallback, MamaThrottledSendCompleteDelegate>(
                    callback,
                    closure,
                    new MamaThrottledSendCompleteDelegate(onSendComplete));

            // Add this to the store
            IntPtr nativeClosure = mCallbackStore.StoreWrapper(wrapper);

            // Call the native function
			int code = NativeMethods.mamaPublisher_sendWithThrottle(
				nativeHandle, 
				message.NativeHandle, 
                (Wombat.MamaPublisher.MamaThrottledSendCompleteDelegate)wrapper.NativeDelegate,
                nativeClosure);
			try
			{
				CheckResultCode(code);
			}

            // If something goes wrong then remove the wrapper from the store
			catch
			{
                // Remove the wrapper
                mCallbackStore.RemoveWrapper(nativeClosure);

                // Dispose it
                ((IDisposable)wrapper).Dispose();

                // Rethrow the exception
				throw;
			}

            // Ensure that the message passed will not delete its native peer
            message.SelfManageLifeTime(false);
		}

		/// <summary>
		/// Send a p2p message from the specified inbox using the throttle.
		/// </summary>
		/// <param name="inbox">The mamaInbox which will process any response to the sent
		/// message.</param>
		/// <param name="message">The mamaMsg to send.</param>
		public void sendFromInbox(
			MamaInbox inbox,
			MamaMsg message)
		{
#if MAMA_WRAPPERS_CHECK_ARGUMENTS
			if (inbox == null)
			{
				throw new ArgumentNullException("inbox");
			}
			if (message == null)
			{
				throw new ArgumentNullException("message");
			}
			EnsurePeerCreated();
#endif // MAMA_WRAPPERS_CHECK_ARGUMENTS
			
			int code = NativeMethods.mamaPublisher_sendFromInbox(nativeHandle, inbox.NativeHandle, message.NativeHandle);
			CheckResultCode(code);
			GC.KeepAlive(inbox);
			GC.KeepAlive(message);
		}

		/// <summary>
		/// Send a p2p message from the specified inbox using the throttle.
		/// The lifecycle of the message sent is controlled by the user of the API. The
		/// callback indicates when the API is no longer using the message and can be
		/// destroyed/reused by the application.
		/// </summary>
		/// <param name="inbox">The MamaInbox which will process any response to the sent message.</param>
		/// <param name="message">The MamaMsg to send.</param>
		/// <param name="callback">The callback which will be invoked when the message
		/// is sent from the throttle queue.</param>
		/// <param name="closure">User supplied data returned when the callback is invoked.</param>
		public void sendFromInboxWithThrottle(
			MamaInbox inbox,
			MamaMsg message,
			MamaSendCompleteCallback callback,
			object closure)
		{
#if MAMA_WRAPPERS_CHECK_ARGUMENTS
			if (inbox == null)
			{
				throw new ArgumentNullException("inbox");
			}
			if (message == null)
			{
				throw new ArgumentNullException("message");
			}
			if (callback == null)
			{
				throw new ArgumentNullException("callback");
			}
			EnsurePeerCreated();
#endif // MAMA_WRAPPERS_CHECK_ARGUMENTS

            // Create a new callback wrapper
            MamaCallbackWrapper<MamaSendCompleteCallback, MamaThrottledSendCompleteDelegate> wrapper =
                new MamaCallbackWrapper<MamaSendCompleteCallback, MamaThrottledSendCompleteDelegate>(
                    callback,
                    closure,
                    new MamaThrottledSendCompleteDelegate(onSendComplete));

            // Add this to the store
            IntPtr nativeClosure = mCallbackStore.StoreWrapper(wrapper);

			// Call the native function
			int code = NativeMethods.mamaPublisher_sendFromInboxWithThrottle(
				nativeHandle,
                inbox.NativeHandle,
				message.NativeHandle, 
                (Wombat.MamaPublisher.MamaThrottledSendCompleteDelegate)wrapper.NativeDelegate,
                nativeClosure);
			try
			{
				CheckResultCode(code);
			}

            // If something goes wrong then remove the wrapper from the store
			catch
			{
                // Remove the wrapper
                mCallbackStore.RemoveWrapper(nativeClosure);

                // Dispose it
                ((IDisposable)wrapper).Dispose();
                
                // Rethrow the exception
				throw;
			}
		
            // Ensure that the message passed will not delete its native peer
            message.SelfManageLifeTime(false);
		}

        // C-like callback used in the interop call
        private delegate void MamaThrottledSendCompleteDelegate(
            IntPtr publisher,
            IntPtr message,
            int status,
            IntPtr closure);

		/// <summary>
		/// Send a reply in response to a request to an inbox.
		/// </summary>
		/// <param name="request">The MamaMsg to which you are responding.</param>
		/// <param name="reply">The MamaMsg to be sent as the reply.</param>
		public void sendReplyToInbox(
			MamaMsg request,
			MamaMsg reply)
		{
#if MAMA_WRAPPERS_CHECK_ARGUMENTS
			if (request == null)
			{
				throw new ArgumentNullException("request");
			}
			if (reply == null)
			{
				throw new ArgumentNullException("reply");
			}
			EnsurePeerCreated();
#endif // MAMA_WRAPPERS_CHECK_ARGUMENTS

			int code = NativeMethods.mamaPublisher_sendReplyToInbox(nativeHandle, request.NativeHandle, reply.NativeHandle);
			CheckResultCode(code);
			GC.KeepAlive(request);
			GC.KeepAlive(reply);
		}

		/// <summary>
		/// Destroy the publisher.
		/// A synonym to the <see cref="Wombat.MamaWrapper.Dispose()">MamaWrapper.Dispose</see> method.
		/// </summary>
		public void destroy()
		{
			Dispose();
		}

		#region Implementation details

        /// <summary>
        /// This handler is called whenever an asynchronous publisher send has completed.
        /// </summary>
        /// <param name="publisher">
        /// Native reference to the publisher/
        /// </param>
        /// <param name="msg">
        /// Native message.
        /// </param>
        /// <param name="status">
        /// Status returned.
        /// </param>
        /// <param name="closure">
        /// The closure is a reference to the callback wrapper object used to keep the C#
        /// objects alive.
        /// </param>
		private void onSendComplete(IntPtr publisher, IntPtr msg, int status, IntPtr closure)
		{
            // Extract the wrapper object from the store
            using (MamaCallbackWrapper<MamaSendCompleteCallback, MamaThrottledSendCompleteDelegate> wrapper 
                = mCallbackStore.RemoveWrapper(closure))
            {
				if (mReusableMsg == null)
				{
					mReusableMsg = new MamaMsg (msg);
				}
				else
				{
					mReusableMsg.setNativeHandle (msg);
				}
                
				MamaSendCompleteCallback callback = (MamaSendCompleteCallback)wrapper.Callback;
                callback.onSendComplete(this, mReusableMsg, (MamaStatus.mamaStatus)status, wrapper.Closure);
			}
		}

        /// <summary>
        /// Get the MamaTransport.
        /// The result contains the reusable MamaTransport object of the
        /// nativeHandle object. Applications calling this method will receive the same
        /// reusable object for repeated calls on same nativeHandle object.
        /// </summary>
        public MamaTransport getTransport()
        {
            EnsurePeerCreated();
            IntPtr transport = IntPtr.Zero;
            int code = NativeMethods.mamaPublisher_getTransport(nativeHandle, ref transport);
            CheckResultCode(code);

            if (mReusableTransport == null)
            {
                mReusableTransport = new MamaTransport (transport);
            }
            else
            {
                mReusableTransport.setNativeHandle(transport);
            }
            return mReusableTransport;
        }

        /// <summary>
        /// Get the name of the transport.
        /// </summary>
        public string getRoot()
        {
            // Get the symbol from the native layer
            IntPtr ret = IntPtr.Zero;
            CheckResultCode(NativeMethods.mamaPublisher_getRoot(nativeHandle, ref ret));

            // Convert to an ANSI string
            return Marshal.PtrToStringAnsi(ret);
        }

        /// <summary>
        /// Get the name of the transport.
        /// </summary>
        public string getSource()
        {
            // Get the symbol from the native layer
            IntPtr ret = IntPtr.Zero;
            CheckResultCode(NativeMethods.mamaPublisher_getSource(nativeHandle, ref ret));

            // Convert to an ANSI string
            return Marshal.PtrToStringAnsi(ret);
        }

        /// <summary>
        /// Get the name of the transport.
        /// </summary>
        public string getSymbol()
        {
            // Get the symbol from the native layer
            IntPtr ret = IntPtr.Zero;
            CheckResultCode(NativeMethods.mamaPublisher_getSymbol(nativeHandle, ref ret));

            // Convert to an ANSI string
            return Marshal.PtrToStringAnsi(ret);
        }

        // =====================================================================================
        private static void onCreate(IntPtr nativeHandle, IntPtr closure)
        {
            // Obtain the handle from the closure
            GCHandle handle = (GCHandle)closure;

            // Extract the impl from the handle
            MamaPublisher pub = (MamaPublisher) handle.Target;

            // Use the impl to invoke the error callback
            if (null != pub)
            {
                // Invoke the callback
                pub.mCallback.onCreate(pub);
            }
        }

        private static void onError(IntPtr nativeHandle, short status, string topic, IntPtr closure)
        {
            // Obtain the handle from the closure
            GCHandle handle = (GCHandle)closure;

            // Extract the impl from the handle
            MamaPublisher pub = (MamaPublisher)handle.Target;

            // Use the impl to invoke the error callback
            if (null != pub)
            {
                // Invoke the callback
                pub.mCallback.onError(pub, (MamaStatus.mamaStatus) status, topic);
            }
        }

        private static void onDestroy(IntPtr nativeHandle, IntPtr closure)
        {
            // Obtain the handle from the closure
            GCHandle handle = (GCHandle)closure;

            // Extract the impl from the handle
            MamaPublisher pub = (MamaPublisher)handle.Target;

            // Use the impl to invoke the error callback
            if (null != pub)
            {
                // Invoke the callback
                pub.mCallback.onDestroy(pub);
            }
        }

        private static NativeMethods.PublisherCallbacks mCallbackDelegates;

        static MamaPublisher()
        {
            // Create the callback delegates structure
            mCallbackDelegates = new NativeMethods.PublisherCallbacks();

            // Create the individual delegates
            mCallbackDelegates.mCreate    = new OnPublisherCreateDelegate(MamaPublisher.onCreate);
            mCallbackDelegates.mDestroy   = new OnPublisherDestroyDelegate(MamaPublisher.onDestroy);
            mCallbackDelegates.mError     = new OnPublisherErrorDelegate(MamaPublisher.onError);
        }

        private delegate void OnPublisherCreateDelegate(IntPtr nativeHandle, IntPtr closure);

        private delegate void OnPublisherErrorDelegate(IntPtr nativeHandle, short status, string topic, IntPtr closure);

        private delegate void OnPublisherDestroyDelegate(IntPtr nativeHandle, IntPtr closure);
        // =====================================================================================
       
        // Interop API
		private struct NativeMethods
		{
            [StructLayout(LayoutKind.Sequential)]
            public struct PublisherCallbacks
            {
                public OnPublisherCreateDelegate mCreate;
                public OnPublisherErrorDelegate mError;
                public OnPublisherDestroyDelegate mDestroy;
                public IntPtr mReserved;
            }  

            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaPublisher_create(
				ref IntPtr result, 
				IntPtr tport,
				[MarshalAs(UnmanagedType.LPStr)] string symbol, 
				[MarshalAs(UnmanagedType.LPStr)] string source,
				[MarshalAs(UnmanagedType.LPStr)] string root);

            
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern int mamaPublisher_createWithCallbacks(
                ref IntPtr result,
                IntPtr tport,
                IntPtr queue,
                ref PublisherCallbacks callbacks,
                IntPtr closure,
                [MarshalAs(UnmanagedType.LPStr)] string symbol,
                [MarshalAs(UnmanagedType.LPStr)] string source,
                [MarshalAs(UnmanagedType.LPStr)] string root);

            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaPublisher_send(
				IntPtr publisher, 
				IntPtr msg);
			
			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaPublisher_sendWithThrottle(
				IntPtr publisher,
				IntPtr msg,
				MamaThrottledSendCompleteDelegate sendCompleteCallback,
				IntPtr closure);

			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaPublisher_sendFromInboxWithThrottle(
				IntPtr publisher, 
				IntPtr inbox, 
				IntPtr msg,
				MamaThrottledSendCompleteDelegate sendCompleteCallback,
				IntPtr closure);

			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaPublisher_sendFromInbox(
				IntPtr publisher, 
				IntPtr inbox, 
				IntPtr msg);

			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaPublisher_sendReplyToInbox(
				IntPtr publisher, 
				IntPtr request,
				IntPtr reply);

			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaPublisher_destroy(IntPtr publisher);

			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaPublisher_getTransport(
				IntPtr publisher,
				ref IntPtr result);

            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern int mamaPublisher_getRoot(IntPtr publisher, ref IntPtr ret);

            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern int mamaPublisher_getSource(IntPtr publisher, ref IntPtr ret);

            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern int mamaPublisher_getSymbol(IntPtr publisher, ref IntPtr ret);
        }

		private Hashtable mCallbacks = new Hashtable();

        private MamaPublisherCallback mCallback = null;

        #endregion Implementation details
	}
}
