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
  
class BookTickerCpu : public MamdaOrderBookHandler
{
public:
    BookTickerCpu () {}
    virtual ~BookTickerCpu () {}
	
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

	void onBookRecap (
        MamdaSubscription*                 subscription,
        MamdaOrderBookListener&            listener,
        const MamaMsg*                     msg,
        const MamdaOrderBookComplexDelta*  delta,
        const MamdaOrderBookRecap&         recap,
        const MamdaOrderBook&              book)
    {
//         printf("\n RECAP \n");
//         book.dump(std::cout);
    }
    
    void onBookDelta (
        MamdaSubscription*                 subscription,
        MamdaOrderBookListener&            listener,
        const MamaMsg*                     msg,
        const MamdaOrderBookSimpleDelta&   delta,
        const MamdaOrderBook&              book)
    {
//         printf("\n UPDATE \n");
//         book.dump(std::cout);
    }
    
   void onBookComplexDelta (
        MamdaSubscription*                 subscription,
        MamdaOrderBookListener&            listener,
        const MamaMsg*                     msg,
        const MamdaOrderBookComplexDelta&  delta,
        const MamdaOrderBook&              book)
    {
//         printf("\n COMPLEX UPDATE \n");
//         book.dump(std::cout);
    }
    void onBookClear (
        MamdaSubscription*          subscription,
        MamdaOrderBookListener&     listener,
        const MamaMsg*              msg,
        const MamdaOrderBookClear&  clear,
        const MamdaOrderBook&       book)
    {
    }

    void onBookGap (
        MamdaSubscription*          subscription,
        MamdaOrderBookListener&     listener,
        const MamaMsg*              msg,
        const MamdaOrderBookGap&    event,
        const MamdaOrderBook&       book)
    {
    }
};

class MamdaBookPerfTest : public ::testing::Test
{
protected:
	MamdaBookPerfTest () {}
	virtual ~MamdaBookPerfTest () {}
  
	virtual void SetUp()
	{
		mamaBridge bridge;
		bridge = Mama::loadBridge("wmw");
		Mama::open();
// 		mama_enableLogging (stderr, MAMA_LOG_LEVEL_FINEST);
		dictionary = new MamaDictionary;
		dictionary->populateFromFile("dictionary");
        MamdaCommonFields::reset();
        MamdaCommonFields::setDictionary (*dictionary);
        MamdaOrderBookFields::reset();
        MamdaOrderBookFields::setDictionary (*dictionary);
		
		mSubscription = new MamdaSubscription;	
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
		
		if (dictionary)
		{
		    delete dictionary;
		    dictionary = NULL;
		}
	}
		
	MamaDictionary*      	dictionary;
	MamdaSubscription* 		mSubscription;

};


TEST_F (MamdaBookPerfTest, RecapNoProcessEntries)
{
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener;
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (false);
    BookTickerCpu* ticker = new BookTickerCpu;
    mBookListener->addHandler (ticker);

    MamaMsg* msg = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_INITIAL, 1);

    START_RECORDING_CPU(1000000);
    ticker->callMamdaOnMsg(mSubscription, *msg);
    STOP_RECORDING_CPU ();

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates recap msg and sends this mgs to orderBookListener                  "
               "\n repetitively (1000000), not processing entries                                       "
               "\n ********************************************************************************** \n",               
               test_info->name());
    }
    
    delete ticker;
    ticker = NULL;
    delete msg;
    msg = NULL;
}

TEST_F (MamdaBookPerfTest, RecapProcessEntries)
{
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener;
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (true);
    BookTickerCpu* ticker = new BookTickerCpu;
    mBookListener->addHandler (ticker);

    MamaMsg* msg = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_INITIAL, 1);

    START_RECORDING_CPU(1000000);
    ticker->callMamdaOnMsg(mSubscription, *msg);
    STOP_RECORDING_CPU ();

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates recap msg and sends this mgs to orderBookListener                  "
               "\n repetitively (1000000), processing entries                                       "
               "\n ********************************************************************************** \n",               
               test_info->name());
    }
    
    delete ticker;
    ticker = NULL;
    delete msg;
    msg = NULL;
}

TEST_F (MamdaBookPerfTest, SimpleUpdateNoProcessEntries)
{
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener;
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (false);
    BookTickerCpu* ticker = new BookTickerCpu;
    mBookListener->addHandler (ticker);
    
    MamaMsg* initial = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_INITIAL, 1);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_UPDATE, 1);

    START_RECORDING_CPU(1000000);
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
               "\n repetitively (1000000), not processing entries                                       "
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

TEST_F (MamdaBookPerfTest, SimpleUpdateProcessEntries)
{
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener;
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (true);
    BookTickerCpu* ticker = new BookTickerCpu;
    mBookListener->addHandler (ticker);
    
    MamaMsg* initial = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_INITIAL, 1);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_UPDATE, 1);
    
    START_RECORDING_CPU(1000000);
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
               "\n repetitively (1000000), processing entries                                       "
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


TEST_F (MamdaBookPerfTest, ComplexUpdateNoProcessEntries)
{
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener;
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (false);
    BookTickerCpu* ticker = new BookTickerCpu;
    mBookListener->addHandler (ticker);
    
    MamaMsg* initial = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_INITIAL, 2);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_UPDATE, 2);
    
    START_RECORDING_CPU(1000000);
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
               "\n repetitively (1000000), not processing entries                                       "
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


TEST_F (MamdaBookPerfTest, ComplexUpdateProcessEntries)
{
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener;
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (true);
    BookTickerCpu* ticker = new BookTickerCpu;
    mBookListener->addHandler (ticker);
    
    MamaMsg* initial = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_INITIAL, 2);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_UPDATE, 2);

    START_RECORDING_CPU(1000000);
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
               "\n repetitively (1000000), processing entries                                       "
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


