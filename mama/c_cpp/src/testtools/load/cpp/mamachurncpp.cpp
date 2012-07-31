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

#include "mamachurn.h"

using namespace Wombat;

typedef struct
{
    char*              symbol;
    MamaSubscription*  sub;
    MamaQueue*         queue;
} tArrayItem;

#define MAX_SYMBOLS 250000

static MamaTransport* gTransport;
static MamaSource*    gSource;
static MamaTimer*     gChurnTimer;
static MamaTimer*     gStatsTimer;
static MamaTimer*     gLifeTimer;
static MamaQueue*     gDefaultQueue;
static mamaBridge     gBridge;
static const char*    gMiddleware = "wmw";

static int gThrottle     = 0;
static int gNumToChurn   = 0;
static int gChurnTimeout = 2;
static int gStatsTimeout = 2;
static int gVerbosity    = 0;
static int gSymbolCount;

static tArrayItem gSymbolArray[MAX_SYMBOLS];

static int              gThreads = 0;
static MamaQueue*       gQueues;             
static MamaDispatcher*  gDispatchers;        

/* Stats counters */
static int gCreateMsgCount    = 0;
static int gInitialMsgCount   = 0;
static int gRecapMsgCount     = 0;
static int gOtherMsgCount     = 0;
static int gErrorCount        = 0;
static int gChurned           = 0;
static int gGapCount          = 0;
static int gGroupSubscription = 0;

static const char* gUsageString[]=
{
"Subscribe to an arbitrary number of symbols and, every second, destroy and",
"recreate a configurable number of subscriptions",
"",
"Usage: mamachurnc -S symbolNamespace -tport transport [options]",
"",
"Options:",
"-c|-churn churnRate  churns per interval",
"-f symbolFile        file containing a list of symbols to subscribe to",
"-k|-kill lifetime    kill program after specified time (in seconds)",
"-s symbol            symbol to subscribe to",
"-S                   symbol name space",
"-g                   create group subscription(s)",
"-m                   The middleware to use [wmw/lbm/tibrv]. Default is wmw.",
"-tport               name of the transport being used",
"-t|-timeout seconds  churn interval",
"-stats               stats interval",
"-v                   increase verbosity (mama)",
"-V                   increase verbosity (this program). Pass up to three times",
NULL
};

static void ClearCounters(void)
{
    gInitialMsgCount = 0;
    gCreateMsgCount  = 0;
    gOtherMsgCount   = 0;
    gRecapMsgCount   = 0;
    gChurned         = 0;
    gErrorCount      = 0;
    gGapCount        = 0;
}

#define CHURN_LOG_LEVEL_NORMAL 0
#define CHURN_LOG_LEVEL_FINE   1
#define CHURN_LOG_LEVEL_FINER  2
#define CHURN_LOG_LEVEL_FINEST 3


#define STATS_FIELD_WIDTH -5 /* negative = left alignment */

void StatsCallback::onTimer (MamaTimer* timer)
{
    time_t  t;
    struct tm tmResult;
    char timeBuf[32];

    time(&t);

    localtime_r(&t, &tmResult);

    strftime(timeBuf, sizeof timeBuf, "%Y-%m-%d %H:%M:%S", &tmResult);

    printf("%s  Churned %*d Initials %*d Creates %*d Recaps %*d OtherMsg %*d Error %*d Gaps %*d\n",
        timeBuf,
        STATS_FIELD_WIDTH, gChurned,
        STATS_FIELD_WIDTH, gInitialMsgCount,
        STATS_FIELD_WIDTH, gCreateMsgCount,
        STATS_FIELD_WIDTH, gRecapMsgCount,
        STATS_FIELD_WIDTH, gOtherMsgCount,
        STATS_FIELD_WIDTH, gErrorCount,
        STATS_FIELD_WIDTH, gGapCount);
    ClearCounters();
}

static void usage (int exitStatus)
{
    int i = 0;
    while (gUsageString[i]!=NULL)
    {
        printf("%s\n", gUsageString[i++]);
    }
    exit(exitStatus);
}

