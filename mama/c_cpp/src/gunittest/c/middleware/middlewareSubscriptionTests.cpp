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


class MiddlewareSubscriptionTests : public ::testing::Test
{
protected:
    MiddlewareSubscriptionTests(void);
    virtual ~MiddlewareSubscriptionTests(void);

    virtual void SetUp(void);
    virtual void TearDown(void);

    mamaBridge mBridge;
};

MiddlewareSubscriptionTests::MiddlewareSubscriptionTests(void)
{
}

MiddlewareSubscriptionTests::~MiddlewareSubscriptionTests(void)
{
}

void MiddlewareSubscriptionTests::SetUp(void)
{
	mama_loadBridge (&mBridge,getMiddleware());
}

void MiddlewareSubscriptionTests::TearDown(void)
{
}

static void onCreate(mamaSubscription subscription, void*	closure)
{
}

static void onError(mamaSubscription subscription, 
                    mama_status       status, 
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
                  mamaMsg	       msg,
                  void*	           closure,
                  void*	           itemClosure)
{
}

static void onGap(mamaSubscription subsc, void* closure)
{
}

static void onRecapRequest(mamaSubscription subsc, void* closure)
{
}
/*===================================================================
 =               mamaSubscription bridge functions                   =
 ====================================================================*/

TEST_F (MiddlewareSubscriptionTests, DISABLED_createDestroy)
/* cores*/
{
    subscriptionBridge subscriber = NULL;
    mamaTransport      tport      = NULL;
    const char*        tportName  = "test_tport";
    mamaSource         source     = NULL;
    const char*        sourceName = "src";
    const char*        symbol     = "SYM";
    mamaQueue          queue      = NULL;
    void*              closure    = NULL;
    mamaSubscription   parent     = NULL;

    mamaTransport_allocate(&tport);
    mamaTransport_create(tport, tportName, mBridge);
    
    mamaSource_create(&source);
    mamaSource_setId(source, "SRC");
    mamaSource_setTransport(source, tport);
    mamaSource_setSymbolNamespace(source, "NASDAQ");
    
    mamaMsgCallbacks callbacks; 
    callbacks.onCreate          = onCreate; 
    callbacks.onError           = onError; 
    callbacks.onQuality         = onQuality; 
    callbacks.onMsg             = onMsg; 
    callbacks.onGap             = onGap; 
    callbacks.onRecapRequest    = onRecapRequest;

    mamaQueue_create(&queue,mBridge);
    
    mamaSubscription_allocate(&parent);
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
    mamaTransport      tport      = (mamaTransport)      NOT_NULL;
    const char*        sourceName = (char*)              NOT_NULL;
    const char*        symbol     = (char*)              NOT_NULL;
    mamaQueue          queue      = (mamaQueue)          NOT_NULL;
    void*              closure    =                      NULL;
    mamaSubscription   parent     = (mamaSubscription)   NOT_NULL;
    mamaMsgCallbacks   callbacks;
    
    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaSubscriptionCreate(NULL, sourceName, symbol,
                                                    tport, queue, callbacks,
                                                    parent, closure));
}

TEST_F (MiddlewareSubscriptionTests, createInvalidTport)
{
    subscriptionBridge subscriber = (subscriptionBridge) NOT_NULL; 
    const char*        sourceName = (char*)              NOT_NULL;
    const char*        symbol     = (char*)              NOT_NULL;
    mamaQueue          queue      = (mamaQueue)          NOT_NULL;
    void*              closure    =                      NULL;
    mamaSubscription   parent     = (mamaSubscription)   NOT_NULL;
    mamaMsgCallbacks   callbacks;
    
    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaSubscriptionCreate(&subscriber, sourceName, symbol,
                                                    NULL, queue, callbacks,
                                                    parent, closure));
}

TEST_F (MiddlewareSubscriptionTests, createInvalidSourceName)
{
    subscriptionBridge subscriber = (subscriptionBridge) NOT_NULL; 
    mamaTransport      tport      = (mamaTransport)      NOT_NULL;
    const char*        symbol     = (char*)              NOT_NULL;
    mamaQueue          queue      = (mamaQueue)          NOT_NULL;
    void*              closure    =                      NULL;
    mamaSubscription   parent     = (mamaSubscription)   NOT_NULL;
    mamaMsgCallbacks   callbacks;
    
    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaSubscriptionCreate(&subscriber, NULL, symbol,
                                                    tport, queue, callbacks,
                                                    parent, closure));
}

