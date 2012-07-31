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

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <string.h>
#include <signal.h>

#include <mama/mamacpp.h>
#include <mama/MamaQueueGroup.h>
#include <mama/log.h>

using namespace Wombat;

using std::list;
using std::vector;
using std::string;
using std::cout;
using std::cerr;
using std::endl;
using std::for_each;


typedef list<MamaSubscription *> SubscriptionList;
typedef vector<const char*>      FieldList;  
typedef vector<const char*>      SymbolList;

#define BUFFER_SIZE 256

static  char* gUsageString[]=
{
    "mamaentitlecpp -  Example MAMA API market data subscriber to demonstrate",
    "                  entitlements functionality.",
    "",
    "mamaentitlecpp -S symbolNameSpace [-s symbol | -f symbolFile]",
    "               -tport transportName] [-dict_tport transportName]",
    "               [-d dictSymbolNameSpace] [-t shutdown timer] [-version]",
    "               [-m middleware] [-vg?I1q]",
    "",
    "Options:",
    "",
    "-d                The symbol name space for the data dictionary. Default:WOMBAT",
    "-dict_tport       The name of the transport to use for obtaining the data",
    "                  dictionary.",
    "-f                The path to a file containg a list of subscriptions which the",
    "                  application will subscribe to.",
    "-g                Create group subscription(s)",
    "-I                Disable requesting of initial values.",
    "-m                The middleware to use [wmw/lbm/tibrv]. Default is wmw.",
    "-q                Enter quiet mode. Pass up to 3 times.",
    "-S, -source, -SN  The symbol name space for the data.",
    "-s                An individual symbol to subscribe to.",
    "                  Can be passed multiple times.",
    "-tport            The name of the transport being used. Corresponds to entries",
    "                  in the mama.properties file.",
    "-v                Increase the level of logging verbosity. Pass up to 4",
    "                  times.",
    "-version          Print the version of the API and exit.  Requires -m [middleware]",
    "-1                Create a snapshot subscription (no updates received).",
    "-?, --help        Print this usage information and exit.",
    "-app              Set the application name.",
    "-threads          Number of threads to use when creating subscriptions.",
    "-t                The time in seconds after which the program will exit cleanly.",
    "                  Default is not to exit",
    "",  
    "Example:",
    "",
    "Subscribe to the MSFT BBO from the NASDAQ UTP feed. Feed running with a",
    "symbol namespace of \"NASDAQ\". Middleware is WombatTCP:",
    "",
    "mamaentitlecpp -S NASDAQ -s MSFT -tport tport_name -m wmw",
    NULL
};



class MamaEntitle : public MamaEntitlementCallback
{

public:
    MamaEntitle();
    ~MamaEntitle();
    void parseCommandLine                (int argc, const char* argv[]);   
    void initializeMama                  (void);
    void buildDataDictionary             (void);
    void readSymbolsFromFile             (void);
    void subscribeToSymbols              (void);
    void unsubscribeFromSymbols          (void);
    void start                           (void);
    void stop                            (void);
    void shutdown                        (void);
    void setDictionaryComplete           (bool bComplete);
    int  getQuietness                    (void);
    MamaDictionary* getMamaDictionary    (void);
    void removeSubscription              (MamaSubscription* subscription);
    bool hasSubscriptions                (void);
    bool hasSymbols                      (void);
    MamaEntitle                          (const MamaEntitle& copy);
    MamaEntitle&                         operator= (const MamaEntitle& rhs);
    void*                                getDisplayCallback();
    void enableOutputLog                 ();
    void enableTimedShutdown             ();
    bool displayFieldData                ();
    bool useNewIterators                 ();
    mamaBridge getMamaBridge             ();
    void registerEntitlementCallbacks    ();
    void onSessionDisconnect             (const sessionDisconnectReason reason,
                                          const char* userId,
                                          const char* host,
                                          const char* appName);

