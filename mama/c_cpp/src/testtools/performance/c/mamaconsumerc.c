/* $Id: mamaconsumerc.c,v 1.1.2.2 2011/09/28 15:21:09 emmapollock Exp $
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

#include "wombat/wincompat.h"
#include "mama/mama.h"
#include "string.h"
#include <stdlib.h>

/* MAMA function checking macro */
#define MAMA_CHECK(status)                                                     \
{                                                                              \
  if (MAMA_STATUS_OK != status) {                                              \
    printf("Function error - function returned %s - error code %d at %s:%d\n", \
      mamaStatus_stringForStatus(status), status, __FILE__, __LINE__);         \
    exit(status);                                                              \
  }                                                                            \
}

#define MC_MIN_SAMPLE_RATE             1
#define MC_MIN_INTERVAL_RATE           10
#define MC_MAX_SUBSCRIPTIONS           250000
static mamaDateTime     gMsgTime       = NULL;
static mamaDateTime     gNowTime       = NULL;
static struct timeval gNow, gLast;
static uint32_t         gIntervalRate  = 1000*MC_MIN_INTERVAL_RATE; /* prints debug every gIntervalRate microseconds */
static uint32_t         gSampleRate    = MC_MIN_SAMPLE_RATE; /* parse message data every gSampleRate msgs */
static const char*      gSymFileName   = NULL;
static const char**     gSymbolList    = NULL;
static uint32_t         gNumSymbols    = 0;
static uint32_t         gTimeInterval = 0;
static mamaSubscription* gSubscriptionList   = NULL;
static const char*       gSymbolNamespace    = NULL;
static int              gVcount = 0;
static int              gflush = 0;
static const char *     gUsageString[] =
{
"This sample application demonstrates how to consume ",
"mamaMsg's from a basic subscription.",
"",
" It accepts the following command line arguments:",
"      [-S namespace]     Symbol name space for the data",
"      [-s symbol | -f symbolFile ]  The topic(s) to which to subscribe. Default value",
"                         is \"MAMA_TOPIC\".",
"      [-tport name]      The transport parameters to be used from",
"                         mama.properties. Default is sub",
"      [-m middleware]    The middleware to use [wmw/lbm/tibrv]. Default",
"                         is wmw.",
"      [-r sampleRate]    Message parsing rate (1 in every r msgs). default ",
"                         is every 1 msgs.",
"      [-i ]              Output interval for stats - default is every 10000",
"                         messages.",
"      [-v]               Increase verbosity. Can be passed multiple times",
NULL
};


/**********************************************************
 *                  FUNCTION PROTOTYPES                   *
 **********************************************************/
void mc_parseCommandLine
(
  int argc,
  const char **  argv,
  const char ** topic,
  const char ** transportName,
  const char ** middleware,
  uint32_t    * pSampleRate,
  uint32_t    * pIntervalRate,
  MamaLogLevel *pMamaLogLevel
);

static void mc_initializeMama
(
  mamaBridge       * pBridge,
  const char       * middleware,
  mamaQueue        * pQueue,
  mamaTransport    * pTransport,
  const char       * transportName
);

static void mc_createSubscriber
(
  mamaMsgCallbacks * pCb,
  MamaLogLevel       logLevel,
  mamaTransport      transport,
  mamaQueue          queue
);

static void MAMACALLTYPE mc_createCb
(
  mamaSubscription       subscription,
  void               *   closure
);

static void MAMACALLTYPE mc_errorCb
(
  mamaSubscription       subscription,
  mama_status            status,
  void*                  platformError,
  const char*            subject,
  void*                  closure
);

static void MAMACALLTYPE mc_msgCb
(
  mamaSubscription       subscription,
  mamaMsg                msg,
  void *                 closure,
  void *                 itemClosure
);

static void MAMACALLTYPE mc_msgCb2
(
  mamaSubscription       subscription,
  mamaMsg                msg,
  void *                 closure,
  void *                 itemClosure
);

static void mc_usage
(
  int
  exitStatus
);

static void mc_readSymsFromFile
(
  void
);

/**********************************************************
 *                EOF  FUNCTION PROTOTYPES                *
 **********************************************************/


/**********************************************************
 *                        MAIN                            *
 **********************************************************/