TEST_F (MiddlewareSubscriptionTests, createInvalidSymbol)
{
    subscriptionBridge subscriber = (subscriptionBridge) NOT_NULL; 
    mamaTransport      tport      = (mamaTransport)      NOT_NULL;
    const char*        sourceName = (char*)              NOT_NULL;
    mamaQueue          queue      = (mamaQueue)          NOT_NULL;
    void*              closure    =                      NULL;
    mamaSubscription   parent     = (mamaSubscription)   NOT_NULL;
    mamaMsgCallbacks   callbacks;
    
    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaSubscriptionCreate(&subscriber, sourceName, NULL,
                                                    tport, queue, callbacks,
                                                    parent, closure));
}

TEST_F (MiddlewareSubscriptionTests, createInvalidQueue)
{
    subscriptionBridge subscriber = (subscriptionBridge) NOT_NULL; 
    mamaTransport      tport      = (mamaTransport)      NOT_NULL;
    const char*        sourceName = (char*)              NOT_NULL;
    const char*        symbol     = (char*)              NOT_NULL;
    void*              closure    =                      NULL;
    mamaSubscription   parent     = (mamaSubscription)   NOT_NULL;
    mamaMsgCallbacks   callbacks;
    
    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaSubscriptionCreate(&subscriber, sourceName, symbol,
                                                    tport, NULL, callbacks,
                                                    parent, closure));
}

TEST_F (MiddlewareSubscriptionTests, createInvalidParent)
{
    subscriptionBridge subscriber = (subscriptionBridge) NOT_NULL; 
    mamaTransport      tport      = (mamaTransport)      NOT_NULL;
    const char*        sourceName = (char*)              NOT_NULL;
    const char*        symbol     = (char*)              NOT_NULL;
    mamaQueue          queue      = (mamaQueue)          NOT_NULL;
    void*              closure    =                      NULL;
    mamaMsgCallbacks   callbacks;
    
    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaSubscriptionCreate(&subscriber, sourceName, symbol,
                                                    tport, queue, callbacks,
                                                    NULL, closure));
}
/* TEST COMMENTED OUT BECAUSE mamaMsgCallbacks CAN'T BE CAST AS NULL!
TEST_F (MiddlewareSubscriptionTests, createInvalidCallbacks)
{
    subscriptionBridge subscriber = (subscriptionBridge) NOT_NULL; 
    mamaTransport      tport      = (mamaTransport)      NOT_NULL;
    const char*        sourceName = (char*)              NOT_NULL;
    const char*        symbol     = (char*)              NOT_NULL;
    mamaQueue          queue      = (mamaQueue)          NOT_NULL;
    void*              closure    =                      NULL;
    mamaSubscription   parent     = (mamaSubscription)   NOT_NULL;
    
    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaSubscriptionCreate(&subscriber, sourceName, symbol,
                                                    tport, queue, NULL,
                                                    parent, closure));
}
*/

TEST_F (MiddlewareSubscriptionTests, createWildCardInvalidResult)
{
    const char*         source      = (char*)              NOT_NULL;
    const char*         symbol      = (char*)              NOT_NULL;
    mamaTransport       tport       = (mamaTransport)      NOT_NULL;
    mamaQueue           queue       = (mamaQueue)          NOT_NULL;
    mamaSubscription    parent      = (mamaSubscription)   NOT_NULL;
    void*               closure     =                      NULL;
    mamaMsgCallbacks    callbacks; 
    
    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaSubscriptionCreateWildCard(NULL, source, symbol, 
                                                            tport, queue, callbacks, 
                                                            parent, closure));
}

TEST_F (MiddlewareSubscriptionTests, createWildCardInvalidSource)
{
    subscriptionBridge  subscriber  = (subscriptionBridge) NOT_NULL;
    const char*         source      = (char*)              NOT_NULL;
    const char*         symbol      = (char*)              NOT_NULL;
    mamaTransport       tport       = (mamaTransport)      NOT_NULL;
    mamaQueue           queue       = (mamaQueue)          NOT_NULL;
    mamaSubscription    parent      = (mamaSubscription)   NOT_NULL;
    void*               closure     =                      NULL;
    mamaMsgCallbacks    callbacks; 
    
    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaSubscriptionCreateWildCard(&subscriber, NULL, symbol, 
                                                            tport, queue, callbacks, 
                                                            parent, closure));
}

