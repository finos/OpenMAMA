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
#include <mama/fieldcache/MamaFieldCacheFieldTypes.h>
#include <string>

using namespace Wombat;

class MamaFieldCacheFieldTypesTest : public ::testing::Test
{
	// Member variables
protected:
	/* Work around for problem in gtest where the this pointer can't be accessed
	 * from a test fixture.
	 */
	MamaFieldCacheFieldTypesTest *m_this;

	// Member functions
protected:

	// Construction and Destruction
	MamaFieldCacheFieldTypesTest();
	virtual ~MamaFieldCacheFieldTypesTest();

	// Setup and teardown functions
	virtual void SetUp();
	virtual void TearDown();

};

MamaFieldCacheFieldTypesTest::MamaFieldCacheFieldTypesTest()
{
}

MamaFieldCacheFieldTypesTest::~MamaFieldCacheFieldTypesTest()
{
}

void MamaFieldCacheFieldTypesTest::SetUp()
{
}

void MamaFieldCacheFieldTypesTest::TearDown()
{
}

TEST_F(MamaFieldCacheFieldTypesTest, testWrongType)
{
    MamaFieldCacheField fieldBase;
    fieldBase.create(1, MAMA_FIELD_TYPE_BOOL);
    
    MamaFieldCacheFieldChar charField;
    ASSERT_THROW(charField.set(fieldBase, 'i'), std::invalid_argument);
    MamaFieldCacheFieldI8 i8Field;
    ASSERT_THROW(i8Field.set(fieldBase, 3), std::invalid_argument);
    MamaFieldCacheFieldU8 u8Field;
    ASSERT_THROW(u8Field.set(fieldBase, 3), std::invalid_argument);
    MamaFieldCacheFieldI16 i16Field;
    ASSERT_THROW(i16Field.set(fieldBase, 3), std::invalid_argument);
    MamaFieldCacheFieldU16 u16Field;
    ASSERT_THROW(u16Field.set(fieldBase, 3), std::invalid_argument);
    MamaFieldCacheFieldI32 i32Field;
    ASSERT_THROW(i32Field.set(fieldBase, 3), std::invalid_argument);
    MamaFieldCacheFieldU32 u32Field;
    ASSERT_THROW(u32Field.set(fieldBase, 3), std::invalid_argument);
    MamaFieldCacheFieldI64 i64Field;
    ASSERT_THROW(i64Field.set(fieldBase, 3), std::invalid_argument);
    MamaFieldCacheFieldU64 u64Field;
    ASSERT_THROW(u64Field.set(fieldBase, 3), std::invalid_argument);
    MamaFieldCacheFieldF32 f32Field;
    ASSERT_THROW(f32Field.set(fieldBase, 3), std::invalid_argument);
    MamaFieldCacheFieldF64 f64Field;
    ASSERT_THROW(f64Field.set(fieldBase, 3), std::invalid_argument);
    MamaFieldCacheFieldString stringField;
    ASSERT_THROW(stringField.set(fieldBase, "hi"), std::invalid_argument);
    MamaFieldCacheFieldPrice priceField;
    ASSERT_THROW(priceField.set(fieldBase, MamaPrice()), std::invalid_argument);
    MamaFieldCacheFieldDateTime dateTimeField;
    ASSERT_THROW(dateTimeField.set(fieldBase, MamaDateTime()), std::invalid_argument);
}

TEST_F(MamaFieldCacheFieldTypesTest, testBoolClass)
{
    MamaFieldCacheField fieldBase;
    fieldBase.create(1, MAMA_FIELD_TYPE_BOOL);
    MamaFieldCacheFieldBool field;

    field.set(fieldBase, true);
    ASSERT_TRUE(field.get(fieldBase));

    field.set(fieldBase, false);
    ASSERT_FALSE(field.get(fieldBase));
}

TEST_F(MamaFieldCacheFieldTypesTest, testBoolFunc)
{
    MamaFieldCacheField fieldBase;
    fieldBase.create(1, MAMA_FIELD_TYPE_BOOL);

    MamaFieldCacheFieldBool field;

    mama_bool_t value;
    setFieldValue(fieldBase, true);
    ASSERT_TRUE(field.get(fieldBase));
    getFieldValue(fieldBase, value);
    ASSERT_TRUE(value);

    setFieldValue(fieldBase, false);
    ASSERT_FALSE(field.get(fieldBase));
    getFieldValue(fieldBase, value);
    ASSERT_FALSE(value);
}

TEST_F(MamaFieldCacheFieldTypesTest, testCharClass)
{
    MamaFieldCacheField fieldBase;
    fieldBase.create(1, MAMA_FIELD_TYPE_CHAR, "");
    MamaFieldCacheFieldChar field;
    
    field.set(fieldBase, 'a');
    ASSERT_EQ('a', field.get(fieldBase));
    
    field.set(fieldBase, 'Z');
    ASSERT_EQ('Z', field.get(fieldBase));
    
    field.set(fieldBase, 33);
    ASSERT_EQ('!', field.get(fieldBase));
}

TEST_F(MamaFieldCacheFieldTypesTest, testCharFunc)
{
    MamaFieldCacheField fieldBase;
    fieldBase.create(1, MAMA_FIELD_TYPE_CHAR, "");
    
    MamaFieldCacheFieldChar field;
    char value = '\0';
    
    setFieldValue(fieldBase, 'a');
    ASSERT_EQ('a', field.get(fieldBase));
    getFieldValue(fieldBase, value);
    ASSERT_EQ('a', value);

    setFieldValue(fieldBase, 'Z');
    ASSERT_EQ('Z', field.get(fieldBase));
    getFieldValue(fieldBase, value);
    ASSERT_EQ('Z', value);

    setFieldValue(fieldBase, 33);
    ASSERT_EQ('!', field.get(fieldBase));
    getFieldValue(fieldBase, value);
    ASSERT_EQ('!', value);
}

