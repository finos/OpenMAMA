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
#include <MainUnitTestCpp.h>
#include <mama/fieldcache/MamaFieldCache.h>
#include <mama/fieldcache/MamaFieldCacheRecord.h>
#include <mama/fieldcache/MamaFieldCacheFieldTypes.h>
#include <algorithm>

using namespace Wombat;

class MamaFieldCacheIteratorTest : public ::testing::Test
{
	// Member variables
protected:
	/* Work around for problem in gtest where the this pointer can't be accessed
	 * from a test fixture.
	 */
	MamaFieldCacheIteratorTest *m_this;

	// Member functions
protected:

	// Construction and Destruction
	MamaFieldCacheIteratorTest();
	virtual ~MamaFieldCacheIteratorTest();

	// Setup and teardown functions
	virtual void SetUp();
	virtual void TearDown();
    
private:
    mamaBridge mBridge;
};

MamaFieldCacheIteratorTest::MamaFieldCacheIteratorTest()
{
}

MamaFieldCacheIteratorTest::~MamaFieldCacheIteratorTest()
{
}

void MamaFieldCacheIteratorTest::SetUp()
{
}

void MamaFieldCacheIteratorTest::TearDown()
{
}

struct TestStdIteratorPredFind
{
    TestStdIteratorPredFind(std::size_t fid)
        : mFid(fid)
    {}
    bool operator()(const MamaFieldCacheField& field)
    {
        return field.getFid() == mFid;
    }

    std::size_t mFid;
};

struct TestStdIteratorPredTransform
{
    MamaFieldCacheField& operator()(MamaFieldCacheField& field)
    {
        field.setPublish(true);
        return field;
    }
};

struct TestStdIteratorPredCountEven
{
    TestStdIteratorPredCountEven()
        : mCount(0)
        , mTotalCount(0)
    {
    }
    void operator()(const MamaFieldCacheField& field)
    {
        if (field.getFid() % 2)
        {
            ++mCount;
        }
        ++mTotalCount;
    }
    
    std::size_t mCount;
    std::size_t mTotalCount;
};

TEST_F(MamaFieldCacheIteratorTest, testIterator)
{
    MamaFieldCache fieldCache;
    fieldCache.create();
    
    MamaFieldCacheRecord record;
    record.create();

    record.add(1, MAMA_FIELD_TYPE_I32, "MamaFieldCacheIteratorTest.testIterator");
    record.add(2, MAMA_FIELD_TYPE_I32, "MamaFieldCacheIteratorTest.testIterator");
    record.add(3, MAMA_FIELD_TYPE_I32, "MamaFieldCacheIteratorTest.testIterator");

    fieldCache.apply(record);

    ASSERT_EQ(3, fieldCache.getSize());

    MamaFieldCacheFieldI32 i32Field;
    int counter = 0;
    for (MamaFieldCache::iterator it = fieldCache.begin();
            it != fieldCache.end();
            ++it)
    {
        MamaFieldCacheField& field = *it;
        ASSERT_EQ(counter+1, field.getFid());
        ASSERT_EQ(MAMA_FIELD_TYPE_I32, field.getType());
        ASSERT_STREQ("MamaFieldCacheIteratorTest.testIterator", field.getName());
        i32Field.set(field, counter+1);
        counter++;
    }
    ASSERT_EQ(3, counter);
    
    counter = 0;
    for (MamaFieldCache::const_iterator it = fieldCache.begin();
            it != fieldCache.end();
            ++it)
    {
        const MamaFieldCacheField& field = *it;
        ASSERT_EQ(counter+1, field.getFid());
        ASSERT_EQ(MAMA_FIELD_TYPE_I32, field.getType());
        ASSERT_STREQ("MamaFieldCacheIteratorTest.testIterator", field.getName());
        ASSERT_EQ(counter+1, i32Field.get(field));
        counter++;
    }
    ASSERT_EQ(3, counter);
}

TEST_F(MamaFieldCacheIteratorTest, testIterator2)
{
    MamaFieldCache fieldCache;
    fieldCache.create();
    
    MamaFieldCacheRecord record;
    record.create();

    record.add(1, MAMA_FIELD_TYPE_I32, "MamaFieldCacheIteratorTest.testIterator");
    record.add(2, MAMA_FIELD_TYPE_I32, "MamaFieldCacheIteratorTest.testIterator");
    record.add(3, MAMA_FIELD_TYPE_I32, "MamaFieldCacheIteratorTest.testIterator");

    fieldCache.apply(record);

    ASSERT_EQ(3, fieldCache.getSize());

    int counter = 0;
    MamaFieldCache::iterator it = fieldCache.begin();
    MamaFieldCache::iterator end = fieldCache.end();
    for ( ; it != end; ++it)
    {
        MamaFieldCacheField& field = *it;
        ASSERT_EQ(counter+1, field.getFid());
        if (counter == 1)
            break;
        counter++;
    }
    MamaFieldCacheField& field = *it;
    ASSERT_EQ(MAMA_FIELD_TYPE_I32, field.getType());
    // it is not pointing to end, but to a real field.
    // Check that all memory is freed and no memory is freed more than once!
}


