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

class MamdaTradeListenerTradeIdTests : public ::testing::Test
{     
  
    class TradeTickerCB : public MamdaTradeHandler   
                        , public MamdaErrorListener
                        , public MamdaQualityListener
    { 
        public:
        string                  myTradeId;  
        MamdaFieldState         myTradeIdFieldState;
        string                  myOrigTradeId;
        MamdaFieldState         myOrigTradeIdFieldState;
        string                  myCorrTradeId; 
        MamdaFieldState         myCorrTradeIdFieldState;
        
            
        TradeTickerCB () {}
        virtual ~TradeTickerCB () {}            

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

        void onTradeRecap (
            MamdaSubscription*      subscription,
            MamdaTradeListener&     listener,
            const MamaMsg&          msg,
            const MamdaTradeRecap&  recap)
        {         
        }

        void onTradeReport (
            MamdaSubscription*      subscription,
            MamdaTradeListener&     listener,
            const MamaMsg&          msg,
            const MamdaTradeReport& event,
            const MamdaTradeRecap&  recap)
        {       
            myTradeId = event.getTradeId(); 
            myTradeIdFieldState = event.getTradeIdFieldState();
            cout << " msg:" <<  msg.toString() << "\n";
            cout << "myTradeId: " << myTradeId << "\n";       
        }

        void onTradeGap (
            MamdaSubscription*              subscription,
            MamdaTradeListener&             listener,
            const MamaMsg&                  msg,
            const MamdaTradeGap&            event,
            const MamdaTradeRecap&          recap)
        {           
        }

        void onTradeCancelOrError (
            MamdaSubscription*              subscription,
            MamdaTradeListener&             listener,
            const MamaMsg&                  msg,
            const MamdaTradeCancelOrError&  event,
            const MamdaTradeRecap&          recap)
        {  
             myOrigTradeId = event.getOrigTradeId(); 
             myOrigTradeIdFieldState = event.getOrigTradeIdFieldState();
             cout << "msg:" << msg.toString() << "\n";
             cout << "myOrigTradeId: " << myOrigTradeId << "\n";
        }

        void onTradeCorrection (
            MamdaSubscription*              subscription,
            MamdaTradeListener&             listener,
            const MamaMsg&                  msg,
            const MamdaTradeCorrection&     event,
            const MamdaTradeRecap&          recap)
        {  
             myCorrTradeId = event.getCorrTradeId();  
             myCorrTradeIdFieldState = event.getCorrTradeIdFieldState();
             cout << "msg: " << msg.toString() << "\n";
             cout << "myCorrTradeId: " << myCorrTradeId << "\n";
        }

        void onTradeClosing (
            MamdaSubscription*              subscription,
            MamdaTradeListener&             listener,
            const MamaMsg&                  msg,
            const MamdaTradeClosing&        event,
            const MamdaTradeRecap&          recap)
        { 
        }

        void onTradeOutOfSequence (
            MamdaSubscription*              subscription,
            MamdaTradeListener&             listener,
            const MamaMsg&                  msg,
            const MamdaTradeOutOfSequence&  event,
            const MamdaTradeRecap&          recap) 
        {  
        }

        void onTradePossiblyDuplicate (
            MamdaSubscription*                  subscription,
            MamdaTradeListener&                 listener,
            const MamaMsg&                      msg,
            const MamdaTradePossiblyDuplicate&  event,
            const MamdaTradeRecap&              recap) 
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
    };    
        
protected:
    MamdaTradeListenerTradeIdTests () {}
    virtual ~MamdaTradeListenerTradeIdTests () {}  
  
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
            
            ticker = new TradeTickerCB;      
            mTradeListener->addHandler (ticker); 
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
        
        if (mSubscription)
        {     
            delete mSubscription;
            mSubscription = NULL;
        }    
        if (mTradeListener)
        {     
            delete mTradeListener;
            mTradeListener = NULL;
        } 
        
        if (ticker)
        {     
            delete ticker;
            ticker = NULL;
        }  
    } 
    
    MamaDictionary*          mDictionary;
    MamdaSubscription*       mSubscription;
    MamdaTradeListener*      mTradeListener;
    MamaMsg*                 mMsg;
    TradeTickerCB*           ticker;
};

TEST_F (MamdaTradeListenerTradeIdTests,testTradeId)
{        
    mMsg = new MamaMsg();
    mMsg->create();  
    addMamaHeaderFields(*mMsg,
                            MAMA_MSG_TYPE_TRADE,
                            MAMA_MSG_STATUS_OK,
                            0); 
   
    mMsg->addString("wTradeId",477,"A");
    ticker->callMamdaOnMsg(mSubscription, *mMsg); 
    EXPECT_STREQ("A",ticker->myTradeId.c_str());
    EXPECT_EQ(MODIFIED,ticker->myTradeIdFieldState);  
    
    delete mMsg;
    mMsg = NULL;    
}

TEST_F (MamdaTradeListenerTradeIdTests,testOrigTradeId)
{
    mMsg = new MamaMsg();
    mMsg->create();  
    addMamaHeaderFields(*mMsg,
                            MAMA_MSG_TYPE_CANCEL,
                            MAMA_MSG_STATUS_OK,
                            0); 
    mMsg->addString("wOrigTradeId",1096,"B");
    ticker->callMamdaOnMsg(mSubscription, *mMsg); 
    EXPECT_STREQ("B",ticker->myOrigTradeId.c_str()); 
    EXPECT_EQ(MODIFIED, ticker->myOrigTradeIdFieldState);
    
    delete mMsg;
    mMsg = NULL;    
    
}

TEST_F (MamdaTradeListenerTradeIdTests,testCorrTradeId)
{
    mMsg = new MamaMsg();
    mMsg->create();  
    addMamaHeaderFields(*mMsg,
                            MAMA_MSG_TYPE_CORRECTION,
                            MAMA_MSG_STATUS_OK,
                            0); 
    mMsg->addString("wCorrTradeId",4060,"C");
    ticker->callMamdaOnMsg(mSubscription, *mMsg); 
    EXPECT_STREQ("C",ticker->myCorrTradeId.c_str());  
    EXPECT_EQ(MODIFIED, ticker->myCorrTradeIdFieldState);  
    
    delete mMsg;
    mMsg = NULL;    
}







