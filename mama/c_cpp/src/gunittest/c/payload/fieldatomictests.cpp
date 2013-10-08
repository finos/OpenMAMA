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
#include "mama/mama.h"
#include "MainUnitTestC.h"
#include <iostream>
#include "payloadbridge.h"

using std::cout;
using std::endl;

class FieldAtomicTestsC : public ::testing::Test
{
protected:
    FieldAtomicTestsC(void);
    virtual ~FieldAtomicTestsC(void);

    virtual void SetUp(void);
    virtual void TearDown(void);

     mamaPayloadBridge aBridge;
    
};

FieldAtomicTestsC::FieldAtomicTestsC(void)
    : aBridge (NULL)
{
}

FieldAtomicTestsC::~FieldAtomicTestsC(void)
{
}

void FieldAtomicTestsC::SetUp(void)
{
    mama_loadPayloadBridge(&aBridge,getPayload());
}

void FieldAtomicTestsC::TearDown(void)
{
}

/**********************************************************************
 * Bool Tests
 **********************************************************************/
class FieldBoolTests : public FieldAtomicTestsC
{
protected:

    mama_bool_t m_in, m_update, m_out;

    FieldBoolTests()
        : m_in(true)
        , m_update(false)
        , m_out(false)
    {
    }

};

TEST_F(FieldBoolTests, DISABLED_UpdateBoolValid)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddBool(msg, NULL, 1, m_in);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadUpdateBool(field, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_OK);

    ASSERT_EQ(ret, MAMA_STATUS_OK);
}

TEST_F(FieldBoolTests, DISABLED_UpdateBoolInvalidType)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddChar(msg, NULL, 1, 'A');
    ASSERT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadUpdateBool(field, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_WRONG_FIELD_TYPE);

    ASSERT_EQ(ret, MAMA_STATUS_OK);
}

TEST_F(FieldBoolTests, DISABLED_UpdateBoolInValid)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgFieldPayloadCreate(&field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    /*Null Field*/
    ret = aBridge->msgFieldPayloadUpdateBool(NULL, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_NULL_ARG);

    /*Null Underlying field*/
    ret = aBridge->msgFieldPayloadUpdateBool(field, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_NULL_ARG);

    ret = aBridge->msgPayloadDestroy(field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
}

TEST_F(FieldBoolTests, DISABLED_GetBoolValid)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddBool(msg, NULL, 1, m_in);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadGetBool(field, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_OK);

    ASSERT_EQ(ret, MAMA_STATUS_OK);
}

TEST_F(FieldBoolTests, DISABLED_GetBoolInvalidType)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddString(msg, NULL, 1, "FRED");
    ASSERT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadGetBool(field, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_INVALID_ARG);

    ASSERT_EQ(ret, MAMA_STATUS_OK);
}

TEST_F(FieldBoolTests, DISABLED_GetBoolInValid)
{
    msgFieldPayload     field = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgFieldPayloadCreate(&field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    /*Null Field*/
    ret = aBridge->msgFieldPayloadGetBool(NULL, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_NULL_ARG);

    /*Null Underlying field*/
    ret = aBridge->msgFieldPayloadGetBool(field, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_INVALID_ARG);
}

/**********************************************************************
 * Char Tests
 **********************************************************************/

class FieldCharTests : public FieldAtomicTestsC
{
protected:

    char m_in, m_update, m_out;

    FieldCharTests()
        : m_in('A')
        , m_update('B')
        , m_out('\0')
    {
    }

};

TEST_F(FieldCharTests, UpdateCharValid)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddChar(msg, NULL, 1, m_in);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadUpdateChar(field, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_OK);
}

TEST_F(FieldCharTests, UpdateCharInvalidType)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddBool(msg, NULL, 1, 'A');
    ASSERT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadUpdateChar(field, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_WRONG_FIELD_TYPE);
}

TEST_F(FieldCharTests, UpdateCharInValid)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgFieldPayloadCreate(&field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    /*Null Field*/
    ret = aBridge->msgFieldPayloadUpdateChar(NULL, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_NULL_ARG);

    /*Null Underlying field*/
    ret = aBridge->msgFieldPayloadUpdateChar(field, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_NULL_ARG);
}

TEST_F(FieldCharTests, GetCharValid)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddChar(msg, NULL, 1, m_in);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadGetChar(field, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_OK);
}

TEST_F(FieldCharTests, GetCharInvalidType)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddString(msg, NULL, 1, "FRED");
    ASSERT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadGetChar(field, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_INVALID_ARG);
}

