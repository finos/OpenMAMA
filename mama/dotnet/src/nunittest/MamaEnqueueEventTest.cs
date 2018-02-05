using Wombat;
using NUnit.Framework;
using System;
using System.Threading;

namespace NUnitTest
{
    #region Mama3444 Test Fixture

    [TestFixture]
    public class MamaEnqueueEventTest : MamaQueueEventCallback
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
        /// This thread will be used to pump events on the queue
        /// </summary>
        protected Thread m_dispatcher;

        /// <summary>
        /// This event will be fired whenever a queue event is processed.
        /// </summary>
        AutoResetEvent m_event;

        /// <summary>
        /// The number of events processed by the callback function.
        /// </summary>
        protected int m_numberEvents;

        /// <summary>
        /// The queue that will be used to perform the tests.
        /// </summary>
        protected MamaQueue m_queue;

        #endregion

        #region Setup and Teardown

        [SetUp]
        public void Setup()
        {
            MamaCommon.getCmdLineArgs();

            // Load the wmw bridge
            m_bridge = Mama.loadBridge(MamaCommon.middlewareName);

            // Create the mama queue
            m_queue = new MamaQueue(m_bridge);

            // Create the auto reset event
            m_event = new AutoResetEvent(false);

            // Spin a thread to dispatch events
            m_dispatcher = new Thread(new ThreadStart(this.DispatcherThread));

            // Reset all other member variables
            m_closure = null;
            m_numberEvents = 0;

            // Start the thread
            m_dispatcher.Start();
        }

        [TearDown]
        public void Teardown()
        {
            // Destroy the queue, this will cause the thread to quit out
            m_queue.destroy();

            // Wait for the thread to terminate
            m_dispatcher.Join();

            // Destroy the manual reset event
            m_event.Close();
        }

        #endregion

        #region Protected Functions

        protected void DispatcherThread()
        {
            // Start processing events on the queue
            m_queue.dispatch();
        }

        #endregion

        #region MamaQueueEventCallback Implementation

        /// <summary>
        /// Will process an enqueued event, it will simply increment the count of
        /// the number of events.
        /// </summary>
        /// <param name="mamaQueue">
        /// The queue.
        /// </param>
        /// <param name="closure">
        /// Closure object.
        /// </param>
        void MamaQueueEventCallback.onEvent (MamaQueue mamaQueue, object closure)
        {
            // Increment the event count
            Interlocked.Increment(ref m_numberEvents);

            // Save the closure
            Interlocked.Exchange(ref m_closure, closure);

            // Fire the event to indicate a queue event has been processed
            m_event.Set();
        }

        #endregion

        #region Tests

        /// <summary>
        /// This test will check that an exception is thrown if a null argument is passed
        /// </summary>
        [Test]
        [ExpectedException(typeof(ArgumentNullException))]
        public void NullParameterTest()
        {
            // Call the test function
            m_queue.enqueueEvent(null, null);

            // Ensure that no events have been procesed
            Assert.AreEqual(m_numberEvents, 0);
        }

        /// <summary>
        /// This function will test enqueuing a single event.
        /// </summary>
        [Test]
        public void EnqueueEvent()
        {
            // Call the test function
            m_queue.enqueueEvent(this as MamaQueueEventCallback, null);

            // Wait until the event has been fired
            m_event.WaitOne();

            // Ensure that only 1 event has been processed
            Assert.AreEqual(m_numberEvents, 1);
        }

        /// <summary>
        /// This function will test enqueuing a single event and will make sure that the
        /// closure is valid.
        /// </summary>
        [Test]
        public void EnqueueEventWithClosure()
        {
            // Create a closure
            Int32 intClosure = new Int32();
            intClosure = 69;

            // Call the test function passing a
            m_queue.enqueueEvent(this as MamaQueueEventCallback, intClosure);

            // Wait until the event has been fired
            m_event.WaitOne();

            // Ensure that only 1 event has been processed
            Assert.AreEqual(m_numberEvents, 1);

            // Verify that the closure is correct
            Assert.AreEqual((Int32)m_closure, intClosure);
        }

        #endregion
    }

    #endregion
}
