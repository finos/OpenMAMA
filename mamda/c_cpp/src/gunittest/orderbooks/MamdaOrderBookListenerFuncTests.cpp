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
#include <fcntl.h>
#include <fstream>
#include <sys/stat.h>


using namespace Wombat;
using namespace std;

class BookTicker : public MamdaOrderBookHandler
{
public:
    BookTicker () {}
    virtual ~BookTicker () {}
	
	ofstream  myOutputFile;

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
      	myOutputFile << "Book recap" << "\n";
		book.dump(myOutputFile);
    }
    
    void onBookDelta (
        MamdaSubscription*                 subscription,
        MamdaOrderBookListener&            listener,
        const MamaMsg*                     msg,
        const MamdaOrderBookSimpleDelta&   delta,
        const MamdaOrderBook&              book)
    {
      	myOutputFile << "Book delta" << "\n";
		displayDelta (subscription, listener, msg, delta, book,
					  MamdaOrderBookPriceLevel::MAMDA_BOOK_LEVEL_LIMIT);
		book.dump(myOutputFile);
    }
    
   void onBookComplexDelta (
        MamdaSubscription*                 subscription,
        MamdaOrderBookListener&            listener,
        const MamaMsg*                     msg,
        const MamdaOrderBookComplexDelta&  delta,
        const MamdaOrderBook&              book)
    {
      	myOutputFile << "Book complex delta" << "\n";
		displayComplexDelta (subscription, listener, msg, delta, book,
							 MamdaOrderBookPriceLevel::MAMDA_BOOK_LEVEL_LIMIT);
		book.dump(myOutputFile);
    }

    void onBookClear (
        MamdaSubscription*          subscription,
        MamdaOrderBookListener&     listener,
        const MamaMsg*              msg,
        const MamdaOrderBookClear&  clear,
        const MamdaOrderBook&       book)
    {
		myOutputFile << "Book clear" << "\n";
    }

    void onBookGap (
        MamdaSubscription*          subscription,
        MamdaOrderBookListener&     listener,
        const MamaMsg*              msg,
        const MamdaOrderBookGap&    event,
        const MamdaOrderBook&       book)
    {
        myOutputFile << "Book gap" << "\n";
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
		myOutputFile << "DELTA!!!  (seq# " << seqNum << ")\n";
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
		myOutputFile << "COMPLEX DELTA!!!  (seq# " << seqNum << ")\n";
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
		myOutputFile << delta.getPlDeltaSize () 
		<< " " << delta.getPriceLevel()->getPrice() 
		<< " " << delta.getPlDeltaAction() << "\n";		 
    }
};

class MamdaBookPlaybackFuncTest : public ::testing::Test
{
protected:
	MamdaBookPlaybackFuncTest () {}
	virtual ~MamdaBookPlaybackFuncTest () {}
  
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
		mBookListener->setProcessEntries (false);
		
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

TEST_F (MamdaBookPlaybackFuncTest, noEntries)
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

	const char* inputFile2 = Mama::getProperty("comparisonFileNoEntries");

	if (!inputFile2) 
	{
		printf("\n Comparison File, comparisonFileNoEntries, not specified in mama.properties \n");
		exit(1);
	}

	if (!fileExists(inputFile2))
 	{
		printf("%s, specified in mama.properties does not exist. \n", inputFile2);
		exit(1);
 	}
	
	BookTicker* ticker = new BookTicker;
	mBookListener->addHandler (ticker);

	playbackImpl* impl = (playbackImpl*) calloc(1, sizeof (playbackImpl));
	mamaMsg_create (&impl->myTempMsg);
	impl->myInputFileName = (char*) inputFile;
	ticker->myOutputFile.open("NoEntries.out");

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
	}
	ticker->myOutputFile.close();

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
		
	printf("\n PB processed. Comparing output \n");	
	
	bool fail;
    
    fail = CompareTest("NoEntries.out",inputFile2,"NoEntries.errors");
    
	if (fail)
	    cout << "****** PLAYBACK CTA DATA TEST FAILED (NoEntries) ****** See NoEntries.errors for details." << "\n";
	else
	    cout << "****** PLAYBACK CTA DATA TEST PASSED (NoEntries) ****** "<< "\n";

    EXPECT_EQ(0, fail);
		
	cleanup(impl);
}