TEST_F (MamdaBookPerfTest, ComplexUpdateFiveLevelsNoProcessEntries)
{
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener;
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (false);
    BookTickerCpu* ticker = new BookTickerCpu;
    mBookListener->addHandler (ticker);
    
    MamaMsg* initial = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_INITIAL, 5);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_UPDATE, 5);

    START_RECORDING_CPU(1000000);
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
               "\n repetitively (1000000), not processing entries                                       "
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


TEST_F (MamdaBookPerfTest, ComplexUpdateFiveLevelsProcessEntries)
{
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener;
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (true);
    BookTickerCpu* ticker = new BookTickerCpu;
    mBookListener->addHandler (ticker);
    
    MamaMsg* initial = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_INITIAL, 5);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_UPDATE, 5);

    START_RECORDING_CPU(1000000);
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
               "\n repetitively (1000000), processing entries                                       "
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


TEST_F (MamdaBookPerfTest, ComplexUpdateFiveLevelsTwoLevelUpdateNoProcessEntries)
{
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener;
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (false);
    BookTickerCpu* ticker = new BookTickerCpu;
    mBookListener->addHandler (ticker);
    
    MamaMsg* initial = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_INITIAL, 5);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_UPDATE, 2);

    START_RECORDING_CPU(1000000);
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
               "\n repetitively (1000000), processing entries                                           "
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


TEST_F (MamdaBookPerfTest, ComplexUpdateFiveLevelsTwoLevelUpdateProcessEntries)
{
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener;
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (true);
    BookTickerCpu* ticker = new BookTickerCpu;
    mBookListener->addHandler (ticker);
    
    MamaMsg* initial = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_INITIAL, 5);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_UPDATE, 2);

    START_RECORDING_CPU(1000000);
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
               "\n repetitively (1000000), processing entries                                           "
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


TEST_F (MamdaBookPerfTest, ComplexUpdateTenLevelsThreeLevelUpdateNoProcessEntries)
{
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener;
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (false);
    BookTickerCpu* ticker = new BookTickerCpu;
    mBookListener->addHandler (ticker);
    
    MamaMsg* initial = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_INITIAL, 10);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_UPDATE, 3);

    START_RECORDING_CPU(1000000);
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
               "\n repetitively (1000000), processing entries                                           "
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


TEST_F (MamdaBookPerfTest, ComplexUpdateTenLevelsThreeLevelUpdateProcessEntries)
{
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener;
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (true);
    BookTickerCpu* ticker = new BookTickerCpu;
    mBookListener->addHandler (ticker);
    
    MamaMsg* initial = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_INITIAL, 10);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_UPDATE, 3);

    START_RECORDING_CPU(1000000);
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
               "\n repetitively (1000000), processing entries                                           "
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


/******************* LATENCY TESTS BELOW **************************************/


TEST_F (MamdaBookPerfTest, RecapNoProcessEntriesLatency)
{
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener;
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (false);
    BookTickerCpu* ticker = new BookTickerCpu;
    mBookListener->addHandler (ticker);

    MamaMsg* msg = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_INITIAL, 1);

    MamaDateTime* begin = new MamaDateTime();
    MamaDateTime* end = new MamaDateTime();
    begin->setToNow();
    
    for (int i=0; i<1000000; i++)
    {
        ticker->callMamdaOnMsg(mSubscription, *msg);
    }
    end->setToNow();

    mama_f64_t totalTime = (end->getEpochTimeMicroseconds() - begin->getEpochTimeMicroseconds());
    mama_quantity_t latency = totalTime/1000000;

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    std::cout << "\n" << test_info->name() << ": Latency = " << latency << "us \n";

    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates recap msg and sends this mgs to orderBookListener                  "
               "\n repetitively (1000000), not processing entries                                       "
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

TEST_F (MamdaBookPerfTest, RecapProcessEntriesLatency)
{
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener;
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (true);
    BookTickerCpu* ticker = new BookTickerCpu;
    mBookListener->addHandler (ticker);

    MamaMsg* msg = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_INITIAL, 1);
       
    MamaDateTime* begin = new MamaDateTime();
    MamaDateTime* end = new MamaDateTime();
    begin->setToNow();
    
    for (int i=0; i<1000000; i++)
    {
        ticker->callMamdaOnMsg(mSubscription, *msg);
    }
    
    end->setToNow();
    mama_f64_t totalTime = (end->getEpochTimeMicroseconds() - begin->getEpochTimeMicroseconds());
    mama_quantity_t latency = totalTime/1000000;

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    std::cout << "\n" << test_info->name() << ": Latency = " << latency << "us \n";
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates recap msg and sends this mgs to orderBookListener                  "
               "\n repetitively (1000000), processing entries                                       "
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

TEST_F (MamdaBookPerfTest, SimpleUpdateNoProcessEntriesLatency)
{
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener;
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (false);
    BookTickerCpu* ticker = new BookTickerCpu;
    mBookListener->addHandler (ticker);
    
    MamaMsg* initial = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_INITIAL, 1);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_UPDATE, 1);

    MamaDateTime* begin = new MamaDateTime();
    MamaDateTime* end = new MamaDateTime();
    begin->setToNow();
    
    for (int i=0; i<1000000; i++)
    {
        static int seqNum = 1;
        update->updateU32(MamdaCommonFields::MSG_SEQ_NUM, seqNum);
        ticker->callMamdaOnMsg(mSubscription, *update);
        seqNum++;
    }

    end->setToNow();
    mama_f64_t totalTime = (end->getEpochTimeMicroseconds() - begin->getEpochTimeMicroseconds());
    mama_quantity_t latency = totalTime/1000000;

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    std::cout << "\n" << test_info->name() << ": Latency = " << latency << "us \n";
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates a simple update msg and sends this mgs to orderBookListener                  "
               "\n repetitively (1000000), not processing entries                                       "
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

