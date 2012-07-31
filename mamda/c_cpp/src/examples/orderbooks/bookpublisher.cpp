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
#include <mamda/MamdaLock.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <list>
#include "../parsecmd.h"
#include "../dictrequester.h"

/*  *********************************************************************** */
/* This example program integrates advanced MAMA publishing and MAMDA order */ 
/* book publishing functionality to enable publishing of MamdaOrderBook     */ 
/* data to clients. The example application has two main components; the    */
/* publisher manager/publisher and  the order book publishing functionality.*/
/* The subscription handler, MamaDQPublisherManagerCallback, is implemented */
/* in the BookPublisher() class and is tied to an mama subscription and     */
/* handles the subscription level requests for the underlying symbol from   */
/* potential clients, refresh requests and has inherent error handling.     */

/* The publisher manager, MamaDQPublishManager, creates the subscription    */ 
/* used for listening to client requests on a source and acts as a store    */
/* for all the publishers publishing different symbols on that source. The  */
/* MamaDQPublisher, publisher, adds certain fiels to the message, such as   */
/* the message type, before publishing the message.                         */

/* In this application, a MamdaOrderBook object is created and during       */ 
/* runtime is populated with levels and/or entries, depending on the        */
/* configuration paremeters. Data for the example program is artifically    */
/* created as an array of book order data and this drives the example       */
/* program. Using a MamaTimer object orders are processed every second,     */
/* during the MamaTimer::onTimer() callback, and any changes published      */
/* using the publishing functionality.                                      */

/* Clients that connect to the bookpublisher application, subscribing to a  */ 
/* symbol that is being published, will firstly receive a book initial      */
/* message followed by the book updates every second. Recaps requests from  */
/* the clients are handled by the bookpublisher and clients will receive    */
/* book recap messages when required.                                       */

/* The  application also implements a locking mechanism that prevents       */
/* changes to the book when book initials/recaps are published to clients.  */
/* The MamdaLock() enables the use of multiple threads for publishing and   */
/* editing the book, passing in the commandline parameter                   */
/* "-threads {no. of threads}".                                             */

/* The example application also shows how prospective publisher should      */
/* handle book initial/recap requests. To maintain data integrity between   */
/* the publisher and across all clients, upon an initial/recap request,     */
/* the publisher should firstly publish any changes to the book that may    */
/* have been executed and stored, before then publishing the initial/recap. */
/* This way, all client books will match the publisher book.                */
/*  *********************************************************************** */

using std::list;

using namespace Wombat;

typedef vector<const char*>      SymbolList;
/*  ******************************************************************* */
/*  Functionality required for artificially created book orders used to */
/*  update the book object and generate deltas for publishing           */
/* ******************************************************************** */

typedef MamdaOrderBookPriceLevel::Side Side;
typedef MamdaOrderBookPriceLevel::Action PlAction;
typedef MamdaOrderBookEntry::Action EntAction;

Side bidSide = MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID;
Side askSide = MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_ASK;

const PlAction PLADD     = MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD;
const PlAction PLUPDATE  = MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_UPDATE;
const PlAction PLDELETE  = MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_DELETE;
const PlAction PLUNKNOWN = MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_UNKNOWN;

const EntAction ENTADD    = MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD;
const EntAction ENTUPDATE = MamdaOrderBookEntry::MAMDA_BOOK_ACTION_UPDATE;
const EntAction ENTDELETE = MamdaOrderBookEntry::MAMDA_BOOK_ACTION_DELETE;

//  The struct "order" represents all pl and entry info of a typical book order                                                          
typedef struct order
{
    Side        side;
    PlAction    plAction;
    double      price;
    double      volume;
    const char* entId;
    EntAction   entAction;
    double      entSize;
    double      sizeChange;
    int         numEntries;
};         

