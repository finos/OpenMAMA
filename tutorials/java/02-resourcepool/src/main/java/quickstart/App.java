package quickstart;

import com.wombat.mama.*;

import java.net.MalformedURLException;
import java.net.URISyntaxException;
import java.util.logging.Level;


class SubscriptionEventHandler implements MamaSubscriptionCallback
{
    public MamaDictionary mDictionary;

    private static int mMsgReceived = 0;

    @Override
    public void onMsg (final MamaSubscription subscription,
                       final MamaMsg msg) {
        MamaDictionary dictionary = (MamaDictionary) subscription.getClosure();
        System.out.println(msg.toJsonString(dictionary));
//        if (++mMsgReceived == 10) {
//            Mama.stopAll();
//        }
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
        System.out.println("Usage: java -jar 01-quickstart.jar -u [uri]\n");
        System.out.println("Arguments:");
        System.out.println("\t-d [dictionary]\tDictionary file to load. Default: [/opt/openmama/data/dictionaries/data.dict]");
        System.out.println("\t-m [middleware]\tMiddleware bridge to load. Default: [qpid]");
        System.out.println("\t-S [source]\tSource name (prefix) to use. Default: [OM]");
        System.out.println("\t-t [transport]\tTransport from mama.properties to use. Default: [sub]");
        System.out.println("\t-u [uri]\tURI representing subscription in `bridge://transport/source/topic` format\n");
        System.out.println("\t-v\t\tEnable verbose logging");
        System.out.println("\t-B\t\tDisables dictionary request");
        System.out.println("\t-I\t\tPrevents an intial from being requested");
        System.exit(0);
    }

    public static void main(String[] args) throws MalformedURLException, URISyntaxException {
        // Parse the command line options to override defaults
        String transportName = "sub";
        String sourceName = "OM";
        String symbolName = null;
        String dictionaryFile = "/opt/openmama/data/dictionaries/data.dict";
        String uri = null;
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
                Mama.enableLogging (Level.FINEST);
                break;
            default:
                App.usageAndExit();
                break;
            }
        }

        // Symbol name is mandatory here, so error if it's null
        if (symbolName == null && uri == null) {
            App.usageAndExit();
        }

        MamaResourcePool pool = new MamaResourcePool("default");

        // Set up the data dictionary
        MamaDictionary dictionary = null;
        if (requiresDictionary) {
            dictionary = new MamaDictionary();
            dictionary.populateFromFile(dictionaryFile);
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
