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
using System.Diagnostics;
using System.Globalization;

namespace Wombat
{
    /// <summary>
    /// This sample application demonstrates how to publish mama messages, and
    /// respond to requests from a clients inbox.
    ///
    /// It accepts the following command line arguments:
    /// [-s topic]         The topic on which to send messages. Default value
    /// is "MAMA_TOPIC".
    /// [-l topic]         The topic on which to listen for inbound requests.
    /// Default is "MAMA_INBOUND_TOPIC".
    /// [-i interval]      The interval between messages .Default, 0.5.
    /// [-m name]          The middleware to be used. Default, wmw.
    /// [-tport name]      The transport parameters to be used from
    /// mama.properties.
    /// [-q]               Quiet mode. Suppress output.
    /// [-c number]        How many messages to publish (default infinite).
    /// </summary>
    class EntryPoint
    {
        [STAThread]
        static int Main(string[] args)
        {
            try
            {
                return new MamaPublisherCS(args).Run();
            }
            catch (Exception e)
            {
                Console.WriteLine("Error occurred: {0}\r\nDetails:\r\n{1}",
                        e.Message,
                        e.ToString());
                return 1;
            }
        }
    }

    class MamaPublisherCS : MamaPublisher, MamaSubscriptionCallback, MamaTimerCallback
    {
        public MamaPublisherCS(string[] args)
        {
            this.args = args;
        }

        public int Run()
        {
            ParseArgs();
            if (helpNeeded)
            {
                DisplayUsage();
                return 0;
            }

            Mama.logToFile(@"mama.log", MamaLogLevel.MAMA_LOG_LEVEL_FINEST);
            bridge = Mama.loadBridge(middlewareName);
            Mama.open();
            MamaTransport transport = new MamaTransport();
            MamaQueue defaultQueue = Mama.getDefaultEventQueue(bridge);
            MamaTimer timer = new MamaTimer();
            MamaSubscription subscription = new MamaSubscription();
            MamaPublisherCS publisher = this;

            transport.create(transportName, bridge);
            timer.create(defaultQueue, publisher, interval, null);
            subscription.createBasic(transport, defaultQueue, publisher, inboundTopic);
            publisher.create(transport, outboundTopic);
            msg = new MamaMsg();

            Mama.start(bridge);
            Mama.close();
            return 0;
        }

        public void onCreate(MamaSubscription subscription)
        {
            if (!quiet)
            {
                Console.WriteLine("Created inbound subscription.");
            }
        }

        public void onError(MamaSubscription subscription, MamaStatus.mamaStatus status, string subject)
        {
            Console.WriteLine("Error creating subscription: {0}", MamaStatus.stringForStatus(status));
            Exit(1);
        }

        public void onQuality(MamaSubscription subscription, mamaQuality quality, string symbol)
        {
            if (!quiet)
            {
                Console.WriteLine("Subscription {0} quality changed to {1}", symbol, quality);
            }
        }

        public void onMsg(MamaSubscription subscription, MamaMsg msg)
        {
            if (!quiet)
            {
                Console.WriteLine("Received inbound msg. Sending response");
            }

            if (!msg.isFromInbox)
            {
                Console.WriteLine("Inbound msg not from inbox. No reply sent.");
                return;
            }

            Publish(msg);
        }

        public void onGap (MamaSubscription  subscription)
        {
            Console.WriteLine("Subscription gap");
        }

        public void onRecapRequest (MamaSubscription subscription)
        {
            Console.WriteLine("Subscription recap request");
        }

        public void onDestroy(MamaSubscription subscription)
        {
        }

        public void onTimer(MamaTimer mamaTimer, object closure)
        {
            Publish(null);
            if (messageCount_ > 0 && --messageCount_ == 0)
            {
                Mama.stop(bridge);
                return;
            }
        }

        public void onDestroy(MamaTimer mamaTimer, object closure)
        {
        }

        private int messageNumber_ = 0;
        private void Publish(MamaMsg request)
        {
            try
            {
                msg.clear();

                /* Add some fields. This is not required, but illustrates how to
                * send data.
                */
                msg.addI32("MessageNo", 1, messageNumber_++);
                msg.addString("PublisherTopic", 2, outboundTopic);

                if (request != null)
                {
                    if (!quiet)
                    {
                        Console.WriteLine("Publishing message to inbox.");
                    }
                    sendReplyToInbox(request, msg);
                }
                else
                {
                    if (!quiet)
                    {
                        Console.WriteLine("Publishing message to {0}.", outboundTopic);
                    }
                    send(msg);
                }
            }
            catch (MamaException e)
            {
                Console.WriteLine("Error publishing message: {0}.\r\nDetails:\r\n{1}", e.Message, e.ToString());
                Exit(1);
            }
        }

        private static void Exit(int errorCode)
        {
            Environment.ExitCode = errorCode;
            Process.GetCurrentProcess().Kill();
        }

