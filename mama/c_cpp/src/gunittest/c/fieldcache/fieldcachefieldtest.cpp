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
#include "MainUnitTestC.h"
#include <mama/price.h>
#include <mama/datetime.h>
#include <mama/fieldcache/fieldcachefield.h>
#include "fieldcache/fieldcachefieldimpl.h"

class MamaFieldCacheFieldTestC : public ::testing::Test
{
protected:

    /* Work around for problem in gtest where the this pointer can't be accessed
     * from a test fixture.
     */
    MamaFieldCacheFieldTestC *m_this;

    MamaFieldCacheFieldTestC(void);
    virtual ~MamaFieldCacheFieldTestC(void);

    virtual void SetUp(void);
    virtual void TearDown(void);
};

MamaFieldCacheFieldTestC::MamaFieldCacheFieldTestC(void)
    : m_this (NULL)
{
}

MamaFieldCacheFieldTestC::~MamaFieldCacheFieldTestC(void)
{
}

void MamaFieldCacheFieldTestC::SetUp(void)
{
    m_this = this;
}

void MamaFieldCacheFieldTestC::TearDown(void)
{
    m_this = NULL;
}

#define CREATE_FIELD(id, type) \
    mamaFieldCacheField field = NULL; \
    mama_status ret = mamaFieldCacheField_create(&field, id, type, NULL); \
    ASSERT_EQ(MAMA_STATUS_OK, ret);

#define DESTROY_FIELD \
    ret = mamaFieldCacheField_destroy(field); \
    ASSERT_EQ(MAMA_STATUS_OK, ret);

TEST_F(MamaFieldCacheFieldTestC, alignment)
{
    ASSERT_EQ(4, sizeof(mamaFieldType));
    ASSERT_EQ(2, sizeof(mama_fid_t));
    ASSERT_EQ(1, sizeof(mama_bool_t));

    int numBytes = sizeof(int*);
    if (numBytes == 4) // 32bit
    {
//        ASSERT_EQ(2, sizeof(mamaFieldCacheFieldImpl));
    }
    else //64bit
    {
//        ASSERT_EQ(32, sizeof(mamaFieldCacheFieldImpl));
    }
}

TEST_F(MamaFieldCacheFieldTestC, isSupported)
{
    ASSERT_FALSE(mamaFieldCacheField_isSupported(MAMA_FIELD_TYPE_UNKNOWN));
    ASSERT_TRUE(mamaFieldCacheField_isSupported(MAMA_FIELD_TYPE_I16));
    ASSERT_TRUE(mamaFieldCacheField_isSupported(MAMA_FIELD_TYPE_F32));
    ASSERT_TRUE(mamaFieldCacheField_isSupported(MAMA_FIELD_TYPE_F64));
}

TEST_F(MamaFieldCacheFieldTestC, create)
{
    mamaFieldCacheField field = NULL;
    mama_status ret = mamaFieldCacheField_create(&field, 1, MAMA_FIELD_TYPE_I8, NULL);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_EQ(1, field->mFid);
    ASSERT_EQ(MAMA_FIELD_TYPE_I8, field->mType);

    ASSERT_FALSE(field->mIsModified);
    ASSERT_TRUE(field->mPublish);
    ASSERT_TRUE(field->mCheckModified);
    ASSERT_FALSE(field->mName);

    mama_bool_t value;
    
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_isModified(field, &value));
    ASSERT_EQ(field->mIsModified, value);

    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getPublish(field, &value));
    ASSERT_EQ(field->mPublish, value);
    
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getCheckModified(field, &value));
    ASSERT_EQ(field->mCheckModified, value);

    ret = mamaFieldCacheField_destroy(field);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
}

TEST_F(MamaFieldCacheFieldTestC, createWithName)
{
    mamaFieldCacheField field = NULL;
    mama_status ret = mamaFieldCacheField_create(&field, 1, MAMA_FIELD_TYPE_I8, "my new field");
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_EQ(1, field->mFid);
    ASSERT_EQ(MAMA_FIELD_TYPE_I8, field->mType);

    ASSERT_FALSE(field->mIsModified);
    ASSERT_TRUE(field->mPublish);
    ASSERT_TRUE(field->mCheckModified);
    ASSERT_TRUE(field->mName);

    const char* name;
    mamaFieldCacheField_getName(field, &name);
    ASSERT_STREQ("my new field", name);

    ret = mamaFieldCacheField_destroy(field);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
}

TEST_F(MamaFieldCacheFieldTestC, createWithEmptyName)
{
    mamaFieldCacheField field = NULL;
    mama_status ret = mamaFieldCacheField_create(&field, 101, MAMA_FIELD_TYPE_I32, "");
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_EQ(101, field->mFid);
    ASSERT_EQ(MAMA_FIELD_TYPE_I32, field->mType);

    ASSERT_FALSE(field->mIsModified);
    ASSERT_TRUE(field->mPublish);
    ASSERT_TRUE(field->mCheckModified);
    ASSERT_TRUE(field->mName);

    const char* name;
    mamaFieldCacheField_getName(field, &name);
    ASSERT_STREQ("", name);

    ret = mamaFieldCacheField_destroy(field);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
}

TEST_F(MamaFieldCacheFieldTestC, setGetFlags)
{
    mama_bool_t modified;
    mama_bool_t publish;
    mama_bool_t checkModified;
    mamaFieldCacheField field = NULL;
    mama_status ret = mamaFieldCacheField_create(&field, 1, MAMA_FIELD_TYPE_F32, NULL);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_EQ(1, field->mFid);
    ASSERT_EQ(MAMA_FIELD_TYPE_F32, field->mType);
    
    ASSERT_FALSE(field->mIsModified);
    ASSERT_TRUE(field->mPublish);

    mamaFieldCacheField_isModified(field, &modified);
    ASSERT_FALSE(modified);
    mamaFieldCacheField_getPublish(field, &publish);
    ASSERT_TRUE(publish);
    mamaFieldCacheField_getCheckModified(field, &checkModified);
    ASSERT_TRUE(checkModified);
    
    mamaFieldCacheField_setPublish(field, 0);
    mamaFieldCacheField_getPublish(field, &publish);
    ASSERT_FALSE(publish);
    
    mamaFieldCacheField_setCheckModified(field, 0);
    mamaFieldCacheField_getCheckModified(field, &checkModified);
    ASSERT_FALSE(checkModified);
    
    ret = mamaFieldCacheField_destroy(field);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
}

TEST_F(MamaFieldCacheFieldTestC, setDataValueEmpty)
{
    CREATE_FIELD(1, MAMA_FIELD_TYPE_BOOL)
    
    ret = mamaFieldCacheField_setDataValue(field, NULL, 4);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_FALSE(field->mData.data);
            
    mama_i8_t data = -61;
    ret = mamaFieldCacheField_setDataValue(field, (void*)&data, 0);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_FALSE(field->mData.data);
    
    DESTROY_FIELD
}

TEST_F(MamaFieldCacheFieldTestC, setDataValueI8)
{
    mama_i8_t data = 9;
    CREATE_FIELD(1, MAMA_FIELD_TYPE_I8)
    
    ret = mamaFieldCacheField_setDataValue(field, (void*)&data, sizeof(mama_i8_t));
    ASSERT_EQ(9, *(mama_i8_t*)field->mData.data);
            
    data = -61;
    ret = mamaFieldCacheField_setDataValue(field, (void*)&data, sizeof(mama_i8_t));
    ASSERT_EQ(-61, *(mama_i8_t*)field->mData.data);
    
    DESTROY_FIELD
}

