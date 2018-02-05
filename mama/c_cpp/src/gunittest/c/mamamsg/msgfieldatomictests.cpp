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

class MsgFieldAtomicTestsC : public ::testing::Test
{
protected:
    MsgFieldAtomicTestsC(void) :
        mMsg            (NULL),
        mMsgField       (NULL),
        mPayloadBridge  (NULL)
    {}
        
    virtual ~MsgFieldAtomicTestsC(void) {};

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
    mamaMsgField       mMsgField;
    mamaPayloadBridge  mPayloadBridge;
};

/*********************************************************
 * BOOL TESTS
 *********************************************************/

class MsgFieldBoolTests : public MsgFieldAtomicTestsC
{
protected:

	mama_bool_t mUpdate, mOut;

	MsgFieldBoolTests()
		: mUpdate(true)
		, mOut(false)
	{
	}

    virtual void SetUp()
    {
        MsgFieldAtomicTestsC::SetUp();

        mamaMsg_addBool( mMsg, "name", 1, mUpdate);
    }
};

TEST_F (MsgFieldBoolTests, updateBoolValid)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField);
    ASSERT_EQ( mamaMsgField_updateBool(	mMsgField, mUpdate ), MAMA_STATUS_OK);  
}

TEST_F (MsgFieldBoolTests, updateBoolInvalid)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_updateBool( NULL, mUpdate ), MAMA_STATUS_NULL_ARG );
}

TEST_F (MsgFieldBoolTests, getBoolValid)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_getBool( mMsgField, &mOut ), MAMA_STATUS_OK );
    ASSERT_EQ( mOut, true );
}


TEST_F (MsgFieldBoolTests, getBoolInvalid)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_getBool( NULL, &mOut ), MAMA_STATUS_NULL_ARG );
    ASSERT_EQ( mOut, false );
}

TEST_F (MsgFieldBoolTests, getBoolInvalidBool)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_getBool( mMsgField, NULL ), MAMA_STATUS_NULL_ARG );
}

/*********************************************************
 * CHAR TESTS
 *********************************************************/

class MsgFieldCharTests : public MsgFieldAtomicTestsC
{
protected:

    char mUpdate, mOut;

    MsgFieldCharTests()
        : mUpdate('a')
        , mOut('b')
    {
    }

    virtual void SetUp()
    {
        MsgFieldAtomicTestsC::SetUp();

        mamaMsg_addChar( mMsg, "name", 1, mUpdate);
    }

    virtual void TearDown()
    {
        MsgFieldAtomicTestsC::TearDown();
    }
};

TEST_F (MsgFieldCharTests, DISABLED_updateCharValid)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_updateChar( mMsgField, mUpdate ), MAMA_STATUS_OK );
}


TEST_F (MsgFieldCharTests, updateCharInvalid)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_updateChar( NULL, mUpdate ), MAMA_STATUS_NULL_ARG );
}

TEST_F (MsgFieldCharTests, getCharValid)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_getChar( mMsgField, &mOut ), MAMA_STATUS_OK );
    ASSERT_EQ( mOut, 'a' );
}

TEST_F (MsgFieldCharTests, getCharInvalid)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_getChar( NULL, &mOut ), MAMA_STATUS_NULL_ARG );
    ASSERT_EQ( mOut, 'b' );
}

TEST_F (MsgFieldCharTests, getCharInvalidChar)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_getChar( mMsgField, NULL ), MAMA_STATUS_NULL_ARG );
}

/*********************************************************
 * I8 TESTS
 *********************************************************/

class MsgFieldI8Tests : public MsgFieldAtomicTestsC
{
protected:

    mama_i8_t mUpdate, mOut;

    MsgFieldI8Tests()
        : mUpdate(123)
        , mOut(23)
    {
    }

    virtual void SetUp()
    {
        MsgFieldAtomicTestsC::SetUp();
        mamaMsg_addI8( mMsg, "name", 1, mUpdate);
    }
};

TEST_F (MsgFieldI8Tests, updateI8Valid)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_updateI8( mMsgField, mUpdate ), MAMA_STATUS_OK );
}

TEST_F (MsgFieldI8Tests, updateI8Invalid)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_updateI8( NULL, mUpdate ), MAMA_STATUS_NULL_ARG );
}

TEST_F (MsgFieldI8Tests, getI8Valid)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_getI8( mMsgField, &mOut ), MAMA_STATUS_OK );
    ASSERT_EQ( mOut, 123 );
}