        private void ParseArgs()
        {
            for (int i = 0; i < args.Length; )
            {
                string arg = args[i];
                if (arg[0] != '-')
                {
                    Console.WriteLine("Ignoring invalid argument {0}", arg);
                    ++i;
                    continue;
                }
                string opt = arg.Substring(1);
                switch (opt)
                {
                    case "s":
                        if ((i + 1) == args.Length)
                        {
                            Console.WriteLine("Expecting outbound topic name after {0}", arg);
                            ++i;
                            continue;
                        }
                        outboundTopic = args[++i];
                        break;
                    case "l":
                        if ((i + 1) == args.Length)
                        {
                            Console.WriteLine("Expecting inbound topic name after {0}", arg);
                            ++i;
                            continue;
                        }
                        inboundTopic = args[++i];
                        break;
                    case "c":
                        if ((i + 1) == args.Length)
                        {
                            Console.WriteLine("Expecting message count after {0}", arg);
                            ++i;
                            continue;
                        }
                        try
                        {
                            messageCount_ = int.Parse(args[++i], NumberStyles.Integer, CultureInfo.InvariantCulture);
                        }
                        catch // ignore parse error
                        {
                        }
                        break;
                    case "i":
                        if ((i + 1) == args.Length)
                        {
                            Console.WriteLine("Expecting interval after {0}", arg);
                            ++i;
                            continue;
                        }
                        double.TryParse(args[++i], NumberStyles.Float, CultureInfo.InvariantCulture, out interval);
                        break;
                    case "h":
                    case "?":
                        helpNeeded = true;
                        return;
                    case "m":
                        if ((i + 1) == args.Length)
                        {
                            Console.WriteLine("Expected middleware name after {0}", arg);
                            ++i;
                            continue;
                        }
                        middlewareName = args[++i];
                        break;
                    case "tport":
                        if ((i + 1) == args.Length)
                        {
                            Console.WriteLine("Expecting transport name after {0}", arg);
                            ++i;
                            continue;
                        }
                        transportName = args[++i];
                        break;
                    case "q":
                        quiet = true;
                        break;
                    case "v":
                        if (logLevel == MamaLogLevel.MAMA_LOG_LEVEL_WARN)
                        {
                            logLevel = MamaLogLevel.MAMA_LOG_LEVEL_NORMAL;
                            Mama.enableLogging (logLevel);
                        }
                        else if (logLevel == MamaLogLevel.MAMA_LOG_LEVEL_NORMAL)
                        {
                            logLevel = MamaLogLevel.MAMA_LOG_LEVEL_FINE;
                            Mama.enableLogging (logLevel);
                        }
                        else if (logLevel == MamaLogLevel.MAMA_LOG_LEVEL_FINE)
                        {
                            logLevel = MamaLogLevel.MAMA_LOG_LEVEL_FINER;
                            Mama.enableLogging (logLevel);
                        }
                        else
                        {
                            logLevel = MamaLogLevel.MAMA_LOG_LEVEL_FINEST;
                            Mama.enableLogging (logLevel);
                        }
                        break;
                    default:
                        Console.WriteLine("Ignoring invalid option {0}", arg);
                        break;
                }
                ++i;
            }

            Console.WriteLine("Starting Publisher with:\n" +
                "   topic:              {0}\n" +
                "   inbound topic:      {1}\n" +
                "   interval            {2}\n" +
                "   middleware          {3}\n" +
                "   transport:          {4}\n",
                outboundTopic, inboundTopic, interval, middlewareName, transportName);
        }

        private void DisplayUsage()
        {
            Console.WriteLine(usage_);
        }

        private string[] args;
        private int messageCount_ = 0; // infinite by default
        private string outboundTopic = "MAMA_TOPIC";
        private string inboundTopic = "MAMA_INBOUND_TOPIC";
        private string middlewareName = "wmw";
        private string transportName = "pub"; // tib_rvd
        private double interval = 0.5;
        private MamaLogLevel logLevel = MamaLogLevel.MAMA_LOG_LEVEL_WARN;
        private bool helpNeeded = false;
        private bool quiet = false;
        private MamaBridge bridge;
        private MamaMsg msg;

        private const string usage_ = @"
This sample application demonstrates how to publish mama messages, and
respond to requests from a clients inbox.

It accepts the following command line arguments:
     [-s topic]         The topic on which to send messages. Default value
                        is ""MAMA_TOPIC"".
     [-l topic]         The topic on which to listen for inbound requests.
                        Default is ""MAMA_INBOUND_TOPIC"".
     [-i interval]      The interval between messages .Default, 0.5.
     [-tport name]      The transport parameters to be used from
                        mama.properties. Default is pub.
     [-q]               Quiet mode. Suppress output.
     [-c number]        How many messages to publish (default infinite).
";
    }
}
