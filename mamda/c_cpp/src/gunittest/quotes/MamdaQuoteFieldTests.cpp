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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <fstream>

#include <gtest/gtest.h>

#include <mamda/MamdaQuoteFields.h>
#include <mamda/MamdaCommonFields.h>
#include <mamda/MamdaSubscription.h>
#include <mamda/MamdaQuoteHandler.h>
#include <mamda/MamdaConfig.h>
#include <mamda/MamdaQuoteListener.h>
#include <mamda/MamdaDataException.h>
#include <mamda/MamdaFieldState.h>
#include <mamda/MamdaErrorListener.h>
#include <mamda/MamdaQualityListener.h>
#include <mama/MamaDictionary.h>
#include <mama/mamacpp.h>
#include <mama/MamaMsg.h>

#include "common/MamdaUnitTestUtils.h"
#include "common/CpuTestGenerator.h"
#include "common/MemoryTestGenerator.h"

using std::string;

using namespace std;
using namespace Wombat;

class QuoteTickerCB : public MamdaQuoteHandler                  
{
public:
    QuoteTickerCB () {}
    virtual ~QuoteTickerCB () {}

    void callMamdaOnMsg (MamdaSubscription* sub, MamaMsg& msg)
    {   
        try
        {   
           vector<MamdaMsgListener*>& msgListeners = sub->getMsgListeners();
           unsigned long size = msgListeners.size();
           for (unsigned long i = 0; i < size; i++)           
            {      
                MamdaMsgListener* listener = msgListeners[i];
                listener->onMsg (sub, msg,(short)msg.getType());
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

    void onQuoteRecap (
        MamdaSubscription*      subscription,
        MamdaQuoteListener&     listener,
        const MamaMsg&          msg,
        const MamdaQuoteRecap&  recap)
    {     
    }

    void onQuoteUpdate (
        MamdaSubscription*      subscription,
        MamdaQuoteListener&     listener,
        const MamaMsg&          msg,
        const MamdaQuoteUpdate& quote,
        const MamdaQuoteRecap&  recap)
    {   
        
        mQuoteTestCache.myTempSymbol            = quote.getSymbol();
        mQuoteTestCache.myTempSrcTime           = quote.getSrcTime().getAsString();
        mQuoteTestCache.myTempActTime           = quote.getActivityTime().getAsString();
        mQuoteTestCache.myTempLineTime          = quote.getLineTime().getAsString();
        mQuoteTestCache.myTempPubId             = listener.getPubId();
        mQuoteTestCache.myTempBidPrice          = quote.getBidPrice().getValue();
        mQuoteTestCache.myTempBidSize           = quote.getBidSize();
        mQuoteTestCache.myTempBidDepth          = quote.getBidDepth();
        mQuoteTestCache.myTempBidPartId         = quote.getBidPartId();
        mQuoteTestCache.myTempBidClosePrice     = listener.getBidClosePrice().getValue();
        mQuoteTestCache.myTempBidCloseDate      = listener.getBidCloseDate().getAsString();
        mQuoteTestCache.myTempBidPrevClosePrice = listener.getBidPrevClosePrice().getValue();
        mQuoteTestCache.myTempBidPrevCloseDate  = listener.getBidPrevCloseDate().getAsString();
        mQuoteTestCache.myTempBidHigh           = listener.getBidHigh().getValue();
        mQuoteTestCache.myTempBidLow            = listener.getBidLow().getValue();
        mQuoteTestCache.myTempAskPrice          = quote.getAskPrice().getValue();
        mQuoteTestCache.myTempAskSize           = quote.getAskSize();
        mQuoteTestCache.myTempAskDepth          = quote.getAskDepth();
        mQuoteTestCache.myTempAskPartId         = quote.getAskPartId();
        mQuoteTestCache.myTempAskClosePrice     = listener.getAskClosePrice().getValue();
        mQuoteTestCache.myTempAskCloseDate      = listener.getAskCloseDate().getAsString();
        mQuoteTestCache.myTempAskPrevClosePrice = listener.getAskPrevClosePrice().getValue();
        mQuoteTestCache.myTempAskPrevCloseDate  = listener.getAskPrevCloseDate().getAsString();
        mQuoteTestCache.myTempAskHigh           = listener.getAskHigh().getValue();
        mQuoteTestCache.myTempAskLow            = listener.getAskLow().getValue();
        mQuoteTestCache.myTempEventSeqNum       = quote.getEventSeqNum();
        mQuoteTestCache.myTempEventTime         = quote.getEventTime().getAsString();
        mQuoteTestCache.myTempQuoteDate         = quote.getQuoteDate().getAsString();
        mQuoteTestCache.myTempQuoteQualStr      = quote.getQuoteQualStr();
        mQuoteTestCache.myTempQuoteQualNative   = quote.getQuoteQualNative();
        mQuoteTestCache.myTempAskTime           = quote.getAskTime().getAsString();
        mQuoteTestCache.myTempBidTime           = quote.getBidTime().getAsString();
        mQuoteTestCache.myTempAskIndicator      = quote.getAskIndicator();
        mQuoteTestCache.myTempBidIndicator      = quote.getBidIndicator();
        mQuoteTestCache.myTempAskYield          = quote.getAskYield();
        mQuoteTestCache.myTempBidYield          = quote.getBidYield();
        mQuoteTestCache.myTempBidSizesList      = quote.getBidSizesList();
        mQuoteTestCache.myTempAskSizesList      = quote.getAskSizesList();
      //  myTempAskUpdateCount = quote.getAskUpdateCount();
      //  myTempBidUpdateCount = quote.getBidUpdateCount();
        mQuoteTestCache.myTempShortSaleBidTick  = quote.getShortSaleBidTick();
        
        
        mQuoteTestCache.myTempSymbolFieldState            = quote.getSymbolFieldState();
        mQuoteTestCache.myTempPartIdFieldState            = quote.getPartIdFieldState();
        mQuoteTestCache.myTempSrcTimeFieldState           = quote.getSrcTimeFieldState();
        mQuoteTestCache.myTempActTimeFieldState           = quote.getActivityTimeFieldState();
        mQuoteTestCache.myTempLineTimeFieldState          = quote.getLineTimeFieldState();
        mQuoteTestCache.myTempSendTimeFieldState          = quote.getSendTimeFieldState();
        mQuoteTestCache.myTempPubIdFieldState             = listener.getPubIdFieldState();
        mQuoteTestCache.myTempBidPriceFieldState          = quote.getBidPriceFieldState();
        mQuoteTestCache.myTempBidSizeFieldState           = quote.getBidSizeFieldState();
        mQuoteTestCache.myTempBidDepthFieldState          = quote.getBidDepthFieldState();
        mQuoteTestCache.myTempBidPartIdFieldState         = quote.getBidPartIdFieldState();
        mQuoteTestCache.myTempBidClosePriceFieldState     = listener.getBidClosePriceFieldState();
        mQuoteTestCache.myTempBidCloseDateFieldState      = listener.getBidCloseDateFieldState();
        mQuoteTestCache.myTempBidPrevClosePriceFieldState = listener.getBidPrevClosePriceFieldState();
        mQuoteTestCache.myTempBidPrevCloseDateFieldState  = listener.getBidPrevCloseDateFieldState();
        mQuoteTestCache.myTempBidHighFieldState           = listener.getBidHighFieldState();
        mQuoteTestCache.myTempBidLowFieldState            = listener.getBidLowFieldState();
        mQuoteTestCache.myTempAskPriceFieldState          = quote.getAskPriceFieldState();
        mQuoteTestCache.myTempAskSizeFieldState           = quote.getAskSizeFieldState();
        mQuoteTestCache.myTempAskDepthFieldState          = quote.getAskDepthFieldState();
        mQuoteTestCache.myTempAskPartIdFieldState         = quote.getAskPartIdFieldState();
        mQuoteTestCache.myTempAskClosePriceFieldState     = listener.getAskClosePriceFieldState();
        mQuoteTestCache.myTempAskCloseDateFieldState      = listener.getAskCloseDateFieldState();
        mQuoteTestCache.myTempAskPrevClosePriceFieldState = listener.getAskPrevClosePriceFieldState();
        mQuoteTestCache.myTempAskPrevCloseDateFieldState  = listener.getAskPrevCloseDateFieldState();
        mQuoteTestCache.myTempAskHighFieldState           = listener.getAskHighFieldState();
        mQuoteTestCache.myTempAskLowFieldState            = listener.getAskLowFieldState();
        mQuoteTestCache.myTempEventSeqNumFieldState       = quote.getEventSeqNumFieldState();
        mQuoteTestCache.myTempEventTimeFieldState         = quote.getEventTimeFieldState();
        mQuoteTestCache.myTempQuoteDateFieldState         = quote.getQuoteDateFieldState();
        mQuoteTestCache.myTempQuoteQualStrFieldState      = quote.getQuoteQualStrFieldState();
        mQuoteTestCache.myTempQuoteQualNativeFieldState   = quote.getQuoteQualNativeFieldState();
        mQuoteTestCache.myTempAskTimeFieldState           = quote.getAskTimeFieldState();
        mQuoteTestCache.myTempBidTimeFieldState           = quote.getBidTimeFieldState();
        mQuoteTestCache.myTempAskIndicatorFieldState      = quote.getAskIndicatorFieldState();
        mQuoteTestCache.myTempBidIndicatorFieldState      = quote.getBidIndicatorFieldState();
        //myTempAskUpdateCountFieldState                    = quote.getAskUpdateCountFieldState();
        //myTempBidUpdateCountFieldState                    = quote.getBidUpdateCountFieldState();
        mQuoteTestCache.myTempAskYieldFieldState          = quote.getAskYieldFieldState();
        mQuoteTestCache.myTempBidYieldFieldState          = quote.getBidYieldFieldState();
        mQuoteTestCache.myTempBidSizesListFieldState      = quote.getBidSizesListFieldState();
        mQuoteTestCache.myTempAskSizesListFieldState      = quote.getAskSizesListFieldState();
        mQuoteTestCache.myTempShortSaleBidTickFieldState  = quote.getShortSaleBidTickFieldState();           
        //myTempAskUpdateCountFieldState                    = quote.getAskUpdateCountFieldState();
        //myTempBidUpdateCountFieldState                    = quote.getBidUpdateCountFieldState();

    }

    void onQuoteGap (
        MamdaSubscription*      subscription,
        MamdaQuoteListener&     listener,
        const MamaMsg&          msg,
        const MamdaQuoteGap&    event,
        const MamdaQuoteRecap&  recap)                           
    {     
    }

    void onQuoteClosing (
        MamdaSubscription*        subscription,
        MamdaQuoteListener&       listener,
        const MamaMsg&            msg,
        const MamdaQuoteClosing&  event,
        const MamdaQuoteRecap&    recap)
    {  
    }

    void onQuoteOutOfSequence (
        MamdaSubscription*              subscription,
        MamdaQuoteListener&             listener,
        const MamaMsg&                  msg,
        const MamdaQuoteOutOfSequence&  event,
        const MamdaQuoteRecap&          recap)
    {                  
    }

    void onQuotePossiblyDuplicate (
        MamdaSubscription*                  subscription,
        MamdaQuoteListener&                 listener,
        const MamaMsg&                      msg,
        const MamdaQuotePossiblyDuplicate&  event,
        const MamdaQuoteRecap&              recap)
    {  
    }

    void onError (
        MamdaSubscription*   subscription,
        MamdaErrorSeverity   severity,
        MamdaErrorCode       code,
        const char*          errorStr)
    {    
    }

    void onQuality (
        MamdaSubscription*   subscription,
        mamaQuality          quality)
    {        
    }
  
    QuoteControlCache  mQuoteControlCache;
    QuoteTestCache     mQuoteTestCache;
};

class MamdaQuoteFieldTests : public ::testing::Test
{
protected:
    MamdaQuoteFieldTests () {}
    virtual ~MamdaQuoteFieldTests () {}
    
    virtual void SetUp()
    {
        try
        {
            mamaBridge bridge;
            bridge = Mama::loadBridge("wmw");
            Mama::open();
            //mama_enableLogging (stderr, MAMA_LOG_LEVEL_FINEST);
            mDictionary = new MamaDictionary;
            if (!mDictionary)
            {
                FAIL() << "Failed to allocate MamaDictionary\n";
                return;
            }
            
            mDictionary->populateFromFile("dictionary.txt");
            MamdaCommonFields::setDictionary (*mDictionary);
            MamdaQuoteFields::reset();
            MamdaQuoteFields::setDictionary (*mDictionary);
            mSubscription = new MamdaSubscription;
            if (!mSubscription)
            {
                FAIL() << "Failed to allocate MamdaSubscription\n";
                return;
            }
            
            mQuoteListener = new MamdaQuoteListener; 
            if (!mQuoteListener)
            {
                FAIL() << "Failed to allocate MamdaTradeListener\n";
                return;
            }
            mSubscription->addMsgListener(mQuoteListener);      
        }
        catch(MamaStatus status)
        {
            FAIL() << "Failed to setup Mamda: "
                << status.toString() << "\n";
            return;
        }
    }  
    
    virtual void TearDown()
    {       
        
        if (mSubscription)
        {  
            delete mSubscription;
            mSubscription = NULL;
        }
        if (mQuoteListener)
        {   
            delete mQuoteListener;
            mQuoteListener = NULL;
        }    
        if (mDictionary)
        {     
            delete mDictionary;
            mDictionary = NULL;
        }           
    }        
     
    MamaDictionary*         mDictionary;
    MamdaSubscription*      mSubscription;
    MamdaQuoteListener*     mQuoteListener;    
};


TEST_F (MamdaQuoteFieldTests,QuoteFieldTests)
{  
    QuoteTickerCB* ticker = new QuoteTickerCB;
    initializeQuoteControlCache (ticker->mQuoteControlCache);      
    mQuoteListener->addHandler (ticker);   

    MamaMsg* newMessage = NULL;
    newMessage = new MamaMsg();
    newMessage->create();    
   
    MamaDateTime* DateTime = NULL;
    DateTime = new MamaDateTime ("2010/02/02", NULL);    
    
    SetQuoteCommonFields   (*newMessage, *DateTime);
    ticker->callMamdaOnMsg (mSubscription, *newMessage); 
    //ticker->getCache (*mQuoteListener);
    
    EXPECT_STREQ (ticker->mQuoteControlCache.mySymbol,          ticker->mQuoteTestCache.myTempSymbol);
    EXPECT_STREQ (ticker->mQuoteControlCache.mySrcTime,         ticker->mQuoteTestCache.myTempSrcTime);
    EXPECT_STREQ (ticker->mQuoteControlCache.myActTime,         ticker->mQuoteTestCache.myTempActTime);
    EXPECT_STREQ (ticker->mQuoteControlCache.myLineTime,        ticker->mQuoteTestCache.myTempLineTime);
    EXPECT_STREQ (ticker->mQuoteControlCache.myPubId,           ticker->mQuoteTestCache.myTempPubId);
    EXPECT_EQ    (ticker->mQuoteControlCache.myBidPrice,        ticker->mQuoteTestCache.myTempBidPrice);
    EXPECT_EQ    (ticker->mQuoteControlCache.myBidSize,         ticker->mQuoteTestCache.myTempBidSize);
    EXPECT_EQ    (ticker->mQuoteControlCache.myBidDepth,        ticker->mQuoteTestCache.myTempBidDepth);
    EXPECT_STREQ (ticker->mQuoteControlCache.myBidPartId,       ticker->mQuoteTestCache.myTempBidPartId);
    EXPECT_EQ    (ticker->mQuoteControlCache.myBidClosePrice,   ticker->mQuoteTestCache.myTempBidClosePrice);
    EXPECT_STREQ (ticker->mQuoteControlCache.myBidCloseDate,    ticker->mQuoteTestCache.myTempBidCloseDate);
    //EXPECT_EQ    (ticker->mQuoteControlCache.myBidPrevClosePrice,ticker->mQuoteTestCache.myTempBidPrevClosePrice);
    //EXPECT_STREQ (ticker->mQuoteControlCache.myBidPrevCloseDate,ticker->mQuoteTestCache.myTempBidPrevCloseDate);
    EXPECT_EQ    (ticker->mQuoteControlCache.myBidHigh,         ticker->mQuoteTestCache.myTempBidHigh);
    EXPECT_EQ    (ticker->mQuoteControlCache.myBidLow,          ticker->mQuoteTestCache.myTempBidLow);
    EXPECT_EQ    (ticker->mQuoteControlCache.myAskPrice,        ticker->mQuoteTestCache.myTempAskPrice);
    EXPECT_EQ    (ticker->mQuoteControlCache.myAskSize,         ticker->mQuoteTestCache.myTempAskSize);
    EXPECT_EQ    (ticker->mQuoteControlCache.myAskDepth,        ticker->mQuoteTestCache.myTempAskDepth); 
    EXPECT_STREQ (ticker->mQuoteControlCache.myAskPartId,       ticker->mQuoteTestCache.myTempAskPartId);
    EXPECT_EQ    (ticker->mQuoteControlCache.myAskClosePrice,   ticker->mQuoteTestCache.myTempAskClosePrice);
    EXPECT_STREQ (ticker->mQuoteControlCache.myAskCloseDate,    ticker->mQuoteTestCache.myTempAskCloseDate);
    //EXPECT_EQ    (ticker->mQuoteControlCache.myAskPrevClosePrice,ticker->mQuoteTestCache.myTempAskPrevClosePrice);
    //EXPECT_STREQ (ticker->mQuoteControlCache.myAskPrevCloseDate,ticker->mQuoteTestCache.myTempAskPrevCloseDate);
    EXPECT_EQ    (ticker->mQuoteControlCache.myAskHigh,         ticker->mQuoteTestCache.myTempAskHigh);
    EXPECT_EQ    (ticker->mQuoteControlCache.myAskLow,          ticker->mQuoteTestCache.myTempAskLow);
    EXPECT_EQ    (ticker->mQuoteControlCache.myEventSeqNum,     ticker->mQuoteTestCache.myTempEventSeqNum);
    EXPECT_STREQ (ticker->mQuoteControlCache.myEventTime,       ticker->mQuoteTestCache.myTempEventTime);
    EXPECT_STREQ (ticker->mQuoteControlCache.myQuoteDate,       ticker->mQuoteTestCache.myTempQuoteDate);
    EXPECT_STREQ (ticker->mQuoteControlCache.myQuoteQualStr,    ticker->mQuoteTestCache.myTempQuoteQualStr);
    EXPECT_STREQ (ticker->mQuoteControlCache.myQuoteQualNative, ticker->mQuoteTestCache.myTempQuoteQualNative);
    EXPECT_STREQ (ticker->mQuoteControlCache.myAskTime,         ticker->mQuoteTestCache.myTempAskTime);
    EXPECT_STREQ (ticker->mQuoteControlCache.myBidTime,         ticker->mQuoteTestCache.myTempBidTime);
    EXPECT_STREQ (ticker->mQuoteControlCache.myAskIndicator,    ticker->mQuoteTestCache.myTempAskIndicator);
    EXPECT_STREQ (ticker->mQuoteControlCache.myBidIndicator,    ticker->mQuoteTestCache.myTempBidIndicator);
    //EXPECT_EQ    (ticker->mQuoteControlCache.myAskUpdateCount,  ticker->mQuoteTestCache.myTempAskUpdateCount);
    //EXPECT_EQ    (ticker->mQuoteControlCache.myBidUpdateCount,  ticker->mQuoteTestCache.myTempBidUpdateCount);
    EXPECT_EQ    (ticker->mQuoteControlCache.myAskYield,        ticker->mQuoteTestCache.myTempAskYield);
    EXPECT_EQ    (ticker->mQuoteControlCache.myBidYield,        ticker->mQuoteTestCache.myTempBidYield);
    EXPECT_STREQ (ticker->mQuoteControlCache.myBidSizesList,    ticker->mQuoteTestCache.myTempBidSizesList);
    EXPECT_STREQ (ticker->mQuoteControlCache.myAskSizesList,    ticker->mQuoteTestCache.myTempAskSizesList);
    EXPECT_EQ    (ticker->mQuoteControlCache.myShortSaleBidTick,ticker->mQuoteTestCache.myTempShortSaleBidTick);

    delete DateTime;
    DateTime = NULL;

    delete newMessage;
    newMessage = NULL; 

    delete ticker;
    ticker = NULL;        
}


TEST_F (MamdaQuoteFieldTests,QuoteFieldStateTests)
{  
    QuoteTickerCB* ticker = new QuoteTickerCB;
    initializeQuoteControlCache (ticker->mQuoteControlCache);      
    mQuoteListener->addHandler (ticker);   

    MamaMsg* newMessage = NULL;
    newMessage = new MamaMsg();
    newMessage->create();       
   
    MamaDateTime* DateTime = NULL;
    DateTime = new MamaDateTime ("2010/02/02", NULL);   

    SetQuoteClosingFields  (*newMessage,*DateTime);          
    ticker->callMamdaOnMsg (mSubscription, *newMessage);
     
    EXPECT_EQ (ticker->mQuoteControlCache.mySymbolFieldState,           ticker->mQuoteTestCache.myTempSymbolFieldState);
    //EXPECT_EQ (ticker->mQuoteControlCache.mPartIdFieldState,            ticker->mQuoteTestCache.myTempPartIdFieldState);
    EXPECT_EQ (ticker->mQuoteControlCache.mySrcTimeFieldState,          ticker->mQuoteTestCache.myTempSrcTimeFieldState);
    EXPECT_EQ (ticker->mQuoteControlCache.myActTimeFieldState,          ticker->mQuoteTestCache.myTempActTimeFieldState);
    EXPECT_EQ (ticker->mQuoteControlCache.myLineTimeFieldState,         ticker->mQuoteTestCache.myTempLineTimeFieldState);
    //EXPECT_EQ (ticker->mQuoteControlCache.mSendTimeFieldState,          ticker->mQuoteTestCache.myTempSendTimeFieldState);
    EXPECT_EQ (ticker->mQuoteControlCache.myPubIdFieldState,            ticker->mQuoteTestCache.myTempPubIdFieldState);
    EXPECT_EQ (ticker->mQuoteControlCache.myBidPriceFieldState,         ticker->mQuoteTestCache.myTempBidPriceFieldState);
    EXPECT_EQ (ticker->mQuoteControlCache.myBidSizeFieldState,          ticker->mQuoteTestCache.myTempBidSizeFieldState);
    EXPECT_EQ (ticker->mQuoteControlCache.myBidDepthFieldState,         ticker->mQuoteTestCache.myTempBidDepthFieldState);
    EXPECT_EQ (ticker->mQuoteControlCache.myBidPartIdFieldState,        ticker->mQuoteTestCache.myTempBidPartIdFieldState);
    EXPECT_EQ (ticker->mQuoteControlCache.myBidClosePriceFieldState,    ticker->mQuoteTestCache.myTempBidClosePriceFieldState);
    EXPECT_EQ (ticker->mQuoteControlCache.myBidCloseDateFieldState,     ticker->mQuoteTestCache.myTempBidCloseDateFieldState);
    //EXPECT_EQ (ticker->mQuoteControlCache.myBidPrevClosePriceFieldState,ticker->mQuoteTestCache.myTempBidPrevClosePriceFieldState);
    //EXPECT_EQ (ticker->mQuoteControlCache.myBidPrevCloseDateFieldState, ticker->mQuoteTestCache.myTempBidPrevCloseDateFieldState);
    EXPECT_EQ (ticker->mQuoteControlCache.myBidHighFieldState,          ticker->mQuoteTestCache.myTempBidHighFieldState);
    EXPECT_EQ (ticker->mQuoteControlCache.myBidLowFieldState,           ticker->mQuoteTestCache.myTempBidLowFieldState);
    EXPECT_EQ (ticker->mQuoteControlCache.myAskPriceFieldState,         ticker->mQuoteTestCache.myTempAskPriceFieldState);
    EXPECT_EQ (ticker->mQuoteControlCache.myAskSizeFieldState,          ticker->mQuoteTestCache.myTempAskSizeFieldState);
    EXPECT_EQ (ticker->mQuoteControlCache.myAskDepthFieldState,         ticker->mQuoteTestCache.myTempAskDepthFieldState);
    EXPECT_EQ (ticker->mQuoteControlCache.myAskPartIdFieldState,        ticker->mQuoteTestCache.myTempAskPartIdFieldState);
    EXPECT_EQ (ticker->mQuoteControlCache.myAskClosePriceFieldState,    ticker->mQuoteTestCache.myTempAskClosePriceFieldState);
    EXPECT_EQ (ticker->mQuoteControlCache.myAskCloseDateFieldState,     ticker->mQuoteTestCache.myTempAskCloseDateFieldState);
    //EXPECT_EQ (ticker->mQuoteControlCache.myAskPrevClosePriceFieldState,ticker->mQuoteTestCache.myTempAskPrevClosePriceFieldState);
    //EXPECT_EQ (ticker->mQuoteControlCache.myAskPrevCloseDateFieldState, ticker->mQuoteTestCache.myTempAskPrevCloseDateFieldState);
    EXPECT_EQ (ticker->mQuoteControlCache.myAskHighFieldState,          ticker->mQuoteTestCache.myTempAskHighFieldState);
    EXPECT_EQ (ticker->mQuoteControlCache.myAskLowFieldState,           ticker->mQuoteTestCache.myTempAskLowFieldState);
    EXPECT_EQ (ticker->mQuoteControlCache.myEventSeqNumFieldState,      ticker->mQuoteTestCache.myTempEventSeqNumFieldState);
    EXPECT_EQ (ticker->mQuoteControlCache.myEventTimeFieldState,        ticker->mQuoteTestCache.myTempEventTimeFieldState);
    EXPECT_EQ (ticker->mQuoteControlCache.myQuoteDateFieldState,        ticker->mQuoteTestCache.myTempQuoteDateFieldState);
    EXPECT_EQ (ticker->mQuoteControlCache.myQuoteQualStrFieldState,     ticker->mQuoteTestCache.myTempQuoteQualStrFieldState);
    EXPECT_EQ (ticker->mQuoteControlCache.myQuoteQualNativeFieldState,  ticker->mQuoteTestCache.myTempQuoteQualNativeFieldState);
    EXPECT_EQ (ticker->mQuoteControlCache.myAskTimeFieldState,          ticker->mQuoteTestCache.myTempAskTimeFieldState);
    EXPECT_EQ (ticker->mQuoteControlCache.myBidTimeFieldState,          ticker->mQuoteTestCache.myTempBidTimeFieldState);
    EXPECT_EQ (ticker->mQuoteControlCache.myAskIndicatorFieldState,     ticker->mQuoteTestCache.myTempAskIndicatorFieldState);
    EXPECT_EQ (ticker->mQuoteControlCache.myBidIndicatorFieldState,     ticker->mQuoteTestCache.myTempBidIndicatorFieldState);
    //EXPECT_EQ (ticker->mQuoteControlCache.myAskUpdateCountFieldState,   ticker->mQuoteTestCache.myTempAskUpdateCountFieldState);
    //EXPECT_EQ (ticker->mQuoteControlCache.myBidUpdateCountFieldState,   ticker->mQuoteTestCache.myTempBidUpdateCountFieldState);
    EXPECT_EQ (ticker->mQuoteControlCache.myAskYieldFieldState,         ticker->mQuoteTestCache.myTempAskYieldFieldState);
    EXPECT_EQ (ticker->mQuoteControlCache.myBidYieldFieldState,         ticker->mQuoteTestCache.myTempBidYieldFieldState);
    EXPECT_EQ (ticker->mQuoteControlCache.myBidSizesListFieldState,     ticker->mQuoteTestCache.myTempBidSizesListFieldState);
    EXPECT_EQ (ticker->mQuoteControlCache.myAskSizesListFieldState,     ticker->mQuoteTestCache.myTempAskSizesListFieldState);
    //EXPECT_EQ (ticker->mQuoteControlCache.myShortSaleBidTickFieldState, ticker->mQuoteTestCache.myTempShortSaleBidTickFieldState);
   
    delete DateTime;
    DateTime = NULL;

    delete newMessage;
    newMessage = NULL; 

    delete ticker;
    ticker = NULL;      
}



