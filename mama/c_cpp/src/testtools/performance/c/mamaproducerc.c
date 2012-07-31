/* $Id: mamaproducerc.c,v 1.1.2.4 2011/09/29 17:56:54 emmapollock Exp $
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
#include "time.h"

#define TV2NANO(x) (1000 *((x)->tv_sec * 1000000 + (x)->tv_usec))
#define TV2USEC(x) ((x)->tv_sec * 1000000 + (x)->tv_usec)
#define TS2NANO(x) ((x)->tv_sec * 1000000000 + (x)->tv_nsec)


/* MAMA function checking macro */
#define MAMA_CHECK(status)                                                     \
{                                                                              \
  if (MAMA_STATUS_OK != status) {                                              \
    printf("Function error - function returned %s - error code %d at %s:%d\n", \
      mamaStatus_stringForStatus(status), status, __FILE__, __LINE__);         \
    exit(status);                                                              \
  }                                                                            \
}

#define MP_MIN_MSG_SIZE 35 /* TLV for sequence number and mama date time */

static mamaPublisher    gPublisher          =  NULL;
static uint32_t         gMsgTimeStampCount  =  1;
static mamaMsg*         gMsgArray           =  NULL;
static int              gflush              =  0;
static const char *     gUsageString[]      =
{
" This sample application demonstrates how to publish mama messages, and",
" respond to requests from a clients inbox.",
"",
" It accepts the following command line arguments:",
"      [-s topic]         The topic on which to send messages. Default is",
"                         is \"MAMA_TOPIC\".",
"      [-l bytes]         The published message size in bytes.",
"                         Default is 200B.",
"      [-sleep nanosecs]  The rate at which to sleep between publishes to throttle message rate.",
"                         Default is 0 == maximum publishing rate possible.",
"      [-d msgDelta]      The number of bytes to change message by i.e. msg",
"                         will be -l +- -d. default delta is 0",
"      [-i debugInterval] Print message rate information every debugInterval msgs",
"                         Default is 1000000.",
"      [-tport name]      The transport parameters to be used from",
"                         mama.properties. Default is pub",
"      [-m middleware]    The middleware to use [wmw/lbm/tibrv].",
"                         Default is wmw.",
"      [-v]               Increase verbosity. Can be passed multiple times",
"      [-t X]             Insert timestamp every X messages default = 1",
"      [-flush]           flushes stdout to enable logging to file",
NULL
};


/************************************************************
*                     FUNCTION PROTOTYPES                   *
*************************************************************/
static void mp_usage (int exitStatus);

void mp_startCB ( mama_status status );

void mp_nanosleep(unsigned nsec);

static void mp_initializeMama
(
  mamaBridge       * pBridge,
  const char       * middleware,
  mamaQueue        * pQueue,
  mamaTransport    * pTransport,
  const char       * transportName
);

static void mp_createPublisher
(
  mamaPublisher * pPublisher,
  mamaTransport transport,
  const char * outboundTopic
);

static void mp_initMessage
(
  uint32_t msgSize,
  uint32_t msgVar,
  mamaDateTime mamaTime
);

static void mp_publishMessage
(
  uint32_t msgNum,
  mamaDateTime mamaTime,
  uint32_t msgSample
);

void mp_parseCommandLine
(
  int            argc,
  const char **  argv,
  const char **  outboundTopic,
  double     *   pMsgInterval,
  const char **  transportName,
  const char **  middleware,
  uint32_t   *   pRefreshRate,
  uint32_t   *   pSleep,
  uint32_t   *   pMsgSize,
  uint32_t   *   pMsgVar
);


/****************************************************
 *                      MAIN                        *
 ****************************************************/
