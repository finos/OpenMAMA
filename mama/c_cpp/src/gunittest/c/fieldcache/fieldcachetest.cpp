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
#include <mama/mama.h>
#include <mama/msg.h>
#include <mama/fieldcache/fieldcache.h>
#include <mama/fieldcache/fieldcachefield.h>
#include <mama/fieldcache/fieldcacherecord.h>
#include "fieldcache/fieldcacheimpl.h"
#include "fieldcache/fieldcachefieldimpl.h"

class MamaFieldCacheTestC : public ::testing::Test
{
protected:

    /* Work around for problem in gtest where the this pointer can't be accessed
     * from a test fixture.
     */
    MamaFieldCacheTestC *m_this;

    MamaFieldCacheTestC(void);
    virtual ~MamaFieldCacheTestC(void);

    virtual void SetUp(void);
    virtual void TearDown(void);
    
    mamaBridge mMamaBridge;
};

MamaFieldCacheTestC::MamaFieldCacheTestC(void)
    : m_this (NULL)
{
    // To use mamaMsg
    mama_loadBridge (&mMamaBridge, getMiddleware());
    mama_open();
}

MamaFieldCacheTestC::~MamaFieldCacheTestC(void)
{
    mama_stop(mMamaBridge);
    mama_close();
}

void MamaFieldCacheTestC::SetUp(void)
{
    m_this = this;
}

void MamaFieldCacheTestC::TearDown(void)
{
    m_this = NULL;
}

TEST_F(MamaFieldCacheTestC, create)
{
    mamaFieldCache fieldCache = NULL;
    mama_status ret = mamaFieldCache_create(&fieldCache);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    
    mama_size_t size = 0;
    ret = mamaFieldCache_getSize(fieldCache, &size);
    ASSERT_EQ(0, size);
    
    mama_bool_t trackModified;
    mamaFieldCache_getTrackModified(fieldCache, &trackModified);
    ASSERT_TRUE(trackModified);
    
    mama_bool_t lockEnabled;
    mamaFieldCache_getUseLock(fieldCache, &lockEnabled);
    ASSERT_FALSE(lockEnabled);
    
    ret = mamaFieldCache_destroy(fieldCache);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
}

TEST_F(MamaFieldCacheTestC, createNull)
{
    mama_status ret = mamaFieldCache_create(NULL);
    ASSERT_EQ(MAMA_STATUS_NULL_ARG, ret);
}

TEST_F(MamaFieldCacheTestC, setGetTrackModified)
{
    mamaFieldCache fieldCache = NULL;
    mama_status ret = mamaFieldCache_create(&fieldCache);
    
    mama_bool_t trackModified;
    mamaFieldCache_setTrackModified(fieldCache, 1);
    mamaFieldCache_getTrackModified(fieldCache, &trackModified);
    ASSERT_TRUE(trackModified);
    
    mamaFieldCache_setTrackModified(fieldCache, 0);
    mamaFieldCache_getTrackModified(fieldCache, &trackModified);
    ASSERT_FALSE(trackModified);
    
    ret = mamaFieldCache_destroy(fieldCache);
}

TEST_F(MamaFieldCacheTestC, findOrAdd)
{
    mamaFieldCache fieldCache = NULL;
    mama_size_t size = 0;
    mama_status ret = mamaFieldCache_create(&fieldCache);

    mamaFieldCacheField field = NULL;
    mama_bool_t existing = 0;
    ret = mamaFieldCache_findOrAdd(fieldCache, 200, MAMA_FIELD_TYPE_CHAR, NULL, &field, &existing);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_TRUE(field);
    ASSERT_FALSE(existing);
    ret = mamaFieldCache_getSize(fieldCache, &size);
    ASSERT_EQ(1, size);
    
    field = NULL;
    ret = mamaFieldCache_findOrAdd(fieldCache, 200, MAMA_FIELD_TYPE_CHAR, NULL, &field, &existing);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_TRUE(field);
    ASSERT_TRUE(existing);
    ret = mamaFieldCache_getSize(fieldCache, &size);
    ASSERT_EQ(1, size);
    
    ret = mamaFieldCache_destroy(fieldCache);
    
    ret = mamaFieldCache_findOrAdd(NULL, 1, MAMA_FIELD_TYPE_CHAR, NULL, &field, &existing);
    ASSERT_EQ(MAMA_STATUS_NULL_ARG, ret);
}

