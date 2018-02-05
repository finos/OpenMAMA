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
#include <mamda/MamdaBookAtomicListener.h>
#include <mamda/MamdaBookAtomicBookHandler.h>
#include <mamda/MamdaBookAtomicLevelHandler.h>
#include <mamda/MamdaBookAtomicLevelEntryHandler.h>
#include <mamda/MamdaBookAtomicListener.h>
#include <mamda/MamdaOrderBook.h>
#include <mamda/MamdaOrderBookPriceLevel.h>
#include <mamda/MamdaOrderBookEntry.h>
#include <mamda/MamdaOrderBookTypes.h>
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
#include <cstdio>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <fstream>


using namespace Wombat;
using namespace std;
  
class AtomicTickerCpu : public MamdaBookAtomicBookHandler,
                        public MamdaBookAtomicLevelHandler,
                        public MamdaBookAtomicLevelEntryHandler
{
public:
    AtomicTickerCpu () {}
    virtual ~AtomicTickerCpu () {}
	
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


    void onBookAtomicLevelRecap (
        MamdaSubscription*          subscription,
        MamdaBookAtomicListener&    listener,
        const MamaMsg&              msg,
        const MamdaBookAtomicLevel& level)
    {
//         mama_log (MAMA_LOG_LEVEL_FINEST,"\n onBookAtomicLevelRecap \n");
    }
  
    void onBookAtomicBeginBook (
        MamdaSubscription*           subscription,
        MamdaBookAtomicListener&     listener,
        bool                         isRecap) 

    {
//         mama_log (MAMA_LOG_LEVEL_FINEST,"\n onBookAtomicBeginBook \n");
    }

    void onBookAtomicClear (
        MamdaSubscription*          subscription,
        MamdaBookAtomicListener&    listener,
        const MamaMsg&              msg)
    {
//         mama_log (MAMA_LOG_LEVEL_FINEST,"\n onBookAtomicClear \n");

    }
  
    void onBookAtomicGap (
        MamdaSubscription*                subscription,
        MamdaBookAtomicListener&          listener,
        const MamaMsg&                    msg,
        const MamdaBookAtomicGap&         event)
    {
//         mama_log (MAMA_LOG_LEVEL_FINEST,"\n onBookAtomicGap \n");

    }

    void onBookAtomicEndBook (
        MamdaSubscription*           subscription,
        MamdaBookAtomicListener&     listener) 
    {
//         mama_log (MAMA_LOG_LEVEL_FINEST,"\n onBookAtomicEndBook \n");

    }

    void onBookAtomicLevelEntryRecap (
        MamdaSubscription*                  subscription,
        MamdaBookAtomicListener&            listener,
        const MamaMsg&                      msg,
        const MamdaBookAtomicLevelEntry&    levelEntry)
    {
//         mama_log (MAMA_LOG_LEVEL_FINEST,"\n onBookAtomicLevelEntryRecap \n");

    }

    void onBookAtomicLevelDelta (
        MamdaSubscription*          subscription,
        MamdaBookAtomicListener&    listener,
        const MamaMsg&              msg,
        const MamdaBookAtomicLevel& level)
    {
//         mama_log (MAMA_LOG_LEVEL_FINEST,"\n onBookAtomicLevelDelta \n");

    }

    void onBookAtomicLevelEntryDelta (
        MamdaSubscription*                  subscription,
        MamdaBookAtomicListener&            listener,
        const MamaMsg&                      msg,
        const MamdaBookAtomicLevelEntry&    levelEntry)
    {
//         mama_log (MAMA_LOG_LEVEL_FINEST,"\n onBookAtomicLevelEntryDelta \n");
    }
};

class MamdaAtomicBookPerfTest : public ::testing::Test
{
protected:
    MamdaAtomicBookPerfTest () {}
    virtual ~MamdaAtomicBookPerfTest () {}

    virtual void SetUp()
    {
        mamaBridge bridge;
        bridge = Mama::loadBridge("wmw");
        Mama::open();
        //mama_enableLogging (stderr, MAMA_LOG_LEVEL_FINEST);
        dictionary = new MamaDictionary;
        dictionary->populateFromFile ("dictionary");
        MamdaCommonFields::reset();
        MamdaCommonFields::setDictionary (*dictionary);
        MamdaOrderBookFields::reset();
        MamdaOrderBookFields::setDictionary (*dictionary);

        mSubscription = new MamdaSubscription;	
        mSubscription->setType (MAMA_SUBSC_TYPE_BOOK);
        mSubscription->setMdDataType (MAMA_MD_DATA_TYPE_ORDER_BOOK);
    }
  
    virtual void TearDown()
    {
        if (mSubscription)
        {
            delete mSubscription;
            mSubscription = NULL;
        }

        if (dictionary)
        {
            delete dictionary;
            dictionary = NULL;
        }
    }
		