int main (int argc, const char **argv)
{
  const char *     outboundTopic  = "MAMA_TOPIC";
  const char *     transportName  = "pub";
  const char *     middleware     = "wmw";

  double           msgInterval    = 0.5;
  uint32_t         sleepTime      = 0;
  uint32_t         refreshRate    = 1000000;
  uint32_t         msgSize        = 200;
  uint32_t         msgVar         = 0;
  uint32_t         msgNum         = 0;
  uint32_t         randVal        = 0;

  mamaTransport    transport      = NULL;
  mamaBridge       bridge         = NULL;
  mamaQueue        queue          = NULL;
  mamaDateTime     mamaTime       = NULL;
  struct timeval now, last;
  uint32_t i                      = 0;
  int32_t         randValCounts   = 0;

  /* parse command line arguments */
  mp_parseCommandLine          ( argc, argv,
                                 &outboundTopic,
                                 &msgInterval,
                                 &transportName,
                                 &middleware,
                                 &refreshRate,
                                 &sleepTime,
                                 &msgSize,
                                 &msgVar
                               );
  mp_initializeMama            ( &bridge,
                                 middleware,
                                 &queue,
                                 &transport,
                                 transportName
                               );

  mp_createPublisher           ( &gPublisher,
                                 transport,
                                 outboundTopic
                               );

  mama_startBackground         ( bridge,
                                 mp_startCB
                               );

  MAMA_CHECK(mamaDateTime_create (&mamaTime));

  mp_initMessage (msgSize, msgVar, mamaTime);
  gettimeofday (&last, NULL);
  printf ("Starting to publish\n");
  randValCounts = (2*msgVar)+1;

  if (sleepTime)
  {
    for(;;)
    {
      randVal = last.tv_usec % randValCounts;
      for (i=0; i<refreshRate;i++)
      {
        if (i%gMsgTimeStampCount==0)
        {
          mamaDateTime_setToNow(mamaTime);
        }
        mp_publishMessage (msgNum++,mamaTime,randVal );
        mp_nanosleep(sleepTime);
      }
      gettimeofday (&now, NULL);
      printf ("RATE = %llu\n", (uint64_t)((float)refreshRate/(((float)((now.tv_sec - last.tv_sec)*1000000) + (now.tv_usec - last.tv_usec))/1000000.0f)));
      if (gflush)
      {
        fflush(stdout);
      }
      last = now;
    }
  }
  else
  {
    for(;;)
    {
      randVal = last.tv_usec % randValCounts;
      for (i=0; i<refreshRate;i++)
      {
        if (i%gMsgTimeStampCount==0)
        {
          mamaDateTime_setToNow(mamaTime);
        }
        mp_publishMessage (msgNum++, mamaTime, randVal);
      }
      gettimeofday (&now, NULL);
      printf ("RATE = %llu\n", (uint64_t)((float)refreshRate/(((float)((now.tv_sec - last.tv_sec)*1000000) + (now.tv_usec - last.tv_usec))/1000000.0f)));
      if (gflush)
      {
        fflush(stdout);
      }
      last = now;
    }
  }
  return 0;
}


/****************************************************
 *               mp_initializeMama                  *
 ****************************************************/
static void mp_initializeMama
(
  mamaBridge       * pBridge,
  const char       * middleware,
  mamaQueue        * pQueue,
  mamaTransport    * pTransport,
  const char       * transportName
)
{

  MAMA_CHECK( mama_loadBridge (pBridge, middleware) );

  MAMA_CHECK(mama_open ());

  /*Use the default internal event queue for all subscriptions and timers*/
  mama_getDefaultEventQueue (*pBridge, pQueue);

  MAMA_CHECK( mamaTransport_allocate (pTransport));

  MAMA_CHECK( mamaTransport_create (*pTransport, transportName, *pBridge));
}


/****************************************************
 *               mp_createPublisher                 *
 ****************************************************/
static void mp_createPublisher
(
  mamaPublisher * pPublisher,
  mamaTransport transport,
  const char * outboundTopic
)
{
  MAMA_CHECK(mamaPublisher_create (pPublisher,
                                   transport,
                                   outboundTopic,
                                   NULL,   /* Not needed for basic publishers */
                                   NULL) /* Not needed for basic publishers */
            );
}


/****************************************************
 *                  mp_initMessage                  *
 ****************************************************/
static void mp_initMessage (uint32_t msgSize, uint32_t msgVar, mamaDateTime mamaTime )
{
  uint32_t i           =  0;
  char *   padding     =  NULL;
  int32_t paddingSize =  msgSize - MP_MIN_MSG_SIZE;

  if (paddingSize - msgVar <= 0)
  {
    printf("ERROR::msgSize too small - must be a minimum of %d\n", MP_MIN_MSG_SIZE + msgVar);
    exit(1);
  }
  paddingSize +=msgVar;

  padding = (char*)malloc(paddingSize);
  memset(padding, 'A', paddingSize);

  gMsgArray = (mamaMsg*)calloc(((2*msgVar)+1), sizeof(mamaMsg));

  mamaDateTime_setToNow(mamaTime);

  for (i=0; i<=2*msgVar; i++)
  {
    MAMA_CHECK(mamaMsg_create(&gMsgArray[i]));
    MAMA_CHECK(mamaMsg_addU32(gMsgArray[i], NULL, 10, 0));
    MAMA_CHECK(mamaMsg_addDateTime(gMsgArray[i], NULL, 16, mamaTime));
    padding[paddingSize - i - 1] = '\0';
    MAMA_CHECK(mamaMsg_addString (gMsgArray[i], NULL, 10004, padding));
  }
}


