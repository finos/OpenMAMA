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
#include "MamaSubscriptionTest.h"

/* ************************************************************************* */
/* Construction and Destruction */
/* ************************************************************************* */

MamaSubscriptionTest::MamaSubscriptionTest(void)
{
    //m_bridge    = NULL;
    //m_transport = NULL;
}

MamaSubscriptionTest::~MamaSubscriptionTest(void)
{
}

/* ************************************************************************* */
/* Setup and Teardown */
/* ************************************************************************* */

void MamaSubscriptionTest::SetUp(void)
{   
	// Save the this pointer in the member variable to get around gtest problems
	m_this = this;

    // Load the bridge
    m_bridge = Mama::loadBridge(getMiddleware());

    // Open mama
    Mama::open();

    transportName[0] = '\0';
    strncat(transportName, "sub_", 5);
    strncat(transportName, getMiddleware(), 4);

    m_transport.create(transportName, m_bridge);

}

void MamaSubscriptionTest::TearDown(void)
{
    // Destroy the transport
    //delete m_transport;

    // Close mama
    Mama::close();

    // Clean the member this pointer
    m_this = NULL;   
}


/* ************************************************************************* */
/* Private Classes - Basic Subscription */
/* ************************************************************************* */

class TestBasicCallback : public MamaBasicSubscriptionCallback
{
    // Private member variables
    mamaBridge m_bridge;
    
public:
    
    TestBasicCallback(mamaBridge bridge)
    {
        m_bridge = bridge;
    }

     virtual void onCreate (
        MamaBasicSubscription*  subscription)
     {            
     }

     virtual void onError (
        MamaBasicSubscription*  subscription,
        const MamaStatus&       status,
        const char*             topic)
     {   
     }

     virtual void onMsg (
        MamaBasicSubscription*  subscription, 
        MamaMsg&          msg)
     {
         // Stop on the first message
         Mama::stop(m_bridge);
     }

     virtual void onDestroy(MamaBasicSubscription *subscription, void *closure)
     {
         Mama::stop(m_bridge);
     }    
};

class TestBasicCallback_RecreateOnMsg : public MamaBasicSubscriptionCallback
{
    // Private member variables
    mamaBridge m_bridge;
    TestBasicCallback *m_testCallback;
    MamaTransport *m_transport;
    
public:
    
     TestBasicCallback_RecreateOnMsg(mamaBridge bridge, MamaTransport *transport)
     {
         m_bridge = bridge;
         m_transport = transport;
         m_testCallback = new TestBasicCallback(bridge);
     }

     virtual ~TestBasicCallback_RecreateOnMsg(void)
     {
         if(NULL != m_testCallback)
         {
             delete m_testCallback;
         }
     }

     virtual void onCreate (
        MamaBasicSubscription*  subscription)
     {            
     }

     virtual void onError (
        MamaBasicSubscription*  subscription,
        const MamaStatus&       status,
        const char*             topic)
     {   
     }

     virtual void onMsg (
        MamaBasicSubscription*  subscription, 
        MamaMsg&          msg)
     {
         // Destroy the subscription
         subscription->destroy();

         // Get the default queue
         MamaQueue *queue = Mama::getDefaultEventQueue(m_bridge);

         /* Attempt to recreate, this should not work as the subscription will be in the
          * wrong state.
          */
         try
         {
            subscription->createBasic(m_transport, queue, m_testCallback, "MAMA_TOPIC");                  
         }

         catch(MamaStatus mamaStatus)
         {
             // Check for the correct error
             if(mamaStatus.getStatus() == MAMA_STATUS_SUBSCRIPTION_INVALID_STATE)
             {
                // Stop and quit
                Mama::stop(m_bridge);
                return;
             }             
         }

         // If got here then something is wrong
         throw new MamaStatus(MAMA_STATUS_SYSTEM_ERROR);
     }    
};


class TestBasicCallback_RecreateOnDestroy : public MamaBasicSubscriptionCallback
{
    // Private member variables
    mamaBridge m_bridge;
    TestBasicCallback *m_testCallback;
    MamaTransport *m_transport;
    
public:
    
