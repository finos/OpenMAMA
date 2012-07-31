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

/*----------------------------------------------------------------------------
 *
 * This sample application demonstrates how to publish mama messages, and
 * respond to requests from a clients inbox. 
 *
 * It accepts the following command line arguments: 
 *      [-s topic]         The topic on which to send messages. Default value
 *                         is "MAMA_TOPIC".
 *      [-l topic]         The topic on which to listen for inbound requests.
 *                         Default is "MAMA_INBOUND_TOPIC".
 *      [-i interval]      The interval between messages .Default, 0.5.
 *      [-tport name]      The transport parameters to be used from
 *                         mama.properties. 
 *      [-q]               Quiet mode. Suppress output.
 *      [-c number]        How many messages to publish (default infinite).
 *
 *
 *---------------------------------------------------------------------------*/

#include "wombat/port.h"
#include <iostream>

#include "mama/mama.h"
#include "mama/mamacpp.h"
#include "mama/MamaDQPublisherManagerCallback.h"
#include "mama/MamaDQPublisherManager.h"
#include "mama/MamaDQPublisher.h"
#include "mama/MamaQueueEventCallback.h"
#include "mama/subscmsgtype.h"

#include <vector>
#include <list>
#include <string.h>

using namespace Wombat;

using std::cerr;
using std::cout;
using std::endl;
using std::list;
using std::vector;
using std::string;

typedef list<MamaSubscription *> SubscriptionList;
typedef vector<const char*>      SymbolList;

static const char *  gUsageString[] =
{
    " This sample application demonstrates how to publish mama messages, and",
    " respond to requests from a clients inbox.",
    "",
    " It accepts the following command line arguments:",
    "      [-s topic]         The topic on which to send messages. Default value",
    "                         is \"MAMA_TOPIC\".",
    "      [-l topic]         The topic on which to listen for inbound requests.",
    "                         Default is \"MAMA_INBOUND_TOPIC\".",
    "      [-i interval]      The interval between messages .Default, 0.5.",
    "      [-m middleware]    The middleware to use [wmw/lbm/tibrv]. Default is wmw.", 
    "      [-tport name]      The transport parameters to be used from",
    "                         mama.properties. Default is pub",
    "      [-q]               Quiet mode. Suppress output.",
    NULL
};

class SubscriptionHandler;

class MamaProxy : public MamaTimerCallback
{
public:
    MamaProxy  ();
    ~MamaProxy () {};

    void       parseCommandLine       (int argc, const char* argv[]);   
    void       initializeMama         ();
    void       start                  ();
    void       createPublisher        ();
    void       shutdownMama           ();
    void       usage                  (int exitStatus);
    void       onTimer                (MamaTimer* timer);
    void       onDestroy              (MamaTimer* timer, void* closure);

    void       readSymbolsFromFile    ();
    void       subscribeToSymbols     ();
    void       removeSubscription     (MamaSubscription* subscription);

    bool       useNewIterators        ();
    bool       hasSubscriptions       ();
    bool       hasSymbols             ();
    bool       hasSpecificFields      ();
    bool       printVectorFields      ();
    bool       displayFieldData       ();

    void       enableOutputLog        ();

    mamaBridge getMamaBridge          ();
    int        getQuietness           ()  {return mQuietness;}
    void*      getDisplayCallback     ();        
    MamaSubscription* getMamaSubscription (const char * symbol);


    SymbolList               mSymbolList;
private:
    MamaDQPublisherManager*  mPubManager;
    MamaTransport*           mPubTransport;
    const char*              mPubTport;
    mamaBridge               mPubBridge;
    const char*              mPubMiddleware;
    MamaQueue*               mPubDefaultQueue;
    const char*              mPubSource;

    MamaTransport*           mSubTransport;
    const char*              mSubTport;
    mamaBridge               mSubBridge;
    const char*              mSubMiddleware;
    MamaQueue*               mSubDefaultQueue;
    const char*              mSubSource;