TEST_F(MamaFieldCacheFieldTestC, setDataValueBool)
{
    mama_bool_t data = 1;
    CREATE_FIELD(1, MAMA_FIELD_TYPE_BOOL)

    ret = mamaFieldCacheField_setDataValue(field, (void*)&data, sizeof(mama_bool_t));
    ASSERT_TRUE(*(mama_bool_t*)field->mData.data);

    data = 0;
    ret = mamaFieldCacheField_setDataValue(field, (void*)&data, sizeof(mama_bool_t));
    ASSERT_FALSE(*(mama_bool_t*)field->mData.data);

    DESTROY_FIELD
}

TEST_F(MamaFieldCacheFieldTestC, setDataPointerEmpty)
{
    void* old = NULL;

    CREATE_FIELD(1, MAMA_FIELD_TYPE_STRING)
    
    ret = mamaFieldCacheField_setDataPointer(field, NULL, &old);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_FALSE(field->mData.str);
    ASSERT_EQ(0, field->mDataSize);
    ASSERT_FALSE(old);
    ASSERT_FALSE(field->mIsModified);

    DESTROY_FIELD
}

TEST_F(MamaFieldCacheFieldTestC, setDataPointerString)
{
    char data1[] = "hello";
    void* old = NULL;

    CREATE_FIELD(1, MAMA_FIELD_TYPE_STRING)

    ret = mamaFieldCacheField_setDataPointer(field, data1, &old);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_TRUE(field->mData.str);
    ASSERT_EQ(0, field->mDataSize);
    ASSERT_FALSE(old);

    char data2[] = "another hello";
    ret = mamaFieldCacheField_setDataPointer(field, data2, &old);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_TRUE(field->mData.str);
    ASSERT_EQ(0, field->mDataSize);
    ASSERT_TRUE(old);    

    DESTROY_FIELD
}

TEST_F(MamaFieldCacheFieldTestC, testSetGetBool)
{
    mama_i8_t retValue;
    mama_bool_t data = 1;

    CREATE_FIELD(1, MAMA_FIELD_TYPE_BOOL)
    
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setBool(field, data));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getBool(field, &retValue));
    ASSERT_EQ(data, retValue);

    data = 0;
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setBool(field, data));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getBool(field, &retValue));        
    ASSERT_EQ(data, retValue);

    DESTROY_FIELD
}

TEST_F(MamaFieldCacheFieldTestC, testSetGetChar)
{
    char retValue;
    char data = '1';

    CREATE_FIELD(1, MAMA_FIELD_TYPE_CHAR)
    
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setChar(field, data));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getChar(field, &retValue));        
    ASSERT_EQ(data, retValue);

    data = 'G';
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setChar(field, data));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getChar(field, &retValue));        
    ASSERT_EQ(data, retValue);

    DESTROY_FIELD
}

TEST_F(MamaFieldCacheFieldTestC, testSetGetI8)
{
    mama_i8_t retValue;
    mama_i8_t data = 11;

    CREATE_FIELD(1, MAMA_FIELD_TYPE_I8)

    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setI8(field, data));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getI8(field, &retValue));
    ASSERT_EQ(data, retValue);
    
    data = -22;
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setI8(field, data));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getI8(field, &retValue));        
    ASSERT_EQ(data, retValue);
    
    DESTROY_FIELD
}

TEST_F(MamaFieldCacheFieldTestC, testSetGetU8)
{
    mama_u8_t retValue;
    mama_u8_t data = 11;

    CREATE_FIELD(1, MAMA_FIELD_TYPE_U8)
    
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setU8(field, data));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getU8(field, &retValue));
    ASSERT_EQ(data, retValue);
    
    data = 3;
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setU8(field, data));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getU8(field, &retValue));        
    ASSERT_EQ(data, retValue);
    
    DESTROY_FIELD
}

TEST_F(MamaFieldCacheFieldTestC, testSetGetI16)
{
    mama_i16_t retValue;
    mama_i16_t data = 11;

    CREATE_FIELD(1, MAMA_FIELD_TYPE_I16)

    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setI16(field, data));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getI16(field, &retValue));
    ASSERT_EQ(data, retValue);
    
    data = -22;
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setI16(field, data));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getI16(field, &retValue));        
    ASSERT_EQ(data, retValue);
    
    DESTROY_FIELD
}

TEST_F(MamaFieldCacheFieldTestC, testSetGetU16)
{
    mama_u16_t retValue;
    mama_u16_t data = 11;

    CREATE_FIELD(1, MAMA_FIELD_TYPE_U16)
    
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setU16(field, data));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getU16(field, &retValue));
    ASSERT_EQ(data, retValue);
    
    data = 3;
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setU16(field, data));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getU16(field, &retValue));        
    ASSERT_EQ(data, retValue);
    
    DESTROY_FIELD
}

TEST_F(MamaFieldCacheFieldTestC, testSetGetI32)
{
    mama_i32_t retValue;
    mama_i32_t data = 11;

    CREATE_FIELD(1, MAMA_FIELD_TYPE_I32)

    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setI32(field, data));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getI32(field, &retValue));
    ASSERT_EQ(data, retValue);
    
    data = -22;
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setI32(field, data));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getI32(field, &retValue));        
    ASSERT_EQ(data, retValue);
    
    DESTROY_FIELD
}

TEST_F(MamaFieldCacheFieldTestC, testSetGetU32)
{
    mama_u32_t retValue;
    mama_u32_t data = 11;

    CREATE_FIELD(1, MAMA_FIELD_TYPE_U32)
    
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setU32(field, data));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getU32(field, &retValue));
    ASSERT_EQ(data, retValue);
    
    data = 3;
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setU32(field, data));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getU32(field, &retValue));        
    ASSERT_EQ(data, retValue);
    
    DESTROY_FIELD
}

TEST_F(MamaFieldCacheFieldTestC, testSetGetI64)
{
    mama_i64_t retValue;
    mama_i64_t data = 11;

    CREATE_FIELD(1, MAMA_FIELD_TYPE_I64)

    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setI64(field, data));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getI64(field, &retValue));
    ASSERT_EQ(data, retValue);
    
    data = -22;
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setI64(field, data));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getI64(field, &retValue));        
    ASSERT_EQ(data, retValue);
    
    DESTROY_FIELD
}

TEST_F(MamaFieldCacheFieldTestC, testSetGetU64)
{
    mama_u64_t retValue;
    mama_u64_t data = 11;

    CREATE_FIELD(1, MAMA_FIELD_TYPE_U64)
    
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setU64(field, data));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getU64(field, &retValue));
    ASSERT_EQ(data, retValue);
    
    data = 3;
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setU64(field, data));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getU64(field, &retValue));        
    ASSERT_EQ(data, retValue);
    
    DESTROY_FIELD
}

TEST_F(MamaFieldCacheFieldTestC, testSetGetF32)
{
    mama_f32_t retValue;
    mama_f32_t data = 11.3;

    CREATE_FIELD(1, MAMA_FIELD_TYPE_F32)
    
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setF32(field, data));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getF32(field, &retValue));
    ASSERT_DOUBLE_EQ(data, retValue);
    
    data = -3.01;
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setF32(field, data));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getF32(field, &retValue));        
    ASSERT_DOUBLE_EQ(data, retValue);
    
    DESTROY_FIELD
}

