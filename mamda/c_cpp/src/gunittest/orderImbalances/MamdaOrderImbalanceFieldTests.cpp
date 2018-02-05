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

#include <mamda/MamdaCommonFields.h>
#include <mamda/MamdaOrderImbalanceListener.h>
#include <mamda/MamdaOrderImbalanceFields.h>
#include <mamda/MamdaOrderImbalanceHandler.h>
#include <mamda/MamdaOrderImbalanceRecap.h>
#include <mamda/MamdaOrderImbalanceUpdate.h>
#include <mamda/MamdaOrderImbalanceSide.h>
#include <mamda/MamdaOrderImbalanceType.h>
#include <mamda/MamdaErrorListener.h>
#include <mamda/MamdaQualityListener.h>
#include <mamda/MamdaSubscription.h>
#include <mamda/MamdaConfig.h>
#include <mamda/MamdaDataException.h>
#include <mamda/MamdaFieldState.h>
#include <mamda/MamdaMsgListener.h>
#include <mama/MamaMsg.h>
#include <mama/MamaDictionary.h>
#include <mama/mamacpp.h>

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
#include <vector>

using std::string;

using namespace std;
using namespace Wombat;

class OrderImbalanceTickerCB : public MamdaOrderImbalanceHandler
{
    public:
        OrderImbalanceTickerCB () {}
        virtual ~OrderImbalanceTickerCB () {}


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

    void onOrderImbalance (
            MamdaSubscription*           subscription,
            MamdaOrderImbalanceListener& listener,
            const MamaMsg&               msg,
            MamdaOrderImbalanceRecap&    imbalance,
            MamdaOrderImbalanceUpdate&   update)

    {               
        mOrderImbalanceTestCache.myTestHighIndicationPrice  = listener.getBuyVolume();
        mOrderImbalanceTestCache.myTestSellVolume           = listener.getSellVolume();
        mOrderImbalanceTestCache.myTestBuyVolume            = listener.getBuyVolume();
        mOrderImbalanceTestCache.myTestHighIndicationPrice  = listener.getHighIndicationPrice().getValue();
        mOrderImbalanceTestCache.myTestLowIndicationPrice   = listener.getLowIndicationPrice().getValue();
        mOrderImbalanceTestCache.myTestIndicationPrice      = listener.getImbalancePrice().getValue() ;
        mOrderImbalanceTestCache.myTestMatchVolume          = listener.getMatchVolume();
        mOrderImbalanceTestCache.myTestSecurityStatusQual   = listener.getImbalanceState();  
        mOrderImbalanceTestCache.myTestInsideMatchPrice     = listener.getMatchPrice().getValue();
        mOrderImbalanceTestCache.myTestFarClearingPrice     = listener.getFarClearingPrice().getValue();
        mOrderImbalanceTestCache.myTestNearClearingPrice    = listener.getNearClearingPrice().getValue();
        mOrderImbalanceTestCache.myTestNoClearingPrice      = listener.getNoClearingPrice();
        mOrderImbalanceTestCache.myTestPriceVarInd          = listener.getPriceVarInd();
        mOrderImbalanceTestCache.myTestCrossType            = listener.getCrossType();
    }


    void onNoOrderImbalance (
            MamdaSubscription*           subscription,
            MamdaOrderImbalanceListener& listener,
            const MamaMsg&               msg,
            MamdaOrderImbalanceRecap&    imbalance,
            MamdaOrderImbalanceUpdate&   update)
    {     
        mOrderImbalanceTestCache.myTestHighIndicationPrice  = listener.getBuyVolume();
        mOrderImbalanceTestCache.myTestSellVolume           = listener.getSellVolume();
        mOrderImbalanceTestCache.myTestBuyVolume            = listener.getBuyVolume();
        mOrderImbalanceTestCache.myTestHighIndicationPrice  = listener.getHighIndicationPrice().getValue();
        mOrderImbalanceTestCache.myTestLowIndicationPrice   = listener.getLowIndicationPrice().getValue();
        mOrderImbalanceTestCache.myTestIndicationPrice      = listener.getImbalancePrice().getValue() ;
        mOrderImbalanceTestCache.myTestMatchVolume          = listener.getMatchVolume();
        mOrderImbalanceTestCache.myTestSecurityStatusQual   = listener.getImbalanceState(); 
        mOrderImbalanceTestCache.myTestInsideMatchPrice     = listener.getMatchPrice().getValue();
        mOrderImbalanceTestCache.myTestFarClearingPrice     = listener.getFarClearingPrice().getValue();
        mOrderImbalanceTestCache.myTestNearClearingPrice    = listener.getNearClearingPrice().getValue();
        mOrderImbalanceTestCache.myTestNoClearingPrice      = listener.getNoClearingPrice();
        mOrderImbalanceTestCache.myTestPriceVarInd          = listener.getPriceVarInd();
        mOrderImbalanceTestCache.myTestCrossType            = listener.getCrossType();
    }