TEST_F(MamaFieldCacheTestC, applyFieldNew)
{
    mama_status ret = MAMA_STATUS_OK;
    mamaFieldCache fieldCache = NULL;
    mamaFieldCacheField cachedField = NULL;
    mama_size_t size = 0;
    mama_bool_t boolValue = 0;
    mamaFieldCache_create(&fieldCache);
    
    mamaFieldCacheField field1 = NULL;
    mamaFieldCacheField_create(&field1, 2, MAMA_FIELD_TYPE_F32, NULL);
    mamaFieldCacheField_setF32(field1, 13.13);
    mamaFieldCacheField_setPublish(field1, 1);
    mamaFieldCacheField_setCheckModified(field1, 1);
    
    mamaFieldCache_getSize(fieldCache, &size);
    ASSERT_EQ(0, size);
    
    mamaFieldCache_applyField(fieldCache, field1);

    mamaFieldCache_getSize(fieldCache, &size);
    ASSERT_EQ(1, size);

    ret = mamaFieldCache_find(fieldCache, 2, NULL, &cachedField);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_TRUE(cachedField != NULL);

    mamaFieldCacheField_getPublish(cachedField, &boolValue);
    ASSERT_EQ(1, boolValue);
    boolValue = 0;
    mamaFieldCacheField_getCheckModified(cachedField, &boolValue);
    ASSERT_EQ(1, boolValue);
    boolValue = 0;
    mamaFieldCacheField_isModified(cachedField, &boolValue);
    ASSERT_EQ(1, boolValue);

    mamaFieldCacheField field2 = NULL;
    mamaFieldCacheField_create(&field2, 4, MAMA_FIELD_TYPE_I32, "name4");
    mamaFieldCacheField_setI32(field2, 321);
    mamaFieldCacheField_setPublish(field2, 0);
    mamaFieldCacheField_setCheckModified(field2, 0);

    mamaFieldCache_applyField(fieldCache, field2);

    mamaFieldCache_getSize(fieldCache, &size);
    ASSERT_EQ(2, size);

    cachedField = NULL;
    ret = mamaFieldCache_find(fieldCache, 4, "", &cachedField);
    ASSERT_TRUE(cachedField != NULL);
    ASSERT_EQ(MAMA_STATUS_OK, ret);

    boolValue = 0;
    mamaFieldCacheField_getPublish(cachedField, &boolValue);
    ASSERT_EQ(0, boolValue);
    boolValue = 0;
    mamaFieldCacheField_getCheckModified(cachedField, &boolValue);
    ASSERT_EQ(0, boolValue);
    boolValue = 0;
    mamaFieldCacheField_isModified(cachedField, &boolValue);
    ASSERT_EQ(0, boolValue);

    const char* cachedName = NULL;
    ret = mamaFieldCacheField_getName(cachedField, &cachedName);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_TRUE(cachedName != NULL);
    ASSERT_STREQ("name4", cachedName);

    mamaFieldCacheField_destroy(field1);
    mamaFieldCacheField_destroy(field2);
    mamaFieldCache_destroy(fieldCache);
}

TEST_F(MamaFieldCacheTestC, applyMsgNew)
{
    mamaFieldCache fieldCache = NULL;
    mama_size_t size = 0;
    mama_bool_t modified = 0;
    const char* name = "";
    mama_size_t len = 0;
    mama_status ret = mamaFieldCache_create(&fieldCache);
    
    mamaMsg message = NULL;
    mamaMsg_create(&message);
    mamaMsg_addBool(message, "test_bool", 10, 1);
    mamaMsg_addF64(message, "test_f64", 25, 12.3);
    mamaMsg_addI32(message, "test_i32", 66, -101);
    mamaMsg_addString(message, "test_string", 90, "hello world");
    
    ret = mamaFieldCache_applyMessage(fieldCache, message, NULL);
    
//    mamaMsg_clear(message);
    mamaMsg_destroy (message);
    
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ret = mamaFieldCache_getSize(fieldCache, &size);
    ASSERT_EQ(4, size);

    mamaFieldCacheField field = NULL;

    ret = mamaFieldCache_find(fieldCache, 10, NULL, &field);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_EQ(10, field->mFid);
    ASSERT_EQ(MAMA_FIELD_TYPE_BOOL, field->mType);
    mama_bool_t resultBool = 0;
    mamaFieldCacheField_getBool(field, &resultBool);
    ASSERT_EQ(1, resultBool);
    mamaFieldCacheField_isModified(field, &modified);
    ASSERT_TRUE(modified);
    mamaFieldCacheField_getName(field, &name);
    ASSERT_TRUE(name == NULL);

    field = NULL;
    ret = mamaFieldCache_find(fieldCache, 25, NULL, &field);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_EQ(25, field->mFid);
    mama_f64_t resultF64 = 0;
    mamaFieldCacheField_getF64(field, &resultF64);
    ASSERT_DOUBLE_EQ(12.3, resultF64);
    mamaFieldCacheField_isModified(field, &modified);
    ASSERT_TRUE(modified);

    field = NULL;
    ret = mamaFieldCache_find(fieldCache, 66, NULL, &field);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_EQ(66, field->mFid);
    mama_i32_t resultI32 = 0;
    mamaFieldCacheField_getI32(field, &resultI32);
    ASSERT_DOUBLE_EQ(-101, resultI32);
    mamaFieldCacheField_isModified(field, &modified);
    ASSERT_TRUE(modified);

    field = NULL;
    ret = mamaFieldCache_find(fieldCache, 90, NULL, &field);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_EQ(90, field->mFid);
    const char* resultString = NULL;
    mamaFieldCacheField_getString(field, &resultString, &len);
    ASSERT_STREQ("hello world", resultString);
    mamaFieldCacheField_isModified(field, &modified);
    ASSERT_TRUE(modified);

    size = 0;
    mamaFieldCacheList_getSize(fieldCache->mModifiedFields, &size);
    ASSERT_EQ(4, size);

    ret = mamaFieldCache_destroy(fieldCache);
    
    ret = mamaFieldCache_applyMessage(NULL, message, NULL);
    ASSERT_EQ(MAMA_STATUS_NULL_ARG, ret);
    ret = mamaFieldCache_applyMessage(fieldCache, NULL, NULL);
    ASSERT_EQ(MAMA_STATUS_NULL_ARG, ret);
}

