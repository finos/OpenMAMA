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

#include "wombat/port.h"
#include <iostream>
#include <list>
#include <string.h>

#include <mama/mamacpp.h>

using namespace Wombat;

using std::list;


typedef list<MamaSubscription *> SubscriptionList;

static const char* gUsageString[] =
{
    "This sample application subscribes to a symbol list and then makes individual",
    "subscriptions to the symbols returned.",
    "",
    "Usage: mamasymbollistsubscribercpp [options]",
    "",
    "Options:",
    "",
    "[-S, -source]      The symbol name space for the data.",
    "[-m middleware]    The middleware to use [wmw/lbm/tibrv].",
    "[-tport name]      The transport parameters to be used from",
    "                   mama.properties.",
    "[-d name]          The symbol name space for the data dictionary. Default:WOMBAT",
    "[-dict_tport] name The name of the transport to use for obtaining the data",
    "                   dictionary.",
    "[-q]               Quiet mode. Suppress output.",
    "[-v]               Increase the level of logging verbosity. Pass up to 4",
    "                   times.",
    "[-threads] n       The number of threads/queues from which to dispatch data.",
    "                   The subscriptions will be created accross all queues in a",
    "                   round robin fashion.",
    "",
    NULL
};

class MamaSymbolListSubscriber
{
public:
    MamaSymbolListSubscriber             ();
    ~MamaSymbolListSubscriber            ();

    MamaDictionary* getMamaDictionary    ();
    mamaBridge getMamaBridge             ();

    void parseCommandLine                (int argc, const char* argv[]);
    void initializeMama                  ();
    void buildDataDictionary             ();
    void setDictionaryComplete           (bool complete);
    void subscribeToSymbols              ();
    void start                           ();
    int  getQuietness                    (); 
    void addSubject                      (string symbol);
    void createSymbolListSubscription    ();

private:
    mamaBridge               mBridgeImpl;
    const char*              mMiddleware;
    MamaQueue*               mDefaultQueue;
    int                      mThreads;
    const char*              mTransportName;
    MamaTransport*           mTransport;
    const char*              mDictSourceName;
    MamaSource*              mDictSource;
    const char*              mDictTransportName;
    MamaTransport*           mDictTransport;
    bool                     mDictionaryComplete; 
    int                      mQuietness;
    int						 mBuildDataDict;
    const char*              mSourceName;
    MamaDictionary*          mDictionary;
    void*                    mSubscriptionCallback;
    void*                    mSymbolListSubscriptionCallback;
    SubscriptionList         mSubscriptionList;
    MamaQueueGroup*          mQueueGroup;
    MamaLogLevel             mMamaLogLevel;
    vector<string>           mSubjectList;

    void usage               (int exitStatus);
};

class SymbolListSubscriptionCallback : public MamaSubscriptionCallback
{
public:
    SymbolListSubscriptionCallback          () {};
    virtual ~SymbolListSubscriptionCallback () {};

    SymbolListSubscriptionCallback          (MamaSymbolListSubscriber* mamaSymbolListSubscriber);

    virtual void onCreate                   (MamaSubscription*  subscription);

    virtual void onError                    (MamaSubscription*  subscription,
                                             const MamaStatus&  status,
                                             const char*        subject);

    virtual void onMsg                      (MamaSubscription*  subscription,
                                             MamaMsg&           msg);

    virtual void onQuality                  (MamaSubscription*  subscription,
                                             mamaQuality        quality,
                                             const char*        symbol,
                                             short              cause,
                                             const void*        platformInfo);

    virtual void onGap                      (MamaSubscription*  subscription);

    virtual void onRecapRequest             (MamaSubscription*  subscription);

    string gettoken                         (string&            instring,  
                                             string             delimiter);

private:
    MamaSymbolListSubscriber* mMamaSymbolListSubscriber;
};


class SubscriptionCallback : public MamaSubscriptionCallback
{
public:
    SubscriptionCallback            () {};
    virtual ~SubscriptionCallback   () {};

    SubscriptionCallback            (MamaSymbolListSubscriber* mamaSymbolListSubscriber);

    virtual void onCreate           (MamaSubscription*  subscription);

    virtual void onError            (MamaSubscription*  subscription,
                                     const MamaStatus&  status,
                                     const char*        subject);

    virtual void onMsg              (MamaSubscription*  subscription,
                                     MamaMsg&           msg);