TEST_F (MsgFieldI8Tests, getI8Invalid)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_getI8( NULL, &mOut ), MAMA_STATUS_NULL_ARG );
    ASSERT_EQ( mOut, 23 );
}

TEST_F (MsgFieldI8Tests, getI8InvalidI8)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_getI8( mMsgField, NULL ), MAMA_STATUS_NULL_ARG );
}

/*********************************************************
 * U8 TESTS
 *********************************************************/

class MsgFieldU8Tests : public MsgFieldAtomicTestsC
{
protected:

    mama_u8_t mUpdate, mOut;

    MsgFieldU8Tests()
        : mUpdate(111)
        , mOut(11)
    {
    }

    virtual void SetUp()
    {
        MsgFieldAtomicTestsC::SetUp();
        mamaMsg_addU8( mMsg, "name", 1, mUpdate );
    }
};

TEST_F (MsgFieldU8Tests, updateU8Valid)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_updateU8( mMsgField, mUpdate ), MAMA_STATUS_OK );
}

TEST_F (MsgFieldU8Tests, updateU8Invalid)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_updateU8( NULL, mUpdate ), MAMA_STATUS_NULL_ARG );
}

TEST_F (MsgFieldU8Tests, getU8Valid)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_getU8( mMsgField, &mOut ), MAMA_STATUS_OK );
    ASSERT_EQ( mOut, 111 );
}

TEST_F (MsgFieldU8Tests, getU8Invalid)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_getU8( NULL, &mOut ), MAMA_STATUS_NULL_ARG );
    ASSERT_EQ( mOut, 11 );
}

TEST_F (MsgFieldU8Tests, getU8InvalidU8)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_getU8( mMsgField, NULL ), MAMA_STATUS_NULL_ARG );
}

/*********************************************************
 * I16 TESTS
 *********************************************************/

class MsgFieldI16Tests : public MsgFieldAtomicTestsC
{
protected:

    mama_i16_t mUpdate, mOut;

    MsgFieldI16Tests()
        : mUpdate(12345)
        , mOut(1234)
    {
    }

    virtual void SetUp()
    {
        MsgFieldAtomicTestsC::SetUp();
        mamaMsg_addI16( mMsg, "name", 1, mUpdate );
    }
};

TEST_F (MsgFieldI16Tests, updateI16Valid)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_updateI16( mMsgField, mUpdate ), MAMA_STATUS_OK);
}

TEST_F (MsgFieldI16Tests, updateI16Invalid)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_updateI16( NULL, mUpdate ), MAMA_STATUS_NULL_ARG );
}

TEST_F (MsgFieldI16Tests, getI16Valid)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_getI16( mMsgField, &mOut ), MAMA_STATUS_OK );
    ASSERT_EQ( mOut, 12345 );
}

TEST_F (MsgFieldI16Tests, getI16Invalid)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_getI16( NULL, &mOut ), MAMA_STATUS_NULL_ARG );
    ASSERT_EQ( mOut, 1234 );
}

TEST_F (MsgFieldI16Tests, getI16InvalidI16)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_getI16( mMsgField, NULL ), MAMA_STATUS_NULL_ARG );
}

/*********************************************************
 * U16 TESTS
 *********************************************************/

class MsgFieldU16Tests : public MsgFieldAtomicTestsC
{
protected:

    mama_u16_t mUpdate, mOut;

    MsgFieldU16Tests()
        : mUpdate(12345)
        , mOut(1234)
    {
    }

    virtual void SetUp()
    {
        MsgFieldAtomicTestsC::SetUp();
        mamaMsg_addU16( mMsg, "name", 1, mUpdate );
    }
};

TEST_F (MsgFieldU16Tests, DISABLED_updateU16Valid)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_updateU16( mMsgField, mUpdate ), MAMA_STATUS_OK );
}

TEST_F (MsgFieldU16Tests, updateU16Invalid)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_updateU16( NULL, mUpdate ), MAMA_STATUS_NULL_ARG );
}

TEST_F (MsgFieldU16Tests, getU16Valid)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_getU16( mMsgField, &mOut ), MAMA_STATUS_OK );
    ASSERT_EQ( mOut, 12345 );
}

TEST_F (MsgFieldU16Tests, getU16Invalid)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_getU16( NULL, &mOut ), MAMA_STATUS_NULL_ARG );
    ASSERT_EQ( mOut, 1234 );
}

