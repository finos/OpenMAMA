/* OpenMAMA: The open middleware agnostic messaging API
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
#include "mama/queue.h"

class MamaQueueTestC : public ::testing::Test
{
    protected:
        MamaQueueTestC();
        virtual ~MamaQueueTestC();
        virtual void SetUp();
        virtual void TearDown();
    public:
        mamaBridge      mBridge;
        MamaQueueTestC* m_this;
        int             m_numQueues;
        int             m_queueCounter;
        int             m_numEvents;
        int             m_eventCounter;
        int             m_numDispatches;
        int             m_highWaterMarkOccurance;
        int             m_lowWaterMarkOccurance;
        uint64_t        m_timeout;
        mamaDispatcher  dispatcher[10];
        mamaQueue       qArray[10];
};

MamaQueueTestC::MamaQueueTestC()
{
    mBridge;
    m_this = this;

    m_highWaterMarkOccurance = 0;
    m_lowWaterMarkOccurance  = 0;
    m_timeout                = 5000;
}

MamaQueueTestC::~MamaQueueTestC()
{
    m_this = NULL;
}

void MamaQueueTestC::SetUp()
{
    mama_loadBridge (&mBridge, getMiddleware());
    mama_open();
}

void MamaQueueTestC::TearDown()
{
    mama_close();
}

static void MAMACALLTYPE startCallback (mama_status status)
{
}

void highWaterMarkCallback (mamaQueue queue, size_t size, void* closure)
{
    MamaQueueTestC* fixture = (MamaQueueTestC*)closure;
    fixture->m_highWaterMarkOccurance++;
}

void lowWaterMarkCallback (mamaQueue queue, size_t size, void* closure)
{
    MamaQueueTestC* fixture = (MamaQueueTestC*)closure;
    fixture->m_lowWaterMarkOccurance++;
}

void MAMACALLTYPE onEvent (mamaQueue queue, void* closure)
{
    MamaQueueTestC* fixture = (MamaQueueTestC *)closure;
    fixture->m_eventCounter++;
    if (fixture->m_eventCounter == fixture->m_numEvents)
    {
        mamaQueue_stopDispatch (queue);
    }
}
void MAMACALLTYPE onTimedEvent (mamaQueue queue, void* closure)
{
}

void MAMACALLTYPE onBgEvent (mamaQueue queue, void* closure)
{
    MamaQueueTestC* fixture = (MamaQueueTestC *)closure;
    fixture->m_numDispatches++;
    
    if (fixture->m_numDispatches == 1000)
    {
        for (int x=0; x!=10; x++)
        {
            mamaQueue_stopDispatch (fixture->qArray[x]);
        }
    }
}

void MAMACALLTYPE onEnqueue (mamaQueue queue, void* closure)
{
}

void MAMACALLTYPE onHighWatermark (mamaQueue queue, size_t size, void* closure)
{
}

void MAMACALLTYPE onLowWatermark (mamaQueue queue, size_t size, void* closure)
{
}

/* ************************************************************************* */
/* Test Functions */
/* ************************************************************************* */

/*  Description:    Get the default event queue 
 *
 *  Expected Result: MAMA_STATUS_OK
 */
TEST_F (MamaQueueTestC, GetDefaultQueue)
{
    mamaQueue defaultQueue;

    ASSERT_EQ (MAMA_STATUS_OK, 
               mama_getDefaultEventQueue (mBridge, &defaultQueue));
}

/*  Description:   Create a mamaQueue then destroy it.
 *
 *  Expected Result: MAMA_STATUS_OK
 */
TEST_F (MamaQueueTestC, CreateNonDefaultQueue)
{
    mamaQueue queue;

    ASSERT_EQ (MAMA_STATUS_OK, 
               mamaQueue_create (&queue, mBridge));

    ASSERT_EQ (MAMA_STATUS_OK, 
               mamaQueue_destroy (queue));
}

/*  Description:  Enqueue an event on the default event queue. 
 *
 *  Expected Result: MAMA_STATUS_OK
 */
