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
#include "fieldcache/fieldcachelist.h"

class MamaFieldCacheListTestC : public ::testing::Test
{
protected:

    /* Work around for problem in gtest where the this pointer can't be accessed
     * from a test fixture.
     */
    MamaFieldCacheListTestC *m_this;

    MamaFieldCacheListTestC();
    virtual ~MamaFieldCacheListTestC();

    virtual void SetUp();
    virtual void TearDown();
};

MamaFieldCacheListTestC::MamaFieldCacheListTestC()
    : m_this (NULL)
{
}

MamaFieldCacheListTestC::~MamaFieldCacheListTestC()
{
}

void MamaFieldCacheListTestC::SetUp()
{
    m_this = this;
}

void MamaFieldCacheListTestC::TearDown()
{
    m_this = NULL;
}

TEST_F(MamaFieldCacheListTestC, create)
{
    mama_status ret;
    mamaFieldCacheList list;
    ret = mamaFieldCacheList_create(&list);
    ASSERT_EQ(MAMA_STATUS_OK, ret);

    mama_size_t size = 100;
    ret = mamaFieldCacheList_getSize(list, &size);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_EQ(0, size);
    
    ret = mamaFieldCacheList_destroy(list);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
}

TEST_F(MamaFieldCacheListTestC, emptySetGet)
{
    mama_status ret;
    mamaFieldCacheList list;
    ret = mamaFieldCacheList_create(&list);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    
    mama_size_t size = 0;
    ret = mamaFieldCacheList_getSize(list, &size);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_EQ(0, size);
    
    void* value = NULL;
    ret = mamaFieldCacheList_set(list, 2, value);
    ASSERT_EQ(MAMA_STATUS_INVALID_ARG, ret);
    
    ret = mamaFieldCacheList_get(list, 2, &value);
    ASSERT_EQ(MAMA_STATUS_INVALID_ARG, ret);
    
    ret = mamaFieldCacheList_destroy(list);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
}

TEST_F(MamaFieldCacheListTestC, addGet)
{
    mama_status ret;
    int realValue = 5;
    mamaFieldCacheList list;
    ret = mamaFieldCacheList_create(&list);
    ASSERT_EQ(MAMA_STATUS_OK, ret);

    void* value = &realValue;
    ret = mamaFieldCacheList_add(list, value);
    ASSERT_EQ(MAMA_STATUS_OK, ret);

    mama_size_t size = 0;
    ret = mamaFieldCacheList_getSize(list, &size);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_EQ(1, size);
    
    ret = mamaFieldCacheList_get(list, 0, &value);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_TRUE(value == &realValue);

    value = &realValue;
    ret = mamaFieldCacheList_add(list, value);

    ret = mamaFieldCacheList_get(list, 1, &value);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_TRUE(value == &realValue);

    size = 0;
    ret = mamaFieldCacheList_getSize(list, &size);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_EQ(2, size);
    
    ret = mamaFieldCacheList_destroy(list);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
}

TEST_F(MamaFieldCacheListTestC, clear)
{
    mama_status ret;
    int realValue = 5;
    mamaFieldCacheList list;
    ret = mamaFieldCacheList_create(&list);
    ASSERT_EQ(MAMA_STATUS_OK, ret);

    void* value = &realValue;
    ret = mamaFieldCacheList_add(list, value);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ret = mamaFieldCacheList_add(list, value);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ret = mamaFieldCacheList_add(list, value);
    ASSERT_EQ(MAMA_STATUS_OK, ret);

    mama_size_t size = 0;
    ret = mamaFieldCacheList_getSize(list, &size);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_EQ(3, size);

    mamaFieldCacheList_clear(list);

    ret = mamaFieldCacheList_getSize(list, &size);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_EQ(0, size);

    ret = mamaFieldCacheList_destroy(list);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
}
