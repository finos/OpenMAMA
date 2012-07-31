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

#include <mama/mamacpp.h>
#include <mama/MamaDQPublisherManagerCallback.h>
#include <mama/MamaDQPublisherManager.h>
#include <mama/MamaDQPublisher.h>
#include <mama/MamaQueueGroup.h>
#include <mama/MamaSource.h>
#include <mama/MamaDQPublisher.h>
#include <mama/subscmsgtype.h>

#include <mamda/MamdaOrderBookFields.h>
#include <mamda/MamdaSubscription.h>
#include <mamda/MamdaCommonFields.h>
#include <mamda/MamdaOrderBookHandler.h>
#include <mamda/MamdaOrderBookListener.h>
#include <mamda/MamdaOrderBook.h>
#include <mamda/MamdaOrderBookPriceLevel.h>
#include <mamda/MamdaOrderBookEntry.h>
#include <mamda/MamdaErrorListener.h>
#include <mamda/MamdaQualityListener.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <list>
#include <set>
#include "../parsecmd.h"
#include "../dictrequester.h"
#include <sstream>

using namespace Wombat;

typedef list<MamdaSubscription*> SubscriptionList;
typedef list<const char*> SymbolList;

class BookTicker : public MamdaOrderBookHandler
{
public:
    BookTicker () {}
    virtual ~BookTicker () {}
    
void callMamdaOnMsg (MamdaSubscription* sub, MamaMsg& msg)
{
    try
    {
        vector<MamdaMsgListener*>& msgListeners = sub->getMsgListeners();
        unsigned long size = msgListeners.size();
        for (unsigned long i = 0; i < size; i++)
        {
            MamdaMsgListener* listener = msgListeners[i];
            listener->onMsg (sub, msg, (short)msg.getType());
        }  
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Exception: Unknown" << std::endl;
    }
    return;
}


    void onBookRecap (
        MamdaSubscription*                 subscription,
        MamdaOrderBookListener&            listener,
        const MamaMsg*                     msg,
        const MamdaOrderBookComplexDelta*  delta,
        const MamdaOrderBookRecap&         recap,
        const MamdaOrderBook&              book)
    {
        printf("\n Book Recap \n");
        book.dump(std::cout);
    }
    
    void onBookDelta (
        MamdaSubscription*                 subscription,
        MamdaOrderBookListener&            listener,
        const MamaMsg*                     msg,
        const MamdaOrderBookSimpleDelta&   delta,
        const MamdaOrderBook&              book)
    {
        printf("\n Book Delta \n");
        book.dump(std::cout);
    }
    
   void onBookComplexDelta (
        MamdaSubscription*                 subscription,
        MamdaOrderBookListener&            listener,
        const MamaMsg*                     msg,
        const MamdaOrderBookComplexDelta&  delta,
        const MamdaOrderBook&              book)
    {
        printf("\n Book Complex Delta \n");
        book.dump(std::cout);
    }
    void onBookClear (
        MamdaSubscription*          subscription,
        MamdaOrderBookListener&     listener,
        const MamaMsg*              msg,
        const MamdaOrderBookClear&  clear,
        const MamdaOrderBook&       book)
    {
        printf("\n Book Clear \n");
        book.dump(std::cout);
    }

    void onBookGap (
        MamdaSubscription*          subscription,
        MamdaOrderBookListener&     listener,
        const MamaMsg*              msg,
        const MamdaOrderBookGap&    event,
        const MamdaOrderBook&       book)
    {
        printf("\n Book Gap \n");
        book.dump(std::cout);
    }
};


class SubscriptionHandler;

class BookPublisher : public MamaTimerCallback
{
public:
    BookPublisher (); 
    ~BookPublisher () {};
       
