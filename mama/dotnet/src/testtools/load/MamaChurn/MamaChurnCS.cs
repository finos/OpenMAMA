/* $Id: MamaChurnCS.cs,v 1.1.2.1 2012/10/09 15:08:18 stuartbeattie Exp $
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
using System.IO;
using System.Collections;
using System.Threading;

using Wombat;



namespace MamaChurnCS
{
    /// <summary>
    /// MamaChurn
    /// </summary>
    class MamaChurnCS : MamaTransportCallback,
        MamaSubscriptionCallback,
        MamaDictionaryCallback,
        MamaMsgFieldIterator,
        MamaSymbolMapCallback,
        MamaQueueMonitorCallback,
        MamaTimerCallback
    {

        private static string[] m_usage =
        {
        "Subscribe to an arbitrary number of symbols and, every second, destroy and",
        "recreate a configurable number of subscriptions",
        "",
        "Usage: mamachurnc -S symbolNamespace -tport transport [options]",
        "",
        "Options:",
        "-c|-churn churnRate  churns per interval",
        "-f symbolFile        file containing a list of symbols to subscribe to",
        "-k|-kill lifetime    kill program after specified time (in seconds)",
        "-s symbol            symbol to subscribe to",
        "-S                   symbol name space",
        "-g                   create group subscription(s)",
        "-m                   The middleware to use [wmw/lbm/tibrv]. Default is wmw.",
        "-tport               name of the transport being used",
        "-t|-timeout seconds  churn interval",
        "-threads			  The number of threads to create",
        "-stats               stats interval",
        "-v                   increase verbosity (mama)",
        "-V                   increase verbosity (this program). Pass up to three times",
        null
        };


        const int MAXSYMBOLS = 5000;
        private string mamaMiddlewareName = "wmw";
        private string mamaTransportName = "rv1";
        private string mamaSourceName = "NASDAQ";
        private Random mamaRandom = new Random();
        private string[] mamaSymbols = new string[MAXSYMBOLS];
        private int mamaNumSymbols = 0;
        private string mamaDictionarySourceName = "WOMBAT";
        private long mamaThrottle = -1;
        private long mamaRecapThrottle = -1;
        private string mamaMapFilename = null;
        private int mamaThreads = 5;
        private int mamaHighWaterMark = 0;
        private int mamaLowWaterMark = 0;
        private MamaLogLevel mamaLogLevel = MamaLogLevel.MAMA_LOG_LEVEL_WARN;
        private int mamaQuietness = 0;
        private int quota = 10;

        // dictionary status
        private volatile bool dictionaryComplete = false;

        private MamaSymbolMapFile mamaSymbolMapFile;
        private MamaBridge mamaBridge;
        private MamaTransport mamaTransport;
        private MamaSubscription[] mamaSubscriptions = new MamaSubscription[MAXSYMBOLS];
        private MamaSource mamaDictionarySource;
        private MamaDictionary mamaDictionary;
        private MamaQueue mamaDefaultQueue;
        private MamaQueueGroup mamaQueueGroup;
        private MamaSource mamaSource;

        private StatsReport stats;

        /// <summary>
        /// The thread that will churn subscriptions, this thread creates a timer that will perform the
        /// churn when it ticks. The thread keeps creating timers.
        /// </summary>
        private Thread m_churnThread;

        /// <summary>
        /// The frequency in seconds at which the subscriptions will be churned.
        /// </summary>
        private double m_churnTimeout;

        /// <summary>
        /// This event is set when the program is about to shutdown and will kill the churn thread.
        /// </summary>
        private ManualResetEvent m_killEvent;

        /// <summary>
        /// The duration of the program in seconds.
        /// </summary>
        private double m_killTimeout;

        /// <summary>
        /// This class implements the timer callback used whenever the application is killed.
        /// </summary>
        private class KillTimerCallback : MamaTimerCallback
        {
            /// <summary>
            /// The main bridge.
            /// </summary>
            private MamaBridge m_bridge;

            internal KillTimerCallback(MamaBridge bridge)
            {
                // Save arguments in member variables
                m_bridge = bridge;
            }

            public void onTimer(MamaTimer mamaTimer, object closure)
            {
                // Destroy the timer
                mamaTimer.destroy();

                // Stop the bridge to unblock the main thread
                Mama.stop(m_bridge);
            }

            public void onDestroy(MamaTimer mamaTimer, object closure)
            {
            }
        }

        class StatsReport : MamaTimerCallback
        {
            MamaTimer statstimer;

            public int mCreateMsgCount;
            public int mInitialMsgCount;
            public int mOtherMsgCount;
            public int mErrorCount;
            public int mRecreates;

            public StatsReport(MamaQueue mamaDefaultQueue)
            {
                statstimer = new MamaTimer();

                statstimer.create(mamaDefaultQueue, this, 2, null);

                Console.WriteLine("    Time" + " {0, -4} {1, -4} {2, -4} {3, -4} {4, -2}",
                    "Create", "Inital", "OtherMsg", "Error", "Recreate");
            }

            public void destroy()
            {
                statstimer.destroy();
                statstimer = null;
            }

            public void onTimer(MamaTimer mamaTimer, object closure)
            {
                DateTime saveNow = DateTime.Now;

                Console.WriteLine(saveNow.ToLongTimeString() + "     {0, -6} {1, -6} {2, -6} {3, -6} {4, -6}",
                                    mCreateMsgCount, mInitialMsgCount, mOtherMsgCount, mErrorCount, mRecreates);

                mCreateMsgCount = 0;
                mInitialMsgCount = 0;
                mOtherMsgCount = 0;
                mErrorCount = 0;
                mRecreates = 0;
            }

            public void onDestroy(MamaTimer mamaTimer, object closure)
            {
            }

        }

        private void initializeMama()
        {
            mamaBridge = Mama.loadBridge(mamaMiddlewareName);

            Console.WriteLine(Mama.getVersion(mamaBridge));

            Mama.open();

            if (mamaHighWaterMark > 0 || mamaLowWaterMark > 0)
            {
                if (mamaHighWaterMark > 0)
                {
                    Mama.setDefaultQueueHighWatermark(mamaHighWaterMark);
                }

                if (mamaLowWaterMark > 0)
                {
                    try
                    {
                        Mama.setDefaultQueueLowWatermark(mamaLowWaterMark);
                    }
                    catch (Exception e)
                    {
                        Console.WriteLine("Could not set default queue low water mark MamaStatus: " + e);
                    }
                }
            }

            // create the transport
            mamaTransport = new MamaTransport();

            mamaTransport.setTransportCallback(this);

            // the default throttle rate is 500 msg/sec
            if (mamaThrottle != -1)
            {
                mamaTransport.setOutboundThrottle(MamaTransport.MamaThrottleInstance.MAMA_THROTTLE_DEFAULT, mamaThrottle);
            }

            // the default recap throttle rate is 250 msg/sec
            if (mamaRecapThrottle != -1)
            {
                mamaTransport.setOutboundThrottle(MamaTransport.MamaThrottleInstance.MAMA_THROTTLE_RECAP, mamaRecapThrottle);
            }

            mamaTransport.create(mamaTransportName, mamaBridge);

            // create default queue and, if required, queue group
            createQueues();

            mamaDictionarySource = new MamaSource();
            mamaDictionarySource.symbolNamespace = mamaDictionarySourceName;
            mamaDictionarySource.transport = mamaTransport;

            // download dictionary
            mamaDictionary = new MamaDictionary();

            mamaDictionary.create(
                mamaDefaultQueue,
                this,
                mamaDictionarySource,
                3,
                10.0f);

            loadSymbolMap();

            Mama.start(mamaBridge);

            if (!dictionaryComplete)
            {
                throw new Exception("Can't create dictionary.");
            }

        }

        public int SymbolMapFunc(
            object closure,
            ref string result,
            string symbol,
            int maxLen)
        {
            return mamaSymbolMapFile.map(ref result, symbol, maxLen);
        }

        private void loadSymbolMap()
        {
            if (mamaMapFilename != null)
            {
                mamaSymbolMapFile = new MamaSymbolMapFile();
                mamaSymbolMapFile.create();
                mamaSymbolMapFile.load(mamaMapFilename);
                mamaTransport.setSymbolMapFunc(this, null);
            }
        }

        private void createQueues()
        {
            mamaDefaultQueue = Mama.getDefaultEventQueue(mamaBridge);

            mamaQueueGroup = new MamaQueueGroup(mamaBridge, mamaThreads);

            if (mamaThreads > 0)
            {
                // Has queue monitoring been enabled?
                if (mamaHighWaterMark > 0 || mamaLowWaterMark > 0)
                {
                    for (int index = 0; index < mamaThreads; index++)
                    {
                        MamaQueue queue = mamaQueueGroup.getNextQueue();
                        string queueName = "QUEUE " + index;
                        Console.WriteLine("Setting monitor for " + queueName);
                        queue.setQueueMonitorCallbacks(this, queueName);
                        if (mamaHighWaterMark > 0)
                        {
                            queue.setHighWatermark(mamaHighWaterMark);
                        }

                        if (mamaLowWaterMark > 0)
                        {
                            try
                            {
                                // Not supported on all middlewares.
                                queue.setLowWatermark(mamaLowWaterMark);
                            }
                            catch (Exception e)
                            {
                                Console.WriteLine("Could not set " + queueName + " queue low water mark MamaStatus: " + e);
                            }
                        }
                    }
                }
            }
        }

        private void createSubscriptions()
        {
            mamaSource = new MamaSource();
            mamaSource.symbolNamespace = mamaSourceName;
            mamaSource.transport = mamaTransport;

            for (int i = 0; i < mamaNumSymbols; i++)
            {
                mamaSubscriptions[i] = new MamaSubscription();

                mamaSubscriptions[i].setServiceLevel(mamaServiceLevel.MAMA_SERVICE_LEVEL_REAL_TIME);

                mamaSubscriptions[i].setSubscriptionType(mamaSubscriptionType.MAMA_SUBSC_TYPE_NORMAL);

                mamaSubscriptions[i].setTimeout(10);

                mamaSubscriptions[i].setRetries(3);

                mamaSubscriptions[i].setRequiresInitial(true);

                mamaSubscriptions[i].create(
                    mamaQueueGroup == null ? mamaDefaultQueue : mamaQueueGroup.getNextQueue(),
                    this,
                    mamaSource,
                    mamaSymbols[i]);
            }
        }

        /// <summary>
        /// Subscription callbacks
        /// </summary>
        /// <param name="args"></param>
        ////////////////////////////////////////////////////////////////////////////////////      

        public void onCreate(MamaSubscription subscription)
        {
            stats.mCreateMsgCount++;
        }

        public void onError(MamaSubscription subscription,
            MamaStatus.mamaStatus status,
            string subject)
        {
            stats.mErrorCount++;
        }

        public void onQuality(MamaSubscription subscription,
            mamaQuality quality,
            string symbol)
        {
            Console.WriteLine("Subscription quality:" + (int)quality);
        }

        public void onMsg(MamaSubscription subscription,
            MamaMsg msg)
        {

            if (msg.getType() == mamaMsgType.MAMA_MSG_TYPE_INITIAL)
            {
                stats.mInitialMsgCount++;
            }
            else
                stats.mOtherMsgCount++;

            if (mamaLogLevel > MamaLogLevel.MAMA_LOG_LEVEL_NORMAL)
            {
                Console.WriteLine("msg.getSymbol msg.getStatus");
            }
        }

        public void onGap(MamaSubscription subscription)
        {
            Console.WriteLine("Subscription gap");
        }

        public void onRecapRequest(MamaSubscription subscription)
        {
            Console.WriteLine("Subscription recap request");
        }

        public void onDestroy(MamaSubscription subscription)
        {
        }

        /// <summary>
        /// Transport callbacks
        /// </summary>
        //////////////////////////////////////////////////////////////////////////////////////////////////
        public void onDisconnect(MamaTransport transport)
        {
            Console.WriteLine("Transport disconnected.");
        }

        public void onReconnect(MamaTransport transport)
        {
            Console.WriteLine("Transport Reconnected.");
        }

        public void onQuality(MamaTransport transport)
        {
            Console.WriteLine("Transport Quality.");
        }

        public void onConnect(MamaTransport transport)
        {
            Console.WriteLine("Transport Connect");
        }

        public void onAcceptConnect(MamaTransport transport)
        {
            Console.WriteLine("Transport Accept Connect");
        }

        public void onAcceptReconnect(MamaTransport transport)
        {
            Console.WriteLine("Transport Accept Reconnect.");
        }

        public void onPublisherDisconnect(MamaTransport transport)
        {
            Console.WriteLine("Transport Publisher disconnected.");
        }

        public void onNamingServiceConnect(MamaTransport transport)
        {
            Console.WriteLine("Transport Naming Service Connect.");
        }

        public void onNamingServiceDisconnect(MamaTransport transport)
        {
            Console.WriteLine("Transport Naming Service Disconnected.");
        }

        /// <summary>
        /// Dictionary callbacks
        /// </summary>
        /////////////////////////////////////////////////////////////////////////////////////////////////
        public void onTimeout(MamaDictionary dictionary)
        {
            Console.WriteLine("Dictionary timeout.");

            Mama.stop(mamaBridge);
        }

        public void onError(MamaDictionary dictionary, string message)
        {
            Console.WriteLine("Dictionary error:" + message);

            Mama.stop(mamaBridge);
        }

        public void onComplete(MamaDictionary dictionary)
        {
            Console.WriteLine("Dictionary complete.");

            dictionaryComplete = true;

            Mama.stop(mamaBridge);
        }
        /// <summary>
        /// FieldIterator callback
        /// </summary>
        //////////////////////////////////////////////////////////////////////////////////////////////////
        public void onField(MamaMsg msg,
            MamaMsgField field,
            object closure)
        {
            MamaFieldDescriptor descriptor = field.getDescriptor();
            Console.WriteLine(descriptor.getName() + " : " + field.getAsString());
        }

        /// <summary>
        /// MamaQueueMonitorCallback callbacks
        /// </summary>
        /////////////////////////////////////////////////////////////////////////////////////////////////
        public void HighWatermarkExceeded(
            MamaQueue mamaQueue,
            int size,
            object closure)
        {
            Console.WriteLine(closure + " queue high water mark exceeded. Size " + size);
        }

        public void LowWatermark(
            MamaQueue mamaQueue,
            int size,
            object closure)
        {
            Console.WriteLine(closure + "queue low water mark exceeded. Size " + size);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////

        private bool parseCommandLine(string[] args)
        {
            if ((args == null) || (args.Length == 0))
            {
                Console.WriteLine("Running with default: -S NASDAQ -s MSFT -m wmw -tport rv1");
                mamaSymbols[mamaNumSymbols] = "MSFT";
                mamaNumSymbols++;
                return true;
            }

            for (int i = 0; i < args.Length; i++)
            {
                if (args[i].CompareTo("-S") == 0)
                {
                    if ((i + 1) < args.Length)
                    {
                        mamaSourceName = args[++i];
                        continue;
                    }
                }

                if (args[i].CompareTo("-d") == 0)
                {
                    if ((i + 1) < args.Length)
                    {
                        mamaDictionarySourceName = args[++i];
                        continue;
                    }
                }

                if (args[i].CompareTo("-s") == 0)
                {
                    if ((i + 1) < args.Length)
                    {
                        mamaSymbols[mamaNumSymbols] = args[++i];
                        mamaNumSymbols++;
                        if (mamaNumSymbols == 4999)
                        {
                            Console.WriteLine("Using max of 5000 symbols");
                            break;
                        }
                        continue;
                    }
                }

                if (args[i].CompareTo("-m") == 0)
                {
                    if ((i + 1) < args.Length)
                    {
                        mamaMiddlewareName = args[++i];
                        continue;
                    }
                }

                if (args[i].CompareTo("-tport") == 0)
                {
                    if ((i + 1) < args.Length)
                    {
                        mamaTransportName = args[++i];
                        continue;
                    }
                }

                if ((args[i].CompareTo("-t") == 0) || (args[i].CompareTo("-timeout") == 0))
                {
                    if ((i + 1) < args.Length)
                    {
                        m_churnTimeout = double.Parse(args[++i]);
                        continue;
                    }
                }

                if ((args[i].CompareTo("-k") == 0) || (args[i].CompareTo("-kill") == 0))
                {
                    if ((i + 1) < args.Length)
                    {
                        m_killTimeout = double.Parse(args[++i]);
                        continue;
                    }
                }

                if (args[i].CompareTo("--help") == 0)
                {
                    foreach (string usageLine in m_usage)
                    {
                        Console.WriteLine(usageLine);
                    }
                    return false;
                }

                if (args[i].CompareTo("-?") == 0)
                {
                    foreach (string usageLine in m_usage)
                    {
                        Console.WriteLine(usageLine);
                    }
                    return false;
                }

                if (args[i].CompareTo("-f") == 0)
                {
                    if ((i + 1) < args.Length)
                    {
                        string fileName = args[++i];
                        TextReader tr = new StreamReader(fileName);
                        string symbol = null;
                        while ((symbol = tr.ReadLine()) != null)
                        {
                            symbol = symbol.Trim();

                            if (symbol.Length != 0)
                            {
                                mamaSymbols[mamaNumSymbols] = symbol;
                                mamaNumSymbols++;
                            }
                        }
                        continue;

                    }
                }

                if (args[i].CompareTo("-r") == 0)
                {
                    if ((i + 1) < args.Length)
                    {
                        mamaThrottle = Convert.ToInt64(args[++i]);
                        continue;
                    }
                }

                if (args[i].CompareTo("-rr") == 0)
                {
                    if ((i + 1) < args.Length)
                    {
                        mamaRecapThrottle = Convert.ToInt64(args[++i]);
                        continue;
                    }
                }

                if (args[i].CompareTo("-mp") == 0)
                {
                    if ((i + 1) < args.Length)
                    {
                        mamaMapFilename = args[++i];
                        continue;
                    }
                }

                if (args[i].CompareTo("-threads") == 0)
                {
                    if ((i + 1) < args.Length)
                    {
                        mamaThreads = Convert.ToInt32(args[++i]);
                        continue;
                    }
                }

                if (args[i].CompareTo("-hw") == 0)
                {
                    if ((i + 1) < args.Length)
                    {
                        mamaHighWaterMark = Convert.ToInt32(args[++i]);
                        continue;
                    }
                }

                if (args[i].CompareTo("-lw") == 0)
                {
                    if ((i + 1) < args.Length)
                    {
                        mamaLowWaterMark = Convert.ToInt32(args[++i]);
                        continue;
                    }
                }

                if (args[i].CompareTo("-c") == 0)
                {
                    if ((i + 1) < args.Length)
                    {
                        quota = Convert.ToInt32(args[++i]);
                        continue;
                    }
                }

                if (args[i].CompareTo("-v") == 0)
                {
                    if (mamaLogLevel == MamaLogLevel.MAMA_LOG_LEVEL_WARN)
                    {
                        mamaLogLevel = MamaLogLevel.MAMA_LOG_LEVEL_NORMAL;
                        Mama.enableLogging(mamaLogLevel);
                    }
                    else if (mamaLogLevel == MamaLogLevel.MAMA_LOG_LEVEL_NORMAL)
                    {
                        mamaLogLevel = MamaLogLevel.MAMA_LOG_LEVEL_FINE;
                        Mama.enableLogging(mamaLogLevel);
                    }
                    else if (mamaLogLevel == MamaLogLevel.MAMA_LOG_LEVEL_FINE)
                    {
                        mamaLogLevel = MamaLogLevel.MAMA_LOG_LEVEL_FINER;
                        Mama.enableLogging(mamaLogLevel);
                    }
                    else
                    {
                        mamaLogLevel = MamaLogLevel.MAMA_LOG_LEVEL_FINEST;
                        Mama.enableLogging(mamaLogLevel);
                    }
                    continue;
                }

                if (args[i].CompareTo("-q") == 0)
                {
                    mamaQuietness++;
                    continue;
                }

                if (args[i].CompareTo("-log_file") == 0)
                {
                    if ((i + 1) < args.Length)
                    {
                        Mama.logToFile(args[++i],
                            MamaLogLevel.MAMA_LOG_LEVEL_FINEST);
                        continue;
                    }
                }
            }

            if (mamaNumSymbols == 0)
            {
                mamaSymbols[mamaNumSymbols] = "MSFT";
                mamaNumSymbols++;
            }

            return true;
        }

        /// <summary>
        /// This is the thread procedure that is executed by the churn thread, it will keep
        /// churning subscriptions until the kill event is set.
        /// </summary>
        private void ChurnThreadProc()
        {

            for (; ; )
            {
                // Check the for kill event
                bool signal = m_killEvent.WaitOne(1000, false);

                // Quit the loop if the kill signal was received
                if (signal)
                {
                    break;
                }

                // Churn subscriptions
                for (int j = 0; j < quota; j++)
                {
                    int i = mamaRandom.Next(0, mamaNumSymbols);

                    MamaTimer timer = new MamaTimer();

                    // Get the queue
                    MamaQueue queue = mamaSubscriptions[i].subscQueue;
                    if (queue != null)
                    {
                        timer.create(queue, this, m_churnTimeout, i);
                    }
                }
            }
        }

        private void unintializeMama()
        {
            // Destroy the queues wiating for them to shutdown
            mamaQueueGroup.destroyWait();

            // Destroy the dictionary
            mamaDictionary.Dispose();

            // Destroy the transport
            mamaTransport.destroy();

            // Close Mama
            Mama.close();
        }

        private MamaChurnCS(string[] args)
        {
            try
            {
                if (parseCommandLine(args))
                {

                    // Start up the underlying API
                    initializeMama();
                    try
                    {
                        // Create all of the subscriptions
                        createSubscriptions();
                        try
                        {
                            // Create the autoreset event that will be used to shutdown
                            m_killEvent = new ManualResetEvent(false);

                            // Create a spearate thread to perform the churn
                            m_churnThread = new Thread(new ThreadStart(this.ChurnThreadProc));
                            m_churnThread.IsBackground = true;
                            m_churnThread.Name = "destroyerThread: ";
                            m_churnThread.Start();

                            stats = new StatsReport(mamaDefaultQueue);

                            // If the timeout has been supplied then create a new timer
                            if (m_killTimeout > 0)
                            {
                                MamaTimer killTimer = new MamaTimer();
                                killTimer.create(Mama.getDefaultEventQueue(mamaBridge), new KillTimerCallback(mamaBridge), m_killTimeout, null);
                            }

                            // Keep procssing messages until the kill timer elapses
                            Mama.start(mamaBridge);

                            // Destroy the stats timer
                            stats.destroy();

                            // Kill the churn thread by signaling the auto reset event
                            m_killEvent.Set();

                            // Wait until the thread terminates
                            m_churnThread.Join();
                        }

                        finally
                        {
                            // Stop processing all the queues
                            mamaQueueGroup.stop();

                            // Destroy all the subscriptions                            
                            for (int subIndex = 0; subIndex < mamaSubscriptions.Length; subIndex++)
                            {
                                mamaSubscriptions[subIndex].destroy();
                                mamaSubscriptions[subIndex] = null;
                            }
                        }
                    }

                    finally
                    {
                        // Uninitialise the api
                        unintializeMama();
                    }
                }
            }

            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }
        }



        public void onTimer(MamaTimer mamaTimer, object closure)
        {
            mamaTimer.destroy();

            // Only continue if the application is not shutting down
            bool shuttingDown = m_killEvent.WaitOne(0, false);
            if (!shuttingDown)
            {
                int i = (int)closure;

                mamaSubscriptions[i].destroy();

                mamaSubscriptions[i] = new MamaSubscription();

                mamaSubscriptions[i].setServiceLevel(mamaServiceLevel.MAMA_SERVICE_LEVEL_REAL_TIME);

                mamaSubscriptions[i].setSubscriptionType(mamaSubscriptionType.MAMA_SUBSC_TYPE_NORMAL);

                mamaSubscriptions[i].setTimeout(10);

                mamaSubscriptions[i].setRetries(3);

                mamaSubscriptions[i].setRequiresInitial(true);

                mamaSubscriptions[i].create(
                    mamaQueueGroup == null ? mamaDefaultQueue : mamaQueueGroup.getNextQueue(),
                    this,
                    mamaSource,
                    mamaSymbols[i]);
                stats.mRecreates++;
            }
        }

        public void onDestroy(MamaTimer mamaTimer, object closure)
        {
        }


        [STAThread]
        static void Main(string[] args)
        {
            MamaChurnCS listen = new MamaChurnCS(args);
        }
    }
}



