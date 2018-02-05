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
        ofstream Fieldtest1; 
        //Fieldtest1.open ("ControlCallBackMsgs/TradeFields/TradeFieldTest.out");
        Fieldtest1.open ("newMsgs/newTradeFieldTest.out");
        if(!Fieldtest1)
        {
            cout << "Cannot open file.\n";
            exit(1);
        }

        Fieldtest1  << recap.getSymbol() 
                    << recap.getSrcTime().getAsString()  
                    << event.getTradePartId()
                    //<< recap.getF64("wTotalVolume",475,113561846)    
                    << recap.getTotalValue()
                    << recap.getVwap() 
                    << recap.getTradeCount()
                    << event.getIsIrregular()
                    << event.getTradeQual()  
                    //<< recap.getI32("wTradeSeqNum",483, 1)
                    << event.getTradeQualNative()
                    << listener.getTradePrice().getValue() 
                    << recap.getNetChange().getValue()   
                    << recap.getPctChange()
                    << recap.getTradeUnits() 
                    << recap.getTradeDirection()
                    << recap.getLastTime().getAsString()
                    << event.getTradeVolume()  
                    << recap.getActivityTime().getAsString() 
                    //<< recap.MamaMsg::getDateTime("wSendTime",?, val)    
                    << recap.getLineTime().getAsString()                         
                    << listener.getPubId()    
                    << recap.getEventSeqNum()   
                    << event.getEventTime().getAsString()      
                    << recap.getIrregPartId()     
                    << recap.getIrregVolume()    
                    << recap.getIrregTime().getAsString()      
                    << recap.getTradeDate().getAsString()   
                    << recap.getOffExAccVolume()    
                    << recap.getOnExAccVolume()   
                    << recap.getOpenPrice().getValue()         
                    << recap.getHighPrice().getValue()  
                    << recap.getLowPrice().getValue()    
                    << recap.getClosePrice().getValue()    
                    << recap.getPrevClosePrice().getValue()    
                    << recap.getAdjPrevClosePrice().getValue()    
                    << recap.getPrevCloseDate().getAsString()    
                    << recap.getBlockCount()   
                    << recap.getBlockVolume()  
                    << recap.getOffExVwap()    
                    << recap.getOnExVwap()    
                    << recap.getOffExTotalValue()    
                    << recap.getOnExTotalValue()    
                    << recap.getStdDev()    
                    << recap.getStdDevSum()    
                    << recap.getStdDevSumSquares()   
                    << recap.getLastSeqNum()   
                    << recap.getHighSeqNum()    
                    << recap.getLowSeqNum()    
                    << recap.getTotalVolumeSeqNum()    
                    << recap.getCurrencyCode()      
                    << recap.getSettlePrice().getValue()     
                    << recap.getSettleDate().getAsString()    
                    << listener.getOrderId()    
                    << listener.getUniqueId()     
                    << listener.getTradeAction()   
                    << listener.getTradeExecVenue() 
                    << listener.getTradeSellersSaleDays()
                    << listener.getOrigStopStock() 
                    //<< recap.getI64("wOrigSeqNum",423, 1) causes this test to abort    
                    << listener.getOrigPrice().getValue()   
                    << listener.getOrigVolume()    
                    << listener.getOrigPartId()    
                    << listener.getOrigQual()    
                    << listener.getOrigQualNative()    
                    //<< recap.getI64("wOrigSaleDays",422, 1) causes this test to abort    
                    << listener.getOrigStopStock()  
                    << listener.getCorrPrice().getValue()    
                    << listener.getCorrPartId()    
                    << listener.getCorrQual()    
                    << listener.getCorrQualNative()    
                    //<< recap.getI64("wCorrSaleDays",250, 1)causes this test to abort      
                    << listener.getCorrStopStock()    
                    << listener.getOffExchangeTradePrice().getValue()    
                    << listener.getOnExchangeTradePrice().getValue()   
                    << flush; 
  
        Fieldtest1.close();     
        
        ofstream  Fieldtest2;  
        //Fieldtest2.open ("ControlCallBackMsgs/TradeFieldStates/TradeFieldStateTest.out");
        Fieldtest2.open ("newMsgs/newTradeFieldStatesTest.out");  
        if(!Fieldtest2)
        {
            cout << "Cannot open file.\n";
            exit(1);
        }
            
        Fieldtest2  << recap.getSymbolFieldState()          
                    << recap.getSrcTimeFieldState()   
                    << event.getTradePartIdFieldState()
                    //<< recap.getF64("wTotalVolume",475,113561846)    
                    << recap.getTotalValueFieldState() 
                    << recap.getVwapFieldState()
                    << recap.getTradeCountFieldState()
                    << event.getIsIrregularFieldState() 
                    << event.getTradeQualFieldState()  
                    //<< recap.getI32("wTradeSeqNum",483, 1)
                    << event.getTradeQualNativeFieldState()
                    << listener.getTradePriceFieldState()
                    << recap.getNetChangeFieldState()  
                    << recap.getPctChangeFieldState()
                    << recap.getTradeUnitsFieldState() 
                    << recap.getTradeDirectionFieldState()
                    << recap.getLastTimeFieldState()
                    << event.getTradeVolumeFieldState()  
                    << recap.getActivityTimeFieldState() 
                    //<< recap.MamaMsg::getDateTime("wSendTime",?, val)    
                    << recap.getLineTimeFieldState()                       
                    << listener.getPubIdFieldState()    
                    << recap.getEventSeqNumFieldState()   
                    << event.getEventTimeFieldState()      
                    << recap.getIrregPartIdFieldState()     
                    << recap.getIrregVolumeFieldState()    
                    << recap.getIrregTimeFieldState()      
                    << recap.getTradeDateFieldState()  
                    << recap.getOffExAccVolumeFieldState()    
                    << recap.getOnExAccVolumeFieldState()   
                    << recap.getOpenPriceFieldState()         
                    << recap.getHighPriceFieldState() 
                    << recap.getLowPriceFieldState()  
                    << recap.getClosePriceFieldState()  
                    << recap.getPrevClosePriceFieldState()    
                    << recap.getAdjPrevClosePriceFieldState()    
                    << recap.getPrevCloseDateFieldState()    
                    << recap.getBlockCountFieldState()   
                    << recap.getBlockVolumeFieldState()  
                    << recap.getOffExVwapFieldState()    
                    << recap.getOnExVwapFieldState()    
                    << recap.getOffExTotalValueFieldState()    
                    << recap.getOnExTotalValueFieldState()    
                    << recap.getStdDevFieldState()    
                    << recap.getStdDevSumFieldState()    
                    << recap.getStdDevSumSquaresFieldState()   
                    << recap.getLastSeqNumFieldState()   
                    << recap.getHighSeqNumFieldState()    
                    << recap.getLowSeqNumFieldState()    
                    << recap.getTotalVolumeSeqNumFieldState()    
                    << recap.getCurrencyCodeFieldState()     
                    << recap.getSettlePriceFieldState()    
                    << recap.getSettleDateFieldState()    
                    << listener.getOrderIdFieldState()    
                    << listener.getUniqueIdFieldState()     
                    << listener.getTradeActionFieldState()   
                    << listener.getTradeExecVenueFieldState() 
                    << listener.getTradeSellersSaleDaysFieldState()
                    << listener.getOrigStopStockFieldState() 
                    //<< recap.getI64("wOrigSeqNum",423, 1) causes this test to abort    
                    << listener.getOrigPriceFieldState()   
                    << listener.getOrigVolumeFieldState()    
                    << listener.getOrigPartIdFieldState()    
                    << listener.getOrigQualFieldState()    
                    << listener.getOrigQualNativeFieldState()    
                    //<< recap.getI64("wOrigSaleDays",422, 1) causes this test to abort    
                    << listener.getOrigStopStockFieldState()  
                    << listener.getCorrPriceFieldState()   
                    << listener.getCorrPartIdFieldState()    
                    << listener.getCorrQualFieldState()    
                    << listener.getCorrQualNativeFieldState()    
                    //<< recap.getI64("wCorrSaleDays",250, 1)causes this test to abort      
                    << listener.getCorrStopStockFieldState()    
                    << listener.getOffExchangeTradePriceFieldState()   
                    << listener.getOnExchangeTradePriceFieldState()   
                    << flush;  
   
        Fieldtest2.close();            
        return ;   
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
    }

    void onTradeCorrection (
        MamdaSubscription*              subscription,
        MamdaTradeListener&             listener,
        const MamaMsg&                  msg,
        const MamdaTradeCorrection&     event,
        const MamdaTradeRecap&          recap)
    {        
      
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

class MamdaTradeFieldTests : public ::testing::Test
{
protected:
    MamdaTradeFieldTests () {}
    virtual ~MamdaTradeFieldTests () {}  
    
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
        if (mDictionary)
        {     
            delete mDictionary;
            mDictionary = NULL;
        }  
    }
    
    MamaDictionary*         mDictionary;
    MamdaSubscription*      mSubscription;
    MamdaTradeListener*     mTradeListener;    
};

