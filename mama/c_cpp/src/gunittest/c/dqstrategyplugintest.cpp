#include <gtest/gtest.h>
#include "MainUnitTestC.h"
#include "queueimpl.h"
#include "dqstrategy.h"
#include "subscriptionimpl.h"
#include "mama/transport.h"
#include "mama/types.h"
#include "mama/quality.h"
#include "dqstrategyplugin.c"
#include "mama/mamacpp.h"

class MamaDqStrategyPluginTestC : public ::testing::Test
{
    protected:
                                  MamaDqStrategyPluginTestC();
        virtual                   ~MamaDqStrategyPluginTestC();

        virtual void              SetUp();
        virtual void              TearDown();

        mamaPluginInfo            pluginInfo;
        dqStrategy                strategy;
        mamaDqContext             ctx;
        mamaSubscription          subscription;
        mamaMsgCallbacks          callbacks;
    public:
        MamaDqStrategyPluginTestC *m_this;
        mamaBridge                m_bridge;
        mamaMsg                   m_msg;
        mamaTransport             m_transport;
        mamaQueue                 m_queue;
        mamaSource                m_source;
        wombatThrottle            m_throttle;
};

MamaDqStrategyPluginTestC::MamaDqStrategyPluginTestC()
{
}

MamaDqStrategyPluginTestC::~MamaDqStrategyPluginTestC()
{
}
static void MAMACALLTYPE onCreate (mamaSubscription subscription, void* closure);

static void MAMACALLTYPE onError(mamaSubscription subscription,
                    mama_status      status,
                    void*            platformError,
                    const char*      subject,
                    void*            closure);

static void MAMACALLTYPE onQuality(mamaSubscription subsc,
                      mamaQuality      quality,
                      const char*      symbol,
                      short            cause,
                      const void*      platformInfo,
                      void*            closure);

static void MAMACALLTYPE onMsg(mamaSubscription subscription,
                  mamaMsg          msg,
                  void*            closure,
                  void*            itemClosure);

static void MAMACALLTYPE onGap(mamaSubscription subsc, void* closure);
static void MAMACALLTYPE onRecapRequest(mamaSubscription subsc, void* closure);
static void MAMACALLTYPE onDestroy(mamaSubscription subsc, void* closure);

void MamaDqStrategyPluginTestC::SetUp(void)
{
    m_this = this;
    ASSERT_EQ(MAMA_STATUS_OK, mama_loadBridge (&m_bridge, getMiddleware()));
    ASSERT_EQ(MAMA_STATUS_OK, mama_open());

    ASSERT_EQ(MAMA_STATUS_OK, mamaTransport_allocate (&m_transport));
    ASSERT_EQ(MAMA_STATUS_OK, mamaTransport_create (m_transport, NULL, m_bridge));

    ASSERT_EQ(MAMA_STATUS_OK, mamaMsg_create(&m_msg));
    ASSERT_EQ(MAMA_STATUS_OK, mamaMsg_addString(m_msg, getSymbol(), 101, getSource()));

    ASSERT_EQ(MAMA_STATUS_OK, mamaQueue_create(&m_queue, m_bridge));
    ASSERT_EQ(MAMA_STATUS_OK, mamaSource_create(&m_source));
    ASSERT_EQ(MAMA_STATUS_OK, mamaSource_setTransport(m_source, m_transport));

    callbacks.onCreate          = onCreate;
    callbacks.onError           = onError;
    callbacks.onQuality         = onQuality;
    callbacks.onMsg             = onMsg;
    callbacks.onGap             = onGap;
    callbacks.onRecapRequest    = onRecapRequest;
    callbacks.onDestroy         = onDestroy;

    ASSERT_EQ(MAMA_STATUS_OK, mamaSubscription_allocate(&subscription));
    ASSERT_EQ(MAMA_STATUS_OK, mamaSubscription_create(subscription, m_queue, &callbacks, m_source, "FR0000031122.EUR.XPAR", NULL));
    ASSERT_EQ(MAMA_STATUS_OK, wombatThrottle_allocate (&m_throttle));
    ASSERT_EQ(MAMA_STATUS_OK, wombatThrottle_create (m_throttle, m_queue));

    ASSERT_EQ(MAMA_STATUS_OK, dqStrategy_create(&strategy, subscription));
    ASSERT_EQ(MAMA_STATUS_OK, dqContext_initializeContext(&ctx, 100, NULL));
    ctx.mCache = (mamaMsg*) calloc(1, sizeof(mamaMsg));
}

