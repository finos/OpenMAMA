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

#include <mamda/MamdaOrderBookFields.h>
#include <mamda/MamdaSubscription.h>
#include <mamda/MamdaCommonFields.h>
#include <mamda/MamdaOrderBookHandler.h>
#include <mamda/MamdaOrderBookListener.h>
#include <mamda/MamdaOrderBook.h>
#include <mamda/MamdaOrderBookPriceLevel.h>
#include <mamda/MamdaOrderBookEntry.h>
#include <mamda/MamdaOrderBookTypes.h>
#include <mamda/MamdaBookAtomicListener.h>
#include <mamda/MamdaBookAtomicBookHandler.h>
#include <mamda/MamdaBookAtomicLevelHandler.h>
#include <mamda/MamdaBookAtomicLevelEntryHandler.h>
#include <mamda/MamdaErrorListener.h>
#include <mamda/MamdaQualityListener.h>
#include <mama/MamaDictionary.h>
#include <mama/MamaReservedFields.h>
#include <mama/mamacpp.h>

#include "common/CpuTestGenerator.h"
#include "common/MemoryTestGenerator.h"
#include <wombat/wMessageStats.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <sstream>
#include <cstdio>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <fstream>
#include <sys/stat.h>
#include <vector>
#include <set>

using namespace Wombat;
using namespace std;

