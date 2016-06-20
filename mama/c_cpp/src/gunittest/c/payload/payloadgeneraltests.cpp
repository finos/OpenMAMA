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
#include "payloadbridge.h"
#include "msgimpl.h"
#include <set>
#include <limits>
using std::cout;
using std::endl;


class PayloadGeneralTests : public ::testing::Test
{
protected:
    PayloadGeneralTests(void);
    virtual ~PayloadGeneralTests(void);

    virtual void SetUp(void);
    virtual void TearDown(void);

    mamaPayloadBridge   aBridge;
    mamaBridge          mMiddlewareBridge;
    mama_status         result;
};

PayloadGeneralTests::PayloadGeneralTests(void)
    : aBridge (NULL)
    , mMiddlewareBridge (NULL)
    , result (MAMA_STATUS_OK)
{
}

PayloadGeneralTests::~PayloadGeneralTests(void)
{
}

void PayloadGeneralTests::SetUp(void)
{
    mama_loadPayloadBridge (&aBridge,getPayload());
    mama_loadBridge (&mMiddlewareBridge, getMiddleware());
    mama_open ();
}

void PayloadGeneralTests::TearDown(void)
{
    mama_close ();
}

void dummyIteratorCallback (const mamaMsg       msg,
                            const mamaMsgField  field,
                            void*               closure)
{

}


/* ************************************************************************* */
/* Tests */
/* ************************************************************************* */
TEST_F(PayloadGeneralTests, CreateValid)
{
   msgPayload       testPayload = NULL;

   result = aBridge->msgPayloadCreate(&testPayload);

   EXPECT_EQ (MAMA_STATUS_OK, result);
   aBridge->msgPayloadDestroy(testPayload);
}

TEST_F(PayloadGeneralTests, CreateInValid)
{
   result = aBridge->msgPayloadCreate(NULL);

   EXPECT_EQ (MAMA_STATUS_NULL_ARG, result);
}

