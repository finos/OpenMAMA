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
using System.Collections;
using System.IO;
using System.Runtime.InteropServices;
using System.Threading;
using Wombat;

namespace MamaListen
{
    /// <summary>
    /// MamaListen - Generic Mama API subscriber.
    ///
    ///Illustrates the following MAMA API features:
    ///1. Data dictionary initialization.
    ///2. Creating a transport.
    ///3. Creating different types of subscriptions (Snapshot, group etc)
    ///4. Walking all received fields in messages.
    ///5  Displaying only specific fields in a message.
    /// </summary>
    class MamaListen
    {
        #region Class Member Definition

        #region Private Constants

        /// <summary>
        /// The usage string
        /// </summary>
        private const string usage = "MamaListen [-S SOURCE] [-s SYMBOL] [-m MIDDLEWARE] [-tport TRANSPORT] [-f SYMBOL_FILE] [-r THROTTLE_RATE] [-rr RECAP_THROTTLE_RATE] [-mp MAP_FILENAME] [-threads NUM_THREADS] [-hw HIGH_WATERMARK] [-lw LOW_WATERMARK] [-v] [-v] [-v] [-v] [-q] [-q] [-q] [-ni] [-log_file LOG_FILE] [-d DICT_SOURCE] [-dict_tport DICTIONARY_TRANSPORT] [--help] [-?]";

        #endregion

        #region Private Member Variables

        /// <summary>
        /// The mama bridge.
        /// </summary>
        private MamaBridge m_bridge;

        /// <summary>
        /// A reference is held to the default queue for convenience.
        /// </summary>
        private MamaQueue m_defaultQueue;

        /// <summary>
        /// The mama dictionary, note that it may not be used.
        /// </summary>
        private MamaDictionary m_dictionary;

        /// <summary>
        /// The name of the dictionary source, this will default to WOMBAT.
        /// </summary>
        private string m_dictionarySourceName;

        /// <summary>
        /// The name of the dictionary transport, if one isn't specified then the main transport will be used.
        /// </summary>
        private string m_dictionaryTransportName;

        /// <summary>
        /// If set the dictionary will be downloaded on startup, this is the default behaviour.
        /// </summary>
        private bool m_downloadDictionary;

        /// <summary>
        /// The high watermark at which the queue callback will be invoked.
        /// </summary>
        private int m_highWaterMark;

        /// <summary>
        /// Whether to use the iterator when parsing through the message.
        /// </summary>
        private bool m_iterator;

        /// <summary>
        /// The log level is controlled by the number of 'v's passed on the command line, this will default to warn.
        /// </summary>
        private MamaLogLevel m_logLevel;

        /// <summary>
        /// The low watermark at which the queue callback will be invoked
        /// </summary>
        private int m_lowWaterMark;

        /// <summary>
        /// The name of the middleware to load, this defaults to Wombat Middlware.
        /// </summary>
        private string m_middlewareName;

        /// <summary>
        /// The number of threads to use, this corresponds to the number of queues created.
        /// </summary>
        private int m_numberThreads;

        /// <summary>
        /// The queue group will be used if multiple threads have been specified.
        /// </summary>
        private MamaQueueGroup m_queueGroup;

        /// <summary>
        /// The quietness level, defaults to 0.
        /// </summary>
        private int m_quietness;

        /// <summary>
        /// The rate to set the recap throttle, if this is -1 then no rate will be applied.
        /// </summary>
        private long m_recapThrottleRate;

        /// <summary>
        /// The amount of time the application will run for, by default this is set to 0 which means
        /// that no shutdown timer will be created.
        /// </summary>
        private double m_shutdownTime;

        /// <summary>
        /// This flag will be set to create snapshot subscriptions, this is off by default.
        /// </summary>
        private bool m_snapshot;

        /// <summary>
        /// The source to use when creating subscriptions.
        /// </summary>
        private MamaSource m_source;

        /// <summary>
        /// The name of the source, defaults to WOMBAT.
        /// </summary>
        private string m_sourceName;

        /// <summary>
        /// Contains all of the subscriptions.
        /// </summary>
        private ArrayList m_subscriptions;

        /// <summary>
        /// To track the number of subscriptions created.
        /// </summary>
        public int m_numberSubscriptions;

        /// <summary>
        /// Contains all the symbols for which subscriptions must be established.
        /// </summary>
        private ArrayList m_symbols;

        /// <summary>
        /// The rate to set the throttle, if this is -1 then no rate will be applied.
        /// </summary>
        private long m_throttleRate;