int main (int argc, const char** argv)
{
  static mamaTransport    transport     = NULL;
  static mamaMsgCallbacks cb;

  const char *     topic          = "MAMA_TOPIC";
  const char *     transportName  = "sub";
  MamaLogLevel     logLevel       = MAMA_LOG_LEVEL_NORMAL;
  const char *     middleware     = "wmw";
  mamaBridge       bridge         = NULL;
  mamaQueue        queue          = NULL;

  gSymbolList = (const char**)calloc (MC_MAX_SUBSCRIPTIONS, sizeof (char*));

  gSubscriptionList = (mamaSubscription*)calloc (MC_MAX_SUBSCRIPTIONS, sizeof (mamaSubscription));

  mamaDateTime_create(&gMsgTime);
  mamaDateTime_create(&gNowTime);
  mamaDateTime_setToNow(gNowTime);

  gettimeofday(&gLast, NULL);
  gettimeofday(&gNow, NULL);

  mc_parseCommandLine (argc,
                       argv,
                       &topic,
                       &transportName,
                       &middleware,
                       &gSampleRate,
                       &gIntervalRate,
                       &logLevel
                      );

  if (gNumSymbols == 0)
  {
    mc_readSymsFromFile ();
  }

  mc_initializeMama ( &bridge,
                      middleware,
                      &queue,
                      &transport,
                      transportName
                    );


  mc_createSubscriber(&cb,
                      logLevel,
                      transport,
                      queue
                     );

  mama_start (bridge);

  return 0;
}


/**********************************************************
 *                    mc_initializeMama                   *
 **********************************************************/
static void mc_initializeMama
(
  mamaBridge       * pBridge,
  const char       * middleware,
  mamaQueue        * pQueue,
  mamaTransport    * pTransport,
  const char       * transportName
)
{
  MAMA_CHECK(mama_loadBridge (pBridge, middleware));
  MAMA_CHECK(mama_open ());
  mama_getDefaultEventQueue (*pBridge, pQueue);
  MAMA_CHECK(mamaTransport_allocate (pTransport));
  MAMA_CHECK(mamaTransport_create (*pTransport, transportName, *pBridge));
}


/**********************************************************
 *                    mc_createSubscriber                 *
 **********************************************************/
static void mc_createSubscriber
(
  mamaMsgCallbacks * pCb,
  MamaLogLevel       logLevel,
  mamaTransport      transport,
  mamaQueue          queue
)
{
  int i=0;
  const char* prefix = NULL;

  pCb->onCreate  = mc_createCb;
  pCb->onError   = mc_errorCb;
  if (gTimeInterval)
  {
    pCb->onMsg     = mc_msgCb2;
  }
  else
  {
    pCb->onMsg     = mc_msgCb;
  }
  pCb->onQuality = NULL;       /* not used by basic subscriptions */
  pCb->onGap = NULL;           /* not used by basic subscriptions */
  pCb->onRecapRequest = NULL;  /* not used by basic subscriptions */

  for (i=0; i<gNumSymbols; i++)
  {
    if (NULL !=gSymbolNamespace)
    {
      prefix = strdup(gSymbolNamespace);
      strcat((char*)prefix,".");
      strcat((char*)prefix,gSymbolList[i]);
    }
    else
    {
      prefix = gSymbolList[i];
    }
    MAMA_CHECK(mamaSubscription_allocate (&gSubscriptionList[i]));
    mamaSubscription_setDebugLevel (gSubscriptionList[i], logLevel);
    MAMA_CHECK(mamaSubscription_createBasic (gSubscriptionList[i],
                                             transport,
                                             queue,
                                             pCb,
                                             prefix,
                                             NULL));

    if (gVcount)
    {
      printf("Created subscription for %s\n", prefix);
    }
    free((void*)prefix);
  }
}


/**********************************************************
 *                      mc_createCb                        *
 **********************************************************/
static void MAMACALLTYPE mc_createCb
(
  mamaSubscription   subscription,
  void             * closure
)
{
}


/**********************************************************
 *                      mc_errorCb                        *
 **********************************************************/
static void MAMACALLTYPE mc_errorCb
(
  mamaSubscription   subscription,
  mama_status        status,
  void             * platformError,
  const char       * subject,
  void             * closure
)
{
  printf ("mamaconsumerc: Error creating subscription: %s\n",
           mamaStatus_stringForStatus (status));
  exit (status);
}



/**********************************************************
 *                       mc_msgCb                         *
 **********************************************************/