    const char*              mFilename;
    bool                     sendSync;
    int                      mQuietness;
    SubscriptionList         mSubscriptionList;
    SubscriptionHandler*     managerCallback;
    MamaLogLevel             mMamaLogLevel;
    MamaTimer*               syncTimer;
};

class TransportCallback : public MamaTransportCallback
{
public:
    TransportCallback          () {}
    virtual ~TransportCallback () {}

    virtual void onDisconnect (MamaTransport *tport)
    {
        printf ("TRANSPORT DISCONNECTED\n");
    }

    virtual void onReconnect (MamaTransport *tport)
    {
        printf ("TRANSPORT RECONNECTED\n");
    }

    virtual void onQuality (MamaTransport* tport,
                            short          cause,
                            const void*    platformInfo)
    {
        printf ("QUALITY - ");
        mamaQuality quality = tport->getQuality ();
        printf ("%s\n", mamaQuality_convertToString (quality));
    }

private:
    TransportCallback   (const TransportCallback& copy);
    TransportCallback&  operator= (const TransportCallback& rhs);
};


class SubscriptionHandler : public MamaDQPublisherManagerCallback
{

public:
    SubscriptionHandler (MamaProxy* mamaProxy) {mMsg.create(); mMamaProxy = mamaProxy;}
    ~SubscriptionHandler() {}
    void onCreate (MamaDQPublisherManager*  publisher);
    
    void onNewRequest (
        MamaDQPublisherManager*  publisherManager,
        const char*              symbol,
        short                    subType,
        short                    msgType,
        MamaMsg&                 msg);
      
    void onRequest (
        MamaDQPublisherManager*  publisherManager,
        const MamaPublishTopic&  publishTopicInfo,
        short                    subType,
        short                    msgType,
        MamaMsg&                 msg);
    
    void onRefresh (
        MamaDQPublisherManager*  publisherManager,
        const MamaPublishTopic&  publishTopicInfo,
        short                    subType,
        short                    msgType,
        MamaMsg&                 msg);
        
    void onError (
        MamaDQPublisherManager*  publisher,
        const MamaStatus&        status,
        const char*              errortxt,
        MamaMsg*                 msg);

private:
    MamaProxy* mMamaProxy;
    MamaMsg    mMsg;
};

class CacheCallback : public MamaSubscriptionCallback, MamaQueueEventCallback
                    
{
public:
    virtual ~CacheCallback      (void);
    
    CacheCallback (MamaProxy* mamaProxy, MamaQueue*  queue);


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

    virtual void onEvent          (MamaQueue& queue,
                                   void* closure);
     
    void setPublisher (MamaDQPublisher* pub) {mpublisher = pub;}
    void sendRecap    ();
    void sendRecap    (MamaMsg*     msg);
    
private:
    MamaProxy*        mMamaProxy;
    CacheCallback     (const CacheCallback& copy);
    CacheCallback&    operator= (const CacheCallback& rhs);
    MamaMsg           *cachedMsg;
    MamaDQPublisher*  mpublisher;
    MamaQueue*        mQueue;
};

CacheCallback::CacheCallback (MamaProxy* mamaProxy, MamaQueue*  queue)
{
    mMamaProxy = mamaProxy;
    mQueue     = queue;
    cachedMsg  = NULL;
	mpublisher = NULL;
}

CacheCallback::~CacheCallback ()
{   

}