TEST_F(MamaFieldCacheFieldTypesTest, testI8Class)
{
    MamaFieldCacheField fieldBase;
    fieldBase.create(1, MAMA_FIELD_TYPE_I8, "");
    MamaFieldCacheFieldI8 field;
    
    field.set(fieldBase, 8);
    ASSERT_EQ(8, field.get(fieldBase));
    
    field.set(fieldBase, -1);
    ASSERT_EQ(-1, field.get(fieldBase));
}

TEST_F(MamaFieldCacheFieldTypesTest, testI8Func)
{
    MamaFieldCacheField fieldBase;
    fieldBase.create(1, MAMA_FIELD_TYPE_I8, "");
    
    MamaFieldCacheFieldI8 field;
    mama_i8_t value = 0;
    setFieldValue(fieldBase, 8);
    ASSERT_EQ(8, field.get(fieldBase));
    getFieldValue(fieldBase, value);
    ASSERT_EQ(8, value);

    setFieldValue(fieldBase, -1);
    ASSERT_EQ(-1, field.get(fieldBase));
    getFieldValue(fieldBase, value);
    ASSERT_EQ(-1, value);
}

TEST_F(MamaFieldCacheFieldTypesTest, testU8Class)
{
    MamaFieldCacheField fieldBase;
    fieldBase.create(1, MAMA_FIELD_TYPE_U8, "");
    MamaFieldCacheFieldU8 field;
    
    field.set(fieldBase, 8);
    ASSERT_EQ(8, field.get(fieldBase));
    
    field.set(fieldBase, 2);
    ASSERT_EQ(2, field.get(fieldBase));
}

TEST_F(MamaFieldCacheFieldTypesTest, testU8Func)
{
    MamaFieldCacheField fieldBase;
    fieldBase.create(1, MAMA_FIELD_TYPE_U8, "");
    
    MamaFieldCacheFieldU8 field;
    mama_u8_t value = 0;

    setFieldValue(fieldBase, 8);
    ASSERT_EQ(8, field.get(fieldBase));
    getFieldValue(fieldBase, value);
    ASSERT_EQ(8, value);
    
    setFieldValue(fieldBase, 2);
    ASSERT_EQ(2, field.get(fieldBase));
    getFieldValue(fieldBase, value);
    ASSERT_EQ(2, value);
}

TEST_F(MamaFieldCacheFieldTypesTest, testI16Class)
{
    MamaFieldCacheField fieldBase;
    fieldBase.create(1, MAMA_FIELD_TYPE_I16, "");
    MamaFieldCacheFieldI16 field;
    
    field.set(fieldBase, 8);
    ASSERT_EQ(8, field.get(fieldBase));
    
    field.set(fieldBase, -1);
    ASSERT_EQ(-1, field.get(fieldBase));
}

TEST_F(MamaFieldCacheFieldTypesTest, testI16Func)
{
    MamaFieldCacheField fieldBase;
    fieldBase.create(1, MAMA_FIELD_TYPE_I16, "");
    
    MamaFieldCacheFieldI16 field;
    mama_i16_t value = 0;
    setFieldValue(fieldBase, 8);
    ASSERT_EQ(8, field.get(fieldBase));
    getFieldValue(fieldBase, value);
    ASSERT_EQ(8, value);

    setFieldValue(fieldBase, -1);
    ASSERT_EQ(-1, field.get(fieldBase));
    getFieldValue(fieldBase, value);
    ASSERT_EQ(-1, value);
}

TEST_F(MamaFieldCacheFieldTypesTest, testU16Class)
{
    MamaFieldCacheField fieldBase;
    fieldBase.create(1, MAMA_FIELD_TYPE_U16, "");
    MamaFieldCacheFieldU16 field;
    
    field.set(fieldBase, 8);
    ASSERT_EQ(8, field.get(fieldBase));
    
    field.set(fieldBase, 2);
    ASSERT_EQ(2, field.get(fieldBase));
}

TEST_F(MamaFieldCacheFieldTypesTest, testU16Func)
{
    MamaFieldCacheField fieldBase;
    fieldBase.create(1, MAMA_FIELD_TYPE_U16, "");
    
    MamaFieldCacheFieldU16 field;
    mama_u16_t value = 0;
    setFieldValue(fieldBase, 8);
    ASSERT_EQ(8, field.get(fieldBase));
    getFieldValue(fieldBase, value);
    ASSERT_EQ(8, value);

    setFieldValue(fieldBase, 2);
    ASSERT_EQ(2, field.get(fieldBase));
    getFieldValue(fieldBase, value);
    ASSERT_EQ(2, value);
}

TEST_F(MamaFieldCacheFieldTypesTest, testI32)
{
    MamaFieldCacheField fieldBase;
    fieldBase.create(1, MAMA_FIELD_TYPE_I32, "");
    
    MamaFieldCacheFieldI32 field;
    mama_i32_t value = 0;
    field.set(fieldBase, 8);
    ASSERT_EQ(8, field.get(fieldBase));
    getFieldValue(fieldBase, value);
    ASSERT_EQ(8, value);
    
    setFieldValue(fieldBase, -1);
    ASSERT_EQ(-1, field.get(fieldBase));
    getFieldValue(fieldBase, value);
    ASSERT_EQ(-1, value);
}

TEST_F(MamaFieldCacheFieldTypesTest, testU32)
{
    MamaFieldCacheField fieldBase;
    fieldBase.create(1, MAMA_FIELD_TYPE_U32, "");
    
    MamaFieldCacheFieldU32 field;
    mama_u32_t value = 0;
    field.set(fieldBase, 8);
    ASSERT_EQ(8, field.get(fieldBase));
    getFieldValue(fieldBase, value);
    ASSERT_EQ(8, value);
    
    setFieldValue(fieldBase, 2);
    ASSERT_EQ(2, field.get(fieldBase));
    getFieldValue(fieldBase, value);
    ASSERT_EQ(2, value);
    
    // Field is truncated
    field.set(fieldBase, 1.5);
    ASSERT_EQ(1, field.get(fieldBase));
    getFieldValue(fieldBase, value);
    ASSERT_EQ(1, value);
    
    // Field is truncated
    setFieldValue(fieldBase, 3.1);
    ASSERT_EQ(3, field.get(fieldBase));
    getFieldValue(fieldBase, value);
    ASSERT_EQ(3, value);
}