static void die (const char *fmt, ...)
{
    va_list ap;

    va_start (ap, fmt);
    printf   ("Fatal Error: ");
    vprintf  (fmt, ap);
    printf   ("\n");
    va_end   (ap);

    exit(EXIT_FAILURE);
}

void SubscriptionCallbacks::onCreate (MamaSubscription* subscription)
{
    const char* symbol = NULL;

    gCreateMsgCount++;
    if (gVerbosity >= CHURN_LOG_LEVEL_FINEST)
    {
        symbol = subscription->getSymbol();
        printf("%s Type: Create\n", NULL==symbol ? "" : symbol);
    }
}

void SubscriptionCallbacks::onError (MamaSubscription*   subscription,
                                     const MamaStatus&   status,
                                     const char*         subject)
{
    gErrorCount++;
    if (gVerbosity >= CHURN_LOG_LEVEL_FINEST)
    {
        printf("subscription error, %s: %s\n",  subject, status.toString());
    }
    subscription->activate();
}

void SubscriptionCallbacks::onMsg (MamaSubscription* subscription,
                                   MamaMsg&          msg)
{
    mamaMsgType msgType = msg.getType();
    const char* symbol = NULL;

    switch (msgType)
    {
        case MAMA_MSG_TYPE_INITIAL:
          gInitialMsgCount++;
        break;
        
        case  MAMA_MSG_TYPE_RECAP:
          gRecapMsgCount++;
        break;
        
        default:
          gOtherMsgCount++;
        break;
    }

    if (gVerbosity >= CHURN_LOG_LEVEL_FINEST)
    {
        symbol = subscription->getSymbol();
        printf("%s Type: %s Status %s \n",
        NULL==symbol ? "" : symbol,
        msg.getMsgTypeName(), 
        msg.getMsgStatusString());
    }
}


static void destroySubscription (int index)
{
    tArrayItem* item = &gSymbolArray[index];

    if (gVerbosity >= CHURN_LOG_LEVEL_FINEST)
        printf("destroying %s\n",item->symbol);

    if (item->sub != NULL)
    {
        (item->sub)->destroy();
        delete item->sub;
        item->sub = NULL;
    }
}

static void createSubscription (int index)
{
    SubscriptionCallbacks* callbacks = new SubscriptionCallbacks();
    tArrayItem* item = &gSymbolArray[index];
  
    if (!item->queue)
    {
        item->queue = (gQueues ==
                   NULL ? gDefaultQueue : &gQueues[index % gThreads]);
    }

    if (gVerbosity >= CHURN_LOG_LEVEL_FINEST)
        printf("creating %s\n",item->symbol);

    if (gGroupSubscription)
    {
        (item->sub)->setSubscriptionType(MAMA_SUBSC_TYPE_GROUP);
    }

    item->sub = new MamaSubscription();
    (item->sub)->create (item->queue, callbacks, gSource, item->symbol, (void*) &index);
}

void LifetimeCallback::onTimer (MamaTimer* timer)
{
    Mama::stop(gBridge);
    printf("Lifetime expired\n");
}


void RecreateCallback::onEvent (MamaQueue& queue, void* closure)
{
	unsigned long churnIndex  = (unsigned long)closure;
    destroySubscription(churnIndex);
    createSubscription(churnIndex);
}

void ChurnCallback::onTimer (MamaTimer* timer)
{
    int churnCount  = 0;
    int churnIndex  = 0;
    double interval = 0;

    /* initialize random seed: */
    srand ( time(NULL) );

    while (churnCount < gNumToChurn)
    {
        churnIndex = rand() % gSymbolCount;

        if (gVerbosity >= CHURN_LOG_LEVEL_FINEST)
            printf("Churning symbol: %s\n", gSymbolArray[churnIndex].symbol);

        interval = ((double)rand() / ((double)(RAND_MAX)+(double)(1)) );
        gSymbolArray[churnIndex].queue->enqueueEvent(recreateCallback, (void*)churnIndex);
        churnCount++;
    }

    gChurned+=churnCount;
}