// This test compares the output fields from a new msg with 
// a control msg which is known to give correct output.  
TEST_F (MamdaTradeFieldTests,FieldTests)
{  
    TradeTickerCB* ticker = new TradeTickerCB;      
    mTradeListener->addHandler (ticker);   

    MamaMsg* newMessage = NULL;
    newMessage = new MamaMsg();
    newMessage->create();     
  
    MamaDateTime* DateTime=NULL;
    DateTime = new MamaDateTime("2010/02/02",NULL);     
    
    SetTradeFields(*newMessage,*DateTime);          
    ticker->callMamdaOnMsg(mSubscription, *newMessage); 
    
    bool fail;
    printf("\n Trade processed. Comparing output \n");
    fail = CompareTest("newMsgs/newTradeFieldTest.out","ControlCallBackMsgs/TradeFields/TradeFieldTest.out",
                       "newCallBackErrorMsgs/TradeFieldErrorMsgs/newTradeFieldTest.errors");      
    if (fail)
        cout << "****** TRADEFIELDS TEST FAILED (TradeFields) ****** " <<
                "See newCallBackErrorMsgs/TradeFieldErrorMsgs/newTradeFieldTest.errors for details." << "\n";
    else
        cout << "****** TRADEFIELDS TEST PASSED (TradeFields) ****** "<< "\n";  
    
    EXPECT_EQ(0,fail);
    
    delete ticker;
    ticker = NULL;  

    delete newMessage;
    newMessage = NULL;
    
}


