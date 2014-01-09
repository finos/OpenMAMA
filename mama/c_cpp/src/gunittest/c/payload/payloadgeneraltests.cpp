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
    mama_status         result;
};

PayloadGeneralTests::PayloadGeneralTests(void)
    : aBridge (NULL)
    , result (MAMA_STATUS_OK)
{
}

PayloadGeneralTests::~PayloadGeneralTests(void)
{
}

void PayloadGeneralTests::SetUp(void)
{
	mama_loadPayloadBridge (&aBridge,getPayload());
}

void PayloadGeneralTests::TearDown(void)
{
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

   EXPECT_EQ(result, MAMA_STATUS_OK);
}

TEST_F(PayloadGeneralTests, CreateInValid)
{
   result = aBridge->msgPayloadCreate(NULL);

   EXPECT_EQ(result, MAMA_STATUS_NULL_ARG);
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

   EXPECT_EQ(result, MAMA_STATUS_OK);
}

TEST_F(PayloadGeneralTests, GetTypeValid)
{
   mamaPayloadType     result;

   result = aBridge->msgPayloadGetType();
   EXPECT_NE(result, MAMA_PAYLOAD_UNKNOWN);
}

TEST_F(PayloadGeneralTests, CopyValid)
{
    msgPayload          testPayload = NULL;
    msgPayload          copyPayload = NULL;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);

    result = aBridge->msgPayloadCreate(&copyPayload);
    EXPECT_EQ (result, MAMA_STATUS_OK);

    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

    result = aBridge->msgPayloadCopy(testPayload, &copyPayload);
    EXPECT_EQ(result, MAMA_STATUS_OK);
}

TEST_F(PayloadGeneralTests, CopyInValidPayload)
{
   msgPayload           copyPayload = NULL;

   result = aBridge->msgPayloadCopy(NULL, &copyPayload);
   EXPECT_EQ(result, MAMA_STATUS_NULL_ARG);
}

/* Requires discussion regarding what this test is actually expecting to check.
 * Assuming the CopyValid test requires a payload to be created first.
 */
TEST_F(PayloadGeneralTests, CopyInValidCopy)
{
   msgPayload           testPayload = NULL;
   msgPayload           copyPayload = NULL;

   result = aBridge->msgPayloadCreate(&testPayload);
   EXPECT_EQ(result, MAMA_STATUS_OK);

   aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
   aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
   aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
   aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

   result = aBridge->msgPayloadCopy(testPayload, &copyPayload);
   EXPECT_EQ(result, MAMA_STATUS_OK);
}

TEST_F(PayloadGeneralTests, ClearValid)
{
   msgPayload           testPayload = NULL;

   result = aBridge->msgPayloadCreate(&testPayload);
   EXPECT_EQ(result, MAMA_STATUS_OK);

   result = aBridge->msgPayloadClear(testPayload);
   EXPECT_EQ(result, MAMA_STATUS_OK);
}

