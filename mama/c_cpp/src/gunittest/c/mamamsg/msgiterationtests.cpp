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
#include <stdlib.h>
#include "MainUnitTestC.h"
#include <iostream>
#include <map>
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

static void MAMACALLTYPE msgOnField (const mamaMsg	   msg,
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
        default:
            break;
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
    mamaMsgIterator iterator    = NULL;
    mamaMsgField    field       = NULL;
    mamaFieldType   type        = MAMA_FIELD_TYPE_UNKNOWN;    

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
            default:
                break;
        }
    }

    mamaMsgIterator_destroy (iterator);

    /* destroy the message. */
    mamaMsg_destroy (msg);
}

class MsgNewIteratorTestC : public ::testing::Test
{
protected:
    MsgNewIteratorTestC(void);
    virtual ~MsgNewIteratorTestC(void);

    virtual void SetUp(void);
    virtual void TearDown(void);

    mamaBridge      mBridge;
    mamaMsg         msg;
    mamaMsgIterator iterator;
    mamaDictionary  dict;
    mamaMsgField    field;
    std::map<mama_fid_t, uint64_t> values;
};

MsgNewIteratorTestC::MsgNewIteratorTestC(void)
    : mBridge(NULL),
      msg(NULL),
      iterator(NULL),
      dict(NULL),
      field(NULL)
{
}

MsgNewIteratorTestC::~MsgNewIteratorTestC(void)
{
}

void MsgNewIteratorTestC::SetUp(void)
{
    mama_loadBridge (&mBridge, getMiddleware());
    mama_open();

    /* add a fields to the message. */
    mamaMsg_create    (&msg);
    for (mama_fid_t f = 101; f < 106; f++)
    {
        char buf[64];
        sprintf (buf, "field_%u", f);
        int val = rand();
        mamaMsg_addU64 (msg, buf, f, val);
        values.insert(std::pair<mama_fid_t, uint64_t>(f, val));
    }

    /* Build the MAMA Dictionary from our test message. */
    mamaDictionary_create (&dict);
    mamaDictionary_buildDictionaryFromMessage (dict, msg);

    /* Create the message iterator */
    mamaMsgIterator_create (&iterator, dict);
    mamaMsgIterator_associate (iterator, msg);
}

void MsgNewIteratorTestC::TearDown(void)
{
    /* Cleanup the memory. */
    mamaMsgIterator_destroy (iterator);
    mamaDictionary_destroy  (dict);
    mamaMsg_destroy (msg);

    mama_close();
}

/* Description:      Attempt to create a valid iterator
 *
 * Expected Result:  MAMA_STATUS_OK
 */
TEST_F (MsgNewIteratorTestC, IteratorCreate)
{
    mama_status status = MAMA_STATUS_OK;
    mamaMsgIterator createIter = NULL;
    status = mamaMsgIterator_create (&createIter, dict);

    ASSERT_EQ (MAMA_STATUS_OK, status);

    mamaMsgIterator_destroy (createIter);
}

/* Description:      Attempt to create an iterator with a NULL iterator.
 *
 * Expected Result:  MAMA_STATUS_NULL_ARG
 */
TEST_F (MsgNewIteratorTestC, DISABLED_IteratorCreateNullIter)
{
    mama_status status = MAMA_STATUS_OK;
    status = mamaMsgIterator_create (NULL, dict);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, status);
}

/* Description:      Attempt to create an iterator with a NULL dict.
 *
 * Expected Result:  MAMA_STATUS_NULL_ARG
 */
TEST_F (MsgNewIteratorTestC, IteratorCreateNullDict)
{
    mama_status status = MAMA_STATUS_OK;
    mamaMsgIterator createIter = NULL;
    status = mamaMsgIterator_create (&createIter, NULL);

    ASSERT_EQ (MAMA_STATUS_OK, status);

    mamaMsgIterator_destroy (createIter);
}

/* Description:      Attempt to destroy a valid iterator
 *
 * Expected Result:  MAMA_STATUS_OK
 */