TEST_F (MamdaBookPerfTest, SimpleUpdateProcessEntriesLatency)
{
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener;
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (true);
    BookTickerCpu* ticker = new BookTickerCpu;
    mBookListener->addHandler (ticker);
    
    MamaMsg* initial = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_INITIAL, 1);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_UPDATE, 1);

    MamaDateTime* begin = new MamaDateTime();
    MamaDateTime* end = new MamaDateTime();
    begin->setToNow();
    
    for (int i=0; i<1000000; i++)
    {
        static int seqNum = 1;    
        update->updateU32(MamdaCommonFields::MSG_SEQ_NUM, seqNum);
        ticker->callMamdaOnMsg(mSubscription, *update);
        seqNum++;
    }
    
    end->setToNow();
    mama_f64_t totalTime = (end->getEpochTimeMicroseconds() - begin->getEpochTimeMicroseconds());
    mama_quantity_t latency = totalTime/1000000;

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    std::cout << "\n" << test_info->name() << ": Latency = " << latency << "us \n";
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates a simple update msg and sends this mgs to orderBookListener                  "
               "\n repetitively (1000000), processing entries                                       "
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


TEST_F (MamdaBookPerfTest, ComplexUpdateNoProcessEntriesLatency)
{
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener;
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (false);
    BookTickerCpu* ticker = new BookTickerCpu;
    mBookListener->addHandler (ticker);
    
    MamaMsg* initial = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_INITIAL, 2);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_UPDATE, 2);
    
    MamaDateTime* begin = new MamaDateTime();
    MamaDateTime* end = new MamaDateTime();
    begin->setToNow();
    
    for (int i=0; i<1000000; i++)
    {
        static int seqNum = 1;    
        update->updateU32(MamdaCommonFields::MSG_SEQ_NUM, seqNum);
        ticker->callMamdaOnMsg(mSubscription, *update);
        seqNum++;
    }
    
    end->setToNow();
    mama_f64_t totalTime = (end->getEpochTimeMicroseconds() - begin->getEpochTimeMicroseconds());
    mama_quantity_t latency = totalTime/1000000;

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    std::cout << "\n" << test_info->name() << ": Latency = " << latency << "us \n";
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates a complex update msg and sends this mgs to orderBookListener                  "
               "\n repetitively (1000000), not processing entries                                       "
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


TEST_F (MamdaBookPerfTest, ComplexUpdateProcessEntriesLatency)
{
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener;
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (true);
    BookTickerCpu* ticker = new BookTickerCpu;
    mBookListener->addHandler (ticker);
    
    MamaMsg* initial = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_INITIAL, 2);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_UPDATE, 2);

    MamaDateTime* begin = new MamaDateTime();
    MamaDateTime* end = new MamaDateTime();
    begin->setToNow();
    
    for (int i=0; i<1000000; i++)
    {
        static int seqNum = 1;    
        update->updateU32(MamdaCommonFields::MSG_SEQ_NUM, seqNum);
        ticker->callMamdaOnMsg(mSubscription, *update);
        seqNum++;
    }
    
    end->setToNow();
    mama_f64_t totalTime = (end->getEpochTimeMicroseconds() - begin->getEpochTimeMicroseconds());
    mama_quantity_t latency = totalTime/1000000;

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    std::cout << "\n" << test_info->name() << ": Latency = " << latency << "us \n";
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates a complex update msg and sends this mgs to orderBookListener                  "
               "\n repetitively (1000000), processing entries                                       "
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


TEST_F (MamdaBookPerfTest, ComplexUpdateFiveLevelsNoProcessEntriesLatency)
{
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener;
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (false);
    BookTickerCpu* ticker = new BookTickerCpu;
    mBookListener->addHandler (ticker);
    
    MamaMsg* initial = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_INITIAL, 5);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_UPDATE, 5);

    MamaDateTime* begin = new MamaDateTime();
    MamaDateTime* end = new MamaDateTime();
    begin->setToNow();
    
    for (int i=0; i<1000000; i++)
    {
        static int seqNum = 1;    
        update->updateU32(MamdaCommonFields::MSG_SEQ_NUM, seqNum);
        ticker->callMamdaOnMsg(mSubscription, *update);
        seqNum++;
    }
    
    end->setToNow();
    mama_f64_t totalTime = (end->getEpochTimeMicroseconds() - begin->getEpochTimeMicroseconds());
    mama_quantity_t latency = totalTime/1000000;

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    std::cout << "\n" << test_info->name() << ": Latency = " << latency << "us \n";
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates a book of Five levels and updates all five levels in book          "
               "\n repetitively (1000000), not processing entries                                       "
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


TEST_F (MamdaBookPerfTest, ComplexUpdateFiveLevelsProcessEntriesLatency)
{
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener;
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (true);
    BookTickerCpu* ticker = new BookTickerCpu;
    mBookListener->addHandler (ticker);
    
    MamaMsg* initial = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_INITIAL, 5);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_UPDATE, 5);
    
    MamaDateTime* begin = new MamaDateTime();
    MamaDateTime* end = new MamaDateTime();
    begin->setToNow();
    
    for (int i=0; i<1000000; i++)
    {
        static int seqNum = 1;    
        update->updateU32(MamdaCommonFields::MSG_SEQ_NUM, seqNum);
        ticker->callMamdaOnMsg(mSubscription, *update);
        seqNum++;
    }
    
    end->setToNow();
    mama_f64_t totalTime = (end->getEpochTimeMicroseconds() - begin->getEpochTimeMicroseconds());
    mama_quantity_t latency = totalTime/1000000;

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    std::cout << "\n" << test_info->name() << ": Latency = " << latency << "us \n";
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates a book of Five levels and updates all five levels in book          "
               "\n repetitively (1000000), processing entries                                       "
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


