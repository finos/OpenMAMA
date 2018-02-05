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

#include <mamda/MamdaSecStatusFields.h>
#include <mamda/MamdaCommonFields.h>
#include <mamda/MamdaSubscription.h>
#include <mamda/MamdaConfig.h>
#include <mamda/MamdaSecStatusListener.h>
#include <mamda/MamdaSecStatusHandler.h>
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

class MamdaSecStatusListenerShortSaleTests : public ::testing::Test
{
protected:
    MamdaSecStatusListenerShortSaleTests () {}
    virtual ~MamdaSecStatusListenerShortSaleTests () {}  

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
            MamdaSecStatusFields::reset();
            MamdaSecStatusFields::setDictionary (*mDictionary);
                      
            mSubscription = new MamdaSubscription;
            if (!mSubscription)
            {
                FAIL() << "Failed to allocate MamdaSubscription\n";
                return;
            }
            mSecStatusListener = new MamdaSecStatusListener; 
            if (!mSecStatusListener)
            {
                FAIL() << "Failed to allocate MamdaTradeListener\n";
                return;
            }
            mSubscription->addMsgListener(mSecStatusListener);
          
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
        if (mSecStatusListener)
        {
            delete mSecStatusListener;
            mSecStatusListener = NULL;
        }        
    }        

    MamaDictionary*         mDictionary;
    MamdaSecStatusListener* mSecStatusListener;
    MamdaSubscription*      mSubscription;
};

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


TEST_F(MamdaSecStatusListenerShortSaleTests, TestShortSaleCircuitBreaker)
{
    MamaMsg* mMsg = NULL;
    mMsg = new MamaMsg;
    mMsg->create();
    addMamaHeaderFields(*mMsg,
                        MAMA_MSG_TYPE_SEC_STATUS,
                        MAMA_MSG_STATUS_OK,
                        1);
                        
    callMamdaOnMsg(mSubscription,*mMsg);  
    EXPECT_TRUE(' ' == mSecStatusListener->getShortSaleCircuitBreaker());
    EXPECT_EQ(NOT_INITIALISED,mSecStatusListener->getShortSaleCircuitBreakerFieldState());   
                        
    mMsg->addChar("wShortSaleCircuitBreaker",5248,'A');    
    callMamdaOnMsg(mSubscription,*mMsg);  
    EXPECT_TRUE('A' == mSecStatusListener->getShortSaleCircuitBreaker());
    EXPECT_EQ(MODIFIED,mSecStatusListener->getShortSaleCircuitBreakerFieldState());          
    
    mMsg->updateChar("wShortSaleCircuitBreaker",5248,'D');    
    callMamdaOnMsg(mSubscription,*mMsg);  
    EXPECT_TRUE('D' == mSecStatusListener->getShortSaleCircuitBreaker());
    EXPECT_EQ(MODIFIED,mSecStatusListener->getShortSaleCircuitBreakerFieldState());  
}



