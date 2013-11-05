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
#include <string.h>
#include "payloadbridge.h"

using std::cout;
using std::endl;

class FieldCompositeTestsC : public ::testing::Test
{
protected:
    FieldCompositeTestsC(void);
    virtual ~FieldCompositeTestsC(void);

    virtual void SetUp(void);
    virtual void TearDown(void);

     mamaPayloadBridge aBridge;

};

FieldCompositeTestsC::FieldCompositeTestsC(void)
    : aBridge (NULL)
{
}

FieldCompositeTestsC::~FieldCompositeTestsC(void)
{
}

void FieldCompositeTestsC::SetUp(void)
{
    mama_loadPayloadBridge(&aBridge,getPayload());
}

void FieldCompositeTestsC::TearDown(void)
{
}

/**********************************************************************
 * String Tests
 **********************************************************************/
class FieldStringTests : public FieldCompositeTestsC
{
protected:

    char* m_in;
    char* m_update;
    const char* m_out;

    FieldStringTests()
        : m_out(NULL)
    {
        m_in = strdup ("IN");
        m_update = strdup ("UPDATE");
    }

    ~FieldStringTests()
    {
        free (m_in);
        free (m_update);
    }

};

/*
 * The update string tests are disabled as the current payload Macro, 
 * INITIALIZE_PAYLOAD_BRIDGE, omits the updateString function.
 */
TEST_F(FieldStringTests, DISABLED_UpdateStringValid)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    EXPECT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddString(msg, NULL, 1, m_in);
    EXPECT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    EXPECT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadUpdateString(field, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_OK);
}

TEST_F(FieldStringTests, DISABLED_UpdateStringInvalidType)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    EXPECT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddChar(msg, NULL, 1, 'A');
    EXPECT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    EXPECT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadUpdateString(field, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_WRONG_FIELD_TYPE);
}

TEST_F(FieldStringTests, DISABLED_UpdateStringInValid)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgFieldPayloadCreate(&field);
    EXPECT_EQ(ret, MAMA_STATUS_OK);

    /*Null Field*/
    ret = aBridge->msgFieldPayloadUpdateString(NULL, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_NULL_ARG);

    /*Null Underlying field*/
    ret = aBridge->msgFieldPayloadUpdateString(field, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_NULL_ARG);
}

TEST_F(FieldStringTests, GetStringValid)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    EXPECT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddString(msg, NULL, 1, m_in);
    EXPECT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    EXPECT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadGetString(field, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_OK);
}

TEST_F(FieldStringTests, GetStringInvalidType)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    EXPECT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddChar(msg, NULL, 1, '0');
    EXPECT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    EXPECT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadGetString(field, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_INVALID_ARG);
}

TEST_F(FieldStringTests, GetStringInValid)
{
    msgFieldPayload     field = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgFieldPayloadCreate(&field);
    EXPECT_EQ(ret, MAMA_STATUS_OK);

    /*Null Field*/
    ret = aBridge->msgFieldPayloadGetString(NULL, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_NULL_ARG);

    /*Null Underlying field*/
    ret = aBridge->msgFieldPayloadGetString(field, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_INVALID_ARG);
}

/**********************************************************************
 * DateTime Tests
 **********************************************************************/
class FieldDateTimeTests : public FieldCompositeTestsC
{
protected:

    mamaDateTime m_in;
    mamaDateTime m_update;
    mamaDateTime m_out;
    char         m_str[64];

    FieldDateTimeTests()
        : m_in (NULL)
        , m_update (NULL)
        , m_out (NULL)
    {
        ::memset ((void *) m_str, 0, sizeof (m_str));

        mamaDateTime_create(&m_in);
        mamaDateTime_setToNow(m_in);
        mamaDateTime_create(&m_update);
        mamaDateTime_setToNow(m_update);
        mamaDateTime_create(&m_out);

        mamaDateTime_getAsString(m_in, m_str, 64);
    }

    ~FieldDateTimeTests()
    {
        free (m_in);
        free (m_update);
    }

};

TEST_F(FieldDateTimeTests, UpdateDateTimeValid)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    EXPECT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddDateTime(msg, NULL, 1, m_in);
    EXPECT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    EXPECT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadUpdateDateTime(field, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_OK);
}

TEST_F(FieldDateTimeTests, UpdateDateTimeInvalidType)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    EXPECT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddChar(msg, NULL, 1, 'A');
    EXPECT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    EXPECT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadUpdateDateTime(field, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_WRONG_FIELD_TYPE);
}