TEST_F(PayloadGeneralTests, ClearInValid)
{
   result = aBridge->msgPayloadClear(NULL);
   EXPECT_EQ(result, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadGeneralTests, DestroyValid)
{
   msgPayload          testPayload = NULL;

   result = aBridge->msgPayloadCreate(&testPayload);
   EXPECT_EQ(result, MAMA_STATUS_OK);

   result = aBridge->msgPayloadDestroy(testPayload);
   EXPECT_EQ(result, MAMA_STATUS_OK);
}

TEST_F(PayloadGeneralTests, DestroyInValid)
{
   msgPayload           testPayload = NULL;

   result = aBridge->msgPayloadCreate(&testPayload);
   EXPECT_EQ(result, MAMA_STATUS_OK);

   result = aBridge->msgPayloadDestroy(NULL);
   EXPECT_EQ(result, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadGeneralTests, SetParentValid)
{
   msgPayload          testPayload = NULL;
   mamaMsg             testParent = NULL;

   result = aBridge->msgPayloadCreate(&testPayload);
   EXPECT_EQ(result, MAMA_STATUS_OK);

   result = aBridge->msgPayloadSetParent(testPayload, testParent);
   EXPECT_EQ(result, MAMA_STATUS_OK);
}

TEST_F(PayloadGeneralTests, SetParentInValidPayload)
{
   mamaMsg              testParent = NULL;

   result = aBridge->msgPayloadSetParent(NULL, testParent);
   EXPECT_EQ(result, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadGeneralTests, SetParentInValidParent)
{
   msgPayload           testPayload = NULL;

   result = aBridge->msgPayloadSetParent(testPayload, NULL);
   EXPECT_EQ(result, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadGeneralTests, GetByteSizeValid)
{
    msgPayload          testPayload = NULL;
    mama_size_t         testSize = 0;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);

    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

    result = aBridge->msgPayloadGetByteSize(testPayload, &testSize);

    CHECK_NON_IMPLEMENTED_OPTIONAL(result);

    EXPECT_EQ(result, MAMA_STATUS_OK);
}

TEST_F(PayloadGeneralTests, GetByteSizeInValidPayload)
{
   mama_size_t          testSize = 0;

   result = aBridge->msgPayloadGetByteSize(NULL, &testSize);
   EXPECT_EQ(result, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadGeneralTests, GetByteSizeInValidSize)
{
   msgPayload           testPayload = NULL;

   result = aBridge->msgPayloadGetByteSize(testPayload, NULL);
   EXPECT_EQ(result, MAMA_STATUS_NULL_ARG);
}


TEST_F(PayloadGeneralTests, GetNumberFieldsValid)
{
	mama_size_t         numFields = 0;
	msgPayload          testPayload = NULL;

	result = aBridge->msgPayloadCreate(&testPayload);
    EXPECT_EQ(result, MAMA_STATUS_OK);

	result = aBridge->msgPayloadAddChar(testPayload, NULL, 1, 'a');
	result = aBridge->msgPayloadAddChar(testPayload, NULL, 2, 'b');
	result = aBridge->msgPayloadAddChar(testPayload, NULL, 3, 'c');
	result = aBridge->msgPayloadAddChar(testPayload, NULL, 4, 'd');
	result = aBridge->msgPayloadAddChar(testPayload, NULL, 5, 'e');
	result = aBridge->msgPayloadAddChar(testPayload, NULL, 6, 'f');
	result = aBridge->msgPayloadAddChar(testPayload, NULL, 7, 'g');
	result = aBridge->msgPayloadAddChar(testPayload, NULL, 8, 'h');
	result = aBridge->msgPayloadAddChar(testPayload, NULL, 9, 'i');
    EXPECT_EQ(result, MAMA_STATUS_OK);

	result = aBridge->msgPayloadGetNumFields(testPayload, &numFields);
	EXPECT_EQ (result, MAMA_STATUS_OK);
	EXPECT_EQ (numFields, (mama_size_t)9);
}

TEST_F(PayloadGeneralTests, GetNumberFieldsInValidPayload)
{
	mama_size_t          numFields = 0;

	result = aBridge->msgPayloadGetNumFields(NULL, &numFields);
	EXPECT_EQ (result, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadGeneralTests, GetNumberFieldsInValidNumFields)
{
	msgPayload           testPayload = NULL;

	aBridge->msgPayloadCreate(&testPayload);
	aBridge->msgPayloadAddChar(testPayload, NULL, 1, 'a');

	result = aBridge->msgPayloadGetNumFields(testPayload, NULL);
	EXPECT_EQ (result, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadGeneralTests, GetSendSubjectValid)
{
    msgPayload          testPayload = NULL;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);

    result = aBridge->msgPayloadGetSendSubject(testPayload, NULL);
    EXPECT_EQ (result, MAMA_STATUS_NOT_IMPLEMENTED);
}


TEST_F(PayloadGeneralTests, GetSendSubjectInValidMsg)
{
    const char**          mySubject = NULL;

	result = aBridge->msgPayloadGetSendSubject(NULL, mySubject);

	CHECK_NON_IMPLEMENTED_OPTIONAL(result);

	EXPECT_EQ (result, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadGeneralTests, GetSendSubjectInValidSubject)
{
    msgPayload          testPayload = NULL;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);

	result = aBridge->msgPayloadGetSendSubject(testPayload, NULL);
	EXPECT_EQ (result, MAMA_STATUS_NOT_IMPLEMENTED);
}

TEST_F(PayloadGeneralTests, ToStringValid)
{
    msgPayload          testPayload = NULL;
    const char*         char_result;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);

    char_result = aBridge->msgPayloadToString(testPayload);
	EXPECT_STREQ (char_result, "{}");
}

TEST_F(PayloadGeneralTests, ToStringValidConsistent)
{
	msgPayload          testPayload = NULL;
	msgPayload          testPayload2 = NULL;
	const char*         char_result;
	const char*         char_result2;

	result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);

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
}

TEST_F(PayloadGeneralTests, ToStringInValid)
{
    msgPayload          testPayload = NULL;
    const char*         char_result;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);

	char_result = aBridge->msgPayloadToString(NULL);
	EXPECT_STREQ (char_result, NULL);
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
	ASSERT_EQ (result, MAMA_STATUS_OK);

	aBridge->msgFieldPayloadCreate(&testMamaField);
	ASSERT_EQ (result, MAMA_STATUS_OK);

	result = aBridge->msgPayloadIterateFields(
			testPayload,
			(mamaMsg)NOT_NULL,
			(mamaMsgField)testMamaField,
			dummyIteratorCallback,
			NULL);

	EXPECT_EQ (result, MAMA_STATUS_OK);
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
	EXPECT_EQ (result, MAMA_STATUS_NULL_ARG);
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
	EXPECT_EQ (result, MAMA_STATUS_OK);

	result = aBridge->msgPayloadIterateFields(testPayload, NULL, testMamaField, testMamaMsgIteratorCb, &testClosure);
	EXPECT_EQ (result, MAMA_STATUS_NULL_ARG);
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
	EXPECT_EQ (result, MAMA_STATUS_OK);

	result = aBridge->msgPayloadIterateFields(testPayload, testMamaMsg, NULL, testMamaMsgIteratorCb, &testClosure);
	EXPECT_EQ (result, MAMA_STATUS_NULL_ARG);
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
	EXPECT_EQ (result, MAMA_STATUS_OK);

	result = aBridge->msgPayloadIterateFields(testPayload, testMamaMsg, testMamaField, NULL, &testClosure);
	EXPECT_EQ (result, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadGeneralTests, SerializeValid)
{
    msgPayload          testPayload = NULL;
    char*               testBuffer;
    mama_size_t         testBufferLength = 10;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);

    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

	result = aBridge->msgPayloadSerialize(testPayload, (const void**)&testBuffer, &testBufferLength);
	EXPECT_EQ (result, MAMA_STATUS_OK);
}

TEST_F(PayloadGeneralTests, SerializeInValidPayLoad)
{
    msgPayload          testPayload = NULL;
    char*               testBuffer;
    mama_size_t         testBufferLength = 0;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);

	result = aBridge->msgPayloadSerialize(NULL, (const void**)&testBuffer, &testBufferLength);
	EXPECT_EQ (result, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadGeneralTests, SerializeInValidBuffer)
{
    msgPayload          testPayload = NULL;
    mama_size_t         testBufferLength = 10;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);

    aBridge->msgPayloadAddString (testPayload, "name2", 101, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 101, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 101, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 101, "Fun");

	result = aBridge->msgPayloadSerialize(testPayload, NULL, &testBufferLength);
	EXPECT_EQ (result, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadGeneralTests, SerializeInValidBufferLength)
{
    msgPayload          testPayload = NULL;
    char*               testBuffer;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);

    aBridge->msgPayloadAddString (testPayload, "name2", 101, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 101, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 101, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 101, "Fun");

	result = aBridge->msgPayloadSerialize(testPayload, (const void**)&testBuffer, NULL);
	EXPECT_EQ (result, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadGeneralTests, UnSerializeValid)
{
    msgPayload          testPayload = NULL;
    const void*         testBuffer;
    mama_size_t         testBufferLength = 50;

    result = aBridge->msgPayloadCreate(&testPayload);
	ASSERT_EQ (result, MAMA_STATUS_OK);

    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    result = aBridge->msgPayloadSerialize(testPayload, (const void**)&testBuffer, &testBufferLength);
	ASSERT_EQ (result, MAMA_STATUS_OK);

    result = aBridge->msgPayloadUnSerialize(testPayload, (const void**)testBuffer, testBufferLength);
	ASSERT_EQ (result, MAMA_STATUS_OK);
}

TEST_F(PayloadGeneralTests, UnSerializeInValidPayload)
{
    msgPayload          testPayload = NULL;
    const void*         testBuffer;
    mama_size_t         testBufferLength = 10;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);

    result = aBridge->msgPayloadUnSerialize(NULL, &testBuffer, testBufferLength);
	EXPECT_EQ (result, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadGeneralTests, UnSerializeInValidBuffer)
{
    msgPayload          testPayload = NULL;
    mama_size_t         testBufferLength = 0;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);

    result = aBridge->msgPayloadUnSerialize(testPayload, NULL, testBufferLength);
	EXPECT_EQ (result, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadGeneralTests, UnSerializeInValidBufferLength)
{
    msgPayload          testPayload = NULL;
    const void*         testBuffer;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);

    result = aBridge->msgPayloadUnSerialize(testPayload, &testBuffer, NULL);
	EXPECT_EQ (result, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadGeneralTests, GetByteBufferValid)
{
    msgPayload          testPayload = NULL;
    char*               testBuffer;
    mama_size_t         testBufferLength = 0;
    mamaMsg             testMamaMsg = NULL;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);

    result = aBridge->msgPayloadSetParent(testPayload, testMamaMsg);
    EXPECT_EQ(result, MAMA_STATUS_OK);

    aBridge->msgPayloadAddString (testPayload, "name2", 101, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 101, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 101, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 101, "Fun");

	result = aBridge->msgPayloadGetByteBuffer(testPayload, (const void**)&testBuffer, &testBufferLength);
	EXPECT_EQ (result, MAMA_STATUS_OK);
}

TEST_F(PayloadGeneralTests, GetByteBufferInValidPayload)
{
    msgPayload          testPayload = NULL;
    char*               testBuffer;
    mama_size_t         testBufferLength = 0;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);

    aBridge->msgPayloadAddString (testPayload, "name2", 101, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 101, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 101, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 101, "Fun");

	result = aBridge->msgPayloadGetByteBuffer(NULL, (const void**)&testBuffer, &testBufferLength);
	EXPECT_EQ (result, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadGeneralTests, GetByteBufferInValidBuffer)
{
    msgPayload          testPayload = NULL;
    mama_size_t         testBufferLength = 0;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);

    aBridge->msgPayloadAddString (testPayload, "name2", 101, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 101, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 101, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 101, "Fun");

	result = aBridge->msgPayloadGetByteBuffer(testPayload, NULL, &testBufferLength);
	EXPECT_EQ (result, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadGeneralTests, GetByteBufferInValidBufferLength)
{
    msgPayload          testPayload = NULL;
    char*               testBuffer;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);

    aBridge->msgPayloadAddString (testPayload, "name2", 101, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 101, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 101, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 101, "Fun");

	result = aBridge->msgPayloadGetByteBuffer(testPayload, (const void**)&testBuffer, NULL);
	EXPECT_EQ (result, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadGeneralTests, SetByteBufferValid)
{
    msgPayload          testPayload = NULL;
    const char*         testBuffer;
    mama_size_t         testBufferLength = 100;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);

    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

	result = aBridge->msgPayloadGetByteBuffer(testPayload, (const void**)&testBuffer, &testBufferLength);
	EXPECT_EQ (result, MAMA_STATUS_OK);

	result = aBridge->msgPayloadCreateFromByteBuffer(&testPayload, aBridge, (const void*)testBuffer, testBufferLength);
	EXPECT_EQ (result, MAMA_STATUS_OK);

    result = aBridge->msgPayloadSetByteBuffer(&testPayload, aBridge, &testBuffer, testBufferLength);
	EXPECT_EQ (result, MAMA_STATUS_OK);
}

