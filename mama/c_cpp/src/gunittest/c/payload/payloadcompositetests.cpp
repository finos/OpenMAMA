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

#include <iostream>
#include <cstring>
#include <gtest/gtest.h>
#include "mama/mama.h"
#include "payloadbridge.h"
#include "MainUnitTestC.h"


using namespace ::testing;

/**
 * Boilerplate test suite base class
 * ****************************************************************************
 */
class PayloadCompositeTests : public Test  //: public ForkedTest
{
protected:

    std::string m_middleware;
    std::string m_payload;

    // Reusables
    mamaBridge        m_middlewareBridge;
    mamaPayloadBridge m_payloadBridge;
    msgPayload        m_msg;
    mama_status       m_status;

    PayloadCompositeTests()
        : m_middleware(::getMiddleware())
        , m_payload(::getPayload())
        , m_middlewareBridge(NULL)
        , m_payloadBridge(NULL)
        , m_msg(NULL)
        , m_status(MAMA_STATUS_OK)
    {
        Test::RecordProperty("payload", m_payload.c_str());
    }

    // Common SetUp
    virtual void SetUp()
    {
        //ForkedTest::SetUp();
        mama_loadBridge(&m_middlewareBridge, m_middleware.c_str());
        mama_loadPayloadBridge(&m_payloadBridge, m_payload.c_str());
        mama_open();
        m_status = m_payloadBridge->msgPayloadCreate(&m_msg);
        ASSERT_EQ (MAMA_STATUS_OK, m_status);
    }

    // Common TearDown
    virtual void TearDown()
    {
        m_status = m_payloadBridge->msgPayloadDestroy(m_msg);
        EXPECT_EQ (MAMA_STATUS_OK, m_status);
        mama_close();

        //ForkedTest::TearDown();
    }
};

/**
 * ****************************************************************************
 * STRING TEST SUITE
 * ****************************************************************************
 */

class PayloadStringTests : public PayloadCompositeTests
{
protected:

    const char *m_in, *m_update, *m_out;

    PayloadStringTests() 
        : m_in("__TEST__")
        , m_update("__TSET__")
        , m_out(NULL)
    {
    }
    
    virtual void SetUp()
    {
        PayloadCompositeTests::SetUp();

        m_out = NULL;
    }

    virtual void TearDown()
    {
        PayloadCompositeTests::TearDown();
    }
};

/**
 * AddString test fixtures
 * ****************************************************************************
 */

TEST_F(PayloadStringTests, AddString)
{
    m_status = m_payloadBridge->msgPayloadAddString(m_msg, NULL, 1, m_in);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
}

