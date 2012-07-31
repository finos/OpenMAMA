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

#include <mamda/MamdaSubscription.h>
#include <mamda/MamdaCommonFields.h>
#include <mamda/MamdaTradeFields.h>
#include <mamda/MamdaQuoteFields.h>
#include <mamda/MamdaLock.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "parsecmd.h"
#include "dictrequester.h"


#include <list>
using std::list;

using namespace Wombat;

typedef vector<const char*>      SymbolList;

class MamdaPublisher : public MamaTimerCallback, MamaDQPublisherManagerCallback
{
public:
    MamdaPublisher (); 
    ~MamdaPublisher () {};
       
    void                onTimer                  (MamaTimer* timer);
    void                createMsg                (const char* sym, 
                                                  const char* partId);
    void                createPublisherTransport (mamaBridge bridge,
                                                  const char* pubTport);
    void                createPublisherManager   (const char* pubSource, 
                                                  mamaBridge bridge);
    void                createTimer              (const char* symbol,
                                                  mamaBridge bridge);
    void                publishMessage           (MamaMsg*  request);
    MamaMsg&            getPublishMsg            ();
    void                setQueueGroup            (MamaQueueGroup* queues);
    MamaQueueGroup*     getQueueGroup            ();
    void                onCreate                 (MamaDQPublisherManager*  publisher);
    bool                publishingSymbol         (const char * symbol);
    
    void                setRecapFields      (MamaMsg& msg );
    void                setUpdateFields     (MamaMsg& msg);

    void                addMamaHeaderFields (MamaMsg&         msg,
                                             short            msgType,
                                             mamaMdMsgType    appMsgType,
                                             short            msgStatus);
    
    void onNewRequest (MamaDQPublisherManager* publisherManager,
                       const char*             symbol,
                       short                   subType,
                       short                   msgType,
                       MamaMsg&                msg);
      
        
    void onRequest  (MamaDQPublisherManager* publisherManager,
                     const MamaPublishTopic& publishTopicInfo,
                     short                   subType,
                     short                   msgType,
                     MamaMsg&                msg);
    
    void onRefresh (MamaDQPublisherManager* publisherManager,
                    const MamaPublishTopic& publishTopicInfo,
                    short                   subType,
                    short                   msgType,
                    MamaMsg&                msg) { } 
        
     void onError (MamaDQPublisherManager* publisher,
                   const MamaStatus&       status,
                   const char*             errortxt,
                   MamaMsg*                msg);


    void                acquireLock              ();
    void                releaseLock              ();                   

private:
    MamaDQPublisherManager*     mPublisherManager;
    MamaTransport*              mPubTransport;
    MamaDateTime                mPublishTime;
    MamaMsg                     mPublishMsg;
    MamaDQPublisher*            mPublisher; 
    MamaTimer*                  mTimer;
    MamdaLock                   mLock;
    MamaQueueGroup*             mQueueGroup;
    SymbolList                  mSymbolList;
    bool                        mPublishing;
    const char*                 mSymbol;
    const char*                 mPartId;
    mama_seqnum_t               mSeqNum;
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
        MamdaPublisher* mMamdaPublisher = new MamdaPublisher;
        CommonCommandLineParser     cmdLine (argc, argv);

        // Initialise the MAMA API
        mamaBridge bridge = cmdLine.getBridge();
       
        Mama::open ();
        
        const char* symbol      = cmdLine.getOptString("s");
        const char* partId      = cmdLine.getOptString("p");
        const char* pubSource   = cmdLine.getOptString("SP");
        const char* pubTport    = cmdLine.getOptString("tportp");
        MamaSource* source      = cmdLine.getSource();   
        
        MamaQueueGroup   queues (cmdLine.getNumThreads(), bridge);
        mMamdaPublisher->setQueueGroup(&queues);;
        
        mMamdaPublisher->createPublisherTransport(bridge, pubTport);
        mMamdaPublisher->createPublisherManager(pubSource, bridge);