void CacheCallback::onMsg (MamaSubscription* subscription, 
                           MamaMsg&          msg)
{
    bool detached = false;

    switch (msg.getStatus ())
    {
    case MAMA_MSG_STATUS_BAD_SYMBOL:
    case MAMA_MSG_STATUS_EXPIRED:
    case MAMA_MSG_STATUS_TIMEOUT:
         mMamaProxy->removeSubscription (subscription);
         subscription->destroy();
         delete subscription;
         break;
    default:
         break;
    }

    switch (msg.getType ())
    {
    case MAMA_MSG_TYPE_DELETE:
    case MAMA_MSG_TYPE_EXPIRE:
         mMamaProxy->removeSubscription (subscription);
         subscription->destroy();
         delete subscription;
         if (mpublisher)
         {
             mpublisher->send (&msg);
         }
         return;
    case MAMA_MSG_TYPE_RECAP:
    case MAMA_MSG_TYPE_INITIAL:
         if (!cachedMsg)
         {
             detached = true;
             cachedMsg = msg.detach();
         }
         else
         {
             cachedMsg->clear();
             cachedMsg->applyMsg(msg);
         }
         break;
    default: 
    	 cachedMsg->applyMsg(msg);
         break;
    }

    if (mpublisher)
    {
    	if (detached)
    	{
            mpublisher->setStatus (cachedMsg->getStatus ());
            mpublisher->send      (cachedMsg);
    	}
    	else
    	{
            mpublisher->setStatus (msg.getStatus ());
            mpublisher->send      (&msg);
    	}
    }
}
void CacheCallback::onEvent(MamaQueue& queue, void* closure)
{
    if ((mpublisher) && (cachedMsg))
    {
        if (closure)
        {
        	try 
            {
                cachedMsg->updateU8 (NULL, 
                                     MamaFieldMsgType.mFid, 
                                     MAMA_MSG_TYPE_INITIAL);
            }
            catch (MamaStatus &status)
            {
                cachedMsg->updateI16 (NULL, 
                                      MamaFieldMsgType.mFid, 
                                      MAMA_MSG_TYPE_INITIAL);
            }

            MamaMsg *request = (MamaMsg *)closure;
            mpublisher->sendReply (*request, cachedMsg);
            delete request;
        }
        else
        {
        	try 
            {
                cachedMsg->updateU8 (NULL, 
                                     MamaFieldMsgType.mFid, 
                                     MAMA_MSG_TYPE_RECAP);
            }
            catch (MamaStatus &status)
            {
                cachedMsg->updateI16 (NULL, 
                                      MamaFieldMsgType.mFid, 
                                      MAMA_MSG_TYPE_RECAP);
            }
            mpublisher->send (cachedMsg);
        }
    }
            
}

void CacheCallback::sendRecap ()
{
    mQueue->enqueueEvent  (this, NULL);                        
}

void CacheCallback::sendRecap (MamaMsg* request)
{
    mQueue->enqueueEvent  (this, request);                        
}

void CacheCallback::onCreate (MamaSubscription* subscriber)
{
}

void CacheCallback::onGap (MamaSubscription*  subscription)
{
    printf ("Gap for subscription: %s.%s\n",
             subscription->getSubscSource (),
             subscription->getSymbol ());
    flush (cout);
}

void CacheCallback::onRecapRequest (MamaSubscription*  subscription)
{
    printf ("Recap request for subscription: %s.%s\n",
            subscription->getSubscSource (),
            subscription->getSymbol ());
    flush (cout);
}

void CacheCallback::onError (MamaSubscription* subscriber, 
                             const MamaStatus& status,
                             const char*       subject)
{
    fprintf (stderr,
             "An error occurred creating subscription: %s\n",
             status.toString());
    flush (cout);
}

void CacheCallback::onQuality (MamaSubscription*  subscription,
                               mamaQuality        quality,
                               const char*        symbol,
                               short              cause,
                               const void*        platformInfo)
{
    printf ("Quality change: %s\n", 
            mamaQuality_convertToString(quality));
    flush (cout);
}
                                   
                                   
void MamaProxy::createPublisher ()
{
    managerCallback =  new SubscriptionHandler(this);
    srand ( time(NULL) );
    
    mPubManager = new MamaDQPublisherManager();
  
    mPubManager->create (mPubTransport, 
                         Mama::getDefaultEventQueue(mPubBridge),
                         managerCallback,
                         mPubSource);
          
	mPubManager->setSeqNum (0);
	mPubManager->setSenderId (0);
 
    if (sendSync)
    {
        syncTimer = new MamaTimer();
        syncTimer->create(Mama::getDefaultEventQueue (mPubBridge), 
                          this,
                          15,
                          NULL);         
    }                                   
}

    
void MamaProxy::start ()
{
    if (mPubBridge != mSubBridge)
    {
        Mama::startBackground (mPubBridge, NULL);
    }
    Mama::start(mSubBridge);
}