TEST_F (MamdaBookPlaybackFuncTest, processEntries)
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

	const char* inputFile2 = Mama::getProperty("comparisonFileProcessEntries");

	if (!inputFile2) 
	{
		printf("\n Comparison File, comparisonFileProcessEntries, not specified in mama.properties \n");
		exit(1);
	}

	if (!fileExists(inputFile2))
 	{
		printf("%s, specified in mama.properties does not exist. \n", inputFile2);
		exit(1);
 	}

	BookTicker* ticker = new BookTicker;
	mBookListener->addHandler (ticker);
	mBookListener->setProcessEntries (true);

	
	playbackImpl* impl = (playbackImpl*) calloc(1, sizeof (playbackImpl));
	mamaMsg_create (&impl->myTempMsg);
	impl->myInputFileName = (char*) inputFile;
	ticker->myOutputFile.open("ProcessEntries.out");

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
	}
	ticker->myOutputFile.close();

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

	printf("\n PB processed. Comparing output \n");	
	
    bool fail;
    
    fail = CompareTest("ProcessEntries.out",inputFile2,"processEntries.errors");
	
	if (fail)
	    cout << "****** PLAYBACK CTA DATA TEST FAILED (ProcessEntries) ****** See processEntries.errors for details." << "\n";
	else
	    cout << "****** PLAYBACK CTA DATA TEST PASSED (ProcessEntries) ****** "<< "\n";
		
	cleanup(impl);
    
    EXPECT_EQ(0,fail);
}


TEST_F (MamdaBookPlaybackFuncTest, optimisedNoEntries)
{
	const char* inputFile = Mama::getProperty("optimisedCTAInputFile");

	if (!inputFile) 
	{
		printf("\n CTA playback Data Input File, optimisedCTADataInputFile, not specified in mama.properties \n");
		exit(1);
	}

	if (!fileExists(inputFile))
 	{
		printf("%s, specified in mama.properties does not exist. \n", inputFile);
		exit(1);
 	}

	const char* inputFile2 = Mama::getProperty("optimisedNoEntriesComparisonFile");

	if (!inputFile2) 
	{
		printf("\n Comparison File, optimisedNoEntriesComparisonFile, not specified in mama.properties \n");
		exit(1);
	}

	if (!fileExists(inputFile2))
 	{
		printf("%s, specified in mama.properties does not exist. \n", inputFile2);
		exit(1);
 	}

	BookTicker* ticker = new BookTicker;
	mBookListener->addHandler (ticker);
	mBookListener->setProcessEntries (false);

	
	playbackImpl* impl = (playbackImpl*) calloc(1, sizeof (playbackImpl));
	mamaMsg_create (&impl->myTempMsg);
	impl->myInputFileName = (char*) inputFile;
	ticker->myOutputFile.open("optimisedNoEntries.out");

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
	}
	ticker->myOutputFile.close();

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
		
	printf("\n PB processed. Comparing output \n");	
    
	bool fail;
    
    fail = CompareTest("optimisedNoEntries.out",inputFile2,"optimisedNoEntries.errors");    
	
	if (fail)
	    cout << "****** PLAYBACK CTA DATA TEST FAILED (optimisedNoEntries) ******"
	         << " See optimisedNoEntries.errors for details." << "\n";
	else
	    cout << "****** PLAYBACK CTA DATA TEST PASSED (optimisedNoEntries) ****** "<< "\n";
		
	cleanup(impl);

    EXPECT_EQ(0,fail);
}

TEST_F (MamdaBookPlaybackFuncTest, optimisedProcessEntries)
{
	const char* inputFile = Mama::getProperty("optimisedCTAInputFile");

	if (!inputFile) 
	{
		printf("\n CTA playback Data Input File, optimisedCTAInputFile, not specified in mama.properties \n");
		exit(1);
	}

	if (!fileExists(inputFile))
 	{
		printf("%s, specified in mama.properties does not exist. \n", inputFile);
		exit(1);
 	}

	const char* inputFile2 = Mama::getProperty("optimisedProcessEntriesComparisonFile");

	if (!inputFile2) 
	{
		printf("\n Comparison File, optimisedProcessEntriesComparisonFile, not specified in mama.properties \n");
		exit(1);
	}

	if (!fileExists(inputFile2))
 	{
		printf("%s, specified in mama.properties does not exist. \n", inputFile2);
		exit(1);
 	}
	
	BookTicker* ticker = new BookTicker;
	mBookListener->addHandler (ticker);
	mBookListener->setProcessEntries (true);
	
	playbackImpl* impl = (playbackImpl*) calloc(1, sizeof (playbackImpl));
	mamaMsg_create (&impl->myTempMsg);
	impl->myInputFileName = (char*) inputFile;
	ticker->myOutputFile.open("optimisedProcessEntries.out");

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
	}
	ticker->myOutputFile.close();

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
		
	printf("\n PB processed. Comparing output \n");	
	
    bool fail;
    
    fail = CompareTest("optimisedProcessEntries.out",inputFile2,"optimisedProcessEntries.errors");    
	
	if (fail)
	    cout << "****** PLAYBACK CTA DATA TEST FAILED (otpimisedProcessEntries) ******" 
	         << "See optimisedProcessEntries.errors for details." << "\n";
	else
	    cout << "****** PLAYBACK CTA DATA TEST PASSED (otpimisedProcessEntries) ****** "<< "\n";
		
	cleanup(impl);
    
    EXPECT_EQ(0,fail);
}