    void                onTimer              (MamaTimer*  timer);
    void                subscribeToSymbols   ();
    void                acquireLock          (const char* symbol);                               
    void                releaseLock          (const char* symbol);
    void                createPublisher      ();
    void                createTimer          ();
    void                publishMessage       (MamaMsg*    request, 
                                              const char* symbol);
    void                start                ();
    void                initializeMama       ();
    void                createSources        ();
    void                setSymbolList        (const vector<const char*>& list);
    void                setSubSourceName     (const char* source);
    void                setPubSourceName     (const char* source);
    void                setSubTport          (const char* tport);
    void                setPubTport          (const char* tport);
    void                setMiddleware        (const char* middleware);
    void                setProcessEntries    (bool        process);  
    MamdaSubscription*  getMamdaSubscription (const char* symbol);
    mamaBridge          getBridge            ();
    SubscriptionHandler* getCb               ();
    MamaMsg&            getPublishMsg        ();
    MamdaOrderBook*     getBook              ();

private:
    MamaDQPublisherManager*     mPubManager;
    SubscriptionHandler*        managerCallback;
    SubscriptionList            mSubscriptionList;
    vector<const char*>         mSymbolList;
    MamdaOrderBook*             mBook;
    MamaDateTime                mBookTime;
    MamaMsg                     mPublishMsg;
    MamaDQPublisher*            mPublisher; 
    MamaTimer*                  mTimer;
    mamaBridge                  mBridge;
    const char*                 mMiddleware;
    const char*                 mPubTport;
    const char*                 mSubTport;
    MamaTransport*              mSubTransport;
    MamaTransport*              mPubTransport;
    const char*                 mPubSourceName;
    const char*                 mSubSourceName;
    MamaSource*                 mPubSource;
    MamaSource*                 mSubSource;
    bool                        mProcessEntries;
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
    TransportCallback (const TransportCallback& copy);
    TransportCallback& operator= (const TransportCallback& rhs);
};

   
class SubscriptionHandler : public MamaDQPublisherManagerCallback
{
public:
    SubscriptionHandler (BookPublisher* bookPublisher) {mBookPublisher = bookPublisher;}
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
    BookPublisher*  mBookPublisher;
};

int main (int argc, const char **argv)
{
    try
    {
        BookPublisher* mBookPublisher = new BookPublisher;
        CommonCommandLineParser cmdLine (argc, argv);
        
        mBookPublisher->setSymbolList       (cmdLine.getSymbolList());
        mBookPublisher->setSubSourceName    (cmdLine.getOptString ("SS"));
        mBookPublisher->setPubSourceName    (cmdLine.getOptString ("SP"));
        mBookPublisher->setSubTport         (cmdLine.getOptString ("tports"));
        mBookPublisher->setPubTport         (cmdLine.getOptString ("tportp"));
        mBookPublisher->setMiddleware       (cmdLine.getOptString ("m"));
        mBookPublisher->setProcessEntries   (cmdLine.getOptBool   ('e'));  
    
        mBookPublisher->initializeMama();
        mBookPublisher->createSources();

        // Get and initialize the dictionary
        DictRequester  dictRequester        (mBookPublisher->getBridge());
        dictRequester.requestDictionary     (cmdLine.getDictSource());
        MamdaCommonFields::setDictionary    (*dictRequester.getDictionary());
        MamdaOrderBookFields::setDictionary (*dictRequester.getDictionary ());
        
        mBookPublisher->createPublisher();
    
        mBookPublisher->getPublishMsg().create();
    
        mBookPublisher->subscribeToSymbols();          
      
        //create publisher and also set up MamaTimer to process order and publish changes
        mBookPublisher->createTimer();
       
        //set up new Book and enable order book publishing              
        mBookPublisher->start();
    }
    catch (MamaStatus &e)
    {  
        // This exception can be thrown from Mama.open ()
        // Mama::createTransport (transportName) and from
        // MamdaSubscription constructor when entitlements is enabled.
        cerr << "MamaStatus exception in main (): " << e.toString () << endl;
        exit (1);
    }
    catch (std::exception &ex)
    {
        cerr << "Exception in main (): " << ex.what () << endl;
        exit (1);
    }
    catch (...)
    {
        cerr << "Unknown Exception in main ()." << endl;
        exit (1);
    }

    exit (1);    
}

