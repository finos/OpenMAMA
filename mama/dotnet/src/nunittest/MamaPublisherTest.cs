using Wombat;
using NUnit.Framework;
using System;
using System.Threading;

namespace NUnitTest
{
    #region Mama3444 Test Fixture

    [TestFixture]
    public class MamaPublisherTest : MamaPublisherCallback, MamaStartBackgroundCallback
    {
        #region Protected Member Variables

        /// <summary>
        /// The main bridge.
        /// </summary>
        protected MamaBridge m_bridge;

        /// <summary>
        /// The closure object obtained by the MamaQueueEventCallback.
        /// </summary>
        protected object m_closure;

        /// <summary>
        /// The queue that will be used to perform the tests.
        /// </summary>
        protected MamaQueueGroup m_queueGroup;

        protected MamaTransport m_transport;

        protected MamaMsg m_msg;

        protected int createCount;
        protected int errorCount;
        protected int destroyCount;

        #endregion

        public void onStartComplete(MamaStatus.mamaStatus status)
        {
        }

        [SetUp]
        public void Setup()
        {
            // Set the logging to this level
            Mama.enableLogging(MamaLogLevel.MAMA_LOG_LEVEL_FINEST);

            MamaCommon.getCmdLineArgs();

            createCount = 0;
            errorCount = 0;
            destroyCount = 0;

            // Load the wmw bridge
            m_bridge = Mama.loadBridge(MamaCommon.middlewareName);

            Mama.open();

            Mama.log(MamaLogLevel.MAMA_LOG_LEVEL_FINE, "MamaPublisherTest.Setup: transport=" +
                MamaCommon.middlewareName + " tport=" + MamaCommon.transportName +
                " source=" + MamaCommon.source + " symbol=" + MamaCommon.symbol);

            // Create the mama queue
            m_queueGroup = new MamaQueueGroup(m_bridge, 1);

            m_msg = new MamaMsg();

            m_transport = new MamaTransport();
            m_transport.create(MamaCommon.transportName, m_bridge);

            Mama.startBackground(m_bridge, this);

            Thread.Sleep(1000);

            GC.KeepAlive(m_bridge);
            GC.KeepAlive(m_transport);
            GC.KeepAlive(m_queueGroup);
            GC.KeepAlive(m_msg);
        }

        [TearDown]
        public void Teardown()
        {
            Thread.Sleep(2000);

            m_transport.destroy();

            m_queueGroup.destroy();

            m_msg.destroy();

            Thread.Sleep(1000);
            Mama.stop(m_bridge);

            Thread.Sleep(1000);
            Mama.close();
        }

        public void onCreate(MamaPublisher publisher)
        {
            createCount++;
            string subject = publisher.getSource() + "." + publisher.getSymbol();     // test getters
            Mama.log(MamaLogLevel.MAMA_LOG_LEVEL_FINE, "MamaPublisherTest.onCreate: " + subject);
        }

        public void onError(MamaPublisher publisher,
                     MamaStatus.mamaStatus status,
                     string topic)
        {
            errorCount++;
            string subject = publisher.getSource() + "." + publisher.getSymbol();     // test getters
            Mama.log(MamaLogLevel.MAMA_LOG_LEVEL_FINE, "MamaPublisherTest.onError: " + subject + " status=" + ((int) status));
        }

        public void onDestroy(MamaPublisher publisher)
        {
            // Cannot use publisher object here, its native handle had been destroyed
            destroyCount++;
            Mama.log(MamaLogLevel.MAMA_LOG_LEVEL_FINE, "MamaPublisherTest.onDestroy: ");
        }
    
        #endregion

        #region Tests

