#include <stdio.h>
#include <mama/mama.h>
#include <mama/resourcepool.h>

// Rudimentary error checking mechanism for brevity
static void checkStatusAndExitOnFailure(mama_status status, const char* scenario) {
    if (status != MAMA_STATUS_OK) {
        fprintf(stderr,
                "Failure encountered during %s: %s",
                scenario,
                mamaStatus_stringForStatus(status));
        exit(EXIT_SUCCESS);
    }
}

static void usageAndExit(const char* appName) {
    fprintf(stderr,
             "This is an OpenMAMA market data subscriber which assumes an\n"
             "OpenMAMA compatible data source is available to connect to and is\n"
             "configured in mama.properties.\n\n"
             "Usage: %s -u [uri]\n\n"
             "Arguments:\n"
             "\t-d [dictionary]\tDictionary file to load. Default: [/opt/openmama/data/dictionaries/data.dict]\n"
             "\t-S [source]\tSource name (prefix) to use. Default: [OM]\n"
             "\t-t [transport]\tTransport from mama.properties to use. Default: [sub]\n"
             "\t-u [uri]\tURI representing subscription in `bridge://transport/source/topic` format\n"
             "\t-v\t\tEnable verbose logging\n"
             "\t-B\t\tDisables dictionary request\n"
             "\t-I\t\tPrevents an initial from being requested\n"
             "\n",
            appName);
    exit(EXIT_SUCCESS);
}

static void MAMACALLTYPE
subscriptionOnMsg  (mamaSubscription    subscription,
                    mamaMsg             msg,
                    void*               closure,
                    void*               itemClosure) {
//    static int msgsReceived = 0;
//    if (++msgsReceived == 10) {
//        mama_stopAll ();
//    }
    fprintf (stdout,
             "Message Received: %s\n",
             mamaMsg_toJsonStringWithDictionary (msg, closure));
}

int main(int argc, char* argv[])
{
    // OpenMAMA status variable which we will use throughout this example
    mama_status status;
    const char* transportName = "sub";
    const char* sourceName = "OM";
    const char* symbolName = NULL;
    const char* uri = NULL;
    const char* dictionaryFile = "/opt/openmama/data/dictionaries/data.dict";
    int requiresDictionary = 1;
    int requiresInitial = 1;

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
        else if (strcmp ("-u", argv[i]) == 0)
        {
            uri = argv[++i];
        }
        else if (strcmp ("-v", argv[i]) == 0)
        {
            // Set up the Log level for OpenMAMA internals
            mama_setLogLevel(MAMA_LOG_LEVEL_FINEST);
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

    // Set up the data dictionary
    mamaDictionary dictionary = NULL;
    if (requiresDictionary)
    {
        status = mamaDictionary_create (&dictionary);
        checkStatusAndExitOnFailure (status, "mamaDictionary_create");
        status = mamaDictionary_populateFromFile (dictionary, dictionaryFile);
        checkStatusAndExitOnFailure (status, "mamaDictionary_populateFromFile");
    }

    mamaResourcePool pool;
    mamaResourcePool_create (&pool, "default");

    // This is a set of callbacks which we'll NULL set here
    mamaMsgCallbacks callbacks;
    memset(&callbacks, 0, sizeof(callbacks));

    // Only the onMsg callback is mandatory in mamaResourcePool
    callbacks.onMsg = subscriptionOnMsg;

    // The OpenMAMA subscription reference to be populated
    mamaSubscription subscription;
    if (NULL != uri) {
        status = mamaResourcePool_createSubscriptionFromUri (
            pool,
            &subscription,
            uri,
            callbacks,
            dictionary);
        checkStatusAndExitOnFailure(
            status, "mamaResourcePool_createSubscriptionFromUri");
    } else {
        status = mamaResourcePool_createSubscriptionFromComponents (
            pool,
            &subscription,
            transportName,
            sourceName,
            symbolName,
            callbacks,
            dictionary);
        checkStatusAndExitOnFailure(
            status, "mamaResourcePool_createSubscriptionFromComponents");
    }

    if (requiresInitial) {
        mamaSubscription_setRequiresInitial (subscription, requiresInitial);
    }

    mamaSubscription_activate (subscription);

    // Block here for demonstration purposes
    mama_startAll (MAMA_BOOL_TRUE);

    // Clean up connection on termination
    mamaResourcePool_destroy (pool);
    mamaDictionary_destroy (dictionary);

    mama_close();
}