    virtual void onQuality          (MamaSubscription*  subscription,
                                     mamaQuality        quality,
                                     const char*        symbol,
                                     short              cause,
                                     const void*        platformInfo);

    virtual void onGap              (MamaSubscription*  subscription);

    virtual void onRecapRequest     (MamaSubscription*  subscription);

private:
    MamaSymbolListSubscriber* mMamaSymbolListSubscriber;
};


class DictionaryCallback : public MamaDictionaryCallback
{
public:
    DictionaryCallback (MamaSymbolListSubscriber* mamaSymbolListSubscriber, 
                        mamaBridge bridgeImpl)
    {
        mMamaSymbolListSubscriber = mamaSymbolListSubscriber;
        mBridgeImpl = bridgeImpl;
    }

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
        mMamaSymbolListSubscriber->setDictionaryComplete (true);
        Mama::stop (mBridgeImpl);
    }

private:
    MamaSymbolListSubscriber* mMamaSymbolListSubscriber;
    mamaBridge                mBridgeImpl;
};

SymbolListSubscriptionCallback::SymbolListSubscriptionCallback (MamaSymbolListSubscriber* mamaSymbolListSubscriber)
{
    mMamaSymbolListSubscriber = mamaSymbolListSubscriber;
}

void SymbolListSubscriptionCallback::onCreate (MamaSubscription* subscription)
{
    printf ("Created symbol list subscription");
}

void SymbolListSubscriptionCallback::onError (MamaSubscription* subscription, 
                                              const MamaStatus& status, 
                                              const char*       subject)
{
    fprintf (stderr,
             "Symbol list subscription: An error occurred creating subscription: %s\n",
             status.toString());
    flush (cout);
}

void SymbolListSubscriptionCallback::onMsg (MamaSubscription* subscription, 
                                            MamaMsg&          msg)
{
    const char* newSymbols = NULL;

    switch (msg.getType())
    {
        case MAMA_MSG_TYPE_INITIAL:
            printf("Received initial for symbol list subscription.  Updating list of symbols to subscribe to.\n");
            newSymbols = msg.getString(MamaFieldSymbolList.mName, 
                                       MamaFieldSymbolList.mFid);
            if (strlen(newSymbols) > 0)
            {
                string parsedSymbols = string (newSymbols);
                while (!parsedSymbols.empty())
                {
                    mMamaSymbolListSubscriber->addSubject (gettoken (parsedSymbols, ","));
                }
            }
            break;
        case MAMA_MSG_TYPE_END_OF_INITIALS:
            printf ("End of initials.  Subscribing to symbols.\n");
            mMamaSymbolListSubscriber->subscribeToSymbols ();
            flush (cout);
        default:
            break;
    }
}

string SymbolListSubscriptionCallback::gettoken (string& inString, 
                                                 string delimiter)
{
    string outString;

    if (!inString.empty())
    {
      string::size_type pos = inString.find (delimiter, 0);

      if (pos == string::npos)
      {
        outString = inString;
        inString = "";
      }
      else
      {
        outString = inString.substr (0, (pos));
        inString = inString.substr (pos + 1, inString.size() - pos);
      }
    }
    return outString;
}

void SymbolListSubscriptionCallback::onGap (MamaSubscription*  subscription)
{
    printf ("Gap for symbol list subscription.\n");
    flush (cout);
}

void SymbolListSubscriptionCallback::onRecapRequest (MamaSubscription*  subscription)
{
    printf ("Recap request for symbol list subscription.");
    flush (cout);
}

void SymbolListSubscriptionCallback::onQuality (MamaSubscription*  subscription,
                                                mamaQuality        quality,
                                                const char*        symbol,
                                                short              cause,
                                                const void*        platformInfo)
{
    printf ("Quality change: %s\n",
            mamaQuality_convertToString(quality));
    flush (cout);
}

SubscriptionCallback::SubscriptionCallback (MamaSymbolListSubscriber* mamaSymbolListSubscriber)
{
    mMamaSymbolListSubscriber = mamaSymbolListSubscriber;
}

void SubscriptionCallback::onCreate (MamaSubscription*  subscription)
{}

void SubscriptionCallback::onError (MamaSubscription*  subscription, 
                                    const MamaStatus&  status, 
                                    const char*        subject)
{
    fprintf (stderr,
             "An error occurred creating subscription: %s\n",
             status.toString());
    flush (cout);
}