        /// <summary>
        /// The mama transport.
        /// </summary>
        private MamaTransport m_transport;

        /// <summary>
        /// The name of the transport to use, note that there is no default.
        /// </summary>
        private string m_transportName;

        /// <summary>
        /// Semaphore object that guards the outer class from calling mama.close until
        /// all subscription callbacks are recieved
        /// </summary>
        public Semaphore m_subscriptionSemaphore;

        #endregion

        #region Internal Classes

        #region ListenerDictionaryCallback Class

        /// <summary>
        /// This callback is used to download the dictionary and will stop the bridge
        /// whenever the dictionary comes back on an error occurs.
        /// </summary>
        internal class ListenerDictionaryCallback : MamaDictionaryCallback
        {
            /// <summary>
            /// The bridge will be stopped whenever a callback is received.
            /// </summary>
            private MamaBridge m_bridge;

            /// <summary>
            /// Flag will be set if the dictionary is downloaded correctly.
            /// </summary>
            private bool m_dictionaryComplete;

            internal ListenerDictionaryCallback(MamaBridge bridge)
            {
                // Save arguments in member variables
                m_bridge = bridge;
            }

            internal bool DictionaryComplete
            {
                get
                {
                    return m_dictionaryComplete;
                }
            }

            public void onTimeout(MamaDictionary dictionary)
            {
                Console.WriteLine("Dictionary timeout.");

                Mama.stop(m_bridge);
            }

            public void onError(MamaDictionary dictionary, string message)
            {
                Console.WriteLine("Dictionary error:" + message);

                Mama.stop(m_bridge);
            }

            public void onComplete(MamaDictionary dictionary)
            {
                Console.WriteLine("Dictionary complete.");

                m_dictionaryComplete = true;

                Mama.stop(m_bridge);
            }
        }

        #endregion

        #region ListenerFieldCallback Class

        internal class ListenerFieldCallback : MamaMsgFieldIterator
        {
            /// <summary>
            /// The dictionary.
            /// </summary>
            private MamaDictionary m_dictionary;

            /// <summary>
            /// Whether to use the iterator when parsing through the message.
            /// </summary>
            private bool m_iterator;

            /// <summary>
            /// The quietness level.
            /// </summary>
            private int m_quietness;

            internal ListenerFieldCallback(MamaDictionary dictionary, bool iterator, int quietness)
            {
                // Save arguments in member variables
                m_dictionary = dictionary;
                m_iterator = iterator;
                m_quietness = quietness;
            }

            public void onField(MamaMsg msg, MamaMsgField field, object closure)
            {
                // Get the indent from the closure
                int indent = (int)closure;

                // Display the field with the specified indent
                displayField(field, indent);
            }