class AtomicBookTicker : public MamdaBookAtomicBookHandler,
                         public MamdaBookAtomicLevelHandler,
                         public MamdaBookAtomicLevelEntryHandler,
                         public MamdaErrorListener,
                         public MamdaQualityListener
{
public:
    AtomicBookTicker()
    : mEntryCount(0),
      mPrevEntryCount(0),
      mLevelCount(0),
      mErrorCount(0),
      mGapCount(0),
      mClearCount(0),
      mBookCount(0) ,
      mCurrPriceLevel(0.0) ,
      mCurrPriceLevelSize(0.0) ,
      mRunningPriceLevelSize(0.0) ,
      mProcessEntries(false) ,
      mCurrPriceLevelNumEntries(0.0),
      mCurrSide('U')
    {}

    virtual ~AtomicBookTicker () {}

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

    void onBookAtomicClear (
            MamdaSubscription*          subscription,
            MamdaBookAtomicListener&    listener,
            const MamaMsg&              msg)
    {
        ++mClearCount;
    }

    void onBookAtomicGap (
            MamdaSubscription*                subscription,
            MamdaBookAtomicListener&          listener,
            const MamaMsg&                    msg,
            const MamdaBookAtomicGap&         event)
    {
        ++mGapCount;
    }

    void onBookAtomicBeginBook (
            MamdaSubscription*           subscription,
            MamdaBookAtomicListener&     listener,
            bool                         isRecap)
    {
        mEntryCount = 0;
        mLevelCount = 0;
        mPrevEntryCount = 0;

        ++mBookCount;
    }

    void onBookAtomicEndBook (
            MamdaSubscription*           subscription,
            MamdaBookAtomicListener&     listener)
    {
        if(mProcessEntries)
        {
            ASSERT_EQ(mCurrPriceLevelNumEntries, mEntryCount - mPrevEntryCount);
            ASSERT_EQ(mCurrPriceLevelSize, mRunningPriceLevelSize);
        }

        mCurrPriceLevelSize = 0;
        mRunningPriceLevelSize = 0;
        mPrevEntryCount = mEntryCount;
        mCurrPriceLevelNumEntries = 0;
    }

    void processLevel(
            const MamdaBookAtomicLevel& level)
    {
        if (mProcessEntries)
        {
            /* Check that correct number of entry callbacks were received */
            ASSERT_EQ(mCurrPriceLevelNumEntries, mEntryCount - mPrevEntryCount);
            /* Check that the size of entries match size of price level */
            ASSERT_EQ(mCurrPriceLevelSize, mRunningPriceLevelSize);
        }
        mCurrSide = level.getPriceLevelSide();
        mPrevEntryCount = mEntryCount;
        mCurrPriceLevel = level.getPriceLevelPrice();
        mCurrPriceLevelSize = level.getPriceLevelSize();
        mCurrPriceLevelNumEntries = level.getPriceLevelNumEntries();

        mRunningPriceLevelSize = 0.0;
        ++mLevelCount;
    }

    void processEntry(
            const MamdaBookAtomicLevelEntry& levelEntry)
    {
        ASSERT_EQ(mCurrPriceLevel, levelEntry.getPriceLevelPrice());
        ASSERT_EQ(mCurrSide, levelEntry.getPriceLevelSide());

        mRunningPriceLevelSize += levelEntry.getPriceLevelEntrySize();
        ++mEntryCount;
    }

    void onBookAtomicLevelRecap (
            MamdaSubscription*          subscription,
            MamdaBookAtomicListener&    listener,
            const MamaMsg&              msg,
            const MamdaBookAtomicLevel& level)
    {
        processLevel(level);
    }

    void onBookAtomicLevelDelta (
            MamdaSubscription*          subscription,
            MamdaBookAtomicListener&    listener,
            const MamaMsg&              msg,
            const MamdaBookAtomicLevel& level)
    {
        processLevel(level);
    }

    void onBookAtomicLevelEntryRecap (
            MamdaSubscription*                  subscription,
            MamdaBookAtomicListener&            listener,
            const MamaMsg&                      msg,
            const MamdaBookAtomicLevelEntry&    levelEntry)
    {
        processEntry(levelEntry);
    }

    void onBookAtomicLevelEntryDelta (
            MamdaSubscription*                  subscription,
            MamdaBookAtomicListener&            listener,
            const MamaMsg&                      msg,
            const MamdaBookAtomicLevelEntry&    levelEntry)
    {
        processEntry(levelEntry);
    }

    void onError (
            MamdaSubscription*  subscription,
            MamdaErrorSeverity  severity,
            MamdaErrorCode      code,
            const char *        errorStr)
    {
        ++mErrorCount;
    }

    void onQuality (MamdaSubscription* subscription, mamaQuality quality)
    {
    }

    void clearCounters()
    {
        ASSERT_TRUE(mErrorCount == 0);
        ASSERT_TRUE(mGapCount == 0);
        ASSERT_TRUE(mClearCount == 0);

        mPrevEntryCount = mEntryCount = mLevelCount = mErrorCount = mGapCount = mClearCount = mBookCount = 0;
    }

    uint32_t mEntryCount;
    uint32_t mPrevEntryCount;
    uint32_t mLevelCount;
    uint32_t mErrorCount;
    uint32_t mGapCount;
    uint32_t mClearCount;
    uint32_t mBookCount;
    double   mCurrPriceLevelNumEntries;

    double   mCurrPriceLevel;
    double   mCurrPriceLevelSize;
    double   mRunningPriceLevelSize;

    bool     mProcessEntries;
    char     mCurrSide;
};

class MamdaBookAtomicListenerV5Tests : public ::testing::Test
{
protected:
    MamdaBookAtomicListenerV5Tests ()
    : m_numEntries(1000)
    , mNumPriceLevels(1000)
    , mMaxPrice(100.0)
    , mNumMessages(50)
    , mMsgs(NULL)
    , mNumUsedPriceLevels(NULL)
    {
        srand(time(NULL));

        mMsgs = new MamaMsg[mNumMessages];
        mNumUsedPriceLevels = new uint32_t[mNumMessages];

        //Set up the dictionary
        mDictionary = new MamaDictionary;
        mDictionary->populateFromFile("dictionary");
        MamdaCommonFields::setDictionary (*mDictionary);
        MamdaOrderBookFields::setDictionary (*mDictionary);
    }
    virtual ~MamdaBookAtomicListenerV5Tests ()
    {
        if (mDictionary)
        {
            delete mDictionary;
            mDictionary = NULL;
        }

        MamdaCommonFields::reset();
        MamdaOrderBookFields::reset();

        delete[] mMsgs;
        delete[] mNumUsedPriceLevels;
    }

