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

class MsgCompositeTestsC : public ::testing::Test
{
protected:
    MsgCompositeTestsC(void) :
        mMsg           (NULL),
        mPayloadBridge (NULL)
    {}
    virtual ~MsgCompositeTestsC(void) {};

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
/**********************
 *  STRING TEST SUITE *
 *********************/

class MsgStringTests : public MsgCompositeTestsC
{
protected:

    const char *m_in, *m_update, *m_out;

    MsgStringTests()
        : m_in("__TEST__")
        , m_update("__TSET__")
        , m_out(NULL)
    {
    }
};

/* addString tests */
TEST_F(MsgStringTests, addStringValid)
{
    ASSERT_EQ(mamaMsg_addString(mMsg, NULL, 1, m_in), MAMA_STATUS_OK);
}

TEST_F(MsgStringTests, addStringInValidMsg)
{
    ASSERT_EQ(mamaMsg_addString(NULL, NULL, 1, m_in), MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgStringTests, DISABLED_addStringInValidString)
{
    ASSERT_EQ(mamaMsg_addString(mMsg, NULL, 1, NULL), MAMA_STATUS_NULL_ARG);
}

/* updateString tests */
TEST_F(MsgStringTests, updateStringValid)
{
    ASSERT_EQ(mamaMsg_updateString(mMsg, NULL, 1, m_update), 
              MAMA_STATUS_OK);
}

TEST_F(MsgStringTests, updateStringInValidMsg)
{
    ASSERT_EQ(mamaMsg_updateString(NULL, NULL, 1, m_update), 
              MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgStringTests, updateStringInValidString)
{
    ASSERT_EQ(mamaMsg_updateString(mMsg, NULL, 1, NULL),
              MAMA_STATUS_NULL_ARG);
}

/* getString tests */
TEST_F(MsgStringTests, getStringValid)
{
    ASSERT_EQ(mamaMsg_addString(mMsg, NULL, 1, m_in), MAMA_STATUS_OK);
    ASSERT_EQ(mamaMsg_getString(mMsg, NULL, 1, &m_out), MAMA_STATUS_OK);
    
    ASSERT_STREQ(m_in, m_out);
}

TEST_F(MsgStringTests, getStringInValidOutput)
{
    ASSERT_EQ(mamaMsg_addString(mMsg, NULL, 1, m_in), MAMA_STATUS_OK);
    ASSERT_EQ(mamaMsg_getString(mMsg, NULL, 1, NULL), MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgStringTests, getStringInValidMsg)
{
    ASSERT_EQ(mamaMsg_addString(mMsg, NULL, 1, m_in), MAMA_STATUS_OK);
    ASSERT_EQ(mamaMsg_getString(NULL, NULL, 1, NULL), MAMA_STATUS_NULL_ARG);
}


/**********************
 *  OPAQUE TEST SUITE *
 *********************/
#define OPAQUE_SIZE 12
 class MsgOpaqueTests : public MsgCompositeTestsC
 {
 protected:

    //Reusable variables.
    const void* m_in;
    const void* m_update;
    const void* m_out;

    MsgOpaqueTests()
    : m_in(NULL)
    , m_update(NULL)
    , m_out(NULL)
    {
        m_in = malloc (OPAQUE_SIZE);
        m_update = malloc (OPAQUE_SIZE);
    }

    ~MsgOpaqueTests()
    {
        free ((void*)m_in);
        free ((void*)m_update);
    }
 };

/* addOpaque tests */
TEST_F(MsgOpaqueTests, addOpaqueValid)
{
    ASSERT_EQ(mamaMsg_addOpaque(mMsg, NULL, 1, m_in, OPAQUE_SIZE),
              MAMA_STATUS_OK);
}

TEST_F(MsgOpaqueTests, addOpaqueInValidMsg)
{
    ASSERT_EQ(mamaMsg_addOpaque(NULL, NULL, 1, m_in, OPAQUE_SIZE),
              MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgOpaqueTests, addOpaqueInValidOpaque)
{
    ASSERT_EQ(mamaMsg_addOpaque(mMsg, NULL, 1, NULL, OPAQUE_SIZE),
              MAMA_STATUS_NULL_ARG);
}

/* updateOpaque tests */
TEST_F(MsgOpaqueTests, updateOpaqueValid)
{
    ASSERT_EQ(mamaMsg_updateOpaque(mMsg, NULL, 1, m_update, OPAQUE_SIZE),
              MAMA_STATUS_OK);
}

TEST_F(MsgOpaqueTests, updateOpaqueInValidMsg)
{
    ASSERT_EQ(mamaMsg_updateOpaque(NULL, NULL, 1, m_update, OPAQUE_SIZE), 
              MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgOpaqueTests, updateOpaqueInValidOpaque)
{
    ASSERT_EQ(mamaMsg_updateOpaque(mMsg, NULL, 1, NULL, OPAQUE_SIZE),
              MAMA_STATUS_NULL_ARG);
}

/* getOpaque tests */
TEST_F(MsgOpaqueTests, getOpaqueValid)
{
    mama_size_t size = 0;
    ASSERT_EQ(mamaMsg_addOpaque(mMsg, NULL, 1, m_in, OPAQUE_SIZE),
              MAMA_STATUS_OK);
    ASSERT_EQ(mamaMsg_getOpaque(mMsg, NULL, 1, &m_out, &size),
              MAMA_STATUS_OK);
}

TEST_F(MsgOpaqueTests, getOpaqueInValidOutput)
{
    mama_size_t size = 0;
    ASSERT_EQ(mamaMsg_addOpaque(mMsg, NULL, 1, m_in, OPAQUE_SIZE),
              MAMA_STATUS_OK);
    ASSERT_EQ(mamaMsg_getOpaque(mMsg, NULL, 1, NULL, &size),
              MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgOpaqueTests, getOpaqueInValidMessage)
{
    mama_size_t size = 0;
    ASSERT_EQ(mamaMsg_addOpaque(mMsg, NULL, 1, m_in, OPAQUE_SIZE),
              MAMA_STATUS_OK);
    ASSERT_EQ(mamaMsg_getOpaque(NULL, NULL, 1, NULL, &size),
              MAMA_STATUS_NULL_ARG);
}

/**************************
 *  DATETIME TEST SUITE   *
 **************************/
 class MsgDateTimeTests : public MsgCompositeTestsC
 {
 protected:

    //Reusable variables.
    mamaDateTime m_in;
    mamaDateTime m_update;
    mamaDateTime m_out;

    MsgDateTimeTests()
    : m_in(NULL)
    , m_update(NULL)
    , m_out(NULL)
    {
    }
        virtual void SetUp()
        {
            MsgCompositeTestsC::SetUp();
            mamaDateTime_create(&m_in);
            mamaDateTime_create(&m_update);
            mamaDateTime_create(&m_out);

            mamaDateTime_setToNow(m_in);
            mamaDateTime_setToNow(m_update);
        }

        virtual void TearDown()
        {
            mamaDateTime_destroy(m_in);
            mamaDateTime_destroy(m_update);
            mamaDateTime_destroy(m_out);

            MsgCompositeTestsC::TearDown();
        }
 };

/* addDateTime tests */
TEST_F(MsgDateTimeTests, addDateTimeValid)
{
    ASSERT_EQ(mamaMsg_addDateTime(mMsg, NULL, 1, m_in),
              MAMA_STATUS_OK);
}

TEST_F(MsgDateTimeTests, addDateTimeInValidMsg)
{
    ASSERT_EQ(mamaMsg_addDateTime(NULL, NULL, 1, m_in),
              MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgDateTimeTests, addDateTimeInValidDateTime)
{
    ASSERT_EQ(mamaMsg_addDateTime(mMsg, NULL, 1, NULL),
              MAMA_STATUS_NULL_ARG);
}

/* updateDateTime tests */
TEST_F(MsgDateTimeTests, updateDateTimeValid)
{
    ASSERT_EQ(mamaMsg_updateDateTime(mMsg, NULL, 1, m_update),
              MAMA_STATUS_OK);
}

TEST_F(MsgDateTimeTests, updateDateTimeInValidMsg)
{
    ASSERT_EQ(mamaMsg_updateDateTime(NULL, NULL, 1, m_update), 
              MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgDateTimeTests, updateDateTimeInValidDateTime)
{
    ASSERT_EQ(mamaMsg_updateDateTime(mMsg, NULL, 1, NULL),
              MAMA_STATUS_NULL_ARG);
}

/* getDateTime tests */
TEST_F(MsgDateTimeTests, getDateTimeValid)
{
    ASSERT_EQ(mamaMsg_addDateTime(mMsg, NULL, 1, m_in),
              MAMA_STATUS_OK);
    ASSERT_EQ(mamaMsg_getDateTime(mMsg, NULL, 1, m_out),
              MAMA_STATUS_OK);
}

TEST_F(MsgDateTimeTests, getDateTimeInValidOutput)
{
    ASSERT_EQ(mamaMsg_addDateTime(mMsg, NULL, 1, m_in),
              MAMA_STATUS_OK);
    ASSERT_EQ(mamaMsg_getDateTime(mMsg, NULL, 1, NULL),
              MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgDateTimeTests, getDateTimeInValidMsg)
{
    ASSERT_EQ(mamaMsg_addDateTime(mMsg, NULL, 1, m_in),
              MAMA_STATUS_OK);
    ASSERT_EQ(mamaMsg_getDateTime(NULL, NULL, 1, m_out),
              MAMA_STATUS_NULL_ARG);
}

/****************************************************
 *                  PRICE TEST SUITE                *
 ****************************************************/
class MsgPriceTestsC : public MsgCompositeTestsC
{
protected:
    //Reusable variables
    mamaPrice m_in;
    mamaPrice m_update;
    mamaPrice m_out;

    MsgPriceTestsC()
        : m_in(NULL)
        , m_update(NULL)
        , m_out(NULL)
    {
    }

    virtual void SetUp()
    {
        MsgCompositeTestsC::SetUp();
        
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

        MsgCompositeTestsC::TearDown();
    }
};
/* addPrice tests */
TEST_F(MsgPriceTestsC, addPriceValid)
{
    ASSERT_EQ(mamaMsg_addPrice(mMsg, NULL, 1, m_in),
              MAMA_STATUS_OK);
}

TEST_F(MsgPriceTestsC, addPriceInValidMsg)
{
    ASSERT_EQ(mamaMsg_addPrice(NULL, NULL, 1, m_in),
              MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgPriceTestsC, addPriceInValidPrice)
{
    ASSERT_EQ(mamaMsg_addPrice(mMsg, NULL, 1, NULL),
              MAMA_STATUS_NULL_ARG);
}

/* updatePrice tests */
TEST_F(MsgPriceTestsC, updatePriceValid)
{
    ASSERT_EQ(mamaMsg_updatePrice(mMsg, NULL, 1, m_update),
              MAMA_STATUS_OK);
}

TEST_F(MsgPriceTestsC, updatePriceInValidMsg)
{
    ASSERT_EQ(mamaMsg_updatePrice(NULL, NULL, 1, m_update), 
              MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgPriceTestsC, updatePriceInValidPrice)
{
    ASSERT_EQ(mamaMsg_updatePrice(mMsg, NULL, 1, NULL),
              MAMA_STATUS_NULL_ARG);
}

/* getPrice tests */
TEST_F(MsgPriceTestsC, getPriceValid)
{
    ASSERT_EQ(mamaMsg_addPrice(mMsg, NULL, 1, m_in),
              MAMA_STATUS_OK);
    ASSERT_EQ(mamaMsg_getPrice(mMsg, NULL, 1, m_out),
              MAMA_STATUS_OK);
}

TEST_F(MsgPriceTestsC, getPriceInValidOutput)
{
    ASSERT_EQ(mamaMsg_addPrice(mMsg, NULL, 1, m_in),
              MAMA_STATUS_OK);
    ASSERT_EQ(mamaMsg_getPrice(mMsg, NULL, 1, NULL),
              MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgPriceTestsC, getPriceInValidMsg)
{
    ASSERT_EQ(mamaMsg_addPrice(mMsg, NULL, 1, m_in),
              MAMA_STATUS_OK);
    ASSERT_EQ(mamaMsg_getPrice(NULL, NULL, 1, m_out),
              MAMA_STATUS_NULL_ARG);
}

class MsgSubMsgTestsC : public MsgCompositeTestsC
{
protected:
    //Reusable variables.
    
    mamaMsg m_in;
    mamaMsg m_update;
    mamaMsg m_out;

    MsgSubMsgTestsC() : m_out(NULL)
    {
        mamaMsg_create(&m_in);
        mamaMsg_create(&m_update);
    }

    ~MsgSubMsgTestsC()
    {
        mamaMsg_destroy(m_in);
        mamaMsg_destroy(m_update);
    }
};
/* addMsg tests */
TEST_F(MsgSubMsgTestsC, addSubMsgValid)
{
    ASSERT_EQ(mamaMsg_addMsg(mMsg, NULL, 1, m_in),
              MAMA_STATUS_OK);
}

TEST_F(MsgSubMsgTestsC, addSubMsgInValidMsg)
{
    ASSERT_EQ(mamaMsg_addMsg(NULL, NULL, 1, m_in),
              MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgSubMsgTestsC, addSubMsgInValidSubMsg)
{
    ASSERT_EQ(mamaMsg_addMsg(mMsg, NULL, 1, NULL),
              MAMA_STATUS_NULL_ARG);
}

/* updateMsg tests */
TEST_F(MsgSubMsgTestsC, DISABLED_updateSubMsgValid)
{
    ASSERT_EQ(mamaMsg_updateSubMsg(mMsg, NULL, 1, m_update),
              MAMA_STATUS_OK);
}

TEST_F(MsgSubMsgTestsC, DISABLED_updateSubMsgInValidMsg)
{
    ASSERT_EQ(mamaMsg_updateSubMsg(NULL, NULL, 1, m_update), 
              MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgSubMsgTestsC, updateSubMsgInValidSubMsg)
{
    ASSERT_EQ(mamaMsg_updateSubMsg(mMsg, NULL, 1, NULL),
              MAMA_STATUS_NULL_ARG);
}

/* getMsg tests */
TEST_F(MsgSubMsgTestsC, getSubMsgValid)
{
    ASSERT_EQ(mamaMsg_addMsg(mMsg, NULL, 1, m_in),
              MAMA_STATUS_OK);
    ASSERT_EQ(mamaMsg_getMsg(mMsg, NULL, 1, &m_out),
              MAMA_STATUS_OK);
}

TEST_F(MsgSubMsgTestsC, DISABLED_getSubMsgInValidOutput)
{
    ASSERT_EQ(mamaMsg_addMsg(mMsg, NULL, 1, m_in),
              MAMA_STATUS_OK);
    ASSERT_EQ(mamaMsg_getMsg(mMsg, NULL, 1, NULL),
              MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgSubMsgTestsC, getSubMsgInValidMsg)
{
    ASSERT_EQ(mamaMsg_addMsg(mMsg, NULL, 1, m_in),
              MAMA_STATUS_OK);
    ASSERT_EQ(mamaMsg_getMsg(NULL, NULL, 1, &m_out),
              MAMA_STATUS_NULL_ARG);
}