TEST_F(MamaFieldCacheFieldTestC, testSetGetF64)
{
    mama_f64_t retValue;
    mama_f64_t data = 11.3;

    CREATE_FIELD(1, MAMA_FIELD_TYPE_F64)
    
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setF64(field, data));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getF64(field, &retValue));
    ASSERT_DOUBLE_EQ(data, retValue);
    
    data = -3.01;
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setF64(field, data));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getF64(field, &retValue));        
    ASSERT_DOUBLE_EQ(data, retValue);
    
    DESTROY_FIELD
}

TEST_F(MamaFieldCacheFieldTestC, testSetGetString)
{
    const char* retValue = NULL;
    mama_size_t len;
    const char* data = "hello";

    CREATE_FIELD(1, MAMA_FIELD_TYPE_STRING)
    
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setString(field, data, 0));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getString(field, &retValue, &len));
    ASSERT_STREQ(data, retValue);
    ASSERT_EQ(strlen(retValue), len);
    
    data = "another string";
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setString(field, data, 0));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getString(field, &retValue, &len));
    ASSERT_STREQ(data, retValue);
    ASSERT_EQ(strlen(retValue), len);
    
    DESTROY_FIELD
}

TEST_F(MamaFieldCacheFieldTestC, testSetGetStringWithLen)
{
    const char* retValue = NULL;
    mama_size_t len;
    const char* data = "hello";

    CREATE_FIELD(1, MAMA_FIELD_TYPE_STRING)
    
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setString(field, data, 5));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getString(field, &retValue, &len));
    ASSERT_STREQ("hello", retValue);
    ASSERT_EQ(strlen(retValue), len);
    
    data = "another string";
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setString(field, data, strlen(data)));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getString(field, &retValue, &len));
    ASSERT_STREQ(data, retValue);
    ASSERT_EQ(strlen(retValue), len);
    
    DESTROY_FIELD
}

TEST_F(MamaFieldCacheFieldTestC, testSetNullString)
{
    const char* retValue = NULL;
    mama_size_t len;
    const char* data = NULL;

    CREATE_FIELD(1, MAMA_FIELD_TYPE_STRING)
    
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setString(field, data, 0));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getString(field, &retValue, &len));
    ASSERT_FALSE(retValue);
    ASSERT_EQ(0, len);
    
    data = "another string";
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setString(field, data, 0));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getString(field, &retValue, &len));        
    ASSERT_STREQ(data, retValue);
    
    data = NULL;
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setString(field, data, 0));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getString(field, &retValue, &len));
    ASSERT_FALSE(retValue);
    ASSERT_EQ(0, len);
    
    DESTROY_FIELD
}

TEST_F(MamaFieldCacheFieldTestC, testSetGetPrice)
{
    mamaPrice retValue = NULL;
    double val = 0.0;
    mamaPrice data = NULL;

    CREATE_FIELD(1, MAMA_FIELD_TYPE_PRICE)
    
    mamaPrice_create(&data);
    mamaPrice_setValue(data, 3.52);
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setPrice(field, data));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getPrice(field, &retValue));
    mamaPrice_getValue(retValue, &val);
    ASSERT_DOUBLE_EQ(3.52, val);
    
    mamaPrice_setValue(data, 8.0);
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setPrice(field, data));
    mamaPrice_destroy(data);

    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getPrice(field, &retValue));        
    mamaPrice_getValue(retValue, &val);
    ASSERT_DOUBLE_EQ(8.0, val);
    
    DESTROY_FIELD
}

TEST_F(MamaFieldCacheFieldTestC, testSetGetDateTime)
{
    mamaDateTime retValue = NULL;
    mama_f64_t val = 0.0;
    mamaDateTime data = NULL;

    CREATE_FIELD(1, MAMA_FIELD_TYPE_TIME)

    mamaDateTime_create(&data);
    mamaDateTime_setEpochTimeF64(data, 123456);
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setDateTime(field, data));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getDateTime(field, &retValue));
    mamaDateTime_getEpochTimeSeconds(retValue, &val);
    ASSERT_DOUBLE_EQ(123456, val);
    
    mamaDateTime_setEpochTimeF64(data, 222222);
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setDateTime(field, data));
    mamaDateTime_destroy(data);

    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getDateTime(field, &retValue));        
    mamaDateTime_getEpochTimeSeconds(retValue, &val);
    ASSERT_DOUBLE_EQ(222222, val);
    
    DESTROY_FIELD
}

TEST_F(MamaFieldCacheFieldTestC, testSetGetI8Vector)
{
    const mama_i8_t* retValue = NULL;
    mama_size_t retSize;
    mama_i8_t dataVector[3] = { 1, 2, 3 };

    CREATE_FIELD(1, MAMA_FIELD_TYPE_VECTOR_I8)

    // Adding only 2 fields
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setI8Vector(field, dataVector, 2));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getI8Vector(field, &retValue, &retSize));
    ASSERT_EQ(2, retSize);
    ASSERT_TRUE(retValue);
    ASSERT_EQ(1, retValue[0]);
    ASSERT_EQ(2, retValue[1]);

    dataVector[0] = 10;
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setI8Vector(field, dataVector, 3));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getI8Vector(field, &retValue, &retSize));
    
    ASSERT_EQ(3, retSize);
    ASSERT_TRUE(retValue);
    ASSERT_EQ(10, retValue[0]);
    ASSERT_EQ(2, retValue[1]);
    ASSERT_EQ(3, retValue[2]);

    DESTROY_FIELD
}

TEST_F(MamaFieldCacheFieldTestC, testSetGetU8Vector)
{
    const mama_u8_t* retValue = NULL;
    mama_size_t retSize;
    mama_u8_t dataVector[3] = { 1, 2, 3 };

    CREATE_FIELD(1, MAMA_FIELD_TYPE_VECTOR_U8)

    // Adding only 2 fields
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setU8Vector(field, dataVector, 2));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getU8Vector(field, &retValue, &retSize));
    ASSERT_EQ(2, retSize);
    ASSERT_TRUE(retValue);
    ASSERT_EQ(1, retValue[0]);
    ASSERT_EQ(2, retValue[1]);

    dataVector[0] = 10;
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setU8Vector(field, dataVector, 3));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getU8Vector(field, &retValue, &retSize));
    
    ASSERT_EQ(3, retSize);
    ASSERT_TRUE(retValue);
    ASSERT_EQ(10, retValue[0]);
    ASSERT_EQ(2, retValue[1]);
    ASSERT_EQ(3, retValue[2]);

    DESTROY_FIELD
}

TEST_F(MamaFieldCacheFieldTestC, testSetGetI16Vector)
{
    const mama_i16_t* retValue = NULL;
    mama_size_t retSize;
    mama_i16_t dataVector[3] = { 1, 2, 3 };

    CREATE_FIELD(1, MAMA_FIELD_TYPE_VECTOR_I16)

    // Adding only 2 fields
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setI16Vector(field, dataVector, 2));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getI16Vector(field, &retValue, &retSize));
    ASSERT_EQ(2, retSize);
    ASSERT_TRUE(retValue);
    ASSERT_EQ(1, retValue[0]);
    ASSERT_EQ(2, retValue[1]);

    dataVector[0] = 10;
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setI16Vector(field, dataVector, 3));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getI16Vector(field, &retValue, &retSize));
    
    ASSERT_EQ(3, retSize);
    ASSERT_TRUE(retValue);
    ASSERT_EQ(10, retValue[0]);
    ASSERT_EQ(2, retValue[1]);
    ASSERT_EQ(3, retValue[2]);

    DESTROY_FIELD
}