            /// <summary>
            /// Display the contents of a field in the console window.
            /// </summary>
            /// <param name="field">
            /// The field to display.
            /// </param>
            /// <param name="indent">
            /// The level of indent to add before the string for formatting purposes.
            /// </param>
            internal void displayField(MamaMsgField field, int indent)
            {
                // Format a log string
                string logMessage = "";

                string indentSpacer = "  ";
                for (int i = 0; i < indent; i++)
                    indentSpacer += "  ";

                if (m_quietness < 1)
                {
                    logMessage = string.Format(
                        "{0}{1, -20 } |{2,5} | {3, 10} | ",
                        indentSpacer,
                        field.getName(),
                        field.getFid(),
                        field.getTypeName());
                }

                mamaFieldType fieldType = field.getType();

                switch (fieldType)
                {
                    case mamaFieldType.MAMA_FIELD_TYPE_MSG:
                        {
                            MamaMsg tmpMsg = field.getMsg();
                            Console.WriteLine(logMessage);
                            Console.WriteLine(indentSpacer + "{");


                            if (!(m_iterator))
                            {
                                tmpMsg.iterateFields(this, m_dictionary, indent + 1);
                            }
                            else
                            {
                                MamaMsgIterator subIterator = new MamaMsgIterator(m_dictionary);
                                MamaMsgField subField;
                                tmpMsg.begin(ref subIterator);
                                while ((subField = subIterator.getField()) != null)
                                {
                                    displayField(subField, indent + 1);
                                    subIterator++;
                                }
                            }
                            Console.WriteLine(indentSpacer + "}");
                        }
                        break;
                    case mamaFieldType.MAMA_FIELD_TYPE_VECTOR_MSG:
                        {
                            MamaMsg[] tmpMsgarray = field.getVectorMsg();
                            foreach (MamaMsg tmpMsg in tmpMsgarray)
                            {
                                Console.WriteLine(logMessage);
                                Console.WriteLine(indentSpacer + "{");
                                if (!(m_iterator))
                                {
                                    tmpMsg.iterateFields(this, m_dictionary, indent + 1);
                                }
                                else
                                {
                                    MamaMsgIterator subIterator = new MamaMsgIterator(m_dictionary);
                                    MamaMsgField subField;
                                    tmpMsg.begin(ref subIterator);
                                    while ((subField = subIterator.getField()) != null)
                                    {
                                        displayField(subField, indent + 1);
                                        subIterator++;
                                    }
                                }
                                Console.WriteLine(indentSpacer + "}");
                            }
                        }
                        break;
                    case mamaFieldType.MAMA_FIELD_TYPE_STRING:
                        logMessage += field.getString();
                        break;
                    case mamaFieldType.MAMA_FIELD_TYPE_BOOL:
                        if (m_quietness < 1)
                            logMessage += field.getBool().ToString();
                        else
                            logMessage += field.getBool();
                        break;
                    case mamaFieldType.MAMA_FIELD_TYPE_CHAR:
                        if (m_quietness < 1)
                            logMessage += field.getChar().ToString();
                        else
                            logMessage += field.getChar();
                        break;
                    case mamaFieldType.MAMA_FIELD_TYPE_I8:
                        if (m_quietness < 1)
                            logMessage += field.getI8().ToString();
                        else
                            logMessage += field.getI8();
                        break;
                    case mamaFieldType.MAMA_FIELD_TYPE_U8:
                        if (m_quietness < 1)
                            logMessage += field.getU8().ToString();
                        else
                            logMessage += field.getU8();
                        break;
                    case mamaFieldType.MAMA_FIELD_TYPE_I16:
                        if (m_quietness < 1)
                            logMessage += field.getI16().ToString();
                        else
                            logMessage += field.getI16();
                        break;
                    case mamaFieldType.MAMA_FIELD_TYPE_U16:
                        if (m_quietness < 1)
                            logMessage += field.getU16().ToString();
                        else
                            logMessage += field.getU16();
                        break;
                    case mamaFieldType.MAMA_FIELD_TYPE_I32:
                        if (m_quietness < 1)
                            logMessage += field.getI32().ToString();
                        else
                            logMessage += field.getI32();
                        break;
                    case mamaFieldType.MAMA_FIELD_TYPE_U32:
                        if (m_quietness < 1)
                            logMessage += field.getU32().ToString();
                        else
                            logMessage += field.getU32();
                        break;
                    case mamaFieldType.MAMA_FIELD_TYPE_I64:
                        if (m_quietness < 1)
                            logMessage += field.getI64().ToString();
                        else
                            logMessage += field.getI64();
                        break;
                    case mamaFieldType.MAMA_FIELD_TYPE_U64:
                        if (m_quietness < 1)
                            logMessage += field.getU64().ToString();
                        else
                            logMessage += field.getU64();
                        break;
                    case mamaFieldType.MAMA_FIELD_TYPE_F32:
                        if (m_quietness < 1)
                            logMessage += field.getF32().ToString();
                        else
                            logMessage += field.getF32();
                        break;
                    case mamaFieldType.MAMA_FIELD_TYPE_F64:
                        if (m_quietness < 1)
                            logMessage += field.getF64().ToString();
                        else
                            logMessage += field.getF64();
                        break;
                    case mamaFieldType.MAMA_FIELD_TYPE_TIME:
                        if (m_quietness < 1)
                            logMessage += field.getDateTime().ToString();
                        else
                            logMessage += field.getDateTime();
                        break;
                    case mamaFieldType.MAMA_FIELD_TYPE_PRICE:
                        if (m_quietness < 1)
                            logMessage += field.getPrice().getAsString();
                        else
                            logMessage += field.getPrice();
                        break;

                    default:

                        logMessage += field.getAsString();
                        break;
                }

                Console.WriteLine(logMessage);
            }
        }

        #endregion

        #region ListenerSubscriptionCallback Class

        internal class ListenerSubscriptionCallback : MamaSubscriptionCallback
        {
            /// <summary>
            /// The dictionary.
            /// </summary>
            private MamaDictionary m_dictionary;

            /// <summary>
            /// Reference to the outer class
            /// </summary>
            private MamaListen m_listener;

            /// <summary>
            /// Whether to use the iterator when parsing through the message.
            /// </summary>
            private bool m_iterator;

            /// <summary>
            /// A reusable iterator for the message.
            /// </summary>
            private MamaMsgIterator m_messageIterator;

