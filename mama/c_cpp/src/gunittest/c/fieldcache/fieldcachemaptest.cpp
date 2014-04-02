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
#include <mama/fieldcache/fieldcache.h>
#include <mama/fieldcache/fieldcachefield.h>
#include "fieldcache/fieldcachefieldimpl.h"
#include "fieldcache/fieldcachemap.h"

class MamaFieldCacheMapTestC : public ::testing::Test
{
protected:

    /* Work around for problem in gtest where the this pointer can't be accessed
     * from a test fixture.
     */
    MamaFieldCacheMapTestC *m_this;

    MamaFieldCacheMapTestC(void);
    virtual ~MamaFieldCacheMapTestC(void);

    virtual void SetUp(void);
    virtual void TearDown(void);
    
private:
//    mamaFieldCache mFieldCache;
};

MamaFieldCacheMapTestC::MamaFieldCacheMapTestC(void)
    : m_this (NULL)
//    : mFieldCache(NULL)
{
}

MamaFieldCacheMapTestC::~MamaFieldCacheMapTestC(void)
{
}

void MamaFieldCacheMapTestC::SetUp(void)
{
    m_this = this;

//    mama_status ret = mamaFieldCache_create(&mFieldCache);
//    ASSERT_EQ(MAMA_STATUS_OK, ret);
}

void MamaFieldCacheMapTestC::TearDown(void)
{
//    if (mFieldCache)
//    {
//        mama_status ret = mamaFieldCache_destroy(mFieldCache);
//        ASSERT_EQ(MAMA_STATUS_OK, ret);
//    }
    
    m_this = NULL;
}

#define CREATE_MULTI_MAP \
        int type_iter = 0; \
        for ( ; type_iter < 2; type_iter++) { \
            mamaFieldCacheMap map = NULL; \
            ret = mamaFieldCacheMap_create(&map); \
            ASSERT_EQ(MAMA_STATUS_OK, ret);

#define DESTROY_MULTI_MAP \
            ret = mamaFieldCacheMap_destroy(map); \
            ASSERT_EQ(MAMA_STATUS_OK, ret); \
        }

#define CREATE_SINGLE_MAP \
        mamaFieldCacheMap map = NULL; \
        ret = mamaFieldCacheMap_create(&map); \
        ASSERT_EQ(MAMA_STATUS_OK, ret);

#define DESTROY_SINGLE_MAP \
        ret = mamaFieldCacheMap_destroy(map); \
        ASSERT_EQ(MAMA_STATUS_OK, ret);

#define CREATE_MAP CREATE_SINGLE_MAP
#define DESTROY_MAP DESTROY_SINGLE_MAP

TEST_F(MamaFieldCacheMapTestC, create) 
{
    mama_status ret;
    CREATE_MAP

    ASSERT_TRUE(map->mAddFunction != NULL);
    ASSERT_TRUE(map->mClearFunction != NULL);
    ASSERT_TRUE(map->mDestroyFunction != NULL);
    ASSERT_TRUE(map->mFindFunction != NULL);

    DESTROY_MAP
}

TEST_F(MamaFieldCacheMapTestC, add)
{
    mama_status ret;
    CREATE_MAP
    
    mamaFieldCacheField field = NULL;
    mamaFieldCacheField_create(&field, 1, MAMA_FIELD_TYPE_BOOL, NULL);
    ret = mamaFieldCacheMap_add(map, field);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    
    field = NULL;
    ret = mamaFieldCacheMap_find(map, 1, MAMA_FIELD_TYPE_BOOL, NULL, &field);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_TRUE(field != NULL);
    ASSERT_EQ(1, field->mFid);
    ASSERT_EQ(MAMA_FIELD_TYPE_BOOL, field->mType);
    
    DESTROY_MAP
}

TEST_F(MamaFieldCacheMapTestC, addSameTwice)
{
    mama_status ret;
    CREATE_MAP
    
    mamaFieldCacheField field1 = NULL;
    mamaFieldCacheField_create(&field1, 1, MAMA_FIELD_TYPE_BOOL, NULL);
    ret = mamaFieldCacheMap_add(map, field1);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    field1 = NULL;

    ret = mamaFieldCacheMap_find(map, 1, MAMA_FIELD_TYPE_BOOL, NULL, &field1);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_TRUE(field1 != NULL);
    ASSERT_EQ(1, field1->mFid);
    ASSERT_EQ(MAMA_FIELD_TYPE_BOOL, field1->mType);
    
    mamaFieldCacheField field2 = NULL;
    ret = mamaFieldCacheField_create(&field2, 1, MAMA_FIELD_TYPE_BOOL, NULL);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ret = mamaFieldCacheMap_add(map, field2);
    ASSERT_EQ(MAMA_STATUS_INVALID_ARG, ret);

    // This has to be explicitely destroyed as it was not added to the cache
    mamaFieldCacheField_destroy(field2);
    
    DESTROY_MAP
}

TEST_F(MamaFieldCacheMapTestC, add2)
{
    mama_status ret;
    CREATE_MAP
    
    mamaFieldCacheField field1 = NULL;
    ret = mamaFieldCacheField_create(&field1, 1, MAMA_FIELD_TYPE_BOOL, NULL);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ret = mamaFieldCacheMap_add(map, field1);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    
    mamaFieldCacheField field2 = NULL;
    ret = mamaFieldCacheField_create(&field2, 2, MAMA_FIELD_TYPE_U64, NULL);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ret = mamaFieldCacheMap_add(map, field2);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    
    ret = mamaFieldCacheMap_find(map, 1, MAMA_FIELD_TYPE_BOOL, NULL, &field1);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    
    ret = mamaFieldCacheMap_find(map, 2, MAMA_FIELD_TYPE_U64, NULL, &field1);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    
    DESTROY_MAP
}

