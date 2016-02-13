/* $Id: middlewareSubscriptionTests.cpp,v 1.1.2.5 2012/12/17 15:46:04 matthewmulhern Exp $
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

static void onCreate (mamaSubscription subscription, void* closure);

static void onError(mamaSubscription subscription,
                    mama_status      status,
                    void*            platformError,
                    const char*      subject,
                    void*            closure);

static void onQuality(mamaSubscription subsc,
                      mamaQuality      quality,
                      const char*      symbol,
                      short            cause,
                      const void*      platformInfo,
                      void*            closure);

static void onMsg(mamaSubscription subscription,
                  mamaMsg          msg,
                  void*            closure,
                  void*            itemClosure);

static void onGap(mamaSubscription subsc, void* closure);
static void onRecapRequest(mamaSubscription subsc, void* closure);
static void onDestroy(mamaSubscription subsc, void* closure);

class MiddlewareSubscriptionTests : public ::testing::Test
{
protected:
    MiddlewareSubscriptionTests(void);
    virtual ~MiddlewareSubscriptionTests(void);

    virtual void SetUp(void);
    virtual void TearDown(void);

    mamaBridge mBridge;
    mamaTransport tport;
    const char*   tportName;

    subscriptionBridge subscriber;
    mamaSource         source;
    const char*        sourceName;
    const char*        symbol;
    mamaQueue          queue;
    void*              closure;
    mamaSubscription   parent;
    mamaMsgCallbacks   callbacks;
};

MiddlewareSubscriptionTests::MiddlewareSubscriptionTests(void)
    : tport (NULL),
      tportName ("test_tport"),
      source (NULL),
      sourceName ("src"),
      symbol ("SYM"),
      queue (NULL),
      closure (NULL)
{
    mama_loadBridge (&mBridge, getMiddleware());

    mamaQueue_create(&queue, mBridge);
}

MiddlewareSubscriptionTests::~MiddlewareSubscriptionTests(void)
{
}

void MiddlewareSubscriptionTests::SetUp(void)
{
    mama_open();  /* Forces loading of entitlements bridges as necessary */
    mamaTransport_allocate (&tport);
    mamaTransport_create   (tport, tportName, mBridge);

    mamaSource_create(&source);
    mamaSource_setId(source, "SRC");
    mamaSource_setTransport(source, tport);
    mamaSource_setSymbolNamespace(source, "NASDAQ");

    mamaSubscription_allocate(&parent);

    callbacks.onCreate          = onCreate; 
    callbacks.onError           = onError; 
    callbacks.onQuality         = onQuality; 
    callbacks.onMsg             = onMsg; 
    callbacks.onGap             = onGap; 
    callbacks.onRecapRequest    = onRecapRequest;
    callbacks.onDestroy         = onDestroy;
}

void MiddlewareSubscriptionTests::TearDown(void)
{
    mamaTransport_destroy (tport);
    mamaSubscription_deallocate(parent);
    mamaQueue_destroy (queue);
    mama_close();
}

static void onCreate (mamaSubscription subscription,
                      void* closure)
{
}

static void onError(mamaSubscription subscription,
                    mama_status      status,
                    void*            platformError,
                    const char*      subject,
                    void*            closure)
{
}

static void onQuality(mamaSubscription subsc,
                      mamaQuality      quality,
                      const char*      symbol,
                      short            cause,
                      const void*      platformInfo,
                      void*            closure)
{
}

static void onMsg(mamaSubscription subscription,
                  mamaMsg          msg,
                  void*            closure,
                  void*            itemClosure)
{
}

static void onGap(mamaSubscription subsc, void* closure)
{
}

static void onRecapRequest(mamaSubscription subsc, void* closure)
{
}

static void onDestroy(mamaSubscription subsc, void* closure)
{
}


/*===================================================================
 =               mamaSubscription bridge functions                   =
 ====================================================================*/

/* TODO:
 * Discuss the validity of these tests - ultimately we double create
 * subscriptions, which I would assume isn't supposed to be expected behaviour.
 */
TEST_F (MiddlewareSubscriptionTests, DISABLED_createDestroy)
/* cores*/
{
    ASSERT_EQ(MAMA_STATUS_OK,
              mamaSubscription_create(parent, queue, &callbacks, source, sourceName, closure));

    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaSubscriptionCreate(&subscriber, sourceName, symbol,
                                                    tport, queue, callbacks,
                                                    parent, closure));
    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaSubscriptionDestroy(subscriber));
    
    ASSERT_EQ(MAMA_STATUS_OK,
              mamaSubscription_destroy(parent));
    
    ASSERT_EQ(MAMA_STATUS_OK,
              mamaSubscription_deallocate(parent));
}

TEST_F (MiddlewareSubscriptionTests, createInvalidResult)
{
    ASSERT_EQ(MAMA_STATUS_NULL_ARG,
              mBridge->bridgeMamaSubscriptionCreate(NULL, sourceName, symbol,
                                                    tport, queue, callbacks,
                                                    parent, closure));
}