void usage (int exitStatus)
{
    std::cerr << "Usage: bookpublisher [-SP publisher source]  [-SS subscribe source]\n " 
              << "[-DT dict tport] -s symbol [-s symbol ...] \n"
              << "[-tportp publish transport] [-tports subscribe transport] [-m middleware]\n";
    exit (exitStatus);
}


void SubscriptionHandler::onCreate (MamaDQPublisherManager*  publisher)
{
    printf ("Created publisher subscription.\n");
    flush (cout);
}

void SubscriptionHandler::onError (
    MamaDQPublisherManager*  publisher,
    const MamaStatus&        status,
    const char*              errortxt,
    MamaMsg*                 msg) 
{
    if (msg) 
        printf ("Unhandled Msg: %s (%s) %s\n", 
            status.toString (), msg->toString (), errortxt);
    else
        printf ("Unhandled Msg: %s %s\n", 
            status.toString (), errortxt);
    flush (cout);
}


void SubscriptionHandler::onNewRequest (
    MamaDQPublisherManager*  publisherManager,
    const char*              symbol,
    short                    subType,
    short                    msgType,
    MamaMsg&                 msg)
{
    MamaMsg*     request = NULL;
    
    MamdaSubscription* mamdaSub = mBookPublisher->getMamdaSubscription(symbol);
    
    if (mamdaSub)
    {
        MamaSubscription*  mSub = mamdaSub->getMamaSubscription();
        MamaDQPublisher*   mPub = publisherManager->createPublisher (symbol, mBookPublisher->getCb());   
        
        printf ("Received New request: %s\n",  symbol);
        
        switch (msgType)
        {
            case MAMA_SUBSC_SUBSCRIBE:
            case MAMA_SUBSC_SNAPSHOT:
                request = msg.detach();
                mBookPublisher->publishMessage (request, symbol);
                break;
            
            default:
                mBookPublisher->publishMessage (NULL, symbol);
                break;
        }
    }
    else
        printf ("Received request for unknown symbol: %s\n",  symbol);
        
    flush (cout);
}
  
void SubscriptionHandler::onRequest (
    MamaDQPublisherManager*  publisherManager,
    const MamaPublishTopic&  publishTopicInfo,
    short                    subType,
    short                    msgType,
    MamaMsg&                 msg)
{  
    MamaMsg* request = NULL;
    printf ("Received request: %s\n",  publishTopicInfo.mSymbol);
    const char* symbol = publishTopicInfo.mSymbol;
    
    mBookPublisher->acquireLock(symbol);
    switch (msgType)
    {      
        case MAMA_SUBSC_SUBSCRIBE:
        case MAMA_SUBSC_SNAPSHOT:
        {
            request = msg.detach();  
            mBookPublisher->getPublishMsg().clear();
            mBookPublisher->getBook()->populateRecap (mBookPublisher->getPublishMsg());
            mBookPublisher->publishMessage (request, symbol);
            break;
        }
        case MAMA_SUBSC_DQ_SUBSCRIBER :
        case MAMA_SUBSC_DQ_PUBLISHER:
        case MAMA_SUBSC_DQ_NETWORK:
        case MAMA_SUBSC_DQ_UNKNOWN:
        case MAMA_SUBSC_DQ_GROUP_SUBSCRIBER:
        {
            mBookPublisher->getPublishMsg().clear();
            mBookPublisher->getBook()->populateRecap (mBookPublisher->getPublishMsg());
            mBookPublisher->publishMessage (NULL, symbol);
            break;
        }
        case MAMA_SUBSC_UNSUBSCRIBE:
        case MAMA_SUBSC_RESUBSCRIBE:
        case MAMA_SUBSC_REFRESH:
        default:
            break;
    }
    mBookPublisher->releaseLock (symbol);
    flush (cout);
}

