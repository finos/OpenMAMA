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

class TradeListenerCB : public MamdaTradeHandler
{
public:
    TradeListenerCB ()
        : mTradeRecapCount (0)
        , mTradeReportCount (0)
        , mTradeGapCount (0)
        , mTradeCancelOrErrorCount (0)
        , mTradeCorrectionCount (0)
        , mTradeClosingCount (0)
        , mTradeOutOfSequenceCount (0)
        , mTradePossiblyDuplicateCount (0)
        , mOnErrorCount (0)
        , mOnQualityCount (0)
    {
    }

    virtual ~TradeListenerCB () {}

    void onTradeRecap (
        MamdaSubscription*      subscription,
        MamdaTradeListener&     listener,
        const MamaMsg&          msg,
        const MamdaTradeRecap&  recap)
    {
        ++mTradeRecapCount;
    }

    void onTradeReport (
        MamdaSubscription*      subscription,
        MamdaTradeListener&     listener,
        const MamaMsg&          msg,
        const MamdaTradeReport& event,
        const MamdaTradeRecap&  recap)
    {
        ++mTradeReportCount;
    }

    void onTradeGap (
        MamdaSubscription*              subscription,
        MamdaTradeListener&             listener,
        const MamaMsg&                  msg,
        const MamdaTradeGap&            event,
        const MamdaTradeRecap&          recap)
    {
        ++mTradeGapCount;
    }

    void onTradeCancelOrError (
        MamdaSubscription*              subscription,
        MamdaTradeListener&             listener,
        const MamaMsg&                  msg,
        const MamdaTradeCancelOrError&  event,
        const MamdaTradeRecap&          recap)
    {
        ++mTradeCancelOrErrorCount;
    }

    void onTradeCorrection (
        MamdaSubscription*              subscription,
        MamdaTradeListener&             listener,
        const MamaMsg&                  msg,
        const MamdaTradeCorrection&     event,
        const MamdaTradeRecap&          recap)
    {
        ++mTradeCorrectionCount;
    }

    void onTradeClosing (
        MamdaSubscription*              subscription,
        MamdaTradeListener&             listener,
        const MamaMsg&                  msg,
        const MamdaTradeClosing&        event,
        const MamdaTradeRecap&          recap)
    {
        ++mTradeClosingCount;
    }

    void onTradeOutOfSequence (
        MamdaSubscription*              subscription,
        MamdaTradeListener&             listener,
        const MamaMsg&                  msg,
        const MamdaTradeOutOfSequence&  event,
        const MamdaTradeRecap&          recap) 
    {
        ++mTradeOutOfSequenceCount;
    }

    void onTradePossiblyDuplicate (
        MamdaSubscription*                  subscription,
        MamdaTradeListener&                 listener,
        const MamaMsg&                      msg,
        const MamdaTradePossiblyDuplicate&  event,
        const MamdaTradeRecap&              recap) 
    {
        ++mTradePossiblyDuplicateCount;
    }

    void onError (
        MamdaSubscription*   subscription,
        MamdaErrorSeverity   severity,
        MamdaErrorCode       errorCode,
        const char*          errorStr)
    {
        ++mOnErrorCount;
    }

    void onQuality (
        MamdaSubscription*   subscription,
        mamaQuality          quality)
    {
        ++mOnQualityCount;
    }

    uint getTradeRecapCount ()          { return mTradeRecapCount; }
    uint getTradeReportCount ()         { return mTradeReportCount; }
    uint getTradeGapCount ()            { return mTradeGapCount; }
    uint getTradeCancelOrErrorCount ()  { return mTradeCancelOrErrorCount; }
    uint getTradeCorrectionCount ()     { return mTradeCorrectionCount; }
    uint getTradeClosingCount ()        { return mTradeClosingCount; }
    uint getTradeOutOfSequenceCount ()  { return mTradeOutOfSequenceCount; }
    uint getTradePossiblyDuplicateCount () { return mTradePossiblyDuplicateCount; }
    uint getOnErrorCount ()             { return mOnErrorCount; }
    uint getOnQualityCount ()           { return mOnQualityCount; }

private:
    uint mTradeRecapCount;
    uint mTradeReportCount;
    uint mTradeGapCount;
    uint mTradeCancelOrErrorCount;
    uint mTradeCorrectionCount;
    uint mTradeClosingCount;
    uint mTradeOutOfSequenceCount;
    uint mTradePossiblyDuplicateCount;
    uint mOnErrorCount;
    uint mOnQualityCount;
};