     TestBasicCallback_RecreateOnDestroy(mamaBridge bridge, MamaTransport *transport)
     {
         m_bridge = bridge;
         m_transport = transport;
         m_testCallback = new TestBasicCallback(bridge);
     }

     virtual ~TestBasicCallback_RecreateOnDestroy(void)
     {
         if(NULL != m_testCallback)
         {
             delete m_testCallback;
         }
     }

     virtual void onCreate (
        MamaBasicSubscription*  subscription)
     {            
     }

     virtual void onDestroy(MamaBasicSubscription *subscription, void *closure)
     {
         // Get the default queue
         MamaQueue *queue = Mama::getDefaultEventQueue(m_bridge);

         /* Recreate the subscription using the normal test callback meaning that mama_stop
          * will be called during the next tick.
          */
         subscription->createBasic(m_transport, queue, m_testCallback, "MAMA_TOPIC");        
     }

     virtual void onError (
        MamaBasicSubscription*  subscription,
        const MamaStatus&       status,
        const char*             topic)
     {   
     }

     virtual void onMsg (
        MamaBasicSubscription*  subscription, 
        MamaMsg&          msg)
     {
         // Destroy the subscription
         subscription->destroy();
     }    
};

/* ************************************************************************* */
/* Private Classes - Subscription */
/* ************************************************************************* */

class TestCallback : public MamaSubscriptionCallback
{
    // Private member variables
    mamaBridge m_bridge;

public:
    
    TestCallback(mamaBridge bridge)
    {
        m_bridge = bridge;
    }

     virtual void onCreate (
        MamaSubscription*  subscription)
     {
     }

     virtual void onError (
        MamaSubscription*  subscription,
        const MamaStatus&  status,
        const char*        symbol)
     {
     }

     virtual void onGap (
        MamaSubscription*  subscription)
     {
     }

     virtual void onDestroy (
        MamaSubscription*  subscription)
     {
         // Unblock
         Mama::stop(m_bridge);
     }

     virtual void onRecapRequest (MamaSubscription*  subscription)
     {
     }

     virtual void onQuality (
        MamaSubscription*  subscription,
        mamaQuality        quality,
        const char*        symbol,
        short              cause,
        const void*        platformInfo)
     {
     }

     virtual void onMsg (
        MamaSubscription*  subscription, 
        MamaMsg&     msg)
     {
         // Stop on the first message
         Mama::stop(m_bridge);
     }    
};

class TestCallback_RecreateOnMsg : public MamaSubscriptionCallback
{
    // Private member variables
    mamaBridge m_bridge;
    TestCallback *m_testCallback;
    MamaTransport *m_transport;

public:
    
     TestCallback_RecreateOnMsg(mamaBridge bridge, MamaTransport *transport)
     {
         m_bridge = bridge;
         m_transport = transport;
         m_testCallback = new TestCallback(bridge);
     }

     virtual ~TestCallback_RecreateOnMsg(void)
     {
         if(NULL != m_testCallback)
         {
             delete m_testCallback;
         }
     }

     virtual void onCreate (
        MamaSubscription*  subscription)
     {
     }

     virtual void onError (
        MamaSubscription*  subscription,
        const MamaStatus&  status,
        const char*        symbol)
     {
     }

     virtual void onGap (
        MamaSubscription*  subscription)
     {
     }

     virtual void onDestroy (
        MamaSubscription*  subscription)
     {
         // Unblock
         Mama::stop(m_bridge);
     }

     virtual void onRecapRequest (MamaSubscription*  subscription)
     {
     }

     virtual void onQuality (
        MamaSubscription*  subscription,
        mamaQuality        quality,
        const char*        symbol,
        short              cause,
        const void*        platformInfo)
     {
     }

