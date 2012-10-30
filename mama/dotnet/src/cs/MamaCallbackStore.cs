/* $Id: MamaCallbackStore.cs,v 1.1.6.5 2012/08/24 16:12:01 clintonmcdowell Exp $
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
using System.Collections.Generic;
using System.Threading;

namespace Wombat
{
    /* ************************************************************** */
    #region MamaCallbackStore Class

    /// <summary>
    /// This class is used to store MamaCallbackWrapper objects, it provides synchronized
    /// access so can be used from any thread.
    /// </summary>
    internal class MamaCallbackStore<TCallback, TDelegate> : IDisposable
    {
        /* ************************************************************** */
        #region Private Member Variables

        /// <summary>
        /// The next available Id for storing callbacks in the store.
        /// </summary>
        private long mNextId;

        /// <summary>
        /// This list will maintain the set of wrappers that will be stored while waiting
        /// for a callback.
        /// </summary>
        private Dictionary<long, MamaCallbackWrapper<TCallback, TDelegate>> mStore;

        /// <summary>
        /// This mutex is used to protected access to the store.
        /// </summary>
        private Mutex mStoreMutex;

        #endregion

        /* ************************************************************** */
        #region Construction and Finalization

        /// <summary>
        /// Default constructor initialises all member variables.
        /// </summary>
        internal MamaCallbackStore()            
        {
            // Create the store
            mStore = new Dictionary<long, MamaCallbackWrapper<TCallback, TDelegate>>();

            // Create the mutex
            mStoreMutex = new Mutex(false);
        }

        /// <summary>
        /// The finalizer will stop the native callbacks coming to this class.
        /// </summary>
        ~MamaCallbackStore()
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
                // Dispose the store
                if (mStore != null)
                {
                    // Dispose all of the objects still in the store
                    foreach (MamaCallbackWrapper<TCallback, TDelegate> wrapper in mStore.Values)
                    {
                        // Get a dispose interface
                        IDisposable disposable = (IDisposable)wrapper;
                        disposable.Dispose();
                    }

                    // Clear the store                    
                    mStore.Clear();
                    mStore = null;
                }

                // Dispose the mutex
                if (mStoreMutex != null)
                {
                    IDisposable disposable = (IDisposable)mStoreMutex;
                    disposable.Dispose();
                    mStoreMutex = null;
                }
            }
        }

        /// <summary>
        /// This function will remove a wrapper object from the store and return it
        /// to the caller. This object should then be disposed by the caller.
        /// </summary>
        /// <param name="handle">
        /// Handle returned from the StoreWrapper function.
        /// </param>
        /// <returns>
        /// The callback wrapper associated with the supplied handle.
        /// </returns>
        /// <exception cref="ArgumentOutOfRangeException">
        /// Thrown if the corresponding wrapper is not in the store.
        /// </exception>
        internal MamaCallbackWrapper<TCallback, TDelegate> RemoveWrapper(IntPtr handle)
        {
            // Returns
            MamaCallbackWrapper<TCallback, TDelegate> ret = null;

            // The object handle is the key
            long key = handle.ToInt64();

            // Acquire the mutex
            mStoreMutex.WaitOne();
            try
            {
                // Get the wrapper from the store
                ret = mStore[key];

                // Remove the object from the store
                mStore.Remove(key);
            }

            finally
            {
                // Release the mutex
                mStoreMutex.ReleaseMutex();
            }

            // Write a log
            if (MamaLogLevel.MAMA_LOG_LEVEL_FINEST == Mama.getLogLevel())
            {
                // Write details of the object that has been removed
                string message = string.Format("MamaCallbackStore: Wrapper removed for key {0} for callback {1}.", key, ret.Callback.ToString());
                Mama.log(MamaLogLevel.MAMA_LOG_LEVEL_FINEST, message);

                // Write the number of items
                message = string.Format("MamaCallbackStore: Store now contains {0} items.", mStore.Count);
                Mama.log(MamaLogLevel.MAMA_LOG_LEVEL_FINEST, message);
            }

            return ret;
        }

        /// <summary>
        /// This function will store the supplied wrapper and return an object
        /// that can be passed into the native layer as a closure.
        /// This should then be passed to the RemoveWrapper function to access
        /// the object whenever the native callback is invoked.
        /// </summary>
        /// <param name="wrapper">
        /// The wrapper object to store.
        /// </param>
        /// <returns>
        /// The handle that can be passed to the native layer.
        /// </returns>
        internal IntPtr StoreWrapper(MamaCallbackWrapper<TCallback, TDelegate> wrapper)
        {
            // Returns
            long ret = 0;

            // Acquire the mutex
            mStoreMutex.WaitOne();
            try
            {
                // Get the next Id
                mNextId ++;

                // This will become the handle
                ret = mNextId;

                // Add the wrapper to the store using the Id as the key
                mStore.Add(ret, wrapper);                
            }

            finally
            {
                // Release the mutex
                mStoreMutex.ReleaseMutex();
            }

            // Write a log
            if(MamaLogLevel.MAMA_LOG_LEVEL_FINEST == Mama.getLogLevel())
            {
                // Write details of the object that has bee added
                string message = string.Format("MamaCallbackStore: Wrapper stored for key {0} for callback {1}.", ret, wrapper.Callback.ToString());
                Mama.log(MamaLogLevel.MAMA_LOG_LEVEL_FINEST, message);

                // Write the number of items
                message = string.Format("MamaCallbackStore: Store now contains {0} items.", mStore.Count);
                Mama.log(MamaLogLevel.MAMA_LOG_LEVEL_FINEST, message);
            }

            /* Return the next Id as the handle. */
            return new IntPtr(ret);
        }

        #endregion
    }

    #endregion
}