TEST_F (MiddlewareSubscriptionTests, createWildCardInvalidSymbol)
{
    subscriptionBridge  subscriber  = (subscriptionBridge) NOT_NULL;
    const char*         source      = (char*)              NOT_NULL;
    mamaTransport       tport       = (mamaTransport)      NOT_NULL;
    mamaQueue           queue       = (mamaQueue)          NOT_NULL;
    mamaSubscription    parent      = (mamaSubscription)   NOT_NULL;
    void*               closure     =                      NULL;
    mamaMsgCallbacks    callbacks; 
    
    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaSubscriptionCreateWildCard(&subscriber, source, NULL, 
                                                            tport, queue, callbacks, 
                                                            parent, closure));
}

TEST_F (MiddlewareSubscriptionTests, createWildCardInvalidTport)
{
    subscriptionBridge  subscriber  = (subscriptionBridge) NOT_NULL;
    const char*         source      = (char*)              NOT_NULL;
    const char*         symbol      = (char*)              NOT_NULL;
    mamaQueue           queue       = (mamaQueue)          NOT_NULL;
    mamaSubscription    parent      = (mamaSubscription)   NOT_NULL;
    void*               closure     =                      NULL;
    mamaMsgCallbacks    callbacks; 
    
    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaSubscriptionCreateWildCard(&subscriber, source, symbol, 
                                                            NULL, queue, callbacks, 
                                                            parent, closure));
}

TEST_F (MiddlewareSubscriptionTests, createWildCardInvalidQueue)
{
    subscriptionBridge  subscriber  = (subscriptionBridge) NOT_NULL;
    const char*         source      = (char*)              NOT_NULL;
    const char*         symbol      = (char*)              NOT_NULL;
    mamaTransport       tport       = (mamaTransport)      NOT_NULL;
    mamaSubscription    parent      = (mamaSubscription)   NOT_NULL;
    void*               closure     =                      NULL;
    mamaMsgCallbacks    callbacks; 
    
    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaSubscriptionCreateWildCard(&subscriber, source, symbol, 
                                                            tport, NULL, callbacks, 
                                                            parent, closure));
}

TEST_F (MiddlewareSubscriptionTests, createWildCardInvalidParent)
{
    subscriptionBridge  subscriber  = (subscriptionBridge) NOT_NULL;
    const char*         source      = (char*)              NOT_NULL;
    const char*         symbol      = (char*)              NOT_NULL;
    mamaTransport       tport       = (mamaTransport)      NOT_NULL;
    mamaQueue           queue       = (mamaQueue)          NOT_NULL;
    void*               closure     =                      NULL;
    mamaMsgCallbacks    callbacks; 
    
    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaSubscriptionCreateWildCard(&subscriber, source, symbol, 
                                                            tport, queue, callbacks, 
                                                            NULL, closure));
}
/* COMMENTED OUT BECAUSE mamaMsg Callbacks CAN'T BE CAST AS NULL
TEST_F (MiddlewareSubscriptionTests, createWildCardInvalidCallbacks)
{
    subscriptionBridge  subscriber  = (subscriptionBridge) NOT_NULL;
    const char*         source      = (char*)              NOT_NULL;
    const char*         symbol      = (char*)              NOT_NULL;
    mamaTransport       tport       = (mamaTransport)      NOT_NULL;
    mamaQueue           queue       = (mamaQueue)          NOT_NULL;
    mamaSubscription    parent      = (mamaSubscription)   NOT_NULL;
    void*               closure     =                      NULL;
    
    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaSubscriptionCreateWildCard(&subscriber, source, symbol, 
                                                            tport, queue, NULL, 
                                                            parent, closure));
}
*/

