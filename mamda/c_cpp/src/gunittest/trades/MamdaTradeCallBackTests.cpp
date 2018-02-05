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

#include <mamda/MamdaTradeFields.h>
#include <mamda/MamdaCommonFields.h>
#include <mamda/MamdaSubscription.h>
#include <mamda/MamdaTradeHandler.h>
#include <mamda/MamdaConfig.h>
#include <mamda/MamdaTradeListener.h>
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

class TradeTickerCB : public MamdaTradeHandler                  
{
public:
    TradeTickerCB () {}
    virtual ~TradeTickerCB () {}    
        

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

    void onTradeRecap (
        MamdaSubscription*      subscription,
        MamdaTradeListener&     listener,
        const MamaMsg&          msg,
        const MamdaTradeRecap&  recap)
    {    
        mTradeTestFields.myTempSymbol = recap.getSymbol ();  
    }

    void onTradeReport (
        MamdaSubscription*      subscription,
        MamdaTradeListener&     listener,
        const MamaMsg&          msg,
        const MamdaTradeReport& event,
        const MamdaTradeRecap&  recap)
    {  
        mTradeTestFields.myTempSymbol            = recap.getSymbol ();
        mTradeTestFields.myTempTradeCount        = recap.getTradeCount ();
        mTradeTestFields.myTempTradeVolume       = event.getTradeVolume ();
        mTradeTestFields.myTempTradePrice        = event.getTradePrice().getAsString();
        mTradeTestFields.myTempTradeEventSeqNum  = event.getEventSeqNum ();
        mTradeTestFields.myTempTradeEventTime    = event.getEventTime().getAsString();
        mTradeTestFields.myTempTradeActivityTime = event.getActivityTime().getAsString();
        mTradeTestFields.myTempTradePartId       = event.getTradePartId();
        mTradeTestFields.myTempTradeQual         = event.getTradeQual();
        mTradeTestFields.myTempTradeQualNative   = event.getTradeQualNative();
        mTradeTestFields.myTempTradeIsIrregular  = event.getIsIrregular(); 
    }

    void onTradeGap (
        MamdaSubscription*              subscription,
        MamdaTradeListener&             listener,
        const MamaMsg&                  msg,
        const MamdaTradeGap&            event,
        const MamdaTradeRecap&          recap)
    {   
        mTradeTestFields.myTempSymbol = recap.getSymbol();         
    }

    void onTradeCancelOrError (
        MamdaSubscription*              subscription,
        MamdaTradeListener&             listener,
        const MamaMsg&                  msg,
        const MamdaTradeCancelOrError&  event,
        const MamdaTradeRecap&          recap)
    {            
       mTradeTestFields.myTempSymbol = subscription->getSymbol();     
    }

    void onTradeCorrection (
        MamdaSubscription*              subscription,
        MamdaTradeListener&             listener,
        const MamaMsg&                  msg,
        const MamdaTradeCorrection&     event,
        const MamdaTradeRecap&          recap)
    {     
        mTradeTestFields.myTempSymbol = recap.getSymbol();          
    }

    void onTradeClosing (
        MamdaSubscription*              subscription,
        MamdaTradeListener&             listener,
        const MamaMsg&                  msg,
        const MamdaTradeClosing&        event,
        const MamdaTradeRecap&          recap)
    {   
        mTradeTestFields.myTempSymbol = recap.getSymbol();
    }

    void onTradeOutOfSequence (
        MamdaSubscription*              subscription,
        MamdaTradeListener&             listener,
        const MamaMsg&                  msg,
        const MamdaTradeOutOfSequence&  event,
        const MamdaTradeRecap&          recap) 
    {   
        mTradeTestFields.myTempSymbol           = subscription->getSymbol();       
        mTradeTestFields.myTempTradeMsgQualStr  = event.getMsgQual().getAsString();
        mTradeTestFields.myTempTradeMsgQual     = event.getMsgQual().getValue();
        mTradeTestFields.myTempTradePrice       = event.getTradePrice().getAsString();
        mTradeTestFields.myTempAccVolume        = recap.getAccVolume();
        mTradeTestFields.myTempTradeEventSeqNum = event.getEventSeqNum();
        mTradeTestFields.myTempTradeEventTime   = event.getEventTime().getAsString();  
    }

