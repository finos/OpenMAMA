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
#include <mama/fieldcache/fieldcacherecord.h>
#include "fieldcache/fieldcacherecordimpl.h"

class MamaFieldCacheRecordTestC : public ::testing::Test
{
protected:

    /* Work around for problem in gtest where the this pointer can't be accessed
     * from a test fixture.
     */
    MamaFieldCacheRecordTestC *m_this;

    MamaFieldCacheRecordTestC();
    virtual ~MamaFieldCacheRecordTestC();

    virtual void SetUp();
    virtual void TearDown();
};

MamaFieldCacheRecordTestC::MamaFieldCacheRecordTestC()
    : m_this (NULL)
{
}

MamaFieldCacheRecordTestC::~MamaFieldCacheRecordTestC()
{
}

void MamaFieldCacheRecordTestC::SetUp()
{
    m_this = this;
}

void MamaFieldCacheRecordTestC::TearDown()
{
    m_this = NULL;
}

TEST_F(MamaFieldCacheRecordTestC, create)
{
    mamaFieldCacheRecord record = NULL;
    mama_status ret = mamaFieldCacheRecord_create(&record);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    
    mama_size_t size;
    ret = mamaFieldCacheRecord_getSize(record, &size);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_EQ(0, size);

    ret = mamaFieldCacheRecord_destroy(record);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
}

TEST_F(MamaFieldCacheRecordTestC, add)
{
    mamaFieldCacheRecord record = NULL;
    mama_status ret = mamaFieldCacheRecord_create(&record);
    ASSERT_EQ(MAMA_STATUS_OK, ret);

    mamaFieldCacheField field;
    ret = mamaFieldCacheRecord_add(record, 1, MAMA_FIELD_TYPE_CHAR, NULL, &field);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    
    mama_size_t size;
    mamaFieldCacheRecord_getSize(record, &size);
    ASSERT_EQ(1, size);

    ret = mamaFieldCacheRecord_destroy(record);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
}

TEST_F(MamaFieldCacheRecordTestC, addResize)
{
    mamaFieldCacheRecord record = NULL;
    mama_status ret = mamaFieldCacheRecord_create(&record);
    ASSERT_EQ(MAMA_STATUS_OK, ret);

    for (int i = 0; i < 15; ++i)
    {
        mamaFieldCacheField field;
        ret = mamaFieldCacheRecord_add(record, i, MAMA_FIELD_TYPE_CHAR, NULL, &field);
        ASSERT_EQ(MAMA_STATUS_OK, ret);
    }
    
    mama_size_t size;
    mamaFieldCacheRecord_getSize(record, &size);
    ASSERT_EQ(15, size);

    ret = mamaFieldCacheRecord_destroy(record);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
}

TEST_F(MamaFieldCacheRecordTestC, find)
{
    mamaFieldCacheRecord record = NULL;
    mama_status ret = mamaFieldCacheRecord_create(&record);
    ASSERT_EQ(MAMA_STATUS_OK, ret);

    mamaFieldCacheField field;
    ret = mamaFieldCacheRecord_add(record, 7, MAMA_FIELD_TYPE_CHAR, "", &field);
    ASSERT_EQ(MAMA_STATUS_OK, ret);

    field = NULL;
    ret = mamaFieldCacheRecord_find(record, 1, NULL, &field);
    ASSERT_EQ(MAMA_STATUS_NOT_FOUND, ret);
    ASSERT_FALSE(field);
    ret = mamaFieldCacheRecord_find(record, 7, NULL, &field);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_TRUE(field);
    mamaFieldType type;
    mamaFieldCacheField_getType(field, &type);
    ASSERT_EQ(MAMA_FIELD_TYPE_CHAR, type);

    ret = mamaFieldCacheRecord_destroy(record);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
}

TEST_F(MamaFieldCacheRecordTestC, getField)
{
    mamaFieldType type;
    mamaFieldCacheRecord record = NULL;
    mama_status ret = mamaFieldCacheRecord_create(&record);
    ASSERT_EQ(MAMA_STATUS_OK, ret);

    mamaFieldCacheField field;
    mamaFieldCacheRecord_add(record, 7, MAMA_FIELD_TYPE_CHAR, "", &field);
    mamaFieldCacheRecord_add(record, 99, MAMA_FIELD_TYPE_F32, NULL, &field);
    
    field = NULL;
    ret = mamaFieldCacheRecord_getField(record, 2, &field);
    ASSERT_EQ(MAMA_STATUS_INVALID_ARG, ret);
    ASSERT_FALSE(field);

    ret = mamaFieldCacheRecord_getField(record, 0, &field);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_TRUE(field);
    mamaFieldCacheField_getType(field, &type);
    ASSERT_EQ(MAMA_FIELD_TYPE_CHAR, type);
    
    ret = mamaFieldCacheRecord_getField(record, 1, &field);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_TRUE(field);
    mamaFieldCacheField_getType(field, &type);
    ASSERT_EQ(MAMA_FIELD_TYPE_F32, type);
    
    ret = mamaFieldCacheRecord_destroy(record);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
}

TEST_F(MamaFieldCacheRecordTestC, clear)
{
    mamaFieldCacheRecord record = NULL;
    mama_status ret = mamaFieldCacheRecord_create(&record);
    ASSERT_EQ(MAMA_STATUS_OK, ret);

    mamaFieldCacheField field;
    mamaFieldCacheRecord_add(record, 7, MAMA_FIELD_TYPE_CHAR, NULL, &field);
    mamaFieldCacheRecord_add(record, 99, MAMA_FIELD_TYPE_F32, NULL, &field);
    
    mamaFieldCacheRecord_clear(record);
    
    mama_size_t size;
    mamaFieldCacheRecord_getSize(record, &size);
    ASSERT_EQ(0, size);
    
    field = NULL;
    ret = mamaFieldCacheRecord_getField(record, 0, &field);
    ASSERT_EQ(MAMA_STATUS_INVALID_ARG, ret);
    ASSERT_FALSE(field);
    
    field = NULL;
    ret = mamaFieldCacheRecord_getField(record, 1, &field);
    ASSERT_EQ(MAMA_STATUS_INVALID_ARG, ret);
    ASSERT_FALSE(field);
    
    ret = mamaFieldCacheRecord_destroy(record);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
}

TEST_F(MamaFieldCacheRecordTestC, reuseAfterClear)
{
    mamaFieldCacheRecord record = NULL;
    mama_status ret = mamaFieldCacheRecord_create(&record);
    ASSERT_EQ(MAMA_STATUS_OK, ret);

    mamaFieldCacheField field;
    mamaFieldCacheRecord_add(record, 7, MAMA_FIELD_TYPE_CHAR, NULL, &field);
    mamaFieldCacheRecord_add(record, 99, MAMA_FIELD_TYPE_F32, NULL, &field);
    
    mamaFieldCacheRecord_clear(record);
    
    mama_size_t size;
    mamaFieldCacheRecord_getSize(record, &size);
    ASSERT_EQ(0, size);

    mamaFieldCacheRecord_add(record, 7, MAMA_FIELD_TYPE_CHAR, NULL, &field);
    ASSERT_EQ(0, size);
    
    field = NULL;
    ret = mamaFieldCacheRecord_getField(record, 0, &field);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_TRUE(field);
    
    ret = mamaFieldCacheRecord_destroy(record);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
}