TEST_F(MamaFieldCacheFieldTestC, testSetGetU16Vector)
{
    CREATE_FIELD(1, MAMA_FIELD_TYPE_VECTOR_U16)

    const mama_u16_t* retValue = NULL;
    mama_size_t retSize;
    
    mama_u16_t dataVector[3] = { 1, 2, 3 };

    // Adding only 2 fields
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setU16Vector(field, dataVector, 2));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getU16Vector(field, &retValue, &retSize));
    ASSERT_EQ(2, retSize);
    ASSERT_TRUE(retValue);
    ASSERT_EQ(1, retValue[0]);
    ASSERT_EQ(2, retValue[1]);

    dataVector[0] = 10;
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setU16Vector(field, dataVector, 3));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getU16Vector(field, &retValue, &retSize));
    
    ASSERT_EQ(3, retSize);
    ASSERT_TRUE(retValue);
    ASSERT_EQ(10, retValue[0]);
    ASSERT_EQ(2, retValue[1]);
    ASSERT_EQ(3, retValue[2]);

    DESTROY_FIELD
}

TEST_F(MamaFieldCacheFieldTestC, testSetGetI32Vector)
{
    const mama_i32_t* retValue = NULL;
    mama_size_t retSize;
    mama_i32_t dataVector[3] = { 1, 2, 3 };

    CREATE_FIELD(1, MAMA_FIELD_TYPE_VECTOR_I32)

    // Adding only 2 fields
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setI32Vector(field, dataVector, 2));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getI32Vector(field, &retValue, &retSize));
    ASSERT_EQ(2, retSize);
    ASSERT_TRUE(retValue);
    ASSERT_EQ(1, retValue[0]);
    ASSERT_EQ(2, retValue[1]);

    dataVector[0] = 10;
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setI32Vector(field, dataVector, 3));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getI32Vector(field, &retValue, &retSize));
    
    ASSERT_EQ(3, retSize);
    ASSERT_TRUE(retValue);
    ASSERT_EQ(10, retValue[0]);
    ASSERT_EQ(2, retValue[1]);
    ASSERT_EQ(3, retValue[2]);

    DESTROY_FIELD
}

TEST_F(MamaFieldCacheFieldTestC, testSetGetU32Vector)
{
    const mama_u32_t* retValue = NULL;
    mama_size_t retSize;
    mama_u32_t dataVector[3] = { 1, 2, 3 };

    CREATE_FIELD(1, MAMA_FIELD_TYPE_VECTOR_U32)

    // Adding only 2 fields
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setU32Vector(field, dataVector, 2));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getU32Vector(field, &retValue, &retSize));
    ASSERT_EQ(2, retSize);
    ASSERT_TRUE(retValue);
    ASSERT_EQ(1, retValue[0]);
    ASSERT_EQ(2, retValue[1]);

    dataVector[0] = 10;
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setU32Vector(field, dataVector, 3));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getU32Vector(field, &retValue, &retSize));
    
    ASSERT_EQ(3, retSize);
    ASSERT_TRUE(retValue);
    ASSERT_EQ(10, retValue[0]);
    ASSERT_EQ(2, retValue[1]);
    ASSERT_EQ(3, retValue[2]);

    DESTROY_FIELD
}

TEST_F(MamaFieldCacheFieldTestC, testSetGetI64Vector)
{
    const mama_i64_t* retValue = NULL;
    mama_size_t retSize;
    mama_i64_t dataVector[3] = { 1, 2, 3 };

    CREATE_FIELD(1, MAMA_FIELD_TYPE_VECTOR_I64)

    // Adding only 2 fields
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setI64Vector(field, dataVector, 2));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getI64Vector(field, &retValue, &retSize));
    ASSERT_EQ(2, retSize);
    ASSERT_TRUE(retValue);
    ASSERT_EQ(1, retValue[0]);
    ASSERT_EQ(2, retValue[1]);

    dataVector[0] = 10;
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setI64Vector(field, dataVector, 3));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getI64Vector(field, &retValue, &retSize));
    
    ASSERT_EQ(3, retSize);
    ASSERT_TRUE(retValue);
    ASSERT_EQ(10, retValue[0]);
    ASSERT_EQ(2, retValue[1]);
    ASSERT_EQ(3, retValue[2]);

    DESTROY_FIELD
}

TEST_F(MamaFieldCacheFieldTestC, testSetGetU64Vector)
{
    const mama_u64_t* retValue = NULL;
    mama_size_t retSize;
    mama_u64_t dataVector[3] = { 1, 2, 3 };

    CREATE_FIELD(1, MAMA_FIELD_TYPE_VECTOR_U64)

    // Adding only 2 fields
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setU64Vector(field, dataVector, 2));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getU64Vector(field, &retValue, &retSize));
    ASSERT_EQ(2, retSize);
    ASSERT_TRUE(retValue);
    ASSERT_EQ(1, retValue[0]);
    ASSERT_EQ(2, retValue[1]);

    retValue = NULL;
    dataVector[0] = 10;
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setU64Vector(field, dataVector, 3));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getU64Vector(field, &retValue, &retSize));
    
    ASSERT_EQ(3, retSize);
    ASSERT_TRUE(retValue);
    ASSERT_EQ(10, retValue[0]);
    ASSERT_EQ(2, retValue[1]);
    ASSERT_EQ(3, retValue[2]);

    DESTROY_FIELD
}

TEST_F(MamaFieldCacheFieldTestC, testSetGetF32Vector)
{
    const mama_f32_t* retValue = NULL;
    mama_size_t retSize;
    mama_f32_t dataVector[3] = { 1, 2, 3 };

    CREATE_FIELD(1, MAMA_FIELD_TYPE_VECTOR_F32)

    // Adding only 2 fields
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setF32Vector(field, dataVector, 2));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getF32Vector(field, &retValue, &retSize));
    ASSERT_EQ(2, retSize);
    ASSERT_TRUE(retValue);
    ASSERT_EQ(1, retValue[0]);
    ASSERT_EQ(2, retValue[1]);

    retValue = NULL;
    dataVector[0] = 10;
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setF32Vector(field, dataVector, 3));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getF32Vector(field, &retValue, &retSize));
    
    ASSERT_EQ(3, retSize);
    ASSERT_TRUE(retValue);
    ASSERT_EQ(10, retValue[0]);
    ASSERT_EQ(2, retValue[1]);
    ASSERT_EQ(3, retValue[2]);

    DESTROY_FIELD
}

TEST_F(MamaFieldCacheFieldTestC, testSetGetF64Vector)
{
    const mama_f64_t* retValue = NULL;
    mama_size_t retSize;
    mama_f64_t dataVector[3] = { 1, 2, 3 };

    CREATE_FIELD(1, MAMA_FIELD_TYPE_VECTOR_F64)

    // Adding only 2 fields
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setF64Vector(field, dataVector, 2));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getF64Vector(field, &retValue, &retSize));
    ASSERT_EQ(2, retSize);
    ASSERT_TRUE(retValue);
    ASSERT_EQ(1, retValue[0]);
    ASSERT_EQ(2, retValue[1]);

    retValue = NULL;
    dataVector[0] = 10;
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setF64Vector(field, dataVector, 3));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getF64Vector(field, &retValue, &retSize));
    
    ASSERT_EQ(3, retSize);
    ASSERT_TRUE(retValue);
    ASSERT_EQ(10, retValue[0]);
    ASSERT_EQ(2, retValue[1]);
    ASSERT_EQ(3, retValue[2]);

    DESTROY_FIELD
}

