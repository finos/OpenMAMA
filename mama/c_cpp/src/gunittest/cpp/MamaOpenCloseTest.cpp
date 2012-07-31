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

/* ************************************************************************* */
/* Includes */
/* ************************************************************************* */
#include "MamaOpenCloseTest.h"
#include "wombat/wincompat.h"

/* ************************************************************************* */
/* Construction and Destruction */
/* ************************************************************************* */

MamaOpenCloseTest::MamaOpenCloseTest(void)
{    
}

MamaOpenCloseTest::~MamaOpenCloseTest(void)
{
}

/* ************************************************************************* */
/* Setup and Teardown */
/* ************************************************************************* */

void MamaOpenCloseTest::SetUp(void)
{   
	// Save the this pointer in the member variable to get around gtest problems
	m_this = this;
}

void MamaOpenCloseTest::TearDown(void)
{
    // Clean the member this pointer
    m_this = NULL;   
}

/* ************************************************************************* */
/* Tests */
/* ************************************************************************* */
TEST_F(MamaOpenCloseTest, OpenClose)
{
    // Load the bridge
    Mama::loadBridge(getMiddleware());

    // Open mama
    Mama::open();

    // Close mama
    Mama::close();
}

TEST_F(MamaOpenCloseTest, NestedOpenClose)
{
    // Load the bridge
    Mama::loadBridge(getMiddleware());

    printf("Attempt first open\n");
    Mama::open();

    printf("Attempt second open\n");
    Mama::open();

    printf("Attempt first close\n");
    Mama::close();

    printf("Attempt second close\n");
    Mama::close();
    
    printf("Finished nested testing\n");
}

TEST_F(MamaOpenCloseTest, OpenCloseReopenSameBridge)
{
    // Load the bridge
    Mama::loadBridge(getMiddleware());

    // Open mama
    Mama::open();

    // Close mama
    Mama::close();

    // Open again, this will cause an error as the bridge has not been reloaded
    try
    {
        Mama::open();    
    }

    catch(MamaStatus status)
    {
        return;
    }

    // If we get here there is a problem
    ASSERT_TRUE(1);
}

TEST_F(MamaOpenCloseTest, OpenCloseReopenNewBridge)
{
    // Load the bridge
    Mama::loadBridge("wmw");

    // Open mama
    Mama::open();

    // Close mama
    Mama::close();

    // Load the lbm bridge
    Mama::loadBridge("lbm");

    // Open again
    Mama::open();    

    // Close again
    Mama::close();
}

//CPP Test
void MamaOpenCloseTest::StartBackgroundCallback::onStartComplete(MamaStatus status)
{
    printf("Start completed\n");
}

TEST_F(MamaOpenCloseTest, StartStopDifferentThreads)
{
    // Load the wmw bridge
    mamaBridge bridge = Mama::loadBridge(getMiddleware());

    // Open mama
    Mama::open();
    
    StartBackgroundCallback* startBackgroundCB = new MamaOpenCloseTest::StartBackgroundCallback();
    
    // Start mama in the background so it uses a different thread
    Mama::startBackground(bridge, (MamaStartCallback*) startBackgroundCB);

    // Sleep to allow the other thread to complete startup
    sleep(2);

    // Stop mama from the main thread
    mama_stop(bridge);

    // Close mama
    Mama::close();
}