     virtual void onMsg (
        MamaSubscription*  subscription, 
        MamaMsg&     msg)
     {
         // Destroy the subscription
         subscription->destroy();

         // Get the default queue
         MamaQueue *queue = Mama::getDefaultEventQueue(m_bridge);

         /* Attempt to recreate, this should not work as the subscription will be in the
          * wrong state.
          */
         try
         {
            subscription->create(m_transport, queue, m_testCallback, "CTA_3", "IBM");
         }

         catch(MamaStatus mamaStatus)
         {
             // Check for the correct error
             if(mamaStatus.getStatus() == MAMA_STATUS_SUBSCRIPTION_INVALID_STATE)
             {
                // Stop and quit
                Mama::stop(m_bridge);
                return;
             }             
         }

         // If got here then something is wrong
         throw new MamaStatus(MAMA_STATUS_SYSTEM_ERROR);
     }    
};

class TestCallback_RecreateOnDestroy : public MamaSubscriptionCallback
{
    // Private member variables
    mamaBridge m_bridge;
    TestCallback *m_testCallback;
    MamaTransport *m_transport;

public:
    
     TestCallback_RecreateOnDestroy(mamaBridge bridge, MamaTransport *transport)
     {
         m_bridge = bridge;
         m_transport = transport;
         m_testCallback = new TestCallback(bridge);
     }

     virtual ~TestCallback_RecreateOnDestroy(void)
     {
         if(NULL != m_testCallback)
         {
             delete m_testCallback;
         }
     }

     virtual void onCreate (
        MamaSubscription*  subscription)
     {
     }

     virtual void onError (
        MamaSubscription*  subscription,
        const MamaStatus&  status,
        const char*        symbol)
     {
     }

     virtual void onGap (
        MamaSubscription*  subscription)
     {
     }

     virtual void onDestroy (
        MamaSubscription*  subscription)
     {
         // Get the default queue
         MamaQueue *queue = Mama::getDefaultEventQueue(m_bridge);

         /* Recreate the subscription using the normal test callback meaning that mama_stop
          * will be called during the next tick.
          */
         subscription->create(m_transport, queue, m_testCallback, "CTA_3", "IBM");
     }

     virtual void onRecapRequest (MamaSubscription*  subscription)
     {
     }

     virtual void onQuality (
        MamaSubscription*  subscription,
        mamaQuality        quality,
        const char*        symbol,
        short              cause,
        const void*        platformInfo)
     {
     }

     virtual void onMsg (
        MamaSubscription*  subscription, 
        MamaMsg&     msg)
     {
         // Destroy the subscription
         subscription->destroy();
     }    
};

#if 0
/* ************************************************************************* */
/* Private Classes - Timer */
/* ************************************************************************* */

class TestTimerCallback : public MamaTimerCallback
{
    // Private member variables
    mamaBridge m_bridge;
    
public:
    
     TestTimerCallback(mamaBridge bridge)
     {
         m_bridge = bridge;
     }

     virtual void onTimer (MamaTimer* timer)
     {
         // Stop on the first message
         Mama::stop(m_bridge);
     }    
};

class TestTimerCallback_RecreateOnTick : public MamaTimerCallback
{
    // Private member variables
    mamaBridge m_bridge;    
    TestTimerCallback *m_testCallback;
    
public:
    
     TestTimerCallback_RecreateOnTick(mamaBridge bridge)
     {
        m_bridge = bridge;
        m_testCallback = new TestTimerCallback(bridge);
     }

     virtual ~TestTimerCallback_RecreateOnTick(void)
     {
         if(NULL != m_testCallback)
         {
             delete m_testCallback;
         }
     }

     virtual void onTimer(MamaTimer* timer)
     {
         // Destroy the timer
         timer->destroy();

         // Get the default queue
         MamaQueue *queue = Mama::getDefaultEventQueue(m_bridge);

         /* Recreate the timer using the normal tick callback meaning that mama_stop
          * will be called during the next tick.
          */
         timer->create(queue, m_testCallback, 1);
     }    
};

class TestTimerCallbackEx : public MamaTimerCallbackEx
{
    // Private member variables
    mamaBridge m_bridge;
    
public:
    
     TestTimerCallbackEx(mamaBridge bridge)
     {
         m_bridge = bridge;
     }

     virtual void onTimer (MamaTimer* timer)
     {
         // Stop on the first message
         Mama::stop(m_bridge);
     }