//Initialise an arry of these orders
order orderArray[] = 
{
    {bidSide, PLADD,    100, 1000, "bid1", ENTADD,    1000, 1000,  1},
    {bidSide, PLADD,    101, 2000, "bid1", ENTADD,    2000, 2000,  1},
    {bidSide, PLUPDATE, 100, 2000, "bid2", ENTADD,    1000, 1000,  2},                
    {askSide, PLADD,    110, 2000, "ask1", ENTADD,    2000, 2000,  1},    
    {askSide, PLADD,    109, 3000, "ask1", ENTADD,    3000, 3000,  1},
    {bidSide, PLUPDATE, 101, 4000, "bid2", ENTADD,    3000, 3000,  2},
    {bidSide, PLUPDATE, 101, 2000, "bid1", ENTDELETE, 0,    -2000, 1},    
    {askSide, PLADD,    104, 4000, "ask1", ENTADD,    4000, 4000,  1},
    {bidSide, PLUPDATE, 100, 1500, "bid3", ENTADD,    500,  500,   3},
    {askSide, PLDELETE, 110, 0,    "ask1", ENTDELETE, 0,    -2000, 0},     
};
/*  ******************************************************************* */

class BookPublisher : public MamaTimerCallback, MamaDQPublisherManagerCallback
{
public:
    BookPublisher (); 
    ~BookPublisher () {};
       
    void                onTimer                  (MamaTimer*   timer);
    void                createBook               (const char*  sym, 
                                                  const char*  partId);
    void                createPublisherTransport (mamaBridge   bridge);
    void                createPublisherManager   (const char*  pubSource, 
                                                  mamaBridge   bridge);
    void                createTimer              (const char*  symbol,
                                                  mamaBridge   bridge);
    void                createMessage            ();
    void                publishMessage           (MamaMsg*     request);
    void                processOrder             ();
    MamaMsg&            getPublishMsg            ();
    MamdaOrderBook*     getBook                  ();
    void                setProcessEntries        (bool         process);
    void                setQueueGroup            (MamaQueueGroup* queues);
    MamaQueueGroup*     getQueueGroup            ();
    void                onCreate                 (MamaDQPublisherManager*  publisher);
    bool                publishingSymbol         (const char*  symbol);

    void onNewRequest (MamaDQPublisherManager* publisherManager,
                       const char*             symbol,
                       short                   subType,
                       short                   msgType,
                       MamaMsg&                msg);
      
        
    void onRequest  (MamaDQPublisherManager*  publisherManager,
                     const MamaPublishTopic&  publishTopicInfo,
                     short                    subType,
                     short                    msgType,
                     MamaMsg&                 msg);
    
    void onRefresh (MamaDQPublisherManager*   publisherManager,
                    const MamaPublishTopic&   publishTopicInfo,
                    short                     subType,
                    short                     msgType,
                    MamaMsg&                  msg) { } 
        
     void onError  (MamaDQPublisherManager*   publisher,
                    const MamaStatus&         status,
                    const char*               errortxt,
                    MamaMsg*                  msg);


    void            acquireLock              ();
    void            releaseLock              ();                   

private:
    MamaDQPublisherManager*     mPublisherManager;
    MamaTransport*              mPubTransport;
    MamdaOrderBook*             mBook;
    MamaDateTime                mBookTime;
    MamaMsg                     mPublishMsg;
    MamaDQPublisher*            mPublisher; 
    MamaTimer*                  mTimer;
    bool                        mProcessEntries;
    MamdaLock                   mBookLock;
    MamaQueueGroup*             mQueueGroup;
    int                         mOrderCount;
    SymbolList                  mSymbolList;
    bool                        mPublishing;
};


class TransportCallback : public MamaTransportCallback
{
public:
    TransportCallback          () {}
    virtual ~TransportCallback () {} 

    virtual void onDisconnect (MamaTransport *tport) 
    {
        if (gExampleLogLevel > EXAMPLE_LOG_LEVEL_NORMAL)
        cout << "TRANSPORT DISCONNECTED\n";
    }

    virtual void onReconnect (MamaTransport *tport)
    {
        if (gExampleLogLevel > EXAMPLE_LOG_LEVEL_NORMAL)
        {
            cout << "TRANSPORT RECONNECTED\n";
        }
    }

    virtual void onQuality (MamaTransport* tport,
                            short          cause,
                            const void*    platformInfo)
    {
        if (gExampleLogLevel > EXAMPLE_LOG_LEVEL_NORMAL)
        {
            mamaQuality quality = tport->getQuality ();
            cout << "QUALITY - " << mamaQuality_convertToString (quality);
            cout << "\n";
        }
    }

private:
    TransportCallback   (const TransportCallback& copy);
    TransportCallback&  operator= (const TransportCallback& rhs);
};