TEST_F(FieldCharTests, GetCharInValid)
{
    msgFieldPayload     field = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgFieldPayloadCreate(&field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    /*Null Field*/
    ret = aBridge->msgFieldPayloadGetChar(NULL, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_NULL_ARG);

    /*Null Underlying field*/
    ret = aBridge->msgFieldPayloadGetChar(field, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_INVALID_ARG);
}

/**********************************************************************
 * I8 Tests
 **********************************************************************/

class FieldI8Tests : public FieldAtomicTestsC
{
protected:

    mama_i8_t m_in, m_update, m_out;

    FieldI8Tests()
        : m_in(3)
        , m_update(4)
        , m_out(0)
    {
    }

};

TEST_F(FieldI8Tests, UpdateI8Valid)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddI8(msg, NULL, 1, m_in);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadUpdateI8(field, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_OK);
}

TEST_F(FieldI8Tests, UpdateI8InvalidType)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddBool(msg, NULL, 1, 'A');
    ASSERT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadUpdateI8(field, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_WRONG_FIELD_TYPE);
}

TEST_F(FieldI8Tests, UpdateI8InValid)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgFieldPayloadCreate(&field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    /*Null Field*/
    ret = aBridge->msgFieldPayloadUpdateI8(NULL, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_NULL_ARG);

    /*Null Underlying field*/
    ret = aBridge->msgFieldPayloadUpdateI8(field, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_NULL_ARG);
}

TEST_F(FieldI8Tests, GetI8Valid)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddI8(msg, NULL, 1, m_in);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadGetI8(field, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_OK);
}

TEST_F(FieldI8Tests, GetI8InvalidType)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddString(msg, NULL, 1, "FRED");
    ASSERT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadGetI8(field, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_INVALID_ARG);
}

TEST_F(FieldI8Tests, GetI8InValid)
{
    msgFieldPayload     field = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgFieldPayloadCreate(&field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    /*Null Field*/
    ret = aBridge->msgFieldPayloadGetI8(NULL, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_NULL_ARG);

    /*Null Underlying field*/
    ret = aBridge->msgFieldPayloadGetI8(field, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_INVALID_ARG);
}

/**********************************************************************
 * U8 Tests
 **********************************************************************/

class FieldU8Tests : public FieldAtomicTestsC
{
protected:

    mama_u8_t m_in, m_update, m_out;

    FieldU8Tests()
        : m_in(3)
        , m_update(4)
        , m_out(0)
    {
    }

};

TEST_F(FieldU8Tests, UpdateU8Valid)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddU8(msg, NULL, 1, m_in);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadUpdateU8(field, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_OK);
}

TEST_F(FieldU8Tests, UpdateU8InvalidType)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddBool(msg, NULL, 1, 'A');
    ASSERT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadUpdateU8(field, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_WRONG_FIELD_TYPE);
}

TEST_F(FieldU8Tests, UpdateU8InValid)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgFieldPayloadCreate(&field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    /*Null Field*/
    ret = aBridge->msgFieldPayloadUpdateU8(NULL, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_NULL_ARG);

    /*Null Underlying field*/
    ret = aBridge->msgFieldPayloadUpdateU8(field, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_NULL_ARG);
}

TEST_F(FieldU8Tests, GetU8Valid)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddU8(msg, NULL, 1, m_in);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadGetU8(field, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_OK);
}

TEST_F(FieldU8Tests, GetU8InvalidType)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddString(msg, NULL, 1, "FRED");
    ASSERT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadGetU8(field, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_INVALID_ARG);
}

TEST_F(FieldU8Tests, GetU8InValid)
{
    msgFieldPayload     field = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgFieldPayloadCreate(&field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    /*Null Field*/
    ret = aBridge->msgFieldPayloadGetU8(NULL, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_NULL_ARG);

    /*Null Underlying field*/
    ret = aBridge->msgFieldPayloadGetU8(field, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_INVALID_ARG);
}

/**********************************************************************
 * I16 Tests
 **********************************************************************/

class FieldI16Tests : public FieldAtomicTestsC
{
protected:

    mama_i16_t m_in, m_update, m_out;

    FieldI16Tests()
        : m_in(3)
        , m_update(4)
        , m_out(0)
    {
    }

};

TEST_F(FieldI16Tests, UpdateI16Valid)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddI16(msg, NULL, 1, m_in);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadUpdateI16(field, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_OK);
}

TEST_F(FieldI16Tests, UpdateI16InvalidType)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddBool(msg, NULL, 1, 'A');
    ASSERT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadUpdateI16(field, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_WRONG_FIELD_TYPE);
}

