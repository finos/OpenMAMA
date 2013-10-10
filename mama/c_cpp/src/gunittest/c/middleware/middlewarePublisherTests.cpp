/* $Id: middlewarePublisherTests.cpp,v 1.1.2.7 2013/02/01 17:26:58 matthewmulhern Exp $
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


class MiddlewarePublisherTests : public ::testing::Test
{
protected:
    MiddlewarePublisherTests(void);
    virtual ~MiddlewarePublisherTests(void);

    virtual void SetUp(void);
    virtual void TearDown(void);

    mamaBridge mBridge;
};

MiddlewarePublisherTests::MiddlewarePublisherTests(void)
{
}

MiddlewarePublisherTests::~MiddlewarePublisherTests(void)
{
}

void MiddlewarePublisherTests::SetUp(void)
{
	mama_loadBridge (&mBridge,getMiddleware());
    mama_open();
}

void MiddlewarePublisherTests::TearDown(void)
{
    mama_close();
}


/*===================================================================
 =               mamaPublisher bridge functions                      =
 ====================================================================*/
TEST_F (MiddlewarePublisherTests, DISABLED_createDestroy)
/* cores */
{
    publisherBridge  publisher         = NULL;
    mamaTransport    tport             = NULL;
    const char*      topic             = "TOPIC";
    const char*      symbol            = "SYM";
    const char*      source            = "SOURCE";
    const char*      root              = "ROOT";
    void*            nativeQueueHandle = NULL;
    mamaPublisher    parent            = NULL;

    ASSERT_EQ(MAMA_STATUS_OK,
              mamaTransport_allocate(&tport));

    ASSERT_EQ(MAMA_STATUS_OK,
              mamaTransport_create(tport, "test_tport", mBridge));

    ASSERT_EQ(MAMA_STATUS_OK,
              mamaPublisher_create(&parent, tport, symbol, source, root));
    
    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaPublisherCreate(&publisher, tport, topic, source, root, nativeQueueHandle, parent));
    
    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaPublisherDestroy(publisher));

    ASSERT_EQ(MAMA_STATUS_OK,
              mamaPublisher_destroy(parent));
    
    ASSERT_EQ(MAMA_STATUS_OK,
              mamaTransport_destroy(tport));
}

TEST_F (MiddlewarePublisherTests, createByIndexInvalidResult)
{
    mamaTransport    tport             = (mamaTransport)   NOT_NULL;
    int              tportIndex        =                   0;
    const char*      topic             = (char*)           NOT_NULL;
    const char*      source            = (char*)           NOT_NULL;
    const char*      root              = (char*)           NOT_NULL;
    void*            nativeQueueHandle =                   NOT_NULL;
    mamaPublisher    parent            = (mamaPublisher)   NOT_NULL;
    
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaPublisherCreateByIndex(NULL, tport,
                                                         tportIndex,
                                                         topic, source,
                                                         root, nativeQueueHandle, 
                                                         parent));
}

TEST_F(MiddlewarePublisherTests, createByIndexInvalidTport)
{
    publisherBridge  result            = (publisherBridge) NOT_NULL;
    int              tportIndex        =                   0;
    const char*      topic             = (char*)           NOT_NULL;
    const char*      source            = (char*)           NOT_NULL;
    const char*      root              = (char*)           NOT_NULL;
    void*            nativeQueueHandle =                   NOT_NULL;
    mamaPublisher    parent            = (mamaPublisher)   NOT_NULL;
    
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaPublisherCreateByIndex(&result, NULL,
                                                         tportIndex,
                                                         topic, source,
                                                         root, nativeQueueHandle, 
                                                         parent));
}

TEST_F(MiddlewarePublisherTests, DISABLED_createByIndexInvalidTopic)
{
    publisherBridge  result            = (publisherBridge) NOT_NULL;
    mamaTransport    tport             = (mamaTransport)   NOT_NULL;
    int              tportIndex        = 0;
    const char*      source            = (char*)           NOT_NULL;
    const char*      root              = (char*)           NOT_NULL;
    void*            nativeQueueHandle =                   NOT_NULL;
    mamaPublisher    parent            = (mamaPublisher)   NOT_NULL;
    
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaPublisherCreateByIndex(&result, tport,
                                                         tportIndex,
                                                         NULL, source,
                                                         root, nativeQueueHandle, 
                                                         parent));
}