    void onOrderImbalanceRecap (
            MamdaSubscription*           subscription,
            MamdaOrderImbalanceListener& listener,
            const MamaMsg&               msg,
            MamdaOrderImbalanceRecap&    imbalance)
    {               
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

    OrderImbalanceControlCache mOrderImbalanceControlCache;
    OrderImbalanceTestCache    mOrderImbalanceTestCache;
};

class MamdaOrderImbalanceFieldTests : public ::testing::Test
{
protected:
    MamdaOrderImbalanceFieldTests () {}
    virtual ~MamdaOrderImbalanceFieldTests () {}      

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
            MamdaOrderImbalanceFields::reset();
            MamdaOrderImbalanceFields::setDictionary (*mDictionary);
           
            mSubscription = new MamdaSubscription;
            if (!mSubscription)
            {
                FAIL() << "Failed to allocate MamdaSubscription\n";
                return;
            }

            mOrderImbalanceListener = new MamdaOrderImbalanceListener;
            if (!mOrderImbalanceListener)
            {
                FAIL() << "Failed to allocate MamdaOrderImbalanceListener\n";
                return;
            }

            mSubscription->addMsgListener(mOrderImbalanceListener);
            
            ticker = new OrderImbalanceTickerCB;      
            mOrderImbalanceListener->addHandler (ticker);   

            newMessage = NULL;
            newMessage = new MamaMsg();
            newMessage->create();
            
            addMamaHeaderFields(*newMessage,
                                MAMA_MSG_TYPE_UPDATE,
                                MAMA_MSG_STATUS_OK,
                                0);                   
            
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

        if (mOrderImbalanceListener)
        {
            delete mOrderImbalanceListener;
            mOrderImbalanceListener = NULL;
        }

        if (mDictionary)
        {
            delete mDictionary;
            mDictionary = NULL;
        }
        
        if (newMessage)
        {     
            delete newMessage;
            newMessage = NULL;
        } 
        
        if (ticker)
        {     
            delete ticker;
            ticker = NULL;
        }                
    }
    
    MamaDictionary*                   mDictionary;
    MamdaSubscription*                mSubscription;
    MamdaOrderImbalanceListener*      mOrderImbalanceListener;
    MamaMsg*                          newMessage;
    OrderImbalanceTickerCB*           ticker;   
};

//Each Test has one Field Set and the SecurityStatus is set to OrderImbNone. 
//These should trigger the noOrderImbalance CallBack.
TEST_F (MamdaOrderImbalanceFieldTests, BuyVolume_onNoOrdImbal_Test)
{       
    newMessage->addI64("wBuyVolume", 239, 1000);
    newMessage->addString("wSecStatusQual",1020,"OrderImbNone");   
    
    ticker->callMamdaOnMsg(mSubscription, *newMessage);        
    
    EXPECT_EQ (ticker->mOrderImbalanceControlCache.myBuyVolume, ticker->mOrderImbalanceTestCache.myTestBuyVolume);
     
   
}

TEST_F (MamdaOrderImbalanceFieldTests, SellVolume_onNoOrdImbal_Test)
{  
    newMessage->addI64("wSellVolume", 453, 2000);
    newMessage->addString("wSecStatusQual",1020,"OrderImbNone");
        
    ticker->callMamdaOnMsg(mSubscription, *newMessage);     
  
    EXPECT_EQ (ticker->mOrderImbalanceControlCache.mySellVolume, ticker->mOrderImbalanceTestCache.myTestSellVolume);   
}
                                                                                                                                                                   
