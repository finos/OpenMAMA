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
#include <mama/MamaMsg.h>

using namespace Wombat;

class MamaFieldCacheTest : public ::testing::Test
{
	// Member variables
protected:
	/* Work around for problem in gtest where the this pointer can't be accessed
	 * from a test fixture.
	 */
	MamaFieldCacheTest *m_this;

	// Member functions
protected:

	// Construction and Destruction
	MamaFieldCacheTest();
	virtual ~MamaFieldCacheTest();

	// Setup and teardown functions
	virtual void SetUp();
	virtual void TearDown();
    
private:
    mamaBridge mBridge;
};

MamaFieldCacheTest::MamaFieldCacheTest()
{
}

MamaFieldCacheTest::~MamaFieldCacheTest()
{
}

void MamaFieldCacheTest::SetUp()
{
    mBridge = Mama::loadBridge(getMiddleware());

    Mama::open();
}

void MamaFieldCacheTest::TearDown()
{
    Mama::close();
}

TEST_F(MamaFieldCacheTest, notCreated)
{
    MamaFieldCache fieldCache;
}

TEST_F(MamaFieldCacheTest, create)
{
    MamaFieldCache fieldCache;
    fieldCache.create();
    
    ASSERT_TRUE(fieldCache.getTrackModified());
    ASSERT_FALSE(fieldCache.getUseFieldNames());
    ASSERT_FALSE(fieldCache.getUseLock());
    ASSERT_EQ(0, fieldCache.getSize());
}

TEST_F(MamaFieldCacheTest, applyField)
{
    MamaFieldCache fieldCache;
    fieldCache.create();
    
    {
    MamaFieldCacheField field;
    field.create(111, MAMA_FIELD_TYPE_CHAR);
    
    ASSERT_EQ(0, fieldCache.getSize());
    fieldCache.apply(field);
    }
    ASSERT_EQ(1, fieldCache.getSize());
    const MamaFieldCacheField* field1 = fieldCache.find(111);
    ASSERT_TRUE(field1);
    ASSERT_EQ(MAMA_FIELD_TYPE_CHAR, field1->getType());
}

TEST_F(MamaFieldCacheTest, applyField1)
{
    MamaFieldCache fieldCache;
    fieldCache.create();

    MamaFieldCacheField newField;
    newField.create(555, MAMA_FIELD_TYPE_STRING, "myfield");
    newField.setPublish(true);
    newField.setCheckModified(true);
    fieldCache.apply(newField);
    MamaFieldCacheField* field = fieldCache.find(555, "myfield");
    ASSERT_TRUE(field);
}

TEST_F(MamaFieldCacheTest, applyRecord)
{
    MamaFieldCache fieldCache;
    fieldCache.create();
    
    ASSERT_FALSE(fieldCache.find(11));
    
    MamaFieldCacheRecord record;
    record.create();
        
    MamaFieldCacheFieldString stringField;
    MamaFieldCacheFieldPrice priceField;
    
    {
    MamaFieldCacheField& field =
        record.add(11, MAMA_FIELD_TYPE_STRING, "MamaFieldCacheTest.applyRecord");
    ASSERT_EQ(11, field.getFid());
    stringField.set(field, "MYSYMBOL");
    }
    {
    MamaFieldCacheField& field =
        record.add(22, MAMA_FIELD_TYPE_PRICE, "MamaFieldCacheTest.applyRecord");
    ASSERT_EQ(22, field.getFid());
    priceField.set(field, MamaPrice(16.1));
    }

    fieldCache.apply(record);
    
    ASSERT_EQ(2, fieldCache.getSize());
    const MamaFieldCacheField* field = fieldCache.find(11);
    ASSERT_TRUE(field);
    ASSERT_EQ(11, field->getFid());
    ASSERT_EQ(MAMA_FIELD_TYPE_STRING, field->getType());
    ASSERT_STREQ("MamaFieldCacheTest.applyRecord", field->getName());
    ASSERT_STREQ("MYSYMBOL", stringField.get(*field));
    
    field = fieldCache.find(22);
    ASSERT_TRUE(field);
    ASSERT_EQ(22, field->getFid());
    ASSERT_EQ(MAMA_FIELD_TYPE_PRICE, field->getType());
    ASSERT_STREQ("MamaFieldCacheTest.applyRecord", field->getName());
    ASSERT_DOUBLE_EQ(16.1, priceField.get(*field).getValue());
}

TEST_F(MamaFieldCacheTest, applyMsg)
{
    MamaFieldCache fieldCache;
    fieldCache.create();

    ASSERT_FALSE(fieldCache.find(11));
    
    MamaMsg msg;
    msg.create();
    msg.addString("nameString", 11, "MYSYMBOL");
    msg.addPrice("namePrice", 22, MamaPrice(1.9));
    
    fieldCache.apply(msg);
    
    ASSERT_EQ(2, fieldCache.getSize());
    
    MamaFieldCacheFieldString stringField;
    MamaFieldCacheFieldPrice priceField;
    {
    const MamaFieldCacheField& field = *fieldCache.find(11);
    ASSERT_EQ(11, field.getFid());
    ASSERT_EQ(MAMA_FIELD_TYPE_STRING, field.getType());
    ASSERT_FALSE(field.getName());
    ASSERT_EQ(std::string("MYSYMBOL"), stringField.get(field));
    }

    {
    const MamaFieldCacheField& field = *fieldCache.find(22);
    ASSERT_EQ(22, field.getFid());
    ASSERT_EQ(MAMA_FIELD_TYPE_PRICE, field.getType());
    ASSERT_FALSE(field.getName());
    ASSERT_TRUE(1.9 < priceField.get(field).getValue() < 1.9);
    }
}