TEST_F(PayloadGeneralTests, SetByteBufferInValidPayload)
{
    msgPayload          testPayload = NULL;
    const char*         testBuffer;
    mama_size_t         testBufferLength = 0;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);

    result = aBridge->msgPayloadSetByteBuffer(NULL, aBridge, &testBuffer, testBufferLength);
	EXPECT_EQ (result, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadGeneralTests, SetByteBufferInValidBridge)
{
    msgPayload          testPayload = NULL;
    const char*         testBuffer;
    mama_size_t         testBufferLength = 0;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);

    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

    result = aBridge->msgPayloadSetByteBuffer(&testPayload, NULL, &testBuffer, testBufferLength);
	EXPECT_EQ (result, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadGeneralTests, SetByteBufferInValidBuffer)
{
    msgPayload          testPayload = NULL;
    mama_size_t         testBufferLength = 0;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);

    result = aBridge->msgPayloadSetByteBuffer(&testPayload, aBridge, NULL, testBufferLength);
	EXPECT_EQ (result, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadGeneralTests, SetByteBufferInValidBufferLength)
{
    msgPayload          testPayload = NULL;
    const char*         testBuffer;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);

    aBridge->msgPayloadAddString (testPayload, "name2", 101, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 101, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 101, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 101, "Fun");

    result = aBridge->msgPayloadSetByteBuffer(&testPayload, aBridge, &testBuffer, 0);
	EXPECT_EQ (result, MAMA_STATUS_NULL_ARG);
}

