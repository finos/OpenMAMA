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

#define VECTOR_SIZE 2
#define MAMA_DATE_TIME_GET_VECTOR_CAST (const mamaDateTime*)
#define MAMA_PRICE_GET_VECTOR_CAST     (const mamaPrice*)

using namespace ::testing;

/**
 * Boilerplate test suite base class
 * ****************************************************************************
 */
class FieldVectorTests : public Test
{
protected:

    std::string m_payload;

    // Reusables
    mamaPayloadBridge m_payloadBridge;
    msgPayload        m_msg;
    msgFieldPayload   m_field;
    mama_status       m_status;
    mama_size_t       m_size;

    FieldVectorTests()
        : m_payload(::getPayload())
        , m_payloadBridge(NULL)
        , m_msg(NULL)
        , m_field(NULL)
        , m_status(MAMA_STATUS_OK)
        , m_size(0)
    {
        Test::RecordProperty("payload", m_payload.c_str());
    }

    // Common SetUp
    virtual void SetUp()
    {
        mama_loadPayloadBridge(&m_payloadBridge, m_payload.c_str());
        
        m_status = m_payloadBridge->msgPayloadCreate(&m_msg);
        ASSERT_EQ (MAMA_STATUS_OK, m_status);
    }

    // Common TearDown
    virtual void TearDown()
    {
        m_status = m_payloadBridge->msgPayloadDestroy(m_msg);
        ASSERT_EQ (MAMA_STATUS_OK, m_status);
    }
};

/*
 * BOOL TEST SUITE
 * ****************************************************************************
 */

class FieldVectorBoolTests : public FieldVectorTests
{
protected:

    mama_bool_t        m_in[VECTOR_SIZE];
    const mama_bool_t* m_out;

    FieldVectorBoolTests() 
        : m_out(NULL)
    {
        m_in[0] = 1;
		m_in[1] = 0;
    }
    
    virtual void SetUp()
    {
        FieldVectorTests::SetUp();
    
        m_payloadBridge->msgPayloadAddVectorBool (m_msg, NULL, 1, m_in, VECTOR_SIZE);

        m_out = NULL;
    }

    virtual void TearDown()
    {
        FieldVectorTests::TearDown();
    }
};

/**
 * GetBool test fixtures
 * ****************************************************************************
 */

TEST_F(FieldVectorBoolTests, GetVectorBool)
{
    m_payloadBridge->msgPayloadGetField (m_msg, NULL, 1, &m_field);
    m_status = m_payloadBridge->msgFieldPayloadGetVectorBool(m_field, &m_out, &m_size);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    ASSERT_EQ (1, m_out[0]);
    ASSERT_EQ (0, m_out[1]);
    ASSERT_EQ ((mama_size_t) VECTOR_SIZE * sizeof(mama_bool_t), m_size);
}

