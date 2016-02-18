/* $Id: timertest.cpp,v 1.1.2.1.2.4 2012/08/24 16:12:00 clintonmcdowell Exp $
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
#include "mama/types.h"
#include "mama/timer.h"
#include "mama/queue.h"
#include "mama/io.h"
#include <publisherimpl.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>


class MamaPublisherTestC : public ::testing::Test
{
protected:
    MamaPublisherTestC();          
    virtual ~MamaPublisherTestC(); 

    virtual void SetUp();        
    virtual void TearDown ();    
public:
    MamaPublisherTestC *m_this;
    mamaBridge          mBridge;
    
};

MamaPublisherTestC::MamaPublisherTestC()
{
}

MamaPublisherTestC::~MamaPublisherTestC()
{
}

void MamaPublisherTestC::SetUp(void)
{
    m_this = this;

    mama_loadBridge (&mBridge, getMiddleware());

    mama_open();
    
}

void MamaPublisherTestC::TearDown(void)
{
    m_this = NULL;
    mama_close();
}


/* ************************************************************************* */
/* Test Functions */
/* ************************************************************************* */

/**
 * Counters for publisher events callbacks
 */
int pubOnCreateCount = 0;
int pubOnErrorCount = 0;
int pubOnDestroyCount = 0;

/**
 * Publisher event callbacks
 */
void pubOnCreate (mamaPublisher publisher, void* closure)
{
    pubOnCreateCount++;
}

void pubOnDestroy (mamaPublisher publisher, void* closure)
{
    pubOnDestroyCount++;
}

void pubOnError (mamaPublisher publisher,
                 mama_status   status,
                 const char*   info,
                 void*         closure)
{
    pubOnErrorCount++;
}

/**
 * Publisher event callbacks via transport topic callbacks
 */
void transportTopicCb (mamaTransport tport,
                       mamaTransportTopicEvent event,
                       const char* topic,
                       const void* platformInfo,
                       void *closure)
{
    switch (event)
    {
        case MAMA_TRANSPORT_TOPIC_PUBLISH_ERROR:
        case MAMA_TRANSPORT_TOPIC_PUBLISH_ERROR_NOT_ENTITLED:
        case MAMA_TRANSPORT_TOPIC_PUBLISH_ERROR_BAD_SYMBOL:
            pubOnErrorCount++;
            break;
        default:
            break;
    }
}

/*  Description: Create a mamaPublisher then destroy it.  
 *
 *  Expected Result: MAMA_STATUS_OK
 */
TEST_F (MamaPublisherTestC, CreateDestroy)
{
    mamaPublisher    publisher = NULL;
    mamaTransport    tport     = NULL;
    const char*      source    = getSource();
   
    pubOnCreateCount = 0;
    pubOnErrorCount = 0;
    pubOnDestroyCount = 0;

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_allocate (&tport));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_create (tport, getTransport(), mBridge));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaPublisher_create (&publisher, tport, source, NULL, NULL));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaPublisher_destroy (publisher));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_destroy (tport));
}

/*  Description: Create a mamaPublisher and get its transport
 *
 *  Expected Result: MAMA_STATUS_OK
 */
TEST_F (MamaPublisherTestC, GetTransport)
{
    mamaPublisher    publisher = NULL;
    mamaTransport    tport     = NULL;
    const char*      symbol    = "SYM";
    const char*      source    = "SRC";
   
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_allocate (&tport));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_create (tport, getTransport(), mBridge));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaPublisher_create (&publisher, tport, symbol, source, NULL));

    ASSERT_EQ(tport, mamaPublisherImpl_getTransportImpl (publisher));

    ASSERT_EQ(tport, mamaPublisherImpl_getTransportImpl (publisher));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaPublisher_destroy (publisher));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_destroy (tport));

    ASSERT_EQ (0, pubOnCreateCount);
    ASSERT_EQ (0, pubOnErrorCount);
    ASSERT_EQ (0, pubOnDestroyCount);
}

/*  Description: Create a mamaPublisher and mamaMsg, send the msg using 
 *               mamaPublisher then destroy both.
 *
 *  Expected Result: MAMA_STATUS_OK
 */
TEST_F (MamaPublisherTestC, Send)
{
    mamaPublisher    publisher = NULL;
    mamaTransport    tport     = NULL;
    const char*      symbol    = getSymbol();
    const char*      source    = getSource();
    mamaMsg          msg       = NULL;

    pubOnCreateCount = 0;
    pubOnErrorCount = 0;
    pubOnDestroyCount = 0;

    ASSERT_EQ (MAMA_STATUS_OK, mama_open());

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaMsg_create (&msg));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaMsg_addString (msg, symbol, 101, source));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_allocate (&tport));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_create (tport, getTransport(), mBridge));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaPublisher_create (&publisher, tport, symbol, source, NULL));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaPublisher_send (publisher, msg));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaPublisher_destroy (publisher));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_destroy (tport));

    ASSERT_EQ (MAMA_STATUS_OK, mama_close());

    ASSERT_EQ (0, pubOnCreateCount);
    ASSERT_EQ (0, pubOnErrorCount);
    ASSERT_EQ (0, pubOnDestroyCount);
}

