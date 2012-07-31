/* $Id: capturec.c,v 1.1.2.3 2011/09/28 14:51:03 emmapollock Exp $
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


#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#include <playback/playbackcapture.h>

#define INITIAL_ARRAY_SIZE   300000
#define MAX_BUFFER           128

typedef struct mamaCaptureConfig_
{
    long                 myThrottle;
    int                  myRequireInitial;
    double               myTimeout;
    mamaQueue            myDefaultQueue;
    const char*          myDictSourceName;
    mamaSource           myDictSource;
    mamaDictionary       myDictionary;
    int                  myDumpDataDict;
    int                  myDictToFile;
    const char*          myDictFile;
    int                  myBuildDataDict;
    int                  myNumSubscriptions;
    int                  myNumSymbols;
    MamaLogLevel         mySubscLogLevel;
    const char*          myCaptureFilename;
    mamaPlaybackCapture  myCapture;
    int                  mySubcriptionSymbol;
    int                  myDumpList;
    mamaTransport        myDictTransport;
    const char*          myDictTport;
    int                  myDictionaryComplete;
    mamaBridge           myBridge;
    const char*          myMiddleware;

}mamaCapture;

typedef struct captureSources_
{
    char*            mySymbol;
    char*            mySymbolNamespace;
    mamaTransport    myTransport;
    char*            myTransportName;
    mamaSource       mySource;
    mamaSubscription mySubscription;
}mamaCaptureSourceList;

static int gCounter  = 0;
static int gListSize = 0;
static int gShuttingDown  = 0;
char*      gSource = NULL;

/********************************************************************
Globals
*********************************************************************/

typedef mamaCapture* mamaCaptureConfig;
typedef mamaCaptureSourceList* mamaCaptureList;

mamaCaptureConfig* gCapture; /*for cleaning up*/
mamaCaptureList* gCaptureList;


static const char* gUsageString[]=
{
" MamaCapture - Application for capturing and serializing",
"               subscriptions.",
" Examples",
"",
" 1. Capturing from single source",
"    mamacapture -m lbm -MS SOURCE:TRANSPORT:SYMBOLS_FILE",
"",
" 2. Capturing from multiples sources and serializing to ",
"    specified file.",
"    mamacapture -m lbm -MS SOURCE:TRANSPORT:SYMBOLS_FILE",
"    mamacapture -m lbm -MS SOURCE2:TRANSPORT2:SYMBOLS_FILE2",
"                 -outfile <file>",
"",
" 3. To print subscription list, run with -d.",
"",
" 4. To save dictionary use -DF <filename>. If full path is not ",
"    specified in the file name, the file will be saved in ",
"    WOMABT_PATH",
"",
NULL
};

/**************************************************************
xMethod Definitions
***************************************************************/
static mama_status
mamaCaptureConfig_create (mamaCaptureConfig* subCapture);
static mama_status
mamaCaptureList_allocate (mamaCaptureList* captureList);

static mama_status
mamaCaptureList_deallocate (mamaCaptureList captureList);

mama_status
mamaCaptureList_parseCommandInput (mamaCaptureConfig subCapture,
                                   mamaCaptureList captureList,
                                   char* commandArg);

mama_status mamaCaptureConfig_destroy (mamaCaptureConfig subCapture);

static mama_status parseCommandLine (mamaCaptureConfig subCapture,
                                     mamaCaptureList* captureList,
                                     int argc, const char** argv);

static mama_status initializeMama      (mamaCaptureConfig subCapture);

static mama_status msshutdown            (mamaCaptureConfig subCapture,
                                        mamaCaptureList captureList);
static mama_status subscribeToSymbols  (mamaCaptureConfig subCapture,
                                        mamaCaptureList captureList);

void
timeoutCb                  (mamaDictionary      dict,
                            void *              closure);
void
errorCb                    (mamaDictionary      dict,
                            const char *        errMsg,
                            void *              closure);