TEST_F(MamaFieldCacheMapTestC, findNotFound)
{
    mama_status ret;
    CREATE_MAP
     
    mamaFieldCacheField fieldFound = NULL;
    ret = mamaFieldCacheMap_find(map, 1, MAMA_FIELD_TYPE_BOOL, NULL, &fieldFound);
    ASSERT_EQ(MAMA_STATUS_NOT_FOUND, ret);
    ASSERT_TRUE(fieldFound == NULL);
    
    DESTROY_MAP
}

TEST_F(MamaFieldCacheMapTestC, find1)
{
    mama_status ret;
    CREATE_MAP
    
    mama_i32_t data = 13;
    
    mamaFieldCacheField field1 = NULL;
    ret = mamaFieldCacheField_create(&field1, 1, MAMA_FIELD_TYPE_I32, NULL);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ret = mamaFieldCacheMap_add(map, field1);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    mamaFieldCacheField_setI32(field1, data);
    
    mamaFieldCacheField field2 = NULL;
    ret = mamaFieldCacheField_create(&field2, 2, MAMA_FIELD_TYPE_U64, NULL);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ret = mamaFieldCacheMap_add(map, field2);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    
    mamaFieldCacheField fieldFound = NULL;
    ret = mamaFieldCacheMap_find(map, 1, MAMA_FIELD_TYPE_I32, NULL, &fieldFound);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    ASSERT_TRUE(fieldFound != NULL);
    ASSERT_EQ(1, fieldFound->mFid);
    ASSERT_EQ(MAMA_FIELD_TYPE_I32, fieldFound->mType);
    mama_i32_t result = 0;
    ret = mamaFieldCacheField_getI32(fieldFound, &result);
    ASSERT_EQ(data, result);
    
    DESTROY_MAP
}

TEST_F(MamaFieldCacheMapTestC, addRealloc)
{
    mama_status ret;
    CREATE_MAP
            
    // 1 2 3 4 5 6 7 8 9 11 12 13 14 15 16 17 18 19 21...
    /* Add a lot of fields to allow buffer reallocation */
    for (int i = 0; i < 128; ++i)
    {
        if (i % 10 == 0) // Do not add some fields
            continue;
        mamaFieldCacheField field = NULL;
        ret = mamaFieldCacheField_create(&field, i+1, MAMA_FIELD_TYPE_BOOL, NULL);
        ASSERT_EQ(MAMA_STATUS_OK, ret);
        ret = mamaFieldCacheMap_add(map, field);
        ASSERT_EQ(MAMA_STATUS_OK, ret);
    }

    /* Find if all the fields are there */
    for (int i = 0; i < 128; ++i)
    {
       mamaFieldCacheField field = NULL;
        if (i % 10 == 0)
        {
            ret = mamaFieldCacheMap_find(map, i+1, MAMA_FIELD_TYPE_BOOL, NULL, &field);
            ASSERT_EQ(MAMA_STATUS_NOT_FOUND, ret);
            ASSERT_TRUE(field == NULL);
        }
        else
        {
            ret = mamaFieldCacheMap_find(map, i+1, MAMA_FIELD_TYPE_BOOL, NULL, &field);
            ASSERT_EQ(MAMA_STATUS_OK, ret);
            ASSERT_TRUE(field != NULL);
            ASSERT_EQ(i+1, field->mFid);
        }
    }
    
    DESTROY_MAP
}

TEST_F(MamaFieldCacheMapTestC, addReallocWithEmpties)
{
    // create a dummy cache to simulate empties in the internal fid ids
    mama_status ret;
    mamaFieldCacheMap mapDummy = NULL;
    ret = mamaFieldCacheMap_create(&mapDummy);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
    
    CREATE_SINGLE_MAP

    for (int i = 0; i < 512; ++i)
    {
        mamaFieldCacheField field = NULL;
        mamaFieldCacheField_create(&field, i+1, MAMA_FIELD_TYPE_BOOL, NULL);
        ASSERT_EQ(MAMA_STATUS_OK, ret);
        if (i % 10 == 0) // Do not add some fields
        {
            ret = mamaFieldCacheMap_add(mapDummy, field);
        }
        else
        {
            ret = mamaFieldCacheMap_add(map, field);
        }
        ASSERT_EQ(MAMA_STATUS_OK, ret);
    }
    
    /* Find if all the fields are there */
    for (int i = 0; i < 512; ++i)
    {
        mamaFieldCacheField field = NULL;
        if (i % 10 == 0) // check empty fields
        {
            ret = mamaFieldCacheMap_find(mapDummy, i+1, MAMA_FIELD_TYPE_BOOL, NULL, &field);
            ASSERT_EQ(MAMA_STATUS_OK, ret);
            ASSERT_TRUE(field != NULL);
            ASSERT_EQ(i+1, field->mFid);
        }
        else
        {
            ret = mamaFieldCacheMap_find(map, i+1, MAMA_FIELD_TYPE_BOOL, NULL, &field);
            ASSERT_EQ(MAMA_STATUS_OK, ret);
            ASSERT_TRUE(field != NULL);
            ASSERT_EQ(i+1, field->mFid);
        }
    }
    
    DESTROY_SINGLE_MAP
            
    ret = mamaFieldCacheMap_destroy(mapDummy);
    ASSERT_EQ(MAMA_STATUS_OK, ret);
}
