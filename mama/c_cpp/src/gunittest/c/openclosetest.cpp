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

#include <gtest/gtest.h>
#include "mama/mama.h"
#include "mama/status.h"
#include "MainUnitTestC.h"

class MamaOpenCloseTestC : public ::testing::Test
{
protected:

    /* Work around for problem in gtest where the this pointer can't be accessed
     * from a test fixture.
     */
    MamaOpenCloseTestC *m_this;

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
    m_this = this;
}

void MamaOpenCloseTestC::TearDown(void)
{
    m_this = NULL;
}

static void MAMACALLTYPE startCallback(mama_status status)
{
    printf("In start in background callback\n");
}


/* ************************************************************************* */
/* Tests */
/* ************************************************************************* */
TEST_F(MamaOpenCloseTestC, OpenClose)
{
    mamaBridge mBridge;
    mama_loadBridge(&mBridge, getMiddleware());

    mama_open();

    mama_close();
}

TEST_F(MamaOpenCloseTestC, NestedOpenClose)
{
    mamaBridge mBridge;
    mama_loadBridge(&mBridge, getMiddleware());

    printf("Attempt first open\n");
    mama_open();

    printf("Attempt second open\n");
    mama_open();

    printf("Attempt first close\n");
    mama_close();

    printf("Attempt second close\n");
    mama_close();

    printf("Finished nested testing\n");
}

TEST_F(MamaOpenCloseTestC, OpenCloseReopenSameBridge)
{
    mamaBridge mBridge;
    mama_loadBridge(&mBridge, getMiddleware());

    mama_open();

    mama_close();

    ASSERT_EQ(MAMA_STATUS_OK, mama_open());

    return;

    // If we get here there is a problem
    ASSERT_TRUE(1);
}

TEST_F(MamaOpenCloseTestC, OpenCloseReopenNewBridge)
{
    mamaBridge mBridge;
    mama_loadBridge(&mBridge, "wmw");

    mama_open();

    mama_close();

    mama_loadBridge(&mBridge, "lbm");

    mama_open();

    mama_close();
}

TEST_F(MamaOpenCloseTestC, StartStopDifferentThreads)
{
    mamaBridge mBridge;
    mama_loadBridge(&mBridge, getMiddleware());

    mama_open();

    // Start mama in the background so it uses a different thread
    ASSERT_EQ(MAMA_STATUS_OK, mama_startBackground(mBridge,  startCallback));

    // Sleep to allow the other thread to complete startup
    sleep(2);

    mama_stop(mBridge);

    mama_close();
}

