#include <gtest/gtest.h>
#include "mama/resourcepool.h"
#include "mama/mama.h"
#include "property.h"
#include <cstring>
#include <cstdlib>

class MamaResourcePoolTestC : public ::testing::Test
{	
protected:
    MamaResourcePoolTestC () {};
    virtual ~MamaResourcePoolTestC () {};

    void SetUp() override {
        mTransportName = strdup("sub");
        mBridgeName = strdup("qpid");
    };
    void TearDown () override {
        free ((void*)mBridgeName);
        free ((void*)mTransportName);
    };

    const char* mTransportName;
    const char* mBridgeName;
};

TEST_F (MamaResourcePoolTestC, CreateDestroy)
{
    mamaResourcePool pool;
    ASSERT_EQ (MAMA_STATUS_OK, mamaResourcePool_create (&pool, "test"));
    ASSERT_EQ (MAMA_STATUS_OK, mamaResourcePool_destroy (pool));
}

TEST_F (MamaResourcePoolTestC, CreateNullParams)
{
    mamaResourcePool pool;
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, mamaResourcePool_create (NULL, "test"));
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, mamaResourcePool_create (&pool, NULL));
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, mamaResourcePool_create (NULL, NULL));
}

TEST_F (MamaResourcePoolTestC, CreateTransportFromNameDestroyTransport)
{
    mamaResourcePool pool;
    mamaTransport transport = NULL;
    mama_setProperty("mama.resource_pool.test.bridges", mBridgeName);
    char prop[PROPERTY_NAME_MAX_LENGTH];
    snprintf(prop, sizeof(prop), "mama.%s.transport.%s.param", mBridgeName, mTransportName);
    mama_setProperty(prop, "value");
    ASSERT_EQ (MAMA_STATUS_OK, mamaResourcePool_create (&pool, "test"));
    ASSERT_EQ (MAMA_STATUS_OK, mamaResourcePool_createTransportFromName (
                                   pool,
                                   &transport,
                                   mTransportName));
    ASSERT_NE (transport, (void*)NULL);
    ASSERT_EQ (MAMA_STATUS_OK, mamaResourcePool_destroy (pool));
}


TEST_F (MamaResourcePoolTestC, CreateTransportInvalidParams)
{
    mamaResourcePool pool;
    mamaTransport transport = NULL;
    ASSERT_EQ (MAMA_STATUS_OK, mamaResourcePool_create (&pool, "test"));
    ASSERT_EQ (MAMA_STATUS_NULL_ARG,
               mamaResourcePool_createTransportFromName (
                   NULL, &transport, mTransportName));
    ASSERT_EQ (transport, (void*)NULL);
    ASSERT_EQ (MAMA_STATUS_NULL_ARG,
               mamaResourcePool_createTransportFromName (
                   pool, NULL, mTransportName));
    ASSERT_EQ (transport, (void*)NULL);
    ASSERT_EQ (MAMA_STATUS_NULL_ARG,
               mamaResourcePool_createTransportFromName (
                   pool, &transport, NULL));
    ASSERT_EQ (transport, (void*)NULL);
    ASSERT_EQ (MAMA_STATUS_NULL_ARG,
               mamaResourcePool_createTransportFromName (
                   NULL, NULL, NULL));
    ASSERT_EQ (transport, (void*)NULL);
    ASSERT_EQ (MAMA_STATUS_INVALID_ARG,
               mamaResourcePool_createTransportFromName (
                   pool, &transport, ""));
    ASSERT_EQ (transport, (void*)NULL);
    ASSERT_EQ (MAMA_STATUS_OK, mamaResourcePool_destroy (pool));
}

static void MAMACALLTYPE onResourcePoolTestSubMsg (mamaSubscription sub,
                                                   mamaMsg          msg,
                                                   void*            closure,
                                                   void*            itemClosure) {

}

TEST_F (MamaResourcePoolTestC, CreateSubscriptionFromUri)
{
    mamaResourcePool pool;
    ASSERT_EQ (MAMA_STATUS_OK, mamaResourcePool_create (&pool, "test"));
    mamaSubscription subscription;
    mamaMsgCallbacks callbacks;
    memset(&callbacks, 0, sizeof(mamaMsgCallbacks));
    callbacks.onMsg = onResourcePoolTestSubMsg;
    ASSERT_EQ (MAMA_STATUS_OK, mamaResourcePool_createSubscriptionFromUri(
                                   pool,
                                   &subscription,
                                   "qpid://sub/SOURCE/test.topic",
                                   callbacks,
                                   (void*)NULL));
    const char* sourceName;
    mamaSubscription_getSource (subscription, &sourceName);
    ASSERT_STREQ (sourceName, "SOURCE");
    const char* topicName;
    mamaSubscription_getSubscSymbol (subscription, &topicName);
    mamaTransport transport;
    mamaSubscription_getTransport (subscription, &transport);
    const char* transportName;
    mamaTransport_getName (transport, &transportName);
    ASSERT_STREQ (transportName, "sub");
    ASSERT_EQ (MAMA_STATUS_OK, mamaResourcePool_destroy (pool));
}