void MamaProxy::removeSubscription (MamaSubscription* subscribed)
{
    mSubscriptionList.remove (subscribed);
}


void MamaProxy::subscribeToSymbols ()
{
    MamaSubscription* sub = NULL;
    CacheCallback*    cb  = NULL;
    
    SymbolList::iterator i;
    for (i=mSymbolList.begin(); i != mSymbolList.end(); i++)
    {
        sub = new MamaSubscription;
        cb = new CacheCallback (this, Mama::getDefaultEventQueue(mSubBridge));
        sub->create (mSubTransport,
                     Mama::getDefaultEventQueue(mSubBridge),
                     cb,
                     mSubSource,
                     *i,
                     NULL);

        mSubscriptionList.push_back (sub);
    }
}

void MamaProxy::initializeMama ()
{
    mPubBridge = Mama::loadBridge (mPubMiddleware);
    mSubBridge = Mama::loadBridge (mSubMiddleware);
    
    Mama::open ();
        
    mSubTransport = new MamaTransport;
    mSubTransport->create (mSubTport, mSubBridge);
    mSubTransport->setTransportCallback (new TransportCallback ());
        
    mPubTransport = new MamaTransport;
    mPubTransport->create (mPubTport, mPubBridge);
    mPubTransport->setTransportCallback (new TransportCallback ());
}
    
void MamaProxy::onTimer (MamaTimer* timer)
{
    timer->destroy();
    mPubManager->sendSyncRequest(50, 5, 30);

    printf ("Sent Sync Request\n");
	flush (cout);
}

void MamaProxy::onDestroy(MamaTimer* timer, void* closure)
{
    printf ("Timer destroyed\n");
}

void MamaProxy::shutdownMama ()
{
   
}

int main (int argc, const char **argv)
{
    MamaProxy  mMamaProxy;
    mMamaProxy.parseCommandLine (argc, argv);
    
    try
    {
        mMamaProxy.initializeMama ();
		mMamaProxy.createPublisher();
        mMamaProxy.subscribeToSymbols();
        mMamaProxy.start();
        mMamaProxy.shutdownMama();
    }
    catch (MamaStatus &status)
    {
        cerr << "Caught MAMA exception: " << status.toString () << endl;
        exit (1);
    }
}

void SubscriptionHandler::onCreate (MamaDQPublisherManager*  publisher)
{
    printf ("Created publisher subscription.\n");
	flush  (cout);
}

void SubscriptionHandler::onError (MamaDQPublisherManager*  publisher,
                                   const MamaStatus&        status,
                                   const char*              errortxt,
                                   MamaMsg*                 msg) 
{
    if (msg)
    {
        printf ("Unhandled Msg: %s (%s) %s\n", 
                status.toString (), 
                msg->toString (), 
                errortxt);
    }
    else
    {
        printf ("Unhandled Msg: %s %s\n", 
                status.toString (), 
                errortxt);
    }
    flush (cout);
}

