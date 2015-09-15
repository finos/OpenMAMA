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

#include <gtest/gtest.h>
#include "mama/mama.h"
#include "mama/status.h"
#include "MainUnitTestC.h"


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

static void MAMACALLTYPE startCallback (mama_status status)
{
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
    mamaBridge mBridge;
    mama_loadBridge (&mBridge, getMiddleware());

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
    mamaBridge mBridge;
    mama_loadBridge (&mBridge, getMiddleware());

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
    mamaBridge mBridge;
    mama_loadBridge (&mBridge, getMiddleware());

    ASSERT_EQ (MAMA_STATUS_OK,  mama_open());

    ASSERT_EQ (MAMA_STATUS_OK, mama_close());
    
    /* bridge must be loaded again after close */
    mama_loadBridge (&mBridge, getMiddleware());
    
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
    mamaBridge mBridge;
    ASSERT_EQ (MAMA_STATUS_OK,  mama_loadBridge (&mBridge, getMiddleware()));

    ASSERT_EQ (MAMA_STATUS_OK,  mama_open());
 
    ASSERT_EQ (MAMA_STATUS_OK,  mama_close());

    ASSERT_EQ (MAMA_STATUS_OK,  mama_loadBridge (&mBridge, "avis"));

    ASSERT_EQ (MAMA_STATUS_OK,  mama_open());

    ASSERT_EQ (MAMA_STATUS_OK,  mama_close());
}

/*  Description:     Load the middleware bridge, initialize MAMA, begin
 *                   processing messages in a non-default thread, stop
 *                   processing on the non-default thread, close MAMA.
 *
 *  Expected Result: MAMA_STATUS_OK
 */
TEST_F (MamaOpenCloseTestC, StartStopDifferentThreads)
{
    mamaBridge mBridge;
    mama_loadBridge (&mBridge, getMiddleware());

    ASSERT_EQ (MAMA_STATUS_OK, mama_open());

    /* Start mama in the background so it uses a different thread */
    ASSERT_EQ (MAMA_STATUS_OK, mama_startBackground (mBridge, startCallback));

    /* Sleep to allow the other thread to complete startup */
    sleep(2);

    ASSERT_EQ (MAMA_STATUS_OK, mama_stop (mBridge));

    ASSERT_EQ (MAMA_STATUS_OK, mama_close());
}