TEST_F(MamaFieldCacheTestC, applyMsgUpdate)
{
    mamaFieldCache fieldCache = NULL;
    mama_bool_t modified = 0;
    mama_size_t size = 0;
    mama_size_t len = 0;
    mama_status ret = mamaFieldCache_create(&fieldCache);
    
    mamaMsg message;
    mamaMsg_create(&message);
    mamaMsg_addBool(message, "test_bool", 10, 1);
    mamaMsg_addF64(message, "test_f64", 25, 12.3);
    mamaMsg_addString(message, "test_string", 90, "hello world");
    
    mamaFieldCache_applyMessage(fieldCache, message, NULL);

    ret = mamaFieldCache_getSize(fieldCache, &size);
    ASSERT_EQ(3, size);
    
    mamaFieldCacheField field = NULL;
    ret = mamaFieldCache_find(fieldCache, 10, NULL, &field);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_EQ(10, field->mFid);
    mama_bool_t resultBool = 0;
    mamaFieldCacheField_getBool(field, &resultBool);
    ASSERT_EQ(1, resultBool);
    mamaFieldCacheField_isModified(field, &modified);
    ASSERT_TRUE(modified);

    field = NULL;
    ret = mamaFieldCache_find(fieldCache, 25, NULL, &field);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_EQ(25, field->mFid);
    mama_f64_t resultF64 = 0;
    mamaFieldCacheField_getF64(field, &resultF64);
    ASSERT_DOUBLE_EQ(12.3, resultF64);
    mamaFieldCacheField_isModified(field, &modified);
    ASSERT_TRUE(modified);

    field = NULL;
    ret = mamaFieldCache_find(fieldCache, 90, NULL, &field);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_EQ(90, field->mFid);
    const char* resultString = NULL;
    mamaFieldCacheField_getString(field, &resultString, &len);
    ASSERT_STREQ("hello world", resultString);
    mamaFieldCacheField_isModified(field, &modified);
    ASSERT_TRUE(modified);

    size = 0;
    mamaFieldCacheList_getSize(fieldCache->mModifiedFields, &size);
    ASSERT_EQ(3, size);

    mamaMsg_clear(message);
    
    mamaMsg_addF64(message, "test_f32", 25, 10.9);
    mamaMsg_addI32(message, "test_i32", 60, -123);

    mamaFieldCache_applyMessage(fieldCache, message, NULL);

    ret = mamaFieldCache_getSize(fieldCache, &size);
    ASSERT_EQ(4, size);

    mamaFieldCacheList_getSize(fieldCache->mModifiedFields, &size);
    ASSERT_EQ(4, size);

    ret = mamaFieldCache_find(fieldCache, 10, NULL, &field);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_EQ(10, field->mFid);
    resultBool = 0;
    mamaFieldCacheField_getBool(field, &resultBool);
    ASSERT_EQ(1, resultBool);
    mamaFieldCacheField_isModified(field, &modified);
    ASSERT_TRUE(modified);
    
    ret = mamaFieldCache_find(fieldCache, 60, NULL, &field);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_EQ(60, field->mFid);
    mama_i32_t resultI32 = 0;
    mamaFieldCacheField_getI32(field, &resultI32);
    ASSERT_DOUBLE_EQ(-123, resultI32);
    mamaFieldCacheField_isModified(field, &modified);
    ASSERT_TRUE(modified);
    
    ret = mamaFieldCache_find(fieldCache, 25, NULL, &field);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_EQ(25, field->mFid);
    resultF64 = 0;
    mamaFieldCacheField_getF64(field, &resultF64);
    ASSERT_DOUBLE_EQ(10.9, resultF64);
    mamaFieldCacheField_isModified(field, &modified);
    ASSERT_TRUE(modified);
    
    ret = mamaFieldCache_find(fieldCache, 90, NULL, &field);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_EQ(90, field->mFid);
    resultString = NULL;
    mamaFieldCacheField_getString(field, &resultString, &len);
    ASSERT_STREQ("hello world", resultString);
    mamaFieldCacheField_isModified(field, &modified);
    ASSERT_TRUE(modified);

    ret = mamaFieldCache_destroy(fieldCache);
    mamaMsg_destroy(message);
}

TEST_F(MamaFieldCacheTestC, applyMsgUsingNames)
{
    mamaFieldCache fieldCache = NULL;
    mama_size_t size = 0;
    const char* name = "";
    mama_status ret = mamaFieldCache_create(&fieldCache);
    mamaFieldCache_setUseFieldNames(fieldCache, 1);
    
    mamaMsg message;
    mamaMsg_create(&message);
    mamaMsg_addBool(message, "test_bool", 10, 1);
    mamaMsg_addF64(message, "test_f64", 25, 12.3);
    mamaMsg_addI32(message, "test_i32", 66, -101);
    mamaMsg_addString(message, "test_string", 90, "hello world");
    
    ret = mamaFieldCache_applyMessage(fieldCache, message, NULL);
    
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ret = mamaFieldCache_getSize(fieldCache, &size);
    ASSERT_EQ(4, size);

    mamaFieldCacheField field = NULL;
    ret = mamaFieldCache_find(fieldCache, 10, NULL, &field);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_EQ(10, field->mFid);
    mamaFieldCacheField_getName(field, &name);
    ASSERT_STREQ("test_bool", name);

    field = NULL;
    ret = mamaFieldCache_find(fieldCache, 25, NULL, &field);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_EQ(25, field->mFid);
    mamaFieldCacheField_getName(field, &name);
    ASSERT_STREQ("test_f64", name);

    field = NULL;
    ret = mamaFieldCache_find(fieldCache, 66, NULL, &field);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_EQ(66, field->mFid);
    mamaFieldCacheField_getName(field, &name);
    ASSERT_STREQ("test_i32", name);

    field = NULL;
    ret = mamaFieldCache_find(fieldCache, 90, NULL, &field);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_EQ(90, field->mFid);
    mamaFieldCacheField_getName(field, &name);
    ASSERT_STREQ("test_string", name);

    mamaMsg_destroy (message);

    ret = mamaFieldCache_destroy(fieldCache);
}