    MamaDictionary*     dictionary;
    MamdaSubscription*  mSubscription;
};

TEST_F (MamdaAtomicBookPerfTest, atomicRecapNoProcessEntries)
{
    MamdaBookAtomicListener* mBookListener = new MamdaBookAtomicListener;
    mSubscription->addMsgListener (mBookListener);
    AtomicTickerCpu* ticker = new AtomicTickerCpu;
	mBookListener->addBookHandler (ticker);
    mBookListener->addLevelHandler (ticker);

    MamaMsg* msg = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_INITIAL, 1);
   
    START_RECORDING_CPU (500000);
    ticker->callMamdaOnMsg (mSubscription, *msg);
	STOP_RECORDING_CPU ();
    
    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates recap msg and sends this mgs to orderBookListener                  "
               "\n repetitively 500000, not processing entries                                       "
               "\n ********************************************************************************** \n",               
               test_info->name());
    }
    
    delete ticker;
    ticker = NULL;
    delete msg;
    msg = NULL;
}

TEST_F (MamdaAtomicBookPerfTest, atomicRecapProcessEntries)
{
    MamdaBookAtomicListener* mBookListener = new MamdaBookAtomicListener;
    mSubscription->addMsgListener (mBookListener);
    AtomicTickerCpu* ticker = new AtomicTickerCpu;
    mBookListener->addBookHandler (ticker);
    mBookListener->addLevelHandler (ticker);
    mBookListener->addLevelEntryHandler (ticker);

    MamaMsg* msg = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_INITIAL, 1);

    START_RECORDING_CPU(500000);
    ticker->callMamdaOnMsg(mSubscription, *msg);
    STOP_RECORDING_CPU ();

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates recap msg and sends this mgs to orderBookListener                  "
               "\n repetitively 500000, processing entries                                       "
               "\n ********************************************************************************** \n",               
               test_info->name());
    }
    
    delete ticker;
    ticker = NULL;
    delete msg;
    msg = NULL;
}

TEST_F (MamdaAtomicBookPerfTest, atomicSimpleUpdateNoProcessEntries)
{
    MamdaBookAtomicListener* mBookListener = new MamdaBookAtomicListener;
    mSubscription->addMsgListener (mBookListener);
    AtomicTickerCpu* ticker = new AtomicTickerCpu;
    mBookListener->addBookHandler (ticker);
    mBookListener->addLevelHandler (ticker);

    MamaMsg* initial = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_INITIAL, 1);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_UPDATE, 1);

    START_RECORDING_CPU(500000);
    static int seqNum = 1;    
    update->updateU32(MamdaCommonFields::MSG_SEQ_NUM, seqNum);          
    ticker->callMamdaOnMsg(mSubscription, *update);
    seqNum++;
    STOP_RECORDING_CPU ();

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates a simple update msg and sends this mgs to orderBookListener                  "
               "\n repetitively 500000, not processing entries                                       "
               "\n ********************************************************************************** \n",               
               test_info->name());
    }
    
    delete ticker;
    ticker = NULL;
    delete initial;
    initial = NULL;
    delete update;
    update = NULL;
}

TEST_F (MamdaAtomicBookPerfTest, atomicSimpleUpdateProcessEntries)
{
    MamdaBookAtomicListener* mBookListener = new MamdaBookAtomicListener;
    mSubscription->addMsgListener (mBookListener);
    AtomicTickerCpu* ticker = new AtomicTickerCpu;
    mBookListener->addBookHandler (ticker);
    mBookListener->addLevelHandler (ticker);
    mBookListener->addLevelEntryHandler (ticker);

    MamaMsg* initial = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_INITIAL, 1);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_UPDATE, 1);

    START_RECORDING_CPU(500000);
    static int seqNum = 1;    
    update->updateU32(MamdaCommonFields::MSG_SEQ_NUM, seqNum);          
    ticker->callMamdaOnMsg(mSubscription, *update);
    seqNum++;
    STOP_RECORDING_CPU ();

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates a simple update msg and sends this mgs to orderBookListener                  "
               "\n repetitively 500000, processing entries                                       "
               "\n ********************************************************************************** \n",               
               test_info->name());
    }
    
    delete ticker;
    ticker = NULL;
    delete initial;
    initial = NULL;
    delete update;
    update = NULL;
}


