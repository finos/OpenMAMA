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
#include <gtest/gtest-spi.h>
#include <mamda/MamdaOrderBookFields.h>
#include <mama/mamacpp.h>
#include <mamda/MamdaSubscription.h>
#include <mamda/MamdaCommonFields.h>
#include <mamda/MamdaOrderBookHandler.h>
#include <mamda/MamdaOrderBookListener.h>
#include <mamda/MamdaOrderBook.h>
#include <mamda/MamdaOrderBookPriceLevel.h>
#include <mamda/MamdaOrderBookEntry.h>
#include <mamda/MamdaOrderBookTypes.h>
#include <mama/MamaDictionary.h>

#include "../../../../../mama/c_cpp/src/c/playback/playbackpublisher.h"
#include "../../../../../mama/c_cpp/src/c/playback/playbackcapture.h"
#include "../../../../../mama/c_cpp/src/c/playback/playbackFileParser.h"

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
#include <fstream>


using namespace Wombat;
using namespace std;

class BookTicker : public MamdaOrderBookHandler
{
public:
    BookTicker () {}
    virtual ~BookTicker () {}
	
    //ofstream		std::cout;

	MamdaOrderBook*	myOrderBook;

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

	void onBookRecap (
        MamdaSubscription*                 subscription,
        MamdaOrderBookListener&            listener,
        const MamaMsg*                     msg,
        const MamdaOrderBookComplexDelta*  delta,
        const MamdaOrderBookRecap&         recap,
        const MamdaOrderBook&              book)
    {
       	//std::cout << "Book recap" << "\n";
 		//book.dump(std::cout);
		ASSERT_NO_THROW(myOrderBook->copy(book););
 		//myOrderBook->dump(std::cout);
		ASSERT_NO_THROW(myOrderBook->assertEqual(book););
    }
    
    void onBookDelta (
        MamdaSubscription*                 subscription,
        MamdaOrderBookListener&            listener,
        const MamaMsg*                     msg,
        const MamdaOrderBookSimpleDelta&   delta,
        const MamdaOrderBook&              book)
    {
//       	std::cout << "Book delta" << "\n";
// 		displayDelta (subscription, listener, msg, delta, book,
// 					  MamdaOrderBookPriceLevel::MAMDA_BOOK_LEVEL_LIMIT);
// 		book.dump(std::cout);
		ASSERT_NO_THROW(myOrderBook->apply(delta););
// 		myOrderBook->dump(std::cout);
		ASSERT_NO_THROW(myOrderBook->assertEqual(book););
    }
    
   void onBookComplexDelta (
        MamdaSubscription*                 subscription,
        MamdaOrderBookListener&            listener,
        const MamaMsg*                     msg,
        const MamdaOrderBookComplexDelta&  delta,
        const MamdaOrderBook&              book)
    {
       	//std::cout << "Book complex delta" << "\n";
 		//displayComplexDelta (subscription, listener, msg, delta, book,
 		//					 MamdaOrderBookPriceLevel::MAMDA_BOOK_LEVEL_LIMIT);
 		//book.dump(std::cout);
		ASSERT_NO_THROW(myOrderBook->apply(delta););
 		//myOrderBook->dump(std::cout);
		ASSERT_NO_THROW(myOrderBook->assertEqual(book););
    }
    void onBookClear (
        MamdaSubscription*          subscription,
        MamdaOrderBookListener&     listener,
        const MamaMsg*              msg,
        const MamdaOrderBookClear&  clear,
        const MamdaOrderBook&       book)
    {
 		//std::cout << "Book clear" << "\n";
		myOrderBook->clear();
    }

    void onBookGap (
        MamdaSubscription*          subscription,
        MamdaOrderBookListener&     listener,
        const MamaMsg*              msg,
        const MamdaOrderBookGap&    event,
        const MamdaOrderBook&       book)
    {
        //std::cout << "Book gap" << "\n";
    }
	
	void displayDelta (
        MamdaSubscription*                  subscription,
        MamdaOrderBookListener&             listener,
        const MamaMsg*                      msg,
        const MamdaOrderBookSimpleDelta&    delta,
        const MamdaOrderBook&               book,
        MamdaOrderBookPriceLevel::OrderType orderType)
    {
		mama_seqnum_t seqNum = (msg) ? msg->getSeqNum() : 0;
		std::cout << "DELTA!!!  (seq# " << seqNum << ")\n";
 		printDelta (delta);
    }
	