int main (int argc, const char **argv)
{
    try
    {
        BookPublisher* mBookPublisher = new BookPublisher;
        CommonCommandLineParser     cmdLine (argc, argv);
          
        // Initialise the MAMA API
        mamaBridge bridge = cmdLine.getBridge();
       
        Mama::open ();
        
        const char* symbol      = cmdLine.getOptString("s");          
        const char* partId      = cmdLine.getOptString("p");          
        const char* pubSource   = cmdLine.getOptString("SP");
        MamaSource* source      = cmdLine.getSource();   
        
        MamaQueueGroup   queues (cmdLine.getNumThreads(), bridge);
        mBookPublisher->setQueueGroup (&queues);;
        
        mBookPublisher->setProcessEntries (cmdLine.getOptBool ('e'));
        mBookPublisher->createPublisherTransport (bridge);
        mBookPublisher->createPublisherManager (pubSource, bridge);

        // Get and initialize the dictionary
        DictRequester    dictRequester      (bridge);
        dictRequester.requestDictionary     (cmdLine.getDictSource());
        MamdaCommonFields::setDictionary    (*dictRequester.getDictionary());
        MamdaOrderBookFields::setDictionary (*dictRequester.getDictionary());
                     
        //create publisher and also set up MamaTimer to process order and publish changes
        mBookPublisher->createTimer   (symbol, bridge);
        mBookPublisher->createMessage ();
       
        //set up new Book and enable order book publishing
        mBookPublisher->createBook (symbol, partId);          
               
        Mama::start (bridge);
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

/* ******************************************************************************** */
/* Book publishing functionality. This section contains necessary functionality to  */
/* edit a book and publish the deltaMsg() generated                                 */
/* ******************************************************************************** */

void BookPublisher::onTimer (MamaTimer* timer)
{  
    if (!mPublishing) return;
    //  On every timer update we take the next order from the orderArray and processEntries

    // clear book when at end of orderArray
    // To avoid editing the book while publishing using multiple threads, we need to have a 
    // lock during the book editing functionality and also when sending initial/recap data.
    // We use the MamdaLock class for this functionality and have two functions called
    // acquireLock() and releaseLock() that implement this functionality. 
    acquireLock();
    bool publish = false;
    if (10 == mOrderCount)
    {
        mBook->clear(true);
        mOrderCount = 0;
        mPublishMsg.updateU8 (NULL, MamaFieldMsgType.mFid, MAMA_MSG_TYPE_BOOK_CLEAR);
        publish = true;
    }
    else
    { 
        // process multiple orders, alike a complex update
        processOrder();
        processOrder();
        // get changes to the book and publish
        publish = mBook->populateDelta(mPublishMsg);
    }
    if (publish) publishMessage(NULL);    
    releaseLock();
}

/* ******************************************************************************** */
/* processOrder() processes the next order in the orderArray,then loops back to 1st */
/* when all order are processed, clearing the book at this point. This generates    */
/* changes to the book for publishing. Any real world application would replace     */ 
/* this functionality with it's own appropriate book handling functionality         */
/* ******************************************************************************** */

void BookPublisher::processOrder () 
{   
    MamdaOrderBookPriceLevel* level = NULL;
    MamdaOrderBookEntry*      entry = NULL;
    order thisOrder = orderArray[mOrderCount];
    mBookTime.setToNow();

    if (mProcessEntries)
    {          
        switch (thisOrder.entAction)
        {
            case ENTDELETE:
            {
                level = mBook->getLevelAtPrice (thisOrder.price, thisOrder.side);
                
                if (level)
                    entry = level->findEntry (thisOrder.entId);
                if (entry)
                    mBook->deleteEntry (entry, mBookTime, NULL);
                break;
            }
            case ENTADD:
            {
                mBook->addEntry (thisOrder.entId, thisOrder.entSize,
                                 thisOrder.price, thisOrder.side,
                                 mBookTime, NULL, NULL);
                break;
            }
            case ENTUPDATE:
            {
                entry = level->findEntry (thisOrder.entId);
                mBook->updateEntry (entry, thisOrder.entSize,
                                    mBookTime, NULL);
                break;
            }
        }
    }
    else
    {
        level = mBook->getLevelAtPrice(thisOrder.price, thisOrder.side);
        if (level)
        {
            level->setSizeChange (thisOrder.sizeChange);
            level->setPrice      (thisOrder.price);
            level->setSize       (thisOrder.volume);
            level->setNumEntries (thisOrder.numEntries);           
            level->setTime       (mBookTime);
            level->setAction     (thisOrder.plAction);
        }
        else
        {
            level = new MamdaOrderBookPriceLevel();
            level->setSide       (thisOrder.side);
            level->setSizeChange (thisOrder.sizeChange);
            level->setPrice      (thisOrder.price);
            level->setSize       (thisOrder.volume);
            level->setNumEntries (thisOrder.numEntries);           
            level->setTime       (mBookTime);
            level->setAction     (thisOrder.plAction);                
        }
        
        switch (thisOrder.plAction)
        {
            case PLDELETE:
                mBook->deleteLevel(*level);
                break;
            case PLADD:
                mBook->addLevel(*level);
                break;
            case PLUPDATE:
                mBook->updateLevel(*level);
                break;
        }
    }
    mOrderCount++;
}

void BookPublisher::createBook (const char* symbol, const char* partId) 
{ 
    mBook = new MamdaOrderBook();
    // This turns on the generation of deltas at the order book 
    mBook->generateDeltaMsgs (true);
    if (symbol) mBook->setSymbol (symbol); 
    if (partId) mBook->setPartId (partId); 
    mBookTime.setToNow();
    mBook->setBookTime (mBookTime);
    mSymbolList.push_back (symbol);
}

void BookPublisher::publishMessage (MamaMsg*  request)
{
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
        //clear publish message for later use
        mPublishMsg.clear();
    }
    catch (MamaStatus &status)
    {
        cerr << "Error publishing message: "
             << status.toString () << endl;
        exit (1);
    }
}

/* ******************************************************************************** */

void BookPublisher::onCreate (MamaDQPublisherManager*  publisher)
{
    if (gExampleLogLevel > EXAMPLE_LOG_LEVEL_NORMAL)
    {
        cout << "Created publisher subscription.\n";
    }
}

void BookPublisher::onError (
    MamaDQPublisherManager*  publisher,
    const MamaStatus&        status,
    const char*              errortxt,
    MamaMsg*                 msg) 
{
    if (gExampleLogLevel > EXAMPLE_LOG_LEVEL_NORMAL)
    {
        if (msg) 
        {
            cout << "Unhandled Msg:"
                 <<  status.toString ()
                 << msg->toString ()
                 << errortxt
                 << "\n";
        }
        else
        {
            cout << "Unhandled Msg: "
                 << status.toString ()
                 << errortxt
                 << "\n";
        }
    }
}

void BookPublisher::onNewRequest (
    MamaDQPublisherManager*  publisherManager,
    const char*              symbol,
    short                    subType,
    short                    msgType,
    MamaMsg&                 msg)
{

    MamaMsg*     request = NULL;
    
    if (publishingSymbol(symbol))  
    {
        mPublisher = mPublisherManager->createPublisher(symbol, this);
        mPublishing = true;
        if (gExampleLogLevel > EXAMPLE_LOG_LEVEL_NORMAL)
        {        
            cout << "Received New request: " <<  symbol << endl;
        }
        bool publish = false;
        switch (msgType)
        {
            case MAMA_SUBSC_SUBSCRIBE:
            case MAMA_SUBSC_SNAPSHOT:
                acquireLock();
                getBook()->populateRecap(getPublishMsg());
                publishMessage(&msg);
                releaseLock();          
                break;           
            default:
                acquireLock();
                getBook()->populateRecap(getPublishMsg());
                publishMessage(&msg);
                releaseLock();          
                break;
        }
    }
    else if (gExampleLogLevel > EXAMPLE_LOG_LEVEL_NORMAL)
    {        
        cout << "Received request for unknown symbol: " <<  symbol << endl;
    }
}

void BookPublisher::onRequest (
    MamaDQPublisherManager*  publisherManager,
    const MamaPublishTopic&  publishTopicInfo,
    short                    subType,
    short                    msgType,
    MamaMsg&                 msg)
{  
    MamaMsg*     request = NULL;
    const char*  symbol  = publishTopicInfo.mSymbol;
    bool         publish = false;
    if (gExampleLogLevel > EXAMPLE_LOG_LEVEL_NORMAL)
    {
        cout << "Received request: "
             << symbol
             << "\n";
    }
    
    switch (msgType)
    {      
        case MAMA_SUBSC_SUBSCRIBE:
        case MAMA_SUBSC_SNAPSHOT:
            // publish current delta list first 
            acquireLock();
            publish = getBook()->populateDelta(getPublishMsg());
            if (publish) publishMessage(NULL);
            publish = false;
                
            getBook()->populateRecap(getPublishMsg());
            publishMessage(&msg);
            releaseLock();          
            break;
            
        case MAMA_SUBSC_DQ_SUBSCRIBER :
        case MAMA_SUBSC_DQ_PUBLISHER:
        case MAMA_SUBSC_DQ_NETWORK:
        case MAMA_SUBSC_DQ_UNKNOWN:
        case MAMA_SUBSC_DQ_GROUP_SUBSCRIBER:
            // publish current delta list first
            acquireLock();
            publish = getBook()->populateDelta(getPublishMsg());
            if (publish) publishMessage(NULL);
            
            getBook()->populateRecap(getPublishMsg());
            publishMessage(NULL);
            releaseLock();
            break;
        case MAMA_SUBSC_UNSUBSCRIBE:
        case MAMA_SUBSC_RESUBSCRIBE:
        case MAMA_SUBSC_REFRESH:
        default:
            break;
    }
}

void BookPublisher::createPublisherTransport (mamaBridge bridge)
{
    mPubTransport = new MamaTransport;
    mPubTransport->create ("pub", bridge);
    mPubTransport->setTransportCallback (new TransportCallback ());
}        

void BookPublisher::createPublisherManager (const char* pubSource, mamaBridge bridge)
{
    mPublisherManager = new MamaDQPublisherManager();

    //create pub
    mPublisherManager->create (mPubTransport, 
                         mQueueGroup->getNextQueue(),
                         this,
                         pubSource);
}
 
void BookPublisher::createTimer (const char* symbol, mamaBridge bridge)
{
    //create timer, on next queue, for editing book and publishing changes every 1s
    mTimer = new MamaTimer();
    mTimer->create (mQueueGroup->getNextQueue(), this, 1);
}

void BookPublisher::createMessage ()
{
    mPublishMsg.create();
}

void BookPublisher::setProcessEntries (bool process)
{
    mProcessEntries = process;
}

MamdaOrderBook* BookPublisher::getBook()
{   
    return mBook;
}

MamaMsg& BookPublisher::getPublishMsg()
{   
    mPublishMsg.clear();
    return mPublishMsg;
}

void BookPublisher::acquireLock ()
{ 
    ACQUIRE_WLOCK(mBookLock); 
}

void BookPublisher::releaseLock ()
{ 
    RELEASE_WLOCK(mBookLock); 
}    

void BookPublisher::setQueueGroup (MamaQueueGroup* queues)
{
    mQueueGroup = queues;
}

bool BookPublisher::publishingSymbol (const char * symbol) 
{
    SymbolList::const_iterator i;
    
    for (i = mSymbolList.begin (); i != mSymbolList.end (); i++)
    {
        if  (strcmp(*i,symbol)==0)
        {
            return true;
        }
    }
    return false;
}

MamaQueueGroup* BookPublisher::getQueueGroup()
{
    return mQueueGroup;
}

BookPublisher::BookPublisher()
    : mPublisherManager (NULL)
    , mPubTransport     (NULL)
    , mBook             (NULL)
    , mBookLock         (MamdaLock::SHARED, "bookpublisher(book)")
    , mQueueGroup       (NULL)
    , mOrderCount       (0)
    , mPublishing       (false)
{
}
    
void usage (int exitStatus)
{
    std::cerr << "Usage: bookpublisher [-SP publisher source] -s symbol [-s symbol ...]\n " 
              << "[-DT dict tport] [-p partId] [-e] process entries [-threads num. of threads] \n";
    exit (exitStatus);
}
