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

#include <mama/MamaDictionary.h>
#include <mama/mamacpp.h>
#include <mama/MamaMsg.h>

#include <mamda/MamdaTradeFields.h>
#include <mamda/MamdaCommonFields.h>
#include <mamda/MamdaSubscription.h>
#include <mamda/MamdaConfig.h>
#include <mamda/MamdaTradeListener.h>
#include <mamda/MamdaTradeHandler.h>
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

class MamdaTradeListenerShortSaleTests : public ::testing::Test
{
protected:
    MamdaTradeListenerShortSaleTests () {}
    virtual ~MamdaTradeListenerShortSaleTests () {}  

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
            MamdaTradeFields::reset();
            MamdaTradeFields::setDictionary (*mDictionary);
                      
            mSubscription = new MamdaSubscription;
            if (!mSubscription)
            {
                FAIL() << "Failed to allocate MamdaSubscription\n";
                return;
            }
            mTradeListener = new MamdaTradeListener; 
            if (!mTradeListener)
            {
                FAIL() << "Failed to allocate MamdaTradeListener\n";
                return;
            }
            mSubscription->addMsgListener(mTradeListener);
          
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
        if (mDictionary)
        {     
            delete mDictionary;
            mDictionary = NULL;
        }  
        if (mTradeListener)
        {
            delete mTradeListener;
            mTradeListener = NULL;
        }        
    }

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
  
    MamaDictionary*         mDictionary;
    MamdaTradeListener*     mTradeListener;
    MamdaSubscription*      mSubscription;
};

TEST_F(MamdaTradeListenerShortSaleTests, tradelistenerRecapFuncTest)
{
    MamaMsg* mMsg = NULL;
    mMsg = new MamaMsg;
    mMsg->create();
    addMamaHeaderFields(*mMsg,
                        MAMA_MSG_TYPE_INITIAL,
                        MAMA_MSG_STATUS_OK,
                        1);
                        
    mMsg->addChar("wShortSaleCircuitBreaker",5248,'A');    
    callMamdaOnMsg(mSubscription,*mMsg);  
    EXPECT_TRUE('A' == mTradeListener->getShortSaleCircuitBreaker());
    EXPECT_EQ(MODIFIED,mTradeListener->getShortSaleCircuitBreakerFieldState());   
}

TEST_F(MamdaTradeListenerShortSaleTests, tradelistenerTradeFuncTest)
{
    MamaMsg* mMsg = NULL;
    mMsg = new MamaMsg;
    mMsg->create();
    addMamaHeaderFields(*mMsg,
                        MAMA_MSG_TYPE_TRADE,
                        MAMA_MSG_STATUS_OK,
                        1);                        
                        
    mMsg->addChar("wShortSaleCircuitBreaker",5248,'A');    
    callMamdaOnMsg(mSubscription,*mMsg);  
    EXPECT_TRUE('A' == mTradeListener->getShortSaleCircuitBreaker());
    EXPECT_EQ(MODIFIED,mTradeListener->getShortSaleCircuitBreakerFieldState());          
   
}

TEST_F(MamdaTradeListenerShortSaleTests, tradelistenerTradeCorrectionFuncTest)
{
    MamaMsg* mMsg = NULL;
    mMsg = new MamaMsg;
    mMsg->create();
    addMamaHeaderFields(*mMsg,
                        MAMA_MSG_TYPE_CORRECTION,
                        MAMA_MSG_STATUS_OK,
                        1);
                        
    mMsg->addChar("wCorrShortSaleCircuitBreaker",5250,'A');    
    callMamdaOnMsg(mSubscription,*mMsg);  
    EXPECT_TRUE('A' == mTradeListener->getCorrShortSaleCircuitBreaker());
    EXPECT_EQ(MODIFIED,mTradeListener->getCorrShortSaleCircuitBreakerFieldState());           
    
}

TEST_F(MamdaTradeListenerShortSaleTests, tradelistenerTradeCanelFuncTest)
{
    MamaMsg* mMsg = NULL;
    mMsg = new MamaMsg;
    mMsg->create();
    addMamaHeaderFields(*mMsg,
                        MAMA_MSG_TYPE_CORRECTION,
                        MAMA_MSG_STATUS_OK,
                        1);
                        
    mMsg->addChar("wOrigShortSaleCircuitBreaker",5249,'A');    
    callMamdaOnMsg(mSubscription,*mMsg);  
    EXPECT_TRUE('A' == mTradeListener->getOrigShortSaleCircuitBreaker());
    EXPECT_EQ(MODIFIED,mTradeListener->getOrigShortSaleCircuitBreakerFieldState());           
    
}