        // Get and initialize the dictionary
        DictRequester    dictRequester   (bridge);
        dictRequester.requestDictionary  (cmdLine.getDictSource());
        MamdaCommonFields::setDictionary (*dictRequester.getDictionary());
        MamdaTradeFields::setDictionary  (*dictRequester.getDictionary ());
        MamdaQuoteFields::setDictionary  (*dictRequester.getDictionary ());

        //create publisher and also set up MamaTimer to process order and publish changes
        mMamdaPublisher->createTimer(symbol, bridge);
       
        mMamdaPublisher->createMsg(symbol, partId);
               
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

void MamdaPublisher::onTimer (MamaTimer* timer)
{  
    if (!mPublishing) return;
    
    mPublishTime.setToNow();
    
    acquireLock();
    addMamaHeaderFields (mPublishMsg,
                         MAMA_MSG_TYPE_QUOTE,
                         MAMA_MD_MSG_TYPE_QUOTE,
                         MAMA_MSG_STATUS_OK);

    setUpdateFields (mPublishMsg);

    publishMessage(NULL);
    releaseLock();
}


void MamdaPublisher::createMsg (const char* symbol, const char* partId) 
{ 
    mSymbol = symbol;
    mPartId = partId;
    mPublishTime.setToNow();
    addMamaHeaderFields (mPublishMsg,
                         MAMA_MSG_TYPE_INITIAL,
                         MAMA_MD_MSG_TYPE_GENERAL,
                         MAMA_MSG_STATUS_OK);

    setRecapFields (mPublishMsg);
    
    mSymbolList.push_back (symbol);
}

void MamdaPublisher::publishMessage (MamaMsg*  request)
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

void MamdaPublisher::onCreate (MamaDQPublisherManager*  publisher)
{
    if (gExampleLogLevel > EXAMPLE_LOG_LEVEL_NORMAL)
    {
        cout << "Created publisher subscription.\n";
    }
}

void MamdaPublisher::onError (
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

void MamdaPublisher::onNewRequest (
    MamaDQPublisherManager* publisherManager,
    const char*             symbol,
    short                   subType,
    short                   msgType,
    MamaMsg&                msg)
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
                publishMessage(&msg);
                releaseLock();          
                break;           
            default:
                acquireLock();
                publishMessage(&msg);
                releaseLock();          
                break;
        }
    }
    else
        if (gExampleLogLevel > EXAMPLE_LOG_LEVEL_NORMAL)
            cout << "Received request for unknown symbol: " <<  symbol << endl;
}
void MamdaPublisher::onRequest (
    MamaDQPublisherManager*  publisherManager,
    const MamaPublishTopic&  publishTopicInfo,
    short                    subType,
    short                    msgType,
    MamaMsg&                 msg)
{  
    MamaMsg*     request = NULL;
    const char* symbol = publishTopicInfo.mSymbol;
    bool publish = false;
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
            acquireLock();
            publishMessage(&msg);
            releaseLock();
            break;
            
        case MAMA_SUBSC_DQ_SUBSCRIBER :
        case MAMA_SUBSC_DQ_PUBLISHER:
        case MAMA_SUBSC_DQ_NETWORK:
        case MAMA_SUBSC_DQ_UNKNOWN:
        case MAMA_SUBSC_DQ_GROUP_SUBSCRIBER:
            acquireLock();
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

void MamdaPublisher::createPublisherTransport (mamaBridge bridge, const char* pubTport)
{
    mPubTransport = new MamaTransport;
    mPubTransport->create (pubTport, bridge);
    mPubTransport->setTransportCallback (new TransportCallback ());
}        

void MamdaPublisher::createPublisherManager (const char* pubSource, mamaBridge bridge)
{
    mPublisherManager = new MamaDQPublisherManager();

    //create pub
    mPublisherManager->create (mPubTransport, 
                               mQueueGroup->getNextQueue(),
                               this,
                               pubSource);
}
 
void MamdaPublisher::createTimer (const char* symbol, mamaBridge bridge)
{
    //create timer, on next queue, for publishing trades every 1s
    mTimer = new MamaTimer();
    mTimer->create (mQueueGroup->getNextQueue(), this, 1);
}

MamaMsg& MamdaPublisher::getPublishMsg()
{   
    return mPublishMsg;
}

void MamdaPublisher::acquireLock ()
{ 
    ACQUIRE_WLOCK(mLock); 
}

void MamdaPublisher::releaseLock ()
{ 
    RELEASE_WLOCK(mLock); 
}    

void MamdaPublisher::setQueueGroup (MamaQueueGroup* queues)
{
    mQueueGroup = queues;
}

bool MamdaPublisher::publishingSymbol (const char * symbol) 
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

MamaQueueGroup* MamdaPublisher::getQueueGroup()
{
    return mQueueGroup;
}

//This function adds the Mama Header fields setting the msgType, msgStatus, seqNum (updating every tick) and the senderID. 
void 
MamdaPublisher::addMamaHeaderFields (MamaMsg&         msg,
                                     short            msgType,
                                     mamaMdMsgType    appMsgType,
                                     short            msgStatus)
{  
    msg.addU8  (NULL, 1, msgType);
    msg.addU8  (NULL, 2, msgStatus);
    msg.addU32 (NULL, 10, mSeqNum);
    msg.addU32 (NULL, 17, appMsgType);
    msg.addU64 (NULL, 20, 1);
    msg.addU64 ("MamaSenderId", 20, 6847160); 
    mSeqNum++;
}


void MamdaPublisher::setRecapFields(MamaMsg& msg)
{     
    msg.addDateTime ("wLineTime", 1174, mPublishTime);
    msg.addDateTime ("wSrcTime", 465, mPublishTime);  
    msg.addString   ("wIssueSymbol", 305, mSymbol);
    
    msg.addI32      ("wQuoteCount",1034, mSeqNum);
    msg.addI32      ("wTradeSeqNum",483,mSeqNum);
    msg.addString   ("wTradeId",477,"A");
    msg.addString   ("wOrderRef",2209,"B");
    msg.addI32      ("wOrderId",408,mSeqNum);
    msg.addI64      ("wTotalValue",474, 1000);
    msg.addI64      ("wVwap",494, 100);

    msg.addPrice    ("wBidPrice",237, 143.950000);
    msg.addI64      ("wBidSize",238, 736);
    msg.addF64      ("wAskPrice",109, 143.960000);
    msg.addI64      ("wAskSize",110,331);
    msg.addString   ("wQuoteQualifier",440,"normal");
    msg.addString   ("wAskPartId",108,"P");
    msg.addDateTime ("wQuoteTime",442,mPublishTime);
    msg.addI32      ("wQuoteSeqNum",441,18);
    msg.addI32      ("wQuoteCount",1034,469136);
    msg.addString   ("wCondition",243, "R");      
    msg.addU16      ("wMsgQual",21,1);    

}

void MamdaPublisher::setUpdateFields(MamaMsg& msg)
{      
    msg.addDateTime ("wLineTime", 1174, mPublishTime);
    msg.addDateTime ("wSrcTime", 465, mPublishTime);  
    msg.addString   ("wIssueSymbol", 305, mSymbol);
    msg.addI32      ("wQuoteCount",1034, mSeqNum);

}


MamdaPublisher::MamdaPublisher()
    : mPublisherManager (NULL)
    , mPubTransport     (NULL)
    , mLock             (MamdaLock::SHARED, "mamdapublisher()")
    , mQueueGroup       (NULL)
    , mPublishing       (false)
    , mSeqNum           (0)
{
    mPublishMsg.create();
}

void usage (int exitStatus)
{
    std::cerr << "Usage: mamdapublisher [-SP publisher source] -s symbol [-s symbol ...]\n " 
              << "[-DT dict tport] [-p partId] [-threads no. of threads] \n";
    exit (exitStatus);
}



