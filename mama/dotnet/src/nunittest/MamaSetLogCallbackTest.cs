using NUnit.Framework;
using System;
using Wombat;

namespace NUnitTest
{
    [TestFixture]
    public class MamaSetLogCallbackTest : MamaBaseLogTest
    {
        /* ****************************************************** */
        #region Tests

        [Test]
        public void LogMessage()
        {
            // Set the log function
            Mama.setLogCallback(m_callback);

            // Write a log
            Mama.log(MamaLogLevel.MAMA_LOG_LEVEL_WARN, "This is a test");

            // Check to make sure that the message has been received
            Assert.AreEqual(m_callback.Buffer, "This is a test");

            // Repeat
            Mama.log(MamaLogLevel.MAMA_LOG_LEVEL_WARN, "Hooray");

            // Check to make sure that the message has been received
            Assert.AreEqual(m_callback.Buffer, "Hooray");
        }

        #endregion
    }
}