            /// <summary>
            /// The quietness level.
            /// </summary>
            private int m_quietness;

            /// <summary>
            /// The total number of subscriptions that must be created.
            /// </summary>
            private int m_totalSubscriptions;

            internal ListenerSubscriptionCallback(MamaListen listener, MamaDictionary dictionary, bool iterator, int quietness, int totalSubscriptions)
            {
                // Save arguments in member variables
                m_dictionary = dictionary;
                m_iterator = iterator;
                m_quietness = quietness;
                m_totalSubscriptions = totalSubscriptions;
                m_listener = listener;

                // Create the message iterator
                if (iterator)
                {
                    m_messageIterator = new MamaMsgIterator(m_dictionary);
                }
            }

            public void onCreate(MamaSubscription subscription)
            {
                // Increment the number of subscriptions
                int totalSubscriptions = Interlocked.Increment(ref m_listener.m_numberSubscriptions);

                // Log a message if we've got them all
                if (totalSubscriptions == m_totalSubscriptions)
                {
                    Console.WriteLine("All Subscriptions created.");
                }
            }

            public void onError(MamaSubscription subscription, MamaStatus.mamaStatus status, string subject)
            {
                Console.WriteLine("Subscription error:" + subject);
                subscription.destroy();
            }

            public void onGap(MamaSubscription subscription)
            {
                Console.WriteLine("Subscription gap");
            }

            public void onQuality(MamaSubscription subscription, mamaQuality quality, string symbol)
            {
                Console.WriteLine("Subscription quality:" + (int)quality);
            }

            public void onRecapRequest(MamaSubscription subscription)
            {
                Console.WriteLine("Subscription recap request");
            }

            public void onDestroy(MamaSubscription subscription)
            {
                Interlocked.Decrement(ref m_listener.m_numberSubscriptions);
                if (m_listener.m_numberSubscriptions <= 0)
                {
                    m_listener.m_subscriptionSemaphore.Release(1);
                }
            }

            public void onMsg(MamaSubscription subscription, MamaMsg msg)
            {
                // Create a field callback class to display the field contents
                ListenerFieldCallback fieldCallback = new ListenerFieldCallback(m_dictionary, m_iterator, m_quietness);

                if (m_quietness < 2)
                {
                    Console.WriteLine(subscription.subscSource + "." +
                             subscription.subscSymbol + " TYPE: " +
                             msg.getType() + " Status: " +
                             msg.getStatus());
                }

                if (m_quietness < 1)
                {
                    if (m_iterator)
                    {
                        // Set the iterator to the beginning of the message
                        msg.begin(ref m_messageIterator);

                        // Keep going until there are no more fields
                        MamaMsgField field = m_messageIterator.getField();
                        while (field != null)
                        {
                            // Display the field
                            fieldCallback.displayField(field, 0);

                            m_messageIterator++;

                            // Get the next field
                            field = m_messageIterator.getField();
                        }
                    }

                    else
                    {
                        // Otherwise use the iteration function on the message itself
                        int indentlevel = 0;
                        msg.iterateFields(fieldCallback, m_dictionary, indentlevel);
                    }
                }
            }
        }

        #endregion

        #region ListenerQueueCallback Class

        /// <summary>
        /// This class implements the queue monitoring callback and will write a message to the console
        /// whenever one of the watermarks is hit.
        /// </summary>
        internal class ListenerQueueCallback : MamaQueueMonitorCallback
        {
            public void HighWatermarkExceeded(MamaQueue mamaQueue, int size, object closure)
            {
                Console.WriteLine("Default queue high water mark exceeded. Size " + size);
            }

            public void LowWatermark(MamaQueue mamaQueue, int size, object closure)
            {
                Console.WriteLine("Default queue low water mark exceeded. Size " + size);
            }
        }

        #endregion

        #region ListenerTransportCallback Class

        /// <summary>
        /// This class implements the transport callbacks which simply log a message whenever something
        /// happens.
        /// </summary>
        internal class ListenerTransportCallback : MamaTransportCallback
        {
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
        }

        #endregion

        #region ListenShutdownTimerCallback Class

        /// <summary>
        /// This class implements the shutdown timer callback and will stop mama processing messages
        /// whenever the timer ticks. This will then unblock the main thread and allow the application
        /// to shut down.
        /// </summary>
        internal class ListenShutdownTimerCallback : MamaTimerCallback
        {
            /// <summary>
            /// The MAMA bridge is needed to stop processing.
            /// </summary>
            private MamaBridge m_bridge;