/* TODO: Check the values in the created payload.
 */
TEST_F(PayloadGeneralTests, CreateFromByteBufferValid)
{
    msgPayload          testPayload = NULL;
    msgPayload          testPayloadNew = NULL;
    char*               testBuffer;
    mama_size_t         testBufferLength = 0;

    result = aBridge->msgPayloadCreate(&testPayload);
    result = aBridge->msgPayloadCreate(&testPayloadNew);
	EXPECT_EQ (result, MAMA_STATUS_OK);

    aBridge->msgPayloadAddString (testPayload, "name2", 101, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 101, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 101, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 101, "Fun");

	result = aBridge->msgPayloadGetByteBuffer(testPayload, (const void**)&testBuffer, &testBufferLength);
	EXPECT_EQ (result, MAMA_STATUS_OK);

	result = aBridge->msgPayloadCreateFromByteBuffer(&testPayloadNew, aBridge, (const void*)testBuffer, testBufferLength);
	EXPECT_EQ (result, MAMA_STATUS_OK);
}

TEST_F(PayloadGeneralTests, CreateFromByteBufferInValidPayLoad)
{
    msgPayload          testPayload = NULL;
    char*               testBuffer;
    mama_size_t         testBufferLength = 0;

    result = aBridge->msgPayloadCreate(&testPayload);
    EXPECT_EQ(result, MAMA_STATUS_OK);

	result = aBridge->msgPayloadGetByteBuffer(testPayload, (const void**)&testBuffer, &testBufferLength);
	EXPECT_EQ (result, MAMA_STATUS_OK);

	result = aBridge->msgPayloadCreateFromByteBuffer(NULL, aBridge, (const void*)testBuffer, testBufferLength);
	EXPECT_EQ (result, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadGeneralTests, CreateFromByteBufferInValidBridge)
{
    msgPayload          testPayload = NULL;
    msgPayload          testPayloadNew = NULL;
    char*               testBuffer;
    mama_size_t         testBufferLength = 0;

    result = aBridge->msgPayloadCreate(&testPayload);
    result = aBridge->msgPayloadCreate(&testPayloadNew);
	EXPECT_EQ (result, MAMA_STATUS_OK);

    aBridge->msgPayloadAddString (testPayload, "name2", 101, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 101, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 101, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 101, "Fun");

	result = aBridge->msgPayloadGetByteBuffer(testPayload, (const void**)&testBuffer, &testBufferLength);
	EXPECT_EQ (result, MAMA_STATUS_OK);

	result = aBridge->msgPayloadCreateFromByteBuffer(&testPayloadNew, NULL, (const void*)testBuffer, testBufferLength);
	EXPECT_EQ (result, MAMA_STATUS_OK);
}

TEST_F(PayloadGeneralTests, CreateFromByteBufferInValidBuffer)
{
    msgPayload          testPayload = NULL;
    msgPayload          testPayloadNew = NULL;
    char*               testBuffer;
    mama_size_t         testBufferLength = 0;

    result = aBridge->msgPayloadCreate(&testPayload);
    result = aBridge->msgPayloadCreate(&testPayloadNew);
	EXPECT_EQ (result, MAMA_STATUS_OK);

    aBridge->msgPayloadAddString (testPayload, "name2", 101, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 101, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 101, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 101, "Fun");

	result = aBridge->msgPayloadGetByteBuffer(testPayload, (const void**)&testBuffer, &testBufferLength);
	EXPECT_EQ (result, MAMA_STATUS_OK);

	result = aBridge->msgPayloadCreateFromByteBuffer(&testPayloadNew, aBridge, NULL, testBufferLength);
	EXPECT_EQ (result, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadGeneralTests, CreateFromByteBufferInValidBufferLength)
{
    msgPayload          testPayload = NULL;
    msgPayload          testPayloadNew = NULL;
    char*               testBuffer;
    mama_size_t         testBufferLength = 0;

    result = aBridge->msgPayloadCreate(&testPayload);
    result = aBridge->msgPayloadCreate(&testPayloadNew);
	EXPECT_EQ (result, MAMA_STATUS_OK);

    aBridge->msgPayloadAddString (testPayload, "name2", 101, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 101, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 101, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 101, "Fun");

	result = aBridge->msgPayloadGetByteBuffer(testPayload, (const void**)&testBuffer, &testBufferLength);
	EXPECT_EQ (result, MAMA_STATUS_OK);

	result = aBridge->msgPayloadCreateFromByteBuffer(&testPayloadNew, aBridge, (const void*)testBuffer, 0);
	EXPECT_EQ (result, MAMA_STATUS_INVALID_ARG);
}

TEST_F(PayloadGeneralTests, ApplyValid)
{
    msgPayload          testDestPayload = NULL;
    msgPayload          testSrcPayload = NULL;

    result = aBridge->msgPayloadCreate(&testDestPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);

    result = aBridge->msgPayloadCreate(&testSrcPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);

	result = aBridge->msgPayloadApply(testDestPayload, testSrcPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);

    aBridge->msgPayloadAddString (testSrcPayload, "name", NULL, "Unit");

	result = aBridge->msgPayloadApply(testDestPayload, testSrcPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);

    aBridge->msgPayloadAddString (testSrcPayload, NULL, 101, "Testing");

	result = aBridge->msgPayloadApply(testDestPayload, testSrcPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);
}

TEST_F(PayloadGeneralTests, ApplyInValidDestPayload)
{
    msgPayload          testSrcPayload = NULL;
    mama_size_t         numFields = 0;

    result = aBridge->msgPayloadCreate(&testSrcPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);

	result = aBridge->msgPayloadAddChar(testSrcPayload, NULL, 9, 'i');
    EXPECT_EQ(result, MAMA_STATUS_OK);

	result = aBridge->msgPayloadGetNumFields(testSrcPayload, &numFields);
	EXPECT_EQ (result, MAMA_STATUS_OK);
	EXPECT_EQ (numFields, (mama_size_t) 1);

    result = aBridge->msgPayloadApply(NULL, testSrcPayload);
    EXPECT_EQ (result, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadGeneralTests, ApplyInValidSrcPayload)
{
    msgPayload          testDestPayload = NULL;
    mama_size_t         numFields = 0;

    result = aBridge->msgPayloadCreate(&testDestPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);

	result = aBridge->msgPayloadAddChar(testDestPayload, NULL, 9, 'i');
    EXPECT_EQ(result, MAMA_STATUS_OK);

	result = aBridge->msgPayloadGetNumFields(testDestPayload, &numFields);
	EXPECT_EQ (result, MAMA_STATUS_OK);
	EXPECT_EQ (numFields, (mama_size_t) 1);

    result = aBridge->msgPayloadApply(testDestPayload, NULL);
    EXPECT_EQ (result, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadGeneralTests, GetNativeMsgValid)
{
    msgPayload          testPayload = NULL;
    char*               testNativeMsg;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);
    
    result = aBridge->msgPayloadGetNativeMsg(testPayload, (void**)&testNativeMsg);
	EXPECT_EQ (result, MAMA_STATUS_OK);
}

TEST_F(PayloadGeneralTests, GetNativeMsgInValidMsg)
{
    msgPayload          testPayload = NULL;
    char*               testNativeMsg;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);
    
    result = aBridge->msgPayloadGetNativeMsg(NULL, (void**)&testNativeMsg);
	EXPECT_EQ (result, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadGeneralTests, GetNativeMsgInValidNativeMsg)
{
    msgPayload          testPayload = NULL;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);
    
    result = aBridge->msgPayloadGetNativeMsg(testPayload, NULL);
	EXPECT_EQ (result, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadGeneralTests, GetFieldAsStringValid)
{
    msgPayload          testPayload = NULL;
    char                testName;
    mama_fid_t          testFid = 105;
    char                testBuf[256];
    mama_size_t         testLen = 10;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);
    
    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");
    
    result = aBridge->msgPayloadGetFieldAsString(testPayload, &testName, testFid, testBuf, testLen);
	EXPECT_EQ (result, MAMA_STATUS_OK);
}

TEST_F(PayloadGeneralTests, GetFieldAsStringInValidPayload)
{
    msgPayload          testPayload = NULL;
    char                testName;
    mama_fid_t          testFid = 0;
    char                testBuf;
    mama_size_t         testLen = 0;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);
    
    result = aBridge->msgPayloadGetFieldAsString(NULL, &testName, testFid, &testBuf, testLen);
	EXPECT_EQ (result, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadGeneralTests, GetFieldAsStringInValidName)
{
    msgPayload          testPayload = NULL;
    mama_fid_t          testFid = 105;
    char                testBuf[256];
    mama_size_t         testLen = 10;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);
    
    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");
    
    result = aBridge->msgPayloadGetFieldAsString(testPayload, NULL, testFid, testBuf, testLen);
	EXPECT_EQ (result, MAMA_STATUS_OK);
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
	EXPECT_EQ (result, MAMA_STATUS_OK);
    
    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");
    
    result = aBridge->msgPayloadGetFieldAsString(testPayload, &testName, 0, &testBuf, testLen);
	EXPECT_EQ (result, MAMA_STATUS_OK);
    */
}

TEST_F(PayloadGeneralTests, GetFieldAsStringInValidBuffer)
{
    msgPayload          testPayload = NULL;
    char                testName;
    mama_fid_t          testFid = 105;
    mama_size_t         testLen = 10;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);

    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");
    
    result = aBridge->msgPayloadGetFieldAsString(testPayload, &testName, testFid, NULL, testLen);
	EXPECT_EQ (result, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadGeneralTests, GetFieldAsStringInValidBufferLength)
{
    msgPayload          testPayload = NULL;
    char                testName;
    mama_fid_t          testFid = 105;
    char                testBuf[256];

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);
    
    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

    result = aBridge->msgPayloadGetFieldAsString(testPayload, &testName, testFid, testBuf, 0);
	EXPECT_EQ (result, MAMA_STATUS_INVALID_ARG);
}

