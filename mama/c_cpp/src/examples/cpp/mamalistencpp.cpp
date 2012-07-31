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

/**
 *
 * MamaListen - Generic Mama API subscriber.
 *
 * <pre>
 * Illustrates the following MAMA API features:
 *   1. Data dictionary initialization.
 *   2. Examining standard fields (e.g. MdMsgType, MdMsgStatus).
 *   3. Walking all received fields in messages.
 *   4. Picking specific fields received in messages.
 *
 * Examples:
 *
 * 1. Display data dictionary with dictionary requested from source
 *    "ORDER" (default dictionary source is "WOMBAT" and used in
 *    subsequent examples, below).
 *
 *    mamalistencpp -D -d ORDER -S NASDAQ -s MSFT
 *
 * 2. Subscribing to top-of-book MSFT quote from Island ITCH feed (all
 *    fields) where the feed handler source is SOURCE:
 *
 *    mamalistencpp -S SOURCE -s MSFT
 *
 * 3. Subscribing to top-of-book MSFT & ORCL from Island ITCH feed
 *    (specific fields):
 *
 *    mamalistencpp -S ORDER -s MSFT.ISLD -s MSFT.ISLD wBidPrice wBidSize 
 *        wAskPrice  wAskSize wTradePrice wTradeVolume wTotalVolume
 *
 * 4. Subscribing to a list of symbols from a file (all fields) and
 *    report updates quietly (an additional -q would eliminate printing
 *    of all normal updates):
 *
 *    mamalistencpp -S NASDAQ -q -f file
 * 
 * 5. Use the -threads x parameter to dispatch messages from x additional
 *    queues.
 * <pre>
 *
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
#include <mama/MamaTimer.h>
#include <mama/MamaTimerCallback.h>

#include <wombat/port.h>

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
    "mamalistencpp - Generic Mama API C++ subscriber.",
    "",
    "Illustrates the following MAMA API features:",
    "   1. Data dictionary initialization.",
    "   2. Examining standard fields (e.g. MdMsgType, MdMsgStatus).",
    "   3. Walking all received fields in messages.",
    "",
    "See the mamalistenc help output for a full list of options.",
    "",
    " Examples:",
    "",
    " 1. Display data dictionary with dictionary requested from source",
    "    \"ORDER\" (default dictionary source is \"WOMBAT\" and used in",
    "    subsequent examples, below). Middleware WombatTCP.",
    "",
    "    mamalistencpp -D -d ORDER -S ORDER -s MSFT.ISLD -m wmw",
    "", 
    " 2. Subscribing to top-of-book MSFT quote from Island ITCH feed (all",
    "    fields) where the feed handler source is ORDER:",
    "",
    "    mamalistencpp -S ORDER -s MSFT.ISLD -m wmw",
    "",
    " 3. Subscribing to top-of-book MSFT & ORCL from Island ITCH feed",
    "    (specific fields). Middleware LBM",
    "",
    "    mamalistencpp -S ORDER -s MSFT.ISLD -s MSFT.ISLD -m lbm wBidPrice wBidSize",
    "    wAskPrice wAskSize wTradePrice wTradeVolume wTotalVolume",
    "",
    " 4. Subscribing to a list of symbols from a file (all fields) and",
    "    report updates quietly (an additional -q would eliminate printing",
    "    of all normal updates). Middleware Tibrv:",
    "",
    "    mamalistencpp -S NASDAQ -q -f file-name.txt -m tibrv",
    NULL
};

class MamaListen : public MamaTimerCallback
{

public:
    MamaListen  ();
    ~MamaListen ();

    void  parseCommandLine                (int argc, const char* argv[]);   
    void  initializeMama                  ();
    void  buildDataDictionary             ();
    void  dumpDataDictionary              ();
    void  loadSymbolMap                   ();
    void  readSymbolsFromFile             ();
    void  subscribeToSymbols              ();
    void  start                           ();
    void  shutdownListener                ();
    void  setDictionaryComplete           (bool bComplete);
    int   getQuietness                    ();
    void  removeSubscription              (MamaSubscription* subscription);
    bool  hasSubscriptions                ();
    bool  hasSymbols                      ();
    bool  hasSpecificFields               ();
    bool  printVectorFields               ();
    void* getDisplayCallback              ();
    void  enableOutputLog                 ();
    bool  displayFieldData                ();
    bool  useNewIterators                 ();

    mamaBridge      getMamaBridge         ();
    MamaDictionary* getMamaDictionary     ();
    virtual void onTimer                  (MamaTimer* timer);
    virtual void onDestroy                (MamaTimer* timer, void* closure);

    MamaListen                            (const MamaListen& copy);
    MamaListen&                           operator= (const MamaListen& rhs);
  
private:
    MamaTimer                mShutdownTimer;
    int                      mShutdownTime;
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
    int						 mBuildDataDict;
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

    DisplayCallback               (MamaListen*        mamaListen,
                                   FieldList*         fieldList);

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
                                  
    void displaySpecificFields    (const MamaMsg&     msg,
                                   MamaSubscription   *subsc );

    void displayField             (const MamaMsg&     msg,
                                   const MamaFieldDescriptor *field);

    void displayAllFields         (const MamaMsg&     msg);

    void subMsgDisplayAllFields   (const MamaMsg&     msg);

    void displayMsgField          (const MamaMsg&     msg, 
                                   const MamaMsgField&  field);

    template <class Vector>
    void displayVectorField       (Vector*            field, 
                                   size_t             size,
                                   char*              format);
 
    template <class T>
    void printData                (char*              format,
                                   T                  data);

    /* This is required for VC6 because it chokes on instantiating
     * the template with "unsigned char.
     */
    void printData               (char*               format,
                                  unsigned char       data);