TEST_F(MamaFieldCacheFieldTestC, testSetGetStringVector)
{
    const char** retValue = NULL;
    mama_size_t retSize;
    const char* dataVector[3] = { "one", "two", "three" };

    CREATE_FIELD(1, MAMA_FIELD_TYPE_VECTOR_STRING)

    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getStringVector(field, &retValue, &retSize));
    ASSERT_EQ(0, retSize);
    ASSERT_FALSE(retValue);

    // Adding only 2 fields
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setStringVector(field, dataVector, 2));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getStringVector(field, &retValue, &retSize));
    ASSERT_EQ(2, retSize);
    ASSERT_TRUE(retValue);
    ASSERT_STREQ("one", retValue[0]);
    ASSERT_STREQ("two", retValue[1]);

    retValue = NULL;
    dataVector[0] = "one modified";
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setStringVector(field, dataVector, 3));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getStringVector(field, &retValue, &retSize));
    ASSERT_EQ(3, retSize);
    ASSERT_TRUE(retValue);
    ASSERT_STREQ("one modified", retValue[0]);
    ASSERT_STREQ("two", retValue[1]);
    ASSERT_STREQ("three", retValue[2]);

    DESTROY_FIELD
}

TEST_F(MamaFieldCacheFieldTestC, testSetGetPriceVector)
{
    const mamaPrice* retValue = NULL;
    mama_size_t retSize;
    double value;

    CREATE_FIELD(1, MAMA_FIELD_TYPE_VECTOR_PRICE)

    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getPriceVector(field, &retValue, &retSize));
    ASSERT_EQ(0, retSize);
    ASSERT_FALSE(retValue);
    
    mamaPrice dataVector[3] = { NULL, NULL, NULL };
    mamaPrice_create(&dataVector[0]);
    mamaPrice_setValue(dataVector[0], 0.1);
    mamaPrice_create(&dataVector[1]);
    mamaPrice_setValue(dataVector[1], 0.2);
    mamaPrice_create(&dataVector[2]);
    mamaPrice_setValue(dataVector[2], 0.3);

    retValue = NULL;
    // Adding only 2 fields
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setPriceVector(field, dataVector, 2));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getPriceVector(field, &retValue, &retSize));
    ASSERT_EQ(2, retSize);
    ASSERT_TRUE(retValue);
    mamaPrice_getValue(retValue[0], &value);
    ASSERT_DOUBLE_EQ(0.1, value);
    mamaPrice_getValue(retValue[1], &value);
    ASSERT_DOUBLE_EQ(0.2, value);
    
    retValue = NULL;
    mamaPrice_setValue(dataVector[0], 1);
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setPriceVector(field, dataVector, 3));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getPriceVector(field, &retValue, &retSize));
    ASSERT_EQ(3, retSize);
    ASSERT_TRUE(retValue);
    mamaPrice_getValue(retValue[0], &value);
    ASSERT_DOUBLE_EQ(1, value);
    mamaPrice_getValue(retValue[1], &value);
    ASSERT_DOUBLE_EQ(0.2, value);
    mamaPrice_getValue(retValue[2], &value);
    ASSERT_DOUBLE_EQ(0.3, value);
    
    mamaPrice_destroy(dataVector[0]);
    mamaPrice_destroy(dataVector[1]);
    mamaPrice_destroy(dataVector[2]);

    DESTROY_FIELD
}

TEST_F(MamaFieldCacheFieldTestC, testSetGetDateTimeVector)
{
    const mamaDateTime* retValue = NULL;
    mama_size_t retSize;
    mama_f64_t value;

    CREATE_FIELD(1, MAMA_FIELD_TYPE_VECTOR_TIME)

    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getDateTimeVector(field, &retValue, &retSize));
    ASSERT_EQ(0, retSize);
    ASSERT_FALSE(retValue);
    
    mamaDateTime dataVector[3] = { NULL, NULL, NULL };
    mamaDateTime_create(&dataVector[0]);
    mamaDateTime_setEpochTimeF64(dataVector[0], 1000);
    mamaDateTime_create(&dataVector[1]);
    mamaDateTime_setEpochTimeF64(dataVector[1], 2000);
    mamaDateTime_create(&dataVector[2]);
    mamaDateTime_setEpochTimeF64(dataVector[2], 3000);

    // Adding only 2 fields
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setDateTimeVector(field, dataVector, 2));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getDateTimeVector(field, &retValue, &retSize));
    ASSERT_EQ(2, retSize);
    ASSERT_TRUE(retValue);
    mamaDateTime_getEpochTimeSeconds(dataVector[0], &value);
    ASSERT_DOUBLE_EQ(1000, value);
    mamaDateTime_getEpochTimeSeconds(dataVector[1], &value);
    ASSERT_DOUBLE_EQ(2000, value);
    
    retValue = NULL;
    mamaDateTime_setEpochTimeF64(dataVector[0], 5000);
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_setDateTimeVector(field, dataVector, 3));
    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCacheField_getDateTimeVector(field, &retValue, &retSize));
    ASSERT_EQ(3, retSize);
    ASSERT_TRUE(retValue);
    mamaDateTime_getEpochTimeSeconds(dataVector[0], &value);
    ASSERT_DOUBLE_EQ(5000, value);
    mamaDateTime_getEpochTimeSeconds(dataVector[1], &value);
    ASSERT_DOUBLE_EQ(2000, value);
    mamaDateTime_getEpochTimeSeconds(dataVector[2], &value);
    ASSERT_DOUBLE_EQ(3000, value);

    mamaDateTime_destroy(dataVector[0]);
    mamaDateTime_destroy(dataVector[1]);
    mamaDateTime_destroy(dataVector[2]);
    
    DESTROY_FIELD
}

TEST_F(MamaFieldCacheFieldTestC, copyCommonFields)
{
    mama_status ret;
    mama_fid_t fid = 1;
    mamaFieldType type = MAMA_FIELD_TYPE_BOOL;
    mamaFieldCacheField field1 = NULL;
    ret = mamaFieldCacheField_create(&field1, fid, type, "field1");
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    mamaFieldCacheField field2 = NULL;
    ret = mamaFieldCacheField_create(&field2, fid, type, NULL);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    
    mamaFieldCacheField_setPublish(field1, 1);
    mamaFieldCacheField_setCheckModified(field1, 1);
    
    mamaFieldCacheField_setPublish(field2, 0);
    mamaFieldCacheField_setCheckModified(field2, 0);
                                // SOURCE, DESTINATION
    ret = mamaFieldCacheField_copy(field1, field2);
    ASSERT_EQ(MAMA_STATUS_OK, ret);

    mama_bool_t value;
    mamaFieldCacheField_isModified(field2, &value);
    ASSERT_FALSE(value);
    mamaFieldCacheField_getPublish(field2, &value);
    ASSERT_TRUE(value);
    mamaFieldCacheField_getCheckModified(field2, &value);
    ASSERT_TRUE(value);
    const char* name;
    mamaFieldCacheField_getName(field2, &name);
    ASSERT_STREQ("field1", name);
    
    ret = mamaFieldCacheField_destroy(field1);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ret = mamaFieldCacheField_destroy(field2);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
}