        [Test]
        public void PublishTest()
        {
            int numPublishes = 10;

            for (int i = 0; i < numPublishes; ++i)
            {
                MamaPublisher pub = new MamaPublisher();
                pub.create(m_transport, MamaCommon.symbol, MamaCommon.source);
                string topic = pub.getSource() + "." + pub.getSymbol();     // test getters

                m_msg.clear();
                m_msg.addU8("", (ushort)MamaReservedFields.MsgType.getFid(), (byte)mamaMsgType.MAMA_MSG_TYPE_INITIAL);
                m_msg.addU8("", (ushort)MamaReservedFields.MsgStatus.getFid(), (byte)mamaMsgStatus.MAMA_MSG_STATUS_OK);
                m_msg.addString("", 11, "TEST STRING");    // MdFeedName

                Mama.log(MamaLogLevel.MAMA_LOG_LEVEL_FINE, "Sendx " + i);
                try
                {
                    pub.send(m_msg);
                }
                catch (Exception ex)
                {
                    Mama.log(MamaLogLevel.MAMA_LOG_LEVEL_FINE, "Exception " + ex.StackTrace);
                }
                Mama.log(MamaLogLevel.MAMA_LOG_LEVEL_FINE, "Pub destroy");
                pub.destroy();

                Thread.Sleep(100);
            }

            Assert.AreEqual(0, createCount, "create");
            Assert.AreEqual(0, destroyCount, "destroy");
            Assert.AreEqual(0, errorCount, "error");
        }

        [Test]
        public void PublishTestWithCallbacks()
        {
            int numPublishes = 10;

            for (int i = 0; i < numPublishes; ++i)
            {
                MamaPublisher pub = new MamaPublisher();
                pub.createWithCallbacks(m_transport, Mama.getDefaultEventQueue(m_bridge), this, null, MamaCommon.symbol, MamaCommon.source, null);
                string topic = pub.getSource() + "." + pub.getSymbol();     // test getters

                m_msg.clear();
                m_msg.addU8("", (ushort)MamaReservedFields.MsgType.getFid(), (byte)mamaMsgType.MAMA_MSG_TYPE_INITIAL);
                m_msg.addU8("", (ushort)MamaReservedFields.MsgStatus.getFid(), (byte)mamaMsgStatus.MAMA_MSG_STATUS_OK);
                m_msg.addString("", 11, "TEST STRING");    // MdFeedName

                Mama.log(MamaLogLevel.MAMA_LOG_LEVEL_FINE, "Send " + i);
                try
                {
                    pub.send(m_msg);
                }
                catch (Exception ex)
                {
                    Mama.log(MamaLogLevel.MAMA_LOG_LEVEL_FINE, "Exception " + ex.StackTrace);
                }
                pub.destroy();

                Thread.Sleep(100);
            }

            Assert.AreEqual(numPublishes, createCount, "create");
            Assert.AreEqual(numPublishes, destroyCount, "destroy");
            Assert.AreEqual(0, errorCount, "error");
        }

        [Test]
        public void PublishTestWithCallbacksErrors()
        {
            int numPublishes = 10;

            for (int i = 0; i < numPublishes; ++i)
            {
                MamaPublisher pub = new MamaPublisher();
                pub.createWithCallbacks(m_transport, Mama.getDefaultEventQueue(m_bridge), this, null, MamaCommon.symbol, "BADSRC", null);
                string topic = pub.getSource() + "." + pub.getSymbol();     // test getters

                m_msg.clear();
                m_msg.addU8("", (ushort)MamaReservedFields.MsgType.getFid(), (byte)mamaMsgType.MAMA_MSG_TYPE_INITIAL);
                m_msg.addU8("", (ushort)MamaReservedFields.MsgStatus.getFid(), (byte)mamaMsgStatus.MAMA_MSG_STATUS_OK);
                m_msg.addString("", 11, "TEST STRING");    // MdFeedName

                Mama.log(MamaLogLevel.MAMA_LOG_LEVEL_FINE, "Send " + i);
                try
                {
                    pub.send(m_msg);
                }
                catch (Exception ex)
                {
                    Mama.log(MamaLogLevel.MAMA_LOG_LEVEL_FINE, "Exception " + ex.StackTrace);
                }
                pub.destroy();

                Thread.Sleep(100);
            }

            Assert.AreEqual(numPublishes, createCount, "create");
            Assert.AreEqual(numPublishes, destroyCount, "destroy");
            Assert.AreEqual(numPublishes, errorCount, "error");
        }

        #endregion
    }
}