TEST_F (MamdaOrderImbalanceFieldTests, HighIndication_onNoOrdImbal_Test)
{      
    newMessage->addPrice("wHighIndicationPrice",276,2000);
    newMessage->addString("wSecStatusQual",1020,"OrderImbNone");
        
    ticker->callMamdaOnMsg(mSubscription, *newMessage);     
  
    EXPECT_EQ (ticker->mOrderImbalanceControlCache.myHighIndicationPrice, ticker->mOrderImbalanceTestCache.myTestHighIndicationPrice);    
}

TEST_F (MamdaOrderImbalanceFieldTests, LowIndication_onNoOrdImbal_Test)
{      
    newMessage->addPrice("wLowIndicationPrice",325,3000);
    newMessage->addString("wSecStatusQual",1020,"OrderImbNone");

        
    ticker->callMamdaOnMsg(mSubscription, *newMessage);     
  
    EXPECT_EQ (ticker->mOrderImbalanceControlCache.myLowIndicationPrice, ticker->mOrderImbalanceTestCache.myTestLowIndicationPrice);
}

TEST_F (MamdaOrderImbalanceFieldTests, Indication_onNoOrdImbal_Test)
{     
    newMessage->addPrice("wIndicationPrice",978,4000);
    newMessage->addString("wSecStatusQual",1020,"OrderImbNone");
        
    ticker->callMamdaOnMsg(mSubscription, *newMessage);     
  
    EXPECT_EQ (ticker->mOrderImbalanceControlCache.myIndicationPrice, ticker->mOrderImbalanceTestCache.myTestIndicationPrice);  
}    

TEST_F (MamdaOrderImbalanceFieldTests, MatchVolume_onNoOrdImbal_Test)
{    
    newMessage->addI64("wMatchVolume",979,4000);
    newMessage->addString("wSecStatusQual",1020,"OrderImbNone");
        
    ticker->callMamdaOnMsg(mSubscription, *newMessage);     
  
    EXPECT_EQ (ticker->mOrderImbalanceControlCache.myMatchVolume, ticker->mOrderImbalanceTestCache.myTestMatchVolume);   
}                                                                                                                                                                  

TEST_F (MamdaOrderImbalanceFieldTests, SecurityStatus_onNoOrdImbal_Test)
{ 
    newMessage->addString("wSecStatusQual",1020,"OrderImbNone");    
        
    ticker->callMamdaOnMsg(mSubscription, *newMessage);     
  
    EXPECT_STREQ ("OrderImbNone", ticker->mOrderImbalanceTestCache.myTestSecurityStatusQual);
   
}                                                                                                                                                                  

TEST_F (MamdaOrderImbalanceFieldTests, InsideMatchPrice_onNoOrdImbal_Test)
{  
    newMessage->addPrice("wInsideMatchPrice",1023,5000); 
    newMessage->addString("wSecStatusQual",1020,"OrderImbNone");
        
    ticker->callMamdaOnMsg(mSubscription, *newMessage);     
  
    EXPECT_EQ (ticker->mOrderImbalanceControlCache.myInsideMatchPrice, ticker->mOrderImbalanceTestCache.myTestInsideMatchPrice);  
}           

TEST_F (MamdaOrderImbalanceFieldTests, FarClearingPrice_onNoOrdImbal_Test)
{  
    newMessage->addPrice("wFarClearingPrice",1024,6000);
    newMessage->addString("wSecStatusQual",1020,"OrderImbNone");
        
    ticker->callMamdaOnMsg(mSubscription, *newMessage);     
  
    EXPECT_EQ (ticker->mOrderImbalanceControlCache.myFarClearingPrice, ticker->mOrderImbalanceTestCache.myTestFarClearingPrice);
  
}   

TEST_F (MamdaOrderImbalanceFieldTests, NearClearingPrice_onNoOrdImbal_Test)
{      
    newMessage->addPrice("wNearClearingPrice",1025,6000);
    newMessage->addString("wSecStatusQual",1020,"OrderImbNone");
        
    ticker->callMamdaOnMsg(mSubscription, *newMessage);     
  
    EXPECT_EQ (ticker->mOrderImbalanceControlCache.myNearClearingPrice, ticker->mOrderImbalanceTestCache.myTestNearClearingPrice);
    
}           