TEST_F (MamdaAtomicBookPerfTest, atomicComplexUpdateNoProcessEntries)
{
    MamdaBookAtomicListener* mBookListener = new MamdaBookAtomicListener;
    mSubscription->addMsgListener (mBookListener);
    AtomicTickerCpu* ticker = new AtomicTickerCpu;
    mBookListener->addBookHandler (ticker);
    mBookListener->addLevelHandler (ticker);

    MamaMsg* initial = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_INITIAL, 2);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_UPDATE, 2);

    START_RECORDING_CPU(500000);
    static int seqNum = 1;    
    update->updateU32(MamdaCommonFields::MSG_SEQ_NUM, seqNum);          
    ticker->callMamdaOnMsg(mSubscription, *update);
    seqNum++;
    STOP_RECORDING_CPU ();

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates a complex update msg and sends this mgs to orderBookListener                  "
               "\n repetitively 500000, not processing entries                                       "
               "\n ********************************************************************************** \n",               
               test_info->name());
    }
    
    delete ticker;
    ticker = NULL;
    delete initial;
    initial = NULL;
    delete update;
    update = NULL;
}


TEST_F (MamdaAtomicBookPerfTest, atomicComplexUpdateProcessEntries)
{
    MamdaBookAtomicListener* mBookListener = new MamdaBookAtomicListener;
    mSubscription->addMsgListener (mBookListener);
    AtomicTickerCpu* ticker = new AtomicTickerCpu;
    mBookListener->addBookHandler (ticker);
    mBookListener->addLevelHandler (ticker);
    mBookListener->addLevelEntryHandler (ticker);

    MamaMsg* initial = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_INITIAL, 2);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_UPDATE, 2);

    START_RECORDING_CPU(500000);
    static int seqNum = 1;    
    update->updateU32(MamdaCommonFields::MSG_SEQ_NUM, seqNum);          
    ticker->callMamdaOnMsg(mSubscription, *update);
    seqNum++;
    STOP_RECORDING_CPU ();

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates a complex update msg and sends this mgs to orderBookListener                  "
               "\n repetitively 500000, processing entries                                       "
               "\n ********************************************************************************** \n",               
               test_info->name());

    }
    
    delete ticker;
    ticker = NULL;
    delete initial;
    initial = NULL;
    delete update;
    update = NULL;
}

TEST_F (MamdaAtomicBookPerfTest, atomicComplexUpdateFiveLevelsNoProcessEntries)
{
    MamdaBookAtomicListener* mBookListener = new MamdaBookAtomicListener;
    mSubscription->addMsgListener (mBookListener);
    AtomicTickerCpu* ticker = new AtomicTickerCpu;
    mBookListener->addBookHandler (ticker);
    mBookListener->addLevelHandler (ticker);
    
    MamaMsg* initial = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_INITIAL, 5);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_UPDATE, 5);
    
    START_RECORDING_CPU(500000);
    static int seqNum = 1;    
    update->updateU32(MamdaCommonFields::MSG_SEQ_NUM, seqNum);          
    ticker->callMamdaOnMsg(mSubscription, *update);
    seqNum++;
    STOP_RECORDING_CPU ();

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates a book of Five levels and updates all five levels in book          "
               "\n repetitively 500000, not processing entries                                       "
               "\n ********************************************************************************** \n",               
               test_info->name());
    }
    
    delete ticker;
    ticker = NULL;
    delete initial;
    initial = NULL;
    delete update;
    update = NULL;
}


TEST_F (MamdaAtomicBookPerfTest, atomicComplexUpdateFiveLevelsProcessEntries)
{
    MamdaBookAtomicListener* mBookListener = new MamdaBookAtomicListener;
    mSubscription->addMsgListener (mBookListener);
    AtomicTickerCpu* ticker = new AtomicTickerCpu;
    mBookListener->addBookHandler (ticker);
    mBookListener->addLevelHandler (ticker);
    mBookListener->addLevelEntryHandler (ticker);

    MamaMsg* initial = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_INITIAL, 5);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_UPDATE, 5);
    
    START_RECORDING_CPU(500000);
    static int seqNum = 1;    
    update->updateU32(MamdaCommonFields::MSG_SEQ_NUM, seqNum);          
    ticker->callMamdaOnMsg(mSubscription, *update);
    seqNum++;
    STOP_RECORDING_CPU ();

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates a book of Five levels and updates all five levels in book          "
               "\n repetitively 500000, processing entries                                       "
               "\n ********************************************************************************** \n",               
               test_info->name());
    }
    
    delete ticker;
    ticker = NULL;
    delete initial;
    initial = NULL;
    delete update;
    update = NULL;
}