void SubscriptionCallback::onMsg (MamaSubscription*  subscription, 
                                  MamaMsg&     msg)
{
    if (mMamaSymbolListSubscriber->getQuietness() < 1)
    {
        printf ("Received message of type %s for %s.\n", 
                msg.getMsgTypeName(),
                subscription->getSymbol());
    }   
}

void SubscriptionCallback::onQuality (MamaSubscription*  subscription, mamaQuality        quality, 
                                      const char*        symbol,
                                      short              cause,
                                      const void*        platformInfo)
{
    printf ("%s: quality is now : %s\n",
            symbol,
            mamaQuality_convertToString(quality));
    flush (cout);
}

void SubscriptionCallback::onGap (MamaSubscription*  subscription)
{
    printf ("Gap for subscription: %s.%s\n",
            subscription->getSubscSource (),
            subscription->getSymbol ());
    flush (cout);
}

void SubscriptionCallback::onRecapRequest (MamaSubscription*  subscription)
{
     printf ("Recap request for subscription: %s.%s\n",
             subscription->getSubscSource (),
             subscription->getSymbol ());
    flush (cout);
}

void MamaSymbolListSubscriber::usage (int exitStatus)
{
    int i = 0;
    while (gUsageString[i] != NULL)
    {
        printf ("%s\n", gUsageString[i++]);
    }
    exit(exitStatus);
}

int main (int argc, const char** argv)
{
    MamaSymbolListSubscriber mMamaSymbolListSubscriber;
    mMamaSymbolListSubscriber.parseCommandLine (argc, argv);
    
    try
    {
        mMamaSymbolListSubscriber.initializeMama ();
        mMamaSymbolListSubscriber.buildDataDictionary ();
        mMamaSymbolListSubscriber.createSymbolListSubscription ();
        
        printf ("Type CTRL-C to exit.\n");
        mMamaSymbolListSubscriber.start ();
    }
    catch (MamaStatus status)
    {
        cerr << "Exception MamaStatus: " << status.toString () << endl;
    }
    return 0;
}

MamaSymbolListSubscriber::MamaSymbolListSubscriber ():
    mBridgeImpl                        (NULL),
    mMiddleware                        ("wmw"),
    mDefaultQueue                      (NULL),
    mThreads                           (0),
    mTransportName                     (NULL),
    mTransport                         (NULL),
    mDictSourceName                    ("WOMBAT"),
    mDictSource                        (NULL),
    mDictTransportName                 (NULL),
    mDictTransport                     (NULL),
    mDictionaryComplete                (false),
    mQuietness                         (0),
    mSourceName                        (NULL),
    mDictionary                        (NULL),
    mSubscriptionCallback              (NULL),
    mSymbolListSubscriptionCallback    (NULL),
    mQueueGroup                        (NULL),
    mMamaLogLevel                      (MAMA_LOG_LEVEL_WARN),
	mBuildDataDict                     (true)
{}
  
MamaSymbolListSubscriber::~MamaSymbolListSubscriber ()
{
    if (mSubscriptionCallback)
    {
        delete (SubscriptionCallback*)mSubscriptionCallback;
        mSubscriptionCallback = NULL;
    }
    if (mSymbolListSubscriptionCallback)
    {
        delete (SymbolListSubscriptionCallback*)mSymbolListSubscriptionCallback;
        mSymbolListSubscriptionCallback = NULL;
    }
    if (mQueueGroup)
    {
        delete mQueueGroup;
        mQueueGroup = NULL;
    }
    if (mDictionary)
    {
        delete mDictionary;
        mDictionary = NULL;
    }
    if (mDictSource)
    {
        delete mDictSource;
        mDictSource = NULL;
    }
} 

void MamaSymbolListSubscriber::start ()
{
    Mama::start (mBridgeImpl);
}

void MamaSymbolListSubscriber::buildDataDictionary ()
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

void MamaSymbolListSubscriber::initializeMama ()
{
    mBridgeImpl = Mama::loadBridge (mMiddleware);

    Mama::open ();

    mDefaultQueue = Mama::getDefaultEventQueue (mBridgeImpl);

    mTransport = new MamaTransport;
    mTransport->create (mTransportName, mBridgeImpl);

    if (mDictTransportName != NULL)
    {
        if (strlen(mDictTransportName) == 0)
        {
            mDictTransportName = NULL;
        }
        mDictTransport = new MamaTransport;
        mDictTransport->create (mDictTransportName, mBridgeImpl);
    }
    else
    {
        mDictTransport = mTransport;
    }
    
    mDictSource = new MamaSource (mDictSourceName,
                                  mDictTransport,
                                  mDictSourceName);
}

