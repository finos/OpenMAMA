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
#include "MainUnitTestC.h"
#include <iostream>

using std::cout;
using std::endl;

class MsgAtomicTestsC : public ::testing::Test
{
protected:
    MsgAtomicTestsC(void)
        : mMsg           (NULL)
        , mPayloadBridge (NULL)
    {}

    virtual ~MsgAtomicTestsC(void) {};

    virtual void SetUp(void) 
    {
        mama_loadPayloadBridge (&mPayloadBridge, getPayload());
        mamaMsg_create (&mMsg);
    };

    virtual void TearDown(void) 
    {
        mamaMsg_destroy(mMsg);
    };
    
    mamaMsg            mMsg;
    mamaPayloadBridge mPayloadBridge;
};


/**********************************************************************
 * Bool Tests
 **********************************************************************/
class MsgBoolTests : public MsgAtomicTestsC
{
protected:

    mama_bool_t mIn, mUpdate, mOut;

    MsgBoolTests()
        : mIn(true)
        , mUpdate(false)
        , mOut(false)
    {
    }
};

TEST_F (MsgBoolTests, addBoolValid)
{
    ASSERT_EQ(mamaMsg_addBool(mMsg, "Name", 100, mIn), MAMA_STATUS_OK);
}

TEST_F (MsgBoolTests, addBoolInValid)
{
    ASSERT_EQ(mamaMsg_addBool(NULL, "Name", 100, mIn), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgBoolTests, updateBoolValid)
{
    ASSERT_EQ(mamaMsg_updateBool(mMsg, "Name", 100, mUpdate), MAMA_STATUS_OK);
}

TEST_F (MsgBoolTests, updateBoolInValid)
{
    ASSERT_EQ(mamaMsg_updateBool(NULL, "Name", 100, mUpdate), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgBoolTests, getBoolValid)
{
    ASSERT_EQ(mamaMsg_addBool(mMsg, "Name", 100, mIn), MAMA_STATUS_OK);
    ASSERT_EQ(mamaMsg_getBool(mMsg, "Name", 100, &mOut), MAMA_STATUS_OK);
}

TEST_F (MsgBoolTests, getBoolInValid)
{
    ASSERT_EQ(mamaMsg_addBool(mMsg, "Name", 100, mIn), MAMA_STATUS_OK);
    ASSERT_EQ(mamaMsg_getBool(NULL, "Name", 100, &mOut), MAMA_STATUS_NULL_ARG);
}


/**********************************************************************
 * Char Tests
 **********************************************************************/
class MsgCharTests : public MsgAtomicTestsC
{
protected:

    char mIn, mUpdate, mOut;

    MsgCharTests()
        : mIn(true)
        , mUpdate(false)
        , mOut(false)
    {
    }

};


TEST_F (MsgCharTests, addCharValid)
{
    ASSERT_EQ(mamaMsg_addChar(mMsg, "Name", 100, mIn), MAMA_STATUS_OK);
}

TEST_F (MsgCharTests, addCharInValid)
{
    ASSERT_EQ(mamaMsg_addChar(NULL, "Name", 100, mIn), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgCharTests, updateCharValid)
{
    ASSERT_EQ(mamaMsg_updateChar(mMsg, "Name", 100, mUpdate), MAMA_STATUS_OK);
}

TEST_F (MsgCharTests, updateCharInValid)
{
    ASSERT_EQ(mamaMsg_updateChar(NULL, "Name", 100, mUpdate), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgCharTests, getCharValid)
{
    ASSERT_EQ(mamaMsg_addChar(mMsg, "Name", 100, mIn), MAMA_STATUS_OK);
    ASSERT_EQ(mamaMsg_getChar(mMsg, "Name", 100, &mOut), MAMA_STATUS_OK);
}

TEST_F (MsgCharTests, getCharInValid)
{
    ASSERT_EQ(mamaMsg_addChar(mMsg, "Name", 100, mIn), MAMA_STATUS_OK);
    ASSERT_EQ(mamaMsg_getChar(NULL, "Name", 100, &mOut), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgCharTests, getCharInValidChar)
{
    ASSERT_EQ(mamaMsg_addChar(mMsg, "Name", 100, mIn), MAMA_STATUS_OK);
    ASSERT_EQ(mamaMsg_getChar(mMsg, "Name", 100, NULL), MAMA_STATUS_NULL_ARG);
}


/**********************************************************************
 * I8 Tests
 **********************************************************************/
class MsgI8Tests : public MsgAtomicTestsC
{
protected:

    mama_i8_t mIn, mUpdate, mOut;

    MsgI8Tests()
        : mIn(true)
        , mUpdate(false)
        , mOut(false)
    {
    }

};


TEST_F (MsgI8Tests, addI8Valid)
{
    ASSERT_EQ(mamaMsg_addI8(mMsg, "Name", 100, mIn), MAMA_STATUS_OK);
}

TEST_F (MsgI8Tests, addI8InValid)
{
    ASSERT_EQ(mamaMsg_addI8(NULL, "Name", 100, mIn), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgI8Tests, updateI8Valid)
{
    ASSERT_EQ(mamaMsg_updateI8(mMsg, "Name", 100, mUpdate), MAMA_STATUS_OK);
}

TEST_F (MsgI8Tests, updateI8InValid)
{
    ASSERT_EQ(mamaMsg_updateI8(NULL, "Name", 100, mUpdate), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgI8Tests, getI8Valid)
{
    ASSERT_EQ(mamaMsg_addI8(mMsg, "Name", 100, mIn), MAMA_STATUS_OK);
    ASSERT_EQ(mamaMsg_getI8(mMsg, "Name", 100, &mOut), MAMA_STATUS_OK);
}

TEST_F (MsgI8Tests, getI8InValid)
{
    ASSERT_EQ(mamaMsg_addI8(mMsg, "Name", 100, mIn), MAMA_STATUS_OK);
    ASSERT_EQ(mamaMsg_getI8(NULL, "Name", 100, &mOut), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgI8Tests, getI8InValidI8)
{
    ASSERT_EQ(mamaMsg_addI8(mMsg, "Name", 100, mIn), MAMA_STATUS_OK);
    ASSERT_EQ(mamaMsg_getI8(mMsg, "Name", 100, NULL), MAMA_STATUS_NULL_ARG);
}

/**********************************************************************
 * U8 Tests
 **********************************************************************/
class MsgU8Tests : public MsgAtomicTestsC
{
protected:

    mama_u8_t mIn, mUpdate, mOut;

    MsgU8Tests()
        : mIn(true)
        , mUpdate(false)
        , mOut(false)
    {
    }

};


TEST_F (MsgU8Tests, addU8Valid)
{
    ASSERT_EQ(mamaMsg_addU8(mMsg, "Name", 100, mIn), MAMA_STATUS_OK);
}

TEST_F (MsgU8Tests, addU8InValid)
{
    ASSERT_EQ(mamaMsg_addU8(NULL, "Name", 100, mIn), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgU8Tests, updateU8Valid)
{
    ASSERT_EQ(mamaMsg_updateU8(mMsg, "Name", 100, mUpdate), MAMA_STATUS_OK);
}

TEST_F (MsgU8Tests, updateU8InValid)
{
    ASSERT_EQ(mamaMsg_updateU8(NULL, "Name", 100, mUpdate), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgU8Tests, getU8Valid)
{
    ASSERT_EQ(mamaMsg_addU8(mMsg, "Name", 100, mIn), MAMA_STATUS_OK);
    ASSERT_EQ(mamaMsg_getU8(mMsg, "Name", 100, &mOut), MAMA_STATUS_OK);
}

TEST_F (MsgU8Tests, getU8InValid)
{
    ASSERT_EQ(mamaMsg_addU8(mMsg, "Name", 100, mIn), MAMA_STATUS_OK);
    ASSERT_EQ(mamaMsg_getU8(NULL, "Name", 100, &mOut), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgU8Tests, getU8InValidU8)
{
    ASSERT_EQ(mamaMsg_addU8(mMsg, "Name", 100, mIn), MAMA_STATUS_OK);
    ASSERT_EQ(mamaMsg_getU8(mMsg, "Name", 100, NULL), MAMA_STATUS_NULL_ARG);
}

/**********************************************************************
 * I16 Tests
 **********************************************************************/
class MsgI16Tests : public MsgAtomicTestsC
{
protected:

    mama_i16_t mIn, mUpdate, mOut;

    MsgI16Tests()
        : mIn(true)
        , mUpdate(false)
        , mOut(false)
    {
    }

};


TEST_F (MsgI16Tests, addI16Valid)
{
    ASSERT_EQ(mamaMsg_addI16(mMsg, "Name", 100, mIn), MAMA_STATUS_OK);
}

TEST_F (MsgI16Tests, addI16InValid)
{
    ASSERT_EQ(mamaMsg_addI16(NULL, "Name", 100, mIn), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgI16Tests, updateI16Valid)
{
    ASSERT_EQ(mamaMsg_updateI16(mMsg, "Name", 100, mUpdate), MAMA_STATUS_OK);
}

TEST_F (MsgI16Tests, updateI16InValid)
{
    ASSERT_EQ(mamaMsg_updateI16(NULL, "Name", 100, mUpdate), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgI16Tests, getI16Valid)
{
    ASSERT_EQ(mamaMsg_addI16(mMsg, "Name", 100, mIn), MAMA_STATUS_OK);
    ASSERT_EQ(mamaMsg_getI16(mMsg, "Name", 100, &mOut), MAMA_STATUS_OK);
}

TEST_F (MsgI16Tests, getI16InValid)
{
    ASSERT_EQ(mamaMsg_addI16(mMsg, "Name", 100, mIn), MAMA_STATUS_OK);
    ASSERT_EQ(mamaMsg_getI16(NULL, "Name", 100, &mOut), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgI16Tests, getI16InValidI16)
{
    ASSERT_EQ(mamaMsg_addI16(mMsg, "Name", 100, mIn), MAMA_STATUS_OK);
    ASSERT_EQ(mamaMsg_getI16(mMsg, "Name", 100, NULL), MAMA_STATUS_NULL_ARG);
}

/**********************************************************************
 * U16 Tests
 **********************************************************************/
class MsgU16Tests : public MsgAtomicTestsC
{
protected:

    mama_u16_t mIn, mUpdate, mOut;

    MsgU16Tests()
        : mIn(true)
        , mUpdate(false)
        , mOut(false)
    {
    }

};


TEST_F (MsgU16Tests, addU16Valid)
{
    ASSERT_EQ(mamaMsg_addU16(mMsg, "Name", 100, mIn), MAMA_STATUS_OK);
}

TEST_F (MsgU16Tests, addU16InValid)
{
    ASSERT_EQ(mamaMsg_addU16(NULL, "Name", 100, mIn), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgU16Tests, updateU16Valid)
{
    ASSERT_EQ(mamaMsg_updateU16(mMsg, "Name", 100, mUpdate), MAMA_STATUS_OK);
}

TEST_F (MsgU16Tests, updateU16InValid)
{
    ASSERT_EQ(mamaMsg_updateU16(NULL, "Name", 100, mUpdate), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgU16Tests, getU16Valid)
{
    ASSERT_EQ(mamaMsg_addU16(mMsg, "Name", 100, mIn), MAMA_STATUS_OK);
    ASSERT_EQ(mamaMsg_getU16(mMsg, "Name", 100, &mOut), MAMA_STATUS_OK);
}

TEST_F (MsgU16Tests, getU16InValid)
{
    ASSERT_EQ(mamaMsg_addU16(mMsg, "Name", 100, mIn), MAMA_STATUS_OK);
    ASSERT_EQ(mamaMsg_getU16(NULL, "Name", 100, &mOut), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgU16Tests, getU16InValidU16)
{
    ASSERT_EQ(mamaMsg_addU16(mMsg, "Name", 100, mIn), MAMA_STATUS_OK);
    ASSERT_EQ(mamaMsg_getU16(mMsg, "Name", 100, NULL), MAMA_STATUS_NULL_ARG);
}

/**********************************************************************
 * I32 Tests
 **********************************************************************/
class MsgI32Tests : public MsgAtomicTestsC
{
protected:

    mama_i32_t mIn, mUpdate, mOut;

    MsgI32Tests()
        : mIn(true)
        , mUpdate(false)
        , mOut(false)
    {
    }

};


TEST_F (MsgI32Tests, addI32Valid)
{
    ASSERT_EQ(mamaMsg_addI32(mMsg, "Name", 100, mIn), MAMA_STATUS_OK);
}

TEST_F (MsgI32Tests, addI32InValid)
{
    ASSERT_EQ(mamaMsg_addI32(NULL, "Name", 100, mIn), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgI32Tests, updateI32Valid)
{
    ASSERT_EQ(mamaMsg_updateI32(mMsg, "Name", 100, mUpdate), MAMA_STATUS_OK);
}

TEST_F (MsgI32Tests, updateI32InValid)
{
    ASSERT_EQ(mamaMsg_updateI32(NULL, "Name", 100, mUpdate), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgI32Tests, getI32Valid)
{
    ASSERT_EQ(mamaMsg_addI32(mMsg, "Name", 100, mIn), MAMA_STATUS_OK);
    ASSERT_EQ(mamaMsg_getI32(mMsg, "Name", 100, &mOut), MAMA_STATUS_OK);
}

TEST_F (MsgI32Tests, getI32InValid)
{
    ASSERT_EQ(mamaMsg_addI32(mMsg, "Name", 100, mIn), MAMA_STATUS_OK);
    ASSERT_EQ(mamaMsg_getI32(NULL, "Name", 100, &mOut), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgI32Tests, getI32InValidI32)
{
    ASSERT_EQ(mamaMsg_addI32(mMsg, "Name", 100, mIn), MAMA_STATUS_OK);
    ASSERT_EQ(mamaMsg_getI32(mMsg, "Name", 100, NULL), MAMA_STATUS_NULL_ARG);
}

/**********************************************************************
 * U32 Tests
 **********************************************************************/
class MsgU32Tests : public MsgAtomicTestsC
{
protected:

    mama_u32_t mIn, mUpdate, mOut;

    MsgU32Tests()
        : mIn(true)
        , mUpdate(false)
        , mOut(false)
    {
    }

};


TEST_F (MsgU32Tests, addU32Valid)
{
    ASSERT_EQ(mamaMsg_addU32(mMsg, "Name", 100, mIn), MAMA_STATUS_OK);
}

TEST_F (MsgU32Tests, addU32InValid)
{
    ASSERT_EQ(mamaMsg_addU32(NULL, "Name", 100, mIn), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgU32Tests, updateU32Valid)
{
    ASSERT_EQ(mamaMsg_updateU32(mMsg, "Name", 100, mUpdate), MAMA_STATUS_OK);
}

TEST_F (MsgU32Tests, updateU32InValid)
{
    ASSERT_EQ(mamaMsg_updateU32(NULL, "Name", 100, mUpdate), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgU32Tests, getU32Valid)
{
    ASSERT_EQ(mamaMsg_addU32(mMsg, "Name", 100, mIn), MAMA_STATUS_OK);
    ASSERT_EQ(mamaMsg_getU32(mMsg, "Name", 100, &mOut), MAMA_STATUS_OK);
}

TEST_F (MsgU32Tests, getU32InValid)
{
    ASSERT_EQ(mamaMsg_addU32(mMsg, "Name", 100, mIn), MAMA_STATUS_OK);
    ASSERT_EQ(mamaMsg_getU32(NULL, "Name", 100, &mOut), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgU32Tests, getU32InValidU32)
{
    ASSERT_EQ(mamaMsg_addU32(mMsg, "Name", 100, mIn), MAMA_STATUS_OK);
    ASSERT_EQ(mamaMsg_getU32(mMsg, "Name", 100, NULL), MAMA_STATUS_NULL_ARG);
}

/**********************************************************************
 * I64 Tests
 **********************************************************************/
class MsgI64Tests : public MsgAtomicTestsC
{
protected:

    mama_i64_t mIn, mUpdate, mOut;

    MsgI64Tests()
        : mIn(true)
        , mUpdate(false)
        , mOut(false)
    {
    }

};


TEST_F (MsgI64Tests, addI64Valid)
{
    ASSERT_EQ(mamaMsg_addI64(mMsg, "Name", 100, mIn), MAMA_STATUS_OK);
}

TEST_F (MsgI64Tests, addI64InValid)
{
    ASSERT_EQ(mamaMsg_addI64(NULL, "Name", 100, mIn), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgI64Tests, updateI64Valid)
{
    ASSERT_EQ(mamaMsg_updateI64(mMsg, "Name", 100, mUpdate), MAMA_STATUS_OK);
}

TEST_F (MsgI64Tests, updateI64InValid)
{
    ASSERT_EQ(mamaMsg_updateI64(NULL, "Name", 100, mUpdate), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgI64Tests, getI64Valid)
{
    ASSERT_EQ(mamaMsg_addI64(mMsg, "Name", 100, mIn), MAMA_STATUS_OK);
    ASSERT_EQ(mamaMsg_getI64(mMsg, "Name", 100, &mOut), MAMA_STATUS_OK);
}

TEST_F (MsgI64Tests, getI64InValid)
{
    ASSERT_EQ(mamaMsg_addI64(mMsg, "Name", 100, mIn), MAMA_STATUS_OK);
    ASSERT_EQ(mamaMsg_getI64(NULL, "Name", 100, &mOut), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgI64Tests, getI64InValidI64)
{
    ASSERT_EQ(mamaMsg_addI64(mMsg, "Name", 100, mIn), MAMA_STATUS_OK);
    ASSERT_EQ(mamaMsg_getI64(mMsg, "Name", 100, NULL), MAMA_STATUS_NULL_ARG);
}

/**********************************************************************
 * U64 Tests
 **********************************************************************/
class MsgU64Tests : public MsgAtomicTestsC
{
protected:

    mama_u64_t mIn, mUpdate, mOut;

    MsgU64Tests()
        : mIn(true)
        , mUpdate(false)
        , mOut(false)
    {
    }

};


TEST_F (MsgU64Tests, addU64Valid)
{
    ASSERT_EQ(mamaMsg_addU64(mMsg, "Name", 100, mIn), MAMA_STATUS_OK);
}

TEST_F (MsgU64Tests, addU64InValid)
{
    ASSERT_EQ(mamaMsg_addU64(NULL, "Name", 100, mIn), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgU64Tests, updateU64Valid)
{
    ASSERT_EQ(mamaMsg_updateU64(mMsg, "Name", 100, mUpdate), MAMA_STATUS_OK);
}

TEST_F (MsgU64Tests, updateU64InValid)
{
    ASSERT_EQ(mamaMsg_updateU64(NULL, "Name", 100, mUpdate), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgU64Tests, getU64Valid)
{
    ASSERT_EQ(mamaMsg_addU64(mMsg, "Name", 100, mIn), MAMA_STATUS_OK);
    ASSERT_EQ(mamaMsg_getU64(mMsg, "Name", 100, &mOut), MAMA_STATUS_OK);
}

TEST_F (MsgU64Tests, getU64InValid)
{
    ASSERT_EQ(mamaMsg_addU64(mMsg, "Name", 100, mIn), MAMA_STATUS_OK);
    ASSERT_EQ(mamaMsg_getU64(NULL, "Name", 100, &mOut), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgU64Tests, getU64InValidU64)
{
    ASSERT_EQ(mamaMsg_addU64(mMsg, "Name", 100, mIn), MAMA_STATUS_OK);
    ASSERT_EQ(mamaMsg_getU64(mMsg, "Name", 100, NULL), MAMA_STATUS_NULL_ARG);
}

/**********************************************************************
 * F32 Tests
 **********************************************************************/
class MsgF32Tests : public MsgAtomicTestsC
{
protected:

    mama_f32_t mIn, mUpdate, mOut;

    MsgF32Tests()
        : mIn(true)
        , mUpdate(false)
        , mOut(false)
    {
    }

};


TEST_F (MsgF32Tests, addF32Valid)
{
    ASSERT_EQ(mamaMsg_addF32(mMsg, "Name", 100, mIn), MAMA_STATUS_OK);
}

TEST_F (MsgF32Tests, addF32InValid)
{
    ASSERT_EQ(mamaMsg_addF32(NULL, "Name", 100, mIn), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgF32Tests, updateF32Valid)
{
    ASSERT_EQ(mamaMsg_updateF32(mMsg, "Name", 100, mUpdate), MAMA_STATUS_OK);
}

TEST_F (MsgF32Tests, updateF32InValid)
{
    ASSERT_EQ(mamaMsg_updateF32(NULL, "Name", 100, mUpdate), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgF32Tests, getF32Valid)
{
    ASSERT_EQ(mamaMsg_addF32(mMsg, "Name", 100, mIn), MAMA_STATUS_OK);
    ASSERT_EQ(mamaMsg_getF32(mMsg, "Name", 100, &mOut), MAMA_STATUS_OK);
}

TEST_F (MsgF32Tests, getF32InValid)
{
    ASSERT_EQ(mamaMsg_addF32(mMsg, "Name", 100, mIn), MAMA_STATUS_OK);
    ASSERT_EQ(mamaMsg_getF32(NULL, "Name", 100, &mOut), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgF32Tests, getF32InValidF32)
{
    ASSERT_EQ(mamaMsg_addF32(mMsg, "Name", 100, mIn), MAMA_STATUS_OK);
    ASSERT_EQ(mamaMsg_getF32(mMsg, "Name", 100, NULL), MAMA_STATUS_NULL_ARG);
}

/**********************************************************************
 * F64 Tests
 **********************************************************************/
class MsgF64Tests : public MsgAtomicTestsC
{
protected:

    mama_f64_t mIn, mUpdate, mOut;

    MsgF64Tests()
        : mIn(true)
        , mUpdate(false)
        , mOut(false)
    {
    }

};


TEST_F (MsgF64Tests, addF64Valid)
{
    ASSERT_EQ(mamaMsg_addF64(mMsg, "Name", 100, mIn), MAMA_STATUS_OK);
}

TEST_F (MsgF64Tests, addF64InValid)
{
    ASSERT_EQ(mamaMsg_addF64(NULL, "Name", 100, mIn), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgF64Tests, updateF64Valid)
{
    ASSERT_EQ(mamaMsg_updateF64(mMsg, "Name", 100, mUpdate), MAMA_STATUS_OK);
}

TEST_F (MsgF64Tests, updateF64InValid)
{
    ASSERT_EQ(mamaMsg_updateF64(NULL, "Name", 100, mUpdate), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgF64Tests, getF64Valid)
{
    ASSERT_EQ(mamaMsg_addF64(mMsg, "Name", 100, mIn), MAMA_STATUS_OK);
    ASSERT_EQ(mamaMsg_getF64(mMsg, "Name", 100, &mOut), MAMA_STATUS_OK);
}

TEST_F (MsgF64Tests, getF64InValid)
{
    ASSERT_EQ(mamaMsg_addF64(mMsg, "Name", 100, mIn), MAMA_STATUS_OK);
    ASSERT_EQ(mamaMsg_getF64(NULL, "Name", 100, &mOut), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgF64Tests, getF64InValidF64)
{
    ASSERT_EQ(mamaMsg_addF64(mMsg, "Name", 100, mIn), MAMA_STATUS_OK);
    ASSERT_EQ(mamaMsg_getF64(mMsg, "Name", 100, NULL), MAMA_STATUS_NULL_ARG);
}

