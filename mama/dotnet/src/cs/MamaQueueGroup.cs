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
using System.Threading;

namespace Wombat
{
    /* ************************************************************** */
    #region MamaQueueGroup Class

    /// <summary>
	/// A class for distributing events across multiple queues
    /// in a round robin fashion.
	/// </summary>
	public class MamaQueueGroup : IDisposable
    {
        /* ************************************************************** */
        #region Class Member Definition

        /* ************************************************************** */
        #region Private Member Variables

        /// <summary>
        /// This is used to track the next queue.
        /// </summary>
        private volatile int mNextQueueIndex;

        /// <summary>
        /// Array of queues contained in this group.
        /// </summary>
        private MamaQueueThread[] mQueueThreads;

        #endregion

        /* ************************************************************** */
        #region Nested Classes

        /// <summary>
        /// This class provides a MAMA queue as well as a thread to process it.
        /// </summary>
        private class MamaQueueThread : IDisposable
        {
            /* ************************************************************** */
            #region Class Member Definitino

            /* ************************************************************** */
            #region Private Member Variables

            /// <summary>
            /// The queue that the thread will be processing.
            /// </summary>
            private MamaQueue mQueue;

            /// <summary>
            /// The thread doing the work.
            /// </summary>
            private Thread mWorkerThread;

            #endregion

            #endregion

            /* ************************************************************** */
            #region Construction and Finalization

            /// <summary>
            /// Construct initialises all member variables including createing the
            /// thread.
            /// </summary>
            /// <param name="bridge">
            /// The MAMA bridge.
            /// </param>
            public MamaQueueThread(MamaBridge bridge)
            {
                // Create the queue
                mQueue = new MamaQueue(bridge);

                // Start dispatching straight away
                start();
            }

            /// <summary>
            /// Finalizer disposes all unmanaged resources.
            /// </summary>
            ~MamaQueueThread()
            {
                // Dispose only unmanaged resources
                Dispose(false);
            }

            #endregion

            /* ************************************************************** */
            #region Internal Operations

            /// <summary>
            /// Destroy while waiting for the queue to shutdown complete, this is equivalent to disposing
            /// the class.
            /// </summary>
            internal void destroyWait()
            {
                // Stop dispatching the queue, this will not return until the thread has shut down
                stop();

                // Destroy the queue, but wait until it has shutdown
                mQueue.destroyWait();
                mQueue = null;

                // Supress the finalizer as we have effectively disposed the class
                GC.SuppressFinalize(this);
            }

            /// <summary>
            /// Read only property returns the queue.
            /// </summary>
            internal MamaQueue Queue
            {
                get
                {
                    return mQueue;
                }
            }

            /// <summary>
            /// Start dispatching on the queue, this creates a new worker thread.
            /// </summary>
            internal void start()
            {
                // Only continue if the thread is not valid
                if (null == mWorkerThread)
                {
                    // Create the thread
                    mWorkerThread = new Thread(new ThreadStart(this.workerThreadProc));

                    // It will run in the background
                    mWorkerThread.IsBackground = true;

                    // Format the name with the queue hash code
                    mWorkerThread.Name = string.Format("MamaQueueManager: {0}", mQueue.GetHashCode());

                    // Start the thread
                    mWorkerThread.Start();
                }
            }

            /// <summary>
            /// Stop dispatching on the queue, this will cause the worker thread to shutdown.
            /// </summary>
            internal void stop()
            {
                // Only continue if there is a thread dispatching the queue
                if (null != mWorkerThread)
                {
                    // Stop dispatching on the queue, this will release the worker thread
                    mQueue.stopDispatch();

                    // Wait until the thread exits
                    mWorkerThread.Join();

                    // Clear the member variable so the thread can be recreated if dispatching is restarted
                    mWorkerThread = null;
                }
            }

            #endregion

            /* ************************************************************** */
            #region Private Operations

            /// <summary>
            /// Dispose(bool disposing) executes in two distinct scenarios.
            /// If disposing equals true, the method has been called directly
            /// or indirectly by a user's code. Managed and unmanaged resources
            /// can be disposed.
            /// If disposing equals false, the method has been called by the
            /// runtime from inside the finalizer and you should not reference
            /// other objects. Only unmanaged resources can be disposed.
            /// </summary>
            /// <param name="disposing">
            /// True if managed resources can be disposed.
            /// </param>
            private void Dispose(bool disposing)
            {
                // Dispose managed objects
                if (disposing)
                {
                    // Stop dispatching the queue, this will not return until the thread has shut down
                    stop();

                    // Destroy the queue
                    mQueue.Dispose();
                    mQueue = null;
                }
            }

            /// <summary>
            /// This is the worker thread procedure that dispatches events from the queue.
            /// </summary>
            private void workerThreadProc()
            {
                // Start dispatching on the queue
                mQueue.dispatch();
            }

            #endregion

            /* ************************************************************** */
            #region Public Operations

