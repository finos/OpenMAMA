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

const int ITERATIONS = 1000000;

using std::string;

using namespace std;
using namespace Wombat;

class TradeTicker : public MamdaTradeHandler
{
public:
    TradeTicker () {}
    virtual ~TradeTicker () {}

    void callMamdaOnMsg(MamdaSubscription* sub, MamaMsg& msg)
    {
        try
        {
            vector<MamdaMsgListener*>& msgListeners = sub->getMsgListeners();
            unsigned long size = msgListeners.size();
            for (unsigned long i = 0; i < size; i++)
            {
                MamdaMsgListener* listener = msgListeners[i];
                listener->onMsg (sub, msg, (short)msg.getType());
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
//        cout << endl << "Trade Recap " << endl;
    }

    void onTradeReport (
        MamdaSubscription*      subscription,
        MamdaTradeListener&     listener,
        const MamaMsg&          msg,
        const MamdaTradeReport& event,
        const MamdaTradeRecap&  recap)
    {
//        cout << endl <<  "Trade Report " << endl;
    }

    void onTradeGap (
        MamdaSubscription*              subscription,
        MamdaTradeListener&             listener,
        const MamaMsg&                  msg,
        const MamdaTradeGap&            event,
        const MamdaTradeRecap&          recap)
    {
//        cout << endl <<  "Trade Gap " << endl;	
    }

    void onTradeCancelOrError (
        MamdaSubscription*              subscription,
        MamdaTradeListener&             listener,
        const MamaMsg&                  msg,
        const MamdaTradeCancelOrError&  event,
        const MamdaTradeRecap&          recap)
    {
//        cout << endl <<  "Trade Cancel Or Error " << endl;
    }

    void onTradeCorrection (
        MamdaSubscription*              subscription,
        MamdaTradeListener&             listener,
        const MamaMsg&                  msg,
        const MamdaTradeCorrection&     event,
        const MamdaTradeRecap&          recap)
    {
//        cout << endl <<  "Trade Correction " << endl;
    }

    void onTradeClosing (
        MamdaSubscription*              subscription,
        MamdaTradeListener&             listener,
        const MamaMsg&                  msg,
        const MamdaTradeClosing&        event,
        const MamdaTradeRecap&          recap)
    {
//        cout << endl <<  "Trade Closing " << endl;
    }

    void onTradeOutOfSequence (
        MamdaSubscription*              subscription,
        MamdaTradeListener&             listener,
        const MamaMsg&                  msg,
        const MamdaTradeOutOfSequence&  event,
        const MamdaTradeRecap&          recap)
    {
//        cout << endl <<  "Trade Out Of Sequence " << endl;
    }

    void onTradePossiblyDuplicate (
        MamdaSubscription*                  subscription,
        MamdaTradeListener&                 listener,
        const MamaMsg&                      msg,
        const MamdaTradePossiblyDuplicate&  event,
        const MamdaTradeRecap&              recap)
    {
//        cout << endl <<  "Trade Duplicate " << endl;
    }
};

class MamdaTradeListenerPerfTest : public ::testing::Test
{
protected:
    MamdaTradeListenerPerfTest () {}
    virtual ~MamdaTradeListenerPerfTest () {}

    virtual void SetUp()
    {
        try
        {
            mamaBridge bridge;
            bridge = Mama::loadBridge("wmw");
            Mama::open();
            //mama_enableLogging (stderr, MAMA_LOG_LEVEL_FINEST);
           
            mDictionary = new MamaDictionary;
            mDictionary->populateFromFile("dictionary.txt");
            MamdaCommonFields::reset();
            MamdaCommonFields::setDictionary (*mDictionary);
            MamdaTradeFields::reset();
            MamdaTradeFields::setDictionary (*mDictionary);

            mSubscription = new MamdaSubscription;
            mSubscription->setType(MAMA_SUBSC_TYPE_BOOK);
            mSubscription->setMdDataType(MAMA_MD_DATA_TYPE_ORDER_BOOK);
        }
        catch (MamaStatus status)
        {
            FAIL() << "Failed to setup Mamda: "
                   << status.toString() << endl;
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

        if (mDictionary)
        {
            delete mDictionary;
            mDictionary = NULL;
        }
    }

    MamaDictionary*         mDictionary;
    MamdaSubscription*      mSubscription;
};

TEST_F(MamdaTradeListenerPerfTest, ProcessRecapMsg)
{
    MamdaTradeListener *mTradeListener = new MamdaTradeListener;
    mSubscription->addMsgListener (mTradeListener);
    TradeTicker *ticker = new TradeTicker;
    mTradeListener->addHandler(ticker);

    MamaMsg* recapMsg = new MamaMsg();
    recapMsg->create();
    SetTradeRecapFields(*recapMsg);

    START_RECORDING_CPU(ITERATIONS);
        ticker->callMamdaOnMsg(mSubscription, *recapMsg);
    STOP_RECORDING_CPU ();

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();

    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates recap msg and sends this msg to TradeListener                      "
               "\n repetitively (%d), not processing entries                                            "
               "\n ********************************************************************************** \n",
               test_info->name(), ITERATIONS);
    }

    delete mTradeListener;
    mTradeListener = NULL;
    delete ticker;
    ticker = NULL;
    delete recapMsg;
    recapMsg = NULL;
}

TEST_F(MamdaTradeListenerPerfTest, ProcessUpdateMsg)
{
    MamdaTradeListener *mTradeListener = new MamdaTradeListener;
    mSubscription->addMsgListener (mTradeListener);
    TradeTicker *ticker = new TradeTicker;
    mTradeListener->addHandler(ticker);

    MamaMsg* msg = new MamaMsg();
    msg->create();

    int seqNum = 0; // variable to avoid duplicate messages

    // Create recap msg
    SetTradeRecapFields(*msg);
    msg->updateU32(MamdaCommonFields::MSG_SEQ_NUM, seqNum);
    msg->addI32("wTradeCount",901,seqNum++);
    ticker->callMamdaOnMsg(mSubscription, *msg);
	
    // set up date
    MamaDateTime* date = new MamaDateTime();
    date->setToNow();

    // reset msg and rebuild typical trade msg
    msg->clear();
    SetTradeReportFields(*msg, *date);

    START_RECORDING_CPU(ITERATIONS);
        msg->updateU32(MamdaCommonFields::MSG_SEQ_NUM, seqNum);
        msg->updateI32("wTradeCount", 901, seqNum);
        ticker->callMamdaOnMsg(mSubscription, *msg);
        seqNum++;
    STOP_RECORDING_CPU ();

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();

    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates update msg and sends this msg to TradeListener                     "
               "\n repetitively (%d), not processing entries                                            "
               "\n ********************************************************************************** \n",
               test_info->name(), ITERATIONS);
    }

    delete date;
    date = NULL;
    delete mTradeListener;
    mTradeListener = NULL;
    delete ticker;
    ticker = NULL;
    delete msg;
    msg = NULL;
}