TEST_F(FieldI16Tests, UpdateI16InValid)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgFieldPayloadCreate(&field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    /*Null Field*/
    ret = aBridge->msgFieldPayloadUpdateI16(NULL, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_NULL_ARG);

    /*Null Underlying field*/
    ret = aBridge->msgFieldPayloadUpdateI16(field, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_NULL_ARG);
}

TEST_F(FieldI16Tests, GetI16Valid)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddI16(msg, NULL, 1, m_in);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadGetI16(field, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_OK);
}

TEST_F(FieldI16Tests, GetI16InvalidType)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddString(msg, NULL, 1, "FRED");
    ASSERT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadGetI16(field, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_INVALID_ARG);
}

TEST_F(FieldI16Tests, GetI16InValid)
{
    msgFieldPayload     field = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgFieldPayloadCreate(&field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    /*Null Field*/
    ret = aBridge->msgFieldPayloadGetI16(NULL, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_NULL_ARG);

    /*Null Underlying field*/
    ret = aBridge->msgFieldPayloadGetI16(field, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_INVALID_ARG);
}

/**********************************************************************
 * U16 Tests
 **********************************************************************/

class FieldU16Tests : public FieldAtomicTestsC
{
protected:

    mama_u16_t m_in, m_update, m_out;

    FieldU16Tests()
        : m_in(3)
        , m_update(4)
        , m_out(0)
    {
    }

};

TEST_F(FieldU16Tests, UpdateU16Valid)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddU16(msg, NULL, 1, m_in);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadUpdateU16(field, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_OK);
}

TEST_F(FieldU16Tests, UpdateU16InvalidType)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddBool(msg, NULL, 1, 'A');
    ASSERT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadUpdateU16(field, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_WRONG_FIELD_TYPE);
}

TEST_F(FieldU16Tests, UpdateU16InValid)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgFieldPayloadCreate(&field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    /*Null Field*/
    ret = aBridge->msgFieldPayloadUpdateU16(NULL, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_NULL_ARG);

    /*Null Underlying field*/
    ret = aBridge->msgFieldPayloadUpdateU16(field, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_NULL_ARG);
}

TEST_F(FieldU16Tests, GetU16Valid)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddU16(msg, NULL, 1, m_in);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadGetU16(field, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_OK);
}

TEST_F(FieldU16Tests, GetU16InvalidType)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddString(msg, NULL, 1, "FRED");
    ASSERT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadGetU16(field, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_INVALID_ARG);
}

TEST_F(FieldU16Tests, GetU16InValid)
{
    msgFieldPayload     field = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgFieldPayloadCreate(&field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    /*Null Field*/
    ret = aBridge->msgFieldPayloadGetU16(NULL, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_NULL_ARG);

    /*Null Underlying field*/
    ret = aBridge->msgFieldPayloadGetU16(field, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_INVALID_ARG);
}

/**********************************************************************
 * I32 Tests
 **********************************************************************/

class FieldI32Tests : public FieldAtomicTestsC
{
protected:

    mama_i32_t m_in, m_update, m_out;

    FieldI32Tests()
        : m_in(3)
        , m_update(4)
        , m_out(0)
    {
    }

};

TEST_F(FieldI32Tests, UpdateI32Valid)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddI32(msg, NULL, 1, m_in);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadUpdateI32(field, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_OK);
}

TEST_F(FieldI32Tests, UpdateI32InvalidType)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddBool(msg, NULL, 1, 'A');
    ASSERT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadUpdateI32(field, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_WRONG_FIELD_TYPE);
}

TEST_F(FieldI32Tests, UpdateI32InValid)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgFieldPayloadCreate(&field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    /*Null Field*/
    ret = aBridge->msgFieldPayloadUpdateI32(NULL, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_NULL_ARG);

    /*Null Underlying field*/
    ret = aBridge->msgFieldPayloadUpdateI32(field, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_NULL_ARG);
}

TEST_F(FieldI32Tests, GetI32Valid)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddI32(msg, NULL, 1, m_in);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadGetI32(field, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_OK);
}

TEST_F(FieldI32Tests, GetI32InvalidType)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddString(msg, NULL, 1, "FRED");
    ASSERT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadGetI32(field, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_INVALID_ARG);
}

