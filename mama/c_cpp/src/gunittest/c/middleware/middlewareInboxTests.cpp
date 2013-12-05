/* $Id: middlewareInboxTests.cpp,v 1.1.2.6 2013/02/04 13:53:24 matthewmulhern Exp $
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


class MiddlewareInboxTests : public ::testing::Test
{
protected:
    MiddlewareInboxTests(void);
    virtual ~MiddlewareInboxTests(void);

    virtual void SetUp(void);
    virtual void TearDown(void);

    mamaBridge mBridge;
};

MiddlewareInboxTests::MiddlewareInboxTests(void)
{
}

MiddlewareInboxTests::~MiddlewareInboxTests(void)
{
}

void MiddlewareInboxTests::SetUp(void)
{
	mama_loadBridge (&mBridge,getMiddleware());
}

void MiddlewareInboxTests::TearDown(void)
{
}

void onMsg(mamaMsg msg, void *closure)
{
}

void onError(mama_status status, void *closure)
{
}

void onInboxDestroy(mamaInbox inbox, void *closure)
{
}

/*===================================================================
 =               mamaInbox bridge functions                          =
 ====================================================================*/
TEST_F (MiddlewareInboxTests, create)
{
    inboxBridge   bridge    = NULL;
    mamaTransport transport = NULL;
    mamaQueue     queue     = NULL;
    void*         closure   = NULL;
    mamaInbox     parent    = NULL;
    const char*   name      = "test_tport";

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_allocate(&transport));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_create(transport, name, mBridge));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaQueue_create(&queue, mBridge));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaInbox_create(&parent, transport, queue, onMsg, 
                                onError, closure));
   
    ASSERT_EQ (MAMA_STATUS_OK, 
               mBridge->bridgeMamaInboxCreate(&bridge,
                                              transport,
                                              queue,
                                              onMsg,
                                              onError,
                                              onInboxDestroy,
                                              closure,
                                              parent));

    /* We have to clean up here and destroy the transport, otherwise other
     * tests will fail to run correctly.
     */
    ASSERT_EQ (MAMA_STATUS_OK, mBridge->bridgeMamaInboxDestroy(bridge));
    ASSERT_EQ (MAMA_STATUS_OK, mamaInbox_destroy(parent));
    ASSERT_EQ (MAMA_STATUS_OK, mamaTransport_destroy(transport));
}

TEST_F (MiddlewareInboxTests, createInvalidBridge)
{
    mamaTransport            transport          = (mamaTransport) NOT_NULL;
    mamaQueue                queue              = (mamaQueue) NOT_NULL;
    mamaInboxMsgCallback     msgCB              = (mamaInboxMsgCallback) NOT_NULL;
    mamaInboxErrorCallback   errorCB            = (mamaInboxErrorCallback) NOT_NULL;
    mamaInboxDestroyCallback onInboxDestroyed   = (mamaInboxDestroyCallback) NOT_NULL;
    void*                    closure            = NOT_NULL;
    mamaInbox                parent             = (mamaInbox) NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaInboxCreate(NULL,
                                              transport,
                                              queue,
                                              msgCB,
                                              errorCB,
                                              onInboxDestroyed,
                                              closure,
                                              parent));

}

TEST_F (MiddlewareInboxTests, createInvalidTransport)
{
    inboxBridge              bridge             = (inboxBridge) NOT_NULL;
    mamaQueue                queue              = (mamaQueue) NOT_NULL;
    mamaInboxMsgCallback     msgCB              = (mamaInboxMsgCallback) NOT_NULL;
    mamaInboxErrorCallback   errorCB            = (mamaInboxErrorCallback) NOT_NULL;
    mamaInboxDestroyCallback onInboxDestroyed   = (mamaInboxDestroyCallback) NOT_NULL;
    void*                    closure            = NOT_NULL;
    mamaInbox                parent             = (mamaInbox) NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaInboxCreate(&bridge,
                                              NULL,
                                              queue,
                                              msgCB,
                                              errorCB,
                                              onInboxDestroyed,
                                              closure,
                                              parent));

}

TEST_F (MiddlewareInboxTests, createInvalidQueue)
{
    inboxBridge              bridge             = (inboxBridge) NOT_NULL;
    mamaTransport            transport          = (mamaTransport) NOT_NULL;
    mamaInboxMsgCallback     msgCB              = (mamaInboxMsgCallback) NOT_NULL;
    mamaInboxErrorCallback   errorCB            = (mamaInboxErrorCallback) NOT_NULL;
    mamaInboxDestroyCallback onInboxDestroyed   = (mamaInboxDestroyCallback) NOT_NULL;
    void*                    closure            = NOT_NULL;
    mamaInbox                parent             = (mamaInbox) NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaInboxCreate(&bridge,
                                              transport,
                                              NULL,
                                              msgCB,
                                              errorCB,
                                              onInboxDestroyed,
                                              closure,
                                              parent));
}

