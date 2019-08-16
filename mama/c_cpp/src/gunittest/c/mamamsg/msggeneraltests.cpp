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
#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif
#include <stdint.h>
#include "mama/mama.h"
#include "mama/msg.h"
#include "MainUnitTestC.h"
#include <iostream>
#include "msgimpl.h"
#include <cstdlib>
#include "dqstrategy.h"
#include "msgutils.h"
#include "mama/msgqualifier.h"

using std::cout;
using std::endl;

class MsgGeneralTestsC : public ::testing::Test
{
protected:
    MsgGeneralTestsC(void) 
        : mMsg           (NULL)
        , mPayloadBridge (NULL)
        , mStatus        (MAMA_STATUS_OK)
    {
        mama_loadPayloadBridge (&mPayloadBridge, getPayload());
        mama_loadBridge (&mMiddlewareBridge, getMiddleware());

    }

    virtual ~MsgGeneralTestsC(void)
    {

    };

    virtual void SetUp(void) 
    {
        mama_open();
        mamaMsg_create (&mMsg);
    };

    virtual void TearDown(void) 
    {
        mamaMsg_destroy(mMsg);
        mama_close();
    };
    
    mamaMsg            mMsg;
    mamaPayloadBridge  mPayloadBridge;
    mamaBridge         mMiddlewareBridge;
    mama_status        mStatus;
};

static const int INCLUDE_FIELD_NAME = (1 << 4);
static const int INCLUDE_FIELD_ID   = (1 << 2);

/*********************************************************************
 * Tests
*********************************************************************/
TEST_F (MsgGeneralTestsC, payloadConvertToString)
{
    ASSERT_STREQ (mamaPayload_convertToString (MAMA_PAYLOAD_WOMBAT_MSG), "WombatMsg");
    ASSERT_STREQ (mamaPayload_convertToString (MAMA_PAYLOAD_TIBRV), "TIBRV");
    ASSERT_STREQ (mamaPayload_convertToString (MAMA_PAYLOAD_FAST), "FAST");
    ASSERT_STREQ (mamaPayload_convertToString (MAMA_PAYLOAD_V5), "V5");
    ASSERT_STREQ (mamaPayload_convertToString (MAMA_PAYLOAD_QPID), "QPID");
    ASSERT_STREQ (mamaPayload_convertToString (MAMA_PAYLOAD_TICK42BLP), "TICK42BLP");
    ASSERT_STREQ (mamaPayload_convertToString (MAMA_PAYLOAD_RAI), "rai");
    ASSERT_STREQ (mamaPayload_convertToString (MAMA_PAYLOAD_EXEGY), "EXEGY");
    ASSERT_STREQ (mamaPayload_convertToString (MAMA_PAYLOAD_SOLACE), "solacemsg");
    ASSERT_STREQ (mamaPayload_convertToString (MAMA_PAYLOAD_UNKNOWN), "unknown");
}

TEST_F (MsgGeneralTestsC, msgCreateValid)
{
    mamaMsg createMsg = NULL;
    ASSERT_EQ (mamaMsg_create(&createMsg), MAMA_STATUS_OK);
    mamaMsg_destroy(createMsg);
}