    virtual void SetUp()
    {
        mamaBridge bridge;
        bridge = Mama::loadBridge("wmw");
        Mama::open();

        //mama_enableLogging (stderr, MAMA_LOG_LEVEL_OFF);

        generatePrices();

        /* Create the V5 orderbook initial */
        char*  buf         = NULL;
        size_t size        = 0;

        /*
        <template name="MamaHeadersOrderBookSnapshot">
        <uInt8       name="MdMsgType"     id="1"/>
        <uInt32      name="MdSeqNum"      id="10"/>
        <uInt8       name="MdMsgStatus"   id="2"/>
        <uInt16      name="MamaSenderId"  id="20"/>
        <timestamp   name="MamaSendTime"  id="16"/>
        <uInt32      name="wSeqNum"       id="498"/>
        <uInt16      name="wEntitleCode"  id="496"/>
        <string      name="wSymbol"       id="470" length="12" presence="optional"/>
        </template>

        <template name="OrderBookEntrySnapshot" id="51">
        <templateRef name="MamaHeadersOrderBookSnapshot" type="master"/>
        <uInt8       name="wBookType"     id="4714"><constant value ="1"/></uInt8>
        <sequence    name ="wPriceLevels" id="699">
        <templateRef name="wPlEntry"/>
        </sequence>
        </template>
         */

        MamaMsg tmpMsg;

        for(int j = 0; j<mNumMessages; ++j)
        {
            mUsedPrices.clear();

            mNumUsedPriceLevels[j] = mUsedPrices.size();

            /*
             * We reuse a single cache and then copy the temporary message to
             * make copy of the cache
             */
            tmpMsg.createFromBuffer(buf, size);
            mMsgs[j].copy(tmpMsg);
        }

        mSubscription = new MamdaSubscription;
        mAtomicBookListener = new MamdaBookAtomicListener;
        mSubscription->addMsgListener (mAtomicBookListener);

        mSubscription->setType(MAMA_SUBSC_TYPE_BOOK);
        mSubscription->setMdDataType(MAMA_MD_DATA_TYPE_ORDER_BOOK);
    }

    virtual void TearDown()
    {
        if (mSubscription)
        {
            delete mSubscription;
            mSubscription = NULL;
        }

        if (mAtomicBookListener)
        {
            delete mAtomicBookListener;
            mAtomicBookListener = NULL;
        }

        Mama::close();
    }

    void generatePrices()
    {
        int rNum;
        double price=0.0;

        mPrices.clear();

        for(int i = 0; i < mNumPriceLevels; i++)
        {
            rNum = rand();
            price = (rNum * mMaxPrice) / (double)RAND_MAX;
            mPrices.push_back(price);
        }
    }

    MamaDictionary*      	mDictionary;
    MamdaSubscription* 		mSubscription;
    MamdaBookAtomicListener* mAtomicBookListener;

    MamaMsg*                mMsgs;
    uint32_t*               mNumUsedPriceLevels;

    vector<double>          mPrices;
    set<double>             mUsedPrices;

    uint32_t                m_numEntries;
    uint32_t                mNumPriceLevels;
    uint32_t                mNumMessages;
    double                  mMaxPrice;
};

TEST_F (MamdaBookAtomicListenerV5Tests, noMsgApplied)
{
    AtomicBookTicker* aticker = new AtomicBookTicker;
    mAtomicBookListener->addBookHandler(aticker);
    mAtomicBookListener->addLevelEntryHandler(aticker);
    mAtomicBookListener->addLevelHandler(aticker);

    ASSERT_EQ(0, aticker->mEntryCount);
    ASSERT_EQ(0, aticker->mLevelCount);
    ASSERT_EQ(0, aticker->mBookCount);

    aticker->clearCounters();

    delete aticker;
}

