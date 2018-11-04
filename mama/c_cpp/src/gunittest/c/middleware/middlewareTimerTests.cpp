/* $Id: middlewareTimerTests.cpp,v 1.1.2.5 2012/11/30 17:32:33 matthewmulhern Exp $
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

static void MAMACALLTYPE
noopTimerCb(mamaTimer timer, void *closure)
{
}

class MiddlewareTimerTests : public ::testing::Test
{
protected:
    MiddlewareTimerTests(void);
    virtual ~MiddlewareTimerTests(void);

    virtual void SetUp(void);
    virtual void TearDown(void);

public:
    mamaTimer mTimerParent;
    mamaBridge mBridge;
    timerBridge mTimerBridge;
    mamaQueue mDefaultQueue;
};

MiddlewareTimerTests::MiddlewareTimerTests(void)
{
}

MiddlewareTimerTests::~MiddlewareTimerTests(void)
{
}

void MiddlewareTimerTests::SetUp(void)
{
    mama_loadBridge (&mBridge,getMiddleware());
    mama_open ();
    mama_getDefaultEventQueue(mBridge,&mDefaultQueue);
    mamaTimer_create(&mTimerParent, mDefaultQueue, noopTimerCb, 1.0, NULL);
}

void MiddlewareTimerTests::TearDown(void)
{
    mamaTimer_destroy(mTimerParent);
    mama_close ();
}

static void MAMACALLTYPE onTick(mamaTimer timer, void* closure)
{
    MiddlewareTimerTests* timerTests = (MiddlewareTimerTests*) closure;
    ASSERT_EQ(MAMA_STATUS_OK, timerTests->mBridge->bridgeMamaTimerDestroy(timerTests->mTimerBridge));

}

static void MAMACALLTYPE onTimerDestroy(mamaTimer timer, void* closure)
{
    MiddlewareTimerTests* timerTests = (MiddlewareTimerTests*) closure;
    timerTests->mBridge->bridgeStop(timerTests->mDefaultQueue);
}
/*===================================================================
 =               mamaTimer bridge functions                          =
 ====================================================================*/

TEST_F (MiddlewareTimerTests, create)
{
    double        interval      =             0.001;
    void*         handle        =             NULL;

    ASSERT_EQ(MAMA_STATUS_OK, 
              mamaQueue_getNativeHandle(mDefaultQueue, &handle));
    
    ASSERT_EQ(MAMA_STATUS_OK, 
              mBridge->bridgeMamaTimerCreate(&mTimerBridge, handle, onTick,
                                             onTimerDestroy, interval,
                                             mTimerParent, (void*)this));

    /* Will block until timer destroys itself */
    mBridge->bridgeStart(mDefaultQueue);
}

TEST_F (MiddlewareTimerTests, createInvalidResult)
{
    void*         nativeQueueHandle =               NOT_NULL;
    mamaTimerCb   action            = (mamaTimerCb) NOT_NULL;
    mamaTimerCb   onTimerDestroyed  = (mamaTimerCb) NOT_NULL;
    double        interval          =               0.0;
    void*         closure           =               NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTimerCreate(NULL, nativeQueueHandle,
                                              action, onTimerDestroyed,
                                              interval, mTimerParent, closure));
}

TEST_F (MiddlewareTimerTests, createInvalidNativeQueueHandle)
{
    timerBridge   result            = (timerBridge) NOT_NULL;
    mamaTimerCb   action            = (mamaTimerCb) NOT_NULL;
    mamaTimerCb   onTimerDestroyed  = (mamaTimerCb) NOT_NULL;
    double        interval          =               0.0;
    void*         closure           =               NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTimerCreate(&result, NULL,
                                              action, onTimerDestroyed,
                                              interval, mTimerParent, closure));
}

TEST_F (MiddlewareTimerTests, createInvalidActionCB)
{
    timerBridge   result            = (timerBridge) NOT_NULL;
    void*         nativeQueueHandle =               NOT_NULL;
    mamaTimerCb   onTimerDestroyed  = (mamaTimerCb) NOT_NULL;
    double        interval          =               0.0;
    void*         closure           =               NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTimerCreate(&result, nativeQueueHandle,
                                              NULL, onTimerDestroyed,
                                              interval, mTimerParent, closure));
}

TEST_F (MiddlewareTimerTests, DISABLED_createInvalidDestroyCB)
{
    timerBridge   result            = (timerBridge) NOT_NULL;
    void*         nativeQueueHandle =               NOT_NULL;
    mamaTimerCb   action            = (mamaTimerCb) NOT_NULL;
    double        interval          =               0.0;
    void*         closure           =               NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTimerCreate(&result, nativeQueueHandle,
                                              action, NULL,
                                              interval, mTimerParent, closure));
}

/* TODO: Determine if interval maybe a NULL value */
TEST_F (MiddlewareTimerTests, DISABLED_createInvalidInterval)
{
    timerBridge   result            = (timerBridge) NOT_NULL;
    void*         nativeQueueHandle =               NOT_NULL;
    mamaTimerCb   action            = (mamaTimerCb) NOT_NULL;
    mamaTimerCb   onTimerDestroyed  = (mamaTimerCb) NOT_NULL;
    void*         closure           =               NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTimerCreate(&result, nativeQueueHandle,
                                              action, onTimerDestroyed,
                                              0, mTimerParent, closure));
}