TEST_F(MamaFieldCacheFieldTypesTest, testI64)
{
    MamaFieldCacheField fieldBase;
    fieldBase.create(1, MAMA_FIELD_TYPE_I64, "");
    
    MamaFieldCacheFieldI64 field;
    mama_i64_t value = 0;
    field.set(fieldBase, 8);
    ASSERT_EQ(8, field.get(fieldBase));
    getFieldValue(fieldBase, value);
    ASSERT_EQ(8, value);
    
    setFieldValue(fieldBase, -1);
    ASSERT_EQ(-1, field.get(fieldBase));
    getFieldValue(fieldBase, value);
    ASSERT_EQ(-1, value);
}

TEST_F(MamaFieldCacheFieldTypesTest, testU64)
{
    MamaFieldCacheField fieldBase;
    fieldBase.create(1, MAMA_FIELD_TYPE_U64, "");
    
    MamaFieldCacheFieldU64 field;
    mama_i64_t value = 0;
    field.set(fieldBase, 8);
    ASSERT_EQ(8, field.get(fieldBase));
    getFieldValue(fieldBase, value);
    ASSERT_EQ(8, value);
    
    setFieldValue(fieldBase, 2);
    ASSERT_EQ(2, field.get(fieldBase));
    getFieldValue(fieldBase, value);
    ASSERT_EQ(2, value);
}

TEST_F(MamaFieldCacheFieldTypesTest, testF32)
{
    MamaFieldCacheField fieldBase;
    fieldBase.create(1, MAMA_FIELD_TYPE_F32, "");
    
    MamaFieldCacheFieldF32 field;
    mama_f32_t value = 0;
    field.set(fieldBase, 8);
    ASSERT_FLOAT_EQ(8, field.get(fieldBase));
    getFieldValue(fieldBase, value);
    ASSERT_FLOAT_EQ(8, value);
    
    setFieldValue(fieldBase, -2.1);
    ASSERT_FLOAT_EQ(-2.1, field.get(fieldBase));
    getFieldValue(fieldBase, value);
    ASSERT_FLOAT_EQ(-2.1, value);
}

TEST_F(MamaFieldCacheFieldTypesTest, testF64)
{
    MamaFieldCacheField fieldBase;
    fieldBase.create(1, MAMA_FIELD_TYPE_F64, "");
    
    MamaFieldCacheFieldF64 field;
    mama_f64_t value = 0;
    field.set(fieldBase, 8);
    ASSERT_DOUBLE_EQ(8, field.get(fieldBase));
    getFieldValue(fieldBase, value);
    ASSERT_DOUBLE_EQ(8, value);
    
    setFieldValue(fieldBase, 2.1);
    ASSERT_DOUBLE_EQ(2.1, field.get(fieldBase));
    getFieldValue(fieldBase, value);
    ASSERT_DOUBLE_EQ(2.1, value);
}

TEST_F(MamaFieldCacheFieldTypesTest, testString)
{
    MamaFieldCacheField fieldBase;
    fieldBase.create(1, MAMA_FIELD_TYPE_STRING, "");
    
    MamaFieldCacheFieldString field;
    const char* value;
    const char* cstr = "abc";
    field.set(fieldBase, cstr);
    ASSERT_STREQ("abc", field.get(fieldBase));
    getFieldValue(fieldBase, value);
    ASSERT_STREQ("abc", value);

    cstr = "cba1";
    setFieldValue(fieldBase, cstr);
    const char* res;
    res = field.get(fieldBase);
    ASSERT_STREQ(cstr, res);
    getFieldValue(fieldBase, value);
    ASSERT_STREQ("cba1", value);
}

TEST_F(MamaFieldCacheFieldTypesTest, testPrice)
{
    MamaFieldCacheField fieldBase;
    fieldBase.create(1, MAMA_FIELD_TYPE_PRICE, "");
    MamaFieldCacheFieldPrice field;
    const char* s;
    
    MamaPrice value;
    MamaPrice price1;
    price1.setValue(123.2);
    field.set(fieldBase, price1);
    s = price1.getAsString(); // let this MamaPrice allocate some memory
    
    MamaPrice priceRet1 = field.get(fieldBase);
    ASSERT_DOUBLE_EQ(123.2, priceRet1.getValue());
    s = priceRet1.getAsString();
    getFieldValue(fieldBase, value);
    ASSERT_DOUBLE_EQ(123.2, value.getValue());
    
    MamaPrice price2;
    price2.setValue(10);
    setFieldValue(fieldBase, price2);
    s = price2.getAsString();

    MamaPrice priceRet2 = field.get(fieldBase);
    ASSERT_DOUBLE_EQ(10, priceRet2.getValue());
    s = priceRet2.getAsString();
    getFieldValue(fieldBase, value);
    ASSERT_DOUBLE_EQ(10, value.getValue());
}

TEST_F(MamaFieldCacheFieldTypesTest, testDateTime)
{
    MamaFieldCacheField fieldBase;
    fieldBase.create(1, MAMA_FIELD_TYPE_TIME, "");
    MamaFieldCacheFieldDateTime field;
    const char* s;
    
    MamaDateTime value;
    MamaDateTime dateTime1;
    dateTime1.setEpochTimeF64(2000);
    field.set(fieldBase, dateTime1);
    s = dateTime1.getAsString();
    
    MamaDateTime dateTimeRet1 = field.get(fieldBase);
    ASSERT_DOUBLE_EQ(2000, dateTimeRet1.getEpochTimeSeconds());
    s = dateTimeRet1.getAsString();
    getFieldValue(fieldBase, value);
    ASSERT_DOUBLE_EQ(2000, value.getEpochTimeSeconds());
    
    MamaDateTime dateTime2;
    dateTime2.setEpochTimeF64(2500);
    setFieldValue(fieldBase, dateTime2);
    s = dateTime2.getAsString();
    
    MamaDateTime dateTimeRet2 = field.get(fieldBase);
    ASSERT_DOUBLE_EQ(2500, dateTimeRet2.getEpochTimeSeconds());
    s = dateTimeRet2.getAsString();
    getFieldValue(fieldBase, value);
    ASSERT_DOUBLE_EQ(2500, value.getEpochTimeSeconds());
}