     virtual void onDestroy(MamaTimer *timer, void *closure)
     {
         Mama::stop(m_bridge);
     }
};

class TestTimerCallbackEx_RecreateOnDestroy : public MamaTimerCallbackEx
{
    // Private member variables
    mamaBridge m_bridge;
    TestTimerCallback *m_testCallback;
    
public:
    
     TestTimerCallbackEx_RecreateOnDestroy(mamaBridge bridge)
     {
         m_bridge = bridge;
         m_testCallback = new TestTimerCallback(bridge);
     }

     virtual ~TestTimerCallbackEx_RecreateOnDestroy(void)
     {
         if(NULL != m_testCallback)
         {
             delete m_testCallback;
         }
     }

     virtual void onTimer (MamaTimer* timer)
     {
         // Destroy the timer
         timer->destroy();         
     }

     virtual void onDestroy(MamaTimer *timer, void *closure)
     {
         // Get the default queue
         MamaQueue *queue = Mama::getDefaultEventQueue(m_bridge);

         /* Recreate the timer using the normal tick callback meaning that mama_stop
          * will be called during the next tick.
          */
         timer->create(queue, m_testCallback, 1);
     }
};

/* ************************************************************************* */
/* Private Classes - Inbox */
/* ************************************************************************* */

class TestInboxCallback : public MamaInboxCallback
{
    // Private member variables
    mamaBridge m_bridge;
    
public:
    
    TestInboxCallback(mamaBridge bridge)
    {
        m_bridge = bridge;
    }

    virtual void onMsg (
        MamaInbox*         inbox,
        MamaMsg&     msg)
    {
        Mama::stop(m_bridge);
    }

    virtual void onError (
        MamaInbox*         inbox,
        const MamaStatus&  status)
    {
    }
};

class TestInboxCallbackEx : public MamaInboxCallbackEx
{
    // Private member variables
    mamaBridge m_bridge;
    
public:
    
    TestInboxCallbackEx(mamaBridge bridge)
    {
        m_bridge = bridge;
    }

    virtual void onMsg (
        MamaInbox*         inbox,
        MamaMsg&     msg)
    {
        Mama::stop(m_bridge);
    }

    virtual void onError (
        MamaInbox*         inbox,
        const MamaStatus&  status)
    {
    }
    
    virtual void onDestroy(MamaInbox *inbox, void *closure)
    {
        Mama::stop(m_bridge);
    }
};

class TestInboxCallback_RecreateOnMsg : public MamaInboxCallback
{
    // Private member variables
    mamaBridge m_bridge;    
    MamaTransport *m_transport;
    TestInboxCallback *m_testCallback;
    
public:
    
     TestInboxCallback_RecreateOnMsg(mamaBridge bridge, MamaTransport *transport)
     {
        m_bridge = bridge;
        m_transport = transport;
        m_testCallback = new TestInboxCallback(bridge);
     }

     virtual ~TestInboxCallback_RecreateOnMsg(void)
     {
         if(NULL != m_testCallback)
         {
             delete m_testCallback;
         }
     }

    virtual void onMsg(MamaInbox *inbox, MamaMsg &msg)
    {
        // Destroy the inbox
        inbox->destroy();

        // Get the default queue
        MamaQueue *queue = Mama::getDefaultEventQueue(m_bridge);

        // Recreate the inbox
        inbox->create(m_transport, queue, m_testCallback);

        // Create a message
        MamaMsg message;
        message.create();
        message.addI32("field", 1, 222);

        // Create a publisher
        MamaPublisher publisher;
        publisher.create(m_transport, "MAMA_INBOUND_TOPIC");

        // Send the inbox request
        publisher.sendFromInbox(inbox, &message);
    }

    virtual void onError(MamaInbox *inbox, const MamaStatus &status)
    {
    }
};

class TestInboxCallback_RecreateOnDestroy : public MamaInboxCallback
{
    // Private member variables
    mamaBridge m_bridge;    
    MamaTransport *m_transport;
    TestInboxCallback *m_testCallback;
    
public:
    