static void MAMACALLTYPE mc_msgCb
(
  mamaSubscription  subscription,
  mamaMsg           msg,
  void*             closure,
  void*             itemClosure
)
{
  static uint64_t msgCount=0;
  static uint32_t seqNum =0;
  static uint64_t targetMsgCount=0;

  seqNum++;

  if (msgCount++ < targetMsgCount)
  {
    return;
  }
{
  static uint32_t retSeqNum = 0;
  static uint64_t gapCount = 0;
  static int64_t latency = 0;
  static int64_t runningLatency = 0;
  static int64_t lowLatency = INT64_MAX;
  static int64_t highLatency=INT64_MIN;
  static uint64_t lastMsgCount =0;
  static uint64_t sampleCount =0;

  mamaMsg_getU32 (msg, NULL, 10, &retSeqNum);
  mamaMsg_getDateTime (msg, NULL, 16, gMsgTime);
  mamaDateTime_setToNow(gNowTime);
  mamaDateTime_diffMicroseconds(gNowTime, gMsgTime, &latency);

  if (seqNum != retSeqNum)
  {
    gapCount++;
    seqNum = retSeqNum;
  }

  runningLatency +=latency;
  sampleCount++;

  if (latency < lowLatency)  lowLatency=latency;
  if (latency > highLatency) highLatency=latency;

  if ((msgCount - lastMsgCount) >= gIntervalRate)
  {
    gettimeofday(&gNow, NULL);
    printf ("Rate: %llu\tLatency(usec): Low: %lld\tAverage: %lld\tHigh: %lld\tGaps: %llu\n",
           (uint64_t)((float)(msgCount-lastMsgCount)/(((float)((gNow.tv_sec - gLast.tv_sec)*1000000) + (gNow.tv_usec - gLast.tv_usec))/1000000.0f)),
           lowLatency,
           (int64_t)((float)runningLatency/(float)sampleCount),
           highLatency,
           gapCount);
    if (gflush)
    {
      fflush(stdout);
    }
    latency = runningLatency = gapCount = 0;
    lowLatency = INT64_MAX;
    highLatency= INT64_MIN;
    gLast=gNow;
    sampleCount =0;
    lastMsgCount = msgCount;
    targetMsgCount = msgCount+gSampleRate;
  }
}
}


/**********************************************************
 *                       mc_msgCb2                         *
 **********************************************************/
static void MAMACALLTYPE mc_msgCb2
(
  mamaSubscription  subscription,
  mamaMsg           msg,
  void*             closure,
  void*             itemClosure
)
{
  static uint64_t msgCount=0;
  static uint64_t targetMsgCount=0;


  if (msgCount++ < targetMsgCount)
  {
    return;
  }
{
  static uint32_t retSeqNum = 0;
  static uint64_t gapCount = 0;
  static int64_t latency = 0;
  static int64_t runningLatency = 0;
  static int64_t lowLatency = INT64_MAX;
  static int64_t highLatency=INT64_MIN;
  static uint64_t lastMsgCount =0;
  static uint64_t sampleCount =0;
  char nowTime [40] = {0x0};
  struct tm * now;
  time_t nTime;

  mamaMsg_getU32 (msg, NULL, 10, &retSeqNum);
  mamaMsg_getDateTime (msg, NULL, 16, gMsgTime);
  mamaDateTime_setToNow(gNowTime);
  mamaDateTime_diffMicroseconds(gNowTime, gMsgTime, &latency);

  runningLatency +=latency;
  sampleCount++;

  if (latency < lowLatency)  lowLatency=latency;
  if (latency > highLatency) highLatency=latency;

  gettimeofday(&gNow, NULL);
  if (gNow.tv_sec - gLast.tv_sec >= gTimeInterval)
  {
    time(&nTime);
    now = localtime(&nTime);
    strftime(nowTime, 100, "%Y/%m/%d - %H:%M:%S", now);
    printf ("%s Rate: %llu\tLatency(usec): Low: %lld\tAverage: %lld\tHigh: %lld\n",
            nowTime,
           (uint64_t)((float)(msgCount-lastMsgCount)/(((float)((gNow.tv_sec - gLast.tv_sec)*1000000) + (gNow.tv_usec - gLast.tv_usec))/1000000.0f)),
           lowLatency,
           (int64_t)((float)runningLatency/(float)sampleCount),
           highLatency);
    if (gflush)
    {
      fflush(stdout);
    }
    latency = runningLatency = gapCount = 0;
    lowLatency = INT64_MAX;
    highLatency= INT64_MIN;
    gLast=gNow;
    sampleCount =0;
    lastMsgCount = msgCount;
    targetMsgCount = msgCount+gSampleRate;
  }
}
}


/**********************************************************
 *                   mc_parseCommandLine                  *
 **********************************************************/