            internal ListenShutdownTimerCallback(MamaBridge bridge)
            {
                m_bridge = bridge;
            }

            public void onTimer(MamaTimer mamaTimer, object closure)
            {
                mamaTimer.destroy();
            }

            public void onDestroy(MamaTimer mamaTimer, object closure)
            {
                Mama.stop(m_bridge);
            }
        }

        #endregion

        #region NativeMethods Class

        /// <summary>
        /// This class contains all native method declarations.
        /// </summary>
        internal class NativeMethods
        {
            public enum ConsoleEvent
            {
                CTRL_C = 0
            }

            public delegate bool ControlEventHandler(ConsoleEvent consoleEvent);

            [DllImport("kernel32.dll")]
            public static extern bool SetConsoleCtrlHandler(ControlEventHandler e, bool add);
        }

        #endregion

        #endregion

        #endregion

        #region Construction and Finalization

        /// <summary>
        /// Constructor initialises all member variables including setting the defaults for the various
        /// options that can be provided on the command line.
        /// </summary>
        private MamaListen()
        {
            // Initialise member variables
            m_dictionarySourceName      = "WOMBAT";
            m_downloadDictionary        = true;
            m_logLevel                  = MamaLogLevel.MAMA_LOG_LEVEL_WARN;
            m_middlewareName            = "wmw";
            m_recapThrottleRate         = -1;
            m_sourceName                = "Wombat";
            m_subscriptions             = new ArrayList();
            m_symbols                   = new ArrayList();
            m_throttleRate              = -1;
            m_numberSubscriptions       = 0;
            m_subscriptionSemaphore     = new Semaphore(0,1);
        }

        #endregion

        #region Internal Functions

        /// <summary>
        /// This function will run the application and create market data subscriptions.
        /// </summary>
        /// <param name="args">
        /// Command line arguments.
        /// </param>
        internal void run(string[] args)
        {
            // Install an event handler to capture the Ctrl-C press
            NativeMethods.ControlEventHandler eventHandler = new NativeMethods.ControlEventHandler(this.onControlHandler);
            NativeMethods.SetConsoleCtrlHandler(eventHandler, true);

            // Parse the command line arguments
            bool shouldContinue = parseCommandLine(args);
            if (shouldContinue)
            {
                initializeMama();
                try
                {
                    createSubscriptions();
                    try
                    {
                        // Create a shutdown timer if appropriate
                        if (m_shutdownTime > 0)
                        {
                            // The timer will be destroyed whenever it ticks
                            MamaTimer shutdownTimer = new MamaTimer();
                            shutdownTimer.create(m_defaultQueue, new ListenShutdownTimerCallback(m_bridge), m_shutdownTime, null);
                        }

                        // Start the bridge, this call will block until either Ctrl C is pressed or the shutdown timer ticks
                        Mama.start(m_bridge);
                    }

                    finally
                    {
                        if (m_numberThreads > 0)
                        {
                            m_queueGroup.stop();
                        }
                    }
                }
                finally
                {
                    // Clean up mama
                    uninitializeMama();
                }
            }
        }

        #endregion

        #region Private Functions

        /// <summary>
        /// This function will create the dictionary.
        /// </summary>
        private void createDictionary()
        {
            if (m_downloadDictionary)
            {
                using (MamaSource dictionarySource = new MamaSource())
                {
                    /* If a dictionary transport has been specified then a new one must be created otherwise
                     * the main transport will be re-used.
                     */
                    MamaTransport dictionaryTransport = null;
                    try
                    {
                        if((m_dictionaryTransportName == null) || (m_dictionaryTransportName == string.Empty))
                        {
                            dictionarySource.transport = m_transport;
                        }
                        else
                        {
                            dictionaryTransport = new MamaTransport();
                            dictionaryTransport.create(m_dictionaryTransportName, m_bridge);

                            dictionarySource.transport = dictionaryTransport;
                        }

                        // The symbol namespace must also be set
                        dictionarySource.symbolNamespace = m_dictionarySourceName;

                        // Instantiate a callback class
                        ListenerDictionaryCallback dictionaryCallback = new ListenerDictionaryCallback(m_bridge);

                        // Create the actual dictionary object using the default queue
                        m_dictionary = new MamaDictionary();
                        m_dictionary.create(m_defaultQueue, dictionaryCallback, dictionarySource, 3, 10.0);

                        // Start the bridge, this will block until the dictionary is downloaded or something goes wrong
                        Mama.start(m_bridge);

                        // If something went wrong then throw an exception
                        if (!dictionaryCallback.DictionaryComplete)
                        {
                            throw new ApplicationException("Can't create dictionary.");
                        }
                    }

                    finally
                    {
                        if (dictionaryTransport != null)
                        {
                            dictionaryTransport.destroy();
                        }
                    }
                }
            }
        }