     TestInboxCallback_RecreateOnDestroy(mamaBridge bridge, MamaTransport *transport)
     {
        m_bridge = bridge;
        m_transport = transport;
        m_testCallback = new TestInboxCallback(bridge);
     }

     virtual ~TestInboxCallback_RecreateOnDestroy(void)
     {
         if(NULL != m_testCallback)
         {
             delete m_testCallback;
         }
     }

     virtual void onMsg(MamaInbox *inbox, MamaMsg &msg)
     {
         // Destroy the inbox
         inbox->destroy();
     }

     virtual void onError(MamaInbox *inbox, const MamaStatus &status)
     {
     }

     virtual void onDestroy(MamaInbox *inbox, void *closure)
     {
         // Get the default queue
         MamaQueue *queue = Mama::getDefaultEventQueue(m_bridge);

         // Recreate the inbox
         inbox->create(m_transport, queue, m_testCallback);

         // Create a message
         MamaMsg message;
         message.create();
         message.addI32("field", 1, 222);

         // Create a publisher
         MamaPublisher publisher;
         publisher.create(m_transport, "MAMA_INBOUND_TOPIC");

         // Send the inbox request
         publisher.sendFromInbox(inbox, &message);         
     }
};

#endif

/* ************************************************************************* */
/* Test Functions - Basic Subscription */
/* ************************************************************************* */

TEST_F(MamaSubscriptionTest, BasicSubscription)
{
    // Create a callback object
    TestBasicCallback *testCallback = new TestBasicCallback(m_bridge);
    if(NULL != testCallback)
    {
        // Allocate a basic subscription
        MamaBasicSubscription *basicSubscription = new MamaBasicSubscription();
        if(NULL != basicSubscription)
        {        
            // Get the default queue
            MamaQueue *queue = Mama::getDefaultEventQueue(m_bridge);

            // Create the subscription
            //basicSubscription->createBasic(m_transport, queue, testCallback, "CTA_3.IBM");
            basicSubscription->createBasic(&m_transport, queue, testCallback, "JGRAY");
            printf("subscription created for JGRAY-  start dispatching thread on default queue\n");
            // Process messages until the first message is received
            
            Mama::start(m_bridge);

            // Destroy the subscription
            basicSubscription->destroy();            

            // Delete the basic subscription
            delete basicSubscription;
        }

        delete testCallback;
    }
}
#if 0
TEST_F(MamaSubscriptionTest, BasicSubscriptionEx)
{
    // Create a callback object
    TestBasicCallbackEx *testCallback = new TestBasicCallbackEx(m_bridge);
    if(NULL != testCallback)
    {
        // Allocate a basic subscription
        MamaBasicSubscription *basicSubscription = new MamaBasicSubscription();
        if(NULL != basicSubscription)
        {        
            // Get the default queue
            MamaQueue *queue = Mama::getDefaultEventQueue(m_bridge);

            // Create the subscription
            basicSubscription->createBasic(m_transport, queue, testCallback, "MAMA_TOPIC");

            // Process messages until the first message is received
            Mama::start(m_bridge);

            // Destroy the subscription
            basicSubscription->destroy();            

            // Process messages until the destroy is received
            Mama::start(m_bridge);

            // Delete the basic subscription
            delete basicSubscription;
        }

        delete testCallback;
    }
}

TEST_F(MamaSubscriptionTest, BasicSubscriptionRecreateOnDestroy)
{
    // Create a callback object
    TestBasicCallbackEx_RecreateOnDestroy *testCallback = new TestBasicCallbackEx_RecreateOnDestroy(m_bridge, m_transport);
    if(NULL != testCallback)
    {
        // Allocate a basic subscription
        MamaBasicSubscription *basicSubscription = new MamaBasicSubscription();
        if(NULL != basicSubscription)
        {        
            // Get the default queue
            MamaQueue *queue = Mama::getDefaultEventQueue(m_bridge);

            // Create the subscription
            basicSubscription->createBasic(m_transport, queue, testCallback, "MAMA_TOPIC");

            // Process messages until the first message is received
            Mama::start(m_bridge);

            // Destroy the subscription
            basicSubscription->destroy();            

            // Delete the basic subscription
            delete basicSubscription;
        }

        delete testCallback;
    }
}

