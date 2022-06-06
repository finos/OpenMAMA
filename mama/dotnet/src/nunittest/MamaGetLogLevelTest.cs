using NUnit.Framework;
using System;
using Wombat;

namespace NUnitTest
{
    [TestFixture]
    public class MamaGetLogLevelTest : MamaBaseLogTest
    {
        /* ****************************************************** */
        #region Tests

        [Test]
        public void InvalidArguments()
        {
            // Create an invalid log level
            MamaLogLevel logLevel = (MamaLogLevel)19;
            
            Assert.Throws<ArgumentOutOfRangeException>(() => Mama.enableLogging(logLevel));
        }

        [Test]
        public void LogLevels()
        {
            // Enumerate all the available log levels
            foreach (MamaLogLevel level in Enum.GetValues(typeof(MamaLogLevel)))
            {
                // Set the logging to this level
                Mama.setLogLevel(level);

                // Read the log level back
                if (Mama.getLogLevel() != level)
                {
                    throw new InvalidOperationException("The correct log level was not obtained.");
                }
            }
        }

        [Test]
        public void Off()
        {
            // Disable logging
            Mama.disableLogging();

            // Verify that the log level is now off
            if (Mama.getLogLevel() != MamaLogLevel.MAMA_LOG_LEVEL_OFF)
            {
                throw new InvalidOperationException("Logging was not correctly turned off.");
            }
        }

        #endregion
    }
}
