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
                  << MamaStatus(status).toString()
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
            "\t-d [dictionary]\tDictionary file to load. Default: [/opt/openmama/data/dictionaries/data.dict]\n"
            "\t-m [middleware]\tMiddleware bridge to load. Default: [qpid]\n"
            "\t-S [source]\tSource name (prefix) to use. Default: [OM]\n"
            "\t-t [transport]\tTransport from mama.properties to use. Default: [sub]\n"
            "\t-v\t\tEnable verbose logging\n"
            "\t-B\t\tDisables dictionary request\n"
            "\t-I\t\tPrevents an intial from being requested\n"
            "\n",
            appName);
    exit(EXIT_SUCCESS);
}

class SubscriptionEventHandler : public MamaSubscriptionCallback
{
public:
    virtual void onMsg (MamaSubscription*      subscription,
                        MamaMsg&               msg) {
        MamaMsgIterator* iterator = new MamaMsgIterator(
            (MamaDictionary*)subscription->getClosure());

        std::cout << std::endl
                  << "+------------------------+--------+--------------+-------------------\n"
                  << "| FIELD NAME             | FID    | TYPE         | VALUE (AS STRING) \n"
                  << "+------------------------+--------+--------------+-------------------\n";

        msg.begin(*iterator);
        while (*(*iterator) != NULL)
        {
            MamaMsgField afield = *(*iterator);

            mama_fid_t fid = afield.getFid();
            const char* fieldType = afield.getTypeName();
            const char* fieldName = afield.getName();
            char fieldValueAsString[1024];

            afield.getAsString(fieldValueAsString, sizeof(fieldValueAsString));

            std::cout << "| ";
            std::cout.width(22);
            std::cout << std::left << fieldName;
            std::cout << " | ";
            std::cout.width(6);
            std::cout << std::left << fid;
            std::cout << " | ";
            std::cout.width(12);
            std::cout << std::left << fieldType;
            std::cout << " | " << std::left << fieldValueAsString;
            std::cout << std::endl;

            ++(*iterator);
        }

        delete iterator;
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
    // Parse the command line options to override defaults
    const char* middlewareName = "qpid";
    const char* transportName = "sub";
    const char* sourceName = "OM";
    const char* symbolName = NULL;
    const char* dictionaryFile = "/opt/openmama/data/dictionaries/data.dict";
    bool requiresDictionary = true;
    bool requiresInitial = true;

    // Parse out command line options
    int i = 0;
    for (i = 1; i < argc; )
    {
        if (strcmp ("-B", argv[i]) == 0)
        {
            requiresDictionary = 0;
        }
        else if (strcmp ("-d", argv[i]) == 0)
        {
            dictionaryFile = argv[++i];
        }
        else if (strcmp ("-I", argv[i]) == 0)
        {
            requiresInitial = 0;
        }
        else if (strcmp ("-m", argv[i]) == 0)
        {
            middlewareName = argv[++i];
        }
        else if (strcmp ("-s", argv[i]) == 0)
        {
            symbolName = argv[++i];
        }
        else if (strcmp ("-S", argv[i]) == 0)
        {
            sourceName = argv[++i];
        }
        else if (strcmp ("-t", argv[i]) == 0)
        {
            transportName = argv[++i];
        }
        else if (strcmp ("-v", argv[i]) == 0)
        {
            // Set up the Log level for OpenMAMA internals
            Mama::setLogLevel(MAMA_LOG_LEVEL_FINEST);
        }
        else if ((strcmp (argv[i], "-h") == 0)||
                 (strcmp (argv[i], "--help") == 0)||
                 (strcmp (argv[i], "-?") == 0))
        {
            usageAndExit(argv[0]);
        }
        else
        {
            fprintf(stderr, "Unknown arg: %s\n",    argv[i]);
            usageAndExit(argv[0]);
        }
        ++i;
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