TEST_F(MamaFieldCacheTestC, getFullMsg1)
{
    mamaFieldCache fieldCache = NULL;
    mama_status ret = mamaFieldCache_create(&fieldCache);

    mamaFieldCacheField field = NULL;
    mamaFieldCacheField_create(&field, 10, MAMA_FIELD_TYPE_BOOL, NULL);
    mamaFieldCacheField_setBool(field, 1);
    mamaFieldCache_applyField(fieldCache, field);

    /* This field must never be published */
    mamaFieldCacheField_create(&field, 25, MAMA_FIELD_TYPE_F64, NULL);
    mamaFieldCacheField_setF64(field, 3.1);
    mamaFieldCacheField_setPublish(field, 0);
    mamaFieldCache_applyField(fieldCache, field);

    mamaFieldCacheField_create(&field, 66, MAMA_FIELD_TYPE_I32, NULL);
    mamaFieldCacheField_setI32(field, -100);
    mamaFieldCache_applyField(fieldCache, field);

    // Creating a string field without a value. Even if this field is added to the
    // cache, it will not be added to the message because we cannot add a NULL
    // string to a mama message
    mamaFieldCacheField_create(&field, 110, MAMA_FIELD_TYPE_STRING, NULL);
    mamaFieldCache_applyField(fieldCache, field);

    mamaFieldCacheField_create(&field, 90, MAMA_FIELD_TYPE_STRING, NULL);
    mamaFieldCacheField_setString(field, "hello world", 0);
    mamaFieldCache_applyField(fieldCache, field);

    mamaMsg message;
    mamaMsg_create(&message);

    // should use add* methods to populate the message because the message is empty
    mamaFieldCache_getFullMessage(fieldCache, message);

    mama_bool_t resultBool = 0;
    ret = mamaMsg_getBool(message, "test_bool", 10, &resultBool);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    mama_f64_t resultF64;
    ret = mamaMsg_getF64(message, "test_f32", 25, &resultF64);
    ASSERT_EQ(MAMA_STATUS_NOT_FOUND, ret);
    mama_i32_t resultI32;
    ret = mamaMsg_getI32(message, "test_i32", 66, &resultI32);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    const char* resultString;
    ret = mamaMsg_getString(message, "test_string", 90, &resultString);
    ASSERT_EQ(MAMA_STATUS_OK, ret);

    ASSERT_EQ(1, resultBool);
    ASSERT_DOUBLE_EQ(-100, resultI32);
    ASSERT_STREQ("hello world", resultString);

    mamaMsg_destroy (message);
    ret = mamaFieldCache_destroy(fieldCache);
}

TEST_F(MamaFieldCacheTestC, getFullMsg2)
{
    mamaFieldCache fieldCache = NULL;
    mama_status ret = mamaFieldCache_create(&fieldCache);    
   
    mamaFieldCacheField field = NULL;
    mamaFieldCacheField_create(&field, 10, MAMA_FIELD_TYPE_BOOL, NULL);
    mamaFieldCacheField_setBool(field, 1);
//    mamaFieldCacheField_setModified(field, 0); // Set this field to unmodified
    mamaFieldCache_applyField(fieldCache, field);
    
    /* This field must never be published */
    mamaFieldCacheField_create(&field, 25, MAMA_FIELD_TYPE_F64, NULL);
    mamaFieldCacheField_setF64(field, 3.1);
    mamaFieldCacheField_setPublish(field, 0);
    mamaFieldCache_applyField(fieldCache, field);
    
    mamaFieldCacheField_create(&field, 66, MAMA_FIELD_TYPE_I32, NULL);
    mamaFieldCacheField_setI32(field, -100);
    mamaFieldCache_applyField(fieldCache, field);
    
    // Creating a string field without a value. Even if this field is added to the
    // cache, it will not be added to the message because we cannot add a NULL
    // string to a mama message
    mamaFieldCacheField_create(&field, 110, MAMA_FIELD_TYPE_STRING, NULL);
    mamaFieldCache_applyField(fieldCache, field);
    
    mamaFieldCacheField_create(&field, 90, MAMA_FIELD_TYPE_STRING, NULL);
    mamaFieldCacheField_setString(field, "hello world", 0);
    mamaFieldCache_applyField(fieldCache, field);
    
    mamaMsg message;
    mamaMsg_create(&message);

    // Cache is not using names so pass NULL
    mamaMsg_addBool(message, NULL, 1, 1);

    // should use update* methods to populate the message because the message is not empty
    mamaFieldCache_getFullMessage(fieldCache, message);
    
    mama_bool_t resultBool;
    ret = mamaMsg_getBool(message, "fake_bool", 1, &resultBool);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_EQ(1, resultBool);
    ret = mamaMsg_getBool(message, "test_bool", 10, &resultBool);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    mama_f64_t resultF64;
    ret = mamaMsg_getF64(message, "test_f32", 25, &resultF64);
    ASSERT_EQ(MAMA_STATUS_NOT_FOUND, ret);
    mama_i32_t resultI32;
    ret = mamaMsg_getI32(message, "test_i32", 66, &resultI32);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    const char* resultString;
    ret = mamaMsg_getString(message, "test_string", 90, &resultString);
    ASSERT_EQ(MAMA_STATUS_OK, ret);

    ASSERT_EQ(1, resultBool);
    ASSERT_DOUBLE_EQ(-100, resultI32);
    ASSERT_STREQ("hello world", resultString);

    mamaMsg_destroy (message);
    ret = mamaFieldCache_destroy(fieldCache);
}