TEST_F(PayloadStringTests, AddStringNullAdd)
{
    ASSERT_NO_FATAL_FAILURE(m_payloadBridge->msgPayloadAddString(m_msg, NULL, 1, NULL));
    m_status = m_payloadBridge->msgPayloadAddString(m_msg, NULL, 1, NULL);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadStringTests, AddStringNullMessage)
{
    m_status = m_payloadBridge->msgPayloadAddString(NULL, NULL, 1, m_in);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

/**
 * UpdateString test fixtures
 * ****************************************************************************
 */

TEST_F(PayloadStringTests, UpdateString)
{
    m_payloadBridge->msgPayloadAddString(m_msg, NULL, 1, m_in);
    m_status = m_payloadBridge->msgPayloadUpdateString(m_msg, NULL, 1, m_update);
    m_payloadBridge->msgPayloadGetString(m_msg, NULL, 1, &m_out);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_STREQ(m_out, m_update);
}

TEST_F(PayloadStringTests, UpdateStringNoAdd)
{
    m_status = m_payloadBridge->msgPayloadUpdateString(m_msg, NULL, 1, m_update);
    m_payloadBridge->msgPayloadGetString(m_msg, NULL, 1, &m_out);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_STREQ(m_out, m_update);
}

TEST_F(PayloadStringTests, UpdateStringNullUpdate)
{
    m_status = m_payloadBridge->msgPayloadUpdateString(m_msg, NULL, 1, NULL);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadStringTests, UpdateStringNullMessage)
{
    m_status = m_payloadBridge->msgPayloadUpdateString(NULL, NULL, 1, m_update);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

/**
 * GetString test fixtures
 * ****************************************************************************
 */

TEST_F(PayloadStringTests, GetString)
{
    m_payloadBridge->msgPayloadAddString(m_msg, NULL, 1, m_in);
    m_status = m_payloadBridge->msgPayloadGetString(m_msg, NULL, 1, &m_out);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_STREQ(m_out, m_in);
}

TEST_F(PayloadStringTests, GetStringNullOutput)
{
    m_payloadBridge->msgPayloadAddString(m_msg, NULL, 1, m_in);
    m_status = m_payloadBridge->msgPayloadGetString(m_msg, NULL, 1, NULL);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadStringTests, GetStringNullMessage)
{
    m_payloadBridge->msgPayloadAddString(m_msg, NULL, 1, m_in);
    m_status = m_payloadBridge->msgPayloadGetString(NULL, NULL, 1, &m_out);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

/**
 * ****************************************************************************
 * OPAQUE TEST SUITE
 * ****************************************************************************
 */

class PayloadOpaqueTests : public PayloadCompositeTests
{
protected:
    
    // Reusables
    uint32_t  m_in;
    uint32_t  m_update;
    uint32_t* m_out;

    PayloadOpaqueTests() 
        : m_in(0xFEEDBEEF)
        , m_update(0xBEEFFEED)
        , m_out(NULL)
    {
    }

    virtual void SetUp()
    {
        PayloadCompositeTests::SetUp();

        m_out = NULL;
    }

    virtual void TearDown()
    {
        PayloadCompositeTests::TearDown();
    }
};

/**
 * AddOpaque test fixtures
 * ****************************************************************************
 */

TEST_F(PayloadOpaqueTests, AddOpaque)
{
    m_status = m_payloadBridge->msgPayloadAddOpaque(m_msg, NULL, 1, (void*)&m_in, sizeof(uint32_t));
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
}

TEST_F(PayloadOpaqueTests, AddOpaqueNullAdd)
{
    m_status = m_payloadBridge->msgPayloadAddOpaque(m_msg, NULL, 1, NULL, sizeof(uint32_t));
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadOpaqueTests, AddOpaqueNullMessage)
{
    m_status = m_payloadBridge->msgPayloadAddOpaque(NULL, NULL, 1, (void*)&m_in, sizeof(uint32_t));
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

/**
 * UpdateOpaque test fixtures
 * ****************************************************************************
 */

TEST_F(PayloadOpaqueTests, UpdateOpaque)
{
    mama_size_t size;
    m_payloadBridge->msgPayloadAddOpaque(m_msg, NULL, 1, (void*)&m_in, sizeof(uint32_t));
    m_status = m_payloadBridge->msgPayloadUpdateOpaque(m_msg, NULL, 1, (void*)&m_update, sizeof(uint32_t));
    m_payloadBridge->msgPayloadGetOpaque(m_msg, NULL, 1, (const void**)&m_out, &size);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, std::memcmp((void*)&m_update, (void*)m_out, sizeof(uint32_t)));
    EXPECT_EQ (sizeof(uint32_t), size);
}

TEST_F(PayloadOpaqueTests, UpdateOpaqueNoAdd)
{
    mama_size_t size;
    m_status = m_payloadBridge->msgPayloadUpdateOpaque(m_msg, NULL, 1, (void*)&m_update, sizeof(uint32_t));
    m_payloadBridge->msgPayloadGetOpaque(m_msg, NULL, 1, (const void**)&m_out, &size);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, std::memcmp((void*)&m_update, (void*)m_out, sizeof(uint32_t)));
    EXPECT_EQ (sizeof(uint32_t), size);
}

TEST_F(PayloadOpaqueTests, UpdateOpaqueNullUpdate)
{
    m_payloadBridge->msgPayloadAddOpaque(m_msg, NULL, 1, (void*)&m_in, sizeof(uint32_t));
    m_status = m_payloadBridge->msgPayloadUpdateOpaque(m_msg, NULL, 1, NULL, sizeof(uint32_t));
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadOpaqueTests, UpdateOpaqueNullMessage)
{
    m_payloadBridge->msgPayloadAddOpaque(m_msg, NULL, 1, (void*)&m_in, sizeof(uint32_t));
    m_status = m_payloadBridge->msgPayloadUpdateOpaque(NULL, NULL, 1, (void*)&m_update, sizeof(uint32_t));
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

/**
 * GetOpaque test fixtures
 * ****************************************************************************
 */

TEST_F(PayloadOpaqueTests, GetOpaque)
{
    mama_size_t size (0);
    m_payloadBridge->msgPayloadAddOpaque(m_msg, NULL, 1, (void*)&m_in, sizeof(uint32_t));
    m_status = m_payloadBridge->msgPayloadGetOpaque(m_msg, NULL, 1, (const void**)&m_out, &size);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, std::memcmp((void*)&m_in, (void*)m_out, sizeof(uint32_t)));
    EXPECT_EQ (sizeof(uint32_t), size);
}

TEST_F(PayloadOpaqueTests, GetOpaqueNullOutput)
{
    mama_size_t size (0);
    m_payloadBridge->msgPayloadAddOpaque(m_msg, NULL, 1, (void*)&m_in, sizeof(uint32_t));
    m_status = m_payloadBridge->msgPayloadGetOpaque(m_msg, NULL, 1, NULL, &size);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadOpaqueTests, GetOpaqueNullMessage)
{
    mama_size_t size (0);
    m_payloadBridge->msgPayloadAddOpaque(m_msg, NULL, 1, (void*)&m_in, sizeof(uint32_t));
    m_status = m_payloadBridge->msgPayloadAddOpaque(NULL, NULL, 1, (void*)&m_in, size);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

/**
 * ****************************************************************************
 * DATETIME TEST SUITE
 * ****************************************************************************
 */

class PayloadDateTimeTests : public PayloadCompositeTests
{
protected:

    // Reusables
    mamaDateTime m_in, m_update, m_out;

    PayloadDateTimeTests() 
        : m_in(NULL)
        , m_update(NULL)
        , m_out(NULL)
    {
    }

    virtual void SetUp()
    {
        PayloadCompositeTests::SetUp();

        mamaDateTime_create(&m_in);
        mamaDateTime_create(&m_update);
        mamaDateTime_create(&m_out);

        mamaDateTime_setTime (m_in, 12, 35, 40, 30);
        mamaDateTime_setTime (m_update, 13, 59, 1, 35);
    }

    virtual void TearDown()
    {
        mamaDateTime_destroy(m_in);
        mamaDateTime_destroy(m_update);
        mamaDateTime_destroy(m_out);
        
        PayloadCompositeTests::TearDown();
    }
};

/**
 * AddDateTime test fixtures
 * ****************************************************************************
 */

TEST_F(PayloadDateTimeTests, AddDateTime)
{
    m_status = m_payloadBridge->msgPayloadAddDateTime(m_msg, NULL, 1, m_in);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
}

TEST_F(PayloadDateTimeTests, AddDateTimeNullAdd)
{
    m_status = m_payloadBridge->msgPayloadAddDateTime(m_msg, NULL, 1, NULL);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadDateTimeTests, AddDateTimeNullMessage)
{
    m_status = m_payloadBridge->msgPayloadAddDateTime(NULL, NULL, 1, m_in);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}


/**
 * UpdateDateTime test fixtures
 * ****************************************************************************
 */

TEST_F(PayloadDateTimeTests, UpdateDateTime)
{
    m_payloadBridge->msgPayloadAddDateTime(m_msg, NULL, 1, m_in);
    m_status = m_payloadBridge->msgPayloadUpdateDateTime(m_msg, NULL, 1, m_update);
    m_payloadBridge->msgPayloadGetDateTime(m_msg, NULL, 1, m_out);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (1, mamaDateTime_equal(m_out, m_update));
}

TEST_F(PayloadDateTimeTests, UpdateDateTimeNoAdd)
{
    m_status = m_payloadBridge->msgPayloadUpdateDateTime(m_msg, NULL, 1, m_update);
    m_payloadBridge->msgPayloadGetDateTime(m_msg, NULL, 1, m_out);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (1, mamaDateTime_equal(m_out, m_update));
}

TEST_F(PayloadDateTimeTests, UpdateDateTimeNullUpdate)
{
    m_payloadBridge->msgPayloadAddDateTime(m_msg, NULL, 1, m_in);
    m_status = m_payloadBridge->msgPayloadUpdateDateTime(m_msg, NULL, 1, NULL);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadDateTimeTests, UpdateDateTimeNullMessage)
{
    m_payloadBridge->msgPayloadAddDateTime(m_msg, NULL, 1, m_in);
    m_status = m_payloadBridge->msgPayloadUpdateDateTime(NULL, NULL, 1, m_update);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

/**
 * GetDateTime test fixtures
 * ****************************************************************************
 */

TEST_F(PayloadDateTimeTests, GetDateTime)
{
    m_payloadBridge->msgPayloadAddDateTime(m_msg, NULL, 1, m_in);
    m_status = m_payloadBridge->msgPayloadGetDateTime(m_msg, NULL, 1, m_out);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (1, mamaDateTime_equal(m_in, m_out));
}

TEST_F(PayloadDateTimeTests, GetDateTimeNullOutput)
{
    m_payloadBridge->msgPayloadAddDateTime(m_msg, NULL, 1, m_in);
    m_status = m_payloadBridge->msgPayloadGetDateTime(m_msg, NULL, 1, NULL);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadDateTimeTests, GetDateTimeNullMessage)
{
    m_payloadBridge->msgPayloadAddDateTime(m_msg, NULL, 1, m_in);
    m_status = m_payloadBridge->msgPayloadGetDateTime(NULL, NULL, 1, m_out);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

/**
 * ****************************************************************************
 * PRICE TEST SUITE
 * ****************************************************************************
 */

class PayloadPriceTests : public PayloadCompositeTests
{
protected:

    // Reusables
    mamaPrice m_in, m_update, m_out;

    PayloadPriceTests() 
        : m_in(NULL)
        , m_update(NULL)
        , m_out(NULL)
    {
    }

    virtual void SetUp()
    {
        PayloadCompositeTests::SetUp();

        mamaPrice_create(&m_in);
        mamaPrice_create(&m_update);
        mamaPrice_create(&m_out);

        mamaPrice_setValue(m_in, 1.0f);
        mamaPrice_setValue(m_update, 2.0f);
    }

    virtual void TearDown()
    {
        mamaPrice_destroy(m_in);
        mamaPrice_destroy(m_update);
        mamaPrice_destroy(m_out);

        PayloadCompositeTests::TearDown();
    }
};

/**
 * AddPrice test suite
 * ****************************************************************************
 */

TEST_F(PayloadPriceTests, AddPrice)
{
    m_status = m_payloadBridge->msgPayloadAddPrice(m_msg, NULL, 1, m_in);
    m_payloadBridge->msgPayloadGetPrice(m_msg, NULL, 1, m_out);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (1, mamaPrice_equal(m_out, m_in));
}

TEST_F(PayloadPriceTests, AddPriceNullAdd)
{
    m_status = m_payloadBridge->msgPayloadAddPrice(m_msg, NULL, 1, NULL);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadPriceTests, AddPriceNullMessage)
{
    m_status = m_payloadBridge->msgPayloadAddPrice(NULL, NULL, 1, m_in);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

/**
 * UpdatePrice test suite
 * ****************************************************************************
 */

TEST_F(PayloadPriceTests, UpdatePrice)
{
    m_payloadBridge->msgPayloadAddPrice(m_msg, NULL, 1, m_in);
    m_status = m_payloadBridge->msgPayloadUpdatePrice(m_msg, NULL, 1, m_update);
    m_payloadBridge->msgPayloadGetPrice(m_msg, NULL, 1, m_out);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (1, mamaPrice_equal(m_out, m_update));
}

TEST_F(PayloadPriceTests, UpdatePriceNoAdd)
{
    m_status = m_payloadBridge->msgPayloadUpdatePrice(m_msg, NULL, 1, m_update);
    m_payloadBridge->msgPayloadGetPrice(m_msg, NULL, 1, m_out);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (1, mamaPrice_equal(m_out, m_update));
}

TEST_F(PayloadPriceTests, UpdatePriceNullUpdate)
{
    m_payloadBridge->msgPayloadAddPrice(m_msg, NULL, 1, m_in);
    m_status = m_payloadBridge->msgPayloadUpdatePrice(m_msg, NULL, 1, NULL);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadPriceTests, UpdatePriceNullMessage)
{
    m_payloadBridge->msgPayloadAddPrice(m_msg, NULL, 1, m_in);
    m_status = m_payloadBridge->msgPayloadUpdatePrice(NULL, NULL, 1, m_update);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

/**
 * GetPrice test suite
 * ****************************************************************************
 */

TEST_F(PayloadPriceTests, GetPrice)
{
    m_payloadBridge->msgPayloadAddPrice(m_msg, NULL, 1, m_in);
    m_status = m_payloadBridge->msgPayloadGetPrice(m_msg, NULL, 1, m_out);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (1, mamaPrice_equal(m_in, m_out));
}

TEST_F(PayloadPriceTests, GetPriceNullOutput)
{
    m_payloadBridge->msgPayloadAddPrice(m_msg, NULL, 1, m_in);
    m_status = m_payloadBridge->msgPayloadGetPrice(m_msg, NULL, 1, NULL);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadPriceTests, GetPriceNullMessage)
{
    m_payloadBridge->msgPayloadAddPrice(m_msg, NULL, 1, m_in);
    m_status = m_payloadBridge->msgPayloadGetPrice(NULL, NULL, 1, m_out);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

/**
 * ****************************************************************************
 * SUB MESSAGE TEST SUITE
 * ****************************************************************************
 */

class PayloadSubMsgTests : public PayloadCompositeTests
{
protected:

    // Reusables
    msgPayload m_in, m_update, m_out;
    mamaMsg m_inMsg, m_updateMsg;

    virtual void SetUp()
    {
        PayloadCompositeTests::SetUp();

        mamaMsg_create(&m_inMsg);
        mamaMsgImpl_getPayload(m_inMsg, &m_in);
        m_payloadBridge->msgPayloadAddBool(m_in, NULL, 2, 0);

        mamaMsg_create(&m_updateMsg);
        mamaMsgImpl_getPayload(m_updateMsg, &m_update);
        m_payloadBridge->msgPayloadAddChar (m_in, NULL, 1, 'A');
    }

    virtual void TearDown()
    {
        if (m_inMsg)
        {
            mamaMsg_destroy(m_inMsg);
            m_in = NULL;
        }
        if (m_updateMsg)
        {
            mamaMsg_destroy(m_updateMsg);
            m_update = NULL;
        }
        PayloadCompositeTests::TearDown();
    }
};

/**
 * AddMsg test fixtures
 * ****************************************************************************
 */

TEST_F(PayloadSubMsgTests, AddSubMsg)
{
    m_status = m_payloadBridge->msgPayloadAddMsg(m_msg, NULL, 1, m_inMsg);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
}

TEST_F(PayloadSubMsgTests, AddSubMsgNullAdd)
{
    m_status = m_payloadBridge->msgPayloadAddMsg(m_msg, NULL, 1, NULL);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadSubMsgTests, AddSubMsgNullMessage)
{
    m_status = m_payloadBridge->msgPayloadAddMsg(NULL, NULL, 1, m_inMsg);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

/**
 * UpdateSubMsg test fixtures
 * ****************************************************************************
 */

TEST_F(PayloadSubMsgTests, UpdateSubMsg)
{
    m_status = m_payloadBridge->msgPayloadUpdateSubMsg(m_msg, NULL, 1, m_updateMsg);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
}

TEST_F(PayloadSubMsgTests, UpdateSubMsgNullUpdate)
{
    m_status = m_payloadBridge->msgPayloadUpdateSubMsg(m_msg, NULL, 1, NULL);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadSubMsgTests, UpdateSubMsgNullMessage)
{
    m_status = m_payloadBridge->msgPayloadUpdateSubMsg(NULL, NULL, 1, m_updateMsg);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

/**
 * GetSubMsg test fixtures
 * ****************************************************************************
 */

TEST_F(PayloadSubMsgTests, GetSubMsg)
{ 
//    mama_bool_t a,b;
    m_payloadBridge->msgPayloadGetMsg(m_msg, NULL, 1, &m_out);
//    m_payloadBridge->msgPayloadGetBool(m_in, NULL, 2, &a);
//    m_payloadBridge->msgPayloadGetBool(m_in, NULL, 2, &b);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
//    EXPECT_EQ(a|b, 0xFF);
//    TODO: Fix
}

TEST_F(PayloadSubMsgTests, GetSubMsgNullUpdate)
{
    m_status = m_payloadBridge->msgPayloadAddMsg(m_msg, NULL, 1, NULL);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadSubMsgTests, GetSubMsgNullMessage)
{
    m_status = m_payloadBridge->msgPayloadAddMsg(NULL, NULL, 1, m_inMsg);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}
