using NUnit.Framework;
using System;
using Wombat;

namespace NUnitTest
{
    [TestFixture]
    public class MamaDisableLoggingTest : MamaBaseLogTest
    {
        /* ****************************************************** */
        #region Tests

        [Test]
        public void DisableLevels()
        {
            // Enumerate all the available log levels
            foreach (MamaLogLevel level in Enum.GetValues(typeof(MamaLogLevel)))
            {
                // Turn the logging on and set it to this level
                Mama.enableLogging(level);

                // Disable logging
                Mama.disableLogging();

                // Write a log message
                Mama.log(level, "This is a test");

                // Verify that no log has been written
                if (m_callback.Buffer != null)
                {
                    throw new InvalidOperationException("A log has been written when logging is disabled.");
                }
            }
        }

        [Test]
        public void DoubleDisable()
        {
            // Disable logging twice, (there should be no exception)
            Mama.disableLogging();
            Mama.disableLogging();

            // Write a log message
            Mama.log(MamaLogLevel.MAMA_LOG_LEVEL_NORMAL, "This is a test");

            // Verify that no log has been written
            if (m_callback.Buffer != null)
            {
                throw new InvalidOperationException("A log has been written when logging is disabled.");
            }
        }

        #endregion
    }
}