private:
    MamaListen* mMamaListen;
    FieldList*  mFieldList;
    DisplayCallback              (const DisplayCallback& copy);
    DisplayCallback&             operator= (const DisplayCallback& rhs);
};

/******************************************************************
MsgIteratorCallback - Implementation of interface for iterating   *
                    - over all fields in a message.               *
******************************************************************/
class MsgIteratorCallback : public MamaMsgFieldIterator
{
public:
    MsgIteratorCallback          () {}
    MsgIteratorCallback          (MamaListen* mamaListen);
    
    virtual ~MsgIteratorCallback (void) {}
    void onField                 (const MamaMsg&       msg,
                                  const MamaMsgField&  field,
                                  void*                closure);
private:
    MamaListen* mMamaListen;
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
    DictionaryCallback (MamaListen* mamaListen, mamaBridge bridgeImpl)
    {
        mMamaListen = mamaListen;
        mBridgeImpl = bridgeImpl;
    }

    virtual ~DictionaryCallback (void) {}

    void onTimeout (void)
    {
        cerr << "Listener timed out waiting for dictionary" << endl;
        Mama::stop (mBridgeImpl);
    }

    void onError (const char* errMsg)
    {
        cerr << "Error getting dictionary: " << errMsg << endl;
        Mama::stop (mBridgeImpl);
    }

    void onComplete (void)
    {
        mMamaListen->setDictionaryComplete (true);
        Mama::stop (mBridgeImpl);
    }
private:
    MamaListen*  mMamaListen;
    mamaBridge   mBridgeImpl;
    DictionaryCallback      (const DictionaryCallback& copy);
    DictionaryCallback&     operator= (const DictionaryCallback& rhs);
};

/******************************************************************
TransportCallback   - Implementation of interfaces for            *
                    - transport connection and disconnection      *
******************************************************************/
class TransportCallback : public MamaTransportCallback
{
public:
    TransportCallback          () {}
    virtual ~TransportCallback () {}

    virtual void onDisconnect (MamaTransport *tport)
    {
        printf ("TransportCallback::onDisconnect\n");        
    }

    virtual void onReconnect  (MamaTransport *tport)
    {
        printf ("TransportCallback::onReconnect\n");        
    }

    virtual void onQuality (MamaTransport* tport,
                            short          cause,
                            const void*    platformInfo)
    {
        printf ("TransportCallback::onQuality\n");
        mamaQuality quality = tport->getQuality ();
        printf ("Transport quality is now %s\n",
                 mamaQuality_convertToString (quality));
    }

    virtual void onPublisherDisconnect (MamaTransport *transport, 
                                          const void *platformInfo)
    {
        printf ("TransportCallback::onPublisherDisconnect\n");
    }

    void onNamingServiceConnect (MamaTransport*  transport)
    {
        printf ("TransportCallback::onNamingServiceConnect\n");
    }
    
    void onNamingServiceDisconnect (MamaTransport*  transport)
    {
        printf ("TransportCallback::onNamingServiceDisconnect\n");
    }

private:
    TransportCallback             (const TransportCallback& copy);
    TransportCallback&            operator= (const TransportCallback& rhs);
};