TEST_F(MamaFieldCacheTest, applyMsgUsingNames)
{
    MamaFieldCache fieldCache;
    fieldCache.create();
    fieldCache.setUseFieldNames(true);

    ASSERT_FALSE(fieldCache.find(11));
    
    MamaMsg msg;
    msg.create();
    msg.addString("nameString", 11, "MYSYMBOL");
    msg.addPrice("namePrice", 22, MamaPrice(1.9));
    
    fieldCache.apply(msg);
    
    ASSERT_EQ(2, fieldCache.getSize());
    
    MamaFieldCacheFieldString stringField;
    MamaFieldCacheFieldPrice priceField;
    {
    const MamaFieldCacheField& field = *fieldCache.find(11);
    ASSERT_EQ(11, field.getFid());
    ASSERT_EQ(MAMA_FIELD_TYPE_STRING, field.getType());
    ASSERT_STREQ("nameString", field.getName());
    ASSERT_EQ(std::string("MYSYMBOL"), stringField.get(field));
    }

    {
    const MamaFieldCacheField& field = *fieldCache.find(22);
    ASSERT_EQ(22, field.getFid());
    ASSERT_EQ(MAMA_FIELD_TYPE_PRICE, field.getType());
    ASSERT_STREQ("namePrice", field.getName());
    ASSERT_TRUE(1.9 < priceField.get(field).getValue() < 1.9);
    }
}

TEST_F(MamaFieldCacheTest, clear)
{
    MamaFieldCache fieldCache;
    fieldCache.create();

    MamaFieldCacheRecord record;
    record.create();
    record.add(11, MAMA_FIELD_TYPE_U32, "MamaFieldCacheTest.applyRecord");
    record.add(22, MAMA_FIELD_TYPE_F64, "MamaFieldCacheTest.applyRecord");
    record.add(40, MAMA_FIELD_TYPE_I32, "MamaFieldCacheTest.applyRecord");

    fieldCache.apply(record);
    
    ASSERT_EQ(3, fieldCache.getSize());

    ASSERT_EQ(MAMA_FIELD_TYPE_U32, fieldCache.find(11)->getType());
    ASSERT_EQ(MAMA_FIELD_TYPE_F64, fieldCache.find(22)->getType());
    ASSERT_EQ(MAMA_FIELD_TYPE_I32, fieldCache.find(40)->getType());
    
    fieldCache.clear();
    
    ASSERT_TRUE(NULL == fieldCache.find(11));
    ASSERT_TRUE(NULL == fieldCache.find(22));
    ASSERT_TRUE(NULL == fieldCache.find(40));
}

TEST_F(MamaFieldCacheTest, getFullMessage)
{
    MamaFieldCache fieldCache;
    fieldCache.create();

    MamaFieldCacheRecord record;
    record.create();

    MamaFieldCacheFieldString stringField;
    {
    MamaFieldCacheField& field =
        record.add(11, MAMA_FIELD_TYPE_STRING, "MamaFieldCacheTest.getFullMessage");
    stringField.set(field, "one");
    }
    {
    MamaFieldCacheField& field =
        record.add(22, MAMA_FIELD_TYPE_STRING, "MamaFieldCacheTest.getFullMessage");
    stringField.set(field, "two");
    }
    {
    MamaFieldCacheField& field =
        record.add(33, MAMA_FIELD_TYPE_STRING, "MamaFieldCacheTest.getFullMessage");
    stringField.set(field, "three");
    }

    fieldCache.apply(record);
    
    MamaMsg msg;
    msg.create();
    fieldCache.getFullMessage(msg);
    ASSERT_EQ(3, msg.getNumFields());
}

TEST_F(MamaFieldCacheTest, getDeltaMessage)
{
    MamaFieldCache fieldCache;
    fieldCache.create();

    MamaFieldCacheRecord record;
    record.create();

    MamaFieldCacheFieldString stringField;
    {
    MamaFieldCacheField& field =
        record.add(11, MAMA_FIELD_TYPE_STRING, "MamaFieldCacheTest.getDeltaMessage");
    stringField.set(field, "one");
    }
    {
    MamaFieldCacheField& field =
        record.add(22, MAMA_FIELD_TYPE_STRING, "MamaFieldCacheTest.getDeltaMessage");
    stringField.set(field, "two");
    }
    {
    MamaFieldCacheField& field =
        record.add(33, MAMA_FIELD_TYPE_STRING, "MamaFieldCacheTest.getDeltaMessage");
    stringField.set(field, "three");
    }
    ASSERT_EQ(3, record.getSize());

    fieldCache.apply(record);
    
    MamaMsg msg;
    msg.create();
    fieldCache.getDeltaMessage(msg);
    ASSERT_EQ(3, msg.getNumFields());
    
    record.clear();

    {
    MamaFieldCacheField& field =
        record.add(11, MAMA_FIELD_TYPE_STRING, "MamaFieldCacheTest.getDeltaMessage");
    stringField.set(field, "one modified");
    }
    {
    MamaFieldCacheField& field =
        record.add(44, MAMA_FIELD_TYPE_I64, "MamaFieldCacheTest.getDeltaMessage");
    MamaFieldCacheFieldI64 i64Field;
    i64Field.set(field, 321);
    }
    ASSERT_EQ(2, record.getSize());
    fieldCache.apply(record);

    msg.clear();
    ASSERT_EQ(0, msg.getNumFields());

    fieldCache.getDeltaMessage(msg);
    // Only 2 field have been modified
    ASSERT_EQ(2, msg.getNumFields());
}