TEST_F(PayloadGeneralTests, GetFieldValid)
{
    msgPayload          testPayload = NULL;
    char                testName;
    mama_fid_t          testFid = 105;
    msgFieldPayload     testResult = NULL;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);
    
    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

    result = aBridge->msgFieldPayloadCreate(&testResult);
    EXPECT_EQ(result, MAMA_STATUS_OK);

    result = aBridge->msgPayloadGetField(testPayload, &testName, testFid, &testResult);
	EXPECT_EQ (result, MAMA_STATUS_OK);
}

TEST_F(PayloadGeneralTests, GetFieldInValidPayload)
{
    msgPayload          testPayload = NULL;
    char                testName;
    mama_fid_t          testFid = 0;
    msgFieldPayload     testResult = NULL;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);
    
    result = aBridge->msgFieldPayloadCreate(&testResult);
    EXPECT_EQ(result, MAMA_STATUS_OK);

    result = aBridge->msgPayloadGetField(NULL, &testName, testFid, &testResult);
	EXPECT_EQ (result, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadGeneralTests, GetFieldInValidName)
{
    msgPayload          testPayload = NULL;
    mama_fid_t          testFid = 105;
    msgFieldPayload     testResult = NULL;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);
    
    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

    result = aBridge->msgFieldPayloadCreate(&testResult);
    EXPECT_EQ(result, MAMA_STATUS_OK);

    result = aBridge->msgPayloadGetField(testPayload, NULL, testFid, &testResult);
	EXPECT_EQ (result, MAMA_STATUS_OK);
}