        /// <summary>
        /// Queues will be created if the user has specified a number of threads to be
        /// used, otherwise the default queue will be used.
        /// </summary>
        private void createQueues()
        {
            // Only continue if the number of threads required is greater than 0
            if (m_numberThreads > 0)
            {
                m_queueGroup = new MamaQueueGroup(m_bridge, m_numberThreads);

                if ((m_highWaterMark > 0) || (m_lowWaterMark > 0))
                {
                    ListenerQueueCallback queueCallback = new ListenerQueueCallback();

                    // Install the callback on each queue
                    for (int index = 0; index < m_numberThreads; index++)
                    {
                        MamaQueue queue = m_queueGroup.getNextQueue();

                        string queueName = ("QUEUE " + index);
                        Console.WriteLine("Setting monitor for " + queueName);

                        // Install the callbacks
                        queue.setQueueMonitorCallbacks(queueCallback, queueName);

                        // Set the watermarks at which the callbacks are invoked
                        if(m_highWaterMark > 0)
                        {
                            queue.setHighWatermark(m_highWaterMark);
                        }

                        if(m_lowWaterMark > 0)
                        {
                            // Not supported on all middlewares.
                            queue.setLowWatermark(m_lowWaterMark);
                        }
                    }
                }
            }
        }

        /// <summary>
        /// Creates the source required to establish market data subscriptions.
        /// </summary>
        private void createSource()
        {
            // Allocate a new source object
            m_source = new MamaSource();

            // Set the source name and main transport
            m_source.symbolNamespace = m_sourceName;
            m_source.transport       = m_transport;
        }

        /// <summary>
        /// This function will create subscriptions for all the symbols in the array list.
        /// </summary>
        private void createSubscriptions()
        {
            // There must be at least 1 symbol
            if (m_symbols.Count < 1)
            {
                throw new ApplicationException("There are no symbols to subscribe to.");
            }

            ListenerSubscriptionCallback subscriptionCallback = new ListenerSubscriptionCallback(this, m_dictionary, m_iterator, m_quietness, m_symbols.Count);

            // Enumerate all the symbol names and create a subscription for each one
            foreach (string symbol in m_symbols)
            {
                MamaSubscription subscription = new MamaSubscription();

                // Set the service level depending on whether we need a snapshot subscription or not
                if (m_snapshot)
                {
                    subscription.setServiceLevel(mamaServiceLevel.MAMA_SERVICE_LEVEL_SNAPSHOT);
                }

                else
                {
                    subscription.setServiceLevel(mamaServiceLevel.MAMA_SERVICE_LEVEL_REAL_TIME);
                }

                // Complete the remaining properties
                subscription.setSubscriptionType(mamaSubscriptionType.MAMA_SUBSC_TYPE_NORMAL);
                subscription.setTimeout(10);
                subscription.setRetries(3);
                subscription.setRequiresInitial(true);

                /* Determine the queue to use, if there are multiple threads running then the next queue in the
                 * group will be acquired.
                 */
                MamaQueue queue = m_defaultQueue;
                if (m_queueGroup != null)
                {
                    queue = m_queueGroup.getNextQueue();
                }

                subscription.create(queue, subscriptionCallback, m_source, symbol);

                // Add the subscription to the array list so that they can all be destroyed later on
                m_subscriptions.Add(subscription);
            }
        }

        /// <summary>
        /// This function will create the mama transport using the values specified in the member
        /// variables.
        /// </summary>
        private void createTransport()
        {
            m_transport = new MamaTransport();

            // Register for the callbacks before the transport is created
            m_transport.setTransportCallback(new ListenerTransportCallback());

            // Set the throttle rate if one has been specified, note that 0 is valid and turns the throttle off
            if(m_throttleRate != -1)
            {
                m_transport.setOutboundThrottle(MamaTransport.MamaThrottleInstance.MAMA_THROTTLE_DEFAULT, m_throttleRate);
            }

            // Set the recap throttle rate if one has been specified, note that 0 is valid and turns the throttle off
            if(m_recapThrottleRate != -1)
            {
                m_transport.setOutboundThrottle(MamaTransport.MamaThrottleInstance.MAMA_THROTTLE_RECAP, m_recapThrottleRate);
            }

            // Create the transport now that everything has been set
            m_transport.create(m_transportName, m_bridge);
        }