    void onEntitlementUpdate             ();
private:
    mamaBridge               mBridgeImpl;
    const char*              mMiddleware;
    MamaQueue*               mDefaultQueue;
    int                      mHighWaterMark;
    int                      mLowWaterMark;
    long                     mThrottle;
    long                     mRecapThrottle;
    int                      mThreads;
    const char*              mTport;
    const char*              mDictSourceName;
    MamaSource*              mDictSource; 
    const char*              mDictTport;
    MamaTransport*           mDictTransport;
    int                      mDumpDataDict;
    bool                     mDictionaryComplete;
    int                      mRequireInitial;
    int                      mSnapshot;
    int                      mGroupSubscription;
    double                   mTimeout;
    int                      mQuietness;
    const char*              mFilename;
    const char*              mMapFilename;
    const char*              mSource;
    MamaLogLevel             mSubscLogLevel;
    MamaSymbolMapFile*       mSymbolMapFromFile;
    MamaDictionary*          mDictionary;
    SymbolList               mSymbolList;
    FieldList                mFieldList;
    void*                    mDisplayCallback;
    SubscriptionList         mSubscriptionList;
    MamaTransport*           mTransport;
    MamaQueueGroup*          mQueueGroup;
    MamaLogLevel             mMamaLogLevel;
    FILE*                    mMamaLogFile;
    bool                     mPrintMessages;
    bool                     mDisplayData;
    bool                     mQualityForAll;
    bool                     mNewIterators;
    long                     mShutdownTimeout;
    MamaTimer*               mShutdownTimer;    

    void usage               (int exitStatus);
    void subscribeToSymbol   (const char* symbol); 

};

/**************************************************************************
 *DisplayCallback -An implementation of the callback interface for   *  
                       -subscriptions                                     *
**************************************************************************/
class DisplayCallback : public MamaSubscriptionCallback
                    
{
public:
    DisplayCallback               () {};
    virtual ~DisplayCallback      (void);
    DisplayCallback               (MamaEntitle*       mamaEntitle);
    virtual void onCreate         (MamaSubscription*  subscription);
    virtual void onError          (MamaSubscription*  subscription,
                                   const MamaStatus&  status,
                                   const char*        subject);
    virtual void onMsg            (MamaSubscription*  subscription,
                                   MamaMsg&           msg);
    virtual void onQuality        (MamaSubscription*  subscription,
                                   mamaQuality        quality,
                                   const char*        symbol,
                                   short              cause,
                                   const void*        platformInfo);   
    virtual void onGap            (MamaSubscription*  subscription);
    virtual void onRecapRequest   (MamaSubscription*  subscription);                                   
 
private:
    MamaEntitle* mMamaEntitle;
    DisplayCallback         (const DisplayCallback& copy);
    DisplayCallback&        operator= (const DisplayCallback& rhs);
};

/******************************************************************
MsgIteratorCallback - Implementation of interface for iterating   *
                    - over all fields in a message.               *
******************************************************************/
class MsgIteratorCallback : public MamaMsgFieldIterator
{
public:
    MsgIteratorCallback ()  {}
    MsgIteratorCallback (MamaEntitle* mamaEntitle);
    
    virtual ~MsgIteratorCallback (void) {}
    void onField                 (const MamaMsg&       msg,
                                  const MamaMsgField&  field,
                                  void*                closure);
private:
    MamaEntitle* mMamaEntitle;
    MsgIteratorCallback          (const MsgIteratorCallback& copy);
    MsgIteratorCallback&         operator= (MsgIteratorCallback& rhs);
};

/******************************************************************
DictionaryCallback  - Implementation of interfaces for dictionary *
                    - population notifications.                   *
******************************************************************/
class DictionaryCallback : public MamaDictionaryCallback
{
public:
    DictionaryCallback          (MamaEntitle* mamaEntitle, mamaBridge bridgeImpl)
                                {mMamaEntitle = mamaEntitle;
                                 mBridgeImpl = bridgeImpl;}
    virtual ~DictionaryCallback (void) {}

    void onTimeout (void)
    {
        cerr << "Listener Timed out waiting for dictionary" << endl;
        Mama::stop (mBridgeImpl);
    }

    void onError (const char* errMsg)
    {
        cerr << "Error getting dictionary: " << errMsg << endl;
        Mama::stop (mBridgeImpl);
    }