static void
transportCb (mamaTransport tport,
             mamaTransportEvent,
             short cause,
             const void* platformInfo,
             void *closure);

static void
completeCb                 (mamaDictionary      dict,
                            void *              closure);


static void
listenerMsgCb (mamaSubscription    subscription,
               mamaMsg             msg,
               void *              closure,
               void *              itemClosure);
static void
listenerErrorCb (mamaSubscription    subscription,
                 mama_status         status,
                 void *              platformError,
                 const char *        subject,
                 void *              closure);
static void
listenerCreateCb (mamaSubscription    subscription,
                  void *              closure);

static void usage (int exitStatus);
mama_status saveOutput (mamaCaptureConfig subCapture,
                        const char* sourceName,
                        const char* transportName,
                        const char* symbolName,
                        mamaMsg msg);

static mama_status
createTransport (mamaCaptureConfig subCapture,
                 mamaTransport* transport,
                 char* tportName);
static void dumpList (mamaCaptureList captureList);

static void
signalCleanup            (int);

static void buildDataDictionary (mamaCaptureConfig subCapture);
static void dumpDataDictionary (mamaCaptureConfig subCapture);
/***********************************************************************
Implementation -
************************************************************************/

mama_status
mamaCaptureList_allocate (mamaCaptureList* captureList)
{
    mamaCaptureSourceList* sourceList =
        (mamaCaptureSourceList*)calloc (INITIAL_ARRAY_SIZE,
                                        sizeof (mamaCaptureSourceList));
    gListSize = INITIAL_ARRAY_SIZE;
    *captureList = (mamaCaptureList)sourceList;
    return MAMA_STATUS_OK;
}

mama_status
mamaCaptureList_deallocate (mamaCaptureList captureList)
{
    mamaCaptureSourceList* sourceList =
        (mamaCaptureSourceList*)captureList;
    int i;
    for (i = 0; i < gCounter; i++)
    {
        free ((char*)sourceList[i].mySymbolNamespace);
        free ((char*)sourceList[i].myTransportName);
        free ((char*)sourceList[i].mySymbol);
        mamaSubscription_destroy (sourceList[i].mySubscription);
        mamaSubscription_deallocate (sourceList[i].mySubscription);
        mamaSource_destroy (sourceList[i].mySource);
        if (sourceList[i].myTransport != NULL)
        {
            mamaTransport_destroy (sourceList[i].myTransport);
            sourceList[i].myTransport = NULL;
        }
    }
    free (sourceList);

    return MAMA_STATUS_OK;
}

mama_status
mamaCaptureList_parseCommandInput (mamaCaptureConfig subCapture,
                                   mamaCaptureList captureList,
                                   char* commandArg)
{
    mamaCaptureSourceList* sourceList =
        (mamaCaptureSourceList*)captureList;
    mamaCapture * capture = (mamaCapture*)subCapture;

    char* source        = NULL;
    char* transportName = NULL;
    char* symbolFile    = NULL;
    int   transportCreated  = 0;

    FILE* fp = NULL;
    char charbuf[1024];

    if (capture == NULL) return MAMA_STATUS_NULL_ARG;

    if (sourceList == NULL)
    {
        mama_log (MAMA_LOG_LEVEL_NORMAL,
                  "parseCommandInput : NullPointer " \
                  "Exception ");
        return MAMA_STATUS_NULL_ARG;
    }

    source        = strtok (commandArg, ":");
    transportName = strtok (NULL, ":");
    symbolFile    = strtok (NULL, " ");

    if (symbolFile != NULL)
    {
        if ((fp = fopen (symbolFile, "r")) == (FILE *)NULL)
        {
            perror (symbolFile);
            exit (1);
        }
    }

    while (fgets (charbuf, 1023, fp))
    {
        char *c = charbuf;
        while ((*c != '\0') && (*c != '\n'))
            c++;
        *c = '\0';
        sourceList[gCounter].mySymbolNamespace = strdup (source);
        sourceList[gCounter].myTransportName   = strdup (transportName);
        sourceList[gCounter].mySymbol          = strdup (charbuf);

        mamaSource_create (&sourceList[gCounter].mySource);
        mamaSource_setId (sourceList[gCounter].mySource,
                          sourceList[gCounter].mySymbolNamespace);
        mamaSource_setSymbolNamespace (sourceList[gCounter].mySource,
                                       sourceList[gCounter].mySymbolNamespace);
        if (transportCreated == 0)
        {
            createTransport(capture,&sourceList[gCounter].myTransport,
                            sourceList[gCounter].myTransportName);
            transportCreated = 1;
        }
        else
        {
            sourceList[gCounter].myTransport=
                sourceList[gCounter-1].myTransport;
        }
        mamaSource_setTransport (sourceList[gCounter].mySource,
                                 sourceList[gCounter].myTransport);
        gCounter++;
    }
    fclose (fp);
    fp = NULL;

    return MAMA_STATUS_OK;
}

