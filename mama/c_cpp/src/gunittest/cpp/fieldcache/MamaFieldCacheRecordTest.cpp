/* 
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
#include <mama/fieldcache/MamaFieldCacheRecord.h>
#include <mama/fieldcache/MamaFieldCacheFieldTypes.h>

using namespace Wombat;

class MamaFieldCacheRecordTest : public ::testing::Test
{
	// Member variables
protected:
	/* Work around for problem in gtest where the this pointer can't be accessed
	 * from a test fixture.
	 */
	MamaFieldCacheRecordTest *m_this;

	// Member functions
protected:

	// Construction and Destruction
	MamaFieldCacheRecordTest();
	virtual ~MamaFieldCacheRecordTest();

	// Setup and teardown functions
	virtual void SetUp();
	virtual void TearDown();

};

MamaFieldCacheRecordTest::MamaFieldCacheRecordTest()
{
}

MamaFieldCacheRecordTest::~MamaFieldCacheRecordTest()
{
}

void MamaFieldCacheRecordTest::SetUp()
{
}

void MamaFieldCacheRecordTest::TearDown()
{
}

TEST_F(MamaFieldCacheRecordTest, notCreated)
{
    MamaFieldCacheRecord record;
}

TEST_F(MamaFieldCacheRecordTest, create)
{
    MamaFieldCacheRecord record;
    record.create();
    
    ASSERT_EQ(0, record.getSize());
}

TEST_F(MamaFieldCacheRecordTest, add)
{
    MamaFieldCacheRecord record;
    record.create();

    MamaFieldCacheField& field = 
            record.add(1, MAMA_FIELD_TYPE_STRING, "MamaFieldCacheRecordTest.create");
    ASSERT_EQ(1, field.getFid());
    ASSERT_EQ(MAMA_FIELD_TYPE_STRING, field.getType());
    ASSERT_STREQ("MamaFieldCacheRecordTest.create", field.getName());
    ASSERT_EQ(1, record.getSize());
}

TEST_F(MamaFieldCacheRecordTest, find)
{
    MamaFieldCacheRecord record;
    record.create();
    
    const MamaFieldCacheField& fieldNotFound = record.find(1);
    ASSERT_EQ(0, fieldNotFound.getFid());
    ASSERT_EQ(MAMA_FIELD_TYPE_UNKNOWN, fieldNotFound.getType());
    
    MamaFieldCacheField& newField =
        record.add(1, MAMA_FIELD_TYPE_STRING, "MamaFieldCacheRecordTest.find");

    MamaFieldCacheFieldString stringField;
    stringField.set(newField, "hello");

    MamaFieldCacheField& fieldFound = record.find(1);
    ASSERT_EQ(1, fieldFound.getFid());
    ASSERT_EQ(MAMA_FIELD_TYPE_STRING, fieldFound.getType());
    ASSERT_STREQ("MamaFieldCacheRecordTest.find", fieldFound.getName());

    ASSERT_EQ(std::string("hello"), stringField.get(fieldFound));
}

TEST_F(MamaFieldCacheRecordTest, clear)
{
    MamaFieldCacheRecord record;
    record.create();

    MamaFieldCacheField& field = 
            record.add(1, MAMA_FIELD_TYPE_STRING, "MamaFieldCacheRecordTest.create");
    ASSERT_EQ(1, record.getSize());

    record.clear();
    ASSERT_EQ(0, record.getSize());

    record.add(1, MAMA_FIELD_TYPE_STRING, "MamaFieldCacheRecordTest.find");
    ASSERT_EQ(1, record.getSize());
}