TEST_F(PayloadGeneralTests, SerializeRoundtrip)
{
    msgPayload testPayload = NULL;
    result = aBridge->msgPayloadCreate (&testPayload);
    EXPECT_EQ (MAMA_STATUS_OK, result);

    mama_bool_t b = 1;
    char ch = 'V';
    char chVector [] = {'A', 'B', 'C'};
    mama_size_t chVectorSize = sizeof(chVector)/sizeof(chVector[0]);
    const char* str = "LONG TEST STRING";
    std::string s = "TEST";
    mamaDateTime dt;
    mamaPrice prc;
    ASSERT_EQ (MAMA_STATUS_OK, mamaDateTime_create (&dt));
    ASSERT_EQ (MAMA_STATUS_OK, mamaDateTime_setToNow (dt));
    ASSERT_EQ (MAMA_STATUS_OK, mamaPrice_create (&prc));
    ASSERT_EQ (MAMA_STATUS_OK, mamaPrice_setValue (prc, 17.79));

    mama_fid_t orig_fid = 100;
    mama_fid_t fid = orig_fid;
    mama_status ret = MAMA_STATUS_OK;
    ret = aBridge->msgPayloadAddBool (testPayload, "FirstNameOnly", ++fid, b);
    EXPECT_EQ (MAMA_STATUS_OK, ret);
    ret = aBridge->msgPayloadAddChar (testPayload, NULL, ++fid, ch);
    EXPECT_EQ (MAMA_STATUS_OK, ret);
    ret = aBridge->msgPayloadAddI8 (testPayload, NULL, ++fid, std::numeric_limits<mama_i8_t>::min ());
    EXPECT_EQ (MAMA_STATUS_OK, ret);
    ret = aBridge->msgPayloadAddU8 (testPayload, NULL, ++fid, std::numeric_limits<mama_u8_t>::max ());
    EXPECT_EQ (MAMA_STATUS_OK, ret);
    ret = aBridge->msgPayloadAddI16 (testPayload, NULL, ++fid, std::numeric_limits<mama_i16_t>::min ());
    EXPECT_EQ (MAMA_STATUS_OK, ret);
    ret = aBridge->msgPayloadAddU16 (testPayload, NULL, ++fid, std::numeric_limits<mama_u16_t>::max ());
    EXPECT_EQ (MAMA_STATUS_OK, ret);
    ret = aBridge->msgPayloadAddI32 (testPayload, NULL, ++fid, std::numeric_limits<mama_i32_t>::min ());
    EXPECT_EQ (MAMA_STATUS_OK, ret);
    ret = aBridge->msgPayloadAddU32 (testPayload, NULL, ++fid, std::numeric_limits<mama_u32_t>::max ());
    EXPECT_EQ (MAMA_STATUS_OK, ret);
    ret = aBridge->msgPayloadAddI64 (testPayload, NULL, ++fid, std::numeric_limits<mama_i64_t>::min ());
    EXPECT_EQ (MAMA_STATUS_OK, ret);
    ret = aBridge->msgPayloadAddU64 (testPayload, NULL, ++fid, std::numeric_limits<mama_u64_t>::max ());
    EXPECT_EQ (MAMA_STATUS_OK, ret);
    ret = aBridge->msgPayloadAddF32 (testPayload, NULL, ++fid, std::numeric_limits<mama_f32_t>::max ());
    EXPECT_EQ (MAMA_STATUS_OK, ret);
    ret = aBridge->msgPayloadAddF64 (testPayload, NULL, ++fid, std::numeric_limits<mama_f64_t>::max ());
    EXPECT_EQ (MAMA_STATUS_OK, ret);
    ret = aBridge->msgPayloadAddString (testPayload, NULL, ++fid, str);
    EXPECT_EQ (MAMA_STATUS_OK, ret);
    ret = aBridge->msgPayloadAddOpaque (testPayload, NULL, ++fid, (void*)s.data(), s.size());
    EXPECT_EQ (MAMA_STATUS_OK, ret);
    ret = aBridge->msgPayloadAddDateTime (testPayload, NULL, ++fid, dt);
    EXPECT_EQ (MAMA_STATUS_OK, ret);
    ret = aBridge->msgPayloadAddPrice (testPayload, NULL, ++fid, prc);
    EXPECT_EQ (MAMA_STATUS_OK, ret);
    ret = aBridge->msgPayloadAddVectorChar (testPayload, NULL, ++fid, chVector,
                                            chVectorSize);

    bool hasVectors = (MAMA_STATUS_NOT_IMPLEMENTED != ret);
    if (hasVectors)
        EXPECT_EQ (MAMA_STATUS_OK, ret);
    else
        --fid;

    // Create an populate a submessage...
    msgPayload subMsg = NULL;
    ret = aBridge->msgPayloadCreate (&subMsg);
    EXPECT_EQ (MAMA_STATUS_OK, ret);

    ret = aBridge->msgPayloadAddU32 (subMsg, NULL, 101, 123);
    EXPECT_EQ (MAMA_STATUS_OK, ret);
    ret = aBridge->msgPayloadAddString (subMsg, NULL, 102, str);
    EXPECT_EQ (MAMA_STATUS_OK, ret);

    ret = aBridge->msgPayloadAddMsg (testPayload, NULL, ++fid, subMsg);

    bool hasSubMessages = (MAMA_STATUS_NOT_IMPLEMENTED != ret);
    if (hasSubMessages)
        EXPECT_EQ (MAMA_STATUS_OK, ret);
    else
        --fid;

    mama_size_t numFieldsOut = 0;
    ret = aBridge->msgPayloadGetNumFields (testPayload, &numFieldsOut);
    EXPECT_EQ (MAMA_STATUS_OK, ret);
    EXPECT_EQ (fid-orig_fid, numFieldsOut);

    const void* buf = NULL;
    mama_size_t bufSize = 0;
    ret = aBridge->msgPayloadSerialize (testPayload, &buf, &bufSize);
    EXPECT_EQ (MAMA_STATUS_OK, ret);
    EXPECT_NE ((void*)NULL, buf);
    EXPECT_NE (0, bufSize);

    msgPayload testPayloadIn = NULL;
    result = aBridge->msgPayloadCreate (&testPayloadIn);
    EXPECT_EQ (MAMA_STATUS_OK, result);

    ret = aBridge->msgPayloadUnSerialize (testPayloadIn, (const void**)buf, bufSize);
    EXPECT_EQ (MAMA_STATUS_OK, ret);

    mama_size_t bs = 0;
    ret = aBridge->msgPayloadGetByteSize (testPayloadIn, &bs);
    if (ret != MAMA_STATUS_NOT_IMPLEMENTED)
    {
        EXPECT_EQ (MAMA_STATUS_OK, ret);
        EXPECT_EQ (bufSize, bs);
    }

    mama_size_t numFieldsIn = 0;
    ret = aBridge->msgPayloadGetNumFields (testPayloadIn, &numFieldsIn);
    EXPECT_EQ (MAMA_STATUS_OK, ret);
    EXPECT_EQ (numFieldsOut, numFieldsIn);

    mama_bool_t bIn = 0;
    char chIn = '\0';
    mama_i8_t i8In = 0;
    mama_u8_t u8In = 0;
    mama_i16_t i16In = 0;
    mama_u16_t u16In = 0;
    mama_i32_t i32In = 0;
    mama_u32_t u32In = 0;
    mama_i64_t i64In = 0;
    mama_u64_t u64In = 0;
    mama_f32_t f32In = 0;
    mama_f64_t f64In = 0;
    const char* strIn = "";
    void* bufIn = NULL;
    mama_size_t bufInSize = 0;
    mamaDateTime dtIn;
    mamaPrice prcIn;
    ASSERT_EQ (MAMA_STATUS_OK, mamaDateTime_create (&dtIn));
    ASSERT_EQ (MAMA_STATUS_OK, mamaPrice_create (&prcIn));

    fid = orig_fid;
    ret = aBridge->msgPayloadGetBool (testPayloadIn, NULL, ++fid, &bIn);
    EXPECT_EQ (MAMA_STATUS_OK, ret);
    EXPECT_EQ (b, bIn);
    ret = aBridge->msgPayloadGetChar (testPayloadIn, NULL, ++fid, &chIn);
    EXPECT_EQ (MAMA_STATUS_OK, ret);
    EXPECT_EQ (ch, chIn);
    ret = aBridge->msgPayloadGetI8 (testPayloadIn, NULL, ++fid, &i8In);
    EXPECT_EQ (MAMA_STATUS_OK, ret);
    EXPECT_EQ (std::numeric_limits<mama_i8_t>::min (), i8In);
    ret = aBridge->msgPayloadGetU8 (testPayloadIn, NULL, ++fid, &u8In);
    EXPECT_EQ (MAMA_STATUS_OK, ret);
    EXPECT_EQ (std::numeric_limits<mama_u8_t>::max (), u8In);
    ret = aBridge->msgPayloadGetI16 (testPayloadIn, NULL, ++fid, &i16In);
    EXPECT_EQ (MAMA_STATUS_OK, ret);
    EXPECT_EQ (std::numeric_limits<mama_i16_t>::min (), i16In);
    ret = aBridge->msgPayloadGetU16 (testPayloadIn, NULL, ++fid, &u16In);
    EXPECT_EQ (MAMA_STATUS_OK, ret);
    EXPECT_EQ (std::numeric_limits<mama_u16_t>::max (), u16In);
    ret = aBridge->msgPayloadGetI32 (testPayloadIn, NULL, ++fid, &i32In);
    EXPECT_EQ (MAMA_STATUS_OK, ret);
    EXPECT_EQ (std::numeric_limits<mama_i32_t>::min (), i32In);
    ret = aBridge->msgPayloadGetU32 (testPayloadIn, NULL, ++fid, &u32In);
    EXPECT_EQ (MAMA_STATUS_OK, ret);
    EXPECT_EQ (std::numeric_limits<mama_u32_t>::max (), u32In);
    ret = aBridge->msgPayloadGetI64 (testPayloadIn, NULL, ++fid, &i64In);
    EXPECT_EQ (MAMA_STATUS_OK, ret);
    EXPECT_EQ (std::numeric_limits<mama_i64_t>::min (), i64In);
    ret = aBridge->msgPayloadGetU64 (testPayloadIn, NULL, ++fid, &u64In);
    EXPECT_EQ (MAMA_STATUS_OK, ret);
    EXPECT_EQ (std::numeric_limits<mama_u64_t>::max (), u64In);
    ret = aBridge->msgPayloadGetF32 (testPayloadIn, NULL, ++fid, &f32In);
    EXPECT_EQ (MAMA_STATUS_OK, ret);
    EXPECT_EQ (std::numeric_limits<mama_f32_t>::max (), f32In);
    ret = aBridge->msgPayloadGetF64 (testPayloadIn, NULL, ++fid, &f64In);
    EXPECT_EQ (MAMA_STATUS_OK, ret);
    EXPECT_EQ (std::numeric_limits<mama_f64_t>::max (), f64In);
    ret = aBridge->msgPayloadGetString (testPayloadIn, NULL, ++fid, &strIn);
    EXPECT_EQ (MAMA_STATUS_OK, ret);
    EXPECT_STREQ (str, strIn);
    ret = aBridge->msgPayloadGetOpaque (testPayloadIn, NULL, ++fid, (const void**)&bufIn, &bufInSize);
    EXPECT_EQ (MAMA_STATUS_OK, ret);
    EXPECT_EQ (0, memcmp (s.data(), bufIn, bufInSize));
    EXPECT_EQ (s.size(), bufInSize);
    ret = aBridge->msgPayloadGetDateTime (testPayload, NULL, ++fid, dtIn);
    EXPECT_EQ (MAMA_STATUS_OK, ret);
    EXPECT_EQ (1, mamaDateTime_equal (dt, dtIn));
    ret = aBridge->msgPayloadGetPrice (testPayloadIn, NULL, ++fid, prcIn);
    EXPECT_EQ (MAMA_STATUS_OK, ret);
    EXPECT_EQ (1, mamaPrice_equal (prc, prcIn));

    if (hasVectors)
    {
        const char* chVectorIn = NULL;
        mama_size_t chVectorInSize = 0;
        ret = aBridge->msgPayloadGetVectorChar (testPayloadIn, NULL, ++fid, &chVectorIn,
                                                &chVectorInSize);
        EXPECT_EQ (MAMA_STATUS_OK, ret);
        EXPECT_EQ (chVectorSize, chVectorInSize);
        for (mama_size_t k = 0; k < chVectorInSize; ++k)
        {
            EXPECT_EQ (chVector[k], chVectorIn[k]);
        }
    }

    if (hasSubMessages)
    {
        msgPayload testPayloadInSub = NULL;
        ret = aBridge->msgPayloadGetMsg (testPayloadIn, NULL, ++fid, &testPayloadInSub);
        EXPECT_EQ (MAMA_STATUS_OK, ret);

        mama_size_t numFieldsInSub = 0;
        ret = aBridge->msgPayloadGetNumFields (testPayloadInSub, &numFieldsInSub);
        EXPECT_EQ (MAMA_STATUS_OK, ret);
        EXPECT_EQ (2, numFieldsInSub);

        EXPECT_EQ (MAMA_STATUS_OK, aBridge->msgPayloadDestroy (testPayloadInSub));
    }

    EXPECT_EQ (MAMA_STATUS_OK, mamaDateTime_destroy (dt));
    EXPECT_EQ (MAMA_STATUS_OK, mamaDateTime_destroy (dtIn));
    EXPECT_EQ (MAMA_STATUS_OK, mamaPrice_destroy (prc));
    EXPECT_EQ (MAMA_STATUS_OK, mamaPrice_destroy (prcIn));

    EXPECT_EQ (MAMA_STATUS_OK, aBridge->msgPayloadDestroy (testPayload));
    EXPECT_EQ (MAMA_STATUS_OK, aBridge->msgPayloadDestroy (testPayloadIn));
    EXPECT_EQ (MAMA_STATUS_OK, aBridge->msgPayloadDestroy (subMsg));
}

/* OPTIONAL TEST:
 * Since a large proportion of middlewares don't use templates, this functionality
 * isn't required.
 */
TEST_F(PayloadGeneralTests, CreateForTemplateValid)
{
   mama_u32_t           g_templateId = 0;
   msgPayload           testPayload = NULL;
   mamaPayloadBridge    g_bridge = NULL;

   result = aBridge->msgPayloadCreateForTemplate(&testPayload, g_bridge, g_templateId);

   CHECK_NON_IMPLEMENTED_OPTIONAL(result);

   EXPECT_EQ (MAMA_STATUS_OK, result);
}

TEST_F(PayloadGeneralTests, CopyValid)
{
    msgPayload          testPayload = NULL;
    msgPayload          copyPayload = NULL;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);

    result = aBridge->msgPayloadCreate(&copyPayload);
    EXPECT_EQ (MAMA_STATUS_OK, result);

    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

    result = aBridge->msgPayloadCopy(testPayload, &copyPayload);
    EXPECT_EQ (MAMA_STATUS_OK, result);
    aBridge->msgPayloadDestroy(testPayload);
    aBridge->msgPayloadDestroy(copyPayload);
}

TEST_F(PayloadGeneralTests, CopyInValidPayload)
{
   msgPayload           copyPayload = NULL;

   result = aBridge->msgPayloadCopy(NULL, &copyPayload);
   EXPECT_EQ (MAMA_STATUS_NULL_ARG, result);
}

/* Requires discussion regarding what this test is actually expecting to check.
 * Assuming the CopyValid test requires a payload to be created first.
 */
