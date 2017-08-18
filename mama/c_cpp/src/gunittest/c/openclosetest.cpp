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

/*  Description: These tests will check the basic functionality behind
 *               opening and closing middleware bridges.
 */

#include "MainUnitTestC.h"
#include <gtest/gtest.h>
#include "mama/mama.h"
#include "mama/status.h"
#include "wombat/wSemaphore.h"

class MamaOpenCloseTestC : public ::testing::Test
{
protected:

    MamaOpenCloseTestC(void);
    virtual ~MamaOpenCloseTestC(void);

    virtual void SetUp(void);
    virtual void TearDown(void);

};

MamaOpenCloseTestC::MamaOpenCloseTestC(void)
{
}

MamaOpenCloseTestC::~MamaOpenCloseTestC(void)
{
}

void MamaOpenCloseTestC::SetUp(void)
{
}

void MamaOpenCloseTestC::TearDown(void)
{
}

static void MAMACALLTYPE startCallback (mama_status status,
                                        mamaBridge  bridge,
                                        void*       closure)
{
    wsem_t* sem = (wsem_t*)closure;
    ASSERT_EQ (0, wsem_post (sem));
}

void MAMACALLTYPE onEventStop (mamaQueue queue, void* closure)
{
    mamaBridge bridge = (mamaBridge)closure;
    mama_stop (bridge);
}

/* ************************************************************************* */
/* Tests */
/* ************************************************************************* */

/*  Description:     Load the middleware bridge, initialize MAMA, close MAMA.
 *
 *  Expected Result: MAMA_STATUS_OK 
 */
TEST_F (MamaOpenCloseTestC, OpenClose)
{
    mamaBridge bridge;
    mama_loadBridge (&bridge, getMiddleware());

    ASSERT_EQ (MAMA_STATUS_OK, mama_open());

    ASSERT_EQ (MAMA_STATUS_OK, mama_close());
}

/*  Description:     Load the middleware bridge, initialize MAMA, increase the
 *                   reference count to 2 by calling mama_open() a second time,
 *                   then close MAMA by calling mama_close() twice.
 *
 *  Expected Result: MAMA_STATUS_OK 
 */
TEST_F (MamaOpenCloseTestC, NestedOpenClose)
{
    mamaBridge bridge;
    mama_loadBridge (&bridge, getMiddleware());

    ASSERT_EQ (MAMA_STATUS_OK, mama_open());

    ASSERT_EQ (MAMA_STATUS_OK, mama_open());

    ASSERT_EQ (MAMA_STATUS_OK, mama_close());

    ASSERT_EQ (MAMA_STATUS_OK, mama_close());

}

/*  Description:     Load the middleware bridge, initialize MAMA, close MAMA,
 *                   re-initialize MAMA.
 *
 *  Expected Result: MAMA_STATUS_OK 
 */
TEST_F (MamaOpenCloseTestC, OpenCloseReopenSameBridge)
{
    mamaBridge bridge;
    mama_loadBridge (&bridge, getMiddleware());

    ASSERT_EQ (MAMA_STATUS_OK,  mama_open());

    ASSERT_EQ (MAMA_STATUS_OK, mama_close());
    
    /* bridge must be loaded again after close */
    ASSERT_EQ (MAMA_STATUS_OK, mama_loadBridge (&bridge, getMiddleware()));
    
    ASSERT_EQ (MAMA_STATUS_OK, mama_open());

    ASSERT_EQ (MAMA_STATUS_OK, mama_close());
}

/*  Description:     Load a wombat middleware bridge, initialize MAMA, close
 *                   MAMA, Load an LBM middleware bridge, close MAMA.
 *
 *  Expected Result: MAMA_STATUS_OK.
 */
TEST_F (MamaOpenCloseTestC, DISABLED_OpenCloseReopenNewBridge)
{
    mamaBridge bridge;
    ASSERT_EQ (MAMA_STATUS_OK, mama_loadBridge (&bridge, getMiddleware()));

    ASSERT_EQ (MAMA_STATUS_OK, mama_open());
 
    ASSERT_EQ (MAMA_STATUS_OK, mama_close());

    ASSERT_EQ (MAMA_STATUS_OK, mama_loadBridge (&bridge, "qpid"));

    ASSERT_EQ (MAMA_STATUS_OK, mama_open());

    ASSERT_EQ (MAMA_STATUS_OK, mama_close());
}

/*  Description:     Load the middleware bridge, initialize MAMA, begin
 *                   processing messages in a non-default thread, stop
 *                   processing on the non-default thread, close MAMA.
 *
 *  Expected Result: MAMA_STATUS_OK
 */
TEST_F (MamaOpenCloseTestC, StartStopDifferentThreads)
{
    mamaBridge bridge;
    ASSERT_EQ (MAMA_STATUS_OK, mama_loadBridge (&bridge, getMiddleware()));

    ASSERT_EQ (MAMA_STATUS_OK, mama_open());

    /* Start mama in the background so it uses a different thread */
    wsem_t sem;
    mamaQueue defaultQueue;
    ASSERT_EQ (0, wsem_init (&sem, 0, 0));
    ASSERT_EQ (MAMA_STATUS_OK, mama_getDefaultEventQueue (bridge, &defaultQueue));
    ASSERT_EQ (MAMA_STATUS_OK, mamaQueue_enqueueEvent (defaultQueue, onEventStop, bridge));
    ASSERT_EQ (MAMA_STATUS_OK, mama_startBackgroundEx (bridge, startCallback, &sem));
    ASSERT_EQ (0, wsem_wait (&sem));

    ASSERT_EQ (MAMA_STATUS_OK, mama_close());
}