TEST_F (MamdaAtomicBookPerfTest, atomicComplexUpdateFiveLevelsTwoLevelUpdateNoProcessEntries)
{
    MamdaBookAtomicListener* mBookListener = new MamdaBookAtomicListener;
    mSubscription->addMsgListener (mBookListener);
    AtomicTickerCpu* ticker = new AtomicTickerCpu;
    mBookListener->addBookHandler (ticker);
    mBookListener->addLevelHandler (ticker);
    
    MamaMsg* initial = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_INITIAL, 5);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_UPDATE, 2);
    
    START_RECORDING_CPU(500000);
    static int seqNum = 1;    
    update->updateU32(MamdaCommonFields::MSG_SEQ_NUM, seqNum);          
    ticker->callMamdaOnMsg(mSubscription, *update);
    seqNum++;
    STOP_RECORDING_CPU ();

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates a book of Five levels and updates top two levels in book           "
               "\n repetitively 500000, processing entries                                           "
               "\n ********************************************************************************** \n",               
               test_info->name());
    }
    
    delete ticker;
    ticker = NULL;
    delete initial;
    initial = NULL;
    delete update;
    update = NULL;
}


TEST_F (MamdaAtomicBookPerfTest, atomicComplexUpdateFiveLevelsTwoLevelUpdateProcessEntries)
{
    MamdaBookAtomicListener* mBookListener = new MamdaBookAtomicListener;
    mSubscription->addMsgListener (mBookListener);
    AtomicTickerCpu* ticker = new AtomicTickerCpu;
    mBookListener->addBookHandler (ticker);
    mBookListener->addLevelHandler (ticker);
    mBookListener->addLevelEntryHandler (ticker);

    MamaMsg* initial = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_INITIAL, 5);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_UPDATE, 2);
    
    START_RECORDING_CPU(500000);
    static int seqNum = 1;    
    update->updateU32(MamdaCommonFields::MSG_SEQ_NUM, seqNum);          
    ticker->callMamdaOnMsg(mSubscription, *update);
    seqNum++;
    STOP_RECORDING_CPU ();
    
    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates a book of Five levels and updates top two levels in book           "
               "\n repetitively 500000, processing entries                                           "
               "\n ********************************************************************************** \n",               
               test_info->name());
    }
        
    delete ticker;
    ticker = NULL;
    delete initial;
    initial = NULL;
    delete update;
    update = NULL;
}


TEST_F (MamdaAtomicBookPerfTest, atomicComplexUpdateTenLevelsThreeLevelUpdateNoProcessEntries)
{
    MamdaBookAtomicListener* mBookListener = new MamdaBookAtomicListener;
    mSubscription->addMsgListener (mBookListener);
    AtomicTickerCpu* ticker = new AtomicTickerCpu;
    mBookListener->addBookHandler (ticker);
    mBookListener->addLevelHandler (ticker);
    
    MamaMsg* initial = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_INITIAL, 10);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_UPDATE, 3);

    START_RECORDING_CPU(500000);
    static int seqNum = 1;    
    update->updateU32(MamdaCommonFields::MSG_SEQ_NUM, seqNum);          
    ticker->callMamdaOnMsg(mSubscription, *update);
    seqNum++;
    STOP_RECORDING_CPU ();

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates a book of ten levels and updates top three levels in book           "
               "\n repetitively 500000, processing entries                                           "
               "\n ********************************************************************************** \n",               
               test_info->name());
    }
    
    delete ticker;
    ticker = NULL;
    delete initial;
    initial = NULL;
    delete update;
    update = NULL;
}


TEST_F (MamdaAtomicBookPerfTest, atomicComplexUpdateTenLevelsThreeLevelUpdateProcessEntries)
{
    MamdaBookAtomicListener* mBookListener = new MamdaBookAtomicListener;
    mSubscription->addMsgListener (mBookListener);
    AtomicTickerCpu* ticker = new AtomicTickerCpu;
    mBookListener->addBookHandler (ticker);
    mBookListener->addLevelHandler (ticker);
    mBookListener->addLevelEntryHandler (ticker);
    
    MamaMsg* initial = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_INITIAL, 10);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_UPDATE, 3);

    START_RECORDING_CPU(500000);
    static int seqNum = 1;    
    update->updateU32(MamdaCommonFields::MSG_SEQ_NUM, seqNum);          
    ticker->callMamdaOnMsg(mSubscription, *update);
    seqNum++;
    STOP_RECORDING_CPU ();

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates a book of ten levels and updates top three levels in book           "
               "\n repetitively 500000, processing entries                                           "
               "\n ********************************************************************************** \n",               
               test_info->name());
    }
        
    delete ticker;
    ticker = NULL;
    delete initial;
    initial = NULL;
    delete update;
    update = NULL;
}


/****************************** LATENCY *****************************/

