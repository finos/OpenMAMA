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
    }
    
    void onBookDelta (
        MamdaSubscription*                 subscription,
        MamdaOrderBookListener&            listener,
        const MamaMsg*                     msg,
        const MamdaOrderBookSimpleDelta&   delta,
        const MamdaOrderBook&              book)
    {
    }
    
   void onBookComplexDelta (
        MamdaSubscription*                 subscription,
        MamdaOrderBookListener&            listener,
        const MamaMsg*                     msg,
        const MamdaOrderBookComplexDelta&  delta,
        const MamdaOrderBook&              book)
    {
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

class MamdaBookPlaybackCpuTest : public ::testing::Test
{
protected:
    MamdaBookPlaybackCpuTest () {}
    virtual ~MamdaBookPlaybackCpuTest () {}

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

    MamaDictionary*      	dictionary;
    MamdaSubscription* 		mSubscription;
};

TEST_F (MamdaBookPlaybackCpuTest, NoEntries)
{
	const char* inputFile = Mama::getProperty ("cpuCTADataInputFile");
	if (!inputFile) 
	{
		printf("\n CTA playback Data Input File, cpuCTADataInputFile, not specified in mama.properties \n");
		exit(1);
	}
	if (!fileExists(inputFile))
 	{
		printf("%s, specified in mama.properties does not exist. \n", inputFile);
		exit(1);
 	}

    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener;
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (false);
    BookTickerCpu* ticker = new BookTickerCpu;
	mBookListener->addHandler (ticker);

	playbackImpl* impl = (playbackImpl*) calloc(1, sizeof (playbackImpl));
	mamaMsg_create (&impl->myTempMsg);
	impl->myInputFileName = (char*) inputFile;

	MamaMsg* newMessage  = NULL;
	newMessage = new MamaMsg();
	
	START_RECORDING_CPU(1)
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
	STOP_RECORDING_CPU ();

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


TEST_F (MamdaBookPlaybackCpuTest, ProcessEntries)
{
	const char* inputFile = Mama::getProperty("cpuCTADataInputFile");
	if (!inputFile) 
	{
		printf("\n CTA playback Data Input File, cpuCTADataInputFile, not specified in mama.properties \n");
		exit(1);
	}
	if (!fileExists(inputFile))
 	{
		printf("%s, specified in mama.properties does not exist. \n", inputFile);
		exit(1);
 	}
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener;
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (false);
	BookTickerCpu* ticker = new BookTickerCpu;
	mBookListener->addHandler (ticker);
	mBookListener->setProcessEntries (true);

	
	playbackImpl* impl = (playbackImpl*) calloc(1, sizeof (playbackImpl));
	mamaMsg_create (&impl->myTempMsg);
	impl->myInputFileName = (char*) inputFile;

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
	   
	START_RECORDING_CPU(1)

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
	STOP_RECORDING_CPU ();
	
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


TEST_F (MamdaBookPlaybackCpuTest, OptimisedNoEntries)
{
	const char* inputFile = Mama::getProperty("cpuOptimisedCTADataInputFile");
	if (!inputFile) 
	{
		printf("\n CTA playback Data Input File, cpuOptimisedCTADataInputFile, not specified in mama.properties \n");
		exit(1);
	}
	if (!fileExists(inputFile))
 	{
		printf("%s, specified in mama.properties does not exist. \n", inputFile);
		exit(1);
 	}
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener;
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (false);
	BookTickerCpu* ticker = new BookTickerCpu;
	mBookListener->addHandler (ticker);
	mBookListener->setProcessEntries (false);

	
	playbackImpl* impl = (playbackImpl*) calloc(1, sizeof (playbackImpl));
	mamaMsg_create (&impl->myTempMsg);
	impl->myInputFileName = (char*) inputFile;

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
	   
	START_RECORDING_CPU(1)
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
	STOP_RECORDING_CPU ();

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

TEST_F (MamdaBookPlaybackCpuTest, OptimisedProcessEntries)
{
	const char* inputFile = Mama::getProperty("cpuOptimisedCTADataInputFile");
	if (!inputFile) 
	{
		printf("\n CTA playback Data Input File, cpuOptimisedCTADataInputFile, not specified in mama.properties \n");
		exit(1);
	}
	if (!fileExists(inputFile))
 	{
		printf("%s, specified in mama.properties does not exist. \n", inputFile);
		exit(1);
 	}
    MamdaOrderBook* mBook = new MamdaOrderBook;
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener(mBook);
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (false);
	BookTickerCpu* ticker = new BookTickerCpu;
	mBookListener->addHandler (ticker);
	mBookListener->setProcessEntries (true);

	
	playbackImpl* impl = (playbackImpl*) calloc(1, sizeof (playbackImpl));
	mamaMsg_create (&impl->myTempMsg);
	impl->myInputFileName = (char*) inputFile;

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
	   
	START_RECORDING_CPU(1)
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
	STOP_RECORDING_CPU ();

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

/* ************************************************************************************** */
/* Repeat tests but aass book to listener                                                 */

TEST_F (MamdaBookPlaybackCpuTest, NoEntriesOwnBook)
{
    const char* inputFile = Mama::getProperty("cpuCTADataInputFile");
    if (!inputFile) 
    {
        printf("\n CTA playback Data Input File, cpuCTADataInputFile, not specified in mama.properties \n");
        exit(1);
    }
    if (!fileExists(inputFile))
    {
        printf("%s, specified in mama.properties does not exist. \n", inputFile);
        exit(1);
    }
    MamdaOrderBook* mBook = new MamdaOrderBook;
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener(mBook);
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (false);
    BookTickerCpu* ticker = new BookTickerCpu;
    mBookListener->addHandler (ticker);

    playbackImpl* impl = (playbackImpl*) calloc(1, sizeof (playbackImpl));
    mamaMsg_create (&impl->myTempMsg);
    impl->myInputFileName = (char*) inputFile;

    MamaMsg* newMessage  = NULL;
    newMessage = new MamaMsg();
    
    START_RECORDING_CPU(1)
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
    STOP_RECORDING_CPU ();

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


TEST_F (MamdaBookPlaybackCpuTest, ProcessEntriesOwnBook)
{
    const char* inputFile = Mama::getProperty("cpuCTADataInputFile");
    if (!inputFile) 
    {
        printf("\n CTA playback Data Input File, cpuCTADataInputFile, not specified in mama.properties \n");
        exit(1);
    }
    if (!fileExists(inputFile))
    {
        printf("%s, specified in mama.properties does not exist. \n", inputFile);
        exit(1);
    }
    MamdaOrderBook* mBook = new MamdaOrderBook;
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener(mBook);
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (false);    
    BookTickerCpu* ticker = new BookTickerCpu;
    mBookListener->addHandler (ticker);
    mBookListener->setProcessEntries (true);

    
    playbackImpl* impl = (playbackImpl*) calloc(1, sizeof (playbackImpl));
    mamaMsg_create (&impl->myTempMsg);
    impl->myInputFileName = (char*) inputFile;

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
       
    START_RECORDING_CPU(1)

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
    STOP_RECORDING_CPU ();
    
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


TEST_F (MamdaBookPlaybackCpuTest, OptimisedNoEntriesOwnBook)
{
    const char* inputFile = Mama::getProperty("cpuOptimisedCTADataInputFile");
    if (!inputFile) 
    {
        printf("\n CTA playback Data Input File, cpuOptimisedCTADataInputFile, not specified in mama.properties \n");
        exit(1);
    }
    if (!fileExists(inputFile))
    {
        printf("%s, specified in mama.properties does not exist. \n", inputFile);
        exit(1);
    }
    MamdaOrderBook* mBook = new MamdaOrderBook;
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener(mBook);
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (false);    
    BookTickerCpu* ticker = new BookTickerCpu;
    mBookListener->addHandler (ticker);
    mBookListener->setProcessEntries (false);

    
    playbackImpl* impl = (playbackImpl*) calloc(1, sizeof (playbackImpl));
    mamaMsg_create (&impl->myTempMsg);
    impl->myInputFileName = (char*) inputFile;

    MamaMsg* newMessage  = NULL;
    newMessage = new MamaMsg();
    
    if (MAMA_STATUS_OK != mamaPlaybackFileParser_allocate (&impl->myParser))
    {
        printf("\nError: Could not create input file parser\n");
        cleanup(impl);
        exit(1);
    }


    if (MAMA_STATUS_OK != mamaPlaybackFileParser_openFile (impl->myParser, impl->myInputFileName)) 
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
       
    START_RECORDING_CPU(1)
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
        newMessage->createFromMsg (impl->myTempMsg, false);
        ticker->callMamdaOnMsg (mSubscription, *newMessage);

        if(!mamaPlaybackFileParser_getNextHeader (impl->myParser, &impl->header))
            impl->header = NULL;
    }
    STOP_RECORDING_CPU ();

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

TEST_F (MamdaBookPlaybackCpuTest, OptimisedProcessEntriesOwnBook)
{
    const char* inputFile = Mama::getProperty("cpuOptimisedCTADataInputFile");
    if (!inputFile) 
    {
        printf("\n CTA playback Data Input File, cpuOptimisedCTADataInputFile, not specified in mama.properties \n");
        exit(1);
    }
    if (!fileExists(inputFile))
    {
        printf("%s, specified in mama.properties does not exist. \n", inputFile);
        exit(1);
    }
    MamdaOrderBook* mBook = new MamdaOrderBook;
    MamdaOrderBookListener* mBookListener = new MamdaOrderBookListener(mBook);
    mSubscription->addMsgListener (mBookListener);
    mBookListener->setProcessEntries (false);    
    BookTickerCpu* ticker = new BookTickerCpu;
    mBookListener->addHandler (ticker);
    mBookListener->setProcessEntries (true);

    
    playbackImpl* impl = (playbackImpl*) calloc(1, sizeof (playbackImpl));
    mamaMsg_create (&impl->myTempMsg);
    impl->myInputFileName = (char*) inputFile;

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
       
    START_RECORDING_CPU(1)
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
    STOP_RECORDING_CPU ();

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


