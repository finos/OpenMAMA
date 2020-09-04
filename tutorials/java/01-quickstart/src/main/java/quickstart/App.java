package quickstart;

import com.wombat.mama.Mama;
import com.wombat.mama.MamaBridge;
import com.wombat.mama.MamaQueue;
import com.wombat.mama.MamaTransport;
import com.wombat.mama.MamaDictionary;
import com.wombat.mama.MamaMsg;
import com.wombat.mama.MamaStatus;
import com.wombat.mama.MamaSource;
import com.wombat.mama.MamaSubscription;
import com.wombat.mama.MamaSubscriptionCallback;
import com.wombat.mama.MamaQuality;
import java.util.logging.Level;

class SubscriptionEventHandler implements MamaSubscriptionCallback
{
    @Override
    public void onMsg (final MamaSubscription subscription,
                       final MamaMsg msg) {
        System.out.println("Message Received: " + msg.toString());
    }

    @Override
    public void onError (MamaSubscription subscription,
                         short status,
                         int platformError,
                         String subject,
                         Exception exception) {
        System.err.println("An error occurred creating subscription for " + subject + ": " + MamaStatus.stringForStatus (status));
    }

    @Override
    public void onCreate (MamaSubscription subscription) {
        // You may add your own event handling logic here
    }

    @Override
    public void onQuality (MamaSubscription subscription,
                           short quality,
                           short cause,
                           final Object platformInfo) {
        // You may add your own event handling logic here
    }

    @Override
    public void onDestroy (MamaSubscription subscription)
    {
        // You may add your own event handling logic here
    }

    @Override
    public void onGap (MamaSubscription subscription)
    {
        // You may add your own event handling logic here
    }

    @Override
    public void onRecapRequest (MamaSubscription subscription)
    {
        // You may add your own event handling logic here
    }
};

public class App {

    private static void usageAndExit() {
        System.out.println("This is an OpenMAMA market data subscriber which assumes an");
        System.out.println("OpenMAMA compatible data source is available to connect to and is");
        System.out.println("configured in mama.properties.\n");
        System.out.println("For more information, see http://openmama.org/quickstart.html\n");
        System.out.println("Usage: java -jar 01-quickstart.jar -s [symbol] [arguments]\n");
        System.out.println("Arguments:");
        System.out.println("\t-m [middleware]\tMiddleware bridge to load. Default: [qpid]");
        System.out.println("\t-S [source]\tSource name (prefix) to use. Default: [OM]");
        System.out.println("\t-t [transport]\tTransport from mama.properties to use. Default: [sub]");
        System.out.println("\t-B\t\tDisables dictionary request");
        System.out.println("\t-I\t\tPrevents an intial from being requested");
        System.exit(1);
    }

    public static void main(String[] args) {
        // Parse the command line options to override defaults
        String middlewareName = "qpid";
        String transportName = "sub";
        String sourceName = "OM";
        String symbolName = null;
        String dictionaryFile = "/opt/openmama/data/dictionaries/data.dict";
        boolean requiresDictionary = true;
        boolean requiresInitial = true;

        if (args.length == 0) {
            App.usageAndExit();
        }

        for (int i = 0; i < args.length; i++) {
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
                Mama.enableLogging (Level.FINEST);
                break;
            default:
                App.usageAndExit();
                break;
            }
        }

        // Symbol name is mandatory here, so error if it's null
        if (symbolName == null) {
            App.usageAndExit();
        }

        // Load the requested OpenMAMA middleware bridge (and default payload too)
        MamaBridge bridge = Mama.loadBridge (middlewareName);

        // Time to initialize OpenMAMA's underlying bridges with an "open"
        Mama.open();

        // Get default event queue from the bridge for testing
        MamaQueue queue = Mama.getDefaultQueue(bridge);

        // Set up the required transport on the specified bridge
        MamaTransport transport = new MamaTransport();
        transport.create(transportName, bridge);

        // Set up the data dictionary
        MamaDictionary dictionary = null;
        if (requiresDictionary) {
            dictionary = new MamaDictionary();
            dictionary.populateFromFile(dictionaryFile);
        }

        // Set up the OpenMAMA source (symbol namespace)
        MamaSource source = new MamaSource(sourceName, transport, sourceName);

        // Set up the event handlers for OpenMAMA
        SubscriptionEventHandler eventHandler = new SubscriptionEventHandler();

        // Set up the OpenMAMA Subscription (interest in a topic)
        MamaSubscription subscription = new MamaSubscription();
        subscription.setRequiresInitial(requiresInitial);
        subscription.createSubscription(eventHandler, queue, source, symbolName, eventHandler);
        subscription.activate();

        // Kick off OpenMAMA now (blocking call, non-blocking call also available)
        Mama.start(bridge);

        // Clean up connection on termination
        Mama.close();
    }
}