TEST_F (MamaQueueTestC, Enqueue)
{
    mamaQueue defaultQueue = NULL;

    ASSERT_EQ (MAMA_STATUS_OK,
               mama_getDefaultEventQueue (mBridge, &defaultQueue));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaQueue_enqueueEvent (defaultQueue, onEvent, m_this));
    
}

/*  Description:   Set the enqueue callback associated with the default queue
 *                 then enqueue an event.
 *
 *  Expected Result: MAMA_STATUS_OK.
 */
TEST_F (MamaQueueTestC, setEnqueueCallback)
{
    mamaQueue defaultQueue = NULL;

    m_numEvents    = 1;
    m_eventCounter = 0;

    ASSERT_EQ (MAMA_STATUS_OK,
               mama_getDefaultEventQueue (mBridge, &defaultQueue));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaQueue_setEnqueueCallback (defaultQueue, onEnqueue, m_this));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaQueue_enqueueEvent (defaultQueue, onEvent, m_this));
    
}

/*  Description:   Set the enqueue callback assiciated with the 
 *                 default queue then remove it.
 *
 *  Expected Result: MAMA_STATUS_OK
 */
TEST_F (MamaQueueTestC, RemoveEnqueueCallback)
{
    mamaQueue defaultQueue = NULL;

    m_numEvents    = 1;
    m_eventCounter = 0;

    ASSERT_EQ (MAMA_STATUS_OK,
               mama_getDefaultEventQueue (mBridge, &defaultQueue));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaQueue_setEnqueueCallback (defaultQueue, onEnqueue, m_this));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaQueue_enqueueEvent (defaultQueue, onEvent, m_this));
    
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaQueue_removeEnqueueCallback (defaultQueue));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaQueue_enqueueEvent (defaultQueue, onEvent, m_this));
    
}

/*  Description:   Enqueue an event on the default queue then dispatch it. 
 *
 *  Expected Result: MAMA_STATUS_OK
 */
TEST_F (MamaQueueTestC, EnqueueDispatch)
{
    mamaQueue defaultQueue = NULL;

    m_numEvents    = 1;
    m_eventCounter = 0;

    ASSERT_EQ (MAMA_STATUS_OK,
               mama_getDefaultEventQueue (mBridge, &defaultQueue));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaQueue_enqueueEvent (defaultQueue, onEvent, m_this));
    
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaQueue_dispatchEvent (defaultQueue));

}

/*  Description:   Create a mamaQueue, enqueue and event on it then
 *                 dispatch it before destroying the queue.
 *
 *  Expected Result: MAMA_STATUS_OK
 */
TEST_F (MamaQueueTestC, EnqueueDispatchNonDefault)
{
    mamaQueue queue = NULL;
    
    m_numEvents    = 1;
    m_eventCounter = 0;

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaQueue_create (&queue, mBridge));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaQueue_enqueueEvent (queue, onEvent, m_this));
    
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaQueue_dispatch (queue));
    
    ASSERT_EQ (MAMA_STATUS_OK, 
               mamaQueue_destroy (queue));
}

/*  Description:   create a mamaQueue, enqueuing multiple events 
 *                 which are dispatched. mamaQueue_stopDispatch() is 
 *                 called in the callback of the last enqueued event 
 *                 when dispatched. The mamaQueue is then destroyed.
 *
 *  Expected Result:
 */
TEST_F (MamaQueueTestC, EnqueueDispatchMany)
{
    mamaQueue queue = NULL;
    
    m_numEvents    = 10;
    m_eventCounter = 0;

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaQueue_create (&queue, mBridge));

    for (int x=0; x<=m_numEvents; x++)
    {
        ASSERT_EQ (MAMA_STATUS_OK,
                   mamaQueue_enqueueEvent (queue, onEvent, m_this));
    }
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaQueue_dispatch (queue));

    ASSERT_EQ (MAMA_STATUS_OK, 
               mamaQueue_destroy (queue));
}