TEST_F(MamaSubscriptionTest, BasicSubscriptionRecreateOnMsg)
{
    // Create a callback object
    TestBasicCallback_RecreateOnMsg *testCallback = new TestBasicCallback_RecreateOnMsg(m_bridge, m_transport);
    if(NULL != testCallback)
    {
        // Allocate a basic subscription
        MamaBasicSubscription *basicSubscription = new MamaBasicSubscription();
        if(NULL != basicSubscription)
        {        
            // Get the default queue
            MamaQueue *queue = Mama::getDefaultEventQueue(m_bridge);

            // Create the subscription
            basicSubscription->createBasic(m_transport, queue, testCallback, "MAMA_TOPIC");

            // Process messages until the first message is received
            Mama::start(m_bridge);

            // Destroy the subscription
            basicSubscription->destroy();            

            // Delete the basic subscription
            delete basicSubscription;
        }

        delete testCallback;
    }
}


/* ************************************************************************* */
/* Test Functions - Subscription */
/* ************************************************************************* */

TEST_F(MamaSubscriptionTest, Subscription)
{
    // Create a callback object
    TestCallback *testCallback = new TestCallback(m_bridge);
    if(NULL != testCallback)
    {
        // Allocate a subscription
        MamaSubscription *subscription = new MamaSubscription();
        if(NULL != subscription)
        {        
            // Get the default queue
            MamaQueue *queue = Mama::getDefaultEventQueue(m_bridge);

            // Create the subscription
            subscription->create(m_transport, queue, testCallback, "CTA_3", "IBM");

            // Process messages until the first message is received
            Mama::start(m_bridge);

            // Destroy the subscription
            subscription->destroy();            

            // Keep processing messages until the destroy is received
            Mama::start(m_bridge);

            // Delete the subscription
            delete subscription;
        }

        delete testCallback;
    }
}

TEST_F(MamaSubscriptionTest, Subscription)
{
    // Create a callback object
    TestCallback *testCallback = new TestCallback(m_bridge);
    if(NULL != testCallback)
    {
        // Allocate a subscription
        MamaSubscription *subscription = new MamaSubscription();
        if(NULL != subscription)
        {        
            // Get the default queue
            MamaQueue *queue = Mama::getDefaultEventQueue(m_bridge);

            // Create the subscription
            subscription->create(m_transport, queue, testCallback, "CTA_3", "IBM");

            // Process messages until the first message is received
            Mama::start(m_bridge);

            // Destroy the subscription
            subscription->destroy();            

            // Process messages until the destroy is received
            Mama::start(m_bridge);

            // Delete the basic subscription
            delete subscription;
        }

        delete testCallback;
    }
}

TEST_F(MamaSubscriptionTest, Subscription_RecreateOnDestroy)
{
    // Create a callback object
    TestCallback_RecreateOnDestroy *testCallback = new TestCallback_RecreateOnDestroy(m_bridge, m_transport);
    if(NULL != testCallback)
    {
        // Allocate a subscription
        MamaSubscription *subscription = new MamaSubscription();
        if(NULL != subscription)
        {        
            // Get the default queue
            MamaQueue *queue = Mama::getDefaultEventQueue(m_bridge);

            // Create the subscription
            subscription->create(m_transport, queue, testCallback, "CTA_3", "IBM");

            // Process messages until the first message is received
            Mama::start(m_bridge);

            // Destroy the subscription
            subscription->destroy();    

            // Keep processing messages until the destroy is received
            Mama::start(m_bridge);

            // Delete the subscription
            delete subscription;
        }

        delete testCallback;
    }
}

