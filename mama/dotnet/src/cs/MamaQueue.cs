/* $Id: MamaQueue.cs,v 1.13.2.8 2012/08/24 16:12:01 clintonmcdowell Exp $
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
using System.Threading;

using System.Collections.Generic;

namespace Wombat
{
	/// <summary>
	/// MamaQueues allow applications to dispatch events in order with
    /// multiple threads.
 	/// </summary>
    public class MamaQueue : MamaWrapper
    {
        private enum QueueState : byte
        {
            Stopped,
            Stopping,
            Running
        };

        QueueState status = QueueState.Stopped;

        /// <summary>
        /// Create a queue
        /// </summary>
        public MamaQueue (MamaBridge bridgeImpl) : base()
        {
			int code = NativeMethods.mamaQueue_create(ref nativeHandle, bridgeImpl.NativeHandle);
			CheckResultCode(code);
        }

		/// <summary/>
		internal MamaQueue(IntPtr nativeHandle) : base(nativeHandle)
		{
		}

		/// <summary>
		/// Specify a high watermark for events on the queue.
		/// The behaviour for setting this value results in on the underlying
		/// middleware varies.
		/// </summary>
		/// <remarks>
		/// LBM: LBM uses an unbounded event queue. Setting this values allows users
		/// of the API to receive a callback if the value is exceeded. 
		/// (<see cref="MamaQueue.setQueueMonitorCallbacks"/> for setting queue-related 
        /// callbacks.) The default behaviour is for the queue to grow unbounded without
		/// notifications.
        /// <para>
		/// The high watermark for LBM can be set for all queues at once by setting the
		/// mama.lbm.eventqueuemonitor.queue_size_warning property for the API. Calls
		/// to this function will override the value specified in mama.properties at
		/// runtime. Callbacks can be disabled by setting this value to 0 -
		/// effectively disabling high watermark checking.</para>
        /// <para>
		/// RV: This will set a queue limit policy of TIBRVQUEUE_DISCARD_FIRST whereby
		/// the oldest events in the queue are discarded first. The discard amount will
		/// be set with a value of 1. I.e. events will be dropped from the queue one at
		/// a time. The default behaviour is an unlimited queue which does not discard events.
        /// </para>
		/// </remarks>
		public void setHighWatermark(int highWatermark)
		{
			int code = NativeMethods.mamaQueue_setHighWatermark(nativeHandle, highWatermark);
			CheckResultCode(code);
		}

		/// <summary>
		/// Get the value of the high water mark for the specified queue. A value of 0
		/// will be returned if no high water mark was previously specified.
		/// </summary>
		public int getHighWatermark()
		{
			int result = 0;
			int code = NativeMethods.mamaQueue_getHighWatermark(nativeHandle, ref result);
			CheckResultCode(code);
			return result;
		}

		/// <summary>
		/// Set the low water mark for the queue. Only supported by Wombat TCP
		/// middleware.
		/// </summary>
        /// <remarks>
		/// The low watermark must be greater than 1 and less than highWaterMark otherwise this method
		/// returns MAMA_STATUS_INVALID_ARG. For this reason the high water mark must
		/// be set before invoking this method.
		/// </remarks>
		public void setLowWatermark(int lowWatermark)
		{
			int code = NativeMethods.mamaQueue_setLowWatermark(nativeHandle, lowWatermark);
			CheckResultCode(code);
		}

		/// <summary>
		/// Get the value of the low water mark for the specified queue. A value of 1
		/// will be returned if no low water mark was previously specified.
		/// </summary>
		public int getLowWatermark()
		{
			int result = 0;
			int code = NativeMethods.mamaQueue_getLowWatermark(nativeHandle, ref result);
			CheckResultCode(code);
			return result;
		}

		public int getEventCount()
		{
			int result = 0;
			int code = NativeMethods.mamaQueue_getEventCount(nativeHandle, ref result);
			CheckResultCode(code);
			return result;
		}

        /// <summary>
        /// Get the name of the bridge associated with the queue - either "wmw", "lbm", or
        /// "tibrv".
        /// </summary>
        public string getQueueBridgeName()
        {
            IntPtr ret = IntPtr.Zero;
            int code = NativeMethods.mamaQueue_getQueueBridgeName(nativeHandle, ref ret);
            CheckResultCode(code);
            return Marshal.PtrToStringAnsi(ret);
        }

		/// <summary>
		/// Specify a set of callbacks which may be invoked in response to certain
		/// conditions arising on the queue.
		/// </summary>
		/// <remarks>
		/// The behaviour here is middleware specific as not all will support all
		/// callbacks.
		/// <para>
		/// LBM: When the high watermark is exceeded the
		/// mamaQueueHighWatermarkExceededCb callback will invoked each time an event
		/// on the queue is dispatched until such time as the number of events on the
		/// queue falls below the high watermark.</para>
        /// <para>RV: Not currently supported.</para>
		/// </remarks>
		public void setQueueMonitorCallbacks(MamaQueueMonitorCallback callback, object closure)
		{
			EnsurePeerCreated();
			mWatermarkCallbackForwarder = new WatermarkCallbackForwarder(this, callback, closure);
			mWatermarkShimCallbacks.mHighWatermarkExceededShimCallback = new WatermarkCallbackForwarder.HighWatermarkExceededCallback(mWatermarkCallbackForwarder.HighWatermarkExceeded);
			mWatermarkShimCallbacks.mLowWatermarkShimCallback = new WatermarkCallbackForwarder.LowWatermarkCallback(mWatermarkCallbackForwarder.LowWatermark);
			int code = NativeMethods.mamaQueue_setQueueMonitorCallbacks(nativeHandle, ref mWatermarkShimCallbacks, IntPtr.Zero);
			CheckResultCode(code);
		}

		/// <summary>
        /// Register the specified callback function to receive a callback
        /// each time an event is enqueued on the specified MamaQueue
        /// </summary>
        public void setEnqueueCallback(MamaQueueEnqueueCallback callback) 
        {
			EnsurePeerCreated();
            mEnqueueCallbackForwarder = new EnqueueCallbackForwarder(this, callback);
            mEnqueueShimCallback = new EnqueueCallbackForwarder.EnqueueCallback(mEnqueueCallbackForwarder.OnEnqueue);
			int code = NativeMethods.mamaQueue_setEnqueueCallback(nativeHandle, mEnqueueShimCallback, IntPtr.Zero);
			CheckResultCode(code);
        }

		/// <summary>
        /// This function will enqueue an event on the mama queue.
        /// </summary>
        /// <param name="callback">
        /// This callback will be invoked by the thread pumping the queue.
        /// </param>
        /// <param name="closure">
        /// Utility object that will be passed back to the callback.
        /// </param>
        /// <exception cref="ArgumentNullException">
        /// Thrown if the callback object is null.
        /// </exception>
        public void enqueueEvent(MamaQueueEventCallback callback, object closure)
        {
            // Verify that the callback object has been supplied
            if(callback == null)
            {
                throw new ArgumentNullException("callback");
            }

            // Make sure the queue has been created
            EnsurePeerCreated();

            // Create a new forwarder object to manage the .Net callback objects
            int code = EnqueueEventForwarder.ForwardEvent(callback, closure, nativeHandle, this);

            // Chek that the return code
            CheckResultCode(code);
        }

        /// <summary>
        /// Check to see if a queue can be destroyed. The queue cannot be destroyed if there are
        /// currently open event objects on it.
        /// </summary>
        /// <returns>
        /// True if the queue can be destroyed.
        /// </returns>
        public bool canDestroy()
        {
            // Returns
            bool ret = false;

            // Verify that the underlying C object has been created
            EnsurePeerCreated();

            // Call the native method
            int mqcd = NativeMethods.mamaQueue_canDestroy(nativeHandle);

            // MAMA_STATUS_OK is returned by the C layer if the queue can be destroyed
            if ((int)MamaStatus.mamaStatus.MAMA_STATUS_OK == mqcd)
            {
                ret = true;
            }

            return ret;
        }

		/// <summary>
		/// Destroy a queue. Note that the queue can only be destroyed if all of the objects created
        /// on it, (timers, subscriptions etc), have been destroyed.
		/// </summary>
        /// <exception cref="MamaException">
        /// Thrown with a status code of MAMA_STATUS_QUEUE_OPEN_OBJECTS if there are still open objects
        /// against the queue.
        /// </exception>
		public void destroy()
		{
            // Verify that the native queue is valid
            EnsurePeerCreated();

            // If messages are being dispatched stop now
            if (status == QueueState.Running)
            {
                stopDispatch();
            }

            // Destroy the queue
            CheckResultCode(NativeMethods.mamaQueue_destroy(nativeHandle));

            // The native queue will not have been destroyed, reset the member variable
            nativeHandle = IntPtr.Zero;
		}

        /// <summary>
        /// Destroy a queue. Note that the queue can only be destroyed if all of the objects created
        /// on it, (timers, subscriptions etc), have been destroyed. This function will block until
        /// all of the objects have been destroyed and will then destroy the queue. 
        /// </summary>        
        public void destroyWait()
        {
            // Verify that the native queue is valid
            EnsurePeerCreated();

            // If messages are being dispatched stop now
            if (status == QueueState.Running)
            {
                stopDispatch();
            }

            // Destroy the queue
            CheckResultCode(NativeMethods.mamaQueue_destroyWait(nativeHandle));

            // The native queue will not have been destroyed, reset the member variable
            nativeHandle = IntPtr.Zero;
        }

        /// <summary>
        /// Destroy a queue. Note that the queue can only be destroyed if all of the objects created
        /// on it, (timers, subscriptions etc), have been destroyed. This function will block until
        /// all of the objects have been destroyed and will then destroy the queue. 
        /// </summary>
        /// <exception cref="MamaException">
        /// Thrown with a status code of MAMA_STATUS_TIMEOUT if the time period elapses before the
        /// queue can be shutdown.
        /// </exception>
        public void destroyTimedWait(long timeout)
        {
            // Verify that the native queue is valid
            EnsurePeerCreated();

            // If messages are being dispatched stop now
            if (status == QueueState.Running)
            {
                stopDispatch();
            }

            // Destroy the queue
            CheckResultCode(NativeMethods.mamaQueue_destroyTimedWait(nativeHandle, timeout));

            // The native queue will not have been destroyed, reset the member variable
            nativeHandle = IntPtr.Zero;
        }

        /// <summary>
        /// This function is called by the base class whenever the MamaQueue is disposed, it will attempt to
        /// destroy the queue if this has not already been done.
        /// <seealso cref="MamaWrapper.DestroyNativePeer"/>
        /// </summary>
        /// <returns>MAMA Status code</returns>
        protected override MamaStatus.mamaStatus DestroyNativePeer()
		{
            // Returns
            MamaStatus.mamaStatus ret = MamaStatus.mamaStatus.MAMA_STATUS_OK;

            // Only continue if the native queue is valid
            if (nativeHandle != IntPtr.Zero)
            {
                ret = (MamaStatus.mamaStatus)NativeMethods.mamaQueue_destroy(nativeHandle);
            }

            return ret;
		}

        /// <summary>
        /// Dispatch messages from the queue. This call blocks and dispatches
        /// until stopDispatch() is called.
        /// </summary>
        public void dispatch ()
        {
            status = QueueState.Running;
			EnsurePeerCreated();
			int code = NativeMethods.mamaQueue_dispatch(nativeHandle);
			CheckResultCode(code);
            status = QueueState.Stopped;
        }

        /// <summary>
        /// Dispatch a single event from the specified queue. If there is no event on 
        /// the queue simply return and do nothing.
        /// </summary>
        public void dispatchEvent ()
        {
            status = QueueState.Running;
			EnsurePeerCreated();
			int code = NativeMethods.mamaQueue_dispatchEvent(nativeHandle);
			CheckResultCode(code);
            status = QueueState.Stopped;
        }

        /// <summary>
        /// Unblock the queue as soon as possible. This will cause mamaDispatchers to
        /// exit. Creating a new dispatcher will resume dispatching events.
        /// </summary>
        public void stopDispatch()
        {
            status = QueueState.Stopping;
			EnsurePeerCreated();
			int code = NativeMethods.mamaQueue_stopDispatch(nativeHandle);
			CheckResultCode(code);
            while (status != QueueState.Stopped)
                Thread.Sleep(10);
        }
    
		/// <summary>
		/// Dispatch messages from the queue. This call blocks and dispatches
		/// until timeout has elapsed.
		/// </summary>
		public void timedDispatch(long timeout)
		{
            status = QueueState.Running;
			EnsurePeerCreated();
			int code = NativeMethods.mamaQueue_timedDispatch(nativeHandle, (ulong) timeout);
			CheckResultCode(code);
            status = QueueState.Stopped;
		}

		#region Implementation details

        #region EnqueueEventForwarder Class

        /// <summary>
        /// This class is used to forward messages from the enqueueEvent function to the
        /// instance of the MamaQueueCallback class that the client originally supplied.
        /// Note that multiple events may be enqueued before any of them are processed. To
        /// avoid the managed objects being collected references are stored in a Dictionary
        /// object. These are then removed whenever the relevant callback fires.
        /// </summary>
        private class EnqueueEventForwarder
        {
            #region Private Member Variables

            /// <summary>
            /// The callback object that will be invoked whenever an event is processed.
            /// </summary>
            private MamaQueueEventCallback mCallback;

            /// <summary>
            /// The closure.
            /// </summary>
            private object mClosure;

            /// <summary>
            /// The index of the forwarder in the static list. This is used to preserve a reference
            /// to the object while waiting for the native layer to invoke the callback.
            /// </summary>
            private long mIndex;

            /// <summary>
            /// The queue on which the event is being processed.
            /// </summary>
            private MamaQueue mSender;

            #endregion

            #region Private Static Member Variables

            /// <summary>
            /// List of all objects that have been enqueued, objects must be preserved to ensure that
            /// they are not garbage collected while waiting for the native layer to enqueue the event.
            /// </summary>                       
			private static Dictionary<long, EnqueueEventForwarder> mEventList;

            /// <summary>
            /// Mutex to controll access to the event list.
            /// </summary>
            private static Mutex mEventMutex;

            /// <summary>
            /// The callback function that will be passed to the native C function.
            /// </summary>
            private static EnqueueCallback mNativeCallback;

            /// <summary>
            /// The next key value to use.
            /// </summary>
            private static long mNextKey;

            #endregion

            #region Public Delegates

            /// <summary>
            /// This delegate is passed to the underlying C layer to act as the callback to
            /// mamaQueue_enqueueEvent.
            /// </summary>
            /// <param name="queue">
            /// The queue.
            /// </param>
            /// <param name="closure">
            /// Utility object passed to mamaQueue_enqueueEvent.
            /// </param>
            public delegate void EnqueueCallback(IntPtr queue, IntPtr closure);

            #endregion

            #region Construction and Finalization

            /// <summary>
            /// Static constructor initialises all static members.
            /// </summary>
            static EnqueueEventForwarder()
            {
                // Create the mutex
                mEventMutex = new Mutex(false);

				// Create the dictionary
                mEventList = new Dictionary<long, EnqueueEventForwarder>();

                // Create the native callback delegate
                mNativeCallback = new EnqueueCallback(EnqueueEventForwarder.OnEvent);

                // Reset the key value
                mNextKey = 0;
            }

            /// <summary>
            /// Constructor initialises all member variables.
            /// </summary>
            /// <param name="callback">
            /// The callback object that will be invoked whenever an event is processed.
            /// </param>            
            /// <param name="closure">
            /// Utility object.
            /// </param>
            /// <param name="sender">
            /// The queue.
            /// </param>
            public EnqueueEventForwarder(MamaQueueEventCallback callback, object closure, MamaQueue sender) 
			{
                // Save arguments in member variables.
                mCallback  = callback;
                mClosure   = closure;                
				mSender    = sender;

                // Save a reference to this object in the static list
                mIndex = AddForwarder(this);
            }

            #endregion

            #region Event Handlers
            
            /// <summary>
            /// This function is called by the underlying C layer whenever an event is processed.
            /// </summary>
            /// <param name="queue">
            /// The queue.
            /// </param>
            /// <param name="closure">
            /// Utility object passed to mamaQueue_enqueueEvent.
            /// </param>
            internal static void OnEvent(IntPtr queue, IntPtr closure)
			{
                // Get the forward object from the event list
                EnqueueEventForwarder forwarder = GetForwarder(closure.ToInt32());
                if(forwarder != null)
                {
                    // Only invoke the callback object if it is valid.
                    if(forwarder.mCallback != null)
                    {
                        /* Invoke the user supplied callback passing in the original closure, the one
                         * sent up from the C layer is ignored as it will not be a valid .Net object.
                         */
                        forwarder.mCallback.onEvent(forwarder.mSender, forwarder.mClosure);
                    }

                    // Remove the reference from the event list
                    RemoveForwarder(forwarder.mIndex);
                }
            }

            #endregion

            #region Private Static Operations

            private static long AddForwarder(EnqueueEventForwarder forwarder)
            {
                // Returns
                long ret = mNextKey;

                // Acquire the mutex
                mEventMutex.WaitOne();

                // Add the forwarder to the list
                mEventList.Add(mNextKey, forwarder);

                // Increment the next key index
                mNextKey ++;

                // Release the mutex
                mEventMutex.ReleaseMutex();
                                
                return ret;
            }

            private static EnqueueEventForwarder GetForwarder(long key)
            {
                // Returns
                EnqueueEventForwarder ret = null;

                // Acquire the mutex
                mEventMutex.WaitOne();

                // Obtain the forwarder from the dictionary
                ret = (EnqueueEventForwarder)mEventList[key];

                // Release the mutex
                mEventMutex.ReleaseMutex();

                return ret;
            }

            private static void RemoveForwarder(long key)
            {
                // Acquire the mutex
                mEventMutex.WaitOne();

                // Remove the forwarder from the array list
                mEventList.Remove(key);

                // Release the mutex
                mEventMutex.ReleaseMutex();
            }

            #endregion

            #region Public Operations

            /// <summary>
            /// Read only property returns the native callback delegate.
            /// </summary>
            public EnqueueCallback NativeCallback
            {
                get
                {
                    return mNativeCallback;
                }
            }

            #endregion

            #region Public Static Operations

            public static int ForwardEvent(MamaQueueEventCallback callback, object closure, IntPtr nativeHandle, MamaQueue sender)
            {
                // Create a new forwarder object to manage the .Net callback objects
                EnqueueEventForwarder forwarder = new EnqueueEventForwarder(callback, closure, sender);

                /* Invoke the native method, the index into the array will be passed as the clousure.
                 */
                return NativeMethods.mamaQueue_enqueueEvent(nativeHandle, forwarder.NativeCallback, new IntPtr(forwarder.mIndex));
            }

            #endregion
        }

        #endregion

		private class EnqueueCallbackForwarder
		{
			public delegate void EnqueueCallback(IntPtr queue, IntPtr closure);

			public EnqueueCallbackForwarder(MamaQueue sender, MamaQueueEnqueueCallback callback) 
			{
				mSender = sender;
				mCallback = callback;
			}

			internal void OnEnqueue(IntPtr queue, IntPtr closure)
			{
				if (mCallback != null) 
				{
					mCallback.onEnqueue(mSender);
				}
			}

			private MamaQueueEnqueueCallback mCallback;
			private MamaQueue mSender;
		}

		private class WatermarkCallbackForwarder
		{
			public delegate void HighWatermarkExceededCallback(IntPtr queue, int size, IntPtr closure);
			public delegate void LowWatermarkCallback(IntPtr queue, int size, IntPtr closure);

			public WatermarkCallbackForwarder(MamaQueue sender, MamaQueueMonitorCallback callback, object closure)
			{
				mSender = sender;
				mCallback = callback;
				mClosure = closure;
			}

			internal void HighWatermarkExceeded(IntPtr queue, int size, IntPtr closure)
			{
				if (mCallback != null)
				{
					mCallback.HighWatermarkExceeded(mSender, size, mClosure);
				}
			}

			internal void LowWatermark(IntPtr queue, int size, IntPtr closure)
			{
				if (mCallback != null)
				{
					mCallback.LowWatermark(mSender, size, mClosure);
				}
			}

			private MamaQueueMonitorCallback mCallback;
			private MamaQueue mSender;
			private object mClosure;
		}

		private struct NativeMethods
		{
			// export definitions
			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaQueue_create(ref IntPtr result,
				IntPtr bridgeImpl);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern int mamaQueue_canDestroy(IntPtr nativeHandle);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaQueue_destroy(IntPtr nativeHandle);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern int mamaQueue_destroyWait(IntPtr nativeHandle);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern int mamaQueue_destroyTimedWait(IntPtr nativeHandle, long timeout);
			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaQueue_setHighWatermark (IntPtr nativeHandle,
				int highWatermark);
			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaQueue_getHighWatermark (IntPtr nativeHandle,
				ref int highWatermark);
			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaQueue_setLowWatermark (IntPtr nativeHandle,
				int lowWatermark);
			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaQueue_getLowWatermark (IntPtr nativeHandle,
				ref int lowWatermark);
			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaQueue_getEventCount (IntPtr nativeHandle, ref int eventCount);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern int mamaQueue_getQueueBridgeName (IntPtr nativeHandle, ref IntPtr name);
			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaQueue_setQueueMonitorCallbacks (IntPtr nativeHandle,
				ref WatermarkCallbacks queueMonitorCallbacks,
				IntPtr closure);
			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaQueue_dispatch (IntPtr nativeHandle);
			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaQueue_dispatchEvent (IntPtr nativeHandle);
			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaQueue_stopDispatch (IntPtr nativeHandle);
			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaQueue_timedDispatch (IntPtr nativeHandle, 
				ulong timeout);
			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaQueue_setEnqueueCallback (IntPtr nativeHandle,
				EnqueueCallbackForwarder.EnqueueCallback callback,
				IntPtr  closure);

            /// <summary>
            /// Add an user event to a queue. Currently only supported using Wombat Middleware.
            /// </summary>
            /// <param name="nativeHandle">
            /// The queue to which the event is to be added
            /// </param>
            /// <param name="callback">
            /// The function to be invoked when the event fires.
            /// </param>
            /// <param name="closure">
            /// Optional arbitrary user supplied data. Passed back to callback function.
            /// </param>
            /// <returns>
            /// MAMA_STATUS_OK if the function succeeds.
            /// </returns>
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern int mamaQueue_enqueueEvent(
                IntPtr nativeHandle,
                EnqueueEventForwarder.EnqueueCallback callback,
                IntPtr closure);
		}

		// state
		private EnqueueCallbackForwarder mEnqueueCallbackForwarder;
		private EnqueueCallbackForwarder.EnqueueCallback mEnqueueShimCallback;

		[StructLayout(LayoutKind.Sequential)]
		private struct WatermarkCallbacks
		{
			public WatermarkCallbackForwarder.HighWatermarkExceededCallback mHighWatermarkExceededShimCallback;
			public WatermarkCallbackForwarder.LowWatermarkCallback mLowWatermarkShimCallback;
		}

		private WatermarkCallbackForwarder mWatermarkCallbackForwarder;
		private WatermarkCallbacks mWatermarkShimCallbacks;
		#endregion // Implementation details

	}
}
