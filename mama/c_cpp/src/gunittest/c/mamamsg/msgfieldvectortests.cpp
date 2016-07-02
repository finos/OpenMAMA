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
#include "mama/msg.h"
#include "mama/msgfield.h"
#include "MainUnitTestC.h"
#include <iostream>

#define VECTOR_SIZE 2U

using std::cout;
using std::endl;

/**
 * Boilerplate test suite base class
 * ****************************************************************************
 */
class MsgFieldVectorTestsC : public ::testing::Test
{
protected:
    mamaMsg            mMsg;
    mamaPayloadBridge  mPayloadBridge;
    mamaMsgField       mField;
    mama_status        mStatus;
    mama_size_t        mSize;

    MsgFieldVectorTestsC()
        : mMsg           (NULL)
        , mPayloadBridge (NULL)
        , mField (NULL)
        , mStatus (MAMA_STATUS_OK)
        , mSize (0)
    {}

    virtual ~MsgFieldVectorTestsC(void) {};

    virtual void SetUp(void) 
    {
        mama_loadPayloadBridge (&mPayloadBridge, getPayload());
        mamaMsg_create (&mMsg);
        //mamaMsgField_create (&mField);
    };

    virtual void TearDown(void) 
    {
        mamaMsg_destroy(mMsg);
        //mamaMsgField_destroy(mField);
        //mama_close();
    };
};

/*
 * BOOL TEST SUITE
 * ****************************************************************************
 */
class MsgFieldVectorBoolTests : public MsgFieldVectorTestsC
{
protected:
    mama_bool_t         mIn[VECTOR_SIZE];
    const mama_bool_t*  mOut;

    MsgFieldVectorBoolTests()
        : mOut(NULL)
    {
        mIn[0] = 0;
        mIn[1] = 1;
    }

    virtual void SetUp()
    {
        MsgFieldVectorTestsC::SetUp();

        mamaMsg_addVectorBool (mMsg, NULL, 1, mIn, VECTOR_SIZE);

        mOut = NULL;
    }

    virtual void TearDown()
    {
        MsgFieldVectorTestsC::TearDown();
    }
};

/**
 * GetBool test fixtures
 * ****************************************************************************
 */

TEST_F(MsgFieldVectorBoolTests, GetVectorBool)
{
    mStatus = mamaMsg_getField (mMsg, NULL, 1, &mField);
    ASSERT_EQ (mStatus, MAMA_STATUS_OK) << "Failed getting field";
    mStatus = mamaMsgField_getVectorBool (mField, &mOut, &mSize);
    ASSERT_EQ (mStatus, MAMA_STATUS_OK) << "Failed getting Vector Bool";
    EXPECT_EQ (0, mOut[0]);
    EXPECT_EQ (1, mOut[1]);
    EXPECT_EQ (VECTOR_SIZE, mSize);
}

TEST_F(MsgFieldVectorBoolTests, DISABLED_GetVectorBoolNullField)
{
    mStatus = mamaMsgField_getVectorBool (NULL, &mOut, &mSize);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, mStatus);
}

TEST_F(MsgFieldVectorBoolTests, GetVectorBoolNullVector)
{
    mStatus = mamaMsg_getField (mMsg, NULL, 1, &mField);
    ASSERT_EQ (MAMA_STATUS_OK, mStatus) << "Failed getting field";
    mStatus = mamaMsgField_getVectorBool (mField, NULL, &mSize);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, mStatus);
}


/*
 * CHAR TEST SUITE
 * ****************************************************************************
 */
class MsgFieldVectorCharTests : public MsgFieldVectorTestsC
{
protected:
    char         mIn[VECTOR_SIZE];
    const char*  mOut;

    MsgFieldVectorCharTests()
        : mOut(NULL)
    {
        mIn[0] = 'X';
        mIn[1] = 'Y';
    }