TEST_F(MamaSubscriptionTest, Subscription_RecreateOnMsg)
{
    // Create a callback object
    TestCallback_RecreateOnMsg *testCallback = new TestCallback_RecreateOnMsg(m_bridge, m_transport);
    if(NULL != testCallback)
    {
        // Allocate a subscription
        MamaSubscription *subscription = new MamaSubscription();
        if(NULL != subscription)
        {        
            // Get the default queue
            MamaQueue *queue = Mama::getDefaultEventQueue(m_bridge);

            // Create the subscription
            subscription->create(m_transport, queue, testCallback, "CTA_3", "IBM");

            // Process messages until the first message is received
            Mama::start(m_bridge);

            // Destroy the subscription
            subscription->destroy();       

            // Keep processing messages until the destroy is received
            Mama::start(m_bridge);

            // Delete the subscription
            delete subscription;
        }

        delete testCallback;
    }
}

/* ************************************************************************* */
/* Test Functions - Timer */
/* ************************************************************************* */

TEST_F(MamaSubscriptionTest, Timer)
{
    // Create a callback object
    TestTimerCallback *testCallback = new TestTimerCallback(m_bridge);
    if(NULL != testCallback)
    {
        // Allocate a timer
        MamaTimer *timer = new MamaTimer();
        if(NULL != timer)
        {        
            // Get the default queue
            MamaQueue *queue = Mama::getDefaultEventQueue(m_bridge);

            // Create the timer
            timer->create(queue, testCallback, 1);
            
            // Process messages until the first message is received
            Mama::start(m_bridge);

            // Destroy the timer
            timer->destroy();            

            // Delete the timer
            delete timer;
        }

        delete testCallback;
    }
}

TEST_F(MamaSubscriptionTest, Timer)
{
    // Create a callback object
    TestTimerCallback *testCallback = new TestTimerCallback(m_bridge);
    if(NULL != testCallback)
    {
        // Allocate a timer
        MamaTimer *timer = new MamaTimer();
        if(NULL != timer)
        {        
            // Get the default queue
            MamaQueue *queue = Mama::getDefaultEventQueue(m_bridge);

            // Create the timer
            timer->create(queue, testCallback, 1);

            // Process messages until the first message is received
            Mama::start(m_bridge);

            // Destroy the timer
            timer->destroy();            

            // Process messages until the destroy is received
            Mama::start(m_bridge);

            // Delete the timer
            delete timer;
        }

        delete testCallback;
    }
}

TEST_F(MamaSubscriptionTest, TimerRecreateOnDestroy)
{
    // Create a callback object
    TestTimerCallback_RecreateOnDestroy *testCallback = new TestTimerCallback_RecreateOnDestroy(m_bridge);
    if(NULL != testCallback)
    {
        // Allocate a timer
        MamaTimer *timer = new MamaTimer();
        if(NULL != timer)
        {        
            // Get the default queue
            MamaQueue *queue = Mama::getDefaultEventQueue(m_bridge);

            // Create the timer
            timer->create(queue, testCallback, 1);
            
            // Process messages until the first message is received
            Mama::start(m_bridge);

            // Destroy the timer
            timer->destroy();            

            // Delete the timer
            delete timer;
        }

        delete testCallback;
    }
}

TEST_F(MamaSubscriptionTest, TimerRecreateOnTick)
{
    // Create a callback object
    TestTimerCallback_RecreateOnTick *testCallback = new TestTimerCallback_RecreateOnTick(m_bridge);
    if(NULL != testCallback)
    {
        // Allocate a timer
        MamaTimer *timer = new MamaTimer();
        if(NULL != timer)
        {        
            // Get the default queue
            MamaQueue *queue = Mama::getDefaultEventQueue(m_bridge);

            // Create the timer
            timer->create(queue, testCallback, 1);
            
            // Process messages until the first message is received
            Mama::start(m_bridge);

            // Destroy the timer
            timer->destroy();            

            // Delete the timer
            delete timer;
        }

        delete testCallback;
    }
}

/* ************************************************************************* */
/* Test Functions - Inbox */
/* ************************************************************************* */