/******************************************************************
Data Quality Callback 
******************************************************************/
class QueueMonitorCallback : public MamaQueueMonitorCallback
{
public:
    QueueMonitorCallback          () {}
    virtual ~QueueMonitorCallback () {}

    virtual void onHighWatermarkExceeded (MamaQueue* queue, 
                                          size_t size, 
                                          void* closure)
    {
        printf ("%s queue high water mark exceeded. Size %d\n",
                queue->getQueueName(), size); 
    }

    virtual void onLowWatermark (MamaQueue* queue, 
                                 size_t size, 
                                 void *closure)
    {
        printf ("%s queue low water mark exceeded. Size %d\n",
                queue->getQueueName(), size); 
    }
private:
    QueueMonitorCallback    (const QueueMonitorCallback& copy);
    QueueMonitorCallback&   operator= (const QueueMonitorCallback& rhs);
};


MamaListen::MamaListen():
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
    mShutdownTime           (0),
	mBuildDataDict			(true)   
{}

MamaListen::~MamaListen()
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

void MamaListen::setDictionaryComplete (bool bComplete)
{
    mDictionaryComplete  = bComplete;
}

bool MamaListen::useNewIterators ()
{
    return (mNewIterators);
}

void* MamaListen::getDisplayCallback ()
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

int MamaListen::getQuietness ()
{
    return mQuietness;
}

bool MamaListen::hasSymbols ()
{
    return mSymbolList.empty ();
}

bool MamaListen::hasSubscriptions ()
{
    return mSubscriptionList.empty ();
}

void MamaListen::parseCommandLine (int argc, const char* argv[])
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
        else if (strcmp (argv[i], "-D") == 0)
        {
            mDumpDataDict = 1;
            i++;
        }
        else if (strcmp (argv[i], "-B") == 0)
        {
            mBuildDataDict = false;
            i++;
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
        else if (strcmp (argv[i], "-r") == 0)
        {
            mThrottle = strtol (argv[i+1], NULL, 10);
            i += 2;
        }
        else if (strcmp (argv[i], "-rr") == 0)
        {
            mRecapThrottle = strtol (argv[i+1], NULL, 10);
            i += 2;
        }
        else if (strcmp (argv[i], "-t") == 0)
        {
            mTimeout = strtod (argv[i+1], NULL);
            i += 2;
        }
        else if (strcmp (argv[i], "-q") == 0)
        {
            mQuietness++;
            i++;
        }
        else if (strcmp (argv[i], "-mp") == 0)
        {
            mMapFilename = argv[i + 1];
            i += 2;
        }
        else if (strcmp (argv[i], "-threads") == 0)
        {
            mThreads = atoi (argv[i + 1]);
            i += 2;
        }
        else if (strcmp (argv[i], "-hw") == 0)
        {
            mHighWaterMark = atoi (argv[i + 1]);
            i += 2;
        }
        else if (strcmp (argv[i], "-lw") == 0)
        {
            mLowWaterMark = atoi (argv[i + 1]);
            i += 2;
        }
        else if (strcmp (argv[i], "-vectors") == 0)
        {
            mPrintMessages = true;
            i++;
        }
        else if (strcmp (argv[i], "-A") == 0)
        {
            mQualityForAll = false;
            i++;
        }
        else if (strcmp (argv[i], "-tport") == 0)
        {
            mTport = argv[i+1];
            i += 2;
        }
        else if (strcmp (argv[i], "-v") == 0)
        {
            if (mMamaLogLevel == MAMA_LOG_LEVEL_WARN)
                mMamaLogLevel = MAMA_LOG_LEVEL_NORMAL;
            else if (mMamaLogLevel == MAMA_LOG_LEVEL_NORMAL)
                mMamaLogLevel = MAMA_LOG_LEVEL_FINE;
            else if (mMamaLogLevel == MAMA_LOG_LEVEL_FINE)
                mMamaLogLevel = MAMA_LOG_LEVEL_FINER;
            else
                mMamaLogLevel = MAMA_LOG_LEVEL_FINEST;

            Mama::enableLogging (mMamaLogLevel, stderr);
            i++;
        }
        else if (strcmp (argv[i], "-no_display") == 0)
        {
            mDisplayData = false;
            i++;
        } 
        else if (strcmp (argv[i], "-log_file") == 0)
        {
            const char* logfileName = argv[i+1];
            if (logfileName == NULL || logfileName[0] == '-'
                || logfileName == "")
            {
                logfileName = "mamalistencpp.log";
                i++;
            }
            else
            {
                i += 2;
            }
            mMamaLogFile = fopen (logfileName, "w+");
            if (mMamaLogFile == NULL)
            {
                fprintf (stderr,"Failed open log file %s\n",
                         argv[i+1]);
                exit (1);
            }
            
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
        else if (strcmp ("-shutdown", argv[i]) == 0)
        {
            mShutdownTime = atoi (argv[i + 1]);
            i += 2;               
        }
        else if (strcmp (argv[i], "-ni") == 0)
        {
            mNewIterators= true;
            ++i;
        }
        else
        {
            mFieldList.push_back (argv[i]);
            i++;
        }
    }
}