TEST_F(FieldDateTimeTests, UpdateDateTimeInValid)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgFieldPayloadCreate(&field);
    EXPECT_EQ(ret, MAMA_STATUS_OK);

    /*Null Field*/
    ret = aBridge->msgFieldPayloadUpdateDateTime(NULL, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_NULL_ARG);

    /*Null Underlying field*/
    ret = aBridge->msgFieldPayloadUpdateDateTime(field, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_NULL_ARG);
}

TEST_F(FieldDateTimeTests, GetDateTimeValid)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    EXPECT_EQ(ret, MAMA_STATUS_OK);
    //DateTime
    ret = aBridge->msgPayloadAddDateTime(msg, NULL, 1, m_in);
    EXPECT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    EXPECT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgFieldPayloadGetDateTime(field, m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_OK);
    //String
    ret = aBridge->msgPayloadAddString(msg, NULL, 2, m_str);
    EXPECT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 2, &field);
    EXPECT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgFieldPayloadGetDateTime(field, m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_OK);
    //F64
    ret = aBridge->msgPayloadAddF64(msg, NULL, 3, 123456.009);
    EXPECT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 3, &field);
    EXPECT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgFieldPayloadGetDateTime(field, m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_OK);
    //I64
    ret = aBridge->msgPayloadAddI64(msg, NULL, 4, 1234556);
    EXPECT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 4, &field);
    EXPECT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgFieldPayloadGetDateTime(field, m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_OK);
    //U64
    ret = aBridge->msgPayloadAddU64(msg, NULL, 5, 1234556);
    EXPECT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 5, &field);
    EXPECT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgFieldPayloadGetDateTime(field, m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_OK);





}

TEST_F(FieldDateTimeTests, GetDateTimeInvalidType)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    EXPECT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddChar(msg, NULL, 1, '0');
    EXPECT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    EXPECT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadGetDateTime(field, m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_WRONG_FIELD_TYPE);
}

TEST_F(FieldDateTimeTests, GetDateTimeInValid)
{
    msgFieldPayload     field = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgFieldPayloadCreate(&field);
    EXPECT_EQ(ret, MAMA_STATUS_OK);

    /*Null Field*/
    ret = aBridge->msgFieldPayloadGetDateTime(NULL, m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_NULL_ARG);

    /*Null Underlying field*/
    ret = aBridge->msgFieldPayloadGetDateTime(field, m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_INVALID_ARG);
}

/**********************************************************************
 * Opaque Tests
 **********************************************************************/
class FieldOpaqueTests : public FieldCompositeTestsC
{
protected:

    const void* m_in;
    mama_size_t in_size;
    const void* m_out;
    mama_size_t out_size;

    FieldOpaqueTests()
        : m_in (NULL)
        , in_size (12)
        , m_out(NULL)
        , out_size (0)
    {
        m_in = calloc (1, in_size);
    }

    ~FieldOpaqueTests()
    {
        free ((void*)m_in);
    }

};

TEST_F(FieldOpaqueTests, GetOpaqueValid)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    EXPECT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddOpaque(msg, NULL, 1, m_in, in_size);
    EXPECT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    EXPECT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadGetOpaque(field, &m_out, &out_size);
    ASSERT_EQ(ret,    MAMA_STATUS_OK);
}

TEST_F(FieldOpaqueTests, GetOpaqueInvalidType)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    EXPECT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddChar(msg, NULL, 1, '0');
    EXPECT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    EXPECT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadGetOpaque(field, &m_out, &out_size);
    ASSERT_EQ(ret,    MAMA_STATUS_INVALID_ARG);
}

TEST_F(FieldOpaqueTests, GetOpaqueInValid)
{
    msgFieldPayload     field = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgFieldPayloadCreate(&field);
    EXPECT_EQ(ret, MAMA_STATUS_OK);

    /*Null Field*/
    ret = aBridge->msgFieldPayloadGetOpaque(NULL, &m_out, &out_size);
    ASSERT_EQ(ret,    MAMA_STATUS_NULL_ARG);

    /*Null Underlying field*/
    ret = aBridge->msgFieldPayloadGetOpaque(field, &m_out, &out_size);
    ASSERT_EQ(ret,    MAMA_STATUS_INVALID_ARG);
}

/**********************************************************************
 * Price Tests
 **********************************************************************/
class FieldPriceTests : public FieldCompositeTestsC
{
protected:

    mamaPrice m_in;
    mamaPrice m_update;
    mamaPrice m_out;


    FieldPriceTests()
        : m_in (NULL)
        , m_update (NULL)
        , m_out (NULL)
    {
        mamaPrice_create(&m_in);
        mamaPrice_setValue(m_in, 12.09);
        mamaPrice_create(&m_update);
        mamaPrice_setValue(m_update, 13.45);
        mamaPrice_create(&m_out);
    }

    ~FieldPriceTests()
    {
    }

};