TEST_F (MsgGeneralTestsC, msgCreateInValid)
{
    ASSERT_EQ (mamaMsg_create(NULL), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgGeneralTestsC, msgCreateForPayloadBridgeValid)
{
    mamaMsg createMsg = NULL;
    ASSERT_EQ (mamaMsg_createForPayloadBridge(&createMsg, mPayloadBridge), MAMA_STATUS_OK);
    mamaMsg_destroy(createMsg);
}

TEST_F (MsgGeneralTestsC, msgCreateForPayloadBridgeInValid)
{
    ASSERT_EQ (mamaMsg_createForPayloadBridge(NULL, mPayloadBridge), MAMA_STATUS_NULL_ARG);
}

/**************************************************************
    New Tests
**************************************************************/

TEST_F (MsgGeneralTestsC, msgClearValid)
{
    //Create & add fields 
    mamaMsg_addString (mMsg, "name0", 100, "test");
    mamaMsg_addBool (mMsg, "name1", 101, true);
    mamaMsg_addChar (mMsg, "name2", 102, 'A');

    ASSERT_EQ (mamaMsg_clear(mMsg), MAMA_STATUS_OK);
}

TEST_F (MsgGeneralTestsC, msgClearInValid)
{
    //Create & add fields 
    mamaMsg_addString (mMsg, "name0", 100, "test");
    mamaMsg_addBool (mMsg, "name1", 101, true);
    mamaMsg_addChar (mMsg, "name2", 102, 'A');

    ASSERT_EQ (mamaMsg_clear(NULL), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgGeneralTestsC, msgDetachValid)
{
    mamaQueue      queue    = NULL; 

    //Create & add fields to msg
    mamaMsg_addString (mMsg, "name0", 101, "test");

    mamaMsgImpl_setBridgeImpl (mMsg, mMiddlewareBridge);

    mama_getDefaultEventQueue(mMiddlewareBridge, &queue);

    mamaMsgImpl_setQueue (mMsg, queue);

    ASSERT_EQ (mamaMsg_detach(mMsg), MAMA_STATUS_OK);
}

TEST_F (MsgGeneralTestsC, msgDetachInValid)
{
    //Create & add fields to msg
    mamaMsg_addString (mMsg, "name0", 101, "test");

    ASSERT_EQ (mamaMsg_detach(NULL), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgGeneralTestsC, msgGetByteBufferValid)
{
    const void*    buffer       = NULL;
    mama_size_t    bufferLength = 0;

    //Create & add fields to msg 
    mamaMsg_addString (mMsg, "name0", 100, "test");
    mamaMsg_addBool (mMsg, "name1", 101, true);
    mamaMsg_addChar (mMsg, "name2", 102, 'A');
   
    ASSERT_EQ (mamaMsg_getByteBuffer (mMsg, &buffer, &bufferLength), MAMA_STATUS_OK); 
}

TEST_F (MsgGeneralTestsC, msgGetByteBufferInValidMsg)
{
    const void*    buffer       = NULL;
    mama_size_t    bufferLength = 0;

    //Create & add fields to msg 
    mamaMsg_addString (mMsg, "name0", 100, "test");
    mamaMsg_addBool (mMsg, "name1", 101, true);
    mamaMsg_addChar (mMsg, "name2", 102, 'A');
   
    ASSERT_EQ (mamaMsg_getByteBuffer (NULL, &buffer, &bufferLength), MAMA_STATUS_NULL_ARG); 
}

TEST_F (MsgGeneralTestsC, msgGetByteBufferInValidBuffer)
{
    mama_size_t    bufferLength = 0;

    //Create & add fields to msg
    mamaMsg_addString (mMsg, "name0", 100, "test");
    mamaMsg_addBool (mMsg, "name1", 101, true);
    mamaMsg_addChar (mMsg, "name2", 102, 'A');
   
    ASSERT_EQ (mamaMsg_getByteBuffer (mMsg, NULL, &bufferLength), MAMA_STATUS_NULL_ARG); 
}

TEST_F (MsgGeneralTestsC, msgGetByteBufferInValidBufferLength)
{
    const void*    buffer       = NULL;

    //Create and add fields to msg 
    mamaMsg_addString (mMsg, "name0", 100, "test");
    mamaMsg_addBool (mMsg, "name1", 101, true);
    mamaMsg_addChar (mMsg, "name2", 102, 'A');

    ASSERT_EQ (mamaMsg_getByteBuffer (mMsg, &buffer, NULL), MAMA_STATUS_NULL_ARG); 
}

TEST_F (MsgGeneralTestsC, msgCreateFromByteBufferValid)
{
    const void*   buffer       = NULL;
    mama_size_t   bufferLength = 0;
    mamaMsg       newMsg       = NULL;

    //add fields to msg 
    mamaMsg_addString (mMsg, "name0", 100, "test");
    mamaMsg_addBool (mMsg, "name1", 101, true);
    mamaMsg_addChar (mMsg, "name2", 102, 'A');

    // Get the byteBuffer from msg 
    ASSERT_EQ (mamaMsg_getByteBuffer (mMsg, &buffer, &bufferLength), MAMA_STATUS_OK); 

    //create mamaMsg using byteBuffer
    ASSERT_EQ (mamaMsg_createFromByteBuffer(&newMsg, buffer, bufferLength), MAMA_STATUS_OK);
    mamaMsg_destroy (newMsg);
}

TEST_F (MsgGeneralTestsC, DISABLED_msgCreateFromByteBufferInValidMsg)
{
    const void*   buffer       = NULL;
    mama_size_t   bufferLength = 0;

    //add fields to msg 
    mamaMsg_addString (mMsg, "name0", 100, "test");
    mamaMsg_addBool (mMsg, "name1", 101, true);
    mamaMsg_addChar (mMsg, "name2", 102, 'A');

    // Get the byteBuffer from msg 
    ASSERT_EQ (mamaMsg_getByteBuffer (mMsg, &buffer, &bufferLength), MAMA_STATUS_OK); 

    //create mamaMsg using byteBuffer
    ASSERT_EQ (mamaMsg_createFromByteBuffer(NULL, buffer, bufferLength), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgGeneralTestsC, DISABLED_msgCreateFromByteBufferInValidBuffer)
{
    mama_size_t   bufferLength = 0;
    mamaMsg       newMsg       = NULL;

    //create mamaMsg using byteBuffer
    ASSERT_EQ (mamaMsg_createFromByteBuffer(&newMsg, NULL, bufferLength), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgGeneralTestsC, msgCreateFromByteBufferInValidBufferLength)
{
    const void*   buffer       = NULL;
    mama_size_t   bufferLength = 0;
    mamaMsg       newMsg       = NULL;

    //add fields to msg 
    mamaMsg_addString (mMsg, "name0", 100, "test");
    mamaMsg_addBool (mMsg, "name1", 101, true);
    mamaMsg_addChar (mMsg, "name2", 102, 'A');

    // Get the byteBuffer from msg 
    ASSERT_EQ (mamaMsg_getByteBuffer (mMsg, &buffer, &bufferLength), MAMA_STATUS_OK); 

    //create mamaMsg using byteBuffer
    ASSERT_EQ (mamaMsg_createFromByteBuffer (&newMsg, buffer, 0), MAMA_STATUS_NULL_ARG);

    mamaMsg_destroy (newMsg);
}

TEST_F (MsgGeneralTestsC, msgGetNumFieldsValid)
{
    const char*  testString = "test";
    mama_size_t  numFields  = 0;

    //add fields to msg
    mamaMsg_addString (mMsg, "name0", 101, testString);
    mamaMsg_addString (mMsg, "name1", 102, "test2");
    mamaMsg_addI8 (mMsg, "name2", 103, 8);
   
    ASSERT_EQ (mamaMsg_getNumFields(mMsg, &numFields), MAMA_STATUS_OK);
}

TEST_F (MsgGeneralTestsC, msgGetNumFieldsInValidMsg)
{
    const char*  testString = "test";
    mama_size_t  numFields  = 0;

    //add fields to msg
    mamaMsg_addString (mMsg, "name0", 101, testString);
    mamaMsg_addString (mMsg, "name1", 102, "test2");
    mamaMsg_addI8 (mMsg, "name2", 103, 8);
   
    ASSERT_EQ (mamaMsg_getNumFields(NULL, &numFields), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgGeneralTestsC, msgGetNumFieldsInValidNumFields)
{
    const char*  testString = "test";

    //add fields to msg
    mamaMsg_addString (mMsg, "name0", 101, testString);
    mamaMsg_addString (mMsg, "name1", 102, "test2");
    mamaMsg_addI8 (mMsg, "name2", 103, 8);
   
    ASSERT_EQ (mamaMsg_getNumFields(mMsg, NULL), MAMA_STATUS_INVALID_ARG);
}

TEST_F (MsgGeneralTestsC, msgGetNumFieldsSubMessage)
{
    mamaMsg     submsg      = NULL;
    const char* testString  = "test";
    mama_status status      = MAMA_STATUS_OK;
    mama_size_t numFields   = 0;
    mama_size_t addedFields = 3;

    ASSERT_EQ (MAMA_STATUS_OK, mamaMsg_create(&submsg));

    ASSERT_EQ (MAMA_STATUS_OK, mamaMsg_addString (mMsg, "name0", 101, testString));
    ASSERT_EQ (MAMA_STATUS_OK, mamaMsg_addU32    (mMsg, "name1", 102, 12345));

    ASSERT_EQ (MAMA_STATUS_OK, mamaMsg_addBool (submsg, "sub1", 10001, (mama_bool_t)1));
    ASSERT_EQ (MAMA_STATUS_OK, mamaMsg_addI8   (submsg, "sub2", 10002, -12));

    status = mamaMsg_addMsg (mMsg, "name2", 103, submsg);

    CHECK_NON_IMPLEMENTED_RECOMMENDED (status);

    EXPECT_EQ (MAMA_STATUS_OK, mamaMsg_getNumFields (mMsg, &numFields));
    EXPECT_EQ (addedFields, numFields);

    ASSERT_EQ (MAMA_STATUS_OK, mamaMsg_destroy (submsg));
    submsg = NULL;
}

TEST_F (MsgGeneralTestsC, msgGetPayloadTypeValid)
{
    char  payloadType = MAMA_PAYLOAD_ID_UNKNOWN;
    
    ASSERT_EQ (mamaMsgImpl_getPayloadId(mMsg, &payloadType), MAMA_STATUS_OK);
}

TEST_F (MsgGeneralTestsC, msgGetPayloadTypeInValidMsg)
{
    char  payloadType = MAMA_PAYLOAD_ID_UNKNOWN;

    ASSERT_EQ (mamaMsgImpl_getPayloadId(NULL, &payloadType), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgGeneralTestsC, msgImplSetMessageOwnerValid)
{
    const char*  testString = "test"; 
    short        owner      = 1;

    //add fields to msg
    mamaMsg_addString (mMsg, "name0", 101, testString);

    //Set mMsg owner
    ASSERT_EQ (mamaMsgImpl_setMessageOwner(mMsg, owner), MAMA_STATUS_OK);
}

TEST_F (MsgGeneralTestsC, DISABLED_msgImplSetMessageOwnerInValidMsg)
{
    const char*  testString = "test"; 
    short        owner      = (short) NULL;

    //add fields to msg
    mamaMsg_addString (mMsg, "name0", 101, testString);

    //Set mMsg owner
    ASSERT_EQ (mamaMsgImpl_setMessageOwner(NULL, owner), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgGeneralTestsC, DISABLED_msgImplSetMessageOwnerInValidOwner)
{
    const char*  testString = "test";

    //add fields to msg
    mamaMsg_addString (mMsg, "name0", 101, testString);

    //Set mMsg owner
    ASSERT_EQ (mamaMsgImpl_setMessageOwner(mMsg, 0), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgGeneralTestsC, DISABLED_msgIterateFieldsValid)
{
    mamaMsgIteratorCb    callback = NULL;
    const mamaDictionary dict     = NULL;
    void*                closure  = 0;
    
    //add fields to msg
    mamaMsg_addString (mMsg, "name0", 101, "test0");
    mamaMsg_addString (mMsg, "name1", 102, "test1");

    ASSERT_EQ (mamaMsg_iterateFields (mMsg, callback, dict, closure), MAMA_STATUS_OK);
}

TEST_F (MsgGeneralTestsC, msgIterateFieldsInValidMsg)
{
    mamaMsgIteratorCb    callback = NULL;
    const mamaDictionary dict     = NULL;
    void*                closure  = 0;

    //add fields to msg
    mamaMsg_addString (mMsg, "name0", 101, "test0");
    mamaMsg_addString (mMsg, "name1", 102, "test1");

    ASSERT_EQ (mamaMsg_iterateFields (NULL, callback, dict, closure), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgGeneralTestsC, msgIterateFieldsInValidCallback)
{
    const mamaDictionary dict     = NULL;
    void*                closure  = 0;

    //add fields to msg
    mamaMsg_addString (mMsg, "name0", 101, "test0");
    mamaMsg_addString (mMsg, "name1", 102, "test1");

    ASSERT_EQ (mamaMsg_iterateFields (mMsg, NULL, dict, closure), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgGeneralTestsC, msgIterateFieldsInValidDict)
{
    mamaMsgIteratorCb    callback = NULL;
    void*                closure  = 0;

    //add fields to msg
    mamaMsg_addString (mMsg, "name0", 101, "test0");
    mamaMsg_addString (mMsg, "name1", 102, "test1");

    ASSERT_EQ (mamaMsg_iterateFields (mMsg, callback, NULL, closure), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgGeneralTestsC, msgIterateFieldsInValidClosure)
{
    mamaMsgIteratorCb    callback = NULL;
    const mamaDictionary dict     = NULL;

    //add fields to msg
    mamaMsg_addString (mMsg, "name0", 101, "test0");
    mamaMsg_addString (mMsg, "name1", 102, "test1");

    ASSERT_EQ (mamaMsg_iterateFields (mMsg, callback, dict, 0), MAMA_STATUS_NULL_ARG);
}
 
TEST_F (MsgGeneralTestsC, msgImplSetQueueValid)
{
    mamaQueue       queue     = NULL;

    mama_getDefaultEventQueue(mMiddlewareBridge, &queue);
    
    //add fields to msg
    mamaMsg_addString (mMsg, "name0", 101, "test0");
    mamaMsg_addString (mMsg, "name1", 102, "test1");
    
    ASSERT_EQ (mamaMsgImpl_setQueue(mMsg, queue), MAMA_STATUS_OK);
}

TEST_F (MsgGeneralTestsC, msgImplSetQueueInValidMsg)
{
    mamaQueue      mQueue     = NULL;

    //add fields to msg
    mamaMsg_addString (mMsg, "name0", 101, "test0");
    mamaMsg_addString (mMsg, "name1", 102, "test1");
    
    ASSERT_EQ (mamaMsgImpl_setQueue(NULL, mQueue), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgGeneralTestsC, msgImplSetQueueInValidQueue)
{
    //add fields to msg
    mamaMsg_addString (mMsg, "name0", 101, "test0");
    mamaMsg_addString (mMsg, "name1", 102, "test1");
    
    ASSERT_EQ (mamaMsgImpl_setQueue(mMsg, NULL), MAMA_STATUS_INVALID_QUEUE);
}

/*
TEST_F (MsgGeneralTestsC, msgImplSetDqStrategyContextValid)
{
    mamaMsg          mMsg               = NULL;
    //mamaDqContext    dqStrategyContext  = NULL;
    const char*      testString         = "";

    //Create & add fields to msg
    mamaMsg_create (&mMsg);
    mamaMsg_addString (mMsg, "name0", 101, "test0");
    mamaMsg_addString (mMsg, "name1", 102, "test1");
    
    ASSERT_EQ (mamaMsgImpl_setDqStrategyContext(mMsg, &dqStrategyContext), MAMA_STATUS_OK);
}

TEST_F (MsgGeneralTestsC, msgImplSetDqStrategyContextInValidMsg)
{
    mamaMsg        mMsg               = NULL;
    //mamaDqContext  dqStrategyContext  = NULL;
    const char*    testString         = "";

    //Create & add fields to msg
    mamaMsg_create (&mMsg);
    mamaMsg_addString (mMsg, "name0", 101, "test0");
    mamaMsg_addString (mMsg, "name1", 102, "test1");
    
    ASSERT_EQ (mamaMsgImpl_setDqStrategyContext(NULL, &dqStrategyContext), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgGeneralTestsC, msgImplSetDqStrategyContextInValidDq)
{
    mamaMsg        mMsg               = NULL;
    //mamaDqContext  dqStrategyContext  = NULL;
    const char*    testString         = "";

    //Create & add fields to msg
    mamaMsg_create (&mMsg);
    mamaMsg_addString (mMsg, "name0", 101, "test0");
    mamaMsg_addString (mMsg, "name1", 102, "test1");
    
    ASSERT_EQ (mamaMsgImpl_setDqStrategyContext(mMsg, NULL), MAMA_STATUS_INVALID_ARG);
}
*/
TEST_F (MsgGeneralTestsC, msgGetSendSubjectValid)
{
    const char*       subjectIn  = "subject";
    const char*       subjectOut = NULL;
    mama_status       status;
   
    //add fields to msg
    mamaMsg_addString (mMsg, "name0", 101, "test0");
    mamaMsg_addString (mMsg, "name1", 102, "test1");

    mamaMsgImpl_setBridgeImpl (mMsg, mMiddlewareBridge);
    mamaMsgImpl_setSubscInfo (mMsg, NULL, NULL, subjectIn, 1);
    status = mamaMsg_getSendSubject(mMsg, &subjectOut);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (status, MAMA_STATUS_OK);
    EXPECT_STREQ(subjectIn, subjectOut);
}

TEST_F (MsgGeneralTestsC, msgGetSendSubjectInValidMsg)
{
    const char*       subject    = "";
   
    //add fields to msg
    mamaMsg_addString (mMsg, "name0", 101, "test0");
    mamaMsg_addString (mMsg, "name1", 102, "test1");
    
    ASSERT_EQ (mamaMsg_getSendSubject(NULL, &subject), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgGeneralTestsC, msgGetSendSubjectInValidSubject)
{
    //add fields to msg
    mamaMsg_addString (mMsg, "name0", 101, "test0");
    mamaMsg_addString (mMsg, "name1", 102, "test1");
    
    ASSERT_EQ (mamaMsg_getSendSubject(mMsg, NULL), MAMA_STATUS_INVALID_ARG);
}

TEST_F (MsgGeneralTestsC, msgImplGetSubjectValid)
{
    const char*       result    = "";
   
    //add fields to msg
    mamaMsg_addString (mMsg, "name0", 101, "test0");
    mamaMsg_addString (mMsg, "name1", 102, "test1");
    
    ASSERT_EQ (mamaMsgImpl_getSubject(mMsg, &result), MAMA_STATUS_OK);
}

TEST_F (MsgGeneralTestsC, msgImplGetSubjectInValidMsg)
{
    const char*       result    = "";
   
    //add fields to msg
    mamaMsg_addString (mMsg, "name0", 101, "test0");
    mamaMsg_addString (mMsg, "name1", 102, "test1");
    
    ASSERT_EQ (mamaMsgImpl_getSubject(NULL, &result), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgGeneralTestsC, msgImplGetSubjectInValidResult)
{
    //add fields to msg
    mamaMsg_addString (mMsg, "name0", 101, "test0");
    mamaMsg_addString (mMsg, "name1", 102, "test1");
    
    ASSERT_EQ (mamaMsgImpl_getSubject(mMsg, NULL), MAMA_STATUS_INVALID_ARG);
}

TEST_F (MsgGeneralTestsC, msgImplGetStatusValid)
{
    mamaMsgStatus    status       = MAMA_MSG_STATUS_UNKNOWN;

    //add fields to msg
    mamaMsg_addString (mMsg, "name0", 101, "test0");
    mamaMsg_addString (mMsg, "name1", 102, "test1");

    ASSERT_EQ (mamaMsgImpl_getStatus(mMsg, &status), MAMA_STATUS_OK);
}

TEST_F (MsgGeneralTestsC, msgImplGetStatusInValidMsg)
{
    mamaMsgStatus    status       = MAMA_MSG_STATUS_UNKNOWN;

    //add fields to msg
    mamaMsg_addString (mMsg, "name0", 101, "test0");
    mamaMsg_addString (mMsg, "name1", 102, "test1");

    ASSERT_EQ (mamaMsgImpl_getStatus(NULL, &status), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgGeneralTestsC, msgImplGetStatusInValidStatus)
{
    mamaMsg          mMsg         = NULL;

    //add fields to msg
    mamaMsg_addString (mMsg, "name0", 101, "test0");
    mamaMsg_addString (mMsg, "name1", 102, "test1");

    ASSERT_EQ (mamaMsgImpl_getStatus(mMsg, NULL), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgGeneralTestsC, msgImplGetStatusFromMsgValid)
{
    mama_i32_t    result       = 0;
    mamaMsgStatus status       = MAMA_MSG_STATUS_UNKNOWN;
   
    //add fields to msg
    mamaMsg_addI32 (mMsg, "name0", 101, 8);
    mamaMsg_getI32 (mMsg, "Name", 100, &result);

    mamaMsgImpl_getStatus(mMsg, &status);

    mamaMsg_addI32 (mMsg,
    		MamaFieldMsgStatus.mName, MamaFieldMsgStatus.mFid, status);

    ASSERT_EQ (mamaMsgImpl_getStatusFromMsg(mMsg), status);
}

TEST_F (MsgGeneralTestsC, msgImplGetStatusFromMsgInValid)
{
    //add fields to msg
    mamaMsg_addString (mMsg, "name0", 101, "test0");
    mamaMsg_addString (mMsg, "name1", 102, "test1");

    ASSERT_EQ (mamaMsgImpl_getStatusFromMsg(mMsg), MAMA_MSG_STATUS_UNKNOWN);
}

TEST_F (MsgGeneralTestsC, msgGetDateTimeMSecValid)
{
    const char*          name         = "test";
    mama_fid_t           fid          = 102;
    mama_u64_t           milliseconds = 0;
    mamaDateTime         dateTime     = NULL;

    //add fields to msg
    mamaDateTime_create(&dateTime);
    mamaDateTime_setToNow(dateTime);

    mamaMsg_addDateTime(mMsg, name, fid, dateTime);

    ASSERT_EQ (mamaMsg_getDateTimeMSec(mMsg, name, fid, &milliseconds), MAMA_STATUS_OK);

    mamaDateTime_destroy(dateTime);
}

TEST_F (MsgGeneralTestsC, DISABLED_msgGetDateTimeMSecInValidMsg)
{
    mamaMsg              mMsg         = NULL;
    const char*          name         = "";
    mama_fid_t           fid          = 0;
    mama_u64_t*          milliseconds = 0;
    mamaDateTime         dateTime     = NULL;
    mamaDateTime         m_out        = NULL;

    //add fields to msg
    mamaDateTime_create(&dateTime);
    mamaDateTime_create(&m_out);
    mamaDateTime_setToNow(dateTime);

    mamaMsg_addDateTime(mMsg, NULL, 102, dateTime);
    mamaMsg_getDateTime(mMsg, NULL, 102, m_out);

    ASSERT_EQ (mamaMsg_getDateTimeMSec(NULL, name, fid, milliseconds), MAMA_STATUS_NULL_ARG); 

    mamaDateTime_destroy(dateTime);
    mamaDateTime_destroy(m_out);
}

TEST_F (MsgGeneralTestsC, msgGetDateTimeMSecInValidName)
{
    mama_fid_t           fid          = 102;
    mama_u64_t           milliseconds = 0;
    mamaDateTime         dateTime     = NULL;
    mamaDateTime         m_out        = NULL;

    //add fields to msg
    mamaDateTime_create(&dateTime);
    mamaDateTime_create(&m_out);
    mamaDateTime_setToNow(dateTime);
    
    mamaMsg_addDateTime(mMsg, NULL, fid, dateTime);
    mamaMsg_getDateTime(mMsg, NULL, fid, m_out);

    /* Invalid name is ok, as long as fid is specified */
    ASSERT_EQ (MAMA_STATUS_OK, mamaMsg_getDateTimeMSec(mMsg, NULL, fid, &milliseconds));

    mamaDateTime_destroy(dateTime);
    mamaDateTime_destroy(m_out);
}

TEST_F (MsgGeneralTestsC, msgGetDateTimeMSecInValidFid)
{
    const char*          name         = "";
    mama_u64_t*          milliseconds = 0;
    mamaDateTime         dateTime     = NULL;
    mamaDateTime         m_out        = NULL;

    //add fields to msg
    mamaDateTime_create(&dateTime);
    mamaDateTime_create(&m_out);
    mamaDateTime_setToNow(dateTime);

    mamaMsg_addDateTime(mMsg, NULL, 102, dateTime);
    mamaMsg_getDateTime(mMsg, NULL, 102, m_out);

    ASSERT_EQ (mamaMsg_getDateTimeMSec(mMsg, name, 0, milliseconds), MAMA_STATUS_INVALID_ARG);

    mamaDateTime_destroy(dateTime);
    mamaDateTime_destroy(m_out);
}

TEST_F (MsgGeneralTestsC, msgGetDateTimeMSecInValidMilliseconds)
{
    const char*          name         = "";
    mama_fid_t           fid          = 0;
    mamaDateTime         dateTime     = NULL;
    mamaDateTime         m_out        = NULL;

    //add fields to msg
    mamaDateTime_create(&dateTime);
    mamaDateTime_create(&m_out);
    mamaDateTime_setToNow(dateTime);

    mamaMsg_addDateTime(mMsg, NULL, 102, dateTime);
    mamaMsg_getDateTime(mMsg, NULL, 102, m_out);

    ASSERT_EQ (mamaMsg_getDateTimeMSec(mMsg, name, fid, 0), MAMA_STATUS_INVALID_ARG);

    mamaDateTime_destroy(dateTime);
    mamaDateTime_destroy(m_out);
}


TEST_F (MsgGeneralTestsC, msgGetEntitleCodeValid)
{
    mama_i32_t             entitleCode  = 100;

    //add fields to msg
    mamaMsg_addI32 (mMsg, ENTITLE_FIELD_NAME, ENTITLE_FIELD_ID, entitleCode);

    ASSERT_EQ (mamaMsg_getEntitleCode(mMsg, &entitleCode), MAMA_STATUS_OK);
}

TEST_F (MsgGeneralTestsC, msgGetEntitleCodeInValidMsg)
{
    mama_i32_t       entitleCode  = 0;

    //add fields to msg
    mamaMsg_addString (mMsg, "name", 100, "test");

    ASSERT_EQ (mamaMsg_getEntitleCode(NULL, &entitleCode), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgGeneralTestsC, DISABLED_msgGetEntitleCodeInValidEntitleCode)
{
    //add fields to msg
    mamaMsg_addString (mMsg, "name", 100, "test");

    ASSERT_EQ (mamaMsg_getEntitleCode(mMsg, NULL), MAMA_STATUS_INVALID_ARG);
}

TEST_F (MsgGeneralTestsC, msgGetFieldAsStringValid)
{
    const char*          fieldName    = "name1";
    mama_fid_t           fid          = 101;
    char                 buffer[100];
    mama_size_t          bufferLength = 100;
   
    //add fields to msg
    mamaMsg_addBool (mMsg, fieldName, fid, true);
  
    ASSERT_EQ (mamaMsg_getFieldAsString(mMsg, fieldName, fid, buffer, bufferLength), MAMA_STATUS_OK);
}

TEST_F (MsgGeneralTestsC, msgGetFieldAsStringInValidMsg)
{
    const char*          fieldName    = "";
    mama_fid_t           fid          = 0;
    char*                buffer       = NULL;
    mama_size_t          bufferLength = 0;
   
    //add fields to msg
    mamaMsg_addBool (mMsg, "name1", 101, true);
  
    ASSERT_EQ (mamaMsg_getFieldAsString(NULL, fieldName, fid, buffer, bufferLength), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgGeneralTestsC, msgGetFieldAsStringInValidFieldName)
{
    mama_fid_t           fid          = 0;
    char*                buffer       = NULL;
    mama_size_t          bufferLength = 0;
   
    //add fields to msg
    mamaMsg_addBool (mMsg, "name1", 101, true);
  
    ASSERT_EQ (mamaMsg_getFieldAsString(mMsg, NULL, fid, buffer, bufferLength), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgGeneralTestsC, msgGetFieldAsStringInValidFid)
{
    mamaMsg              mMsg         = NULL;
    const char*          fieldName    = "";
    char*                buffer       = NULL;
    mama_size_t          bufferLength = 0;
   
    //add fields to msg
    mamaMsg_addBool (mMsg, "name1", 101, true);
  
    ASSERT_EQ (mamaMsg_getFieldAsString(mMsg, fieldName, 0, buffer, bufferLength), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgGeneralTestsC, msgGetFieldAsStringInValidBuffer)
{
    const char*          fieldName    = "";
    mama_fid_t           fid          = 0;
    mama_size_t          bufferLength = 0;
   
    //add fields to msg
    mamaMsg_addBool (mMsg, "name1", 101, true);
  
    ASSERT_EQ (mamaMsg_getFieldAsString(mMsg, fieldName, fid, NULL, bufferLength), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgGeneralTestsC, msgGetFieldAsStringInValidBufferLength)
{
    const char*          fieldName    = "";
    mama_fid_t           fid          = 0;
    char*                buffer       = NULL;
   
    //add fields to msg
    mamaMsg_addBool (mMsg, "name1", 101, true);
  
    ASSERT_EQ (mamaMsg_getFieldAsString(mMsg, fieldName, fid, buffer, 0), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgGeneralTestsC, msgGetFieldAsStringLargeVectorLargeBuffer)
{
    const char*          fieldName    = "name2";
    mama_fid_t           fid          = 102;
    char                 buffer[200];
    mama_size_t          bufferLen    = 200;
    int                  i            = 1;

    mamaMsg              msgs[5];

    for (i = 0; i < 5; ++i)
    {
        ASSERT_EQ (MAMA_STATUS_OK, mamaMsg_create (&msgs[i]));
        ASSERT_EQ (MAMA_STATUS_OK,
                    mamaMsg_addBool (msgs[i], "field1", 101, true));
    }

    // Add fields to msg
    ASSERT_EQ (MAMA_STATUS_OK, mamaMsg_addBool (mMsg, "name1", 101, true));
    ASSERT_EQ (MAMA_STATUS_OK,
                    mamaMsg_addVectorMsg (mMsg, fieldName, fid, msgs, 5));

    EXPECT_EQ (MAMA_STATUS_OK,
                mamaMsg_getFieldAsString(mMsg, fieldName, fid, buffer, bufferLen));

    // Cleanup message array.
    for (i = 0; i < 5; ++i)
    {
        ASSERT_EQ (MAMA_STATUS_OK, mamaMsg_destroy (msgs[i]));
    }
}

TEST_F (MsgGeneralTestsC, msgGetFieldAsStringLargeVectorSmallBuffer)
{
    const char*          fieldName    = "name2";
    mama_fid_t           fid          = 102;
    char                 buffer[10];
    mama_size_t          bufferLen    = 10;
    int                  i            = 1;

    mamaMsg              msgs[5];

    for (i = 0; i < 5; ++i)
    {
        ASSERT_EQ (MAMA_STATUS_OK, mamaMsg_create (&msgs[i]));
        ASSERT_EQ (MAMA_STATUS_OK,
                    mamaMsg_addBool (msgs[i], "field1", 101, true));
    }

    // Add fields to msg
    ASSERT_EQ (MAMA_STATUS_OK, mamaMsg_addBool (mMsg, "name1", 101, true));
    ASSERT_EQ (MAMA_STATUS_OK,
                    mamaMsg_addVectorMsg (mMsg, fieldName, fid, msgs, 5));

    EXPECT_EQ (MAMA_STATUS_OK,
                mamaMsg_getFieldAsString (mMsg, fieldName, fid, buffer, bufferLen));

    // Cleanup message array.
    for (i = 0; i < 5; ++i)
    {
        ASSERT_EQ (MAMA_STATUS_OK, mamaMsg_destroy (msgs[i]));
    }
}


TEST_F (MsgGeneralTestsC, msgSetNewBufferValid)
{
    const void*          buffer       = NULL;
    mama_size_t          size         = 0;
   
    //add fields to msg
    mamaMsg_addBool (mMsg, "name1", 101, true);

    mamaMsg_getByteBuffer (mMsg, &buffer, &size);
 
    ASSERT_EQ (mamaMsg_setNewBuffer(mMsg, (void*)buffer, size), MAMA_STATUS_OK);
}

TEST_F (MsgGeneralTestsC, msgSetNewBufferInValidMsg)
{
    const void*          buffer       = NULL;
    mama_size_t          size         = 0;
   
    //add fields to msg
    mamaMsg_addBool (mMsg, "name1", 101, true);

    mamaMsg_getByteBuffer (mMsg, &buffer, &size);
 
    ASSERT_EQ (mamaMsg_setNewBuffer(NULL, &buffer, size), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgGeneralTestsC, msgSetNewBufferInValidBuffer)
{
    mamaMsg              mMsg         = NULL;
    const void*          buffer       = NULL;
    mama_size_t          size         = 0;
   
    //add fields to msg
    mamaMsg_addBool (mMsg, "name1", 101, true);

    mamaMsg_getByteBuffer (mMsg, &buffer, &size);
 
    ASSERT_EQ (mamaMsg_setNewBuffer(mMsg, 0, size), MAMA_STATUS_NULL_ARG);
}

TEST_F (MsgGeneralTestsC, DISABLED_msgSetNewBufferInValidSize)
{
    const void*          buffer       = NULL;
    mama_size_t          size         = 0;
   
    //add fields to msg
    mamaMsg_addBool (mMsg, "name1", 101, true);

    mamaMsg_getByteBuffer (mMsg, &buffer, &size);
 
    ASSERT_EQ (mamaMsg_setNewBuffer(mMsg, &buffer, 0), MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgGeneralTestsC, msgToNormalizedString)
{
    //add fields to msg
    mamaMsg_addBool(mMsg, "name01", 101, true);
    mamaMsg_addChar(mMsg, "name02", 102, 'F');
    mamaMsg_addU8(mMsg, "name03", 103, UINT8_MAX);
    mamaMsg_addU16(mMsg, "name04", 104, UINT16_MAX);
    mamaMsg_addU32(mMsg, "name05", 105, UINT32_MAX);
    mamaMsg_addU64(mMsg, "name06", 106, UINT64_MAX);
    mamaMsg_addI8(mMsg, "name07", 107, INT8_MAX);
    mamaMsg_addI16(mMsg, "name08", 108, INT16_MAX);
    mamaMsg_addI32(mMsg, "name09", 109, INT32_MAX);
    mamaMsg_addI64(mMsg, "name10", 110, INT64_MAX);
    mamaMsg_addF32(mMsg, "name11", 111, 0.0001F);
    mamaMsg_addF64(mMsg, "name12", 112, 1000000.0L);
    mamaMsg_addString(mMsg, "name13", 113, "banana");
    mamaMsg_addOpaque(mMsg, "name14", 114, (void*)"banana", 7);

    mamaPrice price;
    mamaPrice_create(&price);
    mamaPrice_setValue(price, 0.99F);
    mamaMsg_addPrice(mMsg, "name15", 115, price);

    mamaDateTime dateTime;
    mamaDateTime_create(&dateTime);
    mamaDateTime_setFromString(dateTime, "2013-07-04 10:03:21.123");
    mamaMsg_addDateTime(mMsg, "name16", 116, dateTime);

    mama_u8_t u8vector[2];
    u8vector[0] = 0;
    u8vector[1] = UINT8_MAX;
    mamaMsg_addVectorU8(mMsg, "name17", 117, u8vector, 2);

    mama_u16_t u16vector[2];
    u16vector[0] = 0;
    u16vector[1] = UINT16_MAX;
    mamaMsg_addVectorU16(mMsg, "name18", 118, u16vector, 2);

    mama_u32_t u32vector[2];
    u32vector[0] = 0;
    u32vector[1] = UINT32_MAX;
    mamaMsg_addVectorU32(mMsg, "name19", 119, u32vector, 2);

    mama_u64_t u64vector[2];
    u64vector[0] = 0;
    u64vector[1] = UINT64_MAX;
    mamaMsg_addVectorU64(mMsg, "name20", 120, u64vector, 2);

    mama_i8_t i8vector[2];
    i8vector[0] = 0;
    i8vector[1] = INT8_MAX;
    mamaMsg_addVectorI8(mMsg, "name21", 121, i8vector, 2);

    mama_i16_t i16vector[2];
    i16vector[0] = 0;
    i16vector[1] = INT16_MAX;
    mamaMsg_addVectorI16(mMsg, "name22", 122, i16vector, 2);

    mama_i32_t i32vector[2];
    i32vector[0] = 0;
    i32vector[1] = INT32_MAX;
    mamaMsg_addVectorI32(mMsg, "name23", 123, i32vector, 2);

    mama_i64_t i64vector[2];
    i64vector[0] = 0;
    i64vector[1] = INT64_MAX;
    mamaMsg_addVectorI64(mMsg, "name24", 124, i64vector, 2);

    mama_bool_t iboolvector[2];
    iboolvector[0] = 0;
    iboolvector[1] = 1;
    mamaMsg_addVectorBool(mMsg, "name25", 125, iboolvector, 2);

    char icharvector[4];
    icharvector[0] = 'M';
    icharvector[1] = 'A';
    icharvector[2] = 'M';
    icharvector[3] = 'A';
    mamaMsg_addVectorChar(mMsg, "name26", 126, icharvector, 4);

    mama_f32_t f32vector[2];
    f32vector[0] = -0.04;
    f32vector[1] = 1099.99;
    mamaMsg_addVectorF32(mMsg, "name27", 127, f32vector, 2);

    mama_f64_t f64vector[2];
    f64vector[0] = -100.0;
    f64vector[1] = 100.0;
    mamaMsg_addVectorF64(mMsg, "name28", 128, f64vector, 2);

    mamaMsg msg;
    mamaMsg_create(&msg);
    mamaMsg_addString(msg, "msg1", 1001, "val1");

    mamaMsg_addMsg(mMsg, "name30", 130, msg);

    mamaMsg msg2;
    mamaMsg_create(&msg2);
    mamaMsg_addString(msg2, "msg2", 1002, "val2");

    mamaMsg msgVector[2];
    msgVector[0] = msg;
    msgVector[1] = msg2;
    mamaMsg_addVectorMsg(mMsg, "name31", 131, msgVector, 2);

    const char* actual = mamaMsg_toNormalizedString(mMsg);

    const char* expected =
        "{"
        "name01[101]=true,"
        "name02[102]=F,"
        "name03[103]=255,"
        "name04[104]=65535,"
        "name05[105]=4294967295,"
        "name06[106]=18446744073709551615,"
        "name07[107]=127,"
        "name08[108]=32767,"
        "name09[109]=2147483647,"
        "name10[110]=9223372036854775807,"
        "name11[111]=0.0001,"
        "name12[112]=1000000.0000,"
        "name13[113]=banana,"
        "name14[114]=0x62616e616e61,"
        "name15[115]=0.990000,"
        "name16[116]=2013-07-04 10:03:21.123,"
        "name17[117]={0,255},"
        "name18[118]={0,65535},"
        "name19[119]={0,4294967295},"
        "name20[120]={0,18446744073709551615},"
        "name21[121]={0,127},"
        "name22[122]={0,32767},"
        "name23[123]={0,2147483647},"
        "name24[124]={0,9223372036854775807},"
        "name25[125]={false,true},"
        "name26[126]={M,A,M,A},"
        "name27[127]={-0.0400,1099.9900},"
        "name28[128]={-100.0000,100.0000},"
        "name30[130]={msg1[1001]=val1},"
        "name31[131]={{msg1[1001]=val1},{msg2[1002]=val2}}"
        "}";

    ASSERT_STREQ(expected, actual);

    mamaMsg_destroy(msg);
    mamaMsg_destroy(msg2);
    mamaPrice_destroy(price);
    mamaDateTime_destroy(dateTime);
}

TEST_F(MsgGeneralTestsC, msgToJsonString)
{
    //add fields to msg
    mamaMsg_addBool(mMsg, "name01", 101, true);
    mamaMsg_addChar(mMsg, "name02", 102, 'F');
    mamaMsg_addU8(mMsg, "name03", 103, UINT8_MAX);
    mamaMsg_addU16(mMsg, "name04", 104, UINT16_MAX);
    mamaMsg_addU32(mMsg, "name05", 105, UINT32_MAX);
    mamaMsg_addU64(mMsg, "name06", 106, UINT64_MAX);
    mamaMsg_addI8(mMsg, "name07", 107, INT8_MAX);
    mamaMsg_addI16(mMsg, "name08", 108, INT16_MAX);
    mamaMsg_addI32(mMsg, "name09", 109, INT32_MAX);
    mamaMsg_addI64(mMsg, "name10", 110, INT64_MAX);
    mamaMsg_addF32(mMsg, "name11", 111, 0.0001F);
    mamaMsg_addF64(mMsg, "name12", 112, 1000000.0L);
    mamaMsg_addString(mMsg, "name13", 113, "banana");
    mamaMsg_addOpaque(mMsg, "name14", 114, (void*)"banana", 7);

    mamaPrice price;
    mamaPrice_create(&price);
    mamaPrice_setValue(price, 0.99F);
    mamaMsg_addPrice(mMsg, "name15", 115, price);

    mamaDateTime dateTime;
    mamaDateTime_create(&dateTime);
    mamaDateTime_setFromString(dateTime, "2013-07-04 10:03:21.123456");
    mamaMsg_addDateTime(mMsg, "name16", 116, dateTime);

    mama_u8_t u8vector[2];
    u8vector[0] = 0;
    u8vector[1] = UINT8_MAX;
    mamaMsg_addVectorU8(mMsg, "name17", 117, u8vector, 2);

    mama_u16_t u16vector[2];
    u16vector[0] = 0;
    u16vector[1] = UINT16_MAX;
    mamaMsg_addVectorU16(mMsg, "name18", 118, u16vector, 2);

    mama_u32_t u32vector[2];
    u32vector[0] = 0;
    u32vector[1] = UINT32_MAX;
    mamaMsg_addVectorU32(mMsg, "name19", 119, u32vector, 2);

    mama_u64_t u64vector[2];
    u64vector[0] = 0;
    u64vector[1] = UINT64_MAX;
    mamaMsg_addVectorU64(mMsg, "name20", 120, u64vector, 2);

    mama_i8_t i8vector[2];
    i8vector[0] = 0;
    i8vector[1] = INT8_MAX;
    mamaMsg_addVectorI8(mMsg, "name21", 121, i8vector, 2);

    mama_i16_t i16vector[2];
    i16vector[0] = 0;
    i16vector[1] = INT16_MAX;
    mamaMsg_addVectorI16(mMsg, "name22", 122, i16vector, 2);

    mama_i32_t i32vector[2];
    i32vector[0] = 0;
    i32vector[1] = INT32_MAX;
    mamaMsg_addVectorI32(mMsg, "name23", 123, i32vector, 2);

    mama_i64_t i64vector[2];
    i64vector[0] = 0;
    i64vector[1] = INT64_MAX;
    mamaMsg_addVectorI64(mMsg, "name24", 124, i64vector, 2);

    mama_bool_t iboolvector[2];
    iboolvector[0] = 0;
    iboolvector[1] = 1;
    mamaMsg_addVectorBool(mMsg, "name25", 125, iboolvector, 2);

    char icharvector[4];
    icharvector[0] = 'M';
    icharvector[1] = 'A';
    icharvector[2] = 'M';
    icharvector[3] = 'A';
    mamaMsg_addVectorChar(mMsg, "name26", 126, icharvector, 4);

    mama_f32_t f32vector[2];
    f32vector[0] = -0.04;
    f32vector[1] = 1099.99;
    mamaMsg_addVectorF32(mMsg, "name27", 127, f32vector, 2);

    mama_f64_t f64vector[2];
    f64vector[0] = -100.0;
    f64vector[1] = 100.0;
    mamaMsg_addVectorF64(mMsg, "name28", 128, f64vector, 2);

    mamaMsg msg;
    mamaMsg_create(&msg);
    mamaMsg_addString(msg, "msg1", 1001, "val1");

    mamaMsg_addMsg(mMsg, "name30", 130, msg);

    mamaMsg msg2;
    mamaMsg_create(&msg2);
    mamaMsg_addString(msg2, "msg2", 1002, "val2");

    mamaMsg msgVector[2];
    msgVector[0] = msg;
    msgVector[1] = msg2;
    mamaMsg_addVectorMsg(mMsg, "name31", 131, msgVector, 2);

    const char* actual = mamaMsg_toJsonString(mMsg);

    const char* expected =
        "{"
        "\"name01\":true,"
        "\"name02\":\"F\","
        "\"name03\":255,"
        "\"name04\":65535,"
        "\"name05\":4294967295,"
        "\"name06\":18446744073709551615,"
        "\"name07\":127,"
        "\"name08\":32767,"
        "\"name09\":2147483647,"
        "\"name10\":9223372036854775807,"
        "\"name11\":0.0001,"
        "\"name12\":1000000.0000,"
        "\"name13\":\"banana\","
        "\"name14\":\"0x62616e616e61\","
        "\"name15\":0.990000,"
        "\"name16\":\"2013-07-04 10:03:21.123456\","
        "\"name17\":[0,255],"
        "\"name18\":[0,65535],"
        "\"name19\":[0,4294967295],"
        "\"name20\":[0,18446744073709551615],"
        "\"name21\":[0,127],"
        "\"name22\":[0,32767],"
        "\"name23\":[0,2147483647],"
        "\"name24\":[0,9223372036854775807],"
        "\"name25\":[false,true],"
        "\"name26\":[\"M\",\"A\",\"M\",\"A\"],"
        "\"name27\":[-0.0400,1099.9900],"
        "\"name28\":[-100.0000,100.0000],"
        "\"name30\":{\"msg1\":\"val1\"},"
        "\"name31\":[{\"msg1\":\"val1\"},{\"msg2\":\"val2\"}]"
        "}";

    ASSERT_STREQ(expected, actual);

    mamaMsg_destroy(msg);
    mamaMsg_destroy(msg2);
    mamaPrice_destroy(price);
    mamaDateTime_destroy(dateTime);
}

/*
 * COPY TEST SUITE
 * ****************************************************************************
 */
class MsgCopyTests : public MsgGeneralTestsC
{
protected:
    mamaMsg     mCopy;

    MsgCopyTests()
        : mCopy (NULL)
    {}
    ~MsgCopyTests() {};

    virtual void SetUp (void)
    {
        MsgGeneralTestsC::SetUp();
    }

    virtual void TearDown (void)
    {
        MsgGeneralTestsC::TearDown();
    }
};

/**
 * Copy test fixtures
 * ****************************************************************************
 */
TEST_F(MsgCopyTests, Copy)
{
    mStatus = mamaMsg_create (&mCopy);
    mStatus = mamaMsg_copy (mMsg, &mCopy);
    ASSERT_EQ (mStatus, MAMA_STATUS_OK);
    mamaMsg_destroy (mCopy);
}

TEST_F(MsgCopyTests, CopyNoCreate)
{
    mStatus = mamaMsg_copy (mMsg, &mCopy);
    ASSERT_EQ (mStatus, MAMA_STATUS_OK);
    mamaMsg_destroy (mCopy);
}

TEST_F(MsgCopyTests, CopyNullMsg)
{
    mStatus = mamaMsg_copy (NULL, &mCopy);
    ASSERT_EQ (mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgCopyTests, CopySameMsg)
{
    mStatus = mamaMsg_copy (mMsg, &mMsg);
    ASSERT_EQ (mStatus, MAMA_STATUS_INVALID_ARG);
}

TEST_F(MsgCopyTests, DISABLED_CopyNullCopy)
{
    /* This cores as we always dereference copy */
    mStatus = mamaMsg_copy (mMsg, NULL);
    ASSERT_EQ (mStatus, MAMA_STATUS_NULL_ARG);
}

/**
 * TempCopy test fixtures
 * ****************************************************************************
 */
TEST_F(MsgCopyTests, TempCopy)
{
    mStatus = mamaMsg_getTempCopy (mMsg, &mCopy);
    ASSERT_EQ (mStatus, MAMA_STATUS_OK);
}

TEST_F(MsgCopyTests, DISABLED_TempCopyNullMsg)
{
    /* Cores as there is no NULL checking on src */
    mStatus = mamaMsg_getTempCopy (NULL, &mCopy);
    ASSERT_EQ (mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgCopyTests, DISABLED_TempCopyNullCopy)
{
    /* Cores as there is no NULL checking on copy */
    mStatus = mamaMsg_getTempCopy (mMsg, NULL);
    ASSERT_EQ (mStatus, MAMA_STATUS_NULL_ARG);
}

/*
 * GETBYTESIZE TEST SUITE
 * ****************************************************************************
 */
class MsgGetByteSizeTests : public MsgGeneralTestsC
{
protected:
    mama_size_t mSize;

    MsgGetByteSizeTests()
        : mSize (0)
    {}
    ~MsgGetByteSizeTests() {};

    virtual void SetUp (void)
    {
        MsgGeneralTestsC::SetUp();
    }

    virtual void TearDown (void)
    {
        MsgGeneralTestsC::TearDown();
    }
};

/**
 * GetByteSize test fixtures
 * ****************************************************************************
 */
TEST_F(MsgGetByteSizeTests, GetByteSize)
{
    mStatus = mamaMsg_getByteSize (mMsg, &mSize);
    ASSERT_EQ (mStatus, MAMA_STATUS_OK);
}

TEST_F(MsgGetByteSizeTests, GetByteSizeNullMsg)
{
    mStatus = mamaMsg_getByteSize (NULL, &mSize);
    ASSERT_EQ (mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgGetByteSizeTests, GetByteSizeNullSize)
{
    mStatus = mamaMsg_getByteSize (mMsg, NULL);
    ASSERT_EQ (mStatus, MAMA_STATUS_NULL_ARG);
}

/*
 * APPLYMSG TEST SUITE
 * ****************************************************************************
 */
class MsgApplyMsgTests : public MsgGeneralTestsC
{
protected:
    mamaMsg mApplyMsg;

    MsgApplyMsgTests()
        : mApplyMsg (NULL)
    {
    }
    ~MsgApplyMsgTests() {};

    virtual void SetUp (void)
    {
        MsgGeneralTestsC::SetUp();
        mamaMsg_create (&mApplyMsg);
        mamaMsg_addU8 (mApplyMsg, NULL, 1, 255);
    }

    virtual void TearDown (void)
    {
        MsgGeneralTestsC::TearDown();
        mamaMsg_destroy (mApplyMsg);
    }
};

/**
 * ApplyMsg test fixtures
 * ****************************************************************************
 */
TEST_F(MsgApplyMsgTests, ApplyMsg)
{
    mStatus = mamaMsg_applyMsg (mMsg, mApplyMsg);
    ASSERT_EQ (mStatus, MAMA_STATUS_OK);
}

TEST_F(MsgApplyMsgTests, ApplyMsgNullMsg)
{
    mStatus = mamaMsg_applyMsg (NULL, mApplyMsg);
    ASSERT_EQ (mStatus, MAMA_STATUS_NULL_ARG);
}

TEST_F(MsgApplyMsgTests, DISABLED_ApplyMsgNullApply)
{
    /* Cores as there is no NULL checking on src */
    mStatus = mamaMsg_applyMsg (mMsg, NULL);
    ASSERT_EQ (mStatus, MAMA_STATUS_NULL_ARG);
}