void SubscriptionHandler::onRefresh (
    MamaDQPublisherManager*  publisherManager,
    const MamaPublishTopic&  publishTopicInfo,
    short                    subType,
    short                    msgType,
    MamaMsg&                 msg)
{  
    printf ("Received Refresh: %s\n",  publishTopicInfo.mSymbol);
    flush (cout);
} 

void BookPublisher::acquireLock (const char* symbol)
{
    // acquire lock on listener
    MamdaOrderBookListener* bookListener = NULL;
    MamdaSubscription* mamdaSub = getMamdaSubscription (symbol);

    try
    {
        vector<MamdaMsgListener*>& msgListeners = mamdaSub->getMsgListeners();
        unsigned long size = msgListeners.size();
        for (unsigned long i = 0; i < size; i++)
        {
            MamdaMsgListener* listener = msgListeners[i];
            bookListener = (MamdaOrderBookListener*)listener;
        }  
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Exception: Unknown" << std::endl;
    }
    
    bookListener->acquireWriteLock();
    printf ("\n Listener book lock acquired \n");
}

void BookPublisher::releaseLock(const char* symbol)
{
    //acquirelock on listener
    MamdaOrderBookListener* bookListener = NULL;
    MamdaSubscription* mamdaSub = getMamdaSubscription(symbol);

    try
    {
        vector<MamdaMsgListener*>& msgListeners = mamdaSub->getMsgListeners();
        unsigned long size = msgListeners.size();
        for (unsigned long i = 0; i < size; i++)
        {
            MamdaMsgListener* listener = msgListeners[i];
            bookListener = (MamdaOrderBookListener*)listener;
        }  
    }
    catch (std::exception &e)
    {
      std::cerr << "Exception: " << e.what() << std::endl;
    }
    catch (...)
    {
      std::cerr << "Exception: Unknown" << std::endl;
    }
    
    bookListener->releaseWriteLock();
    printf("\n Listener book lock released \n");
}

void BookPublisher::onTimer (MamaTimer* timer)
{  
    // need to clear publishMsg after every update
    const char* symbol;
    for (vector<const char*>::const_iterator i = mSymbolList.begin ();
         i != mSymbolList.end ();
         ++i)
    {
        symbol = (const char*)*i;
        acquireLock (symbol);
        mPublishMsg.clear();
        if (mBook->populateDelta (mPublishMsg))
        {
            publishMessage (NULL, symbol);
            printf ("\n Publishing Message:- %s \n", mPublishMsg.toString());

        }       
        else printf ("\n Not publishing from Empty State \n");
        releaseLock (symbol);
    }

}

void BookPublisher::subscribeToSymbols () 
{ 

    for (vector<const char*>::const_iterator i = mSymbolList.begin ();
         i != mSymbolList.end ();
         ++i)
    {   
        const char* symbol = *i;
        mBook = new MamdaOrderBook();
        // Turn on delta generation
        mBook->generateDeltaMsgs(true);
        MamdaSubscription*      aSubscription = new MamdaSubscription;
        MamdaOrderBookListener* aBookListener = new MamdaOrderBookListener(mBook);
        aSubscription->addMsgListener (aBookListener);
        aBookListener->setProcessEntries (mProcessEntries);

        BookTicker* aTicker = new BookTicker;
        aBookListener->addHandler (aTicker);
       
        aSubscription->setType (MAMA_SUBSC_TYPE_BOOK);
        aSubscription->setMdDataType (MAMA_MD_DATA_TYPE_ORDER_BOOK);
        mSubscriptionList.push_back (aSubscription);

        aSubscription->create (Mama::getDefaultEventQueue (mBridge), mSubSource, symbol, NULL);

    }
}

