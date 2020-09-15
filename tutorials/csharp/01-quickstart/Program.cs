using System;
using System.Collections;
using System.IO;
using System.Runtime.InteropServices;
using System.Threading;
using Wombat;


namespace _01_quickstart
{
    class Program
    {
        private static void usageAndExit() {
            Console.WriteLine("This is an OpenMAMA market data subscriber which assumes an");
            Console.WriteLine("OpenMAMA compatible data source is available to connect to and is");
            Console.WriteLine("configured in mama.properties.\n");
            Console.WriteLine("For more information, see http://openmama.org/quickstart.html\n");
            Console.WriteLine("Usage: dotnet run -s [symbol] [arguments]\n");
            Console.WriteLine("Arguments:");
            Console.WriteLine("\t-d [dictionary]\tDictionary file to load. Default: [/opt/openmama/data/dictionaries/data.dict]");
            Console.WriteLine("\t-m [middleware]\tMiddleware bridge to load. Default: [qpid]");
            Console.WriteLine("\t-S [source]\tSource name (prefix) to use. Default: [OM]");
            Console.WriteLine("\t-t [transport]\tTransport from mama.properties to use. Default: [sub]");
            Console.WriteLine("\t-v\t\tEnable verbose logging");
            Console.WriteLine("\t-B\t\tDisables dictionary request");
            Console.WriteLine("\t-I\t\tPrevents an intial from being requested");
            System.Environment.Exit(0);
        }

        internal class SubscriptionEventHandler : MamaSubscriptionCallback
        {
            public MamaDictionary mDictionary;

            public void onMsg(MamaSubscription subscription, MamaMsg msg)
            {
                MamaMsgIterator iterator = new MamaMsgIterator(mDictionary);

                // Set the iterator to the beginning of the message
                msg.begin(ref iterator);

                Console.WriteLine("\n+------------------------+--------+--------------+-------------------");
                Console.WriteLine("| FIELD NAME             | FID    | TYPE         | VALUE (AS STRING) ");
                Console.WriteLine("+------------------------+--------+--------------+-------------------");

                // Keep going until there are no more fields
                MamaMsgField field = iterator.getField();
                while (field != null)
                {
                    string fieldType = field.getTypeName();
                    string fieldName = field.getName();
                    int fid = field.getFid();
                    string fieldValueAsString = field.getAsString();

                    Console.WriteLine("| {0,-22} | {1,-6} | {2,-12} | {3}",
                                      fieldName,
                                      fid,
                                      fieldType,
                                      fieldValueAsString);


                    iterator++;

                    // Get the next field
                    field = iterator.getField();
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
            String middlewareName = "qpid";
            String transportName = "sub";
            String sourceName = "OM";
            String symbolName = null;
            String dictionaryFile = "/opt/openmama/data/dictionaries/data.dict";
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
                case "-m":
                    middlewareName = args[++i];
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
                case "-v":
                    Mama.enableLogging (MamaLogLevel.MAMA_LOG_LEVEL_FINEST);
                    break;
                default:
                    usageAndExit();
                    break;
                }
            }

            // Symbol name is mandatory here, so error if it's null
            if (symbolName == null) {
                usageAndExit();
            }

            // Load the requested OpenMAMA middleware bridge (and default payload too)
            MamaBridge bridge = Mama.loadBridge(middlewareName);

            // Time to initialize OpenMAMA's underlying bridges with an "open"
            Mama.open();

            // Get default event queue from the bridge for testing
            MamaQueue queue = Mama.getDefaultEventQueue(bridge);

            // Set up the required transport on the specified bridge
            MamaTransport transport = new MamaTransport();
            transport.create(transportName, bridge);

            // Set up the data dictionary
            MamaDictionary dictionary = null;
            if (requiresDictionary) {
                dictionary = new MamaDictionary();
                dictionary.create(dictionaryFile);
            }

            // Set up the OpenMAMA source (symbol namespace)
            MamaSource source = new MamaSource();
            source.symbolNamespace = sourceName;
            source.transport = transport;

            // Set up the event handlers for OpenMAMA
            SubscriptionEventHandler eventHandler = new SubscriptionEventHandler();
            eventHandler.mDictionary = dictionary;

            // Set up the OpenMAMA Subscription (interest in a topic)
            MamaSubscription subscription = new MamaSubscription();
            subscription.setRequiresInitial(requiresInitial);
            subscription.create(queue, eventHandler, source, symbolName);

            // Kick off OpenMAMA now (blocking call, non-blocking call also available)
            Mama.start(bridge);

            // Clean up connection on termination
            Mama.close();
        }
    }
}