/****************************************************
 *                mp_publishMessage                 *
 ****************************************************/
static void mp_publishMessage
(
  uint32_t msgNum,
  mamaDateTime mamaTime,
  uint32_t msgSample
)
{
  /* Add fields to illustrates how data is placed in the message */
  MAMA_CHECK(mamaMsg_updateU32 (gMsgArray[msgSample], "NULL", 10, msgNum));
  MAMA_CHECK(mamaMsg_updateDateTime(gMsgArray[msgSample], NULL, 16, mamaTime));
  /*Padding already set in previous message*/
  MAMA_CHECK(mamaPublisher_send (gPublisher, gMsgArray[msgSample]));
}


/****************************************************
 *             mp_parseCommandLine                  *
 ****************************************************/
void mp_parseCommandLine
(
  int            argc,
  const char **  argv,
  const char **  outboundTopic,
  double     *   pMsgInterval,
  const char **  transportName,
  const char **  middleware,
  uint32_t   *   pRefreshRate,
  uint32_t   *   pSleep,
  uint32_t   *   pMsgSize,
  uint32_t   *   pMsgVar
)
{
  const char * prefix = "";
  int i = 0;

  for (i = 1; i < argc; )
  {
    if (strcmp ("-p", argv[i]) == 0)
    {
      prefix = argv[i+1];
      i += 2;
    }
    if (strcmp ("-s", argv[i]) == 0)
    {
      *outboundTopic = argv[i+1];
      i += 2;
    }
    else if (strcmp ("-l", argv[i]) == 0)
    {
      *pMsgSize = (uint32_t)atol(argv[i+1]);
      i += 2;
    }
    else if (strcmp ("-sleep", argv[i]) == 0)
    {
      *pSleep = (uint32_t) atol (argv[i+1]);
      i += 2;
    }
    else if (strcmp ("-i", argv[i]) == 0)
    {
      *pRefreshRate = (uint32_t)atol (argv[i+1]);
      i += 2;
    }
    else if (strcmp ("-t", argv[i]) == 0)
    {
      gMsgTimeStampCount = (uint32_t) atol(argv[i+1]);
      i += 2;
    }
    else if (strcmp ("-flush", argv[i]) == 0)
    {
      gflush=1;
      i++;
    }
    else if (strcmp ("-d", argv[i]) == 0)
    {
      *pMsgVar = (uint32_t) atol(argv[i+1]);
      i += 2;
    }
    else if (strcmp ("-tport", argv[i]) == 0)
    {
      *transportName = argv[i+1];
      i += 2;
    }
    else if (strcmp ("-m", argv[i]) == 0)
    {
      *middleware = argv[i+1];
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
    else if ((strcmp (argv[i], "-h")     == 0) ||
             (strcmp (argv[i], "--help") == 0) ||
             (strcmp (argv[i], "-?")     == 0))
    {
      mp_usage(0);
    }
  }

  printf ( "Starting Publisher with:\n"
           "   topic:              %s\n"
           "   interval            %f\n"
           "   transport:          %s\n"
           "   msg size            %d\n",
           *outboundTopic, *pMsgInterval, *transportName, *pMsgSize
         );
}


/****************************************************
 *                      mp_usage                    *
 ****************************************************/
void mp_usage ( int exitStatus )
{
  int i=0;
  while (NULL != gUsageString[i])
  {
    printf ("%s\n", gUsageString[i++]);
  }
  exit (exitStatus);
}


/****************************************************
 *                    mp_nanosleep                  *
 ****************************************************/
void mp_nanosleep(unsigned nsec)
{
  struct timespec st,et;
  unsigned long long start;
  long long delta;

/* FIX for SOLARIS issue */
#ifndef CLOCK_PROCESS_CPUTIME_ID
#define CLOCK_PROCESS_CPUTIME_ID CLOCK_HIGHRES
#endif
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &st);

  start = TS2NANO(&st);
  while(1)
  {
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &et);
    delta = TS2NANO(&et) - start - nsec;
    if(delta >= 0)
    {
      return;
    }
  }
}

/****************************************************
 *                     mp_startCB                   *
 ****************************************************/
void mp_startCB(mama_status status){}
