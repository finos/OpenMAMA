/* $Id$
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

/* Description: This test checks the functionality behind  the creation and 
 *              destruction  of subscriptions. 
 */

#include "gtest/gtest.h"
#include "mama/mama.h"
#include "MainUnitTestC.h"
#include "mama/subscription.h"
#include "mama/source.h"
#include "mama/types.h"


class MamaSubscriptionTest : public ::testing::Test
{
public:
    mamaBridge mBridge;
protected:

    /* Work around for problem in gtest where the this pointer can't be accessed
     * from a test fixture.
     */
    MamaSubscriptionTest *m_this;
    
    mamaQueue         m_defaultQueue;
    mamaMsgCallbacks  simpleCallback;
    mamaSubscription  subscription;
    mamaTransport     m_transport;
    char              transportName[10];

protected:

    MamaSubscriptionTest(void);
    virtual ~MamaSubscriptionTest(void);

    virtual void SetUp(void);
    virtual void TearDown(void);
};

static void MAMACALLTYPE onSimpleCallbackCreate (mamaSubscription sub, void* closure)
{
}


static void MAMACALLTYPE onSimpleCallbackDestroy (mamaSubscription sub, void* closure)
{
    MamaSubscriptionTest* mamaSubscriptionTest = (MamaSubscriptionTest*) closure;
    mama_stop(mamaSubscriptionTest->mBridge);
}


static void MAMACALLTYPE onSimpleCallbackMsg (mamaSubscription sub, 
                                              mamaMsg          msg, 
                                              void*            closure, 
                                              void*            itemClosure)
{
    MamaSubscriptionTest* mamaSubscriptionTest = (MamaSubscriptionTest*) closure;
    mama_stop(mamaSubscriptionTest->mBridge);
}


static void MAMACALLTYPE onSimpleCallbackRecapRequest (mamaSubscription sub, void* closure)
{
}


static void MAMACALLTYPE onSimpleCallbackQuality (mamaSubscription sub, 
                                                  mamaQuality      quality, 
                                                  const char*      symbol, 
                                                  short            cause, 
                                                  const void*      platformInfo, 
                                                  void*            closure)
{
}


static void MAMACALLTYPE onSimpleCallbackGap (mamaSubscription sub, void* closure)
{
}


MamaSubscriptionTest::MamaSubscriptionTest(void)
{
    mBridge             = NULL;
    m_transport         = NULL;
    m_defaultQueue      = NULL;

    memset(&simpleCallback, 0, sizeof(simpleCallback));
    simpleCallback.onCreate        = onSimpleCallbackCreate;
    simpleCallback.onDestroy       = onSimpleCallbackDestroy;
    simpleCallback.onMsg           = onSimpleCallbackMsg;
    simpleCallback.onQuality       = onSimpleCallbackQuality;
    simpleCallback.onGap           = onSimpleCallbackGap;
    simpleCallback.onRecapRequest  = onSimpleCallbackRecapRequest;

    subscription = NULL;    

}


MamaSubscriptionTest::~MamaSubscriptionTest(void)
{
}


void MamaSubscriptionTest::SetUp(void)
{   
	/* Save the this pointer in the member variable to get around gtest problems */
	m_this = this;

    mama_loadBridge (&mBridge, getMiddleware());

    mama_open();

    transportName[0] = '\0';
    strncat (transportName, "sub_", 5);
    strncat (transportName, getMiddleware(), 4);

    mamaTransport_allocate (&m_transport);
    mamaTransport_create (m_transport, transportName, mBridge);
}


void MamaSubscriptionTest::TearDown(void)
{
    mamaTransport_destroy (m_transport);
    m_transport = NULL;

    mama_close();

    m_this = NULL;   
}


/* ************************************************************************* */
/* Test Functions                                                            */
/* ************************************************************************* */

/*  Description:     Create a test source, create a subscription to the test
 *                   source, destroy the subscription, delete the subscription.
 *
 *  Expected Result: MAMA_STATUS_OK
 */
TEST_F (MamaSubscriptionTest, SubscriptionCreateDestroy)
{
    ASSERT_EQ (MAMA_STATUS_OK, mamaSubscription_allocate (&subscription));
    if(NULL != subscription)
    {        
        /* Get the default queue */
        mama_getDefaultEventQueue (mBridge, &m_defaultQueue);
    
        /* create a test source */
        mamaSource testSource = NULL;
        mamaSource_create (&testSource);
        mamaSource_setId (testSource, "TestSource");
        mamaSource_setTransport (testSource, m_transport);
        mamaSource_setSymbolNamespace (testSource, "WOMBAT");

        /* Create the subscription */
        ASSERT_EQ (MAMA_STATUS_OK, 
                   mamaSubscription_create (subscription, 
                                            m_defaultQueue, 
                                            &simpleCallback, 
                                            testSource, 
                                            "TEST_SYMBOL", 
                                            m_this));
        

        /* Destroy the subscription */
        ASSERT_EQ (MAMA_STATUS_OK, mamaSubscription_destroy (subscription));

        /* Deallocate subscription memory */
        ASSERT_EQ (MAMA_STATUS_OK, mamaSubscription_deallocate (subscription));

        ASSERT_EQ (MAMA_STATUS_OK, mamaSource_destroy (testSource));
    }
}