        /// <summary>
        /// This function will destroy all the subscriptions.
        /// </summary>
        private void destroySubscriptions()
        {
            // Enumerate all the subscriptions in the array list
            foreach (MamaSubscription subscription in m_subscriptions)
            {
                // We can destroy the subscription on a different thread as the bridge has been stopped
                subscription.destroyEx();
            }

            // Clear the array
            m_subscriptions.Clear();
        }

        /// <summary>
        /// This function will initialise mama including loading the bridge and creating the queues and transports.
        /// </summary>
        private void initializeMama()
        {
            // Set the desired log level, (based on the number of 'v's passed on the command line)
            Mama.enableLogging(m_logLevel);

            m_bridge = Mama.loadBridge(m_middlewareName);

            Console.WriteLine(Mama.getVersion(m_bridge));

            Mama.open();

            m_defaultQueue = Mama.getDefaultEventQueue(m_bridge);

            // The transport must be created before the dictionary is downloaded
            createTransport();

            createDictionary();

            createQueues();

            // Create the source needed to establish market data subscriptions
            createSource();
        }

        /// <summary>
        /// This handler is called whenever a control event is received such as Ctrl C.
        /// </summary>
        /// <param name="consoleEvent">
        /// The event that has been received.
        /// </param>
        /// <returns>
        /// True is returned if the event is handled, basically just for Ctrl C.
        /// </returns>
        private bool onControlHandler(NativeMethods.ConsoleEvent consoleEvent)
        {
            bool ret = false;

            // Check for Ctrl C
            if (consoleEvent == NativeMethods.ConsoleEvent.CTRL_C)
            {
                destroySubscriptions();
                if (m_numberSubscriptions > 0)
                {
                    m_subscriptionSemaphore.WaitOne();
                }

                Mama.stop(m_bridge);

                // The event has been handled
                ret = true;
            }

            return ret;
        }