TEST_F (MamdaOrderImbalanceFieldTests, NoClearingPrice_onNoOrdImbal_Test)
{      
    newMessage->addChar("wNoClearingPrice",1026,'c');
    newMessage->addString("wSecStatusQual",1020,"OrderImbNone");
        
    ticker->callMamdaOnMsg(mSubscription, *newMessage);     
  
    EXPECT_EQ (ticker->mOrderImbalanceControlCache.myNoClearingPrice, ticker->mOrderImbalanceTestCache.myTestNoClearingPrice);   
}  

TEST_F (MamdaOrderImbalanceFieldTests, PriceVarInd_onNoOrdImbal_Test)
{      
    newMessage->addChar("wPriceVarInd",1027,'a');
    newMessage->addString("wSecStatusQual",1020,"OrderImbNone");
        
    ticker->callMamdaOnMsg(mSubscription, *newMessage);     
  
    EXPECT_EQ (ticker->mOrderImbalanceControlCache.myPriceVarInd, ticker->mOrderImbalanceTestCache.myTestPriceVarInd);  
}   

TEST_F (MamdaOrderImbalanceFieldTests, CrossType_onNoOrdImbal_Test)
{  
    newMessage->addChar("wCrossType",1170,'b');
    newMessage->addString("wSecStatusQual",1020,"OrderImbNone");
        
    ticker->callMamdaOnMsg(mSubscription, *newMessage);     
  
    EXPECT_EQ (ticker->mOrderImbalanceControlCache.myCrossType, ticker->mOrderImbalanceTestCache.myTestCrossType);    
}  

//Each Test has one Field set and the SecurityStatus field is set to OrderImbBuy.
//These messages should trigger the onOrderImbalance CallBack.
TEST_F (MamdaOrderImbalanceFieldTests, BuyVolume_OnOrderImbal_Test)
{      
    newMessage->addI64("wBuyVolume", 239, 1000);
    newMessage->addString("wSecStatusQual",1020,"OrderImbBuy");
        
    ticker->callMamdaOnMsg(mSubscription, *newMessage);         
  
    EXPECT_EQ (ticker->mOrderImbalanceControlCache.myBuyVolume, ticker->mOrderImbalanceTestCache.myTestBuyVolume);   
}

TEST_F (MamdaOrderImbalanceFieldTests, SellVolume_OnOrderImbal_Test)
{  
    newMessage->addI64("wSellVolume", 453, 2000);
    newMessage->addString("wSecStatusQual",1020,"OrderImbBuy");
        
    ticker->callMamdaOnMsg(mSubscription, *newMessage);     
  
    EXPECT_EQ (ticker->mOrderImbalanceControlCache.mySellVolume, ticker->mOrderImbalanceTestCache.myTestSellVolume);   
}
                                                                                                                                                                    
TEST_F (MamdaOrderImbalanceFieldTests, HighIndication_OnOrderImbal_Test)
{     
    newMessage->addPrice("wHighIndicationPrice",276,2000);
    newMessage->addString("wSecStatusQual",1020,"OrderImbBuy");
        
    ticker->callMamdaOnMsg(mSubscription, *newMessage);     
  
    EXPECT_EQ (ticker->mOrderImbalanceControlCache.myHighIndicationPrice, ticker->mOrderImbalanceTestCache.myTestHighIndicationPrice);   
}

TEST_F (MamdaOrderImbalanceFieldTests, LowIndication_OnOrderImbal_Test)
{     
    newMessage->addPrice("wLowIndicationPrice",325,3000);
    newMessage->addString("wSecStatusQual",1020,"OrderImbBuy");

    ticker->callMamdaOnMsg(mSubscription, *newMessage);     
  
    EXPECT_EQ (ticker->mOrderImbalanceControlCache.myLowIndicationPrice, ticker->mOrderImbalanceTestCache.myTestLowIndicationPrice);   
}