            /// <summary>
            /// Disposable implementation destroys the class and kills the queue without waiting.
            /// </summary>
            public void Dispose()
            {
                // Dispose managed and unmanaged resources
                Dispose(true);

                // This object will be cleaned up by the Dispose method.
                // Therefore, you should call GC.SupressFinalize to
                // take this object off the finalization queue
                // and prevent finalization code for this object
                // from executing a second time.
                GC.SuppressFinalize(this);
            }

            #endregion
        }

        #endregion

        #endregion

        /* ************************************************************** */
        #region Construction and Finalization

        /// <summary>
        /// Construct a queue manager with given number of queues/threads
        /// </summary>
		/// <param name="bridgeImpl">
        /// The mama bridge.
        /// </param>
		/// <param name="queueCount">
        /// The number of queues in the group.
        /// </param>
		public MamaQueueGroup(MamaBridge bridgeImpl, int queueCount)
		{
            // Check arguments
            if (queueCount < 1)
            {
                throw new ArgumentOutOfRangeException("queueCount", queueCount, "Queue count should be > 0");
            }

            // Create the array of queue threads
            mQueueThreads = new MamaQueueThread[queueCount];
            for (int nextQueue = 0; nextQueue<queueCount; nextQueue++)
            {
                mQueueThreads[nextQueue] = new MamaQueueThread(bridgeImpl);
            }
		}

        /// <summary>
        /// Finalizer disposes all unmanaged resources.
        /// </summary>
        ~MamaQueueGroup()
        {
            // Dispose only unmanaged resources
            Dispose(false);
        }

        #endregion

        /* ************************************************************** */
        #region Private Operations

        /// <summary>
        /// Dispose(bool disposing) executes in two distinct scenarios.
        /// If disposing equals true, the method has been called directly
        /// or indirectly by a user's code. Managed and unmanaged resources
        /// can be disposed.
        /// If disposing equals false, the method has been called by the
        /// runtime from inside the finalizer and you should not reference
        /// other objects. Only unmanaged resources can be disposed.
        /// </summary>
        /// <param name="disposing">
        /// True if managed resources can be disposed.
        /// </param>
        private void Dispose(bool disposing)
        {
            // If disposing is true then dispose all managed resources
            if (disposing)
            {
                // Lock to prevent thread race conditions
                lock (this)
                {
                    // Enumerate all the threads
                    foreach (MamaQueueThread queueThread in mQueueThreads)
                    {
                        // Dispose the queue
                        queueThread.Dispose();
                    }

                    // Clear the array
                    mQueueThreads = null;
                }
            }

            // Clean up unamanged resources regardless
        }

        #endregion

        /* ************************************************************** */
        #region Public Operations

        /// <summary>
        /// Destroy the queue group and destroy all the queues without waiting, this is equivalent
        /// to disposing the class.
        /// </summary>
        public void destroy()
        {
            Dispose();
        }

        /// <summary>
        /// Destroy the queue group and destroy all the queues waiting on each queue, this is
        /// equivalent to disposing the class.
        /// </summary>
        public void destroyWait()
        {
            // Lock to prevent race conditions
            lock (this)
            {
                // Destroy all the queue threads
                foreach (MamaQueueThread queueThread in mQueueThreads)
                {
                    queueThread.destroyWait();
                }

                // Clear the array
                mQueueThreads = null;
            }

            // The class has been disposed, prevent any further finalization
            GC.SuppressFinalize(this);
        }

        /// <summary>
        /// Dispose managed and unmanaged resources. This will destroy all the queues
        /// without waiting for them to shutdown.
        /// </summary>
        public void Dispose()
        {
            // Dispose managed and unmanaged resources
            Dispose(true);

            // This object will be cleaned up by the Dispose method.
            // Therefore, you should call GC.SupressFinalize to
            // take this object off the finalization queue
            // and prevent finalization code for this object
            // from executing a second time.
            GC.SuppressFinalize(this);
        }

        /// <summary>
        /// Start dispatching on all queues.
        /// </summary>
        public void start()
        {
            // Lock to prevent race conditions
            lock (this)
            {
                // Start all the queue threads
                foreach (MamaQueueThread queueThread in mQueueThreads)
                {
                    queueThread.start();
                }
            }
        }

        /// <summary>
        /// Stop dispatching on all queues.
        /// </summary>
        public void stop()
        {
            // Lock to prevent race conditions
            lock (this)
            {
                // Stop all the queue threads
                foreach (MamaQueueThread queueThread in mQueueThreads)
                {
                    queueThread.stop();
                }
            }
        }

        /// <summary>
        /// This function will return the next queue since it was called last or the first queue in
        /// the group if this is the first time that it has been called.
        /// Note that when all queues have been returned it will go back to the start of the array.
        /// </summary>
        public MamaQueue getNextQueue()
        {
            // Returns
            MamaQueue ret = null;

            lock(this)
            {
                // Only continue if the array of threads is valid
                if (mQueueThreads != null)
                {
                    // If the queue index has reached the end of the array then go back to the start
                    if (mNextQueueIndex >= mQueueThreads.Length)
                    {
                        mNextQueueIndex = 0;
                    }

                    // Get the queue at this index and increment.
                    ret = mQueueThreads[mNextQueueIndex++].Queue;
                }
            }

            return ret;
        }

        #endregion
    }

    #endregion
}