TEST_F (MsgFieldU16Tests, getU16InvalidU16)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_getU16( mMsgField, NULL ), MAMA_STATUS_NULL_ARG );
}

/*********************************************************
 * I32 TESTS
 *********************************************************/

class MsgFieldI32Tests : public MsgFieldAtomicTestsC
{
protected:

    mama_i32_t mUpdate, mOut;

    MsgFieldI32Tests()
        : mUpdate(123456)
        , mOut(12345)
    {
    }

    virtual void SetUp()
    {
        MsgFieldAtomicTestsC::SetUp();
        mamaMsg_addI32( mMsg, "name", 1, mUpdate);
    }
};

TEST_F (MsgFieldI32Tests, updateI32Valid)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_updateI32( mMsgField, mUpdate ), MAMA_STATUS_OK );
}

TEST_F (MsgFieldI32Tests, updateI32Invalid)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_updateI32( NULL, mUpdate ), MAMA_STATUS_NULL_ARG );
}

TEST_F (MsgFieldI32Tests, getI32Valid)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_getI32( mMsgField, &mOut ), MAMA_STATUS_OK );
    ASSERT_EQ( mOut, mUpdate );
}

TEST_F (MsgFieldI32Tests, getI32Invalid)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_getI32( NULL, &mOut ), MAMA_STATUS_NULL_ARG );
    ASSERT_EQ( mOut, 12345 );
}

TEST_F (MsgFieldI32Tests, getI32InvalidI32)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_getI32( mMsgField, NULL ), MAMA_STATUS_NULL_ARG );
}

/*********************************************************
 * U32 TESTS
 *********************************************************/

class MsgFieldU32Tests : public MsgFieldAtomicTestsC
{
protected:

    mama_u32_t mUpdate, mOut;

    MsgFieldU32Tests()
        : mUpdate(123456)
        , mOut(12345)
    {
    }

    virtual void SetUp()
    {
        MsgFieldAtomicTestsC::SetUp();
        mamaMsg_addU32( mMsg, "name", 1, mUpdate );
    }
};

TEST_F (MsgFieldU32Tests, DISABLED_updateU32Valid)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_updateU32( mMsgField, mUpdate ), MAMA_STATUS_OK );
}

TEST_F (MsgFieldU32Tests, updateU32Invalid)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_updateU32( NULL, mUpdate ), MAMA_STATUS_NULL_ARG );
}

TEST_F (MsgFieldU32Tests, getU32Valid)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_getU32( mMsgField, &mOut ), MAMA_STATUS_OK );
    ASSERT_EQ( mOut, 123456U );
}

TEST_F (MsgFieldU32Tests, getU32Invalid)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_getU32( NULL, &mOut ), MAMA_STATUS_NULL_ARG );
    ASSERT_EQ( mOut, 12345U );
}

TEST_F (MsgFieldU32Tests, getU32InvalidU32)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_getU32( mMsgField, NULL ), MAMA_STATUS_NULL_ARG );
}

/*********************************************************
 * I64 TESTS
 *********************************************************/

class MsgFieldI64Tests : public MsgFieldAtomicTestsC
{
protected:

    mama_i64_t mUpdate, mOut;

    MsgFieldI64Tests()
        : mUpdate(1234567)
        , mOut(123456)
    {
    }

    virtual void SetUp()
    {
        MsgFieldAtomicTestsC::SetUp();
        mamaMsg_addI64( mMsg, "name", 1, mUpdate );
    }
};

TEST_F (MsgFieldI64Tests, updateI64Valid)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_updateI64( mMsgField, mUpdate ), MAMA_STATUS_OK );
}

TEST_F (MsgFieldI64Tests, updateI64Invalid)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_updateI64( NULL, mUpdate ), MAMA_STATUS_NULL_ARG );
}

TEST_F (MsgFieldI64Tests, getI64Valid)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_getI64( mMsgField, &mOut ), MAMA_STATUS_OK );
    ASSERT_EQ( mOut, 1234567 );
}

TEST_F (MsgFieldI64Tests, getI64Invalid)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_getI64( NULL, &mOut ), MAMA_STATUS_NULL_ARG );
    ASSERT_EQ( mOut, 123456 );
}

TEST_F (MsgFieldI64Tests, getI64InvalidI64)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_getI64( mMsgField, NULL ), MAMA_STATUS_NULL_ARG );
}

/*********************************************************
 * U64 TESTS
 *********************************************************/