TEST_F (MamdaBookPerfTest, ComplexUpdateFiveLevelsTwoLevelUpdateNoProcessEntriesLatency)
{
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener;
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (false);
    BookTickerCpu* ticker = new BookTickerCpu;
    mBookListener->addHandler (ticker);
    
    MamaMsg* initial = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_INITIAL, 5);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_UPDATE, 2);

    MamaDateTime* begin = new MamaDateTime();
    MamaDateTime* end = new MamaDateTime();
    begin->setToNow();
    
    for (int i=0; i<1000000; i++)
    {
        static int seqNum = 1;    
        update->updateU32(MamdaCommonFields::MSG_SEQ_NUM, seqNum);
        ticker->callMamdaOnMsg(mSubscription, *update);
        seqNum++;
    }
    
    end->setToNow();
    mama_f64_t totalTime = (end->getEpochTimeMicroseconds() - begin->getEpochTimeMicroseconds());
    mama_quantity_t latency = totalTime/1000000;

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    std::cout << "\n" << test_info->name() << ": Latency = " << latency << "us \n";
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates a book of Five levels and updates top two levels in book           "
               "\n repetitively (1000000), processing entries                                           "
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


TEST_F (MamdaBookPerfTest, ComplexUpdateFiveLevelsTwoLevelUpdateProcessEntriesLatency)
{
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener;
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (true);
    BookTickerCpu* ticker = new BookTickerCpu;
    mBookListener->addHandler (ticker);
    
    MamaMsg* initial = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_INITIAL, 5);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_UPDATE, 2);

    MamaDateTime* begin = new MamaDateTime();
    MamaDateTime* end = new MamaDateTime();
    begin->setToNow();
    
    for (int i=0; i<1000000; i++)
    {
        static int seqNum = 1;    
        update->updateU32(MamdaCommonFields::MSG_SEQ_NUM, seqNum);
        ticker->callMamdaOnMsg(mSubscription, *update);
        seqNum++;
    }
    
    end->setToNow();
    mama_f64_t totalTime = (end->getEpochTimeMicroseconds() - begin->getEpochTimeMicroseconds());
    mama_quantity_t latency = totalTime/1000000;

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    std::cout << "\n" << test_info->name() << ": Latency = " << latency << "us \n";
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates a book of Five levels and updates top two levels in book           "
               "\n repetitively (1000000), processing entries                                           "
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


TEST_F (MamdaBookPerfTest, ComplexUpdateTenLevelsThreeLevelUpdateNoProcessEntrieLatency)
{
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener;
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (false);
    BookTickerCpu* ticker = new BookTickerCpu;
    mBookListener->addHandler (ticker);
    
    MamaMsg* initial = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_INITIAL, 10);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_UPDATE, 3);

    MamaDateTime* begin = new MamaDateTime();
    MamaDateTime* end = new MamaDateTime();
    begin->setToNow();
    
    for (int i=0; i<1000000; i++)
    {
        static int seqNum = 1;    
        update->updateU32(MamdaCommonFields::MSG_SEQ_NUM, seqNum);
        ticker->callMamdaOnMsg(mSubscription, *update);
        seqNum++;
    }
    
    end->setToNow();
    mama_f64_t totalTime = (end->getEpochTimeMicroseconds() - begin->getEpochTimeMicroseconds());
    mama_quantity_t latency = totalTime/1000000;

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    std::cout << "\n" << test_info->name() << ": Latency = " << latency << "us \n";
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates a book of ten levels and updates top three levels in book           "
               "\n repetitively (1000000), processing entries                                           "
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


TEST_F (MamdaBookPerfTest, ComplexUpdateTenLevelsThreeLevelUpdateProcessEntriesLatency)
{
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener;
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (true);
    BookTickerCpu* ticker = new BookTickerCpu;
    mBookListener->addHandler (ticker);
    
    MamaMsg* initial = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_INITIAL, 10);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsgOneSide(MAMA_MSG_TYPE_BOOK_UPDATE, 3);
 
    MamaDateTime* begin = new MamaDateTime();
    MamaDateTime* end = new MamaDateTime();
    begin->setToNow();
    
    for (int i=0; i<1000000; i++)
    {
        static int seqNum = 1;    
        update->updateU32(MamdaCommonFields::MSG_SEQ_NUM, seqNum);
        ticker->callMamdaOnMsg(mSubscription, *update);
        seqNum++;
    }

    
    end->setToNow();
    mama_f64_t totalTime = (end->getEpochTimeMicroseconds() - begin->getEpochTimeMicroseconds());
    mama_quantity_t latency = totalTime/1000000;

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    std::cout << "\n" << test_info->name() << ": Latency = " << latency << "us \n";
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates a book of ten levels and updates top three levels in book           "
               "\n repetitively (1000000), processing entries                                           "
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


/***********************  TWO SIDED BOOK **********************************/


TEST_F (MamdaBookPerfTest, RecapNoProcessEntriesTwoSidedBook)
{
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener;
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (false);
    BookTickerCpu* ticker = new BookTickerCpu;
	mBookListener->addHandler (ticker);

    MamaMsg* msg = createBookMsg(MAMA_MSG_TYPE_BOOK_INITIAL, 1);

    START_RECORDING_CPU(1000000);
    ticker->callMamdaOnMsg(mSubscription, *msg);

    STOP_RECORDING_CPU ();

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates a bid and ask side recap msg and sends this mgs to orderBookListener                  "
               "\n repetitively (1000000), not processing entries                                       "
               "\n ********************************************************************************** \n",               
               test_info->name());
    }
    
    delete ticker;
    ticker = NULL;
    delete msg;
    msg = NULL;
}