TEST_F(FieldI32Tests, GetI32InValid)
{
    msgFieldPayload     field = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgFieldPayloadCreate(&field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    /*Null Field*/
    ret = aBridge->msgFieldPayloadGetI32(NULL, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_NULL_ARG);

    /*Null Underlying field*/
    ret = aBridge->msgFieldPayloadGetI32(field, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_INVALID_ARG);
}

/**********************************************************************
 * U32 Tests
 **********************************************************************/

class FieldU32Tests : public FieldAtomicTestsC
{
protected:

    mama_u32_t m_in, m_update, m_out;

    FieldU32Tests()
        : m_in(3)
        , m_update(4)
        , m_out(0)
    {
    }

};

TEST_F(FieldU32Tests, UpdateU32Valid)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddU32(msg, NULL, 1, m_in);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadUpdateU32(field, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_OK);
}

TEST_F(FieldU32Tests, UpdateU32InvalidType)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddBool(msg, NULL, 1, 'A');
    ASSERT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadUpdateU32(field, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_WRONG_FIELD_TYPE);
}

TEST_F(FieldU32Tests, UpdateU32InValid)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgFieldPayloadCreate(&field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    /*Null Field*/
    ret = aBridge->msgFieldPayloadUpdateU32(NULL, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_NULL_ARG);

    /*Null Underlying field*/
    ret = aBridge->msgFieldPayloadUpdateU32(field, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_NULL_ARG);
}

TEST_F(FieldU32Tests, GetU32Valid)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddU32(msg, NULL, 1, m_in);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadGetU32(field, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_OK);
}

TEST_F(FieldU32Tests, GetU32InvalidType)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddString(msg, NULL, 1, "FRED");
    ASSERT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadGetU32(field, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_INVALID_ARG);
}

TEST_F(FieldU32Tests, GetU32InValid)
{
    msgFieldPayload     field = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgFieldPayloadCreate(&field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    /*Null Field*/
    ret = aBridge->msgFieldPayloadGetU32(NULL, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_NULL_ARG);

    /*Null Underlying field*/
    ret = aBridge->msgFieldPayloadGetU32(field, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_INVALID_ARG);
}

/**********************************************************************
 * I64 Tests
 **********************************************************************/

class FieldI64Tests : public FieldAtomicTestsC
{
protected:

    mama_i64_t m_in, m_update, m_out;

    FieldI64Tests()
        : m_in(3)
        , m_update(4)
        , m_out(0)
    {
    }

};

TEST_F(FieldI64Tests, UpdateI64Valid)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddI64(msg, NULL, 1, m_in);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadUpdateI64(field, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_OK);
}

TEST_F(FieldI64Tests, UpdateI64InvalidType)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddBool(msg, NULL, 1, 'A');
    ASSERT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadUpdateI64(field, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_WRONG_FIELD_TYPE);
}

TEST_F(FieldI64Tests, UpdateI64InValid)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgFieldPayloadCreate(&field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    /*Null Field*/
    ret = aBridge->msgFieldPayloadUpdateI64(NULL, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_NULL_ARG);

    /*Null Underlying field*/
    ret = aBridge->msgFieldPayloadUpdateI64(field, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_NULL_ARG);
}

TEST_F(FieldI64Tests, GetI64Valid)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddI64(msg, NULL, 1, m_in);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadGetI64(field, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_OK);
}

TEST_F(FieldI64Tests, GetI64InvalidType)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddString(msg, NULL, 1, "FRED");
    ASSERT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadGetI64(field, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_INVALID_ARG);
}

TEST_F(FieldI64Tests, GetI64InValid)
{
    msgFieldPayload     field = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgFieldPayloadCreate(&field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    /*Null Field*/
    ret = aBridge->msgFieldPayloadGetI64(NULL, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_NULL_ARG);

    /*Null Underlying field*/
    ret = aBridge->msgFieldPayloadGetI64(field, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_INVALID_ARG);
}

/**********************************************************************
 * U64 Tests
 **********************************************************************/

class FieldU64Tests : public FieldAtomicTestsC
{
protected:

    mama_u64_t m_in, m_update, m_out;

    FieldU64Tests()
        : m_in(3)
        , m_update(4)
        , m_out(0)
    {
    }

};

TEST_F(FieldU64Tests, UpdateU64Valid)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddU64(msg, NULL, 1, m_in);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadUpdateU64(field, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_OK);
}

TEST_F(FieldU64Tests, UpdateU64InvalidType)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddBool(msg, NULL, 1, 'A');
    ASSERT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadUpdateU64(field, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_WRONG_FIELD_TYPE);
}

TEST_F(FieldU64Tests, UpdateU64InValid)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgFieldPayloadCreate(&field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    /*Null Field*/
    ret = aBridge->msgFieldPayloadUpdateU64(NULL, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_NULL_ARG);

    /*Null Underlying field*/
    ret = aBridge->msgFieldPayloadUpdateU64(field, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_NULL_ARG);
}