static void create_source (const char* sourceName, 
                           const char* transportName)
{
    char id[256];

    if (gVerbosity >= CHURN_LOG_LEVEL_FINEST)
        printf("Creating transport: %s\n", transportName);
    gTransport = new MamaTransport();
    gTransport->setTransportCallback(new TransportCallback());

	/*The default throttle rate is 500 msg/sec*/
	if (gThrottle > 0)
	{
        gTransport->setOutboundThrottle(gThrottle, MAMA_THROTTLE_DEFAULT);
	}

    gTransport->create(transportName, gBridge);

    gTransport->setInvokeQualityForAllSubscs(0);
    printf("Transport created\n");
    if (gVerbosity >= CHURN_LOG_LEVEL_FINEST)
        printf("Creating source: %s\n", sourceName);

    gSource = new MamaSource();

    snprintf(id,sizeof(id),"Subscription_Source_%s",sourceName);
    gSource->setId(id);

    gSource->setTransport(gTransport);

    gSource->setPublisherSourceName(sourceName);
}

static void AddSymbol (const char* symbol)
{
    if (gSymbolCount<MAX_SYMBOLS)
    {
        gSymbolArray[gSymbolCount].symbol=strdup(symbol);
        gSymbolArray[gSymbolCount].sub=NULL;
        gSymbolCount++;
    }
    else
        die("Too many symbols. Max=%d",MAX_SYMBOLS);
}

static void parseCommandLine (int           argc, 
                              const char**  argv,
                              int*          numToChurn, 
                              const char**  symbolfile, 
                              const char**  sourceName,
                              const char**  transportName, 
                              int*          lifetime)
{
    int i;

    if (argc<2)
        usage(EXIT_SUCCESS);

    for (i=1;i<argc;)
    {
        if (0==strcmp(argv[i],"-S"))
        {
            *sourceName=argv[i+1];
            i+=2;
        }
        else if (strcmp(argv[i], "-tport") == 0)
        {
            *transportName = argv[i+1];
            i += 2;
        }
        else if (strcmp(argv[i], "-s") == 0)
        {
            AddSymbol(argv[i+1]);
            i += 2;
        }
        else if ( (strcmp(argv[i], "-c") == 0) || (strcmp(argv[i], "-churn") == 0) )
        {
            gNumToChurn = atoi(argv[i+1]);
            i += 2;
        }
        else if ( (strcmp(argv[i], "-t") == 0) || (strcmp(argv[i], "-timeout") == 0) )
        {
            gChurnTimeout = atoi(argv[i+1]);
            i += 2;
        }
	    else if (strcmp(argv[i], "-threads") == 0)
        {
            gThreads = atoi(argv[i+1]);
            i += 2;
        }
	    else if (strcmp(argv[i], "-r") == 0)
        {
            gThrottle = atoi(argv[i+1]);
            i += 2;
        }
        else if (strcmp(argv[i], "-stats") == 0)
        {
            gStatsTimeout = atoi(argv[i+1]);
            i += 2;
        }
        else if ( (strcmp(argv[i], "-k") == 0) || (strcmp(argv[i], "-kill") == 0) )
        {
            *lifetime = atoi(argv[i+1]);
            i += 2;
        }
        else if (strcmp(argv[i], "-v") == 0)
        {
            if (mama_getLogLevel () == MAMA_LOG_LEVEL_WARN)
            {
                mama_enableLogging(stderr, MAMA_LOG_LEVEL_NORMAL);
            }
            else if (mama_getLogLevel () == MAMA_LOG_LEVEL_NORMAL)
            {
                mama_enableLogging(stderr, MAMA_LOG_LEVEL_FINE);
            }
            else if (mama_getLogLevel () == MAMA_LOG_LEVEL_FINE)
            {
                mama_enableLogging(stderr, MAMA_LOG_LEVEL_FINER);
            }
            else
            {
                mama_enableLogging(stderr, MAMA_LOG_LEVEL_FINEST);
            }
            i += 1;
        }
        else if (strcmp(argv[i], "-V") == 0)
        {
            if (gVerbosity == CHURN_LOG_LEVEL_NORMAL)
            {
                gVerbosity = CHURN_LOG_LEVEL_FINE;
            }
            else if (gVerbosity == CHURN_LOG_LEVEL_FINE)
            {
                gVerbosity = CHURN_LOG_LEVEL_FINER;
            }
            else
            {
                gVerbosity = CHURN_LOG_LEVEL_FINEST;
            }
            i += 1;
        }
        else if ((strcmp(argv[i],"-?") == 0) || ((strcmp(argv[i],"--help") == 0)))
        {
            usage(EXIT_SUCCESS);
        }
        else if (strcmp (argv[i], "-g") == 0)
        {
            gGroupSubscription = 1;
            i++;
        }
        else if (0==strcmp(argv[i],"-f"))
        {
            *symbolfile=argv[i+1];
            i+=2;
        }
        else if (strcmp ("-m", argv[i]) == 0)
        {
            gMiddleware = argv[i+1];
            i += 2;
        }
        else
        {
            die("Unrecognized parameter: <%s>",argv[i]);
            exit (1);
        }
    }
}