TEST_F(MamaFieldCacheFieldTypesTest, testI8Vector)
{
    MamaFieldCacheField fieldBase;
    fieldBase.create(1, MAMA_FIELD_TYPE_VECTOR_I8, "");
    MamaFieldCacheFieldI8Vector field;
    
    mama_i8_t values[5] = { 0, 1, -2, 3, 4 };
    field.set(fieldBase, values, 5);
    ASSERT_EQ(0, field.get(fieldBase, 0));
    ASSERT_EQ(1, field.get(fieldBase, 1));
    ASSERT_EQ(-2, field.get(fieldBase, 2));
    ASSERT_EQ(3, field.get(fieldBase, 3));
    ASSERT_EQ(4, field.get(fieldBase, 4));
    
    const mama_i8_t* result = NULL;
    mama_size_t size;
    field.get(fieldBase, result, size);
    ASSERT_EQ(5, size);
    ASSERT_EQ(0, result[0]);
    ASSERT_EQ(1, result[1]);
    ASSERT_EQ(-2, result[2]);
    ASSERT_EQ(3, result[3]);
    ASSERT_EQ(4, result[4]);
    getFieldValue(fieldBase, result, size);
    ASSERT_EQ(5, size);
    ASSERT_EQ(0, result[0]);
    ASSERT_EQ(1, result[1]);
    ASSERT_EQ(-2, result[2]);
    ASSERT_EQ(3, result[3]);
    ASSERT_EQ(4, result[4]);
    
    values[2] = 2;
    setFieldValue(fieldBase, values, 5);
    ASSERT_EQ(0, field.get(fieldBase, 0));
    ASSERT_EQ(1, field.get(fieldBase, 1));
    ASSERT_EQ(2, field.get(fieldBase, 2));
    ASSERT_EQ(3, field.get(fieldBase, 3));
    ASSERT_EQ(4, field.get(fieldBase, 4));
}

TEST_F(MamaFieldCacheFieldTypesTest, testU8Vector)
{
    MamaFieldCacheField fieldBase;
    fieldBase.create(1, MAMA_FIELD_TYPE_VECTOR_U8, "");
    MamaFieldCacheFieldU8Vector field;
    
    mama_u8_t values[5] = { 0, 1, 2, 3, 4 };
    field.set(fieldBase, values, 5);
    ASSERT_EQ(0, field.get(fieldBase, 0));
    ASSERT_EQ(1, field.get(fieldBase, 1));
    ASSERT_EQ(2, field.get(fieldBase, 2));
    ASSERT_EQ(3, field.get(fieldBase, 3));
    ASSERT_EQ(4, field.get(fieldBase, 4));
    
    const mama_u8_t* result;
    std::size_t size;
    field.get(fieldBase, result, size);
    ASSERT_EQ(5, size);    
    ASSERT_EQ(0, result[0]);
    ASSERT_EQ(1, result[1]);
    ASSERT_EQ(2, result[2]);
    ASSERT_EQ(3, result[3]);
    ASSERT_EQ(4, result[4]);
    getFieldValue(fieldBase, result, size);
    ASSERT_EQ(5, size);    
    ASSERT_EQ(0, result[0]);
    ASSERT_EQ(1, result[1]);
    ASSERT_EQ(2, result[2]);
    ASSERT_EQ(3, result[3]);
    ASSERT_EQ(4, result[4]);
    
    values[0] = 5;
    setFieldValue(fieldBase, values, 5);
    ASSERT_EQ(5, field.get(fieldBase, 0));
    ASSERT_EQ(1, field.get(fieldBase, 1));
    ASSERT_EQ(2, field.get(fieldBase, 2));
    ASSERT_EQ(3, field.get(fieldBase, 3));
    ASSERT_EQ(4, field.get(fieldBase, 4));
}

TEST_F(MamaFieldCacheFieldTypesTest, testI16Vector)
{
    MamaFieldCacheField fieldBase;
    fieldBase.create(1, MAMA_FIELD_TYPE_VECTOR_I16, "");
    MamaFieldCacheFieldI16Vector field;
    
    mama_i16_t values[5] = { 0, 1, -2, 3, 4 };
    field.set(fieldBase, values, 5);
    ASSERT_EQ(0, field.get(fieldBase, 0));
    ASSERT_EQ(1, field.get(fieldBase, 1));
    ASSERT_EQ(-2, field.get(fieldBase, 2));
    ASSERT_EQ(3, field.get(fieldBase, 3));
    ASSERT_EQ(4, field.get(fieldBase, 4));
    
    const mama_i16_t* result;
    std::size_t size;
    field.get(fieldBase, result, size);
    ASSERT_EQ(5, size);
    ASSERT_EQ(0, result[0]);
    ASSERT_EQ(1, result[1]);
    ASSERT_EQ(-2, result[2]);
    ASSERT_EQ(3, result[3]);
    ASSERT_EQ(4, result[4]);
    getFieldValue(fieldBase, result, size);
    ASSERT_EQ(5, size);
    ASSERT_EQ(0, result[0]);
    ASSERT_EQ(1, result[1]);
    ASSERT_EQ(-2, result[2]);
    ASSERT_EQ(3, result[3]);
    ASSERT_EQ(4, result[4]);
    
    values[2] = 2;
    setFieldValue(fieldBase, values, 5);
    ASSERT_EQ(0, field.get(fieldBase, 0));
    ASSERT_EQ(1, field.get(fieldBase, 1));
    ASSERT_EQ(2, field.get(fieldBase, 2));
    ASSERT_EQ(3, field.get(fieldBase, 3));
    ASSERT_EQ(4, field.get(fieldBase, 4));
}

