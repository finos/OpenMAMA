/* $Id: middlewareMsgTests.cpp,v 1.1.2.6 2013/02/01 17:26:58 matthewmulhern Exp $
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
#include "mama/mama.h"
#include "MainUnitTestC.h"
#include <iostream>
#include "bridge.h"
#include "mama/types.h"

using std::cout;
using std::endl;


class MiddlewareMsgTests : public ::testing::Test
{
protected:
    MiddlewareMsgTests(void);
    virtual ~MiddlewareMsgTests(void);

    virtual void SetUp(void);
    virtual void TearDown(void);

    mamaBridge mBridge;
};

MiddlewareMsgTests::MiddlewareMsgTests(void)
{
}

MiddlewareMsgTests::~MiddlewareMsgTests(void)
{
}

void MiddlewareMsgTests::SetUp(void)
{
	mama_loadBridge (&mBridge,getMiddleware());
    mama_open();
}

void MiddlewareMsgTests::TearDown(void)
{
    mama_close( );
}



/*===================================================================
 =               mamaMsg bridge function pointers                   =
 ====================================================================*/
TEST_F (MiddlewareMsgTests, create)
{
    msgBridge msg  = NULL;
    mamaMsg parent = NULL;

    mamaMsg_create(&parent);

    ASSERT_EQ (MAMA_STATUS_OK, 
               mBridge->bridgeMamaMsgCreate(&msg,parent));
}

TEST_F (MiddlewareMsgTests, createInvalidMsgBridge)
{
    mamaMsg parent = (mamaMsg) NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaMsgCreate(NULL,parent));
}

TEST_F (MiddlewareMsgTests, createInvalidMsg)
{
    msgBridge msg = (msgBridge) NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaMsgCreate(&msg,NULL));
}

TEST_F (MiddlewareMsgTests, isFromInbox)
{
    msgBridge msg  = (msgBridge) NOT_NULL;
    mamaMsg parent = (mamaMsg)   NOT_NULL;

    mamaMsg_create(&parent);

    ASSERT_EQ (MAMA_STATUS_OK, 
               mBridge->bridgeMamaMsgCreate(&msg,parent));
    
    ASSERT_EQ (0, 
               mBridge->bridgeMamaMsgIsFromInbox(msg));

}

TEST_F (MiddlewareMsgTests, isFromInboxInvalid)
{
    ASSERT_EQ (-1,
               mBridge->bridgeMamaMsgIsFromInbox(NULL));
}


TEST_F (MiddlewareMsgTests, destroy)
{
    msgBridge msg  = NULL;
    mamaMsg parent = NULL;
    int destroyMsg = 1;

    ASSERT_EQ(MAMA_STATUS_OK,mamaMsg_create(&parent));

    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaMsgCreate(&msg,parent));

    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaMsgDestroy(msg,destroyMsg));
}

TEST_F (MiddlewareMsgTests, destroyInvalidMsgBridge)
{
    int destroyMsg = 0;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaMsgDestroy(NULL,destroyMsg));
}

TEST_F (MiddlewareMsgTests, destroyMiddlewareMsgInvalid)
{
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaMsgDestroyMiddlewareMsg(NULL));
}


TEST_F (MiddlewareMsgTests, detach)
{
    msgBridge   msg         = NULL;
    mamaMsg     parent      = NULL;
    int         destroyMsg  = 1;

    ASSERT_EQ(MAMA_STATUS_OK,mamaMsg_create(&parent));

    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaMsgCreate(&msg,parent));

    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaMsgDetach(msg));

    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaMsgDestroy(msg,destroyMsg));
}

TEST_F (MiddlewareMsgTests, detachInvalid)
{
    ASSERT_EQ (MAMA_STATUS_NULL_ARG,  
               mBridge->bridgeMamaMsgDetach(NULL));
}

TEST_F (MiddlewareMsgTests, getPlatformError)
{
    msgBridge msg        = NULL;
    mamaMsg   parent     = NULL;
    int       destroyMsg = 1;
    void*     error      = NULL;
    mama_status status   = MAMA_STATUS_OK;

    ASSERT_EQ(MAMA_STATUS_OK,mamaMsg_create(&parent));

    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaMsgCreate(&msg,parent));
    

    status = mBridge->bridgeMamaMsgGetPlatformError(msg, &error);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ(MAMA_STATUS_OK, status);
    
    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaMsgDestroy(msg,destroyMsg));

}

TEST_F (MiddlewareMsgTests, getPlatformErrorInvalidMsgBridge)
{
    void* error = NOT_NULL;
    mama_status status = MAMA_STATUS_OK;

    status = mBridge->bridgeMamaMsgGetPlatformError(NULL,&error);
    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, status);
}

TEST_F (MiddlewareMsgTests, getPlatformErrorInvalidError)
{
    msgBridge msg = (msgBridge) NOT_NULL;
    mama_status status = MAMA_STATUS_OK;

    status = mBridge->bridgeMamaMsgGetPlatformError(msg,NULL);
    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, status);
}