    void onComplete (void)
    {
        mMamaEntitle->setDictionaryComplete (true);
        Mama::stop (mBridgeImpl);
    }
private:
    MamaEntitle* mMamaEntitle;
    mamaBridge  mBridgeImpl;
    DictionaryCallback           (const DictionaryCallback& copy);
    DictionaryCallback&          operator= (const DictionaryCallback& rhs);
};

/******************************************************************
ShutdownTimerCallback  - Implementation of interfaces for timed   *
                       - shutdown.                                *
******************************************************************/
class ShutdownTimerCallback : public MamaTimerCallback
{
public:
    ShutdownTimerCallback (MamaEntitle* mamaEntitle)
    {
        mMamaEntitle = mamaEntitle;
    }
    virtual ~ShutdownTimerCallback (void) {}

    void onTimer (MamaTimer* timer)
    {
        printf ("Shutting down MAMA\n");
        mMamaEntitle->stop ();
        timer->destroy();
    }
private:
    MamaEntitle* mMamaEntitle;
};

/******************************************************************
EntitlementsUpdateTimerCallback - Resubscribe to symbols when a   *
                                - dynamic entitlement update is   *
                                - received.                       *
******************************************************************/
class EntitlementsUpdateTimerCallback : public MamaTimerCallback
{
public:
    EntitlementsUpdateTimerCallback (MamaEntitle* mamaEntitle)
    {
        mMamaEntitle = mamaEntitle;
    }
    virtual ~EntitlementsUpdateTimerCallback (void) {}

    void onTimer (MamaTimer* timer)
    {
        fprintf(stdout, "Re-subscribing to symbols ...\n");
        mMamaEntitle->unsubscribeFromSymbols();
        mMamaEntitle->subscribeToSymbols();
        timer->destroy();
    }
private:
    MamaEntitle* mMamaEntitle;
};

MamaEntitle::MamaEntitle():
    mBridgeImpl             (NULL),
    mMiddleware             ("wmw"),
    mDefaultQueue           (NULL),
    mHighWaterMark          (0),
    mLowWaterMark           (0),
    mThrottle               (-1),
    mRecapThrottle          (-1),
    mThreads                (0),
    mTport                  (NULL),
    mDictSourceName         ("WOMBAT"),
    mDictSource             (NULL),
    mDictTport              (NULL),
    mDictTransport          (NULL),
    mDumpDataDict           (0),
    mDictionaryComplete     (false),
    mRequireInitial         (1),
    mSnapshot               (0),
    mGroupSubscription      (0),
    mTimeout                (10.0),
    mQuietness              (0),
    mFilename               (NULL),
    mMapFilename            (NULL),
    mSource                 (NULL),
    mSubscLogLevel          (MAMA_LOG_LEVEL_NORMAL),
    mSymbolMapFromFile      (NULL),
    mDictionary             (NULL),
    mDisplayCallback        (NULL),
    mTransport              (NULL),
    mQueueGroup             (NULL),
    mMamaLogLevel           (MAMA_LOG_LEVEL_WARN),
    mMamaLogFile            (NULL),
    mPrintMessages          (false),
    mDisplayData            (true),
    mQualityForAll          (true),
    mNewIterators           (false),
    mShutdownTimeout        (0)
{}

MamaEntitle::~MamaEntitle()
{
    if (mDisplayCallback)
    {
        delete (DisplayCallback*)mDisplayCallback;
        mDisplayCallback = NULL;
    }
    if (mSymbolMapFromFile != NULL)
    {
        delete mSymbolMapFromFile;
        mSymbolMapFromFile = NULL;
    }
    if (mQueueGroup != NULL)
    {
        delete mQueueGroup;
        mQueueGroup = NULL;
    }
    if (mDictionary != NULL)
    {
        delete mDictionary;
        mDictionary = NULL;
    }
    if (mDictSource != NULL)
    {
        delete mDictSource;
        mDictSource = NULL;
    }
}

void MamaEntitle::setDictionaryComplete (bool bComplete)
{
    mDictionaryComplete  = bComplete;
}

bool MamaEntitle::useNewIterators ()
{
    return (mNewIterators);
}

void* MamaEntitle::getDisplayCallback ()
{
    if (mDisplayCallback != NULL)
    {
        return mDisplayCallback;
    }
    else
    {
        printf(" Subscription callback null pointer error\n");
    }
    return NULL;
}