// This test checks if the fieldstates have been 
// modified in correspondence with the receit of  
// new field updates and compares it with a control
// that is known to be correct.
TEST_F (MamdaTradeFieldTests,FieldStateTests)
{  
    TradeTickerCB* ticker = new TradeTickerCB;      
    mTradeListener->addHandler (ticker);   

    MamaMsg* newMessage = NULL;
    newMessage = new MamaMsg();
    newMessage->create();     
  
    MamaDateTime* DateTime=NULL;
    DateTime = new MamaDateTime("2010/02/02",NULL);     
    
    SetTradeFields(*newMessage,*DateTime);          
    ticker->callMamdaOnMsg(mSubscription, *newMessage);  
   
    bool fail;
    printf("\n Trade processed. Comparing output \n");
    fail = CompareTest("newMsgs/newTradeFieldStatesTest.out","ControlCallBackMsgs/TradeFieldStates/TradeFieldStateTest.out",
                       "newCallBackErrorMsgs/TradeFieldStateErrorMsgs/newTradeFieldStateTest.errors");     
    
    if (fail)
        cout << "****** TRADEFIELDSTATES TEST FAILED (TradeFieldStates) ******" <<
                "See newCallBackErrorMsgs/TradeFieldStateErrorMsgs/newTradeFieldStateTest.errors for details." << "\n";
    else
        cout << "****** TRADEFIELDSTATES TEST PASSED (TradeFieldStates) ****** "<< "\n";
    
    EXPECT_EQ(0,fail);
    
    delete ticker;
    ticker = NULL;      

    delete newMessage;
    newMessage = NULL;    
}