TEST_F (MsgNewIteratorTestC, IteratorDestroy)
{
    mama_status status = MAMA_STATUS_OK;
    mamaMsgIterator destroyableIter = NULL;
    status = mamaMsgIterator_create (&destroyableIter, dict);
    ASSERT_EQ (MAMA_STATUS_OK, status);

    status = mamaMsgIterator_destroy (destroyableIter);
    ASSERT_EQ (MAMA_STATUS_OK, status);
}

/* Description:      Attempt to destroy an invalid iterator
 *
 * Expected Result:  MAMA_STATUS_NULL_ARG
 */
TEST_F (MsgNewIteratorTestC, IteratorDestroyNullIter)
{
    mama_status status = MAMA_STATUS_OK;
    status = mamaMsgIterator_destroy (NULL);
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, status);
}

/* Description:      Attempt to associate an iterator with a NULL message.
 *
 * Expected Result:  MAMA_STATUS_OK
 */
TEST_F (MsgNewIteratorTestC, IteratorAssociate)
{
    mama_status status = MAMA_STATUS_OK;
    status = mamaMsgIterator_associate (iterator, msg);

    ASSERT_EQ (MAMA_STATUS_OK, status);
}

/* Description:      Attempt to associate a NULL iterator with a valid message.
 *
 * Expected Result:  MAMA_STATUS_NULL_ARG
 */
TEST_F (MsgNewIteratorTestC, IteratorAssociateNullIter)
{
    mama_status status = MAMA_STATUS_OK;
    status = mamaMsgIterator_associate (NULL, msg);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, status);
}

/* Description:      Attempt to associate an iterator with a NULL message.
 *
 * Expected Result:  MAMA_STATUS_NULL_ARG
 */
TEST_F (MsgNewIteratorTestC, IteratorAssociateNullMsg)
{
    mama_status status = MAMA_STATUS_OK;
    status = mamaMsgIterator_associate (iterator, NULL);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, status);
}

/*  Description:     Call begin on the already associated mamaMsgIterator, and
 *                   check that the contents of the first field are returned.
 *
 *  Expected Result: Fid should be 101, value should be 8.
 */
TEST_F (MsgNewIteratorTestC, IteratorBegin)
{
    mama_fid_t      fid      = 0;
    mama_u64_t      content  = 0;

    field = mamaMsgIterator_begin (iterator);

    mamaMsgField_getFid (field, &fid);
    mamaMsgField_getU64 (field, &content);

    /* Check the contents of the field: */
    ASSERT_EQ(values.at(fid), content);
}

/*  Description:     Check that when passed a NULL iterator, begin returns a NULL
 *                   field.
 *
 *  Expected Result: NULL field.
 */
TEST_F (MsgNewIteratorTestC, DISABLED_IteratorBeginNullIter)
{
    field = mamaMsgIterator_begin (NULL);

    /* Check the contents of the field: */
    ASSERT_EQ (NULL, field);
}

/*  Description:     Call begin, check that the first field is returned, then
 *                   call next, and check that the first field is returned again,
 *                   then call next again, until all fields are confirmed.
 */
TEST_F (MsgNewIteratorTestC, IteratorBeginNext)
{
    mama_fid_t      fid      = 0;
    mama_u64_t      content  = 0;

    field = mamaMsgIterator_begin (iterator);

    mamaMsgField_getFid (field, &fid);
    mamaMsgField_getU64 (field, &content);

    /* Check the contents of the field match legal entry */
    ASSERT_EQ(values.at(fid), content);

    field = mamaMsgIterator_next (iterator);

    mamaMsgField_getFid (field, &fid);

    /* Ensure we return the first field again: */
    ASSERT_EQ(values[fid], content);

    /* Remove from reference map */
    values.erase(fid);

    /* For the 4 remaining fields, check contents */
    for (int i = 0; i < 4; i++)
    {
        field = mamaMsgIterator_next (iterator);
        mamaMsgField_getFid (field, &fid);
        mamaMsgField_getU64 (field, &content);
        ASSERT_EQ(values[fid], content);
        /* Remove from reference map */
        values.erase(fid);
    }

    /* Gotta catch 'em all */
    EXPECT_EQ(0, values.size());
}

/*  Description:     Step into the message, determine if it has a next value, 
 *                   retrieve and check the contents of that value.
 *
 *  Expected Result: Non-zero return for hasNext
 */