TEST_F(PayloadGeneralTests, CopyInValidCopy)
{
   msgPayload           testPayload = NULL;
   msgPayload           copyPayload = NULL;

   result = aBridge->msgPayloadCreate(&testPayload);
   EXPECT_EQ (MAMA_STATUS_OK, result);

   aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
   aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
   aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
   aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

   result = aBridge->msgPayloadCopy(testPayload, &copyPayload);
   EXPECT_EQ (MAMA_STATUS_OK, result);
   aBridge->msgPayloadDestroy(testPayload);
   aBridge->msgPayloadDestroy(copyPayload);
}

TEST_F(PayloadGeneralTests, ClearValid)
{
   msgPayload           testPayload = NULL;

   result = aBridge->msgPayloadCreate(&testPayload);
   EXPECT_EQ (MAMA_STATUS_OK, result);

   result = aBridge->msgPayloadClear(testPayload);
   EXPECT_EQ (MAMA_STATUS_OK, result);

   aBridge->msgPayloadDestroy(testPayload);
}

TEST_F(PayloadGeneralTests, ClearInValid)
{
   result = aBridge->msgPayloadClear(NULL);
   EXPECT_EQ (MAMA_STATUS_NULL_ARG, result);
}

TEST_F(PayloadGeneralTests, DestroyValid)
{
   msgPayload          testPayload = NULL;

   result = aBridge->msgPayloadCreate(&testPayload);
   EXPECT_EQ (MAMA_STATUS_OK, result);

   result = aBridge->msgPayloadDestroy(testPayload);
   EXPECT_EQ (MAMA_STATUS_OK, result);
}

TEST_F(PayloadGeneralTests, DestroyInValid)
{
   msgPayload           testPayload = NULL;

   result = aBridge->msgPayloadCreate(&testPayload);
   EXPECT_EQ (MAMA_STATUS_OK, result);

   result = aBridge->msgPayloadDestroy(NULL);
   EXPECT_EQ (MAMA_STATUS_NULL_ARG, result);

   aBridge->msgPayloadDestroy(testPayload);
}

TEST_F(PayloadGeneralTests, SetParentValid)
{
   msgPayload          testPayload = NULL;
   mamaMsg             testParent = NULL;

   result = aBridge->msgPayloadCreate(&testPayload);
   EXPECT_EQ (MAMA_STATUS_OK, result);

   result = aBridge->msgPayloadSetParent(testPayload, testParent);
   EXPECT_EQ (MAMA_STATUS_OK, result);

   aBridge->msgPayloadDestroy(testPayload);
}

TEST_F(PayloadGeneralTests, SetParentInValidPayload)
{
   mamaMsg              testParent = NULL;

   result = aBridge->msgPayloadSetParent(NULL, testParent);
   EXPECT_EQ (MAMA_STATUS_NULL_ARG, result);
}

TEST_F(PayloadGeneralTests, SetParentInValidParent)
{
   msgPayload           testPayload = NULL;

   result = aBridge->msgPayloadSetParent(testPayload, NULL);
   EXPECT_EQ (MAMA_STATUS_NULL_ARG, result);
}

TEST_F(PayloadGeneralTests, GetByteSizeValid)
{
    msgPayload          testPayload = NULL;
    mama_size_t         testSize = 0;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);

    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

    result = aBridge->msgPayloadGetByteSize(testPayload, &testSize);

    result = aBridge->msgPayloadDestroy(testPayload);

    CHECK_NON_IMPLEMENTED_OPTIONAL(result);

    EXPECT_EQ (MAMA_STATUS_OK, result);
}

TEST_F(PayloadGeneralTests, GetByteSizeInValidPayload)
{
   mama_size_t          testSize = 0;

   result = aBridge->msgPayloadGetByteSize(NULL, &testSize);
   EXPECT_EQ (MAMA_STATUS_NULL_ARG, result);
}

TEST_F(PayloadGeneralTests, GetByteSizeInValidSize)
{
   msgPayload           testPayload = NULL;

   result = aBridge->msgPayloadGetByteSize(testPayload, NULL);
   EXPECT_EQ (MAMA_STATUS_NULL_ARG, result);
}


TEST_F(PayloadGeneralTests, GetNumberFieldsValid)
{
	mama_size_t         numFields = 0;
	msgPayload          testPayload = NULL;

	result = aBridge->msgPayloadCreate(&testPayload);
    EXPECT_EQ (MAMA_STATUS_OK, result);

	result = aBridge->msgPayloadAddChar(testPayload, NULL, 1, 'a');
	result = aBridge->msgPayloadAddChar(testPayload, NULL, 2, 'b');
	result = aBridge->msgPayloadAddChar(testPayload, NULL, 3, 'c');
	result = aBridge->msgPayloadAddChar(testPayload, NULL, 4, 'd');
	result = aBridge->msgPayloadAddChar(testPayload, NULL, 5, 'e');
	result = aBridge->msgPayloadAddChar(testPayload, NULL, 6, 'f');
	result = aBridge->msgPayloadAddChar(testPayload, NULL, 7, 'g');
	result = aBridge->msgPayloadAddChar(testPayload, NULL, 8, 'h');
	result = aBridge->msgPayloadAddChar(testPayload, NULL, 9, 'i');
    EXPECT_EQ (MAMA_STATUS_OK, result);

	result = aBridge->msgPayloadGetNumFields(testPayload, &numFields);
	EXPECT_EQ (MAMA_STATUS_OK, result);
	EXPECT_EQ ((mama_size_t)9, numFields);
	aBridge->msgPayloadDestroy(testPayload);
}

TEST_F(PayloadGeneralTests, GetNumberFieldsInValidPayload)
{
	mama_size_t          numFields = 0;

	result = aBridge->msgPayloadGetNumFields(NULL, &numFields);
	EXPECT_EQ (MAMA_STATUS_NULL_ARG, result);
}

TEST_F(PayloadGeneralTests, GetNumberFieldsInValidNumFields)
{
	msgPayload           testPayload = NULL;

	aBridge->msgPayloadCreate(&testPayload);
	aBridge->msgPayloadAddChar(testPayload, NULL, 1, 'a');

    result = aBridge->msgPayloadGetNumFields(testPayload, NULL);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, result);
    aBridge->msgPayloadDestroy(testPayload);
}

TEST_F(PayloadGeneralTests, GetSendSubjectValid)
{
    msgPayload          testPayload = NULL;
    const char*         subjectOut  = NULL;
    const char*         subjectIn   = "testsubj";
    mamaMsg             parentMsg   = NULL;

    //result = aBridge->msgPayloadCreate(&testPayload);
    mamaMsg_createForPayloadBridge(&parentMsg, aBridge);
    mamaMsgImpl_getPayload(parentMsg, &testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);

    mamaMsgImpl_setBridgeImpl (parentMsg, mMiddlewareBridge);
    mamaMsgImpl_setSubscInfo (parentMsg, NULL, NULL, subjectIn, 1);

    result = aBridge->msgPayloadGetSendSubject(testPayload, &subjectOut);
    mamaMsg_destroy(parentMsg);
    CHECK_NON_IMPLEMENTED_OPTIONAL(result);

    EXPECT_EQ (MAMA_STATUS_OK, result);

}


TEST_F(PayloadGeneralTests, GetSendSubjectInValidMsg)
{
    const char**          mySubject = NULL;

	result = aBridge->msgPayloadGetSendSubject(NULL, mySubject);

	CHECK_NON_IMPLEMENTED_OPTIONAL(result);

	EXPECT_EQ (MAMA_STATUS_NULL_ARG, result);
}

TEST_F(PayloadGeneralTests, GetSendSubjectInValidSubject)
{
    msgPayload          testPayload = NULL;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);

    result = aBridge->msgPayloadGetSendSubject(testPayload, NULL);
    aBridge->msgPayloadDestroy(testPayload);

    CHECK_NON_IMPLEMENTED_OPTIONAL(result);

    EXPECT_EQ (MAMA_STATUS_NULL_ARG, result);
}

TEST_F(PayloadGeneralTests, ToStringValid)
{
    msgPayload          testPayload = NULL;
    const char*         char_result;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);

    char_result = aBridge->msgPayloadToString(testPayload);
    EXPECT_STREQ (char_result, "{}");
    aBridge->msgPayloadDestroy(testPayload);
}

TEST_F(PayloadGeneralTests, ToStringValidConsistent)
{
	msgPayload          testPayload = NULL;
	msgPayload          testPayload2 = NULL;
	const char*         char_result;
	const char*         char_result2;

	result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);

	result = aBridge->msgPayloadAddChar(testPayload, NULL, 1, 'a');
	result = aBridge->msgPayloadAddChar(testPayload, NULL, 2, 'b');
	result = aBridge->msgPayloadAddChar(testPayload, NULL, 3, 'c');

	char_result = aBridge->msgPayloadToString(testPayload);

	result = aBridge->msgPayloadCreate(&testPayload2);
	result = aBridge->msgPayloadAddChar(testPayload2, NULL, 1, 'a');
	result = aBridge->msgPayloadAddChar(testPayload2, NULL, 2, 'b');
	result = aBridge->msgPayloadAddChar(testPayload2, NULL, 3, 'c');

    char_result2 = aBridge->msgPayloadToString(testPayload2);

    EXPECT_STREQ (char_result, char_result2);

    aBridge->msgPayloadDestroy(testPayload);
    aBridge->msgPayloadDestroy(testPayload2);
}