	void displayComplexDelta (
        MamdaSubscription*                  subscription,
        MamdaOrderBookListener&             listener,
        const MamaMsg*                      msg,
        const MamdaOrderBookComplexDelta&   delta,
        const MamdaOrderBook&               book,
        MamdaOrderBookPriceLevel::OrderType orderType)    
    {
		mama_seqnum_t seqNum = (msg) ? msg->getSeqNum() : 0;
		std::cout << "COMPLEX DELTA!!!  (seq# " << seqNum << ")\n";
		MamdaOrderBookComplexDelta::iterator end = delta.end();
		MamdaOrderBookComplexDelta::iterator i   = delta.begin();

		for (; i != end; ++i)
		{
			MamdaOrderBookBasicDelta*  basicDelta = *i;
			printDelta (*basicDelta);
		}
    }
	
    void printDelta (const MamdaOrderBookBasicDelta&  delta)
    {
		std::cout << delta.getPlDeltaSize () 
		<< " " << delta.getPriceLevel()->getPrice() 
		<< " " << delta.getPlDeltaAction() << "\n";
    }

};

class MamdaBookPlaybackDeltaTest : public ::testing::Test
{
protected:
	MamdaBookPlaybackDeltaTest () {}
	virtual ~MamdaBookPlaybackDeltaTest () {}
  
	virtual void SetUp()
	{
		mamaBridge bridge;
		bridge = Mama::loadBridge("wmw");
		Mama::open();
 		//mama_enableLogging (stderr, MAMA_LOG_LEVEL_FINEST);
		dictionary = new MamaDictionary;
		dictionary->populateFromFile("dictionary");
		MamdaCommonFields::setDictionary (*dictionary);
		MamdaOrderBookFields::setDictionary (*dictionary);
		
		mSubscription = new MamdaSubscription;
		mBookListener = new MamdaOrderBookListener;
		
		mSubscription->addMsgListener (mBookListener);
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
		
		if (mBookListener)
		{
		    delete mBookListener;
		    mBookListener = NULL;
		}
		
		if (dictionary)
		{
		    delete dictionary;
		    dictionary = NULL;
		}
	}
		
	MamaDictionary*      	dictionary;
	MamdaSubscription* 		mSubscription;
	MamdaOrderBookListener* mBookListener;
};

TEST_F (MamdaBookPlaybackDeltaTest, noEntries)
{
	const char* inputFile = Mama::getProperty("CTAInputFile");
	if (!inputFile) 
	{
		printf("\n CTA playback Data Input File, CTAInputFile, not specified in mama.properties \n");
		exit(1);
	}
	if (!fileExists(inputFile))
 	{
		printf("%s, specified in mama.properties does not exist. \n", inputFile);
		exit(1);
 	}
	
	BookTicker* ticker = new BookTicker;
	ticker->myOrderBook = new MamdaOrderBook();
	mBookListener->addHandler (ticker);
	mBookListener->setProcessEntries (false);
    
    
	playbackImpl* impl = (playbackImpl*) calloc(1, sizeof (playbackImpl));
	mamaMsg_create (&impl->myTempMsg);
	impl->myInputFileName = (char*) inputFile;
 	//ticker->std::cout.open("NoEntries.out");

	MamaMsg* newMessage  = NULL;
	newMessage = new MamaMsg();
	
	if (MAMA_STATUS_OK != mamaPlaybackFileParser_allocate (&impl->myParser))
	{
		printf("\nError: Could not create input file parser\n");
		cleanup(impl);
		exit(1);
	}


	if (MAMA_STATUS_OK!=mamaPlaybackFileParser_openFile(impl->myParser, impl->myInputFileName)) 
	{
		printf("\nError: Could not open input file (%s) for parsing\n", impl->myInputFileName);
		cleanup(impl);
		exit(1);
	}
		
	if (!mamaPlaybackFileParser_getNextHeader (impl->myParser, &impl->header))
    {
        printf ("\nError: No header information in binary playback"
                                " file.\n");
		cleanup(impl);
		exit(1);
    }
	   
	while (impl->header)
	{ 
		if (!mamaPlaybackFileParser_getNextMsg (impl->myParser,
												&impl->myTempMsg))
		{
			printf ("Error: Failed to get mamaMsg. For header %s\n",
					impl->header);
			cleanup(impl);
			exit(1);
		}
		newMessage->createFromMsg(impl->myTempMsg, false);
        
        ticker->callMamdaOnMsg(mSubscription, *newMessage);

        if(!mamaPlaybackFileParser_getNextHeader (impl->myParser, &impl->header))
            impl->header = NULL;
        
        const ::testing::TestInfo* const test_info =
            ::testing::UnitTest::GetInstance()->current_test_info();
        if (HasFailure())
        {
            std::cout << "\n Failed Msg = " << newMessage->toString()  << "\n";            
            std::cout << "Listener Book" << "\n";
            mBookListener->getOrderBook()->dump(std::cout);
            std::cout << "Delta Book" << "\n";
            ticker->myOrderBook->dump(std::cout);
            printf("\n **********************************************************************************   "
                "\n \t\t      **** %s failed ****                                                           "
                "\n This test maintains two books. The first book is maintained using the                   "
                "\n orderbooklistener and is populated with mamaMsg data. The second book is generated      "
                "\n based on the deltas from the first listener. These are applied to the seconds book      "
                "\n and the test then compares both books. The tests looks for exceptions that may be       "
                "\n thrown. It use a input file generated from a book subscription (full day) to a CTA feed."
                "\n In this test we DO NOT process Entries.                                                 "
                "\n ********************************************************************************** \n   ",               
                test_info->name());
                exit(0);
        }

	}

	if (ticker)
	{
		delete ticker;
		ticker = NULL;
	}
	if (newMessage)
	{
		delete newMessage;
		newMessage = NULL;
	}

	cleanup(impl);
}

