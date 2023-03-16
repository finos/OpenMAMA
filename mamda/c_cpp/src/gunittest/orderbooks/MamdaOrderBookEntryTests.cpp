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

#include "common/MainUnitTest.h"
#include <wombat/wMessageStats.h>

#include <cstdlib>
#include <list>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

template <class T>
inline std::string to_String (const T& t)
{
	std::stringstream ss;
	ss << t;
	return ss.str();
}
  
using namespace Wombat;

class MamdaOrderBookEntryTests : public ::testing::Test
{
protected:
    MamdaOrderBookEntryTests ();
    ~MamdaOrderBookEntryTests () override {}
    void SetUp() override;
    void TearDown() override;
    MamdaOrderBookPriceLevel*   mLevel;
    MamdaOrderBook*             mBook;
};

MamdaOrderBookEntryTests::MamdaOrderBookEntryTests() {
    mLevel = NULL;
}

void MamdaOrderBookEntryTests::SetUp()
{
    mLevel = new MamdaOrderBookPriceLevel();
    mBook  = new MamdaOrderBook();
    mLevel->setOrderBook(mBook);
}

void MamdaOrderBookEntryTests::TearDown()
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

TEST_F (MamdaOrderBookEntryTests, AddEntryTest)
{
	MamdaOrderBookEntry* mEntry = new MamdaOrderBookEntry;
	mEntry->setId("TEST");
	mLevel->addEntry(mEntry);
	ASSERT_STREQ ("TEST",(mLevel->findOrCreateEntry("TEST"))->getId());
	EXPECT_EQ(1,mLevel->getNumEntries());
	EXPECT_EQ(1,mLevel->getNumEntriesTotal());
	mLevel->clear();
}

TEST_F (MamdaOrderBookEntryTests, FindOrCreateEntryTest)
{
	MamdaOrderBookEntry* mEntry = mLevel->findOrCreateEntry("TEST");
	ASSERT_STREQ ("TEST",(mLevel->findOrCreateEntry("TEST"))->getId());
	EXPECT_EQ(1,mLevel->getNumEntries());
	EXPECT_EQ(1,mLevel->getNumEntriesTotal());
	mLevel->clear();

}