TEST_F(MamaFieldCacheTestC, getDeltaMsgNotTrackingModif)
{
    mama_bool_t modified = 0;
    mamaFieldCache fieldCache = NULL;
    mama_status ret = mamaFieldCache_create(&fieldCache);

    // setting explicitely to non-tracking mode
    mamaFieldCache_setTrackModified(fieldCache, 0);

    // This field will be published
    mamaFieldCacheField field = NULL;
    mamaFieldCacheField_create(&field, 10, MAMA_FIELD_TYPE_BOOL, NULL);
    mamaFieldCacheField_setBool(field, 1);
    // Enable publishing
    mamaFieldCacheField_setPublish(field, 1);
    // Check if modified before publishing - who cares... global check modified is disabled
    mamaFieldCacheField_setCheckModified(field, 1);
    mamaFieldCache_applyField(fieldCache, field);
    mamaFieldCacheField_destroy(field);

    // This field will not be published
    mamaFieldCacheField_create(&field, 25, MAMA_FIELD_TYPE_F64, NULL);
    mamaFieldCacheField_setF64(field, 3.1);
    // Disable publishing
    mamaFieldCacheField_setPublish(field, 0);
    // Check modified can be anything - no publishing anyway... see above
    mamaFieldCacheField_setCheckModified(field, 1);
    mamaFieldCache_applyField(fieldCache, field);
    mamaFieldCacheField_destroy(field);

    // This field will be published
    mamaFieldCacheField_create(&field, 66, MAMA_FIELD_TYPE_I32, NULL);
    mamaFieldCacheField_setI32(field, -100);
    // Enable publishing
    mamaFieldCacheField_setPublish(field, 1);
    // Check if modified - who cares... global check modified is disabled
    mamaFieldCacheField_setCheckModified(field, 1);
    mamaFieldCache_applyField(fieldCache, field);
    mamaFieldCacheField_destroy(field);

    // This field will be published
    mamaFieldCacheField_create(&field, 90, MAMA_FIELD_TYPE_STRING, NULL);
    mamaFieldCacheField_setString(field, "hello world", 0);
    // Enable publishing
    mamaFieldCacheField_setPublish(field, 1);
    // Don't check if modified - who cares... global check modified is disabled
    mamaFieldCacheField_setCheckModified(field, 0);
    mamaFieldCache_applyField(fieldCache, field);
    mamaFieldCacheField_destroy(field);

    // This field will be published
    mamaFieldCacheField_create(&field, 91, MAMA_FIELD_TYPE_STRING, NULL);
    mamaFieldCacheField_setString(field, "hello world again", 0);
    // Enable publishing
    mamaFieldCacheField_setPublish(field, 1);
    // Don't check if modified - who cares... global check modified is disabled
    mamaFieldCacheField_setCheckModified(field, 1);
    mamaFieldCache_applyField(fieldCache, field);
    mamaFieldCacheField_destroy(field);

    mamaMsg message;
    mamaMsg_create(&message);

    // This results in a Full message because track modified is false
    mamaFieldCache_getDeltaMessage(fieldCache, message);

    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCache_find(fieldCache, 10, "", &field));
    mamaFieldCacheField_isModified(field, &modified);
    // The modified flag is not modified if the cache is not tracking modifications
    ASSERT_FALSE(modified);

    mama_bool_t resultBool;
    ret = mamaMsg_getBool(message, "test_bool", 10, &resultBool);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_EQ(1, resultBool);
    mama_f64_t resultF64;
    ret = mamaMsg_getF64(message, "test_f32", 25, &resultF64);
    ASSERT_EQ(MAMA_STATUS_NOT_FOUND, ret);
    mama_i32_t resultI32;
    ret = mamaMsg_getI32(message, "test_i32", 66, &resultI32);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_EQ(-100, resultI32);
    const char* resultString;
    ret = mamaMsg_getString(message, "test_string", 90, &resultString);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_STREQ("hello world", resultString);
    ret = mamaMsg_getString(message, "test_string", 91, &resultString);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_STREQ("hello world again", resultString);

    mamaMsg_destroy (message);

    ret = mamaFieldCache_destroy(fieldCache);
}