void MamaDqStrategyPluginTestC::TearDown(void)
{
}

static void MAMACALLTYPE onCreate (mamaSubscription subscription,
                      void* closure)
{
}

static void MAMACALLTYPE onError(mamaSubscription subscription,
                    mama_status      status,
                    void*            platformError,
                    const char*      subject,
                    void*            closure)
{
}

static void MAMACALLTYPE onQuality(mamaSubscription subsc,
                      mamaQuality      quality,
                      const char*      symbol,
                      short            cause,
                      const void*      platformInfo,
                      void*            closure)
{
}

static void MAMACALLTYPE onMsg(mamaSubscription subscription,
                  mamaMsg          msg,
                  void*            closure,
                  void*            itemClosure)
{
}

static void MAMACALLTYPE onGap(mamaSubscription subsc, void* closure)
{
}

static void MAMACALLTYPE onRecapRequest(mamaSubscription subsc, void* closure)
{
}

static void MAMACALLTYPE onDestroy(mamaSubscription subsc, void* closure)
{
}

TEST_F (MamaDqStrategyPluginTestC, testSendRecapRequest)
{
    ASSERT_EQ(MAMA_STATUS_OK, imageRequest_create(&ctx.mRecapRequest, subscription, mamaSubscription_getSubjectContext(subscription, m_msg), NULL, m_throttle));
    ASSERT_EQ(MAMA_STATUS_OK, dqStrategy_sendRecapRequest(strategy, m_msg, &ctx));
}

TEST_F (MamaDqStrategyPluginTestC, testSetPossiblyStale)
{
    ctx.mDQState = DQ_STATE_OK;
    ASSERT_EQ(ctx.mDQState, DQ_STATE_OK);
    ASSERT_EQ(MAMA_STATUS_OK, dqStrategy_setPossiblyStale(&ctx));
    ASSERT_EQ(DQ_STATE_POSSIBLY_STALE, ctx.mDQState);
}

TEST_F (MamaDqStrategyPluginTestC, testGetDqState)
{
    dqState state;
    state = DQ_STATE_OK;
    ASSERT_EQ(MAMA_STATUS_OK, dqStrategy_setPossiblyStale(&ctx));
    ASSERT_EQ(MAMA_STATUS_OK, dqStrategy_getDqState(&ctx, &state));
    ASSERT_EQ(DQ_STATE_POSSIBLY_STALE, state);
}
TEST_F (MamaDqStrategyPluginTestC, testInitializeContext)
{
    ASSERT_EQ(MAMA_STATUS_OK, imageRequest_create(&ctx.mRecapRequest, subscription, mamaSubscription_getSubjectContext(subscription, m_msg), NULL, m_throttle));
    ctx.mCache = NULL;
    ASSERT_EQ(MAMA_STATUS_OK, dqContext_initializeContext(&ctx, 5, ctx.mRecapRequest));
    ASSERT_TRUE(NULL != ctx.mCache);
    ASSERT_EQ(MAMA_STATUS_OK, dqContext_initializeContext(&ctx, 0, ctx.mRecapRequest));
    ASSERT_EQ(NULL, ctx.mCache);
}

TEST_F (MamaDqStrategyPluginTestC, testCacheMsg)
{
    ASSERT_EQ(MAMA_STATUS_OK, mamaMsg_create(ctx.mCache));
    ASSERT_TRUE(NULL != ctx.mCache);
    ASSERT_EQ(MAMA_STATUS_OK, dqContext_cacheMsg(&ctx, *ctx.mCache));
}

