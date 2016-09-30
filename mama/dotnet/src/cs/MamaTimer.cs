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
    #region MamaTimer Class

    /// <summary>
	/// A repeating timer
	/// </summary>
	public class MamaTimer : MamaWrapper
    {
        /* ************************************************************** */
        #region Class Member Definition

        /* ************************************************************** */
        #region Native Method Prototypes

        // Interop API calls
        private struct NativeMethods
        {
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern int mamaTimer_create2(
                ref IntPtr result,
                IntPtr queue,
                MamaTimer.OnTimerTickDelegate tickCallback,
                MamaTimer.OnTimerTickDelegate destroyCallback,
                double interval,
                IntPtr closure);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern int mamaTimer_destroy(IntPtr timer);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern int mamaTimer_reset(IntPtr timer);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern int mamaTimer_setInterval(IntPtr timer, double interval);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern int mamaTimer_getInterval(IntPtr timer, ref double interval);
        }

        #endregion

        /* ************************************************************** */
        #region Private Classes

        /// <summary>
        /// This impl will perform the work of invoking user supplied callbacks, it is
        /// held to ensure that the user can destroy and recreate the parent timer during
        /// the callbacks.
        /// </summary>
        private class MamaTimerImpl
        {
            /* ************************************************************** */
            #region Private Member Variables

            /// <summary>
            /// The callback interface.
            /// </summary>
            private MamaTimerCallback mCallback;

            /// <summary>
            /// The closure provided by the user.
            /// </summary>
            private object mClosure;

            /// <summary>
            /// The actual timer object that will be provided in the callbacks.
            /// </summary>
            private MamaTimer mTimer;

            #endregion

            /* ************************************************************** */
            #region Construction and Finalization

            /// <summary>
            /// Constructor initialises all member variables.
            /// </summary>
            /// <param name="callback">
            /// The user callback implementation
            /// </param>
            /// <param name="closure">
            /// The closure supplied to the MamaTimer.create function.
            /// </param>
            /// <param name="timer">
            /// The actual C# timer object.
            /// </param>
            internal MamaTimerImpl(MamaTimerCallback callback, object closure, MamaTimer timer)
            {
                // Save arguments in member variables
                mCallback  = callback;
                mClosure   = closure;
                mTimer     = timer;
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
            /// The closure supplied to the MamaTimer.create function.
            /// </param>
            /// <param name="timer">
            /// The actual C# timer object.
            /// </param>
            /// <returns>
            /// The IntPtr that can then be used for the closure.
            /// </returns>
            internal static IntPtr Create(MamaTimerCallback callback, object closure, MamaTimer timer)
            {
                // Allocate a new impl
                MamaTimerImpl impl = new MamaTimerImpl(callback, closure, timer);

                // Create a GC handle
                GCHandle handle = GCHandle.Alloc(impl);

                // Return the native pointer
                return (IntPtr)handle;
            }

            /// <summary>
            /// This function will invoke the destroy callback
            /// </summary>
            internal void InvokeDestroy()
            {
                if(null != mCallback)
                {
                    mCallback.onDestroy(mTimer, mClosure);
                }
            }

            /// <summary>
            /// This function will invoke the tick callback on the user supplied callback implementation.
            /// </summary>
            internal void InvokeTick()
            {
                // Only invoke the callback if it is supplied
                if(null != mCallback)
                {
                    mCallback.onTimer(mTimer, mClosure);
                }
            }

            #endregion
        }

        #endregion

        /* ************************************************************** */
        #region Private Delegates

        /// <summary>
        /// This delegate describes the native callback functions invoked for timer tick and destroy
        /// events.
        /// </summary>
        /// <param name="timer">
        /// The native timer object.
        /// </param>
        /// <param name="closure">
        /// The closure passed to the timer create function.
        /// </param>
        public delegate void OnTimerTickDelegate(IntPtr timer, IntPtr closure);

        #endregion

        /* ************************************************************** */
        #region Private Static Member Variables

        /// <summary>
        /// This delegate is used to invoke the destroy callback.
        /// </summary>
        private static OnTimerTickDelegate mDestroyDelegate = new OnTimerTickDelegate(MamaTimer.onTimerDestroy);

        /// <summary>
        /// This delegate is used to invoke the tick callback.
        /// </summary>
        private static OnTimerTickDelegate mTickDelegate = new OnTimerTickDelegate(MamaTimer.onTimerTick);

        #endregion

        #endregion

        /* ************************************************************** */
        #region Construction and Finalization

        /// <summary>
        /// Construct a timer object.
        /// </summary>
        public MamaTimer() : base()
        {
        }

        /// <summary>
        /// Construct a timer object using a precreated native timer.
        /// </summary>
        /// <param name="nativeHandle">
        /// The native timer handle.
        /// </param>
        public MamaTimer(IntPtr nativeHandle) : base(nativeHandle)
        {
        }

        #endregion

        /* ************************************************************** */
        #region Private Static Functions

        /// <summary>
        /// This event handler is called by the native layer whenever the timer has been fully destroyed.
        /// It will perform all clean-up and then invoke the onDestroy callback function if this has
        /// been supplied.
        /// </summary>
        /// <param name="timer">
        /// The native timer.
        /// </param>
        /// <param name="closure">
        /// The closure passed down to the native layer.
        /// </param>
        private static void onTimerDestroy(IntPtr timer, IntPtr closure)
        {
            // Obtain the handle from the closure
            GCHandle handle = (GCHandle)closure;

            // Extract the impl from the handle
            MamaTimerImpl impl = (MamaTimerImpl)handle.Target;

            // Use the impl to invoke the destroy callback, (if this has been supplied)
            impl.InvokeDestroy();

            /* The timer has now been destroyed and the impl is no longer required, free the handle to
             * allow the garbage collector to clean it up.
             */
            handle.Free();
        }

        /// <summary>
        /// This event handler is called whenever the timer ticks and will simply cause the appropriate
        /// callback function to be invoked.
        /// </summary>
        /// <param name="timer">
        /// The native timer.
        /// </param>
        /// <param name="closure">
        /// The closure passed down to the native layer.
        /// </param>
        private static void onTimerTick(IntPtr timer, IntPtr closure)
        {
            // Obtain the handle from the closure
            GCHandle handle = (GCHandle)closure;

            // Extract the impl from the handle
            MamaTimerImpl impl = (MamaTimerImpl)handle.Target;

            // Use the impl to invoke the tick callback
            impl.InvokeTick();
        }

        #endregion

        /* ************************************************************** */
        #region Protected Functions

        /// <summary>
        /// This will be called by the base class to destroy the native timer.
        /// <seealso cref="MamaWrapper.DestroyNativePeer"/>
        /// </summary>
        /// <returns>
        /// MAMA Status code
        /// </returns>
        protected override MamaStatus.mamaStatus DestroyNativePeer()
        {
            return (MamaStatus.mamaStatus)NativeMethods.mamaTimer_destroy(NativeHandle);
        }

        #endregion

        /* ************************************************************** */
        #region Public Functions

        /// <summary>
        /// Create a repeating timer. Since the MamaTimer relies on the timer mechanism of the
        /// underlying middleware, the resolution of the timer is also dependent on the
        /// middleware. Consult your middleware documentation for details.
        /// The callback is invoked repeatedly at the specified interval until the timer
        /// is destroyed.
        /// A null value for the queue uses the default mama queue.
        /// </summary>
        /// <param name="queue">
        /// The queue from which the timer event will be dispatched.
        /// </param>
        /// <param name="action">
        /// The callback to be invoked after the interval
        /// </param>
        /// <param name="interval">
        /// The interval in seconds.
        /// </param>
        public void create(MamaQueue queue, MamaTimerCallback action, double interval)
        {
            // Call the overload with a NULL closure
            this.create(queue, action, interval, null);
        }

        /// <summary>
        /// Create a repeating timer. Since the MamaTimer relies on the timer mechanism of the
        /// underlying middleware, the resolution of the timer is also dependent on the
        /// middleware. Consult your middleware documentation for details.
        /// The callback is invoked repeatedly at the specified interval until the timer
        /// is destroyed.
        /// A null value for the queue uses the default mama queue.
        /// </summary>
        /// <param name="queue">
        /// The queue from which the timer event will be dispatched.
        /// </param>
        /// <param name="action">
        /// The callback to be invoked after the interval
        /// </param>
        /// <param name="interval">
        /// The interval in seconds.
        /// </param>
        /// <param name="closure">
        /// Closure data for timer.
        /// </param>
        public void create(MamaQueue queue, MamaTimerCallback action, double interval, object closure)
        {
            // Check the arguments
            if (null == queue)
            {
                throw new ArgumentNullException("queue");
            }

            if (null == action)
            {
                throw new ArgumentNullException("action");
            }

            // Create the impl
            IntPtr impl = MamaTimerImpl.Create(action, closure, this);

            /* Create the timer, register for the destroy callback regardless if the client wants it or not,
             * this is to allow clean-up to be done whenever the timer has been fully destroyed.
             */
            IntPtr nativeTimer = IntPtr.Zero;
            CheckResultCode(NativeMethods.mamaTimer_create2(ref nativeTimer, queue.NativeHandle, mTickDelegate, mDestroyDelegate, interval, impl));

            // Save the native timer in the member variable
            NativeHandle = nativeTimer;
        }

        /// <summary>
        /// Destroy the timer.
        /// This function must be called from the same thread dispatching on the
        /// associated event queue unless both the default queue and dispatch queue are
        /// not actively dispatching.
        /// This is functionally equivalent to disposing the instance.
        /// </summary>
        public void destroy()
        {
            // Dispose, note that the cleanup will not be done until the destroy callback returns
            Dispose();
        }

        /// <summary>
        /// Get the current timer interval.
        /// </summary>
        /// <returns>
        /// The interval in seconds.
        /// </returns>
        public double getInterval()
        {
            // Returns
            double ret = 0;

            // Verify that the C timer has been created
            EnsurePeerCreated();

            // Call the native method to get the interval
            CheckResultCode(NativeMethods.mamaTimer_getInterval(NativeHandle, ref ret));

            return ret;
        }

        /// <summary>
        /// Reset the timer to the beginning of the interval.
        /// </summary>
        public void reset()
        {
            // Verify that the C timer has been created
			EnsurePeerCreated();

            // Call the native method
            CheckResultCode(NativeMethods.mamaTimer_reset(NativeHandle));
        }

        /// <summary>
        /// Set the timer to use a different interval (and reset to the
        /// beginning of that interval).
        /// </summary>
        /// <param name="interval">
        /// The interval in settings.
        /// </param>
        public void setInterval(double interval)
        {
            // Verify that the C timer has been created
			EnsurePeerCreated();

            // Call the native method to set the interval
            CheckResultCode(NativeMethods.mamaTimer_setInterval(NativeHandle, interval));
        }

        #endregion
    }

    #endregion
}