int  MamaEntitle::getQuietness ()
{
    return mQuietness;
}

bool MamaEntitle::hasSymbols ()
{
    return mSymbolList.empty ();
}

bool MamaEntitle::hasSubscriptions ()
{
    return mSubscriptionList.empty ();
}

void MamaEntitle::parseCommandLine (int argc, const char* argv[])
{
    int i = 0;
    for (i = 1; i < argc;  )
    {
        if ((strcmp (argv[i], "-S") == 0) ||
            (strcmp (argv[i], "-source") == 0))
        {
            mSource = argv[i + 1];
            i += 2;
        }
        else if (strcmp (argv[i], "-d") == 0)
        {
            mDictSourceName = argv[i + 1];
            i += 2;
        }
        else if (strcmp (argv[i], "-dict_tport") == 0)
        {
            mDictTport = argv[i+1];
            i += 2;
        }
        else if (strcmp (argv[i], "-I") == 0)
        {
            mRequireInitial = 0;
            i++;
        }
        else if ((strcmp (argv[i], "-h") == 0) ||
                 (strcmp (argv[i], "-?") == 0))
        {
            usage (0); 
            i++;
        }
        else if (strcmp (argv[i], "-s") == 0)
        {
            mSymbolList.push_back (argv[i + 1]);
            i += 2; 
        }
        else if (strcmp (argv[i], "-f") == 0)
        {
            mFilename = argv[i + 1];
            i += 2;
        }
        else if (strcmp (argv[i], "-1") == 0)
        {
            mSnapshot = 1;
            i++;
        }
        else if (strcmp (argv[i], "-g") == 0)
        {
            mGroupSubscription = 1;
            i++;
        }
        else if (strcmp (argv[i], "-q") == 0)
        {
            mQuietness++;
            i++;
        }
        else if (strcmp (argv[i], "-threads") == 0)
        {
            mThreads = atoi (argv[i + 1]);
            i += 2;
        }
        else if (strcmp (argv[i], "-tport") == 0)
        {
            mTport = argv[i+1];
            i += 2;
        }
        else if (strcmp (argv[i], "-v") == 0)
        {
            if (mMamaLogLevel == MAMA_LOG_LEVEL_WARN)
            {
                mMamaLogLevel = MAMA_LOG_LEVEL_NORMAL;
                mama_enableLogging (stderr, MAMA_LOG_LEVEL_NORMAL); 
            }
            else if (mMamaLogLevel == MAMA_LOG_LEVEL_NORMAL)
            {
                mMamaLogLevel = MAMA_LOG_LEVEL_FINE;
                mama_enableLogging (stderr, MAMA_LOG_LEVEL_FINE); 
            }
            else if (mMamaLogLevel == MAMA_LOG_LEVEL_FINE)
            {
                mMamaLogLevel = MAMA_LOG_LEVEL_FINER;
                mama_enableLogging (stderr, MAMA_LOG_LEVEL_FINER);
            }
            else
            {
                mMamaLogLevel = MAMA_LOG_LEVEL_FINEST;
                mama_enableLogging (stderr, MAMA_LOG_LEVEL_FINEST);
            }

            i++;
        }
        else if (strcmp ("-m", argv[i]) == 0)
        {
            mMiddleware = argv[i+1];
            i += 2;               
        }
        else if (strcmp (argv[i], "-V") == 0)
        {
            if (mSubscLogLevel == MAMA_LOG_LEVEL_NORMAL)
            {
                mSubscLogLevel = MAMA_LOG_LEVEL_FINE;
            }
            else if (mSubscLogLevel == MAMA_LOG_LEVEL_FINE)
            {
                mSubscLogLevel = MAMA_LOG_LEVEL_FINER;
            }
            else
            {
                mSubscLogLevel = MAMA_LOG_LEVEL_FINEST;
            }

            i++;
        }
        else if (strcmp (argv[i], "-version") == 0)
        {
            printf ("%s\n", Mama::getVersion (mBridgeImpl)); 
            exit (0);
        }
        else if (strcmp (argv[i], "-t") == 0)
        {
            mShutdownTimeout = strtol (argv[i+1], NULL, 10);
            i+=2;
        }
        else
        {
            mFieldList.push_back (argv[i]);
            i++;
        }
    }
}

