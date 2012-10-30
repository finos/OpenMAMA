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
	/// <summary>
	/// Summary description for MamaQueueManager.
	/// </summary>
	public class MamaQueueManager
	{
        private MamaQueueThread[] threads = null;
        private volatile int threadIndex = 0;
        private static volatile int shutdownTimeout = 2000; // 2 sec
        
        /// <summary>
        /// we run a separate thread per queue
        /// </summary>
        private class MamaQueueThread : MamaQueueCallback
        {
            private MamaQueue queue = null;
            private Thread workerThread;
            protected volatile bool RequestToStop = false;
            
            /// <summary>
            /// Construct and start a thread for the given queue
            /// </summary>
            /// <param name="queue"></param>
            public MamaQueueThread(MamaQueue queue) 
            {
                this.queue = queue;
                
                queue.setEnqueueCallback(this);

                workerThread = new Thread(new ThreadStart(this.run));
                workerThread.Name = "MamaQueueManager:"+queue.GetHashCode();
                workerThread.Start();
            }
            
            /// <summary>
            /// Man thread loop
            /// </summary>
            public void run()
            {
                while (!RequestToStop)
                {
                    lock(queue)
                    {
                        Monitor.Wait(queue);

                        queue.dispatchEvent();
                    }

                }
                
                lock(this) 
                {
                    Monitor.Pulse(this);
                }

            }

            /// <summary>
            /// Return the queue for this thread
            /// </summary>
            /// <returns></returns>
            public MamaQueue getQueue() 
            {
                return queue;
            }
            /// <summary>
            /// Destroy this object
            /// </summary>
            public void destroy()
            {   
                lock (this) 
                {
                    if (queue != null) 
                    {
                        RequestToStop = true;
                        lock (queue)
                        {
                            Monitor.Pulse(queue);
                        }

                        if (!Monitor.Wait(this, shutdownTimeout )) 
                        {
                            workerThread.Abort();
                            workerThread.Join();
                        }
                    }

                    queue.destroy();
                    queue = null;
                }
            }
            
            ~MamaQueueThread()
            {
                destroy();
            }


            public void Enqueue(MamaQueue mamaQueue, object closure)
            {
                lock (queue)
                {
                    Monitor.Pulse(queue);
                }
            }
        }
        
        /// <summary>
        /// Construct a queue manager with given number of queues/threads
        /// </summary>
        /// <param name="queueCount"></param>
        /// <param name="bridgeImpl"></param>
		public MamaQueueManager(MamaBridge bridgeImpl, int queueCount)
		{
            if (queueCount < 1) 
            {
                throw new Exception("Queue count should be > 0");
            }
		    threads = new MamaQueueThread[queueCount];
            for (int i = 0; i < threads.Length; i++) 
            {
                threads[i] = new MamaQueueThread(new MamaQueue(bridgeImpl));
            }
		}
        
        /// <summary>
        /// Destroy this object
        /// </summary>
        public void destroy()
        {
            lock(this)
            {
                if (threads != null) 
                {
                    for (int i = 0; i < threads.Length; i++) 
                    {
                        threads[i].destroy();
                    }

                    threads = null;
                }
            }
        }
        
        /// <summary>
        /// Iterate over the available queues
        /// </summary>
        /// <returns></returns>
        public MamaQueue getNextQueue() 
        {
            lock(this) 
            {
                if (threads != null) 
                {
                    if (threadIndex >= threads.Length) 
                    {
                        threadIndex = 0;
                    }

                    return threads[threadIndex].getQueue();
                }

                return null;
            }
        }
        
        /// <summary>
        /// Set the the maximum shutdown timeout (for each thread/queue)
        /// </summary>
        public static void setShutdownTimeout(int shutdownTimeout)
        {
            MamaQueueManager.shutdownTimeout = shutdownTimeout;
        }

        ~MamaQueueManager()
        {
            destroy();
        }


	}
}
