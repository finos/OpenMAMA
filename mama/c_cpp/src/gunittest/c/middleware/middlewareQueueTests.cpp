/* $Id: middlewareQueueTests.cpp,v 1.1.2.6 2012/12/11 16:24:23 matthewmulhern Exp $
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


class MiddlewareQueueTests : public ::testing::Test
{
protected:
    MiddlewareQueueTests(void);
    virtual ~MiddlewareQueueTests(void);

    virtual void SetUp(void);
    virtual void TearDown(void);

    mamaBridge mBridge;
};

MiddlewareQueueTests::MiddlewareQueueTests(void)
{
}

MiddlewareQueueTests::~MiddlewareQueueTests(void)
{
}

void MiddlewareQueueTests::SetUp(void)
{
    mama_loadBridge (&mBridge,getMiddleware());
    mama_open();
}

void MiddlewareQueueTests::TearDown(void)
{
    mama_close();
}

static void MAMACALLTYPE onEvent(mamaQueue queue, void* closure)
{
}
static void MAMACALLTYPE onEnqueue(mamaQueue queue, void* closure)
{
}


/*===================================================================
 =               mamaQueue bridge functions                          =
 ====================================================================*/
TEST_F (MiddlewareQueueTests, create)
{
    queueBridge queue  = NULL;
    mamaQueue   parent = NULL;

    mamaQueue_create(&parent,mBridge);

    ASSERT_EQ(MAMA_STATUS_OK, 
              mBridge->bridgeMamaQueueCreate(&queue, parent));
    mBridge->bridgeMamaQueueDestroy(queue);
    ASSERT_EQ(MAMA_STATUS_OK, mamaQueue_destroy(parent));
}

TEST_F (MiddlewareQueueTests, createInvalidQueueBridge)
{
    mamaQueue parent = (mamaQueue) NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG,
               mBridge->bridgeMamaQueueCreate(NULL,parent));
}

TEST_F (MiddlewareQueueTests, createInvalidParent)
{
    queueBridge queue = (queueBridge) NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaQueueCreate(&queue,NULL));
}

TEST_F (MiddlewareQueueTests, createUsingNativeInvalidQueueBridge)
{
    mamaQueue   parent      = (mamaQueue) NOT_NULL;
    void*       nativeQueue =             NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaQueueCreateUsingNative(NULL,
                                                         parent,
                                                         nativeQueue));
}

TEST_F (MiddlewareQueueTests, createUsingNativeInvalidParent)
{
    queueBridge queue       = (queueBridge) NOT_NULL;
    void*       nativeQueue =               NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaQueueCreateUsingNative(&queue,
                                                         NULL,
                                                         nativeQueue));
}

TEST_F (MiddlewareQueueTests, createUsingNativeInvalidNativeQueue)
{
    queueBridge queue  = (queueBridge) NOT_NULL;
    mamaQueue   parent = (mamaQueue)   NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaQueueCreateUsingNative(&queue,
                                                         parent,
                                                         NULL));
}

TEST_F (MiddlewareQueueTests, destroy)
{
    queueBridge queue   = NULL;
    mamaQueue   parent  = NULL;

    mamaQueue_create(&parent,mBridge);
    mBridge->bridgeMamaQueueCreate(&queue, parent);

    ASSERT_EQ (MAMA_STATUS_OK, mBridge->bridgeMamaQueueDestroy(queue));
    ASSERT_EQ(MAMA_STATUS_OK, mamaQueue_destroy(parent));
}

TEST_F (MiddlewareQueueTests, destroyInvalid)
{
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaQueueDestroy(NULL));
}

TEST_F (MiddlewareQueueTests, getEventCount)
{
    size_t      count   = 0;
    queueBridge queue   = NULL;
    mamaQueue   parent  = NULL;

    mamaQueue_create(&parent,mBridge);
    mBridge->bridgeMamaQueueCreate(&queue, parent);
    
    ASSERT_EQ (MAMA_STATUS_OK, 
               mBridge->bridgeMamaQueueGetEventCount(queue,&count));
    ASSERT_EQ(MAMA_STATUS_OK, mBridge->bridgeMamaQueueDestroy(queue));
    ASSERT_EQ(MAMA_STATUS_OK, mamaQueue_destroy(parent));
}