void mc_parseCommandLine
(
  int argc,
  const char ** argv,
  const char ** topic,
  const char ** transportName,
  const char ** middleware,
  uint32_t    * pSampleRate,
  uint32_t    * pIntervalRate,
  MamaLogLevel *pMamaLogLevel
)
{
  int i = 0;

  for (i = 1; i < argc;)
  {
    if (strcmp (argv[i], "-s") == 0)
    {
      if (argv[i + 1] != NULL)
      {
        gSymbolList[gNumSymbols++]=strdup (argv[i + 1]);
        i += 2;
      }
      else
      {
        gSymbolList[gNumSymbols++]="";
        i += 2;
      }
    }
    else if (strcmp ("-flush", argv[i]) == 0)
    {
      gflush=1;
      i++;
    }
    else if (strcmp ("-t", argv[i]) == 0)
    {
      gTimeInterval = (uint32_t)atoi(argv[i+1]);
      i += 2;
    }
    else if (strcmp ("-i", argv[i]) == 0)
    {
      *pIntervalRate = (uint32_t)atoi(argv[i+1]);
      if (*pIntervalRate < MC_MIN_INTERVAL_RATE)
      {
        printf("Warning - interval rate must be a minimum of %d messages\n", MC_MIN_SAMPLE_RATE);
        *pIntervalRate = MC_MIN_INTERVAL_RATE;
        printf("Modified interval rate to %d\n", MC_MIN_INTERVAL_RATE);
      }
      i += 2;
    }
    else if (strcmp ("-r", argv[i]) == 0)
    {
      *pSampleRate = (uint32_t)atoi(argv[i+1]);
      if (*pSampleRate < MC_MIN_SAMPLE_RATE)
      {
        printf("Warning - sample rate must be a minimum of %d\n", MC_MIN_SAMPLE_RATE);
        *pSampleRate = MC_MIN_SAMPLE_RATE;
        printf("Modified sample rate to %d\n", MC_MIN_SAMPLE_RATE);
      }
      i += 2;
    }
    else if ((strcmp(argv[i], "-h") == 0) ||
             (strcmp(argv[i], "--help") == 0) ||
             (strcmp(argv[i], "-?") == 0))
    {
      mc_usage(0);
    }
    else if (strcmp (argv[i], "-f") == 0)
    {
      gSymFileName = argv[i + 1];
      i += 2;
    }
    else if(strcmp("-tport", argv[i]) == 0)
    {
      *transportName = argv[i+1];
      i += 2;
    }
    else if (strcmp ("-m", argv[i]) == 0)
    {
      *middleware = (argv[i+1]);
      i += 2;
    }
    else if (strcmp(argv[i], "-S") == 0)
    {
      gSymbolNamespace = argv[i + 1];
      i += 2;
    }
    else if (strcmp (argv[i], "-v") == 0)
    {
      if ( mama_getLogLevel () == MAMA_LOG_LEVEL_WARN )
      {
        mama_enableLogging( stderr, MAMA_LOG_LEVEL_NORMAL );
      }
      else if ( mama_getLogLevel () == MAMA_LOG_LEVEL_NORMAL )
      {
        mama_enableLogging( stderr, MAMA_LOG_LEVEL_FINE );
      }
      else if( mama_getLogLevel () == MAMA_LOG_LEVEL_FINE )
      {
        mama_enableLogging( stderr, MAMA_LOG_LEVEL_FINER );
      }
      else
      {
        mama_enableLogging( stderr, MAMA_LOG_LEVEL_FINEST );
      }
      i++;
    }
  }

  printf ("Starting consumer with:\n topic:              %s\n"
          "   transport:          %s\n",
          *topic, *transportName);
}

/**********************************************************
 *                        mc_usage                        *
 **********************************************************/
void mc_usage (int exitStatus)
{
  int i = 0;
  while (NULL != gUsageString[i])
  {
    printf ("%s\n", gUsageString[i++]);
  }
  exit (exitStatus);
}

static void mc_readSymsFromFile (void)
{
  /* get subjects from file or interactively */
  FILE* fp = NULL;
  char charbuf[1024];

  if (gSymFileName)
  {
    if ((fp = fopen (gSymFileName, "r")) == (FILE *)NULL)
    {
      perror (gSymFileName);
      exit (1);
    }
  }
  else
  {
    fp = stdin;
  }
  if (isatty(fileno (fp)))
  {
    printf ("Enter one symbol per line and terminate with a .\n");
    printf ("Symbol> ");
  }
  while (fgets (charbuf, 1023, fp))
  {
    char *c = charbuf;

    /* Input terminate case */
    if (*c == '.')
      break;

    /* replace newlines with NULLs */
    while ((*c != '\0') && (*c != '\n'))
      c++;
    *c = '\0';

    gSymbolList[gNumSymbols++] = strdup (charbuf);
    if (isatty(fileno (fp)))
    printf ("Symbol> ");
  }
}