TEST_F(PayloadGeneralTests, GetFieldInValidFid)
{
    /*
     * 0 is a valid FID so this test is not necessary
    msgPayload          testPayload = NULL;
    char                testName;
    msgFieldPayload     testResult = NULL;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);
    
    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

    result = aBridge->msgFieldPayloadCreate(&testResult);
    EXPECT_EQ(result, MAMA_STATUS_OK);

    result = aBridge->msgPayloadGetField(testPayload, &testName, 0, &testResult);
	EXPECT_EQ (result, MAMA_STATUS_OK);
    */
}

TEST_F(PayloadGeneralTests, GetFieldInValidResult)
{
    msgPayload          testPayload = NULL;
    char                testName;
    mama_fid_t          testFid = 105;
    msgFieldPayload     testField = NULL;
    //mama_bool_t         value = 1;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);
    
    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

    result = aBridge->msgFieldPayloadCreate(&testField);
    EXPECT_EQ(result, MAMA_STATUS_OK);

    //result = aBridge->msgFieldPayloadUpdateBool(testField, testPayload, value);
	//EXPECT_EQ (result, MAMA_STATUS_NULL_ARG);

    result = aBridge->msgPayloadGetField(testPayload, &testName, testFid, NULL);
	EXPECT_EQ (result, MAMA_STATUS_NULL_ARG);
}

/* ************************************************************************* */
/* Payload Field General Tests */
/* ************************************************************************* */

TEST_F(PayloadGeneralTests, IterCreateValid)
{
    msgPayload          testPayload = NULL;
    msgPayloadIter      testIter = NULL;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);
    
    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

    result = aBridge->msgPayloadIterCreate(&testIter, testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);
}

