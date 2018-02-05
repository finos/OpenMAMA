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

#include <gtest/gtest.h>

#include <mamda/MamdaQuoteFields.h>
#include <mamda/MamdaCommonFields.h>
#include <mama/MamaDictionary.h>
#include <mama/mamacpp.h>
#include <mama/MamaMsg.h>
#include <mamda/MamdaSubscription.h>
#include <mamda/MamdaQuoteHandler.h>
#include <mamda/MamdaConfig.h>
#include <mamda/MamdaQuoteListener.h>
#include <mamda/MamdaDataException.h>
#include <mamda/MamdaFieldState.h>
#include <mamda/MamdaErrorListener.h>
#include <mamda/MamdaQualityListener.h>

#include "common/MamdaUnitTestUtils.h"
#include "common/CpuTestGenerator.h"
#include "common/MemoryTestGenerator.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <fstream>

using std::string;

using namespace std;
using namespace Wombat;

class QuoteTickerCB : public MamdaQuoteHandler                  
{
public:
    QuoteTickerCB () {}
    virtual ~QuoteTickerCB () {} 

    void callMamdaOnMsg(MamdaSubscription* sub, MamaMsg& msg)
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
        mQuoteTestFields.myTempSymbol = recap.getSymbol(); 
    }

    void onQuoteUpdate (
        MamdaSubscription*      subscription,
        MamdaQuoteListener&     listener,
        const MamaMsg&          msg,
        const MamdaQuoteUpdate& quote,
        const MamdaQuoteRecap&  recap)
    {   
        mQuoteTestFields.myTempSymbol         = quote.getSymbol();
        mQuoteTestFields.myTempQuoteCount     = recap.getQuoteCount ();
        mQuoteTestFields.myTempBidPriceStr    = quote.getBidPrice().getAsString();
        mQuoteTestFields.myTempBidSize        = quote.getBidSize ();
        mQuoteTestFields.myTempAskSize        = quote.getAskSize ();
        mQuoteTestFields.myTempAskPriceStr    = quote.getAskPrice().getAsString();
        mQuoteTestFields.myTempEventSeqNum    = quote.getEventSeqNum();
        mQuoteTestFields.myTempEventTimeStr   = listener.getEventTime().getAsString();
        mQuoteTestFields.myTempQuoteQualStr   = quote.getQuoteQualStr(); 
    }

    void onQuoteGap (
        MamdaSubscription*      subscription,
        MamdaQuoteListener&     listener,
        const MamaMsg&          msg,
        const MamdaQuoteGap&    event,
        const MamdaQuoteRecap&  recap)
    {        
        mQuoteTestFields.myTempBeginGapSeqNum = event.getBeginGapSeqNum(); 
        mQuoteTestFields.myTempEndGapSeqNum   = event.getEndGapSeqNum();
    }

    void onQuoteClosing (
        MamdaSubscription*        subscription,
        MamdaQuoteListener&       listener,
        const MamaMsg&            msg,
        const MamdaQuoteClosing&  event,
        const MamdaQuoteRecap&    recap)
    {  
        mQuoteTestFields.myTempSymbol           = event.getSymbol();
        mQuoteTestFields.myTempBidClosePriceStr = event.getBidClosePrice().getAsString();
        mQuoteTestFields.myTempAskClosePriceStr = event.getAskClosePrice().getAsString();
        mQuoteTestFields.myTempEventSeqNum      = event.getEventSeqNum ();
        mQuoteTestFields.myTempEventTimeStr     = listener.getEventTime ().getAsString();                   
    }

    void onQuoteOutOfSequence (
        MamdaSubscription*              subscription,
        MamdaQuoteListener&             listener,
        const MamaMsg&                  msg,
        const MamdaQuoteOutOfSequence&  event,
        const MamdaQuoteRecap&          recap)
    {   
        mQuoteTestFields.myTempSymbol       = recap.getSymbol ();
        mQuoteTestFields.myTempMsgQualStr   = event.getMsgQual().getAsString();
        mQuoteTestFields.myTempMsgQual      = event.getMsgQual().getValue();
        mQuoteTestFields.myTempBidPriceStr  = event.getBidPrice().getAsString();
        mQuoteTestFields.myTempAskPriceStr  = event.getAskPrice().getAsString();
        mQuoteTestFields.myTempEventSeqNum  = event.getEventSeqNum();
        mQuoteTestFields.myTempEventTimeStr = listener.getEventTime ().getAsString();          
    }

    void onQuotePossiblyDuplicate (
        MamdaSubscription*                  subscription,
        MamdaQuoteListener&                 listener,
        const MamaMsg&                      msg,
        const MamdaQuotePossiblyDuplicate&  event,
        const MamdaQuoteRecap&              recap)
    {  
        mQuoteTestFields.myTempSymbol       = recap.getSymbol();
        mQuoteTestFields.myTempMsgQualStr   = event.getMsgQual().getAsString();
        mQuoteTestFields.myTempMsgQual      = event.getMsgQual().getValue();
        mQuoteTestFields.myTempBidPriceStr  = event.getBidPrice().getAsString();
        mQuoteTestFields.myTempAskPriceStr  = event.getAskPrice().getAsString();
        mQuoteTestFields.myTempEventSeqNum  = event.getEventSeqNum();
        mQuoteTestFields.myTempEventTimeStr = listener.getEventTime().getAsString();  
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
  
    QuoteControlFields  mQuoteControlFields;
    QuoteTestFields     mQuoteTestFields;
};

