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
#include "msgimpl.h"
#include "payloadbridge.h"
#include "MainUnitTestC.h"

using namespace ::testing;

/**
 * Boilerplate test suite base class
 * ****************************************************************************
 */
class PayloadVectorTests : public Test
{
public:

    virtual ~PayloadVectorTests()
    {}

protected:

    PayloadVectorTests()
        : m_payload(::getPayload())
        , m_payloadBridge(NULL)
        , m_msg(NULL)
        , m_status(MAMA_STATUS_OK)
    {
        Test::RecordProperty("payload", m_payload.c_str());
    }

    // Common SetUp
    virtual void SetUp()
    {
        // Load payload bridge and create dummy message
        mama_loadPayloadBridge( &m_payloadBridge, m_payload.c_str() );
        m_payloadBridge->msgPayloadCreate( &m_msg );

        // Reset status
        m_status = MAMA_STATUS_OK;
    }

    // Common TearDown
    virtual void TearDown()
    {
        m_payloadBridge->msgPayloadDestroy( m_msg );
    }

    // Reusables
    std::string         m_payload;

    mamaPayloadBridge   m_payloadBridge;
    msgPayload          m_msg;
    mama_status         m_status;


    template < typename T >
    void freeVector( T * & pOut)
    {
        ::free( (void *) pOut );

        pOut = NULL;
    }
};

#define  VECTOR_SIZE                ( 10 )
#define  VECTOR_UPDATE_SIZE         ( 20 )

// ****************************************************************************
// Bool Test Suite
// ****************************************************************************

class PayloadVectorBoolTests : public PayloadVectorTests
{
public:

    virtual ~PayloadVectorBoolTests()
    {}

protected:

    mama_bool_t m_in[ VECTOR_SIZE ];
    mama_bool_t m_update[ VECTOR_UPDATE_SIZE ];
    const mama_bool_t *m_out;
    mama_size_t m_outSize;

    PayloadVectorBoolTests() 
        : m_in( )
        , m_update( )
        , m_out( NULL )
        , m_outSize( 0 )
    {
    }
 
    virtual void SetUp()
    {
        PayloadVectorTests::SetUp();

        for( uint32_t ii(0) ; ii < VECTOR_SIZE ; ++ii )
        {
            m_in[ii] = ( 0 == ii%2 ) ? 1 : 0;
        }

        for( uint32_t ii(0) ; ii < VECTOR_UPDATE_SIZE ; ++ii )
        {
            m_update[ii] = ( 0 == ii%2 ) ? 0 : 1;
        }
    }

    virtual void TearDown()
    {
        PayloadVectorTests::TearDown();
    }
};

// AddVectorBool test fixtures
// ***************************

