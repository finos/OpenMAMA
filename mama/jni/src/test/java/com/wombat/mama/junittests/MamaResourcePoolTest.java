package com.wombat.mama.junittests;

import com.wombat.mama.*;
import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import java.util.logging.Level;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotEquals;

public class MamaResourcePoolTest extends MamaTestBaseTestCase {
    private final String mTransportName = "sub";
    private final String mBridgeName = getBridgeName();
    private MamaResourcePool mPool;

    static class SubscriptionEventHandler implements MamaSubscriptionCallback
    {
        @Override
        public void onCreate(MamaSubscription subscription) {
        }

        @Override
        public void onError(MamaSubscription subscription, short wombatStatus, int platformError, String subject, Exception e) {
        }

        @Override
        public void onMsg(MamaSubscription subscription, MamaMsg msg) {
        }

        @Override
        public void onQuality(MamaSubscription subscription, short quality, short cause, Object platformInfo) {
        }

        @Override
        public void onRecapRequest(MamaSubscription subscription) {

        }

        @Override
        public void onGap(MamaSubscription subscription) {

        }

        @Override
        public void onDestroy(MamaSubscription subscription) {

        }
    }

    @Before
    public void setUp()
    {
        Mama.setProperty("mama.resource_pool.test.bridges", mBridgeName);
        Mama.setProperty("mama.resource_pool.test.default_transport_sub", mTransportName);
        Mama.setProperty("mama.resource_pool.test.default_source_sub", "SOURCE");
        Mama.setProperty(String.format("mama.%s.transport.%s.param", mBridgeName, mTransportName), "value");
        mPool = new MamaResourcePool("test");
    }

    @After
    public void tearDown()
    {
        mPool.destroy();
    }

    @Test(expected = MamaException.class)
    public void testCreateNullParams() throws MamaException {
        new MamaResourcePool(null);
    }

    @Test
    public void createTransportFromName() {
        Mama.setLogLevel(Level.FINEST);
        MamaTransport transport = mPool.createTransportFromName(mTransportName);
        assertNotEquals(null, transport);
        assertEquals(mTransportName, transport.getName());
    }

    @Test(expected = MamaException.class)
    public void createTransportInvalidParameters() {
        mPool.createTransportFromName(null);
    }

    @Test
    public void createSubscriptionFromUri() {
        MamaSubscription subscription = mPool.createSubscriptionFromUri (
                "qpid://sub/SOURCE/test.topic", new SubscriptionEventHandler(), null);
        assertEquals (subscription.getSource().getSymbolNamespace(), "SOURCE");
        assertEquals (subscription.getSymbol(), "test.topic");
        assertEquals (subscription.getTransport().getName(), "sub");
    }

    @Test(expected = MamaException.class)
    public void createSubscriptionFromUriInvalidUriPlainString() {
        mPool.createSubscriptionFromUri("plainString", new SubscriptionEventHandler(), null);
    }

    @Test(expected = MamaException.class)
    public void createSubscriptionFromUriInvalidUriNullArg() {
        mPool.createSubscriptionFromUri(null, new SubscriptionEventHandler(), null);
    }

    @Test(expected = MamaException.class)
    public void createSubscriptionFromUriInvalidHandler() {
        mPool.createSubscriptionFromUri("qpid://sub/SOURCE/test.topic", null, null);
    }

    @Test
    public void createSubscriptionFromComponents() {
        MamaSubscription subscription = mPool.createSubscriptionFromComponents (
                "sub", "SOURCE", "test.topic", new SubscriptionEventHandler(), null);
        assertEquals (subscription.getSource().getSymbolNamespace(), "SOURCE");
        assertEquals (subscription.getSymbol(), "test.topic");
        assertEquals (subscription.getTransport().getName(), "sub");
    }

    @Test
    public void createSubscriptionFromTopicWithSource() {
        MamaSubscription subscription = mPool.createSubscriptionFromTopicWithSource (
                "SOURCE", "test.topic", new SubscriptionEventHandler(), null);
        assertEquals (subscription.getSource().getSymbolNamespace(), "SOURCE");
        assertEquals (subscription.getSymbol(), "test.topic");
        assertEquals (subscription.getTransport().getName(), "sub");
    }

    @Test
    public void createSubscriptionFromTopic() {
        MamaSubscription subscription = mPool.createSubscriptionFromTopic (
                "test.topic", new SubscriptionEventHandler(), null);
        assertEquals (subscription.getSource().getSymbolNamespace(), "SOURCE");
        assertEquals (subscription.getSymbol(), "test.topic");
        assertEquals (subscription.getTransport().getName(), "sub");
    }
}
