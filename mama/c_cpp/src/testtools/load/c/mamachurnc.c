/* $Id: mamachurnc.c,v 1.1.2.3 2011/09/28 15:21:06 emmapollock Exp $
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

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <signal.h>
#include <mama/mama.h>
#include <wombat/wincompat.h>

typedef struct
{
  char* symbol;
  mamaSubscription sub;
  mamaQueue queue;
} tArrayItem;

#define MAX_SYMBOLS 250000

static mamaTransport gTransport;
static mamaSource gSource;
static mamaTimer gChurnTimer;
static mamaTimer gStatsTimer;
static mamaTimer gLifeTimer;

static mamaQueue gDefaultQueue;
static mamaBridge gBridge;
static const char* gMiddleware = "wmw";

static int gThrottle = 0;
static int gNumToChurn = 0;
static int gChurnTimeout = 2;
static int gStatsTimeout = 2;
static int gVerbosity = 0;
static tArrayItem gSymbolArray[MAX_SYMBOLS];
static int gSymbolCount;

static int               gThreads            = 0;
static mamaQueue*        gQueues             = NULL;
static mamaDispatcher*   gDispatchers        = NULL;


/* Stats counters */
static int gCreateMsgCount = 0;
static int gInitialMsgCount = 0;
static int gRecapMsgCount = 0;
static int gOtherMsgCount = 0;
static int gErrorCount = 0;
static int gChurned = 0;
static int gGapCount = 0;
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
  gInitialMsgCount=0;
  gCreateMsgCount=0;
  gOtherMsgCount=0;
  gRecapMsgCount=0;
  gChurned=0;
  gErrorCount=0;
  gGapCount=0;
}

#define CHURN_LOG_LEVEL_NORMAL 0
#define CHURN_LOG_LEVEL_FINE   1
#define CHURN_LOG_LEVEL_FINER  2
#define CHURN_LOG_LEVEL_FINEST 3


#define STATS_FIELD_WIDTH -5 /* negative = left alignment */

static void MAMACALLTYPE statsCallback(mamaTimer timer, void *closure)
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

static void usage(int exitStatus)
{
  int i = 0;
  while (gUsageString[i]!=NULL)
  {
    printf("%s\n", gUsageString[i++]);
  }
  exit(exitStatus);
}

static void die(const char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  printf("Fatal Error: ");
  vprintf(fmt, ap);
  printf("\n");
  va_end(ap);

  exit(EXIT_FAILURE);
}

static void checkStatus(char *err, mama_status status)
{
  if (MAMA_STATUS_OK!=status)
  {
    printf("Fatal Error: %s : status %d (%s)\n",err,status,mamaStatus_stringForStatus(status));
    exit(EXIT_FAILURE);
  }
}

static void MAMACALLTYPE subscriptionOnCreate(mamaSubscription subscription, void* closure)
{
  const char* symbol = NULL;

  gCreateMsgCount++;
  if (gVerbosity>=CHURN_LOG_LEVEL_FINEST)
  {
    (void)mamaSubscription_getSymbol(subscription, &symbol);
    printf("%s Type: Create\n", NULL==symbol ? "" : symbol);
  }
}

static void MAMACALLTYPE subscriptionOnError(mamaSubscription subscription,
                      mama_status      status,
                      void*            platformError,
                      const char*      subject,
                      void*            closure)
{
  gErrorCount++;
  if (gVerbosity>=CHURN_LOG_LEVEL_FINEST)
  {
    printf("subscription error, %s: %s\n",
      subject, mamaStatus_stringForStatus(status));
  }
  mamaSubscription_activate(subscription);
}

static void MAMACALLTYPE subscriptionOnMsg(mamaSubscription subscription,
                                        mamaMsg msg,
                                        void *closure,
                                        void *itemClosure)
{
  mamaMsgType msgType=mamaMsgType_typeForMsg(msg);
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

  if (gVerbosity>=CHURN_LOG_LEVEL_FINEST)
  {
    (void)mamaSubscription_getSymbol(subscription, &symbol);
    printf("%s Type: %s Status %s \n",
      NULL==symbol ? "" : symbol,
      mamaMsgType_stringForMsg(msg),
      mamaMsgStatus_stringForMsg(msg));
  }
}


