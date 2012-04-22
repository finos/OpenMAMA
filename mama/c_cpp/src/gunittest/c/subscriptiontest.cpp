/*----------------------------------------------------------------------------
 *
 * $Id$
 *
 * Copyright (c) 1999-2007 Wombat Financial Software Inc., of Incline
 * Village, NV.  All rights reserved.
 * 
 * This software and documentation constitute an unpublished work and
 * contain valuable trade secrets and proprietary information belonging
 * to Wombat.  None of this material may be copied, duplicated or
 * disclosed without the express written permission of Wombat.  
 * 
 * WOMBAT EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES CONCERNING THIS
 * SOFTWARE AND DOCUMENTATION, INCLUDING ANY WARRANTIES OF
 * MERCHANTABILITY AND/OR FITNESS FOR ANY PARTICULAR PURPOSE, AND
 * WARRANTIES OF PERFORMANCE, AND ANY WARRANTY THAT MIGHT OTHERWISE ARISE
 * FROM COURSE OF DEALING OR USAGE OF TRADE. NO WARRANTY IS EITHER
 * EXPRESS OR IMPLIED WITH RESPECT TO THE USE OF THE SOFTWARE OR
 * DOCUMENTATION.  
 * 
 * Under no circumstances shall Wombat be liable for incidental, special,
 * indirect, direct or consequential damages or loss of profits,
 * interruption of business, or related expenses which may arise from use
 * of software or documentation, including but not limited to those
 * resulting from defects in software and/or documentation, or loss or
 * inaccuracy of data of any kind.
 *
 *--------------------------------------------------------------------------*/

#include "gtest/gtest.h"
#include "mama/mama.h"
#include "MainUnitTestC.h"
#include "mama/subscription.h"
#include "mama/source.h"
#include "mama/types.h"

//using namespace Wombat;

class MamaSubscriptionTest : public ::testing::Test
{
public:
    mamaBridge m_bridge;
protected:

    /* Work around for problem in gtest where the this pointer can't be accessed
     * from a test fixture.
     */
    MamaSubscriptionTest *m_this;

    mamaQueue m_defaultQueue;

    mamaMsgCallbacks simpleCallback;
    
    mamaSubscription subscription;

    mamaTransport m_transport;
    char transportName[10];

protected:

    MamaSubscriptionTest(void);
    virtual ~MamaSubscriptionTest(void);

    virtual void SetUp(void);
    virtual void TearDown(void);
};

static void MAMACALLTYPE onSimpleCallbackCreate(mamaSubscription sub, void* closure)
{
}

static void MAMACALLTYPE onSimpleCallbackDestroy(mamaSubscription sub, void* closure)
{
    MamaSubscriptionTest* mamaSubscriptionTest = (MamaSubscriptionTest*) closure;
    mama_stop(mamaSubscriptionTest->m_bridge);
}

static void MAMACALLTYPE onSimpleCallbackMsg(mamaSubscription sub, mamaMsg msg, void* closure, void* itemClosure)
{
    MamaSubscriptionTest* mamaSubscriptionTest = (MamaSubscriptionTest*) closure;
    mama_stop(mamaSubscriptionTest->m_bridge);
}

static void MAMACALLTYPE onSimpleCallbackRecapRequest(mamaSubscription sub, void* closure)
{
}

static void MAMACALLTYPE onSimpleCallbackQuality(mamaSubscription sub, mamaQuality quality, const char* symbol, short cause, const void* platformInfo, void* closure)
{
}

static void MAMACALLTYPE onSimpleCallbackGap(mamaSubscription sub, void* closure)
{
}

MamaSubscriptionTest::MamaSubscriptionTest(void)
{
    m_bridge            = NULL;
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

    //recreateOnMsgCallback = ;
    //recreateOnDestroyCallback = NULL;
}

MamaSubscriptionTest::~MamaSubscriptionTest(void)
{
}


void MamaSubscriptionTest::SetUp(void)
{   
	// Save the this pointer in the member variable to get around gtest problems
	m_this = this;

    mama_loadBridge(&m_bridge, getMiddleware());

    mama_open();

    transportName[0] = '\0';
    strncat(transportName, "sub_", 5);
    strncat(transportName, getMiddleware(), 4);

    mamaTransport_allocate(&m_transport);
    mamaTransport_create(m_transport, transportName, m_bridge);
}

void MamaSubscriptionTest::TearDown(void)
{
    m_transport = NULL;

    mama_close();

    m_this = NULL;   
}



/* ************************************************************************* */
/* Test Functions - Basic Subscription */
/* ************************************************************************* */

TEST_F(MamaSubscriptionTest, BasicSubscription)
{
    ASSERT_EQ(MAMA_STATUS_OK, mamaSubscription_allocate(&subscription));
    if(NULL != subscription)
    {        
        // Get the default queue
        mama_getDefaultEventQueue(m_bridge, &m_defaultQueue);
    
        //create a test source
        mamaSource testSource = NULL;
        mamaSource_create(&testSource);
        mamaSource_setId(testSource, "TestSource");
        mamaSource_setTransport(testSource, m_transport);
        mamaSource_setSymbolNamespace(testSource, "WOMBAT");
        printf("Note: as there is no real source, this test should hang\n");

        // Create the subscription
        ASSERT_EQ(  MAMA_STATUS_OK, 
            mamaSubscription_create(subscription, m_defaultQueue, &simpleCallback, testSource, "TEST_SYMBOL", m_this));
        
        mama_start(m_bridge);

        // Destroy the subscription
        ASSERT_EQ(MAMA_STATUS_OK, mamaSubscription_destroy(subscription));

        // Delete the basic subscription
        ASSERT_EQ(MAMA_STATUS_OK, mamaSubscription_deallocate(subscription));
    }
}