class MamdaTradeListenerTest : public ::testing::Test
{
protected:
    MamdaTradeListenerTest () {}
    virtual ~MamdaTradeListenerTest () {}

    virtual void SetUp()
    {
        try
        {
            mamaBridge bridge;
            bridge = Mama::loadBridge("wmw"); //TODO - make configurable
            Mama::open();

            mDictionary = new MamaDictionary;
            if (!mDictionary)
            {
                FAIL() << "Failed to allocate MamaDictionary\n";
                return;
            }
            mDictionary->populateFromFile("dictionary.txt"); //TODO - make configurable
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
        catch (MamaStatus status)
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
    }

    MamaDictionary*         mDictionary;
    MamdaSubscription*      mSubscription;
    MamdaTradeListener*     mTradeListener;
};

TEST_F (MamdaTradeListenerTest, TradeReportCount)
{
    TradeListenerCB ticker;
    mTradeListener->addHandler (&ticker);

    MamaMsg report;
    createReport(report);

    MamaMsg update;
    createUpdate(update);

    callMamdaOnMsg(mSubscription, report);
    callMamdaOnMsg(mSubscription, update);

    //NB: depends on wTradeCount being incremented
    EXPECT_EQ (2, ticker.getTradeReportCount())
        << "Unexpected quantity of trade reports published: "
        << ticker.getTradeReportCount();
}

TEST_F (MamdaTradeListenerTest, TradeReportCountProcessUpdatesOff)
{
    TradeListenerCB ticker;
    mTradeListener->addHandler (&ticker);
    mTradeListener->setCheckUpdatesForTrades (false);

    MamaMsg report;
    createReport(report);

    MamaMsg update;
    createUpdate(update);

    callMamdaOnMsg(mSubscription, report);
    callMamdaOnMsg(mSubscription, update);

    EXPECT_EQ (1, ticker.getTradeReportCount())
        << "Unexpected quantity of trade reports published: "
        << ticker.getTradeReportCount();
}

TEST_F (MamdaTradeListenerTest, TradeReportCountUpdateWithoutTradeInfo)
{
    TradeListenerCB ticker;
    mTradeListener->addHandler (&ticker);

    MamaMsg report;
    createReport(report);

    MamaMsg update;
    createUpdateWithoutTrade(update);

    callMamdaOnMsg(mSubscription, report);
    callMamdaOnMsg(mSubscription, update);

    EXPECT_EQ (1, ticker.getTradeReportCount())
        << "Unexpected quantity of trade reports published: "
        << ticker.getTradeReportCount();
}

TEST_F (MamdaTradeListenerTest, TradeReportId)
{
    TradeListenerCB ticker;
    mTradeListener->addHandler (&ticker);
    mama_seqnum_t seqNum = 1;
    MamaMsg recap;
    createRecap(recap);
    callMamdaOnMsg(mSubscription, recap);

    MamaMsg update;
    createReport(update);
    update.addI32("wTradeSeqNum",483, seqNum);
    seqNum++;
    update.addU64("wOrigTradeId",1096,101);
    update.addU64("wTradeId",477,101); 
    callMamdaOnMsg(mSubscription, update);

    //assert no exception is thrown
    ASSERT_NO_THROW(callMamdaOnMsg(mSubscription, update););
}

TEST_F (MamdaTradeListenerTest, TradeReportLastTime)
{
    TradeListenerCB ticker;
    mTradeListener->addHandler (&ticker);
    mama_seqnum_t seqNum = 1;
    MamaMsg recap;
    createRecap(recap);
    callMamdaOnMsg(mSubscription, recap);

    MamaMsg update;
    createReport(update);
    MamaDateTime lastTime;
    lastTime.setToNow();
    update.addDateTime("wLastTime",5251, lastTime);

    callMamdaOnMsg(mSubscription, update);

    //assert no exception is thrown
    ASSERT_NO_THROW(callMamdaOnMsg(mSubscription, update););
}