TEST_F (MiddlewareSubscriptionTests, mute)
{
    subscriptionBridge subscriber = NULL;
    mamaTransport      tport      = NULL;
    const char*        tportName  = "test_tport";
    mamaSource         source     = NULL;
    const char*        sourceName = "src";
    const char*        symbol     = "SYM";
    mamaQueue          queue      = NULL;
    void*              closure    = NULL;
    mamaSubscription   parent     = NULL;

    mamaTransport_allocate(&tport);
    mamaTransport_create(tport, tportName, mBridge);
    
    mamaSource_create(&source);
    mamaSource_setId(source, "SRC");
    mamaSource_setTransport(source, tport);
    mamaSource_setSymbolNamespace(source, "NASDAQ");
    
    mamaMsgCallbacks callbacks; 
    callbacks.onCreate       = onCreate; 
    callbacks.onError        = onError; 
    callbacks.onQuality      = onQuality; 
    callbacks.onMsg          = onMsg; 
    callbacks.onGap          = onGap; 
    callbacks.onRecapRequest = onRecapRequest;

    mamaQueue_create(&queue,mBridge);
    
    mamaSubscription_allocate(&parent);
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
    subscriptionBridge subscriber = NULL;
    mamaTransport      tport      = NULL;
    const char*        tportName  = "test_tport";
    mamaSource         source     = NULL;
    const char*        sourceName = "src";
    const char*        symbol     = "SYM";
    mamaQueue          queue      = NULL;
    void*              closure    = NULL;
    mamaSubscription   parent     = NULL;
    int                res        = NULL;

    mamaTransport_allocate(&tport);
    mamaTransport_create(tport, tportName, mBridge);
    
    mamaSource_create(&source);
    mamaSource_setId(source, "SRC");
    mamaSource_setTransport(source, tport);
    mamaSource_setSymbolNamespace(source, "NASDAQ");
    
    mamaMsgCallbacks callbacks; 
    callbacks.onCreate       = onCreate; 
    callbacks.onError        = onError; 
    callbacks.onQuality      = onQuality; 
    callbacks.onMsg          = onMsg; 
    callbacks.onGap          = onGap; 
    callbacks.onRecapRequest = onRecapRequest;

    mamaQueue_create(&queue,mBridge);
    
    mamaSubscription_allocate(&parent);
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
    subscriptionBridge subscriber = NULL;
    mamaTransport      tport      = NULL;
    const char*        tportName  = "test_tport";
    mamaSource         source     = NULL;
    const char*        sourceName = "src";
    const char*        symbol     = "SYM";
    mamaQueue          queue      = NULL;
    void*              closure    = NULL;
    mamaSubscription   parent     = NULL;
    void*              error      = NULL;

    mamaTransport_allocate(&tport);
    mamaTransport_create(tport, tportName, mBridge);
    
    mamaSource_create(&source);
    mamaSource_setId(source, "SRC");
    mamaSource_setTransport(source, tport);
    mamaSource_setSymbolNamespace(source, "NASDAQ");
    
    mamaMsgCallbacks callbacks; 
    callbacks.onCreate       = onCreate; 
    callbacks.onError        = onError; 
    callbacks.onQuality      = onQuality; 
    callbacks.onMsg          = onMsg; 
    callbacks.onGap          = onGap; 
    callbacks.onRecapRequest = onRecapRequest;

    mamaQueue_create(&queue,mBridge);
    
    mamaSubscription_allocate(&parent);
    ASSERT_EQ(MAMA_STATUS_OK,
              mamaSubscription_create(parent, queue, &callbacks, source, sourceName, closure));

    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaSubscriptionCreate(&subscriber, sourceName, symbol,
                                                    tport, queue, callbacks,
                                                    parent, closure));

    ASSERT_EQ (MAMA_STATUS_OK, 
               mBridge->bridgeMamaSubscriptionGetPlatformError(subscriber, &error));

    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaSubscriptionDestroy(subscriber));
}

TEST_F (MiddlewareSubscriptionTests, getPlatformErrorInvalidError)
{
    subscriptionBridge subscriber = (subscriptionBridge) NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaSubscriptionGetPlatformError(subscriber,
                                                               NULL));
}

TEST_F (MiddlewareSubscriptionTests, getPlatformErrorInvalidSubBridge)
{
    void* error = NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaSubscriptionGetPlatformError(NULL,
                                                               &error));
}