TEST_F(MamaFieldCacheFieldTypesTest, testU16Vector)
{
    MamaFieldCacheField fieldBase;
    fieldBase.create(1, MAMA_FIELD_TYPE_VECTOR_U16, "");
    MamaFieldCacheFieldU16Vector field;
    
    mama_u16_t values[5] = { 0, 1, 2, 3, 4 };
    field.set(fieldBase, values, 5);
    ASSERT_EQ(0, field.get(fieldBase, 0));
    ASSERT_EQ(1, field.get(fieldBase, 1));
    ASSERT_EQ(2, field.get(fieldBase, 2));
    ASSERT_EQ(3, field.get(fieldBase, 3));
    ASSERT_EQ(4, field.get(fieldBase, 4));
    
    const mama_u16_t* result = NULL;
    std::size_t size = 0;
    field.get(fieldBase, result, size);
    ASSERT_EQ(5, size);
    ASSERT_EQ(0, result[0]);
    ASSERT_EQ(1, result[1]);
    ASSERT_EQ(2, result[2]);
    ASSERT_EQ(3, result[3]);
    ASSERT_EQ(4, result[4]);
    getFieldValue(fieldBase, result, size);
    ASSERT_EQ(5, size);
    ASSERT_EQ(0, result[0]);
    ASSERT_EQ(1, result[1]);
    ASSERT_EQ(2, result[2]);
    ASSERT_EQ(3, result[3]);
    ASSERT_EQ(4, result[4]);
    
    values[0] = 5;
    setFieldValue(fieldBase, values, 5);
    ASSERT_EQ(5, field.get(fieldBase, 0));
    ASSERT_EQ(1, field.get(fieldBase, 1));
    ASSERT_EQ(2, field.get(fieldBase, 2));
    ASSERT_EQ(3, field.get(fieldBase, 3));
    ASSERT_EQ(4, field.get(fieldBase, 4));
}

TEST_F(MamaFieldCacheFieldTypesTest, testI32Vector)
{
    MamaFieldCacheField fieldBase;
    fieldBase.create(1, MAMA_FIELD_TYPE_VECTOR_I32, "");
    MamaFieldCacheFieldI32Vector field;
    
    mama_i32_t values[5] = { 0, 1, -2, 3, 4 };
    field.set(fieldBase, values, 5);
    ASSERT_EQ(0, field.get(fieldBase, 0));
    ASSERT_EQ(1, field.get(fieldBase, 1));
    ASSERT_EQ(-2, field.get(fieldBase, 2));
    ASSERT_EQ(3, field.get(fieldBase, 3));
    ASSERT_EQ(4, field.get(fieldBase, 4));
    
    const mama_i32_t* result = NULL;
    std::size_t size = 0;
    field.get(fieldBase, result, size);
    ASSERT_EQ(5, size);
    ASSERT_EQ(0, result[0]);
    ASSERT_EQ(1, result[1]);
    ASSERT_EQ(-2, result[2]);
    ASSERT_EQ(3, result[3]);
    ASSERT_EQ(4, result[4]);
    getFieldValue(fieldBase, result, size);
    ASSERT_EQ(5, size);
    ASSERT_EQ(0, result[0]);
    ASSERT_EQ(1, result[1]);
    ASSERT_EQ(-2, result[2]);
    ASSERT_EQ(3, result[3]);
    ASSERT_EQ(4, result[4]);
    
    values[2] = 2;
    setFieldValue(fieldBase, values, 5);
    ASSERT_EQ(0, field.get(fieldBase, 0));
    ASSERT_EQ(1, field.get(fieldBase, 1));
    ASSERT_EQ(2, field.get(fieldBase, 2));
    ASSERT_EQ(3, field.get(fieldBase, 3));
    ASSERT_EQ(4, field.get(fieldBase, 4));
}

TEST_F(MamaFieldCacheFieldTypesTest, testU32Vector)
{
    MamaFieldCacheField fieldBase;
    fieldBase.create(1, MAMA_FIELD_TYPE_VECTOR_U32, "");
    MamaFieldCacheFieldU32Vector field;
    
    mama_u32_t values[5] = { 0, 1, 2, 3, 4 };
    field.set(fieldBase, values, 5);
    ASSERT_EQ(0, field.get(fieldBase, 0));
    ASSERT_EQ(1, field.get(fieldBase, 1));
    ASSERT_EQ(2, field.get(fieldBase, 2));
    ASSERT_EQ(3, field.get(fieldBase, 3));
    ASSERT_EQ(4, field.get(fieldBase, 4));
    
    const mama_u32_t* result = NULL;
    std::size_t size = 0;
    field.get(fieldBase, result, size);
    ASSERT_EQ(5, size);
    ASSERT_EQ(0, result[0]);
    ASSERT_EQ(1, result[1]);
    ASSERT_EQ(2, result[2]);
    ASSERT_EQ(3, result[3]);
    ASSERT_EQ(4, result[4]);
    getFieldValue(fieldBase, result, size);
    ASSERT_EQ(5, size);
    ASSERT_EQ(0, result[0]);
    ASSERT_EQ(1, result[1]);
    ASSERT_EQ(2, result[2]);
    ASSERT_EQ(3, result[3]);
    ASSERT_EQ(4, result[4]);

    values[0] = 5;
    setFieldValue(fieldBase, values, 5);
    ASSERT_EQ(5, field.get(fieldBase, 0));
    ASSERT_EQ(1, field.get(fieldBase, 1));
    ASSERT_EQ(2, field.get(fieldBase, 2));
    ASSERT_EQ(3, field.get(fieldBase, 3));
    ASSERT_EQ(4, field.get(fieldBase, 4));
}