TEST_F(MamaFieldCacheFieldTestC, copyCommonWithEmptyName)
{
    mama_status ret;
    mama_fid_t fid = 1;
    mamaFieldType type = MAMA_FIELD_TYPE_BOOL;
    mamaFieldCacheField field1 = NULL;
    ret = mamaFieldCacheField_create(&field1, fid, type, "");
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    mamaFieldCacheField field2 = NULL;
    ret = mamaFieldCacheField_create(&field2, fid, type, "");
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    
    ret = mamaFieldCacheField_copy(field1, field2);
    ASSERT_EQ(MAMA_STATUS_OK, ret);

    const char* name;
    mamaFieldCacheField_getName(field2, &name);
    ASSERT_STREQ("", name);
    
    ret = mamaFieldCacheField_destroy(field1);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ret = mamaFieldCacheField_destroy(field2);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
}

TEST_F(MamaFieldCacheFieldTestC, copySelf)
{
    mama_status ret;
    mama_fid_t fid = 1;
    mamaFieldType type = MAMA_FIELD_TYPE_BOOL;
    mamaFieldCacheField field1 = NULL;
    ret = mamaFieldCacheField_create(&field1, fid, type, "");
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    
    ret = mamaFieldCacheField_copy(field1, field1);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    
    ret = mamaFieldCacheField_destroy(field1);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
}

TEST_F(MamaFieldCacheFieldTestC, copyBool)
{
    mama_status ret;
    mama_fid_t fid = 1;
    mamaFieldType type = MAMA_FIELD_TYPE_BOOL;
    mamaFieldCacheField field1 = NULL;
    ret = mamaFieldCacheField_create(&field1, fid, type, NULL);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    mamaFieldCacheField field2 = NULL;
    ret = mamaFieldCacheField_create(&field2, fid, type, NULL);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    
    mamaFieldCacheField_setBool(field1, 1);
    mamaFieldCacheField_setBool(field2, 0);
    
    mama_bool_t value1;
    mama_bool_t value2;
    mamaFieldCacheField_getBool(field1, &value1);
    mamaFieldCacheField_getBool(field2, &value2);
    ASSERT_TRUE(value1);
    ASSERT_FALSE(value2);

    ret = mamaFieldCacheField_copy(field1, field2);
    ASSERT_EQ(MAMA_STATUS_OK, ret);

    mamaFieldCacheField_getBool(field1, &value1);
    mamaFieldCacheField_getBool(field2, &value2);
    ASSERT_EQ(value1, value2);
    
    ret = mamaFieldCacheField_destroy(field1);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ret = mamaFieldCacheField_destroy(field2);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
}

TEST_F(MamaFieldCacheFieldTestC, copyChar)
{
    mama_status ret;
    mama_fid_t fid = 1;
    mamaFieldType type = MAMA_FIELD_TYPE_CHAR;
    mamaFieldCacheField field1 = NULL;
    ret = mamaFieldCacheField_create(&field1, fid, type, NULL);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    mamaFieldCacheField field2 = NULL;
    ret = mamaFieldCacheField_create(&field2, fid, type, NULL);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    
    mamaFieldCacheField_setChar(field1, '1');
    mamaFieldCacheField_setChar(field2, '2');
    
    char value1;
    char value2;
    mamaFieldCacheField_getChar(field1, &value1);
    mamaFieldCacheField_getChar(field2, &value2);
    ASSERT_EQ('1', value1);
    ASSERT_EQ('2', value2);

    ret = mamaFieldCacheField_copy(field1, field2);
    ASSERT_EQ(MAMA_STATUS_OK, ret);

    mamaFieldCacheField_getChar(field1, &value1);
    mamaFieldCacheField_getChar(field2, &value2);
    ASSERT_EQ(value1, value2);
    
    ret = mamaFieldCacheField_destroy(field1);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ret = mamaFieldCacheField_destroy(field2);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
}

TEST_F(MamaFieldCacheFieldTestC, copyI8)
{
    mama_status ret;
    mama_fid_t fid = 1;
    mamaFieldType type = MAMA_FIELD_TYPE_I8;
    mamaFieldCacheField field1 = NULL;
    ret = mamaFieldCacheField_create(&field1, fid, type, NULL);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    mamaFieldCacheField field2 = NULL;
    ret = mamaFieldCacheField_create(&field2, fid, type, NULL);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    
    mamaFieldCacheField_setI8(field1, 1);
    mamaFieldCacheField_setI8(field2, 2);
    
    mama_i8_t value1;
    mama_i8_t value2;
    mamaFieldCacheField_getI8(field1, &value1);
    mamaFieldCacheField_getI8(field2, &value2);
    ASSERT_EQ(1, value1);
    ASSERT_EQ(2, value2);

    ret = mamaFieldCacheField_copy(field1, field2);
    ASSERT_EQ(MAMA_STATUS_OK, ret);

    mamaFieldCacheField_getI8(field1, &value1);
    mamaFieldCacheField_getI8(field2, &value2);
    ASSERT_EQ(value1, value2);    
    ASSERT_EQ(1, value2);

    ret = mamaFieldCacheField_destroy(field1);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ret = mamaFieldCacheField_destroy(field2);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
}

TEST_F(MamaFieldCacheFieldTestC, copyU8)
{
    mama_status ret;
    mama_fid_t fid = 1;
    mamaFieldType type = MAMA_FIELD_TYPE_U8;
    mamaFieldCacheField field1 = NULL;
    ret = mamaFieldCacheField_create(&field1, fid, type, NULL);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    mamaFieldCacheField field2 = NULL;
    ret = mamaFieldCacheField_create(&field2, fid, type, NULL);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    
    mamaFieldCacheField_setU8(field1, 1);
    mamaFieldCacheField_setU8(field2, 2);
    
    mama_u8_t value1;
    mama_u8_t value2;
    mamaFieldCacheField_getU8(field1, &value1);
    mamaFieldCacheField_getU8(field2, &value2);
    ASSERT_EQ(1, value1);
    ASSERT_EQ(2, value2);

    ret = mamaFieldCacheField_copy(field1, field2);
    ASSERT_EQ(MAMA_STATUS_OK, ret);

    mamaFieldCacheField_getU8(field1, &value1);
    mamaFieldCacheField_getU8(field2, &value2);
    ASSERT_EQ(value1, value2);    
    ASSERT_EQ(1, value2);

    ret = mamaFieldCacheField_destroy(field1);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ret = mamaFieldCacheField_destroy(field2);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
}

TEST_F(MamaFieldCacheFieldTestC, copyI16)
{
    mama_status ret;
    mama_fid_t fid = 1;
    mamaFieldType type = MAMA_FIELD_TYPE_I16;
    mamaFieldCacheField field1 = NULL;
    ret = mamaFieldCacheField_create(&field1, fid, type, NULL);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    mamaFieldCacheField field2 = NULL;
    ret = mamaFieldCacheField_create(&field2, fid, type, NULL);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    
    mamaFieldCacheField_setI16(field1, 1);
    mamaFieldCacheField_setI16(field2, 2);
    
    mama_i16_t value1;
    mama_i16_t value2;
    mamaFieldCacheField_getI16(field1, &value1);
    mamaFieldCacheField_getI16(field2, &value2);
    ASSERT_EQ(1, value1);
    ASSERT_EQ(2, value2);

    ret = mamaFieldCacheField_copy(field1, field2);
    ASSERT_EQ(MAMA_STATUS_OK, ret);

    mamaFieldCacheField_getI16(field1, &value1);
    mamaFieldCacheField_getI16(field2, &value2);
    ASSERT_EQ(value1, value2);    
    ASSERT_EQ(1, value2);

    ret = mamaFieldCacheField_destroy(field1);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ret = mamaFieldCacheField_destroy(field2);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
}