TEST_F (MiddlewareTimerTests, createInvalidParent)
{
    timerBridge   result            = (timerBridge) NOT_NULL;
    void*         nativeQueueHandle =               NOT_NULL;
    mamaTimerCb   action            = (mamaTimerCb) NOT_NULL;
    mamaTimerCb   onTimerDestroyed  = (mamaTimerCb) NOT_NULL;
    double        interval          =               0.0;
    void*         closure           =               NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTimerCreate(&result, nativeQueueHandle,
                                              action, onTimerDestroyed,
                                              interval, NULL, closure));
}

TEST_F (MiddlewareTimerTests, destroy)
{
    double        interval      =             1;
    void*         handle        =             NULL;
    
    ASSERT_EQ(MAMA_STATUS_OK, 
              mamaQueue_getNativeHandle(mDefaultQueue, &handle));
    
    ASSERT_EQ(MAMA_STATUS_OK, 
              mBridge->bridgeMamaTimerCreate(&mTimerBridge, handle, onTick,
                                             onTimerDestroy, interval,
                                             mTimerParent, (void*)this));

    /* Will block until timer destroys itself */
    mBridge->bridgeStart(mDefaultQueue);
}

TEST_F (MiddlewareTimerTests, destroyInvalid)
{
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTimerDestroy(NULL));
}

TEST_F (MiddlewareTimerTests, reset)
{
    double        interval      =             1;
    void*         handle        =             NULL;

    ASSERT_EQ(MAMA_STATUS_OK, 
              mamaQueue_getNativeHandle(mDefaultQueue, &handle));
    
    ASSERT_EQ(MAMA_STATUS_OK, 
              mBridge->bridgeMamaTimerCreate(&mTimerBridge, handle, onTick,
                                             onTimerDestroy, interval,
                                             mTimerParent, (void*)this));

    ASSERT_EQ(MAMA_STATUS_OK, 
              mBridge->bridgeMamaTimerReset(mTimerBridge));

    /* Will block until timer destroys itself */
    mBridge->bridgeStart(mDefaultQueue);
}

TEST_F (MiddlewareTimerTests, resetInvalid)
{
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, mBridge->bridgeMamaTimerReset(NULL));
}

TEST_F (MiddlewareTimerTests, setInterval)
{
    double        interval      =             0.001;
    void*         handle        =             NULL;
    mama_f64_t    newInterval   =             0.01;

    ASSERT_EQ(MAMA_STATUS_OK, 
              mamaQueue_getNativeHandle(mDefaultQueue, &handle));
    
    ASSERT_EQ(MAMA_STATUS_OK, 
              mBridge->bridgeMamaTimerCreate(&mTimerBridge, handle, onTick,
                                             onTimerDestroy, interval,
                                             mTimerParent, (void*)this));

    ASSERT_EQ(MAMA_STATUS_OK, 
              mBridge->bridgeMamaTimerSetInterval(mTimerBridge,newInterval));

    /* Will block until timer destroys itself */
    mBridge->bridgeStart(mDefaultQueue);

}

TEST_F (MiddlewareTimerTests, setIntervalInvalidTimerBridge)
{
    mama_f64_t  interval = 500;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTimerSetInterval(NULL,interval));
}

/* TODO: Determine if interval can be set to a NULL value (0.0 in this case) */
TEST_F (MiddlewareTimerTests, DISABLED_setIntervalInvalidInterval)
{
    timerBridge timer = (timerBridge) NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTimerSetInterval(timer, 0));
}

TEST_F (MiddlewareTimerTests, getInterval)
{
    double        interval      =             0.001;
    void*         handle        =             NULL;
    mama_f64_t    newInterval   =             0.01;
    mama_f64_t    testInterval  =             0.005;

    ASSERT_EQ(MAMA_STATUS_OK, 
              mamaQueue_getNativeHandle(mDefaultQueue, &handle));

    ASSERT_EQ(MAMA_STATUS_OK, 
              mBridge->bridgeMamaTimerCreate(&mTimerBridge, handle, onTick,
                                             onTimerDestroy, interval,
                                             mTimerParent, (void*)this));

    ASSERT_EQ(MAMA_STATUS_OK, 
              mBridge->bridgeMamaTimerSetInterval(mTimerBridge,newInterval));

    ASSERT_EQ(MAMA_STATUS_OK, 
              mBridge->bridgeMamaTimerGetInterval(mTimerBridge,&testInterval));

    ASSERT_EQ(testInterval, newInterval);

    /* Will block until timer destroys itself */
    mBridge->bridgeStart(mDefaultQueue);
}

TEST_F (MiddlewareTimerTests, getIntervalInvalidTimerBridge)
{
    mama_f64_t  interval = 0.005;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG,
               mBridge->bridgeMamaTimerGetInterval(NULL,&interval));
}

TEST_F (MiddlewareTimerTests, getIntervalInvalidInterval)
{
    timerBridge timer = (timerBridge) NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaTimerGetInterval(timer,NULL));
}