static void destroySubscription(int index)
{
  mama_status status;

  tArrayItem *item = &gSymbolArray[index];

  if (gVerbosity>=CHURN_LOG_LEVEL_FINEST)
    printf("destroying %s\n",item->symbol);

  status=mamaSubscription_destroy(item->sub);
  checkStatus("mamaSubscription_destroy",status);

  status=mamaSubscription_deallocate(item->sub);
  checkStatus("mamaSubscription_deallocate",status);

  item->sub = NULL;
}

static void createSubscription(int index)
{
  mamaMsgCallbacks callbacks;
  mama_status status;
  tArrayItem *item = &gSymbolArray[index];

  if (!item->queue)
  {
      item->queue = gQueues ==
                    NULL ? gDefaultQueue : gQueues[index % gThreads];
  }

  callbacks.onCreate        = subscriptionOnCreate;
  callbacks.onError         = subscriptionOnError;
  callbacks.onMsg           = subscriptionOnMsg;
  callbacks.onQuality       = NULL; /* not used by basic subscriptions */
  callbacks.onGap           = NULL; /* not used by basic subscriptions */
  callbacks.onRecapRequest  = NULL; /* not used by basic subscriptions */
  callbacks.onDestroy       = NULL; /* not used by basic subscriptions */

  if (gVerbosity>=CHURN_LOG_LEVEL_FINEST)
    printf("creating %s\n",item->symbol);

  status=mamaSubscription_allocate(&(item->sub));
  checkStatus("mamaSubscription_allocate()",status);

  if (gGroupSubscription)
  {
    mamaSubscription_setSubscriptionType(item->sub,
                                        MAMA_SUBSC_TYPE_GROUP);
  }

  status=mamaSubscription_create(item->sub,
                                item->queue,
                                &callbacks,
                                gSource,
                                item->symbol,
                                (void*)index);


  checkStatus("mamaSubscription_create()",status);
}

static void MAMACALLTYPE lifetimeCallback(mamaTimer timer, void *closure)
{
  mama_stop(gBridge);
  printf("Lifetime expired\n");
}

static void MAMACALLTYPE recreateCallback(mamaTimer timer, void *closure)
{
    /* try to destroy existing subscription */
    destroySubscription((int)closure);
    createSubscription((int)closure);
	mamaTimer_destroy(timer);
}

static void MAMACALLTYPE churnCallback(mamaTimer timer, void *closure)
{
  int churnCount = 0;
  int churnIndex = 0;
  double interval = 0;
  mama_status status;
  /* initialize random seed: */
  srand ( time(NULL) );

  while (churnCount < gNumToChurn)
  {
    /* Calculate array index */
    mamaTimer destroytimer;

    churnIndex=rand() % gSymbolCount;
    interval = (   (double)rand() / ((double)(RAND_MAX)+(double)(1)) );
    status=mamaTimer_create(&destroytimer, gSymbolArray[churnIndex].queue,
                                recreateCallback, interval, (void*)churnIndex);

    churnCount++;
  }
  gChurned+=churnCount;
}

static void MAMACALLTYPE transportCb(mamaTransport tport,mamaTransportEvent ev,
                                  short cause,const void *opaque,void *closure)
{
  mamaQuality quality = MAMA_QUALITY_UNKNOWN;
  switch (ev)
  {
   case MAMA_TRANSPORT_DISCONNECT: printf("*** Disconnected\n"); break;
   case MAMA_TRANSPORT_RECONNECT:  printf("*** Reconnected\n");  break;
   case MAMA_TRANSPORT_QUALITY:
    mamaTransport_getQuality(tport,&quality);
    printf("*** Transport quality %s\n",mamaQuality_convertToString(quality));
    break;
  default: printf("*** Transport: Unknown event %d\n",ev);  break;
  }
}