TEST_F(MamaFieldCacheIteratorTest, stdIterator1)
{
    MamaFieldCache fieldCache;
    fieldCache.create();
    
    MamaFieldCacheRecord record;
    record.create();

    record.add(1, MAMA_FIELD_TYPE_I32, "MamaFieldCacheIteratorTest.testIterator1");
    record.add(2, MAMA_FIELD_TYPE_I32, "MamaFieldCacheIteratorTest.testIterator1");
    record.add(3, MAMA_FIELD_TYPE_I32, "MamaFieldCacheIteratorTest.testIterator1");

    fieldCache.apply(record);

    ASSERT_EQ(3, fieldCache.getSize());

    MamaFieldCacheFieldI32 i32Field;
    int counter = 0;
    for (MamaFieldCache::iterator it = fieldCache.begin();
            it != fieldCache.end();
            ++it)
    {
        MamaFieldCacheField& field = *it;
        ASSERT_EQ(counter+1, field.getFid());
        ASSERT_EQ(MAMA_FIELD_TYPE_I32, field.getType());
        ASSERT_STREQ("MamaFieldCacheIteratorTest.testIterator1", field.getName());
        i32Field.set(field, counter+1);
        counter++;
//        field.setModified(false);
    }
    ASSERT_EQ(3, counter);
}
    
TEST_F(MamaFieldCacheIteratorTest, stdIterator2)
{
    MamaFieldCache fieldCache;
    fieldCache.create();
    
    MamaFieldCacheRecord record;
    record.create();

    record.add(1, MAMA_FIELD_TYPE_I32, "MamaFieldCacheIteratorTest.testIterator2");
    record.add(2, MAMA_FIELD_TYPE_I32, "MamaFieldCacheIteratorTest.testIterator2");
    record.add(3, MAMA_FIELD_TYPE_I32, "MamaFieldCacheIteratorTest.testIterator2");

    fieldCache.apply(record);

    ASSERT_EQ(3, fieldCache.getSize());
    MamaFieldCache::const_iterator iter =
            std::find_if(fieldCache.begin(), fieldCache.end(), TestStdIteratorPredFind(2));
    ASSERT_TRUE(iter != fieldCache.end());
    ASSERT_EQ(2, iter->getFid());
    ASSERT_TRUE(iter->isModified());

    iter = std::find_if(fieldCache.begin(), fieldCache.end(), TestStdIteratorPredFind(55));
    ASSERT_TRUE(iter == fieldCache.end());
}

TEST_F(MamaFieldCacheIteratorTest, stdIterator3)
{
    MamaFieldCache fieldCache;
    fieldCache.create();
    
    MamaFieldCacheRecord record;
    record.create();

    record.add(1, MAMA_FIELD_TYPE_I32, "MamaFieldCacheIteratorTest.testIterator3").setPublish(false);
    record.add(2, MAMA_FIELD_TYPE_I32, "MamaFieldCacheIteratorTest.testIterator3").setPublish(false);
    record.add(3, MAMA_FIELD_TYPE_I32, "MamaFieldCacheIteratorTest.testIterator3").setPublish(false);
    record.add(4, MAMA_FIELD_TYPE_CHAR, "MamaFieldCacheIteratorTest.testIterator3").setPublish(false);
    record.add(5, MAMA_FIELD_TYPE_CHAR, "MamaFieldCacheIteratorTest.testIterator3").setPublish(false);

    fieldCache.apply(record);
    // all the new fields are modified after first apply

    ASSERT_EQ(5, fieldCache.getSize());
    int counter = 0;
    // set modified to false and publish to true
    std::transform(fieldCache.begin(),
                    fieldCache.end(),
                    fieldCache.begin(),
                    TestStdIteratorPredTransform());

    for (MamaFieldCache::iterator it = fieldCache.begin();
            it != fieldCache.end();
            ++it)
    {
        MamaFieldCacheField& field = *it;
        ASSERT_EQ(counter+1, field.getFid());
        ASSERT_STREQ("MamaFieldCacheIteratorTest.testIterator3", field.getName());
        ASSERT_TRUE(field.getPublish());
        counter++;
    }
    
    TestStdIteratorPredCountEven pred;
    pred = std::for_each(fieldCache.begin(), fieldCache.end(), pred);
    ASSERT_EQ(fieldCache.getSize(), pred.mTotalCount);
    ASSERT_EQ(3, pred.mCount);
}