TEST_F (MamaResourcePoolTestC, CreateSubscriptionFromUriInvalidUri)
{
    mamaResourcePool pool;
    ASSERT_EQ (MAMA_STATUS_OK, mamaResourcePool_create (&pool, "test"));
    mamaSubscription subscription;
    mamaMsgCallbacks callbacks;
    memset(&callbacks, 0, sizeof(mamaMsgCallbacks));
    callbacks.onMsg = onResourcePoolTestSubMsg;
    ASSERT_EQ (MAMA_STATUS_INVALID_ARG,
               mamaResourcePool_createSubscriptionFromUri(
                                   pool,
                                   &subscription,
                                   "plainString",
                                   callbacks,
                                   (void*)NULL));
    ASSERT_EQ (MAMA_STATUS_NULL_ARG,
               mamaResourcePool_createSubscriptionFromUri(
                   pool,
                   &subscription,
                   NULL,
                   callbacks,
                   (void*)NULL));
    ASSERT_EQ (MAMA_STATUS_INVALID_ARG,
               mamaResourcePool_createSubscriptionFromUri(
                   pool,
                   &subscription,
                   "",
                   callbacks,
                   (void*)NULL));
    ASSERT_EQ (MAMA_STATUS_OK, mamaResourcePool_destroy (pool));
}

TEST_F (MamaResourcePoolTestC, CreateSubscriptionFromUriInvalidOnMsgCb) {
    mamaResourcePool pool;
    ASSERT_EQ (MAMA_STATUS_OK, mamaResourcePool_create (&pool, "test"));
    mamaSubscription subscription;
    // No onMsg callback provided
    mamaMsgCallbacks callbacks;
    memset(&callbacks, 0, sizeof(mamaMsgCallbacks));
    ASSERT_EQ (MAMA_STATUS_INVALID_ARG,
               mamaResourcePool_createSubscriptionFromUri(
                   pool,
                   &subscription,
                   "qpid://sub/SOURCE/test.topic",
                   callbacks,
                   (void*)NULL));
    ASSERT_EQ (MAMA_STATUS_OK, mamaResourcePool_destroy (pool));
}

TEST_F (MamaResourcePoolTestC, CreateSubscriptionFromUriNullArgs) {
    mamaResourcePool pool;
    ASSERT_EQ (MAMA_STATUS_OK, mamaResourcePool_create (&pool, "test"));
    mamaSubscription subscription;
    // No onMsg callback provided
    mamaMsgCallbacks callbacks;
    memset(&callbacks, 0, sizeof(mamaMsgCallbacks));
    callbacks.onMsg = onResourcePoolTestSubMsg;
    ASSERT_EQ (MAMA_STATUS_NULL_ARG,
               mamaResourcePool_createSubscriptionFromUri(
                   NULL,
                   &subscription,
                   "qpid://sub/SOURCE/test.topic",
                   callbacks,
                   (void*)NULL));
    ASSERT_EQ (MAMA_STATUS_NULL_ARG,
               mamaResourcePool_createSubscriptionFromUri(
                   pool,
                   NULL,
                   "qpid://sub/SOURCE/test.topic",
                   callbacks,
                   (void*)NULL));
    ASSERT_EQ (MAMA_STATUS_NULL_ARG,
               mamaResourcePool_createSubscriptionFromUri(
                   pool,
                   &subscription,
                   NULL,
                   callbacks,
                   (void*)NULL));
    ASSERT_EQ (MAMA_STATUS_OK, mamaResourcePool_destroy (pool));
}