TEST_F (MiddlewareQueueTests, getEventCountInvalidQueueBridge)
{
    size_t count = (size_t) NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG,
               mBridge->bridgeMamaQueueGetEventCount(NULL,&count));
}

TEST_F (MiddlewareQueueTests, getEventCountInvalidCount)
{
    queueBridge queue = (queueBridge) NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaQueueGetEventCount(queue,NULL));
}

/* TODO: Fix these.
 * Disabling dispatch tests, because the standard queue implementation has
 * dispatch as a blocking call.
 */
TEST_F (MiddlewareQueueTests, DISABLED_dispatch)
{
    queueBridge queue   = NULL;
    mamaQueue   parent  = NULL;
    void*       closure = NULL;

    mamaQueue_create(&parent,mBridge);
    mBridge->bridgeMamaQueueCreate(&queue, parent);
    mBridge->bridgeMamaQueueEnqueueEvent(queue,onEvent,closure);
    
    ASSERT_EQ (MAMA_STATUS_OK, 
               mBridge->bridgeMamaQueueDispatch(queue));
    ASSERT_EQ(MAMA_STATUS_OK, mBridge->bridgeMamaQueueDestroy(queue));
}

TEST_F (MiddlewareQueueTests, DISABLED_dispatchMany)
{
    queueBridge queue   = NULL;
    mamaQueue   parent  = NULL;
    void*       closure = NOT_NULL;

    
    mamaQueue_create(&parent,mBridge);
    mBridge->bridgeMamaQueueCreate(&queue, parent);
    
    for(int i=0;i<100;i++)
    {
        ASSERT_EQ(MAMA_STATUS_OK, 
                  mBridge->bridgeMamaQueueEnqueueEvent(queue,
                                                       onEvent,
                                                       closure));
    }

    for(int i=0;i<100;i++)
    {
        ASSERT_EQ(MAMA_STATUS_OK, 
                  mBridge->bridgeMamaQueueDispatch(queue));
    }
    ASSERT_EQ(MAMA_STATUS_OK, mBridge->bridgeMamaQueueDestroy(queue));
}

TEST_F (MiddlewareQueueTests, DISABLED_dispatchManyQueues)
{
    queueBridge queue[10] = {NULL};
    mamaQueue   parent    = NULL;
    void*       closure   = NOT_NULL;

    mamaQueue_create(&parent,mBridge);
    
    for(int counter=0;counter<10;counter++)
    {
        ASSERT_EQ(MAMA_STATUS_OK, 
                  mBridge->bridgeMamaQueueCreate(&queue[counter], 
                                                 parent));

        for(int i=0;i<100;i++)
        {
            ASSERT_EQ(MAMA_STATUS_OK, 
                      mBridge->bridgeMamaQueueEnqueueEvent(queue[counter],
                                                           onEvent,
                                                           closure));
        }

        for(int i=0;i<100;i++)
        {
            ASSERT_EQ(MAMA_STATUS_OK, 
                      mBridge->bridgeMamaQueueDispatch(queue[counter]));
        }
        ASSERT_EQ(MAMA_STATUS_OK, mBridge->bridgeMamaQueueDestroy(queue[counter]));

    }

    for(int counter=0;counter<10;counter++)
    {
        ASSERT_EQ(MAMA_STATUS_OK, 
                  mBridge->bridgeMamaQueueDestroy(queue[counter]));
    }
}

TEST_F (MiddlewareQueueTests, dispatchInvalid)
{
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaQueueDispatch(NULL));
}