TEST_F(MamaSubscriptionTest, Inbox)
{
    // Create a callback object
    TestInboxCallback *testCallback = new TestInboxCallback(m_bridge);
    if(NULL != testCallback)
    {
        // Allocate an inbox
        MamaInbox *inbox = new MamaInbox();
        if(NULL != inbox)
        {        
            // Get the default queue
            MamaQueue *queue = Mama::getDefaultEventQueue(m_bridge);

            // Create the inbox
            inbox->create(m_transport, queue, testCallback);

            // Create a message
            MamaMsg message;
            message.create();
            message.addI32("field", 1, 222);

            // Create a publisher
            MamaPublisher publisher;
            publisher.create(m_transport, "MAMA_INBOUND_TOPIC");

            // Send the inbox request
            publisher.sendFromInbox(inbox, &message);
            
            // Process messages until the first message is received
            Mama::start(m_bridge);

            // Destroy the inbox
            inbox->destroy();            

            // Delete the inbox
            delete inbox;
        }

        delete testCallback;
    }
}

TEST_F(MamaSubscriptionTest, Inbox)
{
    // Create a callback object
    TestInboxCallback *testCallback = new TestInboxCallback(m_bridge);
    if(NULL != testCallback)
    {
        // Allocate an inbox
        MamaInbox *inbox = new MamaInbox();
        if(NULL != inbox)
        {        
            // Get the default queue
            MamaQueue *queue = Mama::getDefaultEventQueue(m_bridge);

            // Create the inbox
            inbox->create(m_transport, queue, testCallback);

            // Create a message
            MamaMsg message;
            message.create();
            message.addI32("field", 1, 222);

            // Create a publisher
            MamaPublisher publisher;
            publisher.create(m_transport, "MAMA_INBOUND_TOPIC");

            // Send the inbox request
            publisher.sendFromInbox(inbox, &message);

            // Process messages until the first message is received
            Mama::start(m_bridge);

            // Destroy the inbox
            inbox->destroy();            

            // Process messages until the destroy is received
            Mama::start(m_bridge);

            // Delete the inbox
            delete inbox;
        }

        delete testCallback;
    }
}

TEST_F(MamaSubscriptionTest, InboxRecreateOnDestroy)
{
    // Create a callback object
    TestInboxCallback_RecreateOnDestroy *testCallback = new TestInboxCallback_RecreateOnDestroy(m_bridge, m_transport);
    if(NULL != testCallback)
    {
        // Allocate an inbox
        MamaInbox *inbox = new MamaInbox();
        if(NULL != inbox)
        {        
            // Get the default queue
            MamaQueue *queue = Mama::getDefaultEventQueue(m_bridge);

            // Create the inbox
            inbox->create(m_transport, queue, testCallback);

            // Create a message
            MamaMsg message;
            message.create();
            message.addI32("field", 1, 222);

            // Create a publisher
            MamaPublisher publisher;
            publisher.create(m_transport, "MAMA_INBOUND_TOPIC");

            // Send the inbox request
            publisher.sendFromInbox(inbox, &message);

            // Process messages until the first message is received
            Mama::start(m_bridge);

            // Destroy the inbox
            inbox->destroy();            

            // Delete the inbox
            delete inbox;
        }

        delete testCallback;
    }
}

TEST_F(MamaSubscriptionTest, InboxRecreateOnMsg)
{
    // Create a callback object
    TestInboxCallback_RecreateOnMsg *testCallback = new TestInboxCallback_RecreateOnMsg(m_bridge, m_transport);
    if(NULL != testCallback)
    {
        // Allocate a timer
        MamaInbox *inbox = new MamaInbox();
        if(NULL != inbox)
        {        
            // Get the default queue
            MamaQueue *queue = Mama::getDefaultEventQueue(m_bridge);

            // Create the inbox
            inbox->create(m_transport, queue, testCallback);

            // Create a message
            MamaMsg message;
            message.create();
            message.addI32("field", 1, 222);

            // Create a publisher
            MamaPublisher publisher;
            publisher.create(m_transport, "MAMA_INBOUND_TOPIC");

            // Send the inbox request
            publisher.sendFromInbox(inbox, &message);
            
            // Process messages until the first message is received
            Mama::start(m_bridge);

            // Destroy the inbox
            inbox->destroy();            

            // Delete the inbox
            delete inbox;
        }

        delete testCallback;
    }
}
#endif