TEST_F (MiddlewareSubscriptionTests, createInvalidTport)
{
    ASSERT_EQ(MAMA_STATUS_NULL_ARG,
              mBridge->bridgeMamaSubscriptionCreate(&subscriber, sourceName, symbol,
                                                    NULL, queue, callbacks,
                                                    parent, closure));
}

TEST_F (MiddlewareSubscriptionTests, createInvalidSourceName)
{
    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaSubscriptionCreate(&subscriber, NULL, symbol,
                                                    tport, queue, callbacks,
                                                    parent, closure));
}

TEST_F (MiddlewareSubscriptionTests, createInvalidSymbol)
{
    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaSubscriptionCreate(&subscriber, sourceName, NULL,
                                                    tport, queue, callbacks,
                                                    parent, closure));
}

TEST_F (MiddlewareSubscriptionTests, createInvalidQueue)
{
    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaSubscriptionCreate(&subscriber, sourceName, symbol,
                                                    tport, NULL, callbacks,
                                                    parent, closure));
}

TEST_F (MiddlewareSubscriptionTests, createInvalidParent)
{
    ASSERT_EQ(MAMA_STATUS_NULL_ARG,
              mBridge->bridgeMamaSubscriptionCreate(&subscriber, sourceName, symbol,
                                                    tport, queue, callbacks,
                                                    NULL, closure));
}
/* TEST COMMENTED OUT BECAUSE mamaMsgCallbacks CAN'T BE CAST AS NULL!
TEST_F (MiddlewareSubscriptionTests, createInvalidCallbacks)
{
    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaSubscriptionCreate(&subscriber, sourceName, symbol,
                                                    tport, queue, NULL,
                                                    parent, closure));
}
*/

TEST_F (MiddlewareSubscriptionTests, createWildCardInvalidResult)
{
	mama_status status = mBridge->bridgeMamaSubscriptionCreateWildCard(NULL, sourceName, symbol,
                                                            tport, queue, callbacks,
                                                            parent, closure);
	CHECK_NON_IMPLEMENTED_OPTIONAL(status);
    ASSERT_EQ(MAMA_STATUS_OK,
    		  status);
}

TEST_F (MiddlewareSubscriptionTests, createWildCardInvalidSource)
{
	mama_status status = mBridge->bridgeMamaSubscriptionCreateWildCard(&subscriber, NULL, symbol,
                                                            tport, queue, callbacks,
                                                            parent, closure);
	CHECK_NON_IMPLEMENTED_OPTIONAL(status);
    ASSERT_EQ(MAMA_STATUS_OK,
    		  status);
}

TEST_F (MiddlewareSubscriptionTests, createWildCardInvalidSymbol)
{
	mama_status status = mBridge->bridgeMamaSubscriptionCreateWildCard(&subscriber, sourceName, NULL,
                                                            tport, queue, callbacks,
                                                            parent, closure);
	CHECK_NON_IMPLEMENTED_OPTIONAL(status);
    ASSERT_EQ(MAMA_STATUS_OK,
    		  status);
}

TEST_F (MiddlewareSubscriptionTests, createWildCardInvalidTport)
{
	mama_status status = mBridge->bridgeMamaSubscriptionCreateWildCard(&subscriber, sourceName, symbol,
                                                            NULL, queue, callbacks,
                                                            parent, closure);
	CHECK_NON_IMPLEMENTED_OPTIONAL(status);
    ASSERT_EQ(MAMA_STATUS_OK,
    		  status);
}

TEST_F (MiddlewareSubscriptionTests, createWildCardInvalidQueue)
{
	mama_status status = mBridge->bridgeMamaSubscriptionCreateWildCard(&subscriber, sourceName, symbol,
                                                            tport, NULL, callbacks,
                                                            parent, closure);
	CHECK_NON_IMPLEMENTED_OPTIONAL(status);
    ASSERT_EQ(MAMA_STATUS_OK,
    		  status);
}

TEST_F (MiddlewareSubscriptionTests, createWildCardInvalidParent)
{
	mama_status status = mBridge->bridgeMamaSubscriptionCreateWildCard(&subscriber, sourceName, symbol,
                                                            tport, queue, callbacks,
                                                            NULL, closure);
	CHECK_NON_IMPLEMENTED_OPTIONAL(status);
    ASSERT_EQ(MAMA_STATUS_OK,
              status);
}
/* COMMENTED OUT BECAUSE mamaMsg Callbacks CAN'T BE CAST AS NULL
TEST_F (MiddlewareSubscriptionTests, createWildCardInvalidCallbacks)
{
    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaSubscriptionCreateWildCard(&subscriber, sourceName, symbol, 
                                                            tport, queue, NULL, 
                                                            parent, closure));
}
*/