TEST_F(PayloadGeneralTests, IterCreateInValidtestIter)
{
    msgPayload          testPayload = NULL;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);
    
    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

    result = aBridge->msgPayloadIterCreate(NULL, testPayload);
	EXPECT_EQ (result, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadGeneralTests, IterCreateInValidPayload)
{
    msgPayloadIter      testIter = NULL;

    result = aBridge->msgPayloadIterCreate(&testIter, NULL);
	EXPECT_EQ (result, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadGeneralTests, IterDestroyValid)
{
    msgPayloadIter      testIter = NULL;
    msgFieldPayload     testPayload = NULL;

    result = aBridge->msgPayloadCreate(&testPayload);
    EXPECT_EQ (result, MAMA_STATUS_OK);

    result = aBridge->msgPayloadIterCreate(&testIter, testPayload);

    result = aBridge->msgPayloadIterDestroy(testIter);
    EXPECT_EQ (result, MAMA_STATUS_OK);
}

TEST_F(PayloadGeneralTests, IterDestroyInValidIter)
{
    result = aBridge->msgPayloadIterDestroy(NULL);
	EXPECT_EQ (result, MAMA_STATUS_NULL_ARG);
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
	EXPECT_EQ (result, MAMA_STATUS_OK);
    
    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

    result = aBridge->msgPayloadIterCreate(&testIter, testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);

    if (NULL == aBridge->msgPayloadIterNext(testIter, testField, testPayload))
        FAIL();

    if (NULL == aBridge->msgPayloadIterNext(testIter, testField, testPayload))
        FAIL();

    if (NULL == aBridge->msgPayloadIterNext(testIter, testField, testPayload))
        FAIL();

    if (NULL == aBridge->msgPayloadIterNext(testIter, testField, testPayload))
        FAIL();
}

TEST_F(PayloadGeneralTests, IterNextValidConsistent)
{
    msgPayload          testPayload = NULL;
    msgPayloadIter      testIter = NULL;
    msgFieldPayload     testField = NULL;

    result = aBridge->msgPayloadCreate(&testPayload);
    EXPECT_EQ (result, MAMA_STATUS_OK);

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
    EXPECT_EQ (result, MAMA_STATUS_OK);

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
}

TEST_F(PayloadGeneralTests, IterNextInValidIter)
{
    msgPayload          testPayload = NULL;
    msgFieldPayload     testField = NULL;
    msgFieldPayload     output = NULL;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);
    
    result = aBridge->msgFieldPayloadCreate(&testField);
    EXPECT_EQ(result, MAMA_STATUS_OK);

    output = aBridge->msgPayloadIterNext(NULL, testField, testPayload);
    if (output != NULL)
        FAIL();
}

TEST_F(PayloadGeneralTests, IterNextInValidField)
{
    msgPayload          testPayload = NULL;
    msgPayloadIter      testIter = NULL;
    msgFieldPayload     output = NULL;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);
    
    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

    result = aBridge->msgPayloadIterCreate(&testIter, testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);

    output = aBridge->msgPayloadIterNext(testIter, NULL, testPayload);
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
	EXPECT_EQ (result, MAMA_STATUS_OK);
    
    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

    result = aBridge->msgFieldPayloadCreate(&testField);
    EXPECT_EQ(result, MAMA_STATUS_OK);

    result = aBridge->msgPayloadIterCreate(&testIter, testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);

    output = aBridge->msgPayloadIterNext(testIter, testField, NULL);
    if (output != NULL)
        FAIL();
}

TEST_F(PayloadGeneralTests, IterBeginValid)
{
    msgPayload          testPayload = NULL;
    msgPayloadIter      testIter = NULL;
    msgFieldPayload     testField = NULL;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);
    
    //Call on empty msg first
    aBridge->msgPayloadIterBegin(testIter, testField, testPayload);

    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

    result = aBridge->msgFieldPayloadCreate(&testField);
    EXPECT_EQ(result, MAMA_STATUS_OK);

    result = aBridge->msgPayloadIterCreate(&testIter, testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);

    if (NULL == aBridge->msgPayloadIterBegin(testIter, testField, testPayload))
        FAIL();
}

TEST_F(PayloadGeneralTests, IterBeginInValidIter)
{
    msgPayload          testPayload = NULL;
    msgPayloadIter      testIter = NULL;
    msgFieldPayload     testField = NULL;
    msgFieldPayload     output = NULL;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);
    
    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

    result = aBridge->msgFieldPayloadCreate(&testField);
    EXPECT_EQ(result, MAMA_STATUS_OK);

    result = aBridge->msgPayloadIterCreate(&testIter, testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);

    output = aBridge->msgPayloadIterBegin(NULL, testField, testPayload);
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
	EXPECT_EQ (result, MAMA_STATUS_OK);
    
    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

    result = aBridge->msgFieldPayloadCreate(&testField);
    EXPECT_EQ(result, MAMA_STATUS_OK);

    result = aBridge->msgPayloadIterCreate(&testIter, testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);

    output = aBridge->msgPayloadIterBegin(testIter, NULL, testPayload);
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
	EXPECT_EQ (result, MAMA_STATUS_OK);
    
    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

    result = aBridge->msgFieldPayloadCreate(&testField);
    EXPECT_EQ(result, MAMA_STATUS_OK);

    result = aBridge->msgPayloadIterCreate(&testIter, testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);

    output = aBridge->msgPayloadIterBegin(testIter, testField, NULL);
    if (output != NULL)
        FAIL();
}

TEST_F(PayloadGeneralTests, IterHasNextValid)
{
    msgPayload          testPayload = NULL;
    msgPayloadIter      testIter = NULL;
    mama_bool_t         output = NULL;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);
    
    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

    result = aBridge->msgPayloadIterCreate(&testIter, testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);

    output = aBridge->msgPayloadIterHasNext(testIter, testPayload);
	EXPECT_EQ (output, true);
}

