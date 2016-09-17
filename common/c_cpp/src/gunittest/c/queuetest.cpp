/* $Id: queuetest.cpp,v 1.1.2.1 2012/11/21 10:03:55 matthewmulhern Exp $
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
#include <sys/types.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>

#include "wombat/port.h"
#include "wombat/queue.h"
#include "wombat/wSemaphore.h"



class CommonQueueTestC : public ::testing::Test
{	
protected:
    CommonQueueTestC();          
    virtual ~CommonQueueTestC(); 

    virtual void SetUp();        
    virtual void TearDown ();    
public:
};

CommonQueueTestC::CommonQueueTestC()
{
}

CommonQueueTestC::~CommonQueueTestC()
{
}

void CommonQueueTestC::SetUp(void)
{	
}

void CommonQueueTestC::TearDown(void)
{
}

#if defined(__cplusplus)
extern "C" {
#endif

static void  MAMACALLTYPE onEvent(wombatQueue queue, void* closure)
{
}

#if defined(__cplusplus)
}
#endif

/* ************************************************************************* */
/* Test Functions */
/* ************************************************************************* */

/*  Description: Allocates space for a queue 
 *  
 *  Expected Result: WOMBAT_QUEUE_OK
 */
TEST_F (CommonQueueTestC, allocate)
{
    wombatQueue queue = NULL;

    ASSERT_EQ (WOMBAT_QUEUE_OK, wombatQueue_allocate(&queue));

    free (queue);
}

/*  Description: Allocates space for a queue, creates it then destroys it.
 *  
 *  Expected Result: WOMBAT_QUEUE_OK
 */
TEST_F (CommonQueueTestC, createDestroy)
{
    wombatQueue queue       = NULL;
    uint32_t    maxSize     = 1000;
    uint32_t    initialSize = 10;
    uint32_t    growBySize  = 100;

    ASSERT_EQ (WOMBAT_QUEUE_OK, wombatQueue_allocate(&queue));
    
    ASSERT_EQ (WOMBAT_QUEUE_OK, wombatQueue_create(queue,
                                                   maxSize,
                                                   initialSize,
                                                   growBySize));

    ASSERT_EQ (WOMBAT_QUEUE_OK, wombatQueue_destroy(queue));
}

/*  Description: Allocates space for 100 queues, creates them then 
 *               destroys them. 
 *  
 *  Expected Result: WOMBAT_QUEUE_OK
 */
TEST_F (CommonQueueTestC, createDestroyMany)
{
    wombatQueue     queue[100]  = {NULL};
    uint32_t        maxSize     = 1000;
    uint32_t        initialSize = 10;
    uint32_t        growBySize  = 100;
    
    for(int i=0;i<100;i++)
    {
        ASSERT_EQ (WOMBAT_QUEUE_OK, wombatQueue_allocate(&queue[i]));
    }
    
    for(int i=0;i<100;i++)
    {
        ASSERT_EQ (WOMBAT_QUEUE_OK, wombatQueue_create(queue[i],
                                                       maxSize,
                                                       initialSize,
                                                       growBySize));
    }
    
    for(int i=0;i<100;i++) 
    {
        ASSERT_EQ (WOMBAT_QUEUE_OK, wombatQueue_destroy(queue[i]));
    }
}

/*  Description: Creates a Queue, sets a new max size, gets that max size
 *               and compares the two.
 *  
 *  Expected Result: newsize=checksize=3000
 */
TEST_F (CommonQueueTestC, SetGetMaxSize)
{
    wombatQueue     queue           = NULL;
    uint32_t        maxSize         = 1000;
    uint32_t        initialSize     = 10;
    uint32_t        growBySize      = 100;
    unsigned int    newMaxSize      = 3000;
    unsigned int    checkMaxSize    = 0;

    ASSERT_EQ (WOMBAT_QUEUE_OK, wombatQueue_allocate(&queue));

    ASSERT_EQ (WOMBAT_QUEUE_OK, wombatQueue_create(queue,
                                                   maxSize,
                                                   initialSize,
                                                   growBySize));

    ASSERT_EQ (WOMBAT_QUEUE_OK, wombatQueue_setMaxSize(queue, 
                                                       newMaxSize));

    ASSERT_EQ (WOMBAT_QUEUE_OK, wombatQueue_getMaxSize(queue, 
                                                       &checkMaxSize));

    ASSERT_EQ (newMaxSize, checkMaxSize);

    ASSERT_EQ (WOMBAT_QUEUE_OK, wombatQueue_destroy(queue));
}