TEST_F (MamdaBookPerfTest, RecapProcessEntriesTwoSidedBook)
{
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener;
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (true);
    BookTickerCpu* ticker = new BookTickerCpu;
    mBookListener->addHandler (ticker);

    MamaMsg* msg = createBookMsg(MAMA_MSG_TYPE_BOOK_INITIAL, 1);

    START_RECORDING_CPU(1000000);
    ticker->callMamdaOnMsg(mSubscription, *msg);
    STOP_RECORDING_CPU ();

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates a bid and ask side recap msg and sends this mgs to orderBookListener"
               "\n repetitively (1000000), processing entries                                       "
               "\n ********************************************************************************** \n",               
               test_info->name());
    }
    
    delete ticker;
    ticker = NULL;
    delete msg;
    msg = NULL;
}

TEST_F (MamdaBookPerfTest, SimpleUpdateNoProcessEntriesTwoSidedBook)
{
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener;
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (false);
    BookTickerCpu* ticker = new BookTickerCpu;
    mBookListener->addHandler (ticker);
    
    MamaMsg* initial = createBookMsg(MAMA_MSG_TYPE_BOOK_INITIAL, 1);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsg(MAMA_MSG_TYPE_BOOK_UPDATE, 1);

    START_RECORDING_CPU(1000000);
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
               "\n This test creates a bid and ask side update msg and sends this mgs to orderBookListener                  "
               "\n repetitively (1000000), not processing entries                                       "
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

TEST_F (MamdaBookPerfTest, SimpleUpdateProcessEntriesTwoSidedBook)
{
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener;
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (true);
    BookTickerCpu* ticker = new BookTickerCpu;
    mBookListener->addHandler (ticker);
    
    MamaMsg* initial = createBookMsg(MAMA_MSG_TYPE_BOOK_INITIAL, 1);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsg(MAMA_MSG_TYPE_BOOK_UPDATE, 1);
    
    START_RECORDING_CPU(1000000);
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
               "\n This test creates a a bid and ask side update msg and sends this mgs to orderBookListener                  "
               "\n repetitively (1000000), processing entries                                       "
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


TEST_F (MamdaBookPerfTest, ComplexUpdateNoProcessEntriesTwoSidedBook)
{
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener;
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (false);
    BookTickerCpu* ticker = new BookTickerCpu;
    mBookListener->addHandler (ticker);
    
    MamaMsg* initial = createBookMsg(MAMA_MSG_TYPE_BOOK_INITIAL, 2);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsg(MAMA_MSG_TYPE_BOOK_UPDATE, 2);
    
    START_RECORDING_CPU(1000000);
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
               "\n This test creates a complex a bid and ask side update msg and sends this mgs to orderBookListener                  "
               "\n repetitively (1000000), not processing entries                                       "
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


TEST_F (MamdaBookPerfTest, ComplexUpdateProcessEntriesTwoSidedBook)
{
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener;
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (true);
    BookTickerCpu* ticker = new BookTickerCpu;
    mBookListener->addHandler (ticker);
    
    MamaMsg* initial = createBookMsg(MAMA_MSG_TYPE_BOOK_INITIAL, 2);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsg(MAMA_MSG_TYPE_BOOK_UPDATE, 2);

    START_RECORDING_CPU(1000000);
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
               "\n This test creates a complex a bid and ask side update msg and sends this mgs to orderBookListener                  "
               "\n repetitively (1000000), processing entries                                       "
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


TEST_F (MamdaBookPerfTest, ComplexUpdateFiveLevelsNoProcessEntriesTwoSidedBook)
{
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener;
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (false);
    BookTickerCpu* ticker = new BookTickerCpu;
    mBookListener->addHandler (ticker);
    
    MamaMsg* initial = createBookMsg(MAMA_MSG_TYPE_BOOK_INITIAL, 5);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsg(MAMA_MSG_TYPE_BOOK_UPDATE, 5);

    START_RECORDING_CPU(1000000);
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
               "\n This test creates a book  of Five levels, on each siden and updates all five levels  "
               "\n in each side of the book repetitively (1000000), not processing entries              "
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


TEST_F (MamdaBookPerfTest, ComplexUpdateFiveLevelsProcessEntriesTwoSidedBook)
{
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener;
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (true);
    BookTickerCpu* ticker = new BookTickerCpu;
    mBookListener->addHandler (ticker);
    
    MamaMsg* initial = createBookMsg(MAMA_MSG_TYPE_BOOK_INITIAL, 5);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsg(MAMA_MSG_TYPE_BOOK_UPDATE, 5);

    START_RECORDING_CPU(1000000);
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
               "\n This test creates a book  of Five levels, on each siden and updates all five levels  "
               "\n in each side of the book repetitively (1000000), processing entries                  "
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


TEST_F (MamdaBookPerfTest, ComplexUpdateFiveLevelsTwoLevelUpdateNoProcessEntriesTwoSidedBook)
{
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener;
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (false);
    BookTickerCpu* ticker = new BookTickerCpu;
    mBookListener->addHandler (ticker);
    
    MamaMsg* initial = createBookMsg(MAMA_MSG_TYPE_BOOK_INITIAL, 5);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsg(MAMA_MSG_TYPE_BOOK_UPDATE, 2);

    START_RECORDING_CPU(1000000);
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
               "\n This test creates a book  of Five levels, on each side and updates two levels  "
               "\n in each side of the book repetitively (1000000), not processing entries              "
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


TEST_F (MamdaBookPerfTest, ComplexUpdateFiveLevelsTwoLevelUpdateProcessEntriesTwoSidedBook)
{
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener;
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (true);
    BookTickerCpu* ticker = new BookTickerCpu;
    mBookListener->addHandler (ticker);
    
    MamaMsg* initial = createBookMsg(MAMA_MSG_TYPE_BOOK_INITIAL, 5);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsg(MAMA_MSG_TYPE_BOOK_UPDATE, 2);

    START_RECORDING_CPU(1000000);
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
               "\n This test creates a book  of Five levels, on each side and updates two levels  "
               "\n in each side of the book repetitively (1000000), processing entries              "
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


TEST_F (MamdaBookPerfTest, ComplexUpdateTenLevelsThreeLevelUpdateNoProcessEntriesTwoSidedBook)
{
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener;
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (false);
    BookTickerCpu* ticker = new BookTickerCpu;
    mBookListener->addHandler (ticker);
    
    MamaMsg* initial = createBookMsg(MAMA_MSG_TYPE_BOOK_INITIAL, 10);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsg(MAMA_MSG_TYPE_BOOK_UPDATE, 3);

    START_RECORDING_CPU(1000000);
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
               "\n This test creates a book  of ten levels, on each side and updates thress levels  "
               "\n in each side of the book repetitively (1000000), not processing entries              "
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


TEST_F (MamdaBookPerfTest, ComplexUpdateTenLevelsThreeLevelUpdateProcessEntriesTwoSidedBook)
{
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener;
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (true);
    BookTickerCpu* ticker = new BookTickerCpu;
    mBookListener->addHandler (ticker);
    
    MamaMsg* initial = createBookMsg(MAMA_MSG_TYPE_BOOK_INITIAL, 10);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsg(MAMA_MSG_TYPE_BOOK_UPDATE, 3);

    START_RECORDING_CPU(1000000);
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
               "\n This test creates a book  of ten levels, on each side and updates thress levels  "
               "\n in each side of the book repetitively (1000000), processing entries              "
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


/******************* LATENCY TESTS BELOW **************************************/


TEST_F (MamdaBookPerfTest, RecapNoProcessEntriesLatencyTwoSidedBook)
{
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener;
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (false);
    BookTickerCpu* ticker = new BookTickerCpu;
    mBookListener->addHandler (ticker);

    MamaMsg* msg = createBookMsg(MAMA_MSG_TYPE_BOOK_INITIAL, 1);

    MamaDateTime* begin = new MamaDateTime();
    MamaDateTime* end = new MamaDateTime();
    begin->setToNow();
    
    for (int i=0; i<1000000; i++)
    {
        ticker->callMamdaOnMsg(mSubscription, *msg);
    }
    
    end->setToNow();
    mama_f64_t totalTime = (end->getEpochTimeMicroseconds() - begin->getEpochTimeMicroseconds());
    mama_quantity_t latency = totalTime/1000000;

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    std::cout << "\n" << test_info->name() << ": Latency = " << latency << "us \n";
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates bid and ask single level recap msg and sends this mgs to orderBookListener                  "
               "\n repetitively (1000000), not processing entries                                       "
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

TEST_F (MamdaBookPerfTest, RecapProcessEntriesLatencyTwoSidedBook)
{
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener;
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (true);
    BookTickerCpu* ticker = new BookTickerCpu;
    mBookListener->addHandler (ticker);

    MamaMsg* msg = createBookMsg(MAMA_MSG_TYPE_BOOK_INITIAL, 1);
       
    MamaDateTime* begin = new MamaDateTime();
    MamaDateTime* end = new MamaDateTime();
    begin->setToNow();
    
    for (int i=0; i<1000000; i++)
    {
        ticker->callMamdaOnMsg(mSubscription, *msg);
    }
    
    end->setToNow();
    mama_f64_t totalTime = (end->getEpochTimeMicroseconds() - begin->getEpochTimeMicroseconds());
    mama_quantity_t latency = totalTime/1000000;

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    std::cout << "\n" << test_info->name() << ": Latency = " << latency << "us \n";
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates bid and ask single level recap msg and sends this mgs to orderBookListener                  "
               "\n repetitively (1000000), processing entries                                       "
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


TEST_F (MamdaBookPerfTest, SimpleUpdateNoProcessEntriesLatencyTwoSidedBook)
{
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener;
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (false);
    BookTickerCpu* ticker = new BookTickerCpu;
    mBookListener->addHandler (ticker);
    
    MamaMsg* initial = createBookMsg(MAMA_MSG_TYPE_BOOK_INITIAL, 1);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsg(MAMA_MSG_TYPE_BOOK_UPDATE, 1);

    MamaDateTime* begin = new MamaDateTime();
    MamaDateTime* end = new MamaDateTime();
    begin->setToNow();
    
    for (int i=0; i<1000000; i++)
    {
        static int seqNum = 1;
        update->updateU32(MamdaCommonFields::MSG_SEQ_NUM, seqNum);
        ticker->callMamdaOnMsg(mSubscription, *update);
        seqNum++;
    }

    end->setToNow();
    mama_f64_t totalTime = (end->getEpochTimeMicroseconds() - begin->getEpochTimeMicroseconds());
    mama_quantity_t latency = totalTime/1000000;

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    std::cout << "\n" << test_info->name() << ": Latency = " << latency << "us \n";
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates a bid and ask update msg and sends this mgs to orderBookListener                  "
               "\n repetitively (1000000), not processing entries                                       "
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


TEST_F (MamdaBookPerfTest, SimpleUpdateProcessEntriesLatencyTwoSidedBook)
{
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener;
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (true);
    BookTickerCpu* ticker = new BookTickerCpu;
    mBookListener->addHandler (ticker);
    
    MamaMsg* initial = createBookMsg(MAMA_MSG_TYPE_BOOK_INITIAL, 1);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsg(MAMA_MSG_TYPE_BOOK_UPDATE, 1);

    MamaDateTime* begin = new MamaDateTime();
    MamaDateTime* end = new MamaDateTime();
    begin->setToNow();
    
    for (int i=0; i<1000000; i++)
    {
        static int seqNum = 1;    
        update->updateU32(MamdaCommonFields::MSG_SEQ_NUM, seqNum);
        ticker->callMamdaOnMsg(mSubscription, *update);
        seqNum++;
    }
    
    end->setToNow();
    mama_f64_t totalTime = (end->getEpochTimeMicroseconds() - begin->getEpochTimeMicroseconds());
    mama_quantity_t latency = totalTime/1000000;

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    std::cout << "\n" << test_info->name() << ": Latency = " << latency << "us \n";
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates a bid and ask update msg and sends this mgs to orderBookListener                  "
               "\n repetitively (1000000), processing entries                                       "
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


TEST_F (MamdaBookPerfTest, ComplexUpdateNoProcessEntriesLatencyTwoSidedBook)
{
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener;
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (false);
    BookTickerCpu* ticker = new BookTickerCpu;
    mBookListener->addHandler (ticker);
    
    MamaMsg* initial = createBookMsg(MAMA_MSG_TYPE_BOOK_INITIAL, 2);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsg(MAMA_MSG_TYPE_BOOK_UPDATE, 2);
    
    MamaDateTime* begin = new MamaDateTime();
    MamaDateTime* end = new MamaDateTime();
    begin->setToNow();
    
    for (int i=0; i<1000000; i++)
    {
        static int seqNum = 1;    
        update->updateU32(MamdaCommonFields::MSG_SEQ_NUM, seqNum);
        ticker->callMamdaOnMsg(mSubscription, *update);
        seqNum++;
    }
    
    end->setToNow();
    mama_f64_t totalTime = (end->getEpochTimeMicroseconds() - begin->getEpochTimeMicroseconds());
    mama_quantity_t latency = totalTime/1000000;

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    std::cout << "\n" << test_info->name() << ": Latency = " << latency << "us \n";
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates a complex bid and ask update msg and sends this mgs to orderBookListener                  "
               "\n repetitively (1000000), not processing entries                                       "
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


TEST_F (MamdaBookPerfTest, ComplexUpdateProcessEntriesLatencyTwoSidedBook)
{
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener;
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (true);
    BookTickerCpu* ticker = new BookTickerCpu;
    mBookListener->addHandler (ticker);
    
    MamaMsg* initial = createBookMsg(MAMA_MSG_TYPE_BOOK_INITIAL, 2);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsg(MAMA_MSG_TYPE_BOOK_UPDATE, 2);

    MamaDateTime* begin = new MamaDateTime();
    MamaDateTime* end = new MamaDateTime();
    begin->setToNow();
    
    for (int i=0; i<1000000; i++)
    {
        static int seqNum = 1;
        update->updateU32(MamdaCommonFields::MSG_SEQ_NUM, seqNum);
        ticker->callMamdaOnMsg(mSubscription, *update);
        seqNum++;
    }
    
    end->setToNow();
    mama_f64_t totalTime = (end->getEpochTimeMicroseconds() - begin->getEpochTimeMicroseconds());
    mama_quantity_t latency = totalTime/1000000;

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    std::cout << "\n" << test_info->name() << ": Latency = " << latency << "us \n";
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates a complex bid and ask update msg and sends this mgs to orderBookListener                  "
               "\n repetitively (1000000), processing entries                                       "
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


TEST_F (MamdaBookPerfTest, ComplexUpdateFiveLevelsNoProcessEntriesLatencyTwoSidedBook)
{
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener;
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (false);
    BookTickerCpu* ticker = new BookTickerCpu;
    mBookListener->addHandler (ticker);
    
    MamaMsg* initial = createBookMsg(MAMA_MSG_TYPE_BOOK_INITIAL, 5);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsg(MAMA_MSG_TYPE_BOOK_UPDATE, 5);

    MamaDateTime* begin = new MamaDateTime();
    MamaDateTime* end = new MamaDateTime();
    begin->setToNow();
    
    for (int i=0; i<1000000; i++)
    {
        static int seqNum = 1;    
        update->updateU32(MamdaCommonFields::MSG_SEQ_NUM, seqNum);
        ticker->callMamdaOnMsg(mSubscription, *update);
        seqNum++;
    }
    
    end->setToNow();
    mama_f64_t totalTime = (end->getEpochTimeMicroseconds() - begin->getEpochTimeMicroseconds());
    mama_quantity_t latency = totalTime/1000000;

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    std::cout << "\n" << test_info->name() << ": Latency = " << latency << "us \n";
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates a book of Five levels, on each side, and updates all five levels   "
               "\n on each side of the book repetitively (1000000), not processing entries              "
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


TEST_F (MamdaBookPerfTest, ComplexUpdateFiveLevelsProcessEntriesLatencyTwoSidedBook)
{
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener;
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (true);
    BookTickerCpu* ticker = new BookTickerCpu;
    mBookListener->addHandler (ticker);
    
    MamaMsg* initial = createBookMsg(MAMA_MSG_TYPE_BOOK_INITIAL, 5);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsg(MAMA_MSG_TYPE_BOOK_UPDATE, 5);
    
    MamaDateTime* begin = new MamaDateTime();
    MamaDateTime* end = new MamaDateTime();
    begin->setToNow();
    
    for (int i=0; i<1000000; i++)
    {
        static int seqNum = 1;    
        update->updateU32(MamdaCommonFields::MSG_SEQ_NUM, seqNum);
        ticker->callMamdaOnMsg(mSubscription, *update);
        seqNum++;
    }
    
    end->setToNow();
    mama_f64_t totalTime = (end->getEpochTimeMicroseconds() - begin->getEpochTimeMicroseconds());
    mama_quantity_t latency = totalTime/1000000;

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    std::cout << "\n" << test_info->name() << ": Latency = " << latency << "us \n";
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates a book of Five levels, on each side, and updates all five levels   "
               "\n on each side of the book repetitively (1000000), not processing entries              "
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


TEST_F (MamdaBookPerfTest, ComplexUpdateFiveLevelsTwoLevelUpdateNoProcessEntriesLatencyTwoSidedBook)
{
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener;
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (false);
    BookTickerCpu* ticker = new BookTickerCpu;
    mBookListener->addHandler (ticker);
    
    MamaMsg* initial = createBookMsg(MAMA_MSG_TYPE_BOOK_INITIAL, 5);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsg(MAMA_MSG_TYPE_BOOK_UPDATE, 2);

    MamaDateTime* begin = new MamaDateTime();
    MamaDateTime* end = new MamaDateTime();
    begin->setToNow();
    
    for (int i=0; i<1000000; i++)
    {
        static int seqNum = 1;    
        update->updateU32(MamdaCommonFields::MSG_SEQ_NUM, seqNum);
        ticker->callMamdaOnMsg(mSubscription, *update);
        seqNum++;
    }
    
    end->setToNow();
    mama_f64_t totalTime = (end->getEpochTimeMicroseconds() - begin->getEpochTimeMicroseconds());
    mama_quantity_t latency = totalTime/1000000;

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    std::cout << "\n" << test_info->name() << ": Latency = " << latency << "us \n";
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates a book of Five levels, on each side, and updates top two levels   "
               "\n on each side of the book repetitively (1000000), not processing entries              "
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


TEST_F (MamdaBookPerfTest, ComplexUpdateFiveLevelsTwoLevelUpdateProcessEntriesLatencyTwoSidedBook)
{
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener;
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (true);
    BookTickerCpu* ticker = new BookTickerCpu;
    mBookListener->addHandler (ticker);
    
    MamaMsg* initial = createBookMsg(MAMA_MSG_TYPE_BOOK_INITIAL, 5);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsg(MAMA_MSG_TYPE_BOOK_UPDATE, 2);

    MamaDateTime* begin = new MamaDateTime();
    MamaDateTime* end = new MamaDateTime();
    begin->setToNow();
    
    for (int i=0; i<1000000; i++)
    {
        static int seqNum = 1;    
        update->updateU32(MamdaCommonFields::MSG_SEQ_NUM, seqNum);
        ticker->callMamdaOnMsg(mSubscription, *update);
        seqNum++;
    }
    
    end->setToNow();
    mama_f64_t totalTime = (end->getEpochTimeMicroseconds() - begin->getEpochTimeMicroseconds());
    mama_quantity_t latency = totalTime/1000000;

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    std::cout << "\n" << test_info->name() << ": Latency = " << latency << "us \n";
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates a book of Five levels, on each side, and updates top two levels   "
               "\n on each side of the book repetitively (1000000), processing entries              "
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


TEST_F (MamdaBookPerfTest, ComplexUpdateTenLevelsThreeLevelUpdateNoProcessEntrieLatencyTwoSidedBook)
{
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener;
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (false);
    BookTickerCpu* ticker = new BookTickerCpu;
    mBookListener->addHandler (ticker);
    
    MamaMsg* initial = createBookMsg(MAMA_MSG_TYPE_BOOK_INITIAL, 10);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsg(MAMA_MSG_TYPE_BOOK_UPDATE, 3);

    MamaDateTime* begin = new MamaDateTime();
    MamaDateTime* end = new MamaDateTime();
    begin->setToNow();
    
    for (int i=0; i<1000000; i++)
    {
        static int seqNum = 1;    
        update->updateU32(MamdaCommonFields::MSG_SEQ_NUM, seqNum);
        ticker->callMamdaOnMsg(mSubscription, *update);
        seqNum++;
    }
    
    end->setToNow();
    mama_f64_t totalTime = (end->getEpochTimeMicroseconds() - begin->getEpochTimeMicroseconds());
    mama_quantity_t latency = totalTime/1000000;

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    std::cout << "\n" << test_info->name() << ": Latency = " << latency << "us \n";
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates a book of ten levels, on each side, and updates top three levels   "
               "\n on each side of the book repetitively (1000000), not processing entries              "
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


TEST_F (MamdaBookPerfTest, ComplexUpdateTenLevelsThreeLevelUpdateProcessEntriesLatencyTwoSidedBook)
{
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener;
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (true);
    BookTickerCpu* ticker = new BookTickerCpu;
    mBookListener->addHandler (ticker);
    
    MamaMsg* initial = createBookMsg(MAMA_MSG_TYPE_BOOK_INITIAL, 10);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    MamaMsg* update = createBookMsg(MAMA_MSG_TYPE_BOOK_UPDATE, 3);
 
    MamaDateTime* begin = new MamaDateTime();
    MamaDateTime* end = new MamaDateTime();
    begin->setToNow();
    
    for (int i=0; i<1000000; i++)
    {
        static int seqNum = 1;    
        update->updateU32(MamdaCommonFields::MSG_SEQ_NUM, seqNum);
        ticker->callMamdaOnMsg(mSubscription, *update);
        seqNum++;
    }

    end->setToNow();
    mama_f64_t totalTime = (end->getEpochTimeMicroseconds() - begin->getEpochTimeMicroseconds());
    mama_quantity_t latency = totalTime/1000000;

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    std::cout << "\n" << test_info->name() << ": Latency = " << latency << "us \n";
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates a book of ten levels, on each side, and updates top three levels   "
               "\n on each side of the book repetitively (1000000), not processing entries              "
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