TEST_F (MiddlewareInboxTests, createInvalidMsgCB)
{
    inboxBridge              bridge             = (inboxBridge) NOT_NULL;
    mamaTransport            transport          = (mamaTransport) NOT_NULL;
    mamaQueue                queue              = (mamaQueue) NOT_NULL;
    mamaInboxErrorCallback   errorCB            = (mamaInboxErrorCallback) NOT_NULL;
    mamaInboxDestroyCallback onInboxDestroyed   = (mamaInboxDestroyCallback) NOT_NULL;
    void*                    closure            = NOT_NULL;
    mamaInbox                parent             = (mamaInbox) NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaInboxCreate(&bridge,
                                              transport,
                                              queue,
                                              NULL,
                                              errorCB,
                                              onInboxDestroyed,
                                              closure,
                                              parent));

}

TEST_F (MiddlewareInboxTests, createByIndexInvalidBridge)
{
    mamaTransport            transport          = (mamaTransport) NOT_NULL;
    int                      tportIndex         = 0; 
    mamaQueue                queue              = (mamaQueue) NOT_NULL;
    mamaInboxMsgCallback     msgCB              = (mamaInboxMsgCallback) NOT_NULL;
    mamaInboxErrorCallback   errorCB            = (mamaInboxErrorCallback) NOT_NULL;
    mamaInboxDestroyCallback onInboxDestroyed   = (mamaInboxDestroyCallback) NOT_NULL;
    void*                    closure            = NOT_NULL;
    mamaInbox                parent             = (mamaInbox) NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaInboxCreateByIndex(NULL,
                                                     transport,
                                                     tportIndex,
                                                     queue,
                                                     msgCB,
                                                     errorCB,
                                                     onInboxDestroyed,
                                                     closure,
                                                     parent));
}

TEST_F (MiddlewareInboxTests, createByIndexInvalidTransport)
{
    inboxBridge              bridge             = (inboxBridge) NOT_NULL;
    int                      tportIndex         = 0;
    mamaQueue                queue              = (mamaQueue) NOT_NULL;
    mamaInboxMsgCallback     msgCB              = (mamaInboxMsgCallback) NOT_NULL;
    mamaInboxErrorCallback   errorCB            = (mamaInboxErrorCallback) NOT_NULL;
    mamaInboxDestroyCallback onInboxDestroyed   = (mamaInboxDestroyCallback) NOT_NULL;
    void*                    closure            = NOT_NULL;
    mamaInbox                parent             = (mamaInbox) NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaInboxCreateByIndex(&bridge,
                                                     NULL,
                                                     tportIndex,
                                                     queue,
                                                     msgCB,
                                                     errorCB,
                                                     onInboxDestroyed,
                                                     closure,
                                                     parent));
}

TEST_F (MiddlewareInboxTests, createByIndexInvalidQueue)
{
    inboxBridge              bridge             = (inboxBridge) NOT_NULL;
    mamaTransport            transport          = (mamaTransport) NOT_NULL;
    int                      tportIndex         = 0;
    mamaInboxMsgCallback     msgCB              = (mamaInboxMsgCallback) NOT_NULL;
    mamaInboxErrorCallback   errorCB            = (mamaInboxErrorCallback) NOT_NULL;
    mamaInboxDestroyCallback onInboxDestroyed   = (mamaInboxDestroyCallback) NOT_NULL;
    void*                    closure            = NOT_NULL;
    mamaInbox                parent             = (mamaInbox) NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaInboxCreateByIndex(&bridge,
                                                     transport,
                                                     tportIndex,
                                                     NULL,
                                                     msgCB,
                                                     errorCB,
                                                     onInboxDestroyed,
                                                     closure,
                                                     parent));
}

TEST_F (MiddlewareInboxTests, createByIndexInvalidMsgCB)
{
    inboxBridge              bridge             = (inboxBridge) NOT_NULL;
    mamaTransport            transport          = (mamaTransport) NOT_NULL;
    int                      tportIndex         = 0;
    mamaQueue                queue              = (mamaQueue) NOT_NULL;
    mamaInboxErrorCallback   errorCB            = (mamaInboxErrorCallback) NOT_NULL;
    mamaInboxDestroyCallback onInboxDestroyed   = (mamaInboxDestroyCallback) NOT_NULL;
    void*                    closure            = NOT_NULL;
    mamaInbox                parent             = (mamaInbox) NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaInboxCreateByIndex(&bridge,
                                                     transport,
                                                     tportIndex,
                                                     queue,
                                                     NULL,
                                                     errorCB,
                                                     onInboxDestroyed,
                                                     closure,
                                                     parent));
}

TEST_F (MiddlewareInboxTests, CreateDestroy)
{
    inboxBridge   bridge    = NULL;
    mamaTransport transport = NULL;
    mamaQueue     queue     = NULL;
    void*         closure   = NULL;
    mamaInbox     parent    = NULL;
    const char*   name      = "test_tport";

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_allocate(&transport));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_create(transport, name, mBridge));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaQueue_create(&queue, mBridge));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaInbox_create(&parent, transport, queue, onMsg, 
                                onError, closure));
   
    ASSERT_EQ (MAMA_STATUS_OK, 
               mBridge->bridgeMamaInboxCreate(&bridge,
                                              transport,
                                              queue,
                                              onMsg,
                                              onError,
                                              onInboxDestroy,
                                              closure,
                                              parent));

    ASSERT_EQ (MAMA_STATUS_OK, 
               mBridge->bridgeMamaInboxDestroy(bridge));
}

TEST_F (MiddlewareInboxTests, destroyInvalid)
{
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, mBridge->bridgeMamaInboxDestroy(NULL));
}

