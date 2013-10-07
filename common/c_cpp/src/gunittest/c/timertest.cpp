/* $Id: timertest.cpp,v 1.1.2.1 2012/11/21 10:03:55 matthewmulhern Exp $
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
#include <cstring>
#include <cstdio>
#include <cstdlib>

#include "wombat/port.h"
#include "timers.h"
#include <sys/types.h>




class CommonTimerTestC : public ::testing::Test
{	
protected:
    CommonTimerTestC();          
    virtual ~CommonTimerTestC(); 

    virtual void SetUp();        
    virtual void TearDown ();    
public:
    CommonTimerTestC *m_this;
};

CommonTimerTestC::CommonTimerTestC()
{
    m_this = NULL;
}

CommonTimerTestC::~CommonTimerTestC()
{
}

void CommonTimerTestC::SetUp(void)
{	
}

void CommonTimerTestC::TearDown(void)
{
}

#if defined(__cplusplus)
extern "C" {
#endif

static void  onTimerFire(timerElement timer, void* closure)
{
    ASSERT_EQ (0, destroyTimer(closure,timer)) <<"Could not destroy timer!";
}

#if defined(__cplusplus)
}
#endif

/* ************************************************************************* */
/* Test Functions */
/* ************************************************************************* */

/*  Description: Creates a timerHeap, starts dispatching then destroys the
 *               heap 
 *  
 *  Expected Result: Functions return zero
 */
TEST_F (CommonTimerTestC, createDestroyHeap)
{
    timerHeap heap = NULL;

    ASSERT_EQ (0, createTimerHeap(&heap));
    
    ASSERT_EQ (0, startDispatchTimerHeap(heap));
    
    ASSERT_EQ (0, destroyHeap(heap));
}

/*  Description: Creates 10 timerHeaps, starts dispatching  on them all 
 *               then destroys them all.
 *  
 *  Expected Result: Functions return zero
 */
TEST_F (CommonTimerTestC, createDestroyManyHeaps)
{
    timerHeap heap = NULL;

    for(int i=0; i<10 ;i++)
    {
        ASSERT_EQ (0, createTimerHeap(&heap));

        ASSERT_EQ (0, startDispatchTimerHeap(heap));

        ASSERT_EQ (0, destroyHeap(heap));
    }
}

/*  Description: Creates a timerHeap, starts dispatching  then creates a  
 *               timer which destroys itself in it's callback. Heap is
 *               then destroyed.
 *  
 *  Expected Result: Functions return zero
 */
TEST_F (CommonTimerTestC, createDestroyTimer)
{
    struct timeval  timeout;
    timerElement    timer   = NULL;
    timerHeap       heap    = NULL;

    timeout.tv_sec  = 0;
    timeout.tv_usec = 500;
    
    ASSERT_EQ (0, createTimerHeap(&heap));
    
    ASSERT_EQ (0, startDispatchTimerHeap(heap));
    
    ASSERT_EQ (0, createTimer(&timer, heap, onTimerFire, &timeout, heap));
    sleep(2);

    ASSERT_EQ (0, destroyHeap(heap));

}

/*  Description: Creates a timerHeap, starts dispatching  then creates   
 *               100 timers which destroys themselves by callback. Heap is
 *               then destroyed.
 *  
 *  Expected Result: Functions return zero
 */
TEST_F (CommonTimerTestC, createDestroyManyTimers)
{
    struct timeval  timeout;
    timerElement    timer[100]  = {NULL};
    timerHeap       heap        = NULL;
   
    timeout.tv_sec = 0;
    timeout.tv_usec = 500;
    
    ASSERT_EQ (0, createTimerHeap(&heap));

    ASSERT_EQ (0, startDispatchTimerHeap(heap));
    
    for(int i=0; i<100 ;i++)
    {
        ASSERT_EQ (0, createTimer(&timer[i], 
                                  heap, 
                                  onTimerFire, 
                                  &timeout, 
                                  heap));
    }
    sleep(2);
    
    ASSERT_EQ (0, destroyHeap(heap));
}


/*  Description: Creates a 10 timerHeaps, starts dispatching  on them all and 
 *               creates 10 timers per heap, all destroying by callback.
 *  
 *  Expected Result: Functions return zero
 */
TEST_F (CommonTimerTestC, createDestroyManyHeapsWithTimers)
{
    struct timeval  timeout;
    timerElement    timer[10]   = {NULL};
    timerHeap       heap        = NULL;

    timeout.tv_sec  = 0;
    timeout.tv_usec = 500;
    
    for(int i=0; i<10 ;i++)
    {
        ASSERT_EQ (0, createTimerHeap(&heap));

        ASSERT_EQ (0, startDispatchTimerHeap(heap));

        for (int tcounter=0; tcounter<10; tcounter++)
        {
            ASSERT_EQ (0, createTimer(&timer[i], heap, onTimerFire, &timeout, heap));
        }
        sleep(1);
        
        ASSERT_EQ (0, destroyHeap(heap));
    }
 }

