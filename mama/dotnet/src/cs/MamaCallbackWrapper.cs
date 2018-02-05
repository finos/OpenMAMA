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

namespace Wombat
{
    /* ************************************************************** */
    #region MamaCallbackWrapper Class

    /// <summary>
    /// This class wraps a native callback function allowing the managed
    /// callback object, the closure and the delegate to be maintained while
    /// the code operates inside the native environment. Without this the
    /// managed object will be garbage collected resulting in a crash
    /// whenever the callback is eventually made from the native layer.
    /// </summary>
    internal class MamaCallbackWrapper<TCallback, TDelegate> : IDisposable
    {
        /* ************************************************************** */
        #region Private Member Variables

        /// <summary>
        /// This object will be invoked whenever the callback returns.
        /// </summary>
        private TCallback mCallback;

        /// <summary>
        /// The closure that will be passed back to the managed object whenever
        /// the callback comes in.
        /// </summary>
        private object mClosure;

        /// <summary>
        /// This delegate is passed to the native function.
        /// </summary>
        private TDelegate mNativeDelegate;

        #endregion

        /* ************************************************************** */
        #region Construction and Finalization

        /// <summary>
        /// Default constructor doesn't supply a callback reference.
        /// </summary>
        internal MamaCallbackWrapper()
            : this(default(TCallback), null, default(TDelegate))
        {
        }

        /// <summary>
        /// This constructor initialises the class with a log file callback
        /// interface.
        /// </summary>
        /// <param name="callback">
        /// This object will be invoked whenever the callback returns.
        /// </param>
        /// <param name="closure">
        /// The closure that will be passed back to the managed object whenever
        /// the callback comes in.
        /// </param>
        /// <param name="nativeDelegate">
        /// This delegate is passed to the native function.
        /// </param>
        internal MamaCallbackWrapper(TCallback callback, object closure, TDelegate nativeDelegate)
        {
            // Save arguments in member variables
            mCallback          = callback;
            mClosure           = closure;
            mNativeDelegate    = nativeDelegate;
        }

        /// <summary>
        /// The finalizer will stop the native callbacks coming to this class.
        /// </summary>
        ~MamaCallbackWrapper()
        {
            // Dispose only unmanaged resources
            Dispose(false);
        }

        #endregion

        /* ************************************************************** */
        #region Interface Implementation

        /// <summary>
        /// Disposes all resources held by the class.
        /// </summary>
        void IDisposable.Dispose()
        {
            // Call the overload disposing both managed and unmanaged resources
            Dispose(true);

            // We no longer require the finalizer
            GC.SuppressFinalize(this);
        }

        #endregion

        /* ************************************************************** */
        #region Operations

        /// <summary>
        /// This private function will actually dispose the resources held by the class.
        /// </summary>
        /// <param name="disposing">
        /// True to dispose both managed and unmanaged resources, false to dispose only
        /// unmanaged resources. Only pass false when calling from within the finalizer.
        /// </param>
        private void Dispose(bool disposing)
        {
            // Check whether both managed and unmanaged should be disposed
            if (disposing)
            {
                // Dispose managed resources
                mCallback   = default(TCallback);
                mClosure    = null;
            }

            // Dispose all unmanaged resources
            mNativeDelegate = default(TDelegate);
        }

        /// <summary>
        /// Read / write property accesses the callback object.
        /// </summary>
        internal TCallback Callback
        {
            get
            {
                return mCallback;
            }

            set
            {
                mCallback = value;
            }
        }

        /// <summary>
        /// Read / write property accesses the closure.
        /// </summary>
        internal object Closure
        {
            get
            {
                return mClosure;
            }

            set
            {
                mClosure = value;
            }
        }

        /// <summary>
        /// Read / write property accesses the native delegate.
        /// </summary>
        internal TDelegate NativeDelegate
        {
            get
            {
                return mNativeDelegate;
            }

            set
            {
                mNativeDelegate = value;
            }
        }

        #endregion
    }

    #endregion
}