TEST_F(PayloadGeneralTests, ToStringInValid)
{
    msgPayload          testPayload = NULL;
    const char*         char_result;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);

	char_result = aBridge->msgPayloadToString(NULL);
	EXPECT_STREQ (char_result, NULL);
	aBridge->msgPayloadDestroy(testPayload);
}

/*
 * TODO: At present this test doesn't really check anything, and should be
 * made significantly better. Passing NULL messages, fields and callbacks
 * should all trigger a MAMA_STATUS_NULL_ARG.
 */
TEST_F(PayloadGeneralTests, IterateFieldsValid)
{
    msgPayload          testPayload   = NULL;
    msgFieldPayload     testMamaField = NULL;

    result = aBridge->msgPayloadCreate(&testPayload);
	ASSERT_EQ (MAMA_STATUS_OK, result);

	aBridge->msgFieldPayloadCreate(&testMamaField);
	ASSERT_EQ (MAMA_STATUS_OK, result);

	result = aBridge->msgPayloadIterateFields(
			testPayload,
			(mamaMsg)NOT_NULL,
			(mamaMsgField)testMamaField,
			dummyIteratorCallback,
			NULL);

	EXPECT_EQ (MAMA_STATUS_OK, result);
    aBridge->msgPayloadDestroy(testPayload);
    aBridge->msgFieldPayloadDestroy(testMamaField);
}

/* TODO: This test should return NULL, but  needs to have the parent msg, 
 * callback and mama field created before it is a valid test.
 */
TEST_F(PayloadGeneralTests, IterateFieldsInValidPayload)
{
    mamaMsg             testMamaMsg = NULL;
    mamaMsgField        testMamaField = NULL;
    mamaMsgIteratorCb   testMamaMsgIteratorCb = NULL;
    void*               testClosure;

	result = aBridge->msgPayloadIterateFields(NULL, testMamaMsg, testMamaField, testMamaMsgIteratorCb, &testClosure);
	EXPECT_EQ (MAMA_STATUS_NULL_ARG, result);
}

/* TODO: This test should return NULL, but  needs to have the callback and
 * mama field created before it is a valid test.
 */
TEST_F(PayloadGeneralTests, IterateFieldsInValidParent)
{
    msgPayload          testPayload = NULL;
    mamaMsgField        testMamaField = NULL;
    mamaMsgIteratorCb   testMamaMsgIteratorCb = NULL;
    void*               testClosure;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);

    result = aBridge->msgPayloadIterateFields(testPayload, NULL, testMamaField, testMamaMsgIteratorCb, &testClosure);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, result);
    aBridge->msgPayloadDestroy(testPayload);
}

/* TODO: This test should return NULL, but  needs to have the parent msg and 
 * callback created before it is a valid test.
 */
TEST_F(PayloadGeneralTests, IterateFieldsInValidMamaField)
{
    msgPayload          testPayload = NULL;
    mamaMsg             testMamaMsg = NULL;
    mamaMsgIteratorCb   testMamaMsgIteratorCb = NULL;
    void*               testClosure;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);

    result = aBridge->msgPayloadIterateFields(testPayload, testMamaMsg, NULL, testMamaMsgIteratorCb, &testClosure);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, result);
    aBridge->msgPayloadDestroy(testPayload);
}

/* TODO: This test should return NULL, but  needs to have the parent msg and 
 * mama field created before it is a valid test.
 */
TEST_F(PayloadGeneralTests, IterateFieldsInValidCallBack)
{
    msgPayload          testPayload = NULL;
    mamaMsg             testMamaMsg = NULL;
    mamaMsgField        testMamaField = NULL;
    void*               testClosure;

    result = aBridge->msgPayloadCreate(&testPayload);
    EXPECT_EQ (MAMA_STATUS_OK, result);

    result = aBridge->msgPayloadIterateFields(testPayload, testMamaMsg, testMamaField, NULL, &testClosure);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, result);
    aBridge->msgPayloadDestroy(testPayload);
}

TEST_F(PayloadGeneralTests, SerializeValid)
{
    msgPayload          testPayload = NULL;
    char*               testBuffer;
    mama_size_t         testBufferLength = 10;

    result = aBridge->msgPayloadCreate(&testPayload);
    EXPECT_EQ (MAMA_STATUS_OK, result);

    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

    result = aBridge->msgPayloadSerialize(testPayload, (const void**)&testBuffer, &testBufferLength);
    aBridge->msgPayloadDestroy(testPayload);
    EXPECT_EQ (MAMA_STATUS_OK, result);
}

TEST_F(PayloadGeneralTests, SerializeInValidPayLoad)
{
    msgPayload          testPayload = NULL;
    char*               testBuffer = NULL;
    mama_size_t         testBufferLength = 0;

    result = aBridge->msgPayloadCreate(&testPayload);
    EXPECT_EQ (MAMA_STATUS_OK, result);

    result = aBridge->msgPayloadSerialize(NULL, (const void**)&testBuffer, &testBufferLength);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, result);
    aBridge->msgPayloadDestroy(testPayload);
}

TEST_F(PayloadGeneralTests, SerializeInValidBuffer)
{
    msgPayload          testPayload = NULL;
    mama_size_t         testBufferLength = 10;

    result = aBridge->msgPayloadCreate(&testPayload);
    EXPECT_EQ (MAMA_STATUS_OK, result);

    aBridge->msgPayloadAddString (testPayload, "name2", 101, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 101, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 101, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 101, "Fun");

    result = aBridge->msgPayloadSerialize(testPayload, NULL, &testBufferLength);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, result);
    aBridge->msgPayloadDestroy(testPayload);
}

TEST_F(PayloadGeneralTests, SerializeInValidBufferLength)
{
    msgPayload          testPayload = NULL;
    char*               testBuffer;

    result = aBridge->msgPayloadCreate(&testPayload);
    EXPECT_EQ (MAMA_STATUS_OK, result);

    aBridge->msgPayloadAddString (testPayload, "name2", 101, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 101, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 101, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 101, "Fun");

    result = aBridge->msgPayloadSerialize(testPayload, (const void**)&testBuffer, NULL);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, result);
    result = aBridge->msgPayloadDestroy(testPayload);
}

TEST_F(PayloadGeneralTests, UnSerializeValid)
{
    msgPayload          testPayload = NULL;
    const void*         testBuffer;
    mama_size_t         testBufferLength = 50;

    result = aBridge->msgPayloadCreate(&testPayload);
    ASSERT_EQ (MAMA_STATUS_OK, result);

    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    result = aBridge->msgPayloadSerialize(testPayload, (const void**)&testBuffer, &testBufferLength);
    ASSERT_EQ (MAMA_STATUS_OK, result);

    result = aBridge->msgPayloadUnSerialize(testPayload, (const void**)testBuffer, testBufferLength);
    ASSERT_EQ (MAMA_STATUS_OK, result);
    aBridge->msgPayloadDestroy(testPayload);
}

TEST_F(PayloadGeneralTests, UnSerializeInValidPayload)
{
    msgPayload          testPayload = NULL;
    const void*         testBuffer;
    mama_size_t         testBufferLength = 10;

    result = aBridge->msgPayloadCreate(&testPayload);
    EXPECT_EQ (MAMA_STATUS_OK, result);

    result = aBridge->msgPayloadUnSerialize(NULL, &testBuffer, testBufferLength);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, result);
    aBridge->msgPayloadDestroy(testPayload);
}

TEST_F(PayloadGeneralTests, UnSerializeInValidBuffer)
{
    msgPayload          testPayload = NULL;
    mama_size_t         testBufferLength = 0;

    result = aBridge->msgPayloadCreate(&testPayload);
    EXPECT_EQ (MAMA_STATUS_OK, result);

    result = aBridge->msgPayloadUnSerialize(testPayload, NULL, testBufferLength);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, result);
    aBridge->msgPayloadDestroy(testPayload);
}

TEST_F(PayloadGeneralTests, UnSerializeInValidBufferLength)
{
    msgPayload          testPayload = NULL;
    const void*         testBuffer;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);

    result = aBridge->msgPayloadUnSerialize(testPayload, &testBuffer, 0);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, result);
    aBridge->msgPayloadDestroy(testPayload);
}

TEST_F(PayloadGeneralTests, GetByteBufferValid)
{
    msgPayload          testPayload = NULL;
    char*               testBuffer;
    mama_size_t         testBufferLength = 0;
    mamaMsg             testMamaMsg = NULL;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);

    result = aBridge->msgPayloadSetParent(testPayload, testMamaMsg);
    EXPECT_EQ (MAMA_STATUS_OK, result);

    aBridge->msgPayloadAddString (testPayload, "name2", 101, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 101, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 101, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 101, "Fun");

    result = aBridge->msgPayloadGetByteBuffer(testPayload, (const void**)&testBuffer, &testBufferLength);
    EXPECT_EQ (MAMA_STATUS_OK, result);
    aBridge->msgPayloadDestroy(testPayload);
}

