using Wombat;
using NUnit.Framework;
using System;
using System.Threading;

namespace NUnitTest
{
    [TestFixture]
    public class MamaLogCallbackMessageTest
    {
        #region Nested Classes

        public class TestLogCallback : MamaLogFileCallback2
        {
            // Used to receive log messages
            public string m_buffer = "";

            public void onLog(MamaLogLevel level, string message)
            {
                /* Format the log as a string, do this separately so we can
                 * see the string in the debugger.
                 */
                m_buffer = message;

                /* Write it out. */
                Console.WriteLine(m_buffer);
            }

            public void onLogSizeExceeded()
            {
            }
        }

        #endregion

        #region Private Member Variables

        // This class is used to intercept log messages
        private TestLogCallback m_callback;

        #endregion

        #region Setup and Teardown

        [SetUp]
        public void Setup()
        {
            // Create the callback class
            m_callback = new TestLogCallback();
        }

        [TearDown]
        public void Teardown()
        {
            // Reset member variables
            m_callback = null;
        }

        #endregion

        #region Tests

        [Test]        
        public void TestLogMessage()
        {
            // Set the log function
            Mama.setLogCallback(m_callback);

            // Write a log
            Mama.log(MamaLogLevel.MAMA_LOG_LEVEL_WARN, "This is a test");

            // Check to make sure that the message has been received
            Assert.AreEqual(m_callback.m_buffer, "This is a test");

            // Repeat
            Mama.log(MamaLogLevel.MAMA_LOG_LEVEL_WARN, "Hooray");

            // Check to make sure that the message has been received
            Assert.AreEqual(m_callback.m_buffer, "Hooray");
        }
        
        #endregion
    }
}