class MamdaQuoteCBTest : public ::testing::Test
{
protected:
    MamdaQuoteCBTest () {}
    virtual ~MamdaQuoteCBTest () {}
  
    
    virtual void SetUp()
    {
        try
        {
            mamaBridge bridge;
            bridge = Mama::loadBridge("wmw");
            Mama::open();
            //mama_enableLogging (stderr, MAMA_LOG_LEVEL_FINEST);
            myDictionary = new MamaDictionary;
            if (!myDictionary)
            {
                FAIL() << "Failed to allocate MamaDictionary\n";
                return;
            }
            
            myDictionary->populateFromFile("dictionary.txt");
            MamdaCommonFields::setDictionary (*myDictionary);
            MamdaQuoteFields::reset();
            MamdaQuoteFields::setDictionary (*myDictionary);
            mySubscription = new MamdaSubscription;
            if (!mySubscription)
            {
                FAIL() << "Failed to allocate MamdaSubscription\n";
                return;
            }
            
            myQuoteListener = new MamdaQuoteListener; 
            mySubscription->addMsgListener(myQuoteListener);    
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
        if (mySubscription)
        {  
            delete mySubscription;
            mySubscription = NULL;
        }              
        if (myQuoteListener)
        {   
            delete myQuoteListener;
            myQuoteListener = NULL;
        }    
        if (myDictionary)
        {     
            delete myDictionary;
            myDictionary = NULL;
        }   
    }        
     
    MamaDictionary*         myDictionary;
    MamdaSubscription*      mySubscription;
    MamdaQuoteListener*     myQuoteListener;    
};

TEST_F (MamdaQuoteCBTest, QRecapCallback)
{      
    QuoteTickerCB* ticker = new QuoteTickerCB;
    initializeQuoteControlFields (ticker->mQuoteControlFields);         
    myQuoteListener->addHandler(ticker);         

    MamaMsg* newMessage = NULL;
    newMessage = new MamaMsg();
    newMessage->create();
    SetQuoteRecapFields(*newMessage);      
    ticker->callMamdaOnMsg(mySubscription, *newMessage);  
   
    EXPECT_STREQ (ticker->mQuoteControlFields.mySymbol,  ticker->mQuoteTestFields.myTempSymbol);
    
    delete ticker;
    ticker = NULL;

    delete newMessage;
    newMessage = NULL;     
}

TEST_F (MamdaQuoteCBTest, QUpdateCallback)
{  
    QuoteTickerCB* ticker = new QuoteTickerCB;
    initializeQuoteControlFields (ticker->mQuoteControlFields);         
    myQuoteListener->addHandler (ticker);   

    MamaMsg* newMessage = NULL;
    newMessage = new MamaMsg();
    newMessage->create();
    SetQuoteUpdateFields(*newMessage);          
    ticker->callMamdaOnMsg(mySubscription, *newMessage); 
    
    EXPECT_STREQ (ticker->mQuoteControlFields.mySymbol,       ticker->mQuoteTestFields.myTempSymbol); 
    EXPECT_EQ    (ticker->mQuoteControlFields.myQuoteCount,   ticker->mQuoteTestFields.myTempQuoteCount); 
    EXPECT_STREQ (ticker->mQuoteControlFields.myBidPriceStr,  ticker->mQuoteTestFields.myTempBidPriceStr); 
    EXPECT_EQ    (ticker->mQuoteControlFields.myBidSize,      ticker->mQuoteTestFields.myTempBidSize);
    EXPECT_EQ    (ticker->mQuoteControlFields.myAskSize,      ticker->mQuoteTestFields.myTempAskSize); 
    EXPECT_STREQ (ticker->mQuoteControlFields.myAskPriceStr,  ticker->mQuoteTestFields.myTempAskPriceStr); 
    EXPECT_EQ    (ticker->mQuoteControlFields.myEventSeqNum,  ticker->mQuoteTestFields.myTempEventSeqNum); 
    //EXPECT_STREQ (ticker->mQuoteControlFields.myEventTimeStr, ticker->mQuoteTestFields.myTempEventTimeStr);
    EXPECT_STREQ (ticker->mQuoteControlFields.myQuoteQualStr, ticker->mQuoteTestFields.myTempQuoteQualStr); 
   
    delete ticker;
    ticker = NULL;        

    delete newMessage;
    newMessage = NULL;  
}

TEST_F (MamdaQuoteCBTest, QGap)
{   
    QuoteTickerCB* ticker = new QuoteTickerCB;
    initializeQuoteControlFields (ticker->mQuoteControlFields);         
    myQuoteListener->addHandler (ticker);    

    MamaMsg* newMessage[2];   
    newMessage[0] = new MamaMsg();
    newMessage[1] = new MamaMsg();
    newMessage[0]->create();
    newMessage[1]->create();  
    SetQuoteGapFields1 (*newMessage[0]); 
    SetQuoteGapFields2 (*newMessage[1]); 
    ticker->callMamdaOnMsg (mySubscription, *newMessage[0]);         
    ticker->callMamdaOnMsg (mySubscription, *newMessage[1]);      
  
    EXPECT_EQ (ticker->mQuoteControlFields.myGapBeginSeqNum, ticker->mQuoteTestFields.myTempBeginGapSeqNum);
    EXPECT_EQ (ticker->mQuoteControlFields.myEndGapSeqNum,   ticker->mQuoteTestFields.myTempEndGapSeqNum);    
    
    delete ticker;
    ticker = NULL;
  
    delete newMessage[0];
    newMessage[0] = NULL;
    
    delete newMessage[1];
    newMessage[1] = NULL;    
}   

TEST_F (MamdaQuoteCBTest, QClosingCallback)
{  
    QuoteTickerCB* ticker = new QuoteTickerCB;
    initializeQuoteControlFields (ticker->mQuoteControlFields);         
    myQuoteListener->addHandler (ticker);            
            
    MamaMsg* newMessage;
    newMessage = new MamaMsg();
    newMessage->create();
    SetQuoteClosingFields(*newMessage);          
    ticker->callMamdaOnMsg(mySubscription, *newMessage);  
    
    EXPECT_STREQ (ticker->mQuoteControlFields.mySymbol,           ticker->mQuoteTestFields.myTempSymbol);          
    EXPECT_STREQ (ticker->mQuoteControlFields.myBidClosePriceStr, ticker->mQuoteTestFields.myTempBidClosePriceStr); 
    EXPECT_STREQ (ticker->mQuoteControlFields.myAskClosePriceStr, ticker->mQuoteTestFields.myTempAskClosePriceStr);
    EXPECT_EQ    (ticker->mQuoteControlFields.myEventSeqNum,      ticker->mQuoteTestFields.myTempEventSeqNum);     
    //EXPECT_STREQ (ticker->mQuoteControlFields.myEventTimeStr,     ticker->mQuoteTestFields.myTempEventTimeStr);      

    delete ticker;
    ticker = NULL;

    delete newMessage;
    newMessage = NULL;    
}
 
TEST_F (MamdaQuoteCBTest, QoutOfSeqCallback)
{  
    QuoteTickerCB* ticker = new QuoteTickerCB;
    initializeQuoteControlFields (ticker->mQuoteControlFields);         
    myQuoteListener->addHandler (ticker); 
    myQuoteListener->usePosDupAndOutOfSeqHandlers(true);

    MamaMsg* newMessage;   
    newMessage = new MamaMsg();
    newMessage->create();  
    SetQuoteOutOfSeqFields(*newMessage); 
    ticker->callMamdaOnMsg(mySubscription, *newMessage); 
    
    EXPECT_STREQ(ticker->mQuoteControlFields.mySymbol,ticker->mQuoteTestFields.myTempSymbol); 
    EXPECT_STREQ(ticker->mQuoteControlFields.myMsgQualStr,ticker->mQuoteTestFields.myTempMsgQualStr);
    EXPECT_EQ(ticker->mQuoteControlFields.myMsgQual,ticker->mQuoteTestFields.myTempMsgQual);
    EXPECT_STREQ(ticker->mQuoteControlFields.myBidPriceStr,ticker->mQuoteTestFields.myTempBidPriceStr); 
    EXPECT_STREQ(ticker->mQuoteControlFields.myAskPriceStr,ticker->mQuoteTestFields.myTempAskPriceStr); 
    EXPECT_EQ(ticker->mQuoteControlFields.myEventSeqNum,ticker->mQuoteTestFields.myTempEventSeqNum); 
    //EXPECT_STREQ(ticker->mQuoteControlFields.myEventTimeStr,ticker->mQuoteTestFields.myTempEventTimeStr); 
    
    delete ticker;
    ticker = NULL;
   
    delete newMessage;
    newMessage = NULL;     
}   

TEST_F (MamdaQuoteCBTest, QPossibilyDuplicateCallback)
{  
    QuoteTickerCB* ticker = new QuoteTickerCB;
    initializeQuoteControlFields (ticker->mQuoteControlFields);         
    myQuoteListener->addHandler (ticker);            
    myQuoteListener->usePosDupAndOutOfSeqHandlers(true);

    MamaMsg* newMessage;
    newMessage = new MamaMsg();
    newMessage->create();   
    SetQuotePossDupFields(*newMessage);  
    ticker->callMamdaOnMsg(mySubscription, *newMessage);    

    EXPECT_STREQ(ticker->mQuoteControlFields.mySymbol,ticker->mQuoteTestFields.myTempSymbol); 
    EXPECT_STREQ(ticker->mQuoteControlFields.myMsgQualPossDupStr,ticker->mQuoteTestFields.myTempMsgQualStr);
    EXPECT_EQ(ticker->mQuoteControlFields.myMsgQualPossDup,ticker->mQuoteTestFields.myTempMsgQual); 
    EXPECT_STREQ(ticker->mQuoteControlFields.myBidPriceStr,ticker->mQuoteTestFields.myTempBidPriceStr);
    EXPECT_STREQ(ticker->mQuoteControlFields.myAskPriceStr,ticker->mQuoteTestFields.myTempAskPriceStr);
    EXPECT_EQ(ticker->mQuoteControlFields.myEventSeqNum,ticker->mQuoteTestFields.myTempEventSeqNum);
    //EXPECT_STREQ(myEventTimeStr,myTempEventTimeStr);
    
    delete ticker;
    ticker = NULL;

    delete newMessage;
    newMessage = NULL;
}  