    void onTradePossiblyDuplicate (
        MamdaSubscription*                  subscription,
        MamdaTradeListener&                 listener,
        const MamaMsg&                      msg,
        const MamdaTradePossiblyDuplicate&  event,
        const MamdaTradeRecap&              recap) 
    {      
        mTradeTestFields.myTempSymbol           = subscription->getSymbol ();
        mTradeTestFields.myTempTradeMsgQualStr  = event.getMsgQual().getAsString();
        mTradeTestFields.myTempTradeMsgQual     = event.getMsgQual().getValue();
        mTradeTestFields.myTempTradePrice       = event.getTradePrice().getAsString();
        mTradeTestFields.myTempAccVolume        = recap.getAccVolume ();
        mTradeTestFields.myTempTradeEventSeqNum = event.getEventSeqNum();
        mTradeTestFields.myTempTradeEventTime   = event.getEventTime().getAsString();            
    }

    void onError (
        MamdaSubscription*   subscription,
        MamdaErrorSeverity   severity,
        MamdaErrorCode       errorCode,
        const char*          errorStr)
    {            
    }

    void onQuality (
        MamdaSubscription*   subscription,
        mamaQuality          quality)
    {  
    }

    TradeControlFields mTradeControlFields;
    TradeTestFields    mTradeTestFields;
};

class MamdaTradeCBTest : public ::testing::Test
{
protected:
    MamdaTradeCBTest () {}
    virtual ~MamdaTradeCBTest () {}
  
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
            MamdaTradeFields::reset();
            MamdaTradeFields::setDictionary (*myDictionary);
            mySubscription = new MamdaSubscription;
            if (!mySubscription)
            {
                FAIL() << "Failed to allocate MamdaSubscription\n";
                return;
            }
            myTradeListener = new MamdaTradeListener; 
            if (!myTradeListener)
            {
                FAIL() << "Failed to allocate MamdaTradeListener\n";
                return;
            }
            
            mySubscription->addMsgListener(myTradeListener);       
        }
        catch(MamaStatus status)
        {
            FAIL() << "Failed to setup Mamda: "
            << status.toString() << "\n";
            return;
        }
        mySubscription->setSymbol("SPY"); 
    }  
    
    virtual void TearDown()
    {    
        if (mySubscription)
        {  
            delete mySubscription;
            mySubscription = NULL;
        }              
        if (myTradeListener)
        {   
            delete myTradeListener;
            myTradeListener = NULL;
        }    
        if (myDictionary)
        {     
            delete myDictionary;
            myDictionary = NULL;
        }      
    }        
     
    MamaDictionary*         myDictionary;
    MamdaSubscription*      mySubscription;
    MamdaTradeListener*     myTradeListener;    
};

TEST_F (MamdaTradeCBTest, TradeRecapCallback)
{
    TradeTickerCB* ticker = new TradeTickerCB;      
    myTradeListener->addHandler (ticker);   

    MamaMsg* newMessage = NULL;
    newMessage = new MamaMsg();
    newMessage->create();
    SetTradeRecapFields(*newMessage);          
    ticker->callMamdaOnMsg(mySubscription, *newMessage);         
      
    EXPECT_STREQ (ticker->mTradeControlFields.mySymbol, ticker->mTradeTestFields.myTempSymbol);
 
    delete ticker;
    ticker = NULL;        

    delete newMessage;
    newMessage = NULL;    
}