static void create_source(const char *sourceName, const char *transportName)
{
  mama_status status;
  char id[256];

  if (gVerbosity>=CHURN_LOG_LEVEL_FINEST)
    printf("Creating transport: %s\n", transportName);

  status=mamaTransport_allocate(&gTransport);
  checkStatus("mamaTransport_allocate()",status);

  status=mamaTransport_setTransportCallback(gTransport,transportCb,NULL);
  checkStatus("mamaTransport_setTransportCallback()",status);

	/*The default throttle rate is 500 msg/sec*/
	if (gThrottle > 0)
	{
		status=mamaTransport_setOutboundThrottle (gTransport,
										   MAMA_THROTTLE_DEFAULT,
										   gThrottle);
		checkStatus("mamaTransport_setOutboundThrottle()",status);
	}

  status=mamaTransport_create(gTransport,transportName, gBridge);
  checkStatus("mamaTransport_create()",status);

  status=mamaTransport_setInvokeQualityForAllSubscs(gTransport,0);
  checkStatus("mamaTransport_create()",status);

  if (gVerbosity>=CHURN_LOG_LEVEL_FINEST)
    printf("Creating source: %s\n", sourceName);

  status=mamaSource_create(&gSource);
  checkStatus("mamaSource_create()",status);

  snprintf(id,sizeof(id),"Subscription_Source_%s",sourceName);
  status=mamaSource_setId(gSource,id);
  checkStatus("mamaSource_setId",status);

  status=mamaSource_setTransport(gSource, gTransport);
  checkStatus("mamaSource_setTransport",status);

  status=mamaSource_setSymbolNamespace(gSource, sourceName);
  checkStatus("mamaSource_setSymbolNamespace",status);
}

static void AddSymbol(const char* symbol)
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

static void parseCommandLine(int argc, const char** argv,
  int* numToChurn, const char** symbolfile, const char** sourceName,
  const char** transportName, int* lifetime)
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
      gVerbosity++;
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

  if (gVerbosity>=CHURN_LOG_LEVEL_FINEST)
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
  mama_status status;
  int i;

  printf("\nCleaning up, please wait...\n");

  for (i=0;i<gSymbolCount;i++)
    destroySubscription(i);
  if (gVerbosity>=CHURN_LOG_LEVEL_FINEST)
    printf("subscriptions destroyed\n");

  if (NULL!=gLifeTimer)
  {
    status=mamaTimer_destroy(gLifeTimer);
    checkStatus("mamaTimer_destroy(LifeTimer)",status);
    if (gVerbosity>=CHURN_LOG_LEVEL_FINEST)
    printf("life timer destroyed\n");
  }

  status=mamaTimer_destroy(gChurnTimer);
  checkStatus("mamaTimer_destroy(ChurnTimer)",status);
  if (gVerbosity>=CHURN_LOG_LEVEL_FINEST)
  printf("churn timer destroyed\n");

  status=mamaTimer_destroy(gStatsTimer);
  checkStatus("mamaTimer_destroy(StatsTimer)",status);
  if (gVerbosity>=CHURN_LOG_LEVEL_FINEST)
  printf("stats timer destroyed\n");

  status=mamaSource_destroy(gSource);
  checkStatus("mamaSource_destroy()",status);
  if (gVerbosity>=CHURN_LOG_LEVEL_FINEST)
  printf("source destroyed\n");

  status=mamaTransport_destroy(gTransport);
  checkStatus("mamaTransport_destroy()",status);
  if (gVerbosity>=CHURN_LOG_LEVEL_FINEST)
  printf("transport destroyed\n");

  status=mama_close();
  checkStatus("mamaTimer_close()",status);
  if (gVerbosity>=CHURN_LOG_LEVEL_FINEST)
  printf("mama closed\n");
}

static void signalHandler(int sgnl)
{
  mama_stop(gBridge);
}