/*  Description: Create a mamaPublisher with event callbacks and mamaMsg, send the msg using 
 *               mamaPublisher then destroy both.
 *
 *  Expected Result: MAMA_STATUS_OK
 */
TEST_F (MamaPublisherTestC, EventSendWithCallbacks)
{
    mamaPublisher    publisher     = NULL;
    mamaTransport    tport         = NULL;
    const char*      symbol        = getSymbol();
    const char*      source        = getSource();
    mamaMsg          msg           = NULL;
    mamaQueue        queue         = NULL;
    mamaPublisherCallbacks* cb     = NULL;
    int              i             = 0;
    int              numPublishers = 10;

    pubOnCreateCount = 0;
    pubOnErrorCount = 0;
    pubOnDestroyCount = 0;

    mamaPublisherCallbacks_allocate(&cb);
    cb->onError = (mama_publisherOnErrorCb) pubOnError;
    cb->onCreate = (mama_publisherOnCreateCb) pubOnCreate;
    cb->onDestroy = (mama_publisherOnDestroyCb) pubOnDestroy;

    ASSERT_EQ (MAMA_STATUS_OK, mama_open());

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaMsg_create (&msg));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaMsg_addString (msg, symbol, 101, source));

    ASSERT_EQ (MAMA_STATUS_OK,
               mama_getDefaultEventQueue (mBridge, &queue));
 
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_allocate (&tport));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_create (tport, getTransport(), mBridge));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_setTransportTopicCallback (tport, (mamaTransportTopicCB) transportTopicCb, NULL));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaPublisher_createWithCallbacks (&publisher, tport, queue, symbol, source, NULL, cb, NULL));

    for (i = 0; i < numPublishers; ++i)
    {
        ASSERT_EQ (MAMA_STATUS_OK,
                   mamaPublisher_send (publisher, msg));
    }

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaPublisher_destroy (publisher));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_destroy (tport));

    ASSERT_EQ (MAMA_STATUS_OK, mama_close());

    ASSERT_EQ (1, pubOnCreateCount);
    ASSERT_EQ (0, pubOnErrorCount);
    ASSERT_EQ (1, pubOnDestroyCount);

    mamaPublisherCallbacks_deallocate(cb);
}

/*  Description: Create a mamaPublisher with event callbacks and mamaMsg, send the msg using 
 *               mamaPublisher then destroy both. A non-entitled source is used to generate
 *               publisher error events.
 *
 *  Expected Result: MAMA_STATUS_OK
 */
TEST_F (MamaPublisherTestC, EventSendWithCallbacksBadSource)
{
    mamaPublisher    publisher = NULL;
    mamaTransport    tport     = NULL;
    const char*      symbol    = getSymbol();
    const char*      source    = getBadSource();
    mamaMsg          msg       = NULL;
    mamaQueue        queue     = NULL;
    mamaPublisherCallbacks* cb = NULL;
    int              i         = 0;
    int              numErrors = 10;

    pubOnCreateCount = 0;
    pubOnErrorCount = 0;
    pubOnDestroyCount = 0;

    mamaPublisherCallbacks_allocate(&cb);
    cb->onError = (mama_publisherOnErrorCb) pubOnError;
    cb->onCreate = (mama_publisherOnCreateCb) pubOnCreate;
    cb->onDestroy = (mama_publisherOnDestroyCb) pubOnDestroy;

    ASSERT_EQ (MAMA_STATUS_OK, mama_open());

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaMsg_create (&msg));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaMsg_addString (msg, symbol, 101, source));

    ASSERT_EQ (MAMA_STATUS_OK,
               mama_getDefaultEventQueue (mBridge, &queue));
 
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_allocate (&tport));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_create (tport, getTransport(), mBridge));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_setTransportTopicCallback (tport, (mamaTransportTopicCB) transportTopicCb, NULL));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaPublisher_createWithCallbacks (&publisher, tport, queue, symbol, source, NULL, cb, NULL));

    for (i = 0; i < numErrors; i++)
    {
        ASSERT_EQ (MAMA_STATUS_OK,
                   mamaPublisher_send (publisher, msg));
    }

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaPublisher_destroy (publisher));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_destroy (tport));

    ASSERT_EQ (MAMA_STATUS_OK, mama_close());

    ASSERT_EQ (1, pubOnCreateCount);
    ASSERT_EQ (numErrors, pubOnErrorCount);
    ASSERT_EQ (1, pubOnDestroyCount);

    mamaPublisherCallbacks_deallocate(cb);
}