void SubscriptionHandler::onNewRequest (MamaDQPublisherManager*  publisherManager,
                                        const char*              symbol,
                                        short                    subType,
                                        short                    msgType,
                                        MamaMsg&                 msg)
{
    MamaMsg*          request = NULL;	
    MamaSubscription* mSub    = mMamaProxy->getMamaSubscription (symbol);
    
    if (mSub)	
    {
        CacheCallback* cb     = (CacheCallback*) mSub->getCallback ();
        MamaDQPublisher *mPub = publisherManager->createPublisher (symbol, cb);   
		
        printf ("Received New request: %s\n",  symbol);

        cb->setPublisher(mPub);
		
        switch (msgType)
        {
        case MAMA_SUBSC_SUBSCRIBE:
        case MAMA_SUBSC_SNAPSHOT:
            request = msg.detach();
            cb->sendRecap(request);
            break;
        default:
            cb->sendRecap();
            break;
        }
    }
    else if (subType==MAMA_SUBSC_TYPE_SYMBOL_LIST_NORMAL || subType==MAMA_SUBSC_TYPE_SYMBOL_LIST)
    {
        MamaDQPublisher *mPub = publisherManager->createPublisher(symbol, NULL);

        int numSymbols = 0;
        SymbolList::iterator i;
        string symbols;
        MamaMsg *aMsg = new MamaMsg;
        aMsg->create();

        for (i = mMamaProxy->mSymbolList.begin(); i != mMamaProxy->mSymbolList.end(); i++)
        {
            if (numSymbols > 0)
            {
                symbols += ",";
            }
            numSymbols++;
            symbols += *i;

            if (numSymbols>49)
            {
                aMsg->addString (NULL, 
                                 MamaFieldSymbolList.mFid, 
                                 symbols.c_str());
                aMsg->addU8     (NULL, 
                                 MamaFieldMsgType.mFid, 
                                 MAMA_MSG_TYPE_INITIAL);
                mPub->sendReply(msg, aMsg);
                aMsg->clear();
                numSymbols = 0;
                symbols = "";
            }
        }
        if (numSymbols>0)
        {
            aMsg->addString (NULL, MamaFieldSymbolList.mFid, symbols.c_str());
            aMsg->addU8 (NULL, MamaFieldMsgType.mFid, MAMA_MSG_TYPE_INITIAL);
            mPub->sendReply(msg, aMsg);
            aMsg->clear();
        }
    }
    else
    {
        printf ("Received request for unknown symbol: %s\n",  symbol);
    }
    flush (cout);
}
  
void SubscriptionHandler::onRequest (MamaDQPublisherManager*  publisherManager,
                                     const MamaPublishTopic&  publishTopicInfo,
                                     short                    subType,
                                     short                    msgType,
                                     MamaMsg&                 msg)
{  
    MamaMsg* request = NULL;
    printf ("Received request: %s\n",  publishTopicInfo.mSymbol);
    
    if (subType == MAMA_SUBSC_TYPE_SYMBOL_LIST_NORMAL || subType == MAMA_SUBSC_TYPE_SYMBOL_LIST)
    {

        MamaMsg *aMsg = new MamaMsg;
        aMsg->create();

        int numSymbols = 0;
        string symbols;
        SymbolList::iterator i;

        for (i = mMamaProxy->mSymbolList.begin(); i != mMamaProxy->mSymbolList.end(); i++)
        {
            if (numSymbols > 0)
            {
                symbols += ",";
            }

            numSymbols++;
            symbols += *i;

            if (numSymbols>49)
            {
                aMsg->addString (NULL, 
                                 MamaFieldSymbolList.mFid, 
                                 symbols.c_str());
                aMsg->addU8     (NULL, 
                                 MamaFieldMsgType.mFid, 
                                 MAMA_MSG_TYPE_INITIAL);

                publishTopicInfo.mPub->sendReply(msg, aMsg);
                aMsg->clear();
                numSymbols = 0;
                symbols = "";
            }
        }

        if (numSymbols > 0)
        {
            aMsg->addString (NULL, 
                             MamaFieldSymbolList.mFid, 
                             symbols.c_str());
            aMsg->addU8     (NULL, 
                             MamaFieldMsgType.mFid, 
                             MAMA_MSG_TYPE_INITIAL);
            publishTopicInfo.mPub->sendReply(msg, aMsg);
            aMsg->clear();
        }
        return;
    }

    switch (msgType)
    {      
        case MAMA_SUBSC_SUBSCRIBE:
        case MAMA_SUBSC_SNAPSHOT:
             request = msg.detach();
             ((CacheCallback*)publishTopicInfo.mCache)->sendRecap(request);
             break;

        case MAMA_SUBSC_DQ_SUBSCRIBER :
        case MAMA_SUBSC_DQ_PUBLISHER:
        case MAMA_SUBSC_DQ_NETWORK:
        case MAMA_SUBSC_DQ_UNKNOWN:
        case MAMA_SUBSC_DQ_GROUP_SUBSCRIBER:
             ((CacheCallback*)publishTopicInfo.mCache)->sendRecap();
             break;

        case MAMA_SUBSC_UNSUBSCRIBE:
        case MAMA_SUBSC_RESUBSCRIBE:
        case MAMA_SUBSC_REFRESH:
        default:
             break;
    }
    flush (cout);
}

