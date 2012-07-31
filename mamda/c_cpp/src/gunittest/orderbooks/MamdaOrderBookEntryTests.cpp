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

#include "common/CpuTestGenerator.h"
#include "common/MemoryTestGenerator.h"
#include <wombat/wMessageStats.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <sstream>

template <class T>
inline std::string to_String (const T& t)
{
	std::stringstream ss;
	ss << t;
	return ss.str();
}
  
using namespace Wombat;

class MamdaBookTest : public ::testing::Test
{
protected:
	MamdaBookTest () {}
	virtual ~MamdaBookTest () {}
  
	virtual void SetUp()
	{
		mLevel = new MamdaOrderBookPriceLevel();
        mBook  = new MamdaOrderBook();
        
        mLevel->setOrderBook(mBook);
	}
  
	virtual void TearDown()
	{
		if (mLevel)
		{
		delete mLevel;
		mLevel = NULL;
		}
		
        if (mBook)
        {
        delete mBook;
        mBook = NULL;
        }
	}
	MamdaOrderBookPriceLevel*   mLevel;
    MamdaOrderBook*             mBook;
};

TEST_F (MamdaBookTest, AddEntryTest)
{
	printf("\n\n");
	MamdaOrderBookEntry* mEntry = new MamdaOrderBookEntry;
	mEntry->setId("TEST");
	mLevel->addEntry(mEntry);
	ASSERT_STREQ ("TEST",(mLevel->findOrCreateEntry("TEST"))->getId());
	EXPECT_EQ(1,mLevel->getNumEntries());
	EXPECT_EQ(1,mLevel->getNumEntriesTotal());
	mLevel->clear();

}

TEST_F (MamdaBookTest, FindOrCreateEntryTest)
{
	printf("\n\n");
	MamdaOrderBookEntry* mEntry = mLevel->findOrCreateEntry("TEST");
	ASSERT_STREQ ("TEST",(mLevel->findOrCreateEntry("TEST"))->getId());
	EXPECT_EQ(1,mLevel->getNumEntries());
	EXPECT_EQ(1,mLevel->getNumEntriesTotal());
	mLevel->clear();

}

TEST_F (MamdaBookTest, CpuFindOrCreateEntryTest)
{
	printf("\n\n");
	MamdaOrderBookEntry* mEntry = mLevel->findOrCreateEntry("TEST1");
	START_RECORDING_CPU (100000000);
	MamdaOrderBookEntry* mEntry = mLevel->findOrCreateEntry("TEST1");
	STOP_RECORDING_CPU ();
	mLevel->clear();

}

TEST_F (MamdaBookTest, CpuAddXEntriesTest)
{
	printf("\n\n");
	int numEntries;
	const char* strNumEntries = Mama::getProperty("AddEntryNumEntries");
	if (!strNumEntries) 
    {
        strNumEntries = "10"; 
        printf("\n !!!!!!! Error reading \"AddEntryNumEntries\" parameter !!!!!!!\n"); 
    }
	numEntries = atoi(strNumEntries);

	const char* searchEntryId = Mama::getProperty("AddEntrySearchId");
	if (!searchEntryId) 
    { 
        searchEntryId = "10"; 
        printf("\n !!!!!!! Error reading \"AddEntrySearchId\" parameter !!!!!!!\n"); 
    }
	
	for (int i=1; i<=numEntries; i++)
	{
	    string id;
	    id = to_String(i);
	    string entryId ("TEST");
	    entryId += id;
	    MamdaOrderBookEntry* mEntry = new MamdaOrderBookEntry;
	    mEntry->setId(entryId.c_str());
	    mLevel->addEntry(mEntry);
	}
	EXPECT_EQ(numEntries,mLevel->getNumEntries());
	EXPECT_EQ(numEntries,mLevel->getNumEntriesTotal());
	
	START_RECORDING_CPU (100000000);
	MamdaOrderBookEntry* mEntry = mLevel->findOrCreateEntry(searchEntryId);
	STOP_RECORDING_CPU ();
	mLevel->clear();

}

TEST_F (MamdaBookTest, CpuFindOrCreateXEntriesTest)
{
	printf("\n\n");
	int numEntries;
	const char* strNumEntries = Mama::getProperty("findOrCreateNumEntries");
	if (!strNumEntries) 
    { 
        strNumEntries = "10"; 
        printf("\n !!!!!!! Error reading \"findOrCreateNumEntries\" parameter !!!!!!!\n"); 
    }
	numEntries = atoi(strNumEntries);
	
	const char* searchEntryId = Mama::getProperty("findOrCreateEntrySearchId");
	if (!searchEntryId) 
    { 
        searchEntryId = "10"; 
        printf("\n !!!!!!! Error reading \"findOrCreateEntrySearchId\" parameter !!!!!!!\n"); 
    }
	
	for (int i=1; i<=numEntries; i++)
	{
	    string id = to_String(i);
	    string entryId ("TEST");
	    entryId += id;
	    MamdaOrderBookEntry* mEntry = mLevel->findOrCreateEntry(entryId.c_str());
	}
	EXPECT_EQ(numEntries,mLevel->getNumEntries());
	EXPECT_EQ(numEntries,mLevel->getNumEntriesTotal());
	
	START_RECORDING_CPU (100000000);
	MamdaOrderBookEntry* mEntry = mLevel->findOrCreateEntry(searchEntryId);
	STOP_RECORDING_CPU ();
	mLevel->clear();
	
}