TEST_F (MamdaBookPlaybackDeltaTest, processEntriesAndUseEntryManager)
{
    const char* inputFile = Mama::getProperty("CTAInputFile");
    if (!inputFile) 
    {
        printf("\n CTA playback Data Input File, CTAInputFile, not specified in mama.properties \n");
        exit(1);
    }
    if (!fileExists(inputFile))
    {
        printf("%s, specified in mama.properties does not exist. \n", inputFile);
        exit(1);
    }
    
    BookTicker* ticker = new BookTicker;
    ticker->myOrderBook = new MamdaOrderBook();
    mBookListener->addHandler (ticker);
    mBookListener->setProcessEntries (true);
    mBookListener->setUseEntryManager(true);
    
    playbackImpl* impl = (playbackImpl*) calloc(1, sizeof (playbackImpl));
    mamaMsg_create (&impl->myTempMsg);
    impl->myInputFileName = (char*) inputFile;
//  ticker->std::cout.open("NoEntries.out");

    MamaMsg* newMessage  = NULL;
    newMessage = new MamaMsg();
    
    if (MAMA_STATUS_OK != mamaPlaybackFileParser_allocate (&impl->myParser))
    {
        printf("\nError: Could not create input file parser\n");
        cleanup(impl);
        exit(1);
    }


    if (MAMA_STATUS_OK!=mamaPlaybackFileParser_openFile(impl->myParser, impl->myInputFileName)) 
    {
        printf("\nError: Could not open input file (%s) for parsing\n", impl->myInputFileName);
        cleanup(impl);
        exit(1);
    }
        
    if (!mamaPlaybackFileParser_getNextHeader (impl->myParser, &impl->header))
    {
        printf ("\nError: No header information in binary playback"
                                " file.\n");
        cleanup(impl);
        exit(1);
    }
       
    while (impl->header)
    { 
        if (!mamaPlaybackFileParser_getNextMsg (impl->myParser,
                                                &impl->myTempMsg))
        {
            printf ("Error: Failed to get mamaMsg. For header %s\n",
                    impl->header);
            cleanup(impl);
            exit(1);
        }
        newMessage->createFromMsg(impl->myTempMsg, false);
        
        ticker->callMamdaOnMsg(mSubscription, *newMessage);

        if(!mamaPlaybackFileParser_getNextHeader (impl->myParser, &impl->header))
            impl->header = NULL;
        
        const ::testing::TestInfo* const test_info =
            ::testing::UnitTest::GetInstance()->current_test_info();
        if (HasFailure())
        {
            std::cout << "\n Failed Msg = " << newMessage->toString()  << "\n";            
            std::cout << "Listener Book" << "\n";
            mBookListener->getOrderBook()->dump(std::cout);
            std::cout << "Delta Book" << "\n";
            ticker->myOrderBook->dump(std::cout);
            
            printf("\n **********************************************************************************   "
                "\n \t\t      **** %s failed ****                                                           "
                "\n This test maintains two books. The first book is maintained using the                   "
                "\n orderbooklistener and is populated with mamaMsg data. The second book is generated      "
                "\n based on the deltas from the first listener. These are applied to the seconds book      "
                "\n and the test then compares both books. The tests looks for exceptions that may be       "
                "\n thrown. It use a input file generated from a book subscription (full day) to a CTA feed."
                "\n In this test we process Entries.                                                        "
                "\n ********************************************************************************** \n   ",               
                test_info->name());
                exit(0);
        }

    }

    if (ticker)
    {
        delete ticker;
        ticker = NULL;
    }
    if (newMessage)
    {
        delete newMessage;
        newMessage = NULL;
    }

    cleanup(impl);
}


