#include <stdio.h>
#include <mama/mama.h>


// Rudimentary error checking mechanism for brevity
void checkStatusAndExitOnFailure(mama_status status, const char* scenario) {
    if (status != MAMA_STATUS_OK) {
        fprintf(stderr,
                "Failure encountered during %s: %s",
                scenario,
                mamaStatus_stringForStatus(status));
        exit(EXIT_FAILURE);
    }
}

void usageAndExit(const char* appName) {
    fprintf(stderr,
            "This is an OpenMAMA market data subscriber which assumes an\n"
            "OpenMAMA compatible data source is available to connect to and is\n"
            "configured in mama.properties.\n\n"
            "For more information, see http://openmama.org/quickstart.html\n\n"
            "Usage: %s -s [symbol] [arguments]\n\n"
            "Arguments:\n"
            "\t-m [middleware]\tMiddleware bridge to load. Default: [qpid]\n"
            "\t-S [source]\tSource name (prefix) to use. Default: [OM]\n"
            "\t-t [transport]\tTransport from mama.properties to use. Default: [sub]\n"
            "\t-B\t\tDisables dictionary request\n"
            "\t-I\t\tPrevents an intial from being requested\n"
            "\n",
            appName);
    exit(EXIT_FAILURE);
}

void MAMACALLTYPE
subscriptionOnMsg  (mamaSubscription    subscription,
                    mamaMsg             msg,
                    void*               closure,
                    void*               itemClosure) {
    fprintf(stdout, "Message Received: %s\n", mamaMsg_toString(msg));
}

void MAMACALLTYPE
subscriptionOnError (mamaSubscription   subscription,
                     mama_status        status,
                     void*              platformError,
                     const char*        subject,
                     void*              closure)
{
    fprintf(stdout,
            "An error occurred creating subscription for %s: %s\n",
            subject ? subject : "(null)",
            mamaStatus_stringForStatus (status));
}

void MAMACALLTYPE
subscriptionOnCreate (mamaSubscription  subscription, void* closure)
{
    // You may add your own event handling logic here
}

void MAMACALLTYPE
subscriptionOnQuality (mamaSubscription subsc,
                       mamaQuality      quality,
                       const char*      symbol,
                       short            cause,
                       const void*      platformInfo,
                       void*            closure)
{
    fprintf(stdout,
            "Quality changed to %s for %s, cause %d\n",
            mamaQuality_convertToString (quality), symbol, cause);
}

int main(int argc, char* argv[])
{
    // OpenMAMA status variable which we will use throughout this example
    mama_status status;

    // This is the OpenMAMA bridge which abstracts the middleware for the app
    mamaBridge bridge;

    // The OpenMAMA queue which we will use. You can also create your own
    mamaQueue queue;

    // The OpenMAMA transport configured in $WOMBAT_PATH/mama.properties
    mamaTransport transport;

    // Optional reference to the OpenMAMA dictionary (id to field mapping)
    mamaDictionary dictionary;

    // The OpenMAMA source for market data requests
    mamaSource source;

    // The OpenMAMA subscription reference
    mamaSubscription subscription;

    // This is a set of callbacks which we'll populate here
    mamaMsgCallbacks callbacks;

    // NULL initialize any stack structs for safety
    memset(&callbacks, 0, sizeof(callbacks));

    // Parse the command line options to override defaults
    const char* middlewareName = "qpid";
    const char* transportName = "sub";
    const char* sourceName = "OM";
    const char* symbolName = NULL;
    const char* dictionaryFile = "/opt/openmama/data/dictionaries/data.dict";
    int requiresDictionary = 1;
    int requiresInitial = 1;

    // Parse out command line options
    int opt;
    while((opt = getopt(argc, argv, ":Bd:Im:s:S:t:v")) != -1) {
        switch(opt) {
            case 'B':
                requiresDictionary = 0;
                break;
            case 'd':
                dictionaryFile = optarg;
                break;
            case 'I':
                requiresInitial = 0;
                break;
            case 'm':
                middlewareName = optarg;
                break;
            case 's':
                symbolName = optarg;
                break;
            case 'S':
                sourceName = optarg;
                break;
            case 't':
                transportName = optarg;
                break;
            case 'v':
                // Set up the Log level for OpenMAMA internals
                mama_setLogLevel(MAMA_LOG_LEVEL_FINEST);
                break;
            default:
                fprintf(stderr, "Encountered '%c'\n\n", optopt);
                usageAndExit(argv[0]);
                break;
        }
    }

    // Symbol name is mandatory here, so error if it's NULL
    if (symbolName == NULL) {
        usageAndExit(argv[0]);
    }

    // Load the requested OpenMAMA middleware bridge (and default payload too)
    status = mama_loadBridge(&bridge, middlewareName);
    checkStatusAndExitOnFailure(status, "mama_loadBridge");

    // Time to initialize OpenMAMA's underlying bridges with an "open"
    status = mama_open();
    checkStatusAndExitOnFailure(status, "mama_open");

    // Get default event queue from the bridge for testing
    status = mama_getDefaultEventQueue(bridge, &queue);
    checkStatusAndExitOnFailure(status, "mama_getDefaultEventQueue");

    // Set up the required transport on the specified bridge
    status = mamaTransport_allocate(&transport);
    checkStatusAndExitOnFailure(status, "mamaTransport_allocate");
    status = mamaTransport_create(transport, transportName, bridge);
    checkStatusAndExitOnFailure(status, "mamaTransport_create");

    // Set up the data dictionary
    if (requiresDictionary) {
        status = mamaDictionary_create (&dictionary);
        checkStatusAndExitOnFailure(status, "mamaDictionary_create");
        status = mamaDictionary_populateFromFile(dictionary, dictionaryFile);
        checkStatusAndExitOnFailure(status, "mamaDictionary_populateFromFile");
    } else {
        dictionary = NULL;
    }

    // Set up the OpenMAMA source (symbol namespace)
    status = mamaSource_create(&source);
    checkStatusAndExitOnFailure(status, "mamaSource_create");
    status = mamaSource_setSymbolNamespace(source, sourceName);
    checkStatusAndExitOnFailure(status, "mamaSource_setSymbolNamespace");
    status = mamaSource_setTransport(source, transport);
    checkStatusAndExitOnFailure(status, "mamaSource_setTransport");

    // Set up the event handlers for OpenMAMA
    callbacks.onMsg = subscriptionOnMsg;
    callbacks.onCreate = subscriptionOnCreate;
    callbacks.onError = subscriptionOnError;

    // Set up the OpenMAMA Subscription (interest in a topic)
    status = mamaSubscription_allocate(&subscription);
    checkStatusAndExitOnFailure(status, "mamaSubscription_allocate");
    status = mamaSubscription_create(
            subscription,
            queue,
            &callbacks,
            source,
            symbolName,
            dictionary
    );
    checkStatusAndExitOnFailure(status, "mamaSubscription_create");
    status = mamaSubscription_setRequiresInitial(subscription, requiresInitial);
    checkStatusAndExitOnFailure(status, "mamaSubscription_setRequiresInitial");
    status = mamaSubscription_activate(subscription);
    checkStatusAndExitOnFailure(status, "mamaSubscription_activate");

    // Kick off OpenMAMA now (blocking call, non-blocking call also available)
    status = mama_start(bridge);
    checkStatusAndExitOnFailure(status, "mama_start");

    // Clean up connection on termination
    status = mama_close();
    checkStatusAndExitOnFailure(status, "mama_close");
}