TEST_F (MamdaAtomicBookPerfTest, atomicRecapNoProcessEntriesLatency)
{
    MamdaBookAtomicListener* mBookListener = new MamdaBookAtomicListener;
    mSubscription->addMsgListener (mBookListener);
    AtomicTickerCpu* ticker = new AtomicTickerCpu;
    mBookListener->addBookHandler (ticker);
    mBookListener->addLevelHandler (ticker);

    MamaMsg* msg = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_INITIAL, 1);

    MamaDateTime* begin = new MamaDateTime();
    MamaDateTime* end = new MamaDateTime();
    begin->setToNow();
    
    for (int i=0; i< 500000; i++)
    {
        ticker->callMamdaOnMsg(mSubscription, *msg);
    }
    end->setToNow();
    mama_f64_t totalTime = (end->getEpochTimeMicroseconds() - begin->getEpochTimeMicroseconds());
    mama_quantity_t latency = totalTime/500000;

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    std::cout << "\n" << test_info->name() << ": Latency = " << latency << "us \n";
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates recap msg and sends this mgs to orderBookListener                  "
               "\n repetitively 500000, not processing entries                                       "
               "\n ********************************************************************************** \n",               
               test_info->name());
    }
    
    delete ticker;
    ticker = NULL;
    delete begin;
    begin = NULL;
    delete end;
    end = NULL;
    delete msg;
    msg = NULL;
}

TEST_F (MamdaAtomicBookPerfTest, atomicRecapProcessEntriesLatency)
{
    MamdaBookAtomicListener* mBookListener = new MamdaBookAtomicListener;
    mSubscription->addMsgListener (mBookListener);
    AtomicTickerCpu* ticker = new AtomicTickerCpu;
    mBookListener->addBookHandler (ticker);
    mBookListener->addLevelHandler (ticker);
    mBookListener->addLevelEntryHandler (ticker);

    MamaMsg* msg = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_INITIAL, 1);
       
    MamaDateTime* begin = new MamaDateTime();
    MamaDateTime* end = new MamaDateTime();
    begin->setToNow();
    
    for (int i=0; i< 500000; i++)
    {
        ticker->callMamdaOnMsg(mSubscription, *msg);
    }

    end->setToNow();
    mama_f64_t totalTime = (end->getEpochTimeMicroseconds() - begin->getEpochTimeMicroseconds());
    mama_quantity_t latency = totalTime/500000;

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    std::cout << "\n" << test_info->name() << ": Latency = " << latency << "us \n";
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates recap msg and sends this mgs to orderBookListener                  "
               "\n repetitively 500000, processing entries                                       "
               "\n ********************************************************************************** \n",               
               test_info->name());
    }
    
    delete ticker;
    ticker = NULL;
    delete begin;
    begin = NULL;
    delete end;
    end = NULL;
    delete msg;
    msg = NULL;
}

TEST_F (MamdaAtomicBookPerfTest, atomicSimpleUpdateNoProcessEntriesLatency)
{
    MamdaBookAtomicListener* mBookListener = new MamdaBookAtomicListener;
    mSubscription->addMsgListener (mBookListener);
    AtomicTickerCpu* ticker = new AtomicTickerCpu;
    mBookListener->addBookHandler (ticker);
    mBookListener->addLevelHandler (ticker);

    MamaMsg* initial = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_INITIAL, 1);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_UPDATE, 1);

    MamaDateTime* begin = new MamaDateTime();
    MamaDateTime* end = new MamaDateTime();
    begin->setToNow();
    
    for (int i=0; i<500000; i++)
    {
        static int seqNum = 1;    
        update->updateU32(MamdaCommonFields::MSG_SEQ_NUM, seqNum);
        ticker->callMamdaOnMsg(mSubscription, *update);
        seqNum++;
    }
    
    end->setToNow();
    mama_f64_t totalTime = (end->getEpochTimeMicroseconds() - begin->getEpochTimeMicroseconds());
    mama_quantity_t latency = totalTime/500000;

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    std::cout << "\n" << test_info->name() << ": Latency = " << latency << "us \n";
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates a simple update msg and sends this mgs to orderBookListener                  "
               "\n repetitively 500000, not processing entries                                       "
               "\n ********************************************************************************** \n",               
               test_info->name());
    }
    
    delete ticker;
    ticker = NULL;
    delete begin;
    begin = NULL;
    delete end;
    end = NULL;
    delete initial;
    initial = NULL;
    delete update;
    update = NULL;
}

