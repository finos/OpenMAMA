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
#include <mama/fieldcache/MamaFieldCacheField.h>
#include <string>

using namespace Wombat;

class MamaFieldCacheFieldTest : public ::testing::Test
{
	// Member variables
protected:
	/* Work around for problem in gtest where the this pointer can't be accessed
	 * from a test fixture.
	 */
	MamaFieldCacheFieldTest *m_this;

	// Member functions
protected:

	// Construction and Destruction
	MamaFieldCacheFieldTest();
	virtual ~MamaFieldCacheFieldTest();

	// Setup and teardown functions
	virtual void SetUp();
	virtual void TearDown();

};

MamaFieldCacheFieldTest::MamaFieldCacheFieldTest()
{
}

MamaFieldCacheFieldTest::~MamaFieldCacheFieldTest()
{
}

void MamaFieldCacheFieldTest::SetUp()
{
}

void MamaFieldCacheFieldTest::TearDown()
{
}

TEST_F(MamaFieldCacheFieldTest, notCreated)
{
    MamaFieldCacheField field;
    // Check if dtor works even if the C field was not created
}

TEST_F(MamaFieldCacheFieldTest, createDescSingle)
{
    MamaFieldCacheField field;
    ASSERT_EQ(0, field.getFid());
    ASSERT_EQ(MAMA_FIELD_TYPE_UNKNOWN, field.getType());
    ASSERT_FALSE(field.getPublish());
    ASSERT_FALSE(field.getCheckModified());
    ASSERT_FALSE(field.getName());
    ASSERT_FALSE(field.isModified());
    ASSERT_FALSE(field.getCValue());
    
    field.create(1, MAMA_FIELD_TYPE_I32, "name");
    ASSERT_EQ(1, field.getFid());
    ASSERT_EQ(MAMA_FIELD_TYPE_I32, field.getType());
    ASSERT_TRUE(field.getPublish());
    ASSERT_TRUE(field.getCheckModified());
    ASSERT_STREQ("name", field.getName());
    ASSERT_FALSE(field.isModified());
    ASSERT_TRUE(field.getCValue());
    ASSERT_FALSE(field.isVector());
}

TEST_F(MamaFieldCacheFieldTest, createDescVector)
{
    MamaFieldCacheField field;
    field.create(1, MAMA_FIELD_TYPE_VECTOR_I32, "name");
    ASSERT_EQ(1, field.getFid());
    ASSERT_EQ(MAMA_FIELD_TYPE_VECTOR_I32, field.getType());
    ASSERT_TRUE(field.getPublish());
    ASSERT_TRUE(field.getCheckModified());
    ASSERT_STREQ("name", field.getName());
    ASSERT_FALSE(field.isModified());
    ASSERT_TRUE(field.getCValue());
    ASSERT_TRUE(field.isVector());
}

TEST_F(MamaFieldCacheFieldTest, testFlags)
{
    MamaFieldCacheField field;
    field.create(1, MAMA_FIELD_TYPE_VECTOR_I32, "name");
    
    field.setPublish(true);
    ASSERT_TRUE(field.getPublish());
    
    field.setPublish(false);
    ASSERT_FALSE(field.getPublish());
    
    field.setCheckModified(true);
    ASSERT_TRUE(field.getCheckModified());
    
    field.setCheckModified(false);
    ASSERT_FALSE(field.getCheckModified());
}

TEST_F(MamaFieldCacheFieldTest, testCValue)
{
    MamaFieldCacheField field;
    ASSERT_FALSE(field.getCValue());
    
    field.create(1 , MAMA_FIELD_TYPE_F64, "");
    
    mamaFieldCacheField origCfield = field.getCValue();
    ASSERT_TRUE(field.getCValue());
    field.getCValue() = NULL;
    ASSERT_FALSE(field.getCValue());
    field.getCValue() = origCfield;
}

TEST_F(MamaFieldCacheFieldTest, testCopy)
{
    MamaFieldCacheField field1;
    
    field1.create(2, MAMA_FIELD_TYPE_I32);
    MamaFieldCacheField field2(field1);
    
    ASSERT_EQ(field1.getFid(), field2.getFid());
    ASSERT_EQ(field1.getType(), field2.getType());
    ASSERT_STREQ(field1.getName(), field2.getName());
}

//TEST_F(MamaFieldCacheFieldTest, testSetValue)
//{
//    MamaFieldCacheField field1;
//    field1.create(1, MAMA_FIELD_TYPE_CHAR);
//    field1.setValue('a');
//    {
//    char v;
//    field1.getValue(v);
//    ASSERT_EQ('a', v);
//    }
//    
//    MamaFieldCacheField field2;
//    field2.create(2, MAMA_FIELD_TYPE_BOOL);
//    field2.setValue(true);
//    {
//    mama_bool_t v;
//    field2.getValue(v);
//    ASSERT_TRUE(v);
//    }
//    
//    MamaFieldCacheField field5;
//    field5.create(5, MAMA_FIELD_TYPE_I16);
//    field5.setValue(6);
//    {
//    mama_i32_t v;
//    field5.getValue(v);
//    ASSERT_EQ(6, v);
//    }
//    
//    MamaFieldCacheField field11;
//    field11.create(11, MAMA_FIELD_TYPE_F32);
//    field11.setValue(6.5);
//    {
//    mama_f32_t v;
//    field11.getValue(v);
//    ASSERT_FLOAT_EQ(6.5, v);
//    }
//    
//    MamaFieldCacheField field12;
//    field12.create(12, MAMA_FIELD_TYPE_F64);
//    field12.setValue(16.5);
//    {
//    mama_f64_t v;
//    field12.getValue(v);
//    ASSERT_DOUBLE_EQ(16.5, v);
//    }
//    
//    mama_size_t len;
//
//    MamaFieldCacheField field20;
//    field20.create(20, MAMA_FIELD_TYPE_STRING);
//    field20.setValue("a", 1);
//    {
//    const char* v = NULL;
//    field20.getValue(v, len);
//    ASSERT_STREQ("a", v);
//    ASSERT_EQ(1, len);
//    }
//    
//    MamaFieldCacheField field21;
//    field20.create(21, MAMA_FIELD_TYPE_VECTOR_I8);
//    mama_i8_t values[5] = { 0, 1, -2, 3, 4 };
//    field21.setValue(values, 1);
//    {
//    
//    const mama_i8_t* result = NULL;
//    field21.getValue(result, len);
//    ASSERT_EQ(5, len);
//    ASSERT_EQ(0, result[0]);
//    ASSERT_EQ(1, result[1]);
//    ASSERT_EQ(-2, result[2]);
//    ASSERT_EQ(3, result[3]);
//    ASSERT_EQ(4, result[4]);
//    }
//}