/*  Description: Creates a queue and enqueues 200 events. getSize 
 *               is then called which gives the number of undispatched
 *               events on queue.
 *               
 *  Expected Result: size=200
 */
TEST_F (CommonQueueTestC, getSize)
{
    wombatQueue queue       = NULL;
    uint32_t    maxSize     = 1000;
    uint32_t    initialSize = 10;
    uint32_t    growBySize  = 100;
    int         size        = 0;
    void*       data        = NULL;
    void*       closure     = NULL;

    ASSERT_EQ (WOMBAT_QUEUE_OK, wombatQueue_allocate(&queue));

    ASSERT_EQ (WOMBAT_QUEUE_OK, wombatQueue_create(queue,
                                                   maxSize,
                                                   initialSize,
                                                   growBySize));
    for(int i=0;i<200;i++)
    {
        ASSERT_EQ (WOMBAT_QUEUE_OK, wombatQueue_enqueue(queue,
                                                        onEvent,
                                                        &data,
                                                        &closure));
    }

    ASSERT_EQ (WOMBAT_QUEUE_OK, wombatQueue_getSize(queue,&size));

    ASSERT_EQ (200, size);

    ASSERT_EQ (WOMBAT_QUEUE_OK, wombatQueue_destroy(queue));
}



/*  Description: Creates a queue, enqueues event, dispatches it then 
 *               queue is destroyed.
 *  
 *  Expected Result: WOMBAT_QUEUE_OK
 */
TEST_F (CommonQueueTestC, EnqueueDispatch)
{
    wombatQueue queue       = NULL;
    uint32_t    maxSize     = 1000;
    uint32_t    initialSize = 10;
    uint32_t    growBySize  = 100;
    void*       data        = NULL;
    void*       closure     = NULL;

    ASSERT_EQ (WOMBAT_QUEUE_OK, wombatQueue_allocate(&queue));

    ASSERT_EQ (WOMBAT_QUEUE_OK, wombatQueue_create(queue,
                                                   maxSize,
                                                   initialSize,
                                                   growBySize));

    ASSERT_EQ (WOMBAT_QUEUE_OK, wombatQueue_enqueue (queue,
                                                     onEvent,
                                                     data,
                                                     closure));

    ASSERT_EQ (WOMBAT_QUEUE_OK, wombatQueue_dispatch (queue,
                                                      &data,
                                                      &closure));

    ASSERT_EQ (WOMBAT_QUEUE_OK, wombatQueue_destroy (queue));
}

/*  Description: Creates a queue, enqueues 200 events, dispatches them then 
 *               queue is destroyed.
 *  
 *  Expected Result: WOMBAT_QUEUE_OK
 */
TEST_F (CommonQueueTestC, EnqueueDispatchMany)
{
    wombatQueue queue       = NULL;
    uint32_t    maxSize     = 1000;
    uint32_t    initialSize = 10;
    uint32_t    growBySize  = 100;
    void*       data        = NULL;
    void*       closure     = NULL;

    ASSERT_EQ (WOMBAT_QUEUE_OK, wombatQueue_allocate(&queue));

    ASSERT_EQ (WOMBAT_QUEUE_OK, wombatQueue_create(queue,
                                                   maxSize,
                                                   initialSize,
                                                   growBySize));

    for(int i=0;i<200;i++)
    {
        ASSERT_EQ (WOMBAT_QUEUE_OK, wombatQueue_enqueue(queue,
                                                        onEvent,
                                                        data,
                                                        closure));
    }

    for(int i=0;i<200;i++)
    {
        ASSERT_EQ (WOMBAT_QUEUE_OK, wombatQueue_dispatch(queue,
                                                         &data,
                                                         &closure));
    }

    ASSERT_EQ (WOMBAT_QUEUE_OK, wombatQueue_destroy(queue));
}

/*  Description: Creates a queue and enqueues 200 events. timedDispatched
 *               waits 500 microseconds then dispatches each event.
 *  
 *  Expected Result: WOMBAT_QUEUE_OK
 */