mama_status createTransport (mamaCaptureConfig subCapture,
                             mamaTransport* transport,
                             char* tportName)
{
    mama_status   status =  MAMA_STATUS_OK;
    mamaCapture*  capture = (mamaCapture*)subCapture;

    if (capture == NULL) return MAMA_STATUS_NULL_ARG;


    status = mamaTransport_allocate (transport);
    if (status != MAMA_STATUS_OK)
    {
        mama_log (MAMA_LOG_LEVEL_NORMAL,
                  "Failed to allocate transport STATUS: %d %s\n", status,
                  mamaStatus_stringForStatus (status));
        exit(1);
    }

    status = mamaTransport_setTransportCallback(*transport, transportCb, NULL);

    status = mamaTransport_create (*transport, tportName, capture->myBridge);

    if (status != MAMA_STATUS_OK)
    {
        mama_log (MAMA_LOG_LEVEL_NORMAL,
                  "Failed to create transport STATUS: %d %s\n", status,
                  mamaStatus_stringForStatus (status));
        exit(1);
    }

    /*if we still haven't create a transport for
      dictionary then do so */
    if (capture->myDictTransport == NULL)
    {
        capture->myDictTransport = *transport;

        mamaSource_create             (&capture->myDictSource);
        mamaSource_setTransport       (capture->myDictSource,
                                      capture->myDictTransport);
        mamaSource_setSymbolNamespace (capture->myDictSource,
                                       capture->myDictSourceName);
        if (status != MAMA_STATUS_OK)
        {
            mama_log (MAMA_LOG_LEVEL_NORMAL,
                      "Failed to create data dictionary transport STATUS: %d %s\n",
                      status, mamaStatus_stringForStatus (status));
            exit(1);
        }
    }
    return status;
}

mama_status
mamaCaptureConfig_create (mamaCaptureConfig* subCapture)
{
    mamaCapture* capture =
        (mamaCapture*)calloc (1, sizeof (mamaCapture));

    if (capture == NULL) return MAMA_STATUS_NOMEM;

    /**default initializations*/
    capture->myRequireInitial     = 1;
    capture->myTimeout            = 40.0;
    capture->myDictSourceName     = "WOMBAT";
    capture->myDictSource         = NULL;
    capture->myDictionary         = NULL;
    capture->myDumpDataDict       = 0;
    capture->myDictToFile         = 0;
    capture->myDictFile           = NULL;
    capture->myBuildDataDict      = 1;
    capture->myNumSubscriptions   = 0;
    capture->myNumSymbols         = 0;
    capture->mySubscLogLevel      = MAMA_LOG_LEVEL_NORMAL;
    capture->myCaptureFilename    = NULL;
    capture->myCapture            = NULL;
    capture->mySubcriptionSymbol  = 0;
    capture->myDictTransport      = NULL;
    capture->myDictionaryComplete = 0;
    capture->myDefaultQueue       = NULL;
    capture->myBridge             = NULL;
    capture->myMiddleware         = "wmw";
    *subCapture = (mamaCaptureConfig)capture;

    return MAMA_STATUS_OK;
}