TEST_F(MamaFieldCacheTestC, getDeltaMsgAlwaysPublishFields)
{
    mama_bool_t modified = 0;
    mama_size_t numFields = 0;
    mamaFieldCache fieldCache = NULL;
    mama_status ret = mamaFieldCache_create(&fieldCache);
    
    // tracking modification state globally at cache level
    mamaFieldCache_setTrackModified(fieldCache, 1);

    mamaFieldCacheField field = NULL;
    // This field must be always published
    mamaFieldCacheField_create(&field, 66, MAMA_FIELD_TYPE_I32, NULL);
    mamaFieldCacheField_setI32(field, -100);
    // Enable publishing
    mamaFieldCacheField_setPublish(field, 1);
    // Don't check if modified before publishing -> always publish
    mamaFieldCacheField_setCheckModified(field, 0);
    mamaFieldCache_applyField(fieldCache, field);
    mamaFieldCacheField_destroy(field);

    mama_size_t size = 0;
    mamaFieldCacheList_getSize(fieldCache->mModifiedFields, &size);
    ASSERT_EQ(0, size);
    mamaFieldCacheList_getSize(fieldCache->mAlwaysPublishFields, &size);
    ASSERT_EQ(1, size);

    mamaMsg message;
    mamaMsg_create(&message);

    mamaFieldCache_getDeltaMessage(fieldCache, message);
    mamaMsg_getNumFields(message, &numFields);
    ASSERT_EQ(1, numFields);
    mamaMsg_destroy(message);
    message = NULL;

    mamaMsg_create(&message);

    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCache_find(fieldCache, 66, "", &field));
    mamaFieldCacheField_isModified(field, &modified);
    ASSERT_FALSE(modified);

    mamaFieldCache_getDeltaMessage(fieldCache, message);

    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCache_find(fieldCache, 66, "", &field));
    mamaFieldCacheField_isModified(field, &modified);
    ASSERT_FALSE(modified);

    mama_i32_t resultI32 = 0;
    ret = mamaMsg_getI32(message, "test_i32", 66, &resultI32);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_EQ(-100, resultI32);

    mamaMsg_destroy (message);
    ret = mamaFieldCache_destroy(fieldCache);
}

TEST_F(MamaFieldCacheTestC, getDeltaMsgTrackingModif)
{
    mama_size_t size = 0;
    mama_bool_t modified = 0;
    mamaFieldCache fieldCache = NULL;
    mama_status ret = mamaFieldCache_create(&fieldCache);

    // tracking modification state globally at cache level
    mamaFieldCache_setTrackModified(fieldCache, 1);

    mamaFieldCacheField field = NULL;
    // This field must be always published
    mamaFieldCacheField_create(&field, 66, MAMA_FIELD_TYPE_I32, NULL);
    mamaFieldCacheField_setI32(field, -100);
    // Enable publishing
    mamaFieldCacheField_setPublish(field, 1);
    // Don't check if modified before publishing -> always publish
    mamaFieldCacheField_setCheckModified(field, 0);
    mamaFieldCache_applyField(fieldCache, field);
    mamaFieldCacheField_destroy(field);

    size = 0;
    mamaFieldCacheList_getSize(fieldCache->mAlwaysPublishFields, &size);
    ASSERT_EQ(1, size);
    size = 0;
    mamaFieldCacheList_getSize(fieldCache->mModifiedFields, &size);
    ASSERT_EQ(0, size);

    mamaMsg message;
    mamaMsg_create(&message);

    // reset the modified field and remove from list for field 66
    mamaFieldCache_getDeltaMessage(fieldCache, message);
    mamaMsg_destroy(message);
    message = NULL;

    size = 0;
    mamaFieldCacheList_getSize(fieldCache->mAlwaysPublishFields, &size);
    ASSERT_EQ(1, size);
    size = 0;
    mamaFieldCacheList_getSize(fieldCache->mModifiedFields, &size);
    ASSERT_EQ(0, size);

    // This field will be published
    mamaFieldCacheField_create(&field, 10, MAMA_FIELD_TYPE_BOOL, NULL);
    mamaFieldCacheField_setBool(field, 1);
    // Enable publishing
    mamaFieldCacheField_setPublish(field, 1);
    // Check if modified before publishing
    mamaFieldCacheField_setCheckModified(field, 1);
    mamaFieldCache_applyField(fieldCache, field);
    mamaFieldCacheField_destroy(field);

    size = 0;
    mamaFieldCacheList_getSize(fieldCache->mModifiedFields, &size);
    ASSERT_EQ(1, size);

    // This field must never be published because publish is false
    mamaFieldCacheField_create(&field, 25, MAMA_FIELD_TYPE_F64, NULL);
    mamaFieldCacheField_setF64(field, 3.1);
    // Disable publishing
    mamaFieldCacheField_setPublish(field, 0);
    // Check modified can be anything - no publishing anyway... see above
    mamaFieldCacheField_setCheckModified(field, 1);
    mamaFieldCache_applyField(fieldCache, field);
    mamaFieldCacheField_destroy(field);

    size = 0;
    mamaFieldCacheList_getSize(fieldCache->mModifiedFields, &size);
    ASSERT_EQ(1, size);

    // This field will always be published
    mamaFieldCacheField_create(&field, 90, MAMA_FIELD_TYPE_STRING, NULL);
    mamaFieldCacheField_setString(field, "hello world", 0);
    // Enable publishing
    mamaFieldCacheField_setPublish(field, 1);
    // Don't check if modified
    mamaFieldCacheField_setCheckModified(field, 0);
    mamaFieldCache_applyField(fieldCache, field);
    mamaFieldCacheField_destroy(field);

    size = 0;
    mamaFieldCacheList_getSize(fieldCache->mModifiedFields, &size);
    ASSERT_EQ(1, size);

    // This field will always be published
    mamaFieldCacheField_create(&field, 91, MAMA_FIELD_TYPE_STRING, NULL);
    mamaFieldCacheField_setString(field, "hello world again", 0);
    // Enable publishing
    mamaFieldCacheField_setPublish(field, 1);
    // Don't check if modified
    mamaFieldCacheField_setCheckModified(field, 0);
    mamaFieldCache_applyField(fieldCache, field);
    mamaFieldCacheField_destroy(field);

    size = 0;
    mamaFieldCacheList_getSize(fieldCache->mModifiedFields, &size);
    ASSERT_EQ(1, size);

    mamaMsg_create(&message);

    mamaFieldCache_getDeltaMessage(fieldCache, message);

    size = 0;
    mamaFieldCacheList_getSize(fieldCache->mModifiedFields, &size);
    ASSERT_EQ(0, size);

    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCache_find(fieldCache, 10, "", &field));
    mamaFieldCacheField_isModified(field, &modified);
    ASSERT_FALSE(modified);

    mama_bool_t resultBool;
    ret = mamaMsg_getBool(message, "test_bool", 10, &resultBool);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_EQ(1, resultBool);
    mama_f64_t resultF64;
    ret = mamaMsg_getF64(message, "test_f32", 25, &resultF64);
    ASSERT_EQ(MAMA_STATUS_NOT_FOUND, ret);
    mama_i32_t resultI32;
    ret = mamaMsg_getI32(message, "test_i32", 66, &resultI32);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_EQ(-100, resultI32);
    const char* resultString;
    ret = mamaMsg_getString(message, "test_string", 90, &resultString);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_STREQ("hello world", resultString);
    ret = mamaMsg_getString(message, "test_string", 91, &resultString);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_STREQ("hello world again", resultString);

    mamaMsg_destroy (message);

    ret = mamaFieldCache_destroy(fieldCache);
}