TEST_F(MamaFieldCacheFieldTestC, copyU16)
{
    mama_status ret;
    mama_fid_t fid = 1;
    mamaFieldType type = MAMA_FIELD_TYPE_U16;
    mamaFieldCacheField field1 = NULL;
    ret = mamaFieldCacheField_create(&field1, fid, type, NULL);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    mamaFieldCacheField field2 = NULL;
    ret = mamaFieldCacheField_create(&field2, fid, type, NULL);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    
    mamaFieldCacheField_setU16(field1, 1);
    mamaFieldCacheField_setU16(field2, 2);
    
    mama_u16_t value1;
    mama_u16_t value2;
    mamaFieldCacheField_getU16(field1, &value1);
    mamaFieldCacheField_getU16(field2, &value2);
    ASSERT_EQ(1, value1);
    ASSERT_EQ(2, value2);

    ret = mamaFieldCacheField_copy(field1, field2);
    ASSERT_EQ(MAMA_STATUS_OK, ret);

    mamaFieldCacheField_getU16(field1, &value1);
    mamaFieldCacheField_getU16(field2, &value2);
    ASSERT_EQ(value1, value2);    
    ASSERT_EQ(1, value2);

    ret = mamaFieldCacheField_destroy(field1);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ret = mamaFieldCacheField_destroy(field2);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
}

TEST_F(MamaFieldCacheFieldTestC, copyI32)
{
    mama_status ret;
    mama_fid_t fid = 1;
    mamaFieldType type = MAMA_FIELD_TYPE_I32;
    mamaFieldCacheField field1 = NULL;
    ret = mamaFieldCacheField_create(&field1, fid, type, NULL);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    mamaFieldCacheField field2 = NULL;
    ret = mamaFieldCacheField_create(&field2, fid, type, NULL);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    
    mamaFieldCacheField_setI32(field1, 1);
    mamaFieldCacheField_setI32(field2, 2);
    
    mama_i32_t value1;
    mama_i32_t value2;
    mamaFieldCacheField_getI32(field1, &value1);
    mamaFieldCacheField_getI32(field2, &value2);
    ASSERT_EQ(1, value1);
    ASSERT_EQ(2, value2);

    ret = mamaFieldCacheField_copy(field1, field2);
    ASSERT_EQ(MAMA_STATUS_OK, ret);

    mamaFieldCacheField_getI32(field1, &value1);
    mamaFieldCacheField_getI32(field2, &value2);
    ASSERT_EQ(value1, value2);    
    ASSERT_EQ(1, value2);

    ret = mamaFieldCacheField_destroy(field1);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ret = mamaFieldCacheField_destroy(field2);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
}

TEST_F(MamaFieldCacheFieldTestC, copyU32)
{
    mama_status ret;
    mama_fid_t fid = 1;
    mamaFieldType type = MAMA_FIELD_TYPE_U32;
    mamaFieldCacheField field1 = NULL;
    ret = mamaFieldCacheField_create(&field1, fid, type, NULL);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    mamaFieldCacheField field2 = NULL;
    ret = mamaFieldCacheField_create(&field2, fid, type, NULL);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    
    mamaFieldCacheField_setU32(field1, 1);
    mamaFieldCacheField_setU32(field2, 2);
    
    mama_u32_t value1;
    mama_u32_t value2;
    mamaFieldCacheField_getU32(field1, &value1);
    mamaFieldCacheField_getU32(field2, &value2);
    ASSERT_EQ(1, value1);
    ASSERT_EQ(2, value2);

    ret = mamaFieldCacheField_copy(field1, field2);
    ASSERT_EQ(MAMA_STATUS_OK, ret);

    mamaFieldCacheField_getU32(field1, &value1);
    mamaFieldCacheField_getU32(field2, &value2);
    ASSERT_EQ(value1, value2);    
    ASSERT_EQ(1, value2);

    ret = mamaFieldCacheField_destroy(field1);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ret = mamaFieldCacheField_destroy(field2);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
}

TEST_F(MamaFieldCacheFieldTestC, copyI64)
{
    mama_status ret;
    mama_fid_t fid = 1;
    mamaFieldType type = MAMA_FIELD_TYPE_I64;
    mamaFieldCacheField field1 = NULL;
    ret = mamaFieldCacheField_create(&field1, fid, type, NULL);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    mamaFieldCacheField field2 = NULL;
    ret = mamaFieldCacheField_create(&field2, fid, type, NULL);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    
    mamaFieldCacheField_setI64(field1, 1);
    mamaFieldCacheField_setI64(field2, 2);
    
    mama_i64_t value1;
    mama_i64_t value2;
    mamaFieldCacheField_getI64(field1, &value1);
    mamaFieldCacheField_getI64(field2, &value2);
    ASSERT_EQ(1, value1);
    ASSERT_EQ(2, value2);

    ret = mamaFieldCacheField_copy(field1, field2);
    ASSERT_EQ(MAMA_STATUS_OK, ret);

    mamaFieldCacheField_getI64(field1, &value1);
    mamaFieldCacheField_getI64(field2, &value2);
    ASSERT_EQ(value1, value2);    
    ASSERT_EQ(1, value2);

    ret = mamaFieldCacheField_destroy(field1);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ret = mamaFieldCacheField_destroy(field2);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
}

TEST_F(MamaFieldCacheFieldTestC, copyU64)
{
    mama_status ret;
    mama_fid_t fid = 1;
    mamaFieldType type = MAMA_FIELD_TYPE_U64;
    mamaFieldCacheField field1 = NULL;
    ret = mamaFieldCacheField_create(&field1, fid, type, NULL);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    mamaFieldCacheField field2 = NULL;
    ret = mamaFieldCacheField_create(&field2, fid, type, NULL);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    
    mamaFieldCacheField_setU64(field1, 1);
    mamaFieldCacheField_setU64(field2, 2);
    
    mama_u64_t value1;
    mama_u64_t value2;
    mamaFieldCacheField_getU64(field1, &value1);
    mamaFieldCacheField_getU64(field2, &value2);
    ASSERT_EQ(1, value1);
    ASSERT_EQ(2, value2);

    ret = mamaFieldCacheField_copy(field1, field2);
    ASSERT_EQ(MAMA_STATUS_OK, ret);

    mamaFieldCacheField_getU64(field1, &value1);
    mamaFieldCacheField_getU64(field2, &value2);
    ASSERT_EQ(value1, value2);    
    ASSERT_EQ(1, value2);

    ret = mamaFieldCacheField_destroy(field1);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ret = mamaFieldCacheField_destroy(field2);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
}


TEST_F(MamaFieldCacheFieldTestC, copyF32)
{
    mama_status ret;
    mama_fid_t fid = 1;
    mamaFieldType type = MAMA_FIELD_TYPE_F32;
    mamaFieldCacheField field1 = NULL;
    ret = mamaFieldCacheField_create(&field1, fid, type, NULL);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    mamaFieldCacheField field2 = NULL;
    ret = mamaFieldCacheField_create(&field2, fid, type, NULL);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    
    mamaFieldCacheField_setF32(field1, 1);
    mamaFieldCacheField_setF32(field2, 2);
    
    mama_f32_t value1;
    mama_f32_t value2;
    mamaFieldCacheField_getF32(field1, &value1);
    mamaFieldCacheField_getF32(field2, &value2);
    ASSERT_DOUBLE_EQ(1, value1);
    ASSERT_DOUBLE_EQ(2, value2);

    ret = mamaFieldCacheField_copy(field1, field2);
    ASSERT_EQ(MAMA_STATUS_OK, ret);

    mamaFieldCacheField_getF32(field1, &value1);
    mamaFieldCacheField_getF32(field2, &value2);
    ASSERT_DOUBLE_EQ(value1, value2);    
    ASSERT_DOUBLE_EQ(1, value2);

    ret = mamaFieldCacheField_destroy(field1);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ret = mamaFieldCacheField_destroy(field2);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
}