TEST_F(FieldPriceTests, UpdatePriceValid)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    EXPECT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddPrice(msg, NULL, 1, m_in);
    EXPECT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    EXPECT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadUpdatePrice(field, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_OK);
}

TEST_F(FieldPriceTests, UpdatePriceInvalidType)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    EXPECT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddChar(msg, NULL, 1, 'A');
    EXPECT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    EXPECT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadUpdatePrice(field, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_WRONG_FIELD_TYPE);
}

TEST_F(FieldPriceTests, UpdatePriceInValid)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgFieldPayloadCreate(&field);
    EXPECT_EQ(ret, MAMA_STATUS_OK);

    /*Null Field*/
    ret = aBridge->msgFieldPayloadUpdatePrice(NULL, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_NULL_ARG);

    /*Null Underlying field*/
    ret = aBridge->msgFieldPayloadUpdatePrice(field, msg, m_update);
    ASSERT_EQ(ret,    MAMA_STATUS_NULL_ARG);
}

TEST_F(FieldPriceTests, GetPriceValid)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    EXPECT_EQ(ret, MAMA_STATUS_OK);
    //Price
    ret = aBridge->msgPayloadAddPrice(msg, NULL, 1, m_in);
    EXPECT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    EXPECT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgFieldPayloadGetPrice(field, m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_OK);
    //F64
    ret = aBridge->msgPayloadAddF64(msg, NULL, 3, 123456.009);
    EXPECT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 3, &field);
    EXPECT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgFieldPayloadGetPrice(field, m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_OK);
}

TEST_F(FieldPriceTests, GetPriceInvalidType)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgPayloadCreate(&msg);
    EXPECT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddChar(msg, NULL, 1, '0');
    EXPECT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    EXPECT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadGetPrice(field, m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_WRONG_FIELD_TYPE);
}

TEST_F(FieldPriceTests, GetPriceInValid)
{
    msgFieldPayload     field = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    ret = aBridge->msgFieldPayloadCreate(&field);
    EXPECT_EQ(ret, MAMA_STATUS_OK);

    /*Null Field*/
    ret = aBridge->msgFieldPayloadGetPrice(NULL, m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_NULL_ARG);

    /*Null Underlying field*/
    ret = aBridge->msgFieldPayloadGetPrice(field, m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_INVALID_ARG);
}

/**********************************************************************
 * Msg Tests
 **********************************************************************/
class FieldMsgTests : public FieldCompositeTestsC
{
protected:

    msgPayload m_in;
    mama_size_t in_size;
    msgPayload m_out;
    mama_size_t out_size;

    FieldMsgTests()
        : m_in (NULL)
        , in_size (0)
        , m_out(NULL)
        , out_size (NULL)
    {}

    ~FieldMsgTests()
    {}

};

TEST_F(FieldMsgTests, GetMsgValid)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    aBridge->msgPayloadCreate(&m_in);
    ret = aBridge->msgPayloadCreate(&msg);
    EXPECT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddMsg(msg, NULL, 1, m_in);
    EXPECT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    EXPECT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadGetMsg(field, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_OK);

    aBridge->msgPayloadDestroy(m_in);
}

TEST_F(FieldMsgTests, GetMsgInvalidType)
{
    msgFieldPayload     field = NULL;
    msgPayload          msg = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    aBridge->msgPayloadCreate(&m_in);
    ret = aBridge->msgPayloadCreate(&msg);
    EXPECT_EQ(ret, MAMA_STATUS_OK);
    ret = aBridge->msgPayloadAddChar(msg, NULL, 1, '0');
    EXPECT_EQ(ret, MAMA_STATUS_OK);
     ret = aBridge->msgPayloadGetField(msg, NULL, 1, &field);
    EXPECT_EQ(ret, MAMA_STATUS_OK);

    ret = aBridge->msgFieldPayloadGetMsg(field, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_WRONG_FIELD_TYPE);

    aBridge->msgPayloadDestroy(m_in);
}

TEST_F(FieldMsgTests, GetMsgInValid)
{
    msgFieldPayload     field = NULL;
    mama_status         ret = MAMA_STATUS_OK;

    aBridge->msgPayloadCreate(&m_in);

    ret = aBridge->msgFieldPayloadCreate(&field);
    EXPECT_EQ(ret, MAMA_STATUS_OK);

    /*Null Field*/
    ret = aBridge->msgFieldPayloadGetMsg(NULL, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_NULL_ARG);

    /*Null Underlying field*/
    ret = aBridge->msgFieldPayloadGetMsg(field, &m_out);
    ASSERT_EQ(ret,    MAMA_STATUS_INVALID_ARG);

    aBridge->msgPayloadDestroy(m_in);
}