/*  Description: mamaQueue is created then many events are enqueued.
 *               mamaQueue_timedDispatch dispatches one event and 
 *               blocks for specified time.
 *
 *  Expected Result: MAMA_STATUS_OK
 */
TEST_F (MamaQueueTestC, TimedDispatch)
{
    mamaQueue queue = NULL;
    
    m_numEvents    = 1000;
    m_eventCounter = 0;

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaQueue_create (&queue, mBridge));

    for (int x=0; x<=m_numEvents; x++)
    {
        ASSERT_EQ (MAMA_STATUS_OK,
                   mamaQueue_enqueueEvent (queue, onTimedEvent, m_this));
    }

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaQueue_timedDispatch (queue, m_timeout));

    ASSERT_EQ (MAMA_STATUS_OK, 
               mamaQueue_destroy (queue));
}

/*  Description:   high and low watermarks are established and corresponding
 *                 callback functions associated. many events enqueued  before
 *                 dispatching has begun to trigger high watermark callback then
 *                 dispatching begun to trigger low watermark callback. 
 *
 *  Expected Result: MAMA_STATUS_OK
 */
TEST_F (MamaQueueTestC, MonitorWatermarks)
{
    mamaQueue queue   = NULL;
    m_numEvents       = 20;
    m_eventCounter    = 0;

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaQueue_create (&queue, mBridge));
    
    mamaQueueMonitorCallbacks queueCallbacks;
    queueCallbacks.onQueueHighWatermarkExceeded = onHighWatermark;
    queueCallbacks.onQueueLowWatermark          = onLowWatermark;
    
    ASSERT_EQ (MAMA_STATUS_OK, 
               mamaQueue_setQueueMonitorCallbacks (queue, &queueCallbacks, m_this));

    ASSERT_EQ (MAMA_STATUS_OK, 
               mamaQueue_setHighWatermark (queue, 10));

    ASSERT_EQ (MAMA_STATUS_OK, 
               mamaQueue_setLowWatermark (queue, 5));

    for (int x=0; x<=m_numEvents; x++)
    {
        ASSERT_EQ (MAMA_STATUS_OK,
                   mamaQueue_enqueueEvent (queue, onEvent, m_this));
    }

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaQueue_dispatch(queue));

    ASSERT_EQ (MAMA_STATUS_OK, 
               mamaQueue_destroy (queue));
}

/*  Description:   multiple queues created and an equal amount of events enqueued on each. 
 *                 mamaDispatchers are created to begin dispatching each queue's events on 
 *                 a separate thread each. Dispatchers and queues are destroyed once events
 *                 have been dispatched.
 *
 *  Expected Result: MAMA_STATUS_OK
 */
TEST_F (MamaQueueTestC, DispatchManyQueuesWithDispatchers)
{
    m_numQueues     = 10;
    m_numEvents     = 100;
    m_eventCounter  = 0;
    m_numDispatches = 0;

    for (m_queueCounter = 0; m_queueCounter!=m_numQueues; m_queueCounter++)
    {
        ASSERT_EQ (MAMA_STATUS_OK,
                   mamaQueue_create (&qArray[m_queueCounter], mBridge));

        for (int x=0; x!=m_numEvents; x++)
        {
            ASSERT_EQ (MAMA_STATUS_OK,
                       mamaQueue_enqueueEvent (qArray[m_queueCounter], onBgEvent, m_this));
        }
    }

    for (m_queueCounter = 0; m_queueCounter!=m_numQueues; m_queueCounter++)
    {
        ASSERT_EQ (MAMA_STATUS_OK,
                   mamaDispatcher_create (&dispatcher[m_queueCounter], qArray[m_queueCounter]));
    }

    for (int x=0; x!=10; x++)
    {
        mamaDispatcher_destroy (dispatcher[x]);
    }
    for (m_queueCounter = 0; m_queueCounter!=m_numQueues; m_queueCounter++)
    {
        mamaQueue_destroy (qArray[m_queueCounter]);
    }

}