TEST_F (MamdaBookAtomicListenerV5Tests, BookHandlerOnly)
{
    AtomicBookTicker* aticker = new AtomicBookTicker;
    mAtomicBookListener->addBookHandler(aticker);

    aticker->callMamdaOnMsg(mSubscription, mMsgs[0]);

    ASSERT_EQ(0, aticker->mEntryCount);
    ASSERT_EQ(0, aticker->mLevelCount);
    ASSERT_EQ(1, aticker->mBookCount);

    aticker->clearCounters();

    delete aticker;
}

TEST_F (MamdaBookAtomicListenerV5Tests, BookAndLevelHandler)
{
    AtomicBookTicker* aticker = new AtomicBookTicker;
    mAtomicBookListener->addBookHandler(aticker);
    mAtomicBookListener->addLevelHandler(aticker);

    aticker->callMamdaOnMsg(mSubscription, mMsgs[0]);

    ASSERT_EQ(0, aticker->mEntryCount);
    ASSERT_EQ(mNumUsedPriceLevels[0], aticker->mLevelCount);
    ASSERT_EQ(1, aticker->mBookCount);

    aticker->clearCounters();

    delete aticker;
}

TEST_F (MamdaBookAtomicListenerV5Tests, BookLevelAndEntryHandler)
{
    AtomicBookTicker* aticker = new AtomicBookTicker;
    mAtomicBookListener->addBookHandler(aticker);
    mAtomicBookListener->addLevelHandler(aticker);
    mAtomicBookListener->addLevelEntryHandler(aticker);
    aticker->mProcessEntries = true;

    aticker->callMamdaOnMsg(mSubscription, mMsgs[0]);

    ASSERT_EQ(m_numEntries, aticker->mEntryCount);
    ASSERT_EQ(mNumUsedPriceLevels[0], aticker->mLevelCount);
    ASSERT_EQ(1, aticker->mBookCount);

    aticker->clearCounters();

    delete aticker;
}

TEST_F (MamdaBookAtomicListenerV5Tests, BookHandlerOnlyMultiple)
{
    AtomicBookTicker* aticker = new AtomicBookTicker;
    mAtomicBookListener->addBookHandler(aticker);

    for(int i = 0; i< mNumMessages; ++i)
    {
        aticker->callMamdaOnMsg(mSubscription, mMsgs[i]);
        ASSERT_EQ(0, aticker->mEntryCount);
        ASSERT_EQ(0, aticker->mLevelCount);
    }

    ASSERT_EQ(mNumMessages, aticker->mBookCount);

    aticker->clearCounters();

    delete aticker;
}

TEST_F (MamdaBookAtomicListenerV5Tests, BookAndLevelHandlerMultiple)
{
    AtomicBookTicker* aticker = new AtomicBookTicker;
    mAtomicBookListener->addBookHandler(aticker);
    mAtomicBookListener->addLevelHandler(aticker);

    for(int i = 0; i < mNumMessages; ++i)
    {
        aticker->callMamdaOnMsg(mSubscription, mMsgs[i]);
        ASSERT_EQ(0, aticker->mEntryCount);
        ASSERT_EQ(mNumUsedPriceLevels[i], aticker->mLevelCount);
    }

    ASSERT_EQ(mNumMessages, aticker->mBookCount);

    aticker->clearCounters();

    delete aticker;
}

TEST_F (MamdaBookAtomicListenerV5Tests, BookLevelAndEntryHandlerMultiple)
{
    AtomicBookTicker* aticker = new AtomicBookTicker;
    mAtomicBookListener->addBookHandler(aticker);
    mAtomicBookListener->addLevelHandler(aticker);
    mAtomicBookListener->addLevelEntryHandler(aticker);
    aticker->mProcessEntries = true;

    for(int i = 0; i < mNumMessages; i++)
    {
        aticker->callMamdaOnMsg(mSubscription, mMsgs[i]);
        ASSERT_EQ(m_numEntries, aticker->mEntryCount);
        ASSERT_EQ(mNumUsedPriceLevels[i], aticker->mLevelCount);
    }

    ASSERT_EQ(mNumMessages, aticker->mBookCount);

    aticker->clearCounters();

    delete aticker;
}