void MamaEntitle::subscribeToSymbol (const char* symbol)
{
    static int howMany = 0;

    if (!mDisplayCallback)
    {
        mDisplayCallback = new DisplayCallback (this);
    }
    
    MamaSubscription* sub = NULL;
    sub = new MamaSubscription;
    
    sub->setRetries (3);
    sub->setRequiresInitial (mRequireInitial);
        
    if (mSnapshot)
    {
        sub->setSubscriptionType (MAMA_SUBSC_TYPE_NORMAL);
        sub->setServiceLevel (MAMA_SERVICE_LEVEL_SNAPSHOT, 0);
    }
    else if (mGroupSubscription)
    {
        sub->setSubscriptionType (MAMA_SUBSC_TYPE_GROUP);
        sub->setServiceLevel (MAMA_SERVICE_LEVEL_REAL_TIME, 0);
    }
    else
    {
        sub->setSubscriptionType (MAMA_SUBSC_TYPE_NORMAL);
        sub->setServiceLevel (MAMA_SERVICE_LEVEL_REAL_TIME, 0);
    }
    
    sub->create (
        mTransport,
        mQueueGroup->getNextQueue (),
        (DisplayCallback*)mDisplayCallback,
        mSource,
        symbol,
        NULL);
    
    mSubscriptionList.push_back (sub);

    if (++howMany % 1000 == 0)
    {
        printf ("Subscribed to %d subjects.\n", howMany);
    }
}

void  MamaEntitle::subscribeToSymbols ()
{
    mQueueGroup = new MamaQueueGroup (mThreads, mBridgeImpl);

    SymbolList::iterator i;
    for (i=mSymbolList.begin(); i != mSymbolList.end(); i++)
    {
        subscribeToSymbol (*i);
    }
}

void MamaEntitle::buildDataDictionary ()
{
    DictionaryCallback cb (this, mBridgeImpl);

    mDictionary = new MamaDictionary;
    mDictionary->create (mDefaultQueue,
                          &cb,
                          mDictSource,
                          3,
                          10.0);

    Mama::start (mBridgeImpl);
    if (mDictionaryComplete == false)
    {
        cerr << "Failed to create dictionary. Exiting." << endl;
        exit (1);
    }
}

void MamaEntitle::start ()
{
    Mama::start (mBridgeImpl);
}

void MamaEntitle::stop ()
{
    Mama::stop (mBridgeImpl);
}

void MamaEntitle::shutdown ()
{
    if (mQueueGroup != NULL)
    {
        mQueueGroup->stopDispatch();
    }
    if (mDictionary != NULL)
    {
        delete mDictionary;
        mDictionary = NULL;
    }
        
    SubscriptionList::const_iterator i;
    
    for (i = mSubscriptionList.begin (); i != mSubscriptionList.end (); i++)
    {
        ((MamaSubscription*)*i)->destroy();
        delete *i;
    }
    
    if (mQueueGroup != NULL)
    {
        delete mQueueGroup;
        mQueueGroup = NULL;
    }
    
    if ((mDictTransport !=  NULL)  && (mDictTransport  != mTransport))                 
    {
        delete mDictTransport;
        mDictTransport = NULL;
    }
    
    if (mTransport != NULL)
    {
        delete mTransport;
        mTransport = NULL;
    }

    Mama::close ();
}

void MamaEntitle::initializeMama ()
{
    mBridgeImpl = Mama::loadBridge (mMiddleware);
    
    Mama::open ();

    mDefaultQueue = Mama::getDefaultEventQueue (mBridgeImpl);
        
    mTransport = new MamaTransport;
    mTransport->create (mTport, mBridgeImpl);
  
    if (mDictTport != NULL)
    {
        if (strlen(mDictTport) == 0)
        {
            mDictTport = NULL;
        }
        mDictTransport = new MamaTransport;
        mDictTransport->create (mDictTport, mBridgeImpl);
        
    }
    else
    {
        mDictTransport=mTransport;
    }
    mDictSource = new MamaSource (mDictSourceName, 
                                  mDictTransport, 
                                  mDictSourceName);
}