void SubscriptionHandler::onRefresh (MamaDQPublisherManager*  publisherManager,
                                     const MamaPublishTopic&  publishTopicInfo,
                                     short                    subType,
                                     short                    msgType,
                                     MamaMsg&                 msg)
{  
    printf ("Received Refresh: %s\n",  publishTopicInfo.mSymbol);
	flush (cout);
} 
    
MamaSubscription* MamaProxy::getMamaSubscription (const char * symbol) 
{
    SubscriptionList::const_iterator i;
    
    for (i = mSubscriptionList.begin (); i != mSubscriptionList.end (); i++)
    {
        if  (strcmp(((MamaSubscription*)*i)->getSymbol() , symbol) == 0)
        {
            return (MamaSubscription*)*i;
        }
    }
    return NULL;
}

void MamaProxy::readSymbolsFromFile (void) 
{
    /* Get subjects from file or interactively */
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
        /* Replace newlines with NULLs */
        char *c = charbuf;

        /* Input terminate case */
        if (*c == '.')
        {
            break;
        }

        while ((*c != '\0') && (*c != '\n'))
        {
            c++;
        }
        *c = '\0';

        /* Copy the string and subscribe */
        mSymbolList.push_back (strdup (charbuf));

        if (isatty(fileno (fp)))
        {
            printf ("Symbol> ");
        }
    }
}

void MamaProxy::parseCommandLine (int argc, const char **argv)
{
    int i = 0;
    for (i = 1; i < argc;)
    {
        if (strcmp (argv[i], "-SS") == 0)
        {
            mSubSource = argv[i + 1];
            i += 2;
        }
        else if (strcmp (argv[i], "-SP") == 0)
        {
            mPubSource = argv[i + 1];
            i += 2;
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
        else if ((strcmp (argv[i], "-h") == 0) ||
                 (strcmp (argv[i], "-?") == 0))
        {
            usage (0); 
            i++;
        }
        else if (strcmp ("-tports", argv[i]) == 0)
        {
            mSubTport = argv[i+1];
            i += 2;
        }
        else if (strcmp ("-tportp", argv[i]) == 0)
        {
            mPubTport = argv[i+1];
            i += 2;
        }
        else if (strcmp ("-ms", argv[i]) == 0)
        {
            mSubMiddleware = argv[i+1];
            i += 2;              
        }
        else if (strcmp ("-mp", argv[i]) == 0)
        {
            mPubMiddleware = argv[i+1];
            i += 2;                 
        }
        else if (strcmp ("-q", argv[i]) == 0)
        {
            mQuietness++;
            i++;
        }
        else if (strcmp ("-sync", argv[i]) == 0)
        {
            sendSync=true;
            i++;
        }
        else if (strcmp( argv[i], "-v") == 0 )
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
        else
        {
            printf ("Unknown arg %s\n", argv[i]);
            usage (0); 
        }
    }
    
    if (mSymbolList.empty ())
    {
        readSymbolsFromFile ();
    }
}

void MamaProxy::usage (int exitStatus)
{
   int i = 0;
   while (gUsageString[i] != NULL)
   {
       printf ("%s\n", gUsageString[i++]);
   }
   exit (exitStatus);
}

MamaProxy::MamaProxy():
    mPubBridge          (NULL),
    mSubBridge          (NULL),
    mPubMiddleware      ("wmw"),
    mSubMiddleware      ("wmw"),
    mPubDefaultQueue    (NULL),
    mSubDefaultQueue    (NULL),
    mPubTport           (NULL),
    mSubTport           (NULL),
    mQuietness          (0),
    mFilename           (NULL),
    mPubSource          ("MAMA_PROXY"),
    mSubSource          (NULL),
    sendSync            (false),
    mPubTransport       (NULL),
    mSubTransport       (NULL),
    mPubManager         (NULL),
    mMamaLogLevel       (MAMA_LOG_LEVEL_WARN)
{}