TEST_F(MamaFieldCacheFieldTypesTest, testI64Vector)
{
    MamaFieldCacheField fieldBase;
    fieldBase.create(1, MAMA_FIELD_TYPE_VECTOR_I64, "");
    MamaFieldCacheFieldI64Vector field;
    
    mama_i64_t values[5] = { 0, 1, -2, 3, 4 };
    field.set(fieldBase, values, 5);
    ASSERT_EQ(0, field.get(fieldBase, 0));
    ASSERT_EQ(1, field.get(fieldBase, 1));
    ASSERT_EQ(-2, field.get(fieldBase, 2));
    ASSERT_EQ(3, field.get(fieldBase, 3));
    ASSERT_EQ(4, field.get(fieldBase, 4));
    
    const mama_i64_t* result = NULL;
    std::size_t size = 0;
    field.get(fieldBase, result, size);
    ASSERT_EQ(5, size);
    ASSERT_EQ(0, result[0]);
    ASSERT_EQ(1, result[1]);
    ASSERT_EQ(-2, result[2]);
    ASSERT_EQ(3, result[3]);
    ASSERT_EQ(4, result[4]);
    getFieldValue(fieldBase, result, size);
    ASSERT_EQ(5, size);
    ASSERT_EQ(0, result[0]);
    ASSERT_EQ(1, result[1]);
    ASSERT_EQ(-2, result[2]);
    ASSERT_EQ(3, result[3]);
    ASSERT_EQ(4, result[4]);
    
    values[2] = 2;
    setFieldValue(fieldBase, values, 5);
    ASSERT_EQ(0, field.get(fieldBase, 0));
    ASSERT_EQ(1, field.get(fieldBase, 1));
    ASSERT_EQ(2, field.get(fieldBase, 2));
    ASSERT_EQ(3, field.get(fieldBase, 3));
    ASSERT_EQ(4, field.get(fieldBase, 4));
}

TEST_F(MamaFieldCacheFieldTypesTest, testU64Vector)
{
    MamaFieldCacheField fieldBase;
    fieldBase.create(1, MAMA_FIELD_TYPE_VECTOR_U64, "");
    MamaFieldCacheFieldU64Vector field;
    
    mama_u64_t values[5] = { 0, 1, 2, 3, 4 };
    field.set(fieldBase, values, 5);
    ASSERT_EQ(0, field.get(fieldBase, 0));
    ASSERT_EQ(1, field.get(fieldBase, 1));
    ASSERT_EQ(2, field.get(fieldBase, 2));
    ASSERT_EQ(3, field.get(fieldBase, 3));
    ASSERT_EQ(4, field.get(fieldBase, 4));
    
    const mama_u64_t* result = NULL;
    std::size_t size = 0;
    field.get(fieldBase, result, size);
    ASSERT_EQ(5, size);    
    ASSERT_EQ(0, result[0]);
    ASSERT_EQ(1, result[1]);
    ASSERT_EQ(2, result[2]);
    ASSERT_EQ(3, result[3]);
    ASSERT_EQ(4, result[4]);
    getFieldValue(fieldBase, result, size);
    ASSERT_EQ(5, size);
    ASSERT_EQ(0, result[0]);
    ASSERT_EQ(1, result[1]);
    ASSERT_EQ(2, result[2]);
    ASSERT_EQ(3, result[3]);
    ASSERT_EQ(4, result[4]);
    
    values[0] = 5;
    setFieldValue(fieldBase, values, 5);
    ASSERT_EQ(5, field.get(fieldBase, 0));
    ASSERT_EQ(1, field.get(fieldBase, 1));
    ASSERT_EQ(2, field.get(fieldBase, 2));
    ASSERT_EQ(3, field.get(fieldBase, 3));
    ASSERT_EQ(4, field.get(fieldBase, 4));
}

TEST_F(MamaFieldCacheFieldTypesTest, testF32Vector)
{
    MamaFieldCacheField fieldBase;
    fieldBase.create(1, MAMA_FIELD_TYPE_VECTOR_F32, "");
    MamaFieldCacheFieldF32Vector field;
    
    mama_f32_t values[5] = { 0.1, 1, -2, 3, 4.1 };
    field.set(fieldBase, values, 5);
    ASSERT_FLOAT_EQ(0.1, field.get(fieldBase, 0));
    ASSERT_FLOAT_EQ(1, field.get(fieldBase, 1));
    ASSERT_FLOAT_EQ(-2, field.get(fieldBase, 2));
    ASSERT_FLOAT_EQ(3, field.get(fieldBase, 3));
    ASSERT_FLOAT_EQ(4.1, field.get(fieldBase, 4));
    
    const mama_f32_t* result = NULL;
    std::size_t size = 0;
    field.get(fieldBase, result, size);
    ASSERT_EQ(5, size);
    ASSERT_TRUE(result);
    ASSERT_FLOAT_EQ(0.1, result[0]);
    ASSERT_FLOAT_EQ(1, result[1]);
    ASSERT_FLOAT_EQ(-2, result[2]);
    ASSERT_FLOAT_EQ(3, result[3]);
    ASSERT_FLOAT_EQ(4.1, result[4]);
    getFieldValue(fieldBase, result, size);
    ASSERT_EQ(5, size);    
    ASSERT_FLOAT_EQ(0.1, result[0]);
    ASSERT_FLOAT_EQ(1, result[1]);
    ASSERT_FLOAT_EQ(-2, result[2]);
    ASSERT_FLOAT_EQ(3, result[3]);
    ASSERT_FLOAT_EQ(4.1, result[4]);
    
    values[2] = 2.5;
    setFieldValue(fieldBase, values, 5);
    ASSERT_FLOAT_EQ(0.1, field.get(fieldBase, 0));
    ASSERT_FLOAT_EQ(1, field.get(fieldBase, 1));
    ASSERT_FLOAT_EQ(2.5, field.get(fieldBase, 2));
    ASSERT_FLOAT_EQ(3, field.get(fieldBase, 3));
    ASSERT_FLOAT_EQ(4.1, field.get(fieldBase, 4));
}