TEST_F (MiddlewareQueueTests, timedDispatch)
{
    uint64_t    timeout = 500;
    queueBridge queue   = NULL;
    mamaQueue   parent  = NULL;
    void*       closure = NOT_NULL;
    
    mamaQueue_create(&parent,mBridge);
    mBridge->bridgeMamaQueueCreate(&queue, parent);
    mBridge->bridgeMamaQueueEnqueueEvent(queue,onEvent,closure);
    mBridge->bridgeMamaQueueEnqueueEvent(queue,onEvent,closure);
    mBridge->bridgeMamaQueueEnqueueEvent(queue,onEvent,closure);

    ASSERT_EQ(MAMA_STATUS_OK, 
              mBridge->bridgeMamaQueueTimedDispatch(queue,timeout));
    ASSERT_EQ(MAMA_STATUS_OK, mBridge->bridgeMamaQueueDestroy(queue));
    ASSERT_EQ(MAMA_STATUS_OK, mamaQueue_destroy(parent));
}

TEST_F (MiddlewareQueueTests, timedDispatchInvalidQueueBridge)
{
    uint64_t timeout = (uint64_t) NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaQueueTimedDispatch(NULL,timeout));
}

/* TODO: Need to determine the expectation around passing NULL 
 * to the TimedDispatch call
 */
TEST_F (MiddlewareQueueTests, DISABLED_timedDispatchInvalidTimeout)
{
    queueBridge queue = (queueBridge) NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaQueueTimedDispatch(queue, 0));
}

TEST_F (MiddlewareQueueTests, dispatchEventInvalid)
{
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaQueueDispatchEvent(NULL));
}

TEST_F (MiddlewareQueueTests, enqueueEvent)
{
    queueBridge queue   = NULL;
    mamaQueue   parent  = NULL;
    void*       closure = NOT_NULL;

    mamaQueue_create(&parent,mBridge);
    mBridge->bridgeMamaQueueCreate(&queue, parent);
   
    ASSERT_EQ (MAMA_STATUS_OK, 
               mBridge->bridgeMamaQueueEnqueueEvent(queue,
                                                    onEvent,
                                                    closure));
    ASSERT_EQ(MAMA_STATUS_OK, mBridge->bridgeMamaQueueDestroy(queue));
    ASSERT_EQ(MAMA_STATUS_OK, mamaQueue_destroy(parent));
}

TEST_F (MiddlewareQueueTests, enqueueEventInvalidQueueBridge)
{
    mamaQueueEventCB onQueueEvent = (mamaQueueEventCB) NOT_NULL;
    void*            closure      =                    NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaQueueEnqueueEvent(NULL,
                                                    onQueueEvent,
                                                    closure));
}

TEST_F (MiddlewareQueueTests, enqueueEventInvalidCallback)
{
    queueBridge queue   = (queueBridge) NOT_NULL;
    void*       closure =               NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaQueueEnqueueEvent(queue,
                                                    NULL,
                                                    closure));
}

TEST_F (MiddlewareQueueTests, stopDispatchInvalid)
{
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaQueueStopDispatch(NULL));
}

/* TODO: As with dispatch tests, dispatch is a blocking call, so this test
 * doesn't really make any sense.
 */
TEST_F (MiddlewareQueueTests, DISABLED_setEnqueueCallback)
{
    queueBridge queue   = NULL;
    mamaQueue   parent  = NULL;
    void*       closure = NOT_NULL;
    
    mamaQueue_create(&parent,mBridge);
    mBridge->bridgeMamaQueueCreate(&queue, parent);

    ASSERT_EQ(MAMA_STATUS_OK, 
              mBridge->bridgeMamaQueueSetEnqueueCallback(queue,
                                                         onEnqueue,
                                                         closure));

    mBridge->bridgeMamaQueueEnqueueEvent(queue,onEvent,closure);
    mBridge->bridgeMamaQueueDispatch(queue);
}

TEST_F (MiddlewareQueueTests, setEnqueueCallbackInvalidQueueBridge)
{
    mamaQueueEnqueueCB onEnqueue = (mamaQueueEnqueueCB) NOT_NULL;
    void*              closure   =                      NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaQueueSetEnqueueCallback(NULL,
                                                          onEnqueue,
                                                          closure));
}