TEST_F(FieldU64Tests, GetU64Valid)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddU64(msg, NULL, 1, m_in);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadGetU64(field, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_OK);
}

TEST_F(FieldU64Tests, GetU64InvalidType)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddString(msg, NULL, 1, "FRED");
    ASSERT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadGetU64(field, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_INVALID_ARG);
}

TEST_F(FieldU64Tests, GetU64InValid)
{
    msgFieldPayload     field = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgFieldPayloadCreate(&field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    /*Null Field*/
    ret = aBridge->msgFieldPayloadGetU64(NULL, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_NULL_ARG);

    /*Null Underlying field*/
    ret = aBridge->msgFieldPayloadGetU64(field, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_INVALID_ARG);
}
/**********************************************************************
 * F32 Tests
 **********************************************************************/

class FieldF32Tests : public FieldAtomicTestsC
{
protected:

    mama_f32_t m_in, m_update, m_out;

    FieldF32Tests()
        : m_in(12.00)
        , m_update(13.00)
        , m_out(0.00)
    {
    }

};

TEST_F(FieldF32Tests, UpdateF32Valid)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddF32(msg, NULL, 1, m_in);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadUpdateF32(field, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_OK);
}

TEST_F(FieldF32Tests, UpdateF32InvalidType)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddBool(msg, NULL, 1, 'A');
    ASSERT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadUpdateF32(field, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_WRONG_FIELD_TYPE);
}

TEST_F(FieldF32Tests, UpdateF32InValid)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgFieldPayloadCreate(&field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    /*Null Field*/
    ret = aBridge->msgFieldPayloadUpdateF32(NULL, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_NULL_ARG);

    /*Null Underlying field*/
    ret = aBridge->msgFieldPayloadUpdateF32(field, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_NULL_ARG);
}

TEST_F(FieldF32Tests, GetF32Valid)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddF32(msg, NULL, 1, m_in);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadGetF32(field, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_OK);
}

TEST_F(FieldF32Tests, GetF32InvalidType)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddString(msg, NULL, 1, "FRED");
    ASSERT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadGetF32(field, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_INVALID_ARG);
}

TEST_F(FieldF32Tests, GetF32InValid)
{
    msgFieldPayload     field = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgFieldPayloadCreate(&field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    /*Null Field*/
    ret = aBridge->msgFieldPayloadGetF32(NULL, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_NULL_ARG);

    /*Null Underlying field*/
    ret = aBridge->msgFieldPayloadGetF32(field, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_INVALID_ARG);
}

/**********************************************************************
 * F64 Tests
 **********************************************************************/

class FieldF64Tests : public FieldAtomicTestsC
{
protected:

    mama_f64_t m_in, m_update, m_out;

    FieldF64Tests()
        : m_in(3.123)
        , m_update(4.456)
        , m_out(0.00)
    {
    }

};

TEST_F(FieldF64Tests, UpdateF64Valid)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddF64(msg, NULL, 1, m_in);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadUpdateF64(field, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_OK);
}

TEST_F(FieldF64Tests, UpdateF64InvalidType)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddBool(msg, NULL, 1, 'A');
    ASSERT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadUpdateF64(field, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_WRONG_FIELD_TYPE);
}

TEST_F(FieldF64Tests, UpdateF64InValid)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgFieldPayloadCreate(&field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    /*Null Field*/
    ret = aBridge->msgFieldPayloadUpdateF64(NULL, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_NULL_ARG);

    /*Null Underlying field*/
    ret = aBridge->msgFieldPayloadUpdateF64(field, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_NULL_ARG);
}

TEST_F(FieldF64Tests, GetF64Valid)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddF64(msg, NULL, 1, m_in);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadGetF64(field, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_OK);
}

TEST_F(FieldF64Tests, GetF64InvalidType)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    ASSERT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddString(msg, NULL, 1, "FRED");
    ASSERT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadGetF64(field, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_INVALID_ARG);
}

TEST_F(FieldF64Tests, GetF64InValid)
{
    msgFieldPayload     field = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgFieldPayloadCreate(&field);
    ASSERT_EQ(ret, MAMA_STATUS_OK);

    /*Null Field*/
    ret = aBridge->msgFieldPayloadGetF64(NULL, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_NULL_ARG);

    /*Null Underlying field*/
    ret = aBridge->msgFieldPayloadGetF64(field, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_INVALID_ARG);
}