TEST_F(MamaFieldCacheFieldTypesTest, testF64Vector)
{
    MamaFieldCacheField fieldBase;
    fieldBase.create(1, MAMA_FIELD_TYPE_VECTOR_F64, "");
    MamaFieldCacheFieldF64Vector field;
    
    mama_f64_t values[5] = { 0.1, 1, -2, 3, 4.1 };
    field.set(fieldBase, values, 5);
    ASSERT_DOUBLE_EQ(0.1, field.get(fieldBase, 0));
    ASSERT_DOUBLE_EQ(1, field.get(fieldBase, 1));
    ASSERT_DOUBLE_EQ(-2, field.get(fieldBase, 2));
    ASSERT_DOUBLE_EQ(3, field.get(fieldBase, 3));
    ASSERT_DOUBLE_EQ(4.1, field.get(fieldBase, 4));
    
    const mama_f64_t* result = NULL;
    std::size_t size = 0;
    field.get(fieldBase, result, size);
    ASSERT_EQ(5, size);
    ASSERT_TRUE(result);
    ASSERT_DOUBLE_EQ(0.1, result[0]);
    ASSERT_DOUBLE_EQ(1, result[1]);
    ASSERT_DOUBLE_EQ(-2, result[2]);
    ASSERT_DOUBLE_EQ(3, result[3]);
    ASSERT_DOUBLE_EQ(4.1, result[4]);
    getFieldValue(fieldBase, result, size);
    ASSERT_EQ(5, size);    
    ASSERT_DOUBLE_EQ(0.1, result[0]);
    ASSERT_DOUBLE_EQ(1, result[1]);
    ASSERT_DOUBLE_EQ(-2, result[2]);
    ASSERT_DOUBLE_EQ(3, result[3]);
    ASSERT_DOUBLE_EQ(4.1, result[4]);
    
    values[2] = 2.5;
    setFieldValue(fieldBase, values, 5);
    ASSERT_DOUBLE_EQ(0.1, field.get(fieldBase, 0));
    ASSERT_DOUBLE_EQ(1, field.get(fieldBase, 1));
    ASSERT_DOUBLE_EQ(2.5, field.get(fieldBase, 2));
    ASSERT_DOUBLE_EQ(3, field.get(fieldBase, 3));
    ASSERT_DOUBLE_EQ(4.1, field.get(fieldBase, 4));
}

TEST_F(MamaFieldCacheFieldTypesTest, testStringVector)
{
    MamaFieldCacheField fieldBase;
    fieldBase.create(1, MAMA_FIELD_TYPE_VECTOR_STRING, "");
    MamaFieldCacheFieldStringVector field;
    
    const char* values[5] = { "0.1", "1", "-2", "hello", "4.1" };
    field.set(fieldBase, values, 5);
    ASSERT_STREQ("0.1", field.get(fieldBase, 0));
    ASSERT_STREQ("1", field.get(fieldBase, 1));
    ASSERT_STREQ("-2", field.get(fieldBase, 2));
    ASSERT_STREQ("hello", field.get(fieldBase, 3));
    ASSERT_STREQ("4.1", field.get(fieldBase, 4));
    
    const char** result = NULL;
    std::size_t size = 0;
    field.get(fieldBase, result, size);
    ASSERT_EQ(5, size);
    ASSERT_TRUE(result);
    ASSERT_STREQ("0.1", result[0]);
    ASSERT_STREQ("1", result[1]);
    ASSERT_STREQ("-2", result[2]);
    ASSERT_STREQ("hello", result[3]);
    ASSERT_STREQ("4.1", result[4]);
    getFieldValue(fieldBase, result, size);
    ASSERT_EQ(5, size);    
    ASSERT_STREQ("0.1", result[0]);
    ASSERT_STREQ("1", result[1]);
    ASSERT_STREQ("-2", result[2]);
    ASSERT_STREQ("hello", result[3]);
    ASSERT_STREQ("4.1", result[4]);
    
    values[2] = "2.5 modified";
    setFieldValue(fieldBase, values, 5);
    ASSERT_STREQ("0.1", field.get(fieldBase, 0));
    ASSERT_STREQ("1", field.get(fieldBase, 1));
    ASSERT_STREQ("2.5 modified", field.get(fieldBase, 2));
    ASSERT_STREQ("hello", field.get(fieldBase, 3));
    ASSERT_STREQ("4.1", field.get(fieldBase, 4));
}

TEST_F(MamaFieldCacheFieldTypesTest, testPriceVector)
{
    MamaFieldCacheField fieldBase;
    fieldBase.create(1, MAMA_FIELD_TYPE_VECTOR_PRICE, "");
    MamaFieldCacheFieldPriceVector field;
    
    MamaPrice values[5];
    values[0].setValue(0.1);
    values[1].setValue(1);
    values[2].setValue(-2);
    values[3].setValue(3);
    values[4].setValue(4.1);
    
    field.set(fieldBase, values, 5);
    ASSERT_DOUBLE_EQ(0.1, field.get(fieldBase, 0)->getValue());
    ASSERT_DOUBLE_EQ(1, field.get(fieldBase, 1)->getValue());
    ASSERT_DOUBLE_EQ(-2, field.get(fieldBase, 2)->getValue());
    ASSERT_DOUBLE_EQ(3, field.get(fieldBase, 3)->getValue());
    ASSERT_DOUBLE_EQ(4.1, field.get(fieldBase, 4)->getValue());
    ASSERT_FALSE(field.get(fieldBase, 10));
    
    const MamaPrice* result = NULL;
    std::size_t size = 0;
    field.get(fieldBase, result, size);
    ASSERT_TRUE(result);
    ASSERT_EQ(5, size);
    ASSERT_DOUBLE_EQ(0.1, result[0].getValue());
    ASSERT_DOUBLE_EQ(1, result[1].getValue());
    ASSERT_DOUBLE_EQ(-2, result[2].getValue());
    ASSERT_DOUBLE_EQ(3, result[3].getValue());
    ASSERT_DOUBLE_EQ(4.1, result[4].getValue());
//    getFieldValue(fieldBase, result, size);
//    ASSERT_TRUE(result);
//    ASSERT_EQ(5, size);
//    ASSERT_DOUBLE_EQ(0.1, result[0].getValue());
//    ASSERT_DOUBLE_EQ(1, result[1].getValue());
//    ASSERT_DOUBLE_EQ(-2, result[2].getValue());
//    ASSERT_DOUBLE_EQ(3, result[3].getValue());
//    ASSERT_DOUBLE_EQ(4.1, result[4].getValue());
    
    values[2].setValue(55.5);
    setFieldValue(fieldBase, values, 5);
    field.get(fieldBase, result, size);
    ASSERT_TRUE(result);
    ASSERT_EQ(5, size);
    ASSERT_DOUBLE_EQ(0.1, result[0].getValue());
    ASSERT_DOUBLE_EQ(1, result[1].getValue());
    ASSERT_DOUBLE_EQ(55.5, result[2].getValue());
    ASSERT_DOUBLE_EQ(3, result[3].getValue());
    ASSERT_DOUBLE_EQ(4.1, result[4].getValue());
}