TEST_F (MiddlewareMsgTests,setSendSubject )
{
    msgBridge msg        = NULL;
    mamaMsg   parent     = NULL;
    char*     symbol     = "SYM";
    char*     subject    = "SUBJECT";
    int       destroyMsg = 1;

    ASSERT_EQ(MAMA_STATUS_OK,mamaMsg_create(&parent));

    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaMsgCreate(&msg,parent));

    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaMsgSetSendSubject(msg,symbol,subject));

    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaMsgDestroy(msg,destroyMsg));

}

TEST_F (MiddlewareMsgTests, setSendSubjectInvalidMsgBridge)
{
    const char* symbol  = (char*) NOT_NULL;
    const char* subject = (char*) NOT_NULL;
    
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaMsgSetSendSubject(NULL,symbol,subject));
}

TEST_F (MiddlewareMsgTests, setSendSubjectInvalidSymbol)
{
    msgBridge   msg     = (msgBridge) NOT_NULL;
    const char* subject = (char*)     NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaMsgSetSendSubject(msg,NULL,subject));
}

TEST_F (MiddlewareMsgTests, setSendSubjectInvalidSubject)
{
    msgBridge   msg    = (msgBridge) NOT_NULL;
    const char* symbol = (char*)     NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaMsgSetSendSubject(msg,symbol,NULL));
}

TEST_F (MiddlewareMsgTests, getNativeHandle)
{
    msgBridge msg        = NULL;
    mamaMsg   parent     = NULL;
    void*     result     = NULL;
    int       destroyMsg = 1;

    ASSERT_EQ(MAMA_STATUS_OK,mamaMsg_create(&parent));

    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaMsgCreate(&msg,parent));

    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaMsgGetNativeHandle(msg,&result));

    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaMsgDestroy(msg,destroyMsg));
}

TEST_F (MiddlewareMsgTests, getNativeHandleInvalidMsgBridge)
{
    void* result = NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaMsgGetNativeHandle(NULL,&result));
}

TEST_F (MiddlewareMsgTests, getNativeHandleInvalidResult)
{
    msgBridge msg = (msgBridge) NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaMsgGetNativeHandle(msg,NULL));
}

TEST_F (MiddlewareMsgTests, duplicateReplyHandle)
{
    msgBridge msg        = NULL;
    mamaMsg   parent     = NULL;
    void*     result     = NULL;
    int       destroyMsg = 1;

    ASSERT_EQ(MAMA_STATUS_OK,mamaMsg_create(&parent));

    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaMsgCreate(&msg,parent));

    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaMsgDuplicateReplyHandle(msg,&result));

    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaMsgDestroy(msg,destroyMsg));
}

TEST_F (MiddlewareMsgTests, duplicateReplyHandleInvalidMsgBridge)
{
    void* result = NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaMsgDuplicateReplyHandle(NULL,&result));
}

TEST_F (MiddlewareMsgTests, duplicateReplyHandleInvalidResult)
{
    msgBridge msg;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaMsgDuplicateReplyHandle(msg,NULL));
}

TEST_F (MiddlewareMsgTests, DISABLED_copyReplyHandle)
{
    /*
     * This test should be a valid test of copying a reply handle
     * from a valid message
     */
}

TEST_F (MiddlewareMsgTests, copyReplyHandleInvalidSrc)
{
    void* dest = NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaMsgCopyReplyHandle(NULL,&dest));
}

TEST_F (MiddlewareMsgTests, copyReplyHandleInvalidDest)
{
    void* src = NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaMsgCopyReplyHandle(&src,NULL));
}

TEST_F (MiddlewareMsgTests, DISABLED_setReplyHandle)
{
    /*
     * This test should be a valid test of setting a reply handle
     * in a valid message
     */
}

TEST_F (MiddlewareMsgTests, SetReplyHandleInvalidMsg)
{
    void* result = NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaMsgSetReplyHandle(NULL,result));
}

TEST_F (MiddlewareMsgTests, SetReplyHandleInvalidResult)
{
    msgBridge msg = (msgBridge) NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaMsgSetReplyHandle(msg,NULL));
}

TEST_F (MiddlewareMsgTests, DISABLED_setReplyHandleAndIncrement)
{
    /*
     * This test should be a valid test of setting a reply handle
     * in a valid message
     */
}

TEST_F (MiddlewareMsgTests, SetReplyHandleAndIncrementInvalidMsgBridge)
{
    void* result = NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaMsgSetReplyHandleAndIncrement(NULL,result));
}

TEST_F (MiddlewareMsgTests, SetReplyHandleAndIncrementInvalidresult)
{
    msgBridge msg = (msgBridge) NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaMsgSetReplyHandleAndIncrement(msg,NULL));
}


TEST_F (MiddlewareMsgTests, destroyReplyHandleInvalid)
{
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaMsgDestroyReplyHandle(NULL));
}