TEST_F (MamdaAtomicBookPerfTest,atomicSimpleUpdateProcessEntriesLatency)
{
    MamdaBookAtomicListener* mBookListener = new MamdaBookAtomicListener;
    mSubscription->addMsgListener (mBookListener);
    AtomicTickerCpu* ticker = new AtomicTickerCpu;
    mBookListener->addBookHandler (ticker);
    mBookListener->addLevelHandler (ticker);
    mBookListener->addLevelEntryHandler (ticker);

    MamaMsg* initial = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_INITIAL, 1);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_UPDATE, 1);

    MamaDateTime* begin = new MamaDateTime();
    MamaDateTime* end = new MamaDateTime();
    begin->setToNow();
    
    for (int i=0; i<500000; i++)
    {
        static int seqNum = 1;    
        update->updateU32(MamdaCommonFields::MSG_SEQ_NUM, seqNum);
        ticker->callMamdaOnMsg(mSubscription, *update);
        seqNum++;
    }
    
    end->setToNow();
    mama_f64_t totalTime = (end->getEpochTimeMicroseconds() - begin->getEpochTimeMicroseconds());
    mama_quantity_t latency = totalTime/500000;

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    std::cout << "\n" << test_info->name() << ": Latency = " << latency << "us \n";
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates a simple update msg and sends this mgs to orderBookListener                  "
               "\n repetitively 500000, processing entries                                       "
               "\n ********************************************************************************** \n",               
               test_info->name());
    }
    
    delete ticker;
    ticker = NULL;
    delete begin;
    begin = NULL;
    delete end;
    end = NULL;
    delete initial;
    initial = NULL;
    delete update;
    update = NULL;
}


TEST_F (MamdaAtomicBookPerfTest, atomicComplexUpdateNoProcessEntriesLatency)
{
    MamdaBookAtomicListener* mBookListener = new MamdaBookAtomicListener;
    mSubscription->addMsgListener (mBookListener);
    AtomicTickerCpu* ticker = new AtomicTickerCpu;
    mBookListener->addBookHandler (ticker);
    mBookListener->addLevelHandler (ticker);

    MamaMsg* initial = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_INITIAL, 2);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_UPDATE, 2);
    
    MamaDateTime* begin = new MamaDateTime();
    MamaDateTime* end = new MamaDateTime();
    begin->setToNow();
    
    for (int i=0; i<500000; i++)
    {
        static int seqNum = 1;    
        update->updateU32(MamdaCommonFields::MSG_SEQ_NUM, seqNum);
        ticker->callMamdaOnMsg(mSubscription, *update);
        seqNum++;
    }
    
    end->setToNow();
    mama_f64_t totalTime = (end->getEpochTimeMicroseconds() - begin->getEpochTimeMicroseconds());
    mama_quantity_t latency = totalTime/500000;

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    std::cout << "\n" << test_info->name() << ": Latency = " << latency << "us \n";
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates a complex update msg and sends this mgs to orderBookListener                  "
               "\n repetitively 500000, not processing entries                                       "
               "\n ********************************************************************************** \n",               
               test_info->name());
    }
    
    delete ticker;
    ticker = NULL;
    delete begin;
    begin = NULL;
    delete end;
    end = NULL;
    delete initial;
    initial = NULL;
    delete update;
    update = NULL;
}


TEST_F (MamdaAtomicBookPerfTest, atomicComplexUpdateProcessEntriesLatency)
{
    MamdaBookAtomicListener* mBookListener = new MamdaBookAtomicListener;
    mSubscription->addMsgListener (mBookListener);
    AtomicTickerCpu* ticker = new AtomicTickerCpu;
    mBookListener->addBookHandler (ticker);
    mBookListener->addLevelHandler (ticker);
    mBookListener->addLevelEntryHandler (ticker);

    MamaMsg* initial = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_INITIAL, 2);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_UPDATE, 2);

    MamaDateTime* begin = new MamaDateTime();
    MamaDateTime* end = new MamaDateTime();
    begin->setToNow();
    
    for (int i=0; i<500000; i++)
    {
        static int seqNum = 1;    
        update->updateU32(MamdaCommonFields::MSG_SEQ_NUM, seqNum);
        ticker->callMamdaOnMsg(mSubscription, *update);
        seqNum++;
    }
    
    end->setToNow();
    mama_f64_t totalTime = (end->getEpochTimeMicroseconds() - begin->getEpochTimeMicroseconds());
    mama_quantity_t latency = totalTime/500000;

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    std::cout << "\n" << test_info->name() << ": Latency = " << latency << "us \n";
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates a complex update msg and sends this mgs to orderBookListener                  "
               "\n repetitively 500000, processing entries                                       "
               "\n ********************************************************************************** \n",               
               test_info->name());

    }
    
    delete ticker;
    ticker = NULL;
    delete begin;
    begin = NULL;
    delete end;
    end = NULL;
    delete initial;
    initial = NULL;
    delete update;
    update = NULL;
}