void BookPublisher::publishMessage (MamaMsg* request, const char* symbol)
{
    if (!mPublisher)
    {    
        MamdaSubscription* mamdaSub = getMamdaSubscription(symbol);
        
        if (mamdaSub)
        {
            MamaSubscription* mSub = mamdaSub->getMamaSubscription();
            mPublisher = mPubManager->createPublisher (symbol, managerCallback);   
        }
    }
    
    try
    {
        if (request)
        {
            mPublisher->sendReply (*request, &mPublishMsg);
        }
        else
        {
            mPublisher->send (&mPublishMsg);
        }

    }
    catch (MamaStatus &status)
    {
        cerr << "Error publishing message: "
             << status.toString () << endl;
        exit (1);
    }
}

MamdaSubscription* BookPublisher::getMamdaSubscription (const char * symbol) 
{
    SubscriptionList::const_iterator i;
    
    for (i = mSubscriptionList.begin (); i != mSubscriptionList.end (); i++)
    {
        if  (strcmp(((MamdaSubscription*)*i)->getSymbol() , symbol)==0)
        {
            return (MamdaSubscription*)*i;
        }
    }
    return NULL; 
}   

void BookPublisher::createTimer()
{
    // Create timer for editing book and publishing changes every 5s
    mTimer = new MamaTimer();
    mTimer->create (Mama::getDefaultEventQueue(mBridge), this, 2.0);
}

void BookPublisher::initializeMama ()
{
    if (!mMiddleware) mMiddleware = "wmw";
    mBridge = Mama::loadBridge (mMiddleware);
    
    Mama::open ();
        
    mSubTransport = new MamaTransport;
    mSubTransport->create (mSubTport, mBridge);
    mSubTransport->setTransportCallback (new TransportCallback ());
        
    mPubTransport = new MamaTransport;
    mPubTransport->create (mPubTport, mBridge);
    mPubTransport->setTransportCallback (new TransportCallback ());    
}

void BookPublisher::createPublisher ()
{
    managerCallback =  new SubscriptionHandler(this);
    srand ( time(NULL) );
    
    mPubManager = new MamaDQPublisherManager();
  
    mPubManager->create (mPubTransport, 
                         Mama::getDefaultEventQueue(mBridge),
                         managerCallback,
                         mPubSourceName);
}
      
void BookPublisher::createSources()
{                                
    mSubSource = new MamaSource ("default", 
                                 mSubTport, 
                                 mSubSourceName,
                                 mBridge);
                                  
    mSubSource->getTransport()->setOutboundThrottle (
                                    500, 
                                    MAMA_THROTTLE_DEFAULT);                                      
}

void BookPublisher::setSymbolList(const vector<const char*>& list)
{
    mSymbolList = list;
}

void BookPublisher::setSubSourceName(const char* source)
{
    mSubSourceName = source;
}

void BookPublisher::setPubSourceName(const char* source)
{
    mPubSourceName = source;
}

void BookPublisher::setSubTport(const char* tport)
{
    mSubTport = tport;
}

void BookPublisher::setPubTport(const char* tport)
{
    mPubTport = tport;
}

void BookPublisher::setMiddleware(const char* middleware)
{
    mMiddleware = middleware;
}
 
void BookPublisher::setProcessEntries(bool process)
{
    mProcessEntries = process;
}
    
mamaBridge BookPublisher::getBridge()
{
    return mBridge;
}
 
MamdaOrderBook* BookPublisher::getBook()
{   
    return mBook;
}

MamaMsg& BookPublisher::getPublishMsg()
{   
    return mPublishMsg;
}

SubscriptionHandler* BookPublisher::getCb()
{
    return managerCallback;
}
void BookPublisher::start ()
{
    Mama::start(mBridge);
}

BookPublisher::BookPublisher()
    : mPubManager    (NULL)
    , mPubTransport  (NULL)
    , mBook          (NULL)
    , mBridge        (NULL)
    , mMiddleware    ("wmw")
    , mPubTport      (NULL)
    , mSubTport      (NULL)
    , mPubSourceName (NULL)
    , mSubSourceName (NULL)
    , mPubSource     (NULL)
    , mSubSource     (NULL)
{
}
    
    