TEST_F (MamaResourcePoolTestC, CreateSubscriptionFromComponents)
{
    mamaResourcePool pool;
    ASSERT_EQ (MAMA_STATUS_OK, mamaResourcePool_create (&pool, "test"));
    mamaSubscription subscription;
    mamaMsgCallbacks callbacks;
    memset(&callbacks, 0, sizeof(mamaMsgCallbacks));
    callbacks.onMsg = onResourcePoolTestSubMsg;
    mama_setProperty("mama.resource_pool.test.bridges", mBridgeName);
    char prop[PROPERTY_NAME_MAX_LENGTH];
    snprintf(prop, sizeof(prop), "mama.%s.transport.%s.param", mBridgeName, mTransportName);
    mama_setProperty(prop, "value");

    // Expected failure since we haven't configured a default bridge for this pool
    ASSERT_EQ (MAMA_STATUS_OK, mamaResourcePool_createSubscriptionFromComponents(
                                   pool,
                                   &subscription,
                                   "sub",
                                   "SOURCE",
                                   "test.topic",
                                   callbacks,
                                   (void*)NULL));

    const char* sourceName;
    mamaSubscription_getSource (subscription, &sourceName);
    ASSERT_STREQ (sourceName, "SOURCE");
    const char* topicName;
    mamaSubscription_getSubscSymbol (subscription, &topicName);
    mamaTransport transport;
    mamaSubscription_getTransport (subscription, &transport);
    const char* transportName;
    mamaTransport_getName (transport, &transportName);
    ASSERT_STREQ (transportName, "sub");
    ASSERT_EQ (MAMA_STATUS_OK, mamaResourcePool_destroy (pool));
}

TEST_F (MamaResourcePoolTestC, CreateSubscriptionFromTopicWithSource)
{
    mamaResourcePool pool;
    ASSERT_EQ (MAMA_STATUS_OK, mamaResourcePool_create (&pool, "test"));
    mamaSubscription subscription;
    mamaMsgCallbacks callbacks;
    memset(&callbacks, 0, sizeof(mamaMsgCallbacks));
    callbacks.onMsg = onResourcePoolTestSubMsg;
    mama_setProperty("mama.resource_pool.test.bridges", mBridgeName);
    mama_setProperty("mama.resource_pool.test.default_transport_sub", mTransportName);
    char prop[PROPERTY_NAME_MAX_LENGTH];
    snprintf(prop, sizeof(prop), "mama.%s.transport.%s.param", mBridgeName, mTransportName);
    mama_setProperty(prop, "value");

    // Expected failure since we haven't configured a default bridge for this pool
    ASSERT_EQ (MAMA_STATUS_OK, mamaResourcePool_createSubscriptionFromTopicWithSource(
                                   pool,
                                   &subscription,
                                   "SOURCE",
                                   "test.topic",
                                   callbacks,
                                   (void*)NULL));

    const char* sourceName;
    mamaSubscription_getSource (subscription, &sourceName);
    ASSERT_STREQ (sourceName, "SOURCE");
    const char* topicName;
    mamaSubscription_getSubscSymbol (subscription, &topicName);
    mamaTransport transport;
    mamaSubscription_getTransport (subscription, &transport);
    const char* transportName;
    mamaTransport_getName (transport, &transportName);
    ASSERT_STREQ (transportName, "sub");
    ASSERT_EQ (MAMA_STATUS_OK, mamaResourcePool_destroy (pool));
}

TEST_F (MamaResourcePoolTestC, CreateSubscriptionFromTopic)
{
    mamaResourcePool pool;
    ASSERT_EQ (MAMA_STATUS_OK, mamaResourcePool_create (&pool, "test"));
    mamaSubscription subscription;
    mamaMsgCallbacks callbacks;
    memset(&callbacks, 0, sizeof(mamaMsgCallbacks));
    callbacks.onMsg = onResourcePoolTestSubMsg;
    mama_setProperty("mama.resource_pool.test.bridges", mBridgeName);
    mama_setProperty("mama.resource_pool.test.default_transport_sub", mTransportName);
    mama_setProperty("mama.resource_pool.test.default_source_sub", "SOURCE");
    char prop[PROPERTY_NAME_MAX_LENGTH];
    snprintf(prop, sizeof(prop), "mama.%s.transport.%s.param", mBridgeName, mTransportName);
    mama_setProperty(prop, "value");

    // Expected failure since we haven't configured a default bridge for this pool
    ASSERT_EQ (MAMA_STATUS_OK, mamaResourcePool_createSubscriptionFromTopic(
                                   pool,
                                   &subscription,
                                   "test.topic",
                                   callbacks,
                                   (void*)NULL));

    const char* sourceName;
    mamaSubscription_getSource (subscription, &sourceName);
    ASSERT_STREQ (sourceName, "SOURCE");
    const char* topicName;
    mamaSubscription_getSubscSymbol (subscription, &topicName);
    mamaTransport transport;
    mamaSubscription_getTransport (subscription, &transport);
    const char* transportName;
    mamaTransport_getName (transport, &transportName);
    ASSERT_STREQ (transportName, "sub");
    ASSERT_EQ (MAMA_STATUS_OK, mamaResourcePool_destroy (pool));
}