class MsgFieldU64Tests : public MsgFieldAtomicTestsC
{
protected:

    mama_u64_t mUpdate, mOut;

    MsgFieldU64Tests()
        : mUpdate(1234567)
        , mOut(123456)
    {
    }

    virtual void SetUp()
    {
        MsgFieldAtomicTestsC::SetUp();
        mamaMsg_addU64( mMsg, "name", 1, mUpdate );
    }
};

TEST_F (MsgFieldU64Tests, updateU64Valid)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_updateU64( mMsgField, mUpdate ), MAMA_STATUS_OK );
}

TEST_F (MsgFieldU64Tests, updateU64Invalid)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_updateU64( NULL, mUpdate ), MAMA_STATUS_NULL_ARG );
}

TEST_F (MsgFieldU64Tests, getU64Valid)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_getU64( mMsgField, &mOut ), MAMA_STATUS_OK );
    ASSERT_EQ( mOut, 1234567U );
}

TEST_F (MsgFieldU64Tests, getU64Invalid)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_getU64( NULL, &mOut ), MAMA_STATUS_NULL_ARG );
    ASSERT_EQ( mOut, 123456U );
}

TEST_F (MsgFieldU64Tests, getU64InvalidU64)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_getU64( mMsgField, NULL ), MAMA_STATUS_NULL_ARG );
}

/*********************************************************
 * F32 TESTS
 *********************************************************/

class MsgFieldF32Tests : public MsgFieldAtomicTestsC
{
protected:

    mama_f32_t mUpdate, mOut;

    MsgFieldF32Tests()
        : mUpdate( 5.2f )
        , mOut( 2.6f )
    {
    }

    virtual void SetUp()
    {
        MsgFieldAtomicTestsC::SetUp();
        mamaMsg_addF32( mMsg, "name", 1, mUpdate );
    }

    virtual void TearDown()
    {
        MsgFieldAtomicTestsC::TearDown();
    }
};

TEST_F (MsgFieldF32Tests, updateF32Valid)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_updateF32( mMsgField, mUpdate ), MAMA_STATUS_OK );
}

TEST_F (MsgFieldF32Tests, updateF32Invalid)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_updateF32( NULL, mUpdate ), MAMA_STATUS_NULL_ARG );
}

TEST_F (MsgFieldF32Tests, getF32Valid)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_getF32( mMsgField, &mOut ), MAMA_STATUS_OK );
    ASSERT_EQ( mOut, 5.2f );
}

TEST_F (MsgFieldF32Tests, getF32Invalid)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_getF32( NULL, &mOut ), MAMA_STATUS_NULL_ARG );
    ASSERT_EQ( mOut, 2.6f );
}

TEST_F (MsgFieldF32Tests, getF32InvalidF32)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_getF32( mMsgField, NULL ), MAMA_STATUS_NULL_ARG );
}

/*********************************************************
 * F64 TESTS
 *********************************************************/

class MsgFieldF64Tests : public MsgFieldAtomicTestsC
{
protected:

    mama_f64_t mUpdate, mOut;

    MsgFieldF64Tests()
        : mUpdate( 5.2f )
        , mOut( 2.6f )
    {
    }

    virtual void SetUp()
    {
        MsgFieldAtomicTestsC::SetUp();
        mamaMsg_addF64( mMsg, "name", 1, mUpdate );
    }

    virtual void TearDown()
    {
        MsgFieldAtomicTestsC::TearDown();
    }
};

TEST_F (MsgFieldF64Tests, updateF64Valid)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_updateF64( mMsgField, mUpdate ), MAMA_STATUS_OK );
}

TEST_F (MsgFieldF64Tests, updateF64Invalid)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_updateF64( NULL, mUpdate ), MAMA_STATUS_NULL_ARG );
}

TEST_F (MsgFieldF64Tests, getF64Valid)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_getF64( mMsgField, &mOut ), MAMA_STATUS_OK );
    ASSERT_EQ( mOut, 5.2f );
}

TEST_F (MsgFieldF64Tests, getF64Invalid)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_getF64( NULL, &mOut ), MAMA_STATUS_NULL_ARG );
    ASSERT_EQ( mOut, 2.6f );
}

TEST_F (MsgFieldF64Tests, getF64InvalidF64)
{
    mamaMsg_getField( mMsg, "name", 1, &mMsgField );
    ASSERT_EQ( mamaMsgField_getF64( mMsgField, NULL ), MAMA_STATUS_NULL_ARG );
}