TEST_F (MamdaAtomicBookPerfTest, atomicComplexUpdateFiveLevelsNoProcessEntriesLatency)
{
    MamdaBookAtomicListener* mBookListener = new MamdaBookAtomicListener;
    mSubscription->addMsgListener (mBookListener);
    AtomicTickerCpu* ticker = new AtomicTickerCpu;
    mBookListener->addBookHandler (ticker);
    mBookListener->addLevelHandler (ticker);
    
    MamaMsg* initial = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_INITIAL, 5);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_UPDATE, 5);

    MamaDateTime* begin = new MamaDateTime();
    MamaDateTime* end = new MamaDateTime();
    begin->setToNow();
    
    for (int i=0; i<500000; i++)
    {
        static int seqNum = 1;    
        update->updateU32(MamdaCommonFields::MSG_SEQ_NUM, seqNum);
        ticker->callMamdaOnMsg(mSubscription, *update);
        seqNum++;
    }
    
    end->setToNow();
    mama_f64_t totalTime = (end->getEpochTimeMicroseconds() - begin->getEpochTimeMicroseconds());
    mama_quantity_t latency = totalTime/500000;

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    std::cout << "\n" << test_info->name() << ": Latency = " << latency << "us \n";
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates a book of Five levels and updates all five levels in book          "
               "\n repetitively 500000, not processing entries                                       "
               "\n ********************************************************************************** \n",               
               test_info->name());
    }
    
    delete ticker;
    ticker = NULL;
    delete begin;
    begin = NULL;
    delete end;
    end = NULL;
    delete initial;
    initial = NULL;
    delete update;
    update = NULL;
}


TEST_F (MamdaAtomicBookPerfTest, atomicComplexUpdateFiveLevelsProcessEntriesLatency)
{
    MamdaBookAtomicListener* mBookListener = new MamdaBookAtomicListener;
    mSubscription->addMsgListener (mBookListener);
    AtomicTickerCpu* ticker = new AtomicTickerCpu;
    mBookListener->addBookHandler (ticker);
    mBookListener->addLevelHandler (ticker);
    mBookListener->addLevelEntryHandler (ticker);

    MamaMsg* initial = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_INITIAL, 5);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_UPDATE, 5);
    
    MamaDateTime* begin = new MamaDateTime();
    MamaDateTime* end = new MamaDateTime();
    begin->setToNow();
    
    for (int i=0; i<500000; i++)
    {
        static int seqNum = 1;    
        update->updateU32(MamdaCommonFields::MSG_SEQ_NUM, seqNum);
        ticker->callMamdaOnMsg(mSubscription, *update);
        seqNum++;
    }
    
    end->setToNow();
    mama_f64_t totalTime = (end->getEpochTimeMicroseconds() - begin->getEpochTimeMicroseconds());
    mama_quantity_t latency = totalTime/500000;

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    std::cout << "\n" << test_info->name() << ": Latency = " << latency << "us \n";
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates a book of Five levels and updates all five levels in book          "
               "\n repetitively 500000, processing entries                                       "
               "\n ********************************************************************************** \n",               
               test_info->name());
    }
    
    delete ticker;
    ticker = NULL;
    delete begin;
    begin = NULL;
    delete end;
    end = NULL;
    delete initial;
    initial = NULL;
    delete update;
    update = NULL;
}


TEST_F (MamdaAtomicBookPerfTest, atomicComplexUpdateFiveLevelsTwoLevelUpdateNoProcessEntriesLatency)
{
    MamdaBookAtomicListener* mBookListener = new MamdaBookAtomicListener;
    mSubscription->addMsgListener (mBookListener);
    AtomicTickerCpu* ticker = new AtomicTickerCpu;
    mBookListener->addBookHandler (ticker);
    mBookListener->addLevelHandler (ticker);
    
    MamaMsg* initial = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_INITIAL, 5);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_UPDATE, 2);

    MamaDateTime* begin = new MamaDateTime();
    MamaDateTime* end = new MamaDateTime();
    begin->setToNow();
    
    for (int i=0; i<500000; i++)
    {
        static int seqNum = 1;    
        update->updateU32(MamdaCommonFields::MSG_SEQ_NUM, seqNum);
        ticker->callMamdaOnMsg(mSubscription, *update);
        seqNum++;
    }
    
    end->setToNow();
    mama_f64_t totalTime = (end->getEpochTimeMicroseconds() - begin->getEpochTimeMicroseconds());
    mama_quantity_t latency = totalTime/500000;

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    std::cout << "\n" << test_info->name() << ": Latency = " << latency << "us \n";
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates a book of Five levels and updates top two levels in book           "
               "\n repetitively 500000, processing entries                                           "
               "\n ********************************************************************************** \n",               
               test_info->name());
    }
    
    delete ticker;
    ticker = NULL;
    delete begin;
    begin = NULL;
    delete end;
    end = NULL;
    delete initial;
    initial = NULL;
    delete update;
    update = NULL;
}