void MamaListen::subscribeToSymbol (const char* symbol)
{
    static int howMany = 0;
    

    if (!mDisplayCallback)
    {
        mDisplayCallback = new DisplayCallback (this, &mFieldList);
    }

    MamaSubscription* sub = NULL;
    sub = new MamaSubscription;

    sub->setTimeout          (mTimeout);
    sub->setRetries          (3);
    sub->setRequiresInitial  (mRequireInitial);
    sub->setSubscriptionType (MAMA_SUBSC_TYPE_NORMAL);
    sub->setServiceLevel     (MAMA_SERVICE_LEVEL_REAL_TIME, 0);
    
    if (mSnapshot)
    {
        sub->setServiceLevel (MAMA_SERVICE_LEVEL_SNAPSHOT, 0);
    }

    if (mGroupSubscription)
    {
        sub->setSubscriptionType (MAMA_SUBSC_TYPE_GROUP);
    }

    sub->create (mTransport,
                 mQueueGroup->getNextQueue (),
                 (DisplayCallback*)mDisplayCallback,
                 mSource,
                 symbol,
                 NULL);

    mSubscriptionList.push_back (sub);
    sub->setDebugLevel (mSubscLogLevel);

    if (++howMany % 1000 == 0)
    {
        printf ("Subscribed to %d subjects.\n", howMany);
    }
}

void  MamaListen::subscribeToSymbols ()
{
    int index = 0;
    mQueueGroup = new MamaQueueGroup (mThreads, mBridgeImpl);

    /* Has queue monitoring been enabled? */
    if (mHighWaterMark>0 || mLowWaterMark>0)
    {
        for (index=0; index<mThreads; index++)
        {
            char queueNameBuf[12];
            
            MamaQueue* queue = mQueueGroup->getNextQueue ();
            
            snprintf (queueNameBuf, 12, "QUEUE %d", index);

            printf ("Setting monitor for %s\n", queueNameBuf);

            queue->setQueueName (queueNameBuf);

            queue->setQueueMonitorCallback (new QueueMonitorCallback (), NULL);

            if (mHighWaterMark>0)
            {
                queue->setHighWatermark (mHighWaterMark);
            }
        
            if (mLowWaterMark>0)
            {
                try
                {
                    /*Not supported on all middlewares.*/
                    queue->setLowWatermark (mLowWaterMark);
                }
                catch (MamaStatus status)
                {
                    cerr << "Could not set "
                         << queue->getQueueName () 
                         << "queue low water mark MamaStatus: "
                         << status.toString ()
                         << endl;
                } 
            }
        }
    }

    SymbolList::iterator i;
    for (i=mSymbolList.begin(); i != mSymbolList.end(); i++)
    {
        subscribeToSymbol (*i);
    }
}

void MamaListen::dumpDataDictionary ()
{
    int i = 0;

    if (mDumpDataDict)
    {
        int size = mDictionary->getSize ();
       
        printf ("DICT SIZE: %d\n", size);
        for( i = 0; i < size; i++)
        {
            const MamaFieldDescriptor *field = mDictionary->getFieldByIndex(i);
            if (field)
            {
                printf ("  %3d : %-7s(%4d) : %s\n",
                        field->getFid(), 
                        field->getTypeName(),
                        field->getType(),
                        field->getName());
            }
        }
    }
}