TEST_F(MamaFieldCacheTestC, getDeltaMsgMultipleUpdates)
{
    mama_size_t size = 0;
    mama_bool_t modified = 0;
    mamaFieldCache fieldCache = NULL;
    mama_status ret = mamaFieldCache_create(&fieldCache);

    // tracking modification state globally at cache level
//    mamaFieldCache_setTrackModified(fieldCache, 1);

    mamaFieldCacheField field = NULL;
    mamaFieldCacheField_create(&field, 66, MAMA_FIELD_TYPE_I32, NULL);
    mamaFieldCacheField_setI32(field, -100);
    // Enable publishing
    mamaFieldCacheField_setPublish(field, 1);
    // Check if modified before publishing
    mamaFieldCacheField_setCheckModified(field, 1);

    mamaFieldCache_applyField(fieldCache, field);

    size = 0;
    mamaFieldCacheList_getSize(fieldCache->mModifiedFields, &size);
    ASSERT_EQ(1, size);

    // apply a second time before getDeltaMsg is called
    mamaFieldCacheField_setI32(field, 80);
    mamaFieldCache_applyField(fieldCache, field);

    size = 0;
    mamaFieldCacheList_getSize(fieldCache->mModifiedFields, &size);
    ASSERT_EQ(1, size);

    mamaFieldCacheField_destroy(field);

    // This field must never be published because publish is false
    mamaFieldCacheField_create(&field, 25, MAMA_FIELD_TYPE_F64, NULL);
    mamaFieldCacheField_setF64(field, 3.1);
    // Disable publishing
    mamaFieldCacheField_setPublish(field, 0);
    // Check modified can be anything - no publishing anyway... see above
    mamaFieldCacheField_setCheckModified(field, 1);
    mamaFieldCache_applyField(fieldCache, field);
    mamaFieldCacheField_destroy(field);

    size = 0;
    mamaFieldCacheList_getSize(fieldCache->mModifiedFields, &size);
    ASSERT_EQ(1, size);

    size = 0;
    mamaFieldCacheList_getSize(fieldCache->mModifiedFields, &size);
    ASSERT_EQ(1, size);

    mamaMsg message = NULL;
    mamaMsg_create(&message);

    mamaFieldCache_getDeltaMessage(fieldCache, message);

    ASSERT_EQ(MAMA_STATUS_OK, mamaFieldCache_find(fieldCache, 66, "", &field));
    mamaFieldCacheField_isModified(field, &modified);
    ASSERT_FALSE(modified);

    mama_i32_t resultI32 = 0;
    ret = mamaMsg_getI32(message, "test_i32", 66, &resultI32);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_EQ(80, resultI32);

    mama_f64_t resultF64;
    ret = mamaMsg_getF64(message, "test_f32", 25, &resultF64);
    ASSERT_EQ(MAMA_STATUS_NOT_FOUND, ret);

    mamaMsg_destroy (message);

    ret = mamaFieldCache_destroy(fieldCache);
}