TEST_F (MsgNewIteratorTestC, IteratorHasNext)
{
    mama_fid_t      fid      = 0;
    mama_u64_t      content  = 0;
    mama_bool_t     hasNext  = 0;

    field = mamaMsgIterator_begin (iterator);

    mamaMsgField_getFid (field, &fid);
    mamaMsgField_getU64 (field, &content);

    /* Check the contents of the field: */
    ASSERT_EQ(values[fid], content);

    field = mamaMsgIterator_next (iterator);

    /* Ensure we return the first field again: */
    ASSERT_EQ(values[fid], content);

    /* Check if we have a next value: */
    hasNext = mamaMsgIterator_hasNext (iterator);

    ASSERT_NE (0, hasNext);

    /* Get the next value */
    field = mamaMsgIterator_next (iterator);
    mamaMsgField_getFid (field, &fid);
    mamaMsgField_getU64 (field, &content);

    /* Ensure we return the first field again: */
    ASSERT_EQ(values[fid], content);
}

/*  Description:     Step to the end of the message, check if it has a next,
 *                   expecting that it does not.
 *
 *  Expected Result: Zero return for hasNext
 */
TEST_F (MsgNewIteratorTestC, IteratorHasNoNext)
{
    mama_fid_t      fid      = 0;
    mama_u64_t      content  = 0;

    field = mamaMsgIterator_begin (iterator);

    mamaMsgField_getFid (field, &fid);
    mamaMsgField_getU64 (field, &content);

    /* Check the contents of the field: */
    ASSERT_EQ(values[fid], content);

    field = mamaMsgIterator_next (iterator);

    /* Ensure we return the first field again: */
    ASSERT_EQ(values[fid], content);

    /* Move to the last message: */
    field = mamaMsgIterator_next (iterator);
    field = mamaMsgIterator_next (iterator);
    field = mamaMsgIterator_next (iterator);
    field = mamaMsgIterator_next (iterator);

    /* Move past last message - should not crash */
    field = mamaMsgIterator_next (iterator);
    ASSERT_EQ(NULL, field);

    /* Check if we have a next value: */
    ASSERT_EQ (0, mamaMsgIterator_hasNext (iterator));
}

/*  Description:     Attempt to check hasNext for a NULL iterator.
 *
 *  Expected Result: Zero return for hasNext
 */
TEST_F (MsgNewIteratorTestC, DISABLED_IteratorHasNextNullIter)
{
    mama_bool_t hasNext = 0;

    /* Check if we have a next value: */
    hasNext = mamaMsgIterator_hasNext (NULL);

    ASSERT_EQ (0, hasNext);
}

/*  Description:     Attempt to set a valid dictionary for a valid iterator.
 *
 *  Expected Result: MAMA_STATUS_OK
 */
TEST_F (MsgNewIteratorTestC, IteratorSetDict)
{
    mama_status status = MAMA_STATUS_OK;

    status = mamaMsgIterator_setDict (iterator, dict);

    ASSERT_EQ (MAMA_STATUS_OK, status);
}

/*  Description:     Attempt to set a NULL dictionary for a valid iterator.
 *
 *  Expected Result: MAMA_STATUS_OK
 */
TEST_F (MsgNewIteratorTestC, IteratorSetDictNullDict)
{
    mama_status status = MAMA_STATUS_OK;

    status = mamaMsgIterator_setDict (iterator, NULL);

    ASSERT_EQ (MAMA_STATUS_OK, status);
}

/*  Description:     Attempt to set a valid dictionary for a NULL iterator.
 *
 *  Expected Result: MAMA_STATUS_NULL_ARG
 */
TEST_F (MsgNewIteratorTestC, IteratorSetDictNullIter)
{
    mama_status status = MAMA_STATUS_OK;

    status = mamaMsgIterator_setDict (NULL, dict);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, status);
}

/*  Description:     Attempt to set a NULL dictionary for a NULL iterator.
 *
 *  Expected Result: MAMA_STATUS_NULL_ARG 
 */
TEST_F (MsgNewIteratorTestC, IteratorSetDictNullIterNullDict)
{
    mama_status status = MAMA_STATUS_OK;

    status = mamaMsgIterator_setDict (NULL, NULL);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, status);
}
