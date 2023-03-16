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

#include "common/MainUnitTest.h"
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

using namespace std;

using namespace Wombat;

class MamdaOrderBookPriceLevelTests : public ::testing::Test
{
protected:
	MamdaOrderBookPriceLevelTests () {}
	virtual ~MamdaOrderBookPriceLevelTests () {}
  
	virtual void SetUp()
	{
            mBridge = Mama::loadBridge(getMiddleware());
            Mama::open();

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
            Mama::close();
	}
	MamdaOrderBook* mBook;
	MamdaOrderBookPriceLevel* mLevel;
        MamdaOrderBookPriceLevel* mLevel1;
        MamdaOrderBookBasicDeltaList* mDeltaList;
        MamdaOrderBookEntry*          mEntry;
        mamaBridge mBridge;

};

/* ************ FUNCTIONALITY TESTS ******************* */
TEST_F (MamdaOrderBookPriceLevelTests, DoubleFindOrCreateLevelTest)
{
	printf("\n\n");
	mBook->findOrCreateLevel(100, MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
	EXPECT_EQ(1,mBook->getTotalNumLevels());
	//add price check
	mBook->clear(true);
}

TEST_F (MamdaOrderBookPriceLevelTests, MamaPriceFindOrCreateLevelTest)
{
	printf("\n\n");
	MamaPrice price;
	price.setValue(100);
	mBook->findOrCreateLevel(price, MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
	EXPECT_EQ(1,mBook->getTotalNumLevels());
	//add price check
	mBook->clear(true);
}

TEST_F (MamdaOrderBookPriceLevelTests, addEntriesFromLevelTest)
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