TEST_F (MamdaBookPlaybackDeltaTest, Mamda1478_processEntriesAndNotUseEntryManager)
{
    const char* inputFile = Mama::getProperty("CTAInputFile");
    if (!inputFile) 
    {
        printf("\n CTA playback Data Input File, CTAInputFile, not specified in mama.properties \n");
        exit(1);
    }
    if (!fileExists(inputFile))
    {
        printf("%s, specified in mama.properties does not exist. \n", inputFile);
        exit(1);
    }
    
    BookTicker* ticker = new BookTicker;
    ticker->myOrderBook = new MamdaOrderBook();
    mBookListener->addHandler (ticker);
    mBookListener->setProcessEntries (true);
    mBookListener->setUseEntryManager(false);
    
    playbackImpl* impl = (playbackImpl*) calloc(1, sizeof (playbackImpl));
    mamaMsg_create (&impl->myTempMsg);
    impl->myInputFileName = (char*) inputFile;
//  ticker->std::cout.open("NoEntries.out");

    MamaMsg* newMessage  = NULL;
    newMessage = new MamaMsg();
    
    if (MAMA_STATUS_OK != mamaPlaybackFileParser_allocate (&impl->myParser))
    {
        printf("\nError: Could not create input file parser\n");
        cleanup(impl);
        exit(1);
    }


    if (MAMA_STATUS_OK!=mamaPlaybackFileParser_openFile(impl->myParser, impl->myInputFileName)) 
    {
        printf("\nError: Could not open input file (%s) for parsing\n", impl->myInputFileName);
        cleanup(impl);
        exit(1);
    }
        
    if (!mamaPlaybackFileParser_getNextHeader (impl->myParser, &impl->header))
    {
        printf ("\nError: No header information in binary playback"
                                " file.\n");
        cleanup(impl);
        exit(1);
    }
       
    while (impl->header)
    { 
        if (!mamaPlaybackFileParser_getNextMsg (impl->myParser,
                                                &impl->myTempMsg))
        {
            printf ("Error: Failed to get mamaMsg. For header %s\n",
                    impl->header);
            cleanup(impl);
            exit(1);
        }
        newMessage->createFromMsg(impl->myTempMsg, false);
        
        ticker->callMamdaOnMsg(mSubscription, *newMessage);

        if(!mamaPlaybackFileParser_getNextHeader (impl->myParser, &impl->header))
            impl->header = NULL;
        
        const ::testing::TestInfo* const test_info =
            ::testing::UnitTest::GetInstance()->current_test_info();
        if (HasFailure())
        {
            std::cout << "\n Failed Msg = " << newMessage->toString()  << "\n";            
            std::cout << "Listener Book" << "\n";
            mBookListener->getOrderBook()->dump(std::cout);
            std::cout << "Delta Book" << "\n";
            ticker->myOrderBook->dump(std::cout);
            
            printf("\n **********************************************************************************   "
                "\n \t\t      **** %s failed ****                                                           "
                "\n This test maintains two books. The first book is maintained using the                   "
                "\n orderbooklistener and is populated with mamaMsg data. The second book is generated      "
                "\n based on the deltas from the first listener. These are applied to the second book       "
                "\n and the test then compares both books. The test looks for exceptions that may be        "
                "\n thrown. It use a input file generated from a book subscription (full day) to a CTA feed."
                "\n This test processes Entries and DOES NOT use the Entry Manager                          "
                "\n ********************************************************************************** \n   ",               
                test_info->name());
                exit(0);
        }

    }

    if (ticker)
    {
        delete ticker;
        ticker = NULL;
    }
    if (newMessage)
    {
        delete newMessage;
        newMessage = NULL;
    }

    cleanup(impl);
}