TEST_F(PayloadVectorBoolTests, AddVectorBool)
{
    m_status = m_payloadBridge->msgPayloadAddVectorBool(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorBool(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK , m_status);
    EXPECT_EQ ( 0, ::memcmp( m_in, m_out, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE , m_outSize);

    //freeVector( m_out );
}

TEST_F(PayloadVectorBoolTests, AddVectorBoolAfterInit)
{
    m_status = m_payloadBridge->msgPayloadAddVectorBool(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadAddVectorBool(m_msg, NULL, 2, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorBool(m_msg, NULL, 2, &m_out, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_OK , m_status);
    EXPECT_EQ ( 0, ::memcmp( m_in, m_out, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE , m_outSize);

    //freeVector( m_out );
}

TEST_F(PayloadVectorBoolTests, AddVectorBoolNullAdd)
{
    m_status = m_payloadBridge->msgPayloadAddVectorBool(m_msg, NULL, 1, NULL, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG , m_status);
}

TEST_F(PayloadVectorBoolTests, AddVectorBoolNullMessage)
{
    m_status = m_payloadBridge->msgPayloadAddVectorBool(NULL, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorBoolTests, AddVectorBoolInvalidName)
{
    m_status = m_payloadBridge->msgPayloadAddVectorBool(m_msg, "Bob", 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadAddVectorBool(m_msg, NULL, 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorBoolTests, AddVectorBoolInvalidFid)
{
    m_status = m_payloadBridge->msgPayloadAddVectorBool(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadAddVectorBool(m_msg, NULL, 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

// UpdateVectorBool test fixtures
// ******************************

TEST_F(PayloadVectorBoolTests, UpdateVectorBool)
{
    m_status = m_payloadBridge->msgPayloadAddVectorBool(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorBool(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorBool(m_msg, NULL, 1, m_update, VECTOR_UPDATE_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorBool(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_update, m_out, (sizeof(mama_bool_t) * VECTOR_UPDATE_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_UPDATE_SIZE, m_outSize);
}

TEST_F(PayloadVectorBoolTests, UpdateVectorBoolInitByName)
{
    m_status = m_payloadBridge->msgPayloadUpdateVectorBool(m_msg, "Bob", 0, m_update, VECTOR_UPDATE_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorBool(m_msg, "Bob", 0, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_update, m_out, (sizeof(mama_bool_t) * VECTOR_UPDATE_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_UPDATE_SIZE, m_outSize);
}

TEST_F(PayloadVectorBoolTests, UpdateVectorBoolInitByFid)
{
    m_status = m_payloadBridge->msgPayloadUpdateVectorBool(m_msg, "Rhubarb", 1, m_update, VECTOR_UPDATE_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorBool(m_msg, "Rhubarb", 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_update, m_out, (sizeof(mama_bool_t) * VECTOR_UPDATE_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_UPDATE_SIZE, m_outSize);
}

TEST_F(PayloadVectorBoolTests, UpdateVectorBoolNullUpdate)
{
    m_status = m_payloadBridge->msgPayloadAddVectorBool(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorBool(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorBool(m_msg, NULL, 1, NULL, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorBoolTests, UpdateVectorBoolNullMessage)
{
    m_status = m_payloadBridge->msgPayloadAddVectorBool(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorBool(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorBool(NULL, NULL, 1, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorBoolTests, UpdateVectorBoolInvalidName)
{
    m_status = m_payloadBridge->msgPayloadAddVectorBool(m_msg, "Bob", 0, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorBool(m_msg, "Bob", 0, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorBool(m_msg, NULL, 0, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorBoolTests, UpdateVectorBoolInvalidFid)
{
    m_status = m_payloadBridge->msgPayloadAddVectorBool(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorBool(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorBool(m_msg, NULL, 0, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorBoolTests, UpdateVectorBoolInvalidFidNo)
{
    m_status = m_payloadBridge->msgPayloadAddVectorBool(m_msg, "Bob", 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadUpdateVectorBool(m_msg, "Bob", 1, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
}

// GetVectorBool test fixtures
// ***************************

TEST_F(PayloadVectorBoolTests, GetVectorBool)
{
    m_status = m_payloadBridge->msgPayloadAddVectorBool(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorBool(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, memcmp( m_in, m_out, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_NE(0, memcmp( m_update, m_out, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorBool(m_msg, NULL, 1, m_update, VECTOR_UPDATE_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorBool(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, memcmp( m_update, m_out, (sizeof(mama_bool_t) * VECTOR_UPDATE_SIZE) ) );
    EXPECT_NE(0, memcmp( m_in, m_out, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_UPDATE_SIZE, m_outSize);
    EXPECT_NE ((mama_size_t)VECTOR_SIZE, m_outSize);
}

TEST_F(PayloadVectorBoolTests, GetVectorBoolNullResult)
{
    m_status = m_payloadBridge->msgPayloadAddVectorBool(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorBool(m_msg, NULL, 1, NULL, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorBoolTests, GetVectorBoolNullSize)
{
    m_status = m_payloadBridge->msgPayloadAddVectorBool(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorBool(m_msg, NULL, 1, &m_out, NULL);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorBoolTests, GetVectorBoolNotFound)
{
    m_status = m_payloadBridge->msgPayloadAddVectorBool(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorBool(m_msg, NULL, 2, &m_out, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_NOT_FOUND, m_status);
}

// ****************************************************************************
// Char Test Suite
// ****************************************************************************

class PayloadVectorCharTests : public PayloadVectorTests
{
public:

    virtual ~PayloadVectorCharTests()
    {}

protected:

    char m_in[ VECTOR_SIZE ];
    char m_update[ VECTOR_UPDATE_SIZE ];
    const char *m_out;
    mama_size_t m_outSize;

    PayloadVectorCharTests()
        : m_in( )
        , m_update( )
        , m_out( NULL )
        , m_outSize( 0 )
    {
    }
    
    virtual void SetUp()
    {
        PayloadVectorTests::SetUp();

        for( uint32_t ii(0) ; ii < VECTOR_SIZE ; ++ii )
        {
            m_in[ii] = 'a' + ii;
            m_update[ii] = 'A' + ii;
        }
    }

    virtual void TearDown()
    {
        PayloadVectorTests::TearDown();
    }
};

// AddVectorChar test fixtures
// ***************************

TEST_F(PayloadVectorCharTests, AddVectorChar)
{
    m_status = m_payloadBridge->msgPayloadAddVectorChar(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorChar(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK , m_status);
    EXPECT_EQ ( 0, ::memcmp( m_in, m_out, (sizeof(char) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE , m_outSize);
}

TEST_F(PayloadVectorCharTests, AddVectorCharNullChar)
{
    m_status = m_payloadBridge->msgPayloadAddVectorChar(m_msg, NULL, 1, NULL, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG , m_status);
}

TEST_F(PayloadVectorCharTests, AddVectorCharNullMsg)
{
    m_status = m_payloadBridge->msgPayloadAddVectorChar(NULL, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorCharTests, AddVectorCharAfterInit)
{
    m_status = m_payloadBridge->msgPayloadAddVectorChar(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadAddVectorChar(m_msg, NULL, 2, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorChar(m_msg, NULL, 2, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK , m_status);
    EXPECT_EQ ( 0, ::memcmp( m_in, m_out, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE , m_outSize);

    //freeVector( m_out );
}

TEST_F(PayloadVectorCharTests, AddVectorCharInvalidName)
{
    m_status = m_payloadBridge->msgPayloadAddVectorChar(m_msg, "Bob", 0, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadAddVectorChar(m_msg, NULL, 0, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorCharTests, AddVectorCharInvalidFid)
{
    m_status = m_payloadBridge->msgPayloadAddVectorChar(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadAddVectorChar(m_msg, NULL, 0, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

// UpdateVectorChar test fixtures
// ******************************

TEST_F(PayloadVectorCharTests, UpdateVectorChar)
{
    m_status = m_payloadBridge->msgPayloadAddVectorChar(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorChar(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(char) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorChar(m_msg, NULL, 1, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorChar(m_msg, NULL, 1, &m_out, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_update, m_out, (sizeof(char) * VECTOR_UPDATE_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_UPDATE_SIZE, m_outSize);
}

TEST_F(PayloadVectorCharTests, UpdateVectorCharInitByName)
{
    m_status = m_payloadBridge->msgPayloadUpdateVectorChar(m_msg, "Bob", 0, m_update, VECTOR_UPDATE_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorChar(m_msg, "Bob", 0, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_update, m_out, (sizeof(mama_bool_t) * VECTOR_UPDATE_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_UPDATE_SIZE, m_outSize);
}

TEST_F(PayloadVectorCharTests, UpdateVectorCharNullUpdate)
{
    m_status = m_payloadBridge->msgPayloadAddVectorChar(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorChar(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(char) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorChar(m_msg, NULL, 1, NULL, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorCharTests, UpdateVectorCharNullMessage)
{
    m_status = m_payloadBridge->msgPayloadAddVectorChar(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorChar(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(char) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorChar(NULL, NULL, 1, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorCharTests, UpdateVectorCharInvalidName)
{
    m_status = m_payloadBridge->msgPayloadAddVectorChar(m_msg, "Bob", 0, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorChar(m_msg, "Bob", 0, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorChar(m_msg, NULL, 0, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorCharTests, UpdateVectorCharInvalidFid)
{
    m_status = m_payloadBridge->msgPayloadAddVectorChar(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorChar(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorChar(m_msg, NULL, 0, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorCharTests, UpdateVectorCharInvalidFidNo)
{
    m_status = m_payloadBridge->msgPayloadAddVectorChar(m_msg, "Bob", 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadUpdateVectorChar(m_msg, "Bob", 1, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
}

// GetVectorChar test fixtures
// ***************************

TEST_F(PayloadVectorCharTests, GetVectorChar)
{
    m_status = m_payloadBridge->msgPayloadAddVectorChar(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorChar(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, memcmp( m_in, m_out, (sizeof(char) * VECTOR_SIZE) ) );
    EXPECT_NE(0, memcmp( m_update, m_out, (sizeof(char) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorChar(m_msg, NULL, 1, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorChar(m_msg, NULL, 1, &m_out, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, memcmp( m_update, m_out, (sizeof(char) * VECTOR_UPDATE_SIZE) ) );
    EXPECT_NE(0, memcmp( m_in, m_out, (sizeof(char) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_UPDATE_SIZE, m_outSize);
    EXPECT_NE ((mama_size_t)VECTOR_SIZE, m_outSize);
}

TEST_F(PayloadVectorCharTests, GetVectorCharNullResult)
{
    m_status = m_payloadBridge->msgPayloadAddVectorChar(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorChar(m_msg, NULL, 1, NULL, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorCharTests, GetVectorCharNullSize)
{
    m_status = m_payloadBridge->msgPayloadAddVectorChar(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorChar(m_msg, NULL, 1, &m_out, NULL);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorCharTests, GetVectorCharNotFound)
{
    m_status = m_payloadBridge->msgPayloadAddVectorChar(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorChar(m_msg, NULL, 2, &m_out, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_NOT_FOUND, m_status);
}

// ****************************************************************************
// I8 Test Suite
// ****************************************************************************

class PayloadVectorI8Tests : public PayloadVectorTests
{
public:

    virtual ~PayloadVectorI8Tests()
    {}

protected:

    mama_i8_t m_in[ VECTOR_SIZE ];
    mama_i8_t m_update[ VECTOR_UPDATE_SIZE ];
    const mama_i8_t *m_out;
    mama_size_t m_outSize;

    PayloadVectorI8Tests()
        : m_in( )
        , m_update( )
        , m_out( NULL )
        , m_outSize( 0 )
    {
    }
    
    virtual void SetUp()
    {
        PayloadVectorTests::SetUp();

        for( uint32_t ii(0) ; ii < VECTOR_SIZE ; ++ii )
        {
            m_in[ii] = ii;
            m_update[ii] = 100 + ii;
        }
    }

    virtual void TearDown()
    {
        PayloadVectorTests::TearDown();
    }
};


// AddVectorI8 test fixtures
// *************************

TEST_F(PayloadVectorI8Tests, AddVectorI8)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI8(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI8(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK , m_status);
    EXPECT_EQ ( 0, ::memcmp( m_in, m_out, (sizeof(mama_i8_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE , m_outSize);
}

TEST_F(PayloadVectorI8Tests, AddVectorI8NullI8)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI8(m_msg, NULL, 1, NULL, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG , m_status);
}

TEST_F(PayloadVectorI8Tests, AddVectorI8NullMsg)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI8(NULL, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorI8Tests, AddVectorI8AfterInit)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI8(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadAddVectorI8(m_msg, NULL, 2, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI8(m_msg, NULL, 2, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK , m_status);
    EXPECT_EQ ( 0, ::memcmp( m_in, m_out, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE , m_outSize);

    //freeVector( m_out );
}

TEST_F(PayloadVectorI8Tests, AddVectorI8InvalidName)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI8(m_msg, "Bob", 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadAddVectorI8(m_msg, NULL, 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorI8Tests, AddVectorI8InvalidFid)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI8(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadAddVectorI8(m_msg, NULL, 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

// UpdateVectorI8 test fixtures
// ****************************

TEST_F(PayloadVectorI8Tests, UpdateVectorI8)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI8(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI8(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_i8_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorI8(m_msg, NULL, 1, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI8(m_msg, NULL, 1, &m_out, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_update, m_out, (sizeof(mama_i8_t) * VECTOR_UPDATE_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_UPDATE_SIZE, m_outSize);
}

TEST_F(PayloadVectorI8Tests, UpdateVectorI8NullUpdate)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI8(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI8(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_i8_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorI8(m_msg, NULL, 1, NULL, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorI8Tests, UpdateVectorI8NullMessage)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI8(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI8(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_i8_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorI8(NULL, NULL, 1, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorI8Tests, UpdateVectorI8InitByName)
{
    m_status = m_payloadBridge->msgPayloadUpdateVectorI8(m_msg, "Bob", 0, m_update, VECTOR_UPDATE_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI8(m_msg, "Bob", 0, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_update, m_out, (sizeof(mama_bool_t) * VECTOR_UPDATE_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_UPDATE_SIZE, m_outSize);
}

TEST_F(PayloadVectorI8Tests, UpdateVectorI8InvalidName)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI8(m_msg, "Bob", 0, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI8(m_msg, "Bob", 0, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorI8(m_msg, NULL, 0, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorI8Tests, UpdateVectorI8InvalidFid)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI8(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI8(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorI8(m_msg, NULL, 0, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorI8Tests, UpdateVectorI8InvalidFidNo)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI8(m_msg, "Bob", 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadUpdateVectorI8(m_msg, "Bob", 1, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
}

// GetVectorI8 test fixtures
// *************************

TEST_F(PayloadVectorI8Tests, GetVectorI8)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI8(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI8(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, memcmp( m_in, m_out, (sizeof(mama_i8_t) * VECTOR_SIZE) ) );
    EXPECT_NE(0, memcmp( m_update, m_out, (sizeof(mama_i8_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorI8(m_msg, NULL, 1, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI8(m_msg, NULL, 1, &m_out, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, memcmp( m_update, m_out, (sizeof(mama_i8_t) * VECTOR_UPDATE_SIZE) ) );
    EXPECT_NE(0, memcmp( m_in, m_out, (sizeof(mama_i8_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_UPDATE_SIZE, m_outSize);
    EXPECT_NE ((mama_size_t)VECTOR_SIZE, m_outSize);
}

TEST_F(PayloadVectorI8Tests, GetVectorI8NullResult)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI8(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI8(m_msg, NULL, 1, NULL, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorI8Tests, GetVectorI8NullSize)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI8(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI8(m_msg, NULL, 1, &m_out, NULL);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorI8Tests, GetVectorI8NotFound)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI8(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI8(m_msg, NULL, 2, &m_out, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_NOT_FOUND, m_status);
}

// ****************************************************************************
// U8 Test Suite
// ****************************************************************************

class PayloadVectorU8Tests : public PayloadVectorTests
{
public:

    virtual ~PayloadVectorU8Tests()
    {}

protected:

    mama_u8_t m_in[ VECTOR_SIZE ];
    mama_u8_t m_update[ VECTOR_UPDATE_SIZE ];
    const mama_u8_t *m_out;
    mama_size_t m_outSize;

    PayloadVectorU8Tests()
        : m_in( )
        , m_update( )
        , m_out( NULL )
        , m_outSize( 0 )
    {
    }
    
    virtual void SetUp()
    {
        PayloadVectorTests::SetUp();

        for( uint32_t ii(0) ; ii < VECTOR_SIZE ; ++ii )
        {
            m_in[ii] = ii;
            m_update[ii] = 100 + ii;
        }
    }

    virtual void TearDown()
    {
        PayloadVectorTests::TearDown();
    }
};


// AddVectorU8 test fixtures
// *************************

TEST_F(PayloadVectorU8Tests, AddVectorU8)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU8(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU8(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK , m_status);
    EXPECT_EQ ( 0, ::memcmp( m_in, m_out, (sizeof(mama_u8_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE , m_outSize);
}

TEST_F(PayloadVectorU8Tests, AddVectorU8NullU8)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU8(m_msg, NULL, 1, NULL, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG , m_status);
}

TEST_F(PayloadVectorU8Tests, AddVectorU8NullMsg)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU8(NULL, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorU8Tests, AddVectorU8AfterInit)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU8(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadAddVectorU8(m_msg, NULL, 2, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU8(m_msg, NULL, 2, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK , m_status);
    EXPECT_EQ ( 0, ::memcmp( m_in, m_out, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE , m_outSize);

    //freeVector( m_out );
}

TEST_F(PayloadVectorU8Tests, AddVectorU8InvalidName)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU8(m_msg, "Bob", 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadAddVectorU8(m_msg, NULL, 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorU8Tests, AddVectorU8InvalidFid)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU8(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadAddVectorU8(m_msg, NULL, 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

// UpdateVectorU8 test fixtures
// ****************************

TEST_F(PayloadVectorU8Tests, UpdateVectorU8)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU8(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU8(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_u8_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorU8(m_msg, NULL, 1, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU8(m_msg, NULL, 1, &m_out, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_update, m_out, (sizeof(mama_u8_t) * VECTOR_UPDATE_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_UPDATE_SIZE, m_outSize);
}

TEST_F(PayloadVectorU8Tests, UpdateVectorU8NullUpdate)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU8(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU8(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_u8_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorU8(m_msg, NULL, 1, NULL, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorU8Tests, UpdateVectorU8NullMessage)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU8(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU8(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_u8_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorU8(NULL, NULL, 1, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorU8Tests, UpdateVectorU8InitByName)
{
    m_status = m_payloadBridge->msgPayloadUpdateVectorU8(m_msg, "Bob", 0, m_update, VECTOR_UPDATE_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU8(m_msg, "Bob", 0, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_update, m_out, (sizeof(mama_bool_t) * VECTOR_UPDATE_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_UPDATE_SIZE, m_outSize);
}

TEST_F(PayloadVectorU8Tests, UpdateVectorU8InvalidName)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU8(m_msg, "Bob", 0, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU8(m_msg, "Bob", 0, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorU8(m_msg, NULL, 0, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorU8Tests, UpdateVectorU8InvalidFid)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU8(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU8(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorU8(m_msg, NULL, 0, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorU8Tests, UpdateVectorU8InvalidFidNo)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU8(m_msg, "Bob", 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadUpdateVectorU8(m_msg, "Bob", 1, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
}

// GetVectorU8 test fixtures
// *************************

TEST_F(PayloadVectorU8Tests, GetVectorU8)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU8(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU8(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, memcmp( m_in, m_out, (sizeof(mama_u8_t) * VECTOR_SIZE) ) );
    EXPECT_NE(0, memcmp( m_update, m_out, (sizeof(mama_u8_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorU8(m_msg, NULL, 1, m_update, VECTOR_UPDATE_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU8(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, memcmp( m_update, m_out, (sizeof(mama_u8_t) * VECTOR_UPDATE_SIZE) ) );
    EXPECT_NE(0, memcmp( m_in, m_out, (sizeof(mama_u8_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_UPDATE_SIZE, m_outSize);
    EXPECT_NE ((mama_size_t)VECTOR_SIZE, m_outSize);
}

TEST_F(PayloadVectorU8Tests, GetVectorU8NullResult)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU8(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU8(m_msg, NULL, 1, NULL, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorU8Tests, GetVectorU8NullSize)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU8(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU8(m_msg, NULL, 1, &m_out, NULL);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorU8Tests, GetVectorU8NotFound)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU8(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU8(m_msg, NULL, 2, &m_out, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_NOT_FOUND, m_status);
}

// ****************************************************************************
// I16 Test Suite
// ****************************************************************************

class PayloadVectorI16Tests : public PayloadVectorTests
{
public:

    virtual ~PayloadVectorI16Tests()
    {}

protected:

    mama_i16_t m_in[ VECTOR_SIZE ];
    mama_i16_t m_update[ VECTOR_UPDATE_SIZE ];
    const mama_i16_t *m_out;
    mama_size_t m_outSize;

    PayloadVectorI16Tests()
        : m_in( )
        , m_update( )
        , m_out( NULL )
        , m_outSize( 0 )
    {
    }
    
    virtual void SetUp()
    {
        PayloadVectorTests::SetUp();

        for( uint32_t ii(0) ; ii < VECTOR_SIZE ; ++ii )
        {
            m_in[ii] = ii;
            m_update[ii] = 100 + ii;
        }
    }

    virtual void TearDown()
    {
        PayloadVectorTests::TearDown();
    }
};


// AddVectorI16 test fixtures
// **************************

TEST_F(PayloadVectorI16Tests, AddVectorI16)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI16(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI16(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK , m_status);
    EXPECT_EQ ( 0, ::memcmp( m_in, m_out, (sizeof(mama_i16_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE , m_outSize);
}

TEST_F(PayloadVectorI16Tests, AddVectorI16NullI16)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI16(m_msg, NULL, 1, NULL, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG , m_status);
}

TEST_F(PayloadVectorI16Tests, AddVectorI16NullMsg)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI16(NULL, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorI16Tests, AddVectorI16AfterInit)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI16(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadAddVectorI16(m_msg, NULL, 2, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI16(m_msg, NULL, 2, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK , m_status);
    EXPECT_EQ ( 0, ::memcmp( m_in, m_out, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE , m_outSize);

    //freeVector( m_out );
}

TEST_F(PayloadVectorI16Tests, AddVectorI16InvalidName)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI16(m_msg, "Bob", 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadAddVectorI16(m_msg, NULL, 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorI16Tests, AddVectorI16InvalidFid)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI16(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadAddVectorI16(m_msg, NULL, 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

// UpdateVectorI16 test fixtures
// *****************************

TEST_F(PayloadVectorI16Tests, UpdateVectorI16)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI16(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI16(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_i16_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorI16(m_msg, NULL, 1, m_update, VECTOR_UPDATE_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI16(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_update, m_out, (sizeof(mama_i16_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_UPDATE_SIZE, m_outSize);
}

TEST_F(PayloadVectorI16Tests, UpdateVectorI16NullUpdate)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI16(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI16(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_i16_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorI16(m_msg, NULL, 1, NULL, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorI16Tests, UpdateVectorI16NullMessage)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI16(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI16(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_i16_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorI16(NULL, NULL, 1, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorI16Tests, UpdateVectorI16InitByName)
{
    m_status = m_payloadBridge->msgPayloadUpdateVectorI16(m_msg, "Bob", 0, m_update, VECTOR_UPDATE_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI16(m_msg, "Bob", 0, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_update, m_out, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_UPDATE_SIZE, m_outSize);
}

TEST_F(PayloadVectorI16Tests, UpdateVectorI16InvalidName)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI16(m_msg, "Bob", 0, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI16(m_msg, "Bob", 0, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorI16(m_msg, NULL, 0, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorI16Tests, UpdateVectorI16InvalidFid)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI16(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI16(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorI16(m_msg, NULL, 0, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorI16Tests, UpdateVectorI16InvalidFidNo)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI16(m_msg, "Bob", 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadUpdateVectorI16(m_msg, "Bob", 1, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
}

// GetVectorI16 test fixtures
// **************************

TEST_F(PayloadVectorI16Tests, GetVectorI16)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI16(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI16(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, memcmp( m_in, m_out, (sizeof(mama_i16_t) * VECTOR_SIZE) ) );
    EXPECT_NE(0, memcmp( m_update, m_out, (sizeof(mama_i16_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorI16(m_msg, NULL, 1, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI16(m_msg, NULL, 1, &m_out, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, memcmp( m_update, m_out, (sizeof(mama_i16_t) * VECTOR_UPDATE_SIZE) ) );
    EXPECT_NE(0, memcmp( m_in, m_out, (sizeof(mama_i16_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_UPDATE_SIZE, m_outSize);
    EXPECT_NE ((mama_size_t)VECTOR_SIZE, m_outSize);
}

TEST_F(PayloadVectorI16Tests, GetVectorI16NullResult)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI16(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI16(m_msg, NULL, 1, NULL, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorI16Tests, GetVectorI16NullSize)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI16(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI16(m_msg, NULL, 1, &m_out, NULL);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorI16Tests, GetVectorI16NotFound)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI16(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI16(m_msg, NULL, 2, &m_out, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_NOT_FOUND, m_status);
}

// ****************************************************************************
// U16 Test Suite
// ****************************************************************************

class PayloadVectorU16Tests : public PayloadVectorTests
{
public:

    virtual ~PayloadVectorU16Tests()
    {}

protected:

    mama_u16_t m_in[ VECTOR_SIZE ];
    mama_u16_t m_update[ VECTOR_UPDATE_SIZE ];
    const mama_u16_t *m_out;
    mama_size_t m_outSize;

    PayloadVectorU16Tests()
        : m_in( )
        , m_update( )
        , m_out( NULL )
        , m_outSize( 0 )
    {
    }
    
    virtual void SetUp()
    {
        PayloadVectorTests::SetUp();

        for( uint32_t ii(0) ; ii < VECTOR_SIZE ; ++ii )
        {
            m_in[ii] = ii;
            m_update[ii] = 100 + ii;
        }
    }

    virtual void TearDown()
    {
        PayloadVectorTests::TearDown();
    }
};


// AddVectorU16 test fixtures
// **************************

TEST_F(PayloadVectorU16Tests, AddVectorU16)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU16(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU16(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK , m_status);
    EXPECT_EQ ( 0, ::memcmp( m_in, m_out, (sizeof(mama_u16_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE , m_outSize);
}

TEST_F(PayloadVectorU16Tests, AddVectorU16NullU16)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU16(m_msg, NULL, 1, NULL, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG , m_status);
}

TEST_F(PayloadVectorU16Tests, AddVectorU16NullMsg)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU16(NULL, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorU16Tests, AddVectorU16AfterInit)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU16(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadAddVectorU16(m_msg, NULL, 2, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU16(m_msg, NULL, 2, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK , m_status);
    EXPECT_EQ ( 0, ::memcmp( m_in, m_out, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE , m_outSize);

    //freeVector( m_out );
}

TEST_F(PayloadVectorU16Tests, AddVectorU16InvalidName)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU16(m_msg, "Bob", 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadAddVectorU16(m_msg, NULL, 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorU16Tests, AddVectorU16InvalidFid)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU16(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadAddVectorU16(m_msg, NULL, 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

// UpdateVectorU16 test fixtures
// *****************************

TEST_F(PayloadVectorU16Tests, UpdateVectorU16)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU16(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU16(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_u16_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorU16(m_msg, NULL, 1, m_update, VECTOR_UPDATE_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU16(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_update, m_out, (sizeof(mama_u16_t) * VECTOR_UPDATE_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_UPDATE_SIZE, m_outSize);
}

TEST_F(PayloadVectorU16Tests, UpdateVectorU16NullUpdate)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU16(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU16(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_u16_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorU16(m_msg, NULL, 1, NULL, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorU16Tests, UpdateVectorU16NullMessage)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU16(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU16(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_u16_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorU16(NULL, NULL, 1, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorU16Tests, UpdateVectorU16InitByName)
{
    m_status = m_payloadBridge->msgPayloadUpdateVectorU16(m_msg, "Bob", 0, m_update, VECTOR_UPDATE_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU16(m_msg, "Bob", 0, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_update, m_out, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_UPDATE_SIZE, m_outSize);
}

TEST_F(PayloadVectorU16Tests, UpdateVectorU16InvalidName)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU16(m_msg, "Bob", 0, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU16(m_msg, "Bob", 0, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorU16(m_msg, NULL, 0, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorU16Tests, UpdateVectorU16InvalidFid)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU16(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU16(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorU16(m_msg, NULL, 0, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorU16Tests, UpdateVectorU16InvalidFidNo)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU16(m_msg, "Bob", 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadUpdateVectorU16(m_msg, "Bob", 1, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
}

// GetVectorU16 test fixtures
// **************************

TEST_F(PayloadVectorU16Tests, GetVectorU16)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU16(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU16(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, memcmp( m_in, m_out, (sizeof(mama_u16_t) * VECTOR_SIZE) ) );
    EXPECT_NE(0, memcmp( m_update, m_out, (sizeof(mama_u16_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorU16(m_msg, NULL, 1, m_update, VECTOR_UPDATE_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU16(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, memcmp( m_update, m_out, (sizeof(mama_u16_t) * VECTOR_UPDATE_SIZE) ) );
    EXPECT_NE(0, memcmp( m_in, m_out, (sizeof(mama_u16_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_UPDATE_SIZE, m_outSize);
    EXPECT_NE ((mama_size_t)VECTOR_SIZE, m_outSize);
}

TEST_F(PayloadVectorU16Tests, GetVectorU16NullResult)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU16(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU16(m_msg, NULL, 1, NULL, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorU16Tests, GetVectorU16NullSize)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU16(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU16(m_msg, NULL, 1, &m_out, NULL);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorU16Tests, GetVectorU16NotFound)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU16(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU16(m_msg, NULL, 2, &m_out, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_NOT_FOUND, m_status);
}

// ****************************************************************************
// I32 Test Suite
// ****************************************************************************

class PayloadVectorI32Tests : public PayloadVectorTests
{
public:

    virtual ~PayloadVectorI32Tests()
    {}

protected:

    mama_i32_t m_in[ VECTOR_SIZE ];
    mama_i32_t m_update[ VECTOR_UPDATE_SIZE ];
    const mama_i32_t *m_out;
    mama_size_t m_outSize;

    PayloadVectorI32Tests()
        : m_in( )
        , m_update( )
        , m_out( NULL )
        , m_outSize( 0 )
    {
    }
    
    virtual void SetUp()
    {
        PayloadVectorTests::SetUp();

        for( uint32_t ii(0) ; ii < VECTOR_SIZE ; ++ii )
        {
            m_in[ii] = ii;
            m_update[ii] = 100 + ii;
        }
    }

    virtual void TearDown()
    {
        PayloadVectorTests::TearDown();
    }
};


// AddVectorI32 test fixtures
// **************************

TEST_F(PayloadVectorI32Tests, AddVectorI32)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI32(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI32(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK , m_status);
    EXPECT_EQ ( 0, ::memcmp( m_in, m_out, (sizeof(mama_i32_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE , m_outSize);
}

TEST_F(PayloadVectorI32Tests, AddVectorI32NullI32)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI32(m_msg, NULL, 1, NULL, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG , m_status);
}

TEST_F(PayloadVectorI32Tests, AddVectorI32NullMsg)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI32(NULL, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorI32Tests, AddVectorI32AfterInit)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI32(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadAddVectorI32(m_msg, NULL, 2, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI32(m_msg, NULL, 2, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK , m_status);
    EXPECT_EQ ( 0, ::memcmp( m_in, m_out, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE , m_outSize);

    //freeVector( m_out );
}

TEST_F(PayloadVectorI32Tests, AddVectorI32InvalidName)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI32(m_msg, "Bob", 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadAddVectorI32(m_msg, NULL, 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorI32Tests, AddVectorI32InvalidFid)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI32(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadAddVectorI32(m_msg, NULL, 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

// UpdateVectorI32 test fixtures
// *****************************

TEST_F(PayloadVectorI32Tests, UpdateVectorI32)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI32(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI32(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_i32_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorI32(m_msg, NULL, 1, m_update, VECTOR_UPDATE_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI32(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_update, m_out, (sizeof(mama_i32_t) * VECTOR_UPDATE_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_UPDATE_SIZE, m_outSize);
}

TEST_F(PayloadVectorI32Tests, UpdateVectorI32NullUpdate)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI32(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI32(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_i32_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorI32(m_msg, NULL, 1, NULL, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorI32Tests, UpdateVectorI32NullMessage)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI32(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI32(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_i32_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorI32(NULL, NULL, 1, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorI32Tests, UpdateVectorI32InitByName)
{
    m_status = m_payloadBridge->msgPayloadUpdateVectorI32(m_msg, "Bob", 0, m_update, VECTOR_UPDATE_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI32(m_msg, "Bob", 0, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_update, m_out, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_UPDATE_SIZE, m_outSize);
}

TEST_F(PayloadVectorI32Tests, UpdateVectorI32InvalidName)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI32(m_msg, "Bob", 0, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI32(m_msg, "Bob", 0, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorI32(m_msg, NULL, 0, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorI32Tests, UpdateVectorI32InvalidFid)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI32(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI32(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorI32(m_msg, NULL, 0, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorI32Tests, UpdateVectorI32InvalidFidNo)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI32(m_msg, "Bob", 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadUpdateVectorI32(m_msg, "Bob", 1, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
}

// GetVectorI32 test fixtures
// **************************

TEST_F(PayloadVectorI32Tests, GetVectorI32)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI32(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI32(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, memcmp( m_in, m_out, (sizeof(mama_i32_t) * VECTOR_SIZE) ) );
    EXPECT_NE(0, memcmp( m_update, m_out, (sizeof(mama_i32_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorI32(m_msg, NULL, 1, m_update, VECTOR_UPDATE_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI32(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, memcmp( m_update, m_out, (sizeof(mama_i32_t) * VECTOR_UPDATE_SIZE) ) );
    EXPECT_NE(0, memcmp( m_in, m_out, (sizeof(mama_i32_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_UPDATE_SIZE, m_outSize);
    EXPECT_NE ((mama_size_t)VECTOR_SIZE, m_outSize);
}

TEST_F(PayloadVectorI32Tests, GetVectorI32NullResult)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI32(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI32(m_msg, NULL, 1, NULL, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorI32Tests, GetVectorI32NullSize)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI32(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI32(m_msg, NULL, 1, &m_out, NULL);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorI32Tests, GetVectorI32NotFound)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI32(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI32(m_msg, NULL, 2, &m_out, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_NOT_FOUND, m_status);
}

// ****************************************************************************
// U32 Test Suite
// ****************************************************************************

class PayloadVectorU32Tests : public PayloadVectorTests
{
public:

    virtual ~PayloadVectorU32Tests()
    {}

protected:

    mama_u32_t m_in[ VECTOR_SIZE ];
    mama_u32_t m_update[ VECTOR_UPDATE_SIZE ];
    const mama_u32_t *m_out;
    mama_size_t m_outSize;

    PayloadVectorU32Tests()
        : m_in( )
        , m_update( )
        , m_out( NULL )
        , m_outSize( 0 )
    {
    }
    
    virtual void SetUp()
    {
        PayloadVectorTests::SetUp();

        for( uint32_t ii(0) ; ii < VECTOR_SIZE ; ++ii )
        {
            m_in[ii] = ii;
            m_update[ii] = 100 + ii;
        }
    }

    virtual void TearDown()
    {
        PayloadVectorTests::TearDown();
    }
};


// AddVectorU32 test fixtures
// **************************

TEST_F(PayloadVectorU32Tests, AddVectorU32)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU32(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU32(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK , m_status);
    EXPECT_EQ ( 0, ::memcmp( m_in, m_out, (sizeof(mama_u32_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE , m_outSize);
}

TEST_F(PayloadVectorU32Tests, AddVectorU32NullU32)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU32(m_msg, NULL, 1, NULL, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG , m_status);
}

TEST_F(PayloadVectorU32Tests, AddVectorU32NullMsg)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU32(NULL, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorU32Tests, AddVectorU32AfterInit)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU32(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadAddVectorU32(m_msg, NULL, 2, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU32(m_msg, NULL, 2, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK , m_status);
    EXPECT_EQ ( 0, ::memcmp( m_in, m_out, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE , m_outSize);

    //freeVector( m_out );
}

TEST_F(PayloadVectorU32Tests, AddVectorU32InvalidName)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU32(m_msg, "Bob", 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadAddVectorU32(m_msg, NULL, 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorU32Tests, AddVectorU32InvalidFid)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU32(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadAddVectorU32(m_msg, NULL, 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

// UpdateVectorU32 test fixtures
// *****************************

TEST_F(PayloadVectorU32Tests, UpdateVectorU32)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU32(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU32(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_u32_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorU32(m_msg, NULL, 1, m_update, VECTOR_UPDATE_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU32(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_update, m_out, (sizeof(mama_u32_t) * VECTOR_UPDATE_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_UPDATE_SIZE, m_outSize);
}

TEST_F(PayloadVectorU32Tests, UpdateVectorU32NullUpdate)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU32(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU32(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_u32_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorU32(m_msg, NULL, 1, NULL, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorU32Tests, UpdateVectorU32NullMessage)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU32(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU32(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_u32_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorU32(NULL, NULL, 1, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorU32Tests, UpdateVectorU32InitByName)
{
    m_status = m_payloadBridge->msgPayloadUpdateVectorU32(m_msg, "Bob", 0, m_update, VECTOR_UPDATE_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU32(m_msg, "Bob", 0, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_update, m_out, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_UPDATE_SIZE, m_outSize);
}

TEST_F(PayloadVectorU32Tests, UpdateVectorU32InvalidName)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU32(m_msg, "Bob", 0, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU32(m_msg, "Bob", 0, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorU32(m_msg, NULL, 0, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorU32Tests, UpdateVectorU32InvalidFid)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU32(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU32(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorU32(m_msg, NULL, 0, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorU32Tests, UpdateVectorU32InvalidFidNo)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU32(m_msg, "Bob", 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadUpdateVectorU32(m_msg, "Bob", 1, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
}

// GetVectorU32 test fixtures
// **************************

TEST_F(PayloadVectorU32Tests, GetVectorU32)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU32(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU32(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, memcmp( m_in, m_out, (sizeof(mama_u32_t) * VECTOR_SIZE) ) );
    EXPECT_NE(0, memcmp( m_update, m_out, (sizeof(mama_u32_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorU32(m_msg, NULL, 1, m_update, VECTOR_UPDATE_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU32(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, memcmp( m_update, m_out, (sizeof(mama_u32_t) * VECTOR_UPDATE_SIZE) ) );
    EXPECT_NE(0, memcmp( m_in, m_out, (sizeof(mama_u32_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_UPDATE_SIZE, m_outSize);
    EXPECT_NE ((mama_size_t)VECTOR_SIZE, m_outSize);
}

TEST_F(PayloadVectorU32Tests, GetVectorU32NullResult)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU32(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU32(m_msg, NULL, 1, NULL, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorU32Tests, GetVectorU32NullSize)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU32(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU32(m_msg, NULL, 1, &m_out, NULL);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorU32Tests, GetVectorU32NotFound)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU32(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU32(m_msg, NULL, 2, &m_out, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_NOT_FOUND, m_status);
}

// ****************************************************************************
// I64 Test Suite
// ****************************************************************************

class PayloadVectorI64Tests : public PayloadVectorTests
{
public:

    virtual ~PayloadVectorI64Tests()
    {}

protected:

    mama_i64_t m_in[ VECTOR_SIZE ];
    mama_i64_t m_update[ VECTOR_UPDATE_SIZE ];
    const mama_i64_t *m_out;
    mama_size_t m_outSize;

    PayloadVectorI64Tests()
        : m_in( )
        , m_update( )
        , m_out( NULL )
        , m_outSize( 0 )
    {
    }
    
    virtual void SetUp()
    {
        PayloadVectorTests::SetUp();

        for( uint32_t ii(0) ; ii < VECTOR_SIZE ; ++ii )
        {
            m_in[ii] = ii;
            m_update[ii] = 100 + ii;
        }
    }

    virtual void TearDown()
    {
        PayloadVectorTests::TearDown();
    }
};


// AddVectorI64 test fixtures
// **************************

TEST_F(PayloadVectorI64Tests, AddVectorI64)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI64(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI64(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK , m_status);
    EXPECT_EQ ( 0, ::memcmp( m_in, m_out, (sizeof(mama_i64_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE , m_outSize);
}

TEST_F(PayloadVectorI64Tests, AddVectorI64NullI64)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI64(m_msg, NULL, 1, NULL, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG , m_status);
}

TEST_F(PayloadVectorI64Tests, AddVectorI64NullMsg)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI64(NULL, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorI64Tests, AddVectorI64AfterInit)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI64(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadAddVectorI64(m_msg, NULL, 2, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI64(m_msg, NULL, 2, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK , m_status);
    EXPECT_EQ ( 0, ::memcmp( m_in, m_out, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE , m_outSize);

    //freeVector( m_out );
}

TEST_F(PayloadVectorI64Tests, AddVectorI64InvalidName)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI64(m_msg, "Bob", 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadAddVectorI64(m_msg, NULL, 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorI64Tests, AddVectorI64InvalidFid)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI64(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadAddVectorI64(m_msg, NULL, 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

// UpdateVectorI64 test fixtures
// *****************************

TEST_F(PayloadVectorI64Tests, UpdateVectorI64)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI64(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI64(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_i64_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorI64(m_msg, NULL, 1, m_update, VECTOR_UPDATE_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI64(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_update, m_out, (sizeof(mama_i64_t) * VECTOR_UPDATE_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_UPDATE_SIZE, m_outSize);
}

TEST_F(PayloadVectorI64Tests, UpdateVectorI64NullUpdate)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI64(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI64(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_i64_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorI64(m_msg, NULL, 1, NULL, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorI64Tests, UpdateVectorI64NullMessage)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI64(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI64(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_i64_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorI64(NULL, NULL, 1, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorI64Tests, UpdateVectorI64InitByName)
{
    m_status = m_payloadBridge->msgPayloadUpdateVectorI64(m_msg, "Bob", 0, m_update, VECTOR_UPDATE_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI64(m_msg, "Bob", 0, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_update, m_out, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_UPDATE_SIZE, m_outSize);
}

TEST_F(PayloadVectorI64Tests, UpdateVectorI64InvalidName)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI64(m_msg, "Bob", 0, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI64(m_msg, "Bob", 0, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorI64(m_msg, NULL, 0, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorI64Tests, UpdateVectorI64InvalidFid)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI64(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI64(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorI64(m_msg, NULL, 0, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorI64Tests, UpdateVectorI64InvalidFidNo)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI64(m_msg, "Bob", 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadUpdateVectorI64(m_msg, "Bob", 1, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
}

// GetVectorI64 test fixtures
// **************************

TEST_F(PayloadVectorI64Tests, GetVectorI64)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI64(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI64(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, memcmp( m_in, m_out, (sizeof(mama_i64_t) * VECTOR_SIZE) ) );
    EXPECT_NE(0, memcmp( m_update, m_out, (sizeof(mama_i64_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorI64(m_msg, NULL, 1, m_update, VECTOR_UPDATE_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI64(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, memcmp( m_update, m_out, (sizeof(mama_i64_t) * VECTOR_UPDATE_SIZE) ) );
    EXPECT_NE(0, memcmp( m_in, m_out, (sizeof(mama_i64_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_UPDATE_SIZE, m_outSize);
}

TEST_F(PayloadVectorI64Tests, GetVectorI64NullResult)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI64(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI64(m_msg, NULL, 1, NULL, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorI64Tests, GetVectorI64NullSize)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI64(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI64(m_msg, NULL, 1, &m_out, NULL);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorI64Tests, GetVectorI64NotFound)
{
    m_status = m_payloadBridge->msgPayloadAddVectorI64(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorI64(m_msg, NULL, 2, &m_out, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_NOT_FOUND, m_status);
}

// ****************************************************************************
// U64 Test Suite
// ****************************************************************************

class PayloadVectorU64Tests : public PayloadVectorTests
{
public:

    virtual ~PayloadVectorU64Tests()
    {}

protected:

    mama_u64_t m_in[ VECTOR_SIZE ];
    mama_u64_t m_update[ VECTOR_UPDATE_SIZE ];
    const mama_u64_t *m_out;
    mama_size_t m_outSize;

    PayloadVectorU64Tests()
        : m_in( )
        , m_update( )
        , m_out( NULL )
        , m_outSize( 0 )
    {
    }
    
    virtual void SetUp()
    {
        PayloadVectorTests::SetUp();

        for( uint32_t ii(0) ; ii < VECTOR_SIZE ; ++ii )
        {
            m_in[ii] = ii;
            m_update[ii] = 100 + ii;
        }
    }

    virtual void TearDown()
    {
        PayloadVectorTests::TearDown();
    }
};


// AddVectorU64 test fixtures
// **************************

TEST_F(PayloadVectorU64Tests, AddVectorU64)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU64(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU64(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK , m_status);
    EXPECT_EQ ( 0, ::memcmp( m_in, m_out, (sizeof(mama_u64_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE , m_outSize);
}

TEST_F(PayloadVectorU64Tests, AddVectorU64NullU64)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU64(m_msg, NULL, 1, NULL, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG , m_status);
}

TEST_F(PayloadVectorU64Tests, AddVectorU64NullMsg)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU64(NULL, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorU64Tests, AddVectorU64AfterInit)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU64(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadAddVectorU64(m_msg, NULL, 2, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU64(m_msg, NULL, 2, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK , m_status);
    EXPECT_EQ ( 0, ::memcmp( m_in, m_out, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE , m_outSize);

    //freeVector( m_out );
}

TEST_F(PayloadVectorU64Tests, AddVectorU64InvalidName)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU64(m_msg, "Bob", 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadAddVectorU64(m_msg, NULL, 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorU64Tests, AddVectorU64InvalidFid)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU64(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadAddVectorU64(m_msg, NULL, 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

// UpdateVectorU64 test fixtures
// *****************************

TEST_F(PayloadVectorU64Tests, UpdateVectorU64)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU64(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU64(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_u64_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorU64(m_msg, NULL, 1, m_update, VECTOR_UPDATE_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU64(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_update, m_out, (sizeof(mama_u64_t) * VECTOR_UPDATE_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_UPDATE_SIZE, m_outSize);
}

TEST_F(PayloadVectorU64Tests, UpdateVectorU64NullUpdate)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU64(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU64(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_u64_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorU64(m_msg, NULL, 1, NULL, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorU64Tests, UpdateVectorU64NullMessage)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU64(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU64(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_u64_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorU64(NULL, NULL, 1, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorU64Tests, UpdateVectorU64InitByName)
{
    m_status = m_payloadBridge->msgPayloadUpdateVectorU64(m_msg, "Bob", 0, m_update, VECTOR_UPDATE_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU64(m_msg, "Bob", 0, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_update, m_out, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_UPDATE_SIZE, m_outSize);
}

TEST_F(PayloadVectorU64Tests, UpdateVectorU64InvalidName)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU64(m_msg, "Bob", 0, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU64(m_msg, "Bob", 0, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorU64(m_msg, NULL, 0, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorU64Tests, UpdateVectorU64InvalidFid)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU64(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU64(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorU64(m_msg, NULL, 0, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorU64Tests, UpdateVectorU64InvalidFidNo)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU64(m_msg, "Bob", 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadUpdateVectorU64(m_msg, "Bob", 1, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
}

// GetVectorU64 test fixtures
// **************************

TEST_F(PayloadVectorU64Tests, GetVectorU64)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU64(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU64(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, memcmp( m_in, m_out, (sizeof(mama_u64_t) * VECTOR_SIZE) ) );
    EXPECT_NE(0, memcmp( m_update, m_out, (sizeof(mama_u64_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorU64(m_msg, NULL, 1, m_update, VECTOR_UPDATE_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU64(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, memcmp( m_update, m_out, (sizeof(mama_u64_t) * VECTOR_UPDATE_SIZE) ) );
    EXPECT_NE(0, memcmp( m_in, m_out, (sizeof(mama_u64_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_UPDATE_SIZE, m_outSize);
    EXPECT_NE ((mama_size_t)VECTOR_SIZE, m_outSize);
}

TEST_F(PayloadVectorU64Tests, GetVectorU64NullResult)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU64(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU64(m_msg, NULL, 1, NULL, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorU64Tests, GetVectorU64NullSize)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU64(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU64(m_msg, NULL, 1, &m_out, NULL);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorU64Tests, GetVectorU64NotFound)
{
    m_status = m_payloadBridge->msgPayloadAddVectorU64(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorU64(m_msg, NULL, 2, &m_out, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_NOT_FOUND, m_status);
}

// ****************************************************************************
// F32 Test Suite
// ****************************************************************************

class PayloadVectorF32Tests : public PayloadVectorTests
{
public:

    virtual ~PayloadVectorF32Tests()
    {}

protected:

    mama_f32_t m_in[ VECTOR_SIZE ];
    mama_f32_t m_update[ VECTOR_UPDATE_SIZE ];
    mama_f32_t m_pi;
    mama_f32_t m_e;
    const mama_f32_t *m_out;
    mama_size_t m_outSize;

    PayloadVectorF32Tests()
        : m_in( )
        , m_update( )
        , m_pi( 3.14159265 )
        , m_e( 2.71828183 )
        , m_out( NULL )
        , m_outSize( 0 )
    {
    }
    
    virtual void SetUp()
    {
        PayloadVectorTests::SetUp();

        for( uint32_t ii(0) ; ii < VECTOR_SIZE ; ++ii )
        {
            m_in[ii] =  m_pi + (mama_f32_t) ii;
            m_update[ii] = m_e + (mama_f32_t) ii;
        }
    }

    virtual void TearDown()
    {
        PayloadVectorTests::TearDown();
    }
};


// AddVectorF32 test fixtures
// **************************

TEST_F(PayloadVectorF32Tests, AddVectorF32)
{
    m_status = m_payloadBridge->msgPayloadAddVectorF32(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorF32(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK , m_status);
    EXPECT_EQ ( 0, ::memcmp( m_in, m_out, (sizeof(mama_f32_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE , m_outSize);
}

TEST_F(PayloadVectorF32Tests, AddVectorF32NullF32)
{
    m_status = m_payloadBridge->msgPayloadAddVectorF32(m_msg, NULL, 1, NULL, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG , m_status);
}

TEST_F(PayloadVectorF32Tests, AddVectorF32NullMsg)
{
    m_status = m_payloadBridge->msgPayloadAddVectorF32(NULL, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorF32Tests, AddVectorF32AfterInit)
{
    m_status = m_payloadBridge->msgPayloadAddVectorF32(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadAddVectorF32(m_msg, NULL, 2, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorF32(m_msg, NULL, 2, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK , m_status);
    EXPECT_EQ ( 0, ::memcmp( m_in, m_out, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE , m_outSize);

    //freeVector( m_out );
}

TEST_F(PayloadVectorF32Tests, AddVectorF32InvalidName)
{
    m_status = m_payloadBridge->msgPayloadAddVectorF32(m_msg, "Bob", 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadAddVectorF32(m_msg, NULL, 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorF32Tests, AddVectorF32InvalidFid)
{
    m_status = m_payloadBridge->msgPayloadAddVectorF32(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadAddVectorF32(m_msg, NULL, 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

// UpdateVectorF32 test fixtures
// *****************************

TEST_F(PayloadVectorF32Tests, UpdateVectorF32)
{
    m_status = m_payloadBridge->msgPayloadAddVectorF32(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorF32(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_f32_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorF32(m_msg, NULL, 1, m_update, VECTOR_UPDATE_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorF32(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_update, m_out, (sizeof(mama_f32_t) * VECTOR_UPDATE_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_UPDATE_SIZE, m_outSize);
}

TEST_F(PayloadVectorF32Tests, UpdateVectorF32NullUpdate)
{
    m_status = m_payloadBridge->msgPayloadAddVectorF32(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorF32(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_f32_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorF32(m_msg, NULL, 1, NULL, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorF32Tests, UpdateVectorF32NullMessage)
{
    m_status = m_payloadBridge->msgPayloadAddVectorF32(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorF32(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_f32_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorF32(NULL, NULL, 1, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorF32Tests, UpdateVectorF32InitByName)
{
    m_status = m_payloadBridge->msgPayloadUpdateVectorF32(m_msg, "Bob", 0, m_update, VECTOR_UPDATE_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorF32(m_msg, "Bob", 0, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_update, m_out, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_UPDATE_SIZE, m_outSize);
}

TEST_F(PayloadVectorF32Tests, UpdateVectorF32InvalidName)
{
    m_status = m_payloadBridge->msgPayloadAddVectorF32(m_msg, "Bob", 0, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorF32(m_msg, "Bob", 0, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorF32(m_msg, NULL, 0, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorF32Tests, UpdateVectorF32InvalidFid)
{
    m_status = m_payloadBridge->msgPayloadAddVectorF32(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorF32(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorF32(m_msg, NULL, 0, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorF32Tests, UpdateVectorF32InvalidFidNo)
{
    m_status = m_payloadBridge->msgPayloadAddVectorF32(m_msg, "Bob", 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadUpdateVectorF32(m_msg, "Bob", 1, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
}

// GetVectorF32 test fixtures
// **************************

TEST_F(PayloadVectorF32Tests, GetVectorF32)
{
    m_status = m_payloadBridge->msgPayloadAddVectorF32(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorF32(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, memcmp( m_in, m_out, (sizeof(mama_f32_t) * VECTOR_SIZE) ) );
    EXPECT_NE(0, memcmp( m_update, m_out, (sizeof(mama_f32_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorF32(m_msg, NULL, 1, m_update, VECTOR_UPDATE_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorF32(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, memcmp( m_update, m_out, (sizeof(mama_f32_t) * VECTOR_UPDATE_SIZE) ) );
    EXPECT_NE(0, memcmp( m_in, m_out, (sizeof(mama_f32_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_UPDATE_SIZE, m_outSize);
    EXPECT_NE ((mama_size_t)VECTOR_SIZE, m_outSize);
}

TEST_F(PayloadVectorF32Tests, GetVectorF32NullResult)
{
    m_status = m_payloadBridge->msgPayloadAddVectorF32(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorF32(m_msg, NULL, 1, NULL, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorF32Tests, GetVectorF32NullSize)
{
    m_status = m_payloadBridge->msgPayloadAddVectorF32(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorF32(m_msg, NULL, 1, &m_out, NULL);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorF32Tests, GetVectorF32NotFound)
{
    m_status = m_payloadBridge->msgPayloadAddVectorF32(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorF32(m_msg, NULL, 2, &m_out, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_NOT_FOUND, m_status);
}

// ****************************************************************************
// F64 Test Suite
// ****************************************************************************

class PayloadVectorF64Tests : public PayloadVectorTests
{
public:

    virtual ~PayloadVectorF64Tests()
    {}

protected:

    mama_f64_t m_in[ VECTOR_SIZE ];
    mama_f64_t m_update[ VECTOR_UPDATE_SIZE ];
    mama_f64_t m_pi;
    mama_f64_t m_e;
    const mama_f64_t *m_out;
    mama_size_t m_outSize;

    PayloadVectorF64Tests()
        : m_in( )
        , m_update( )
        , m_pi( 3.14159265 )
        , m_e( 2.71828183 )
        , m_out( NULL )
        , m_outSize( 0 )
    {
    }
    
    virtual void SetUp()
    {
        PayloadVectorTests::SetUp();

        for( uint32_t ii(0) ; ii < VECTOR_SIZE ; ++ii )
        {
            m_in[ii] =  m_pi + (mama_f64_t) ii;
            m_update[ii] = m_e + (mama_f64_t) ii;
        }
    }

    virtual void TearDown()
    {
        PayloadVectorTests::TearDown();
    }
};


// AddVectorF64 test fixtures
// **************************

TEST_F(PayloadVectorF64Tests, AddVectorF64)
{
    m_status = m_payloadBridge->msgPayloadAddVectorF64(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorF64(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK , m_status);
    EXPECT_EQ ( 0, ::memcmp( m_in, m_out, (sizeof(mama_f64_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE , m_outSize);
}

TEST_F(PayloadVectorF64Tests, AddVectorF64NullF64)
{
    m_status = m_payloadBridge->msgPayloadAddVectorF64(m_msg, NULL, 1, NULL, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG , m_status);
}

TEST_F(PayloadVectorF64Tests, AddVectorF64NullMsg)
{
    m_status = m_payloadBridge->msgPayloadAddVectorF64(NULL, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorF64Tests, AddVectorF64AfterInit)
{
    m_status = m_payloadBridge->msgPayloadAddVectorF64(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadAddVectorF64(m_msg, NULL, 2, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorF64(m_msg, NULL, 2, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK , m_status);
    EXPECT_EQ ( 0, ::memcmp( m_in, m_out, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE , m_outSize);

    //freeVector( m_out );
}

TEST_F(PayloadVectorF64Tests, AddVectorF64InvalidName)
{
    m_status = m_payloadBridge->msgPayloadAddVectorF64(m_msg, "Bob", 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadAddVectorF64(m_msg, NULL, 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorF64Tests, AddVectorF64InvalidFid)
{
    m_status = m_payloadBridge->msgPayloadAddVectorF64(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadAddVectorF64(m_msg, NULL, 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

// UpdateVectorF64 test fixtures
// *****************************

TEST_F(PayloadVectorF64Tests, UpdateVectorF64)
{
    m_status = m_payloadBridge->msgPayloadAddVectorF64(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorF64(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_f64_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorF64(m_msg, NULL, 1, m_update, VECTOR_UPDATE_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorF64(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_update, m_out, (sizeof(mama_f64_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_UPDATE_SIZE, m_outSize);
}

TEST_F(PayloadVectorF64Tests, UpdateVectorF64NullUpdate)
{
    m_status = m_payloadBridge->msgPayloadAddVectorF64(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorF64(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_f64_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorF64(m_msg, NULL, 1, NULL, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorF64Tests, UpdateVectorF64NullMessage)
{
    m_status = m_payloadBridge->msgPayloadAddVectorF64(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorF64(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_f64_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorF64(NULL, NULL, 1, m_update, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorF64Tests, UpdateVectorF64InitByName)
{
    m_status = m_payloadBridge->msgPayloadUpdateVectorF64(m_msg, "Bob", 0, m_update, VECTOR_UPDATE_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorF64(m_msg, "Bob", 0, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_update, m_out, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_UPDATE_SIZE, m_outSize);
}

TEST_F(PayloadVectorF64Tests, UpdateVectorF64InvalidName)
{
    m_status = m_payloadBridge->msgPayloadAddVectorF64(m_msg, "Bob", 0, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorF64(m_msg, "Bob", 0, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorF64(m_msg, NULL, 0, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorF64Tests, UpdateVectorF64InvalidFid)
{
    m_status = m_payloadBridge->msgPayloadAddVectorF64(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorF64(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, ::memcmp( m_in, m_out, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorF64(m_msg, NULL, 0, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorF64Tests, UpdateVectorF64InvalidFidNo)
{
    m_status = m_payloadBridge->msgPayloadAddVectorF64(m_msg, "Bob", 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadUpdateVectorF64(m_msg, "Bob", 1, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
}

// GetVectorF64 test fixtures
// **************************

TEST_F(PayloadVectorF64Tests, GetVectorF64)
{
    m_status = m_payloadBridge->msgPayloadAddVectorF64(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorF64(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, memcmp( m_in, m_out, (sizeof(mama_f64_t) * VECTOR_SIZE) ) );
    EXPECT_NE(0, memcmp( m_update, m_out, (sizeof(mama_f64_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorF64(m_msg, NULL, 1, m_update, VECTOR_UPDATE_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorF64(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (0, memcmp( m_update, m_out, (sizeof(mama_f64_t) * VECTOR_UPDATE_SIZE) ) );
    EXPECT_NE(0, memcmp( m_in, m_out, (sizeof(mama_f64_t) * VECTOR_SIZE) ) );
    EXPECT_EQ ((mama_size_t)VECTOR_UPDATE_SIZE, m_outSize);
    EXPECT_NE ((mama_size_t)VECTOR_SIZE, m_outSize);
}

TEST_F(PayloadVectorF64Tests, GetVectorF64NullResult)
{
    m_status = m_payloadBridge->msgPayloadAddVectorF64(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorF64(m_msg, NULL, 1, NULL, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorF64Tests, GetVectorF64NullSize)
{
    m_status = m_payloadBridge->msgPayloadAddVectorF64(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorF64(m_msg, NULL, 1, &m_out, NULL);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorF64Tests, GetVectorF64NotFound)
{
    m_status = m_payloadBridge->msgPayloadAddVectorF64(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorF64(m_msg, NULL, 2, &m_out, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_NOT_FOUND, m_status);
}

// ****************************************************************************
// String Test Suite
// ****************************************************************************

class PayloadVectorStringTests : public PayloadVectorTests
{
public:

    virtual ~PayloadVectorStringTests()
    {}

protected:

    const char * m_in[ VECTOR_SIZE ];
    const char * m_update[ VECTOR_UPDATE_SIZE ];
    const char ** m_out;
    mama_size_t m_outSize;

    PayloadVectorStringTests()
        : m_in( )
        , m_update( )
        , m_out( NULL )
        , m_outSize( 0 )
    {
    }
    
    virtual void SetUp()
    {
        PayloadVectorTests::SetUp();

        for( uint32_t ii(0) ; ii < VECTOR_SIZE ; ++ii )
        {
            char buffer[20];
            ::snprintf(buffer, 20, "InitialString%u", ii);
            m_in[ii] = ::strndup( buffer, 20 );
        }

        for( uint32_t ii(0) ; ii < VECTOR_UPDATE_SIZE ; ++ii )
        {
            char buffer[20];
            ::snprintf(buffer, 20, "UpdateString%u", ii);
            m_update[ii] = ::strndup( buffer, 20 );
        }
    }

    virtual void TearDown()
    {
        for( uint32_t ii(0) ; ii < VECTOR_SIZE ; ++ii )
        {
            free( (void *) m_in[ii] ); m_in[ii] = NULL;
        }

        for( uint32_t ii(0) ; ii < VECTOR_UPDATE_SIZE ; ++ii )
        {
            free( (void *) m_update[ii] ); m_update[ii] = NULL;
        }

        PayloadVectorTests::TearDown();
    }
};


// AddVectorString test fixtures
// *****************************

TEST_F(PayloadVectorStringTests, AddVectorString)
{
    m_status = m_payloadBridge->msgPayloadAddVectorString(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorString(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK , m_status);

    int getResult(0);
    for( unsigned int ii( 0 ) ; ((ii < VECTOR_SIZE) && (0 == getResult)) ; ++ii )
    {
        getResult = ::strncmp( m_in[ii], m_out[ii], 20 );
    }

    EXPECT_EQ ( 0, getResult );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE , m_outSize);
}

TEST_F(PayloadVectorStringTests, AddVectorStringNullString)
{
    m_status = m_payloadBridge->msgPayloadAddVectorString(m_msg, NULL, 1, NULL, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG , m_status);
}

TEST_F(PayloadVectorStringTests, AddVectorStringNullMsg)
{
    m_status = m_payloadBridge->msgPayloadAddVectorString(NULL, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorStringTests, AddVectorStringAfterInit)
{
    m_status = m_payloadBridge->msgPayloadAddVectorString(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadAddVectorString(m_msg, NULL, 2, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorString(m_msg, NULL, 2, &m_out, &m_outSize);

    int getResult(0);
    for( unsigned int ii( 0 ) ; ((ii < VECTOR_SIZE) && (0 == getResult)) ; ++ii )
    {
        getResult = ::strncmp( m_in[ii], m_out[ii], 20 );
    }

    EXPECT_EQ ( 0, getResult );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE , m_outSize);
}

TEST_F(PayloadVectorStringTests, AddVectorStringInvalidName)
{
    m_status = m_payloadBridge->msgPayloadAddVectorString(m_msg, "Bob", 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadAddVectorString(m_msg, NULL, 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorStringTests, AddVectorStringInvalidFid)
{
    m_status = m_payloadBridge->msgPayloadAddVectorString(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadAddVectorString(m_msg, NULL, 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

// UpdateVectorString test fixtures
// ********************************

TEST_F(PayloadVectorStringTests, UpdateVectorString)
{
    m_status = m_payloadBridge->msgPayloadAddVectorString(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorString(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    int getResult(0);
    for( unsigned int ii( 0 ) ; ((ii < VECTOR_SIZE) && (0 == getResult)) ; ++ii )
    {
        getResult = ::strncmp( m_in[ii], m_out[ii], 20 );
    }

    EXPECT_EQ ( 0, getResult );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorString(m_msg, NULL, 1, m_update, VECTOR_UPDATE_SIZE);
    ASSERT_EQ(MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorString(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    getResult = 0;
    for( unsigned int ii( 0 ) ; ((ii < VECTOR_UPDATE_SIZE) && (0 == getResult)) ; ++ii )
    {
        getResult = ::strncmp( m_update[ii], m_out[ii], 20 );
    }

    EXPECT_EQ (0, getResult);
    EXPECT_EQ ((mama_size_t)VECTOR_UPDATE_SIZE, m_outSize);
}

TEST_F(PayloadVectorStringTests, UpdateVectorStringNullUpdate)
{
    m_status = m_payloadBridge->msgPayloadAddVectorString(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorString(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    int getResult(0);
    for( unsigned int ii( 0 ) ; ((ii < VECTOR_SIZE) && (0 == getResult)) ; ++ii )
    {
        getResult = ::strncmp( m_in[ii], m_out[ii], 20 );
    }

    EXPECT_EQ ( 0, getResult );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorString(m_msg, NULL, 1, NULL, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorStringTests, UpdateVectorStringNullMessage)
{
    m_status = m_payloadBridge->msgPayloadAddVectorString(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorString(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    int getResult(0);
    for( unsigned int ii( 0 ) ; ((ii < VECTOR_SIZE) && (0 == getResult)) ; ++ii )
    {
        getResult = ::strncmp( m_in[ii], m_out[ii], 20 );
    }

    EXPECT_EQ ( 0, getResult );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorString(NULL, NULL, 1, m_update, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorStringTests, UpdateVectorStringInitByName)
{
    m_status = m_payloadBridge->msgPayloadUpdateVectorString(m_msg, "Bob", 0, m_update, VECTOR_UPDATE_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorString(m_msg, "Bob", 0, &m_out, &m_outSize);

    int getResult(0);
    for( unsigned int ii( 0 ) ; ((ii < VECTOR_UPDATE_SIZE) && (0 == getResult)) ; ++ii )
    {
        getResult = ::strncmp( m_update[ii], m_out[ii], 20 );
    }

    EXPECT_EQ ( 0, getResult );
    EXPECT_EQ ((mama_size_t)VECTOR_UPDATE_SIZE, m_outSize);
}

TEST_F(PayloadVectorStringTests, UpdateVectorStringInvalidName)
{
    m_status = m_payloadBridge->msgPayloadAddVectorString(m_msg, "Bob", 0, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorString(m_msg, "Bob", 0, &m_out, &m_outSize);
    int getResult(0);
    for( unsigned int ii( 0 ) ; ((ii < VECTOR_SIZE) && (0 == getResult)) ; ++ii )
    {
        getResult = ::strncmp( m_in[ii], m_out[ii], 20 );
    }

    EXPECT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ ( 0, getResult );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorString(m_msg, NULL, 0, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorStringTests, UpdateVectorStringInvalidFid)
{
    m_status = m_payloadBridge->msgPayloadAddVectorString(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorString(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    int getResult(0);
    for( unsigned int ii( 0 ) ; ((ii < VECTOR_SIZE) && (0 == getResult)) ; ++ii )
    {
        getResult = ::strncmp( m_in[ii], m_out[ii], 20 );
    }

    EXPECT_EQ ( 0, getResult );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorString(m_msg, NULL, 0, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorStringTests, UpdateVectorStringInvalidFidNo)
{
    m_status = m_payloadBridge->msgPayloadAddVectorString(m_msg, "Bob", 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadUpdateVectorString(m_msg, "Bob", 1, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
}

// GetVectorString test fixtures
// *****************************

TEST_F(PayloadVectorStringTests, GetVectorString)
{
    m_status = m_payloadBridge->msgPayloadAddVectorString(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorString(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    int getResult(0);
    for( unsigned int ii( 0 ) ; ((ii < VECTOR_SIZE) && (0 == getResult)) ; ++ii )
    {
        getResult = ::strncmp( m_in[ii], m_out[ii], 20 );
    }

    EXPECT_EQ ( 0, getResult );
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE , m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorString(m_msg, NULL, 1, m_update, VECTOR_UPDATE_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorString(m_msg, NULL, 1, &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    getResult = 0;
    for( unsigned int ii( 0 ) ; ((ii < VECTOR_UPDATE_SIZE) && (0 == getResult)) ; ++ii )
    {
        getResult = ::strncmp( m_update[ii], m_out[ii], 20 );
    }

    EXPECT_EQ (0, getResult);
    EXPECT_EQ ((mama_size_t)VECTOR_UPDATE_SIZE, m_outSize);
    EXPECT_NE ((mama_size_t)VECTOR_SIZE, m_outSize);
}

TEST_F(PayloadVectorStringTests, GetVectorStringNullResult)
{
    m_status = m_payloadBridge->msgPayloadAddVectorString(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorString(m_msg, NULL, 1, NULL, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorStringTests, GetVectorStringNullSize)
{
    m_status = m_payloadBridge->msgPayloadAddVectorString(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorString(m_msg, NULL, 1, &m_out, NULL);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorStringTests, GetVectorStringNotFound)
{
    m_status = m_payloadBridge->msgPayloadAddVectorString(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorString(m_msg, NULL, 2, &m_out, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_NOT_FOUND, m_status);
}

// ****************************************************************************
// Msg Test Suite
// ****************************************************************************

class PayloadVectorMsgTests : public PayloadVectorTests
{
public:

    virtual ~PayloadVectorMsgTests()
    {}

protected:

    mamaMsg m_in[ VECTOR_SIZE ];
    mamaMsg m_update[ VECTOR_UPDATE_SIZE ];
    const mamaMsg * m_out;
    mama_size_t m_outSize;

    PayloadVectorMsgTests()
        : m_in( )
        , m_update( )
        , m_out( NULL )
        , m_outSize( 0 )
    {
    }
    
    virtual void SetUp()
    {
        PayloadVectorTests::SetUp();

        for( uint32_t ii(0) ; ii < VECTOR_SIZE ; ++ii )
        {
            char symbol[ MAX_SUBJECT ];
            ::snprintf( symbol, MAX_SUBJECT, "InSymbol%d", ii);

            mamaMsg_create( &m_in[ii] );
            mamaMsgImpl_setSubscInfo ( m_in[ii],
                                       "_MD",
                                       "InSource",
                                       symbol,
                                       0 );
            mamaMsgImpl_setStatus ( m_in[ii],
                                    MAMA_MSG_STATUS_OK );
        }

        for( uint32_t ii(0) ; ii < VECTOR_UPDATE_SIZE ; ++ii )
        {
            char symbol[ MAX_SUBJECT ];
            ::snprintf( symbol, MAX_SUBJECT, "UpdateSymbol%d", ii);

            mamaMsg_create( &m_update[ii] );
            mamaMsgImpl_setSubscInfo ( m_update[ii],
                                       "_MD",
                                       "UpdateSource",
                                       symbol,
                                       1 );
            mamaMsgImpl_setStatus ( m_update[ii],
                                    MAMA_MSG_STATUS_LINE_DOWN );
        }
    }

    virtual void TearDown()
    {
        for( uint32_t ii(0) ; ii < VECTOR_SIZE ; ++ii )
        {
            mamaMsg_destroy( m_in[ii] ); m_in[ii] = NULL;
        }

        for( uint32_t ii(0) ; ii < VECTOR_UPDATE_SIZE ; ++ii )
        {
            mamaMsg_destroy( m_update[ii] ); m_update[ii] = NULL;
        }

        PayloadVectorTests::TearDown();
    }
};

// AddVectorMsg test fixtures
// **************************

TEST_F(PayloadVectorMsgTests, AddVectorMsg)
{
    m_status = m_payloadBridge->msgPayloadAddVectorMsg(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorMsg(m_msg,
                                                       NULL,
                                                       1,
                                                       (void* const**) &m_out,
                                                       &m_outSize);
    EXPECT_EQ (MAMA_STATUS_OK , m_status);
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE , m_outSize);
}

TEST_F(PayloadVectorMsgTests, AddVectorMsgNullMsgDst)
{
    m_status = m_payloadBridge->msgPayloadAddVectorMsg(m_msg, NULL, 1, NULL, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG , m_status);
}

TEST_F(PayloadVectorMsgTests, AddVectorMsgNullMsg)
{
    m_status = m_payloadBridge->msgPayloadAddVectorMsg(NULL, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorMsgTests, AddVectorMsgAfterInit)
{
    m_status = m_payloadBridge->msgPayloadAddVectorMsg(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadAddVectorMsg(m_msg, NULL, 2, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorMsg(m_msg,
                                                       NULL,
                                                       1,
                                                       (void* const**) &m_out,
                                                       &m_outSize);
    EXPECT_EQ (MAMA_STATUS_OK , m_status);
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE , m_outSize);
}

TEST_F(PayloadVectorMsgTests, AddVectorMsgInvalidName)
{
    m_status = m_payloadBridge->msgPayloadAddVectorMsg(m_msg, "Bob", 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadAddVectorMsg(m_msg, NULL, 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorMsgTests, AddVectorMsgInvalidFid)
{
    m_status = m_payloadBridge->msgPayloadAddVectorMsg(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadAddVectorMsg(m_msg, NULL, 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

// UpdateVectorMsg test fixtures
// *****************************

TEST_F(PayloadVectorMsgTests, UpdateVectorMsg)
{
    m_status = m_payloadBridge->msgPayloadAddVectorMsg(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadUpdateVectorMsg(m_msg, NULL, 1, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorMsg(m_msg, NULL, 1, (void* const**) &m_out, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ ((mama_size_t)VECTOR_UPDATE_SIZE, m_outSize);
}

TEST_F(PayloadVectorMsgTests, UpdateVectorMsgNullUpdate)
{
    m_status = m_payloadBridge->msgPayloadAddVectorMsg(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadUpdateVectorMsg(m_msg, NULL, 1, NULL, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorMsgTests, UpdateVectorMsgNullMessage)
{
    m_status = m_payloadBridge->msgPayloadAddVectorMsg(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadUpdateVectorMsg(NULL, NULL, 1, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorMsgTests, UpdateVectorMsgInitByName)
{
    m_status = m_payloadBridge->msgPayloadUpdateVectorMsg(m_msg, "Bob", 0, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorMsg(m_msg, "Bob", 0, (void* const**) &m_out, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ ((mama_size_t)VECTOR_UPDATE_SIZE, m_outSize);
}

TEST_F(PayloadVectorMsgTests, UpdateVectorMsgInvalidName)
{
    m_status = m_payloadBridge->msgPayloadAddVectorMsg(m_msg, "Bob", 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorMsg(m_msg, "Bob", 0, (void* const**) &m_out, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorMsg(m_msg, NULL, 0, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorMsgTests, UpdateVectorMsgInvalidFid)
{
    m_status = m_payloadBridge->msgPayloadAddVectorMsg(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorMsg(m_msg, NULL, 1, (void* const**) &m_out, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorMsg(m_msg, NULL, 0, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorMsgTests, UpdateVectorMsgInvalidFidNo)
{
    m_status = m_payloadBridge->msgPayloadAddVectorMsg(m_msg, "Bob", 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadUpdateVectorMsg(m_msg, "Bob", 1, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
}

// GetVectorMsg test fixtures
// **************************

TEST_F(PayloadVectorMsgTests, GetVectorMsg)
{
    m_status = m_payloadBridge->msgPayloadAddVectorMsg(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorMsg(m_msg, NULL, 1, (void* const**) &m_out, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE , m_outSize);

    m_status = m_payloadBridge->msgPayloadUpdateVectorMsg(m_msg, NULL, 1, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorMsg(m_msg, NULL, 1, (void* const**) &m_out, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ ((mama_size_t)VECTOR_UPDATE_SIZE, m_outSize);
}

TEST_F(PayloadVectorMsgTests, GetVectorMsgNullResult)
{
    m_status = m_payloadBridge->msgPayloadAddVectorMsg(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorMsg(m_msg, NULL, 1, (void* const**) NULL, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorMsgTests, GetVectorMsgNullSize)
{
    m_status = m_payloadBridge->msgPayloadAddVectorMsg(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorMsg(m_msg, NULL, 1, (void* const**) &m_out, NULL);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorMsgTests, GetVectorMsgNotFound)
{
    m_status = m_payloadBridge->msgPayloadAddVectorMsg(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorMsg(m_msg, NULL, 2, (void* const**) &m_out, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_NOT_FOUND, m_status);
}

// ****************************************************************************
// DateTime Test Suite
// ****************************************************************************

class PayloadVectorDateTimeTests : public PayloadVectorTests
{
public:

    virtual ~PayloadVectorDateTimeTests()
    {}

protected:

    mamaDateTime m_in[ VECTOR_SIZE ];
    mamaDateTime m_update[ VECTOR_UPDATE_SIZE ];
    const mamaDateTime *m_out;
    mama_size_t m_outSize;

    PayloadVectorDateTimeTests()
        : m_in( )
        , m_update( )
        , m_out( )
        , m_outSize( 0 )
    {
    }
    
    virtual void SetUp()
    {
        PayloadVectorTests::SetUp();

        for( uint32_t ii(0) ; ii < VECTOR_SIZE ; ++ii )
        {
            mamaDateTime_create( &m_in[ ii ] );
            mamaDateTime_setToNow( m_in[ ii ] );

            mamaDateTime_addSeconds( m_in[ ii ],
                                     (ii * 10) );
        }

        for( uint32_t ii(0) ; ii < VECTOR_UPDATE_SIZE ; ++ii )
        {
            mamaDateTime_create( &m_update[ ii ] );
            mamaDateTime_setToNow( m_update[ ii ] );

            mamaDateTime_addSeconds( m_update[ ii ],
                                     ((ii + 1) * 24 * 60 * 60) );
        }
    }

    virtual void TearDown()
    {
        for( uint32_t ii(0) ; ii < VECTOR_SIZE ; ++ii )
        {
            mamaDateTime_destroy( m_in[ii] );
        }

        for( uint32_t ii(0) ; ii < VECTOR_UPDATE_SIZE ; ++ii )
        {
            mamaDateTime_destroy( m_update[ii] );
        }

        PayloadVectorTests::TearDown();
    }
};

// AddVectorDateTime test fixtures
// *******************************

TEST_F(PayloadVectorDateTimeTests, DISABLED_AddVectorDateTime)
// Disabled as mamaMsg_addVectorDateTime is not implemented.
{
    m_status = m_payloadBridge->msgPayloadAddVectorDateTime(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorDateTime(m_msg, NULL, 1, (mama_u64_t* const*) &m_out, &m_outSize);
    ASSERT_EQ (MAMA_STATUS_OK , m_status);
    EXPECT_EQ (VECTOR_SIZE , m_outSize);

    for( unsigned int ii(0) ; ii < VECTOR_SIZE ; ++ii )
    {
        char buf[56];
        int eq = mamaDateTime_equal (m_in[ii],
                                     m_out[ii] );
        mamaDateTime_getAsString(m_in[ii], buf, 56);
        mamaDateTime_getAsString(m_out[ii], buf, 56);
        EXPECT_EQ (1, eq);
    }
}

TEST_F(PayloadVectorDateTimeTests, DISABLED_AddVectorDateTimeNullDateTime)
// Disabled as mamaMsg_addVectorDateTime is not implemented.
{
    m_status = m_payloadBridge->msgPayloadAddVectorDateTime(m_msg, NULL, 1, NULL, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG , m_status);
}

TEST_F(PayloadVectorDateTimeTests, DISABLED_AddVectorDateTimeNullMsg)
// Disabled as mamaMsg_addVectorDateTime is not implemented.
{
    m_status = m_payloadBridge->msgPayloadAddVectorDateTime(NULL, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorDateTimeTests, DISABLED_AddVectorDateTimeAfterInit)
// Disabled as mamaMsg_addVectorDateTime is not implemented.
{
    m_status = m_payloadBridge->msgPayloadAddVectorDateTime(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadAddVectorDateTime(m_msg, NULL, 2, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
}

TEST_F(PayloadVectorDateTimeTests, DISABLED_AddVectorDateTimeInvalidName)
// Disabled as mamaMsg_addVectorDateTime is not implemented.
{
    m_status = m_payloadBridge->msgPayloadAddVectorDateTime(m_msg, "Bob", 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadAddVectorDateTime(m_msg, NULL, 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorDateTimeTests, DISABLED_AddVectorDateTimeInvalidFid)
// Disabled as mamaMsg_addVectorDateTime is not implemented.
{
    m_status = m_payloadBridge->msgPayloadAddVectorDateTime(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadAddVectorDateTime(m_msg, NULL, 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

// UpdateVectorDateTime test fixtures
// **********************************
// Disabled as mamaMsg_addVectorDateTime is not implemented.
TEST_F(PayloadVectorDateTimeTests, DISABLED_UpdateVectorDateTime)
{
    m_status = m_payloadBridge->msgPayloadAddVectorDateTime(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    //m_status = m_payloadBridge->msgPayloadGetVectorDateTime(m_msg, NULL, 1, (mama_u64_t* const*) &m_out, &m_outSize);
    //EXPECT_EQ (MAMA_STATUS_OK, m_status);
    //EXPECT_EQ (VECTOR_SIZE, m_outSize);

    /*
    if( MAMA_STATUS_OK == m_status )
    {
        for( unsigned int ii(0) ; ii < VECTOR_SIZE ; ++ii )
        {
            int eq = mamaDateTime_equal( m_in[ii],
                                         m_out[ii] );
            EXPECT_EQ (0, eq);
        }
    }
    else
    {
        FAIL();
    }
    */

    m_status = m_payloadBridge->msgPayloadUpdateVectorTime(m_msg, NULL, 1, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    //m_status = m_payloadBridge->msgPayloadGetVectorDateTime(m_msg, NULL, 1, (mama_u64_t* const*) &m_out, &m_outSize);
    //EXPECT_EQ (MAMA_STATUS_OK, m_status);
    //EXPECT_EQ (VECTOR_UPDATE_SIZE, m_outSize);

    /*
    if( MAMA_STATUS_OK == m_status )
    {
        for( unsigned int ii(0) ; ii < VECTOR_UPDATE_SIZE ; ++ii )
        {
            int eq = mamaDateTime_equal( m_update[ii],
                                         m_out[ii] );
            EXPECT_EQ (0, eq);
        }
    }
    else
    {
        FAIL();
    }
    */
}

TEST_F(PayloadVectorDateTimeTests, DISABLED_UpdateVectorDateTimeNullUpdate)
// Disabled as mamaMsg_addVectorPrice is not implemented.
{
    m_status = m_payloadBridge->msgPayloadAddVectorDateTime(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    //m_status = m_payloadBridge->msgPayloadGetVectorDateTime(m_msg, NULL, 1, (mama_u64_t* const*) &m_out, &m_outSize);
    //EXPECT_EQ (MAMA_STATUS_OK, m_status);
    //EXPECT_EQ (VECTOR_SIZE, m_outSize);

    /*
    if( MAMA_STATUS_OK == m_status )
    {
        for( unsigned int ii(0) ; ii < VECTOR_SIZE ; ++ii )
        {
            int eq = mamaDateTime_equal( m_in[ii],
                                         m_out[ii] );
            EXPECT_EQ (0, eq);
        }
    }
    else
    {
        FAIL();
    }
    */

    m_status = m_payloadBridge->msgPayloadUpdateVectorTime(m_msg, NULL, 1, NULL, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorDateTimeTests, DISABLED_UpdateVectorDateTimeNullMessage)
// Disabled as mamaMsg_addVectorDateTime is not implemented.
{
    m_status = m_payloadBridge->msgPayloadAddVectorDateTime(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    //m_status = m_payloadBridge->msgPayloadGetVectorDateTime(m_msg, NULL, 1, (mama_u64_t* const*) &m_out, &m_outSize);
    //EXPECT_EQ (MAMA_STATUS_OK, m_status);
    //EXPECT_EQ (VECTOR_SIZE, m_outSize);

    /*
    if( MAMA_STATUS_OK == m_status )
    {
        for( unsigned int ii(0) ; ii < VECTOR_SIZE ; ++ii )
        {
            int eq = mamaDateTime_equal( m_in[ii],
                                         m_out[ii] );

            EXPECT_EQ (0, eq);
        }
    }
    else
    {
        FAIL();
    }
    */

    m_status = m_payloadBridge->msgPayloadUpdateVectorTime(NULL, NULL, 1, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

// GetVectorDateTime test fixtures
// *******************************
TEST_F(PayloadVectorDateTimeTests, DISABLED_GetVectorDateTime)
// Disabled as mamaMsg_addVectorDateTime is not implemented.
{
    m_status = m_payloadBridge->msgPayloadAddVectorDateTime(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    // TODO Check prototype for GetVectorDateTime
    m_status = m_payloadBridge->msgPayloadGetVectorDateTime(m_msg, NULL, 1, (mama_u64_t* const*) &m_out, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    if( MAMA_STATUS_OK == m_status )
    {
        for( unsigned int ii(0) ; ii < VECTOR_SIZE ; ++ii )
        {
            int eq = mamaDateTime_equal( m_in[ii],
                                         m_out[ii] );

            EXPECT_EQ (1, eq);
        }
    }
    else
    {
        FAIL();
    }

    m_status = m_payloadBridge->msgPayloadUpdateVectorTime(m_msg, NULL, 1, m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    // TODO Check prototype for GetVectorDateTime
    m_status = m_payloadBridge->msgPayloadGetVectorDateTime(m_msg, NULL, 1, (mama_u64_t* const*) &m_out, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ ((mama_size_t)VECTOR_UPDATE_SIZE, m_outSize);

    if( MAMA_STATUS_OK == m_status )
    {
        for( unsigned int ii(0) ; ii < VECTOR_UPDATE_SIZE ; ++ii )
        {
            int eq = mamaDateTime_equal( m_update[ii],
                                         m_out[ii] );

            EXPECT_EQ (1, eq);
        }
    }
    else
    {
        FAIL();
    }
}

TEST_F(PayloadVectorDateTimeTests, DISABLED_GetVectorDateTimeNullResult)
// Disabled as mamaMsg_addVectorDateTime is not implemented.
{
    m_status = m_payloadBridge->msgPayloadAddVectorDateTime(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    // TODO Check prototype for GetVectorDateTime
    m_status = m_payloadBridge->msgPayloadGetVectorDateTime(m_msg, NULL, 1, NULL, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorDateTimeTests, DISABLED_GetVectorDateTimeNullSize)
// Disabled as mamaMsg_addVectorDateTime is not implemented.
{
    m_status = m_payloadBridge->msgPayloadAddVectorDateTime(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    // TODO Check prototype for GetVectorDateTime
    m_status = m_payloadBridge->msgPayloadGetVectorDateTime(m_msg, NULL, 1, m_out, NULL);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

// ****************************************************************************
// Price Test Suite
// ****************************************************************************

class PayloadVectorPriceTests : public PayloadVectorTests
{
public:

    virtual ~PayloadVectorPriceTests()
    {}

protected:

    mamaPrice m_in[ VECTOR_SIZE ];
    mamaPrice m_update[ VECTOR_UPDATE_SIZE ];
    const mamaPrice *m_out;
    mama_size_t m_outSize;

    PayloadVectorPriceTests()
        : m_in( )
        , m_update( )
        , m_out( NULL )
        , m_outSize( 0 )
    {
    }
    
    virtual void SetUp()
    {
        PayloadVectorTests::SetUp();

        for( uint32_t ii(0) ; ii < VECTOR_SIZE ; ++ii )
        {
            mamaPrice_create( &m_in[ ii ] );
            mamaPrice_setValue( m_in[ ii ], (double) (((double) ii ) * 1.1) );
        }

        for( uint32_t ii(0) ; ii < VECTOR_UPDATE_SIZE ; ++ii )
        {
            mamaPrice_create( &m_update[ ii ] );
            mamaPrice_setValue( m_update[ ii ], (double) (((double) ii ) * 2.2) );
        }
    }

    virtual void TearDown()
    {
        for( uint32_t ii(0) ; ii < VECTOR_SIZE ; ++ii )
        {
            mamaPrice_destroy( m_in[ii] );
        }

        for( uint32_t ii(0) ; ii < VECTOR_UPDATE_SIZE ; ++ii )
        {
            mamaPrice_destroy( m_update[ii] );
        }

        PayloadVectorTests::TearDown();
    }
};

// AddVectorPrice test fixtures
// ****************************

TEST_F(PayloadVectorPriceTests, DISABLED_AddVectorPrice)
// Disabled as mamaMsg_addVectorPrice is not implemented.
{
    m_status = m_payloadBridge->msgPayloadAddVectorPrice(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    // TODO Check prototype for GetVectorPrice
    m_status = m_payloadBridge->msgPayloadGetVectorPrice(m_msg, NULL, 1, (void* const*) &m_out, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_OK , m_status);

    EXPECT_EQ (VECTOR_SIZE , m_outSize);

    if( MAMA_STATUS_OK == m_status )
    {
        for( unsigned int ii(0) ; ii < VECTOR_SIZE ; ++ii )
        {
            int eq = mamaPrice_equal( m_in[ii],
                                      m_out[ii] );
            EXPECT_EQ (1, eq);
        }
    }
    else
    {
        FAIL();
    }
}

TEST_F(PayloadVectorPriceTests, DISABLED_AddVectorPriceNullPrice)
// Disabled as mamaMsg_addVectorPrice is not implemented.
{
    m_status = m_payloadBridge->msgPayloadAddVectorPrice(m_msg, NULL, 1, NULL, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG , m_status);
}

TEST_F(PayloadVectorPriceTests, DISABLED_AddVectorPriceNullMsg)
// Disabled as mamaMsg_addVectorPrice is not implemented.
{
    m_status = m_payloadBridge->msgPayloadAddVectorPrice(NULL, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorPriceTests, DISABLED_AddVectorPriceAfterInit)
// Disabled as mamaMsg_addVectorPrice is not implemented.
{
    m_status = m_payloadBridge->msgPayloadAddVectorPrice(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadAddVectorPrice(m_msg, NULL, 2, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
}

TEST_F(PayloadVectorPriceTests, DISABLED_AddVectorPriceInvalidName)
// Disabled as mamaMsg_addVectorPrice is not implemented.
{
    m_status = m_payloadBridge->msgPayloadAddVectorPrice(m_msg, "Bob", 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadAddVectorPrice(m_msg, NULL, 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorPriceTests, DISABLED_AddVectorPriceInvalidFid)
// Disabled as mamaMsg_addVectorPrice is not implemented.
{
    m_status = m_payloadBridge->msgPayloadAddVectorPrice(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadAddVectorPrice(m_msg, NULL, 0, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

// UpdateVectorPrice test fixtures
// *******************************
// wmsgPayload_updateVectorPrice not implemented
TEST_F(PayloadVectorPriceTests, DISABLED_UpdateVectorPrice)
// Disabled as mamaMsg_addVectorPrice is not implemented.
{
    m_status = m_payloadBridge->msgPayloadAddVectorPrice(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorPrice(m_msg, NULL, 1, (void* const*) &m_out, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (VECTOR_SIZE, m_outSize);

    if( MAMA_STATUS_OK == m_status )
    {
        for( unsigned int ii(0) ; ii < VECTOR_SIZE ; ++ii )
        {
            int eq = mamaPrice_equal( m_in[ii],
                                         m_out[ii] );
            EXPECT_EQ (1, eq);
        }
    }
    else
    {
        FAIL();
    }

    // TODO Interface inconsistent
    m_status = m_payloadBridge->msgPayloadUpdateVectorPrice(m_msg, NULL, 1, (void* const**) m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    /*
    m_status = m_payloadBridge->msgPayloadGetVectorPrice(m_msg, NULL, 1, (void* const*) &m_out, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ (VECTOR_UPDATE_SIZE, m_outSize);

    if( MAMA_STATUS_OK == m_status )
    {
        for( unsigned int ii(0) ; ii < VECTOR_UPDATE_SIZE ; ++ii )
        {
            int eq = mamaPrice_equal( m_update[ii],
                                         m_out[ii] );
            EXPECT_EQ (1, eq);
        }
    }
    else
    {
        FAIL();
    }
    */
}

TEST_F(PayloadVectorPriceTests, DISABLED_UpdateVectorPriceNullUpdate)
// Disabled as mamaMsg_addVectorPrice is not implemented.
{
    m_status = m_payloadBridge->msgPayloadAddVectorPrice(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    // TODO Check prototype for GetVectorPrice
    m_status = m_payloadBridge->msgPayloadGetVectorPrice(m_msg, NULL, 1, (void* const*) &m_out, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    if( MAMA_STATUS_OK == m_status )
    {
        for( unsigned int ii(0) ; ii < VECTOR_SIZE ; ++ii )
        {
            int eq = mamaPrice_equal( m_in[ii],
                                      m_out[ii] );
            EXPECT_EQ (1, eq);
        }
    }
    else
    {
        FAIL();
    }

    m_status = m_payloadBridge->msgPayloadUpdateVectorPrice(m_msg, NULL, 1, NULL, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorPriceTests, DISABLED_UpdateVectorPriceNullMessage)
// Disabled as mamaMsg_addVectorPrice is not implemented.
{
    m_status = m_payloadBridge->msgPayloadAddVectorPrice(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    // TODO Check prototype for GetVectorPrice
    m_status = m_payloadBridge->msgPayloadGetVectorPrice(m_msg, NULL, 1, (void* const*) &m_out, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    if( MAMA_STATUS_OK == m_status )
    {
        for( unsigned int ii(0) ; ii < VECTOR_SIZE ; ++ii )
        {
            int eq = mamaPrice_equal( m_in[ii],
                                      m_out[ii] );

            EXPECT_EQ (1, eq);
        }
    }
    else
    {
        FAIL();
    }

    m_status = m_payloadBridge->msgPayloadUpdateVectorPrice(NULL, NULL, 1, (void* const**) m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

// GetVectorPrice test fixtures
// ****************************
TEST_F(PayloadVectorPriceTests, DISABLED_GetVectorPrice)
// Disabled as mamaMsg_addVectorPrice is not implemented.
{
    m_status = m_payloadBridge->msgPayloadAddVectorPrice(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    // TODO Check prototype for GetVectorPrice
    m_status = m_payloadBridge->msgPayloadGetVectorPrice(m_msg, NULL, 1, (void* const*) &m_out, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ ((mama_size_t)VECTOR_SIZE, m_outSize);

    if( MAMA_STATUS_OK == m_status )
    {
        for( unsigned int ii(0) ; ii < VECTOR_SIZE ; ++ii )
        {
            int eq = mamaPrice_equal( m_in[ii],
                                         m_out[ii] );

            EXPECT_EQ (1, eq);
        }
    }
    else
    {
        FAIL();
    }

    // TODO Interface Inconsistent
    m_status = m_payloadBridge->msgPayloadUpdateVectorPrice(m_msg, NULL, 1, (void* const**) m_update, VECTOR_UPDATE_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorPrice(m_msg, NULL, 1, (void* const*) &m_out, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_EQ ((mama_size_t)VECTOR_UPDATE_SIZE, m_outSize);

    if( MAMA_STATUS_OK == m_status )
    {
        for( unsigned int ii(0) ; ii < VECTOR_UPDATE_SIZE ; ++ii )
        {
            int eq = mamaPrice_equal( m_update[ii],
                                         m_out[ii] );

            EXPECT_EQ (1, eq);
        }
    }
    else
    {
        FAIL();
    }
}

TEST_F(PayloadVectorPriceTests, DISABLED_GetVectorPriceNullResult)
// Disabled as mamaMsg_addVectorPrice is not implemented.
{
    m_status = m_payloadBridge->msgPayloadAddVectorPrice(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorPrice(m_msg, NULL, 1, NULL, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorPriceTests, DISABLED_GetVectorPriceNullSize)
// Disabled as mamaMsg_addVectorPrice is not implemented.
{
    m_status = m_payloadBridge->msgPayloadAddVectorPrice(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorPrice(m_msg, NULL, 1, (void* const*) &m_out, NULL);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(PayloadVectorPriceTests, DISABLED_GetVectorPriceNotFound)
// Disabled as mamaMsg_addVectorPrice is not implemented.
{
    m_status = m_payloadBridge->msgPayloadAddVectorPrice(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    EXPECT_EQ (MAMA_STATUS_OK, m_status);

    m_status = m_payloadBridge->msgPayloadGetVectorPrice(m_msg, NULL, 2, (void* const*) &m_out, &m_outSize);
    EXPECT_EQ (MAMA_STATUS_NOT_FOUND, m_status);
}