TEST_F (MamdaTradeCBTest, TradeReportCallback)
{
    TradeTickerCB* ticker = new TradeTickerCB;      
    myTradeListener->addHandler (ticker);   

    MamaMsg* newMessage = NULL;
    newMessage = new MamaMsg();
    newMessage->create();
    
    MamaDateTime* Date = NULL;
    Date = new MamaDateTime("13:20:33.767");
    
    SetTradeReportFields(*newMessage,*Date);          
    ticker->callMamdaOnMsg(mySubscription, *newMessage);         
    
    EXPECT_STREQ (ticker->mTradeControlFields.mySymbol,            ticker->mTradeTestFields.myTempSymbol);
    EXPECT_EQ    (ticker->mTradeControlFields.myTradeCount,        ticker->mTradeTestFields.myTempTradeCount);
    EXPECT_EQ    (ticker->mTradeControlFields.myTradeVolume,       ticker->mTradeTestFields.myTempTradeVolume);
    EXPECT_STREQ (ticker->mTradeControlFields.myTradePrice,        ticker->mTradeTestFields.myTempTradePrice);
    EXPECT_EQ    (ticker->mTradeControlFields.myTradeEventSeqNum,  ticker->mTradeTestFields.myTempTradeEventSeqNum);
    EXPECT_STREQ (ticker->mTradeControlFields.myTradeEventTime,    ticker->mTradeTestFields.myTempTradeEventTime);
    EXPECT_STREQ (ticker->mTradeControlFields.myTradeActivityTime, ticker->mTradeTestFields.myTempTradeActivityTime);
    EXPECT_STREQ (ticker->mTradeControlFields.myTradePartId,       ticker->mTradeTestFields.myTempTradePartId);
    EXPECT_STREQ (ticker->mTradeControlFields.myTradeQual,         ticker->mTradeTestFields.myTempTradeQual);
    EXPECT_STREQ (ticker->mTradeControlFields.myTradeQualNative,   ticker->mTradeTestFields.myTempTradeQualNative);
    EXPECT_EQ    (ticker->mTradeControlFields.myTradeIsIrregular,  ticker->mTradeTestFields.myTempTradeIsIrregular);   
  
    delete ticker;
    ticker = NULL;        

    delete newMessage;
    newMessage = NULL;
}

TEST_F (MamdaTradeCBTest, TradeCancelOrErrorCallback)
{
    TradeTickerCB* ticker = new TradeTickerCB;      
    myTradeListener->addHandler (ticker);   

    MamaMsg* newMessage = NULL;
    newMessage = new MamaMsg();
    newMessage->create();
    SetTradeCancelOrErrorFields(*newMessage);          
    ticker->callMamdaOnMsg(mySubscription, *newMessage); 
    
    EXPECT_STREQ (ticker->mTradeControlFields.mySymbol, ticker->mTradeTestFields.myTempSymbol);
    
    delete ticker;
    ticker = NULL;        

    delete newMessage;
    newMessage = NULL;
}

TEST_F (MamdaTradeCBTest, TradeCorrectionCallback)
{
    TradeTickerCB* ticker = new TradeTickerCB;      
    myTradeListener->addHandler (ticker);   

    MamaMsg* newMessage = NULL;
    newMessage = new MamaMsg();
    newMessage->create();
    SetTradeCorrectionFields(*newMessage);          
    ticker->callMamdaOnMsg(mySubscription, *newMessage);    

    EXPECT_STREQ (ticker->mTradeControlFields.mySymbol, ticker->mTradeTestFields.myTempSymbol);

    delete ticker;
    ticker = NULL;        

    delete newMessage;
    newMessage = NULL;
}

TEST_F (MamdaTradeCBTest, TradeGapCallback)
{
    TradeTickerCB* ticker = new TradeTickerCB;      
    myTradeListener->addHandler (ticker);   

    MamaMsg* newMessage[2];
    newMessage[0] = new MamaMsg();
    newMessage[1] = new MamaMsg();
    newMessage[0]->create();   
    newMessage[1]->create();     
    SetTradeGapFields1(*newMessage[0]); 
    SetTradeGapFields2(*newMessage[1]);  
    
    ticker->callMamdaOnMsg (mySubscription, *newMessage[0]);          
    ticker->callMamdaOnMsg (mySubscription, *newMessage[1]);
    
    EXPECT_STREQ (ticker->mTradeControlFields.mySymbol, ticker->mTradeTestFields.myTempSymbol);
     
    delete ticker;
    ticker = NULL;        

    delete newMessage[0];
    newMessage[0] = NULL;

    delete newMessage[1];
    newMessage[1] = NULL;
}