TEST_F(MiddlewarePublisherTests, DISABLED_createByIndexInvalidSource)
{
    publisherBridge  result            = (publisherBridge) NOT_NULL;
    mamaTransport    tport             = (mamaTransport)   NOT_NULL;
    int              tportIndex        =                   0;
    const char*      topic             = (char*)           NOT_NULL;
    const char*      root              = (char*)           NOT_NULL;
    void*            nativeQueueHandle =                   NOT_NULL;
    mamaPublisher    parent            = (mamaPublisher)   NOT_NULL;
    
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaPublisherCreateByIndex(&result, tport,
                                                         tportIndex,
                                                         topic, NULL,
                                                         root, nativeQueueHandle, 
                                                         parent));
}

TEST_F(MiddlewarePublisherTests, DISABLED_createByIndexInvalidRoot)
{
    publisherBridge  result            = (publisherBridge) NOT_NULL;
    mamaTransport    tport             = (mamaTransport)   NOT_NULL;
    int              tportIndex        =                   0;
    const char*      topic             = (char*)           NOT_NULL;
    const char*      source            = (char*)           NOT_NULL;
    void*            nativeQueueHandle =                   NOT_NULL;
    mamaPublisher    parent            = (mamaPublisher)   NOT_NULL;
    
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaPublisherCreateByIndex(&result, tport,
                                                         tportIndex,
                                                         topic, source,
                                                         NULL, nativeQueueHandle, 
                                                         parent));
}

TEST_F(MiddlewarePublisherTests, createByIndexInvalidNativeQueueHandle)
{
    publisherBridge  result            = (publisherBridge) NOT_NULL;
    mamaTransport    tport             = (mamaTransport)   NOT_NULL;
    int              tportIndex        =                   0;
    const char*      topic             = (char*)           NOT_NULL;
    const char*      source            = (char*)           NOT_NULL;
    const char*      root              = (char*)           NOT_NULL;
    mamaPublisher    parent            = (mamaPublisher)   NOT_NULL;
    
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaPublisherCreateByIndex(&result, tport,
                                                         tportIndex,
                                                         topic, source,
                                                         root, NULL, 
                                                         parent));
}

TEST_F(MiddlewarePublisherTests, createByIndexInvalidParent)
{
    publisherBridge  result            = (publisherBridge) NOT_NULL;
    mamaTransport    tport             = (mamaTransport)   NOT_NULL;
    int              tportIndex        =                   0;
    const char*      topic             = (char*)           NOT_NULL;
    const char*      source            = (char*)           NOT_NULL;
    const char*      root              = (char*)           NOT_NULL;
    void*            nativeQueueHandle =                   NOT_NULL;
    
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaPublisherCreateByIndex(&result, tport,
                                                         tportIndex,
                                                         topic, source,
                                                         root, nativeQueueHandle, 
                                                         NULL));
}


TEST_F (MiddlewarePublisherTests, createInvalidResult)
{
    mamaTransport    tport             = (mamaTransport)   NOT_NULL;
    const char*      topic             = (char*)           NOT_NULL;
    const char*      source            = (char*)           NOT_NULL;
    const char*      root              = (char*)           NOT_NULL;
    void*            nativeQueueHandle =                   NOT_NULL;
    mamaPublisher    parent            = (mamaPublisher)   NOT_NULL;
    
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaPublisherCreate(NULL, tport, topic,
                                                  source, root, 
                                                  nativeQueueHandle,
                                                  parent));
}

TEST_F (MiddlewarePublisherTests, createInvalidTport)
{
    publisherBridge  result            = (publisherBridge) NOT_NULL;
    const char*      topic             = (char*)           NOT_NULL;
    const char*      source            = (char*)           NOT_NULL;
    const char*      root              = (char*)           NOT_NULL;
    void*            nativeQueueHandle =                   NOT_NULL;
    mamaPublisher    parent            = (mamaPublisher)   NOT_NULL;
    
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaPublisherCreate(&result, NULL, topic,
                                                  source, root, 
                                                  nativeQueueHandle,
                                                  parent));
}

TEST_F (MiddlewarePublisherTests, createInvalidTopic)
{
    publisherBridge  result            = (publisherBridge) NOT_NULL;
    mamaTransport    tport             = (mamaTransport)   NOT_NULL;
    const char*      source            = (char*)           NOT_NULL;
    const char*      root              = (char*)           NOT_NULL;
    void*            nativeQueueHandle =                   NOT_NULL;
    mamaPublisher    parent            = (mamaPublisher)   NOT_NULL;
    
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaPublisherCreate(&result, tport, NULL,
                                                  source, root, 
                                                  nativeQueueHandle,
                                                  parent));
}