TEST_F(FieldVectorBoolTests, GetVectorBoolNullOut)
{
    m_status = m_payloadBridge->msgFieldPayloadGetVectorBool(m_field, NULL, &m_size);
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(FieldVectorBoolTests, GetVectorBoolNullSize)
{
    m_status = m_payloadBridge->msgFieldPayloadGetVectorBool(m_field, &m_out, NULL);
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

/**
 * ****************************************************************************
 * CHAR TEST SUITE
 * ****************************************************************************
 */

class FieldVectorCharTests : public FieldVectorTests
{
protected:

    char        m_in[VECTOR_SIZE];
    const char* m_out;

    FieldVectorCharTests() 
        : m_out(NULL)
    {
        m_in[0] = 'X';
        m_in[1] = 'Y';
    }
    
    virtual void SetUp()
    {
        FieldVectorTests::SetUp();
    
        m_payloadBridge->msgPayloadAddVectorChar (m_msg, NULL, 1, m_in, VECTOR_SIZE);

        m_out = NULL;
    }

    virtual void TearDown()
    {
        FieldVectorTests::TearDown();
    }
};

/**
 * GetBool test fixtures
 * ****************************************************************************
 */

TEST_F(FieldVectorCharTests, GetVectorChar)
{
    m_payloadBridge->msgPayloadGetField (m_msg, NULL, 1, &m_field);
    m_status = m_payloadBridge->msgFieldPayloadGetVectorChar(m_field, &m_out, &m_size);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    ASSERT_EQ ('X', m_out[0]);
    ASSERT_EQ ('Y', m_out[1]);
    ASSERT_EQ ((mama_size_t)VECTOR_SIZE, m_size);
}

TEST_F(FieldVectorCharTests, GetVectorCharNullOut)
{
    m_status = m_payloadBridge->msgFieldPayloadGetVectorChar(m_field, NULL, &m_size);
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(FieldVectorCharTests, GetVectorCharNullSize)
{
    m_status = m_payloadBridge->msgFieldPayloadGetVectorChar(m_field, &m_out, NULL);
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

/**
 * ****************************************************************************
 * I8 TEST SUITE
 * ****************************************************************************
 */

class FieldVectorI8Tests : public FieldVectorTests
{
protected:

    mama_i8_t        m_in[VECTOR_SIZE];
    const mama_i8_t* m_out;

    FieldVectorI8Tests() 
        : m_out(NULL)
    {
        m_in[0] = 1;
        m_in[1] = 2;
    }
    
    virtual void SetUp()
    {
        FieldVectorTests::SetUp();
    
        m_payloadBridge->msgPayloadAddVectorI8 (m_msg, NULL, 1, m_in, VECTOR_SIZE);

        m_out = NULL;
    }

    virtual void TearDown()
    {
        FieldVectorTests::TearDown();
    }
};

/**
 * GetI8 test fixtures
 * ****************************************************************************
 */

TEST_F(FieldVectorI8Tests, GetVectorI8)
{
    m_payloadBridge->msgPayloadGetField (m_msg, NULL, 1, &m_field);
    m_status = m_payloadBridge->msgFieldPayloadGetVectorI8(m_field, &m_out, &m_size);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    ASSERT_EQ (1, m_out[0]);
    ASSERT_EQ (2, m_out[1]);
    ASSERT_EQ ((mama_size_t) VECTOR_SIZE * sizeof(mama_i8_t), m_size);
}

TEST_F(FieldVectorI8Tests, GetVectorI8NullOut)
{
    m_status = m_payloadBridge->msgFieldPayloadGetVectorI8(m_field, NULL, &m_size);
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(FieldVectorI8Tests, GetVectorI8NullSize)
{
    m_status = m_payloadBridge->msgFieldPayloadGetVectorI8(m_field, &m_out, NULL);
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

/**
 * ****************************************************************************
 * U8 TEST SUITE
 * ****************************************************************************
 */

class FieldVectorU8Tests : public FieldVectorTests
{
protected:

    mama_u8_t        m_in[VECTOR_SIZE];
    const mama_u8_t* m_out;

    FieldVectorU8Tests() 
        : m_out(NULL)
    {
        m_in[0] = 128;
        m_in[1] = 255;
    }
    
    virtual void SetUp()
    {
        FieldVectorTests::SetUp();
    
        m_payloadBridge->msgPayloadAddVectorU8 (m_msg, NULL, 1, m_in, VECTOR_SIZE);
    //    m_payloadBridge->msgFieldPayloadUpdateU8(m_field, m_payloadBridge, m_in[0]);
    //    m_payloadBridge->msgFieldPayloadUpdateU8(m_field, m_payloadBridge, m_in[1]);

        m_out = NULL;
    }

    virtual void TearDown()
    {
        FieldVectorTests::TearDown();
    }
};

/**
 * GetU8 test fixtures
 * ****************************************************************************
 */

TEST_F(FieldVectorU8Tests, GetVectorU8)
{
    m_payloadBridge->msgPayloadGetField (m_msg, NULL, 1, &m_field);
    m_status = m_payloadBridge->msgFieldPayloadGetVectorU8(m_field, &m_out, &m_size);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    ASSERT_EQ (128, m_out[0]);
    ASSERT_EQ (255, m_out[1]);
    ASSERT_EQ ((mama_size_t) VECTOR_SIZE, m_size);
}

TEST_F(FieldVectorU8Tests, GetVectorU8NullOut)
{
    m_status = m_payloadBridge->msgFieldPayloadGetVectorU8(m_field, NULL, &m_size);
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(FieldVectorU8Tests, GetVectorU8NullSize)
{
    m_status = m_payloadBridge->msgFieldPayloadGetVectorU8(m_field, &m_out, NULL);
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

/**
 * ****************************************************************************
 * I16 TEST SUITE
 * ****************************************************************************
 */

class FieldVectorI16Tests : public FieldVectorTests
{
protected:

    mama_i16_t        m_in[VECTOR_SIZE];
    const mama_i16_t* m_out;

    FieldVectorI16Tests() 
        : m_out(NULL)
    {
        m_in[0] = -32768;
        m_in[1] = 32767;
    }
    
    virtual void SetUp()
    {
        FieldVectorTests::SetUp();

        m_payloadBridge->msgPayloadAddVectorI16 (m_msg, NULL, 1, m_in, VECTOR_SIZE);

    
//        m_payloadBridge->msgFieldPayloadUpdateI8(m_field, m_payloadBridge, m_in[0]);
//        m_payloadBridge->msgFieldPayloadUpdateI8(m_field, m_payloadBridge, m_in[1]);

        m_out = NULL;
    }

    virtual void TearDown()
    {
        FieldVectorTests::TearDown();
    }
};

/**
 * GetI8 test fixtures
 * ****************************************************************************
 */

TEST_F(FieldVectorI16Tests, GetVectorI16)
{
    m_payloadBridge->msgPayloadGetField (m_msg, NULL, 1, &m_field);
    m_status = m_payloadBridge->msgFieldPayloadGetVectorI16(m_field, &m_out, &m_size);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    ASSERT_EQ (-32768, m_out[0]);
    ASSERT_EQ (32767, m_out[1]);
    ASSERT_EQ ((mama_size_t) VECTOR_SIZE, m_size);
}

TEST_F(FieldVectorI16Tests, GetVectorI16NullOut)
{
    m_status = m_payloadBridge->msgFieldPayloadGetVectorI16(m_field, NULL, &m_size);
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(FieldVectorI16Tests, GetVectorI16NullSize)
{
    m_status = m_payloadBridge->msgFieldPayloadGetVectorI16(m_field, &m_out, NULL);
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

/**
 * ****************************************************************************
 * U8 TEST SUITE
 * ****************************************************************************
 */

class FieldVectorU16Tests : public FieldVectorTests
{
protected:

    mama_u16_t        m_in[VECTOR_SIZE];
    const mama_u16_t* m_out;

    FieldVectorU16Tests() 
        : m_out(NULL)
    {
        m_in[0] = 32768;
        m_in[1] = 65535;
    }
    
    virtual void SetUp()
    {
        FieldVectorTests::SetUp();
    
        m_payloadBridge->msgPayloadAddVectorU16 (m_msg, NULL, 1, m_in, VECTOR_SIZE);


        m_out = NULL;
    }

    virtual void TearDown()
    {
        FieldVectorTests::TearDown();
    }
};

/**
 * GetU16 test fixtures
 * ****************************************************************************
 */

TEST_F(FieldVectorU16Tests, GetVectorU16)
{
    m_payloadBridge->msgPayloadGetField (m_msg, NULL, 1, &m_field);
    m_status = m_payloadBridge->msgFieldPayloadGetVectorU16(m_field, &m_out, &m_size);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    ASSERT_EQ (32768, m_out[0]);
    ASSERT_EQ (65535, m_out[1]);
    ASSERT_EQ ((mama_size_t) VECTOR_SIZE, m_size);
}

TEST_F(FieldVectorU16Tests, GetVectorU16NullOut)
{
    m_status = m_payloadBridge->msgFieldPayloadGetVectorU16(m_field, NULL, &m_size);
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(FieldVectorU16Tests, GetVectorU16NullSize)
{
    m_status = m_payloadBridge->msgFieldPayloadGetVectorU16(m_field, &m_out, NULL);
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

/**
 * ****************************************************************************
 * I32 TEST SUITE
 * ****************************************************************************
 */

class FieldVectorI32Tests : public FieldVectorTests
{
protected:

    mama_i32_t        m_in[VECTOR_SIZE];
    const mama_i32_t* m_out;

    FieldVectorI32Tests() 
        : m_out(NULL)
    {
        m_in[0] = -214783648;
        m_in[1] = 214783647;
    }
    
    virtual void SetUp()
    {
        FieldVectorTests::SetUp();
    
        m_payloadBridge->msgPayloadAddVectorI32 (m_msg, NULL, 1, m_in, VECTOR_SIZE);

        m_out = NULL;
    }

    virtual void TearDown()
    {
        FieldVectorTests::TearDown();
    }
};

/**
 * GetI32 test fixtures
 * ****************************************************************************
 */

TEST_F(FieldVectorI32Tests, GetVectorI32)
{
    m_payloadBridge->msgPayloadGetField (m_msg, NULL, 1, &m_field);
    m_status = m_payloadBridge->msgFieldPayloadGetVectorI32(m_field, &m_out, &m_size);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    ASSERT_EQ (-214783648, m_out[0]);
    ASSERT_EQ (214783647, m_out[1]);
    ASSERT_EQ ((mama_size_t) VECTOR_SIZE, m_size);
}

TEST_F(FieldVectorI32Tests, GetVectorI32NullOut)
{
    m_status = m_payloadBridge->msgFieldPayloadGetVectorI32(m_field, NULL, &m_size);
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(FieldVectorI32Tests, GetVectorI32NullSize)
{
    m_status = m_payloadBridge->msgFieldPayloadGetVectorI32(m_field, &m_out, NULL);
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

/**
 * ****************************************************************************
 * U32 TEST SUITE
 * ****************************************************************************
 */

class FieldVectorU32Tests : public FieldVectorTests
{
protected:

    mama_u32_t        m_in[VECTOR_SIZE];
    const mama_u32_t* m_out;

    FieldVectorU32Tests() 
        : m_out(NULL)
    {
        m_in[0] = 2147483648;
        m_in[1] = 4294967295;
    }
    
    virtual void SetUp()
    {
        FieldVectorTests::SetUp();
    
        m_payloadBridge->msgPayloadAddVectorU32 (m_msg, NULL, 1, m_in, VECTOR_SIZE);

        m_out = NULL;
    }

    virtual void TearDown()
    {
        FieldVectorTests::TearDown();
    }
};

/**
 * GetU32 test fixtures
 * ****************************************************************************
 */

TEST_F(FieldVectorU32Tests, GetVectorU32)
{
    m_payloadBridge->msgPayloadGetField (m_msg, NULL, 1, &m_field);
    m_status = m_payloadBridge->msgFieldPayloadGetVectorU32(m_field, &m_out, &m_size);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    ASSERT_EQ (2147483648, m_out[0]);
    ASSERT_EQ (4294967295, m_out[1]);
    ASSERT_EQ ((mama_size_t) VECTOR_SIZE, m_size);
}

TEST_F(FieldVectorU32Tests, GetVectorU32NullOut)
{
    m_status = m_payloadBridge->msgFieldPayloadGetVectorU32(m_field, NULL, &m_size);
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(FieldVectorU32Tests, GetVectorU32NullSize)
{
    m_status = m_payloadBridge->msgFieldPayloadGetVectorU32(m_field, &m_out, NULL);
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

/**
 * ****************************************************************************
 * I64 TEST SUITE
 * ****************************************************************************
 */

class FieldVectorI64Tests : public FieldVectorTests
{
protected:

    mama_i64_t        m_in[VECTOR_SIZE];
    const mama_i64_t* m_out;

    FieldVectorI64Tests() 
        : m_out(NULL)
    {
        m_in[0] =-9223372036854775807;
        m_in[1] = 9223372036854775807;
    }
    
    virtual void SetUp()
    {
        FieldVectorTests::SetUp();
    
//        m_payloadBridge->msgFieldPayloadUpdateI64(m_field, m_payloadBridge, m_in[0]);
//        m_payloadBridge->msgFieldPayloadUpdateI64(m_field, m_payloadBridge, m_in[1]);

        m_payloadBridge->msgPayloadAddVectorI64 (m_msg, NULL, 1, m_in, VECTOR_SIZE);


        m_out = NULL;
    }

    virtual void TearDown()
    {
        FieldVectorTests::TearDown();
    }
};

/**
 * GetI8 test fixtures
 * ****************************************************************************
 */

TEST_F(FieldVectorI64Tests, GetVectorI64)
{
    m_payloadBridge->msgPayloadGetField (m_msg, NULL, 1, &m_field);
    m_status = m_payloadBridge->msgFieldPayloadGetVectorI64(m_field, &m_out, &m_size);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    ASSERT_EQ (-9223372036854775807, m_out[0]);
    ASSERT_EQ (9223372036854775807, m_out[1]);
    ASSERT_EQ ((mama_size_t) VECTOR_SIZE, m_size);
}

TEST_F(FieldVectorI64Tests, GetVectorI64NullOut)
{
    m_status = m_payloadBridge->msgFieldPayloadGetVectorI64(m_field, NULL, &m_size);
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(FieldVectorI64Tests, GetVectorI64NullSize)
{
    m_status = m_payloadBridge->msgFieldPayloadGetVectorI64(m_field, &m_out, NULL);
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

/**
 * ****************************************************************************
 * U64 TEST SUITE
 * ****************************************************************************
 */

class FieldVectorU64Tests : public FieldVectorTests
{
public:

    static const mama_u64_t LARGE_NUM_1;
    static const mama_u64_t LARGE_NUM_2;

protected:

    mama_u64_t        m_in[VECTOR_SIZE];
    const mama_u64_t* m_out;

    FieldVectorU64Tests() 
        : m_out(NULL)
    {
        m_in[0] = LARGE_NUM_1;
        m_in[1] = LARGE_NUM_2;
    }
    
    virtual void SetUp()
    {
        FieldVectorTests::SetUp();
    
        m_payloadBridge->msgPayloadAddVectorU64 (m_msg, NULL, 1, m_in, VECTOR_SIZE);

        m_out = NULL;
    }

    virtual void TearDown()
    {
        FieldVectorTests::TearDown();
    }

};

const mama_u64_t FieldVectorU64Tests::LARGE_NUM_1 = 9223372036854775808ULL;
const mama_u64_t FieldVectorU64Tests::LARGE_NUM_2 = 18446744073709551615ULL;

/**
 * GetU8 test fixtures
 * ****************************************************************************
 */

TEST_F(FieldVectorU64Tests, GetVectorU64)
{
    m_payloadBridge->msgPayloadGetField (m_msg, NULL, 1, &m_field);
    m_status = m_payloadBridge->msgFieldPayloadGetVectorU64(m_field, &m_out, &m_size);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    ASSERT_EQ (FieldVectorU64Tests::LARGE_NUM_1, m_out[0]);
    ASSERT_EQ (FieldVectorU64Tests::LARGE_NUM_2, m_out[1]);
    ASSERT_EQ ((mama_size_t) VECTOR_SIZE, m_size);
}

TEST_F(FieldVectorU64Tests, GetVectorU64NullOut)
{
    m_status = m_payloadBridge->msgFieldPayloadGetVectorU64(m_field, NULL, &m_size);
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(FieldVectorU64Tests, GetVectorU64NullSize)
{
    m_status = m_payloadBridge->msgFieldPayloadGetVectorU64(m_field, &m_out, NULL);
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

/**
 * ****************************************************************************
 * F32 TEST SUITE
 * ****************************************************************************
 */

class FieldVectorF32Tests : public FieldVectorTests
{
protected:

    mama_f32_t        m_in[VECTOR_SIZE];
    const mama_f32_t* m_out;

    FieldVectorF32Tests() 
        : m_out(NULL)
    {
        m_in[0] = 1.0f;
        m_in[1] = 2.0f;
    }
    
    virtual void SetUp()
    {
        FieldVectorTests::SetUp();
    
        m_payloadBridge->msgPayloadAddVectorF32 (m_msg, NULL, 1, m_in, VECTOR_SIZE);

        m_out = NULL;
    }

    virtual void TearDown()
    {
        FieldVectorTests::TearDown();
    }
};

/**
 * GetF32 test fixtures
 * ****************************************************************************
 */

TEST_F(FieldVectorF32Tests, GetVectorF32)
{
    m_payloadBridge->msgPayloadGetField (m_msg, NULL, 1, &m_field);
    m_status = m_payloadBridge->msgFieldPayloadGetVectorF32(m_field, &m_out, &m_size);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    ASSERT_EQ (1.0f, m_out[0]);
    ASSERT_EQ (2.0f, m_out[1]);
    ASSERT_EQ ((mama_size_t) VECTOR_SIZE, m_size);
}

TEST_F(FieldVectorF32Tests, GetVectorF32NullOut)
{
    m_status = m_payloadBridge->msgFieldPayloadGetVectorF32(m_field, NULL, &m_size);
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(FieldVectorF32Tests, GetVectorF32NullSize)
{
    m_status = m_payloadBridge->msgFieldPayloadGetVectorF32(m_field, &m_out, NULL);
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

/**
 * ****************************************************************************
 * F64 TEST SUITE
 * ****************************************************************************
 */

class FieldVectorF64Tests : public FieldVectorTests
{
protected:

    mama_f64_t        m_in[VECTOR_SIZE];
    const mama_f64_t* m_out;

    FieldVectorF64Tests() 
        : m_out(NULL)
    {
        m_in[0] = 1.0f;
        m_in[1] = 2.0f;
    }
    
    virtual void SetUp()
    {
        FieldVectorTests::SetUp();
    
        m_payloadBridge->msgPayloadAddVectorF64 (m_msg, NULL, 1, m_in, VECTOR_SIZE);

        m_out = NULL;
    }

    virtual void TearDown()
    {
        FieldVectorTests::TearDown();
    }
};

/**
 * GetF64 test fixtures
 * ****************************************************************************
 */

TEST_F(FieldVectorF64Tests, GetVectorF64)
{
    m_payloadBridge->msgPayloadGetField (m_msg, NULL, 1, &m_field);
    m_status = m_payloadBridge->msgFieldPayloadGetVectorF64(m_field, &m_out, &m_size);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    ASSERT_EQ (1.0f, m_out[0]);
    ASSERT_EQ (2.0f, m_out[1]);
    ASSERT_EQ ((mama_size_t) VECTOR_SIZE, m_size);
}

TEST_F(FieldVectorF64Tests, GetVectorF64NullOut)
{
    m_status = m_payloadBridge->msgFieldPayloadGetVectorF64(m_field, NULL, &m_size);
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(FieldVectorF64Tests, GetVectorF64NullSize)
{
    m_status = m_payloadBridge->msgFieldPayloadGetVectorF64(m_field, &m_out, NULL);
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

/**
 * ****************************************************************************
 * STRING TEST SUITE
 * ****************************************************************************
 */

class FieldVectorStringTests : public FieldVectorTests
{
protected:

    const char*  m_in[VECTOR_SIZE];
    const char** m_out;

    FieldVectorStringTests() 
        : m_out(NULL)
    {
        m_in[0] = "ABC";
        m_in[1] = "XYZ";
    }
    
    virtual void SetUp()
    {
        FieldVectorTests::SetUp();
            
        m_payloadBridge->msgPayloadAddVectorString (m_msg, NULL, 1, m_in, VECTOR_SIZE);

        m_out = NULL;
    }

    virtual void TearDown()
    {
        FieldVectorTests::TearDown();
    }
};

/**
 * GetString test fixtures
 * ****************************************************************************
 */

TEST_F(FieldVectorStringTests, GetVectorString)
{
    m_payloadBridge->msgPayloadGetField (m_msg, NULL, 1, &m_field);
    m_status = m_payloadBridge->msgFieldPayloadGetVectorString(m_field, &m_out, &m_size);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    EXPECT_STREQ ("ABC", m_out[0]);
    EXPECT_STREQ ("XYZ", m_out[1]);
    ASSERT_EQ ((mama_size_t) VECTOR_SIZE, m_size);
}

TEST_F(FieldVectorStringTests, GetVectorStringNullOut)
{
    m_status = m_payloadBridge->msgFieldPayloadGetVectorString(m_field, NULL, &m_size);
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(FieldVectorStringTests, GetVectorStringNullSize)
{
    m_status = m_payloadBridge->msgFieldPayloadGetVectorString(m_field, &m_out, NULL);
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

/**
 * ****************************************************************************
 * DATETIME TEST SUITE
 * ****************************************************************************
 */

class FieldVectorDateTimeTests : public FieldVectorTests
{
protected:

    mamaDateTime        m_in[VECTOR_SIZE];
    const mamaDateTime* m_out;

    FieldVectorDateTimeTests() 
        : m_out(NULL)
    {
        mamaDateTime_create(&m_in[0]);
        mamaDateTime_create(&m_in[1]);

        mamaDateTime_setEpochTime(m_in[0], 1, 0, MAMA_DATE_TIME_PREC_SECONDS);
        mamaDateTime_setEpochTime(m_in[1], 2, 0, MAMA_DATE_TIME_PREC_SECONDS);
    }
    
    virtual ~FieldVectorDateTimeTests() 
    {}

    virtual void SetUp()
    {
        FieldVectorTests::SetUp();
    
        m_payloadBridge->msgPayloadAddVectorDateTime (m_msg, NULL, 1, m_in, VECTOR_SIZE);

        m_out = NULL;
    }

    virtual void TearDown()
    {
        if (m_in[0])
        {
            mamaDateTime_destroy (m_in[0]);
            m_in[0] = NULL;
        }

        if (m_in[1])
        {
            mamaDateTime_destroy (m_in[1]);
            m_in[1] = NULL;
        }

        FieldVectorTests::TearDown();
    }
};

/**
 * GetDateTime test fixtures
 * ****************************************************************************
 */

TEST_F(FieldVectorDateTimeTests, GetVectorDateTime)
{
    size_t i = 0;
    m_status = m_payloadBridge->msgPayloadGetField(m_msg, NULL, 1, &m_field);
    CHECK_NON_IMPLEMENTED_OPTIONAL(m_status);
    ASSERT_EQ(m_status, MAMA_STATUS_OK);
    m_status = m_payloadBridge->msgFieldPayloadGetVectorDateTime(m_field, MAMA_DATE_TIME_GET_VECTOR_CAST(&m_out), &m_size);
    for (i = 0; i < VECTOR_SIZE; i++)
    {
        ASSERT_EQ (1, mamaDateTime_equal (m_out[i], m_in[i]));
    }
    ASSERT_EQ(m_status, MAMA_STATUS_OK);
}

TEST_F(FieldVectorDateTimeTests, GetVectorDateTimeNullOut)
{
    m_status = m_payloadBridge->msgFieldPayloadGetVectorDateTime(m_field, NULL, &m_size);
    CHECK_NON_IMPLEMENTED_OPTIONAL(m_status);
    ASSERT_EQ (m_status, MAMA_STATUS_NULL_ARG);
}

TEST_F(FieldVectorDateTimeTests, GetVectorDateTimeNullSize)
{
    m_status = m_payloadBridge->msgFieldPayloadGetVectorDateTime(m_field, MAMA_DATE_TIME_GET_VECTOR_CAST(&m_out), NULL);
    CHECK_NON_IMPLEMENTED_OPTIONAL(m_status);
    ASSERT_EQ (m_status, MAMA_STATUS_NULL_ARG);
}

/**
 * ****************************************************************************
 * PRICE TEST SUITE
 * ****************************************************************************
 */

class FieldVectorPriceTests : public FieldVectorTests
{
protected:

    mamaPrice        m_in[VECTOR_SIZE];
    const mamaPrice* m_out;

    FieldVectorPriceTests() 
        : m_in ()
        , m_out (NULL)
    {
        ::memset ((void *) m_in, 0, sizeof (m_in));
        mamaPrice_create(&m_in[0]);
        mamaPrice_create(&m_in[1]);

        mamaPrice_setValue(m_in[0], 1.0f);
        mamaPrice_setValue(m_in[1], 2.0f);
    }
    
    virtual ~FieldVectorPriceTests() 
    {}

    virtual void SetUp()
    {
        FieldVectorTests::SetUp();
    
        m_payloadBridge->msgPayloadAddVectorPrice (m_msg, NULL, 1, m_in, VECTOR_SIZE);

        m_out = NULL;
    }

    virtual void TearDown()
    {
        if (m_in[0])
        {
            mamaPrice_destroy (m_in[0]);
            m_in[0] = NULL;
        }

        if (m_in[1])
        {
            mamaPrice_destroy (m_in[1]);
            m_in[1] = NULL;
        }

        FieldVectorTests::TearDown();
    }
};

/**
 * GetDateTime test fixtures
 * ****************************************************************************
 */

TEST_F(FieldVectorPriceTests, GetVectorPrice)
{
    size_t i = 0;
    m_status = m_payloadBridge->msgPayloadGetField(m_msg, NULL, 1, &m_field);
    CHECK_NON_IMPLEMENTED_OPTIONAL(m_status);
    ASSERT_EQ(m_status, MAMA_STATUS_OK);
    m_status = m_payloadBridge->msgFieldPayloadGetVectorPrice(m_field, MAMA_PRICE_GET_VECTOR_CAST(&m_out), &m_size);
    ASSERT_EQ(m_status, MAMA_STATUS_OK);
    for (i = 0; i < VECTOR_SIZE; i++)
    {
        ASSERT_EQ (1, mamaPrice_equal (m_out[i], m_in[i]));
    }
}

TEST_F(FieldVectorPriceTests, GetVectorPriceNullOut)
{
    m_status = m_payloadBridge->msgFieldPayloadGetVectorPrice(m_field, NULL, &m_size);
    CHECK_NON_IMPLEMENTED_OPTIONAL(m_status);
    ASSERT_EQ (m_status, MAMA_STATUS_NULL_ARG);
}

TEST_F(FieldVectorPriceTests, GetVectorPriceNullSize)
{
    m_status = m_payloadBridge->msgFieldPayloadGetVectorPrice(m_field, MAMA_PRICE_GET_VECTOR_CAST(&m_out), NULL);
    CHECK_NON_IMPLEMENTED_OPTIONAL(m_status);
    ASSERT_EQ (m_status, MAMA_STATUS_NULL_ARG);
}

/**
 * ****************************************************************************
 * SUB MSG TEST SUITE
 * ****************************************************************************
 */

class FieldVectorSubMsgTests : public FieldVectorTests
{
protected:

//    msgPayload        m_in[VECTOR_SIZE];
    mamaMsg m_in[ VECTOR_SIZE ];
    const msgPayload* m_out;

    FieldVectorSubMsgTests() 
        : m_out(NULL)
    {
        /* TODO - MamaMsgs */
        ::memset ((void *) m_in, 0, sizeof (m_in));
    }
    
    virtual void SetUp()
    {
        FieldVectorTests::SetUp();
    
        /* TODO - this requires a MamaMsg */
/*        m_payloadBridge->msgPayloadAddVectorMsg (m_msg, NULL, 1, m_in, VECTOR_SIZE); */

        m_out = NULL;

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

    }

    virtual void TearDown()
    {
        FieldVectorTests::TearDown();
    }
};

/**
 * GetVectorMsg test fixtures
 * ****************************************************************************
 */

TEST_F(FieldVectorSubMsgTests, GetVectorSubMsg)
{
    char             testName;

    m_status = m_payloadBridge->msgPayloadAddVectorMsg(m_msg, NULL, 1, m_in, VECTOR_SIZE);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);

    // The following is to populate impl->mWombatField
    m_status = m_payloadBridge->msgPayloadGetField(m_msg, &testName, 1, &m_field);

    m_status = m_payloadBridge->msgFieldPayloadGetVectorMsg(m_field, &m_out, &m_size);
    ASSERT_EQ (MAMA_STATUS_OK, m_status);
    ASSERT_EQ ((mama_size_t) VECTOR_SIZE, m_size);
}

TEST_F(FieldVectorSubMsgTests, GetVectorSubMsgNullOut)
{
    m_status = m_payloadBridge->msgFieldPayloadGetVectorMsg(m_field, NULL, &m_size);
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}

TEST_F(FieldVectorSubMsgTests, GetVectorSubMsgNullSize)
{
    m_status = m_payloadBridge->msgFieldPayloadGetVectorMsg(m_field, &m_out, NULL);
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, m_status);
}