TEST_F(PayloadGeneralTests, GetByteBufferInValidPayload)
{
    msgPayload          testPayload = NULL;
    char*               testBuffer;
    mama_size_t         testBufferLength = 0;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);

    aBridge->msgPayloadAddString (testPayload, "name2", 101, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 101, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 101, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 101, "Fun");

    result = aBridge->msgPayloadGetByteBuffer(NULL, (const void**)&testBuffer, &testBufferLength);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, result);
    aBridge->msgPayloadDestroy(testPayload);
}

TEST_F(PayloadGeneralTests, GetByteBufferInValidBuffer)
{
    msgPayload          testPayload = NULL;
    mama_size_t         testBufferLength = 0;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);

    aBridge->msgPayloadAddString (testPayload, "name2", 101, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 101, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 101, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 101, "Fun");

    result = aBridge->msgPayloadGetByteBuffer(testPayload, NULL, &testBufferLength);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, result);
    aBridge->msgPayloadDestroy(testPayload);
}

TEST_F(PayloadGeneralTests, GetByteBufferInValidBufferLength)
{
    msgPayload          testPayload = NULL;
    char*               testBuffer;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);

    aBridge->msgPayloadAddString (testPayload, "name2", 101, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 101, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 101, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 101, "Fun");

    result = aBridge->msgPayloadGetByteBuffer(testPayload, (const void**)&testBuffer, NULL);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, result);
    aBridge->msgPayloadDestroy(testPayload);
}

TEST_F(PayloadGeneralTests, SetByteBufferValid)
{
    msgPayload          testPayload = NULL;
    const char*         testBuffer = NULL;
    mama_size_t         testBufferLength = 100;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);

    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

	result = aBridge->msgPayloadGetByteBuffer(testPayload, (const void**)&testBuffer, &testBufferLength);
	EXPECT_EQ (MAMA_STATUS_OK, result);

	result = aBridge->msgPayloadCreateFromByteBuffer(&testPayload, aBridge, (const void*)testBuffer, testBufferLength);
	EXPECT_EQ (MAMA_STATUS_OK, result);

    result = aBridge->msgPayloadSetByteBuffer(testPayload, aBridge, (const void*)testBuffer, testBufferLength);
    EXPECT_EQ (MAMA_STATUS_OK, result);

    aBridge->msgPayloadDestroy(testPayload);
}

TEST_F(PayloadGeneralTests, SetByteBufferInValidPayload)
{
    msgPayload          testPayload = NULL;
    const char*         testBuffer = NULL;
    mama_size_t         testBufferLength = 0;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);

    result = aBridge->msgPayloadSetByteBuffer(NULL, aBridge, (const void*)testBuffer, testBufferLength);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, result);
    aBridge->msgPayloadDestroy(testPayload);
}

TEST_F(PayloadGeneralTests, SetByteBufferInValidBridge)
{
    msgPayload          testPayload = NULL;
    const char*         testBuffer = NULL;
    mama_size_t         testBufferLength = 0;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);

    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

    result = aBridge->msgPayloadSetByteBuffer(testPayload, NULL, (const void*)testBuffer, testBufferLength);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, result);

    aBridge->msgPayloadDestroy(testPayload);
}

TEST_F(PayloadGeneralTests, SetByteBufferInValidBuffer)
{
    msgPayload          testPayload = NULL;
    mama_size_t         testBufferLength = 0;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);

    result = aBridge->msgPayloadSetByteBuffer(testPayload, aBridge, NULL, testBufferLength);
	EXPECT_EQ (MAMA_STATUS_NULL_ARG, result);
	aBridge->msgPayloadDestroy(testPayload);
}

TEST_F(PayloadGeneralTests, SetByteBufferInValidBufferLength)
{
    msgPayload          testPayload = NULL;
    const char*         testBuffer = NULL;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);

    aBridge->msgPayloadAddString (testPayload, "name2", 101, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 101, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 101, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 101, "Fun");

    result = aBridge->msgPayloadSetByteBuffer(testPayload, aBridge, (const void*)testBuffer, 0);
	EXPECT_EQ (MAMA_STATUS_NULL_ARG, result);
	aBridge->msgPayloadDestroy(testPayload);
}

/* TODO: Check the values in the created payload.
 */
TEST_F(PayloadGeneralTests, CreateFromByteBufferValid)
{
    msgPayload          testPayload = NULL;
    msgPayload          testPayloadNew = NULL;
    const void*         testBuffer = NULL;
    mama_size_t         testBufferLength = 0;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);

    aBridge->msgPayloadAddString (testPayload, "name2", 101, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 101, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 101, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 101, "Fun");

	result = aBridge->msgPayloadGetByteBuffer(testPayload, &testBuffer, &testBufferLength);
	EXPECT_EQ (MAMA_STATUS_OK, result);

	result = aBridge->msgPayloadCreateFromByteBuffer(&testPayloadNew, aBridge, testBuffer, testBufferLength);
	EXPECT_EQ (MAMA_STATUS_OK, result);

    //aBridge->msgPayloadDestroy(testPayloadNew);
    aBridge->msgPayloadDestroy(testPayload);

}

TEST_F(PayloadGeneralTests, CreateFromByteBufferInValidPayLoad)
{
    msgPayload          testPayload = NULL;
    char*               testBuffer;
    mama_size_t         testBufferLength = 0;

    result = aBridge->msgPayloadCreate(&testPayload);
    EXPECT_EQ (MAMA_STATUS_OK, result);

	result = aBridge->msgPayloadGetByteBuffer(testPayload, (const void**)&testBuffer, &testBufferLength);
	EXPECT_EQ (MAMA_STATUS_OK, result);

	result = aBridge->msgPayloadCreateFromByteBuffer(NULL, aBridge, (const void*)testBuffer, testBufferLength);
	EXPECT_EQ (MAMA_STATUS_NULL_ARG, result);

    aBridge->msgPayloadDestroy(testPayload);
}

TEST_F(PayloadGeneralTests, CreateFromByteBufferInValidBridge)
{
    msgPayload          testPayload = NULL;
    msgPayload          testPayloadNew = NULL;
    char*               testBuffer;
    mama_size_t         testBufferLength = 0;

    result = aBridge->msgPayloadCreate(&testPayload);
    result = aBridge->msgPayloadCreate(&testPayloadNew);
	EXPECT_EQ (MAMA_STATUS_OK, result);

    aBridge->msgPayloadAddString (testPayload, "name2", 101, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 101, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 101, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 101, "Fun");

	result = aBridge->msgPayloadGetByteBuffer(testPayload, (const void**)&testBuffer, &testBufferLength);
	EXPECT_EQ (MAMA_STATUS_OK, result);

	result = aBridge->msgPayloadCreateFromByteBuffer(&testPayloadNew, NULL, (const void*)testBuffer, testBufferLength);
	EXPECT_EQ (MAMA_STATUS_OK, result);

    aBridge->msgPayloadDestroy(testPayload);
}

TEST_F(PayloadGeneralTests, CreateFromByteBufferInValidBuffer)
{
    msgPayload          testPayload = NULL;
    msgPayload          testPayloadNew = NULL;
    char*               testBuffer;
    mama_size_t         testBufferLength = 0;

    result = aBridge->msgPayloadCreate(&testPayload);
    result = aBridge->msgPayloadCreate(&testPayloadNew);
	EXPECT_EQ (MAMA_STATUS_OK, result);

    aBridge->msgPayloadAddString (testPayload, "name2", 101, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 101, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 101, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 101, "Fun");

	result = aBridge->msgPayloadGetByteBuffer(testPayload, (const void**)&testBuffer, &testBufferLength);
	EXPECT_EQ (MAMA_STATUS_OK, result);

    result = aBridge->msgPayloadCreateFromByteBuffer(&testPayloadNew, aBridge, NULL, testBufferLength);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, result);

    aBridge->msgPayloadDestroy(testPayload);
}

TEST_F(PayloadGeneralTests, CreateFromByteBufferInValidBufferLength)
{
    msgPayload          testPayload = NULL;
    msgPayload          testPayloadNew = NULL;
    char*               testBuffer;
    mama_size_t         testBufferLength = 0;

    result = aBridge->msgPayloadCreate(&testPayload);
    result = aBridge->msgPayloadCreate(&testPayloadNew);
	EXPECT_EQ (MAMA_STATUS_OK, result);

    aBridge->msgPayloadAddString (testPayload, "name2", 101, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 101, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 101, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 101, "Fun");

	result = aBridge->msgPayloadGetByteBuffer(testPayload, (const void**)&testBuffer, &testBufferLength);
	EXPECT_EQ (MAMA_STATUS_OK, result);

    result = aBridge->msgPayloadCreateFromByteBuffer(&testPayloadNew, aBridge, (const void*)testBuffer, 0);
    EXPECT_EQ (MAMA_STATUS_INVALID_ARG, result);

    aBridge->msgPayloadDestroy(testPayload);
}