TEST_F (MamaDqStrategyPluginTestC, testDetachMsg)
{
    ASSERT_EQ(MAMA_STATUS_OK, mamaMsg_create(ctx.mCache));
    ASSERT_TRUE(NULL != ctx.mCache);
    ASSERT_EQ(MAMA_STATUS_OK, dqStrategyImpl_detachMsg(&ctx, *ctx.mCache));
}

TEST_F (MamaDqStrategyPluginTestC, testGetRecoverGaps)
{ 
    int result;
    ASSERT_EQ(MAMA_STATUS_OK, dqStrategy_getRecoverGaps(strategy, &result));
    ASSERT_EQ(result, 1);
}
TEST_F (MamaDqStrategyPluginTestC, testSetRecoverGaps)
{
    int result;
    ASSERT_EQ(MAMA_STATUS_OK, dqStrategy_setRecoverGaps(strategy, 5));
    ASSERT_EQ(MAMA_STATUS_OK, dqStrategy_getRecoverGaps(strategy, &result));
    ASSERT_EQ(result, 5);
}

TEST_F (MamaDqStrategyPluginTestC, testDestroy)
{
    ASSERT_EQ(MAMA_STATUS_OK, dqStrategy_destroy(strategy));
}

TEST_F(MamaDqStrategyPluginTestC, testTransportEventHook)
{
    mamaQuality        quality;
    mamaTransportEvent tPortEvent;
    ASSERT_EQ(MAMA_STATUS_OK, dqstrategyMamaPlugin_transportEventHook(pluginInfo, m_transport, 0, tPortEvent));
    ASSERT_EQ(MAMA_STATUS_OK, mamaTransport_getQuality(m_transport, &quality));
    ASSERT_EQ(MAMA_QUALITY_MAYBE_STALE, quality);

    ASSERT_EQ(MAMA_STATUS_OK, dqstrategyMamaPlugin_transportEventHook(pluginInfo, m_transport, 1, tPortEvent));
    ASSERT_EQ(MAMA_STATUS_OK, mamaTransport_getQuality(m_transport, &quality));
    ASSERT_EQ(MAMA_QUALITY_MAYBE_STALE, quality);

}

TEST_F(MamaDqStrategyPluginTestC, testSubscriptionPostCreateEventHook)
{
    dqStrategy s;
    
    strategy->mRecoverGaps = 5;
    mamaSubscription_setDqStrategy(subscription, strategy);
    s = mamaSubscription_getDqStrategy(subscription);
    ASSERT_EQ(5, s->mRecoverGaps);
    
    ASSERT_EQ(MAMA_STATUS_OK, dqstrategyMamaPlugin_subscriptionPostCreateHook(pluginInfo, subscription));
    s = mamaSubscription_getDqStrategy(subscription);
    ASSERT_EQ(0, s->mRecoverGaps);
}

TEST_F(MamaDqStrategyPluginTestC, testIsInitialMesageOrRecap)
{
    ASSERT_EQ(0, isInitialMessageOrRecap(0));
    ASSERT_EQ(1, isInitialMessageOrRecap(1));
    ASSERT_EQ(0,isInitialMessageOrRecap(2));
    ASSERT_EQ(0, isInitialMessageOrRecap(3));
    ASSERT_EQ(0, isInitialMessageOrRecap(4));
    ASSERT_EQ(0, isInitialMessageOrRecap(5));
    ASSERT_EQ(1, isInitialMessageOrRecap(6));
}

TEST_F(MamaDqStrategyPluginTestC, testFillGap)
{

    ASSERT_EQ(MAMA_STATUS_OK, dqContext_initializeContext(&ctx, 5, ctx.mRecapRequest));
    ASSERT_TRUE(NULL != ctx.mCache);
    //setup cache with appropriate seqnum
    ASSERT_EQ(MAMA_STATUS_OK, mamaMsg_addI64(m_msg, "MdSeqNum", 10, 1));
    ASSERT_EQ(MAMA_STATUS_OK, dqContext_cacheMsg(&ctx, m_msg));
    //test both return values
    ASSERT_EQ(1 , fillGap(&ctx, 2, subscription));
    ASSERT_EQ(0 , fillGap(&ctx, 5, subscription));
}
