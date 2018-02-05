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

#include <mama/mamacpp.h>
#include <mamda/MamdaOrderBook.h>
#include <mamda/MamdaOrderBookPriceLevel.h>
#include <mamda/MamdaOrderBookTypes.h>
#include <mamda/MamdaOrderBookBasicDeltaList.h>
#include <mamda/MamdaOrderBookEntry.h>

#include "common/CpuTestGenerator.h"
#include "common/MemoryTestGenerator.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sstream>
#include <iostream>

using namespace std;

using namespace Wombat;

class MamdaBookTest : public ::testing::Test
{
protected:
	MamdaBookTest () {}
	virtual ~MamdaBookTest () {}
  
	virtual void SetUp()
	{
		mBook = new MamdaOrderBook();
		mLevel = new MamdaOrderBookPriceLevel();
        mLevel1 = new MamdaOrderBookPriceLevel();
        mDeltaList = new MamdaOrderBookBasicDeltaList();
        mEntry  = new MamdaOrderBookEntry();
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
	MamdaOrderBook* mBook;
	MamdaOrderBookPriceLevel* mLevel;
    MamdaOrderBookPriceLevel* mLevel1;
    MamdaOrderBookBasicDeltaList* mDeltaList;
    MamdaOrderBookEntry*          mEntry;

};

/* ************ FUNCTIONALITY TESTS ******************* */
TEST_F (MamdaBookTest, DoubleFindOrCreateLevelTest)
{
	printf("\n\n");
	mBook->findOrCreateLevel(100, MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
	EXPECT_EQ(1,mBook->getTotalNumLevels());
	//add price check
	mBook->clear(true);
}

TEST_F (MamdaBookTest, MamaPriceFindOrCreateLevelTest)
{
	printf("\n\n");
	MamaPrice price;
	price.setValue(100);
	mBook->findOrCreateLevel(price, MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
	EXPECT_EQ(1,mBook->getTotalNumLevels());
	//add price check
	mBook->clear(true);
}

TEST_F (MamdaBookTest, addEntriesFromLevelTest)
{
    printf("\n\n");
    mEntry->setId("TEST");
    mLevel->addEntry(mEntry);
    mLevel1->addEntriesFromLevel(mLevel,NULL,mDeltaList); 
    string tmp("ComplexUpdate:\n  Price=0, Side=B, PlDeltaAction=A, EntryId=TEST, EntryAction=A, PlSize = 0, NumEntriesTotal = 1, EntrySize = 0");   
    ostringstream output;
    mDeltaList->dump(output);         
    string result = output.str();   
    ASSERT_TRUE(result.compare(output.str()) == 0);    
}

/* ************ END FUNCTIONALITY TESTS ******************* */

/* ************ CPU TESTS ******************* */
TEST_F (MamdaBookTest, CpuDoubleFindOrCreateLevelTest)
{
	printf("\n\n");
	double dPrice =100;
	mBook->findOrCreateLevel(dPrice, MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);  
	START_RECORDING_CPU (100000000);
	mBook->findOrCreateLevel(dPrice, MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
	STOP_RECORDING_CPU ();
	mBook->clear(true);
}

TEST_F (MamdaBookTest, CpuMamaPriceFindOrCreateLevelTest)
{
	printf("\n\n");
	MamaPrice price;
	price.setValue(100);
	mBook->findOrCreateLevel(price, MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
	START_RECORDING_CPU (100000000);
	mBook->findOrCreateLevel(price, MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
	STOP_RECORDING_CPU ();
	mBook->clear(true);
}

TEST_F (MamdaBookTest, CpuDoubleFindOrCreateXLevelsTest)
{
	printf("\n\n");
	double dPrice =100; double searchPrice; int numLevels;
	
	const char* strLevels = Mama::getProperty("findOrCreateNumLevels");
	if (!strLevels) 
    { 
        strLevels = "100"; 
        printf("\n !!!!!!! Error reading \"findOrCreateNumLevels\" parameter !!!!!!!\n"); 
    }
	numLevels = atoi(strLevels);
	
	const char* strSearchPrice = Mama::getProperty("searchPrice");
	if (!strSearchPrice) 
    { 
        strSearchPrice = "100.05"; 
        printf("\n !!!!!!! Error reading \"searchPrice\" parameter !!!!!!!\n"); 
    }
	searchPrice = atof(strSearchPrice);
	  
	for (int i=1; i<=numLevels; i++)
	{
	    mBook->findOrCreateLevel(dPrice, MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);  
	    dPrice+=0.01;
	}
	mBook->findOrCreateLevel(10000.0, MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    //printf("\n Num of Levels = %d \n", mBook->getTotalNumLevels());
	
	START_RECORDING_CPU (100000000);
	mBook->findOrCreateLevel(dPrice, MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
	STOP_RECORDING_CPU ();
	mBook->clear(true);
}

TEST_F (MamdaBookTest, CpuMamaPriceFindOrCreateXLevelsTest)
{
	printf("\n\n");
	MamaPrice price; double dPrice = 100;
	price.setValue(dPrice);
	double searchPrice; int numLevels;
	
	const char* strLevels = Mama::getProperty("findOrCreateNumLevels");
	if (!strLevels) 
    { 
        strLevels = "100"; 
        printf("\n !!!!!!! Error reading \"findOrCreateNumLevels\" parameter !!!!!!!\n"); 
    }
	numLevels = atoi(strLevels);
	
	const char* strSearchPrice = Mama::getProperty("searchPrice");
	if (!strSearchPrice) { strSearchPrice = "100"; printf("\n !!!!!!! Error reading \"searchPrice\" parameter !!!!!!!\n"); }
	searchPrice = atof(strSearchPrice);
	
	MamaPrice searchMamaPrice;
	searchMamaPrice.setValue(searchPrice);
	
	for (int i=1; i<+numLevels; i++)
	{
	    mBook->findOrCreateLevel(price, MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);  
	    dPrice+=0.01;
	    price.setValue(dPrice);
	}
	price.setValue(10000.0);
	mBook->findOrCreateLevel(price, MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    //printf("\n Num of Levels = %d \n", mBook->getTotalNumLevels());

	START_RECORDING_CPU (100000000);
	mBook->findOrCreateLevel(searchMamaPrice, MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
	STOP_RECORDING_CPU ();
	
	mBook->clear(true);
}

/* ************ END CPU TESTS ******************* */


/* ************ MEMORY TESTS ******************* */
TEST_F (MamdaBookTest, MemoryFindOrCreateOneLevelTest)
{
	printf("\n\n");
	START_RECORDING_MEMORY ();
	for (int i=0;i<1000;i++)
	{
	    mBook->findOrCreateLevel(100, MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
	}
	STOP_RECORDING_MEMORY ();
	mBook->clear(true);

}


/* ************ END MEMORY TESTS ******************* */
