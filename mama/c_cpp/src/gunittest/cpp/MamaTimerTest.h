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
#ifndef MAMATIMERTEST_H
#define MAMATIMERTEST_H

/* ************************************************************************* */
/* Includes */
/* ************************************************************************* */
#include "gtest/gtest.h"
#include "mama/mamacpp.h"
#include "MainUnitTestCpp.h"
#include "mama/MamaTimerCallback.h"

#include "mama/timer.h"
#include "mama/types.h"

/* ************************************************************************* */
/* Test Fixture */
/* ************************************************************************* */

using namespace Wombat;

class MamaTimerTestCPP : public ::testing::Test
{
	// Member variables
protected:
    
    // This is a reference to the first timer in the 2 timer test
    MamaTimer m_firstTimer;

    // Holds all objects required to initialise mama
	//PMamaInitialisation m_init;
    
    // The number of callbacks received in the ForTimer test
    int m_numberForCallbacks;

    // The number of timers that will be created in the ForTimer test
    int m_numberForTimers;

    // The number of times the recursive callback function was invoked
    int m_numberRecursiveCallbacks;

	// Holds all parameters
	//PMamaParameters m_parameters;
    MamaQueue* m_defaultQueue;

    // This native queue is used by the second timer in the TwoTimerThreaded test
    MamaQueue m_secondTimerQueue;
    
    /* Work around for problem in gtest where the this pointer can't be accessed
	 * from a test fixture.
	 */
	MamaTimerTestCPP *m_this;

    // The interval used for all the timer tests
    double m_timerInterval;
    
    MamaTimer mtarray[1000];
    
    mamaBridge m_bridge;
    MamaTransport m_transport;
    char transportName[10];
    
	// Member functions
protected:
    
    // Protected functions
    //void CreateTimer(mamaTimerCb callback, mamaTimer *mTimer = NULL);
    void CreateTimer(MamaTimerCallback* callback, MamaQueue *queue, MamaTimer *mTimer = NULL);
    
    // Callback functions
    //static void MAMACALLTYPE OnForTimerTick(mamaTimer timer, void* closure);
    //static void MAMACALLTYPE OnRecursiveTimerTick(mamaTimer timer, void* closure);
    static void MAMACALLTYPE OnTwoTimerThreaded1Tick(mamaTimer timer, void* closure);
    static void MAMACALLTYPE OnTwoTimerThreaded2Tick(mamaTimer timer, void* closure);
    static void MAMACALLTYPE OnTwoTimerTick(mamaTimer timer, void* closure);    

protected:

	// Construction and Destruction
	MamaTimerTestCPP (void);
	virtual ~MamaTimerTestCPP(void);

	// Setup and teardown functions
	virtual void SetUp(void);
	virtual void TearDown(void);	


    class OnForTimerTick : public MamaTimerCallback
    {
        void onTimer(MamaTimer* timer);
        void onDestroy(MamaTimer* timer, void * closure);
    };

    class OnForRecursiveTick : public MamaTimerCallback
    {
        void onTimer(MamaTimer* timer);
        void onDestroy(MamaTimer* timer, void * closure);
        
        //For keeping callback for recursive new timers        
        MamaTimerTestCPP::OnForRecursiveTick *m_callback;
    };

    class OnForTwoTimerTick : public MamaTimerCallback
    {
        void onTimer(MamaTimer* timer);
        void onDestroy(MamaTimer* timer, void * closure);

        MamaTimerTestCPP::OnForTwoTimerTick *m_callback;
    };

    class StopDispatchingTick : public MamaTimerCallback
    {
        void onTimer(MamaTimer* timer);
        void onDestroy(MamaTimer* timer, void * closure);
    };

    class OnTwoTimerThreaded1Tick : public MamaTimerCallback
    {
        void onTimerTick(MamaTimer* timer, void * closure);
        void onDestroy(MamaTimer* timer, void * closure);
    };

};

#endif