void MamaSymbolListSubscriber::parseCommandLine (int argc, const char** argv)
{
    int i = 1;

    for (i = 1; i < argc;)
    {
        if ((strcmp (argv[i], "-S") == 0) ||
            (strcmp (argv[i], "-source") == 0)||
            (strcmp (argv[i], "-SN") == 0)||
            (strcmp (argv[i], "-symbolnamespace") == 0))
        {
            mSourceName = argv[i + 1];
            i += 2;
        }
        else if (strcmp (argv[i], "-d") == 0)
        {
            mDictSourceName = argv[i + 1];
            i += 2;
        }
        else if (strcmp ("-m", argv[i]) == 0)
        {
            mMiddleware = argv[i+1];
            i += 2;
        }
        else if (strcmp (argv[i], "-q") == 0)
        {
            mQuietness++;
            i++;
        }
        else if (strcmp (argv[i], "-B") == 0)
		{
			mBuildDataDict = false;
			i++;
		}
        else if (strcmp (argv[i], "-tport") == 0)
        {
            mTransportName = argv[i+1];
            i += 2;
        }
        else if (strcmp (argv[i], "-dict_tport") == 0)
        {
            mDictTransportName = argv[i+1];
            i += 2;
        }
        else if (strcmp (argv[i], "-threads") == 0)
        {
            mThreads = atoi (argv[i + 1]);
            i += 2;
        }
        else if (strcmp (argv[i], "-v") == 0)
        {
            if (mama_getLogLevel () == MAMA_LOG_LEVEL_WARN)
            {
                mama_enableLogging (stderr, MAMA_LOG_LEVEL_NORMAL);
            }
            else if (mama_getLogLevel () == MAMA_LOG_LEVEL_NORMAL)
            {
                mama_enableLogging (stderr, MAMA_LOG_LEVEL_FINE);
            }
            else if (mama_getLogLevel () == MAMA_LOG_LEVEL_FINE)
            {
                mama_enableLogging (stderr, MAMA_LOG_LEVEL_FINER);
            }
            else
            {
                mama_enableLogging (stderr, MAMA_LOG_LEVEL_FINEST);
            }
            i++;
        }
        else if ((strcmp (argv[i], "-?") == 0)||
                 ((strcmp (argv[i], "--help") == 0)))
        {
            usage (0);
        }
    }
}

void MamaSymbolListSubscriber::addSubject (string symbol)
{
    mSubjectList.push_back (symbol);
}

int MamaSymbolListSubscriber::getQuietness (void)
{
    return mQuietness;
}

void MamaSymbolListSubscriber::subscribeToSymbols ()
{
    mQueueGroup           = new MamaQueueGroup (mThreads, mBridgeImpl);
    mSubscriptionCallback = new SubscriptionCallback (this);

    vector<string>::iterator i;
    for (i=mSubjectList.begin(); i != mSubjectList.end(); i++)
    {
        MamaSubscription* sub = NULL;
        sub = new MamaSubscription;
        string symbol = *(i);

        sub->create (mTransport,
                     mQueueGroup->getNextQueue (),
                     (SubscriptionCallback*)mSubscriptionCallback,
                     mSourceName,
                     symbol.c_str(),
                     NULL);

        mSubscriptionList.push_back (sub);
    }
}

void MamaSymbolListSubscriber::createSymbolListSubscription ()
{
    MamaSubscription* sub       = NULL;
    const char* symbolListName  = "SYMBOL_LIST_NORMAL";

    sub = new MamaSubscription;
    mSymbolListSubscriptionCallback = new SymbolListSubscriptionCallback (this);

    sub->setSubscriptionType (MAMA_SUBSC_TYPE_SYMBOL_LIST);
    sub->create (mTransport,
                 mDefaultQueue,
                 (SymbolListSubscriptionCallback*)mSymbolListSubscriptionCallback,
                 mSourceName,
                 symbolListName,
                 NULL);
}

void MamaSymbolListSubscriber::setDictionaryComplete (bool complete)
{
    mDictionaryComplete  = complete;
}

