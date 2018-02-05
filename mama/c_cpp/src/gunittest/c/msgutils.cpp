/* $Id: timertest.cpp,v 1.1.2.1.2.4 2012/08/24 16:12:00 clintonmcdowell Exp $
 *
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
#include "MainUnitTestC.h"
#include <mama/mama.h>
#include <msgutils.h>

class MamaMsgUtilsTestC : public ::testing::Test
{
protected:
    MamaMsgUtilsTestC();
    virtual ~MamaMsgUtilsTestC();

    virtual void SetUp();
    virtual void TearDown ();

    mamaBridge bridge;
    mamaMsg    msg;
};

MamaMsgUtilsTestC::MamaMsgUtilsTestC() :
                        bridge (NULL),
                        msg    (NULL)
{
    mama_loadBridge (&bridge, getMiddleware ());
    mama_open ();
}

MamaMsgUtilsTestC::~MamaMsgUtilsTestC()
{
    mama_close ();
}

void MamaMsgUtilsTestC::SetUp(void)
{
    /* Create a new message for each test. */
    ASSERT_EQ (MAMA_STATUS_OK, mamaMsg_create (&msg));

    /* Add a number of additional fields to ensure the message is partially
     * populated
     */
    ASSERT_EQ (MAMA_STATUS_OK, mamaMsg_addI8  (msg, "test1", 1001, 10));
    ASSERT_EQ (MAMA_STATUS_OK, mamaMsg_addF32 (msg, "test2", 1002, 25.22));
}

void MamaMsgUtilsTestC::TearDown(void)
{
    /* Destroy the message after each test */
    ASSERT_EQ (MAMA_STATUS_OK, mamaMsg_destroy (msg));
    msg = NULL;
}

/* ************************************************************************* */
/* Test Functions */
/* ************************************************************************* */

/*  Description: Populate a mamaMsg with a number of fields, including an issue
 *  symbol. Call msgUtils_getIssueSymbol and ensure it a) returns MAMA_STATUS_OK
 *  and b) gets the correct string back.
 *
 *  Expected Result: MAMA_STATUS_OK and "TEST"
 */
TEST_F (MamaMsgUtilsTestC, getIssueSymbol)
{
    const char* result = NULL;

    ASSERT_EQ (MAMA_STATUS_OK, mamaMsg_addString (msg,
                                                  "wIssueSymbol",
                                                  305,
                                                  "TEST"));

    EXPECT_EQ (MAMA_STATUS_OK, msgUtils_getIssueSymbol (msg, &result));
    EXPECT_STREQ ("TEST", result);
}

/*  Description: Populate a mamaMsg with a number of fields, including a
 *  wIndexSymbol, but not wIssueSymbol. Call msgUtils_getIssueSymbol and ensure
 *  it a) returns MAMA_STATUS_OK and b) gets the correct string back.
 *
 *  Expected Result: MAMA_STATUS_OK and "TEST"
 */
TEST_F (MamaMsgUtilsTestC, getIssueSymbolIndexSymbol)
{
    const char* result = NULL;

    ASSERT_EQ (MAMA_STATUS_OK, mamaMsg_addString (msg,
                                                  "wIssueSymbol",
                                                  293,
                                                  "TEST"));

    EXPECT_EQ (MAMA_STATUS_OK, msgUtils_getIssueSymbol (msg, &result));
    EXPECT_STREQ ("TEST", result);
}

/*  Description: Populate a mamaMsg with a number of fields, then call
 *  msgUtils_setStatus with a valid status.
 *
 *  Expected Result: MAMA_STATUS_OK
 */
TEST_F (MamaMsgUtilsTestC, setStatus)
{
    mamaMsgStatus status = MAMA_MSG_STATUS_STALE;

    EXPECT_EQ (MAMA_STATUS_OK, msgUtils_setStatus (msg, status));
}

/*  Description: Attempt to call msgUtils_setStatus with a valid status, but
 *  against a NULL message.
 *
 *  Expected Result: MAMA_STATUS_NULL_ARG
 */
TEST_F (MamaMsgUtilsTestC, setStatusNullMsg)
{
    mamaMsgStatus status = MAMA_MSG_STATUS_STALE;
    mamaMsg nullMsg = NULL;

    EXPECT_EQ (MAMA_STATUS_NULL_ARG, msgUtils_setStatus (nullMsg, status));
}

/*  Description: Populate a mamaMsg with a number of fields, but do not populate
 *  either wIssueSymbol or wIndexSymbol. Call msgUtils_getIssueSymbol and ensure
 *  it a) returns MAMA_STATUS_OK and b) doesn't try and populate the result.
 *
 *  Expected Result: MAMA_STATUS_NOT_FOUND and NULL
 */
TEST_F (MamaMsgUtilsTestC, getIssueSymbolNoSymbol)
{
    const char* result = NULL;

    EXPECT_EQ (MAMA_STATUS_NOT_FOUND, msgUtils_getIssueSymbol (msg, &result));
    EXPECT_EQ (NULL, result);
}