TEST_F (MiddlewareSubscriptionTests, mute)
{
    ASSERT_EQ(MAMA_STATUS_OK,
              mamaSubscription_create(parent, queue, &callbacks, source, sourceName, closure));

    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaSubscriptionCreate(&subscriber, sourceName, symbol,
                                                    tport, queue, callbacks,
                                                    parent, closure));

    ASSERT_EQ (MAMA_STATUS_OK, 
               mBridge->bridgeMamaSubscriptionMute(subscriber));

    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaSubscriptionDestroy(subscriber));
}

TEST_F (MiddlewareSubscriptionTests, muteInvalid)
{
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaSubscriptionMute(NULL));
}

TEST_F (MiddlewareSubscriptionTests, destroyInvalid)
{
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaSubscriptionDestroy(NULL));
}

TEST_F (MiddlewareSubscriptionTests, isValid)
{
    int res = NULL;
    ASSERT_EQ(MAMA_STATUS_OK,
              mamaSubscription_create(parent, queue, &callbacks, source, sourceName, closure));

    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaSubscriptionCreate(&subscriber, sourceName, symbol,
                                                    tport, queue, callbacks,
                                                    parent, closure));
    
    res = mBridge->bridgeMamaSubscriptionIsValid(subscriber);
    ASSERT_TRUE(res != NULL);

}

TEST_F (MiddlewareSubscriptionTests, isValidInvalid)
{
    int res = NULL;

    res = mBridge->bridgeMamaSubscriptionIsValid(NULL);
    
    ASSERT_TRUE(res == 0);
}


TEST_F (MiddlewareSubscriptionTests, hasWildcards)
{
    int res = NULL;

    res = mBridge->bridgeMamaSubscriptionHasWildcards(NULL);
    
    ASSERT_TRUE(res == 0);
}

TEST_F (MiddlewareSubscriptionTests, getPlatformError)
{
    void* error = NOT_NULL;
    mama_status status = MAMA_STATUS_OK;
    ASSERT_EQ(MAMA_STATUS_OK,
              mamaSubscription_create(parent, queue, &callbacks, source, sourceName, closure));

    ASSERT_EQ (MAMA_STATUS_OK,
    		   mBridge->bridgeMamaSubscriptionCreate(&subscriber, sourceName, symbol,
    		                                                    tport, queue, callbacks,
    		                                                    parent, closure));

    status = mBridge->bridgeMamaSubscriptionGetPlatformError(subscriber, &error);

    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaSubscriptionDestroy(subscriber));

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ(MAMA_STATUS_OK,
              status);
}

TEST_F (MiddlewareSubscriptionTests, getPlatformErrorInvalidError)
{
	mama_status status = mBridge->bridgeMamaSubscriptionGetPlatformError(subscriber,
                                                               NULL);
	CHECK_NON_IMPLEMENTED_OPTIONAL(status);
    ASSERT_EQ (MAMA_STATUS_NULL_ARG,
               status);
}

TEST_F (MiddlewareSubscriptionTests, getPlatformErrorInvalidSubBridge)
{
    void* error = NOT_NULL;
    mama_status status = mBridge->bridgeMamaSubscriptionGetPlatformError(NULL,
                                                               &error);
    CHECK_NON_IMPLEMENTED_OPTIONAL(status);
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               status);
}

TEST_F (MiddlewareSubscriptionTests, isTportDisconnectedInvalid)
{
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaSubscriptionIsTportDisconnected(NULL));
}

TEST_F (MiddlewareSubscriptionTests, setTopicClosure)
{
    void* newClosure = NOT_NULL;
    mama_status status = MAMA_STATUS_OK;
    ASSERT_EQ(MAMA_STATUS_OK,
              mamaSubscription_create(parent, queue, &callbacks, source, sourceName, closure));

    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaSubscriptionCreate(&subscriber, sourceName, symbol,
                                                    tport, queue, callbacks,
                                                    parent, closure));

    status = mBridge->bridgeMamaSubscriptionSetTopicClosure(subscriber,newClosure);

    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaSubscriptionDestroy(subscriber));

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_OK,
               status);
}

TEST_F (MiddlewareSubscriptionTests, setTopicClosureInvalidSubBridge)
{
    void* closure = NOT_NULL;
    mama_status status = mBridge->bridgeMamaSubscriptionSetTopicClosure(NULL,
                                                              closure);

    CHECK_NON_IMPLEMENTED_OPTIONAL(status);

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               status);
}


TEST_F (MiddlewareSubscriptionTests, muteCurrentTopic)
{
    ASSERT_EQ(MAMA_STATUS_OK,
              mamaSubscription_create(parent, queue, &callbacks, source, sourceName, closure));

    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaSubscriptionCreate(&subscriber, sourceName, symbol,
                                                    tport, queue, callbacks,
                                                    parent, closure));

    ASSERT_EQ (MAMA_STATUS_OK, 
               mBridge->bridgeMamaSubscriptionMuteCurrentTopic(subscriber));

    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaSubscriptionDestroy(subscriber));
}

TEST_F (MiddlewareSubscriptionTests, muteCurrentTopicInvalid)
{
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaSubscriptionMuteCurrentTopic(NULL));
}