TEST_F(MamaFieldCacheTestC, clear)
{
    mamaFieldCache fieldCache = NULL;
    mama_size_t size = 0;
    mama_status ret = mamaFieldCache_create(&fieldCache);
    ASSERT_EQ(MAMA_STATUS_OK, ret);

    ret = mamaFieldCache_clear(fieldCache);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    
    mama_bool_t existing = 0;
    mamaFieldCacheField field = NULL;
    ret = mamaFieldCache_findOrAdd(fieldCache, 2, MAMA_FIELD_TYPE_CHAR, NULL, &field, &existing);
    ASSERT_FALSE(existing);

    ret = mamaFieldCache_clear(fieldCache);
    ASSERT_EQ(MAMA_STATUS_OK, ret);

    ret = mamaFieldCache_getSize(fieldCache, &size);
    ASSERT_EQ(0, size);
    
    ret = mamaFieldCache_find(fieldCache, 2, NULL, &field);
    ASSERT_EQ(MAMA_STATUS_NOT_FOUND, ret);
    ASSERT_FALSE(field);

    ret = mamaFieldCache_clear(fieldCache);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    
    ret = mamaFieldCache_getSize(fieldCache, &size);
    ASSERT_EQ(0, size);

    ret = mamaFieldCache_destroy(fieldCache);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
}

TEST_F(MamaFieldCacheTestC, applyRecordNew)
{
    mama_bool_t modified = 0;
    mama_size_t size = 0;
    mamaFieldCache fieldCache = NULL;
    mama_status ret = mamaFieldCache_create(&fieldCache);
    mamaFieldCacheField field = NULL;
    mamaFieldType type;
    mama_fid_t fid;
    mamaFieldCacheRecord record;

    mamaFieldCacheRecord_create(&record);

    for (int i = 0; i < 20; ++i)
    {
        field = NULL;
        mamaFieldCacheRecord_add(record, i+1, MAMA_FIELD_TYPE_I64, NULL, &field);
    }

    mamaFieldCache_applyRecord(fieldCache, record);
    
    ret = mamaFieldCache_getSize(fieldCache, &size);
    ASSERT_EQ(20, size);

    for (int i = 0; i < 20; ++i)
    {
        ret = mamaFieldCache_find(fieldCache, i+1, NULL, &field);
        ASSERT_EQ(MAMA_STATUS_OK, ret);
        ASSERT_TRUE(field);
        mamaFieldCacheField_isModified(field, &modified);
        ASSERT_TRUE(modified);
        mamaFieldCacheField_getType(field, &type);
        ASSERT_EQ(MAMA_FIELD_TYPE_I64, type);
        mamaFieldCacheField_getFid(field, &fid);
        ASSERT_EQ(i+1, fid);
    }

    ret = mamaFieldCache_destroy(fieldCache);
    mamaFieldCacheRecord_destroy(record);
}

TEST_F(MamaFieldCacheTestC, applyRecordUpdate)
{
    mama_bool_t modified = 0;
    mama_size_t size = 0;
    mamaFieldCache fieldCache = NULL;
    mama_status ret = mamaFieldCache_create(&fieldCache);
    mamaFieldCacheField field = NULL;
    mamaFieldCacheRecord record20;
    mamaFieldCacheRecord record10;

    mamaFieldCacheRecord_create(&record20);
    mamaFieldCacheRecord_create(&record10);

    for (int i = 0; i < 20; ++i)
    {
        field = NULL;
        mamaFieldCacheRecord_add(record20, i+1, MAMA_FIELD_TYPE_I64, NULL, &field);
        
        if (i < 10)
        {
            field = NULL;
            mamaFieldCacheRecord_add(record10, i+1, MAMA_FIELD_TYPE_I64, NULL, &field);
        }
    }

    mamaFieldCache_applyRecord(fieldCache, record20);

    ret = mamaFieldCache_getSize(fieldCache, &size);
    ASSERT_EQ(20, size);
    
    for (int i = 0; i < 20; ++i)
    {
        ret = mamaFieldCache_find(fieldCache, i+1, NULL, &field);
        ASSERT_EQ(MAMA_STATUS_OK, ret);
        ASSERT_TRUE(field);
        mamaFieldCacheField_isModified(field, &modified);
        ASSERT_TRUE(modified);
    }

    // To reset the modified flag
    mamaMsg msg;
    mamaMsg_create(&msg);
    mamaFieldCache_getDeltaMessage(fieldCache, msg);

    // Don't update all the fields in cache - only the first 10
    mamaFieldCache_applyRecord(fieldCache, record10);
    
    for (int i = 0; i < 10; ++i) // Only the first 10
    {
        ret = mamaFieldCache_find(fieldCache, i+1, NULL, &field);
        ASSERT_EQ(MAMA_STATUS_OK, ret);
        ASSERT_TRUE(field);
        mamaFieldCacheField_isModified(field, &modified);
        ASSERT_TRUE(modified);
    }
    for (int i = 10; i < 20; ++i) // The last 10 are not modified
    {
        ret = mamaFieldCache_find(fieldCache, i+1, NULL, &field);
        ASSERT_EQ(MAMA_STATUS_OK, ret);
        ASSERT_TRUE(field);
        mamaFieldCacheField_isModified(field, &modified);
        ASSERT_FALSE(modified);
    }
    
    ret = mamaFieldCache_getSize(fieldCache, &size);
    ASSERT_EQ(20, size);

    ret = mamaFieldCache_destroy(fieldCache);
    mamaFieldCacheRecord_destroy(record20);
    mamaFieldCacheRecord_destroy(record10);
}