TEST_F(PayloadGeneralTests, ApplyValid)
{
    msgPayload          testDestPayload = NULL;
    msgPayload          testSrcPayload = NULL;

    result = aBridge->msgPayloadCreate(&testDestPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);

    result = aBridge->msgPayloadCreate(&testSrcPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);

	result = aBridge->msgPayloadApply(testDestPayload, testSrcPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);

    aBridge->msgPayloadAddString (testSrcPayload, "name", 0, "Unit");

	result = aBridge->msgPayloadApply(testDestPayload, testSrcPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);

    aBridge->msgPayloadAddString (testSrcPayload, NULL, 101, "Testing");

    result = aBridge->msgPayloadApply(testDestPayload, testSrcPayload);
    EXPECT_EQ (MAMA_STATUS_OK, result);

    aBridge->msgPayloadDestroy(testSrcPayload);
    aBridge->msgPayloadDestroy(testDestPayload);
}

TEST_F(PayloadGeneralTests, ApplyInValidDestPayload)
{
    msgPayload          testSrcPayload = NULL;
    mama_size_t         numFields = 0;

    result = aBridge->msgPayloadCreate(&testSrcPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);

	result = aBridge->msgPayloadAddChar(testSrcPayload, NULL, 9, 'i');
    EXPECT_EQ (MAMA_STATUS_OK, result);

	result = aBridge->msgPayloadGetNumFields(testSrcPayload, &numFields);
	EXPECT_EQ (MAMA_STATUS_OK, result);
	EXPECT_EQ ((mama_size_t) 1, numFields);

    result = aBridge->msgPayloadApply(NULL, testSrcPayload);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, result);

    aBridge->msgPayloadDestroy(testSrcPayload);
}

TEST_F(PayloadGeneralTests, ApplyInValidSrcPayload)
{
    msgPayload          testDestPayload = NULL;
    mama_size_t         numFields = 0;

    result = aBridge->msgPayloadCreate(&testDestPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);

	result = aBridge->msgPayloadAddChar(testDestPayload, NULL, 9, 'i');
    EXPECT_EQ (MAMA_STATUS_OK, result);

	result = aBridge->msgPayloadGetNumFields(testDestPayload, &numFields);
	EXPECT_EQ (MAMA_STATUS_OK, result);
	EXPECT_EQ ((mama_size_t) 1, numFields);

    result = aBridge->msgPayloadApply(testDestPayload, NULL);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, result);

    aBridge->msgPayloadDestroy(testDestPayload);
}

TEST_F(PayloadGeneralTests, GetNativeMsgValid)
{
    msgPayload          testPayload = NULL;
    char*               testNativeMsg;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);
    
    result = aBridge->msgPayloadGetNativeMsg(testPayload, (void**)&testNativeMsg);
    EXPECT_EQ (MAMA_STATUS_OK, result);

    aBridge->msgPayloadDestroy(testPayload);
}

TEST_F(PayloadGeneralTests, GetNativeMsgInValidMsg)
{
    msgPayload          testPayload = NULL;
    char*               testNativeMsg;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);
    
    result = aBridge->msgPayloadGetNativeMsg(NULL, (void**)&testNativeMsg);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, result);

    aBridge->msgPayloadDestroy(testPayload);
}

TEST_F(PayloadGeneralTests, GetNativeMsgInValidNativeMsg)
{
    msgPayload          testPayload = NULL;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);
    
    result = aBridge->msgPayloadGetNativeMsg(testPayload, NULL);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, result);

    aBridge->msgPayloadDestroy(testPayload);
}

TEST_F(PayloadGeneralTests, GetFieldAsStringValid)
{
    msgPayload          testPayload = NULL;
    char                testName;
    mama_fid_t          testFid = 105;
    char                testBuf[256];
    mama_size_t         testLen = 10;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);
    
    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");
    
    result = aBridge->msgPayloadGetFieldAsString(testPayload, &testName, testFid, testBuf, testLen);
    EXPECT_EQ (MAMA_STATUS_OK, result);

    aBridge->msgPayloadDestroy(testPayload);
}

TEST_F(PayloadGeneralTests, GetFieldAsStringInValidPayload)
{
    msgPayload          testPayload = NULL;
    char                testName;
    mama_fid_t          testFid = 0;
    char                testBuf;
    mama_size_t         testLen = 0;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);
    
    result = aBridge->msgPayloadGetFieldAsString(NULL, &testName, testFid, &testBuf, testLen);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, result);

    aBridge->msgPayloadDestroy(testPayload);
}

TEST_F(PayloadGeneralTests, GetFieldAsStringInValidName)
{
    msgPayload          testPayload = NULL;
    mama_fid_t          testFid = 105;
    char                testBuf[256];
    mama_size_t         testLen = 10;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);
    
    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");
    
    result = aBridge->msgPayloadGetFieldAsString(testPayload, NULL, testFid, testBuf, testLen);
    EXPECT_EQ (MAMA_STATUS_OK, result);

    aBridge->msgPayloadDestroy(testPayload);
}

TEST_F(PayloadGeneralTests, GetFieldAsStringInValidFid)
{
    /*
     * 0 is a valid fid so this test is  not necessary
    msgPayload          testPayload = NULL;
    char                testName;
    mama_fid_t          testFid = 105;
    char                testBuf;
    mama_size_t         testLen = 10;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);
    
    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");
    
    result = aBridge->msgPayloadGetFieldAsString(testPayload, &testName, 0, &testBuf, testLen);
	EXPECT_EQ (MAMA_STATUS_OK, result);
    */
}

TEST_F(PayloadGeneralTests, GetFieldAsStringInValidBuffer)
{
    msgPayload          testPayload = NULL;
    char                testName;
    mama_fid_t          testFid = 105;
    mama_size_t         testLen = 10;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);

    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");
    
    result = aBridge->msgPayloadGetFieldAsString(testPayload, &testName, testFid, NULL, testLen);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, result);
    aBridge->msgPayloadDestroy(testPayload);
}

TEST_F(PayloadGeneralTests, GetFieldAsStringInValidBufferLength)
{
    msgPayload          testPayload = NULL;
    char                testName;
    mama_fid_t          testFid = 105;
    char                testBuf[256];

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);
    
    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

    result = aBridge->msgPayloadGetFieldAsString(testPayload, &testName, testFid, testBuf, 0);
    EXPECT_EQ (MAMA_STATUS_INVALID_ARG, result);

    aBridge->msgPayloadDestroy(testPayload);
}

TEST_F(PayloadGeneralTests, GetFieldValid)
{
    msgPayload          testPayload = NULL;
    char                testName;
    mama_fid_t          testFid = 105;
    msgFieldPayload     testResult = NULL;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);
    
    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

    result = aBridge->msgPayloadGetField(testPayload, &testName, testFid, &testResult);
    EXPECT_EQ (MAMA_STATUS_OK, result);

    aBridge->msgPayloadDestroy(testPayload);
}

TEST_F(PayloadGeneralTests, GetFieldInValidPayload)
{
    msgPayload          testPayload = NULL;
    char                testName;
    mama_fid_t          testFid = 0;
    msgFieldPayload     testResult = NULL;

    result = aBridge->msgPayloadCreate(&testPayload);
    EXPECT_EQ (MAMA_STATUS_OK, result);

    result = aBridge->msgPayloadGetField(NULL, &testName, testFid, &testResult);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, result);

    aBridge->msgPayloadDestroy(testPayload);
}

TEST_F(PayloadGeneralTests, GetFieldInValidName)
{
    msgPayload          testPayload = NULL;
    mama_fid_t          testFid = 105;
    msgFieldPayload     testResult = NULL;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);
    
    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

    result = aBridge->msgPayloadGetField(testPayload, NULL, testFid, &testResult);
    EXPECT_EQ (MAMA_STATUS_OK, result);

    aBridge->msgPayloadDestroy(testPayload);
}

TEST_F(PayloadGeneralTests, GetFieldInValidFid)
{
    /*
     * 0 is a valid FID so this test is not necessary
    msgPayload          testPayload = NULL;
    char                testName;
    msgFieldPayload     testResult = NULL;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);
    
    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

    result = aBridge->msgFieldPayloadCreate(&testResult);
    EXPECT_EQ (MAMA_STATUS_OK, result);

    result = aBridge->msgPayloadGetField(testPayload, &testName, 0, &testResult);
	EXPECT_EQ (MAMA_STATUS_OK, result);
    */
}

TEST_F(PayloadGeneralTests, GetFieldInValidResult)
{
    msgPayload          testPayload = NULL;
    char                testName;
    mama_fid_t          testFid = 105;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);
    
    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

    result = aBridge->msgPayloadGetField(testPayload, &testName, testFid, NULL);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, result);
    aBridge->msgPayloadDestroy(testPayload);
}