        /// <summary>
        /// This function will parse the command line and initialise the various member variables with
        /// the information passed in.
        /// </summary>
        /// <param name="args">
        /// The array of command line arguments.
        /// </param>
        /// <returns>
        /// True to continue with the application or false to quit out.
        /// </returns>
        private bool parseCommandLine(string[] args)
        {
            bool ret = true;

            if ((args == null) || (args.Length == 0))
            {
                throw new ApplicationException("No command line arguments supplied.");
            }

            // Iterate through all the arguments and check for possible values
            for (int index = 0; index < args.Length; index++)
            {
                // Create snapshot subscriptions
                if (args[index].CompareTo("-1") == 0)
                {
                    m_snapshot = true;
                    continue;
                }

                // Don't obtain the dictionary
                if (args[index].CompareTo("-B") == 0)
                {
                    m_downloadDictionary = false;
                    continue;
                }

                // The dictionary source name
                if (args[index].CompareTo("-d") == 0)
                {
                    if ((index + 1) < args.Length)
                    {
                        m_dictionarySourceName = args[++index];
                        continue;
                    }
                }

                // The dictionary transport
                if (args[index].CompareTo("-dict_tport") == 0)
                {
                    if ((index + 1) < args.Length)
                    {
                        m_dictionaryTransportName = args[++index];
                        continue;
                    }
                }

                // Symbol file path
                if (args[index].CompareTo("-f") == 0)
                {
                    if ((index + 1) < args.Length)
                    {
                        string fileName = args[++index];

                        readSymbolsFromFile(fileName);
                        continue;
                    }
                }

                // Display the usage string
                if ((args[index].CompareTo("--help") == 0) || (args[index].CompareTo("-?") == 0))
                {
                    Console.WriteLine(usage);

                    ret = false;
                    break;
                }

                // The high watermark level for queue monitoring
                if (args[index].CompareTo("-hw") == 0)
                {
                    if ((index + 1) < args.Length)
                    {
                        m_highWaterMark = Convert.ToInt32(args[++index]);
                        continue;
                    }
                }

                // Write out to a log file
                if (args[index].CompareTo("-log_file") == 0)
                {
                    if ((index + 1) < args.Length)
                    {
                        m_logLevel = MamaLogLevel.MAMA_LOG_LEVEL_FINEST;

                        Mama.logToFile(args[++index], MamaLogLevel.MAMA_LOG_LEVEL_FINEST);
                        continue;
                    }
                }

                // The low watermark level for queue monitoring
                if (args[index].CompareTo("-lw") == 0)
                {
                    if ((index + 1) < args.Length)
                    {
                        m_lowWaterMark = Convert.ToInt32(args[++index]);
                        continue;
                    }
                }

                // The middleware name
                if (args[index].CompareTo("-m") == 0)
                {
                    if ((index + 1) < args.Length)
                    {
                        m_middlewareName = args[++index];
                        continue;
                    }
                }

                // Use new iterator functionality
                if (args[index].CompareTo("-ni") == 0)
                {
                    m_iterator = true;
                    continue;
                }

                // The quietness level
                if (args[index].CompareTo("-q") == 0)
                {
                    m_quietness++;
                    continue;
                }

                // The throttle rate
                if (args[index].CompareTo("-r") == 0)
                {
                    if ((index + 1) < args.Length)
                    {
                        m_throttleRate = Convert.ToInt64(args[++index]);
                        continue;
                    }
                }

                // Recap throttle rate
                if (args[index].CompareTo("-rr") == 0)
                {
                    if ((index + 1) < args.Length)
                    {
                        m_recapThrottleRate = Convert.ToInt64(args[++index]);
                        continue;
                    }
                }

                // The source name
                if (args[index].CompareTo("-S") == 0)
                {
                    if ((index + 1) < args.Length)
                    {
                        m_sourceName = args[++index];
                        continue;
                    }
                }

                // The symbol name
                if (args[index].CompareTo("-s") == 0)
                {
                    if ((index + 1) < args.Length)
                    {
                        m_symbols.Add(args[++index]);
                        continue;
                    }
                }

                // Length of time to run for
                if (args[index].CompareTo("-shutdown") == 0)
                {
                    m_shutdownTime = double.Parse(args[++index]);
                    continue;
                }

                // Number of threads to run
                if (args[index].CompareTo("-threads") == 0)
                {
                    if ((index + 1) < args.Length)
                    {
                        m_numberThreads = Convert.ToInt32(args[++index]);
                        continue;
                    }

                }

                // The main transport
                if (args[index].CompareTo("-tport") == 0)
                {
                    if ((index + 1) < args.Length)
                    {
                        m_transportName = args[++index];
                        continue;
                    }

                }

                // The verbosity level
                if (args[index].CompareTo("-v") == 0)
                {
                    switch (m_logLevel)
                    {
                        case MamaLogLevel.MAMA_LOG_LEVEL_WARN:
                            m_logLevel = MamaLogLevel.MAMA_LOG_LEVEL_NORMAL;
                            break;
                        case MamaLogLevel.MAMA_LOG_LEVEL_NORMAL:
                            m_logLevel = MamaLogLevel.MAMA_LOG_LEVEL_FINE;
                            break;
                        case MamaLogLevel.MAMA_LOG_LEVEL_FINE:
                            m_logLevel = MamaLogLevel.MAMA_LOG_LEVEL_FINER;
                            break;
                        case MamaLogLevel.MAMA_LOG_LEVEL_FINER:
                            m_logLevel = MamaLogLevel.MAMA_LOG_LEVEL_FINEST;
                            break;
                    }
                    continue;
                }
            }

            if (m_symbols.Count == 0)
            {
                throw new ApplicationException("No symbols were supplied");
            }

            return ret;
        }

        /// <summary>
        /// This function will read all the symbols from a file name and populate the symbols array list.
        /// </summary>
        /// <param name="fileName">
        /// Full path to the file.
        /// </param>
        private void readSymbolsFromFile(string fileName)
        {
            // Open the file with a text reader
            using (StreamReader reader = new StreamReader(fileName))
            {
                // Each line contains a symbol
                while (reader.Peek() >= 0)
                {
                    string symbol = reader.ReadLine();

                    m_symbols.Add(symbol);
                }
            }
        }

        /// <summary>
        /// This function will clean up all the objects initialised before Mama.start is called.
        /// </summary>
        private void uninitializeMama()
        {
            if (m_source != null)
            {
                m_source.destroy();
                m_source = null;
            }

            if (m_dictionary != null)
            {
                m_dictionary.Dispose();
                m_dictionary = null;
            }

            if (m_queueGroup != null)
            {
                m_queueGroup.destroyWait();
                m_queueGroup = null;
            }

            if (m_transport != null)
            {
                m_transport.destroy();
                m_transport = null;
            }

            Mama.close();
        }

        #endregion

        #region Main Function

        [STAThread]
        static void Main(string[] args)
        {
            try
            {
                // Create the main class
                MamaListen listen = new MamaListen();

                // Start the application
                listen.run(args);
            }

            catch (Exception except)
            {
                // Display the message
                Console.WriteLine(except.Message);
            }
        }

        #endregion
    }
}
