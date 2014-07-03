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
#include "msgimpl.h"
#include "MainUnitTestC.h"
#include <iostream>
#include <cstring>

using std::cout;
using std::endl;

#define  VECTOR_SIZE                ( 10U )
#define  VECTOR_UPDATE_SIZE         ( 20U )

class MsgVectorTestsC : public ::testing::Test
{
protected:
    MsgVectorTestsC(void) 
        : mMsg           (NULL)
        , mPayloadBridge (NULL)
        , mStatus        (MAMA_STATUS_OK)
    {}
    virtual ~MsgVectorTestsC(void) {};

    virtual void SetUp(void) 
    {
        mama_loadPayloadBridge (&mPayloadBridge, getPayload());
        mamaMsg_create (&mMsg);
        mStatus = MAMA_STATUS_OK;
    };

    virtual void TearDown(void) 
    {
        mamaMsg_destroy(mMsg);
    };
    
    mamaMsg            mMsg;
    mamaPayloadBridge  mPayloadBridge;
    mama_status        mStatus;
};


// ****************************************************************************
// Bool Test Suite
// ****************************************************************************

class MsgVectorBoolTestsC : public MsgVectorTestsC
{
public:

    virtual ~MsgVectorBoolTestsC()
    {}

protected:

    mama_bool_t mIn[ VECTOR_SIZE ];
    mama_bool_t mUpdate[ VECTOR_UPDATE_SIZE ];
    const mama_bool_t *mOut;
    mama_size_t mOutSize;

    MsgVectorBoolTestsC() 
        : mIn( )
        , mUpdate( )
        , mOut( NULL )
        , mOutSize( 0 )
    {}
 
    virtual void SetUp()
    {
        MsgVectorTestsC::SetUp();

        for( uint32_t ii(0) ; ii < VECTOR_SIZE ; ++ii )
        {
            mIn[ii] = ( 0 == ii%2 ) ? 1 : 0;
        }

        for( uint32_t ii(0) ; ii < VECTOR_UPDATE_SIZE ; ++ii )
        {
            mUpdate[ii] = ( 0 == ii%2 ) ? 0 : 1;
        }
    }

    virtual void TearDown()
    {
        MsgVectorTestsC::TearDown();
    }
};

// AddVectorBool test fixtures
// ***************************

TEST_F(MsgVectorBoolTestsC, AddVectorBool)
{
    mStatus = mamaMsg_addVectorBool (mMsg,
                                     NULL,
                                     1,
                                     mIn,
                                     VECTOR_SIZE);

    EXPECT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorBool (mMsg,
                                     NULL,
                                     1,
                                     &mOut,
                                     &mOutSize);

    EXPECT_EQ( mStatus, MAMA_STATUS_OK );
 //   EXPECT_EQ( 0, ::memcmp( mIn, mOut, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_SIZE );
}