TEST_F (MamdaOrderImbalanceFieldTests, Indication_OnOrderImbal_Test)
{     
    newMessage->addPrice("wIndicationPrice",978,4000);
    newMessage->addString("wSecStatusQual",1020,"OrderImbBuy");

    ticker->callMamdaOnMsg(mSubscription, *newMessage);     
  
    EXPECT_EQ (ticker->mOrderImbalanceControlCache.myIndicationPrice, ticker->mOrderImbalanceTestCache.myTestIndicationPrice); 
}    

TEST_F (MamdaOrderImbalanceFieldTests, MatchVolume_OnOrderImbal_Test)
{  
    newMessage->addI64("wMatchVolume",979,4000);
    newMessage->addString("wSecStatusQual",1020,"OrderImbBuy");

    ticker->callMamdaOnMsg(mSubscription, *newMessage);     
  
    EXPECT_EQ (ticker->mOrderImbalanceControlCache.myMatchVolume, ticker->mOrderImbalanceTestCache.myTestMatchVolume); 
}        

TEST_F (MamdaOrderImbalanceFieldTests, InsideMatchPrice_OnOrderImbal_Test)
{      
    newMessage->addPrice("wInsideMatchPrice",1023,5000); 
    newMessage->addString("wSecStatusQual",1020,"OrderImbBuy");

    ticker->callMamdaOnMsg(mSubscription, *newMessage);     
  
    EXPECT_EQ (ticker->mOrderImbalanceControlCache.myInsideMatchPrice, ticker->mOrderImbalanceTestCache.myTestInsideMatchPrice);    
}           

TEST_F (MamdaOrderImbalanceFieldTests, FarClearingPrice_OnOrderImbal_Test)
{      
    newMessage->addPrice("wFarClearingPrice",1024,6000);
    newMessage->addString("wSecStatusQual",1020,"OrderImbBuy");

    ticker->callMamdaOnMsg(mSubscription, *newMessage);     
  
    EXPECT_EQ (ticker->mOrderImbalanceControlCache.myFarClearingPrice, ticker->mOrderImbalanceTestCache.myTestFarClearingPrice); 
}   

TEST_F (MamdaOrderImbalanceFieldTests, NearClearingPrice_OnOrderImbal_Test)
{    
    newMessage->addPrice("wNearClearingPrice",1025,6000);
    newMessage->addString("wSecStatusQual",1020,"OrderImbBuy");

    ticker->callMamdaOnMsg(mSubscription, *newMessage);     
  
    EXPECT_EQ (ticker->mOrderImbalanceControlCache.myNearClearingPrice, ticker->mOrderImbalanceTestCache.myTestNearClearingPrice); 
}           

TEST_F (MamdaOrderImbalanceFieldTests, NoClearingPrice_OnOrderImbal_Test)
{     
    newMessage->addChar("wNoClearingPrice",1026,'c');
    newMessage->addString("wSecStatusQual",1020,"OrderImbBuy");
        
    ticker->callMamdaOnMsg(mSubscription, *newMessage);     
  
    EXPECT_EQ (ticker->mOrderImbalanceControlCache.myNoClearingPrice, ticker->mOrderImbalanceTestCache.myTestNoClearingPrice);     
}  

TEST_F (MamdaOrderImbalanceFieldTests, PriceVarInd_OnOrderImbal_Test)
{       
    newMessage->addChar("wPriceVarInd",1027,'a');
    newMessage->addString("wSecStatusQual",1020,"OrderImbBuy");
        
    ticker->callMamdaOnMsg(mSubscription, *newMessage);     
  
    EXPECT_EQ (ticker->mOrderImbalanceControlCache.myPriceVarInd, ticker->mOrderImbalanceTestCache.myTestPriceVarInd);  
}   

TEST_F (MamdaOrderImbalanceFieldTests, CrossType_OnOrderImbal_Test)
{ 
    newMessage->addChar("wCrossType",1170,'b');
    newMessage->addString("wSecStatusQual",1020,"OrderImbBuy");
        
    ticker->callMamdaOnMsg(mSubscription, *newMessage);     
  
    EXPECT_EQ (ticker->mOrderImbalanceControlCache.myCrossType, ticker->mOrderImbalanceTestCache.myTestCrossType);    
   
}

