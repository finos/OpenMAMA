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
/*
 * Description	:	This test harness will cover MAMA issue 3418 which includes
 *                  several timer tests. See the function header for each 
 *                  fixture for more information. Note that this test should
 *					be run against different versions of LBM including 3.5.2
 *					and an earlier version.
 */

/* ************************************************************************* */
/* Includes */
/* ************************************************************************* */
#include "MamaTimerTest.h"
#include <cstring>
#include <cstdio>

/* ************************************************************************* */
/* Construction and Destruction */
/* ************************************************************************* */
using namespace Wombat;

MamaTimerTestCPP::MamaTimerTestCPP(void)
{	
    // Initialise all member variables
    m_numberForCallbacks        = 0;
    m_numberForTimers           = 1000;
    m_numberRecursiveCallbacks  = 0;
    m_timerInterval             = 0.0001;    
}

MamaTimerTestCPP::~MamaTimerTestCPP(void)
{
}

/* ************************************************************************* */
/* Setup and Teardown */
/* ************************************************************************* */

void MamaTimerTestCPP::SetUp(void)
{	


	// Save the this pointer in the member variable to get around gtest problems
	m_this = this;

    m_bridge = Mama::loadBridge(getMiddleware());

    Mama::open();

    m_defaultQueue = Mama::getDefaultEventQueue(m_bridge);


    transportName[0] = '\0';
    strncat(transportName, "sub_", 5);
    strncat(transportName, getMiddleware(), 4);

    m_transport.create(transportName, m_bridge); 
}

void MamaTimerTestCPP::TearDown(void)
{

    m_this = NULL;
}

/* ************************************************************************* */
/* Protected Functions */
/* ************************************************************************* */




void MamaTimerTestCPP::StopDispatchingTick::onTimer(MamaTimer* timer)
{
    timer->destroy();
}

void MamaTimerTestCPP::StopDispatchingTick::onDestroy(MamaTimer* timer, void * closure)
{
    MamaTimerTestCPP* accessor = (MamaTimerTestCPP *)closure;
    Mama::stop(accessor->m_bridge);
}

void MamaTimerTestCPP::OnForTimerTick::onTimer(MamaTimer* timer)
{
    timer->destroy();
}

void MamaTimerTestCPP::OnForTimerTick::onDestroy(MamaTimer* timer, void * closure)
{

    MamaTimerTestCPP *fixture = (MamaTimerTestCPP *)closure;
    fixture->m_numberForCallbacks ++;

    // If all the timers have been destroyed then quit out
    if(fixture->m_numberForCallbacks == fixture->m_numberForTimers)
    {
        Mama::stop(fixture->m_bridge);
    }
}

void MamaTimerTestCPP::OnForRecursiveTick::onTimer(MamaTimer* timer)
{
    // Stop the timer firing
    //EXPECT_EQ(mamaTimer_destroy(timer), MAMA_STATUS_OK);
    m_callback = (MamaTimerTestCPP::OnForRecursiveTick*)timer->getCallback();  //store the callback for reuse
    timer->destroy();

}

void MamaTimerTestCPP::OnForRecursiveTick::onDestroy(MamaTimer* timer, void * closure)
{
    // Cast the closure to a test fixture
    printf("in recursive onDestroy\n");
    MamaTimerTestCPP *fixture = (MamaTimerTestCPP *)closure;

    // Increment the number of times this function has been called
     fixture->m_numberRecursiveCallbacks ++;

    // If this is the 10th pass then quit out
    if(fixture->m_numberRecursiveCallbacks == 10)
    {
        //EXPECT_EQ(mama_stop(fixture->m_init->bridge), MAMA_STATUS_OK);
        Mama::stop(fixture->m_bridge);
    }
    else
    {
        fixture->mtarray[0].create(fixture->m_defaultQueue, m_callback, fixture->m_timerInterval, fixture->m_this);
    }
}

void MamaTimerTestCPP::OnForTwoTimerTick::onTimer(MamaTimer* timer)
{
    printf("on timer tick\n");

}

void MamaTimerTestCPP::OnForTwoTimerTick::onDestroy(MamaTimer* timer, void * closure)
{
    // Cast the closure to a test fixture
    printf("in TwoTick onDestroy\n");
}

/* ************************************************************************* */
/* Test Functions */
/* ************************************************************************* */

/*  Description :   This test will create a large number of timers in a for loop, each one will
 *                  be freed in its callback function.
 */
TEST_F(MamaTimerTestCPP, ForTimer)
{
    // Create a whole lot of timers
    MamaTimerTestCPP::OnForTimerTick timerCb;
    for(int counter=0; counter<m_numberForTimers; counter++)
    {
        printf("Creating new timer\n");
        m_timerInterval = (counter + 1)/100;
        mtarray[counter].create(m_defaultQueue, &timerCb, m_timerInterval, m_this);
    }
    Mama::start(m_bridge);
    printf("Timers started\n");
 
}    

/*  Description :   This function will create a timer which will create another timer
 *                  in its callback function, (the first timer will be destroyed).
 */

TEST_F(MamaTimerTestCPP, RecursiveTimer)
{
    
    MamaTimerTestCPP::OnForRecursiveTick timerCb;

    mtarray[0].create(m_defaultQueue, &timerCb, m_timerInterval, m_this);

    // Start processing messages, after 10 timers have been created this will stop blocking
    Mama::start(m_bridge);
}    



/*  Description :   This test will create 2 timers, the first will constantly reset itself
 *                  while the second will continue to reset the first timer as well.
 *                  The whole test will run for 20 seconds.
 */

TEST_F(MamaTimerTestCPP, TwoTimer)
{

    MamaTimerTestCPP::OnForTwoTimerTick* timerCb = new MamaTimerTestCPP::OnForTwoTimerTick;
    MamaTimerTestCPP::StopDispatchingTick* stopperCb = new MamaTimerTestCPP::StopDispatchingTick;

    mtarray[0].create(m_defaultQueue, timerCb, m_timerInterval, m_this);
    mtarray[1].create(m_defaultQueue, timerCb, m_timerInterval*2, m_this); //use same callback

    //this timer calls stop destroy on tick and Mama::stop on destroy
    mtarray[2].create(m_defaultQueue, stopperCb, 3, m_this); //3 secs

    Mama::start(m_bridge);
    
    mtarray[0].destroy();
    mtarray[1].destroy();
}

