using System;
using System.Collections;
using System.IO;
using System.Runtime.InteropServices;
using System.Threading;
using Wombat;


namespace _02_resourcepool
{
    class Program
    {
        private static void usageAndExit() {
            Console.WriteLine("This is an OpenMAMA market data subscriber which assumes an");
            Console.WriteLine("OpenMAMA compatible data source is available to connect to and is");
            Console.WriteLine("configured in mama.properties.\n");
            Console.WriteLine("Usage: java -jar 01-quickstart.jar -u [uri]\n");
            Console.WriteLine("Arguments:");
            Console.WriteLine("\t-d [dictionary]\tDictionary file to load. Default: [/opt/openmama/data/dictionaries/data.dict]");
            Console.WriteLine("\t-m [middleware]\tMiddleware bridge to load. Default: [qpid]");
            Console.WriteLine("\t-S [source]\tSource name (prefix) to use. Default: [OM]");
            Console.WriteLine("\t-t [transport]\tTransport from mama.properties to use. Default: [sub]");
            Console.WriteLine("\t-u [uri]\tURI representing subscription in `bridge://transport/source/topic` format\n");
            Console.WriteLine("\t-v\t\tEnable verbose logging");
            Console.WriteLine("\t-B\t\tDisables dictionary request");
            Console.WriteLine("\t-I\t\tPrevents an intial from being requested");
            System.Environment.Exit(0);
        }

        internal class SubscriptionEventHandler : MamaSubscriptionCallback
        {
            public MamaDictionary mDictionary;
            private static int mMsgReceived = 0;

            public void onMsg(MamaSubscription subscription, MamaMsg msg)
            {
                Console.WriteLine(msg.ToJsonString(mDictionary));
                if (++mMsgReceived == 10) {
                    Mama.stopAll();
                }
            }

            public void onError(MamaSubscription subscription, MamaStatus.mamaStatus status, string subject)
            {
                Console.WriteLine("An error occurred creating subscription for " + subject + ": " + status.ToString());
            }

            public void onCreate(MamaSubscription subscription)
            {
                // You may add your own event handling logic here
            }

            public void onQuality(MamaSubscription subscription, mamaQuality quality, string symbol)
            {
                // You may add your own event handling logic here
            }

            public void onDestroy(MamaSubscription subscription)
            {
                // You may add your own event handling logic here
            }

            public void onGap(MamaSubscription subscription)
            {
                // You may add your own event handling logic here
            }
            public void onRecapRequest(MamaSubscription subscription)
            {
                // You may add your own event handling logic here
            }
        }

        static void Main(string[] args)
        {
            // Parse the command line options to override defaults
            string transportName = "sub";
            string sourceName = "OM";
            string symbolName = null;
            string dictionaryFile = "/opt/openmama/data/dictionaries/data.dict";
            string uri = null;
            bool requiresDictionary = true;
            bool requiresInitial = true;

            if (args.Length == 0) {
                usageAndExit();
            }

            for (int i = 0; i < args.Length; i++) {
                switch (args[i]) {
                case "-B":
                    requiresDictionary = false;
                    break;
                case "-d":
                    dictionaryFile = args[++i];
                    break;
                case "-I":
                    requiresInitial = false;
                    break;
                case "-s":
                    symbolName = args[++i];
                    break;
                case "-S":
                    sourceName = args[++i];
                    break;
                case "-t":
                    transportName = args[++i];
                    break;
                case "-u":
                    uri = args[++i];
                    break;
                case "-v":
                    Mama.enableLogging (MamaLogLevel.MAMA_LOG_LEVEL_FINEST);
                    break;
                default:
                    usageAndExit();
                    break;
                }
            }

            // Symbol name is mandatory here, so error if it's null
            if (symbolName == null && uri == null) {
                usageAndExit();
            }

            MamaResourcePool pool = new MamaResourcePool("default");

            // Set up the data dictionary
            MamaDictionary dictionary = null;
            if (requiresDictionary) {
                dictionary = new MamaDictionary();
                dictionary.create(dictionaryFile);
            }

            // Set up the event handlers for OpenMAMA
            SubscriptionEventHandler eventHandler = new SubscriptionEventHandler();
            eventHandler.mDictionary = dictionary;

            // The OpenMAMA subscription reference to be populated
            MamaSubscription subscription;
            if (null != uri) {
                subscription = pool.createSubscriptionFromUri (
                    uri,
                    eventHandler,
                    dictionary);
            } else {
                subscription = pool.createSubscriptionFromComponents (
                    transportName,
                    sourceName,
                    symbolName,
                    eventHandler,
                    dictionary);
            }

            if (requiresInitial) {
                subscription.setRequiresInitial (requiresInitial);
            }

            subscription.activate();
            Mama.startAll();
        }
    }
}