TEST_F (MiddlewareQueueTests, setEnqueueCallbackInvalidCallback)
{
    queueBridge        queue   = (queueBridge) NOT_NULL;
    void*              closure =               NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaQueueSetEnqueueCallback(queue,
                                                          NULL,closure));
}

/* TODO: As with dispatch tests, dispatch is a blocking call, so this test
 * doesn't really make any sense.
 */
TEST_F (MiddlewareQueueTests, DISABLED_removeEnqueueCallback)
{
    queueBridge queue   = NULL;
    mamaQueue   parent  = NULL;
    void*       closure = NOT_NULL;
    
    mamaQueue_create(&parent,mBridge);
    mBridge->bridgeMamaQueueCreate(&queue, parent);

    ASSERT_EQ(MAMA_STATUS_OK, 
              mBridge->bridgeMamaQueueSetEnqueueCallback(queue,
                                                         onEnqueue,
                                                         closure));

    mBridge->bridgeMamaQueueEnqueueEvent(queue,onEvent,closure);
    mBridge->bridgeMamaQueueDispatch(queue);
    
    ASSERT_EQ(MAMA_STATUS_OK, 
              mBridge->bridgeMamaQueueRemoveEnqueueCallback(queue));
    
    mBridge->bridgeMamaQueueEnqueueEvent(queue,onEvent,closure);
    mBridge->bridgeMamaQueueDispatch(queue);
}

TEST_F (MiddlewareQueueTests, removeEnqueueCallbackInvalid)
{
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaQueueRemoveEnqueueCallback(NULL));
}

TEST_F (MiddlewareQueueTests, getNativeHandleInvalidQueueBridge)
{
    void* result = NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaQueueGetNativeHandle(NULL,&result));
}

TEST_F (MiddlewareQueueTests, getNativeHandleInvalidResult)
{
    queueBridge queue = (queueBridge) NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaQueueGetNativeHandle(queue,NULL));
}

TEST_F (MiddlewareQueueTests, setHighWatermark)
{
    size_t      highWatermark   = 10;
    queueBridge queue           = NULL;
    mamaQueue   parent          = NULL;

    mamaQueue_create(&parent,mBridge);
    mBridge->bridgeMamaQueueCreate(&queue, parent);

    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaQueueSetHighWatermark(queue,
                                                       highWatermark));
    ASSERT_EQ(MAMA_STATUS_OK, mBridge->bridgeMamaQueueDestroy(queue));
    ASSERT_EQ(MAMA_STATUS_OK, mamaQueue_destroy(parent));
}

TEST_F (MiddlewareQueueTests, setHighWatermarkInvalidQueueBridge)
{
    size_t highWatermark = (size_t) NOT_NULL;
    
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaQueueSetHighWatermark(NULL,
                                                        highWatermark));
}

TEST_F (MiddlewareQueueTests, setHighWatermarkInvalidWatermark)
{
    queueBridge queue = (queueBridge) NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaQueueSetHighWatermark(queue, 0));
}

TEST_F (MiddlewareQueueTests, setLowWatermark)
{
    size_t      lowWatermark = 2;
    queueBridge queue        = NULL;
    mamaQueue   parent       = NULL;

    mamaQueue_create(&parent,mBridge);
    mBridge->bridgeMamaQueueCreate(&queue, parent);

    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaQueueSetLowWatermark(queue, lowWatermark));
    ASSERT_EQ(MAMA_STATUS_OK, mBridge->bridgeMamaQueueDestroy(queue));
    ASSERT_EQ(MAMA_STATUS_OK, mamaQueue_destroy(parent));
}

TEST_F (MiddlewareQueueTests, setLowWatermarkInvalidQueueBridge)
{
    size_t lowWatermark = (size_t) NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaQueueSetLowWatermark(NULL,lowWatermark));
}

TEST_F (MiddlewareQueueTests, setLowWatermarkInvalidWatermark)
{
    queueBridge queue = (queueBridge) NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaQueueSetLowWatermark(queue, 0));
}