TEST_F(MamaFieldCacheFieldTestC, copyF64)
{
    mama_status ret;
    mama_fid_t fid = 1;
    mamaFieldType type = MAMA_FIELD_TYPE_F64;
    mamaFieldCacheField field1 = NULL;
    ret = mamaFieldCacheField_create(&field1, fid, type, NULL);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    mamaFieldCacheField field2 = NULL;
    ret = mamaFieldCacheField_create(&field2, fid, type, NULL);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    
    mamaFieldCacheField_setF64(field1, 1);
    mamaFieldCacheField_setF64(field2, 2);
    
    mama_f64_t value1;
    mama_f64_t value2;
    mamaFieldCacheField_getF64(field1, &value1);
    mamaFieldCacheField_getF64(field2, &value2);
    ASSERT_DOUBLE_EQ(1, value1);
    ASSERT_DOUBLE_EQ(2, value2);

    ret = mamaFieldCacheField_copy(field1, field2);
    ASSERT_EQ(MAMA_STATUS_OK, ret);

    mamaFieldCacheField_getF64(field1, &value1);
    mamaFieldCacheField_getF64(field2, &value2);
    ASSERT_DOUBLE_EQ(value1, value2);    
    ASSERT_DOUBLE_EQ(1, value2);

    ret = mamaFieldCacheField_destroy(field1);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ret = mamaFieldCacheField_destroy(field2);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
}

TEST_F(MamaFieldCacheFieldTestC, copyString)
{
    mama_status ret;
    mama_size_t len;
    mama_fid_t fid = 1;
    mamaFieldType type = MAMA_FIELD_TYPE_STRING;
    mamaFieldCacheField field1 = NULL;
    ret = mamaFieldCacheField_create(&field1, fid, type, NULL);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    mamaFieldCacheField field2 = NULL;
    ret = mamaFieldCacheField_create(&field2, fid, type, NULL);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    
    mamaFieldCacheField_setString(field1, "1", 0);
    mamaFieldCacheField_setString(field2, "2", 0);
    
    const char* value1;
    const char* value2;
    mamaFieldCacheField_getString(field1, &value1, &len);
    mamaFieldCacheField_getString(field2, &value2, &len);
    ASSERT_STREQ("1", value1);
    ASSERT_STREQ("2", value2);

    ret = mamaFieldCacheField_copy(field1, field2);
    ASSERT_EQ(MAMA_STATUS_OK, ret);

    mamaFieldCacheField_getString(field1, &value1, &len);
    mamaFieldCacheField_getString(field2, &value2, &len);
    ASSERT_STREQ(value1, value2);    
    ASSERT_STREQ("1", value2);

    ret = mamaFieldCacheField_destroy(field1);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ret = mamaFieldCacheField_destroy(field2);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
}

TEST_F(MamaFieldCacheFieldTestC, copyPrice)
{
    mama_status ret;
    mama_fid_t fid = 1;
    mamaFieldType type = MAMA_FIELD_TYPE_PRICE;
    mamaFieldCacheField field1 = NULL;
    ret = mamaFieldCacheField_create(&field1, fid, type, NULL);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    mamaFieldCacheField field2 = NULL;
    ret = mamaFieldCacheField_create(&field2, fid, type, NULL);
    ASSERT_EQ(MAMA_STATUS_OK, ret);

    mamaPrice data;
    mamaPrice_create(&data);

    mamaPrice_setValue(data, 1.1);
    ret = mamaFieldCacheField_setPrice(field1, data);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    mamaPrice_setValue(data, 2.1);
    ret = mamaFieldCacheField_setPrice(field2, data);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    mamaPrice_destroy(data);
   
    mamaPrice value1 = NULL;
    mamaPrice value2 = NULL;
    ret = mamaFieldCacheField_getPrice(field1, &value1);
    ASSERT_TRUE(field1);
    ASSERT_TRUE(value1);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ret = mamaFieldCacheField_getPrice(field2, &value2);
    ASSERT_TRUE(field2);
    ASSERT_TRUE(value2);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    double val1;
    ret = mamaPrice_getValue(value1, &val1);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_DOUBLE_EQ(1.1, val1);
    double val2;
    ret = mamaPrice_getValue(value2, &val2);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_DOUBLE_EQ(2.1, val2);

    ret = mamaFieldCacheField_copy(field1, field2);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    
    mamaFieldCacheField_getPrice(field1, &value1);
    mamaFieldCacheField_getPrice(field2, &value2);
    mamaPrice_getValue(value1, &val1);
    mamaPrice_getValue(value2, &val2);
    ASSERT_DOUBLE_EQ(val1, val2);
    ASSERT_DOUBLE_EQ(1.1, val2);

    ret = mamaFieldCacheField_destroy(field1);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ret = mamaFieldCacheField_destroy(field2);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
}

TEST_F(MamaFieldCacheFieldTestC, copyDateTime)
{
    mama_status ret;
    mama_fid_t fid = 1;
    mamaFieldType type = MAMA_FIELD_TYPE_TIME;
    mamaFieldCacheField field1 = NULL;
    ret = mamaFieldCacheField_create(&field1, fid, type, NULL);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    mamaFieldCacheField field2 = NULL;
    ret = mamaFieldCacheField_create(&field2, fid, type, NULL);
    ASSERT_EQ(MAMA_STATUS_OK, ret);

    mamaDateTime data;
    mamaDateTime_create(&data);

    mamaDateTime_setEpochTimeF64(data, 1000);
    ret = mamaFieldCacheField_setDateTime(field1, data);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    mamaDateTime_setEpochTimeF64(data, 2000);
    ret = mamaFieldCacheField_setDateTime(field2, data);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    mamaDateTime_destroy(data);
   
    mamaDateTime value1 = NULL;
    mamaDateTime value2 = NULL;
    ret = mamaFieldCacheField_getDateTime(field1, &value1);
    ASSERT_TRUE(field1);
    ASSERT_TRUE(value1);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ret = mamaFieldCacheField_getDateTime(field2, &value2);
    ASSERT_TRUE(field2);
    ASSERT_TRUE(value2);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    double val1;
    ret = mamaDateTime_getEpochTimeSeconds(value1, &val1);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_DOUBLE_EQ(1000, val1);
    double val2;
    ret = mamaDateTime_getEpochTimeSeconds(value2, &val2);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_DOUBLE_EQ(2000, val2);

    ret = mamaFieldCacheField_copy(field1, field2);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    
    mamaFieldCacheField_getDateTime(field1, &value1);
    mamaFieldCacheField_getDateTime(field2, &value2);
    mamaDateTime_getEpochTimeSeconds(value1, &val1);
    mamaDateTime_getEpochTimeSeconds(value2, &val2);
    ASSERT_DOUBLE_EQ(val1, val2);
    ASSERT_DOUBLE_EQ(1000, val2);

    ret = mamaFieldCacheField_destroy(field1);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ret = mamaFieldCacheField_destroy(field2);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
}