/* Description:     Load the middleware bridge, search for the same bridge.  
 */
TEST_F (MamaOpenCloseTestC, GetBridge)
{
    mamaBridge bridge;
    mamaBridge foundBridge;

    ASSERT_EQ (MAMA_STATUS_OK, mama_loadBridge (&bridge, getMiddleware ()));

    ASSERT_EQ (MAMA_STATUS_OK, mama_getMiddlewareBridge (&foundBridge, getMiddleware ()));
    ASSERT_EQ (bridge, foundBridge);
    ASSERT_TRUE (NULL != foundBridge);
}

/* Description:     Load the middleware bridge, search for a different bridge.  
 */
TEST_F (MamaOpenCloseTestC, GetNotFoundBridge)
{
    mamaBridge bridge;
    mamaBridge foundBridge;

    ASSERT_EQ (MAMA_STATUS_OK, mama_loadBridge (&bridge, getMiddleware ()));

    /* Using "pizza"" as an example, in case someone decides "test" is a good name. */
    ASSERT_EQ (MAMA_STATUS_NOT_FOUND, mama_getMiddlewareBridge (&foundBridge, "pizza"));
    ASSERT_NE (bridge, foundBridge);
    ASSERT_TRUE (NULL == foundBridge);
}

/* Description:     Load the middleware bridge, pass a NULL bridge.  
 */
TEST_F (MamaOpenCloseTestC, GetNullBridge)
{
    mamaBridge bridge;

    ASSERT_EQ (MAMA_STATUS_OK, mama_loadBridge (&bridge, getMiddleware ()));

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, mama_getMiddlewareBridge (NULL, getMiddleware ()));
}

/* Description:     Load the middleware bridge, search for a different bridge.  
 */
TEST_F (MamaOpenCloseTestC, GetNullBridgeName)
{
    mamaBridge bridge;
    mamaBridge foundBridge;

    ASSERT_EQ (MAMA_STATUS_OK, mama_loadBridge (&bridge, getMiddleware ()));

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, mama_getMiddlewareBridge (&foundBridge, NULL));
}

/* Description:     Load the payload bridge, search for the same bridge.
 */
TEST_F (MamaOpenCloseTestC, GetPayload)
{
    mamaPayloadBridge bridge;
    mamaPayloadBridge foundBridge;

    ASSERT_EQ (MAMA_STATUS_OK, mama_loadPayloadBridge (&bridge, getPayload ()));

    ASSERT_EQ (MAMA_STATUS_OK, mama_getPayloadBridge (&foundBridge, getPayload ()));
    ASSERT_EQ (bridge, foundBridge);
    ASSERT_TRUE (NULL   != foundBridge);
}

/* Description:     Load the payload bridge, search for the same bridge.
 */
TEST_F (MamaOpenCloseTestC, GetNotFoundPayload)
{
    mamaPayloadBridge bridge;
    mamaPayloadBridge foundBridge;

    ASSERT_EQ (MAMA_STATUS_OK, mama_loadPayloadBridge (&bridge, getPayload ()));

    /* Use "pizza" in case someone decides to use a "test" payload */ 
    ASSERT_EQ (MAMA_STATUS_NOT_FOUND, mama_getPayloadBridge (&foundBridge, "pizza"));
    ASSERT_NE (bridge, foundBridge);
    ASSERT_TRUE (NULL   == foundBridge);
}

/* Description:     Load the payload bridge, pass a NULL bridge.  
 */
TEST_F (MamaOpenCloseTestC, GetNullPayload)
{
    mamaPayloadBridge bridge;

    ASSERT_EQ (MAMA_STATUS_OK, mama_loadPayloadBridge (&bridge, getPayload ()));

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, mama_getPayloadBridge (NULL, getPayload ()));
}

/* Description:     Load the payload bridge, pass a null bridge name.  
 */
TEST_F (MamaOpenCloseTestC, GetNullPayloadBridge)
{
    mamaPayloadBridge bridge;
    mamaPayloadBridge foundBridge;

    ASSERT_EQ (MAMA_STATUS_OK, mama_loadPayloadBridge (&bridge, getPayload ()));

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, mama_getPayloadBridge (&foundBridge, NULL));
}

/*  Description:     Load the middleware bridge, open and close
 *                   150 times, then ensure that you can continue
 *                   to open and close without errors due to limit
 *                   of plugins having been reached.
 *
 *  Expected Result: MAMA_STATUS_OK
 */

TEST_F(MamaOpenCloseTestC, OpenCloseReopenManyTimes)
{
    mamaBridge bridge;

    for(int i = 0; i < 150; i++)
    {
        mama_loadBridge (&bridge, getMiddleware());
        mama_open();
        mama_close();

    }
        ASSERT_EQ (MAMA_STATUS_OK, mama_loadBridge (&bridge, getMiddleware()));
        ASSERT_EQ(MAMA_STATUS_OK, mama_open());
        ASSERT_EQ(MAMA_STATUS_OK, mama_close());
}