void MamaListen::buildDataDictionary ()
{
    DictionaryCallback cb (this, mBridgeImpl);
    if (mBuildDataDict)
    {
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
}

bool MamaListen::hasSpecificFields ()
{
    if( mFieldList.empty ())
    {   
        return false;
    }
    return true;
}

void MamaListen::onTimer(MamaTimer *timer)
{
	// Destroy the timer
    mShutdownTimer.destroy();

    // Shutdown mama
    Mama::stop(mBridgeImpl);
}

void MamaListen::onDestroy(MamaTimer *timer, void* closure)
{
    printf ("Timer dstroyed\n");
}

void MamaListen::start ()
{
    // Create the shutdown timer
    if(mShutdownTime > 0)
    {
        mShutdownTimer.create(mDefaultQueue, this, (mama_f64_t)mShutdownTime, NULL);
    }

	// Start mama
    Mama::start (mBridgeImpl);
}

void MamaListen::shutdownListener ()
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

    // Destroy all the subscriptions
    SubscriptionList::const_iterator i;

    for (i = mSubscriptionList.begin (); i != mSubscriptionList.end (); i++)
    {
        ((MamaSubscription*)*i)->destroy();
        delete *i;
    } 

    // Destroy the queue group, this must be done after everything using the queues has been destroyed
    if (mQueueGroup != NULL)
    {
        // Destroy all the queues and wait until everything has been cleaned up
        mQueueGroup->destroyWait();
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

void MamaListen::initializeMama ()
{
    mBridgeImpl = Mama::loadBridge (mMiddleware);
    
    Mama::open ();

    mDefaultQueue = Mama::getDefaultEventQueue (mBridgeImpl);
        
    if (mHighWaterMark > 0)
    {
        mDefaultQueue->setHighWatermark (mHighWaterMark);
    }

    if (mLowWaterMark > 0)
    {
        try
        {
            /*Not implemented for all middlewares.*/
            mDefaultQueue->setLowWatermark (mLowWaterMark);
        }
        catch (MamaStatus status)
        {
            cerr << "Could not set default queue"
                    "low water mark MamaStatus: " 
                 << status.toString () 
                 << endl;
        }
    }

    mTransport = new MamaTransport;
    mTransport->setTransportCallback (new TransportCallback ());
    mTransport->create               (mTport, mBridgeImpl);  

    if (!mQualityForAll)
    {
        mTransport->setInvokeQualityForAllSubscs (false);
    }

    if (mThrottle != -1)
    {
        mTransport->setOutboundThrottle (mThrottle, MAMA_THROTTLE_DEFAULT);
    }

    if (mRecapThrottle != -1)
    {
        mTransport->setOutboundThrottle (mRecapThrottle, MAMA_THROTTLE_RECAP);
    }
    
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

bool MamaListen::displayFieldData ()
{
    return mDisplayData;
}

MamaDictionary* MamaListen::getMamaDictionary ()
{
    return mDictionary;
}

mamaBridge MamaListen::getMamaBridge()
{
    return mBridgeImpl;
}
void MamaListen::readSymbolsFromFile (void) 
{
    /* get subjects from file or interactively */
    FILE* fp = NULL;
    char  charbuf[1024];

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

void MamaListen::removeSubscription (MamaSubscription* subscribed)
{
    mSubscriptionList.remove (subscribed);
}

void MamaListen::loadSymbolMap (void)
{
    if (mMapFilename)
    {
        mSymbolMapFromFile = new MamaSymbolMapFile;
        mama_status status = mSymbolMapFromFile->load (mMapFilename);
        if (status != MAMA_STATUS_OK)
        {
            printf ("Failed to initialize symbol map file: %s\n",
                    mMapFilename);
            exit (1);
        }
        mTransport->setSymbolMap (mSymbolMapFromFile);
    }
}

void MamaListen::enableOutputLog ()
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

bool MamaListen::printVectorFields ()
{
    return mPrintMessages;
}

MsgIteratorCallback::MsgIteratorCallback (MamaListen* mamaListen)
{
    mMamaListen = mamaListen;
}

void MsgIteratorCallback::onField (const MamaMsg&       msg,
                                   const MamaMsgField&  field,
                                   void*                closure)
{
    ((DisplayCallback*)mMamaListen
        ->getDisplayCallback())->displayMsgField (msg, field);
}

void DisplayCallback::displayMsgField (const MamaMsg&       msg,
                                       const MamaMsgField&  field)
{
    if (mMamaListen->getQuietness () < 1)
    {
        char fieldBuffer [BUFFER_SIZE];
        sprintf(fieldBuffer,"%20s | %3d | %20s | ",
                field.getName (), field.getFid (),
                field.getTypeName ());
 
       printData ("%s", fieldBuffer);
    }
   
    short fieldType = field.getType ();
    
    switch (fieldType)
    {
    case MAMA_FIELD_TYPE_MSG:
         {
             MamaMsg tmp;
             field.getMsg(tmp);
             subMsgDisplayAllFields (tmp);
         }
         break;
    case MAMA_FIELD_TYPE_OPAQUE:
         break;
    case MAMA_FIELD_TYPE_STRING:
         printData ("%s\n", field.getString ());
         break;
    case MAMA_FIELD_TYPE_BOOL:
         printData ("%d\n", field.getBool ());
         break;
    case MAMA_FIELD_TYPE_CHAR:
         printData ("%c\n", field.getChar ()); 
         break;
    case MAMA_FIELD_TYPE_I8:
         printData ("%d\n", field.getI8 ());
         break;
    case MAMA_FIELD_TYPE_U8:
         printData ("%u\n", (int) field.getU8 ());
         break;
    case MAMA_FIELD_TYPE_I16:
         printData ("%d\n", field.getI16 ());
         break;
    case MAMA_FIELD_TYPE_U16:
         printData ("%u\n", field.getU16 ());
         break;
    case MAMA_FIELD_TYPE_I32:
         printData ("%d\n", field.getI32 ());
         break;
    case MAMA_FIELD_TYPE_U32:
         printData ("%u\n", field.getU32 ());
         break;
    case MAMA_FIELD_TYPE_I64:
         printData ("%lld\n", field.getI64 ());
         break;
    case MAMA_FIELD_TYPE_U64:
         printData ("%llu\n", field.getU64 ());
         break;
    case MAMA_FIELD_TYPE_F32:
         printData ("%f\n", field.getF32 ());
         break;
    case MAMA_FIELD_TYPE_F64:
         printData ("%lf\n", field.getF64 ());
         break;
    case MAMA_FIELD_TYPE_TIME:
         {
             MamaDateTime      mamaDateTime;
             char              dateTime [BUFFER_SIZE]; 
             field.getDateTime (mamaDateTime);
             mamaDateTime.getAsString (dateTime,BUFFER_SIZE);
             printData ("%s\n", dateTime);
         }
         break;
    case MAMA_FIELD_TYPE_PRICE:
         {
             MamaPrice             mamaPrice;
             char                  price [BUFFER_SIZE];
             field.getPrice        (mamaPrice);
             mamaPrice.getAsString (price, BUFFER_SIZE);
             printData             ("%s\n", price);
         } 
         break;
    case MAMA_FIELD_TYPE_VECTOR_I8:
         {    
             const int8_t*      vectorI8;
             size_t             resultLen;
             field.getVectorI8  (vectorI8, resultLen);
             displayVectorField (vectorI8, resultLen, "%d");
         }     
         break;
    case MAMA_FIELD_TYPE_VECTOR_U8:
         {
             const uint8_t*     vectorU8;
             size_t             resultLen;
             field.getVectorU8  (vectorU8, resultLen);
             displayVectorField (vectorU8, resultLen, "%u");
         } 
         break;
    case MAMA_FIELD_TYPE_VECTOR_I16:
         {
             const int16_t*     vectorI16;
             size_t             resultLen;
             field.getVectorI16 (vectorI16, resultLen);
             displayVectorField (vectorI16, resultLen, "%d");
         }  
         break;
    case MAMA_FIELD_TYPE_VECTOR_U16:
         {
             const uint16_t*    vectorU16;
             size_t             resultLen;
             field.getVectorU16 (vectorU16, resultLen);
             displayVectorField (vectorU16, resultLen, "%u");
         }
         break;
    case MAMA_FIELD_TYPE_VECTOR_I32:
         {
             const int32_t*     vectorI32;
             size_t             resultLen;
             field.getVectorI32 (vectorI32, resultLen);
             displayVectorField (vectorI32, resultLen, "%d");
         } 
         break;
    case MAMA_FIELD_TYPE_VECTOR_U32:
         {    
             const uint32_t*    vectorU32;
             size_t             resultLen;
             field.getVectorU32 (vectorU32, resultLen);
             displayVectorField (vectorU32, resultLen, "%u");
         }
         break;
    case MAMA_FIELD_TYPE_VECTOR_I64:
         {
             const int64_t*     vectorI64;
             size_t             resultLen;
             field.getVectorI64 (vectorI64, resultLen);
             displayVectorField (vectorI64, resultLen, "%d");
         }
         break;
    case MAMA_FIELD_TYPE_VECTOR_U64:
         {
             const uint64_t*    vectorU64;
             size_t             resultLen;
             field.getVectorU64 (vectorU64, resultLen);
             displayVectorField (vectorU64, resultLen, "%u");
         }   
         break;
    case MAMA_FIELD_TYPE_VECTOR_F32:
         {
             const mama_f32_t*  vectorF32;
             size_t             resultLen;
             field.getVectorF32 (vectorF32, resultLen);
             displayVectorField (vectorF32, resultLen, "%f");
         }   
         break;
    case MAMA_FIELD_TYPE_VECTOR_F64:
         {
             const mama_f64_t*  vectorF64;
             size_t             resultLen;
             field.getVectorF64 (vectorF64, resultLen);
             displayVectorField (vectorF64, resultLen, "%ld");
         }
         break;
    case MAMA_FIELD_TYPE_VECTOR_STRING:
         {   
             const char**          vectorString;
             size_t                resultLen;
             field.getVectorString (vectorString, resultLen);
             displayVectorField    (vectorString, resultLen, "%s");
         }  
         break;
    case MAMA_FIELD_TYPE_VECTOR_MSG:
         {  
             if (mMamaListen->printVectorFields ())
             {                   
                 size_t             resultLen = 0;
                 const MamaMsg**    mamaMsg   = NULL;
                 field.getVectorMsg (mamaMsg, resultLen);

                 if (resultLen == 0)
                 {
                     return;
                 }

                 for (size_t counter = 0; counter < resultLen; counter++)
                 {
                     subMsgDisplayAllFields (*mamaMsg[counter]);
                 }
             }
             else
             {
                 printData ("%s", "\n");
             }
         }
         break;
    case MAMA_FIELD_TYPE_VECTOR_TIME:
    case MAMA_FIELD_TYPE_VECTOR_PRICE:
    case MAMA_FIELD_TYPE_UNKNOWN:
    default:
         printData ("%s", "\n");
         break;
    }

    
}

DisplayCallback::DisplayCallback (MamaListen* mamaListen,
                                  FieldList*  myFieldList)
{
    mMamaListen = mamaListen;
    mFieldList  = myFieldList;
}

DisplayCallback::~DisplayCallback ()
{   
}

void DisplayCallback::displayField (const MamaMsg&             msg,
                                    const MamaFieldDescriptor* field)
{
    char fieldValueStr[256];
    if (field)
    {
        msg.getFieldAsString (field, fieldValueStr, 256);
        if (mMamaListen->getQuietness () < 1)
        {
            printf ("%20s | %3d | %20s | %s\n",
                    field->getName (),     
                    field->getFid (),
                    field->getTypeName (), 
                    fieldValueStr);
        }
    }
    
}

void DisplayCallback::displayAllFields (const MamaMsg& msg)
{
    printData ("%s", "\n");
    MsgIteratorCallback cb (mMamaListen);


    if (!(mMamaListen->useNewIterators()))
    {
        msg.iterateFields (cb, mMamaListen->getMamaDictionary (), NULL);   
    }
    else
    {
        /* Iterators cannot be used between different queues.  It would
           be possible to create one reusable iterator per queue. */
        MamaMsgIterator* mMamaMsgIterator = new MamaMsgIterator(mMamaListen->getMamaDictionary ());

        msg.begin(*mMamaMsgIterator);
        
        while (*(*mMamaMsgIterator) != NULL)
        {
            displayMsgField (msg, *(*mMamaMsgIterator));
            ++(*mMamaMsgIterator);
        }

        delete mMamaMsgIterator;
    }
}

void DisplayCallback::subMsgDisplayAllFields (const MamaMsg& msg)
{
    if (!(mMamaListen->useNewIterators()))
    {
        displayAllFields (msg);
    }
    else
    {
        MamaMsgIterator subiterator (mMamaListen->getMamaDictionary ());
        
        printData ("%s", "\n {");

        msg.begin(subiterator);
        
        while (*subiterator != NULL)
        {
            displayMsgField (msg, *subiterator);
            ++subiterator;
        }
        
        printData ("%s", "}");
    }
}


void DisplayCallback::onMsg (MamaSubscription* subscription, 
                             MamaMsg&    msg)
{
    switch (msg.getType ())
    {
    case MAMA_MSG_TYPE_DELETE:
    case MAMA_MSG_TYPE_EXPIRE:
         mMamaListen->removeSubscription (subscription);
         subscription->destroy();
         delete subscription;
        
         if (mMamaListen->hasSubscriptions ())
         {
             fprintf (stderr, "Symbol deleted or expired. No more subscriptions\n");
             exit(1);
         }
         return;
         
    default: 
         break;
    }

    switch (msg.getStatus ())
    {
    case MAMA_MSG_STATUS_BAD_SYMBOL:
    case MAMA_MSG_STATUS_EXPIRED:
    case MAMA_MSG_STATUS_TIMEOUT:
         mMamaListen->removeSubscription (subscription);
         subscription->destroy();
         delete subscription;
         
         if (mMamaListen->hasSubscriptions ())
         {
             fprintf (stderr,
                      "Symbol deleted or expired. No more subscriptions\n");
             exit(1);
         }
         return;
    default:
         break;
    }
     
    if (mMamaListen->getQuietness() < 2)
    {
        char msgBuffer [BUFFER_SIZE];
        sprintf (msgBuffer,"%s.%s Type: %s Status %s \n",
                 subscription->getSubscSource (),
                 subscription->getSymbol (),
                 msg.getMsgTypeName (),
                 msg.getMsgStatusString ());
        printData ("%s", msgBuffer); 
    }
    
    if (mMamaListen->hasSpecificFields ())
    {
        displaySpecificFields (msg, subscription);
    }
    else
    {
        displayAllFields (msg);
    }
    flush (cout);
}

void DisplayCallback::displaySpecificFields (const MamaMsg& msg, 
                                             MamaSubscription* subsc)
{
    if (mMamaListen->getQuietness () < 2)
    {
        char msgBuffer[BUFFER_SIZE];
        sprintf(msgBuffer,"%s.%s Type: %s Status %s \n",
                subsc->getSubscSource (),
                subsc->getSymbol (),
                msg.getMsgTypeName (),
                msg.getMsgStatusString ());
        printData ("%s",msgBuffer);
    }

    for (FieldList::iterator i = mFieldList->begin();
         i != mFieldList->end();
         i++)
    {
        const MamaFieldDescriptor *field =
            mMamaListen->getMamaDictionary()->getFieldByName (*i);
        displayField (msg, field);
    }
}

template <class T>
void DisplayCallback::printData (char*        format,
                                 T            data)
{
    if (mMamaListen->getQuietness () < 1)
    {
        if (mMamaListen->displayFieldData ())
        {
            printf (format, data);
        }
    }
}

void DisplayCallback::printData (char*           format,
                                 unsigned char   data)
{
    if (mMamaListen->getQuietness () < 1)
    {
        if (mMamaListen->displayFieldData ())
        {
            printf (format, data);
        }
    }
}

template <class Vector>
void DisplayCallback::displayVectorField  (Vector*  field, 
                                           size_t   size,
                                           char*    format)
{
    if(mMamaListen->printVectorFields ())
    {
        printData ("%s", "[");
        for (size_t counter = 0; counter < size; counter++)
        {
            printData (format, field[counter]);
            counter == size-1 ? 
            printData ("%s", ""):
            printData ("%s", ",");
        }
        printData ("%s", "]");
    }
    printf ("\n");
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

void DisplayCallback::onError (MamaSubscription*      subscriber, 
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




void MamaListen::usage (int exitStatus)
{
   int i = 0;
   while (gUsageString[i] != NULL)
   {
       printf ("%s\n", gUsageString[i++]);
   }
   exit(exitStatus);
}

/******************************************************************************
 * Main
 *****************************************************************************/
int main (int argc, const char** argv)
{
    MamaListen  mMamaListen;
    mMamaListen.parseCommandLine (argc, argv);

    if (mMamaListen.hasSymbols ())
    {
        mMamaListen.readSymbolsFromFile ();
    }

    try
    {
        mMamaListen.initializeMama      ();
        mMamaListen.buildDataDictionary ();
        mMamaListen.dumpDataDictionary  ();
        mMamaListen.loadSymbolMap       ();
        mMamaListen.subscribeToSymbols  ();

        printf ("Type CTRL-C to exit.\n");

        /**
         * Wait for some signal to end.
         */
        mMamaListen.enableOutputLog  ();
        mMamaListen.start            ();
        mMamaListen.shutdownListener ();
    }
    catch (MamaStatus status)
    {
        cerr << "Exception MamaStatus: " << status.toString () << endl;
    }
    return 0;
}