TEST_F(PayloadGeneralTests, IterHasNextInValidIter)
{
    msgPayload          testPayload = NULL;
    mama_bool_t         output = NULL;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);
    
    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

    output = aBridge->msgPayloadIterHasNext(NULL, testPayload);
	EXPECT_EQ (output, false);
}

TEST_F(PayloadGeneralTests, IterHasNextInValidPayload)
{
    msgPayload          testPayload = NULL;
    msgPayloadIter      testIter = NULL;
    mama_bool_t         output = NULL;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);
    
    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

    result = aBridge->msgPayloadIterCreate(&testIter, testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);

    output = aBridge->msgPayloadIterHasNext(testIter, NULL);
	EXPECT_EQ (output, 0);
}

TEST_F(PayloadGeneralTests, IterAssociateValid)
{
    msgPayload          testPayload = NULL;
    msgPayloadIter      testIter = NULL;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);
    
    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

    result = aBridge->msgPayloadIterCreate(&testIter, testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);

    result = aBridge->msgPayloadIterAssociate(testIter, testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);

    result = aBridge->msgPayloadIterAssociate(testIter, testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);
}

TEST_F(PayloadGeneralTests, IterAssociateTwiceValid)
{
    msgPayload          testPayload = NULL;
    msgPayload          testPayload2 = NULL;
    msgPayloadIter      testIter = NULL;
    msgFieldPayload     testField = NULL;
    mama_fid_t          test_fid = 0;

    result = aBridge->msgPayloadCreate(&testPayload);
    EXPECT_EQ (result, MAMA_STATUS_OK);

    // Create 2 payloads
    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

    result = aBridge->msgPayloadCreate(&testPayload2);
    EXPECT_EQ (result, MAMA_STATUS_OK);

    aBridge->msgPayloadAddString (testPayload2, "name2", 202, "Repeating");
    aBridge->msgPayloadAddString (testPayload2, "name3", 203, "Things");
    aBridge->msgPayloadAddString (testPayload2, "name4", 204, "Is");
    aBridge->msgPayloadAddString (testPayload2, "name5", 205, "Great");

    // Create iterator 
    result = aBridge->msgPayloadIterCreate(&testIter, testPayload);
    EXPECT_EQ (result, MAMA_STATUS_OK);

    // associate iterator with a payload
    result = aBridge->msgPayloadIterAssociate(testIter, testPayload);
    EXPECT_EQ (result, MAMA_STATUS_OK);

    // Test current payload exist and iterated correctly
    result = aBridge->msgPayloadGetField(testPayload,NULL,102,&testField); 
    EXPECT_EQ (result, MAMA_STATUS_OK);

    testField = aBridge->msgPayloadIterNext(testIter,testField,testPayload);
     // Make sure we're traversing on a payload
    testField = aBridge->msgPayloadIterNext(testIter,testField,testPayload);

    result = aBridge->msgFieldPayloadGetFid(testField,NULL,NULL, &test_fid);
    EXPECT_EQ (test_fid, 103);

    // reuse iterator with new payload
    result = aBridge->msgPayloadIterAssociate(testIter, testPayload2);
    EXPECT_EQ (result, MAMA_STATUS_OK);

    // Test current payload exists and iterated correctly
    result = aBridge->msgPayloadGetField(testPayload2,NULL,202,&testField); 
    EXPECT_EQ (result, MAMA_STATUS_OK);

    testField = aBridge->msgPayloadIterNext(testIter,testField,testPayload2);
    // Make sure we're traversing on the 2nd payload
    testField = aBridge->msgPayloadIterNext(testIter,testField,testPayload2); 

    result = aBridge->msgFieldPayloadGetFid(testField,NULL,NULL, &test_fid);
    EXPECT_EQ (test_fid, 203);

    result = aBridge->msgPayloadIterDestroy(testIter);
    EXPECT_EQ(result, MAMA_STATUS_OK);

    result = aBridge->msgPayloadDestroy(testPayload);
    EXPECT_EQ(result, MAMA_STATUS_OK);

    result = aBridge->msgPayloadDestroy(testPayload2);
    EXPECT_EQ(result, MAMA_STATUS_OK);
}

TEST_F(PayloadGeneralTests, IterAssociateInValidIter)
{
    msgPayload          testPayload = NULL;
    msgPayloadIter      testIter = NULL;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);
    
    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

    result = aBridge->msgPayloadIterCreate(&testIter, testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);

    result = aBridge->msgPayloadIterAssociate(NULL, testPayload);
	EXPECT_EQ (result, MAMA_STATUS_NULL_ARG);
}

TEST_F(PayloadGeneralTests, IterAssociateInValidPayload)
{
    msgPayload          testPayload = NULL;
    msgPayloadIter      testIter = NULL;

    result = aBridge->msgPayloadCreate(&testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);
    
    aBridge->msgPayloadAddString (testPayload, "name2", 102, "Unit");
    aBridge->msgPayloadAddString (testPayload, "name3", 103, "Testing");
    aBridge->msgPayloadAddString (testPayload, "name4", 104, "Is");
    aBridge->msgPayloadAddString (testPayload, "name5", 105, "Fun");

    result = aBridge->msgPayloadIterCreate(&testIter, testPayload);
	EXPECT_EQ (result, MAMA_STATUS_OK);

    result = aBridge->msgPayloadIterAssociate(&testIter, NULL);
	EXPECT_EQ (result, MAMA_STATUS_NULL_ARG);
}