mama_status mamaCaptureConfig_destroy(mamaCaptureConfig subCapture)
{
    mamaCapture * capture = (mamaCapture*)subCapture;
    if (capture == NULL) return MAMA_STATUS_NULL_ARG;

    free (capture);
    capture = NULL;
    return MAMA_STATUS_OK;
}
/*******************************************************************
Callbacks
*******************************************************************/


static void
transportCb (mamaTransport tport,
             mamaTransportEvent ev,
             short cause,
             const void* platformInfo,
             void *closure)

{
    if (ev == MAMA_TRANSPORT_RECONNECT)
    {
        mama_log (MAMA_LOG_LEVEL_FINE,
                  "RECONNECTED!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    }
    else
    {
        mama_log (MAMA_LOG_LEVEL_FINE,
                  "DISCONNECTED!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    }
}

static void
listenerMsgCb       (mamaSubscription    subscription,
                     mamaMsg             msg,
                     void *              closure,
                     void *              itemClosure)
{
    mamaCaptureSourceList* context = (mamaCaptureSourceList*)closure;

    const char* subSource    = NULL;
    const char* subSymbol    = NULL;
    const char* subTransport = NULL;
    if (context == NULL )
    {
        mama_log (MAMA_LOG_LEVEL_FINE,
                  "listenerMsgCb: Null pointer exception");
        return;
    }
    mamaSubscription_getSource (subscription, &subSource);
    mamaSubscription_getSubscSymbol (subscription, &subSymbol);
    mamaTransport_getName (context->myTransport,
    &subTransport);
    if (!gShuttingDown)
    saveOutput(*gCapture, subSource, subTransport, subSymbol,msg);

    /*DEBUG OUTPUT
    mama_log (MAMA_LOG_LEVEL_NORMAL,
              "Capture [%p], Source [%s], Tport [%s] , Symbol [%s]",
              context->capture, subSource, subTransport , subSymbol);
    */
}
static void
listenerErrorCb (mamaSubscription    subscription,
                             mama_status         status,
                             void *              platformError,
                             const char *        subject,
                             void *              closure)
{
    mama_log (MAMA_LOG_LEVEL_NORMAL,
              "An error occurred creating listener for %s: %s\n",
              subject,
              mamaStatus_stringForStatus (status));
}
static void
listenerCreateCb (mamaSubscription    subscription,
                              void *              closure)
{

}


void
timeoutCb                  (mamaDictionary      dict,
                            void *              closure)
{
    mama_log(MAMA_LOG_LEVEL_NORMAL,
             "Listener Timed out waiting for dictionary\n" );
    mama_stop ((*gCapture)->myBridge);

}
void
errorCb                    (mamaDictionary      dict,
                            const char *        errMsg,
                            void *              closure)
{
    mama_log(MAMA_LOG_LEVEL_NORMAL, "Error getting dictionary: %s\n", errMsg );
    mama_stop ((*gCapture)->myBridge);

}
void
completeCb                 (mamaDictionary      dict,
                            void *              closure)
{
   mamaCapture * capture = (mamaCapture*)closure;
   if (capture == NULL)
   {
       mama_log (MAMA_LOG_LEVEL_FINE,
                 "completeCb: null pointer arguement ");
       return;
   }
   capture->myDictionaryComplete = 1;
   mama_log(MAMA_LOG_LEVEL_NORMAL, "Dictionary creation complete !!!!");

   mama_stop ((*gCapture)->myBridge);
}

/***************************************************************************
Utility Functions
**************************************************************************/

void usage (int exitStatus)
{
    int i = 0;
    while (gUsageString[i]!=NULL)
    {
        printf("%s\n", gUsageString[i++]);
    }
    exit (exitStatus);
}
static void
signalCleanup (int signal)
{

}


void dumpList (mamaCaptureList captureList)
{
    int counter = 0;
    mamaCaptureSourceList * sourceList
        = (mamaCaptureSourceList*)captureList;
    for (counter = 0; counter< gCounter; counter++)
    {
        mama_log (MAMA_LOG_LEVEL_NORMAL,
                  "Source = %s, symbol = %s, tport = %s, " \
                  "tportaddress =%p\n",
                  sourceList[counter].mySymbolNamespace,
                  sourceList[counter].mySymbol,
                  sourceList[counter].myTransportName,
                  sourceList[counter].myTransport);

    }
}

static mama_status initializeMama (mamaCaptureConfig subCapture)
{
    mamaCapture * capture = (mamaCapture*)subCapture;
    mama_status status = MAMA_STATUS_OK;

    status = mama_loadBridge (&capture->myBridge, capture->myMiddleware);

    if (MAMA_STATUS_OK!=status)
    {
        mama_log (MAMA_LOG_LEVEL_NORMAL,"Could not open MAMA %s bridge.",
                                         capture->myMiddleware);
        exit (1);
    }

    status = mama_open ();
    if (status != MAMA_STATUS_OK)
    {
        mama_log (MAMA_LOG_LEVEL_NORMAL,
                  "Failed to initialize MAMA STATUS: %d (%s)\n", status,
                  mamaStatus_stringForStatus (status));
        exit(1);
    }

    mama_getDefaultEventQueue (capture->myBridge, &capture->myDefaultQueue);

    status = mamaCapture_allocate(&capture->myCapture);
    if (status != MAMA_STATUS_OK)
    {
        mama_log (MAMA_LOG_LEVEL_NORMAL,
                  "Failed to create capture STATUS: %d %s\n", status,
                  mamaStatus_stringForStatus (status));
        exit(1);

    }
    if (capture->myDictTport != NULL)
    {
        mama_log (MAMA_LOG_LEVEL_FINE,
                  "Creating data dictionary transport using name: %s\n",
                  capture->myDictTport);
        if (strlen(capture->myDictTport)==0) capture->myDictTport = NULL;

        status = mamaTransport_allocate (&capture->myDictTransport);
        status = mamaTransport_create (capture->myDictTransport,
                                       capture->myDictTport,
                                       capture->myBridge);
        mamaSource_create             (&capture->myDictSource);
        mamaSource_setTransport       (capture->myDictSource,
                                      capture->myDictTransport);
        mamaSource_setSymbolNamespace (capture->myDictSource,
                                       capture->myDictSourceName);
        if (status != MAMA_STATUS_OK)
        {
            mama_log (MAMA_LOG_LEVEL_NORMAL,
                      "Failed to create data dictionary transport STATUS: %d %s\n",
                      status, mamaStatus_stringForStatus (status));
            exit(1);
        }
    }

    return MAMA_STATUS_OK;
}

static mama_status parseCommandLine (mamaCaptureConfig subCapture,
                                     mamaCaptureList* captureList,
                                     int argc, const char** argv)
{
    mamaCapture * capture = (mamaCapture*)subCapture;
    mamaCaptureSourceList* sourceList = NULL;

    int i;
    if (capture == NULL) return MAMA_STATUS_NULL_ARG;

    for (i = 1; i < argc;  )
    {
        if (strcmp (argv[i], "-MS") == 0)
        {
            if (gCounter == 0)
            {
                mamaCaptureList_allocate (&sourceList);
            }
            *captureList = (mamaCaptureList)sourceList;
            gSource = (char*)argv[i + 1];
            i+=2;
        }
        else if (strcmp (argv[i], "-I") == 0)
        {
            capture->myRequireInitial = 0;
            i++;
        }
        else if (strcmp (argv[i], "-d") == 0)
        {
            capture->myDumpDataDict = 1;
            i++;
        }
        else if (strcmp (argv[i], "-subs")==0)
        {
            capture->myDumpList     = 1;
            i++;
        }
        else if (strcmp (argv[i], "-m")==0)
        {
            capture->myMiddleware = argv[i+1];
            i+=2;
        }
        else if (strcmp (argv[i], "-DF") == 0)
        {
            capture->myDictToFile = 1;
            capture->myDictFile  = argv[i+1];
            i+=2;
        }
        else if (strcmp (argv[i], "-B") == 0)
        {
            capture->myBuildDataDict = 0;
            i++;
        }
        else if (strcmp (argv[i], "-outfile") == 0)
        {
            capture->myCaptureFilename = argv[i + 1];
            i += 2;
        }
        else if (strcmp (argv[i], "-r") == 0)
        {
            capture->myThrottle = strtol (argv[i+1], NULL, 10);
            i += 2;
        }
        else if (strcmp (argv[i], "-t") == 0)
        {
            capture->myTimeout = strtod (argv[i+1], NULL);
            i += 2;
        }
        else if (strcmp (argv[i], "-dict_tport") == 0)
        {
            capture->myDictTport = argv[i+1];
            i += 2;
        }
        else if (strcmp (argv[i], "-d") == 0)
        {
            capture->myDictSourceName = argv[i + 1];
            i += 2;
        }
        else if ((strcmp (argv[i], "-help") == 0) ||
                 (strcmp (argv[i], "-?") == 0))
        {
            usage (0);
            i++;
        }
        else if (strcmp (argv[i], "-v") == 0)
        {
            if (mama_getLogLevel() == MAMA_LOG_LEVEL_WARN)
            {
                mama_enableLogging (stderr, MAMA_LOG_LEVEL_NORMAL);
            }
            else if (mama_getLogLevel() == MAMA_LOG_LEVEL_NORMAL)
            {
                mama_enableLogging (stderr, MAMA_LOG_LEVEL_FINE);
            }
            else if (mama_getLogLevel() == MAMA_LOG_LEVEL_FINE)
            {
                mama_enableLogging (stderr, MAMA_LOG_LEVEL_FINER);
            }
            else
            {
                mama_enableLogging (stderr, MAMA_LOG_LEVEL_FINEST);
            }

            i++;
        }

        else if (strcmp( argv[i], "-V" ) == 0)
        {
            if (capture->mySubscLogLevel == MAMA_LOG_LEVEL_NORMAL)
            {
                capture->mySubscLogLevel = MAMA_LOG_LEVEL_FINE;
            }
            else if (capture->mySubscLogLevel == MAMA_LOG_LEVEL_FINE)
            {
                capture->mySubscLogLevel = MAMA_LOG_LEVEL_FINER;
            }
            else
            {
                capture->mySubscLogLevel = MAMA_LOG_LEVEL_FINEST;
            }

            i++;
        }
        else
        {
            i++;
        }
    }


    /*If argv params are not specified
      display usage and exit*/
    if ( *captureList == NULL)
    {
        usage (0);
    }
    return MAMA_STATUS_OK;
}

void buildDataDictionary (mamaCaptureConfig subCapture)
{
    mamaCapture * capture = (mamaCapture*)subCapture;
    mamaDictionaryCallbackSet dictionaryCallback;
    mama_status result;

    if (capture->myBuildDataDict)
    {
        dictionaryCallback.onComplete = completeCb;
        dictionaryCallback.onTimeout  = timeoutCb;
        dictionaryCallback.onError = errorCb;

        result = mama_createDictionary  (&capture->myDictionary,
                                         capture->myDefaultQueue,
                                         dictionaryCallback,
                                         capture->myDictSource,
                                         10.0,
                                         3,
                                         capture);

        if (result != MAMA_STATUS_OK)
        {
            mama_logStdout (MAMA_LOG_LEVEL_NORMAL, "Exception creating "
                            "dictionary: MamaStatus: %s\n",
                            mamaStatus_stringForStatus (result));
            exit (1);
        }

        mama_start (capture->myBridge);

        if (!capture->myDictionaryComplete)
        {
            mama_log(MAMA_LOG_LEVEL_NORMAL, "MamaCapture Could not create dictionary.\n" );
            exit(1);
        }
    }
}

void dumpDataDictionary (mamaCaptureConfig subCapture)
{
    mamaCapture * capture = (mamaCapture*)subCapture;
    mama_fid_t i = 0;

    if (capture->myDictToFile)
    {
        mama_log (MAMA_LOG_LEVEL_NORMAL, "dictionary writing to file [%p]",
                  capture->myDictionary);

        mama_log (MAMA_LOG_LEVEL_NORMAL, "Writing dict to DictFile [%s]",
                   capture->myDictFile);
        mamaDictionary_writeToFile (capture->myDictionary,
                                    capture->myDictFile);
    }

    if (capture->myDumpDataDict)
    {
        size_t size;

        mamaDictionary_getSize (capture->myDictionary, &size);

        for( i = 0; i < size; i++)
        {
            mamaFieldDescriptor field = NULL;
            mamaDictionary_getFieldDescriptorByIndex (capture->myDictionary,
                                                      &field,
                                                      i);
            if (field)
            {
                const char* typeName = "unknown";
                char tmp[100];

                switch (mamaFieldDescriptor_getType (field))
                {
                    case MAMA_FIELD_TYPE_STRING:
                        typeName = "string";
                        break;
                    case MAMA_FIELD_TYPE_BOOL:
                        typeName = "boolean";
                        break;
                    case MAMA_FIELD_TYPE_CHAR:
                    case MAMA_FIELD_TYPE_I8:
                        typeName = "char";
                        break;
                    case MAMA_FIELD_TYPE_U8:
                    case MAMA_FIELD_TYPE_I16:
                    case MAMA_FIELD_TYPE_U16:
                    case MAMA_FIELD_TYPE_I32:
                    case MAMA_FIELD_TYPE_U32:
                    case MAMA_FIELD_TYPE_I64:
                    case MAMA_FIELD_TYPE_U64:
                        typeName = "integer";
                        break;
                    case MAMA_FIELD_TYPE_F64:
                        typeName = "double";
                        break;
                    case MAMA_FIELD_TYPE_PRICE:
                        typeName = "price";
                        break;
                    default:
                        snprintf (tmp, 100, "unknown(%d)",
                                  mamaFieldDescriptor_getType (field));
                        typeName = tmp;
                        break;
                }
                mama_logStdout(MAMA_LOG_LEVEL_NORMAL,"  %3d : %-7s : %s\n",
                               mamaFieldDescriptor_getFid(field), typeName,
                               mamaFieldDescriptor_getName(field));
            }
        }
    }
}
static mama_status msshutdown(mamaCaptureConfig subCapture,
                            mamaCaptureList captureList)
{
    mamaCapture * capture = (mamaCapture*)subCapture;
    mamaCaptureSourceList * sourceList
        = (mamaCaptureSourceList*)captureList;

    if (capture == NULL) return MAMA_STATUS_NULL_ARG;
    if (sourceList == NULL) return MAMA_STATUS_NULL_ARG;

    if (capture->myCapture != NULL)
    {
        mamaCapture_deallocate (capture->myCapture);
        capture->myCapture = NULL;
    }

    mama_close ();
    mamaCaptureConfig_destroy (capture);
    mamaCaptureList_deallocate (sourceList);

    return MAMA_STATUS_OK;
}


static mama_status subscribeToSymbols (mamaCaptureConfig subCapture,
                                       mamaCaptureList captureList)
{
    mamaCapture * capture = (mamaCapture*)subCapture;
    mamaCaptureSourceList * sourceList
        = (mamaCaptureSourceList*)captureList;

    int counter = 0;
    int howMany = 0;

    mama_status status;

    mamaMsgCallbacks callbacks;
    if (capture == NULL) return MAMA_STATUS_NULL_ARG;
    if (sourceList == NULL) return MAMA_STATUS_NULL_ARG;

    callbacks.onCreate          = listenerCreateCb;
    callbacks.onError           = listenerErrorCb;
    callbacks.onMsg             = listenerMsgCb;
    callbacks.onQuality         = NULL;
    callbacks.onGap             = NULL;
    callbacks.onRecapRequest    = NULL;

    printf ("Subscribe to [%d] Symbols \n", gCounter);

    for (counter = 0; counter < gCounter; counter++)
    {
        mamaSubscription_allocate (&sourceList[counter].mySubscription);

        mamaSubscription_setTimeout (sourceList[counter].mySubscription,
                                     capture->myTimeout);
        /* Create a regular market data subscription.  */
        mamaSubscription_setRequiresInitial (sourceList[counter].mySubscription,
                                             capture->myRequireInitial);

        mamaSubscription_setDebugLevel (sourceList[counter].mySubscription,
                                        capture->mySubscLogLevel);

        status =
           mamaSubscription_create (sourceList[counter].mySubscription,
                                    capture->myDefaultQueue,
                                    &callbacks,
                                    sourceList[counter].mySource,
                                    sourceList[counter].mySymbol,
                                    (void*)&sourceList[counter]);

       if (status != MAMA_STATUS_OK)
       {
           mama_log (MAMA_LOG_LEVEL_NORMAL,
                     "Error creating subscription for %s. STATUS: %d\n",
                      sourceList[counter].mySymbol, status);
           continue;
       }
       mama_log (MAMA_LOG_LEVEL_FINE, "Listening on %s.", sourceList[counter].mySymbol);

       if (++howMany % 1000 == 0)
       {
           mama_logStdout (MAMA_LOG_LEVEL_NORMAL,
                           "Subscribed to %d subjects.\n",
                           howMany);
       }
    }
    return MAMA_STATUS_OK;
}

mama_status saveOutput (mamaCaptureConfig subCapture,
                        const char* sourceName, const char* transportName,
                        const char* symbolName, mamaMsg msg)
{
    mamaCapture * capture = (mamaCapture*)subCapture;
    if (capture == NULL) return MAMA_STATUS_NULL_ARG;

    mamaCapture_setTransportName (&capture->myCapture,
                                  transportName);

    mamaCapture_setFeedSource (&capture->myCapture,
                               sourceName);
    mamaCapture_setSymbol (&capture->myCapture,
                           symbolName);

    mamaCapture_saveMamaMsg (&capture->myCapture,
                             &msg);

    return MAMA_STATUS_OK;
}

int main(int argc, const char* argv[])
{
    mamaCaptureConfig mCapture = NULL;
    mamaCaptureList mCaptureList     = NULL;
    mamaCaptureConfig_create (&mCapture);

    if (mCapture == NULL)
    {
        mama_log (MAMA_LOG_LEVEL_NORMAL,
                  "Allocation of memory for capture failed!!!!");
        exit (1);
    }

    gCapture  = &mCapture;
    parseCommandLine (mCapture , &mCaptureList, argc, argv);

    /*Set up a signal handler so that we don't
      just stop without cleaning up*/
    gCaptureList = &mCaptureList;
    initializeMama (mCapture);

    mamaCaptureList_parseCommandInput (mCapture,
                                       mCaptureList, gSource);
    mamaCapture_openFile (&mCapture->myCapture,
                       mCapture->myCaptureFilename);


    buildDataDictionary(mCapture);
    dumpDataDictionary (mCapture);

    if (mCapture->myDumpList)
    {
        dumpList (mCaptureList);
    }
    subscribeToSymbols (mCapture,mCaptureList);

    mama_logStdout (MAMA_LOG_LEVEL_NORMAL, "Type CTRL-C to exit.\n\n");

    mama_start (mCapture->myBridge);

    mamaCapture_closeFile(mCapture->myCapture);
    msshutdown (mCapture,mCaptureList);

    return 0;
}