TEST_F (MiddlewareSubscriptionTests, isTportDisconnected)
{
    subscriptionBridge subscriber = NULL;
    mamaTransport      tport      = NULL;
    const char*        tportName  = "test_tport";
    mamaSource         source     = NULL;
    const char*        sourceName = "src";
    const char*        symbol     = "SYM";
    mamaQueue          queue      = NULL;
    void*              closure    = NULL;
    mamaSubscription   parent     = NULL;
    int                res        = NULL;
    mamaTransport_allocate(&tport);
    mamaTransport_create(tport, tportName, mBridge);
    
    mamaSource_create(&source);
    mamaSource_setId(source, "SRC");
    mamaSource_setTransport(source, tport);
    mamaSource_setSymbolNamespace(source, "NASDAQ");
    
    mamaMsgCallbacks callbacks; 
    callbacks.onCreate       = onCreate; 
    callbacks.onError        = onError; 
    callbacks.onQuality      = onQuality; 
    callbacks.onMsg          = onMsg; 
    callbacks.onGap          = onGap; 
    callbacks.onRecapRequest = onRecapRequest;

    mamaQueue_create(&queue,mBridge);
    
    mamaSubscription_allocate(&parent);
    ASSERT_EQ(MAMA_STATUS_OK,
              mamaSubscription_create(parent, queue, &callbacks, source, sourceName, closure));

    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaSubscriptionCreate(&subscriber, sourceName, symbol,
                                                    tport, queue, callbacks,
                                                    parent, closure));

    res=mBridge->bridgeMamaSubscriptionIsTportDisconnected(subscriber);
    ASSERT_TRUE(res != NULL);

    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaSubscriptionDestroy(subscriber));
}

TEST_F (MiddlewareSubscriptionTests, isTportDisconnectedInvalid)
{
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaSubscriptionIsTportDisconnected(NULL));
}

TEST_F (MiddlewareSubscriptionTests, setTopicClosure)
{
    subscriptionBridge subscriber = NULL;
    mamaTransport      tport      = NULL;
    const char*        tportName  = "test_tport";
    mamaSource         source     = NULL;
    const char*        sourceName = "src";
    const char*        symbol     = "SYM";
    mamaQueue          queue      = NULL;
    void*              closure    = NULL;
    void*              newClosure = NULL;
    mamaSubscription   parent     = NULL;

    mamaTransport_allocate(&tport);
    mamaTransport_create(tport, tportName, mBridge);
    
    mamaSource_create(&source);
    mamaSource_setId(source, "SRC");
    mamaSource_setTransport(source, tport);
    mamaSource_setSymbolNamespace(source, "NASDAQ");
    
    mamaMsgCallbacks callbacks; 
    callbacks.onCreate       = onCreate; 
    callbacks.onError        = onError; 
    callbacks.onQuality      = onQuality; 
    callbacks.onMsg          = onMsg; 
    callbacks.onGap          = onGap; 
    callbacks.onRecapRequest = onRecapRequest;

    mamaQueue_create(&queue,mBridge);
    
    mamaSubscription_allocate(&parent);
    ASSERT_EQ(MAMA_STATUS_OK,
              mamaSubscription_create(parent, queue, &callbacks, source, sourceName, closure));

    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaSubscriptionCreate(&subscriber, sourceName, symbol,
                                                    tport, queue, callbacks,
                                                    parent, closure));

    ASSERT_EQ (MAMA_STATUS_OK, 
               mBridge->bridgeMamaSubscriptionSetTopicClosure(subscriber,newClosure));

    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaSubscriptionDestroy(subscriber));
}

TEST_F (MiddlewareSubscriptionTests, setTopicClosureInvalidSubBridge)
{
    void* closure = NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaSubscriptionSetTopicClosure(NULL,
                                                              closure));
}


TEST_F (MiddlewareSubscriptionTests, muteCurrentTopic)
{
    subscriptionBridge subscriber = NULL;
    mamaTransport      tport      = NULL;
    const char*        tportName  = "test_tport";
    mamaSource         source     = NULL;
    const char*        sourceName = "src";
    const char*        symbol     = "SYM";
    mamaQueue          queue      = NULL;
    void*              closure    = NULL;
    mamaSubscription   parent     = NULL;

    mamaTransport_allocate(&tport);
    mamaTransport_create(tport, tportName, mBridge);
    
    mamaSource_create(&source);
    mamaSource_setId(source, "SRC");
    mamaSource_setTransport(source, tport);
    mamaSource_setSymbolNamespace(source, "NASDAQ");
    
    mamaMsgCallbacks callbacks; 
    callbacks.onCreate       = onCreate; 
    callbacks.onError        = onError; 
    callbacks.onQuality      = onQuality; 
    callbacks.onMsg          = onMsg; 
    callbacks.onGap          = onGap; 
    callbacks.onRecapRequest = onRecapRequest;

    mamaQueue_create(&queue,mBridge);
    
    mamaSubscription_allocate(&parent);
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

