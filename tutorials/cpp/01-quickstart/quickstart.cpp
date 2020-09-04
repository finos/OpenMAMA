#include <cstdio>
#include <iostream>
#include <mama/mamacpp.h>


using namespace Wombat;

// Rudimentary error checking mechanism for brevity
void checkStatusAndExitOnFailure(mama_status status, const char* scenario) {
    if (status != MAMA_STATUS_OK) {
        std::cerr << "Failure encountered during "
                  << scenario 
                  << ": "
                  << MamaStatus(status).toString())
                  << std::endl;
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

class SubscriptionEventHandler : public MamaSubscriptionCallback
{
public:
    virtual void onMsg (MamaSubscription*      subscription,
                        MamaMsg&               msg) {
        std::cout << "Message Received: " << msg.toString() << std::endl;
    }

    virtual void onError (MamaSubscription*    subscription,
                          const MamaStatus&    status,
                          const char*          subject) {
        std::cerr << "An error occurred creating subscription for "
                  << (subject ? subject : "(null)")
                  << ": "
                  << status.toString()
                  << std::endl;
    }

    virtual void onCreate (MamaSubscription*   subscription) {
        // You may add your own event handling logic here
    }

    virtual void onQuality (MamaSubscription*  subscription,
                            mamaQuality        quality,
                            const char*        symbol,
                            short              cause,
                            const void*        platformInfo) {
        // You may add your own event handling logic here
    }
};

int main(int argc, char* argv[])
{
    // OpenMAMA status variable which we will use throughout this example
    mama_status status;

    // Parse the command line options to override defaults
    const char* middlewareName = "qpid";
    const char* transportName = "sub";
    const char* sourceName = "OM";
    const char* symbolName = NULL;
    const char* dictionaryFile = "/opt/openmama/data/dictionaries/data.dict";
    bool requiresDictionary = true;
    bool requiresInitial = true;

    // Parse out command line options
    int opt;
    while((opt = getopt(argc, argv, ":Bd:Im:s:S:t:v")) != -1) {
        switch(opt) {
            case 'B':
                requiresDictionary = false;
                break;
            case 'd':
                dictionaryFile = optarg;
                break;
            case 'I':
                requiresInitial = false;
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
                Mama::setLogLevel(MAMA_LOG_LEVEL_FINEST);
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

    try {
        // Load the requested OpenMAMA middleware bridge (and default payload too)
        mamaBridge bridge = Mama::loadBridge (middlewareName);

        // Time to initialize OpenMAMA's underlying bridges with an "open"
        Mama::open();

        // Get default event queue from the bridge for testing
        auto queue = Mama::getDefaultEventQueue(bridge);

        // Set up the required transport on the specified bridge
        auto transport = new MamaTransport();
        transport->create(transportName, bridge);

        // Set up the data dictionary
        MamaDictionary* dictionary = nullptr;
        if (requiresDictionary) {
            dictionary = new MamaDictionary();
            dictionary->populateFromFile(dictionaryFile);
        }

        // Set up the OpenMAMA source (symbol namespace)
        auto source = new MamaSource(sourceName, transport, sourceName);

        // Set up the event handlers for OpenMAMA
        auto eventHandler = new SubscriptionEventHandler();

        // Set up the OpenMAMA Subscription (interest in a topic)
        auto subscription = new MamaSubscription();
        subscription->setRequiresInitial(requiresInitial);
        subscription->create(queue, new SubscriptionEventHandler(), source, symbolName, dictionary);
        subscription->activate();

        // Kick off OpenMAMA now (blocking call, non-blocking call also available)
        Mama::start(bridge);

        // Clean up connection on termination
        Mama::close();
    } catch (MamaStatus const & status) {
        std::cerr << "Exception MamaStatus: " << status.toString () << std::endl;
    } catch (...) {
        std::cerr << "Unexpected exception found!" << std::endl;
    }
}
