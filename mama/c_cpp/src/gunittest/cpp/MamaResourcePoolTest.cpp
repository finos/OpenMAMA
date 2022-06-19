#include <gtest/gtest.h>
#include "mama/resourcepool.h"
#include "mama/mama.h"
#include "wombat/property.h"
#include "mama/MamaResourcePool.h"
#include "mama/MamaStatus.h"
#include "mama/mamacpp.h"
#include <cstring>
#include <cstdlib>
#include <memory>

using namespace Wombat;

class MamaResourcePoolTestCPP : public ::testing::Test
{	
protected:
    MamaResourcePoolTestCPP () {};
    ~MamaResourcePoolTestCPP () {};

    void SetUp() override {
        mTransportName = strdup("sub");
        mBridgeName = strdup("qpid");
        Mama::setProperty("mama.resource_pool.test.bridges", mBridgeName);
        Mama::setProperty("mama.resource_pool.test.default_transport_sub", mTransportName);
        Mama::setProperty("mama.resource_pool.test.default_source_sub", "SOURCE");
        char prop[PROPERTY_NAME_MAX_LENGTH];
        snprintf(prop, sizeof(prop), "mama.%s.transport.%s.param", mBridgeName, mTransportName);
        Mama::setProperty(prop, "value");
    };
    void TearDown () override {
        free ((void*)mBridgeName);
        free ((void*)mTransportName);
    };

    const char* mTransportName;
    const char* mBridgeName;
};

TEST_F (MamaResourcePoolTestCPP, CreateDestroy)
{
    auto pool = new MamaResourcePool("test");
    delete pool;
}

TEST_F (MamaResourcePoolTestCPP, CreateNullParams)
{
    ASSERT_THROW (new MamaResourcePool(nullptr), MamaStatus);
}

TEST_F (MamaResourcePoolTestCPP, CreateTransportFromNameDestroyTransport)
{
    std::unique_ptr<MamaResourcePool> pool(new MamaResourcePool("test"));
    auto* transport = pool->createTransportFromName (mTransportName);
    ASSERT_NE (transport, nullptr);
}


TEST_F (MamaResourcePoolTestCPP, CreateTransportInvalidParams)
{
    std::unique_ptr<MamaResourcePool> pool(new MamaResourcePool("test"));
    ASSERT_THROW (pool->createTransportFromName (nullptr), MamaStatus);
}

TEST_F (MamaResourcePoolTestCPP, CreateSubscriptionFromUri)
{
    std::unique_ptr<MamaResourcePool> pool(new MamaResourcePool("test"));
    class SubscriptionEventHandler : public MamaSubscriptionCallback
    {
        void onMsg (MamaSubscription* subscription, MamaMsg& msg) override {}
    };
    SubscriptionEventHandler eventHandler;
    auto *subscription = pool->createSubscriptionFromUri (
        "qpid://sub/SOURCE/test.topic", &eventHandler);
    ASSERT_STREQ (subscription->getSource()->getPublisherSourceName(), "SOURCE");
    ASSERT_STREQ (subscription->getSymbol(), "test.topic");
    ASSERT_STREQ (subscription->getTransport()->getName(), "sub");
}

TEST_F (MamaResourcePoolTestCPP, CreateSubscriptionFromUriInvalidUri)
{
    std::unique_ptr<MamaResourcePool> pool(new MamaResourcePool("test"));
    class SubscriptionEventHandler : public MamaSubscriptionCallback
    {
        void onMsg (MamaSubscription* subscription, MamaMsg& msg) override {}
    };
    SubscriptionEventHandler eventHandler;
    ASSERT_THROW (pool->createSubscriptionFromUri("plainString", &eventHandler), MamaStatus);
    ASSERT_THROW (pool->createSubscriptionFromUri(nullptr, &eventHandler), MamaStatus);
}

TEST_F (MamaResourcePoolTestCPP, CreateSubscriptionFromUriNullArgs) {
    std::unique_ptr<MamaResourcePool> pool(new MamaResourcePool("test"));
    class SubscriptionEventHandler : public MamaSubscriptionCallback
    {
        void onMsg (MamaSubscription* subscription, MamaMsg& msg) override {}
    };
    SubscriptionEventHandler eventHandler;
    ASSERT_THROW (pool->createSubscriptionFromUri("qpid://sub/SOURCE/test.topic", nullptr), MamaStatus);
}

TEST_F (MamaResourcePoolTestCPP, CreateSubscriptionFromComponents)
{
    std::unique_ptr<MamaResourcePool> pool(new MamaResourcePool("test"));
    class SubscriptionEventHandler : public MamaSubscriptionCallback
    {
        void onMsg (MamaSubscription* subscription, MamaMsg& msg) override {}
    };
    SubscriptionEventHandler eventHandler;
    auto *subscription = pool->createSubscriptionFromComponents ("sub", "SOURCE", "test.topic", &eventHandler);
    ASSERT_STREQ (subscription->getSource()->getPublisherSourceName(), "SOURCE");
    ASSERT_STREQ (subscription->getSymbol(), "test.topic");
    ASSERT_STREQ (subscription->getTransport()->getName(), "sub");
}

TEST_F (MamaResourcePoolTestCPP, CreateSubscriptionFromTopicWithSource)
{
    std::unique_ptr<MamaResourcePool> pool(new MamaResourcePool("test"));
    class SubscriptionEventHandler : public MamaSubscriptionCallback
    {
        void onMsg (MamaSubscription* subscription, MamaMsg& msg) override {}
    };
    SubscriptionEventHandler eventHandler;
    auto *subscription = pool->createSubscriptionFromTopicWithSource("SOURCE", "test.topic", &eventHandler);
    ASSERT_STREQ (subscription->getSource()->getPublisherSourceName(), "SOURCE");
    ASSERT_STREQ (subscription->getSymbol(), "test.topic");
    ASSERT_STREQ (subscription->getTransport()->getName(), "sub");
}

TEST_F (MamaResourcePoolTestCPP, CreateSubscriptionFromTopic)
{
    std::unique_ptr<MamaResourcePool> pool(new MamaResourcePool("test"));
    class SubscriptionEventHandler : public MamaSubscriptionCallback
    {
        void onMsg (MamaSubscription* subscription, MamaMsg& msg) override {}
    };
    SubscriptionEventHandler eventHandler;
    auto *subscription = pool->createSubscriptionFromTopic("test.topic", &eventHandler);
    ASSERT_STREQ (subscription->getSource()->getPublisherSourceName(), "SOURCE");
    ASSERT_STREQ (subscription->getSymbol(), "test.topic");
    ASSERT_STREQ (subscription->getTransport()->getName(), "sub");
}