bool MamaEntitle::displayFieldData ()
{
    return mDisplayData;
}

MamaDictionary* MamaEntitle::getMamaDictionary ()
{
    return mDictionary;
}

mamaBridge MamaEntitle::getMamaBridge()
{
    return mBridgeImpl;
}
void MamaEntitle::readSymbolsFromFile (void) 
{
        /* get subjects from file or interactively */
        FILE* fp = NULL;
        char charbuf[1024];

        if (mFilename && mFilename[0] )
        {
            if ((fp = fopen (mFilename, "r")) == (FILE *)NULL)
            {
                perror (mFilename);
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
            /* replace newlines with NULLs */
            char *c = charbuf;

            /* Input terminate case */
            if (*c == '.')
                break;

            while ((*c != '\0') && (*c != '\n'))
            {
                c++;
            }
            *c = '\0';

            /* copy the string and subscribe */
            mSymbolList.push_back (strdup (charbuf));
            if (isatty(fileno (fp)))
            {
              printf ("Symbol> ");
            }
        }
}

void MamaEntitle::removeSubscription (MamaSubscription* subscribed)
{
    mSubscriptionList.remove (subscribed);
}

void MamaEntitle::enableOutputLog ()
{
    if (mMamaLogFile != NULL)
    {
        try
        {
            Mama::enableLogging (MAMA_LOG_LEVEL_NORMAL, mMamaLogFile);
        }
        catch (...)
        {
            cerr << "Log File exception" << endl;
        }
    }
}

void MamaEntitle::enableTimedShutdown ()
{
    mShutdownTimer = new MamaTimer;
    if (mShutdownTimeout > 0)
    {
        mShutdownTimer->create (mDefaultQueue, 
                               new ShutdownTimerCallback (this),
                               mShutdownTimeout,
                               NULL);
    }
}

DisplayCallback::DisplayCallback (MamaEntitle* mamaEntitle)
{
    mMamaEntitle = mamaEntitle;
}

DisplayCallback::~DisplayCallback ()
{}

void DisplayCallback::onMsg (MamaSubscription* subscription, 
                             MamaMsg&    msg)
{
    MamaFieldDescriptor* field       = NULL;
    const char*          issueSymbol = NULL;
    mama_i32_t           entitleCode = 0;

    printf ("Update for %s: \n", subscription->getSymbol());    

    field = (mMamaEntitle->getMamaDictionary())->getFieldByName ("wIssueSymbol");
    issueSymbol = msg.getString (field->getName(), field->getFid());

    printf ("Issue symbol: %s", issueSymbol);

    field = (mMamaEntitle->getMamaDictionary())->getFieldByName ("wEntitleCode");
    entitleCode = msg.getI32 (field->getName(), field->getFid());

    printf ("Entitle code: %d", entitleCode);

    flush (cout);
}

/**
 * Method invoked when subscription creation is complete, and before any calls
 * to <code>onMsg</code>.
 * @param subscription The subscription.
 */
void DisplayCallback::onCreate (MamaSubscription* subscriber)
{

}

void DisplayCallback::onGap (MamaSubscription*  subscription)
{
    printf ("Gap for subscription: %s.%s\n",
            subscription->getSubscSource (),
            subscription->getSymbol ());
    flush (cout);
}

void DisplayCallback::onRecapRequest (MamaSubscription*  subscription)
{
    printf ("Recap request for subscription: %s.%s\n",
            subscription->getSubscSource (),
            subscription->getSymbol ());
    flush (cout);
}

void DisplayCallback::onError (MamaSubscription* subscriber, 
                                    const MamaStatus& status,
                                    const char*       subject)
{
    fprintf (stderr,
             "An error occurred creating subscription: %s\n",
             status.toString());
    flush (cout);
}

void DisplayCallback::onQuality (MamaSubscription*  subscription,
                                 mamaQuality        quality,
                                 const char*        symbol,
                                 short              cause,
                                 const void*        platformInfo)
{
    printf ("Quality change: %s\n", 
            mamaQuality_convertToString(quality));
    flush (cout);
}

void MamaEntitle::usage (int exitStatus)
{
   int i = 0;
   while (gUsageString[i] != NULL)
   {
       printf ("%s\n", gUsageString[i++]);
   }
   exit(exitStatus);
}

/**
 * Register entitlement callbacks.
 */
void MamaEntitle::registerEntitlementCallbacks ()
{
    Mama::registerEntitlementCallbacks (this);
}

/**
 * Session disconnect callback.
 *
 * Called when there is a
 * request from the Site Server to
 * disconnect the user's session.
 */
void MamaEntitle::onSessionDisconnect (const sessionDisconnectReason reason,
                                       const char* userId,
                                       const char* host,
                                       const char* appName)
{
    char disconnect_answer[1024];
    char* reason_string = "reason unknown";

    fprintf(stderr, "reason=%d\n", reason);
    switch (reason)
    {
        case SESSION_DISCONNECT_REASON_ADMIN:
            reason_string = "admin request";
            break;
        case SESSION_DISCONNECT_REASON_NEW_CONNECTION:
            reason_string = "new connection";
            break;
        case SESSION_DISCONNECT_REASON_ENTITLEMENTS_UPDATE:
            reason_string = "entitlements update";
            break;
        case SESSION_DISCONNECT_REASON_INVALID:
            reason_string = "reason invalid";
            break;
    }

    printf("Request for session disconnect received.\n");
    printf("Reason=[%s]\nUser id=[%s]\nHost=[%s]\nApplication name=[%s]\n",
           reason_string, userId, host, appName);

    printf("Do you wish to disconnect (Y/N)?\n");
    while (fgets(disconnect_answer, (sizeof(disconnect_answer) - 1), stdin))
    {
        if ((*disconnect_answer == 'Y') || (*disconnect_answer == 'y'))
        {
            /*
             * This will cause mama_start (in main) to return
             * and cause subscriptions to be cleaned up.
             */
            fprintf(stdout, "Disconnecting...\n");
            Mama::stop(mBridgeImpl);
            break;
        }
        else
        {
            fprintf(stdout, "Ignoring disconnect request.\n");
            break;
        }
    }
}

/**
 * Entitlements update callback.
 *
 * Called after a dynamic entitlements update has occurred.
 */
void MamaEntitle::onEntitlementUpdate(void)
{
    long       currentTimeInSeconds;
    char*      currentTimeString;
    MamaTimer* entitlementsUpdateTimer;

    time(&currentTimeInSeconds);
    currentTimeString = ctime(&currentTimeInSeconds);

    /*
     * Since the subscriptions where created
     * by the default event queue,
     * ensure they are also re-created by the
     * default event queue and associated thread.
     * We use a timer to achieve this.
     */
    entitlementsUpdateTimer = new MamaTimer;
    entitlementsUpdateTimer->create(
                    mDefaultQueue,
                    new EntitlementsUpdateTimerCallback(this),
                    0,
                    NULL);

    printf("Dynamic entitlements update received (%20.20s).\n",
           currentTimeString + 4);
}

/*
 * Unsubscribe from all specified symbols.
 */
void MamaEntitle::unsubscribeFromSymbols (void)
{
    SubscriptionList::const_iterator i;

    for (i = mSubscriptionList.begin (); i != mSubscriptionList.end (); i++)
    {
        ((MamaSubscription*)*i)->destroy();
        delete *i;
    }
}


/******************************************************************************
 * Main
 *****************************************************************************/
int main (int argc, const char** argv)
{
    MamaEntitle  mMamaEntitle;
    mMamaEntitle.parseCommandLine (argc, argv);

    if (mMamaEntitle.hasSymbols ())
    {
        mMamaEntitle.readSymbolsFromFile ();
    }

    try
    {
        mMamaEntitle.initializeMama ();
        mMamaEntitle.buildDataDictionary ();
        mMamaEntitle.registerEntitlementCallbacks ();
        mMamaEntitle.subscribeToSymbols ();

        printf ("Type CTRL-C to exit.\n");

        /**
         * Wait for some signal to end.
         */
        mMamaEntitle.enableOutputLog();
        mMamaEntitle.enableTimedShutdown();

        mMamaEntitle.start ();
        mMamaEntitle.shutdown ();
    }
    catch (MamaStatus status)
    {
        cerr << "Exception MamaStatus: " << status.toString () << endl;
    }
    return 0;
}