TEST_F (MamdaAtomicBookPerfTest, atomicComplexUpdateFiveLevelsTwoLevelUpdateProcessEntriesLatency)
{
    MamdaBookAtomicListener* mBookListener = new MamdaBookAtomicListener;
    mSubscription->addMsgListener (mBookListener);
    AtomicTickerCpu* ticker = new AtomicTickerCpu;
    mBookListener->addBookHandler (ticker);
    mBookListener->addLevelHandler (ticker);
    mBookListener->addLevelEntryHandler (ticker);

    MamaMsg* initial = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_INITIAL, 5);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_UPDATE, 2);

    MamaDateTime* begin = new MamaDateTime();
    MamaDateTime* end = new MamaDateTime();
    begin->setToNow();
    
    for (int i=0; i<500000; i++)
    {
        static int seqNum = 1;    
        update->updateU32(MamdaCommonFields::MSG_SEQ_NUM, seqNum);
        ticker->callMamdaOnMsg(mSubscription, *update);
        seqNum++;
    }
    
    end->setToNow();
    mama_f64_t totalTime = (end->getEpochTimeMicroseconds() - begin->getEpochTimeMicroseconds());
    mama_quantity_t latency = totalTime/500000;

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    std::cout << "\n" << test_info->name() << ": Latency = " << latency << "us \n";
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates a book of Five levels and updates top two levels in book           "
               "\n repetitively 500000, processing entries                                           "
               "\n ********************************************************************************** \n",               
               test_info->name());
    }
        
    delete ticker;
    ticker = NULL;
    delete begin;
    begin = NULL;
    delete end;
    end = NULL;
    delete initial;
    initial = NULL;
    delete update;
    update = NULL;
}


TEST_F (MamdaAtomicBookPerfTest, atomicComplexUpdateTenLevelsThreeLevelUpdateNoProcessEntriesLatency)
{
    MamdaBookAtomicListener* mBookListener = new MamdaBookAtomicListener;
    mSubscription->addMsgListener (mBookListener);
    AtomicTickerCpu* ticker = new AtomicTickerCpu;
    mBookListener->addBookHandler (ticker);
    mBookListener->addLevelHandler (ticker);
    
    MamaMsg* initial = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_INITIAL, 10);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_UPDATE, 3);

    MamaDateTime* begin = new MamaDateTime();
    MamaDateTime* end = new MamaDateTime();
    begin->setToNow();
    
    for (int i=0; i<500000; i++)
    {
        static int seqNum = 1;    
        update->updateU32(MamdaCommonFields::MSG_SEQ_NUM, seqNum);
        ticker->callMamdaOnMsg(mSubscription, *update);
        seqNum++;
    }
    
    end->setToNow();
    mama_f64_t totalTime = (end->getEpochTimeMicroseconds() - begin->getEpochTimeMicroseconds());
    mama_quantity_t latency = totalTime/500000;

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    std::cout << "\n" << test_info->name() << ": Latency = " << latency << "us \n";
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates a book of ten levels and updates top three levels in book           "
               "\n repetitively 500000, processing entries                                           "
               "\n ********************************************************************************** \n",               
               test_info->name());
    }
    
    delete ticker;
    ticker = NULL;
    delete begin;
    begin = NULL;
    delete end;
    end = NULL;
    delete initial;
    initial = NULL;
    delete update;
    update = NULL;
}


TEST_F (MamdaAtomicBookPerfTest, atomicComplexUpdateTenLevelsThreeLevelUpdateProcessEntriesLatency)
{
    MamdaBookAtomicListener* mBookListener = new MamdaBookAtomicListener;
    mSubscription->addMsgListener (mBookListener);
    AtomicTickerCpu* ticker = new AtomicTickerCpu;
    mBookListener->addBookHandler (ticker);
    mBookListener->addLevelHandler (ticker);
    mBookListener->addLevelEntryHandler (ticker);
    
    MamaMsg* initial = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_INITIAL, 10);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_UPDATE, 3);
 
    MamaDateTime* begin = new MamaDateTime();
    MamaDateTime* end = new MamaDateTime();
    begin->setToNow();
    
    for (int i=0; i<500000; i++)
    {
        static int seqNum = 1;    
        update->updateU32(MamdaCommonFields::MSG_SEQ_NUM, seqNum);
        ticker->callMamdaOnMsg(mSubscription, *update);
        seqNum++;
    }

    end->setToNow();
    mama_f64_t totalTime = (end->getEpochTimeMicroseconds() - begin->getEpochTimeMicroseconds());
    mama_quantity_t latency = totalTime/500000;

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    std::cout << "\n" << test_info->name() << ": Latency = " << latency << "us \n";
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates a book of ten levels and updates top three levels in book           "
               "\n repetitively 500000, processing entries                                           "
               "\n ********************************************************************************** \n",               
               test_info->name());
    }
        
    delete ticker;
    ticker = NULL;
    delete begin;
    begin = NULL;
    delete end;
    end = NULL;
    delete initial;
    initial = NULL;
    delete update;
    update = NULL;
}