    virtual void SetUp()
    {
        MsgFieldVectorTestsC::SetUp();

        mamaMsg_addVectorChar (mMsg, NULL, 1, mIn, VECTOR_SIZE);

        mOut = NULL;
    }

    virtual void TearDown()
    {
        MsgFieldVectorTestsC::TearDown();
    }
};

/**
 * GetChar test fixtures
 * ****************************************************************************
 */

TEST_F(MsgFieldVectorCharTests, GetVectorChar)
{
    mamaMsg_getField (mMsg, NULL, 1, &mField);
    mStatus = mamaMsgField_getVectorChar (mField, &mOut, &mSize);
    ASSERT_EQ (mStatus, MAMA_STATUS_OK);
    EXPECT_EQ (mOut[0], 'X');
    EXPECT_EQ (mOut[1], 'Y');
    EXPECT_EQ (mSize, VECTOR_SIZE);
}

TEST_F(MsgFieldVectorCharTests, DISABLED_GetVectorCharNullField)
{
    mStatus = mamaMsgField_getVectorChar (NULL, &mOut, &mSize);
    EXPECT_EQ (mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgFieldVectorCharTests, GetVectorCharNullVector)
{
    mamaMsg_getField (mMsg, NULL, 1, &mField);
    mStatus = mamaMsgField_getVectorChar (mField, NULL, &mSize);
    EXPECT_EQ (mStatus, MAMA_STATUS_NULL_ARG);
}

/*
 * I8 TEST SUITE
 * ****************************************************************************
 */
class MsgFieldVectorI8Tests : public MsgFieldVectorTestsC
{
protected:
    mama_i8_t           mIn[VECTOR_SIZE];
    const mama_i8_t*    mOut;

    MsgFieldVectorI8Tests()
        : mOut(NULL)
    {
        mIn[0] = 1;
        mIn[1] = 2;
    }

    virtual void SetUp()
    {
        MsgFieldVectorTestsC::SetUp();

        mamaMsg_addVectorI8 (mMsg, NULL, 1, mIn, VECTOR_SIZE);

        mOut = NULL;
    }

    virtual void TearDown()
    {
        MsgFieldVectorTestsC::TearDown();
    }
};

/**
 * GetI8 test fixtures
 * ****************************************************************************
 */

TEST_F(MsgFieldVectorI8Tests, GetVectorI8)
{
    mamaMsg_getField (mMsg, NULL, 1, &mField);
    mStatus = mamaMsgField_getVectorI8 (mField, &mOut, &mSize);
    ASSERT_EQ (mStatus, MAMA_STATUS_OK);
    EXPECT_EQ (mOut[0], 1);
    EXPECT_EQ (mOut[1], 2);
    EXPECT_EQ (mSize, VECTOR_SIZE);
}

TEST_F(MsgFieldVectorI8Tests, DISABLED_GetVectorI8NullField)
{
    mStatus = mamaMsgField_getVectorI8 (NULL, &mOut, &mSize);
    EXPECT_EQ (mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgFieldVectorI8Tests, GetVectorI8NullVector)
{
    mamaMsg_getField (mMsg, NULL, 1, &mField);
    mStatus = mamaMsgField_getVectorI8 (mField, NULL, &mSize);
    EXPECT_EQ (mStatus, MAMA_STATUS_NULL_ARG);
}

/*
 * U8 TEST SUITE
 * ****************************************************************************
 */
class MsgFieldVectorU8Tests : public MsgFieldVectorTestsC
{
protected:
    mama_u8_t           mIn[VECTOR_SIZE];
    const mama_u8_t*    mOut;

    MsgFieldVectorU8Tests()
        : mOut(NULL)
    {
        mIn[0] = 128;
        mIn[1] = 255;
    }

    virtual void SetUp()
    {
        MsgFieldVectorTestsC::SetUp();

        mamaMsg_addVectorU8 (mMsg, NULL, 1, mIn, VECTOR_SIZE);

        mOut = NULL;
    }

    virtual void TearDown()
    {
        MsgFieldVectorTestsC::TearDown();
    }
};

/**
 * Get8 test fixtures
 * ****************************************************************************
 */

TEST_F(MsgFieldVectorU8Tests, GetVectorU8)
{
    mamaMsg_getField (mMsg, NULL, 1, &mField);
    mStatus = mamaMsgField_getVectorU8 (mField, &mOut, &mSize);
    ASSERT_EQ (mStatus, MAMA_STATUS_OK);
    EXPECT_EQ (mOut[0], 128);
    EXPECT_EQ (mOut[1], 255);
    EXPECT_EQ (mSize, VECTOR_SIZE);
}

TEST_F(MsgFieldVectorU8Tests, DISABLED_GetVectorU8NullField)
{
    mStatus = mamaMsgField_getVectorU8 (NULL, &mOut, &mSize);
    EXPECT_EQ (mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgFieldVectorU8Tests, GetVectorU8NullVector)
{
    mamaMsg_getField (mMsg, NULL, 1, &mField);
    mStatus = mamaMsgField_getVectorU8 (mField, NULL, &mSize);
    EXPECT_EQ (mStatus, MAMA_STATUS_NULL_ARG);
}

/*
 * I16 TEST SUITE
 * ****************************************************************************
 */
class MsgFieldVectorI16Tests : public MsgFieldVectorTestsC
{
protected:
    mama_i16_t           mIn[VECTOR_SIZE];
    const mama_i16_t*    mOut;

    MsgFieldVectorI16Tests()
        : mOut(NULL)
    {
        mIn[0] = -32768;
        mIn[1] = 32767;
    }

    virtual void SetUp()
    {
        MsgFieldVectorTestsC::SetUp();

        mamaMsg_addVectorI16 (mMsg, NULL, 1, mIn, VECTOR_SIZE);

        mOut = NULL;
    }

    virtual void TearDown()
    {
        MsgFieldVectorTestsC::TearDown();
    }
};

/**
 * GetI16 test fixtures
 * ****************************************************************************
 */

TEST_F(MsgFieldVectorI16Tests, GetVectorI16)
{
    mamaMsg_getField (mMsg, NULL, 1, &mField);
    mStatus = mamaMsgField_getVectorI16 (mField, &mOut, &mSize);
    ASSERT_EQ (mStatus, MAMA_STATUS_OK);
    EXPECT_EQ (mOut[0], -32768);
    EXPECT_EQ (mOut[1], 32767);
    EXPECT_EQ (mSize, VECTOR_SIZE);
}

TEST_F(MsgFieldVectorI16Tests, DISABLED_GetVectorI16NullField)
{
    mStatus = mamaMsgField_getVectorI16 (NULL, &mOut, &mSize);
    EXPECT_EQ (mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgFieldVectorI16Tests, GetVectorI16NullVector)
{
    mamaMsg_getField (mMsg, NULL, 1, &mField);
    mStatus = mamaMsgField_getVectorI16 (mField, NULL, &mSize);
    EXPECT_EQ (mStatus, MAMA_STATUS_NULL_ARG);
}

/*
 * U16 TEST SUITE
 * ****************************************************************************
 */
class MsgFieldVectorU16Tests : public MsgFieldVectorTestsC
{
protected:
    mama_u16_t           mIn[VECTOR_SIZE];
    const mama_u16_t*    mOut;

    MsgFieldVectorU16Tests()
        : mOut(NULL)
    {
        mIn[0] = 32768;
        mIn[1] = 65535;
    }

    virtual void SetUp()
    {
        MsgFieldVectorTestsC::SetUp();

        mamaMsg_addVectorU16 (mMsg, NULL, 1, mIn, VECTOR_SIZE);

        mOut = NULL;
    }

    virtual void TearDown()
    {
        MsgFieldVectorTestsC::TearDown();
    }
};

/**
 * GetU16 test fixtures
 * ****************************************************************************
 */

TEST_F(MsgFieldVectorU16Tests, GetVectorU16)
{
    mamaMsg_getField (mMsg, NULL, 1, &mField);
    mStatus = mamaMsgField_getVectorU16 (mField, &mOut, &mSize);
    ASSERT_EQ (mStatus, MAMA_STATUS_OK);
    EXPECT_EQ (mOut[0], 32768);
    EXPECT_EQ (mOut[1], 65535);
    EXPECT_EQ (mSize, VECTOR_SIZE);
}

TEST_F(MsgFieldVectorU16Tests, DISABLED_GetVectorU16NullField)
{
    mStatus = mamaMsgField_getVectorU16 (NULL, &mOut, &mSize);
    EXPECT_EQ (mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgFieldVectorU16Tests, GetVectorU16NullVector)
{
    mamaMsg_getField (mMsg, NULL, 1, &mField);
    mStatus = mamaMsgField_getVectorU16 (mField, NULL, &mSize);
    EXPECT_EQ (mStatus, MAMA_STATUS_NULL_ARG);
}

/*
 * I32 TEST SUITE
 * ****************************************************************************
 */
class MsgFieldVectorI32Tests : public MsgFieldVectorTestsC
{
protected:
    mama_i32_t           mIn[VECTOR_SIZE];
    const mama_i32_t*    mOut;

    MsgFieldVectorI32Tests()
        : mOut(NULL)
    {
        mIn[0] = -214783648;
        mIn[1] = 214783647;
    }

    virtual void SetUp()
    {
        MsgFieldVectorTestsC::SetUp();

        mamaMsg_addVectorI32 (mMsg, NULL, 1, mIn, VECTOR_SIZE);

        mOut = NULL;
    }

    virtual void TearDown()
    {
        MsgFieldVectorTestsC::TearDown();
    }
};

/**
 * GetI32 test fixtures
 * ****************************************************************************
 */

TEST_F(MsgFieldVectorI32Tests, GetVectorI32)
{
    mamaMsg_getField (mMsg, NULL, 1, &mField);
    mStatus = mamaMsgField_getVectorI32 (mField, &mOut, &mSize);
    ASSERT_EQ (mStatus, MAMA_STATUS_OK);
    EXPECT_EQ (mOut[0], -214783648);
    EXPECT_EQ (mOut[1], 214783647);
    EXPECT_EQ (mSize, VECTOR_SIZE);
}

TEST_F(MsgFieldVectorI32Tests, DISABLED_GetVectorI32NullField)
{
    mStatus = mamaMsgField_getVectorI32 (NULL, &mOut, &mSize);
    EXPECT_EQ (mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgFieldVectorI32Tests, GetVectorI32NullVector)
{
    mamaMsg_getField (mMsg, NULL, 1, &mField);
    mStatus = mamaMsgField_getVectorI32 (mField, NULL, &mSize);
    EXPECT_EQ (mStatus, MAMA_STATUS_NULL_ARG);
}

/*
 * U32 TEST SUITE
 * ****************************************************************************
 */
class MsgFieldVectorU32Tests : public MsgFieldVectorTestsC
{
protected:
    mama_u32_t           mIn[VECTOR_SIZE];
    const mama_u32_t*    mOut;

    MsgFieldVectorU32Tests()
        : mOut(NULL)
    {
        mIn[0] = 2147483648;
        mIn[1] = 4294967295;
    }

    virtual void SetUp()
    {
        MsgFieldVectorTestsC::SetUp();

        mamaMsg_addVectorU32 (mMsg, NULL, 1, mIn, VECTOR_SIZE);

        mOut = NULL;
    }

    virtual void TearDown()
    {
        MsgFieldVectorTestsC::TearDown();
    }
};

/**
 * GetU32 test fixtures
 * ****************************************************************************
 */

TEST_F(MsgFieldVectorU32Tests, GetVectorU32)
{
    mamaMsg_getField (mMsg, NULL, 1, &mField);
    mStatus = mamaMsgField_getVectorU32 (mField, &mOut, &mSize);
    ASSERT_EQ (mStatus, MAMA_STATUS_OK);
    EXPECT_EQ (mOut[0], 2147483648);
    EXPECT_EQ (mOut[1], 4294967295);
    EXPECT_EQ (mSize, VECTOR_SIZE);
}

TEST_F(MsgFieldVectorU32Tests, DISABLED_GetVectorU32NullField)
{
    mStatus = mamaMsgField_getVectorU32 (NULL, &mOut, &mSize);
    EXPECT_EQ (mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgFieldVectorU32Tests, DISABLED_GetVectorU32NullVector)
{
    mamaMsg_getField (mMsg, NULL, 1, &mField);
    mStatus = mamaMsgField_getVectorU32 (mField, &mOut, &mSize);
    EXPECT_EQ (mStatus, MAMA_STATUS_NULL_ARG);
}

/*
 * I64 TEST SUITE
 * ****************************************************************************
 */
class MsgFieldVectorI64Tests : public MsgFieldVectorTestsC
{
protected:
    mama_i64_t           mIn[VECTOR_SIZE];
    const mama_i64_t*    mOut;

    MsgFieldVectorI64Tests()
        : mOut(NULL)
    {
        mIn[0] =-9223372036854775807;
        mIn[1] = 9223372036854775807;
    }

    virtual void SetUp()
    {
        MsgFieldVectorTestsC::SetUp();

        mamaMsg_addVectorI64 (mMsg, NULL, 1, mIn, VECTOR_SIZE);

        mOut = NULL;
    }

    virtual void TearDown()
    {
        MsgFieldVectorTestsC::TearDown();
    }
};

/**
 * GetI64 test fixtures
 * ****************************************************************************
 */

TEST_F(MsgFieldVectorI64Tests, GetVectorI64)
{
    mamaMsg_getField (mMsg, NULL, 1, &mField);
    mStatus = mamaMsgField_getVectorI64 (mField, &mOut, &mSize);
    ASSERT_EQ (mStatus, MAMA_STATUS_OK);
    EXPECT_EQ(mOut[0], -9223372036854775807);
    EXPECT_EQ(mOut[1], 9223372036854775807);
    EXPECT_EQ (mSize, VECTOR_SIZE);
}

TEST_F(MsgFieldVectorI64Tests, DISABLED_GetVectorI64NullField)
{
    mStatus = mamaMsgField_getVectorI64 (NULL, &mOut, &mSize);
    EXPECT_EQ (mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgFieldVectorI64Tests, GetVectorI64NullVector)
{
    mamaMsg_getField (mMsg, NULL, 1, &mField);
    mStatus = mamaMsgField_getVectorI64 (mField, NULL, &mSize);
    EXPECT_EQ (mStatus, MAMA_STATUS_NULL_ARG);
}

/*
 * U64 TEST SUITE
 * ****************************************************************************
 */
class MsgFieldVectorU64Tests : public MsgFieldVectorTestsC
{
protected:
    mama_u64_t           mIn[VECTOR_SIZE];
    const mama_u64_t*    mOut;

    MsgFieldVectorU64Tests()
        : mOut(NULL)
    {
        mIn[0] = 9223372036854775808U;
        mIn[1] = 18446744073709551615U;
    }

    virtual void SetUp()
    {
        MsgFieldVectorTestsC::SetUp();

        mamaMsg_addVectorU64 (mMsg, NULL, 1, mIn, VECTOR_SIZE);

        mOut = NULL;
    }

    virtual void TearDown()
    {
        MsgFieldVectorTestsC::TearDown();
    }
};

/**
 * GetU64 test fixtures
 * ****************************************************************************
 */

TEST_F(MsgFieldVectorU64Tests, GetVectorU64)
{
    mamaMsg_getField (mMsg, NULL, 1, &mField);
    mStatus = mamaMsgField_getVectorU64 (mField, &mOut, &mSize);
    ASSERT_EQ (mStatus, MAMA_STATUS_OK);
    EXPECT_EQ (mOut[0], 9223372036854775808U);
    EXPECT_EQ (mOut[1], 18446744073709551615U);
    EXPECT_EQ (mSize, VECTOR_SIZE);
}

TEST_F(MsgFieldVectorU64Tests, DISABLED_GetVectorU64NullField)
{
    mStatus = mamaMsgField_getVectorU64 (NULL, &mOut, &mSize);
    EXPECT_EQ (mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgFieldVectorU64Tests, GetVectorU64NullVector)
{
    mamaMsg_getField (mMsg, NULL, 1, &mField);
    mStatus = mamaMsgField_getVectorU64 (mField, NULL, &mSize);
    EXPECT_EQ (mStatus, MAMA_STATUS_NULL_ARG);
}

/*
 * F32 TEST SUITE
 * ****************************************************************************
 */
class MsgFieldVectorF32Tests : public MsgFieldVectorTestsC
{
protected:
    mama_f32_t           mIn[VECTOR_SIZE];
    const mama_f32_t*    mOut;

    MsgFieldVectorF32Tests()
        : mOut(NULL)
    {
        mIn[0] = 1.0f;
        mIn[1] = 2.0f;
    }

    virtual void SetUp()
    {
        MsgFieldVectorTestsC::SetUp();

        mamaMsg_addVectorF32 (mMsg, NULL, 1, mIn, VECTOR_SIZE);

        mOut = NULL;
    }

    virtual void TearDown()
    {
        MsgFieldVectorTestsC::TearDown();
    }
};

/**
 * GetF32 test fixtures
 * ****************************************************************************
 */

TEST_F(MsgFieldVectorF32Tests, GetVectorF32)
{
    mamaMsg_getField (mMsg, NULL, 1, &mField);
    mStatus = mamaMsgField_getVectorF32 (mField, &mOut, &mSize);
    ASSERT_EQ (mStatus, MAMA_STATUS_OK);
    EXPECT_EQ (mOut[0], 1.0f);
    EXPECT_EQ (mOut[1], 2.0f);
    EXPECT_EQ (mSize, VECTOR_SIZE);
}

TEST_F(MsgFieldVectorF32Tests, DISABLED_GetVectorF32NullField)
{
    mStatus = mamaMsgField_getVectorF32 (NULL, &mOut, &mSize);
    EXPECT_EQ (mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgFieldVectorF32Tests, DISABLED_GetVectorF32NullVector)
{
    mamaMsg_getField (mMsg, NULL, 1, &mField);
    mStatus = mamaMsgField_getVectorF32 (mField, &mOut, &mSize);
    EXPECT_EQ (mStatus, MAMA_STATUS_NULL_ARG);
}

/*
 * F64 TEST SUITE
 * ****************************************************************************
 */
class MsgFieldVectorF64Tests : public MsgFieldVectorTestsC
{
protected:
    mama_f64_t           mIn[VECTOR_SIZE];
    const mama_f64_t*    mOut;

    MsgFieldVectorF64Tests()
        : mOut(NULL)
    {
        mIn[0] = 1.0f;
        mIn[1] = 2.0f;
    }

    virtual void SetUp()
    {
        MsgFieldVectorTestsC::SetUp();

        mamaMsg_addVectorF64 (mMsg, NULL, 1, mIn, VECTOR_SIZE);

        mOut = NULL;
    }

    virtual void TearDown()
    {
        MsgFieldVectorTestsC::TearDown();
    }
};

/**
 * GetF64 test fixtures
 * ****************************************************************************
 */

TEST_F(MsgFieldVectorF64Tests, GetVectorF64)
{
    mamaMsg_getField (mMsg, NULL, 1, &mField);
    mStatus = mamaMsgField_getVectorF64 (mField, &mOut, &mSize);
    ASSERT_EQ (mStatus, MAMA_STATUS_OK);
    EXPECT_EQ (mOut[0], 1.0f);
    EXPECT_EQ (mOut[1], 2.0f);
    EXPECT_EQ (mSize, VECTOR_SIZE);
}

TEST_F(MsgFieldVectorF64Tests, DISABLED_GetVectorF64NullField)
{
    mStatus = mamaMsgField_getVectorF64 (NULL, &mOut, &mSize);
    EXPECT_EQ (mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgFieldVectorF64Tests, GetVectorF64NullVector)
{
    mamaMsg_getField (mMsg, NULL, 1, &mField);
    mStatus = mamaMsgField_getVectorF64 (mField, NULL, &mSize);
    EXPECT_EQ (mStatus, MAMA_STATUS_NULL_ARG);
}

/*
 * STRING TEST SUITE
 * ****************************************************************************
 */
class MsgFieldVectorStringTests : public MsgFieldVectorTestsC
{
protected:
    const char*         mIn[VECTOR_SIZE];
    const char**        mOut;

    MsgFieldVectorStringTests()
        : mOut(NULL)
    {
        mIn[0] = "ABC";
        mIn[1] = "XYZ";
    }

    virtual void SetUp()
    {
        MsgFieldVectorTestsC::SetUp();

        mamaMsg_addVectorString (mMsg, NULL, 1, mIn, VECTOR_SIZE);

        mOut = NULL;
    }

    virtual void TearDown()
    {
        MsgFieldVectorTestsC::TearDown();
    }
};

/**
 * String test fixtures
 * ****************************************************************************
 */

TEST_F(MsgFieldVectorStringTests, GetVectorString)
{
    mamaMsg_getField (mMsg, NULL, 1, &mField);
    mStatus = mamaMsgField_getVectorString (mField, &mOut, &mSize);
    ASSERT_EQ (mStatus, MAMA_STATUS_OK);
    EXPECT_STREQ (mOut[0], "ABC");
    EXPECT_STREQ (mOut[1], "XYZ");
    EXPECT_EQ (mSize, VECTOR_SIZE);
}

TEST_F(MsgFieldVectorStringTests, DISABLED_GetVectorStringNullField)
{
    mStatus = mamaMsgField_getVectorString (NULL, &mOut, &mSize);
    EXPECT_EQ (mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgFieldVectorStringTests, GetVectorStringNullVector)
{
    mamaMsg_getField (mMsg, NULL, 1, &mField);
    mStatus = mamaMsgField_getVectorString (mField, NULL, &mSize);
    EXPECT_EQ (mStatus, MAMA_STATUS_NULL_ARG);
}

/*
 * DATETIME TEST SUITE
 * ****************************************************************************
 */
class MsgFieldVectorDateTimeTests : public MsgFieldVectorTestsC
{
protected:
    mamaDateTime            mIn[VECTOR_SIZE];
    const mamaDateTime**    mOut;

    MsgFieldVectorDateTimeTests()
        : mOut(NULL)
    {
        mamaDateTime_create(&mIn[0]);
        mamaDateTime_create(&mIn[1]);

        mamaDateTime_setEpochTime(mIn[0], 1, 0, MAMA_DATE_TIME_PREC_SECONDS);
        mamaDateTime_setEpochTime(mIn[1], 2, 0, MAMA_DATE_TIME_PREC_SECONDS);
    }

    virtual void SetUp()
    {
        MsgFieldVectorTestsC::SetUp();

        mamaMsg_addVectorDateTime (mMsg, NULL, 1, mIn, VECTOR_SIZE);

        mOut = NULL;
    }

    virtual void TearDown()
    {
        MsgFieldVectorTestsC::TearDown();
    }
};

/**
 * DateTime test fixtures
 * ****************************************************************************
 */

TEST_F(MsgFieldVectorDateTimeTests, DISABLED_GetVectorDateTime)
{
    /* No vector DateTime functions */
}

TEST_F(MsgFieldVectorDateTimeTests, DISABLED_GetVectorDateTimeNullField)
{
    /* No vector DateTime functions */
}

TEST_F(MsgFieldVectorDateTimeTests, DISABLED_GetVectorDateTimeNullVector)
{
    /* No vector DateTime functions */
}

/*
 * PRICE TEST SUITE
 * ****************************************************************************
 */
class MsgFieldVectorPriceTests : public MsgFieldVectorTestsC
{
protected:
    mamaPrice            mIn[VECTOR_SIZE];
    const mamaPrice**    mOut;

    MsgFieldVectorPriceTests()
        : mOut(NULL)
    {
        mamaPrice_create(&mIn[0]);
        mamaPrice_create(&mIn[1]);

        mamaPrice_setValue (mIn[0], 1.0f);
        mamaPrice_setValue (mIn[1], 2.0f);
    }

    virtual void SetUp()
    {
        MsgFieldVectorTestsC::SetUp();

        mamaMsg_addVectorPrice (mMsg, NULL, 1, mIn, VECTOR_SIZE);

        mOut = NULL;
    }

    virtual void TearDown()
    {
        MsgFieldVectorTestsC::TearDown();
    }
};

/**
 * Price test fixtures
 * ****************************************************************************
 */

TEST_F(MsgFieldVectorPriceTests, DISABLED_GetVectorPrice)
{
    /* No vector Price functions */
}

TEST_F(MsgFieldVectorPriceTests, DISABLED_GetVectorPriceNullField)
{
    /* No vector Price functions */
}

TEST_F(MsgFieldVectorPriceTests, DISABLED_GetVectorPriceNullVector)
{
    /* No vector Price functions */
}

/**
 * ****************************************************************************
 * SUB MSG TEST SUITE
 * ****************************************************************************
 */
class MsgFieldVectorSubMsgTests : public MsgFieldVectorTestsC
{
protected:

    mamaMsg mIn[VECTOR_SIZE];
    const mamaMsg* mOut;

    MsgFieldVectorSubMsgTests()
        : mOut(NULL)
    {
        memset (mIn, 0, VECTOR_SIZE * sizeof(mamaMsg));
    }

    virtual void SetUp()
    {
        MsgFieldVectorTestsC::SetUp();

        for (uint32_t i = 0 ; i < VECTOR_SIZE ; i++)
        {
            mamaMsg_create (&mIn[i]);
        }

        mamaMsg_addVectorMsg (mMsg, NULL, 1, mIn, VECTOR_SIZE);
    }

    virtual void TearDown()
    {
        MsgFieldVectorTestsC::TearDown();
        for (uint32_t i = 0 ; i < VECTOR_SIZE ; i++)
        {
            mamaMsg_destroy (mIn[i]);
        }
    }
};

/**
 * GetVectorMsg test fixtures
 * ****************************************************************************
 */

TEST_F(MsgFieldVectorSubMsgTests, GetVectorSubMsg)
{
    mamaMsg_getField (mMsg, NULL, 1, &mField);
    mStatus = mamaMsgField_getVectorMsg (mField, &mOut, &mSize);
    EXPECT_EQ (mStatus, MAMA_STATUS_OK);
}

TEST_F(MsgFieldVectorSubMsgTests, DISABLED_GetVectorSubMsgNullField)
{
    mStatus = mamaMsgField_getVectorMsg (NULL, &mOut, &mSize);
    EXPECT_EQ (mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgFieldVectorSubMsgTests, DISABLED_GetVectorSubMsgNullVector)
{   
    /* This cores because mamaMsgField_getVectorMsg always dereferences result */
    mamaMsg_getField (mMsg, NULL, 1, &mField);
    mStatus = mamaMsgField_getVectorMsg (mField, NULL, &mSize);
    EXPECT_EQ (mStatus, MAMA_STATUS_NULL_ARG);
}