/////////////////////////////////////////////////////////////////
//-------------------- Latency Tests --------------------------//
/////////////////////////////////////////////////////////////////

TEST_F(MamdaTradeListenerPerfTest, ProcessRecapMsgLatency)
{
    MamdaTradeListener *mTradeListener = new MamdaTradeListener;
    mSubscription->addMsgListener (mTradeListener);
    TradeTicker *ticker = new TradeTicker;
    mTradeListener->addHandler (ticker);

    // create recap msg
    MamaMsg *recapMsg = new MamaMsg;
    recapMsg->create();
    SetTradeRecapFields(*recapMsg);

    MamaDateTime *begin = new MamaDateTime();
    MamaDateTime *end = new MamaDateTime();
    begin->setToNow();

    for (int i = 0; i < ITERATIONS; ++i)
    {
        ticker->callMamdaOnMsg(mSubscription, *recapMsg);
    }
    end->setToNow();

    mama_f64_t totalTime = (end->getEpochTimeMicroseconds() - begin->getEpochTimeMicroseconds());
    mama_quantity_t latency = totalTime / ITERATIONS;

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    std::cout << "\n" << test_info->name() << ": Latency = " << latency << "us \n";

    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates recap msg and sends this mgs to orderBookListener                  "
               "\n repetitively (%d), not processing entries                                       "
               "\n ********************************************************************************** \n",
               test_info->name(), ITERATIONS);
    }

    delete ticker;
    ticker = NULL;
    delete mTradeListener;
    mTradeListener = NULL;
    delete recapMsg;
    recapMsg = NULL;
    delete begin;
    begin = NULL;
    delete end;
    end = NULL;
}

TEST_F(MamdaTradeListenerPerfTest, ProcessUpdateMsgLatency)
{
    MamdaTradeListener *mTradeListener = new MamdaTradeListener;
    mSubscription->addMsgListener (mTradeListener);
    TradeTicker *ticker = new TradeTicker;
    mTradeListener->addHandler (ticker);

    int seqNum = 0; // variable to avoid duplicate messages

    // create and fire recap msg
    MamaMsg *msg = new MamaMsg;
    msg->create();
    SetTradeRecapFields(*msg);
    msg->updateU32(MamdaCommonFields::MSG_SEQ_NUM, seqNum);
    msg->addI32("wTradeCount",901,seqNum++);
    ticker->callMamdaOnMsg(mSubscription, *msg);

    // set up date
    MamaDateTime* date = new MamaDateTime();
    date->setToNow();

    // clear msg and create report msg
    msg->clear();
    SetTradeReportFields(*msg, *date);

    MamaDateTime *begin = new MamaDateTime();
    MamaDateTime *end = new MamaDateTime();

    begin->setToNow();
    for ( ; seqNum <= ITERATIONS; ++seqNum)
    {
        msg->updateU32(MamdaCommonFields::MSG_SEQ_NUM, seqNum);
        msg->updateI32("wTradeCount", 901, seqNum);
        ticker->callMamdaOnMsg(mSubscription, *msg);
    }
    end->setToNow();

    mama_f64_t totalTime = (end->getEpochTimeMicroseconds() - begin->getEpochTimeMicroseconds());
    mama_quantity_t latency = totalTime / ITERATIONS;

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    std::cout << "\n" << test_info->name() << ": Latency = " << latency << "us \n";

    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates recap msg and sends this mgs to orderBookListener                  "
               "\n repetitively (%d), not processing entries                                       "
               "\n ********************************************************************************** \n",
               test_info->name(), ITERATIONS);
    }

    delete ticker;
    ticker = NULL;
    delete mTradeListener;
    mTradeListener = NULL;
    delete msg;
    msg = NULL;
    delete date;
    date = NULL;
    delete begin;
    begin = NULL;
    delete end;
    end = NULL;
}