static void readSymbolsFromFile(const char* symbolfile)
{
    FILE* fd;
    int line;
    char buf[1024];
    char *c;

    fd=fopen(symbolfile,"r");
    if (!fd)
    {
        printf("Can't open symbol file %s\n",symbolfile);
        exit(EXIT_FAILURE);
    }

    if (gVerbosity >= CHURN_LOG_LEVEL_FINEST)
    printf("Loading symbols from file %s\n",symbolfile);

    line=0;
    while ( fgets(buf,sizeof(buf)-1,fd) )
    {
        line++;

        /* replace newlines with NULLs */
        c = buf;
        while ((*c != '\0') && (*c != '\n'))
            c++;
        *c = '\0';

        AddSymbol(buf);
    }

    fclose(fd);
}

static void cleanup(void)
{
    printf("\nCleaning up, please wait...\n");

    if (NULL != gLifeTimer)
    {
        gLifeTimer->destroy();
        delete gLifeTimer;
        gLifeTimer = NULL;
        if (gVerbosity >= CHURN_LOG_LEVEL_FINEST)
            printf("life timer destroyed\n");
    }
  
    if (NULL != gChurnTimer)
    {
        gChurnTimer->destroy();
        delete gChurnTimer;
        gChurnTimer = NULL;
        if (gVerbosity >= CHURN_LOG_LEVEL_FINEST)
            printf("churn timer destroyed\n");
    }

    if (NULL != gStatsTimer)
    {
        gStatsTimer->destroy();
        delete gStatsTimer;
        gStatsTimer = NULL;
        if (gVerbosity >= CHURN_LOG_LEVEL_FINEST)
            printf("stats timer destroyed\n");
    }

    if (NULL != gSource)
    {
        delete gSource;
        gSource = NULL;
        if (gVerbosity >= CHURN_LOG_LEVEL_FINEST)
            printf("source destroyed\n");
    }

    if ((gThreads > 0) && (NULL != gQueues) && (NULL != gDispatchers))
    {
        for (int i = 0; i<gThreads; ++i) 
        {
            gQueues[i].destroy();
            gDispatchers[i].destroy();
        }

        delete [] gDispatchers;
        delete [] gQueues;

        if (gVerbosity >= CHURN_LOG_LEVEL_FINEST)
            printf("thread queues destroyed\n");
    }

    for (int i=0; i<gSymbolCount; i++)
        destroySubscription (i);
    if (gVerbosity >= CHURN_LOG_LEVEL_FINEST)
        printf("subscriptions destroyed\n");

    if (NULL != gTransport)
    {
        delete gTransport;
        gTransport = NULL;
        if (gVerbosity >= CHURN_LOG_LEVEL_FINEST)
            printf("transport destroyed\n");
    }

    Mama::close();

    if (gVerbosity >= CHURN_LOG_LEVEL_FINEST)
        printf("mama closed\n");
}