TEST_F(MamaFieldCacheFieldTypesTest, testDateTimeVector)
{
    MamaFieldCacheField fieldBase;
    fieldBase.create(1, MAMA_FIELD_TYPE_VECTOR_TIME, "");
    MamaFieldCacheFieldDateTimeVector field;
    
    MamaDateTime values[5];
    values[0].setEpochTimeF64(1000);
    values[1].setEpochTimeF64(2000);
    values[2].setEpochTimeF64(3000);
    values[3].setEpochTimeF64(4000);
    values[4].setEpochTimeF64(5000);
    
    field.set(fieldBase, values, 5);
    ASSERT_DOUBLE_EQ(1000, field.get(fieldBase, 0)->getEpochTimeSeconds());
    ASSERT_DOUBLE_EQ(2000, field.get(fieldBase, 1)->getEpochTimeSeconds());
    ASSERT_DOUBLE_EQ(3000, field.get(fieldBase, 2)->getEpochTimeSeconds());
    ASSERT_DOUBLE_EQ(4000, field.get(fieldBase, 3)->getEpochTimeSeconds());
    ASSERT_DOUBLE_EQ(5000, field.get(fieldBase, 4)->getEpochTimeSeconds());
    ASSERT_FALSE(field.get(fieldBase, 10));
    
    const MamaDateTime* result = NULL;
    std::size_t size = 0;
    field.get(fieldBase, result, size);
    ASSERT_TRUE(result);
    ASSERT_EQ(5, size);
    ASSERT_DOUBLE_EQ(1000, result[0].getEpochTimeSeconds());
    ASSERT_DOUBLE_EQ(2000, result[1].getEpochTimeSeconds());
    ASSERT_DOUBLE_EQ(3000, result[2].getEpochTimeSeconds());
    ASSERT_DOUBLE_EQ(4000, result[3].getEpochTimeSeconds());
    ASSERT_DOUBLE_EQ(5000, result[4].getEpochTimeSeconds());
//    getFieldValue(fieldBase, result, size);
//    ASSERT_TRUE(result);
//    ASSERT_EQ(5, size);
//    ASSERT_DOUBLE_EQ(1000, result[0].getEpochTimeSeconds());
//    ASSERT_DOUBLE_EQ(2000, result[1].getEpochTimeSeconds());
//    ASSERT_DOUBLE_EQ(3000, result[2].getEpochTimeSeconds());
//    ASSERT_DOUBLE_EQ(4000, result[3].getEpochTimeSeconds());
//    ASSERT_DOUBLE_EQ(5000, result[4].getEpochTimeSeconds());
    
    values[2].setEpochTimeF64(6000);
    setFieldValue(fieldBase, values, 5);
    
    field.get(fieldBase, result, size);
    ASSERT_TRUE(result);
    ASSERT_EQ(5, size);
    ASSERT_DOUBLE_EQ(1000, result[0].getEpochTimeSeconds());
    ASSERT_DOUBLE_EQ(2000, result[1].getEpochTimeSeconds());
    ASSERT_DOUBLE_EQ(6000, result[2].getEpochTimeSeconds());
    ASSERT_DOUBLE_EQ(4000, result[3].getEpochTimeSeconds());
    ASSERT_DOUBLE_EQ(5000, result[4].getEpochTimeSeconds());
}

TEST_F(MamaFieldCacheFieldTypesTest, testFromString)
{
    MamaFieldCacheField field1;
    field1.create(1, MAMA_FIELD_TYPE_CHAR);
    ASSERT_EQ(MAMA_FIELD_TYPE_CHAR, field1.getType());
    mamaFieldCacheFieldFromString(field1, "a");
    
    MamaFieldCacheFieldChar charField;
    ASSERT_EQ('a', charField.get(field1));
    
    MamaFieldCacheField field2;
    field2.create(2, MAMA_FIELD_TYPE_I32);
    mamaFieldCacheFieldFromString(field2, "321");
    
    MamaFieldCacheFieldI32 i32Field;
    ASSERT_EQ(321, i32Field.get(field2));
    
    MamaFieldCacheField field3;
    field3.create(3, MAMA_FIELD_TYPE_BOOL);
    mamaFieldCacheFieldFromString(field3, "1");
    
    MamaFieldCacheFieldBool boolField;
    ASSERT_TRUE(boolField.get(field3));
    
    MamaFieldCacheField field4;
    field4.create(4, MAMA_FIELD_TYPE_STRING);
    mamaFieldCacheFieldFromString(field4, "hello");
    
    MamaFieldCacheFieldString stringField;
    ASSERT_STREQ("hello", stringField.get(field4));
    
    MamaFieldCacheField field5;
    field5.create(5, MAMA_FIELD_TYPE_F32);
    mamaFieldCacheFieldFromString(field5, "3.21");
    
    MamaFieldCacheFieldF32 f32Field;
    ASSERT_FLOAT_EQ(3.21, f32Field.get(field5));
}