TEST_F (MiddlewarePublisherTests, createInvalidSource)
{
    publisherBridge  result            = (publisherBridge) NOT_NULL;
    mamaTransport    tport             = (mamaTransport)   NOT_NULL;
    const char*      topic             = (char*)           NOT_NULL;
    const char*      root              = (char*)           NOT_NULL;
    void*            nativeQueueHandle =                   NOT_NULL;
    mamaPublisher    parent            = (mamaPublisher)   NOT_NULL;
    
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaPublisherCreate(&result, tport, topic,
                                                  NULL, root, 
                                                  nativeQueueHandle,
                                                  parent));
}

TEST_F (MiddlewarePublisherTests, createInvalidRoot)
{
    publisherBridge  result            = (publisherBridge) NOT_NULL;
    mamaTransport    tport             = (mamaTransport)   NOT_NULL;
    const char*      topic             = (char*)           NOT_NULL;
    const char*      source            = (char*)           NOT_NULL;
    void*            nativeQueueHandle =                   NOT_NULL;
    mamaPublisher    parent            = (mamaPublisher)   NOT_NULL;
    
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaPublisherCreate(&result, tport, topic,
                                                  source, NULL, 
                                                  nativeQueueHandle,
                                                  parent));
}

TEST_F (MiddlewarePublisherTests, createInvalidNativeQueueHandle)
{
    publisherBridge  result            = (publisherBridge) NOT_NULL;
    mamaTransport    tport             = (mamaTransport)   NOT_NULL;
    const char*      topic             = (char*)           NOT_NULL;
    const char*      source            = (char*)           NOT_NULL;
    const char*      root              = (char*)           NOT_NULL;
    mamaPublisher    parent            = (mamaPublisher)   NOT_NULL;
    
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaPublisherCreate(&result, tport, topic,
                                                  source, root, 
                                                  NULL, parent));
}

TEST_F (MiddlewarePublisherTests, createInvalidPublisher)
{
    publisherBridge  result            = (publisherBridge) NOT_NULL;
    mamaTransport    tport             = (mamaTransport)   NOT_NULL;
    const char*      topic             = (char*)           NOT_NULL;
    const char*      source            = (char*)           NOT_NULL;
    const char*      root              = (char*)           NOT_NULL;
    void*            nativeQueueHandle =                   NOT_NULL;
    
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaPublisherCreate(&result, tport, topic,
                                                  source, root, 
                                                  nativeQueueHandle,
                                                  NULL));
}

TEST_F (MiddlewarePublisherTests, destroyInvalid)
{
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaPublisherDestroy(NULL));
}

TEST_F (MiddlewarePublisherTests, DISABLED_send)
/* cores */
{
    publisherBridge  publisher         = NULL;
    mamaTransport    tport             = NULL;
    const char*      topic             = "TOPIC";
    const char*      symbol            = "SYM";
    const char*      source            = "SOURCE";
    const char*      root              = "ROOT";
    void*            nativeQueueHandle = NULL;
    mamaPublisher    parent            = NULL;
    mamaMsg          msg               = NULL;

    ASSERT_EQ(MAMA_STATUS_OK,
              mamaTransport_allocate(&tport));

    ASSERT_EQ(MAMA_STATUS_OK,
              mamaTransport_create(tport, "test_tport", mBridge));

    ASSERT_EQ(MAMA_STATUS_OK,
              mamaMsg_create(&msg));

    ASSERT_EQ(MAMA_STATUS_OK,
              mamaPublisher_create(&parent, tport, symbol, source, root));
    
    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaPublisherCreate(&publisher, tport, 
                                                 topic, source, root, 
                                                 nativeQueueHandle, parent));
    
    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaPublisherSend(publisher, msg));

    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaPublisherDestroy(publisher));

    ASSERT_EQ(MAMA_STATUS_OK,
              mamaPublisher_destroy(parent));
    
    ASSERT_EQ(MAMA_STATUS_OK,
              mamaMsg_destroy(msg));

    ASSERT_EQ(MAMA_STATUS_OK,
              mamaTransport_destroy(tport));
}

TEST_F (MiddlewarePublisherTests, sendInvalidPublisherBridge)
{
    mamaMsg msg = (mamaMsg) NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaPublisherSend(NULL,msg));
}

