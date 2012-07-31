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

#include <gtest/gtest.h>
#include "MainUnitTestC.h"
#include "wombat/wConfig.h"
#include "mama/types.h"
#include "mama/timer.h"
#include "mama/queue.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>


class MamaTimerTestC : public ::testing::Test
{	
protected:
    MamaTimerTestC();          
    virtual ~MamaTimerTestC(); 

    virtual void SetUp();        
    virtual void TearDown ();    
public:
    // Initialise all member variables
    int             m_numberForCallbacks;
    int             m_numberForTimers;
    int             m_numberRecursiveCallbacks;
    double          m_timerInterval;    
    mamaBridge      m_bridge;
    mamaQueue       m_defaultQueue;
    mamaTransport   m_transport;

    mamaTimer       mtarray[1000];

    MamaTimerTestC *m_this;
    char transportName[10];

    mamaTimer shortTimer;
    mamaTimer longTimer;
    mamaTimer stopperTimer;
    mamaTimer recursiveTimer;

    mamaTimerCb m_TickCallback;
    mamaTimerCb m_DestroyCallback;

};

MamaTimerTestC::MamaTimerTestC()
{
    m_numberForCallbacks        = 0;
    m_numberForTimers           = 10;
    m_numberRecursiveCallbacks  = 0;
    m_timerInterval             = 0.0001;    
    m_bridge                    = NULL;
    m_defaultQueue              = NULL;
    m_transport                 = NULL;
    shortTimer                  = NULL;
    longTimer                   = NULL;
    stopperTimer                = NULL;
    recursiveTimer              = NULL;

    m_TickCallback              = NULL;
    m_DestroyCallback           = NULL;
    m_this                      = NULL;
}

MamaTimerTestC::~MamaTimerTestC()
{
}

void MamaTimerTestC::SetUp(void)
{	
	// Save the this pointer in the member variable to get around gtest problems
	m_this = this;
    
    mama_loadBridge(&m_bridge, getMiddleware());

    mama_open();

    mama_getDefaultEventQueue(m_bridge, &m_defaultQueue);

    transportName[0] = '\0';
    strncat(transportName, "sub_", 5);
    strncat(transportName, getMiddleware(), 4);

    mamaTransport_allocate(&m_transport);
    mamaTransport_create(m_transport, transportName, m_bridge); 
}

void MamaTimerTestC::TearDown(void)
{
    m_this = NULL;
}

static void MAMACALLTYPE onTimerTick(mamaTimer timer, void* closure)
{
    printf("Timer ticking....\n");
    mamaTimer_destroy(timer);
}

static void MAMACALLTYPE onTimerDestroy(mamaTimer timer, void* closure)
{
    printf("Timer destroying...\n");
    MamaTimerTestC *fixture = (MamaTimerTestC *)closure;
    fixture->m_numberForCallbacks ++;
    printf("m_numberForCallbacks = %i\n", fixture->m_numberForCallbacks);

    // If all the timers have been destroyed then quit out
    if(fixture->m_numberForCallbacks == fixture->m_numberForTimers)
    {
        printf("Stopping timer\n");
        mama_stop(fixture->m_bridge);
    }
}

static void MAMACALLTYPE onStopDispatchingTimerTick(mamaTimer timer, void* closure)
{
    mamaTimer_destroy(timer);
}

static void MAMACALLTYPE onStopDispatchingTimerDestroy(mamaTimer timer, void* closure)
{
    printf("Stopping Two Timers ticking\n");
    MamaTimerTestC* fixture = (MamaTimerTestC *)closure;
    mama_stop(fixture->m_bridge);

    mamaTimer_destroy(fixture->shortTimer);
    mamaTimer_destroy(fixture->longTimer);
}


static void MAMACALLTYPE onRecursiveTimerTick(mamaTimer timer, void* closure)
{
    MamaTimerTestC *fixture = (MamaTimerTestC*) closure;
    EXPECT_EQ(mamaTimer_destroy(timer), MAMA_STATUS_OK);
    fixture->recursiveTimer = NULL;
}


static void MAMACALLTYPE onRecursiveTimerDestroy(mamaTimer timer, void* closure)
{
    // Cast the closure to a test fixture
    printf("in recursive onDestroy\n");
    MamaTimerTestC *fixture = (MamaTimerTestC *)closure;

    // Increment the number of times this function has been called
     fixture->m_numberRecursiveCallbacks ++;

    // If this is the 10th pass then quit out
    if(fixture->m_numberRecursiveCallbacks == 10)
    {
        //EXPECT_EQ(mama_stop(fixture->m_init->bridge), MAMA_STATUS_OK);
        mama_stop(fixture->m_bridge);
    }
    else
    {
        mamaTimer_create2(&fixture->recursiveTimer, fixture->m_defaultQueue, onRecursiveTimerTick, onRecursiveTimerDestroy, fixture->m_timerInterval, fixture->m_this);
    }
}


static void MAMACALLTYPE onTwoTimerTick(mamaTimer timer, void* closure)
{
    printf("In TwoTimer Tick\n");

}

static void MAMACALLTYPE onTwoTimerDestroy(mamaTimer timer, void * closure)
{
    // Cast the closure to a test fixture
    printf("In TwoTimer Destroy\n");
}
/* ************************************************************************* */
/* Test Functions */
/* ************************************************************************* */

/*  Description :   This test will create a large number of timers in a for loop, each one will
 *                  be freed in its callback function.
 */
TEST_F(MamaTimerTestC, ForTimer)
{
    // Create a whole lot of timers
    for(int counter=0; counter<m_numberForTimers; counter++)
    {
        printf("Creating new timer\n");
        m_timerInterval = (counter + 1)/100;
        mamaTimer_create2(&mtarray[counter], m_defaultQueue, onTimerTick, onTimerDestroy, m_timerInterval, m_this);
    }

    ASSERT_EQ(MAMA_STATUS_OK, mama_start(m_bridge));
 
}    


/*  Description :   This function will create a timer which will create another timer
 *                  in its callback function, (the first timer will be destroyed).
 */

TEST_F(MamaTimerTestC, RecursiveTimer)
{    
    //Store the call
    mamaTimer_create2(&recursiveTimer, m_defaultQueue, onRecursiveTimerTick, onRecursiveTimerDestroy, m_timerInterval, m_this);

    // Start processing messages, after 10 timers have been created this will stop blocking
    mama_start(m_bridge);
}    


/*  Description :   This test will create 2 timers, the first will constantly reset itself
 *                  while the second will continue to reset the first timer as well.
 *                  The whole test will run for 3 seconds.
 */
TEST_F(MamaTimerTestC, TwoTimer)
{

    double interval = 0.1;
    double testDuration = 3.0;

    mamaTimer_create2(&shortTimer, m_defaultQueue, onTwoTimerTick, onTwoTimerDestroy, interval, m_this);
    mamaTimer_create2(&longTimer, m_defaultQueue, onTwoTimerTick, onTwoTimerDestroy, interval*2.0, m_this); //use same callback

    mamaTimer_create2(  &stopperTimer, 
                        m_defaultQueue, 
                        onStopDispatchingTimerTick, 
                        onStopDispatchingTimerDestroy, 
                        testDuration, 
                        m_this); 
    
    mama_start(m_bridge);
    
}