/* ************************************************************************* */
/* Payload Field General Tests */
/* ************************************************************************* */

TEST_F(PayloadGeneralTests, IterCreateValid)
{
    msgPayload          testPayload = NULL;
    msgPayloadIter      testIter = NULL;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);
    
    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

    result = aBridge->msgPayloadIterCreate(&testIter, testPayload);
    EXPECT_EQ (MAMA_STATUS_OK, result);

    aBridge->msgPayloadIterDestroy(testIter);
    aBridge->msgPayloadDestroy(testPayload);
}

TEST_F(PayloadGeneralTests, IterCreateInValidtestIter)
{
    msgPayload          testPayload = NULL;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);
    
    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

    result = aBridge->msgPayloadIterCreate(NULL, testPayload);
	EXPECT_EQ (MAMA_STATUS_NULL_ARG, result);

    aBridge->msgPayloadDestroy(testPayload);
}

TEST_F(PayloadGeneralTests, IterCreateInValidPayload)
{
    msgPayloadIter      testIter = NULL;

    result = aBridge->msgPayloadIterCreate(&testIter, NULL);
	EXPECT_EQ (MAMA_STATUS_NULL_ARG, result);
}

TEST_F(PayloadGeneralTests, IterDestroyValid)
{
    msgPayloadIter      testIter = NULL;
    msgFieldPayload     testPayload = NULL;

    result = aBridge->msgPayloadCreate(&testPayload);
    EXPECT_EQ (MAMA_STATUS_OK, result);

    result = aBridge->msgPayloadIterCreate(&testIter, testPayload);

    result = aBridge->msgPayloadIterDestroy(testIter);
    EXPECT_EQ (MAMA_STATUS_OK, result);

    aBridge->msgPayloadDestroy(testPayload);
}

TEST_F(PayloadGeneralTests, IterDestroyInValidIter)
{
    result = aBridge->msgPayloadIterDestroy(NULL);
	EXPECT_EQ (MAMA_STATUS_NULL_ARG, result);
}

TEST_F(PayloadGeneralTests, IterEndValid)
{
    msgPayload          testMsg = NULL;
    msgPayloadIter      testIter = NULL;
    msgFieldPayload     output = NULL;

    output = aBridge->msgPayloadIterEnd(&testIter, testMsg);

    if (output != NULL)
        FAIL();
}

TEST_F(PayloadGeneralTests, IterEndInValidIter)
{
    msgPayload          testMsg = NULL;
    msgFieldPayload     output = NULL;

    output = aBridge->msgPayloadIterEnd(NULL, testMsg);

    if (output != NULL)
        FAIL();
}

TEST_F(PayloadGeneralTests, IterEndInValidMsg)
{
    msgPayloadIter      testIter = NULL;
    msgFieldPayload     output = NULL;

    output = aBridge->msgPayloadIterEnd(&testIter, NULL);

    if (output != NULL)
        FAIL();
}

TEST_F(PayloadGeneralTests, IterNextValid)
{
    msgPayload          testPayload = NULL;
    msgPayloadIter      testIter = NULL;
    msgFieldPayload     testField = NULL;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);
    
    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

    result = aBridge->msgPayloadIterCreate(&testIter, testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);

    if (NULL == aBridge->msgPayloadIterNext(testIter, testField, testPayload))
        FAIL();

    if (NULL == aBridge->msgPayloadIterNext(testIter, testField, testPayload))
        FAIL();

    if (NULL == aBridge->msgPayloadIterNext(testIter, testField, testPayload))
        FAIL();

    if (NULL == aBridge->msgPayloadIterNext(testIter, testField, testPayload))
        FAIL();

    aBridge->msgPayloadIterDestroy(testIter);
    aBridge->msgPayloadDestroy(testPayload);
}

TEST_F(PayloadGeneralTests, IterNextValidConsistent)
{
    msgPayload          testPayload = NULL;
    msgPayloadIter      testIter = NULL;
    msgFieldPayload     testField = NULL;

    result = aBridge->msgPayloadCreate(&testPayload);
    EXPECT_EQ (MAMA_STATUS_OK, result);

    /* A reference group of fids. The same fids as in the payload testPayload. 
     * See later on.
     */
    std::set<mama_fid_t> reference_fids_set;
    reference_fids_set.insert(102);
    reference_fids_set.insert(103);
    reference_fids_set.insert(104);
    reference_fids_set.insert(105);

    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

    result = aBridge->msgPayloadIterCreate(&testIter, testPayload);
    EXPECT_EQ (MAMA_STATUS_OK, result);

    mama_fid_t test_fid;
    std::set<mama_fid_t> returned_fids_set;
    if (NULL == (testField = aBridge->msgPayloadIterNext (testIter,
                                                          testField,
                                                          testPayload)))
        FAIL();

    aBridge->msgFieldPayloadGetFid (testField, NULL, NULL, &test_fid);
    returned_fids_set.insert(test_fid);

    if (NULL == (testField = aBridge->msgPayloadIterNext (testIter,
                                                          testField,
                                                          testPayload)))
        FAIL();

    aBridge->msgFieldPayloadGetFid (testField, NULL, NULL, &test_fid);
    returned_fids_set.insert(test_fid);

    if (NULL == (testField = aBridge->msgPayloadIterNext (testIter,
                                                          testField,
                                                          testPayload)))
        FAIL();

    aBridge->msgFieldPayloadGetFid (testField, NULL, NULL, &test_fid);
    returned_fids_set.insert(test_fid);

    if (NULL == (testField = aBridge->msgPayloadIterNext (testIter,
                                                          testField,
                                                          testPayload)))
        FAIL();

    aBridge->msgFieldPayloadGetFid (testField, NULL, NULL, &test_fid);
    returned_fids_set.insert(test_fid);

    /* Check that all fids that are returned by each iterated field are the 
     * same as the reference set (consistency check)
     */
    if (returned_fids_set != reference_fids_set)
        FAIL();

    /* Check that the last call to msgPayloadIterNext brings NULL
     * (completeness check)
     */
    if (NULL != (testField = aBridge->msgPayloadIterNext (testIter,
                                                          testField,
                                                          testPayload)))
        FAIL();

    aBridge->msgPayloadIterDestroy(testIter);
    aBridge->msgPayloadDestroy(testPayload);
}

TEST_F(PayloadGeneralTests, IterNextInValidIter)
{
    msgPayload          testPayload = NULL;
    msgFieldPayload     testField = NULL;
    msgFieldPayload     output = NULL;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);
    
    result = aBridge->msgFieldPayloadCreate(&testField);
    EXPECT_EQ (MAMA_STATUS_OK, result);

    output = aBridge->msgPayloadIterNext(NULL, testField, testPayload);

    aBridge->msgFieldPayloadDestroy(testField);
    aBridge->msgPayloadDestroy(testPayload);
    if (output != NULL)
        FAIL();
}

TEST_F(PayloadGeneralTests, IterNextInValidField)
{
    msgPayload          testPayload = NULL;
    msgPayloadIter      testIter = NULL;
    msgFieldPayload     output = NULL;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);
    
    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

    result = aBridge->msgPayloadIterCreate(&testIter, testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);

    output = aBridge->msgPayloadIterNext(testIter, NULL, testPayload);

    aBridge->msgPayloadIterDestroy(testIter);
    aBridge->msgPayloadDestroy(testPayload);
    if (output == NULL)
        FAIL();
}

TEST_F(PayloadGeneralTests, IterNextInValidPayload)
{
    msgPayload          testPayload = NULL;
    msgPayloadIter      testIter = NULL;
    msgFieldPayload     testField = NULL;
    msgFieldPayload     output = NULL;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);
    
    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

    result = aBridge->msgFieldPayloadCreate(&testField);
    EXPECT_EQ (MAMA_STATUS_OK, result);

    result = aBridge->msgPayloadIterCreate(&testIter, testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);

    output = aBridge->msgPayloadIterNext(testIter, testField, NULL);

    aBridge->msgPayloadIterDestroy(testIter);
    aBridge->msgFieldPayloadDestroy(testField);
    aBridge->msgPayloadDestroy(testPayload);
    if (output != NULL)
        FAIL();
}

TEST_F(PayloadGeneralTests, IterBeginValid)
{
    msgPayload          testPayload = NULL;
    msgPayloadIter      testIter = NULL;
    msgFieldPayload     testField = NULL;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);
    
    //Call on empty msg first
    aBridge->msgPayloadIterBegin(testIter, testField, testPayload);

    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

    result = aBridge->msgFieldPayloadCreate(&testField);
    EXPECT_EQ (MAMA_STATUS_OK, result);

    result = aBridge->msgPayloadIterCreate(&testIter, testPayload);
    EXPECT_EQ (MAMA_STATUS_OK, result);

    if (NULL == aBridge->msgPayloadIterBegin(testIter, testField, testPayload))
        FAIL();
    aBridge->msgPayloadIterDestroy(testIter);
    aBridge->msgFieldPayloadDestroy(testField);
    aBridge->msgPayloadDestroy(testPayload);
}