TEST_F(MsgVectorBoolTestsC, AddVectorBoolNullAdd)
{
    mStatus = mamaMsg_addVectorBool (mMsg,
                                     NULL,
                                     1,
                                     NULL,
                                     VECTOR_SIZE);

    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgVectorBoolTestsC, AddVectorBoolNullMsg)
{
    mStatus = mamaMsg_addVectorBool (NULL,
                                     NULL,
                                     1,
                                     mIn,
                                     VECTOR_SIZE);

    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

// UpdateVectorBool test fixtures
// ******************************

TEST_F(MsgVectorBoolTestsC, UpdateVectorBool)
{
    mStatus = mamaMsg_addVectorBool (mMsg,
                                     NULL,
                                     1,
                                     mIn,
                                     VECTOR_SIZE);

    EXPECT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorBool (mMsg,
                                     NULL,
                                     1,
                                     &mOut,
                                     &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( 0, ::memcmp( mIn, mOut, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_SIZE );

    mStatus = mamaMsg_updateVectorBool (mMsg,
                                        NULL,
                                        1,
                                        mUpdate,
                                        VECTOR_UPDATE_SIZE);

    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorBool (mMsg,
                                     NULL,
                                     1,
                                     &mOut,
                                     &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
    EXPECT_EQ( 0, ::memcmp( mUpdate, mOut, (sizeof(mama_bool_t) * VECTOR_UPDATE_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
}

TEST_F(MsgVectorBoolTestsC, UpdateVectorBoolNullAdd)
{
    mStatus = mamaMsg_updateVectorBool (mMsg,
                                        NULL,
                                        1,
                                        NULL,
                                        VECTOR_UPDATE_SIZE);

    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgVectorBoolTestsC, UpdateVectorBoolNullMsg)
{
    mStatus = mamaMsg_updateVectorBool (NULL,
                                        NULL,
                                        1,
                                        mUpdate,
                                        VECTOR_UPDATE_SIZE);

    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

// GetVectorBool test fixtures
// ***************************

TEST_F(MsgVectorBoolTestsC, GetVectorBool)
{
    mStatus = mamaMsg_addVectorBool (mMsg,
                                     NULL,
                                     1,
                                     mIn,
                                     VECTOR_SIZE);
    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorBool (mMsg,
                                     NULL,
                                     1,
                                     &mOut,
                                     &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( 0, ::memcmp( mIn, mOut, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_NE( 0, ::memcmp( mUpdate, mOut, (sizeof(mama_bool_t) * VECTOR_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_SIZE );

    mStatus = mamaMsg_updateVectorBool (mMsg,
                                        NULL,
                                        1,
                                        mUpdate,
                                        VECTOR_UPDATE_SIZE);
    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorBool (mMsg,
                                     NULL,
                                     1,
                                     &mOut,
                                     &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
    EXPECT_EQ( 0, ::memcmp( mUpdate, mOut, (sizeof(mama_bool_t) * VECTOR_UPDATE_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
    EXPECT_NE( mOutSize, VECTOR_SIZE );
}

TEST_F(MsgVectorBoolTestsC, GetVectorBoolNullAdd)
{
    mStatus = mamaMsg_addVectorBool (mMsg,
                                     NULL,
                                     1,
                                     mIn,
                                     VECTOR_SIZE);
    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorBool (mMsg,
                                     NULL,
                                     1,
                                     NULL,
                                     &mOutSize);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);

    mStatus = mamaMsg_getVectorBool (mMsg,
                                     NULL,
                                     1,
                                     &mOut,
                                     NULL);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgVectorBoolTestsC, GetVectorBoolNullMsg)
{
    mStatus = mamaMsg_addVectorBool (mMsg,
                                     NULL,
                                     1,
                                     mIn,
                                     VECTOR_SIZE);
    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorBool (NULL,
                                     NULL,
                                     1,
                                     &mOut,
                                     &mOutSize);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

// ****************************************************************************
// Char Test Suite
// ****************************************************************************

class MsgVectorCharTestsC : public MsgVectorTestsC
{
public:

    virtual ~MsgVectorCharTestsC()
    {}

protected:

    char mIn[ VECTOR_SIZE ];
    char mUpdate[ VECTOR_UPDATE_SIZE ];
    const char *mOut;
    mama_size_t mOutSize;

    MsgVectorCharTestsC() 
        : mIn( )
        , mUpdate( )
        , mOut( NULL )
        , mOutSize( 0 )
    {}
 
    virtual void SetUp()
    {
        MsgVectorTestsC::SetUp();

        for( uint32_t ii(0) ; ii < VECTOR_SIZE ; ++ii )
        {
            mIn[ii] = char('A' + ii);
        }

        for( uint32_t ii(0) ; ii < VECTOR_UPDATE_SIZE ; ++ii )
        {
            mUpdate[ii] = char( 'K' + ii );
        }
    }

    virtual void TearDown()
    {
        MsgVectorTestsC::TearDown();
    }
};

// AddVectorChar test fixtures
// ***************************

TEST_F(MsgVectorCharTestsC, AddVectorChar)
{
    mStatus = mamaMsg_addVectorChar (mMsg,
                                     NULL,
                                     1,
                                     mIn,
                                     VECTOR_SIZE);

    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorChar (mMsg,
                                     NULL,
                                     1,
                                     &mOut,
                                     &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( 0, ::memcmp( mIn, mOut, (sizeof(char) * VECTOR_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_SIZE );
}

TEST_F(MsgVectorCharTestsC, AddVectorCharNullAdd)
{
    mStatus = mamaMsg_addVectorChar (mMsg,
                                     NULL,
                                     1,
                                     NULL,
                                     VECTOR_SIZE);

    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgVectorCharTestsC, AddVectorCharNullMsg)
{
    mStatus = mamaMsg_addVectorChar (NULL,
                                     NULL,
                                     1,
                                     mIn,
                                     VECTOR_SIZE);

    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

// UpdateVectorChar test fixtures
// ******************************

TEST_F(MsgVectorCharTestsC, UpdateVectorChar)
{
    mStatus = mamaMsg_addVectorChar (mMsg,
                                     NULL,
                                     1,
                                     mIn,
                                     VECTOR_SIZE);

    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorChar (mMsg,
                                     NULL,
                                     1,
                                     &mOut,
                                     &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( 0, ::memcmp( mIn, mOut, (sizeof(char) * VECTOR_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_SIZE );

    mStatus = mamaMsg_updateVectorChar (mMsg,
                                        NULL,
                                        1,
                                        mUpdate,
                                        VECTOR_UPDATE_SIZE);

    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorChar (mMsg,
                                     NULL,
                                     1,
                                     &mOut,
                                     &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
    EXPECT_EQ( 0, ::memcmp( mUpdate, mOut, (sizeof(char) * VECTOR_UPDATE_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
}

TEST_F(MsgVectorCharTestsC, UpdateVectorCharNullAdd)
{
    mStatus = mamaMsg_updateVectorChar (mMsg,
                                        NULL,
                                        1,
                                        NULL,
                                        VECTOR_UPDATE_SIZE);

    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgVectorCharTestsC, UpdateVectorCharNullMsg)
{
    mStatus = mamaMsg_updateVectorChar (NULL,
                                        NULL,
                                        1,
                                        mUpdate,
                                        VECTOR_UPDATE_SIZE);

    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

// GetVectorChar test fixtures
// ***************************

TEST_F(MsgVectorCharTestsC, GetVectorChar)
{
    mStatus = mamaMsg_addVectorChar (mMsg,
                                     NULL,
                                     1,
                                     mIn,
                                     VECTOR_SIZE);
    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorChar (mMsg,
                                     NULL,
                                     1,
                                     &mOut,
                                     &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( 0, ::memcmp( mIn, mOut, (sizeof(char) * VECTOR_SIZE) ) );
    EXPECT_NE( 0, ::memcmp( mUpdate, mOut, (sizeof(char) * VECTOR_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_SIZE );

    mStatus = mamaMsg_updateVectorChar (mMsg,
                                        NULL,
                                        1,
                                        mUpdate,
                                        VECTOR_UPDATE_SIZE);
    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorChar (mMsg,
                                     NULL,
                                     1,
                                     &mOut,
                                     &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
    EXPECT_EQ( 0, ::memcmp( mUpdate, mOut, (sizeof(char) * VECTOR_UPDATE_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
    EXPECT_NE( mOutSize, VECTOR_SIZE );
}

TEST_F(MsgVectorCharTestsC, GetVectorCharNullAdd)
{
    mStatus = mamaMsg_addVectorChar (mMsg,
                                     NULL,
                                     1,
                                     mIn,
                                     VECTOR_SIZE);
    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorChar (mMsg,
                                     NULL,
                                     1,
                                     NULL,
                                     &mOutSize);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);

    mStatus = mamaMsg_getVectorChar (mMsg,
                                     NULL,
                                     1,
                                     &mOut,
                                     NULL);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgVectorCharTestsC, GetVectorCharNullMsg)
{
    mStatus = mamaMsg_addVectorChar (mMsg,
                                     NULL,
                                     1,
                                     mIn,
                                     VECTOR_SIZE);
    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorChar (NULL,
                                     NULL,
                                     1,
                                     &mOut,
                                     &mOutSize);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

// ****************************************************************************
// I8 Test Suite
// ****************************************************************************

class MsgVectorI8TestsC : public MsgVectorTestsC
{
public:

    virtual ~MsgVectorI8TestsC()
    {}

protected:

    mama_i8_t mIn[ VECTOR_SIZE ];
    mama_i8_t mUpdate[ VECTOR_UPDATE_SIZE ];
    const mama_i8_t *mOut;
    mama_size_t mOutSize;

    MsgVectorI8TestsC() 
        : mIn( )
        , mUpdate( )
        , mOut( NULL )
        , mOutSize( 0 )
    {}
 
    virtual void SetUp()
    {
        MsgVectorTestsC::SetUp();

        for( uint32_t ii(0) ; ii < VECTOR_SIZE ; ++ii )
        {
            mIn[ii] = -5 + (mama_i8_t) ii;
        }

        for( uint32_t ii(0) ; ii < VECTOR_UPDATE_SIZE ; ++ii )
        {
            mUpdate[ii] = -10 + (mama_i8_t) ii;
        }
    }

    virtual void TearDown()
    {
        MsgVectorTestsC::TearDown();
    }
};

// AddVectorI8 test fixtures
// *************************

TEST_F(MsgVectorI8TestsC, AddVectorI8)
{
    mStatus = mamaMsg_addVectorI8 (mMsg,
                                   NULL,
                                   1,
                                   mIn,
                                   VECTOR_SIZE);

    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorI8 (mMsg,
                                   NULL,
                                   1,
                                   &mOut,
                                   &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( 0, ::memcmp( mIn, mOut, (sizeof(char) * VECTOR_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_SIZE );
}

TEST_F(MsgVectorI8TestsC, AddVectorI8NullAdd)
{
    mStatus = mamaMsg_addVectorI8 (mMsg,
                                   NULL,
                                   1,
                                   NULL,
                                   VECTOR_SIZE);

    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgVectorI8TestsC, AddVectorI8NullMsg)
{
    mStatus = mamaMsg_addVectorI8 (NULL,
                                   NULL,
                                   1,
                                   mIn,
                                   VECTOR_SIZE);

    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

// UpdateVectorI8 test fixtures
// ****************************

TEST_F(MsgVectorI8TestsC, UpdateVectorI8)
{
    mStatus = mamaMsg_addVectorI8 (mMsg,
                                   NULL,
                                   1,
                                   mIn,
                                   VECTOR_SIZE);

    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorI8 (mMsg,
                                   NULL,
                                   1,
                                   &mOut,
                                   &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( 0, ::memcmp( mIn, mOut, (sizeof(char) * VECTOR_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_SIZE );

    mStatus = mamaMsg_updateVectorI8 (mMsg,
                                      NULL,
                                      1,
                                      mUpdate,
                                      VECTOR_UPDATE_SIZE);

    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorI8 (mMsg,
                                   NULL,
                                   1,
                                   &mOut,
                                   &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
    EXPECT_EQ( 0, ::memcmp( mUpdate, mOut, (sizeof(char) * VECTOR_UPDATE_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
}

TEST_F(MsgVectorI8TestsC, UpdateVectorI8NullAdd)
{
    mStatus = mamaMsg_updateVectorI8 (mMsg,
                                      NULL,
                                      1,
                                      NULL,
                                      VECTOR_UPDATE_SIZE);

    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgVectorI8TestsC, UpdateVectorI8NullMsg)
{
    mStatus = mamaMsg_updateVectorI8 (NULL,
                                      NULL,
                                      1,
                                      mUpdate,
                                      VECTOR_UPDATE_SIZE);

    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

// GetVectorI8 test fixtures
// *************************

TEST_F(MsgVectorI8TestsC, GetVectorI8)
{
    mStatus = mamaMsg_addVectorI8 (mMsg,
                                   NULL,
                                   1,
                                   mIn,
                                   VECTOR_SIZE);
    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorI8 (mMsg,
                                   NULL,
                                   1,
                                   &mOut,
                                   &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( 0, ::memcmp( mIn, mOut, (sizeof(char) * VECTOR_SIZE) ) );
    EXPECT_NE( 0, ::memcmp( mUpdate, mOut, (sizeof(char) * VECTOR_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_SIZE );

    mStatus = mamaMsg_updateVectorI8 (mMsg,
                                      NULL,
                                      1,
                                      mUpdate,
                                      VECTOR_UPDATE_SIZE);
    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorI8 (mMsg,
                                   NULL,
                                   1,
                                   &mOut,
                                   &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
    EXPECT_EQ( 0, ::memcmp( mUpdate, mOut, (sizeof(char) * VECTOR_UPDATE_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
    EXPECT_NE( mOutSize, VECTOR_SIZE );
}

TEST_F(MsgVectorI8TestsC, GetVectorI8NullAdd)
{
    mStatus = mamaMsg_addVectorI8 (mMsg,
                                   NULL,
                                   1,
                                   mIn,
                                   VECTOR_SIZE);
    EXPECT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorI8 (mMsg,
                                   NULL,
                                   1,
                                   NULL,
                                   &mOutSize);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);

    mStatus = mamaMsg_getVectorI8 (mMsg,
                                   NULL,
                                   1,
                                   &mOut,
                                   NULL);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgVectorI8TestsC, GetVectorI8NullMsg)
{
    mStatus = mamaMsg_addVectorI8 (mMsg,
                                   NULL,
                                   1,
                                   mIn,
                                   VECTOR_SIZE);
    EXPECT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorI8 (NULL,
                                   NULL,
                                   1,
                                   &mOut,
                                   &mOutSize);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

// ****************************************************************************
// U8 Test Suite
// ****************************************************************************

class MsgVectorU8TestsC : public MsgVectorTestsC
{
public:

    virtual ~MsgVectorU8TestsC()
    {}

protected:

    mama_u8_t mIn[ VECTOR_SIZE ];
    mama_u8_t mUpdate[ VECTOR_UPDATE_SIZE ];
    const mama_u8_t *mOut;
    mama_size_t mOutSize;

    MsgVectorU8TestsC() 
        : mIn( )
        , mUpdate( )
        , mOut( NULL )
        , mOutSize( 0 )
    {}
 
    virtual void SetUp()
    {
        MsgVectorTestsC::SetUp();

        for( uint32_t ii(0) ; ii < VECTOR_SIZE ; ++ii )
        {
            mIn[ii] = ii;
        }

        for( uint32_t ii(0) ; ii < VECTOR_UPDATE_SIZE ; ++ii )
        {
            mUpdate[ii] = 10 + (mama_u8_t) ii;
        }
    }

    virtual void TearDown()
    {
        MsgVectorTestsC::TearDown();
    }
};

// AddVectorU8 test fixtures
// *************************

TEST_F(MsgVectorU8TestsC, AddVectorU8)
{
    mStatus = mamaMsg_addVectorU8 (mMsg,
                                   NULL,
                                   1,
                                   mIn,
                                   VECTOR_SIZE);

    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorU8 (mMsg,
                                   NULL,
                                   1,
                                   &mOut,
                                   &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( 0, ::memcmp( mIn, mOut, (sizeof(char) * VECTOR_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_SIZE );
}

TEST_F(MsgVectorU8TestsC, AddVectorU8NullAdd)
{
    mStatus = mamaMsg_addVectorU8 (mMsg,
                                   NULL,
                                   1,
                                   NULL,
                                   VECTOR_SIZE);

    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgVectorU8TestsC, AddVectorU8NullMsg)
{
    mStatus = mamaMsg_addVectorU8 (NULL,
                                   NULL,
                                   1,
                                   mIn,
                                   VECTOR_SIZE);

    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

// UpdateVectorU8 test fixtures
// ****************************

TEST_F(MsgVectorU8TestsC, UpdateVectorU8)
{
    mStatus = mamaMsg_addVectorU8 (mMsg,
                                   NULL,
                                   1,
                                   mIn,
                                   VECTOR_SIZE);

    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorU8 (mMsg,
                                   NULL,
                                   1,
                                   &mOut,
                                   &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( 0, ::memcmp( mIn, mOut, (sizeof(char) * VECTOR_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_SIZE );

    mStatus = mamaMsg_updateVectorU8 (mMsg,
                                      NULL,
                                      1,
                                      mUpdate,
                                      VECTOR_UPDATE_SIZE);

    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorU8 (mMsg,
                                   NULL,
                                   1,
                                   &mOut,
                                   &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
    EXPECT_EQ( 0, ::memcmp( mUpdate, mOut, (sizeof(char) * VECTOR_UPDATE_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
}

TEST_F(MsgVectorU8TestsC, UpdateVectorU8NullAdd)
{
    mStatus = mamaMsg_updateVectorU8 (mMsg,
                                      NULL,
                                      1,
                                      NULL,
                                      VECTOR_UPDATE_SIZE);

    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgVectorU8TestsC, UpdateVectorU8NullMsg)
{
    mStatus = mamaMsg_updateVectorU8 (NULL,
                                      NULL,
                                      1,
                                      mUpdate,
                                      VECTOR_UPDATE_SIZE);

    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

// GetVectorU8 test fixtures
// *************************

TEST_F(MsgVectorU8TestsC, GetVectorU8)
{
    mStatus = mamaMsg_addVectorU8 (mMsg,
                                   NULL,
                                   1,
                                   mIn,
                                   VECTOR_SIZE);
    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorU8 (mMsg,
                                   NULL,
                                   1,
                                   &mOut,
                                   &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( 0, ::memcmp( mIn, mOut, (sizeof(char) * VECTOR_SIZE) ) );
    EXPECT_NE( 0, ::memcmp( mUpdate, mOut, (sizeof(char) * VECTOR_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_SIZE );

    mStatus = mamaMsg_updateVectorU8 (mMsg,
                                      NULL,
                                      1,
                                      mUpdate,
                                      VECTOR_UPDATE_SIZE);
    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorU8 (mMsg,
                                   NULL,
                                   1,
                                   &mOut,
                                   &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
    EXPECT_EQ( 0, ::memcmp( mUpdate, mOut, (sizeof(char) * VECTOR_UPDATE_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
    EXPECT_NE( mOutSize, VECTOR_SIZE );
}

TEST_F(MsgVectorU8TestsC, GetVectorU8NullAdd)
{
    mStatus = mamaMsg_addVectorU8 (mMsg,
                                   NULL,
                                   1,
                                   mIn,
                                   VECTOR_SIZE);
    EXPECT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorU8 (mMsg,
                                   NULL,
                                   1,
                                   NULL,
                                   &mOutSize);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);

    mStatus = mamaMsg_getVectorU8 (mMsg,
                                   NULL,
                                   1,
                                   &mOut,
                                   NULL);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgVectorU8TestsC, GetVectorU8NullMsg)
{
    mStatus = mamaMsg_addVectorU8 (mMsg,
                                   NULL,
                                   1,
                                   mIn,
                                   VECTOR_SIZE);
    EXPECT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorU8 (NULL,
                                   NULL,
                                   1,
                                   &mOut,
                                   &mOutSize);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

// ****************************************************************************
// I16 Test Suite
// ****************************************************************************

class MsgVectorI16TestsC : public MsgVectorTestsC
{
public:

    virtual ~MsgVectorI16TestsC()
    {}

protected:

    mama_i16_t mIn[ VECTOR_SIZE ];
    mama_i16_t mUpdate[ VECTOR_UPDATE_SIZE ];
    const mama_i16_t *mOut;
    mama_size_t mOutSize;

    MsgVectorI16TestsC() 
        : mIn( )
        , mUpdate( )
        , mOut( NULL )
        , mOutSize( 0 )
    {}
 
    virtual void SetUp()
    {
        MsgVectorTestsC::SetUp();

        for( uint32_t ii(0) ; ii < VECTOR_SIZE ; ++ii )
        {
            mIn[ii] = -5 + (mama_i16_t) ii;
        }

        for( uint32_t ii(0) ; ii < VECTOR_UPDATE_SIZE ; ++ii )
        {
            mUpdate[ii] = -10 + (mama_i16_t) ii;
        }
    }

    virtual void TearDown()
    {
        MsgVectorTestsC::TearDown();
    }
};

// AddVectorI16 test fixtures
// **************************

TEST_F(MsgVectorI16TestsC, AddVectorI16)
{
    mStatus = mamaMsg_addVectorI16 (mMsg,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);

    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorI16 (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);
 
    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( 0, ::memcmp( mIn, mOut, (sizeof(char) * VECTOR_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_SIZE );
}

TEST_F(MsgVectorI16TestsC, AddVectorI16NullAdd)
{
    mStatus = mamaMsg_addVectorI16 (mMsg,
                                    NULL,
                                    1,
                                    NULL,
                                    VECTOR_SIZE);

    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgVectorI16TestsC, AddVectorI16NullMsg)
{
    mStatus = mamaMsg_addVectorI16 (NULL,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);

    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

// UpdateVectorI16 test fixtures
// *****************************

TEST_F(MsgVectorI16TestsC, UpdateVectorI16)
{
    mStatus = mamaMsg_addVectorI16 (mMsg,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);

    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorI16 (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( 0, ::memcmp( mIn, mOut, (sizeof(char) * VECTOR_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_SIZE );

    mStatus = mamaMsg_updateVectorI16 (mMsg,
                                       NULL,
                                       1,
                                       mUpdate,
                                       VECTOR_UPDATE_SIZE);

    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorI16 (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
    EXPECT_EQ( 0, ::memcmp( mUpdate, mOut, (sizeof(char) * VECTOR_UPDATE_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
}

TEST_F(MsgVectorI16TestsC, UpdateVectorI16NullAdd)
{
    mStatus = mamaMsg_updateVectorI16 (mMsg,
                                       NULL,
                                       1,
                                       NULL,
                                       VECTOR_UPDATE_SIZE);

    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgVectorI16TestsC, UpdateVectorI16NullMsg)
{
    mStatus = mamaMsg_updateVectorI16 (NULL,
                                       NULL,
                                       1,
                                       mUpdate,
                                       VECTOR_UPDATE_SIZE);

    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

// GetVectorI16 test fixtures
// **************************

TEST_F(MsgVectorI16TestsC, GetVectorI16)
{
    mStatus = mamaMsg_addVectorI16 (mMsg,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);
    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorI16 (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( 0, ::memcmp( mIn, mOut, (sizeof(char) * VECTOR_SIZE) ) );
    EXPECT_NE( 0, ::memcmp( mUpdate, mOut, (sizeof(char) * VECTOR_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_SIZE );

    mStatus = mamaMsg_updateVectorI16 (mMsg,
                                       NULL,
                                       1,
                                       mUpdate,
                                       VECTOR_UPDATE_SIZE);
    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorI16 (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
    EXPECT_EQ( 0, ::memcmp( mUpdate, mOut, (sizeof(char) * VECTOR_UPDATE_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
    EXPECT_NE( mOutSize, VECTOR_SIZE );
}

TEST_F(MsgVectorI16TestsC, GetVectorI16NullAdd)
{
    mStatus = mamaMsg_addVectorI16 (mMsg,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);
    EXPECT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorI16 (mMsg,
                                    NULL,
                                    1,
                                    NULL,
                                    &mOutSize);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);

    mStatus = mamaMsg_getVectorI16 (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    NULL);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgVectorI16TestsC, GetVectorI16NullMsg)
{
    mStatus = mamaMsg_addVectorI16 (mMsg,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);
    EXPECT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorI16 (NULL,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

// ****************************************************************************
// U16 Test Suite
// ****************************************************************************

class MsgVectorU16TestsC : public MsgVectorTestsC
{
public:

    virtual ~MsgVectorU16TestsC()
    {}

protected:

    mama_u16_t mIn[ VECTOR_SIZE ];
    mama_u16_t mUpdate[ VECTOR_UPDATE_SIZE ];
    const mama_u16_t *mOut;
    mama_size_t mOutSize;

    MsgVectorU16TestsC() 
        : mIn( )
        , mUpdate( )
        , mOut( NULL )
        , mOutSize( 0 )
    {}
 
    virtual void SetUp()
    {
        MsgVectorTestsC::SetUp();

        for( uint32_t ii(0) ; ii < VECTOR_SIZE ; ++ii )
        {
            mIn[ii] = 0 + (mama_u16_t) ii;
        }

        for( uint32_t ii(0) ; ii < VECTOR_UPDATE_SIZE ; ++ii )
        {
            mUpdate[ii] = 10 + (mama_u16_t) ii;
        }
    }

    virtual void TearDown()
    {
        MsgVectorTestsC::TearDown();
    }
};

// AddVectorU16 test fixtures
// **************************

TEST_F(MsgVectorU16TestsC, AddVectorU16)
{
    mStatus = mamaMsg_addVectorU16 (mMsg,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);

    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorU16 (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);
 
    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( 0, ::memcmp( mIn, mOut, (sizeof(char) * VECTOR_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_SIZE );
}

TEST_F(MsgVectorU16TestsC, AddVectorU16NullAdd)
{
    mStatus = mamaMsg_addVectorU16 (mMsg,
                                    NULL,
                                    1,
                                    NULL,
                                    VECTOR_SIZE);

    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgVectorU16TestsC, AddVectorU16NullMsg)
{
    mStatus = mamaMsg_addVectorU16 (NULL,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);

    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

// UpdateVectorU16 test fixtures
// *****************************

TEST_F(MsgVectorU16TestsC, UpdateVectorU16)
{
    mStatus = mamaMsg_addVectorU16 (mMsg,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);

    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorU16 (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( 0, ::memcmp( mIn, mOut, (sizeof(char) * VECTOR_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_SIZE );

    mStatus = mamaMsg_updateVectorU16 (mMsg,
                                       NULL,
                                       1,
                                       mUpdate,
                                       VECTOR_UPDATE_SIZE);

    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorU16 (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
    EXPECT_EQ( 0, ::memcmp( mUpdate, mOut, (sizeof(char) * VECTOR_UPDATE_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
}

TEST_F(MsgVectorU16TestsC, UpdateVectorU16NullAdd)
{
    mStatus = mamaMsg_updateVectorU16 (mMsg,
                                       NULL,
                                       1,
                                       NULL,
                                       VECTOR_UPDATE_SIZE);

    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgVectorU16TestsC, UpdateVectorU16NullMsg)
{
    mStatus = mamaMsg_updateVectorU16 (NULL,
                                       NULL,
                                       1,
                                       mUpdate,
                                       VECTOR_UPDATE_SIZE);

    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

// GetVectorU16 test fixtures
// **************************

TEST_F(MsgVectorU16TestsC, GetVectorU16)
{
    mStatus = mamaMsg_addVectorU16 (mMsg,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);
    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorU16 (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( 0, ::memcmp( mIn, mOut, (sizeof(char) * VECTOR_SIZE) ) );
    EXPECT_NE( 0, ::memcmp( mUpdate, mOut, (sizeof(char) * VECTOR_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_SIZE );

    mStatus = mamaMsg_updateVectorU16 (mMsg,
                                       NULL,
                                       1,
                                       mUpdate,
                                       VECTOR_UPDATE_SIZE);
    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorU16 (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
    EXPECT_EQ( 0, ::memcmp( mUpdate, mOut, (sizeof(char) * VECTOR_UPDATE_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
    EXPECT_NE( mOutSize, VECTOR_SIZE );
}

TEST_F(MsgVectorU16TestsC, GetVectorU16NullAdd)
{
    mStatus = mamaMsg_addVectorU16 (mMsg,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);
    EXPECT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorU16 (mMsg,
                                    NULL,
                                    1,
                                    NULL,
                                    &mOutSize);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);

    mStatus = mamaMsg_getVectorU16 (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    NULL);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgVectorU16TestsC, GetVectorU16NullMsg)
{
    mStatus = mamaMsg_addVectorU16 (mMsg,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);
    EXPECT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorU16 (NULL,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

// ****************************************************************************
// I32 Test Suite
// ****************************************************************************

class MsgVectorI32TestsC : public MsgVectorTestsC
{
public:

    virtual ~MsgVectorI32TestsC()
    {}

protected:

    mama_i32_t mIn[ VECTOR_SIZE ];
    mama_i32_t mUpdate[ VECTOR_UPDATE_SIZE ];
    const mama_i32_t *mOut;
    mama_size_t mOutSize;

    MsgVectorI32TestsC() 
        : mIn( )
        , mUpdate( )
        , mOut( NULL )
        , mOutSize( 0 )
    {}
 
    virtual void SetUp()
    {
        MsgVectorTestsC::SetUp();

        for( uint32_t ii(0) ; ii < VECTOR_SIZE ; ++ii )
        {
            mIn[ii] = -5 + (mama_i32_t) ii;
        }

        for( uint32_t ii(0) ; ii < VECTOR_UPDATE_SIZE ; ++ii )
        {
            mUpdate[ii] = -10 + (mama_i32_t) ii;
        }
    }

    virtual void TearDown()
    {
        MsgVectorTestsC::TearDown();
    }
};

// AddVectorI32 test fixtures
// **************************

TEST_F(MsgVectorI32TestsC, AddVectorI32)
{
    mStatus = mamaMsg_addVectorI32 (mMsg,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);

    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorI32 (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);
 
    EXPECT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( 0, ::memcmp( mIn, mOut, (sizeof(char) * VECTOR_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_SIZE );
}

TEST_F(MsgVectorI32TestsC, AddVectorI32NullAdd)
{
    mStatus = mamaMsg_addVectorI32 (mMsg,
                                    NULL,
                                    1,
                                    NULL,
                                    VECTOR_SIZE);

    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgVectorI32TestsC, AddVectorI32NullMsg)
{
    mStatus = mamaMsg_addVectorI32 (NULL,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);

    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

// UpdateVectorI32 test fixtures
// *****************************

TEST_F(MsgVectorI32TestsC, UpdateVectorI32)
{
    mStatus = mamaMsg_addVectorI32 (mMsg,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);

    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorI32 (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( 0, ::memcmp( mIn, mOut, (sizeof(char) * VECTOR_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_SIZE );

    mStatus = mamaMsg_updateVectorI32 (mMsg,
                                       NULL,
                                       1,
                                       mUpdate,
                                       VECTOR_UPDATE_SIZE);

    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorI32 (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
    EXPECT_EQ( 0, ::memcmp( mUpdate, mOut, (sizeof(char) * VECTOR_UPDATE_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
}

TEST_F(MsgVectorI32TestsC, UpdateVectorI32NullAdd)
{
    mStatus = mamaMsg_updateVectorI32 (mMsg,
                                       NULL,
                                       1,
                                       NULL,
                                       VECTOR_UPDATE_SIZE);

    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgVectorI32TestsC, UpdateVectorI32NullMsg)
{
    mStatus = mamaMsg_updateVectorI32 (NULL,
                                       NULL,
                                       1,
                                       mUpdate,
                                       VECTOR_UPDATE_SIZE);

    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

// GetVectorI32 test fixtures
// **************************

TEST_F(MsgVectorI32TestsC, GetVectorI32)
{
    mStatus = mamaMsg_addVectorI32 (mMsg,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);
    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorI32 (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( 0, ::memcmp( mIn, mOut, (sizeof(char) * VECTOR_SIZE) ) );
    EXPECT_NE( 0, ::memcmp( mUpdate, mOut, (sizeof(char) * VECTOR_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_SIZE );

    mStatus = mamaMsg_updateVectorI32 (mMsg,
                                       NULL,
                                       1,
                                       mUpdate,
                                       VECTOR_UPDATE_SIZE);
    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorI32 (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
    EXPECT_EQ( 0, ::memcmp( mUpdate, mOut, (sizeof(char) * VECTOR_UPDATE_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
    EXPECT_NE( mOutSize, VECTOR_SIZE );
}

TEST_F(MsgVectorI32TestsC, GetVectorI32NullAdd)
{
    mStatus = mamaMsg_addVectorI32 (mMsg,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);
    EXPECT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorI32 (mMsg,
                                    NULL,
                                    1,
                                    NULL,
                                    &mOutSize);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);

    mStatus = mamaMsg_getVectorI32 (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    NULL);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgVectorI32TestsC, GetVectorI32NullMsg)
{
    mStatus = mamaMsg_addVectorI32 (mMsg,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);
    EXPECT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorI32 (NULL,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

// ****************************************************************************
// U32 Test Suite
// ****************************************************************************

class MsgVectorU32TestsC : public MsgVectorTestsC
{
public:

    virtual ~MsgVectorU32TestsC()
    {}

protected:

    mama_u32_t mIn[ VECTOR_SIZE ];
    mama_u32_t mUpdate[ VECTOR_UPDATE_SIZE ];
    const mama_u32_t *mOut;
    mama_size_t mOutSize;

    MsgVectorU32TestsC() 
        : mIn( )
        , mUpdate( )
        , mOut( NULL )
        , mOutSize( 0 )
    {}
 
    virtual void SetUp()
    {
        MsgVectorTestsC::SetUp();

        for( uint32_t ii(0) ; ii < VECTOR_SIZE ; ++ii )
        {
            mIn[ii] = 0 + (mama_u32_t) ii;
        }

        for( uint32_t ii(0) ; ii < VECTOR_UPDATE_SIZE ; ++ii )
        {
            mUpdate[ii] = 10 + (mama_u32_t) ii;
        }
    }

    virtual void TearDown()
    {
        MsgVectorTestsC::TearDown();
    }
};

// AddVectorU32 test fixtures
// **************************

TEST_F(MsgVectorU32TestsC, AddVectorU32)
{
    mStatus = mamaMsg_addVectorU32 (mMsg,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);

    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorU32 (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);
 
    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( 0, ::memcmp( mIn, mOut, (sizeof(char) * VECTOR_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_SIZE );
}

TEST_F(MsgVectorU32TestsC, AddVectorU32NullAdd)
{
    mStatus = mamaMsg_addVectorU32 (mMsg,
                                    NULL,
                                    1,
                                    NULL,
                                    VECTOR_SIZE);

    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgVectorU32TestsC, AddVectorU32NullMsg)
{
    mStatus = mamaMsg_addVectorU32 (NULL,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);

    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

// UpdateVectorU32 test fixtures
// *****************************

TEST_F(MsgVectorU32TestsC, UpdateVectorU32)
{
    mStatus = mamaMsg_addVectorU32 (mMsg,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);

    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorU32 (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( 0, ::memcmp( mIn, mOut, (sizeof(char) * VECTOR_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_SIZE );

    mStatus = mamaMsg_updateVectorU32 (mMsg,
                                       NULL,
                                       1,
                                       mUpdate,
                                       VECTOR_UPDATE_SIZE);

    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorU32 (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
    EXPECT_EQ( 0, ::memcmp( mUpdate, mOut, (sizeof(char) * VECTOR_UPDATE_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
}

TEST_F(MsgVectorU32TestsC, UpdateVectorU32NullAdd)
{
    mStatus = mamaMsg_updateVectorU32 (mMsg,
                                       NULL,
                                       1,
                                       NULL,
                                       VECTOR_UPDATE_SIZE);

    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgVectorU32TestsC, UpdateVectorU32NullMsg)
{
    mStatus = mamaMsg_updateVectorU32 (NULL,
                                       NULL,
                                       1,
                                       mUpdate,
                                       VECTOR_UPDATE_SIZE);

    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

// GetVectorU32 test fixtures
// **************************

TEST_F(MsgVectorU32TestsC, GetVectorU32)
{
    mStatus = mamaMsg_addVectorU32 (mMsg,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);
    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorU32 (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( 0, ::memcmp( mIn, mOut, (sizeof(char) * VECTOR_SIZE) ) );
    EXPECT_NE( 0, ::memcmp( mUpdate, mOut, (sizeof(char) * VECTOR_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_SIZE );

    mStatus = mamaMsg_updateVectorU32 (mMsg,
                                       NULL,
                                       1,
                                       mUpdate,
                                       VECTOR_UPDATE_SIZE);
    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorU32 (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
    EXPECT_EQ( 0, ::memcmp( mUpdate, mOut, (sizeof(char) * VECTOR_UPDATE_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
    EXPECT_NE( mOutSize, VECTOR_SIZE );
}

TEST_F(MsgVectorU32TestsC, GetVectorU32NullAdd)
{
    mStatus = mamaMsg_addVectorU32 (mMsg,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);
    EXPECT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorU32 (mMsg,
                                    NULL,
                                    1,
                                    NULL,
                                    &mOutSize);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);

    mStatus = mamaMsg_getVectorU32 (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    NULL);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgVectorU32TestsC, GetVectorU32NullMsg)
{
    mStatus = mamaMsg_addVectorU32 (mMsg,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);
    EXPECT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorU32 (NULL,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

// ****************************************************************************
// I64 Test Suite
// ****************************************************************************

class MsgVectorI64TestsC : public MsgVectorTestsC
{
public:

    virtual ~MsgVectorI64TestsC()
    {}

protected:

    mama_i64_t mIn[ VECTOR_SIZE ];
    mama_i64_t mUpdate[ VECTOR_UPDATE_SIZE ];
    const mama_i64_t *mOut;
    mama_size_t mOutSize;

    MsgVectorI64TestsC() 
        : mIn( )
        , mUpdate( )
        , mOut( NULL )
        , mOutSize( 0 )
    {}
 
    virtual void SetUp()
    {
        MsgVectorTestsC::SetUp();

        for( uint32_t ii(0) ; ii < VECTOR_SIZE ; ++ii )
        {
            mIn[ii] = -5 + (mama_i64_t) ii;
        }

        for( uint32_t ii(0) ; ii < VECTOR_UPDATE_SIZE ; ++ii )
        {
            mUpdate[ii] = -10 + (mama_i64_t) ii;
        }
    }

    virtual void TearDown()
    {
        MsgVectorTestsC::TearDown();
    }
};

// AddVectorI64 test fixtures
// **************************

TEST_F(MsgVectorI64TestsC, AddVectorI64)
{
    mStatus = mamaMsg_addVectorI64 (mMsg,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);

    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorI64 (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);
 
    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( 0, ::memcmp( mIn, mOut, (sizeof(char) * VECTOR_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_SIZE );
}

TEST_F(MsgVectorI64TestsC, AddVectorI64NullAdd)
{
    mStatus = mamaMsg_addVectorI64 (mMsg,
                                    NULL,
                                    1,
                                    NULL,
                                    VECTOR_SIZE);

    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgVectorI64TestsC, AddVectorI64NullMsg)
{
    mStatus = mamaMsg_addVectorI64 (NULL,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);

    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

// UpdateVectorI64 test fixtures
// *****************************

TEST_F(MsgVectorI64TestsC, UpdateVectorI64)
{
    mStatus = mamaMsg_addVectorI64 (mMsg,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);

    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorI64 (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( 0, ::memcmp( mIn, mOut, (sizeof(char) * VECTOR_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_SIZE );

    mStatus = mamaMsg_updateVectorI64 (mMsg,
                                       NULL,
                                       1,
                                       mUpdate,
                                       VECTOR_UPDATE_SIZE);

    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorI64 (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
    EXPECT_EQ( 0, ::memcmp( mUpdate, mOut, (sizeof(char) * VECTOR_UPDATE_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
}

TEST_F(MsgVectorI64TestsC, UpdateVectorI64NullAdd)
{
    mStatus = mamaMsg_updateVectorI64 (mMsg,
                                       NULL,
                                       1,
                                       NULL,
                                       VECTOR_UPDATE_SIZE);

    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgVectorI64TestsC, UpdateVectorI64NullMsg)
{
    mStatus = mamaMsg_updateVectorI64 (NULL,
                                       NULL,
                                       1,
                                       mUpdate,
                                       VECTOR_UPDATE_SIZE);

    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

// GetVectorI64 test fixtures
// **************************

TEST_F(MsgVectorI64TestsC, GetVectorI64)
{
    mStatus = mamaMsg_addVectorI64 (mMsg,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);
    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorI64 (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( 0, ::memcmp( mIn, mOut, (sizeof(char) * VECTOR_SIZE) ) );
    EXPECT_NE( 0, ::memcmp( mUpdate, mOut, (sizeof(char) * VECTOR_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_SIZE );

    mStatus = mamaMsg_updateVectorI64 (mMsg,
                                       NULL,
                                       1,
                                       mUpdate,
                                       VECTOR_UPDATE_SIZE);
    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorI64 (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
    EXPECT_EQ( 0, ::memcmp( mUpdate, mOut, (sizeof(char) * VECTOR_UPDATE_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
    EXPECT_NE( mOutSize, VECTOR_SIZE );
}

TEST_F(MsgVectorI64TestsC, GetVectorI64NullAdd)
{
    mStatus = mamaMsg_addVectorI64 (mMsg,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);
    EXPECT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorI64 (mMsg,
                                    NULL,
                                    1,
                                    NULL,
                                    &mOutSize);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);

    mStatus = mamaMsg_getVectorI64 (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    NULL);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgVectorI64TestsC, GetVectorI64NullMsg)
{
    mStatus = mamaMsg_addVectorI64 (mMsg,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);
    EXPECT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorI64 (NULL,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

// ****************************************************************************
// U64 Test Suite
// ****************************************************************************

class MsgVectorU64TestsC : public MsgVectorTestsC
{
public:

    virtual ~MsgVectorU64TestsC()
    {}

protected:

    mama_u64_t mIn[ VECTOR_SIZE ];
    mama_u64_t mUpdate[ VECTOR_UPDATE_SIZE ];
    const mama_u64_t *mOut;
    mama_size_t mOutSize;

    MsgVectorU64TestsC() 
        : mIn( )
        , mUpdate( )
        , mOut( NULL )
        , mOutSize( 0 )
    {}
 
    virtual void SetUp()
    {
        MsgVectorTestsC::SetUp();

        for( uint32_t ii(0) ; ii < VECTOR_SIZE ; ++ii )
        {
            mIn[ii] = 0 + (mama_u64_t) ii;
        }

        for( uint32_t ii(0) ; ii < VECTOR_UPDATE_SIZE ; ++ii )
        {
            mUpdate[ii] = 10 + (mama_u64_t) ii;
        }
    }

    virtual void TearDown()
    {
        MsgVectorTestsC::TearDown();
    }
};

// AddVectorU64 test fixtures
// **************************

TEST_F(MsgVectorU64TestsC, AddVectorU64)
{
    mStatus = mamaMsg_addVectorU64 (mMsg,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);

    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorU64 (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);
 
    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( 0, ::memcmp( mIn, mOut, (sizeof(char) * VECTOR_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_SIZE );
}

TEST_F(MsgVectorU64TestsC, AddVectorU64NullAdd)
{
    mStatus = mamaMsg_addVectorU64 (mMsg,
                                    NULL,
                                    1,
                                    NULL,
                                    VECTOR_SIZE);

    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgVectorU64TestsC, AddVectorU64NullMsg)
{
    mStatus = mamaMsg_addVectorU64 (NULL,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);

    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

// UpdateVectorU64 test fixtures
// *****************************

TEST_F(MsgVectorU64TestsC, UpdateVectorU64)
{
    mStatus = mamaMsg_addVectorU64 (mMsg,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);

    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorU64 (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( 0, ::memcmp( mIn, mOut, (sizeof(char) * VECTOR_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_SIZE );

    mStatus = mamaMsg_updateVectorU64 (mMsg,
                                       NULL,
                                       1,
                                       mUpdate,
                                       VECTOR_UPDATE_SIZE);

    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorU64 (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
    EXPECT_EQ( 0, ::memcmp( mUpdate, mOut, (sizeof(char) * VECTOR_UPDATE_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
}

TEST_F(MsgVectorU64TestsC, UpdateVectorU64NullAdd)
{
    mStatus = mamaMsg_updateVectorU64 (mMsg,
                                       NULL,
                                       1,
                                       NULL,
                                       VECTOR_UPDATE_SIZE);

    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgVectorU64TestsC, UpdateVectorU64NullMsg)
{
    mStatus = mamaMsg_updateVectorU64 (NULL,
                                       NULL,
                                       1,
                                       mUpdate,
                                       VECTOR_UPDATE_SIZE);

    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

// GetVectorU64 test fixtures
// **************************

TEST_F(MsgVectorU64TestsC, GetVectorU64)
{
    mStatus = mamaMsg_addVectorU64 (mMsg,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);
    EXPECT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorU64 (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( 0, ::memcmp( mIn, mOut, (sizeof(char) * VECTOR_SIZE) ) );
    EXPECT_NE( 0, ::memcmp( mUpdate, mOut, (sizeof(char) * VECTOR_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_SIZE );

    mStatus = mamaMsg_updateVectorU64 (mMsg,
                                       NULL,
                                       1,
                                       mUpdate,
                                       VECTOR_UPDATE_SIZE);
    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorU64 (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
    EXPECT_EQ( 0, ::memcmp( mUpdate, mOut, (sizeof(char) * VECTOR_UPDATE_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
    EXPECT_NE( mOutSize, VECTOR_SIZE );
}

TEST_F(MsgVectorU64TestsC, GetVectorU64NullAdd)
{
    mStatus = mamaMsg_addVectorU64 (mMsg,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);
    EXPECT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorU64 (mMsg,
                                    NULL,
                                    1,
                                    NULL,
                                    &mOutSize);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);

    mStatus = mamaMsg_getVectorU64 (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    NULL);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgVectorU64TestsC, GetVectorU64NullMsg)
{
    mStatus = mamaMsg_addVectorU64 (mMsg,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);
    EXPECT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorU64 (NULL,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

// ****************************************************************************
// F32 Test Suite
// ****************************************************************************

class MsgVectorF32TestsC : public MsgVectorTestsC
{
public:

    virtual ~MsgVectorF32TestsC()
    {}

protected:

    mama_f32_t mIn[ VECTOR_SIZE ];
    mama_f32_t mUpdate[ VECTOR_UPDATE_SIZE ];
    mama_f32_t mPi;
    mama_f32_t mE;
    const mama_f32_t *mOut;
    mama_size_t mOutSize;

    MsgVectorF32TestsC() 
        : mIn( )
        , mUpdate( )
        , mPi( 3.14159265 )
        , mE( 2.71828183 )
        , mOut( NULL )
        , mOutSize( 0 )
    {}
 
    virtual void SetUp()
    {
        MsgVectorTestsC::SetUp();

        for( uint32_t ii(0) ; ii < VECTOR_SIZE ; ++ii )
        {
            mIn[ii] =  mPi + (mama_f32_t) ii;
        }

        for( uint32_t ii(0) ; ii < VECTOR_UPDATE_SIZE ; ++ii )
        {
            mUpdate[ii] = mE + (mama_f32_t) ii;
        }
    }

    virtual void TearDown()
    {
        MsgVectorTestsC::TearDown();
    }
};

// AddVectorF32 test fixtures
// **************************

TEST_F(MsgVectorF32TestsC, AddVectorF32)
{
    mStatus = mamaMsg_addVectorF32 (mMsg,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);

    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorF32 (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);
 
    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( 0, ::memcmp( mIn, mOut, (sizeof(char) * VECTOR_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_SIZE );
}

TEST_F(MsgVectorF32TestsC, AddVectorF32NullAdd)
{
    mStatus = mamaMsg_addVectorF32 (mMsg,
                                    NULL,
                                    1,
                                    NULL,
                                    VECTOR_SIZE);

    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgVectorF32TestsC, AddVectorF32NullMsg)
{
    mStatus = mamaMsg_addVectorF32 (NULL,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);

    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

// UpdateVectorF32 test fixtures
// *****************************

TEST_F(MsgVectorF32TestsC, UpdateVectorF32)
{
    mStatus = mamaMsg_addVectorF32 (mMsg,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);

    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorF32 (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( 0, ::memcmp( mIn, mOut, (sizeof(char) * VECTOR_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_SIZE );

    mStatus = mamaMsg_updateVectorF32 (mMsg,
                                       NULL,
                                       1,
                                       mUpdate,
                                       VECTOR_UPDATE_SIZE);

    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorF32 (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
    EXPECT_EQ( 0, ::memcmp( mUpdate, mOut, (sizeof(char) * VECTOR_UPDATE_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
}

TEST_F(MsgVectorF32TestsC, UpdateVectorF32NullAdd)
{
    mStatus = mamaMsg_updateVectorF32 (mMsg,
                                       NULL,
                                       1,
                                       NULL,
                                       VECTOR_UPDATE_SIZE);

    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgVectorF32TestsC, UpdateVectorF32NullMsg)
{
    mStatus = mamaMsg_updateVectorF32 (NULL,
                                       NULL,
                                       1,
                                       mUpdate,
                                       VECTOR_UPDATE_SIZE);

    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

// GetVectorF32 test fixtures
// **************************

TEST_F(MsgVectorF32TestsC, GetVectorF32)
{
    mStatus = mamaMsg_addVectorF32 (mMsg,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);
    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorF32 (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( 0, ::memcmp( mIn, mOut, (sizeof(char) * VECTOR_SIZE) ) );
    EXPECT_NE( 0, ::memcmp( mUpdate, mOut, (sizeof(char) * VECTOR_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_SIZE );

    mStatus = mamaMsg_updateVectorF32 (mMsg,
                                       NULL,
                                       1,
                                       mUpdate,
                                       VECTOR_UPDATE_SIZE);
    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorF32 (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
    EXPECT_EQ( 0, ::memcmp( mUpdate, mOut, (sizeof(char) * VECTOR_UPDATE_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
    EXPECT_NE( mOutSize, VECTOR_SIZE );
}

TEST_F(MsgVectorF32TestsC, GetVectorF32NullAdd)
{
    mStatus = mamaMsg_addVectorF32 (mMsg,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);
    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorF32 (mMsg,
                                    NULL,
                                    1,
                                    NULL,
                                    &mOutSize);
    ASSERT_EQ(mStatus, MAMA_STATUS_NULL_ARG);

    mStatus = mamaMsg_getVectorF32 (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    NULL);
    ASSERT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgVectorF32TestsC, GetVectorF32NullMsg)
{
    mStatus = mamaMsg_addVectorF32 (mMsg,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);
    EXPECT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorF32 (NULL,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

// ****************************************************************************
// F64 Test Suite
// ****************************************************************************

class MsgVectorF64TestsC : public MsgVectorTestsC
{
public:

    virtual ~MsgVectorF64TestsC()
    {}

protected:

    mama_f64_t mIn[ VECTOR_SIZE ];
    mama_f64_t mUpdate[ VECTOR_UPDATE_SIZE ];
    mama_f64_t mPi;
    mama_f64_t mE;
    const mama_f64_t *mOut;
    mama_size_t mOutSize;

    MsgVectorF64TestsC() 
        : mIn( )
        , mUpdate( )
        , mPi( 3.14159265 )
        , mE( 2.71828183 )
        , mOut( NULL )
        , mOutSize( 0 )
    {}
 
    virtual void SetUp()
    {
        MsgVectorTestsC::SetUp();

        for( uint32_t ii(0) ; ii < VECTOR_SIZE ; ++ii )
        {
            mIn[ii] =  mPi + (mama_f64_t) ii;
        }

        for( uint32_t ii(0) ; ii < VECTOR_UPDATE_SIZE ; ++ii )
        {
            mUpdate[ii] = mE + (mama_f64_t) ii;
        }
    }

    virtual void TearDown()
    {
        MsgVectorTestsC::TearDown();
    }
};

// AddVectorF64 test fixtures
// **************************

TEST_F(MsgVectorF64TestsC, AddVectorF64)
{
    mStatus = mamaMsg_addVectorF64 (mMsg,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);

    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorF64 (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);
 
    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( 0, ::memcmp( mIn, mOut, (sizeof(char) * VECTOR_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_SIZE );
}

TEST_F(MsgVectorF64TestsC, AddVectorF64NullAdd)
{
    mStatus = mamaMsg_addVectorF64 (mMsg,
                                    NULL,
                                    1,
                                    NULL,
                                    VECTOR_SIZE);

    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgVectorF64TestsC, AddVectorF64NullMsg)
{
    mStatus = mamaMsg_addVectorF64 (NULL,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);

    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

// UpdateVectorF64 test fixtures
// *****************************

TEST_F(MsgVectorF64TestsC, UpdateVectorF64)
{
    mStatus = mamaMsg_addVectorF64 (mMsg,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);

    EXPECT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorF64 (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( 0, ::memcmp( mIn, mOut, (sizeof(char) * VECTOR_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_SIZE );

    mStatus = mamaMsg_updateVectorF64 (mMsg,
                                       NULL,
                                       1,
                                       mUpdate,
                                       VECTOR_UPDATE_SIZE);

    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorF64 (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
    EXPECT_EQ( 0, ::memcmp( mUpdate, mOut, (sizeof(char) * VECTOR_UPDATE_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
}

TEST_F(MsgVectorF64TestsC, UpdateVectorF64NullAdd)
{
    mStatus = mamaMsg_updateVectorF64 (mMsg,
                                       NULL,
                                       1,
                                       NULL,
                                       VECTOR_UPDATE_SIZE);

    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgVectorF64TestsC, UpdateVectorF64NullMsg)
{
    mStatus = mamaMsg_updateVectorF64 (NULL,
                                       NULL,
                                       1,
                                       mUpdate,
                                       VECTOR_UPDATE_SIZE);

    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

// GetVectorF64 test fixtures
// **************************

TEST_F(MsgVectorF64TestsC, GetVectorF64)
{
    mStatus = mamaMsg_addVectorF64 (mMsg,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);
    EXPECT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorF64 (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( 0, ::memcmp( mIn, mOut, (sizeof(char) * VECTOR_SIZE) ) );
    EXPECT_NE( 0, ::memcmp( mUpdate, mOut, (sizeof(char) * VECTOR_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_SIZE );

    mStatus = mamaMsg_updateVectorF64 (mMsg,
                                       NULL,
                                       1,
                                       mUpdate,
                                       VECTOR_UPDATE_SIZE);
    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorF64 (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
    EXPECT_EQ( 0, ::memcmp( mUpdate, mOut, (sizeof(char) * VECTOR_UPDATE_SIZE) ) );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
    EXPECT_NE( mOutSize, VECTOR_SIZE );
}

TEST_F(MsgVectorF64TestsC, GetVectorF64NullAdd)
{
    mStatus = mamaMsg_addVectorF64 (mMsg,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);
    EXPECT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorF64 (mMsg,
                                    NULL,
                                    1,
                                    NULL,
                                    &mOutSize);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);

    mStatus = mamaMsg_getVectorF64 (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    NULL);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgVectorF64TestsC, GetVectorF64NullMsg)
{
    mStatus = mamaMsg_addVectorF64 (mMsg,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);
    EXPECT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorF64 (NULL,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

// ****************************************************************************
// String Test Suite
// ****************************************************************************

class MsgVectorStringTestsC : public MsgVectorTestsC
{
public:

    virtual ~MsgVectorStringTestsC()
    {}

protected:

    const char * mIn[ VECTOR_SIZE ];
    const char * mUpdate[ VECTOR_UPDATE_SIZE ];
    const char ** mOut;
    mama_size_t mOutSize;

    MsgVectorStringTestsC() 
        : mIn( )
        , mUpdate( )
        , mOut( NULL )
        , mOutSize( 0 )
    {}
 
    virtual void SetUp()
    {
        MsgVectorTestsC::SetUp();

        for( uint32_t ii(0) ; ii < VECTOR_SIZE ; ++ii )
        {
            char buffer[20];
            ::snprintf(buffer, 20, "InitialString%u", ii);
            mIn[ii] = ::strndup( buffer, 20 );
        }

        for( uint32_t ii(0) ; ii < VECTOR_UPDATE_SIZE ; ++ii )
        {
            char buffer[20];
            ::snprintf(buffer, 20, "UpdateString%u", ii);
            mUpdate[ii] = ::strndup( buffer, 20 );
        }
    }

    virtual void TearDown()
    {
        for( uint32_t ii(0) ; ii < VECTOR_SIZE ; ++ii )
        {
            free( (void *) mIn[ii] ); mIn[ii] = NULL;
        }

        for( uint32_t ii(0) ; ii < VECTOR_UPDATE_SIZE ; ++ii )
        {
            free( (void *) mUpdate[ii] ); mUpdate[ii] = NULL;
        }

        MsgVectorTestsC::TearDown();
    }
};

// AddVectorString test fixtures
// *****************************

TEST_F(MsgVectorStringTestsC, AddVectorString)
{
    mStatus = mamaMsg_addVectorString (mMsg,
                                       NULL,
                                       1,
                                       mIn,
                                       VECTOR_SIZE);
    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorString (mMsg,
                                       NULL,
                                       1,
                                       &mOut,
                                       &mOutSize);
    ASSERT_EQ( mStatus, MAMA_STATUS_OK );

    int getResult(0);
    for( unsigned int ii( 0 ) ; ((ii < VECTOR_SIZE) && (0 == getResult)) ; ++ii )
    {
        getResult = ::strncmp( mIn[ii], mOut[ii], 20 );
    }
    EXPECT_EQ( 0, getResult );
    EXPECT_EQ( mOutSize, VECTOR_SIZE );
}

TEST_F(MsgVectorStringTestsC, AddVectorStringNullAdd)
{
    mStatus = mamaMsg_addVectorString (mMsg,
                                       NULL,
                                       1,
                                       NULL,
                                       VECTOR_SIZE);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgVectorStringTestsC, AddVectorStringNullMsg)
{
    mStatus = mamaMsg_addVectorString (NULL,
                                       NULL,
                                       1,
                                       mIn,
                                       VECTOR_SIZE);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

// UpdateVectorString test fixtures
// ********************************

TEST_F(MsgVectorStringTestsC, UpdateVectorString)
{
    mStatus = mamaMsg_addVectorString (mMsg,
                                       NULL,
                                       1,
                                       mIn,
                                       VECTOR_SIZE);
    EXPECT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorString (mMsg,
                                       NULL,
                                       1,
                                       &mOut,
                                       &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    int getResult(0);
    for( unsigned int ii( 0 ) ; ((ii < VECTOR_SIZE) && (0 == getResult)) ; ++ii )
    {
        getResult = ::strncmp( mIn[ii], mOut[ii], 20 );
    }
    ASSERT_EQ( mOutSize, VECTOR_SIZE );

    mStatus = mamaMsg_updateVectorString (mMsg,
                                          NULL,
                                          1,
                                          mUpdate,
                                          VECTOR_UPDATE_SIZE);
    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorString (mMsg,
                                       NULL,
                                       1,
                                       &mOut,
                                       &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    getResult = 0;
    for( unsigned int ii( 0 ) ; ((ii < VECTOR_UPDATE_SIZE) && (0 == getResult)) ; ++ii )
    {
        getResult = ::strncmp( mUpdate[ii], mOut[ii], 20 );
    }
    EXPECT_EQ( getResult, 0 );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
}

TEST_F(MsgVectorStringTestsC, UpdateVectorStringNullAdd)
{
    mStatus = mamaMsg_updateVectorString (mMsg,
                                          NULL,
                                          1,
                                          NULL,
                                          VECTOR_UPDATE_SIZE);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgVectorStringTestsC, UpdateVectorStringNullMsg)
{
    mStatus = mamaMsg_updateVectorString (NULL,
                                          NULL,
                                          1,
                                          mUpdate,
                                          VECTOR_UPDATE_SIZE);

    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

// GetVectorString test fixtures
// *****************************

TEST_F(MsgVectorStringTestsC, GetVectorString)
{
    mStatus = mamaMsg_addVectorString (mMsg,
                                       NULL,
                                       1,
                                       mIn,
                                       VECTOR_SIZE);
    EXPECT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorString (mMsg,
                                       NULL,
                                       1,
                                       &mOut,
                                       &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    int getResult(0);
    for( unsigned int ii( 0 ) ; ((ii < VECTOR_SIZE) && (0 == getResult)) ; ++ii )
    {
        getResult = ::strncmp( mIn[ii], mOut[ii], 20 );
    }
    EXPECT_EQ( getResult, 0 );
    EXPECT_EQ( mOutSize, VECTOR_SIZE );

    mStatus = mamaMsg_updateVectorString (mMsg,
                                          NULL,
                                          1,
                                          mUpdate,
                                          VECTOR_UPDATE_SIZE);
    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorString (mMsg,
                                       NULL,
                                       1,
                                       &mOut,
                                       &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
    getResult = 0;
    for( unsigned int ii( 0 ) ; ((ii < VECTOR_SIZE) && (0 == getResult)) ; ++ii )
    {
        getResult = ::strncmp( mUpdate[ii], mOut[ii], 20 );
    }
    EXPECT_EQ( getResult, 0 );
}

TEST_F(MsgVectorStringTestsC, GetVectorStringNullAdd)
{
    mStatus = mamaMsg_addVectorString (mMsg,
                                       NULL,
                                       1,
                                       mIn,
                                       VECTOR_SIZE);
    EXPECT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorString (mMsg,
                                       NULL,
                                       1,
                                       NULL,
                                       &mOutSize);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);

    mStatus = mamaMsg_getVectorString (mMsg,
                                       NULL,
                                       1,
                                       &mOut,
                                       NULL);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgVectorStringTestsC, GetVectorStringNullMsg)
{
    mStatus = mamaMsg_addVectorString (mMsg,
                                       NULL,
                                       1,
                                       mIn,
                                       VECTOR_SIZE);
    EXPECT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorString (NULL,
                                       NULL,
                                       1,
                                       &mOut,
                                       &mOutSize);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

// ****************************************************************************
// Msg Test Suite
// ****************************************************************************

class MsgVectorMsgTestsC : public MsgVectorTestsC
{
public:

    virtual ~MsgVectorMsgTestsC()
    {}

protected:

    mamaMsg mIn[ VECTOR_SIZE ];
    mamaMsg mUpdate[ VECTOR_UPDATE_SIZE ];
    const mamaMsg *mOut;
    mama_size_t mOutSize;

    MsgVectorMsgTestsC() 
        : mIn( )
        , mUpdate( )
        , mOut( NULL )
        , mOutSize( 0 )
    {}
 
    virtual void SetUp()
    {
        MsgVectorTestsC::SetUp();

        for( uint32_t ii(0) ; ii < VECTOR_SIZE ; ++ii )
        {
            char symbol[ MAX_SUBJECT ];
            ::snprintf( symbol, MAX_SUBJECT, "InSymbol%d", ii);

            mamaMsg_create( &mIn[ii] );
            mamaMsgImpl_setSubscInfo ( mIn[ii],
                                       "_MD",
                                       "InSource",
                                       symbol,
                                       0 );
            mamaMsgImpl_setStatus ( mIn[ii],
                                    MAMA_MSG_STATUS_OK );
        }

        for( uint32_t ii(0) ; ii < VECTOR_UPDATE_SIZE ; ++ii )
        {
            char symbol[ MAX_SUBJECT ];
            ::snprintf( symbol, MAX_SUBJECT, "UpdateSymbol%d", ii);

            mamaMsg_create( &mUpdate[ii] );
            mamaMsgImpl_setSubscInfo ( mUpdate[ii],
                                       "_MD",
                                       "UpdateSource",
                                       symbol,
                                       1 );
            mamaMsgImpl_setStatus ( mUpdate[ii],
                                    MAMA_MSG_STATUS_LINE_DOWN );
        }
    }

    virtual void TearDown()
    {
        for( uint32_t ii(0) ; ii < VECTOR_SIZE ; ++ii )
        {
            mamaMsg_destroy( mIn[ii] ); mIn[ii] = NULL;
        }

        for( uint32_t ii(0) ; ii < VECTOR_UPDATE_SIZE ; ++ii )
        {
            mamaMsg_destroy( mUpdate[ii] ); mUpdate[ii] = NULL;
        }

        MsgVectorTestsC::TearDown();
    }
};

// AddVectorMsg test fixtures
// **************************

TEST_F(MsgVectorMsgTestsC, AddVectorMsg)
{
    mStatus = mamaMsg_addVectorMsg (mMsg,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);
    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorMsg (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);
    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( mOutSize, VECTOR_SIZE );
}

TEST_F(MsgVectorMsgTestsC, AddVectorMsgNullAdd)
{
    mStatus = mamaMsg_addVectorMsg (mMsg,
                                    NULL,
                                    1,
                                    NULL,
                                    VECTOR_SIZE);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgVectorMsgTestsC, AddVectorMsgNullMsg)
{
    mStatus = mamaMsg_addVectorMsg (NULL,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

// UpdateVectorMsg test fixtures
// *****************************

TEST_F(MsgVectorMsgTestsC, UpdateVectorMsg)
{
    mStatus = mamaMsg_addVectorMsg (mMsg,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);
    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorMsg (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);
    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( mOutSize, VECTOR_SIZE );

    mStatus = mamaMsg_updateVectorMsg (mMsg,
                                       NULL,
                                       1,
                                       mUpdate,
                                       VECTOR_UPDATE_SIZE);
    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorMsg (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);
    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
}

TEST_F(MsgVectorMsgTestsC, UpdateVectorMsgNullAdd)
{
    mStatus = mamaMsg_updateVectorMsg (mMsg,
                                       NULL,
                                       1,
                                       NULL,
                                       VECTOR_UPDATE_SIZE);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgVectorMsgTestsC, UpdateVectorMsgNullMsg)
{
    mStatus = mamaMsg_updateVectorMsg (NULL,
                                       NULL,
                                       1,
                                       mUpdate,
                                       VECTOR_UPDATE_SIZE);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

// GetVectorMsg test fixtures
// **************************

TEST_F(MsgVectorMsgTestsC, GetVectorMsg)
{
    mStatus = mamaMsg_addVectorMsg (mMsg,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);
    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorMsg (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);
    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( mOutSize, VECTOR_SIZE );

    mStatus = mamaMsg_updateVectorMsg (mMsg,
                                       NULL,
                                       1,
                                       mUpdate,
                                       VECTOR_UPDATE_SIZE);
    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorMsg (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);

    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
}

TEST_F(MsgVectorMsgTestsC, GetVectorMsgNullAdd)
{
    mStatus = mamaMsg_addVectorMsg (mMsg,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);
    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorMsg (mMsg,
                                    NULL,
                                    1,
                                    NULL,
                                    &mOutSize);
    ASSERT_EQ(mStatus, MAMA_STATUS_NULL_ARG);

    mStatus = mamaMsg_getVectorMsg (mMsg,
                                    NULL,
                                    1,
                                    &mOut,
                                    NULL);
    ASSERT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgVectorMsgTestsC, GetVectorMsgNullMsg)
{
    mStatus = mamaMsg_addVectorMsg (mMsg,
                                    NULL,
                                    1,
                                    mIn,
                                    VECTOR_SIZE);
    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorMsg (NULL,
                                    NULL,
                                    1,
                                    &mOut,
                                    &mOutSize);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

// ****************************************************************************
// DateTime Test Suite
// ****************************************************************************

class MsgVectorDateTimeTestsC : public MsgVectorTestsC
{
public:

    virtual ~MsgVectorDateTimeTestsC()
    {}

protected:

    mamaDateTime mIn[ VECTOR_SIZE ];
    mamaDateTime mUpdate[ VECTOR_UPDATE_SIZE ];
    const mamaDateTime *mOut;
    mama_size_t mOutSize;

    MsgVectorDateTimeTestsC() 
        : mIn( )
        , mUpdate( )
        , mOut( NULL )
        , mOutSize( 0 )
    {}
 
    virtual void SetUp()
    {
        MsgVectorTestsC::SetUp();

        for( uint32_t ii(0) ; ii < VECTOR_SIZE ; ++ii )
        {
            mamaDateTime_create( &mIn[ ii ] );
            mamaDateTime_setToNow( mIn[ ii ] );

            mamaDateTime_addSeconds( mIn[ ii ],
                                     (ii * 10) );
        }

        for( uint32_t ii(0) ; ii < VECTOR_UPDATE_SIZE ; ++ii )
        {
            mamaDateTime_create( &mUpdate[ ii ] );
            mamaDateTime_setToNow( mUpdate[ ii ] );

            mamaDateTime_addSeconds( mUpdate[ ii ],
                                     ((ii + 1) * 24 * 60 * 60) );
        }
    }

    virtual void TearDown()
    {
        for( uint32_t ii(0) ; ii < VECTOR_SIZE ; ++ii )
        {
            mamaDateTime_destroy( mIn[ii] );
        }

        for( uint32_t ii(0) ; ii < VECTOR_UPDATE_SIZE ; ++ii )
        {
            mamaDateTime_destroy( mUpdate[ii] );
        }

        MsgVectorTestsC::TearDown();
    }
};

// AddVectorDateTime test fixtures
// *******************************

TEST_F(MsgVectorDateTimeTestsC, DISABLED_AddVectorDateTime)
// Disabled as mamaMsg_addVectorDateTime is not implemented.
{
    mStatus = mamaMsg_addVectorDateTime (mMsg,
                                         NULL,
                                         1,
                                         mIn,
                                         VECTOR_SIZE);
    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorDateTime (mMsg,
                                         NULL,
                                         1,
                                         (mama_u64_t* const*) &mOut,
                                         &mOutSize);
    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( mOutSize, VECTOR_SIZE );

    if (MAMA_STATUS_OK == mStatus)
    {
        for (unsigned int ii(0) ; ii < VECTOR_SIZE ; ++ii)
        {
            int eq = mamaDateTime_equal( mIn[ii],
                                         mOut[ii] );
            EXPECT_EQ(1, eq);
        }
    }
}

TEST_F(MsgVectorDateTimeTestsC, DISABLED_AddVectorDateTimeNullAdd)
// Disabled as mamaMsg_addVectorDateTime is not implemented.
{
    mStatus = mamaMsg_addVectorDateTime (mMsg,
                                         NULL,
                                         1,
                                         NULL,
                                         VECTOR_SIZE);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgVectorDateTimeTestsC, AddVectorDateTimeNullMsg)
{
    mStatus = mamaMsg_addVectorDateTime (NULL,
                                         NULL,
                                         1,
                                         mIn,
                                         VECTOR_SIZE);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

// UpdateVectorDateTime test fixtures
// **********************************

TEST_F(MsgVectorDateTimeTestsC, DISABLED_UpdateVectorDateTime)
// Disabled as mamaMsg_updateVectorTime is not implemented.
{
    mStatus = mamaMsg_addVectorDateTime (mMsg,
                                         NULL,
                                         1,
                                         mIn,
                                         VECTOR_SIZE);
    EXPECT_EQ(mStatus, MAMA_STATUS_OK);

    if (MAMA_STATUS_OK == mStatus)
    {
        mStatus = mamaMsg_getVectorDateTime (mMsg,
                                             NULL,
                                             1,
                                             (mama_u64_t* const*) &mOut,
                                             &mOutSize);
        EXPECT_EQ( mStatus, MAMA_STATUS_OK );
        EXPECT_EQ( mOutSize, VECTOR_SIZE );
    }


    /*
    mStatus = mamaMsg_updateVectorTime (mMsg,
                                        NULL,
                                        1,
                                        mUpdate,
                                        VECTOR_UPDATE_SIZE);
    EXPECT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorDateTime (mMsg,
                                         NULL,
                                         1,
                                         (mama_u64_t* const*) &mOut,
                                         &mOutSize);

    EXPECT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
    if (MAMA_STATUS_OK == mStatus)
    {
        for (unsigned int ii(0) ; ii < VECTOR_SIZE ; ++ii)
        {
            int eq = mamaDateTime_equal( mUpdate[ii],
                                         mOut[ii] );
            EXPECT_EQ(1, eq);
        }
    }
    */
}

TEST_F(MsgVectorDateTimeTestsC, DISABLED_UpdateVectorDateTimeNullAdd)
// Disabled as mamaMsg_updateVectorTime is not implemented.
{
    /*
    mStatus = mamaMsg_updateVectorTime (mMsg,
                                        NULL,
                                        1,
                                        NULL,
                                        VECTOR_UPDATE_SIZE);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
    */
}

TEST_F(MsgVectorDateTimeTestsC, DISABLED_UpdateVectorDateTimeNullMsg)
// Disabled as mamaMsg_updateVectorTime is not implemented.
{
    /*
    mStatus = mamaMsg_updateVectorTime (NULL,
                                        NULL,
                                        1,
                                        mUpdate,
                                        VECTOR_UPDATE_SIZE);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
    */
}

// GetVectorDateTime test fixtures
// *******************************
TEST_F(MsgVectorDateTimeTestsC, DISABLED_GetVectorDateTime)
// Disabled as mamaMsg_addVectorTime is not implemented.
{
    mStatus = mamaMsg_addVectorDateTime (mMsg,
                                         NULL,
                                         1,
                                         mIn,
                                         VECTOR_SIZE);
    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    if (MAMA_STATUS_OK == mStatus)
    {
        mStatus = mamaMsg_getVectorDateTime (mMsg,
                                             NULL,
                                             1,
                                             (mama_u64_t* const*) &mOut,
                                             &mOutSize);
        ASSERT_EQ( mStatus, MAMA_STATUS_OK );
        EXPECT_EQ( mOutSize, VECTOR_SIZE );
        if( MAMA_STATUS_OK == mStatus)
        {
            for (unsigned int ii(0) ; ii < VECTOR_SIZE ; ++ii)
            {
                int eq = mamaDateTime_equal( mIn[ii],
                                             mOut[ii] );
                EXPECT_EQ(1, eq);
            }
        }
    }

    /*
    mStatus = mamaMsg_updateVectorDateTime (mMsg,
                                            NULL,
                                            1,
                                            mUpdate,
                                            VECTOR_UPDATE_SIZE);
    EXPECT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorDateTime (mMsg,
                                         NULL,
                                         1,
                                         (mama_u64_t* const*) &mOut,
                                         &mOutSize);

    EXPECT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
    for (unsigned int ii(0) ; ii < VECTOR_UPDATE_SIZE ; ++ii)
    {
        int eq = mamaDateTime_equal( mUpdate[ii],
                                     mOut[ii] );
        EXPECT_EQ(1, eq);
    }
    */
}

TEST_F(MsgVectorDateTimeTestsC, DISABLED_GetVectorDateTimeNullAdd)
// Disabled as mamaMsg_addVectorDateTime is not implemented.
{
    mStatus = mamaMsg_addVectorDateTime (mMsg,
                                         NULL,
                                         1,
                                         mIn,
                                         VECTOR_SIZE);
    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorDateTime (mMsg,
                                         NULL,
                                         1,
                                         NULL,
                                         &mOutSize);
    ASSERT_EQ(mStatus, MAMA_STATUS_NULL_ARG);

    mStatus = mamaMsg_getVectorDateTime (mMsg,
                                         NULL,
                                         1,
                                         (mama_u64_t* const*) &mOut,
                                         NULL);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgVectorDateTimeTestsC, DISABLED_GetVectorDateTimeNullMsg)
// Disabled as mamaMsg_addVectorDateTime is not implemented.
{
    mStatus = mamaMsg_addVectorDateTime (mMsg,
                                         NULL,
                                         1,
                                         mIn,
                                         VECTOR_SIZE);
    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorDateTime (NULL,
                                         NULL,
                                         1,
                                         (mama_u64_t* const*) &mOut,
                                         &mOutSize);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

// ****************************************************************************
// Price Test Suite
// ****************************************************************************

class MsgVectorPriceTestsC : public MsgVectorTestsC
{
public:

    virtual ~MsgVectorPriceTestsC()
    {}

protected:

    mamaPrice mIn[ VECTOR_SIZE ];
    mamaPrice mUpdate[ VECTOR_UPDATE_SIZE ];
    const mamaPrice *mOut;
    mama_size_t mOutSize;

    MsgVectorPriceTestsC() 
        : mIn( )
        , mUpdate( )
        , mOut( NULL )
        , mOutSize( 0 )
    {}
 
    virtual void SetUp()
    {
        MsgVectorTestsC::SetUp();

        for( uint32_t ii(0) ; ii < VECTOR_SIZE ; ++ii )
        {
            mamaPrice_create( &mIn[ ii ] );
            mamaPrice_setValue( mIn[ ii ], (double) (((double) ii ) * 1.1) );
        }

        for( uint32_t ii(0) ; ii < VECTOR_UPDATE_SIZE ; ++ii )
        {
            mamaPrice_create( &mUpdate[ ii ] );
            mamaPrice_setValue( mUpdate[ ii ], (double) (((double) ii ) * 2.2) );
        }
    }

    virtual void TearDown()
    {
        for( uint32_t ii(0) ; ii < VECTOR_SIZE ; ++ii )
        {
            mamaPrice_destroy( mIn[ii] );
        }

        for( uint32_t ii(0) ; ii < VECTOR_UPDATE_SIZE ; ++ii )
        {
            mamaPrice_destroy( mUpdate[ii] );
        }

        MsgVectorTestsC::TearDown();
    }
};

// AddVectorPrice test fixtures
// ****************************

TEST_F(MsgVectorPriceTestsC, DISABLED_AddVectorPrice)
// Disabled as mamaMsg_addVectorPrice is not implemented.
{
    mStatus = mamaMsg_addVectorPrice (mMsg,
                                      NULL,
                                      1,
                                      mIn,
                                      VECTOR_SIZE);
    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorPrice (mMsg,
                                      NULL,
                                      1,
                                      (const mamaPrice*) &mOut,
                                      &mOutSize);
    ASSERT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( mOutSize, VECTOR_SIZE );

    if (MAMA_STATUS_OK == mStatus)
    {
        for (unsigned int ii(0) ; ii < VECTOR_SIZE ; ++ii)
        {
            int eq = mamaPrice_equal( mIn[ii],
                                      mOut[ii] );
            EXPECT_EQ(1, eq);
        }
    }
}

TEST_F(MsgVectorPriceTestsC, DISABLED_AddVectorPriceNullAdd)
// Disabled as mamaMsg_addVectorPrice is not implemented.
{
    mStatus = mamaMsg_addVectorPrice (mMsg,
                                      NULL,
                                      1,
                                      NULL,
                                      VECTOR_SIZE);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgVectorPriceTestsC, AddVectorPriceNullMsg)
{
    mStatus = mamaMsg_addVectorPrice (NULL,
                                      NULL,
                                      1,
                                      mIn,
                                      VECTOR_SIZE);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

// UpdateVectorPrice test fixtures
// *******************************

TEST_F(MsgVectorPriceTestsC, DISABLED_UpdateVectorPrice)
// Disabled as  mamaMsg_updateVectorPrice is not implemented.
{
    mStatus = mamaMsg_addVectorPrice (mMsg,
                                      NULL,
                                      1,
                                      mIn,
                                      VECTOR_SIZE);
    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    if (MAMA_STATUS_OK == mStatus)
    {
        mStatus = mamaMsg_getVectorPrice (mMsg,
                                          NULL,
                                          1,
                                          (const mamaPrice*) &mOut,
                                          &mOutSize);
        ASSERT_EQ( mStatus, MAMA_STATUS_OK );
        EXPECT_EQ( mOutSize, VECTOR_SIZE );
    }

    /*
    mStatus = mamaMsg_updateVectorPrice (mMsg,
                                         NULL,
                                         1,
                                         (void* const**) &mUpdate,
                                         VECTOR_UPDATE_SIZE);
    EXPECT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorPrice (mMsg,
                                      NULL,
                                      1,
                                      (const mamaPrice*) &mOut,
                                      &mOutSize);
    EXPECT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
    if (MAMA_STATUS_OK == mStatus)
    {
        for (unsigned int ii(0) ; ii < VECTOR_SIZE ; ++ii)
        {
            int eq = mamaPrice_equal( mOut[ii],
                                      mUpdate[ii] );
            EXPECT_EQ(1, eq);
        }
    }
    */
}

TEST_F(MsgVectorPriceTestsC, DISABLED_UpdateVectorPriceNullAdd)
// Disabeld as mamaMsg_updateVectorPrice is not implemented.
{
    /*
    mStatus = mamaMsg_updateVectorPrice (mMsg,
                                         NULL,
                                         1,
                                         NULL,
                                         VECTOR_UPDATE_SIZE);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
    */
}

TEST_F(MsgVectorPriceTestsC, DISABLED_UpdateVectorPriceNullMsg)
// Disabled as mamaMsg_updateVectorPrice is not implemented.
{
    /*
    mStatus = mamaMsg_updateVectorPrice (NULL,
                                         NULL,
                                         1,
                                         (void* const**) mUpdate,
                                         VECTOR_UPDATE_SIZE);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
    */
}

// GetVectorPrice test fixtures
// *******************************
TEST_F(MsgVectorPriceTestsC, DISABLED_GetVectorPrice)
// Disabled as mamaMsg_addVectorPrice is not implemented.
{
    mStatus = mamaMsg_addVectorPrice (mMsg,
                                      NULL,
                                      1,
                                      mIn,
                                      VECTOR_SIZE);
    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    if (MAMA_STATUS_OK == mStatus)
    {
        mStatus = mamaMsg_getVectorPrice (mMsg,
                                          NULL,
                                          1,
                                          (const mamaPrice*) &mOut,
                                          &mOutSize);
        ASSERT_EQ( mStatus, MAMA_STATUS_OK );
        EXPECT_EQ( mOutSize, VECTOR_SIZE );
        if( MAMA_STATUS_OK == mStatus)
        {
            for (unsigned int ii(0) ; ii < VECTOR_SIZE ; ++ii)
            {
                int eq = mamaPrice_equal( mIn[ii],
                                             mOut[ii] );
                EXPECT_EQ(1, eq);
            }
        }
    }

    /*
    mStatus = mamaMsg_updateVectorPrice (mMsg,
                                         NULL,
                                         1,
                                         (void* const**) mUpdate,
                                         VECTOR_UPDATE_SIZE);
    EXPECT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorPrice (mMsg,
                                      NULL,
                                      1,
                                      (const mamaPrice*) &mOut,
                                      &mOutSize);

    EXPECT_EQ( mStatus, MAMA_STATUS_OK );
    EXPECT_EQ( mOutSize, VECTOR_UPDATE_SIZE );
    for (unsigned int ii(0) ; ii < VECTOR_UPDATE_SIZE ; ++ii)
    {
        int eq = mamaPrice_equal( mUpdate[ii],
                                  mOut[ii] );
        EXPECT_EQ(0, eq);
    }
    */
}

TEST_F(MsgVectorPriceTestsC, DISABLED_GetVectorPriceNullAdd)
// Disabled as mamaMsg_addVectorPrice is not implemented.
{
    mStatus = mamaMsg_addVectorPrice (mMsg,
                                      NULL,
                                      1,
                                      mIn,
                                      VECTOR_SIZE);
    ASSERT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorPrice (mMsg,
                                      NULL,
                                      1,
                                      NULL,
                                      &mOutSize);
    ASSERT_EQ(mStatus, MAMA_STATUS_NULL_ARG);

    mStatus = mamaMsg_getVectorPrice (mMsg,
                                      NULL,
                                      1,
                                      (const mamaPrice*) &mOut,
                                      NULL);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgVectorPriceTestsC, DISABLED_GetVectorPriceNullMsg)
// Disabled as mamaMsg_addVectorPrice is not implemented.
{
    mStatus = mamaMsg_addVectorPrice (mMsg,
                                      NULL,
                                      1,
                                      mIn,
                                      VECTOR_SIZE);
    EXPECT_EQ(mStatus, MAMA_STATUS_OK);

    mStatus = mamaMsg_getVectorPrice (NULL,
                                      NULL,
                                      1,
                                      (const mamaPrice*) &mOut,
                                      &mOutSize);
    EXPECT_EQ(mStatus, MAMA_STATUS_NULL_ARG);
}

