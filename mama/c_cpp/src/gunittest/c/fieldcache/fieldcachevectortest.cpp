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
#include "fieldcache/fieldcachevector.h"

class MamaFieldCacheVectorTestC : public ::testing::Test
{
protected:

    /* Work around for problem in gtest where the this pointer can't be accessed
     * from a test fixture.
     */
    MamaFieldCacheVectorTestC *m_this;

    MamaFieldCacheVectorTestC();
    virtual ~MamaFieldCacheVectorTestC();

    virtual void SetUp();
    virtual void TearDown();
};

MamaFieldCacheVectorTestC::MamaFieldCacheVectorTestC()
    : m_this (NULL)
{
}

MamaFieldCacheVectorTestC::~MamaFieldCacheVectorTestC()
{
}

void MamaFieldCacheVectorTestC::SetUp()
{
    m_this = this;
}

void MamaFieldCacheVectorTestC::TearDown()
{
    m_this = NULL;
}

TEST_F(MamaFieldCacheVectorTestC, create)
{
    mama_status ret;
    mamaFieldCacheVector vector;
    ret = mamaFieldCacheVector_create(&vector);
    ASSERT_EQ(MAMA_STATUS_OK, ret);

    mama_size_t size = 100;
    ret = mamaFieldCacheVector_getSize(vector, &size);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_EQ(0, size);
    
    ret = mamaFieldCacheVector_destroy(vector);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
}

TEST_F(MamaFieldCacheVectorTestC, emptySetGet)
{
    mama_status ret;
    mamaFieldCacheVector vector;
    ret = mamaFieldCacheVector_create(&vector);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    
    mama_size_t size = 0;
    ret = mamaFieldCacheVector_getSize(vector, &size);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_EQ(0, size);
    
    void* value = NULL;
    ret = mamaFieldCacheVector_set(vector, 2, value);
    ASSERT_EQ(MAMA_STATUS_INVALID_ARG, ret);
    
    ret = mamaFieldCacheVector_get(vector, 2, &value);
    ASSERT_EQ(MAMA_STATUS_INVALID_ARG, ret);
    
    ret = mamaFieldCacheVector_destroy(vector);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
}

TEST_F(MamaFieldCacheVectorTestC, setGet)
{
    mama_status ret;
    mamaFieldCacheVector vector;
    ret = mamaFieldCacheVector_create(&vector);
    ASSERT_EQ(MAMA_STATUS_OK, ret);

    ret = mamaFieldCacheVector_grow(vector, 10);
    ASSERT_EQ(MAMA_STATUS_OK, ret);

    void* value = (void*)&ret;
    ret = mamaFieldCacheVector_set(vector, 0, value);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ret = mamaFieldCacheVector_set(vector, 1, value);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ret = mamaFieldCacheVector_set(vector, 4, value);
    ASSERT_EQ(MAMA_STATUS_OK, ret);

    value = NULL;
    ret = mamaFieldCacheVector_get(vector, 0, &value);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_TRUE(&ret == value);
    ret = mamaFieldCacheVector_get(vector, 1, &value);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_TRUE(&ret == value);
    ret = mamaFieldCacheVector_get(vector, 2, &value);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_TRUE(NULL == value);
    ret = mamaFieldCacheVector_get(vector, 4, &value);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_TRUE(&ret == value);

    ret = mamaFieldCacheVector_destroy(vector);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
}


TEST_F(MamaFieldCacheVectorTestC, realloc)
{
    mama_status ret;
    mamaFieldCacheVector vector;
    ret = mamaFieldCacheVector_create(&vector);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    
    ret = mamaFieldCacheVector_grow(vector, 10);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    
    mama_size_t size = 0;
    ret = mamaFieldCacheVector_getSize(vector, &size);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_EQ(10, size);
    
    void* value = NULL;
    ret = mamaFieldCacheVector_set(vector, 2, value);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    
    ret = mamaFieldCacheVector_get(vector, 2, &value);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    
    ret = mamaFieldCacheVector_destroy(vector);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
}

TEST_F(MamaFieldCacheVectorTestC, shrink)
{
    mama_status ret;
    mamaFieldCacheVector vector;
    ret = mamaFieldCacheVector_create(&vector);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    
    ret = mamaFieldCacheVector_grow(vector, 10);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    
    mama_size_t size = 0;
    ret = mamaFieldCacheVector_getSize(vector, &size);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_EQ(10, size);
    
    ret = mamaFieldCacheVector_grow(vector, 8);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    
    // Vector does not shrink!
    size = 0;
    ret = mamaFieldCacheVector_getSize(vector, &size);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_EQ(10, size);
    
    void* value = NULL;
    ret = mamaFieldCacheVector_set(vector, 9, value);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    
    ret = mamaFieldCacheVector_get(vector, 9, &value);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    
    ret = mamaFieldCacheVector_destroy(vector);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
}

TEST_F(MamaFieldCacheVectorTestC, clear)
{
    mama_status ret;
    mamaFieldCacheVector vector;
    ret = mamaFieldCacheVector_create(&vector);
    ASSERT_EQ(MAMA_STATUS_OK, ret);

    ret = mamaFieldCacheVector_grow(vector, 10);
    ASSERT_EQ(MAMA_STATUS_OK, ret);

    void* value = (void*)&ret;
    ret = mamaFieldCacheVector_set(vector, 0, value);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ret = mamaFieldCacheVector_set(vector, 1, value);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ret = mamaFieldCacheVector_set(vector, 4, value);
    ASSERT_EQ(MAMA_STATUS_OK, ret);

    mamaFieldCacheVector_clear(vector);

    mama_size_t size = 0;
    ret = mamaFieldCacheVector_getSize(vector, &size);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_EQ(10, size);

    value = NULL;
    ret = mamaFieldCacheVector_get(vector, 0, &value);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_TRUE(NULL == value);
    ret = mamaFieldCacheVector_get(vector, 1, &value);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_TRUE(NULL == value);
    ret = mamaFieldCacheVector_get(vector, 2, &value);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_TRUE(NULL == value);
    ret = mamaFieldCacheVector_get(vector, 4, &value);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_TRUE(NULL == value);

    ret = mamaFieldCacheVector_destroy(vector);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
}

TEST_F(MamaFieldCacheVectorTestC, free)
{
    mama_status ret;
    mamaFieldCacheVector vector;
    ret = mamaFieldCacheVector_create(&vector);
    ASSERT_EQ(MAMA_STATUS_OK, ret);

    ret = mamaFieldCacheVector_free(vector);
    ASSERT_EQ(MAMA_STATUS_OK, ret);

    mamaFieldCacheVector_grow(vector, 10);
    ret = mamaFieldCacheVector_free(vector);
    ASSERT_EQ(MAMA_STATUS_OK, ret);

    mamaFieldCacheVector_set(vector, 3, malloc(50));
    mamaFieldCacheVector_set(vector, 6, malloc(50));

    ret = mamaFieldCacheVector_free(vector);
    ASSERT_EQ(MAMA_STATUS_OK, ret);

    ret = mamaFieldCacheVector_destroy(vector);
    ASSERT_EQ(MAMA_STATUS_OK, ret);

    // there must not be any memory leak
}
