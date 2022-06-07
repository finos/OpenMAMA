using NUnit.Framework;
using Wombat;

namespace NUnitTest
{
    public class MamaResourcePoolTest
    {
        private string mTransportName = "sub";
        private string mBridgeName;
        private MamaResourcePool mPool;

        class SubscriptionEventHandler : MamaSubscriptionCallback
        {
            public void onCreate(MamaSubscription subscription) {
            }

            public void onError(MamaSubscription subscription, MamaStatus.mamaStatus status, string subject) {
            }

            public void onMsg(MamaSubscription subscription, MamaMsg msg) {
            }

            public void onQuality(MamaSubscription subscription, mamaQuality quality, string subject) {
            }

            public void onRecapRequest(MamaSubscription subscription) {

            }

            public void onGap(MamaSubscription subscription) {

            }

            public void onDestroy(MamaSubscription subscription) {

            }
        }

        [SetUp]
        public void setUp()
        {
            mBridgeName = MamaCommon.middlewareName;
            Mama.open();
            Mama.setProperty("mama.resource_pool.test.bridges", mBridgeName);
            Mama.setProperty("mama.resource_pool.test.default_transport_sub", mTransportName);
            Mama.setProperty("mama.resource_pool.test.default_source_sub", "SOURCE");
            Mama.setProperty(string.Format("mama.{0}.transport.{1}.param", mBridgeName, mTransportName), "value");
            mPool = new MamaResourcePool("test");
        }

        [TearDown]
        public void tearDown()
        {
            mPool.destroy();
            Mama.close();
        }

        [Test]
        public void testCreateNullParams(){
            Assert.Throws<MamaException>(() => new MamaResourcePool(null));
        }

        [Test]
        public void createTransportFromName() {
            MamaTransport transport = mPool.createTransportFromName(mTransportName);
            Assert.NotNull(transport);
            Assert.AreEqual(mTransportName, transport.getName());
        }

        [Test]
        public void createTransportInvalidParameters() {
            Assert.Throws<MamaException>(() => mPool.createTransportFromName(null));
        }

        [Test]
        public void createSubscriptionFromUri() {
            MamaSubscription subscription = mPool.createSubscriptionFromUri (
                    "qpid://sub/SOURCE/test.topic", new SubscriptionEventHandler(), null);
            Assert.AreEqual (subscription.subscSource, "SOURCE");
            Assert.AreEqual (subscription.subscSymbol, "test.topic");
        }

        [Test]
        public void createSubscriptionFromUriInvalidUriPlainString() {
            Assert.Throws<System.UriFormatException>(() => mPool.createSubscriptionFromUri("plainstring", new SubscriptionEventHandler(), null));
        }

        [Test]
        public void createSubscriptionFromUriInvalidUriNullArg() {
            Assert.Throws<MamaException>(() => mPool.createSubscriptionFromUri(null, new SubscriptionEventHandler(), null));
        }

        [Test]
        public void createSubscriptionFromUriInvalidHandler() {
            Assert.Throws<MamaException>(() => mPool.createSubscriptionFromUri("qpid://sub/SOURCE/test.topic", null, null));
        }

        [Test]
        public void createSubscriptionFromComponents() {
            MamaSubscription subscription = mPool.createSubscriptionFromComponents (
                    "sub", "SOURCE", "test.topic", new SubscriptionEventHandler(), null);
            Assert.AreEqual (subscription.subscSource, "SOURCE");
            Assert.AreEqual (subscription.subscSymbol, "test.topic");
        }

        [Test]
        public void createSubscriptionFromTopicWithSource() {
            MamaSubscription subscription = mPool.createSubscriptionFromTopicWithSource (
                    "SOURCE", "test.topic", new SubscriptionEventHandler(), null);
            Assert.AreEqual (subscription.subscSource, "SOURCE");
            Assert.AreEqual (subscription.subscSymbol, "test.topic");
        }

        [Test]
        public void createSubscriptionFromTopic() {
            MamaSubscription subscription = mPool.createSubscriptionFromTopic (
                    "test.topic", new SubscriptionEventHandler(), null);
            Assert.AreEqual (subscription.subscSource, "SOURCE");
            Assert.AreEqual (subscription.subscSymbol, "test.topic");
        }
    }
}