TEST_F (CommonQueueTestC, TimedDispatch)
{
    wombatQueue queue       = NULL;
    uint32_t    maxSize     = 1000;
    uint32_t    initialSize = 10;
    uint32_t    growBySize  = 100;
    uint64_t    timeout     = 500;
    void*       data        = NULL;
    void*       closure     = NULL;

    ASSERT_EQ (WOMBAT_QUEUE_OK, wombatQueue_allocate(&queue));

    ASSERT_EQ (WOMBAT_QUEUE_OK, wombatQueue_create(queue,
                                                   maxSize,
                                                   initialSize,
                                                   growBySize));

    for(int i=0;i<200;i++)
    {
        ASSERT_EQ (WOMBAT_QUEUE_OK, wombatQueue_enqueue(queue,
                                                        onEvent,
                                                        &data,
                                                        &closure));
    }

    for(int i=0;i<200;i++)
    {
        ASSERT_EQ (WOMBAT_QUEUE_OK, wombatQueue_timedDispatch(queue,
                                                              &data,
                                                              &closure,
                                                              timeout));
    }

    ASSERT_EQ (WOMBAT_QUEUE_OK, wombatQueue_destroy(queue));
}

/*  Description: Enqueues 3 events, then polls 4 times to dispatch the 3
 *               events and return status.
 *
 *  Expected Result: WOMBAT_QUEUE_WOULD_BLOCK
 */
TEST_F (CommonQueueTestC, Poll)
{
    wombatQueue queue       = NULL;
    uint32_t    maxSize     = 1000;
    uint32_t    initialSize = 10;
    uint32_t    growBySize  = 100;
    void*       data        = NULL;
    void*       closure     = NULL;

    ASSERT_EQ (WOMBAT_QUEUE_OK, wombatQueue_allocate(&queue));

    ASSERT_EQ (WOMBAT_QUEUE_OK, wombatQueue_create(queue,
                                                   maxSize,
                                                   initialSize,
                                                   growBySize));

    for(int i=0;i<3;i++)
    {
        ASSERT_EQ (WOMBAT_QUEUE_OK, wombatQueue_enqueue (queue,
                                                         onEvent,
                                                         data,
                                                         closure));
    }

    for(int i=0;i<3;i++)
    {
        ASSERT_EQ (WOMBAT_QUEUE_OK, wombatQueue_poll(queue,
                                                     &data,
                                                     &closure));   
    }

    ASSERT_EQ (WOMBAT_QUEUE_WOULD_BLOCK, wombatQueue_poll(queue,
                                                          &data,
                                                          &closure));   

    ASSERT_EQ (WOMBAT_QUEUE_OK, wombatQueue_destroy (queue));
}

/*  Description: Allocates space for 100 queues, enqueues and dispatches
 *               200 events on each queue then queues are destroyed. 
 *  
 *  Expected Result: WOMBAT_QUEUE_OK
 */
TEST_F (CommonQueueTestC, EnqueueDispatchManyQueues)
{
    wombatQueue queue[100]  = {NULL};
    uint32_t    maxSize     = 1000;
    uint32_t    initialSize = 10;
    uint32_t    growBySize  = 100;
    void*       data        = NULL;
    void*       closure     = NULL;

    for(int i=0;i<100;i++)
    {
        ASSERT_EQ (WOMBAT_QUEUE_OK, wombatQueue_allocate(&queue[i]));
    }

    for(int i=0;i<100;i++)
    {
        ASSERT_EQ (WOMBAT_QUEUE_OK, wombatQueue_create(queue[i],
                                                       maxSize,
                                                       initialSize,
                                                       growBySize));
    } 

    for(int i=0;i<100;i++)
    {
        for(int x=0;x<200;x++)
        {
            ASSERT_EQ (WOMBAT_QUEUE_OK, wombatQueue_enqueue(queue[i],
                                                            onEvent,
                                                            data,
                                                            closure));
        }
    }

    for(int i=0;i<100;i++)
    {
        for(int x=0;x<200;x++)
        {
            ASSERT_EQ (WOMBAT_QUEUE_OK, wombatQueue_dispatch(queue[i],
                                                             &data,
                                                             &closure));
        }
    }

    for(int i=0;i<100;i++) 
    {
        ASSERT_EQ (WOMBAT_QUEUE_OK, wombatQueue_destroy (queue[i]));
    }
}