TEST_F (MamdaBookPlaybackDeltaTest, Mamda1479_processEntriesAndNotUseEntryManagerAndSetStrictChecking)
{
    const char* inputFile = Mama::getProperty("CTAInputFile");
    if (!inputFile) 
    {
        printf("\n CTA playback Data Input File, CTAInputFile, not specified in mama.properties \n");
        exit(1);
    }
    if (!fileExists(inputFile))
    {
        printf("%s, specified in mama.properties does not exist. \n", inputFile);
        exit(1);
    }
    
    BookTicker* ticker = new BookTicker;
    ticker->myOrderBook = new MamdaOrderBook();
    mBookListener->addHandler (ticker);
    mBookListener->setProcessEntries (true);
    mBookListener->setUseEntryManager(false);
    mBookListener->getOrderBook()->setStrictChecking(true);
    ticker->myOrderBook->setStrictChecking(true);
    
    playbackImpl* impl = (playbackImpl*) calloc(1, sizeof (playbackImpl));
    mamaMsg_create (&impl->myTempMsg);
    impl->myInputFileName = (char*) inputFile;
//  ticker->std::cout.open("NoEntries.out");

    MamaMsg* newMessage  = NULL;
    newMessage = new MamaMsg();
    
    if (MAMA_STATUS_OK != mamaPlaybackFileParser_allocate (&impl->myParser))
    {
        printf("\nError: Could not create input file parser\n");
        cleanup(impl);
        exit(1);
    }

    if (MAMA_STATUS_OK!=mamaPlaybackFileParser_openFile(impl->myParser, impl->myInputFileName)) 
    {
        printf("\nError: Could not open input file (%s) for parsing\n", impl->myInputFileName);
        cleanup(impl);
        exit(1);
    }
        
    if (!mamaPlaybackFileParser_getNextHeader (impl->myParser, &impl->header))
    {
        printf ("\nError: No header information in binary playback"
                                " file.\n");
        cleanup(impl);
        exit(1);
    }
       
    while (impl->header)
    { 
        if (!mamaPlaybackFileParser_getNextMsg (impl->myParser,
                                                &impl->myTempMsg))
        {
            printf ("Error: Failed to get mamaMsg. For header %s\n",
                    impl->header);
            cleanup(impl);
            exit(1);
        }
        newMessage->createFromMsg(impl->myTempMsg, false);
        
        ticker->callMamdaOnMsg(mSubscription, *newMessage);

        if(!mamaPlaybackFileParser_getNextHeader (impl->myParser, &impl->header))
            impl->header = NULL;
        
        const ::testing::TestInfo* const test_info =
            ::testing::UnitTest::GetInstance()->current_test_info();
        if (HasFailure())
        {
            std::cout << "\n Failed Msg = " << newMessage->toString()  << "\n";            
            std::cout << "Listener Book" << "\n";
            mBookListener->getOrderBook()->dump(std::cout);
            std::cout << "Delta Book" << "\n";
            ticker->myOrderBook->dump(std::cout);
            
            printf("\n **********************************************************************************   "
                "\n \t\t      **** %s failed ****                                                           "
                "\n This test maintains two books. The first book is maintained using the                   "
                "\n orderbooklistener and is populated with mamaMsg data. The second book is generated      "
                "\n based on the deltas from the first listener. These are applied to the second book       "
                "\n and the test then compares both books. The test looks for exceptions that may be        "
                "\n thrown. It use a input file generated from a book subscription (full day) to a CTA feed."
                "\n This test processes Entries and DOES NOT use the Entry Manager                          "
                "\n ********************************************************************************** \n   ",               
                test_info->name());
                exit(0);
        }

    }

    if (ticker)
    {
        delete ticker;
        ticker = NULL;
    }
    if (newMessage)
    {
        delete newMessage;
        newMessage = NULL;
    }

    cleanup(impl);
}


TEST_F (MamdaBookPlaybackDeltaTest, Mamda1480)
{
    BookTicker* ticker = new BookTicker;
    ticker->myOrderBook = new MamdaOrderBook();
    mBookListener->addHandler (ticker);
    mBookListener->setProcessEntries (true);
    
    
    MamaMsg* initial = createBookMsg(MAMA_MSG_TYPE_BOOK_INITIAL, 5);
    ticker->callMamdaOnMsg(mSubscription, *initial);
    
    static int seqNum = 1;  
    MamaMsg* update = createBookMsg(MAMA_MSG_TYPE_BOOK_UPDATE, 1, 2);
    
    update->updateU32(MamdaCommonFields::MSG_SEQ_NUM, seqNum);          
    ticker->callMamdaOnMsg(mSubscription, *update);
    seqNum++;
    
    MamaMsg* update2 = createBookMsg(MAMA_MSG_TYPE_BOOK_UPDATE, 1);
    
    update->updateU32(MamdaCommonFields::MSG_SEQ_NUM, seqNum);          
    ticker->callMamdaOnMsg(mSubscription, *update2);
    seqNum++;    
        
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
        
    if (ticker)
    {
        delete ticker;
        ticker = NULL;
    }
}


