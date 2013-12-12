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

/*  Description: These tests check the iteration through and access of 
 *               fields within a mamaMsg.
 */

#include "mama/mama.h"
#include "mama/msg.h"
#include <gtest/gtest.h>
#include <cstdlib>
#include "MainUnitTestC.h"
#include <iostream>
#include "bridge.h"
#include "mama/types.h"

#define MAX_FIELD_STR_LEN 64

class MsgIterateTestC : public ::testing::Test
{
protected:
    MsgIterateTestC(void);
    virtual ~MsgIterateTestC(void);

    virtual void SetUp(void);
    virtual void TearDown(void);

    mamaBridge mBridge;
};

MsgIterateTestC::MsgIterateTestC(void)
{
}

MsgIterateTestC::~MsgIterateTestC(void)
{
}

void MsgIterateTestC::SetUp(void)
{
    mama_loadBridge (&mBridge, getMiddleware());
    mama_open();
}

void MsgIterateTestC::TearDown(void)
{
    mama_close();
}

static void msgOnField (const mamaMsg	   msg,
                        const mamaMsgField field,
                        void*              closure)
{
    
    mama_fid_t    fid  = 0;
    mamaFieldType type = MAMA_FIELD_TYPE_UNKNOWN;    
    
    mamaMsgField_getFid (field, &fid);
    mamaMsgField_getType (field, &type);
    switch(type)
    {
        case MAMA_FIELD_TYPE_STRING:
        {
            char buffer[MAX_FIELD_STR_LEN];
            ASSERT_EQ (101, fid);
            mamaMsgField_getAsString (field, buffer, MAX_FIELD_STR_LEN);
            ASSERT_STREQ ("This is an iteration test.", buffer);
            break;
        }
        case MAMA_FIELD_TYPE_U8:
        {
            mama_u8_t buffer = 0;
            ASSERT_EQ (102, fid);
            mamaMsgField_getU8 (field, &buffer);
            ASSERT_EQ (8, buffer);
            break;
        }
        case MAMA_FIELD_TYPE_U16:
        {
            mama_u16_t buffer = 0;
            ASSERT_EQ (103, fid);
            mamaMsgField_getU16 (field, &buffer);
            ASSERT_EQ (16, buffer);
            break;
        }
        case MAMA_FIELD_TYPE_U32:
        {
            mama_u32_t buffer = 0;
            ASSERT_EQ (104, fid);
            mamaMsgField_getU32 (field, &buffer);
            ASSERT_EQ (32, buffer);
            break;
        }
        case MAMA_FIELD_TYPE_U64:
        {
            mama_u64_t buffer = 0;
            ASSERT_EQ (105, fid);
            mamaMsgField_getU64 (field, &buffer);
            ASSERT_EQ (64, buffer);
            break;
        }
    }
    
    ASSERT_EQ ((void*)2, closure);
}
/* ************************************************************************* */
/* Test Functions */
/* ************************************************************************* */

/*  Description:     Create a mamaMsg, add strings to fields in the message,
 *                   and iterate through the fields printing values via 
 *                   callback method.
 *
 *  Expected Result: MAMA_STATUS_OK
 */
TEST_F (MsgIterateTestC, IteratorCallback)
{
    /* create a mama message. */
    mamaMsg msg = NULL;
    mamaMsg_create (&msg);

    /* add a fields to the message. */
    mamaMsg_addString (msg, "string", 101, "This is an iteration test.");
    mamaMsg_addU8     (msg, "u8", 102, 8);
    mamaMsg_addU16    (msg, "u16", 103, 16);
    mamaMsg_addU32    (msg, "u32", 104, 32);
    mamaMsg_addU64    (msg, "u64", 105, 64);

    /* iterate through the msg's fields */
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaMsg_iterateFields (msg, msgOnField, NULL, (void*) 2));
    
    /* destroy the message. */
    mamaMsg_destroy (msg);
}

/*  Description:     Create a mamaMsg, add strings to fields in the message,
 *                   create an iterator and iterate through the msg's fields.  
 *
 *  Expected Result: MAMA_STATUS_OK
 */

TEST_F (MsgIterateTestC, CreateIterator)
{   
    mama_fid_t      fid         = 0;
    const char*     name        = NULL;
    mamaMsgIterator iterator    = NULL;
    mamaMsgField    field       = NULL;
    mamaFieldType   type        = MAMA_FIELD_TYPE_UNKNOWN;    
    char            buffer[MAX_FIELD_STR_LEN];

    /* Create a mama message. */
    mamaMsg msg = NULL;
    mamaMsg_create (&msg);
    
    /* add a fields to the message. */
    mamaMsg_addString (msg, "string", 101, "This is an iteration test.");
    mamaMsg_addU8     (msg, "u8", 102, 8);
    mamaMsg_addU16    (msg, "u16", 103, 16);
    mamaMsg_addU32    (msg, "u32", 104, 32);
    mamaMsg_addU64    (msg, "u64", 105, 64);
   
    /* Create iterator and message field required.*/
    ASSERT_EQ (MAMA_STATUS_OK, 
               mamaMsgIterator_create (&iterator,NULL));
    
    mamaMsgIterator_associate (iterator,msg);
    
    while ((field = mamaMsgIterator_next(iterator)) != NULL)
    {
        /* Operate on contents of each field. */
        mamaMsgField_getFid (field, &fid);
        mamaMsgField_getType (field, &type);
        switch(type)
        {
            case MAMA_FIELD_TYPE_STRING:
            {
                char buffer[MAX_FIELD_STR_LEN];
                ASSERT_EQ (101, fid);
                mamaMsgField_getAsString (field, buffer, MAX_FIELD_STR_LEN);
                ASSERT_STREQ ("This is an iteration test.", buffer);
                break;
            }
            case MAMA_FIELD_TYPE_U8:
            {
                mama_u8_t buffer = 0;
                ASSERT_EQ (102, fid);
                mamaMsgField_getU8 (field, &buffer);
                ASSERT_EQ (8, buffer);
                break;
            }
            case MAMA_FIELD_TYPE_U16:
            {
                mama_u16_t buffer = 0;
                ASSERT_EQ (103, fid);
                mamaMsgField_getU16 (field, &buffer);
                ASSERT_EQ (16, buffer);
                break;
            }
            case MAMA_FIELD_TYPE_U32:
            {
                mama_u32_t buffer = 0;
                ASSERT_EQ (104, fid);
                mamaMsgField_getU32 (field, &buffer);
                ASSERT_EQ (32, buffer);
                break;
            }
            case MAMA_FIELD_TYPE_U64:
            {
                mama_u64_t buffer = 0;
                ASSERT_EQ (105, fid);
                mamaMsgField_getU64 (field, &buffer);
                ASSERT_EQ (64, buffer);
                break;
            }
        }
    }

    /* destroy the message. */
    mamaMsg_destroy (msg);
}

