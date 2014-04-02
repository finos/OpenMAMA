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
#include <mama/msg.h>
#include <mama/fieldcache/fieldcache.h>
#include <mama/fieldcache/fieldcachefield.h>
#include <mama/fieldcache/fieldcacheiterator.h>
#include <mama/mama.h>
#include "fieldcache/fieldcacheimpl.h"
#include "fieldcache/fieldcachefieldimpl.h"
#include "fieldcache/fieldcachemap.h"

class MamaFieldCacheIteratorTestC : public ::testing::Test
{
protected:

    /* Work around for problem in gtest where the this pointer can't be accessed
     * from a test fixture.
     */
    MamaFieldCacheIteratorTestC *m_this;

    MamaFieldCacheIteratorTestC(void);
    virtual ~MamaFieldCacheIteratorTestC(void);

    virtual void SetUp(void);
    virtual void TearDown(void);

    mamaBridge mMamaBridge;
};

MamaFieldCacheIteratorTestC::MamaFieldCacheIteratorTestC(void)
    : m_this (NULL)
{
    // To use mamaMsg
    mama_loadBridge (&mMamaBridge, getMiddleware());
    mama_open();
}

MamaFieldCacheIteratorTestC::~MamaFieldCacheIteratorTestC(void)
{
    mama_stop(mMamaBridge);
    mama_close();
}

void MamaFieldCacheIteratorTestC::SetUp(void)
{
    m_this = this;
}

void MamaFieldCacheIteratorTestC::TearDown(void)
{
    m_this = NULL;
}

TEST_F(MamaFieldCacheIteratorTestC, iteratorBegin)
{
    mamaFieldCache fieldCache = NULL;
    mama_status ret = mamaFieldCache_create(&fieldCache);
    
    mamaMsg message;
    mamaMsg_create(&message);
    mamaMsg_addBool(message, "test_bool", 10, 1);
    mamaMsg_addF64(message, "test_f32", 25, 12.3);
    mamaMsg_addI32(message, "test_i32", 66, -101);
    mamaMsg_addString(message, "test_string", 90, "hello world");
    mamaFieldCache_applyMessage(fieldCache, message, NULL);

    mamaFieldCacheIterator iterator;
    mamaFieldCacheIterator_create(&iterator, fieldCache);
    
    mamaFieldCacheField field = NULL;
    
    mama_fid_t fid = 0;
    field = mamaFieldCacheIterator_begin(iterator);
    ASSERT_TRUE(field);
    mamaFieldCacheField_getFid(field, &fid);
    ASSERT_EQ(10, fid);
    
    mamaMsg_destroy(message);
    mamaFieldCacheIterator_destroy(iterator);

    ret = mamaFieldCache_destroy(fieldCache);
}

TEST_F(MamaFieldCacheIteratorTestC, iteratorNext)
{
    mamaFieldCache fieldCache = NULL;
    mama_status ret = mamaFieldCache_create(&fieldCache);
    
    mamaMsg message;
    mamaMsg_create(&message);
    mamaMsg_addBool(message, "test_bool", 10, 1);
    mamaMsg_addF64(message, "test_f32", 25, 12.3);
    mamaMsg_addI32(message, "test_i32", 66, -101);
    mamaMsg_addString(message, "test_string", 90, "hello world");
    mamaFieldCache_applyMessage(fieldCache, message, NULL);

    mamaFieldCacheIterator iterator;
    mamaFieldCacheIterator_create(&iterator, fieldCache);
    
    mamaFieldCacheField begin = NULL;
    begin = mamaFieldCacheIterator_begin(iterator);
    
    ASSERT_TRUE(mamaFieldCacheIterator_hasNext(iterator));

    mamaFieldCacheField current = NULL;
    current = mamaFieldCacheIterator_next(iterator);
    ASSERT_TRUE(current);
    ASSERT_TRUE(current == begin);

    mama_fid_t fid = 0;
    ASSERT_TRUE(mamaFieldCacheIterator_hasNext(iterator));
    current = mamaFieldCacheIterator_next(iterator);
    ASSERT_TRUE(current);
    mamaFieldCacheField_getFid(current, &fid);
    ASSERT_EQ(25, fid);
    
    ASSERT_TRUE(mamaFieldCacheIterator_hasNext(iterator));
    current = mamaFieldCacheIterator_next(iterator);
    ASSERT_TRUE(current);
    mamaFieldCacheField_getFid(current, &fid);
    ASSERT_EQ(66, fid);
    
    ASSERT_TRUE(mamaFieldCacheIterator_hasNext(iterator));
    current = mamaFieldCacheIterator_next(iterator);
    mamaFieldCacheField_getFid(current, &fid);
    ASSERT_EQ(90, fid);
    
    ASSERT_FALSE(mamaFieldCacheIterator_hasNext(iterator));
    
    current = mamaFieldCacheIterator_next(iterator);
    ASSERT_FALSE(current);
    
    mamaMsg_destroy(message);
    mamaFieldCacheIterator_destroy(iterator);

    ret = mamaFieldCache_destroy(fieldCache);
}

TEST_F(MamaFieldCacheIteratorTestC, iteratorLoop)
{
    mamaFieldCache fieldCache = NULL;
    mama_status ret = mamaFieldCache_create(&fieldCache);
    
    mamaMsg message;
    mamaMsg_create(&message);
    mamaMsg_addBool(message, "test_bool", 10, 1);
    mamaMsg_addF64(message, "test_f32", 25, 12.3);
    mamaMsg_addI32(message, "test_i32", 66, -101);
    mamaMsg_addString(message, "test_string", 90, "hello world");
    mamaMsg_addString(message, "test_string_empty", 88, "");
    mamaFieldCache_applyMessage(fieldCache, message, NULL);

    mamaFieldCacheIterator iterator;
    mamaFieldCacheIterator_create(&iterator, fieldCache);
    
    int counter = 0;
    mamaFieldCacheField current = NULL;
    while (mamaFieldCacheIterator_hasNext(iterator))
    {
        current = mamaFieldCacheIterator_next(iterator);
        ASSERT_TRUE(current);
        counter++;
    }
    ASSERT_EQ(5, counter);
   
    mamaMsg_destroy(message);
    mamaFieldCacheIterator_destroy(iterator);

    ret = mamaFieldCache_destroy(fieldCache);
}