static void signalHandler(int sgnl)
{
    Mama::stop(gBridge);
}


int main (int argc, const char **argv)
{
    mama_status status;
    int         numToChurn    = 0;
    const char* symbolfile    = NULL;
    const char* sourceName    = NULL;
    const char* transportName = NULL;
    int         lifetimeSecs  = 0;

    parseCommandLine (argc, 
                      argv,
                      &numToChurn, 
                      &symbolfile, 
                      &sourceName, 
                      &transportName, 
                      &lifetimeSecs);

    if (NULL!=symbolfile)
        readSymbolsFromFile(symbolfile);

    signal(SIGINT, signalHandler);

    if (NULL==sourceName)    die("source not specified");
    if (NULL==transportName) die("transport not specified");
    if (gSymbolCount<1)      die("no symbols specified");

    if (gVerbosity >= CHURN_LOG_LEVEL_FINEST)
        printf("Using %d symbol%s\n",gSymbolCount,1==gSymbolCount?"":"s");

    /* Set churn to 100% if anything unusual is tried */
    if ( (gNumToChurn>gSymbolCount) || (gNumToChurn==0) )
        gNumToChurn = gSymbolCount;

    if (gVerbosity >= CHURN_LOG_LEVEL_FINEST)
        printf("%d churn%s/sec (%d%% of symbols)\n",
               gNumToChurn,1==gNumToChurn?"":"s",100*gNumToChurn/gSymbolCount);

    gBridge = Mama::loadBridge(gMiddleware);

    Mama::open();

    gDefaultQueue = Mama::getDefaultEventQueue (gBridge);
    create_source(sourceName, transportName);

	if (gThreads > 0)
	{
        gQueues      = new MamaQueue[gThreads];
        gDispatchers = new MamaDispatcher[gThreads];

		for (int i = 0; i < gThreads; i++)
		{
	        gQueues[i].create(gBridge);
            gDispatchers[i].create(&gQueues[i]);
		}
	}

    for (int i=0; i < gSymbolCount; i++)
    {
        createSubscription (i);
    }

    if (gVerbosity >= CHURN_LOG_LEVEL_FINEST)
    {
        printf("Starting churn timer\n");
    }
    gChurnTimer = new MamaTimer();
    ChurnCallback* churnCallback = new ChurnCallback();
    gChurnTimer->create (gDefaultQueue, churnCallback, gChurnTimeout, NULL);

    if (gVerbosity >= CHURN_LOG_LEVEL_FINEST)
    {
        printf("Starting stats timer\n");
    }
    gStatsTimer = new MamaTimer();
    StatsCallback* statsCallback = new StatsCallback();
    gStatsTimer->create (gDefaultQueue, statsCallback, gStatsTimeout, NULL);

    if (lifetimeSecs>0)
    {
        if (gVerbosity >= CHURN_LOG_LEVEL_FINEST)
        {  
            printf("Starting life timer, program will stop in %d seconds%s\n",lifetimeSecs,1==lifetimeSecs?"":"s");
        }
        
        gLifeTimer = new MamaTimer();
        LifetimeCallback* lifetimeCallback = new LifetimeCallback();
        gLifeTimer->create (gDefaultQueue, lifetimeCallback, lifetimeSecs, NULL);
    }
    if (gVerbosity >= CHURN_LOG_LEVEL_FINEST)
    {
        printf("Starting mama\n");
    }

    printf("(Type CTRL-C to exit)\n\n");

    /* execution continues here only after SIGINT interrupt (e.g. CTRL-C) or lifetime timer event*/
    Mama::start(gBridge);

    cleanup();

    printf("Finished\n");
    return 0;
}