/*  Description: Populate a mamaMsg with a number of fields, including the
 *  mdMsgTotal field with the value 64. Call msgUtils_msgTotal and ensure it
 *  a) returns MAMA_STATUS_OK and b) populates the result with the correct value
 *
 *  Expected Result: MAMA_STATUS_OK and 64.
 */
TEST_F (MamaMsgUtilsTestC, msgTotal)
{
    short result = 0;

    ASSERT_EQ (MAMA_STATUS_OK, mamaMsg_addI32 (msg,
                                               MamaFieldMsgTotal.mName,
                                               MamaFieldMsgTotal.mFid,
                                               64));

    ASSERT_EQ (MAMA_STATUS_OK, mamaMsg_addI32 (msg,
                                               MamaFieldMsgNum.mName,
                                               MamaFieldMsgNum.mFid,
                                               128));

    EXPECT_EQ (MAMA_STATUS_OK, msgUtils_msgTotal (msg, &result));
    EXPECT_EQ (64, result);
}

/*  Description: Populate a mamaMsg with a number of fields, but not the
 *  mdMsgTotal field. Call msgUtils_msgTotal and ensure it a) returns
 *  MAMA_STATUS_NOT_FOUND and b) doesn't try and populate the result.
 *
 *  Expected Result: MAMA_STATUS_NOT_FOUND and 0
 */
TEST_F (MamaMsgUtilsTestC, msgTotalEmptyField)
{
    short result = 0;

    ASSERT_EQ (MAMA_STATUS_OK, mamaMsg_addI32 (msg,
                                               MamaFieldMsgNum.mName,
                                               MamaFieldMsgNum.mFid,
                                               128));

    EXPECT_EQ (MAMA_STATUS_NOT_FOUND, msgUtils_msgTotal (msg, &result));
    EXPECT_EQ (0, result);
}

/*  Description: Call msgUtils_msgTotal against a NULL msg and ensure it
 *  a) returns MAMA_STATUS_NOT_FOUND and b) doesn't try and populate the result.
 *
 *  Expected Result: MAMA_STATUS_NULL_ARG and 0
 */
TEST_F (MamaMsgUtilsTestC, msgTotalNullMsg)
{
    short   result = 0;
    mamaMsg nullMsg = NULL;

    EXPECT_EQ (MAMA_STATUS_NULL_ARG, msgUtils_msgTotal (nullMsg, &result));
    EXPECT_EQ (0, result);
}

/*  Description: Populate a mamaMsg with a number of fields, including the
 *  mdMsgNum field. Call msgUtils_getNum and ensure it a) returns
 *  MAMA_STATUS_OK, and b) populates the result with the correct value.
 *
 *  Expected Result: MAMA_STATUS_OK and 128
 */
TEST_F (MamaMsgUtilsTestC, msgNum)
{
    short result = 0;

    ASSERT_EQ (MAMA_STATUS_OK, mamaMsg_addI32 (msg,
                                               MamaFieldMsgTotal.mName,
                                               MamaFieldMsgTotal.mFid,
                                               64));

    ASSERT_EQ (MAMA_STATUS_OK, mamaMsg_addI32 (msg,
                                               MamaFieldMsgNum.mName,
                                               MamaFieldMsgNum.mFid,
                                               128));

    EXPECT_EQ (MAMA_STATUS_OK, msgUtils_msgNum (msg, &result));
    EXPECT_EQ (128, result);
}

/*  Description: Populate a mamaMsg with a number of fields, but not the
 *  mdMsgNum field. Call msgUtils_getNum and ensure it a) returns
 *  MAMA_STATUS_NOT_FOUND and b) doesn't attempt the populate the result.
 *
 *  Expected Result: MAMA_STATUS_NOT_FOUND and 0.
 */
TEST_F (MamaMsgUtilsTestC, msgNumEmptyField)
{
    short result = 0;

    ASSERT_EQ (MAMA_STATUS_OK, mamaMsg_addI32 (msg,
                                               MamaFieldMsgTotal.mName,
                                               MamaFieldMsgTotal.mFid,
                                               64));

    EXPECT_EQ (MAMA_STATUS_NOT_FOUND, msgUtils_msgNum (msg, &result));
    EXPECT_EQ (0, result);
}

/*  Description: Call msgUtils_getNum against a NULL message and ensure it
 *  a) returns MAMA_STATUS_NULL_ARG and b) doesn't attempt the populate the
 *  result.
 *
 *  Expected Result: MAMA_STATUS_NULL_ARG and 0
 */
TEST_F (MamaMsgUtilsTestC, msgNumNullMsg)
{
    short   result = 0;
    mamaMsg nullMsg = NULL;

    EXPECT_EQ (MAMA_STATUS_NULL_ARG, msgUtils_msgNum (nullMsg, &result));
    EXPECT_EQ (0, result);
}

