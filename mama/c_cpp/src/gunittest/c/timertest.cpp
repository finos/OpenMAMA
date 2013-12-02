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
    MamaTimerTestC *m_this;
    mamaBridge mBridge;
    
    int         tCounter;
    int         numTimers;
    mamaTimer   tarray[100];
    mamaTimer   longTimer;
    mamaTimer   shortTimer;
    mamaTimer   stopTimer;
    mamaTimer   timer;
    mamaQueue   queue;
    mama_f64_t  interval;
};

MamaTimerTestC::MamaTimerTestC()
{
}

MamaTimerTestC::~MamaTimerTestC()
{
}

void MamaTimerTestC::SetUp(void)
{	
    interval = 0.01;
    m_this   = this;

    mama_loadBridge (&mBridge, getMiddleware());
    mama_open (); 
    ASSERT_EQ (MAMA_STATUS_OK,
               mama_getDefaultEventQueue (mBridge, &queue));
}

void MamaTimerTestC::TearDown(void)
{
    mama_close ();
    m_this = NULL;
}

static void MAMACALLTYPE onTimerTick (mamaTimer timer, void* closure)
{
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTimer_destroy(timer));
}

static void MAMACALLTYPE onTimerDestroy (mamaTimer timer, void* closure)
{
    MamaTimerTestC* fixture = (MamaTimerTestC *)closure;
    fixture->tCounter++;

    if (fixture->tCounter == fixture->numTimers)
    {
        ASSERT_EQ (MAMA_STATUS_OK,
                   mama_stop (fixture->mBridge));
    }
}

static void MAMACALLTYPE onRecursiveTimerDestroy (mamaTimer timer, void* closure)
{
    MamaTimerTestC* fixture = (MamaTimerTestC *)closure;
    fixture->tCounter++;

    if (fixture->tCounter == fixture->numTimers)
    {
        ASSERT_EQ (MAMA_STATUS_OK,
                   mama_stop(fixture->mBridge));
    }
    else
    {
        mamaTimer_create2 (&timer, fixture->queue, onTimerTick, 
                           onRecursiveTimerDestroy, fixture->interval, fixture);
    }

}

static void MAMACALLTYPE onShortTimerTick (mamaTimer timer, void* closure)
{
}

static void MAMACALLTYPE onLongTimerTick (mamaTimer timer, void* closure)
{
}

static void MAMACALLTYPE onTwoTimerDestroy (mamaTimer timer, void* closure)
{
}

static void MAMACALLTYPE onStopTimerTick (mamaTimer timer, void* closure)
{
    mamaTimer_destroy(timer);
}

static void MAMACALLTYPE onStopTimerDestroy (mamaTimer timer, void* closure)
{
    MamaTimerTestC* fixture = (MamaTimerTestC *)closure;

    mamaTimer_destroy (fixture->shortTimer);
    mamaTimer_destroy (fixture->longTimer);
    
    mama_stop (fixture->mBridge);
}
/* ************************************************************************* */
/* Test Functions */
/* ************************************************************************* */

/*  Description: Create a mamaTimer which destroys itself in it's callback.
 *                   
 *  Expected Result: MAMA_STATUS_OK
 */
TEST_F (MamaTimerTestC, CreateDestroy)
{

    MamaTimerTestC* fixture = (MamaTimerTestC *)m_this;
    fixture->tCounter  = 0;
    fixture->numTimers = 1;
    
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTimer_create2 (&timer, fixture->queue, onTimerTick, 
                                  onTimerDestroy, fixture->interval, m_this));
    
    ASSERT_EQ (MAMA_STATUS_OK, mama_start (mBridge));
    
    ASSERT_EQ (MAMA_STATUS_OK, mama_close());

}

/*  Description: Create many mamaTimers which destroy themselves when fired.    
 *               mama_close() is called once the last timer has fired.
 *
 *  Expected Result: MAMA_STATUS_OK
 */
TEST_F (MamaTimerTestC, CreateDestroyMany)
{
    MamaTimerTestC* fixture = (MamaTimerTestC *)m_this;
    fixture->tCounter  = 0;
    fixture->numTimers = 100;
    
    for (int x=0; x!=fixture->numTimers; x++)
    {
        ASSERT_EQ (MAMA_STATUS_OK,
                   mamaTimer_create2 (&tarray[x], fixture->queue, onTimerTick, 
                                      onTimerDestroy,fixture->interval, m_this));
    }

    ASSERT_EQ (MAMA_STATUS_OK, mama_start (mBridge));
    
    ASSERT_EQ (MAMA_STATUS_OK, mama_close());

}

/*  Description: Create a timer whiich creates another timer when fired.
 *               This repeats for 10 additional timers.
 *                   
 *  Expected Result: MAMA_STATUS_OK
 */
TEST_F (MamaTimerTestC, RecursiveCreateDestroy)
{
    MamaTimerTestC* fixture = (MamaTimerTestC *)m_this;
    fixture->tCounter  = 0;
    fixture->numTimers = 11;
    
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTimer_create2(&timer, fixture->queue, onTimerTick, 
                                 onRecursiveTimerDestroy, fixture->interval, m_this));
     
    ASSERT_EQ (MAMA_STATUS_OK, mama_start(mBridge));
    
    ASSERT_EQ (MAMA_STATUS_OK, mama_close());

}

/*  Description: Two timers are created which tick indefinately at different rates,
 *               A third timer is used to destroy both of these after a set duration.
 *                   
 *  Expected Result: MAMA_STATUS_OK
 */
TEST_F (MamaTimerTestC, TwoTimer)
{
    MamaTimerTestC* fixture = (MamaTimerTestC *)m_this;

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTimer_create(&shortTimer, fixture->queue, onShortTimerTick, 
                                fixture->interval, m_this));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTimer_create (&longTimer, fixture->queue, onLongTimerTick, 
                                 ((fixture->interval)*2), m_this));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTimer_create2 (&stopTimer, fixture->queue, onStopTimerTick, 
                                  onStopTimerDestroy, ((fixture->interval)*100), m_this));

    ASSERT_EQ (MAMA_STATUS_OK, mama_start (mBridge));
    
    ASSERT_EQ (MAMA_STATUS_OK, mama_close());
}