TEST_F (MamdaTradeCBTest, TradeOutOfSeqCallback)
{
    TradeTickerCB* ticker = new TradeTickerCB;      
    myTradeListener->addHandler (ticker);   
    myTradeListener->usePosDupAndOutOfSeqHandlers(true);

    MamaMsg* newMessage = NULL;
    newMessage = new MamaMsg();
    newMessage->create();
    SetTradeOutOfSeqFields (*newMessage);          
    ticker->callMamdaOnMsg (mySubscription, *newMessage);    
    
    EXPECT_STREQ (ticker->mTradeControlFields.mySymbol,           ticker->mTradeTestFields.myTempSymbol);           
    EXPECT_STREQ (ticker->mTradeControlFields.myTradeMsgQualStr,  ticker->mTradeTestFields.myTempTradeMsgQualStr); 
    EXPECT_EQ    (ticker->mTradeControlFields.myTradeMsgQual,     ticker->mTradeTestFields.myTempTradeMsgQual);    
    EXPECT_STREQ (ticker->mTradeControlFields.myTradePrice,       ticker->mTradeTestFields.myTempTradePrice);    
    EXPECT_EQ    (ticker->mTradeControlFields.myAccVolume,        ticker->mTradeTestFields.myTempAccVolume);    
    EXPECT_EQ    (ticker->mTradeControlFields.myTradeEventSeqNum, ticker->mTradeTestFields.myTempTradeEventSeqNum);
    EXPECT_STREQ (ticker->mTradeControlFields.myTradeEventTime,   ticker->mTradeTestFields.myTempTradeEventTime);    
   
    delete ticker;
    ticker = NULL;        

    delete newMessage;
    newMessage = NULL;
}

TEST_F (MamdaTradeCBTest, TradeClosingCallback)
{
    TradeTickerCB* ticker = new TradeTickerCB;      
    myTradeListener->addHandler (ticker);   
   
    MamaMsg* newMessage = NULL;
    newMessage = new MamaMsg();
    newMessage->create();
    SetTradeClosingFields  (*newMessage);          
    ticker->callMamdaOnMsg (mySubscription, *newMessage);         
        
    EXPECT_STREQ (ticker->mTradeControlFields.mySymbol, ticker->mTradeTestFields.myTempSymbol);
    
    delete ticker;
    ticker = NULL;        

    delete newMessage;
    newMessage = NULL; 
}
 
TEST_F (MamdaTradeCBTest, TradePossDupCallback)
{
    TradeTickerCB* ticker = new TradeTickerCB;      
    myTradeListener->addHandler (ticker);   
    myTradeListener->usePosDupAndOutOfSeqHandlers(true);

    MamaMsg* newMessage = NULL;
    newMessage = new MamaMsg();
    newMessage->create();
    SetTradePossDupFields(*newMessage);          
    ticker->callMamdaOnMsg(mySubscription, *newMessage);    
    
     EXPECT_STREQ (ticker->mTradeControlFields.mySymbol,                 ticker->mTradeTestFields.myTempSymbol);           
     EXPECT_STREQ (ticker->mTradeControlFields.myTradeMsgQualPossDupStr, ticker->mTradeTestFields.myTempTradeMsgQualStr);  
     EXPECT_EQ    (ticker->mTradeControlFields.myTradeMsgQualPossDup,    ticker->mTradeTestFields.myTempTradeMsgQual);    
     EXPECT_STREQ (ticker->mTradeControlFields.myTradePrice,             ticker->mTradeTestFields.myTempTradePrice);   
     EXPECT_EQ    (ticker->mTradeControlFields.myAccVolume,              ticker->mTradeTestFields.myTempAccVolume);       
     EXPECT_EQ    (ticker->mTradeControlFields.myTradeEventSeqNum,       ticker->mTradeTestFields.myTempTradeEventSeqNum); 
     EXPECT_STREQ (ticker->mTradeControlFields.myTradeEventTime,         ticker->mTradeTestFields.myTempTradeEventTime);     

    delete ticker;
    ticker = NULL;        

    delete newMessage;
    newMessage = NULL;
}





