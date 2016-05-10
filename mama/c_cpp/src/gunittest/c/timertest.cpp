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
#include "mama/transport.h"
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
    mamaBridge    m_bridge;
    mamaTransport m_transport;
    int           m_tCounter;
    int           m_numTimers;
    mamaTimer     m_timers[100];
    mamaTimer     m_longTimer;
    mamaTimer     m_shortTimer;
    mamaTimer     m_stopTimer;
    mamaTimer     m_timer;
    mamaQueue     m_queue;
    mama_f64_t    m_interval;
};

MamaTimerTestC::MamaTimerTestC()
{
}

MamaTimerTestC::~MamaTimerTestC()
{
}

void MamaTimerTestC::SetUp(void)
{	
    m_tCounter = 0;
    m_interval = 0.01;

    ASSERT_EQ (MAMA_STATUS_OK, mama_loadBridge (&m_bridge, getMiddleware()));
    ASSERT_EQ (MAMA_STATUS_OK, mama_getDefaultEventQueue (m_bridge, &m_queue));
    ASSERT_EQ (MAMA_STATUS_OK, mama_open());
    ASSERT_EQ (MAMA_STATUS_OK, mamaTransport_allocate (&m_transport));
    ASSERT_EQ (MAMA_STATUS_OK, mamaTransport_create (m_transport, NULL, m_bridge));
}

void MamaTimerTestC::TearDown(void)
{
    ASSERT_EQ (MAMA_STATUS_OK, mamaTransport_destroy (m_transport));
    ASSERT_EQ (MAMA_STATUS_OK, mama_close());
}

static void MAMACALLTYPE onTimerTick (mamaTimer timer, void* closure)
{
    ASSERT_EQ (MAMA_STATUS_OK, mamaTimer_destroy(timer));
}

static void MAMACALLTYPE onTimerDestroy (mamaTimer timer, void* closure)
{
    MamaTimerTestC* fixture = (MamaTimerTestC *)closure;
    if (++fixture->m_tCounter == fixture->m_numTimers)
    {
        ASSERT_EQ (MAMA_STATUS_OK,
                   mama_stop (fixture->m_bridge));
    }
}

static void MAMACALLTYPE onRecursiveTimerDestroy (mamaTimer timer, void* closure)
{
    MamaTimerTestC* fixture = (MamaTimerTestC *)closure;
    if (++fixture->m_tCounter == fixture->m_numTimers)
    {
        ASSERT_EQ (MAMA_STATUS_OK,
                   mama_stop(fixture->m_bridge));
    }
    else
    {
        ASSERT_EQ (MAMA_STATUS_OK,
                   mamaTimer_create2 (&fixture->m_timer, fixture->m_queue, onTimerTick,
                                      onRecursiveTimerDestroy, fixture->m_interval, fixture));
    }

}

static void MAMACALLTYPE onShortTimerTick (mamaTimer timer, void* closure)
{
}

static void MAMACALLTYPE onLongTimerTick (mamaTimer timer, void* closure)
{
}

static void MAMACALLTYPE onStopTimerTick (mamaTimer timer, void* closure)
{
    ASSERT_EQ (MAMA_STATUS_OK, mamaTimer_destroy(timer));
}

static void MAMACALLTYPE onStopTimerDestroy (mamaTimer timer, void* closure)
{
    MamaTimerTestC* fixture = (MamaTimerTestC *)closure;

    ASSERT_EQ (MAMA_STATUS_OK, mamaTimer_destroy (fixture->m_shortTimer));
    ASSERT_EQ (MAMA_STATUS_OK, mamaTimer_destroy (fixture->m_longTimer));

    ASSERT_EQ (MAMA_STATUS_OK, mama_stop (fixture->m_bridge));
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
    m_numTimers = 1;
    
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTimer_create2 (&m_timer, m_queue, onTimerTick,
                                  onTimerDestroy, m_interval, this));
    
    ASSERT_EQ (MAMA_STATUS_OK, mama_start (m_bridge));
}

/*  Description: Create many mamaTimers which destroy themselves when fired.    
 *               mama_close() is called once the last timer has fired.
 *
 *  Expected Result: MAMA_STATUS_OK
 */
TEST_F (MamaTimerTestC, CreateDestroyMany)
{
    m_numTimers = 100;  // FIXME: Storage hardcoded to 100!
    
    for (int x=0; x!=m_numTimers; x++)
    {
        ASSERT_EQ (MAMA_STATUS_OK,
                   mamaTimer_create2 (&m_timers[x], m_queue, onTimerTick,
                                      onTimerDestroy, m_interval, this));
    }

    ASSERT_EQ (MAMA_STATUS_OK, mama_start (m_bridge));
}

/*  Description: Create a timer whiich creates another timer when fired.
 *               This repeats for 10 additional timers.
 *                   
 *  Expected Result: MAMA_STATUS_OK
 */
TEST_F (MamaTimerTestC, RecursiveCreateDestroy)
{
    m_numTimers = 11;
    
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTimer_create2(&m_timer, m_queue, onTimerTick,
                                 onRecursiveTimerDestroy, m_interval, this));
     
    ASSERT_EQ (MAMA_STATUS_OK, mama_start(m_bridge));
}

/*  Description: Two timers are created which tick indefinately at different rates,
 *               A third timer is used to destroy both of these after a set duration.
 *                   
 *  Expected Result: MAMA_STATUS_OK
 */
TEST_F (MamaTimerTestC, TwoTimer)
{
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTimer_create(&m_shortTimer, m_queue, onShortTimerTick,
                                m_interval, this));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTimer_create (&m_longTimer, m_queue, onLongTimerTick,
                                 m_interval*2, this));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTimer_create2 (&m_stopTimer, m_queue, onStopTimerTick,
                                  onStopTimerDestroy, m_interval*100, this));

    ASSERT_EQ (MAMA_STATUS_OK, mama_start (m_bridge));
}