TEST_F(PayloadGeneralTests, IterBeginInValidIter)
{
    msgPayload          testPayload = NULL;
    msgPayloadIter      testIter = NULL;
    msgFieldPayload     testField = NULL;
    msgFieldPayload     output = NULL;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);
    
    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

    result = aBridge->msgFieldPayloadCreate(&testField);
    EXPECT_EQ (MAMA_STATUS_OK, result);

    result = aBridge->msgPayloadIterCreate(&testIter, testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);

    output = aBridge->msgPayloadIterBegin(NULL, testField, testPayload);
    aBridge->msgPayloadIterDestroy(testIter);
    aBridge->msgFieldPayloadDestroy(testField);
    aBridge->msgPayloadDestroy(testPayload);
    if (output != NULL)
        FAIL();
}

TEST_F(PayloadGeneralTests, IterBeginInValidField)
{
    msgPayload          testPayload = NULL;
    msgPayloadIter      testIter = NULL;
    msgFieldPayload     testField = NULL;
    msgFieldPayload     output = NULL;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);
    
    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

    result = aBridge->msgFieldPayloadCreate(&testField);
    EXPECT_EQ (MAMA_STATUS_OK, result);

    result = aBridge->msgPayloadIterCreate(&testIter, testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);

    output = aBridge->msgPayloadIterBegin(testIter, NULL, testPayload);
    aBridge->msgPayloadIterDestroy(testIter);
    aBridge->msgFieldPayloadDestroy(testField);
    aBridge->msgPayloadDestroy(testPayload);
    if (output == NULL)
        FAIL();
}

TEST_F(PayloadGeneralTests, IterBeginInValidPayload)
{
    msgPayload          testPayload = NULL;
    msgPayloadIter      testIter = NULL;
    msgFieldPayload     testField = NULL;
    msgFieldPayload     output = NULL;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);
    
    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

    result = aBridge->msgFieldPayloadCreate(&testField);
    EXPECT_EQ (MAMA_STATUS_OK, result);

    result = aBridge->msgPayloadIterCreate(&testIter, testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);

    output = aBridge->msgPayloadIterBegin(testIter, testField, NULL);

    aBridge->msgPayloadIterDestroy(testIter);
    aBridge->msgFieldPayloadDestroy(testField);
    aBridge->msgPayloadDestroy(testPayload);
    if (output != NULL)
        FAIL();
}

TEST_F(PayloadGeneralTests, IterHasNextValid)
{
    msgPayload          testPayload = NULL;
    msgPayloadIter      testIter = NULL;
    mama_bool_t         output = NULL;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);
    
    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

    result = aBridge->msgPayloadIterCreate(&testIter, testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);

    output = aBridge->msgPayloadIterHasNext(testIter, testPayload);
    EXPECT_EQ (true, output);
    aBridge->msgPayloadIterDestroy(testIter);
    aBridge->msgPayloadDestroy(testPayload);
}

TEST_F(PayloadGeneralTests, IterHasNextInValidIter)
{
    msgPayload          testPayload = NULL;
    mama_bool_t         output = NULL;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);
    
    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

    output = aBridge->msgPayloadIterHasNext(NULL, testPayload);
    EXPECT_EQ (false, output);
    aBridge->msgPayloadDestroy(testPayload);
}

TEST_F(PayloadGeneralTests, IterHasNextInValidPayload)
{
    msgPayload          testPayload = NULL;
    msgPayloadIter      testIter = NULL;
    mama_bool_t         output = NULL;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);
    
    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

    result = aBridge->msgPayloadIterCreate(&testIter, testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);

    output = aBridge->msgPayloadIterHasNext(testIter, NULL);
    EXPECT_EQ (0, output);
    aBridge->msgPayloadIterDestroy(testIter);
    aBridge->msgPayloadDestroy(testPayload);
}

TEST_F(PayloadGeneralTests, IterAssociateValid)
{
    msgPayload          testPayload = NULL;
    msgPayloadIter      testIter = NULL;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);
    
    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

    result = aBridge->msgPayloadIterCreate(&testIter, testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);

    result = aBridge->msgPayloadIterAssociate(testIter, testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);

    result = aBridge->msgPayloadIterAssociate(testIter, testPayload);
    EXPECT_EQ (MAMA_STATUS_OK, result);
    aBridge->msgPayloadIterDestroy(testIter);
    aBridge->msgPayloadDestroy(testPayload);
}

TEST_F(PayloadGeneralTests, IterAssociateTwiceValid)
{
    msgPayload           testPayload = NULL;
    msgPayload           testPayload2 = NULL;
    msgPayloadIter       testIter = NULL;
    msgFieldPayload      testField = NULL;
    mama_fid_t           test_fid = 0;
    std::set<mama_fid_t> pl1_fids;
    std::set<mama_fid_t> pl2_fids;

    result = aBridge->msgPayloadCreate(&testPayload);
    EXPECT_EQ (MAMA_STATUS_OK, result);

    // Create 2 payloads
    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    pl1_fids.insert(102);
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    pl1_fids.insert(103);
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    pl1_fids.insert(104);
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");
    pl1_fids.insert(105);

    result = aBridge->msgPayloadCreate(&testPayload2);
    EXPECT_EQ (MAMA_STATUS_OK, result);

    aBridge->msgPayloadAddString (testPayload2, "name2", 202, "Repeating");
    pl2_fids.insert(202);
    aBridge->msgPayloadAddString (testPayload2, "name3", 203, "Things");
    pl2_fids.insert(203);
    aBridge->msgPayloadAddString (testPayload2, "name4", 204, "Is");
    pl2_fids.insert(204);
    aBridge->msgPayloadAddString (testPayload2, "name5", 205, "Great");
    pl2_fids.insert(205);

    // Create iterator 
    result = aBridge->msgPayloadIterCreate(&testIter, testPayload);
    EXPECT_EQ (MAMA_STATUS_OK, result);

    // associate iterator with a payload
    result = aBridge->msgPayloadIterAssociate(testIter, testPayload);
    EXPECT_EQ (MAMA_STATUS_OK, result);

    // Test current payload exist and iterated correctly
    result = aBridge->msgPayloadGetField(testPayload,NULL,102,&testField); 
    EXPECT_EQ (MAMA_STATUS_OK, result);

    testField = aBridge->msgPayloadIterNext(testIter,testField,testPayload);
     // Make sure we're traversing on a payload
    testField = aBridge->msgPayloadIterNext(testIter,testField,testPayload);

    result = aBridge->msgFieldPayloadGetFid(testField,NULL,NULL, &test_fid);
    EXPECT_NE (pl1_fids.find(test_fid), pl1_fids.end());

    // reuse iterator with new payload
    result = aBridge->msgPayloadIterAssociate(testIter, testPayload2);
    EXPECT_EQ (MAMA_STATUS_OK, result);

    // Test current payload exists and iterated correctly
    result = aBridge->msgPayloadGetField(testPayload2,NULL,202,&testField); 
    EXPECT_EQ (MAMA_STATUS_OK, result);

    testField = aBridge->msgPayloadIterNext(testIter,testField,testPayload2);
    // Make sure we're traversing on the 2nd payload
    testField = aBridge->msgPayloadIterNext(testIter,testField,testPayload2); 

    result = aBridge->msgFieldPayloadGetFid(testField,NULL,NULL, &test_fid);
    EXPECT_NE (pl2_fids.find(test_fid), pl2_fids.end());

    result = aBridge->msgPayloadIterDestroy(testIter);
    EXPECT_EQ (MAMA_STATUS_OK, result);

    result = aBridge->msgPayloadDestroy(testPayload);
    EXPECT_EQ (MAMA_STATUS_OK, result);

    result = aBridge->msgPayloadDestroy(testPayload2);
    EXPECT_EQ (MAMA_STATUS_OK, result);
}

TEST_F(PayloadGeneralTests, IterAssociateInValidIter)
{
    msgPayload          testPayload = NULL;
    msgPayloadIter      testIter = NULL;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);
    
    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

    result = aBridge->msgPayloadIterCreate(&testIter, testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);

    result = aBridge->msgPayloadIterAssociate(NULL, testPayload);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, result);
    aBridge->msgPayloadIterDestroy(testIter);
    aBridge->msgPayloadDestroy(testPayload);
}

TEST_F(PayloadGeneralTests, IterAssociateInValidPayload)
{
    msgPayload          testPayload = NULL;
    msgPayloadIter      testIter = NULL;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (MAMA_STATUS_OK, result);
    
    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

    result = aBridge->msgPayloadIterCreate(&testIter, testPayload);
    EXPECT_EQ (MAMA_STATUS_OK, result);

    result = aBridge->msgPayloadIterAssociate(testIter, NULL);
    EXPECT_EQ (MAMA_STATUS_NULL_ARG, result);

    aBridge->msgPayloadIterDestroy(testIter);
    aBridge->msgPayloadDestroy(testPayload);
}

