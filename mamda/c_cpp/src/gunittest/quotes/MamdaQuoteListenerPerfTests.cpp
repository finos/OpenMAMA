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
//using namespace QuoteControlFields;
//using namespace QuoteTestFields;


class QuoteTicker : public MamdaQuoteHandler
{
public:
    QuoteTicker () {}
    virtual ~QuoteTicker () {}

    void callMamdaOnMsg (MamdaSubscription* sub, MamaMsg& msg)
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

    void onQuoteRecap (
        MamdaSubscription*      subscription,
        MamdaQuoteListener&     listener,
        const MamaMsg&          msg,
        const MamdaQuoteRecap&  recap)
    {
//        cout << endl << "Quote recap... " << endl;
    }

    void onQuoteUpdate (
        MamdaSubscription*      subscription,
        MamdaQuoteListener&     listener,
        const MamaMsg&          msg,
        const MamdaQuoteUpdate& quote,
        const MamdaQuoteRecap&  recap)
    {
//        cout << endl << "Quote update... " << endl;
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
};

class MamdaQuoteListenerPerfTest : public ::testing::Test
{
protected:
    MamdaQuoteListenerPerfTest () {}
    virtual ~MamdaQuoteListenerPerfTest () {}

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
            MamdaCommonFields::reset();
            MamdaCommonFields::setDictionary (*myDictionary);
            MamdaQuoteFields::reset();
            MamdaQuoteFields::setDictionary (*myDictionary);

            mySubscription = new MamdaSubscription;
            if (!mySubscription)
            {
                FAIL() << "Failed to allocate MamdaSubscription\n";
                return;
            }
            mySubscription->setType(MAMA_SUBSC_TYPE_BOOK);
            mySubscription->setMdDataType(MAMA_MD_DATA_TYPE_ORDER_BOOK);
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
        if (mySubscription)
        {
            delete mySubscription;
            mySubscription = NULL;
        }
        if (myDictionary)
        {
            delete myDictionary;
            myDictionary = NULL;
        }
    }

    MamaDictionary*         myDictionary;
    MamdaSubscription*      mySubscription;
};

TEST_F(MamdaQuoteListenerPerfTest, ProcessRecapMsg)
{
    MamdaQuoteListener *myQuoteListener = new MamdaQuoteListener;
    mySubscription->addMsgListener (myQuoteListener);
    QuoteTicker *ticker = new QuoteTicker;
    myQuoteListener->addHandler(ticker);

    MamaMsg* recapMsg = new MamaMsg();
    recapMsg->create();
    SetQuoteRecapFields(*recapMsg);

    START_RECORDING_CPU(ITERATIONS);
        ticker->callMamdaOnMsg(mySubscription, *recapMsg);
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

    delete myQuoteListener;
    myQuoteListener = NULL;
    delete ticker;
    ticker = NULL;
    delete recapMsg;
    recapMsg = NULL;
}

TEST_F(MamdaQuoteListenerPerfTest, ProcessUpdateMsg)
{
    MamdaQuoteListener *myQuoteListener = new MamdaQuoteListener;
    mySubscription->addMsgListener (myQuoteListener);
    QuoteTicker *ticker = new QuoteTicker;
    myQuoteListener->addHandler(ticker);

    MamaMsg* msg = new MamaMsg();
    msg->create();

    // fire recap first
    SetQuoteRecapFields(*msg);
    ticker->callMamdaOnMsg(mySubscription, *msg);

    // clear msg and create update msg
    msg->clear();
    SetQuoteUpdateFields(*msg);

    START_RECORDING_CPU(ITERATIONS);
        ticker->callMamdaOnMsg(mySubscription, *msg);
    STOP_RECORDING_CPU();

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

    delete myQuoteListener;
    myQuoteListener = NULL;
    delete ticker;
    ticker = NULL;
    delete msg;
    msg = NULL;
}

/////////////////////////////////////////////////////////////////
//-------------------- Latency Tests --------------------------//
/////////////////////////////////////////////////////////////////

TEST_F(MamdaQuoteListenerPerfTest, ProcessRecapMsgLatency)
{
    MamdaQuoteListener *myQuoteListener = new MamdaQuoteListener;
    mySubscription->addMsgListener (myQuoteListener);
    QuoteTicker *ticker = new QuoteTicker;
    myQuoteListener->addHandler(ticker);

    // create recap msg
    MamaMsg *recapMsg = new MamaMsg();
    recapMsg->create();
    SetQuoteRecapFields(*recapMsg);

    MamaDateTime *begin = new MamaDateTime();
    MamaDateTime *end = new MamaDateTime();

    begin->setToNow();
    for (int i = 0; i < ITERATIONS; ++i)
    {
        ticker->callMamdaOnMsg(mySubscription, *recapMsg);
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
    delete myQuoteListener;
    myQuoteListener = NULL;
    delete recapMsg;
    recapMsg = NULL;
    delete begin;
    begin = NULL;
    delete end;
    end = NULL;
}

TEST_F(MamdaQuoteListenerPerfTest, ProcessUpdateMsgLatency)
{
    MamdaQuoteListener *myQuoteListener = new MamdaQuoteListener;
    mySubscription->addMsgListener (myQuoteListener);
    QuoteTicker *ticker = new QuoteTicker;
    myQuoteListener->addHandler(ticker);

    // create and fire recap msg
    MamaMsg *msg = new MamaMsg();
    msg->create();
    SetQuoteRecapFields(*msg);
    ticker->callMamdaOnMsg(mySubscription, *msg);

    // clear msg and create update msg
    msg->clear();
    SetQuoteUpdateFields(*msg);

    MamaDateTime *begin = new MamaDateTime();
    MamaDateTime *end   = new MamaDateTime();

    begin->setToNow();
    for (int i = 0; i < ITERATIONS; ++i)
    {
        ticker->callMamdaOnMsg(mySubscription, *msg);
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
    delete myQuoteListener;
    myQuoteListener = NULL;
    delete msg;
    msg = NULL;
    delete begin;
    begin = NULL;
    delete end;
    end = NULL;
}