/*  Description: Create a mamaPublisher with event callbacks and mamaMsg, send the msg using 
 *               mamaPublisher then destroy both. A non-entitled source is used to generate
 *               publisher error events. But the error callback is not set, so no callbacks 
 *               should be received.
 *
 *  Expected Result: MAMA_STATUS_OK
 */
TEST_F (MamaPublisherTestC, EventSendWithCallbacksNoErrorCallback)
{
    mamaPublisher    publisher = NULL;
    mamaTransport    tport     = NULL;
    const char*      symbol    = getSymbol();
    const char*      source    = getBadSource();
    mamaMsg          msg       = NULL;
    mamaQueue        queue     = NULL;
    mamaPublisherCallbacks* cb = NULL;
    int              i         = 0;
    int              numErrors = 10;

    pubOnCreateCount = 0;
    pubOnErrorCount = 0;
    pubOnDestroyCount = 0;

    mamaPublisherCallbacks_allocate(&cb);
    cb->onError = NULL;
    cb->onCreate = (mama_publisherOnCreateCb) pubOnCreate;    /* No error callback */
    cb->onDestroy = (mama_publisherOnDestroyCb) pubOnDestroy;

    ASSERT_EQ (MAMA_STATUS_OK, mama_open());

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaMsg_create (&msg));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaMsg_addString (msg, symbol, 101, source));

    ASSERT_EQ (MAMA_STATUS_OK,
               mama_getDefaultEventQueue (mBridge, &queue));
 
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_allocate (&tport));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_create (tport, getTransport(), mBridge));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_setTransportTopicCallback (tport, (mamaTransportTopicCB) transportTopicCb, NULL));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaPublisher_createWithCallbacks (&publisher, tport, queue, symbol, source, NULL, cb, NULL));

    for (i = 0; i < numErrors; i++)
    {
        ASSERT_EQ (MAMA_STATUS_OK,
                   mamaPublisher_send (publisher, msg));
    }

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaPublisher_destroy (publisher));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_destroy (tport));

    ASSERT_EQ (MAMA_STATUS_OK, mama_close());

    ASSERT_EQ (1, pubOnCreateCount);
    ASSERT_EQ (0, pubOnErrorCount);
    ASSERT_EQ (1, pubOnDestroyCount);

    mamaPublisherCallbacks_deallocate(cb);
}

/*  Description: Create a mamaPublisher with event callbacks and mamaMsg, send the msg using 
 *               mamaPublisher then destroy both. A non-entitled source is used to generate
 *               publisher error events. But the callbacks are not set, so no callbacks 
 *               should be received.
 *
 *  Expected Result: MAMA_STATUS_OK
 */
TEST_F (MamaPublisherTestC, EventSendWithCallbacksNoCallbacks)
{
    mamaPublisher    publisher = NULL;
    mamaTransport    tport     = NULL;
    const char*      symbol    = getSymbol();
    const char*      source    = getBadSource();
    mamaMsg          msg       = NULL;
    mamaQueue        queue     = NULL;
    mamaPublisherCallbacks* cb = NULL;
    int              i         = 0;
    int              numErrors = 10;

    pubOnCreateCount = 0;
    pubOnErrorCount = 0;
    pubOnDestroyCount = 0;

    mamaPublisherCallbacks_allocate(&cb);
    cb->onError = NULL;
    cb->onCreate = NULL;
    cb->onDestroy = NULL;

    ASSERT_EQ (MAMA_STATUS_OK, mama_open());

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaMsg_create (&msg));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaMsg_addString (msg, symbol, 101, source));

    ASSERT_EQ (MAMA_STATUS_OK,
               mama_getDefaultEventQueue (mBridge, &queue));
 
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_allocate (&tport));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_create (tport, getTransport(), mBridge));

    ASSERT_EQ (MAMA_STATUS_OK,
		mamaTransport_setTransportTopicCallback (tport, (mamaTransportTopicCB) transportTopicCb, NULL));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaPublisher_createWithCallbacks (&publisher, tport, queue, symbol, source, NULL, cb, NULL));

    for (i = 0; i < numErrors; i++)
    {
        ASSERT_EQ (MAMA_STATUS_OK,
                   mamaPublisher_send (publisher, msg));
    }

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaPublisher_destroy (publisher));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_destroy (tport));

    ASSERT_EQ (MAMA_STATUS_OK, mama_close());

    ASSERT_EQ (0, pubOnCreateCount);
    ASSERT_EQ (0, pubOnErrorCount);
    ASSERT_EQ (0, pubOnDestroyCount);

    mamaPublisherCallbacks_deallocate(cb);
}