int main(int argc, const char **argv)
{
  mama_status status;
  int numToChurn = 0;
  const char* symbolfile = NULL;
  const char* sourceName = NULL;
  const char* transportName = NULL;
  int lifetimeSecs = 0;
  int i;

  parseCommandLine(argc, argv,
    &numToChurn, &symbolfile, &sourceName, &transportName, &lifetimeSecs);

  if (NULL!=symbolfile)
    readSymbolsFromFile(symbolfile);

  signal(SIGINT, signalHandler);

  if (NULL==sourceName) die("source not specified");
  if (NULL==transportName) die("transport not specified");
  if (gSymbolCount<1) die("no symbols specified");

  if (gVerbosity>=CHURN_LOG_LEVEL_FINEST)
  printf("Using %d symbol%s\n",gSymbolCount,1==gSymbolCount?"":"s");

  /* Set churn to 100% if anything unusual is tried */
  if ( (gNumToChurn>gSymbolCount) || (gNumToChurn==0) )
    gNumToChurn=gSymbolCount;

    if (gVerbosity>=CHURN_LOG_LEVEL_FINEST)
  printf("%d churn%s/sec (%d%% of symbols)\n",
    gNumToChurn,1==gNumToChurn?"":"s",100*gNumToChurn/gSymbolCount);

  status = mama_loadBridge (&gBridge, gMiddleware);

    if (status != MAMA_STATUS_OK)
    {
        printf ("Error loading %s bridge: %s\n",
                gMiddleware,
                mamaStatus_stringForStatus (status));
        exit (status);
    }

  status=mama_open();
  checkStatus("mama_open()",status);

  mama_getDefaultEventQueue (gBridge, &gDefaultQueue);
  create_source(sourceName, transportName);

	if (gThreads > 0)
	{
		gQueues      = (mamaQueue*)calloc
							(gThreads, sizeof (mamaQueue));
		gDispatchers = (mamaDispatcher*)calloc
							(gThreads, sizeof (mamaDispatcher));

		for (i = 0; i < gThreads; i++)
		{


			if (MAMA_STATUS_OK != (status = mamaQueue_create (&gQueues[i],
							gBridge)))
			{
				fprintf (stderr, "Error creating queue %s\n",
						 mamaStatus_stringForStatus( status ) );
				exit(1);
			}

			status = mamaDispatcher_create (&gDispatchers[i] , gQueues[i]);

			if (status != MAMA_STATUS_OK)
			{
				fprintf (stderr, "Error creating dispatcher %s\n",
						 mamaStatus_stringForStatus( status ) );
				exit(1);
			}
		}
	}

  for (i=0;i<gSymbolCount;i++)
    createSubscription(i);

  if (gVerbosity>=CHURN_LOG_LEVEL_FINEST)
    printf("Starting churn timer\n");
  status=mamaTimer_create(&gChurnTimer, gDefaultQueue, churnCallback, gChurnTimeout, NULL);
  checkStatus("mamaTimer_create(ChurnTimer)",status);
if (gVerbosity>=CHURN_LOG_LEVEL_FINEST)
  printf("Starting stats timer\n");
  status=mamaTimer_create(&gStatsTimer, gDefaultQueue, statsCallback, gStatsTimeout, NULL);
  checkStatus("mamaTimer_create(gStatsTimer)",status);

  if (lifetimeSecs>0)
  {
  if (gVerbosity>=CHURN_LOG_LEVEL_FINEST)
    printf("Starting life timer, program will stop in %d seconds%s\n",lifetimeSecs,1==lifetimeSecs?"":"s");
    status=mamaTimer_create(&gLifeTimer, gDefaultQueue, lifetimeCallback, lifetimeSecs, NULL);
    checkStatus("mamaTimer_create(gLifeTimer)",status);
  }
if (gVerbosity>=CHURN_LOG_LEVEL_FINEST)
  printf("Starting mama\n");
  printf("(Type CTRL-C to exit)\n\n");


  status=mama_start (gBridge);
  checkStatus("mama_start",status);

  /* execution continues here only after SIGINT interrupt (e.g. CTRL-C) */

  cleanup();

  printf("Finished\n");
  return 0;
}