TEST_F (MiddlewarePublisherTests, sendInvalidMsg)
{
    publisherBridge publisher = (publisherBridge) NOT_NULL;
    
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaPublisherSend(publisher,NULL));
}

TEST_F (MiddlewarePublisherTests, sendReplyToInboxInvalidPublisherBridge)
{
    mamaMsg request = (mamaMsg) NOT_NULL;
    mamaMsg reply   = (mamaMsg) NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaPublisherSendReplyToInbox(NULL,
                                                            request,
                                                            reply));
}

TEST_F (MiddlewarePublisherTests, sendReplyToInboxInvalidRequest)
{
    publisherBridge publisher = (publisherBridge) NOT_NULL;
    mamaMsg         reply     = (mamaMsg)         NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaPublisherSendReplyToInbox(publisher,
                                                            NULL,
                                                            reply));
}

TEST_F (MiddlewarePublisherTests, sendReplyToInboxInvalidReply)
{
    publisherBridge publisher = (publisherBridge) NOT_NULL;
    mamaMsg         request   = (mamaMsg)         NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaPublisherSendReplyToInbox(publisher,
                                                            request,
                                                            NULL));
}

TEST_F (MiddlewarePublisherTests, sendReplyToInboxHandleInvalidPublisherBridge)
{
    mamaMsg request = (mamaMsg) NOT_NULL;
    mamaMsg reply   = (mamaMsg) NOT_NULL;
    
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaPublisherSendReplyToInboxHandle(NULL,request,
                                                                  reply));
}

TEST_F (MiddlewarePublisherTests, sendReplyToInboxHandleInvalidRequest)
{
    publisherBridge publisher= (publisherBridge) NOT_NULL;
    mamaMsg         reply   = (mamaMsg)          NOT_NULL;
    
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaPublisherSendReplyToInboxHandle(publisher,
                                                                  NULL,
                                                                  reply));
}

TEST_F (MiddlewarePublisherTests, sendReplyToInboxHandleInvalidReply)
{
    publisherBridge publisher= (publisherBridge) NOT_NULL;
    mamaMsg         request = (mamaMsg)          NOT_NULL;
    
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaPublisherSendReplyToInboxHandle(publisher,
                                                                  request,
                                                                  NULL));
}

TEST_F (MiddlewarePublisherTests, sendFromInboxByIndexInvalidPublisherBridge)
{
    int             tportIndex =                   0;
    mamaInbox       inbox      = (mamaInbox)       NOT_NULL;
    mamaMsg         msg        = (mamaMsg)         NOT_NULL;
    
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaPublisherSendFromInboxByIndex(NULL, 
                                                                tportIndex,
                                                                inbox ,msg));
}

TEST_F (MiddlewarePublisherTests, sendFromInboxByIndexInvalidInbox)
{
    publisherBridge publisher  = (publisherBridge) NOT_NULL;
    int             tportIndex =                   0;
    mamaMsg         msg        = (mamaMsg)         NOT_NULL;
    
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaPublisherSendFromInboxByIndex(publisher, 
                                                                tportIndex,
                                                                NULL, msg));
}

TEST_F (MiddlewarePublisherTests, sendFromInboxByIndexInvalidMsg)
{
    publisherBridge publisher  = (publisherBridge) NOT_NULL;
    int             tportIndex =                   0;
    mamaInbox       inbox      = (mamaInbox)       NOT_NULL;
    
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaPublisherSendFromInboxByIndex(publisher, 
                                                                tportIndex,
                                                                inbox, NULL));
}

TEST_F (MiddlewarePublisherTests, sendFromInboxInvalidPublisherBridge)
{
    mamaInbox       inbox     = (mamaInbox)       NOT_NULL;
    mamaMsg         msg       = (mamaMsg)         NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaPublisherSendFromInbox(NULL,
                                                         inbox, msg));
}

TEST_F (MiddlewarePublisherTests, sendFromInboxInvalidInbox)
{
    publisherBridge publisher = (publisherBridge) NOT_NULL;
    mamaMsg         msg       = (mamaMsg)         NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaPublisherSendFromInbox(publisher,
                                                         NULL, msg));
}

TEST_F (MiddlewarePublisherTests, sendFromInboxInvalidMsg)
{
    publisherBridge publisher = (publisherBridge) NOT_NULL;
    mamaInbox       inbox     = (mamaInbox)       NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaPublisherSendFromInbox(publisher,
                                                         inbox, NULL));
}

