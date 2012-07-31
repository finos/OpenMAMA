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
#include <mamda/MamdaFieldState.h>
#include <mamda/MamdaTradeSide.h>
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

class MamdaTradeSideTests : public ::testing::Test
{
public: 
    MamaDictionary*     mDictionary;
    MamdaSubscription*  mSubscription;
    MamdaTradeListener* mTradeListener;   
    
protected:
    MamdaTradeSideTests () {}
    virtual ~MamdaTradeSideTests () {}  

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
        if (mSubscription)
        {
            delete mSubscription;
            mSubscription = NULL;
        }      
    }  

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
}; 

TEST_F (MamdaTradeSideTests, NoSide)
{
     MamaMsg* mMsg = createBlankMsg(MAMA_MSG_TYPE_TRADE,
                   MAMA_MSG_STATUS_OK,
                   1);
   
    callMamdaOnMsg(mSubscription,*mMsg);    
    EXPECT_STREQ("",mTradeListener->getSide());  
    EXPECT_EQ(NOT_INITIALISED,mTradeListener->getSideFieldState()); 
    
    delete mMsg;
    mMsg = NULL;    
}

TEST_F (MamdaTradeSideTests, TradeSide_Buy)
{
    MamaMsg* mMsg = createBlankMsg(MAMA_MSG_TYPE_TRADE,
                   MAMA_MSG_STATUS_OK,
                   1);
    mMsg->addString("wTradeSide",1161,"1");    
    callMamdaOnMsg(mSubscription,*mMsg);    
    EXPECT_EQ(TRADE_SIDE_BUY,mamdaTradeSideFromString(mTradeListener->getSide()));  
    EXPECT_EQ(MODIFIED,mTradeListener->getSideFieldState());   
    
    delete mMsg;
    mMsg = NULL;        
}

TEST_F (MamdaTradeSideTests, TradeSide_int1)
{     
    MamaMsg* mMsg = createBlankMsg(MAMA_MSG_TYPE_TRADE,
                   MAMA_MSG_STATUS_OK,
                   1);
    mMsg->addU32("wTradeSide",1161,1);   
    callMamdaOnMsg(mSubscription,*mMsg);    
    EXPECT_STREQ("Buy",mTradeListener->getSide());  
    EXPECT_EQ(MODIFIED,mTradeListener->getSideFieldState());   
    
    delete mMsg;
    mMsg = NULL;     
}

TEST_F (MamdaTradeSideTests, AggressorSide_S)
{
    MamaMsg* mMsg = createBlankMsg(MAMA_MSG_TYPE_TRADE,
                   MAMA_MSG_STATUS_OK,
                   1);        
    mMsg->addChar("wAggressorSide",4486,'S'); 
    callMamdaOnMsg(mSubscription,*mMsg); 
    
    EXPECT_STREQ("S",mTradeListener->getSide());  
    EXPECT_EQ(MODIFIED,mTradeListener->getSideFieldState());     
    
    delete mMsg;
    mMsg = NULL;    
}

TEST_F (MamdaTradeSideTests, AggressorSide_twoMsgs)
{
    
    MamaMsg* mMsg = createBlankMsg(MAMA_MSG_TYPE_TRADE,
                   MAMA_MSG_STATUS_OK,
                   1);        
    mMsg->addChar("wAggressorSide",4486,'S'); 
    callMamdaOnMsg(mSubscription,*mMsg); 
    
    EXPECT_STREQ("S",mTradeListener->getSide());  
    cout << "mTradeListener->getSide():" << mTradeListener->getSide() << "\n";
    EXPECT_EQ(MODIFIED,mTradeListener->getSideFieldState());     
    
    mMsg->updateChar("wAggressorSide",4486,'B');
    callMamdaOnMsg(mSubscription,*mMsg); 
    
    EXPECT_STREQ("B",mTradeListener->getSide());
    cout << "mTradeListener->getSide():" << mTradeListener->getSide() << "\n";
    EXPECT_EQ(MODIFIED,mTradeListener->getSideFieldState());  
    
    delete mMsg;
    mMsg = NULL;     
}


